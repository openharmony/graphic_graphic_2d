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
    EXPECT_TRUE(curveAnimations.size() == CORRECT_SIZE);
    if (curveAnimations.size() != CORRECT_SIZE) {
        return;
    }
    EXPECT_FALSE(curveAnimations[FIRST_ANIMATION] == nullptr);
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
    if (curveAnimations.size() != CORRECT_SIZE) {
        return;
    }
    EXPECT_FALSE(curveAnimations[FIRST_ANIMATION] == nullptr);
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
    if (curveAnimations.size() != CORRECT_SIZE) {
        return;
    }
    EXPECT_FALSE(curveAnimations[FIRST_ANIMATION] == nullptr);
    EXPECT_TRUE(curveAnimations[FIRST_ANIMATION]->IsRunning());
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::INTERPOLATING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateCustomCurveTest003 end";
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
    EXPECT_TRUE(curveAnimations.size() == CORRECT_SIZE);
    if (curveAnimations.size() != CORRECT_SIZE) {
        return;
    }
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
    EXPECT_TRUE(curveAnimations.size() == CORRECT_SIZE);
    if (curveAnimations.size() != CORRECT_SIZE) {
        return;
    }
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
    EXPECT_TRUE(curveAnimations.size() == CORRECT_SIZE);
    if (curveAnimations.size() != CORRECT_SIZE) {
        return;
    }
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
    EXPECT_TRUE(curveAnimations.size() == CORRECT_SIZE);
    if (curveAnimations.size() != CORRECT_SIZE) {
        return;
    }
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
    EXPECT_TRUE(curveAnimations.size() == CORRECT_SIZE);
    if (curveAnimations.size() != CORRECT_SIZE) {
        return;
    }
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
    EXPECT_TRUE(curveAnimations.size() == CORRECT_SIZE);
    if (curveAnimations.size() != CORRECT_SIZE) {
        return;
    }
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
    EXPECT_TRUE(curveAnimations.size() == CORRECT_SIZE);
    if (curveAnimations.size() != CORRECT_SIZE) {
        return;
    }
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
    EXPECT_TRUE(curveAnimations.size() == CORRECT_SIZE);
    if (curveAnimations.size() != CORRECT_SIZE) {
        return;
    }
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
    EXPECT_TRUE(curveAnimations.size() == CORRECT_SIZE);
    if (curveAnimations.size() != CORRECT_SIZE) {
        return;
    }
    EXPECT_FALSE(curveAnimations[FIRST_ANIMATION] == nullptr);
    EXPECT_TRUE(curveAnimations[FIRST_ANIMATION]->IsRunning());
    EXPECT_TRUE(timingCurve.type_ == RSAnimationTimingCurve::CurveType::SPRING);
    NotifyStartAnimation();
    GTEST_LOG_(INFO) << "RSAnimationTimingCurveTest CreateSpringTest002 end";
}

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
} // namespace Rosen
} // namespace OHOS