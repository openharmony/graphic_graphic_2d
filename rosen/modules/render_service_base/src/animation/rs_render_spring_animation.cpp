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

#include "animation/rs_animation_trace_utils.h"
#include "command/rs_animation_command.h"
#include "command/rs_message_processor.h"
#include "pipeline/rs_render_node.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr static float SECOND_TO_MILLISECOND = 1e3;
constexpr static float MILLISECOND_TO_SECOND = 1e-3;
constexpr static float SECOND_TO_NANOSECOND = 1e9;
constexpr static float RESPONSE_THRESHOLD = 0.001f;
constexpr static float FRACTION_THRESHOLD = 0.001f;
constexpr static float FRAME_TIME_INTERVAL = 1.0f / 90.0f;
} // namespace

RSRenderSpringAnimation::RSRenderSpringAnimation(AnimationId id, const PropertyId& propertyId,
    const std::shared_ptr<RSRenderPropertyBase>& originValue,
    const std::shared_ptr<RSRenderPropertyBase>& startValue,
    const std::shared_ptr<RSRenderPropertyBase>& endValue)
    : RSRenderPropertyAnimation(id, propertyId, originValue), startValue_(startValue), endValue_(endValue)
{
    // spring model is not initialized, so we can't calculate estimated duration
}

void RSRenderSpringAnimation::DumpAnimationType(std::string& out) const
{
    out += "Type:RSRenderSpringAnimation";
}

void RSRenderSpringAnimation::SetSpringParameters(float response, float dampingRatio, float blendDuration)
{
    response_ = response;
    dampingRatio_ = dampingRatio;
    blendDuration_ = blendDuration * SECOND_TO_NANOSECOND; // convert to ns
}

void RSRenderSpringAnimation::SetZeroThreshold(float zeroThreshold)
{
    constexpr float ZERO = 0.0f;
    if (zeroThreshold < ZERO) {
        ROSEN_LOGE("RSRenderSpringAnimation::SetZeroThreshold: invalid threshold value.");
        needLogicallyFinishCallback_ = false;
        return;
    }
    zeroThreshold_ = zeroThreshold;
    needLogicallyFinishCallback_ = true;
}

#ifdef ROSEN_OHOS
bool RSRenderSpringAnimation::Marshalling(Parcel& parcel) const
{
    if (!RSRenderPropertyAnimation::Marshalling(parcel)) {
        ROSEN_LOGE("RSRenderSpringAnimation::Marshalling, RenderPropertyAnimation failed");
        return false;
    }
    if (!(RSRenderPropertyBase::Marshalling(parcel, startValue_) &&
            RSRenderPropertyBase::Marshalling(parcel, endValue_))) {
        ROSEN_LOGE("RSRenderSpringAnimation::Marshalling, MarshallingHelper failed");
        return false;
    }

    if (!(RSMarshallingHelper::Marshalling(parcel, response_) &&
            RSMarshallingHelper::Marshalling(parcel, dampingRatio_) &&
            RSMarshallingHelper::Marshalling(parcel, blendDuration_) &&
            RSMarshallingHelper::Marshalling(parcel, needLogicallyFinishCallback_) &&
            RSMarshallingHelper::Marshalling(parcel, zeroThreshold_))) {
        return false;
    }

    if (initialVelocity_) {
        return RSMarshallingHelper::Marshalling(parcel, true) &&
               RSMarshallingHelper::Marshalling(parcel, initialVelocity_);
    }

    return RSMarshallingHelper::Marshalling(parcel, false);
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
            RSRenderPropertyBase::Unmarshalling(parcel, endValue_))) {
        return false;
    }

    auto haveInitialVelocity = false;
    if (!(RSMarshallingHelper::Unmarshalling(parcel, response_) &&
            RSMarshallingHelper::Unmarshalling(parcel, dampingRatio_) &&
            RSMarshallingHelper::Unmarshalling(parcel, blendDuration_) &&
            RSMarshallingHelper::Unmarshalling(parcel, needLogicallyFinishCallback_) &&
            RSMarshallingHelper::Unmarshalling(parcel, zeroThreshold_) &&
            RSMarshallingHelper::Unmarshalling(parcel, haveInitialVelocity))) {
        return false;
    }

    if (haveInitialVelocity) {
        return RSMarshallingHelper::Unmarshalling(parcel, initialVelocity_);
    }
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
        // calculateAnimationValue_ is embedded modify for stat animate frame drop
        calculateAnimationValue_ = false;
        return;
    }
    if (springValueEstimator_ == nullptr) {
        ROSEN_LOGE("RSRenderSpringAnimation::OnAnimate, springValueEstimator is null");
        return;
    }
    if (ROSEN_EQ(fraction, 1.0f, FRACTION_THRESHOLD)) {
        prevMappedTime_ = GetDuration() * MILLISECOND_TO_SECOND;
        springValueEstimator_->UpdateAnimationValue(prevMappedTime_, GetAdditive());
        return;
    }

    prevMappedTime_ = GetDuration() * fraction * MILLISECOND_TO_SECOND;
    springValueEstimator_->UpdateAnimationValue(prevMappedTime_, GetAdditive());

    if (GetNeedLogicallyFinishCallback() && (animationFraction_.GetRemainingRepeatCount() == 1)) {
        auto currentValue = springValueEstimator_->GetAnimationProperty();
        if (currentValue == nullptr) {
            ROSEN_LOGE("RSRenderSpringAnimation::OnAnimate, failed to get current animation value");
            return;
        }
        auto targetValue = animationFraction_.GetCurrentIsReverseCycle() ? startValue_ : endValue_;
        if (!currentValue->IsNearEqual(targetValue, zeroThreshold_)) {
            return;
        }
        auto zeroValue = startValue_ - startValue_;
        auto velocity = springValueEstimator_->GetPropertyVelocity(prevMappedTime_);
        if ((velocity * FRAME_TIME_INTERVAL)->IsNearEqual(zeroValue, zeroThreshold_)) {
            CallLogicallyFinishCallback();
            needLogicallyFinishCallback_ = false;
        }
    }
}

void RSRenderSpringAnimation::DumpFraction(float fraction, int64_t time)
{
    RSAnimationTraceUtils::GetInstance().addAnimationFrameTrace(
        GetTargetId(), GetAnimationId(), GetPropertyId(), fraction, GetPropertyValue(), time);
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
    // stop running the previous animation and inherit velocity from it
    InheritSpringAnimation(prevAnimation);
}

void RSRenderSpringAnimation::InheritSpringAnimation(const std::shared_ptr<RSRenderAnimation>& prevAnimation)
{
    // return if no other spring animation(s) running, or the other animation is finished
    // meanwhile, align run time for both spring animations, prepare for status inheritance
    if (prevAnimation == nullptr || prevAnimation->Animate(animationFraction_.GetLastFrameTime())) {
        blendDuration_ = 0;
        return;
    }

    if (springValueEstimator_ == nullptr) {
        ROSEN_LOGD("RSRenderSpringAnimation::InheritSpringAnimation failed, springValueEstimator is null");
        return;
    }

    // extract spring status from previous spring animation
    auto prevSpringAnimation = std::static_pointer_cast<RSRenderSpringAnimation>(prevAnimation);

    // inherit spring status from previous spring animation
    if (!InheritSpringStatus(prevSpringAnimation.get())) {
        blendDuration_ = 0;
        return;
    }
    // inherit spring response
    auto& previousValueEstimator = prevSpringAnimation->springValueEstimator_;
    if (previousValueEstimator) {
        springValueEstimator_->SetResponse(previousValueEstimator->GetResponse());
    } else {
        ROSEN_LOGD("RSRenderSpringAnimation::InheritSpringAnimation, the springValueEstimator of previous "
                   "animation is null");
        blendDuration_ = 0;
    }

    if (ROSEN_EQ(springValueEstimator_->GetResponse(), response_, RESPONSE_THRESHOLD)) {
        // if response is not changed, we can skip blend duration
        blendDuration_ = 0;
    } else if (blendDuration_ == 0) {
        // if blend duration is not set, we can skip blend duration
        springValueEstimator_->SetResponse(response_);
    } else if (ROSEN_EQ(response_, prevSpringAnimation->response_, RESPONSE_THRESHOLD)) {
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
        ROSEN_LOGD("RSRenderSpringAnimation::OnDetach, target is nullptr");
        return;
    }
    auto propertyId = GetPropertyId();
    auto id = GetAnimationId();
    target->GetAnimationManager().UnregisterSpringAnimation(propertyId, id);
}

void RSRenderSpringAnimation::OnInitialize(int64_t time)
{
    if (springValueEstimator_ == nullptr) {
        ROSEN_LOGD("RSRenderSpringAnimation::OnInitialize failed, springValueEstimator is null");
        RSRenderPropertyAnimation::OnInitialize(time);
        return;
    }

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
            auto response = springValueEstimator_->GetResponse();
            response += (response_ - response) * blendRatio;
            springValueEstimator_->SetResponse(response);
            blendDuration_ -= blendTime;
        } else {
            // if blend duration is over, set response to final response
            springValueEstimator_->SetResponse(response_);
            blendDuration_ = 0;
        }
    }

    if (initialVelocity_ == nullptr) {
        initialVelocity_ = startValue_ * 0.f;
    }

    RSAnimationTraceUtils::GetInstance().addSpringInitialVelocityTrace(
        GetPropertyId(), GetAnimationId(), initialVelocity_, GetPropertyValue());
    springValueEstimator_->SetInitialVelocity(initialVelocity_);
    springValueEstimator_->UpdateSpringParameters();

    if (blendDuration_) {
        // blend is still in progress, no need to estimate duration, use 300ms as default
        SetDuration(300);
    } else {
        // blend finished, estimate duration until the spring system reaches rest
        SetDuration(std::lroundf(springValueEstimator_->UpdateDuration() * SECOND_TO_MILLISECOND));
        // this will set needInitialize_ to false
        RSRenderPropertyAnimation::OnInitialize(time);
    }
}

std::tuple<std::shared_ptr<RSRenderPropertyBase>, std::shared_ptr<RSRenderPropertyBase>,
    std::shared_ptr<RSRenderPropertyBase>>
RSRenderSpringAnimation::GetSpringStatus() const
{
    // if animation is never started, return start value and initial velocity
    // fraction_threshold will change with animationScale.
    if (ROSEN_EQ(prevMappedTime_, 0.0f, FRACTION_THRESHOLD / animationFraction_.GetAnimationScale())) {
        return { startValue_, endValue_, initialVelocity_ };
    }

    if (springValueEstimator_ == nullptr) {
        ROSEN_LOGD("RSRenderSpringAnimation::GetSpringStatus failed, springValueEstimator is null");
        return { startValue_, endValue_, initialVelocity_ };
    }

    auto velocity = springValueEstimator_->GetPropertyVelocity(prevMappedTime_);

    // return current position and velocity
    return { springValueEstimator_->GetAnimationProperty(), endValue_, velocity };
}

bool RSRenderSpringAnimation::InheritSpringStatus(const RSRenderSpringAnimation* from)
{
    if (from == nullptr) {
        ROSEN_LOGD("RSRenderSpringAnimation::InheritSpringStatus failed, from is null!");
        return false;
    }

    if (springValueEstimator_ == nullptr) {
        ROSEN_LOGD("RSRenderSpringAnimation::InheritSpringStatus failed, springValueEstimator is null!");
        return false;
    }

    auto [lastValue, endValue, velocity] = from->GetSpringStatus();
    if (lastValue == nullptr) {
        ROSEN_LOGD("RSRenderSpringAnimation::InheritSpringStatus, unexpected lastValue null pointer!");
        return false;
    }

    if (startValue_ && !startValue_->IsEqual(endValue) && !(from == this)) {
        // means property values may change due to direct setting or other animations
        ROSEN_LOGD(
            "RSRenderSpringAnimation::InheritSpringStatus, current animation can't continue with last animation");
        return false;
    }

    // inherit spring status from last spring animation
    startValue_ = lastValue->Clone();
    initialVelocity_ = velocity;
    originValue_ = startValue_->Clone();
    lastValue_ = startValue_->Clone();
    springValueEstimator_->UpdateStartValueAndLastValue(startValue_, lastValue_);
    springValueEstimator_->SetInitialVelocity(velocity);
    return true;
}

bool RSRenderSpringAnimation::GetNeedLogicallyFinishCallback() const
{
    return needLogicallyFinishCallback_;
}

void RSRenderSpringAnimation::CallLogicallyFinishCallback() const
{
    NodeId targetId = GetTargetId();
    AnimationId animationId = GetAnimationId();

    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCallback>(targetId, animationId, LOGICALLY_FINISHED);
    RSMessageProcessor::Instance().AddUIMessage(ExtractPid(animationId), std::move(command));
}

void RSRenderSpringAnimation::SetInitialVelocity(const std::shared_ptr<RSRenderPropertyBase>& velocity)
{
    if (!velocity) {
        ROSEN_LOGD("RSRenderSpringAnimation::SetInitialVelocity: velocity is a nullptr.");
        return;
    }
    // if inheritance happens, this velocity will be replace again
    initialVelocity_ = velocity;
}

void RSRenderSpringAnimation::InitValueEstimator()
{
    if (property_ == nullptr) {
        ROSEN_LOGD("RSRenderSpringAnimation::InitValueEstimator failed, the property is null");
        return;
    }
    if (springValueEstimator_ == nullptr) {
        springValueEstimator_ = property_->CreateRSSpringValueEstimator();
    }
    springValueEstimator_->InitRSSpringValueEstimator(property_, startValue_, endValue_, lastValue_);
    springValueEstimator_->SetResponse(response_);
    springValueEstimator_->SetDampingRatio(dampingRatio_);
}
} // namespace Rosen
} // namespace OHOS
