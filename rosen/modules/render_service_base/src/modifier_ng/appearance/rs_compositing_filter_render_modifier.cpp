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

#include "modifier_ng/appearance/rs_compositing_filter_render_modifier.h"

namespace OHOS::Rosen::ModifierNG {
const RSCompositingFilterRenderModifier::LegacyPropertyApplierMap
    RSCompositingFilterRenderModifier::LegacyPropertyApplierMap_ = {
        { RSPropertyType::AIINVERT, RSRenderModifier::PropertyApplyHelperAdd<Vector4f, &RSProperties::SetAiInvert,
                                        &RSProperties::GetAiInvert> },
        { RSPropertyType::GRAY_SCALE,
            RSRenderModifier::PropertyApplyHelperAdd<float, &RSProperties::SetGrayScale, &RSProperties::GetGrayScale> },
        { RSPropertyType::BRIGHTNESS, RSRenderModifier::PropertyApplyHelperAdd<float, &RSProperties::SetBrightness,
                                          &RSProperties::GetBrightness> },
        { RSPropertyType::CONTRAST,
            RSRenderModifier::PropertyApplyHelperAdd<float, &RSProperties::SetContrast, &RSProperties::GetContrast> },
        { RSPropertyType::SATURATE,
            RSRenderModifier::PropertyApplyHelperAdd<float, &RSProperties::SetSaturate, &RSProperties::GetSaturate> },
        { RSPropertyType::SEPIA,
            RSRenderModifier::PropertyApplyHelperAdd<float, &RSProperties::SetSepia, &RSProperties::GetSepia> },
        { RSPropertyType::INVERT,
            RSRenderModifier::PropertyApplyHelperAdd<float, &RSProperties::SetInvert, &RSProperties::GetInvert> },
        { RSPropertyType::HUE_ROTATE,
            RSRenderModifier::PropertyApplyHelperAdd<float, &RSProperties::SetHueRotate, &RSProperties::GetHueRotate> },
        { RSPropertyType::COLOR_BLEND, RSRenderModifier::PropertyApplyHelperAdd<Color, &RSProperties::SetColorBlend,
                                           &RSProperties::GetColorBlend> },
        { RSPropertyType::LIGHT_UP_EFFECT_DEGREE,
            RSRenderModifier::PropertyApplyHelper<float, &RSProperties::SetLightUpEffect> },
        { RSPropertyType::DYNAMIC_DIM_DEGREE,
            RSRenderModifier::PropertyApplyHelper<float, &RSProperties::SetDynamicDimDegree> },
        { RSPropertyType::LINEAR_GRADIENT_BLUR_PARA,
            RSRenderModifier::PropertyApplyHelper<std::shared_ptr<RSLinearGradientBlurPara>,
                &RSProperties::SetLinearGradientBlurPara> },
        { RSPropertyType::FOREGROUND_BLUR_RADIUS,
            RSRenderModifier::PropertyApplyHelperAdd<float, &RSProperties::SetForegroundBlurRadius,
                &RSProperties::GetForegroundBlurRadius> },
        { RSPropertyType::FOREGROUND_BLUR_SATURATION,
            RSRenderModifier::PropertyApplyHelper<float, &RSProperties::SetForegroundBlurSaturation> },
        { RSPropertyType::FOREGROUND_BLUR_BRIGHTNESS,
            RSRenderModifier::PropertyApplyHelper<float, &RSProperties::SetForegroundBlurBrightness> },
        { RSPropertyType::FOREGROUND_BLUR_MASK_COLOR,
            RSRenderModifier::PropertyApplyHelperAdd<Color, &RSProperties::SetForegroundBlurMaskColor,
                &RSProperties::GetForegroundBlurMaskColor> },
        { RSPropertyType::FOREGROUND_BLUR_COLOR_MODE,
            RSRenderModifier::PropertyApplyHelper<int, &RSProperties::SetForegroundBlurColorMode> },
        { RSPropertyType::FOREGROUND_BLUR_RADIUS_X,
            RSRenderModifier::PropertyApplyHelperAdd<float, &RSProperties::SetForegroundBlurRadiusX,
                &RSProperties::GetForegroundBlurRadiusX> },
        { RSPropertyType::FOREGROUND_BLUR_RADIUS_Y,
            RSRenderModifier::PropertyApplyHelperAdd<float, &RSProperties::SetForegroundBlurRadiusY,
                &RSProperties::GetForegroundBlurRadiusY> },
        { RSPropertyType::FG_BLUR_DISABLE_SYSTEM_ADAPTATION,
            RSRenderModifier::PropertyApplyHelper<bool, &RSProperties::SetFgBlurDisableSystemAdaptation> },
    };

void RSCompositingFilterRenderModifier::ResetProperties(RSProperties& properties)
{
    properties.SetAiInvert(std::nullopt);
    properties.SetGrayScale(std::nullopt);
    properties.SetBrightness(std::nullopt);
    properties.SetContrast(std::nullopt);
    properties.SetSaturate(std::nullopt);
    properties.SetSepia(std::nullopt);
    properties.SetInvert(std::nullopt);
    properties.SetHueRotate(std::nullopt);
    properties.SetColorBlend(std::nullopt);
    properties.SetLightUpEffect(1.f);
    properties.SetDynamicDimDegree(std::nullopt);
    properties.SetLinearGradientBlurPara(nullptr);
    properties.SetForegroundBlurRadius(0.f);
    properties.SetForegroundBlurSaturation(1.f);
    properties.SetForegroundBlurBrightness(1.f);
    properties.SetForegroundBlurMaskColor(Color());
    properties.SetForegroundBlurColorMode(0);
    properties.SetForegroundBlurRadiusX(0.f);
    properties.SetForegroundBlurRadiusY(0.f);
    properties.SetFgBlurDisableSystemAdaptation(true);
}
} // namespace OHOS::Rosen::ModifierNG
