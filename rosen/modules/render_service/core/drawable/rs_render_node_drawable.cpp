/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

#include "drawable/rs_render_node_drawable.h"

#include "common/rs_common_def.h"
#include "common/rs_optional_trace.h"
#include "display_engine/rs_luminance_control.h"
#include "feature/uifirst/rs_uifirst_manager.h"
#include "gfx/performance/rs_perfmonitor_reporter.h"
#include "include/gpu/vk/GrVulkanTrackerInterface.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "pipeline/render_thread/rs_uni_render_util.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_task_dispatcher.h"
#include "platform/common/rs_log.h"
#include "render/rs_effect_luminance_manager.h"
#include "rs_trace.h"
#include "system/rs_system_parameters.h"
#include "string_utils.h"
#include "pipeline/main_thread/rs_main_thread.h"
#ifdef SUBTREE_PARALLEL_ENABLE
#include "rs_parallel_manager.h"
#include "rs_parallel_misc.h"
#endif

namespace OHOS::Rosen::DrawableV2 {
#ifdef RS_ENABLE_VK
#ifdef USE_M133_SKIA
#include "include/gpu/ganesh/vk/GrVkBackendSurface.h"
#else
#include "include/gpu/GrBackendSurface.h"
#endif

#include "platform/ohos/backend/native_buffer_utils.h"
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif
RSRenderNodeDrawable::Registrar RSRenderNodeDrawable::instance_;
thread_local bool RSRenderNodeDrawable::drawBlurForCache_ = false;
thread_local bool RSRenderNodeDrawable::isOpDropped_ = true;
thread_local bool RSRenderNodeDrawable::occlusionCullingEnabled_ = false;
thread_local bool RSRenderNodeDrawable::isOffScreenWithClipHole_ = false;

namespace {
constexpr int32_t DRAWING_CACHE_MAX_UPDATE_TIME = 3;
constexpr float CACHE_FILL_ALPHA = 0.2f;
constexpr float CACHE_UPDATE_FILL_ALPHA = 0.8f;
constexpr int TRACE_LEVEL_PRINT_NODEID = 6;
}
RSRenderNodeDrawable::RSRenderNodeDrawable(std::shared_ptr<const RSRenderNode>&& node)
    : RSRenderNodeDrawableAdapter(std::move(node))
{
    auto task = [this] { this->RSRenderNodeDrawable::ClearCachedSurface(); };
    RegisterClearSurfaceFunc(task);
}

RSRenderNodeDrawable::~RSRenderNodeDrawable()
{
    ClearDrawingCacheDataMap();
    ClearCachedSurface();
    ResetClearSurfaceFunc();
}

RSRenderNodeDrawable::Ptr RSRenderNodeDrawable::OnGenerate(std::shared_ptr<const RSRenderNode> node)
{
    return new RSRenderNodeDrawable(std::move(node));
}

void RSRenderNodeDrawable::Draw(Drawing::Canvas& canvas)
{
    if (UNLIKELY(RSUniRenderThread::IsInCaptureProcess())) {
        OnCapture(canvas);
    } else {
        OnDraw(canvas);
    }
}

/*
 * This function will be called recursively many times, and the logic should be as concise as possible.
 */
void RSRenderNodeDrawable::OnDraw(Drawing::Canvas& canvas)
{
    auto& captureParam = RSUniRenderThread::GetCaptureParam();
    if (canvas.GetUICapture() && captureParam.captureFinished_) {
        return;
    }
    Drawing::GPUResourceTag::SetCurrentNodeId(GetId());
    RSRenderNodeDrawable::TotalProcessedNodeCountInc();
    Drawing::Rect bounds = GetRenderParams() ? GetRenderParams()->GetFrameRect() : Drawing::Rect(0, 0, 0, 0);
    // Skip nodes that were culled by the control-level occlusion.
    if (SkipCulledNodeOrEntireSubtree(canvas, bounds)) {
        return;
    }

    UpdateFilterDisplayHeadroom(canvas);

#ifdef SUBTREE_PARALLEL_ENABLE
    if (RSParallelManager::Singleton().OnDrawNodeDrawable(canvas, bounds, this)) {
        return;
    }
#endif
    DrawBackground(canvas, bounds);

    CollectInfoForUnobscuredUEC(canvas);

    DrawContent(canvas, bounds);

    bool stopDrawForRangeCapture = (canvas.GetUICapture() &&
        captureParam.endNodeId_ == GetId() &&
        captureParam.endNodeId_ != INVALID_NODEID);
    if (stopDrawForRangeCapture) {
        captureParam.captureFinished_ = true;
    }
    if (!captureParam.isSoloNodeUiCapture_ && !stopDrawForRangeCapture) {
        DrawChildren(canvas, bounds);
    }

    DrawForeground(canvas, bounds);
}

bool RSRenderNodeDrawable::SkipCulledNodeOrEntireSubtree(Drawing::Canvas& canvas, Drawing::Rect& bounds)
{
    auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(&canvas);
    auto id = GetId();
    if (LIKELY(id != INVALID_NODEID) && IsOcclusionCullingEnabled()) {
        if (paintFilterCanvas->GetCulledEntireSubtree().count(id) > 0) {
            RS_OPTIONAL_TRACE_NAME_FMT("%s, id: %" PRIu64 " culled entire subtree success", __func__, id);
            SetDrawSkipType(DrawSkipType::OCCLUSION_SKIP);
            return true;
        }
    }
    return false;
}

/*
 * This function will be called recursively many times, and the logic should be as concise as possible.
 */
void RSRenderNodeDrawable::OnCapture(Drawing::Canvas& canvas)
{
    RSRenderNodeDrawable::OnDraw(canvas);
}

CM_INLINE void RSRenderNodeDrawable::GenerateCacheIfNeed(
    Drawing::Canvas& canvas, RSRenderParams& params)
{
    // check if drawing cache enabled
    if (params.GetDrawingCacheType() != RSDrawingCacheType::DISABLED_CACHE) {
        RS_OPTIONAL_TRACE_NAME_FMT("RSCanvasRenderNodeDrawable::OnDraw id:%llu cacheType:%d cacheChanged:%d"
                                   " size:[%.2f, %.2f] ChildHasVisibleFilter:%d ChildHasVisibleEffect:%d"
                                   " shadowRect:[%.2f, %.2f, %.2f, %.2f] HasFilterOrEffect:%d",
            params.GetId(), params.GetDrawingCacheType(), params.GetDrawingCacheChanged(), params.GetCacheSize().x_,
            params.GetCacheSize().y_, params.ChildHasVisibleFilter(), params.ChildHasVisibleEffect(),
            params.GetShadowRect().GetLeft(), params.GetShadowRect().GetTop(), params.GetShadowRect().GetWidth(),
            params.GetShadowRect().GetHeight(), HasFilterOrEffect());
    }

    RS_LOGI_IF(DEBUG_NODE, "RSRenderNodeDrawable::GenerateCacheCondition drawingCacheType:%{public}d"
        " RSFreezeFlag:%{public}d OpincGetCachedMark:%{public}d", params.GetDrawingCacheType(),
        params.GetRSFreezeFlag(), GetOpincDrawCache().OpincGetCachedMark());
    if (params.GetRSFreezeFlag()) {
        RS_OPTIONAL_TRACE_NAME_FMT("RSCanvasRenderNodeDrawable::GenerateCacheIfNeed id:%llu"
                                   " GetRSFreezeFlag:%d hasFilter:%d",
            params.GetId(), params.GetRSFreezeFlag(), params.ChildHasVisibleFilter());
    }

    // check drawing cache type (disabled: clear cache)
    if ((params.GetDrawingCacheType() == RSDrawingCacheType::DISABLED_CACHE &&
        !GetOpincDrawCache().OpincGetCachedMark()) && !params.GetRSFreezeFlag()) {
        ClearCachedSurface();
        ClearDrawingCacheDataMap();
        ClearDrawingCacheContiUpdateTimeMap();
        return;
    }

    {
        std::scoped_lock<std::recursive_mutex> cacheLock(cacheMutex_);
        if (cachedSurface_ == nullptr) {
            // Remove node id in update time map to avoid update time exceeds DRAWING_CACHE_MAX_UPDATE_TIME
            // (If cache disabled for node not on the tree, we clear cache in OnSync func, but we can't clear node
            // id in drawingCacheUpdateTimeMap_ [drawable will not be visited in RT].
            // If this node is marked node group by arkui again, we should first clear update time here, otherwise
            // update time will accumulate.)
            ClearDrawingCacheDataMap();
            ClearDrawingCacheContiUpdateTimeMap();
        }
    }
    // generate(first time)/update cache(cache changed) [TARGET -> DISABLED if >= MAX UPDATE TIME]
    int32_t updateTimes = 0;
    bool needUpdateCache = CheckIfNeedUpdateCache(params, updateTimes);
    params.SetNeedUpdateCache(needUpdateCache);
    int32_t continuousUpdateTimes = 0;
    {
        std::lock_guard<std::mutex> lock(drawingCacheContiUpdateTimeMapMutex_);
        if (drawingCacheContinuousUpdateTimeMap_.count(nodeId_) > 0) {
            continuousUpdateTimes = drawingCacheContinuousUpdateTimeMap_.at(nodeId_);
        }
    }
    if (needUpdateCache && params.GetDrawingCacheType() == RSDrawingCacheType::TARGETED_CACHE &&
        continuousUpdateTimes > DRAWING_CACHE_MAX_UPDATE_TIME) {
        RS_LOGD("RSRenderNodeDrawable::GenerateCacheCondition totalUpdateTimes:%{public}d "
                "continuousUpdateTimes:%{public}d needUpdateCache:%{public}d",
                updateTimes, continuousUpdateTimes, needUpdateCache);
        RS_TRACE_NAME_FMT("DisableCache by continuous update time > 3, id:%" PRIu64 "", params.GetId());
        params.SetDrawingCacheType(RSDrawingCacheType::DISABLED_CACHE);
        ClearCachedSurface();
    }
    // reset drawing cache changed false for render param if drawable is visited this frame
    // if this drawble is skipped due to occlusion skip of app surface node, this flag should be kept for next frame
    params.SetDrawingCacheChanged(false, true);
    bool hasFilter = params.ChildHasVisibleFilter() || params.ChildHasVisibleEffect();
    if ((params.GetDrawingCacheType() == RSDrawingCacheType::DISABLED_CACHE || (!needUpdateCache && !hasFilter))
        && !GetOpincDrawCache().OpincGetCachedMark() && !params.GetRSFreezeFlag()) {
        return;
    }

    if (needUpdateCache) {
        filterInfoVec_.clear();
    }
    bool isForegroundFilterCache = params.GetForegroundFilterCache() != nullptr;
    // in case of no filter
    if (needUpdateCache && (!hasFilter || isForegroundFilterCache || params.GetRSFreezeFlag())) {
        RS_TRACE_NAME_FMT("UpdateCacheSurface id:%" PRIu64 ", isForegroundFilter:%d", nodeId_, isForegroundFilterCache);
        RSRenderNodeDrawableAdapter* root = curDrawingCacheRoot_;
        curDrawingCacheRoot_ = this;
        hasSkipCacheLayer_ = false;
        hasChildInBlackList_ = false;
        UpdateCacheSurface(canvas, params);
        curDrawingCacheRoot_ = root;
        return;
    }

    // in case of with filter
    auto curCanvas = static_cast<RSPaintFilterCanvas*>(&canvas);
    if (needUpdateCache) {
        // 1. update cache without filer/shadow/effect & clip hole
        auto canvasType = curCanvas->GetCacheType();
        // set canvas type as OFFSCREEN to not draw filter/shadow/filter
        curCanvas->SetCacheType(RSPaintFilterCanvas::CacheType::OFFSCREEN);
        bool isOffScreenWithClipHole = isOffScreenWithClipHole_;
        isOffScreenWithClipHole_ = true;
        RS_TRACE_NAME_FMT("UpdateCacheSurface with filter id:%" PRIu64 "", nodeId_);
        RSRenderNodeDrawableAdapter* root = curDrawingCacheRoot_;
        curDrawingCacheRoot_ = this;
        hasSkipCacheLayer_ = false;
        hasChildInBlackList_ = false;
        UpdateCacheSurface(canvas, params);
        // if this NodeGroup contains other nodeGroup with filter, we should reset the isOffScreenWithClipHole_
        isOffScreenWithClipHole_ = isOffScreenWithClipHole;
        curCanvas->SetCacheType(canvasType);
        curDrawingCacheRoot_ = root;
    }
}

void RSRenderNodeDrawable::TraverseSubTreeAndDrawFilterWithClip(Drawing::Canvas& canvas, const RSRenderParams& params)
{
    if (filterInfoVec_.empty()) {
        return;
    }
    RSRenderNodeDrawableAdapter* root = curDrawingCacheRoot_;
    curDrawingCacheRoot_ = this;
    filterNodeSize_ = filterInfoVec_.size();
    Drawing::AutoCanvasRestore arc(canvas, true);
    bool isOpDropped = isOpDropped_;
    isOpDropped_ = false;
    drawBlurForCache_ = true; // may use in uifirst subthread
    auto drawableCacheType = GetCacheType();
    SetCacheType(DrawableCacheType::NONE);
    RS_TRACE_NAME_FMT("DrawBlurForCache id:%" PRIu64 "", nodeId_);

    DrawBackground(canvas, params.GetBounds());
    Drawing::Region filterRegion;
    for (auto& item : filterInfoVec_) {
        for (auto& rect: item.rectVec_) {
            Drawing::Region region;
            region.SetRect(rect);
            filterRegion.Op(region, Drawing::RegionOp::UNION);
        }
    }
    Drawing::Path filetrPath;
    filterRegion.GetBoundaryPath(&filetrPath);
    canvas.ClipPath(filetrPath);
    DrawContent(canvas, params.GetFrameRect());
    DrawChildren(canvas, params.GetBounds());
    curDrawingCacheRoot_->SetLastDrawnFilterNodeId(0);

    SetCacheType(drawableCacheType);
    isOpDropped_ = isOpDropped;
    drawBlurForCache_ = false;
    curDrawingCacheRoot_ = root;
}

bool RSRenderNodeDrawable::DealWithWhiteListNodes(Drawing::Canvas& canvas)
{
    auto captureParam = RSUniRenderThread::GetCaptureParam();
    const auto& whiteList = RSUniRenderThread::Instance().GetWhiteList();
    if (!captureParam.isMirror_ || whiteList.empty() || captureParam.rootIdInWhiteList_ != INVALID_NODEID) {
        return false;
    }
    
    const auto& params = GetRenderParams();
    if (!params) {
        SetDrawSkipType(DrawSkipType::RENDER_PARAMS_NULL);
        RS_LOGE("RSSurfaceRenderNodeDrawable::OnCapture params is nullptr");
        return true;
    }
    auto info = params->GetVirtualScreenWhiteListInfo();
    if (info.find(captureParam.virtualScreenId_) != info.end()) {
        DrawChildren(canvas, params->GetFrameRect());
    }
    return true;
}

CM_INLINE void RSRenderNodeDrawable::CheckCacheTypeAndDraw(
    Drawing::Canvas& canvas, const RSRenderParams& params, bool isInCapture)
{
    RS_OPTIONAL_TRACE_BEGIN_LEVEL(TRACE_LEVEL_PRINT_NODEID, "CheckCacheTypeAndDraw nodeId[%llu]", nodeId_);
    bool hasFilter = params.ChildHasVisibleFilter() || params.ChildHasVisibleEffect();
    RS_LOGI_IF(DEBUG_NODE,
        "RSRenderNodeDrawable::CheckCacheTAD hasFilter:%{public}d drawingCacheType:%{public}d",
        hasFilter, params.GetDrawingCacheType());
    auto originalCacheType = GetCacheType();
    // can not draw cache because skipCacheLayer in capture process, such as security layers...
    if (GetCacheType() != DrawableCacheType::NONE && hasSkipCacheLayer_ && isInCapture) {
        SetCacheType(DrawableCacheType::NONE);
    }
    if (hasFilter && params.GetDrawingCacheType() != RSDrawingCacheType::DISABLED_CACHE &&
        params.GetForegroundFilterCache() == nullptr && GetCacheType() != DrawableCacheType::NONE) {
        // traverse children to draw filter/shadow/effect
        TraverseSubTreeAndDrawFilterWithClip(canvas, params);
    }
    // if children don't have any filter or effect, stop traversing
    if (params.GetForegroundFilterCache() == nullptr && drawBlurForCache_ && curDrawingCacheRoot_ &&
        curDrawingCacheRoot_->GetFilterNodeSize() == 0) {
        RS_OPTIONAL_TRACE_NAME_FMT("CheckCacheTypeAndDraw id:%llu child without filter, skip", nodeId_);
        RS_OPTIONAL_TRACE_END_LEVEL(TRACE_LEVEL_PRINT_NODEID);
        return;
    }
    // in case of generating cache with filter in offscreen, clip hole for filter/shadow but drawing others
    if (isOffScreenWithClipHole_) {
        if (HasFilterOrEffect() && params.GetForegroundFilterCache() == nullptr) {
            // clip hole for filter/shadow
            DrawBackgroundWithoutFilterAndEffect(canvas, params);
            DrawContent(canvas, params.GetFrameRect());
            DrawChildren(canvas, params.GetBounds());
            DrawForeground(canvas, params.GetBounds());
            RS_OPTIONAL_TRACE_END_LEVEL(TRACE_LEVEL_PRINT_NODEID);
            return;
        }
        CollectInfoForNodeWithoutFilter(canvas);
    }
    RS_LOGI_IF(DEBUG_NODE, "RSRenderNodeDrawable::CheckCacheTAD GetCacheType is %{public}hu", GetCacheType());
    switch (GetCacheType()) {
        case DrawableCacheType::NONE: {
            DrawWithoutNodeGroupCache(canvas, params, originalCacheType);
            break;
        }
        case DrawableCacheType::CONTENT: {
            DrawWithNodeGroupCache(canvas, params);
            break;
        }
        default:
            break;
    }
    RS_OPTIONAL_TRACE_END_LEVEL(TRACE_LEVEL_PRINT_NODEID);
}

void RSRenderNodeDrawable::DrawWithoutNodeGroupCache(
    Drawing::Canvas& canvas, const RSRenderParams& params, DrawableCacheType originalCacheType)
{
    if (drawBlurForCache_ && curDrawingCacheRoot_) {
        auto& filterInfoVec = curDrawingCacheRoot_->GetfilterInfoVec();
        auto begin = std::find_if(filterInfoVec.begin(), filterInfoVec.end(),
            [nodeId = GetId()](const auto& item) -> bool { return item.nodeId_ == nodeId; });
        if (begin == filterInfoVec.end()) {
            CheckRegionAndDrawWithoutFilter(filterInfoVec, canvas, params);
        } else {
            CheckRegionAndDrawWithFilter(begin, filterInfoVec, canvas, params);
        }
    } else {
        RSRenderNodeDrawable::OnDraw(canvas);
    }
    SetCacheType(originalCacheType);
}

void RSRenderNodeDrawable::DrawWithNodeGroupCache(Drawing::Canvas& canvas, const RSRenderParams& params)
{
#ifdef RS_ENABLE_PREFETCH
            __builtin_prefetch(&cachedImage_, 0, 1);
#endif
    RS_OPTIONAL_TRACE_NAME_FMT("DrawCachedImage id:%llu", nodeId_);
    RS_LOGD("RSRenderNodeDrawable::CheckCacheTAD drawingCacheIncludeProperty is %{public}d",
        params.GetDrawingCacheIncludeProperty());
    if (hasSkipCacheLayer_ && curDrawingCacheRoot_) {
        curDrawingCacheRoot_->SetSkipCacheLayer(true);
    }
    const auto& uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    if (uniParam && uniParam->IsMirrorScreen() && hasChildInBlackList_) {
        RS_OPTIONAL_TRACE_NAME_FMT(
            "RSRenderNodeDrawable::DrawWithNodeGroupCache skip DrawCachedImage on mirror screen if node is in "
            "wireless screen mirroring blacklist");
        RSRenderNodeDrawable::OnDraw(canvas);
        return;
    }

    auto curCanvas = static_cast<RSPaintFilterCanvas*>(&canvas);
    if (!curCanvas) {
        RS_LOGD("RSRenderNodeDrawable::DrawWithNodeGroupCache curCanvas is null");
        return;
    }
    if (LIKELY(!params.GetDrawingCacheIncludeProperty())) {
        DrawBackground(canvas, params.GetBounds());
        DrawCachedImage(*curCanvas, params.GetCacheSize());
        DrawForeground(canvas, params.GetBounds());
    } else if (params.GetForegroundFilterCache() != nullptr) {
        DrawBeforeCacheWithForegroundFilter(canvas, params.GetBounds());
        DrawCachedImage(*curCanvas, params.GetCacheSize(), params.GetForegroundFilterCache());
        DrawAfterCacheWithForegroundFilter(canvas, params.GetBounds());
    } else {
        DrawBeforeCacheWithProperty(canvas, params.GetBounds());
        DrawCachedImage(*curCanvas, params.GetCacheSize());
        DrawAfterCacheWithProperty(canvas, params.GetBounds());
    }
    ClearDrawingCacheContiUpdateTimeMap();
    UpdateCacheInfoForDfx(canvas, params.GetBounds(), params.GetId());
}

void RSRenderNodeDrawable::CheckRegionAndDrawWithoutFilter(
    const std::vector<FilterNodeInfo>& filterInfoVec, Drawing::Canvas& canvas, const RSRenderParams& params)
{
    if (!curDrawingCacheRoot_) {
        return;
    }
    auto& withoutFilterMatrixMap = curDrawingCacheRoot_->GetWithoutFilterMatrixMap();
    if (withoutFilterMatrixMap.find(GetId()) == withoutFilterMatrixMap.end()) {
        RS_LOGE("RSRenderNodeDrawable::CheckRegionAndDrawWithoutFilter can not find matrix of cached node in "
                "withoutFilterMatrixMap, id:%{public}" PRIu64 "", GetId());
        return;
    }
    auto matrix = withoutFilterMatrixMap.at(GetId());
    Drawing::Rect dst;
    matrix.MapRect(dst, params.GetBounds());
    Drawing::RectI dstRect(static_cast<int>(dst.GetLeft()), static_cast<int>(dst.GetTop()),
        static_cast<int>(dst.GetLeft() + dst.GetWidth()), static_cast<int>(dst.GetTop() + dst.GetHeight()));
    auto filterBegin = std::find_if(filterInfoVec.begin(), filterInfoVec.end(),
        [nodeId = curDrawingCacheRoot_->GetLastDrawnFilterNodeId()](
            const auto& item) -> bool { return item.nodeId_ == nodeId; });
    if (filterBegin == filterInfoVec.end()) {
        filterBegin = filterInfoVec.begin();
    } else {
        filterBegin++; // check isIntersect with undrawn filters
    }
    if (IsIntersectedWithFilter(filterBegin, filterInfoVec, dstRect)) {
        RSRenderNodeDrawable::OnDraw(canvas);
    } else if (params.ChildHasVisibleEffect() || params.ChildHasVisibleFilter()) {
        DrawChildren(canvas, params.GetBounds());
    }
}

void RSRenderNodeDrawable::CheckRegionAndDrawWithFilter(std::vector<FilterNodeInfo>::const_iterator& begin,
    const std::vector<FilterNodeInfo>& filterInfoVec, Drawing::Canvas& canvas, const RSRenderParams& params)
{
    if (!curDrawingCacheRoot_ || begin == filterInfoVec.end()) {
        return;
    }
    curDrawingCacheRoot_->SetLastDrawnFilterNodeId(GetId());
    CheckShadowRectAndDrawBackground(canvas, params);
    curDrawingCacheRoot_->ReduceFilterNodeSize();
    Drawing::Rect dst;
    auto matrix = begin->matrix_;
    matrix.MapRect(dst, params.GetBounds());
    Drawing::RectI dstRect(static_cast<int>(dst.GetLeft()), static_cast<int>(dst.GetTop()),
        static_cast<int>(dst.GetLeft() + dst.GetWidth()), static_cast<int>(dst.GetTop() + dst.GetHeight()));
    begin++; // check isIntersect with undrawn filters
    if (IsIntersectedWithFilter(begin, filterInfoVec, dstRect)) {
        DrawContent(canvas, params.GetFrameRect());
        DrawChildren(canvas, params.GetBounds());
        // DrawChildren may reduce filterNodeSize, if still have filter in other subtree of
        // curDrawingCacheRoot_, we should draw foreground here
        if (curDrawingCacheRoot_->GetFilterNodeSize() > 0) {
            auto filterBegin = std::find_if(filterInfoVec.begin(), filterInfoVec.end(),
                [nodeId = curDrawingCacheRoot_->GetLastDrawnFilterNodeId()](
                    const auto& item) -> bool { return item.nodeId_ == nodeId; });
            if (filterBegin != filterInfoVec.end()) {
                filterBegin++; // check isIntersect with undrawn filters
            }
            if (IsIntersectedWithFilter(filterBegin, filterInfoVec, dstRect)) {
                DrawForeground(canvas, params.GetBounds());
            }
        }
    }
}

bool RSRenderNodeDrawable::IsIntersectedWithFilter(std::vector<FilterNodeInfo>::const_iterator& begin,
    const std::vector<FilterNodeInfo>& filterInfoVec, Drawing::RectI& dstRect)
{
    bool isIntersected = false;
    for (auto iter = begin; iter != filterInfoVec.end() && !isIntersected; ++iter) {
        for (auto rect : iter->rectVec_) {
            if (rect.Intersect(dstRect)) {
                isIntersected = true;
                break;
            }
        }
    }
    return isIntersected;
}

void RSRenderNodeDrawable::ClearDrawingCacheDataMap()
{
    {
        std::lock_guard<std::mutex> lock(drawingCacheMapMutex_);
        drawingCacheUpdateTimeMap_.erase(nodeId_);
    }
    // clear Rendergroup dfx data map
    RSPerfMonitorReporter::GetInstance().ClearRendergroupDataMap(nodeId_);
}

void RSRenderNodeDrawable::ClearDrawingCacheContiUpdateTimeMap()
{
    std::lock_guard<std::mutex> lock(drawingCacheContiUpdateTimeMapMutex_);
    drawingCacheContinuousUpdateTimeMap_.erase(nodeId_);
}

void RSRenderNodeDrawable::UpdateCacheInfoForDfx(Drawing::Canvas& canvas, const Drawing::Rect& rect, NodeId id)
{
    if (!isDrawingCacheDfxEnabled_) {
        return;
    }
    Drawing::Rect dst;
    canvas.GetTotalMatrix().MapRect(dst, rect);
    RectI dfxRect(static_cast<int>(dst.GetLeft()), static_cast<int>(dst.GetTop()), static_cast<int>(dst.GetWidth()),
        static_cast<int>(dst.GetHeight()));
    int32_t updateTimes = 0;
    {
        std::lock_guard<std::mutex> lock(drawingCacheMapMutex_);
        if (drawingCacheUpdateTimeMap_.count(nodeId_) > 0) {
            updateTimes = drawingCacheUpdateTimeMap_.at(nodeId_);
        }
    }
    {
        std::lock_guard<std::mutex> lock(drawingCacheInfoMutex_);
        drawingCacheInfos_[id] = std::make_pair(dfxRect, updateTimes);
    }
}

void RSRenderNodeDrawable::InitDfxForCacheInfo()
{
    isDrawingCacheEnabled_ = RSSystemParameters::GetDrawingCacheEnabled();
    auto& uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    if (LIKELY(uniParam)) {
        isDrawingCacheDfxEnabled_ = uniParam->IsDrawingCacheDfxEnabled();
    }
    if (isDrawingCacheDfxEnabled_) {
        std::lock_guard<std::mutex> lock(drawingCacheInfoMutex_);
        drawingCacheInfos_.clear();
        cacheUpdatedNodeMap_.clear();
    }

#ifdef DDGR_ENABLE_FEATURE_OPINC
    autoCacheDrawingEnable_ = RSSystemProperties::GetAutoCacheDebugEnabled() && RSOpincDrawCache::IsAutoCacheEnable();
    autoCacheRenderNodeInfos_.clear();
    ClearOpincState();
#endif
}

void RSRenderNodeDrawable::DrawDfxForCacheInfo(
    RSPaintFilterCanvas& canvas, const std::unique_ptr<RSRenderParams>& params)
{
    if (isDrawingCacheEnabled_ && isDrawingCacheDfxEnabled_) {
        std::lock_guard<std::mutex> lock(drawingCacheInfoMutex_);
        for (const auto& [id, cacheInfo] : drawingCacheInfos_) {
            std::string extraInfo = ", updateTimes:" + std::to_string(cacheInfo.second);
            bool cacheUpdated = cacheUpdatedNodeMap_.count(id) > 0;
            auto color = cacheUpdated ? Drawing::Color::COLOR_RED : Drawing::Color::COLOR_BLUE;
            float alpha = cacheUpdated ? CACHE_UPDATE_FILL_ALPHA : CACHE_FILL_ALPHA;
            RSUniRenderUtil::DrawRectForDfx(canvas, cacheInfo.first, color, alpha, extraInfo);
        }
    }

    if (autoCacheDrawingEnable_ && !isDrawingCacheDfxEnabled_) {
        for (const auto& info : autoCacheRenderNodeInfos_) {
            RSUniRenderUtil::DrawRectForDfx(
                canvas, info.first, Drawing::Color::COLOR_BLUE, 0.2f, info.second); // alpha 0.2 by default
        }
    }
}

void RSRenderNodeDrawable::SetCacheType(DrawableCacheType cacheType)
{
    cacheType_ = cacheType;
}

DrawableCacheType RSRenderNodeDrawable::GetCacheType() const
{
    return cacheType_;
}

std::shared_ptr<Drawing::Surface> RSRenderNodeDrawable::GetCachedSurface(pid_t threadId) const
{
    std::scoped_lock<std::recursive_mutex> lock(cacheMutex_);
    return threadId == cacheThreadId_ ? cachedSurface_ : nullptr;
}

void RSRenderNodeDrawable::InitCachedSurface(Drawing::GPUContext* gpuContext, const Vector2f& cacheSize,
    pid_t threadId, bool isNeedFP16, GraphicColorGamut colorGamut)
{
    std::scoped_lock<std::recursive_mutex> lock(cacheMutex_);
#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)) && (defined RS_ENABLE_EGLIMAGE)
    if (gpuContext == nullptr) {
        return;
    }
    ClearCachedSurface();
    cacheThreadId_ = threadId;
    int32_t width = 0;
    int32_t height = 0;
    if (GetOpincDrawCache().IsComputeDrawAreaSucc()) {
        auto& unionRect = GetOpincDrawCache().GetOpListUnionArea();
        width = static_cast<int32_t>(unionRect.GetWidth());
        height = static_cast<int32_t>(unionRect.GetHeight());
    } else {
        width = static_cast<int32_t>(cacheSize.x_);
        height = static_cast<int32_t>(cacheSize.y_);
    }

#ifdef RS_ENABLE_GL
    if (OHOS::Rosen::RSSystemProperties::GetGpuApiType() != OHOS::Rosen::GpuApiType::VULKAN &&
        OHOS::Rosen::RSSystemProperties::GetGpuApiType() != OHOS::Rosen::GpuApiType::DDGR) {
        Drawing::ImageInfo info = Drawing::ImageInfo::MakeN32Premul(width, height);
        cachedSurface_ = Drawing::Surface::MakeRenderTarget(gpuContext, true, info);
    }
#endif
#ifdef RS_ENABLE_VK
    if (OHOS::Rosen::RSSystemProperties::GetGpuApiType() == OHOS::Rosen::GpuApiType::VULKAN ||
        OHOS::Rosen::RSSystemProperties::GetGpuApiType() == OHOS::Rosen::GpuApiType::DDGR) {
        auto colorType = Drawing::ColorType::COLORTYPE_RGBA_8888;
        VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;
        // When colorType is FP16, buffer's colorspace must be sRGB
        // In other cases, the colorspace follows screen colorspace
        auto colorSpace = RSBaseRenderEngine::ConvertColorGamutToDrawingColorSpace(colorGamut);
        if (isNeedFP16) {
            colorType = Drawing::ColorType::COLORTYPE_RGBA_F16;
            format = VK_FORMAT_R16G16B16A16_SFLOAT;
            colorSpace = Drawing::ColorSpace::CreateSRGB();
        }
        cachedBackendTexture_ = NativeBufferUtils::MakeBackendTexture(width, height, ExtractPid(nodeId_), format);
        auto vkTextureInfo = cachedBackendTexture_.GetTextureInfo().GetVKTextureInfo();
        if (!cachedBackendTexture_.IsValid() || !vkTextureInfo) {
            return;
        }
        vulkanCleanupHelper_ = new NativeBufferUtils::VulkanCleanupHelper(RsVulkanContext::GetSingleton(),
            vkTextureInfo->vkImage, vkTextureInfo->vkAlloc.memory, vkTextureInfo->vkAlloc.statName);
        REAL_ALLOC_CONFIG_SET_STATUS(true);
        cachedSurface_ = Drawing::Surface::MakeFromBackendTexture(gpuContext, cachedBackendTexture_.GetTextureInfo(),
            Drawing::TextureOrigin::BOTTOM_LEFT, 1, colorType, colorSpace,
            NativeBufferUtils::DeleteVkImage, vulkanCleanupHelper_);
        REAL_ALLOC_CONFIG_SET_STATUS(false);
    }
#endif
#else
    cachedSurface_ =
        Drawing::Surface::MakeRasterN32Premul(static_cast<int32_t>(cacheSize.x_), static_cast<int32_t>(cacheSize.y_));
#endif
}

bool RSRenderNodeDrawable::NeedInitCachedSurface(const Vector2f& newSize)
{
    auto width = static_cast<int32_t>(newSize.x_);
    auto height = static_cast<int32_t>(newSize.y_);
    if (GetOpincDrawCache().IsComputeDrawAreaSucc()) {
        auto& unionRect = GetOpincDrawCache().GetOpListUnionArea();
        width = static_cast<int32_t>(unionRect.GetWidth());
        height = static_cast<int32_t>(unionRect.GetHeight());
    }
    std::scoped_lock<std::recursive_mutex> lock(cacheMutex_);
    if (cachedSurface_ == nullptr) {
        return true;
    }
    auto cacheCanvas = cachedSurface_->GetCanvas();
    if (cacheCanvas == nullptr) {
        return true;
    }
    return cacheCanvas->GetWidth() != width || cacheCanvas->GetHeight() != height;
}

#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
struct SharedTextureContext {
    SharedTextureContext(std::shared_ptr<Drawing::Image> sharedImage)
        : sharedImage_(std::move(sharedImage)) {}

private:
    std::shared_ptr<Drawing::Image> sharedImage_;
};

static void DeleteSharedTextureContext(void* context)
{
    SharedTextureContext* cleanupHelper = static_cast<SharedTextureContext*>(context);
    if (cleanupHelper != nullptr) {
        delete cleanupHelper;
    }
}
#endif

std::shared_ptr<Drawing::Image> RSRenderNodeDrawable::GetCachedImage(RSPaintFilterCanvas& canvas)
{
    std::scoped_lock<std::recursive_mutex> lock(cacheMutex_);
    if (!cachedSurface_ || !cachedImage_) {
        RS_LOGE("RSRenderNodeDrawable::GetCachedImage invalid cachedSurface_");
        return nullptr;
    }

    // do not use threadId to judge image grcontext change
    if (cachedImage_->IsValid(canvas.GetGPUContext().get())) {
        return cachedImage_;
    }
#ifdef RS_ENABLE_GL
    if (OHOS::Rosen::RSSystemProperties::GetGpuApiType() != OHOS::Rosen::GpuApiType::VULKAN &&
        OHOS::Rosen::RSSystemProperties::GetGpuApiType() != OHOS::Rosen::GpuApiType::DDGR) {
        if (canvas.GetGPUContext() == nullptr) {
            return nullptr;
        }
        Drawing::TextureOrigin origin = Drawing::TextureOrigin::BOTTOM_LEFT;
        Drawing::BitmapFormat info = Drawing::BitmapFormat{cachedImage_->GetColorType(), cachedImage_->GetAlphaType()};
        SharedTextureContext* sharedContext = new SharedTextureContext(cachedImage_); // will move image
        cachedImage_ = std::make_shared<Drawing::Image>();
        bool ret = cachedImage_->BuildFromTexture(*canvas.GetGPUContext(), cachedBackendTexture_.GetTextureInfo(),
            origin, info, nullptr, DeleteSharedTextureContext, sharedContext);
        if (!ret) {
            RS_LOGE("RSRenderNodeDrawable::GetCachedImage image BuildFromTexture failed");
            return nullptr;
        }
    }
#endif

#ifdef RS_ENABLE_VK
    if (OHOS::Rosen::RSSystemProperties::GetGpuApiType() == OHOS::Rosen::GpuApiType::VULKAN ||
        OHOS::Rosen::RSSystemProperties::GetGpuApiType() == OHOS::Rosen::GpuApiType::DDGR) {
        if (vulkanCleanupHelper_ == nullptr || canvas.GetGPUContext() == nullptr) {
            return nullptr;
        }
        Drawing::TextureOrigin origin = Drawing::TextureOrigin::BOTTOM_LEFT;
        Drawing::BitmapFormat info = Drawing::BitmapFormat{cachedImage_->GetColorType(), cachedImage_->GetAlphaType()};
        // Ensure the image's color space matches the target surface's color profile.
        auto colorSpace = cachedSurface_->GetImageInfo().GetColorSpace();
        cachedImage_ = std::make_shared<Drawing::Image>();
        bool ret = cachedImage_->BuildFromTexture(*canvas.GetGPUContext(), cachedBackendTexture_.GetTextureInfo(),
            origin, info, colorSpace, NativeBufferUtils::DeleteVkImage, vulkanCleanupHelper_->Ref());
        if (!ret) {
            RS_LOGE("RSRenderNodeDrawable::GetCachedImage image BuildFromTexture failed");
            return nullptr;
        }
    }
#endif
    return cachedImage_;
}

void RSRenderNodeDrawable::SetCacheImageByCapture(std::shared_ptr<Drawing::Image> image)
{
    std::lock_guard<std::mutex> lock(freezeByCaptureMutex_);
    cachedImageByCapture_ = image;
}

std::shared_ptr<Drawing::Image> RSRenderNodeDrawable::GetCacheImageByCapture() const
{
    std::lock_guard<std::mutex> lock(freezeByCaptureMutex_);
    return cachedImageByCapture_;
}

void RSRenderNodeDrawable::DrawCachedImage(RSPaintFilterCanvas& canvas, const Vector2f& boundSize,
    const std::shared_ptr<RSFilter>& rsFilter)
{
    auto cacheImage = GetCachedImage(canvas);
    std::lock_guard<std::mutex> lock(freezeByCaptureMutex_);
    if (cachedImageByCapture_) {
        // node has freezed, and to draw surfaceCapture image
        cacheImage = cachedImageByCapture_;
    }
    if (cacheImage == nullptr) {
        RS_LOGE("RSRenderNodeDrawable::DrawCachedImage image null");
        return;
    }
    if (RSSystemProperties::GetRecordingEnabled()) {
        if (cacheImage->IsTextureBacked()) {
            RS_LOGI("RSRenderNodeDrawable::DrawCachedImage convert cacheImage from texture to raster image");
            cacheImage = cacheImage->MakeRasterImage();
        }
    }
    if (cacheImage == nullptr || cacheImage->GetWidth() == 0 || cacheImage->GetHeight() == 0) {
        RS_LOGE("RSRenderNodeDrawable::DrawCachedImage invalid cacheimage");
        return;
    }
    float scaleX = boundSize.x_ / static_cast<float>(cacheImage->GetWidth());
    float scaleY = boundSize.y_ / static_cast<float>(cacheImage->GetHeight());
    if (GetOpincDrawCache().IsComputeDrawAreaSucc()) {
        auto& unionRect = GetOpincDrawCache().GetOpListUnionArea();
        scaleX = unionRect.GetWidth() / static_cast<float>(cacheImage->GetWidth());
        scaleY = unionRect.GetHeight() / static_cast<float>(cacheImage->GetHeight());
    }

    Drawing::AutoCanvasRestore arc(canvas, true);
    canvas.Scale(scaleX, scaleY);
    Drawing::Brush brush;
    canvas.AttachBrush(brush);
    auto samplingOptions = Drawing::SamplingOptions(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NONE);
    if (GetOpincDrawCache().IsComputeDrawAreaSucc() && GetOpincDrawCache().DrawAutoCache(canvas, *cacheImage,
        samplingOptions, Drawing::SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT)) {
        canvas.DetachBrush();
        GetOpincDrawCache().DrawAutoCacheDfx(canvas, autoCacheRenderNodeInfos_);
        return;
    }
    if (rsFilter != nullptr) {
        RS_OPTIONAL_TRACE_NAME_FMT("RSRenderNodeDrawable::DrawCachedImage image width: %d, height: %d, %s",
            cacheImage->GetWidth(), cacheImage->GetHeight(), rsFilter->GetDescription().c_str());
        auto foregroundFilter = std::static_pointer_cast<RSDrawingFilterOriginal>(rsFilter);
        foregroundFilter->DrawImageRect(canvas, cacheImage, Drawing::Rect(0, 0, cacheImage->GetWidth(),
        cacheImage->GetHeight()), Drawing::Rect(0, 0, cacheImage->GetWidth(), cacheImage->GetHeight()));
     } else {
         canvas.DrawImage(*cacheImage, 0.0, 0.0, samplingOptions);
     }
    canvas.DetachBrush();
}

void RSRenderNodeDrawable::ClearCachedSurface()
{
    SetCacheType(DrawableCacheType::NONE);
    std::scoped_lock<std::recursive_mutex> lock(cacheMutex_);
    if (cachedSurface_ == nullptr) {
        return;
    }
    RS_OPTIONAL_TRACE_NAME_FMT("ClearCachedSurface id:%llu", GetId());

    auto clearTask = [surface = cachedSurface_]() mutable { surface = nullptr; };
    cachedSurface_ = nullptr;
    cachedImage_ = nullptr;
    RSTaskDispatcher::GetInstance().PostTask(cacheThreadId_.load(), clearTask);

#ifdef RS_ENABLE_VK
    if (OHOS::Rosen::RSSystemProperties::GetGpuApiType() == OHOS::Rosen::GpuApiType::VULKAN ||
        OHOS::Rosen::RSSystemProperties::GetGpuApiType() == OHOS::Rosen::GpuApiType::DDGR) {
        vulkanCleanupHelper_ = nullptr;
    }
#endif
}

bool RSRenderNodeDrawable::CheckIfNeedUpdateCache(RSRenderParams& params, int32_t& updateTimes)
{
    {
        std::lock_guard<std::mutex> lock(drawingCacheMapMutex_);
        if (drawingCacheUpdateTimeMap_.count(nodeId_) > 0) {
            updateTimes = drawingCacheUpdateTimeMap_.at(nodeId_);
        }
    }

    RS_OPTIONAL_TRACE_NAME_FMT("CheckUpdateCache id:%llu updateTimes:%d type:%d cacheChanged:%d size:[%.2f, %.2f]",
        nodeId_, updateTimes, params.GetDrawingCacheType(), params.GetDrawingCacheChanged(),
        params.GetCacheSize().x_, params.GetCacheSize().y_);

    // node freeze
    if (params.GetRSFreezeFlag()) {
        return updateTimes == 0;
    }

    if ((params.GetDrawingCacheType() == RSDrawingCacheType::TARGETED_CACHE && params.NeedFilter() &&
        params.GetDrawingCacheIncludeProperty()) || ROSEN_LE(params.GetCacheSize().x_, 0.f) ||
        ROSEN_LE(params.GetCacheSize().y_, 0.f)) {
        params.SetDrawingCacheType(RSDrawingCacheType::DISABLED_CACHE);
        ClearCachedSurface();
        return false;
    }

    if (NeedInitCachedSurface(params.GetCacheSize())) {
        ClearCachedSurface();
        return true;
    }

    if (updateTimes == 0 || params.GetDrawingCacheChanged()) {
        return true;
    }
    return false;
}

void RSRenderNodeDrawable::UpdateCacheSurface(Drawing::Canvas& canvas, const RSRenderParams& params)
{
    auto startTime = RSPerfMonitorReporter::GetInstance().StartRendergroupMonitor();
    auto curCanvas = static_cast<RSPaintFilterCanvas*>(&canvas);
    pid_t threadId = gettid();
#ifdef SUBTREE_PARALLEL_ENABLE
    // Adapt to the subtree feature to ensure the correct thread ID(TID) is used.
    RSParallelMisc::AdaptSubTreeThreadId(threadId, threadId);
#endif

    bool isHdrOn = false;
    bool isScRGBEnable = RSSystemParameters::IsNeedScRGBForP3(curCanvas->GetTargetColorGamut()) &&
        RSUifirstManager::Instance().GetUiFirstSwitch();
    bool isNeedFP16 = isHdrOn || isScRGBEnable;
    auto cacheSurface = GetCachedSurface(threadId);
    if (cacheSurface == nullptr) {
        RS_TRACE_NAME_FMT("InitCachedSurface size:[%.2f, %.2f]", params.GetCacheSize().x_, params.GetCacheSize().y_);
        InitCachedSurface(curCanvas->GetGPUContext().get(), params.GetCacheSize(), threadId, isNeedFP16,
            curCanvas->GetTargetColorGamut());
        cacheSurface = GetCachedSurface(threadId);
        if (cacheSurface == nullptr) {
            return;
        }
    }

    auto cacheCanvas = std::make_shared<RSPaintFilterCanvas>(cacheSurface.get());
    if (!cacheCanvas) {
        return;
    }

    // copy current canvas properties into cacheCanvas
    const auto& renderEngine = RSUniRenderThread::Instance().GetRenderEngine();
    if (renderEngine) {
        cacheCanvas->SetHighContrast(renderEngine->IsHighContrastEnabled());
    }
    cacheCanvas->CopyConfigurationToOffscreenCanvas(*curCanvas);
    cacheCanvas->CopyHDRConfiguration(*curCanvas);
    // Using filter cache in multi-thread environment may cause GPU memory leak or invalid textures
    // [PLANNNING] disable it in sub-thread.

    // When drawing CacheSurface, all child node should be drawn.
    // So set isOpDropped_ = false here.
    bool isOpDropped = isOpDropped_;
    isOpDropped_ = false;
    Drawing::AutoCanvasRestore arc(*cacheCanvas, true);
    cacheCanvas->Clear(Drawing::Color::COLOR_TRANSPARENT);

    GetOpincDrawCache().OpincCanvasUnionTranslate(*cacheCanvas);
    if (params.GetRSFreezeFlag()) {
        cacheCanvas->SetDisableFilterCache(true);
    }
    // draw content + children
    auto bounds = params.GetBounds();
    ApplyForegroundColorIfNeed(*cacheCanvas, bounds);
    if (LIKELY(!params.GetDrawingCacheIncludeProperty())) {
        DrawContent(*cacheCanvas, params.GetFrameRect());
        DrawChildren(*cacheCanvas, bounds);
    } else if (params.GetForegroundFilterCache() != nullptr) {
        DrawCacheWithForegroundFilter(*cacheCanvas, bounds);
    } else {
        DrawCacheWithProperty(*cacheCanvas, bounds);
    }
    GetOpincDrawCache().ResumeOpincCanvasTranslate(*cacheCanvas);

    isOpDropped_ = isOpDropped;

    // get image & backend
    {
        std::scoped_lock<std::recursive_mutex> lock(cacheMutex_);
        cachedImage_ = cacheSurface->GetImageSnapshot();
        if (cachedImage_) {
            SetCacheType(DrawableCacheType::CONTENT);
        }
    }

#if RS_ENABLE_GL
    // vk backend has been created when surface init.
    if (OHOS::Rosen::RSSystemProperties::GetGpuApiType() != OHOS::Rosen::GpuApiType::VULKAN &&
        OHOS::Rosen::RSSystemProperties::GetGpuApiType() != OHOS::Rosen::GpuApiType::DDGR) {
        cachedBackendTexture_ = cacheSurface->GetBackendTexture();
    }
#endif
    // update cache updateTimes
    int32_t updateTimes = 0;
    {
        std::lock_guard<std::mutex> lock(drawingCacheMapMutex_);
        drawingCacheUpdateTimeMap_[nodeId_]++;
        updateTimes = drawingCacheUpdateTimeMap_[nodeId_];
    }
    {
        std::lock_guard<std::mutex> lock(drawingCacheContiUpdateTimeMapMutex_);
        drawingCacheContinuousUpdateTimeMap_[nodeId_]++;
    }
    {
        std::lock_guard<std::mutex> lock(drawingCacheInfoMutex_);
        cacheUpdatedNodeMap_.emplace(params.GetId(), true);
    }
    auto ctx = RSUniRenderThread::Instance().GetRSRenderThreadParams()->GetContext();
    RSPerfMonitorReporter::GetInstance().EndRendergroupMonitor(startTime, nodeId_, ctx, updateTimes);
}

void RSRenderNodeDrawable::UpdateFilterDisplayHeadroom(Drawing::Canvas& canvas)
{
    if (canvas.GetDrawingType() != Drawing::DrawingType::PAINT_FILTER) {
        return;
    }

    auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(&canvas);
    NodeId screenId = paintFilterCanvas->GetScreenId();
    auto headroom = RSEffectLuminanceManager::GetInstance().GetDisplayHeadroom(screenId);

    const auto& params = GetRenderParams();
    if (!params) {
        return;
    }
    if (params->GetForegroundFilterCache()) {
        params->GetForegroundFilterCache()->SetDisplayHeadroom(headroom);
    }
    if (params->GetBackgroundFilter()) {
        params->GetBackgroundFilter()->SetDisplayHeadroom(headroom);
    }
}

int RSRenderNodeDrawable::GetTotalProcessedNodeCount()
{
    return totalProcessedNodeCount_;
}

void RSRenderNodeDrawable::TotalProcessedNodeCountInc()
{
    ++totalProcessedNodeCount_;
}

void RSRenderNodeDrawable::ClearTotalProcessedNodeCount()
{
    totalProcessedNodeCount_ = 0;
}

int RSRenderNodeDrawable::GetProcessedNodeCount()
{
    return processedNodeCount_;
}

void RSRenderNodeDrawable::ProcessedNodeCountInc()
{
    ++processedNodeCount_;
}

void RSRenderNodeDrawable::ClearProcessedNodeCount()
{
    processedNodeCount_ = 0;
}

int RSRenderNodeDrawable::GetSnapshotProcessedNodeCount()
{
    return snapshotProcessedNodeCount_;
}

void RSRenderNodeDrawable::SnapshotProcessedNodeCountInc()
{
    if (RSUniRenderThread::GetCaptureParam().isSingleSurface_) {
        ++snapshotProcessedNodeCount_;
    }
}

void RSRenderNodeDrawable::ClearSnapshotProcessedNodeCount()
{
    snapshotProcessedNodeCount_ = 0;
}

bool RSRenderNodeDrawable::ShouldPaint() const
{
    return LIKELY(renderParams_ != nullptr) && renderParams_->GetShouldPaint();
}

// opinc dfx
std::string RSRenderNodeDrawable::GetNodeDebugInfo()
{
    std::string ret("");
    const auto& params = GetRenderParams();
    if (!params) {
        return ret;
    }
    auto& unionRect = opincDrawCache_.GetOpListUnionArea();
    AppendFormat(ret, "%" PRIu64 ", rootF:%d record:%d rootS:%d opCan:%d isRD:%d, GetOpDropped:%d,"
        " isOpincDropNodeExt:%d", params->GetId(), params->OpincGetRootFlag(),
        opincDrawCache_.GetRecordState(), opincDrawCache_.GetRootNodeStrategyType(), opincDrawCache_.IsOpCanCache(),
        opincDrawCache_.GetDrawAreaEnableState(), GetOpDropped(), isOpincDropNodeExt_);
    auto& info = opincDrawCache_.GetOpListHandle().GetOpInfo();
    AppendFormat(ret, " opNum:%d opPercent:%d", info.num, info.percent);
    auto bounds = params->GetBounds();
    AppendFormat(ret, ", rr{%.1f %.1f %.1f %.1f}",
        0.f, 0.f, bounds.GetWidth(), bounds.GetHeight());
    AppendFormat(ret, ", ur{%.1f %.1f %.1f %.1f}",
        unionRect.GetLeft(), unionRect.GetTop(), unionRect.GetWidth(), unionRect.GetHeight());
    return ret;
}
void RSRenderNodeDrawable::ClearOpincState()
{
    // Init opincRootTotalCount when the new thread init
    opincRootTotalCount_ = 0;
    isOpincDropNodeExt_ = true;
}

} // namespace OHOS::Rosen::DrawableV2
