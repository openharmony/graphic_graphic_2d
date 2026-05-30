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
// Test Fixture for Animation Control Tests
// ============================================================================

class AnimationControlTest : public RSGraphicTest {
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
    std::shared_ptr<RSAnimation> CreateCurveAnimation(std::shared_ptr<AnimationCustomModifier> modifier)
    {
        RSAnimationTimingProtocol protocol;
        protocol.SetDuration(2000);

        auto timingCurve = RSAnimationTimingCurve::EASE_IN_OUT;
        auto animations = RSNode::Animate(RSGraphicTestDirector::Instance().GetRSUIContext(),
            protocol, timingCurve, [&]() {
            modifier->SetPosition(1000.0f);
        });

        // Return the first animation from the vector for control method calls
        if (!animations.empty()) {
            return animations[0];
        }
        return nullptr;
    }
};

// ============================================================================
// Section 1: Normal Value Tests - Pause/Resume (正常值测试)
// ============================================================================

/*
 * @tc.name: Pause_Normal_DuringAnimation
 * @tc.desc: Test pausing an animation during normal execution
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationControlTest, ANIMATION_TEST, Pause_Normal_DuringAnimation)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier);

    // Pause the animation
    animation->Pause();
}

/*
 * @tc.name: Resume_Normal_AfterPause
 * @tc.desc: Test resuming an animation after pausing
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationControlTest, ANIMATION_TEST, Resume_Normal_AfterPause)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier);

    // Pause then resume
    animation->Pause();
    animation->Resume();
}

/*
 * @tc.name: Finish_Normal_BeforeCompletion
 * @tc.desc: Test finishing an animation before natural completion
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationControlTest, ANIMATION_TEST, Finish_Normal_BeforeCompletion)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier);

    // Finish the animation immediately
    animation->Finish();
}

/*
 * @tc.name: Reverse_Normal_DuringAnimation
 * @tc.desc: Test reversing an animation during normal execution
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationControlTest, ANIMATION_TEST, Reverse_Normal_DuringAnimation)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier);

    // Reverse the animation
    animation->Reverse();
}

/*
 * @tc.name: SetFraction_Normal_HalfProgress
 * @tc.desc: Test setting animation fraction to 0.5 (half progress)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationControlTest, ANIMATION_TEST, SetFraction_Normal_HalfProgress)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier);

    // Set fraction to 0.5
    animation->SetFraction(0.5f);
}

/*
 * @tc.name: SetFraction_Normal_QuarterProgress
 * @tc.desc: Test setting animation fraction to 0.25 (quarter progress)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationControlTest, ANIMATION_TEST, SetFraction_Normal_QuarterProgress)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier);

    // Set fraction to 0.25
    animation->SetFraction(0.25f);
}

/*
 * @tc.name: SetFraction_Normal_ThreeQuarterProgress
 * @tc.desc: Test setting animation fraction to 0.75 (three-quarter progress)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationControlTest, ANIMATION_TEST, SetFraction_Normal_ThreeQuarterProgress)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier);

    // Set fraction to 0.75
    animation->SetFraction(0.75f);
}

// ============================================================================
// Section 2: Boundary Value Tests - Pause/Resume (边界值测试)
// ============================================================================

/*
 * @tc.name: SetFraction_Boundary_Zero
 * @tc.desc: Test setting animation fraction to 0.0 (beginning)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationControlTest, ANIMATION_TEST, SetFraction_Boundary_Zero)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier);

    // Set fraction to 0.0 (boundary: beginning)
    animation->SetFraction(0.0f);
}

/*
 * @tc.name: SetFraction_Boundary_One
 * @tc.desc: Test setting animation fraction to 1.0 (end)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationControlTest, ANIMATION_TEST, SetFraction_Boundary_One)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier);

    // Set fraction to 1.0 (boundary: end)
    animation->SetFraction(1.0f);
}

/*
 * @tc.name: SetFraction_Boundary_MinPositive
 * @tc.desc: Test setting animation fraction to minimum positive float
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationControlTest, ANIMATION_TEST, SetFraction_Boundary_MinPositive)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier);

    // Set fraction to FLT_MIN (boundary: minimum positive)
    animation->SetFraction(FLT_MIN);
}

/*
 * @tc.name: Pause_Resume_Boundary_ShortAnimation
 * @tc.desc: Test pause/resume on very short duration animation (1ms)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationControlTest, ANIMATION_TEST, Pause_Resume_Boundary_ShortAnimation)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(1); // Boundary: minimum duration

    auto timingCurve = RSAnimationTimingCurve::EASE_IN_OUT;
    auto animations = RSNode::Animate(RSGraphicTestDirector::Instance().GetRSUIContext(),
        protocol, timingCurve, [&]() {
        animModifier->SetPosition(1000.0f);
    });

    if (!animations.empty()) {
        animations[0]->Pause();
        animations[0]->Resume();
    }
}

// ============================================================================
// Section 3: Abnormal Value Tests - Pause/Resume (异常值测试)
// ============================================================================

/*
 * @tc.name: SetFraction_Abnormal_Negative
 * @tc.desc: Test setting animation fraction to negative value
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationControlTest, ANIMATION_TEST, SetFraction_Abnormal_Negative)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier);

    // Set fraction to -0.5 (abnormal: negative)
    animation->SetFraction(-0.5f);
}

/*
 * @tc.name: SetFraction_Abnormal_GreaterThanOne
 * @tc.desc: Test setting animation fraction to value greater than 1
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationControlTest, ANIMATION_TEST, SetFraction_Abnormal_GreaterThanOne)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier);

    // Set fraction to 1.5 (abnormal: > 1)
    animation->SetFraction(1.5f);
}

/*
 * @tc.name: SetFraction_Abnormal_MaxNegative
 * @tc.desc: Test setting animation fraction to very large negative value
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationControlTest, ANIMATION_TEST, SetFraction_Abnormal_MaxNegative)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier);

    // Set fraction to -100 (abnormal: very large negative)
    animation->SetFraction(-100.0f);
}

/*
 * @tc.name: SetFraction_Abnormal_VeryLargePositive
 * @tc.desc: Test setting animation fraction to very large positive value
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationControlTest, ANIMATION_TEST, SetFraction_Abnormal_VeryLargePositive)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier);

    // Set fraction to 100 (abnormal: very large positive)
    animation->SetFraction(100.0f);
}

/*
 * @tc.name: SetFraction_Abnormal_NaN
 * @tc.desc: Test setting animation fraction to NaN
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationControlTest, ANIMATION_TEST, SetFraction_Abnormal_NaN)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier);

    // Set fraction to NaN (abnormal)
    animation->SetFraction(NAN);
}

/*
 * @tc.name: SetFraction_Abnormal_Infinity
 * @tc.desc: Test setting animation fraction to infinity
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationControlTest, ANIMATION_TEST, SetFraction_Abnormal_Infinity)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier);

    // Set fraction to Infinity (abnormal)
    animation->SetFraction(INFINITY);
}

/*
 * @tc.name: Pause_Abnormal_NotStarted
 * @tc.desc: Test pausing animation that hasn't started
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationControlTest, ANIMATION_TEST, Pause_Abnormal_NotStarted)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier);
    // Don't start the animation, just pause

    animation->Pause();
}

/*
 * @tc.name: Resume_Abnormal_NotPaused
 * @tc.desc: Test resuming animation that wasn't paused
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationControlTest, ANIMATION_TEST, Resume_Abnormal_NotPaused)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier);

    // Resume without pausing first
    animation->Resume();
}

/*
 * @tc.name: Reverse_Abnormal_NotStarted
 * @tc.desc: Test reversing animation that hasn't started
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationControlTest, ANIMATION_TEST, Reverse_Abnormal_NotStarted)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier);
    // Don't start the animation, just reverse

    animation->Reverse();
}

/*
 * @tc.name: Finish_Abnormal_AlreadyFinished
 * @tc.desc: Test finishing animation that's already finished
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationControlTest, ANIMATION_TEST, Finish_Abnormal_AlreadyFinished)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier);

    // Finish twice
    animation->Finish();
    animation->Finish(); // Abnormal: already finished
}

// ============================================================================
// Section 4: Multi-Scenario Combination Tests (多场景组合测试)
// ============================================================================

/*
 * @tc.name: Combo_Pause_Resume_SetFraction
 * @tc.desc: Test combination: pause -> set fraction -> resume
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationControlTest, ANIMATION_TEST, Combo_Pause_Resume_SetFraction)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier);

    // Pause, set fraction, then resume
    animation->Pause();
    animation->SetFraction(0.3f);
    animation->Resume();
}

/*
 * @tc.name: Combo_Pause_Reverse_Resume
 * @tc.desc: Test combination: pause -> reverse -> resume
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationControlTest, ANIMATION_TEST, Combo_Pause_Reverse_Resume)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier);

    // Pause, reverse, then resume
    animation->Pause();
    animation->Reverse();
    animation->Resume();
}

/*
 * @tc.name: Combo_SetFraction_Pause_SetFraction_Resume
 * @tc.desc: Test combination: set fraction -> pause -> set fraction -> resume
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationControlTest, ANIMATION_TEST, Combo_SetFraction_Pause_SetFraction_Resume)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier);

    // Set fraction, pause, set fraction again, then resume
    animation->SetFraction(0.2f);
    animation->Pause();
    animation->SetFraction(0.6f);
    animation->Resume();
}

/*
 * @tc.name: Combo_Pause_Finish
 * @tc.desc: Test combination: pause -> finish
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationControlTest, ANIMATION_TEST, Combo_Pause_Finish)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier);

    // Pause then finish
    animation->Pause();
    animation->Finish();
}

/*
 * @tc.name: Combo_Reverse_SetFraction
 * @tc.desc: Test combination: reverse -> set fraction
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationControlTest, ANIMATION_TEST, Combo_Reverse_SetFraction)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier);

    // Reverse then set fraction
    animation->Reverse();
    animation->SetFraction(0.4f);
}

/*
 * @tc.name: Combo_Multiple_Pause_Resume
 * @tc.desc: Test combination: multiple pause/resume cycles
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationControlTest, ANIMATION_TEST, Combo_Multiple_Pause_Resume)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier);

    // Multiple pause/resume cycles
    animation->Pause();
    animation->Resume();
    animation->Pause();
    animation->Resume();
    animation->Pause();
    animation->Resume();
}

/*
 * @tc.name: Combo_SetFraction_Reverse_SetFraction
 * @tc.desc: Test combination: set fraction -> reverse -> set fraction
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationControlTest, ANIMATION_TEST, Combo_SetFraction_Reverse_SetFraction)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier);

    // Set fraction, reverse, set fraction again
    animation->SetFraction(0.3f);
    animation->Reverse();
    animation->SetFraction(0.7f);
}

/*
 * @tc.name: Combo_Pause_SetFraction_Reverse_Resume
 * @tc.desc: Test complex combination: pause -> set fraction -> reverse -> resume
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationControlTest, ANIMATION_TEST, Combo_Pause_SetFraction_Reverse_Resume)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier);

    // Complex combination
    animation->Pause();
    animation->SetFraction(0.25f);
    animation->Reverse();
    animation->Resume();
}

/*
 * @tc.name: Combo_Finish_SetFraction
 * @tc.desc: Test combination: finish -> set fraction (edge case)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationControlTest, ANIMATION_TEST, Combo_Finish_SetFraction)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier);

    // Finish then set fraction (edge case)
    animation->Finish();
    animation->SetFraction(0.5f);
}

/*
 * @tc.name: State_Query_IsStarted
 * @tc.desc: Test querying IsStarted state after animation creation
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationControlTest, ANIMATION_TEST, State_Query_IsStarted)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier);

    // Query IsStarted state
    bool isStarted = animation->IsStarted();
    std::cout << "Animation IsStarted: " << isStarted << std::endl;
}

/*
 * @tc.name: State_Query_IsRunning
 * @tc.desc: Test querying IsRunning state during animation
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationControlTest, ANIMATION_TEST, State_Query_IsRunning)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier);

    // Query IsRunning state
    bool isRunning = animation->IsRunning();
    std::cout << "Animation IsRunning: " << isRunning << std::endl;
}

/*
 * @tc.name: State_Query_IsPaused
 * @tc.desc: Test querying IsPaused state after pause
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationControlTest, ANIMATION_TEST, State_Query_IsPaused)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier);

    animation->Pause();

    // Query IsPaused state
    bool isPaused = animation->IsPaused();
    std::cout << "Animation IsPaused: " << isPaused << std::endl;
}

/*
 * @tc.name: State_Query_IsFinished
 * @tc.desc: Test querying IsFinished state after finish
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationControlTest, ANIMATION_TEST, State_Query_IsFinished)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier);

    animation->Finish();

    // Query IsFinished state
    bool isFinished = animation->IsFinished();
    std::cout << "Animation IsFinished: " << isFinished << std::endl;
}

/*
 * @tc.name: State_Query_AllStates
 * @tc.desc: Test querying all animation states in sequence
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationControlTest, ANIMATION_TEST, State_Query_AllStates)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateCurveAnimation(animModifier);

    // Query all states
    std::cout << "IsStarted: " << animation->IsStarted() << std::endl;
    std::cout << "IsRunning: " << animation->IsRunning() << std::endl;
    std::cout << "IsPaused: " << animation->IsPaused() << std::endl;
    std::cout << "IsFinished: " << animation->IsFinished() << std::endl;

    animation->Pause();
    std::cout << "After Pause - IsPaused: " << animation->IsPaused() << std::endl;

    animation->Resume();
    std::cout << "After Resume - IsPaused: " << animation->IsPaused() << std::endl;

    animation->Finish();
    std::cout << "After Finish - IsFinished: " << animation->IsFinished() << std::endl;
}

} // namespace OHOS::Rosen
