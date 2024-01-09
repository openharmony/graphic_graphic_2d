/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef FRAMEWORKS_BOOTANIMATION_INCLUDE_BOOT_ANIMATIONCONFIG_H
#define FRAMEWORKS_BOOTANIMATION_INCLUDE_BOOT_ANIMATIONCONFIG_H

#include "util.h"

namespace OHOS {
class BootAnimationConfig {
public:
    bool ReadPicZipFile(ImageStructVec& vec, int32_t& freq);
    bool CheckFrameRateValid(int32_t ratevalue);
    void CheckBootVideoEnabled();
    void ReadPicZipFile();
    std::string GetPicZipPath();
    std::string GetSoundUrl();
    std::string GetCustomCfgFile();
    std::string GetBootVideoPath();
    std::string GetBootExtraVideoPath();
    int32_t GetRotateScreenId();
    int32_t GetRotateDegree();
    void ParserCustomCfgFile();
    bool IsBootVideoEnabled();
private:
    BootCustomConfig custConfig_;
    bool bootVideoEnabled_ = true;
};
} // namespace OHOS

#endif // FRAMEWORKS_BOOTANIMATION_INCLUDE_BOOT_ANIMATIONCONFIG_H
