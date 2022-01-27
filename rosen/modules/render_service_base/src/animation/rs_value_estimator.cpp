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

#include "animation/rs_value_estimator.h"

#include "common/rs_common_def.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
Quaternion RSValueEstimator::Estimate(float fraction, const Quaternion& startValue, const Quaternion& endValue)
{
    auto value = startValue;
    return value.Slerp(endValue, fraction);
}

std::shared_ptr<RSFilter> RSValueEstimator::Estimate(
    float fraction, const std::shared_ptr<RSFilter>& startValue, const std::shared_ptr<RSFilter>& endValue)
{
    if ((startValue == nullptr || !startValue->IsAnimatable()) && (endValue == nullptr || !endValue->IsAnimatable())) {
        return endValue;
    }

    if (startValue == nullptr || !startValue->IsAnimatable()) {
        return endValue * fraction;
    }

    if (endValue == nullptr || !endValue->IsAnimatable()) {
        return (fraction < 0.5f) ? startValue * (1.0f - fraction * 2) : endValue;
    }

    if (startValue->GetFilterAnimType() == endValue->GetFilterAnimType()) {
        return startValue * (1.0f - fraction) + endValue * fraction;
    } else {
        return (fraction < 0.5f) ? startValue * (1.0f - fraction * 2) : endValue * (fraction * 2 - 1.0f);
    }
}

float RSValueEstimator::EstimateFraction(const std::shared_ptr<RSInterpolator>& interpolator, const Vector2f& value,
    const Vector2f& startValue, const Vector2f& endValue)
{
    return 0.0f;
}

float RSValueEstimator::EstimateFraction(const std::shared_ptr<RSInterpolator>& interpolator, const Vector4f& value,
    const Vector4f& startValue, const Vector4f& endValue)
{
    return 0.0f;
}

float RSValueEstimator::EstimateFraction(const std::shared_ptr<RSInterpolator>& interpolator, const Quaternion& value,
    const Quaternion& startValue, const Quaternion& endValue)
{
    return 0.0f;
}

float RSValueEstimator::EstimateFraction(const std::shared_ptr<RSInterpolator>& interpolator, const RSColor& value,
    const RSColor& startValue, const RSColor& endValue)
{
    return 0.0f;
}

float RSValueEstimator::EstimateFraction(const std::shared_ptr<RSInterpolator>& interpolator, const Matrix3f& value,
    const Matrix3f& startValue, const Matrix3f& endValue)
{
    return 0.0f;
}

float RSValueEstimator::EstimateFraction(const std::shared_ptr<RSInterpolator>& interpolator,
    const std::shared_ptr<RSFilter>& value, const std::shared_ptr<RSFilter>& startValue,
    const std::shared_ptr<RSFilter>& endValue)
{
    return 0.0f;
}
} // namespace Rosen
} // namespace OHOS
