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
    : RSRenderPropertyAnimation(id, propertyId, originValue), RSSpringModel<std::shared_ptr<RSRenderPropertyBase>>(),
    startValue_(startValue), endValue_(endValue)
{
    // spring model is not initialized, so we can't calculate estimated duration
}

void RSRenderSpringAnimation::SetSpringParameters(float response, float dampingRatio,
    std::shared_ptr<RSRenderPropertyBase> initialVelocity)
{
    response_ = response;
    dampingRatio_ = dampingRatio;
    initialVelocity_ = initialVelocity;
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
            RSRenderPropertyBase::Marshalling(parcel, initialVelocity_))) {
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
            RSRenderPropertyBase::Unmarshalling(parcel, endValue_) &&
            RSRenderPropertyBase::Unmarshalling(parcel, initialVelocity_))) {
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
    auto prevAnimationId = target->GetAnimationManager().QuerySpringAnimation(propertyId);
    target->GetAnimationManager().RegisterSpringAnimation(propertyId, GetAnimationId());
    auto prevAnimation = target->GetAnimationManager().GetAnimation(prevAnimationId);
    if (prevAnimation == nullptr) {
        UpdateSpringParameters();
        return;
    }

    // extract spring status from previous spring animation
    auto prevSpringAnimation = std::static_pointer_cast<RSRenderSpringAnimation>(prevAnimation);
    auto status = prevSpringAnimation->GetSpringStatus();
    // inherit spring position
    startValue_ = std::get<0>(status);
    originValue_ = startValue_;
    lastValue_ = startValue_;
    // inherit spring velocity
    initialVelocity_ = std::get<1>(status);

    // re-calculate spring parameters and duration
    UpdateSpringParameters();

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

void RSRenderSpringAnimation::UpdateSpringParameters()
{
    initialOffset_ = startValue_->GetValue() - endValue_;
    CalculateSpringParameters();
    SetDuration(GetEstimatedDuration() * SECOND_TO_MILLISECOND);
}

void RSRenderSpringAnimation::OnAnimateInner(float fraction)
{
    if (GetPropertyId() == 0) {
        return;
    }
    // always record fraction from previous iterator, will be used to calculate velocity
    prevFraction_ = fraction;
    auto displacement = CalculateDisplacement(
        fraction * GetDuration() * MILLISECOND_TO_SECOND);
    SetAnimationValue(endValue_->GetValue() + displacement);
}

std::tuple<std::shared_ptr<RSRenderPropertyBase>,
        std::shared_ptr<RSRenderPropertyBase>> RSRenderSpringAnimation::GetSpringStatus()
{
    auto displacement = CalculateDisplacement(
        prevFraction_ * GetDuration() * MILLISECOND_TO_SECOND);
    auto velocity = GetInstantaneousVelocity(
        prevFraction_ * GetDuration() * MILLISECOND_TO_SECOND);
    return std::make_tuple(endValue_->GetValue() + displacement, velocity);
}
} // namespace Rosen
} // namespace OHOS
