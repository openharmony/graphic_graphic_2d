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
#include <parameters.h>

#include "benchmarks/rs_recording_thread.h"
#include "rs_trace.h"
#include "system/rs_system_parameters.h"

#include "common/rs_optional_trace.h"
#include "common/rs_singleton.h"
#include "drawable/rs_surface_render_node_drawable.h"
#include "luminance/rs_luminance_control.h"
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
#include "pipeline/sk_resource_manager.h"
#include "pipeline/rs_pointer_render_manager.h"
#include "platform/common/rs_log.h"
#include "platform/ohos/rs_jank_stats.h"
#include "property/rs_point_light_manager.h"
#include "screen_manager/rs_screen_manager.h"
// dfx
#include "drawable/dfx/rs_dirty_rects_dfx.h"
#include "drawable/dfx/rs_skp_capture_dfx.h"
#include "platform/ohos/overdraw/rs_overdraw_controller.h"
#include "utils/performanceCaculate.h"
namespace OHOS::Rosen::DrawableV2 {
namespace {
constexpr const char* CLEAR_GPU_CACHE = "ClearGpuCache";
constexpr const char* DEFAULT_CLEAR_GPU_CACHE = "DefaultClearGpuCache";
constexpr int32_t NO_SPECIAL_LAYER = 0;
constexpr int32_t HAS_SPECIAL_LAYER = 1;
constexpr int32_t CAPTURE_WINDOW = 2; // To be deleted after captureWindow being deleted
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
        overdrawSurface_ = Drawing::Surface::MakeRaster(info);
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
    }

    bool enable_;
    mutable std::shared_ptr<RSPaintFilterCanvas> curCanvas_;
    std::shared_ptr<Drawing::Surface> overdrawSurface_ = nullptr;
    std::shared_ptr<Drawing::OverDrawCanvas> overdrawCanvas_ = nullptr;
};

void DoScreenRcdTask(std::shared_ptr<RSProcessor>& processor, std::unique_ptr<RcdInfo>& rcdInfo,
    const ScreenInfo& screenInfo)
{
    if (screenInfo.state != ScreenState::HDI_OUTPUT_ENABLE) {
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
    auto& curAllSurfaceDrawables = params->GetAllMainAndLeashSurfaceDrawables();
    auto dirtyManager = displayNodeSp->GetSyncDirtyManager();

    RSUniRenderUtil::MergeDirtyHistory(displayNodeSp, bufferAge, false, true);
    Occlusion::Region dirtyRegion = RSUniRenderUtil::MergeVisibleDirtyRegion(
        curAllSurfaceDrawables, RSUniRenderThread::Instance().GetDrawStatusVec(), false);
    const auto clipRectThreshold = RSSystemProperties::GetClipRectThreshold();
    if (clipRectThreshold < 1.f) {
        Occlusion::Region allDirtyRegion{ Occlusion::Rect{ dirtyManager->GetDirtyRegion() } };
        allDirtyRegion.OrSelf(dirtyRegion);
        auto bound = allDirtyRegion.GetBound();
        if (allDirtyRegion.GetSize() > 1 && !bound.IsEmpty() &&
            allDirtyRegion.Area() > bound.Area() * clipRectThreshold) {
            dirtyManager->MergeDirtyRectAfterMergeHistory(bound.ToRectI());
            RS_OPTIONAL_TRACE_NAME_FMT("dirty expand: %s to %s",
                allDirtyRegion.GetRegionInfo().c_str(), bound.GetRectInfo().c_str());
        }
    }
    RSUniRenderUtil::SetAllSurfaceDrawableGlobalDityRegion(curAllSurfaceDrawables, dirtyManager->GetDirtyRegion());

    // DFX START
    rsDirtyRectsDfx.SetDirtyRegion(dirtyRegion);
    // DFX END

    RectI rect = dirtyManager->GetDirtyRegionFlipWithinSurface();
    auto rects = RSUniRenderUtil::ScreenIntersectDirtyRects(dirtyRegion, screenInfo);
    if (!rect.IsEmpty()) {
        rects.emplace_back(rect);
        RectI screenRectI(0, 0, static_cast<int32_t>(screenInfo.phyWidth), static_cast<int32_t>(screenInfo.phyHeight));
        GpuDirtyRegionCollection::GetInstance().UpdateGlobalDirtyInfoForDFX(rect.IntersectRect(screenRectI));
    }

    return rects;
}

static std::vector<RectI> MergeDirtyHistoryInVirtual(RSDisplayRenderNode& displayNode,
    int32_t bufferAge, ScreenInfo& screenInfo)
{
    auto params = static_cast<RSDisplayRenderParams*>(displayNode.GetRenderParams().get());
    auto& renderThreadParams = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    if (!renderThreadParams || !params) {
        return {};
    }
    auto& curAllSurfaceDrawables = params->GetAllMainAndLeashSurfaceDrawables();
    auto dirtyManager = displayNode.GetSyncDirtyManager();

    RSUniRenderUtil::MergeDirtyHistoryInVirtual(displayNode, bufferAge, true);
    Occlusion::Region dirtyRegion = RSUniRenderUtil::MergeVisibleDirtyRegionInVirtual(curAllSurfaceDrawables, true);

    RectI rect = dirtyManager->GetRectFlipWithinSurface(dirtyManager->GetDirtyRegionInVirtual());
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

static void ClipRegion(Drawing::Canvas& canvas, Drawing::Region& region, bool clear = true)
{
    if (region.IsEmpty()) {
        // [planning] Remove this after frame buffer can cancel
        canvas.ClipRect(Drawing::Rect());
    } else if (region.IsRect()) {
        canvas.ClipRegion(region);
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

    // clear canvas after clip region if need
    if (clear && !region.IsEmpty()) {
        canvas.Clear(Drawing::Color::COLOR_TRANSPARENT);
    }
}

bool RSDisplayRenderNodeDrawable::CheckDisplayNodeSkip(std::shared_ptr<RSDisplayRenderNode> displayNode,
    RSDisplayRenderParams* params, std::shared_ptr<RSProcessor> processor)
{
    if (displayNode->GetSyncDirtyManager()->IsCurrentFrameDirty() || params == nullptr ||
        (params->GetMainAndLeashSurfaceDirty() || RSUifirstManager::Instance().HasDoneNode()) ||
        RSMainThread::Instance()->GetDirtyFlag()) {
        return false;
    }

    RS_LOGD("DisplayNode skip");
    RS_TRACE_NAME("DisplayNode skip");
    GpuDirtyRegionCollection::GetInstance().AddSkipProcessFramesNumberForDFX();
#ifdef OHOS_PLATFORM
    RSUniRenderThread::Instance().SetSkipJankAnimatorFrame(true);
#endif
    auto pendingDrawables = RSUifirstManager::Instance().GetPendingPostDrawables();
    if (!RSUniRenderThread::Instance().GetRSRenderThreadParams()->GetForceCommitLayer() &&
        pendingDrawables.size() == 0) {
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

    RSUifirstManager::Instance().CreateUIFirstLayer(processor);

    // commit RCD layers
    auto rcdInfo = std::make_unique<RcdInfo>();
    auto screenInfo = params->GetScreenInfo();
    DoScreenRcdTask(processor, rcdInfo, screenInfo);
    processor->PostProcess();
    return true;
}

void RSDisplayRenderNodeDrawable::SetDisplayNodeSkipFlag(RSRenderThreadParams& uniParam, bool flag)
{
    isDisplayNodeSkipStatusChanged_ = (isDisplayNodeSkip_ != flag);
    isDisplayNodeSkip_ = flag;
    uniParam.SetForceMirrorScreenDirty(isDisplayNodeSkipStatusChanged_ && isDisplayNodeSkip_);
}

void RSDisplayRenderNodeDrawable::OnDraw(Drawing::Canvas& canvas)
{
    // if screen power off, skip on draw
    if (SkipDisplayIfScreenOff()) {
        return;
    }
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

    isDrawingCacheEnabled_ = RSSystemParameters::GetDrawingCacheEnabled();
    isDrawingCacheDfxEnabled_ = RSSystemParameters::GetDrawingCacheEnabledDfx();
    {
        if (isDrawingCacheDfxEnabled_) {
            std::lock_guard<std::mutex> lock(drawingCacheInfoMutex_);
            drawingCacheInfos_.clear();
            cacheUpdatedNodeMap_.clear();
        }
    }

#ifdef DDGR_ENABLE_FEATURE_OPINC
    autoCacheEnable_ = RSSystemProperties::IsDdgrOpincEnable();
    autoCacheDrawingEnable_ = RSSystemProperties::GetAutoCacheDebugEnabled() && autoCacheEnable_;
    autoCacheRenderNodeInfos_.clear();
    opincRootTotalCount_ = 0;
    isOpincDropNodeExt_ = true;
#endif

    // check rotation for point light
    constexpr int ROTATION_NUM = 4;
    auto screenRotation = GetRenderParams()->GetScreenRotation();
    ScreenId paramScreenId = params->GetScreenId();
    if (RSSystemProperties::IsFoldScreenFlag() && paramScreenId == 0) {
        screenRotation = static_cast<ScreenRotation>((static_cast<int>(screenRotation) + 1) % ROTATION_NUM);
    }
    RSPointLightManager::Instance()->SetScreenRotation(screenRotation);

    auto nodeSp = std::const_pointer_cast<RSRenderNode>(renderNode);
    auto displayNodeSp = std::static_pointer_cast<RSDisplayRenderNode>(nodeSp);
    const RectI& dirtyRegion = displayNodeSp->GetSyncDirtyManager()->GetCurrentFrameDirtyRegion();
    RS_TRACE_NAME_FMT("RSDisplayRenderNodeDrawable [%" PRIu64 "] %d %d %d %d", paramScreenId,
        dirtyRegion.left_, dirtyRegion.top_, dirtyRegion.width_, dirtyRegion.height_);
    RS_LOGD("RSDisplayRenderNodeDrawable::OnDraw node: %{public}" PRIu64 ", child size:%{public}u", params->GetId(),
        displayNodeSp->GetChildrenCount());
    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    if (!screenManager) {
        RS_LOGE("RSDisplayRenderNodeDrawable::OnDraw ScreenManager is nullptr");
        return;
    }
    ScreenInfo curScreenInfo = screenManager->QueryScreenInfo(paramScreenId);
    // skip frame according to skipFrameInterval value of SetScreenSkipFrameInterval interface
    if (nodeSp->SkipFrame(curScreenInfo.skipFrameInterval)) {
        RS_TRACE_NAME("SkipFrame, screenId:" + std::to_string(paramScreenId));
        screenManager->ForceRefreshOneFrameIfNoRNV();
        return;
    }

    auto screenInfo = params->GetScreenInfo();
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

    auto mirroredNode = params->GetMirrorSource().lock();
    if (!mirroredNode && displayNodeSp->GetCacheImgForCapture()) {
        displayNodeSp->SetCacheImgForCapture(nullptr);
    }
    if (mirroredNode ||
        displayNodeSp->GetCompositeType() == RSDisplayRenderNode::CompositeType::UNI_RENDER_EXPAND_COMPOSITE) {
        auto renderEngine = RSUniRenderThread::Instance().GetRenderEngine();
        if (renderEngine == nullptr) {
            RS_LOGE("RSDisplayRenderNodeDrawable::OnDraw RenderEngine is null!");
            return;
        }
        bool isRenderThread = true;
        if (!processor->Init(*displayNodeSp, params->GetDisplayOffsetX(), params->GetDisplayOffsetY(),
            mirroredNode ? mirroredNode->GetScreenId() : INVALID_SCREEN_ID, renderEngine, isRenderThread)) {
            RS_LOGE("RSDisplayRenderNodeDrawable::OnDraw processor init failed!");
            return;
        }
        if (mirroredNode) {
            if (displayNodeSp->GetCompositeType() == RSDisplayRenderNode::CompositeType::UNI_RENDER_COMPOSITE) {
                RS_LOGD("RSDisplayRenderNodeDrawable::OnDraw wired screen projection.");
                WiredScreenProjection(displayNodeSp, *params, processor);
                return;
            }
            castScreenEnableSkipWindow_ = screenManager->GetCastScreenEnableSkipWindow(paramScreenId);
            if (castScreenEnableSkipWindow_) {
                RS_LOGD("RSDisplayRenderNodeDrawable::OnDraw, Enable CastScreen SkipWindow.");
                screenManager->GetCastScreenBlackList(currentBlackList_);
            } else {
                RS_LOGD("RSDisplayRenderNodeDrawable::OnDraw, Enable RecordScreen SkipWindow.");
                currentBlackList_ = screenManager->GetVirtualScreenBlackList(paramScreenId);
            }
            uniParam->SetBlackList(currentBlackList_);
            uniParam->SetWhiteList(screenInfo.whiteList);
            RS_LOGD("RSDisplayRenderNodeDrawable::OnDraw Mirror screen.");
            DrawMirrorScreen(displayNodeSp, *params, processor);
            lastBlackList_ = currentBlackList_;
        } else {
            bool isOpDropped = uniParam->IsOpDropped();
            uniParam->SetOpDropped(false);
            auto expandProcessor = RSProcessor::ReinterpretCast<RSUniRenderVirtualProcessor>(processor);
            if (!expandProcessor) {
                RS_LOGE("RSDisplayRenderNodeDrawable::OnDraw expandProcessor is null!");
                return;
            }
            RSDirtyRectsDfx rsDirtyRectsDfx(displayNodeSp, params);
            std::vector<RectI> damageRegionRects;
            // disable expand screen dirty when skipFrameInterval > 1, because the dirty history is incorrect
            if (uniParam->IsVirtualDirtyEnabled() && curScreenInfo.skipFrameInterval <= 1) {
                int32_t bufferAge = expandProcessor->GetBufferAge();
                damageRegionRects = MergeDirtyHistory(displayNodeSp, bufferAge, screenInfo, rsDirtyRectsDfx);
                uniParam->Reset();
                if (!uniParam->IsVirtualDirtyDfxEnabled()) {
                    expandProcessor->SetDirtyInfo(damageRegionRects);
                }
            } else {
                std::vector<RectI> emptyRects = {};
                expandProcessor->SetRoiRegionToCodec(emptyRects);
            }
            rsDirtyRectsDfx.SetVirtualDirtyRects(damageRegionRects, screenInfo);
            DrawExpandScreen(*expandProcessor);
            rsDirtyRectsDfx.OnDrawVirtual(curCanvas_);
            uniParam->SetOpDropped(isOpDropped);
        }
        processor->PostProcess();
        return;
    }

    if (uniParam->IsOpDropped() && CheckDisplayNodeSkip(displayNodeSp, params, processor)) {
        RSMainThread::Instance()->SetFrameIsRender(false);
        SetDisplayNodeSkipFlag(*uniParam, true);
        return;
    }
    SetDisplayNodeSkipFlag(*uniParam, false);
    RSMainThread::Instance()->SetFrameIsRender(true);

    bool isHdrOn = params->GetHDRPresent();
    RS_LOGD("SetHDRPresent: %{public}d OnDraw", isHdrOn);
    if (isHdrOn) {
        params->SetNewPixelFormat(GRAPHIC_PIXEL_FMT_RGBA_1010102);
    }
    RSUniRenderThread::Instance().WaitUntilDisplayNodeBufferReleased(displayNodeSp);
    auto& hardwareNodes = RSUniRenderThread::Instance().GetRSRenderThreadParams()->GetHardwareEnabledTypeNodes();
    for (const auto& surfaceNode : hardwareNodes) {
        auto params = surfaceNode == nullptr ? nullptr :
            static_cast<RSSurfaceRenderParams*>(surfaceNode->GetRenderParams().get());
        if (params != nullptr) {
            params->SetLayerCreated(false);
        }
    }
    // displayNodeSp to get  rsSurface witch only used in renderThread
    auto renderFrame = RequestFrame(displayNodeSp, *params, processor);
    if (!renderFrame) {
        RS_LOGE("RSDisplayRenderNodeDrawable::OnDraw failed to request frame");
        return;
    }

    RSDirtyRectsDfx rsDirtyRectsDfx(displayNodeSp, params);
    std::vector<RectI> damageRegionrects;
    Drawing::Region clipRegion;
    if (uniParam->IsPartialRenderEnabled()) {
        damageRegionrects = MergeDirtyHistory(displayNodeSp, renderFrame->GetBufferAge(), screenInfo, rsDirtyRectsDfx);
        uniParam->Reset();
        clipRegion = GetFilpedRegion(damageRegionrects, screenInfo);
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

    ScreenId screenId = curScreenInfo.id;
    curCanvas_->SetTargetColorGamut(params->GetNewColorSpace());
    curCanvas_->SetScreenId(screenId);
    if (isHdrOn) {
        // 0 means defalut hdrBrightnessRatio
        float hdrBrightnessRatio = RSLuminanceControl::Get().GetHdrBrightnessRatio(screenId, 0);
        curCanvas_->SetBrightnessRatio(hdrBrightnessRatio);
        curCanvas_->SetHDRPresent(isHdrOn);
    }

    // canvas draw
    {
        RSOverDrawDfx rsOverDrawDfx(curCanvas_);
        {
            RSSkpCaptureDfx capture(curCanvas_);
            Drawing::AutoCanvasRestore acr(*curCanvas_, true);

            bool isOpDropped = uniParam->IsOpDropped();
            bool needOffscreen = params->GetNeedOffscreen();
            if (needOffscreen) {
                uniParam->SetOpDropped(false);
                // draw black background in rotation for camera
                curCanvas_->Clear(Drawing::Color::COLOR_BLACK);
                ClearTransparentBeforeSaveLayer();
                PrepareOffscreenRender(*displayNodeSp);
            }

            if (uniParam->IsOpDropped()) {
                uniParam->SetClipRegion(clipRegion);
                ClipRegion(*curCanvas_, clipRegion);
            } else {
                curCanvas_->Clear(Drawing::Color::COLOR_TRANSPARENT);
            }

            if (!needOffscreen) {
                curCanvas_->ConcatMatrix(params->GetMatrix());
            }

            SetHighContrastIfEnabled(*curCanvas_);
            RSRenderNodeDrawable::OnDraw(*curCanvas_);
            DrawCurtainScreen();
            if (needOffscreen) {
                if (canvasBackup_ != nullptr) {
                    Drawing::AutoCanvasRestore acr(*canvasBackup_, true);
                    canvasBackup_->ConcatMatrix(params->GetMatrix());
                    FinishOffscreenRender(
                        Drawing::SamplingOptions(Drawing::FilterMode::NEAREST, Drawing::MipmapMode::NONE));
                    uniParam->SetOpDropped(isOpDropped);
                } else {
                    RS_LOGE("RSDisplayRenderNodeDrawable::OnDraw canvasBackup_ is nullptr");
                }
            }
            // watermark and color filter should be applied after offscreen render.
            DrawWatermarkIfNeed(*params, *curCanvas_);
            SwitchColorFilter(*curCanvas_);
        }
        rsDirtyRectsDfx.OnDraw(curCanvas_);
        if (RSSystemProperties::IsFoldScreenFlag() && !params->IsRotationChanged()) {
            offscreenSurface_ = nullptr;
        }

        specialLayerType_ = GetSpecialLayerType(*params);
        if (uniParam->HasMirrorDisplay() && UNLIKELY(!params->IsRotationChanged()) &&
            curCanvas_->GetSurface() != nullptr && specialLayerType_ == NO_SPECIAL_LAYER) {
            cacheImgForCapture_ = curCanvas_->GetSurface()->GetImageSnapshot();
        }
    }
    RSMainThread::Instance()->SetDirtyFlag(false);

    if (Drawing::PerformanceCaculate::GetDrawingFlushPrint()) {
        RS_LOGI("Drawing Performance Flush start %{public}lld", Drawing::PerformanceCaculate::GetUpTime(false));
    }
    RS_TRACE_BEGIN("RSDisplayRenderNodeDrawable Flush");
    renderFrame->Flush();
    RS_TRACE_END();
    if (Drawing::PerformanceCaculate::GetDrawingFlushPrint()) {
        RS_LOGI("Drawing Performance Flush end %{public}lld", Drawing::PerformanceCaculate::GetUpTime(false));
        Drawing::PerformanceCaculate::ResetCaculateTimeCount();
    }

    // process round corner display
    auto rcdInfo = std::make_unique<RcdInfo>();
    DoScreenRcdTask(processor, rcdInfo, screenInfo);

    if (!RSMainThread::Instance()->WaitHardwareThreadTaskExecute()) {
        RS_LOGW("RSDisplayRenderNodeDrawable::ondraw: hardwareThread task has too many to Execute");
    }

    RS_TRACE_BEGIN("RSDisplayRenderNodeDrawable CommitLayer");
    for (const auto& surfaceNode : hardwareNodes) {
        if (surfaceNode == nullptr) {
            continue;
        }
        auto params = static_cast<RSSurfaceRenderParams*>(surfaceNode->GetRenderParams().get());
        if (params->GetHardwareEnabled()) {
            processor->CreateLayer(*surfaceNode, *params);
        }
    }
    displayNodeSp->SetDirtyRects(damageRegionrects);
    processor->ProcessDisplaySurface(*displayNodeSp);
    RSUifirstManager::Instance().CreateUIFirstLayer(processor);
    processor->PostProcess();
    RS_TRACE_END();

    if (!mirroredNode) {
        RS_TRACE_BEGIN("RSDisplayRenderNodeDrawable ProcessColorPicker");
        RSPointerRenderManager::GetInstance().ProcessColorPicker(processor, curCanvas_->GetGPUContext());
        RSPointerRenderManager::GetInstance().SetCacheImgForPointer(nullptr);
        RS_TRACE_END();
    }
}

void RSDisplayRenderNodeDrawable::DrawMirrorScreen(std::shared_ptr<RSDisplayRenderNode>& displayNodeSp,
    RSDisplayRenderParams& params, std::shared_ptr<RSProcessor> processor)
{
    auto uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams().get();
    if (!uniParam) {
        RS_LOGE("RSDisplayRenderNodeDrawable::DrawMirrorScreen uniParam is null");
        return;
    }

    auto mirroredNode = params.GetMirrorSource().lock();
    if (!mirroredNode) {
        RS_LOGE("RSDisplayRenderNodeDrawable::DrawMirrorScreen mirroredNode is null");
        return;
    }
    auto mirroredParams = static_cast<RSDisplayRenderParams*>(mirroredNode->GetRenderParams().get());
    if (!mirroredParams) {
        RS_LOGE("RSDisplayRenderNodeDrawable::DrawMirrorScreen mirroredParams is null");
        return;
    }
    specialLayerType_ = GetSpecialLayerType(*mirroredParams);
    auto& hardwareNodes = RSUniRenderThread::Instance().GetRSRenderThreadParams()->GetHardwareEnabledTypeNodes();

    auto virtualProcesser = RSProcessor::ReinterpretCast<RSUniRenderVirtualProcessor>(processor);
    if (!virtualProcesser) {
        RS_LOGE("RSDisplayRenderNodeDrawable::DrawMirrorScreen virtualProcesser is null");
        return;
    }

    if (mirroredNode->GetSecurityDisplay() != displayNodeSp->GetSecurityDisplay() &&
        specialLayerType_ != NO_SPECIAL_LAYER) {
        DrawMirror(displayNodeSp, params, virtualProcesser,
            &RSDisplayRenderNodeDrawable::OnCapture, *uniParam);
    } else if (hardwareNodes.size() > 0 && RSSystemProperties::GetHardwareComposerEnabledForMirrorMode()) {
        DrawMirror(displayNodeSp, params, virtualProcesser,
            &RSDisplayRenderNodeDrawable::DrawHardwareEnabledNodes, *uniParam);
    } else {
        DrawMirrorCopy(displayNodeSp, mirroredNode, params, virtualProcesser, *uniParam);
    }
}

int32_t RSDisplayRenderNodeDrawable::GetSpecialLayerType(RSDisplayRenderParams& params)
{
    auto hasGeneralSpecialLayer = params.HasSecurityLayer() || params.HasSkipLayer() || params.HasProtectedLayer() ||
        RSUniRenderThread::Instance().IsCurtainScreenOn() || params.GetHDRPresent();
    if (RSUniRenderThread::GetCaptureParam().isSnapshot_) {
        return hasGeneralSpecialLayer ? HAS_SPECIAL_LAYER :
            (params.HasCaptureWindow() ? CAPTURE_WINDOW : NO_SPECIAL_LAYER);
    }
    auto uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams() ?
       RSUniRenderThread::Instance().GetRSRenderThreadParams().get() : nullptr;
    if (hasGeneralSpecialLayer || (uniParam && !uniParam->GetWhiteList().empty()) || !currentBlackList_.empty()) {
        return HAS_SPECIAL_LAYER;
    } else if (params.HasCaptureWindow()) {
        return CAPTURE_WINDOW;
    }
    return NO_SPECIAL_LAYER;
}

std::vector<RectI> RSDisplayRenderNodeDrawable::CalculateVirtualDirty(RSDisplayRenderNode& displayNode,
    std::shared_ptr<RSUniRenderVirtualProcessor> virtualProcesser,
    RSDisplayRenderParams& params, Drawing::Matrix canvasMatrix)
{
    std::vector<RectI> mappedDamageRegionRects;
    auto uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams().get();
    if (!uniParam) {
        RS_LOGE("RSDisplayRenderNodeDrawable::CalculateVirtualDirty uniParam is null");
        return mappedDamageRegionRects;
    }
    auto mirroredNode = params.GetMirrorSource().lock();
    if (!mirroredNode) {
        RS_LOGE("RSDisplayRenderNodeDrawable::CalculateVirtualDirty mirroredNode is nullptr.");
        return mappedDamageRegionRects;
    }
    auto mirroredNodeDrawable = GetDrawableById(params.GetMirrorSourceId());
    if (!mirroredNodeDrawable) {
        RS_LOGE("RSDisplayRenderNodeDrawable::CalculateVirtualDirty mirroredNodeDrawable is nullptr");
        return mappedDamageRegionRects;
    }
    auto mirroredParams = static_cast<RSDisplayRenderParams*>(mirroredNodeDrawable->GetRenderParams().get());
    if (!mirroredParams) {
        RS_LOGE("RSDisplayRenderNodeDrawable::CalculateVirtualDirty mirroredParams is nullptr");
        return mappedDamageRegionRects;
    }

    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    if (!screenManager) {
        RS_LOGE("RSDisplayRenderNodeDrawable::CalculateVirtualDirty ScreenManager is nullptr");
        return mappedDamageRegionRects;
    }
    ScreenInfo mainScreenInfo = screenManager->QueryScreenInfo(mirroredNode->GetScreenId());
    ScreenInfo curScreenInfo = screenManager->QueryScreenInfo(params.GetScreenId());

    int32_t bufferAge = virtualProcesser->GetBufferAge();
    int32_t actualAge = curScreenInfo.skipFrameInterval ?
        static_cast<int32_t>(curScreenInfo.skipFrameInterval) * bufferAge : bufferAge;
    std::vector<RectI> damageRegionRects = MergeDirtyHistoryInVirtual(*mirroredNode, actualAge, mainScreenInfo);
    std::shared_ptr<RSObjAbsGeometry> tmpGeo = std::make_shared<RSObjAbsGeometry>();
    for (auto& rect : damageRegionRects) {
        RectI mappedRect = tmpGeo->MapRect(rect.ConvertTo<float>(), canvasMatrix);
        mappedDamageRegionRects.emplace_back(mappedRect);
    }
    if (!(lastMatrix_ == canvasMatrix) || !(lastMirrorMatrix_ == mirroredParams->GetMatrix()) ||
        uniParam->GetForceMirrorScreenDirty() || lastBlackList_ != currentBlackList_ ||
        mirroredParams->IsSpecialLayerChanged()) {
        displayNode.GetSyncDirtyManager()->ResetDirtyAsSurfaceSize();
        lastMatrix_ = canvasMatrix;
        lastMirrorMatrix_ = mirroredParams->GetMatrix();
    }
    RectI hwcRect = mirroredNode->GetSyncDirtyManager()->GetHwcDirtyRegion();
    if (!hwcRect.IsEmpty()) {
        RectI mappedHwcRect = tmpGeo->MapRect(hwcRect.ConvertTo<float>(), canvasMatrix);
        displayNode.GetSyncDirtyManager()->MergeDirtyRect(mappedHwcRect);
    }
    displayNode.UpdateDisplayDirtyManager(bufferAge, false, true);
    auto extraDirty = displayNode.GetSyncDirtyManager()->GetDirtyRegion();
    if (!extraDirty.IsEmpty()) {
        mappedDamageRegionRects.emplace_back(extraDirty);
    }
    if (!uniParam->IsVirtualDirtyDfxEnabled()) {
        virtualProcesser->SetDirtyInfo(mappedDamageRegionRects);
    }
    return mappedDamageRegionRects;
}

void RSDisplayRenderNodeDrawable::DrawMirror(std::shared_ptr<RSDisplayRenderNode>& displayNodeSp,
    RSDisplayRenderParams& params, std::shared_ptr<RSUniRenderVirtualProcessor> virtualProcesser,
    DrawFuncPtr drawFunc, RSRenderThreadParams& uniParam)
{
    auto mirroredNode = params.GetMirrorSource().lock();
    if (!mirroredNode) {
        RS_LOGE("RSDisplayRenderNodeDrawable::DrawMirror mirroredNode is null");
        return;
    }

    curCanvas_ = virtualProcesser->GetCanvas();
    if (curCanvas_ == nullptr) {
        RS_LOGE("RSDisplayRenderNodeDrawable::DrawMirror failed to get canvas.");
        return;
    }
    curCanvas_->Clear(Drawing::Color::COLOR_BLACK);
    curCanvas_->SetDisableFilterCache(true);
    auto mirroredParams = static_cast<RSDisplayRenderParams*>(mirroredNode->GetRenderParams().get());
    if (!mirroredParams) {
        RS_LOGE("RSDisplayRenderNodeDrawable::DrawMirror mirroredParams is null");
        return;
    }
    auto hasSecSurface = mirroredParams->GetDisplayHasSecSurface();
    if (hasSecSurface[mirroredNode->GetScreenId()]) {
        std::vector<RectI> emptyRects = {};
        virtualProcesser->SetRoiRegionToCodec(emptyRects);
        SetCanvasBlack(*virtualProcesser);
        return;
    }
    curCanvas_->Save();
    virtualProcesser->ScaleMirrorIfNeed(*displayNodeSp, *curCanvas_);

    RSDirtyRectsDfx rsDirtyRectsDfx(displayNodeSp, &params);
    if (uniParam.IsVirtualDirtyEnabled()) {
        Drawing::Matrix matrix = curCanvas_->GetTotalMatrix();
        std::vector<RectI> dirtyRects = CalculateVirtualDirty(*displayNodeSp, virtualProcesser, params, matrix);
        rsDirtyRectsDfx.SetVirtualDirtyRects(dirtyRects, params.GetScreenInfo());
    } else {
        std::vector<RectI> emptyRects = {};
        virtualProcesser->SetRoiRegionToCodec(emptyRects);
    }

    curCanvas_->ConcatMatrix(mirroredParams->GetMatrix());
    PrepareOffscreenRender(*mirroredNode);

    auto mirroredNodeDrawable = std::static_pointer_cast<RSDisplayRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(mirroredNode));
    // set mirror screen capture param
    // Don't need to scale here since the canvas has been switched from mirror frame to offscreen
    // surface in PrepareOffscreenRender() above. The offscreen surface has the same size as
    // the main display that's why no need additional scale.
    RSUniRenderThread::SetCaptureParam(CaptureParam(false, false, true, 1.0f, 1.0f));

    curCanvas_->Save();
    Drawing::Region clipRegion;
    clipRegion.Clone(uniParam.GetClipRegion());
    ResetRotateIfNeed(*mirroredNode, *virtualProcesser, clipRegion);

    // To be deleted after captureWindow being deleted
    std::shared_ptr<Drawing::Image> cacheImage = mirroredNode->GetCacheImgForCapture();
    mirroredNode->SetCacheImgForCapture(nullptr);
    if (cacheImage && specialLayerType_ == CAPTURE_WINDOW) {
        RS_LOGD("RSDisplayRenderNodeDrawable::DrawMirrorScreen, Enable recording optimization.");
        uniParam.SetHasCaptureImg(true);
        mirroredNodeDrawable->DrawHardwareEnabledNodesMissedInCacheImage(*curCanvas_);
        RSUniRenderUtil::ProcessCacheImage(*curCanvas_, *cacheImage);
    }
    curCanvas_->Restore();
    RSRenderParams::SetParentSurfaceMatrix(curCanvas_->GetTotalMatrix());
    if (uniParam.IsOpDropped() && cacheImage && specialLayerType_ == CAPTURE_WINDOW) {
        ClipRegion(*curCanvas_, clipRegion, false);
    }
    bool isOpDropped = uniParam.IsOpDropped();
    uniParam.SetOpDropped(false); // disable partial render
    (mirroredNodeDrawable.get()->*drawFunc)(*curCanvas_);
    uniParam.SetOpDropped(isOpDropped);
    RSUniRenderThread::ResetCaptureParam();
    FinishOffscreenRender(Drawing::SamplingOptions(Drawing::CubicResampler::Mitchell()));
    curCanvas_->Restore();
    curCanvas_->ResetMatrix();
    rsDirtyRectsDfx.OnDrawVirtual(curCanvas_);
    uniParam.SetHasCaptureImg(false);
    uniParam.SetStartVisit(false);
    uniParam.SetBlackList({});
    uniParam.SetWhiteList({});
}

void RSDisplayRenderNodeDrawable::DrawMirrorCopy(std::shared_ptr<RSDisplayRenderNode>& displayNodeSp,
    std::shared_ptr<RSDisplayRenderNode>& mirroredNode, RSDisplayRenderParams& params,
    std::shared_ptr<RSUniRenderVirtualProcessor> virtualProcesser, RSRenderThreadParams& uniParam)
{
    auto mirroredNodeDrawable = std::static_pointer_cast<RSDisplayRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(mirroredNode));
    if (mirroredNodeDrawable == nullptr) {
        RS_LOGE("RSDisplayRenderNodeDrawable::DrawMirrorCopy mirroredNodeDrawable is nullptr");
        return;
    }
    auto cacheImage = mirroredNodeDrawable->GetCacheImgForCapture();
    mirroredNodeDrawable->SetCacheImgForCapture(nullptr);
    bool isOpDropped = uniParam.IsOpDropped();
    uniParam.SetOpDropped(false);
    mirroredNode->SetOriginScreenRotation(displayNodeSp->GetOriginScreenRotation());
    virtualProcesser->CalculateTransform(*mirroredNode);
    RSDirtyRectsDfx rsDirtyRectsDfx(displayNodeSp, &params);
    if (uniParam.IsVirtualDirtyEnabled()) {
        auto dirtyRects = CalculateVirtualDirty(
            *displayNodeSp, virtualProcesser, params, virtualProcesser->GetCanvasMatrix());
        rsDirtyRectsDfx.SetVirtualDirtyRects(dirtyRects, params.GetScreenInfo());
    } else {
        std::vector<RectI> emptyRects = {};
        virtualProcesser->SetRoiRegionToCodec(emptyRects);
    }
    if (cacheImage) {
        curCanvas_ = virtualProcesser->GetCanvas();
        if (curCanvas_) {
            mirroredNodeDrawable->DrawHardwareEnabledNodesMissedInCacheImage(*curCanvas_);
            RSUniRenderUtil::ProcessCacheImage(*curCanvas_, *cacheImage);
        }
    } else {
        virtualProcesser->ProcessDisplaySurface(*mirroredNode);
        curCanvas_ = virtualProcesser->GetCanvas();
    }
    uniParam.SetOpDropped(isOpDropped);
    if (curCanvas_) {
        curCanvas_->ResetMatrix();
    }
    rsDirtyRectsDfx.OnDrawVirtual(curCanvas_);
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
    // set expand screen capture param(isSnapshot, isSingleSurface, isMirror)
    RSUniRenderThread::SetCaptureParam(CaptureParam(false, false, false, scaleX, scaleY));
    RSRenderNodeDrawable::OnCapture(*curCanvas_);
    RSUniRenderThread::ResetCaptureParam();
}

void RSDisplayRenderNodeDrawable::WiredScreenProjection(std::shared_ptr<RSDisplayRenderNode> displayNodeSp,
    RSDisplayRenderParams& params, std::shared_ptr<RSProcessor> processor)
{
    if (!displayNodeSp) {
        RS_LOGE("RSDisplayRenderNodeDrawable::WiredScreenProjection displayNodeSp is null");
        return;
    }
    RSUniRenderThread::Instance().WaitUntilDisplayNodeBufferReleased(displayNodeSp);
    auto renderFrame = RequestFrame(displayNodeSp, params, processor);
    if (!renderFrame) {
        RS_LOGE("RSDisplayRenderNodeDrawable::WiredScreenProjection failed to request frame");
        return;
    }
    auto drSurface = renderFrame->GetFrame()->GetSurface();
    if (!drSurface) {
        RS_LOGE("RSDisplayRenderNodeDrawable::WiredScreenProjection DrawingSurface is null");
        return;
    }
    curCanvas_ = std::make_shared<RSPaintFilterCanvas>(drSurface.get());
    if (!curCanvas_) {
        RS_LOGE("RSDisplayRenderNodeDrawable::WiredScreenProjection failed to create canvas");
        return;
    }
    auto mirroredNode = params.GetMirrorSource().lock();
    if (!mirroredNode) {
        RS_LOGE("RSDisplayRenderNodeDrawable::WiredScreenProjection mirroredNode is null");
        return;
    }
    curCanvas_->Save();
    ScaleAndRotateMirrorForWiredScreen(*displayNodeSp, *mirroredNode);
    RSDirtyRectsDfx rsDirtyRectsDfx(displayNodeSp, &params);
    std::vector<RectI> damageRegionRects = CalculateVirtualDirtyForWiredScreen(
        *displayNodeSp, renderFrame, params, curCanvas_->GetTotalMatrix());
    rsDirtyRectsDfx.SetVirtualDirtyRects(damageRegionRects, params.GetScreenInfo());
    bool forceCPU = false;
    auto drawParams = RSUniRenderUtil::CreateBufferDrawParam(*mirroredNode, forceCPU);
    auto renderEngine = RSUniRenderThread::Instance().GetRenderEngine();
    drawParams.isMirror = true;
    renderEngine->DrawDisplayNodeWithParams(*curCanvas_, *mirroredNode, drawParams);
    curCanvas_->Restore();
    rsDirtyRectsDfx.OnDrawVirtual(curCanvas_);
    renderFrame->Flush();
    processor->ProcessDisplaySurface(*displayNodeSp);
    processor->PostProcess();
}

std::vector<RectI> RSDisplayRenderNodeDrawable::CalculateVirtualDirtyForWiredScreen(RSDisplayRenderNode& displayNode,
    std::unique_ptr<RSRenderFrame>& renderFrame, RSDisplayRenderParams& params, Drawing::Matrix canvasMatrix)
{
    std::vector<RectI> damageRegionRects;
    auto mirroredNode = params.GetMirrorSource().lock();
    if (!mirroredNode) {
        RS_LOGE("RSDisplayRenderNodeDrawable::CalculateVirtualDirtyForWiredScreen mirroredNode is null");
        return damageRegionRects;
    }
    auto uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams().get();
    if (uniParam == nullptr || !uniParam->IsVirtualDirtyEnabled()) {
        RS_LOGE("RSDisplayRenderNodeDrawable::CalculateVirtualDirtyForWiredScreen invalid uniparam");
        return damageRegionRects;
    }
    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    if (screenManager == nullptr) {
        RS_LOGE("RSDisplayRenderNodeDrawable::CalculateVirtualDirtyForWiredScreen screenManager is null");
        return damageRegionRects;
    }
    int32_t bufferAge = renderFrame->GetBufferAge();
    auto curScreenInfo = params.GetScreenInfo();
    int32_t actualAge = curScreenInfo.skipFrameInterval ?
        static_cast<int32_t>(curScreenInfo.skipFrameInterval) * bufferAge : bufferAge;
    ScreenInfo mainScreenInfo = screenManager->QueryScreenInfo(mirroredNode->GetScreenId());
    std::shared_ptr<RSObjAbsGeometry> tmpGeo = std::make_shared<RSObjAbsGeometry>();
    // merge history dirty and map to mirrored wired screen by matrix
    auto tempDamageRegionRects = MergeDirtyHistoryInVirtual(*mirroredNode, actualAge, mainScreenInfo);
    for (auto& rect : tempDamageRegionRects) {
        RectI mappedRect = tmpGeo->MapRect(rect.ConvertTo<float>(), canvasMatrix);
        damageRegionRects.emplace_back(mappedRect);
    }

    auto syncDirtyManager = displayNode.GetSyncDirtyManager();
    if (syncDirtyManager == nullptr) {
        RS_LOGE("RSDisplayRenderNodeDrawable::CalculateVirtualDirtyForWiredScreen syncDirtyManager is null");
        return damageRegionRects;
    }
    // reset dirty rect as mirrored wired screen size when first time connection or matrix changed
    if (!(lastMatrix_ == canvasMatrix)) {
        syncDirtyManager->ResetDirtyAsSurfaceSize();
        lastMatrix_ = canvasMatrix;
    }
    displayNode.UpdateDisplayDirtyManager(bufferAge, false, true);
    auto extraDirty = syncDirtyManager->GetDirtyRegion();
    if (!extraDirty.IsEmpty()) {
        damageRegionRects.emplace_back(extraDirty);
    }
    if (!uniParam->IsVirtualDirtyDfxEnabled()) {
        renderFrame->SetDamageRegion(damageRegionRects);
    }
    return damageRegionRects;
}

void RSDisplayRenderNodeDrawable::ScaleAndRotateMirrorForWiredScreen(RSDisplayRenderNode& node,
    RSDisplayRenderNode& mirroredNode)
{
    auto mirroredParams = static_cast<RSDisplayRenderParams*>(mirroredNode.GetRenderParams().get());
    if (!mirroredParams) {
        RS_LOGE("RSDisplayRenderNodeDrawable::ScaleAndRotateMirrorForWiredScreen mirroredParams is null");
    }
    auto mainScreenInfo = mirroredParams->GetScreenInfo();
    auto mainWidth = static_cast<float>(mainScreenInfo.width);
    auto mainHeight = static_cast<float>(mainScreenInfo.height);
    auto nodeParams = static_cast<RSDisplayRenderParams*>(node.GetRenderParams().get());
    if (!nodeParams) {
        RS_LOGE("RSDisplayRenderNodeDrawable::ScaleAndRotateMirrorForWiredScreen nodeParams is null");
    }
    auto mirrorScreenInfo = nodeParams->GetScreenInfo();
    auto mirrorWidth = static_cast<float>(mirrorScreenInfo.width);
    auto mirrorHeight = static_cast<float>(mirrorScreenInfo.height);
    auto rotation = mirroredNode.GetScreenRotation();
    auto screenManager = CreateOrGetScreenManager();
    if (screenManager) {
        auto screenCorrection = screenManager->GetScreenCorrection(mirroredParams->GetScreenId());
        if (screenCorrection != ScreenRotation::INVALID_SCREEN_ROTATION &&
            screenCorrection != ScreenRotation::ROTATION_0) {
            // Recaculate rotation if mirrored screen has additional rotation angle
            rotation = static_cast<ScreenRotation>((static_cast<int>(rotation) + SCREEN_ROTATION_NUM
                - static_cast<int>(screenCorrection)) % SCREEN_ROTATION_NUM);
        }
    }
    if (rotation == ScreenRotation::ROTATION_90 || rotation == ScreenRotation::ROTATION_270) {
        std::swap(mainWidth, mainHeight);
    }
    curCanvas_->Clear(SK_ColorBLACK);
    // Scale
    if (mainWidth > 0 && mainHeight > 0) {
        auto scaleNum = std::min(mirrorWidth / mainWidth, mirrorHeight / mainHeight);
        // 2 for calc X and Y
        curCanvas_->Translate((mirrorWidth - (scaleNum * mainWidth)) / 2, (mirrorHeight - (scaleNum * mainHeight)) / 2);
        curCanvas_->Scale(scaleNum, scaleNum);
    }
    // Rotate
    RotateMirrorCanvas(rotation, static_cast<float>(mainScreenInfo.width), static_cast<float>(mainScreenInfo.height));
}

void RSDisplayRenderNodeDrawable::SetCanvasBlack(RSProcessor& processor)
{
    curCanvas_->Clear(Drawing::Color::COLOR_BLACK);
    processor.PostProcess();
    RS_LOGI("RSDisplayRenderNodeDrawable::SetCanvasBlack, set canvas to black because of security layer.");
    curCanvas_->SetDisableFilterCache(false);
}

// To be deleted after captureWindow being deleted
void RSDisplayRenderNodeDrawable::ResetRotateIfNeed(RSDisplayRenderNode& mirroredNode,
    RSUniRenderVirtualProcessor& mirroredProcessor, Drawing::Region& clipRegion)
{
    Drawing::Matrix invertMatrix;
    if (mirroredNode.GetResetRotate() &&
        mirroredProcessor.GetScreenTransformMatrix().Invert(invertMatrix)) {
        // If both canvas and skImage have rotated, we need to reset the canvas
        curCanvas_->ConcatMatrix(invertMatrix);

        // If both canvas and clipRegion have rotated, we need to reset the clipRegion
        Drawing::Path path;
        if (clipRegion.GetBoundaryPath(&path)) {
            path.Transform(invertMatrix);
            Drawing::Region clip;
            clip.SetRect(Drawing::RectI(0, 0, curCanvas_->GetWidth(), curCanvas_->GetHeight()));
            clipRegion.SetPath(path, clip);
        }
    }
}

void RSDisplayRenderNodeDrawable::RotateMirrorCanvas(ScreenRotation& rotation, float mainWidth, float mainHeight)
{
    if (rotation == ScreenRotation::ROTATION_0) {
        return;
    } else if (rotation == ScreenRotation::ROTATION_90) {
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

    specialLayerType_ = GetSpecialLayerType(*params);
    if (specialLayerType_ != NO_SPECIAL_LAYER || UNLIKELY(RSUniRenderThread::GetCaptureParam().isMirror_)) {
        RS_LOGD("RSDisplayRenderNodeDrawable::OnCapture: \
            process RSDisplayRenderNode(id:[%{public}" PRIu64 "]) Not using UniRender buffer.",
            params->GetId());
        RS_TRACE_NAME("Process RSDisplayRenderNodeDrawable[" +
            std::to_string(params->GetScreenId()) + "] Not using UniRender buffer.");

        // Adding matrix affine transformation logic
        if (!UNLIKELY(RSUniRenderThread::GetCaptureParam().isMirror_)) {
            rscanvas->ConcatMatrix(params->GetMatrix());
        }

        // Currently, capture do not support HDR display
        rscanvas->SetCapture(true);
        RSRenderNodeDrawable::OnCapture(canvas);
        DrawWatermarkIfNeed(*params, *rscanvas);
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

    uint32_t hwcNodesNum = static_cast<uint32_t>(params->GetHardwareEnabledNodes().size());
    uint32_t hwcTopNodesNum = static_cast<uint32_t>(params->GetHardwareEnabledTopNodes().size());

    RS_LOGD("RSDisplayRenderNodeDrawable::DrawHardwareEnabledNodes: \
        process RSDisplayRenderNode(id:[%{public}" PRIu64 "]) \
        using UniRender buffer with hwcNodes(%{public}u, %{public}u)",
        params->GetId(), hwcNodesNum, hwcTopNodesNum);
    RS_TRACE_NAME_FMT("Process RSDisplayRenderNodeDrawable[%" PRIu64 "] \
        using UniRender buffer with hwcNodes(%u, %u)",
        params->GetScreenId(), hwcNodesNum, hwcTopNodesNum);

    if (hwcNodesNum > 0) {
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

    if (hwcTopNodesNum > 0) {
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

void RSDisplayRenderNodeDrawable::DrawHardwareEnabledNodesMissedInCacheImage(Drawing::Canvas& canvas)
{
    auto params = static_cast<RSDisplayRenderParams*>(GetRenderParams().get());
    if (!params) {
        RS_LOGE("RSDisplayRenderNodeDrawable::DrawHardwareEnabledNodesMissedInCacheImage params is null!");
        return;
    }

    Drawing::AutoCanvasRestore acr(canvas, true);
    FindHardwareEnabledNodes();
    if (params->GetHardwareEnabledNodes().size() != 0) {
        AdjustZOrderAndDrawSurfaceNode(params->GetHardwareEnabledNodes(), canvas, *params);
    }
}

void RSDisplayRenderNodeDrawable::SwitchColorFilter(RSPaintFilterCanvas& canvas) const
{
    const auto& renderEngine = RSUniRenderThread::Instance().GetRenderEngine();
    if (!renderEngine) {
        RS_LOGE("RSDisplayRenderNodeDrawable::SwitchColorFilter renderEngine is null");
        return;
    }
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
    const auto& renderEngine = RSUniRenderThread::Instance().GetRenderEngine();
    if (renderEngine) {
        canvas.SetHighContrast(renderEngine->IsHighContrastEnabled());
    }
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

void RSDisplayRenderNodeDrawable::DrawWatermarkIfNeed(RSDisplayRenderParams& params, RSPaintFilterCanvas& canvas) const
{
    if (!RSUniRenderThread::Instance().GetWatermarkFlag()) {
        return;
    }
    auto image = RSUniRenderThread::Instance().GetWatermarkImg();
    if (image == nullptr) {
        return;
    }
    if (auto screenManager = CreateOrGetScreenManager()) {
        RS_TRACE_FUNC();
        auto screenInfo = screenManager->QueryScreenInfo(params.GetScreenId());
        auto mainWidth = static_cast<float>(screenInfo.width);
        auto mainHeight = static_cast<float>(screenInfo.height);

        // in certain cases (such as fold screen), the width and height must be swapped to fix the screen rotation.
        int angle = RSUniRenderUtil::GetRotationFromMatrix(canvas.GetTotalMatrix());
        if (angle == RS_ROTATION_90 || angle == RS_ROTATION_270) {
            std::swap(mainWidth, mainHeight);
        }
        auto srcRect = Drawing::Rect(0, 0, image->GetWidth(), image->GetHeight());
        auto dstRect = Drawing::Rect(0, 0, mainWidth, mainHeight);
        Drawing::Brush rectBrush;
        canvas.AttachBrush(rectBrush);
        canvas.DrawImageRect(*image, srcRect, dstRect, Drawing::SamplingOptions(),
            Drawing::SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT);
        canvas.DetachBrush();
    }
}

void RSDisplayRenderNodeDrawable::DrawCurtainScreen() const
{
    if (!RSUniRenderThread::Instance().IsCurtainScreenOn() || !curCanvas_) {
        return;
    }
    RS_TRACE_FUNC();
    curCanvas_->Clear(Drawing::Color::COLOR_BLACK);
}

void RSDisplayRenderNodeDrawable::ClearTransparentBeforeSaveLayer()
{
    RS_TRACE_NAME("ClearTransparentBeforeSaveLayer");
    auto& hardwareNodes = RSUniRenderThread::Instance().GetRSRenderThreadParams()->GetHardwareEnabledTypeNodes();
    for (const auto& surfaceNode : hardwareNodes) {
        if (surfaceNode == nullptr) {
            continue;
        }
        auto params = static_cast<RSSurfaceRenderParams*>(surfaceNode->GetRenderParams().get());
        if (!params || !params->GetHardwareEnabled()) {
            continue;
        }
        auto& layerInfo = params->GetLayerInfo();
        auto& dstRect = layerInfo.boundRect;
        curCanvas_->Save();
        curCanvas_->ConcatMatrix(layerInfo.matrix);
        curCanvas_->ClipRect({ static_cast<float>(dstRect.x), static_cast<float>(dstRect.y),
            static_cast<float>(dstRect.x + dstRect.w), static_cast<float>(dstRect.y + dstRect.h) },
            Drawing::ClipOp::INTERSECT, false);
        curCanvas_->Clear(Drawing::Color::COLOR_TRANSPARENT);
        curCanvas_->Restore();
    }
}

void RSDisplayRenderNodeDrawable::PrepareOffscreenRender(const RSRenderNode& node)
{
    // cleanup
    canvasBackup_ = nullptr;
    // check offscreen size and hardware renderer
    useFixedOffscreenSurfaceSize_ = false;
    const auto& property = node.GetRenderProperties();
    uint32_t offscreenWidth = property.GetFrameWidth();
    uint32_t offscreenHeight = property.GetFrameHeight();
    auto params = static_cast<RSDisplayRenderParams*>(GetRenderParams().get());
    // use fixed surface size in order to reduce create texture
    if (RSSystemProperties::IsFoldScreenFlag() && params && params->IsRotationChanged()) {
        useFixedOffscreenSurfaceSize_ = true;
        int32_t maxRenderSize =
            static_cast<int32_t>(std::max(params->GetScreenInfo().width, params->GetScreenInfo().height));
        offscreenWidth = maxRenderSize;
        offscreenHeight = maxRenderSize;
    }

    if (offscreenWidth <= 0 || offscreenHeight <= 0) {
        RS_LOGE("RSDisplayRenderNodeDrawable::PrepareOffscreenRender, offscreenWidth or offscreenHeight is invalid");
        return;
    }
    if (curCanvas_->GetSurface() == nullptr) {
        curCanvas_->ClipRect(Drawing::Rect(0, 0, offscreenWidth, offscreenHeight), Drawing::ClipOp::INTERSECT, false);
        RS_LOGE("RSDisplayRenderNodeDrawable::PrepareOffscreenRender, current surface is nullptr");
        return;
    }
    // create offscreen surface and canvas
    if (useFixedOffscreenSurfaceSize_) {
        if (!offscreenSurface_) {
            RS_TRACE_NAME_FMT("make offscreen surface with fixed size: [%d, %d]", offscreenWidth, offscreenHeight);
            offscreenSurface_ = curCanvas_->GetSurface()->MakeSurface(offscreenWidth, offscreenHeight);
        }
    } else {
        offscreenSurface_ = curCanvas_->GetSurface()->MakeSurface(offscreenWidth, offscreenHeight);
    }
    
    if (offscreenSurface_ == nullptr) {
        RS_LOGE("RSDisplayRenderNodeDrawable::PrepareOffscreenRender, offscreenSurface is nullptr");
        curCanvas_->ClipRect(Drawing::Rect(0, 0, offscreenWidth, offscreenHeight), Drawing::ClipOp::INTERSECT, false);
        return;
    }
    auto offscreenCanvas = std::make_shared<RSPaintFilterCanvas>(offscreenSurface_.get());

    // copy HDR properties into offscreen canvas
    offscreenCanvas->CopyHDRConfiguration(*curCanvas_);
    // copy current canvas properties into offscreen canvas
    offscreenCanvas->CopyConfigurationToOffscreenCanvas(*curCanvas_);

    // backup current canvas and replace with offscreen canvas
    canvasBackup_ = std::exchange(curCanvas_, offscreenCanvas);
}

void RSDisplayRenderNodeDrawable::FinishOffscreenRender(const Drawing::SamplingOptions& sampling)
{
    if (canvasBackup_ == nullptr) {
        RS_LOGE("RSDisplayRenderNodeDrawable::FinishOffscreenRender, canvasBackup_ is nullptr");
        return;
    }
    // draw offscreen surface to current canvas
    Drawing::Brush paint;
    paint.SetAntiAlias(true);
    paint.SetForceBrightnessDisable(true);
    canvasBackup_->AttachBrush(paint);
    canvasBackup_->DrawImage(*offscreenSurface_->GetImageSnapshot().get(), 0, 0, sampling);
    canvasBackup_->DetachBrush();
    // restore current canvas and cleanup
    if (!useFixedOffscreenSurfaceSize_) {
        offscreenSurface_ = nullptr;
    }
    curCanvas_ = std::move(canvasBackup_);
}

bool RSDisplayRenderNodeDrawable::SkipDisplayIfScreenOff() const
{
    if (!RSSystemProperties::GetSkipDisplayIfScreenOffEnabled() || !RSSystemProperties::IsPhoneType()) {
        return false;
    }
    auto screenManager = CreateOrGetScreenManager();
    auto renderNode = renderNode_.lock();
    if (!screenManager || !renderNode) {
        RS_LOGE("RSDisplayRenderNodeDrawable::SkipRenderFrameIfScreenOff, failed to get screen manager/renderNode!");
        return false;
    }
    // in certain cases such as wireless display, render skipping may be disabled.
    if (screenManager->GetDisableRenderControlScreensCount() != 0) {
        return false;
    }
    auto nodeSp = std::const_pointer_cast<RSRenderNode>(renderNode);
    auto displayNodeSp = std::static_pointer_cast<RSDisplayRenderNode>(nodeSp);
    if (!displayNodeSp) {
        RS_LOGE("RSDisplayRenderNodeDrawable::SkipRenderFrameIfScreenOff, display node is null!.");
        return false;
    }
    ScreenId id = displayNodeSp->GetScreenId();
    if (!screenManager->IsScreenPowerOff(id)) {
        return false;
    }
    if (screenManager->GetPowerOffNeedProcessOneFrame()) {
        RS_LOGD("RSDisplayRenderNodeDrawable::SkipRenderFrameIfScreenOff screen_%{public}" PRIu64
            " power off, one more frame.", id);
        screenManager->ResetPowerOffNeedProcessOneFrame();
        return false;
    }
    return true;
}
} // namespace OHOS::Rosen::DrawableV2
