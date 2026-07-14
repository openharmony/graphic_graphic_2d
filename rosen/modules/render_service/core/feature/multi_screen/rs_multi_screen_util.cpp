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

#include "rs_multi_screen_util.h"

#include "common/rs_optional_trace.h"
#include "drawable/dfx/rs_dirty_rects_dfx.h"
#include "drawable/rs_logical_display_render_node_drawable.h"
#include "drawable/rs_render_node_drawable.h"
#include "drawable/rs_screen_render_node_drawable.h"
#include "feature/dirty/rs_uni_dirty_compute_util.h"
#include "feature/drm/rs_drm_util.h"
#include "feature/frame_stability/rs_frame_stability_manager.h"
#include "feature/special_layer/rs_special_layer_utils.h"
#include "feature/pointer_window_manager/rs_pointer_window_manager.h"
#include "graphic_feature_param_manager.h"
#include "params/rs_render_thread_params.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "pipeline/render_thread/rs_uni_render_util.h"
#include "pipeline/render_thread/rs_uni_render_virtual_processor.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "rs_profiler_capture_recorder.h"
#include "rs_trace.h"
#include "system/rs_system_parameters.h"

namespace OHOS::Rosen::DrawableV2 {

void RSMultiScreenUtil::HandleMirrorDisplay(
    RSLogicalDisplayRenderNodeDrawable& drawable,
    RSLogicalDisplayRenderParams& params,
    const std::shared_ptr<RSProcessor>& processor)
{
    auto& uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    if (!uniParam) {
        drawable.SetDrawSkipType(DrawSkipType::RENDER_THREAD_PARAMS_NULL);
        RS_LOGE("RSMultiScreenUtil::%{public}s uniParam is null", __func__);
        return;
    }

    auto [_, screenParams] = drawable.GetScreenParams(params);
    if (!screenParams) {
        RS_LOGE("RSMultiScreenUtil::%{public}s screenParams is null", __func__);
        return;
    }

    if (!processor || !processor->UpdateMirrorInfo(drawable)) {
        drawable.SetDrawSkipType(DrawSkipType::RENDER_ENGINE_NULL);
        RS_LOGE("RSMultiScreenUtil::%{public}s processor update info failed", __func__);
        return;
    }

    const auto& screenProperty = screenParams->GetScreenProperty();
    uniParam->SetSecurityDisplay(params.IsSecurityDisplay());
    drawable.currentBlackList_ = RSSpecialLayerUtils::GetMergeBlackList(screenProperty);
    RSUniRenderThread::Instance().SetBlackList(drawable.currentBlackList_);
    drawable.curVisibleRect_ = RSUniRenderThread::Instance().GetVisibleRect();
    drawable.enableVisibleRect_ = RSUniRenderThread::Instance().GetEnableVisibleRect();

    if (params.GetCompositeType() == CompositeType::UNI_RENDER_COMPOSITE) {
        RSMultiScreenUtil::DrawPhysicalMirrorDisplay(drawable, params, processor);
        drawable.lastBlackList_ = drawable.currentBlackList_;
        drawable.lastEnableVisibleRect_ = drawable.enableVisibleRect_;
        drawable.lastVisibleRect_ = drawable.curVisibleRect_;
        uniParam->SetSecurityDisplay(false);
        RSUniRenderThread::Instance().SetBlackList({});
        return;
    }

    drawable.currentTypeBlackList_ = screenProperty.GetTypeBlackList();
    RSUniRenderThread::Instance().SetTypeBlackList(drawable.currentTypeBlackList_);
    RSUniRenderThread::Instance().SetWhiteList(screenProperty.GetWhiteList());
    drawable.curSecExemption_ = params.GetSecurityExemption();
    uniParam->SetSecExemption(drawable.curSecExemption_);

    RSMultiScreenUtil::DrawVirtualMirrorDisplay(drawable, params, processor);

    drawable.lastEnableVisibleRect_ = drawable.enableVisibleRect_;
    drawable.lastVisibleRect_ = drawable.curVisibleRect_;
    drawable.lastTypeBlackList_ = drawable.currentTypeBlackList_;
    drawable.lastSecExemption_ = drawable.curSecExemption_;
    uniParam->SetSecurityDisplay(false);
    drawable.lastBlackList_ = drawable.currentBlackList_;
    RSUniRenderThread::Instance().SetBlackList({});
    RSUniRenderThread::Instance().SetTypeBlackList({});
    RSUniRenderThread::Instance().SetWhiteList({});
    uniParam->SetSecExemption(false);
}

void RSMultiScreenUtil::HandleVirtualExtendDisplay(
    RSLogicalDisplayRenderNodeDrawable& drawable,
    RSLogicalDisplayRenderParams& params,
    const std::shared_ptr<RSProcessor>& processor)
{
    auto& uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    if (!uniParam) {
        drawable.SetDrawSkipType(DrawSkipType::RENDER_THREAD_PARAMS_NULL);
        RS_LOGE("RSMultiScreenUtil::%{public}s uniParam is null", __func__);
        return;
    }

    auto [_, screenParams] = drawable.GetScreenParams(params);
    if (!screenParams) {
        RS_LOGE("RSMultiScreenUtil::%{public}s screenParams is null", __func__);
        return;
    }

    auto virtualProcessor = RSProcessor::ReinterpretCast<RSUniRenderVirtualProcessor>(processor);
    if (!virtualProcessor) {
        RS_LOGE("RSMultiScreenUtil::%{public}s virtualProcessor is nullptr", __func__);
        return;
    }

    if (!virtualProcessor->UpdateMirrorInfo(drawable)) {
        drawable.SetDrawSkipType(DrawSkipType::RENDER_ENGINE_NULL);
        RS_LOGE("RSMultiScreenUtil::%{public}s processor update info failed", __func__);
        return;
    }

    const auto& screenProperty = screenParams->GetScreenProperty();
    uniParam->SetSecurityDisplay(params.IsSecurityDisplay());
    drawable.currentBlackList_ = RSSpecialLayerUtils::GetMergeBlackList(screenProperty);
    RSUniRenderThread::Instance().SetBlackList(drawable.currentBlackList_);
    RSUniRenderThread::Instance().SetWhiteList(screenProperty.GetWhiteList());
    drawable.curSecExemption_ = params.GetSecurityExemption();
    uniParam->SetSecExemption(drawable.curSecExemption_);

    RSMultiScreenUtil::DrawVirtualExtend(drawable, params, virtualProcessor);

    drawable.lastSecExemption_ = drawable.curSecExemption_;
    uniParam->SetSecurityDisplay(false);
    drawable.lastBlackList_ = drawable.currentBlackList_;
    RSUniRenderThread::Instance().SetBlackList({});
    RSUniRenderThread::Instance().SetTypeBlackList({});
    RSUniRenderThread::Instance().SetWhiteList({});
    uniParam->SetSecExemption(false);
}

void RSMultiScreenUtil::HandleMirrorScreen(
    RSScreenRenderNodeDrawable& drawable,
    RSScreenRenderParams& mirrorSourceParams,
    RSScreenRenderParams& params,
    const std::shared_ptr<RSProcessor>& processor)
{
    auto& uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    if (!uniParam) {
        drawable.SetDrawSkipType(DrawSkipType::RENDER_PARAMS_OR_UNI_PARAMS_NULL);
        RS_LOGE("RSMultiScreenUtil::%{public}s uniParam is null!", __func__);
        return;
    }

    const auto& screenProperty = params.GetScreenProperty();

    RSUniRenderThread::BufferManagerGuard bufferGuard;
    DirtyStatusAutoUpdate dirtyStatusAutoUpdate(*uniParam, params.GetChildDisplayCount() == 1);
    if (!processor || !processor->InitForRenderThread(drawable, RSUniRenderThread::Instance().GetRenderEngine())) {
        // Clear cacheImgForCapture so that mirrorScreen avoid using an expired cache
        drawable.cacheImgForCapture_ = nullptr;
        drawable.SetDrawSkipType(DrawSkipType::RENDER_ENGINE_NULL);
        drawable.syncDirtyManager_->ResetDirtyAsSurfaceSize();
        drawable.syncDirtyManager_->UpdateDirty(false);
        RS_LOGD("RSMultiScreenUtil::%{public}s processor init for render thread failed!", __func__);
        return;
    }

    const auto& mirrorSourceProperty = mirrorSourceParams.GetScreenProperty();
    drawable.offsetX_ = mirrorSourceProperty.GetOffsetX();
    drawable.offsetY_ = mirrorSourceProperty.GetOffsetY();
    bool enableVisibleRect = screenProperty.GetEnableVisibleRect();
    RSUniRenderThread::Instance().SetEnableVisibleRect(enableVisibleRect);
    if (enableVisibleRect) {
        const auto& rect = screenProperty.GetVisibleRect();
        auto curVisibleRect = Drawing::RectI(rect.x, rect.y, rect.x + rect.w, rect.y + rect.h);
        RSUniRenderThread::Instance().SetVisibleRect(curVisibleRect);
    }

    if (params.GetCompositeType() == CompositeType::UNI_RENDER_COMPOSITE) {
        drawable.SetDrawSkipType(DrawSkipType::WIRED_SCREEN_PROJECTION);
        RSUniRenderThread::Instance().WaitUntilScreenNodeBufferReleased(drawable);
        drawable.physicalMirrorRenderFrame_ = drawable.RequestFrame(params, processor);
        if (!drawable.physicalMirrorRenderFrame_) {
            drawable.SetDrawSkipType(DrawSkipType::REQUEST_FRAME_FAIL);
            RS_LOGE("RSMultiScreenUtil::%{public}s failed to request frame", __func__);
            return;
        }
        auto drSurface = drawable.physicalMirrorRenderFrame_->GetFrame()->GetSurface();
        if (!drSurface) {
            RS_LOGE("RSMultiScreenUtil::%{public}s DrawingSurface is null", __func__);
            return;
        }
        drawable.curCanvas_ = std::make_shared<RSPaintFilterCanvas>(drSurface.get());
        drawable.RSRenderNodeDrawable::OnDraw(*drawable.curCanvas_);
        drawable.physicalMirrorRenderFrame_->Flush();
        processor->ProcessScreenSurfaceForRenderThread(drawable);
        RSPointerWindowManager::Instance().HardCursorCreateLayer(processor, drawable.GetId());
        processor->PostProcess();
        bufferGuard.SetAcquireFence(drawable.physicalMirrorRenderFrame_->GetAcquireFence());
        drawable.physicalMirrorRenderFrame_ = nullptr;
        return;
    }

    auto virtualProcessor = RSProcessor::ReinterpretCast<RSUniRenderVirtualProcessor>(processor);
    if (!virtualProcessor) {
        RS_LOGE("RSMultiScreenUtil::%{public}s virtualProcessor is null!", __func__);
        return;
    }
    drawable.curCanvas_ = virtualProcessor->GetCanvas();
    drawable.RSRenderNodeDrawable::OnDraw(*drawable.curCanvas_);
    if (virtualProcessor->GetDisplaySkipInMirror()) {
        RS_TRACE_NAME("skip in virtual screen and cancelbuffer");
        virtualProcessor->SetDisplaySkipInMirror(false);
        virtualProcessor->CancelCurrentFrame();
        return;
    }
    drawable.DrawCurtainScreen();
    processor->PostProcess();
    bufferGuard.SetAcquireFence(virtualProcessor->GetFrameAcquireFence());
    drawable.SetDrawSkipType(DrawSkipType::MIRROR_DRAWABLE_SKIP);
}

void RSMultiScreenUtil::HandleVirtualExtendScreen(
    RSScreenRenderNodeDrawable& drawable,
    RSScreenRenderParams& params,
    const std::shared_ptr<RSProcessor>& processor)
{
    auto& uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    if (!uniParam) {
        drawable.SetDrawSkipType(DrawSkipType::RENDER_PARAMS_OR_UNI_PARAMS_NULL);
        RS_LOGE("RSMultiScreenUtil::%{public}s uniParam is null!", __func__);
        return;
    }

    const auto& screenProperty = params.GetScreenProperty();
    auto screenInfo = screenProperty.GetScreenInfo();
    ScreenId paramScreenId = params.GetScreenId();

    RSUniRenderThread::BufferManagerGuard bufferGuard;
    DirtyStatusAutoUpdate dirtyStatusAutoUpdate(*uniParam, params.GetChildDisplayCount() == 1);
    if (!processor || !processor->InitForRenderThread(drawable, RSUniRenderThread::Instance().GetRenderEngine())) {
        // Clear cacheImgForCapture so that mirrorScreen avoid using an expired cache
        drawable.cacheImgForCapture_ = nullptr;
        drawable.SetDrawSkipType(DrawSkipType::RENDER_ENGINE_NULL);
        drawable.syncDirtyManager_->ResetDirtyAsSurfaceSize();
        drawable.syncDirtyManager_->UpdateDirty(false);
        RS_LOGD("RSMultiScreenUtil::%{public}s processor init for render thread failed!", __func__);
        return;
    }

    RS_LOGD("RSMultiScreenUtil::%{public}s Expand screen.", __func__);
    bool isOpDropped = uniParam->IsOpDropped();
    uniParam->SetOpDropped(uniParam->IsVirtualExpandScreenDirtyEnabled());
    auto expandProcessor = RSProcessor::ReinterpretCast<RSUniRenderVirtualProcessor>(processor);
    if (!expandProcessor) {
        drawable.SetDrawSkipType(DrawSkipType::EXPAND_PROCESSOR_NULL);
        RS_LOGE("RSMultiScreenUtil::%{public}s expandProcessor is null!", __func__);
        return;
    }
    RSDirtyRectsDfx rsDirtyRectsDfx(drawable);
    std::vector<RectI> damageRegionRects;
    RSUniDirtyComputeUtil::MergeVirtualExpandScreenAccumulatedDirtyRegions(drawable, params);
    int32_t bufferAge = expandProcessor->GetBufferAge();
    damageRegionRects = RSUniRenderUtil::MergeDirtyHistory(
        drawable, bufferAge, screenInfo, rsDirtyRectsDfx, params);
    uniParam->Reset();
    if (uniParam->IsVirtualExpandScreenDirtyEnabled() && !uniParam->IsVirtualDirtyDfxEnabled() &&
        !expandProcessor->IsMultiSurfaceExtendMode()) {
        expandProcessor->SetDirtyInfo(damageRegionRects);
    } else {
        std::vector<RectI> emptyRects = {};
        expandProcessor->SetRoiRegionToCodec(emptyRects);
    }

    // record current frame refresh area to frame stability manager
    std::vector<RectI> refreshRects(damageRegionRects);
    refreshRects.emplace_back(drawable.syncDirtyManager_->GetHwcDirtyRegion());
    RSFrameStabilityManager::GetInstance().RecordCurrentFrameDirty(
        paramScreenId, refreshRects, screenInfo.width * screenInfo.height);

    rsDirtyRectsDfx.SetVirtualDirtyRects(damageRegionRects, screenInfo);
    drawable.curCanvas_ = expandProcessor->GetCanvas();
    auto& curCanvas = drawable.curCanvas_;
    curCanvas->Save();
    if (uniParam->IsVirtualExpandScreenDirtyEnabled()) {
        drawable.UpdateSurfaceDrawRegion(curCanvas, &params);
        curCanvas->SetDrawnRegion(params.GetDrawnRegion());
        if (uniParam->IsDirtyAlignEnabled() && RSUniDirtyComputeUtil::IsDamageRegionGpuTileValid() &&
            damageRegionRects.size() > RSUniDirtyComputeUtil::DIRTY_REGION_COUNT_THRESHOLD) {
            RS_TRACE_NAME("dirty align enabled and no clip operation");
            curCanvas->Clear(Drawing::Color::COLOR_TRANSPARENT);
        } else {
            Drawing::Region clipRegion = RSUniDirtyComputeUtil::GetFlippedRegion(damageRegionRects, screenInfo);
            RSUniDirtyComputeUtil::ClipRegion(*curCanvas, clipRegion);
        }
    } else {
        curCanvas->Clear(Drawing::Color::COLOR_TRANSPARENT);
    }
    RSUniDirtyComputeUtil::ClearVirtualExpandScreenAccumulatedDirtyRegions(drawable, params);
    drawable.RSRenderNodeDrawable::OnDraw(*curCanvas);
    params.ResetVirtualExpandAccumulatedParams();
    RSUniRenderThread::ResetCaptureParam();
    auto targetSurfaceRenderNodeDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
        params.GetTargetSurfaceRenderNodeDrawable().lock());
    // Sometimes may require setting cacheImg, such as MultiScreenView and SingleAppCast
    if ((targetSurfaceRenderNodeDrawable || params.HasMirrorScreen()) && curCanvas->GetSurface()) {
        RS_TRACE_NAME_FMT("%s set cacheImg for capture or multiscreenview", __func__);
        drawable.cacheImgForCapture_ = curCanvas->GetSurface()->GetImageSnapshot();
        drawable.cacheImgForMultiScreenView_ = drawable.cacheImgForCapture_;
    } else {
        drawable.cacheImgForCapture_ = nullptr;
        drawable.cacheImgForMultiScreenView_ = nullptr;
    }
    // Restore the initial state of the canvas to avoid state accumulation
    curCanvas->RestoreToCount(0);
    if (curCanvas) {
        rsDirtyRectsDfx.OnDrawVirtual(*curCanvas);
    }
    uniParam->SetOpDropped(isOpDropped);
    drawable.DrawCurtainScreen();
    processor->PostProcess();
    bufferGuard.SetAcquireFence(expandProcessor->GetFrameAcquireFence());
    drawable.SetDrawSkipType(DrawSkipType::MIRROR_DRAWABLE_SKIP);
}

void RSMultiScreenUtil::DrawPhysicalMirrorDisplay(
    RSLogicalDisplayRenderNodeDrawable& drawable,
    RSLogicalDisplayRenderParams& params,
    const std::shared_ptr<RSProcessor>& processor)
{
    auto& curCanvas = drawable.curCanvas_;
    if (!curCanvas) {
        RS_LOGE("RSMultiScreenUtil::%{public}s failed to create canvas", __func__);
        return;
    }

    auto [multiScreenParams, isValid] = RSMultiScreenUtil::GetMultiScreenParams(params);
    if (!isValid) {
        RS_LOGE("RSMultiScreenUtil::%{public}s params are invalid", __func__);
        return;
    }

    auto& [screenDrawable, screenParams, mirrorSourceDisplayDrawable, mirrorSourceDisplayParams,
        mirrorSourceScreenDrawable, mirrorSourceScreenParams] = multiScreenParams;
    auto cacheImage = mirrorSourceScreenDrawable->GetCacheImgForCapture();
    const auto& specialLayerMgr = mirrorSourceDisplayParams->GetSpecialLayerMgr();
    bool needProcessSecLayer = !MultiScreenParam::IsExternalScreenSecure() &&
        specialLayerMgr.Find(SpecialLayerType::HAS_SECURITY);
    const auto& currentBlackList = drawable.currentBlackList_;
    bool hasProtected = specialLayerMgr.Find(SpecialLayerType::HAS_PROTECTED);
    bool hasSpecialLayer = mirrorSourceScreenParams->GetHDRPresent() || needProcessSecLayer ||
        !currentBlackList.empty() || hasProtected;
    bool enableVisibleRect = drawable.enableVisibleRect_;
    bool isRedraw = RSSystemParameters::GetDebugMirrorOndrawEnabled() || !cacheImage ||
        (RSSystemParameters::GetWiredScreenOndrawEnabled() && !enableVisibleRect && hasSpecialLayer);

    RS_TRACE_NAME_FMT("%s, cacheImage[%d], enableVisibleRect[%d], mirroredHDR[%d], needProcessSecLayer[%d], "
        "blackListSize[%zu], hasProtected[%d]",
        __func__, cacheImage != nullptr, enableVisibleRect, mirrorSourceScreenParams->GetHDRPresent(),
        needProcessSecLayer, currentBlackList.size(), hasProtected);

    // HDR does not support wired screen
    if (isRedraw) {
        drawable.isMirrorSLRCopy_ = false;
        RSMultiScreenUtil::DumpRenderStrategy(drawable, params.GetScreenId(),
            RenderStrategy::DRAW_PHYSICAL_MIRROR_REBUILD, "");
        RSMultiScreenUtil::DrawPhysicalMirrorRebuild(drawable, params, processor);
    } else {
        // cacheImage must be valid when isMirrorSLRCopy_ is true
        drawable.isMirrorSLRCopy_ = !enableVisibleRect && RSSystemProperties::GetSLRScaleEnabled();
        RSMultiScreenUtil::DumpRenderStrategy(drawable, params.GetScreenId(),
            RenderStrategy::DRAW_PHYSICAL_MIRROR_FROM_CACHE, "");
        RSMultiScreenUtil::DrawPhysicalMirrorFromCache(drawable, params);
    }
}

void RSMultiScreenUtil::DrawPhysicalMirrorFromCache(
    RSLogicalDisplayRenderNodeDrawable& drawable,
    RSLogicalDisplayRenderParams& params)
{
    auto [multiScreenParams, isValid] = RSMultiScreenUtil::GetMultiScreenParams(params);
    if (!isValid) {
        RS_LOGE("RSMultiScreenUtil::%{public}s params are invalid", __func__);
        return;
    }

    auto& [screenDrawable, screenParams, mirrorSourceDisplayDrawable, mirrorSourceDisplayParams,
        mirrorSourceScreenDrawable, mirrorSourceScreenParams] = multiScreenParams;

    auto& curCanvas = drawable.curCanvas_;
    curCanvas->Save();
    curCanvas->SetOnMultipleScreen(true); // use for hdr content tmo to fixed sdr nits
    drawable.ScaleAndRotateMirrorForWiredScreen(*mirrorSourceDisplayDrawable);
    RSDirtyRectsDfx rsDirtyRectsDfx(*screenDrawable);
    std::shared_ptr<RSSLRScaleFunction> scaleManager = drawable.scaleManager_;
    auto matrix = drawable.isMirrorSLRCopy_ ? scaleManager->GetScaleMatrix() : curCanvas->GetTotalMatrix();
    matrix.PreTranslate(-mirrorSourceDisplayParams->GetOffsetX(), -mirrorSourceDisplayParams->GetOffsetY());
    std::vector<RectI> damageRegionRects = drawable.CalculateVirtualDirtyForWiredScreen(*screenDrawable, matrix);
    rsDirtyRectsDfx.SetVirtualDirtyRects(damageRegionRects, screenParams->GetScreenInfo());

    auto width = mirrorSourceDisplayParams->GetFixedWidth();
    auto height = mirrorSourceDisplayParams->GetFixedHeight();

    auto cacheImage = mirrorSourceScreenDrawable->GetCacheImgForCapture();
    bool enableVisibleRect = drawable.enableVisibleRect_;
    const auto& curVisibleRect = drawable.curVisibleRect_;
    if (cacheImage && RSSystemProperties::GetDrawMirrorCacheImageEnabled()) {
        RS_TRACE_NAME_FMT("%s, use cacheImage", __func__);
        if (drawable.isMirrorSLRCopy_) {
            RS_TRACE_NAME_FMT("%s, use SLRScale", __func__);
            scaleManager->ProcessCacheImage(*curCanvas, *cacheImage);
        } else if (!enableVisibleRect) {
            RS_TRACE_NAME_FMT("%s, use SkiaScale", __func__);
            Drawing::RectI srcRect = { mirrorSourceDisplayParams->GetOffsetX(), mirrorSourceDisplayParams->GetOffsetY(),
                mirrorSourceDisplayParams->GetOffsetX() + width, mirrorSourceDisplayParams->GetOffsetY() + height };
            RSUniRenderUtil::ProcessCacheImageRect(*curCanvas, *cacheImage, srcRect,
                Drawing::Rect(0, 0, width, height));
        } else {
            RS_TRACE_NAME_FMT("%s, visibleRect[%d, %d, %d, %d]", __func__,
                curVisibleRect.GetLeft(), curVisibleRect.GetTop(),
                curVisibleRect.GetWidth(), curVisibleRect.GetHeight());
            RSUniRenderUtil::ProcessCacheImageRect(*curCanvas, *cacheImage, curVisibleRect,
                Drawing::Rect(0, 0, curVisibleRect.GetWidth(), curVisibleRect.GetHeight()));
        }
    } else {
        RS_TRACE_NAME_FMT("%s, use displaySurface", __func__);
        auto surfaceHandler = mirrorSourceScreenDrawable->GetRSSurfaceHandlerOnDraw();
        auto drawParams = RSUniRenderUtil::CreateBufferDrawParam(*surfaceHandler, false); // false: draw with gpu
        auto renderEngine = RSUniRenderThread::Instance().GetRenderEngine();
        if (enableVisibleRect) {
            drawParams.srcRect = curVisibleRect;
            drawParams.dstRect = Drawing::Rect(0, 0, curVisibleRect.GetWidth(), curVisibleRect.GetHeight());
        } else {
            drawParams.srcRect = { mirrorSourceDisplayParams->GetOffsetX(), mirrorSourceDisplayParams->GetOffsetY(),
                mirrorSourceDisplayParams->GetOffsetX() + width, mirrorSourceDisplayParams->GetOffsetY() + height };
            drawParams.dstRect = Drawing::Rect(0, 0, width, height);
        }
        drawParams.isMirror = true;
        renderEngine->DrawScreenNodeWithParams(*curCanvas, *surfaceHandler, drawParams);
        RSMainThread::Instance()->RequestNextVSync();
    }
    curCanvas->Restore();
    rsDirtyRectsDfx.OnDrawVirtual(*curCanvas);
}

void RSMultiScreenUtil::DrawPhysicalMirrorRebuild(
    RSLogicalDisplayRenderNodeDrawable& drawable,
    RSLogicalDisplayRenderParams& params,
    const std::shared_ptr<RSProcessor>& processor)
{
    auto& uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    if (uniParam == nullptr) {
        RS_LOGE("RSMultiScreenUtil::%{public}s uniParam is null", __func__);
        return;
    }
    auto [multiScreenParams, isValid] = RSMultiScreenUtil::GetMultiScreenParams(params);
    if (!isValid) {
        RS_LOGE("RSMultiScreenUtil::%{public}s params are invalid", __func__);
        return;
    }

    auto& [screenDrawable, screenParams, mirrorSourceDisplayDrawable, mirrorSourceDisplayParams,
        mirrorSourceScreenDrawable, mirrorSourceScreenParams] = multiScreenParams;

    RS_TRACE_FUNC();
    RSDirtyRectsDfx rsDirtyRectsDfx(*screenDrawable);

    // for HDR
    auto& curCanvas = drawable.curCanvas_;
    curCanvas->SetOnMultipleScreen(true);
    curCanvas->SetDisableFilterCache(true);

    // Draw security mask if has security surface and external screen is not secure
    const auto& specialLayerMgr = mirrorSourceDisplayParams->GetSpecialLayerMgr();
    auto hasSecSurface = specialLayerMgr.Find(SpecialLayerType::HAS_SECURITY);
    if (hasSecSurface && !MultiScreenParam::IsExternalScreenSecure()) {
        drawable.DrawSecurityMask();
        drawable.virtualDirtyNeedRefresh_ = true;
        rsDirtyRectsDfx.OnDrawVirtual(*curCanvas);
        return;
    }

    curCanvas->Save();
    drawable.ScaleAndRotateMirrorForWiredScreen(*mirrorSourceDisplayDrawable);
    Drawing::Matrix canvasMatrix = curCanvas->GetTotalMatrix();
    curCanvas->SetHighContrast(RSUniRenderThread::Instance().IsHighContrastTextModeOn());
    bool isOpDropped = uniParam->IsOpDropped();
    uniParam->SetOpDropped(false);
    auto screenInfo = mirrorSourceScreenParams->GetScreenInfo();
    uniParam->SetScreenInfo(screenInfo);

    auto width = mirrorSourceDisplayParams->GetFixedWidth();
    auto height = mirrorSourceDisplayParams->GetFixedHeight();
    Drawing::Rect rect(0, 0, width, height);
    curCanvas->ClipRect(rect, Drawing::ClipOp::INTERSECT, false);
    auto offsetX = mirrorSourceDisplayParams->GetOffsetX();
    auto offsetY = mirrorSourceDisplayParams->GetOffsetY();
    curCanvas->Translate(-offsetX, -offsetY);
    curCanvas->ConcatMatrix(mirrorSourceDisplayParams->GetMatrix());
    RSRenderParams::SetParentSurfaceMatrix(curCanvas->GetTotalMatrix());

    // Render mirror content from the source tree
    mirrorSourceDisplayDrawable->RSRenderNodeDrawable::OnDraw(*curCanvas);
    mirrorSourceDisplayDrawable->DrawAdditionalContent(*curCanvas);

    bool displayP3Enable = screenParams->GetNewColorSpace() == GRAPHIC_COLOR_GAMUT_DISPLAY_P3;
    drawable.DrawCurtainScreen();
    // 1.f: wired screen not use hdr, use default value 1.f
    RSUniRenderUtil::SwitchColorFilter(*curCanvas, 1.f, displayP3Enable);

    uniParam->SetOpDropped(isOpDropped);
    RSUniRenderThread::Instance().SetBlackList({});
    curCanvas->Restore();
    rsDirtyRectsDfx.OnDrawVirtual(*curCanvas);
    if (specialLayerMgr.Find(SpecialLayerType::HAS_PROTECTED)) {
        canvasMatrix.PreTranslate(-offsetX, -offsetY);
        RSDrmUtil::DRMCreateLayer(processor, canvasMatrix);
        screenParams->SetGlobalZOrder(screenParams->GetGlobalZOrder() + 1);
    }
}

void RSMultiScreenUtil::DrawVirtualMirrorDisplay(
    RSLogicalDisplayRenderNodeDrawable& drawable,
    RSLogicalDisplayRenderParams& params,
    const std::shared_ptr<RSProcessor>& processor)
{
    // uniParam/drawable/mirrorSourceDisplayParams/renderParams_ not null in caller
    auto& uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    auto [multiScreenParams, isValid] = RSMultiScreenUtil::GetMultiScreenParams(params);
    if (!isValid) {
        RS_LOGE("RSMultiScreenUtil::%{public}s params are invalid", __func__);
        return;
    }

    auto& [screenDrawable, screenParams, mirrorSourceDisplayDrawable, mirrorSourceDisplayParams,
        mirrorSourceScreenDrawable, mirrorSourceScreenParams] = multiScreenParams;

    auto virtualProcesser = RSProcessor::ReinterpretCast<RSUniRenderVirtualProcessor>(processor);
    if (!virtualProcesser) {
        RS_LOGE("RSMultiScreenUtil::%{public}s virtualProcesser is null", __func__);
        return;
    }
    virtualProcesser->CanvasInit(drawable);

    auto cacheImage = mirrorSourceScreenDrawable ? mirrorSourceScreenDrawable->GetCacheImgForCapture() : nullptr;
    auto specialLayerType = RSSpecialLayerUtils::GetSpecialLayerStateInVisibleRect(&params, screenParams);
    // if specialLayer is visible and no CacheImg
    bool needProcessSpecialLayer = mirrorSourceDisplayParams->IsSecurityDisplay() != params.IsSecurityDisplay() &&
        specialLayerType == DisplaySpecialLayerState::HAS_SPECIAL_LAYER;

    std::bitset<RebuildReason::MAX_VALUE> rebuildReasonFlag;
    rebuildReasonFlag.set(RebuildReason::PROCESS_SPECIAL_LAYER, needProcessSpecialLayer);
    rebuildReasonFlag.set(RebuildReason::MIRROR_SCREEN_HDR_PRESENT, screenParams->GetHDRPresent());
    rebuildReasonFlag.set(RebuildReason::MIRROR_SOURCE_HDR_PRESENT, mirrorSourceScreenParams->GetHDRPresent());
    rebuildReasonFlag.set(RebuildReason::CACHE_IMAGE_NULL, cacheImage == nullptr);
    rebuildReasonFlag.set(RebuildReason::VIRTUAL_SCREEN_MUTE, params.GetVirtualScreenMuteStatus());
    rebuildReasonFlag.set(RebuildReason::SCREEN_OFF, mirrorSourceScreenDrawable->IsRenderSkipIfScreenOff());
    rebuildReasonFlag.set(RebuildReason::COLOR_FILTER_MODE, RSUniRenderThread::Instance().IsColorFilterModeOn());

    RenderStrategy curStrategy = rebuildReasonFlag.any() ?
        RenderStrategy::DRAW_VIRTUAL_MIRROR_REBUILD : RenderStrategy::DRAW_VIRTUAL_MIRROR_FROM_CACHE;
    RSMultiScreenUtil::DumpRenderStrategy(drawable, params.GetScreenId(), curStrategy, rebuildReasonFlag.to_string());
    RS_TRACE_NAME_FMT("%s, mirrorSourceScreenId[%" PRIu64 "], rebuildReasonFlag[%s], specialLayerType[%d]",
        __func__, mirrorSourceScreenParams->GetScreenId(), rebuildReasonFlag.to_string().c_str(), specialLayerType);

    if (rebuildReasonFlag.any()) {
        virtualProcesser->SetDrawVirtualMirrorCopy(false);
        RSMultiScreenUtil::DrawVirtualMirrorRebuild(drawable, params, virtualProcesser, *uniParam);
    } else {
        virtualProcesser->SetDrawVirtualMirrorCopy(true);
        RSMultiScreenUtil::DrawVirtualMirrorFromCache(drawable, params, virtualProcesser, *uniParam);
    }
}

void RSMultiScreenUtil::DrawVirtualMirrorFromCache(
    RSLogicalDisplayRenderNodeDrawable& drawable,
    RSLogicalDisplayRenderParams& params,
    const std::shared_ptr<RSUniRenderVirtualProcessor>& processor,
    RSRenderThreadParams& uniParam)
{
    RS_TRACE_FUNC();
    auto [multiScreenParams, isValid] = RSMultiScreenUtil::GetMultiScreenParams(params);
    if (!isValid) {
        RS_LOGE("RSMultiScreenUtil::%{public}s params are invalid", __func__);
        return;
    }

    auto& [screenDrawable, screenParams, mirrorSourceDisplayDrawable, mirrorSourceDisplayParams,
        mirrorSourceScreenDrawable, mirrorSourceScreenParams] = multiScreenParams;
    auto cacheImage = mirrorSourceScreenDrawable ? mirrorSourceScreenDrawable->GetCacheImgForCapture() : nullptr;
    bool isOpDropped = uniParam.IsOpDropped();
    uniParam.SetOpDropped(false);
#ifdef RS_PROFILER_ENABLED
    if (processor && processor->GetCanvas()) {
        float width = static_cast<float>(processor->GetCanvas()->GetWidth());
        float height = static_cast<float>(processor->GetCanvas()->GetHeight());
        auto canvas = RSCaptureRecorder::GetInstance().TryInstantCapture(width, height, SkpCaptureType::IMG_CACHED);
        if (canvas) {
            processor->GetCanvas()->AddCanvas(canvas);
        }
    }
#endif
    processor->CalculateTransform(drawable.GetOriginScreenRotation());
    RSDirtyRectsDfx rsDirtyRectsDfx(*screenDrawable);
    bool enableVisibleRect = drawable.enableVisibleRect_;
    std::shared_ptr<RSSLRScaleFunction> slrManager = enableVisibleRect ? nullptr : processor->GetSlrManager();
    drawable.curCanvas_ = processor->GetCanvas().get();

    // OffScreenRender will change the drawable.curCanvas_'s target, so you have to use a reference
    auto& curCanvas = drawable.curCanvas_;
    if (!curCanvas) {
        RS_LOGE("RSMultiScreenUtil::%{public}s canvas is null!", __func__);
        return;
    }
    curCanvas->SetOnMultipleScreen(true); // use for hdr content tmo to fixed sdr nits
    /*
     * If Top and Left in curVisibleRect_ are not zero,that means the regional screen mirror position may be offset,
     * and the dirty area location may be incorrect. Need to disable the dirty area.
     */
    const auto& curVisibleRect = drawable.curVisibleRect_;
    if (!uniParam.IsVirtualDirtyEnabled() ||
        (enableVisibleRect && (curVisibleRect.GetTop() > 0 || curVisibleRect.GetLeft() > 0))) {
        std::vector<RectI> emptyRects = {};
        processor->SetRoiRegionToCodec(emptyRects);
    } else {
        auto dirtyRects = drawable.CalculateVirtualDirty(processor, *screenDrawable, params,
            slrManager ? slrManager->GetScaleMatrix() : processor->GetCanvasMatrix());
        rsDirtyRectsDfx.SetVirtualDirtyRects(dirtyRects, screenParams->GetScreenInfo());
        if (processor->GetDisplaySkipInMirror()) {
            RS_TRACE_NAME_FMT("%s, display node skip", __func__);
            curCanvas->RestoreToCount(0);
            return;
        }
    }
    // Clean up the content of the previous frame
    curCanvas->Clear(Drawing::Color::COLOR_TRANSPARENT);

    // If the self-drawing node is not fullscreen, need to re-adaptation curCanvas_ clear
    if (RSUniRenderUtil::ProcessSingleSelfDrawingNode(*curCanvas, *mirrorSourceScreenParams, params)) {
        return;
    }
    processor->CanvasClipRegionForUniscaleMode();
    RSUniRenderThread::SetCaptureParam(CaptureParam(false, false, true));

    auto mirrorSourceScreenInfo = mirrorSourceScreenParams->GetScreenInfo();
    float mirrorSourceSamplingScale = mirrorSourceScreenInfo.isSamplingOn ? mirrorSourceScreenInfo.samplingScale : 1.0f;

    curCanvas->Save();
    if (slrManager) {
        auto scaleNum = slrManager->GetScaleNum() * mirrorSourceSamplingScale;
        curCanvas->Scale(scaleNum, scaleNum);
    } else if (mirrorSourceScreenInfo.isSamplingOn) {
        curCanvas->Scale(mirrorSourceSamplingScale, mirrorSourceSamplingScale);
    }
    std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr> hwcNodes;
    std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr> hwcTopNodes;
    RSUniRenderUtil::CollectHardwareEnabledNodesByDisplayNodeId(hwcNodes, hwcTopNodes,
        mirrorSourceDisplayDrawable->GetId());
    RSUniRenderUtil::AdjustZOrderAndDrawSurfaceNode(hwcNodes, *curCanvas, *mirrorSourceScreenParams);
    curCanvas->Restore();

    if (cacheImage && RSSystemProperties::GetDrawMirrorCacheImageEnabled()) {
        RS_TRACE_NAME_FMT("%s, use cacheImage, hwcNodes[%zu], hwcTopNodes[%zu], slrManager[%d]",
            __func__, hwcNodes.size(), hwcTopNodes.size(), slrManager != nullptr);
        if (!enableVisibleRect) {
            processor->ProcessCacheImage(*cacheImage);
        } else {
            RSUniRenderUtil::ProcessCacheImageRect(*curCanvas, *cacheImage, curVisibleRect,
                Drawing::Rect(0, 0, curVisibleRect.GetWidth(), curVisibleRect.GetHeight()));
        }
    } else {
        RS_TRACE_NAME_FMT("%s, use displaySurface, hwcNodes[%zu], hwcTopNodes[%zu], slrManager[%d]",
            __func__, hwcNodes.size(), hwcTopNodes.size(), slrManager != nullptr);
        processor->ProcessScreenSurfaceForRenderThread(*mirrorSourceScreenDrawable);
    }

    curCanvas->Save();
    if (slrManager) {
        auto scaleNum = slrManager->GetScaleNum() * mirrorSourceSamplingScale;
        curCanvas->Scale(scaleNum, scaleNum);
    } else if (mirrorSourceScreenInfo.isSamplingOn) {
        curCanvas->Scale(mirrorSourceSamplingScale, mirrorSourceSamplingScale);
    }
    RSUniRenderUtil::AdjustZOrderAndDrawSurfaceNode(hwcTopNodes, *curCanvas, *mirrorSourceScreenParams);
    curCanvas->Restore();

#ifdef RS_PROFILER_ENABLED
    RSCaptureRecorder::GetInstance().EndInstantCapture(SkpCaptureType::IMG_CACHED);
#endif
    RSUniRenderThread::ResetCaptureParam();
    uniParam.SetOpDropped(isOpDropped);
    // Restore the initial state of the canvas to avoid state accumulation
    curCanvas->RestoreToCount(0);
    rsDirtyRectsDfx.OnDrawVirtual(*curCanvas);
}

void RSMultiScreenUtil::DrawVirtualMirrorRebuild(
    RSLogicalDisplayRenderNodeDrawable& drawable,
    RSLogicalDisplayRenderParams& params,
    const std::shared_ptr<RSUniRenderVirtualProcessor>& processer,
    RSRenderThreadParams& uniParam)
{
    // uniParam/drawable/mirrorSourceDisplayParams not null in caller
    RS_TRACE_FUNC();

    auto [multiScreenParams, isValid] = RSMultiScreenUtil::GetMultiScreenParams(params);
    if (!isValid) {
        RS_LOGE("RSMultiScreenUtil::%{public}s params are invalid", __func__);
        return;
    }

    auto& [screenDrawable, screenParams, mirrorSourceDisplayDrawable, mirrorSourceDisplayParams,
        mirrorSourceScreenDrawable, mirrorSourceScreenParams] = multiScreenParams;
    drawable.curCanvas_ = processer->GetCanvas().get();

    // OffScreenRender will change the drawable.curCanvas_'s target, so you have to use a reference
    auto& curCanvas = drawable.curCanvas_;
    if (!curCanvas) {
        RS_LOGE("RSMultiScreenUtil::%{public}s canvas is null!", __func__);
        return;
    }
    bool enableVisibleRect = drawable.enableVisibleRect_;
    // for HDR
    if (screenParams->GetHDRPresent()) {
        RS_LOGD("RSMultiScreenUtil::%{public}s HDRCast isHDREnabledVirtualScreen true", __func__);
        curCanvas->SetHDREnabledVirtualScreen(true);
        curCanvas->SetHdrOn(true);
    } else {
        curCanvas->SetOnMultipleScreen(true);
    }
    curCanvas->SetDisableFilterCache(true);
    if (params.GetVirtualScreenMuteStatus() || RSSpecialLayerUtils::NeedProcessSecLayerInDisplay(
        enableVisibleRect, *screenParams, params, *mirrorSourceDisplayParams)) {
#ifdef RS_PROFILER_ENABLED
        float width = static_cast<float>(curCanvas->GetWidth());
        float height = static_cast<float>(curCanvas->GetHeight());
        auto canvas = RSCaptureRecorder::GetInstance().TryInstantCapture(width, height, SkpCaptureType::ON_CAPTURE);
        if (canvas) {
            curCanvas->AddCanvas(canvas);
        }
#endif
        std::vector<RectI> emptyRects = {};
        processer->SetRoiRegionToCodec(emptyRects);
        drawable.DrawSecurityMask();
#ifdef RS_PROFILER_ENABLED
        RSCaptureRecorder::GetInstance().EndInstantCapture(SkpCaptureType::ON_CAPTURE);
#endif
        drawable.virtualDirtyNeedRefresh_ = true;
        curCanvas->RestoreToCount(0);
        return;
    }
    curCanvas->Save();
    processer->ScaleMirrorIfNeed(drawable.GetOriginScreenRotation(), *curCanvas);
    const auto& mirrorSourceScreenProperty = mirrorSourceScreenParams->GetScreenProperty();
    auto fixedWidth = mirrorSourceDisplayParams->GetFixedWidth();
    auto fixedHeight = mirrorSourceDisplayParams->GetFixedHeight();
    drawable.UpdateSlrScale(mirrorSourceScreenProperty, fixedWidth, fixedHeight);
    drawable.ScaleCanvasIfNeeded(mirrorSourceScreenProperty);

    RSDirtyRectsDfx rsDirtyRectsDfx(*screenDrawable);
    if (uniParam.IsVirtualDirtyEnabled() && !enableVisibleRect) {
        Drawing::Matrix matrix = curCanvas->GetTotalMatrix();
        std::vector<RectI> dirtyRects = drawable.CalculateVirtualDirty(processer, *screenDrawable, params, matrix);
        rsDirtyRectsDfx.SetVirtualDirtyRects(dirtyRects, screenParams->GetScreenInfo());
        if (processer->GetDisplaySkipInMirror()) {
            RS_TRACE_NAME_FMT("%s, display node skip", __func__);
            curCanvas->RestoreToCount(0);
            return;
        }
    } else {
        std::vector<RectI> emptyRects = {};
        processer->SetRoiRegionToCodec(emptyRects);
    }
    // Clean up the content of the previous frame
    curCanvas->Clear(Drawing::Color::COLOR_TRANSPARENT);

    // If the self-drawing node is not fullscreen, need to re-adaptation curCanvas_ clear
    if (RSUniRenderUtil::ProcessSingleSelfDrawingNode(*curCanvas, *mirrorSourceScreenParams, params)) {
        return;
    }
    bool isSamplingOn = mirrorSourceScreenProperty.GetIsSamplingOn();
    processer->CanvasClipRegionForUniscaleMode(drawable.visibleClipRectMatrix_, isSamplingOn);
    // Set whitelist rect to meta data before concat matrix
    RSSpecialLayerUtils::SetWhiteListRectToMetaData(
        *curCanvas, uniParam, screenParams->GetScreenProperty(), *mirrorSourceDisplayParams, drawable.scaleManager_);
    curCanvas->ConcatMatrix(mirrorSourceDisplayParams->GetMatrix());
    drawable.PrepareOffscreenRender(*mirrorSourceDisplayDrawable, false, false,
        screenParams->GetFixVirtualBuffer10Bit());
    // Add this flag to disable color picking operations during mirror screen redrawing
    curCanvas->SetIsDrawingOffscreenMirror(true);
#ifdef RS_PROFILER_ENABLED
    float width = static_cast<float>(curCanvas->GetWidth());
    float height = static_cast<float>(curCanvas->GetHeight());
    auto canvas = RSCaptureRecorder::GetInstance().TryInstantCapture(width, height, SkpCaptureType::ON_CAPTURE);
    if (canvas) {
        curCanvas->AddCanvas(canvas);
    }
#endif
    // set mirror screen capture param
    // Don't need to scale here since the canvas has been switched from mirror frame to offscreen
    // surface in PrepareOffscreenRender() above. The offscreen surface has the same size as
    // the main display that's why no need additional scale.
    RSUniRenderThread::SetCaptureParam(CaptureParam(false, false, true));
    RSUniRenderThread::GetCaptureParam().virtualScreenId_ = params.GetScreenId();
    RSRenderParams::SetParentSurfaceMatrix(curCanvas->GetTotalMatrix());
    bool isOpDropped = uniParam.IsOpDropped();
    uniParam.SetOpDropped(false); // disable partial render

    // Render mirror content from the source tree
    if (mirrorSourceDisplayDrawable->ShouldPaint()) {
        mirrorSourceDisplayDrawable->offsetX_ = mirrorSourceScreenProperty.GetOffsetX();
        mirrorSourceDisplayDrawable->offsetY_ = mirrorSourceScreenProperty.GetOffsetY();

        // The paintFilterCanvas here is offScreen Canvas
        RSAutoCanvasRestore acr(curCanvas, RSPaintFilterCanvas::SaveType::kCanvasAndAlpha);

        // When set isMirror_ of CaptureParam true, the execution falls through to children's OnCapture function
        mirrorSourceDisplayDrawable->RSRenderNodeDrawable::OnCapture(*curCanvas);
        mirrorSourceDisplayDrawable->DrawAdditionalContent(*curCanvas);
    } else {
        RS_TRACE_NAME_FMT("%s, mirrorSourceDisplayDrawable shouldn't paint", __func__);
        RS_LOGI("RSMultiScreenUtil::%{public}s mirrorSourceDisplayDrawable shouldn't paint!", __func__);
    }

    uniParam.SetOpDropped(isOpDropped);
    RSUniRenderThread::ResetCaptureParam();
#ifdef RS_PROFILER_ENABLED
    RSCaptureRecorder::GetInstance().EndInstantCapture(SkpCaptureType::ON_CAPTURE);
#endif
    Drawing::SamplingOptions samplingOptions(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    drawable.FinishOffscreenRender(samplingOptions, isSamplingOn);
    // Restore the initial state of the canvas to avoid state accumulation
    curCanvas->RestoreToCount(0);
    rsDirtyRectsDfx.OnDrawVirtual(*curCanvas);
}

void RSMultiScreenUtil::DrawVirtualExtend(
    RSLogicalDisplayRenderNodeDrawable& drawable,
    RSLogicalDisplayRenderParams& params,
    const std::shared_ptr<RSUniRenderVirtualProcessor>& processor)
{
    auto [_, screenParam] = drawable.GetScreenParams(params);
    if (!screenParam) {
        RS_LOGE("RSMultiScreenUtil::%{public}s screenParam is nullptr", __func__);
        return;
    }

    if (!processor) {
        RS_LOGE("RSMultiScreenUtil::%{public}s processor is nullptr", __func__);
        return;
    }

    RS_TRACE_NAME_FMT("%s, hdr[%d], isMultiSurface[%d]", __func__, screenParam->GetHDRPresent(),
        processor->IsMultiSurfaceExtendMode());
    RSMultiScreenUtil::DumpRenderStrategy(drawable, params.GetScreenId(), RenderStrategy::DRAW_VIRTUAL_EXTEND, "");

    const RSScreenProperty& screenProperty = screenParam->GetScreenProperty();
    // OffScreenRender will change the drawable.curCanvas_'s target, so you have to use a reference
    auto& canvas = drawable.curCanvas_;
    RSAutoCanvasRestore acr(canvas);
    params.ApplyAlphaAndMatrixToCanvas(*canvas);

    // Check for multi-surface extend mode (only when surfaces have specified regions)
    if (processor->IsMultiSurfaceExtendMode()) {
        if (screenParam->GetHDRPresent()) {
            canvas->SetHDREnabledVirtualScreen(true);
            canvas->SetHdrOn(true);
        }
        // Multi-surface with regions: render to offscreen, then blit regions to each surface
        drawable.PrepareOffscreenRender(drawable, false, false, screenParam->GetFixVirtualBuffer10Bit());
        drawable.RSRenderNodeDrawable::OnDraw(*canvas);

        // Get offscreen image and blit to each surface
        auto offscreenImage = drawable.offscreenSurface_ ? drawable.offscreenSurface_->GetImageSnapshot() : nullptr;
        if (offscreenImage) {
            processor->BlitRegionsToSurfaces(offscreenImage);
        } else {
            RS_LOGE("RSMultiScreenUtil::%{public}s Failed to get offscreen image for multi-surface extend mode",
                __func__);
        }
        // Note: FinishOffscreenRender is not needed here since we're not drawing back to a single canvas
        drawable.canvasBackup_ = nullptr;
    } else if (screenParam->GetHDRPresent()) {
        RS_LOGD("RSMultiScreenUtil::%{public}s HDRCast isHDREnabledVirtualScreen true", __func__);
        canvas->SetHDREnabledVirtualScreen(true);
        canvas->SetHdrOn(true);
        drawable.PrepareOffscreenRender(drawable, false, false, screenParam->GetFixVirtualBuffer10Bit());
        drawable.RSRenderNodeDrawable::OnDraw(*canvas);
        Drawing::SamplingOptions samplingOptions(Drawing::FilterMode::NEAREST, Drawing::MipmapMode::NONE);
        drawable.FinishOffscreenRender(samplingOptions, screenProperty.GetIsSamplingOn());
    } else {
        canvas->SetOnMultipleScreen(true); // use for hdr content tmo to fixed sdr nits
        drawable.RSRenderNodeDrawable::OnDraw(*canvas);
    }
}

void RSMultiScreenUtil::DumpRenderStrategy(
    RSLogicalDisplayRenderNodeDrawable& drawable,
    ScreenId screenId,
    RenderStrategy path,
    const std::string& reason)
{
    if (drawable.lastFrameRenderStrategy_ == path) {
        return;
    }

    RS_TRACE_NAME_FMT("%s ScreenId[%" PRIu64 "] with nodeId[%" PRIu64
        "] render changed from [%d] to [%d] because of [%s]", __func__, screenId,
        drawable.GetId(), static_cast<int>(drawable.lastFrameRenderStrategy_), static_cast<int>(path), reason.c_str());

    RS_LOGI("RSMultiScreenUtil::%{public}s ScreenId[%{public}" PRIu64 "] with nodeId[%{public}" PRIu64
        "] render changed from [%{public}d] to [%{public}d] because of [%{public}s]", __func__, screenId,
        drawable.GetId(), static_cast<int>(drawable.lastFrameRenderStrategy_), static_cast<int>(path), reason.c_str());

    drawable.lastFrameRenderStrategy_ = path;
}

std::pair<MultiScreenParams, bool> RSMultiScreenUtil::GetMultiScreenParams(RSLogicalDisplayRenderParams& params)
{
    MultiScreenParams multiScreenParams;

    auto screenDrawable = std::static_pointer_cast<RSScreenRenderNodeDrawable>(
        params.GetAncestorScreenDrawable().lock());
    if (!screenDrawable || screenDrawable->GetNodeType() != RSRenderNodeType::SCREEN_NODE) {
        RS_LOGE("RSMultiScreenUtil::%{public}s, screenDrawable is nullptr", __func__);
        return { multiScreenParams, false };
    }
    multiScreenParams.screenDrawable = screenDrawable;

    auto screenParams = static_cast<RSScreenRenderParams*>(screenDrawable->GetRenderParams().get());
    if (!screenParams) {
        RS_LOGE("RSMultiScreenUtil::%{public}s, screenParams is nullptr", __func__);
        return { multiScreenParams, false };
    }
    multiScreenParams.screenParams = screenParams;

    auto mirrorSourceDisplayDrawable = std::static_pointer_cast<RSLogicalDisplayRenderNodeDrawable>(
        params.GetMirrorSourceDrawable().lock());
    if (!mirrorSourceDisplayDrawable) {
        RS_LOGE("RSMultiScreenUtil::%{public}s, mirrorSourceDisplayDrawable is nullptr", __func__);
        return { multiScreenParams, false };
    }
    multiScreenParams.mirrorSourceDisplayDrawable = mirrorSourceDisplayDrawable;

    auto mirrorSourceDisplayParams =
        static_cast<RSLogicalDisplayRenderParams*>(mirrorSourceDisplayDrawable->GetRenderParams().get());
    if (!mirrorSourceDisplayParams) {
        RS_LOGE("RSMultiScreenUtil::%{public}s, mirrorSourceDisplayParams is nullptr", __func__);
        return { multiScreenParams, false };
    }
    multiScreenParams.mirrorSourceDisplayParams = mirrorSourceDisplayParams;

    auto mirrorSourceScreenDrawable = std::static_pointer_cast<RSScreenRenderNodeDrawable>(
        mirrorSourceDisplayParams->GetAncestorScreenDrawable().lock());
    if (!mirrorSourceScreenDrawable) {
        RS_LOGE("RSMultiScreenUtil::%{public}s, mirrorSourceScreenDrawable is nullptr", __func__);
        return { multiScreenParams, false };
    }
    multiScreenParams.mirrorSourceScreenDrawable = mirrorSourceScreenDrawable;

    auto mirrorSourceScreenParams =
        static_cast<RSScreenRenderParams*>(mirrorSourceScreenDrawable->GetRenderParams().get());
    if (!mirrorSourceScreenParams) {
        RS_LOGE("RSMultiScreenUtil::%{public}s, mirrorSourceScreenParams is nullptr", __func__);
        return { multiScreenParams, false };
    }
    multiScreenParams.mirrorSourceScreenParams = mirrorSourceScreenParams;

    return { multiScreenParams, true };
}

} // namespace OHOS::Rosen::DrawableV2
