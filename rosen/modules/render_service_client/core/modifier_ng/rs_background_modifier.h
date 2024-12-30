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

#ifndef RENDER_SERVICE_CLIENT_CORE_MODIFIER_NG_RS_BACKGROUND_MODIFIER_H
#define RENDER_SERVICE_CLIENT_CORE_MODIFIER_NG_RS_BACKGROUND_MODIFIER_H

#include "modifier_ng/rs_extended_modifier.h"
#include "modifier_ng/rs_modifier_ng.h"

namespace OHOS::Rosen::ModifierNG {
class RSC_EXPORT RSBackgroundColorModifier : public RSModifier {
public:
    RSBackgroundColorModifier() = default;
    ~RSBackgroundColorModifier() override = default;

    static inline constexpr auto Type = ModifierNG::RSModifierType::BACKGROUND_COLOR;
    ModifierNG::RSModifierType GetType() const override
    {
        return Type;
    };

    void SetBackgroundColor(Color color);

    void SetBgBrightnessRates(const Vector4f& rates);
    void SetBgBrightnessSaturation(float saturation);
    void SetBgBrightnessPosCoeff(const Vector4f& coeff);
    void SetBgBrightnessNegCoeff(const Vector4f& coeff);
    void SetBgBrightnessFract(float fraction);

    Color GetBackgroundColor() const;

    Vector4f GetBgBrightnessRates() const;
    float GetBgBrightnessSaturation() const;
    Vector4f GetBgBrightnessPosCoeff() const;
    Vector4f GetBgBrightnessNegCoeff() const;
    float GetBgBrightnessFract() const;

protected:
};

class RSC_EXPORT RSBackgroundShaderModifier : public RSModifier {
public:
    RSBackgroundShaderModifier() = default;
    ~RSBackgroundShaderModifier() override = default;

    static inline constexpr auto Type = ModifierNG::RSModifierType::BACKGROUND_SHADER;
    ModifierNG::RSModifierType GetType() const override
    {
        return Type;
    };

    void SetBackgroundShader(const std::shared_ptr<RSShader>& shader);
    void SetBackgroundShaderProgress(float progress);

    std::shared_ptr<RSShader> GetBackgroundShader() const;
    float GetBackgroundShaderProgress() const;

protected:
};

class RSC_EXPORT RSBackgroundImageModifier : public RSModifier {
public:
    RSBackgroundImageModifier() = default;
    ~RSBackgroundImageModifier() override = default;

    static inline constexpr auto Type = ModifierNG::RSModifierType::BACKGROUND_IMAGE;
    ModifierNG::RSModifierType GetType() const override
    {
        return Type;
    };

    void SetBgImage(const std::shared_ptr<RSImage>& image);
    void SetBgImageInnerRect(const Vector4f& innerRect);
    void SetBgImageWidth(float width);
    void SetBgImageHeight(float height);
    void SetBgImagePositionX(float positionX);
    void SetBgImagePositionY(float positionY);
    void SetBgImageDstRect(const Vector4f& dstRect);

    std::shared_ptr<RSImage> GetBgImage() const;
    Vector4f GetBgImageInnerRect() const;
    float GetBgImageWidth() const;
    float GetBgImageHeight() const;
    float GetBgImagePositionX() const;
    float GetBgImagePositionY() const;
    Vector4f GetBgImageDstRect() const;

protected:
};

// class RSC_EXPORT RSBackgroundFilterModifier : public RSFilterModifier {
class RSC_EXPORT RSBackgroundFilterModifier : public RSModifier {
public:
    RSBackgroundFilterModifier() = default;
    ~RSBackgroundFilterModifier() override = default;

    static inline constexpr auto Type = ModifierNG::RSModifierType::BACKGROUND_FILTER;
    ModifierNG::RSModifierType GetType() const override
    {
        return Type;
    };

    void SetBackgroundFilter(const std::shared_ptr<RSFilter>& backgroundFilter);

    void SetSystemBarEffect(bool systemBarEffect);
    void SetWaterRippleProgress(float progress);
    void SetWaterRippleParams(const std::optional<RSWaterRipplePara>& para);

    void SetMagnifierParams(const std::shared_ptr<RSMagnifierParams>& params);
    void SetBackgroundBlurRadius(float backgroundBlurRadius);
    void SetBackgroundBlurSaturation(float backgroundBlurSaturation);
    void SetBackgroundBlurBrightness(float backgroundBlurBrightness);
    void SetBackgroundBlurMaskColor(Color backgroundMaskColor);
    void SetBackgroundBlurColorMode(int backgroundColorMode);
    void SetBackgroundBlurRadiusX(float backgroundBlurRadiusX);
    void SetBackgroundBlurRadiusY(float backgroundBlurRadiusY);

    void SetBehindWindowFilterRadius(float behindWindowFilterRadius);
    void SetBehindWindowFilterSaturation(float behindWindowFilterSaturation);
    void SetBehindWindowFilterBrightness(float behindWindowFilterBrightness);
    void SetBehindWindowFilterMaskColor(Color behindWindowFilterMaskColor);

    bool GetSystemBarEffect() const;
    float GetWaterRippleProgress() const;
    std::optional<RSWaterRipplePara> GetWaterRippleParams() const;

    std::shared_ptr<RSMagnifierParams> GetMagnifierParams() const;
    float GetBackgroundBlurRadius() const;
    float GetBackgroundBlurSaturation() const;
    float GetBackgroundBlurBrightness() const;
    Color GetBackgroundBlurMaskColor() const;
    int GetBackgroundBlurColorMode() const;
    float GetBackgroundBlurRadiusX() const;
    float GetBackgroundBlurRadiusY() const;

    float GetBehindWindowFilterRadius() const;
    float GetBehindWindowFilterSaturation() const;
    float GetBehindWindowFilterBrightness() const;
    Color GetBehindWindowFilterMaskColor() const;

protected:
};

class RSC_EXPORT RSBehindWindowFilterModifier : public RSModifier {
public:
    RSBehindWindowFilterModifier() = default;
    ~RSBehindWindowFilterModifier() override = default;

    static inline constexpr auto Type = ModifierNG::RSModifierType::BEHIND_WINDOW_FILTER;
    ModifierNG::RSModifierType GetType() const override
    {
        return Type;
    };

    void SetBehindWindowFilterRadius(float behindWindowFilterRadius);
    void SetBehindWindowFilterSaturation(float behindWindowFilterSaturation);
    void SetBehindWindowFilterBrightness(float behindWindowFilterBrightness);
    void SetBehindWindowFilterMaskColor(const Color behindWindowFilterMaskColor);

    float GetBehindWindowFilterRadius() const;
    float GetBehindWindowFilterSaturation() const;
    float GetBehindWindowFilterBrightness() const;
    Color GetBehindWindowFilterMaskColor() const;
};

class RSC_EXPORT RSUseEffectModifier : public RSModifier {
public:
    RSUseEffectModifier() = default;
    ~RSUseEffectModifier() override = default;

    static inline constexpr auto Type = ModifierNG::RSModifierType::USE_EFFECT;
    ModifierNG::RSModifierType GetType() const override
    {
        return Type;
    };

    void SetUseEffect(bool useEffect);
    void SetUseEffectType(UseEffectType useEffectType);

    bool GetUseEffect() const;
    UseEffectType GetUseEffectType() const;

protected:
};

class RSC_EXPORT RSDynamicLightUpModifier : public RSModifier {
public:
    RSDynamicLightUpModifier() = default;
    ~RSDynamicLightUpModifier() override = default;

    static inline constexpr auto Type = ModifierNG::RSModifierType::DYNAMIC_LIGHT_UP;
    ModifierNG::RSModifierType GetType() const override
    {
        return Type;
    };

    void SetDynamicLightUpRate(const std::optional<float>& rate);
    void SetDynamicLightUpDegree(const std::optional<float>& lightUpDegree);

    std::optional<float> GetDynamicLightUpRate() const;
    std::optional<float> GetDynamicLightUpDegree() const;

protected:
};

class RSC_EXPORT RSBackgroundStyleModifier : public RSExtendedModifier {
public:
    RSBackgroundStyleModifier() = default;
    ~RSBackgroundStyleModifier() override = default;

    static inline constexpr auto Type = ModifierNG::RSModifierType::BACKGROUND_STYLE;
    ModifierNG::RSModifierType GetType() const override
    {
        return Type;
    };

    void Draw(RSDrawingContext& context) const override
    {
        return;
    }

protected:
};
} // namespace OHOS::Rosen::ModifierNG
#endif // RENDER_SERVICE_CLIENT_CORE_MODIFIER_NG_RS_BACKGROUND_MODIFIER_H
