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

#include "drawable/rs_surface_render_node_drawable.h"

#include "acquire_fence_manager.h"
#include "impl_interface/region_impl.h"
#include "rs_trace.h"
#include "common/rs_color.h"
#include "common/rs_common_def.h"
#include "common/rs_optional_trace.h"
#include "common/rs_obj_abs_geometry.h"
#include "draw/brush.h"
#include "drawable/rs_display_render_node_drawable.h"
#include "memory/rs_tag_tracker.h"
#include "params/rs_display_render_params.h"
#include "params/rs_surface_render_params.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_surface_handler.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_uni_render_thread.h"
#include "pipeline/rs_uni_render_util.h"
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
#include "pipeline/pointer_render/rs_pointer_render_manager.h"
#endif

#include "platform/common/rs_log.h"
#include "platform/ohos/rs_node_stats.h"
#include "utils/rect.h"
#include "utils/region.h"

#include "pipeline/rs_uifirst_manager.h"
#include "pipeline/parallel_render/rs_sub_thread_manager.h"
#include "pipeline/rs_main_thread.h"
#include "static_factory.h"
#ifdef RS_ENABLE_VK
#include "include/gpu/GrBackendSurface.h"
#include "platform/ohos/backend/native_buffer_utils.h"
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif
#include "render/rs_high_performance_visual_engine.h"

#include "luminance/rs_luminance_control.h"
#ifdef USE_VIDEO_PROCESSING_ENGINE
#include "metadata_helper.h"
#endif
namespace {
constexpr int32_t CORNER_SIZE = 4;
constexpr float GAMMA2_2 = 2.2f;
}
namespace OHOS::Rosen::DrawableV2 {
RSSurfaceRenderNodeDrawable::Registrar RSSurfaceRenderNodeDrawable::instance_;

RSSurfaceRenderNodeDrawable::RSSurfaceRenderNodeDrawable(std::shared_ptr<const RSRenderNode>&& node)
    : RSRenderNodeDrawable(std::move(node)), syncDirtyManager_(std::make_shared<RSDirtyRegionManager>())
{
    auto nodeSp = std::const_pointer_cast<RSRenderNode>(node);
    auto surfaceNode = std::static_pointer_cast<RSSurfaceRenderNode>(nodeSp);
    name_ = surfaceNode->GetName();
    if (name_.find("SCBScreenLock") != std::string::npos) {
        vmaCacheOff_ = true;
    }
    surfaceNodeType_ = surfaceNode->GetSurfaceNodeType();
#ifndef ROSEN_CROSS_PLATFORM
    consumerOnDraw_ = surfaceNode->GetRSSurfaceHandler()->GetConsumer();
#endif
}

RSSurfaceRenderNodeDrawable::~RSSurfaceRenderNodeDrawable()
{
    ClearCacheSurfaceInThread();
}

RSRenderNodeDrawable::Ptr RSSurfaceRenderNodeDrawable::OnGenerate(std::shared_ptr<const RSRenderNode> node)
{
    RS_TRACE_NAME("RSRenderNodeDrawable::Ptr RSSurfaceRenderNodeDrawable::OnGenerate");
    return new RSSurfaceRenderNodeDrawable(std::move(node));
}

void RSSurfaceRenderNodeDrawable::OnGeneralProcess(
    RSPaintFilterCanvas& canvas, RSSurfaceRenderParams& surfaceParams, bool isSelfDrawingSurface)
{
    auto bounds = surfaceParams.GetFrameRect();

    // 1. draw background
    DrawBackground(canvas, bounds);

    // 2. draw self drawing node
    if (surfaceParams.GetBuffer() != nullptr) {
        DealWithSelfDrawingNodeBuffer(canvas, surfaceParams);
    }

    if (isSelfDrawingSurface) {
        canvas.Restore();
    }

    if (surfaceParams.GetNeedCacheSurface()) {
        // 3/4 Draw content and children of this node by the main canvas, and cache
        drawWindowCache_.DrawAndCacheWindowContent(this, canvas, bounds);
    } else {
        // 3. Draw content of this node by the main canvas.
        DrawContent(canvas, bounds);

        // 4. Draw children of this node by the main canvas.
        DrawChildren(canvas, bounds);
    }

    // 5. Draw foreground of this node by the main canvas.
    DrawForeground(canvas, bounds);
}

void RSSurfaceRenderNodeDrawable::MergeSubSurfaceNodesDirtyRegionForMainWindow(
    RSSurfaceRenderParams& surfaceParams, Occlusion::Region& surfaceDirtyRegion) const
{
    for (const auto& drawable : GetDrawableVectorById(surfaceParams.GetAllSubSurfaceNodeIds())) {
        auto surfaceNodeDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(drawable);
        if (surfaceNodeDrawable && surfaceNodeDrawable->GetSyncDirtyManager()) {
            Occlusion::Region subSurfaceDirtyRegion(surfaceNodeDrawable->GetSyncDirtyManager()->GetDirtyRegion());
            surfaceDirtyRegion.OrSelf(subSurfaceDirtyRegion);
        }
    }
}

Drawing::Region RSSurfaceRenderNodeDrawable::CalculateVisibleDirtyRegion(RSRenderThreadParams& uniParam,
    RSSurfaceRenderParams& surfaceParams, RSSurfaceRenderNodeDrawable& surfaceDrawable, bool isOffscreen) const
{
    Drawing::Region resultRegion;
    if (!surfaceParams.IsMainWindowType()) {
        return resultRegion;
    }

    // FUTURE: return real region
    if (isOffscreen) {
        resultRegion.SetRect(Drawing::RectI(0, 0,
        DRAWING_MAX_S32_FITS_IN_FLOAT, DRAWING_MAX_S32_FITS_IN_FLOAT));
        return resultRegion;
    }

    auto visibleRegion = surfaceParams.GetVisibleRegion();
    if (uniParam.IsOcclusionEnabled() && visibleRegion.IsEmpty()) {
        return resultRegion;
    }
    // The region is dirty region of this SurfaceNode (including sub-surface node in its subtree).
    Occlusion::Region surfaceNodeDirtyRegion(GetSyncDirtyManager()->GetDirtyRegion());
    // Dirty region of sub-surface nodes should also be count in parent main surface node, to avoid early skipping.
    if (surfaceParams.HasSubSurfaceNodes()) {
        MergeSubSurfaceNodesDirtyRegionForMainWindow(surfaceParams, surfaceNodeDirtyRegion);
    }
    // The region is the result of global dirty region AND occlusion region.
    Occlusion::Region globalDirtyRegion = GetGlobalDirtyRegion();
    // This include dirty region and occlusion region when surfaceNode is mainWindow.
    auto dirtyRegion = globalDirtyRegion.Or(surfaceNodeDirtyRegion);
    auto visibleDirtyRegion = dirtyRegion.And(visibleRegion);
    if (visibleDirtyRegion.IsEmpty()) {
        RS_LOGD("RSSurfaceRenderNodeDrawable::OnDraw occlusion skip SurfaceName:%s NodeId:%" PRIu64 "",
            surfaceDrawable.GetName().c_str(), surfaceParams.GetId());
        return resultRegion;
    }

    for (auto& rect : visibleDirtyRegion.GetRegionRects()) {
        Drawing::Region tempRegion;
        tempRegion.SetRect(Drawing::RectI(rect.left_, rect.top_, rect.right_, rect.bottom_));
        resultRegion.Op(tempRegion, Drawing::RegionOp::UNION);
    }
    return resultRegion;
}

bool RSSurfaceRenderNodeDrawable::PrepareOffscreenRender()
{
    // cleanup
    canvasBackup_ = nullptr;

    // check offscreen size
    if (curCanvas_->GetSurface() == nullptr) {
        RS_LOGE("RSSurfaceRenderNodeDrawable::PrepareOffscreenRender, current surface is nullptr");
        return false;
    }
    int offscreenWidth = curCanvas_->GetSurface()->Width();
    int offscreenHeight = curCanvas_->GetSurface()->Height();
    auto& uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    if (uniParam && uniParam->IsMirrorScreen() &&
        uniParam->GetCompositeType() == RSDisplayRenderNode::CompositeType::UNI_RENDER_COMPOSITE) {
        auto screenInfo = uniParam->GetScreenInfo();
        offscreenWidth = static_cast<int>(screenInfo.width);
        offscreenHeight = static_cast<int>(screenInfo.height);
    }
    if (offscreenWidth <= 0 || offscreenHeight <= 0) {
        RS_LOGE("RSSurfaceRenderNodeDrawable::PrepareOffscreenRender, offscreenWidth or offscreenHeight is invalid");
        return false;
    }

    int maxRenderSize = std::max(offscreenWidth, offscreenHeight);
    // create offscreen surface and canvas
    if (offscreenSurface_ == nullptr || maxRenderSize_ != maxRenderSize) {
        RS_LOGD("PrepareOffscreenRender create offscreen surface offscreenSurface_,\
            new [%{public}d, %{public}d %{public}d]", offscreenWidth, offscreenHeight, maxRenderSize);
        RS_TRACE_NAME_FMT("PrepareOffscreenRender surface size: [%d, %d]", maxRenderSize, maxRenderSize);
        maxRenderSize_ = maxRenderSize;
        offscreenSurface_ = curCanvas_->GetSurface()->MakeSurface(maxRenderSize_, maxRenderSize_);
    }
    if (offscreenSurface_ == nullptr) {
        RS_LOGE("RSSurfaceRenderNodeDrawable::PrepareOffscreenRender, offscreenSurface is nullptr");
        return false;
    }

    offscreenCanvas_ = std::make_shared<RSPaintFilterCanvas>(offscreenSurface_.get());

    // copy HDR properties into offscreen canvas
    offscreenCanvas_->CopyHDRConfiguration(*curCanvas_);
    // copy current canvas properties into offscreen canvas
    offscreenCanvas_->CopyConfigurationToOffscreenCanvas(*curCanvas_);

    // backup current canvas and replace with offscreen canvas
    canvasBackup_ = curCanvas_;
    curCanvas_ = offscreenCanvas_.get();
    curCanvas_->SetDisableFilterCache(true);
    arc_ = std::make_unique<RSAutoCanvasRestore>(curCanvas_, RSPaintFilterCanvas::SaveType::kCanvasAndAlpha);
    curCanvas_->Clear(Drawing::Color::COLOR_TRANSPARENT);
    return true;
}

void RSSurfaceRenderNodeDrawable::FinishOffscreenRender(const Drawing::SamplingOptions& sampling)
{
    if (canvasBackup_ == nullptr) {
        RS_LOGE("RSSurfaceRenderNodeDrawable::FinishOffscreenRender, canvasBackup_ is nullptr");
        return;
    }
    if (offscreenSurface_ == nullptr) {
        RS_LOGE("RSSurfaceRenderNodeDrawable::FinishOffscreenRender, offscreenSurface_ is nullptr");
        return;
    }
    auto image = offscreenSurface_->GetImageSnapshot();
    if (image == nullptr) {
        RS_LOGE("RSSurfaceRenderNodeDrawable::FinishOffscreenRender, Surface::GetImageSnapshot is nullptr");
        return;
    }
    // draw offscreen surface to current canvas
    Drawing::Brush paint;
    paint.SetAntiAlias(true);
    canvasBackup_->AttachBrush(paint);
    canvasBackup_->DrawImage(*image, 0, 0, sampling);
    canvasBackup_->DetachBrush();
    arc_ = nullptr;
    curCanvas_ = canvasBackup_;
}

bool RSSurfaceRenderNodeDrawable::IsHardwareEnabled()
{
    auto& hardwareDrawables =
        RSUniRenderThread::Instance().GetRSRenderThreadParams()->GetHardwareEnabledTypeDrawables();
    for (const auto& [_, drawable] : hardwareDrawables) {
        if (!drawable || !drawable->GetRenderParams()) {
            continue;
        }
        auto params = static_cast<RSSurfaceRenderParams*>(drawable->GetRenderParams().get());
        if (!params || !params->GetHardwareEnabled()) {
            continue;
        }
        return true;
    }
    return false;
}

void RSSurfaceRenderNodeDrawable::OnDraw(Drawing::Canvas& canvas)
{
    SetDrawSkipType(DrawSkipType::NONE);
    if (!ShouldPaint()) {
        SetDrawSkipType(DrawSkipType::SHOULD_NOT_PAINT);
        RS_TRACE_NAME_FMT("RSSurfaceRenderNodeDrawable::OnDraw %s should not paint", name_.c_str());
        return;
    }

    if (vmaCacheOff_) {
        Drawing::StaticFactory::SetVmaCacheStatus(false); // render this frame with vma cache off
    }

    auto rscanvas = reinterpret_cast<RSPaintFilterCanvas*>(&canvas);
    if (!rscanvas) {
        SetDrawSkipType(DrawSkipType::CANVAS_NULL);
        RS_LOGE("RSSurfaceRenderNodeDrawable::OnDraw, rscanvas us nullptr");
        return;
    }
    auto& uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    if (UNLIKELY(!uniParam)) {
        SetDrawSkipType(DrawSkipType::RENDER_THREAD_PARAMS_NULL);
        RS_LOGE("RSSurfaceRenderNodeDrawable::OnDraw uniParam is nullptr");
        return;
    }
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(GetRenderParams().get());
    if (!surfaceParams) {
        SetDrawSkipType(DrawSkipType::RENDER_PARAMS_NULL);
        RS_LOGE("RSSurfaceRenderNodeDrawable::OnDraw params is nullptr");
        return;
    }
    if (surfaceParams->GetSkipDraw()) {
        SetDrawSkipType(DrawSkipType::SURFACE_PARAMS_SKIP_DRAW);
        RS_TRACE_NAME_FMT("RSSurfaceRenderNodeDrawable::OnDraw SkipDraw [%s] Id:%" PRIu64 "",
            name_.c_str(), surfaceParams->GetId());
        return;
    }
    auto renderEngine = RSUniRenderThread::Instance().GetRenderEngine();
    if (!renderEngine) {
        RS_LOGE("RSSurfaceRenderNodeDrawable::OnDraw renderEngine is nullptr");
        return;
    }
    auto unmappedCache = surfaceParams->GetBufferClearCacheSet();
    if (unmappedCache.size() > 0) {
        // remove imagecahce when its bufferQueue gobackground
        renderEngine->ClearCacheSet(unmappedCache);
    }
    if (autoCacheEnable_) {
        nodeCacheType_ = NodeStrategyType::CACHE_NONE;
    }
    bool isUiFirstNode = rscanvas->GetIsParallelCanvas();
    bool disableFilterCache = rscanvas->GetDisableFilterCache();
    if (!disableFilterCache && !isUiFirstNode && surfaceParams->GetOccludedByFilterCache()) {
        SetDrawSkipType(DrawSkipType::FILTERCACHE_OCCLUSION_SKIP);
        RS_TRACE_NAME_FMT("RSSurfaceRenderNodeDrawable::OnDraw filterCache occlusion skip [%s] %sAlpha: %f, "
            "NodeId:%" PRIu64 "", name_.c_str(), surfaceParams->GetAbsDrawRect().ToString().c_str(),
            surfaceParams->GetGlobalAlpha(), surfaceParams->GetId());
        return;
    }
    hasSkipCacheLayer_ =
        (surfaceParams->GetIsSecurityLayer() && !uniParam->GetSecExemption()) || surfaceParams->GetIsSkipLayer();
    if (hasSkipCacheLayer_ && curDrawingCacheRoot_) {
        curDrawingCacheRoot_->SetSkipCacheLayer(true);
    }

    Drawing::Region curSurfaceDrawRegion = CalculateVisibleDirtyRegion(*uniParam, *surfaceParams, *this, isUiFirstNode);
    if (!isUiFirstNode) {
        MergeDirtyRegionBelowCurSurface(*uniParam, curSurfaceDrawRegion);
        if (uniParam->IsOpDropped() && surfaceParams->IsVisibleDirtyRegionEmpty(curSurfaceDrawRegion)) {
            SetDrawSkipType(DrawSkipType::OCCLUSION_SKIP);
            RS_TRACE_NAME_FMT("RSSurfaceRenderNodeDrawable::OnDraw occlusion skip SurfaceName:%s %sAlpha: %f, NodeId:"
                "%" PRIu64 "", name_.c_str(), surfaceParams->GetAbsDrawRect().ToString().c_str(),
                surfaceParams->GetGlobalAlpha(), surfaceParams->GetId());
            return;
        }
    }
    const auto &absDrawRect = surfaceParams->GetAbsDrawRect();
    // syncDirtyManager_ is not null
    const RectI& currentFrameDirty = syncDirtyManager_->GetCurrentFrameDirtyRegion();
    const RectI& mergeHistoryDirty = syncDirtyManager_->GetDirtyRegion();
    // warning : don't delete this trace or change trace level to optional !!!
    RS_TRACE_NAME_FMT("RSSurfaceRenderNodeDrawable::OnDraw:[%s] (%d, %d, %d, %d)Alpha: %f, "
        "currentFrameDirty (%d, %d, %d, %d), mergeHistoryDirty (%d, %d, %d, %d)", name_.c_str(),
        absDrawRect.left_, absDrawRect.top_, absDrawRect.width_, absDrawRect.height_, surfaceParams->GetGlobalAlpha(),
        currentFrameDirty.left_, currentFrameDirty.top_, currentFrameDirty.width_, currentFrameDirty.height_,
        mergeHistoryDirty.left_, mergeHistoryDirty.top_, mergeHistoryDirty.width_, mergeHistoryDirty.height_);

    RS_LOGD("RSSurfaceRenderNodeDrawable::OnDraw node:%{public}" PRIu64 ", name:%{public}s,"
            "OcclusionVisible:%{public}d Bound:%{public}s",
        surfaceParams->GetId(), name_.c_str(), surfaceParams->GetOcclusionVisible(),
        surfaceParams->GetBounds().ToString().c_str());

    RSUiFirstProcessStateCheckerHelper stateCheckerHelper(
        surfaceParams->GetFirstLevelNodeId(), surfaceParams->GetUifirstRootNodeId(), nodeId_);
    if (!RSUiFirstProcessStateCheckerHelper::CheckMatchAndWaitNotify(*surfaceParams)) {
        SetDrawSkipType(DrawSkipType::CHECK_MATCH_AND_WAIT_NOTIFY_FAIL);
        return;
    }

    if (drawWindowCache_.DealWithCachedWindow(this, *rscanvas, *surfaceParams)) {
        return;
    }
    if (DealWithUIFirstCache(*rscanvas, *surfaceParams, *uniParam)) {
        if (GetDrawSkipType() == DrawSkipType::NONE) {
            SetDrawSkipType(DrawSkipType::UI_FIRST_CACHE_SKIP);
        }
        return;
    }

    RSRenderNodeSingleDrawableLocker singleLocker(this);
    if (UNLIKELY(!singleLocker.IsLocked())) {
        SetDrawSkipType(DrawSkipType::MULTI_ACCESS);
        singleLocker.DrawableOnDrawMultiAccessEventReport(__func__);
        RS_LOGE("RSSurfaceRenderNodeDrawable::OnDraw node %{public}" PRIu64 " onDraw!!!", GetId());
        return;
    }

    std::shared_ptr<Drawing::GPUContext> gpuContext = nullptr;
    auto realTid = gettid();
    if (realTid == RSUniRenderThread::Instance().GetTid()) {
        gpuContext = RSUniRenderThread::Instance().GetRenderEngine()->GetRenderContext()->GetSharedDrGPUContext();
    } else {
        gpuContext = RSSubThreadManager::Instance()->GetGrContextFromSubThread(realTid);
    }
    RSTagTracker tagTracker(gpuContext.get(), surfaceParams->GetId(),
        RSTagTracker::TAGTYPE::TAG_DRAW_SURFACENODE, surfaceParams->GetName());

    // Draw base pipeline start
    RSAutoCanvasRestore acr(rscanvas, RSPaintFilterCanvas::SaveType::kAll);
    bool needOffscreen = (realTid == RSUniRenderThread::Instance().GetTid()) &&
        surfaceParams->GetNeedOffscreen() && !rscanvas->GetTotalMatrix().IsIdentity() &&
        surfaceParams->IsAppWindow() && GetName().substr(0, 3) != "SCB" && !IsHardwareEnabled() &&
        (surfaceParams->GetVisibleRegion().Area() == surfaceParams->GetOpaqueRegion().Area());
    curCanvas_ = rscanvas;
    if (needOffscreen) {
        releaseCount_ = 0;
        if (!PrepareOffscreenRender()) {
            needOffscreen = false;
        }
    } else {
        if (offscreenSurface_ != nullptr) {
            releaseCount_++;
            if (releaseCount_ == MAX_RELEASE_FRAME) {
                std::shared_ptr<Drawing::Surface> hold = offscreenSurface_;
                RSUniRenderThread::Instance().PostTask([hold] {});
                offscreenSurface_ = nullptr;
                releaseCount_ = 0;
            }
        }
    }

    surfaceParams->ApplyAlphaAndMatrixToCanvas(*curCanvas_, !needOffscreen);

    bool isSelfDrawingSurface = surfaceParams->GetSurfaceNodeType() == RSSurfaceNodeType::SELF_DRAWING_NODE &&
        !surfaceParams->IsSpherizeValid() && !surfaceParams->IsAttractionValid();
    if (isSelfDrawingSurface) {
        SetSkip(surfaceParams->GetBuffer() != nullptr ? SkipType::SKIP_BACKGROUND_COLOR : SkipType::NONE);
        // Restore in OnGeneralProcess
        curCanvas_->Save();
    }

    if (surfaceParams->IsMainWindowType()) {
        RSRenderNodeDrawable::ClearTotalProcessedNodeCount();
        RSRenderNodeDrawable::ClearProcessedNodeCount();
        if (!surfaceParams->GetNeedOffscreen()) {
            curCanvas_->PushDirtyRegion(curSurfaceDrawRegion);
        }
    }

    auto parentSurfaceMatrix = RSRenderParams::GetParentSurfaceMatrix();
    RSRenderParams::SetParentSurfaceMatrix(curCanvas_->GetTotalMatrix());

    // add a blending disable rect op behind floating window, to enable overdraw buffer feature on special gpu.
    if (surfaceParams->IsLeashWindow() && RSSystemProperties::GetGpuOverDrawBufferOptimizeEnabled()
        && surfaceParams->IsGpuOverDrawBufferOptimizeNode()) {
        EnableGpuOverDrawDrawBufferOptimization(*curCanvas_, surfaceParams);
    }

    if (surfaceParams->GetRSFreezeFlag() && GetCacheImageByCapture()) {
        RS_TRACE_NAME("Drawing cachedImage by capture");
        DrawCachedImage(*curCanvas_, surfaceParams->GetCacheSize());
    } else {
        if (GetCacheImageByCapture()) {
            SetCacheImageByCapture(nullptr);
        }
        OnGeneralProcess(*curCanvas_, *surfaceParams, isSelfDrawingSurface);
    }

    if (needOffscreen && canvasBackup_) {
        Drawing::AutoCanvasRestore acrBackUp(*canvasBackup_, true);
        if (surfaceParams->HasSandBox()) {
            canvasBackup_->SetMatrix(surfaceParams->GetParentSurfaceMatrix());
            canvasBackup_->ConcatMatrix(surfaceParams->GetMatrix());
        } else {
            canvasBackup_->ConcatMatrix(surfaceParams->GetMatrix());
        }
        FinishOffscreenRender(
            Drawing::SamplingOptions(Drawing::FilterMode::NEAREST, Drawing::MipmapMode::NONE));
        RS_LOGD("FinishOffscreenRender %{public}s node type %{public}d", surfaceParams->GetName().c_str(),
            int(surfaceParams->GetSurfaceNodeType()));
    }

    // Draw base pipeline end
    if (surfaceParams->IsMainWindowType()) {
        if (!surfaceParams->GetNeedOffscreen()) {
            curCanvas_->PopDirtyRegion();
        }
        int processedNodes = RSRenderNodeDrawable::GetProcessedNodeCount();
        AcquireFenceTracker::SetContainerNodeNum(processedNodes);
        RS_TRACE_NAME_FMT("RSUniRenderThread::Render() the number of total ProcessedNodes: %d",
            RSRenderNodeDrawable::GetTotalProcessedNodeCount());
        const RSNodeStatsType nodeStats = CreateRSNodeStatsItem(
            RSRenderNodeDrawable::GetTotalProcessedNodeCount(), GetId(), GetName());
        RSNodeStats::GetInstance().AddNodeStats(nodeStats);
    }

    RSRenderParams::SetParentSurfaceMatrix(parentSurfaceMatrix);
}

void RSSurfaceRenderNodeDrawable::MergeDirtyRegionBelowCurSurface(
    RSRenderThreadParams& uniParam, Drawing::Region& region)
{
    if (!renderParams_) {
        return;
    }
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(renderParams_.get());
    auto isMainWindowType = surfaceParams->IsMainWindowType();
    auto visibleRegion = surfaceParams->GetVisibleRegion();
    if (isMainWindowType && visibleRegion.IsEmpty()) {
        return;
    }
    if (isMainWindowType || surfaceParams->IsLeashWindow()) {
        auto& accumulatedDirtyRegion = uniParam.GetAccumulatedDirtyRegion();
        Occlusion::Region calcRegion;
        if ((isMainWindowType && surfaceParams->IsParentScaling()) ||
            surfaceParams->IsSubSurfaceNode() || uniParam.IsAllSurfaceVisibleDebugEnabled()) {
            calcRegion = visibleRegion;
        } else if (!surfaceParams->GetTransparentRegion().IsEmpty()) {
            auto transparentRegion = surfaceParams->GetTransparentRegion();
            calcRegion = visibleRegion.And(transparentRegion);
        }
        if (!calcRegion.IsEmpty()) {
            auto dirtyRegion = calcRegion.And(accumulatedDirtyRegion);
            if (!dirtyRegion.IsEmpty()) {
                for (auto& rect : dirtyRegion.GetRegionRects()) {
                    Drawing::Region tempRegion;
                    tempRegion.SetRect(Drawing::RectI(
                        rect.left_, rect.top_, rect.right_, rect.bottom_));
                    region.Op(tempRegion, Drawing::RegionOp::UNION);
                }
            }
        }
        // [planing] surfaceDirtyRegion can be optimized by visibleDirtyRegion in some case.
        auto surfaceDirtyRegion = Occlusion::Region {
            Occlusion::Rect{ GetSyncDirtyManager()->GetDirtyRegion() } };
        accumulatedDirtyRegion.OrSelf(surfaceDirtyRegion);
        // add children window dirty here for uifirst leasf window will not traverse cached children
        if (surfaceParams->GetUifirstNodeEnableParam() != MultiThreadCacheType::NONE) {
            auto childrenDirtyRegion = Occlusion::Region {
                Occlusion::Rect{ surfaceParams->GetUifirstChildrenDirtyRectParam() } };
            accumulatedDirtyRegion.OrSelf(childrenDirtyRegion);
        }
    }
}

void RSSurfaceRenderNodeDrawable::OnCapture(Drawing::Canvas& canvas)
{
    if (!ShouldPaint()) {
        return;
    }

    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(GetRenderParams().get());
    if (!surfaceParams) {
        RS_LOGE("RSSurfaceRenderNodeDrawable::OnCapture surfaceParams is nullptr");
        return;
    }

    if (vmaCacheOff_) {
        Drawing::StaticFactory::SetVmaCacheStatus(false); // render this frame with vma cache off
    }

    // HidePrivacyContent is only for UICapture or NoneSystemCalling-WindowCapture
    bool isHiddenScene = canvas.GetUICapture() ||
        (RSUniRenderThread::GetCaptureParam().isSingleSurface_ &&
        !RSUniRenderThread::GetCaptureParam().isSystemCalling_);
    if ((surfaceNodeType_ == RSSurfaceNodeType::UI_EXTENSION_COMMON_NODE ||
        surfaceNodeType_ == RSSurfaceNodeType::UI_EXTENSION_SECURE_NODE) &&
        isHiddenScene && surfaceParams->GetHidePrivacyContent()) {
        RS_LOGE("RSSurfaceRenderNodeDrawable::OnCapture surfacenode nodeId:[%{public}" PRIu64
                "] is not allowed to be captured", nodeId_);
        return;
    }

    RSUiFirstProcessStateCheckerHelper stateCheckerHelper(
        surfaceParams->GetFirstLevelNodeId(), surfaceParams->GetUifirstRootNodeId(), nodeId_);
    auto& uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    if (UNLIKELY(!uniParam)) {
        RS_LOGE("RSSurfaceRenderNodeDrawable::OnCapture uniParam is nullptr");
        return;
    }

    auto rscanvas = static_cast<RSPaintFilterCanvas*>(&canvas);
    if (!rscanvas) {
        RS_LOGE("RSSurfaceRenderNodeDrawable::OnCapture, rscanvas us nullptr");
        return;
    }
    rscanvas->SetHighContrast(RSUniRenderThread::Instance().IsHighContrastTextModeOn());
    // process white list
    auto whiteList = RSUniRenderThread::Instance().GetWhiteList();
    SetVirtualScreenWhiteListRootId(whiteList, surfaceParams->GetLeashPersistentId());

    if (CheckIfSurfaceSkipInMirror(*surfaceParams)) {
        return;
    }

    if (uniParam->IsOcclusionEnabled() && surfaceParams->IsMainWindowType() &&
        surfaceParams->GetVisibleRegionInVirtual().IsEmpty() && whiteList.empty() &&
        UNLIKELY(RSUniRenderThread::GetCaptureParam().isMirror_)) {
        RS_TRACE_NAME("RSSurfaceRenderNodeDrawable::OnCapture occlusion skip :[" + name_ + "] " +
            surfaceParams->GetAbsDrawRect().ToString());
        return;
    }

    RS_TRACE_NAME("RSSurfaceRenderNodeDrawable::OnCapture:[" + name_ + "] " +
        surfaceParams->GetAbsDrawRect().ToString() + "Alpha: " +
        std::to_string(surfaceParams->GetGlobalAlpha()));
    RSAutoCanvasRestore acr(rscanvas, RSPaintFilterCanvas::SaveType::kCanvasAndAlpha);

    // First node don't need to concat matrix for application
    if (RSUniRenderThread::GetCaptureParam().isFirstNode_) {
        // Planning: If node is a sandbox.
        rscanvas->MultiplyAlpha(surfaceParams->GetAlpha());
        RSUniRenderThread::GetCaptureParam().isFirstNode_ = false;
    } else {
        surfaceParams->ApplyAlphaAndMatrixToCanvas(*rscanvas);
    }

    CaptureSurface(*rscanvas, *surfaceParams);
    ResetVirtualScreenWhiteListRootId(surfaceParams->GetLeashPersistentId());
}

bool RSSurfaceRenderNodeDrawable::CheckIfSurfaceSkipInMirror(const RSSurfaceRenderParams& surfaceParams)
{
    if (!RSUniRenderThread::GetCaptureParam().isMirror_) {
        return false;
    }
    // Check black list.
    const auto& blackList = RSUniRenderThread::Instance().GetBlackList();
    if (surfaceParams.IsLeashWindow() && blackList.find(surfaceParams.GetLeashPersistentId()) != blackList.end()) {
        RS_LOGD("RSSurfaceRenderNodeDrawable::CheckIfSurfaceSkipInMirror: \
            (LeashPersistentId:[%{public}" PRIu64 "]) is in black list", surfaceParams.GetLeashPersistentId());
        return true;
    }
    if (blackList.find(surfaceParams.GetId()) != blackList.end()) {
        RS_LOGD("RSSurfaceRenderNodeDrawable::CheckIfSurfaceSkipInMirror: \
            (surfaceParamsId:[%{public}" PRIu64 "]) is in black list", surfaceParams.GetId());
        return true;
    }
    // Check white list.
    const auto& whiteList = RSUniRenderThread::Instance().GetWhiteList();
    if (!whiteList.empty() && RSUniRenderThread::GetCaptureParam().rootIdInWhiteList_ == INVALID_NODEID) {
        RS_LOGD("RSSurfaceRenderNodeDrawable::CheckIfSurfaceSkipInMirror: \
            (id:[%{public}" PRIu64 "]) isn't in white list", surfaceParams.GetId());
        return true;
    }

    return false;
}

void RSSurfaceRenderNodeDrawable::SetVirtualScreenWhiteListRootId(
    const std::unordered_set<NodeId>& whiteList, NodeId id)
{
    if (whiteList.find(id) == whiteList.end()) {
        return;
    }
    // don't update if it's ancestor has already set
    if (RSUniRenderThread::GetCaptureParam().rootIdInWhiteList_ != INVALID_NODEID) {
        return;
    }
    RSUniRenderThread::GetCaptureParam().rootIdInWhiteList_ = id;
}

void RSSurfaceRenderNodeDrawable::ResetVirtualScreenWhiteListRootId(NodeId id)
{
    // only reset by the node which sets the flag
    if (RSUniRenderThread::GetCaptureParam().rootIdInWhiteList_ == id) {
        RSUniRenderThread::GetCaptureParam().rootIdInWhiteList_ = INVALID_NODEID;
    }
}

void RSSurfaceRenderNodeDrawable::CaptureSurface(RSPaintFilterCanvas& canvas, RSSurfaceRenderParams& surfaceParams)
{
    auto& uniParams = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    if (UNLIKELY(!uniParams)) {
        RS_LOGE("RSSurfaceRenderNodeDrawable::CaptureSurface uniParams is nullptr");
        return;
    }
    auto isSnapshotSkipLayer =
        RSUniRenderThread::GetCaptureParam().isSnapshot_ && surfaceParams.GetIsSnapshotSkipLayer();
    if (UNLIKELY(surfaceParams.GetIsSecurityLayer() && !uniParams->GetSecExemption()) ||
        surfaceParams.GetIsSkipLayer() || isSnapshotSkipLayer) {
        RS_LOGD("RSSurfaceRenderNodeDrawable::CaptureSurface: \
            process RSSurfaceRenderNode(id:[%{public}" PRIu64 "] name:[%{public}s]) with security or skip layer.",
            surfaceParams.GetId(), name_.c_str());
        RS_TRACE_NAME("CaptureSurface with security or skip layer");
        if (RSUniRenderThread::GetCaptureParam().isSingleSurface_) {
            Drawing::Brush rectBrush;
            rectBrush.SetColor(Drawing::Color::COLOR_WHITE);
            canvas.AttachBrush(rectBrush);
            canvas.DrawRect(Drawing::Rect(0, 0, surfaceParams.GetBounds().GetWidth(),
                surfaceParams.GetBounds().GetHeight()));
            canvas.DetachBrush();
        }
        return;
    }

    if (surfaceParams.GetIsProtectedLayer()) {
        RS_LOGD("RSSurfaceRenderNodeDrawable::CaptureSurface: \
            process RSSurfaceRenderNode(id:[%{public}" PRIu64 "] name:[%{public}s]) with protected layer.",
            surfaceParams.GetId(), name_.c_str());
        Drawing::Brush rectBrush;
        rectBrush.SetColor(Drawing::Color::COLOR_BLACK);
        canvas.AttachBrush(rectBrush);
        canvas.DrawRect(Drawing::Rect(0, 0, surfaceParams.GetBounds().GetWidth(),
            surfaceParams.GetBounds().GetHeight()));
        canvas.DetachBrush();
        return;
    }

    RS_LOGD("HDR hasHdrPresent_:%{public}d", canvas.IsCapture());
    bool hasHidePrivacyContent = surfaceParams.HasPrivacyContentLayer() &&
        RSUniRenderThread::GetCaptureParam().isSingleSurface_ &&
        !RSUniRenderThread::GetCaptureParam().isSystemCalling_;
    if (!(surfaceParams.HasSecurityLayer() || surfaceParams.HasSkipLayer() || surfaceParams.HasSnapshotSkipLayer() ||
        surfaceParams.HasProtectedLayer() || hasHdrPresent_ || hasHidePrivacyContent)) {
        if (drawWindowCache_.DealWithCachedWindow(this, canvas, surfaceParams)) {
            if (RSUniRenderThread::GetCaptureParam().isSingleSurface_) {
                RS_LOGI("%{public}s DealWithCachedWindow", __func__);
            }
            return;
        }
        if (DealWithUIFirstCache(canvas, surfaceParams, *uniParams)) {
            if (RSUniRenderThread::GetCaptureParam().isSingleSurface_) {
                RS_LOGI("%{public}s DealWithUIFirstCache", __func__);
            }
            return;
        }
    }

    if (!RSUiFirstProcessStateCheckerHelper::CheckMatchAndWaitNotify(surfaceParams, false)) {
        RS_LOGE("RSSurfaceRenderNodeDrawable::OnCapture CheckMatchAndWaitNotify failed");
        return;
    }

    RSRenderNodeSingleDrawableLocker singleLocker(this);
    if (UNLIKELY(!singleLocker.IsLocked())) {
        singleLocker.DrawableOnDrawMultiAccessEventReport(__func__);
        RS_LOGE("RSSurfaceRenderNodeDrawable::CaptureSurface node %{public}" PRIu64 " onDraw!!!", GetId());
        return;
    }

    bool isSelfDrawingSurface = surfaceParams.GetSurfaceNodeType() == RSSurfaceNodeType::SELF_DRAWING_NODE &&
        !surfaceParams.IsSpherizeValid() && !surfaceParams.IsAttractionValid();
    if (isSelfDrawingSurface) {
        SetSkip(surfaceParams.GetBuffer() != nullptr ? SkipType::SKIP_BACKGROUND_COLOR : SkipType::NONE);
        // Restore in OnGeneralProcess
        canvas.Save();
    }

    auto parentSurfaceMatrix = RSRenderParams::GetParentSurfaceMatrix();
    RSRenderParams::SetParentSurfaceMatrix(canvas.GetTotalMatrix());

    OnGeneralProcess(canvas, surfaceParams, isSelfDrawingSurface);

    RSRenderParams::SetParentSurfaceMatrix(parentSurfaceMatrix);
}

GraphicColorGamut RSSurfaceRenderNodeDrawable::GetAncestorDisplayColorGamut(const RSSurfaceRenderParams& surfaceParams)
{
    GraphicColorGamut targetColorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB;
    auto ancestorDrawable = surfaceParams.GetAncestorDisplayDrawable().lock();
    if (!ancestorDrawable) {
        RS_LOGE("ancestorDrawable return nullptr");
        return targetColorGamut;
    }
    auto ancestorDisplayDrawable = std::static_pointer_cast<RSDisplayRenderNodeDrawable>(ancestorDrawable);
    if (!ancestorDisplayDrawable) {
        RS_LOGE("ancestorDisplayDrawable return nullptr");
        return targetColorGamut;
    }
    auto& ancestorParam = ancestorDrawable->GetRenderParams();
    if (!ancestorParam) {
        RS_LOGE("ancestorParam return nullptr");
        return targetColorGamut;
    }

    auto renderParams = static_cast<RSDisplayRenderParams*>(ancestorParam.get());
    targetColorGamut = renderParams->GetNewColorSpace();
    RS_LOGD("params.targetColorGamut is %{public}d in DealWithSelfDrawingNodeBuffer", targetColorGamut);
    return targetColorGamut;
}

void RSSurfaceRenderNodeDrawable::DealWithSelfDrawingNodeBuffer(
    RSPaintFilterCanvas& canvas, RSSurfaceRenderParams& surfaceParams)
{
    if (surfaceParams.GetHardwareEnabled() && !RSUniRenderThread::IsInCaptureProcess()) {
        if (!IsHardwareEnabledTopSurface() && !surfaceParams.IsLayerTop()) {
            ClipHoleForSelfDrawingNode(canvas, surfaceParams);
        }
        if (surfaceParams.GetNeedMakeImage()) {
            RS_TRACE_NAME_FMT("DealWithSelfDrawingNodeBuffer Id:%" PRIu64 "", surfaceParams.GetId());
            RSAutoCanvasRestore arc(&canvas);
            surfaceParams.SetGlobalAlpha(1.0f);
            pid_t threadId = gettid();
            auto params = RSUniRenderUtil::CreateBufferDrawParam(*this, false, threadId);

            Drawing::Matrix rotateMatrix = canvas.GetTotalMatrix();
            rotateMatrix.PreConcat(params.matrix);

            auto renderEngine = RSUniRenderThread::Instance().GetRenderEngine();
            if (!renderEngine) {
                RS_LOGE("DealWithSelfDrawingNodeBuffer renderEngine is nullptr");
                return;
            }
            VideoInfo videoInfo;
            auto surfaceNodeImage = renderEngine->CreateImageFromBuffer(canvas, params, videoInfo);

            SurfaceNodeInfo surfaceNodeInfo = {surfaceNodeImage, rotateMatrix, params.srcRect, params.dstRect};
            HveFilter::GetHveFilter().PushSurfaceNodeInfo(surfaceNodeInfo);
        }
        return;
    }
    if (surfaceParams.GetIsProtectedLayer()) {
        RS_LOGD("protected layer cannot draw in non-protected context.");
        return;
    }
    if (surfaceParams.IsInFixedRotation()) {
        DrawBufferForRotationFixed(canvas, surfaceParams);
        return;
    }

    RSAutoCanvasRestore arc(&canvas);
    surfaceParams.SetGlobalAlpha(1.0f);
    pid_t threadId = gettid();
    auto params = RSUniRenderUtil::CreateBufferDrawParam(*this, false, threadId);
    params.targetColorGamut = GetAncestorDisplayColorGamut(surfaceParams);
#ifdef USE_VIDEO_PROCESSING_ENGINE
    params.sdrNits = surfaceParams.GetSdrNit();
    params.tmoNits = surfaceParams.GetDisplayNit();
    params.displayNits = params.tmoNits / std::pow(surfaceParams.GetBrightnessRatio(), GAMMA2_2); // gamma 2.2
#endif
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
    if (IsHardwareEnabledTopSurface() && RSUniRenderThread::Instance().GetRSRenderThreadParams()->HasMirrorDisplay()) {
        RSPointerRenderManager::GetInstance().SetCacheImgForPointer(canvas.GetSurface()->GetImageSnapshot());
    }
#endif

    DrawSelfDrawingNodeBuffer(canvas, surfaceParams, params);
}

void RSSurfaceRenderNodeDrawable::ClipHoleForSelfDrawingNode(RSPaintFilterCanvas& canvas,
    RSSurfaceRenderParams& surfaceParams)
{
    RSAutoCanvasRestore arc(&canvas);
    auto bounds = surfaceParams.GetBounds();
    canvas.ClipRect({std::round(bounds.GetLeft()), std::round(bounds.GetTop()),
        std::round(bounds.GetRight()), std::round(bounds.GetBottom())});
    canvas.Clear(Drawing::Color::COLOR_TRANSPARENT);
}

void RSSurfaceRenderNodeDrawable::DrawBufferForRotationFixed(RSPaintFilterCanvas& canvas,
    RSSurfaceRenderParams& surfaceParams)
{
    ClipHoleForSelfDrawingNode(canvas, surfaceParams);

    Drawing::Brush brush;
    brush.SetBlendMode(Drawing::BlendMode::DST_OVER);
    Drawing::SaveLayerOps layerOps(nullptr, &brush);
    canvas.SaveLayer(layerOps);

    Drawing::Matrix inverse;
    if (!(surfaceParams.GetLayerInfo().matrix.Invert(inverse))) {
        RS_LOGE("DrawBufferForRotationFixed failed to get invert matrix");
    }
    canvas.ConcatMatrix(inverse);
    auto params = RSUniRenderUtil::CreateBufferDrawParamForRotationFixed(*this, surfaceParams);
    RSUniRenderThread::Instance().GetRenderEngine()->DrawSurfaceNodeWithParams(canvas, *this, params);
    canvas.Restore();
}

void RSSurfaceRenderNodeDrawable::DrawSelfDrawingNodeBuffer(
    RSPaintFilterCanvas& canvas, const RSSurfaceRenderParams& surfaceParams, BufferDrawParam& params)
{
    auto bgColor = surfaceParams.GetBackgroundColor();
    auto renderEngine = RSUniRenderThread::Instance().GetRenderEngine();
    if ((surfaceParams.GetSelfDrawingNodeType() != SelfDrawingNodeType::VIDEO) &&
        (bgColor != RgbPalette::Transparent())) {
        Drawing::Brush brush;
        brush.SetColor(Drawing::Color(bgColor.AsArgbInt()));
        if (HasCornerRadius(surfaceParams)) {
            auto bounds = RSPropertiesPainter::Rect2DrawingRect({ 0, 0,
                std::round(surfaceParams.GetBounds().GetWidth()), std::round(surfaceParams.GetBounds().GetHeight()) });
            Drawing::SaveLayerOps layerOps(&bounds, nullptr);
            canvas.SaveLayer(layerOps);
            canvas.AttachBrush(brush);
            canvas.DrawRoundRect(RSPropertiesPainter::RRect2DrawingRRect(surfaceParams.GetRRect()));
            canvas.DetachBrush();
            renderEngine->DrawSurfaceNodeWithParams(canvas, *this, params);
            canvas.Restore();
        } else {
            canvas.AttachBrush(brush);
            canvas.DrawRect(surfaceParams.GetBounds());
            canvas.DetachBrush();
            renderEngine->DrawSurfaceNodeWithParams(canvas, *this, params);
        }
    } else {
        renderEngine->DrawSurfaceNodeWithParams(canvas, *this, params);
    }
}

bool RSSurfaceRenderNodeDrawable::HasCornerRadius(const RSSurfaceRenderParams& surfaceParams) const
{
    auto rrect = surfaceParams.GetRRect();
    for (auto index = 0; index < CORNER_SIZE; ++index) {
        if (!ROSEN_EQ(rrect.radius_[index].x_, 0.f) || !ROSEN_EQ(rrect.radius_[index].y_, 0.f)) {
            return true;
        }
    }
    return false;
}

bool RSSurfaceRenderNodeDrawable::DealWithUIFirstCache(
    RSPaintFilterCanvas& canvas, RSSurfaceRenderParams& surfaceParams, RSRenderThreadParams& uniParams)
{
    auto enableType = surfaceParams.GetUifirstNodeEnableParam();
    auto cacheState = GetCacheSurfaceProcessedStatus();
    if ((!RSUniRenderThread::GetCaptureParam().isSnapshot_ && enableType == MultiThreadCacheType::NONE &&
        // WAITING may change to DOING in subThread at any time
        cacheState != CacheProcessStatus::WAITING && cacheState != CacheProcessStatus::DOING) ||
        (RSUniRenderThread::GetCaptureParam().isSnapshot_ && !HasCachedTexture())) {
        return false;
    }
    RS_TRACE_NAME_FMT("DrawUIFirstCache [%s] %" PRIu64 ", type %d",
        name_.c_str(), surfaceParams.GetId(), enableType);
    RSUifirstManager::Instance().AddReuseNode(surfaceParams.GetId());
    Drawing::Rect bounds = GetRenderParams() ? GetRenderParams()->GetBounds() : Drawing::Rect(0, 0, 0, 0);
    RSAutoCanvasRestore acr(&canvas);
    // Alpha and matrix have been applied in func CaptureSurface
    if (!RSUniRenderThread::GetCaptureParam().isSnapshot_ && !RSUniRenderThread::GetCaptureParam().isMirror_) {
        canvas.MultiplyAlpha(surfaceParams.GetAlpha());
        canvas.ConcatMatrix(surfaceParams.GetMatrix());
    }
    DrawBackground(canvas, bounds);
    bool drawCacheSuccess = true;
    if (surfaceParams.GetUifirstUseStarting() != INVALID_NODEID) {
        drawCacheSuccess = DrawUIFirstCacheWithStarting(canvas, surfaceParams.GetUifirstUseStarting());
    } else {
        bool canSkipFirstWait = (enableType == MultiThreadCacheType::ARKTS_CARD) &&
            uniParams.GetUIFirstCurrentFrameCanSkipFirstWait();
        drawCacheSuccess = DrawUIFirstCache(canvas, canSkipFirstWait);
    }
    if (!drawCacheSuccess) {
        SetDrawSkipType(DrawSkipType::UI_FIRST_CACHE_FAIL);
        RS_TRACE_NAME_FMT("[%s] reuse failed!", name_.c_str());
    }
    DrawForeground(canvas, bounds);
    if (uniParams.GetUIFirstDebugEnabled()) {
        DrawUIFirstDfx(canvas, enableType, surfaceParams, drawCacheSuccess);
    }
    return true;
}

void RSSurfaceRenderNodeDrawable::DrawUIFirstDfx(RSPaintFilterCanvas& canvas, MultiThreadCacheType enableType,
    RSSurfaceRenderParams& surfaceParams, bool drawCacheSuccess)
{
    auto sizeDebug = surfaceParams.GetCacheSize();
    Drawing::Brush rectBrush;
    if (drawCacheSuccess) {
        if (enableType == MultiThreadCacheType::ARKTS_CARD) {
            // rgba: Alpha 128, blue 128
            rectBrush.SetColor(Drawing::Color(0, 0, 128, 128));
        } else {
            // rgba: Alpha 128, green 128, blue 128
            rectBrush.SetColor(Drawing::Color(0, 128, 128, 128));
        }
    } else {
        // rgba: Alpha 128, red 128
        rectBrush.SetColor(Drawing::Color(128, 0, 0, 128));
    }
    canvas.AttachBrush(rectBrush);
    canvas.DrawRect(Drawing::Rect(0, 0, sizeDebug.x_, sizeDebug.y_));
    canvas.DetachBrush();
}

void RSSurfaceRenderNodeDrawable::EnableGpuOverDrawDrawBufferOptimization(Drawing::Canvas& canvas,
    RSSurfaceRenderParams* surfaceParams)
{
    const Vector4f& radius = surfaceParams->GetOverDrawBufferNodeCornerRadius();
    if (radius.IsZero()) {
        return;
    }
    RS_OPTIONAL_TRACE_NAME_FMT("EnableGpuOverDrawDrawBufferOptimization Id:%" PRIu64 "", surfaceParams->GetId());
    const Drawing::Rect& bounds = surfaceParams->GetFrameRect();
    Drawing::Brush brush;
    // must set src blend mode, so overdraw buffer feature can enabled.
    brush.SetBlendMode(Drawing::BlendMode::SRC);
    // cause the rect will be covered by the child background node, so we just add a white rect
    brush.SetColor(Drawing::Color::COLOR_WHITE);
    canvas.AttachBrush(brush);
    Drawing::AutoCanvasRestore arc(canvas, true);
    canvas.Translate(radius.x_, radius.y_);
    canvas.DrawRect(Drawing::Rect {0, 0, bounds.GetWidth() - 2 * radius.x_, bounds.GetHeight() - 2 * radius.y_});
    canvas.DetachBrush();
}

const Occlusion::Region& RSSurfaceRenderNodeDrawable::GetVisibleDirtyRegion() const
{
    return visibleDirtyRegion_;
}

void RSSurfaceRenderNodeDrawable::SetVisibleDirtyRegion(const Occlusion::Region& region)
{
    visibleDirtyRegion_ = region;
}

void RSSurfaceRenderNodeDrawable::SetAlignedVisibleDirtyRegion(const Occlusion::Region& alignedRegion)
{
    alignedVisibleDirtyRegion_ = alignedRegion;
}

void RSSurfaceRenderNodeDrawable::SetGlobalDirtyRegion(Occlusion::Region region)
{
    globalDirtyRegion_ = region;
}

const Occlusion::Region& RSSurfaceRenderNodeDrawable::GetGlobalDirtyRegion() const
{
    return globalDirtyRegion_;
}

void RSSurfaceRenderNodeDrawable::SetDirtyRegionAlignedEnable(bool enable)
{
    isDirtyRegionAlignedEnable_ = enable;
}

void RSSurfaceRenderNodeDrawable::SetDirtyRegionBelowCurrentLayer(Occlusion::Region& region)
{
#ifndef ROSEN_CROSS_PLATFORM
    if (!renderParams_) {
        return;
    }
    Occlusion::Rect dirtyRect { renderParams_->GetOldDirtyInSurface() };
    Occlusion::Region dirtyRegion { dirtyRect };
    dirtyRegionBelowCurrentLayer_ = dirtyRegion.And(region);
    dirtyRegionBelowCurrentLayerIsEmpty_ = dirtyRegionBelowCurrentLayer_.IsEmpty();
#endif
}

std::shared_ptr<RSDirtyRegionManager> RSSurfaceRenderNodeDrawable::GetSyncDirtyManager() const
{
    return syncDirtyManager_;
}

#ifndef ROSEN_CROSS_PLATFORM
void RSSurfaceRenderNodeDrawable::RegisterDeleteBufferListenerOnSync(sptr<IConsumerSurface> consumer)
{
    auto renderEngine = RSUniRenderThread::Instance().GetRenderEngine();
    if (!renderEngine || !consumerOnDraw_) {
        return;
    }
    renderEngine->RegisterDeleteBufferListener(consumerOnDraw_);
}
#endif

} // namespace OHOS::Rosen::DrawableV2
