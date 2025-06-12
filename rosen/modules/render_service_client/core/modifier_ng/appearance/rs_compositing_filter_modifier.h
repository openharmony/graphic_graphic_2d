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

#ifndef RENDER_SERVICE_CLIENT_CORE_MODIFIER_NG_APPEARANCE_RS_COMPOSITING_FILTER_MODIFIER_H
#define RENDER_SERVICE_CLIENT_CORE_MODIFIER_NG_APPEARANCE_RS_COMPOSITING_FILTER_MODIFIER_H

#include "modifier_ng/rs_modifier_ng.h"

namespace OHOS::Rosen::ModifierNG {
class RSC_EXPORT RSCompositingFilterModifier : public RSModifier {
public:
    RSCompositingFilterModifier() = default;
    ~RSCompositingFilterModifier() override = default;

    static inline constexpr auto Type = RSModifierType::COMPOSITING_FILTER;
    RSModifierType GetType() const override
    {
        return Type;
    };

    void SetAiInvert(const std::optional<Vector4f>& aiInvert);
    std::optional<Vector4f> GetAiInvert() const;

    void SetGrayScale(std::optional<float> grayScale);
    std::optional<float> GetGrayScale() const;
    void SetBrightness(std::optional<float> brightness);
    std::optional<float> GetBrightness() const;
    void SetContrast(std::optional<float> contrast);
    std::optional<float> GetContrast() const;
    void SetSaturate(std::optional<float> saturate);
    std::optional<float> GetSaturate() const;
    void SetSepia(std::optional<float> sepia);
    std::optional<float> GetSepia() const;
    void SetInvert(std::optional<float> invert);
    std::optional<float> GetInvert() const;
    void SetHueRotate(std::optional<float> hueRotate);
    std::optional<float> GetHueRotate() const;
    void SetColorBlend(const std::optional<Color> colorBlend);
    std::optional<Color> GetColorBlend() const;

    void SetLightUpEffectDegree(float lightUpEffectDegree);
    float GetLightUpEffectDegree() const;

    void SetDynamicDimDegree(std::optional<float> dimDegree);
    std::optional<float> GetDynamicDimDegree() const;

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
    void SetFgBlurDisableSystemAdaptation(bool disableSystemAdaptation);
    bool GetFgBlurDisableSystemAdaptation() const;
};
} // namespace OHOS::Rosen::ModifierNG
#endif // RENDER_SERVICE_CLIENT_CORE_MODIFIER_NG_APPEARANCE_RS_COMPOSITING_FILTER_MODIFIER_H
