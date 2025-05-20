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
#include "rs_frame_report.h"
#include "common/rs_color.h"
#include "common/rs_common_def.h"
#include "common/rs_optional_trace.h"
#include "common/rs_obj_abs_geometry.h"
#include "common/rs_special_layer_manager.h"
#include "display_engine/rs_luminance_control.h"
#include "draw/brush.h"
#include "drawable/rs_display_render_node_drawable.h"
#include "feature/uifirst/rs_sub_thread_manager.h"
#include "feature/uifirst/rs_uifirst_manager.h"
#include "graphic_feature_param_manager.h"
#include "memory/rs_tag_tracker.h"
#include "params/rs_display_render_params.h"
#include "params/rs_surface_render_params.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "pipeline/render_thread/rs_uni_render_util.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_surface_handler.h"
#include "pipeline/rs_surface_render_node.h"
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
#include "pipeline/magic_pointer_render/rs_magic_pointer_render_manager.h"
#endif
#include "gfx/fps_info/rs_surface_fps_manager.h"

#include "platform/common/rs_log.h"
#include "platform/ohos/rs_node_stats.h"
#include "utils/graphic_coretrace.h"
#include "utils/rect.h"
#include "utils/region.h"

#include "pipeline/main_thread/rs_main_thread.h"
#include "static_factory.h"
#ifdef RS_ENABLE_VK
#include "include/gpu/GrBackendSurface.h"
#include "platform/ohos/backend/native_buffer_utils.h"
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif
#include "render/rs_high_performance_visual_engine.h"
#include "render/rs_pixel_map_util.h"
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
    if (surfaceNode->GetSurfaceWindowType() != SurfaceWindowType::SCB_SCREEN_LOCK) {
        vmaCacheOff_ = true;
    }
    surfaceNodeType_ = surfaceNode->GetSurfaceNodeType();
#ifndef ROSEN_CROSS_PLATFORM
    consumerOnDraw_ = surfaceNode->GetRSSurfaceHandler()->GetConsumer();
#endif
    subThreadCache_.SetNodeId(surfaceNode->GetId());
}

RSRenderNodeDrawable::Ptr RSSurfaceRenderNodeDrawable::OnGenerate(std::shared_ptr<const RSRenderNode> node)
{
    RS_TRACE_NAME("RSRenderNodeDrawable::Ptr RSSurfaceRenderNodeDrawable::OnGenerate");
    return new RSSurfaceRenderNodeDrawable(std::move(node));
}

bool RSSurfaceRenderNodeDrawable::CheckDrawAndCacheWindowContent(RSSurfaceRenderParams& surfaceParams,
    RSRenderThreadParams& uniParams) const
{
    if (!surfaceParams.GetNeedCacheSurface()) {
        return false;
    }

    if (surfaceParams.IsCloneNode() && RSUniRenderThread::GetCaptureParam().isSnapshot_) {
        return false;
    }

    if (!surfaceParams.IsCrossNode()) {
        return true;
    }
    if (uniParams.IsFirstVisitCrossNodeDisplay() &&
        RSUniRenderThread::IsExpandScreenMode() && !uniParams.HasDisplayHdrOn() &&
        uniParams.GetCrossNodeOffScreenStatus() != CrossNodeOffScreenRenderDebugType::DISABLED) {
        RS_TRACE_NAME_FMT("%s cache cross node[%s]", __func__, GetName().c_str());
        return true;
    }
    return false;
}

void RSSurfaceRenderNodeDrawable::OnGeneralProcess(RSPaintFilterCanvas& canvas,
    RSSurfaceRenderParams& surfaceParams, RSRenderThreadParams& uniParams, bool isSelfDrawingSurface)
{
    RECORD_GPURESOURCE_CORETRACE_CALLER(Drawing::CoreFunction::
        RS_RSSURFACERENDERNODEDRAWABLE_ONGENERALPROCESS);
    auto bounds = surfaceParams.GetFrameRect();

    if (surfaceParams.GetGlobalPositionEnabled()) {
        auto matrix = surfaceParams.GetTotalMatrix();
        matrix.Translate(-offsetX_, -offsetY_);
        canvas.ConcatMatrix(matrix);
        if (!lastGlobalPositionEnabled_) {
            lastGlobalPositionEnabled_ = true;
            RS_LOGI("RSSurfaceRenderNodeDrawable::%{public}s Translate screenId=[%{public}" PRIu64 "] "
                "offsetX=%{public}d offsetY=%{public}d", __func__, curDisplayScreenId_, offsetX_, offsetY_);
        }
        RS_TRACE_NAME_FMT("RSSurfaceRenderNodeDrawable::%s Translate screenId=[%" PRIu64 "] "
            "offsetX=%d offsetY=%d", __func__, curDisplayScreenId_, offsetX_, offsetY_);
    } else if (lastGlobalPositionEnabled_) {
        lastGlobalPositionEnabled_ = false;
    }

    // 1. draw background
    if (surfaceParams.IsLeashWindow()) {
        DrawLeashWindowBackground(canvas, bounds,
            uniParams.IsStencilPixelOcclusionCullingEnabled(), surfaceParams.GetStencilVal());
    } else {
        DrawBackground(canvas, bounds);
    }

    // 2. draw self drawing node
    if (surfaceParams.GetBuffer() != nullptr) {
        DealWithSelfDrawingNodeBuffer(canvas, surfaceParams);
    }

    if (isSelfDrawingSurface) {
        canvas.Restore();
    }

    if (CheckDrawAndCacheWindowContent(surfaceParams, uniParams)) {
        // 3/4 Draw content and children of this node by the main canvas, and cache
        subThreadCache_.GetRSDrawWindowCache().DrawAndCacheWindowContent(this, canvas, surfaceParams.GetBounds());
    } else {
        // 3. Draw content of this node by the main canvas.
        DrawContent(canvas, bounds);

        // 4. Draw children of this node by the main canvas.
        DrawChildren(canvas, bounds);
    }

    // 5. Draw foreground of this node by the main canvas.
    DrawForeground(canvas, bounds);

    DrawWatermark(canvas, surfaceParams);

    if (surfaceParams.IsCrossNode() &&
        uniParams.GetCrossNodeOffScreenStatus() == CrossNodeOffScreenRenderDebugType::ENABLE_DFX) {
        // rgba: Alpha 128, green 128, blue 128
        Drawing::Color color(0, 128, 128, 128);
        subThreadCache_.GetRSDrawWindowCache().DrawCrossNodeOffscreenDFX(canvas, surfaceParams, uniParams, color);
    }
}

void RSSurfaceRenderNodeDrawable::DrawWatermark(RSPaintFilterCanvas& canvas, const RSSurfaceRenderParams& params)
{
    if (params.IsWatermarkEmpty()) {
        RS_LOGE("SurfaceNodeDrawable DrawWatermark Name:%{public}s Id:%{public}" PRIu64 " water mark count is zero",
            GetName().c_str(), params.GetId());
        return;
    }
    auto& renderThreadParams = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    if (!renderThreadParams) {
        RS_LOGE("SurfaceNodeDrawable DrawWatermark renderThreadParams is nullptr");
        return;
    }
    auto surfaceRect = params.GetBounds();
    for (auto& [name, isEnabled] : params.GetWatermarksEnabled()) {
        if (!isEnabled) {
            continue;
        }
        auto watermark = renderThreadParams->GetWatermark(name);
        if (!watermark) {
            continue;
        }
        auto imagePtr = RSPixelMapUtil::ExtractDrawingImage(watermark);
        if (!imagePtr || imagePtr->GetWidth() == 0 || imagePtr->GetHeight() == 0) {
            continue;
        }
        auto imageRect = Drawing::Rect(0, 0, imagePtr->GetWidth(), imagePtr->GetHeight());
        Drawing::Brush brush;
        brush.SetShaderEffect(Drawing::ShaderEffect::CreateImageShader(
            *imagePtr, Drawing::TileMode::REPEAT, Drawing::TileMode::REPEAT,
            Drawing::SamplingOptions(), Drawing::Matrix()));
        canvas.AttachBrush(brush);
        canvas.DrawRect(surfaceRect);
        canvas.DetachBrush();
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
    if (uniParam.IsOcclusionEnabled() && visibleRegion.IsEmpty() && !surfaceParams.IsFirstLevelCrossNode()) {
        return resultRegion;
    }
    // The region is dirty region of this SurfaceNode.
    Occlusion::Region dirtyRegion;
    for (const auto& rect : GetSyncDirtyManager()->GetDirtyRegionForQuickReject()) {
        Occlusion::Region region = Occlusion::Region(Occlusion::Rect(rect));
        dirtyRegion.OrSelf(region);
    }
    // visibility of cross-display surface (which is generally at the top) is ignored.
    auto visibleDirtyRegion = surfaceParams.IsFirstLevelCrossNode() ? dirtyRegion : dirtyRegion.And(visibleRegion);
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
#ifdef RS_ENABLE_GPU
    RSTagTracker tagTracker(canvasBackup_->GetGPUContext().get(),
        RSTagTracker::SOURCETYPE::SOURCE_FINISHOFFSCREENRENDER);
#endif
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

bool RSSurfaceRenderNodeDrawable::IsHardwareEnabledTopSurface() const
{
    return surfaceNodeType_ == RSSurfaceNodeType::CURSOR_NODE && RSSystemProperties::GetHardCursorEnabled();
}

void RSSurfaceRenderNodeDrawable::PreprocessUnobscuredUEC(RSPaintFilterCanvas& canvas)
{
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(GetRenderParams().get());
    if (!surfaceParams) {
        RS_LOGE("RSSurfaceRenderNodeDrawable::PreprocessUnobscuredUEC params is nullptr");
        return;
    }
    if (!surfaceParams->IsUnobscuredUIExtension()) {
        return;
    }
    canvas.ResetMatrix();
    auto& unobscuredUECMatrixMap = GetUnobscuredUECMatrixMap();
    if (unobscuredUECMatrixMap.find(GetId()) == unobscuredUECMatrixMap.end()) {
        RS_LOGE("PreprocessUnobscuredUEC can't find matrix of cached node in unobscuredMatrixMap");
        return;
    }
    canvas.ConcatMatrix(unobscuredUECMatrixMap.at(GetId()));
}

bool RSSurfaceRenderNodeDrawable::DrawCacheImageForMultiScreenView(RSPaintFilterCanvas& canvas,
    const RSSurfaceRenderParams& surfaceParams)
{
    auto sourceDisplayNodeDrawable =
        std::static_pointer_cast<RSDisplayRenderNodeDrawable>(
            surfaceParams.GetSourceDisplayRenderNodeDrawable().lock());
    if (sourceDisplayNodeDrawable) {
        auto cacheImgForMultiScreenView = sourceDisplayNodeDrawable->GetCacheImgForMultiScreenView();
        if (cacheImgForMultiScreenView) {
            RS_TRACE_NAME_FMT("DrawCacheImageForMultiScreenView with cache id:%llu rect:%s",
                surfaceParams.GetId(), surfaceParams.GetRRect().rect_.ToString().c_str());
            RSUniRenderUtil::ProcessCacheImageForMultiScreenView(canvas, *cacheImgForMultiScreenView,
                surfaceParams.GetRRect().rect_);
        } else {
            RS_TRACE_NAME_FMT("DrawCacheImageForMultiScreenView without cache id:%llu", surfaceParams.GetId());
        }
        return true;
    }
    return false;
}

void RSSurfaceRenderNodeDrawable::OnDraw(Drawing::Canvas& canvas)
{
    RECORD_GPURESOURCE_CORETRACE_CALLER_WITHNODEID(Drawing::CoreFunction::
        RS_RSSURFACERENDERNODEDRAWABLE_ONDRAW, GetId());
    SetDrawSkipType(DrawSkipType::NONE);
    if (!ShouldPaint()) {
        SetDrawSkipType(DrawSkipType::SHOULD_NOT_PAINT);
        RS_TRACE_NAME_FMT("RSSurfaceRenderNodeDrawable::OnDraw %s should not paint", name_.c_str());
        return;
    }

    if (vmaCacheOff_) {
        Drawing::StaticFactory::SetVmaCacheStatus(false); // render this frame with vma cache off
    }
    Drawing::GPUResourceTag::SetCurrentNodeId(GetId());
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
    if (surfaceParams->IsUnobscuredUIExtension() && !UIExtensionNeedToDraw()) {
        RS_LOGE("Current Unobsucred UEC[%{public}s,%{public}" PRIu64 "] needn't to draw",
            name_.c_str(), surfaceParams->GetId());
        return;
    }
    if (DrawCacheImageForMultiScreenView(*rscanvas, *surfaceParams)) {
        return;
    }
    auto cloneSourceDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
        surfaceParams->GetCloneSourceDrawable().lock());
    auto cloneSourceParams = cloneSourceDrawable ? cloneSourceDrawable->GetRenderParams().get() : nullptr;
    if (cloneSourceParams) {
        cloneSourceDrawable->OnDraw(*rscanvas);
        return;
    }
    if (DrawCloneNode(*rscanvas, *uniParam, *surfaceParams, false)) {
        return;
    }
    if (surfaceParams->GetSkipDraw()) {
        SetDrawSkipType(DrawSkipType::SURFACE_PARAMS_SKIP_DRAW);
        RS_TRACE_NAME_FMT("RSSurfaceRenderNodeDrawable::OnDraw SkipDraw [%s] Id:%" PRIu64 "",
            name_.c_str(), surfaceParams->GetId());
        return;
    }
    if (CheckIfSurfaceSkipInMirror(*surfaceParams)) {
        SetDrawSkipType(DrawSkipType::SURFACE_SKIP_IN_MIRROR);
        RS_TRACE_NAME_FMT("RSSurfaceRenderNodeDrawable::OnDraw surface skipped in mirror name:[%s] id:%" PRIu64,
            name_.c_str(), surfaceParams->GetId());
        return;
    }
    RS_LOGD("RSSurfaceRenderNodeDrawable ondraw name:%{public}s nodeId:[%{public}" PRIu64 "]", name_.c_str(),
        surfaceParams->GetId());

    auto renderEngine = RSUniRenderThread::Instance().GetRenderEngine();
    if (!renderEngine) {
        SetDrawSkipType(DrawSkipType::RENDER_ENGINE_NULL);
        RS_LOGE("RSSurfaceRenderNodeDrawable::OnDraw renderEngine is nullptr");
        return;
    }
    if (autoCacheEnable_) {
        nodeCacheType_ = NodeStrategyType::CACHE_NONE;
    }
    // syncDirtyManager_ is not null
    const RectI& currentFrameDirty = syncDirtyManager_->GetCurrentFrameDirtyRegion();
    const auto& absDrawRect = surfaceParams->GetAbsDrawRect();
    bool isUiFirstNode = rscanvas->GetIsParallelCanvas();
    bool disableFilterCache = rscanvas->GetDisableFilterCache();
    if (!disableFilterCache && !isUiFirstNode && surfaceParams->GetOccludedByFilterCache()) {
        SetDrawSkipType(DrawSkipType::FILTERCACHE_OCCLUSION_SKIP);
        RS_TRACE_NAME_FMT("RSSurfaceRenderNodeDrawable::OnDraw[%s](%d, %d, %d, %d) filterCache occlusion skip, "
            "id:%" PRIu64 ", alpha:%f, dirty(%d, %d, %d, %d)", name_.c_str(), absDrawRect.left_, absDrawRect.top_,
            absDrawRect.width_, absDrawRect.height_, surfaceParams->GetId(), surfaceParams->GetGlobalAlpha(),
            currentFrameDirty.left_, currentFrameDirty.top_, currentFrameDirty.width_, currentFrameDirty.height_);
        return;
    }
    auto specialLayerManager = surfaceParams->GetSpecialLayerMgr();
    hasSkipCacheLayer_ =
        (specialLayerManager.Find(SpecialLayerType::SECURITY) && !uniParam->GetSecExemption()) ||
            specialLayerManager.Find(SpecialLayerType::SKIP);
    if (hasSkipCacheLayer_ && curDrawingCacheRoot_) {
        curDrawingCacheRoot_->SetSkipCacheLayer(true);
    }
    if (surfaceParams->GetHardCursorStatus()) {
        SetDrawSkipType(DrawSkipType::HARD_CURSOR_ENAbLED);
        RS_TRACE_NAME_FMT("RSSurfaceRenderNodeDrawable::OnDraw hardcursor skip SurfaceName:%s", name_.c_str());
        return;
    }

    Drawing::Region curSurfaceDrawRegion = CalculateVisibleDirtyRegion(*uniParam, *surfaceParams, *this, isUiFirstNode);

    if (!isUiFirstNode) {
        if (uniParam->IsOpDropped() && surfaceParams->IsVisibleDirtyRegionEmpty(curSurfaceDrawRegion)) {
            SetDrawSkipType(DrawSkipType::OCCLUSION_SKIP);
            RS_TRACE_NAME_FMT("RSSurfaceRenderNodeDrawable::OnDraw[%s](%d, %d, %d, %d) occlusion skip, "
                "id:%" PRIu64 ", alpha:%f, dirty(%d, %d, %d, %d)", name_.c_str(), absDrawRect.left_, absDrawRect.top_,
                absDrawRect.width_, absDrawRect.height_, surfaceParams->GetId(), surfaceParams->GetGlobalAlpha(),
                currentFrameDirty.left_, currentFrameDirty.top_, currentFrameDirty.width_, currentFrameDirty.height_);
            return;
        }
    }
    const RectI& mergeHistoryDirty = syncDirtyManager_->GetDirtyRegion();
    // warning : don't delete this trace or change trace level to optional !!!
    RS_TRACE_NAME_FMT("RSSurfaceRenderNodeDrawable::OnDraw[%s](%d, %d, %d, %d), id:%" PRIu64 ", alpha:%f"
        ", preSub:%d, dirty(%d, %d, %d, %d), mergedDirty(%d, %d, %d, %d)", name_.c_str(), absDrawRect.left_,
        absDrawRect.top_, absDrawRect.width_, absDrawRect.height_, GetId(), surfaceParams->GetGlobalAlpha(),
        surfaceParams->GetPreSubHighPriorityType(),
        currentFrameDirty.left_, currentFrameDirty.top_, currentFrameDirty.width_, currentFrameDirty.height_,
        mergeHistoryDirty.left_, mergeHistoryDirty.top_, mergeHistoryDirty.width_, mergeHistoryDirty.height_);

    RS_LOGD("RSSurfaceRenderNodeDrawable::OnDraw node:%{public}" PRIu64 ", name:%{public}s,"
            "OcclusionVisible:%{public}d Bound:%{public}s",
        surfaceParams->GetId(), name_.c_str(), surfaceParams->GetOcclusionVisible(),
        surfaceParams->GetBounds().ToString().c_str());

    RSUiFirstProcessStateCheckerHelper stateCheckerHelper(
        surfaceParams->GetFirstLevelNodeId(), surfaceParams->GetUifirstRootNodeId(), nodeId_);
    // Regional screen recording does not enable uifirst.
    bool enableVisiableRect = RSUniRenderThread::Instance().GetEnableVisiableRect();
    if (!enableVisiableRect) {
        if (subThreadCache_.DealWithUIFirstCache(this, *rscanvas, *surfaceParams, *uniParam)) {
            if (GetDrawSkipType() == DrawSkipType::NONE) {
                SetDrawSkipType(DrawSkipType::UI_FIRST_CACHE_SKIP);
            }
            return;
        }
    }

    // can't use NodeMatchOptimize if leash window is on draw
    auto cacheState = RSUifirstManager::Instance().GetCacheSurfaceProcessedStatus(*surfaceParams);
    auto useNodeMatchOptimize = rscanvas->GetIsParallelCanvas() &&
        (cacheState == CacheProcessStatus::WAITING || cacheState == CacheProcessStatus::DOING);
    if (!RSUiFirstProcessStateCheckerHelper::CheckMatchAndWaitNotify(*surfaceParams, useNodeMatchOptimize)) {
        SetDrawSkipType(DrawSkipType::CHECK_MATCH_AND_WAIT_NOTIFY_FAIL);
        RS_LOGE("RSSurfaceRenderNodeDrawable::OnDraw CheckMatchAndWaitNotify failed");
        return;
    }

    RSRenderNodeSingleDrawableLocker singleLocker(this);
    if (UNLIKELY(!singleLocker.IsLocked())) {
        singleLocker.DrawableOnDrawMultiAccessEventReport(__func__);
        RS_LOGE("RSSurfaceRenderNodeDrawable::OnDraw node %{public}" PRIu64 " onDraw!!!", GetId());
        if (RSSystemProperties::GetSingleDrawableLockerEnabled()) {
            SetDrawSkipType(DrawSkipType::MULTI_ACCESS);
            return;
        }
    }

    subThreadCache_.TotalProcessedSurfaceCountInc(*rscanvas);
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
        RotateOffScreenParam::GetRotateOffScreenSurfaceNodeEnable() &&
        surfaceParams->GetNeedOffscreen() && !rscanvas->GetTotalMatrix().IsIdentity() &&
        surfaceParams->IsAppWindow() && GetName().substr(0, 3) != "SCB" && !IsHardwareEnabled() &&
        (surfaceParams->GetVisibleRegion().Area() == (surfaceParams->GetOpaqueRegion().Area() +
        surfaceParams->GetRoundedCornerRegion().Area()));
    curCanvas_ = rscanvas;
    if (needOffscreen) {
        isInRotationFixed_ = false;
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
    PreprocessUnobscuredUEC(*curCanvas_);
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
        if (!(surfaceParams->GetNeedOffscreen() && RotateOffScreenParam::GetRotateOffScreenSurfaceNodeEnable())) {
            subThreadCache_.PushDirtyRegionToStack(*curCanvas_, curSurfaceDrawRegion);
        }
    }

    // disable filter cache when surface global position is enabled
    bool isDisableFilterCache = curCanvas_->GetDisableFilterCache();
    curCanvas_->SetDisableFilterCache(isDisableFilterCache || surfaceParams->GetGlobalPositionEnabled());
    auto parentSurfaceMatrix = RSRenderParams::GetParentSurfaceMatrix();
    RSRenderParams::SetParentSurfaceMatrix(curCanvas_->GetTotalMatrix());

    // add a blending disable rect op behind floating window, to enable overdraw buffer feature on special gpu.
    if (surfaceParams->IsLeashWindow() && RSSystemProperties::GetGpuOverDrawBufferOptimizeEnabled()
        && surfaceParams->IsGpuOverDrawBufferOptimizeNode()) {
        EnableGpuOverDrawDrawBufferOptimization(*curCanvas_, surfaceParams);
    }
    OnGeneralProcess(*curCanvas_, *surfaceParams, *uniParam, isSelfDrawingSurface);
    if (surfaceParams->GetRSFreezeFlag() && GetCacheImageByCapture() && !isUiFirstNode) {
        RS_TRACE_NAME("Drawing cachedImage by capture");
        DrawCachedImage(*curCanvas_, surfaceParams->GetCacheSize());
    } else {
        if (GetCacheImageByCapture()) {
            SetCacheImageByCapture(nullptr);
        }
        RS_LOGI_LIMIT(
            "RSSurfaceRenderNodeDrawable::OnDraw name:%{public}s, the number of total processedNodes: %{public}d",
            name_.c_str(), RSRenderNodeDrawable::GetTotalProcessedNodeCount());
    }

    if (needOffscreen && canvasBackup_) {
        Drawing::AutoCanvasRestore acrBackUp(*canvasBackup_, true);
        if (isInRotationFixed_) {
            canvasBackup_->Clear(Drawing::Color::COLOR_BLACK);
        }
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
        if (!(surfaceParams->GetNeedOffscreen() && RotateOffScreenParam::GetRotateOffScreenSurfaceNodeEnable())) {
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

    curCanvas_->SetDisableFilterCache(isDisableFilterCache);
    RSRenderParams::SetParentSurfaceMatrix(parentSurfaceMatrix);
    SetUIExtensionNeedToDraw(false);
}

void RSSurfaceRenderNodeDrawable::CrossDisplaySurfaceDirtyRegionConversion(
    const RSRenderThreadParams& uniParam, const RSSurfaceRenderParams& surfaceParam, RectI& surfaceDirtyRect) const
{
    if (!surfaceParam.IsFirstLevelCrossNode()) {
        return;
    }
    auto displayConversionMatrices = surfaceParam.GetCrossNodeSkipDisplayConversionMatrix();
    auto curConversionMatrix = displayConversionMatrices.find(uniParam.GetCurrentVisitDisplayDrawableId());
    if (curConversionMatrix != displayConversionMatrices.end()) {
        // transfer from the display coordinate system during quickprepare into current display coordinate system.
        std::shared_ptr<RSObjAbsGeometry> geoPtr = std::make_shared<RSObjAbsGeometry>();
        surfaceDirtyRect = geoPtr->MapRect(surfaceDirtyRect.ConvertTo<float>(), curConversionMatrix->second);
    }
}

void RSSurfaceRenderNodeDrawable::OnCapture(Drawing::Canvas& canvas)
{
    RECORD_GPURESOURCE_CORETRACE_CALLER(Drawing::CoreFunction::
        RS_RSSURFACERENDERNODEDRAWABLE_ONCAPTURE);
    if (!ShouldPaint()) {
        return;
    }
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(GetRenderParams().get());
    if (!surfaceParams) {
        RS_LOGE("RSSurfaceRenderNodeDrawable::OnCapture surfaceParams is nullptr");
        return;
    }

    auto cloneSourceDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
        surfaceParams->GetCloneSourceDrawable().lock());
    auto cloneSourceParams = cloneSourceDrawable ? cloneSourceDrawable->GetRenderParams().get() : nullptr;
    if (cloneSourceParams) {
        cloneSourceDrawable->OnCapture(canvas);
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

#ifdef RS_ENABLE_GPU
    RSTagTracker tagTracker(rscanvas->GetGPUContext().get(), RSTagTracker::SOURCETYPE::SOURCE_ONCAPTURE);
#endif

    if (DrawCloneNode(*rscanvas, *uniParam, *surfaceParams, true)) {
        return;
    }

    rscanvas->SetHighContrast(RSUniRenderThread::Instance().IsHighContrastTextModeOn());
    // process white list
    auto whiteList = RSUniRenderThread::Instance().GetWhiteList();
    SetVirtualScreenWhiteListRootId(whiteList, surfaceParams->GetLeashPersistentId());

    if (CheckIfSurfaceSkipInMirror(*surfaceParams)) {
        SetDrawSkipType(DrawSkipType::SURFACE_SKIP_IN_MIRROR);
        RS_TRACE_NAME_FMT("RSSurfaceRenderNodeDrawable::OnCapture surface skipped in mirror name:[%s] id:%" PRIu64,
            name_.c_str(), surfaceParams->GetId());
        return;
    }

    if (surfaceParams->GetHardCursorStatus() &&
        (uniParam->HasPhysicMirror() || RSUniRenderThread::GetCaptureParam().isSnapshot_)) {
        SetDrawSkipType(DrawSkipType::HARD_CURSOR_ENAbLED);
        RS_TRACE_NAME_FMT("RSSurfaceRenderNodeDrawable::OnCapture hardcursor skip SurfaceName:%s", name_.c_str());
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
    if (DrawCacheImageForMultiScreenView(*rscanvas, *surfaceParams)) {
        return;
    }
    RSAutoCanvasRestore acr(rscanvas, RSPaintFilterCanvas::SaveType::kCanvasAndAlpha);

    // First node don't need to concat matrix for application
    if (RSUniRenderThread::GetCaptureParam().isFirstNode_) {
        // Planning: If node is a sandbox.
        rscanvas->MultiplyAlpha(surfaceParams->GetAlpha());
        RSUniRenderThread::GetCaptureParam().isFirstNode_ = false;
    } else {
        PreprocessUnobscuredUEC(*rscanvas);
        surfaceParams->ApplyAlphaAndMatrixToCanvas(*rscanvas);
    }

    CaptureSurface(*rscanvas, *surfaceParams);
    ResetVirtualScreenWhiteListRootId(surfaceParams->GetLeashPersistentId());
}

bool RSSurfaceRenderNodeDrawable::CheckIfSurfaceSkipInMirror(const RSSurfaceRenderParams& surfaceParams)
{
    const auto& uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    if (uniParam && !uniParam->IsMirrorScreen()) {
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
    // Check type black list.
    const auto& typeBlackList = RSUniRenderThread::Instance().GetTypeBlackList();
    NodeType nodeType = static_cast<NodeType>(surfaceParams.GetSurfaceNodeType());
    if (typeBlackList.find(nodeType) != typeBlackList.end()) {
        RS_LOGD("RSSurfaceRenderNodeDrawable::CheckIfSurfaceSkipInMirror: "
            "(surfaceNodeType:[%{public}u]) is in type black list", nodeType);
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

bool RSSurfaceRenderNodeDrawable::IsVisibleRegionEqualOnPhysicalAndVirtual(RSSurfaceRenderParams& surfaceParams)
{
    // leash window has no visible region, we should check its children.
    if (surfaceParams.IsLeashWindow()) {
        for (const auto& nestedDrawable : GetDrawableVectorById(surfaceParams.GetAllSubSurfaceNodeIds())) {
            auto surfaceDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(nestedDrawable);
            if (!surfaceDrawable) {
                continue;
            }
            auto renderParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable->GetRenderParams().get());
            if (!renderParams) {
                continue;
            }
            if (!IsVisibleRegionEqualOnPhysicalAndVirtual(*renderParams)) {
                RS_LOGD("%{public}s visible region not equal, id:%{public}" PRIu64,
                    renderParams->GetName().c_str(), renderParams->GetId());
                RS_TRACE_NAME_FMT("%s visible region not equal, id:%" PRIu64,
                    renderParams->GetName().c_str(), renderParams->GetId());
                return false;
            }
        }
        return true;
    }

    auto visibleRegion = surfaceParams.GetVisibleRegion();
    auto virtualVisibleRegion = surfaceParams.GetVisibleRegionInVirtual();
    // use XOR to check if two regions are equal.
    return visibleRegion.Xor(virtualVisibleRegion).IsEmpty();
}

void RSSurfaceRenderNodeDrawable::CaptureSurface(RSPaintFilterCanvas& canvas, RSSurfaceRenderParams& surfaceParams)
{
    RECORD_GPURESOURCE_CORETRACE_CALLER(Drawing::CoreFunction::
        RS_RSSURFACERENDERNODEDRAWABLE_CAPTURESURFACE);
    auto& uniParams = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    if (UNLIKELY(!uniParams)) {
        RS_LOGE("RSSurfaceRenderNodeDrawable::CaptureSurface uniParams is nullptr");
        return;
    }
    const auto& specialLayerManager = surfaceParams.GetSpecialLayerMgr();
    bool isSecLayersNotExempted = specialLayerManager.Find(SpecialLayerType::SECURITY) && !uniParams->GetSecExemption();
    bool needSkipDrawWhite =
        RSUniRenderThread::GetCaptureParam().isNeedBlur_ || RSUniRenderThread::GetCaptureParam().isSelfCapture_;
    // Draw White
    if (RSUniRenderThread::GetCaptureParam().isSingleSurface_ &&
        (UNLIKELY(isSecLayersNotExempted && !needSkipDrawWhite) || specialLayerManager.Find(SpecialLayerType::SKIP))) {
        RS_LOGD("RSSurfaceRenderNodeDrawable::CaptureSurface: "
                "process RSSurfaceRenderNode(id:[%{public}" PRIu64 "] name:[%{public}s])"
                "draw white with security or skip layer for SingleSurface, isNeedBlur:[%{public}s], "
                "isSelfCapture:[%{public}s]",
            surfaceParams.GetId(), name_.c_str(), RSUniRenderThread::GetCaptureParam().isNeedBlur_ ? "true" : "false",
            RSUniRenderThread::GetCaptureParam().isSelfCapture_ ? "true" : "false");
        RS_TRACE_NAME_FMT(
            "CaptureSurface: RSSurfaceRenderNode(id:[%" PRIu64 "] name:[%s])"
            "draw white with security or skip layer for SingleSurface, isNeedBlur: [%s], isSelfCapture:[%s]",
            surfaceParams.GetId(), name_.c_str(), RSUniRenderThread::GetCaptureParam().isNeedBlur_ ? "true" : "false",
            RSUniRenderThread::GetCaptureParam().isSelfCapture_ ? "true" : "false");

        Drawing::Brush rectBrush;
        rectBrush.SetColor(Drawing::Color::COLOR_WHITE);
        canvas.AttachBrush(rectBrush);
        canvas.DrawRect(Drawing::Rect(
            0, 0, surfaceParams.GetBounds().GetWidth(), surfaceParams.GetBounds().GetHeight()));
        canvas.DetachBrush();
        return;
    }

    // Draw Black
    bool isScreenshot = RSUniRenderThread::GetCaptureParam().isSnapshot_ &&
        !RSUniRenderThread::GetCaptureParam().isSingleSurface_;
    if (specialLayerManager.Find(SpecialLayerType::PROTECTED) || UNLIKELY(isSecLayersNotExempted && isScreenshot)) {
        RS_LOGD("RSSurfaceRenderNodeDrawable::CaptureSurface: "
            "process RSSurfaceRenderNode(id:[%{public}" PRIu64 "] name:[%{public}s])"
            "draw black with protected layer or screenshot security layer", surfaceParams.GetId(), name_.c_str());
        RS_TRACE_NAME_FMT("CaptureSurface: RSSurfaceRenderNode(id:[%" PRIu64 "] name:[%s])"
            "draw black with protected layer or screenshot security layer", surfaceParams.GetId(), name_.c_str());

        Drawing::Brush rectBrush;
        rectBrush.SetColor(Drawing::Color::COLOR_BLACK);
        canvas.AttachBrush(rectBrush);
        canvas.DrawRect(Drawing::Rect(0, 0, surfaceParams.GetBounds().GetWidth(),
            surfaceParams.GetBounds().GetHeight()));
        canvas.DetachBrush();
        return;
    }

    // Skip Drawing
    auto isSnapshotSkipLayer =
        RSUniRenderThread::GetCaptureParam().isSnapshot_ && specialLayerManager.Find(SpecialLayerType::SNAPSHOT_SKIP);
    if ((!RSUniRenderThread::GetCaptureParam().isSingleSurface_ && specialLayerManager.Find(SpecialLayerType::SKIP)) ||
        isSnapshotSkipLayer) {
        RS_LOGD("RSSurfaceRenderNodeDrawable::CaptureSurface: "
            "process RSSurfaceRenderNode(id:[%{public}" PRIu64 "] name:[%{public}s])"
            "skip layer or snapshotskip layer", surfaceParams.GetId(), name_.c_str());
        RS_TRACE_NAME_FMT("CaptureSurface: RSSurfaceRenderNode(id:[%" PRIu64 "] name:[%s])"
            "skip layer or snapshotskip layer", surfaceParams.GetId(), name_.c_str());
        return;
    }

    RS_LOGD("HDRService hasHdrPresent_: %{public}d, GetId: %{public}" PRIu64 "",
        surfaceParams.GetHDRPresent(), surfaceParams.GetId());
    bool hasHidePrivacyContent = surfaceParams.HasPrivacyContentLayer() &&
        RSUniRenderThread::GetCaptureParam().isSingleSurface_ &&
        !RSUniRenderThread::GetCaptureParam().isSystemCalling_;
    bool enableVisiableRect = RSUniRenderThread::Instance().GetEnableVisiableRect();
    if (!(specialLayerManager.Find(HAS_GENERAL_SPECIAL) || surfaceParams.GetHDRPresent() || hasHidePrivacyContent ||
        enableVisiableRect || !IsVisibleRegionEqualOnPhysicalAndVirtual(surfaceParams))) {
        if (subThreadCache_.DealWithUIFirstCache(this, canvas, surfaceParams, *uniParams)) {
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
        if (RSSystemProperties::GetSingleDrawableLockerEnabled()) {
            return;
        }
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

    OnGeneralProcess(canvas, surfaceParams, *uniParams, isSelfDrawingSurface);

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
    RECORD_GPURESOURCE_CORETRACE_CALLER(Drawing::CoreFunction::
        RS_RSSURFACERENDERNODEDRAWABLE_DEALWITHSELFDRAWINGNODEBUFFER);
    if ((surfaceParams.GetHardwareEnabled() || surfaceParams.GetHardCursorStatus()) &&
        RSUniRenderThread::IsExpandScreenMode()) {
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

            // Use to adapt to AIBar DSS solution
            Color solidLayerColor = RgbPalette::Transparent();
            if (surfaceParams.GetIsHwcEnabledBySolidLayer()) {
                solidLayerColor = surfaceParams.GetSolidLayerColor();
                RS_TRACE_NAME_FMT("solidLayer enabled, color:%08x", solidLayerColor.AsArgbInt());
            }
            SurfaceNodeInfo surfaceNodeInfo = {
                surfaceNodeImage, rotateMatrix, params.srcRect, params.dstRect, solidLayerColor};

            HveFilter::GetHveFilter().PushSurfaceNodeInfo(surfaceNodeInfo);
        }
        return;
    }
    if (surfaceParams.GetSpecialLayerMgr().Find(SpecialLayerType::PROTECTED)) {
        RS_LOGD("protected layer cannot draw in non-protected context.");
        return;
    }
    if (surfaceParams.IsInFixedRotation()) {
        isInRotationFixed_ = true;
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
    // color temperature
    params.layerLinearMatrix = surfaceParams.GetLayerLinearMatrix();
    params.hasMetadata = surfaceParams.GetSdrHasMetadata();
#endif
    params.colorFollow = surfaceParams.GetColorFollow(); // force the buffer to follow the colorspace of canvas
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
    if (IsHardwareEnabledTopSurface() && RSUniRenderThread::Instance().GetRSRenderThreadParams()->HasMirrorDisplay()) {
        RSMagicPointerRenderManager::GetInstance().SetCacheImgForPointer(canvas.GetSurface()->GetImageSnapshot());
    }
#endif

    DrawSelfDrawingNodeBuffer(canvas, surfaceParams, params);
}

bool RSSurfaceRenderNodeDrawable::RecordTimestamp(NodeId id, uint32_t seqNum)
{
    uint64_t currentTime = static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count());
    auto& surfaceFpsManager = RSSurfaceFpsManager::GetInstance();
    return surfaceFpsManager.RecordPresentTime(id, currentTime, seqNum);
}

bool RSSurfaceRenderNodeDrawable::DrawCloneNode(RSPaintFilterCanvas& canvas,
                                                RSRenderThreadParams& uniParam,
                                                RSSurfaceRenderParams& surfaceParams, bool isCapture)
{
    if (!surfaceParams.IsCloneNode()) {
        return false;
    }
    auto clonedNodeRenderDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
        surfaceParams.GetClonedNodeRenderDrawable().lock());
    if (clonedNodeRenderDrawable == nullptr) {
        RS_LOGD("RSSurfaceRenderNodeDrawable::DrawCloneNode clonedNodeRenderDrawable is null");
        return false;
    }
    if (!surfaceParams.IsClonedNodeOnTheTree()) {
        RS_LOGI("RSSurfaceRenderNodeDrawable::DrawCloneNode clonedNode of %{public}s isn't on the tree",
            name_.c_str());
        clonedNodeRenderDrawable->subThreadCache_.GetRSDrawWindowCache().ClearCache();
        return false;
    }
    RS_TRACE_NAME_FMT("RSSurfaceRenderNodeDrawable::DrawCloneNode Draw cloneNode %s", name_.c_str());
    bool isOpDropped = uniParam.IsOpDropped();
    uniParam.SetOpDropped(false);
    RSAutoCanvasRestore acr(&canvas, RSPaintFilterCanvas::SaveType::kCanvasAndAlpha);
    canvas.MultiplyAlpha(surfaceParams.GetAlpha());
    isCapture ? clonedNodeRenderDrawable->OnCapture(canvas) : clonedNodeRenderDrawable->OnDraw(canvas);
    uniParam.SetOpDropped(isOpDropped);
    clonedNodeRenderDrawable->subThreadCache_.GetRSDrawWindowCache().ClearCache();
    return true;
}

void RSSurfaceRenderNodeDrawable::ClipHoleForSelfDrawingNode(RSPaintFilterCanvas& canvas,
    RSSurfaceRenderParams& surfaceParams)
{
    if (surfaceParams.GetForceDisableClipHoleForDRM()) {
        RS_LOGD("DMA buffer avoid clippingHole during Attraction effect");
        RS_OPTIONAL_TRACE_NAME_FMT("DMA buffer avoid clippingHole during Attraction effect [%s] ", name_.c_str());
        return;
    }
    RSAutoCanvasRestore arc(&canvas);
    auto bounds = surfaceParams.GetBounds();
    canvas.ClipRect({std::round(bounds.GetLeft()), std::round(bounds.GetTop()),
        std::round(bounds.GetRight()), std::round(bounds.GetBottom())});
    canvas.Clear(Drawing::Color::COLOR_TRANSPARENT);
    if (RSSystemProperties::GetDebugTraceEnabled()) {
        Drawing::RectF absRect;
        canvas.GetTotalMatrix().MapRect(absRect, bounds);
        RS_TRACE_NAME_FMT("hwc debug: clipHole: [%f,%f,%f,%f], absRect: [%s]", bounds.GetLeft(), bounds.GetTop(),
            bounds.GetRight(), bounds.GetBottom(), absRect.ToString().c_str());
    }
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
    RECORD_GPURESOURCE_CORETRACE_CALLER(Drawing::CoreFunction::
        RS_RSSURFACERENDERNODEDRAWABLE_DRAWSELFDRAWINGNODEBUFFER);
#ifdef RS_ENABLE_GPU
    RSTagTracker tagTracker(canvas.GetGPUContext().get(), RSTagTracker::SOURCETYPE::SOURCE_DRAWSELFDRAWINGNODEBUFFER);
#endif
    if (params.buffer == nullptr) {
        RS_LOGE("RSSurfaceRenderNodeDrawable::DrawSelfDrawingNodeBuffer params.buffer is nullptr");
    } else {
        RecordTimestamp(surfaceParams.GetId(), params.buffer->GetSeqNum());
    }
    auto bgColor = surfaceParams.GetBackgroundColor();
    if (surfaceParams.GetHardwareEnabled() && surfaceParams.GetIsHwcEnabledBySolidLayer()) {
        bgColor = surfaceParams.GetSolidLayerColor();
        RS_LOGD("solidLayer enabled, %{public}s, brush set color: %{public}08x", __func__, bgColor.AsArgbInt());
    }
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

Drawing::Matrix RSSurfaceRenderNodeDrawable::GetGravityMatrix(float imgWidth, float imgHeight)
{
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(GetRenderParams().get());
    if (!surfaceParams) {
        RS_LOGE("RSSurfaceRenderNodeDrawable::GetGravityTranslate surfaceParams is nullptr");
        return Drawing::Matrix();
    }
    auto gravity = surfaceParams->GetUIFirstFrameGravity();
    float boundsWidth = surfaceParams->GetCacheSize().x_;
    float boundsHeight = surfaceParams->GetCacheSize().y_;
    Drawing::Matrix gravityMatrix;
    RSPropertiesPainter::GetGravityMatrix(gravity, RectF {0.0f, 0.0f, boundsWidth, boundsHeight},
        imgWidth, imgHeight, gravityMatrix);
    return gravityMatrix;
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
