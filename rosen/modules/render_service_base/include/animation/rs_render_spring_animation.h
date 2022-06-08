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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_SPRING_ANIMATION_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_SPRING_ANIMATION_H

#include "animation/rs_render_property_animation.h"
#include "animation/rs_spring_model.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr float SECOND_TO_MILLISECOND = 1e3;
constexpr float MILLISECOND_TO_SECOND = 1e-3;
}
template<typename T>
class RSRenderSpringAnimation : public RSRenderPropertyAnimation<T>, public RSSpringModel<T> {
public:
    explicit RSRenderSpringAnimation(AnimationId id, const RSAnimatableProperty& property, const T& originValue,
        const T& startValue, const T& endValue)
        : RSRenderPropertyAnimation<T>(id, property, originValue),
          RSSpringModel<T>(),
          startValue_(startValue), endValue_(endValue)
    {
        // spring model is not initialized, so we can't calculate estimated duration
    }

    void SetSpringParameters(float response, float dampingRatio, T initialVelocity = {})
    {
        RSSpringModel<T>::response_ = response;
        RSSpringModel<T>::dampingRatio_ = dampingRatio;
        RSSpringModel<T>::initialVelocity_ = initialVelocity;
        RSSpringModel<T>::initialOffset_ = endValue_ - startValue_;

        RSSpringModel<T>::CalculateSpringParameters();
        RSRenderAnimation::SetDuration(RSSpringModel<T>::GetEstimatedDuration() * SECOND_TO_MILLISECOND);
    }

    virtual ~RSRenderSpringAnimation() = default;

#ifdef ROSEN_OHOS
    bool Marshalling(Parcel& parcel) const override
    {
        if (!RSRenderPropertyAnimation<T>::Marshalling(parcel)) {
            ROSEN_LOGE("RSRenderSpringAnimation::Marshalling, RenderPropertyAnimation failed");
            return false;
        }
        if (!(RSMarshallingHelper::Marshalling(parcel, startValue_) &&
                RSMarshallingHelper::Marshalling(parcel, endValue_) &&
                RSMarshallingHelper::Marshalling(parcel, RSSpringModel<T>::response_) &&
                RSMarshallingHelper::Marshalling(parcel, RSSpringModel<T>::dampingRatio_) &&
                RSMarshallingHelper::Marshalling(parcel, RSSpringModel<T>::initialVelocity_))) {
            ROSEN_LOGE("RSRenderSpringAnimation::Marshalling, MarshallingHelper failed");
            return false;
        }
        return true;
    }

    static RSRenderSpringAnimation* Unmarshalling(Parcel& parcel)
    {
        RSRenderSpringAnimation* renderSpringAnimation = new RSRenderSpringAnimation<T>();
        if (!renderSpringAnimation->ParseParam(parcel)) {
            ROSEN_LOGE("RSRenderSpringAnimation::Unmarshalling, failed");
            delete renderSpringAnimation;
            return nullptr;
        }
        return renderSpringAnimation;
    }
#endif
protected:
    void OnSetFraction(float fraction) override
    {
        // spring animation should not support set fraction
        OnAnimateInner(fraction);
    }

    void OnAnimate(float fraction) override
    {
        OnAnimateInner(fraction);
    }

private:
#ifdef ROSEN_OHOS
    bool ParseParam(Parcel& parcel) override
    {
        if (!RSRenderPropertyAnimation<T>::ParseParam(parcel)) {
            ROSEN_LOGE("RSRenderSpringAnimation::ParseParam, ParseParam Fail");
            return false;
        }

        if (!(RSMarshallingHelper::Unmarshalling(parcel, startValue_) &&
                RSMarshallingHelper::Unmarshalling(parcel, endValue_) &&
                RSMarshallingHelper::Unmarshalling(parcel, RSSpringModel<T>::response_) &&
                RSMarshallingHelper::Unmarshalling(parcel, RSSpringModel<T>::dampingRatio_) &&
                RSMarshallingHelper::Unmarshalling(parcel, RSSpringModel<T>::initialVelocity_))) {
            ROSEN_LOGE("RSRenderSpringAnimation::ParseParam, Unmarshalling Fail");
            return false;
        }

        RSSpringModel<T>::initialOffset_ = endValue_ - startValue_;
        RSSpringModel<T>::CalculateSpringParameters();
        RSRenderAnimation::SetDuration(RSSpringModel<T>::GetEstimatedDuration() * SECOND_TO_MILLISECOND);

        return true;
    }
#endif
    RSRenderSpringAnimation() = default;
    void OnAnimateInner(float fraction)
    {
        if (RSRenderPropertyAnimation<T>::GetProperty() == RSAnimatableProperty::INVALID) {
            return;
        }
        auto displacement = RSSpringModel<T>::CalculateDisplacement(fraction * RSRenderAnimation::GetDuration() * MILLISECOND_TO_SECOND);
        RSRenderPropertyAnimation<T>::SetAnimationValue(startValue_ + displacement);
    }

    T startValue_ {};
    T endValue_ {};
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_SPRING_ANIMATION_H
