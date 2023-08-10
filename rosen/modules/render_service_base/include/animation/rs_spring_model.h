/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#ifndef ROSEN_ENGINE_CORE_ANIMATION_RS_SPRING_MODEL_H
#define ROSEN_ENGINE_CORE_ANIMATION_RS_SPRING_MODEL_H

#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {
// RSAnimatableType should have following operators: + - *float ==
template<typename RSAnimatableType>
class RSB_EXPORT RSSpringModel {
public:
    explicit RSSpringModel(float response, float dampingRatio, const RSAnimatableType& initialOffset,
        const RSAnimatableType& initialVelocity, float minimumAmplitude);

    virtual ~RSSpringModel() = default;

    RSAnimatableType CalculateDisplacement(double time) const;
    float EstimateDuration() const;

protected:
    RSSpringModel() = default;
    void CalculateSpringParameters();

    // physical parameters of spring-damper model
    float response_ { 0.0f };
    float dampingRatio_ { 0.0f };
    RSAnimatableType initialOffset_;
    RSAnimatableType initialVelocity_;

    // estimated duration until the spring is at rest
    float minimumAmplitudeRatio_ { 0.001f };

private:
    float EstimateDurationForUnderDampedModel() const
    {
        return 0.0f;
    }
    float EstimateDurationForCriticalDampedModel() const
    {
        return 0.0f;
    }
    float EstimateDurationForOverDampedModel() const
    {
        return 0.0f;
    }
    float BinarySearchTime(float left, float right, RSAnimatableType target) const
    {
        return 0.0f;
    }
    // calculated intermediate coefficient
    float coeffDecay_ { 0.0f };
    RSAnimatableType coeffScale_ {};
    float dampedAngularVelocity_ { 0.0f };
    RSAnimatableType coeffScaleAlt_ {};
    float coeffDecayAlt_ { 0.0f };
};

// only used in interpolatingSpring animation
template<>
RSB_EXPORT float RSSpringModel<float>::EstimateDuration() const;
template<>
RSB_EXPORT float RSSpringModel<float>::BinarySearchTime(float left, float right, float target) const;
template<>
RSB_EXPORT float RSSpringModel<float>::EstimateDurationForUnderDampedModel() const;
template<>
RSB_EXPORT float RSSpringModel<float>::EstimateDurationForCriticalDampedModel() const;
template<>
RSB_EXPORT float RSSpringModel<float>::EstimateDurationForOverDampedModel() const;
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_ENGINE_CORE_ANIMATION_RS_SPRING_MODEL_H
