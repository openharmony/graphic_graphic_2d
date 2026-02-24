/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "rs_uni_render_processor.h"

#include <vector>

#include "hdi_layer.h"
#include "hdi_layer_info.h"
#include "rs_trace.h"
#include "rs_uni_render_util.h"
#include "string_utils.h"
#include "surface_type.h"

#include "common/rs_common_hook.h"
#include "common/rs_optional_trace.h"
#include "dirty_region/rs_gpu_dirty_collector.h"
#include "display_engine/rs_luminance_control.h"
#include "drawable/rs_screen_render_node_drawable.h"
#include "drawable/rs_surface_render_node_drawable.h"
#include "feature/anco_manager/rs_anco_manager.h"
#include "feature/dirty/rs_uni_dirty_compute_util.h"
#include "feature/round_corner_display/rs_rcd_surface_render_node.h"
#ifdef RS_ENABLE_TV_PQ_METADATA
#include "feature/tv_metadata/rs_tv_metadata_manager.h"
#endif
// hpae offline
#include "feature/hwc/hpae_offline/rs_hpae_offline_processor.h"
#include "feature/hwc/hpae_offline/rs_hpae_offline_util.h"
#include "info_collection/rs_layer_compose_collection.h"
#include "params/rs_screen_render_params.h"
#include "params/rs_surface_render_params.h"
#include "platform/common/rs_log.h"
#include "rs_render_composer_manager.h"
#include "rs_surface_layer.h"

namespace OHOS {
namespace Rosen {
constexpr std::chrono::milliseconds HPAE_OFFLINE_TIMEOUT{100};
RSUniRenderProcessor::RSUniRenderProcessor()
    : uniComposerAdapter_(std::make_unique<RSUniRenderComposerAdapter>())
{
}

RSUniRenderProcessor::~RSUniRenderProcessor() noexcept
{
}

bool RSUniRenderProcessor::Init(RSScreenRenderNode& node, int32_t offsetX, int32_t offsetY, ScreenId mirroredId,
                                std::shared_ptr<RSBaseRenderEngine> renderEngine)
{
    if (!RSProcessor::Init(node, offsetX, offsetY, mirroredId, renderEngine)) {
        return false;
    }
    // In uni render mode, we can handle screen rotation in the rendering process,
    // so we do not need to handle rotation in composer adapter any more,
    // just pass the buffer to composer straightly.
    screenInfo_.rotation = ScreenRotation::ROTATION_0;

    // Initialize composerClient
    composerClient_ = RSRenderComposerManager::GetInstance().CreateRSRenderComposerClient(screenInfo_.id);
    if (composerClient_ == nullptr) {
        return false;
    }
    return uniComposerAdapter_->Init(screenInfo_, offsetX_, offsetY_);
}

bool RSUniRenderProcessor::InitForRenderThread(DrawableV2::RSScreenRenderNodeDrawable& screenDrawable,
    std::shared_ptr<RSBaseRenderEngine> renderEngine)
{
    if (!RSProcessor::InitForRenderThread(screenDrawable, renderEngine)) {
        return false;
    }
    // In uni render mode, we can handle screen rotation in the rendering process,
    // so we do not need to handle rotation in composer adapter any more,
    // just pass the buffer to composer straightly.
    screenInfo_.rotation = ScreenRotation::ROTATION_0;

    // Initialize composerClient
    composerClient_ = RSRenderComposerManager::GetInstance().CreateRSRenderComposerClient(screenInfo_.id);
    if (composerClient_ == nullptr) {
        return false;
    }
    return uniComposerAdapter_->Init(screenInfo_, offsetX_, offsetY_);
}

bool RSUniRenderProcessor::UpdateMirrorInfo(DrawableV2::RSLogicalDisplayRenderNodeDrawable& displayDrawable)
{
    if (!RSProcessor::UpdateMirrorInfo(displayDrawable)) {
        return false;
    }
    return uniComposerAdapter_->UpdateMirrorInfo(mirrorAdaptiveCoefficient_);
}

void RSUniRenderProcessor::PostProcess()
{
    uniComposerAdapter_->CommitLayers();
    LayerComposeCollection::GetInstance().UpdateUniformOrOfflineComposeFrameNumberForDFX(layers_.size());
    RS_LOGD("RSUniRenderProcessor::PostProcess layers_:%{public}zu", layers_.size());
}

void RSUniRenderProcessor::CreateLayer(RSSurfaceRenderNode& node, RSSurfaceRenderParams& params,
    const std::shared_ptr<ProcessOfflineResult>& offlineResult)
{
    auto surfaceHandler = node.GetRSSurfaceHandler();
    auto buffer = offlineResult ? offlineResult->buffer : surfaceHandler->GetBuffer();
    auto consumer = offlineResult ? offlineResult->consumer : surfaceHandler->GetConsumer();
    if (buffer == nullptr || consumer == nullptr) {
        return;
    }
    auto& layerInfo = params.GetLayerInfo();
    const Rect& dirtyRect = offlineResult ? offlineResult->damageRect : params.GetBufferDamage();
    auto preBuffer = offlineResult ? offlineResult->preBuffer : params.GetPreBuffer();
    auto acquireFence = offlineResult ? offlineResult->acquireFence : params.GetAcquireFence();
    RSLayerPtr layer = GetLayerInfo(
        params, buffer, preBuffer, consumer, acquireFence, offlineResult);
    if (layer == nullptr) {
        return;
    }
    layer->SetSdrNit(params.GetSdrNit());
    layer->SetDisplayNit(params.GetDisplayNit());
    layer->SetBrightnessRatio(params.GetBrightnessRatio());
    layer->SetLayerLinearMatrix(params.GetLayerLinearMatrix());
#ifdef RS_ENABLE_TV_PQ_METADATA
    RSTvMetadataManager::Instance().UpdateTvMetadata(params, buffer);
#endif
    uniComposerAdapter_->SetMetaDataInfoToLayer(layer, params.GetBuffer(), consumer);
    CreateSolidColorLayer(layer, params);
    auto& layerRect = layer->GetLayerSize();
    auto& cropRect = layer->GetCropRect();
    RS_OPTIONAL_TRACE_NAME_FMT(
        "CreateLayer name:%s ScreenId:%llu zorder:%d layerRect:[%d, %d, %d, %d] cropRect:[%d, %d, %d, %d]"
        "dirty:[%d, %d, %d, %d] buffer:[%d, %d] alpha:[%f] type:[%d] transform:[%d]",
        offlineResult ? "DeviceOfflineLayer" : node.GetName().c_str(), screenInfo_.id, layerInfo.zOrder,
        layerRect.x, layerRect.y, layerRect.w, layerRect.h,
        cropRect.x, cropRect.y, cropRect.w, cropRect.h,
        dirtyRect.x, dirtyRect.y, dirtyRect.w, dirtyRect.h, buffer->GetSurfaceBufferWidth(),
        buffer->GetSurfaceBufferHeight(), layerInfo.alpha, layerInfo.layerType, layer->GetTransformType());
    RS_LOGD_IF(DEBUG_PIPELINE,
        "CreateLayer name:%{public}s ScreenId:%{public}" PRIu64 " zorder:%{public}d layerRect:[%{public}d, %{public}d, "
        "%{public}d, %{public}d] cropRect:[%{public}d, %{public}d, %{public}d, %{public}d] "
        "drity:[%{public}d, %{public}d, %{public}d, %{public}d] "
        "buffer:[%{public}d, %{public}d] alpha:[%{public}f] transform:[%{public}d]",
        offlineResult ? "DeviceOfflineLayer" : node.GetName().c_str(), screenInfo_.id, layerInfo.zOrder,
        layerRect.x, layerRect.y, layerRect.w, layerRect.h,
        cropRect.x, cropRect.y, cropRect.w, cropRect.h,
        dirtyRect.x, dirtyRect.y, dirtyRect.w, dirtyRect.h, buffer->GetSurfaceBufferWidth(),
        buffer->GetSurfaceBufferHeight(), layerInfo.alpha, layer->GetTransformType());
    layers_.emplace_back(layer);
    params.SetLayerCreated(true);
}

void RSUniRenderProcessor::CreateLayerForRenderThread(DrawableV2::RSSurfaceRenderNodeDrawable& surfaceDrawable,
    const std::shared_ptr<ProcessOfflineResult>& offlineResult)
{
    auto& paramsSp = surfaceDrawable.GetRenderParams();
    if (!paramsSp) {
        return;
    }
    auto& params = *(static_cast<RSSurfaceRenderParams*>(paramsSp.get()));
    auto buffer = offlineResult ? offlineResult->buffer : params.GetBuffer();
    if (buffer == nullptr) {
        return;
    }
    auto& layerInfo = params.GetLayerInfo();
    const Rect& dirtyRect = offlineResult ? offlineResult->damageRect : params.GetBufferDamage();
    const auto& srcRect = offlineResult ? offlineResult->bufferRect : layerInfo.srcRect;
    auto preBuffer = offlineResult ? offlineResult->preBuffer : params.GetPreBuffer();
    auto acquireFence = offlineResult ? offlineResult->acquireFence : params.GetAcquireFence();
    auto consumer = offlineResult ? offlineResult->consumer : surfaceDrawable.GetConsumerOnDraw();
    RSLayerPtr layer = GetLayerInfo(static_cast<RSSurfaceRenderParams&>(params), buffer, preBuffer,
        consumer, acquireFence, offlineResult);
    if (layer == nullptr) {
        return;
    }
    layer->SetNodeId(surfaceDrawable.GetId());  // node id only for dfx
    auto& renderParams = static_cast<RSSurfaceRenderParams&>(params);
    layer->SetSdrNit(renderParams.GetSdrNit());
    layer->SetDisplayNit(renderParams.GetDisplayNit());
    layer->SetBrightnessRatio(renderParams.GetBrightnessRatio());
    layer->SetLayerLinearMatrix(renderParams.GetLayerLinearMatrix());
    RS_OPTIONAL_TRACE_NAME_FMT(
        "CreateLayer name:%s zorder:%d src:[%d, %d, %d, %d] dst:[%d, %d, %d, %d] adjustDst[%d %d %d %d] "
        "dirty:[%d, %d, %d, %d] buffer:[%d, %d] alpha:[%f] type:[%d] rog[w:%f h:%f]",
        offlineResult ? "DeviceOfflineLayer" : surfaceDrawable.GetName().c_str(), layerInfo.zOrder,
        srcRect.x, srcRect.y, srcRect.w, srcRect.h,
        layerInfo.dstRect.x, layerInfo.dstRect.y, layerInfo.dstRect.w, layerInfo.dstRect.h,
        layer->GetLayerSize().x, layer->GetLayerSize().y, layer->GetLayerSize().w, layer->GetLayerSize().h,
        dirtyRect.x, dirtyRect.y, dirtyRect.w, dirtyRect.h,
        buffer->GetSurfaceBufferWidth(), buffer->GetSurfaceBufferHeight(), layerInfo.alpha, layerInfo.layerType,
        uniComposerAdapter_->GetScreenInfo().GetRogWidthRatio(),
        uniComposerAdapter_->GetScreenInfo().GetRogHeightRatio());
    RS_LOGD("CreateLayer name:%{public}s zorder:%{public}d src:[%{public}d, %{public}d, %{public}d, %{public}d] "
            "dst:[%{public}d, %{public}d, %{public}d, %{public}d] "
            "drity:[%{public}d, %{public}d, %{public}d, %{public}d] "
            "buffer:[%{public}d, %{public}d] alpha:[%{public}f] type:%{public}d]",
        offlineResult ? "DeviceOfflineLayer" : surfaceDrawable.GetName().c_str(), layerInfo.zOrder,
        srcRect.x, srcRect.y, srcRect.w, srcRect.h,
        layerInfo.dstRect.x, layerInfo.dstRect.y, layerInfo.dstRect.w, layerInfo.dstRect.h,
        dirtyRect.x, dirtyRect.y, dirtyRect.w, dirtyRect.h,
        buffer->GetSurfaceBufferWidth(), buffer->GetSurfaceBufferHeight(), layerInfo.alpha, layerInfo.layerType);
#ifdef RS_ENABLE_TV_PQ_METADATA
    RSTvMetadataManager::Instance().UpdateTvMetadata(params, buffer);
#endif
    uniComposerAdapter_->SetMetaDataInfoToLayer(layer, params.GetBuffer(), consumer);
    CreateSolidColorLayer(layer, params);
    layers_.emplace_back(layer);
    params.SetLayerCreated(true);
}

void RSUniRenderProcessor::CreateSolidColorLayer(RSLayerPtr layer, RSSurfaceRenderParams& params)
{
    // color can be APP Node color or XCOM color
    auto color = params.GetSolidLayerColor();
    if (!params.GetIsHwcEnabledBySolidLayer()) {
        return;
    }
    auto solidColorLayer = RSSurfaceLayer::CreateLayer(composerClient_);
    if (solidColorLayer == nullptr) {
        RS_LOGE("RSUniRenderProcessor::CreateSolidColorLayer failed to create layer");
        return;
    }
    solidColorLayer->SetNodeId(params.GetId());
    solidColorLayer->CopyLayerInfo(layer);
    if (layer->GetZorder() > 0) {
        solidColorLayer->SetZorder(layer->GetZorder() - 1);
    }
    solidColorLayer->SetTransform(GraphicTransformType::GRAPHIC_ROTATE_NONE);
    auto dstRect = params.layerInfo_.dstRect;
    auto rogWidthRatio = uniComposerAdapter_->GetScreenInfo().GetRogWidthRatio();
    auto rogHeightRatio = uniComposerAdapter_->GetScreenInfo().GetRogHeightRatio();
    GraphicIRect layerRect = { static_cast<int32_t>(std::floor(dstRect.x * rogWidthRatio)),
        static_cast<int32_t>(std::floor(dstRect.y * rogHeightRatio)),
        static_cast<int32_t>(std::ceil(dstRect.w * rogWidthRatio)),
        static_cast<int32_t>(std::ceil(dstRect.h * rogHeightRatio)) };
    RS_OPTIONAL_TRACE_NAME_FMT("CreateSolidColorLayer name:%s id:%" PRIu64 " dst:[%d, %d, %d, %d] "
        "adjustDst:[%d, %d, %d, %d] color:%08x",
        params.GetName().c_str(), params.GetId(), dstRect.x, dstRect.y, dstRect.w, dstRect.h,
        layerRect.x, layerRect.y, layerRect.w, layerRect.h, color.AsArgbInt());
    solidColorLayer->SetLayerSize(layerRect);
    solidColorLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_SOLID_COLOR);
    solidColorLayer->SetLayerColor({color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha()});
    solidColorLayer->SetSurface({});
    solidColorLayer->SetBuffer({}, {});
    solidColorLayer->SetPreBuffer({});
    solidColorLayer->SetMetaData({});
    layers_.emplace_back(solidColorLayer);
}

bool RSUniRenderProcessor::GetForceClientForDRM(RSSurfaceRenderParams& params)
{
    if (params.GetSpecialLayerMgr().Find(SpecialLayerType::PROTECTED) == false) {
        return false;
    }
    if (params.GetAnimateState() == true ||
        RSUniRenderUtil::GetRotationDegreeFromMatrix(params.GetTotalMatrix()) % RS_ROTATION_90 != 0) {
        return true;
    }
    if (!params.GetCornerRadiusInfoForDRM().empty()) {
        return true;
    }
    bool forceClientForDRM = false;
    auto ancestorDisplayDrawable =
        std::static_pointer_cast<DrawableV2::RSScreenRenderNodeDrawable>(params.GetAncestorScreenDrawable().lock());
    auto& uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    if (ancestorDisplayDrawable == nullptr || ancestorDisplayDrawable->GetRenderParams() == nullptr ||
        uniParam == nullptr) {
        RS_LOGE("%{public}s ancestorDisplayDrawable/ancestorDisplayDrawableParams/uniParam is nullptr", __func__);
        return false;
    } else {
        auto screenParams = static_cast<RSScreenRenderParams*>(ancestorDisplayDrawable->GetRenderParams().get());
        forceClientForDRM = screenParams->IsRotationChanged() || uniParam->GetCacheEnabledForRotation();
    }
    return forceClientForDRM;
}

RSLayerPtr RSUniRenderProcessor::GetLayerInfo(RSSurfaceRenderParams& params, sptr<SurfaceBuffer>& buffer,
    sptr<SurfaceBuffer>& preBuffer, const sptr<IConsumerSurface>& consumer, const sptr<SyncFence>& acquireFence,
    const std::shared_ptr<ProcessOfflineResult>& offlineResult)
{
    RSLayerPtr layer = RSSurfaceLayer::CreateLayer(composerClient_);
    if (layer == nullptr) {
        RS_LOGE("RSUniRenderProcessor::GetLayerInfo failed to create layer");
        return nullptr;
    }
    layer->SetNodeId(params.GetId());  // node id only for dfx
    auto layerInfo = params.layerInfo_;
    if (params.GetHwcGlobalPositionEnabled()) {
        // dst and matrix transform to screen position
        layerInfo.matrix.PostTranslate(-offsetX_, -offsetY_);
        layerInfo.dstRect.x -= offsetX_;
        layerInfo.dstRect.y -= offsetY_;
    }
    layer->SetNeedBilinearInterpolation(params.NeedBilinearInterpolation());
    layer->SetSurface(consumer);
    layer->SetBuffer(buffer, acquireFence);
    layer->SetPreBuffer(preBuffer);
    if (!offlineResult) {
        // while using hpae_offline, prebuffer should not be consumed by dss
        params.SetPreBuffer(nullptr);
    }
    layer->SetZorder(layerInfo.zOrder);
    if (params.GetTunnelLayerId()) {
        RS_TRACE_NAME_FMT("%s lpp set tunnel layer type", __func__);
        layerInfo.layerType = GraphicLayerType::GRAPHIC_LAYER_TYPE_TUNNEL;
    }
    layer->SetType(layerInfo.layerType);
    layer->SetRotationFixed(params.GetFixRotationByUser());

    GraphicLayerAlpha alpha;
    alpha.enGlobalAlpha = true;
    // Alpha of 255 indicates opacity
    alpha.gAlpha = static_cast<uint8_t>(std::clamp(layerInfo.alpha, 0.0f, 1.0f) * RGBA_MAX);
    layer->SetAlpha(alpha);
    GraphicIRect dstRect = layerInfo.dstRect;
    if (layerInfo.layerType != GraphicLayerType::GRAPHIC_LAYER_TYPE_CURSOR) {
        auto rogWidthRatio = uniComposerAdapter_->GetScreenInfo().GetRogWidthRatio();
        auto rogHeightRatio = uniComposerAdapter_->GetScreenInfo().GetRogHeightRatio();
        dstRect = { static_cast<int32_t>(std::floor(layerInfo.dstRect.x * rogWidthRatio)),
            static_cast<int32_t>(std::floor(layerInfo.dstRect.y * rogHeightRatio)),
            static_cast<int32_t>(std::ceil(layerInfo.dstRect.w * rogWidthRatio)),
            static_cast<int32_t>(std::ceil(layerInfo.dstRect.h * rogHeightRatio)) };
    }
    if (layerInfo.layerType == GraphicLayerType::GRAPHIC_LAYER_TYPE_CURSOR &&
        ((layerInfo.dstRect.w != layerInfo.srcRect.w) || (layerInfo.dstRect.h != layerInfo.srcRect.h))) {
        dstRect = {layerInfo.dstRect.x, layerInfo.dstRect.y, layerInfo.srcRect.w, layerInfo.srcRect.h};
    }
    layer->SetLayerSize(dstRect);
    layer->SetBoundSize(layerInfo.boundRect);
    bool forceClientForDRM = GetForceClientForDRM(params);
    RS_OPTIONAL_TRACE_NAME_FMT("%s nodeName[%s] forceClientForDRM[%d]",
        __func__, params.GetName().c_str(), forceClientForDRM);
    RS_LOGD("%{public}s nodeName[%{public}s] forceClientForDRM[%{public}d]",
        __func__, params.GetName().c_str(), forceClientForDRM);
    bool forceClient = RSSystemProperties::IsForceClient() || forceClientForDRM;
    layer->SetCompositionType(forceClient ? GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT :
        GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    layer->SetCornerRadiusInfoForDRM(params.GetCornerRadiusInfoForDRM());
    auto bufferBackgroundColor = params.GetBackgroundColor();
    GraphicLayerColor backgroundColor = {
        .r = bufferBackgroundColor.GetRed(),
        .g = bufferBackgroundColor.GetGreen(),
        .b = bufferBackgroundColor.GetBlue(),
        .a = bufferBackgroundColor.GetAlpha()
    };
    layer->SetBackgroundColor(backgroundColor);

    std::vector<GraphicIRect> visibleRegions;
    visibleRegions.emplace_back(dstRect);
    layer->SetVisibleRegions(visibleRegions);
    std::vector<GraphicIRect> dirtyRegions;
    if (RSSystemProperties::GetHwcDirtyRegionEnabled()) {
        if (offlineResult) {
            // hpae offline
            GraphicIRect dirtyRect = GraphicIRect { offlineResult->damageRect.x, offlineResult->damageRect.y,
                offlineResult->damageRect.w, offlineResult->damageRect.h };
            auto intersectRect = RSUniDirtyComputeUtil::IntersectRect(offlineResult->bufferRect, dirtyRect);
            RS_OPTIONAL_TRACE_NAME_FMT("intersectRect:[%d, %d, %d, %d]",
                intersectRect.x, intersectRect.y, intersectRect.w, intersectRect.h);
            dirtyRegions.emplace_back(intersectRect);
        } else {
            const auto& bufferDamage = params.GetBufferDamage();
            Rect selfDrawingDirtyRect = bufferDamage;
            // When the size of the damage region equals that of the buffer, use dirty region from gpu crc
            bool isUseSelfDrawingDirtyRegion = buffer != nullptr && buffer->GetSurfaceBufferWidth() == bufferDamage.w &&
                buffer->GetSurfaceBufferHeight() == bufferDamage.h && bufferDamage.x == 0 && bufferDamage.y == 0;
            bool isSelfDrawingDirtyRegionValid = false;
            if (isUseSelfDrawingDirtyRegion) {
                isSelfDrawingDirtyRegionValid = RSGpuDirtyCollector::DirtyRegionCompute(buffer, selfDrawingDirtyRect);
            }
            if (isSelfDrawingDirtyRegionValid) {
                RS_OPTIONAL_TRACE_NAME_FMT("selfDrawingDirtyRect:[%d, %d, %d, %d]",
                    selfDrawingDirtyRect.x, selfDrawingDirtyRect.y, selfDrawingDirtyRect.w, selfDrawingDirtyRect.h);
            }
            bool isTargetedHwcDirtyRegion = params.GetIsBufferFlushed() ||
                RsCommonHook::Instance().GetHardwareEnabledByHwcnodeBelowSelfInAppFlag();
            GraphicIRect dirtyRect = isTargetedHwcDirtyRegion ? GraphicIRect { selfDrawingDirtyRect.x,
                selfDrawingDirtyRect.y, selfDrawingDirtyRect.w, selfDrawingDirtyRect.h } : GraphicIRect { 0, 0, 0, 0 };
            auto intersectRect = RSUniDirtyComputeUtil::IntersectRect(layerInfo.srcRect, dirtyRect);
            RS_OPTIONAL_TRACE_NAME_FMT("intersectRect:[%d, %d, %d, %d]",
                intersectRect.x, intersectRect.y, intersectRect.w, intersectRect.h);
            dirtyRegions.emplace_back(intersectRect);
        }
    } else {
        dirtyRegions.emplace_back(layerInfo.srcRect);
    }
    layer->SetDirtyRegions(dirtyRegions);

    layer->SetBlendType(layerInfo.blendType);
    layer->SetAncoFlags(layerInfo.ancoFlags);
    layer->SetAncoSrcRect(layerInfo.ancoCropRect);
    RSAncoManager::UpdateLayerSrcRectForAnco(layerInfo.ancoFlags, layerInfo.ancoCropRect, layerInfo.srcRect);
    layer->SetGravity(layerInfo.gravity);
    if (offlineResult) {
        layer->SetCropRect(offlineResult->bufferRect);
        layer->SetTransform(GraphicTransformType::GRAPHIC_ROTATE_NONE);
        layer->SetUseDeviceOffline(true);
    } else {
        layer->SetCropRect(layerInfo.srcRect);
        layer->SetTransform(layerInfo.transformType);
    }
    if (layerInfo.layerType == GraphicLayerType::GRAPHIC_LAYER_TYPE_CURSOR) {
        layer->SetTransform(GraphicTransformType::GRAPHIC_ROTATE_NONE);
        // Set the highest z-order for hardCursor
        layer->SetZorder(static_cast<int32_t>(TopLayerZOrder::POINTER_WINDOW));
    }
    auto matrix = GraphicMatrix {layerInfo.matrix.Get(Drawing::Matrix::Index::SCALE_X),
        layerInfo.matrix.Get(Drawing::Matrix::Index::SKEW_X), layerInfo.matrix.Get(Drawing::Matrix::Index::TRANS_X),
        layerInfo.matrix.Get(Drawing::Matrix::Index::SKEW_Y), layerInfo.matrix.Get(Drawing::Matrix::Index::SCALE_Y),
        layerInfo.matrix.Get(Drawing::Matrix::Index::TRANS_Y), layerInfo.matrix.Get(Drawing::Matrix::Index::PERSP_0),
        layerInfo.matrix.Get(Drawing::Matrix::Index::PERSP_1), layerInfo.matrix.Get(Drawing::Matrix::Index::PERSP_2)};
    layer->SetMatrix(matrix);
    layer->SetLayerSourceTuning(params.GetLayerSourceTuning());
    layer->SetLayerArsr(layerInfo.arsrTag);
    layer->SetLayerCopybit(layerInfo.copybitTag);
    layer->SetIgnoreAlpha(params.GetSurfaceBufferOpaque());
    HandleTunnelLayerParameters(params, layer);
    return layer;
}

bool RSUniRenderProcessor::ProcessOfflineLayer(
    std::shared_ptr<DrawableV2::RSSurfaceRenderNodeDrawable>& surfaceDrawable, bool async)
{
    uint64_t taskId = RSUniRenderThread::Instance().GetVsyncId();
    if (!async) {
        if (!RSHpaeOfflineProcessor::GetOfflineProcessor().PostProcessOfflineTask(surfaceDrawable, taskId)) {
            RS_LOGW("RSUniRenderProcessor::ProcessOfflineLayer: post offline task failed, go redraw");
            return false;
        }
    }
    std::shared_ptr<ProcessOfflineResult> processOfflineResult = std::make_shared<ProcessOfflineResult>();
    bool waitSuccess = RSHpaeOfflineProcessor::GetOfflineProcessor().WaitForProcessOfflineResult(
        taskId, HPAE_OFFLINE_TIMEOUT, *processOfflineResult);
    if (waitSuccess && processOfflineResult->taskSuccess) {
        CreateLayerForRenderThread(*surfaceDrawable, processOfflineResult);
        return true;
    } else {
        RS_LOGE("offline processor process failed!");
        return false;
    }
}

bool RSUniRenderProcessor::ProcessOfflineLayer(std::shared_ptr<RSSurfaceRenderNode>& node)
{
    RS_OFFLINE_LOGD("ProcessOfflineLayer(node)");
    uint64_t taskId = RSUniRenderThread::Instance().GetVsyncId();
    if (!RSHpaeOfflineProcessor::GetOfflineProcessor().PostProcessOfflineTask(node, taskId)) {
        RS_LOGW("RSUniRenderProcessor::ProcessOfflineLayer: post offline task failed, go redraw");
        return false;
    }
    std::shared_ptr<ProcessOfflineResult> processOfflineResult = std::make_shared<ProcessOfflineResult>();
    bool waitSuccess = RSHpaeOfflineProcessor::GetOfflineProcessor().WaitForProcessOfflineResult(
        taskId, HPAE_OFFLINE_TIMEOUT, *processOfflineResult);
    if (waitSuccess && processOfflineResult->taskSuccess) {
        auto params = static_cast<RSSurfaceRenderParams*>(node->GetStagingRenderParams().get());
        CreateLayer(*node, *params, processOfflineResult);
        return true;
    } else {
        RS_LOGE("offline processor process failed!");
        return false;
    }
}

void RSUniRenderProcessor::ProcessSurface(RSSurfaceRenderNode &node)
{
    RS_LOGE("It is update to DrawableV2 to process node now!!");
}

void RSUniRenderProcessor::ProcessScreenSurface(RSScreenRenderNode& node)
{
    auto layer = uniComposerAdapter_->CreateLayer(node);
    if (layer == nullptr) {
        RS_LOGE("RSUniRenderProcessor::ProcessScreenSurface: failed to createLayer for node(id: %{public}" PRIu64 ")",
            node.GetId());
        return;
    }
    if (node.GetFingerprint()) {
        layer->SetLayerMaskInfo(LayerMask::LAYER_MASK_HBM_SYNC);
        RS_LOGD("RSUniRenderProcessor::ProcessScreenSurface, set layer mask hbm sync");
    } else {
        layer->SetLayerMaskInfo(LayerMask::LAYER_MASK_NORMAL);
    }
    layers_.emplace_back(layer);
    auto drawable = node.GetRenderDrawable();
    if (!drawable) {
        return;
    }
    auto screenDrawable = std::static_pointer_cast<DrawableV2::RSScreenRenderNodeDrawable>(drawable);
    auto surfaceHandler = screenDrawable->GetRSSurfaceHandlerOnDraw();
    RSUniRenderThread::Instance().SetAcquireFence(surfaceHandler->GetAcquireFence());
}

void RSUniRenderProcessor::ProcessScreenSurfaceForRenderThread(
    DrawableV2::RSScreenRenderNodeDrawable& screenDrawable)
{
    auto layer = uniComposerAdapter_->CreateLayer(screenDrawable);
    if (layer == nullptr) {
        RS_LOGE("RSUniRenderProcessor::ProcessScreenSurface: failed to createLayer for node(id: %{public}" PRIu64 ")",
            screenDrawable.GetId());
        return;
    }
    auto& params = screenDrawable.GetRenderParams();
    if (!params) {
        return;
    }
    if (params->GetFingerprint()) {
        layer->SetLayerMaskInfo(LayerMask::LAYER_MASK_HBM_SYNC);
        RS_LOGD("RSUniRenderProcessor::ProcessScreenSurface, set layer mask hbm sync");
    } else {
        layer->SetLayerMaskInfo(LayerMask::LAYER_MASK_NORMAL);
    }
    layers_.emplace_back(layer);
    auto surfaceHandler = screenDrawable.GetRSSurfaceHandlerOnDraw();
    if (!surfaceHandler) {
        return;
    }
    RSUniRenderThread::Instance().SetAcquireFence(surfaceHandler->GetAcquireFence());
}

void RSUniRenderProcessor::ProcessRcdSurface(RSRcdSurfaceRenderNode& node)
{
    auto layer = uniComposerAdapter_->CreateLayer(node);
    if (layer == nullptr) {
        RS_LOGE("RSUniRenderProcessor::ProcessRcdSurface: failed to createLayer for node(id: %{public}" PRIu64 ")",
            node.GetId());
        return;
    }
    layers_.emplace_back(layer);
}

void RSUniRenderProcessor::HandleTunnelLayerParameters(RSSurfaceRenderParams& params, RSLayerPtr& layer)
{
    if (!layer || !params.GetTunnelLayerId()) {
        return;
    }
    layer->SetTunnelLayerId(params.GetTunnelLayerId());
    layer->SetTunnelLayerProperty(TunnelLayerProperty::TUNNEL_PROP_BUFFER_ADDR |
        TunnelLayerProperty::TUNNEL_PROP_DEVICE_COMMIT);
}

#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
std::vector<RSLayerPtr> RSUniRenderProcessor::GetLayers() const
{
    return layers_;
}
#endif
} // namespace Rosen
} // namespace OHOS
