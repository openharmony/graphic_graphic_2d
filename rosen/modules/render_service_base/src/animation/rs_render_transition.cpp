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

#include "animation/rs_render_transition.h"

#include "animation/rs_transition_effect.h"
#include "pipeline/rs_render_node.h"
#include "platform/common/rs_log.h"
#include "transaction/rs_marshalling_helper.h"

namespace OHOS {
namespace Rosen {
RSRenderTransition::RSRenderTransition(
    AnimationId id, const std::shared_ptr<const RSTransitionEffect>& effect, bool isTransitionIn)
    : RSRenderAnimation(id), isTransitionIn_(isTransitionIn)
{
    if (isTransitionIn) {
        effects_ = effect->GetTransitionInEffects();
    } else {
        effects_ = effect->GetTransitionOutEffects();
    }
}

#ifdef ROSEN_OHOS
bool RSRenderTransition::Marshalling(Parcel& parcel) const
{
    if (!RSRenderAnimation::Marshalling(parcel)) {
        ROSEN_LOGE("RSRenderTransition::Marshalling, step1 failed");
        return false;
    }
    if (!RSMarshallingHelper::Marshalling(parcel, effects_) ||
        !RSMarshallingHelper::Marshalling(parcel, isTransitionIn_) ||
        !interpolator_->Marshalling(parcel)) {
        ROSEN_LOGE("RSRenderTransition::Marshalling, step2 failed");
        return false;
    }
    return true;
}

RSRenderTransition* RSRenderTransition::Unmarshalling(Parcel& parcel)
{
    RSRenderTransition* renderTransition = new RSRenderTransition();
    if (!renderTransition->ParseParam(parcel)) {
        ROSEN_LOGE("RSRenderTransition::Unmarshalling, ParseParam Failed");
        delete renderTransition;
        return nullptr;
    }
    return renderTransition;
}

bool RSRenderTransition::ParseParam(Parcel& parcel)
{
    if (!RSRenderAnimation::ParseParam(parcel)) {
        ROSEN_LOGE("RSRenderTransition::ParseParam, RenderAnimation failed");
        return false;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, effects_)) {
        ROSEN_LOGE("RSRenderTransition::ParseParam, effect failed");
        return false;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, isTransitionIn_)) {
        ROSEN_LOGE("RSRenderTransition::ParseParam, transition direction failed");
        return false;
    }
    std::shared_ptr<RSInterpolator> interpolator(RSInterpolator::Unmarshalling(parcel));
    if (interpolator == nullptr) {
        ROSEN_LOGE("RSRenderTransition::ParseParam, interpolator is nullptr");
        return false;
    }
    SetInterpolator(interpolator);
    return true;
}
#endif
void RSRenderTransition::OnAnimate(float fraction)
{
    currentFraction_ = interpolator_->Interpolate(fraction);
    if (isTransitionIn_) {
        currentFraction_ = 1 - currentFraction_;
    }
    auto target = GetTarget();
    if (target == nullptr) {
        ROSEN_LOGE("RSRenderTransition::OnAnimate, target is nullptr");
        return;
    }
    target->SetDirty();
}

void RSRenderTransition::OnAttach()
{
    auto target = GetTarget();
    if (target == nullptr) {
        ROSEN_LOGE("RSRenderTransition::OnAttach, target is nullptr");
        return;
    }
    if (effects_.empty()) {
        ROSEN_LOGE("RSRenderTransition::OnAttach, effects is empty");
        return;
    }
    target->GetAnimationManager().RegisterTransition(
        GetAnimationId(), [=](const std::unique_ptr<RSTransitionProperties>& transitionProperties) {
            for (auto& effect : effects_) {
                effect->OnTransition(transitionProperties, currentFraction_);
            }
        });
}

void RSRenderTransition::OnDetach()
{
    auto target = GetTarget();
    if (target == nullptr) {
        ROSEN_LOGE("RSRenderTransition::OnDetach, target is nullptr");
        return;
    }
    target->GetAnimationManager().UnregisterTransition(GetAnimationId());
}
} // namespace Rosen
} // namespace OHOS
