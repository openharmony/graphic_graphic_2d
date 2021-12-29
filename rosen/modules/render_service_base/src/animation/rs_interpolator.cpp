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

#include "animation/rs_interpolator.h"

#include <algorithm>
#include <cmath>

#include "animation/rs_animation_common.h"
#include "animation/rs_cubic_bezier_interpolator.h"
#include "animation/rs_spring_interpolator.h"

namespace OHOS {
namespace Rosen {
const std::shared_ptr<RSInterpolator> RSInterpolator::DEFAULT =
    std::make_shared<RSCubicBezierInterpolator>(0.42f, 0.0f, 0.58f, 1.0f);

#ifdef ROSEN_OHOS
RSInterpolator* RSInterpolator::Unmarshalling(Parcel& parcel)
{
    uint16_t interpolatorType = parcel.ReadUint16();
    RSInterpolator* ret = nullptr;
    switch (interpolatorType) {
        case InterpolatorType::LINEAR:
            ret = new LinearInterpolator();
            break;
        case InterpolatorType::CUSTOM:
            ret = RSCustomInterpolator::Unmarshalling(parcel);
            break;
        case InterpolatorType::CUBIC_BEZIER:
            ret = RSCubicBezierInterpolator::Unmarshalling(parcel);
            break;
        case InterpolatorType::SPRING:
            ret = RSSpringInterpolator::Unmarshalling(parcel);
            break;
        default:
            break;
    }
    return ret;
}
RSCustomInterpolator* RSCustomInterpolator::Unmarshalling(Parcel& parcel)
{
    std::vector<float> times, values;
    if (!(parcel.ReadFloatVector(&times) && parcel.ReadFloatVector(&values))) {
        ROSEN_LOGE("Unmarshalling CustomInterpolator failed");
        return nullptr;
    }
    return new RSCustomInterpolator(std::move(times), std::move(values));
}

#endif

RSCustomInterpolator::RSCustomInterpolator(const std::vector<float>&& times, const std::vector<float>&& values)
    : times_(times), values_(values)
{}

RSCustomInterpolator::RSCustomInterpolator(const std::function<float(float)>& func, int duration)
    : interpolateFunc_(func)
{
    Convert(duration);
}

void RSCustomInterpolator::Convert(int duration)
{
    if (interpolateFunc_ == nullptr) {
        return;
    }
    uint64_t frameInterval = 16666667; // TODO need to get from the client
    int numAnim = static_cast<int>(std::ceil(static_cast<double>(duration * MS_TO_NS) / frameInterval));
    const int maxSamplePoints = 300;
    const int minSamplePoints = 2;
    numAnim = std::min(std::max(minSamplePoints, numAnim), maxSamplePoints);
    float lastAnimFrame = numAnim - 1;
    for (int i = 0; i < numAnim; i++) {
        float time = i / lastAnimFrame;
        float value = interpolateFunc_(time);
        times_.push_back(time);
        values_.push_back(value);
    }
}

float RSCustomInterpolator::Interpolate(float input)
{
    if (input < times_[0] + EPSILON) {
        return times_[0];
    }
    if (input > times_[times_.size() - 1] - EPSILON) {
        return times_[times_.size() - 1];
    }
    auto firstGreatValue = upper_bound(times_.begin(), times_.end(), input);
    int endLocation = firstGreatValue - times_.begin();
    int startLocation = endLocation - 1;

    float fraction = (input - times_[startLocation]) / (times_[endLocation] - times_[startLocation]);
    float ret = fraction * (values_[endLocation] - values_[startLocation]) + values_[startLocation];
    return ret;
}
} // namespace Rosen
} // namespace OHOS
