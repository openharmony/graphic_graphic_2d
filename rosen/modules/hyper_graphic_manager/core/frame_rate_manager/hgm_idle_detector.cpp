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
#include "rs_trace.h"
#include "hgm_task_handle_thread.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr uint64_t BUFFER_IDLE_TIME_OUT = 200000000; // 200ms
    constexpr uint64_t MAX_CACHED_VALID_SURFACE_NAME_COUNT = 60;
    constexpr uint32_t FPS_MAX = 120;
    const std::string ACE_ANIMATOR_NAME = "AceAnimato";
    const std::string OTHER_SURFACE = "Other_SF";
}

void HgmIdleDetector::UpdateSurfaceTime(const std::string& surfaceName, uint64_t timestamp,
    pid_t pid, UIFWKType uiFwkType)
{
    if (surfaceName.empty()) {
        return;
    }

    if (!GetAppSupportedState() || frameTimeMap_.size() > MAX_CACHED_VALID_SURFACE_NAME_COUNT) {
        if (!frameTimeMap_.empty()) {
            frameTimeMap_.clear();
        }
        return;
    }

    std::string validSurfaceName = "";
    bool needHighRefresh = false;
    switch (uiFwkType) {
        case UIFWKType::FROM_UNKNOWN:
            needHighRefresh = GetUnknownFrameworkState(surfaceName, validSurfaceName);
            break;
        case UIFWKType::FROM_SURFACE:
            needHighRefresh = GetSurfaceFrameworkState(surfaceName, validSurfaceName);
            break;
        default:
            break;
    }
    if (!needHighRefresh) {
        return;
    }

    RS_TRACE_NAME_FMT("UpdateSurfaceTime:: Not Idle SurFace Name = [%s]  From Pid = [%d]",
        surfaceName.c_str(), pid);
    frameTimeMap_[validSurfaceName] = timestamp;
}

bool HgmIdleDetector::GetUnknownFrameworkState(const std::string& surfaceName,
    std::string& uiFwkType)
{
    for (auto& supportedAppBuffer : supportAppBufferList_) {
        if (surfaceName.rfind(supportedAppBuffer, 0) == 0) {
            uiFwkType = supportedAppBuffer;
            return true;
        }
    }
    return false;
}

bool HgmIdleDetector::GetSurfaceFrameworkState(const std::string& surfaceName,
    std::string& validSurfaceName)
{
    for (auto& supportedAppBuffer : supportAppBufferList_) {
        if (surfaceName.rfind(supportedAppBuffer, 0) == 0) {
            validSurfaceName = supportedAppBuffer;
            return true;
        }
    }
    if (std::count(supportAppBufferList_.begin(), supportAppBufferList_.end(), OTHER_SURFACE)) {
        validSurfaceName = OTHER_SURFACE;
        return true;
    }
    return false;
}

bool HgmIdleDetector::GetSurfaceIdleState(uint64_t timestamp)
{
    if (frameTimeMap_.empty()) {
        return true;
    }

    bool idle = true;
    for (auto iter = frameTimeMap_.begin(); iter != frameTimeMap_.end();) {
        if ((timestamp - iter->second) > BUFFER_IDLE_TIME_OUT) {
            iter = frameTimeMap_.erase(iter);
        } else {
            idle = false;
            ++iter;
        }
    }

    return idle;
}

bool HgmIdleDetector::ThirdFrameNeedHighRefresh()
{
    if (appBufferBlackList_.empty() || (!aceAnimatorIdleState_ &&
        !std::count(appBufferBlackList_.begin(), appBufferBlackList_.end(), ACE_ANIMATOR_NAME))) {
        return true;
    }

    if (frameTimeMap_.empty() ||
        std::count(appBufferBlackList_.begin(), appBufferBlackList_.end(), OTHER_SURFACE)) {
        return false;
    }

    for (auto &[surfaceName, _] : frameTimeMap_) {
        if (!std::count(appBufferBlackList_.begin(), appBufferBlackList_.end(), surfaceName)) {
            return true;
        }
    }
    return false;
}

int32_t HgmIdleDetector::GetTouchUpExpectedFPS()
{
    if (appBufferList_.empty()) {
        return GetAceAnimatorExpectedFrameRate() > ANIMATOR_NO_EXPECTED_FRAME_RATE ? GetAceAnimatorExpectedFrameRate()
                                                                                   : FPS_MAX;
    }
    if (!aceAnimatorIdleState_) {
        auto iter = std::find_if(appBufferList_.begin(), appBufferList_.end(),
            [&](const auto& appBuffer) {
            return appBuffer.first == ACE_ANIMATOR_NAME;
        });
        if (iter != appBufferList_.end() && frameTimeMap_.empty()) {
            return GetAceAnimatorExpectedFrameRate() > ANIMATOR_NO_EXPECTED_FRAME_RATE
                    ? std::min(GetAceAnimatorExpectedFrameRate(), iter->second)
                    : iter->second;
        }
    }

    for (auto &[surfaceName, _] : frameTimeMap_) {
        auto iter = std::find_if(appBufferList_.begin(), appBufferList_.end(),
            [&surfaceName = surfaceName](const std::pair<std::string, int32_t>& appBuffer) {
            return appBuffer.first == surfaceName;
        });
        if (iter == appBufferList_.end()) {
            return FPS_MAX;
        }
    }

    for (auto &[surfaceName, touchUpExpectedFPS] : appBufferList_) {
        if ((surfaceName == ACE_ANIMATOR_NAME && !aceAnimatorIdleState_) ||
            frameTimeMap_.count(surfaceName)) {
            return touchUpExpectedFPS;
        }
    }

    return FPS_MAX;
}

} // namespace Rosen
} // namespace OHOS