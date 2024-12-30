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

#include "modifier_ng/rs_foreground_modifier.h"

namespace OHOS::Rosen::ModifierNG {
// =============================================
// RSCompositingFilterModifier
void RSCompositingFilterModifier::SetAiInvert(const Vector4f& aiInvert)
{
    Setter(RSPropertyType::AIINVERT, aiInvert);
}
Vector4f RSCompositingFilterModifier::GetAiInvert() const
{
    return Getter<Vector4f>(RSPropertyType::AIINVERT, {});
}

void RSCompositingFilterModifier::SetGrayScale(float grayScale)
{
    Setter(RSPropertyType::GRAY_SCALE, grayScale);
}
float RSCompositingFilterModifier::GetGrayScale() const
{
    return Getter<float>(RSPropertyType::GRAY_SCALE, 0.f);
}

void RSCompositingFilterModifier::SetBrightness(float brightness)
{
    Setter(RSPropertyType::BRIGHTNESS, brightness);
}
float RSCompositingFilterModifier::GetBrightness() const
{
    return Getter<float>(RSPropertyType::BRIGHTNESS, 0.f);
}

void RSCompositingFilterModifier::SetContrast(float contrast)
{
    Setter(RSPropertyType::CONTRAST, contrast);
}
float RSCompositingFilterModifier::GetContrast() const
{
    return Getter<float>(RSPropertyType::CONTRAST, 0.f);
}

void RSCompositingFilterModifier::SetSaturate(float saturate)
{
    Setter(RSPropertyType::SATURATE, saturate);
}
float RSCompositingFilterModifier::GetSaturate() const
{
    return Getter<float>(RSPropertyType::SATURATE, 0.f);
}

void RSCompositingFilterModifier::SetSepia(float sepia)
{
    Setter(RSPropertyType::SEPIA, sepia);
}
float RSCompositingFilterModifier::GetSepia() const
{
    return Getter<float>(RSPropertyType::SEPIA, 0.f);
}

void RSCompositingFilterModifier::SetInvert(float invert)
{
    Setter(RSPropertyType::INVERT, invert);
}
float RSCompositingFilterModifier::GetInvert() const
{
    return Getter<float>(RSPropertyType::INVERT, 0.f);
}

void RSCompositingFilterModifier::SetHueRotate(float hueRotate)
{
    Setter(RSPropertyType::HUE_ROTATE, hueRotate);
}
float RSCompositingFilterModifier::GetHueRotate() const
{
    return Getter<float>(RSPropertyType::HUE_ROTATE, 0.f);
}

void RSCompositingFilterModifier::SetColorBlend(const Color colorBlend)
{
    Setter(RSPropertyType::COLOR_BLEND, colorBlend);
}
Color RSCompositingFilterModifier::GetColorBlend() const
{
    return Getter<Color>(RSPropertyType::COLOR_BLEND, Color(RgbPalette::Transparent()));
}

void RSCompositingFilterModifier::SetLightUpEffectDegree(float lightUpEffectDegree)
{
    Setter(RSPropertyType::LIGHT_UP_EFFECT_DEGREE, lightUpEffectDegree);
}
float RSCompositingFilterModifier::GetLightUpEffectDegree() const
{
    return Getter<float>(RSPropertyType::LIGHT_UP_EFFECT_DEGREE, {});
}

void RSCompositingFilterModifier::SetDynamicDimDegree(float dimDegree)
{
    Setter(RSPropertyType::DYNAMIC_DIM_DEGREE, dimDegree);
}
float RSCompositingFilterModifier::GetDynamicDimDegree() const
{
    return Getter<float>(RSPropertyType::DYNAMIC_DIM_DEGREE, {});
}

void RSCompositingFilterModifier::SetFilter(const std::shared_ptr<RSFilter>& filter)
{
    // Setter(RSPropertyType::FILTER, filter);
}
std::shared_ptr<RSFilter> RSCompositingFilterModifier::GetFilter() const
{
    return Getter<std::shared_ptr<RSFilter>>(RSPropertyType::FILTER, {});
}

void RSCompositingFilterModifier::SetLinearGradientBlurPara(const std::shared_ptr<RSLinearGradientBlurPara>& para)
{
    Setter<RSProperty, std::shared_ptr<RSLinearGradientBlurPara>>(RSPropertyType::LINEAR_GRADIENT_BLUR_PARA, para);
}
std::shared_ptr<RSLinearGradientBlurPara> RSCompositingFilterModifier::GetLinearGradientBlurPara() const
{
    return Getter<std::shared_ptr<RSLinearGradientBlurPara>>(RSPropertyType::LINEAR_GRADIENT_BLUR_PARA, {});
}

void RSCompositingFilterModifier::SetForegroundBlurRadius(float radius)
{
    Setter(RSPropertyType::FOREGROUND_BLUR_RADIUS, radius);
}
float RSCompositingFilterModifier::GetForegroundBlurRadius() const
{
    return Getter<float>(RSPropertyType::FOREGROUND_BLUR_RADIUS, {});
}

void RSCompositingFilterModifier::SetForegroundBlurSaturation(float saturation)
{
    Setter(RSPropertyType::FOREGROUND_BLUR_SATURATION, saturation);
}
float RSCompositingFilterModifier::GetForegroundBlurSaturation() const
{
    return Getter<float>(RSPropertyType::FOREGROUND_BLUR_SATURATION, {});
}

void RSCompositingFilterModifier::SetForegroundBlurBrightness(float brightness)
{
    Setter(RSPropertyType::FOREGROUND_BLUR_BRIGHTNESS, brightness);
}
float RSCompositingFilterModifier::GetForegroundBlurBrightness() const
{
    return Getter<float>(RSPropertyType::FOREGROUND_BLUR_BRIGHTNESS, {});
}

void RSCompositingFilterModifier::SetForegroundBlurMaskColor(const Color maskColor)
{
    Setter(RSPropertyType::FOREGROUND_BLUR_MASK_COLOR, maskColor);
}
Color RSCompositingFilterModifier::GetForegroundBlurMaskColor() const
{
    return Getter<Color>(RSPropertyType::FOREGROUND_BLUR_MASK_COLOR, {});
}

void RSCompositingFilterModifier::SetForegroundBlurColorMode(int colorMode)
{
    Setter<RSProperty>(RSPropertyType::FOREGROUND_BLUR_COLOR_MODE, colorMode);
}
int RSCompositingFilterModifier::GetForegroundBlurColorMode() const
{
    return Getter<int>(RSPropertyType::FOREGROUND_BLUR_COLOR_MODE, {});
}

void RSCompositingFilterModifier::SetForegroundBlurRadiusX(float blurRadiusX)
{
    Setter(RSPropertyType::FOREGROUND_BLUR_RADIUS_X, blurRadiusX);
}
float RSCompositingFilterModifier::GetForegroundBlurRadiusX() const
{
    return Getter<float>(RSPropertyType::FOREGROUND_BLUR_RADIUS_X, {});
}

void RSCompositingFilterModifier::SetForegroundBlurRadiusY(float blurRadiusY)
{
    Setter(RSPropertyType::FOREGROUND_BLUR_RADIUS_Y, blurRadiusY);
}
float RSCompositingFilterModifier::GetForegroundBlurRadiusY() const
{
    return Getter<float>(RSPropertyType::FOREGROUND_BLUR_RADIUS_Y, {});
}

void RSForegroundColorModifier::SetForegroundColor(const Color color)
{
    Setter(RSPropertyType::FOREGROUND_COLOR, color);
}
Color RSForegroundColorModifier::GetForegroundColor() const
{
    return Getter<Color>(RSPropertyType::FOREGROUND_COLOR, {});
}
} // namespace OHOS::Rosen::ModifierNG
