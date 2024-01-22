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

#include "animation/rs_cubic_bezier_interpolator.h"

namespace OHOS {
namespace Rosen {
RSCubicBezierInterpolator::RSCubicBezierInterpolator(float ctrx1, float ctry1, float ctrx2, float ctry2)
    : controllx1_(ctrx1), controlly1_(ctry1), controllx2_(ctrx2), controlly2_(ctry2)
{}

RSCubicBezierInterpolator::RSCubicBezierInterpolator(uint64_t id, float ctrx1, float ctry1, float ctrx2, float ctry2)
    : RSInterpolator(id), controllx1_(ctrx1), controlly1_(ctry1), controllx2_(ctrx2), controlly2_(ctry2)
{}

float RSCubicBezierInterpolator ::InterpolateImpl(float input) const
{
    constexpr float ONE = 1.0f;
    if (ROSEN_EQ(input, ONE, 1e-6f)) {
        return ONE;
    }
    return GetCubicBezierValue(SEARCH_STEP * BinarySearch(input), controlly1_, controlly2_);
}

bool RSCubicBezierInterpolator::Marshalling(Parcel& parcel) const
{
    if (!parcel.WriteUint16(InterpolatorType::CUBIC_BEZIER)) {
        return false;
    }
    if (!parcel.WriteUint64(id_)) {
        return false;
    }
    if (!(parcel.WriteFloat(controllx1_) && parcel.WriteFloat(controlly1_) && parcel.WriteFloat(controllx2_) &&
            parcel.WriteFloat(controlly2_))) {
        return false;
    }
    return true;
}
RSCubicBezierInterpolator* RSCubicBezierInterpolator::Unmarshalling(Parcel& parcel)
{
    auto id = parcel.ReadUint64();
    if (id == 0) {
        return nullptr;
    }
    float x1 = 0;
    float y1 = 0;
    float x2 = 0;
    float y2 = 0;
    if (!(parcel.ReadFloat(x1) && parcel.ReadFloat(y1) && parcel.ReadFloat(x2) && parcel.ReadFloat(y2))) {
        return nullptr;
    }
    return new RSCubicBezierInterpolator(id, x1, y1, x2, y2);
}

float RSCubicBezierInterpolator::GetCubicBezierValue(const float time, const float ctr1, const float ctr2) const
{
    return THIRD_RDER * (1.0f - time) * (1.0f - time) * time * ctr1 + THIRD_RDER * (1.0f - time) * time * time * ctr2 +
           time * time * time;
}

int RSCubicBezierInterpolator::BinarySearch(float key) const
{
    int low = 0;
    int high = MAX_RESOLUTION;
    int middle;
    float approximation;
    while (low <= high) {
        middle = (low + high) / 2;
        approximation = GetCubicBezierValue(SEARCH_STEP * middle, controllx1_, controllx2_);
        if (approximation < key) {
            low = middle + 1;
        } else {
            high = middle - 1;
        }
        if (fabs(approximation - key) <= 1e-6) {
            return middle;
        }
    }
    return low;
}
} // namespace Rosen
} // namespace OHOS
