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

#include "boot_independent_display_strategy.h"

#include "log.h"
#include "transaction/rs_interfaces.h"

using namespace OHOS;
void BootIndependentDisplayStrategy::Display(int32_t duration, std::vector<BootAnimationConfig>& configs)
{
    LOGI("BootIndependentDisplayStrategy START");
    GetConnectToRenderMap(configs.size());
    Rosen::RSInterfaces& interface = Rosen::RSInterfaces::GetInstance();
    BootAnimationConfig screenConfig;
    for (const auto& config : configs) {
        if (connectToRenderMap_.find(config.screenId) == connectToRenderMap_.end()) {
            LOGE("screen is not prepare:" BPUBU64 "", config.screenId);
            continue;
        }
        interface.SetScreenPowerStatus(config.screenId, Rosen::ScreenPowerStatus::POWER_STATUS_ON);
        Rosen::RSScreenModeInfo modeInfo = interface.GetScreenActiveMode(config.screenId);
        int screenWidth = modeInfo.GetScreenWidth();
        int screenHeight = modeInfo.GetScreenHeight();
        std::shared_ptr<BootAnimationOperation> op = std::make_shared<BootAnimationOperation>();
        Rosen::ScreenId defaultId = interface.GetDefaultScreenId();
        if (config.screenId != defaultId) {
            op->SetSoundEnable(false);
        } else {
            screenConfig = config;
        }
        op->Init(config, screenWidth, screenHeight, duration, connectToRenderMap_.find(config.screenId)->second);
        operators_.emplace_back(op);
    }

    for (const auto& op : operators_) {
        if (op->GetThread().joinable()) {
            op->GetThread().join();
        }
    }

    if (IsOtaUpdate() && connectToRenderMap_.find(screenConfig.screenId) != connectToRenderMap_.end()) {
        bootCompileProgress_ = std::make_shared<BootCompileProgress>();
        bootCompileProgress_->Init(configPath_, screenConfig, connectToRenderMap_.find(screenConfig.screenId)->second);
    }

    while (!CheckExitAnimation()) {
        usleep(SLEEP_TIME_US);
    }
}
