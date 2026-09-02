/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "animation/rs_render_path_animation.h"

#include "animation/rs_animation_trace_utils.h"
#include "animation/rs_value_estimator.h"
#include "pipeline/rs_canvas_render_node.h"
#include "platform/common/rs_log.h"
#include "render/rs_path.h"
#include "rs_profiler.h"

namespace OHOS {
namespace Rosen {
RSRenderPathAnimation::RSRenderPathAnimation(AnimationId id, const PropertyId& propertyId,
    const std::shared_ptr<RSRenderPropertyBase>& originPosition,
    const std::shared_ptr<RSRenderPropertyBase>& startPosition,
    const std::shared_ptr<RSRenderPropertyBase>& endPosition, float originRotation,
    const std::shared_ptr<RSPath>& animationPath) : RSRenderPropertyAnimation(id, propertyId, originPosition),
    originRotation_(originRotation), startValue_(startPosition), endValue_(endPosition),
    animationPath_(animationPath)
{}

void RSRenderPathAnimation::DumpAnimationInfo(std::string& out) const
{
    out.append("Type:RSRenderPathAnimation");
}

void RSRenderPathAnimation::SetInterpolator(const std::shared_ptr<RSInterpolator>& interpolator)
{
    interpolator_ = interpolator;
}

const std::shared_ptr<RSInterpolator>& RSRenderPathAnimation::GetInterpolator() const
{
    return interpolator_;
}

void RSRenderPathAnimation::SetRotationMode(const RotationMode& rotationMode)
{
    if (IsStarted()) {
        ROSEN_LOGE("Failed to enable rotate, path animation has started!");
        return;
    }

    rotationMode_ = rotationMode;
}

RotationMode RSRenderPathAnimation::GetRotationMode() const
{
    return rotationMode_;
}

void RSRenderPathAnimation::SetBeginFraction(float fraction)
{
    if (IsStarted()) {
        ROSEN_LOGE("Failed to set begin fraction, path animation has started!");
        return;
    }

    if (fraction < FRACTION_MIN || fraction > FRACTION_MAX || fraction > endFraction_) {
        ROSEN_LOGE("Failed to set begin fraction, invalid value:%{public}f", fraction);
        return;
    }

    beginFraction_ = fraction;
}

float RSRenderPathAnimation::GetBeginFraction() const
{
    return beginFraction_;
}

void RSRenderPathAnimation::SetEndFraction(float fraction)
{
    if (IsStarted()) {
        ROSEN_LOGE("Failed to set end fraction, path animation has started!");
        return;
    }

    if (fraction < FRACTION_MIN || fraction > FRACTION_MAX || fraction < beginFraction_) {
        ROSEN_LOGE("Failed to set end fraction, invalid value:%{public}f", fraction);
        return;
    }

    endFraction_ = fraction;
}

float RSRenderPathAnimation::GetEndFraction() const
{
    return endFraction_;
}

void RSRenderPathAnimation::SetIsNeedPath(const bool isNeedPath)
{
    isNeedPath_ = isNeedPath;
}

void RSRenderPathAnimation::SetPathNeedAddOrigin(bool needAddOrigin)
{
    if (IsStarted()) {
        ROSEN_LOGE("Failed to set need Add Origin, path animation has started!");
        return;
    }

    needAddOrigin_ = needAddOrigin;
}

void RSRenderPathAnimation::SetRotationId(const PropertyId id)
{
    rotationId_ = id;
}

bool RSRenderPathAnimation::OnAnimate(float fraction)
{
    if (animationPath_ == nullptr || property_ == nullptr) {
        ROSEN_LOGE("Failed to animate motion path, path or property is null!");
        return false;
    }

    Vector2f position;
    float tangent = 0.0f;
    auto propertyType = property_->GetPropertyType();
    if (propertyType == RSPropertyType::VECTOR2F) {
        GetPosTanValue(fraction, position, tangent);
        UpdateVector2fPathValue(position);
        SetPathValue(position, tangent);
    } else if (propertyType == RSPropertyType::VECTOR4F) {
        if (valueEstimator_ == nullptr || interpolator_ == nullptr) {
            ROSEN_LOGE("RSRenderPathAnimation::OnAnimate failed, valueEstimator or interpolator is null");
            return false;
        }
        if (!isNeedPath_) {
            valueEstimator_->UpdateAnimationValue(interpolator_->Interpolate(fraction), GetAdditive());
            return false;
        }
        if (valueEstimator_->GetEstimatorType() != RSValueEstimatorType::CURVE_VALUE_ESTIMATOR) {
            ROSEN_LOGE("RSRenderPathAnimation::OnAnimate failed, invalid valueEstimator type");
            return false;
        }
        GetPosTanValue(fraction, position, tangent);
        auto vector4fValueEstimator = std::static_pointer_cast<RSCurveValueEstimator<Vector4f>>(valueEstimator_);
        auto animationValue =
            vector4fValueEstimator->GetAnimationValue(interpolator_->Interpolate(fraction), GetAdditive());
        UpdateVector4fPathValue(animationValue, position);
        SetPathValue(animationValue, tangent);
    } else {
        ROSEN_LOGE("RSRenderPathAnimation::OnAnimate failed, unsupported property type:%{public}d",
            static_cast<int>(propertyType));
    }
    return false;
}

void RSRenderPathAnimation::RebuildPropertyValue(float fraction)
{
    if (GetOriginValue() == nullptr || interpolator_ == nullptr || valueEstimator_ == nullptr) {
        ROSEN_LOGE("RSRenderPathAnimation::RebuildPropertyValue failed: "
            "originValue[%{public}d] interpolator[%{public}d] valueEstimator[%{public}d]",
            GetOriginValue() != nullptr, interpolator_ != nullptr, valueEstimator_ != nullptr);
        return;
    }
    if (property_->GetPropertyType() != RSPropertyType::VECTOR4F) {
        return;
    }
    auto interpolatedFraction = interpolator_->Interpolate(fraction);
    valueEstimator_->RebuildValue(interpolatedFraction);
}

void RSRenderPathAnimation::OnRemoveOnCompletion()
{
    auto target = GetTarget();
    if (target == nullptr) {
        ROSEN_LOGE("Failed to remove on completion, target is null!");
        return;
    }

    target->GetMutableRenderProperties().SetRotation(originRotation_);
    RSRenderPropertyAnimation::OnRemoveOnCompletion();
}

void RSRenderPathAnimation::OnAttach()
{
    auto target = GetTarget();
    if (target == nullptr) {
        ROSEN_LOGE("RSRenderPathAnimation::OnAttach, target is nullptr");
        return;
    }
    // check if any other path animation running on this property
    auto propertyId = GetPropertyId();
    auto animationManager = target->GetAnimationManager();
    if (!animationManager) {
        return;
    }
    auto prevAnimation = animationManager->QueryPathAnimation(propertyId);
    animationManager->RegisterPathAnimation(propertyId, GetAnimationId());

    // return if no other path animation(s) running
    if (prevAnimation == nullptr) {
        return;
    }

    // set previous path animation to FINISHED
    prevAnimation->Finish();
}

void RSRenderPathAnimation::OnDetach()
{
    auto target = GetTarget();
    if (target == nullptr) {
        ROSEN_LOGE("RSRenderPathAnimation::OnDetach, target is nullptr");
        return;
    }
    auto propertyId = GetPropertyId();
    auto id = GetAnimationId();
    if (auto animationManager = target->GetAnimationManager()) {
        animationManager->UnregisterPathAnimation(propertyId, id);
    }
}

void RSRenderPathAnimation::SetPathValue(const Vector2f& value, float tangent)
{
    SetRotationValue(tangent);
    auto animatableProperty = property_ ?
        property_->CastToAnimatablePropertyOf<Vector2f>(__func__) : nullptr;
    if (animatableProperty != nullptr) {
        animatableProperty->Set(value);
    }
}

void RSRenderPathAnimation::SetPathValue(const Vector4f& value, float tangent)
{
    SetRotationValue(tangent);
    auto animatableProperty = property_ ?
        property_->CastToAnimatablePropertyOf<Vector4f>(__func__) : nullptr;
    if (animatableProperty != nullptr) {
        animatableProperty->Set(value);
    }
}

void RSRenderPathAnimation::SetRotationValue(const float tangent)
{
    switch (GetRotationMode()) {
        case RotationMode::ROTATE_AUTO:
            SetRotation(tangent);
            break;
        case RotationMode::ROTATE_AUTO_REVERSE:
            SetRotation(tangent + 180.0f);
            break;
        case RotationMode::ROTATE_NONE:
            break;
        default:
            ROSEN_LOGE("Unknow rotate mode!");
            break;
    }
}

void RSRenderPathAnimation::SetRotation(const float tangent)
{
    auto target = GetTarget();
    if (target == nullptr) {
        ROSEN_LOGD("Failed to set rotation value, target is null!");
        return;
    }

    auto baseProperty = target->GetProperty(rotationId_);
    auto property = baseProperty ?
        baseProperty->CastToPropertyOf<float>(__func__) : nullptr;
    if (property != nullptr) {
        property->Set(tangent);
    }
}

void RSRenderPathAnimation::GetPosTanValue(float fraction, Vector2f& position, float& tangent)
{
    float distance = animationPath_->GetDistance();
    float progress = GetBeginFraction() * (FRACTION_MAX - fraction) + GetEndFraction() * fraction;
    animationPath_->GetPosTan(distance * progress, position, tangent);
}

void RSRenderPathAnimation::UpdateVector2fPathValue(Vector2f& value)
{
    if (needAddOrigin_) {
        auto originValue = GetOriginValue();
        auto animatableProperty = originValue ?
            originValue->CastToAnimatablePropertyOf<Vector2f>(__func__) : nullptr;
        if (animatableProperty) {
            value += animatableProperty->Get();
        }
    }
}

void RSRenderPathAnimation::UpdateVector4fPathValue(Vector4f& value, const Vector2f& position)
{
    value[0] = position[0];
    value[1] = position[1];
    if (needAddOrigin_) {
        auto originValue = GetOriginValue();
        auto animatableProperty = originValue ?
            originValue->CastToAnimatablePropertyOf<Vector4f>(__func__) : nullptr;
        if (animatableProperty) {
            value[0] += animatableProperty->Get()[0];
            value[1] += animatableProperty->Get()[1];
        }
    }
}

void RSRenderPathAnimation::InitValueEstimator()
{
    if (valueEstimator_ == nullptr) {
        valueEstimator_ = property_->CreateRSValueEstimator(RSValueEstimatorType::CURVE_VALUE_ESTIMATOR);
    }

    if (valueEstimator_ == nullptr) {
        ROSEN_LOGE("RSRenderPathAnimation::InitValueEstimator, valueEstimator_ is nullptr.");
        return;
    }
    valueEstimator_->InitCurveAnimationValue(property_, startValue_, endValue_, lastValue_);
}
} // namespace Rosen
} // namespace OHOS
