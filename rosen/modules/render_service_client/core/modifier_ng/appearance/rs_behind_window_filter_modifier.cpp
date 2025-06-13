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

#include "modifier_ng/appearance/rs_behind_window_filter_modifier.h"

namespace OHOS::Rosen::ModifierNG {
void RSBehindWindowFilterModifier::SetBehindWindowFilterRadius(float behindWindowFilterRadius)
{
    Setter(RSPropertyType::BEHIND_WINDOW_FILTER_RADIUS, behindWindowFilterRadius);
}

float RSBehindWindowFilterModifier::GetBehindWindowFilterRadius() const
{
    return Getter(RSPropertyType::BEHIND_WINDOW_FILTER_RADIUS, 0.f);
}

void RSBehindWindowFilterModifier::SetBehindWindowFilterSaturation(float behindWindowFilterSaturation)
{
    Setter(RSPropertyType::BEHIND_WINDOW_FILTER_SATURATION, behindWindowFilterSaturation);
}

float RSBehindWindowFilterModifier::GetBehindWindowFilterSaturation() const
{
    return Getter(RSPropertyType::BEHIND_WINDOW_FILTER_SATURATION, 0.f);
}

void RSBehindWindowFilterModifier::SetBehindWindowFilterBrightness(float behindWindowFilterBrightness)
{
    Setter(RSPropertyType::BEHIND_WINDOW_FILTER_BRIGHTNESS, behindWindowFilterBrightness);
}

float RSBehindWindowFilterModifier::GetBehindWindowFilterBrightness() const
{
    return Getter(RSPropertyType::BEHIND_WINDOW_FILTER_BRIGHTNESS, 0.f);
}

void RSBehindWindowFilterModifier::SetBehindWindowFilterMaskColor(const Color behindWindowFilterMaskColor)
{
    Setter(RSPropertyType::BEHIND_WINDOW_FILTER_MASK_COLOR, behindWindowFilterMaskColor);
}

Color RSBehindWindowFilterModifier::GetBehindWindowFilterMaskColor() const
{
    return Getter(RSPropertyType::BEHIND_WINDOW_FILTER_MASK_COLOR, Color(RgbPalette::Transparent()));
}
} // namespace OHOS::Rosen::ModifierNG
