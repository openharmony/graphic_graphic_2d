/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "rsanimationbase_fuzzer.h"

#include <securec.h>

#include "animation/rs_animation_fraction.h"
#include "animation/rs_animation_manager.h"
#include "animation/rs_animation_timing_protocol.h"
#include "animation/rs_animation_trace_utils.h"
#include "animation/rs_cubic_bezier_interpolator.h"
#include "animation/rs_interpolator.h"
#include "animation/rs_render_animation.h"
#include "animation/rs_render_curve_animation.h"
#include "animation/rs_render_interactive_implict_animator_map.h"
#include "animation/rs_render_interactive_implict_animator.h"
#include "animation/rs_render_interpolating_spring_animation.h"
#include "animation/rs_render_keyframe_animation.h"
#include "animation/rs_render_path_animation.h"
#include "animation/rs_render_spring_animation.h"
#include "animation/rs_render_transition_effect.h"
#include "animation/rs_render_transition.h"
#include "animation/rs_spring_interpolator.h"
#include "animation/rs_spring_model.h"
#include "animation/rs_steps_interpolator.h"
#include "animation/rs_value_estimator.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_context.h"
#include "render/rs_path.h"

namespace OHOS {
    using namespace Rosen;
    namespace {
        constexpr size_t STR_LEN = 10;
        const uint8_t* data_ = nullptr;
        size_t size_ = 0;
        size_t pos;
    }

    /*
    * describe: get data from outside untrusted data(data_) which size is according to sizeof(T)
    * tips: only support basic type
    */
    template<class T>
    T GetData()
    {
        T object {};
        size_t objectSize = sizeof(object);
        if (data_ == nullptr || objectSize > size_ - pos) {
            return object;
        }
        errno_t ret = memcpy_s(&object, objectSize, data_ + pos, objectSize);
        if (ret != EOK) {
            return {};
        }
        pos += objectSize;
        return object;
    }

    /*
    * get a string from data_
    */
    std::string GetStringFromData(int strlen)
    {
        char cstr[strlen];
        cstr[strlen - 1] = '\0';
        for (int i = 0; i < strlen - 1; i++) {
            cstr[i] = GetData<char>();
        }
        std::string str(cstr);
        return str;
    }

    void RSAnimationFractionFuzzerTest()
    {
        // get data
        float animationScale = GetData<float>();
        int64_t time = GetData<int64_t>();
        bool isInStartDelay = GetData<bool>();
        bool isFinished = GetData<bool>();
        bool isRepeatFinished = GetData<bool>();
        bool isEnable = GetData<bool>();
        float fraction = GetData<float>();
        int remainTime = GetData<int>();
        ForwardDirection direction = GetData<ForwardDirection>();
        int64_t lastFrameTime = GetData<int64_t>();

        // test
        RSAnimationFraction::Init();
        RSAnimationFraction::GetAnimationScale();
        RSAnimationFraction::SetAnimationScale(animationScale);
        auto animationFraction = std::make_shared<RSAnimationFraction>();
        std::tie(fraction, isInStartDelay, isFinished, isRepeatFinished) =
            animationFraction->GetAnimationFraction(time);
        animationFraction->UpdateRemainTimeFraction(fraction, remainTime);
        animationFraction->GetRemainingRepeatCount();
        animationFraction->GetStartFraction();
        animationFraction->GetEndFraction();
        animationFraction->SetDirectionAfterStart(direction);
        animationFraction->SetLastFrameTime(lastFrameTime);
        animationFraction->GetLastFrameTime();
        animationFraction->GetCurrentIsReverseCycle();
        animationFraction->GetCurrentTimeFraction();
        animationFraction->SetRepeatCallbackEnable(isEnable);
        animationFraction->GetRepeatCallbackEnable();
        animationFraction->ResetFraction();
    }

    void RSAnimationManagerFuzzerTest()
    {
        // get data
        AnimationId keyId = GetData<AnimationId>();
        AnimationId id = GetData<AnimationId>();
        pid_t pid = GetData<pid_t>();
        int64_t time = GetData<int64_t>();
        bool nodeIsOnTheTree = GetData<bool>();
        RSSurfaceNodeAbilityState abilityState = GetData<RSSurfaceNodeAbilityState>();
        bool isEnable = GetData<bool>();
        const FrameRateGetFunc func;
        float width = GetData<float>();
        float height = GetData<float>();
        float scaleX = GetData<float>();
        float scaleY = GetData<float>();

        PropertyId propertyId = GetData<PropertyId>();
        AnimationId animId = GetData<AnimationId>();

        // test
        auto animationManager = std::make_shared<RSAnimationManager>();
        animationManager->RemoveAnimation(keyId);
        animationManager->CancelAnimationByPropertyId(propertyId);
        animationManager->GetAnimation(id);
        animationManager->FilterAnimationByPid(pid);
        animationManager->GetAnimationsSize();
        animationManager->GetAnimationPid();
        animationManager->Animate(time, nodeIsOnTheTree, abilityState);
        animationManager->RegisterSpringAnimation(propertyId, animId);
        animationManager->UnregisterSpringAnimation(propertyId, animId);
        animationManager->QuerySpringAnimation(propertyId);
        animationManager->RegisterPathAnimation(propertyId, animId);
        animationManager->UnregisterPathAnimation(propertyId, animId);
        animationManager->QueryPathAnimation(propertyId);
        animationManager->RegisterParticleAnimation(propertyId, animId);
        animationManager->UnregisterParticleAnimation(propertyId, animId);
        animationManager->GetParticleAnimations();
        animationManager->GetParticleAnimation();
        animationManager->GetFrameRateRange();
        animationManager->GetDecideFrameRateRange();
        animationManager->SetRateDeciderEnable(isEnable, func);
        animationManager->SetRateDeciderSize(width, height);
        animationManager->SetRateDeciderScale(scaleX, scaleY);
    }

    void RSAnimationTimingProtocolFuzzerTest()
    {
        // get data
        int duration = GetData<int>();
        int startDelay = GetData<int>();
        float speed = GetData<float>();
        int repeatCount = GetData<int>();
        bool autoReverse = GetData<bool>();
        FillMode fillMode = GetData<FillMode>();
        bool isForward = GetData<bool>();
        FrameRateRange range;
        FinishCallbackType finishCallbackType = FinishCallbackType::TIME_SENSITIVE;

        // test
        auto animationTimeProtocol = std::make_shared<RSAnimationTimingProtocol>();
        animationTimeProtocol->SetDuration(duration);
        animationTimeProtocol->SetStartDelay(startDelay);
        animationTimeProtocol->SetSpeed(speed);
        animationTimeProtocol->SetRepeatCount(repeatCount);
        animationTimeProtocol->SetAutoReverse(autoReverse);
        animationTimeProtocol->SetFillMode(fillMode);
        animationTimeProtocol->SetDirection(isForward);
        animationTimeProtocol->SetFrameRateRange(range);
        animationTimeProtocol->SetFinishCallbackType(finishCallbackType);
        animationTimeProtocol->GetDuration();
        animationTimeProtocol->GetStartDelay();
        animationTimeProtocol->GetSpeed();
        animationTimeProtocol->GetRepeatCount();
        animationTimeProtocol->GetAutoReverse();
        animationTimeProtocol->GetFillMode();
        animationTimeProtocol->GetDirection();
        animationTimeProtocol->GetFrameRateRange();
        animationTimeProtocol->GetFinishCallbackType();
    }

    void RSAnimationTraceUtilsFuzzerTest()
    {
        // get data
        const std::string nodeName = GetStringFromData(STR_LEN);
        const std::string info = GetStringFromData(STR_LEN);
        const uint64_t nodeId = GetData<uint64_t>();
        const uint64_t propertyId = GetData<uint64_t>();
        const uint64_t animationId = GetData<uint64_t>();
        const int propertyType = GetData<int>();
        const int animationType = GetData<int>();
        const int animationDelay = GetData<int>();
        const int animationDur = GetData<int>();
        const int repeat = GetData<int>();
        bool isAddLogInfo = GetData<bool>();
        auto startValue = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
        auto endValue = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
        auto value = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
        auto initialVelocity = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
        const float fraction = GetData<float>();
        const int64_t time = GetData<int64_t>();

        // test
        RSAnimationTraceUtils::GetInstance().addAnimationNameTrace(nodeName);
        RSAnimationTraceUtils::GetInstance().addAnimationFinishTrace(info, nodeId, animationId, isAddLogInfo);
        RSAnimationTraceUtils::GetInstance().addAnimationCreateTrace(
            nodeId, nodeName, propertyId, animationId, animationType, propertyType, startValue, endValue,
            animationDelay, animationDur, repeat);
        RSAnimationTraceUtils::GetInstance().addAnimationFrameTrace(
            nodeId, nodeName, animationId, propertyId, fraction, value, time, animationDur, repeat);
        RSAnimationTraceUtils::GetInstance().addSpringInitialVelocityTrace(
            propertyId, animationId, initialVelocity, value);
        RSAnimationTraceUtils::GetInstance().ParseRenderPropertyVaule(value);
    }

    void RSCubicBezierInterpolatorFuzzerTest()
    {
        // get data
        float x1 = GetData<float>();
        float x2 = GetData<float>();
        float y1 = GetData<float>();
        float y2 = GetData<float>();
        float input = GetData<float>();
        Parcel parcel;

        // test
        auto interpolator = std::make_shared<RSCubicBezierInterpolator>(x1, y1, x2, y2);
        interpolator->Interpolate(input);
        interpolator->InterpolateImpl(input);
        interpolator->GetType();
        interpolator->Marshalling(parcel);
        auto copyInterpolator =
            std::shared_ptr<RSCubicBezierInterpolator>(RSCubicBezierInterpolator::Unmarshalling(parcel));
    }

    void RSInterpolatorFuzzerTest1()
    {
        // get data
        Parcel parcel;
        int duration = GetData<int>();

        // test
        auto linearInterpolator = std::make_shared<LinearInterpolator>();
        linearInterpolator->Marshalling(parcel);
        auto copyLinearInterpolator =
            std::shared_ptr<LinearInterpolator>(LinearInterpolator::Unmarshalling(parcel));
        linearInterpolator->GetType();

        auto lambda = [](float value) -> float { return value; };
        auto customInterpolator = std::make_shared<RSCustomInterpolator>(lambda, duration);
        customInterpolator->Marshalling(parcel);
        auto copyCustomInterpolator =
            std::shared_ptr<RSCustomInterpolator>(RSCustomInterpolator::Unmarshalling(parcel));
        customInterpolator->GetType();
    }

    void RSInterpolatorFuzzerTest2()
    {
        Parcel parcel1;
        uint16_t type = GetData<uint16_t>();
        auto interpolator1 = RSInterpolator::Unmarshalling(parcel1);
        parcel1.WriteUint16(type);
        interpolator1 = RSInterpolator::Unmarshalling(parcel1);

        Parcel parcel2;
        auto interpolator2 = LinearInterpolator::Unmarshalling(parcel2);
        if (interpolator2) {
            interpolator2->GetType();
        }

        uint16_t id = GetData<uint16_t>();
        float time = GetData<float>();
        std::vector<float> times = { time };
        float value = GetData<float>();
        std::vector<float> values = { value };
        float input = GetData<float>();
        auto customInterpolator2 = std::make_shared<RSCustomInterpolator>(id, std::move(times), std::move(values));
        customInterpolator2->InterpolateImpl(input);
        customInterpolator2->InterpolateImpl(time);
        Parcel parcel3;
        customInterpolator2->Marshalling(parcel3);
        auto copyCustomInterpolator2 = RSCustomInterpolator::Unmarshalling(parcel3);
        if (copyCustomInterpolator2) {
            copyCustomInterpolator2->GetType();
        }
    }

    void RSSpringInterpolatorFuzzerTest()
    {
        // get data
        float response = GetData<float>();
        float dampingRatio = GetData<float>();
        float initialVelocity = GetData<float>();
        float fraction = GetData<float>();

        // test
        auto animation = std::make_shared<RSSpringInterpolator>(response, dampingRatio, initialVelocity);
        Parcel parcel;
        animation->Marshalling(parcel);
        auto copyInterpolator = std::shared_ptr<RSSpringInterpolator>(RSSpringInterpolator::Unmarshalling(parcel));
        animation->Interpolate(fraction);
    }

    void RSStepsInterpolatorFuzzerTest()
    {
        // get data
        int32_t steps = GetData<int32_t>();
        StepsCurvePosition position = GetData<StepsCurvePosition>();
        float fraction = GetData<float>();

        // test
        auto animation = std::make_shared<RSStepsInterpolator>(steps, position);
        Parcel parcel;
        animation->Marshalling(parcel);
        auto copyInterpolator = std::shared_ptr<RSStepsInterpolator>(RSStepsInterpolator::Unmarshalling(parcel));
        animation->Interpolate(fraction);
        animation->InterpolateImpl(fraction);
        animation->GetType();
    }

    void RSRenderCurveAnimationFuzzerTest1()
    {
        // get data
        auto animationId = GetData<AnimationId>();
        auto propertyId = GetData<PropertyId>();
        int32_t steps = GetData<int32_t>();
        StepsCurvePosition position = GetData<StepsCurvePosition>();
        auto property = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
        auto startValue = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
        auto endValue = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
        std::string out;

        // test
        auto animation = std::make_shared<RSRenderCurveAnimation>(
            animationId, propertyId, property, startValue, endValue);
        animation->DumpAnimationInfo(out);
        auto interpolator = std::make_shared<RSStepsInterpolator>(steps, position);
        animation->SetInterpolator(interpolator);
        animation->GetInterpolator();
        Parcel parcel;
        animation->Marshalling(parcel);
        auto copyAnimation = std::shared_ptr<RSRenderCurveAnimation>(RSRenderCurveAnimation::Unmarshalling(parcel));
    }

    void RSRenderCurveAnimationFuzzerTest2()
    {
        // get data
        auto animationId = GetData<AnimationId>();
        auto propertyId = GetData<PropertyId>();
        int32_t steps = GetData<int32_t>();
        StepsCurvePosition position = GetData<StepsCurvePosition>();
        auto property = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
        auto startValue = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
        auto endValue = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
        auto fraction = GetData<float>();

        // test
        auto animation = std::make_shared<RSRenderCurveAnimation>(
            animationId, propertyId, property, startValue, endValue);
        auto interpolator = std::make_shared<RSStepsInterpolator>(steps, position);
        animation->property_ = property;
        animation->SetInterpolator(interpolator);
        animation->OnSetFraction(fraction);
        animation->OnAnimateInner(fraction, interpolator);
        animation->InitValueEstimator();
        animation->OnSetFraction(fraction);
        animation->UpdateFractionAfterContinue();
        animation->OnAnimate(fraction);
        animation->OnAnimateInner(fraction, interpolator);
    }

    void RSRenderInteractiveImplictAnimatorAndMapFuzzerTest()
    {
        // get data
        auto animatorId = GetData<InteractiveImplictAnimatorId>();
        auto nodeId = GetData<NodeId>();
        auto animationId = GetData<AnimationId>();
        auto position = GetData<RSInteractiveAnimationPosition>();
        auto fraction = GetData<float>();
        std::vector<std::pair<NodeId, AnimationId>> animations;
        animations.push_back({ nodeId, animationId });
        RSContext context;

        // test
        auto interactiveAnimator = std::make_shared<RSRenderInteractiveImplictAnimator>(
            animatorId, context.weak_from_this());
        interactiveAnimator->AddAnimations(animations);
        interactiveAnimator->PauseAnimator();
        interactiveAnimator->ContinueAnimator();
        interactiveAnimator->FinishAnimator(position);
        interactiveAnimator->ReverseAnimator();
        interactiveAnimator->SetFractionAnimator(fraction);
        context.GetInteractiveImplictAnimatorMap().RegisterInteractiveImplictAnimator(interactiveAnimator);
        context.GetInteractiveImplictAnimatorMap().GetInteractiveImplictAnimator(animatorId);
        context.GetInteractiveImplictAnimatorMap().UnregisterInteractiveImplictAnimator(animatorId);
    }

    void RSRenderInterpolatingSpringAnimationFuzzerTest1()
    {
        // get data
        auto animationId = GetData<AnimationId>();
        auto propertyId = GetData<PropertyId>();
        auto property = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
        auto startValue = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
        auto endValue = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
        auto response = GetData<float>();
        auto dampingRatio = GetData<float>();
        auto normalizedInitialVelocity = GetData<float>();
        auto minimumAmplitudeRatio = GetData<float>();
        auto zeroThreshold = GetData<float>();
        std::string out;

        // test
        auto animation = std::make_shared<RSRenderInterpolatingSpringAnimation>(
            animationId, propertyId, property, startValue, endValue);
        animation->DumpAnimationInfo(out);
        animation->SetSpringParameters(response, dampingRatio, normalizedInitialVelocity, minimumAmplitudeRatio);
        animation->SetZeroThreshold(zeroThreshold);
        Parcel parcel;
        animation->Marshalling(parcel);
        auto copyAnimation = std::shared_ptr<RSRenderInterpolatingSpringAnimation>(
            RSRenderInterpolatingSpringAnimation::Unmarshalling(parcel));
    }

    void RSRenderInterpolatingSpringAnimationFuzzerTest2()
    {
        // get data
        auto animationId = GetData<AnimationId>();
        auto propertyId = GetData<PropertyId>();
        auto property = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
        auto startValue = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
        auto endValue = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
        auto response = GetData<float>();
        auto dampingRatio = GetData<float>();
        auto normalizedInitialVelocity = GetData<float>();
        auto minimumAmplitudeRatio = GetData<float>();
        auto zeroThreshold = GetData<float>();
        auto fraction = GetData<float>();
        auto time = GetData<int64_t>();

        // test
        auto animation = std::make_shared<RSRenderInterpolatingSpringAnimation>(
            animationId, propertyId, property, startValue, endValue);
        animation->property_ = property;
        animation->SetSpringParameters(response, dampingRatio, normalizedInitialVelocity, minimumAmplitudeRatio);
        animation->OnSetFraction(fraction);
        animation->OnAnimate(fraction);
        animation->CalculateVelocity(fraction);
        animation->SetZeroThreshold(zeroThreshold);

        animation->InitValueEstimator();
        animation->OnSetFraction(fraction);
        animation->UpdateFractionAfterContinue();
        animation->CalculateTimeFraction(fraction);
        animation->OnAnimate(fraction);
        animation->OnAnimate(1.0f);
        animation->OnInitialize(time);
        animation->CalculateVelocity(fraction);
        animation->GetNeedLogicallyFinishCallback();
        animation->CallLogicallyFinishCallback();
    }

    void RSRenderKeyframeAnimationFuzzerTest1()
    {
        // get data
        auto animationId = GetData<AnimationId>();
        auto propertyId = GetData<PropertyId>();
        int32_t steps = GetData<int32_t>();
        StepsCurvePosition position = GetData<StepsCurvePosition>();
        auto property = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
        auto startValue = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
        auto fraction = GetData<float>();
        auto startDuration = GetData<int>();
        auto endDuration = GetData<int>();
        auto isDuration = GetData<bool>();
        std::vector<std::tuple<float, std::shared_ptr<RSRenderPropertyBase>, std::shared_ptr<RSInterpolator>>>
            keyframes;
        std::string out;

        // test
        auto animation = std::make_shared<RSRenderKeyframeAnimation>(animationId, propertyId, property);
        animation->DumpAnimationInfo(out);
        auto interpolator = std::make_shared<RSStepsInterpolator>(steps, position);
        animation->AddKeyframe(fraction, startValue, interpolator);
        keyframes.push_back({ fraction, startValue, interpolator });
        animation->AddKeyframes(keyframes);
        animation->AddKeyframe(startDuration, endDuration, startValue, interpolator);
        animation->SetDurationKeyframe(isDuration);
        Parcel parcel;
        animation->Marshalling(parcel);
        auto copyAnimation = std::shared_ptr<RSRenderKeyframeAnimation>(
            RSRenderKeyframeAnimation::Unmarshalling(parcel));
    }

    void RSRenderKeyframeAnimationFuzzerTest2()
    {
        // get data
        auto animationId = GetData<AnimationId>();
        auto propertyId = GetData<PropertyId>();
        int32_t steps = GetData<int32_t>();
        StepsCurvePosition position = GetData<StepsCurvePosition>();
        auto property = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
        auto fraction = GetData<float>();

        // test
        auto animation = std::make_shared<RSRenderKeyframeAnimation>(animationId, propertyId, property);
        auto interpolator = std::make_shared<RSStepsInterpolator>(steps, position);
        animation->property_ = property;
        Parcel parcel;
        parcel.WriteFloat(fraction);
        parcel.WriteFloat(fraction);
        RSMarshallingHelper::Marshalling(parcel, property);
        interpolator->Marshalling(parcel);
        animation->ParseDurationKeyframesParam(parcel, 1);
        animation->OnAnimate(fraction);
        animation->InitValueEstimator();
        animation->OnAnimate(fraction);
    }

    void RSRenderPathAnimationFuzzerTest1()
    {
        // get data
        auto animationId = GetData<AnimationId>();
        auto propertyId = GetData<PropertyId>();
        int32_t steps = GetData<int32_t>();
        StepsCurvePosition position = GetData<StepsCurvePosition>();
        auto property = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
        auto startValue = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
        auto endValue = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
        auto originRotation = GetData<float>();
        auto rotationMode = GetData<RotationMode>();
        auto fraction = GetData<float>();
        auto isNeedPath = GetData<bool>();
        std::string out;

        // test
        auto animation = std::make_shared<RSRenderPathAnimation>(
            animationId, propertyId, property, startValue, endValue, originRotation, RSPath::CreateRSPath());
        animation->DumpAnimationInfo(out);
        auto interpolator = std::make_shared<RSStepsInterpolator>(steps, position);
        animation->SetInterpolator(interpolator);
        animation->GetInterpolator();
        animation->SetRotationMode(rotationMode);
        animation->GetRotationMode();
        animation->SetBeginFraction(fraction);
        animation->GetBeginFraction();
        animation->SetEndFraction(fraction);
        animation->GetEndFraction();
        animation->SetIsNeedPath(isNeedPath);
        animation->SetPathNeedAddOrigin(isNeedPath);
        animation->SetRotationId(propertyId);
        Parcel parcel;
        animation->Marshalling(parcel);
        auto copyAnimation = std::shared_ptr<RSRenderPathAnimation>(RSRenderPathAnimation::Unmarshalling(parcel));
    }

    void RSRenderPathAnimationFuzzerTest2()
    {
        // get data
        auto animationId = GetData<AnimationId>();
        auto propertyId = GetData<PropertyId>();
        NodeId nodeId = GetData<NodeId>();
        Vector4f value = Vector4f(GetData<float>(), GetData<float>(), GetData<float>(), GetData<float>());
        Vector2f value2 = Vector2f(GetData<float>(), GetData<float>());
        auto property = std::make_shared<RSRenderAnimatableProperty<Vector4f>>(value);
        auto startValue = std::make_shared<RSRenderAnimatableProperty<Vector4f>>(value);
        auto endValue = std::make_shared<RSRenderAnimatableProperty<Vector4f>>(value);
        auto originRotation = GetData<float>();
        auto rotationMode = GetData<RotationMode>();
        auto fraction = GetData<float>();
        auto isNeedPath = GetData<bool>();
        auto tangent = GetData<float>();
        auto renderNode = std::make_shared<RSRenderNode>(nodeId);

        // test
        auto animation1 = std::make_shared<RSRenderPathAnimation>(
            animationId, propertyId, property, startValue, endValue, originRotation, nullptr);
        animation1->OnAnimate(fraction);

        // no node
        auto animation2 = std::make_shared<RSRenderPathAnimation>(
            animationId, propertyId, property, startValue, endValue, originRotation, RSPath::CreateRSPath());
        animation2->property_ = property;
        animation2->SetIsNeedPath(isNeedPath);
        animation2->OnAnimate(fraction);
        animation2->OnAttach();
        animation2->OnRemoveOnCompletion();
        animation2->OnDetach();
        animation2->SetRotation(tangent);

        animation2->Attach(renderNode.get());
        animation2->InitValueEstimator();
        animation2->OnRemoveOnCompletion();
        animation2->OnAttach();
        animation2->OnDetach();
        animation2->SetPathValue(value2, tangent);
        animation2->SetPathValue(value, tangent);
        animation2->SetRotationMode(rotationMode);
        animation2->SetRotationValue(tangent);
        animation2->SetRotation(tangent);
        animation2->GetPosTanValue(fraction, value2, tangent);
        animation2->SetPathNeedAddOrigin(isNeedPath);
        animation2->UpdateVector2fPathValue(value2);
        animation2->UpdateVector4fPathValue(value, value2);
    }

    void RSCurveValueEstimatorFuzzerTest()
    {
        // get data
        bool isAdditive = GetData<bool>();
        float fraction = GetData<float>();
        int32_t steps = GetData<int32_t>();
        int duration = GetData<int>();
        StepsCurvePosition position = GetData<StepsCurvePosition>();
        auto property = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
        auto startValue = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
        auto endValue = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
        auto lastValue = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());

        // test
        auto curveValueEstimator = std::make_shared<RSCurveValueEstimator<float>>();
        curveValueEstimator->InitCurveAnimationValue(property, startValue, endValue, lastValue);
        curveValueEstimator->UpdateAnimationValue(fraction, isAdditive);
        curveValueEstimator->GetAnimationValue(fraction, isAdditive);
        auto interpolator = std::make_shared<RSStepsInterpolator>(steps, position);
        curveValueEstimator->EstimateFraction(interpolator);
        curveValueEstimator->EstimateFraction(interpolator, fraction, duration);
    }

    void RSKeyframeValueEstimatorFuzzerTest()
    {
        // get data
        bool isAdditive = GetData<bool>();
        float fraction = GetData<float>();
        int32_t steps = GetData<int32_t>();
        StepsCurvePosition position = GetData<StepsCurvePosition>();
        auto property = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
        auto startValue = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
        auto endValue = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
        auto lastValue = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
        std::vector<std::tuple<float, std::shared_ptr<RSRenderPropertyBase>, std::shared_ptr<RSInterpolator>>>
            keyframes;
        std::vector<std::tuple<float, float, std::shared_ptr<RSRenderPropertyBase>, std::shared_ptr<RSInterpolator>>>
            durationKeyframes;

        // test
        auto keyFrameValueEstimator = std::make_shared<RSKeyframeValueEstimator<float>>();
        auto interpolator = std::make_shared<RSStepsInterpolator>(steps, position);
        keyframes.push_back({ fraction, property, interpolator });
        durationKeyframes.push_back({ fraction, fraction, property, interpolator });
        keyFrameValueEstimator->InitKeyframeAnimationValue(property, keyframes, lastValue);
        keyFrameValueEstimator->InitDurationKeyframeAnimationValue(property, durationKeyframes, lastValue);
        keyFrameValueEstimator->UpdateAnimationValue(fraction, isAdditive);
        keyFrameValueEstimator->GetAnimationValue(fraction, isAdditive);
        keyFrameValueEstimator->GetDurationKeyframeAnimationValue(fraction, isAdditive);
    }

    void RSSpringValueEstimatorFuzzerTest()
    {
        // get data
        bool isAdditive = GetData<bool>();
        auto property = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
        auto startValue = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
        auto endValue = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
        auto lastValue = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
        float response = GetData<float>();
        float dampingRatio = GetData<float>();
        float time = GetData<float>();

        // test
        auto springBaseValueEstimator = std::make_shared<RSSpringValueEstimatorBase>();
        springBaseValueEstimator->SetResponse(response);
        springBaseValueEstimator->SetDampingRatio(dampingRatio);
        springBaseValueEstimator->GetResponse();
        springBaseValueEstimator->GetDampingRatio();
        springBaseValueEstimator->SetInitialVelocity(property);
        springBaseValueEstimator->InitRSSpringValueEstimator(property, startValue, endValue, lastValue);
        springBaseValueEstimator->UpdateStartValueAndLastValue(startValue, lastValue);
        springBaseValueEstimator->UpdateAnimationValue(time, isAdditive);
        springBaseValueEstimator->GetPropertyVelocity(time);
        springBaseValueEstimator->UpdateDuration();
        springBaseValueEstimator->UpdateSpringParameters();

        auto springValueEstimator = std::make_shared<RSSpringValueEstimator<float>>();
        springValueEstimator->InitSpringModel();
        springValueEstimator->SetResponse(response);
        springValueEstimator->SetDampingRatio(dampingRatio);
        springValueEstimator->GetResponse();
        springValueEstimator->GetDampingRatio();
        springValueEstimator->SetInitialVelocity(property);
        springValueEstimator->InitRSSpringValueEstimator(property, startValue, endValue, lastValue);
        springValueEstimator->UpdateStartValueAndLastValue(startValue, lastValue);
        springValueEstimator->UpdateAnimationValue(time, isAdditive);
        springValueEstimator->GetAnimationValue(time, isAdditive);
        springValueEstimator->GetAnimationProperty();
        springValueEstimator->GetPropertyVelocity(time);
        springValueEstimator->UpdateDuration();
        springValueEstimator->UpdateSpringParameters();
    }

    void RSSpringModelFuzzerTest()
    {
        // get data
        float initialOffset = GetData<float>();
        float initialVelocity = GetData<float>();
        float response = GetData<float>();
        float dampingRatio = GetData<float>();
        float minimumAmplitude = GetData<float>();
        double time = GetData<double>();

        // test
        std::make_shared<RSSpringModel<float>>();
        auto model1 = std::make_shared<RSSpringModel<float>>(
            response, dampingRatio, initialOffset, initialVelocity, minimumAmplitude);
        model1->CalculateDisplacement(time);
        model1->EstimateDuration();
    }

    void RSRenderTransitionFuzzerTest()
    {
        // get data
        AnimationId animationId = GetData<AnimationId>();

        bool isTransitionIn = GetData<bool>();
        std::string str = GetStringFromData(STR_LEN);
        int32_t steps = GetData<int32_t>();
        StepsCurvePosition position = GetData<StepsCurvePosition>();
        float alpha = GetData<float>();

        // test
        auto interpolator = std::make_shared<RSStepsInterpolator>(steps, position);
        std::shared_ptr<RSRenderTransitionEffect> child = std::make_shared<RSTransitionFade>(alpha);
        std::vector<std::shared_ptr<RSRenderTransitionEffect>> vec = { child };
        auto renderTransition = std::make_shared<RSRenderTransition>(animationId, vec, isTransitionIn);
        renderTransition->DumpAnimationInfo(str);
        renderTransition->SetInterpolator(interpolator);
        Parcel parcel;
        renderTransition->Marshalling(parcel);
        auto copyRenderTransition = std::shared_ptr<RSRenderTransition>(RSRenderTransition::Unmarshalling(parcel));
    }

    void RSRenderTransitionEffectFuzzerTest()
    {
        // get data
        float scaleX = GetData<float>();
        float scaleY = GetData<float>();
        float scaleZ = GetData<float>();
        float dx = GetData<float>();
        float dy = GetData<float>();
        float dz = GetData<float>();
        float radian = GetData<float>();
        float translateX = GetData<float>();
        float translateY = GetData<float>();
        float translateZ = GetData<float>();
        float alpha = GetData<float>();
        float fraction = GetData<float>();
        auto property = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
        auto startValue = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
        auto endValue = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
        Parcel parcel;

        // test
        auto transitionFade = std::make_shared<RSTransitionFade>(alpha);
        transitionFade->UpdateFraction(fraction);
        transitionFade->Marshalling(parcel);
        auto copyTransitionFade =
            std::shared_ptr<RSRenderTransitionEffect>(RSRenderTransitionEffect::Unmarshalling(parcel));

        auto transitionScale = std::make_shared<RSTransitionScale>(scaleX, scaleY, scaleZ);
        transitionScale->UpdateFraction(fraction);
        transitionScale->Marshalling(parcel);
        auto copyTransitionScale = std::shared_ptr<RSRenderTransitionEffect>(RSTransitionScale::Unmarshalling(parcel));

        auto transitionTranslate = std::make_shared<RSTransitionTranslate>(translateX, translateY, translateZ);
        transitionTranslate->UpdateFraction(fraction);
        transitionTranslate->Marshalling(parcel);
        auto copyTransitionTranslate =
            std::shared_ptr<RSRenderTransitionEffect>(RSTransitionTranslate::Unmarshalling(parcel));

        auto transitionRotate = std::make_shared<RSTransitionRotate>(dx, dy, dz, radian);
        transitionRotate->UpdateFraction(fraction);
        transitionRotate->Marshalling(parcel);
        auto copyTransitionRotate =
            std::shared_ptr<RSRenderTransitionEffect>(RSTransitionRotate::Unmarshalling(parcel));

        auto transitionCustom = std::make_shared<RSTransitionCustom>(property, startValue, endValue);
        transitionCustom->UpdateFraction(fraction);
    }

    void RSRenderSpringAnimationFuzzerTest()
    {
        // get data
        float response = GetData<float>();
        float dampingRatio = GetData<float>();
        float blendDuration = GetData<float>();
        float minimumAmplitudeRatio = GetData<float>();
        float zeroThreshold = GetData<float>();
        AnimationId animationId = GetData<AnimationId>();
        PropertyId propertyId = GetData<PropertyId>();
        auto originValue = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
        auto startValue = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
        auto endValue = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
        auto velocity = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
        std::string str = GetStringFromData(STR_LEN);

        // test
        std::shared_ptr<RSRenderCurveAnimation> animation =
            std::make_shared<RSRenderCurveAnimation>(animationId, propertyId, originValue, startValue, endValue);
        auto springAnimation =
            std::make_shared<RSRenderSpringAnimation>(animationId, propertyId, originValue, startValue, endValue);
        springAnimation->DumpAnimationInfo(str);
        springAnimation->SetSpringParameters(response, dampingRatio, blendDuration, minimumAmplitudeRatio);
        springAnimation->SetZeroThreshold(zeroThreshold);
        springAnimation->SetInitialVelocity(velocity);
        springAnimation->InheritSpringAnimation(animation);
    }

    void RSRenderAnimation1FuzzerTest()
    {
        // get data
        AnimationId animationId = GetData<AnimationId>();
        NodeId nodeId = GetData<NodeId>();
        bool bForceDetach = GetData<bool>();
        float fraction = GetData<float>();
        auto fillMode = GetData<FillMode>();
        auto time = GetData<int64_t>();
        auto animation = std::make_shared<RSRenderAnimation>(animationId);
        auto renderNode = std::make_shared<RSRenderNode>(nodeId);
        std::string nodeName = GetStringFromData(STR_LEN);
        renderNode->SetNodeName(nodeName);
        std::string outString;
        Parcel parcel;
        // test
        animation->Attach(renderNode.get());
        animation->DumpAnimation(outString);
        animation->DumpAnimationInfo(outString);
        animation->ParseParam(parcel);
        animation->IsStarted();
        animation->IsRunning();
        animation->IsPaused();
        animation->IsFinished();
        animation->GetPropertyId();
        animation->GetTargetName();
        animation->Detach(bForceDetach);
        animation->Attach(renderNode.get());
        animation->GetTarget();
        animation->SetFractionInner(fraction);
        animation->SetFillMode(fillMode);
        animation->ProcessFillModeOnStart(fraction);
        animation->ProcessFillModeOnFinish(fraction);
        animation->ProcessOnRepeatFinish();
        animation->SetStartTime(time);
        animation->GetAnimateVelocity();
    }

    void RSRenderAnimation2FuzzerTest()
    {
        // get data
        AnimationId animationId = GetData<AnimationId>();
        auto pos = GetData<RSInteractiveAnimationPosition>();
        float fraction = GetData<float>();
        bool isReversed = GetData<bool>();
        auto time = GetData<int64_t>();
        auto animation1 = std::make_shared<RSRenderAnimation>(animationId);

        // status error
        animation1->Finish();
        animation1->FinishOnPosition(pos);
        animation1->FinishOnCurrentPosition();
        animation1->Pause();
        animation1->Resume();
        animation1->SetFraction(fraction);
        animation1->SetReversedAndContinue();
        animation1->SetReversed(isReversed);
        animation1->Animate(time);

        // status normal
        animation1->Start();
        animation1->Pause();
        animation1->SetFraction(fraction);
        animation1->SetReversed(isReversed);
        animation1->SetReversedAndContinue();
        animation1->Animate(time);
        animation1->SetStartTime(time);
        animation1->Animate(time);
        animation1->animationFraction_.SetLastFrameTime(time);
        animation1->Animate(time);
        animation1->FinishOnPosition(pos);
        animation1->Start();

        auto animation2 = std::make_shared<RSRenderAnimation>(animationId);
        animation2->Start();
        animation2->Finish();

        auto animation3 = std::make_shared<RSRenderAnimation>(animationId);
        animation3->Start();
        animation3->FinishOnCurrentPosition();
    }

    void RSRenderPropertyAnimationFuzzerTest1()
    {
        // get data
        AnimationId animationId = GetData<AnimationId>();
        PropertyId propertyId = GetData<PropertyId>();
        auto property = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
        bool isAdditive = GetData<bool>();
        auto frameInterval = GetData<float>();
        auto fraction = GetData<float>();
        auto time = GetData<float>();
        std::string out;

        auto renderPropertyAnimation = std::make_shared<RSRenderPropertyAnimation>(animationId, propertyId, property);
        renderPropertyAnimation->DumpAnimationInfo(out);
        renderPropertyAnimation->SetAdditive(isAdditive);
        renderPropertyAnimation->GetAdditive();
        renderPropertyAnimation->AttachRenderProperty(property);
        Parcel parcel;
        renderPropertyAnimation->Marshalling(parcel);
        renderPropertyAnimation->ParseParam(parcel);
        renderPropertyAnimation->SetPropertyValue(property);
        renderPropertyAnimation->GetPropertyValue();
        renderPropertyAnimation->GetOriginValue();
        renderPropertyAnimation->GetLastValue();
        renderPropertyAnimation->SetAnimationValue(property);
        renderPropertyAnimation->GetAnimationValue(property);
        renderPropertyAnimation->OnRemoveOnCompletion();
        renderPropertyAnimation->RecordLastAnimateValue();
        renderPropertyAnimation->UpdateAnimateVelocity(frameInterval);
        renderPropertyAnimation->ProcessAnimateVelocityUnderAngleRotation(frameInterval);
        renderPropertyAnimation->DumpFraction(fraction, time);
    }

    void RSRenderPropertyAnimationFuzzerTest2()
    {
        // get data
        AnimationId animationId = GetData<AnimationId>();
        PropertyId propertyId = GetData<PropertyId>();
        auto property = std::make_shared<RSRenderAnimatableProperty<float>>(GetData<float>());
        bool isAdditive = GetData<bool>();
        auto frameInterval = GetData<float>();
        auto fraction = GetData<float>();
        auto time = GetData<float>();
        NodeId nodeId = GetData<NodeId>();
        auto renderNode = std::make_shared<RSRenderNode>(nodeId);

        auto renderPropertyAnimation = std::make_shared<RSRenderPropertyAnimation>(animationId, propertyId, property);

        renderPropertyAnimation->Attach(renderNode.get());
        renderPropertyAnimation->Start();

        renderPropertyAnimation->SetAdditive(isAdditive);
        renderPropertyAnimation->GetAdditive();
        renderPropertyAnimation->AttachRenderProperty(property);

        renderPropertyAnimation->SetAnimationValue(nullptr);
        renderPropertyAnimation->SetAnimationValue(property);
        renderPropertyAnimation->GetAnimationValue(nullptr);
        renderPropertyAnimation->GetAnimationValue(property);
        renderPropertyAnimation->OnRemoveOnCompletion();
        renderPropertyAnimation->RecordLastAnimateValue();
        renderPropertyAnimation->ProcessAnimateVelocityUnderAngleRotation(frameInterval);
        renderPropertyAnimation->DumpFraction(fraction, time);
    }

    bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }
        // initialize
        data_ = data;
        size_ = size;
        pos = 0;

        RSAnimationFractionFuzzerTest();
        RSAnimationManagerFuzzerTest();
        RSAnimationTimingProtocolFuzzerTest();
        RSAnimationTraceUtilsFuzzerTest();
        RSCubicBezierInterpolatorFuzzerTest();
        RSInterpolatorFuzzerTest1();
        RSInterpolatorFuzzerTest2();
        RSSpringInterpolatorFuzzerTest();
        RSStepsInterpolatorFuzzerTest();
        RSRenderCurveAnimationFuzzerTest1();
        RSRenderCurveAnimationFuzzerTest2();
        RSRenderInteractiveImplictAnimatorAndMapFuzzerTest();
        RSRenderInterpolatingSpringAnimationFuzzerTest1();
        RSRenderInterpolatingSpringAnimationFuzzerTest2();
        RSRenderKeyframeAnimationFuzzerTest1();
        RSRenderKeyframeAnimationFuzzerTest2();
        RSRenderPathAnimationFuzzerTest1();
        RSRenderPathAnimationFuzzerTest2();
        RSCurveValueEstimatorFuzzerTest();
        RSKeyframeValueEstimatorFuzzerTest();
        RSSpringValueEstimatorFuzzerTest();
        RSSpringModelFuzzerTest();
        RSRenderTransitionFuzzerTest();
        RSRenderTransitionEffectFuzzerTest();
        RSRenderSpringAnimationFuzzerTest();
        RSRenderAnimation1FuzzerTest();
        RSRenderAnimation2FuzzerTest();
        RSRenderPropertyAnimationFuzzerTest1();
        RSRenderPropertyAnimationFuzzerTest2();
        return true;
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}

