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

namespace OHOS::Rosen::ModifierNG {
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
    properties.SetBackgroundColor(Color());
    properties.SetBgBrightnessRates(Vector4f());
    properties.SetBgBrightnessSaturation(0.f);
    properties.SetBgBrightnessPosCoeff(Vector4f());
    properties.SetBgBrightnessNegCoeff(Vector4f());
    properties.SetBgBrightnessFract(1.f);
}
} // namespace OHOS::Rosen::ModifierNG
