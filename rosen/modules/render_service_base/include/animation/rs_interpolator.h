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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_INTERPOLATOR_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_INTERPOLATOR_H

#include <cmath>
#include <functional>
#include <memory>
#include <vector>

#include "common/rs_macros.h"

#include <parcel.h>
#include <refbase.h>

#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {

enum InterpolatorType : uint16_t {
    LINEAR = 1,
    CUSTOM,
    CUBIC_BEZIER,
    SPRING,
    STEPS,
};

class RSInterpolator : public Parcelable {
public:
    static RSB_EXPORT const std::shared_ptr<RSInterpolator> DEFAULT;
    RSInterpolator() = default;
    virtual ~RSInterpolator() = default;

    virtual bool Marshalling(Parcel& parcel) const override = 0;
    static RSB_EXPORT RSInterpolator* Unmarshalling(Parcel& parcel);

    virtual float Interpolate(float input) const = 0;
};

class LinearInterpolator : public RSInterpolator {
public:
    LinearInterpolator() = default;
    virtual ~LinearInterpolator() = default;

    bool Marshalling(Parcel& parcel) const override
    {
        if (!parcel.WriteUint16(InterpolatorType::LINEAR)) {
            return false;
        }
        return true;
    }

    float Interpolate(float input) const override
    {
        return input;
    }
};

class RSB_EXPORT RSCustomInterpolator : public RSInterpolator {
public:
    RSCustomInterpolator(const std::function<float(float)>& func, int duration);
    virtual ~RSCustomInterpolator() = default;

    float Interpolate(float input) const override;

    bool Marshalling(Parcel& parcel) const override
    {
        if (!(parcel.WriteUint16(InterpolatorType::CUSTOM) && parcel.WriteFloatVector(times_) &&
                parcel.WriteFloatVector(values_))) {
            return false;
        }
        return true;
    }
    static RSCustomInterpolator* Unmarshalling(Parcel& parcel);

private:
    RSCustomInterpolator(const std::vector<float>&& times, const std::vector<float>&& values);
    void Convert(int duration);

    std::vector<float> times_;
    std::vector<float> values_;
    std::function<float(float)> interpolateFunc_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_INTERPOLATOR_H
