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

#include <functional>
#include <string>
#include <unordered_map>

namespace OHOS::Rosen {
class RsCommonHook {
public:
    static RsCommonHook& Instance();
    void RegisterStartNewAnimationListener(std::function<void()> listener);
    void OnStartNewAnimation();
    // source crop tuning
    void SetVideoSurfaceFlag(bool videoSurfaceFlag);
    bool GetVideoSurfaceFlag() const;

    // use in updating hwcnode hardware state with background alpha
    void SetHardwareEnabledByHwcnodeBelowSelfInAppFlag(bool hardwareEnabledByHwcNodeSkippedFlag);
    void SetHardwareEnabledByBackgroundAlphaFlag(bool hardwareEnabledByBackgroundAlphaSkippedFlag);
    bool GetHardwareEnabledByHwcnodeBelowSelfInAppFlag() const;
    bool GetHardwareEnabledByBackgroundAlphaFlag() const;

    // temporary code: ignore below current hardware composer node rect size
    void SetHardwareEnabledByIgnoringInsideFlag(bool hardwareEnabledByIgnoringInsideFlag);
    bool GetHardwareEnabledByIgnoringInsideFlag() const;

private:
    std::function<void()> startNewAniamtionFunc_ = nullptr;
    // source crop tuning
    bool videoSurfaceFlag_ = false;

    // use in updating hwcnode hardware state with background alpha
    bool hardwareEnabledByHwcnodeSkippedFlag_ = false;
    bool hardwareEnabledByBackgroundAlphaSkippedFlag_ = false;

    // temporary code: ignore below current hardware composer node rect size
    bool hardwareEnabledByIgnoringInsideFlag_ = false;
};
} // namespace OHOS::Rosen
#endif