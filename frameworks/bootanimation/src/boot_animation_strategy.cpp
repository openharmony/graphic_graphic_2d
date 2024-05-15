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

#include "boot_animation_strategy.h"

#include "log.h"
#include <parameters.h>
#include "util.h"

namespace OHOS {
namespace {
    constexpr const char* DUE_UPDATE_TYPE_PARAM = "persist.dupdate_engine.update_type";
    const std::string DUE_UPDATE_TYPE_MANUAL = "manual";
    const std::string DUE_UPDATE_TYPE_NIGHT = "night";
    constexpr const char* OTA_BMS_COMPILE_SWITCH = "const.bms.optimizing_apps.switch";
    const std::string OTA_BMS_COMPILE_SWITCH_OFF = "off";
    const std::string OTA_BMS_COMPILE_SWITCH_ON = "on";
}

bool BootAnimationStrategy::CheckExitAnimation()
{
    if (!isAnimationEnd_) {
        LOGI("boot animation is end");
        system::SetParameter(BOOT_ANIMATION_FINISHED, "true");
        isAnimationEnd_ = true;
    }
    bool bootEventCompleted = system::GetBoolParameter(BOOT_COMPLETED, false);
    if (bootEventCompleted) {
        LOGI("read boot completed is true");
        return true;
    }
    return false;
}

bool BootAnimationStrategy::CheckNeedOtaCompile() const
{
    LOGI("CheckNeedOtaCompile");
    std::string otaCompileSwitch = system::GetParameter(OTA_BMS_COMPILE_SWITCH, OTA_BMS_COMPILE_SWITCH_OFF);
    if (otaCompileSwitch != OTA_BMS_COMPILE_SWITCH_ON) {
        LOGI("ota compile switch is: %{public}s", otaCompileSwitch.c_str());
        return false;
    }

    std::string dueUpdateType = system::GetParameter(DUE_UPDATE_TYPE_PARAM, "");
    LOGI("dueUpdateType is: %{public}s", dueUpdateType.c_str());
    bool isOtaUpdate = dueUpdateType == DUE_UPDATE_TYPE_MANUAL || dueUpdateType == DUE_UPDATE_TYPE_NIGHT;

    std::string bmsCompileStatus  = system::GetParameter(BMS_COMPILE_STATUS, "-1");
    LOGI("bmsCompileStatus is: %{public}s", bmsCompileStatus.c_str());
    bool isCompileDone = bmsCompileStatus == BMS_COMPILE_STATUS_END;

    if (isOtaUpdate && !isCompileDone) {
        return true;
    }
    return false;
}
} // namespace OHOS
