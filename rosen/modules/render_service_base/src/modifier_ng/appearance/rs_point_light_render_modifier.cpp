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

#include "modifier_ng/appearance/rs_point_light_render_modifier.h"

namespace OHOS::Rosen::ModifierNG {
const RSPointLightRenderModifier::LegacyPropertyApplierMap RSPointLightRenderModifier::LegacyPropertyApplierMap_ = {
    { RSPropertyType::LIGHT_INTENSITY, RSRenderModifier::PropertyApplyHelper<float, &RSProperties::SetLightIntensity> },
    { RSPropertyType::LIGHT_COLOR, RSRenderModifier::PropertyApplyHelper<Color, &RSProperties::SetLightColor> },
    { RSPropertyType::LIGHT_POSITION,
        RSRenderModifier::PropertyApplyHelper<Vector4f, &RSProperties::SetLightPosition> },
    { RSPropertyType::ILLUMINATED_BORDER_WIDTH,
        RSRenderModifier::PropertyApplyHelper<float, &RSProperties::SetIlluminatedBorderWidth> },
    { RSPropertyType::ILLUMINATED_TYPE, RSRenderModifier::PropertyApplyHelper<int, &RSProperties::SetIlluminatedType> },
    { RSPropertyType::BLOOM, RSRenderModifier::PropertyApplyHelper<float, &RSProperties::SetBloom> },
};

void RSPointLightRenderModifier::ResetProperties(RSProperties& properties)
{
    properties.SetLightIntensity(0.f);
    properties.SetLightColor(RgbPalette::White());
    properties.SetLightPosition(Vector4f());
    properties.SetIlluminatedBorderWidth(0.f);
    properties.SetIlluminatedType(0);
    properties.SetBloom(0.f);
}
} // namespace OHOS::Rosen::ModifierNG