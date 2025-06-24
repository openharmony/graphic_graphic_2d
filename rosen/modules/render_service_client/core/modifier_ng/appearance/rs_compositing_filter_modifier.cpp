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

#include "modifier_ng/appearance/rs_compositing_filter_modifier.h"

namespace OHOS::Rosen::ModifierNG {
void RSCompositingFilterModifier::SetAiInvert(const std::optional<Vector4f>& aiInvert)
{
    SetterOptional<RSAnimatableProperty>(RSPropertyType::AIINVERT, aiInvert);
}

std::optional<Vector4f> RSCompositingFilterModifier::GetAiInvert() const
{
    return GetterOptional<Vector4f>(RSPropertyType::AIINVERT);
}

void RSCompositingFilterModifier::SetGrayScale(std::optional<float> grayScale)
{
    SetterOptional<RSAnimatableProperty>(RSPropertyType::GRAY_SCALE, grayScale);
}

std::optional<float> RSCompositingFilterModifier::GetGrayScale() const
{
    return GetterOptional<float>(RSPropertyType::GRAY_SCALE);
}

void RSCompositingFilterModifier::SetBrightness(std::optional<float> brightness)
{
    SetterOptional<RSAnimatableProperty>(RSPropertyType::BRIGHTNESS, brightness);
}

std::optional<float> RSCompositingFilterModifier::GetBrightness() const
{
    return GetterOptional<float>(RSPropertyType::BRIGHTNESS);
}

void RSCompositingFilterModifier::SetContrast(std::optional<float> contrast)
{
    SetterOptional<RSAnimatableProperty>(RSPropertyType::CONTRAST, contrast);
}

std::optional<float> RSCompositingFilterModifier::GetContrast() const
{
    return GetterOptional<float>(RSPropertyType::CONTRAST);
}

void RSCompositingFilterModifier::SetSaturate(std::optional<float> saturate)
{
    SetterOptional<RSAnimatableProperty>(RSPropertyType::SATURATE, saturate);
}

std::optional<float> RSCompositingFilterModifier::GetSaturate() const
{
    return GetterOptional<float>(RSPropertyType::SATURATE);
}

void RSCompositingFilterModifier::SetSepia(std::optional<float> sepia)
{
    SetterOptional<RSAnimatableProperty>(RSPropertyType::SEPIA, sepia);
}

std::optional<float> RSCompositingFilterModifier::GetSepia() const
{
    return GetterOptional<float>(RSPropertyType::SEPIA);
}

void RSCompositingFilterModifier::SetInvert(std::optional<float> invert)
{
    SetterOptional<RSAnimatableProperty>(RSPropertyType::INVERT, invert);
}

std::optional<float> RSCompositingFilterModifier::GetInvert() const
{
    return GetterOptional<float>(RSPropertyType::INVERT);
}

void RSCompositingFilterModifier::SetHueRotate(std::optional<float> hueRotate)
{
    SetterOptional<RSAnimatableProperty>(RSPropertyType::HUE_ROTATE, hueRotate);
}

std::optional<float> RSCompositingFilterModifier::GetHueRotate() const
{
    return GetterOptional<float>(RSPropertyType::HUE_ROTATE);
}

void RSCompositingFilterModifier::SetColorBlend(const std::optional<Color> colorBlend)
{
    SetterOptional<RSAnimatableProperty>(RSPropertyType::COLOR_BLEND, colorBlend);
}

std::optional<Color> RSCompositingFilterModifier::GetColorBlend() const
{
    return GetterOptional<Color>(RSPropertyType::COLOR_BLEND);
}

void RSCompositingFilterModifier::SetLightUpEffectDegree(float lightUpEffectDegree)
{
    Setter(RSPropertyType::LIGHT_UP_EFFECT_DEGREE, lightUpEffectDegree);
}

float RSCompositingFilterModifier::GetLightUpEffectDegree() const
{
    return Getter<float>(RSPropertyType::LIGHT_UP_EFFECT_DEGREE, 1.f);
}

void RSCompositingFilterModifier::SetDynamicDimDegree(std::optional<float> dimDegree)
{
    SetterOptional<RSAnimatableProperty>(RSPropertyType::DYNAMIC_DIM_DEGREE, dimDegree);
}

std::optional<float> RSCompositingFilterModifier::GetDynamicDimDegree() const
{
    return GetterOptional<float>(RSPropertyType::DYNAMIC_DIM_DEGREE);
}

void RSCompositingFilterModifier::SetLinearGradientBlurPara(const std::shared_ptr<RSLinearGradientBlurPara>& para)
{
    Setter<RSProperty, std::shared_ptr<RSLinearGradientBlurPara>>(RSPropertyType::LINEAR_GRADIENT_BLUR_PARA, para);
}

std::shared_ptr<RSLinearGradientBlurPara> RSCompositingFilterModifier::GetLinearGradientBlurPara() const
{
    return Getter<std::shared_ptr<RSLinearGradientBlurPara>>(RSPropertyType::LINEAR_GRADIENT_BLUR_PARA, nullptr);
}

void RSCompositingFilterModifier::SetForegroundBlurRadius(float radius)
{
    Setter(RSPropertyType::FOREGROUND_BLUR_RADIUS, radius);
}

float RSCompositingFilterModifier::GetForegroundBlurRadius() const
{
    return Getter<float>(RSPropertyType::FOREGROUND_BLUR_RADIUS, 0.f);
}

void RSCompositingFilterModifier::SetForegroundBlurSaturation(float saturation)
{
    Setter(RSPropertyType::FOREGROUND_BLUR_SATURATION, saturation);
}

float RSCompositingFilterModifier::GetForegroundBlurSaturation() const
{
    return Getter<float>(RSPropertyType::FOREGROUND_BLUR_SATURATION, 1.f);
}

void RSCompositingFilterModifier::SetForegroundBlurBrightness(float brightness)
{
    Setter(RSPropertyType::FOREGROUND_BLUR_BRIGHTNESS, brightness);
}

float RSCompositingFilterModifier::GetForegroundBlurBrightness() const
{
    return Getter<float>(RSPropertyType::FOREGROUND_BLUR_BRIGHTNESS, 1.f);
}

void RSCompositingFilterModifier::SetForegroundBlurMaskColor(const Color maskColor)
{
    Setter(RSPropertyType::FOREGROUND_BLUR_MASK_COLOR, maskColor);
}

Color RSCompositingFilterModifier::GetForegroundBlurMaskColor() const
{
    return Getter<Color>(RSPropertyType::FOREGROUND_BLUR_MASK_COLOR, RSColor());
}

void RSCompositingFilterModifier::SetForegroundBlurColorMode(int colorMode)
{
    Setter<RSProperty>(RSPropertyType::FOREGROUND_BLUR_COLOR_MODE, colorMode);
}

int RSCompositingFilterModifier::GetForegroundBlurColorMode() const
{
    return Getter<int>(RSPropertyType::FOREGROUND_BLUR_COLOR_MODE, 0);
}

void RSCompositingFilterModifier::SetForegroundBlurRadiusX(float blurRadiusX)
{
    Setter(RSPropertyType::FOREGROUND_BLUR_RADIUS_X, blurRadiusX);
}

float RSCompositingFilterModifier::GetForegroundBlurRadiusX() const
{
    return Getter<float>(RSPropertyType::FOREGROUND_BLUR_RADIUS_X, 0.f);
}

void RSCompositingFilterModifier::SetForegroundBlurRadiusY(float blurRadiusY)
{
    Setter(RSPropertyType::FOREGROUND_BLUR_RADIUS_Y, blurRadiusY);
}

float RSCompositingFilterModifier::GetForegroundBlurRadiusY() const
{
    return Getter<float>(RSPropertyType::FOREGROUND_BLUR_RADIUS_Y, 0.f);
}

void RSCompositingFilterModifier::SetFgBlurDisableSystemAdaptation(bool disableSystemAdaptation)
{
    Setter<RSProperty>(RSPropertyType::FG_BLUR_DISABLE_SYSTEM_ADAPTATION, disableSystemAdaptation);
}

bool RSCompositingFilterModifier::GetFgBlurDisableSystemAdaptation() const
{
    return Getter(RSPropertyType::FG_BLUR_DISABLE_SYSTEM_ADAPTATION, true);
}
} // namespace OHOS::Rosen::ModifierNG
