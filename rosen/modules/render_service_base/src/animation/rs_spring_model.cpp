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
constexpr float FLOAT_NEAR_ZERO_THRESHOLD = 1e-6f;
constexpr double DOUBLE_NEAR_ZERO_THRESHOLD = 1e-6;
constexpr float SPRING_MIN_THRESHOLD = 5e-5f;

template<>
float toFloat(float value)
{
    return std::fabs(value);
}
} // namespace

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
    double naturalAngularVelocity = 2 * FLOAT_PI / response_;
    if (dampingRatio_ < 1) { // Under-damped Systems
        dampedAngularVelocity_ = naturalAngularVelocity * sqrt(1.0f - dampingRatio_ * dampingRatio_);
        coeffDecay_ = -dampingRatio_ * naturalAngularVelocity;
        coeffScale_ = (initialVelocity_ + initialOffset_ * dampingRatio_ * naturalAngularVelocity) *=
            (1 / dampedAngularVelocity_);
    } else if (ROSEN_EQ(dampingRatio_, 1.0f)) { // Critically-Damped Systems
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
        ROSEN_LOGE("RSSpringModel::%{public}s, uninitialized spring model", __func__);
        return 0.0f;
    }

    // convert templated type to float, simplify estimation of spring duration
    float coeffScale = coeffScale_->ToFloat();
    float initialOffset = initialOffset_->ToFloat();
    float estimatedDuration = 0.0f;
    float minimumAmplitude = std::max(initialOffset * minimumAmplitudeRatio_, SPRING_MIN_AMPLITUDE);

    if (dampingRatio_ < 1) { // Under-damped
        if (ROSEN_EQ(coeffDecay_, 0.0f)) {
            ROSEN_LOGE("RSSpringModel::%{public}s, coeffDecay_ euqal zero.", __func__);
            return 0.0f;
        }
        estimatedDuration = log(fmax(coeffScale, initialOffset) / minimumAmplitude) / -coeffDecay_;
    } else if (ROSEN_EQ(dampingRatio_, 1.0f)) { // Critically-damped
        // critical damping spring use dampingRatio = 0.999 to esimate duration approximately
        constexpr float dampingRatio = 0.999f;
        double naturalAngularVelocity = 2 * FLOAT_PI / response_;
        double dampedAngularVelocity = naturalAngularVelocity * sqrt(1.0f - dampingRatio * dampingRatio);
        if (ROSEN_EQ(dampedAngularVelocity, 0.0)) {
            return 0.0f;
        }
        double tempCoeffA = 1.0 / (dampingRatio * naturalAngularVelocity);
        double tempCoeffB = toFloat((initialVelocity_ + initialOffset_ * dampingRatio * naturalAngularVelocity) *
                                    (1 / dampedAngularVelocity));
        double tempCoeffC = sqrt(initialOffset * initialOffset + tempCoeffB * tempCoeffB);
        if (ROSEN_EQ(tempCoeffC, 0.0)) {
            return 0.0f;
        }
        estimatedDuration = log(tempCoeffC / minimumAmplitude) * tempCoeffA;
    } else { // Over-damped
        if (ROSEN_EQ(coeffDecay_, 0.0f) || ROSEN_EQ(coeffDecayAlt_, 0.0f)) {
            ROSEN_LOGE("RSSpringModel::%{public}s, coeffDecay_ or coeffDecayAlt_ euqal zero.", __func__);
            return 0.0f;
        }
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
        ROSEN_LOGE("RSSpringModel::%{public}s, uninitialized spring model", __func__);
        return {};
    }
    double coeffDecay = exp(coeffDecay_ * time);
    if (dampingRatio_ < 1) {
        // under-damped
        double rad = dampedAngularVelocity_ * time;
        auto coeffPeriod = (initialOffset_ * cos(rad)) += (coeffScale_ * sin(rad));
        return coeffPeriod *= coeffDecay;
    } else if (ROSEN_EQ(dampingRatio_, 1.0f)) {
        // critical-damped
        return ((coeffScale_ * time) += initialOffset_) *= coeffDecay;
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
        ROSEN_LOGE("RSSpringModel::%{public}s, uninitialized spring model", __func__);
        return 0.0f;
    }
    if (response_ < 0.0f || (ROSEN_EQ(initialOffset_, 0.0f) && ROSEN_EQ(initialVelocity_, 0.0f)) ||
        !std::isfinite(response_) || !std::isfinite(dampingRatio_) || !std::isfinite(initialVelocity_)) {
        ROSEN_LOGE("RSSpringModel::%{public}s, parameters is invalid", __func__);
        return 0.0f;
    }
    float estimatedDuration = 0.0f;
    if (dampingRatio_ < 1.0f) { // Under-damped
        estimatedDuration = EstimateDurationForUnderDampedModel();
    } else if (ROSEN_EQ(dampingRatio_, 1.0f)) { // Critically-damped
        estimatedDuration = EstimateDurationForCriticalDampedModel();
    } else { // Over-damped
        estimatedDuration = EstimateDurationForOverDampedModel();
    }
    return std::clamp(estimatedDuration, SPRING_MIN_DURATION, SPRING_MAX_DURATION);
}

template<>
float RSSpringModel<float>::BinarySearchTime(float left, float right, float target) const
{
    bool isIncrease = CalculateDisplacement(left) < CalculateDisplacement(right);

    while (left < right - 1e-3) {
        float midTime = left + (right - left) / 2.0f;
        float midValue = CalculateDisplacement(midTime);
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
    if (response_ <= 0.0f) {
        ROSEN_LOGE("RSSpringModel<float>::EstimateDurationForUnderDampedModel, uninitialized response.");
        return 0.0f;
    }

    float threshold = fmax(toFloat(minimumAmplitudeRatio_ * initialOffset_),
        SPRING_MIN_THRESHOLD); // avoiding 0 in logarithmic expressions
    double naturalAngularVelocity = 2.0 * FLOAT_PI / response_;
    double dampingAngularVelocity = sqrt(1.0 - dampingRatio_ * dampingRatio_) * naturalAngularVelocity;
    if (ROSEN_EQ(dampingAngularVelocity, 0.0)) {
        ROSEN_LOGE("RSSpringModel<float>::EstimateDurationForUnderDampedModel, dampingAngularVelocity equal zero.");
        // critical damping spring will almost rest at 2 * natural period
        return response_ * 2;
    }
    double tmpCoeffA = -1.0 / (dampingRatio_ * naturalAngularVelocity);
    double tmpCoeffB =
        sqrt(pow(initialOffset_, 2) +
             (pow((initialVelocity_ + dampingRatio_ * naturalAngularVelocity * initialOffset_) / dampingAngularVelocity,
                 2)));
    if (ROSEN_EQ(tmpCoeffB, 0.0)) {
        return 0.0f;
    }
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
    if (response_ <= 0.0f) {
        ROSEN_LOGE("RSSpringModel::EstimateDurationForCriticalDampedModel, uninitialized response.");
        return 0.0f;
    }

    float estimatedDuration = 0.0f;
    float threshold = fmax(toFloat(minimumAmplitudeRatio_ * initialOffset_), SPRING_MIN_THRESHOLD);
    double naturalAngularVelocity = 2.0 * FLOAT_PI / response_;
    double tmpCoeff = (initialVelocity_ + naturalAngularVelocity * initialOffset_);
    if (ROSEN_EQ(tmpCoeff, 0.0, DOUBLE_NEAR_ZERO_THRESHOLD)) {
        if (ROSEN_EQ(naturalAngularVelocity, 0.0) || ROSEN_EQ(initialOffset_ / threshold, 0.0f)) {
            ROSEN_LOGE("RSSpringModel::EstimateDurationForCriticalDampedModel, invalid parameters.");
            return 0.0f;
        }
        estimatedDuration = 1.0f / naturalAngularVelocity * log(std::fabs(initialOffset_ / threshold));
        return estimatedDuration;
    }
    if (ROSEN_EQ(naturalAngularVelocity * tmpCoeff, 0.0)) {
        ROSEN_LOGE("RSSpringModel::EstimateDurationForCriticalDampedModel, denominator euqal zero.");
        return 0.0f;
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
    if (response_ <= 0.0f) {
        ROSEN_LOGE("RSSpringModel::EstimateDurationForOverDampedModel, uninitialized response.");
        return 0.0f;
    }
    float estimatedDuration = 0.0f;
    float threshold = fmax(toFloat(minimumAmplitudeRatio_ * initialOffset_), SPRING_MIN_THRESHOLD);
    double naturalAngularVelocity = 2.0 * FLOAT_PI / response_;
    double tmpCoeffA = dampingRatio_ + sqrt(pow(dampingRatio_, 2) - 1.0);
    double tmpCoeffB = dampingRatio_ - sqrt(pow(dampingRatio_, 2) - 1.0);
    double tmpCoeffC = initialOffset_ * naturalAngularVelocity + initialVelocity_ * tmpCoeffA;
    double tmpCoeffD = initialOffset_ * naturalAngularVelocity + initialVelocity_ * tmpCoeffB;
    double tmpCoeffE = 2.0 * naturalAngularVelocity * sqrt(pow(dampingRatio_, 2) - 1.0);
    if (ROSEN_EQ(tmpCoeffE, 0.0)) {
        ROSEN_LOGE("RSSpringModel<float>::EstimateDurationForOverDampedModel(), invalid parameters.");
        return 0.0f;
    }
    double tmpCoeffF = 1.0 / tmpCoeffE;
    if (ROSEN_EQ(tmpCoeffC, 0.0, DOUBLE_NEAR_ZERO_THRESHOLD)) {
        double tmpCoeff = initialOffset_ * naturalAngularVelocity * tmpCoeffA + initialVelocity_;
        if (ROSEN_EQ(-tmpCoeffB * naturalAngularVelocity, 0.0) || ROSEN_EQ(tmpCoeff, 0.0)) {
            ROSEN_LOGE("RSSpringModel<float>::EstimateDurationForOverDampedModel(), denominator equal zero.");
            return 0.0f;
        }
        estimatedDuration =
            1.0f / (-tmpCoeffB * naturalAngularVelocity) * log(std::fabs(tmpCoeffF * threshold / tmpCoeff));
        return estimatedDuration;
    }
    if (ROSEN_EQ(tmpCoeffD, 0.0, DOUBLE_NEAR_ZERO_THRESHOLD)) {
        double tmpCoeff = -initialOffset_ * naturalAngularVelocity * tmpCoeffB - initialVelocity_;
        if (ROSEN_EQ(-tmpCoeffA * naturalAngularVelocity, 0.0) || ROSEN_EQ(tmpCoeff, 0.0)) {
            return 0.0f;
        }
        estimatedDuration =
            1.0f / (-tmpCoeffA * naturalAngularVelocity) * log(std::fabs(tmpCoeffF * threshold / tmpCoeff));
        return estimatedDuration;
    }
    float extremumTime = (tmpCoeffC / tmpCoeffD > DOUBLE_NEAR_ZERO_THRESHOLD) ? tmpCoeffF * log(tmpCoeffC / tmpCoeffD)
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
template class RSSpringModel<RRect>;
template class RSSpringModel<Vector4<Color>>;
template class RSSpringModel<std::shared_ptr<RSFilter>>;
template class RSSpringModel<std::shared_ptr<RSRenderPropertyBase>>;
} // namespace Rosen
} // namespace OHOS
