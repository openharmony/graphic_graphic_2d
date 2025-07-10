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

#include "modifier_ng/appearance/rs_point_light_modifier.h"

namespace OHOS::Rosen::ModifierNG {
void RSPointLightModifier::SetLightIntensity(float lightIntensity)
{
    Setter(RSPropertyType::LIGHT_INTENSITY, lightIntensity);
}

float RSPointLightModifier::GetLightIntensity() const
{
    return Getter<float>(RSPropertyType::LIGHT_INTENSITY, {});
}

void RSPointLightModifier::SetLightColor(const Color lightColor)
{
    Setter(RSPropertyType::LIGHT_COLOR, lightColor);
}

Color RSPointLightModifier::GetLightColor() const
{
    return Getter<Color>(RSPropertyType::LIGHT_COLOR, {});
}

void RSPointLightModifier::SetLightPosition(const Vector4f& lightPosition)
{
    Setter(RSPropertyType::LIGHT_POSITION, lightPosition);
}

Vector4f RSPointLightModifier::GetLightPosition() const
{
    return Getter<Vector4f>(RSPropertyType::LIGHT_POSITION, {});
}

void RSPointLightModifier::SetIlluminatedBorderWidth(float illuminatedBorderWidth)
{
    Setter(RSPropertyType::ILLUMINATED_BORDER_WIDTH, illuminatedBorderWidth);
}

float RSPointLightModifier::GetIlluminatedBorderWidth() const
{
    return Getter<float>(RSPropertyType::ILLUMINATED_BORDER_WIDTH, {});
}

void RSPointLightModifier::SetIlluminatedType(int illuminatedType)
{
    Setter<RSProperty>(RSPropertyType::ILLUMINATED_TYPE, illuminatedType);
}

int RSPointLightModifier::GetIlluminatedType() const
{
    return Getter<int>(RSPropertyType::ILLUMINATED_TYPE, {});
}

void RSPointLightModifier::SetBloom(float bloomIntensity)
{
    Setter(RSPropertyType::BLOOM, bloomIntensity);
}

float RSPointLightModifier::GetBloom() const
{
    return Getter<float>(RSPropertyType::BLOOM, {});
}
} // namespace OHOS::Rosen::ModifierNG