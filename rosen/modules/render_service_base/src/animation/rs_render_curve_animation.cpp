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

#include "animation/rs_render_curve_animation.h"

#include "animation/rs_value_estimator.h"
#include "platform/common/rs_log.h"
#include "transaction/rs_marshalling_helper.h"

namespace OHOS {
namespace Rosen {
RSRenderCurveAnimation::RSRenderCurveAnimation(AnimationId id, const PropertyId& propertyId,
    const std::shared_ptr<RSRenderPropertyBase>& originValue, const std::shared_ptr<RSRenderPropertyBase>& startValue,
    const std::shared_ptr<RSRenderPropertyBase>& endValue) : RSRenderPropertyAnimation(id, propertyId, originValue),
    startValue_(startValue), endValue_(endValue)
{}

void RSRenderCurveAnimation::DumpAnimationType(std::string& out) const
{
    out += "Type:RSRenderCurveAnimation";
}

void RSRenderCurveAnimation::SetInterpolator(const std::shared_ptr<RSInterpolator>& interpolator)
{
    interpolator_ = interpolator;
}

const std::shared_ptr<RSInterpolator>& RSRenderCurveAnimation::GetInterpolator() const
{
    return interpolator_;
}

bool RSRenderCurveAnimation::Marshalling(Parcel& parcel) const
{
    if (!RSRenderPropertyAnimation::Marshalling(parcel)) {
        ROSEN_LOGE("RSRenderCurveAnimation::Marshalling, RenderPropertyAnimation failed");
        return false;
    }
    if (!(RSRenderPropertyBase::Marshalling(parcel, startValue_) &&
            RSRenderPropertyBase::Marshalling(parcel, endValue_) && interpolator_ != nullptr &&
            interpolator_->Marshalling(parcel))) {
        ROSEN_LOGE("RSRenderCurveAnimation::Marshalling, MarshallingHelper failed");
        return false;
    }
    return true;
}

RSRenderCurveAnimation* RSRenderCurveAnimation::Unmarshalling(Parcel& parcel)
{
    RSRenderCurveAnimation* renderCurveAnimation = new RSRenderCurveAnimation();
    if (!renderCurveAnimation->ParseParam(parcel)) {
        ROSEN_LOGE("RSRenderCurveAnimation::Unmarshalling, failed");
        delete renderCurveAnimation;
        return nullptr;
    }
    return renderCurveAnimation;
}

bool RSRenderCurveAnimation::ParseParam(Parcel& parcel)
{
    if (!RSRenderPropertyAnimation::ParseParam(parcel)) {
        ROSEN_LOGE("RSRenderCurveAnimation::ParseParam, ParseParam Fail");
        return false;
    }

    if (!(RSRenderPropertyBase::Unmarshalling(parcel, startValue_) &&
            RSRenderPropertyBase::Unmarshalling(parcel, endValue_))) {
        ROSEN_LOGE("RSRenderCurveAnimation::ParseParam, Unmarshalling Fail");
        return false;
    }

    std::shared_ptr<RSInterpolator> interpolator(RSInterpolator::Unmarshalling(parcel));
    if (interpolator == nullptr) {
        ROSEN_LOGE("RSRenderCurveAnimation::ParseParam, Unmarshalling interpolator failed");
        return false;
    }
    SetInterpolator(interpolator);
    return true;
}

void RSRenderCurveAnimation::OnSetFraction(float fraction)
{
    if (valueEstimator_ == nullptr) {
        return;
    }
    valueEstimator_->UpdateAnimationValue(fraction, GetAdditive());
    SetValueFraction(fraction);
    fractionChangeInfo_ = { true, fraction };
}

void RSRenderCurveAnimation::UpdateFractionAfterContinue()
{
    auto& [bChangeFraction, valueFraction] = fractionChangeInfo_;
    if (valueEstimator_ != nullptr && bChangeFraction) {
        SetFractionInner(valueEstimator_->EstimateFraction(interpolator_, valueFraction, GetDuration()));
        bChangeFraction = false;
        valueFraction = 0.0f;
    }
}

void RSRenderCurveAnimation::OnAnimate(float fraction)
{
    OnAnimateInner(fraction, interpolator_);
}

void RSRenderCurveAnimation::OnAnimateInner(float fraction, const std::shared_ptr<RSInterpolator>& interpolator)
{
    if (GetPropertyId() == 0) {
        // calculateAnimationValue_ is embedded modify for stat animate frame drop
        calculateAnimationValue_ = false;
        return;
    }

    if (valueEstimator_ == nullptr || interpolator == nullptr) {
        return;
    }
    auto interpolatorValue = interpolator->Interpolate(fraction);
    SetValueFraction(interpolatorValue);
    valueEstimator_->UpdateAnimationValue(interpolatorValue, GetAdditive());
}

void RSRenderCurveAnimation::InitValueEstimator()
{
    if (valueEstimator_ == nullptr) {
        valueEstimator_ = property_->CreateRSValueEstimator(RSValueEstimatorType::CURVE_VALUE_ESTIMATOR);
    }
    if (valueEstimator_ == nullptr) {
        ROSEN_LOGE("RSRenderCurveAnimation::InitValueEstimator, valueEstimator_ is nullptr");
        return;
    }
    valueEstimator_->InitCurveAnimationValue(property_, startValue_, endValue_, lastValue_);
}
} // namespace Rosen
} // namespace OHOS
