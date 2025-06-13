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

#ifndef RENDER_SERVICE_CLIENT_CORE_MODIFIER_NG_APPEARANCE_RS_BLEND_MODIFIER_H
#define RENDER_SERVICE_CLIENT_CORE_MODIFIER_NG_APPEARANCE_RS_BLEND_MODIFIER_H

#include "modifier_ng/rs_modifier_ng.h"

namespace OHOS::Rosen::ModifierNG {
class RSC_EXPORT RSBlendModifier : public RSModifier {
public:
    RSBlendModifier() = default;
    ~RSBlendModifier() override = default;

    static inline constexpr auto Type = RSModifierType::BLENDER;
    RSModifierType GetType() const override
    {
        return Type;
    };

    void SetColorBlendMode(RSColorBlendMode colorBlendMode);
    void SetColorBlendApplyType(RSColorBlendApplyType colorBlendApplyType);
    void SetFgBrightnessRates(const Vector4f& rates);
    void SetFgBrightnessSaturation(float saturation);
    void SetFgBrightnessPosCoeff(const Vector4f& coeff);
    void SetFgBrightnessNegCoeff(const Vector4f& coeff);
    void SetFgBrightnessFract(float fract);

    RSColorBlendMode GetColorBlendMode() const;
    RSColorBlendApplyType GetColorBlendApplyType() const;
    Vector4f GetFgBrightnessRates() const;
    float GetFgBrightnessSaturation() const;
    Vector4f GetFgBrightnessPosCoeff() const;
    Vector4f GetFgBrightnessNegCoeff() const;
    float GetFgBrightnessFract() const;
};
} // namespace OHOS::Rosen::ModifierNG
#endif // RENDER_SERVICE_CLIENT_CORE_MODIFIER_NG_APPEARANCE_RS_BLEND_MODIFIER_H
