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
#include "platform/common/rs_log.h"

static constexpr int DURATION_MIN = 0;
static constexpr int DURATION_VALUE_INDEX = 2;

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

void RSKeyframeAnimation::AddKeyFrame(int startDuration, int endDuration,
    const std::shared_ptr<RSPropertyBase>& value, const RSAnimationTimingCurve& timingCurve)
{
    if (startDuration > endDuration) {
        return;
    }

    if (IsStarted()) {
        return;
    }

    durationKeyframes_.push_back({ startDuration, endDuration, value, timingCurve });
}

void RSKeyframeAnimation::SetDurationKeyframe(bool isDuration)
{
    isDurationKeyframe_ = isDuration;
}

void RSKeyframeAnimation::InitInterpolationValue()
{
    if (isDurationKeyframe_) {
        if (durationKeyframes_.empty()) {
            return;
        }

        auto beginKeyframe = durationKeyframes_.front();
        durationKeyframes_.insert(durationKeyframes_.begin(),
            { DURATION_MIN, DURATION_MIN, GetOriginValue(), RSAnimationTimingCurve::LINEAR });

        startValue_ = std::get<DURATION_VALUE_INDEX>(durationKeyframes_.front());
        endValue_ = std::get<DURATION_VALUE_INDEX>(durationKeyframes_.back());
        RSPropertyAnimation::InitInterpolationValue();
        return;
    }

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
    }
}

void RSKeyframeAnimation::StartUIAnimation(const std::shared_ptr<RSRenderKeyframeAnimation>& animation)
{
    StartCustomAnimation(animation);
}

void RSKeyframeAnimation::OnStart()
{
    RSPropertyAnimation::OnStart();
    if (keyframes_.empty() && durationKeyframes_.empty()) {
        ROSEN_LOGE("Failed to start keyframe animation, keyframes is null!");
        return;
    }
    auto animation = std::make_shared<RSRenderKeyframeAnimation>(GetId(), GetPropertyId(),
        originValue_->GetRenderProperty());
    animation->SetDurationKeyframe(isDurationKeyframe_);
    animation->SetAdditive(GetAdditive());
    UpdateParamToRenderAnimation(animation);
    if (isDurationKeyframe_) {
        for (const auto& [startDuration, endDuration, value, curve] : durationKeyframes_) {
            animation->AddKeyframe(startDuration, endDuration, value->GetRenderProperty(),
                curve.GetInterpolator(GetDuration()));
        }
    } else {
        for (const auto& [fraction, value, curve] : keyframes_) {
            animation->AddKeyframe(fraction, value->GetRenderProperty(), curve.GetInterpolator(GetDuration()));
        }
    }
    if (isCustom_) {
        animation->AttachRenderProperty(property_->GetRenderProperty());
        StartUIAnimation(animation);
    } else {
        StartRenderAnimation(animation);
    }
}
} // namespace Rosen
} // namespace OHOS
