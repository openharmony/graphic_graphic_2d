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
RSRenderKeyframeAnimation::RSRenderKeyframeAnimation(AnimationId id, const PropertyId& propertyId,
    const std::shared_ptr<RSRenderPropertyBase>& originValue)
    : RSRenderPropertyAnimation(id, propertyId, originValue)
{}

void RSRenderKeyframeAnimation::DumpAnimationInfo(std::string& out) const
{
    out.append("Type:RSRenderKeyframeAnimation");
    DumpProperty(out);
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
    if (valueEstimator_ == nullptr) {
        ROSEN_LOGE("RSRenderKeyframeAnimation::InitValueEstimator, valueEstimator_ is nullptr.");
        return;
    }

    if (isDurationKeyframe_) {
        valueEstimator_->InitDurationKeyframeAnimationValue(property_, durationKeyframes_, lastValue_);
    } else {
        valueEstimator_->InitKeyframeAnimationValue(property_, keyframes_, lastValue_);
    }
}
} // namespace Rosen
} // namespace OHOS
