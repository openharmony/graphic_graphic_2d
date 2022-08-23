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

#include "animation/rs_keyframe_animation.h"

#include "animation/rs_render_keyframe_animation.h"
#include "command/rs_animation_command.h"
#include "modifier/rs_property.h"
#include "transaction/rs_transaction_proxy.h"
#include "ui/rs_node.h"

namespace OHOS {
namespace Rosen {
RSKeyframeAnimation::RSKeyframeAnimation(std::shared_ptr<RSPropertyBase> property) : RSPropertyAnimation(property)
{}

void RSKeyframeAnimation::AddKeyFrame(float fraction, const std::shared_ptr<RSPropertyBase>& value,
    const RSAnimationTimingCurve& timingCurve)
{
    if (fraction < FRACTION_MIN || fraction > FRACTION_MAX) {
        return;
    }

    if (IsStarted()) {
        return;
    }

    keyframes_.push_back({ fraction, value, timingCurve });
}

void RSKeyframeAnimation::AddKeyFrames(
    const std::vector<std::tuple<float, std::shared_ptr<RSPropertyBase>, RSAnimationTimingCurve>>& keyframes)
{
    if (IsStarted()) {
        return;
    }

    keyframes_ = keyframes;
}

void RSKeyframeAnimation::InitInterpolationValue()
{
    if (keyframes_.empty()) {
        return;
    }

    auto beginKeyframe = keyframes_.front();
    if (std::abs(std::get<FRACTION_INDEX>(beginKeyframe) - FRACTION_MIN) > EPSILON) {
        keyframes_.insert(keyframes_.begin(), { FRACTION_MIN, GetOriginValue(), RSAnimationTimingCurve::LINEAR });
    }

    startValue_ = std::get<VALUE_INDEX>(keyframes_.front());
    endValue_ = std::get<VALUE_INDEX>(keyframes_.back());
    RSPropertyAnimation::InitInterpolationValue();
}

void RSKeyframeAnimation::StartRenderAnimation(const std::shared_ptr<RSRenderKeyframeAnimation>& animation)
{
    auto target = GetTarget().lock();
    if (target == nullptr) {
        ROSEN_LOGE("Failed to start keyframe animation, target is null!");
        return;
    }

    std::unique_ptr<RSCommand> command = std::make_unique<RSAnimationCreateKeyframe>(target->GetId(), animation);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, target->IsRenderServiceNode(), target->GetFollowType(), target->GetId());
        if (target->NeedForcedSendToRemote()) {
            std::unique_ptr<RSCommand> commandForRemote =
                std::make_unique<RSAnimationCreateKeyframe>(target->GetId(), animation);
            transactionProxy->AddCommand(commandForRemote, true, target->GetFollowType(), target->GetId());
        }
        if (target->NeedSendExtraCommand()) {
            std::unique_ptr<RSCommand> extraCommand =
                std::make_unique<RSAnimationCreateKeyframe>(target->GetId(), animation);
            transactionProxy->AddCommand(extraCommand, !target->IsRenderServiceNode(), target->GetFollowType(),
                target->GetId());
        }
    }
}

void RSKeyframeAnimation::StartUIAnimation(const std::shared_ptr<RSRenderKeyframeAnimation>& animation)
{
    StartCustomPropertyAnimation(animation);
}

void RSKeyframeAnimation::OnStart()
{
    RSPropertyAnimation::OnStart();
    if (keyframes_.empty()) {
        ROSEN_LOGE("Failed to start keyframe animation, keyframes is null!");
        return;
    }
    auto animation = std::make_shared<RSRenderKeyframeAnimation>(GetId(), GetPropertyId(),
        originValue_->CreateRenderProperty());
    for (const auto& [fraction, value, curve] : keyframes_) {
        animation->AddKeyframe(fraction, value->CreateRenderProperty(), curve.GetInterpolator(GetDuration()));
    }
    animation->SetAdditive(GetAdditive());
    UpdateParamToRenderAnimation(animation);
    if (isCustom_) {
        StartUIAnimation(animation);
    } else {
        StartRenderAnimation(animation);
    }
}
} // namespace Rosen
} // namespace OHOS
