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

#ifndef UIEFFECT_WAVE_GRADIENT_MASK_PARA_H
#define UIEFFECT_WAVE_GRADIENT_MASK_PARA_H
#include "ui_effect/mask/include/mask_para.h"
#include "common/rs_vector2.h"
#include "ui_effect/utils.h"
 
namespace OHOS {
namespace Rosen {
// limits for wave gradient mask center parameters
constexpr std::pair<float, float> WAVE_GRADIENT_MASK_WAVE_CENTER_LIMITS { -10.0f, 10.0f };
// limits for wave gradient mask radius parameters
constexpr std::pair<float, float> WAVE_GRADIENT_MASK_BLUR_RADIUS_LIMITS { 0.0f, 5.0f };
// limits for wave gradient mask width parameters
constexpr std::pair<float, float> WAVE_GRADIENT_MASK_WAVE_WIDTH_LIMITS { 0.f, 5.0f };
// limits for wave gradient mask propagation radius parameters
constexpr std::pair<float, float> WAVE_GRADIENT_MASK_PROPAGATION_RADIUS_LIMITS { 0.0f, 10.0f };
// limits for wave gradient mask turbulence strength parameters
constexpr std::pair<float, float> WAVE_GRADIENT_MASK_TURBULENCE_STRENGTH_LIMITS { -1.0f, 1.0f };
 
class WaveGradientMaskPara : public MaskPara {
public:
    WaveGradientMaskPara()
    {
        type_ = MaskPara::Type::WAVE_GRADIENT_MASK;
    }
    ~WaveGradientMaskPara() override = default;
 
    void SetWaveCenter(Vector2f& waveCenter)
    {
        waveCenter_ = UIEffect::GetLimitedPara(waveCenter, WAVE_GRADIENT_MASK_WAVE_CENTER_LIMITS);
    }
 
    const Vector2f& GetWaveCenter() const
    {
        return waveCenter_;
    }
 
    void SetBlurRadius(float blurRadius)
    {
        blurRadius_ = UIEffect::GetLimitedPara(blurRadius, WAVE_GRADIENT_MASK_BLUR_RADIUS_LIMITS);
    }
 
    const float& GetBlurRadius() const
    {
        return blurRadius_;
    }
 
    void SetWaveWidth(float waveWidth)
    {
        waveWidth_ = UIEffect::GetLimitedPara(waveWidth, WAVE_GRADIENT_MASK_WAVE_WIDTH_LIMITS);
    }
 
    const float& GetWaveWidth() const
    {
        return waveWidth_;
    }
 
    void SetPropagationRadius(float propagationRadius)
    {
        propagationRadius_ = UIEffect::GetLimitedPara(propagationRadius, WAVE_GRADIENT_MASK_PROPAGATION_RADIUS_LIMITS);
    }
 
    const float& GetPropagationRadius() const
    {
        return propagationRadius_;
    }
 
    void SetTurbulenceStrength(float turbulenceStrength)
    {
        turbulenceStrength_ =
            UIEffect::GetLimitedPara(turbulenceStrength, WAVE_GRADIENT_MASK_TURBULENCE_STRENGTH_LIMITS);
    }
 
    const float& GetTurbulenceStrength() const
    {
        return turbulenceStrength_;
    }
 
private:
    Vector2f waveCenter_ = { 0.0f, 0.0f };
    float waveWidth_ = 0.0f;
    float propagationRadius_ = 0.0f;
    float blurRadius_ = 0.0f;
    float turbulenceStrength_ = 0.0f;
};
} // namespace Rosen
} // namespace OHOS
#endif // UIEFFECT_WAVE_GRADIENT_MASK_PARA_H