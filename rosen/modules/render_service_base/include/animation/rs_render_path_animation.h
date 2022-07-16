/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_PATH_ANIMATION_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_PATH_ANIMATION_H

#include "animation/rs_interpolator.h"
#include "animation/rs_render_property_animation.h"

namespace OHOS {
namespace Rosen {
template<typename T>
class RSRenderPathAnimation : public RSRenderPropertyAnimation<T> {
static constexpr float UNDEFINED_FLOAT = 0.0f;
public:
    RSRenderPathAnimation(AnimationId id, const PropertyId& propertyId, const T& originPosition,
        const T& startPosition, const T& endPosition, float originRotation,
        const std::shared_ptr<RSPath>& animationPath) : RSRenderPropertyAnimation<T>(id, propertyId, originPosition),
        originRotation_(originRotation), startValue_(startPosition), endValue_(endPosition),
        animationPath_(animationPath)
    {}

    ~RSRenderPathAnimation() = default;

    void SetInterpolator(const std::shared_ptr<RSInterpolator>& interpolator)
    {
        interpolator_ = interpolator;
    }

    const std::shared_ptr<RSInterpolator>& GetInterpolator() const
    {
        return interpolator_;
    }

    void SetRotationMode(const RotationMode& rotationMode)
    {
        if (RSRenderAnimation::IsStarted()) {
            ROSEN_LOGE("Failed to enable rotate, path animation has started!");
            return;
        }

        rotationMode_ = rotationMode;
    }

    RotationMode GetRotationMode() const
    {
        return rotationMode_;
    }

    void SetBeginFraction(float fraction)
    {
        if (RSRenderAnimation::IsStarted()) {
            ROSEN_LOGE("Failed to set begin fraction, path animation has started!");
            return;
        }

        if (fraction < FRACTION_MIN || fraction > FRACTION_MAX || fraction > endFraction_) {
            ROSEN_LOGE("Failed to set begin fraction, invalid value:%f", fraction);
            return;
        }

        beginFraction_ = fraction;
    }

    float GetBeginFraction() const
    {
        return beginFraction_;
    }

    void SetEndFraction(float fraction)
    {
        if (RSRenderAnimation::IsStarted()) {
            ROSEN_LOGE("Failed to set end fraction, path animation has started!");
            return;
        }

        if (fraction < FRACTION_MIN || fraction > FRACTION_MAX || fraction < beginFraction_) {
            ROSEN_LOGE("Failed to set end fraction, invalid value:%f", fraction);
            return;
        }

        endFraction_ = fraction;
    }

    float GetEndFraction() const
    {
        return endFraction_;
    }

    void SetIsNeedPath(const bool isNeedPath)
    {
        isNeedPath_ = isNeedPath;
    }

    void SetPathNeedAddOrigin(bool needAddOrigin)
    {
        if (RSRenderAnimation::IsStarted()) {
            ROSEN_LOGE("Failed to set need Add Origin, path animation has started!");
            return;
        }

        needAddOrigin_ = needAddOrigin;
    }

    void SetRotationId(const PropertyId id)
    {
        rotationId_ = id;
    }

#ifdef ROSEN_OHOS
    bool Marshalling(Parcel& parcel) const override
    {
        if (!RSRenderPropertyAnimation<T>::Marshalling(parcel)) {
            ROSEN_LOGE("RSRenderPathAnimation::Marshalling, RenderPropertyAnimation failed");
            return false;
        }
        if (!(parcel.WriteFloat(originRotation_) && parcel.WriteFloat(beginFraction_) &&
                parcel.WriteFloat(endFraction_) && RSMarshallingHelper::Marshalling(parcel, *animationPath_) &&
                RSMarshallingHelper::Marshalling(parcel, startValue_) &&
                RSMarshallingHelper::Marshalling(parcel, endValue_) &&
                parcel.WriteInt32(static_cast<std::underlying_type<RotationMode>::type>(rotationMode_)) &&
                parcel.WriteBool(isNeedPath_) && interpolator_->Marshalling(parcel))) {
            ROSEN_LOGE("RSRenderPathAnimation::Marshalling, write failed");
            return false;
        }
        return true;
    }

    static RSRenderPathAnimation* Unmarshalling(Parcel& parcel)
    {
        RSRenderPathAnimation* renderPathAnimation = new RSRenderPathAnimation<T>();

        if (!renderPathAnimation->ParseParam(parcel)) {
            ROSEN_LOGE("RSRenderPathAnimation::Unmarshalling, Parse RenderProperty Fail");
            delete renderPathAnimation;
            return nullptr;
        }
        return renderPathAnimation;
    }
#endif

protected:
    void OnAnimate(float fraction) override
    {}

    void OnRemoveOnCompletion() override
    {
        auto target = RSRenderAnimation::GetTarget();
        if (target == nullptr) {
            ROSEN_LOGE("Failed to remove on completion, target is null!");
            return;
        }

        target->GetMutableRenderProperties().SetRotation(originRotation_);
        RSRenderPropertyAnimation<T>::OnRemoveOnCompletion();
    }

private:
#ifdef ROSEN_OHOS
    bool ParseParam(Parcel& parcel) override
    {
        if (!RSRenderPropertyAnimation<T>::ParseParam(parcel)) {
            ROSEN_LOGE("RSRenderPathAnimation::ParseParam, Parse RenderProperty Fail");
            return false;
        }

        int32_t rotationMode;
        std::shared_ptr<RSPath> path;
        bool isNeedPath = true;
        if (!(parcel.ReadFloat(originRotation_) && parcel.ReadFloat(beginFraction_) &&
                parcel.ReadFloat(endFraction_) && RSMarshallingHelper::Unmarshalling(parcel, path) &&
                RSMarshallingHelper::Unmarshalling(parcel, startValue_) &&
                RSMarshallingHelper::Unmarshalling(parcel, endValue_) && parcel.ReadInt32(rotationMode) &&
                parcel.ReadBool(isNeedPath))) {
            ROSEN_LOGE("RSRenderPathAnimation::ParseParam, Parse PathAnimation Failed");
            return false;
        }

        std::shared_ptr<RSInterpolator> interpolator(RSInterpolator::Unmarshalling(parcel));
        if (interpolator == nullptr) {
            ROSEN_LOGE("RSRenderPathAnimation::ParseParam, Unmarshalling interpolator failed");
            return false;
        }
        SetInterpolator(interpolator);
        SetRotationMode(static_cast<RotationMode>(rotationMode));
        SetIsNeedPath(isNeedPath);
        return true;
    }
#endif
    void SetPathValue(const T& value, float tangent)
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

        RSRenderPropertyAnimation<T>::SetPropertyValue(value);
    }

    void SetRotation(const float tangent)
    {
        auto target = RSRenderAnimation::GetTarget();
        if (target == nullptr) {
            ROSEN_LOGE("Failed to set rotation value, target is null!");
            return;
        }

        auto modifier = target->GetModifier(rotationId_);
        if (modifier != nullptr) {
            auto property = std::static_pointer_cast<RSRenderProperty<float>>(modifier->GetProperty());
            if (property != nullptr) {
                property->Set(tangent);
            }
        }
    }

    void GetPosTanValue(float fraction, Vector2f& position, float& tangent)
    {
        float distance = animationPath_->GetDistance();
        float progress = GetBeginFraction() * (FRACTION_MAX - fraction) + GetEndFraction() * fraction;
        animationPath_->GetPosTan(distance * progress, position, tangent);
    }

    RSRenderPathAnimation() = default;

    float originRotation_ { UNDEFINED_FLOAT };
    float beginFraction_ { FRACTION_MIN };
    float endFraction_ { FRACTION_MAX };
    T startValue_ {};
    T endValue_ {};
    bool isNeedPath_ { true };
    bool needAddOrigin_ { false };
    PropertyId rotationId_;
    RotationMode rotationMode_ { RotationMode::ROTATE_NONE };
    std::shared_ptr<RSInterpolator> interpolator_ { RSInterpolator::DEFAULT };
    std::shared_ptr<RSPath> animationPath_;
};

template<>
void RSRenderPathAnimation<Vector2f>::OnAnimate(float fraction);

template<>
void RSRenderPathAnimation<Vector4f>::OnAnimate(float fraction);
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_PATH_ANIMATION_H
