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
constexpr uint32_t ACE_ANIMATOR_OFFSET = 16;
constexpr uint64_t BUFFER_IDLE_TIME_OUT = 200000000; // 200ms
constexpr uint64_t MAX_CACHED_VALID_SURFACE_NAME_COUNT = 60;
constexpr int32_t ANIMATOR_NO_EXPECTED_FRAME_RATE = 0;
const std::string ACE_ANIMATOR_NAME = "AceAnimato";
const std::string OTHER_SURFACE = "Other_SF";
}

void HgmIdleDetector::SetAppSupportedState(bool appSupported)
{
    appSupported_ = appSupported;
    if (!appSupported_) {
        aceAnimatorIdleState_ = true;
        aceAnimatorExpectedFrameRate_ = ANIMATOR_NOT_RUNNING;
        surfaceTimeMap_.clear();
        bufferFpsMap_.clear();
    }
}

void HgmIdleDetector::SetAceAnimatorIdleState(bool aceAnimatorIdleState)
{
    if (!appSupported_ || bufferFpsMap_.find(ACE_ANIMATOR_NAME) == bufferFpsMap_.end() ||
        bufferFpsMap_[ACE_ANIMATOR_NAME] == 0) {
        // only if bufferFpsMap_ contains ACE_ANIMATOR_NAME, aceAnimatorIdleState_ can be set to false
        aceAnimatorIdleState_ = true;
        return;
    }

    aceAnimatorIdleState_ = aceAnimatorIdleState;
}

void HgmIdleDetector::UpdateSurfaceTime(const std::string& surfaceName, uint64_t timestamp,
    pid_t pid, UIFWKType uiFwkType)
{
    if (surfaceName.empty()) {
        return;
    }

    if (!GetAppSupportedState() || surfaceTimeMap_.size() > MAX_CACHED_VALID_SURFACE_NAME_COUNT) {
        surfaceTimeMap_.clear();
        return;
    }

    std::string validSurfaceType = "";
    bool hasValidFramework = false;
    switch (uiFwkType) {
        case UIFWKType::FROM_UNKNOWN:
            hasValidFramework = GetUnknownFrameworkState(surfaceName, validSurfaceType);
            break;
        case UIFWKType::FROM_SURFACE:
            hasValidFramework = GetSurfaceFrameworkState(surfaceName, validSurfaceType);
            break;
        default:
            break;
    }
    if (!hasValidFramework) {
        return;
    }

    RS_TRACE_NAME_FMT("UpdateSurfaceTime:: Not Idle SurFace Name = [%s]  From Pid = [%d]",
        surfaceName.c_str(), pid);
    // only types contained by bufferFpsMap_ can be add to surfaceTimeMap_
    surfaceTimeMap_[validSurfaceType] = timestamp;
}

bool HgmIdleDetector::GetUnknownFrameworkState(const std::string& surfaceName,
    std::string& uiFwkType)
{
    for (const auto& [supportedAppBuffer, fps] : bufferFpsMap_) {
        if (surfaceName.rfind(supportedAppBuffer, 0) == 0) {
            if (fps > 0) {
                uiFwkType = supportedAppBuffer;
                return true;
            } else {
                return false;
            }
        }
    }
    return false;
}

bool HgmIdleDetector::GetSurfaceFrameworkState(const std::string& surfaceName,
    std::string& validSurfaceName)
{
    for (const auto& [supportedAppBuffer, fps] : bufferFpsMap_) {
        if (surfaceName.rfind(supportedAppBuffer, 0) == 0) {
            if (fps > 0) {
                validSurfaceName = supportedAppBuffer;
                return true;
            } else {
                return false;
            }
        }
    }
    if (bufferFpsMap_.find(OTHER_SURFACE) != bufferFpsMap_.end() &&
        bufferFpsMap_[OTHER_SURFACE] > 0) {
        validSurfaceName = OTHER_SURFACE;
        return true;
    }
    return false;
}

void HgmIdleDetector::UpdateSurfaceState(uint64_t timestamp)
{
    for (auto iter = surfaceTimeMap_.begin(); iter != surfaceTimeMap_.end();) {
        if (timestamp > iter->second && (timestamp - iter->second) > BUFFER_IDLE_TIME_OUT) {
            iter = surfaceTimeMap_.erase(iter);
        } else {
            ++iter;
        }
    }
}

int32_t HgmIdleDetector::GetTouchUpExpectedFPS()
{
    if (aceAnimatorIdleState_ && surfaceTimeMap_.empty()) {
        return 0;
    }

    int32_t aceAnimatorVote = 0;
    if (!aceAnimatorIdleState_) {
        // only if bufferFpsMap_ contains ACE_ANIMATOR_NAME, aceAnimatorIdleState_ can be set to false
        aceAnimatorVote = aceAnimatorExpectedFrameRate_ > ANIMATOR_NO_EXPECTED_FRAME_RATE
                            ? std::min(aceAnimatorExpectedFrameRate_, bufferFpsMap_[ACE_ANIMATOR_NAME])
                            : bufferFpsMap_[ACE_ANIMATOR_NAME];
    }

    int32_t surfaceVote = 0;
    for (const auto& [surfaceName, _] : surfaceTimeMap_) {
        // only types contained by bufferFpsMap_ can be add to surfaceTimeMap_
        surfaceVote = std::max(surfaceVote, bufferFpsMap_[surfaceName]);
    }

    RS_TRACE_NAME_FMT("GetTouchUpExpectedFPS: aceAnimatorVote:[%d], surfaceVote:[%d]", aceAnimatorVote, surfaceVote);
    return std::max(aceAnimatorVote, surfaceVote);
}

void HgmIdleDetector::UpdateAceAnimatorExpectedFrameRate(int32_t aceAnimatorExpectedFrameRate)
{
    // aceAnimatorExpectedFrameRate int32_t  example: 0x003c0001
    // [0, 16) is existAnimatorNoExpectdRate = 1
    // [16, 32) is aceAnimatorExpectedFrameRate = 60
    if (aceAnimatorExpectedFrameRate < 0) {
        return;
    }
    bool existAnimatorNoExpectdRate = aceAnimatorExpectedFrameRate & 1;
    auto iter = bufferFpsMap_.find(ACE_ANIMATOR_NAME);
    if (iter != bufferFpsMap_.end() && iter->second > 0 && existAnimatorNoExpectdRate) {
        aceAnimatorExpectedFrameRate = 0;
    } else {
        aceAnimatorExpectedFrameRate = aceAnimatorExpectedFrameRate >> ACE_ANIMATOR_OFFSET;
    }

    if (aceAnimatorExpectedFrameRate_ != 0 &&
        (aceAnimatorExpectedFrameRate == 0 || aceAnimatorExpectedFrameRate > aceAnimatorExpectedFrameRate_)) {
        aceAnimatorExpectedFrameRate_ = aceAnimatorExpectedFrameRate;
    }
}
} // namespace Rosen
} // namespace OHOS