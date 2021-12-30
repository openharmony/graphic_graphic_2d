/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_CURVE_ANIMATION_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_CURVE_ANIMATION_H

#include "animation/rs_interpolator.h"
#include "animation/rs_render_property_animation.h"
#include "animation/rs_value_estimator.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
template<typename T>
class RSRenderCurveAnimation : public RSRenderPropertyAnimation<T> {
public:
    RSRenderCurveAnimation(AnimationId id, const RSAnimatableProperty& property, const T& originValue,
        const T& startValue, const T& endValue)
        : RSRenderPropertyAnimation<T>(id, property, originValue), startValue_(startValue), endValue_(endValue)
    {}

    virtual ~RSRenderCurveAnimation() = default;

    void SetInterpolator(const std::shared_ptr<RSInterpolator>& interpolator)
    {
        interpolator_ = interpolator;
    }

    const std::shared_ptr<RSInterpolator>& GetInterpolator() const
    {
        return interpolator_;
    }

    void SetValueEstimator(const std::shared_ptr<RSValueEstimator>& valueEstimator)
    {
        valueEstimator_ = valueEstimator;
    }

    const std::shared_ptr<RSValueEstimator>& GetValueEstimator() const
    {
        return valueEstimator_;
    }
#ifdef ROSEN_OHOS
    bool Marshalling(Parcel& parcel) const override
    {
        if (!RSRenderPropertyAnimation<T>::Marshalling(parcel)) {
            return false;
        }
        if (!(RSMarshallingHelper::Marshalling(parcel, startValue_) &&
                RSMarshallingHelper::Marshalling(parcel, endValue_) && interpolator_->Marshalling(parcel))) {
            return false;
        }
        return true;
    }

    static RSRenderCurveAnimation* Unmarshalling(Parcel& parcel)
    {
        RSRenderCurveAnimation* renderCurveAnimation = new RSRenderCurveAnimation<T>();
        if (!renderCurveAnimation->ParseParam(parcel)) {
            ROSEN_LOGE("Unmarshaling RenderCurveAnimation failed");
            delete renderCurveAnimation;
            return nullptr;
        }
        return renderCurveAnimation;
    }
#endif
protected:
    void OnSetFraction(float fraction) override
    {
        OnAnimateInner(fraction, linearInterpolator_);
        RSRenderAnimation::SetFractionInner(valueEstimator_->EstimateFraction(
            interpolator_, RSRenderPropertyAnimation<T>::GetLastValue(), startValue_, endValue_));
    }

    void OnAnimate(float fraction) override
    {
        OnAnimateInner(fraction, interpolator_);
    }

private:
#ifdef ROSEN_OHOS
    bool ParseParam(Parcel& parcel) override
    {
        if (!RSRenderPropertyAnimation<T>::ParseParam(parcel)) {
            ROSEN_LOGE("ParseProperty Fail");
            return false;
        }

        if (!(RSMarshallingHelper::Unmarshalling(parcel, startValue_) &&
                RSMarshallingHelper::Unmarshalling(parcel, endValue_))) {
            ROSEN_LOGE("Unmarshalling value Fail");
            return false;
        }
        std::shared_ptr<RSInterpolator> interpolator(RSInterpolator::Unmarshalling(parcel));
        SetInterpolator(interpolator);
        return true;
    }
#endif
    RSRenderCurveAnimation() = default;
    void OnAnimateInner(float fraction, const std::shared_ptr<RSInterpolator>& interpolator)
    {
        auto interpolationValue =
            valueEstimator_->Estimate(interpolator_->Interpolate(fraction), startValue_, endValue_);
        RSRenderPropertyAnimation<T>::SetAnimationValue(interpolationValue);
    }

    T startValue_ {};
    T endValue_ {};
    std::shared_ptr<RSInterpolator> interpolator_ { RSInterpolator::DEFAULT };
    std::shared_ptr<RSInterpolator> linearInterpolator_ { std::make_shared<LinearInterpolator>() };
    std::shared_ptr<RSValueEstimator> valueEstimator_ { std::make_shared<RSValueEstimator>() };
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_CURVE_ANIMATION_H
