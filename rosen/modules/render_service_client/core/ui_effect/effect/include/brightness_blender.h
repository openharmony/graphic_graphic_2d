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
#include "ui_effect/utils.h"

namespace OHOS {
namespace Rosen {
constexpr std::pair<float, float> BRIGHTNESS_BLENDER_LIMITS {-20.f, 20.f}; // limits for brightness blender pamameters

class BrightnessBlender : public Blender {
public:
    BrightnessBlender()
    {
        this->blenderType_ = Blender::BRIGHTNESS_BLENDER;
    }
    ~BrightnessBlender() override = default;

    void SetCubicRate(float cubicRate)
    {
        cubicRate_ = UIEffect::GetLimitedPara(cubicRate, BRIGHTNESS_BLENDER_LIMITS);
    }

    float GetCubicRate() const
    {
        return cubicRate_;
    }

    void SetQuadRate(float quadRate)
    {
        quadRate_ = UIEffect::GetLimitedPara(quadRate, BRIGHTNESS_BLENDER_LIMITS);
    }

    float GetQuadRate() const
    {
        return quadRate_;
    }

    void SetLinearRate(float linearRate)
    {
        linearRate_ = UIEffect::GetLimitedPara(linearRate, BRIGHTNESS_BLENDER_LIMITS);
    }

    float GetLinearRate() const
    {
        return linearRate_;
    }

    void SetDegree(float degree)
    {
        degree_ = UIEffect::GetLimitedPara(degree, BRIGHTNESS_BLENDER_LIMITS);
    }

    float GetDegree() const
    {
        return degree_;
    }

    void SetSaturation(float saturation)
    {
        saturation_ = UIEffect::GetLimitedPara(saturation, {0.0f, BRIGHTNESS_BLENDER_LIMITS.second});
    }

    float GetSaturation() const
    {
        return saturation_;
    }

    void SetPositiveCoeff(const Vector3f&  positiveCoeff)
    {
        positiveCoeff_ = UIEffect::GetLimitedPara(positiveCoeff, BRIGHTNESS_BLENDER_LIMITS);
    }

    const Vector3f& GetPositiveCoeff() const
    {
        return positiveCoeff_;
    }

    void SetNegativeCoeff(const Vector3f& negativeCoeff)
    {
        negativeCoeff_ = UIEffect::GetLimitedPara(negativeCoeff, BRIGHTNESS_BLENDER_LIMITS);
    }

    const Vector3f& GetNegativeCoeff() const
    {
        return negativeCoeff_;
    }

    void SetFraction(float fraction)
    {
        fraction_ = UIEffect::GetLimitedPara(fraction, {0.f, 1.f});
    }

    float GetFraction() const
    {
        return fraction_;
    }

    void SetHdr(bool enableHdr)
    {
        enableHdr_ = enableHdr;
    }

    bool GetHdr() const
    {
        return enableHdr_;
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
    bool enableHdr_ = false;
};
} // namespace Rosen
} // namespace OHOS
#endif // UIEFFECT_EFFECT_BRIGHTNESS_BLENDER_H
