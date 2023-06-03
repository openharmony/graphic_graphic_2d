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

#include "animation/rs_spring_model.h"

#include <algorithm>
#include <cmath>

#include "common/rs_vector2.h"
#include "common/rs_vector4.h"
#include "modifier/rs_render_property.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr float SPRING_MIN_DAMPING_RATIO = 1e-4f;
constexpr float SPRING_MAX_DAMPING_RATIO = 1e4f;
constexpr float SPRING_MIN_DURATION = 0.001f;
constexpr float SPRING_MAX_DURATION = 300.0f;
constexpr float SPRING_MIN_RESPONSE = 1e-8;
constexpr float SPRING_MIN_AMPLITUDE_RATIO = 1e-3f;
constexpr float SPRING_MIN_AMPLITUDE = 1e-5f;
// helper function to simplify estimation of spring duration
template<typename RSAnimatableType>
float toFloat(RSAnimatableType value)
{
    return 1.f;
}
template<>
float toFloat(float value)
{
    return std::fabs(value);
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

template<typename RSAnimatableType>
RSSpringModel<RSAnimatableType>::RSSpringModel(float response, float dampingRatio,
    const RSAnimatableType& initialOffset, const RSAnimatableType& initialVelocity, float minimumAmplitudeRatio)
    : response_(response), dampingRatio_(dampingRatio), initialOffset_(initialOffset),
      initialVelocity_(initialVelocity), minimumAmplitudeRatio_(minimumAmplitudeRatio)
{
    CalculateSpringParameters();
}

template<typename RSAnimatableType>
void RSSpringModel<RSAnimatableType>::CalculateSpringParameters()
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
    double naturalAngularVelocity = 2 * M_PI / response_;
    if (dampingRatio_ < 1) { // Under-damped Systems
        dampedAngularVelocity_ = naturalAngularVelocity * sqrt(1.0f - dampingRatio_ * dampingRatio_);
        coeffDecay_ = -dampingRatio_ * naturalAngularVelocity;
        coeffScale_ =
            (initialVelocity_ + initialOffset_ * dampingRatio_ * naturalAngularVelocity) * (1 / dampedAngularVelocity_);
    } else if (dampingRatio_ == 1) { // Critically-Damped Systems
        coeffDecay_ = -naturalAngularVelocity;
        coeffScale_ = initialVelocity_ + initialOffset_ * naturalAngularVelocity;
    } else { // Over-damped Systems
        double coeffTmp = sqrt(dampingRatio_ * dampingRatio_ - 1);
        coeffDecay_ = (-dampingRatio_ + coeffTmp) * naturalAngularVelocity;
        coeffScale_ = (initialOffset_ * ((dampingRatio_ + coeffTmp) * naturalAngularVelocity) + initialVelocity_) *
                      (0.5f / (naturalAngularVelocity * coeffTmp));
        coeffScaleAlt_ = (initialOffset_ * ((coeffTmp - dampingRatio_) * naturalAngularVelocity) - initialVelocity_) *
                         (0.5f / (naturalAngularVelocity * coeffTmp));
        coeffDecayAlt_ = (-dampingRatio_ - coeffTmp) * naturalAngularVelocity;
    }
}

template<typename RSAnimatableType>
float RSSpringModel<RSAnimatableType>::EstimateDuration() const
{
    if (dampingRatio_ <= 0.0f) {
        ROSEN_LOGE("RSSpringModel::%s, uninitialized spring model", __func__);
        return 0.0f;
    }

    // convert templated type to float, simplify estimation of spring duration
    float coeffScale = toFloat(coeffScale_);
    float initialOffset = toFloat(initialOffset_);
    float estimatedDuration = 0.0f;
    float minimumAmplitude = std::max(initialOffset * minimumAmplitudeRatio_, SPRING_MIN_AMPLITUDE);

    if (dampingRatio_ < 1) { // Under-damped
        estimatedDuration = log(fmax(coeffScale, initialOffset) / minimumAmplitude) / -coeffDecay_;
    } else if (dampingRatio_ == 1) { // Critically-damped
        // critical damping spring will rest at 2 * natural period
        estimatedDuration = response_ * 2;
    } else { // Over-damped
        float coeffScaleAlt = toFloat(coeffScaleAlt_);
        double durationMain =
            (coeffScale <= minimumAmplitude) ? 0 : (log(coeffScale / minimumAmplitude) / -coeffDecay_);
        double durationAlt =
            (coeffScaleAlt <= minimumAmplitude) ? 0 : (log(coeffScaleAlt / minimumAmplitude) / -coeffDecayAlt_);
        estimatedDuration = fmax(durationMain, durationAlt);
    }
    return std::clamp(estimatedDuration, SPRING_MIN_DURATION, SPRING_MAX_DURATION);
}

template<typename RSAnimatableType>
RSAnimatableType RSSpringModel<RSAnimatableType>::CalculateDisplacement(double time) const
{
    if (dampingRatio_ <= 0.0f) {
        ROSEN_LOGE("RSSpringModel::%s, uninitialized spring model", __func__);
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

template<>
void RSSpringModel<std::shared_ptr<RSRenderPropertyBase>>::CalculateSpringParameters()
{
    // sanity check
    dampingRatio_ = std::clamp(dampingRatio_, SPRING_MIN_DAMPING_RATIO, SPRING_MAX_DAMPING_RATIO);
    if (response_ <= 0) {
        response_ = SPRING_MIN_RESPONSE;
    }
    if (minimumAmplitudeRatio_ <= 0) {
        minimumAmplitudeRatio_ = SPRING_MIN_AMPLITUDE_RATIO;
    }
    if (initialOffset_ == nullptr) {
        return;
    }

    // calculate internal parameters
    double naturalAngularVelocity = 2 * M_PI / response_;
    if (dampingRatio_ < 1) { // Under-damped Systems
        dampedAngularVelocity_ = naturalAngularVelocity * sqrt(1.0f - dampingRatio_ * dampingRatio_);
        coeffDecay_ = -dampingRatio_ * naturalAngularVelocity;
        coeffScale_ = (initialVelocity_ + initialOffset_ * dampingRatio_ * naturalAngularVelocity) *=
            (1 / dampedAngularVelocity_);
    } else if (dampingRatio_ == 1) { // Critically-Damped Systems
        coeffDecay_ = -naturalAngularVelocity;
        coeffScale_ = initialVelocity_ + initialOffset_ * naturalAngularVelocity;
    } else { // Over-damped Systems
        double coeffTmp = sqrt(dampingRatio_ * dampingRatio_ - 1);
        coeffDecay_ = (-dampingRatio_ + coeffTmp) * naturalAngularVelocity;
        coeffScale_ = (initialOffset_ * ((dampingRatio_ + coeffTmp) * naturalAngularVelocity) += initialVelocity_) *=
            (0.5f / (naturalAngularVelocity * coeffTmp));
        coeffScaleAlt_ = (initialOffset_ * ((coeffTmp - dampingRatio_) * naturalAngularVelocity) -= initialVelocity_) *=
            (0.5f / (naturalAngularVelocity * coeffTmp));
        coeffDecayAlt_ = (-dampingRatio_ - coeffTmp) * naturalAngularVelocity;
    }
}

template<>
float RSSpringModel<std::shared_ptr<RSRenderPropertyBase>>::EstimateDuration() const
{
    if (dampingRatio_ <= 0.0f) {
        ROSEN_LOGE("RSSpringModel::%s, uninitialized spring model", __func__);
        return 0.0f;
    }

    // convert templated type to float, simplify estimation of spring duration
    float coeffScale = coeffScale_->ToFloat();
    float initialOffset = initialOffset_->ToFloat();
    float estimatedDuration = 0.0f;
    float minimumAmplitude = std::max(initialOffset * minimumAmplitudeRatio_, SPRING_MIN_AMPLITUDE);

    if (dampingRatio_ < 1) { // Under-damped
        estimatedDuration = log(fmax(coeffScale, initialOffset) / minimumAmplitude) / -coeffDecay_;
    } else if (dampingRatio_ == 1) { // Critically-damped
        // critical damping spring will rest at 2 * natural period
        estimatedDuration = response_ * 2;
    } else { // Over-damped
        float coeffScaleAlt = coeffScaleAlt_->ToFloat();
        double durationMain =
            (coeffScale <= minimumAmplitude) ? 0 : (log(coeffScale / minimumAmplitude) / -coeffDecay_);
        double durationAlt =
            (coeffScaleAlt <= minimumAmplitude) ? 0 : (log(coeffScaleAlt / minimumAmplitude) / -coeffDecayAlt_);
        estimatedDuration = fmax(durationMain, durationAlt);
    }
    return std::clamp(estimatedDuration, SPRING_MIN_DURATION, SPRING_MAX_DURATION);
}

template<>
std::shared_ptr<RSRenderPropertyBase> RSSpringModel<std::shared_ptr<RSRenderPropertyBase>>::CalculateDisplacement(
    double time) const
{
    if (dampingRatio_ <= 0.0f) {
        ROSEN_LOGE("RSSpringModel::%s, uninitialized spring model", __func__);
        return {};
    }
    double coeffDecay = exp(coeffDecay_ * time);
    if (dampingRatio_ < 1) {
        // under-damped
        double rad = dampedAngularVelocity_ * time;
        auto coeffPeriod = (initialOffset_ * cos(rad)) += (coeffScale_ * sin(rad));
        return coeffPeriod *= coeffDecay;
    } else if (dampingRatio_ == 1) {
        // critical-damped
        return (initialOffset_ + coeffScale_ * time) *= coeffDecay;
    } else {
        // over-damped
        double coeffDecayAlt = exp(coeffDecayAlt_ * time);
        return (coeffScale_ * coeffDecay) += (coeffScaleAlt_ * coeffDecayAlt);
    }
}

template<>
float RSSpringModel<float>::EstimateDuration() const
{
    if (dampingRatio_ <= 0.0f) {
        ROSEN_LOGE("RSSpringModel::%s, uninitialized spring model", __func__);
        return 0.0f;
    }
    float estimatedDuration = 0.0f;
    float naturalAngularVelocity = 2.0f * PI / response_;
    float threshold = toFloat(minimumAmplitudeRatio_ * initialOffset_);
    if (dampingRatio_ < 1.0f) { // Under-damped
        float dampingAngularVelocity = sqrt(1.0f - dampingRatio_ * dampingRatio_) * naturalAngularVelocity;
        float tmpCoeffA = -1.0f / (dampingRatio_ * naturalAngularVelocity);
        float tmpCoeffB = sqrt(
            pow(initialOffset_, 2) +
            (pow((initialVelocity_ + dampingRatio_ * naturalAngularVelocity * initialOffset_) / dampingAngularVelocity,
                2)));
        estimatedDuration = tmpCoeffA * log(std::fabs(threshold / tmpCoeffB));
    } else if (dampingRatio_ == 1.0f) { // Critically-damped
        float tmpCoeff = (initialVelocity_ + naturalAngularVelocity * initialOffset_);
        if (tmpCoeff == 0.0f) {
            estimatedDuration = 1 / naturalAngularVelocity * log(std::fabs(initialOffset_ / threshold));
        } else {
            float extremumTime = initialVelocity_ / (naturalAngularVelocity * tmpCoeff);
            if (extremumTime <= 0.0f) {
                threshold = initialOffset_ >= 0.0f ? threshold : -threshold;
                estimatedDuration = BinarySearchTime(0, SPRING_MAX_DURATION, threshold);
            } else {
                threshold = initialVelocity_ >= 0.0f ? threshold : -threshold;
                float extremumValue = RSSpringModel<float>::CalculateDisplacement(extremumTime);
                if (std::fabs(extremumValue) < std::fabs(threshold)) {
                    estimatedDuration = BinarySearchTime(0, extremumTime, threshold);
                } else {
                    estimatedDuration = BinarySearchTime(extremumTime, SPRING_MAX_DURATION, threshold);
                }
            }
        }
    } else { // Over-damped
        float tmpCoeffA = dampingRatio_ + sqrt(pow(dampingRatio_, 2) - 1);
        float tmpCoeffB = dampingRatio_ - sqrt(pow(dampingRatio_, 2) - 1);
        float tmpCoeffC = initialOffset_ * naturalAngularVelocity + initialVelocity_ * tmpCoeffA;
        float tmpCoeffD = initialOffset_ * naturalAngularVelocity + initialVelocity_ * tmpCoeffB;
        float tmpCoeffE = 1.0f / (2.0f * naturalAngularVelocity * sqrt(pow(dampingRatio_, 2) - 1.0f));

        if (tmpCoeffC == 0.0f) {
            float tmpCoeff = initialOffset_ * naturalAngularVelocity * tmpCoeffA + initialVelocity_;
            estimatedDuration =
                1 / (-tmpCoeffB * naturalAngularVelocity) * log(std::fabs(tmpCoeffE * threshold / tmpCoeff));
        } else if (tmpCoeffB == 0.0f) {
            float tmpCoeff = -initialOffset_ * naturalAngularVelocity * tmpCoeffB - initialVelocity_;
            estimatedDuration =
                1 / (-tmpCoeffA * naturalAngularVelocity) * log(std::fabs(tmpCoeffE * threshold / tmpCoeff));
        } else if (tmpCoeffA / tmpCoeffB <= 1.0f) {
            threshold = initialOffset_ >= 0.0f ? threshold : -threshold;
            estimatedDuration = BinarySearchTime(0.0f, SPRING_MAX_DURATION, threshold);
        } else {
            float extremumTime = 1 / tmpCoeffE * log(tmpCoeffC / tmpCoeffD);
            threshold = initialVelocity_ > 0.0f ? threshold : -threshold;
            float extremumValue = RSSpringModel<float>::CalculateDisplacement(extremumTime);
            if (std::fabs(extremumValue) < std::fabs(threshold)) {
                estimatedDuration = BinarySearchTime(0, extremumTime, threshold);
            } else {
                estimatedDuration = BinarySearchTime(extremumTime, SPRING_MAX_DURATION, threshold);
            }
        }
    }
    return std::clamp(estimatedDuration, SPRING_MIN_DURATION, SPRING_MAX_DURATION);
}

template<>
float RSSpringModel<float>::BinarySearchTime(float left, float right, float target) const
{
    bool isIncrease = CalculateDisplacement(left) < CalculateDisplacement(right) ? true : false;
    return BinarySearchTime(left, right, target, isIncrease);
}

template<>
float RSSpringModel<float>::BinarySearchTime(float left, float right, float target, bool& isIncrease) const
{
    if (left >= right) {
        return right;
    }

    float midTime = left + (right - left) / 2.0f;
    auto midValue = RSSpringModel<float>::CalculateDisplacement(midTime);
    if (std::fabs(midValue - target) < 1e-6) {
        return midTime;
    } else if ((midValue < target) ^ isIncrease) {
        return BinarySearchTime(left, midTime, target, isIncrease);
    } else {
        return BinarySearchTime(midTime, right, target, isIncrease);
    }
}

template class RSSpringModel<float>;
template class RSSpringModel<Color>;
template class RSSpringModel<Matrix3f>;
template class RSSpringModel<Vector2f>;
template class RSSpringModel<Vector4f>;
template class RSSpringModel<RRect>;
template class RSSpringModel<Quaternion>;
template class RSSpringModel<Vector4<Color>>;
template class RSSpringModel<std::shared_ptr<RSFilter>>;
template class RSSpringModel<std::shared_ptr<RSRenderPropertyBase>>;
} // namespace Rosen
} // namespace OHOS
