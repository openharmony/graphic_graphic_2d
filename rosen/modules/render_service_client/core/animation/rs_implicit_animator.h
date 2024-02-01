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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_IMPLICIT_ANIMATOR_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_IMPLICIT_ANIMATOR_H

#include <stack>
#include <utility>
#include <vector>

#include "animation/rs_animation_timing_curve.h"
#include "animation/rs_animation_timing_protocol.h"
#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {
class AnimationFinishCallback;
class AnimationRepeatCallback;
class RSAnimation;
class RSPropertyBase;
class RSImplicitAnimationParam;
class RSTransitionEffect;
class RSMotionPathOption;
class RSNode;

class RSC_EXPORT RSImplicitAnimator {
public:
    RSImplicitAnimator() = default;
    virtual ~RSImplicitAnimator() = default;

    // open implicit animation with given animation options, finish callback and repeatcallback
    int OpenImplicitAnimation(const RSAnimationTimingProtocol& timingProtocol,
        const RSAnimationTimingCurve& timingCurve, std::shared_ptr<AnimationFinishCallback>&& finishCallback,
        std::shared_ptr<AnimationRepeatCallback>&& repeatCallback);
    // open implicit animation with given animation options and finish callback
    int OpenImplicitAnimation(const RSAnimationTimingProtocol& timingProtocol,
        const RSAnimationTimingCurve& timingCurve, std::shared_ptr<AnimationFinishCallback>&& finishCallback);
    // open implicit animation with current options and given finish callback
    int OpenImplicitAnimation(std::shared_ptr<AnimationFinishCallback>&& finishCallback);
    // open implicit animation with current callback and given timing protocol & curve
    int OpenImplicitAnimation(
        const RSAnimationTimingProtocol& timingProtocol, const RSAnimationTimingCurve& timingCurve);

    // close implicit animation and return all animations
    std::vector<std::shared_ptr<RSAnimation>> CloseImplicitAnimation();

    void BeginImplicitKeyFrameAnimation(float fraction, const RSAnimationTimingCurve& timingCurve);
    void BeginImplicitKeyFrameAnimation(float fraction);
    void EndImplicitKeyFrameAnimation();

    void BeginImplicitDurationKeyFrameAnimation(int duration, const RSAnimationTimingCurve& timingCurve);
    void EndImplicitDurationKeyFrameAnimation();

    void BeginImplicitTransition(const std::shared_ptr<const RSTransitionEffect>& effect, bool isTransitionIn);
    void EndImplicitTransition();

    void BeginImplicitPathAnimation(const std::shared_ptr<RSMotionPathOption>& motionPathOption);
    void EndImplicitPathAnimation();

    bool NeedImplicitAnimation();

    void CreateImplicitAnimation(const std::shared_ptr<RSNode>& target, std::shared_ptr<RSPropertyBase> property,
        const std::shared_ptr<RSPropertyBase>& startValue, const std::shared_ptr<RSPropertyBase>& endValue);

    void CreateImplicitAnimationWithInitialVelocity(const std::shared_ptr<RSNode>& target,
        const std::shared_ptr<RSPropertyBase>& property, const std::shared_ptr<RSPropertyBase>& startValue,
        const std::shared_ptr<RSPropertyBase>& endValue, const std::shared_ptr<RSPropertyBase>& velocity);

    void CreateImplicitTransition(RSNode& target);
    void CancelImplicitAnimation(
        const std::shared_ptr<RSNode>& target, const std::shared_ptr<RSPropertyBase>& property);

private:
    void EndImplicitAnimation();
    void BeginImplicitCurveAnimation();
    void BeginImplicitSpringAnimation();
    void BeginImplicitInterpolatingSpringAnimation();
    void BeginImplicitCancelAnimation();

    void CloseImplicitAnimationInner();
    bool ProcessEmptyAnimations(const std::shared_ptr<AnimationFinishCallback>& finishCallback);

    void PushImplicitParam(const std::shared_ptr<RSImplicitAnimationParam>& implicitParam);
    void PopImplicitParam();
    void CreateEmptyAnimation();

    void SetPropertyValue(std::shared_ptr<RSPropertyBase> property, const std::shared_ptr<RSPropertyBase>& value);

    void ExecuteWithoutAnimation(const std::function<void()>& callback);

    std::stack<std::tuple<RSAnimationTimingProtocol, RSAnimationTimingCurve,
        const std::shared_ptr<AnimationFinishCallback>, std::shared_ptr<AnimationRepeatCallback>>>
        globalImplicitParams_;
    std::stack<std::shared_ptr<RSImplicitAnimationParam>> implicitAnimationParams_;
    std::stack<std::vector<std::pair<std::shared_ptr<RSAnimation>, NodeId>>> implicitAnimations_;
    std::stack<std::map<std::pair<NodeId, PropertyId>, std::shared_ptr<RSAnimation>>> keyframeAnimations_;
    std::stack<std::tuple<bool, int, int>> durationKeyframeParams_;

    bool implicitAnimationDisabled_ { false };
    friend class RSNode;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_IMPLICIT_ANIMATOR_H
