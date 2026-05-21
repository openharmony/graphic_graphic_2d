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
#include "params/rs_screen_render_params.h"
#include "common/rs_occlusion_region.h"

namespace OHOS {
namespace Rosen {
class RSDirtyRectsDfx;
class RSUniDirtyComputeUtil {
public:
    // Dirty region count threshold:
    // - Use clipping rect when region count <= this threshold (1 dirty region)
    // - Switch to GPU tile alignment for better performance when count exceeds threshold
    static inline constexpr size_t DIRTY_REGION_COUNT_THRESHOLD{1};

    static std::vector<RectI> GetCurrentFrameVisibleDirty(DrawableV2::RSScreenRenderNodeDrawable& screenDrawable,
        ScreenInfo& screenInfo, RSScreenRenderParams& params);
    static std::vector<RectI> ScreenIntersectDirtyRects(const Occlusion::Region &region, const ScreenInfo& screenInfo);
    static std::vector<RectI> GetFilpDirtyRects(const std::vector<RectI>& srcRects, const ScreenInfo& screenInfo);
    static std::vector<RectI> FilpRects(const std::vector<RectI>& srcRects, const ScreenInfo& screenInfo);
    static GraphicIRect IntersectRect(const GraphicIRect& first, const GraphicIRect& second);
    static void UpdateVirtualExpandScreenAccumulatedParams(RSScreenRenderParams& params, bool isCurrentFrameDirty);
    static bool CheckVirtualExpandScreenSkip(
        RSScreenRenderParams& params, DrawableV2::RSScreenRenderNodeDrawable& screenDrawable);
    static bool CheckCurrentFrameHasDirtyInVirtual(DrawableV2::RSScreenRenderNodeDrawable& screenDrawable);
    static void AccumulateVirtualExpandScreenDirtyRegions(
        DrawableV2::RSScreenRenderNodeDrawable& screenDrawable, RSScreenRenderParams& params);
    static void MergeVirtualExpandScreenAccumulatedDirtyRegions(
        DrawableV2::RSScreenRenderNodeDrawable& screenDrawable, RSScreenRenderParams& params);
    static void ClearVirtualExpandScreenAccumulatedDirtyRegions(
        DrawableV2::RSScreenRenderNodeDrawable& screenDrawable, RSScreenRenderParams& params);
    static bool HasMirrorDisplay();
    static Drawing::Region GetFlippedRegion(const std::vector<RectI>& rects, ScreenInfo& screenInfo);
    static void ClipRegion(Drawing::Canvas& canvas, Drawing::Region& region, bool clear = true);

    static bool IsDamageRegionGpuTileInited() noexcept
    {
        return damageRegionGpuTileInited_;
    }

    static bool IsDamageRegionGpuTileValid() noexcept
    {
        return damageRegionGpuTile_.first > 0 && damageRegionGpuTile_.second > 0;
    }

    static void SetDamageRegionGpuTile(std::pair<int, int> damageRegionGpuTile) noexcept
    {
        damageRegionGpuTile_ = damageRegionGpuTile;
        damageRegionGpuTileInited_ = true;
    }

    static const std::pair<int, int>& GetDamageRegionGpuTile() noexcept
    {
        return damageRegionGpuTile_;
    }

    static RSVisibleLevel GetRegionVisibleLevel(const Occlusion::Region& selfDrawRegion,
        const Occlusion::Region& visibleRegion);
private:
    static bool damageRegionGpuTileInited_;
    static std::pair<int, int> damageRegionGpuTile_;
};

class DirtyStatusAutoUpdate {
public:
    DirtyStatusAutoUpdate(RSRenderThreadParams& uniParam, bool isSingleLogicalDisplay) : uniParam_(uniParam)
    {
        isVirtualDirtyEnabled_ = uniParam_.IsVirtualDirtyEnabled();
        isVirtualExpandScreenDirtyEnabled_ = uniParam_.IsVirtualExpandScreenDirtyEnabled();
        uniParam_.SetVirtualDirtyEnabled(isVirtualDirtyEnabled_ && isSingleLogicalDisplay);
        uniParam_.SetVirtualExpandScreenDirtyEnabled(isVirtualExpandScreenDirtyEnabled_ && isSingleLogicalDisplay);
    }
    ~DirtyStatusAutoUpdate()
    {
        uniParam_.SetVirtualDirtyEnabled(isVirtualDirtyEnabled_);
        uniParam_.SetVirtualExpandScreenDirtyEnabled(isVirtualExpandScreenDirtyEnabled_);
    }
private:
    RSRenderThreadParams& uniParam_;
    bool isVirtualDirtyEnabled_;
    bool isVirtualExpandScreenDirtyEnabled_;
};

class RSUniFilterDirtyComputeUtil {
public:
    static FilterDirtyRegionInfo GenerateFilterDirtyRegionInfo(
        RSRenderNode& filterNode, const std::optional<Occlusion::Region>& preDirty, bool isSurface);
    // Entry for filter dirty region process
    static void DealWithFilterDirtyRegion(Occlusion::Region& damageRegion, Occlusion::Region& drawRegion,
        DrawableV2::RSScreenRenderNodeDrawable& screenDrawable, const std::optional<Drawing::Matrix>& matrix,
        bool dirtyAlign = false);
    static RectI GetVisibleFilterRect(const RSRenderNode& node);
private:
    static bool DealWithFilterDirtyForScreen(Occlusion::Region& damageRegion, Occlusion::Region& drawRegion,
        DrawableV2::RSScreenRenderNodeDrawable& screenDrawable, const std::optional<Drawing::Matrix>& matrix);

    static bool DealWithFilterDirtyForSurface(Occlusion::Region& damageRegion, Occlusion::Region& drawRegion,
        std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr>& drawables,
        const std::optional<Drawing::Matrix>& matrix);
    // basic check point
    static bool CheckMergeFilterDirty(Occlusion::Region& damageRegion, Occlusion::Region& drawRegion,
        RSDirtyRegionManager& dirtyManager, const std::optional<Drawing::Matrix>& matrix,
        const std::optional<Occlusion::Region>& visibleRegion);

    static void ResetFilterInfoStatus(DrawableV2::RSScreenRenderNodeDrawable& screenDrawable,
        std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr>& surfaceDrawables);
    
    static bool FilterCachePartialRenderEnabled(const FilterDirtyRegionInfo& info);

    static Occlusion::Region GetVisibleEffectRegion(RSRenderNode& filterNode);
    inline static bool dirtyAlignEnabled_ = false;
};
}
}
#endif // RENDER_SERVICE_CORE_FEATURE_RS_UNI_DIRTY_COMPUTE_UTIL_H