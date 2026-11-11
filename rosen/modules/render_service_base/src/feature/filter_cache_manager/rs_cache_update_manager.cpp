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

#include "feature/filter_cache_manager/rs_filter_cache_manager.h"
#include "feature/filter_cache_manager/rs_cache_update_manager.h"
#include "feature/filter_cache_manager/rs_filter_cache_memory_controller.h"
#include "rs_trace.h"
#include "common/rs_occlusion_region.h"
#include "render/rs_filter.h"
#include "rs_profiler.h"

#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
#include "hpae_base/rs_hpae_base_data.h"
#include "hpae_base/rs_hpae_filter_cache_manager.h"
#include "src/image/SkImage_Base.h"

#include "common/rs_optional_trace.h"
#include "draw/canvas.h"
#include "draw/surface.h"
#include "drawable/rs_property_drawable_utils.h"
#include "effect/rs_render_filter_base.h"
#include "memory/rs_tag_tracker.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "render/rs_drawing_filter.h"
#include "render/rs_high_performance_visual_engine.h"

#ifdef USE_M133_SKIA
#include "include/gpu/ganesh/GrBackendSurface.h"
#else
#include "include/gpu/GrBackendSurface.h"
#endif

namespace OHOS {
namespace Rosen {

constexpr int AIBAR_CACHE_UPDATE_INTERVAL = 6;
constexpr int ROTATION_CACHE_UPDATE_INTERVAL = 1;
std::atomic<uint32_t> RSCacheUpdateManager::currentVsyncRefreshRate_ = 0;

RSCacheUpdateManager::RSCacheUpdateManager() {}
RSCacheUpdateManager::~RSCacheUpdateManager() {}

bool RSCacheUpdateManager::IsForceUseFilterCache() const
{
    return stagingForceUseCache_;
}

void RSCacheUpdateManager::MarkFilterForceUseCache(bool forceUseCache)
{
    stagingForceUseCache_ = forceUseCache;
}

bool RSCacheUpdateManager::IsForceClearFilterCache() const
{
    return stagingForceClearCache_;
}

void RSCacheUpdateManager::MarkFilterForceClearCache()
{
    stagingForceClearCache_ = true;
}

void RSCacheUpdateManager::RecordFilterInfos(const std::shared_ptr<RSFilter>& rsFilter)
{
    auto filter = std::static_pointer_cast<RSDrawingFilter>(rsFilter);
    if (filter == nullptr) {
        return;
    }
    stagingFilterHashChanged_ = stagingCachedFilterHash_ != filter->Hash();
    if (stagingFilterHashChanged_) {
        stagingCachedFilterHash_ = filter->Hash();
    }
    filterType_ = filter->GetFilterType();
    canSkipFrame_ = filter->CanSkipFrame();
}

void RSCacheUpdateManager::MarkFilterRegionChanged()
{
    stagingFilterRegionChanged_ = true;
}

void RSCacheUpdateManager::MarkFilterRegionInteractWithDirty()
{
    stagingFilterInteractWithDirty_ = true;
}

void RSCacheUpdateManager::MarkForceClearCacheWithLastFrame()
{
    stagingForceClearCacheForLastFrame_ = true;
}

void RSCacheUpdateManager::MarkFilterRegionIsLargeArea()
{
    stagingIsLargeArea_ = true;
}

void RSCacheUpdateManager::MarkInForegroundFilterAndCheckNeedForceClearCache(NodeId offscreenCanvasNodeId)
{
    offscreenNodeId_ = offscreenCanvasNodeId;
    if (offscreenNodeId_ != lastOffscreenNodeId_ && lastCacheType_ != FilterCacheType::NONE) {
        MarkFilterForceClearCache();
    }
}

void RSCacheUpdateManager::SetCurrentVsyncRefreshRate(uint32_t rate)
{
    currentVsyncRefreshRate_.store(rate, std::memory_order_relaxed);
}

uint32_t RSCacheUpdateManager::GetCurrentVsyncRefreshRate()
{
    return currentVsyncRefreshRate_.load(std::memory_order_relaxed);
}

void RSCacheUpdateManager::UpdateFlags(FilterCacheType type, bool cacheValid)
{
    stagingClearType_ = type;
    isFilterCacheValid_ = cacheValid;
    int cacheUpdateInterval_tmp = RSSystemProperties::GetFilterCacheUpdateInterval();
    // blockade gradient blur when aps skip-frame
    bool isGradientBlur = filterType_ == RSFilter::LINEAR_GRADIENT_BLUR;
    cacheUpdateInterval_tmp = (nodeFilterRate_ > 0 && !isGradientBlur) ?
        CalApsCacheUpdateInterval(cacheUpdateInterval_tmp, nodeFilterRate_) : cacheUpdateInterval_tmp;
    if (!cacheValid) {
        cacheUpdateInterval_ = stagingRotationChanged_ ? ROTATION_CACHE_UPDATE_INTERVAL : 
            (filterType_ == RSFilter::AIBAR ? AIBAR_CACHE_UPDATE_INTERVAL : (((stagingIsLargeArea_ && canSkipFrame_) 
                || (nodeFilterRate_ > 0 && !isGradientBlur)) ? cacheUpdateInterval_tmp : 0));
        pendingPurge_ = false;
        return;
    }
    if (stagingIsAIBarInteractWithHWC_ || stagingFilterInteractWithDirty_ || stagingRotationChanged_ ||
        (nodeFilterRate_ > 0 && !isGradientBlur)) {
        ReduceCacheUpdateInterval();
    }
    stagingIsAIBarInteractWithHWC_ = false;
    RS_TRACE_NAME_FMT(
        "UpdateFlags::cacheUpdateInterval_tmp:%d, cacheUpdateInterval_:%d, nodeFilterRate_:%d, cacheValid:%d",
        cacheUpdateInterval_tmp, cacheUpdateInterval_, nodeFilterRate_, cacheValid);
}

int RSCacheUpdateManager::CalApsCacheUpdateInterval(int cacheUpdateInterval_tmp, int32_t nodeTargetRate)
{
    int32_t realtimeRate = RSCacheUpdateManager::GetCurrentVsyncRefreshRate();
    RS_TRACE_NAME_FMT("CalApsCacheUpdateInterval realtimeRate:%d", realtimeRate);
    if (realtimeRate < nodeTargetRate) {
        cacheUpdateInterval_tmp = 0;
    } else {
        cacheUpdateInterval_tmp = (realtimeRate + nodeTargetRate - 1)/nodeTargetRate - 1;
    }
    return cacheUpdateInterval_tmp;
}

bool RSCacheUpdateManager::ReduceCacheUpdateInterval()
{
    if (cacheUpdateInterval_ <= 0) {
        return false;
    }

    RS_TRACE_NAME_FMT("ReduceCacheUpdateInterval cacheUpdateInterval_:%d", cacheUpdateInterval_);
    cacheUpdateInterval_--;
    pendingPurge_ = true;
    return true;
}

bool RSCacheUpdateManager::ReduceCacheUpdateIntervalIfPendingPurge()
{
    if (cacheUpdateInterval_ <= 0) {
        return false;
    }

    if (pendingPurge_) {
        cacheUpdateInterval_--;
    }
    return true;
}

bool RSCacheUpdateManager::CheckAndUpdateAIBarCacheStatus(bool intersectHwcDamage)
{
    if (filterType_ != RSFilter::AIBAR) {
        return false;
    }
    // should mark stagingIsAIBarInteractWithHWC_ once intersectHwcDamage is true
    // or pendingPurge_ is true (which means AIBar has been intersected with damage during cache consuming)
    if (intersectHwcDamage || pendingPurge_) {
        stagingIsAIBarInteractWithHWC_ = true;
    }
    RS_OPTIONAL_TRACE_NAME_FMT("RSFilterCacheManager::CheckAndUpdateAIBarCacheStatus \
        cacheUpdateInterval_:%d forceClearCacheForLastFrame_:%d",
        cacheUpdateInterval_, stagingForceClearCacheForLastFrame_);
    if (cacheUpdateInterval_ == 0 || stagingForceClearCacheForLastFrame_) {
        return false;
    } else {
        MarkFilterForceUseCache(true);
        return true;
    }
}

void RSCacheUpdateManager::MarkEffectNode()
{
    stagingIsEffectNode_  = true;
}

bool RSCacheUpdateManager::IsFilterCacheValid() const
{
    return isFilterCacheValid_;
}

void RSCacheUpdateManager::OnSyncStagingParams(const std::shared_ptr<RSCacheUpdateManager>& updateCacheManager)
{
    if (updateCacheManager == nullptr) {
        return;
    }

    // renderParams to stagingParams
    lastCacheType_ = updateCacheManager->lastCacheType_;

    // save staging param value
    lastOffscreenNodeId_ = offscreenNodeId_;
    lastStagingFilterInteractWithDirty_ = stagingFilterInteractWithDirty_;

    // stagingParams init
    stagingFilterHashChanged_ = false;
    stagingForceClearCacheForLastFrame_ = false;
    stagingIsEffectNode_ = false;
    stagingIsSkipFrame_ = false;
    stagingClearType_ = FilterCacheType::BOTH;

    stagingFilterRegionChanged_ = false;
    stagingFilterInteractWithDirty_ = false;
    stagingRotationChanged_ = false;
    stagingForceClearCache_ = false;
    stagingForceUseCache_ = false;
    stagingIsOccluded_ = false;

    stagingIsLargeArea_ = false;
    isFilterCacheValid_ = false;

    debugEnabled_ = false;
}

void RSCacheUpdateManager::MarkNeedClearFilterCache(NodeId nodeId)
{
    RS_TRACE_NAME_FMT("RSCacheUpdateManager::MarkNeedClearFilterCache nodeId[%llu] forceUseCache_:%d,"
        "forceClearCache_:%d, hashChanged:%d, regionChanged_:%d, belowDirty_:%d,"
        "lastCacheType:%d, cacheUpdateInterval_:%d, canSkip:%d, isLargeArea:%d, filterType_:%d, pendingPurge_:%d,"
        "forceClearCacheWithLastFrame:%d, rotationChanged:%d, offscreenNodeId:%llu", nodeId,
        stagingForceUseCache_, stagingForceClearCache_, stagingFilterHashChanged_,
        stagingFilterRegionChanged_, stagingFilterInteractWithDirty_,
        lastCacheType_, cacheUpdateInterval_, canSkipFrame_, stagingIsLargeArea_,
        filterType_, pendingPurge_, stagingForceClearCacheForLastFrame_, stagingRotationChanged_,
        offscreenNodeId_);

    ROSEN_LOGD("RSCacheUpdateManager::MarkNeedClearFilterCache, forceUseCache_:%{public}d,"
        "forceClearCache_:%{public}d, hashChanged:%{public}d, regionChanged_:%{public}d, belowDirty_:%{public}d,"
        "lastCacheType:%{public}hhu, cacheUpdateInterval_:%{public}d, canSkip:%{public}d, isLargeArea:%{public}d,"
        "filterType_:%{public}d, pendingPurge_:%{public}d,"
        "forceClearCacheWithLastFrame:%{public}d, rotationChanged:%{public}d,"
        "offscreenNodeId:%{public}" PRIu64,
        stagingForceUseCache_, stagingForceClearCache_,
        stagingFilterHashChanged_, stagingFilterRegionChanged_, stagingFilterInteractWithDirty_,
        lastCacheType_, cacheUpdateInterval_, canSkipFrame_, stagingIsLargeArea_,
        filterType_, pendingPurge_, stagingForceClearCacheForLastFrame_, stagingRotationChanged_,
        offscreenNodeId_);

    nodeFilterRate_ = nodeFilterAppRate_ != INVALID_FILTER_RATE ? nodeFilterAppRate_ : nodeFilterApsRate_;

    // if do not request NextVsync, close skip
    if (stagingForceClearCacheForLastFrame_) {
        if (nodeFilterRate_ <= 0) {
            cacheUpdateInterval_ = 0;
        }
    }

    stagingIsSkipFrame_ = stagingIsLargeArea_ && canSkipFrame_ && !stagingFilterRegionChanged_;

    // no valid cache
    if (lastCacheType_ == FilterCacheType::NONE) {
        UpdateFlags(FilterCacheType::BOTH, false);
        return;
    }
    // No need to invalidate cache if background image is not null or freezed
    if (stagingForceUseCache_) {
        UpdateFlags(FilterCacheType::NONE, true);
        return;
    }

    // clear both two type cache: 1. force clear 2. filter region changed 3.skip-frame finished
    // 4. background changed and effectNode rotated will enable skip-frame, the last frame need to update.
    if (stagingForceClearCache_ || (stagingFilterRegionChanged_ && !stagingRotationChanged_) || NeedPendingPurge() ||
        ((stagingFilterInteractWithDirty_ || stagingRotationChanged_) && cacheUpdateInterval_ <= 0)) {
        UpdateFlags(FilterCacheType::BOTH, false);
        return;
    }

    // clear snapshot cache last frame and clear filtered cache current frame
    if (lastCacheType_ == FilterCacheType::FILTERED_SNAPSHOT && stagingFilterHashChanged_) {
        UpdateFlags(FilterCacheType::FILTERED_SNAPSHOT, false);
        return;
    }

    // when blur filter changes, we need to clear filtered cache if it valid.
    UpdateFlags(stagingFilterHashChanged_ ?
        FilterCacheType::FILTERED_SNAPSHOT : FilterCacheType::NONE, true);
}

bool RSCacheUpdateManager::NeedPendingPurge() const
{
    if (nodeFilterRate_ > 0) {
        return false;
    }
    return !stagingFilterInteractWithDirty_ && pendingPurge_;
}

bool RSCacheUpdateManager::IsSkippingFrame() const
{
    return (stagingFilterInteractWithDirty_ || stagingRotationChanged_) && cacheUpdateInterval_ > 0;
}

void RSCacheUpdateManager::MarkRotationChanged()
{
    stagingRotationChanged_ = true;
}

void RSCacheUpdateManager::MarkNodeIsOccluded(bool isOccluded)
{
    stagingIsOccluded_ = isOccluded;
}

bool RSCacheUpdateManager::WouldDrawLargeAreaBlur()
{
    RS_TRACE_NAME_FMT("wouldDrawLargeAreaBlur stagingIsLargeArea:%d canSkipFrame:%d"
        " stagingUpdateInterval:%d stagingFilterInteractWithDirty:%d",
        stagingIsLargeArea_, canSkipFrame_, cacheUpdateInterval_, stagingFilterInteractWithDirty_);
    ROSEN_LOGD("wouldDrawLargeAreaBlur stagingIsLargeArea:%{public}d canSkipFrame:%{public}d"
        " stagingUpdateInterval:%{public}d stagingFilterInteractWithDirty:%{public}d",
        stagingIsLargeArea_, canSkipFrame_, cacheUpdateInterval_, stagingFilterInteractWithDirty_);
    if (stagingIsLargeArea_) {
        if (!canSkipFrame_) {
            return true;
        }
        return cacheUpdateInterval_ == 1 && stagingFilterInteractWithDirty_;
    }
    return false;
}

bool RSCacheUpdateManager::WouldDrawLargeAreaBlurPrecisely()
{
    RS_TRACE_NAME_FMT("wouldDrawLargeAreaBlurPrecisely stagingIsLargeArea:%d stagingForceClearCache:%d"
        " canSkipFrame:%d stagingFilterHashChanged:%d stagingFilterInteractWithDirty:%d stagingFilterRegionChanged:%d"
        " stagingUpdateInterval:%d stagingLastCacheType:%d", stagingIsLargeArea_,
        stagingForceClearCache_, canSkipFrame_, stagingFilterHashChanged_, stagingFilterInteractWithDirty_,
        stagingFilterRegionChanged_, cacheUpdateInterval_, lastCacheType_);
    if (!stagingIsLargeArea_) {
        return false;
    }
    if (stagingForceClearCache_) {
        return true;
    }
    if (!canSkipFrame_ && !stagingFilterHashChanged_) {
        return true;
    }
    if (!stagingFilterInteractWithDirty_ && !stagingFilterHashChanged_ && !stagingFilterRegionChanged_) {
        return false;
    }
    if (cacheUpdateInterval_ == 0) {
        return true;
    }
    if (lastCacheType_ == FilterCacheType::FILTERED_SNAPSHOT && stagingFilterHashChanged_) {
        return true;
    }
    return false;
}

} // namespace Rosen
} // namespace OHOS

#endif