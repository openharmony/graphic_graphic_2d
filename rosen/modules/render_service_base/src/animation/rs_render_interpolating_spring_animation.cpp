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
#include "command/rs_animation_command.h"
#include "command/rs_message_processor.h"
#include "pipeline/rs_render_node.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr float SECOND_TO_MILLISECOND = 1e3;
constexpr float MILLISECOND_TO_SECOND = 1e-3;
constexpr float FRAME_TIME_INTERVAL = 1.0f / 90.0f;
} // namespace

RSRenderInterpolatingSpringAnimation::RSRenderInterpolatingSpringAnimation(AnimationId id, const PropertyId& propertyId,
    const std::shared_ptr<RSRenderPropertyBase>& originValue, const std::shared_ptr<RSRenderPropertyBase>& startValue,
    const std::shared_ptr<RSRenderPropertyBase>& endValue)
    : RSRenderPropertyAnimation(id, propertyId, originValue), RSSpringModel<float>(), startValue_(startValue),
      endValue_(endValue)
{
    // spring model is not initialized, so we can't calculate estimated duration
}

void RSRenderInterpolatingSpringAnimation::DumpAnimationType(std::string& out) const
{
    out += "Type:RSRenderInterpolatingSpringAnimation";
}

void RSRenderInterpolatingSpringAnimation::SetSpringParameters(
    float response, float dampingRatio, float normalizedInitialVelocity, float minimumAmplitudeRatio)
{
    response_ = response;
    dampingRatio_ = dampingRatio;
    normalizedInitialVelocity_ = normalizedInitialVelocity;
    minimumAmplitudeRatio_ = minimumAmplitudeRatio;
}

void RSRenderInterpolatingSpringAnimation::SetZeroThreshold(float zeroThreshold)
{
    constexpr float ZERO = 0.0f;
    if (zeroThreshold < ZERO) {
        ROSEN_LOGE("RSRenderInterpolatingSpringAnimation::SetZeroThreshold: invalid threshold value.");
        needLogicallyFinishCallback_ = false;
        return;
    }
    zeroThreshold_ = zeroThreshold;
    needLogicallyFinishCallback_ = true;
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
            RSMarshallingHelper::Marshalling(parcel, normalizedInitialVelocity_) &&
            RSMarshallingHelper::Marshalling(parcel, minimumAmplitudeRatio_) &&
            RSMarshallingHelper::Marshalling(parcel, needLogicallyFinishCallback_) &&
            RSMarshallingHelper::Marshalling(parcel, zeroThreshold_))) {
        ROSEN_LOGE("RSRenderInterpolatingSpringAnimation::Marshalling, invalid parametter failed");
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
            RSMarshallingHelper::Unmarshalling(parcel, normalizedInitialVelocity_) &&
            RSMarshallingHelper::Unmarshalling(parcel, minimumAmplitudeRatio_) &&
            RSMarshallingHelper::Unmarshalling(parcel, needLogicallyFinishCallback_) &&
            RSMarshallingHelper::Unmarshalling(parcel, zeroThreshold_))) {
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
    if (valueEstimator_ == nullptr) {
        ROSEN_LOGD("RSRenderInterpolatingSpringAnimation::OnAnimate, valueEstimator_ is nullptr.");
        return;
    }
    if (GetPropertyId() == 0) {
        // calculateAnimationValue_ is embedded modify for stat animate frame drop
        calculateAnimationValue_ = false;
        return;
    } else if (ROSEN_EQ(fraction, 1.0f)) {
        valueEstimator_->UpdateAnimationValue(1.0f, GetAdditive());
        return;
    }
    auto mappedTime = fraction * GetDuration() * MILLISECOND_TO_SECOND;
    float displacement = 1.0f + CalculateDisplacement(mappedTime);
    valueEstimator_->UpdateAnimationValue(displacement, GetAdditive());
    if (GetNeedLogicallyFinishCallback() && (animationFraction_.GetRemainingRepeatCount() == 1)) {
        auto interpolationValue = valueEstimator_->Estimate(displacement, startValue_, endValue_);
        auto endValue = animationFraction_.GetCurrentIsReverseCycle() ? startValue_ : endValue_;
        auto velocity = CalculateVelocity(mappedTime);
        auto zeroValue = startValue_ - startValue_;
        if (!interpolationValue->IsNearEqual(endValue, zeroThreshold_)) {
            return;
        }
        if ((velocity * FRAME_TIME_INTERVAL)->IsNearEqual(zeroValue, zeroThreshold_)) {
            CallLogicallyFinishCallback();
            needLogicallyFinishCallback_ = false;
        }
    }
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

std::shared_ptr<RSRenderPropertyBase> RSRenderInterpolatingSpringAnimation::CalculateVelocity(float time) const
{
    float TIME_INTERVAL = 1e-6f; // 1e-6f : 1 microsecond
    float currentDisplacement = 1.0f + CalculateDisplacement(time);
    float nextDisplacement = 1.0f + CalculateDisplacement(time + TIME_INTERVAL);
    auto velocity = (valueEstimator_->Estimate(nextDisplacement, startValue_, endValue_) -
        valueEstimator_->Estimate(currentDisplacement, startValue_, endValue_)) * (1 / TIME_INTERVAL);

    return velocity;
}

bool RSRenderInterpolatingSpringAnimation::GetNeedLogicallyFinishCallback() const
{
    return needLogicallyFinishCallback_;
}

void RSRenderInterpolatingSpringAnimation::CallLogicallyFinishCallback() const
{
    NodeId targetId = GetTargetId();
    AnimationId animationId = GetAnimationId();

    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCallback>(targetId, animationId, LOGICALLY_FINISHED);
    RSMessageProcessor::Instance().AddUIMessage(ExtractPid(animationId), std::move(command));
}
} // namespace Rosen
} // namespace OHOS