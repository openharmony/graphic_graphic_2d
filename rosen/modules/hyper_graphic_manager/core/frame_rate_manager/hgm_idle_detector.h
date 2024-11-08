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
    static constexpr int32_t ANIMATOR_NOT_RUNNING = -1;

    HgmIdleDetector() = default;
    ~HgmIdleDetector() = default;

    void SetAppSupportedState(bool appSupported)
    {
        std::lock_guard<std::mutex> lock(appSupportedMutex_);
        appSupported_ = appSupported;
    }

    bool GetAppSupportedState()
    {
        std::lock_guard<std::mutex> lock(appSupportedMutex_);
        return appSupported_;
    }

    void SetAceAnimatorIdleState(bool aceAnimatorIdleState)
    {
        aceAnimatorIdleState_ = aceAnimatorIdleState;
    }

    bool GetAceAnimatorIdleState() const
    {
        return aceAnimatorIdleState_;
    }

    void UpdateAceAnimatorExpectedFrameRate(int32_t aceAnimatorExpectedFrameRate)
    {
        if (aceAnimatorExpectedFrameRate > aceAnimatorExpectedFrameRate_) {
            aceAnimatorExpectedFrameRate_ = aceAnimatorExpectedFrameRate;
        }
    }

    int32_t GetAceAnimatorExpectedFrameRate() const
    {
        return aceAnimatorExpectedFrameRate_;
    }

    void ResetAceAnimatorExpectedFrameRate()
    {
        aceAnimatorExpectedFrameRate_ = ANIMATOR_NOT_RUNNING;
    }

    void UpdateSurfaceTime(const std::string& surfaceName, uint64_t timestamp,  pid_t pid);
    bool GetSurfaceIdleState(uint64_t timestamp);
    int32_t GetTouchUpExpectedFPS();
    bool ThirdFrameNeedHighRefresh();
    void ClearAppBufferList()
    {
        std::lock_guard<std::mutex> lock(appBufferListMutex_);
        appBufferList_.clear();
    }
    void ClearAppBufferBlackList()
    {
        std::lock_guard<std::mutex> lock(appBufferBlackListMutex_);
        appBufferBlackList_.clear();
    }
    void UpdateAppBufferList(std::vector<std::pair<std::string, int32_t>> &appBufferList)
    {
        std::lock_guard<std::mutex> lock(appBufferListMutex_);
        appBufferList_ = appBufferList;
    }
    void UpdateAppBufferBlackList(std::vector<std::string> &appBufferBlackList)
    {
        std::lock_guard<std::mutex> lock(appBufferBlackListMutex_);
        appBufferBlackList_ = appBufferBlackList;
    }
    void UpdateSupportAppBufferList(std::vector<std::string> &supportAppBufferList)
    {
        supportAppBufferList_ = supportAppBufferList;
    }
private:
    bool appSupported_ = false;
    bool aceAnimatorIdleState_ = true;
    int32_t aceAnimatorExpectedFrameRate_ = ANIMATOR_NOT_RUNNING;
    std::mutex appSupportedMutex_;
    std::mutex appBufferListMutex_;
    std::mutex appBufferBlackListMutex_;
    // FORMAT: <buffername>
    std::vector<std::string> appBufferBlackList_;
    std::vector<std::string> supportAppBufferList_;
    // FORMAT: <buffername, time>
    std::unordered_map<std::string, uint64_t> frameTimeMap_;
    // FORMAT: <buffername, fps>
    std::vector<std::pair<std::string, int32_t>> appBufferList_;
};
} // namespace Rosen
} // namespace OHOS
#endif // HGM_IDLE_DETECTOR_H
