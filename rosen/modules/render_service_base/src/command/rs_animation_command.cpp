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

#include "command/rs_animation_command.h"

#include <memory>

#include "animation/rs_render_particle.h"
#include "common/rs_common_def.h"
#include "modifier/rs_render_modifier.h"
#include "modifier/rs_render_property.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {

namespace {
static AnimationCommandHelper::AnimationCallbackProcessor animationCallbackProcessor = nullptr;
}

void AnimationCommandHelper::AnimationCallback(
    RSContext& context, NodeId targetId, AnimationId animId, AnimationCallbackEvent event)
{
    if (animationCallbackProcessor != nullptr) {
        animationCallbackProcessor(targetId, animId, event);
    }
}

void AnimationCommandHelper::SetAnimationCallbackProcessor(AnimationCallbackProcessor processor)
{
    animationCallbackProcessor = processor;
}

void AnimationCommandHelper::CreateAnimation(
    RSContext& context, NodeId targetId, const std::shared_ptr<RSRenderAnimation>& animation)
{
    auto node = context.GetNodeMap().GetRenderNode<RSRenderNode>(targetId);
    if (node == nullptr) {
        return;
    }
    if (animation == nullptr) {
        RS_LOGE("AnimationCommandHelper::CreateAnimation, animation is nullptr");
        return;
    }
    node->GetAnimationManager().AddAnimation(animation);
    auto modifier = node->GetModifier(animation->GetPropertyId());
    if (modifier != nullptr) {
        animation->AttachRenderProperty(modifier->GetProperty());
    }
    auto currentTime = context.GetCurrentTimestamp();
    animation->SetStartTime(currentTime);
    animation->Attach(node.get());
    // register node as animating node
    context.RegisterAnimatingRenderNode(node);
}

void AnimationCommandHelper::CreateParticleAnimation(
    RSContext& context, NodeId targetId, const std::shared_ptr<RSRenderParticleAnimation>& animation)
{
    auto node = context.GetNodeMap().GetRenderNode<RSRenderNode>(targetId);
    if (node == nullptr) {
        return;
    }
    if (animation == nullptr) {
        RS_LOGE("AnimationCommandHelper::CreateParticleAnimation, animation is nullptr");
        return;
    }
    auto propertyId = animation->GetPropertyId();
    node->GetAnimationManager().AddAnimation(animation);
    auto property = std::make_shared<RSRenderProperty<RSRenderParticleVector>>(
        animation->GetRenderParticle(), propertyId);
    auto modifier = std::make_shared<RSParticlesRenderModifier>(property);
    node->AddModifier(modifier);
    animation->AttachRenderProperty(property);
    auto currentTime = context.GetCurrentTimestamp();
    animation->SetStartTime(currentTime);
    animation->Attach(node.get());
    // register node as animating node
    context.RegisterAnimatingRenderNode(node);
}

void AnimationCommandHelper::CancelAnimation(RSContext& context, NodeId targetId, PropertyId propertyId)
{
    auto node = context.GetNodeMap().GetRenderNode<RSRenderNode>(targetId);
    if (node == nullptr) {
        return;
    }

    auto& animationManager = node->GetAnimationManager();
    animationManager.CancelAnimationByPropertyId(propertyId);
}
} // namespace Rosen
} // namespace OHOS
