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

#ifndef ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_ANIMATION_COMMAND_H
#define ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_ANIMATION_COMMAND_H

#include "animation/rs_render_animation.h"
#include "animation/rs_render_curve_animation.h"
#include "animation/rs_render_interpolating_spring_animation.h"
#include "animation/rs_render_keyframe_animation.h"
#include "animation/rs_render_path_animation.h"
#include "animation/rs_render_spring_animation.h"
#include "animation/rs_render_transition.h"
#include "animation/rs_render_particle_animation.h"
#include "command/rs_command_templates.h"
#include "common/rs_macros.h"
#include "pipeline/rs_render_node.h"

namespace OHOS {
namespace Rosen {
//Each command HAVE TO have UNIQUE ID in ALL HISTORY
//If a command is not used and you want to delete it,
//just COMMENT it - and never use this value anymore
enum RSAnimationCommandType : uint16_t {
    // curve animation
    ANIMATION_CREATE_CURVE = 0x0100,
    // particle animation
    ANIMATION_CREATE_PARTICLE,
    // particle animation NG
    ANIMATION_CREATE_PARTICLE_NG,
    // keyframe animation
    ANIMATION_CREATE_KEYFRAME = 0x0102,
    // path animation
    ANIMATION_CREATE_PATH = 0x0103,
    // transition animation
    ANIMATION_CREATE_TRANSITION = 0x0104,
    // spring animation
    ANIMATION_CREATE_SPRING = 0x0105,
    // interpolating spring animation
    ANIMATION_CREATE_INTERPOLATING_SPRING = 0x0106,

    // operations
    ANIMATION_START = 0x0200,
    ANIMATION_PAUSE = 0x0201,
    ANIMATION_RESUME = 0x0202,
    ANIMATION_FINISH = 0x0203,
    ANIMATION_REVERSE = 0x0204,
    ANIMATION_SET_FRACTION = 0x0205,
    ANIMATION_CANCEL = 0x0206,

    // UI operation
    ANIMATION_CALLBACK = 0x0300,

    // interactive animator operation
    INTERACTIVE_ANIMATOR_CREATE = 0x0400,
    INTERACTIVE_ANIMATOR_DESTORY = 0x0401,
    INTERACTIVE_ANIMATOR_PAUSE = 0x0402,
    INTERACTIVE_ANIMATOR_CONTINUE = 0x0403,
    INTERACTIVE_ANIMATOR_FINISH = 0x0404,
    INTERACTIVE_ANIMATOR_REVERSE = 0x0405,
    INTERACTIVE_ANIMATOR_SET_FRACTION = 0x0406,
};

enum AnimationCallbackEvent : uint16_t { REPEAT_FINISHED, FINISHED, LOGICALLY_FINISHED };

class RSB_EXPORT AnimationCommandHelper {
public:
    template<void (RSRenderAnimation::*OP)()>
    static void AnimOp(RSContext& context, NodeId nodeId, AnimationId animId)
    {
        [[maybe_unused]] auto [node, animation] = GetNodeAndAnimation(context, nodeId, animId, __PRETTY_FUNCTION__);
        if (animation) {
            (*animation.*OP)();
        }
    }
    template<void (RSRenderAnimation::*OP)()>
    static void AnimOpReg(RSContext& context, NodeId nodeId, AnimationId animId)
    {
        auto [node, animation] = GetNodeAndAnimation(context, nodeId, animId, __PRETTY_FUNCTION__);
        if (node && animation) {
            (*animation.*OP)();
            // register node on animation start or resume
            context.RegisterAnimatingRenderNode(node);
        }
    }
    template<typename T, void (RSRenderAnimation::*OP)(T)>
    static void AnimOp(RSContext& context, NodeId nodeId, AnimationId animId, T param)
    {
        [[maybe_unused]] auto [node, animation] = GetNodeAndAnimation(context, nodeId, animId, __PRETTY_FUNCTION__);
        if (animation) {
            (*animation.*OP)(param);
        }
    }
    static void CreateAnimation(
        RSContext& context, NodeId targetId, const std::shared_ptr<RSRenderAnimation>& animation);
    static void CreateParticleAnimation(RSContext& context, NodeId targetId,
        const std::shared_ptr<RSRenderParticleAnimation>& animation);
    static void CreateParticleAnimationNG(
        RSContext& context, NodeId targetId, const std::shared_ptr<RSRenderParticleAnimation>& animation);

    using AnimationCallbackProcessor = void (*)(NodeId, AnimationId, uint64_t, AnimationCallbackEvent);
    static void AnimationCallback(RSContext& context,
                                  NodeId targetId, AnimationId animId, uint64_t token,
                                  AnimationCallbackEvent event);
    static RSB_EXPORT void SetAnimationCallbackProcessor(AnimationCallbackProcessor processor);
    static void CancelAnimation(RSContext& context, NodeId targetId, PropertyId propertyId);

    static void CreateInteractiveAnimator(RSContext& context,
        InteractiveImplictAnimatorId targetId, std::vector<std::pair<NodeId, AnimationId>> animations,
        bool startImmediately);
    static void DestoryInteractiveAnimator(RSContext& context, InteractiveImplictAnimatorId targetId);
    static void InteractiveAnimatorAddAnimations(RSContext& context,
        InteractiveImplictAnimatorId targetId, std::vector<std::pair<NodeId, AnimationId>> animations);
    static void PauseInteractiveAnimator(RSContext& context, InteractiveImplictAnimatorId targetId);
    static void ContinueInteractiveAnimator(RSContext& context, InteractiveImplictAnimatorId targetId);
    static void FinishInteractiveAnimator(RSContext& context,
        InteractiveImplictAnimatorId targetId, RSInteractiveAnimationPosition finishPos);
    static void ReverseInteractiveAnimator(RSContext& context, InteractiveImplictAnimatorId targetId);
    static void SetFractionInteractiveAnimator(RSContext& context,
        InteractiveImplictAnimatorId targetId, float fraction);
private:
    using NodeAndAnimationPair =
    std::pair<const std::shared_ptr<RSRenderNode>, const std::shared_ptr<RSRenderAnimation>>;
    static NodeAndAnimationPair GetNodeAndAnimation(
    RSContext& context, NodeId& nodeId, AnimationId& animId, const char* funcName);
};

// animation operation
ADD_COMMAND(RSAnimationStart,
    ARG(PERMISSION_APP, ANIMATION, ANIMATION_START,
        AnimationCommandHelper::AnimOpReg<&RSRenderAnimation::Start>, NodeId, AnimationId))
ADD_COMMAND(RSAnimationPause,
    ARG(PERMISSION_APP, ANIMATION, ANIMATION_PAUSE,
        AnimationCommandHelper::AnimOp<&RSRenderAnimation::Pause>, NodeId, AnimationId))
ADD_COMMAND(RSAnimationResume,
    ARG(PERMISSION_APP, ANIMATION, ANIMATION_RESUME,
        AnimationCommandHelper::AnimOpReg<&RSRenderAnimation::Resume>, NodeId, AnimationId))
ADD_COMMAND(RSAnimationFinish,
    ARG(PERMISSION_APP, ANIMATION, ANIMATION_FINISH,
        AnimationCommandHelper::AnimOpReg<&RSRenderAnimation::Finish>, NodeId, AnimationId))
ADD_COMMAND(RSAnimationReverse,
    ARG(PERMISSION_APP, ANIMATION, ANIMATION_REVERSE,
        AnimationCommandHelper::AnimOp<bool, &RSRenderAnimation::SetReversed>, NodeId, AnimationId, bool))
ADD_COMMAND(RSAnimationSetFraction,
    ARG(PERMISSION_APP, ANIMATION, ANIMATION_SET_FRACTION,
        AnimationCommandHelper::AnimOp<float, &RSRenderAnimation::SetFraction>, NodeId, AnimationId, float))
ADD_COMMAND(RSAnimationCancel,
    ARG(PERMISSION_APP, ANIMATION, ANIMATION_CANCEL,
        AnimationCommandHelper::CancelAnimation, NodeId, PropertyId))

ADD_COMMAND(RSAnimationCallback,
    ARG(PERMISSION_APP, ANIMATION, ANIMATION_CALLBACK,
        AnimationCommandHelper::AnimationCallback, NodeId, AnimationId, uint64_t, AnimationCallbackEvent))

// create curve animation
ADD_COMMAND(RSAnimationCreateCurve,
    ARG(PERMISSION_APP, ANIMATION, ANIMATION_CREATE_CURVE, AnimationCommandHelper::CreateAnimation,
        NodeId, std::shared_ptr<RSRenderCurveAnimation>))

// create particle animation
ADD_COMMAND(RSAnimationCreateParticle,
    ARG(PERMISSION_APP, ANIMATION, ANIMATION_CREATE_PARTICLE, AnimationCommandHelper::CreateParticleAnimation, NodeId,
        std::shared_ptr<RSRenderParticleAnimation>))

// create particle animation in ModifierNG
ADD_COMMAND(RSAnimationCreateParticleNG,
    ARG(PERMISSION_APP, ANIMATION, ANIMATION_CREATE_PARTICLE_NG, AnimationCommandHelper::CreateParticleAnimationNG,
        NodeId, std::shared_ptr<RSRenderParticleAnimation>))

// create keyframe animation
ADD_COMMAND(RSAnimationCreateKeyframe,
    ARG(PERMISSION_APP, ANIMATION, ANIMATION_CREATE_KEYFRAME,
        AnimationCommandHelper::CreateAnimation, NodeId, std::shared_ptr<RSRenderKeyframeAnimation>))

// create path animation
ADD_COMMAND(RSAnimationCreatePath,
    ARG(PERMISSION_APP, ANIMATION, ANIMATION_CREATE_PATH,
        AnimationCommandHelper::CreateAnimation, NodeId, std::shared_ptr<RSRenderPathAnimation>))

// create transition animation
ADD_COMMAND(RSAnimationCreateTransition,
    ARG(PERMISSION_APP, ANIMATION, ANIMATION_CREATE_TRANSITION,
        AnimationCommandHelper::CreateAnimation, NodeId, std::shared_ptr<RSRenderTransition>))

// create spring animation
ADD_COMMAND(RSAnimationCreateSpring,
    ARG(PERMISSION_APP, ANIMATION, ANIMATION_CREATE_SPRING,
        AnimationCommandHelper::CreateAnimation, NodeId, std::shared_ptr<RSRenderSpringAnimation>))

// create interpolating spring animation
ADD_COMMAND(RSAnimationCreateInterpolatingSpring,
    ARG(PERMISSION_APP, ANIMATION, ANIMATION_CREATE_INTERPOLATING_SPRING, AnimationCommandHelper::CreateAnimation,
        NodeId, std::shared_ptr<RSRenderInterpolatingSpringAnimation>))

// interactive implict animator operation
ADD_COMMAND(RSInteractiveAnimatorCreate,
    ARG(PERMISSION_APP, ANIMATION, INTERACTIVE_ANIMATOR_CREATE,
        AnimationCommandHelper::CreateInteractiveAnimator, InteractiveImplictAnimatorId,
        std::vector<std::pair<NodeId, AnimationId>>, bool))
ADD_COMMAND(RSInteractiveAnimatorDestory,
    ARG(PERMISSION_APP, ANIMATION, INTERACTIVE_ANIMATOR_DESTORY,
        AnimationCommandHelper::DestoryInteractiveAnimator, InteractiveImplictAnimatorId))
ADD_COMMAND(RSInteractiveAnimatorPause,
    ARG(PERMISSION_APP, ANIMATION, INTERACTIVE_ANIMATOR_PAUSE,
        AnimationCommandHelper::PauseInteractiveAnimator, InteractiveImplictAnimatorId))
ADD_COMMAND(RSInteractiveAnimatorContinue,
    ARG(PERMISSION_APP, ANIMATION, INTERACTIVE_ANIMATOR_CONTINUE,
        AnimationCommandHelper::ContinueInteractiveAnimator, InteractiveImplictAnimatorId))
ADD_COMMAND(RSInteractiveAnimatorFinish,
    ARG(PERMISSION_APP, ANIMATION, INTERACTIVE_ANIMATOR_FINISH,
        AnimationCommandHelper::FinishInteractiveAnimator, InteractiveImplictAnimatorId,
        RSInteractiveAnimationPosition))
ADD_COMMAND(RSInteractiveAnimatorReverse,
    ARG(PERMISSION_APP, ANIMATION, INTERACTIVE_ANIMATOR_REVERSE,
        AnimationCommandHelper::ReverseInteractiveAnimator, InteractiveImplictAnimatorId))
ADD_COMMAND(RSInteractiveAnimatorSetFraction,
    ARG(PERMISSION_APP, ANIMATION, INTERACTIVE_ANIMATOR_SET_FRACTION,
        AnimationCommandHelper::SetFractionInteractiveAnimator, InteractiveImplictAnimatorId, float))
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_ANIMATION_COMMAND_H
