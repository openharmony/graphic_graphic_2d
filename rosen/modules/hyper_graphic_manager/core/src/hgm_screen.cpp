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

#include "hgm_screen.h"

#include "hgm_log.h"

namespace OHOS::Rosen {
HgmScreen::HgmScreen() {}

HgmScreen::HgmScreen(ScreenId id, int32_t mode)
    : id_(id), activeModeId_(mode) {}

HgmScreen::~HgmScreen() {}

uint32_t HgmScreen::GetActiveRefreshRate()
{
    auto profilePtr = GetModeViaId(activeModeId_);
    if (!profilePtr) {
        HGM_LOGI("HgmScreen current mode is not supported, failed to get refreshrate");
        return RATE_NOT_SUPPORTED;
    }

    return profilePtr->GetRate();
}

int32_t HgmScreen::SetActiveRefreshRate(int32_t sceneId, uint32_t rate)
{
    // check if this screen supports the requested rate
    if (supportedRefreshRates_.find(rate) == supportedRefreshRates_.end()) {
        HGM_LOGE("HgmScreen refreshrate %{public}u is not supported, abandoned", rate);
        return HGM_ERROR;
    }

    // decide if this rate should be accepted
    if (!IfSwitchToRate(sceneId, rate)) {
        return HGM_ERROR;
    }

    int32_t modeIdToApply = GetModeIdViaRate(rate);
    if (modeIdToApply >= 0) {
        SetActiveModeId(modeIdToApply);
    }
    return modeIdToApply;
}

int32_t HgmScreen::SetRateAndResolution(int32_t sceneId, uint32_t rate, int32_t width, int32_t height)
{
    if (!IfSwitchToRate(sceneId, rate)) {
        return HGM_ERROR;
    }

    int32_t modeIdToApply = GetModeIdViaResolutionAndRate(width, height, rate);
    if (modeIdToApply >= 0) {
        SetActiveModeId(modeIdToApply);
    }
    return modeIdToApply;
}

int32_t HgmScreen::AddScreenModeInfo(int32_t width, int32_t height, uint32_t rate, int32_t modeId)
{
    if (supportedModeIds_.find(modeId) == supportedModeIds_.end()) {
        supportedModeIds_.emplace(modeId);
    } else {
        return HGM_SCREEN_MODE_EXIST;
    }

    if (supportedRefreshRates_.find(rate) == supportedRefreshRates_.end()) {
        supportedRefreshRates_.emplace(rate);
    }

    auto newProfile = std::make_shared<ScreenProfile>(width, height, rate, modeId);
    screenModeInfos_.emplace_back(newProfile);
    return EXEC_SUCCESS;
}

void HgmScreen::VerifySupportedProfiles() const
{
    for (auto modePtr : screenModeInfos_) {
        HGM_LOGI("HgmScreen verify modes w : %{public}d, h : %{public}d, "
            "rate : %{public}u, modeId : %{public}d, currentMode : %{public}d",
            modePtr->GetWidth(), modePtr->GetHeight(), modePtr->GetRate(), modePtr->GetModeId(), activeModeId_);
    }
}

void HgmScreen::SetActiveModeId(int32_t modeId)
{
    HGM_LOGI("HgmScreen setting activeModeId to %{public}d", modeId);
    activeModeId_ = modeId;
}

std::shared_ptr<HgmScreen::ScreenProfile> HgmScreen::GetModeViaId(int32_t id) const
{
    for (auto mode : screenModeInfos_) {
        if (mode->GetModeId() != id) {
            continue;
        }
        return mode;
    }

    HGM_LOGW("HgmScreen failed to get activeMode via modeId : %{public}d", id);
    return nullptr;
}

bool HgmScreen::IfSwitchToRate(int32_t screenId, uint32_t rate) const
{
    // decision fabricating system to be developped, now it approves as long as we have different rates
    auto profilePtr = GetModeViaId(activeModeId_);
    if (!profilePtr) {
        return false;
    }

    if (rate == profilePtr->GetRate() || screenId < 0) {
        HGM_LOGI("HgmScreen Set framerate to %{public}u is rejected!!!", rate);
        return false;
    }

    HGM_LOGI("HgmScreen Set framerate to %{public}u is accepted", rate);
    return true;
}

int32_t HgmScreen::GetModeIdViaRate(uint32_t rate) const
{
    // get the corresponding mode id with the given refreshrate, using the current resolution
    auto supportedRates = supportedRefreshRates_.find(rate);
    if (supportedRates == supportedRefreshRates_.end()) {
        HGM_LOGE("HgmScreen freshrate %{public}u is not supported by any modeId", rate);
        return HGM_ERROR;
    }

    auto profilePtr = GetModeViaId(activeModeId_);
    if (!profilePtr) {
        HGM_LOGW("HgmScreen activeMode is not among supported modes");
        return HGM_ERROR;
    }

    HGM_LOGI("HgmScreen getting a new mode with the resolution of current mode : %{public}d", activeModeId_);
    int32_t mode = GetModeIdViaResolutionAndRate(profilePtr->GetWidth(), profilePtr->GetHeight(), rate);
    return mode;
}

int32_t HgmScreen::GetModeIdViaResolutionAndRate(int32_t width, int32_t height, uint32_t rate) const
{
    // get the corresponding mode id with the given resolution and refreshrate
    for (auto& mode : screenModeInfos_) {
        if (mode->GetRate() != rate || mode->GetWidth() != width || mode->GetHeight() != height) {
            continue;
        }
        int32_t modeIdFound = mode->GetModeId();
        HGM_LOGI("HgmScreen mode : %{public}d is found for w : %{public}d, h : %{public}d, rate : %{public}u",
            modeIdFound, width, height, rate);
        return modeIdFound;
    }

    HGM_LOGW("HgmScreen NO mode is found for w : %{public}d, h : %{public}d, rate : %{public}u",
        width, height, rate);
    return HGM_ERROR;
}
} // namespace OHOS::Rosen