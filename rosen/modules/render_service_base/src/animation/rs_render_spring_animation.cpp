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

#include "animation/rs_render_spring_animation.h"

#include "pipeline/rs_render_node.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr static float MILLISECOND_TO_SECOND = 1e-3;
constexpr static float SECOND_TO_NANOSECOND = 1e9;
constexpr static float RESPONSE_THRESHOLD = 0.001f;
constexpr static float FRACTION_THRESHOLD = 0.001f;
constexpr static float SPRING_MAX_DURATION = 30.0f;
} // namespace

RSRenderSpringAnimation::RSRenderSpringAnimation(AnimationId id, const PropertyId& propertyId,
    const std::shared_ptr<RSRenderPropertyBase>& originValue,
    const std::shared_ptr<RSRenderPropertyBase>& startValue,
    const std::shared_ptr<RSRenderPropertyBase>& endValue)
    : RSRenderPropertyAnimation(id, propertyId, originValue), RSSpringModel<std::shared_ptr<RSRenderPropertyBase>>(),
      startValue_(startValue), endValue_(endValue), originStartValue_(startValue_)
{
    // spring model is not initialized, so we can't calculate estimated duration
}

void RSRenderSpringAnimation::SetSpringParameters(float response, float dampingRatio, float blendDuration)
{
    response_ = response;
    finalResponse_ = response;
    dampingRatio_ = dampingRatio;
    blendDuration_ = blendDuration * SECOND_TO_NANOSECOND; // convert to ns
}

void RSRenderSpringAnimation::SetZeroThreshold(float zeroThreshold)
{
    zeroThreshold_ = zeroThreshold;
}

#ifdef ROSEN_OHOS
bool RSRenderSpringAnimation::Marshalling(Parcel& parcel) const
{
    if (!RSRenderPropertyAnimation::Marshalling(parcel)) {
        ROSEN_LOGE("RSRenderSpringAnimation::Marshalling, RenderPropertyAnimation failed");
        return false;
    }
    if (!(RSRenderPropertyBase::Marshalling(parcel, startValue_) &&
            RSRenderPropertyBase::Marshalling(parcel, endValue_) &&
            RSRenderPropertyBase::Marshalling(parcel, originStartValue_))) {
        ROSEN_LOGE("RSRenderSpringAnimation::Marshalling, MarshallingHelper failed");
        return false;
    }

    if (!(RSMarshallingHelper::Marshalling(parcel, response_) &&
            RSMarshallingHelper::Marshalling(parcel, dampingRatio_) &&
            RSMarshallingHelper::Marshalling(parcel, blendDuration_))) {
        return false;
    }

    return true;
}

RSRenderSpringAnimation* RSRenderSpringAnimation::Unmarshalling(Parcel& parcel)
{
    auto* renderSpringAnimation = new RSRenderSpringAnimation();
    if (!renderSpringAnimation->ParseParam(parcel)) {
        ROSEN_LOGE("RSRenderSpringAnimation::Unmarshalling, failed");
        delete renderSpringAnimation;
        return nullptr;
    }
    return renderSpringAnimation;
}

bool RSRenderSpringAnimation::ParseParam(Parcel& parcel)
{
    if (!RSRenderPropertyAnimation::ParseParam(parcel)) {
        ROSEN_LOGE("RSRenderSpringAnimation::ParseParam, ParseParam Fail");
        return false;
    }

    if (!(RSRenderPropertyBase::Unmarshalling(parcel, startValue_) &&
            RSRenderPropertyBase::Unmarshalling(parcel, endValue_) &&
            RSRenderPropertyBase::Unmarshalling(parcel, originStartValue_))) {
        return false;
    }

    if (!(RSMarshallingHelper::Unmarshalling(parcel, response_) &&
            RSMarshallingHelper::Unmarshalling(parcel, dampingRatio_) &&
            RSMarshallingHelper::Unmarshalling(parcel, blendDuration_))) {
        return false;
    }
    // copy response to final response
    finalResponse_ = response_;

    return true;
}
#endif

void RSRenderSpringAnimation::OnSetFraction(float fraction)
{
    // spring animation should not support set fraction
    OnAnimate(fraction);
}

void RSRenderSpringAnimation::OnAnimate(float fraction)
{
    if (GetPropertyId() == 0) {
        return;
    } else if (ROSEN_EQ(fraction, 1.0f, FRACTION_THRESHOLD)) {
        SetAnimationValue(endValue_);
        prevMappedTime_ = GetDuration() * MILLISECOND_TO_SECOND;
        return;
    }
    auto mappedTime = fraction * GetDuration() * MILLISECOND_TO_SECOND;
    auto displacement = CalculateDisplacement(mappedTime);
    SetAnimationValue(endValue_ + displacement);

    // keep the mapped time, this will be used to calculate instantaneous velocity
    prevMappedTime_ = mappedTime;
}

std::tuple<bool, bool> RSRenderSpringAnimation::OnAnimateByTime(float time)
{
    // keep the mapped time, this will be used to calculate instantaneous velocity
    prevMappedTime_ = time;

    bool isRepeatFinished = false;
    bool isFinished = true;
    if (GetPropertyId() == 0) {
        return { isFinished, isRepeatFinished };
    }
    isFinished = false;
    animationFraction_.UpdateReverseState(isFinished);
    float timeInSecond = time * MILLISECOND_TO_SECOND;
    auto displacement = CalculateDisplacement(timeInSecond);
    auto currentValue =
        animationFraction_.CurrentIsReverseCycle() ? startValue_ - displacement : endValue_ + displacement;
    auto targetValue = animationFraction_.CurrentIsReverseCycle() ? startValue_ : endValue_;
    if ((currentValue->IsNearEqual(targetValue, zeroThreshold_)) || (timeInSecond >= SPRING_MAX_DURATION)) {
        currentValue = targetValue;
        constexpr double FRAME_TIME_INTERVAL = 1.0f / 60.0f;
        auto velocity = CalculateVelocity(timeInSecond);
        auto zero = startValue_ - startValue_;
        // only when the distance between two frames is also close to 0, can the spring animation be seen as finished.
        // limit the duration of spring animation by SPRING_MAX_DURATION
        if (((velocity * FRAME_TIME_INTERVAL)->IsNearEqual(zero, zeroThreshold_)) ||
            (timeInSecond >= SPRING_MAX_DURATION)) {
            animationFraction_.OnCurrentAnimationRoundFinish();
            animationFraction_.ResetPlayTime();
            if (animationFraction_.IsInRepeat()) {
                isRepeatFinished = true;
            }
            if (animationFraction_.GetRemainingRepeatCount() == 0) {
                isFinished = true;
                animationFraction_.UpdateReverseState(isFinished);
                SetAnimationValue(currentValue);
                return { isFinished, animationFraction_.GetRepeatCallbackEnable() };
            }
        }
    }

    SetAnimationValue(currentValue);
    return { isFinished, isRepeatFinished };
}

void RSRenderSpringAnimation::OnAttach()
{
    auto target = GetTarget();
    if (target == nullptr) {
        ROSEN_LOGE("RSRenderSpringAnimation::OnAttach, target is nullptr");
        return;
    }
    // check if any other spring animation running on this property
    auto propertyId = GetPropertyId();
    auto prevAnimation = target->GetAnimationManager().QuerySpringAnimation(propertyId);
    target->GetAnimationManager().RegisterSpringAnimation(propertyId, GetAnimationId());

    // return if no other spring animation(s) running, or the other animation is finished
    // meanwhile, align run time for both spring animations, prepare for status inheritance
    if (prevAnimation == nullptr || prevAnimation->Animate(animationFraction_.GetLastFrameTime())) {
        blendDuration_ = 0;
        return;
    }

    // extract spring status from previous spring animation
    auto prevSpringAnimation = std::static_pointer_cast<RSRenderSpringAnimation>(prevAnimation);

    // inherit spring status from previous spring animation
    InheritSpringStatus(prevSpringAnimation.get());
    // inherit spring response
    response_ = prevSpringAnimation->response_;
    if (ROSEN_EQ(response_, finalResponse_, RESPONSE_THRESHOLD)) {
        // if response is not changed, we can skip blend duration
        blendDuration_ = 0;
    } else if (blendDuration_ == 0) {
        // if blend duration is not set, we can skip blend duration
        response_ = finalResponse_;
    } else if (ROSEN_EQ(finalResponse_, prevSpringAnimation->finalResponse_, RESPONSE_THRESHOLD)) {
        // if previous spring is blending to the same final response, we can continue previous blending process
        blendDuration_ = prevSpringAnimation->blendDuration_;
    }

    // set previous spring animation to FINISHED
    prevSpringAnimation->FinishOnCurrentPosition();
}

void RSRenderSpringAnimation::OnDetach()
{
    auto target = GetTarget();
    if (target == nullptr) {
        ROSEN_LOGE("RSRenderSpringAnimation::OnDetach, target is nullptr");
        return;
    }
    auto propertyId = GetPropertyId();
    auto id = GetAnimationId();
    target->GetAnimationManager().UnregisterSpringAnimation(propertyId, id);
}

void RSRenderSpringAnimation::OnInitialize(int64_t time)
{
    if (blendDuration_) {
        auto lastFrameTime = animationFraction_.GetLastFrameTime();

        // reset animation fraction
        InheritSpringStatus(this);
        animationFraction_.ResetFraction();
        prevMappedTime_ = 0.0f;

        // blend response by linear interpolation
        uint64_t blendTime = (time - lastFrameTime) * animationFraction_.GetAnimationScale();
        if (blendTime < blendDuration_) {
            auto blendRatio = static_cast<float>(blendTime) / static_cast<float>(blendDuration_);
            response_ += (finalResponse_ - response_) * blendRatio;
            blendDuration_ -= blendTime;
        } else {
            // if blend duration is over, set response to final response
            response_ = finalResponse_;
            blendDuration_ = 0;
        }
    }

    // set the initial status of spring model
    initialOffset_ = startValue_ - endValue_;
    if (initialVelocity_ == nullptr) {
        initialVelocity_ = initialOffset_ * 0.f;
    }
    CalculateSpringParameters();

    if (!blendDuration_) {
        // this will set needInitialize_ to false
        RSRenderPropertyAnimation::OnInitialize(time);
        SetZeroThreshold(startValue_->GetZeroThresholdByDefault());
    }
}

void RSRenderSpringAnimation::ProcessFillModeOnFinishByTime(float endTime)
{
    auto fillMode = GetFillMode();
    if (fillMode == FillMode::FORWARDS || fillMode == FillMode::BOTH) {
        if (animationFraction_.GetEndFraction() == 0) {
            SetAnimationValue(originStartValue_);
        } else {
            SetAnimationValue(endValue_);
        }
    } else {
        OnRemoveOnCompletion();
    }
}

AnimationTimingMode RSRenderSpringAnimation::GetAnimationTimingMode() const
{
    return AnimationTimingMode::BY_TIME;
}

std::tuple<std::shared_ptr<RSRenderPropertyBase>, std::shared_ptr<RSRenderPropertyBase>, int64_t>
RSRenderSpringAnimation::GetSpringStatus() const
{
    // if animation is never started, return start value and initial velocity
    if (ROSEN_EQ(prevMappedTime_, 0.0f, FRACTION_THRESHOLD)) {
        return { startValue_, initialVelocity_, animationFraction_.GetRemainingDelayTime() };
    }

    // use average velocity over a small time interval to estimate instantaneous velocity
    auto velocity = CalculateVelocity(prevMappedTime_);

    // return current position, velocity and remainingDelayTime
    return { lastValue_->Clone(), velocity, animationFraction_.GetRemainingDelayTime() };
}

void RSRenderSpringAnimation::InheritSpringStatus(const RSRenderSpringAnimation* from)
{
    // inherit spring status from another spring animation
    int64_t remainingDelayTime = 0;
    std::tie(startValue_, initialVelocity_, remainingDelayTime) = from->GetSpringStatus();
    originValue_ = startValue_->Clone();
    lastValue_ = startValue_->Clone();
    animationFraction_.SetRemainingDelayTime(remainingDelayTime);
}

std::shared_ptr<RSRenderPropertyBase> RSRenderSpringAnimation::CalculateVelocity(float time) const
{
    float TIME_INTERVAL = animationFraction_.CurrentIsReverseCycle() ? -1e-6f : 1e-6f; // 1e-6f : 1 microsecond
    auto velocity =
        (CalculateDisplacement(time + TIME_INTERVAL) - CalculateDisplacement(time)) * (1 / std::fabs(TIME_INTERVAL));

    return velocity;
}
} // namespace Rosen
} // namespace OHOS
