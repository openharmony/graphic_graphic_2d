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

#ifndef RENDER_SERVICE_CLIENT_CORE_MODIFIER_NG_APPEARANCE_RS_BACKGROUND_FILTER_MODIFIER_H
#define RENDER_SERVICE_CLIENT_CORE_MODIFIER_NG_APPEARANCE_RS_BACKGROUND_FILTER_MODIFIER_H

#include "modifier_ng/rs_modifier_ng.h"

namespace OHOS::Rosen::ModifierNG {
class RSC_EXPORT RSBackgroundFilterModifier : public RSModifier {
public:
    RSBackgroundFilterModifier() = default;
    ~RSBackgroundFilterModifier() override = default;

    static inline constexpr auto Type = RSModifierType::BACKGROUND_FILTER;
    RSModifierType GetType() const override
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
    void SetBgBlurDisableSystemAdaptation(bool disableSystemAdaptation);
    void SetGreyCoef(const std::optional<Vector2f>& greyCoef);
    void SetAlwaysSnapshot(bool enable);

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
    bool GetBgBlurDisableSystemAdaptation() const;
    std::optional<Vector2f> GetGreyCoef() const;
    bool GetAlwaysSnapshot() const;
};
} // namespace OHOS::Rosen::ModifierNG
#endif // RENDER_SERVICE_CLIENT_CORE_MODIFIER_NG_APPEARANCE_RS_BACKGROUND_FILTER_MODIFIER_H
