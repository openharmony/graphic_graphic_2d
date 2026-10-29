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
    Setter(RSPropertyType::BACKGROUND_COLOR, color, color.GetAlpha() != DEFAULT_BACKGROUND_COLOR.GetAlpha());
}

Color RSBackgroundColorModifier::GetBackgroundColor() const
{
    return Getter(RSPropertyType::BACKGROUND_COLOR, DEFAULT_BACKGROUND_COLOR);
}

void RSBackgroundColorModifier::SetBgBrightnessRates(const Vector4f& rates)
{
    Setter(RSPropertyType::BG_BRIGHTNESS_RATES, rates, rates != DEFAULT_BG_BRIGHTNESS_RATES);
}

Vector4f RSBackgroundColorModifier::GetBgBrightnessRates() const
{
    return Getter(RSPropertyType::BG_BRIGHTNESS_RATES, DEFAULT_BG_BRIGHTNESS_RATES);
}

void RSBackgroundColorModifier::SetBgBrightnessSaturation(float saturation)
{
    Setter(RSPropertyType::BG_BRIGHTNESS_SATURATION, saturation, saturation != DEFAULT_BG_BRIGHTNESS_SATURATION);
}

float RSBackgroundColorModifier::GetBgBrightnessSaturation() const
{
    return Getter(RSPropertyType::BG_BRIGHTNESS_SATURATION, DEFAULT_BG_BRIGHTNESS_SATURATION);
}

void RSBackgroundColorModifier::SetBgBrightnessPosCoeff(const Vector4f& coeff)
{
    Setter(RSPropertyType::BG_BRIGHTNESS_POSCOEFF, coeff, coeff != DEFAULT_BG_BRIGHTNESS_POSCOEFF);
}

Vector4f RSBackgroundColorModifier::GetBgBrightnessPosCoeff() const
{
    return Getter(RSPropertyType::BG_BRIGHTNESS_POSCOEFF, DEFAULT_BG_BRIGHTNESS_POSCOEFF);
}

void RSBackgroundColorModifier::SetBgBrightnessNegCoeff(const Vector4f& coeff)
{
    Setter(RSPropertyType::BG_BRIGHTNESS_NEGCOEFF, coeff, coeff != DEFAULT_BG_BRIGHTNESS_NEGCOEFF);
}

Vector4f RSBackgroundColorModifier::GetBgBrightnessNegCoeff() const
{
    return Getter(RSPropertyType::BG_BRIGHTNESS_NEGCOEFF, DEFAULT_BG_BRIGHTNESS_NEGCOEFF);
}

void RSBackgroundColorModifier::SetBgBrightnessFract(float fraction)
{
    Setter(RSPropertyType::BG_BRIGHTNESS_FRACTION, fraction, fraction != DEFAULT_BG_BRIGHTNESS_FRACTION);
}

float RSBackgroundColorModifier::GetBgBrightnessFract() const
{
    return Getter(RSPropertyType::BG_BRIGHTNESS_FRACTION, DEFAULT_BG_BRIGHTNESS_FRACTION);
}
} // namespace OHOS::Rosen::ModifierNG
