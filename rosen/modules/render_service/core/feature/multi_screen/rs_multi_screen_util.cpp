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
        RSMultiScreenUtil::DrawWiredMirrorDisplay(drawable, params, processor);
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

    if (!processor || !processor->UpdateMirrorInfo(drawable)) {
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

    drawable.DrawExpandDisplay(params);

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
        drawable.wiredMirrorRenderFrame_ = drawable.RequestFrame(params, processor);
        if (!drawable.wiredMirrorRenderFrame_) {
            drawable.SetDrawSkipType(DrawSkipType::REQUEST_FRAME_FAIL);
            RS_LOGE("RSMultiScreenUtil::%{public}s failed to request frame", __func__);
            return;
        }
        auto drSurface = drawable.wiredMirrorRenderFrame_->GetFrame()->GetSurface();
        if (!drSurface) {
            RS_LOGE("RSMultiScreenUtil::%{public}s DrawingSurface is null", __func__);
            return;
        }
        drawable.curCanvas_ = std::make_shared<RSPaintFilterCanvas>(drSurface.get());
        drawable.RSRenderNodeDrawable::OnDraw(*drawable.curCanvas_);
        drawable.wiredMirrorRenderFrame_->Flush();
        processor->ProcessScreenSurfaceForRenderThread(drawable);
        RSPointerWindowManager::Instance().HardCursorCreateLayer(processor, drawable.GetId());
        processor->PostProcess();
        bufferGuard.SetAcquireFence(drawable.wiredMirrorRenderFrame_->GetAcquireFence());
        drawable.wiredMirrorRenderFrame_ = nullptr;
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
    if (uniParam->IsVirtualExpandScreenDirtyEnabled() && !uniParam->IsVirtualDirtyDfxEnabled()) {
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
    auto curCanvas = drawable.curCanvas_;
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

void RSMultiScreenUtil::DrawWiredMirrorDisplay(
    RSLogicalDisplayRenderNodeDrawable& drawable,
    RSLogicalDisplayRenderParams& params,
    const std::shared_ptr<RSProcessor>& processor)
{
    RSPaintFilterCanvas* curCanvas = drawable.curCanvas_;
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
        RSMultiScreenUtil::DumpDrawingPath(drawable, params.GetScreenId(), DrawingPath::DRAW_WIRED_MIRROR_REBUILD, "");
        RSMultiScreenUtil::DrawWiredMirrorRebuild(drawable, params, processor);
    } else {
        // cacheImage must be valid when isMirrorSLRCopy_ is true
        drawable.isMirrorSLRCopy_ = !enableVisibleRect && RSSystemProperties::GetSLRScaleEnabled();
        RSMultiScreenUtil::DumpDrawingPath(drawable, params.GetScreenId(), DrawingPath::DRAW_WIRED_MIRROR_COPY, "");
        RSMultiScreenUtil::DrawWiredMirrorCopy(drawable, params);
    }
}

void RSMultiScreenUtil::DrawWiredMirrorCopy(
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

    RSPaintFilterCanvas* curCanvas = drawable.curCanvas_;
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

void RSMultiScreenUtil::DrawWiredMirrorRebuild(
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
    RSPaintFilterCanvas* curCanvas = drawable.curCanvas_;
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

    std::bitset<8> rebuildReasonFlag;
    rebuildReasonFlag.set(0, RSUniRenderThread::Instance().IsColorFilterModeOn());
    rebuildReasonFlag.set(1, mirrorSourceScreenParams->GetHDRPresent());
    rebuildReasonFlag.set(2, cacheImage == nullptr);
    rebuildReasonFlag.set(3, params.GetVirtualScreenMuteStatus());
    rebuildReasonFlag.set(4, mirrorSourceScreenDrawable->IsRenderSkipIfScreenOff());
    rebuildReasonFlag.set(5, screenParams->GetHDRPresent());
    rebuildReasonFlag.set(6, needProcessSpecialLayer);

    RS_TRACE_NAME_FMT("%s, mirrorSourceScreenId[%" PRIu64 "], rebuildReasonFlag[%s], specialLayerType[%d]",
        __func__, mirrorSourceScreenParams->GetScreenId(), rebuildReasonFlag.to_string().c_str(), specialLayerType);

    if (rebuildReasonFlag.any()) {
        RSMultiScreenUtil::DumpDrawingPath(drawable, params.GetScreenId(),
            DrawingPath::DRAW_VIRTUAL_MIRROR_REBUILD, rebuildReasonFlag.to_string());
        virtualProcesser->SetDrawVirtualMirrorCopy(false);
        drawable.DrawMirror(params, virtualProcesser, *uniParam);
    } else {
        RSMultiScreenUtil::DumpDrawingPath(drawable, params.GetScreenId(),
            DrawingPath::DRAW_VIRTUAL_MIRROR_COPY, rebuildReasonFlag.to_string());
        virtualProcesser->SetDrawVirtualMirrorCopy(true);
        drawable.DrawMirrorCopy(params, virtualProcesser, *uniParam);
    }
}

void RSMultiScreenUtil::DumpDrawingPath(
    RSLogicalDisplayRenderNodeDrawable& drawable,
    ScreenId screenId,
    DrawingPath path,
    const std::string& reason)
{
    if (drawable.lastFrameDrawingPath_ == path) {
        return;
    }

    RS_LOGI("RSMultiScreenUtil::%{public}s mirrorScreenId[%{public}" PRIu64
        "] drawing path changed from [%{public}d] to [%{public}d], reason[%{public}s]",
        __func__, screenId, static_cast<int>(drawable.lastFrameDrawingPath_),
        static_cast<int>(path), reason.c_str());

    drawable.lastFrameDrawingPath_ = path;
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
