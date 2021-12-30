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

#include "animation/rs_render_transition.h"

#include "pipeline/rs_render_node.h"
#include "platform/common/rs_log.h"
#include "transaction/rs_marshalling_helper.h"

namespace OHOS {
namespace Rosen {
RSRenderTransition::RSRenderTransition(AnimationId id, const RSTransitionEffect& effect) : RSRenderAnimation(id)
{
    effect_ = RSRenderTransitionEffect::CreateTransitionEffect(effect);
    effectType_ = effect;
}
#ifdef ROSEN_OHOS
bool RSRenderTransition::Marshalling(Parcel& parcel) const
{
    if (!RSRenderAnimation::Marshalling(parcel)) {
        return false;
    }
    if (!(RSMarshallingHelper::Marshalling(parcel, effectType_) && interpolator_->Marshalling(parcel))) {
        return false;
    }
    return true;
}

RSRenderTransition* RSRenderTransition::Unmarshalling(Parcel& parcel)
{
    RSRenderTransition* renderTransition = new RSRenderTransition();
    if (!renderTransition->ParseParam(parcel)) {
        ROSEN_LOGE("RenderTransition Unmarshalling Failed");
        delete renderTransition;
        return nullptr;
    }
    return renderTransition;
}

bool RSRenderTransition::ParseParam(Parcel& parcel)
{
    if (!RSRenderAnimation::ParseParam(parcel)) {
        ROSEN_LOGE("Unmarshalling Base Class Failed");
        return false;
    }
    RSTransitionEffect effect;
    if (!RSMarshallingHelper::Unmarshalling(parcel, effect)) {
        return false;
    }
    effect_ = RSRenderTransitionEffect::CreateTransitionEffect(effect);
    std::shared_ptr<RSInterpolator> interpolator(RSInterpolator::Unmarshalling(parcel));
    if (interpolator == nullptr) {
        return false;
    }
    SetInterpolator(interpolator);
    return true;
}
#endif
void RSRenderTransition::OnAnimate(float fraction)
{
    currentFraction_ = interpolator_->Interpolate(fraction);
    auto target = GetTarget();
    if (target == nullptr) {
        return;
    }
    target->SetDirty();
}

void RSRenderTransition::OnAttach()
{
    auto target = GetTarget();
    if (target == nullptr || effect_ == nullptr) {
        return;
    }
    target->GetAnimationManager().RegisterTransition(GetAnimationId(),
        [=](RSPaintFilterCanvas& canvas, const RSProperties& renderProperties) {
        if (effect_ == nullptr) {
            return;
        }
        effect_->OnTransition(canvas, renderProperties, currentFraction_);
    });
}

void RSRenderTransition::OnDetach()
{
    auto target = GetTarget();
    if (target == nullptr || effect_ == nullptr) {
        return;
    }
    target->GetAnimationManager().UnregisterTransition(GetAnimationId());
}
} // namespace Rosen
} // namespace OHOS
