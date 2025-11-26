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

#include "display_engine/rs_luminance_control.h"

namespace {
constexpr float HDR_DEFAULT_TMO_NIT = 1000.0f;
constexpr float HDR_DEFAULT_SCALER = 1000.0f / 203.0f;
}

namespace OHOS {
namespace Rosen {
RSLuminanceControl& RSLuminanceControl::Get()
{
    static RSLuminanceControl instance;
    return instance;
}

RSLuminanceControl::~RSLuminanceControl()
{
    // destructor
}

void RSLuminanceControl::UpdateScreenStatus(ScreenId screenId, ScreenPowerStatus powerStatus)
{
    // Update screen status.
}

bool RSLuminanceControl::SetHdrStatus(ScreenId screenId, HdrStatus hdrStatus)
{
    // Update HDR status in order to determine brightness.
    return false;
}

bool RSLuminanceControl::IsHdrOn(ScreenId screenId)
{
    return false;
}

bool RSLuminanceControl::IsDimmingOn(ScreenId screenId)
{
    return false;
}

void RSLuminanceControl::DimmingIncrease(ScreenId screenId)
{
    // Update HDR dimming index.
}

void RSLuminanceControl::SetSdrLuminance(ScreenId screenId, uint32_t level)
{
    // Update SDR brightness.
}

uint32_t RSLuminanceControl::GetNewHdrLuminance(ScreenId screenId)
{
    return 0;
}

void RSLuminanceControl::SetNowHdrLuminance(ScreenId screenId, uint32_t level)
{
    // Update HDR brightness after dimming increase.
}

bool RSLuminanceControl::IsNeedUpdateLuminance(ScreenId screenId)
{
    return false;
}

float RSLuminanceControl::GetSdrDisplayNits(ScreenId screenId)
{
    return HDR_DEFAULT_TMO_NIT;
}

float RSLuminanceControl::GetDisplayNits(ScreenId screenId)
{
    return HDR_DEFAULT_TMO_NIT;
}

double RSLuminanceControl::GetHdrBrightnessRatio(ScreenId screenId, uint32_t mode)
{
    return 1.0; // 1.0 refers to default value, no need to process.
}

float RSLuminanceControl::CalScaler(const float& maxContentLightLevel,
    const std::vector<uint8_t>& dynamicMetadata, const float& ratio, HdrStatus hdrStatus)
{
    return HDR_DEFAULT_SCALER;
}

bool RSLuminanceControl::IsHdrPictureOn()
{
    return false;
}

bool RSLuminanceControl::IsScreenNoHeadroom(ScreenId screenId) const
{
    return false;
}

double RSLuminanceControl::GetMaxScaler(ScreenId screenId) const
{
    return HDR_DEFAULT_SCALER;
}

BrightnessInfo RSLuminanceControl::GetBrightnessInfo(ScreenId screenId)
{
    return BrightnessInfo{};
}

bool RSLuminanceControl::IsBrightnessInfoChanged(ScreenId screenId)
{
    return false;
}

void RSLuminanceControl::HandleGamutSpecialRender(std::vector<ScreenColorGamut>& modes)
{
    // Update screen supported color gamut
}

uint32_t RSLuminanceControl::ConvertScalerFromFloatToLevel(float& scaler) const
{
    return 0;
}

float RSLuminanceControl::ConvertScalerFromLevelToFloat(uint32_t& level) const
{
    return 1.0f;
}

void RSLuminanceControl::SetCurDisplayHdrBrightnessScaler(ScreenId screenId,
    std::unordered_map<HdrStatus, std::unordered_map<uint32_t, uint32_t>>& curDisplayHdrBrightnessScaler)
{
    return;
}

bool RSLuminanceControl::IsHardwareHdrDisabled(bool checkBrightnessRatio, ScreenId screenId) const
{
    return false;
}
} // namespace Rosen
} // namespace OHOS