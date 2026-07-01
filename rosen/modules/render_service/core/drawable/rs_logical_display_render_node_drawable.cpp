/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "drawable/rs_logical_display_render_node_drawable.h"

#include "common/rs_optional_trace.h"
#include "rs_trace.h"
#include "graphic_feature_param_manager.h"
#include "feature/dirty/rs_uni_dirty_compute_util.h"
#include "feature/drm/rs_drm_util.h"
#include "feature/hdr/rs_hdr_util.h"
#include "feature/multi_screen/rs_multi_screen_util.h"
#include "feature/special_layer/rs_special_layer_utils.h"
#include "feature/uifirst/rs_uifirst_manager.h"
#include "params/rs_logical_display_render_params.h"
#include "params/rs_screen_render_params.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "pipeline/render_thread/rs_uni_render_util.h"
#include "property/rs_point_light_manager.h"
#include "render/rs_pixel_map_util.h"
#include "render/rs_effect_luminance_manager.h"
#include "static_factory.h"

// dfx
#include "drawable/dfx/rs_refresh_rate_dfx.h"
#include "drawable/dfx/rs_dirty_rects_dfx.h"
#ifdef RS_PROFILER_ENABLED
#include "rs_profiler_capture_recorder.h"
#endif

#ifdef SUBTREE_PARALLEL_ENABLE
#include "rs_parallel_manager.h"
#endif

namespace OHOS::Rosen::DrawableV2 {

namespace {
constexpr int32_t MAX_DAMAGE_REGION_INFO = 300;

std::string RectVectorToString(const std::vector<RectI>& regionRects)
{
    std::string results = "";
    for (auto& rect : regionRects) {
        results += rect.ToString();
    }
    return results;
}

bool EnablescRGBForP3AndUiFirst(const GraphicColorGamut& currentGamut)
{
    return RSSystemParameters::IsNeedScRGBForP3(currentGamut) && RSUifirstManager::Instance().GetUiFirstSwitch();
}

void FinishHdrDraw(Drawing::Brush& paint, float hdrBrightnessRatio)
{
    RS_LOGD("HDR FinishHdrDraw");
    Drawing::Filter filter = paint.GetFilter();
    Drawing::ColorMatrix luminanceMatrix;
    luminanceMatrix.SetScale(hdrBrightnessRatio, hdrBrightnessRatio, hdrBrightnessRatio, 1.0f);
    auto luminanceColorFilter = std::make_shared<Drawing::ColorFilter>(Drawing::ColorFilter::FilterType::MATRIX,
        luminanceMatrix);
    filter.SetColorFilter(luminanceColorFilter);
    paint.SetFilter(filter);
}

}
RSLogicalDisplayRenderNodeDrawable::Registrar RSLogicalDisplayRenderNodeDrawable::instance_;
std::shared_ptr<Drawing::RuntimeEffect> RSLogicalDisplayRenderNodeDrawable::brightnessAdjustmentShaderEffect_ = nullptr;
constexpr int32_t CAPTURE_WINDOW = 2; // To be deleted after captureWindow being deleted
RSLogicalDisplayRenderNodeDrawable::RSLogicalDisplayRenderNodeDrawable(std::shared_ptr<const RSRenderNode>&& node)
    : RSRenderNodeDrawable(std::move(node))
{
}

RSLogicalDisplayRenderNodeDrawable::~RSLogicalDisplayRenderNodeDrawable()
{
    RS_LOGI("%{public}s, NodeId:[%{public}" PRIu64 "]", __func__, GetId());
}

RSRenderNodeDrawable::Ptr RSLogicalDisplayRenderNodeDrawable::OnGenerate(std::shared_ptr<const RSRenderNode> node)
{
    return new RSLogicalDisplayRenderNodeDrawable(std::move(node));
}

void RSLogicalDisplayRenderNodeDrawable::ClearCanvasStencil(RSPaintFilterCanvas& canvas,
    const RSLogicalDisplayRenderParams& params, const RSRenderThreadParams& uniParam,
    const RSScreenProperty& screenProperty)
{
    if (!uniParam.IsStencilPixelOcclusionCullingEnabled()) {
        return;
    }
    auto topSurfaceOpaqueRects = params.GetTopSurfaceOpaqueRects();
    if (topSurfaceOpaqueRects.empty()) {
        return;
    }
    RS_OPTIONAL_TRACE_NAME_FMT("ClearStencil, rect(0, 0, %u, %u), stencilVal: 0",
        screenProperty.GetWidth(), screenProperty.GetHeight());
    canvas.ClearStencil({0, 0, screenProperty.GetWidth(), screenProperty.GetHeight()}, 0);
    std::reverse(topSurfaceOpaqueRects.begin(), topSurfaceOpaqueRects.end());
    auto maxStencilVal = TOP_OCCLUSION_SURFACES_NUM * OCCLUSION_ENABLE_SCENE_NUM;
    canvas.SetMaxStencilVal(maxStencilVal);
    for (size_t i = 0; i < topSurfaceOpaqueRects.size(); i++) {
        Drawing::RectI rect {topSurfaceOpaqueRects[i].left_,
            topSurfaceOpaqueRects[i].top_,
            topSurfaceOpaqueRects[i].right_,
            topSurfaceOpaqueRects[i].bottom_};
        auto stencilVal = OCCLUSION_ENABLE_SCENE_NUM *
            (TOP_OCCLUSION_SURFACES_NUM - topSurfaceOpaqueRects.size() + i + 1);
        RS_OPTIONAL_TRACE_NAME_FMT("ClearStencil, rect(%" PRId32 ", %" PRId32 ", %" PRId32 ", %" PRId32 "), "
            "stencilVal: %zu", rect.GetLeft(), rect.GetTop(), rect.GetWidth(), rect.GetHeight(), stencilVal);
        canvas.ClearStencil(rect, static_cast<uint32_t>(stencilVal));
    }
}

void RSLogicalDisplayRenderNodeDrawable::OnDraw(Drawing::Canvas& canvas)
{
    RS_TRACE_NAME_FMT("RSLogicalDisplayRenderNodeDrawable:OnDraw %" PRIu64, GetId());
    SetDrawSkipType(DrawSkipType::NONE);
    if (!ShouldPaint()) {
        SetDrawSkipType(DrawSkipType::SHOULD_NOT_PAINT);
        return;
    }

    Drawing::GPUResourceTag::SetCurrentNodeId(GetId());
    curCanvas_ = reinterpret_cast<RSPaintFilterCanvas*>(&canvas);

    if (!curCanvas_) {
        SetDrawSkipType(DrawSkipType::CANVAS_NULL);
        RS_LOGE("RSLogicalDisplayRenderNodeDrawable::OnDraw curCanvas is nullptr!");
        return;
    }

    auto& uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    if (UNLIKELY(!uniParam)) {
        SetDrawSkipType(DrawSkipType::RENDER_THREAD_PARAMS_NULL);
        RS_LOGE("RSLogicalDisplayRenderNodeDrawable::OnDraw uniParam is nullptr!");
        return;
    }

    auto params = static_cast<RSLogicalDisplayRenderParams*>(GetRenderParams().get());
    if (!params) {
        SetDrawSkipType(DrawSkipType::RENDER_PARAMS_NULL);
        RS_LOGE("RSLogicalDisplayRenderNodeDrawable::OnDraw params is nullptr!");
        return;
    }
    RSAutoCanvasRestore acr(curCanvas_, RSPaintFilterCanvas::SaveType::kCanvasAndAlpha);

    auto [_, screenParams] = GetScreenParams(*params);
    if (!screenParams) {
        RS_LOGE("RSLogicalDisplayRenderNodeDrawable::OnDraw screenParams is nullptr!");
        return;
    }

    const auto& screenProperty = screenParams->GetScreenProperty();
    const auto& processor = uniParam->GetRSProcessor();
    auto mirroredDrawable = params->GetMirrorSourceDrawable().lock();
    ScreenId paramScreenId = params->GetScreenId();
    RSEffectLuminanceManager::GetInstance().SetCurrentScreenId(paramScreenId);
    RSEffectLuminanceManager::GetInstance().SetHdrPipelineStatus(screenParams->GetHDRPresent());
    auto mirroredRenderParams = mirroredDrawable ? mirroredDrawable->GetRenderParams().get() : nullptr;
    if (mirroredRenderParams) {
        RSMultiScreenUtil::HandleMirrorDisplay(*this, *params, processor);
        return;
    }
    if (params->GetCompositeType() == CompositeType::UNI_RENDER_EXPAND_COMPOSITE) {
        RSMultiScreenUtil::HandleVirtualExtendDisplay(*this, *params, processor);
        return;
    }
    RSMultiScreenUtil::DumpRenderStrategy(*this, params->GetScreenId(), RenderStrategy::DRAW_AS_MAIN_SCREEN, "");

    auto hdrBrightnessRatio = screenParams->GetHdrBrightnessRatio();
    bool isHdrOn = screenParams->GetHDRPresent();
    bool isScRGBEnable = EnablescRGBForP3AndUiFirst(screenParams->GetNewColorSpace());
    bool isOpDropped = uniParam->IsOpDropped();
    bool needOffscreen = params->GetNeedOffscreen() || screenProperty.GetIsSamplingOn() || isScRGBEnable;
    bool backToFP16 = true;
    if (isHdrOn && !needOffscreen) {
#ifdef ROSEN_OHOS
        backToFP16 = RSHdrUtil::NeedBackToFP16(GetId(), screenParams);
#endif
        needOffscreen = backToFP16;
    }

    if (params->GetNeedOffscreen()) {
        uniParam->SetOpDropped(false);
        curCanvas_->Clear(Drawing::Color::COLOR_BLACK);
    }

    if (needOffscreen) {
        scaleManager_ = uniParam->GetSLRScaleManager();
        UpdateSlrScale(screenProperty, params->GetFixedWidth(), params->GetFixedHeight(), screenParams);
        ScaleCanvasIfNeeded(screenProperty);
        auto rect = curCanvas_->GetDeviceClipBounds();
        if (screenProperty.GetIsSamplingOn() && scaleManager_ != nullptr) {
            screenParams->SetSlrMatrix(scaleManager_->GetScaleMatrix());
        }
        PrepareOffscreenRender(*this, !screenProperty.GetIsSamplingOn(), !screenProperty.GetIsSamplingOn());
#ifdef RS_PROFILER_ENABLED
        if (auto canvas =
                RSCaptureRecorder::GetInstance().TryInstantCapture(static_cast<float>(curCanvas_->GetWidth()),
                    static_cast<float>(curCanvas_->GetHeight()), SkpCaptureType::EXTENDED)) {
            curCanvas_->AddCanvas(canvas);
        }
#endif
        if (!params->GetNeedOffscreen() && !screenProperty.GetIsSamplingOn()) {
            curCanvas_->ClipRect(rect);
        } else {
            curCanvas_->Clear(Drawing::Color::COLOR_TRANSPARENT);
        }
    }
    if (isHdrOn && !backToFP16) {
        RS_LOGD("RSLogicalDisplayRenderNodeDrawable::OnDraw HDR SCALE %{public}f"
            " first used in drawOp.", screenParams->GetHdrBrightnessRatio());
        curCanvas_->GetSurface()->SetHdrScale(hdrBrightnessRatio);
        RSFilterCacheManager::SetScrHdr(hdrBrightnessRatio);
    } else {
        curCanvas_->GetSurface()->SetHdrScale(1.0f);
        RSFilterCacheManager::SetScrHdr(1.0f);
    }

    // prepare canvas
    if (!params->GetNeedOffscreen()) {
        params->ApplyAlphaAndMatrixToCanvas(*curCanvas_);
    }

    ClearCanvasStencil(*curCanvas_, *params, *uniParam, screenProperty);

    // canvas draw
#ifdef SUBTREE_PARALLEL_ENABLE
    if (!RSParallelManager::Singleton().OnDrawLogicDisplayNodeDrawable(this, curCanvas_)) {
        RSRenderNodeDrawable::OnDraw(*curCanvas_);
    }
#else
    RSRenderNodeDrawable::OnDraw(*curCanvas_);
#endif

    DrawAdditionalContent(*curCanvas_);
    curCanvas_->GetSurface()->SetHdrScale(1.0f);
    RSFilterCacheManager::SetScrHdr(1.0f);
    if (needOffscreen && canvasBackup_) {
        Drawing::AutoCanvasRestore acrBackUp(*canvasBackup_, true);
        if (params->GetNeedOffscreen()) {
            params->ApplyAlphaAndMatrixToCanvas(*canvasBackup_);
        }
        ClearTransparentBeforeSaveLayer();
#ifdef RS_PROFILER_ENABLED
        RSCaptureRecorder::GetInstance().EndInstantCapture(SkpCaptureType::EXTENDED);
#endif
        FinishOffscreenRender(Drawing::SamplingOptions(Drawing::FilterMode::NEAREST, Drawing::MipmapMode::NONE),
            screenProperty.GetIsSamplingOn(), hdrBrightnessRatio);
        uniParam->SetOpDropped(isOpDropped);
    }

    if (RotateOffScreenParam::GetRotateOffScreenScreenNodeEnable() && !params->IsRotationChanged()) {
        offscreenSurface_ = nullptr;
        offscreenCanvas_ = nullptr;
    }
}

void RSLogicalDisplayRenderNodeDrawable::OnCapture(Drawing::Canvas& canvas)
{
    if (!ShouldPaint()) {
        return;
    }
    const auto& params = static_cast<RSLogicalDisplayRenderParams*>(GetRenderParams().get());
    if (!params) {
        RS_LOGE("RSLogicalDisplayRenderNodeDrawable::OnCapture params is null!");
        return;
    }
    auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(&canvas);
    if (!paintFilterCanvas) {
        RS_LOGE("RSLogicalDisplayRenderNodeDrawable::OnCapture paintFilterCanvas is null!");
        return;
    }
    RSAutoCanvasRestore acr(paintFilterCanvas, RSPaintFilterCanvas::SaveType::kCanvasAndAlpha);
    auto screenDrawable = std::static_pointer_cast<RSScreenRenderNodeDrawable>(
        params->GetAncestorScreenDrawable().lock());
    if (!screenDrawable) {
        RS_LOGE("RSLogicalDisplayRenderNodeDrawable::OnCapture screenDrawable is null!");
        return;
    }
    auto screenParam = static_cast<RSScreenRenderParams*>(screenDrawable->GetRenderParams().get());
    if (!screenParam) {
        RS_LOGE("RSLogicalDisplayRenderNodeDrawable::OnCapture screenParams is null!");
        return;
    }
    sptr<SurfaceBuffer> virtualBuffer = nullptr;
    sptr<SyncFence> virtualFence = nullptr;
    const auto& screenProperty = screenParam->GetScreenProperty();
    bool virtualHasBuffer = false;
    auto surfaceConfigs = screenProperty.GetMultiSurfaceConfigs();
    if (!surfaceConfigs.empty()) {
        auto producerSurface = surfaceConfigs[0].surface;
        if (producerSurface) {
            float matrix[16];
            bool isUseNewMatrix = false;
            GSError sRet = producerSurface->GetLastFlushedBuffer(virtualBuffer, virtualFence, matrix, isUseNewMatrix);
            if (sRet == GSERROR_OK) {
                virtualHasBuffer = true;
                RS_TRACE_NAME_FMT("RSLogicalDisplayRenderNodeDrawable::OnCapture Using buffer from virtual screen");
            }
        }
    } else {
        RS_LOGD("RSLogicalDisplayRenderNodeDrawable::%{public}s: no surface configs for virtual screen",
            __func__);
    }
    bool noBuffer = (RSUniRenderThread::GetCaptureParam().isSnapshot_ &&
        screenDrawable->GetRSSurfaceHandlerOnDraw()->GetBuffer() == nullptr) && !virtualHasBuffer;
    if (noBuffer) {
        RS_LOGW("RSLogicalDisplayRenderNodeDrawable::OnCapture: buffer is null!");
    }

    auto specialLayerType = RSSpecialLayerUtils::GetSpecialLayerStateInSubTree(*params, screenParam);
    // Screenshot blackList, exclude surfaceNode in blackList while capturing displayNode
    auto currentBlackList = RSUniRenderThread::Instance().GetBlackList();
    if (specialLayerType != DisplaySpecialLayerState::NO_SPECIAL_LAYER || screenProperty.GetIsSamplingOn() ||
        UNLIKELY(RSUniRenderThread::GetCaptureParam().isMirror_) || screenDrawable->IsRenderSkipIfScreenOff() ||
        !currentBlackList.empty() || UNLIKELY(noBuffer)) {
        offsetX_ = screenProperty.GetOffsetX();
        offsetY_ = screenProperty.GetOffsetY();
        RS_LOGD("RSLogicalDisplayRenderNodeDrawable::OnCapture: \
            process RSLogicalDisplayRenderNode(id:[%{public}" PRIu64 "]) Not using UniRender buffer.",
            params->GetId());
        RS_TRACE_NAME_FMT("RSLogicalDisplayRenderNode::%s screenId: [%" PRIu64 "]"
            " Not using UniRender buffer. specialLayer: %d, noBuffer: %d, "
            "isSamplingOn: %d, isRenderSkipIfScreenOff: %d, blackList: %zu, "
            "offsetX: %d, offsetY: %d", __func__, params->GetScreenId(),
            specialLayerType != DisplaySpecialLayerState::NO_SPECIAL_LAYER, noBuffer, screenProperty.GetIsSamplingOn(),
            screenDrawable->IsRenderSkipIfScreenOff(), currentBlackList.size(), offsetX_, offsetY_);

        if (!UNLIKELY(RSUniRenderThread::GetCaptureParam().isMirror_)) {
            params->ApplyAlphaAndMatrixToCanvas(*paintFilterCanvas);
        }

        RSRenderNodeDrawable::OnCapture(canvas);
        // paintFilterCanvas is offScreen create Canvas
        DrawAdditionalContent(*paintFilterCanvas);
    } else {
        canvas.Clear(Drawing::Color::COLOR_BLACK);
        DrawHardwareEnabledNodes(canvas, *params, virtualBuffer, virtualFence);
    }
}

void RSLogicalDisplayRenderNodeDrawable::DrawHardwareEnabledNodes(Drawing::Canvas& canvas,
    RSLogicalDisplayRenderParams& params, sptr<SurfaceBuffer> virtualBuffer, sptr<SyncFence> virtualFence)
{
    auto rsCanvas = static_cast<RSPaintFilterCanvas*>(&canvas);
    if (!rsCanvas) {
        RS_LOGE("RSLogicalDisplayRenderNodeDrawable::DrawHardwareEnabledNodes rscanvas is null!");
        return;
    }
    auto screenDrawable = std::static_pointer_cast<RSScreenRenderNodeDrawable>(
        params.GetAncestorScreenDrawable().lock());
    auto screenParams = static_cast<RSScreenRenderParams*>(screenDrawable->GetRenderParams().get());

    std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr> hwcNodes;
    std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr> hwcTopNodes;
    RSUniRenderUtil::CollectHardwareEnabledNodesByDisplayNodeId(hwcNodes, hwcTopNodes, params.GetId());

    RS_LOGI("RSLogicalDisplayRenderNodeDrawable::DrawHardwareEnabledNodes: \
        process RSScreenRenderNode(id:[%{public}" PRIu64 "]) \
        Using UniRender buffer with hwcNodes(%{public}zu), hwcTopNodes(%{public}zu).",
        GetId(), hwcNodes.size(), hwcTopNodes.size());
    RS_TRACE_NAME_FMT("Process ScreenRenderNodeDrawable[%{public}" PRIu64 "] \
        using UniRender buffer with hwcNodes(%{public}zu), hwcTopNodes(%{public}zu).",
        params.GetScreenId(), hwcNodes.size(), hwcTopNodes.size());

    RSUniRenderUtil::AdjustZOrderAndDrawSurfaceNode(hwcNodes, canvas, *screenParams);

    auto renderEngine = RSUniRenderThread::Instance().GetRenderEngine();
    auto drawParams = (virtualBuffer && virtualFence)
        ? RSUniRenderUtil::CreateBufferDrawParam(virtualBuffer, virtualFence, false)
        : RSUniRenderUtil::CreateBufferDrawParam(*screenDrawable->GetRSSurfaceHandlerOnDraw(), false);
    RSBaseRenderUtil::WriteSurfaceBufferToPng(drawParams.buffer);
    renderEngine->DrawScreenNodeWithParams(*rsCanvas, *screenDrawable->GetRSSurfaceHandlerOnDraw(), drawParams);
    RSUniRenderUtil::AdjustZOrderAndDrawSurfaceNode(hwcTopNodes, canvas, *screenParams);
}

void RSLogicalDisplayRenderNodeDrawable::DrawAdditionalContent(RSPaintFilterCanvas& canvas)
{
    RS_TRACE_FUNC();
    DrawWatermarkIfNeed(canvas);
    RSRefreshRateDfx(*this).OnDraw(canvas);
}

void RSLogicalDisplayRenderNodeDrawable::DrawWatermarkIfNeed(RSPaintFilterCanvas& canvas,
    const Drawing::Rect& drawRegion)
{
    if (!RSUniRenderThread::Instance().GetWatermarkFlag()) {
        return;
    }
    const auto& params = static_cast<RSLogicalDisplayRenderParams*>(GetRenderParams().get());
    if (UNLIKELY(!params)) {
        RS_LOGE("RSLogicalDisplayRenderNodeDrawable::DrawWatermarkIfNeed params is null");
        return;
    }
    auto [_, screenParams] = GetScreenParams(*params);
    if (!screenParams) {
        return;
    }
    auto image = RSUniRenderThread::Instance().GetWatermarkImg();
    if (UNLIKELY(image == nullptr)) {
        RS_LOGE("RSLogicalDisplayRenderNodeDrawable::DrawWatermarkIfNeed image is null");
        return;
    }
    RS_TRACE_FUNC();
    auto srcRect = Drawing::Rect(0, 0, image->GetWidth(), image->GetHeight());
    if (drawRegion.IsValid()) {
        canvas.DrawImageRect(*image, srcRect, drawRegion, Drawing::SamplingOptions(),
            Drawing::SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT);
        return;
    }

    RSAutoCanvasRestore acr(&canvas);
    Drawing::Matrix invertMatrix;
    if (params->GetMatrix().Invert(invertMatrix)) {
        canvas.ConcatMatrix(invertMatrix);
    }

    auto rotation = params->GetScreenRotation();
    auto screenCorrection = screenParams->GetScreenProperty().GetScreenCorrection();
    auto mainWidth = params->GetFixedWidth();
    auto mainHeight = params->GetFixedHeight();

    if (screenCorrection != ScreenRotation::INVALID_SCREEN_ROTATION && screenCorrection != ScreenRotation::ROTATION_0) {
        // Recalculate rotation if mirrored screen has additional rotation angle
        rotation = static_cast<ScreenRotation>((static_cast<int>(rotation) + SCREEN_ROTATION_NUM -
            static_cast<int>(screenCorrection)) % SCREEN_ROTATION_NUM);
    }

    if (rotation == ScreenRotation::ROTATION_90 || rotation == ScreenRotation::ROTATION_270) {
        std::swap(mainWidth, mainHeight);
    }
    if (rotation != ScreenRotation::ROTATION_0) {
        if (rotation == ScreenRotation::ROTATION_90) {
            canvas.Rotate(-(RS_ROTATION_90), 0, 0); // 90 degree
            canvas.Translate(-(static_cast<float>(mainWidth)), 0);
        } else if (rotation == ScreenRotation::ROTATION_180) {
            canvas.Rotate(-(RS_ROTATION_180), static_cast<float>(mainWidth) / 2, // 2 half of screen width
                static_cast<float>(mainHeight) / 2); // 2 half of screen height
        } else if (rotation == ScreenRotation::ROTATION_270) {
            canvas.Rotate(-(RS_ROTATION_270), 0, 0); // 270 degree
            canvas.Translate(0, -(static_cast<float>(mainHeight)));
        }
    }

    auto dstRect = Drawing::Rect(0, 0, mainWidth, mainHeight);
    Drawing::Brush rectBrush;
    canvas.AttachBrush(rectBrush);
    RS_TRACE_NAME("RSLogicalDisplayRenderNodeDrawable::DrawWatermarkIfNeed DrawImageRect");
    canvas.DrawImageRect(*image, srcRect, dstRect, Drawing::SamplingOptions(),
        Drawing::SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT);
    canvas.DetachBrush();
}

void RSLogicalDisplayRenderNodeDrawable::UpdateDisplayDirtyManager(std::shared_ptr<RSDirtyRegionManager> dirtyManager,
    int32_t bufferage, bool useAlignedDirtyRegion)
{
    dirtyManager->SetBufferAge(bufferage);
    dirtyManager->UpdateDirty(useAlignedDirtyRegion);
}

std::vector<RectI> RSLogicalDisplayRenderNodeDrawable::CalculateVirtualDirty(
    std::shared_ptr<RSUniRenderVirtualProcessor> virtualProcesser, RSScreenRenderNodeDrawable& curScreenDrawable,
    RSLogicalDisplayRenderParams& params, Drawing::Matrix canvasMatrix)
{
    // uniParam/drawable/mirroredParams not null in caller
    Occlusion::Region mappedDamageRegion;
    auto& uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    auto curScreenParams = static_cast<RSScreenRenderParams*>(curScreenDrawable.GetRenderParams().get());
    auto drawable = curScreenParams->GetMirrorSourceDrawable().lock();
    auto mirrorDisplayDrawable = params.GetMirrorSourceDrawable().lock();
    if (!drawable || !mirrorDisplayDrawable) {
        RS_LOGE("%{public}s mirroredDrawable or mirrorDisplayDrawable nullptr", __func__);
        virtualProcesser->SetRoiRegionToCodec(mappedDamageRegion.GetRegionRectIs());
        return mappedDamageRegion.GetRegionRectIs();
    }
    auto mirroredDrawable = std::static_pointer_cast<RSScreenRenderNodeDrawable>(drawable);
    auto mirrorParams = static_cast<RSScreenRenderParams*>(mirroredDrawable->GetRenderParams().get());
    auto mirrorDisplayParams =
        static_cast<RSLogicalDisplayRenderParams*>(mirrorDisplayDrawable->GetRenderParams().get());

    bool specialLayerChange = lastTypeBlackList_ != currentTypeBlackList_ || lastBlackList_ != currentBlackList_ ||
        (mirrorDisplayParams && mirrorDisplayParams->IsSpecialLayerChanged()) ||
        lastSecExemption_ != curSecExemption_ || (enableVisibleRect_ && (lastVisibleRect_ != curVisibleRect_ ||
        params.HasSecLayerInVisibleRectChanged()));
    bool needRefresh = !(lastCanvasMatrix_ == canvasMatrix) || !(lastMirrorMatrix_ == mirrorParams->GetMatrix()) ||
        uniParam->GetForceMirrorScreenDirty() || uniParam->GetVirtualDirtyRefresh() || virtualDirtyNeedRefresh_ ||
        curScreenParams->GetHasMirroredScreenChanged() || specialLayerChange ||
        !curScreenDrawable.GetSyncDirtyManager()->GetCurrentFrameDirtyRegion().IsEmpty() ||
        lastEnableVisibleRect_ != enableVisibleRect_ || !curScreenParams->IsEqualVsyncPeriod();
    if (needRefresh) {
        curScreenDrawable.GetSyncDirtyManager()->ResetDirtyAsSurfaceSize();
        virtualDirtyNeedRefresh_ = false;
        lastCanvasMatrix_ = canvasMatrix;
        lastMirrorMatrix_ = mirrorParams->GetMatrix();
    }
    std::string screenName = curScreenParams->GetScreenProperty().Name();
    if (!RSUniDirtyComputeUtil::CheckCurrentFrameHasDirtyInVirtual(curScreenDrawable) &&
        !curScreenDrawable.GetAccumulateDirtyInSkipFrame() && !needRefresh &&
        screenName.find("fromAnco") == std::string::npos) {
        virtualProcesser->SetDisplaySkipInMirror(true);
        return mappedDamageRegion.GetRegionRectIs();
    }
    curScreenDrawable.SetAccumulateDirtyInSkipFrame(false);
    const auto& mainScreenInfo = mirrorParams->GetScreenInfo();
    int32_t bufferAge = virtualProcesser->GetBufferAge();
    std::vector<RectI> damageRegionRects = RSUniRenderUtil::MergeDirtyHistoryInVirtual(
        *mirroredDrawable, bufferAge, mainScreenInfo, params.GetScreenId(), false);
    std::shared_ptr<RSObjAbsGeometry> tmpGeo = std::make_shared<RSObjAbsGeometry>();
    for (auto& rect : damageRegionRects) {
        RectI mappedRect = tmpGeo->MapRect(rect.ConvertTo<float>(), canvasMatrix);
        mappedDamageRegion.OrSelf(Occlusion::Region(Occlusion::Rect(mappedRect)));
    }

    RectI hwcRect = mirroredDrawable->GetSyncDirtyManager()->GetHwcDirtyRegion();
    const std::map<RSSurfaceNodeType, RectI>& typeHwcRectList =
        mirroredDrawable->GetSyncDirtyManager()->GetTypeHwcDirtyRegion();
    for (auto& typeHwcRect : typeHwcRectList) {
        NodeType nodeType = static_cast<NodeType>(typeHwcRect.first);
        if (currentTypeBlackList_.find(nodeType) == currentTypeBlackList_.end()) {
            hwcRect = hwcRect.JoinRect(typeHwcRect.second);
        }
    }
    if (!hwcRect.IsEmpty()) {
        if (mainScreenInfo.isSamplingOn && mainScreenInfo.samplingScale > 0) {
            Drawing::Matrix scaleMatrix;
            scaleMatrix.SetScaleTranslate(mainScreenInfo.samplingScale, mainScreenInfo.samplingScale,
                mainScreenInfo.samplingTranslateX, mainScreenInfo.samplingTranslateY);
            hwcRect = RSObjAbsGeometry::MapRect(hwcRect.ConvertTo<float>(), scaleMatrix);
            const Vector4<int> expandSize{mainScreenInfo.samplingDistance, mainScreenInfo.samplingDistance,
                mainScreenInfo.samplingDistance, mainScreenInfo.samplingDistance};
            hwcRect = hwcRect.MakeOutset(expandSize);
        }
        RectI mappedHwcRect = tmpGeo->MapRect(hwcRect.ConvertTo<float>(), canvasMatrix);
        curScreenDrawable.GetSyncDirtyManager()->MergeDirtyRect(mappedHwcRect);
    }
    UpdateDisplayDirtyManager(curScreenDrawable.GetSyncDirtyManager(), bufferAge, false);
    auto extraDirty = curScreenDrawable.GetSyncDirtyManager()->GetDirtyRegion();
    mappedDamageRegion.OrSelf(Occlusion::Region(Occlusion::Rect(extraDirty)));
    // Align all virtual mirror screen scenes.
    if (RSUniDirtyComputeUtil::IsDamageRegionGpuTileValid()) {
        RS_TRACE_NAME_FMT("%s, dirty align enabled with gpu tile(%d, %d)", __func__,
            RSUniDirtyComputeUtil::GetDamageRegionGpuTile().first,
            RSUniDirtyComputeUtil::GetDamageRegionGpuTile().second);
        mappedDamageRegion = mappedDamageRegion.GetAlignedRegion(
            RSUniDirtyComputeUtil::GetDamageRegionGpuTile().first,
            RSUniDirtyComputeUtil::GetDamageRegionGpuTile().second);
    }

    if (!virtualProcesser->GetDrawVirtualMirrorCopy()) {
        RSUniFilterDirtyComputeUtil::DealWithFilterDirtyRegion(
            mappedDamageRegion, mappedDamageRegion, *mirroredDrawable, canvasMatrix, true);
    }
    auto mappedDamageRegionRects = mappedDamageRegion.GetRegionRectIs();
    if (!uniParam->IsVirtualDirtyDfxEnabled()) {
        virtualProcesser->SetDirtyInfo(mappedDamageRegionRects);
        RS_TRACE_NAME_FMT("SetDamageRegion damageRegionrects num: %zu, info: %s",
            mappedDamageRegionRects.size(),
            RectVectorToString(mappedDamageRegionRects).substr(0, MAX_DAMAGE_REGION_INFO).c_str());
    }
    return mappedDamageRegionRects;
}

std::vector<RectI> RSLogicalDisplayRenderNodeDrawable::CalculateVirtualDirtyForWiredScreen(
    RSScreenRenderNodeDrawable& curScreenDrawable, Drawing::Matrix canvasMatrix)
{
    std::vector<RectI> damageRegionRects;
    auto curScreenParams = static_cast<RSScreenRenderParams*>(curScreenDrawable.GetRenderParams().get());
    auto mirroredDrawable =
        std::static_pointer_cast<RSScreenRenderNodeDrawable>(curScreenParams->GetMirrorSourceDrawable().lock());
    if (!mirroredDrawable || !mirroredDrawable->GetRenderParams()) {
        RS_LOGE("RSLogicalDisplayRenderNodeDrawable::CalculateVirtualDirtyForWiredScreen mirroredNode is null");
        return damageRegionRects;
    }
    const auto& uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    /*
     * If Top and Left in curVisibleRect_ are not zero,that means the regional screen mirror position may be offset,
     * and the dirty area location may be incorrect. Need to disable the dirty area.
     */
    if (uniParam == nullptr || !uniParam->IsVirtualDirtyEnabled() ||
        (enableVisibleRect_ && (curVisibleRect_.GetTop() > 0 || curVisibleRect_.GetLeft() > 0))) {
        RS_LOGE("RSLogicalDisplayRenderNodeDrawable::CalculateVirtualDirtyForWiredScreen invalid uniparam");
        return damageRegionRects;
    }

    if (!curScreenParams->IsEqualVsyncPeriod()) {
        RS_LOGD("RSLogicalDisplayRenderNodeDrawable::CalculateVirtualDirtyForWiredScreen frame rate is irregular");
        return damageRegionRects;
    }
    int32_t bufferAge = curScreenDrawable.GetBufferAge();
    const auto mirroredParams = static_cast<RSScreenRenderParams*>(mirroredDrawable->GetRenderParams().get());
    const auto& mainScreenInfo = mirroredParams->GetScreenInfo();
    std::shared_ptr<RSObjAbsGeometry> tmpGeo = std::make_shared<RSObjAbsGeometry>();
    // merge history dirty and map to mirrored wired screen by matrix
    auto tempDamageRegionRects = RSUniRenderUtil::MergeDirtyHistoryInVirtual(
        *mirroredDrawable, bufferAge, mainScreenInfo, true);
    for (auto& rect : tempDamageRegionRects) {
        RectI mappedRect = tmpGeo->MapRect(rect.ConvertTo<float>(), canvasMatrix);
        damageRegionRects.emplace_back(mappedRect);
    }

    auto syncDirtyManager = curScreenDrawable.GetSyncDirtyManager();
    // reset dirty rect as mirrored wired screen size when first time connection or matrix changed
    bool needRefresh = !(lastCanvasMatrix_ == canvasMatrix) || !(lastMirrorMatrix_ == mirroredParams->GetMatrix()) ||
        uniParam->GetForceMirrorScreenDirty() || lastEnableVisibleRect_ != enableVisibleRect_ ||
        (enableVisibleRect_ && lastVisibleRect_ != curVisibleRect_) ||
        lastBlackList_ != currentBlackList_ || uniParam->GetVirtualDirtyRefresh() || virtualDirtyNeedRefresh_;
    if (needRefresh) {
        curScreenDrawable.GetSyncDirtyManager()->ResetDirtyAsSurfaceSize();
        virtualDirtyNeedRefresh_ = false;
        lastCanvasMatrix_ = canvasMatrix;
        lastMirrorMatrix_ = mirroredParams->GetMatrix();
    }
    RectI hwcRect = mirroredDrawable->GetSyncDirtyManager()->GetHwcDirtyRegion();
    if (!hwcRect.IsEmpty()) {
        RectI mappedHwcRect = tmpGeo->MapRect(hwcRect.ConvertTo<float>(), canvasMatrix);
        syncDirtyManager->MergeDirtyRect(mappedHwcRect);
    }
    UpdateDisplayDirtyManager(syncDirtyManager, bufferAge, false);
    auto extraDirty = syncDirtyManager->GetDirtyRegion();
    if (!extraDirty.IsEmpty()) {
        damageRegionRects.emplace_back(extraDirty);
    }
    if (!uniParam->IsVirtualDirtyDfxEnabled()) {
        curScreenDrawable.SetDamageRegion(damageRegionRects);
        RS_TRACE_NAME_FMT("SetDamageRegion damageRegionrects num: %zu, info: %s",
            damageRegionRects.size(),
            RectVectorToString(damageRegionRects).substr(0, MAX_DAMAGE_REGION_INFO).c_str());
    }
    return damageRegionRects;
}

void RSLogicalDisplayRenderNodeDrawable::CheckDirtyRefresh(CompositeType type, bool hasSecLayerInVisibleRect)
{
    auto params = static_cast<RSLogicalDisplayRenderParams*>(GetRenderParams().get());
    if (!params) {
        return;
    }
    auto [mirroredDrawable, mirroredParams, _, __] = GetMirrorSourceParams(*params);
    if (!mirroredDrawable || !mirroredParams) {
        return;
    }
    auto& uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    if (uniParam == nullptr) {
        return;
    }
    auto hasSecSurface = mirroredParams->GetSpecialLayerMgr().Find(SpecialLayerType::HAS_SECURITY);
    if (type == CompositeType::UNI_RENDER_COMPOSITE) {
        if (hasSecSurface) {
            uniParam->SetVirtualDirtyRefresh(true);
        }
    } else if (type == CompositeType::UNI_RENDER_MIRROR_COMPOSITE) {
        if ((((!enableVisibleRect_ && hasSecSurface) || (enableVisibleRect_ && hasSecLayerInVisibleRect)) &&
            !uniParam->GetSecExemption()) || params->GetVirtualScreenMuteStatus()) {
            uniParam->SetVirtualDirtyRefresh(true);
        }
    }
}

void RSLogicalDisplayRenderNodeDrawable::DrawCurtainScreen() const
{
    if (!RSUniRenderThread::Instance().IsCurtainScreenOn()) {
        return;
    }
    RS_TRACE_FUNC();
    // curCanvas_ cannot be nullptr in caller
    curCanvas_->Clear(Drawing::Color::COLOR_BLACK);
}

void RSLogicalDisplayRenderNodeDrawable::DrawSecurityMask()
{
    RS_TRACE_FUNC();
    RSAutoCanvasRestore acr(curCanvas_, RSPaintFilterCanvas::SaveType::kCanvasAndAlpha);
    
    auto params = static_cast<RSLogicalDisplayRenderParams*>(GetRenderParams().get());
    auto [_, screenParams] = GetScreenParams(*params);
    if (!screenParams) {
        return;
    }

    auto imagePtr = screenParams->GetScreenProperty().GetSecurityMask();
    // Set canvas to black if imagePtr is null
    auto image = RSPixelMapUtil::ExtractDrawingImage(imagePtr);
    if (!image || image->GetWidth() == 0 || image->GetHeight() == 0) {
        RS_LOGE("RSLogicalDisplayRenderNodeDrawable::%{public}s, image is invalid, SetCanvasBlack", __func__);
        RS_TRACE_NAME_FMT("%s, image is invalid, SetCanvasBlack", __func__);
        curCanvas_->Clear(Drawing::Color::COLOR_BLACK);
        curCanvas_->SetDisableFilterCache(false);
        return;
    }

    float realImageWidth = static_cast<float>(image->GetWidth());
    float realImageHeight = static_cast<float>(image->GetHeight());
    curCanvas_->Clear(Drawing::Color::COLOR_TRANSPARENT);
    auto srcRect = Drawing::Rect(0, 0, image->GetWidth(), image->GetHeight());
    float screenWidth = static_cast<float>(screenParams->GetScreenProperty().GetWidth());
    float screenHeight = static_cast<float>(screenParams->GetScreenProperty().GetHeight());
    // Area to be drawn in the actual image
    auto dstRect = RSUniRenderUtil::GetImageRegions(screenWidth, screenHeight, realImageWidth, realImageHeight);
    // Make sure the canvas is oriented accurately.
    curCanvas_->ResetMatrix();

    Drawing::SamplingOptions samplingOptions(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    Drawing::Brush brush;
    curCanvas_->AttachBrush(brush);
    curCanvas_->DrawImageRect(*image, srcRect, dstRect, samplingOptions,
        Drawing::SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT);
    DrawWatermarkIfNeed(*curCanvas_, dstRect);
    curCanvas_->DetachBrush();

    RS_LOGI("RSLogicalDisplayRenderNodeDrawable::%{public}s", __func__);
    curCanvas_->SetDisableFilterCache(false);
}

void RSLogicalDisplayRenderNodeDrawable::UpdateSlrScale(const RSScreenProperty& screenProperty,
    float srcWidth, float srcHeight, RSScreenRenderParams* params)
{
    if (screenProperty.GetIsSamplingOn() && RSSystemProperties::GetSLRScaleEnabled()) {
        if (scaleManager_== nullptr) {
            scaleManager_ = std::make_shared<RSSLRScaleFunction>(screenProperty.GetPhyWidth(),
                screenProperty.GetPhyHeight(), srcWidth, srcHeight);
        } else {
            scaleManager_->CheckOrRefreshScreen(screenProperty.GetPhyWidth(),
                screenProperty.GetPhyHeight(), srcWidth, srcHeight);
        }
        if (params != nullptr) {
            scaleManager_->CheckOrRefreshColorSpace(params->GetNewColorSpace());
        }
    } else {
        scaleManager_ = nullptr;
    }
}

void RSLogicalDisplayRenderNodeDrawable::ScaleCanvasIfNeeded(const RSScreenProperty& screenProperty)
{
    if (!screenProperty.GetIsSamplingOn()) {
        scaleManager_ = nullptr;
        return;
    }
    if (RSSystemProperties::GetSLRScaleEnabled() && scaleManager_ != nullptr) {
        scaleManager_->CanvasScale(*curCanvas_);
        return;
    }
    scaleManager_ = nullptr;
    if (enableVisibleRect_) {
        // save canvas matrix to calculate visible clip rect
        visibleClipRectMatrix_ = curCanvas_->GetTotalMatrix();
    }
    curCanvas_->Translate(screenProperty.GetSamplingTranslateX(), screenProperty.GetSamplingTranslateY());
    curCanvas_->Scale(screenProperty.GetSamplingScale(), screenProperty.GetSamplingScale());
}

void RSLogicalDisplayRenderNodeDrawable::UpdateMainSizeFromContentRect(
    RSLogicalDisplayRenderParams* params, float& mainWidth, float& mainHeight)
{
    const Rect& contentRect = params->GetDisplayContentRect();
    if (contentRect.w > 0 && contentRect.h > 0) {
        mainWidth = static_cast<float>(contentRect.w);
        mainHeight = static_cast<float>(contentRect.h);
    }
}

void RSLogicalDisplayRenderNodeDrawable::ScaleAndRotateMirrorForWiredScreen(
    RSLogicalDisplayRenderNodeDrawable& mirroredDrawable)
{
    auto mirroredParams = static_cast<RSLogicalDisplayRenderParams*>(mirroredDrawable.GetRenderParams().get());
    auto params = static_cast<RSLogicalDisplayRenderParams*>(GetRenderParams().get());
    if (!mirroredParams || !params) {
        RS_LOGE("RSLogicalDisplayRenderNodeDrawable::ScaleAndRotateMirrorForWiredScreen mirroredParams is null");
        return;
    }
    auto [_, mirroredScreenParams] = GetScreenParams(*mirroredParams);
    auto [__, screenParam] = GetScreenParams(*params);
    if (!mirroredScreenParams || !screenParam) {
        RS_LOGE("RSLogicalDisplayRenderNodeDrawable::ScaleAndRotateMirrorForWiredScreen nodeParams is null");
        return;
    }

    float mainWidth = enableVisibleRect_ ? curVisibleRect_.GetWidth() : mirroredParams->GetFixedWidth();
    float mainHeight = enableVisibleRect_ ? curVisibleRect_.GetHeight() : mirroredParams->GetFixedHeight();
    UpdateMainSizeFromContentRect(mirroredParams, mainWidth, mainHeight);

    const auto& screenProperty = screenParam->GetScreenProperty();
    float mirrorWidth = screenProperty.GetPhyWidth();
    float mirrorHeight = screenProperty.GetPhyHeight();

    auto mirrorSourceRotation = params->GetMirrorSourceRotation();
    auto rotation = mirrorSourceRotation != ScreenRotation::INVALID_SCREEN_ROTATION ?
        mirrorSourceRotation : mirroredParams->GetScreenRotation();
    RS_TRACE_NAME_FMT("%s Source[%" PRIu64 "](%f, %f) -> mirrorScreen[%" PRIu64 "](%f, %f), rotation[%d]", __func__,
        mirroredParams->GetScreenId(), mainWidth, mainHeight,
        params->GetScreenId(), mirrorWidth, mirrorHeight, static_cast<int>(rotation));
    auto screenCorrection = mirroredScreenParams->GetScreenProperty().GetScreenCorrection();
    if (screenCorrection != ScreenRotation::INVALID_SCREEN_ROTATION && screenCorrection != ScreenRotation::ROTATION_0) {
        // Recalculate rotation if mirrored screen has additional rotation angle
        rotation = static_cast<ScreenRotation>((static_cast<int>(rotation) + SCREEN_ROTATION_NUM
            - static_cast<int>(screenCorrection)) % SCREEN_ROTATION_NUM);
    }
    // Rotate
    RotateMirrorCanvas(rotation, mirrorWidth, mirrorHeight);
    if (rotation == ScreenRotation::ROTATION_90 || rotation == ScreenRotation::ROTATION_270) {
        std::swap(mirrorWidth, mirrorHeight);
    }
    curCanvas_->Clear(SK_ColorBLACK);
    // must after rotate and swap width/height
    if (isMirrorSLRCopy_) {
        if (scaleManager_ == nullptr) {
            scaleManager_ = std::make_unique<RSSLRScaleFunction>(mirrorWidth, mirrorHeight, mainWidth, mainHeight);
        } else {
            scaleManager_->CheckOrRefreshScreen(mirrorWidth, mirrorHeight, mainWidth, mainHeight);
        }
        scaleManager_->CheckOrRefreshColorSpace(mirroredScreenParams->GetNewColorSpace());
        isMirrorSLRCopy_ = scaleManager_->GetIsSLRCopy();
    }
    // Scale
    if (mainWidth > 0 && mainHeight > 0) {
        if (isMirrorSLRCopy_) {
            scaleManager_->CanvasScale(*curCanvas_);
        } else {
            auto scaleNum = std::min(mirrorWidth / mainWidth, mirrorHeight / mainHeight);
            curCanvas_->Translate((mirrorWidth - scaleNum * mainWidth) * 0.5f,
                (mirrorHeight - scaleNum * mainHeight) * 0.5f);
            curCanvas_->Scale(scaleNum, scaleNum);
            curCanvas_->ClipRect(Drawing::Rect(0, 0, mainWidth, mainHeight), Drawing::ClipOp::INTERSECT, false);
        }
    }
}

void RSLogicalDisplayRenderNodeDrawable::RotateMirrorCanvas(ScreenRotation& rotation, float width, float height)
{
    switch (rotation) {
        case ScreenRotation::ROTATION_0:
            break;
        case ScreenRotation::ROTATION_90:
            curCanvas_->Translate(width / 2.0f, height / 2.0f);
            curCanvas_->Rotate(90, 0, 0); // 90 degrees
            curCanvas_->Translate(-(height / 2.0f), -(width / 2.0f));
            break;
        case ScreenRotation::ROTATION_180:
            // 180 is the rotate angle, calculate half width and half height requires divide by 2
            curCanvas_->Rotate(180, width / 2.0f, height / 2.0f);
            break;
        case ScreenRotation::ROTATION_270:
            curCanvas_->Translate(width / 2.0f, height / 2.0f);
            curCanvas_->Rotate(270, 0, 0); // 270 degrees
            curCanvas_->Translate(-(height / 2.0f), -(width / 2.0f));
            break;
        default:
            break;
    }
}

void RSLogicalDisplayRenderNodeDrawable::ClearTransparentBeforeSaveLayer()
{
    if (!canvasBackup_) {
        return;
    }
    RS_TRACE_NAME("ClearTransparentBeforeSaveLayer");
    auto& hardwareDrawables =
        RSUniRenderThread::Instance().GetRSRenderThreadParams()->GetHardwareEnabledTypeDrawables();
    if (UNLIKELY(!renderParams_)) {
        RS_LOGE("RSLogicalDisplayRenderNodeDrawable::ClearTransparentBeforeSaveLayer renderParams is null!");
        return;
    }
    auto params = static_cast<RSLogicalDisplayRenderParams*>(renderParams_.get());
    for (const auto& [_, displayNodeId, drawable] : hardwareDrawables) {
        auto surfaceDrawable = static_cast<RSSurfaceRenderNodeDrawable*>(drawable.get());
        if (!surfaceDrawable || displayNodeId != params->GetId()) {
            continue;
        }
        auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable->GetRenderParams().get());
        if (!surfaceParams || !surfaceParams->GetHardwareEnabled()) {
            continue;
        }
        Drawing::AutoCanvasRestore arc(*canvasBackup_, true);
        canvasBackup_->SetMatrix(surfaceParams->GetLayerInfo().matrix);
        canvasBackup_->ClipRect(surfaceParams->GetBounds());
        canvasBackup_->Clear(Drawing::Color::COLOR_TRANSPARENT);
    }
}

void RSLogicalDisplayRenderNodeDrawable::PrepareOffscreenRender(
    const RSLogicalDisplayRenderNodeDrawable& displayDrawable, bool useFixedSize, bool useCanvasSize, bool fixFormat)
{
    const auto& params = static_cast<RSLogicalDisplayRenderParams*>(displayDrawable.GetRenderParams().get());
    if (UNLIKELY(!params)) {
        RS_LOGE("RSLogicalDisplayRenderNodeDrawable::PrepareOffscreenRender params is null!");
        return;
    }
    canvasBackup_ = nullptr;
    useFixedOffscreenSurfaceSize_ = false;

    auto frameSize = params->GetFrameRect();
    auto offscreenWidth = static_cast<int32_t>(frameSize.GetWidth());
    auto offscreenHeight = static_cast<int32_t>(frameSize.GetHeight());

    RS_TRACE_FUNC();
    // use fixed surface size in order to reduce create texture
    if (useFixedSize && RotateOffScreenParam::GetRotateOffScreenScreenNodeEnable()
        && params->IsRotationChanged()) {
        useFixedOffscreenSurfaceSize_ = true;
        int32_t maxRenderLength = {};
        if (RSSystemProperties::GetCacheOptimizeRotateEnable()) {
            maxRenderLength = std::ceil(std::hypot(offscreenWidth, offscreenHeight));
            offscreenTranslateX_ = std::round((maxRenderLength - offscreenWidth) * 0.5f);
            offscreenTranslateY_ = std::round((maxRenderLength - offscreenHeight) * 0.5f);
        } else {
            maxRenderLength =
                static_cast<int32_t>(std::max(params->GetFixedWidth(), params->GetFixedHeight()));
            if (offscreenSurface_ != nullptr
                && maxRenderLength != std::max(offscreenSurface_->Width(), offscreenSurface_->Height())) {
                RS_TRACE_NAME("offscreen surface's max size has changed");
                offscreenSurface_ = nullptr;
            }
        }
        offscreenWidth = maxRenderLength;
        offscreenHeight = maxRenderLength;
    } else {
        offscreenTranslateX_ = 0;
        offscreenTranslateY_ = 0;
    }
    if (params->IsRotationChanged()) {
        if (RSUniRenderThread::Instance().GetVmaOptimizeFlag()) {
            // render this frame with vma cache on
            Drawing::StaticFactory::SetVmaCacheStatus(true);
        }
    }

    if (offscreenWidth <= 0 || offscreenHeight <= 0) {
        RS_LOGE("RSLogicalDisplayRenderNodeDrawable::PrepareOffscreenRender, "
            "offscreenWidth or offscreenHeight is invalid");
        return;
    }
    if (curCanvas_->GetSurface() == nullptr) {
        curCanvas_->ClipRect(Drawing::Rect(0, 0, offscreenWidth, offscreenHeight), Drawing::ClipOp::INTERSECT, false);
        RS_LOGE("RSLogicalDisplayRenderNodeDrawable::PrepareOffscreenRender, current surface is nullptr");
        return;
    }

    auto [_, screenParams] = GetScreenParams(*params);
    if (!screenParams) {
        RS_LOGE("RSLogicalDisplayRenderNodeDrawable::%{public}s screenParams is nullptr", __func__);
        return;
    }
    bool createOffscreenSurface = !params->GetNeedOffscreen() || !useFixedOffscreenSurfaceSize_ || !offscreenSurface_ ||
        ((screenParams->GetHDRPresent() || curCanvas_->GetHDREnabledVirtualScreen()) &&
        offscreenSurface_->GetImageInfo().GetColorType() != Drawing::ColorType::COLORTYPE_RGBA_F16);
    bool canvasReplacable = false;
    if (createOffscreenSurface) {
        bool hdrOrScRGB = screenParams->GetHDRPresent() || curCanvas_->GetHDREnabledVirtualScreen() ||
            EnablescRGBForP3AndUiFirst(screenParams->GetNewColorSpace());
        RS_TRACE_NAME_FMT("make offscreen surface with fixed size: [%d, %d], HDR:%d", offscreenWidth, offscreenHeight,
            hdrOrScRGB);
        if (hdrOrScRGB) {
            if (!params->GetNeedOffscreen() && useCanvasSize) {
                offscreenWidth = curCanvas_->GetWidth();
                offscreenHeight = curCanvas_->GetHeight();
            }
            auto colorType = Drawing::ColorType::COLORTYPE_RGBA_F16;
            auto colorSpace = Drawing::ColorSpace::CreateSRGB();
            canvasReplacable = screenParams->GetScreenHDRStatus() == HdrStatus::HDR_EFFECT &&
                RSSystemProperties::GetEDRCanvasReplaceEnabled();
            // Disable fp16 for hardware constrains. 'true' indicates check brightness ratio
            // If the HDR type on screen is only EDR, the canvas is replacable and replaced in onDraw
            if (RSLuminanceControl::Get().IsHardwareHdrDisabled(true, params->GetScreenId()) || canvasReplacable) {
                colorType = RSBaseRenderUtil::GetColorTypeFromBufferFormat(screenParams->GetNewPixelFormat());
                colorSpace =
                    RSBaseRenderEngine::ConvertColorGamutToDrawingColorSpace(screenParams->GetNewColorSpace());
            }
            RS_LOGD("RS_EDR canvasReplacable:%{public}d, colorType:%{public}d", canvasReplacable, colorType);
            Drawing::ImageInfo info = {
                offscreenWidth, offscreenHeight, colorType, Drawing::ALPHATYPE_PREMUL, colorSpace };
            offscreenSurface_ = curCanvas_->GetSurface()->MakeSurface(info);
        } else if (fixFormat) {
            // Use for SDR scenario in HDR cast. The alpha bits will lose if offscreen canvas follows
            // virtual screen canvas to use 1010102. Fix 8888 bit for offscreen canvas
            auto colorType = Drawing::ColorType::COLORTYPE_RGBA_8888;
            auto colorSpace =
                RSBaseRenderEngine::ConvertColorGamutToDrawingColorSpace(screenParams->GetNewColorSpace());
            Drawing::ImageInfo info = {
                offscreenWidth, offscreenHeight, colorType, Drawing::ALPHATYPE_PREMUL, colorSpace };
            offscreenSurface_ = curCanvas_->GetSurface()->MakeSurface(info);
        } else {
            offscreenSurface_ = curCanvas_->GetSurface()->MakeSurface(offscreenWidth, offscreenHeight);
        }
    }

    if (offscreenSurface_ == nullptr) {
        RS_LOGE("RSLogicalDisplayRenderNodeDrawable::PrepareOffscreenRender, offscreenSurface is nullptr");
        curCanvas_->ClipRect(Drawing::Rect(0, 0, offscreenWidth, offscreenHeight), Drawing::ClipOp::INTERSECT, false);
        return;
    }

    offscreenCanvas_ = std::make_shared<RSPaintFilterCanvas>(offscreenSurface_.get());

    if (RSSystemProperties::GetCacheOptimizeRotateEnable()) {
        offscreenCanvas_->ResetMatrix();
        offscreenCanvas_->Translate(offscreenTranslateX_, offscreenTranslateY_);
    }

    // copy HDR properties into offscreen canvas
    offscreenCanvas_->CopyHDRConfiguration(*curCanvas_);
    // set canvas replacable if only edr effect
    offscreenCanvas_->SetCanvasReplacable(canvasReplacable);
    // copy current canvas properties into offscreen canvas
    offscreenCanvas_->CopyConfigurationToOffscreenCanvas(*curCanvas_);

    // backup current canvas and replace with offscreen canvas
    canvasBackup_ = std::exchange(curCanvas_, offscreenCanvas_.get());
}

void RSLogicalDisplayRenderNodeDrawable::FinishOffscreenRender(
    const Drawing::SamplingOptions& sampling, bool isSamplingOn, float hdrBrightnessRatio)
{
    RS_TRACE_NAME_FMT("%s: isSamplingOn:%d, hdrBrightnessRatio:%f", __func__, isSamplingOn, hdrBrightnessRatio);
    if (canvasBackup_ == nullptr) {
        RS_LOGE("RSLogicalDisplayRenderNodeDrawable::FinishOffscreenRender, canvasBackup_ is nullptr");
        return;
    }
    if (offscreenSurface_ == nullptr) {
        RS_LOGE("RSLogicalDisplayRenderNodeDrawable::FinishOffscreenRender, offscreenSurface_ is nullptr");
        return;
    }
    std::shared_ptr<Drawing::Image> image = nullptr;

    Drawing::Brush paint;
    bool isUseCustomShader = false;
#ifdef USE_VIDEO_PROCESSING_ENGINE
    if (canvasBackup_->GetHDREnabledVirtualScreen()) {
        isUseCustomShader = RSHdrUtil::HDRCastProcess(image, paint, sampling, offscreenSurface_, canvasBackup_);
    }
#endif
    if (!isUseCustomShader) {
        image = offscreenSurface_->GetImageSnapshot();
    }
    if (image == nullptr) {
        RS_LOGE("RSLogicalDisplayRenderNodeDrawable::FinishOffscreenRender, image is nullptr");
        return;
    }

    if (ROSEN_LNE(hdrBrightnessRatio, 1.0f)) {
        auto shader = MakeBrightnessAdjustmentShader(image, sampling, hdrBrightnessRatio);
        if (shader) {
            paint.SetShaderEffect(shader);
            isUseCustomShader = true;
        } else {
            FinishHdrDraw(paint, hdrBrightnessRatio);
        }
    }

    paint.SetAntiAlias(true);
    canvasBackup_->AttachBrush(paint);

    if (isSamplingOn) {
        if (RSSystemProperties::GetSLRScaleEnabled() && scaleManager_) {
            scaleManager_->ProcessOffscreenImage(*canvasBackup_, *image);
        } else {
            canvasBackup_->DrawImage(*image, 0, 0, sampling);
        }
    } else if (RSSystemProperties::GetCacheOptimizeRotateEnable()) {
        if (isUseCustomShader) {
            canvasBackup_->Translate(-offscreenTranslateX_, -offscreenTranslateY_);
            canvasBackup_->DrawRect({ 0., 0., image->GetImageInfo().GetWidth(), image->GetImageInfo().GetHeight() });
        } else {
            canvasBackup_->DrawImage(*image, -offscreenTranslateX_, -offscreenTranslateY_, sampling);
        }
        canvasBackup_->Translate(offscreenTranslateX_, offscreenTranslateY_);
    } else {
        if (isUseCustomShader) {
            canvasBackup_->DrawRect({ 0., 0., image->GetImageInfo().GetWidth(), image->GetImageInfo().GetHeight() });
        } else {
            canvasBackup_->DrawImage(*image, 0, 0, sampling);
        }
    }
    canvasBackup_->DetachBrush();

    if (!useFixedOffscreenSurfaceSize_) {
        offscreenSurface_ = nullptr;
        offscreenCanvas_ = nullptr;
    }
    curCanvas_ = canvasBackup_;
    canvasBackup_ = nullptr;
}

std::shared_ptr<Drawing::ShaderEffect> RSLogicalDisplayRenderNodeDrawable::MakeBrightnessAdjustmentShader(
    const std::shared_ptr<Drawing::Image>& image, const Drawing::SamplingOptions& sampling, float hdrBrightnessRatio)
{
    static const std::string shaderString(R"(
        uniform shader imageInput;
        uniform float ratio;
        half4 main(float2 xy) {
            half4 c = imageInput.eval(xy);
            return half4(c.rgb * ratio, c.a);
        }
    )");
    if (brightnessAdjustmentShaderEffect_ == nullptr) {
        Drawing::RuntimeEffectOptions runtimeEffectOptions;
        runtimeEffectOptions.useHighpLocalCoords = true;
        brightnessAdjustmentShaderEffect_ = Drawing::RuntimeEffect::CreateForShader(shaderString, runtimeEffectOptions);
        if (brightnessAdjustmentShaderEffect_ == nullptr) {
            ROSEN_LOGE("RSLogicalDisplayRenderNodeDrawable::MakeBrightnessAdjustmentShaderBuilder effect is null");
            return nullptr;
        }
    }

    auto builder = std::make_shared<Drawing::RuntimeShaderBuilder>(brightnessAdjustmentShaderEffect_);
    if (!builder) {
        ROSEN_LOGE("RSLogicalDisplayRenderNodeDrawable::MakeBrightnessAdjustmentShaderBuilder builder is null");
        return nullptr;
    }
    builder->SetChild("imageInput", Drawing::ShaderEffect::CreateImageShader(*image, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, sampling, Drawing::Matrix()));
    builder->SetUniform("ratio", hdrBrightnessRatio);
    return builder->MakeShader(nullptr, false);
}

RSLogicalDisplayRenderNodeDrawable::AncestorParams RSLogicalDisplayRenderNodeDrawable::GetScreenParams(
    RSRenderParams& params)
{
    auto displayParams = static_cast<RSLogicalDisplayRenderParams*>(&params);
    if (!displayParams) {
        return { nullptr, nullptr };
    }
    auto screenDrawable = std::static_pointer_cast<RSScreenRenderNodeDrawable>(
        displayParams->GetAncestorScreenDrawable().lock());
    if (!screenDrawable || screenDrawable->GetNodeType() != RSRenderNodeType::SCREEN_NODE) {
        return { nullptr, nullptr };
    }
    auto screenParams = static_cast<RSScreenRenderParams*>(screenDrawable->GetRenderParams().get());
    return {screenDrawable, screenParams};
}

RSLogicalDisplayRenderNodeDrawable::MirrorSourceParams RSLogicalDisplayRenderNodeDrawable::GetMirrorSourceParams(
    RSRenderParams& params)
{
    auto displayParams = static_cast<RSLogicalDisplayRenderParams*>(&params);
    if (!displayParams) {
        return { nullptr, nullptr, nullptr, nullptr };
    }
    auto mirroredDrawable = std::static_pointer_cast<RSLogicalDisplayRenderNodeDrawable>(
        displayParams->GetMirrorSourceDrawable().lock());
    if (!mirroredDrawable) {
        return { nullptr, nullptr, nullptr, nullptr };
    }
    auto mirroredParams = static_cast<RSLogicalDisplayRenderParams*>(mirroredDrawable->GetRenderParams().get());
    if (!mirroredParams) {
        return { mirroredDrawable, nullptr, nullptr, nullptr };
    }

    return std::tuple_cat(std::tuple{mirroredDrawable, mirroredParams}, GetScreenParams(*mirroredParams));
}
} // namespace OHOS::ROSEN::DrawableV2