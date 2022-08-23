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
class RSPath;

class RSRenderPathAnimation : public RSRenderPropertyAnimation {
static constexpr float UNDEFINED_FLOAT = 0.0f;
public:
    RSRenderPathAnimation(AnimationId id, const PropertyId& propertyId,
        const std::shared_ptr<RSRenderPropertyBase>& originPosition,
        const std::shared_ptr<RSRenderPropertyBase>& startPosition,
        const std::shared_ptr<RSRenderPropertyBase>& endPosition, float originRotation,
        const std::shared_ptr<RSPath>& animationPath);

    ~RSRenderPathAnimation() = default;

    void SetInterpolator(const std::shared_ptr<RSInterpolator>& interpolator);

    const std::shared_ptr<RSInterpolator>& GetInterpolator() const;

    void SetRotationMode(const RotationMode& rotationMode);

    RotationMode GetRotationMode() const;

    void SetBeginFraction(float fraction);

    float GetBeginFraction() const;

    void SetEndFraction(float fraction);

    float GetEndFraction() const;

    void SetIsNeedPath(const bool isNeedPath);

    void SetPathNeedAddOrigin(bool needAddOrigin);

    void SetRotationId(const PropertyId id);

#ifdef ROSEN_OHOS
    bool Marshalling(Parcel& parcel) const override;

    static RSRenderPathAnimation* Unmarshalling(Parcel& parcel);
#endif

protected:
    RSRenderPathAnimation() = default;

    void OnAnimate(float fraction) override;

    void OnRemoveOnCompletion() override;

private:
#ifdef ROSEN_OHOS
    bool ParseParam(Parcel& parcel) override;
#endif
    void SetPathValue(const std::shared_ptr<RSRenderPropertyBase>& value, float tangent);

    void SetRotation(const float tangent);

    void GetPosTanValue(float fraction, Vector2f& position, float& tangent);

    std::shared_ptr<RSRenderPropertyBase> UpdateVector2fPathValue(const Vector2f& value, const Vector2f& position);
    std::shared_ptr<RSRenderPropertyBase> UpdateVector4fPathValue(const Vector4f& value, const Vector2f& position);

    float originRotation_ { UNDEFINED_FLOAT };
    float beginFraction_ { FRACTION_MIN };
    float endFraction_ { FRACTION_MAX };
    bool isNeedPath_ { true };
    bool needAddOrigin_ { false };
    PropertyId rotationId_;
    RotationMode rotationMode_ { RotationMode::ROTATE_NONE };
    std::shared_ptr<RSRenderPropertyBase> startValue_ {};
    std::shared_ptr<RSRenderPropertyBase> endValue_ {};
    std::shared_ptr<RSInterpolator> interpolator_ { RSInterpolator::DEFAULT };
    std::shared_ptr<RSPath> animationPath_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_PATH_ANIMATION_H
