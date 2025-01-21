/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "animation/rs_cubic_bezier_interpolator.h"

namespace OHOS {
namespace Rosen {
namespace {
inline float GetCubicBezierValue(const float time, const float ctl1, const float ctl2)
{
    if (time < 0.0f) {
        return 0.0f;
    }
    if (time > 1.0f) {
        return 1.0f;
    }
    constexpr static int three = 3.0;
    const float oneMinusTime = 1.0f - time;
    return three * oneMinusTime * oneMinusTime * time * ctl1 + three * oneMinusTime * time * time * ctl2 +
           time * time * time;
}
} // namespace

RSCubicBezierInterpolator::RSCubicBezierInterpolator(float ctlX1, float ctlY1, float ctlX2, float ctlY2)
    : controlX1_(ctlX1), controlY1_(ctlY1), controlX2_(ctlX2), controlY2_(ctlY2)
{}

RSCubicBezierInterpolator::RSCubicBezierInterpolator(uint64_t id, float ctlX1, float ctlY1, float ctlX2, float ctlY2)
    : RSInterpolator(id), controlX1_(ctlX1), controlY1_(ctlY1), controlX2_(ctlX2), controlY2_(ctlY2)
{}

float RSCubicBezierInterpolator::InterpolateImpl(float input) const
{
    constexpr float ONE = 1.0f;
    if (ROSEN_EQ(input, ONE, 1e-6f)) {
        return ONE;
    }
    return GetCubicBezierValue(SEARCH_STEP * BinarySearch(input), controlY1_, controlY2_);
}

int RSCubicBezierInterpolator::BinarySearch(float key) const
{
    int low = 0;
    int high = MAX_RESOLUTION;
    int middle = 0;
    float approximation = 0.0;
    constexpr float epsilon = 1e-6f;
    while (low <= high) {
        middle = (static_cast<unsigned int>(low + high)) >> 1;
        approximation = GetCubicBezierValue(SEARCH_STEP * middle, controlX1_, controlX2_);
        if (ROSEN_EQ(approximation, key, epsilon)) {
            // Early exit if the key is found within an acceptable error range
            return middle;
        } else if (approximation < key) {
            low = middle + 1;
        } else {
            high = middle - 1;
        }
    }
    return low;
}
} // namespace Rosen
} // namespace OHOS
