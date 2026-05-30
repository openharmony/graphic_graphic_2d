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

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

// ============================================================================
// Test Fixture for Interactive Animation Tests
// ============================================================================

class InteractiveAnimationTest : public RSGraphicTest {
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
        std::shared_ptr<AnimationCustomModifier> modifier)
    {
        RSAnimationTimingProtocol protocol;
        protocol.SetDuration(2000);

        auto timingCurve = RSAnimationTimingCurve::EASE_IN_OUT;
        auto animations = RSNode::Animate(RSGraphicTestDirector::Instance().GetRSUIContext(),
            protocol, timingCurve, [&]() {
            modifier->SetPosition(1000.0f);
        });

        if (!animations.empty()) {
            return animations[0];
        }
        return nullptr;
    }
};

// ============================================================================
// Section 1: Normal Value Tests - Interactive Methods (正常值测试)
// ============================================================================

/*
 * @tc.name: InteractivePause_NORMAL_DuringAnimation
 * @tc.desc: Test interactive pause during normal animation playback
 * @tc.type: FUNC
 */
GRAPHIC_TEST(InteractiveAnimationTest, ANIMATION_TEST, InteractivePause_NORMAL_DuringAnimation)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier);

    // Interactive pause
    animation->InteractivePause();
}

/*
 * @tc.name: InteractiveContinue_NORMAL_AfterPause
 * @tc.desc: Test interactive continue after pause
 * @tc.type: FUNC
 */
GRAPHIC_TEST(InteractiveAnimationTest, ANIMATION_TEST, InteractiveContinue_NORMAL_AfterPause)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier);

    // Interactive pause then continue
    animation->InteractivePause();
    animation->InteractiveContinue();
}

/*
 * @tc.name: InteractiveFinish_NORMAL_AtStart
 * @tc.desc: Test interactive finish at animation start
 * @tc.type: FUNC
 */
GRAPHIC_TEST(InteractiveAnimationTest, ANIMATION_TEST, InteractiveFinish_NORMAL_AtStart)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier);

    // Interactive finish at start
    animation->InteractiveFinish(RSInteractiveAnimationPosition::START);
}

/*
 * @tc.name: InteractiveFinish_NORMAL_AtEnd
 * @tc.desc: Test interactive finish at animation end
 * @tc.type: FUNC
 */
GRAPHIC_TEST(InteractiveAnimationTest, ANIMATION_TEST, InteractiveFinish_NORMAL_AtEnd)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier);

    // Interactive finish at end
    animation->InteractiveFinish(RSInteractiveAnimationPosition::END);
}

/*
 * @tc.name: InteractiveFinish_NORMAL_AtProgress
 * @tc.desc: Test interactive finish at middle progress
 * @tc.type: FUNC
 */
GRAPHIC_TEST(InteractiveAnimationTest, ANIMATION_TEST, InteractiveFinish_NORMAL_AtProgress)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier);

    // Interactive finish at progress (0.5)
    animation->InteractiveFinish(RSInteractiveAnimationPosition::CURRENT);
}

/*
 * @tc.name: InteractiveReverse_NORMAL_DuringAnimation
 * @tc.desc: Test interactive reverse during animation
 * @tc.type: FUNC
 */
GRAPHIC_TEST(InteractiveAnimationTest, ANIMATION_TEST, InteractiveReverse_NORMAL_DuringAnimation)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier);

    // Interactive reverse
    animation->InteractiveReverse();
}

/*
 * @tc.name: InteractiveSetFraction_NORMAL_Half
 * @tc.desc: Test interactive set fraction to 0.5
 * @tc.type: FUNC
 */
GRAPHIC_TEST(InteractiveAnimationTest, ANIMATION_TEST, InteractiveSetFraction_NORMAL_Half)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier);

    // Interactive set fraction to 0.5
    animation->InteractiveSetFraction(0.5f);
}

/*
 * @tc.name: InteractiveSetFraction_NORMAL_Quarter
 * @tc.desc: Test interactive set fraction to 0.25
 * @tc.type: FUNC
 */
GRAPHIC_TEST(InteractiveAnimationTest, ANIMATION_TEST, InteractiveSetFraction_NORMAL_Quarter)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier);

    // Interactive set fraction to 0.25
    animation->InteractiveSetFraction(0.25f);
}

/*
 * @tc.name: InteractiveSetFraction_NORMAL_ThreeQuarter
 * @tc.desc: Test interactive set fraction to 0.75
 * @tc.type: FUNC
 */
GRAPHIC_TEST(InteractiveAnimationTest, ANIMATION_TEST, InteractiveSetFraction_NORMAL_ThreeQuarter)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier);

    // Interactive set fraction to 0.75
    animation->InteractiveSetFraction(0.75f);
}

// ============================================================================
// Section 2: Boundary Value Tests - Interactive Methods (边界值测试)
// ============================================================================

/*
 * @tc.name: InteractiveSetFraction_Boundary_Zero
 * @tc.desc: Test interactive set fraction to 0.0 (beginning)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(InteractiveAnimationTest, ANIMATION_TEST, InteractiveSetFraction_Boundary_Zero)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier);

    // Interactive set fraction to 0.0 (boundary)
    animation->InteractiveSetFraction(0.0f);
}

/*
 * @tc.name: InteractiveSetFraction_Boundary_One
 * @tc.desc: Test interactive set fraction to 1.0 (end)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(InteractiveAnimationTest, ANIMATION_TEST, InteractiveSetFraction_Boundary_One)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier);

    // Interactive set fraction to 1.0 (boundary)
    animation->InteractiveSetFraction(1.0f);
}

/*
 * @tc.name: InteractiveSetFraction_Boundary_MinPositive
 * @tc.desc: Test interactive set fraction to FLT_MIN
 * @tc.type: FUNC
 */
GRAPHIC_TEST(InteractiveAnimationTest, ANIMATION_TEST, InteractiveSetFraction_Boundary_MinPositive)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier);

    // Interactive set fraction to FLT_MIN (boundary)
    animation->InteractiveSetFraction(FLT_MIN);
}

/*
 * @tc.name: InteractiveFinish_Boundary_PositionStart
 * @tc.desc: Test interactive finish with START position
 * @tc.type: FUNC
 */
GRAPHIC_TEST(InteractiveAnimationTest, ANIMATION_TEST, InteractiveFinish_Boundary_PositionStart)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier);

    // Interactive finish at START position
    animation->InteractiveFinish(RSInteractiveAnimationPosition::START);
}

/*
 * @tc.name: InteractiveFinish_Boundary_PositionEnd
 * @tc.desc: Test interactive finish with END position
 * @tc.type: FUNC
 */
GRAPHIC_TEST(InteractiveAnimationTest, ANIMATION_TEST, InteractiveFinish_Boundary_PositionEnd)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier);

    // Interactive finish at END position
    animation->InteractiveFinish(RSInteractiveAnimationPosition::END);
}

// ============================================================================
// Section 3: Abnormal Value Tests - Interactive Methods (异常值测试)
// ============================================================================

/*
 * @tc.name: InteractiveSetFraction_Abnormal_Negative
 * @tc.desc: Test interactive set fraction with negative value
 * @tc.type: FUNC
 */
GRAPHIC_TEST(InteractiveAnimationTest, ANIMATION_TEST, InteractiveSetFraction_Abnormal_Negative)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier);

    // Interactive set fraction to -0.5 (abnormal)
    animation->InteractiveSetFraction(-0.5f);
}

/*
 * @tc.name: InteractiveSetFraction_Abnormal_GreaterThanOne
 * @tc.desc: Test interactive set fraction with value > 1
 * @tc.type: FUNC
 */
GRAPHIC_TEST(InteractiveAnimationTest, ANIMATION_TEST, InteractiveSetFraction_Abnormal_GreaterThanOne)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier);

    // Interactive set fraction to 1.5 (abnormal)
    animation->InteractiveSetFraction(1.5f);
}

/*
 * @tc.name: InteractiveSetFraction_Abnormal_LargeNegative
 * @tc.desc: Test interactive set fraction with large negative value
 * @tc.type: FUNC
 */
GRAPHIC_TEST(InteractiveAnimationTest, ANIMATION_TEST, InteractiveSetFraction_Abnormal_LargeNegative)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier);

    // Interactive set fraction to -10.0 (abnormal)
    animation->InteractiveSetFraction(-10.0f);
}

/*
 * @tc.name: InteractiveSetFraction_Abnormal_LargePositive
 * @tc.desc: Test interactive set fraction with large positive value
 * @tc.type: FUNC
 */
GRAPHIC_TEST(InteractiveAnimationTest, ANIMATION_TEST, InteractiveSetFraction_Abnormal_LargePositive)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier);

    // Interactive set fraction to 100.0 (abnormal)
    animation->InteractiveSetFraction(100.0f);
}

/*
 * @tc.name: InteractiveSetFraction_Abnormal_NaN
 * @tc.desc: Test interactive set fraction with NaN
 * @tc.type: FUNC
 */
GRAPHIC_TEST(InteractiveAnimationTest, ANIMATION_TEST, InteractiveSetFraction_Abnormal_NaN)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier);

    // Interactive set fraction to NaN (abnormal)
    animation->InteractiveSetFraction(NAN);
}

/*
 * @tc.name: InteractiveSetFraction_Abnormal_Infinity
 * @tc.desc: Test interactive set fraction with infinity
 * @tc.type: FUNC
 */
GRAPHIC_TEST(InteractiveAnimationTest, ANIMATION_TEST, InteractiveSetFraction_Abnormal_Infinity)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier);

    // Interactive set fraction to Infinity (abnormal)
    animation->InteractiveSetFraction(INFINITY);
}

/*
 * @tc.name: InteractivePause_Abnormal_NotStarted
 * @tc.desc: Test interactive pause before animation starts
 * @tc.type: FUNC
 */
GRAPHIC_TEST(InteractiveAnimationTest, ANIMATION_TEST, InteractivePause_Abnormal_NotStarted)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier);
    // Don't add animation, just pause

    // Interactive pause without starting (abnormal)
    animation->InteractivePause();
}

/*
 * @tc.name: InteractiveContinue_Abnormal_NotPaused
 * @tc.desc: Test interactive continue without pause
 * @tc.type: FUNC
 */
GRAPHIC_TEST(InteractiveAnimationTest, ANIMATION_TEST, InteractiveContinue_Abnormal_NotPaused)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier);

    // Interactive continue without pausing first (abnormal)
    animation->InteractiveContinue();
}

/*
 * @tc.name: InteractiveReverse_Abnormal_NotStarted
 * @tc.desc: Test interactive reverse before animation starts
 * @tc.type: FUNC
 */
GRAPHIC_TEST(InteractiveAnimationTest, ANIMATION_TEST, InteractiveReverse_Abnormal_NotStarted)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier);
    // Don't add animation, just reverse

    // Interactive reverse without starting (abnormal)
    animation->InteractiveReverse();
}

/*
 * @tc.name: InteractiveFinish_Abnormal_AlreadyFinished
 * @tc.desc: Test interactive finish when already finished
 * @tc.type: FUNC
 */
GRAPHIC_TEST(InteractiveAnimationTest, ANIMATION_TEST, InteractiveFinish_Abnormal_AlreadyFinished)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier);

    // Interactive finish twice (abnormal)
    animation->InteractiveFinish(RSInteractiveAnimationPosition::END);
    animation->InteractiveFinish(RSInteractiveAnimationPosition::END);
}

// ============================================================================
// Section 4: Multi-Scenario Combination Tests (多场景组合测试)
// ============================================================================

/*
 * @tc.name: Combo_InteractivePause_InteractiveContinue
 * @tc.desc: Test combo: interactive pause -> interactive continue
 * @tc.type: FUNC
 */
GRAPHIC_TEST(InteractiveAnimationTest, ANIMATION_TEST, Combo_InteractivePause_InteractiveContinue)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier);

    // Pause then continue
    animation->InteractivePause();
    animation->InteractiveContinue();
}

/*
 * @tc.name: Combo_InteractivePause_InteractiveSetFraction
 * @tc.desc: Test combo: interactive pause -> interactive set fraction
 * @tc.type: FUNC
 */
GRAPHIC_TEST(InteractiveAnimationTest, ANIMATION_TEST, Combo_InteractivePause_InteractiveSetFraction)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier);

    // Pause then set fraction
    animation->InteractivePause();
    animation->InteractiveSetFraction(0.4f);
}

/*
 * @tc.name: Combo_InteractivePause_InteractiveSetFraction_Continue
 * @tc.desc: Test combo: pause -> set fraction -> continue
 * @tc.type: FUNC
 */
GRAPHIC_TEST(InteractiveAnimationTest, ANIMATION_TEST, Combo_InteractivePause_InteractiveSetFraction_Continue)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier);

    // Pause, set fraction, then continue
    animation->InteractivePause();
    animation->InteractiveSetFraction(0.6f);
    animation->InteractiveContinue();
}

/*
 * @tc.name: Combo_InteractiveReverse_InteractiveSetFraction
 * @tc.desc: Test combo: interactive reverse -> interactive set fraction
 * @tc.type: FUNC
 */
GRAPHIC_TEST(InteractiveAnimationTest, ANIMATION_TEST, Combo_InteractiveReverse_InteractiveSetFraction)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier);

    // Reverse then set fraction
    animation->InteractiveReverse();
    animation->InteractiveSetFraction(0.3f);
}

/*
 * @tc.name: Combo_InteractiveFinish_Start_AfterReverse
 * @tc.desc: Test combo: interactive reverse -> interactive finish at start
 * @tc.type: FUNC
 */
GRAPHIC_TEST(InteractiveAnimationTest, ANIMATION_TEST, Combo_InteractiveReverse_InteractiveFinish_Start)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier);

    // Reverse then finish at start
    animation->InteractiveReverse();
    animation->InteractiveFinish(RSInteractiveAnimationPosition::START);
}

/*
 * @tc.name: Combo_Multiple_InteractivePause_Continue
 * @tc.desc: Test combo: multiple interactive pause/continue cycles
 * @tc.type: FUNC
 */
GRAPHIC_TEST(InteractiveAnimationTest, ANIMATION_TEST, Combo_Multiple_InteractivePause_Continue)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier);

    // Multiple pause/continue cycles
    animation->InteractivePause();
    animation->InteractiveContinue();
    animation->InteractivePause();
    animation->InteractiveContinue();
    animation->InteractivePause();
    animation->InteractiveContinue();
}

/*
 * @tc.name: Combo_InteractiveSetFraction_Pause_Continue
 * @tc.desc: Test combo: set fraction -> pause -> continue
 * @tc.type: FUNC
 */
GRAPHIC_TEST(InteractiveAnimationTest, ANIMATION_TEST, Combo_InteractiveSetFraction_Pause_Continue)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier);

    // Set fraction, pause, then continue
    animation->InteractiveSetFraction(0.4f);
    animation->InteractivePause();
    animation->InteractiveContinue();
}

/*
 * @tc.name: Combo_InteractivePause_InteractiveFinish
 * @tc.desc: Test combo: interactive pause -> interactive finish
 * @tc.type: FUNC
 */
GRAPHIC_TEST(InteractiveAnimationTest, ANIMATION_TEST, Combo_InteractivePause_InteractiveFinish)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier);

    // Pause then finish
    animation->InteractivePause();
    animation->InteractiveFinish(RSInteractiveAnimationPosition::CURRENT);
}

/*
 * @tc.name: Combo_InteractiveSetFraction_InteractiveReverse
 * @tc.desc: Test combo: set fraction -> interactive reverse
 * @tc.type: FUNC
 */
GRAPHIC_TEST(InteractiveAnimationTest, ANIMATION_TEST, Combo_InteractiveSetFraction_InteractiveReverse)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier);

    // Set fraction then reverse
    animation->InteractiveSetFraction(0.7f);
    animation->InteractiveReverse();
}

/*
 * @tc.name: Combo_Complex_InteractiveWorkflow
 * @tc.desc: Test complex interactive workflow: pause -> set fraction -> reverse -> continue
 * @tc.type: FUNC
 */
GRAPHIC_TEST(InteractiveAnimationTest, ANIMATION_TEST, Combo_Complex_InteractiveWorkflow)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier);

    // Complex workflow: pause -> set fraction -> reverse -> pause -> continue
    animation->InteractivePause();
    animation->InteractiveSetFraction(0.3f);
    animation->InteractiveReverse();
    animation->InteractivePause();
    animation->InteractiveContinue();
}

/*
 * @tc.name: Combo_InteractiveFinish_AfterSetFraction
 * @tc.desc: Test combo: set fraction -> interactive finish at progress
 * @tc.type: FUNC
 */
GRAPHIC_TEST(InteractiveAnimationTest, ANIMATION_TEST, Combo_InteractiveFinish_AfterSetFraction)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier);

    // Set fraction then finish
    animation->InteractiveSetFraction(0.8f);
    animation->InteractiveFinish(RSInteractiveAnimationPosition::CURRENT);
}

/*
 * @tc.name: Combo_InteractiveSetFractionMultiple_Values
 * @tc.desc: Test combo: multiple interactive set fraction calls
 * @tc.type: FUNC
 */
GRAPHIC_TEST(InteractiveAnimationTest, ANIMATION_TEST, Combo_InteractiveSetFractionMultiple_Values)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier);

    // Multiple set fraction calls
    animation->InteractiveSetFraction(0.2f);
    animation->InteractiveSetFraction(0.4f);
    animation->InteractiveSetFraction(0.6f);
    animation->InteractiveSetFraction(0.8f);
}

} // namespace OHOS::Rosen
