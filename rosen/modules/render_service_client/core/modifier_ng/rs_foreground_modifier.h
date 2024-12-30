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

#ifndef RENDER_SERVICE_CLIENT_CORE_MODIFIER_NG_RS_FOREGROUND_MODIFIER_H
#define RENDER_SERVICE_CLIENT_CORE_MODIFIER_NG_RS_FOREGROUND_MODIFIER_H

#include "modifier_ng/rs_modifier_ng.h"

namespace OHOS::Rosen::ModifierNG {
class RSC_EXPORT RSCompositingFilterModifier : public RSModifier {
public:
    RSCompositingFilterModifier() = default;
    ~RSCompositingFilterModifier() override = default;

    static inline constexpr auto Type = ModifierNG::RSModifierType::COMPOSITING_FILTER;
    ModifierNG::RSModifierType GetType() const override
    {
        return Type;
    };

    void SetAiInvert(const Vector4f& aiInvert);
    Vector4f GetAiInvert() const;

    void SetGrayScale(float grayScale);
    float GetGrayScale() const;
    void SetBrightness(float brightness);
    float GetBrightness() const;
    void SetContrast(float contrast);
    float GetContrast() const;
    void SetSaturate(float saturate);
    float GetSaturate() const;
    void SetSepia(float sepia);
    float GetSepia() const;
    void SetInvert(float invert);
    float GetInvert() const;
    void SetHueRotate(float hueRotate);
    float GetHueRotate() const;
    void SetColorBlend(const Color colorBlend);
    Color GetColorBlend() const;

    void SetLightUpEffectDegree(float lightUpEffectDegree);
    float GetLightUpEffectDegree() const;

    void SetDynamicDimDegree(float dimDegree);
    float GetDynamicDimDegree() const;

    void SetFilter(const std::shared_ptr<RSFilter>& filter);
    std::shared_ptr<RSFilter> GetFilter() const;
    void SetLinearGradientBlurPara(const std::shared_ptr<RSLinearGradientBlurPara>& para);
    std::shared_ptr<RSLinearGradientBlurPara> GetLinearGradientBlurPara() const;
    void SetForegroundBlurRadius(float radius);
    float GetForegroundBlurRadius() const;
    void SetForegroundBlurSaturation(float saturation);
    float GetForegroundBlurSaturation() const;
    void SetForegroundBlurBrightness(float brightness);
    float GetForegroundBlurBrightness() const;
    void SetForegroundBlurMaskColor(const Color maskColor);
    Color GetForegroundBlurMaskColor() const;
    void SetForegroundBlurColorMode(int colorMode);
    int GetForegroundBlurColorMode() const;
    void SetForegroundBlurRadiusX(float blurRadiusX);
    float GetForegroundBlurRadiusX() const;
    void SetForegroundBlurRadiusY(float blurRadiusY);
    float GetForegroundBlurRadiusY() const;
};

class RSC_EXPORT RSForegroundColorModifier : public RSModifier {
public:
    RSForegroundColorModifier() = default;
    ~RSForegroundColorModifier() override = default;

    static inline constexpr auto Type = ModifierNG::RSModifierType::FOREGROUND_COLOR;
    ModifierNG::RSModifierType GetType() const override
    {
        return Type;
    };

    void SetForegroundColor(const Color color);
    Color GetForegroundColor() const;
};
} // namespace OHOS::Rosen::ModifierNG
#endif // RENDER_SERVICE_CLIENT_CORE_MODIFIER_NG_RS_FOREGROUND_MODIFIER_H
