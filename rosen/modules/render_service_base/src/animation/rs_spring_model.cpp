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

bool WillOvershootImpl(float response, float dampingRatio, double initialOffset, double initialVelocity)
{
    // The underdamped model does not have the overshoot state.
    if (ROSEN_LNE(dampingRatio, 1.0f, SPRING_DAMPING_RATIO_EPSILON)) {
        return false;
    }
    // Determine whether the future speed direction will change by calculating the time of the extreme point.
    // For details, see EstimateDurationForCriticalDampedModel and EstimateDurationForOverDampedModel.
    double naturalAngularVelocity = 2.0 * FLOAT_PI / response;
    if (ROSEN_EQ(dampingRatio, 1.0f)) {
        double tmpCoeff = initialVelocity + naturalAngularVelocity * initialOffset;
        if (ROSEN_EQ(tmpCoeff, 0.0, DOUBLE_NEAR_ZERO_THRESHOLD)) {
            return false;
        }
        if (ROSEN_EQ(naturalAngularVelocity * tmpCoeff, 0.0)) {
            return false;
        }
        float extremumTime = initialVelocity / (naturalAngularVelocity * tmpCoeff);
        return extremumTime > 0.0f;
    } else {
        double tmpCoeffE = 2.0 * naturalAngularVelocity * sqrt(pow(dampingRatio, 2) - 1.0);
        if (ROSEN_EQ(tmpCoeffE, 0.0)) {
            return false;
        }
        double tmpCoeffA = dampingRatio + sqrt(pow(dampingRatio, 2) - 1.0);
        double tmpCoeffC = initialOffset * naturalAngularVelocity + initialVelocity * tmpCoeffA;
        if (ROSEN_EQ(tmpCoeffC, 0.0, DOUBLE_NEAR_ZERO_THRESHOLD)) {
            return false;
        }
        double tmpCoeffB = dampingRatio - sqrt(pow(dampingRatio, 2) - 1.0);
        double tmpCoeffD = initialOffset * naturalAngularVelocity + initialVelocity * tmpCoeffB;
        if (ROSEN_EQ(tmpCoeffD, 0.0, DOUBLE_NEAR_ZERO_THRESHOLD)) {
            return false;
        }
        double tmpCoeffG = tmpCoeffC / tmpCoeffD;
        if (tmpCoeffG <= DOUBLE_NEAR_ZERO_THRESHOLD) {
            return false;
        }
        double tmpCoeffF = 1.0 / tmpCoeffE;
        float extremumTime = tmpCoeffF * log(tmpCoeffG);
        return extremumTime > 0.0f;
    }
}

template<size_t N>
bool WillOvershootByData(float response, float dampingRatio, const float* offsetData, const float* velocityData)
{
    // Each dimension must not overshoot to be considered non-overshoot.
    for (size_t i = 0; i < N; ++i) {
        if (WillOvershootImpl(response, dampingRatio, offsetData[i], velocityData[i])) {
            return true;
        }
    }
    return false;
}
} // namespace

// Explicit Instantiation
#define DECLARE_PROPERTY(T, TYPE_ENUM)
#define DECLARE_ANIMATABLE_PROPERTY(T, TYPE_ENUM) template class RSSpringModel<T>
#include "modifier/rs_property_def.in"
#undef DECLARE_PROPERTY
#undef DECLARE_ANIMATABLE_PROPERTY

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
    if (ROSEN_EQ(naturalAngularVelocity, 0.0, DOUBLE_NEAR_ZERO_THRESHOLD)) {
        naturalAngularVelocity = DOUBLE_NEAR_ZERO_THRESHOLD;
    }
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
    if (dampingRatio_ <= 0.0f || response_ <= 0.0f) {
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

template<>
bool RSSpringModel<float>::WillOvershootInner(
    float response, float dampingRatio, const float& initialOffset, const float& initialVelocity)
{
    if (response <= 0.0f) {
        ROSEN_LOGE("RSSpringModel::%{public}s, uninitialized response.", __func__);
        return false;
    }
    return WillOvershootImpl(response, dampingRatio, initialOffset, initialVelocity);
}

template<>
bool RSSpringModel<Vector2f>::WillOvershootInner(
    float response, float dampingRatio, const Vector2f& initialOffset, const Vector2f& initialVelocity)
{
    if (response <= 0.0f) {
        ROSEN_LOGE("RSSpringModel::%{public}s, uninitialized response.", __func__);
        return false;
    }
    return WillOvershootByData<Vector2f::V2SIZE>(response, dampingRatio, initialOffset.data_, initialVelocity.data_);
}

template<>
bool RSSpringModel<Vector3f>::WillOvershootInner(
    float response, float dampingRatio, const Vector3f& initialOffset, const Vector3f& initialVelocity)
{
    if (response <= 0.0f) {
        ROSEN_LOGE("RSSpringModel::%{public}s, uninitialized response.", __func__);
        return false;
    }
    return WillOvershootByData<Vector3f::V3SIZE>(response, dampingRatio, initialOffset.data_, initialVelocity.data_);
}

template<>
bool RSSpringModel<Vector4f>::WillOvershootInner(
    float response, float dampingRatio, const Vector4f& initialOffset, const Vector4f& initialVelocity)
{
    if (response <= 0.0f) {
        ROSEN_LOGE("RSSpringModel::%{public}s, uninitialized response.", __func__);
        return false;
    }
    return WillOvershootByData<Vector4f::V4SIZE>(response, dampingRatio, initialOffset.data_, initialVelocity.data_);
}

template<>
bool RSSpringModel<Quaternion>::WillOvershootInner(
    float response, float dampingRatio, const Quaternion& initialOffset, const Quaternion& initialVelocity)
{
    if (response <= 0.0f) {
        ROSEN_LOGE("RSSpringModel::%{public}s, uninitialized response.", __func__);
        return false;
    }
    return WillOvershootByData<Quaternion::V4SIZE>(response, dampingRatio, initialOffset.data_, initialVelocity.data_);
}

template<>
bool RSSpringModel<Matrix3f>::WillOvershootInner(
    float response, float dampingRatio, const Matrix3f& initialOffset, const Matrix3f& initialVelocity)
{
    if (response <= 0.0f) {
        ROSEN_LOGE("RSSpringModel::%{public}s, uninitialized response.", __func__);
        return false;
    }
    return WillOvershootByData<Matrix3f::MATRIX3_SIZE>(
        response, dampingRatio, initialOffset.GetConstData(), initialVelocity.GetConstData());
}

template<>
bool RSSpringModel<Color>::WillOvershootInner(
    float response, float dampingRatio, const Color& initialOffset, const Color& initialVelocity)
{
    if (response <= 0.0f) {
        ROSEN_LOGE("RSSpringModel::%{public}s, uninitialized response.", __func__);
        return false;
    }
    if (WillOvershootImpl(response, dampingRatio, initialOffset.GetAlpha(), initialVelocity.GetAlpha())) {
        return true;
    }
    if (WillOvershootImpl(response, dampingRatio, initialOffset.GetBlue(), initialVelocity.GetBlue())) {
        return true;
    }
    if (WillOvershootImpl(response, dampingRatio, initialOffset.GetGreen(), initialVelocity.GetGreen())) {
        return true;
    }
    if (WillOvershootImpl(response, dampingRatio, initialOffset.GetRed(), initialVelocity.GetRed())) {
        return true;
    }
    return false;
}

template<>
bool RSSpringModel<Vector4<Color>>::WillOvershootInner(
    float response, float dampingRatio, const Vector4<Color>& initialOffset, const Vector4<Color>& initialVelocity)
{
    if (response <= 0.0f) {
        ROSEN_LOGE("RSSpringModel::%{public}s, uninitialized response.", __func__);
        return false;
    }
    for (uint32_t i = 0; i < Vector4<Color>::V4SIZE; ++i) {
        const auto& offset = initialOffset.data_[i];
        const auto& velocity = initialVelocity.data_[i];
        if (WillOvershootImpl(response, dampingRatio, offset.GetAlpha(), velocity.GetAlpha())) {
            return true;
        }
        if (WillOvershootImpl(response, dampingRatio, offset.GetBlue(), velocity.GetBlue())) {
            return true;
        }
        if (WillOvershootImpl(response, dampingRatio, offset.GetGreen(), velocity.GetGreen())) {
            return true;
        }
        if (WillOvershootImpl(response, dampingRatio, offset.GetRed(), velocity.GetRed())) {
            return true;
        }
    }
    return false;
}

template<>
bool RSSpringModel<RRect>::WillOvershootInner(
    float response, float dampingRatio, const RRect& initialOffset, const RRect& initialVelocity)
{
    if (response <= 0.0f) {
        ROSEN_LOGE("RSSpringModel::%{public}s, uninitialized response.", __func__);
        return false;
    }
    if (WillOvershootByData<4>(response, dampingRatio, initialOffset.rect_.data_, initialVelocity.rect_.data_)) {
        return true;
    }
    for (uint32_t i = 0; i < 4; ++i) {
        if (WillOvershootByData<Vector2f::V2SIZE>(
                response, dampingRatio, initialOffset.radius_[i].data_, initialVelocity.radius_[i].data_)) {
            return true;
        }
    }
    return false;
}

template<>
float RSSpringModel<float>::Sqrt(float value)
{
    return sqrt(std::fabs(value));
}

template<>
Vector2f RSSpringModel<Vector2f>::Sqrt(Vector2f value)
{
    return Vector2f(sqrt(std::fabs(value.x_)), sqrt(std::fabs(value.y_)));
}

template<>
Vector3f RSSpringModel<Vector3f>::Sqrt(Vector3f value)
{
    return Vector3f(sqrt(std::fabs(value.x_)), sqrt(std::fabs(value.y_)), sqrt(std::fabs(value.z_)));
}

template<>
Vector4f RSSpringModel<Vector4f>::Sqrt(Vector4f value)
{
    return Vector4f(
        sqrt(std::fabs(value.x_)), sqrt(std::fabs(value.y_)), sqrt(std::fabs(value.z_)), sqrt(std::fabs(value.w_)));
}

template<>
Quaternion RSSpringModel<Quaternion>::Sqrt(Quaternion value)
{
    return Quaternion(
        sqrt(std::fabs(value.x_)), sqrt(std::fabs(value.y_)), sqrt(std::fabs(value.z_)), sqrt(std::fabs(value.w_)));
}

template<>
Matrix3f RSSpringModel<Matrix3f>::Sqrt(Matrix3f value)
{
    const float* data = value.GetConstData();
    return Matrix3f(sqrt(std::fabs(data[0])), sqrt(std::fabs(data[1])), sqrt(std::fabs(data[2])),
        sqrt(std::fabs(data[3])), sqrt(std::fabs(data[4])), sqrt(std::fabs(data[5])),
        sqrt(std::fabs(data[6])), sqrt(std::fabs(data[7])), sqrt(std::fabs(data[8])));
}

template<>
Color RSSpringModel<Color>::Sqrt(Color value)
{
    return Color(static_cast<int16_t>(sqrt(std::fabs(value.GetRed()))),
        static_cast<int16_t>(sqrt(std::fabs(value.GetGreen()))),
        static_cast<int16_t>(sqrt(std::fabs(value.GetBlue()))),
        static_cast<int16_t>(sqrt(std::fabs(value.GetAlpha()))));
}

template<>
Vector4<Color> RSSpringModel<Vector4<Color>>::Sqrt(Vector4<Color> value)
{
    return Vector4<Color>(
        Color(static_cast<int16_t>(sqrt(std::fabs(value.data_[0].GetRed()))),
            static_cast<int16_t>(sqrt(std::fabs(value.data_[0].GetGreen()))),
            static_cast<int16_t>(sqrt(std::fabs(value.data_[0].GetBlue()))),
            static_cast<int16_t>(sqrt(std::fabs(value.data_[0].GetAlpha())))),
        Color(static_cast<int16_t>(sqrt(std::fabs(value.data_[1].GetRed()))),
            static_cast<int16_t>(sqrt(std::fabs(value.data_[1].GetGreen()))),
            static_cast<int16_t>(sqrt(std::fabs(value.data_[1].GetBlue()))),
            static_cast<int16_t>(sqrt(std::fabs(value.data_[1].GetAlpha())))),
        Color(static_cast<int16_t>(sqrt(std::fabs(value.data_[2].GetRed()))),
            static_cast<int16_t>(sqrt(std::fabs(value.data_[2].GetGreen()))),
            static_cast<int16_t>(sqrt(std::fabs(value.data_[2].GetBlue()))),
            static_cast<int16_t>(sqrt(std::fabs(value.data_[2].GetAlpha())))),
        Color(static_cast<int16_t>(sqrt(std::fabs(value.data_[3].GetRed()))),
            static_cast<int16_t>(sqrt(std::fabs(value.data_[3].GetGreen()))),
            static_cast<int16_t>(sqrt(std::fabs(value.data_[3].GetBlue()))),
            static_cast<int16_t>(sqrt(std::fabs(value.data_[3].GetAlpha())))));
}

template<>
RRect RSSpringModel<RRect>::Sqrt(RRect value)
{
    RRect result;
    for (int i = 0; i < 4; ++i) {
        result.rect_.data_[i] = sqrt(std::fabs(value.rect_.data_[i]));
    }
    for (int i = 0; i < 4; ++i) {
        result.radius_[i] = Vector2f(sqrt(std::fabs(value.radius_[i].x_)), sqrt(std::fabs(value.radius_[i].y_)));
    }
    return result;
}

template<>
float RSSpringModel<float>::GetFrameThreshold(double time) const
{
    if (ROSEN_GE(dampingRatio_, 1.0f, SPRING_DAMPING_RATIO_EPSILON)) {
        return CalculateDisplacement(time);
    }
    auto squareInitialOffset = initialOffset_ * initialOffset_;
    auto squareCoeffScale = coeffScale_ * coeffScale_;
    double coeffDecay = exp(coeffDecay_ * time);
    return Sqrt(squareInitialOffset + squareCoeffScale) * coeffDecay;
}

template<>
Vector2f RSSpringModel<Vector2f>::GetFrameThreshold(double time) const
{
    if (ROSEN_GE(dampingRatio_, 1.0f, SPRING_DAMPING_RATIO_EPSILON)) {
        return CalculateDisplacement(time);
    }
    auto squareInitialOffset = initialOffset_ * initialOffset_;
    auto squareCoeffScale = coeffScale_ * coeffScale_;
    double coeffDecay = exp(coeffDecay_ * time);
    return Sqrt(squareInitialOffset + squareCoeffScale) * coeffDecay;
}

template<>
Vector3f RSSpringModel<Vector3f>::GetFrameThreshold(double time) const
{
    if (ROSEN_GE(dampingRatio_, 1.0f, SPRING_DAMPING_RATIO_EPSILON)) {
        return CalculateDisplacement(time);
    }
    auto squareInitialOffset = initialOffset_ * initialOffset_;
    auto squareCoeffScale = coeffScale_ * coeffScale_;
    double coeffDecay = exp(coeffDecay_ * time);
    return Sqrt(squareInitialOffset + squareCoeffScale) * coeffDecay;
}

template<>
Vector4f RSSpringModel<Vector4f>::GetFrameThreshold(double time) const
{
    if (ROSEN_GE(dampingRatio_, 1.0f, SPRING_DAMPING_RATIO_EPSILON)) {
        return CalculateDisplacement(time);
    }
    auto squareInitialOffset = initialOffset_ * initialOffset_;
    auto squareCoeffScale = coeffScale_ * coeffScale_;
    double coeffDecay = exp(coeffDecay_ * time);
    return Sqrt(squareInitialOffset + squareCoeffScale) * coeffDecay;
}

template<>
Quaternion RSSpringModel<Quaternion>::GetFrameThreshold(double time) const
{
    if (ROSEN_GE(dampingRatio_, 1.0f, SPRING_DAMPING_RATIO_EPSILON)) {
        return CalculateDisplacement(time);
    }
    auto squareInitialOffset = initialOffset_ * initialOffset_;
    auto squareCoeffScale = coeffScale_ * coeffScale_;
    double coeffDecay = exp(coeffDecay_ * time);
    return Sqrt(squareInitialOffset + squareCoeffScale) * coeffDecay;
}

template<>
Matrix3f RSSpringModel<Matrix3f>::GetFrameThreshold(double time) const
{
    if (ROSEN_GE(dampingRatio_, 1.0f, SPRING_DAMPING_RATIO_EPSILON)) {
        return CalculateDisplacement(time);
    }
    auto squareInitialOffset = initialOffset_ * initialOffset_;
    auto squareCoeffScale = coeffScale_ * coeffScale_;
    double coeffDecay = exp(coeffDecay_ * time);
    return Sqrt(squareInitialOffset + squareCoeffScale) * coeffDecay;
}

template<>
Color RSSpringModel<Color>::GetFrameThreshold(double time) const
{
    if (ROSEN_GE(dampingRatio_, 1.0f, SPRING_DAMPING_RATIO_EPSILON)) {
        return CalculateDisplacement(time);
    }
    auto squareInitialOffset = initialOffset_ * initialOffset_;
    auto squareCoeffScale = coeffScale_ * coeffScale_;
    double coeffDecay = exp(coeffDecay_ * time);
    return Sqrt(squareInitialOffset + squareCoeffScale) * coeffDecay;
}

template<>
Vector4<Color> RSSpringModel<Vector4<Color>>::GetFrameThreshold(double time) const
{
    if (ROSEN_GE(dampingRatio_, 1.0f, SPRING_DAMPING_RATIO_EPSILON)) {
        return CalculateDisplacement(time);
    }
    auto squareInitialOffset = initialOffset_ * initialOffset_;
    auto squareCoeffScale = coeffScale_ * coeffScale_;
    double coeffDecay = exp(coeffDecay_ * time);
    return Sqrt(squareInitialOffset + squareCoeffScale) * coeffDecay;
}

template<>
RRect RSSpringModel<RRect>::GetFrameThreshold(double time) const
{
    if (ROSEN_GE(dampingRatio_, 1.0f, SPRING_DAMPING_RATIO_EPSILON)) {
        return CalculateDisplacement(time);
    }
    auto squareInitialOffset = initialOffset_ * initialOffset_;
    auto squareCoeffScale = coeffScale_ * coeffScale_;
    double coeffDecay = exp(coeffDecay_ * time);
    return Sqrt(squareInitialOffset + squareCoeffScale) * coeffDecay;
}

template<>
float RSSpringModel<float>::toFloat(float value)
{
    return std::fabs(value);
}
template<>
float RSSpringModel<Vector4f>::toFloat(Vector4f value)
{
    return value.GetLength();
}
template<>
float RSSpringModel<Quaternion>::toFloat(Quaternion value)
{
    return value.GetLength();
}
template<>
float RSSpringModel<Vector2f>::toFloat(Vector2f value)
{
    return value.GetLength();
}
} // namespace Rosen
} // namespace OHOS
