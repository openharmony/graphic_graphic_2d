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

#include "animation/rs_curve_animation.h"
#include "include/animation/rs_steps_interpolator.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using namespace ANIMATIONTEST;
class RSAnimationTimingCurveTest : public RSAnimationBaseTest {
};

#ifndef MODIFIER_NG
/**
 * @tc.name: CreateCustomCurveTest001
 * @tc.desc: Verify the CreateCustomCurve of animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTimingCurveTest, CreateCustomCurveTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateCustomCurveTest001 start";
    /**
     * @tc.steps: step1. init animation
     */
    auto alphaProperty = std::make_shared<RSAnimatableProperty<float>>(ANIMATION_START_VALUE);
    auto alphaModifier = std::make_shared<RSAlphaModifier>(alphaProperty);
    canvasNode->AddModifier(alphaModifier);
    auto timingCurve = RSAnimationTimingCurve::CreateCustomCurve([](float input) { return input; });
    RSAnimationTimingProtocol protocol;
    auto curveAnimations = RSNode::Animate(protocol, timingCurve, [&alphaProperty]() {
        alphaProperty->Set(1.f);
    });
    /**
     * @tc.steps: step2. start animation test
     */
    ASSERT_TRUE(curveAnimations.size() == CORRECT_SIZE);
    EXPECT_TRUE(curveAnimations[FIRST_ANIMATION]);
    EXPECT_TRUE(curveAnimations[FIRST_ANIMATION]->IsRunning());
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateCustomCurveTest001 end";
}

/**
 * @tc.name: CreateCustomCurveTest002
 * @tc.desc: Verify the CreateCustomCurve of animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTimingCurveTest, CreateCustomCurveTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateCustomCurveTest002 start";
    /**
     * @tc.steps: step1. init animation
     */
    auto scaleProperty = std::make_shared<RSAnimatableProperty<Vector2f>>(ANIMATION_NORMAL_SCALE);
    auto scaleModifier = std::make_shared<RSScaleModifier>(scaleProperty);
    canvasNode->AddModifier(scaleModifier);
    auto timingCurve = RSAnimationTimingCurve::CreateCustomCurve([](float input) { return input * 2.f; });
    RSAnimationTimingProtocol protocol;
    auto curveAnimations = RSNode::Animate(protocol, timingCurve, [&scaleProperty]() {
        scaleProperty->Set(ANIMATION_DOUBLE_SCALE);
    });
    /**
     * @tc.steps: step2. start animation test
     */
    EXPECT_TRUE(curveAnimations.size() == CORRECT_SIZE);
    EXPECT_TRUE(curveAnimations[FIRST_ANIMATION]);
    EXPECT_TRUE(curveAnimations[FIRST_ANIMATION]->IsRunning());
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateCustomCurveTest002 end";
}

/**
 * @tc.name: CreateCustomCurveTest003
 * @tc.desc: Verify the CreateCustomCurve of animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTimingCurveTest, CreateCustomCurveTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateCustomCurveTest003 start";
    /**
     * @tc.steps: step1. init animation
     */
    auto rotationProperty = std::make_shared<RSAnimatableProperty<float>>(ANIMATION_START_VALUE);
    auto rotationModifier = std::make_shared<RSRotationModifier>(rotationProperty);
    canvasNode->AddModifier(rotationModifier);
    auto timingCurve = RSAnimationTimingCurve::CreateCustomCurve([](float input) { return 0.f; });
    RSAnimationTimingProtocol protocol;
    auto curveAnimations = RSNode::Animate(protocol, timingCurve, [&rotationProperty]() {
        rotationProperty->Set(90.f);
    });
    /**
     * @tc.steps: step2. start animation test
     */
    EXPECT_TRUE(curveAnimations.size() == CORRECT_SIZE);
    EXPECT_TRUE(curveAnimations[FIRST_ANIMATION]);
    EXPECT_TRUE(curveAnimations[FIRST_ANIMATION]->IsRunning());
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateCustomCurveTest003 end";
}

/**
 * @tc.name: CreateCustomCurveTest004
 * @tc.desc: Verify the CreateCustomCurve of animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTimingCurveTest, CreateCustomCurveTest004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateCustomCurveTest004 start";
    /**
     * @tc.steps: step1. init animation
     */
    auto alphaProperty = std::make_shared<RSAnimatableProperty<float>>(ANIMATION_START_VALUE);
    auto alphaModifier = std::make_shared<RSAlphaModifier>(alphaProperty);
    canvasNode->AddModifier(alphaModifier);
    auto interpolator = RSAnimationTimingCurve::DEFAULT.interpolator_;
    auto timingCurveDefault = RSAnimationTimingCurve::CreateCustomCurve([&](float input) {
        return interpolator->Interpolate(input);
    });
    RSAnimationTimingProtocol protocol;
    auto curveAnimations = RSNode::Animate(protocol, timingCurveDefault, [&alphaProperty]() {
        alphaProperty->Set(1.f);
    });
    /**
     * @tc.steps: step2. start animation test
     */
    ASSERT_TRUE(curveAnimations.size() == CORRECT_SIZE);
    ASSERT_TRUE(curveAnimations[FIRST_ANIMATION]);
    EXPECT_TRUE(curveAnimations[FIRST_ANIMATION]->IsRunning());
    EXPECT_TRUE(timingCurveDefault.type_ == RSAnimationTimingCurve::DEFAULT.type_);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateCustomCurveTest004 end";
}

/**
 * @tc.name: CreateCustomCurveTest005
 * @tc.desc: Verify the CreateCustomCurve of animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTimingCurveTest, CreateCustomCurveTest005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateCustomCurveTest005 start";
    /**
     * @tc.steps: step1. init animation
     */
    auto alphaProperty = std::make_shared<RSAnimatableProperty<float>>(ANIMATION_START_VALUE);
    auto alphaModifier = std::make_shared<RSAlphaModifier>(alphaProperty);
    canvasNode->AddModifier(alphaModifier);
    auto interpolator = RSAnimationTimingCurve::LINEAR.interpolator_;
    auto timingCurveLinear = RSAnimationTimingCurve::CreateCustomCurve([&](float input) {
        return interpolator->Interpolate(input);
    });
    RSAnimationTimingProtocol protocol;
    auto curveAnimations = RSNode::Animate(protocol, timingCurveLinear, [&alphaProperty]() {
        alphaProperty->Set(1.f);
    });
    /**
     * @tc.steps: step2. start animation test
     */
    ASSERT_TRUE(curveAnimations.size() == CORRECT_SIZE);
    ASSERT_TRUE(curveAnimations[FIRST_ANIMATION]);
    EXPECT_TRUE(curveAnimations[FIRST_ANIMATION]->IsRunning());
    EXPECT_TRUE(timingCurveLinear.type_ == RSAnimationTimingCurve::LINEAR.type_);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateCustomCurveTest005 end";
}

/**
 * @tc.name: CreateCustomCurveTest006
 * @tc.desc: Verify the CreateCustomCurve of animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTimingCurveTest, CreateCustomCurveTest006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateCustomCurveTest006 start";
    /**
     * @tc.steps: step1. init animation
     */
    auto alphaProperty = std::make_shared<RSAnimatableProperty<float>>(ANIMATION_START_VALUE);
    auto alphaModifier = std::make_shared<RSAlphaModifier>(alphaProperty);
    canvasNode->AddModifier(alphaModifier);
    auto interpolator = RSAnimationTimingCurve::EASE.interpolator_;
    auto timingCurveEase = RSAnimationTimingCurve::CreateCustomCurve([&](float input) {
        return interpolator->Interpolate(input);
    });
    RSAnimationTimingProtocol protocol;
    auto curveAnimations = RSNode::Animate(protocol, timingCurveEase, [&alphaProperty]() {
        alphaProperty->Set(1.f);
    });
    /**
     * @tc.steps: step2. start animation test
     */
    ASSERT_TRUE(curveAnimations.size() == CORRECT_SIZE);
    ASSERT_TRUE(curveAnimations[FIRST_ANIMATION]);
    EXPECT_TRUE(curveAnimations[FIRST_ANIMATION]->IsRunning());
    EXPECT_TRUE(timingCurveEase.type_ == RSAnimationTimingCurve::EASE.type_);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateCustomCurveTest006 end";
}

/**
 * @tc.name: CreateCustomCurveTest007
 * @tc.desc: Verify the CreateCustomCurve of animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTimingCurveTest, CreateCustomCurveTest007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateCustomCurveTest007 start";
    /**
     * @tc.steps: step1. init animation
     */
    auto alphaProperty = std::make_shared<RSAnimatableProperty<float>>(ANIMATION_START_VALUE);
    auto alphaModifier = std::make_shared<RSAlphaModifier>(alphaProperty);
    canvasNode->AddModifier(alphaModifier);
    auto interpolator = RSAnimationTimingCurve::EASE_IN.interpolator_;
    auto timingCurveEaseIn = RSAnimationTimingCurve::CreateCustomCurve([&](float input) {
        return interpolator->Interpolate(input);
    });
    RSAnimationTimingProtocol protocol;
    auto curveAnimations = RSNode::Animate(protocol, timingCurveEaseIn, [&alphaProperty]() {
        alphaProperty->Set(1.f);
    });
    /**
     * @tc.steps: step2. start animation test
     */
    ASSERT_TRUE(curveAnimations.size() == CORRECT_SIZE);
    ASSERT_TRUE(curveAnimations[FIRST_ANIMATION]);
    EXPECT_TRUE(curveAnimations[FIRST_ANIMATION]->IsRunning());
    EXPECT_TRUE(timingCurveEaseIn.type_ == RSAnimationTimingCurve::EASE_IN.type_);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateCustomCurveTest007 end";
}

/**
 * @tc.name: CreateCustomCurveTest008
 * @tc.desc: Verify the CreateCustomCurve of animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTimingCurveTest, CreateCustomCurveTest008, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateCustomCurveTest008 start";
    /**
     * @tc.steps: step1. init animation
     */
    auto alphaProperty = std::make_shared<RSAnimatableProperty<float>>(ANIMATION_START_VALUE);
    auto alphaModifier = std::make_shared<RSAlphaModifier>(alphaProperty);
    canvasNode->AddModifier(alphaModifier);
    auto interpolator = RSAnimationTimingCurve::EASE_OUT.interpolator_;
    auto timingCurveEaseOut = RSAnimationTimingCurve::CreateCustomCurve([&](float input) {
        return interpolator->Interpolate(input);
    });
    RSAnimationTimingProtocol protocol;
    auto curveAnimations = RSNode::Animate(protocol, timingCurveEaseOut, [&alphaProperty]() {
        alphaProperty->Set(1.f);
    });
    /**
     * @tc.steps: step2. start animation test
     */
    ASSERT_TRUE(curveAnimations.size() == CORRECT_SIZE);
    ASSERT_TRUE(curveAnimations[FIRST_ANIMATION]);
    EXPECT_TRUE(curveAnimations[FIRST_ANIMATION]->IsRunning());
    EXPECT_TRUE(timingCurveEaseOut.type_ == RSAnimationTimingCurve::EASE_OUT.type_);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateCustomCurveTest008 end";
}

/**
 * @tc.name: CreateCustomCurveTest009
 * @tc.desc: Verify the CreateCustomCurve of animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTimingCurveTest, CreateCustomCurveTest009, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateCustomCurveTest009 start";
    /**
     * @tc.steps: step1. init animation
     */
    auto alphaProperty = std::make_shared<RSAnimatableProperty<float>>(ANIMATION_START_VALUE);
    auto alphaModifier = std::make_shared<RSAlphaModifier>(alphaProperty);
    canvasNode->AddModifier(alphaModifier);
    auto interpolator = RSAnimationTimingCurve::EASE_IN_OUT.interpolator_;
    auto timingCurveEaseInOut = RSAnimationTimingCurve::CreateCustomCurve([&](float input) {
        return interpolator->Interpolate(input);
    });
    RSAnimationTimingProtocol protocol;
    auto curveAnimations = RSNode::Animate(protocol, timingCurveEaseInOut, [&alphaProperty]() {
        alphaProperty->Set(1.f);
    });
    /**
     * @tc.steps: step2. start animation test
     */
    ASSERT_TRUE(curveAnimations.size() == CORRECT_SIZE);
    ASSERT_TRUE(curveAnimations[FIRST_ANIMATION]);
    EXPECT_TRUE(curveAnimations[FIRST_ANIMATION]->IsRunning());
    EXPECT_TRUE(timingCurveEaseInOut.type_ == RSAnimationTimingCurve::EASE_IN_OUT.type_);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateCustomCurveTest009 end";
}

/**
 * @tc.name: CreateCustomCurveTest010
 * @tc.desc: Verify the CreateCustomCurve of animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTimingCurveTest, CreateCustomCurveTest010, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateCustomCurveTest010 start";
    /**
     * @tc.steps: step1. init animation
     */
    auto alphaProperty = std::make_shared<RSAnimatableProperty<float>>(ANIMATION_START_VALUE);
    auto alphaModifier = std::make_shared<RSAlphaModifier>(alphaProperty);
    canvasNode->AddModifier(alphaModifier);
    auto interpolator = RSAnimationTimingCurve::SPRING.interpolator_;
    auto timingCurveSpring = RSAnimationTimingCurve::CreateCustomCurve([&](float input) {
        return interpolator ? interpolator->Interpolate(input) : input;
    });
    RSAnimationTimingProtocol protocol;
    auto curveAnimations = RSNode::Animate(protocol, timingCurveSpring, [&alphaProperty]() {
        alphaProperty->Set(1.f);
    });
    /**
     * @tc.steps: step2. start animation test
     */
    ASSERT_TRUE(curveAnimations.size() == CORRECT_SIZE);
    ASSERT_TRUE(curveAnimations[FIRST_ANIMATION]);
    EXPECT_TRUE(curveAnimations[FIRST_ANIMATION]->IsRunning());
    EXPECT_TRUE(timingCurveSpring.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateCustomCurveTest010 end";
}

/**
 * @tc.name: CreateCustomCurveTest011
 * @tc.desc: Verify the CreateCustomCurve of animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTimingCurveTest, CreateCustomCurveTest011, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateCustomCurveTest011 start";
    /**
     * @tc.steps: step1. init animation
     */
    auto alphaProperty = std::make_shared<RSAnimatableProperty<float>>(ANIMATION_START_VALUE);
    auto alphaModifier = std::make_shared<RSAlphaModifier>(alphaProperty);
    canvasNode->AddModifier(alphaModifier);
    auto interpolator = RSAnimationTimingCurve::INTERACTIVE_SPRING.interpolator_;
    auto timingCurveInteractiveSpring = RSAnimationTimingCurve::CreateCustomCurve([&](float input) {
        return interpolator ? interpolator->Interpolate(input) : input;
    });
    RSAnimationTimingProtocol protocol;
    auto curveAnimations = RSNode::Animate(protocol, timingCurveInteractiveSpring, [&alphaProperty]() {
        alphaProperty->Set(1.f);
    });
    /**
     * @tc.steps: step2. start animation test
     */
    ASSERT_TRUE(curveAnimations.size() == CORRECT_SIZE);
    ASSERT_TRUE(curveAnimations[FIRST_ANIMATION]);
    EXPECT_TRUE(curveAnimations[FIRST_ANIMATION]->IsRunning());
    EXPECT_TRUE(timingCurveInteractiveSpring.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateCustomCurveTest011 end";
}

/**
 * @tc.name: CreateCustomCurveTest012
 * @tc.desc: Verify the CreateCustomCurve of animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTimingCurveTest, CreateCustomCurveTest012, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateCustomCurveTest012 start";
    /**
     * @tc.steps: step1. init animation
     */
    auto alphaProperty = std::make_shared<RSAnimatableProperty<float>>(ANIMATION_START_VALUE);
    auto alphaModifier = std::make_shared<RSAlphaModifier>(alphaProperty);
    canvasNode->AddModifier(alphaModifier);
    RSAnimationTimingCurve cubicCurve = RSAnimationTimingCurve::CreateCubicCurve(0.4f, 0.0f, 0.6f, 1.0f);
    auto interpolator = cubicCurve.interpolator_;
    auto timingCurveCubic = RSAnimationTimingCurve::CreateCustomCurve([&](float input) {
        return interpolator->Interpolate(input);
    });
    RSAnimationTimingProtocol protocol;
    auto curveAnimations = RSNode::Animate(protocol, timingCurveCubic, [&alphaProperty]() {
        alphaProperty->Set(1.f);
    });
    /**
     * @tc.steps: step2. start animation test
     */
    ASSERT_TRUE(curveAnimations.size() == CORRECT_SIZE);
    ASSERT_TRUE(curveAnimations[FIRST_ANIMATION]);
    EXPECT_TRUE(curveAnimations[FIRST_ANIMATION]->IsRunning());
    EXPECT_TRUE(timingCurveCubic.type_ == cubicCurve.type_);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateCustomCurveTest012 end";
}

/**
 * @tc.name: CreateCustomCurveTest013
 * @tc.desc: Verify the CreateCustomCurve of animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTimingCurveTest, CreateCustomCurveTest013, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateCustomCurveTest013 start";
    /**
     * @tc.steps: step1. init animation
     */
    auto alphaProperty = std::make_shared<RSAnimatableProperty<float>>(ANIMATION_START_VALUE);
    auto alphaModifier = std::make_shared<RSAlphaModifier>(alphaProperty);
    canvasNode->AddModifier(alphaModifier);
    RSAnimationTimingCurve springCurve = RSAnimationTimingCurve::CreateSpringCurve(0.1f, 0.23f, 0.34f, 0.43f);
    auto interpolator = springCurve.interpolator_;
    auto timingCurveSpring = RSAnimationTimingCurve::CreateCustomCurve([&](float input) {
        return interpolator->Interpolate(input);
    });
    RSAnimationTimingProtocol protocol;
    auto curveAnimations = RSNode::Animate(protocol, timingCurveSpring, [&alphaProperty]() {
        alphaProperty->Set(1.f);
    });
    /**
     * @tc.steps: step2. start animation test
     */
    ASSERT_TRUE(curveAnimations.size() == CORRECT_SIZE);
    ASSERT_TRUE(curveAnimations[FIRST_ANIMATION]);
    EXPECT_TRUE(curveAnimations[FIRST_ANIMATION]->IsRunning());
    EXPECT_TRUE(timingCurveSpring.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateCustomCurveTest013 end";
}

/**
 * @tc.name: CreateCustomCurveTest014
 * @tc.desc: Verify the CreateCustomCurve of animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTimingCurveTest, CreateCustomCurveTest014, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateCustomCurveTest014 start";
    /**
     * @tc.steps: step1. init animation
     */
    auto alphaProperty = std::make_shared<RSAnimatableProperty<float>>(ANIMATION_START_VALUE);
    auto alphaModifier = std::make_shared<RSAlphaModifier>(alphaProperty);
    canvasNode->AddModifier(alphaModifier);
    RSAnimationTimingCurve timeCurve = RSAnimationTimingCurve();
    auto interpolator = timeCurve.interpolator_;
    auto defaultConstructtimingCurve = RSAnimationTimingCurve::CreateCustomCurve([&](float input) {
        return interpolator->Interpolate(input);
    });
    RSAnimationTimingProtocol protocol;
    auto curveAnimations = RSNode::Animate(protocol, defaultConstructtimingCurve, [&alphaProperty]() {
        alphaProperty->Set(1.f);
    });
    /**
     * @tc.steps: step2. start animation test
     */
    ASSERT_TRUE(curveAnimations.size() == CORRECT_SIZE);
    ASSERT_TRUE(curveAnimations[FIRST_ANIMATION]);
    EXPECT_TRUE(curveAnimations[FIRST_ANIMATION]->IsRunning());
    EXPECT_TRUE(defaultConstructtimingCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateCustomCurveTest013 end";
}

/**
 * @tc.name: CreateCubicCurveTest001
 * @tc.desc: Verify the CreateCubicCurve of animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTimingCurveTest, CreateCubicCurveTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateCubicCurveTest001 start";
    /**
     * @tc.steps: step1. init animation
     */
    auto alphaProperty = std::make_shared<RSAnimatableProperty<float>>(ANIMATION_START_VALUE);
    auto alphaModifier = std::make_shared<RSAlphaModifier>(alphaProperty);
    canvasNode->AddModifier(alphaModifier);
    RSAnimationTimingCurve timingCurve = RSAnimationTimingCurve::CreateCubicCurve(0.42f, 0.0f, 0.58f, 1.0f);
    RSAnimationTimingProtocol protocol;
    auto curveAnimations = RSNode::Animate(protocol, timingCurve, [&alphaProperty]() {
        alphaProperty->Set(1.f);
    });
    /**
     * @tc.steps: step2. start animation test
     */
    ASSERT_TRUE(curveAnimations.size() == CORRECT_SIZE);
    EXPECT_FALSE(curveAnimations[FIRST_ANIMATION] == nullptr);
    EXPECT_TRUE(curveAnimations[FIRST_ANIMATION]->IsRunning());
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateCubicCurveTest001 end";
}

/**
 * @tc.name: CreateCubicCurveTest002
 * @tc.desc: Verify the CreateCubicCurve of animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTimingCurveTest, CreateCubicCurveTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateCubicCurveTest002 start";
    /**
     * @tc.steps: step1. init animation
     */
    auto scaleProperty = std::make_shared<RSAnimatableProperty<Vector2f>>(ANIMATION_NORMAL_SCALE);
    auto scaleModifier = std::make_shared<RSScaleModifier>(scaleProperty);
    canvasNode->AddModifier(scaleModifier);
    RSAnimationTimingCurve timingCurve = RSAnimationTimingCurve::CreateCubicCurve(0.25f, 0.1f, 0.25f, 1.0f);
    RSAnimationTimingProtocol protocol;
    auto curveAnimations = RSNode::Animate(protocol, timingCurve, [&scaleProperty]() {
        scaleProperty->Set(ANIMATION_DOUBLE_SCALE);
    });
    /**
     * @tc.steps: step2. start animation test
     */
    ASSERT_TRUE(curveAnimations.size() == CORRECT_SIZE);
    EXPECT_FALSE(curveAnimations[FIRST_ANIMATION] == nullptr);
    EXPECT_TRUE(curveAnimations[FIRST_ANIMATION]->IsRunning());
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateCubicCurveTest002 end";
}

/**
 * @tc.name: CreateCubicCurveTest003
 * @tc.desc: Verify the CreateCubicCurve of animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTimingCurveTest, CreateCubicCurveTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateCubicCurveTest003 start";
    /**
     * @tc.steps: step1. init animation
     */
    auto rotationProperty = std::make_shared<RSAnimatableProperty<float>>(ANIMATION_START_VALUE);
    auto rotationModifier = std::make_shared<RSRotationModifier>(rotationProperty);
    canvasNode->AddModifier(rotationModifier);
    RSAnimationTimingCurve timingCurve = RSAnimationTimingCurve::CreateCubicCurve(0.0f, 0.0f, 0.58f, 1.0f);
    RSAnimationTimingProtocol protocol;
    auto curveAnimations = RSNode::Animate(protocol, timingCurve, [&rotationProperty]() {
        rotationProperty->Set(90.f);
    });
    /**
     * @tc.steps: step2. start animation test
     */
    ASSERT_TRUE(curveAnimations.size() == CORRECT_SIZE);
    EXPECT_FALSE(curveAnimations[FIRST_ANIMATION] == nullptr);
    EXPECT_TRUE(curveAnimations[FIRST_ANIMATION]->IsRunning());
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateCubicCurveTest003 end";
}

/**
 * @tc.name: CreateStepsCurveTest001
 * @tc.desc: Verify the CreateStepsCurve of animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTimingCurveTest, CreateStepsCurveTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateStepsCurveTest001 start";
    /**
     * @tc.steps: step1. init animation
     */
    auto scaleProperty = std::make_shared<RSAnimatableProperty<Vector2f>>(ANIMATION_NORMAL_SCALE);
    auto scaleModifier = std::make_shared<RSScaleModifier>(scaleProperty);
    canvasNode->AddModifier(scaleModifier);
    auto timingCurve = RSAnimationTimingCurve::CreateStepsCurve(5, StepsCurvePosition::START);
    RSAnimationTimingProtocol protocol;
    auto curveAnimations = RSNode::Animate(protocol, timingCurve, [&scaleProperty]() {
        scaleProperty->Set(ANIMATION_DOUBLE_SCALE);
    });
    /**
     * @tc.steps: step2. start animation test
     */
    ASSERT_TRUE(curveAnimations.size() == CORRECT_SIZE);
    EXPECT_FALSE(curveAnimations[FIRST_ANIMATION] == nullptr);
    EXPECT_TRUE(curveAnimations[FIRST_ANIMATION]->IsRunning());
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateStepsCurveTest001 end";
}

/**
 * @tc.name: CreateStepsCurveTest002
 * @tc.desc: Verify the CreateStepsCurve of animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTimingCurveTest, CreateStepsCurveTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateStepsCurveTest002 start";
    /**
     * @tc.steps: step1. init animation
     */
    auto rotationProperty = std::make_shared<RSAnimatableProperty<float>>(ANIMATION_START_VALUE);
    auto rotationModifier = std::make_shared<RSRotationModifier>(rotationProperty);
    canvasNode->AddModifier(rotationModifier);
    auto timingCurve = RSAnimationTimingCurve::CreateStepsCurve(5, StepsCurvePosition::END);
    RSAnimationTimingProtocol protocol;
    auto curveAnimations = RSNode::Animate(protocol, timingCurve, [&rotationProperty]() {
        rotationProperty->Set(90.f);
    });
    /**
     * @tc.steps: step2. start animation test
     */
    ASSERT_TRUE(curveAnimations.size() == CORRECT_SIZE);
    EXPECT_FALSE(curveAnimations[FIRST_ANIMATION] == nullptr);
    EXPECT_TRUE(curveAnimations[FIRST_ANIMATION]->IsRunning());
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateStepsCurveTest002 end";
}

/**
 * @tc.name: CreateSpringCurveTest001
 * @tc.desc: Verify the CreateSpringCurve of animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTimingCurveTest, CreateSpringCurveTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateSpringCurveTest001 start";
    /**
     * @tc.steps: step1. init animation
     */
    auto scaleProperty = std::make_shared<RSAnimatableProperty<Vector2f>>(ANIMATION_NORMAL_SCALE);
    auto scaleModifier = std::make_shared<RSScaleModifier>(scaleProperty);
    canvasNode->AddModifier(scaleModifier);
    auto timingCurve = RSAnimationTimingCurve::CreateSpringCurve(0.1f, 0.2f, 0.3f, 0.4f);
    RSAnimationTimingProtocol protocol;
    auto curveAnimations = RSNode::Animate(protocol, timingCurve, [&scaleProperty]() {
        scaleProperty->Set(ANIMATION_DOUBLE_SCALE);
    });
    /**
     * @tc.steps: step2. start animation test
     */
    ASSERT_TRUE(curveAnimations.size() == CORRECT_SIZE);
    EXPECT_FALSE(curveAnimations[FIRST_ANIMATION] == nullptr);
    EXPECT_TRUE(curveAnimations[FIRST_ANIMATION]->IsRunning());
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateSpringCurveTest001 end";
}

/**
 * @tc.name: CreateSpringCurveTest002
 * @tc.desc: Verify the CreateSpringCurve of animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTimingCurveTest, CreateSpringCurveTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateSpringCurveTest002 start";
    /**
     * @tc.steps: step1. init animation
     */
    auto scaleProperty = std::make_shared<RSAnimatableProperty<Vector2f>>(ANIMATION_NORMAL_SCALE);
    auto scaleModifier = std::make_shared<RSScaleModifier>(scaleProperty);
    canvasNode->AddModifier(scaleModifier);
    auto timingCurve = RSAnimationTimingCurve::CreateSpringCurve(0.4f, 0.3f, 0.2f, 0.1f);
    RSAnimationTimingProtocol protocol;
    auto curveAnimations = RSNode::Animate(protocol, timingCurve, [&scaleProperty]() {
        scaleProperty->Set(ANIMATION_DOUBLE_SCALE);
    });
    /**
     * @tc.steps: step2. start animation test
     */
    ASSERT_TRUE(curveAnimations.size() == CORRECT_SIZE);
    EXPECT_FALSE(curveAnimations[FIRST_ANIMATION] == nullptr);
    EXPECT_TRUE(curveAnimations[FIRST_ANIMATION]->IsRunning());
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateSpringCurveTest002 end";
}

/**
 * @tc.name: CreateSpringTest001
 * @tc.desc: Verify the CreateSpring of animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTimingCurveTest, CreateSpringTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateSpringTest001 start";
    /**
     * @tc.steps: step1. init animation
     */
    auto rotationProperty = std::make_shared<RSAnimatableProperty<float>>(ANIMATION_START_VALUE);
    auto rotationModifier = std::make_shared<RSRotationModifier>(rotationProperty);
    canvasNode->AddModifier(rotationModifier);
    auto timingCurve = RSAnimationTimingCurve::CreateSpring(0.1f, 0.5f);
    RSAnimationTimingProtocol protocol;
    auto curveAnimations = RSNode::Animate(protocol, timingCurve, [&rotationProperty]() {
        rotationProperty->Set(90.f);
    });
    /**
     * @tc.steps: step2. start animation test
     */
    ASSERT_TRUE(curveAnimations.size() == CORRECT_SIZE);
    EXPECT_FALSE(curveAnimations[FIRST_ANIMATION] == nullptr);
    EXPECT_TRUE(curveAnimations[FIRST_ANIMATION]->IsRunning());
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::SPRING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateSpringTest001 end";
}

/**
 * @tc.name: CreateSpringTest002
 * @tc.desc: Verify the CreateCustomCurve of animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTimingCurveTest, CreateSpringTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateSpringTest002 start";
    /**
     * @tc.steps: step1. init animation
     */
    auto rotationProperty = std::make_shared<RSAnimatableProperty<float>>(ANIMATION_START_VALUE);
    auto rotationModifier = std::make_shared<RSRotationModifier>(rotationProperty);
    canvasNode->AddModifier(rotationModifier);
    auto timingCurve = RSAnimationTimingCurve::CreateSpring(0.5f, 1.0f, 0.1f);
    RSAnimationTimingProtocol protocol;
    auto curveAnimations = RSNode::Animate(protocol, timingCurve, [&rotationProperty]() {
        rotationProperty->Set(90.f);
    });
    /**
     * @tc.steps: step2. start animation test
     */
    ASSERT_TRUE(curveAnimations.size() == CORRECT_SIZE);
    EXPECT_FALSE(curveAnimations[FIRST_ANIMATION] == nullptr);
    EXPECT_TRUE(curveAnimations[FIRST_ANIMATION]->IsRunning());
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::SPRING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateSpringTest002 end";
}
#endif

/**
 * @tc.name: CreateSpringCurve001
 * @tc.desc: Verify the CreateSpringCurve of animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTimingCurveTest, CreateSpringCurve001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateSpringTest001 start";
    float velocity = 0.5f;
    float mass = 0.5f;
    float stiffness = 0.0f;
    float damping = 0.5f;
    auto timingCurve = RSAnimationTimingCurve::CreateSpringCurve(velocity, mass, stiffness, damping);
    auto timingCurve_ptr = std::make_shared<RSAnimationTimingCurve>(timingCurve);
    EXPECT_TRUE(timingCurve_ptr != nullptr);

    float mass1 = 0.0f;
    float stiffness1 = 1.0f;
    auto timingCurve1 = RSAnimationTimingCurve::CreateSpringCurve(velocity, mass1, stiffness1, damping);
    auto timingCurve_ptr1 = std::make_shared<RSAnimationTimingCurve>(timingCurve);
    EXPECT_TRUE(timingCurve_ptr1 != nullptr);
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateSpringCurve001 end";
}

/**
 * @tc.name: CreateInterpolatingSpring001
 * @tc.desc: Verify the CreateInterpolatingSpring of animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTimingCurveTest, CreateInterpolatingSpring001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateInterpolatingSpring001 start";
    float mass = 0.5f;
    float stiffness = 0.0f;
    float damping = 0.5f;
    float velocity = 1.0f;
    auto timingCurve = RSAnimationTimingCurve::CreateInterpolatingSpring(mass, stiffness, damping, velocity);
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING_SPRING);

    float mass1 = 0.5f;
    float stiffness1 = 0.5f;
    auto timingCurve1 = RSAnimationTimingCurve::CreateInterpolatingSpring(mass1, stiffness1, damping, velocity);
    EXPECT_TRUE(timingCurve1.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING_SPRING);

    float mass2 = -1.0f;
    float stiffness2 = 0.5f;
    auto timingCurve2 = RSAnimationTimingCurve::CreateInterpolatingSpring(mass2, stiffness2, damping, velocity);
    EXPECT_TRUE(timingCurve2.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING_SPRING);
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateInterpolatingSpring001 end";
}

#ifndef MODIFIER_NG
/**
 * @tc.name: CreateCubicCurveTest004
 * @tc.desc: Verify the CreateCubicCurve of animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTimingCurveTest, CreateCubicCurveTest004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateCubicCurveTest004 start";
    /**
     * @tc.steps: step1. init animation
     */
    auto alphaProperty = std::make_shared<RSAnimatableProperty<float>>(ANIMATION_START_VALUE);
    auto alphaModifier = std::make_shared<RSAlphaModifier>(alphaProperty);
    canvasNode->AddModifier(alphaModifier);
    RSAnimationTimingCurve timingCurve = RSAnimationTimingCurve::CreateCubicCurve(0.9f, 0.1f, 0.56f, 1.0f);
    RSAnimationTimingProtocol protocol;
    auto curveAnimations = RSNode::Animate(protocol, timingCurve, [&alphaProperty]() {
        alphaProperty->Set(0.6f);
    });
    /**
     * @tc.steps: step2. start animation test
     */
    ASSERT_TRUE(curveAnimations.size() == CORRECT_SIZE);
    ASSERT_TRUE(curveAnimations[FIRST_ANIMATION] != nullptr);
    EXPECT_TRUE(curveAnimations[FIRST_ANIMATION]->IsRunning());
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateCubicCurveTest004 end";
}

/**
 * @tc.name: CreateCubicCurveTest005
 * @tc.desc: Verify the CreateCubicCurve of animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTimingCurveTest, CreateCubicCurveTest005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateCubicCurveTest005 start";
    /**
     * @tc.steps: step1. init animation
     */
    auto scaleProperty = std::make_shared<RSAnimatableProperty<Vector2f>>(ANIMATION_NORMAL_SCALE);
    auto scaleModifier = std::make_shared<RSScaleModifier>(scaleProperty);
    canvasNode->AddModifier(scaleModifier);
    RSAnimationTimingCurve timingCurve = RSAnimationTimingCurve::CreateCubicCurve(0.234f, 0.12f, 0.25f, 1.0f);
    RSAnimationTimingProtocol protocol;
    auto curveAnimations = RSNode::Animate(protocol, timingCurve, [&scaleProperty]() {
        scaleProperty->Set(ANIMATION_DOUBLE_SCALE);
    });
    /**
     * @tc.steps: step2. start animation test
     */
    ASSERT_TRUE(curveAnimations.size() == CORRECT_SIZE);
    ASSERT_TRUE(curveAnimations[FIRST_ANIMATION] != nullptr);
    EXPECT_TRUE(curveAnimations[FIRST_ANIMATION]->IsRunning());
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateCubicCurveTest005 end";
}

/**
 * @tc.name: CreateCubicCurveTest006
 * @tc.desc: Verify the CreateCubicCurve of animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTimingCurveTest, CreateCubicCurveTest006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateCubicCurveTest006 start";
    /**
     * @tc.steps: step1. init animation
     */
    auto rotationProperty = std::make_shared<RSAnimatableProperty<float>>(ANIMATION_START_VALUE);
    auto rotationModifier = std::make_shared<RSRotationModifier>(rotationProperty);
    canvasNode->AddModifier(rotationModifier);
    RSAnimationTimingCurve timingCurve = RSAnimationTimingCurve::CreateCubicCurve(0.33f, 0.5f, 0.58f, 1.0f);
    RSAnimationTimingProtocol protocol;
    auto curveAnimations = RSNode::Animate(protocol, timingCurve, [&rotationProperty]() {
        rotationProperty->Set(90.f);
    });
    /**
     * @tc.steps: step2. start animation test
     */
    ASSERT_TRUE(curveAnimations.size() == CORRECT_SIZE);
    ASSERT_TRUE(curveAnimations[FIRST_ANIMATION] != nullptr);
    EXPECT_TRUE(curveAnimations[FIRST_ANIMATION]->IsRunning());
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateCubicCurveTest006 end";
}

/**
 * @tc.name: CreateCubicCurveTest007
 * @tc.desc: Verify the CreateCubicCurve of animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTimingCurveTest, CreateCubicCurveTest007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateCubicCurveTest007 start";
    /**
     * @tc.steps: step1. init animation
     */
    auto rotationProperty = std::make_shared<RSAnimatableProperty<float>>(ANIMATION_START_VALUE);
    auto rotationModifier = std::make_shared<RSRotationModifier>(rotationProperty);
    canvasNode->AddModifier(rotationModifier);
    RSAnimationTimingCurve timingCurve = RSAnimationTimingCurve::CreateCubicCurve(0.33f, 0.4f, 0.58f, 1.0f);
    RSAnimationTimingProtocol protocol;
    auto curveAnimations = RSNode::Animate(protocol, timingCurve, [&rotationProperty]() {
        rotationProperty->Set(90.f);
    });
    /**
     * @tc.steps: step2. start animation test
     */
    ASSERT_TRUE(curveAnimations.size() == CORRECT_SIZE);
    ASSERT_TRUE(curveAnimations[FIRST_ANIMATION] != nullptr);
    EXPECT_TRUE(curveAnimations[FIRST_ANIMATION]->IsRunning());
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateCubicCurveTest007 end";
}

/**
 * @tc.name: CreateCubicCurveTest008
 * @tc.desc: Verify the CreateCubicCurve of animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTimingCurveTest, CreateCubicCurveTest008, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateCubicCurveTest008 start";
    /**
     * @tc.steps: step1. init animation
     */
    auto rotationProperty = std::make_shared<RSAnimatableProperty<float>>(ANIMATION_START_VALUE);
    auto rotationModifier = std::make_shared<RSRotationModifier>(rotationProperty);
    canvasNode->AddModifier(rotationModifier);
    RSAnimationTimingCurve timingCurve = RSAnimationTimingCurve::CreateCubicCurve(0.178f, 0.523f, 0.58f, 1.0f);
    RSAnimationTimingProtocol protocol;
    auto curveAnimations = RSNode::Animate(protocol, timingCurve, [&rotationProperty]() {
        rotationProperty->Set(90.f);
    });
    /**
     * @tc.steps: step2. start animation test
     */
    ASSERT_TRUE(curveAnimations.size() == CORRECT_SIZE);
    ASSERT_TRUE(curveAnimations[FIRST_ANIMATION] != nullptr);
    EXPECT_TRUE(curveAnimations[FIRST_ANIMATION]->IsRunning());
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateCubicCurveTest008 end";
}

/**
 * @tc.name: CreateCubicCurveTest009
 * @tc.desc: Verify the CreateCubicCurve of animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTimingCurveTest, CreateCubicCurveTest009, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateCubicCurveTest009 start";
    /**
     * @tc.steps: step1. init animation
     */
    auto rotationProperty = std::make_shared<RSAnimatableProperty<float>>(ANIMATION_START_VALUE);
    auto rotationModifier = std::make_shared<RSRotationModifier>(rotationProperty);
    canvasNode->AddModifier(rotationModifier);
    RSAnimationTimingCurve timingCurve = RSAnimationTimingCurve::CreateCubicCurve(0.84f, 0.989f, 0.58f, 1.0f);
    RSAnimationTimingProtocol protocol;
    auto curveAnimations = RSNode::Animate(protocol, timingCurve, [&rotationProperty]() {
        rotationProperty->Set(90.f);
    });
    /**
     * @tc.steps: step2. start animation test
     */
    ASSERT_TRUE(curveAnimations.size() == CORRECT_SIZE);
    ASSERT_TRUE(curveAnimations[FIRST_ANIMATION] != nullptr);
    EXPECT_TRUE(curveAnimations[FIRST_ANIMATION]->IsRunning());
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateCubicCurveTest009 end";
}

/**
 * @tc.name: CreateCubicCurveTest010
 * @tc.desc: Verify the CreateCubicCurve of animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTimingCurveTest, CreateCubicCurveTest010, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateCubicCurveTest010 start";
    /**
     * @tc.steps: step1. init animation
     */
    auto rotationProperty = std::make_shared<RSAnimatableProperty<float>>(ANIMATION_START_VALUE);
    auto rotationModifier = std::make_shared<RSRotationModifier>(rotationProperty);
    canvasNode->AddModifier(rotationModifier);
    RSAnimationTimingCurve timingCurve = RSAnimationTimingCurve::CreateCubicCurve(0.45f, 0.12f, 0.58f, 1.0f);
    RSAnimationTimingProtocol protocol;
    auto curveAnimations = RSNode::Animate(protocol, timingCurve, [&rotationProperty]() {
        rotationProperty->Set(90.f);
    });
    /**
     * @tc.steps: step2. start animation test
     */
    ASSERT_TRUE(curveAnimations.size() == CORRECT_SIZE);
    ASSERT_TRUE(curveAnimations[FIRST_ANIMATION] != nullptr);
    EXPECT_TRUE(curveAnimations[FIRST_ANIMATION]->IsRunning());
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateCubicCurveTest010 end";
}

/**
 * @tc.name: CreateCubicCurveTest011
 * @tc.desc: Verify the CreateCubicCurve of animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTimingCurveTest, CreateCubicCurveTest011, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateCubicCurveTest011 start";
    /**
     * @tc.steps: step1. init animation
     */
    auto rotationProperty = std::make_shared<RSAnimatableProperty<float>>(ANIMATION_START_VALUE);
    auto rotationModifier = std::make_shared<RSRotationModifier>(rotationProperty);
    canvasNode->AddModifier(rotationModifier);
    RSAnimationTimingCurve timingCurve = RSAnimationTimingCurve::CreateCubicCurve(0.84f, 0.14f, 0.58f, 1.0f);
    RSAnimationTimingProtocol protocol;
    auto curveAnimations = RSNode::Animate(protocol, timingCurve, [&rotationProperty]() {
        rotationProperty->Set(90.f);
    });
    /**
     * @tc.steps: step2. start animation test
     */
    ASSERT_TRUE(curveAnimations.size() == CORRECT_SIZE);
    ASSERT_TRUE(curveAnimations[FIRST_ANIMATION] != nullptr);
    EXPECT_TRUE(curveAnimations[FIRST_ANIMATION]->IsRunning());
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateCubicCurveTest011 end";
}

/**
 * @tc.name: CreateCubicCurveTest012
 * @tc.desc: Verify the CreateCubicCurve of animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTimingCurveTest, CreateCubicCurveTest012, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateCubicCurveTest012 start";
    /**
     * @tc.steps: step1. init animation
     */
    auto rotationProperty = std::make_shared<RSAnimatableProperty<float>>(ANIMATION_START_VALUE);
    auto rotationModifier = std::make_shared<RSRotationModifier>(rotationProperty);
    canvasNode->AddModifier(rotationModifier);
    RSAnimationTimingCurve timingCurve = RSAnimationTimingCurve::CreateCubicCurve(0.15f, 0.23f, 0.58f, 1.0f);
    RSAnimationTimingProtocol protocol;
    auto curveAnimations = RSNode::Animate(protocol, timingCurve, [&rotationProperty]() {
        rotationProperty->Set(90.f);
    });
    /**
     * @tc.steps: step2. start animation test
     */
    ASSERT_TRUE(curveAnimations.size() == CORRECT_SIZE);
    ASSERT_TRUE(curveAnimations[FIRST_ANIMATION] != nullptr);
    EXPECT_TRUE(curveAnimations[FIRST_ANIMATION]->IsRunning());
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateCubicCurveTest012 end";
}

/**
 * @tc.name: CreateCubicCurveTest013
 * @tc.desc: Verify the CreateCubicCurve of animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTimingCurveTest, CreateCubicCurveTest013, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateCubicCurveTest013 start";
    /**
     * @tc.steps: step1. init animation
     */
    auto rotationProperty = std::make_shared<RSAnimatableProperty<float>>(ANIMATION_START_VALUE);
    auto rotationModifier = std::make_shared<RSRotationModifier>(rotationProperty);
    canvasNode->AddModifier(rotationModifier);
    RSAnimationTimingCurve timingCurve = RSAnimationTimingCurve::CreateCubicCurve(0.314f, 0.541f, 0.58f, 1.0f);
    RSAnimationTimingProtocol protocol;
    auto curveAnimations = RSNode::Animate(protocol, timingCurve, [&rotationProperty]() {
        rotationProperty->Set(90.f);
    });
    /**
     * @tc.steps: step2. start animation test
     */
    ASSERT_TRUE(curveAnimations.size() == CORRECT_SIZE);
    ASSERT_TRUE(curveAnimations[FIRST_ANIMATION] != nullptr);
    EXPECT_TRUE(curveAnimations[FIRST_ANIMATION]->IsRunning());
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateCubicCurveTest013 end";
}

/**
 * @tc.name: ConstCurveTest001
 * @tc.desc: Verify the ConstCurve of animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTimingCurveTest, ConstCurveTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest ConstCurveTest001 start";
    /**
     * @tc.steps: step1. init animation
     */
    auto rotationProperty = std::make_shared<RSAnimatableProperty<float>>(ANIMATION_START_VALUE);
    auto rotationModifier = std::make_shared<RSRotationModifier>(rotationProperty);
    canvasNode->AddModifier(rotationModifier);
    RSAnimationTimingCurve linearCurve = RSAnimationTimingCurve::LINEAR;
    RSAnimationTimingProtocol protocol;
    auto curveAnimations = RSNode::Animate(protocol, linearCurve, [&rotationProperty]() {
        rotationProperty->Set(90.f);
    });
    /**
     * @tc.steps: step2. start animation test
     */
    ASSERT_TRUE(curveAnimations.size() == CORRECT_SIZE);
    ASSERT_TRUE(curveAnimations[FIRST_ANIMATION]);
    EXPECT_TRUE(curveAnimations[FIRST_ANIMATION]->IsRunning());
    EXPECT_TRUE(linearCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest ConstCurveTest001 end";
}

/**
 * @tc.name: ConstCurveTest002
 * @tc.desc: Verify the ConstCurve of animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTimingCurveTest, ConstCurveTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest ConstCurveTest002 start";
    /**
     * @tc.steps: step1. init animation
     */
    auto rotationProperty = std::make_shared<RSAnimatableProperty<float>>(ANIMATION_START_VALUE);
    auto rotationModifier = std::make_shared<RSRotationModifier>(rotationProperty);
    canvasNode->AddModifier(rotationModifier);
    RSAnimationTimingCurve easeCurve = RSAnimationTimingCurve::EASE;
    RSAnimationTimingProtocol protocol;
    auto curveAnimations = RSNode::Animate(protocol, easeCurve, [&rotationProperty]() {
        rotationProperty->Set(90.f);
    });
    /**
     * @tc.steps: step2. start animation test
     */
    ASSERT_TRUE(curveAnimations.size() == CORRECT_SIZE);
    ASSERT_TRUE(curveAnimations[FIRST_ANIMATION]);
    EXPECT_TRUE(curveAnimations[FIRST_ANIMATION]->IsRunning());
    EXPECT_TRUE(easeCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest ConstCurveTest002 end";
}

/**
 * @tc.name: ConstCurveTest003
 * @tc.desc: Verify the ConstCurve of animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTimingCurveTest, ConstCurveTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest ConstCurveTest003 start";
    /**
     * @tc.steps: step1. init animation
     */
    auto rotationProperty = std::make_shared<RSAnimatableProperty<float>>(ANIMATION_START_VALUE);
    auto rotationModifier = std::make_shared<RSRotationModifier>(rotationProperty);
    canvasNode->AddModifier(rotationModifier);
    RSAnimationTimingCurve easeInCurve = RSAnimationTimingCurve::EASE_IN;
    RSAnimationTimingProtocol protocol;
    auto curveAnimations = RSNode::Animate(protocol, easeInCurve, [&rotationProperty]() {
        rotationProperty->Set(90.f);
    });
    /**
     * @tc.steps: step2. start animation test
     */
    ASSERT_TRUE(curveAnimations.size() == CORRECT_SIZE);
    ASSERT_TRUE(curveAnimations[FIRST_ANIMATION]);
    EXPECT_TRUE(curveAnimations[FIRST_ANIMATION]->IsRunning());
    EXPECT_TRUE(easeInCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest ConstCurveTest003 end";
}

/**
 * @tc.name: ConstCurveTest004
 * @tc.desc: Verify the ConstCurve of animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTimingCurveTest, ConstCurveTest004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest ConstCurveTest004 start";
    /**
     * @tc.steps: step1. init animation
     */
    auto rotationProperty = std::make_shared<RSAnimatableProperty<float>>(ANIMATION_START_VALUE);
    auto rotationModifier = std::make_shared<RSRotationModifier>(rotationProperty);
    canvasNode->AddModifier(rotationModifier);
    RSAnimationTimingCurve easeOutCurve = RSAnimationTimingCurve::EASE_OUT;
    RSAnimationTimingProtocol protocol;
    auto curveAnimations = RSNode::Animate(protocol, easeOutCurve, [&rotationProperty]() {
        rotationProperty->Set(90.f);
    });
    /**
     * @tc.steps: step2. start animation test
     */
    ASSERT_TRUE(curveAnimations.size() == CORRECT_SIZE);
    ASSERT_TRUE(curveAnimations[FIRST_ANIMATION]);
    EXPECT_TRUE(curveAnimations[FIRST_ANIMATION]->IsRunning());
    EXPECT_TRUE(easeOutCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest ConstCurveTest004 end";
}

/**
 * @tc.name: ConstCurveTest005
 * @tc.desc: Verify the ConstCurve of animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTimingCurveTest, ConstCurveTest005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest ConstCurveTest005 start";
    /**
     * @tc.steps: step1. init animation
     */
    auto rotationProperty = std::make_shared<RSAnimatableProperty<float>>(ANIMATION_START_VALUE);
    auto rotationModifier = std::make_shared<RSRotationModifier>(rotationProperty);
    canvasNode->AddModifier(rotationModifier);
    RSAnimationTimingCurve easeInOutCurve = RSAnimationTimingCurve::EASE_IN_OUT;
    RSAnimationTimingProtocol protocol;
    auto curveAnimations = RSNode::Animate(protocol, easeInOutCurve, [&rotationProperty]() {
        rotationProperty->Set(90.f);
    });
    /**
     * @tc.steps: step2. start animation test
     */
    ASSERT_TRUE(curveAnimations.size() == CORRECT_SIZE);
    ASSERT_TRUE(curveAnimations[FIRST_ANIMATION]);
    EXPECT_TRUE(curveAnimations[FIRST_ANIMATION]->IsRunning());
    EXPECT_TRUE(easeInOutCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest ConstCurveTest005 end";
}

/**
 * @tc.name: ConstCurveTest006
 * @tc.desc: Verify the ConstCurve of animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTimingCurveTest, ConstCurveTest006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest ConstCurveTest006 start";
    /**
     * @tc.steps: step1. init animation
     */
    auto rotationProperty = std::make_shared<RSAnimatableProperty<float>>(ANIMATION_START_VALUE);
    auto rotationModifier = std::make_shared<RSRotationModifier>(rotationProperty);
    canvasNode->AddModifier(rotationModifier);
    RSAnimationTimingCurve defaultCurve = RSAnimationTimingCurve::DEFAULT;
    RSAnimationTimingProtocol protocol;
    auto curveAnimations = RSNode::Animate(protocol, defaultCurve, [&rotationProperty]() {
        rotationProperty->Set(90.f);
    });
    /**
     * @tc.steps: step2. start animation test
     */
    ASSERT_TRUE(curveAnimations.size() == CORRECT_SIZE);
    ASSERT_TRUE(curveAnimations[FIRST_ANIMATION]);
    EXPECT_TRUE(curveAnimations[FIRST_ANIMATION]->IsRunning());
    EXPECT_TRUE(defaultCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest ConstCurveTest006 end";
}

/**
 * @tc.name: ConstCurveTest007
 * @tc.desc: Verify the ConstCurve of animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTimingCurveTest, ConstCurveTest007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest ConstCurveTest007 start";
    /**
     * @tc.steps: step1. init animation
     */
    auto rotationProperty = std::make_shared<RSAnimatableProperty<float>>(ANIMATION_START_VALUE);
    auto rotationModifier = std::make_shared<RSRotationModifier>(rotationProperty);
    canvasNode->AddModifier(rotationModifier);
    RSAnimationTimingCurve springCurve = RSAnimationTimingCurve::SPRING;
    RSAnimationTimingProtocol protocol;
    auto curveAnimations = RSNode::Animate(protocol, springCurve, [&rotationProperty]() {
        rotationProperty->Set(90.f);
    });
    /**
     * @tc.steps: step2. start animation test
     */
    ASSERT_TRUE(curveAnimations.size() == CORRECT_SIZE);
    ASSERT_TRUE(curveAnimations[FIRST_ANIMATION]);
    EXPECT_TRUE(curveAnimations[FIRST_ANIMATION]->IsRunning());
    EXPECT_TRUE(springCurve.type_ == RSAnimationTimingCurve::CurveType::SPRING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest ConstCurveTest007 end";
}

/**
 * @tc.name: ConstCurveTest008
 * @tc.desc: Verify the ConstCurve of animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTimingCurveTest, ConstCurveTest008, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest ConstCurveTest008 start";
    /**
     * @tc.steps: step1. init animation
     */
    auto rotationProperty = std::make_shared<RSAnimatableProperty<float>>(ANIMATION_START_VALUE);
    auto rotationModifier = std::make_shared<RSRotationModifier>(rotationProperty);
    canvasNode->AddModifier(rotationModifier);
    RSAnimationTimingCurve interactiveSpringCurve = RSAnimationTimingCurve::INTERACTIVE_SPRING;
    RSAnimationTimingProtocol protocol;
    auto curveAnimations = RSNode::Animate(protocol, interactiveSpringCurve, [&rotationProperty]() {
        rotationProperty->Set(90.f);
    });
    /**
     * @tc.steps: step2. start animation test
     */
    ASSERT_TRUE(curveAnimations.size() == CORRECT_SIZE);
    ASSERT_TRUE(curveAnimations[FIRST_ANIMATION]);
    EXPECT_TRUE(curveAnimations[FIRST_ANIMATION]->IsRunning());
    EXPECT_TRUE(interactiveSpringCurve.type_ == RSAnimationTimingCurve::CurveType::SPRING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest ConstCurveTest008 end";
}

/**
 * @tc.name: ConstCurveTest009
 * @tc.desc: Verify the ConstCurve of animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationTimingCurveTest, ConstCurveTest009, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest ConstCurveTest009 start";
    /**
     * @tc.steps: step1. init animation
     */
    auto rotationProperty = std::make_shared<RSAnimatableProperty<float>>(ANIMATION_START_VALUE);
    auto rotationModifier = std::make_shared<RSRotationModifier>(rotationProperty);
    canvasNode->AddModifier(rotationModifier);
    RSAnimationTimingCurve timeCurve = RSAnimationTimingCurve();
    RSAnimationTimingProtocol protocol;
    auto curveAnimations = RSNode::Animate(protocol, timeCurve, [&rotationProperty]() {
        rotationProperty->Set(90.f);
    });
    /**
     * @tc.steps: step2. start animation test
     */
    ASSERT_TRUE(curveAnimations.size() == CORRECT_SIZE);
    ASSERT_TRUE(curveAnimations[FIRST_ANIMATION]);
    EXPECT_TRUE(curveAnimations[FIRST_ANIMATION]->IsRunning());
    EXPECT_TRUE(timeCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest ConstCurveTest009 end";
}
#endif
} // namespace Rosen
} // namespace OHOS