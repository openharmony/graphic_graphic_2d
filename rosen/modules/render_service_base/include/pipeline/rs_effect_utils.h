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
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_BASE_CORE_PIPELINE_RS_EFFECT_UTILS_H
