/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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
#include "animation/rs_interpolating_spring_animation.h"
#include "animation/rs_keyframe_animation.h"
#include "animation/rs_motion_path_option.h"
#include "animation/rs_path_animation.h"
#include "animation/rs_spring_animation.h"
#include "animation/rs_transition.h"
#include "modifier/rs_extended_modifier.h"
#include "platform/common/rs_log.h"
#include "command/rs_node_showing_command.h"

namespace OHOS {
namespace Rosen {
RSImplicitAnimationParam::RSImplicitAnimationParam(
    const RSAnimationTimingProtocol& timingProtocol, ImplicitAnimationParamType type)
    : animationType_(type), timingProtocol_(timingProtocol)
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
    auto range = timingProtocol_.GetFrameRateRange();
    if (range.IsValid()) {
        animation->SetFrameRateRange(range);
    }
}

RSImplicitCancelAnimationParam::RSImplicitCancelAnimationParam(const RSAnimationTimingProtocol& timingProtocol)
    : RSImplicitAnimationParam(timingProtocol, ImplicitAnimationParamType::CANCEL)
{}

void RSImplicitCancelAnimationParam::AddPropertyToPendingSyncList(const std::shared_ptr<RSPropertyBase>& property)
{
    pendingSyncList_.emplace_back(property);
}

void RSImplicitCancelAnimationParam::SyncProperties()
{
    if (pendingSyncList_.empty()) {
        return;
    }

    // Create sync map
    RSNodeGetShowingPropertiesAndCancelAnimation::PropertiesMap RSpropertiesMap;
    RSNodeGetShowingPropertiesAndCancelAnimation::PropertiesMap RTpropertiesMap;
    for (auto& rsProperty : pendingSyncList_) {
        auto node = rsProperty->target_.lock();
        if (node == nullptr) {
            continue;
        }
        if (!node->HasPropertyAnimation(rsProperty->GetId()) || rsProperty->GetIsCustom()) {
            continue;
        }
        auto& propertiesMap = node->IsRenderServiceNode() ? RSpropertiesMap : RTpropertiesMap;
        propertiesMap.emplace(std::make_pair<NodeId, PropertyId>(node->GetId(), rsProperty->GetId()),
            std::make_pair<std::shared_ptr<RSRenderPropertyBase>, std::vector<AnimationId>>(
                nullptr, node->GetAnimationByPropertyId(rsProperty->GetId())));
    }
    pendingSyncList_.clear();

    if (!RSpropertiesMap.empty()) {
        ExecuteSyncPropertiesTask(std::move(RSpropertiesMap), true);
    }
    if (!RTpropertiesMap.empty()) {
        ExecuteSyncPropertiesTask(std::move(RTpropertiesMap), false);
    }
}

void RSImplicitCancelAnimationParam::ExecuteSyncPropertiesTask(
    RSNodeGetShowingPropertiesAndCancelAnimation::PropertiesMap&& propertiesMap, bool isRenderService)
{
    // create task and execute it in RS
    auto task = std::make_shared<RSNodeGetShowingPropertiesAndCancelAnimation>(1e8, std::move(propertiesMap));
    RSTransactionProxy::GetInstance()->ExecuteSynchronousTask(task, isRenderService);

    // Test if the task is executed successfully
    if (!task || !task->IsSuccess()) {
        ROSEN_LOGE("RSImplicitCancelAnimationParam::ExecuteSyncPropertiesTask failed to execute task.");
        return;
    }

    // Apply task result
    for (const auto& [key, value] : task->GetProperties()) {
        const auto& [nodeId, propertyId] = key;
        auto node = RSNodeMap::Instance().GetNode(nodeId);
        if (node == nullptr) {
            ROSEN_LOGE("RSImplicitCancelAnimationParam::ExecuteSyncPropertiesTask failed to get target node.");
            continue;
        }
        auto modifier = node->GetModifier(propertyId);
        if (modifier == nullptr) {
            ROSEN_LOGE("RSImplicitCancelAnimationParam::ExecuteSyncPropertiesTask failed to get target modifier.");
            continue;
        }
        auto property = modifier->GetProperty();
        if (property == nullptr) {
            ROSEN_LOGE("RSImplicitCancelAnimationParam::ExecuteSyncPropertiesTask failed to get target property.");
            continue;
        }
        node->CancelAnimationByProperty(propertyId, !property->GetIsCustom());
        const auto& [propertyValue, animations] = value;
        if (propertyValue != nullptr) {
            // successfully canceled RS animation and extract value, update ui value
            property->SetValueFromRender(propertyValue);
        } else {
            // property or node is not yet created in RS, just trigger a force update
            property->UpdateOnAllAnimationFinish();
        }
    }
}

std::shared_ptr<RSAnimation> RSImplicitCancelAnimationParam::CreateEmptyAnimation(
    std::shared_ptr<RSPropertyBase> property, const std::shared_ptr<RSPropertyBase>& startValue,
    const std::shared_ptr<RSPropertyBase>& endValue) const
{
    auto curveAnimation = std::make_shared<RSCurveAnimation>(property, endValue - startValue);
    curveAnimation->SetDuration(0);
    return curveAnimation;
}

RSImplicitCurveAnimationParam::RSImplicitCurveAnimationParam(
    const RSAnimationTimingProtocol& timingProtocol, const RSAnimationTimingCurve& timingCurve)
    : RSImplicitAnimationParam(timingProtocol, ImplicitAnimationParamType::CURVE), timingCurve_(timingCurve)
{}

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
    const RSAnimationTimingProtocol& timingProtocol, const RSAnimationTimingCurve& timingCurve,
    float fraction, int duration)
    : RSImplicitAnimationParam(timingProtocol, ImplicitAnimationParamType::KEYFRAME), timingCurve_(timingCurve),
      fraction_(fraction), duration_(duration)
{}

std::shared_ptr<RSAnimation> RSImplicitKeyframeAnimationParam::CreateAnimation(
    std::shared_ptr<RSPropertyBase> property, const bool& isCreateDurationKeyframe, const int& startDuration,
    const std::shared_ptr<RSPropertyBase>& startValue, const std::shared_ptr<RSPropertyBase>& endValue) const
{
    auto keyFrameAnimation = std::make_shared<RSKeyframeAnimation>(property);
    keyFrameAnimation->SetDurationKeyframe(isCreateDurationKeyframe);
    if (isCreateDurationKeyframe) {
        keyFrameAnimation->AddKeyFrame(startDuration, startDuration + duration_, endValue, timingCurve_);
    } else {
        keyFrameAnimation->AddKeyFrame(fraction_, endValue, timingCurve_);
    }
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

void RSImplicitKeyframeAnimationParam::AddKeyframe(std::shared_ptr<RSAnimation>& animation, const int startDuration,
    const std::shared_ptr<RSPropertyBase>& startValue, const std::shared_ptr<RSPropertyBase>& endValue) const
{
    if (animation == nullptr) {
        return;
    }

    auto keyframeAnimation = std::static_pointer_cast<RSKeyframeAnimation>(animation);
    if (keyframeAnimation != nullptr) {
        keyframeAnimation->AddKeyFrame(startDuration, startDuration + duration_, endValue, timingCurve_);
    }
}

RSImplicitPathAnimationParam::RSImplicitPathAnimationParam(const RSAnimationTimingProtocol& timingProtocol,
    const RSAnimationTimingCurve& timingCurve, const std::shared_ptr<RSMotionPathOption>& motionPathOption)
    : RSImplicitAnimationParam(timingProtocol, ImplicitAnimationParamType::PATH), timingCurve_(timingCurve),
      motionPathOption_(motionPathOption)
{}

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
    : RSImplicitAnimationParam(timingProtocol, ImplicitAnimationParamType::SPRING), timingCurve_(timingCurve)
{}

std::shared_ptr<RSAnimation> RSImplicitSpringAnimationParam::CreateAnimation(std::shared_ptr<RSPropertyBase> property,
    const std::shared_ptr<RSPropertyBase>& startValue, const std::shared_ptr<RSPropertyBase>& endValue) const
{
    auto springAnimation = std::make_shared<RSSpringAnimation>(property, startValue, endValue);
    springAnimation->SetTimingCurve(timingCurve_);
    springAnimation->SetIsCustom(property->GetIsCustom());
    ApplyTimingProtocol(springAnimation);
    return springAnimation;
}

RSImplicitInterpolatingSpringAnimationParam::RSImplicitInterpolatingSpringAnimationParam(
    const RSAnimationTimingProtocol& timingProtocol, const RSAnimationTimingCurve& timingCurve)
    : RSImplicitAnimationParam(timingProtocol, ImplicitAnimationParamType::INTERPOLATING_SPRING),
      timingCurve_(timingCurve)
{}

std::shared_ptr<RSAnimation> RSImplicitInterpolatingSpringAnimationParam::CreateAnimation(
    std::shared_ptr<RSPropertyBase> property, const std::shared_ptr<RSPropertyBase>& startValue,
    const std::shared_ptr<RSPropertyBase>& endValue) const
{
    auto interpolatingSpringAnimation =
        std::make_shared<RSInterpolatingSpringAnimation>(property, startValue, endValue);
    interpolatingSpringAnimation->SetTimingCurve(timingCurve_);
    interpolatingSpringAnimation->SetIsCustom(property->GetIsCustom());
    ApplyTimingProtocol(interpolatingSpringAnimation);
    return interpolatingSpringAnimation;
}

RSImplicitTransitionParam::RSImplicitTransitionParam(const RSAnimationTimingProtocol& timingProtocol,
    const RSAnimationTimingCurve& timingCurve, const std::shared_ptr<const RSTransitionEffect>& effect,
    bool isTransitionIn)
    : RSImplicitAnimationParam(timingProtocol, ImplicitAnimationParamType::TRANSITION), timingCurve_(timingCurve),
      isTransitionIn_(isTransitionIn), effect_(effect)
{}

std::shared_ptr<RSAnimation> RSImplicitTransitionParam::CreateAnimation()
{
    if (transition_ == nullptr) {
        transition_ = std::make_shared<RSTransition>(effect_, isTransitionIn_);
        transition_->SetTimingCurve(timingCurve_);
        ApplyTimingProtocol(transition_);
    }
    return transition_;
}

std::shared_ptr<RSAnimation> RSImplicitTransitionParam::CreateAnimation(const std::shared_ptr<RSPropertyBase>& property,
    const std::shared_ptr<RSPropertyBase>& startValue, const std::shared_ptr<RSPropertyBase>& endValue)
{
    auto& customEffects = isTransitionIn_ ? effect_->customTransitionInEffects_ : effect_->customTransitionOutEffects_;
    for (auto& customEffect : customEffects) {
        if (property->modifier_.lock() == std::static_pointer_cast<RSModifier>(customEffect->modifier_)) {
            customEffect->Custom(property, startValue, endValue);
            break;
        }
    }
    if (transition_ == nullptr) {
        transition_ = std::make_shared<RSTransition>(effect_, isTransitionIn_);
        transition_->SetTimingCurve(timingCurve_);
        transition_->SetIsCustom(true);
        ApplyTimingProtocol(transition_);
    }
    return transition_;
}
} // namespace Rosen
} // namespace OHOS
