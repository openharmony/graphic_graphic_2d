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
#include <mutex>
#include <parcel.h>
#include <refbase.h>
#include <unordered_map>
#include <vector>

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

class RSB_EXPORT RSInterpolator : public Parcelable {
public:
    static RSB_EXPORT const std::shared_ptr<RSInterpolator> DEFAULT;
    ~RSInterpolator() override = default;

    bool Marshalling(Parcel& parcel) const override = 0;
    [[nodiscard]] static RSB_EXPORT std::shared_ptr<RSInterpolator> Unmarshalling(Parcel& parcel);

    float Interpolate(float input);

protected:
    RSInterpolator();
    RSInterpolator(uint64_t id) : id_(id) {};
    uint64_t id_;

private:
    virtual float InterpolateImpl(float input) const = 0;
    static uint64_t GenerateId();
    float prevInput_ { -1.0f };
    float prevOutput_ { -1.0f };
};

class RSB_EXPORT LinearInterpolator : public RSInterpolator {
public:
    LinearInterpolator() = default;
    ~LinearInterpolator() override = default;

    bool Marshalling(Parcel& parcel) const override;
    [[nodiscard]] static LinearInterpolator* Unmarshalling(Parcel& parcel);

private:
    LinearInterpolator(uint64_t id) : RSInterpolator(id) {}
    float InterpolateImpl(float input) const override
    {
        return input;
    }
};

class RSB_EXPORT RSCustomInterpolator : public RSInterpolator {
public:
    RSCustomInterpolator(const std::function<float(float)>& func, int duration);
    ~RSCustomInterpolator() override = default;

    bool Marshalling(Parcel& parcel) const override;
    [[nodiscard]] static RSCustomInterpolator* Unmarshalling(Parcel& parcel);

private:
    RSCustomInterpolator(uint64_t id, const std::vector<float>&& times, const std::vector<float>&& values);
    float InterpolateImpl(float input) const override;
    void Convert(int duration);

    std::vector<float> times_;
    std::vector<float> values_;
    std::function<float(float)> interpolateFunc_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_INTERPOLATOR_H
