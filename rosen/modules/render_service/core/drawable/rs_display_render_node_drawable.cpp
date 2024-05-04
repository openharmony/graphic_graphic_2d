/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "drawable/rs_display_render_node_drawable.h"

#include <memory>
#include <string>

#include "benchmarks/rs_recording_thread.h"
#include "rs_trace.h"
#include "system/rs_system_parameters.h"

#include "common/rs_optional_trace.h"
#include "common/rs_singleton.h"
#include "drawable/rs_surface_render_node_drawable.h"
#include "memory/rs_tag_tracker.h"
#include "params/rs_display_render_params.h"
#include "params/rs_surface_render_params.h"
#include "pipeline/round_corner_display/rs_rcd_render_manager.h"
#include "pipeline/round_corner_display/rs_round_corner_display.h"
#include "pipeline/rs_base_render_engine.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_processor_factory.h"
#include "pipeline/rs_uifirst_manager.h"
#include "pipeline/rs_uni_render_listener.h"
#include "pipeline/rs_uni_render_thread.h"
#include "pipeline/rs_uni_render_util.h"
#include "platform/common/rs_log.h"
#include "platform/ohos/rs_jank_stats.h"
#include "property/rs_point_light_manager.h"
#include "screen_manager/rs_screen_manager.h"
// dfx
#include "drawable/dfx/rs_dirty_rects_dfx.h"
#include "drawable/dfx/rs_skp_capture_dfx.h"
#include "platform/ohos/overdraw/rs_overdraw_controller.h"
namespace OHOS::Rosen::DrawableV2 {
namespace {
constexpr const char* CLEAR_GPU_CACHE = "ClearGpuCache";
}
class RSOverDrawDfx {
public:
    explicit RSOverDrawDfx(std::shared_ptr<RSPaintFilterCanvas> curCanvas)
    {
        enable_ = RSOverdrawController::GetInstance().IsEnabled() && curCanvas != nullptr;
        curCanvas_ = curCanvas;
        StartOverDraw();
    }
    ~RSOverDrawDfx()
    {
        FinishOverDraw();
    }
private:
    void StartOverDraw()
    {
        if (!enable_) {
            return;
        }
        auto gpuContext = curCanvas_->GetGPUContext();
        if (gpuContext == nullptr) {
            RS_LOGE("RSOverDrawDfx::StartOverDraw failed: need gpu canvas");
            return;
        }

        auto width = curCanvas_->GetWidth();
        auto height = curCanvas_->GetHeight();
        Drawing::ImageInfo info =
            Drawing::ImageInfo { width, height, Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
        overdrawSurface_ = Drawing::Surface::MakeRenderTarget(gpuContext.get(), false, info);
        if (!overdrawSurface_) {
            RS_LOGE("RSOverDrawDfx::StartOverDraw failed: surface is nullptr");
            return;
        }
        overdrawCanvas_ = std::make_shared<Drawing::OverDrawCanvas>(overdrawSurface_->GetCanvas());
        curCanvas_->AddCanvas(overdrawCanvas_.get());
    }
    void FinishOverDraw()
    {
        if (!enable_) {
            return;
        }
        if (!overdrawSurface_) {
            RS_LOGE("RSOverDrawDfx::FinishOverDraw overdrawSurface is nullptr");
            return;
        }
        auto image = overdrawSurface_->GetImageSnapshot();
        if (image == nullptr) {
            RS_LOGE("RSOverDrawDfx::FinishOverDraw image is nullptr");
            return;
        }
        Drawing::Brush brush;
        auto overdrawColors = RSOverdrawController::GetInstance().GetColorArray();
        auto colorFilter = Drawing::ColorFilter::CreateOverDrawColorFilter(overdrawColors.data());
        Drawing::Filter filter;
        filter.SetColorFilter(colorFilter);
        brush.SetFilter(filter);
        curCanvas_->AttachBrush(brush);
        curCanvas_->DrawImage(*image, 0, 0, Drawing::SamplingOptions());
        curCanvas_->DetachBrush();
        overdrawSurface_ = nullptr;
        overdrawCanvas_ = nullptr;
    }

    bool enable_;
    mutable std::shared_ptr<RSPaintFilterCanvas> curCanvas_;
    std::shared_ptr<Drawing::Surface> overdrawSurface_ = nullptr;
    std::shared_ptr<Drawing::OverDrawCanvas> overdrawCanvas_ = nullptr;
};

void DoScreenRcdTask(std::shared_ptr<RSProcessor>& processor, std::unique_ptr<RcdInfo>& rcdInfo,
    ScreenInfo& screenInfo_)
{
    if (screenInfo_.state != ScreenState::HDI_OUTPUT_ENABLE) {
        RS_LOGD("DoScreenRcdTask is not at HDI_OUPUT mode");
        return;
    }
    if (RSSingleton<RoundCornerDisplay>::GetInstance().GetRcdEnable()) {
        RSSingleton<RoundCornerDisplay>::GetInstance().RunHardwareTask(
            [&processor, &rcdInfo]() {
                auto hardInfo = RSSingleton<RoundCornerDisplay>::GetInstance().GetHardwareInfo();
                rcdInfo->processInfo = {processor, hardInfo.topLayer, hardInfo.bottomLayer,
                    hardInfo.resourceChanged};
                RSRcdRenderManager::GetInstance().DoProcessRenderTask(rcdInfo->processInfo);
            }
        );
    }
}

RSDisplayRenderNodeDrawable::Registrar RSDisplayRenderNodeDrawable::instance_;

RSDisplayRenderNodeDrawable::RSDisplayRenderNodeDrawable(std::shared_ptr<const RSRenderNode>&& node)
    : RSRenderNodeDrawable(std::move(node))
{}

RSRenderNodeDrawable::Ptr RSDisplayRenderNodeDrawable::OnGenerate(std::shared_ptr<const RSRenderNode> node)
{
    return new RSDisplayRenderNodeDrawable(std::move(node));
}

static inline std::vector<RectI> MergeDirtyHistory(std::shared_ptr<RSDisplayRenderNode> displayNodeSp,
    int32_t bufferAge, ScreenInfo& screenInfo, RSDirtyRectsDfx& rsDirtyRectsDfx)
{
    auto params = static_cast<RSDisplayRenderParams*>(displayNodeSp->GetRenderParams().get());
    auto& renderThreadParams = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    if (!renderThreadParams) {
        return {};
    }
    auto& curAllSurfaces = params->GetAllMainAndLeashSurfaces();
    auto dirtyManager = displayNodeSp->GetSyncDirtyManager();

    RSUniRenderUtil::MergeDirtyHistory(displayNodeSp, bufferAge, false, true);
    Occlusion::Region dirtyRegion = RSUniRenderUtil::MergeVisibleDirtyRegion(
        curAllSurfaces, RSUniRenderThread::Instance().GetDrawStatusVec(), false, true);
    RSUniRenderUtil::SetAllSurfaceGlobalDityRegion(curAllSurfaces, dirtyManager->GetDirtyRegion());

    // DFX START
    rsDirtyRectsDfx.SetDirtyRegion(dirtyRegion);
    // DFX END

    RectI rect = dirtyManager->GetDirtyRegionFlipWithinSurface();
    auto rects = RSUniRenderUtil::ScreenIntersectDirtyRects(dirtyRegion, screenInfo);
    if (!rect.IsEmpty()) {
        rects.emplace_back(rect);
    }

    return rects;
}

std::unique_ptr<RSRenderFrame> RSDisplayRenderNodeDrawable::RequestFrame(
    std::shared_ptr<RSDisplayRenderNode> displayNodeSp, RSDisplayRenderParams& params,
    std::shared_ptr<RSProcessor> processor) const
{
    RS_TRACE_NAME("RSDisplayRenderNodeDrawable:RequestFrame");
    auto renderEngine = RSUniRenderThread::Instance().GetRenderEngine();
    if (renderEngine == nullptr) {
        RS_LOGE("RSDisplayRenderNodeDrawable::RequestFrame RenderEngine is null!");
        return nullptr;
    }

    if (!processor->Init(*displayNodeSp, params.GetDisplayOffsetX(), params.GetDisplayOffsetY(), INVALID_SCREEN_ID,
        renderEngine, true)) {
        RS_LOGE("RSDisplayRenderNodeDrawable::RequestFrame processor init failed!");
        return nullptr;
    }

    if (!displayNodeSp->IsSurfaceCreated()) {
        sptr<IBufferConsumerListener> listener = new RSUniRenderListener(displayNodeSp);
        if (!displayNodeSp->CreateSurface(listener)) {
            RS_LOGE("RSDisplayRenderNodeDrawable::RequestFrame CreateSurface failed");
            return nullptr;
        }
    }

    auto rsSurface = displayNodeSp->GetRSSurface();
    if (!rsSurface) {
        RS_LOGE("RSDisplayRenderNodeDrawable::RequestFrame No RSSurface found");
        return nullptr;
    }
    auto bufferConfig = RSBaseRenderUtil::GetFrameBufferRequestConfig(params.GetScreenInfo(), true, false,
        params.GetNewColorSpace(), params.GetNewPixelFormat());
    RS_LOGD("RequestFrame colorspace is %{public}d, pixelformat is %{public}d", params.GetNewColorSpace(),
        params.GetNewPixelFormat());
    auto renderFrame = renderEngine->RequestFrame(std::static_pointer_cast<RSSurfaceOhos>(rsSurface), bufferConfig);
    if (!renderFrame) {
        RS_LOGE("RSDisplayRenderNodeDrawable::RequestFrame renderEngine requestFrame is null");
        return nullptr;
    }

    return renderFrame;
}

static inline Drawing::Region GetFilpedRegion(std::vector<RectI>& rects, ScreenInfo& screenInfo)
{
    Drawing::Region region;

    for (auto& r : rects) {
        int32_t topAfterFilp = 0;
#ifdef RS_ENABLE_VK
        topAfterFilp = (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
                           RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR)
                           ? r.top_
                           : static_cast<int32_t>(screenInfo.GetRotatedHeight()) - r.GetBottom();
#else
        topAfterFilp = static_cast<int32_t>(screenInfo.GetRotatedHeight()) - r.GetBottom();
#endif
        Drawing::Region tmpRegion;
        tmpRegion.SetRect(Drawing::RectI(r.left_, topAfterFilp, r.left_ + r.width_, topAfterFilp + r.height_));
        RS_OPTIONAL_TRACE_NAME_FMT("GetFilpedRegion orig ltrb[%d %d %d %d] to fliped rect ltrb[%d %d %d %d]",
            r.left_, r.top_, r.left_ + r.width_, r.top_ + r.height_, r.left_, topAfterFilp, r.left_ + r.width_,
            topAfterFilp + r.height_);
        region.Op(tmpRegion, Drawing::RegionOp::UNION);
    }
    return region;
}

static inline void ClipRegion(Drawing::Canvas& canvas, Drawing::Region& region)
{
    if (region.IsEmpty()) {
        // [planning] Remove this after frame buffer can cancel
        canvas.ClipRect(Drawing::Rect());
    } else if (region.IsRect()) {
        canvas.ClipRegion(region);
        canvas.Clear(Drawing::Color::COLOR_TRANSPARENT);
    } else {
        RS_TRACE_NAME("RSDisplayDrawable: clipPath");
#ifdef RS_ENABLE_VK
        if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
            RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
            canvas.ClipRegion(region);
        } else {
            Drawing::Path dirtyPath;
            region.GetBoundaryPath(&dirtyPath);
            canvas.ClipPath(dirtyPath, Drawing::ClipOp::INTERSECT, true);
        }
#else
        Drawing::Path dirtyPath;
        region.GetBoundaryPath(&dirtyPath);
        canvas.ClipPath(dirtyPath, Drawing::ClipOp::INTERSECT, true);
#endif
    }
}

bool RSDisplayRenderNodeDrawable::CheckDisplayNodeSkip(std::shared_ptr<RSDisplayRenderNode> displayNode,
    RSDisplayRenderParams* params, std::shared_ptr<RSProcessor> processor)
{
    if (displayNode->GetSyncDirtyManager()->IsCurrentFrameDirty() ||
        (params->GetMainAndLeashSurfaceDirty() || RSUifirstManager::Instance().HasDoneNode())) {
        return false;
    }

    RS_LOGD("DisplayNode skip");
    RS_TRACE_NAME("DisplayNode skip");
#ifdef OHOS_PLATFORM
    RSUniRenderThread::Instance().SetSkipJankAnimatorFrame(true);
#endif
    if (!RSUniRenderThread::Instance().GetRSRenderThreadParams()->GetForceCommitLayer()) {
        RS_TRACE_NAME("DisplayNodeSkip skip commit");
        return true;
    }

    if (!processor->Init(*displayNode, params->GetDisplayOffsetX(), params->GetDisplayOffsetY(), INVALID_SCREEN_ID,
        RSUniRenderThread::Instance().GetRenderEngine(), true)) {
        RS_LOGE("RSDisplayRenderNodeDrawable::CheckDisplayNodeSkip processor init failed");
        return false;
    }

    auto& hardwareNodes = RSUniRenderThread::Instance().GetRSRenderThreadParams()->GetHardwareEnabledTypeNodes();
    for (const auto& surfaceNode : hardwareNodes) {
        if (surfaceNode == nullptr) {
            continue;
        }
        auto params = static_cast<RSSurfaceRenderParams*>(surfaceNode->GetRenderParams().get());
        if (params->GetHardwareEnabled()) {
            processor->CreateLayer(*surfaceNode, *params);
        }
    }
    if (!RSMainThread::Instance()->WaitHardwareThreadTaskExecute()) {
        RS_LOGW("RSDisplayRenderNodeDrawable::CheckDisplayNodeSkip: hardwareThread task has too many to Execute");
    }
    processor->ProcessDisplaySurface(*displayNode);
    // planning: commit RCD layers
    processor->PostProcess();
    return true;
}

void RSDisplayRenderNodeDrawable::RemoveClearMemoryTask() const
{
    auto& unirenderThread = RSUniRenderThread::Instance();
    unirenderThread.RemoveTask(CLEAR_GPU_CACHE);
}

void RSDisplayRenderNodeDrawable::PostClearMemoryTask() const
{
    auto& unirenderThread = RSUniRenderThread::Instance();
    unirenderThread.ClearMemoryCache(unirenderThread.GetClearMoment(), unirenderThread.GetClearMemDeeply());
}

void RSDisplayRenderNodeDrawable::OnDraw(Drawing::Canvas& canvas)
{
    // canvas will generate in every request frame
    (void)canvas;

    auto renderNode = renderNode_.lock();
    if (renderNode == nullptr) {
        RS_LOGE("RSDisplayRenderNodeDrawable::OnDraw render node is null!");
        return;
    }

    auto params = static_cast<RSDisplayRenderParams*>(GetRenderParams().get());
    if (!params) {
        RS_LOGE("RSDisplayRenderNodeDrawable::OnDraw params is null!");
        return;
    }
    RS_LOGD("RSDisplayRenderNodeDrawable::OnDraw params %s", params->ToString().c_str());

    isDrawingCacheEnabled_ = RSSystemParameters::GetDrawingCacheEnabled();
    isDrawingCacheDfxEnabled_ = RSSystemParameters::GetDrawingCacheEnabledDfx();
    {
        std::lock_guard<std::mutex> lock(drawingCacheInfoMutex_);
        drawingCacheInfos_.clear();
    }

    // check rotation for point light
    constexpr int ROTATION_NUM = 4;
    auto screenRotation = params->GetScreenRotation();
    if (RSSystemProperties::IsFoldScreenFlag() && params->GetScreenId() == 0) {
        screenRotation = static_cast<ScreenRotation>((static_cast<int>(screenRotation) + 1) % ROTATION_NUM);
    }
    RSPointLightManager::Instance()->SetScreenRotation(screenRotation);

    auto nodeSp = std::const_pointer_cast<RSRenderNode>(renderNode);
    auto displayNodeSp = std::static_pointer_cast<RSDisplayRenderNode>(nodeSp);
    RS_TRACE_NAME("RSDisplayRenderNodeDrawable[" + std::to_string(params->GetScreenId()) + "]" +
                  displayNodeSp->GetSyncDirtyManager()->GetCurrentFrameDirtyRegion().ToString().c_str());
    RS_LOGD("RSDisplayRenderNodeDrawable::OnDraw node: %{public}" PRIu64 ", child size:%{public}u", params->GetId(),
        displayNodeSp->GetChildrenCount());
    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    if (!screenManager) {
        RS_LOGE("RSDisplayRenderNodeDrawable::OnDraw ScreenManager is nullptr");
        return;
    }
    ScreenInfo curScreenInfo = screenManager->QueryScreenInfo(params->GetScreenId());
    // skip frame according to skipFrameInterval value of SetScreenSkipFrameInterval interface
    if (nodeSp->SkipFrame(curScreenInfo.skipFrameInterval)) {
        RS_TRACE_NAME("SkipFrame, screenId:" + std::to_string(params->GetScreenId()));
        screenManager->ForceRefreshOneFrameIfNoRNV();
        return;
    }

    auto screenInfo = params->GetScreenInfo();
    SetVirtualScreenType(*displayNodeSp, screenInfo);
    auto processor = RSProcessorFactory::CreateProcessor(params->GetCompositeType());
    if (!processor) {
        RS_LOGE("RSDisplayRenderNodeDrawable::OnDraw RSProcessor is null!");
        return;
    }

    auto uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams().get();
    if (!uniParam) {
        RS_LOGE("RSDisplayRenderNodeDrawable::OnDraw uniParam is null");
        return;
    }
    canvasRotation_ = screenManager->GetCanvasRotation(params->GetScreenId());

    auto mirroredNode = params->GetMirrorSource().lock();
    if (!mirroredNode && displayNodeSp->GetCacheImgForCapture()) {
        mirroredNode->SetCacheImgForCapture(nullptr);
    }
    if (mirroredNode ||
        displayNodeSp->GetCompositeType() == RSDisplayRenderNode::CompositeType::UNI_RENDER_EXPAND_COMPOSITE) {
        auto renderEngine = RSUniRenderThread::Instance().GetRenderEngine();
        if (renderEngine == nullptr) {
            RS_LOGE("RSDisplayRenderNodeDrawable::OnDraw RenderEngine is null!");
            return;
        }
        if (!processor->Init(*displayNodeSp, params->GetDisplayOffsetX(), params->GetDisplayOffsetY(),
            mirroredNode ? mirroredNode->GetScreenId() : INVALID_SCREEN_ID, renderEngine)) {
            RS_LOGE("RSDisplayRenderNodeDrawable::OnDraw processor init failed!");
            return;
        }
        if (mirroredNode) {
            DrawMirrorScreen(*displayNodeSp, *params, processor);
        } else {
            auto expandProcessor = std::static_pointer_cast<RSUniRenderVirtualProcessor>(processor);
            DrawExpandScreen(*expandProcessor);
        }
        processor->PostProcess();
        return;
    }

    if (uniParam->IsOpDropped() && CheckDisplayNodeSkip(displayNodeSp, params, processor)) {
        RSMainThread::Instance()->SetFrameIsRender(false);
        RSUniRenderThread::Instance().DvsyncRequestNextVsync();
        return;
    }
    RSMainThread::Instance()->SetFrameIsRender(true);
    RSUniRenderThread::Instance().DvsyncRequestNextVsync();

    ScreenId screenId = curScreenInfo.id;
    bool hdrPresent = params->GetHDRPresent();
    RS_LOGD("SetHDRPresent: %{public}d OnDraw", hdrPresent);
    if (hdrPresent) {
        params->SetNewPixelFormat(GRAPHIC_PIXEL_FMT_RGBA_1010102);
    }
    // displayNodeSp to get  rsSurface witch only used in renderThread
    auto renderFrame = RequestFrame(displayNodeSp, *params, processor);
    if (!renderFrame) {
        RS_LOGE("RSDisplayRenderNodeDrawable::OnDraw failed to request frame");
        return;
    }

    RSDirtyRectsDfx rsDirtyRectsDfx(displayNodeSp);
    std::vector<RectI> damageRegionrects;
    if (uniParam->IsPartialRenderEnabled()) {
        damageRegionrects = MergeDirtyHistory(displayNodeSp, renderFrame->GetBufferAge(), screenInfo, rsDirtyRectsDfx);
        uniParam->Reset();
        if (!uniParam->IsRegionDebugEnabled()) {
            renderFrame->SetDamageRegion(damageRegionrects);
        }
    }

    auto drSurface = renderFrame->GetFrame()->GetSurface();
    if (!drSurface) {
        RS_LOGE("RSDisplayRenderNodeDrawable::OnDraw DrawingSurface is null");
        return;
    }

    curCanvas_ = std::make_shared<RSPaintFilterCanvas>(drSurface.get());
    if (!curCanvas_) {
        RS_LOGE("RSDisplayRenderNodeDrawable::OnDraw failed to create canvas");
        return;
    }

    curCanvas_->SetHDRPresent(hdrPresent);
    if (hdrPresent) {
        curCanvas_->SetBrightnessRatio(0.5f);
        curCanvas_->SetScreenId(screenId);
    }
    curCanvas_->SetCurDisplayNode(displayNodeSp);

    RemoveClearMemoryTask();
    // canvas draw
    {
        RSOverDrawDfx rsOverDrawDfx(curCanvas_);
        RSSkpCaptureDfx capture(curCanvas_);
        Drawing::AutoCanvasRestore acr(*curCanvas_, true);
        curCanvas_->ConcatMatrix(params->GetMatrix());
        if (uniParam->IsOpDropped()) {
            auto region = GetFilpedRegion(damageRegionrects, screenInfo);
            ClipRegion(*curCanvas_, region);
        } else {
            curCanvas_->Clear(Drawing::Color::COLOR_TRANSPARENT);
        }
        if (params->IsRotationChanged()) {
            // draw black background in rotation for camera
            curCanvas_->Clear(Drawing::Color::COLOR_BLACK);
        }
        SetHighContrastIfEnabled(*curCanvas_);
        RSRenderNodeDrawable::OnDraw(*curCanvas_);
        DrawWatermarkIfNeed(*displayNodeSp, *curCanvas_);
        DrawCurtainScreen(*displayNodeSp, *curCanvas_);
        // switch color filtering
        SwitchColorFilter(*curCanvas_);
    }
    PostClearMemoryTask();
    rsDirtyRectsDfx.OnDraw(curCanvas_);

    if (isDrawingCacheEnabled_ && isDrawingCacheDfxEnabled_) {
        for (const auto& [rect, updateTimes] : drawingCacheInfos_) {
            std::string extraInfo = ", updateTimes:" + std::to_string(updateTimes);
            RSUniRenderUtil::DrawRectForDfx(*curCanvas_, rect, Drawing::Color::COLOR_GREEN, 0.2f, extraInfo);
        }
    }

    RS_TRACE_BEGIN("RSDisplayRenderNodeDrawable Flush");
    renderFrame->Flush();
    RS_TRACE_END();

    // process round corner display
    auto rcdInfo = std::make_unique<RcdInfo>();
    DoScreenRcdTask(processor, rcdInfo, screenInfo);

    RS_TRACE_BEGIN("RSDisplayRenderNodeDrawable CommitLayer");
    auto& hardwareNodes = RSUniRenderThread::Instance().GetRSRenderThreadParams()->GetHardwareEnabledTypeNodes();
    float globalZOrder = 0.f;
    for (const auto& surfaceNode : hardwareNodes) {
        if (surfaceNode == nullptr) {
            continue;
        }
        auto params = static_cast<RSSurfaceRenderParams*>(surfaceNode->GetRenderParams().get());
        if (params->GetHardwareEnabled()) {
            processor->CreateLayer(*surfaceNode, *params);
            globalZOrder++;
        }
    }
    displayNodeSp->SetGlobalZOrder(globalZOrder);
    processor->ProcessDisplaySurface(*displayNodeSp);
    processor->PostProcess();
    RS_TRACE_END();
}

void RSDisplayRenderNodeDrawable::DrawMirrorScreen(RSDisplayRenderNode& displayNodeSp,
    RSDisplayRenderParams& params, std::shared_ptr<RSProcessor> processor)
{
    auto mirroredNode = params.GetMirrorSource().lock();
    auto mirroredParams = static_cast<RSDisplayRenderParams*>(mirroredNode->GetRenderParams().get());
    auto hasSecSurface = mirroredParams->GetDisplayHasSecSurface();
    auto hasSkipSurface = mirroredParams->GetDisplayHasSkipSurface();
    auto hasProtectedSurface = mirroredParams->GetDisplayHasProtectedSurface();
    auto hasCaptureWindow = mirroredParams->GethasCaptureWindow();
    auto mirroredProcessor = std::static_pointer_cast<RSUniRenderVirtualProcessor>(processor);
    bool hasSpicalLayer = (hasSecSurface[mirroredNode->GetScreenId()] || hasSkipSurface[mirroredNode->GetScreenId()] ||
        hasProtectedSurface[mirroredNode->GetScreenId()] || hasCaptureWindow[mirroredNode->GetScreenId()] ||
        !params.GetScreenInfo().filteredAppSet.empty() || RSUniRenderThread::Instance().IsCurtainScreenOn());
    auto& hardwareNodes = RSUniRenderThread::Instance().GetRSRenderThreadParams()->GetHardwareEnabledTypeNodes();

    if (mirroredNode->GetSecurityDisplay() != displayNodeSp.GetSecurityDisplay() &&
        mirroredProcessor && hasSpicalLayer) {
        DrawMirror(displayNodeSp, params, processor, &RSDisplayRenderNodeDrawable::OnCapture);
    } else if (hardwareNodes.size() > 0) {
        DrawMirror(displayNodeSp, params, processor, &RSDisplayRenderNodeDrawable::DrawHardwareEnabledNodes);
    } else {
        mirroredNode->SetOriginScreenRotation(displayNodeSp.GetOriginScreenRotation());
        processor->ProcessDisplaySurface(*mirroredNode);
    }
}

void RSDisplayRenderNodeDrawable::DrawMirror(RSDisplayRenderNode& displayNodeSp, RSDisplayRenderParams& params,
    std::shared_ptr<RSProcessor> processor, DrawFuncPtr drawFunc)
{
    auto mirroredNode = params.GetMirrorSource().lock();
    auto mirroredParams = static_cast<RSDisplayRenderParams*>(mirroredNode->GetRenderParams().get());
    auto hasSecSurface = mirroredParams->GetDisplayHasSecSurface();
    auto hasSkipSurface = mirroredParams->GetDisplayHasSkipSurface();
    auto mirroredProcessor = std::static_pointer_cast<RSUniRenderVirtualProcessor>(processor);

    curCanvas_ = mirroredProcessor->GetCanvas();
    if (curCanvas_ == nullptr) {
        RS_LOGE("RSDisplayRenderNodeDrawable::DrawMirror failed to get canvas.");
        return;
    }
    curCanvas_->SetDisableFilterCache(true);
    if (hasSecSurface[mirroredNode->GetScreenId()]) {
        SetCanvasBlack(*processor);
        return;
    }
    curCanvas_->Save();
    ScaleMirrorIfNeed(displayNodeSp, processor);
    RotateMirrorCanvasIfNeed(displayNodeSp);
    std::shared_ptr<Drawing::Image> cacheImageProcessed = GetCacheImageFromMirrorNode(mirroredNode);
    mirroredNode->SetCacheImgForCapture(nullptr);
    bool noSpecialLayer = (!hasSecSurface[mirroredNode->GetScreenId()] &&
        !hasSkipSurface[mirroredNode->GetScreenId()] && params.GetScreenInfo().filteredAppSet.empty());
    if (cacheImageProcessed && noSpecialLayer) {
        RS_LOGD("RSDisplayRenderNodeDrawable::DrawMirrorScreen, Enable recording optimization.");
        RSUniRenderThread::Instance().GetRSRenderThreadParams()->SetHasCaptureImg(true);
        processCacheImage(*cacheImageProcessed, *mirroredNode, *mirroredProcessor);
    }
    auto mirroredNodeDrawable = std::static_pointer_cast<RSDisplayRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(std::move(mirroredNode)));
    // set mirror screen capture param
    float mirrorScaleX = mirroredProcessor->GetMirrorScaleX();
    float mirrorScaleY = mirroredProcessor->GetMirrorScaleY();
    RSUniRenderThread::SetCaptureParam(CaptureParam(true, false, true, mirrorScaleX, mirrorScaleY));
    RSRenderParams::SetParentSurfaceMatrix(curCanvas_->GetTotalMatrix());
    (mirroredNodeDrawable.get()->*drawFunc)(*curCanvas_);
    RSUniRenderThread::ResetCaptureParam();
    curCanvas_->Restore();
    RSUniRenderThread::Instance().GetRSRenderThreadParams()->SetHasCaptureImg(false);
    RSUniRenderThread::Instance().GetRSRenderThreadParams()->SetStartVisit(false);
}

void RSDisplayRenderNodeDrawable::DrawExpandScreen(RSUniRenderVirtualProcessor& processor)
{
    curCanvas_ = processor.GetCanvas();
    if (curCanvas_ == nullptr) {
        RS_LOGE("RSDisplayRenderNodeDrawable::DrawExpandScreen failed to get canvas.");
        return;
    }
    float scaleX = 1.0f;
    float scaleY = 1.0f;
    // set expand screen capture param(isInCaptureFlag, isSingleSurface, isMirror)
    RSUniRenderThread::SetCaptureParam(CaptureParam(true, false, false, scaleX, scaleY));
    RSRenderNodeDrawable::OnCapture(*curCanvas_);
    RSUniRenderThread::ResetCaptureParam();
}

void RSDisplayRenderNodeDrawable::SetVirtualScreenType(RSDisplayRenderNode& node, const ScreenInfo& screenInfo)
{
    auto mirroredNode = node.GetMirrorSource().lock();
    switch (screenInfo.state) {
        case ScreenState::PRODUCER_SURFACE_ENABLE:
            node.SetCompositeType(mirroredNode ?
                RSDisplayRenderNode::CompositeType::UNI_RENDER_MIRROR_COMPOSITE :
                RSDisplayRenderNode::CompositeType::UNI_RENDER_EXPAND_COMPOSITE);
            break;
        case ScreenState::HDI_OUTPUT_ENABLE:
            node.SetCompositeType(node.IsForceSoftComposite() ?
                RSDisplayRenderNode::CompositeType::SOFTWARE_COMPOSITE :
                RSDisplayRenderNode::CompositeType::UNI_RENDER_COMPOSITE);
            break;
        default:
            RS_LOGD("RSDisplayRenderNodeDrawable::SetVirtualScreenType ScreenState unsupported");
            return;
    }
}

void RSDisplayRenderNodeDrawable::SetCanvasBlack(RSProcessor& processor)
{
    curCanvas_->Clear(Drawing::Color::COLOR_BLACK);
    processor.PostProcess();
    RS_LOGI("RSDisplayRenderNodeDrawable::SetCanvasBlack, set canvas to black because of security layer.");
    curCanvas_->SetDisableFilterCache(false);
}

void RSDisplayRenderNodeDrawable::processCacheImage(Drawing::Image& cacheImageProcessed,
    RSDisplayRenderNode& mirroredNode, RSUniRenderVirtualProcessor& mirroredProcessor)
{
    curCanvas_->Save();
    if (mirroredNode.GetResetRotate()) {
        Drawing::Matrix invertMatrix;
        if (mirroredProcessor.GetScreenTransformMatrix().Invert(invertMatrix)) {
            // If both canvas and skImage have rotated, we need to reset the canvas
            curCanvas_->ConcatMatrix(invertMatrix);
        }
    }
    Drawing::Brush brush;
    brush.SetAntiAlias(true);
    curCanvas_->AttachBrush(brush);
    auto sampling = Drawing::SamplingOptions(Drawing::FilterMode::NEAREST, Drawing::MipmapMode::NEAREST);
    curCanvas_->DrawImage(cacheImageProcessed, 0, 0, sampling);
    curCanvas_->DetachBrush();
    curCanvas_->Restore();
}

std::shared_ptr<Drawing::Image> RSDisplayRenderNodeDrawable::GetCacheImageFromMirrorNode(
    std::shared_ptr<RSDisplayRenderNode> mirrorNode)
{
    auto cacheImage = mirrorNode->GetCacheImgForCapture();
    bool parallelComposition = RSMainThread::Instance()->GetParallelCompositionEnabled();
    auto renderEngine = RSUniRenderThread::Instance().GetRenderEngine();
    if (!parallelComposition || cacheImage == nullptr || renderEngine == nullptr) {
        return cacheImage;
    }
    auto image = std::make_shared<Drawing::Image>();
    if (auto renderContext = renderEngine->GetRenderContext()) {
        auto grContext = renderContext->GetDrGPUContext();
        auto imageBackendTexure = cacheImage->GetBackendTexture(false, nullptr);
        if (grContext != nullptr && imageBackendTexure.IsValid()) {
            Drawing::BitmapFormat bitmapFormat = {Drawing::ColorType::COLORTYPE_RGBA_8888,
                Drawing::AlphaType::ALPHATYPE_PREMUL};
            image->BuildFromTexture(*grContext, imageBackendTexure.GetTextureInfo(),
                Drawing::TextureOrigin::BOTTOM_LEFT, bitmapFormat, nullptr);
        }
    }
    return image;
}

void RSDisplayRenderNodeDrawable::ScaleMirrorIfNeed(RSDisplayRenderNode& node, std::shared_ptr<RSProcessor> processor)
{
    auto screenManager = CreateOrGetScreenManager();
    auto mirroredNode = node.GetMirrorSource().lock();
    auto mirroredParams = static_cast<RSDisplayRenderParams*>(mirroredNode->GetRenderParams().get());
    auto mainScreenInfo = mirroredParams->GetScreenInfo();
    auto mainWidth = static_cast<float>(mainScreenInfo.width);
    auto mainHeight = static_cast<float>(mainScreenInfo.height);
    auto mirrorWidth = node.GetRenderProperties().GetBoundsWidth();
    auto mirrorHeight = node.GetRenderProperties().GetBoundsHeight();
    auto scaleMode = screenManager->GetScaleMode(node.GetScreenId());
    if (canvasRotation_) {
        if ((RSSystemProperties::IsFoldScreenFlag() && mirroredNode->GetScreenId() == 0) ||
            mirroredNode->GetScreenRotation() == ScreenRotation::ROTATION_90 ||
            mirroredNode->GetScreenRotation() == ScreenRotation::ROTATION_270) {
            std::swap(mainWidth, mainHeight);
        }
    } else {
        if ((RSSystemProperties::IsFoldScreenFlag() && mirroredNode->GetScreenId() == 0)) {
            auto oriRotation = node.GetOriginScreenRotation();
            auto curRotation = node.GetScreenRotation();
            auto rotation = static_cast<ScreenRotation>((static_cast<int>(oriRotation) -
                static_cast<int>(curRotation) + SCREEN_ROTATION_NUM) % SCREEN_ROTATION_NUM);
            if (rotation == ScreenRotation::ROTATION_0 ||
                rotation == ScreenRotation::ROTATION_180) {
                std::swap(mainWidth, mainHeight);
            }
        } else {
            if ((node.GetOriginScreenRotation() == ScreenRotation::ROTATION_90 ||
                node.GetOriginScreenRotation() == ScreenRotation::ROTATION_270)) {
                std::swap(mirrorWidth, mirrorHeight);
            }
        }
    }
    // If the width and height not match the main screen, calculate the dstRect.
    if (mainWidth != mirrorWidth || mainHeight != mirrorHeight) {
        curCanvas_->Clear(SK_ColorBLACK);
        auto mirroredProcessor = std::static_pointer_cast<RSUniRenderVirtualProcessor>(processor);
        if (scaleMode == ScreenScaleMode::FILL_MODE) {
            mirroredProcessor->Fill(*curCanvas_, mainWidth, mainHeight, mirrorWidth, mirrorHeight);
        } else if (scaleMode == ScreenScaleMode::UNISCALE_MODE) {
            mirroredProcessor->UniScale(*curCanvas_, mainWidth, mainHeight, mirrorWidth, mirrorHeight);
        }
    }
}

void RSDisplayRenderNodeDrawable::RotateMirrorCanvasIfNeed(RSDisplayRenderNode& node)
{
    auto mirroredNode = node.GetMirrorSource().lock();
    if ((canvasRotation_ && (RSSystemProperties::IsFoldScreenFlag() && mirroredNode->GetScreenId() == 0)) ||
        (!canvasRotation_ && !(RSSystemProperties::IsFoldScreenFlag() && mirroredNode->GetScreenId() == 0))) {
        return;
    }
    auto mirroredParams = static_cast<RSDisplayRenderParams*>(mirroredNode->GetRenderParams().get());
    auto mainScreenInfo = mirroredParams->GetScreenInfo();
    auto mainWidth = static_cast<float>(mainScreenInfo.width);
    auto mainHeight = static_cast<float>(mainScreenInfo.height);
    auto rotation = mirroredNode->GetScreenRotation();
    if (RSSystemProperties::IsFoldScreenFlag() && mirroredNode->GetScreenId() == 0) {
        if (rotation == ScreenRotation::ROTATION_0 || rotation == ScreenRotation::ROTATION_180) {
            std::swap(mainWidth, mainHeight);
        }
        auto oriRotation = node.GetOriginScreenRotation();
        rotation = static_cast<ScreenRotation>((static_cast<int>(oriRotation) -
            static_cast<int>(rotation) + SCREEN_ROTATION_NUM) % SCREEN_ROTATION_NUM);
    }
    if (rotation != ScreenRotation::ROTATION_0) {
        if (rotation == ScreenRotation::ROTATION_90) {
            curCanvas_->Rotate(90, 0, 0); // 90 is the rotate angle
            curCanvas_->Translate(0, -mainHeight);
        } else if (rotation == ScreenRotation::ROTATION_180) {
            // 180 is the rotate angle, calculate half width and half height requires divide by 2
            curCanvas_->Rotate(180, mainWidth / 2, mainHeight / 2);
        } else if (rotation == ScreenRotation::ROTATION_270) {
            curCanvas_->Rotate(270, 0, 0); // 270 is the rotate angle
            curCanvas_->Translate(-mainWidth, 0);
        }
    }
}

void RSDisplayRenderNodeDrawable::OnCapture(Drawing::Canvas& canvas)
{
    auto renderNode = renderNode_.lock();
    if (renderNode == nullptr) {
        RS_LOGE("RSDisplayRenderNodeDrawable::OnCapture render node is null!");
        return;
    }

    auto params = static_cast<RSDisplayRenderParams*>(GetRenderParams().get());
    if (!params) {
        RS_LOGE("RSDisplayRenderNodeDrawable::OnCapture params is null!");
        return;
    }

    auto nodeSp = std::const_pointer_cast<RSRenderNode>(renderNode);
    auto displayNodeSp = std::static_pointer_cast<RSDisplayRenderNode>(nodeSp);

    auto rscanvas = static_cast<RSPaintFilterCanvas*>(&canvas);
    if (!rscanvas) {
        RS_LOGE("RSDisplayRenderNodeDrawable::OnCapture, rscanvas us nullptr");
        return;
    }

    Drawing::AutoCanvasRestore acr(canvas, true);

    if (params->HasSecurityLayer() || params->HasSkipLayer() || params->HasProtectedLayer() ||
        params->HasCaptureWindow() || params->GetHDRPresent() || RSUniRenderThread::Instance().IsCurtainScreenOn()) {
        RS_LOGD("RSDisplayRenderNodeDrawable::OnCapture: params %{public}s \
            process RSDisplayRenderNode(id:[%{public}" PRIu64 "]) Not using UniRender buffer.",
            params->ToString().c_str(), params->GetId());
        RS_TRACE_NAME("RSDisplayRenderNodeDrawable::OnCapture: processRSDisplayRenderNodeDrawable[" +
            std::to_string(params->GetScreenId()) + "] Not using UniRender buffer.");

        // Adding matrix affine transformation logic
        if (!UNLIKELY(RSUniRenderThread::GetCaptureParam().isMirror_)) {
            rscanvas->ConcatMatrix(params->GetMatrix());
        }

        // Currently, capture do not support HDR display
        rscanvas->SetCapture(true);
        RSRenderNodeDrawable::OnCapture(canvas);
        DrawWatermarkIfNeed(*displayNodeSp, *rscanvas);
    } else {
        auto processor = RSProcessorFactory::CreateProcessor(params->GetCompositeType());
        if (!processor) {
            RS_LOGE("RSDisplayRenderNodeDrawable::OnCapture RSProcessor is null!");
            return;
        }

        DrawHardwareEnabledNodes(canvas, displayNodeSp, params);
    }
}

void RSDisplayRenderNodeDrawable::DrawHardwareEnabledNodes(Drawing::Canvas& canvas,
    std::shared_ptr<RSDisplayRenderNode> displayNodeSp, RSDisplayRenderParams* params)
{
    if (displayNodeSp == nullptr || params == nullptr) {
        RS_LOGE("RSDisplayRenderNodeDrawable::DrawHardwareEnabledNodes: invalid displayNode or params");
        return;
    }

    auto rscanvas = static_cast<RSPaintFilterCanvas*>(&canvas);
    if (!rscanvas) {
        RS_LOGE("RSDisplayRenderNodeDrawable::DrawHardwareEnabledNodes, rscanvas us nullptr");
        return;
    }

    FindHardwareEnabledNodes();

    if (displayNodeSp->GetBuffer() == nullptr) {
        RS_LOGE("RSDisplayRenderNodeDrawable::DrawHardwareEnabledNodes: buffer is null!");
        return;
    }

    RS_LOGD("RSDisplayRenderNodeDrawable::DrawHardwareEnabledNodes: params %{public}s \
        process RSDisplayRenderNode(id:[%{public}" PRIu64 "]) using UniRender buffer.",
    params->ToString().c_str(), params->GetId());
    RS_TRACE_NAME("RSDisplayRenderNodeDrawable::DrawHardwareEnabledNodes: processRSDisplayRenderNodeDrawable[" +
        std::to_string(params->GetScreenId()) + "] using UniRender buffer.");

    if (params->GetHardwareEnabledNodes().size() != 0) {
        AdjustZOrderAndDrawSurfaceNode(params->GetHardwareEnabledNodes(), canvas, *params);
    }

    auto renderEngine = RSUniRenderThread::Instance().GetRenderEngine();
    auto drawParams = RSUniRenderUtil::CreateBufferDrawParam(*displayNodeSp, false);

    // Screen capture considering color inversion
    ColorFilterMode colorFilterMode = renderEngine->GetColorFilterMode();
    if (colorFilterMode >= ColorFilterMode::INVERT_COLOR_ENABLE_MODE &&
        colorFilterMode <= ColorFilterMode::INVERT_DALTONIZATION_TRITANOMALY_MODE) {
        RS_LOGD("RSDisplayRenderNodeDrawable::DrawHardwareEnabledNodes: \
            SetColorFilterModeToPaint mode:%{public}d.", static_cast<int32_t>(colorFilterMode));
        RSBaseRenderUtil::SetColorFilterModeToPaint(colorFilterMode, drawParams.paint);
    }

    // To get dump image
    // execute "param set rosen.dumpsurfacetype.enabled 4 && setenforce 0 && param set rosen.afbc.enabled 0"
    RSBaseRenderUtil::WriteSurfaceBufferToPng(drawParams.buffer);
    renderEngine->DrawDisplayNodeWithParams(*rscanvas, *displayNodeSp, drawParams);

    if (params->GetHardwareEnabledTopNodes().size() != 0) {
        AdjustZOrderAndDrawSurfaceNode(params->GetHardwareEnabledTopNodes(), canvas, *params);
    }
}

void RSDisplayRenderNodeDrawable::DrawHardwareEnabledNodes(Drawing::Canvas& canvas)
{
    auto renderNode = renderNode_.lock();
    if (renderNode == nullptr) {
        RS_LOGE("RSDisplayRenderNodeDrawable::DrawHardwareEnabledNodes render node is null!");
        return;
    }

    auto params = static_cast<RSDisplayRenderParams*>(GetRenderParams().get());
    if (!params) {
        RS_LOGE("RSDisplayRenderNodeDrawable::DrawHardwareEnabledNodes params is null!");
        return;
    }

    auto nodeSp = std::const_pointer_cast<RSRenderNode>(renderNode);
    auto displayNodeSp = std::static_pointer_cast<RSDisplayRenderNode>(nodeSp);

    Drawing::AutoCanvasRestore acr(canvas, true);
    DrawHardwareEnabledNodes(canvas, displayNodeSp, params);
}

void RSDisplayRenderNodeDrawable::SwitchColorFilter(RSPaintFilterCanvas& canvas) const
{
    auto renderEngine = RSUniRenderThread::Instance().GetRenderEngine();
    ColorFilterMode colorFilterMode = renderEngine->GetColorFilterMode();
    if (colorFilterMode == ColorFilterMode::INVERT_COLOR_DISABLE_MODE ||
        colorFilterMode >= ColorFilterMode::DALTONIZATION_NORMAL_MODE) {
        return;
    }

    Drawing::AutoCanvasRestore acr(*curCanvas_, true);
    RS_TRACE_NAME_FMT("RSDisplayRenderNodeDrawable::SetColorFilterModeToPaint mode:%d",
        static_cast<int32_t>(colorFilterMode));
    Drawing::Brush brush;
    RSBaseRenderUtil::SetColorFilterModeToPaint(colorFilterMode, brush);
#if defined (RS_ENABLE_GL) || defined (RS_ENABLE_VK)
#ifdef NEW_RENDER_CONTEXT
    RSTagTracker tagTracker(
        renderEngine->GetDrawingContext()->GetDrawingContext(),
        RSTagTracker::TAG_SAVELAYER_COLOR_FILTER);
#else
    RSTagTracker tagTracker(
        renderEngine->GetRenderContext()->GetDrGPUContext(),
        RSTagTracker::TAG_SAVELAYER_COLOR_FILTER);
#endif
#endif
    Drawing::SaveLayerOps slr(nullptr, &brush, Drawing::SaveLayerOps::INIT_WITH_PREVIOUS);
    canvas.SaveLayer(slr);
}

void RSDisplayRenderNodeDrawable::SetHighContrastIfEnabled(RSPaintFilterCanvas& canvas) const
{
    auto renderEngine = RSUniRenderThread::Instance().GetRenderEngine();
    canvas.SetHighContrast(renderEngine->IsHighContrastEnabled());
}

void RSDisplayRenderNodeDrawable::FindHardwareEnabledNodes()
{
    auto displayParams = static_cast<RSDisplayRenderParams*>(GetRenderParams().get());
    if (!displayParams) {
        RS_LOGE("RSDisplayRenderNodeDrawable::FindHardwareEnabledNodes displayParams is null!");
        return;
    }

    displayParams->GetHardwareEnabledTopNodes().clear();
    displayParams->GetHardwareEnabledNodes().clear();
    auto& hardwareNodes = RSUniRenderThread::Instance().GetRSRenderThreadParams()->GetHardwareEnabledTypeNodes();
    for (const auto& surfaceNode : hardwareNodes) {
        if (surfaceNode == nullptr) {
            continue;
        }
        auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceNode->GetRenderParams().get());
        if (!surfaceParams->GetHardwareEnabled()) {
            continue;
        }
        // To get dump image
        // execute "param set rosen.dumpsurfacetype.enabled 4 && setenforce 0 && param set rosen.afbc.enabled 0"
        auto buffer = surfaceParams->GetBuffer();
        RSBaseRenderUtil::WriteSurfaceBufferToPng(buffer, surfaceParams->GetId());
        if (surfaceNode->IsHardwareEnabledTopSurface()) {
            // surfaceNode which should be drawn above displayNode like pointer window
            displayParams->GetHardwareEnabledTopNodes().emplace_back(surfaceNode);
        } else {
            // surfaceNode which should be drawn below displayNode
            displayParams->GetHardwareEnabledNodes().emplace_back(surfaceNode);
        }
    }
}


void RSDisplayRenderNodeDrawable::AdjustZOrderAndDrawSurfaceNode(
    std::vector<std::shared_ptr<RSSurfaceRenderNode>>& nodes,
    Drawing::Canvas& canvas, RSDisplayRenderParams& params) const
{
    if (!RSSystemProperties::GetHardwareComposerEnabled()) {
        RS_LOGW("RSDisplayRenderNodeDrawable::AdjustZOrderAndDrawSurfaceNode: \
            HardwareComposer is not enabled.");
        return;
    }

    // sort the surfaceNodes by ZOrder
    std::stable_sort(
        nodes.begin(), nodes.end(), [](const auto& first, const auto& second) -> bool {
            return first->GetGlobalZOrder() < second->GetGlobalZOrder();
        });

    Drawing::AutoCanvasRestore acr(canvas, true);
    canvas.ConcatMatrix(params.GetMatrix());
    auto rscanvas = static_cast<RSPaintFilterCanvas*>(&canvas);
    if (!rscanvas) {
        RS_LOGE("RSDisplayRenderNodeDrawable::AdjustZOrderAndDrawSurfaceNode, rscanvas us nullptr");
        return;
    }
    // draw hardware-composition nodes
    for (auto& surfaceNode : nodes) {
        Drawing::AutoCanvasRestore acr(canvas, true);
        std::shared_ptr<RSSurfaceRenderNodeDrawable> surfaceNodeDrawable =
            std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
            DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
        auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceNode->GetRenderParams().get());
        if (!surfaceParams) {
            RS_LOGE("RSDisplayRenderNodeDrawable::AdjustZOrderAndDrawSurfaceNode surfaceParams is nullptr");
            continue;
        }
        // SelfDrawingNodes need to use LayerMatrix(totalMatrix) when doing capturing
        auto matrix = surfaceParams->GetLayerInfo().matrix;
        matrix.PostScale(RSUniRenderThread::GetCaptureParam().scaleX_, RSUniRenderThread::GetCaptureParam().scaleY_);
        canvas.SetMatrix(matrix);

        surfaceNodeDrawable->DealWithSelfDrawingNodeBuffer(*surfaceNode, *rscanvas, *surfaceParams);
    }
}

void RSDisplayRenderNodeDrawable::DrawWatermarkIfNeed(
    RSDisplayRenderNode& node, RSPaintFilterCanvas& canvas) const
{
    if (RSUniRenderThread::Instance().GetWatermarkFlag()) {
        RS_TRACE_FUNC();
        sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
        auto screenInfo = screenManager->QueryScreenInfo(node.GetScreenId());
        auto image = RSUniRenderThread::Instance().GetWatermarkImg();
        if (image == nullptr) {
            return;
        }

        Drawing::SaveLayerOps slr(nullptr, nullptr, Drawing::SaveLayerOps::INIT_WITH_PREVIOUS);
        canvas.SaveLayer(slr); // avoid abnormal dsicard
        auto srcRect = Drawing::Rect(0, 0, image->GetWidth(), image->GetHeight());
        auto dstRect = Drawing::Rect(0, 0, screenInfo.width, screenInfo.height);
        Drawing::Brush rectBrush;
        canvas.AttachBrush(rectBrush);
        canvas.DrawImageRect(*image, srcRect, dstRect, Drawing::SamplingOptions(),
            Drawing::SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT);
        canvas.DetachBrush();
        canvas.Restore();
    }
}

void RSDisplayRenderNodeDrawable::DrawCurtainScreen(
    RSDisplayRenderNode& node, RSPaintFilterCanvas& canvas) const
{
    if (!RSUniRenderThread::Instance().IsCurtainScreenOn()) {
        return;
    }
    RS_TRACE_FUNC();
    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    auto screenInfo = screenManager->QueryScreenInfo(node.GetScreenId());
    float screenWidth = static_cast<float>(screenInfo.width);
    float screenHeight = static_cast<float>(screenInfo.height);
    Drawing::Brush brush;
    int maxAlpha = 255;
    brush.SetARGB(maxAlpha, 0, 0, 0); // not transparent black
    canvas.AttachBrush(brush);
    canvas.DrawRect(Drawing::Rect(0, 0, screenWidth, screenHeight));
    canvas.DetachBrush();
}
} // namespace OHOS::Rosen::DrawableV2
