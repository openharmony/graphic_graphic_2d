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

#include <string>
#include <unordered_map>
#include <vector>
#include "pipeline/rs_render_node.h"
namespace OHOS {
namespace Rosen {

enum class UIFWKType : int32_t {
    FROM_UNKNOWN = 0,
    FROM_SURFACE = 1,
};
constexpr uint32_t ACE_ANIMATOR_OFFSET = 16;

class HgmIdleDetector {
public:
    static constexpr int32_t ANIMATOR_NOT_RUNNING = -1;

    HgmIdleDetector() = default;
    ~HgmIdleDetector() = default;

    void SetAppSupportedState(bool appSupported);
    bool GetAppSupportedState() const
    {
        return appSupported_;
    }

    void SetBufferFpsMap(std::unordered_map<std::string, int32_t> bufferFpsMap)
    {
        bufferFpsMap_ = std::move(bufferFpsMap);
    }

    int32_t GetTouchUpExpectedFPS();

    void SetAceAnimatorIdleState(bool aceAnimatorIdleState);
    bool GetAceAnimatorIdleState() const
    {
        return aceAnimatorIdleState_;
    }

    void UpdateAceAnimatorExpectedFrameRate(int32_t aceAnimatorExpectedFrameRate);

    void ResetAceAnimatorExpectedFrameRate()
    {
        aceAnimatorExpectedFrameRate_ = ANIMATOR_NOT_RUNNING;
    }

    void UpdateSurfaceTime(const std::string& surfaceName, uint64_t timestamp,
        pid_t pid, UIFWKType uiFwkType = UIFWKType::FROM_UNKNOWN);

    void UpdateSurfaceState(uint64_t timestamp);

    bool GetSurfaceIdleState() const
    {
        return surfaceTimeMap_.empty();
    }

private:
    bool GetUnknownFrameworkState(const std::string& surfaceName,
        std::string& uiFwkType);
    bool GetSurfaceFrameworkState(const std::string& surfaceName,
        std::string& validSurfaceName);
    bool appSupported_ = false;
    bool aceAnimatorIdleState_ = true;
    int32_t aceAnimatorExpectedFrameRate_ = ANIMATOR_NOT_RUNNING;

    // FORMAT: <buffername, time>
    std::unordered_map<std::string, uint64_t> surfaceTimeMap_;
    // FORMAT: <buffername, fps>
    std::unordered_map<std::string, int32_t> bufferFpsMap_;
};
} // namespace Rosen
} // namespace OHOS
#endif // HGM_IDLE_DETECTOR_H
