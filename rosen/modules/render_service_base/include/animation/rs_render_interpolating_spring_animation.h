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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_INTERPOLATING_SPRING_ANIMATION_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_INTERPOLATING_SPRING_ANIMATION_H

#include "animation/rs_render_property_animation.h"
#include "animation/rs_spring_model.h"
#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSRenderInterpolatingSpringAnimation : public RSRenderPropertyAnimation, public RSSpringModel<float> {
public:
    explicit RSRenderInterpolatingSpringAnimation(AnimationId id, const PropertyId& propertyId,
        const std::shared_ptr<RSRenderPropertyBase>& originValue,
        const std::shared_ptr<RSRenderPropertyBase>& startValue, const std::shared_ptr<RSRenderPropertyBase>& endValue);

    void DumpAnimationType(std::string& out) const override;

    void SetSpringParameters(
        float response, float dampingRatio, float normalizedInitialVelocity, float minimumAmplitudeRatio = 0.00025f);
    void SetZeroThreshold(float zeroThreshold);

    ~RSRenderInterpolatingSpringAnimation() override = default;

#ifdef ROSEN_OHOS
    bool Marshalling(Parcel& parcel) const override;
    [[nodiscard]] static RSRenderInterpolatingSpringAnimation* Unmarshalling(Parcel& parcel);
#endif
protected:
    void OnSetFraction(float fraction) override;
    void OnAnimate(float fraction) override;
    void InitValueEstimator() override;
    void OnInitialize(int64_t time) override;

private:
#ifdef ROSEN_OHOS
    bool ParseParam(Parcel& parcel) override;
#endif
    RSRenderInterpolatingSpringAnimation() = default;
    std::shared_ptr<RSRenderPropertyBase> CalculateVelocity(float time) const;
    bool GetNeedLogicallyFinishCallback() const;
    void CallLogicallyFinishCallback() const;

    std::shared_ptr<RSRenderPropertyBase> startValue_;
    std::shared_ptr<RSRenderPropertyBase> endValue_;
    float normalizedInitialVelocity_ = 0.0;
    bool needLogicallyFinishCallback_ = false;

    // used to determine whether the animation is near finish
    float zeroThreshold_ = 0.0f;

    friend class RSInterpolatingSpringAnimation;
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_INTERPOLATING_SPRING_ANIMATION_H