/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "modifier_ng/appearance/rs_hdr_brightness_modifier.h"

namespace OHOS::Rosen::ModifierNG {
void RSHDRBrightnessModifier::SetHDRBrightness(float hdrBrightness)
{
    Setter(RSPropertyType::HDR_BRIGHTNESS, hdrBrightness);
}

float RSHDRBrightnessModifier::GetHDRBrightness() const
{
    return Getter(RSPropertyType::HDR_BRIGHTNESS, 1.f);
}

void RSHDRBrightnessModifier::SetHDRUIBrightness(float brightness)
{
    Setter(RSPropertyType::HDR_UI_BRIGHTNESS, brightness);
}

float RSHDRBrightnessModifier::GetHDRUIBrightness() const
{
    return Getter(RSPropertyType::HDR_UI_BRIGHTNESS, 1.f);
}

void RSHDRBrightnessModifier::SetHDRBrightnessFactor(float factor)
{
    Setter(RSPropertyType::HDR_BRIGHTNESS_FACTOR, factor);
}

float RSHDRBrightnessModifier::GetHDRBrightnessFactor() const
{
    return Getter(RSPropertyType::HDR_BRIGHTNESS_FACTOR, 1.f);
}
} // namespace OHOS::Rosen::ModifierNG
