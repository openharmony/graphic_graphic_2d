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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_VALUE_ESTIMATOR_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_VALUE_ESTIMATOR_H

#include <memory>

#include "animation/rs_animation_common.h"
#include "animation/rs_interpolator.h"
#include "common/rs_color.h"
#include "common/rs_matrix3.h"
#include "common/rs_vector2.h"
#include "common/rs_vector4.h"
#include "render/rs_filter.h"

namespace OHOS {
namespace Rosen {
class RSValueEstimator {
public:
    template<typename T>
    static T Estimate(float fraction, const T& startValue, const T& endValue)
    {
        return startValue * (1.0f - fraction) + endValue * fraction;
    }

    static Quaternion Estimate(float fraction, const Quaternion& startValue, const Quaternion& endValue);

    static std::shared_ptr<RSFilter> Estimate(
        float fraction, const std::shared_ptr<RSFilter>& startValue, const std::shared_ptr<RSFilter>& endValue);

    template<typename T>
    static float EstimateFraction(
        const std::shared_ptr<RSInterpolator>& interpolator, const T& value, const T& startValue, const T& endValue)
    {
        float start = FRACTION_MIN;
        float end = FRACTION_MAX;
        auto byValue = endValue - startValue;
        while (end > start + EPSILON) {
            float mid = (start + end) / 2.0f;
            float fraction = interpolator->Interpolate(mid);
            auto interpolationValue = Estimate(fraction, startValue, endValue);
            if (value < interpolationValue) {
                (byValue > 0) ? (end = mid) : (start = mid);
            } else {
                (byValue > 0) ? (start = mid) : (end = mid);
            }

            if (std::abs(value - interpolationValue) <= EPSILON) {
                return mid;
            }
        }

        return FRACTION_MIN;
    }

    static float EstimateFraction(const std::shared_ptr<RSInterpolator>& interpolator, const Vector2f& value,
        const Vector2f& startValue, const Vector2f& endValue);

    static float EstimateFraction(const std::shared_ptr<RSInterpolator>& interpolator, const Vector4f& value,
        const Vector4f& startValue, const Vector4f& endValue);

    static float EstimateFraction(const std::shared_ptr<RSInterpolator>& interpolator, const Quaternion& value,
        const Quaternion& startValue, const Quaternion& endValue);

    static float EstimateFraction(const std::shared_ptr<RSInterpolator>& interpolator, const RSColor& value,
        const RSColor& startValue, const RSColor& endValue);

    static float EstimateFraction(const std::shared_ptr<RSInterpolator>& interpolator, const Matrix3f& value,
        const Matrix3f& startValue, const Matrix3f& endValue);

    static float EstimateFraction(const std::shared_ptr<RSInterpolator>& interpolator,
        const std::shared_ptr<RSFilter>& value, const std::shared_ptr<RSFilter>& startValue,
        const std::shared_ptr<RSFilter>& endValue);
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_VALUE_ESTIMATOR_H
