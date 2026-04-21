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

#include "modifier_ng/appearance/rs_overlay_ng_shader_modifier.h"

namespace OHOS::Rosen::ModifierNG {
void RSOverlayNGShaderModifier::SetLightIntensity(float lightIntensity)
{
    Setter(RSPropertyType::LIGHT_INTENSITY, lightIntensity);
}

float RSOverlayNGShaderModifier::GetLightIntensity() const
{
    return Getter<float>(RSPropertyType::LIGHT_INTENSITY, {});
}

void RSOverlayNGShaderModifier::SetLightColor(const Color lightColor)
{
    Setter(RSPropertyType::LIGHT_COLOR, lightColor);
}

Color RSOverlayNGShaderModifier::GetLightColor() const
{
    return Getter<Color>(RSPropertyType::LIGHT_COLOR, {});
}

void RSOverlayNGShaderModifier::SetLightPosition(const Vector4f& lightPosition)
{
    Setter(RSPropertyType::LIGHT_POSITION, lightPosition);
}

Vector4f RSOverlayNGShaderModifier::GetLightPosition() const
{
    return Getter<Vector4f>(RSPropertyType::LIGHT_POSITION, {});
}

void RSOverlayNGShaderModifier::SetIlluminatedBorderWidth(float illuminatedBorderWidth)
{
    Setter(RSPropertyType::ILLUMINATED_BORDER_WIDTH, illuminatedBorderWidth);
}

float RSOverlayNGShaderModifier::GetIlluminatedBorderWidth() const
{
    return Getter<float>(RSPropertyType::ILLUMINATED_BORDER_WIDTH, {});
}

void RSOverlayNGShaderModifier::SetIlluminatedType(int illuminatedType)
{
    Setter<RSProperty>(RSPropertyType::ILLUMINATED_TYPE, illuminatedType);
}

int RSOverlayNGShaderModifier::GetIlluminatedType() const
{
    return Getter<int>(RSPropertyType::ILLUMINATED_TYPE, {});
}

void RSOverlayNGShaderModifier::SetBloom(float bloomIntensity)
{
    Setter(RSPropertyType::BLOOM, bloomIntensity);
}

float RSOverlayNGShaderModifier::GetBloom() const
{
    return Getter<float>(RSPropertyType::BLOOM, {});
}

void RSOverlayNGShaderModifier::SetOverlayNGShader(const std::shared_ptr<RSNGShaderBase>& overlayShader)
{
    Setter<RSProperty, std::shared_ptr<RSNGShaderBase>>(RSPropertyType::OVERLAY_NG_SHADER, overlayShader);
}

std::shared_ptr<RSNGShaderBase> RSOverlayNGShaderModifier::GetOverlayNGShader() const
{
    return Getter<std::shared_ptr<RSNGShaderBase>>(RSPropertyType::OVERLAY_NG_SHADER, nullptr);
}
} // namespace OHOS::Rosen::ModifierNG