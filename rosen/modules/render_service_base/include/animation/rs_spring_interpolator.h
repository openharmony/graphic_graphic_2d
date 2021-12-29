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

#ifndef ROSEN_ENGINE_CORE_ANIMATION_RS_SPRING_INTERPOLATOR_H
#define ROSEN_ENGINE_CORE_ANIMATION_RS_SPRING_INTERPOLATOR_H

#include "animation/rs_interpolator.h"

namespace OHOS {
namespace Rosen {
class RSSpringInterpolator : public RSInterpolator {
public:
    RSSpringInterpolator(float response, float dampingRatio, float initialVelocity)
        // initialOffset: 1, minimumAmplitude: 0.001
        : RSSpringInterpolator(response, dampingRatio, 1, 0.001, initialVelocity, 0)
    {}

    ~RSSpringInterpolator() override {};

    float Interpolate(float input) override
    {
        return InterpolateImpl(input * duration_);
    }
    bool Marshalling(Parcel& parcel) const override;
#ifdef ROSEN_OHOS
    static RSSpringInterpolator* Unmarshalling(Parcel& parcel);
#endif
protected:
    explicit RSSpringInterpolator(float response, float dampingRatio, float initialOffset, float minimumAmplitude,
        float initialVelocity, float duration);
    void CalculateSpringParameters();
    float InterpolateImpl(double seconds) const;

    float response_;
    float dampingRatio_;
    float initialVelocity_;
    float initialOffset_;
    float minimumAmplitude_;
    float duration_;

private:
    void EstimateDuration();
    double CalculateDisplacement(double mappedTime) const;

    // common intermediate coefficient
    float coeffDecay_;
    float coeffScale_;
    // only for under-damped systems
    float dampedAngularVelocity_;
    // only for over-damped systems
    float coeffScaleMinus_;
    float coeffDecayMinus_;
};

class RSValueSpringInterpolator : public RSSpringInterpolator {
public:
    explicit RSValueSpringInterpolator(float response, float dampingRatio, float initialOffset, float minimumAmplitude)
        // initialVelocity: 0, duration: 0
        : RSSpringInterpolator(response, dampingRatio, initialOffset, minimumAmplitude, 0, 0)
    {}

    ~RSValueSpringInterpolator() override {};

    void UpdateParameters(
        float response, float dampingRatio, float initialVelocity, float initialOffset, float minimumAmplitude);

    float GetInstantaneousVelocity(int64_t microseconds) const;

    float InterpolateValue(int64_t microseconds) const;

    int64_t GetEstimatedDuration() const;

    void SetReversed(bool isReversed)
    {
        isReversed_ = isReversed;
    }

private:
    bool isReversed_ = false;
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_ENGINE_CORE_ANIMATION_RS_SPRING_INTERPOLATOR_H
