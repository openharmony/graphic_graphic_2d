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

void RSLuminanceControl::SetHdrStatus(uint64_t screenId, bool isHdrOn)
{
}

bool RSLuminanceControl::IsHdrOn(uint64_t screenId) const
{
    return false;
}

bool RSLuminanceControl::IsDimmingOn(uint64_t screenId) const
{
    return false;
}
void RSLuminanceControl::DimmingIncrease(uint64_t screenId)
{
}

void RSLuminanceControl::SetSdrLuminance(uint64_t screenId, uint32_t level)
{
}

uint32_t RSLuminanceControl::GetNewHdrLuminance(uint64_t screenId)
{
    return 0;
}

void RSLuminanceControl::SetNowHdrLuminance(uint64_t screenId, uint32_t level)
{
}

bool RSLuminanceControl::IsNeedUpdateLuminace(uint64_t screenId)
{
    return false;
}

float RSLuminanceControl::GetHdrTmoNits(uint64_t screenId, int Mode) const
{
    return HDR_DEFAULT_TMO_NIT;
}

float RSLuminanceControl::GetHdrDisplayNits(uint64_t screenId) const
{
    return HDR_DEFAULT_TMO_NIT;
}

double RSLuminanceControl::GetHdrBrightnessRatio(uint64_t screenId, int Mode) const
{
    return 1.0;
}
} // namespace Rosen
} // namespace OHOS
