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

#include "common/rs_rect.h"
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
constexpr float FLOAT_NEAR_ZERO_THRESHOLD = 1e-6f;
constexpr double DOUBLE_NEAR_ZERO_THRESHOLD = 1e-6;
constexpr float SPRING_MIN_THRESHOLD = 1.0f / 4096.0f;
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
    const RSAnimatableType& initialOffset, const RSAnimatableType& initialVelocity, float minimumAmplitude)
    : response_(response), dampingRatio_(dampingRatio), initialOffset_(initialOffset),
      initialVelocity_(initialVelocity), minimumAmplitudeRatio_(minimumAmplitude)
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
    if (dampingRatio_ < 0.0f) {
        ROSEN_LOGE("RSSpringModel::%s, uninitialized spring model", __func__);
        return 0.0f;
    }
    if (response_ < 0.0f || (ROSEN_EQ(initialOffset_, 0.0f) && ROSEN_EQ(initialVelocity_, 0.0f)) ||
        !std::isfinite(response_) || !std::isfinite(dampingRatio_) || !std::isfinite(initialVelocity_)) {
        ROSEN_LOGE("RSSpringModel::%s, parameters is invalid", __func__);
        return 0.0f;
    }
    float estimatedDuration = 0.0f;
    if (dampingRatio_ < 1.0f) { // Under-damped
        estimatedDuration = EstimateDurationForUnderDampedModel();
    } else if (dampingRatio_ == 1.0f) { // Critically-damped
        estimatedDuration = EstimateDurationForCriticalDampedModel();
    } else { // Over-damped
        estimatedDuration = EstimateDurationForOverDampedModel();
    }
    return std::clamp(estimatedDuration, SPRING_MIN_DURATION, SPRING_MAX_DURATION);
}

template<>
float RSSpringModel<float>::BinarySearchTime(float left, float right, float target) const
{
    float midTime = 0.0f;
    float midValue = 0.0f;
    bool isIncrease = CalculateDisplacement(left) < CalculateDisplacement(right);

    while (left < right - 1e-3) {
        midTime = left + (right - left) / 2.0f;
        midValue = CalculateDisplacement(midTime);
        if (!std::isfinite(midTime) || !std::isfinite(midValue)) {
            return right;
        }
        if (ROSEN_EQ(midValue, target, FLOAT_NEAR_ZERO_THRESHOLD)) {
            return midTime;
        }
        if ((midValue < target) != isIncrease) {
            right = midTime;
        } else {
            left = midTime;
        }
    }

    return right;
}

template<>
float RSSpringModel<float>::EstimateDurationForUnderDampedModel() const
{
    float threshold = fmax(toFloat(minimumAmplitudeRatio_ * initialOffset_),
        SPRING_MIN_THRESHOLD); // avoiding 0 in logarithmic expressions
    double naturalAngularVelocity = 2.0 * M_PI / response_;
    double dampingAngularVelocity = sqrt(1.0 - dampingRatio_ * dampingRatio_) * naturalAngularVelocity;
    double tmpCoeffA = -1.0 / (dampingRatio_ * naturalAngularVelocity);
    double tmpCoeffB =
        sqrt(pow(initialOffset_, 2) +
             (pow((initialVelocity_ + dampingRatio_ * naturalAngularVelocity * initialOffset_) / dampingAngularVelocity,
                 2)));
    double tmpCoeffC = std::fabs(threshold / tmpCoeffB);
    if (ROSEN_EQ(tmpCoeffC, 0.0)) {
        return 0.0f;
    }
    float estimatedDuration = tmpCoeffA * log(tmpCoeffC);
    return estimatedDuration;
}

template<>
float RSSpringModel<float>::EstimateDurationForCriticalDampedModel() const
{
    float estimatedDuration = 0.0f;
    float threshold = fmax(toFloat(minimumAmplitudeRatio_ * initialOffset_), SPRING_MIN_THRESHOLD);
    double naturalAngularVelocity = 2.0 * M_PI / response_;
    double tmpCoeff = (initialVelocity_ + naturalAngularVelocity * initialOffset_);
    if (ROSEN_EQ(tmpCoeff, 0.0, DOUBLE_NEAR_ZERO_THRESHOLD)) {
        estimatedDuration = 1.0f / naturalAngularVelocity * log(std::fabs(initialOffset_ / threshold));
        return estimatedDuration;
    }
    float extremumTime = initialVelocity_ / (naturalAngularVelocity * tmpCoeff);
    extremumTime = std::clamp(extremumTime, SPRING_MIN_DURATION, SPRING_MAX_DURATION);
    float extremumValue = CalculateDisplacement(extremumTime);
    threshold = extremumValue > 0.0f ? threshold : -threshold;
    if (std::fabs(extremumValue) < std::fabs(threshold)) {
        estimatedDuration = BinarySearchTime(SPRING_MIN_DURATION, extremumTime, threshold);
    } else {
        estimatedDuration = BinarySearchTime(extremumTime, SPRING_MAX_DURATION, threshold);
    }
    return estimatedDuration;
}

template<>
float RSSpringModel<float>::EstimateDurationForOverDampedModel() const
{
    float estimatedDuration = 0.0f;
    float threshold = fmax(toFloat(minimumAmplitudeRatio_ * initialOffset_), SPRING_MIN_THRESHOLD);
    double naturalAngularVelocity = 2.0 * M_PI / response_;
    double tmpCoeffA = dampingRatio_ + sqrt(pow(dampingRatio_, 2) - 1.0);
    double tmpCoeffB = dampingRatio_ - sqrt(pow(dampingRatio_, 2) - 1.0);
    double tmpCoeffC = initialOffset_ * naturalAngularVelocity + initialVelocity_ * tmpCoeffA;
    double tmpCoeffD = initialOffset_ * naturalAngularVelocity + initialVelocity_ * tmpCoeffB;
    double tmpCoeffE = 1.0 / (2.0 * naturalAngularVelocity * sqrt(pow(dampingRatio_, 2) - 1.0));
    if (ROSEN_EQ(tmpCoeffC, 0.0, DOUBLE_NEAR_ZERO_THRESHOLD)) {
        double tmpCoeff = initialOffset_ * naturalAngularVelocity * tmpCoeffA + initialVelocity_;
        estimatedDuration =
            1.0f / (-tmpCoeffB * naturalAngularVelocity) * log(std::fabs(tmpCoeffE * threshold / tmpCoeff));
        return estimatedDuration;
    }
    if (ROSEN_EQ(tmpCoeffD, 0.0, DOUBLE_NEAR_ZERO_THRESHOLD)) {
        double tmpCoeff = -initialOffset_ * naturalAngularVelocity * tmpCoeffB - initialVelocity_;
        estimatedDuration =
            1.0f / (-tmpCoeffA * naturalAngularVelocity) * log(std::fabs(tmpCoeffE * threshold / tmpCoeff));
        return estimatedDuration;
    }
    float extremumTime = (tmpCoeffC / tmpCoeffD > DOUBLE_NEAR_ZERO_THRESHOLD) ? tmpCoeffE * log(tmpCoeffC / tmpCoeffD)
                                                                              : SPRING_MIN_DURATION;
    extremumTime = std::clamp(extremumTime, SPRING_MIN_DURATION, SPRING_MAX_DURATION);
    float extremumValue = CalculateDisplacement(extremumTime);
    threshold = extremumValue > 0.0f ? threshold : -threshold;
    if (std::fabs(extremumValue) < std::fabs(threshold)) {
        estimatedDuration = BinarySearchTime(SPRING_MIN_DURATION, extremumTime, threshold);
    } else {
        estimatedDuration = BinarySearchTime(extremumTime, SPRING_MAX_DURATION, threshold);
    }
    return estimatedDuration;
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
