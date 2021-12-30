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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_INTERPOLATOR_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_INTERPOLATOR_H

#include <cmath>
#include <functional>
#include <memory>
#include <vector>

#ifdef ROSEN_OHOS
#include <parcel.h>
#include <refbase.h>
#endif

namespace OHOS {
namespace Rosen {

enum InterpolatorType : uint16_t {
    LINEAR = 1,
    CUSTOM,
    CUBIC_BEZIER,
    SPRING,
};

#ifdef ROSEN_OHOS
class RSInterpolator : public Parcelable {
#else
class RSInterpolator {
#endif
public:
    static const std::shared_ptr<RSInterpolator> DEFAULT;
    RSInterpolator() = default;
    virtual ~RSInterpolator() = default;

#ifdef ROSEN_OHOS
    static RSInterpolator* Unmarshalling(Parcel& parcel);
    virtual bool Marshalling(Parcel& parcel) const override = 0;
#endif

    virtual float Interpolate(float input) = 0;
};

class LinearInterpolator : public RSInterpolator {
public:
    LinearInterpolator() = default;
    virtual ~LinearInterpolator() = default;

#ifdef ROSEN_OHOS
    bool Marshalling(Parcel& parcel) const override
    {
        if (!parcel.WriteUint16(InterpolatorType::LINEAR)) {
            return false;
        }
        return true;
    }
#endif

    float Interpolate(float input) override
    {
        return input;
    }
};

class RSCustomInterpolator : public RSInterpolator {
public:
    RSCustomInterpolator(const std::function<float(float)>& func, int duration);
    virtual ~RSCustomInterpolator() = default;

    float Interpolate(float input) override;

#ifdef ROSEN_OHOS
    bool Marshalling(Parcel& parcel) const override
    {
        if (!(parcel.WriteUint16(InterpolatorType::CUSTOM) && parcel.WriteFloatVector(times_) &&
                parcel.WriteFloatVector(values_))) {
            return false;
        }
        return true;
    }
    static RSCustomInterpolator* Unmarshalling(Parcel& parcel);
#endif

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
