/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "rs_realtime_refresh_rate_manager.h"

#include <chrono>
#include <condition_variable>
#include <mutex>

#include "hgm_core.h"
#include "rs_trace.h"
#include "pipeline/main_thread/rs_main_thread.h"

namespace OHOS::Rosen {
RSRealtimeRefreshRateManager& RSRealtimeRefreshRateManager::Instance()
{
    static RSRealtimeRefreshRateManager instance;
    return instance;
}

void RSRealtimeRefreshRateManager::SetShowRefreshRateEnabled(bool enabled, int32_t type)
{
    RS_LOGD("SetShowRefreshRateEnabled: enabled[%{public}d] type[%{public}d]", enabled, type);
    std::unique_lock<std::mutex> threadLock(threadMutex_);
    auto frameRateMgr = HgmCore::Instance().GetFrameRateMgr();
    if (frameRateMgr == nullptr ||
        type <= static_cast<int32_t>(RealtimeRefreshRateType::START) ||
        type >= static_cast<int32_t>(RealtimeRefreshRateType::END)) {
        return;
    }

    RealtimeRefreshRateType enumType = static_cast<RealtimeRefreshRateType>(type);
    if (enumType == RealtimeRefreshRateType::SHOW) {
        if (showEnabled_ == enabled) {
            return;
        }
        showEnabled_ = enabled;
    } else if (enumType == RealtimeRefreshRateType::COLLECT) {
        if (collectEnabled_ == enabled) {
            return;
        }
        collectEnabled_ = enabled;
    } else {
        return;
    }

    StatisticsRefreshRateDataLocked(frameRateMgr);
}

void RSRealtimeRefreshRateManager::StatisticsRefreshRateDataLocked(std::shared_ptr<HgmFrameRateManager> frameRateMgr)
{
    if (!showEnabled_ && !collectEnabled_) {
        HgmTaskHandleThread::Instance().RemoveEvent(EVENT_ID);
        isCollectRefreshRateTaskRunning_ = false;
        std::unique_lock<std::mutex> lock(showRealtimeFrameMutex_);
        currRealtimeRefreshRateMap_.clear();
        realtimeFrameCountMap_.clear();
        return;
    }

    if (isCollectRefreshRateTaskRunning_) {
        return;
    }

    startTime_ = std::chrono::steady_clock::now();
    showRefreshRateTask_ = [this, frameRateMgr] () {
        std::unique_lock<std::mutex> lock(showRealtimeFrameMutex_);
        auto diff = std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::steady_clock::now() - startTime_);
        for (auto [screenId, frameCount]: realtimeFrameCountMap_) {
            uint32_t fps = std::round(frameCount * static_cast<float>(NS_PER_S) / diff.count());
            fps = fps <= IDLE_FPS_THRESHOLD ? 1u : fps;
            if (auto iter = currRealtimeRefreshRateMap_.find(screenId);
                iter == currRealtimeRefreshRateMap_.end() || iter->second != fps) {
                currRealtimeRefreshRateMap_[screenId] = fps;
                isRealtimeRefreshRateChange_ = true;
            }
        }
        auto refreshRate = HgmCore::Instance().GetScreenCurrentRefreshRate(frameRateMgr->GetCurScreenId());
        if (isRealtimeRefreshRateChange_|| lastRefreshRate_ != refreshRate) {
            lastRefreshRate_ = refreshRate;
            isRealtimeRefreshRateChange_ = false;
            if (showEnabled_) {
                RSMainThread::Instance()->SetDirtyFlag();
                RSMainThread::Instance()->RequestNextVSync();
            }
        }
        startTime_ = std::chrono::steady_clock::now();
        realtimeFrameCountMap_.clear();
        HgmTaskHandleThread::Instance().PostEvent(EVENT_ID, showRefreshRateTask_, EVENT_INTERVAL);
    };
    isCollectRefreshRateTaskRunning_ = true;
    HgmTaskHandleThread::Instance().PostEvent(EVENT_ID, showRefreshRateTask_, EVENT_INTERVAL);
}

uint32_t RSRealtimeRefreshRateManager::GetRealtimeRefreshRate(ScreenId screenId)
{
    RS_LOGD("GetRealtimeRefreshRate: screenId[%{public}" PRIu64"]", screenId);
    {
        std::unique_lock<std::mutex> threadLock(threadMutex_);
        if (!showEnabled_ && !collectEnabled_) {
            return 0;
        }
    }
    if (screenId == INVALID_SCREEN_ID) {
        auto frameRateMgr = HgmCore::Instance().GetFrameRateMgr();
        if (frameRateMgr == nullptr) {
            return DEFAULT_REALTIME_REFRESH_RATE;
        }
        screenId = frameRateMgr->GetCurScreenId();
    }
    std::unique_lock<std::mutex> lock(showRealtimeFrameMutex_);
    if (auto iter = currRealtimeRefreshRateMap_.find(screenId); iter != currRealtimeRefreshRateMap_.end()) {
        uint32_t currentRefreshRate = OHOS::Rosen::HgmCore::Instance().GetScreenCurrentRefreshRate(screenId);
        if (iter->second > currentRefreshRate) {
            return currentRefreshRate;
        }
        return iter->second;
    }
    return DEFAULT_REALTIME_REFRESH_RATE;
}
} // namespace OHOS::Rosen
