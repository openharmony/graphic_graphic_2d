/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "animation/rs_render_interpolating_spring_animation.h"

#include "animation/rs_value_estimator.h"
#include "pipeline/rs_render_node.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr float SECOND_TO_MILLISECOND = 1e3;
constexpr float MILLISECOND_TO_SECOND = 1e-3;
} // namespace

RSRenderInterpolatingSpringAnimation::RSRenderInterpolatingSpringAnimation(AnimationId id, const PropertyId& propertyId,
    const std::shared_ptr<RSRenderPropertyBase>& originValue, const std::shared_ptr<RSRenderPropertyBase>& startValue,
    const std::shared_ptr<RSRenderPropertyBase>& endValue)
    : RSRenderPropertyAnimation(id, propertyId, originValue), RSSpringModel<float>(), startValue_(startValue),
      endValue_(endValue)
{
    // spring model is not initialized, so we can't calculate estimated duration
}

void RSRenderInterpolatingSpringAnimation::SetSpringParameters(
    float response, float dampingRatio, float normalizedInitialVelocity)
{
    response_ = response;
    dampingRatio_ = dampingRatio;
    normalizedInitialVelocity_ = normalizedInitialVelocity;
}

#ifdef ROSEN_OHOS
bool RSRenderInterpolatingSpringAnimation::Marshalling(Parcel& parcel) const
{
    if (!RSRenderPropertyAnimation::Marshalling(parcel)) {
        ROSEN_LOGE("RSRenderInterpolatingSpringAnimation::Marshalling, RenderPropertyAnimation failed");
        return false;
    }
    if (!(RSRenderPropertyBase::Marshalling(parcel, startValue_) &&
            RSRenderPropertyBase::Marshalling(parcel, endValue_))) {
        ROSEN_LOGE("RSRenderInterpolatingSpringAnimation::Marshalling, MarshallingHelper failed");
        return false;
    }

    if (!(RSMarshallingHelper::Marshalling(parcel, response_) &&
            RSMarshallingHelper::Marshalling(parcel, dampingRatio_) &&
            RSMarshallingHelper::Marshalling(parcel, normalizedInitialVelocity_))) {
        return false;
    }

    return true;
}

RSRenderInterpolatingSpringAnimation* RSRenderInterpolatingSpringAnimation::Unmarshalling(Parcel& parcel)
{
    auto* renderInterpolatingSpringAnimation = new RSRenderInterpolatingSpringAnimation();
    if (!renderInterpolatingSpringAnimation->ParseParam(parcel)) {
        ROSEN_LOGE("RenderInterpolatingSpringAnimation::Unmarshalling, failed");
        delete renderInterpolatingSpringAnimation;
        return nullptr;
    }
    return renderInterpolatingSpringAnimation;
}

bool RSRenderInterpolatingSpringAnimation::ParseParam(Parcel& parcel)
{
    if (!RSRenderPropertyAnimation::ParseParam(parcel)) {
        ROSEN_LOGE("RSRenderInterpolatingSpringAnimation::ParseParam, ParseParam Fail");
        return false;
    }

    if (!(RSRenderPropertyBase::Unmarshalling(parcel, startValue_) &&
            RSRenderPropertyBase::Unmarshalling(parcel, endValue_))) {
        return false;
    }

    if (!(RSMarshallingHelper::Unmarshalling(parcel, response_) &&
            RSMarshallingHelper::Unmarshalling(parcel, dampingRatio_) &&
            RSMarshallingHelper::Unmarshalling(parcel, normalizedInitialVelocity_))) {
        return false;
    }

    return true;
}
#endif

void RSRenderInterpolatingSpringAnimation::OnSetFraction(float fraction)
{
    // interpolating spring animation should not support set fraction
    OnAnimate(fraction);
}

void RSRenderInterpolatingSpringAnimation::OnAnimate(float fraction)
{
    if (GetPropertyId() == 0) {
        return;
    } else if (ROSEN_EQ(fraction, 1.0f)) {
        valueEstimator_->UpdateAnimationValue(1.0f, GetAdditive());
        return;
    }
    auto mappedTime = fraction * GetDuration() * MILLISECOND_TO_SECOND;
    float displacement = 1.0f + CalculateDisplacement(mappedTime);
    if (valueEstimator_ == nullptr) {
        return;
    }
    valueEstimator_->UpdateAnimationValue(displacement, GetAdditive());
}

void RSRenderInterpolatingSpringAnimation::OnInitialize(int64_t time)
{
    // set the initial status of spring model
    initialOffset_ = -1.0f;
    initialVelocity_ = initialOffset_ * (-normalizedInitialVelocity_);
    CalculateSpringParameters();
    // use duration calculated by spring model as animation duration
    SetDuration(std::lroundf(EstimateDuration() * SECOND_TO_MILLISECOND));
    // this will set needInitialize_ to false
    RSRenderPropertyAnimation::OnInitialize(time);
}

void RSRenderInterpolatingSpringAnimation::InitValueEstimator()
{
    if (valueEstimator_ == nullptr) {
        valueEstimator_ = property_->CreateRSValueEstimator(RSValueEstimatorType::CURVE_VALUE_ESTIMATOR);
    }
    valueEstimator_->InitCurveAnimationValue(property_, startValue_, endValue_, lastValue_);
}
} // namespace Rosen
} // namespace OHOS