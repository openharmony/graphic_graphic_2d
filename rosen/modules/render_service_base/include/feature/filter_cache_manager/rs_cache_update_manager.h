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

#ifndef RENDER_SERVICE_BASE_RENDER_RS_CACHE_UPDATE_MANAGER_H
#define RENDER_SERVICE_BASE_RENDER_RS_CACHE_UPDATE_MANAGER_H
#include <atomic>
#include <condition_variable>
#include "common/rs_macros.h"
#include "common/rs_rect.h"
#include "pipeline/rs_dirty_region_manager.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "render/rs_filter.h"
#include "utils/rect.h"
namespace OHOS {
namespace Rosen {
class RSDrawingFilter;
class RSHpaeFilterCacheManager;

class RSB_EXPORT RSCacheUpdateManager final {
    friend class RSFilterCacheManager;
public:
    RSCacheUpdateManager();
    ~RSCacheUpdateManager();
    RSCacheUpdateManager(const RSCacheUpdateManager&) = delete;
    RSCacheUpdateManager(const RSCacheUpdateManager&&) = delete;
    RSCacheUpdateManager& operator=(const RSCacheUpdateManager&) = delete;
    RSCacheUpdateManager& operator=(const RSCacheUpdateManager&&) = delete;

    enum CacheType : uint8_t {
        CACHE_TYPE_NONE = 0,
        CACHE_TYPE_SNAPSHOT = 1,
        CACHE_TYPE_FILTERED_SNAPSHOT = 2,
        CACHE_TYPE_BOTH = CACHE_TYPE_SNAPSHOT | CACHE_TYPE_FILTERED_SNAPSHOT,
    };

    bool IsForceUseFilterCache() const;
    void MarkFilterForceUseCache(bool forceUseCache = true);
    bool IsForceClearFilterCache() const;
    void MarkFilterForceClearCache();
    void RecordFilterInfos(const std::shared_ptr<RSFilter>& rsFilter);
    void MarkFilterRegionChanged();
    void MarkFilterRegionInteractWithDirty();
    void MarkForceClearCacheWithLastFrame();
    void MarkFilterRegionIsLargeArea();
    bool CheckAndUpdateAIBarCacheStatus(bool intersectHwcDamage);
    void MarkEffectNode();
    void MarkNeedClearFilterCache(NodeId nodeId);
    bool NeedPendingPurge() const;
    bool IsSkippingFrame() const;
    void MarkRotationChanged();
    void MarkNodeIsOccluded(bool isOcclude);
    bool WouldDrawLargeAreaBlur();
    bool WouldDrawLargeAreaBlurPrecisely();

    // Return true if the interval is successfully reduced, return false otherwise.
    bool ReduceCacheUpdateInterval();
    bool ReduceCacheUpdateIntervalIfPendingPurge();

    void OnSyncStagingParams(const std::shared_ptr<RSCacheUpdateManager>& updateCacheManager);

    RSFilter::FilterType GetFilterType() const
    {
        return filterType_;
    }

    void MarkInForegroundFilterAndCheckNeedForceClearCache(NodeId offscreenCanvasNodeId);
    bool IsFilterCacheValid() const;
    void MarkDebugEnabled()
    {
        debugEnabled_ = true;
    }
    inline int32_t GetNodeFilterApsRate() const
    {
        return nodeFilterApsRate_;
    }
    inline void SetNodeFilterApsRate(int32_t nodeApsRate)
    {
        nodeFilterApsRate_ = nodeApsRate;
    }

    inline int32_t GetNodeFilterAppRate() const
    {
        return nodeFilterAppRate_;
    }
    inline void SetNodeFilterAppRate(int32_t nodeAppRate)
    {
        nodeFilterAppRate_ = nodeAppRate;
    }

    static void SetCurrentVsyncRefreshRate(uint32_t rate);
    static uint32_t GetCurrentVsyncRefreshRate();

    int CalApsCacheUpdateInterval(int cacheUpdateInterval_tmp, int32_t nodeTargetRate);

private:
    void UpdateFlags(FilterCacheType type, bool cacheValid);

    // flags for clearing filter cache
    NodeId offscreenNodeId_ = INVALID_NODEID;

    // All stagingXXX variables should be read & written by render_service thread
    bool stagingForceUseCache_ = false;
    bool stagingForceClearCache_ = false;
    uint32_t stagingCachedFilterHash_ = 0;
    bool stagingFilterHashChanged_ = false;
    bool stagingFilterRegionChanged_ = false;
    bool stagingFilterInteractWithDirty_ = false;
    bool stagingRotationChanged_ = false;
    bool stagingForceClearCacheForLastFrame_ = false;
    bool stagingIsAIBarInteractWithHWC_ = false;
    bool stagingIsEffectNode_ = false;

    // the type cache needed clear before drawing
    FilterCacheType stagingClearType_ = FilterCacheType::NONE;
    FilterCacheType lastCacheType_ = FilterCacheType::NONE;
    bool stagingIsOccluded_ = false;

    // force cache with cacheUpdateInterval_
    bool stagingIsLargeArea_ = false;
    bool canSkipFrame_ = false;
    bool stagingIsSkipFrame_ = false;
    RSFilter::FilterType filterType_ = RSFilter::NONE;
    bool forceUseCache_ = false;
    bool belowDirty_ = false;

    // Cache age, used to determine if we can delay the cache update.
    int cacheUpdateInterval_ = 0;
    bool isFilterCacheValid_ = false;   // catch status in current frame
    // whether we need to purge the cache after this frame.
    bool pendingPurge_ = false;

    // last offscreenNodeId_ value
    NodeId lastOffscreenNodeId_ = INVALID_NODEID;
    bool lastStagingFilterInteractWithDirty_ = false;
    bool debugEnabled_ = false;

    int nodeFilterApsRate_ = INVALID_FILTER_RATE;
    int nodeFilterAppRate_ = INVALID_FILTER_RATE;
    int nodeFilterRate_ = INVALID_FILTER_RATE;

    static std::atomic<uint32_t> currentVsyncRefreshRate_;
};
}   // namespace Rosen
}   // namespace OHOS

#endif  // RENDER_SERVICE_BASE_RENDER_RS_CACHE_UPDATE_MANAGER_H
