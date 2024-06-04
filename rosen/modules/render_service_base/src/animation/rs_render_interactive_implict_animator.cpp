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
#include "pipeline/rs_context.h"

namespace OHOS {
namespace Rosen {
RSRenderInteractiveImplictAnimator::RSRenderInteractiveImplictAnimator(
    InteractiveImplictAnimatorId id, const std::weak_ptr<RSContext>& context) : id_(id), context_(context)
{}

void RSRenderInteractiveImplictAnimator::AddAnimations(std::vector<std::pair<NodeId, AnimationId>> animations)
{
    auto context = context_.lock();
    if (context == nullptr) {
        return;
    }
    animations_.clear();
    for (auto& [nodeId, animationId] : animations) {
        auto node = context->GetNodeMap().GetRenderNode<RSRenderNode>(nodeId);
        if (node == nullptr) {
            continue;
        }
        auto animation = node->GetAnimationManager().GetAnimation(animationId);
        if (animation == nullptr) {
            continue;
        }
        animations_.emplace_back(nodeId, animationId);
    }
}

void RSRenderInteractiveImplictAnimator::PauseAnimator()
{
    auto context = context_.lock();
    if (context == nullptr) {
        return;
    }
    for (auto& [nodeId, animationId] : animations_) {
        auto node = context->GetNodeMap().GetRenderNode<RSRenderNode>(nodeId);
        if (node == nullptr) {
            continue;
        }
        auto animation = node->GetAnimationManager().GetAnimation(animationId);
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
        return;
    }
    for (auto& [nodeId, animationId] : animations_) {
        auto node = context->GetNodeMap().GetRenderNode<RSRenderNode>(nodeId);
        if (node == nullptr) {
            continue;
        }
        auto animation = node->GetAnimationManager().GetAnimation(animationId);
        if (animation == nullptr) {
            return;
        }
        animation->Resume();
        // register node on animation start or resume
        context->RegisterAnimatingRenderNode(node);
    }
}

void RSRenderInteractiveImplictAnimator::FinishAnimator(RSInteractiveAnimationPosition finishPos)
{
    auto context = context_.lock();
    if (context == nullptr) {
        return;
    }
    for (auto& [nodeId, animationId] : animations_) {
        auto node = context->GetNodeMap().GetRenderNode<RSRenderNode>(nodeId);
        if (node == nullptr) {
            continue;
        }
        auto animation = node->GetAnimationManager().GetAnimation(animationId);
        if (animation == nullptr) {
            return;
        }
        animation->FinishOnPosition(finishPos);
    }
}

void RSRenderInteractiveImplictAnimator::ReverseAnimator()
{
    auto context = context_.lock();
    if (context == nullptr) {
        return;
    }
    for (auto& [nodeId, animationId] : animations_) {
        auto node = context->GetNodeMap().GetRenderNode<RSRenderNode>(nodeId);
        if (node == nullptr) {
            continue;
        }
        auto animation = node->GetAnimationManager().GetAnimation(animationId);
        if (animation == nullptr) {
            return;
        }
        animation->SetReversedAndContinue();
        context->RegisterAnimatingRenderNode(node);
    }
}

void RSRenderInteractiveImplictAnimator::SetFractionAnimator(float fraction)
{
    auto context = context_.lock();
    if (context == nullptr) {
        return;
    }
    for (auto& [nodeId, animationId] : animations_) {
        auto node = context->GetNodeMap().GetRenderNode<RSRenderNode>(nodeId);
        if (node == nullptr) {
            continue;
        }
        auto animation = node->GetAnimationManager().GetAnimation(animationId);
        if (animation == nullptr) {
            return;
        }
        animation->SetFraction(fraction);
    }
}

} // namespace Rosen
} // namespace OHOS
