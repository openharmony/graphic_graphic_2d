/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#ifndef UIEFFECT_EFFECT_BRIGHTNESS_BLENDER_H
#define UIEFFECT_EFFECT_BRIGHTNESS_BLENDER_H

#include "blender.h"
#include "common/rs_vector3.h"

namespace OHOS {
namespace Rosen {
class BrightnessBlender : public Blender {
public:
    BrightnessBlender()
    {
        this->blenderType_ = Blender::BRIGHTNESS_BLENDER;
    }
    ~BrightnessBlender() override = default;

    void SetCubicRate(float cubicRate)
    {
        cubicRate_ = cubicRate;
    }

    float GetCubicRate() const
    {
        return cubicRate_;
    }

    void SetQuadRate(float quadRate)
    {
        quadRate_ = quadRate;
    }

    float GetQuadRate() const
    {
        return quadRate_;
    }

    void SetLinearRate(float linearRate)
    {
        linearRate_ = linearRate;
    }

    float GetLinearRate() const
    {
        return linearRate_;
    }

    void SetDegree(float degree)
    {
        degree_ = degree;
    }

    float GetDegree() const
    {
        return degree_;
    }

    void SetSaturation(float saturation)
    {
        saturation_ = saturation;
    }

    float GetSaturation() const
    {
        return saturation_;
    }

    void SetPositiveCoeff(Vector3f positiveCoeff)
    {
        positiveCoeff_ = positiveCoeff;
    }

    const Vector3f& GetPositiveCoeff() const
    {
        return positiveCoeff_;
    }

    void SetNegativeCoeff(Vector3f negativeCoeff)
    {
        negativeCoeff_ = negativeCoeff;
    }

    const Vector3f& GetNegativeCoeff() const
    {
        return negativeCoeff_;
    }

    void SetFraction(float fraction)
    {
        fraction_ = fraction;
    }

    float GetFraction() const
    {
        return fraction_;
    }

private:
    float cubicRate_ = 0.0f;
    float quadRate_ = 0.0f;
    float linearRate_ = 1.0f;
    float degree_ = 0.0f;
    float saturation_ = 1.0f;
    Vector3f positiveCoeff_;
    Vector3f negativeCoeff_;
    float fraction_ = 1.0f;
};
} // namespace Rosen
} // namespace OHOS
#endif // UIEFFECT_EFFECT_BRIGHTNESS_BLENDER_H
