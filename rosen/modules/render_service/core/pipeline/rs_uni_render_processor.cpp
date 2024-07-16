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
#include "string_utils.h"
#include "surface_type.h"

#include "common/rs_optional_trace.h"
#include "platform/common/rs_log.h"
#include "pipeline/parallel_render/rs_sub_thread_manager.h"
#include "pipeline/round_corner_display/rs_rcd_surface_render_node.h"

namespace OHOS {
namespace Rosen {
RSUniRenderProcessor::RSUniRenderProcessor()
    : uniComposerAdapter_(std::make_unique<RSUniRenderComposerAdapter>())
{
}

RSUniRenderProcessor::~RSUniRenderProcessor() noexcept
{
}

bool RSUniRenderProcessor::Init(RSDisplayRenderNode& node, int32_t offsetX, int32_t offsetY, ScreenId mirroredId,
                                std::shared_ptr<RSBaseRenderEngine> renderEngine, bool isRenderThread)
{
    if (!RSProcessor::Init(node, offsetX, offsetY, mirroredId, renderEngine, isRenderThread)) {
        return false;
    }
    // In uni render mode, we can handle screen rotation in the rendering process,
    // so we do not need to handle rotation in composer adapter any more,
    // just pass the buffer to composer straightly.
    screenInfo_.rotation = ScreenRotation::ROTATION_0;
    isPhone_ = RSMainThread::Instance()->GetDeviceType() == DeviceType::PHONE;
    return uniComposerAdapter_->Init(screenInfo_, offsetX_, offsetY_, mirrorAdaptiveCoefficient_);
}

void RSUniRenderProcessor::PostProcess()
{
    uniComposerAdapter_->CommitLayers(layers_);
    if (!isPhone_) {
        MultiLayersPerf(layerNum_);
    }
    RS_LOGD("RSUniRenderProcessor::PostProcess layers_:%{public}zu", layers_.size());
}

void RSUniRenderProcessor::CreateLayer(const RSSurfaceRenderNode& node, RSSurfaceRenderParams& params)
{
    auto buffer = params.GetBuffer();
    if (buffer == nullptr) {
        return;
    }
    auto& layerInfo = params.layerInfo_;
    RS_OPTIONAL_TRACE_NAME_FMT(
        "CreateLayer name:%s src:[%d, %d, %d, %d] dst:[%d, %d, %d, %d] buffer:[%d, %d] alpha:[%f]",
        node.GetName().c_str(), layerInfo.srcRect.x, layerInfo.srcRect.y, layerInfo.srcRect.w, layerInfo.srcRect.h,
        layerInfo.dstRect.x, layerInfo.dstRect.y, layerInfo.dstRect.w, layerInfo.dstRect.h,
        buffer->GetSurfaceBufferWidth(), buffer->GetSurfaceBufferHeight(), layerInfo.alpha);
    auto& preBuffer = params.GetPreBuffer();
    ScalingMode scalingMode = params.GetPreScalingMode();
    if (node.GetConsumer()->GetScalingMode(buffer->GetSeqNum(), scalingMode) == GSERROR_OK) {
        params.SetPreScalingMode(scalingMode);
    }
    LayerInfoPtr layer = GetLayerInfo(
        params, buffer, preBuffer, node.GetConsumer(), params.GetAcquireFence());
    if (layer != nullptr) {
        layer->SetDisplayNit(node.GetDisplayNit());
        layer->SetBrightnessRatio(node.GetBrightnessRatio());
    }

    uniComposerAdapter_->SetMetaDataInfoToLayer(layer, params.GetBuffer(), node.GetConsumer());
    layers_.emplace_back(layer);
    params.SetLayerCreated(true);
}

void RSUniRenderProcessor::CreateUIFirstLayer(DrawableV2::RSSurfaceRenderNodeDrawable& drawable,
    RSSurfaceRenderParams& params)
{
    auto buffer = drawable.GetBuffer();
    if (buffer == nullptr && drawable.GetAvailableBufferCount() <= 0) {
        RS_TRACE_NAME_FMT("HandleSubThreadNode wait %" PRIu64 "", params.GetId());
        RSSubThreadManager::Instance()->WaitNodeTask(params.GetId());
    }
    auto& surfaceHandler = static_cast<RSSurfaceHandler&>(drawable);
    if (!RSBaseRenderUtil::ConsumeAndUpdateBuffer(surfaceHandler, true) || !drawable.GetBuffer()) {
        RS_LOGE("CreateUIFirstLayer ConsumeAndUpdateBuffer or GetBuffer return  false");
        return;
    }
    buffer = drawable.GetBuffer();
    auto preBuffer = drawable.GetPreBuffer();
    LayerInfoPtr layer = GetLayerInfo(
        params, buffer, preBuffer.buffer, drawable.GetConsumer(), drawable.GetAcquireFence());
    uniComposerAdapter_->SetMetaDataInfoToLayer(layer, params.GetBuffer(), drawable.GetConsumer());
    layers_.emplace_back(layer);
    auto& layerInfo = params.layerInfo_;
    RS_LOGD("RSUniRenderProcessor::CreateUIFirstLayer: [%{public}s-%{public}" PRIu64 "] "
        "src: %{public}d %{public}d %{public}d %{public}d, "
        "dst: %{public}d %{public}d %{public}d %{public}d, zOrder: %{public}d",
        drawable.GetName().c_str(), drawable.GetNodeId(),
        layerInfo.srcRect.x, layerInfo.srcRect.y, layerInfo.srcRect.w, layerInfo.srcRect.h,
        layerInfo.dstRect.x, layerInfo.dstRect.y, layerInfo.dstRect.w, layerInfo.dstRect.h, layerInfo.zOrder);
}

LayerInfoPtr RSUniRenderProcessor::GetLayerInfo(RSSurfaceRenderParams& params, sptr<SurfaceBuffer>& buffer,
    sptr<SurfaceBuffer>& preBuffer, const sptr<IConsumerSurface>& consumer, const sptr<SyncFence>& acquireFence)
{
    LayerInfoPtr layer = HdiLayerInfo::CreateHdiLayerInfo();
    auto& layerInfo = params.layerInfo_;
    layer->SetSurface(consumer);
    layer->SetBuffer(buffer, acquireFence);
    layer->SetPreBuffer(preBuffer);
    preBuffer = nullptr;
    layer->SetZorder(layerInfo.zOrder);

    GraphicLayerAlpha alpha;
    alpha.enGlobalAlpha = true;
    // Alpha of 255 indicates opacity
    alpha.gAlpha = static_cast<uint8_t>(std::clamp(layerInfo.alpha, 0.0f, 1.0f) * RGBA_MAX);
    layer->SetAlpha(alpha);
    layer->SetLayerSize(layerInfo.dstRect);
    layer->SetBoundSize(layerInfo.boundRect);
    bool forceClient = RSSystemProperties::IsForceClient() ||
        (params.GetIsProtectedLayer() && params.GetAnimateState());
    layer->SetCompositionType(forceClient ? GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT :
        GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);

    std::vector<GraphicIRect> visibleRegions;
    visibleRegions.emplace_back(layerInfo.dstRect);
    layer->SetVisibleRegions(visibleRegions);
    std::vector<GraphicIRect> dirtyRegions;
    dirtyRegions.emplace_back(layerInfo.srcRect);
    layer->SetDirtyRegions(dirtyRegions);

    layer->SetBlendType(layerInfo.blendType);
    layer->SetCropRect(layerInfo.srcRect);
    layer->SetGravity(layerInfo.gravity);
    layer->SetTransform(layerInfo.transformType);
    auto matrix = GraphicMatrix {layerInfo.matrix.Get(Drawing::Matrix::Index::SCALE_X),
        layerInfo.matrix.Get(Drawing::Matrix::Index::SKEW_X), layerInfo.matrix.Get(Drawing::Matrix::Index::TRANS_X),
        layerInfo.matrix.Get(Drawing::Matrix::Index::SKEW_Y), layerInfo.matrix.Get(Drawing::Matrix::Index::SCALE_Y),
        layerInfo.matrix.Get(Drawing::Matrix::Index::TRANS_Y), layerInfo.matrix.Get(Drawing::Matrix::Index::PERSP_0),
        layerInfo.matrix.Get(Drawing::Matrix::Index::PERSP_1), layerInfo.matrix.Get(Drawing::Matrix::Index::PERSP_2)};
    layer->SetMatrix(matrix);
    layer->SetScalingMode(params.GetPreScalingMode());
    return layer;
}

void RSUniRenderProcessor::ProcessSurface(RSSurfaceRenderNode &node)
{
    auto layer = uniComposerAdapter_->CreateLayer(node);
    if (layer == nullptr) {
        RS_LOGE("RSUniRenderProcessor::ProcessSurface: failed to createLayer for node(id: %{public}" PRIu64 ")",
            node.GetId());
        return;
    }
    node.MarkCurrentFrameHardwareEnabled();
    layers_.emplace_back(layer);
}

void RSUniRenderProcessor::ProcessDisplaySurface(RSDisplayRenderNode& node)
{
    auto layer = uniComposerAdapter_->CreateLayer(node);
    if (layer == nullptr) {
        RS_LOGE("RSUniRenderProcessor::ProcessDisplaySurface: failed to createLayer for node(id: %{public}" PRIu64 ")",
            node.GetId());
        return;
    }
    if (node.GetFingerprint()) {
        layer->SetLayerMaskInfo(HdiLayerInfo::LayerMask::LAYER_MASK_HBM_SYNC);
        RS_LOGD("RSUniRenderProcessor::ProcessDisplaySurface, set layer mask hbm sync");
    } else {
        layer->SetLayerMaskInfo(HdiLayerInfo::LayerMask::LAYER_MASK_NORMAL);
    }
    layers_.emplace_back(layer);
    layerNum_ = node.GetSurfaceCountForMultiLayersPerf();
    RSUniRenderThread::Instance().SetAcquireFence(node.GetAcquireFence());
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

std::vector<LayerInfoPtr> RSUniRenderProcessor::GetLayers() const
{
    return layers_;
}
} // namespace Rosen
} // namespace OHOS
