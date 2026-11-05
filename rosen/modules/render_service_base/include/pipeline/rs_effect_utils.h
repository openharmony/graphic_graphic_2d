/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_BASE_CORE_PIPELINE_RS_EFFECT_UTILS_H
#define RENDER_SERVICE_BASE_CORE_PIPELINE_RS_EFFECT_UTILS_H

#include "effect/rs_render_effect_common_def.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "property/rs_properties.h"

namespace OHOS {

namespace Rosen {
class RSB_EXPORT RSEffectUtils {
public:
    static bool AccumulateFilterRenderContext(RSRenderNode& node,
        const RSRenderNode& rootNode, FilterRenderContext& context);

    // true：causes filter cache to be generated offscreen.
    static bool IsOffscreenForFilterCache(RSRenderNode& node);

    // Check if filter node check can be skipped in occluded sub tree.
    // Returns true when:
    // 1. curSurfaceNode exists (current surface is being processed)
    // 2. curSurfaceNode is opaque, no need to consider dirty regions of surface nodes below it
    // 3. Current frame dirty region is empty (no content needs update)
    // In this case, the sub tree is fully occluded and clean, so filter check can be skipped.
    static bool ShouldSkipFilterNodeCheckInOccludedSubTree(
        const std::shared_ptr<RSSurfaceRenderNode>& curSurfaceNode, const RSRenderNode& rootNode,
        RSDirtyRegionManager& dirtyManager);

    // Update filter cache with current frame dirty region and pending purge filter dirty rect.
    // This function processes both material filter and background filter:
    // 1. Updates filter cache with current frame dirty region
    // 2. Updates pending purge filter dirty rect for cache management
    static void UpdateFilterCacheWithBelowDirtyAndPendingPurge(RSRenderNode& node,
        RSDirtyRegionManager& dirtyManager);

    // true: has filter that only depends on background
    static bool HasBackgroundDependentFilter(const RSProperties& properties);

    // Check if the current snapshot region is still inside the last snapshot region.
    // If the current region is no longer inside the last one (or the region changed
    // while needing to draw behind window), the filter cache is invalidated by
    // marking the filter status as changed. Used for background filter cache validation.
    static void CheckRegionInsideForFilterCache(RSRenderNode& node,
        std::shared_ptr<DrawableV2::RSFilterDrawable> filterDrawable);

    // Check if the snapshot or draw region has changed compared to the last frame.
    // For FILTERED_SNAPSHOT cache type, the draw region is compared; otherwise the
    // snapshot region is compared. If the region changed, the filter cache is invalidated.
    // isForeground distinguishes foreground (compositing) filters from background
    // (material) filters when marking the status change.
    static void CheckRegionChangeForFilterCache(RSRenderNode& node,
        std::shared_ptr<DrawableV2::RSFilterDrawable> filterDrawable, bool isForeground);

    // Called once per frame during sync phase (RSDrawFrame::Sync).
    // Sets the current vsync start time for RSFilterCacheManager to tag
    // newly generated cache data.
    static void Sync(int64_t vsyncStartTime);

    // HVE precondition: check whether the renderNode carries a filter/effect
    // requiring HVE processing. Sole caller is HveFilter::CheckPrecondition.
    static bool HasValidFilterOrEffect(const RSRenderNode& renderNode);

private:
    // Walk up to the enclosing EFFECT_NODE and verify it has a valid background filter.
    static bool HasValidEffectNode(const std::shared_ptr<RSRenderNode>& node);
    // Walk the parent chain for a valid UseEffect node, then validate via HasValidEffectNode.
    static bool HasValidEffect(const RSRenderNode* node);
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_BASE_CORE_PIPELINE_RS_EFFECT_UTILS_H
