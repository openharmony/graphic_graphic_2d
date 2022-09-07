/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
RSRenderSpringAnimation::RSRenderSpringAnimation(AnimationId id, const PropertyId& propertyId,
    const std::shared_ptr<RSRenderPropertyBase>& originValue,
    const std::shared_ptr<RSRenderPropertyBase>& startValue,
    const std::shared_ptr<RSRenderPropertyBase>& endValue)
    : RSRenderPropertyAnimation(id, propertyId, originValue),
    RSSpringModel<std::shared_ptr<RSRenderPropertyBase>>(),
    startValue_(startValue), endValue_(endValue)
{
    // spring model is not initialized, so we can't calculate estimated duration
}

void RSRenderSpringAnimation::SetSpringParameters(
    float response, float dampingRatio)
{
    response_ = response;
    dampingRatio_ = dampingRatio;
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
            RSMarshallingHelper::Marshalling(parcel, dampingRatio_))) {
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
            RSRenderPropertyBase::Unmarshalling(parcel, endValue_))) {
        return false;
    }

    if (!(RSMarshallingHelper::Unmarshalling(parcel, response_) &&
            RSMarshallingHelper::Unmarshalling(parcel, dampingRatio_))) {
        return false;
    }

    return true;
}
#endif

void RSRenderSpringAnimation::OnSetFraction(float fraction)
{
    // spring animation should not support set fraction
    OnAnimateInner(fraction);
}

void RSRenderSpringAnimation::OnAnimate(float fraction)
{
    OnAnimateInner(fraction);
}

void RSRenderSpringAnimation::OnAttach()
{
    auto target = GetTarget();
    if (target == nullptr) {
        ROSEN_LOGE("RSRenderSpringAnimation::OnAttach, target is nullptr");
        return;
    }
    auto propertyId = GetPropertyId();
    // check if any other spring animation running on this property
    auto prevAnimation = target->GetAnimationManager().QuerySpringAnimation(propertyId);
    if (prevAnimation == nullptr) {
        target->GetAnimationManager().RegisterSpringAnimation(propertyId, GetAnimationId());
        return;
    }

    // extract spring status from previous spring animation
    auto prevSpringAnimation = std::static_pointer_cast<RSRenderSpringAnimation>(prevAnimation);
    auto status = prevSpringAnimation->GetSpringStatus();
    // inherit spring position
    startValue_ = std::get<0>(status);
    originValue_ = startValue_->Clone();
    lastValue_ = startValue_->Clone();
    // inherit spring velocity
    initialVelocity_ = std::get<1>(status);

    target->GetAnimationManager().RegisterSpringAnimation(propertyId, GetAnimationId());

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

void RSRenderSpringAnimation::OnInitialize()
{
    initialOffset_ = startValue_ - endValue_;
    if (initialVelocity_  == nullptr) {
        initialVelocity_ = initialOffset_ * 0.f;
    }
    CalculateSpringParameters();
    SetDuration(std::lroundf(GetEstimatedDuration() * SECOND_TO_MILLISECOND));
}

void RSRenderSpringAnimation::OnAnimateInner(float fraction)
{
    if (GetPropertyId() == 0 || ROSEN_EQ(fraction, prevFraction_)) {
        return;
    } else if (ROSEN_EQ(fraction, 1.0f)) {
        SetAnimationValue(endValue_);
        prevFraction_ = fraction;
        return;
    }
    auto displacement = CalculateDisplacement(fraction * GetDuration() * MILLISECOND_TO_SECOND);
    SetAnimationValue(endValue_ + displacement);

    // record fraction to calculate velocity when needed
    prevFraction_ = fraction;
}

std::tuple<std::shared_ptr<RSRenderPropertyBase>, std::shared_ptr<RSRenderPropertyBase>>
RSRenderSpringAnimation::GetSpringStatus()
{
    if (ROSEN_EQ(prevFraction_, 0.0f)) {
        return { startValue_, initialVelocity_ };
    }
    auto displacement = CalculateDisplacement(prevFraction_ * GetDuration() * MILLISECOND_TO_SECOND);
    auto velocity = GetInstantaneousVelocity(prevFraction_ * GetDuration() * MILLISECOND_TO_SECOND);
    return { endValue_ + displacement, velocity };
}
} // namespace Rosen
} // namespace OHOS
