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

#include "luminance/rs_luminance_control.h"

namespace {
constexpr float HDR_DEFAULT_TMO_NIT = 500.0f;
}

namespace OHOS {
namespace Rosen {

void RSLuminanceControl::SetHdrStatus(ScreenId screenId, bool isHdrOn)
{
    // Update HDR status in order to determine brightness.
}

bool RSLuminanceControl::IsHdrOn(ScreenId screenId) const
{
    return false;
}

bool RSLuminanceControl::IsDimmingOn(ScreenId screenId) const
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

float RSLuminanceControl::GetHdrTmoNits(ScreenId screenId, int Mode) const
{
    return HDR_DEFAULT_TMO_NIT;
}

float RSLuminanceControl::GetHdrDisplayNits(ScreenId screenId) const
{
    return HDR_DEFAULT_TMO_NIT;
}

double RSLuminanceControl::GetHdrBrightnessRatio(ScreenId screenId, int Mode) const
{
    return 1.0; // 1.0 refers to default value, no need to process.
}
} // namespace Rosen
} // namespace OHOS
