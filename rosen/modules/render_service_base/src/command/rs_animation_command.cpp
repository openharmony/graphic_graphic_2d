/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

namespace OHOS {
namespace Rosen {

namespace {
static AnimationCommandHelper::FinishCallbackProcessor finishCallbackProcessor = nullptr;
}

void AnimationCommandHelper::AnimationFinishCallback(RSContext& context, NodeId targetId, AnimationId animId)
{
    if (finishCallbackProcessor != nullptr) {
        finishCallbackProcessor(targetId, animId);
    }
}

void AnimationCommandHelper::SetFinishCallbackProcessor(FinishCallbackProcessor processor)
{
    finishCallbackProcessor = processor;
}

void AnimationCommandHelper::CreateAnimation(
    RSContext& context, NodeId targetId, const std::shared_ptr<RSRenderAnimation>& animation)
{
    auto node = context.GetNodeMap().GetRenderNode<RSRenderNode>(targetId);
    if (node == nullptr) {
        return;
    }
    node->GetAnimationManager().AddAnimation(animation);
    auto modifier = node->GetModifier(animation->GetPropertyId());
    if (modifier != nullptr) {
        animation->AttachRenderProperty(modifier->GetProperty());
    }
    animation->Attach(node.get());
    animation->Start();
    auto beginTime = context.GetTransactionTimestamp();
    auto currentTime = context.GetCurrentTimestamp();
    // If the animation is already finished
    if (beginTime != 0 && (currentTime - beginTime) > static_cast<unsigned long>(animation->GetDuration() * MS_TO_NS)) {
        animation->SetStartTime(beginTime);
        animation->Animate(currentTime);
    } else {
        animation->SetStartTime(currentTime);
    }
    // register node on animation add
    context.RegisterAnimatingRenderNode(node);
}
} // namespace Rosen
} // namespace OHOS
