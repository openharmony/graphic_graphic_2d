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

#include "rsanimation_fuzzer.h"

#include <securec.h>

#include "rs_animation.h"
#include "rs_animation_callback.h"
#include "rs_animation_group.h"
#include "rs_animation_timing_curve.h"
#include "rs_curve_animation.h"
#include "rs_implicit_animation_param.h"
#include "rs_implicit_animator_map.h"
#include "rs_implicit_animator.h"
#include "rs_interactive_implict_animator.h"
#include "rs_interpolating_spring_animation.h"
#include "rs_keyframe_animation.h"
#include "rs_motion_path_option.h"
#include "rs_path.h"
#include "rs_path_animation.h"
#include "rs_property_animation.h"
#include "rs_spring_animation.h"
#include "rs_steps_interpolator.h"
#include "rs_transition.h"
#include "rs_transition_effect.h"
#include "modifier/rs_modifier_manager_map.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_ui_director.h"

namespace OHOS {
    using namespace Rosen;
    namespace {
        const uint8_t* g_data = nullptr;
        size_t g_size = 0;
        size_t g_pos;
        constexpr size_t STR_LEN = 10;
    }

    /*
    * describe: get data from outside untrusted data(g_data) which size is according to sizeof(T)
    * tips: only support basic type
    */
    template<class T>
    T GetData()
    {
        T object {};
        size_t objectSize = sizeof(object);
        if (g_data == nullptr || objectSize > g_size - g_pos) {
            return object;
        }
        errno_t ret = memcpy_s(&object, objectSize, g_data + g_pos, objectSize);
        if (ret != EOK) {
            return {};
        }
        g_pos += objectSize;
        return object;
    }

    /*
    * get a string from g_data
    */
    std::string GetStringFromData(int strlen)
    {
        char cstr[strlen];
        cstr[strlen - 1] = '\0';
        for (int i = 0; i < strlen - 1; i++) {
            char tmp = GetData<char>();
            if (tmp == '\0') {
                tmp = '1';
            }
            cstr[i] = tmp;
        }
        std::string str(cstr);
        return str;
    }

    void RsAnimationGroupFuzzTest()
    {
        float fraction = GetData<float>();
        auto firstTransition = std::make_shared<RSTransition>(RSTransitionEffect::EMPTY, true);
        auto secondTransition = std::make_shared<RSTransition>(RSTransitionEffect::SCALE, true);

        auto animationGroup = std::make_shared<RSAnimationGroup>();
        animationGroup->AddAnimation(firstTransition);
        animationGroup->AddAnimation(secondTransition);
        animationGroup->RemoveAnimation(secondTransition);
        animationGroup->SetFinishCallback([]() {});
        animationGroup->Start(nullptr);
        animationGroup->GetId();
        animationGroup->GetTarget();
        animationGroup->Pause();
        animationGroup->Resume();
        animationGroup->Finish();
        animationGroup->Reverse();
        animationGroup->SetFraction(fraction);
        animationGroup->IsStarted();
        animationGroup->IsRunning();
        animationGroup->IsPaused();
        animationGroup->IsFinished();
    }

    void RsAnimationTimingCurveFuzzTest()
    {
        float ctrlX1 = GetData<float>();
        float ctrlY1 = GetData<float>();
        float ctrlX2 = GetData<float>();
        float ctrlY2 = GetData<float>();
        float velocity = GetData<float>();
        float mass = GetData<float>();
        float stiffness = GetData<float>();
        float damping = GetData<float>();
        float response = GetData<float>();
        float dampingRatio = GetData<float>();
        int32_t steps = GetData<int32_t>();
        StepsCurvePosition position = StepsCurvePosition::END;

        auto customCurve = RSAnimationTimingCurve::CreateCustomCurve([](float input) { return input; });
        auto cubicCurve = RSAnimationTimingCurve::CreateCubicCurve(ctrlX1, ctrlY1, ctrlX2, ctrlY2);
        auto stepCurve = RSAnimationTimingCurve::CreateStepsCurve(steps, position);
        auto springCurve = RSAnimationTimingCurve::CreateSpringCurve(velocity, mass, stiffness, damping);
        auto spring = RSAnimationTimingCurve::CreateSpring(response, dampingRatio);
        auto emptyCurve = std::make_shared<RSAnimationTimingCurve>();
        auto copyCurve = std::make_shared<RSAnimationTimingCurve>(stepCurve);
    }

    void RsCurveAnimationFuzzTest()
    {
        float first = GetData<float>();
        float second = GetData<float>();
        float third = GetData<float>();
        float fraction = GetData<float>();
        bool isCustom = GetData<bool>();
        auto firstProperty = std::make_shared<RSAnimatableProperty<float>>(first);
        auto secondProperty = std::make_shared<RSAnimatableProperty<float>>(second);
        auto thirdProperty = std::make_shared<RSAnimatableProperty<float>>(third);

        auto firstCurve = std::make_shared<RSCurveAnimation>(firstProperty, secondProperty);
        auto secondCurve = std::make_shared<RSCurveAnimation>(firstProperty, secondProperty, thirdProperty);
        secondCurve->GetId();
        secondCurve->GetTarget();
        secondCurve->SetIsCustom(isCustom);
        secondCurve->SetTimingCurve(RSAnimationTimingCurve::DEFAULT);
        secondCurve->GetTimingCurve();
        secondCurve->SetFinishCallback([]() {});
        secondCurve->SetFraction(fraction);
        secondCurve->Start(nullptr);
        secondCurve->IsStarted();
        secondCurve->IsRunning();
        secondCurve->Pause();
        secondCurve->IsPaused();
        secondCurve->Resume();
        secondCurve->Reverse();
        secondCurve->Finish();
        secondCurve->IsFinished();
    }

    void RsKeyframeAnimationFuzzTest()
    {
        float first = GetData<float>();
        float second = GetData<float>();
        float third = GetData<float>();
        float fraction = GetData<float>();
        bool isCustom = GetData<bool>();
        auto firstProperty = std::make_shared<RSAnimatableProperty<float>>(first);
        auto secondProperty = std::make_shared<RSAnimatableProperty<float>>(second);
        auto thirdProperty = std::make_shared<RSAnimatableProperty<float>>(third);

        auto keyframe = std::make_shared<RSKeyframeAnimation>(firstProperty);
        keyframe->AddKeyFrame(fraction, secondProperty, RSAnimationTimingCurve::DEFAULT);
        keyframe->AddKeyFrames({{fraction, secondProperty, RSAnimationTimingCurve::DEFAULT}});
        keyframe->GetId();
        keyframe->GetTarget();
        keyframe->SetIsCustom(isCustom);
        keyframe->SetFinishCallback([]() {});
        keyframe->SetFraction(fraction);
        keyframe->Start(nullptr);
        keyframe->IsStarted();
        keyframe->IsRunning();
        keyframe->Pause();
        keyframe->IsPaused();
        keyframe->Resume();
        keyframe->Reverse();
        keyframe->Finish();
        keyframe->IsFinished();
    }

    void RsPathAnimationFuzzTest()
    {
        float first = GetData<float>();
        float second = GetData<float>();
        float third = GetData<float>();
        float fraction = GetData<float>();
        bool isCustom = GetData<bool>();
        float beginFraction = GetData<float>();
        float endFraction = GetData<float>();
        bool pathNeedOrigin = GetData<bool>();
        auto path = GetStringFromData(STR_LEN);
        auto rotationMode = RotationMode::ROTATE_NONE;
        auto firstProperty = std::make_shared<RSAnimatableProperty<float>>(first);
        auto secondProperty = std::make_shared<RSAnimatableProperty<float>>(second);
        auto thirdProperty = std::make_shared<RSAnimatableProperty<float>>(third);

        auto firstPathAnimation = std::make_shared<RSPathAnimation>(firstProperty, RSPath::CreateRSPath());
        auto secondPathAnimation = std::make_shared<RSPathAnimation>(firstProperty, path,
            secondProperty, thirdProperty);
        secondPathAnimation->SetRotationMode(rotationMode);
        secondPathAnimation->GetRotationMode();
        secondPathAnimation->SetBeginFraction(beginFraction);
        secondPathAnimation->GetBeginFraction();
        secondPathAnimation->SetEndFraction(endFraction);
        secondPathAnimation->GetEndFraction();
        secondPathAnimation->SetPathNeedAddOrigin(pathNeedOrigin);
        secondPathAnimation->GetPathNeedAddOrigin();
        secondPathAnimation->GetId();
        secondPathAnimation->GetTarget();
        secondPathAnimation->SetIsCustom(isCustom);
        secondPathAnimation->SetTimingCurve(RSAnimationTimingCurve::DEFAULT);
        secondPathAnimation->GetTimingCurve();
        secondPathAnimation->SetFinishCallback([]() {});
        secondPathAnimation->SetFraction(fraction);
        secondPathAnimation->Start(nullptr);
        secondPathAnimation->IsStarted();
        secondPathAnimation->IsRunning();
        secondPathAnimation->Pause();
        secondPathAnimation->IsPaused();
        secondPathAnimation->Resume();
        secondPathAnimation->Reverse();
        secondPathAnimation->Finish();
        secondPathAnimation->IsFinished();
    }

    void RsMotionPathOptionFuzzTest()
    {
        auto path = GetStringFromData(STR_LEN);
        float beginFraction = GetData<float>();
        float endFraction = GetData<float>();
        bool pathNeedOrigin = GetData<bool>();
        auto rotationMode = RotationMode::ROTATE_NONE;
        auto motionPathOption = std::make_shared<RSMotionPathOption>(path);

        motionPathOption->GetPath();
        motionPathOption->SetBeginFraction(beginFraction);
        motionPathOption->GetBeginFraction();
        motionPathOption->SetEndFraction(endFraction);
        motionPathOption->GetEndFraction();
        motionPathOption->SetRotationMode(rotationMode);
        motionPathOption->GetRotationMode();
        motionPathOption->SetPathNeedAddOrigin(pathNeedOrigin);
        motionPathOption->GetPathNeedAddOrigin();
    }

    void RsSpringAnimationFuzzTest()
    {
        float first = GetData<float>();
        float second = GetData<float>();
        float third = GetData<float>();
        float fraction = GetData<float>();
        bool isCustom = GetData<bool>();
        auto firstProperty = std::make_shared<RSAnimatableProperty<float>>(first);
        auto secondProperty = std::make_shared<RSAnimatableProperty<float>>(second);
        auto thirdProperty = std::make_shared<RSAnimatableProperty<float>>(third);

        auto firstSpringAnimation = std::make_shared<RSSpringAnimation>(firstProperty, secondProperty);
        auto secondSpringAnimation = std::make_shared<RSSpringAnimation>(firstProperty,
            secondProperty, thirdProperty);
        secondSpringAnimation->GetId();
        secondSpringAnimation->GetTarget();
        secondSpringAnimation->SetIsCustom(isCustom);
        secondSpringAnimation->SetTimingCurve(RSAnimationTimingCurve::DEFAULT);
        secondSpringAnimation->GetTimingCurve();
        secondSpringAnimation->SetFinishCallback([]() {});
        secondSpringAnimation->SetFraction(fraction);
        secondSpringAnimation->Start(nullptr);
        secondSpringAnimation->IsStarted();
        secondSpringAnimation->IsRunning();
        secondSpringAnimation->Pause();
        secondSpringAnimation->IsPaused();
        secondSpringAnimation->Resume();
        secondSpringAnimation->Reverse();
        secondSpringAnimation->Finish();
        secondSpringAnimation->IsFinished();
    }

    void RsTransitionFuzzTest()
    {
        float first = GetData<float>();
        float second = GetData<float>();
        float third = GetData<float>();
        float fraction = GetData<float>();
        auto firstProperty = std::make_shared<RSAnimatableProperty<float>>(first);
        auto secondProperty = std::make_shared<RSAnimatableProperty<float>>(second);
        auto thirdProperty = std::make_shared<RSAnimatableProperty<float>>(third);

        auto firstTransition = std::make_shared<RSTransition>(RSTransitionEffect::EMPTY, true);
        auto secondTransition = std::make_shared<RSTransition>(RSTransitionEffect::SCALE, true);
        secondTransition->SetTransitionEffect(RSTransitionEffect::SCALE);
        secondTransition->GetId();
        secondTransition->GetTarget();
        secondTransition->SetFinishCallback([]() {});
        secondTransition->SetFraction(fraction);
        secondTransition->Start(nullptr);
        secondTransition->IsStarted();
        secondTransition->IsRunning();
        secondTransition->Pause();
        secondTransition->IsPaused();
        secondTransition->Resume();
        secondTransition->Reverse();
        secondTransition->Finish();
        secondTransition->IsFinished();
    }

    void RsTransitionEffectFuzzTest()
    {
        float x1 = GetData<float>();
        float x2 = GetData<float>();
        float y1 = GetData<float>();
        float y2 = GetData<float>();
        float opacity = GetData<float>();
        Vector3f scale = Vector3f(x1, x2, y1);
        Vector3f translate = Vector3f(x2, y1, y2);
        Vector4f axisAngle = Vector4f(x1, x2, y1, y2);

        auto animation = RSTransitionEffect::Create();
        animation->Opacity(opacity);
        animation->Scale(scale);
        animation->Translate(translate);
        animation->Rotate(axisAngle);
    }

    void RsImplicitAnimatorFuzzTest()
    {
        float fraction = GetData<float>();
        auto isTransitionIn = GetData<bool>();
        float first = GetData<float>();
        float second = GetData<float>();
        float third = GetData<float>();
        auto path = GetStringFromData(STR_LEN);
        auto canvasNode = RSCanvasNode::Create();
        auto firstProperty = std::make_shared<RSAnimatableProperty<float>>(first);
        auto secondProperty = std::make_shared<RSAnimatableProperty<float>>(second);
        auto thirdProperty = std::make_shared<RSAnimatableProperty<float>>(third);
        auto motionPathOption = std::make_shared<RSMotionPathOption>(path);
        const RSAnimationTimingProtocol timingProtocol = {};

        auto implicitAnimator = std::make_shared<RSImplicitAnimator>();
        implicitAnimator->OpenImplicitAnimation({}, RSAnimationTimingCurve::DEFAULT, {});
        implicitAnimator->CloseImplicitAnimation();
        implicitAnimator->BeginImplicitKeyFrameAnimation(fraction, RSAnimationTimingCurve::DEFAULT);
        implicitAnimator->BeginImplicitKeyFrameAnimation(fraction);
        implicitAnimator->EndImplicitKeyFrameAnimation();
        implicitAnimator->BeginImplicitTransition(RSTransitionEffect::SCALE, isTransitionIn);
        implicitAnimator->EndImplicitTransition();
        implicitAnimator->BeginImplicitPathAnimation(motionPathOption);
        implicitAnimator->EndImplicitPathAnimation();
        implicitAnimator->NeedImplicitAnimation();
        implicitAnimator->CreateImplicitAnimation(canvasNode, firstProperty, secondProperty, thirdProperty);
    }

    void RsImplicitAnimatorParamFuzzTest()
    {
        float fraction = GetData<float>();
        float first = GetData<float>();
        float second = GetData<float>();
        float third = GetData<float>();
        int32_t animatorId = GetData<int32_t>();
        auto path = GetStringFromData(STR_LEN);
        auto isTransitionIn = GetData<bool>();
        auto firstProperty = std::make_shared<RSAnimatableProperty<float>>(first);
        auto secondProperty = std::make_shared<RSAnimatableProperty<float>>(second);
        auto thirdProperty = std::make_shared<RSAnimatableProperty<float>>(third);
        auto motionPathOption = std::make_shared<RSMotionPathOption>(path);
        const RSAnimationTimingProtocol timingProtocol = {};

        RSImplicitAnimatorMap::Instance().GetAnimator(animatorId);
        auto testKeyframeParam = std::make_shared<RSImplicitKeyframeAnimationParam>(timingProtocol,
            RSAnimationTimingCurve::DEFAULT, fraction, 0);
        testKeyframeParam->GetType();
        testKeyframeParam->CreateAnimation(firstProperty, false, 0, secondProperty, thirdProperty);
        auto testtPathParam = std::make_shared<RSImplicitPathAnimationParam>(timingProtocol,
            RSAnimationTimingCurve::DEFAULT, motionPathOption);
        testtPathParam->GetType();
        testtPathParam->CreateAnimation(firstProperty, secondProperty, thirdProperty);
        auto testSpringParam = std::make_shared<RSImplicitSpringAnimationParam>(timingProtocol,
            RSAnimationTimingCurve::DEFAULT);
        testSpringParam->GetType();
        testSpringParam->CreateAnimation(firstProperty, secondProperty, thirdProperty);
        auto testTransitionParam = std::make_shared<RSImplicitTransitionParam>(timingProtocol,
            RSAnimationTimingCurve::DEFAULT, RSTransitionEffect::SCALE, isTransitionIn);
        testTransitionParam->GetType();
        testTransitionParam->CreateAnimation();
    }

    void AnimationCallbackFuzzTest()
    {
        auto finishCallbackType = GetData<FinishCallbackType>();

        auto finishCallback = std::make_shared<AnimationFinishCallback>(nullptr, finishCallbackType);
        finishCallback->Execute();
        auto repeatCallback = std::make_shared<AnimationRepeatCallback>(nullptr);
        repeatCallback->Execute();
    }

    void RSImplicitAnimatorMapFuzzTest()
    {
        int32_t animatorId = GetData<int32_t>();

        auto implicitAnimatorMap = RSImplicitAnimatorMap::Instance().GetAnimator(animatorId);
        if (implicitAnimatorMap == nullptr) {
            return;
        }
    }

    void RSInteractiveImplictAnimatorFuzzTest()
    {
        auto position = GetData<RSInteractiveAnimationPosition>();
        auto fraction = GetData<float>();
        const RSAnimationTimingProtocol timingProtocol = {};

        auto interactiveAnimator = RSInteractiveImplictAnimator::Create(
            timingProtocol, RSAnimationTimingCurve::DEFAULT);
        interactiveAnimator->AddImplictAnimation([]() {});
        interactiveAnimator->AddAnimation([]() {});
        interactiveAnimator->StartAnimation();
        interactiveAnimator->PauseAnimation();
        interactiveAnimator->ContinueAnimation();
        interactiveAnimator->FinishAnimation(position);
        interactiveAnimator->ReverseAnimation();
        interactiveAnimator->SetFraction(fraction);
        interactiveAnimator->GetFraction();
        interactiveAnimator->GetStatus();
        interactiveAnimator->SetFinishCallBack([]() {});
    }

    void RSInterpolatingSpringAnimationFuzzTest()
    {
        float first = GetData<float>();
        float second = GetData<float>();
        float third = GetData<float>();
        float zeroThreshold = GetData<float>();
        auto firstProperty = std::make_shared<RSAnimatableProperty<float>>(first);
        auto secondProperty = std::make_shared<RSAnimatableProperty<float>>(second);
        auto thirdProperty = std::make_shared<RSAnimatableProperty<float>>(third);

        auto firstAnimation = std::make_shared<RSInterpolatingSpringAnimation>(firstProperty, secondProperty);
        firstAnimation->SetTimingCurve(RSAnimationTimingCurve::DEFAULT);
        firstAnimation->GetTimingCurve();
        auto secondAnimation = std::make_shared<RSInterpolatingSpringAnimation>(
            firstProperty, secondProperty, thirdProperty);
        secondAnimation->SetZeroThreshold(zeroThreshold);
        secondAnimation->IsSupportInteractiveAnimator();
    }


    void RSAnimation1FuzzTest()
    {
        // get data
        auto isFirstStart = GetData<bool>();
        auto animationId = GetData<AnimationId>();

        auto animation = std::make_shared<RSAnimation>();
        std::function<void()> func = nullptr;
        animation->SetFinishCallback(func);
        auto interactiveCallback = std::make_shared<InteractiveAnimatorFinishCallback>([]() {});
        animation->SetInteractiveFinishCallback(interactiveCallback);
        auto repeatCallback = std::make_shared<AnimationRepeatCallback>([]() {});
        animation->SetRepeatCallback(repeatCallback);
        animation->CallRepeatCallback();
        animation->SetRepeatCallback(nullptr);
        animation->CallRepeatCallback();
        animation->GetId();
        animation->IsStarted();
        animation->IsRunning();
        animation->IsPaused();
        animation->IsFinished();
        animation->IsReversed();
        animation->IsUiAnimation();
        animation->GetPropertyId();
        animation->UpdateStagingValue(isFirstStart);
        animation->UpdateParamToRenderAnimation(nullptr);
        animation->DumpAnimation();

        // have target
        auto canvasNode = RSCanvasNode::Create();
        animation->Start(canvasNode);
        animation->StartInner(canvasNode);
        animation->SetFinishCallback([]() {});
        animation->CallFinishCallback();
        animation->CallLogicallyFinishCallback();

        //have uianimation
        auto renderAnimation = std::make_shared<RSRenderAnimation>(animationId);
        animation->UpdateParamToRenderAnimation(renderAnimation);
        animation->StartCustomAnimation(renderAnimation);
        animation->UpdateParamToRenderAnimation(renderAnimation);
        auto rsUiDirector = RSUIDirector::Create();
        rsUiDirector->SendMessages();
    }

    void RSAnimation2FuzzTest()
    {
        // get data
        auto fraction = GetData<float>();
        auto pos = GetData<RSInteractiveAnimationPosition>();

        // do status error
        auto animation1 = std::make_shared<RSAnimation>();
        animation1->Start(nullptr);
        animation1->StartInner(nullptr);
        animation1->Pause();
        animation1->OnPause();
        animation1->Resume();
        animation1->OnResume();
        animation1->Finish();
        animation1->OnFinish();
        animation1->Reverse();
        animation1->OnReverse();
        animation1->SetFraction(fraction);
        animation1->OnSetFraction(fraction);
        animation1->InteractivePause();
        animation1->InteractiveContinue();
        animation1->InteractiveFinish(pos);
        animation1->InteractiveReverse();
        animation1->InteractiveSetFraction(fraction);

        // have target
        auto animation2 = std::make_shared<RSAnimation>();
        auto canvasNode = RSCanvasNode::Create();
        animation2->Start(canvasNode);
        animation2->StartInner(canvasNode);
        animation2->Start(canvasNode);
        animation2->Pause();
        animation2->OnPause();
        animation2->SetFraction(fraction);
        animation2->OnSetFraction(fraction);
        animation2->Resume();
        animation2->OnResume();
        animation2->Reverse();
        animation2->OnReverse();
        animation2->InteractivePause();
        animation2->InteractiveSetFraction(fraction);
        animation2->InteractiveReverse();
        animation2->InteractiveContinue();
        animation2->Finish();
        animation2->OnFinish();

        auto animation3 = std::make_shared<RSAnimation>();
        animation3->Start(canvasNode);
        animation3->StartInner(canvasNode);
        animation3->InteractiveFinish(pos);
        auto rsUiDirector = RSUIDirector::Create();
        rsUiDirector->SendMessages();
    }

    void RSAnimation3FuzzTest()
    {
        // get data
        auto animationId = GetData<AnimationId>();
        auto fraction = GetData<float>();
        auto pos = GetData<RSInteractiveAnimationPosition>();

        // have uianiamtion
        auto animation1 = std::make_shared<RSAnimation>();
        auto canvasNode = RSCanvasNode::Create();
        auto renderAnimation = std::make_shared<RSRenderAnimation>(animationId);
        animation1->Start(canvasNode);
        animation1->StartInner(canvasNode);
        animation1->StartCustomAnimation(renderAnimation);
        animation1->Pause();
        animation1->OnPause();
        animation1->SetFraction(fraction);
        animation1->OnSetFraction(fraction);
        animation1->Resume();
        animation1->OnResume();
        animation1->Reverse();
        animation1->OnReverse();
        animation1->InteractivePause();
        animation1->InteractiveSetFraction(fraction);
        animation1->InteractiveReverse();
        animation1->InteractiveContinue();
        animation1->Finish();
        animation1->OnFinish();

        auto animation2 = std::make_shared<RSAnimation>();
        animation2->Start(canvasNode);
        animation2->StartInner(canvasNode);
        animation2->StartCustomAnimation(renderAnimation);
        animation2->InteractiveFinish(pos);
        auto rsUiDirector = RSUIDirector::Create();
        rsUiDirector->SendMessages();
    }

    bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }
        // initialize
        g_data = data;
        g_size = size;
        g_pos = 0;
        // get data
        int32_t managerId = GetData<int32_t>();
        RSModifierManagerMap::Instance()->GetModifierManager(managerId);
        RsAnimationGroupFuzzTest();
        RsAnimationTimingCurveFuzzTest();
        RsCurveAnimationFuzzTest();
        RsKeyframeAnimationFuzzTest();
        RsPathAnimationFuzzTest();
        RsMotionPathOptionFuzzTest();
        RsSpringAnimationFuzzTest();
        RsTransitionFuzzTest();
        RsTransitionEffectFuzzTest();
        RsImplicitAnimatorFuzzTest();
        RsImplicitAnimatorParamFuzzTest();
        AnimationCallbackFuzzTest();
        RSImplicitAnimatorMapFuzzTest();
        RSInteractiveImplictAnimatorFuzzTest();
        RSInterpolatingSpringAnimationFuzzTest();
        RSAnimation1FuzzTest();
        RSAnimation2FuzzTest();
        RSAnimation3FuzzTest();
        return true;
    }
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}

