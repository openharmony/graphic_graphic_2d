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

void RsCommonHook::RegisterStartNewAnimationListener(std::function<void()> listener)
{
    startNewAniamtionFunc_ = listener;
}

void RsCommonHook::OnStartNewAnimation()
{
    if (startNewAniamtionFunc_) {
        startNewAniamtionFunc_();
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

// temporary code: ignore below current hardware composer node rect size BEGIN
void RsCommonHook::SetHardwareEnabledByIgnoringInsideFlag(bool hardwareEnabledByIgnoringInsideFlag)
{
    hardwareEnabledByIgnoringInsideFlag_ = hardwareEnabledByIgnoringInsideFlag;
}

bool RsCommonHook::GetHardwareEnabledByIgnoringInsideFlag() const
{
    return hardwareEnabledByIgnoringInsideFlag_;
}
// temporary code: ignore below current hardware composer node rect size END

} // namespace OHOS::Rosen