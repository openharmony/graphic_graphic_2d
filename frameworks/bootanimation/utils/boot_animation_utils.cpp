/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "boot_animation_utils.h"
#include "log.h"
#include "parameters.h"

namespace OHOS {
bool BootAnimationUtils::GetBootAnimationSoundEnabled()
{
    static bool soundEnabled =
        std::atoi((system::GetParameter("persist.graphic.bootsound.enabled", "1")).c_str()) != 0;
    LOGI("BootAnimationUtils::GetBootAnimationSoundEnabled is %d", soundEnabled);
    return soundEnabled;
}

void BootAnimationUtils::SetBootAnimationSoundEnabled(bool isEnabled)
{
    LOGI("BootAnimationUtils::SetBootAnimationSoundEnabled is %d", isEnabled);
    system::SetParameter("persist.graphic.bootsound.enabled", isEnabled ? "1" : "0");
}
} // namespace OHOS
