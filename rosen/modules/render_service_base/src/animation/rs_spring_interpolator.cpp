/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "animation/rs_spring_interpolator.h"

#include <algorithm>
#include <cmath>

#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
constexpr float MIN_DAMPING_RATIO = 0.0f;
constexpr float MAX_DAMPING_RATIO = 2.0f;
constexpr float MIN_DURATION = 0.02f;
constexpr float MAX_DURATION = 300.0f;
constexpr float SECONDS_TO_MICROSECONDS = 1e6f;
constexpr float MIN_RESPONSE = 1e-8;
constexpr float MIN_AMPLITUDE = 0.001f;

RSSpringInterpolator::RSSpringInterpolator(float response, float dampingRatio, float initialOffset,
    float minimumAmplitude, float initialVelocity, float duration)
    : response_(response), dampingRatio_(dampingRatio), initialVelocity_(initialVelocity),
      initialOffset_(initialOffset), minimumAmplitude_(minimumAmplitude), duration_(duration)
{
    CalculateSpringParameters();
    ROSEN_LOGD("SAG, %s response = %.2f, dampingRatio = %.2f, initialVelocity "
               "= %.2f, initialOffset = %.2f minimumAmplitude = %.5f, duration = %.2f",
        __func__, response_, dampingRatio_, initialVelocity_, initialOffset_, minimumAmplitude_, duration_);
}

bool RSSpringInterpolator::Marshalling(Parcel& parcel) const
{
    if (!parcel.WriteUint16(InterpolatorType::SPRING)) {
        ROSEN_LOGE("RSSpringInterpolator::Marshalling, Write type failed");
        return false;
    }
    if (!(parcel.WriteFloat(response_) && parcel.WriteFloat(dampingRatio_) && parcel.WriteFloat(initialVelocity_) &&
            parcel.WriteFloat(initialOffset_) && parcel.WriteFloat(minimumAmplitude_) &&
            parcel.WriteFloat(duration_))) {
        ROSEN_LOGE("RSSpringInterpolator::Marshalling, Write value failed");
        return false;
    }
    return true;
}

RSSpringInterpolator* RSSpringInterpolator::Unmarshalling(Parcel& parcel)
{
    float response, dampingRatio, initialVelocity, initialOffset, minimumAmplitude, duration;
    if (!(parcel.ReadFloat(response) && parcel.ReadFloat(dampingRatio) && parcel.ReadFloat(initialVelocity) &&
            parcel.ReadFloat(initialOffset) && parcel.ReadFloat(minimumAmplitude) && parcel.ReadFloat(duration))) {
        ROSEN_LOGE("RSSpringInterpolator::Unmarshalling, SpringInterpolator failed");
        return nullptr;
    }
    auto ret =
        new RSSpringInterpolator(response, dampingRatio, initialOffset, minimumAmplitude, initialVelocity, duration);
    return ret;
}

float RSSpringInterpolator::InterpolateImpl(double seconds) const
{
    if (seconds <= 0) {
        return 0;
    } else if (seconds >= duration_) {
        return initialOffset_;
    }

    double displacement = CalculateDisplacement(seconds);
    return initialOffset_ - displacement;
}

double RSSpringInterpolator::CalculateDisplacement(double mappedTime) const
{
    double coeffDecay = exp(coeffDecay_ * mappedTime);
    if (dampingRatio_ < 1) {
        // under-damped
        double rad = dampedAngularVelocity_ * mappedTime;
        double coeffPeriod = initialOffset_ * cos(rad) + coeffScale_ * sin(rad);
        return coeffDecay * coeffPeriod;
    } else if (dampingRatio_ == 1) {
        // critical-damped
        return coeffDecay * (initialOffset_ + coeffScale_ * mappedTime);
    } else {
        // over-damped
        double coeffDecayMinus = exp(coeffDecayMinus_ * mappedTime);
        return coeffDecay * coeffScale_ + coeffDecayMinus * coeffScaleMinus_;
    }
}

void RSSpringInterpolator::CalculateSpringParameters()
{
    // handle wrong parameters:
    // 1. dampingRatio which not in [0, 2]
    // 2. response/minimumAmplitude which <= 0
    dampingRatio_ = std::clamp(dampingRatio_, MIN_DAMPING_RATIO, MAX_DAMPING_RATIO);
    if (response_ <= 0) {
        response_ = MIN_RESPONSE;
    }

    float naturalAngularVelocity = 2 * M_PI / response_;
    float initialVelocity = -initialVelocity_;
    if (dampingRatio_ < 1) {
        // Underdamped Systems
        dampedAngularVelocity_ = naturalAngularVelocity * sqrt(1.0f - dampingRatio_ * dampingRatio_);
        coeffDecay_ = -dampingRatio_ * naturalAngularVelocity;
        coeffScale_ =
            (initialVelocity + dampingRatio_ * naturalAngularVelocity * initialOffset_) / dampedAngularVelocity_;
    } else if (dampingRatio_ == 1) {
        // Critically-Damped Systems
        coeffDecay_ = -naturalAngularVelocity;
        coeffScale_ = initialVelocity + naturalAngularVelocity * initialOffset_;
    } else {
        // Overdamped Systems
        float coeffTmp = sqrt(dampingRatio_ * dampingRatio_ - 1);
        coeffDecay_ = (-dampingRatio_ + coeffTmp) * naturalAngularVelocity;
        coeffScale_ = ((dampingRatio_ + coeffTmp) * naturalAngularVelocity * initialOffset_ + initialVelocity) /
                      (2 * naturalAngularVelocity * coeffTmp);
        coeffScaleMinus_ = (-(dampingRatio_ - coeffTmp) * naturalAngularVelocity * initialOffset_ - initialVelocity) /
                           (2 * naturalAngularVelocity * coeffTmp);
        coeffDecayMinus_ = (-dampingRatio_ - coeffTmp) * naturalAngularVelocity;
    }

    if (duration_ <= 0) {
        EstimateDuration();
    }
}

void RSSpringInterpolator::EstimateDuration()
{
    if (minimumAmplitude_ <= 0) {
        minimumAmplitude_ = MIN_AMPLITUDE;
    }
    if (dampingRatio_ < 1) {
        // Underdamped Systems
        if (coeffScale_ != 0) {
            duration_ = log(abs(coeffScale_) / minimumAmplitude_) / -coeffDecay_;
        }
    } else if (dampingRatio_ == 1) {
        // Critically-Damped Systems
        if ((coeffScale_ + initialOffset_) != 0) {
            duration_ = log(abs(coeffScale_ + initialOffset_) / minimumAmplitude_) / -coeffDecay_;
        }
    } else {
        // Overdamped Systems
        if (coeffScale_ != 0 && coeffScaleMinus_ != 0) {
            duration_ = fmax(log(abs(coeffScale_) / minimumAmplitude_ * 2) / -coeffDecay_,
                log(abs(coeffScaleMinus_) / minimumAmplitude_ * 2) / -coeffDecayMinus_);
        }
    }
    ROSEN_LOGD("SAG, %s estimated duration = %.5f, actual duration = %.5f", __func__, duration_,
        std::clamp(duration_, MIN_DURATION, MAX_DURATION));
    duration_ = std::clamp(duration_, MIN_DURATION, MAX_DURATION);
}

void RSValueSpringInterpolator::UpdateParameters(
    float response, float dampingRatio, float initialVelocity, float initialOffset, float minimumAmplitude)
{
    if (response == response_ && dampingRatio == dampingRatio_ && initialVelocity == initialVelocity &&
        initialOffset == initialOffset && isReversed_ == false) {
        return;
    }

    isReversed_ = false;
    response_ = response;
    dampingRatio_ = dampingRatio;
    initialVelocity_ = initialVelocity;
    initialOffset_ = initialOffset;
    minimumAmplitude_ = minimumAmplitude;
    duration_ = -1; // recalculate spring duration

    CalculateSpringParameters();

    ROSEN_LOGD("SAG, %s response = %.2f, dampingRatio = %.2f, initialOffset = %.2f, initialVelocity "
               "= %.2f, minimumAmplitude = %.2f, duration = %.2f",
        __func__, response_, dampingRatio_, initialOffset_, initialVelocity_, minimumAmplitude_, duration_);
}

float RSValueSpringInterpolator::GetInstantaneousVelocity(int64_t microseconds) const
{
    if (microseconds <= 0) {
        return initialVelocity_;
    } else {
        return (InterpolateValue(microseconds + 1) - InterpolateValue(microseconds)) * SECONDS_TO_MICROSECONDS;
    }
}

float RSValueSpringInterpolator::InterpolateValue(int64_t microseconds) const
{
    if (isReversed_) {
        return initialOffset_ - InterpolateImpl(duration_ - microseconds / SECONDS_TO_MICROSECONDS);
    } else {
        return InterpolateImpl(microseconds / SECONDS_TO_MICROSECONDS);
    }
}

int64_t RSValueSpringInterpolator::GetEstimatedDuration() const
{
    return static_cast<int64_t>(duration_ * SECONDS_TO_MICROSECONDS);
}

} // namespace Rosen
} // namespace OHOS
