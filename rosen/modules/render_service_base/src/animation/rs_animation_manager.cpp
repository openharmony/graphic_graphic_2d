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
#include "common/rs_optional_trace.h"
#include "pipeline/rs_dirty_region_manager.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_render_node.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
class RSRootRenderNode;

void RSAnimationManager::DumpAnimations(std::string& out) const
{
    if (animations_.empty()) {
        return;
    }
    const auto lengthTwo = 2;
    out += ", RSAnimationManager: [";
    for (auto[id, animation]: animations_) {
        if (!animation) {
            continue;
        }
        animation->DumpAnimation(out);
        out += ", ";
    }
    out = out.substr(0, out.length() - lengthTwo);
    out += "]";
}

void RSAnimationManager::AddAnimation(const std::shared_ptr<RSRenderAnimation>& animation)
{
    AnimationId key = animation->GetAnimationId();
    if (animations_.find(key) != animations_.end()) {
        ROSEN_LOGE("RSAnimationManager::AddAnimation, The animation already exists when is added");
        return;
    }
    auto it = std::find(pendingCancelAnimation_.begin(), pendingCancelAnimation_.end(), key);
    if (it != pendingCancelAnimation_.end()) {
        pendingCancelAnimation_.erase(it);
        ROSEN_LOGE("RSAnimationManager::AddAnimation, animation is a pendingCancelAnimation");
        return;
    }
    animations_.emplace(key, animation);
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
    animations_.erase(animationItr);
}

void RSAnimationManager::CancelAnimationByPropertyId(PropertyId id)
{
    EraseIf(animations_, [id, this](const auto& pair) {
        if (pair.second && (pair.second->GetPropertyId() == id)) {
            OnAnimationFinished(pair.second);
            return true;
        }
        return false;
    });
}

void RSAnimationManager::AttemptCancelAnimationByAnimationId(const std::vector<AnimationId>& animations)
{
    for (auto& animationId : animations) {
        bool isErased = EraseIf(animations_, [animationId, this](const auto& pair) {
            if (pair.second && (pair.first == animationId)) {
                OnAnimationFinished(pair.second);
                return true;
            }
            return false;
        });
        if (!isErased) {
            pendingCancelAnimation_.emplace_back(animationId);
        }
    }
}

void RSAnimationManager::FilterAnimationByPid(pid_t pid)
{
    ROSEN_LOGD("RSAnimationManager::FilterAnimationByPid removing all animations belong to pid %{public}llu",
        (unsigned long long)pid);
    // remove all animations belong to given pid (by matching higher 32 bits of animation id)
    EraseIf(animations_, [pid, this](const auto& pair) -> bool {
        if (ExtractPid(pair.first) != pid) {
            return false;
        }
        pair.second->Finish();
        pair.second->Detach();
        return true;
    });
}

uint32_t RSAnimationManager::GetAnimationsSize()
{
    return animations_.size();
}

pid_t RSAnimationManager::GetAnimationPid() const
{
    if (animations_.size() > 0) {
        // shift right 32-bit numbers to get pid
        return animations_.begin()->first >> 32;
    }
    return 0;
}

std::tuple<bool, bool, bool> RSAnimationManager::Animate(int64_t time, bool nodeIsOnTheTree)
{
    // process animation
    bool hasRunningAnimation = false;
    bool needRequestNextVsync = false;
    // isCalculateAnimationValue is embedded modify for stat animate frame drop
    bool isCalculateAnimationValue = false;
    rsRange_.Reset();
    rateDecider_.Reset();
    // iterate and execute all animations, remove finished animations
    EraseIf(animations_, [this, &hasRunningAnimation, time,
        &needRequestNextVsync, nodeIsOnTheTree, &isCalculateAnimationValue](auto& iter) -> bool {
        auto& animation = iter.second;
        if (!nodeIsOnTheTree && animation->GetRepeatCount() == -1) {
            hasRunningAnimation = animation->IsRunning() || hasRunningAnimation;
            return false;
        }
        bool isFinished = animation->Animate(time);
        if (isFinished) {
            isCalculateAnimationValue = true;
            OnAnimationFinished(animation);
        } else {
            hasRunningAnimation = animation->IsRunning() || hasRunningAnimation;
            needRequestNextVsync = animation->IsRunning() || needRequestNextVsync;
            isCalculateAnimationValue = animation->IsCalculateAniamtionValue() || isCalculateAnimationValue;

            auto range = animation->GetFrameRateRange();
            if (range.IsValid()) {
                rsRange_.Merge(range);
            }
            RS_OPTIONAL_TRACE_BEGIN("AddDecisionElement property id: [" + std::to_string(animation->GetPropertyId()) +
                "], animation id: [" + std::to_string(animation->GetAnimationId()) + "]");
            rateDecider_.AddDecisionElement(animation->GetPropertyId(), animation->GetAnimateVelocity(), range);
            RS_OPTIONAL_TRACE_END();
        }
        return isFinished;
    });
    rateDecider_.MakeDecision(frameRateGetFunc_);
    isCalculateAnimationValue = isCalculateAnimationValue && nodeIsOnTheTree;

    return { hasRunningAnimation, needRequestNextVsync, isCalculateAnimationValue };
}

void RSAnimationManager::SetRateDeciderEnable(bool enabled, const FrameRateGetFunc& func)
{
    rateDecider_.SetEnable(enabled);
    frameRateGetFunc_ = func;
}

void RSAnimationManager::SetRateDeciderScaleSize(float width, float height)
{
    rateDecider_.SetScaleReferenceSize(width, height);
}

const FrameRateRange& RSAnimationManager::GetDecideFrameRateRange() const
{
    return rateDecider_.GetFrameRateRange();
}

const FrameRateRange& RSAnimationManager::GetFrameRateRange() const
{
    return rsRange_;
}

const std::shared_ptr<RSRenderAnimation> RSAnimationManager::GetAnimation(AnimationId id) const
{
    auto animationItr = animations_.find(id);
    if (animationItr == animations_.end()) {
        ROSEN_LOGD("RSAnimationManager::GetAnimation, animation [%{public}" PRIu64 "] not found", id);
        return nullptr;
    }
    return animationItr->second;
}

void RSAnimationManager::OnAnimationFinished(const std::shared_ptr<RSRenderAnimation>& animation)
{
    NodeId targetId = animation->GetTargetId();
    AnimationId animationId = animation->GetAnimationId();

    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCallback>(targetId, animationId, FINISHED);
    RSMessageProcessor::Instance().AddUIMessage(ExtractPid(animationId), std::move(command));
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

void RSAnimationManager::RegisterPathAnimation(PropertyId propertyId, AnimationId animId)
{
    pathAnimations_[propertyId] = animId;
}

void RSAnimationManager::UnregisterPathAnimation(PropertyId propertyId, AnimationId animId)
{
    auto it = pathAnimations_.find(propertyId);
    if (it != pathAnimations_.end() && it->second == animId) {
        pathAnimations_.erase(it);
    }
}

std::shared_ptr<RSRenderAnimation> RSAnimationManager::QueryPathAnimation(PropertyId propertyId)
{
    auto it = pathAnimations_.find(propertyId);
    if (it == pathAnimations_.end() || it->second == 0) {
        return nullptr;
    }
    return GetAnimation(it->second);
}

void RSAnimationManager::RegisterParticleAnimation(PropertyId propertyId, AnimationId animId)
{
    particleAnimations_[propertyId] = animId;
}

void RSAnimationManager::UnregisterParticleAnimation(PropertyId propertyId, AnimationId animId)
{
    auto it = particleAnimations_.find(propertyId);
    if (it != particleAnimations_.end() && it->second == animId) {
        particleAnimations_.erase(it);
    }
}

const std::unordered_map<PropertyId, AnimationId>& RSAnimationManager::GetParticleAnimations()
{
    return particleAnimations_;
}
} // namespace Rosen
} // namespace OHOS
