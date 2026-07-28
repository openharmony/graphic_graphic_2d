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

#include "modifier_ng/appearance/rs_coverage_ng_shader_modifier.h"

namespace OHOS::Rosen::ModifierNG {
void RSCoverageNGShaderModifier::SetLightIntensity(float lightIntensity)
{
    Setter(RSPropertyType::LIGHT_INTENSITY, lightIntensity);
}

float RSCoverageNGShaderModifier::GetLightIntensity() const
{
    return Getter<float>(RSPropertyType::LIGHT_INTENSITY, {});
}

void RSCoverageNGShaderModifier::SetLightColor(const Color lightColor)
{
    Setter(RSPropertyType::LIGHT_COLOR, lightColor);
}

Color RSCoverageNGShaderModifier::GetLightColor() const
{
    return Getter<Color>(RSPropertyType::LIGHT_COLOR, {});
}

void RSCoverageNGShaderModifier::SetLightPosition(const Vector4f& lightPosition)
{
    Setter(RSPropertyType::LIGHT_POSITION, lightPosition);
}

Vector4f RSCoverageNGShaderModifier::GetLightPosition() const
{
    return Getter<Vector4f>(RSPropertyType::LIGHT_POSITION, {});
}

void RSCoverageNGShaderModifier::SetIlluminatedBorderWidth(float illuminatedBorderWidth)
{
    Setter(RSPropertyType::ILLUMINATED_BORDER_WIDTH, illuminatedBorderWidth);
}

float RSCoverageNGShaderModifier::GetIlluminatedBorderWidth() const
{
    return Getter<float>(RSPropertyType::ILLUMINATED_BORDER_WIDTH, {});
}

void RSCoverageNGShaderModifier::SetIlluminatedType(int illuminatedType)
{
    Setter<RSProperty>(RSPropertyType::ILLUMINATED_TYPE, illuminatedType);
}

int RSCoverageNGShaderModifier::GetIlluminatedType() const
{
    return Getter<int>(RSPropertyType::ILLUMINATED_TYPE, {});
}

void RSCoverageNGShaderModifier::SetBloom(float bloomIntensity)
{
    Setter(RSPropertyType::BLOOM, bloomIntensity);
}

float RSCoverageNGShaderModifier::GetBloom() const
{
    return Getter<float>(RSPropertyType::BLOOM, {});
}

void RSCoverageNGShaderModifier::SetCoverageNGShader(const std::shared_ptr<RSNGShaderBase>& coverageShader)
{
    Setter<RSProperty, std::shared_ptr<RSNGShaderBase>>(RSPropertyType::COVERAGE_NG_SHADER, coverageShader);
}

std::shared_ptr<RSNGShaderBase> RSCoverageNGShaderModifier::GetCoverageNGShader() const
{
    return Getter<std::shared_ptr<RSNGShaderBase>>(RSPropertyType::COVERAGE_NG_SHADER, nullptr);
}
} // namespace OHOS::Rosen::ModifierNG