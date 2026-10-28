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

#ifndef RENDER_SERVICE_CLIENT_CORE_MODIFIER_NG_BACKGROUND_RS_BACKGROUND_COLOR_MODIFIER_H
#define RENDER_SERVICE_CLIENT_CORE_MODIFIER_NG_BACKGROUND_RS_BACKGROUND_COLOR_MODIFIER_H

#include "modifier_ng/rs_modifier_ng.h"

namespace OHOS::Rosen::ModifierNG {
class RSC_EXPORT RSBackgroundColorModifier : public RSModifier {
public:
    inline static const Color DEFAULT_BACKGROUND_COLOR{RgbPalette::Transparent()};
    inline static const Vector4f DEFAULT_BG_BRIGHTNESS_RATES{0.f};
    inline static const float DEFAULT_BG_BRIGHTNESS_SATURATION{0.f};
    inline static const Vector4f DEFAULT_BG_BRIGHTNESS_POSCOEFF{0.f};
    inline static const Vector4f DEFAULT_BG_BRIGHTNESS_NEGCOEFF{0.f};
    inline static const float DEFAULT_BG_BRIGHTNESS_FRACTION{1.f};

    RSBackgroundColorModifier()
    {
        affectsRendering_ = false;
    }

    ~RSBackgroundColorModifier() override = default;

    static inline constexpr auto Type = RSModifierType::BACKGROUND_COLOR;
    RSModifierType GetType() const override
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
    bool IsDefault(RSPropertyType type, const std::shared_ptr<RSPropertyBase>& property) const override
    {
        switch (type) {
            case RSPropertyType::BACKGROUND_COLOR:
                return GetterWithoutCheck<Color>(property).GetAlpha() == DEFAULT_BACKGROUND_COLOR.GetAlpha();
            case RSPropertyType::BG_BRIGHTNESS_RATES:
                return GetterWithoutCheck<Vector4f>(property) == DEFAULT_BG_BRIGHTNESS_RATES;
            case RSPropertyType::BG_BRIGHTNESS_SATURATION:
                return GetterWithoutCheck<float>(property) == DEFAULT_BG_BRIGHTNESS_SATURATION;
            case RSPropertyType::BG_BRIGHTNESS_POSCOEFF:
                return GetterWithoutCheck<Vector4f>(property) == DEFAULT_BG_BRIGHTNESS_POSCOEFF;
            case RSPropertyType::BG_BRIGHTNESS_NEGCOEFF:
                return GetterWithoutCheck<Vector4f>(property) == DEFAULT_BG_BRIGHTNESS_NEGCOEFF;
            case RSPropertyType::BG_BRIGHTNESS_FRACTION:
                return GetterWithoutCheck<float>(property) == DEFAULT_BG_BRIGHTNESS_FRACTION;
            default:
                return false;
        }
    }
};
} // namespace OHOS::Rosen::ModifierNG
#endif // RENDER_SERVICE_CLIENT_CORE_MODIFIER_NG_BACKGROUND_RS_BACKGROUND_COLOR_MODIFIER_H
