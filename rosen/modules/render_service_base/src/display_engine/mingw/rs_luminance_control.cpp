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
    const std::vector<uint8_t>& dynamicMetadata, const float& ratio)
{
    return HDR_DEFAULT_SCALER;
}

bool RSLuminanceControl::IsHdrPictureOn()
{
    return false;
}

bool IsCloseHardwareHdr()
{
    return false;
}
} // namespace Rosen
} // namespace OHOS