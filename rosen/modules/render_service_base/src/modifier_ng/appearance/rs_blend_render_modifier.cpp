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

namespace OHOS::Rosen::ModifierNG {
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
        RSRenderModifier::PropertyApplyHelper<float, &RSProperties::SetFgBrightnessFract> },
    { RSPropertyType::FG_BRIGHTNESS_HDR,
        RSRenderModifier::PropertyApplyHelper<bool, &RSProperties::SetFgBrightnessHdr> }
};

void RSBlendRenderModifier::ResetProperties(RSProperties& properties)
{
    properties.SetColorBlendMode(0);
    properties.SetColorBlendApplyType(0);
    properties.SetFgBrightnessRates(Vector4f());
    properties.SetFgBrightnessSaturation(0.f);
    properties.SetFgBrightnessPosCoeff(Vector4f());
    properties.SetFgBrightnessNegCoeff(Vector4f());
    properties.SetFgBrightnessFract(1.f);
    properties.SetFgBrightnessHdr(false);
}
} // namespace OHOS::Rosen::ModifierNG
