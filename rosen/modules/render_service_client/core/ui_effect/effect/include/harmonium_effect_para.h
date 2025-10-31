/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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
#ifndef UIEFFECT_EFFECT_HARMONIUM_EFFECT_PARA_H
#define UIEFFECT_EFFECT_HARMONIUM_EFFECT_PARA_H
#include <iostream>

#include "visual_effect_para.h"
#include "ui_effect/mask/include/mask_para.h"
#include "ui_effect/utils.h"

namespace OHOS {
namespace Rosen {
class HarmoniumEffectPara : public VisualEffectPara {
public:
    HarmoniumEffectPara()
    {
        this->type_ = VisualEffectPara::ParaType::HARMONIUM_EFFECT;
    }
    ~HarmoniumEffectPara() override = default;

    void SetMask(std::shared_ptr<MaskPara> mask)
    {
        maskPara_ = mask;
    }

    const std::shared_ptr<MaskPara>& GetMask() const
    {
        return maskPara_;
    }

    void SetUseEffectMask(std::shared_ptr<MaskPara> mask)
    {
        useEffectMaskPara_ = mask;
    }

    const std::shared_ptr<MaskPara>& GetUseEffectMask() const
    {
        return useEffectMaskPara_;
    }

    void SetRipplePosition(const std::vector<Vector2f>& position)
    {
        ripplePosition_ = position;
    }

    const std::vector<Vector2f>& GetRipplePosition() const
    {
        return ripplePosition_;
    }

    void SetRippleProgress(float progress)
    {
        rippleProgress_ = progress;
    }

    const float& GetRippleProgress() const
    {
        return rippleProgress_;
    }

    void SetTintColor(const Vector4f& color)
    {
        tintColor_ = color;
    }

    const Vector4f& GetTintColor() const
    {
        return tintColor_;
    }

    void SetDistortProgress(float progress)
    {
        distortProgress_ = progress;
    }

    const float& GetDistortProgress() const
    {
        return distortProgress_;
    }

    void SetMaterialFactor(float factor)
    {
        materialFactor_ = factor;
    }

    const float& GetMaterialFactor() const
    {
        return materialFactor_;
    }

    void SetDistortFactor(float factor)
    {
        distortFactor_ = factor;
    }

    const float& GetDistortFactor() const
    {
        return distortFactor_;
    }

    void SetReflectionFactor(float factor)
    {
        reflectionFactor_ = factor;
    }

    const float& GetReflectionFactor() const
    {
        return reflectionFactor_;
    }

    void SetRefractionFactor(float factor)
    {
        refractionFactor_ = factor;
    }

    const float& GetRefractionFactor() const
    {
        return refractionFactor_;
    }

    void SetCornerRadius(float radius)
    {
        cornerRadius_ = radius;
    }

    const float& GetCornerRadius() const
    {
        return cornerRadius_;
    }

    void SetEnable(bool enable)
    {
        enable_ = enable;
    }

    const bool& GetEnable() const
    {
        return enable_;
    }

    void SetRate(float rate)
    {
        rate_ = rate;
    }

    const float& GetRate() const
    {
        return rate_;
    }

    void SetLightUpDegree(float lightUpDegree)
    {
        lightUpDegree_ = lightUpDegree;
    }

    const float& GetLightUpDegree() const
    {
        return lightUpDegree_;
    }

    void SetCubicCoeff(float cubicCoeff)
    {
        cubicCoeff_ = cubicCoeff;
    }

    const float& GetCubicCoeff() const
    {
        return cubicCoeff_;
    }

    void SetQuadCoeff(float quadCoeff)
    {
        quadCoeff_ = quadCoeff;
    }

    const float& GetQuadCoeff() const
    {
        return quadCoeff_;
    }

    void SetSaturation(float saturation)
    {
        saturation_ = saturation;
    }

    const float& GetSaturation() const
    {
        return saturation_;
    }

    void SetPosRGB(const Vector3f& posRGB)
    {
        posRGB_ = posRGB;
    }

    const Vector3f& GetPosRGB() const
    {
        return posRGB_;
    }

    void SetNegRGB(const Vector3f& negRGB)
    {
        negRGB_ = negRGB;
    }

    const Vector3f& GetNegRGB() const
    {
        return negRGB_;
    }

    void SetFraction(float fraction)
    {
        fraction_ = fraction;
    }

    const float& GetFraction() const
    {
        return fraction_;
    }

private:
    std::shared_ptr<MaskPara> maskPara_ = nullptr;
    std::shared_ptr<MaskPara> useEffectMaskPara_ = nullptr;
    float rippleProgress_ = 0.0f;
    float distortProgress_ = 0.0f;
    std::vector<Vector2f> ripplePosition_;
    Vector4f tintColor_;
    float materialFactor_ = 0.0f;
    float distortFactor_ = 0.0f;
    float reflectionFactor_ = 0.0f;
    float refractionFactor_ = 0.0f;
    float cornerRadius_ = 0.0f;
    bool enable_ = false;
    float rate_ = 0.0f;
    float lightUpDegree_ = 0.0f;
    float cubicCoeff_ = 0.0f;
    float quadCoeff_ = 0.0f;
    float saturation_ = 0.0f;
    Vector3f posRGB_;
    Vector3f negRGB_;
    float fraction_ = 1.0f; // 1 means default value
};
} // namespace Rosen
} // namespace OHOS
#endif // UIEFFECT_EFFECT_HARMONIUM_EFFECT_PARA_H
