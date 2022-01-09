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

#include "animation/rs_animation_group.h"

#include "animation/rs_animation_callback.h"
#include "platform/common/rs_log.h"
#include "ui/rs_node.h"

namespace OHOS {
namespace Rosen {
void RSAnimationGroup::AddAnimation(const std::shared_ptr<RSAnimation>& animation)
{
    if (animation == nullptr) {
        ROSEN_LOGE("Failed to add animation, adding animation is null!");
        return;
    }

    if (IsStarted()) {
        ROSEN_LOGE("Failed to add animation, group animation has started!");
        return;
    }

    if (animation->IsStarted()) {
        ROSEN_LOGE("Failed to add animation, adding animation has started!");
        return;
    }

    auto iter = find(animations_.begin(), animations_.end(), animation);
    if (iter != animations_.end()) {
        ROSEN_LOGE("Failed to add animation, animation already exists!");
        return;
    }

    animations_.emplace_back(animation);
}

void RSAnimationGroup::RemoveAnimation(const std::shared_ptr<RSAnimation>& animation)
{
    if (animation == nullptr) {
        ROSEN_LOGE("Failed to remove animation, animation is null!");
        return;
    }

    if (IsStarted()) {
        ROSEN_LOGE("Failed to remove animation, group animation has started!");
        return;
    }

    if (animation->IsStarted()) {
        ROSEN_LOGE("Failed to remove animation, removing animation has started!");
        return;
    }

    auto iter = find(animations_.begin(), animations_.end(), animation);
    if (iter == animations_.end()) {
        ROSEN_LOGE("Failed to remove animation, animation not exists!");
        return;
    }

    animations_.erase(iter);
}

void RSAnimationGroup::OnStart()
{
    if (animations_.empty()) {
        ROSEN_LOGE("Failed to start animations, animations is empty!");
        return;
    }

    auto target = target_.lock();
    if (target == nullptr) {
        ROSEN_LOGE("Failed to start group animation, target is null!");
        return;
    }

    std::shared_ptr<AnimationFinishCallback> finishCallback =
        std::make_shared<AnimationFinishCallback>([weakTarget = target_, weakAnimation = weak_from_this()]() {
            auto target = weakTarget.lock();
            if (target == nullptr) {
                ROSEN_LOGE("Failed to finish group animation, target is null!");
                return;
            }

            auto animation = weakAnimation.lock();
            if (animation == nullptr) {
                ROSEN_LOGE("Failed to finish group animation, group animation is null!");
                return;
            }

            target->RemoveAnimation(animation);
        });

    for (auto& animation : animations_) {
        int duration = duration_ < animation->GetDuration() ? duration_ : animation->GetDuration();
        int startDelay = startDelay_ + animation->GetStartDelay();

        animation->SetDuration(duration);
        animation->SetStartDelay(startDelay);
        animation->SetFinishCallback(finishCallback);
        animation->Start(target);
    }
}

void RSAnimationGroup::OnPause()
{
    if (animations_.empty()) {
        ROSEN_LOGE("Failed to pause animation, group animation is empty!");
        return;
    }

    for (auto& animation : animations_) {
        animation->Pause();
    }
}

void RSAnimationGroup::OnResume()
{
    if (animations_.empty()) {
        ROSEN_LOGE("Failed to resume animation, group animation is empty!");
        return;
    }

    for (auto& animation : animations_) {
        animation->Resume();
    }
}

void RSAnimationGroup::OnFinish()
{
    if (animations_.empty()) {
        ROSEN_LOGE("Failed to finish animation, group animation is empty!");
        return;
    }

    for (auto& animation : animations_) {
        animation->Finish();
    }
}

void RSAnimationGroup::OnReverse()
{
    if (animations_.empty()) {
        ROSEN_LOGE("Failed to reverse animation, group animation is empty!");
        return;
    }

    for (auto& animation : animations_) {
        animation->Reverse();
    }
}

void RSAnimationGroup::OnSetFraction(float fraction)
{
    if (animations_.empty()) {
        ROSEN_LOGE("Failed to set fraction, group animation is empty!");
        return;
    }

    for (auto& animation : animations_) {
        animation->SetFraction(fraction);
    }
}

RSAnimatableProperty RSAnimationGroup::GetProperty() const
{
    return RSAnimatableProperty::INVALID;
}
} // namespace Rosen
} // namespace OHOS
