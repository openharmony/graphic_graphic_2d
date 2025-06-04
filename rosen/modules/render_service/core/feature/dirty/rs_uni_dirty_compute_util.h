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

#ifndef RENDER_SERVICE_CORE_FEATURE_RS_UNI_DIRTY_COMPUTE_UTIL_H
#define RENDER_SERVICE_CORE_FEATURE_RS_UNI_DIRTY_COMPUTE_UTIL_H

#include <list>
#include <mutex>
#include <set>
#include <unordered_map>

#include "common/rs_obj_abs_geometry.h"
#include "drawable/rs_surface_render_node_drawable.h"
#include "params/rs_display_render_params.h"
#include "common/rs_occlusion_region.h"

namespace OHOS {
namespace Rosen {
class RSDirtyRectsDfx;
class RSUniDirtyComputeUtil {
public:
    static std::vector<RectI> GetCurrentFrameVisibleDirty(DrawableV2::RSDisplayRenderNodeDrawable& displayDrawable,
        ScreenInfo& screenInfo, RSDisplayRenderParams& params);
    static std::vector<RectI> ScreenIntersectDirtyRects(const Occlusion::Region &region, ScreenInfo& screenInfo);
    static std::vector<RectI> GetFilpDirtyRects(const std::vector<RectI>& srcRects, const ScreenInfo& screenInfo);
    static std::vector<RectI> FilpRects(const std::vector<RectI>& srcRects, const ScreenInfo& screenInfo);
    static GraphicIRect IntersectRect(const GraphicIRect& first, const GraphicIRect& second);
};

class RSUniFilterDirtyComputeUtil {
public:
    static FilterDirtyRegionInfo GenerateFilterDirtyRegionInfo(
        RSRenderNode& filterNode, const std::optional<Occlusion::Region>& preDirty);
    // Entry for filter dirty region process
    static void DealWithFilterDirtyRegion(Occlusion::Region& damageRegion, Occlusion::Region& drawRegion,
        DrawableV2::RSDisplayRenderNodeDrawable& displayDrawable, const std::optional<Drawing::Matrix>& matrix,
        bool dirtyAlign = false);
private:
    static bool DealWithFilterDirtyForDisplay(Occlusion::Region& damageRegion, Occlusion::Region& drawRegion,
        DrawableV2::RSDisplayRenderNodeDrawable& displayDrawable, const std::optional<Drawing::Matrix>& matrix);

    static bool DealWithFilterDirtyForSurface(Occlusion::Region& damageRegion, Occlusion::Region& drawRegion,
        std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr>& drawables,
        const std::optional<Drawing::Matrix>& matrix);
    // basic check point
    static bool CheckMergeFilterDirty(Occlusion::Region& damageRegion, Occlusion::Region& drawRegion,
        FilterDirtyRegionInfoList& filterList, const std::optional<Drawing::Matrix>& matrix,
        const std::optional<Occlusion::Region>& visibleRegion);

    static void ResetFilterInfoStatus(DrawableV2::RSDisplayRenderNodeDrawable& displayDrawable,
        std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr>& surfaceDrawables);

    static Occlusion::Region GetVisibleEffectRegion(RSRenderNode& filterNode);
    inline static bool dirtyAlignEnabled_ = false;
};
}
}
#endif // RENDER_SERVICE_CORE_FEATURE_RS_UNI_DIRTY_COMPUTE_UTIL_H