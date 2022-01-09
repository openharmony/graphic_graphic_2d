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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_CUBIC_BEZIER_INTERPOLATOR_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_CUBIC_BEZIER_INTERPOLATOR_H

#include "animation/rs_interpolator.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
class RSCubicBezierInterpolator : public RSInterpolator {
public:
    explicit RSCubicBezierInterpolator(float ctrx1, float ctry1, float ctrx2, float ctry2)
        : controllx1_(ctrx1), controlly1_(ctry1), controllx2_(ctrx2), controlly2_(ctry2)
    {}
    ~RSCubicBezierInterpolator() = default;

    float Interpolate(float input) const override
    {
        return GetCubicBezierValue(SEARCH_STEP * BinarySearch(input), controlly1_, controlly2_);
    }
#ifdef ROSEN_OHOS
    bool Marshalling(Parcel& parcel) const override
    {
        if (!parcel.WriteUint16(InterpolatorType::CUBIC_BEZIER)) {
            ROSEN_LOGE("CubicBezierInterpolator::Marshalling, write type failed");
            return false;
        }
        if (!(parcel.WriteFloat(controllx1_) && parcel.WriteFloat(controlly1_) && parcel.WriteFloat(controllx2_) &&
                parcel.WriteFloat(controlly2_))) {
            ROSEN_LOGE("CubicBezierInterpolator::Marshalling, write value failed");
            return false;
        }
        return true;
    }
    static RSCubicBezierInterpolator* Unmarshalling(Parcel& parcel)
    {
        float x1 = 0;
        float y1 = 0;
        float x2 = 0;
        float y2 = 0;
        if (!(parcel.ReadFloat(x1) && parcel.ReadFloat(y1) && parcel.ReadFloat(x2) && parcel.ReadFloat(y2))) {
            ROSEN_LOGE("CubicBezierInterpolator::Unmarshalling, read failed");
            return nullptr;
        }
        return new RSCubicBezierInterpolator(x1, y1, x2, y2);
    }
#endif

private:
    float GetCubicBezierValue(const float time, const float ctr1, const float ctr2) const
    {
        return THIRD_RDER * (1.0f - time) * (1.0f - time) * time * ctr1 +
               THIRD_RDER * (1.0f - time) * time * time * ctr2 + time * time * time;
    }

    int BinarySearch(float key) const
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

    constexpr static int MAX_RESOLUTION = 4000;
    constexpr static float SEARCH_STEP = 1.0f / MAX_RESOLUTION;
    constexpr static int THIRD_RDER = 3.0;

    float controllx1_;
    float controlly1_;
    float controllx2_;
    float controlly2_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_CUBIC_BEZIER_INTERPOLATOR_H
