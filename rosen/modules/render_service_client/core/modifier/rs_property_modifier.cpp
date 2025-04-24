/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "modifier/rs_property_modifier.h"

#include "modifier/rs_modifier_type.h"
#include "modifier/rs_render_modifier.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {

RSEnvForegroundColorModifier::RSEnvForegroundColorModifier(const std::shared_ptr<RSPropertyBase>& property)
    : RSForegroundModifier(property, RSModifierType::ENV_FOREGROUND_COLOR)
{}
RSModifierType RSEnvForegroundColorModifier::GetModifierType() const
{
    return RSModifierType::ENV_FOREGROUND_COLOR;
}
std::shared_ptr<RSRenderModifier> RSEnvForegroundColorModifier::CreateRenderModifier() const
{
    auto renderProperty = GetRenderProperty();
    auto renderModifier = std::make_shared<RSEnvForegroundColorRenderModifier>(renderProperty);
    return renderModifier;
}

RSEnvForegroundColorStrategyModifier::RSEnvForegroundColorStrategyModifier(
    const std::shared_ptr<RSPropertyBase>& property)
    : RSForegroundModifier(property, RSModifierType::ENV_FOREGROUND_COLOR_STRATEGY)
{}
RSModifierType RSEnvForegroundColorStrategyModifier::GetModifierType() const
{
    return RSModifierType::ENV_FOREGROUND_COLOR_STRATEGY;
}
std::shared_ptr<RSRenderModifier> RSEnvForegroundColorStrategyModifier::CreateRenderModifier() const
{
    auto renderProperty = GetRenderProperty();
    auto renderModifier = std::make_shared<RSEnvForegroundColorStrategyRenderModifier>(renderProperty);
    return renderModifier;
}

RSCustomClipToFrameModifier::RSCustomClipToFrameModifier(const std::shared_ptr<RSPropertyBase>& property)
    : RSForegroundModifier(property, RSModifierType::CUSTOM_CLIP_TO_FRAME)
{}

RSModifierType RSCustomClipToFrameModifier::GetModifierType() const
{
    return RSModifierType::CUSTOM_CLIP_TO_FRAME;
}

std::shared_ptr<RSRenderModifier> RSCustomClipToFrameModifier::CreateRenderModifier() const
{
    auto renderProperty = GetRenderProperty();
    auto renderModifier = std::make_shared<RSCustomClipToFrameRenderModifier>(renderProperty);
    return renderModifier;
}

RSHDRBrightnessModifier::RSHDRBrightnessModifier(const std::shared_ptr<RSPropertyBase>& property)
    : RSForegroundModifier(property, RSModifierType::HDR_BRIGHTNESS)
{}

RSModifierType RSHDRBrightnessModifier::GetModifierType() const
{
    return RSModifierType::HDR_BRIGHTNESS;
}

std::shared_ptr<RSRenderModifier> RSHDRBrightnessModifier::CreateRenderModifier() const
{
    auto renderProperty = GetRenderProperty();
    auto renderModifier = std::make_shared<RSHDRBrightnessRenderModifier>(renderProperty);
    return renderModifier;
}

RSBehindWindowFilterRadiusModifier::RSBehindWindowFilterRadiusModifier(const std::shared_ptr<RSPropertyBase>& property)
    : RSBackgroundModifier(property, RSModifierType::BEHIND_WINDOW_FILTER_RADIUS)
{
    property_->SetThresholdType(ThresholdType::COARSE);
}

RSModifierType RSBehindWindowFilterRadiusModifier::GetModifierType() const
{
    return RSModifierType::BEHIND_WINDOW_FILTER_RADIUS;
}

std::shared_ptr<RSRenderModifier> RSBehindWindowFilterRadiusModifier::CreateRenderModifier() const
{
    auto renderProperty = GetRenderProperty();
    auto renderModifier = std::make_shared<RSBehindWindowFilterRadiusRenderModifier>(renderProperty);
    return renderModifier;
}

RSBehindWindowFilterSaturationModifier::RSBehindWindowFilterSaturationModifier(
    const std::shared_ptr<RSPropertyBase>& property) : RSBackgroundModifier(property,
    RSModifierType::BEHIND_WINDOW_FILTER_SATURATION)
{
    property_->SetThresholdType(ThresholdType::COARSE);
}

RSModifierType RSBehindWindowFilterSaturationModifier::GetModifierType() const
{
    return RSModifierType::BEHIND_WINDOW_FILTER_SATURATION;
}

std::shared_ptr<RSRenderModifier> RSBehindWindowFilterSaturationModifier::CreateRenderModifier() const
{
    auto renderProperty = GetRenderProperty();
    auto renderModifier = std::make_shared<RSBehindWindowFilterSaturationRenderModifier>(renderProperty);
    return renderModifier;
}

RSBehindWindowFilterBrightnessModifier::RSBehindWindowFilterBrightnessModifier(
    const std::shared_ptr<RSPropertyBase>& property) : RSBackgroundModifier(property,
    RSModifierType::BEHIND_WINDOW_FILTER_BRIGHTNESS)
{
    property_->SetThresholdType(ThresholdType::COARSE);
}

RSModifierType RSBehindWindowFilterBrightnessModifier::GetModifierType() const
{
    return RSModifierType::BEHIND_WINDOW_FILTER_BRIGHTNESS;
}

std::shared_ptr<RSRenderModifier> RSBehindWindowFilterBrightnessModifier::CreateRenderModifier() const
{
    auto renderProperty = GetRenderProperty();
    auto renderModifier = std::make_shared<RSBehindWindowFilterBrightnessRenderModifier>(renderProperty);
    return renderModifier;
}

RSBehindWindowFilterMaskColorModifier::RSBehindWindowFilterMaskColorModifier(
    const std::shared_ptr<RSPropertyBase>& property) : RSBackgroundModifier(property,
    RSModifierType::BEHIND_WINDOW_FILTER_MASK_COLOR)
{
    property_->SetThresholdType(ThresholdType::COLOR);
}

RSModifierType RSBehindWindowFilterMaskColorModifier::GetModifierType() const
{
    return RSModifierType::BEHIND_WINDOW_FILTER_MASK_COLOR;
}

std::shared_ptr<RSRenderModifier> RSBehindWindowFilterMaskColorModifier::CreateRenderModifier() const
{
    auto renderProperty = GetRenderProperty();
    auto renderModifier = std::make_shared<RSBehindWindowFilterMaskColorRenderModifier>(renderProperty);
    return renderModifier;
}

RSComplexShaderParamModifier::RSComplexShaderParamModifier(
    const std::shared_ptr<RSPropertyBase>& property) : RSBackgroundModifier(property,
    RSModifierType::COMPLEX_SHADER_PARAM)
{}
 
RSModifierType RSComplexShaderParamModifier::GetModifierType() const
{
    return RSModifierType::COMPLEX_SHADER_PARAM;
}
 
std::shared_ptr<RSRenderModifier> RSComplexShaderParamModifier::CreateRenderModifier() const
{
    auto renderProperty = GetRenderProperty();
    auto renderModifier = std::make_shared<RSComplexShaderParamRenderModifier>(renderProperty);
    return renderModifier;
}

void RSBoundsModifier::Apply(const std::shared_ptr<RSObjAbsGeometry>& geometry)
{
    auto value = std::static_pointer_cast<RSProperty<Vector4f>>(property_)->Get();
    geometry->SetRect(value.x_, value.y_, value.z_, value.w_);
}

void RSBoundsSizeModifier::Apply(const std::shared_ptr<RSObjAbsGeometry>& geometry)
{
    auto value = std::static_pointer_cast<RSProperty<Vector2f>>(property_)->Get();
    geometry->SetSize(value.x_, value.y_);
}

void RSBoundsPositionModifier::Apply(const std::shared_ptr<RSObjAbsGeometry>& geometry)
{
    auto value = std::static_pointer_cast<RSProperty<Vector2f>>(property_)->Get();
    geometry->SetPosition(value.x_, value.y_);
}

void RSPivotModifier::Apply(const std::shared_ptr<RSObjAbsGeometry>& geometry)
{
    auto value = std::static_pointer_cast<RSProperty<Vector2f>>(property_)->Get();
    geometry->SetPivot(value.x_, value.y_);
}

void RSPivotZModifier::Apply(const std::shared_ptr<RSObjAbsGeometry>& geometry)
{
    auto value = std::static_pointer_cast<RSProperty<float>>(property_)->Get();
    geometry->SetPivotZ(value);
}

void RSQuaternionModifier::Apply(const std::shared_ptr<RSObjAbsGeometry>& geometry)
{
    auto value = std::static_pointer_cast<RSProperty<Quaternion>>(property_)->Get();
    geometry->SetQuaternion(value);
}

void RSRotationModifier::Apply(const std::shared_ptr<RSObjAbsGeometry>& geometry)
{
    auto value = std::static_pointer_cast<RSProperty<float>>(property_)->Get();
    value += geometry->GetRotation();
    geometry->SetRotation(value);
}

void RSRotationXModifier::Apply(const std::shared_ptr<RSObjAbsGeometry>& geometry)
{
    auto value = std::static_pointer_cast<RSProperty<float>>(property_)->Get();
    value += geometry->GetRotationX();
    geometry->SetRotationX(value);
}

void RSRotationYModifier::Apply(const std::shared_ptr<RSObjAbsGeometry>& geometry)
{
    auto value = std::static_pointer_cast<RSProperty<float>>(property_)->Get();
    value += geometry->GetRotationY();
    geometry->SetRotationY(value);
}

void RSCameraDistanceModifier::Apply(const std::shared_ptr<RSObjAbsGeometry>& geometry)
{
    auto value = std::static_pointer_cast<RSProperty<float>>(property_)->Get();
    geometry->SetCameraDistance(value);
}

void RSScaleModifier::Apply(const std::shared_ptr<RSObjAbsGeometry>& geometry)
{
    auto value = std::static_pointer_cast<RSProperty<Vector2f>>(property_)->Get();
    value *= Vector2f(geometry->GetScaleX(), geometry->GetScaleY());
    geometry->SetScale(value.x_, value.y_);
}

void RSScaleZModifier::Apply(const std::shared_ptr<RSObjAbsGeometry>& geometry)
{
    auto value = std::static_pointer_cast<RSProperty<float>>(property_)->Get();
    value *= geometry->GetScaleZ();
    geometry->SetScaleZ(value);
}

void RSSkewModifier::Apply(const std::shared_ptr<RSObjAbsGeometry>& geometry)
{
    auto value = std::static_pointer_cast<RSProperty<Vector3f>>(property_)->Get();
    value += Vector3f(geometry->GetSkewX(), geometry->GetSkewY(), geometry->GetSkewZ());
    geometry->SetSkew(value.x_, value.y_, value.z_);
}

void RSPerspModifier::Apply(const std::shared_ptr<RSObjAbsGeometry>& geometry)
{
    auto value = std::static_pointer_cast<RSProperty<Vector4f>>(property_)->Get();
    value = Vector4f(geometry->GetPerspX(), geometry->GetPerspY(), geometry->GetPerspZ(), geometry->GetPerspW());
    geometry->SetPersp(value.x_, value.y_, value.z_, value.w_);
}

void RSTranslateModifier::Apply(const std::shared_ptr<RSObjAbsGeometry>& geometry)
{
    auto value = std::static_pointer_cast<RSProperty<Vector2f>>(property_)->Get();
    value += Vector2f(geometry->GetTranslateX(), geometry->GetTranslateY());
    geometry->SetTranslateX(value.x_);
    geometry->SetTranslateY(value.y_);
}

void RSTranslateZModifier::Apply(const std::shared_ptr<RSObjAbsGeometry>& geometry)
{
    auto value = std::static_pointer_cast<RSProperty<float>>(property_)->Get();
    value += geometry->GetTranslateZ();
    geometry->SetTranslateZ(value);
}

#define DECLARE_ANIMATABLE_MODIFIER_CREATE(MODIFIER_NAME)                                          \
    std::shared_ptr<RSRenderModifier> RS##MODIFIER_NAME##Modifier::CreateRenderModifier() const    \
    {                                                                                              \
        auto renderProperty = GetRenderProperty();                                                 \
        auto renderModifier = std::make_shared<RS##MODIFIER_NAME##RenderModifier>(renderProperty); \
        return renderModifier;                                                                     \
    }

#define DECLARE_ANIMATABLE_MODIFIER(MODIFIER_NAME, TYPE, MODIFIER_TYPE, DELTA_OP, MODIFIER_TIER, THRESHOLD_TYPE) \
    RS##MODIFIER_NAME##Modifier::RS##MODIFIER_NAME##Modifier(const std::shared_ptr<RSPropertyBase>& property)    \
        : RS##MODIFIER_TIER##Modifier(property, RSModifierType::MODIFIER_TYPE)                                   \
    {                                                                                                            \
        property_->SetThresholdType(ThresholdType::THRESHOLD_TYPE);                                              \
    }                                                                                                            \
    RSModifierType RS##MODIFIER_NAME##Modifier::GetModifierType() const                                          \
    {                                                                                                            \
        return RSModifierType::MODIFIER_TYPE;                                                                    \
    }                                                                                                            \
    DECLARE_ANIMATABLE_MODIFIER_CREATE(MODIFIER_NAME)

#define DECLARE_NOANIMATABLE_MODIFIER_CREATE(MODIFIER_NAME)                                        \
    std::shared_ptr<RSRenderModifier> RS##MODIFIER_NAME##Modifier::CreateRenderModifier() const    \
    {                                                                                              \
        auto renderProperty = GetRenderProperty();                                                 \
        auto renderModifier = std::make_shared<RS##MODIFIER_NAME##RenderModifier>(renderProperty); \
        return renderModifier;                                                                     \
    }

#define DECLARE_NOANIMATABLE_MODIFIER(MODIFIER_NAME, TYPE, MODIFIER_TYPE, MODIFIER_TIER)                            \
    RS##MODIFIER_NAME##Modifier::RS##MODIFIER_NAME##Modifier(const std::shared_ptr<RSPropertyBase>& property)       \
        : RS##MODIFIER_TIER##Modifier(property, RSModifierType::MODIFIER_TYPE)                                      \
    {}                                                                                                              \
    RSModifierType RS##MODIFIER_NAME##Modifier::GetModifierType() const                                             \
    {                                                                                                               \
        return RSModifierType::MODIFIER_TYPE;                                                                       \
    }                                                                                                               \
    DECLARE_NOANIMATABLE_MODIFIER_CREATE(MODIFIER_NAME)

#include "modifier/rs_modifiers_def.in"

#undef DECLARE_ANIMATABLE_MODIFIER
#undef DECLARE_NOANIMATABLE_MODIFIER
} // namespace Rosen
} // namespace OHOS
