/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include <scoped_bytrace.h>
#include "hdi_screen.h"
#include <chrono>
#include "hdi_log.h"
#include "vsync_sampler.h"

#define CHECK_DEVICE_NULL(sptrDevice)                                \
    do {                                                             \
        if ((sptrDevice) == nullptr) {                               \
            HLOGE("[%{public}s]: HdiDevice is nullptr.", __func__);  \
            return GRAPHIC_DISPLAY_NULL_PTR;                         \
        }                                                            \
    } while (0)

namespace OHOS {
namespace Rosen {

std::unique_ptr<HdiScreen> HdiScreen::CreateHdiScreen(uint32_t screenId)
{
    return std::make_unique<HdiScreen>(screenId);
}

HdiScreen::HdiScreen(uint32_t screenId) : screenId_(screenId)
{
    HLOGI("Create screen, screenId is %{public}d", screenId);
}

HdiScreen::~HdiScreen()
{
    HLOGI("Destroy screen, screenId is %{public}d", screenId_);

    Destroy();
}

void HdiScreen::OnVsync(uint32_t sequence, uint64_t ns, void *data)
{
    ScopedBytrace onVsyncTrace("HdiScreen::OnVsync_" + std::to_string((ns)));
    if (ns == 0) {
        HLOGW("Vsync ns is 0, drop this callback");
        return;
    }

    // trigger vsync
    // if the sampler->GetHardwareVSyncStatus() is false, this OnVsync callback will be disable
    // we need to add this process
    auto sampler = CreateVSyncSampler();
    if (sampler->GetHardwareVSyncStatus()) {
        bool enable = sampler->AddSample(ns);
        sampler->SetHardwareVSyncStatus(enable);
    }
}

bool HdiScreen::Init()
{
    if (device_ != nullptr) {
        HLOGI("HdiScreen has been initialized");
        return true;
    }

    device_ = HdiDevice::GetInstance();
    if (device_ == nullptr) {
        HLOGE("[%{public}s]: HdiDevice is nullptr.", __func__);
        return false;
    }

    int32_t ret = device_->RegScreenVBlankCallback(screenId_, HdiScreen::OnVsync, this);
    if (ret != GRAPHIC_DISPLAY_SUCCESS) {
        Destroy();
        HLOGE("RegScreenVBlankCallback failed, ret is %{public}d", ret);
        return false;
    }

    ret = device_->SetScreenVsyncEnabled(screenId_, true);
    if (ret != GRAPHIC_DISPLAY_SUCCESS) {
        Destroy();
        HLOGE("SetScreenVsyncEnabled failed, ret is %{public}d", ret);
        return false;
    }

    HLOGI("Init hdiScreen succeed");

    return true;
}

bool HdiScreen::SetHdiDevice(HdiDevice* device)
{
    if (device_ != nullptr) {
        return true;
    }

    if (device == nullptr) {
        HLOGE("Input HdiDevice is null");
        return false;
    }
    
    device_ = device;
    return true;
}

int32_t HdiScreen::GetScreenCapability(GraphicDisplayCapability &dcap) const
{
    CHECK_DEVICE_NULL(device_);
    return device_->GetScreenCapability(screenId_, dcap);
}

int32_t HdiScreen::GetScreenSupportedModes(std::vector<GraphicDisplayModeInfo> &modes) const
{
    CHECK_DEVICE_NULL(device_);
    return device_->GetScreenSupportedModes(screenId_, modes);
}

int32_t HdiScreen::GetScreenMode(uint32_t &modeId) const
{
    CHECK_DEVICE_NULL(device_);
    return device_->GetScreenMode(screenId_, modeId);
}

int32_t HdiScreen::SetScreenMode(uint32_t modeId) const
{
    CHECK_DEVICE_NULL(device_);
    return device_->SetScreenMode(screenId_, modeId);
}

int32_t HdiScreen::SetScreenOverlayResolution(uint32_t width, uint32_t height) const
{
    CHECK_DEVICE_NULL(device_);
    return device_->SetScreenOverlayResolution(screenId_, width, height);
}

int32_t HdiScreen::GetScreenPowerStatus(GraphicDispPowerStatus &status) const
{
    CHECK_DEVICE_NULL(device_);
    return device_->GetScreenPowerStatus(screenId_, status);
}

int32_t HdiScreen::SetScreenPowerStatus(GraphicDispPowerStatus status) const
{
    CHECK_DEVICE_NULL(device_);
    return device_->SetScreenPowerStatus(screenId_, status);
}

int32_t HdiScreen::GetScreenBacklight(uint32_t &level) const
{
    CHECK_DEVICE_NULL(device_);
    return device_->GetScreenBacklight(screenId_, level);
}

int32_t HdiScreen::SetScreenBacklight(uint32_t level) const
{
    CHECK_DEVICE_NULL(device_);
    return device_->SetScreenBacklight(screenId_, level);
}

int32_t HdiScreen::SetScreenVsyncEnabled(bool enabled) const
{
    CHECK_DEVICE_NULL(device_);
    return device_->SetScreenVsyncEnabled(screenId_, enabled);
}

int32_t HdiScreen::GetScreenSupportedColorGamuts(std::vector<GraphicColorGamut> &gamuts) const
{
    CHECK_DEVICE_NULL(device_);
    return device_->GetScreenSupportedColorGamuts(screenId_, gamuts);
}

int32_t HdiScreen::SetScreenColorGamut(GraphicColorGamut gamut) const
{
    CHECK_DEVICE_NULL(device_);
    return device_->SetScreenColorGamut(screenId_, gamut);
}

int32_t HdiScreen::GetScreenColorGamut(GraphicColorGamut &gamut) const
{
    CHECK_DEVICE_NULL(device_);
    return device_->GetScreenColorGamut(screenId_, gamut);
}

int32_t HdiScreen::SetScreenGamutMap(GraphicGamutMap gamutMap) const
{
    CHECK_DEVICE_NULL(device_);
    return device_->SetScreenGamutMap(screenId_, gamutMap);
}

int32_t HdiScreen::GetScreenGamutMap(GraphicGamutMap &gamutMap) const
{
    CHECK_DEVICE_NULL(device_);
    return device_->GetScreenGamutMap(screenId_, gamutMap);
}

int32_t HdiScreen::SetScreenColorTransform(const std::vector<float>& matrix) const
{
    CHECK_DEVICE_NULL(device_);
    return device_->SetScreenColorTransform(screenId_, matrix);
}

int32_t HdiScreen::GetHDRCapabilityInfos(GraphicHDRCapability &info) const
{
    CHECK_DEVICE_NULL(device_);
    return device_->GetHDRCapabilityInfos(screenId_, info);
}

int32_t HdiScreen::GetSupportedMetaDataKey(std::vector<GraphicHDRMetadataKey> &keys) const
{
    CHECK_DEVICE_NULL(device_);
    return device_->GetSupportedMetaDataKey(screenId_, keys);
}

void HdiScreen::Destroy()
{
}

} // namespace Rosen
} // namespace OHOS
