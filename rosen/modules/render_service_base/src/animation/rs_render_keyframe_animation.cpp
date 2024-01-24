/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "animation/rs_render_keyframe_animation.h"

#include "animation/rs_interpolator.h"
#include "animation/rs_value_estimator.h"
#include "platform/common/rs_log.h"
#include "transaction/rs_marshalling_helper.h"

namespace OHOS {
namespace Rosen {
namespace {
static constexpr int MAX_KEYFRAME_SIZE_NUMBER = 100000;
}
RSRenderKeyframeAnimation::RSRenderKeyframeAnimation(AnimationId id, const PropertyId& propertyId,
    const std::shared_ptr<RSRenderPropertyBase>& originValue)
    : RSRenderPropertyAnimation(id, propertyId, originValue)
{}

void RSRenderKeyframeAnimation::DumpAnimationType(std::string& out) const
{
    out += "Type:RSRenderKeyframeAnimation";
}

void RSRenderKeyframeAnimation::AddKeyframe(float fraction, const std::shared_ptr<RSRenderPropertyBase>& value,
    const std::shared_ptr<RSInterpolator>& interpolator)
{
    if (fraction < FRACTION_MIN || fraction > FRACTION_MAX) {
        ROSEN_LOGE("Failed to add key frame, fraction is invalid!");
        return;
    }

    if (IsStarted()) {
        ROSEN_LOGE("Failed to add key frame, animation has started!");
        return;
    }

    keyframes_.push_back({ fraction, value, interpolator });
}

void RSRenderKeyframeAnimation::AddKeyframes(const std::vector<std::tuple<float, std::shared_ptr<RSRenderPropertyBase>,
    std::shared_ptr<RSInterpolator>>>& keyframes)
{
    if (IsStarted()) {
        ROSEN_LOGE("Failed to add key frame, animation has started!");
        return;
    }

    keyframes_ = keyframes;
}

void RSRenderKeyframeAnimation::AddKeyframe(int startDuration, int endDuration,
    const std::shared_ptr<RSRenderPropertyBase>& value, const std::shared_ptr<RSInterpolator>& interpolator)
{
    if (startDuration > endDuration) {
        ROSEN_LOGE("Failed to add key frame, startDuration larger than endDuration!");
        return;
    }

    if (IsStarted()) {
        ROSEN_LOGE("Failed to add key frame, animation has started!");
        return;
    }

    if (GetDuration() <= 0) {
        ROSEN_LOGE("Failed to add key frame, total duration is 0!");
        return;
    }

    durationKeyframes_.push_back(
        { startDuration / (float)GetDuration(), endDuration / (float)GetDuration(), value, interpolator });
}

void RSRenderKeyframeAnimation::SetDurationKeyframe(bool isDuration)
{
    isDurationKeyframe_ = isDuration;
}

bool RSRenderKeyframeAnimation::Marshalling(Parcel& parcel) const
{
    if (!RSRenderPropertyAnimation::Marshalling(parcel)) {
        ROSEN_LOGE("RSRenderKeyframeAnimation::Marshalling, RenderPropertyAnimation failed");
        return false;
    }
    if (!parcel.WriteBool(isDurationKeyframe_)) {
        ROSEN_LOGE("RSRenderKeyframeAnimation::Marshalling, isDurationKeyframe_ failed");
        return false;
    }
    if (isDurationKeyframe_) {
        uint32_t size = durationKeyframes_.size();
        if (!parcel.WriteUint32(size)) {
            ROSEN_LOGE("RSRenderKeyframeAnimation::Marshalling, Write durationkeyframe size failed");
            return false;
        }
        for (const auto& [startFraction, endFraction, property, interpolator] : durationKeyframes_) {
            if (!(parcel.WriteFloat(startFraction) && parcel.WriteFloat(endFraction) &&
                    RSRenderPropertyBase::Marshalling(parcel, property) &&
                    interpolator != nullptr && interpolator->Marshalling(parcel))) {
                ROSEN_LOGE("RSRenderKeyframeAnimation::Marshalling, Write durationkeyframe value failed");
                return false;
            }
        }
        return true;
    }
    uint32_t size = keyframes_.size();
    if (!parcel.WriteUint32(size)) {
        ROSEN_LOGE("RSRenderKeyframeAnimation::Marshalling, Write size failed");
        return false;
    }
    for (const auto& [value, property, interpolator] : keyframes_) {
        if (!(parcel.WriteFloat(value) && RSRenderPropertyBase::Marshalling(parcel, property) &&
                interpolator != nullptr && interpolator->Marshalling(parcel))) {
            ROSEN_LOGE("RSRenderKeyframeAnimation::Marshalling, Write value failed");
            return false;
        }
    }
    return true;
}

RSRenderKeyframeAnimation* RSRenderKeyframeAnimation::Unmarshalling(Parcel& parcel)
{
    RSRenderKeyframeAnimation* renderKeyframeAnimation = new RSRenderKeyframeAnimation();
    if (!renderKeyframeAnimation->ParseParam(parcel)) {
        ROSEN_LOGE("RSRenderKeyframeAnimation::Unmarshalling, ParseParam failed");
        delete renderKeyframeAnimation;
        return nullptr;
    }
    return renderKeyframeAnimation;
}

bool RSRenderKeyframeAnimation::ParseParam(Parcel& parcel)
{
    if (!RSRenderPropertyAnimation::ParseParam(parcel)) {
        ROSEN_LOGE("RSRenderKeyframeAnimation::ParseParam, RenderPropertyAnimation fail");
        return false;
    }
    if (!parcel.ReadBool(isDurationKeyframe_)) {
        ROSEN_LOGE("RSRenderKeyframeAnimation::ParseParam, Parse isDurationKeyframe fail");
        return false;
    }
    uint32_t size = 0;
    if (!parcel.ReadUint32(size)) {
        ROSEN_LOGE("RSRenderKeyframeAnimation::ParseParam, Parse Keyframes size fail");
        return false;
    }
    if (size > MAX_KEYFRAME_SIZE_NUMBER) {
        ROSEN_LOGE("RSRenderKeyframeAnimation::ParseParam, Keyframes size number is too large.");
        return false;
    }
    keyframes_.clear();
    durationKeyframes_.clear();
    if (isDurationKeyframe_) {
        return ParseDurationKeyframesParam(parcel, size);
    }
    float tupValue0 = 0;
    for (uint32_t i = 0; i < size; i++) {
        if (!(parcel.ReadFloat(tupValue0))) {
            ROSEN_LOGE("RSRenderKeyframeAnimation::ParseParam, Unmarshalling value failed");
            return false;
        }
        std::shared_ptr<RSRenderPropertyBase> tupValue1;
        if (!RSRenderPropertyBase::Unmarshalling(parcel, tupValue1)) {
            return false;
        }
        std::shared_ptr<RSInterpolator> interpolator(RSInterpolator::Unmarshalling(parcel));
        keyframes_.emplace_back(std::make_tuple(tupValue0, tupValue1, interpolator));
    }
    return true;
}

bool RSRenderKeyframeAnimation::ParseDurationKeyframesParam(Parcel& parcel, int keyframeSize)
{
    float startFraction = 0;
    float endFraction = 0;
    for (uint32_t i = 0; i < keyframeSize; i++) {
        if (!(parcel.ReadFloat(startFraction)) || !(parcel.ReadFloat(endFraction))) {
            ROSEN_LOGE("RSRenderKeyframeAnimation::ParseParam, Unmarshalling duration value failed");
            return false;
        }
        std::shared_ptr<RSRenderPropertyBase> tupValue1;
        if (!RSRenderPropertyBase::Unmarshalling(parcel, tupValue1)) {
            return false;
        }
        std::shared_ptr<RSInterpolator> interpolator(RSInterpolator::Unmarshalling(parcel));
        durationKeyframes_.emplace_back(std::make_tuple(startFraction, endFraction, tupValue1, interpolator));
    }
    return true;
}

void RSRenderKeyframeAnimation::OnAnimate(float fraction)
{
    if (keyframes_.empty() && durationKeyframes_.empty()) {
        ROSEN_LOGE("Failed to animate key frame, keyframes is empty!");
        return;
    }

    if (valueEstimator_ == nullptr) {
        return;
    }
    valueEstimator_->UpdateAnimationValue(fraction, GetAdditive());
}

void RSRenderKeyframeAnimation::InitValueEstimator()
{
    if (valueEstimator_ == nullptr) {
        valueEstimator_ = property_->CreateRSValueEstimator(RSValueEstimatorType::KEYFRAME_VALUE_ESTIMATOR);
    }
    if (isDurationKeyframe_) {
        valueEstimator_->InitDurationKeyframeAnimationValue(property_, durationKeyframes_, lastValue_);
    } else {
        valueEstimator_->InitKeyframeAnimationValue(property_, keyframes_, lastValue_);
    }
}
} // namespace Rosen
} // namespace OHOS
