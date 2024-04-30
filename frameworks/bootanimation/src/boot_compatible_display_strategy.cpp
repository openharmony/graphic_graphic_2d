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

#include "boot_compatible_display_strategy.h"

#include "log.h"
#include "transaction/rs_interfaces.h"

using namespace OHOS;

void BootCompatibleDisplayStrategy::Display(std::vector<BootAnimationConfig>& configs)
{
    LOGI("BootCompatibleDisplayStrategy START");
    if (configs.size() != 1) {
        LOGE("config size error");
        return;
    }

    Rosen::RSInterfaces& interface = Rosen::RSInterfaces::GetInstance();
    for (auto& config : configs) {
        config.screenId = interface.GetDefaultScreenId();
        if (config.rotateScreenId >= 0) {
            config.screenId = interface.GetActiveScreenId();
            if (config.screenId > 0) {
                interface.SetScreenPowerStatus(0, Rosen::ScreenPowerStatus::POWER_STATUS_OFF_FAKE);
                interface.SetScreenPowerStatus(config.screenId, Rosen::ScreenPowerStatus::POWER_STATUS_ON);
                config.rotateDegree = 0;
                config.videoDefaultPath = config.videoExtraPath;
            }
        } else {
            interface.SetScreenPowerStatus(config.screenId, Rosen::ScreenPowerStatus::POWER_STATUS_ON);
        }

        Rosen::RSScreenModeInfo modeInfo = interface.GetScreenActiveMode(config.screenId);
        int32_t screenWidth = modeInfo.GetScreenWidth();
        int32_t screenHeight = modeInfo.GetScreenHeight();
        operator_ = std::make_shared<BootAnimationOperation>();
        operator_->Init(config, screenWidth, screenHeight);
        operator_->GetThread().join();

        if (CheckNeedOtaCompile()) {
            bootCompileProgress_ = std::make_shared<BootCompileProgress>();
            bootCompileProgress_->Init(config);
        }
    }

    while (!CheckExitAnimation()) {
        usleep(SLEEP_TIME_US);
    }
}
