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

#include "modifier_ng/appearance/rs_background_filter_modifier.h"

namespace OHOS::Rosen::ModifierNG {
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
    return Getter<std::shared_ptr<RSMagnifierParams>>(RSPropertyType::MAGNIFIER_PARA, nullptr);
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
    return Getter(RSPropertyType::BACKGROUND_BLUR_SATURATION, 1.f);
}

void RSBackgroundFilterModifier::SetBackgroundBlurBrightness(float backgroundBlurBrightness)
{
    Setter(RSPropertyType::BACKGROUND_BLUR_BRIGHTNESS, backgroundBlurBrightness);
}

float RSBackgroundFilterModifier::GetBackgroundBlurBrightness() const
{
    return Getter(RSPropertyType::BACKGROUND_BLUR_BRIGHTNESS, 1.f);
}
void RSBackgroundFilterModifier::SetBackgroundBlurMaskColor(Color backgroundMaskColor)
{
    Setter(RSPropertyType::BACKGROUND_BLUR_MASK_COLOR, backgroundMaskColor);
}

Color RSBackgroundFilterModifier::GetBackgroundBlurMaskColor() const
{
    return Getter(RSPropertyType::BACKGROUND_BLUR_MASK_COLOR, RSColor());
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

void RSBackgroundFilterModifier::SetBgBlurDisableSystemAdaptation(bool disableSystemAdaptation)
{
    Setter<RSProperty>(RSPropertyType::BG_BLUR_DISABLE_SYSTEM_ADAPTATION, disableSystemAdaptation);
}

bool RSBackgroundFilterModifier::GetBgBlurDisableSystemAdaptation() const
{
    return Getter(RSPropertyType::BG_BLUR_DISABLE_SYSTEM_ADAPTATION, true);
}

void RSBackgroundFilterModifier::SetAlwaysSnapshot(bool enable)
{
    Setter<RSProperty>(RSPropertyType::ALWAYS_SNAPSHOT, enable);
}

bool RSBackgroundFilterModifier::GetAlwaysSnapshot() const
{
    return Getter(RSPropertyType::ALWAYS_SNAPSHOT, false);
}

void RSBackgroundFilterModifier::SetGreyCoef(const std::optional<Vector2f>& greyCoef)
{
    SetterOptional<RSAnimatableProperty>(RSPropertyType::GREY_COEF, greyCoef);
}

std::optional<Vector2f> RSBackgroundFilterModifier::GetGreyCoef() const
{
    return GetterOptional<Vector2f>(RSPropertyType::GREY_COEF);
}

void RSBackgroundFilterModifier::SetUIFilter(std::shared_ptr<RSUIFilter> backgroundFilter)
{
    if (!backgroundFilter) {
        return;
    }
    auto iter = properties_.find(RSPropertyType::BACKGROUND_UI_FILTER);
    if (iter != properties_.end() && iter->second != nullptr) {
        std::shared_ptr<RSUIFilter> oldUIFilter = nullptr;
        auto oldProperty = std::static_pointer_cast<RSProperty<std::shared_ptr<RSUIFilter>>>(iter->second);
        if (oldProperty) {
            oldUIFilter = oldProperty->Get();
        }
        if (oldUIFilter && backgroundFilter->IsStructureSame(oldUIFilter)) {
            oldUIFilter->SetValue(backgroundFilter);
            return;
        }
    }
    AttachUIFilterProperty(backgroundFilter);
}

void RSBackgroundFilterModifier::AttachUIFilterProperty(std::shared_ptr<RSUIFilter> uiFilter)
{
    if (!uiFilter) {
        return;
    }
    auto property = std::make_shared<RSProperty<std::shared_ptr<RSUIFilter>>>(uiFilter);
    auto node = node_.lock();
    if (!node) {
        return;
    }
    for (auto type : uiFilter->GetUIFilterTypes()) {
        auto paraGroup = uiFilter->GetUIFilterPara(type);
        if (!paraGroup) {
            continue;
        }
        for (auto& prop : paraGroup->GetLeafProperties()) {
            if (!prop) {
                continue;
            }
            prop->target_ = node;
            node->RegisterProperty(prop);
        }
    }
    AttachProperty(RSPropertyType::BACKGROUND_UI_FILTER, property);
}

void RSBackgroundFilterModifier::SetNGFilterBase(std::shared_ptr<RSNGFilterBase> filter)
{
    Setter<RSProperty>(RSPropertyType::BACKGROUND_NG_FILTER, filter);
}

std::shared_ptr<RSNGFilterBase> RSBackgroundFilterModifier::GetNGFilterBase() const
{
    return Getter(RSPropertyType::BACKGROUND_NG_FILTER, nullptr);
}

} // namespace OHOS::Rosen::ModifierNG
