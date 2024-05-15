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

#ifndef FRAMEWORKS_BOOTANIMATION_INCLUDE_BOOT_ANIMATION_CONFIG_H
#define FRAMEWORKS_BOOTANIMATION_INCLUDE_BOOT_ANIMATION_CONFIG_H

#include <string>
#include <vector>

#include "screen_manager/screen_types.h"

namespace OHOS {
struct BootAnimationConfig {
    std::string picZipPath;
    std::string soundPath;
    std::string videoDefaultPath;
    std::string videoExtraPath;
    int32_t rotateScreenId = -1;
    int32_t rotateDegree = 0;
    Rosen::ScreenId screenId = Rosen::INVALID_SCREEN_ID;
    std::vector<std::string> videoExtPath;
};
} // namespace OHOS

#endif // FRAMEWORKS_BOOTANIMATION_INCLUDE_BOOT_ANIMATION_CONFIG_H
