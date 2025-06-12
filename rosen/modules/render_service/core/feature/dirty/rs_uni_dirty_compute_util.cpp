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

#include "rs_uni_dirty_compute_util.h"

#include <cstdint>
#include <memory>
#include <parameter.h>
#include <parameters.h>
#include <string>

#include "common/rs_optional_trace.h"
#include "common/rs_rectangles_merger.h"
#include "drawable/dfx/rs_dirty_rects_dfx.h"
#include "drawable/rs_display_render_node_drawable.h"
#include "drawable/rs_surface_render_node_drawable.h"
#ifdef RS_ENABLE_OVERLAY_DISPLAY
#include "feature/overlay_display/rs_overlay_display_manager.h"
#endif
#include "feature/uifirst/rs_uifirst_manager.h"
#include "info_collection/rs_gpu_dirty_region_collection.h"
#include "params/rs_display_render_params.h"
#include "params/rs_surface_render_params.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/render_thread/rs_base_render_util.h"
#include "pipeline/rs_effect_render_node.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "platform/common/rs_log.h"
#include "property/rs_properties.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr int32_t NO_SPECIAL_LAYER = 0;
}
std::vector<RectI> RSUniDirtyComputeUtil::GetCurrentFrameVisibleDirty(
    DrawableV2::RSDisplayRenderNodeDrawable& displayDrawable, ScreenInfo& screenInfo, RSDisplayRenderParams& params)
{
    Occlusion::Region damageRegions;
    auto& curAllSurfaceDrawables = params.GetAllMainAndLeashSurfaceDrawables();
    // update all child surfacenode history
    for (auto it = curAllSurfaceDrawables.rbegin(); it != curAllSurfaceDrawables.rend(); ++it) {
        auto surfaceNodeDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(*it);
        if (surfaceNodeDrawable == nullptr) {
            RS_LOGI("GetCurrentFrameVisibleDirty surfaceNodeDrawable is nullptr");
            continue;
        }
        auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceNodeDrawable->GetRenderParams().get());
        auto surfaceDirtyManager = surfaceNodeDrawable->GetSyncDirtyManager();
        if (!surfaceParams || !surfaceDirtyManager) {
            RS_LOGI("RSUniDirtyComputeUtil::GetCurrentFrameVisibleDirty node(%{public}" PRIu64") params or "
                "dirty manager is nullptr", surfaceNodeDrawable->GetId());
            continue;
        }
        if (!surfaceParams->IsAppWindow() || surfaceParams->GetDstRect().IsEmpty()) {
            continue;
        }
        // for cross-display surface, only consider the dirty region on the first display (use global dirty for others).
        if (surfaceParams->IsFirstLevelCrossNode() &&
            !RSUniRenderThread::Instance().GetRSRenderThreadParams()->IsFirstVisitCrossNodeDisplay()) {
            continue;
        }
        auto visibleRegion = surfaceParams->GetVisibleRegion();
        auto surfaceCurrentFrameDirtyRegion = surfaceDirtyManager->GetCurrentFrameDirtyRegion();
        Occlusion::Region currentFrameDirtyRegion { Occlusion::Rect {
            surfaceCurrentFrameDirtyRegion.left_, surfaceCurrentFrameDirtyRegion.top_,
            surfaceCurrentFrameDirtyRegion.GetRight(), surfaceCurrentFrameDirtyRegion.GetBottom() } };
        Occlusion::Region damageRegion = currentFrameDirtyRegion.And(visibleRegion);
        damageRegions.OrSelf(damageRegion);
    }
    auto rects = RSUniDirtyComputeUtil::ScreenIntersectDirtyRects(damageRegions, screenInfo);
    RectI rect = displayDrawable.GetSyncDirtyManager()->GetDirtyRegionFlipWithinSurface();
    if (!rect.IsEmpty()) {
        rects.emplace_back(rect);
    }
    return rects;
}

std::vector<RectI> RSUniDirtyComputeUtil::ScreenIntersectDirtyRects(
    const Occlusion::Region &region, ScreenInfo& screenInfo)
{
    const std::vector<Occlusion::Rect>& rects = region.GetRegionRects();
    std::vector<RectI> retRects;
    for (const Occlusion::Rect& rect : rects) {
        // origin transformation
#ifdef RS_ENABLE_VK
        if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
            RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
            retRects.emplace_back(RectI(rect.left_, rect.top_,
                rect.right_ - rect.left_, rect.bottom_ - rect.top_));
        } else {
            retRects.emplace_back(RectI(rect.left_, screenInfo.GetRotatedHeight() - rect.bottom_,
                rect.right_ - rect.left_, rect.bottom_ - rect.top_));
        }
#else
        retRects.emplace_back(RectI(rect.left_, screenInfo.GetRotatedHeight() - rect.bottom_,
            rect.right_ - rect.left_, rect.bottom_ - rect.top_));
#endif
    }
    RS_LOGD("ScreenIntersectDirtyRects size %{public}d %{public}s", region.GetSize(), region.GetRegionInfo().c_str());
    return retRects;
}

std::vector<RectI> RSUniDirtyComputeUtil::GetFilpDirtyRects(
    const std::vector<RectI>& srcRects, const ScreenInfo& screenInfo)
{
#ifdef RS_ENABLE_VK
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return srcRects;
    }
#endif

    return FilpRects(srcRects, screenInfo);
}

std::vector<RectI> RSUniDirtyComputeUtil::FilpRects(const std::vector<RectI>& srcRects, const ScreenInfo& screenInfo)
{
    std::vector<RectI> retRects;
    for (const RectI& rect : srcRects) {
        retRects.emplace_back(RectI(rect.left_, screenInfo.GetRotatedHeight() - rect.top_ - rect.height_,
            rect.width_, rect.height_));
    }
    return retRects;
}

GraphicIRect RSUniDirtyComputeUtil::IntersectRect(const GraphicIRect& first, const GraphicIRect& second)
{
    int left = std::max(first.x, second.x);
    int top = std::max(first.y, second.y);
    int right = std::min(first.x + first.w, second.x + second.w);
    int bottom = std::min(first.y + first.h, second.y + second.h);
    int width = right - left;
    int height = bottom - top;

    if (width <= 0 || height <= 0) {
        return GraphicIRect { 0, 0, 0, 0 };
    } else {
        return GraphicIRect { left, top, width, height };
    }
}

void RSUniFilterDirtyComputeUtil::DealWithFilterDirtyRegion(Occlusion::Region& damageRegion,
    Occlusion::Region& drawRegion, DrawableV2::RSDisplayRenderNodeDrawable& displayDrawable,
    const std::optional<Drawing::Matrix>& matrix, bool dirtyAlign)
{
    dirtyAlignEnabled_ = dirtyAlign;
    auto& displayParams = displayDrawable.GetRenderParams();
    if (UNLIKELY(displayParams == nullptr)) {
        return;
    }
    auto& surfaceDrawables = displayParams->GetAllMainAndLeashSurfaceDrawables();
    // Iteratively process filters recorded in display manager and surface manager, until convergence.
    bool elementChanged = false;
    do {
        elementChanged = false;
        elementChanged |= DealWithFilterDirtyForDisplay(damageRegion, drawRegion, displayDrawable, matrix);
        elementChanged |= DealWithFilterDirtyForSurface(damageRegion, drawRegion, surfaceDrawables, matrix);
    } while (elementChanged);
    ResetFilterInfoStatus(displayDrawable, surfaceDrawables);
}

bool RSUniFilterDirtyComputeUtil::DealWithFilterDirtyForDisplay(Occlusion::Region& damageRegion,
    Occlusion::Region& drawRegion, DrawableV2::RSDisplayRenderNodeDrawable& displayDrawable,
    const std::optional<Drawing::Matrix>& matrix)
{
    auto displayDirtyManager = displayDrawable.GetSyncDirtyManager();
    if (UNLIKELY(displayDirtyManager == nullptr)) {
        return false;
    }
    return CheckMergeFilterDirty(damageRegion, drawRegion,
        displayDirtyManager->GetFilterCollector().GetFilterDirtyRegionInfoList(true), matrix, std::nullopt);
}

bool RSUniFilterDirtyComputeUtil::DealWithFilterDirtyForSurface(Occlusion::Region& damageRegion,
    Occlusion::Region& drawRegion, std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr>& drawables,
    const std::optional<Drawing::Matrix>& matrix)
{
    bool elementChanged = false;
    for (auto it = drawables.begin(); it != drawables.end(); ++it) {
        auto surfaceNodeDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(*it);
        if (UNLIKELY(surfaceNodeDrawable == nullptr)) {
            RS_LOGI("DealWithFilterDirtyForSurface surfaceNodeDrawable is nullptr");
            continue;
        }
        auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceNodeDrawable->GetRenderParams().get());
        auto surfaceDirtyManager = surfaceNodeDrawable->GetSyncDirtyManager();
        if (UNLIKELY(surfaceParams == nullptr || surfaceDirtyManager == nullptr)) {
            RS_LOGI("DealWithFilterDirtyForSurface surface param or dirty manager is nullptr");
            continue;
        }
        elementChanged |= CheckMergeFilterDirty(damageRegion, drawRegion,
            surfaceDirtyManager->GetFilterCollector().GetFilterDirtyRegionInfoList(true),
            matrix, surfaceParams->GetVisibleRegion());
    }
    return elementChanged;
}

bool RSUniFilterDirtyComputeUtil::CheckMergeFilterDirty(Occlusion::Region& damageRegion, Occlusion::Region& drawRegion,
    FilterDirtyRegionInfoList& filterList, const std::optional<Drawing::Matrix>& matrix,
    const std::optional<Occlusion::Region>& visibleRegion)
{
    auto addDirtyInIntersect = [&] (FilterDirtyRegionInfo& info) {
        // case - 1. If this filter is already counted in damage region, skip it.
        if (info.addToDirty_) {
            return false;
        }
        // case - 2. If this filter is not intersected with drawRegion, skip it.
        Occlusion::Region intersectRegion = matrix.has_value() ?
            RSObjAbsGeometry::MapRegion(info.intersectRegion_, matrix.value()) : info.intersectRegion_;
        intersectRegion = visibleRegion.has_value() ?
            intersectRegion.And(visibleRegion.value()) : intersectRegion;
        if (drawRegion.And(intersectRegion).IsEmpty()) {
            return false;
        }
        // case - 3. Add this filter into both damage region (for GPU) and draw region (for RS).
        RS_OPTIONAL_TRACE_NAME_FMT("Filter [%" PRIu64 "], intersected with draw region: %s, add %s to damage.",
            info.id_, drawRegion.GetRegionInfo().c_str(), info.filterDirty_.GetRegionInfo().c_str());
        Occlusion::Region dirtyRegion = matrix.has_value() ?
            RSObjAbsGeometry::MapRegion(info.filterDirty_, matrix.value()) : info.filterDirty_;
        Occlusion::Region alignedDirtyRegion = matrix.has_value() ?
            RSObjAbsGeometry::MapRegion(info.alignedFilterDirty_, matrix.value()) : info.alignedFilterDirty_;
        damageRegion.OrSelf(dirtyRegion);
        drawRegion.OrSelf(dirtyAlignEnabled_ ? alignedDirtyRegion : dirtyRegion);
        info.addToDirty_ = true;
        return true;
    };
    // return if any filter is added to damage region and draw region.
    return std::find_if(filterList.begin(), filterList.end(), addDirtyInIntersect) != filterList.end();
}

void RSUniFilterDirtyComputeUtil::ResetFilterInfoStatus(DrawableV2::RSDisplayRenderNodeDrawable& displayDrawable,
    std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr>& surfaceDrawables)
{
    auto resetFilterStatus = [] (FilterDirtyRegionInfo& filterInfo) { filterInfo.addToDirty_ = false; };
    if (auto displayDirtyManager = displayDrawable.GetSyncDirtyManager()) {
        auto& displayFilterList = displayDirtyManager->GetFilterCollector().GetFilterDirtyRegionInfoList(true);
        std::for_each(displayFilterList.begin(), displayFilterList.end(), resetFilterStatus);
    }
    for (auto it = surfaceDrawables.rbegin(); it != surfaceDrawables.rend(); ++it) {
        auto surfaceNodeDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(*it);
        if (UNLIKELY(surfaceNodeDrawable == nullptr)) {
            RS_LOGI("ResetFilterInfoStatus surfaceNodeDrawable is nullptr");
            continue;
        }
        if (auto surfaceDirtyManager = surfaceNodeDrawable->GetSyncDirtyManager()) {
            auto& surfaceFilterList = surfaceDirtyManager->GetFilterCollector().GetFilterDirtyRegionInfoList(true);
            std::for_each(surfaceFilterList.begin(), surfaceFilterList.end(), resetFilterStatus);
        }
    }
}

FilterDirtyRegionInfo RSUniFilterDirtyComputeUtil::GenerateFilterDirtyRegionInfo(
    RSRenderNode& filterNode, const std::optional<Occlusion::Region>& preDirty)
{
    bool effectNodeExpandDirty =
        filterNode.IsInstanceOf<RSEffectRenderNode>() && !filterNode.FirstFrameHasEffectChildren();
    auto filterRegion = effectNodeExpandDirty ?
        GetVisibleEffectRegion(filterNode) : Occlusion::Region(Occlusion::Rect(filterNode.GetOldDirtyInSurface()));
    auto dirtyRegion = effectNodeExpandDirty ?
        filterRegion.Or(Occlusion::Region(Occlusion::Rect(filterNode.GetFilterRect()))) : filterRegion;
    if (filterNode.NeedDrawBehindWindow()) {
        filterRegion = Occlusion::Region(Occlusion::Rect(filterNode.GetFilterRect()));
        dirtyRegion = filterRegion;
    }
    FilterDirtyRegionInfo filterInfo = {
        .id_ = filterNode.GetId(),
        .intersectRegion_ = filterRegion,
        .filterDirty_ = dirtyRegion,
        .alignedFilterDirty_ = dirtyRegion.GetAlignedRegion(MAX_DIRTY_ALIGNMENT_SIZE),
        .belowDirty_ = preDirty.value_or(Occlusion::Region())
    };
    return filterInfo;
}

Occlusion::Region RSUniFilterDirtyComputeUtil::GetVisibleEffectRegion(RSRenderNode& filterNode)
{
    auto context = filterNode.GetContext().lock();
    Occlusion::Region childEffectRegion;
    if (!context) {
        RS_LOGE("GetVisibleEffectRegion filter node%{public}" PRIu64 " context is nullptr", filterNode.GetId());
        return childEffectRegion;
    }
    for (auto& nodeId : filterNode.GetVisibleEffectChild()) {
        if (auto& subNode = context->GetNodeMap().GetRenderNode<RSRenderNode>(nodeId)) {
            childEffectRegion = childEffectRegion.OrSelf(Occlusion::Region(subNode->GetOldDirtyInSurface()));
        }
    }
    return childEffectRegion;
}

void RSUniDirtyComputeUtil::UpdateVirtualExpandDisplayAccumulatedParams(
    RSDisplayRenderParams& params, DrawableV2::RSDisplayRenderNodeDrawable& displayDrawable)
{
    // All other factors that may prevent skipping virtual expand display need to be considered
    // update accumulated dirty region
    params.SetAccumulatedDirty(params.GetAccumulatedDirty() ||
        (displayDrawable.GetSyncDirtyManager()->IsCurrentFrameDirty() || params.GetMainAndLeashSurfaceDirty()));

    // update accumulated hdr status changed
    params.SetAccumulatedHdrStatusChanged(params.GetAccumulatedHdrStatusChanged() || params.IsHDRStatusChanged());
}

bool RSUniDirtyComputeUtil::CheckVirtualExpandDisplaySkip(
    RSDisplayRenderParams& params, DrawableV2::RSDisplayRenderNodeDrawable& displayDrawable)
{
    // Regardless of whether the current frame is skipped, the state needs to be accumulated
    if (!RSSystemProperties::GetVirtualExpandScreenSkipEnabled()) {
        return false;
    }
    if (displayDrawable.GetSpecialLayerType(params) != NO_SPECIAL_LAYER) {
        RS_TRACE_NAME("CheckVirtualExpandDisplaySkip has special layer can not skip");
        return false;
    }
    RS_TRACE_NAME_FMT("CheckVirtualExpandDisplaySkip isAccumulatedDirty: %d, isAccumulatedHdrStatusChanged: %d",
        params.GetAccumulatedDirty(), params.GetAccumulatedHdrStatusChanged());
    return !params.GetAccumulatedDirty() && !params.GetAccumulatedHdrStatusChanged();
}
} // namespace Rosen
} // namespace OHOS