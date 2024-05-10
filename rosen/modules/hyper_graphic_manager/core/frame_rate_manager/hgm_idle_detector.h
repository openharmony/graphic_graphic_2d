/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef HGM_IDLE_DETECTOR_H
#define HGM_IDLE_DETECTOR_H

#include <mutex>
#include <unordered_map>

namespace OHOS {
namespace Rosen {

class HgmIdleDetector {
public:
    HgmIdleDetector() = default;
    ~HgmIdleDetector() = default;

    void SetAppSupportStatus(bool appSupported)
    {
        std::lock_guard<std::mutex> lock(appSupportedMutex_);
        appSupported_ = appSupported;
    }

    bool GetAppSupportStatus()
    {
        std::lock_guard<std::mutex> lock(appSupportedMutex_);
        return appSupported_;
    }

    void SetAceAnimatorIdleStatus(bool aceAnimatorIdleState)
    {
        aceAnimatorIdleState_ = aceAnimatorIdleState;
    }

    bool GetAceAnimatorIdleStatus() const
    {
        return aceAnimatorIdleState_;
    }

    void SetTouchUpTime(uint64_t touchUpTime)
    {
        std::lock_guard<std::mutex> lock(touchUpTimeMutex_);
        touchUpTime_ = touchUpTime;
    }

    uint64_t GetTouchUpTime()
    {
        std::lock_guard<std::mutex> lock(touchUpTimeMutex_);
        return touchUpTime_;
    }

    void UpdateSurfaceTime(const std::string& name, uint64_t timestamp);
    bool GetSurFaceIdleState(uint64_t timestamp);
 
private:
    bool appSupported_ = false;
    bool aceAnimatorIdleState_ = true;
    uint64_t touchUpTime_ = 0;
    std::mutex appSupportedMutex_;
    std::mutex touchUpTimeMutex_;
    // FORMAT: <buffername, time>
    std::unordered_map<std::string, uint64_t> frameTimeMap_;
};
} // namespace Rosen
} // namespace OHOS
#endif // HGM_IDLE_DETECTOR_H
