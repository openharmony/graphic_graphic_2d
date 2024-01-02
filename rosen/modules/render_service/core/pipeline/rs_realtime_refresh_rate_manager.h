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

namespace OHOS::Rosen {
class RSRealtimeRefreshRateManager {
public:
    static RSRealtimeRefreshRateManager& Instance();

    bool GetShowRefreshRateEnabled() const
    {
        return enableState_;
    }
    void SetShowRefreshRateEnabled(bool enable);
    uint32_t GetRealtimeRefreshRate() const
    {
        return currRealtimeRefreshRate_;
    }
private:
    friend class RSHardwareThread;
    RSRealtimeRefreshRateManager() = default;
    ~RSRealtimeRefreshRateManager() = default;

    inline void CountRealtimeFrame()
    {
        if (enableState_) {
            realtimeFrameCount_++;
        }
    }

    std::atomic<bool> enableState_ = false;
    uint32_t currRealtimeRefreshRate_ = 1;
    std::atomic<uint32_t> realtimeFrameCount_ = 0;
};
} // namespace OHOS::Rosen
#endif // RS_REALTIME_REFRESH_RATE_MANAGER_H
