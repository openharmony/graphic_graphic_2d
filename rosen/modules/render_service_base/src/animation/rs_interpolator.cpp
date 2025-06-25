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

#include "animation/rs_interpolator.h"

#include <algorithm>
#include <cmath>

#include "sandbox_utils.h"

#include "animation/rs_animation_common.h"
#include "animation/rs_cubic_bezier_interpolator.h"
#include "animation/rs_spring_interpolator.h"
#include "animation/rs_steps_interpolator.h"
#include "platform/common/rs_log.h"
#include "transaction/rs_marshalling_helper.h"

namespace OHOS {
namespace Rosen {

static pid_t pid_ = GetRealPid();

const std::shared_ptr<RSInterpolator> RSInterpolator::DEFAULT =
    std::make_shared<RSCubicBezierInterpolator>(0.42f, 0.0f, 0.58f, 1.0f);

RSInterpolator::RSInterpolator() : id_(GenerateId()) {}

void RSInterpolator::Init()
{
    pid_ = GetRealPid();
}

uint64_t RSInterpolator::GenerateId()
{
    if (pid_ == 0) {
        pid_ = GetRealPid();
    }
    static std::atomic<uint32_t> currentId_ = 0;

    auto currentId = currentId_.fetch_add(1, std::memory_order_relaxed);
    if (currentId == UINT32_MAX) {
        ROSEN_LOGE("RSInterpolator Id overflow");
    }

    // concat two 32-bit numbers to one 64-bit number
    return ((AnimationId)pid_ << 32) | (currentId);
}

float RSInterpolator::Interpolate(float input)
{
    if (input == prevInput_) {
        return prevOutput_;
    }
    prevInput_ = input;
    prevOutput_ = InterpolateImpl(input);
    return prevOutput_;
}

RSCustomInterpolator::RSCustomInterpolator(
    uint64_t id, const std::vector<float>&& times, const std::vector<float>&& values)
    : RSInterpolator(id), times_(times), values_(values)
{}

RSCustomInterpolator::RSCustomInterpolator(const std::function<float(float)>& func, int duration)
    : interpolateFunc_(func)
{
    Convert(duration);
}

void RSCustomInterpolator::Convert(int duration)
{
    if (interpolateFunc_ == nullptr) {
        ROSEN_LOGE("RSCustomInterpolator::Convert, interpolateFunc_ is nullptr");
        return;
    }
    constexpr uint64_t frameInterval = 16666667;
    auto numAnim = static_cast<size_t>(std::ceil(static_cast<double>(duration * MS_TO_NS) / frameInterval));
    numAnim = std::clamp(numAnim, MIN_SAMPLE_POINTS, MAX_SAMPLE_POINTS);
    float lastAnimFrame = numAnim - 1;
    if (lastAnimFrame <= 0.0f) {
        ROSEN_LOGE("RSCustomInterpolator::Convert, lastAnimFrame is invalid.");
        return;
    }
    for (size_t i = 0; i < numAnim; i++) {
        float time = static_cast<float>(i) / lastAnimFrame;
        float value = interpolateFunc_(time);
        times_.push_back(time);
        values_.push_back(value);
    }
}

float RSCustomInterpolator::InterpolateImpl(float input) const
{
    if (times_.size() <= 0) {
        ROSEN_LOGE("RSCustomInterpolator::InterpolateImpl, times_ vector is empty.");
        return 0.0f;
    }
    if (times_.size() != values_.size()) {
        ROSEN_LOGE("RSCustomInterpolator::InterpolateImpl, times_ and values_ have different sizes.");
        return 0.0f;
    }
    auto firstGreatValueIterator = std::upper_bound(times_.begin(), times_.end(), input);
    if (firstGreatValueIterator == times_.end()) {
        return values_.back();
    }
    if (firstGreatValueIterator == times_.begin()) {
        return values_.front();
    }
    auto endLocation = std::distance(times_.begin(), firstGreatValueIterator);
    int startLocation = endLocation - 1;
    float number = times_[endLocation] - times_[startLocation];
    if (ROSEN_LE(number, 0.f)) {
        ROSEN_LOGE("RSCustomInterpolator::Interpolate, time between startLocation and endLocation is less than zero.");
        return 0.0f;
    }
    float fraction = (input - times_[startLocation]) / number;
    float ret = fraction * (values_[endLocation] - values_[startLocation]) + values_[startLocation];
    return ret;
}
} // namespace Rosen
} // namespace OHOS
