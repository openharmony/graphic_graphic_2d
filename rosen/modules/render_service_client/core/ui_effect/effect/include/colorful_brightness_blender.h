/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
#ifndef UIEFFECT_EFFECT_COLORFUL_BRIGHTNESS_BLENDER_H
#define UIEFFECT_EFFECT_COLORFUL_BRIGHTNESS_BLENDER_H

#include "blender.h"
#include "brightness_blender.h"
#include "ui_effect/utils.h"
#include "common/rs_vector3.h"

namespace OHOS {
namespace Rosen {

class ColorfulBrightnessBlender : public Blender {
public:
    ColorfulBrightnessBlender()
    {
        this->blenderType_ = Blender::COLORFUL_BRIGHTNESS_BLENDER;
    }
    ~ColorfulBrightnessBlender() override = default;

    void SetDarkenWeight(float val) { darkenWeight_ = UIEffect::GetLimitedPara(val, {0.0f, 1.0f}); }
    float GetDarkenWeight() const { return darkenWeight_; }

    void SetFraction(float val) { fraction_ = UIEffect::GetLimitedPara(val, {0.0f, 1.0f}); }
    float GetFraction() const { return fraction_; }

    void SetCubicRate(float val) { cubicRate_ = UIEffect::GetLimitedPara(val, BRIGHTNESS_BLENDER_LIMITS); }
    float GetCubicRate() const { return cubicRate_; }

    void SetQuadRate(float val) { quadRate_ = UIEffect::GetLimitedPara(val, BRIGHTNESS_BLENDER_LIMITS); }
    float GetQuadRate() const { return quadRate_; }

    void SetLinearRate(float val) { linearRate_ = UIEffect::GetLimitedPara(val, BRIGHTNESS_BLENDER_LIMITS); }
    float GetLinearRate() const { return linearRate_; }

    void SetDegree(float val) { degree_ = UIEffect::GetLimitedPara(val, BRIGHTNESS_BLENDER_LIMITS); }
    float GetDegree() const { return degree_; }

    void SetSaturation(float val)
    {
        saturation_ = UIEffect::GetLimitedPara(val, {0.0f, BRIGHTNESS_BLENDER_LIMITS.second});
    }
    float GetSaturation() const { return saturation_; }

    void SetPositiveCoeff(const Vector3f& val)
    {
        positiveCoeff_ = UIEffect::GetLimitedPara(val, BRIGHTNESS_BLENDER_LIMITS);
    }
    const Vector3f& GetPositiveCoeff() const { return positiveCoeff_; }

    void SetNegativeCoeff(const Vector3f& val)
    {
        negativeCoeff_ = UIEffect::GetLimitedPara(val, BRIGHTNESS_BLENDER_LIMITS);
    }
    const Vector3f& GetNegativeCoeff() const { return negativeCoeff_; }

    void SetVibrancyStrength(float val)
    {
        vibrancyStrength_ = UIEffect::GetLimitedPara(val, {0.0f, 1.0f});
    }
    float GetVibrancyStrength() const { return vibrancyStrength_; }

    void SetLumaDiff(float val) { lumaDiff_ = UIEffect::GetLimitedPara(val, {0.0f, 1.0f}); }
    float GetLumaDiff() const { return lumaDiff_; }

    void SetHdrEnabled(bool val) { hdrEnabled_ = val; }
    bool GetHdrEnabled() const { return hdrEnabled_; }

private:
    float fraction_ = 1.0f;
    float cubicRate_ = 0.0f;
    float quadRate_ = 0.0f;
    float linearRate_ = 1.0f;
    float degree_ = 0.0f;
    float saturation_ = 1.0f;
    Vector3f positiveCoeff_;
    Vector3f negativeCoeff_;
    float darkenWeight_ = 1.0f;
    float vibrancyStrength_ = 0.0f;
    float lumaDiff_ = 0.0f;
    bool hdrEnabled_ = true;
};

} // namespace Rosen
} // namespace OHOS
#endif // UIEFFECT_EFFECT_COLORFUL_BRIGHTNESS_BLENDER_H
