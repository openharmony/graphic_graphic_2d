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

#include <functional>
#include <memory>
#include <vector>

#include "animation/rs_animation.h"
#include "animation/rs_animation_timing_curve.h"
#include "animation/rs_animation_timing_protocol.h"
#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {
class RSUIContext;

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

    static std::weak_ptr<RSInteractiveImplictAnimator> CreateGroup(const std::shared_ptr<RSUIContext> rsUIContext,
        RSAnimationTimingProtocol& timingProtocol, const RSAnimationTimingCurve& timingCurve);

    size_t AddImplictAnimation(std::function<void()> callback);
    /*
     * @brief add animations form callback
     * @param callback use RSNode::Animate to create animation
     *                 just use RSNode::Animate can create animation
     *                 RSNode::Animate use self Animate protocal and curve crate animation
     *                 In group animation context, can add:
     *                 - Common property animations (e.g., alpha, scale, translate, rotate)
     *                 - Keyframe animations (using RSNode::Animate with keyframes)
     *                 - Motion path animations (using RSNode::Animate with motion path)
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

    RSInteractiveAnimationState GetStatus() const { return state_; }

    InteractiveImplictAnimatorId GetId() const { return id_; }

    void SetFinishCallBack(const std::function<void()>& finishCallback);

protected:
    RSInteractiveImplictAnimator(const std::shared_ptr<RSUIContext> rsUIContext,
        const RSAnimationTimingProtocol& timingProtocol, const RSAnimationTimingCurve& timingCurve,
        bool isGroupAnimator = false);

private:
    static InteractiveImplictAnimatorId GenerateId();
    const InteractiveImplictAnimatorId id_;

    /**
     * @brief Validate animation timing protocol parameters and fix invalid values (except duration)
     * @param timingProtocol The timing protocol to validate and fix
     * @return true if duration is valid, false otherwise
     */
    static bool ValidateTimingProtocol(RSAnimationTimingProtocol& timingProtocol);

    static void InitUniRenderEnabled();
    bool IsUniRenderEnabled() const;
    void FinishOnCurrent();
    void CallFinishCallback();
    void AddCommand(std::unique_ptr<RSCommand>& command, bool isRenderServiceCommand = false,
        FollowType followType = FollowType::NONE, NodeId nodeId = 0) const;
    std::shared_ptr<InteractiveAnimatorFinishCallback> GetAnimatorFinishCallback();
    void SendCreateAnimatorCommand(const std::vector<std::pair<NodeId, AnimationId>>& renderAnimations);

    RSInteractiveAnimationState state_ { RSInteractiveAnimationState::INACTIVE };
    std::weak_ptr<RSUIContext> rsUIContext_;
    RSAnimationTimingProtocol timingProtocol_;
    RSAnimationTimingCurve timingCurve_;

    std::vector<std::pair<std::weak_ptr<RSAnimation>, NodeId>> animations_;
    std::function<void()> finishCallback_;
    std::weak_ptr<InteractiveAnimatorFinishCallback> animatorCallback_;
    AnimationId fractionAnimationId_ { 0 };
    NodeId fractionNodeId_ { 0 };

    bool isGroupAnimator_ { false };
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_INTERACTIVE_IMPLICT_ANIMATOR_H
