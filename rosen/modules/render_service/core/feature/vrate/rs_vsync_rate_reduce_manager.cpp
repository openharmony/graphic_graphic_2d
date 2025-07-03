/*
 * Copyright (c) 2024-2024 Huawei Device Co., Ltd.
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

#include "rs_vsync_rate_reduce_manager.h"
#include <parameters.h>
#include <memory>
#include <ratio>
#include <thread>
#include <unordered_set>
#include "common/rs_obj_abs_geometry.h"
#include "common/rs_optional_trace.h"
#include "graphic_feature_param_manager.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "platform/common/rs_log.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {

namespace {
// reduce the app refresh rate by ratio on the frame rate of entire system.
// if frame rate is 120, ratio of 1 means 120, 2 means 60, 3 means 40, 4 means 30
// vsync rate table, first dimension means the balance level as one frame duration increased
// second dimension means the occlusion level
constexpr int VSYNC_RATE_TABLE[][4] = {
    {1, 1, 2, 2},
    {1, 2, 2, 3},
    {2, 2, 3, 3},
    {2, 2, 3, 4},
    {2, 3, 4, 4},
};
constexpr float V_VAL_MIN = 0.0f;
constexpr float V_VAL_MAX = 1.0f;
constexpr float WORKLOAD_TIMES[] = {1.0f, 1.5f, 2.0f, 2.5f};
// WORKLOAD_TIMES + LEVEL_RANGE_UP, LEVEL_RANGE_UP is 0.0f;
constexpr float WORKLOAD_TIMES_UP[] = {1.0f, 1.5f, 2.0f, 2.5f};
// WORKLOAD_TIMES - LEVEL_RANGE_DOWN, LEVEL_RANGE_DOWN is 0.2f;
constexpr float WORKLOAD_TIMES_DOWN[] = {0.8f, 1.3f, 1.8f, 2.3f};
constexpr float V_VAL_INTERVALS[] = {V_VAL_MAX, 0.75f, 0.5f, 0.25f, V_VAL_MIN};
constexpr int BALANCE_FRAME_COUNT = 3;
constexpr int WORKLOAD_TIMES_SIZE = sizeof(WORKLOAD_TIMES) / sizeof(WORKLOAD_TIMES[0]);
constexpr int WORKLOAD_LEVEL_COUNT = sizeof(VSYNC_RATE_TABLE) / sizeof(VSYNC_RATE_TABLE[0]);

// v_val of the v-rate result
constexpr float V_VAL_LEVEL_1 = 1.0f;
constexpr float V_VAL_LEVEL_2 = 0.8f;
constexpr float V_VAL_LEVEL_3 = 0.6f;
constexpr float V_VAL_LEVEL_4 = 0.4f;
constexpr float V_VAL_LEVEL_5 = 0.2f;
constexpr float V_VAL_LEVEL_6 = 0.1f;
constexpr float V_VAL_LEVEL_BEHINDWINDOW = -1.0f;
// ratio of area, such as biggest Rect and total visible Area
constexpr float CONTINUOUS_RATIO_LEVEL_0 = 4.0f / 8.0f;
constexpr float CONTINUOUS_RATIO_LEVEL_1 = 3.0f / 8.0f;
constexpr float CONTINUOUS_RATIO_LEVEL_2 = 2.0f / 8.0f;
constexpr float CONTINUOUS_RATIO_LEVEL_3 = 1.0f / 8.0f;
constexpr float CONTINUOUS_RATIO_LEVEL_4 = 1.0f / 10.0f;
constexpr float CONTINUOUS_RATIO_LEVEL_5 = 1.0f / 12.0f;
constexpr int64_t PERIOD_CHECK_THRESHOLD = 1000 * 1000 * 1000; // 1000,000,000ns = 1.0s
constexpr int32_t DEFAULT_RATE = 1;
constexpr int32_t SIMI_VISIBLE_RATE = 2;
constexpr int32_t SYSTEM_ANIMATED_SCENES_RATE = 2;
constexpr int32_t INVISBLE_WINDOW_RATE = 10;
constexpr int32_t MIN_REFRESH_RATE = 30;
constexpr int INVISBLE_WINDOW_VSYNC_RATIO = std::numeric_limits<int>::max();
constexpr int V_VAL_LEVEL_6_WINDOW_VSYNC_RATIO = std::numeric_limits<int>::max() >> 1;
} // Anonymous namespace

void RSVsyncRateReduceManager::SetFocusedNodeId(NodeId focusedNodeId)
{
    if (!vRateReduceEnabled_) {
        return;
    }
    focusedNodeId_ = focusedNodeId;
}

void RSVsyncRateReduceManager::PushWindowNodeId(NodeId nodeId)
{
    if (!vRateReduceEnabled_) {
        return;
    }
    curAllMainAndLeashWindowNodesIds_.emplace_back(nodeId);
}

void RSVsyncRateReduceManager::ClearLastVisMapForVsyncRate()
{
    if (!vRateReduceEnabled_) {
        return;
    }
    lastVisMapForVSyncVisLevel_.clear();
}

void RSVsyncRateReduceManager::FrameDurationBegin()
{
    if (!vRateConditionQualified_) {
        return;
    }
    curTime_ = Now();
}

void RSVsyncRateReduceManager::FrameDurationEnd()
{
    if (!vRateConditionQualified_) {
        return;
    }
    if (oneFramePeriod_ > 0) {
        float val = static_cast<float>(Now() - curTime_) / static_cast<float>(oneFramePeriod_);
        EnqueueFrameDuration(val);
    }
    curTime_ = 0;
}

void RSVsyncRateReduceManager::SetIsReduceBySystemAnimatedScenes(bool isReduceBySystemAnimatedScenes)
{
    if (!vRateReduceEnabled_) {
        return;
    }
    isReduceBySystemAnimatedScenes_ = isReduceBySystemAnimatedScenes;
}

void RSVsyncRateReduceManager::EnqueueFrameDuration(float duration)
{
    std::lock_guard<std::mutex> lock(mutexFrameDuration_);
    while (frameDurations_.size() >= BALANCE_FRAME_COUNT) {
        frameDurations_.pop_front();
    }
    frameDurations_.push_back(duration);
}

void RSVsyncRateReduceManager::CollectSurfaceVsyncInfo(const ScreenInfo& screenInfo, RSSurfaceRenderNode& surfaceNode)
{
    if (!vRateConditionQualified_) {
        return;
    }
    if (surfaceNode.IsHardwareEnabledTopSurface()) {
        return;
    }
    if (!surfaceNode.IsSCBNode() || surfaceNode.GetDstRect().IsEmpty() || surfaceNode.IsLeashWindow()) {
        return;
    }
    const auto& nodeId = surfaceNode.GetId();
    if (surfaceVRateMap_.find(nodeId) != surfaceVRateMap_.end()) {
        return;
    }
    const auto& nodeName = surfaceNode.GetName();
    auto& geoPtr = surfaceNode.GetRenderProperties().GetBoundsGeometry();
    if (geoPtr == nullptr) {
        RS_LOGE("CollectSurfaceVsyncInfo geoPtr is null %{public}s", nodeName.c_str());
        return;
    }
    auto& appAbsRect = geoPtr->GetAbsRect();
    int width = static_cast<int>(std::round(appAbsRect.width_ * screenInfo.GetRogWidthRatio()));
    int height = static_cast<int>(std::round(appAbsRect.height_ * screenInfo.GetRogHeightRatio()));
    SurfaceVRateInfo vRateInfo;
    vRateInfo.name = nodeName;
    vRateInfo.nodeId = nodeId;
    vRateInfo.visibleRegion = surfaceNode.GetVisibleRegion();
    vRateInfo.visibleRegionBehindWindow = surfaceNode.GetVisibleRegionBehindWindow();
    vRateInfo.appWindowArea = width * height;
    surfaceVRateMap_.emplace(nodeId, std::move(vRateInfo));
}

void RSVsyncRateReduceManager::SetUniVsync()
{
    if (!vRateConditionQualified_) {
        return;
    }
    RS_TRACE_FUNC();
    UpdateRatesLevel();
    CalcRates();
    NotifyVRates();
}

int RSVsyncRateReduceManager::UpdateRatesLevel()
{
    float totalDuration = 0;
    {
        std::lock_guard<std::mutex> lock(mutexFrameDuration_);
        if (frameDurations_.size() < BALANCE_FRAME_COUNT) {
            return curRatesLevel_;
        }
        for (int i = 0; i < BALANCE_FRAME_COUNT; i++) {
            totalDuration = totalDuration + frameDurations_[i];
        }
        frameDurations_.pop_front();
    }
    // stale rate
    int plusLevel = 0;
    // 1: level up
    auto lastRatesLevel = curRatesLevel_;
    int newLevel = -1;
    for (int i = WORKLOAD_TIMES_SIZE - 1; i >= lastRatesLevel; --i) {
        if (totalDuration > static_cast<float>(BALANCE_FRAME_COUNT) * WORKLOAD_TIMES_UP[i]) {
            newLevel = i + 1;
            break;
        }
    }
    if (newLevel > lastRatesLevel) {
        plusLevel = newLevel;
    }

    // 2: level down
    if (newLevel <= 0) {
        newLevel = lastRatesLevel;
        for (int i = 0; i <= lastRatesLevel; ++i) {
            if (totalDuration < static_cast<float>(BALANCE_FRAME_COUNT) * WORKLOAD_TIMES_DOWN[i]) {
                newLevel = i;
                break;
            }
        }
        if (newLevel < lastRatesLevel) {
            plusLevel = -1; // only down one level
        }
    }
    int tempLevel = plusLevel > 0 ? plusLevel : (curRatesLevel_ + plusLevel);
    tempLevel = tempLevel > 0 ? tempLevel : 0;
    curRatesLevel_ = (tempLevel >= WORKLOAD_LEVEL_COUNT) ? WORKLOAD_LEVEL_COUNT - 1 : tempLevel;
    RS_LOGD("curRatesLevel: [%{public}d], tempLevel: [%{public}d]", curRatesLevel_, tempLevel);
    return  curRatesLevel_;
}

void RSVsyncRateReduceManager::CalcRates()
{
    if (isSystemAnimatedScenes_) {
        for (const auto& [nodeId, vRateInfo]: surfaceVRateMap_) {
            vSyncRateMap_[nodeId] = SYSTEM_ANIMATED_SCENES_RATE;
            auto iter = lastVSyncRateMap_.find(nodeId);
            SetVSyncRatesChanged(iter == lastVSyncRateMap_.end() || iter->second != SYSTEM_ANIMATED_SCENES_RATE);
            RS_OPTIONAL_TRACE_NAME_FMT("CalcRates name=%s id=%" PRIu64 ",rate=%d,isSysAni=%d", vRateInfo.name.c_str(),
                nodeId, SYSTEM_ANIMATED_SCENES_RATE, isSystemAnimatedScenes_);
            RS_LOGD("CalcRates name=%{public}s id=%{public}" PRIu64 ",rate=%{public}d,isSysAni=%{public}d",
                vRateInfo.name.c_str(), nodeId, SYSTEM_ANIMATED_SCENES_RATE, isSystemAnimatedScenes_);
        }
        return;
    }
    for (const auto& [nodeId, vRateInfo]: surfaceVRateMap_) {
        double vVal = 0;
        int visArea = vRateInfo.visibleRegion.Area();
        const bool visibleRegionBehindWindowEmpty = vRateInfo.visibleRegionBehindWindow.IsEmpty();
        if (vRateInfo.visibleRegion.IsEmpty()) {
            vVal = V_VAL_MIN;
        } else if (visibleRegionBehindWindowEmpty) {
            vVal = V_VAL_LEVEL_BEHINDWINDOW;
        } else if (nodeId == focusedNodeId_ ||
            visArea >= std::round(vRateInfo.appWindowArea * CONTINUOUS_RATIO_LEVEL_0)) {
            vVal = V_VAL_LEVEL_1;
        } else if (visArea <= std::round(vRateInfo.appWindowArea * CONTINUOUS_RATIO_LEVEL_5)) {
            vVal = V_VAL_LEVEL_6;
        } else {
            RectI maxVisRect = CalcMaxVisibleRect(vRateInfo.visibleRegion, vRateInfo.appWindowArea).ToRectI();
            vVal = CalcVValByAreas(vRateInfo.appWindowArea, maxVisRect.width_ * maxVisRect.height_, visArea);
        }
        auto lastIter = lastVSyncRateMap_.find(nodeId);
        if (vVal >= V_VAL_MAX && (lastIter == lastVSyncRateMap_.end() || lastIter->second <= DEFAULT_RATE)) {
            continue;
        }
        int rate = GetRateByBalanceLevel(vVal);
        vSyncRateMap_[nodeId] = rate;
        SetVSyncRatesChanged(lastIter == lastVSyncRateMap_.end() || lastIter->second != rate);
        RS_OPTIONAL_TRACE_NAME_FMT("CalcRates name=%s id=%" PRIu64 ",vVal=%.2f,rate=%d,isSysAni=%d",
            vRateInfo.name.c_str(), nodeId, vVal, rate, isSystemAnimatedScenes_);
        RS_LOGD("CalcRates name=%{public}s nodeid=%{public}" PRIu64
            ",vVal=%{public}.2f,rate=%{public}d,isSysAni=%{public}d",
            vRateInfo.name.c_str(), nodeId, vVal, rate, isSystemAnimatedScenes_);
    }
}

void RSVsyncRateReduceManager::NotifyVRates()
{
    if (vSyncRateMap_.empty() || !CheckNeedNotify()) {
        return;
    }
    linkersRateMap_.clear();
    for (const auto& [nodeId, rate]: vSyncRateMap_) {
        std::vector<uint64_t> linkerIds = appVSyncDistributor_->GetSurfaceNodeLinkerIds(nodeId);
        if (rate != 0 && RSSystemParameters::GetVRateControlEnabled()) {
            for (auto linkerId : linkerIds) {
                linkersRateMap_.emplace(linkerId, rate);
                RS_OPTIONAL_TRACE_NAME_FMT("NotifyVRates linkerid = %" PRIu64 " nodeId=%" PRIu64
                    " rate=%d isSysAnimate=%d", linkerId, nodeId, rate, isSystemAnimatedScenes_);
                RS_LOGD("NotifyVRates linkerid = %{public}" PRIu64 " nodeId=%{public}"
                    PRIu64 " rate=%{public}d isSysAnimate=%{public}d", linkerId, nodeId, rate, isSystemAnimatedScenes_);
            }
        }
        auto iter = lastVSyncRateMap_.find(nodeId);
        if (iter != lastVSyncRateMap_.end() && iter->second == rate) {
            continue;
        }
        if (RSSystemParameters::GetVRateControlEnabled()) {
            needPostTask_.exchange(true);
        }
        appVSyncDistributor_->SetQosVSyncRate(nodeId, rate, isSystemAnimatedScenes_);
    }

    lastVSyncRateMap_ = vSyncRateMap_;
}
int RSVsyncRateReduceManager::GetRateByBalanceLevel(double vVal)
{
    if (std::abs(vVal - V_VAL_LEVEL_BEHINDWINDOW) < 10e-6) {
        int rateBehindWindow = static_cast<int>(ceil(static_cast<double>(rsRefreshRate_) /
            RS_REFRESH_RATE_BEHIND_WINDOW));
        RS_LOGD("RSVsyncRateReduceManager::GetRateByBalanceLevel in behind window condition vVal=%{public}.2f, rate=%d",
            vVal, rateBehindWindow);
        return rateBehindWindow;
    }
    if (vVal <= 0) {
        return INVISBLE_WINDOW_VSYNC_RATIO;
    }
    if (vVal <= V_VAL_LEVEL_6 && curRatesLevel_ != 0) {
        return V_VAL_LEVEL_6_WINDOW_VSYNC_RATIO;
    }
    if (vVal >= V_VAL_INTERVALS[0]) {
        return DEFAULT_RATE;
    }
    if (curRatesLevel_ > WORKLOAD_LEVEL_COUNT - 1) {
        RS_LOGE("GetRateByBalanceLevel curRatesLevel error");
        return DEFAULT_RATE;
    }
    int minRate = rsRefreshRate_ / static_cast<uint32_t>(MIN_REFRESH_RATE);
    auto& rates = VSYNC_RATE_TABLE[curRatesLevel_];
    size_t intervalsSize = sizeof(V_VAL_INTERVALS) / sizeof(float);
    for (size_t i = 1; i < intervalsSize; ++i) {
        if (vVal > V_VAL_INTERVALS[i]) {
            return std::min(minRate, rates[i - 1]);
        }
    }
    return V_VAL_LEVEL_6_WINDOW_VSYNC_RATIO;
}

inline Occlusion::Rect RSVsyncRateReduceManager::GetMaxVerticalRect(const Occlusion::Region &region)
{
    Occlusion::Rect maxRect;
    auto& regionRects = region.GetRegionRects();
    for (const auto& regionRect : regionRects) {
        if (regionRect.Area() > maxRect.Area()) {
            maxRect = regionRect;
        }
    }
    return maxRect;
}

Occlusion::Rect RSVsyncRateReduceManager::CalcMaxVisibleRect(const Occlusion::Region &region,
    int appWindowArea)
{
    int maxRArea = 0;
    Occlusion::Rect maxRect;
    Occlusion::Region srcRegion = region;
    Occlusion::Rect srcBound = region.GetBound();
    int minArea = std::round(appWindowArea * CONTINUOUS_RATIO_LEVEL_5);

    const std::vector<Occlusion::Rect>& rects = region.GetRegionRects();
    std::unordered_set<int> xPositionSet = {srcBound.left_, srcBound.right_};
    std::vector<int> xPositions;
    for (const auto &rect: rects) {
        if (rect.Area() > maxRArea) {
            maxRect = rect;
            maxRArea = maxRect.Area();
        }
        xPositionSet.emplace(rect.left_);
        xPositionSet.emplace(rect.right_);
    }
    if (maxRArea >= std::round(appWindowArea * CONTINUOUS_RATIO_LEVEL_1)) {
        return maxRect;
    }
    xPositions.assign(xPositionSet.begin(), xPositionSet.end());
    std::sort(xPositions.begin(), xPositions.end());
    for (size_t i = 0; i < xPositions.size() - 1; ++i) {
        for (size_t j = i + 1; j < xPositions.size(); ++j) {
            Occlusion::Rect subBound(xPositions[i], srcBound.top_, xPositions[j], srcBound.bottom_);
            int baseArea = std::max(maxRArea, minArea);
            if (subBound.Area() <= baseArea) {
                continue;
            }
            Occlusion::Region subRegion{subBound};
            Occlusion::Region verticalRegion = subRegion.And(srcRegion);
            if (verticalRegion.Area() <= baseArea) {
                continue;
            }
            Occlusion::Rect tmpRect = GetMaxVerticalRect(verticalRegion);
            if (tmpRect.Area() > maxRArea) {
                maxRect = tmpRect;
                maxRArea = tmpRect.Area();
            }
        }
    }
    return maxRect;
}

float RSVsyncRateReduceManager::CalcVValByAreas(int windowArea, int maxVisRectArea, int visTotalArea)
{
    int level0Area = std::round(windowArea * CONTINUOUS_RATIO_LEVEL_0);
    int level1Area = std::round(windowArea * CONTINUOUS_RATIO_LEVEL_1);
    if (visTotalArea >= level0Area || maxVisRectArea >= level1Area) {
        return V_VAL_LEVEL_1;
    }
    int level2Area = std::round(windowArea * CONTINUOUS_RATIO_LEVEL_2);
    if (visTotalArea >= level1Area || maxVisRectArea >= level2Area) {
        return V_VAL_LEVEL_2;
    }
    int level3Area = std::round(windowArea * CONTINUOUS_RATIO_LEVEL_3);
    if (visTotalArea >= level2Area || maxVisRectArea >= level3Area) {
        return V_VAL_LEVEL_3;
    }
    int level4Area = std::round(windowArea * CONTINUOUS_RATIO_LEVEL_4);
    if (visTotalArea >= level3Area || maxVisRectArea >= level4Area) {
        return V_VAL_LEVEL_4;
    }
    int level5Area = std::round(windowArea * CONTINUOUS_RATIO_LEVEL_5);
    if (visTotalArea >= level4Area || maxVisRectArea >= level5Area) {
        return V_VAL_LEVEL_5;
    }
    return V_VAL_LEVEL_6;
}

uint64_t RSVsyncRateReduceManager::Now()
{
    return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch())
            .count();
}

void RSVsyncRateReduceManager::SetVSyncRateByVisibleLevel(std::map<NodeId, RSVisibleLevel>& visMapForVsyncRate,
    std::vector<RSBaseRenderNode::SharedPtr>& curAllSurfaces)
{
    if (!vRateConditionQualified_) {
        return;
    }
    if (!RSMainThread::Instance()->IsSystemAnimatedScenesListEmpty()) {
        visMapForVsyncRate.clear();
        for (auto it = curAllSurfaces.rbegin(); it != curAllSurfaces.rend(); ++it) {
            auto curSurface = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*it);
            if (curSurface == nullptr || curSurface->GetDstRect().IsEmpty() || curSurface->IsLeashWindow()) {
                continue;
            }
            visMapForVsyncRate[curSurface->GetId()] = RSVisibleLevel::RS_SYSTEM_ANIMATE_SCENE;
        }
        isReduceBySystemAnimatedScenes_ = true;
    }
    NotifyVSyncRates(visMapForVsyncRate);
}

bool RSVsyncRateReduceManager::CheckNeedNotify()
{
    bool surfaceIdsChanged = lastAllMainAndLeashWindowNodesIds_ != curAllMainAndLeashWindowNodesIds_;
    bool focusChanged = lastFocusedNodeId_ != focusedNodeId_;
    bool needRefresh = !isSystemAnimatedScenes_ && isReduceBySystemAnimatedScenes_;
    bool notifyCdt = vSyncRatesChanged_ || surfaceIdsChanged || focusChanged || needRefresh;
    RS_LOGD("CheckNeedNotify notifyCdt=%{public}d %{public}s%{public}s%{public}s%{public}s%{public}s", notifyCdt,
        isSystemAnimatedScenes_ ? "sysAnimated|" : "", needRefresh ? "needR|" : "", vSyncRatesChanged_ ? "ratesC|" : "",
        surfaceIdsChanged ? "surfaceIdsC|" : "", focusChanged ? "focusIdC" : "");
    RS_TRACE_NAME_FMT("CheckNeedNotify notifyCdt=%d %s%s%s%s%s", notifyCdt,
        isSystemAnimatedScenes_ ? "sysAnimated|" : "", needRefresh ? "needR|" : "", vSyncRatesChanged_ ? "ratesC|" : "",
        surfaceIdsChanged ? "surfaceIdsC|" : "", focusChanged ? "focusIdC" : "");
    if (!notifyCdt) {
        return false;
    }
    if (surfaceIdsChanged) {
        lastAllMainAndLeashWindowNodesIds_ = curAllMainAndLeashWindowNodesIds_;
    }
    if (focusChanged) {
        lastFocusedNodeId_ = focusedNodeId_;
    }
    if (needRefresh) {
        isReduceBySystemAnimatedScenes_ = false;
    }
    if (surfaceIdsChanged || needRefresh) {
        for (const auto& [nodeId, rate]: lastVSyncRateMap_) {
            if (vSyncRateMap_.find(nodeId) == vSyncRateMap_.end()) {
                vSyncRateMap_.emplace(nodeId, DEFAULT_RATE);
            }
        }
    }
    if (isSystemAnimatedScenes_) {
        isReduceBySystemAnimatedScenes_ = true;
    }
    return true;
}

void RSVsyncRateReduceManager::NotifyVSyncRates(const std::map<NodeId, RSVisibleLevel>& vSyncVisLevelMap)
{
    RS_TRACE_NAME_FMT("NotifyVSyncRates vSyncRates.size=[%lu]", vSyncVisLevelMap.size());
    if (vSyncVisLevelMap.empty()) {
        return;
    }
    auto notifyVsyncFunc = [distributor = appVSyncDistributor_] (NodeId nodeId, RSVisibleLevel level) {
        bool isSysAnimate = false;
        int32_t rate = DEFAULT_RATE;
        if (level == RSVisibleLevel::RS_SEMI_DEFAULT_VISIBLE) {
            rate = SIMI_VISIBLE_RATE;
        } else if (level == RSVisibleLevel::RS_SYSTEM_ANIMATE_SCENE) {
            isSysAnimate = true;
            rate = SYSTEM_ANIMATED_SCENES_RATE;
        } else if (level == RSVisibleLevel::RS_INVISIBLE) {
            rate = INVISBLE_WINDOW_RATE;
        } else {
            rate = DEFAULT_RATE;
        }
        distributor->SetQosVSyncRate(nodeId, rate, isSysAnimate);
        RS_OPTIONAL_TRACE_NAME_FMT("NotifyVSyncRates nodeId=%" PRIu64 " rate=%d isSysAnimate=%d", nodeId, rate,
            isSysAnimate);
    };

    bool isVisibleChanged = lastVisMapForVSyncVisLevel_.size() != vSyncVisLevelMap.size();
    if (!isVisibleChanged) {
        auto iterCur = vSyncVisLevelMap.begin();
        auto iterLast = lastVisMapForVSyncVisLevel_.begin();
        for (; iterCur != vSyncVisLevelMap.end(); iterCur++, iterLast++) {
            if (iterCur->first != iterLast->first || iterCur->second != iterLast->second) {
                isVisibleChanged = true;
                notifyVsyncFunc(iterCur->first, iterCur->second);
            }
        }
        if (isVisibleChanged) {
            lastVisMapForVSyncVisLevel_ = vSyncVisLevelMap;
        }
    } else {
        lastVisMapForVSyncVisLevel_.clear();
        for (const auto& [nodeId, visLevel] : vSyncVisLevelMap) {
            notifyVsyncFunc(nodeId, visLevel);
            lastVisMapForVSyncVisLevel_.emplace(nodeId, visLevel);
        }
    }
}

void RSVsyncRateReduceManager::Init(const sptr<VSyncDistributor>& appVSyncDistributor)
{
    appVSyncDistributor_ = appVSyncDistributor;
    isDeviceSupprotVRate_ = VRateParam::GetVRateEnable();
    vRateReduceEnabled_ = isDeviceSupprotVRate_ && RSSystemParameters::GetVSyncControlEnabled();
    RS_LOGI("Init vRateReduceEnabled_ is %{public}d", vRateReduceEnabled_);
}

void RSVsyncRateReduceManager::ResetFrameValues(uint32_t rsRefreshRate)
{
    vRateConditionQualified_ = false;
    if (!vRateReduceEnabled_) {
        return;
    }
    focusedNodeId_ = 0;
    surfaceVRateMap_.clear();
    isSystemAnimatedScenes_ = !RSMainThread::Instance()->IsSystemAnimatedScenesListEmpty();
    vSyncRatesChanged_ = false;
    vSyncRateMap_.clear();
    curAllMainAndLeashWindowNodesIds_.clear();
    visMapForVSyncVisLevel_.clear();
    vRateConditionQualified_ = rsRefreshRate > 0 && appVSyncDistributor_ != nullptr;
    if (!vRateConditionQualified_) {
        return;
    }
    oneFramePeriod_ = PERIOD_CHECK_THRESHOLD / static_cast<int64_t>(rsRefreshRate);
    rsRefreshRate_ = rsRefreshRate;
}
} // namespace Rosen
} // namespace OHOS
