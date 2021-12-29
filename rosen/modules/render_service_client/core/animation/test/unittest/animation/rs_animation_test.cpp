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

#include "rs_animation_test.h"
#include "rs_animation_callback.h"
#include "rs_animation_group.h"
#include "rs_animation_timing_curve.h"
#include "rs_curve_animation.h"
#include "rs_implicit_animation_param.h"
#include "rs_implicit_animator.h"
#include "rs_keyframe_animation.h"
#include "rs_transition.h"
#include "rs_node.h"
#include "rs_property_node.h"
#include "rs_path_animation.h"
#include "animation/rs_animation_timing_protocol.h"
#include "animation/rs_animation_common.h"
#include "render/rs_path.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
void RSAnimationTest::SetUpTestCase() {}
void RSAnimationTest::TearDownTestCase() {}
void RSAnimationTest::SetUp() {}
void RSAnimationTest::TearDown() {}

/**
 * @tc.name: AnimationParameterTest001
 * @tc.desc: Verify the duration parameter of animation
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSAnimationTest, AnimationParameterTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationParameterTest001 start";
    /**
     * @tc.steps: step1. init animation and test data
     */
    int testDurationData[] = {500, 1000, 1500, 2000, 2500};
    std::unique_ptr<RSCurveAnimation<float>> animation =
        std::make_unique<RSCurveAnimation<float>>(RSAnimatableProperty::BOUNDS_HEIGHT, 100, 200);
    animation->SetTimingCurve(RSAnimationTimingCurve::DEFAULT);
    /**
     * @tc.steps: step2. set animation duration and test
     */
    for (const auto& data : testDurationData) {
        animation->SetDuration(data);
        EXPECT_EQ(data, animation->GetDuration());
    }
}

/**
 * @tc.name: AnimationParameterTest002
 * @tc.desc: Verify the startdelay parameter of animation
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSAnimationTest, AnimationParameterTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationParameterTest002 start";
    /**
     * @tc.steps: step1. init animation and test data
     */
    int testStartDelayData[] = {500, 1000, 1500, 2000, 2500};
    std::unique_ptr<RSCurveAnimation<float>> animation =
        std::make_unique<RSCurveAnimation<float>>(RSAnimatableProperty::BOUNDS_HEIGHT, 200);
    animation->SetTimingCurve(RSAnimationTimingCurve::LINEAR);
    /**
     * @tc.steps: step2. set animation startdelay and test
     */
    for (const auto& data : testStartDelayData) {
        animation->SetStartDelay(data);
        EXPECT_EQ(data, animation->GetStartDelay());
    }
}

/**
 * @tc.name: AnimationParameterTest003
 * @tc.desc: Verify the speed parameter of animation
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSAnimationTest, AnimationParameterTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationParameterTest003 start";
    /**
     * @tc.steps: step1. init animation and test data
     */
    int testSpeedData[] = {1, 2, 3, 4, 5};
    std::unique_ptr<RSCurveAnimation<float>> animation =
        std::make_unique<RSCurveAnimation<float>>(RSAnimatableProperty::BOUNDS_HEIGHT, 100, 200);
    animation->SetTimingCurve(RSAnimationTimingCurve::EASE);
    /**
     * @tc.steps: step2. set animation speed and test
     */
    for (const auto& data : testSpeedData) {
        animation->SetSpeed(data);
        EXPECT_EQ(data, animation->GetSpeed());
    }
}

/**
 * @tc.name: AnimationParameterTest004
 * @tc.desc: Verify the repeatCount parameter of animation
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSAnimationTest, AnimationParameterTest004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationParameterTest004 start";
    /**
     * @tc.steps: step1. init animation and test data
     */
    int testRepeatCountData[] = {1, 2, 3, 4, 5};
    std::unique_ptr<RSCurveAnimation<float>> animation =
        std::make_unique<RSCurveAnimation<float>>(RSAnimatableProperty::BOUNDS_HEIGHT, 100, 200);
    animation->SetTimingCurve(RSAnimationTimingCurve::EASE_IN);
    /**
     * @tc.steps: step2. set animation repeatCount and test
     */
    for (const auto& data : testRepeatCountData) {
        animation->SetRepeatCount(data);
        EXPECT_EQ(data, animation->GetRepeatCount());
    }
}

/**
 * @tc.name: AnimationParameterTest005
 * @tc.desc: Verify the autoReverse parameter of animation
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSAnimationTest, AnimationParameterTest005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationParameterTest005 start";
    /**
     * @tc.steps: step1. init animation and test data
     */
    std::unique_ptr<RSCurveAnimation<float>> animation =
        std::make_unique<RSCurveAnimation<float>>(RSAnimatableProperty::BOUNDS_HEIGHT, 100, 200);
    animation->SetTimingCurve(RSAnimationTimingCurve::EASE_OUT);
    /**
     * @tc.steps: step2. set animation autoReverse and test
     */
    animation->SetAutoReverse(true);
    EXPECT_TRUE(animation->GetAutoReverse());
    animation->SetAutoReverse(false);
    EXPECT_FALSE(animation->GetAutoReverse());
}

/**
 * @tc.name: AnimationParameterTest006
 * @tc.desc: Verify the fillMode parameter of animation
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSAnimationTest, AnimationParameterTest006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationParameterTest006 start";
    /**
     * @tc.steps: step1. init animation and test data
     */
    FillMode testFillModeData[] = {FillMode::FORWARDS, FillMode::BACKWARDS, FillMode::BOTH, FillMode::NONE};
    std::unique_ptr<RSCurveAnimation<float>> animation =
        std::make_unique<RSCurveAnimation<float>>(RSAnimatableProperty::BOUNDS_HEIGHT, 100, 200);
    animation->SetTimingCurve(RSAnimationTimingCurve::EASE_IN_OUT);
    /**
     * @tc.steps: step2. set animation fillMode and test
     */
    for (const auto& data : testFillModeData) {
        animation->SetFillMode(data);
        EXPECT_EQ(data, animation->GetFillMode());
    }
}

/**
 * @tc.name: AnimationParameterTest007
 * @tc.desc: Verify the direction parameter of animation
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSAnimationTest, AnimationParameterTest007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationParameterTest007 start";
    /**
     * @tc.steps: step1. init animation and test data
     */
    std::unique_ptr<RSCurveAnimation<float>> animation =
        std::make_unique<RSCurveAnimation<float>>(RSAnimatableProperty::BOUNDS_HEIGHT, 100, 200);
    RSAnimationTimingCurve curve = RSAnimationTimingCurve::CreateCustomCurve([](float input) { return input; });
    animation->SetTimingCurve(curve);
    /**
     * @tc.steps: step2. set animation autoReverse and test
     */
    animation->SetDirection(true);
    EXPECT_TRUE(animation->GetDirection());
    animation->SetDirection(false);
    EXPECT_FALSE(animation->GetDirection());
}

/**
 * @tc.name: AnimationStatusTest001
 * @tc.desc: Verify the status of animation
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSAnimationTest, AnimationStatusTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationStatusTest001 start";
    /**
     * @tc.steps: step1. init animation
     */
    RSPropertyNode::SharedPtr node = RSNode::Create();
    node->SetBoundsWidth(200);
    std::unique_ptr<RSCurveAnimation<float>> animation =
        std::make_unique<RSCurveAnimation<float>>(RSAnimatableProperty::BOUNDS_WIDTH, 200, 500);
    animation->SetDuration(1000);
    RSAnimationTimingCurve curve = RSAnimationTimingCurve::CreateCubicCurve(0.42f, 0.0f, 0.58f, 1.0f);
    animation->SetTimingCurve(curve);
    animation->SetFinishCallback([&]() { node->SetBoundsWidth(200); });
    /**
     * @tc.steps: step2. start animation test status
     */
    EXPECT_FALSE(animation->IsStarted());
    animation->Start(node);
    EXPECT_TRUE(animation->IsRunning());
    animation->Pause();
    EXPECT_TRUE(animation->IsPaused());
    animation->Resume();
    EXPECT_TRUE(animation->IsRunning());
    animation->Finish();
    EXPECT_TRUE(animation->IsFinished());
    AnimationId id = animation->GetId();
    EXPECT_TRUE(id != 0);
    animation->SetFraction(0.2f);
    RSAnimationTimingCurve timingCurve = animation->GetTimingCurve();
    animation->SetTimingCurve(timingCurve);
    std::weak_ptr<RSPropertyNode> target = animation->GetTarget();
    auto targetNode = target.lock();
    EXPECT_TRUE(targetNode != nullptr);
}

/**
 * @tc.name: AnimationGroupTest001
 * @tc.desc: Verify the status of animationGroup
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSAnimationTest, AnimationGroupTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationGroupTest001 start";
    /**
     * @tc.steps: step1. init animation group
     */
    RSPropertyNode::SharedPtr node = RSNode::Create();
    node->SetBoundsWidth(200);
    std::shared_ptr<RSCurveAnimation<float>> animation1 =
        std::make_shared<RSCurveAnimation<float>>(RSAnimatableProperty::BOUNDS_WIDTH, 200, 500);
    animation1->SetDuration(1000);
    animation1->SetTimingCurve(RSAnimationTimingCurve::EASE_IN_OUT);
    std::shared_ptr<RSCurveAnimation<float>> animation2 =
        std::make_shared<RSCurveAnimation<float>>(RSAnimatableProperty::BOUNDS_WIDTH, 200, 500);
    animation2->SetDuration(1000);
    animation2->SetTimingCurve(RSAnimationTimingCurve::EASE_IN_OUT);
    std::unique_ptr<RSAnimationGroup> animationGroup = std::make_unique<RSAnimationGroup>();
    animationGroup->AddAnimation(animation1);
    animationGroup->AddAnimation(animation2);
    animationGroup->RemoveAnimation(animation1);
    /**
     * @tc.steps: step2. start animation group test
     */
    animationGroup->Start(node);
    EXPECT_FALSE(animation1->IsRunning());
    EXPECT_TRUE(animation2->IsRunning());
    animationGroup->Pause();
    EXPECT_FALSE(animation1->IsPaused());
    EXPECT_TRUE(animation2->IsPaused());
}

/**
 * @tc.name: MotionPathOptionTest001
 * @tc.desc: Verify the  parameter of MotionPathOption
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSAnimationTest, MotionPathOptionTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest MotionPathOptionTest001 start";
    /**
     * @tc.steps: step1. init MotionPathOption
     */
    string path = "L150 0 L50 50";
    std::unique_ptr<RSMotionPathOption> option = std::make_unique<RSMotionPathOption>(path);
    /**
     * @tc.steps: step2. start MotionPathOption test
     */
    EXPECT_EQ(path, option->GetPath());
    float testBeginFractionData[] = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    for (const auto& data : testBeginFractionData) {
        option->SetBeginFraction(data);
        EXPECT_EQ(data, option->GetBeginFraction());
    }
    float testEndFractionData[] = {0.6f, 0.7f, 0.8f, 0.9f, 1.0f};
    for (const auto& data : testEndFractionData) {
        option->SetEndFraction(data);
        EXPECT_EQ(data, option->GetEndFraction());
    }
    RotationMode testRotationModeData[] = {RotationMode::ROTATE_NONE, RotationMode::ROTATE_AUTO,
        RotationMode::ROTATE_AUTO_REVERSE};
    for (const auto& data : testRotationModeData) {
        option->SetRotationMode(data);
        EXPECT_EQ(data, option->GetRotationMode());
    }
}

/**
 * @tc.name: PathAnimationTest001
 * @tc.desc: Verify the  parameter and initial of PathAnimation
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSAnimationTest, PathAnimationTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest PathAnimationTest001 start";
    /**
     * @tc.steps: step1. init PathAnimation
     */
    string path = "L150 0 L50 50";
    std::shared_ptr<RSPath> rsPath= RSPath::CreateRSPath(path);
    std::unique_ptr<RSPathAnimation> animation =
        std::make_unique<RSPathAnimation>(RSAnimatableProperty::BOUNDS_POSITION, rsPath);
    animation->SetTimingCurve(RSAnimationTimingCurve::LINEAR);
    RSAnimationTimingCurve timingCurve = animation->GetTimingCurve();
    /**
     * @tc.steps: step2. start PathAnimation test
     */
    RotationMode testRotationModeData[] = {RotationMode::ROTATE_NONE, RotationMode::ROTATE_AUTO,
        RotationMode::ROTATE_AUTO_REVERSE};
    for (const auto& data : testRotationModeData) {
        animation->SetRotationMode(data);
        EXPECT_EQ(data, animation->GetRotationMode());
    }
    float testBeginFractionData[] = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    for (const auto& data : testBeginFractionData) {
        animation->SetBeginFraction(data);
        EXPECT_EQ(data, animation->GetBeginFraction());
    }
    float testEndFractionData[] = {0.6f, 0.7f, 0.8f, 0.9f, 1.0f};
    for (const auto& data : testEndFractionData) {
        animation->SetEndFraction(data);
        EXPECT_EQ(data, animation->GetEndFraction());
    }
    EXPECT_TRUE(animation->IsAnimatablePathProperty(RSAnimatableProperty::BOUNDS_POSITION));
}

/**
 * @tc.name: PathAnimationTest002
 * @tc.desc: Verify the  parameter and initial of PathAnimation
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSAnimationTest, PathAnimationTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest PathAnimationTest002 start";
    /**
     * @tc.steps: step1. init PathAnimation
     */
    string path = "L150 0 L50 50";
    Vector2f startValue = Vector2f(0, 0);
    Vector2f endValue = Vector2f(100, 100);
    std::unique_ptr<RSPathAnimation> animation =
        std::make_unique<RSPathAnimation>(RSAnimatableProperty::BOUNDS_WIDTH, path, startValue, endValue);
    animation->SetTimingCurve(RSAnimationTimingCurve::LINEAR);
    RSAnimationTimingCurve timingCurve = animation->GetTimingCurve();
    /**
     * @tc.steps: step2. start PathAnimation test
     */
    RotationMode testRotationModeData[] = {RotationMode::ROTATE_NONE, RotationMode::ROTATE_AUTO,
        RotationMode::ROTATE_AUTO_REVERSE};
    for (const auto& data : testRotationModeData) {
        animation->SetRotationMode(data);
        EXPECT_EQ(data, animation->GetRotationMode());
    }
    float testBeginFractionData[] = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    for (const auto& data : testBeginFractionData) {
        animation->SetBeginFraction(data);
        EXPECT_EQ(data, animation->GetBeginFraction());
    }
    float testEndFractionData[] = {0.6f, 0.7f, 0.8f, 0.9f, 1.0f};
    for (const auto& data : testEndFractionData) {
        animation->SetEndFraction(data);
        EXPECT_EQ(data, animation->GetEndFraction());
    }
    EXPECT_FALSE(animation->IsAnimatablePathProperty(RSAnimatableProperty::BOUNDS_WIDTH));
}

/**
 * @tc.name: AnimationCallbackTest001
 * @tc.desc: Verify the  parameter and initial of PathAnimation
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSAnimationTest, AnimationCallbackTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationCallbackTest001 start";
    /**
     * @tc.steps: step1. init animation callback and test data
     */
    int testData = 0;
    std::unique_ptr<AnimationCallback> callback = std::make_unique<AnimationCallback>([&]() { testData += 1; });
    /**
     * @tc.steps: step2. start animation callback test
     */
    callback->Run();
    EXPECT_EQ(1, testData);
}

/**
 * @tc.name: AnimationFinishCallbackTest001
 * @tc.desc: Verify the  parameter and initial of PathAnimation
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSAnimationTest, AnimationFinishCallbackTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationFinishCallbackTest001 start";
    /**
     * @tc.steps: step1. init animation finishCallback and test data
     */
    int testData = 0;
    int count = 1;
    std::unique_ptr<AnimationFinishCallback> callback =
        std::make_unique<AnimationFinishCallback>([&]() { testData += 1; }, count);
    /**
     * @tc.steps: step2. start animation finishCallback test
     */
    callback->Run();
    EXPECT_EQ(1, testData);
}

/**
 * @tc.name: AnimationFinishCallbackTest002
 * @tc.desc: Verify the  parameter and initial of PathAnimation
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSAnimationTest, AnimationFinishCallbackTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest AnimationFinishCallbackTest002 start";
    /**
     * @tc.steps: step1. init animation finishCallback and test data
     */
    int testData = 0;
    int count = 2;
    std::unique_ptr<AnimationFinishCallback> callback =
        std::make_unique<AnimationFinishCallback>([&]() { testData += 1; }, count);
    /**
     * @tc.steps: step2. start animation finishCallback test
     */
    callback->Run();
    EXPECT_EQ(0, testData);
}

/**
 * @tc.name: ImplicitAnimationParamTest001
 * @tc.desc: Verify the  implicit animation parameter
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSAnimationTest, ImplicitAnimationParamTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest ImplicitAnimationParamTest001 start";
    /**
     * @tc.steps: step1. init animation timing curve
     */
    RSAnimationTimingProtocol protocol;
    std::unique_ptr<RSImplicitAnimationParam> animationParam =
        std::make_unique<RSImplicitAnimationParam>(protocol);
    std::shared_ptr<RSAnimation> animation =
        animationParam->CreateAnimation(RSAnimatableProperty::BOUNDS_WIDTH, 0, 100);
    /**
     * @tc.steps: step2. start  implicit animation test
     */
    ImplicitAnimationParamType patamType = animationParam->GetType();
    EXPECT_TRUE(animation == nullptr);
    EXPECT_TRUE(patamType == ImplicitAnimationParamType::NONE);
}

/**
 * @tc.name: ImplicitCurveAnimationParamTest001
 * @tc.desc: Verify the  implicit animation parameter
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSAnimationTest, ImplicitCurveAnimationParamTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest ImplicitCurveAnimationParamTest001 start";
    /**
     * @tc.steps: step1. init animation timing curve
     */
    RSAnimationTimingProtocol protocol;
    RSAnimationTimingCurve curve = RSAnimationTimingCurve::EASE_IN_OUT;
    std::unique_ptr<RSImplicitCurveAnimationParam> animationParam =
        std::make_unique<RSImplicitCurveAnimationParam>(protocol, curve);
    std::shared_ptr<RSAnimation> animation =
        animationParam->CreateAnimation(RSAnimatableProperty::BOUNDS_WIDTH, 0, 100);
    /**
     * @tc.steps: step2. start implicit animation test
     */
    ImplicitAnimationParamType patamType = animationParam->GetType();
    EXPECT_TRUE(animation != nullptr);
    EXPECT_TRUE(patamType == ImplicitAnimationParamType::CURVE);
}

/**
 * @tc.name: ImplicitKeyframeAnimationParamTest001
 * @tc.desc: Verify the  implicit animation parameter
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSAnimationTest, ImplicitKeyframeAnimationParamTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest ImplicitKeyframeAnimationParamTest001 start";
    /**
     * @tc.steps: step1. init animation timing curve
     */
    RSAnimationTimingProtocol protocol;
    RSAnimationTimingCurve curve = RSAnimationTimingCurve::EASE_IN_OUT;
    float fraction = 0.1f;
    std::unique_ptr<RSImplicitKeyframeAnimationParam> animationParam =
        std::make_unique<RSImplicitKeyframeAnimationParam>(protocol, curve, fraction);
    std::shared_ptr<RSAnimation> animation =
        animationParam->CreateAnimation(RSAnimatableProperty::BOUNDS_WIDTH, 0, 100);
    animationParam->AddKeyframe(animation, 100, 200);
    /**
     * @tc.steps: step2. start implicit animation test
     */
    ImplicitAnimationParamType patamType = animationParam->GetType();
    EXPECT_TRUE(animation != nullptr);
    EXPECT_TRUE(patamType == ImplicitAnimationParamType::KEYFRAME);
}

/**
 * @tc.name: ImplicitPathAnimationParamTest001
 * @tc.desc: Verify the  implicit animation parameter
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSAnimationTest, ImplicitPathAnimationParamTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest ImplicitPathAnimationParamTest001 start";
    /**
     * @tc.steps: step1. init animation timing curve
     */
    RSAnimationTimingProtocol protocol;
    RSAnimationTimingCurve curve = RSAnimationTimingCurve::EASE_IN_OUT;
    string path = "L150 0 L50 50";
    std::shared_ptr<RSMotionPathOption> option = std::make_shared<RSMotionPathOption>(path);
    std::unique_ptr<RSImplicitPathAnimationParam> animationParam =
        std::make_unique<RSImplicitPathAnimationParam>(protocol, curve, option);
    Vector2f startValue = Vector2f(0, 0);
    Vector2f endValue = Vector2f(100, 100);
    std::shared_ptr<RSAnimation> animation =
        animationParam->CreateAnimation(RSAnimatableProperty::BOUNDS_POSITION, startValue, endValue);
    /**
     * @tc.steps: step2. start implicit animation test
     */
    ImplicitAnimationParamType patamType = animationParam->GetType();
    EXPECT_TRUE(animation != nullptr);
    EXPECT_TRUE(patamType == ImplicitAnimationParamType::PATH);
}

/**
 * @tc.name: ImplicitTransitionAnimationParamTest001
 * @tc.desc: Verify the  implicit animation parameter
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSAnimationTest, ImplicitTransitionAnimationParamTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest ImplicitTransitionAnimationParamTest001 start";
    /**
     * @tc.steps: step1. init animation timing curve
     */
    RSAnimationTimingProtocol protocol;
    RSAnimationTimingCurve curve = RSAnimationTimingCurve::EASE_IN_OUT;
    std::unique_ptr<RSImplicitTransitionParam> animationParam =
        std::make_unique<RSImplicitTransitionParam>(protocol, curve,
                                                    RSTransitionEffect(RSTransitionEffectType::FADE_IN));
    std::shared_ptr<RSAnimation> animation = animationParam->CreateAnimation();
    /**
     * @tc.steps: step2. start implicit animation test
     */
    ImplicitAnimationParamType patamType = animationParam->GetType();
    EXPECT_TRUE(animation != nullptr);
    EXPECT_TRUE(patamType == ImplicitAnimationParamType::TRANSITION);
}

/**
 * @tc.name: ImplicitAnimatorTest001
 * @tc.desc: Verify the  implicit animator parameter
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSAnimationTest, ImplicitAnimatorTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest ImplicitAnimatorTest001 start";
    /**
     * @tc.steps: step1. init animation timing curve
     */
    RSAnimationTimingProtocol protocol;
    RSAnimationTimingCurve curve = RSAnimationTimingCurve::EASE_IN_OUT;
    RSImplicitAnimator::Instance().OpenImplicitAnimation(protocol, curve, []() {});
    RSPropertyNode::SharedPtr node = RSNode::Create();
    float fraction = 0.2f;
    RSImplicitAnimator::Instance().BeginImplicitKeyFrameAnimation(fraction, curve);
    RSImplicitAnimator::Instance().BeginImplicitKeyFrameAnimation(fraction);
    RSImplicitAnimator::Instance().BeginImplicitTransition(RSTransitionEffect(RSTransitionEffectType::FADE_OUT));
    /**
     * @tc.steps: step2. start implicit animator test
     */
    EXPECT_TRUE(RSImplicitAnimator::Instance().NeedImplicitAnimaton());
    std::shared_ptr<RSAnimation> animation =
        RSImplicitAnimator::Instance().CreateImplicitAnimation(*node, RSAnimatableProperty::BOUNDS_WIDTH, 100, 200);
    EXPECT_FALSE(animation != nullptr);
    std::shared_ptr<RSAnimation> transition =
        RSImplicitAnimator::Instance().CreateImplicitTransition(*node);
    EXPECT_TRUE(transition != nullptr);
    RSImplicitAnimator::Instance().EndImplicitTransition();
    RSImplicitAnimator::Instance().EndImplicitKeyFrameAnimation();
    RSImplicitAnimator::Instance().NeedImplicitAnimaton();
}

/**
 * @tc.name: KeyframeAnimationTest001
 * @tc.desc: Verify the  parameter and initial of KeyframeAnimation
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSAnimationTest, KeyframeAnimationTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest KeyframeAnimationTest001 start";
    /**
     * @tc.steps: step1. init keyframe animation
     */
    std::unique_ptr<RSKeyframeAnimation<float>> animation =
        std::make_unique<RSKeyframeAnimation<float>>(RSAnimatableProperty::BOUNDS_WIDTH);
    animation->AddKeyFrame(0.1f, 100, RSAnimationTimingCurve::EASE_IN_OUT);
    animation->AddKeyFrame(0.5f, 200, RSAnimationTimingCurve::EASE_IN_OUT);
    animation->AddKeyFrame(1.0f, 300, RSAnimationTimingCurve::EASE_IN_OUT);
    /**
     * @tc.steps: step2. start animation test
     */
    EXPECT_TRUE(animation != nullptr);
}

/**
 * @tc.name: KeyframeAnimationTest002
 * @tc.desc: Verify the  parameter and initial of KeyframeAnimation
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSAnimationTest, KeyframeAnimationTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest KeyframeAnimationTest002 start";
    /**
     * @tc.steps: step1. init keyframe animation
     */
    std::unique_ptr<RSKeyframeAnimation<float>> animation =
        std::make_unique<RSKeyframeAnimation<float>>(RSAnimatableProperty::BOUNDS_WIDTH);
    std::vector<std::tuple<float, float, RSAnimationTimingCurve>> keyframes;
    keyframes.push_back(std::make_tuple(0.1f, 100, RSAnimationTimingCurve::LINEAR));
    keyframes.push_back(std::make_tuple(0.5f, 500, RSAnimationTimingCurve::LINEAR));
    keyframes.push_back(std::make_tuple(1.0f, 600, RSAnimationTimingCurve::LINEAR));
    animation->AddKeyFrames(keyframes);
    /**
     * @tc.steps: step2. start animation test
     */
    EXPECT_TRUE(animation != nullptr);
}

/**
 * @tc.name: PropertyAnimationTest001
 * @tc.desc: Verify the  parameter and initial of PathAnimation
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSAnimationTest, PropertyAnimationTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest PropertyAnimationTest001 start";
    /**
     * @tc.steps: step1. init property animation
     */
    std::unique_ptr<RSPropertyAnimation<float>> animation =
        std::make_unique<RSCurveAnimation<float>>(RSAnimatableProperty::BOUNDS_WIDTH, 100);
    /**
     * @tc.steps: step2. start animation test
     */
    EXPECT_TRUE(animation != nullptr);
}

/**
 * @tc.name: TransitionTest001
 * @tc.desc: Verify the  animation of transition
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSAnimationTest, TransitionTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest TransitionTest001 start";
    /**
     * @tc.steps: step1. init transition animation
     */
    std::unique_ptr<RSTransition> animation =
        std::make_unique<RSTransition>(RSTransitionEffect(RSTransitionEffectType::FADE_IN));
    animation->SetTransitionEffect(RSTransitionEffect(RSTransitionEffectType::FADE_OUT));
    animation->SetTimingCurve(RSAnimationTimingCurve::EASE);
    /**
     * @tc.steps: step2. start animation test
     */
    EXPECT_TRUE(animation != nullptr);
}
} // namespace Rosen
} // namespace OHOS
