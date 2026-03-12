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

#include "screen_manager/rs_screen_property.h"

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

    bool GetShowRefreshRateEnabled()
    {
        std::unique_lock<std::mutex> lock(realtimeRateMutex_);
        return showEnabled_;
    }
    void SetShowRefreshRateEnabled(bool enabled, int32_t type);
    void UpdateScreenRefreshRate(const RSScreenProperty& property, ScreenPropertyType type);
    std::pair<uint32_t, uint32_t> GetRefreshRateByScreenId(ScreenId screenId);
    uint32_t GetRealtimeRefreshRateByScreenId(ScreenId screenId);

private:
    friend class RSUniRenderComposerAdapter;
    RSRealtimeRefreshRateManager();
    ~RSRealtimeRefreshRateManager() = default;

    inline void CountRealtimeFrame(const ScreenId screenId)
    {
        std::unique_lock<std::mutex> lock(realtimeRateMutex_);
        if (showEnabled_ || collectEnabled_) {
            realtimeFrameCountMap_[screenId]++;
        }
    }

    bool showEnabled_ = false;
    bool collectEnabled_ = false;
    bool isCollectRefreshRateTaskRunning_ = false;
    std::unordered_map<ScreenId, uint32_t> screenRefreshRateMap_;
    std::unordered_map<ScreenId, uint32_t> currRealtimeRefreshRateMap_;
    std::unordered_map<ScreenId, uint32_t> realtimeFrameCountMap_;
    std::chrono::steady_clock::time_point startTime_;
    std::mutex realtimeRateMutex_;

    std::function<void()> showRefreshRateTask_ = nullptr;

    static constexpr char EVENT_ID[] = "SHOW_REFRESH_RATE";
    static constexpr uint8_t IDLE_FPS_THRESHOLD = 8;
    static constexpr auto NS_PER_S =
        std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::seconds(1)).count();
    static constexpr uint8_t DEFAULT_SCREEN_REFRESH_RATE = 60;
    static constexpr uint8_t DEFAULT_REALTIME_REFRESH_RATE = 1;
    static constexpr uint32_t EVENT_INTERVAL = 250; // 250ms
};
} // namespace OHOS::Rosen
#endif // RS_REALTIME_REFRESH_RATE_MANAGER_H
