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
#include <mutex>
#include <string>
#include <unistd.h>
#include <unordered_map>
#include <unordered_set>

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

    void SetTvPlayerBundleName(const std::string& bundleName);
    const std::string& GetTvPlayerBundleName() const;

    void SetFilterUnderHwcConfigByApp(const std::string& appName, const std::string& val);
    std::string_view GetFilterUnderHwcConfigByApp(const std::string& appName);

    // use in updating hwcnode hardware state with background alpha
    void SetHardwareEnabledByHwcnodeBelowSelfInAppFlag(bool hardwareEnabledByHwcNodeSkippedFlag);
    void SetHardwareEnabledByBackgroundAlphaFlag(bool hardwareEnabledByBackgroundAlphaSkippedFlag);
    bool GetHardwareEnabledByHwcnodeBelowSelfInAppFlag() const;
    bool GetHardwareEnabledByBackgroundAlphaFlag() const;
    void SetComponentPowerFpsFunc(std::function<void(FrameRateRange& range)> func);
    void GetComponentPowerFps(FrameRateRange& range);
    bool GetIsWhiteListForSolidColorLayerFlag() const;
    void SetIsWhiteListForSolidColorLayerFlag(bool isWhiteListForSolidColorLayerFlag);

    void SetSolidColorLayerConfigFromHgm(
        const std::unordered_map<std::string, std::string>& solidLayerConfigFromHgm);
    void SetHwcSolidColorLayerConfigFromHgm(
        const std::unordered_map<std::string, std::string>& hwcSolidLayerConfigFromHgm);

    bool IsSolidColorLayerConfig(const std::string& bundleName);
    bool IsHwcSolidColorLayerConfig(const std::string& bundleName);

    void SetOverlappedHwcNodeInAppEnabledConfig(const std::string& appName, const std::string& val);
    std::string GetOverlappedHwcNodeInAppEnabledConfig(const std::string& appName);

    // DISPLAY ENGINE
    void SetCurrentPkgName(const std::string& pkgName);
    std::string GetCurrentPkgName() const;
    void SetImageEnhancePidList(const std::unordered_set<pid_t>& imageEnhancePidList);
    std::unordered_set<pid_t> GetImageEnhancePidList() const;

private:
    std::function<void(const std::string&)> startNewAniamtionFunc_ = nullptr;
    // source crop tuning
    std::atomic<bool> videoSurfaceFlag_{false};

    // use to implement product isolation for the adaptive P3 scheme
    std::atomic<bool> isAdaptiveColorGamutEnable_{false};

    std::string tvPlayerBundleName_;

    std::unordered_map<std::string, std::string> filterUnderHwcConfig_;

    std::unordered_map<std::string, std::string> overlappedHwcNodeInAppEnabledConfig_;

    // use in updating hwc node hardware state with background alpha
    std::atomic<bool> hardwareEnabledByHwcnodeSkippedFlag_{false};
    std::atomic<bool> hardwareEnabledByBackgroundAlphaSkippedFlag_{false};
    std::atomic<bool> isWhiteListForSolidColorLayerFlag_{false};
    
    std::function<void(FrameRateRange& range)> componentPowerFpsFunc_ = nullptr;

    std::unordered_map<std::string, std::string> solidLayerConfigFromHgm_;
    std::unordered_map<std::string, std::string> hwcSolidLayerConfigFromHgm_;
    
    // DISPLAY ENGINE
    std::string pkgName_{};
    mutable std::mutex setMutex_{};
    std::unordered_set<pid_t> imageEnhancePidList_{};
};
} // namespace OHOS::Rosen
#endif