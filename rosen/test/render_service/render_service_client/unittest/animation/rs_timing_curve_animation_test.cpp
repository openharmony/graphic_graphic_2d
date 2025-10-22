/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include <sys/types.h>
#include <unistd.h>

#include "gtest/gtest.h"

#include "animation/rs_curve_animation.h"
#include "animation/rs_render_curve_animation.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSTimingCurveAnimationTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSTimingCurveAnimationTest::SetUpTestCase() {}
void RSTimingCurveAnimationTest::TearDownTestCase() {}
void RSTimingCurveAnimationTest::SetUp() {}
void RSTimingCurveAnimationTest::TearDown() {}

/**
 * @tc.name: StartAnimationTest
 * @tc.desc: test StartAnimation.
 * @tc.type: FUNC
 */
HWTEST_F(RSTimingCurveAnimationTest, StartAnimationTest, Level1)
{
    auto property = std::make_shared<RSProperty<float>>();
    auto value = std::make_shared<RSProperty<float>>();
    RSCurveAnimation curveAnimation(property, value);
    auto animation = std::make_shared<RSRenderCurveAnimation>();
    curveAnimation.StartInner(nullptr);
    curveAnimation.StartRenderAnimation(animation);
    ASSERT_NE(animation, nullptr);
}

#ifdef MODIFIER_NG_TEST
/**
 * @tc.name: CreateCustomCurveTest001
 * @tc.desc: Verify the CreateCustomCurve of animation
 * @tc.type: FUNC
 */
HWTEST_F(RSTimingCurveAnimationTest, CreateCustomCurveTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSTimingCurveAnimationTest CreateCustomCurveTest001 start";
    /**
     * @tc.steps: step1. init animation
     */
    auto property = std::make_shared<RSAnimatableProperty<float>>(ANIMATION_START_VALUE);
    auto modifier = std::make_shared<RSAlphaModifier>(property);
    canvasNode->AddModifier(modifier);
    auto timingCurve = RSAnimationTimingCurve::CreateCustomCurve([](float input) { return input; });
    RSAnimationTimingProtocol protocol;
    auto animations = RSNode::Animate(protocol, timingCurve, [&property]() { property->Set(1.f); });
    /**
     * @tc.steps: step2. start animation test
     */
    ASSERT_TRUE(animations.size() == CORRECT_SIZE);
    EXPECT_TRUE(animations[FIRST_ANIMATION]);
    EXPECT_TRUE(animations[FIRST_ANIMATION]->IsRunning());
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSTimingCurveAnimationTest CreateCustomCurveTest001 end";
}

/**
 * @tc.name: CreateCustomCurveTest002
 * @tc.desc: Verify the CreateCustomCurve of animation
 * @tc.type: FUNC
 */
HWTEST_F(RSTimingCurveAnimationTest, CreateCustomCurveTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSTimingCurveAnimationTest CreateCustomCurveTest002 start";
    /**
     * @tc.steps: step1. init animation
     */
    auto property = std::make_shared<RSAnimatableProperty<Vector2f>>(ANIMATION_NORMAL_SCALE);
    auto modifier = std::make_shared<RSScaleModifier>(property);
    canvasNode->AddModifier(modifier);
    auto timingCurve = RSAnimationTimingCurve::CreateCustomCurve([](float input) { return input * 2.f; });
    RSAnimationTimingProtocol protocol;
    auto animations = RSNode::Animate(protocol, timingCurve, [&property]() { property->Set(ANIMATION_DOUBLE_SCALE); });
    /**
     * @tc.steps: step2. start animation test
     */
    EXPECT_TRUE(animations.size() == CORRECT_SIZE);
    EXPECT_TRUE(animations[FIRST_ANIMATION]);
    EXPECT_TRUE(animations[FIRST_ANIMATION]->IsRunning());
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSTimingCurveAnimationTest CreateCustomCurveTest002 end";
}

/**
 * @tc.name: CreateCustomCurveTest003
 * @tc.desc: Verify the CreateCustomCurve of animation
 * @tc.type: FUNC
 */
HWTEST_F(RSTimingCurveAnimationTest, CreateCustomCurveTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSTimingCurveAnimationTest CreateCustomCurveTest003 start";
    /**
     * @tc.steps: step1. init animation
     */
    auto property = std::make_shared<RSAnimatableProperty<float>>(ANIMATION_START_VALUE);
    auto modifier = std::make_shared<RSRotationModifier>(property);
    canvasNode->AddModifier(modifier);
    auto timingCurve = RSAnimationTimingCurve::CreateCustomCurve([](float input) { return 0.f; });
    RSAnimationTimingProtocol protocol;
    auto animations = RSNode::Animate(protocol, timingCurve, [&property]() { property->Set(90.f); });
    /**
     * @tc.steps: step2. start animation test
     */
    EXPECT_TRUE(animations.size() == CORRECT_SIZE);
    EXPECT_TRUE(animations[FIRST_ANIMATION]);
    EXPECT_TRUE(animations[FIRST_ANIMATION]->IsRunning());
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSTimingCurveAnimationTest CreateCustomCurveTest003 end";
}

/**
 * @tc.name: CreateCubicCurveTest001
 * @tc.desc: Verify the CreateCubicCurve of animation
 * @tc.type: FUNC
 */
HWTEST_F(RSTimingCurveAnimationTest, CreateCubicCurveTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSTimingCurveAnimationTest CreateCubicCurveTest001 start";
    /**
     * @tc.steps: step1. init animation
     */
    auto property = std::make_shared<RSAnimatableProperty<float>>(ANIMATION_START_VALUE);
    auto alphaModifier = std::make_shared<RSAlphaModifier>(property);
    canvasNode->AddModifier(alphaModifier);
    RSAnimationTimingCurve timingCurve = RSAnimationTimingCurve::CreateCubicCurve(0.42f, 0.0f, 0.58f, 1.0f);
    RSAnimationTimingProtocol protocol;
    auto animations = RSNode::Animate(protocol, timingCurve, [&property]() { property->Set(1.f); });
    /**
     * @tc.steps: step2. start animation test
     */
    ASSERT_TRUE(animations.size() == CORRECT_SIZE);
    EXPECT_FALSE(animations[FIRST_ANIMATION] == nullptr);
    EXPECT_TRUE(animations[FIRST_ANIMATION]->IsRunning());
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSTimingCurveAnimationTest CreateCubicCurveTest001 end";
}

/**
 * @tc.name: CreateCubicCurveTest002
 * @tc.desc: Verify the CreateCubicCurve of animation
 * @tc.type: FUNC
 */
HWTEST_F(RSTimingCurveAnimationTest, CreateCubicCurveTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSTimingCurveAnimationTest CreateCubicCurveTest002 start";
    /**
     * @tc.steps: step1. init animation
     */
    auto property = std::make_shared<RSAnimatableProperty<Vector2f>>(ANIMATION_NORMAL_SCALE);
    auto scaleModifier = std::make_shared<RSScaleModifier>(property);
    canvasNode->AddModifier(scaleModifier);
    RSAnimationTimingCurve timingCurve = RSAnimationTimingCurve::CreateCubicCurve(0.25f, 0.1f, 0.25f, 1.0f);
    RSAnimationTimingProtocol protocol;
    auto animations = RSNode::Animate(protocol, timingCurve, [&property]() { property->Set(ANIMATION_DOUBLE_SCALE); });
    /**
     * @tc.steps: step2. start animation test
     */
    ASSERT_TRUE(animations.size() == CORRECT_SIZE);
    EXPECT_FALSE(animations[FIRST_ANIMATION] == nullptr);
    EXPECT_TRUE(animations[FIRST_ANIMATION]->IsRunning());
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSTimingCurveAnimationTest CreateCubicCurveTest002 end";
}

/**
 * @tc.name: CreateCubicCurveTest003
 * @tc.desc: Verify the CreateCubicCurve of animation
 * @tc.type: FUNC
 */
HWTEST_F(RSTimingCurveAnimationTest, CreateCubicCurveTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSTimingCurveAnimationTest CreateCubicCurveTest003 start";
    /**
     * @tc.steps: step1. init animation
     */
    auto property = std::make_shared<RSAnimatableProperty<float>>(ANIMATION_START_VALUE);
    auto rotationModifier = std::make_shared<RSRotationModifier>(property);
    canvasNode->AddModifier(rotationModifier);
    RSAnimationTimingCurve timingCurve = RSAnimationTimingCurve::CreateCubicCurve(0.0f, 0.0f, 0.58f, 1.0f);
    RSAnimationTimingProtocol protocol;
    auto animations = RSNode::Animate(protocol, timingCurve, [&property]() { property->Set(90.f); });
    /**
     * @tc.steps: step2. start animation test
     */
    ASSERT_TRUE(animations.size() == CORRECT_SIZE);
    EXPECT_FALSE(animations[FIRST_ANIMATION] == nullptr);
    EXPECT_TRUE(animations[FIRST_ANIMATION]->IsRunning());
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSTimingCurveAnimationTest CreateCubicCurveTest003 end";
}

/**
 * @tc.name: CreateStepsCurveTest001
 * @tc.desc: Verify the CreateStepsCurve of animation
 * @tc.type: FUNC
 */
HWTEST_F(RSTimingCurveAnimationTest, CreateStepsCurveTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSTimingCurveAnimationTest CreateStepsCurveTest001 start";
    /**
     * @tc.steps: step1. init animation
     */
    auto property = std::make_shared<RSAnimatableProperty<Vector2f>>(ANIMATION_NORMAL_SCALE);
    auto scaleModifier = std::make_shared<RSScaleModifier>(property);
    canvasNode->AddModifier(scaleModifier);
    auto timingCurve = RSAnimationTimingCurve::CreateStepsCurve(5, StepsCurvePosition::START);
    RSAnimationTimingProtocol protocol;
    auto animations = RSNode::Animate(protocol, timingCurve, [&property]() { property->Set(ANIMATION_DOUBLE_SCALE); });
    /**
     * @tc.steps: step2. start animation test
     */
    ASSERT_TRUE(animations.size() == CORRECT_SIZE);
    EXPECT_FALSE(animations[FIRST_ANIMATION] == nullptr);
    EXPECT_TRUE(animations[FIRST_ANIMATION]->IsRunning());
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSTimingCurveAnimationTest CreateStepsCurveTest001 end";
}

/**
 * @tc.name: CreateStepsCurveTest002
 * @tc.desc: Verify the CreateStepsCurve of animation
 * @tc.type: FUNC
 */
HWTEST_F(RSTimingCurveAnimationTest, CreateStepsCurveTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSTimingCurveAnimationTest CreateStepsCurveTest002 start";
    /**
     * @tc.steps: step1. init animation
     */
    auto property = std::make_shared<RSAnimatableProperty<float>>(ANIMATION_START_VALUE);
    auto rotationModifier = std::make_shared<RSRotationModifier>(property);
    canvasNode->AddModifier(rotationModifier);
    auto timingCurve = RSAnimationTimingCurve::CreateStepsCurve(5, StepsCurvePosition::END);
    RSAnimationTimingProtocol protocol;
    auto animations = RSNode::Animate(protocol, timingCurve, [&property]() { property->Set(90.f); });
    /**
     * @tc.steps: step2. start animation test
     */
    ASSERT_TRUE(animations.size() == CORRECT_SIZE);
    EXPECT_FALSE(animations[FIRST_ANIMATION] == nullptr);
    EXPECT_TRUE(animations[FIRST_ANIMATION]->IsRunning());
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSTimingCurveAnimationTest CreateStepsCurveTest002 end";
}

/**
 * @tc.name: CreateSpringCurveTest001
 * @tc.desc: Verify the CreateSpringCurve of animation
 * @tc.type: FUNC
 */
HWTEST_F(RSTimingCurveAnimationTest, CreateSpringCurveTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSTimingCurveAnimationTest CreateSpringCurveTest001 start";
    /**
     * @tc.steps: step1. init animation
     */
    auto property = std::make_shared<RSAnimatableProperty<Vector2f>>(ANIMATION_NORMAL_SCALE);
    auto scaleModifier = std::make_shared<RSScaleModifier>(property);
    canvasNode->AddModifier(scaleModifier);
    auto timingCurve = RSAnimationTimingCurve::CreateSpringCurve(0.1f, 0.2f, 0.3f, 0.4f);
    RSAnimationTimingProtocol protocol;
    auto animations = RSNode::Animate(protocol, timingCurve, [&property]() { property->Set(ANIMATION_DOUBLE_SCALE); });
    /**
     * @tc.steps: step2. start animation test
     */
    ASSERT_TRUE(animations.size() == CORRECT_SIZE);
    EXPECT_FALSE(animations[FIRST_ANIMATION] == nullptr);
    EXPECT_TRUE(animations[FIRST_ANIMATION]->IsRunning());
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSTimingCurveAnimationTest CreateSpringCurveTest001 end";
}

/**
 * @tc.name: CreateSpringCurveTest002
 * @tc.desc: Verify the CreateSpringCurve of animation
 * @tc.type: FUNC
 */
HWTEST_F(RSTimingCurveAnimationTest, CreateSpringCurveTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSTimingCurveAnimationTest CreateSpringCurveTest002 start";
    /**
     * @tc.steps: step1. init animation
     */
    auto property = std::make_shared<RSAnimatableProperty<Vector2f>>(ANIMATION_NORMAL_SCALE);
    auto scaleModifier = std::make_shared<RSScaleModifier>(property);
    canvasNode->AddModifier(scaleModifier);
    auto timingCurve = RSAnimationTimingCurve::CreateSpringCurve(0.4f, 0.3f, 0.2f, 0.1f);
    RSAnimationTimingProtocol protocol;
    auto animations = RSNode::Animate(protocol, timingCurve, [&property]() { property->Set(ANIMATION_DOUBLE_SCALE); });
    /**
     * @tc.steps: step2. start animation test
     */
    ASSERT_TRUE(animations.size() == CORRECT_SIZE);
    EXPECT_FALSE(animations[FIRST_ANIMATION] == nullptr);
    EXPECT_TRUE(animations[FIRST_ANIMATION]->IsRunning());
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSTimingCurveAnimationTest CreateSpringCurveTest002 end";
}

/**
 * @tc.name: CreateSpringTest001
 * @tc.desc: Verify the CreateSpring of animation
 * @tc.type: FUNC
 */
HWTEST_F(RSTimingCurveAnimationTest, CreateSpringTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSTimingCurveAnimationTest CreateSpringTest001 start";
    /**
     * @tc.steps: step1. init animation
     */
    auto property = std::make_shared<RSAnimatableProperty<float>>(ANIMATION_START_VALUE);
    auto rotationModifier = std::make_shared<RSRotationModifier>(property);
    canvasNode->AddModifier(rotationModifier);
    auto timingCurve = RSAnimationTimingCurve::CreateSpring(0.1f, 0.5f);
    RSAnimationTimingProtocol protocol;
    auto animations = RSNode::Animate(protocol, timingCurve, [&property]() { property->Set(90.f); });
    /**
     * @tc.steps: step2. start animation test
     */
    ASSERT_TRUE(animations.size() == CORRECT_SIZE);
    EXPECT_FALSE(animations[FIRST_ANIMATION] == nullptr);
    EXPECT_TRUE(animations[FIRST_ANIMATION]->IsRunning());
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::SPRING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSTimingCurveAnimationTest CreateSpringTest001 end";
}

/**
 * @tc.name: CreateSpringTest002
 * @tc.desc: Verify the CreateCustomCurve of animation
 * @tc.type: FUNC
 */
HWTEST_F(RSTimingCurveAnimationTest, CreateSpringTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSTimingCurveAnimationTest CreateSpringTest002 start";
    /**
     * @tc.steps: step1. init animation
     */
    auto property = std::make_shared<RSAnimatableProperty<float>>(ANIMATION_START_VALUE);
    auto rotationModifier = std::make_shared<RSRotationModifier>(property);
    canvasNode->AddModifier(rotationModifier);
    auto timingCurve = RSAnimationTimingCurve::CreateSpring(0.5f, 1.0f, 0.1f);
    RSAnimationTimingProtocol protocol;
    auto animations = RSNode::Animate(protocol, timingCurve, [&property]() { property->Set(90.f); });
    /**
     * @tc.steps: step2. start animation test
     */
    ASSERT_TRUE(animations.size() == CORRECT_SIZE);
    EXPECT_FALSE(animations[FIRST_ANIMATION] == nullptr);
    EXPECT_TRUE(animations[FIRST_ANIMATION]->IsRunning());
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::SPRING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSTimingCurveAnimationTest CreateSpringTest002 end";
}

/**
 * @tc.name: CreateSpringCurve001
 * @tc.desc: Verify the CreateSpringCurve of animation
 * @tc.type: FUNC
 */
HWTEST_F(RSTimingCurveAnimationTest, CreateSpringCurve001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSTimingCurveAnimationTest CreateSpringTest001 start";
    float velocity = 1.f;
    float mass = 1.f;
    float stiffness = 0.f;
    float damping = 1.f;
    auto timingCurve = RSAnimationTimingCurve::CreateSpringCurve(velocity, mass, stiffness, damping);
    auto timingCurve_ptr = std::make_shared<RSAnimationTimingCurve>(timingCurve);
    EXPECT_TRUE(timingCurve_ptr != nullptr);

    float mass1 = 0.f;
    float stiffness1 = 1.f;
    auto timingCurve1 = RSAnimationTimingCurve::CreateSpringCurve(velocity, mass1, stiffness1, damping);
    auto timingCurve_ptr1 = std::make_shared<RSAnimationTimingCurve>(timingCurve);
    EXPECT_TRUE(timingCurve_ptr1 != nullptr);
    GTEST_LOG_(INFO) << "RSTimingCurveAnimationTest CreateSpringCurve001 end";
}

/**
 * @tc.name: CreateInterpolatingSpring001
 * @tc.desc: Verify the CreateInterpolatingSpring of animation
 * @tc.type: FUNC
 */
HWTEST_F(RSTimingCurveAnimationTest, CreateInterpolatingSpring001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSTimingCurveAnimationTest CreateInterpolatingSpring001 start";
    float mass = 1.f;
    float stiffness = 0.0f;
    float damping = 1.f;
    float velocity = 0.f;
    auto timingCurve = RSAnimationTimingCurve::CreateInterpolatingSpring(mass, stiffness, damping, velocity);
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING_SPRING);

    float mass1 = 1.f;
    float stiffness1 = 1.f;
    auto timingCurve1 = RSAnimationTimingCurve::CreateInterpolatingSpring(mass1, stiffness1, damping, velocity);
    EXPECT_TRUE(timingCurve1.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING_SPRING);

    float mass2 = -1.0f;
    float stiffness2 = 0.f;
    auto timingCurve2 = RSAnimationTimingCurve::CreateInterpolatingSpring(mass2, stiffness2, damping, velocity);
    EXPECT_TRUE(timingCurve2.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING_SPRING);
    GTEST_LOG_(INFO) << "RSTimingCurveAnimationTest CreateInterpolatingSpring001 end";
}

/**
 * @tc.name: CreateCubicCurveTest004
 * @tc.desc: Verify the CreateCubicCurve of animation
 * @tc.type: FUNC
 */
HWTEST_F(RSTimingCurveAnimationTest, CreateCubicCurveTest004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSTimingCurveAnimationTest CreateCubicCurveTest004 start";
    /**
     * @tc.steps: step1. init animation
     */
    auto property = std::make_shared<RSAnimatableProperty<float>>(ANIMATION_START_VALUE);
    auto alphaModifier = std::make_shared<RSAlphaModifier>(property);
    canvasNode->AddModifier(alphaModifier);
    RSAnimationTimingCurve timingCurve = RSAnimationTimingCurve::CreateCubicCurve(0.9f, 0.1f, 0.56f, 1.0f);
    RSAnimationTimingProtocol protocol;
    auto animations = RSNode::Animate(protocol, timingCurve, [&property]() { property->Set(0.6f); });
    /**
     * @tc.steps: step2. start animation test
     */
    ASSERT_TRUE(animations.size() == CORRECT_SIZE);
    ASSERT_TRUE(animations[FIRST_ANIMATION] != nullptr);
    EXPECT_TRUE(animations[FIRST_ANIMATION]->IsRunning());
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSTimingCurveAnimationTest CreateCubicCurveTest004 end";
}

/**
 * @tc.name: CreateCubicCurveTest005
 * @tc.desc: Verify the CreateCubicCurve of animation
 * @tc.type: FUNC
 */
HWTEST_F(RSTimingCurveAnimationTest, CreateCubicCurveTest005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSTimingCurveAnimationTest CreateCubicCurveTest005 start";
    /**
     * @tc.steps: step1. init animation
     */
    auto property = std::make_shared<RSAnimatableProperty<Vector2f>>(ANIMATION_NORMAL_SCALE);
    auto scaleModifier = std::make_shared<RSScaleModifier>(property);
    canvasNode->AddModifier(scaleModifier);
    RSAnimationTimingCurve timingCurve = RSAnimationTimingCurve::CreateCubicCurve(0.234f, 0.12f, 0.25f, 1.0f);
    RSAnimationTimingProtocol protocol;
    auto animations = RSNode::Animate(protocol, timingCurve, [&property]() { property->Set(ANIMATION_DOUBLE_SCALE); });
    /**
     * @tc.steps: step2. start animation test
     */
    ASSERT_TRUE(animations.size() == CORRECT_SIZE);
    ASSERT_TRUE(animations[FIRST_ANIMATION] != nullptr);
    EXPECT_TRUE(animations[FIRST_ANIMATION]->IsRunning());
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSTimingCurveAnimationTest CreateCubicCurveTest005 end";
}

/**
 * @tc.name: CreateCubicCurveTest006
 * @tc.desc: Verify the CreateCubicCurve of animation
 * @tc.type: FUNC
 */
HWTEST_F(RSTimingCurveAnimationTest, CreateCubicCurveTest006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSTimingCurveAnimationTest CreateCubicCurveTest006 start";
    /**
     * @tc.steps: step1. init animation
     */
    auto property = std::make_shared<RSAnimatableProperty<float>>(ANIMATION_START_VALUE);
    auto rotationModifier = std::make_shared<RSRotationModifier>(property);
    canvasNode->AddModifier(rotationModifier);
    RSAnimationTimingCurve timingCurve = RSAnimationTimingCurve::CreateCubicCurve(0.33f, 0.5f, 0.58f, 1.0f);
    RSAnimationTimingProtocol protocol;
    auto animations = RSNode::Animate(protocol, timingCurve, [&property]() { property->Set(90.f); });
    /**
     * @tc.steps: step2. start animation test
     */
    ASSERT_TRUE(animations.size() == CORRECT_SIZE);
    ASSERT_TRUE(animations[FIRST_ANIMATION] != nullptr);
    EXPECT_TRUE(animations[FIRST_ANIMATION]->IsRunning());
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSTimingCurveAnimationTest CreateCubicCurveTest006 end";
}

/**
 * @tc.name: CreateCubicCurveTest007
 * @tc.desc: Verify the CreateCubicCurve of animation
 * @tc.type: FUNC
 */
HWTEST_F(RSTimingCurveAnimationTest, CreateCubicCurveTest007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSTimingCurveAnimationTest CreateCubicCurveTest007 start";
    /**
     * @tc.steps: step1. init animation
     */
    auto property = std::make_shared<RSAnimatableProperty<float>>(ANIMATION_START_VALUE);
    auto rotationModifier = std::make_shared<RSRotationModifier>(property);
    canvasNode->AddModifier(rotationModifier);
    RSAnimationTimingCurve timingCurve = RSAnimationTimingCurve::CreateCubicCurve(0.33f, 0.4f, 0.58f, 1.0f);
    RSAnimationTimingProtocol protocol;
    auto animations = RSNode::Animate(protocol, timingCurve, [&property]() { property->Set(90.f); });
    /**
     * @tc.steps: step2. start animation test
     */
    ASSERT_TRUE(animations.size() == CORRECT_SIZE);
    ASSERT_TRUE(animations[FIRST_ANIMATION] != nullptr);
    EXPECT_TRUE(animations[FIRST_ANIMATION]->IsRunning());
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSTimingCurveAnimationTest CreateCubicCurveTest007 end";
}

/**
 * @tc.name: CreateCubicCurveTest008
 * @tc.desc: Verify the CreateCubicCurve of animation
 * @tc.type: FUNC
 */
HWTEST_F(RSTimingCurveAnimationTest, CreateCubicCurveTest008, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSTimingCurveAnimationTest CreateCubicCurveTest008 start";
    /**
     * @tc.steps: step1. init animation
     */
    auto property = std::make_shared<RSAnimatableProperty<float>>(ANIMATION_START_VALUE);
    auto rotationModifier = std::make_shared<RSRotationModifier>(property);
    canvasNode->AddModifier(rotationModifier);
    RSAnimationTimingCurve timingCurve = RSAnimationTimingCurve::CreateCubicCurve(0.178f, 0.523f, 0.58f, 1.0f);
    RSAnimationTimingProtocol protocol;
    auto animations = RSNode::Animate(protocol, timingCurve, [&property]() { property->Set(90.f); });
    /**
     * @tc.steps: step2. start animation test
     */
    ASSERT_TRUE(animations.size() == CORRECT_SIZE);
    ASSERT_TRUE(animations[FIRST_ANIMATION] != nullptr);
    EXPECT_TRUE(animations[FIRST_ANIMATION]->IsRunning());
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSTimingCurveAnimationTest CreateCubicCurveTest008 end";
}

/**
 * @tc.name: CreateCubicCurveTest009
 * @tc.desc: Verify the CreateCubicCurve of animation
 * @tc.type: FUNC
 */
HWTEST_F(RSTimingCurveAnimationTest, CreateCubicCurveTest009, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSTimingCurveAnimationTest CreateCubicCurveTest009 start";
    /**
     * @tc.steps: step1. init animation
     */
    auto property = std::make_shared<RSAnimatableProperty<float>>(ANIMATION_START_VALUE);
    auto rotationModifier = std::make_shared<RSRotationModifier>(property);
    canvasNode->AddModifier(rotationModifier);
    RSAnimationTimingCurve timingCurve = RSAnimationTimingCurve::CreateCubicCurve(0.84f, 0.989f, 0.58f, 1.0f);
    RSAnimationTimingProtocol protocol;
    auto animations = RSNode::Animate(protocol, timingCurve, [&property]() { property->Set(90.f); });
    /**
     * @tc.steps: step2. start animation test
     */
    ASSERT_TRUE(animations.size() == CORRECT_SIZE);
    ASSERT_TRUE(animations[FIRST_ANIMATION] != nullptr);
    EXPECT_TRUE(animations[FIRST_ANIMATION]->IsRunning());
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSTimingCurveAnimationTest CreateCubicCurveTest009 end";
}

/**
 * @tc.name: CreateCubicCurveTest010
 * @tc.desc: Verify the CreateCubicCurve of animation
 * @tc.type: FUNC
 */
HWTEST_F(RSTimingCurveAnimationTest, CreateCubicCurveTest010, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSTimingCurveAnimationTest CreateCubicCurveTest010 start";
    /**
     * @tc.steps: step1. init animation
     */
    auto property = std::make_shared<RSAnimatableProperty<float>>(ANIMATION_START_VALUE);
    auto rotationModifier = std::make_shared<RSRotationModifier>(property);
    canvasNode->AddModifier(rotationModifier);
    RSAnimationTimingCurve timingCurve = RSAnimationTimingCurve::CreateCubicCurve(0.45f, 0.12f, 0.58f, 1.0f);
    RSAnimationTimingProtocol protocol;
    auto animations = RSNode::Animate(protocol, timingCurve, [&property]() { property->Set(90.f); });
    /**
     * @tc.steps: step2. start animation test
     */
    ASSERT_TRUE(animations.size() == CORRECT_SIZE);
    ASSERT_TRUE(animations[FIRST_ANIMATION] != nullptr);
    EXPECT_TRUE(animations[FIRST_ANIMATION]->IsRunning());
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSTimingCurveAnimationTest CreateCubicCurveTest010 end";
}

/**
 * @tc.name: CreateCubicCurveTest011
 * @tc.desc: Verify the CreateCubicCurve of animation
 * @tc.type: FUNC
 */
HWTEST_F(RSTimingCurveAnimationTest, CreateCubicCurveTest011, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSTimingCurveAnimationTest CreateCubicCurveTest011 start";
    /**
     * @tc.steps: step1. init animation
     */
    auto property = std::make_shared<RSAnimatableProperty<float>>(ANIMATION_START_VALUE);
    auto rotationModifier = std::make_shared<RSRotationModifier>(property);
    canvasNode->AddModifier(rotationModifier);
    RSAnimationTimingCurve timingCurve = RSAnimationTimingCurve::CreateCubicCurve(0.84f, 0.14f, 0.58f, 1.0f);
    RSAnimationTimingProtocol protocol;
    auto animations = RSNode::Animate(protocol, timingCurve, [&property]() { property->Set(90.f); });
    /**
     * @tc.steps: step2. start animation test
     */
    ASSERT_TRUE(animations.size() == CORRECT_SIZE);
    ASSERT_TRUE(animations[FIRST_ANIMATION] != nullptr);
    EXPECT_TRUE(animations[FIRST_ANIMATION]->IsRunning());
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSTimingCurveAnimationTest CreateCubicCurveTest011 end";
}

/**
 * @tc.name: CreateCubicCurveTest012
 * @tc.desc: Verify the CreateCubicCurve of animation
 * @tc.type: FUNC
 */
HWTEST_F(RSTimingCurveAnimationTest, CreateCubicCurveTest012, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSTimingCurveAnimationTest CreateCubicCurveTest012 start";
    /**
     * @tc.steps: step1. init animation
     */
    auto property = std::make_shared<RSAnimatableProperty<float>>(ANIMATION_START_VALUE);
    auto rotationModifier = std::make_shared<RSRotationModifier>(property);
    canvasNode->AddModifier(rotationModifier);
    RSAnimationTimingCurve timingCurve = RSAnimationTimingCurve::CreateCubicCurve(0.15f, 0.23f, 0.58f, 1.0f);
    RSAnimationTimingProtocol protocol;
    auto animations = RSNode::Animate(protocol, timingCurve, [&property]() { property->Set(90.f); });
    /**
     * @tc.steps: step2. start animation test
     */
    ASSERT_TRUE(animations.size() == CORRECT_SIZE);
    ASSERT_TRUE(animations[FIRST_ANIMATION] != nullptr);
    EXPECT_TRUE(animations[FIRST_ANIMATION]->IsRunning());
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSTimingCurveAnimationTest CreateCubicCurveTest012 end";
}

/**
 * @tc.name: CreateCubicCurveTest013
 * @tc.desc: Verify the CreateCubicCurve of animation
 * @tc.type: FUNC
 */
HWTEST_F(RSTimingCurveAnimationTest, CreateCubicCurveTest013, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSTimingCurveAnimationTest CreateCubicCurveTest013 start";
    /**
     * @tc.steps: step1. init animation
     */
    auto property = std::make_shared<RSAnimatableProperty<float>>(ANIMATION_START_VALUE);
    auto rotationModifier = std::make_shared<RSRotationModifier>(property);
    canvasNode->AddModifier(rotationModifier);
    RSAnimationTimingCurve timingCurve = RSAnimationTimingCurve::CreateCubicCurve(0.314f, 0.541f, 0.58f, 1.0f);
    RSAnimationTimingProtocol protocol;
    auto animations = RSNode::Animate(protocol, timingCurve, [&property]() { property->Set(90.f); });
    /**
     * @tc.steps: step2. start animation test
     */
    ASSERT_TRUE(animations.size() == CORRECT_SIZE);
    ASSERT_TRUE(animations[FIRST_ANIMATION] != nullptr);
    EXPECT_TRUE(animations[FIRST_ANIMATION]->IsRunning());
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSTimingCurveAnimationTest CreateCubicCurveTest013 end";
}
#endif
} // namespace Rosen
} // namespace OHOS