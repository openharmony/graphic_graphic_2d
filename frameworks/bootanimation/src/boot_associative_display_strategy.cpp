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
1
#include "boot_associative_display_strategy.h"

#include "log.h"
#include "transaction/rs_interfaces.h"
#include <parameters.h>

using namespace OHOS;

namespace {
    const bool IS_COORDINATION_SUPPORT =
        system::GetBoolParameter("const.window.foldabledevice.is_coordination_support", false);
    const std::string FOLD_SCREEN_TYPE = system::GetParameter("const.window.foldscreen.type", "");
}

void BootAssociativeDisplayStrategy::Display(int32_t duration, std::vector<BootAnimationConfig>& configs)
{
    LOGI("BootAssociativeDisplayStrategy START");
    if (configs.size() <= 1) {
        LOGE("config size error");
        return;
    }

    if (IsExtraVideoExist(configs)) {
        LOGI("not support play extra video for multi screen now");
        return;
    }

    Rosen::RSInterfaces& interface = Rosen::RSInterfaces::GetInstance();
    Rosen::ScreenId defaultId = interface.GetDefaultScreenId();
    Rosen::ScreenId activeId = interface.GetActiveScreenId();
    LOGI("defaultId: " BPUBU64 ", activeId: " BPUBU64 "", defaultId, activeId);
    bool isSupportCoordination = IsSupportCoordination();
    LOGI("isSupportCoordination = %{public}d", isSupportCoordination);
    if (defaultId != activeId && isSupportCoordination) {
        LOGD("SetScreenPowerStatus POWER_STATUS_OFF_FAKE: " BPUBU64 "", defaultId);
        interface.SetScreenPowerStatus(defaultId, Rosen::ScreenPowerStatus::POWER_STATUS_OFF_FAKE);
        LOGD("SetScreenPowerStatus POWER_STATUS_ON: " BPUBU64 "", activeId);
        interface.SetScreenPowerStatus(activeId, Rosen::ScreenPowerStatus::POWER_STATUS_ON);
    }

    for (const auto& config : configs) {
        if (config.screenId != activeId) {
            continue;
        }

        Rosen::RSScreenModeInfo modeInfo = interface.GetScreenActiveMode(config.screenId);
        int screenWidth = modeInfo.GetScreenWidth();
        int screenHeight = modeInfo.GetScreenHeight();
        operator_ = std::make_shared<BootAnimationOperation>();
        operator_->Init(config, screenWidth, screenHeight, duration);
        if (operator_->GetThread().joinable()) {
            operator_->GetThread().join();
        }

        if (IsOtaUpdate()) {
            bootCompileProgress_ = std::make_shared<BootCompileProgress>();
            bootCompileProgress_->Init(config);
        }

        while (!CheckExitAnimation()) {
            usleep(SLEEP_TIME_US);
        }
    }
}

bool BootAssociativeDisplayStrategy::IsExtraVideoExist(const std::vector<BootAnimationConfig>& configs)
{
    for (const auto& config : configs) {
        if (config.videoExtPath.size() != 0) {
            return true;
        }
    }
    return false;
}

bool BootAssociativeDisplayStrategy::IsSupportCoordination()
{
    // only psd return false
    return IS_COORDINATION_SUPPORT || !(!FOLD_SCREEN_TYPE.empty() && FOLD_SCREEN_TYPE[0] == '2');
}