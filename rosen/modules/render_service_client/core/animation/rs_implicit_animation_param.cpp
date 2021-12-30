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

#include "animation/rs_implicit_animation_param.h"

#include "platform/common/rs_log.h"
#include "animation/rs_transition.h"

namespace OHOS {
namespace Rosen {
RSImplicitAnimationParam::RSImplicitAnimationParam(const RSAnimationTimingProtocol& timingProtocol)
    : timingProtocol_(timingProtocol)
{}

ImplicitAnimationParamType RSImplicitAnimationParam::GetType() const
{
    return animationType_;
}

void RSImplicitAnimationParam::ApplyTimingProtocol(const std::shared_ptr<RSAnimation>& animation) const
{
    animation->SetDuration(timingProtocol_.GetDuration());
    animation->SetStartDelay(timingProtocol_.GetStartDelay());
    animation->SetSpeed(timingProtocol_.GetSpeed());
    animation->SetDirection(timingProtocol_.GetDirection());
    animation->SetAutoReverse(timingProtocol_.GetAutoReverse());
    animation->SetRepeatCount(timingProtocol_.GetRepeatCount());
    animation->SetFillMode(timingProtocol_.GetFillMode());
}

RSImplicitCurveAnimationParam::RSImplicitCurveAnimationParam(
    const RSAnimationTimingProtocol& timingProtocol, const RSAnimationTimingCurve& timingCurve)
    : RSImplicitAnimationParam(timingProtocol), timingCurve_(timingCurve)
{
    animationType_ = ImplicitAnimationParamType::CURVE;
}

RSImplicitKeyframeAnimationParam::RSImplicitKeyframeAnimationParam(
    const RSAnimationTimingProtocol& timingProtocol, const RSAnimationTimingCurve& timingCurve, float fraction)
    : RSImplicitAnimationParam(timingProtocol), timingCurve_(timingCurve), fraction_(fraction)
{
    animationType_ = ImplicitAnimationParamType::KEYFRAME;
}

RSImplicitPathAnimationParam::RSImplicitPathAnimationParam(const RSAnimationTimingProtocol& timingProtocol,
    const RSAnimationTimingCurve& timingCurve, const std::shared_ptr<RSMotionPathOption>& motionPathOption)
    : RSImplicitAnimationParam(timingProtocol), timingCurve_(timingCurve), motionPathOption_(motionPathOption)
{
    animationType_ = ImplicitAnimationParamType::PATH;
}

std::shared_ptr<RSAnimation> RSImplicitPathAnimationParam::CreateAnimation(
    const RSAnimatableProperty& property, const Vector2f& startValue, const Vector2f& endValue) const
{
    if (motionPathOption_ == nullptr) {
        ROSEN_LOGE("Failed to create path animation, motion path option is null!");
        return nullptr;
    }

    auto pathAnimation =
        std::make_shared<RSPathAnimation>(property, motionPathOption_->GetPath(), startValue, endValue);
    pathAnimation->SetBeginFraction(motionPathOption_->GetBeginFraction());
    pathAnimation->SetEndFraction(motionPathOption_->GetEndFraction());
    pathAnimation->SetRotationMode(motionPathOption_->GetRotationMode());
    pathAnimation->SetTimingCurve(timingCurve_);
    ApplyTimingProtocol(pathAnimation);
    return pathAnimation;
}

RSImplicitTransitionParam::RSImplicitTransitionParam(const RSAnimationTimingProtocol& timingProtocol,
    const RSAnimationTimingCurve& timingCurve, const RSTransitionEffect& effect)
    : RSImplicitAnimationParam(timingProtocol),
      timingCurve_(timingCurve),
      effect_(effect)
{
    animationType_ = ImplicitAnimationParamType::TRANSITION;
}

std::shared_ptr<RSAnimation> RSImplicitTransitionParam::CreateAnimation()
{
    auto transition = std::make_shared<RSTransition>(effect_);
    transition->SetTimingCurve(timingCurve_);
    ApplyTimingProtocol(transition);
    return transition;
}
} // namespace Rosen
} // namespace OHOS
