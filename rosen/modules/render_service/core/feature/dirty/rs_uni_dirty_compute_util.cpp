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
#include "drawable/rs_logical_display_render_node_drawable.h"
#include "drawable/rs_screen_render_node_drawable.h"
#include "drawable/rs_surface_render_node_drawable.h"
#ifdef RS_ENABLE_OVERLAY_DISPLAY
#include "feature/overlay_display/rs_overlay_display_manager.h"
#endif
#include "feature/special_layer/rs_special_layer_utils.h"
#include "feature/uifirst/rs_uifirst_manager.h"
#include "info_collection/rs_gpu_dirty_region_collection.h"
#include "params/rs_screen_render_params.h"
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

std::vector<RectI> RSUniDirtyComputeUtil::GetCurrentFrameVisibleDirty(
    DrawableV2::RSScreenRenderNodeDrawable& screenDrawable, ScreenInfo& screenInfo, RSScreenRenderParams& params)
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
        if (!surfaceParams->IsLeashOrMainWindow() || surfaceParams->GetDstRect().IsEmpty()) {
            continue;
        }
        // for cross-screen surface, only consider the dirty region on the first screen (use global dirty for others).
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
        auto& surfaceFilterCollector = surfaceDirtyManager->GetFilterCollector();
        damageRegions.OrSelf(surfaceFilterCollector.GetPureCleanFilterDirtyRegion());
        surfaceFilterCollector.ClearPureCleanFilterDirtyRegion();
    }
    auto screenDirtyManager = screenDrawable.GetSyncDirtyManager();
    if (screenDirtyManager == nullptr) {
        return {};
    }
    auto& displayFilterCollector = screenDirtyManager->GetFilterCollector();
    damageRegions.OrSelf(displayFilterCollector.GetPureCleanFilterDirtyRegion());
    displayFilterCollector.ClearPureCleanFilterDirtyRegion();
    auto rects = RSUniDirtyComputeUtil::ScreenIntersectDirtyRects(damageRegions, screenInfo);
    RectI rect = screenDirtyManager->GetDirtyRegionFlipWithinSurface();
    if (!rect.IsEmpty()) {
        rects.emplace_back(rect);
    }
    return rects;
}

std::vector<RectI> RSUniDirtyComputeUtil::ScreenIntersectDirtyRects(
    const Occlusion::Region &region, const ScreenInfo& screenInfo)
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
    Occlusion::Region& drawRegion, DrawableV2::RSScreenRenderNodeDrawable& screenDrawable,
    const std::optional<Drawing::Matrix>& matrix, bool dirtyAlign)
{
    dirtyAlignEnabled_ = dirtyAlign;
    if (UNLIKELY(screenDrawable.GetRenderParams() == nullptr)) {
        return;
    }
    auto screenParams = static_cast<RSScreenRenderParams*>(screenDrawable.GetRenderParams().get());
    // If screen zoomed, filter cache is globally disabled, thus partial render of filter cache should be disabled.
    RSFilterDirtyCollector::SetValidCachePartialRender(!screenParams->GetZoomed());
    auto& surfaceDrawables = screenParams->GetAllMainAndLeashSurfaceDrawables();
    // Iteratively process filters recorded in screen manager and surface manager, until convergence.
    bool elementChanged = false;
    do {
        elementChanged = false;
        elementChanged |= DealWithFilterDirtyForScreen(damageRegion, drawRegion, screenDrawable, matrix);
        elementChanged |= DealWithFilterDirtyForSurface(damageRegion, drawRegion, surfaceDrawables, matrix);
    } while (elementChanged);
    ResetFilterInfoStatus(screenDrawable, surfaceDrawables);
}

bool RSUniFilterDirtyComputeUtil::DealWithFilterDirtyForScreen(Occlusion::Region& damageRegion,
    Occlusion::Region& drawRegion, DrawableV2::RSScreenRenderNodeDrawable& screenDrawable,
    const std::optional<Drawing::Matrix>& matrix)
{
    auto screenDirtyManager = screenDrawable.GetSyncDirtyManager();
    if (UNLIKELY(screenDirtyManager == nullptr)) {
        return false;
    }
    return CheckMergeFilterDirty(damageRegion, drawRegion, *screenDirtyManager, matrix, std::nullopt);
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
        bool skipComputeIfOccluded =
            surfaceParams->GetVisibleRegion().IsEmpty() || surfaceParams->GetOccludedByFilterCache();
        if (skipComputeIfOccluded) {
            RS_OPTIONAL_TRACE_FMT("Skip filter dirty processing for %s occluded.", surfaceParams->GetName().c_str());
            continue;
        }
        elementChanged |= CheckMergeFilterDirty(damageRegion, drawRegion, *surfaceDirtyManager,
            matrix, surfaceParams->GetVisibleRegion());
    }
    return elementChanged;
}

bool RSUniFilterDirtyComputeUtil::CheckMergeFilterDirty(Occlusion::Region& damageRegion, Occlusion::Region& drawRegion,
    RSDirtyRegionManager& dirtyManager, const std::optional<Drawing::Matrix>& matrix,
    const std::optional<Occlusion::Region>& visibleRegion)
{
    auto& collector = dirtyManager.GetFilterCollector();
    bool filterCachePartialRender =
        !dirtyManager.IsCurrentFrameDirty() && RSFilterDirtyCollector::GetValidCachePartialRender();
    auto addDirtyInIntersect = [&] (FilterDirtyRegionInfo& info) {
        // case - 0. If this filter satisfied certain partial render conditions, skip it.
        if (filterCachePartialRender &&
                !info.forceDisablePartialRender_ && RSFilterDirtyCollector::GetFilterCacheValidForOcclusion(info.id_)) {
            RS_TRACE_NAME_FMT("Filter [%" PRIu64 "], partial render enabled, skip dirty expanding.", info.id_);
            return false;
        }
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
        collector.AddPureCleanFilterDirtyRegion(dirtyRegion);
        // When dirty region alignment is enabled, both damageRegion and drawRegion require alignment
        damageRegion.OrSelf(dirtyAlignEnabled_ ? alignedDirtyRegion : dirtyRegion);
        drawRegion.OrSelf(dirtyAlignEnabled_ ? alignedDirtyRegion : dirtyRegion);
        info.addToDirty_ = true;
        return true;
    };
    // return if any filter is added to damage region and draw region.
    auto& filterList = collector.GetFilterDirtyRegionInfoList(true);
    return std::find_if(filterList.begin(), filterList.end(), addDirtyInIntersect) != filterList.end();
}

void RSUniFilterDirtyComputeUtil::ResetFilterInfoStatus(DrawableV2::RSScreenRenderNodeDrawable& screenDrawable,
    std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr>& surfaceDrawables)
{
    auto resetFilterStatus = [] (FilterDirtyRegionInfo& filterInfo) { filterInfo.addToDirty_ = false; };
    if (auto screenDirtyManager = screenDrawable.GetSyncDirtyManager()) {
        auto& screenFilterList = screenDirtyManager->GetFilterCollector().GetFilterDirtyRegionInfoList(true);
        std::for_each(screenFilterList.begin(), screenFilterList.end(), resetFilterStatus);
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
    RSFilterDirtyCollector::ResetFilterCacheValidForOcclusion();
}

FilterDirtyRegionInfo RSUniFilterDirtyComputeUtil::GenerateFilterDirtyRegionInfo(
    RSRenderNode& filterNode, const std::optional<Occlusion::Region>& preDirty, bool isSurface)
{
    bool effectNodeExpandDirty =
        filterNode.IsInstanceOf<RSEffectRenderNode>() && !filterNode.FirstFrameHasEffectChildren();
    RectI filterRect = filterNode.GetOldDirtyInSurface().JoinRect(filterNode.GetFilterRegion());
    auto filterRegion = effectNodeExpandDirty ?
        GetVisibleEffectRegion(filterNode) : Occlusion::Region(Occlusion::Rect(filterRect));
    auto dirtyRegion = effectNodeExpandDirty ?
        filterRegion.Or(Occlusion::Region(Occlusion::Rect(filterNode.GetFilterRect()))) :
        Occlusion::Region(Occlusion::Rect(filterRect));
    if (filterNode.NeedDrawBehindWindow()) {
        filterRegion = Occlusion::Region(Occlusion::Rect(filterNode.GetFilterRect()));
        dirtyRegion = filterRegion;
    }
    // Subtree dirty region doesn't need to be considered for background filter.
    auto& filterProperties = filterNode.GetRenderProperties();
    FilterDirtyRegionInfo filterInfo = {
        .id_ = filterNode.GetId(),
        .intersectRegion_ = isSurface ? filterRegion : dirtyRegion,
        .filterDirty_ = isSurface ? filterRegion : dirtyRegion,
        .alignedFilterDirty_ = dirtyRegion.GetAlignedRegion(MAX_DIRTY_ALIGNMENT_SIZE),
        .belowDirty_ = preDirty.value_or(Occlusion::Region()),
        .isBackgroundFilterClean_ =
            (filterProperties.GetBackgroundFilter() || filterProperties.GetNeedDrawBehindWindow()) &&
            !filterNode.IsBackgroundInAppOrNodeSelfDirty(),
        .forceDisablePartialRender_ = filterNode.IsPixelStretchValid()
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

RectI RSUniFilterDirtyComputeUtil::GetVisibleFilterRect(const RSSurfaceRenderNode& node)
{
    auto context = node.GetContext().lock();
    RectI visibleFilterRect;
    if (!context) {
        RS_LOGE("GetVisibleFilterRect node%{public}" PRIu64 " context is nullptr", node.GetId());
        return visibleFilterRect;
    }
    const auto& nodeMap = context->GetNodeMap();
    for (auto& nodeId : node.GetVisibleFilterChild()) {
        if (auto subNode = nodeMap.GetRenderNode<RSRenderNode>(nodeId)) {
            // Skip nodes that only have ColorPickerDrawable without any real filter
            if (subNode->IsColorPickerOnlyNode()) {
                continue;
            }
            visibleFilterRect = visibleFilterRect.JoinRect(subNode->GetOldDirtyInSurface());
        }
    }
    return visibleFilterRect;
}

void RSUniDirtyComputeUtil::UpdateVirtualExpandScreenAccumulatedParams(
    RSScreenRenderParams& params, DrawableV2::RSScreenRenderNodeDrawable& screenDrawable,
    const sptr<RSScreenManager>& screenManager)
{
    // All other factors that may prevent skipping virtual expand display need to be considered
    // update accumulated dirty region
    params.SetAccumulatedDirty(params.GetAccumulatedDirty() ||
        (screenDrawable.GetSyncDirtyManager()->IsCurrentFrameDirty() || params.GetMainAndLeashSurfaceDirty()));

    // update accumulated hdr status changed
    params.SetAccumulatedHdrStatusChanged(params.GetAccumulatedHdrStatusChanged() || params.IsHDRStatusChanged());

    // update accumulated special layer status changed
    if (screenManager == nullptr) {
        return;
    }
    auto currentBlackList = screenManager->GetVirtualScreenBlackList(params.GetScreenId());
    if (currentBlackList != params.GetLastBlackList()) {
        params.SetLastBlackList(currentBlackList);
        params.SetAccumulatedSpecialLayerStatusChanged(true);
        return;
    }
    const auto& displayDrawables = params.GetDisplayDrawables();
    for (const auto& drawable : displayDrawables) {
        const auto& displayDrawable = static_cast<DrawableV2::RSLogicalDisplayRenderNodeDrawable*>(drawable.get());
        if (UNLIKELY(displayDrawable == nullptr)) {
            continue;
        }
        const auto& displayParams =
            static_cast<RSLogicalDisplayRenderParams*>(displayDrawable->GetRenderParams().get());
        if (UNLIKELY(displayParams == nullptr)) {
            continue;
        }
        if (displayParams->GetSecurityExemption() != params.GetLastSecExemption()) {
            params.SetLastSecExemption(displayParams->GetSecurityExemption());
            params.SetAccumulatedSpecialLayerStatusChanged(true);
            return;
        }
        params.SetAccumulatedSpecialLayerStatusChanged(
            displayParams->IsSpecialLayerChanged() || params.GetAccumulatedSpecialLayerStatusChanged());
    }
}

bool RSUniDirtyComputeUtil::CheckVirtualExpandScreenSkip(
    RSScreenRenderParams& params, DrawableV2::RSScreenRenderNodeDrawable& screenDrawable)
{
    // Regardless of whether the current frame is skipped, the state needs to be accumulated
    if (!RSSystemProperties::GetVirtualExpandScreenSkipEnabled()) {
        return false;
    }

    const auto& displayDrawables = params.GetDisplayDrawables();
    for (const auto& drawable : displayDrawables) {
        const auto& displayDrawable = static_cast<DrawableV2::RSLogicalDisplayRenderNodeDrawable*>(drawable.get());
        const auto& displayParams = static_cast<RSLogicalDisplayRenderParams*>(drawable->GetRenderParams().get());
        if (UNLIKELY(displayDrawable == nullptr || displayParams == nullptr)) {
            continue;
        }
        if (RSSpecialLayerUtils::GetSpecialLayerStateInSubTree(*displayParams, &params) !=
            DisplaySpecialLayerState::NO_SPECIAL_LAYER) {
            RS_TRACE_NAME("CheckVirtualExpandScreenSkip has special layer can not skip");
            return false;
        }
    }
    RS_TRACE_NAME_FMT("CheckVirtualExpandScreenSkip isAccumulatedDirty: %d, isAccumulatedHdrStatusChanged: %d",
        params.GetAccumulatedDirty(), params.GetAccumulatedHdrStatusChanged());
    return !params.GetAccumulatedDirty() && !params.GetAccumulatedHdrStatusChanged();
}

bool RSUniDirtyComputeUtil::CheckCurrentFrameHasDirtyInVirtual(
    DrawableV2::RSScreenRenderNodeDrawable& screenDrawable)
{
    auto mirrorScreenParams = static_cast<RSScreenRenderParams*>(screenDrawable.GetRenderParams().get());
    auto mainDrawable = mirrorScreenParams->GetMirrorSourceDrawable().lock();
    if (mainDrawable == nullptr) {
        RS_LOGD("CheckCurrentFrameHasDirtyInVirtual, failed to get mainDrawable!");
        return false;
    }
    auto mainScreenParams = static_cast<RSScreenRenderParams*>(mainDrawable->GetRenderParams().get());
    if (mainScreenParams == nullptr) {
        RS_LOGD("CheckCurrentFrameHasDirtyInVirtual, failed to get mirroredRenderParams!");
        return false;
    }
    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    if (screenManager == nullptr) {
        RS_LOGD("CheckCurrentFrameHasDirtyInVirtual, failed to get screen manager!");
        return false;
    }

    auto screenDirtyManager = mainDrawable->GetSyncDirtyManager();
    if (screenDirtyManager == nullptr) {
        RS_LOGD("CheckCurrentFrameHasDirtyInVirtual screenDirtyManager is nullptr");
        return false;
    }

    if (!screenDirtyManager->GetCurrentFrameDirtyRegion().IsEmpty() ||
        !screenDirtyManager->GetHwcDirtyRegion().IsEmpty()) {
        RS_TRACE_NAME("CheckCurrentFrameHasDirtyInVirtual has globaldirty or hwcRect");
        return true;
    }
    const auto& displayDrawables = mirrorScreenParams->GetDisplayDrawables();
    auto& curAllSurfaceDrawables = mainScreenParams->GetAllMainAndLeashSurfaceDrawables();
    for (const auto& drawable : displayDrawables) {
        if (drawable == nullptr) {
            continue;
        }
        const auto& displayParams = static_cast<RSLogicalDisplayRenderParams*>(drawable->GetRenderParams().get());
        if (displayParams == nullptr) {
            continue;
        }
        ScreenId screenId = displayParams->GetScreenId();
        auto curTypeBlackList = screenManager->GetVirtualScreenTypeBlackList(screenId);

        const std::map<RSSurfaceNodeType, RectI>& typeHwcRectList =
            screenDirtyManager->GetTypeHwcDirtyRegion();
        for (auto& typeHwcRect : typeHwcRectList) {
            NodeType nodeType = static_cast<NodeType>(typeHwcRect.first);
            if (curTypeBlackList.find(nodeType) == curTypeBlackList.end() &&
                !typeHwcRect.second.IsEmpty()) {
                RS_TRACE_NAME("CheckCurrentFrameHasDirtyInVirtual has typeHwcRect");
                return true;
            }
        }
        for (const auto& adapter : curAllSurfaceDrawables) {
            if (adapter == nullptr || adapter->GetNodeType() != RSRenderNodeType::SURFACE_NODE) {
                RS_LOGD("CheckCurrentFrameHasDirtyInVirtual adapter is nullptr or error type");
                continue;
            }
            auto surfaceNodeDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(adapter);
            auto surfaceDirtyManager = surfaceNodeDrawable->GetSyncDirtyManager();
            auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceNodeDrawable->GetRenderParams().get());
            if (surfaceDirtyManager == nullptr || surfaceParams == nullptr) {
                RS_LOGD("CheckCurrentFrameHasDirtyInVirtual surfaceDirtyManager or surfaceparams is nullptr");
                continue;
            }

            if (surfaceParams->GetSpecialLayerMgr().FindWithScreen(screenId, SpecialLayerType::IS_BLACK_LIST) ||
                surfaceParams->GetSpecialLayerMgr().Find(SpecialLayerType::SKIP)) {
                continue;
            }
            auto curDirtyRegion = surfaceDirtyManager->GetCurrentFrameDirtyRegion();
            if (surfaceParams->GetVisibleRegionInVirtual().IsIntersectWith(curDirtyRegion)) {
                RS_TRACE_NAME_FMT("CheckCurrentFrameHasDirtyInVirtual name: %s",
                    surfaceParams->GetName().c_str());
                return true;
            }
        }
    }
    return false;
}

void RSUniDirtyComputeUtil::AccumulateVirtualExpandScreenDirtyRegions(
    DrawableV2::RSScreenRenderNodeDrawable& screenDrawable, RSScreenRenderParams& params)
{
    auto& curAllSurfaceDrawables = params.GetAllMainAndLeashSurfaceDrawables();
    for (auto it = curAllSurfaceDrawables.rbegin(); it != curAllSurfaceDrawables.rend(); ++it) {
        auto surfaceNodeDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(*it);
        if (surfaceNodeDrawable == nullptr) {
            continue;
        }
        auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceNodeDrawable->GetRenderParams().get());
        if (surfaceParams ==nullptr || !surfaceParams->IsLeashOrMainWindow()) {
            continue;
        }
        // for cross-screen surface, only consider the dirty region on the first display (use global dirty for others).
        if (surfaceParams->IsFirstLevelCrossNode() && !params.IsFirstVisitCrossNodeDisplay()) {
            continue;
        }
        auto surfaceDirtyManager = surfaceNodeDrawable->GetSyncDirtyManager();
        if (surfaceDirtyManager == nullptr) {
            continue;
        }
        surfaceDirtyManager->AccumulateVirtualExpandScreenDirtyRegions(
            surfaceDirtyManager->GetCurrentFrameDirtyRegion());
    }
    if (auto screenDirtyManager = screenDrawable.GetSyncDirtyManager()) {
        screenDirtyManager->AccumulateVirtualExpandScreenDirtyRegions(
            screenDirtyManager->GetCurrentFrameDirtyRegion());
    }
}

void RSUniDirtyComputeUtil::MergeVirtualExpandScreenAccumulatedDirtyRegions(
    DrawableV2::RSScreenRenderNodeDrawable& screenDrawable, RSScreenRenderParams& params)
{
    if (auto screenDirtyManager = screenDrawable.GetSyncDirtyManager()) {
        if (params.GetAccumulatedSpecialLayerStatusChanged()) {
            screenDirtyManager->ResetDirtyAsSurfaceSize();
        } else {
            for (const auto& rect : screenDirtyManager->GetVirtualExpandScreenAccumulatedDirtyRegions()) {
                screenDirtyManager->MergeDirtyRect(rect);
            }
        }
    }
    auto& curAllSurfaceDrawables = params.GetAllMainAndLeashSurfaceDrawables();
    for (auto it = curAllSurfaceDrawables.rbegin(); it != curAllSurfaceDrawables.rend(); ++it) {
        auto surfaceNodeDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(*it);
        if (surfaceNodeDrawable == nullptr) {
            continue;
        }
        auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceNodeDrawable->GetRenderParams().get());
        if (surfaceParams == nullptr || !surfaceParams->IsLeashOrMainWindow()) {
            continue;
        }
        // for cross-screen surface, only consider the dirty region on the first display (use global dirty for others).
        if (surfaceParams->IsFirstLevelCrossNode() && !params.IsFirstVisitCrossNodeDisplay()) {
            continue;
        }
        auto surfaceDirtyManager = surfaceNodeDrawable->GetSyncDirtyManager();
        if (surfaceDirtyManager == nullptr) {
            continue;
        }
        for (const auto& rect : surfaceDirtyManager->GetVirtualExpandScreenAccumulatedDirtyRegions()) {
            surfaceDirtyManager->MergeDirtyRect(rect);
        }
    }
}

void RSUniDirtyComputeUtil::ClearVirtualExpandScreenAccumulatedDirtyRegions(
    DrawableV2::RSScreenRenderNodeDrawable& screenDrawable, RSScreenRenderParams& params)
{
    auto& curAllSurfaceDrawables = params.GetAllMainAndLeashSurfaceDrawables();
    for (auto it = curAllSurfaceDrawables.rbegin(); it != curAllSurfaceDrawables.rend(); ++it) {
        auto surfaceNodeDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(*it);
        if (surfaceNodeDrawable == nullptr) {
            continue;
        }
        auto surfaceDirtyManager = surfaceNodeDrawable->GetSyncDirtyManager();
        if (surfaceDirtyManager == nullptr) {
            continue;
        }
        surfaceDirtyManager->ClearVirtualExpandScreenAccumulatedDirtyRegions();
    }
    if (auto screenDirtyManager = screenDrawable.GetSyncDirtyManager()) {
        screenDirtyManager->ClearVirtualExpandScreenAccumulatedDirtyRegions();
    }
}
} // namespace Rosen
} // namespace OHOS