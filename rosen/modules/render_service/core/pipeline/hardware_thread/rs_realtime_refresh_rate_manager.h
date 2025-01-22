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

#ifndef RS_REALTIME_REFRESH_RATE_MANAGER_H
#define RS_REALTIME_REFRESH_RATE_MANAGER_H

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <unordered_map>

#include "hgm_frame_rate_manager.h"
#include "screen_manager/screen_types.h"

namespace OHOS::Rosen {
enum class RealtimeRefreshRateType : int32_t {
    START = -1,
    COLLECT = 0,
    SHOW = 1,
    END = 2,
};

class RSRealtimeRefreshRateManager {
public:
    static RSRealtimeRefreshRateManager& Instance();

    bool GetShowRefreshRateEnabled() const
    {
        return showEnabled_;
    }
    void SetShowRefreshRateEnabled(bool enabled, int32_t type);
    uint32_t GetRealtimeRefreshRate(ScreenId screenId);
private:
    friend class RSHardwareThread;
    RSRealtimeRefreshRateManager() = default;
    ~RSRealtimeRefreshRateManager() = default;

    inline void CountRealtimeFrame(const ScreenId screenId)
    {
        if (showEnabled_ || collectEnabled_) {
            std::unique_lock<std::mutex> lock(showRealtimeFrameMutex_);
            realtimeFrameCountMap_[screenId]++;
        }
    }
    void StatisticsRefreshRateDataLocked(std::shared_ptr<HgmFrameRateManager> frameRateMgr);

    std::atomic<bool> showEnabled_ = false;
    std::atomic<bool> collectEnabled_ = false;
    std::atomic<bool> isCollectRefreshRateTaskRunning_ = false;
    std::unordered_map<ScreenId, uint32_t> currRealtimeRefreshRateMap_;
    std::unordered_map<ScreenId, uint32_t> realtimeFrameCountMap_;
    std::mutex showRealtimeFrameMutex_;
    std::mutex threadMutex_;
    std::chrono::steady_clock::time_point startTime_;
    uint32_t lastRefreshRate_ = 0;
    bool isRealtimeRefreshRateChange_ = false;
    std::function<void()> showRefreshRateTask_ = nullptr;
    const std::string EVENT_ID = "SHOW_REFRESH_RATE";

    static constexpr uint8_t IDLE_FPS_THRESHOLD = 8;
    static constexpr auto NS_PER_S =
        std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::seconds(1)).count();
    static constexpr uint8_t DEFAULT_REALTIME_REFRESH_RATE = 1;
    static constexpr uint32_t EVENT_INTERVAL = 250; // 250ms
};
} // namespace OHOS::Rosen
#endif // RS_REALTIME_REFRESH_RATE_MANAGER_H
