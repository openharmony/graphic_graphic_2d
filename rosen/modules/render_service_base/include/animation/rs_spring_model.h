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
#include "common/rs_vector2.h"
#include "common/rs_vector4.h"

namespace OHOS {
namespace Rosen {
class RSRenderPropertyBase;
namespace {
constexpr float SPRING_MIN_DAMPING_RATIO = 1e-4f;
constexpr float SPRING_MAX_DAMPING_RATIO = 1e4f;
constexpr float SPRING_MIN_DURATION = 1e-3f;
constexpr float SPRING_MAX_DURATION = 300.0f;
constexpr float SPRING_MIN_RESPONSE = 1e-8;
constexpr float SPRING_MIN_AMPLITUDE_RATIO = 1e-3f;
constexpr float SPRING_MIN_AMPLITUDE = 1e-5f;
constexpr float FLOAT_PI = 3.14159265f;

// helper function to simplify estimation of spring duration
template<typename RSAnimatableType>
float toFloat(RSAnimatableType value)
{
    return 1.f;
}
template<>
float toFloat(Vector4f value)
{
    return value.GetLength();
}
template<>
float toFloat(Quaternion value)
{
    return value.GetLength();
}
template<>
float toFloat(Vector2f value)
{
    return value.GetLength();
}
} // namespace

// RSAnimatableType should have following operators: + - *float ==
template<typename RSAnimatableType>
class RSB_EXPORT RSSpringModel {
public:
    RSSpringModel() {};

    explicit RSSpringModel(float response, float dampingRatio, const RSAnimatableType& initialOffset,
        const RSAnimatableType& initialVelocity, float minimumAmplitude)
        : response_(response), dampingRatio_(dampingRatio), initialOffset_(initialOffset),
          initialVelocity_(initialVelocity), minimumAmplitudeRatio_(minimumAmplitude)
    {
        CalculateSpringParameters();
    }

    ~RSSpringModel() {};

    RSAnimatableType CalculateDisplacement(double time) const
    {
        if (dampingRatio_ <= 0.0f) {
            return {};
        }
        double coeffDecay = exp(coeffDecay_ * time);
        if (dampingRatio_ < 1) {
            // under-damped
            double rad = dampedAngularVelocity_ * time;
            RSAnimatableType coeffPeriod = initialOffset_ * cos(rad) + coeffScale_ * sin(rad);
            return coeffPeriod * coeffDecay;
        } else if (dampingRatio_ == 1) {
            // critical-damped
            return (initialOffset_ + coeffScale_ * time) * coeffDecay;
        } else {
            // over-damped
            double coeffDecayAlt = exp(coeffDecayAlt_ * time);
            return coeffScale_ * coeffDecay + coeffScaleAlt_ * coeffDecayAlt;
        }
    }

    float EstimateDuration() const
    {
        if (dampingRatio_ <= 0.0f) {
            return 0.0f;
        }

        // convert templated type to float, simplify estimation of spring duration
        float coeffScale = toFloat(coeffScale_);
        float initialOffset = toFloat(initialOffset_);
        float estimatedDuration = 0.0f;
        float minimumAmplitude = std::max(initialOffset * minimumAmplitudeRatio_, SPRING_MIN_AMPLITUDE);

        if (dampingRatio_ < 1) { // Under-damped
            if (ROSEN_EQ(coeffDecay_, 0.0f)) {
                return 0.0f;
            }
            estimatedDuration = log(fmax(coeffScale, initialOffset) / minimumAmplitude) / -coeffDecay_;
        } else if (dampingRatio_ == 1) { // Critically-damped
            // critical damping spring will rest at 2 * natural period
            estimatedDuration = response_ * 2;
        } else { // Over-damped
            if (ROSEN_EQ(coeffDecay_, 0.0f) || ROSEN_EQ(coeffDecayAlt_, 0.0f)) {
                return 0.0f;
            }
            float coeffScaleAlt = toFloat(coeffScaleAlt_);
            double durationMain =
                (coeffScale <= minimumAmplitude) ? 0 : (log(coeffScale / minimumAmplitude) / -coeffDecay_);
            double durationAlt =
                (coeffScaleAlt <= minimumAmplitude) ? 0 : (log(coeffScaleAlt / minimumAmplitude) / -coeffDecayAlt_);
            estimatedDuration = fmax(durationMain, durationAlt);
        }
        return std::clamp(estimatedDuration, SPRING_MIN_DURATION, SPRING_MAX_DURATION);
    }

protected:
    void CalculateSpringParameters()
    {
        // sanity check
        dampingRatio_ = std::clamp(dampingRatio_, SPRING_MIN_DAMPING_RATIO, SPRING_MAX_DAMPING_RATIO);
        if (response_ <= 0) {
            response_ = SPRING_MIN_RESPONSE;
        }
        if (minimumAmplitudeRatio_ <= 0) {
            minimumAmplitudeRatio_ = SPRING_MIN_AMPLITUDE_RATIO;
        }

        // calculate internal parameters
        double naturalAngularVelocity = 2 * FLOAT_PI / response_;
        if (dampingRatio_ < 1) { // Under-damped Systems
            dampedAngularVelocity_ = naturalAngularVelocity * sqrt(1.0f - dampingRatio_ * dampingRatio_);
            if (ROSEN_EQ(dampedAngularVelocity_, 0.0f)) {
                return;
            }
            coeffDecay_ = -dampingRatio_ * naturalAngularVelocity;
            coeffScale_ = (initialVelocity_ + initialOffset_ * dampingRatio_ * naturalAngularVelocity) *
                          (1 / dampedAngularVelocity_);
        } else if (dampingRatio_ == 1) { // Critically-Damped Systems
            coeffDecay_ = -naturalAngularVelocity;
            coeffScale_ = initialVelocity_ + initialOffset_ * naturalAngularVelocity;
        } else { // Over-damped Systems
            double coeffTmp = sqrt(dampingRatio_ * dampingRatio_ - 1);
            if (ROSEN_EQ(naturalAngularVelocity * coeffTmp, 0.0)) {
                return;
            }
            coeffDecay_ = (-dampingRatio_ + coeffTmp) * naturalAngularVelocity;
            coeffScale_ = (initialOffset_ * ((dampingRatio_ + coeffTmp) * naturalAngularVelocity) + initialVelocity_) *
                          (0.5f / (naturalAngularVelocity * coeffTmp));
            coeffScaleAlt_ =
                (initialOffset_ * ((coeffTmp - dampingRatio_) * naturalAngularVelocity) - initialVelocity_) *
                (0.5f / (naturalAngularVelocity * coeffTmp));
            coeffDecayAlt_ = (-dampingRatio_ - coeffTmp) * naturalAngularVelocity;
        }
    }

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

    template<typename T>
    friend class RSSpringValueEstimator;
};

template class RSSpringModel<Vector2f>;
template class RSSpringModel<Vector4f>;
template class RSSpringModel<Quaternion>;

// only used for property of which type is float
template<>
RSB_EXPORT float RSSpringModel<float>::EstimateDuration() const;
template<>
float RSSpringModel<std::shared_ptr<RSRenderPropertyBase>>::EstimateDuration() const;
template<>
RSB_EXPORT float RSSpringModel<float>::BinarySearchTime(float left, float right, float target) const;
template<>
RSB_EXPORT float RSSpringModel<float>::EstimateDurationForUnderDampedModel() const;
template<>
RSB_EXPORT float RSSpringModel<float>::EstimateDurationForCriticalDampedModel() const;
template<>
RSB_EXPORT float RSSpringModel<float>::EstimateDurationForOverDampedModel() const;

template<>
void RSSpringModel<std::shared_ptr<RSRenderPropertyBase>>::CalculateSpringParameters();
template<>
std::shared_ptr<RSRenderPropertyBase> RSSpringModel<std::shared_ptr<RSRenderPropertyBase>>::CalculateDisplacement(
    double time) const;
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_ENGINE_CORE_ANIMATION_RS_SPRING_MODEL_H
