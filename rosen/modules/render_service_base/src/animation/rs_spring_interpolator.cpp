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

#include "animation/rs_spring_interpolator.h"

#include <algorithm>
#include <cmath>

#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {

RSSpringInterpolator::RSSpringInterpolator(float response, float dampingRatio, float initialVelocity)
    // initialOffset: 1, minimumAmplitude: 0.0001
    : RSSpringModel<float>(response, dampingRatio, -1, initialVelocity, 0.0001)
{
    estimatedDuration_ = EstimateDuration();
}

RSSpringInterpolator::RSSpringInterpolator(uint64_t id, float response, float dampingRatio, float initialVelocity)
    // initialOffset: 1, minimumAmplitude: 0.0001
    : RSSpringModel<float>(response, dampingRatio, -1, initialVelocity, 0.0001), RSInterpolator(id)
{
    estimatedDuration_ = EstimateDuration();
}

float RSSpringInterpolator::InterpolateImpl(float fraction) const
{
    if (fraction <= 0) {
        return 0;
    } else if (fraction >= 1.0f) {
        return 1.0f;
    }
    // map [0, 1] to [0, duration], and calculate the output value
    double mappedTime = fraction * estimatedDuration_;
    return 1.0f + CalculateDisplacement(mappedTime);
}
} // namespace Rosen
} // namespace OHOS
