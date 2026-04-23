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

#include "animation/rs_render_interactive_implict_animator.h"

#include "animation/rs_animation_common.h"
#include "animation/rs_animation_trace_utils.h"
#include "animation/rs_render_animation.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_render_node.h"
#include "platform/common/rs_log.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {

RSRenderInteractiveImplictAnimator::RSRenderInteractiveImplictAnimator(
    InteractiveImplictAnimatorId id, const std::weak_ptr<RSContext>& context)
    : id_(id), context_(context)
{}

void RSRenderInteractiveImplictAnimator::AddAnimations(std::vector<std::pair<NodeId, AnimationId>> animations)
{
    auto context = context_.lock();
    if (context == nullptr) {
        ROSEN_LOGE("RSRenderInteractiveImplictAnimator::AddAnimations - context is null");
        return;
    }

    cachedAnimations_.clear();

    for (auto& [nodeId, animationId] : animations) {
        auto node = context->GetNodeMap().GetRenderNode<RSRenderNode>(nodeId);
        if (node == nullptr) {
            ROSEN_LOGE(
                "RSRenderInteractiveImplictAnimator::AddAnimations - node null, nodeId: %{public}" PRIu64, nodeId);
            continue;
        }
        auto animation = node->GetAnimationManager().GetAnimation(animationId);
        if (animation == nullptr) {
            ROSEN_LOGE("RSRenderInteractiveImplictAnimator::AddAnimations - animation null, "
                "nodeId: %{public}" PRIu64 ", animationId: %{public}" PRIu64, nodeId, animationId);
            continue;
        }
        cachedAnimations_.emplace_back(animation);
    }
}

void RSRenderInteractiveImplictAnimator::PauseAnimator()
{
    auto context = context_.lock();
    if (context == nullptr) {
        ROSEN_LOGE("RSRenderInteractiveImplictAnimator::Pause - context is null, id: %{public}" PRIu64, id_);
        return;
    }

    for (auto& weakAnim : cachedAnimations_) {
        auto animation = weakAnim.lock();
        if (animation == nullptr) {
            continue;
        }
        animation->Pause();
    }
}

void RSRenderInteractiveImplictAnimator::ContinueAnimator()
{
    auto context = context_.lock();
    if (context == nullptr) {
        ROSEN_LOGE("RSRenderInteractiveImplictAnimator::Continue - context is null, id: %{public}" PRIu64, id_);
        return;
    }

    for (auto& weakAnim : cachedAnimations_) {
        auto animation = weakAnim.lock();
        if (animation == nullptr) {
            continue;
        }
        animation->Resume();

        if (auto node = context->GetNodeMap().GetRenderNode<RSRenderNode>(animation->GetTargetId())) {
            context->RegisterAnimatingRenderNode(node);
        }
    }
}

void RSRenderInteractiveImplictAnimator::FinishAnimator(RSInteractiveAnimationPosition finishPos)
{
    auto context = context_.lock();
    if (context == nullptr) {
        ROSEN_LOGE("RSRenderInteractiveImplictAnimator::Finish - context is null, id: %{public}" PRIu64, id_);
        return;
    }

    for (auto& weakAnim : cachedAnimations_) {
        auto animation = weakAnim.lock();
        if (animation == nullptr) {
            continue;
        }
        animation->FinishOnPosition(finishPos);
    }
}

void RSRenderInteractiveImplictAnimator::ReverseAnimator()
{
    auto context = context_.lock();
    if (context == nullptr) {
        ROSEN_LOGE("RSRenderInteractiveImplictAnimator::Reverse - context is null, id: %{public}" PRIu64, id_);
        return;
    }

    for (auto& weakAnim : cachedAnimations_) {
        auto animation = weakAnim.lock();
        if (animation == nullptr) {
            continue;
        }
        animation->SetReversedAndContinue();

        if (auto node = context->GetNodeMap().GetRenderNode<RSRenderNode>(animation->GetTargetId())) {
            context->RegisterAnimatingRenderNode(node);
        }
    }
}

void RSRenderInteractiveImplictAnimator::SetFractionAnimator(float fraction)
{
    auto context = context_.lock();
    if (context == nullptr) {
        ROSEN_LOGE("RSRenderInteractiveImplictAnimator::SetFraction - context is null, id: %{public}" PRIu64, id_);
        return;
    }

    for (auto& weakAnim : cachedAnimations_) {
        auto animation = weakAnim.lock();
        if (animation == nullptr) {
            continue;
        }
        animation->SetFraction(fraction);
    }
}

RSRenderTimeDrivenGroupAnimator::RSRenderTimeDrivenGroupAnimator(InteractiveImplictAnimatorId id,
    const std::weak_ptr<RSContext>& context, const RSAnimationTimingProtocol& timingProtocol)
    : RSRenderInteractiveImplictAnimator(id, context), timingProtocol_(timingProtocol)
{
    animationFraction_.SetDuration(timingProtocol_.GetDuration());
    animationFraction_.SetStartDelay(timingProtocol_.GetStartDelay());
    animationFraction_.SetRepeatCount(timingProtocol_.GetRepeatCount());
    animationFraction_.SetAutoReverse(timingProtocol_.GetAutoReverse());
    animationFraction_.SetSpeed(timingProtocol_.GetSpeed());
    animationFraction_.SetRepeatCallbackEnable(true);

    RS_TRACE_NAME_FMT("RSRenderTimeDrivenGroupAnimator::Ctor id[%llu] "
        "duration[%d] startDelay[%d] repeatCount[%d] autoReverse[%d] speed[%f]",
        id_, timingProtocol_.GetDuration(), timingProtocol_.GetStartDelay(), timingProtocol_.GetRepeatCount(),
        timingProtocol_.GetAutoReverse(), timingProtocol_.GetSpeed());
    ROSEN_LOGI("RSRenderTimeDrivenGroupAnimator::Ctor - id: %{public}" PRIu64 ", "
        "timingProtocol-duration: %{public}d, startDelay: %{public}d, repeatCount: %{public}d, "
        "autoReverse: %{public}d, speed: %{public}f",
        id_, timingProtocol_.GetDuration(), timingProtocol_.GetStartDelay(), timingProtocol_.GetRepeatCount(),
        timingProtocol_.GetAutoReverse(), timingProtocol_.GetSpeed());
}

void RSRenderTimeDrivenGroupAnimator::ContinueAnimator()
{
    if (state_ != GroupAnimatorState::PAUSED) {
        ROSEN_LOGE("Failed to continue group animator, state is not PAUSED, id: %{public}" PRIu64, id_);
        return;
    }

    state_ = GroupAnimatorState::RUNNING;
    needUpdateStartTime_ = true;

    auto context = context_.lock();
    if (context == nullptr) {
        ROSEN_LOGE("RSRenderTimeDrivenGroupAnimator::Continue - context is null, id: %{public}" PRIu64, id_);
        return;
    }

    for (auto& weakAnim : cachedAnimations_) {
        auto animation = weakAnim.lock();
        if (animation == nullptr) {
            continue;
        }

        // Check if this animation was in GROUP_WAITING state before pause
        if (groupWaitingAnimationIds_.count(animation->GetAnimationId()) > 0) {
            animation->ResumeGroupWaiting();
        } else {
            animation->Resume();
        }

        if (auto node = context->GetNodeMap().GetRenderNode<RSRenderNode>(animation->GetTargetId())) {
            context->RegisterAnimatingRenderNode(node);
        }
    }

    groupWaitingAnimationIds_.clear();
}

void RSRenderTimeDrivenGroupAnimator::StartChildAnimations()
{
    if (!activeChildAnimations_.empty()) {
        return;
    }

    auto context = context_.lock();
    if (context == nullptr) {
        ROSEN_LOGE(
            "RSRenderTimeDrivenGroupAnimator::StartChildAnimations - context is null, id: %{public}" PRIu64, id_);
        return;
    }

    RS_TRACE_NAME_FMT("RSRenderTimeDrivenGroupAnimator::StartChildAnimations animator[%llu]", id_);
    ROSEN_LOGI("RSRenderTimeDrivenGroupAnimator::StartChildAnimations - id: %{public}" PRIu64
        ", starting %{public}zu child animations after delay", id_, cachedAnimations_.size());

    for (auto& weakAnim : cachedAnimations_) {
        auto animation = weakAnim.lock();
        if (animation == nullptr) {
            continue;
        }

        AnimationId animationId = animation->GetAnimationId();
        activeChildAnimations_.insert(animationId);
        animation->SetGroupAnimator(std::static_pointer_cast<RSRenderTimeDrivenGroupAnimator>(shared_from_this()));
        animation->Resume();

        if (auto node = context->GetNodeMap().GetRenderNode<RSRenderNode>(animation->GetTargetId())) {
            context->RegisterAnimatingRenderNode(node);
        }
    }
}

void RSRenderTimeDrivenGroupAnimator::RemoveActiveChildAnimation(AnimationId animationId)
{
    if (activeChildAnimations_.empty()) {
        return;
    }
    if (auto it = activeChildAnimations_.find(animationId); it != activeChildAnimations_.end()) {
        activeChildAnimations_.erase(it);
        if (activeChildAnimations_.empty()) {
            RS_TRACE_NAME_FMT("RemoveActiveChildAnimation, GroupAnimator[%llu] all children has removed", id_);
            ROSEN_LOGI("RSRenderTimeDrivenGroupAnimator::RemoveActiveChildAnimation, GroupAnimator[%{public}" PRIu64
                "] all children has removed", id_);
            state_ = GroupAnimatorState::FINISHED;
        }
    }
}

void RSRenderTimeDrivenGroupAnimator::StartAnimator()
{
    if (IsStarted()) {
        ROSEN_LOGE("Failed to start group animator, animation has started!");
        return;
    }
    state_ = GroupAnimatorState::RUNNING;
}

void RSRenderTimeDrivenGroupAnimator::PauseAnimator()
{
    if (state_ != GroupAnimatorState::RUNNING) {
        ROSEN_LOGE("Failed to pause group animator, state is not RUNNING, id: %{public}" PRIu64, id_);
        return;
    }

    auto context = context_.lock();
    if (context == nullptr) {
        ROSEN_LOGE("RSRenderTimeDrivenGroupAnimator::Pause - context is null, id: %{public}" PRIu64, id_);
        return;
    }

    state_ = GroupAnimatorState::PAUSED;

    // When pause occurs during the delay period, record the elapsed delay time and add it to the total executed time.
    int64_t minLeftDelayTime = 0;
    animationFraction_.GetAnimationFraction(context->GetCurrentTimestamp(), minLeftDelayTime, false);

    groupWaitingAnimationIds_.clear();
    for (auto& weakAnim : cachedAnimations_) {
        auto animation = weakAnim.lock();
        if (animation == nullptr) {
            continue;
        }

        if (animation->IsRunning()) {
            animation->Pause();
        } else if (animation->IsGroupWaiting()) {
            groupWaitingAnimationIds_.insert(animation->GetAnimationId());
            animation->Pause();
        }
    }
}

void RSRenderTimeDrivenGroupAnimator::SetStartTime(int64_t time)
{
    animationFraction_.SetLastFrameTime(time);
    needUpdateStartTime_ = false;
}

void RSRenderTimeDrivenGroupAnimator::OnAnimate(int64_t timestamp, int64_t& minLeftDelayTime)
{
    if (state_ == GroupAnimatorState::FINISHED) {
        RS_TRACE_NAME_FMT("OnAnimate GroupAnimator[%llu] state is FINISHED", id_);
        FinishAnimator(RSInteractiveAnimationPosition::END);
        return;
    }

    if (state_ != GroupAnimatorState::RUNNING) {
        return;
    }

    if (needUpdateStartTime_) {
        RS_TRACE_NAME_FMT("RSRenderTimeDrivenGroupAnimator::OnAnimate animator[%llu] UpdateStartTime", id_);
        SetStartTime(timestamp);
        minLeftDelayTime = 0;
        return;
    }

    UpdateFraction(timestamp, minLeftDelayTime);
}

void RSRenderTimeDrivenGroupAnimator::UpdateFraction(int64_t timestamp, int64_t& minLeftDelayTime)
{
    auto [fraction, isInStartDelay, isFinished, isRepeatFinished] =
        animationFraction_.GetAnimationFraction(timestamp, minLeftDelayTime, false);

    RS_TRACE_NAME_FMT("RSRenderTimeDrivenGroupAnimator::UpdateFraction animator[%llu] fraction[%f] "
        "runningTime[%lld]ms duration[%d]ms delay[%d]ms repeat[%d] childCount[%zu]",
        id_, fraction, animationFraction_.GetRunningTime() / MS_TO_NS, timingProtocol_.GetDuration(),
        timingProtocol_.GetStartDelay(), timingProtocol_.GetRepeatCount(), cachedAnimations_.size());

    if (isRepeatFinished) {
        ResetChildAnimations();
    }

    if (!isInStartDelay) {
        StartChildAnimations();
    }

    if (isFinished) {
        FinishAnimator(RSInteractiveAnimationPosition::END);
    }
}

void RSRenderTimeDrivenGroupAnimator::ResetChildAnimations()
{
    // Last repeat cycle, no need to reset child animations
    if (animationFraction_.GetRemainingRepeatCount() == 0) {
        return;
    }
    RS_TRACE_NAME_FMT("ResetChildAnimations animator[%llu]", id_);

    auto context = context_.lock();
    if (context == nullptr) {
        ROSEN_LOGE(
            "RSRenderTimeDrivenGroupAnimator::ResetChildAnimations - context is null, id: %{public}" PRIu64, id_);
        return;
    }

    for (auto& weakAnim : cachedAnimations_) {
        auto animation = weakAnim.lock();
        if (animation == nullptr) {
            continue;
        }

        if (animationFraction_.GetAutoReverse()) {
            animation->FlipDirection();
        } else {
            animation->Restart();
        }
    }
}

void RSRenderTimeDrivenGroupAnimator::FinishAnimator(RSInteractiveAnimationPosition finishPos)
{
    RS_TRACE_NAME_FMT("FinishAnimator GroupAnimator[%llu] finishPos[%d] state[%d] childCount[%zu]",
        id_, static_cast<int>(finishPos), static_cast<int>(state_), cachedAnimations_.size());
    ROSEN_LOGI("RSRenderTimeDrivenGroupAnimator::Finish - id: %{public}" PRIu64 ", "
        "finishPos: %{public}d, state: %{public}d, finishing %{public}zu child animations",
        id_, static_cast<int>(finishPos), static_cast<int>(state_), cachedAnimations_.size());

    auto context = context_.lock();
    if (context == nullptr) {
        ROSEN_LOGE("RSRenderTimeDrivenGroupAnimator::Finish - context is null, id: %{public}" PRIu64, id_);
        return;
    }

    for (auto& weakAnim : cachedAnimations_) {
        auto animation = weakAnim.lock();
        if (animation == nullptr) {
            continue;
        }

        auto nodeId = animation->GetTargetId();
        auto node = context->GetNodeMap().GetRenderNode<RSRenderNode>(nodeId);
        if (node == nullptr) {
            ROSEN_LOGW("RSRenderTimeDrivenGroupAnimator::Finish - node null, nodeId: %{public}" PRIu64, nodeId);
            continue;
        }

        auto& animationManager = node->GetAnimationManager();
        animationManager.OnAnimationFinished(animation);
        animationManager.RemoveAnimation(animation->GetAnimationId());

        animation->FinishOnPosition(finishPos);
    }
    state_ = GroupAnimatorState::FINISHED;
    context->GetInteractiveImplictAnimatorMap().UnregisterInteractiveImplictAnimator(id_);
}
} // namespace Rosen
} // namespace OHOS
