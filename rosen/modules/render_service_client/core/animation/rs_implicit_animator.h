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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_IMPLICIT_ANIMATOR_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_IMPLICIT_ANIMATOR_H

#include <stack>

#include "animation/rs_animation.h"
#include "animation/rs_animation_timing_curve.h"
#include "animation/rs_implicit_animation_param.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
class RSImplicitAnimator {
public:
    RSImplicitAnimator() = default;
    virtual ~RSImplicitAnimator() = default;

    void OpenImplicitAnimation(const RSAnimationTimingProtocol& timingProtocol,
        const RSAnimationTimingCurve& timingCurve, const std::function<void()>& finishCallback);
    std::vector<std::shared_ptr<RSAnimation>> CloseImplicitAnimation();
    void BeginImplicitKeyFrameAnimation(float fraction, const RSAnimationTimingCurve& timingCurve);
    void BeginImplicitKeyFrameAnimation(float fraction);
    void BeginImplicitTransition(const std::shared_ptr<const RSTransitionEffect>& effect);
    void BeginImplicitPathAnimation(const std::shared_ptr<RSMotionPathOption>& motionPathOption);
    void EndImplicitPathAnimation();
    void EndImplicitTransition();
    void EndImplicitKeyFrameAnimation();
    bool NeedImplicitAnimaton();

    template<typename T>
    std::shared_ptr<RSAnimation> CreateImplicitAnimation(
        RSNode& target, const RSAnimatableProperty& property, const T& startValue, const T& endValue);

    std::shared_ptr<RSAnimation> CreateImplicitTransition(RSNode& target, bool isTransitionIn);

private:
    void BeginImplicitCurveAnimation();
    void EndImplicitCurveAnimation();
    void PushImplicitParam(const std::shared_ptr<RSImplicitAnimationParam>& implicitParam);
    void PopImplicitParam();
    void CreateEmptyAnimation();

    template<typename T>
    void SetPropertyValue(RSNode& target, const RSAnimatableProperty& property, const T& value)
    {
        std::shared_ptr<RSBasePropertyAccessors> propertyAccess = RSBasePropertyAccessors::GetAccessor(property);
        auto accessors = std::static_pointer_cast<RSPropertyAccessors<T>>(propertyAccess);
        if (accessors->setter_ == nullptr) {
            return;
        }
        (target.stagingProperties_.*accessors->setter_)(value);
    }

    std::stack<std::tuple<RSAnimationTimingProtocol, RSAnimationTimingCurve, std::function<void()>>>
        globalImplicitParams_;
    std::stack<std::shared_ptr<RSImplicitAnimationParam>> implicitAnimationParams_;
    std::stack<std::vector<std::pair<std::shared_ptr<RSAnimation>, NodeId>>> implicitAnimations_;
    std::stack<std::map<std::pair<NodeId, RSAnimatableProperty>, std::shared_ptr<RSAnimation>>> keyframeAnimations_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_IMPLICIT_ANIMATOR_H
