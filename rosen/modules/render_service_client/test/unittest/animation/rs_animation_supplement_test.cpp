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

#include "rs_animation_base_test.h"
#include "rs_animation_test_utils.h"

#include "animation/rs_animation_callback.h"
#include "animation/rs_animation_group.h"
#include "animation/rs_animation_timing_protocol.h"
#include "animation/rs_curve_animation.h"
#include "animation/rs_keyframe_animation.h"
#include "animation/rs_path_animation.h"
#include "animation/rs_spring_animation.h"
#include "animation/rs_transition.h"
#include "animation/rs_implicit_animation_param.h"
#include "animation/rs_render_animation.h"
#include "animation/rs_keyframe_animation.h"
#include "modifier/rs_modifier_manager.h"
#include "transaction/rs_interfaces.h"
#include "render/rs_path.h"
#include "render/rs_light_up_effect_filter.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_display_node.h"
#include "ui/rs_proxy_node.h"
#include "ui/rs_ui_director.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using namespace ANIMATIONTEST;
class RSAnimationTest : public RSAnimationBaseTest {
};

class RSAnimationMock : public RSAnimation {
public:
    RSAnimationMock() : RSAnimation() {}
    ~RSAnimationMock() = default;
    void StartInner(const std::shared_ptr<RSNode>& target)
    {
        RSAnimation::StartInner(target);
    }

    void OnStart() override
    {
        RSAnimation::OnStart();
    }

    void OnReverse() override
    {
        RSAnimation::OnReverse();
    }

    void OnPause() override
    {
        RSAnimation::OnPause();
    }

    void OnResume() override
    {
        RSAnimation::OnResume();
    }

    void OnFinish() override
    {
        RSAnimation::OnFinish();
    }

    void OnSetFraction(float fraction) override
    {
        RSAnimation::OnSetFraction(fraction);
    }

    void OnUpdateStagingValue(bool isFirstStart) override
    {
        RSAnimation::OnUpdateStagingValue(isFirstStart);
    }

    PropertyId GetPropertyId() const override
    {
        return RSAnimation::GetPropertyId();
    }

    bool IsReversed() const
    {
        return RSAnimation::IsReversed();
    }

    void UpdateParamToRenderAnimation(const std::shared_ptr<RSRenderAnimation>& animation)
    {
        RSAnimation::UpdateParamToRenderAnimation(animation);
    }

    void StartCustomAnimation(const std::shared_ptr<RSRenderAnimation>& animation)
    {
        RSAnimation::StartCustomAnimation(animation);
    }
};

class RSRenderAnimationMock : public RSRenderAnimation {
public:
    RSRenderAnimationMock(AnimationId id) : RSRenderAnimation(id) {}
    ~RSRenderAnimationMock() = default;
};

class RSAnimationGroupMock : public RSAnimationGroup {
public:
    RSAnimationGroupMock() : RSAnimationGroup() {}
    ~RSAnimationGroupMock() = default;

    void OnStart() override
    {
        RSAnimationGroup::OnStart();
    }

    void OnPause() override
    {
        RSAnimationGroup::OnPause();
    }

    void OnResume() override
    {
        RSAnimationGroup::OnResume();
    }

    void OnFinish() override
    {
        RSAnimationGroup::OnFinish();
    }

    void OnReverse() override
    {
        RSAnimationGroup::OnReverse();
    }

    void OnSetFraction(float fraction) override
    {
        RSAnimationGroup::OnSetFraction(fraction);
    }

    PropertyId GetPropertyId() const override
    {
        return RSAnimationGroup::GetPropertyId();
    }

    void StartInner(const std::shared_ptr<RSNode>& target)
    {
        RSAnimation::StartInner(target);
    }
};

class RSKeyframeAnimationMock : public RSKeyframeAnimation {
public:
    explicit RSKeyframeAnimationMock(std::shared_ptr<RSPropertyBase> property) : RSKeyframeAnimation(property) {}
    ~RSKeyframeAnimationMock() = default;

    void OnStart() override
    {
        RSKeyframeAnimation::OnStart();
    }

    void InitInterpolationValue() override
    {
        RSKeyframeAnimation::InitInterpolationValue();
    }
};

/**
 * @tc.name: AnimationSupplementTest001
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTest, AnimationSupplementTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSupplementTest001 start";
    /**
     * @tc.steps: step1. init
     */
    auto animation = std::make_shared<RSAnimationMock>();
    std::function<void()> callback = nullptr;
    animation->SetFinishCallback(callback);
    callback = []() {
        std::cout << "RSAnimationTest AnimationSupplementTest001 callback" << std::endl;
    };
    animation->SetFinishCallback(callback);
    EXPECT_TRUE(animation != nullptr);
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSupplementTest001 end";
}

/**
 * @tc.name: AnimationSupplementTest002
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTest, AnimationSupplementTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSupplementTest002 start";
    /**
     * @tc.steps: step1. init
     */
    auto animation = std::make_shared<RSAnimationMock>();
    EXPECT_TRUE(!animation->IsStarted());
    EXPECT_TRUE(!animation->IsRunning());
    EXPECT_TRUE(!animation->IsPaused());
    EXPECT_TRUE(!animation->IsFinished());
    EXPECT_TRUE(!animation->IsReversed());
    std::shared_ptr<RSNode> node = nullptr;
    animation->Reverse();
    animation->Finish();
    animation->Pause();
    animation->Start(node);
    animation->StartInner(node);
    animation->Resume();
    animation->Pause();
    animation->OnPause();
    animation->Resume();
    animation->OnResume();
    animation->Finish();
    animation->OnFinish();
    animation->Reverse();
    animation->OnReverse();
    node = RSCanvasNode::Create();
    node->AddAnimation(animation);
    animation->Pause();
    animation->OnPause();
    animation->Resume();
    animation->OnResume();
    animation->Finish();
    animation->OnFinish();
    animation->Reverse();
    animation->OnReverse();
    EXPECT_TRUE(animation != nullptr);
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSupplementTest002 end";
}

/**
 * @tc.name: AnimationSupplementTest003
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTest, AnimationSupplementTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSupplementTest003 start";
    /**
     * @tc.steps: step1. init
     */
    auto animation = std::make_shared<RSAnimationMock>();
    auto renderAnimation = std::make_shared<RSRenderAnimationMock>(animation->GetId());
    animation->SetFraction(-1.0f);
    animation->SetFraction(0.5f);
    animation->Pause();
    animation->SetFraction(0.5f);
    animation->OnSetFraction(0.5f);
    animation->StartCustomAnimation(renderAnimation);
    std::shared_ptr<RSNode> node = RSCanvasNode::Create();
    node->AddAnimation(animation);
    animation->SetFraction(-1.0f);
    animation->SetFraction(0.5f);
    animation->Pause();
    animation->SetFraction(0.5f);
    animation->OnSetFraction(0.5f);
    animation->StartCustomAnimation(renderAnimation);
    EXPECT_TRUE(animation != nullptr);
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSupplementTest003 end";
}

/**
 * @tc.name: AnimationSupplementTest004
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTest, AnimationSupplementTest004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSupplementTest004 start";
    /**
     * @tc.steps: step1. init
     */
    auto implicitAnimator = std::make_shared<RSImplicitAnimator>();
    auto animations = implicitAnimator->CloseImplicitAnimation();
    EXPECT_TRUE(animations.empty());
    implicitAnimator->BeginImplicitKeyFrameAnimation(0.1f, RSAnimationTimingCurve::LINEAR);
    implicitAnimator->BeginImplicitKeyFrameAnimation(0.2f);
    implicitAnimator->EndImplicitKeyFrameAnimation();
    std::shared_ptr<RSMotionPathOption> motionPathOption;
    implicitAnimator->BeginImplicitPathAnimation(motionPathOption);
    implicitAnimator->EndImplicitPathAnimation();
    std::shared_ptr<const RSTransitionEffect> effect;
    implicitAnimator->BeginImplicitTransition(effect, true);
    implicitAnimator->EndImplicitTransition();
    implicitAnimator->BeginImplicitTransition(effect, false);
    implicitAnimator->EndImplicitTransition();
    std::shared_ptr<RSNode> node;
    std::shared_ptr<RSPropertyBase> property = std::make_shared<RSAnimatableProperty<float>>(0.1f);
    std::shared_ptr<RSPropertyBase> startValue = std::make_shared<RSAnimatableProperty<float>>(2.f);
    std::shared_ptr<RSPropertyBase> endValue = std::make_shared<RSAnimatableProperty<float>>(10.0f);
    implicitAnimator->CreateImplicitAnimation(node, property, startValue, endValue);
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(100);
    implicitAnimator->OpenImplicitAnimation(protocol, RSAnimationTimingCurve::LINEAR, nullptr);
    implicitAnimator->CreateImplicitAnimation(node, property, startValue, endValue);
    implicitAnimator->CreateImplicitTransition(*node);
    implicitAnimator->CloseImplicitAnimation();
    implicitAnimator->EndImplicitKeyFrameAnimation();
    implicitAnimator->EndImplicitTransition();
    implicitAnimator->EndImplicitPathAnimation();

    node = RSCanvasNode::Create();
    implicitAnimator->CreateImplicitAnimation(node, property, startValue, endValue);
    implicitAnimator->OpenImplicitAnimation(protocol, RSAnimationTimingCurve::LINEAR, nullptr);
    implicitAnimator->CreateImplicitAnimation(node, property, startValue, endValue);
    implicitAnimator->CreateImplicitTransition(*node);
    implicitAnimator->CloseImplicitAnimation();
    implicitAnimator->EndImplicitKeyFrameAnimation();
    implicitAnimator->EndImplicitTransition();
    implicitAnimator->EndImplicitPathAnimation();
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSupplementTest004 end";
}

class RSPathAnimationMock : public RSPathAnimation {
public:
    RSPathAnimationMock(std::shared_ptr<RSPropertyBase> property, const std::shared_ptr<RSPath>& animationPath)
        : RSPathAnimation(property, animationPath)
    {}
    RSPathAnimationMock(std::shared_ptr<RSPropertyBase> property, const std::string& path,
        const std::shared_ptr<RSPropertyBase>& startValue,
        const std::shared_ptr<RSPropertyBase>& endValue)
        : RSPathAnimation(property, path, startValue, endValue) {}
    ~RSPathAnimationMock() = default;

    void OnStart()
    {
        RSPathAnimation::OnStart();
    }

    void InitInterpolationValue()
    {
        RSPathAnimation::InitInterpolationValue();
    }

    void OnUpdateStagingValue(bool isFirstStart)
    {
        RSPathAnimation::OnUpdateStagingValue(isFirstStart);
    }

    void InitRotationId(const std::shared_ptr<RSNode>& node)
    {
        RSPathAnimation::InitRotationId(node);
    }

    PropertyId GetRotationPropertyId(const std::shared_ptr<RSNode>& node)
    {
        return RSPathAnimation::GetRotationPropertyId(node);
    }

    void SetRotation(const std::shared_ptr<RSNode>& node, const float rotation)
    {
        RSPathAnimation::SetRotation(node, rotation);
    }

    void OnCallFinishCallback()
    {
        RSPathAnimation::OnCallFinishCallback();
    }
};

/**
 * @tc.name: AnimationSupplementTest005
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTest, AnimationSupplementTest005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSupplementTest005 start";
    /**
     * @tc.steps: step1. init
     */
    Vector4f data(1.f, 1.f, 1.f, 1.f);
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(data);
    std::shared_ptr<RSPath> path;
    auto animation = std::make_shared<RSPathAnimationMock>(property, path);
    EXPECT_TRUE(animation != nullptr);
    animation->SetTimingCurve(RSAnimationTimingCurve::LINEAR);
    auto curve = animation->GetTimingCurve();
    animation->SetTimingCurve(curve);
    animation->SetRotationMode(RotationMode::ROTATE_AUTO);
    animation->SetBeginFraction(-0.1f);
    animation->SetEndFraction(-0.1f);
    animation->SetEndFraction(1.0f);
    animation->SetBeginFraction(0.1f);
    animation->SetPathNeedAddOrigin(true);
    auto begin = animation->GetBeginFraction();
    auto end = animation->GetEndFraction();
    animation->SetBeginFraction(begin);
    animation->SetEndFraction(end);
    auto needAddOrigin = animation->GetPathNeedAddOrigin();
    animation->SetPathNeedAddOrigin(needAddOrigin);
    animation->OnStart();
    std::shared_ptr<RSNode> node = RSCanvasNode::Create();
    animation->Start(node);
    animation->SetRotationMode(RotationMode::ROTATE_AUTO);
    animation->SetEndFraction(1.f);
    animation->SetBeginFraction(0.1f);
    animation->SetPathNeedAddOrigin(true);
    animation->InitInterpolationValue();
    animation->OnStart();

    Vector4f startData(0.f, 1.f, 2.f, 3.f);
    auto startValue = std::make_shared<RSAnimatableProperty<Vector4f>>(startData);
    Vector4f endData(5.f, 6.f, 2.f, 3.f);
    auto endValue = std::make_shared<RSAnimatableProperty<Vector4f>>(endData);
    auto animation2 = std::make_shared<RSPathAnimationMock>(property, "abc", startValue, endValue);
    animation2->InitInterpolationValue();
    animation2->OnUpdateStagingValue(true);
    animation2->SetDirection(false);
    animation2->Start(node);
    animation2->OnStart();
    animation2->OnUpdateStagingValue(false);
    animation2->SetAutoReverse(true);
    animation2->SetRepeatCount(2);
    animation2->OnUpdateStagingValue(false);
    animation2->SetRotationMode(RotationMode::ROTATE_NONE);
    animation2->OnUpdateStagingValue(false);
    animation2->SetRotationMode(RotationMode::ROTATE_AUTO);
    animation2->OnUpdateStagingValue(false);
    animation2->SetRotationMode(RotationMode::ROTATE_AUTO_REVERSE);
    animation2->OnUpdateStagingValue(false);
    animation2->SetDirection(true);
    animation2->OnUpdateStagingValue(false);

    auto id = animation2->GetRotationPropertyId(node);
    animation2->InitRotationId(node);
    animation2->SetRotation(node, 1.f);
    auto rotationProperty = std::make_shared<RSAnimatableProperty<float>>(10.f);
    auto rotationModifier = std::make_shared<RSRotationModifier>(rotationProperty);
    node->AddModifier(rotationModifier);
    auto alphaProperty = std::make_shared<RSAnimatableProperty<float>>(0.1f);
    auto alphaModifier = std::make_shared<RSRotationModifier>(rotationProperty);
    node->AddModifier(alphaModifier);
    id = animation2->GetRotationPropertyId(node);
    EXPECT_TRUE(id != rotationModifier->GetPropertyId());
    animation2->SetRotation(node, 1.f);
    animation2->InitRotationId(node);
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSupplementTest005 end";
}

/**
 * @tc.name: AnimationSupplementTest006
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTest, AnimationSupplementTest006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSupplementTest006 start";
    /**
     * @tc.steps: step1. init
     */
    auto property = std::make_shared<RSAnimatableProperty<float>>(10.f);
    auto modifier = std::make_shared<RSAlphaModifier>(property);
    auto modifierManager = std::make_shared<RSModifierManager>();
    modifierManager->AddModifier(modifier);
    auto animation = std::make_shared<RSAnimationMock>();
    auto renderAnimation = std::make_shared<RSRenderAnimationMock>(animation->GetId());
    EXPECT_TRUE(animation != nullptr);
    modifierManager->RemoveAnimation(animation->GetId());
    modifierManager->AddAnimation(renderAnimation);
    modifierManager->Animate(0);
    modifierManager->Draw();
    modifierManager->RemoveAnimation(animation->GetId());
    modifierManager->AddAnimation(renderAnimation);
    modifierManager->AddAnimation(renderAnimation);
    auto uiAnimation2 = std::make_shared<RSAnimationMock>();
    auto animation2 = std::make_shared<RSRenderAnimationMock>(uiAnimation2->GetId());
    modifierManager->RemoveAnimation(uiAnimation2->GetId());
    modifierManager->RemoveAnimation(animation->GetId());
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSupplementTest006 end";
}

/**
 * @tc.name: AnimationSupplementTest007
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTest, AnimationSupplementTest007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSupplementTest007 start";
    /**
     * @tc.steps: step1. init
     */
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(100);
    auto keyframeParam = std::make_shared<RSImplicitKeyframeAnimationParam>(
        protocol, RSAnimationTimingCurve::LINEAR, 0.1f, 0);
    EXPECT_TRUE(keyframeParam != nullptr);
    std::shared_ptr<RSAnimation> animation;
    Vector4f startData(0.f, 1.f, 2.f, 3.f);
    auto startValue = std::make_shared<RSAnimatableProperty<Vector4f>>(startData);
    Vector4f endData(5.f, 6.f, 2.f, 3.f);
    auto endValue = std::make_shared<RSAnimatableProperty<Vector4f>>(endData);
    keyframeParam->AddKeyframe(animation, startValue, endValue);
    animation = std::make_shared<RSKeyframeAnimation>(startValue);
    keyframeParam->AddKeyframe(animation, startValue, endValue);
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSupplementTest007 end";
}

/**
 * @tc.name: AnimationSupplementTest008
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTest, AnimationSupplementTest008, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSupplementTest008 start";
    /**
     * @tc.steps: step1. init
     */
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(100);
    std::shared_ptr<RSMotionPathOption> option;
    auto pathParam = std::make_shared<RSImplicitPathAnimationParam>(
        protocol, RSAnimationTimingCurve::LINEAR, option);
    EXPECT_TRUE(pathParam != nullptr);
    Vector4f startData(0.f, 1.f, 2.f, 3.f);
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(startData);
    auto startValue = std::make_shared<RSAnimatableProperty<Vector4f>>(startData);
    Vector4f endData(5.f, 6.f, 2.f, 3.f);
    auto endValue = std::make_shared<RSAnimatableProperty<Vector4f>>(endData);
    auto animation = pathParam->CreateAnimation(property, startValue, endValue);

    option = std::make_shared<RSMotionPathOption>("abc");
    auto pathParam2 = std::make_shared<RSImplicitPathAnimationParam>(
        protocol, RSAnimationTimingCurve::LINEAR, option);
    auto animation2 = pathParam->CreateAnimation(property, startValue, endValue);

    auto springParam = std::make_shared<RSImplicitSpringAnimationParam>(protocol, RSAnimationTimingCurve::LINEAR);
    auto animation3 = springParam->CreateAnimation(property, startValue, endValue);
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSupplementTest008 end";
}

class RSTransitionTest : public RSTransition {
public:
    RSTransitionTest(const std::shared_ptr<const RSTransitionEffect>& effect, bool isTransitionIn)
        : RSTransition(effect, isTransitionIn)
    {}
    virtual ~RSTransitionTest() = default;
};

class RSTransitionMock : public RSTransition {
public:
    RSTransitionMock(const std::shared_ptr<const RSTransitionEffect>& effect, bool isTransitionIn)
        : RSTransition(effect, isTransitionIn)
    {}
    virtual ~RSTransitionMock() = default;

    void OnStart()
    {
        RSTransition::OnStart();
    }
};

/**
 * @tc.name: AnimationSupplementTest009
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTest, AnimationSupplementTest009, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSupplementTest009 start";
    /**
     * @tc.steps: step1. init
     */
    std::shared_ptr<const RSTransitionEffect> effect;
    auto animation = std::make_shared<RSTransitionMock>(effect, true);
    EXPECT_TRUE(animation != nullptr);
    animation->OnStart();
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSupplementTest009 end";
}

/**
 * @tc.name: AnimationSupplementTest010
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTest, AnimationSupplementTest010, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSupplementTest010 start";
    /**
     * @tc.steps: step1. init
     */
    std::function<void()> callback;
    AnimationFinishCallback* finishCallback1 = new AnimationFinishCallback(callback);
    delete finishCallback1;
    callback = []() {
        std::cout << "RSAnimationTest AnimationSupplementTest010" << std::endl;
    };
    EXPECT_TRUE(callback != nullptr);
    AnimationFinishCallback* finishCallback2 = new AnimationFinishCallback(callback);
    delete finishCallback2;
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSupplementTest010 end";
}

/**
 * @tc.name: AnimationSupplementTest011
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTest, AnimationSupplementTest011, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSupplementTest011 start";
    /**
     * @tc.steps: step1. init
     */
    std::shared_ptr<RSAnimationMock> animation;
    std::shared_ptr<RSAnimationGroupMock> animationGroup = std::make_shared<RSAnimationGroupMock>();
    animationGroup->AddAnimation(animation);
    animationGroup->RemoveAnimation(animation);
    animation = std::make_shared<RSAnimationMock>();
    std::shared_ptr<RSNode> node = RSCanvasNode::Create();
    animationGroup->StartInner(node);
    animationGroup->AddAnimation(animation);
    animationGroup->RemoveAnimation(animation);
    animationGroup->Pause();
    animation->StartInner(node);
    animationGroup->AddAnimation(animation);
    animationGroup->RemoveAnimation(animation);
    animation->Pause();
    animationGroup->AddAnimation(animation);
    animationGroup->AddAnimation(animation);
    animationGroup->RemoveAnimation(animation);
    animationGroup->RemoveAnimation(animation);
    auto id = animationGroup->GetPropertyId();
    EXPECT_TRUE(id == 0);
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSupplementTest011 end";
}

/**
 * @tc.name: AnimationSupplementTest012
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTest, AnimationSupplementTest012, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSupplementTest012 start";
    /**
     * @tc.steps: step1. init
     */
    std::shared_ptr<RSAnimationGroupMock> animationGroup = std::make_shared<RSAnimationGroupMock>();
    animationGroup->OnStart();
    animationGroup->OnPause();
    animationGroup->OnResume();
    animationGroup->OnFinish();
    animationGroup->OnReverse();
    animationGroup->OnSetFraction(0.1f);
    std::shared_ptr<RSAnimationMock> animation = std::make_shared<RSAnimationMock>();
    animationGroup->AddAnimation(animation);
    animationGroup->OnStart();
    std::shared_ptr<RSNode> node = RSCanvasNode::Create();
    animationGroup->StartInner(node);
    animationGroup->OnStart();
    animationGroup->OnPause();
    animationGroup->OnResume();
    animationGroup->OnFinish();
    animationGroup->OnReverse();
    animationGroup->OnSetFraction(0.1f);
    EXPECT_TRUE(animationGroup != nullptr);
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSupplementTest012 end";
}

/**
 * @tc.name: AnimationSupplementTest013
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTest, AnimationSupplementTest013, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSupplementTest013 start";
    /**
     * @tc.steps: step1. init
     */
    std::shared_ptr<RSPropertyBase> property = std::make_shared<RSAnimatableProperty<float>>(0.1f);
    std::shared_ptr<RSKeyframeAnimationMock> animation = std::make_shared<RSKeyframeAnimationMock>(property);
    std::shared_ptr<RSPropertyBase> value1 = std::make_shared<RSAnimatableProperty<float>>(0.2f);
    std::vector<std::tuple<float, std::shared_ptr<RSPropertyBase>, RSAnimationTimingCurve>> keyframes;
    keyframes.push_back({ 0.5f, value1, RSAnimationTimingCurve::LINEAR });
    animation->AddKeyFrame(-0.1f, value1, RSAnimationTimingCurve::LINEAR);
    animation->AddKeyFrame(0.2f, value1, RSAnimationTimingCurve::LINEAR);
    animation->AddKeyFrames(keyframes);
    std::shared_ptr<RSNode> node = RSCanvasNode::Create();
    animation->Start(node);
    animation->AddKeyFrame(0.3f, value1, RSAnimationTimingCurve::LINEAR);
    animation->AddKeyFrames(keyframes);
    EXPECT_TRUE(animation != nullptr);
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSupplementTest013 end";
}

/**
 * @tc.name: AnimationSupplementTest014
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTest, AnimationSupplementTest014, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSupplementTest014 start";
    /**
     * @tc.steps: step1. init
     */
    std::shared_ptr<RSPropertyBase> property = std::make_shared<RSAnimatableProperty<float>>(0.1f);
    std::shared_ptr<RSKeyframeAnimationMock> animation = std::make_shared<RSKeyframeAnimationMock>(property);
    animation->InitInterpolationValue();
    animation->OnStart();
    std::shared_ptr<RSPropertyBase> value1 = std::make_shared<RSAnimatableProperty<float>>(0.2f);
    animation->AddKeyFrame(0.2f, value1, RSAnimationTimingCurve::LINEAR);
    animation->InitInterpolationValue();
    animation->AddKeyFrame(0.f, value1, RSAnimationTimingCurve::LINEAR);
    animation->InitInterpolationValue();
    animation->OnStart();
    animation->SetIsCustom(true);
    animation->OnStart();
    animation->SetIsCustom(false);
    std::shared_ptr<RSNode> node1 = RSCanvasNode::Create();
    struct RSSurfaceNodeConfig surfaceNodeConfig = {.SurfaceNodeName = "test"};
    std::shared_ptr<RSNode> node2 = RSSurfaceNode::Create(surfaceNodeConfig, false);
    std::shared_ptr<RSNode> node3 = RSSurfaceNode::Create(surfaceNodeConfig, true);
    animation->Start(node1);
    animation->OnStart();
    EXPECT_TRUE(animation != nullptr);
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSupplementTest014 end";
}

/**
 * @tc.name: AnimationSupplementTest015
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTest, AnimationSupplementTest015, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSupplementTest015 start";
    /**
     * @tc.steps: step1. init
     */
    std::shared_ptr<RSPropertyBase> property = std::make_shared<RSAnimatableProperty<float>>(0.1f);
    std::shared_ptr<RSKeyframeAnimationMock> animation1 = std::make_shared<RSKeyframeAnimationMock>(property);
    std::shared_ptr<RSKeyframeAnimationMock> animation2 = std::make_shared<RSKeyframeAnimationMock>(property);
    std::shared_ptr<RSKeyframeAnimationMock> animation3 = std::make_shared<RSKeyframeAnimationMock>(property);
    std::shared_ptr<RSNode> node1 = RSCanvasNode::Create();
    struct RSSurfaceNodeConfig surfaceNodeConfig = {.SurfaceNodeName = "test"};
    std::shared_ptr<RSNode> node2 = RSSurfaceNode::Create(surfaceNodeConfig, false);
    std::shared_ptr<RSNode> node3 = RSSurfaceNode::Create(surfaceNodeConfig, true);
    animation1->Start(node1);
    animation1->OnStart();
    animation1->SetIsCustom(true);
    animation1->OnStart();
    animation2->Start(node1);
    animation2->OnStart();
    animation2->SetIsCustom(true);
    animation2->OnStart();
    animation3->Start(node1);
    animation3->OnStart();
    animation3->SetIsCustom(true);
    animation3->OnStart();
    EXPECT_TRUE(animation1 != nullptr);
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSupplementTest015 end";
}

/**
 * @tc.name: AnimationSupplementTest016
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTest, AnimationSupplementTest016, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSupplementTest016 start";
    /**
     * @tc.steps: step1. init
     */
    Vector4f data(1.f, 1.f, 1.f, 1.f);
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(data);
    Vector4f startData(0.f, 1.f, 2.f, 3.f);
    auto startValue = std::make_shared<RSAnimatableProperty<Vector4f>>(startData);
    Vector4f endData(0.f, 1.f, 2.f, 3.f);
    auto endValue = std::make_shared<RSAnimatableProperty<Vector4f>>(endData);
    auto animation = std::make_shared<RSPathAnimationMock>(property, "abc", startValue, endValue);
    animation->InitInterpolationValue();
    animation->OnUpdateStagingValue(true);
    animation->SetDirection(false);
    std::shared_ptr<RSNode> node = RSCanvasNode::Create();
    animation->Start(node);
    animation->OnStart();
    animation->OnUpdateStagingValue(false);
    animation->SetAutoReverse(true);
    animation->SetRepeatCount(2);
    animation->OnUpdateStagingValue(false);
    animation->InitInterpolationValue();
    auto rotationProperty = std::make_shared<RSAnimatableProperty<float>>(10.f);
    auto rotationModifier = std::make_shared<RSRotationModifier>(rotationProperty);
    node->AddModifier(rotationModifier);
    animation->GetRotationPropertyId(node);
    animation->SetRotation(node, 1.f);
    animation->InitInterpolationValue();
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSupplementTest016 end";
}

/**
 * @tc.name: AnimationSupplementTest017
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTest, AnimationSupplementTest017, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSupplementTest017 start";
    /**
     * @tc.steps: step1. init
     */
    Vector4f data(1.f, 1.f, 1.f, 1.f);
    std::shared_ptr<RSAnimatableProperty<Vector4f>> property;
    std::shared_ptr<RSPath> path = std::make_shared<RSPath>();
    auto animation1 = std::make_shared<RSPathAnimationMock>(property, path);
    animation1->OnCallFinishCallback();
    property = std::make_shared<RSAnimatableProperty<Vector4f>>(data);
    Vector2f startData(0.f, 1.f);
    auto startValue = std::make_shared<RSAnimatableProperty<Vector2f>>(startData);
    Vector2f endData(5.f, 6.f);
    auto endValue = std::make_shared<RSAnimatableProperty<Vector2f>>(endData);
    auto animation = std::make_shared<RSPathAnimationMock>(property, "abc", startValue, endValue);
    EXPECT_TRUE(animation != nullptr);
    animation->SetTimingCurve(RSAnimationTimingCurve::LINEAR);
    auto curve = animation->GetTimingCurve();
    animation->SetTimingCurve(curve);
    animation->SetRotationMode(RotationMode::ROTATE_AUTO);
    animation->SetBeginFraction(-0.1f);
    animation->SetEndFraction(-0.1f);
    animation->SetEndFraction(1.0f);
    animation->SetBeginFraction(0.1f);
    animation->SetPathNeedAddOrigin(true);
    animation->OnStart();
    animation->InitInterpolationValue();
    animation->OnCallFinishCallback();
    std::shared_ptr<RSNode> node = RSCanvasNode::Create();
    animation->Start(node);
    animation->SetRotationMode(RotationMode::ROTATE_AUTO);
    animation->SetEndFraction(1.f);
    animation->SetBeginFraction(0.1f);
    animation->SetPathNeedAddOrigin(true);
    animation->InitInterpolationValue();
    animation->OnStart();
    animation->SetAutoReverse(true);
    animation->SetRepeatCount(2);
    animation->OnUpdateStagingValue(true);
    animation->OnUpdateStagingValue(false);
    animation->SetAutoReverse(false);
    animation->SetRepeatCount(1);
    animation->OnUpdateStagingValue(true);
    animation->OnUpdateStagingValue(false);
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSupplementTest017 end";
}

/**
 * @tc.name: AnimationSupplementTest018
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTest, AnimationSupplementTest018, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSupplementTest018 start";
    /**
     * @tc.steps: step1. init
     */
    Vector4f data(1.f, 1.f, 1.f, 1.f);
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(data);
    Vector2f startData(0.f, 1.f);
    auto startValue = std::make_shared<RSAnimatableProperty<Vector2f>>(startData);
    Vector2f endData(5.f, 6.f);
    auto endValue = std::make_shared<RSAnimatableProperty<Vector2f>>(endData);
    auto animation = std::make_shared<RSPathAnimationMock>(property, "abc", startValue, endValue);
    animation->InitInterpolationValue();

    std::shared_ptr<RSPathAnimationMock> animation1 =
        std::make_shared<RSPathAnimationMock>(property, "abc", startValue, endValue);
    std::shared_ptr<RSPathAnimationMock> animation2 =
        std::make_shared<RSPathAnimationMock>(property, "abc", startValue, endValue);
    struct RSSurfaceNodeConfig surfaceNodeConfig = {.SurfaceNodeName = "test"};
    std::shared_ptr<RSNode> node1 = RSSurfaceNode::Create(surfaceNodeConfig, false);
    std::shared_ptr<RSNode> node2 = RSSurfaceNode::Create(surfaceNodeConfig, true);
    animation1->Start(node1);
    animation1->OnStart();
    animation2->Start(node1);
    animation2->OnStart();
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSupplementTest018 end";
}

class RSSpringAnimationMock : public RSSpringAnimation {
public:
    RSSpringAnimationMock(std::shared_ptr<RSPropertyBase> property, const std::shared_ptr<RSPropertyBase>& byValue)
        : RSSpringAnimation(property, byValue) {}

    RSSpringAnimationMock(std::shared_ptr<RSPropertyBase> property, const std::shared_ptr<RSPropertyBase>& startValue,
        const std::shared_ptr<RSPropertyBase>& endValue)
        : RSSpringAnimation(property, startValue, endValue) {}
    ~RSSpringAnimationMock() = default;

    void OnStart() override
    {
        RSSpringAnimation::OnStart();
    }
};

/**
 * @tc.name: AnimationSupplementTest019
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTest, AnimationSupplementTest019, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSupplementTest019 start";
    /**
     * @tc.steps: step1. init
     */
    auto property = std::make_shared<RSAnimatableProperty<float>>(1.f);
    auto value1 = std::make_shared<RSAnimatableProperty<float>>(2.f);
    auto value2 = std::make_shared<RSAnimatableProperty<float>>(3.f);
    std::shared_ptr<RSSpringAnimationMock> animation1 = std::make_shared<RSSpringAnimationMock>(property, value1);
    std::shared_ptr<RSSpringAnimationMock> animation2 =
        std::make_shared<RSSpringAnimationMock>(property, value1, value2);
    animation1->SetTimingCurve(RSAnimationTimingCurve::LINEAR);
    auto curve = RSAnimationTimingCurve::CreateSpringCurve(1.f, 2.f, 3.f, 4.f);
    animation1->SetTimingCurve(curve);
    animation1->SetIsCustom(true);
    animation1->OnStart();
    animation1->SetIsCustom(false);
    animation1->OnStart();
    std::shared_ptr<RSNode> node = RSCanvasNode::Create();
    animation1->Start(node);
    animation1->SetIsCustom(true);
    animation1->OnStart();
    animation1->SetIsCustom(false);
    animation1->OnStart();

    std::shared_ptr<RSSpringAnimationMock> animation3 =
        std::make_shared<RSSpringAnimationMock>(property, value1, value2);
    struct RSSurfaceNodeConfig surfaceNodeConfig = {.SurfaceNodeName = "test"};
    std::shared_ptr<RSNode> node1 = RSSurfaceNode::Create(surfaceNodeConfig, false);
    std::shared_ptr<RSNode> node2 = RSSurfaceNode::Create(surfaceNodeConfig, true);
    animation2->Start(node1);
    animation2->OnStart();
    animation3->Start(node1);
    animation3->OnStart();
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSupplementTest019 end";
}

class MyData : public RSAnimatableArithmetic<MyData> {
public:
    MyData() : data(0.f) {}
    explicit MyData(const float num) : data(num) {}
    virtual ~MyData() = default;

    MyData Add(const MyData& value) const override
    {
        float res = data + value.data;
        return MyData(res);
    }
    MyData Minus(const MyData& value) const override
    {
        float res = data - value.data;
        return MyData(res);
    }
    MyData Multiply(const float scale) const override
    {
        float res = data * scale;
        return MyData(res);
    }
    bool IsEqual(const MyData& value) const override
    {
        return data == value.data;
    }

    float data;
};

/**
 * @tc.name: AnimationSupplementTest020
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTest, AnimationSupplementTest020, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSupplementTest020 start";
    /**
     * @tc.steps: step1. init
     */
    auto data1 = MyData();
    auto data2 = MyData();
    [[maybe_unused]] bool ret = data1 == data2;

    RSFilter filter1;
    filter1.IsNearEqual(nullptr, ANIMATION_DEFAULT_VALUE);
    filter1.IsNearZero(ANIMATION_DEFAULT_VALUE);

    RSImageBase imageBase;
    Parcel parcel;
    imageBase.Marshalling(parcel);

    RRectT<float> rect1;
    RRectT<float> rect2;
    auto tmp = rect1 * ANIMATION_END_VALUE;
    tmp *= ANIMATION_END_VALUE;
    [[maybe_unused]] bool ret1 = tmp != rect2;
    rect1.IsNearEqual(rect2, ANIMATION_DEFAULT_VALUE);

    RectT<float> rect3;
    RectT<float> rect4;
    rect3.Intersect(rect4);

    RSDisplayNodeConfig config = { 0, false, 0 };
    RSDisplayNode::SharedPtr displayNode = RSDisplayNode::Create(config);
    displayNode->Marshalling(parcel);

    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.EnableCacheForRotation();
    rsInterfaces.DisableCacheForRotation();

    auto mask = RSMask::Unmarshalling(parcel);
    delete mask;

    RSLightUpEffectFilter filter3(ANIMATION_DEFAULT_VALUE);
    filter3.IsNearEqual(nullptr, ANIMATION_DEFAULT_VALUE);
    filter3.IsNearZero(ANIMATION_DEFAULT_VALUE);

    RSModifierExtractor extractor;
    extractor.GetCameraDistance();
    extractor.GetShadowMask();
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSupplementTest020 end";
}

/**
 * @tc.name: AnimationSupplementTest021
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTest, AnimationSupplementTest021, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSupplementTest021 start";
    /**
     * @tc.steps: step1. init
     */
    RSPropertyBase property;
    [[maybe_unused]] auto tmp = property.GetThreshold();
    property.SetValueFromRender(nullptr);

    RSModifierManager modifierManager;
    modifierManager.HasUIAnimation();
    auto animatablProperty = std::make_shared<RSAnimatableProperty<float>>(1.f);
    auto value = std::make_shared<RSAnimatableProperty<float>>(1.f);
    auto springAnimation = std::make_shared<RSSpringAnimationMock>(animatablProperty, value);
    modifierManager.RegisterSpringAnimation(springAnimation->GetPropertyId(), springAnimation->GetId());
    modifierManager.UnregisterSpringAnimation(springAnimation->GetPropertyId(), springAnimation->GetId());

    std::string str;
    RSMotionPathOption option(str);
    option.SetRotationMode(RotationMode::ROTATE_AUTO);
    option.GetRotationMode();
    option.SetPathNeedAddOrigin(true);
    option.GetPathNeedAddOrigin();
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSupplementTest021 end";
}

/**
 * @tc.name: AnimationSupplementTest022
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTest, AnimationSupplementTest022, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSupplementTest022 start";
    /**
     * @tc.steps: step1. init
     */
    auto property = std::make_shared<RSAnimatableProperty<float>>(1.f);
    property->GetStagingValue();
    auto stagingValue = property->Get();
    property->Set(stagingValue);
    property->GetShowingValueAndCancelAnimation();
    property->RequestCancelAnimation();
    auto propertyUnit_ { RSPropertyUnit::UNKNOWN };
    property->SetPropertyUnit(propertyUnit_);
    auto base = std::make_shared<RSRenderPropertyBase>();
    property->SetValueFromRender(base);
    property->SetUpdateCallback(nullptr);
    RSAnimationTimingProtocol timingProtocol;
    RSAnimationTimingCurve timingCurve;
    timingProtocol.SetFinishCallbackType(FinishCallbackType::TIME_SENSITIVE);
    std::shared_ptr<RSPropertyBase> targetValue = std::make_shared<RSAnimatableProperty<float>>(0.1f);
    property->AnimateWithInitialVelocity(timingProtocol, timingCurve, targetValue);

    std::shared_ptr<RSNode> node = RSCanvasNode::Create();
    node->SetShadowMask(true);
    node->IsImplicitAnimationOpen();
    node->GetChildIdByIndex(1);
    std::string nodeName = "nodeName";
    node->SetNodeName(nodeName);
    PropertyCallback callback;
    callback = []() {
    };
    node->AddDurationKeyFrame(100, timingCurve, callback); // 100 Set duration is 100ms
    NodeId id1 = 0;
    NodeId id2 = 1;
    node->RegisterTransitionPair(id1, id2);
    node->UnregisterTransitionPair(id1, id2);
    node->AnimateWithCurrentCallback(timingProtocol, timingCurve, callback);
    std::optional<Vector2f> vec(Vector2f(1.f, 1.f));
    node->SetSandBox(vec);
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSupplementTest022 end";
}

/**
 * @tc.name: AnimationSupplementTest023
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTest, AnimationSupplementTest023, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSupplementTest023 start";
    /**
     * @tc.steps: step1. init
     */
    Vector4f vec1(1.f, 1.f, 1.f, 1.f);
    NodeId nodeId = 1;
    NodeId childNode = 2; // 2 is childNode
    auto node = RSProxyNode::Create(nodeId);
    auto child = RSProxyNode::Create(childNode);
    node->SetBounds(vec1);
    node->SetFrame(vec1);
    node->SetBounds(1.f, 1.f, 1.f, 1.f);
    node->SetBoundsWidth(1.f);
    node->SetBoundsHeight(1.f);
    node->SetFrame(1.f, 1.f, 1.f, 1.f);
    node->SetFramePositionX(1.f);
    node->SetFramePositionY(1.f);
    node->GetType();
    node->AddChild(child, 1);
    node->RemoveChild(child);
    node->ClearChildren();

    auto rsUiDirector = RSUIDirector::Create();
    rsUiDirector->FlushAnimationStartTime(1);
    rsUiDirector->HasUIAnimation();

    Vector4 vec = { 0.f, 0.f, 0.f, 0.f };
    vec.IsNearEqual(vec1, 1.f);
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSupplementTest023 end";
}

class MyNewData : public RSArithmetic<MyNewData> {
public:
    MyNewData() : data(0.f) {}
    explicit MyNewData(const float num) : data(num) {}
    virtual ~MyNewData() = default;
    bool IsEqual(const MyNewData& value) const override
    {
        return data == value.data;
    }

    float data;
};

HWTEST_F(RSAnimationTest, AnimationSupplementTest024, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSupplementTest024 start";
    /**
     * @tc.steps: step1. init
     */
    Vector2f data(1.f, 1.f);
    RRectT<float> rect1;
    RRectT<float> rect2;
    RectT<float> rect3;
    RectT<float> rect4;
    rect1.SetValues(rect3, &data);
    rect2.SetValues(rect4, &data);
    rect1.IsNearEqual(rect2);
    auto temp1 = rect1 - rect2;
    rect1 = temp1 + rect2;
    temp1 = rect1 * (1.f);
    temp1 = temp1 / (1.f);
    temp1 += rect1;
    temp1 -= rect1;
    temp1 *= (1.f);
    temp1 = rect1;
    [[maybe_unused]] bool ret = (temp1 != rect2);
    ret = (temp1 == rect2);

    auto data1 = MyData();
    auto data2 = MyData();
    auto temp2 = data1 + data2;
    temp2 += data1;
    temp2 -= data1;
    data1 = temp2 - data2;
    temp2 = data1 * (1.f);
    temp2 *= (1.f);

    auto data3 = MyNewData();
    auto data4 = MyNewData();
    ret = (data3 == data4);
    ret = (data3 != data4);

    GTEST_LOG_(INFO) << "RSAnimationTest AnimationSupplementTest024 end";
}
} // namespace Rosen
} // namespace OHOS