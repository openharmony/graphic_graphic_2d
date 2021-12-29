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

#include "animation/rs_animation_timing_curve.h"

#include <memory>

#include "animation/rs_cubic_bezier_interpolator.h"
#include "animation/rs_interpolator.h"
#include "animation/rs_spring_interpolator.h"

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

RSAnimationTimingCurve::RSAnimationTimingCurve()
    : RSAnimationTimingCurve(std::make_shared<RSCubicBezierInterpolator>(0.42f, 0.0f, 0.58f, 1.0f))
{}

RSAnimationTimingCurve::RSAnimationTimingCurve(const std::shared_ptr<RSInterpolator>& interpolator)
    : interpolator_(interpolator), customCurveFunc_(nullptr)
{}

RSAnimationTimingCurve::RSAnimationTimingCurve(const std::function<float(float)>& customCurveFunc)
    : interpolator_(nullptr), customCurveFunc_(customCurveFunc)
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
    float response = 1.0;
    float dampingRatio = (damping / (2 * sqrt(mass * stiffness)));
    return RSAnimationTimingCurve(std::make_shared<RSSpringInterpolator>(response, dampingRatio, velocity));
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
