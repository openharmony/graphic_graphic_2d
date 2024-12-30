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

#ifndef RENDER_SERVICE_CLIENT_CORE_MODIFIER_NG_RS_OVERLAY_MODIFIER_H
#define RENDER_SERVICE_CLIENT_CORE_MODIFIER_NG_RS_OVERLAY_MODIFIER_H

#include "modifier_ng/rs_extended_modifier.h"
#include "modifier_ng/rs_modifier_ng.h"
#include "render/rs_border.h"

namespace OHOS::Rosen::ModifierNG {
class RSC_EXPORT RSPointLightModifier : public RSModifier {
public:
    RSPointLightModifier() = default;
    ~RSPointLightModifier() override = default;

    static inline constexpr auto Type = ModifierNG::RSModifierType::POINT_LIGHT;
    ModifierNG::RSModifierType GetType() const override
    {
        return Type;
    };

    void SetLightIntensity(float lightIntensity);
    float GetLightIntensity() const;
    void SetLightColor(const Color lightColor);
    Color GetLightColor() const;
    void SetLightPosition(const Vector4f& lightPosition);
    Vector4f GetLightPosition() const;
    void SetIlluminatedBorderWidth(float illuminatedBorderWidth);
    float GetIlluminatedBorderWidth() const;
    void SetIlluminatedType(int illuminatedType);
    int GetIlluminatedType() const;
    void SetBloom(float bloomIntensity);
    float GetBloom() const;
};

class RSC_EXPORT RSBorderModifier : public RSModifier {
public:
    RSBorderModifier() = default;
    ~RSBorderModifier() override = default;

    static inline constexpr auto Type = ModifierNG::RSModifierType::BORDER;
    ModifierNG::RSModifierType GetType() const override
    {
        return Type;
    };

    void SetBorderColor(const Vector4<Color>& color);
    Vector4<Color> GetBorderColor() const;
    void SetBorderWidth(const Vector4f& width);
    Vector4f GetBorderWidth() const;
    void SetBorderStyle(const Vector4<uint32_t>& style);
    Vector4<uint32_t> GetBorderStyle() const;
    void SetBorderDashWidth(const Vector4f& dashWidth);
    Vector4f GetBorderDashWidth() const;
    void SetBorderDashGap(const Vector4f& dashGap);
    Vector4f GetBorderDashGap() const;
};

class RSC_EXPORT RSParticleEffectModifier : public RSModifier {
public:
    RSParticleEffectModifier() = default;
    ~RSParticleEffectModifier() override = default;

    static inline constexpr auto Type = ModifierNG::RSModifierType::PARTICLE_EFFECT;
    ModifierNG::RSModifierType GetType() const override
    {
        return Type;
    };

    void SetEmitterUpdater(const std::vector<std::shared_ptr<EmitterUpdater>>& para);
    std::vector<std::shared_ptr<EmitterUpdater>> GetEmitterUpdater() const;
    void SetParticleNoiseFields(const std::shared_ptr<ParticleNoiseFields>& para);
    std::shared_ptr<ParticleNoiseFields> GetParticleNoiseFields() const;
};

class RSC_EXPORT RSPixelStretchModifier : public RSModifier {
public:
    RSPixelStretchModifier() = default;
    ~RSPixelStretchModifier() override = default;

    static inline constexpr auto Type = ModifierNG::RSModifierType::PIXEL_STRETCH;
    ModifierNG::RSModifierType GetType() const override
    {
        return Type;
    };

    void SetPixelStretchSize(const Vector4f& stretchSize);
    Vector4f GetPixelStretchSize() const;
    void SetPixelStretchTileMode(int stretchTileMode);
    int GetPixelStretchTileMode() const;
    void SetPixelStretchPercent(const Vector4f& stretchPercent);
    Vector4f GetPixelStretchPercent() const;
};

class RSC_EXPORT RSOverlayStyleModifier : public RSExtendedModifier {
public:
    RSOverlayStyleModifier() = default;
    ~RSOverlayStyleModifier() override = default;

    static inline constexpr auto Type = ModifierNG::RSModifierType::OVERLAY_STYLE;
    ModifierNG::RSModifierType GetType() const override
    {
        return Type;
    };
    void Draw(RSDrawingContext& context) const override
    {
        return;
    }

protected:
    RSPropertyType GetInnerPropertyType() const override
    {
        return RSPropertyType::OVERLAY_STYLE;
    }
};
} // namespace OHOS::Rosen::ModifierNG
#endif // RENDER_SERVICE_CLIENT_CORE_MODIFIER_NG_RS_OVERLAY_MODIFIER_H
