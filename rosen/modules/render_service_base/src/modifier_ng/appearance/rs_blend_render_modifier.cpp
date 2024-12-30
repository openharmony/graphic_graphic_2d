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

#include "modifier_ng/appearance/rs_blend_render_modifier.h"

#include "common/rs_optional_trace.h"
#include "drawable/rs_property_drawable_utils.h"
#include "property/rs_properties.h"
#include "property/rs_properties_def.h"

namespace OHOS::Rosen::ModifierNG {
constexpr int TRACE_LEVEL_TWO = 2;

const RSBlendRenderModifier::LegacyPropertyApplierMap RSBlendRenderModifier::LegacyPropertyApplierMap_ = {
    { RSPropertyType::COLOR_BLEND_MODE, RSRenderModifier::PropertyApplyHelper<int, &RSProperties::SetColorBlendMode> },
    { RSPropertyType::COLOR_BLEND_APPLY_TYPE,
        RSRenderModifier::PropertyApplyHelper<int, &RSProperties::SetColorBlendApplyType> },
    { RSPropertyType::FG_BRIGHTNESS_RATES,
        RSRenderModifier::PropertyApplyHelper<Vector4f, &RSProperties::SetFgBrightnessRates> },
    { RSPropertyType::FG_BRIGHTNESS_SATURATION,
        RSRenderModifier::PropertyApplyHelper<float, &RSProperties::SetFgBrightnessSaturation> },
    { RSPropertyType::FG_BRIGHTNESS_POSCOEFF,
        RSRenderModifier::PropertyApplyHelper<Vector4f, &RSProperties::SetFgBrightnessPosCoeff> },
    { RSPropertyType::FG_BRIGHTNESS_NEGCOEFF,
        RSRenderModifier::PropertyApplyHelper<Vector4f, &RSProperties::SetFgBrightnessNegCoeff> },
    { RSPropertyType::FG_BRIGHTNESS_FRACTION,
        RSRenderModifier::PropertyApplyHelper<float, &RSProperties::SetFgBrightnessFract> }
};

void RSBlendRenderModifier::ResetProperties(RSProperties& properties)
{
    properties.SetColorBlendMode(0);
    properties.SetColorBlendApplyType(0);
    properties.SetFgBrightnessRates({});
    properties.SetFgBrightnessSaturation(0.0);
    properties.SetFgBrightnessPosCoeff({});
    properties.SetFgBrightnessNegCoeff({});
    properties.SetFgBrightnessFract({});
}

void RSBlendRenderModifier::Draw(RSPaintFilterCanvas& canvas, Drawing::Rect& rect)
{
    // TODO
}

/*
 * Protected Methods
 */
bool RSBlendRenderModifier::OnApply(RSModifierContext& context)
{
    stagingColorBlendMode_ = Getter(RSPropertyType::COLOR_BLEND_MODE, 0);
    stagingColorBlendApplyType_ = Getter(RSPropertyType::COLOR_BLEND_APPLY_TYPE, 0);

    if (HasProperty(RSPropertyType::FG_BRIGHTNESS_FRACTION) &&
        ROSEN_LNE(Getter<float>(RSPropertyType::FG_BRIGHTNESS_FRACTION), 1.0f)) {
        RSDynamicBrightnessPara para;
        para.rates_ = Getter(RSPropertyType::FG_BRIGHTNESS_RATES, Vector4f());
        para.saturation_ = Getter(RSPropertyType::FG_BRIGHTNESS_SATURATION, 0.0f);
        para.posCoeff_ = Getter(RSPropertyType::FG_BRIGHTNESS_POSCOEFF, Vector4f());
        para.negCoeff_ = Getter(RSPropertyType::FG_BRIGHTNESS_NEGCOEFF, Vector4f());
        para.fraction_ = Getter(RSPropertyType::FG_BRIGHTNESS_FRACTION, 1.0f);
        if (Rosen::RSSystemProperties::GetDebugTraceLevel() >= TRACE_LEVEL_TWO) {
            stagingPropertyDescription_ = "ForegroundBrightness, cubicCoeff: " + std::to_string(para.rates_.x_) +
                                          ", quadCoeff: " + std::to_string(para.rates_.y_) +
                                          ", rate: " + std::to_string(para.rates_.z_) +
                                          ", lightUpDegree: " + std::to_string(para.rates_.w_) +
                                          ", saturation: " + std::to_string(para.saturation_) +
                                          ", fgBrightnessFract: " + std::to_string(para.fraction_);
        }
        stagingBlender_ = RSPropertyDrawableUtils::MakeDynamicBrightnessBlender(para);
        stagingIsDangerous_ = false;
    } else if (stagingColorBlendMode_ && stagingColorBlendMode_ != static_cast<int>(RSColorBlendMode::NONE)) {
        if (Rosen::RSSystemProperties::GetDebugTraceLevel() >= TRACE_LEVEL_TWO) {
            stagingPropertyDescription_ = "BlendMode, blendMode: " + std::to_string(stagingColorBlendMode_) +
                                          " blendApplyType: " + std::to_string(stagingColorBlendApplyType_);
        }
        // map blendMode to Drawing::BlendMode and convert to Blender
        stagingBlender_ =
            Drawing::Blender::CreateWithBlendMode(static_cast<Drawing::BlendMode>(stagingColorBlendMode_ - 1));
        stagingIsDangerous_ =
            RSPropertyDrawableUtils::IsDangerousBlendMode(stagingColorBlendMode_ - 1, stagingColorBlendApplyType_);
    } else {
        stagingBlender_.reset();
    }
    return (stagingBlender_ != nullptr);
}

void RSBlendRenderModifier::OnSync()
{
    std::swap(stagingColorBlendMode_, renderColorBlendMode_);
    std::swap(stagingColorBlendApplyType_, renderColorBlendApplyType_);
    renderBlender_.reset();
    std::swap(stagingBlender_, renderBlender_);
    std::swap(stagingPropertyDescription_, renderPropertyDescription_);
    std::swap(stagingIsDangerous_, renderIsDangerous_);
}
} // namespace OHOS::Rosen::ModifierNG
