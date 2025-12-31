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

#include "hgm_log.h"
#include "hgm_task_handle_thread.h"
#include "pipeline/main_thread/rs_main_thread.h"

namespace OHOS::Rosen {
RSRealtimeRefreshRateManager& RSRealtimeRefreshRateManager::Instance()
{
    static RSRealtimeRefreshRateManager instance;
    return instance;
}

RSRealtimeRefreshRateManager::RSRealtimeRefreshRateManager()
{
    showRefreshRateTask_ = [this] {
        bool isRealtimeRefreshRateChange = false;
        std::unique_lock<std::mutex> lock(realtimeRateMutex_);
        auto diff = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - startTime_);
        for (auto [screenId, frameCount] : realtimeFrameCountMap_) {
            uint32_t fps = std::round(frameCount * static_cast<float>(NS_PER_S) / diff.count());
            fps = fps <= IDLE_FPS_THRESHOLD ? 1u : fps;
            auto iter = currRealtimeRefreshRateMap_.find(screenId);
            if (iter == currRealtimeRefreshRateMap_.end()) {
                currRealtimeRefreshRateMap_.emplace(screenId, fps);
                isRealtimeRefreshRateChange = true;
            } else if (iter->second != fps) {
                iter->second = fps;
                isRealtimeRefreshRateChange = true;
            }
        }
        if (isRealtimeRefreshRateChange) {
            if (showEnabled_) {
                RSMainThread::Instance()->SetDirtyFlag();
                RSMainThread::Instance()->RequestNextVSync();
            }
        }
        startTime_ = std::chrono::steady_clock::now();
        realtimeFrameCountMap_.clear();
        HgmTaskHandleThread::Instance().PostEvent(EVENT_ID, showRefreshRateTask_, EVENT_INTERVAL);
    };
}

void RSRealtimeRefreshRateManager::SetShowRefreshRateEnabled(bool enabled, int32_t type)
{
    HGM_LOGD("enabled[%{public}d] type[%{public}d]", enabled, type);
    if (type <= static_cast<int32_t>(RealtimeRefreshRateType::START) ||
        type >= static_cast<int32_t>(RealtimeRefreshRateType::END)) {
        return;
    }
    std::unique_lock<std::mutex> lock(realtimeRateMutex_);
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

    if (!showEnabled_ && !collectEnabled_) {
        HgmTaskHandleThread::Instance().RemoveEvent(EVENT_ID);
        isCollectRefreshRateTaskRunning_ = false;
        currRealtimeRefreshRateMap_.clear();
        realtimeFrameCountMap_.clear();
        return;
    }

    if (isCollectRefreshRateTaskRunning_) {
        return;
    }

    startTime_ = std::chrono::steady_clock::now();
    isCollectRefreshRateTaskRunning_ = true;
    HgmTaskHandleThread::Instance().PostEvent(EVENT_ID, showRefreshRateTask_, EVENT_INTERVAL);
}

void RSRealtimeRefreshRateManager::UpdateScreenRefreshRate(ScreenId screenId, uint32_t refreshRate)
{
    std::unique_lock<std::mutex> lock(realtimeRateMutex_);
    bool isScreenRefreshRateChange = false;
    auto iter = screenRefreshRateMap_.find(screenId);
    if (iter == screenRefreshRateMap_.end()) {
        screenRefreshRateMap_.emplace(screenId, refreshRate);
        isScreenRefreshRateChange = true;
    } else if (iter->second != refreshRate) {
        iter->second = refreshRate;
        isScreenRefreshRateChange = true;
    }
    if (isScreenRefreshRateChange) {
        if (showEnabled_) {
            RSMainThread::Instance()->SetDirtyFlag();
            RSMainThread::Instance()->RequestNextVSync();
        }
    }
}

std::pair<uint32_t, uint32_t> RSRealtimeRefreshRateManager::GetRefreshRateByScreenId(ScreenId screenId)
{
    std::unique_lock<std::mutex> lock(realtimeRateMutex_);
    if (!showEnabled_ && !collectEnabled_) {
        return { 0, 0 };
    }
    uint32_t currentRefreshRate = DEFAULT_SCREEN_REFRESH_RATE;
    if (auto iter = screenRefreshRateMap_.find(screenId); iter != screenRefreshRateMap_.end()) {
        currentRefreshRate = iter->second;
    }
    uint32_t realtimeRefreshRate = DEFAULT_REALTIME_REFRESH_RATE;
    if (auto iter = currRealtimeRefreshRateMap_.find(screenId); iter != currRealtimeRefreshRateMap_.end()) {
        // Prevent the refresh rate on the right from being greater than that on the left
        realtimeRefreshRate = std::min(iter->second, currentRefreshRate);
    }
    return { currentRefreshRate, realtimeRefreshRate };
}

uint32_t RSRealtimeRefreshRateManager::GetRealtimeRefreshRateByScreenId(ScreenId screenId)
{
    return GetRefreshRateByScreenId(screenId).second;
}
} // namespace OHOS::Rosen
