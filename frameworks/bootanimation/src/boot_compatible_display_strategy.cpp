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
#include "util.h"

using namespace OHOS;

namespace {
    constexpr uint32_t HDMI_POLL_MAX_RETRIES = 5;
    constexpr uint32_t HDMI_POLL_INTERVAL_US = 20000;
}

bool BootCompatibleDisplayStrategy::PrepareScreenConfig(BootAnimationConfig& config)
{
    Rosen::RSInterfaces& interface = Rosen::RSInterfaces::GetInstance();
    Rosen::ScreenId firstScreenId = GetFirstScreenId();
    if (firstScreenId == Rosen::INVALID_SCREEN_ID) {
        LOGE("connectToRenderMap_ is empty, cannot prepare screen config");
        return false;
    }
    config.screenId = firstScreenId;
    if (GetDeviceType() == DEVICE_TYPE_2IN1) {
        config.screenId = SelectHdmiScreen(firstScreenId);
    }
    if (config.rotateScreenId >= 0) {
        SubscribeActiveScreenIdChanged();
        Rosen::ScreenId activeScreenId = GetActiveScreenId();
        if (activeScreenId != Rosen::INVALID_SCREEN_ID) {
            config.screenId = activeScreenId;
        }
        if (config.screenId > 0) {
            interface.SetScreenPowerStatus(0, Rosen::ScreenPowerStatus::POWER_STATUS_OFF_FAKE);
            interface.SetScreenPowerStatus(config.screenId, Rosen::ScreenPowerStatus::POWER_STATUS_ON);
            config.rotateDegree = 0;
            config.videoDefaultPath = config.videoExtraPath;
        }
    } else {
        interface.SetScreenPowerStatus(config.screenId, Rosen::ScreenPowerStatus::POWER_STATUS_ON);
    }
    if (!config.videoExtPath.empty()) {
        std::string status = GetHingeStatus();
        auto iter = config.videoExtPath.find(status);
        if (iter != config.videoExtPath.end()) {
            config.videoDefaultPath = iter->second;
            config.screenStatus = status;
        }
    }
    if (!HasScreenId(config.screenId)) {
        LOGE("screen is not prepare:" BPUBU64 "", config.screenId);
        return false;
    }
    return true;
}

Rosen::ScreenId BootCompatibleDisplayStrategy::SelectHdmiScreen(Rosen::ScreenId fallbackId)
{
    Rosen::RSInterfaces& interface = Rosen::RSInterfaces::GetInstance();
    for (uint32_t i = 0; i < HDMI_POLL_MAX_RETRIES; i++) {
        std::vector<Rosen::ScreenId> screenIds;
        {
            std::lock_guard<std::mutex> lock(connectToRenderMapMtx_);
            for (const auto& [id, _] : connectToRenderMap_) {
                screenIds.push_back(id);
            }
        }
        for (auto id : screenIds) {
            Rosen::ScreenInterfaceType intfType = interface.GetScreenCapability(id).GetType();
            LOGI("PC: screen " BPUBU64 " interface type: %{public}u", id, static_cast<uint32_t>(intfType));
            if (intfType == Rosen::DISP_INTF_HDMI) {
                LOGI("PC: selected HDMI screen " BPUBU64 "", id);
                return id;
            }
        }
        usleep(HDMI_POLL_INTERVAL_US);
    }
    LOGW("PC: HDMI not found, fallback to first screen");
    return fallbackId;
}

void BootCompatibleDisplayStrategy::RunAnimationAndOta(BootAnimationConfig& config, int32_t duration)
{
    Rosen::RSInterfaces& interface = Rosen::RSInterfaces::GetInstance();
    Rosen::RSScreenModeInfo modeInfo = interface.GetScreenActiveMode(config.screenId);
    sptr<IRemoteObject> connectToRender = GetConnectToRender(config.screenId);
    if (connectToRender == nullptr) {
        LOGE("RunAnimationAndOta screen is not in connectToRenderMap_:" BPUBU64 "", config.screenId);
        return;
    }
    operator_ = std::make_shared<BootAnimationOperation>();
    operator_->Init(config, modeInfo.GetScreenWidth(), modeInfo.GetScreenHeight(), duration, connectToRender);
    if (operator_->GetThread().joinable()) {
        operator_->GetThread().join();
    }
    if (IsOtaUpdate()) {
        bootCompileProgress_ = std::make_shared<BootCompileProgress>();
        bootCompileProgress_->Init(configPath_, config, connectToRender);
    }
}

void BootCompatibleDisplayStrategy::Display(int32_t duration, std::vector<BootAnimationConfig>& configs)
{
    LOGI("BootCompatibleDisplayStrategy START");
    if (configs.size() != 1) {
        LOGE("config size error");
        return;
    }

    GetConnectToRenderMap(configs.size());
    if (HasScreen()) {
        if (PrepareScreenConfig(configs[0])) {
            RunAnimationAndOta(configs[0], duration);
        } else {
            LOGE("PrepareScreenConfig failed, skip animation");
        }
    } else {
        LOGI("BootCompatibleDisplayStrategy::No screen connected");
    }

    while (!CheckExitAnimation()) {
        usleep(SLEEP_TIME_US);
    }
}
