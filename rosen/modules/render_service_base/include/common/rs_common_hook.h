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

#ifndef RS_COMMON_HOOK_H
#define RS_COMMON_HOOK_H

#include <atomic>
#include <functional>
#include <string>
#include <unordered_map>

#include "animation/rs_frame_rate_range.h"

namespace OHOS::Rosen {
class RsCommonHook {
public:
    static RsCommonHook& Instance();
    void RegisterStartNewAnimationListener(std::function<void(const std::string&)> listener);
    void OnStartNewAnimation(const std::string &componentName);
    // source crop tuning
    void SetVideoSurfaceFlag(bool videoSurfaceFlag);
    bool GetVideoSurfaceFlag() const;

    // use to implement product isolation for the adaptive P3 scheme
    void SetAdaptiveColorGamutEnable(bool isAdaptiveColorGamutEnable);
    bool IsAdaptiveColorGamutEnabled() const;

    // use in updating hwcnode hardware state with background alpha
    void SetHardwareEnabledByHwcnodeBelowSelfInAppFlag(bool hardwareEnabledByHwcNodeSkippedFlag);
    void SetHardwareEnabledByBackgroundAlphaFlag(bool hardwareEnabledByBackgroundAlphaSkippedFlag);
    bool GetHardwareEnabledByHwcnodeBelowSelfInAppFlag() const;
    bool GetHardwareEnabledByBackgroundAlphaFlag() const;
    void SetComponentPowerFpsFunc(std::function<void(FrameRateRange& range)> func);
    void GetComponentPowerFps(FrameRateRange& range);
    bool GetIsWhiteListForSolidColorLayerFlag() const;
    void SetIsWhiteListForSolidColorLayerFlag(bool isWhiteListForSolidColorLayerFlag);
    bool GetIsWhiteListForEnableHwcNodeBelowSelfInApp() const;
    void SetIsWhiteListForEnableHwcNodeBelowSelfInApp(bool isWhiteListForEnableHwcNodeBelowSelfInApp);

private:
    std::function<void(const std::string&)> startNewAniamtionFunc_ = nullptr;
    // source crop tuning
    std::atomic<bool> videoSurfaceFlag_{false};

    // use to implement product isolation for the adaptive P3 scheme
    std::atomic<bool> isAdaptiveColorGamutEnable_{false};

    // use in updating hwc node hardware state with background alpha
    std::atomic<bool> hardwareEnabledByHwcnodeSkippedFlag_{false};
    std::atomic<bool> hardwareEnabledByBackgroundAlphaSkippedFlag_{false};
    std::atomic<bool> isWhiteListForSolidColorLayerFlag_{false};
    // use in updating hwc node hardware state with hwc node below self in app
    std::atomic<bool> isWhiteListForEnableHwcNodeBelowSelfInApp_{false};
    
    std::function<void(FrameRateRange& range)> componentPowerFpsFunc_ = nullptr;
};
} // namespace OHOS::Rosen
#endif