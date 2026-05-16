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

#include "drawable/dfx/rs_dirty_rects_dfx.h"
#include "drawable/rs_logical_display_render_node_drawable.h"
#include "drawable/rs_screen_render_node_drawable.h"
#include "feature/dirty/rs_uni_dirty_compute_util.h"
#include "feature/frame_stability/rs_frame_stability_manager.h"
#include "feature/special_layer/rs_special_layer_utils.h"
#include "feature/pointer_window_manager/rs_pointer_window_manager.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "pipeline/render_thread/rs_uni_render_util.h"
#include "pipeline/render_thread/rs_uni_render_virtual_processor.h"
#include "platform/common/rs_log.h"
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
        drawable.WiredScreenProjection(params, processor);
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

    drawable.DrawMirrorScreen(params, processor);

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
        Drawing::Region clipRegion = RSUniDirtyComputeUtil::GetFlippedRegion(damageRegionRects, screenInfo);
        uniParam->SetClipRegion(clipRegion);
        RSUniDirtyComputeUtil::ClipRegion(*curCanvas, clipRegion);
    }
    curCanvas->Clear(Drawing::Color::COLOR_TRANSPARENT);
    RSUniDirtyComputeUtil::ClearVirtualExpandScreenAccumulatedDirtyRegions(drawable, params);
    drawable.RSRenderNodeDrawable::OnDraw(*curCanvas);
    params.ResetVirtualExpandAccumulatedParams();
    RSUniRenderThread::ResetCaptureParam();
    auto targetSurfaceRenderNodeDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
        params.GetTargetSurfaceRenderNodeDrawable().lock());
    // Sometimes may require setting cacheImg, such as MultiScreenView and SingleAppCast
    if ((targetSurfaceRenderNodeDrawable || params.HasMirrorScreen()) && curCanvas->GetSurface()) {
        RS_TRACE_NAME("DrawExpandScreen cacheImg for capture and multiscreenview");
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

} // namespace OHOS::Rosen::DrawableV2
