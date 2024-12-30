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

#include "modifier_ng/rs_overlay_modifier.h"

namespace OHOS::Rosen::ModifierNG {
// =============================================
// RSPointLightModifier
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

// =============================================
// RSBorderModifier
void RSBorderModifier::SetBorderColor(const Vector4<Color>& color)
{
    Setter(RSPropertyType::BORDER_COLOR, color);
}
Vector4<Color> RSBorderModifier::GetBorderColor() const
{
    return Getter<Vector4<Color>>(RSPropertyType::BORDER_COLOR, {});
}

void RSBorderModifier::SetBorderWidth(const Vector4f& width)
{
    Setter(RSPropertyType::BORDER_WIDTH, width);
}
Vector4f RSBorderModifier::GetBorderWidth() const
{
    return Getter<Vector4f>(RSPropertyType::BORDER_WIDTH, {});
}

void RSBorderModifier::SetBorderStyle(const Vector4<uint32_t>& style)
{
    Setter<RSProperty>(RSPropertyType::BORDER_STYLE, style);
}
Vector4<uint32_t> RSBorderModifier::GetBorderStyle() const
{
    return Getter<Vector4<uint32_t>>(RSPropertyType::BORDER_STYLE, {});
}

void RSBorderModifier::SetBorderDashWidth(const Vector4f& dashWidth)
{
    Setter<RSProperty>(RSPropertyType::BORDER_DASH_WIDTH, dashWidth);
}
Vector4f RSBorderModifier::GetBorderDashWidth() const
{
    return Getter<Vector4f>(RSPropertyType::BORDER_DASH_WIDTH, {});
}

void RSBorderModifier::SetBorderDashGap(const Vector4f& dashGap)
{
    Setter<RSProperty>(RSPropertyType::BORDER_DASH_GAP, dashGap);
}
Vector4f RSBorderModifier::GetBorderDashGap() const
{
    return Getter<Vector4f>(RSPropertyType::BORDER_DASH_GAP, {});
}

// =============================================
// RSParticleEffectModifier
void RSParticleEffectModifier::SetEmitterUpdater(const std::vector<std::shared_ptr<EmitterUpdater>>& para)
{
    Setter<RSProperty, std::vector<std::shared_ptr<EmitterUpdater>>>(RSPropertyType::PARTICLE_EMITTER_UPDATER, para);
}
std::vector<std::shared_ptr<EmitterUpdater>> RSParticleEffectModifier::GetEmitterUpdater() const
{
    return Getter<std::vector<std::shared_ptr<EmitterUpdater>>>(RSPropertyType::PARTICLE_EMITTER_UPDATER, {});
}

void RSParticleEffectModifier::SetParticleNoiseFields(const std::shared_ptr<ParticleNoiseFields>& para)
{
    Setter<RSProperty, std::shared_ptr<ParticleNoiseFields>>(RSPropertyType::PARTICLE_NOISE_FIELD, para);
}
std::shared_ptr<ParticleNoiseFields> RSParticleEffectModifier::GetParticleNoiseFields() const
{
    return Getter<std::shared_ptr<ParticleNoiseFields>>(RSPropertyType::PARTICLE_NOISE_FIELD, {});
}

// =============================================
// RSPixelStretchModifier
void RSPixelStretchModifier::SetPixelStretchSize(const Vector4f& stretchSize)
{
    Setter(RSPropertyType::PIXEL_STRETCH_SIZE, stretchSize);
}
Vector4f RSPixelStretchModifier::GetPixelStretchSize() const
{
    return Getter<Vector4f>(RSPropertyType::PIXEL_STRETCH_SIZE, {});
}

void RSPixelStretchModifier::SetPixelStretchTileMode(int stretchTileMode)
{
    Setter<RSProperty>(RSPropertyType::PIXEL_STRETCH_TILE_MODE, stretchTileMode);
}
int RSPixelStretchModifier::GetPixelStretchTileMode() const
{
    return Getter<int>(RSPropertyType::PIXEL_STRETCH_TILE_MODE, {});
}

void RSPixelStretchModifier::SetPixelStretchPercent(const Vector4f& stretchPercent)
{
    Setter(RSPropertyType::PIXEL_STRETCH_PERCENT, stretchPercent);
}
Vector4f RSPixelStretchModifier::GetPixelStretchPercent() const
{
    return Getter<Vector4f>(RSPropertyType::PIXEL_STRETCH_PERCENT, {});
}
} // namespace OHOS::Rosen::ModifierNG
