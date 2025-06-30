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

#include "modifier_ng/appearance/rs_foreground_filter_modifier.h"

namespace OHOS::Rosen::ModifierNG {
void RSForegroundFilterModifier::SetSpherize(float spherize)
{
    Setter(RSPropertyType::SPHERIZE, spherize);
}

float RSForegroundFilterModifier::GetSpherize() const
{
    return Getter(RSPropertyType::SPHERIZE, 0.f);
}

void RSForegroundFilterModifier::SetForegroundEffectRadius(float blurRadius)
{
    Setter(RSPropertyType::FOREGROUND_EFFECT_RADIUS, blurRadius);
}

float RSForegroundFilterModifier::GetForegroundEffectRadius() const
{
    return Getter(RSPropertyType::FOREGROUND_EFFECT_RADIUS, 0.f);
}

void RSForegroundFilterModifier::SetMotionBlurParam(std::shared_ptr<MotionBlurParam> param)
{
    Setter<RSProperty>(RSPropertyType::MOTION_BLUR_PARA, param);
}

std::shared_ptr<MotionBlurParam> RSForegroundFilterModifier::GetMotionBlurParam() const
{
    return Getter<std::shared_ptr<MotionBlurParam>>(RSPropertyType::MOTION_BLUR_PARA, nullptr);
}

void RSForegroundFilterModifier::SetFlyOutParams(const std::optional<RSFlyOutPara>& params)
{
    SetterOptional(RSPropertyType::FLY_OUT_PARAMS, params);
}

std::optional<RSFlyOutPara> RSForegroundFilterModifier::GetFlyOutParams() const
{
    return GetterOptional<RSFlyOutPara>(RSPropertyType::FLY_OUT_PARAMS);
}

void RSForegroundFilterModifier::SetFlyOutDegree(float degree)
{
    Setter(RSPropertyType::FLY_OUT_DEGREE, degree);
}

float RSForegroundFilterModifier::GetFlyOutDegree() const
{
    return Getter(RSPropertyType::FLY_OUT_DEGREE, 0.f);
}

void RSForegroundFilterModifier::SetDistortionK(std::optional<float> distortionK)
{
    SetterOptional<RSAnimatableProperty>(RSPropertyType::DISTORTION_K, distortionK);
}

void RSForegroundFilterModifier::SetUIFilter(std::shared_ptr<RSUIFilter> foregroundFilter)
{
    if (!foregroundFilter) {
        return;
    }
    auto iter = properties_.find(RSPropertyType::FOREGROUND_UI_FILTER);
    if (iter != properties_.end() && iter->second != nullptr) {
        std::shared_ptr<RSUIFilter> oldUIFilter = nullptr;
        auto oldProperty = std::static_pointer_cast<RSProperty<std::shared_ptr<RSUIFilter>>>(iter->second);
        if (oldProperty) {
            oldUIFilter = oldProperty->Get();
        }
        if (oldUIFilter && foregroundFilter->IsStructureSame(oldUIFilter)) {
            oldUIFilter->SetValue(foregroundFilter);
            return;
        }
    }
    AttachUIFilterProperty(foregroundFilter);
}

void RSForegroundFilterModifier::AttachUIFilterProperty(std::shared_ptr<RSUIFilter> uiFilter)
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
    AttachProperty(RSPropertyType::FOREGROUND_UI_FILTER, property);
}

std::optional<float> RSForegroundFilterModifier::GetDistortionK() const
{
    return GetterOptional<float>(RSPropertyType::DISTORTION_K);
}

void RSForegroundFilterModifier::SetAttractionFraction(float fraction)
{
    Setter(RSPropertyType::ATTRACTION_FRACTION, fraction);
}

float RSForegroundFilterModifier::GetAttractionFraction() const
{
    return Getter(RSPropertyType::ATTRACTION_FRACTION, 0.f);
}

void RSForegroundFilterModifier::SetAttractionDstPoint(const Vector2f& dstPoint)
{
    Setter(RSPropertyType::ATTRACTION_DSTPOINT, dstPoint);
}

Vector2f RSForegroundFilterModifier::GetAttractionDstPoint() const
{
    return Getter(RSPropertyType::ATTRACTION_DSTPOINT, Vector2f(0.f, 0.f));
}

void RSForegroundFilterModifier::SetNGFilterBase(std::shared_ptr<RSNGFilterBase> filter)
{
    Setter<RSProperty>(RSPropertyType::FOREGROUND_NG_FILTER, filter);
}

std::shared_ptr<RSNGFilterBase> RSForegroundFilterModifier::GetNGFilterBase() const
{
    return Getter(RSPropertyType::FOREGROUND_NG_FILTER, nullptr);
}

} // namespace OHOS::Rosen::ModifierNG
