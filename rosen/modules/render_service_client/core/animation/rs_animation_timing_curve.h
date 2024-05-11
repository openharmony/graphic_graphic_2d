/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ANIMATION_TIMING_CURVE_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ANIMATION_TIMING_CURVE_H

#include <functional>
#include <map>
#include <memory>

#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {
class RSInterpolator;
enum class StepsCurvePosition;

namespace {
// minimumAmplitudeRatio_ is used for interpointSpring to determine the ending accuracy of spring animation.
// the smaller the minimumAmplitudeRatio_, the closer it is to the endpoint at the end of the animation,
// and the longer the animation duration.
constexpr float DEFAULT_AMPLITUDE_RATIO = 0.00025f;
} // namespace

class RSC_EXPORT RSAnimationTimingCurve final {
public:
    static const RSAnimationTimingCurve DEFAULT;
    static const RSAnimationTimingCurve LINEAR;
    static const RSAnimationTimingCurve EASE;
    static const RSAnimationTimingCurve EASE_IN;
    static const RSAnimationTimingCurve EASE_OUT;
    static const RSAnimationTimingCurve EASE_IN_OUT;
    static const RSAnimationTimingCurve SPRING;
    static const RSAnimationTimingCurve INTERACTIVE_SPRING;

    static RSAnimationTimingCurve CreateCustomCurve(const std::function<float(float)>& customCurveFunc);
    static RSAnimationTimingCurve CreateCubicCurve(float ctrlX1, float ctrlY1, float ctrlX2, float ctrlY2);
    static RSAnimationTimingCurve CreateStepsCurve(int32_t steps, StepsCurvePosition position);
    // Create interpolating spring, which duration is determined by TimingProtocol. Multiple animations on the same
    // property will run simultaneously and act additively.
    static RSAnimationTimingCurve CreateSpringCurve(float velocity, float mass, float stiffness, float damping);
    // Create interpolating spring, which duration is determined by the spring model. Multiple animations on the same
    // property will run simultaneously and act additively.
    static RSAnimationTimingCurve CreateInterpolatingSpring(float mass, float stiffness, float damping, float velocity,
        float minimumAmplitudeRatio = DEFAULT_AMPLITUDE_RATIO);
    // Create physical spring, which duration is determined by the spring model. When mixed with other physical spring
    // animations on the same property, each animation will be replaced by their successor, preserving velocity from one
    // animation to the next.
    static RSAnimationTimingCurve CreateSpring(float response, float dampingRatio, float blendDuration = 0.0f);

    RSAnimationTimingCurve();
    RSAnimationTimingCurve(const RSAnimationTimingCurve& timingCurve) = default;
    RSAnimationTimingCurve& operator=(const RSAnimationTimingCurve& timingCurve) = default;
    virtual ~RSAnimationTimingCurve() = default;

    enum class CurveType { INTERPOLATING, SPRING, INTERPOLATING_SPRING };
    CurveType type_ { CurveType::INTERPOLATING };

private:
    RSAnimationTimingCurve(const std::shared_ptr<RSInterpolator>& interpolator);
    RSAnimationTimingCurve(const std::function<float(float)>& customCurveFunc);
    RSAnimationTimingCurve(float response, float dampingRatio, float blendDuration);
    RSAnimationTimingCurve(
        float response, float dampingRatio, float initialVelocity, CurveType curveType, float minimumAmplitudeRatio);

    float response_ { 0.0f };
    float dampingRatio_ { 0.0f };
    float blendDuration_ { 0.0f };
    float initialVelocity_ { 0.0f };
    float minimumAmplitudeRatio_ { DEFAULT_AMPLITUDE_RATIO };

    std::shared_ptr<RSInterpolator> GetInterpolator(int duration) const;

    std::shared_ptr<RSInterpolator> interpolator_;
    std::function<float(float)> customCurveFunc_;

    friend class RSCurveAnimation;
    friend class RSInterpolatingSpringAnimation;
    friend class RSKeyframeAnimation;
    friend class RSSpringAnimation;
    friend class RSPathAnimation;
    friend class RSTransition;
    friend class ParticleParams;
    template<typename T>
    friend class Change;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ANIMATION_TIMING_CURVE_H
