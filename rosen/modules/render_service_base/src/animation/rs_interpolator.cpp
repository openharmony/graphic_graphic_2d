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

namespace OHOS {
namespace Rosen {
const std::shared_ptr<RSInterpolator> RSInterpolator::DEFAULT =
    std::make_shared<RSCubicBezierInterpolator>(0.42f, 0.0f, 0.58f, 1.0f);

RSInterpolator::RSInterpolator() : id_(GenerateId()) {}

uint64_t RSInterpolator::GenerateId()
{
    static pid_t pid_ = GetRealPid();
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

std::shared_ptr<RSInterpolator> RSInterpolator::Unmarshalling(Parcel& parcel)
{
    uint16_t interpolatorType = parcel.ReadUint16();
    RSInterpolator* ret = nullptr;
    switch (interpolatorType) {
        case InterpolatorType::LINEAR:
            ret = LinearInterpolator::Unmarshalling(parcel);
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
        case InterpolatorType::STEPS:
            ret = RSStepsInterpolator::Unmarshalling(parcel);
            break;
        default:
            break;
    }
    if (ret == nullptr) {
        return nullptr;
    }

    static std::mutex cachedInterpolatorsMutex_;
    static std::unordered_map<uint32_t, std::weak_ptr<RSInterpolator>> cachedInterpolators_;
    static const auto Destructor = [](RSInterpolator* ptr) {
        if (ptr == nullptr) {
            ROSEN_LOGE("RSInterpolator::Unmarshalling, sharePtr is nullptr.");
            return;
        }
        std::unique_lock<std::mutex> lock(cachedInterpolatorsMutex_);
        cachedInterpolators_.erase(ptr->id_); // Unregister interpolator from cache before destruction.
        delete ptr;
    };

    {
        // All cache operations should be performed under lock to prevent race conditions
        std::unique_lock<std::mutex> lock(cachedInterpolatorsMutex_);
        // Check if there is an existing valid interpolator in the cache, return it if found.
        if (auto it = cachedInterpolators_.find(ret->id_); it != cachedInterpolators_.end()) {
            if (auto sharedPtr = it->second.lock()) { // Check if weak pointer is valid
                delete ret;                           // Destroy the newly created object
                return sharedPtr;
            } else { // If the weak pointer has expired, erase it from the cache. This should never happen.
                ROSEN_LOGE("RSInterpolator::Unmarshalling, cached weak pointer expired.");
                cachedInterpolators_.erase(it);
            }
        }
        // No existing interpolator in the cache, create a new one and register it in the cache.
        std::shared_ptr<RSInterpolator> sharedPtr(ret, Destructor);
        cachedInterpolators_.emplace(ret->id_, sharedPtr);
        return sharedPtr;
    }
}

bool LinearInterpolator::Marshalling(Parcel& parcel) const
{
    if (!parcel.WriteUint16(InterpolatorType::LINEAR)) {
        return false;
    }
    if (!parcel.WriteUint64(id_)) {
        return false;
    }
    return true;
}

LinearInterpolator* LinearInterpolator::Unmarshalling(Parcel& parcel)
{
    uint64_t id = parcel.ReadUint64();
    if (id == 0) {
        ROSEN_LOGE("Unmarshalling LinearInterpolator failed");
        return nullptr;
    }
    return new LinearInterpolator(id);
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

bool RSCustomInterpolator::Marshalling(Parcel& parcel) const
{
    if (!parcel.WriteUint16(InterpolatorType::CUSTOM)) {
        ROSEN_LOGE("RSCustomInterpolator::Marshalling, Write type failed");
    }
    if (!parcel.WriteUint64(id_)) {
        ROSEN_LOGE("RSCustomInterpolator::Marshalling, Write id failed");
        return false;
    }
    if (!parcel.WriteFloatVector(times_) || !parcel.WriteFloatVector(values_)) {
        ROSEN_LOGE("RSCustomInterpolator::Marshalling, Write value failed");
        return false;
    }
    return true;
}

RSCustomInterpolator* RSCustomInterpolator::Unmarshalling(Parcel& parcel)
{
    uint64_t id = parcel.ReadUint64();
    std::vector<float> times, values;
    if (!(parcel.ReadFloatVector(&times) && parcel.ReadFloatVector(&values))) {
        ROSEN_LOGE("Unmarshalling CustomInterpolator failed");
        return nullptr;
    }
    return new RSCustomInterpolator(id, std::move(times), std::move(values));
}

void RSCustomInterpolator::Convert(int duration)
{
    if (interpolateFunc_ == nullptr) {
        ROSEN_LOGE("RSCustomInterpolator::Convert, interpolateFunc_ is nullptr");
        return;
    }
    uint64_t frameInterval = 16666667;
    int numAnim = static_cast<int>(std::ceil(static_cast<double>(duration * MS_TO_NS) / frameInterval));
    const int maxSamplePoints = 300;
    const int minSamplePoints = 2;
    numAnim = std::min(std::max(minSamplePoints, numAnim), maxSamplePoints);
    float lastAnimFrame = numAnim - 1;
    if (lastAnimFrame <= 0.0f) {
        ROSEN_LOGE("RSCustomInterpolator::Convert, lastAnimFrame is invalid.");
        return;
    }
    for (int i = 0; i < numAnim; i++) {
        float time = i / lastAnimFrame;
        float value = interpolateFunc_(time);
        times_.push_back(time);
        values_.push_back(value);
    }
}

float RSCustomInterpolator::InterpolateImpl(float input) const
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
    float number = times_[endLocation] - times_[startLocation];
    if (number <= 0.0f) {
        ROSEN_LOGE("RSCustomInterpolator::Interpolate, time between startLocation and endLocation is less than zero.");
        return 0.0f;
    }
    float fraction = (input - times_[startLocation]) / number;
    float ret = fraction * (values_[endLocation] - values_[startLocation]) + values_[startLocation];
    return ret;
}
} // namespace Rosen
} // namespace OHOS
