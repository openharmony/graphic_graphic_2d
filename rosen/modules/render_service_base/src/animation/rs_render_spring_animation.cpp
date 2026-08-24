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

#include <cmath>

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
constexpr static double SECOND_TO_NANOSECOND = 1e9;
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

void RSRenderSpringAnimation::DumpAnimationInfo(std::string& out) const
{
    out.append("Type:RSRenderSpringAnimation");
    DumpProperty(out);
    RSPropertyType type = RSPropertyType::INVALID;
    if (property_ != nullptr) {
        type = property_->GetPropertyType();
    }
    out.append(", StartValue: ").append(RSAnimationTraceUtils::GetInstance().ParseRenderPropertyValue(startValue_));
    out.append(", EndValue: ").append(RSAnimationTraceUtils::GetInstance().ParseRenderPropertyValue(endValue_));
}

void RSRenderSpringAnimation::SetSpringParameters(float response, float dampingRatio, float blendDuration,
    float minimumAmplitudeRatio, std::optional<ConvergeParams> convergeParams)
{
    if (!std::isfinite(response) || !std::isfinite(dampingRatio) || !std::isfinite(blendDuration) ||
        !std::isfinite(minimumAmplitudeRatio)) {
        ROSEN_LOGE("RSRenderSpringAnimation::SetSpringParameters: invalid spring parameters.");
        return;
    }
    response_ = response;
    dampingRatio_ = std::clamp(dampingRatio, SPRING_MIN_DAMPING_RATIO, SPRING_MAX_DAMPING_RATIO);
    blendDuration_ = blendDuration * SECOND_TO_NANOSECOND; // convert to ns
    minimumAmplitudeRatio_ = minimumAmplitudeRatio;
    if (convergeParams.has_value()) {
        if (!std::isfinite(convergeParams->convergeProgressThreshold_) ||
            convergeParams->convergeProgressThreshold_ < 0.0f || convergeParams->convergeProgressThreshold_ > 1.0f ||
            !std::isfinite(convergeParams->convergeResponseFactor_) || convergeParams->convergeResponseFactor_ < 0.0f) {
            ROSEN_LOGE("RSRenderSpringAnimation::%{public}s, invalid convergeParams, set nullopt", __func__);
        } else if (!(ROSEN_EQ(convergeParams->convergeProgressThreshold_, 1.0f) ||
                       ROSEN_EQ(convergeParams->convergeResponseFactor_, 1.0f))) {
            // Both being set to 1 is the default value for the upper layer and should remain consistent with the
            // original.
            convergeParams_ = convergeParams;
        }
    }
}

void RSRenderSpringAnimation::SetZeroThreshold(float zeroThreshold)
{
    constexpr float ZERO = 0.0f;
    if (zeroThreshold < ZERO || !std::isfinite(zeroThreshold)) {
        ROSEN_LOGE("RSRenderSpringAnimation::SetZeroThreshold: invalid threshold value.");
        needLogicallyFinishCallback_ = false;
        return;
    }
    zeroThreshold_ = zeroThreshold;
    needLogicallyFinishCallback_ = true;
}

bool RSRenderSpringAnimation::OnAnimate(float fraction)
{
    if (GetPropertyId() == 0) {
        // calculateAnimationValue_ is embedded modify for stat animate frame drop
        calculateAnimationValue_ = false;
        return false;
    }
    if (springValueEstimator_ == nullptr) {
        ROSEN_LOGE("RSRenderSpringAnimation::OnAnimate, springValueEstimator is null");
        return false;
    }
    if (ROSEN_EQ(fraction, 1.0f, FRACTION_THRESHOLD)) {
        prevMappedTime_ = GetDuration() * MILLISECOND_TO_SECOND;
        if (isConverging_) {
            prevMappedTime_ -= lastConvergeTime_;
        }
        springValueEstimator_->UpdateAnimationValue(prevMappedTime_, GetAdditive());
        return true;
    }
    CheckStartConverge();
    auto mappedTime = GetDuration() * fraction * MILLISECOND_TO_SECOND;
    if (isConverging_) {
        UpdateSpringConvergeParameters();
        // Since the converged spring parameters have been updated, the difference from lastConvergeTime_ should be used
        // as the time input.
        prevMappedTime_ = mappedTime - lastConvergeTime_;
    } else {
        prevMappedTime_ = mappedTime;
    }
    springValueEstimator_->UpdateAnimationValue(prevMappedTime_, GetAdditive());
    if (CheckConvergeStatus(mappedTime)) {
        return true;
    }

    if (GetNeedLogicallyFinishCallback() && (animationFraction_.GetRemainingRepeatCount() == 1)) {
        auto currentValue = springValueEstimator_->GetAnimationProperty();
        if (currentValue == nullptr) {
            ROSEN_LOGE("RSRenderSpringAnimation::OnAnimate, failed to get current animation value");
            return false;
        }
        auto targetValue = animationFraction_.GetCurrentIsReverseCycle() ? startValue_ : endValue_;
        if (!currentValue->IsNearEqual(targetValue, zeroThreshold_)) {
            return false;
        }
        auto zeroValue = startValue_ - startValue_;
        auto velocity = springValueEstimator_->GetPropertyVelocity(prevMappedTime_);
        if (velocity == nullptr) {
            ROSEN_LOGE("RSRenderSpringAnimation::OnAnimate, failed to get velocity");
            return false;
        }
        if ((velocity * FRAME_TIME_INTERVAL)->IsNearEqual(zeroValue, zeroThreshold_)) {
            CallLogicallyFinishCallback();
            needLogicallyFinishCallback_ = false;
        }
    }
    return false;
}

void RSRenderSpringAnimation::RebuildPropertyValue(float fraction)
{
    if (property_ == nullptr || startValue_ == nullptr || endValue_ == nullptr || springValueEstimator_ == nullptr) {
        ROSEN_LOGE("RSRenderSpringAnimation::RebuildPropertyValue failed: "
            "property[%{public}d] startValue[%{public}d] endValue[%{public}d] estimator[%{public}d]",
            property_ != nullptr, startValue_ != nullptr, endValue_ != nullptr, springValueEstimator_ != nullptr);
        return;
    }
    auto baseValue = property_ - (endValue_ - startValue_);
    springValueEstimator_->InitRSSpringValueEstimator(property_, baseValue, property_, baseValue);
    prevMappedTime_ = GetDuration() * fraction * MILLISECOND_TO_SECOND;
    springValueEstimator_->UpdateAnimationValue(prevMappedTime_, false);
}

void RSRenderSpringAnimation::OnAttach()
{
    auto target = GetTarget();
    if (target == nullptr) {
        ROSEN_LOGE("RSRenderSpringAnimation::OnAttach, target is nullptr");
        return;
    }
    auto animationManager = target->GetAnimationManager();
    if (!animationManager) {
        ROSEN_LOGE("%{public}s: animationManager is nullptr", __func__);
        return;
    }
    // check if any other spring animation running on this property
    auto propertyId = GetPropertyId();
    auto prevAnimation = animationManager->QuerySpringAnimation(propertyId);
    animationManager->RegisterSpringAnimation(propertyId, GetAnimationId());
    // stop running the previous animation and inherit velocity from it
    InheritSpringAnimation(prevAnimation, false);
}

void RSRenderSpringAnimation::InheritSpringAnimation(
    const std::shared_ptr<RSRenderAnimation>& prevAnimation, bool isCustom)
{
    // return if no other spring animation(s) running, or the other animation is finished
    // meanwhile, align run time for both spring animations, prepare for status inheritance
    int64_t delayTime = 0;
    if (prevAnimation == nullptr ||
        prevAnimation->Animate(animationFraction_.GetLastFrameTime(), delayTime, isCustom)) {
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
    if (!InheritSpringStatus(prevSpringAnimation.get(), isCustom)) {
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
    if (auto animationManager = target->GetAnimationManager()) {
        animationManager->UnregisterSpringAnimation(propertyId, id);
    }
}

void RSRenderSpringAnimation::OnInitialize(int64_t time, bool isCustom)
{
    if (springValueEstimator_ == nullptr) {
        ROSEN_LOGD("RSRenderSpringAnimation::OnInitialize failed, springValueEstimator is null");
        RSRenderPropertyAnimation::OnInitialize(time, isCustom);
        return;
    }

    isCustom_ = isCustom;

    // The blend takes effect only in non-convergence mode.
    if (blendDuration_ && !isConverging_) {
        auto lastFrameTime = animationFraction_.GetLastFrameTime();

        // reset animation fraction
        InheritSpringStatus(this, isCustom);
        animationFraction_.ResetFraction();
        prevMappedTime_ = 0.0f;

        // blend response by linear interpolation
        const float animationScale = isCustom ? 1.0f : RSAnimationFraction::GetAnimationScale();
        uint64_t blendTime = (time - lastFrameTime) * animationScale;
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

    if (startValue_ != nullptr && initialVelocity_ == nullptr) {
        initialVelocity_ = startValue_ * 0.f;
    }

    RSAnimationTraceUtils::GetInstance().AddSpringInitialVelocityTrace(
        GetPropertyId(), GetAnimationId(), initialVelocity_, GetPropertyValue());
    springValueEstimator_->SetInitialVelocity(initialVelocity_);
    springValueEstimator_->SetMinimumAmplitudeRatio(minimumAmplitudeRatio_);
    springValueEstimator_->UpdateSpringParameters();

    if (blendDuration_ && !isConverging_) {
        // blend is still in progress, no need to estimate duration, use 300ms as default
        SetDuration(300);
    } else {
        // blend finished, estimate duration until the spring system reaches rest
        SetDuration(std::lroundf(springValueEstimator_->UpdateDuration() * SECOND_TO_MILLISECOND));
        // this will set needInitialize_ to false
        RSRenderPropertyAnimation::OnInitialize(time, isCustom);
    }
}

std::tuple<std::shared_ptr<RSRenderPropertyBase>, std::shared_ptr<RSRenderPropertyBase>,
    std::shared_ptr<RSRenderPropertyBase>>
RSRenderSpringAnimation::GetSpringStatus(bool isCustom) const
{
    // if animation is never started, return start value and initial velocity
    // fraction_threshold will change with animationScale.
    const float animationScale = isCustom ? 1.0f : RSAnimationFraction::GetAnimationScale();
    if (ROSEN_EQ(animationScale, 0.0f) || ROSEN_EQ(prevMappedTime_, 0.0f, FRACTION_THRESHOLD / animationScale)) {
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

bool RSRenderSpringAnimation::InheritSpringStatus(const RSRenderSpringAnimation* from, bool isCustom)
{
    if (from == nullptr) {
        ROSEN_LOGD("RSRenderSpringAnimation::InheritSpringStatus failed, from is null!");
        return false;
    }

    if (springValueEstimator_ == nullptr) {
        ROSEN_LOGD("RSRenderSpringAnimation::InheritSpringStatus failed, springValueEstimator is null!");
        return false;
    }

    auto [lastValue, endValue, velocity] = from->GetSpringStatus(isCustom);
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
    if (startValue_) {
        originValue_ = startValue_->Clone();
        lastValue_ = startValue_->Clone();
    }
    springValueEstimator_->UpdateStartValueAndLastValue(startValue_, lastValue_);
    springValueEstimator_->SetInitialVelocity(velocity);
    return true;
}

void RSRenderSpringAnimation::UpdateSpringConvergeParameters()
{
    // It is used to update initialOffset_ and initialVelocity_ of the spring model.
    InheritSpringStatus(this, isCustom_);
    response_ = std::max(convergeParams_->convergeResponseFactor_ * response_, SPRING_MIN_RESPONSE);
    springValueEstimator_->SetResponse(response_);
    springValueEstimator_->UpdateSpringParameters();
}

bool RSRenderSpringAnimation::IsConvergeCloseToTarget() const
{
    // Ensure that springValueEstimator_ is not nullptr
    if (!endThreshold_) {
        ROSEN_LOGE("RSRenderSpringAnimation::%{public}s, failed to get endThreshold", __func__);
        return false;
    }
    auto currentValue = springValueEstimator_->GetAnimationProperty();
    if (!currentValue || !endValue_) {
        ROSEN_LOGE("RSRenderSpringAnimation::%{public}s, failed to get current animation value or endValue", __func__);
        return false;
    }
    // Determine whether it is sufficiently close to the endValue_, and each dimension must meet the requirement.
    return currentValue->IsAbsNearEqual(endValue_, endThreshold_);
}

bool RSRenderSpringAnimation::IsConvergeEnd() const
{
    // Ensure that springValueEstimator_ is not nullptr
    // In convergence mode, it can only stop in advance if it is not in overshoot and is sufficiently close to the
    // target value.
    return !springValueEstimator_->WillOverShoot() && IsConvergeCloseToTarget();
}

void RSRenderSpringAnimation::CheckStartConverge()
{
    if (!convergeParams_.has_value()) {
        return;
    }
    if (isConverging_) {
        return;
    }
    if (springValueEstimator_->IsStartConverging(prevMappedTime_, convergeParams_->convergeProgressThreshold_)) {
        isConverging_ = true;
        lastConvergeTime_ = prevMappedTime_;

        // When entering convergence mode, if the blend is also in progress, the duration is calculated in
        // advance to ensure that the duration is calculated based on the original spring parameters.
        if (blendDuration_) {
            SetDuration(std::lroundf(springValueEstimator_->UpdateDuration() * SECOND_TO_MILLISECOND));
            RSRenderPropertyAnimation::OnInitialize(0);
        }

        auto originInitialOffset = springValueEstimator_->GetInitialOffset();
        endThreshold_ = originInitialOffset * std::min(minimumAmplitudeRatio_, SPRING_TRAIL_END_THRESHOLD);
        auto originLastFrameThreshold = springValueEstimator_->GetLastFrameThreshold();
        endThreshold_->TakeAbsMaxFrom(originLastFrameThreshold);
    }
}

bool RSRenderSpringAnimation::CheckConvergeStatus(float time)
{
    if (!isConverging_) {
        return false;
    }
    if (IsConvergeEnd()) {
        return true;
    }
    // It is the mappedTime of the previous frame, which is used to calculate the time input of the spring model after
    // the parameters are updated.
    lastConvergeTime_ = time;
    return false;
}

bool RSRenderSpringAnimation::GetNeedLogicallyFinishCallback() const
{
    return needLogicallyFinishCallback_;
}

void RSRenderSpringAnimation::CallLogicallyFinishCallback() const
{
    NodeId targetId = GetTargetId();
    AnimationId animationId = GetAnimationId();
    uint64_t token = GetToken();

    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCallback>(targetId, animationId, token, AnimationCallbackEvent::LOGICALLY_FINISHED);
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
    if (springValueEstimator_ == nullptr) {
        ROSEN_LOGE("RSRenderSpringAnimation::InitValueEstimator, springValueEstimator_ is nullptr.");
        return;
    }
    springValueEstimator_->InitRSSpringValueEstimator(property_, startValue_, endValue_, lastValue_);
    springValueEstimator_->SetResponse(response_);
    springValueEstimator_->SetDampingRatio(dampingRatio_);
}

void RSRenderSpringAnimation::ProcessOnRepeatFinish()
{
    RSRenderPropertyAnimation::ProcessOnRepeatFinish();
    isConverging_ = false;
    lastConvergeTime_ = 0.0f;
}
} // namespace Rosen
} // namespace OHOS
