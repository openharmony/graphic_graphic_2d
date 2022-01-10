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

#include "animation/rs_animation_manager.h"

#include <algorithm>
#include <string>

#include "animation/rs_render_animation.h"
#include "command/rs_animation_command.h"
#include "command/rs_message_processor.h"
#include "pipeline/rs_dirty_region_manager.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_render_node_map.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
class RSRootRenderNode;

RSAnimationManager::RSAnimationManager(RSRenderNode* target) : target_(target) {}

void RSAnimationManager::AddAnimation(const std::shared_ptr<RSRenderAnimation>& animation)
{
    AnimationId key = animation->GetAnimationId();
    if (animations_.find(key) != animations_.end()) {
        ROSEN_LOGE("RSAnimationManager::AddAnimation, The animation already exists when is added");
        return;
    }
    animations_.emplace(key, animation);
    OnAnimationAdd(animation->GetProperty());
    animation->Attach(target_);
}

void RSAnimationManager::RemoveAnimation(AnimationId keyId)
{
    auto animationItr = animations_.find(keyId);
    if (animationItr == animations_.end()) {
        ROSEN_LOGE("RSAnimationManager::RemoveAnimation, The Animation does not exist when is deleted");
        return;
    }
    auto animation = animationItr->second;
    OnAnimationRemove(animation->GetProperty());
    animations_.erase(animationItr);
}

bool RSAnimationManager::Animate(int64_t time)
{
    if (animations_.size() == 0) {
        return false;
    }
    auto animationItr = animations_.begin();
    bool requestVsync = false;
    while (animationItr != animations_.end()) {
        auto& animation = animationItr->second;
        bool isFinished = animation->Animate(time);
        if (isFinished) {
            animationItr = animations_.erase(animationItr);
            OnAnimationFinished(animation);
        } else {
            if (animation->IsRunning()) {
                requestVsync = true;
            }
            animationItr++;
        }
    }

    return requestVsync;
}

std::shared_ptr<RSRenderAnimation> RSAnimationManager::GetAnimation(AnimationId id) const
{
    auto animationItr = animations_.find(id);
    if (animationItr == animations_.end()) {
        ROSEN_LOGE("RSAnimationManager::GetAnimation, animtor[%lld] is not found", id);
        return nullptr;
    }
    return animationItr->second;
}

void RSAnimationManager::OnAnimationRemove(const RSAnimatableProperty& property)
{
    animationNum_[property]--;
}

void RSAnimationManager::OnAnimationAdd(const RSAnimatableProperty& property)
{
    animationNum_[property]++;
}

void RSAnimationManager::OnAnimationFinished(std::shared_ptr<RSRenderAnimation>& animation)
{
    animation->Detach();
    OnAnimationRemove(animation->GetProperty());
    // todo AnimationFinishCallbackMessage
    std::shared_ptr<RSCommand> command =
        std::make_shared<RSAnimationFinishCallback>(target_->GetId(), animation->GetAnimationId());
    RSMessageProcessor::Instance().AddUIMessage(command);
}

void RSAnimationManager::UpdateDisappearingChildren(RSDirtyRegionManager& dirtyManager, const RSProperties* parent,
    bool parentDirty)
{
    if (disappearingChildren_.empty()) {
        return;
    }
    disappearingChildren_.remove_if([](std::weak_ptr<RSRenderNode>& child) {
        auto childNode = child.lock();
        if (childNode == nullptr) {
            ROSEN_LOGW("RSAnimationManager::UpdateDisappearingChildren, childNode is nullptr");
            return true;
        }
        bool needToDelete = !childNode->GetAnimationManager().HasTransition();
        if (needToDelete && childNode->IsPendingRemoval()) {
            RSRenderNodeMap::Instance().UnregisterRenderNode(childNode->GetId());
        }
        return needToDelete;
    });
    for (auto child : disappearingChildren_) {
        if (auto childNode = child.lock()) {
            childNode->Update(dirtyManager, parent, parentDirty);
        }
    }
}

void RSAnimationManager::RenderDisappearingChildren(RSPaintFilterCanvas& canvas)
{
    if (disappearingChildren_.empty()) {
        return;
    }
    for (auto child : disappearingChildren_) {
        if (auto childNode = child.lock()) {
            childNode->ProcessRenderBeforeChildren(canvas);
            childNode->ProcessRenderAfterChildren(canvas);
        }
    }
}

void RSAnimationManager::RegisterTransition(AnimationId id, const TransitionCallback& transition)
{
    ClearTransition(id);
    transition_.push_back({ id, transition });
}

void RSAnimationManager::UnregisterTransition(AnimationId id)
{
    ClearTransition(id);
}

void RSAnimationManager::ClearTransition(AnimationId id)
{
    if (transition_.empty()) {
        ROSEN_LOGE("RSAnimationManager::ClearTransition, transition_ is empty");
        return;
    }
    transition_.remove_if([&](std::pair<AnimationId, TransitionCallback>& transition) {
        return id == transition.first;
    });
}

void RSAnimationManager::DoTransition(RSPaintFilterCanvas& canvas, const RSProperties& renderProperties)
{
    if (transition_.empty()) {
        return;
    }
    for (auto& [animationId, transition] : transition_) {
        if (transition != nullptr) {
            transition(canvas, renderProperties);
        }
    }
}

void RSAnimationManager::AddDisappearingChild(std::weak_ptr<RSRenderNode> child)
{
    disappearingChildren_.emplace_back(child);
}

void RSAnimationManager::RemoveDisappearingChild(std::weak_ptr<RSRenderNode> child)
{
    // weak_ptr does not have operator==, use remove_if instead of remove
    disappearingChildren_.remove_if([&child](std::weak_ptr<RSRenderNode>& item) {
        return !(item.owner_before(child) || child.owner_before(item));
    });
}

bool RSAnimationManager::HasTransition()
{
    if (target_ == nullptr) {
        ROSEN_LOGE("RSAnimationManager::HasTransition, target_ is nullptr");
        return false;
    }
    bool hasTransition = !transition_.empty();
    if (target_->IsInstanceOf<RSRootRenderNode>()) {
        return hasTransition;
    }
    auto parentNode = RSBaseRenderNode::ReinterpretCast<RSRenderNode>(target_->GetParent().lock());
    if (parentNode == nullptr) {
        return hasTransition;
    }
    return (hasTransition | parentNode->GetAnimationManager().HasTransition());
}
} // namespace Rosen
} // namespace OHOS
