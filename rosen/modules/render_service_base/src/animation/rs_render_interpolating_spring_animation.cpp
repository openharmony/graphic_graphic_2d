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

#include "animation/rs_animation_trace_utils.h"
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

void RSRenderInterpolatingSpringAnimation::DumpAnimationInfo(std::string& out) const
{
    out += "Type:RSRenderInterpolatingSpringAnimation";
    RSPropertyType type = RSPropertyType::INVALID;
    if (property_ != nullptr) {
        type = property_->GetPropertyType();
        out += ", ModifierType: " + std::to_string(static_cast<int16_t>(property_->GetModifierType()));
    } else {
        out += ", ModifierType: INVALID";
    }
    out += ", StartValue: " + RSAnimationTraceUtils::GetInstance().ParseRenderPropertyValue(startValue_);
    out += ", EndValue: " + RSAnimationTraceUtils::GetInstance().ParseRenderPropertyValue(endValue_);
}

void RSRenderInterpolatingSpringAnimation::SetSpringParameters(
    float response, float dampingRatio, float normalizedInitialVelocity, float minimumAmplitudeRatio)
{
    response_ = response;
    dampingRatio_ = std::clamp(dampingRatio, SPRING_MIN_DAMPING_RATIO, SPRING_MAX_DAMPING_RATIO);
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

void RSRenderInterpolatingSpringAnimation::OnSetFraction(float fraction)
{
    if (valueEstimator_ == nullptr) {
        return;
    }
    valueEstimator_->UpdateAnimationValue(fraction, GetAdditive());
    SetValueFraction(fraction);
    fractionChangeInfo_ = { true, fraction };
}

void RSRenderInterpolatingSpringAnimation::UpdateFractionAfterContinue()
{
    auto& [bChangeFraction, valueFraction] = fractionChangeInfo_;
    if (bChangeFraction) {
        SetFractionInner(CalculateTimeFraction(valueFraction));
        bChangeFraction = false;
        valueFraction = 0.0f;
    }
}

float RSRenderInterpolatingSpringAnimation::CalculateTimeFraction(float targetFraction)
{
    int secondTime = std::ceil(static_cast<float>(GetDuration()) / SECOND_TO_MS);
    if (secondTime <= 0) {
        return FRACTION_MIN;
    }
    int64_t frameTimes = MAX_FRAME_TIME_FRACTION * secondTime;
    float lastFraction = FRACTION_MIN;
    for (int time = 1; time <= frameTimes; time++) {
        float frameFraction = static_cast<float>(time) / frameTimes;
        frameFraction = std::clamp(frameFraction, 0.0f, 1.0f);
        auto mappedTime = frameFraction * GetDuration() * MILLISECOND_TO_SECOND;
        float displacement = 1.0f + CalculateDisplacement(mappedTime);
        if (lastFraction <= targetFraction && displacement >= targetFraction) {
            return frameFraction;
        }
        lastFraction = displacement;
    }
    return FRACTION_MIN;
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
    SetValueFraction(displacement);
    valueEstimator_->UpdateAnimationValue(displacement, GetAdditive());
    if (GetNeedLogicallyFinishCallback() && (animationFraction_.GetRemainingRepeatCount() == 1)) {
        auto interpolationValue = valueEstimator_->Estimate(displacement, startValue_, endValue_);
        auto endValue = animationFraction_.GetCurrentIsReverseCycle() ? startValue_ : endValue_;
        auto velocity = CalculateVelocity(mappedTime);
        auto zeroValue = startValue_ - startValue_;
        if (interpolationValue != nullptr && !interpolationValue->IsNearEqual(endValue, zeroThreshold_)) {
            return;
        }
        if (velocity != nullptr && (velocity * FRAME_TIME_INTERVAL)->IsNearEqual(zeroValue, zeroThreshold_)) {
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
    if (valueEstimator_) {
        valueEstimator_->InitCurveAnimationValue(property_, startValue_, endValue_, lastValue_);
    } else {
        ROSEN_LOGE("RSRenderInterpolatingSpringAnimation::InitValueEstimator, valueEstimator_ is nullptr.");
    }
}

std::shared_ptr<RSRenderPropertyBase> RSRenderInterpolatingSpringAnimation::CalculateVelocity(float time) const
{
    if (valueEstimator_ == nullptr) {
        ROSEN_LOGE("RSRenderInterpolatingSpringAnimation::CalculateVelocity, valueEstimator_ is nullptr.");
        return nullptr;
    }
    constexpr float TIME_INTERVAL = 1e-6f; // 1e-6f : 1 microsecond
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
    uint64_t token = GetToken();

    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCallback>(targetId, animationId, token, LOGICALLY_FINISHED);
    RSMessageProcessor::Instance().AddUIMessage(ExtractPid(animationId), std::move(command));
}
} // namespace Rosen
} // namespace OHOS