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

#include "modifier/rs_render_property.h"
#include "pipeline/rs_canvas_render_node.h"
#include "platform/common/rs_log.h"
#include "transaction/rs_marshalling_helper.h"
#include "rs_profiler.h"

namespace OHOS {
namespace Rosen {
RSRenderPropertyAnimation::RSRenderPropertyAnimation(AnimationId id, const PropertyId& propertyId,
    const std::shared_ptr<RSRenderPropertyBase>& originValue) : RSRenderAnimation(id), propertyId_(propertyId),
    originValue_(originValue->Clone()), lastValue_(originValue->Clone())
{}

void RSRenderPropertyAnimation::DumpAnimationType(std::string& out) const
{
    out += "Type:RSRenderPropertyAnimation";
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
    if (originValue_ != nullptr) {
        property_->SetPropertyType(originValue_->GetPropertyType());
    }
}

bool RSRenderPropertyAnimation::Marshalling(Parcel& parcel) const
{
    if (!RSRenderAnimation::Marshalling(parcel)) {
        ROSEN_LOGE("RSRenderPropertyAnimation::Marshalling, RenderAnimation failed");
        return false;
    }
    if (!parcel.WriteUint64(propertyId_)) {
        ROSEN_LOGE("RSRenderPropertyAnimation::Marshalling, write PropertyId failed");
        return false;
    }
    if (!(RSMarshallingHelper::Marshalling(parcel, isAdditive_) &&
            RSRenderPropertyBase::Marshalling(parcel, originValue_))) {
        ROSEN_LOGE("RSRenderPropertyAnimation::Marshalling, write value failed");
        return false;
    }
    return true;
}

bool RSRenderPropertyAnimation::ParseParam(Parcel& parcel)
{
    if (!RSRenderAnimation::ParseParam(parcel)) {
        ROSEN_LOGE("RSRenderPropertyAnimation::ParseParam, RenderAnimation failed");
        return false;
    }

    if (!(parcel.ReadUint64(propertyId_) && RSMarshallingHelper::Unmarshalling(parcel, isAdditive_))) {
        ROSEN_LOGE("RSRenderPropertyAnimation::ParseParam, Unmarshalling failed");
        return false;
    }
    RS_PROFILER_PATCH_NODE_ID(parcel, propertyId_);
    if (!RSRenderPropertyBase::Unmarshalling(parcel, originValue_)) {
        return false;
    }
    if (originValue_ == nullptr) {
        ROSEN_LOGE("RSRenderPropertyAnimation::ParseParam, originValue_ is nullptr!");
        return false;
    }
    lastValue_ = originValue_->Clone();

    return true;
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

    return lastValue_->Clone();
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
    if (property_->GetPropertyUnit() > RSPropertyUnit::UNKNOWN) {
        auto currAnimateValue = property_->Clone();
        animateVelocity_ = (currAnimateValue - lastAnimateValue_) * (1 / frameInterval);
    }
}
} // namespace Rosen
} // namespace OHOS
