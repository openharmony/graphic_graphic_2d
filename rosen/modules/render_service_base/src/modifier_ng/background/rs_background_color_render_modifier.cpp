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

#include "modifier_ng/background/rs_background_color_render_modifier.h"

#include "drawable/rs_property_drawable_utils.h"
#include "modifier_ng/rs_render_modifier_utils.h"
#include "pipeline/rs_recording_canvas.h"
#include "property/rs_properties_painter.h"

namespace OHOS::Rosen::ModifierNG {
constexpr int TRACE_LEVEL_TWO = 2;

const RSBackgroundColorRenderModifier::LegacyPropertyApplierMap
    RSBackgroundColorRenderModifier::LegacyPropertyApplierMap_ = {
        { RSPropertyType::BACKGROUND_COLOR, RSRenderModifier::PropertyApplyHelperAdd<Color,
                                                &RSProperties::SetBackgroundColor, &RSProperties::GetBackgroundColor> },
        { RSPropertyType::BG_BRIGHTNESS_RATES,
            RSRenderModifier::PropertyApplyHelper<Vector4f, &RSProperties::SetBgBrightnessRates> },
        { RSPropertyType::BG_BRIGHTNESS_SATURATION,
            RSRenderModifier::PropertyApplyHelper<float, &RSProperties::SetBgBrightnessSaturation> },
        { RSPropertyType::BG_BRIGHTNESS_POSCOEFF,
            RSRenderModifier::PropertyApplyHelper<Vector4f, &RSProperties::SetBgBrightnessPosCoeff> },
        { RSPropertyType::BG_BRIGHTNESS_NEGCOEFF,
            RSRenderModifier::PropertyApplyHelper<Vector4f, &RSProperties::SetBgBrightnessNegCoeff> },
        { RSPropertyType::BG_BRIGHTNESS_FRACTION,
            RSRenderModifier::PropertyApplyHelper<float, &RSProperties::SetBgBrightnessFract> },
    };

void RSBackgroundColorRenderModifier::ResetProperties(RSProperties& properties)
{
    properties.SetBackgroundColor({});
    properties.SetBgBrightnessRates({});
    properties.SetBgBrightnessSaturation(0.0);
    properties.SetBgBrightnessPosCoeff({});
    properties.SetBgBrightnessNegCoeff({});
    properties.SetBgBrightnessFract(1.0);
}

bool RSBackgroundColorRenderModifier::OnApply(RSModifierContext& context)
{
    auto bgColor = Getter<Color>(RSPropertyType::BACKGROUND_COLOR, Color(RgbPalette::Transparent()));
    if (bgColor == RgbPalette::Transparent()) {
        return false;
    }

    // regenerate stagingDrawCmdList_
    RSDisplayListModifierUpdater updater(this);
    Drawing::Canvas& canvas = *updater.GetRecordingCanvas();
    Drawing::Brush brush;
    brush.SetColor(Drawing::Color(bgColor.AsArgbInt()));
    if (IsBgBrightnessValid()) {
        if (Rosen::RSSystemProperties::GetDebugTraceLevel() >= TRACE_LEVEL_TWO) {
            RSDynamicBrightnessPara bgBrightnessParams;
            bgBrightnessParams.rates_ = Getter<Vector4f>(RSPropertyType::BG_BRIGHTNESS_RATES, Vector4(0.f));
            bgBrightnessParams.saturation_ = Getter<float>(RSPropertyType::BG_BRIGHTNESS_SATURATION, 0.f);
            bgBrightnessParams.posCoeff_ = Getter<Vector4f>(RSPropertyType::BG_BRIGHTNESS_POSCOEFF, Vector4(0.f));
            bgBrightnessParams.negCoeff_ = Getter<Vector4f>(RSPropertyType::BG_BRIGHTNESS_NEGCOEFF, Vector4(0.f));
            bgBrightnessParams.fraction_ = Getter<float>(RSPropertyType::BG_BRIGHTNESS_FRACTION, 0.f);
            stagingPropertyDescription_ = GetBgBrightnessDescription(bgBrightnessParams);
            auto blender = RSPropertyDrawableUtils::MakeDynamicBrightnessBlender(bgBrightnessParams);
            brush.SetBlender(blender);
        }
    }
    canvas.AttachBrush(brush);
    canvas.DrawRect(RSPropertiesPainter::Rect2DrawingRect(context.GetBoundsRect()));
    canvas.DetachBrush();
    return true;
}

bool RSBackgroundColorRenderModifier::IsBgBrightnessValid()
{
    return (HasProperty(RSPropertyType::BG_BRIGHTNESS_RATES) || HasProperty(RSPropertyType::BG_BRIGHTNESS_SATURATION) ||
            HasProperty(RSPropertyType::BG_BRIGHTNESS_POSCOEFF) ||
            HasProperty(RSPropertyType::BG_BRIGHTNESS_NEGCOEFF) || HasProperty(RSPropertyType::BG_BRIGHTNESS_FRACTION));
}

std::string RSBackgroundColorRenderModifier::GetBgBrightnessDescription(const RSDynamicBrightnessPara& params)
{
    std::string description =
        "BackgroundBrightnessInternal, cubicCoeff: " + std::to_string(params.rates_.x_) +
        ", quadCoeff: " + std::to_string(params.rates_.y_) + ", rate: " + std::to_string(params.rates_.z_) +
        ", lightUpDegree: " + std::to_string(params.rates_.w_) + ", saturation: " + std::to_string(params.saturation_) +
        ", fgBrightnessFract: " + std::to_string(params.fraction_);
    return description;
}
} // namespace OHOS::Rosen::ModifierNG
