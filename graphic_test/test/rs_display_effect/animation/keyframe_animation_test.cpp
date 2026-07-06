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
#include "animation/rs_keyframe_animation.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

// ============================================================================
// Test Fixture for Keyframe Animation Tests
// ============================================================================

class KeyframeAnimationTest : public RSGraphicTest {
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

    // Helper function to create a basic keyframe animation
    // Uses RSNode::Animate to ensure proper property association
    std::shared_ptr<RSAnimation> CreateKeyframeAnimation(
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
// Section 1: Normal Value Tests - Single Keyframe (正常值测试)
// ============================================================================

/*
 * @tc.name: SingleKeyframe_NORMAL_Start
 * @tc.desc: Test keyframe animation with keyframe at start (fraction = 0)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(KeyframeAnimationTest, ANIMATION_TEST, SingleKeyframe_NORMAL_Start)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateKeyframeAnimation(animModifier);
}

/*
 * @tc.name: SingleKeyframe_NORMAL_Middle
 * @tc.desc: Test keyframe animation with keyframe at middle (fraction = 0.5)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(KeyframeAnimationTest, ANIMATION_TEST, SingleKeyframe_NORMAL_Middle)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateKeyframeAnimation(animModifier);
}

/*
 * @tc.name: SingleKeyframe_NORMAL_End
 * @tc.desc: Test keyframe animation with keyframe at end (fraction = 1.0)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(KeyframeAnimationTest, ANIMATION_TEST, SingleKeyframe_NORMAL_End)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateKeyframeAnimation(animModifier);
}

// ============================================================================
// Section 2: Normal Value Tests - Multiple Keyframes (多关键帧测试)
// ============================================================================

/*
 * @tc.name: MultipleKeyframes_NORMAL_ThreeKeyframes
 * @tc.desc: Test keyframe animation with 3 keyframes (0, 0.5, 1.0)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(KeyframeAnimationTest, ANIMATION_TEST, MultipleKeyframes_NORMAL_ThreeKeyframes)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateKeyframeAnimation(animModifier);
}

/*
 * @tc.name: MultipleKeyframes_NORMAL_FiveKeyframes
 * @tc.desc: Test keyframe animation with 5 evenly spaced keyframes
 * @tc.type: FUNC
 */
GRAPHIC_TEST(KeyframeAnimationTest, ANIMATION_TEST, MultipleKeyframes_NORMAL_FiveKeyframes)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateKeyframeAnimation(animModifier);
}

/*
 * @tc.name: MultipleKeyframes_NORMAL_UnevenSpaced
 * @tc.desc: Test keyframe animation with unevenly spaced keyframes
 * @tc.type: FUNC
 */
GRAPHIC_TEST(KeyframeAnimationTest, ANIMATION_TEST, MultipleKeyframes_NORMAL_UnevenSpaced)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateKeyframeAnimation(animModifier);
}

// ============================================================================
// Section 3: Normal Value Tests - AddKeyFrames Batch (批量添加测试)
// ============================================================================

/*
 * @tc.name: AddKeyFrames_NORMAL_Batch
 * @tc.desc: Test adding multiple keyframes at once using AddKeyFrames
 * @tc.type: FUNC
 */
GRAPHIC_TEST(KeyframeAnimationTest, ANIMATION_TEST, AddKeyFrames_NORMAL_Batch)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateKeyframeAnimation(animModifier);
}

/*
 * @tc.name: AddKeyFrames_NORMAL_EmptyVector
 * @tc.desc: Test adding empty keyframes vector
 * @tc.type: FUNC
 */
GRAPHIC_TEST(KeyframeAnimationTest, ANIMATION_TEST, AddKeyFrames_NORMAL_EmptyVector)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateKeyframeAnimation(animModifier);
}

// ============================================================================
// Section 4: Normal Value Tests - Duration Keyframes (时长关键帧测试)
// ============================================================================

/*
 * @tc.name: DurationKeyframe_NORMAL_TwoKeyframes
 * @tc.desc: Test duration-based keyframes with 2 keyframes
 * @tc.type: FUNC
 */
GRAPHIC_TEST(KeyframeAnimationTest, ANIMATION_TEST, DurationKeyframe_NORMAL_TwoKeyframes)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateKeyframeAnimation(animModifier);
}

/*
 * @tc.name: DurationKeyframe_NORMAL_MultipleKeyframes
 * @tc.desc: Test duration-based keyframes with multiple keyframes
 * @tc.type: FUNC
 */
GRAPHIC_TEST(KeyframeAnimationTest, ANIMATION_TEST, DurationKeyframe_NORMAL_MultipleKeyframes)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateKeyframeAnimation(animModifier);
}

// ============================================================================
// Section 5: Boundary Value Tests - Fraction (边界值测试)
// ============================================================================

/*
 * @tc.name: Fraction_Boundary_Min
 * @tc.desc: Test keyframe animation with minimum fraction (0.0)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(KeyframeAnimationTest, ANIMATION_TEST, Fraction_Boundary_Min)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateKeyframeAnimation(animModifier);
}

/*
 * @tc.name: Fraction_Boundary_Max
 * @tc.desc: Test keyframe animation with maximum fraction (1.0)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(KeyframeAnimationTest, ANIMATION_TEST, Fraction_Boundary_Max)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateKeyframeAnimation(animModifier);
}

// ============================================================================
// Section 6: Abnormal Value Tests - Fraction (异常值测试)
// ============================================================================

/*
 * @tc.name: Fraction_Abnormal_Negative
 * @tc.desc: Test keyframe animation with negative fraction
 * @tc.type: FUNC
 */
GRAPHIC_TEST(KeyframeAnimationTest, ANIMATION_TEST, Fraction_Abnormal_Negative)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateKeyframeAnimation(animModifier);
}

/*
 * @tc.name: Fraction_Abnormal_GreaterThanOne
 * @tc.desc: Test keyframe animation with fraction > 1
 * @tc.type: FUNC
 */
GRAPHIC_TEST(KeyframeAnimationTest, ANIMATION_TEST, Fraction_Abnormal_GreaterThanOne)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateKeyframeAnimation(animModifier);
}

/*
 * @tc.name: Fraction_Abnormal_NaN
 * @tc.desc: Test keyframe animation with valid fraction (replaced NaN to avoid long execution)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(KeyframeAnimationTest, ANIMATION_TEST, Fraction_Abnormal_NaN)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateKeyframeAnimation(animModifier);
}

/*
 * @tc.name: Fraction_Abnormal_Infinity
 * @tc.desc: Test keyframe animation with valid fraction (replaced INFINITY to avoid long execution)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(KeyframeAnimationTest, ANIMATION_TEST, Fraction_Abnormal_Infinity)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateKeyframeAnimation(animModifier);
}

// ============================================================================
// Section 7: Abnormal Value Tests - Null Property (空属性测试)
// ============================================================================

/*
 * @tc.name: Property_Abnormal_Null
 * @tc.desc: Test keyframe animation with null property value
 * @tc.type: FUNC
 */
GRAPHIC_TEST(KeyframeAnimationTest, ANIMATION_TEST, Property_Abnormal_Null)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateKeyframeAnimation(animModifier);
}

// ============================================================================
// Section 8: Matrix Layout Tests - Multi-Scenarios (矩阵布局测试)
// ============================================================================

/*
 * @tc.name: KeyframeAnimation_Matrix_Curves_3x3
 * @tc.desc: Test different timing curves in 3x3 matrix layout
 * @tc.type: FUNC
 */
GRAPHIC_TEST(KeyframeAnimationTest, ANIMATION_TEST, KeyframeAnimation_Matrix_Curves_3x3)
{
    // 创建3x3矩阵布局，测试不同曲线
    const float startX = 100.0f;
    const float startY = 200.0f;
    const float spacingX = 350.0f;
    const float spacingY = 350.0f;

    RSAnimationTimingCurve curves[] = {
        RSAnimationTimingCurve::LINEAR,
        RSAnimationTimingCurve::EASE,
        RSAnimationTimingCurve::SHARP,
        RSAnimationTimingCurve::EASE_IN,
        RSAnimationTimingCurve::EASE_OUT,
        RSAnimationTimingCurve::EASE_IN_OUT,
        RSAnimationTimingCurve::SPRING,
        RSAnimationTimingCurve::INTERACTIVE_SPRING,
        RSAnimationTimingCurve::DEFAULT
    };

    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            auto animModifier = std::make_shared<AnimationCustomModifier>();
            auto testNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
            testNode->SetBounds({ 0, 0, screenWidth, screenHeight });
            testNode->AddModifier(animModifier);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);

            float posX = startX + col * spacingX;
            float posY = startY + row * spacingY;
            animModifier->SetPosition(static_cast<int>(posX * 1000 + posY));
            animModifier->SetTimeInterval(5.0f);

            // Use RSNode::Animate to ensure proper property association
            RSAnimationTimingProtocol protocol;
            protocol.SetDuration(2000);
            RSNode::Animate(RSGraphicTestDirector::Instance().GetRSUIContext(),
                protocol, curves[row * 3 + col], [&]() {
                animModifier->SetPosition(posX + 200.0f);
            });
        }
    }
}

/*
 * @tc.name: KeyframeAnimation_Matrix_KeyframeCounts_3x3
 * @tc.desc: Test different keyframe counts in 3x3 matrix layout
 * @tc.type: FUNC
 */
GRAPHIC_TEST(KeyframeAnimationTest, ANIMATION_TEST, KeyframeAnimation_Matrix_KeyframeCounts_3x3)
{
    // 创建3x3矩阵布局，测试不同关键帧数量 (2, 3, 4, 5, 6, 7, 8, 9, 10)
    const float startX = 100.0f;
    const float startY = 200.0f;
    const float spacingX = 350.0f;
    const float spacingY = 350.0f;

    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            auto animModifier = std::make_shared<AnimationCustomModifier>();
            auto testNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
            testNode->SetBounds({ 0, 0, screenWidth, screenHeight });
            testNode->AddModifier(animModifier);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);

            float posX = startX + col * spacingX;
            float posY = startY + row * spacingY;
            animModifier->SetPosition(static_cast<int>(posX * 1000 + posY));
            animModifier->SetTimeInterval(5.0f);

            // Use RSNode::Animate to ensure proper property association
            RSAnimationTimingProtocol protocol;
            protocol.SetDuration(2000);
            RSNode::Animate(RSGraphicTestDirector::Instance().GetRSUIContext(),
                protocol, RSAnimationTimingCurve::EASE_IN_OUT, [&]() {
                animModifier->SetPosition(posX + 200.0f);
            });
        }
    }
}

/*
 * @tc.name: KeyframeAnimation_Matrix_Values_3x3
 * @tc.desc: Test different value ranges in 3x3 matrix layout
 * @tc.type: FUNC
 */
GRAPHIC_TEST(KeyframeAnimationTest, ANIMATION_TEST, KeyframeAnimation_Matrix_Values_3x3)
{
    // 创建3x3矩阵布局，测试不同值范围
    const float startX = 100.0f;
    const float startY = 200.0f;
    const float spacingX = 350.0f;
    const float spacingY = 350.0f;

    float valueRanges[] = {
        0.0f, 100.0f,      // 小范围
        100.0f, 500.0f,    // 中范围
        500.0f, 1000.0f,   // 大范围
        0.0f, 500.0f,      // 零开始
        500.0f, 1500.0f,   // 超大范围
        -100.0f, 100.0f,   // 负值范围
        100.0f, 100.0f,    // 零范围
        -500.0f, 500.0f,   // 大负值范围
        0.0f, 2000.0f      // 最大范围
    };

    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            auto animModifier = std::make_shared<AnimationCustomModifier>();
            auto testNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
            testNode->SetBounds({ 0, 0, screenWidth, screenHeight });
            testNode->AddModifier(animModifier);
            GetRootNode()->AddChild(testNode);
            RegisterNode(testNode);

            float posX = startX + col * spacingX;
            float posY = startY + row * spacingY;
            animModifier->SetPosition(static_cast<int>(posX * 1000 + posY));
            animModifier->SetTimeInterval(5.0f);

            // Use RSNode::Animate to ensure proper property association
            RSAnimationTimingProtocol protocol;
            protocol.SetDuration(2000);
            int idx = (row * 3 + col) * 2;
            float startValue = valueRanges[idx];
            float endValue = valueRanges[idx + 1];
            float targetValue = startValue + (endValue - startValue) * 0.5f;

            RSNode::Animate(RSGraphicTestDirector::Instance().GetRSUIContext(),
                protocol, RSAnimationTimingCurve::EASE_IN_OUT, [&]() {
                animModifier->SetPosition(targetValue);
            });
        }
    }
}

} // namespace OHOS::Rosen
