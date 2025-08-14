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

#include "animation/rs_render_property_animation.h"

#include "animation/rs_animation_trace_utils.h"
#include "modifier/rs_render_property.h"
#include "platform/common/rs_log.h"
#include "transaction/rs_marshalling_helper.h"
#include "rs_profiler.h"

namespace OHOS {
namespace Rosen {
RSRenderPropertyAnimation::RSRenderPropertyAnimation(
    AnimationId id, const PropertyId& propertyId, const std::shared_ptr<RSRenderPropertyBase>& originValue)
    : RSRenderAnimation(id), propertyId_(propertyId)
{
    if (originValue) {
        originValue_ = originValue->Clone();
        lastValue_ = originValue->Clone();
    } else {
        originValue_ = std::make_shared<RSRenderAnimatableProperty<float>>();
        lastValue_ = std::make_shared<RSRenderAnimatableProperty<float>>();
    }
}

void RSRenderPropertyAnimation::DumpAnimationInfo(std::string& out) const
{
    out.append("Type:RSRenderPropertyAnimation");
    DumpProperty(out);
}

void RSRenderPropertyAnimation::DumpProperty(std::string& out) const
{
    if (property_ != nullptr) {
#if defined(MODIFIER_NG)
        if (auto modifierNG = property_->GetModifierNG().lock()) {
            out.append(", ModifierType: ").append(
                std::to_string(static_cast<int16_t>(modifierNG->FindPropertyType(property_))));
        } else {
            out.append(", ModifierType: INVALID");
        }
#else
        out.append(", ModifierType: ").append(std::to_string(static_cast<int16_t>(property_->GetModifierType())));
#endif
    } else {
        out.append(", ModifierType: INVALID");
    }
}

PropertyId RSRenderPropertyAnimation::GetPropertyId() const
{
    return propertyId_;
}

void RSRenderPropertyAnimation::SetAdditive(bool isAdditive)
{
    if (IsStarted()) {
        ROSEN_LOGE("Failed to set additive, animation has started!");
        return;
    }

    isAdditive_ = isAdditive;
}

bool RSRenderPropertyAnimation::GetAdditive()
{
    return isAdditive_;
}

void RSRenderPropertyAnimation::AttachRenderProperty(const std::shared_ptr<RSRenderPropertyBase>& property)
{
    property_ = property;
    if (property_ == nullptr) {
        return;
    }
    InitValueEstimator();
}

void RSRenderPropertyAnimation::SetPropertyValue(const std::shared_ptr<RSRenderPropertyBase>& value)
{
    if (property_ != nullptr) {
        property_->SetValue(value);
    }
}

const std::shared_ptr<RSRenderPropertyBase> RSRenderPropertyAnimation::GetPropertyValue() const
{
    if (property_ != nullptr) {
        return property_->Clone();
    }

    if (lastValue_ != nullptr) {
        return lastValue_->Clone();
    }

    return nullptr;
}

const std::shared_ptr<RSRenderPropertyBase>& RSRenderPropertyAnimation::GetOriginValue() const
{
    return originValue_;
}

const std::shared_ptr<RSRenderPropertyBase>& RSRenderPropertyAnimation::GetLastValue() const
{
    return lastValue_;
}

void RSRenderPropertyAnimation::SetAnimationValue(const std::shared_ptr<RSRenderPropertyBase>& value)
{
    if (value == nullptr) {
        ROSEN_LOGD("RSRenderPropertyAnimation::SetAnimationValue, input is null!");
        return;
    }
    std::shared_ptr<RSRenderPropertyBase> animationValue;
    if (GetAdditive() && (property_ != nullptr)) {
        animationValue = property_->Clone() + (value - lastValue_);
        lastValue_ = value->Clone();
    } else {
        animationValue = value->Clone();
        lastValue_ = value->Clone();
    }
    SetPropertyValue(animationValue);
}

const std::shared_ptr<RSRenderPropertyBase> RSRenderPropertyAnimation::GetAnimationValue(
    const std::shared_ptr<RSRenderPropertyBase>& value)
{
    if (value == nullptr) {
        ROSEN_LOGD("RSRenderPropertyAnimation::GetAnimationValue, input is null!");
        return value;
    }
    std::shared_ptr<RSRenderPropertyBase> animationValue;
    if (GetAdditive()) {
        animationValue = GetPropertyValue() + (value - lastValue_);
    } else {
        animationValue = value->Clone();
    }

    lastValue_ = value->Clone();
    return animationValue;
}

void RSRenderPropertyAnimation::OnRemoveOnCompletion()
{
    std::shared_ptr<RSRenderPropertyBase> backwardValue;
    if (GetAdditive()) {
        backwardValue = GetPropertyValue() + (GetOriginValue() - lastValue_);
    } else {
        backwardValue = GetOriginValue();
    }

    SetPropertyValue(backwardValue);
}

void RSRenderPropertyAnimation::RecordLastAnimateValue()
{
    if (!RSRenderAnimation::isCalcAnimateVelocity_) {
        return;
    }
    animateVelocity_.reset();
    lastAnimateValue_.reset();
    if (property_ != nullptr) {
        lastAnimateValue_ = property_->Clone();
    }
}

void RSRenderPropertyAnimation::UpdateAnimateVelocity(float frameInterval)
{
    if (!RSRenderAnimation::isCalcAnimateVelocity_ ||
        !lastAnimateValue_ || !property_ || ROSEN_EQ<float>(frameInterval, 0)) {
        return;
    }

    if (property_->GetPropertyUnit() == RSPropertyUnit::ANGLE_ROTATION) {
        ProcessAnimateVelocityUnderAngleRotation(frameInterval);
        return;
    }

    if (property_->GetPropertyUnit() > RSPropertyUnit::UNKNOWN) {
        auto currAnimateValue = property_->Clone();
        animateVelocity_ = (currAnimateValue - lastAnimateValue_) * (1 / frameInterval);
        ROSEN_LOGD("%{public}s, currAnimateValue: %{public}f, lastAnimateValue: %{public}f, "
            "animateVelocity: %{public}f", __func__, currAnimateValue->ToFloat(),
            lastAnimateValue_->ToFloat(), animateVelocity_->ToFloat());
    }
}

void RSRenderPropertyAnimation::ProcessAnimateVelocityUnderAngleRotation(float frameInterval)
{
    auto currAnimateValue = property_->Clone();
    if (currAnimateValue == nullptr) {
        ROSEN_LOGE("%{public}s, currAnimateValue is null", __func__);
        return;
    }
    float currAnimateFloatValue = currAnimateValue->ToFloat();
    auto diffValue = currAnimateValue - lastAnimateValue_;
    auto diffFloatValue = currAnimateFloatValue - lastAnimateValue_->ToFloat();
    // 150 means 150 angle.
    // The angle of the representation varies greatly between two frames.
    int32_t factor = std::abs(diffFloatValue) / 150;
    if (factor > 0) {
        if (ROSEN_EQ<float>(currAnimateFloatValue, 0)) {
            ROSEN_LOGE("%{public}s, currAnimateFloatValue is 0", __func__);
            return;
        }
        // 180 means 180 angle, relative to pi radian.
        auto circleValue = currAnimateValue * (180 * factor / currAnimateFloatValue);
        diffValue = diffFloatValue < 0 ? (currAnimateValue + circleValue) - lastAnimateValue_
                    : currAnimateValue - (lastAnimateValue_ + circleValue);
    }
    if (ROSEN_EQ<float>(frameInterval, 0)) {
        ROSEN_LOGE("%{public}s, frameInterval is 0", __func__);
        return;
    }
    animateVelocity_ = diffValue * (1 / frameInterval);
    ROSEN_LOGD("%{public}s, currAnimateValue: %{public}f, lastAnimateValue: %{public}f, "
        "diffFloatValue: %{public}f, factor: %{public}d, animateVelocity: %{public}f",
        __func__, currAnimateValue->ToFloat(), lastAnimateValue_->ToFloat(), diffFloatValue,
        factor, animateVelocity_->ToFloat());
}

void RSRenderPropertyAnimation::DumpFraction(float fraction, int64_t time)
{
    RSAnimationTraceUtils::GetInstance().AddAnimationFrameTrace(GetTarget(), GetTargetId(), GetTargetName(),
        GetAnimationId(), GetPropertyId(), fraction, GetPropertyValue(), time, GetDuration(), GetRepeatCount());
}
} // namespace Rosen
} // namespace OHOS
