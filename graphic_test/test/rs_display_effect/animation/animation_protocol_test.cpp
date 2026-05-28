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

#include "point_custom_modifier_test.h"
#include "rs_graphic_test.h"
#include "rs_graphic_test_director.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

// ============================================================================
// Test Data Structures for Parameterized Testing
// ============================================================================

struct DurationTestParams {
    int duration;
    const char* description;
    bool isNormal;
};

struct SpeedTestParams {
    float speed;
    const char* description;
    bool isNormal;
};

struct RepeatCountTestParams {
    int repeatCount;
    const char* description;
    bool isNormal;
};

struct DelayTestParams {
    int delay;
    const char* description;
    bool isNormal;
};

// ============================================================================
// Test Fixture
// ============================================================================

class AnimationProtocolTest : public RSGraphicTest {
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

    // Helper function to create a basic animation
    void CreateBasicAnimation(std::shared_ptr<PointCustomModifier> modifier,
        const RSAnimationTimingProtocol& protocol,
        const RSAnimationTimingCurve& curve = RSAnimationTimingCurve::EASE_IN_OUT)
    {
        RSNode::Animate(RSGraphicTestDirector::Instance().GetRSUIContext(), protocol, curve, [&]() {
            modifier->SetPosition(1000);
        }, []() {
            std::cout << "Animation finish callback" << std::endl;
        });
    }
};

// ============================================================================
// Section 1: Normal Value Tests (正常值测试)
// ============================================================================

/*
 * @tc.name: Duration_Normal_Short
 * @tc.desc: Test animation with short normal duration (100ms)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationProtocolTest, ANIMATION_TEST, Duration_Normal_Short)
{
    auto pointModifier = std::make_shared<PointCustomModifier>();
    CreateTestNode(pointModifier);
    pointModifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(100); // Normal: short duration
    CreateBasicAnimation(pointModifier, protocol);
}

/*
 * @tc.name: Duration_Normal_Medium
 * @tc.desc: Test animation with medium normal duration (500ms)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationProtocolTest, ANIMATION_TEST, Duration_Normal_Medium)
{
    auto pointModifier = std::make_shared<PointCustomModifier>();
    CreateTestNode(pointModifier);
    pointModifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(500); // Normal: medium duration
    CreateBasicAnimation(pointModifier, protocol);
}

/*
 * @tc.name: Duration_Normal_Long
 * @tc.desc: Test animation with long normal duration (2000ms)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationProtocolTest, ANIMATION_TEST, Duration_Normal_Long)
{
    auto pointModifier = std::make_shared<PointCustomModifier>();
    CreateTestNode(pointModifier);
    pointModifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(2000); // Normal: long duration
    CreateBasicAnimation(pointModifier, protocol);
}

/*
 * @tc.name: Speed_Normal_Slow
 * @tc.desc: Test animation with slow speed (0.5x)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationProtocolTest, ANIMATION_TEST, Speed_Normal_Slow)
{
    auto pointModifier = std::make_shared<PointCustomModifier>();
    CreateTestNode(pointModifier);
    pointModifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(800);
    protocol.SetSpeed(0.5f); // Normal: slow speed
    CreateBasicAnimation(pointModifier, protocol);
}

/*
 * @tc.name: Speed_Normal_Default
 * @tc.desc: Test animation with default speed (1.0x)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationProtocolTest, ANIMATION_TEST, Speed_Normal_Default)
{
    auto pointModifier = std::make_shared<PointCustomModifier>();
    CreateTestNode(pointModifier);
    pointModifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(800);
    protocol.SetSpeed(1.0f); // Normal: default speed
    CreateBasicAnimation(pointModifier, protocol);
}

/*
 * @tc.name: Speed_Normal_Fast
 * @tc.desc: Test animation with fast speed (2.0x)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationProtocolTest, ANIMATION_TEST, Speed_Normal_Fast)
{
    auto pointModifier = std::make_shared<PointCustomModifier>();
    CreateTestNode(pointModifier);
    pointModifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(800);
    protocol.SetSpeed(2.0f); // Normal: fast speed
    CreateBasicAnimation(pointModifier, protocol);
}

/*
 * @tc.name: RepeatCount_Normal_Single
 * @tc.desc: Test animation with single repeat (count = 1)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationProtocolTest, ANIMATION_TEST, RepeatCount_Normal_Single)
{
    auto pointModifier = std::make_shared<PointCustomModifier>();
    CreateTestNode(pointModifier);
    pointModifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(300);
    protocol.SetRepeatCount(1); // Normal: single repeat
    CreateBasicAnimation(pointModifier, protocol);
}

/*
 * @tc.name: RepeatCount_Normal_Multiple
 * @tc.desc: Test animation with multiple repeats (count = 3)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationProtocolTest, ANIMATION_TEST, RepeatCount_Normal_Multiple)
{
    auto pointModifier = std::make_shared<PointCustomModifier>();
    CreateTestNode(pointModifier);
    pointModifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(300);
    protocol.SetRepeatCount(3); // Normal: multiple repeats
    CreateBasicAnimation(pointModifier, protocol);
}

/*
 * @tc.name: Delay_Normal_Short
 * @tc.desc: Test animation with short delay (100ms)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationProtocolTest, ANIMATION_TEST, Delay_Normal_Short)
{
    auto pointModifier = std::make_shared<PointCustomModifier>();
    CreateTestNode(pointModifier);
    pointModifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(800);
    protocol.SetStartDelay(100); // Normal: short delay
    CreateBasicAnimation(pointModifier, protocol);
}

/*
 * @tc.name: Delay_Normal_Long
 * @tc.desc: Test animation with long delay (500ms)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationProtocolTest, ANIMATION_TEST, Delay_Normal_Long)
{
    auto pointModifier = std::make_shared<PointCustomModifier>();
    CreateTestNode(pointModifier);
    pointModifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(800);
    protocol.SetStartDelay(500); // Normal: long delay
    CreateBasicAnimation(pointModifier, protocol);
}

/*
 * @tc.name: Direction_Normal_Forward
 * @tc.desc: Test animation with forward direction
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationProtocolTest, ANIMATION_TEST, Direction_Normal_Forward)
{
    auto pointModifier = std::make_shared<PointCustomModifier>();
    CreateTestNode(pointModifier);
    pointModifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(800);
    protocol.SetDirection(true); // Normal: forward direction
    CreateBasicAnimation(pointModifier, protocol);
}

/*
 * @tc.name: AutoReverse_Normal_Disabled
 * @tc.desc: Test animation without auto reverse
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationProtocolTest, ANIMATION_TEST, AutoReverse_Normal_Disabled)
{
    auto pointModifier = std::make_shared<PointCustomModifier>();
    CreateTestNode(pointModifier);
    pointModifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(400);
    protocol.SetRepeatCount(2);
    protocol.SetAutoReverse(false); // Normal: no auto reverse
    CreateBasicAnimation(pointModifier, protocol);
}

/*
 * @tc.name: AutoReverse_Normal_Enabled
 * @tc.desc: Test animation with auto reverse enabled
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationProtocolTest, ANIMATION_TEST, AutoReverse_Normal_Enabled)
{
    auto pointModifier = std::make_shared<PointCustomModifier>();
    CreateTestNode(pointModifier);
    pointModifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(400);
    protocol.SetRepeatCount(2);
    protocol.SetAutoReverse(true); // Normal: auto reverse enabled
    CreateBasicAnimation(pointModifier, protocol);
}

/*
 * @tc.name: FinishCallbackType_NORMAL_TimeSensitive
 * @tc.desc: Test animation with TIME_SENSITIVE callback type
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationProtocolTest, ANIMATION_TEST, FinishCallbackType_NORMAL_TimeSensitive)
{
    auto pointModifier = std::make_shared<PointCustomModifier>();
    CreateTestNode(pointModifier);
    pointModifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(800);
    protocol.SetFinishCallbackType(FinishCallbackType::TIME_SENSITIVE);
    CreateBasicAnimation(pointModifier, protocol);
}

/*
 * @tc.name: FinishCallbackType_NORMAL_TimeInsensitive
 * @tc.desc: Test animation with TIME_INSENSITIVE callback type
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationProtocolTest, ANIMATION_TEST, FinishCallbackType_NORMAL_TimeInsensitive)
{
    auto pointModifier = std::make_shared<PointCustomModifier>();
    CreateTestNode(pointModifier);
    pointModifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(800);
    protocol.SetFinishCallbackType(FinishCallbackType::TIME_INSENSITIVE);
    CreateBasicAnimation(pointModifier, protocol);
}

/*
 * @tc.name: FinishCallbackType_NORMAL_Logically
 * @tc.desc: Test animation with LOGICALLY callback type
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationProtocolTest, ANIMATION_TEST, FinishCallbackType_NORMAL_Logically)
{
    auto pointModifier = std::make_shared<PointCustomModifier>();
    CreateTestNode(pointModifier);
    pointModifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(800);
    protocol.SetFinishCallbackType(FinishCallbackType::LOGICALLY);
    CreateBasicAnimation(pointModifier, protocol);
}

/*
 * @tc.name: ConstantProtocol_NORMAL_Default
 * @tc.desc: Test the built-in DEFAULT protocol
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationProtocolTest, ANIMATION_TEST, ConstantProtocol_NORMAL_Default)
{
    auto pointModifier = std::make_shared<PointCustomModifier>();
    CreateTestNode(pointModifier);
    pointModifier->SetPosition(100);

    RSAnimationTimingProtocol protocol = RSAnimationTimingProtocol::DEFAULT;
    CreateBasicAnimation(pointModifier, protocol);
}

/*
 * @tc.name: ConstantProtocol_NORMAL_Immediate
 * @tc.desc: Test the built-in IMMEDIATE protocol
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationProtocolTest, ANIMATION_TEST, ConstantProtocol_NORMAL_Immediate)
{
    auto pointModifier = std::make_shared<PointCustomModifier>();
    CreateTestNode(pointModifier);
    pointModifier->SetPosition(100);

    RSAnimationTimingProtocol protocol = RSAnimationTimingProtocol::IMMEDIATE;
    CreateBasicAnimation(pointModifier, protocol);
}

// ============================================================================
// Section 2: Boundary Value Tests (边界值测试)
// ============================================================================

/*
 * @tc.name: Duration_Boundary_Min
 * @tc.desc: Test animation with minimum duration (1ms)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationProtocolTest, ANIMATION_TEST, Duration_Boundary_Min)
{
    auto pointModifier = std::make_shared<PointCustomModifier>();
    CreateTestNode(pointModifier);
    pointModifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(1); // Boundary: minimum positive value
    CreateBasicAnimation(pointModifier, protocol);
}

/*
 * @tc.name: Duration_Boundary_Zero
 * @tc.desc: Test animation with zero duration
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationProtocolTest, ANIMATION_TEST, Duration_Boundary_Zero)
{
    auto pointModifier = std::make_shared<PointCustomModifier>();
    CreateTestNode(pointModifier);
    pointModifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(0); // Boundary: zero
    CreateBasicAnimation(pointModifier, protocol);
}

/*
 * @tc.name: Speed_Boundary_Max
 * @tc.desc: Test animation with maximum speed (FLT_MAX)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationProtocolTest, ANIMATION_TEST, Speed_Boundary_Max)
{
    auto pointModifier = std::make_shared<PointCustomModifier>();
    CreateTestNode(pointModifier);
    pointModifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(800);
    protocol.SetSpeed(FLT_MAX); // Boundary: maximum float
    CreateBasicAnimation(pointModifier, protocol);
}

/*
 * @tc.name: Delay_Boundary_Zero
 * @tc.desc: Test animation with zero delay
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationProtocolTest, ANIMATION_TEST, Delay_Boundary_Zero)
{
    auto pointModifier = std::make_shared<PointCustomModifier>();
    CreateTestNode(pointModifier);
    pointModifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(800);
    protocol.SetStartDelay(0); // Boundary: zero
    CreateBasicAnimation(pointModifier, protocol);
}

/*
 * @tc.name: RepeatCount_Boundary_Zero
 * @tc.desc: Test animation with zero repeat count
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationProtocolTest, ANIMATION_TEST, RepeatCount_Boundary_Zero)
{
    auto pointModifier = std::make_shared<PointCustomModifier>();
    CreateTestNode(pointModifier);
    pointModifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(800);
    protocol.SetRepeatCount(0); // Boundary: zero
    CreateBasicAnimation(pointModifier, protocol);
}

// ============================================================================
// Section 3: Abnormal Value Tests (异常值测试)
// ============================================================================

/*
 * @tc.name: Duration_Abnormal_Negative
 * @tc.desc: Test animation with negative duration (-100ms)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationProtocolTest, ANIMATION_TEST, Duration_Abnormal_Negative)
{
    auto pointModifier = std::make_shared<PointCustomModifier>();
    CreateTestNode(pointModifier);
    pointModifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(-100); // Abnormal: negative value
    CreateBasicAnimation(pointModifier, protocol);
}

/*
 * @tc.name: Duration_Abnormal_MinNegative
 * @tc.desc: Test animation with minimum negative duration (INT32_MIN)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationProtocolTest, ANIMATION_TEST, Duration_Abnormal_MinNegative)
{
    auto pointModifier = std::make_shared<PointCustomModifier>();
    CreateTestNode(pointModifier);
    pointModifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(INT32_MIN); // Abnormal: minimum negative value
    CreateBasicAnimation(pointModifier, protocol);
}

/*
 * @tc.name: Speed_Abnormal_Inf
 * @tc.desc: Test animation with infinite speed
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationProtocolTest, ANIMATION_TEST, Speed_Abnormal_Inf)
{
    auto pointModifier = std::make_shared<PointCustomModifier>();
    CreateTestNode(pointModifier);
    pointModifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(800);
    protocol.SetSpeed(INFINITY); // Abnormal: infinity
    CreateBasicAnimation(pointModifier, protocol);
}

/*
 * @tc.name: Delay_Abnormal_Negative_Small
 * @tc.desc: Test animation with small negative delay (-100ms)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationProtocolTest, ANIMATION_TEST, Delay_Abnormal_Negative_Small)
{
    auto pointModifier = std::make_shared<PointCustomModifier>();
    CreateTestNode(pointModifier);
    pointModifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(800);
    protocol.SetStartDelay(-100); // Abnormal: small negative
    CreateBasicAnimation(pointModifier, protocol);
}

/*
 * @tc.name: Delay_Abnormal_Negative_Large
 * @tc.desc: Test animation with large negative delay (-400ms)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationProtocolTest, ANIMATION_TEST, Delay_Abnormal_Negative_Large)
{
    auto pointModifier = std::make_shared<PointCustomModifier>();
    CreateTestNode(pointModifier);
    pointModifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(800);
    protocol.SetStartDelay(-400); // Abnormal: large negative
    CreateBasicAnimation(pointModifier, protocol);
}

/*
 * @tc.name: Delay_Abnormal_MinNegative
 * @tc.desc: Test animation with minimum negative delay (INT32_MIN)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationProtocolTest, ANIMATION_TEST, Delay_Abnormal_MinNegative)
{
    auto pointModifier = std::make_shared<PointCustomModifier>();
    CreateTestNode(pointModifier);
    pointModifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(800);
    protocol.SetStartDelay(INT32_MIN); // Abnormal: minimum negative value
    CreateBasicAnimation(pointModifier, protocol);
}

// ============================================================================
// Section 4: Multi-Scenario Combination Tests (多场景组合测试)
// ============================================================================

/*
 * @tc.name: Combo_Repeat_AutoReverse_Enabled
 * @tc.desc: Test combination of repeat count and auto reverse enabled
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationProtocolTest, ANIMATION_TEST, Combo_Repeat_AutoReverse_Enabled)
{
    auto pointModifier = std::make_shared<PointCustomModifier>();
    CreateTestNode(pointModifier);
    pointModifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(400);
    protocol.SetRepeatCount(3); // Multiple repeats
    protocol.SetAutoReverse(true); // With auto reverse
    CreateBasicAnimation(pointModifier, protocol);
}

/*
 * @tc.name: Combo_Repeat_AutoReverse_Disabled
 * @tc.desc: Test combination of repeat count without auto reverse
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationProtocolTest, ANIMATION_TEST, Combo_Repeat_AutoReverse_Disabled)
{
    auto pointModifier = std::make_shared<PointCustomModifier>();
    CreateTestNode(pointModifier);
    pointModifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(400);
    protocol.SetRepeatCount(3); // Multiple repeats
    protocol.SetAutoReverse(false); // Without auto reverse
    CreateBasicAnimation(pointModifier, protocol);
}

/*
 * @tc.name: Combo_Direction_Reverse_Repeat
 * @tc.desc: Test combination of reverse direction with repeat
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationProtocolTest, ANIMATION_TEST, Combo_Direction_Reverse_Repeat)
{
    auto pointModifier = std::make_shared<PointCustomModifier>();
    CreateTestNode(pointModifier);
    pointModifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(400);
    protocol.SetDirection(false); // Reverse direction
    protocol.SetRepeatCount(2); // With repeat
    CreateBasicAnimation(pointModifier, protocol);
}

/*
 * @tc.name: Combo_Direction_Reverse_AutoReverse
 * @tc.desc: Test combination of reverse direction with auto reverse
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationProtocolTest, ANIMATION_TEST, Combo_Direction_Reverse_AutoReverse)
{
    auto pointModifier = std::make_shared<PointCustomModifier>();
    CreateTestNode(pointModifier);
    pointModifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(400);
    protocol.SetDirection(false); // Reverse direction
    protocol.SetAutoReverse(true); // With auto reverse
    protocol.SetRepeatCount(2);
    CreateBasicAnimation(pointModifier, protocol);
}

/*
 * @tc.name: Combo_Speed_Duration_Slow_Long
 * @tc.desc: Test combination of slow speed with long duration
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationProtocolTest, ANIMATION_TEST, Combo_Speed_Duration_Slow_Long)
{
    auto pointModifier = std::make_shared<PointCustomModifier>();
    CreateTestNode(pointModifier);
    pointModifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(1500); // Long duration
    protocol.SetSpeed(0.5f); // Slow speed
    CreateBasicAnimation(pointModifier, protocol);
}

/*
 * @tc.name: Combo_Speed_Duration_Fast_Short
 * @tc.desc: Test combination of fast speed with short duration
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationProtocolTest, ANIMATION_TEST, Combo_Speed_Duration_Fast_Short)
{
    auto pointModifier = std::make_shared<PointCustomModifier>();
    CreateTestNode(pointModifier);
    pointModifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(100); // Short duration
    protocol.SetSpeed(5.0f); // Very fast speed
    CreateBasicAnimation(pointModifier, protocol);
}

/*
 * @tc.name: Combo_Delay_Repeat_Multiple
 * @tc.desc: Test combination of delay with multiple repeats
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationProtocolTest, ANIMATION_TEST, Combo_Delay_Repeat_Multiple)
{
    auto pointModifier = std::make_shared<PointCustomModifier>();
    CreateTestNode(pointModifier);
    pointModifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(300);
    protocol.SetStartDelay(200); // With delay
    protocol.SetRepeatCount(3); // Multiple repeats
    CreateBasicAnimation(pointModifier, protocol);
}

/*
 * @tc.name: Combo_All_Parameters_Complex
 * @tc.desc: Test complex combination of all parameters
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationProtocolTest, ANIMATION_TEST, Combo_All_Parameters_Complex)
{
    auto pointModifier = std::make_shared<PointCustomModifier>();
    CreateTestNode(pointModifier);
    pointModifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(500);
    protocol.SetStartDelay(100);
    protocol.SetSpeed(1.5f);
    protocol.SetRepeatCount(2);
    protocol.SetAutoReverse(true);
    protocol.SetDirection(true);
    protocol.SetFinishCallbackType(FinishCallbackType::TIME_SENSITIVE);
    CreateBasicAnimation(pointModifier, protocol);
}

/*
 * @tc.name: Combo_Zero_Repeat_AutoReverse
 * @tc.desc: Test zero repeat with auto reverse (edge case)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationProtocolTest, ANIMATION_TEST, Combo_Zero_Repeat_AutoReverse)
{
    auto pointModifier = std::make_shared<PointCustomModifier>();
    CreateTestNode(pointModifier);
    pointModifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(400);
    protocol.SetRepeatCount(0); // Zero repeat
    protocol.SetAutoReverse(true); // With auto reverse (edge case)
    CreateBasicAnimation(pointModifier, protocol);
}

/*
 * @tc.name: Combo_Delay_FasterThanDuration
 * @tc.desc: Test delay longer than animation duration
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationProtocolTest, ANIMATION_TEST, Combo_Delay_FasterThanDuration)
{
    auto pointModifier = std::make_shared<PointCustomModifier>();
    CreateTestNode(pointModifier);
    pointModifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(200);
    protocol.SetStartDelay(500); // Delay > duration
    CreateBasicAnimation(pointModifier, protocol);
}

/*
 * @tc.name: Combo_VerySlow_VeryLongDuration
 * @tc.desc: Test very slow speed with very long duration
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationProtocolTest, ANIMATION_TEST, Combo_VerySlow_VeryLongDuration)
{
    auto pointModifier = std::make_shared<PointCustomModifier>();
    CreateTestNode(pointModifier);
    pointModifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(2000); // Long duration
    protocol.SetSpeed(0.7f); // Slow speed
    CreateBasicAnimation(pointModifier, protocol);
}

/*
 * @tc.name: Combo_Reverse_Direction_Repeat_AutoReverse
 * @tc.desc: Test triple combination: reverse + repeat + auto reverse
 * @tc.type: FUNC
 */
GRAPHIC_TEST(AnimationProtocolTest, ANIMATION_TEST, Combo_Reverse_Direction_Repeat_AutoReverse)
{
    auto pointModifier = std::make_shared<PointCustomModifier>();
    CreateTestNode(pointModifier);
    pointModifier->SetPosition(100);

    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(400);
    protocol.SetDirection(false); // Reverse
    protocol.SetRepeatCount(2); // Repeat
    protocol.SetAutoReverse(true); // Auto reverse
    CreateBasicAnimation(pointModifier, protocol);
}

} // namespace OHOS::Rosen
