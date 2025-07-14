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
#ifndef UIEFFECT_EFFECT_SHADOW_BLENDER_H
#define UIEFFECT_EFFECT_SHADOW_BLENDER_H

#include "blender.h"
#include "ui_effect/utils.h"

namespace OHOS {
namespace Rosen {
constexpr std::pair<float, float> SHADOW_BLENDER_LIMITS {-100.f, 100.f};

class ShadowBlender : public Blender {
public:
    ShadowBlender()
    {
        this->blenderType_ = Blender::SHADOW_BLENDER;
    }
    ~ShadowBlender() override = default;

    void SetCubicCoeff(float cubic)
    {
        cubic_ = UIEffect::GetLimitedPara(cubic, SHADOW_BLENDER_LIMITS);
    }

    float GetCubicCoeff() const
    {
        return cubic_;
    }

    void SetQuadraticCoeff(float quadratic)
    {
        quadratic_ = UIEffect::GetLimitedPara(quadratic, SHADOW_BLENDER_LIMITS);
    }

    float GetQuadraticCoeff() const
    {
        return quadratic_;
    }

    void SetLinearCoeff(float linear)
    {
        linear_ = UIEffect::GetLimitedPara(linear, SHADOW_BLENDER_LIMITS);
    }

    float GetLinearCoeff() const
    {
        return linear_;
    }

    void SetConstantTerm(float constant)
    {
        constant_ = UIEffect::GetLimitedPara(constant, SHADOW_BLENDER_LIMITS);
    }

    float GetConstantTerm() const
    {
        return constant_;
    }

private:
    float cubic_ = 0.0f;
    float quadratic_ = 0.0f;
    float linear_ = 0.0f;
    float constant_ = 0.0f;
};
} // namespace Rosen
} // namespace OHOS
#endif // UIEFFECT_EFFECT_SHADOW_BLENDER_H
