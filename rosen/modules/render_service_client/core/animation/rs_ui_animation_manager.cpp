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

#include "animation/rs_ui_animation_manager.h"

#include "animation/rs_animation.h"
#include "animation/rs_render_animation.h"
#include "modifier/rs_modifier_manager.h"
#include "modifier/rs_property.h"
#include "modifier/rs_render_property.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
RSUIAnimationManager::RSUIAnimationManager()
{
    modifierManager_ = std::make_shared<RSModifierManager>();
}

void RSUIAnimationManager::AddAnimation(const std::shared_ptr<RSRenderAnimation>& animation,
    const std::shared_ptr<RSAnimation>& uiAnimation)
{
    AnimationId key = animation->GetAnimationId();
    if (animations_.find(key) != animations_.end()) {
        ROSEN_LOGE("RSUIAnimationManager::AddAnimation, The animation already exists when is added");
        return;
    }
    animations_.emplace(key, animation);
    uiAnimations_.emplace(key, uiAnimation);
    OnAnimationAdd(animation);
}

void RSUIAnimationManager::RemoveAnimation(AnimationId keyId)
{
    auto animationItr = animations_.find(keyId);
    if (animationItr == animations_.end()) {
        ROSEN_LOGE("RSUIAnimationManager::RemoveAnimation, The Animation does not exist when is deleted");
        return;
    }
    RemoveUIAnimation(keyId);
    OnAnimationRemove(animationItr->second);
    animations_.erase(animationItr);
}

void RSUIAnimationManager::RemoveUIAnimation(const AnimationId id)
{
    auto uiAnimationItr = uiAnimations_.find(id);
    if (uiAnimationItr == uiAnimations_.end()) {
        ROSEN_LOGE("RSUIAnimationManager::RemoveUIAnimation, The UIAnimation does not exist when is deleted");
        return;
    }
    uiAnimations_.erase(uiAnimationItr);
}

void RSUIAnimationManager::AddAnimatableProp(const PropertyId id,
    const std::shared_ptr<RSPropertyBase>& uiProperty,
    const std::shared_ptr<RSRenderPropertyBase>& renderProperty)
{
    properties_.emplace(id, std::make_pair(uiProperty, renderProperty));
}

const std::shared_ptr<RSRenderPropertyBase> RSUIAnimationManager::GetRenderProperty(
    const PropertyId id)
{
    auto iter = properties_.find(id);
    if (iter != properties_.end()) {
        return iter->second.second;
    }
    return {};
}

void RSUIAnimationManager::RemoveProperty(const PropertyId id)
{
    properties_.erase(id);
}

bool RSUIAnimationManager::Animate(int64_t time)
{
    // process animation
    bool hasRunningAnimation = false;

    // iterate and execute all animations, remove finished animations
    EraseIf(animations_, [this, &hasRunningAnimation, time](auto& iter) -> bool {
        auto& animation = iter.second;
        bool isFinished = UpdateAnimateValue(animation, time);
        if (isFinished) {
            OnAnimationFinished(animation);
            RemoveUIAnimation(animation->GetAnimationId());
        } else {
            hasRunningAnimation = animation->IsRunning() || hasRunningAnimation ;
        }
        return isFinished;
    });

    return hasRunningAnimation;
}

bool RSUIAnimationManager::UpdateAnimateValue(const std::shared_ptr<RSRenderAnimation>& animation, int64_t time)
{
    bool isFinished = animation->Animate(time);
    auto uiProperty = properties_[animation->GetPropertyId()].first;
    auto renderProperty = properties_[animation->GetPropertyId()].second;
    if (uiProperty != nullptr && renderProperty != nullptr) {
        uiProperty->UpdateShowingValue(renderProperty);
        uiProperty->MarkModifierDirty(modifierManager_);
    }
    return isFinished;
}

void RSUIAnimationManager::Draw()
{
    modifierManager_->Draw();
}

const std::shared_ptr<RSRenderAnimation> RSUIAnimationManager::GetAnimation(AnimationId id) const
{
    auto animationItr = animations_.find(id);
    if (animationItr == animations_.end()) {
        ROSEN_LOGE("RSUIAnimationManager::GetAnimation, animation [%" PRIu64 "] not found", id);
        return nullptr;
    }
    return animationItr->second;
}

void RSUIAnimationManager::OnAnimationRemove(const std::shared_ptr<RSRenderAnimation>& animation)
{
    auto iter = animationNum_.find(animation->GetPropertyId());
    if (iter == animationNum_.end()) {
        return;
    }

    iter->second--;
    if (iter->second == 0) {
        RemoveProperty(animation->GetPropertyId());
        animationNum_.erase(iter);
    }
}

void RSUIAnimationManager::OnAnimationAdd(const std::shared_ptr<RSRenderAnimation>& animation)
{
    auto iter = animationNum_.find(animation->GetPropertyId());
    if (iter == animationNum_.end()) {
        animationNum_.emplace(animation->GetPropertyId(), 1);
        return;
    }

    iter->second++;
}

void RSUIAnimationManager::OnAnimationFinished(const std::shared_ptr<RSRenderAnimation>& animation)
{
    auto uiAnimation = uiAnimations_[animation->GetAnimationId()];
    if (uiAnimation != nullptr) {
        uiAnimation->UIAnimationFinish();
    }
    OnAnimationRemove(animation);
    animation->Detach();
}
} // namespace Rosen
} // namespace OHOS
