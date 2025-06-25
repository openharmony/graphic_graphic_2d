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

#include "modifier_ng/background/rs_background_color_modifier.h"

namespace OHOS::Rosen::ModifierNG {
void RSBackgroundColorModifier::SetBackgroundColor(Color color)
{
    Setter(RSPropertyType::BACKGROUND_COLOR, color);
}

Color RSBackgroundColorModifier::GetBackgroundColor() const
{
    return Getter(RSPropertyType::BACKGROUND_COLOR, Color(RgbPalette::Transparent()));
}

void RSBackgroundColorModifier::SetBgBrightnessRates(const Vector4f& rates)
{
    Setter(RSPropertyType::BG_BRIGHTNESS_RATES, rates);
}

Vector4f RSBackgroundColorModifier::GetBgBrightnessRates() const
{
    return Getter(RSPropertyType::BG_BRIGHTNESS_RATES, Vector4f(0.f));
}

void RSBackgroundColorModifier::SetBgBrightnessSaturation(float saturation)
{
    Setter(RSPropertyType::BG_BRIGHTNESS_SATURATION, saturation);
}

float RSBackgroundColorModifier::GetBgBrightnessSaturation() const
{
    return Getter(RSPropertyType::BG_BRIGHTNESS_SATURATION, 0.f);
}

void RSBackgroundColorModifier::SetBgBrightnessPosCoeff(const Vector4f& coeff)
{
    Setter(RSPropertyType::BG_BRIGHTNESS_POSCOEFF, coeff);
}

Vector4f RSBackgroundColorModifier::GetBgBrightnessPosCoeff() const
{
    return Getter(RSPropertyType::BG_BRIGHTNESS_POSCOEFF, Vector4f(0.f));
}

void RSBackgroundColorModifier::SetBgBrightnessNegCoeff(const Vector4f& coeff)
{
    Setter(RSPropertyType::BG_BRIGHTNESS_NEGCOEFF, coeff);
}

Vector4f RSBackgroundColorModifier::GetBgBrightnessNegCoeff() const
{
    return Getter(RSPropertyType::BG_BRIGHTNESS_NEGCOEFF, Vector4f(0.f));
}

void RSBackgroundColorModifier::SetBgBrightnessFract(float fraction)
{
    Setter(RSPropertyType::BG_BRIGHTNESS_FRACTION, fraction);
}

float RSBackgroundColorModifier::GetBgBrightnessFract() const
{
    return Getter(RSPropertyType::BG_BRIGHTNESS_FRACTION, 1.f);
}
} // namespace OHOS::Rosen::ModifierNG
