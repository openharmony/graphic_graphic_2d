/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "anim_custom_modifier_test.h"
#include "rs_graphic_test.h"
#include "rs_graphic_test_director.h"
#include "animation/rs_curve_animation.h"
#include "animation/rs_steps_interpolator.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

// ============================================================================
// Test Fixture for Custom Curve Tests
// ============================================================================

class CustomCurveTest : public RSGraphicTest {
public:
    void BeforeEach() override
    {
        SetScreenSize(screenWidth, screenHeight);
    }

protected:
    const int screenWidth = 1200;
    const int screenHeight = 2000;

    // Helper function to create and register a test node with modifier
    std::shared_ptr<RSCanvasNode> CreateTestNode(std::shared_ptr<Modifier> modifier)
    {
        auto testNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
        testNode->SetBounds({ 0, 0, screenWidth, screenHeight });
        testNode->AddModifier(modifier);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);
        return testNode;
    }

    // Helper function to create and start a basic curve animation
    // Uses RSNode::Animate to ensure proper property association with modifier
    std::shared_ptr<RSAnimation> CreateCurveAnimation(
        std::shared_ptr<AnimationCustomModifier> modifier,
        const RSAnimationTimingCurve& curve)
    {
        RSAnimationTimingProtocol protocol;
        protocol.SetDuration(1000);

        auto animations = RSNode::Animate(RSGraphicTestDirector::Instance().GetRSUIContext(), protocol, curve, [&]() {
            modifier->SetPosition(1000.0f);
        });

        if (!animations.empty()) {
            return animations[0];
        }
        return nullptr;
    }
};

// ============================================================================
// Section 1: Built-in Curve Constants Tests (内置曲线常量测试)
// ============================================================================

/*
 * @tc.name: BuiltInCurve_SHARP_NORMAL
 * @tc.desc: Test built-in SHARP curve
 * @tc.type: FUNC
 */
GRAPHIC_TEST(CustomCurveTest, ANIMATION_TEST, BuiltInCurve_SHARP_NORMAL)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier, RSAnimationTimingCurve::SHARP);
}

/*
 * @tc.name: BuiltInCurve_SHARP_WithRepeat
 * @tc.desc: Test SHARP curve with repeat
 * @tc.type: FUNC
 */
GRAPHIC_TEST(CustomCurveTest, ANIMATION_TEST, BuiltInCurve_SHARP_WithRepeat)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(500);
    protocol.SetRepeatCount(2);

    auto timingCurve = RSAnimationTimingCurve::SHARP;
    RSNode::Animate(RSGraphicTestDirector::Instance().GetRSUIContext(), protocol, timingCurve, [&]() {
        animModifier->SetPosition(1000.0f);
    });
}

/*
 * @tc.name: BuiltInCurve_EASE_IN_NORMAL
 * @tc.desc: Test built-in EASE_IN curve
 * @tc.type: FUNC
 */
GRAPHIC_TEST(CustomCurveTest, ANIMATION_TEST, BuiltInCurve_EASE_IN_NORMAL)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier, RSAnimationTimingCurve::EASE_IN);
}

/*
 * @tc.name: BuiltInCurve_EASE_IN_WithDuration
 * @tc.desc: Test EASE_IN curve with different duration
 * @tc.type: FUNC
 */
GRAPHIC_TEST(CustomCurveTest, ANIMATION_TEST, BuiltInCurve_EASE_IN_WithDuration)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(2000);

    auto timingCurve = RSAnimationTimingCurve::EASE_IN;
    RSNode::Animate(RSGraphicTestDirector::Instance().GetRSUIContext(), protocol, timingCurve, [&]() {
        animModifier->SetPosition(1000.0f);
    });
}

/*
 * @tc.name: BuiltInCurve_EASE_OUT_NORMAL
 * @tc.desc: Test built-in EASE_OUT curve
 * @tc.type: FUNC
 */
GRAPHIC_TEST(CustomCurveTest, ANIMATION_TEST, BuiltInCurve_EASE_OUT_NORMAL)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier, RSAnimationTimingCurve::EASE_OUT);
}

/*
 * @tc.name: BuiltInCurve_EASE_OUT_WithAutoReverse
 * @tc.desc: Test EASE_OUT curve with auto reverse
 * @tc.type: FUNC
 */
GRAPHIC_TEST(CustomCurveTest, ANIMATION_TEST, BuiltInCurve_EASE_OUT_WithAutoReverse)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(800);
    protocol.SetRepeatCount(2);
    protocol.SetAutoReverse(true);

    auto timingCurve = RSAnimationTimingCurve::EASE_OUT;
    RSNode::Animate(RSGraphicTestDirector::Instance().GetRSUIContext(), protocol, timingCurve, [&]() {
        animModifier->SetPosition(1000.0f);
    });
}

/*
 * @tc.name: BuiltInCurve_ALL_Curves_Matrix
 * @tc.desc: Test all built-in curves in matrix layout (3x3)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(CustomCurveTest, ANIMATION_TEST, BuiltInCurve_ALL_Curves_Matrix)
{
    // 创建3x3矩阵布局的动画节点，测试所有内置曲线
    const float startX = 100.0f;
    const float startY = 100.0f;
    const float spacingX = 350.0f;
    const float spacingY = 350.0f;

    // 第一行：LINEAR, EASE, SHARP
    for (int col = 0; col < 3; col++) {
        auto animModifier = std::make_shared<AnimationCustomModifier>();
        auto testNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
        testNode->SetBounds({ 0, 0, screenWidth, screenHeight });
        testNode->AddModifier(animModifier);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);

        float posX = startX + col * spacingX;
        float posY = startY;
        animModifier->SetPosition(static_cast<int>(posX * 1000 + posY));
        animModifier->SetTimeInterval(5.0f);

        RSAnimationTimingProtocol protocol;
        protocol.SetDuration(1000);

        RSAnimationTimingCurve curve;
        if (col == 0) curve = RSAnimationTimingCurve::LINEAR;
        else if (col == 1) curve = RSAnimationTimingCurve::EASE;
        else curve = RSAnimationTimingCurve::SHARP;

        RSNode::Animate(RSGraphicTestDirector::Instance().GetRSUIContext(), protocol, curve, [&]() {
            animModifier->SetPosition(posX + 200.0f);
        });
    }

    // 第二行：EASE_IN, EASE_OUT, EASE_IN_OUT
    for (int col = 0; col < 3; col++) {
        auto animModifier = std::make_shared<AnimationCustomModifier>();
        auto testNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
        testNode->SetBounds({ 0, 0, screenWidth, screenHeight });
        testNode->AddModifier(animModifier);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);

        float posX = startX + col * spacingX;
        float posY = startY + spacingY;
        animModifier->SetPosition(static_cast<int>(posX * 1000 + posY));
        animModifier->SetTimeInterval(5.0f);

        RSAnimationTimingProtocol protocol;
        protocol.SetDuration(1000);

        RSAnimationTimingCurve curve;
        if (col == 0) curve = RSAnimationTimingCurve::EASE_IN;
        else if (col == 1) curve = RSAnimationTimingCurve::EASE_OUT;
        else curve = RSAnimationTimingCurve::EASE_IN_OUT;

        RSNode::Animate(RSGraphicTestDirector::Instance().GetRSUIContext(), protocol, curve, [&]() {
            animModifier->SetPosition(posX + 200.0f);
        });
    }

    // 第三行：SPRING, INTERACTIVE_SPRING, DEFAULT
    for (int col = 0; col < 3; col++) {
        auto animModifier = std::make_shared<AnimationCustomModifier>();
        auto testNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
        testNode->SetBounds({ 0, 0, screenWidth, screenHeight });
        testNode->AddModifier(animModifier);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);

        float posX = startX + col * spacingX;
        float posY = startY + 2.0f * spacingY;
        animModifier->SetPosition(static_cast<int>(posX * 1000 + posY));
        animModifier->SetTimeInterval(5.0f);

        RSAnimationTimingProtocol protocol;
        protocol.SetDuration(1000);

        RSAnimationTimingCurve curve;
        if (col == 0) curve = RSAnimationTimingCurve::SPRING;
        else if (col == 1) curve = RSAnimationTimingCurve::INTERACTIVE_SPRING;
        else curve = RSAnimationTimingCurve::DEFAULT;

        RSNode::Animate(RSGraphicTestDirector::Instance().GetRSUIContext(), protocol, curve, [&]() {
            animModifier->SetPosition(posX + 200.0f);
        });
    }
}

// ============================================================================
// Section 2: CreateCubicCurve Tests (贝塞尔曲线测试)
// ============================================================================

/*
 * @tc.name: CreateCubicCurve_NORMAL_Ease
 * @tc.desc: Test CreateCubicCurve with ease curve (0.25, 0.1, 0.25, 1.0)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(CustomCurveTest, ANIMATION_TEST, CreateCubicCurve_NORMAL_Ease)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto curve = RSAnimationTimingCurve::CreateCubicCurve(0.25f, 0.1f, 0.25f, 1.0f);
    auto animation = CreateCurveAnimation(animModifier, curve);
}

/*
 * @tc.name: CreateCubicCurve_NORMAL_EaseIn
 * @tc.desc: Test CreateCubicCurve with ease-in curve (0.42, 0, 1.0, 1.0)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(CustomCurveTest, ANIMATION_TEST, CreateCubicCurve_NORMAL_EaseIn)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto curve = RSAnimationTimingCurve::CreateCubicCurve(0.42f, 0.0f, 1.0f, 1.0f);
    auto animation = CreateCurveAnimation(animModifier, curve);
}

/*
 * @tc.name: CreateCubicCurve_NORMAL_EaseOut
 * @tc.desc: Test CreateCubicCurve with ease-out curve (0, 0, 0.58, 1.0)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(CustomCurveTest, ANIMATION_TEST, CreateCubicCurve_NORMAL_EaseOut)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto curve = RSAnimationTimingCurve::CreateCubicCurve(0.0f, 0.0f, 0.58f, 1.0f);
    auto animation = CreateCurveAnimation(animModifier, curve);
}

/*
 * @tc.name: CreateCubicCurve_NORMAL_Linear
 * @tc.desc: Test CreateCubicCurve with linear curve (0, 0, 1, 1)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(CustomCurveTest, ANIMATION_TEST, CreateCubicCurve_NORMAL_Linear)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto curve = RSAnimationTimingCurve::CreateCubicCurve(0.0f, 0.0f, 1.0f, 1.0f);
    auto animation = CreateCurveAnimation(animModifier, curve);
}

/*
 * @tc.name: CreateCubicCurve_NORMAL_Custom
 * @tc.desc: Test CreateCubicCurve with custom control points
 * @tc.type: FUNC
 */
GRAPHIC_TEST(CustomCurveTest, ANIMATION_TEST, CreateCubicCurve_NORMAL_Custom)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto curve = RSAnimationTimingCurve::CreateCubicCurve(0.68f, -0.55f, 0.27f, 1.55f);
    auto animation = CreateCurveAnimation(animModifier, curve);
}

/*
 * @tc.name: CreateCubicCurve_Boundary_CtrlX1_Max
 * @tc.desc: Test CreateCubicCurve with maximum ctrlX1 (1.0)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(CustomCurveTest, ANIMATION_TEST, CreateCubicCurve_Boundary_CtrlX1_Max)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto curve = RSAnimationTimingCurve::CreateCubicCurve(1.0f, 0.0f, 1.0f, 1.0f);
    auto animation = CreateCurveAnimation(animModifier, curve);
}

/*
 * @tc.name: CreateCubicCurve_Boundary_CtrlY1_Max
 * @tc.desc: Test CreateCubicCurve with maximum ctrlY1 (1.0)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(CustomCurveTest, ANIMATION_TEST, CreateCubicCurve_Boundary_CtrlY1_Max)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto curve = RSAnimationTimingCurve::CreateCubicCurve(0.5f, 1.0f, 0.5f, 1.0f);
    auto animation = CreateCurveAnimation(animModifier, curve);
}

/*
 * @tc.name: CreateCubicCurve_Boundary_CtrlX2_Max
 * @tc.desc: Test CreateCubicCurve with maximum ctrlX2 (1.0)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(CustomCurveTest, ANIMATION_TEST, CreateCubicCurve_Boundary_CtrlX2_Max)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto curve = RSAnimationTimingCurve::CreateCubicCurve(0.0f, 0.0f, 1.0f, 1.0f);
    auto animation = CreateCurveAnimation(animModifier, curve);
}

/*
 * @tc.name: CreateCubicCurve_Boundary_CtrlY2_Max
 * @tc.desc: Test CreateCubicCurve with maximum ctrlY2 (1.0)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(CustomCurveTest, ANIMATION_TEST, CreateCubicCurve_Boundary_CtrlY2_Max)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto curve = RSAnimationTimingCurve::CreateCubicCurve(0.5f, 0.5f, 0.5f, 1.0f);
    auto animation = CreateCurveAnimation(animModifier, curve);
}

/*
 * @tc.name: CreateCubicCurve_Abnormal_CtrlX1_Negative
 * @tc.desc: Test CreateCubicCurve with negative ctrlX1
 * @tc.type: FUNC
 */
GRAPHIC_TEST(CustomCurveTest, ANIMATION_TEST, CreateCubicCurve_Abnormal_CtrlX1_Negative)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto curve = RSAnimationTimingCurve::CreateCubicCurve(-0.5f, 0.0f, 1.0f, 1.0f);
    auto animation = CreateCurveAnimation(animModifier, curve);
}

/*
 * @tc.name: CreateCubicCurve_Abnormal_CtrlY1_Negative
 * @tc.desc: Test CreateCubicCurve with negative ctrlY1
 * @tc.type: FUNC
 */
GRAPHIC_TEST(CustomCurveTest, ANIMATION_TEST, CreateCubicCurve_Abnormal_CtrlY1_Negative)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto curve = RSAnimationTimingCurve::CreateCubicCurve(0.5f, -0.5f, 1.0f, 1.0f);
    auto animation = CreateCurveAnimation(animModifier, curve);
}

/*
 * @tc.name: CreateCubicCurve_Abnormal_CtrlX2_Negative
 * @tc.desc: Test CreateCubicCurve with negative ctrlX2
 * @tc.type: FUNC
 */
GRAPHIC_TEST(CustomCurveTest, ANIMATION_TEST, CreateCubicCurve_Abnormal_CtrlX2_Negative)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto curve = RSAnimationTimingCurve::CreateCubicCurve(0.5f, 0.0f, -0.5f, 1.0f);
    auto animation = CreateCurveAnimation(animModifier, curve);
}

/*
 * @tc.name: CreateCubicCurve_Abnormal_CtrlY2_Negative
 * @tc.desc: Test CreateCubicCurve with negative ctrlY2
 * @tc.type: FUNC
 */
GRAPHIC_TEST(CustomCurveTest, ANIMATION_TEST, CreateCubicCurve_Abnormal_CtrlY2_Negative)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto curve = RSAnimationTimingCurve::CreateCubicCurve(0.5f, 0.0f, 1.0f, -0.5f);
    auto animation = CreateCurveAnimation(animModifier, curve);
}

/*
 * @tc.name: CreateCubicCurve_Abnormal_All_NaN
 * @tc.desc: Test CreateCubicCurve with all NaN control points
 * @tc.type: FUNC
 */
GRAPHIC_TEST(CustomCurveTest, ANIMATION_TEST, CreateCubicCurve_Abnormal_All_NaN)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto curve = RSAnimationTimingCurve::CreateCubicCurve(NAN, NAN, NAN, NAN);
    auto animation = CreateCurveAnimation(animModifier, curve);
}

/*
 * @tc.name: CreateCubicCurve_Abnormal_All_Infinity
 * @tc.desc: Test CreateCubicCurve with all infinity control points
 * @tc.type: FUNC
 */
GRAPHIC_TEST(CustomCurveTest, ANIMATION_TEST, CreateCubicCurve_Abnormal_All_Infinity)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto curve = RSAnimationTimingCurve::CreateCubicCurve(INFINITY, INFINITY, INFINITY, INFINITY);
    auto animation = CreateCurveAnimation(animModifier, curve);
}

// ============================================================================
// Section 3: CreateStepsCurve Tests (阶梯曲线测试)
// ============================================================================

/*
 * @tc.name: CreateStepsCurve_NORMAL_Start_2Steps
 * @tc.desc: Test CreateStepsCurve with start position, 2 steps
 * @tc.type: FUNC
 */
GRAPHIC_TEST(CustomCurveTest, ANIMATION_TEST, CreateStepsCurve_NORMAL_Start_2Steps)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto curve = RSAnimationTimingCurve::CreateStepsCurve(2, StepsCurvePosition::START);
    auto animation = CreateCurveAnimation(animModifier, curve);
}

/*
 * @tc.name: CreateStepsCurve_NORMAL_End_3Steps
 * @tc.desc: Test CreateStepsCurve with end position, 3 steps
 * @tc.type: FUNC
 */
GRAPHIC_TEST(CustomCurveTest, ANIMATION_TEST, CreateStepsCurve_NORMAL_End_3Steps)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto curve = RSAnimationTimingCurve::CreateStepsCurve(3, StepsCurvePosition::END);
    auto animation = CreateCurveAnimation(animModifier, curve);
}

/*
 * @tc.name: CreateStepsCurve_NORMAL_Start_4Steps
 * @tc.desc: Test CreateStepsCurve with start position, 4 steps
 * @tc.type: FUNC
 */
GRAPHIC_TEST(CustomCurveTest, ANIMATION_TEST, CreateStepsCurve_NORMAL_Start_4Steps)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto curve = RSAnimationTimingCurve::CreateStepsCurve(4, StepsCurvePosition::START);
    auto animation = CreateCurveAnimation(animModifier, curve);
}

/*
 * @tc.name: CreateStepsCurve_NORMAL_End_5Steps
 * @tc.desc: Test CreateStepsCurve with end position, 5 steps
 * @tc.type: FUNC
 */
GRAPHIC_TEST(CustomCurveTest, ANIMATION_TEST, CreateStepsCurve_NORMAL_End_5Steps)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto curve = RSAnimationTimingCurve::CreateStepsCurve(5, StepsCurvePosition::END);
    auto animation = CreateCurveAnimation(animModifier, curve);
}

/*
 * @tc.name: CreateStepsCurve_Boundary_1Step
 * @tc.desc: Test CreateStepsCurve with 1 step (boundary)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(CustomCurveTest, ANIMATION_TEST, CreateStepsCurve_Boundary_1Step)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto curve = RSAnimationTimingCurve::CreateStepsCurve(1, StepsCurvePosition::START);
    auto animation = CreateCurveAnimation(animModifier, curve);
}

/*
 * @tc.name: CreateStepsCurve_Boundary_MaxSteps
 * @tc.desc: Test CreateStepsCurve with many steps (100 steps)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(CustomCurveTest, ANIMATION_TEST, CreateStepsCurve_Boundary_MaxSteps)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto curve = RSAnimationTimingCurve::CreateStepsCurve(100, StepsCurvePosition::START);
    auto animation = CreateCurveAnimation(animModifier, curve);
}

/*
 * @tc.name: CreateStepsCurve_Abnormal_ZeroSteps
 * @tc.desc: Test CreateStepsCurve with 0 steps (abnormal)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(CustomCurveTest, ANIMATION_TEST, CreateStepsCurve_Abnormal_ZeroSteps)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto curve = RSAnimationTimingCurve::CreateStepsCurve(0, StepsCurvePosition::START);
    auto animation = CreateCurveAnimation(animModifier, curve);
}

/*
 * @tc.name: CreateStepsCurve_Abnormal_NegativeSteps
 * @tc.desc: Test CreateStepsCurve with negative steps (abnormal)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(CustomCurveTest, ANIMATION_TEST, CreateStepsCurve_Abnormal_NegativeSteps)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto curve = RSAnimationTimingCurve::CreateStepsCurve(-5, StepsCurvePosition::START);
    auto animation = CreateCurveAnimation(animModifier, curve);
}

// ============================================================================
// Section 4: CreateCustomCurve Tests (自定义函数曲线测试)
// ============================================================================

/*
 * @tc.name: CreateCustomCurve_NORMAL_Linear
 * @tc.desc: Test CreateCustomCurve with linear function
 * @tc.type: FUNC
 */
GRAPHIC_TEST(CustomCurveTest, ANIMATION_TEST, CreateCustomCurve_NORMAL_Linear)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto curve = RSAnimationTimingCurve::CreateCustomCurve([](float t) { return t; });
    auto animation = CreateCurveAnimation(animModifier, curve);
}

/*
 * @tc.name: CreateCustomCurve_NORMAL_Quadratic
 * @tc.desc: Test CreateCustomCurve with quadratic function
 * @tc.type: FUNC
 */
GRAPHIC_TEST(CustomCurveTest, ANIMATION_TEST, CreateCustomCurve_NORMAL_Quadratic)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto curve = RSAnimationTimingCurve::CreateCustomCurve([](float t) { return t * t; });
    auto animation = CreateCurveAnimation(animModifier, curve);
}

/*
 * @tc.name: CreateCustomCurve_NORMAL_EaseInOut
 * @tc.desc: Test CreateCustomCurve with ease-in-out function
 * @tc.type: FUNC
 */
GRAPHIC_TEST(CustomCurveTest, ANIMATION_TEST, CreateCustomCurve_NORMAL_EaseInOut)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto curve = RSAnimationTimingCurve::CreateCustomCurve([](float t) {
        return t < 0.5f ? 2.0f * t * t : 1.0f - 2.0f * (1.0f - t) * (1.0f - t);
    });
    auto animation = CreateCurveAnimation(animModifier, curve);
}

/*
 * @tc.name: CreateCustomCurve_NORMAL_Sinusoidal
 * @tc.desc: Test CreateCustomCurve with sinusoidal function
 * @tc.type: FUNC
 */
GRAPHIC_TEST(CustomCurveTest, ANIMATION_TEST, CreateCustomCurve_NORMAL_Sinusoidal)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto curve = RSAnimationTimingCurve::CreateCustomCurve([](float t) {
        return std::sin(t * 3.14159f); // sin(pi*t)
    });
    auto animation = CreateCurveAnimation(animModifier, curve);
}

/*
 * @tc.name: CreateCustomCurve_Boundary_ZeroInput
 * @tc.desc: Test CreateCustomCurve with function that handles t=0
 * @tc.type: FUNC
 */
GRAPHIC_TEST(CustomCurveTest, ANIMATION_TEST, CreateCustomCurve_Boundary_ZeroInput)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto curve = RSAnimationTimingCurve::CreateCustomCurve([](float t) { return t * 2.0f; });
    auto animation = CreateCurveAnimation(animModifier, curve);
}

/*
 * @tc.name: CreateCustomCurve_Boundary_OneInput
 * @tc.desc: Test CreateCustomCurve with function that handles t=1
 * @tc.type: FUNC
 */
GRAPHIC_TEST(CustomCurveTest, ANIMATION_TEST, CreateCustomCurve_Boundary_OneInput)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto curve = RSAnimationTimingCurve::CreateCustomCurve([](float t) { return std::sqrt(t); });
    auto animation = CreateCurveAnimation(animModifier, curve);
}

/*
 * @tc.name: CreateCustomCurve_Abnormal_ConstantZero
 * @tc.desc: Test CreateCustomCurve with function that always returns 0
 * @tc.type: FUNC
 */
GRAPHIC_TEST(CustomCurveTest, ANIMATION_TEST, CreateCustomCurve_Abnormal_ConstantZero)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto curve = RSAnimationTimingCurve::CreateCustomCurve([](float t) { return 0.0f; });
    auto animation = CreateCurveAnimation(animModifier, curve);
}

/*
 * @tc.name: CreateCustomCurve_Abnormal_ConstantOne
 * @tc.desc: Test CreateCustomCurve with function that always returns 1
 * @tc.type: FUNC
 */
GRAPHIC_TEST(CustomCurveTest, ANIMATION_TEST, CreateCustomCurve_Abnormal_ConstantOne)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto curve = RSAnimationTimingCurve::CreateCustomCurve([](float t) { return 1.0f; });
    auto animation = CreateCurveAnimation(animModifier, curve);
}

/*
 * @tc.name: CreateCustomCurve_Abnormal_GreaterThanOne
 * @tc.desc: Test CreateCustomCurve with function that returns >1
 * @tc.type: FUNC
 */
GRAPHIC_TEST(CustomCurveTest, ANIMATION_TEST, CreateCustomCurve_Abnormal_GreaterThanOne)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto curve = RSAnimationTimingCurve::CreateCustomCurve([](float t) { return t * 2.0f; });
    auto animation = CreateCurveAnimation(animModifier, curve);
}

/*
 * @tc.name: CreateCustomCurve_Abnormal_Negative
 * @tc.desc: Test CreateCustomCurve with function that returns negative values
 * @tc.type: FUNC
 */
GRAPHIC_TEST(CustomCurveTest, ANIMATION_TEST, CreateCustomCurve_Abnormal_Negative)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto curve = RSAnimationTimingCurve::CreateCustomCurve([](float t) { return -t; });
    auto animation = CreateCurveAnimation(animModifier, curve);
}

// ============================================================================
// Section 5: CreateInterpolatingSpring Tests (插值弹簧曲线测试)
// ============================================================================

/*
 * @tc.name: CreateInterpolatingSpring_NORMAL_Default
 * @tc.desc: Test CreateInterpolatingSpring with default parameters
 * @tc.type: FUNC
 */
GRAPHIC_TEST(CustomCurveTest, ANIMATION_TEST, CreateInterpolatingSpring_NORMAL_Default)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(1000);

    auto timingCurve = RSAnimationTimingCurve::CreateInterpolatingSpring(1.0f, 1.0f, 1.0f, 0.0f);
    RSNode::Animate(RSGraphicTestDirector::Instance().GetRSUIContext(), protocol, timingCurve, [&]() {
        animModifier->SetPosition(1000.0f);
    });
}

/*
 * @tc.name: CreateInterpolatingSpring_Boundary_MassZero
 * @tc.desc: Test CreateInterpolatingSpring with zero mass (boundary)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(CustomCurveTest, ANIMATION_TEST, CreateInterpolatingSpring_Boundary_MassZero)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(1000);

    auto timingCurve = RSAnimationTimingCurve::CreateInterpolatingSpring(0.0f, 10.0f, 1.0f, 0.0f);
    RSNode::Animate(RSGraphicTestDirector::Instance().GetRSUIContext(), protocol, timingCurve, [&]() {
        animModifier->SetPosition(1000.0f);
    });
}

/*
 * @tc.name: CreateInterpolatingSpring_Boundary_StiffnessZero
 * @tc.desc: Test CreateInterpolatingSpring with zero stiffness (boundary)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(CustomCurveTest, ANIMATION_TEST, CreateInterpolatingSpring_Boundary_StiffnessZero)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(1000);

    auto timingCurve = RSAnimationTimingCurve::CreateInterpolatingSpring(1.0f, 0.0f, 1.0f, 0.0f);
    RSNode::Animate(RSGraphicTestDirector::Instance().GetRSUIContext(), protocol, timingCurve, [&]() {
        animModifier->SetPosition(1000.0f);
    });
}

/*
 * @tc.name: CreateInterpolatingSpring_Abnormal_MassNegative
 * @tc.desc: Test CreateInterpolatingSpring with negative mass
 * @tc.type: FUNC
 */
GRAPHIC_TEST(CustomCurveTest, ANIMATION_TEST, CreateInterpolatingSpring_Abnormal_MassNegative)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(1000);

    auto timingCurve = RSAnimationTimingCurve::CreateInterpolatingSpring(-1.0f, 10.0f, 1.0f, 0.0f);
    RSNode::Animate(RSGraphicTestDirector::Instance().GetRSUIContext(), protocol, timingCurve, [&]() {
        animModifier->SetPosition(1000.0f);
    });
}

/*
 * @tc.name: CreateInterpolatingSpring_Abnormal_StiffnessNegative
 * @tc.desc: Test CreateInterpolatingSpring with negative stiffness
 * @tc.type: FUNC
 */
GRAPHIC_TEST(CustomCurveTest, ANIMATION_TEST, CreateInterpolatingSpring_Abnormal_StiffnessNegative)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(1000);

    auto timingCurve = RSAnimationTimingCurve::CreateInterpolatingSpring(1.0f, -10.0f, 1.0f, 0.0f);
    RSNode::Animate(RSGraphicTestDirector::Instance().GetRSUIContext(), protocol, timingCurve, [&]() {
        animModifier->SetPosition(1000.0f);
    });
}

/*
 * @tc.name: CreateInterpolatingSpring_Abnormal_AllNaN
 * @tc.desc: Test CreateInterpolatingSpring with all NaN parameters
 * @tc.type: FUNC
 */
GRAPHIC_TEST(CustomCurveTest, ANIMATION_TEST, CreateInterpolatingSpring_Abnormal_AllNaN)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(1000);

    auto timingCurve = RSAnimationTimingCurve::CreateInterpolatingSpring(NAN, NAN, NAN, NAN);
    RSNode::Animate(RSGraphicTestDirector::Instance().GetRSUIContext(), protocol, timingCurve, [&]() {
        animModifier->SetPosition(1000.0f);
    });
}

} // namespace OHOS::Rosen
