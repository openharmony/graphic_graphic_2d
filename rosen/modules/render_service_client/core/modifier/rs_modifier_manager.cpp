
/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "modifier/rs_modifier_manager.h"

#include "animation/rs_render_animation.h"
#include "command/rs_animation_command.h"
#include "command/rs_message_processor.h"
#include "modifier/rs_property_modifier.h"
#include "platform/common/rs_log.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {
void RSModifierManager::AddModifier(const std::shared_ptr<RSModifier>& modifier)
{
    modifiers_.insert(modifier);
}

void RSModifierManager::Draw()
{
    if (modifiers_.empty()) {
        return;
    }

    RS_TRACE_NAME("RSModifierManager Draw num:[" + std::to_string(modifiers_.size()) + "]");
    for (auto modifier : modifiers_) {
        RS_TRACE_NAME("RSModifier::Draw");
        modifier->UpdateToRender();
        modifier->SetDirty(false);
        modifier->ResetRSNodeExtendModifierDirty();
    }
    modifiers_.clear();
}

void RSModifierManager::AddAnimation(const std::shared_ptr<RSRenderAnimation>& animation)
{
    AnimationId key = animation->GetAnimationId();
    if (animations_.find(key) != animations_.end()) {
        ROSEN_LOGE("RSModifierManager::AddAnimation, The animation already exists when is added");
        return;
    }
    animations_.emplace(key, animation);

    std::shared_ptr<RSRenderDisplaySync> displaySync = std::make_shared<RSRenderDisplaySync>(animation);
    displaySync->SetExpectedFrameRateRange(animation->GetFrameRateRange());
    displaySyncs_.emplace(key, displaySync);
}

void RSModifierManager::RemoveAnimation(AnimationId keyId)
{
    auto animationItr = animations_.find(keyId);
    if (animationItr == animations_.end()) {
        ROSEN_LOGE("RSModifierManager::RemoveAnimation, The Animation does not exist when is deleted");
        return;
    }
    animations_.erase(animationItr);
    displaySyncs_.erase(keyId);
}

bool RSModifierManager::HasUIAnimation()
{
    return !animations_.empty();
}

bool RSModifierManager::Animate(int64_t time, int64_t vsyncPeriod)
{
    RS_TRACE_NAME("RunningCustomAnimation num:[" + std::to_string(animations_.size()) + "]");
    // process animation
    bool hasRunningAnimation = false;
    rateDecider_.Reset();

    // iterate and execute all animations, remove finished animations
    EraseIf(animations_, [this, &hasRunningAnimation, time, vsyncPeriod](auto& iter) -> bool {
        auto animation = iter.second.lock();
        if (animation == nullptr) {
            displaySyncs_.erase(iter.first);
            return true;
        }

        bool isFinished = false;
        AnimationId animId = animation->GetAnimationId();
        if (!JudgeAnimateWhetherSkip(animId, time, vsyncPeriod)) {
            isFinished = animation->Animate(time);
        }

        if (isFinished) {
            OnAnimationFinished(animation);
        } else {
            hasRunningAnimation = animation->IsRunning() || hasRunningAnimation;
            rateDecider_.AddDecisionElement(animation->GetPropertyId(),
                animation->GetAnimateVelocity(), animation->GetFrameRateRange());
        }
        return isFinished;
    });
    rateDecider_.MakeDecision(frameRateGetFunc_);

    return hasRunningAnimation;
}

void RSModifierManager::FlushStartAnimation(int64_t time)
{
    for (auto& iter : animations_) {
        auto animation = iter.second.lock();
        if (animation && animation->GetNeedUpdateStartTime()) {
            animation->SetStartTime(time);
        }
    }
}

bool RSModifierManager::JudgeAnimateWhetherSkip(AnimationId animId, int64_t time, int64_t vsyncPeriod)
{
    bool isSkip = false;
    if (!displaySyncs_.count(animId)) {
        return isSkip;
    }

    auto displaySync = displaySyncs_[animId];
    if (displaySync) {
        isSkip = displaySync->OnFrameSkip(time, vsyncPeriod, IsDisplaySyncEnabled());
    }

    return isSkip;
}

void RSModifierManager::SetFrameRateGetFunc(const FrameRateGetFunc& func)
{
    frameRateGetFunc_ = func;
}

const FrameRateRange& RSModifierManager::GetFrameRateRange() const
{
    return rateDecider_.GetFrameRateRange();
}

void RSModifierManager::OnAnimationFinished(const std::shared_ptr<RSRenderAnimation>& animation)
{
    NodeId targetId = animation->GetTargetId();
    AnimationId animationId = animation->GetAnimationId();
    displaySyncs_.erase(animationId);

    std::unique_ptr<RSCommand> command = std::make_unique<RSAnimationCallback>(targetId, animationId, FINISHED);
    RSMessageProcessor::Instance().AddUIMessage(ExtractPid(animationId), command);

    animation->Detach();
}

void RSModifierManager::RegisterSpringAnimation(PropertyId propertyId, AnimationId animId)
{
    springAnimations_[propertyId] = animId;
}

void RSModifierManager::UnregisterSpringAnimation(PropertyId propertyId, AnimationId animId)
{
    auto it = springAnimations_.find(propertyId);
    if (it != springAnimations_.end() && it->second == animId) {
        springAnimations_.erase(it);
    }
}

std::shared_ptr<RSRenderAnimation> RSModifierManager::QuerySpringAnimation(PropertyId propertyId)
{
    auto it = springAnimations_.find(propertyId);
    if (it == springAnimations_.end() || it->second == 0) {
        ROSEN_LOGD("RSModifierManager::QuerySpringAnimation: there is no spring animation on the current property.");
        return nullptr;
    }
    return GetAnimation(it->second);
}

const std::shared_ptr<RSRenderAnimation> RSModifierManager::GetAnimation(AnimationId id) const
{
    auto animationItr = animations_.find(id);
    if (animationItr == animations_.end()) {
        ROSEN_LOGD("RSModifierManager::GetAnimation, animation [%{public}" PRIu64 "] not found", id);
        return nullptr;
    }
    return animationItr->second.lock();
}

void RSModifierManager::SetDisplaySyncEnable(bool isDisplaySyncEnabled)
{
    isDisplaySyncEnabled_ = isDisplaySyncEnabled;
}

bool RSModifierManager::IsDisplaySyncEnabled() const
{
    return isDisplaySyncEnabled_;
}
} // namespace Rosen
} // namespace OHOS
