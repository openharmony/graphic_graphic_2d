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

#include "animation/rs_implicit_animator.h"

#include "animation/rs_animation.h"
#include "animation/rs_animation_callback.h"
#include "animation/rs_animation_trace_utils.h"
#include "animation/rs_implicit_animation_param.h"
#include "animation/rs_path_animation.h"
#include "common/rs_optional_trace.h"
#include "modifier/rs_property.h"
#include "pipeline/rs_node_map.h"
#include "platform/common/rs_log.h"
#include "ui/rs_node.h"
#include "ui/rs_ui_director.h"

namespace OHOS {
namespace Rosen {
int RSImplicitAnimator::OpenImplicitAnimation(const RSAnimationTimingProtocol& timingProtocol,
    const RSAnimationTimingCurve& timingCurve, std::shared_ptr<AnimationFinishCallback>&& finishCallback,
    std::shared_ptr<AnimationRepeatCallback>&& repeatCallback)
{
    globalImplicitParams_.push({ timingProtocol, timingCurve, std::move(finishCallback),
        std::move(repeatCallback) });
    implicitAnimations_.push({});
    keyframeAnimations_.push({});
    durationKeyframeParams_.push({ false, 0, 0 });
    switch (timingCurve.type_) {
        case RSAnimationTimingCurve::CurveType::INTERPOLATING:
            if (timingProtocol.GetDuration() <= 0) {
                // Special case: if duration is 0, we need to cancel existing implicit animations
                BeginImplicitCancelAnimation();
            } else {
                // Begin default curve animation
                BeginImplicitCurveAnimation();
            }
            break;
        case RSAnimationTimingCurve::CurveType::SPRING:
            BeginImplicitSpringAnimation();
            break;
        case RSAnimationTimingCurve::CurveType::INTERPOLATING_SPRING:
            BeginImplicitInterpolatingSpringAnimation();
            break;
        default:
            ROSEN_LOGE("Wrong type of timing curve!");
            return 0;
    }
    return static_cast<int>(globalImplicitParams_.size()) - 1;
}

int RSImplicitAnimator::OpenImplicitAnimation(const RSAnimationTimingProtocol& timingProtocol,
    const RSAnimationTimingCurve& timingCurve, std::shared_ptr<AnimationFinishCallback>&& finishCallback)
{
    return OpenImplicitAnimation(timingProtocol, timingCurve, std::move(finishCallback), nullptr);
}

int RSImplicitAnimator::OpenImplicitAnimation(std::shared_ptr<AnimationFinishCallback>&& finishCallback)
{
    if (globalImplicitParams_.empty()) {
        // if current implicit animation params is empty, use default params, if no animation created, call finish
        // callback immediately
        return OpenImplicitAnimation(
            RSAnimationTimingProtocol::IMMEDIATE, RSAnimationTimingCurve::LINEAR, std::move(finishCallback));
    } else {
        // copy current implicit animation params and replace finish callback
        [[maybe_unused]] const auto& [protocol, curve, unused, unused_repeatCallback] = globalImplicitParams_.top();
        return OpenImplicitAnimation(protocol, curve, std::move(finishCallback));
    }
}

int RSImplicitAnimator::OpenImplicitAnimation(
    const RSAnimationTimingProtocol& timingProtocol, const RSAnimationTimingCurve& timingCurve)
{
    if (globalImplicitParams_.empty()) {
        // current implicit animation params is empty, use empty
        return OpenImplicitAnimation(timingProtocol, timingCurve, nullptr);
    } else {
        // copy current implicit animation callback and replace timing protocol and curve
        [[maybe_unused]] const auto& [protocol, curve, callback, unused_repeatCallback] = globalImplicitParams_.top();
        auto copyOfCallback = callback;
        return OpenImplicitAnimation(timingProtocol, timingCurve, std::move(copyOfCallback));
    }
}

void RSImplicitAnimator::CloseImplicitAnimationInner()
{
    globalImplicitParams_.pop();
    implicitAnimations_.pop();
    keyframeAnimations_.pop();
    durationKeyframeParams_.pop();
    EndImplicitAnimation();
}

void RSImplicitAnimator::ProcessEmptyAnimations(const std::shared_ptr<AnimationFinishCallback>& finishCallback)
{
    // If finish callback either 1. is null or 2. is referenced by any animation or implicitly parameters, we don't
    // do anything.
    if (finishCallback.use_count() != 1) {
        return;
    }
    auto protocol = std::get<RSAnimationTimingProtocol>(globalImplicitParams_.top());
    // we are the only one who holds the finish callback, if the callback is NOT timing sensitive, we need to
    // execute it asynchronously, in order to avoid timing issues.
    if (finishCallback->finishCallbackType_ == FinishCallbackType::TIME_INSENSITIVE || protocol.GetDuration() < 0) {
        ROSEN_LOGD("RSImplicitAnimator::CloseImplicitAnimation, No implicit animations created, execute finish "
                   "callback asynchronously");
        RSUIDirector::PostTask([finishCallback]() { finishCallback->Execute(); });
    } else {
        // we are the only one who holds the finish callback, if the callback is timing sensitive, we need to create
        // a delay task, in order to execute it on the right time.
        ROSEN_LOGD("RSImplicitAnimator::CloseImplicitAnimation, No implicit animations created, execute finish "
                   "callback on delay duration");
        RSUIDirector::PostDelayTask(
            [finishCallback]() { finishCallback->Execute(); }, static_cast<uint32_t>(protocol.GetDuration()));
    }
}

std::vector<std::shared_ptr<RSAnimation>> RSImplicitAnimator::CloseImplicitAnimation()
{
    if (globalImplicitParams_.empty() || implicitAnimations_.empty() || keyframeAnimations_.empty()) {
        ROSEN_LOGD("Failed to close implicit animation, need to open implicit animation firstly!");
        return {};
    }

    // Special case: if implicit animation param type is CANCEL, we need to cancel all implicit animations
    if (implicitAnimationParams_.top()->GetType() == ImplicitAnimationParamType::CANCEL) {
        std::static_pointer_cast<RSImplicitCancelAnimationParam>(implicitAnimationParams_.top())->SyncProperties();
    }

    const auto& finishCallback = std::get<const std::shared_ptr<AnimationFinishCallback>>(globalImplicitParams_.top());
    auto& currentAnimations = implicitAnimations_.top();
    auto& currentKeyframeAnimations = keyframeAnimations_.top();
    // if no implicit animation created by current implicit animation param, we need to take care of finish callback
    if (currentAnimations.empty() && currentKeyframeAnimations.empty()) {
        ProcessEmptyAnimations(finishCallback);
        CloseImplicitAnimationInner();
        return {};
    }
    std::vector<std::shared_ptr<RSAnimation>> resultAnimations;
    [[maybe_unused]] auto& [isDurationKeyframe, totalDuration, currentDuration] = durationKeyframeParams_.top();
    for (const auto& [animationInfo, keyframeAnimation] : currentKeyframeAnimations) {
        auto target = RSNodeMap::Instance().GetNode<RSNode>(animationInfo.first);
        if (target == nullptr) {
            ROSEN_LOGE("Failed to start implicit keyframe animation[%{public}" PRIu64 "], target is null!",
                keyframeAnimation->GetId());
            continue;
        }
        if (isDurationKeyframe) {
            keyframeAnimation->SetDuration(totalDuration);
        }
        // this will actually create the RSRenderKeyframeAnimation
        target->AddAnimation(keyframeAnimation);
        keyframeAnimation->SetFinishCallback(finishCallback);
        resultAnimations.emplace_back(keyframeAnimation);
    }

    for (const auto& [animation, nodeId] : currentAnimations) {
        animation->SetFinishCallback(finishCallback);
        resultAnimations.emplace_back(animation);
    }
    CloseImplicitAnimationInner();
    return resultAnimations;
}

void RSImplicitAnimator::BeginImplicitKeyFrameAnimation(float fraction, const RSAnimationTimingCurve& timingCurve)
{
    if (globalImplicitParams_.empty()) {
        ROSEN_LOGE("Failed to begin keyframe implicit animation, need to open implicit animation firstly!");
        return;
    }

    if (timingCurve.type_ != RSAnimationTimingCurve::CurveType::INTERPOLATING) {
        ROSEN_LOGE("Wrong type of timing curve!");
        return;
    }

    [[maybe_unused]] const auto& [protocol, unused_curve, unused, unused_repeatCallback] = globalImplicitParams_.top();
    if (protocol.GetDuration() <= 0) {
        ROSEN_LOGE("Failed to begin keyframe implicit animation, total duration is 0!");
        return;
    }
    auto keyframeAnimationParam =
        std::make_shared<RSImplicitKeyframeAnimationParam>(protocol, timingCurve, fraction, 0);
    PushImplicitParam(keyframeAnimationParam);
}

void RSImplicitAnimator::BeginImplicitKeyFrameAnimation(float fraction)
{
    if (globalImplicitParams_.empty()) {
        ROSEN_LOGE("Failed to begin keyframe implicit animation, need to open implicit animation firstly!");
        return;
    }

    BeginImplicitKeyFrameAnimation(fraction, std::get<RSAnimationTimingCurve>(globalImplicitParams_.top()));
}

void RSImplicitAnimator::EndImplicitKeyFrameAnimation()
{
    if (implicitAnimationParams_.empty() ||
        implicitAnimationParams_.top()->GetType() != ImplicitAnimationParamType::KEYFRAME) {
        ROSEN_LOGD("Failed to end keyframe implicit animation, need to begin keyframe implicit animation firstly!");
        return;
    }

    PopImplicitParam();
}

void RSImplicitAnimator::BeginImplicitDurationKeyFrameAnimation(int duration, const RSAnimationTimingCurve& timingCurve)
{
    if (globalImplicitParams_.empty()) {
        ROSEN_LOGE("Failed to begin keyframe implicit animation, need to open implicit animation firstly!");
        return;
    }

    if (timingCurve.type_ != RSAnimationTimingCurve::CurveType::INTERPOLATING) {
        ROSEN_LOGE("Wrong type of timing curve!");
        return;
    }

    [[maybe_unused]] const auto& [protocol, unused_curve, unused, unused_repeatCallback] = globalImplicitParams_.top();
    if (protocol.GetDuration() <= 0) {
        ROSEN_LOGE("Failed to begin duration keyframe implicit animation, total duration is 0!");
        return;
    }
    [[maybe_unused]] auto& [isDurationKeyframe, totalDuration, currentDuration] = durationKeyframeParams_.top();
    isDurationKeyframe = true;
    currentDuration = duration;
    auto keyframeAnimationParam =
        std::make_shared<RSImplicitKeyframeAnimationParam>(protocol, timingCurve, 0, duration);
    PushImplicitParam(keyframeAnimationParam);
}

void RSImplicitAnimator::EndImplicitDurationKeyFrameAnimation()
{
    if (implicitAnimationParams_.empty() ||
        implicitAnimationParams_.top()->GetType() != ImplicitAnimationParamType::KEYFRAME) {
        ROSEN_LOGE("Failed to end keyframe implicit animation, need to begin keyframe implicit animation firstly!");
        return;
    }
    [[maybe_unused]] auto& [isDurationKeyframe, totalDuration, currentDuration] = durationKeyframeParams_.top();
    totalDuration += currentDuration;
    currentDuration = 0;
    PopImplicitParam();
}

bool RSImplicitAnimator::NeedImplicitAnimation()
{
    return !implicitAnimationDisabled_ && !implicitAnimationParams_.empty() &&
           implicitAnimationParams_.top()->GetType() != ImplicitAnimationParamType::INVALID;
}

void RSImplicitAnimator::BeginImplicitCurveAnimation()
{
    // params sanity already checked in BeginImplicitAnimation, no need to check again.
    [[maybe_unused]] const auto& [protocol, curve, unused, unused_repeatCallback] = globalImplicitParams_.top();
    auto curveAnimationParam = std::make_shared<RSImplicitCurveAnimationParam>(protocol, curve);
    PushImplicitParam(curveAnimationParam);
}

void RSImplicitAnimator::EndImplicitAnimation()
{
    if (implicitAnimationParams_.empty() ||
        (implicitAnimationParams_.top()->GetType() != ImplicitAnimationParamType::CANCEL &&
        implicitAnimationParams_.top()->GetType() != ImplicitAnimationParamType::CURVE &&
        implicitAnimationParams_.top()->GetType() != ImplicitAnimationParamType::SPRING &&
        implicitAnimationParams_.top()->GetType() != ImplicitAnimationParamType::INTERPOLATING_SPRING)) {
        ROSEN_LOGE("Failed to end implicit animation, need to begin implicit animation firstly!");
        return;
    }

    PopImplicitParam();
}

void RSImplicitAnimator::BeginImplicitPathAnimation(const std::shared_ptr<RSMotionPathOption>& motionPathOption)
{
    if (globalImplicitParams_.empty()) {
        ROSEN_LOGE("Failed to begin path implicit animation, need to open implicit animation firstly!");
        return;
    }

    [[maybe_unused]] const auto& [protocol, curve, unused, unused_repeatCallback] = globalImplicitParams_.top();
    if (curve.type_ != RSAnimationTimingCurve::CurveType::INTERPOLATING) {
        ROSEN_LOGE("Wrong type of timing curve!");
        return;
    }

    if (protocol.GetDuration() <= 0) {
        ROSEN_LOGE("Failed to begin path implicit animation, total duration is 0!");
        return;
    }
    auto pathAnimationParam = std::make_shared<RSImplicitPathAnimationParam>(protocol, curve, motionPathOption);
    PushImplicitParam(pathAnimationParam);
}

void RSImplicitAnimator::EndImplicitPathAnimation()
{
    if (implicitAnimationParams_.empty() ||
        implicitAnimationParams_.top()->GetType() != ImplicitAnimationParamType::PATH) {
        ROSEN_LOGE("Failed to end path implicit animation, need to begin path implicit animation firstly!");
        return;
    }

    PopImplicitParam();
}

void RSImplicitAnimator::BeginImplicitSpringAnimation()
{
    // params sanity already checked in BeginImplicitAnimation, no need to check again.
    [[maybe_unused]] const auto& [protocol, curve, unused, unused_repeatCallback] = globalImplicitParams_.top();
    auto springParam = std::make_shared<RSImplicitSpringAnimationParam>(protocol, curve);
    PushImplicitParam(springParam);
}

void RSImplicitAnimator::BeginImplicitInterpolatingSpringAnimation()
{
    // params sanity already checked in BeginImplicitAnimation, no need to check again.
    [[maybe_unused]] const auto& [protocol, curve, unused, unused_repeatCallback] = globalImplicitParams_.top();
    auto interpolatingSpringParam = std::make_shared<RSImplicitInterpolatingSpringAnimationParam>(protocol, curve);
    PushImplicitParam(interpolatingSpringParam);
}

void RSImplicitAnimator::BeginImplicitCancelAnimation()
{
    [[maybe_unused]] const auto& [protocol, curve, unused, unused_repeatCallback] = globalImplicitParams_.top();
    auto cancelImplicitParam = std::make_shared<RSImplicitCancelAnimationParam>(protocol);
    PushImplicitParam(cancelImplicitParam);
}

void RSImplicitAnimator::BeginImplicitTransition(
    const std::shared_ptr<const RSTransitionEffect>& effect, bool isTransitionIn)
{
    if (globalImplicitParams_.empty()) {
        ROSEN_LOGE("Failed to begin implicit transition, need to open implicit transition firstly!");
        return;
    }

    [[maybe_unused]] const auto& [protocol, curve, unused, unused_repeatCallback] = globalImplicitParams_.top();
    if (curve.type_ != RSAnimationTimingCurve::CurveType::INTERPOLATING) {
        ROSEN_LOGE("Wrong type of timing curve!");
        return;
    }
    auto transitionParam = std::make_shared<RSImplicitTransitionParam>(protocol, curve, effect, isTransitionIn);
    PushImplicitParam(transitionParam);
}

void RSImplicitAnimator::EndImplicitTransition()
{
    if (implicitAnimationParams_.empty() ||
        implicitAnimationParams_.top()->GetType() != ImplicitAnimationParamType::TRANSITION) {
        ROSEN_LOGE("Failed to end implicit transition, need to begin implicit transition firstly!");
        return;
    }

    PopImplicitParam();
}

void RSImplicitAnimator::PushImplicitParam(const std::shared_ptr<RSImplicitAnimationParam>& implicitParam)
{
    implicitAnimationParams_.emplace(implicitParam);
}

void RSImplicitAnimator::PopImplicitParam()
{
    if (implicitAnimationParams_.empty()) {
        ROSEN_LOGE("Failed to pop implicit params, params stack is empty!");
        return;
    }

    implicitAnimationParams_.pop();
}

void RSImplicitAnimator::CreateImplicitTransition(RSNode& target)
{
    if (globalImplicitParams_.empty() || implicitAnimations_.empty() || keyframeAnimations_.empty()) {
        ROSEN_LOGE("Failed to create implicit transition, need to open implicit transition firstly!");
        return;
    }
    auto& params = implicitAnimationParams_.top();
    if (params->GetType() != ImplicitAnimationParamType::TRANSITION) {
        ROSEN_LOGE("Failed to create transition, unknow type!");
        return;
    }
    auto transitionImplicitParam = std::static_pointer_cast<RSImplicitTransitionParam>(params);
    auto transition = transitionImplicitParam->CreateAnimation();
    if (transition != nullptr) {
        target.AddAnimation(transition);
        implicitAnimations_.top().push_back({ transition, target.GetId() });
    }
    return;
}

void RSImplicitAnimator::CancelImplicitAnimation(
    const std::shared_ptr<RSNode>& target, const std::shared_ptr<RSPropertyBase>& property)
{
    if (target == nullptr || property == nullptr) {
        return;
    }
    if (!target->HasPropertyAnimation(property->GetId())) {
        return;
    }
    auto params = implicitAnimationParams_.top();
    if (params->GetType() != ImplicitAnimationParamType::CANCEL) {
        return;
    }
    auto cancelImplicitParam = std::static_pointer_cast<RSImplicitCancelAnimationParam>(params);
    cancelImplicitParam->AddPropertyToPendingSyncList(property);
    return;
}

void RSImplicitAnimator::SetPropertyValue(
    std::shared_ptr<RSPropertyBase> property, const std::shared_ptr<RSPropertyBase>& value)
{
    if (property != nullptr) {
        property->SetValue(value);
    }
}

void RSImplicitAnimator::CreateImplicitAnimation(const std::shared_ptr<RSNode>& target,
    std::shared_ptr<RSPropertyBase> property, const std::shared_ptr<RSPropertyBase>& startValue,
    const std::shared_ptr<RSPropertyBase>& endValue)
{
    if (globalImplicitParams_.empty() || implicitAnimations_.empty() || keyframeAnimations_.empty()) {
        ROSEN_LOGE("Failed to create implicit animation, need to open implicit animation firstly!");
        return;
    }

    if (target == nullptr || property == nullptr) {
        return;
    }

    [[maybe_unused]] const auto& [protocol, curve, unused, unused_repeatCallback] = globalImplicitParams_.top();
    if (protocol.GetRepeatCount() == -1 && property->id_ == 0) {
        ROSEN_LOGE("Failed to create infinite empty animation!");
        return;
    }

    std::shared_ptr<RSAnimation> animation;
    auto params = implicitAnimationParams_.top();
    auto& repeatCallback = std::get<std::shared_ptr<AnimationRepeatCallback>>(globalImplicitParams_.top());
    switch (params->GetType()) {
        case ImplicitAnimationParamType::CURVE: {
            auto curveImplicitParam = static_cast<RSImplicitCurveAnimationParam*>(params.get());
            animation = curveImplicitParam->CreateAnimation(property, startValue, endValue);
            break;
        }
        case ImplicitAnimationParamType::KEYFRAME: {
            auto keyframeImplicitParam = static_cast<RSImplicitKeyframeAnimationParam*>(params.get());
            auto& keyframeAnimations = keyframeAnimations_.top();
            auto keyframeIter = keyframeAnimations.find({ target->GetId(), property->GetId() });
            [[maybe_unused]] auto& [isDurationKeyframe, totalDuration, unused] = durationKeyframeParams_.top();
            SetPropertyValue(property, endValue);
            if (keyframeIter == keyframeAnimations.end()) {
                animation = keyframeImplicitParam->CreateAnimation(property, isDurationKeyframe, totalDuration,
                    startValue, endValue);
                keyframeAnimations[{ target->GetId(), property->GetId() }] = animation;
            } else {
                if (isDurationKeyframe) {
                    keyframeImplicitParam->AddKeyframe(keyframeIter->second, totalDuration, startValue, endValue);
                } else {
                    keyframeImplicitParam->AddKeyframe(keyframeIter->second, startValue, endValue);
                }
            }
            break;
        }
        case ImplicitAnimationParamType::SPRING: {
            auto springImplicitParam = static_cast<RSImplicitSpringAnimationParam*>(params.get());
            animation = springImplicitParam->CreateAnimation(property, startValue, endValue);
            const auto& finishCallback =
                std::get<const std::shared_ptr<AnimationFinishCallback>>(globalImplicitParams_.top());
            if (finishCallback && finishCallback->finishCallbackType_ == FinishCallbackType::LOGICALLY) {
                animation->SetZeroThreshold(property->GetThreshold());
            }
            break;
        }
        case ImplicitAnimationParamType::INTERPOLATING_SPRING: {
            auto interpolatingSpringImplicitParam =
                static_cast<RSImplicitInterpolatingSpringAnimationParam*>(params.get());
            animation = interpolatingSpringImplicitParam->CreateAnimation(property, startValue, endValue);
            const auto& finishCallback =
                std::get<const std::shared_ptr<AnimationFinishCallback>>(globalImplicitParams_.top());
            if (finishCallback && finishCallback->finishCallbackType_ == FinishCallbackType::LOGICALLY) {
                animation->SetZeroThreshold(property->GetThreshold());
            }
            break;
        }
        case ImplicitAnimationParamType::PATH: {
            auto pathImplicitParam = static_cast<RSImplicitPathAnimationParam*>(params.get());
            animation = pathImplicitParam->CreateAnimation(property, startValue, endValue);
            break;
        }
        case ImplicitAnimationParamType::TRANSITION: {
            auto implicitTransitionParam = static_cast<RSImplicitTransitionParam*>(params.get());
            animation = implicitTransitionParam->CreateAnimation(property, startValue, endValue);
            break;
        }
        case ImplicitAnimationParamType::CANCEL: {
            // CreateEmptyAnimation
            if (property->id_ == 0) {
                auto curveImplicitParam = static_cast<RSImplicitCancelAnimationParam*>(params.get());
                animation = curveImplicitParam->CreateEmptyAnimation(property, startValue, endValue);
                break;
            }

            // Create animation with CANCEL type will cancel all running animations of the given property and target.
            // Note: We are currently in the process of refactoring and accidentally changed the order of animation
            // callbacks. Originally, the order was OnChange before OnFinish, but we mistakenly changed it to OnFinish
            // before OnChange. This change has caused some issues, and we need to revert it back to the original order.
            // However, before fixing this, we discovered that there are some changes in arkui that rely on this 'bug'.
            // If we change it back to the original order, it will break the list swipe animation. Therefore, we need
            // to carefully consider the implications of this change before proceeding.
            if (property->GetIsCustom()) {
                property->SetValue(endValue);                         // update set ui value
                property->UpdateCustomAnimation();                    // force sync RS value for custom property
                target->CancelAnimationByProperty(property->GetId()); // finish all ui animation
            } else {
                target->FinishAnimationByProperty(property->GetId()); // finish all ui animation
                property->SetValue(endValue);                         // update set ui value
                property->UpdateOnAllAnimationFinish();               // force sync RS value for native property
            }
            return;
        }
        default:
            ROSEN_LOGE("Failed to create animation, unknow type!");
            break;
    }

    if (animation == nullptr) {
        ROSEN_LOGE("Failed to create animation!");
        return;
    }
    if (repeatCallback != nullptr) {
        animation->SetRepeatCallback(std::move(repeatCallback));
        repeatCallback.reset();
    }

    RSAnimationTraceUtils::GetInstance().addAnimationCreateTrace(target->GetId(), property->GetId(), animation->GetId(),
        static_cast<int>(params->GetType()), static_cast<int>(property->type_), startValue->GetRenderProperty(),
        endValue->GetRenderProperty(), animation->GetStartDelay(), animation->GetDuration());

    if (params->GetType() == ImplicitAnimationParamType::TRANSITION ||
        params->GetType() == ImplicitAnimationParamType::KEYFRAME) {
        // for transition this will create custom transition animation, there is no need to add it to target.
        // for keyframe animations, we don't add it to target now, we will add it later in
        // RSImplicitAnimator::CloseImplicitAnimation.
        return;
    }
    target->AddAnimation(animation);
    implicitAnimations_.top().emplace_back(animation, target->GetId());

    return;
}

void RSImplicitAnimator::ExecuteWithoutAnimation(const std::function<void()>& callback)
{
    if (callback == nullptr) {
        return;
    }
    // disable implicit animation and execute callback, restore previous state after callback.
    auto implicitAnimationDisabled = implicitAnimationDisabled_;
    implicitAnimationDisabled_ = true;
    callback();
    implicitAnimationDisabled_ = implicitAnimationDisabled;
}

void RSImplicitAnimator::CreateImplicitAnimationWithInitialVelocity(const std::shared_ptr<RSNode>& target,
    const std::shared_ptr<RSPropertyBase>& property, const std::shared_ptr<RSPropertyBase>& startValue,
    const std::shared_ptr<RSPropertyBase>& endValue, const std::shared_ptr<RSPropertyBase>& velocity)
{
    if (globalImplicitParams_.empty() || implicitAnimations_.empty() || keyframeAnimations_.empty()) {
        ROSEN_LOGE("RSImplicitAnimator::CreateImplicitAnimationWithInitialVelocity:Failed to create implicit "
                   "animation, need to open implicit animation firstly!");
        return;
    }

    if (target == nullptr || property == nullptr) {
        ROSEN_LOGE(
            "RSImplicitAnimator::CreateImplicitAnimationWithInitialVelocity:target node or property is a nullptr.");
        return;
    }

    std::shared_ptr<RSAnimation> animation;
    auto params = implicitAnimationParams_.top();
    if (!params || params->GetType() != ImplicitAnimationParamType::SPRING) {
        ROSEN_LOGE(
            "RSImplicitAnimator::CreateImplicitAnimationWithInitialVelocity:the parameters of animations are invalid.");
        return;
    }

    auto springImplicitParam = static_cast<RSImplicitSpringAnimationParam*>(params.get());
    if (!springImplicitParam) {
        ROSEN_LOGE("RSImplicitAnimator::CreateImplicitAnimationWithInitialVelocity:the parameter of spring animations "
                   "is null.");
        return;
    }

    animation = springImplicitParam->CreateAnimation(property, startValue, endValue);
    if (!animation) {
        ROSEN_LOGE("RSImplicitAnimator::CreateImplicitAnimationWithInitialVelocity: failed to create animation.");
        return;
    }

    animation->SetInitialVelocity(velocity);
    const auto& finishCallback = std::get<const std::shared_ptr<AnimationFinishCallback>>(globalImplicitParams_.top());
    if (finishCallback && finishCallback->finishCallbackType_ == FinishCallbackType::LOGICALLY) {
        animation->SetZeroThreshold(property->GetThreshold());
    }

    auto& repeatCallback = std::get<std::shared_ptr<AnimationRepeatCallback>>(globalImplicitParams_.top());
    if (repeatCallback) {
        animation->SetRepeatCallback(std::move(repeatCallback));
        repeatCallback.reset();
    }

    target->AddAnimation(animation);
    implicitAnimations_.top().emplace_back(animation, target->GetId());
}
} // namespace Rosen
} // namespace OHOS
