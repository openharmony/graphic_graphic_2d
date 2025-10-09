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

    void SetRipplePosition(const Vector3f& position)
    {
        ripplePosition_ = position;
    }

    const Vector3f& GetRipplePosition() const
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

private:
    std::shared_ptr<MaskPara> maskPara_ = nullptr;
    float rippleProgress_ = 0.0f;
    float distortProgress_ = 0.0f;
    Vector3f ripplePosition_;
    Vector4f tintColor_;
    float materialFactor_ = 0.0f;
    float distortFactor_ = 0.0f;
    float reflectionFactor_ = 0.0f;
    float refractionFactor_ = 0.0f;
    float cornerRadius_ = 0.0f;
};
} // namespace Rosen
} // namespace OHOS
#endif // UIEFFECT_EFFECT_HARMONIUM_EFFECT_PARA_H
