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

#include "hgm_idle_detector.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr uint64_t BUFFER_IDLE_TIME_OUT = 200000000; // 200ms
    constexpr uint64_t MAX_BUFFER_COUNT = 10;
    constexpr uint32_t MAX_BUFFER_LENGTH = 10;
    constexpr uint32_t FPS_120 = 120;
    const std::string ACE_ANIMATOR_NAME = "AceAnimato";
    const std::string OTHER_SURFACE = "Other_SF";
}

void HgmIdleDetector::UpdateSurfaceTime(const std::string& surfaceName, uint64_t timestamp)
{
    if (!GetAppSupportStatus() || frameTimeMap_.size() > MAX_BUFFER_COUNT) {
        if (!frameTimeMap_.empty()) {
            frameTimeMap_.clear();
        }
        return;
    }
    if (surfaceName.empty()) {
        return;
    }

    auto temp = surfaceName;
    if (surfaceName.size() > MAX_BUFFER_LENGTH) {
        temp = surfaceName.substr(0, MAX_BUFFER_LENGTH);
    }

    auto it = std::find(supportAppBufferList_.begin(), supportAppBufferList_.end(), OTHER_SURFACE);
    if (it == supportAppBufferList_.end()) {
        auto bufferName = std::find(supportAppBufferList_.begin(), supportAppBufferList_.end(), temp);
        if (bufferName == supportAppBufferList_.end()) {
            return;
        }
    }

    frameTimeMap_[temp] = timestamp;
}

bool HgmIdleDetector::GetSurfaceIdleState(uint64_t timestamp)
{
    bool idle = true;

    if (frameTimeMap_.empty()) {
        return idle;
    }

    for (auto it = frameTimeMap_.begin(); it != frameTimeMap_.end();) {
        if ((timestamp - it->second) > BUFFER_IDLE_TIME_OUT) {
            it = frameTimeMap_.erase(it);
        } else {
            idle = false;
            it++;
        }
    }

    return idle;
}

bool HgmIdleDetector::GetSupportSurface()
{
    std::lock_guard<std::mutex> lock(appBufferBlackListMutex_);
    if (appBufferBlackList_.empty()) {
        return true;
    }
    if (std::find(appBufferBlackList_.begin(), appBufferBlackList_.end(), ACE_ANIMATOR_NAME) ==
        appBufferBlackList_.end() && !aceAnimatorIdleState_) {
        return true;
    }

    if (frameTimeMap_.empty()) {
        return false;
    }

    for (auto &it : frameTimeMap_) {
        if (std::find(appBufferBlackList_.begin(), appBufferBlackList_.end(), it.first) ==
            appBufferBlackList_.end()) {
            return true;
        }
    }
    return false;
}

int32_t HgmIdleDetector::GetSurfaceUpExpectFps()
{
    int32_t fps = FPS_120;

    std::lock_guard<std::mutex> lock(appBufferListMutex_);
    if (appBufferList_.empty()) {
        return fps;
    }
    if (!aceAnimatorIdleState_) {
        auto iter = std::find_if(appBufferList_.begin(), appBufferList_.end(),
            [&](const auto& pair) { return pair.first == ACE_ANIMATOR_NAME; });
        if (iter != appBufferList_.end() && frameTimeMap_.empty()) {
            return iter->second;
        }
    }

    for (auto &member : frameTimeMap_) {
        auto key = member.first;
        auto it = std::find_if(appBufferList_.begin(), appBufferList_.end(),
            [&key](const std::pair<std::string, int32_t>& pair) { return pair.first == key; });
        if (it == appBufferList_.end()) {
            return fps;
        }
    }

    for (auto &it : appBufferList_) {
        if (it.first == ACE_ANIMATOR_NAME && !aceAnimatorIdleState_) {
            return it.second;
        }
        if (frameTimeMap_.count(it.first)) {
            return it.second;
        }
    }

    return fps;
}

} // namespace Rosen
} // namespace OHOS