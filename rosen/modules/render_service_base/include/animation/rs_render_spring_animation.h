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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_SPRING_ANIMATION_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_SPRING_ANIMATION_H

#include "animation/rs_render_property_animation.h"
#include "animation/rs_spring_model.h"
#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSRenderSpringAnimation : public RSRenderPropertyAnimation,
    public RSSpringModel<std::shared_ptr<RSRenderPropertyBase>> {
public:
    explicit RSRenderSpringAnimation(AnimationId id, const PropertyId& propertyId,
        const std::shared_ptr<RSRenderPropertyBase>& originValue,
        const std::shared_ptr<RSRenderPropertyBase>& startValue,
        const std::shared_ptr<RSRenderPropertyBase>& endValue);

    void SetSpringParameters(float response, float dampingRatio, float blendDuration = 0.0f);

    ~RSRenderSpringAnimation() override = default;

    void SetZeroThreshold(float zeroThreshold);

#ifdef ROSEN_OHOS
    bool Marshalling(Parcel& parcel) const override;
    [[nodiscard]] static RSRenderSpringAnimation* Unmarshalling(Parcel& parcel);
#endif
protected:
    void OnSetFraction(float fraction) override;
    void OnAnimate(float fraction) override;
    std::tuple<bool, bool> OnAnimateByTime(float time) override;

    void OnAttach() override;
    void OnDetach() override;
    void OnInitialize(int64_t time) override;
    void ProcessFillModeOnFinishByTime(float endTime = 0) override;
    AnimationTimingMode GetAnimationTimingMode() const override;

private:
#ifdef ROSEN_OHOS
    bool ParseParam(Parcel& parcel) override;
#endif
    RSRenderSpringAnimation() = default;

    // status inherited related
    float prevMappedTime_ = 0.0f;
    // return current <value, velocity, remainingDelayTime> as a tuple
    std::tuple<std::shared_ptr<RSRenderPropertyBase>, std::shared_ptr<RSRenderPropertyBase>, int64_t>
        GetSpringStatus() const;
    void InheritSpringStatus(const RSRenderSpringAnimation* from);
    std::shared_ptr<RSRenderPropertyBase> CalculateVelocity(float time) const;

    // blend related
    uint64_t blendDuration_ = 0;
    float finalResponse_ = 0.0f;

    std::shared_ptr<RSRenderPropertyBase> startValue_;
    std::shared_ptr<RSRenderPropertyBase> endValue_;
    // save the origin startValue_, as startValue_ will change when inherit happened
    std::shared_ptr<RSRenderPropertyBase> originStartValue_;

    // used to determine the end of animation
    float zeroThreshold_ = 2.0f;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_SPRING_ANIMATION_H
