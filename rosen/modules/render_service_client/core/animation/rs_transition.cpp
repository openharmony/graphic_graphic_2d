/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "animation/rs_transition.h"

#include "animation/rs_render_transition.h"
#include "command/rs_animation_command.h"
#include "modifier/rs_property.h"
#include "transaction/rs_transaction_proxy.h"
#include "ui/rs_node.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
RSTransition::RSTransition(const std::shared_ptr<const RSTransitionEffect>& effect, bool isTransitionIn)
    : isTransitionIn_(isTransitionIn), effect_(effect)
{}

void RSTransition::OnStart()
{
    if (isCustom_) {
        StartCustomTransition();
    }
    StartRenderTransition();
}

void RSTransition::OnUpdateStagingValue(bool isFirstStart)
{
    if (!isCustom_) {
        return;
    }
    auto& customEffects = isTransitionIn_ ? effect_->customTransitionInEffects_ : effect_->customTransitionOutEffects_;
    for (auto& customEffect : customEffects) {
        for (auto& [property, endValue] : customEffect->properties_) {
            property->SetValue(endValue);
        }
        customEffect->properties_.clear();
        customEffect->customTransitionEffects_.clear();
    }
}

void RSTransition::StartCustomTransition()
{
    std::vector<std::shared_ptr<RSRenderTransitionEffect>> transitionEffects;
    auto& customEffects = isTransitionIn_ ? effect_->customTransitionInEffects_ : effect_->customTransitionOutEffects_;
    for (auto& customEffect : customEffects) {
        transitionEffects.insert(transitionEffects.end(), customEffect->customTransitionEffects_.begin(),
            customEffect->customTransitionEffects_.end());
    }
    auto transition = std::make_shared<RSRenderTransition>(GetId(), transitionEffects, isTransitionIn_);
    auto interpolator = timingCurve_.GetInterpolator(GetDuration());
    transition->SetInterpolator(interpolator);
    UpdateParamToRenderAnimation(transition);
    StartCustomAnimation(transition);
}

void RSTransition::StartRenderTransition()
{
    auto target = GetTarget().lock();
    if (target == nullptr) {
        ROSEN_LOGE("Failed to start transition animation, target is null!");
        return;
    }

    std::vector<std::shared_ptr<RSRenderTransitionEffect>> transitionEffects;
    if (isTransitionIn_) {
        transitionEffects = effect_->transitionInEffects_;
    } else {
        transitionEffects = effect_->transitionOutEffects_;
    }
    auto transition = std::make_shared<RSRenderTransition>(GetId(), transitionEffects, isTransitionIn_);
    auto interpolator = timingCurve_.GetInterpolator(GetDuration());
    transition->SetInterpolator(interpolator);
    UpdateParamToRenderAnimation(transition);

    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCreateTransition>(target->GetId(), transition);
    target->AddCommand(command, target->IsRenderServiceNode(), target->GetFollowType(), target->GetId());
    if (target->NeedForcedSendToRemote()) {
        std::unique_ptr<RSCommand> commandForRemote =
            std::make_unique<RSAnimationCreateTransition>(target->GetId(), transition);
        target->AddCommand(commandForRemote, true, target->GetFollowType(), target->GetId());
    }
}
} // namespace Rosen
} // namespace OHOS
