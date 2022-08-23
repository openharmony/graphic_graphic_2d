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

#include "animation/rs_implicit_animation_param.h"

#include "animation/rs_curve_animation.h"
#include "animation/rs_keyframe_animation.h"
#include "animation/rs_motion_path_option.h"
#include "animation/rs_path_animation.h"
#include "animation/rs_spring_animation.h"
#include "animation/rs_transition.h"
#include "platform/common/rs_log.h"

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

std::shared_ptr<RSAnimation> RSImplicitCurveAnimationParam::CreateAnimation(std::shared_ptr<RSPropertyBase> property,
    const std::shared_ptr<RSPropertyBase>& startValue, const std::shared_ptr<RSPropertyBase>& endValue) const
{
    auto curveAnimation = std::make_shared<RSCurveAnimation>(property, endValue - startValue);
    curveAnimation->SetTimingCurve(timingCurve_);
    curveAnimation->SetIsCustom(property->GetIsCustom());
    ApplyTimingProtocol(curveAnimation);
    return curveAnimation;
}

RSImplicitKeyframeAnimationParam::RSImplicitKeyframeAnimationParam(
    const RSAnimationTimingProtocol& timingProtocol, const RSAnimationTimingCurve& timingCurve, float fraction)
    : RSImplicitAnimationParam(timingProtocol), timingCurve_(timingCurve), fraction_(fraction)
{
    animationType_ = ImplicitAnimationParamType::KEYFRAME;
}

std::shared_ptr<RSAnimation> RSImplicitKeyframeAnimationParam::CreateAnimation(
    std::shared_ptr<RSPropertyBase> property, const std::shared_ptr<RSPropertyBase>& startValue,
    const std::shared_ptr<RSPropertyBase>& endValue) const
{
    auto keyFrameAnimation = std::make_shared<RSKeyframeAnimation>(property);
    keyFrameAnimation->AddKeyFrame(fraction_, endValue, timingCurve_);
    keyFrameAnimation->SetOriginValue(startValue);
    keyFrameAnimation->SetIsCustom(property->GetIsCustom());
    ApplyTimingProtocol(keyFrameAnimation);
    return keyFrameAnimation;
}

void RSImplicitKeyframeAnimationParam::AddKeyframe(std::shared_ptr<RSAnimation>& animation,
    const std::shared_ptr<RSPropertyBase>& startValue, const std::shared_ptr<RSPropertyBase>& endValue) const
{
    if (animation == nullptr) {
        return;
    }

    auto keyframeAnimation = std::static_pointer_cast<RSKeyframeAnimation>(animation);
    if (keyframeAnimation != nullptr) {
        keyframeAnimation->AddKeyFrame(fraction_, endValue, timingCurve_);
    }
}

RSImplicitPathAnimationParam::RSImplicitPathAnimationParam(const RSAnimationTimingProtocol& timingProtocol,
    const RSAnimationTimingCurve& timingCurve, const std::shared_ptr<RSMotionPathOption>& motionPathOption)
    : RSImplicitAnimationParam(timingProtocol), timingCurve_(timingCurve), motionPathOption_(motionPathOption)
{
    animationType_ = ImplicitAnimationParamType::PATH;
}

std::shared_ptr<RSAnimation> RSImplicitPathAnimationParam::CreateAnimation(std::shared_ptr<RSPropertyBase> property,
        const std::shared_ptr<RSPropertyBase>& startValue, const std::shared_ptr<RSPropertyBase>& endValue) const
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
    pathAnimation->SetPathNeedAddOrigin(motionPathOption_->GetPathNeedAddOrigin());
    pathAnimation->SetTimingCurve(timingCurve_);
    ApplyTimingProtocol(pathAnimation);
    return pathAnimation;
}

RSImplicitSpringAnimationParam::RSImplicitSpringAnimationParam(
    const RSAnimationTimingProtocol& timingProtocol, const RSAnimationTimingCurve& timingCurve)
    : RSImplicitAnimationParam(timingProtocol), timingCurve_(timingCurve)
{
    animationType_ = ImplicitAnimationParamType::SPRING;
}

std::shared_ptr<RSAnimation> RSImplicitSpringAnimationParam::CreateAnimation(std::shared_ptr<RSPropertyBase> property,
    const std::shared_ptr<RSPropertyBase>& startValue, const std::shared_ptr<RSPropertyBase>& endValue) const
{
    auto springAnimation = std::make_shared<RSSpringAnimation>(property, startValue, endValue);
    springAnimation->SetTimingCurve(timingCurve_);
    springAnimation->SetIsCustom(property->GetIsCustom());
    ApplyTimingProtocol(springAnimation);
    return springAnimation;
}

RSImplicitTransitionParam::RSImplicitTransitionParam(const RSAnimationTimingProtocol& timingProtocol,
    const RSAnimationTimingCurve& timingCurve, const std::shared_ptr<const RSTransitionEffect>& effect)
    : RSImplicitAnimationParam(timingProtocol), timingCurve_(timingCurve), effect_(effect)
{
    animationType_ = ImplicitAnimationParamType::TRANSITION;
}

std::shared_ptr<RSAnimation> RSImplicitTransitionParam::CreateAnimation(bool appearing)
{
    auto transition = std::make_shared<RSTransition>(effect_, appearing);
    transition->SetTimingCurve(timingCurve_);
    ApplyTimingProtocol(transition);
    return transition;
}
} // namespace Rosen
} // namespace OHOS
