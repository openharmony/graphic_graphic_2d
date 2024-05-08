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

#include "animation/rs_animation_timing_curve.h"

#include "animation/rs_cubic_bezier_interpolator.h"
#include "animation/rs_interpolator.h"
#include "animation/rs_spring_interpolator.h"
#include "animation/rs_steps_interpolator.h"
#include "common/rs_common_def.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
const RSAnimationTimingCurve RSAnimationTimingCurve::LINEAR =
    RSAnimationTimingCurve(std::make_shared<LinearInterpolator>());

const RSAnimationTimingCurve RSAnimationTimingCurve::EASE =
    RSAnimationTimingCurve::CreateCubicCurve(0.25f, 0.1f, 0.25f, 1.0f);

const RSAnimationTimingCurve RSAnimationTimingCurve::EASE_IN =
    RSAnimationTimingCurve::CreateCubicCurve(0.42f, 0.0f, 1.0f, 1.0f);

const RSAnimationTimingCurve RSAnimationTimingCurve::EASE_OUT =
    RSAnimationTimingCurve::CreateCubicCurve(0.0f, 0.0f, 0.58f, 1.0f);

const RSAnimationTimingCurve RSAnimationTimingCurve::EASE_IN_OUT =
    RSAnimationTimingCurve::CreateCubicCurve(0.42f, 0.0f, 0.58f, 1.0f);

const RSAnimationTimingCurve RSAnimationTimingCurve::DEFAULT = EASE_IN_OUT;

const RSAnimationTimingCurve RSAnimationTimingCurve::SPRING = RSAnimationTimingCurve::CreateSpring(0.55f, 0.825f, 0.0f);

const RSAnimationTimingCurve RSAnimationTimingCurve::INTERACTIVE_SPRING =
    RSAnimationTimingCurve::CreateSpring(0.15f, 0.86f, 0.25f);

RSAnimationTimingCurve::RSAnimationTimingCurve()
    : RSAnimationTimingCurve(std::make_shared<RSCubicBezierInterpolator>(0.42f, 0.0f, 0.58f, 1.0f))
{}

RSAnimationTimingCurve::RSAnimationTimingCurve(const std::shared_ptr<RSInterpolator>& interpolator)
    : interpolator_(interpolator), customCurveFunc_(nullptr)
{}

RSAnimationTimingCurve::RSAnimationTimingCurve(const std::function<float(float)>& customCurveFunc)
    : interpolator_(nullptr), customCurveFunc_(customCurveFunc)
{}

RSAnimationTimingCurve::RSAnimationTimingCurve(float response, float dampingRatio, float blendDuration)
    : type_(CurveType::SPRING), response_(response), dampingRatio_(dampingRatio), blendDuration_(blendDuration),
      interpolator_(nullptr), customCurveFunc_(nullptr)
{}

RSAnimationTimingCurve::RSAnimationTimingCurve(
    float response, float dampingRatio, float initialVelocity, CurveType curveType, float minimumAmplitudeRatio)
    : type_(curveType), response_(response), dampingRatio_(dampingRatio), initialVelocity_(initialVelocity),
      minimumAmplitudeRatio_(minimumAmplitudeRatio), interpolator_(nullptr), customCurveFunc_(nullptr)
{}

RSAnimationTimingCurve RSAnimationTimingCurve::CreateCustomCurve(const std::function<float(float)>& customCurveFunc)
{
    return RSAnimationTimingCurve(customCurveFunc);
}

RSAnimationTimingCurve RSAnimationTimingCurve::CreateCubicCurve(float ctrlX1, float ctrlY1, float ctrlX2, float ctrlY2)
{
    return RSAnimationTimingCurve(std::make_shared<RSCubicBezierInterpolator>(ctrlX1, ctrlY1, ctrlX2, ctrlY2));
}

RSAnimationTimingCurve RSAnimationTimingCurve::CreateSpringCurve(
    float velocity, float mass, float stiffness, float damping)
{
    if (stiffness <= 0.0f || mass * stiffness <= 0.0f) {
        constexpr float response = 0.55f;
        constexpr float dampingRatio = 0.825f;
        ROSEN_LOGE("RSAnimationTimingCurve::CreateSpringCurve, invalid parameters.");
        return RSAnimationTimingCurve(std::make_shared<RSSpringInterpolator>(response, dampingRatio, velocity));
    }
    float response = 2 * PI * sqrt(mass / stiffness);
    float dampingRatio = (damping / (2 * sqrt(mass * stiffness)));
    return RSAnimationTimingCurve(std::make_shared<RSSpringInterpolator>(response, dampingRatio, velocity));
}

RSAnimationTimingCurve RSAnimationTimingCurve::CreateInterpolatingSpring(
    float mass, float stiffness, float damping, float velocity, float minimumAmplitudeRatio)
{
    if (stiffness <= 0.0f || mass * stiffness <= 0.0f) {
        constexpr float response = 0.55f;
        constexpr float dampingRatio = 0.825f;
        ROSEN_LOGE("RSAnimationTimingCurve::CreateInterpolatingSpring, invalid parameters.");
        return RSAnimationTimingCurve(
            response, dampingRatio, velocity, CurveType::INTERPOLATING_SPRING, minimumAmplitudeRatio);
    }
    float response = 2 * PI * sqrt(mass / stiffness);
    float dampingRatio = (damping / (2 * sqrt(mass * stiffness)));
    return RSAnimationTimingCurve(
        response, dampingRatio, velocity, CurveType::INTERPOLATING_SPRING, minimumAmplitudeRatio);
}

RSAnimationTimingCurve RSAnimationTimingCurve::CreateStepsCurve(int32_t steps, StepsCurvePosition position)
{
    return RSAnimationTimingCurve(std::make_shared<RSStepsInterpolator>(steps, position));
}

RSAnimationTimingCurve RSAnimationTimingCurve::CreateSpring(float response, float dampingRatio, float blendDuration)
{
    return RSAnimationTimingCurve(response, dampingRatio, blendDuration);
}

std::shared_ptr<RSInterpolator> RSAnimationTimingCurve::GetInterpolator(int duration) const
{
    if (customCurveFunc_ != nullptr) {
        return std::make_shared<RSCustomInterpolator>(customCurveFunc_, duration);
    }

    return interpolator_;
}
} // namespace Rosen
} // namespace OHOS
