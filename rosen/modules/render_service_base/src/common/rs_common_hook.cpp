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

#include "common/rs_common_hook.h"

namespace OHOS::Rosen {
RsCommonHook& RsCommonHook::Instance()
{
    static RsCommonHook rch;
    return rch;
}

void RsCommonHook::RegisterStartNewAnimationListener(std::function<void(const std::string &)> listener)
{
    startNewAniamtionFunc_ = listener;
}

void RsCommonHook::OnStartNewAnimation(const std::string &componentName)
{
    if (startNewAniamtionFunc_) {
        startNewAniamtionFunc_(componentName);
    }
}

void RsCommonHook::SetVideoSurfaceFlag(bool videoSurfaceFlag)
{
    videoSurfaceFlag_ = videoSurfaceFlag;
}

bool RsCommonHook::GetVideoSurfaceFlag() const
{
    return videoSurfaceFlag_;
}

// use to implement product isolation for the adaptive P3 scheme
void RsCommonHook::SetAdaptiveColorGamutEnable(bool isAdaptiveColorGamutEnable)
{
    isAdaptiveColorGamutEnable_ = isAdaptiveColorGamutEnable;
}

bool RsCommonHook::IsAdaptiveColorGamutEnabled() const
{
    return isAdaptiveColorGamutEnable_;
}

// skip hwcnode hardware state updating
void RsCommonHook::SetHardwareEnabledByHwcnodeBelowSelfInAppFlag(bool hardwareEnabledByHwcnodeSkippedFlag)
{
    hardwareEnabledByHwcnodeSkippedFlag_ = hardwareEnabledByHwcnodeSkippedFlag;
}

void RsCommonHook::SetHardwareEnabledByBackgroundAlphaFlag(bool hardwareEnabledByBackgroundAlphaSkippedFlag)
{
    hardwareEnabledByBackgroundAlphaSkippedFlag_ = hardwareEnabledByBackgroundAlphaSkippedFlag;
}

bool RsCommonHook::GetHardwareEnabledByHwcnodeBelowSelfInAppFlag() const
{
    return hardwareEnabledByHwcnodeSkippedFlag_;
}

bool RsCommonHook::GetHardwareEnabledByBackgroundAlphaFlag() const
{
    return hardwareEnabledByBackgroundAlphaSkippedFlag_;
}

bool RsCommonHook::GetIsWhiteListForSolidColorLayerFlag() const
{
    return isWhiteListForSolidColorLayerFlag_;
}
    
void RsCommonHook::SetIsWhiteListForSolidColorLayerFlag(bool isWhiteListForSolidColorLayerFlag)
{
    isWhiteListForSolidColorLayerFlag_ = isWhiteListForSolidColorLayerFlag;
}

void RsCommonHook::SetComponentPowerFpsFunc(std::function<void(FrameRateRange& range)> func)
{
    componentPowerFpsFunc_ = func;
}

void RsCommonHook::GetComponentPowerFps(FrameRateRange& range)
{
    if (componentPowerFpsFunc_) {
        componentPowerFpsFunc_(range);
    }
}

bool RsCommonHook::GetIsWhiteListForEnableHwcNodeBelowSelfInApp() const
{
    return isWhiteListForEnableHwcNodeBelowSelfInApp_;
}

void RsCommonHook::SetIsWhiteListForEnableHwcNodeBelowSelfInApp(bool isWhiteListForEnableHwcNodeBelowSelfInApp)
{
    isWhiteListForEnableHwcNodeBelowSelfInApp_ = isWhiteListForEnableHwcNodeBelowSelfInApp;
}

} // namespace OHOS::Rosen