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

#include "animation/rs_animation_manager.h"

#include <algorithm>
#include <string>

#include "animation/rs_render_animation.h"
#include "command/rs_animation_command.h"
#include "command/rs_message_processor.h"
#include "pipeline/rs_dirty_region_manager.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_render_node.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
class RSRootRenderNode;

void RSAnimationManager::AddAnimation(const std::shared_ptr<RSRenderAnimation>& animation)
{
    AnimationId key = animation->GetAnimationId();
    if (animations_.find(key) != animations_.end()) {
        ROSEN_LOGE("RSAnimationManager::AddAnimation, The animation already exists when is added");
        return;
    }
    animations_.emplace(key, animation);
    OnAnimationAdd(animation);
}

void RSAnimationManager::RemoveAnimation(AnimationId keyId)
{
    auto animationItr = animations_.find(keyId);
    if (animationItr == animations_.end()) {
        ROSEN_LOGE("RSAnimationManager::RemoveAnimation, The Animation does not exist when is deleted");
        return;
    }
    animationItr->second->Finish();
    animationItr->second->Detach();
    OnAnimationRemove(animationItr->second);
    animations_.erase(animationItr);
}

void RSAnimationManager::FilterAnimationByPid(pid_t pid)
{
    ROSEN_LOGI("RSAnimationManager::FilterAnimationByPid removing all animations belong to pid %d", pid);
    // remove all animations belong to given pid (by matching higher 32 bits of animation id)
    EraseIf(animations_, [pid, this](const auto& pair) -> bool {
        if (static_cast<pid_t>(pair.first >> 32) != pid) {
            return false;
        }
        pair.second->Finish();
        pair.second->Detach();
        OnAnimationRemove(pair.second);
        return true;
    });
}

std::pair<bool, bool> RSAnimationManager::Animate(int64_t time, bool nodeIsOnTheTree)
{
    // process animation
    bool hasRunningAnimation = false;
    bool needRequestNextVsync = false;
    // iterate and execute all animations, remove finished animations
    EraseIf(animations_, [this, &hasRunningAnimation, time,
        &needRequestNextVsync, nodeIsOnTheTree](auto& iter) -> bool {
        auto& animation = iter.second;
        if (!nodeIsOnTheTree && animation->GetRepeatCount() == -1) {
            hasRunningAnimation = animation->IsRunning() || hasRunningAnimation;
            return false;
        }
        bool isFinished = animation->Animate(time);
        if (isFinished) {
            OnAnimationFinished(animation);
        } else {
            hasRunningAnimation = animation->IsRunning() || hasRunningAnimation;
            needRequestNextVsync = animation->IsRunning() || needRequestNextVsync;
        }
        return isFinished;
    });

    return { hasRunningAnimation, needRequestNextVsync };
}

const std::shared_ptr<RSRenderAnimation> RSAnimationManager::GetAnimation(AnimationId id) const
{
    auto animationItr = animations_.find(id);
    if (animationItr == animations_.end()) {
        ROSEN_LOGE("RSAnimationManager::GetAnimation, animation [%" PRIu64 "] not found", id);
        return nullptr;
    }
    return animationItr->second;
}

void RSAnimationManager::OnAnimationRemove(const std::shared_ptr<RSRenderAnimation>& animation)
{
    animationNum_[animation->GetPropertyId()]--;
}

void RSAnimationManager::OnAnimationAdd(const std::shared_ptr<RSRenderAnimation>& animation)
{
    animationNum_[animation->GetPropertyId()]++;
}

void RSAnimationManager::OnAnimationFinished(const std::shared_ptr<RSRenderAnimation>& animation)
{
    NodeId targetId = animation->GetTargetId();
    AnimationId animationId = animation->GetAnimationId();

    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationFinishCallback>(targetId, animationId);
    RSMessageProcessor::Instance().AddUIMessage(ExtractPid(animationId), command);
    OnAnimationRemove(animation);
    animation->Detach();
}

void RSAnimationManager::RegisterSpringAnimation(PropertyId propertyId, AnimationId animId)
{
    springAnimations_[propertyId] = animId;
}

void RSAnimationManager::UnregisterSpringAnimation(PropertyId propertyId, AnimationId animId)
{
    auto it = springAnimations_.find(propertyId);
    if (it != springAnimations_.end() && it->second == animId) {
        springAnimations_.erase(it);
    }
}

std::shared_ptr<RSRenderAnimation> RSAnimationManager::QuerySpringAnimation(PropertyId propertyId)
{
    auto it = springAnimations_.find(propertyId);
    if (it == springAnimations_.end() || it->second == 0) {
        return nullptr;
    }
    return GetAnimation(it->second);
}
} // namespace Rosen
} // namespace OHOS
