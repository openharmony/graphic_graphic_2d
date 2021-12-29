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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ANIMATION_TIMING_CURVE_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ANIMATION_TIMING_CURVE_H

#include <functional>
#include <map>

#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {
class RSInterpolator;

class RS_EXPORT RSAnimationTimingCurve final {
public:
    static const RSAnimationTimingCurve DEFAULT;

    static const RSAnimationTimingCurve LINEAR;

    static const RSAnimationTimingCurve EASE;

    static const RSAnimationTimingCurve EASE_IN;

    static const RSAnimationTimingCurve EASE_OUT;

    static const RSAnimationTimingCurve EASE_IN_OUT;

    static RSAnimationTimingCurve CreateCustomCurve(const std::function<float(float)>& customCurveFunc);

    static RSAnimationTimingCurve CreateCubicCurve(float ctrlX1, float ctrlY1, float ctrlX2, float ctrlY2);

    static RSAnimationTimingCurve CreateSpringCurve(float velocity, float mass, float stiffness, float damping);

    RSAnimationTimingCurve();
    RSAnimationTimingCurve(const RSAnimationTimingCurve& timingCurve) = default;
    RSAnimationTimingCurve& operator=(const RSAnimationTimingCurve& timingCurve) = default;
    virtual ~RSAnimationTimingCurve() = default;

private:
    RSAnimationTimingCurve(const std::shared_ptr<RSInterpolator>& interpolator);
    RSAnimationTimingCurve(const std::function<float(float)>& customCurveFunc);

    std::shared_ptr<RSInterpolator> GetInterpolator(int duration) const;

    std::shared_ptr<RSInterpolator> interpolator_;
    std::function<float(float)> customCurveFunc_;

    template<typename T>
    friend class RSCurveAnimation;
    template<typename T>
    friend class RSKeyframeAnimation;
    friend class RSPathAnimation;
    friend class RSTransition;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ANIMATION_TIMING_CURVE_H
