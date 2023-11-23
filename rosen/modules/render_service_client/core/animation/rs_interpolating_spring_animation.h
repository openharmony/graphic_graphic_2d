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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_INTERPOLATING_SPRING_ANIMATION_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_INTERPOLATING_SPRING_ANIMATION_H

#include <iostream>
#include <memory>

#include "animation/rs_animation_timing_curve.h"
#include "animation/rs_property_animation.h"

namespace OHOS {
namespace Rosen {
class RSRenderInterpolatingSpringAnimation;

class RSC_EXPORT RSInterpolatingSpringAnimation : public RSPropertyAnimation {
public:
    RSInterpolatingSpringAnimation(
        std::shared_ptr<RSPropertyBase> property, const std::shared_ptr<RSPropertyBase>& byValue);

    RSInterpolatingSpringAnimation(std::shared_ptr<RSPropertyBase> property,
        const std::shared_ptr<RSPropertyBase>& startValue, const std::shared_ptr<RSPropertyBase>& endValue);

    virtual ~RSInterpolatingSpringAnimation() = default;

    void SetTimingCurve(const RSAnimationTimingCurve& timingCurve);

    const RSAnimationTimingCurve& GetTimingCurve() const;

    void SetZeroThreshold(const float zeroThreshold) override;

protected:
    void OnStart() override;

private:
    void StartRenderAnimation(const std::shared_ptr<RSRenderInterpolatingSpringAnimation>& animation);

    void StartUIAnimation(const std::shared_ptr<RSRenderInterpolatingSpringAnimation>& animation);

    bool GetIsLogicallyFinishCallback() const;

    RSAnimationTimingCurve timingCurve_;
    float zeroThreshold_ { 0.0f };
    bool isLogicallyFinishCallback_ { false };
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_INTERPOLATING_SPRING_ANIMATION_H