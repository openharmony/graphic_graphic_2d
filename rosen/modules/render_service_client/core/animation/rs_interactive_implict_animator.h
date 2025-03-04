/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_INTERACTIVE_IMPLICT_ANIMATOR_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_INTERACTIVE_IMPLICT_ANIMATOR_H

#include <vector>

#include "animation/rs_animation.h"
#include "animation/rs_animation_timing_curve.h"
#include "animation/rs_animation_timing_protocol.h"
#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {

enum class RSInteractiveAnimationState {
    INACTIVE,
    ACTIVE,
    RUNNING,
    PAUSED,
};

class RSC_EXPORT RSInteractiveImplictAnimator : public std::enable_shared_from_this<RSInteractiveImplictAnimator> {
public:
    virtual ~RSInteractiveImplictAnimator();
    static std::shared_ptr<RSInteractiveImplictAnimator> Create(
        const RSAnimationTimingProtocol& timingProtocol, const RSAnimationTimingCurve& timingCurve);
    static std::shared_ptr<RSInteractiveImplictAnimator> Create(
        const std::shared_ptr<RSUIContext> rsUIContext, const RSAnimationTimingProtocol& timingProtocol,
        const RSAnimationTimingCurve& timingCurve);
    /*
     * @brief add animations form callback
     * @param callback use property set change or RSNode::Animate to create aniamtion
     *                 property set use animator protocal and curve create animation
     *                 RSNode::Animate use self Animate protocal and curve crate aniamtion
     *
     */
    size_t AddImplictAnimation(std::function<void()> callback);
    /*
     * @brief add animations form callback
     * @param callback use RSNode::Animate to create aniamtion
     *                 just use RSNode::Animate can create animation
     *                 RSNode::Animate use self Animate protocal and curve crate aniamtion
     *
     */
    size_t AddAnimation(std::function<void()> callback);

    // aniamtor operation
    int32_t StartAnimation();
    void PauseAnimation();
    void ContinueAnimation();

    /*
     * @brief finsih all animation in animator
     * @param position position is START or CURRENT or END
     *                  START all animaton in animator finish on startvalue and sync stagevalue
     *                  CURRENT all animaton in animator finish on currentvalue and sync stagevalue
     *                  START all animaton in animator finish on endvalue and sync stagevalue
     */
    void FinishAnimation(RSInteractiveAnimationPosition position);
    void ReverseAnimation();

    /*
     * @brief set value fraction for all animations
     *          but just support curve animation and interpolating spring aniamtion
     *          curve animation not support step interpolater and custom interpolater
     * @param fraction animation value faraction
     */
    void SetFraction(float fraction);
    float GetFraction();

    RSInteractiveAnimationState GetStatus() const
    {
        return state_;
    }

    /*
     * @brief set callback of all animation finish
     * @param finishCallback all animations in animator use this finishcallback, just one
     */
    void SetFinishCallBack(const std::function<void()>& finishCallback);
protected:
    explicit RSInteractiveImplictAnimator(
        const std::shared_ptr<RSUIContext> rsUIContext, const RSAnimationTimingProtocol& timingProtocol,
        const RSAnimationTimingCurve& timingCurve);
private:
    static InteractiveImplictAnimatorId GenerateId();
    const InteractiveImplictAnimatorId id_;

    static void InitUniRenderEnabled();
    bool IsUniRenderEnabled() const;
    void FinishOnCurrent();
    void CallFinishCallback();
    void AddCommand(std::unique_ptr<RSCommand>& command, bool isRenderServiceCommand = false,
        FollowType followType = FollowType::NONE, NodeId nodeId = 0) const;
    std::shared_ptr<InteractiveAnimatorFinishCallback> GetAnimatorFinishCallback();

    RSInteractiveAnimationState state_ { RSInteractiveAnimationState::INACTIVE };
    std::weak_ptr<RSUIContext> rsUIContext_;
    RSAnimationTimingProtocol timingProtocol_;
    RSAnimationTimingCurve timingCurve_;

    std::vector<std::pair<std::weak_ptr<RSAnimation>, NodeId>> animations_;
    std::function<void()> finishCallback_;
    std::weak_ptr<InteractiveAnimatorFinishCallback> animatorCallback_;
    AnimationId fractionAnimationId_ { 0 };
    NodeId fractionNodeId_ { 0 };
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_INTERACTIVE_IMPLICT_ANIMATOR_H
