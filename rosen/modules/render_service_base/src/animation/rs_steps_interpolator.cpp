/*
 * Copyright (c) 2022-2022 Huawei Device Co., Ltd.
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

#include "animation/rs_steps_interpolator.h"

#include <algorithm>
#include <cmath>

#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
RSStepsInterpolator::RSStepsInterpolator(int32_t steps, StepsCurvePosition position)
    : steps_(steps <= 0 ? 1 : steps), position_(position)
{}

RSStepsInterpolator::RSStepsInterpolator(uint64_t id, int32_t steps, StepsCurvePosition position)
    : RSInterpolator(id), steps_(steps <= 0 ? 1 : steps), position_(position)
{}

bool RSStepsInterpolator::Marshalling(Parcel& parcel) const
{
    if (!parcel.WriteUint16(InterpolatorType::STEPS)) {
        ROSEN_LOGE("StepsInterpolator marshalling write type failed.");
        return false;
    }
    if (!parcel.WriteUint64(id_)) {
        ROSEN_LOGE("StepsInterpolator marshalling write id failed.");
        return false;
    }
    if (!(parcel.WriteInt32(steps_) && parcel.WriteInt32(static_cast<int32_t>(position_)))) {
        ROSEN_LOGE("StepsInterpolator marshalling write value failed.");
        return false;
    }
    return true;
}

RSStepsInterpolator* RSStepsInterpolator::Unmarshalling(Parcel& parcel)
{
    uint64_t id = parcel.ReadUint64();
    int32_t steps;
    int32_t position;
    if (!(parcel.ReadInt32(steps) && parcel.ReadInt32(position))) {
        ROSEN_LOGE("StepsInterpolator unmarshalling failed.");
        return nullptr;
    }
    return new RSStepsInterpolator(id, steps, static_cast<StepsCurvePosition>(position));
}

float RSStepsInterpolator::InterpolateImpl(float fraction) const
{
    if (fraction < fractionMin || fraction > fractionMax) {
        ROSEN_LOGE("Fraction is less than 0 or larger than 1, return 1.");
        return fractionMax;
    }
    auto currentStep = static_cast<int32_t>(fraction * steps_);
    if (position_ == StepsCurvePosition::START) {
        currentStep++;
    }
    if (steps_ == 0) {
        ROSEN_LOGE("RSStepsInterpolator::Interpolate, steps number is invalid!");
        return static_cast<float>(currentStep);
    }
    // current step should not greater than the total steps number
    currentStep = std::min(currentStep, steps_);
    return static_cast<float>(currentStep) / steps_;
}
} // namespace Rosen
} // namespace OHOS
