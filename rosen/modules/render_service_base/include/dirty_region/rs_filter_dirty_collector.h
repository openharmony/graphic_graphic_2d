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
#ifndef RENDER_SERVICE_BASE_RS_FILTER_DIRTY_COLLECTOR_H
#define RENDER_SERVICE_BASE_RS_FILTER_DIRTY_COLLECTOR_H

#include <list>

#include "common/rs_common_def.h"
#include "common/rs_rect.h"
#include "common/rs_occlusion_region.h"

namespace OHOS {
namespace Rosen {
/*
 * Whether an area of filter should be included into dirty region, and its cache validity depends on:
 * - The filter is inside/outside of a surface.
 * - Surface transparency.
 * These enum values distinguish the different cases.
 */
typedef enum : uint8_t {
    OPAQUE_SURFACE_FILTER       = 0,
    TRANSPARENT_SURFACE_FILTER  = 1,
    CONTAINER_FILTER            = 2,
    MAX_FILTER_DIRTY_TYPE
} FilterDirtyType;

/**
 * This structure shows the basic unit for processing filter dirty regions, including:
 * - NodeId
 * - Intersection test region (this may differ from the actual node bounds, e.g. effect node)
 * - Region to be added to the dirty region
 * - Aligned dirty region (when alignment is enabled, the filter region should also be aligned)
 * - Pre-dirty region, when this filter collected by the main thread when this blur was created
 */
struct FilterDirtyRegionInfo {
    NodeId id_ = INVALID_NODEID;
    Occlusion::Region intersectRegion_ = Occlusion::Region();
    Occlusion::Region filterDirty_ = Occlusion::Region();
    Occlusion::Region alignedFilterDirty_ = Occlusion::Region();
    Occlusion::Region belowDirty_ = Occlusion::Region();
    bool isBackgroundFilterClean_ = false;
    bool addToDirty_ = false;

    FilterDirtyRegionInfo() = default;
    FilterDirtyRegionInfo(const FilterDirtyRegionInfo& info) = default;
    FilterDirtyRegionInfo& operator=(const FilterDirtyRegionInfo& info) = default;
};

typedef std::list<FilterDirtyRegionInfo> FilterDirtyRegionInfoList;

/**
* this class is closed-loop within the dirty region feature.
* For other features, DO NOT modify it.
* This class can only be obtained by reference and copy assignment is forbidden.
 */
class RSB_EXPORT RSFilterDirtyCollector {
public:
    RSFilterDirtyCollector& operator=(const RSFilterDirtyCollector& collector) = delete;
    void CollectFilterDirtyRegionInfo(const FilterDirtyRegionInfo& filterInfo, bool syncToRT);
    FilterDirtyRegionInfoList& GetFilterDirtyRegionInfoList(bool syncToRT);
    void OnSync(RSFilterDirtyCollector& target) const;
    void Clear();
    void AddPureCleanFilterDirtyRegion(const Occlusion::Region& region)
    {
        pureCleanFilterDirtyRegion_.OrSelf(region);
    }
    const Occlusion::Region& GetPureCleanFilterDirtyRegion() const
    {
        return pureCleanFilterDirtyRegion_;
    }
    void ClearPureCleanFilterDirtyRegion()
    {
        pureCleanFilterDirtyRegion_.Reset();
    }
    static bool GetValidCachePartialRender()
    {
        return enablePartialRender_;
    }
    static void SetValidCachePartialRender(bool enable)
    {
        enablePartialRender_ = enable;
    }
    static void RecordFilterCacheValidForOcclusion(NodeId id, bool isValid)
    {
        if (!isValid) {
            return;
        }
        validOcclusionFilterCache_.insert(id);
    }
    static bool GetFilterCacheValidForOcclusion(NodeId id)
    {
        return validOcclusionFilterCache_.find(id) != validOcclusionFilterCache_.end();
    }
    static void ResetFilterCacheValidForOcclusion()
    {
        validOcclusionFilterCache_.clear();
    }
private:
    // Main thread filters affected by below dirty (may invalidate cache).
    FilterDirtyRegionInfoList filtersWithBelowDirty_;
    // RT thread filters requiring full merge on damage intersection.
    FilterDirtyRegionInfoList pureCleanFilters_;

    //Blur that does not affect the display but intersects with dirty region
    //Only used to increase the dirty region of the current frame
    Occlusion::Region pureCleanFilterDirtyRegion_;

    // if filter cache valid for occlusion, dirty region collection can be skipped.
    static std::unordered_set<NodeId> validOcclusionFilterCache_;
    static bool enablePartialRender_;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_BASE_RS_FILTER_DIRTY_COLLECTOR_H
