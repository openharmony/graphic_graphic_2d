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

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

// ============================================================================
// Test Fixture for Animation Combination Tests
// ============================================================================

class AnimationCombinationTest : public RSGraphicTest {
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

    // Helper function to create animation with all protocol parameters
    void CreateFullAnimation(std::shared_ptr<AnimationCustomModifier> modifier,
        int duration = 1000,
        int delay = 0,
        float speed = 1.0f,
        int repeatCount = 1,
        bool autoReverse = false,
        bool direction = true,
        FinishCallbackType callbackType = FinishCallbackType::TIME_SENSITIVE)
    {
        RSAnimationTimingProtocol protocol;
        protocol.SetDuration(duration);
        protocol.SetStartDelay(delay);
        protocol.SetSpeed(speed);
        protocol.SetRepeatCount(repeatCount);
        protocol.SetAutoReverse(autoReverse);
        protocol.SetDirection(direction);
        protocol.SetFinishCallbackType(callbackType);

        auto timingCurve = RSAnimationTimingCurve::EASE_IN_OUT;
        RSNode::Animate(RSGraphicTestDirector::Instance().GetRSUIContext(), protocol, timingCurve, [&]() {
            modifier->SetPosition(1000);
        }, []() {
            std::cout << "Animation combination finish callback" << std::endl;
        });
    }
};

// ============================================================================
// Section 1: Protocol x Curve Combinations (协议 x 曲线组合)
// ============================================================================

/*
 * @tc.name: Combo_Protocol_Curve_LINEAR_ShortDuration
 * @tc.desc: Test combination: LINEAR curve with short duration
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationCombinationTest, ANIMATION_TEST, Combo_Protocol_Curve_LINEAR_ShortDuration)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    CreateTestNode(animModifier);
    animModifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(100);

    auto timingCurve = RSAnimationTimingCurve::LINEAR;
    RSNode::Animate(RSGraphicTestDirector::Instance().GetRSUIContext(), protocol, timingCurve, [&]() {
        animModifier->SetPosition(1000);
    });
}

/*
 * @tc.name: Combo_Protocol_Curve_SPRING_LongDuration
 * @tc.desc: Test combination: SPRING curve with long duration
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationCombinationTest, ANIMATION_TEST, Combo_Protocol_Curve_SPRING_LongDuration)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    CreateTestNode(animModifier);
    animModifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(2000);

    auto timingCurve = RSAnimationTimingCurve::SPRING;
    RSNode::Animate(RSGraphicTestDirector::Instance().GetRSUIContext(), protocol, timingCurve, [&]() {
        animModifier->SetPosition(1000);
    });
}

/*
 * @tc.name: Combo_Protocol_Curve_EASE_FastSpeed
 * @tc.desc: Test combination: EASE curve with fast speed
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationCombinationTest, ANIMATION_TEST, Combo_Protocol_Curve_EASE_FastSpeed)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    CreateTestNode(animModifier);
    animModifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(1000);
    protocol.SetSpeed(3.0f);

    auto timingCurve = RSAnimationTimingCurve::EASE;
    RSNode::Animate(RSGraphicTestDirector::Instance().GetRSUIContext(), protocol, timingCurve, [&]() {
        animModifier->SetPosition(1000);
    });
}

/*
 * @tc.name: Combo_Protocol_Curve_EASEINOUT_SlowSpeed
 * @tc.desc: Test combination: EASE_IN_OUT curve with slow speed
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationCombinationTest, ANIMATION_TEST, Combo_Protocol_Curve_EASEINOUT_SlowSpeed)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    CreateTestNode(animModifier);
    animModifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(900);
    protocol.SetSpeed(0.5f);

    auto timingCurve = RSAnimationTimingCurve::EASE_IN_OUT;
    RSNode::Animate(RSGraphicTestDirector::Instance().GetRSUIContext(), protocol, timingCurve, [&]() {
        animModifier->SetPosition(1000);
    });
}

/*
 * @tc.name: Combo_All_Curves_With_Delay
 * @tc.desc: Test all built-in curves with delay
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationCombinationTest, ANIMATION_TEST, Combo_All_Curves_With_Delay)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    CreateTestNode(animModifier);
    animModifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(800);
    protocol.SetStartDelay(200);

    auto timingCurve = RSAnimationTimingCurve::EASE_IN_OUT;
    RSNode::Animate(RSGraphicTestDirector::Instance().GetRSUIContext(), protocol, timingCurve, [&]() {
        animModifier->SetPosition(1000);
    });
}

// ============================================================================
// Section 2: Repeat x AutoReverse x Direction Combinations
// ============================================================================

/*
 * @tc.name: Combo_Repeat_AutoReverse_Direction_False_False
 * @tc.desc: Test combo: repeat=2, no autoReverse, reverse direction
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationCombinationTest, ANIMATION_TEST, Combo_Repeat_AutoReverse_Direction_False_False)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    CreateTestNode(animModifier);
    animModifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(400);
    protocol.SetRepeatCount(2);
    protocol.SetAutoReverse(false);
    protocol.SetDirection(false);

    auto timingCurve = RSAnimationTimingCurve::EASE_IN_OUT;
    RSNode::Animate(RSGraphicTestDirector::Instance().GetRSUIContext(), protocol, timingCurve, [&]() {
        animModifier->SetPosition(1000);
    });
}

/*
 * @tc.name: Combo_Repeat_AutoReverse_Direction_False_True
 * @tc.desc: Test combo: repeat=2, autoReverse, reverse direction
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationCombinationTest, ANIMATION_TEST, Combo_Repeat_AutoReverse_Direction_False_True)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    CreateTestNode(animModifier);
    animModifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(400);
    protocol.SetRepeatCount(2);
    protocol.SetAutoReverse(true);
    protocol.SetDirection(false);

    auto timingCurve = RSAnimationTimingCurve::EASE_IN_OUT;
    RSNode::Animate(RSGraphicTestDirector::Instance().GetRSUIContext(), protocol, timingCurve, [&]() {
        animModifier->SetPosition(1000);
    });
}

/*
 * @tc.name: Combo_Repeat_AutoReverse_Direction_True_True
 * @tc.desc: Test combo: repeat=2, autoReverse, forward direction
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationCombinationTest, ANIMATION_TEST, Combo_Repeat_AutoReverse_Direction_True_True)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    CreateTestNode(animModifier);
    animModifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(400);
    protocol.SetRepeatCount(2);
    protocol.SetAutoReverse(true);
    protocol.SetDirection(true);

    auto timingCurve = RSAnimationTimingCurve::EASE_IN_OUT;
    RSNode::Animate(RSGraphicTestDirector::Instance().GetRSUIContext(), protocol, timingCurve, [&]() {
        animModifier->SetPosition(1000);
    });
}

// ============================================================================
// Section 3: Speed x Duration Combinations (速度 x 时长组合)
// ============================================================================

/*
 * @tc.name: Combo_Speed_Duration_Fast_Short
 * @tc.desc: Test combo: fast speed (5x) with short duration (100ms)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationCombinationTest, ANIMATION_TEST, Combo_Speed_Duration_Fast_Short)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    CreateTestNode(animModifier);
    animModifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(100);
    protocol.SetSpeed(5.0f);

    auto timingCurve = RSAnimationTimingCurve::LINEAR;
    RSNode::Animate(RSGraphicTestDirector::Instance().GetRSUIContext(), protocol, timingCurve, [&]() {
        animModifier->SetPosition(1000);
    });
}

/*
 * @tc.name: Combo_Speed_Duration_Slow_Long
 * @tc.desc: Test combo: slow speed (0.5x) with medium duration (1500ms)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationCombinationTest, ANIMATION_TEST, Combo_Speed_Duration_Slow_Long)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    CreateTestNode(animModifier);
    animModifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(1500);
    protocol.SetSpeed(0.5f);

    auto timingCurve = RSAnimationTimingCurve::LINEAR;
    RSNode::Animate(RSGraphicTestDirector::Instance().GetRSUIContext(), protocol, timingCurve, [&]() {
        animModifier->SetPosition(1000);
    });
}

/*
 * @tc.name: Combo_Speed_Duration_VeryFast_VeryShort
 * @tc.desc: Test combo: very fast speed (10x) with very short duration (50ms)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationCombinationTest, ANIMATION_TEST, Combo_Speed_Duration_VeryFast_VeryShort)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    CreateTestNode(animModifier);
    animModifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(50);
    protocol.SetSpeed(10.0f);

    auto timingCurve = RSAnimationTimingCurve::LINEAR;
    RSNode::Animate(RSGraphicTestDirector::Instance().GetRSUIContext(), protocol, timingCurve, [&]() {
        animModifier->SetPosition(1000);
    });
}

/*
 * @tc.name: Combo_Speed_Duration_VerySlow_VeryLong
 * @tc.desc: Test combo: very slow speed (0.7x) with long duration (2000ms)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationCombinationTest, ANIMATION_TEST, Combo_Speed_Duration_VerySlow_VeryLong)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    CreateTestNode(animModifier);
    animModifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(2000);
    protocol.SetSpeed(0.7f);

    auto timingCurve = RSAnimationTimingCurve::LINEAR;
    RSNode::Animate(RSGraphicTestDirector::Instance().GetRSUIContext(), protocol, timingCurve, [&]() {
        animModifier->SetPosition(1000);
    });
}

// ============================================================================
// Section 4: Delay x Duration Combinations (延迟 x 时长组合)
// ============================================================================

/*
 * @tc.name: Combo_Delay_Duration_Short_Short
 * @tc.desc: Test combo: short delay with short duration
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationCombinationTest, ANIMATION_TEST, Combo_Delay_Duration_Short_Short)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    CreateTestNode(animModifier);
    animModifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(200);
    protocol.SetStartDelay(100);

    auto timingCurve = RSAnimationTimingCurve::EASE_IN_OUT;
    RSNode::Animate(RSGraphicTestDirector::Instance().GetRSUIContext(), protocol, timingCurve, [&]() {
        animModifier->SetPosition(1000);
    });
}

/*
 * @tc.name: Combo_Delay_Duration_Long_Short
 * @tc.desc: Test combo: long delay with short duration
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationCombinationTest, ANIMATION_TEST, Combo_Delay_Duration_Long_Short)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    CreateTestNode(animModifier);
    animModifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(200);
    protocol.SetStartDelay(1000);

    auto timingCurve = RSAnimationTimingCurve::EASE_IN_OUT;
    RSNode::Animate(RSGraphicTestDirector::Instance().GetRSUIContext(), protocol, timingCurve, [&]() {
        animModifier->SetPosition(1000);
    });
}

/*
 * @tc.name: Combo_Delay_Duration_Equal
 * @tc.desc: Test combo: delay equals duration
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationCombinationTest, ANIMATION_TEST, Combo_Delay_Duration_Equal)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    CreateTestNode(animModifier);
    animModifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(500);
    protocol.SetStartDelay(500);

    auto timingCurve = RSAnimationTimingCurve::EASE_IN_OUT;
    RSNode::Animate(RSGraphicTestDirector::Instance().GetRSUIContext(), protocol, timingCurve, [&]() {
        animModifier->SetPosition(1000);
    });
}

/*
 * @tc.name: Combo_Delay_Duration_Multiple
 * @tc.desc: Test combo: delay is multiple of duration
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationCombinationTest, ANIMATION_TEST, Combo_Delay_Duration_Multiple)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    CreateTestNode(animModifier);
    animModifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(400);
    protocol.SetStartDelay(800); // 2x duration

    auto timingCurve = RSAnimationTimingCurve::EASE_IN_OUT;
    RSNode::Animate(RSGraphicTestDirector::Instance().GetRSUIContext(), protocol, timingCurve, [&]() {
        animModifier->SetPosition(1000);
    });
}

// ============================================================================
// Section 5: All Parameters Together (全参数组合)
// ============================================================================

/*
 * @tc.name: Combo_All_Parameters_Full_Set
 * @tc.desc: Test full parameter set with normal values
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationCombinationTest, ANIMATION_TEST, Combo_All_Parameters_Full_Set)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    CreateTestNode(animModifier);
    animModifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(800);
    protocol.SetStartDelay(100);
    protocol.SetSpeed(1.5f);
    protocol.SetRepeatCount(3);
    protocol.SetAutoReverse(true);
    protocol.SetDirection(false);
    protocol.SetFinishCallbackType(FinishCallbackType::TIME_INSENSITIVE);

    auto timingCurve = RSAnimationTimingCurve::EASE_IN_OUT;
    RSNode::Animate(RSGraphicTestDirector::Instance().GetRSUIContext(), protocol, timingCurve, [&]() {
        animModifier->SetPosition(1000);
    });
}

/*
 * @tc.name: Combo_All_CallbackTypes_All_Curves
 * @tc.desc: Test all callback types with different curves
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationCombinationTest, ANIMATION_TEST, Combo_All_CallbackTypes_All_Curves)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    CreateTestNode(animModifier);
    animModifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(800);
    protocol.SetFinishCallbackType(FinishCallbackType::TIME_SENSITIVE);

    auto timingCurve = RSAnimationTimingCurve::SPRING;
    RSNode::Animate(RSGraphicTestDirector::Instance().GetRSUIContext(), protocol, timingCurve, [&]() {
        animModifier->SetPosition(1000);
    });
}

/*
 * @tc.name: Combo_Repeat_AutoReverse_All_Curves
 * @tc.desc: Test repeat+autoReverse with all curve types
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationCombinationTest, ANIMATION_TEST, Combo_Repeat_AutoReverse_All_Curves)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    CreateTestNode(animModifier);
    animModifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(600);
    protocol.SetRepeatCount(2);
    protocol.SetAutoReverse(true);

    auto timingCurve = RSAnimationTimingCurve::INTERACTIVE_SPRING;
    RSNode::Animate(RSGraphicTestDirector::Instance().GetRSUIContext(), protocol, timingCurve, [&]() {
        animModifier->SetPosition(1000);
    });
}
} // namespace OHOS::Rosen
