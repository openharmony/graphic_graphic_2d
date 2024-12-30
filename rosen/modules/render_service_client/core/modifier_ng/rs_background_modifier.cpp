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

#include "modifier_ng/rs_background_modifier.h"

namespace OHOS::Rosen::ModifierNG {
// ===========================================================================
// RSBackgroundColorModifier
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

// ===========================================================================
// RSBackgroundShaderModifier
void RSBackgroundShaderModifier::SetBackgroundShader(const std::shared_ptr<RSShader>& shader)
{
    Setter<RSProperty, std::shared_ptr<RSShader>>(RSPropertyType::BACKGROUND_SHADER, shader);
}
std::shared_ptr<RSShader> RSBackgroundShaderModifier::GetBackgroundShader() const
{
    return Getter(RSPropertyType::BACKGROUND_SHADER, std::shared_ptr<RSShader>(nullptr));
}
void RSBackgroundShaderModifier::SetBackgroundShaderProgress(float progress)
{
    Setter(RSPropertyType::BACKGROUND_SHADER_PROGRESS, progress);
}
float RSBackgroundShaderModifier::GetBackgroundShaderProgress() const
{
    return Getter(RSPropertyType::BACKGROUND_SHADER_PROGRESS, 0.f);
}

// ===========================================================================
// RSBackgroundImageModifier
void RSBackgroundImageModifier::SetBgImage(const std::shared_ptr<RSImage>& image)
{
    Setter<RSProperty, std::shared_ptr<RSImage>>(RSPropertyType::BG_IMAGE, image);
}
std::shared_ptr<RSImage> RSBackgroundImageModifier::GetBgImage() const
{
    return Getter(RSPropertyType::BG_IMAGE, std::shared_ptr<RSImage>(nullptr));
}
void RSBackgroundImageModifier::SetBgImageInnerRect(const Vector4f& innerRect)
{
    Setter(RSPropertyType::BG_IMAGE_INNER_RECT, innerRect);
}
Vector4f RSBackgroundImageModifier::GetBgImageInnerRect() const
{
    return Getter(RSPropertyType::BG_IMAGE_INNER_RECT, Vector4f(0.f));
}
void RSBackgroundImageModifier::SetBgImageWidth(float width)
{
    Vector4f dstRect = RSBackgroundImageModifier::GetBgImageDstRect();
    dstRect[2] = width; // 2 index of width
    SetBgImageDstRect(dstRect);
}
float RSBackgroundImageModifier::GetBgImageWidth() const
{
    Vector4f dstRect = RSBackgroundImageModifier::GetBgImageDstRect();
    return dstRect[2]; // 2 index of width
}
void RSBackgroundImageModifier::SetBgImageHeight(float height)
{
    Vector4f dstRect = RSBackgroundImageModifier::GetBgImageDstRect();
    dstRect[3] = height; // 3 index of height
    SetBgImageDstRect(dstRect);
}
float RSBackgroundImageModifier::GetBgImageHeight() const
{
    Vector4f dstRect = RSBackgroundImageModifier::GetBgImageDstRect();
    return dstRect[3]; // 3 index of height
}
void RSBackgroundImageModifier::SetBgImagePositionX(float positionX)
{
    Vector4f dstRect = RSBackgroundImageModifier::GetBgImageDstRect();
    dstRect[0] = positionX; // 0 index of position x
    SetBgImageDstRect(dstRect);
}
float RSBackgroundImageModifier::GetBgImagePositionX() const
{
    Vector4f dstRect = RSBackgroundImageModifier::GetBgImageDstRect();
    return dstRect[0]; // 0 index of position x
}
void RSBackgroundImageModifier::SetBgImagePositionY(float positionY)
{
    Vector4f dstRect = RSBackgroundImageModifier::GetBgImageDstRect();
    dstRect[1] = positionY; // 1 index of position y
    SetBgImageDstRect(dstRect);
}
float RSBackgroundImageModifier::GetBgImagePositionY() const
{
    Vector4f dstRect = RSBackgroundImageModifier::GetBgImageDstRect();
    return dstRect[1]; // 1 index of position y
}
void RSBackgroundImageModifier::SetBgImageDstRect(const Vector4f& dstRect)
{
    Setter(RSPropertyType::BG_IMAGE_RECT, dstRect);
}
Vector4f RSBackgroundImageModifier::GetBgImageDstRect() const
{
    return Getter(RSPropertyType::BG_IMAGE_RECT, Vector4f(0.f));
}

// ===========================================================================
// RSBackgroundFilterModifier
void RSBackgroundFilterModifier::SetSystemBarEffect(bool systemBarEffect)
{
    Setter<RSProperty>(RSPropertyType::SYSTEMBAREFFECT, systemBarEffect);
}
bool RSBackgroundFilterModifier::GetSystemBarEffect() const
{
    return Getter(RSPropertyType::SYSTEMBAREFFECT, false);
}
void RSBackgroundFilterModifier::SetWaterRippleProgress(float progress)
{
    Setter(RSPropertyType::WATER_RIPPLE_PROGRESS, progress);
}
float RSBackgroundFilterModifier::GetWaterRippleProgress() const
{
    return Getter(RSPropertyType::WATER_RIPPLE_PROGRESS, 0.f);
}
void RSBackgroundFilterModifier::SetWaterRippleParams(const std::optional<RSWaterRipplePara>& para)
{
    SetterOptional(RSPropertyType::WATER_RIPPLE_PARAMS, para);
}
std::optional<RSWaterRipplePara> RSBackgroundFilterModifier::GetWaterRippleParams() const
{
    return GetterOptional<RSWaterRipplePara>(RSPropertyType::WATER_RIPPLE_PARAMS);
}
void RSBackgroundFilterModifier::SetMagnifierParams(const std::shared_ptr<RSMagnifierParams>& params)
{
    Setter<RSProperty, std::shared_ptr<RSMagnifierParams>>(RSPropertyType::MAGNIFIER_PARA, params);
}
std::shared_ptr<RSMagnifierParams> RSBackgroundFilterModifier::GetMagnifierParams() const
{
    return Getter(RSPropertyType::MAGNIFIER_PARA, std::shared_ptr<RSMagnifierParams>(nullptr));
}
void RSBackgroundFilterModifier::SetBackgroundBlurRadius(float backgroundBlurRadius)
{
    Setter(RSPropertyType::BACKGROUND_BLUR_RADIUS, backgroundBlurRadius);
}
float RSBackgroundFilterModifier::GetBackgroundBlurRadius() const
{
    return Getter(RSPropertyType::BACKGROUND_BLUR_RADIUS, 0.f);
}
void RSBackgroundFilterModifier::SetBackgroundBlurSaturation(float backgroundBlurSaturation)
{
    Setter(RSPropertyType::BACKGROUND_BLUR_SATURATION, backgroundBlurSaturation);
}
float RSBackgroundFilterModifier::GetBackgroundBlurSaturation() const
{
    return Getter(RSPropertyType::BACKGROUND_BLUR_SATURATION, 0.f);
}
void RSBackgroundFilterModifier::SetBackgroundBlurBrightness(float backgroundBlurBrightness)
{
    Setter(RSPropertyType::BACKGROUND_BLUR_BRIGHTNESS, backgroundBlurBrightness);
}
float RSBackgroundFilterModifier::GetBackgroundBlurBrightness() const
{
    return Getter(RSPropertyType::BACKGROUND_BLUR_BRIGHTNESS, 0.f);
}
void RSBackgroundFilterModifier::SetBackgroundBlurMaskColor(Color backgroundMaskColor)
{
    Setter(RSPropertyType::BACKGROUND_BLUR_MASK_COLOR, backgroundMaskColor);
}
Color RSBackgroundFilterModifier::GetBackgroundBlurMaskColor() const
{
    return Getter(RSPropertyType::BACKGROUND_BLUR_MASK_COLOR, Color(RgbPalette::Transparent()));
}
void RSBackgroundFilterModifier::SetBackgroundBlurColorMode(int backgroundColorMode)
{
    Setter<RSProperty>(RSPropertyType::BACKGROUND_BLUR_COLOR_MODE, backgroundColorMode);
}
int RSBackgroundFilterModifier::GetBackgroundBlurColorMode() const
{
    return Getter(RSPropertyType::BACKGROUND_BLUR_COLOR_MODE, 0);
}
void RSBackgroundFilterModifier::SetBackgroundBlurRadiusX(float backgroundBlurRadiusX)
{
    Setter(RSPropertyType::BACKGROUND_BLUR_RADIUS_X, backgroundBlurRadiusX);
}
float RSBackgroundFilterModifier::GetBackgroundBlurRadiusX() const
{
    return Getter(RSPropertyType::BACKGROUND_BLUR_RADIUS_X, 0.f);
}
void RSBackgroundFilterModifier::SetBackgroundBlurRadiusY(float backgroundBlurRadiusY)
{
    Setter(RSPropertyType::BACKGROUND_BLUR_RADIUS_Y, backgroundBlurRadiusY);
}
float RSBackgroundFilterModifier::GetBackgroundBlurRadiusY() const
{
    return Getter(RSPropertyType::BACKGROUND_BLUR_RADIUS_Y, 0.f);
}
void RSBackgroundFilterModifier::SetBehindWindowFilterRadius(float behindWindowFilterRadius)
{
    Setter(RSPropertyType::BEHIND_WINDOW_FILTER_RADIUS, behindWindowFilterRadius);
}
float RSBackgroundFilterModifier::GetBehindWindowFilterRadius() const
{
    return Getter(RSPropertyType::BEHIND_WINDOW_FILTER_RADIUS, 0.f);
}
void RSBackgroundFilterModifier::SetBehindWindowFilterSaturation(float behindWindowFilterSaturation)
{
    Setter(RSPropertyType::BEHIND_WINDOW_FILTER_SATURATION, behindWindowFilterSaturation);
}
float RSBackgroundFilterModifier::GetBehindWindowFilterSaturation() const
{
    return Getter(RSPropertyType::BEHIND_WINDOW_FILTER_SATURATION, 0.f);
}
void RSBackgroundFilterModifier::SetBehindWindowFilterBrightness(float behindWindowFilterBrightness)
{
    Setter(RSPropertyType::BEHIND_WINDOW_FILTER_BRIGHTNESS, behindWindowFilterBrightness);
}
float RSBackgroundFilterModifier::GetBehindWindowFilterBrightness() const
{
    return Getter(RSPropertyType::BEHIND_WINDOW_FILTER_BRIGHTNESS, 0.f);
}
void RSBackgroundFilterModifier::SetBehindWindowFilterMaskColor(Color behindWindowFilterMaskColor)
{
    Setter(RSPropertyType::BEHIND_WINDOW_FILTER_MASK_COLOR, behindWindowFilterMaskColor);
}
Color RSBackgroundFilterModifier::GetBehindWindowFilterMaskColor() const
{
    return Getter(RSPropertyType::BEHIND_WINDOW_FILTER_MASK_COLOR, Color(RgbPalette::Transparent()));
}

// ===========================================================================
// RSBehindWindowFilterModifier
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

// ===========================================================================
// RSUseEffectModifier
void RSUseEffectModifier::SetUseEffect(bool useEffect)
{
    Setter<RSProperty>(RSPropertyType::USE_EFFECT, useEffect);
}
bool RSUseEffectModifier::GetUseEffect() const
{
    return Getter(RSPropertyType::USE_EFFECT, false);
}
void RSUseEffectModifier::SetUseEffectType(UseEffectType useEffectType)
{
    Setter<RSProperty>(RSPropertyType::USE_EFFECT_TYPE, static_cast<int>(useEffectType));
}
UseEffectType RSUseEffectModifier::GetUseEffectType() const
{
    return Getter(RSPropertyType::USE_EFFECT_TYPE, UseEffectType::EFFECT_COMPONENT);
}

// ===========================================================================
// RSDynamicLightUpModifier
void RSDynamicLightUpModifier::SetDynamicLightUpRate(const std::optional<float>& rate)
{
    SetterOptional<RSAnimatableProperty>(RSPropertyType::DYNAMIC_LIGHT_UP_RATE, rate);
}
std::optional<float> RSDynamicLightUpModifier::GetDynamicLightUpRate() const
{
    return GetterOptional<float>(RSPropertyType::DYNAMIC_LIGHT_UP_RATE);
}
void RSDynamicLightUpModifier::SetDynamicLightUpDegree(const std::optional<float>& lightUpDegree)
{
    SetterOptional<RSAnimatableProperty>(RSPropertyType::DYNAMIC_LIGHT_UP_DEGREE, lightUpDegree);
}
std::optional<float> RSDynamicLightUpModifier::GetDynamicLightUpDegree() const
{
    return GetterOptional<float>(RSPropertyType::DYNAMIC_LIGHT_UP_DEGREE);
}
// ===========================================================================
// RSBackgroundStyleModifier
} // namespace OHOS::Rosen::ModifierNG