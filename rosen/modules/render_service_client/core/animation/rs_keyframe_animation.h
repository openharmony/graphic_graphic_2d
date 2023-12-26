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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_KEYFRAME_ANIMATION_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_KEYFRAME_ANIMATION_H

#include <memory>
#include <vector>

#include "animation/rs_animation_common.h"
#include "animation/rs_animation_timing_curve.h"
#include "animation/rs_property_animation.h"

namespace OHOS {
namespace Rosen {
static constexpr int FRACTION_INDEX = 0;
static constexpr int VALUE_INDEX = 1;
static constexpr int INTERPOLATOR_INDEX = 2;

class RSRenderKeyframeAnimation;

class RSC_EXPORT RSKeyframeAnimation : public RSPropertyAnimation {
public:
    RSKeyframeAnimation(std::shared_ptr<RSPropertyBase> property);
    virtual ~RSKeyframeAnimation() = default;

    void AddKeyFrame(float fraction, const std::shared_ptr<RSPropertyBase>& value,
        const RSAnimationTimingCurve& timingCurve);

    void AddKeyFrames(const
        std::vector<std::tuple<float, std::shared_ptr<RSPropertyBase>, RSAnimationTimingCurve>>& keyframes);

    void AddKeyFrame(int startDuration, int endDuration, const std::shared_ptr<RSPropertyBase>& value,
        const RSAnimationTimingCurve& timingCurve);

    void SetDurationKeyframe(bool isDuration);

protected:
    void OnStart() override;

    void InitInterpolationValue() override;

private:
    void StartRenderAnimation(const std::shared_ptr<RSRenderKeyframeAnimation>& animation);

    void StartUIAnimation(const std::shared_ptr<RSRenderKeyframeAnimation>& animation);

    std::vector<std::tuple<float, std::shared_ptr<RSPropertyBase>, RSAnimationTimingCurve>> keyframes_;

    std::vector<std::tuple<int, int, std::shared_ptr<RSPropertyBase>, RSAnimationTimingCurve>> durationKeyframes_;

    bool isDurationKeyframe_ { false };

    friend class RSImplicitKeyframeAnimationParam;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_KEYFRAME_ANIMATION_H
