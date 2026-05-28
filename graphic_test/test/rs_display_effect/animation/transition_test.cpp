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
#include "animation/rs_transition.h"
#include "animation/rs_transition_effect.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

// ============================================================================
// Test Fixture for Transition Tests
// ============================================================================

class TransitionTest : public RSGraphicTest {
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

    // Helper function to create position animation to ensure visual effect
    void AddPositionAnimation(std::shared_ptr<AnimationCustomModifier> modifier)
    {
        RSAnimationTimingProtocol protocol;
        protocol.SetDuration(1000);
        RSNode::Animate(RSGraphicTestDirector::Instance().GetRSUIContext(),
            protocol, RSAnimationTimingCurve::EASE_IN_OUT, [&]() {
            modifier->SetPosition(1000.0f);
        });
    }
};

// ============================================================================
// Section 1: Normal Value Tests - Built-in Effects (正常值测试)
// ============================================================================

/*
 * @tc.name: BuiltInEffect_EMPTY_TransitionIn
 * @tc.desc: Test transition with EMPTY effect (transition in)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(TransitionTest, ANIMATION_TEST, BuiltInEffect_EMPTY_TransitionIn)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto transition = std::make_shared<RSTransition>(nullptr, RSTransitionEffect::EMPTY, true);
    testNode->AddAnimation(transition);
    AddPositionAnimation(animModifier);
}

/*
 * @tc.name: BuiltInEffect_EMPTY_TransitionOut
 * @tc.desc: Test transition with EMPTY effect (transition out)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(TransitionTest, ANIMATION_TEST, BuiltInEffect_EMPTY_TransitionOut)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto transition = std::make_shared<RSTransition>(nullptr, RSTransitionEffect::EMPTY, false);
    testNode->AddAnimation(transition);
    AddPositionAnimation(animModifier);
}

/*
 * @tc.name: BuiltInEffect_OPACITY_TransitionOut
 * @tc.desc: Test transition with OPACITY effect (transition out)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(TransitionTest, ANIMATION_TEST, BuiltInEffect_OPACITY_TransitionOut)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto transition = std::make_shared<RSTransition>(nullptr, RSTransitionEffect::OPACITY, false);
    testNode->AddAnimation(transition);
    AddPositionAnimation(animModifier);
}

/*
 * @tc.name: BuiltInEffect_SCALE_TransitionOut
 * @tc.desc: Test transition with SCALE effect (transition out)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(TransitionTest, ANIMATION_TEST, BuiltInEffect_SCALE_TransitionOut)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto transition = std::make_shared<RSTransition>(nullptr, RSTransitionEffect::SCALE, false);
    testNode->AddAnimation(transition);
    AddPositionAnimation(animModifier);
}

// ============================================================================
// Section 2: Normal Value Tests - Custom Effects (自定义效果测试)
// ============================================================================

/*
 * @tc.name: CustomEffect_OPACITY_Default
 * @tc.desc: Test custom transition with OPACITY effect (default opacity)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(TransitionTest, ANIMATION_TEST, CustomEffect_OPACITY_Default)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto effect = RSTransitionEffect::Create()->Opacity();
    auto transition = std::make_shared<RSTransition>(nullptr, effect, true);
    testNode->AddAnimation(transition);
    AddPositionAnimation(animModifier);
}

/*
 * @tc.name: CustomEffect_OPACITY_HalfOpacity
 * @tc.desc: Test custom transition with OPACITY effect (opacity = 0.5)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(TransitionTest, ANIMATION_TEST, CustomEffect_OPACITY_HalfOpacity)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto effect = RSTransitionEffect::Create()->Opacity(0.5f);
    auto transition = std::make_shared<RSTransition>(nullptr, effect, true);
    testNode->AddAnimation(transition);
    AddPositionAnimation(animModifier);
}

/*
 * @tc.name: CustomEffect_OPACITY_FullOpacity
 * @tc.desc: Test custom transition with OPACITY effect (opacity = 1.0)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(TransitionTest, ANIMATION_TEST, CustomEffect_OPACITY_FullOpacity)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto effect = RSTransitionEffect::Create()->Opacity(1.0f);
    auto transition = std::make_shared<RSTransition>(nullptr, effect, false);
    testNode->AddAnimation(transition);
    AddPositionAnimation(animModifier);
}

/*
 * @tc.name: CustomEffect_SCALE_Uniform
 * @tc.desc: Test custom transition with uniform SCALE effect
 * @tc.type: FUNC
 */
GRAPHIC_TEST(TransitionTest, ANIMATION_TEST, CustomEffect_SCALE_Uniform)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto effect = RSTransitionEffect::Create()->Scale({ 1.0f, 1.0f, 1.0f });
    auto transition = std::make_shared<RSTransition>(nullptr, effect, true);
    testNode->AddAnimation(transition);
    AddPositionAnimation(animModifier);
}

/*
 * @tc.name: CustomEffect_SCALE_NonUniform
 * @tc.desc: Test custom transition with non-uniform SCALE effect
 * @tc.type: FUNC
 */
GRAPHIC_TEST(TransitionTest, ANIMATION_TEST, CustomEffect_SCALE_NonUniform)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto effect = RSTransitionEffect::Create()->Scale({ 2.0f, 1.5f, 0.5f });
    auto transition = std::make_shared<RSTransition>(nullptr, effect, true);
    testNode->AddAnimation(transition);
    AddPositionAnimation(animModifier);
}

/*
 * @tc.name: CustomEffect_TRANSLATE_Positive
 * @tc.desc: Test custom transition with positive TRANSLATE effect
 * @tc.type: FUNC
 */
GRAPHIC_TEST(TransitionTest, ANIMATION_TEST, CustomEffect_TRANSLATE_Positive)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto effect = RSTransitionEffect::Create()->Translate({ 100.0f, 200.0f, 0.0f });
    auto transition = std::make_shared<RSTransition>(nullptr, effect, true);
    testNode->AddAnimation(transition);
    AddPositionAnimation(animModifier);
}

/*
 * @tc.name: CustomEffect_TRANSLATE_Negative
 * @tc.desc: Test custom transition with negative TRANSLATE effect
 * @tc.type: FUNC
 */
GRAPHIC_TEST(TransitionTest, ANIMATION_TEST, CustomEffect_TRANSLATE_Negative)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto effect = RSTransitionEffect::Create()->Translate({ -100.0f, -200.0f, 0.0f });
    auto transition = std::make_shared<RSTransition>(nullptr, effect, true);
    testNode->AddAnimation(transition);
    AddPositionAnimation(animModifier);
}

/*
 * @tc.name: CustomEffect_ROTATE_XAxis
 * @tc.desc: Test custom transition with X-axis rotation
 * @tc.type: FUNC
 */
GRAPHIC_TEST(TransitionTest, ANIMATION_TEST, CustomEffect_ROTATE_XAxis)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto effect = RSTransitionEffect::Create()->Rotate({ 1.0f, 0.0f, 0.0f, 90.0f });
    auto transition = std::make_shared<RSTransition>(nullptr, effect, true);
    testNode->AddAnimation(transition);
    AddPositionAnimation(animModifier);
}

/*
 * @tc.name: CustomEffect_ROTATE_YAxis
 * @tc.desc: Test custom transition with Y-axis rotation
 * @tc.type: FUNC
 */
GRAPHIC_TEST(TransitionTest, ANIMATION_TEST, CustomEffect_ROTATE_YAxis)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto effect = RSTransitionEffect::Create()->Rotate({ 0.0f, 1.0f, 0.0f, 180.0f });
    auto transition = std::make_shared<RSTransition>(nullptr, effect, true);
    testNode->AddAnimation(transition);
    AddPositionAnimation(animModifier);
}

/*
 * @tc.name: CustomEffect_ROTATE_ZAxis
 * @tc.desc: Test custom transition with Z-axis rotation
 * @tc.type: FUNC
 */
GRAPHIC_TEST(TransitionTest, ANIMATION_TEST, CustomEffect_ROTATE_ZAxis)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto effect = RSTransitionEffect::Create()->Rotate({ 0.0f, 0.0f, 1.0f, 360.0f });
    auto transition = std::make_shared<RSTransition>(nullptr, effect, true);
    testNode->AddAnimation(transition);
    AddPositionAnimation(animModifier);
}

// ============================================================================
// Section 3: Normal Value Tests - Combined Effects (组合效果测试)
// ============================================================================

/*
 * @tc.name: CombinedEffect_OPACITY_TRANSLATE
 * @tc.desc: Test transition with OPACITY + TRANSLATE combined effect
 * @tc.type: FUNC
 */
GRAPHIC_TEST(TransitionTest, ANIMATION_TEST, CombinedEffect_OPACITY_TRANSLATE)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto effect = RSTransitionEffect::Create()->Opacity(0.3f)->Translate({ 200.0f, 0.0f, 0.0f });
    auto transition = std::make_shared<RSTransition>(nullptr, effect, true);
    testNode->AddAnimation(transition);
    AddPositionAnimation(animModifier);
}

/*
 * @tc.name: CombinedEffect_SCALE_ROTATE
 * @tc.desc: Test transition with SCALE + ROTATE combined effect
 * @tc.type: FUNC
 */
GRAPHIC_TEST(TransitionTest, ANIMATION_TEST, CombinedEffect_SCALE_ROTATE)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto effect = RSTransitionEffect::Create()->Scale({ 2.0f, 2.0f, 1.0f })->Rotate({ 0.0f, 0.0f, 1.0f, 180.0f });
    auto transition = std::make_shared<RSTransition>(nullptr, effect, true);
    testNode->AddAnimation(transition);
    AddPositionAnimation(animModifier);
}

// ============================================================================
// Section 4: Normal Value Tests - Asymmetric Transition (非对称转场测试)
// ============================================================================

/*
 * @tc.name: Asymmetric_SCALE_TRANSLATE
 * @tc.desc: Test asymmetric transition: SCALE (in) + TRANSLATE (out)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(TransitionTest, ANIMATION_TEST, Asymmetric_SCALE_TRANSLATE)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto transitionIn = RSTransitionEffect::Create()->Scale({ 1.5f, 1.5f, 1.0f });
    auto transitionOut = RSTransitionEffect::Create()->Translate({ -200.0f, 0.0f, 0.0f });
    auto effect = RSTransitionEffect::Asymmetric(transitionIn, transitionOut);
    auto transition = std::make_shared<RSTransition>(nullptr, effect, true);
    testNode->AddAnimation(transition);
    AddPositionAnimation(animModifier);
}

// ============================================================================
// Section 5: Normal Value Tests - SetTimingCurve (设置曲线测试)
// ============================================================================

/*
 * @tc.name: SetTimingCurve_NORMAL_Linear
 * @tc.desc: Test transition with LINEAR timing curve
 * @tc.type: FUNC
 */
GRAPHIC_TEST(TransitionTest, ANIMATION_TEST, SetTimingCurve_NORMAL_Linear)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto transition = std::make_shared<RSTransition>(nullptr, RSTransitionEffect::OPACITY, true);
    transition->SetTimingCurve(RSAnimationTimingCurve::LINEAR);
    testNode->AddAnimation(transition);
    AddPositionAnimation(animModifier);
}

/*
 * @tc.name: SetTimingCurve_NORMAL_EaseInOut
 * @tc.desc: Test transition with EASE_IN_OUT timing curve
 * @tc.type: FUNC
 */
GRAPHIC_TEST(TransitionTest, ANIMATION_TEST, SetTimingCurve_NORMAL_EaseInOut)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto transition = std::make_shared<RSTransition>(nullptr, RSTransitionEffect::SCALE, false);
    transition->SetTimingCurve(RSAnimationTimingCurve::EASE_IN_OUT);
    testNode->AddAnimation(transition);
    AddPositionAnimation(animModifier);
}

// ============================================================================
// Section 6: Boundary Value Tests - Opacity (边界值测试)
// ============================================================================

/*
 * @tc.name: Opacity_Boundary_One
 * @tc.desc: Test transition with opacity = 1 (fully opaque)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(TransitionTest, ANIMATION_TEST, Opacity_Boundary_One)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto effect = RSTransitionEffect::Create()->Opacity(1.0f);
    auto transition = std::make_shared<RSTransition>(nullptr, effect, false);
    testNode->AddAnimation(transition);
    AddPositionAnimation(animModifier);
}

// ============================================================================
// Section 7: Abnormal Value Tests - Opacity (异常值测试)
// ============================================================================

/*
 * @tc.name: Opacity_Abnormal_GreaterThanOne
 * @tc.desc: Test transition with opacity > 1
 * @tc.type: FUNC
 */
GRAPHIC_TEST(TransitionTest, ANIMATION_TEST, Opacity_Abnormal_GreaterThanOne)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto effect = RSTransitionEffect::Create()->Opacity(1.5f);
    auto transition = std::make_shared<RSTransition>(nullptr, effect, true);
    testNode->AddAnimation(transition);
    AddPositionAnimation(animModifier);
}

// ============================================================================
// Section 8: Abnormal Value Tests - Scale (缩放异常值测试)
// ============================================================================

/*
 * @tc.name: Scale_Abnormal_Zero
 * @tc.desc: Test transition with zero scale
 * @tc.type: FUNC
 */
GRAPHIC_TEST(TransitionTest, ANIMATION_TEST, Scale_Abnormal_Zero)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto effect = RSTransitionEffect::Create()->Scale({ 0.0f, 0.0f, 0.0f });
    auto transition = std::make_shared<RSTransition>(nullptr, effect, true);
    testNode->AddAnimation(transition);
    AddPositionAnimation(animModifier);
}

/*
 * @tc.name: Scale_Abnormal_Negative
 * @tc.desc: Test transition with negative scale (flip effect)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(TransitionTest, ANIMATION_TEST, Scale_Abnormal_Negative)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto effect = RSTransitionEffect::Create()->Scale({ -1.0f, -1.0f, 1.0f });
    auto transition = std::make_shared<RSTransition>(nullptr, effect, true);
    testNode->AddAnimation(transition);
    AddPositionAnimation(animModifier);
}

/*
 * @tc.name: Scale_Abnormal_VeryLarge
 * @tc.desc: Test transition with very large scale
 * @tc.type: FUNC
 */
GRAPHIC_TEST(TransitionTest, ANIMATION_TEST, Scale_Abnormal_VeryLarge)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto effect = RSTransitionEffect::Create()->Scale({ 100.0f, 100.0f, 1.0f });
    auto transition = std::make_shared<RSTransition>(nullptr, effect, true);
    testNode->AddAnimation(transition);
    AddPositionAnimation(animModifier);
}

// ============================================================================
// Section 9: Matrix Layout Tests - Multi-Scenarios (矩阵布局测试)
// ============================================================================

/*
 * @tc.name: Transition_Matrix_BuiltInEffects_2x2
 * @tc.desc: Test built-in effects in 2x2 matrix layout
 * @tc.type: FUNC
 */
GRAPHIC_TEST(TransitionTest, ANIMATION_TEST, Transition_Matrix_BuiltInEffects_2x2)
{
    // 创建2x2矩阵布局，测试内置转场效果
    const float startX = 100.0f;
    const float startY = 300.0f;
    const float spacingX = 500.0f;
    const float spacingY = 500.0f;

    std::shared_ptr<const RSTransitionEffect> effects[] = {
        RSTransitionEffect::EMPTY,
        RSTransitionEffect::OPACITY,
        RSTransitionEffect::SCALE,
        RSTransitionEffect::OPACITY
    };

    bool isIn[] = { true, true, false, false };

    for (int row = 0; row < 2; row++) {
        for (int col = 0; col < 2; col++) {
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

            auto transition = std::make_shared<RSTransition>(nullptr, effects[row * 2 + col], isIn[row * 2 + col]);
            testNode->AddAnimation(transition);

            RSAnimationTimingProtocol protocol;
            protocol.SetDuration(1000);
            RSNode::Animate(RSGraphicTestDirector::Instance().GetRSUIContext(),
                protocol, RSAnimationTimingCurve::EASE_IN_OUT, [&]() {
                animModifier->SetPosition(1000.0f);
            });
        }
    }
}

/*
 * @tc.name: Transition_Matrix_OpacityValues_3x3
 * @tc.desc: Test different opacity values in 3x3 matrix layout
 * @tc.type: FUNC
 */
GRAPHIC_TEST(TransitionTest, ANIMATION_TEST, Transition_Matrix_OpacityValues_3x3)
{
    // 创建3x3矩阵布局，测试不同透明度值
    const float startX = 100.0f;
    const float startY = 200.0f;
    const float spacingX = 350.0f;
    const float spacingY = 350.0f;

    float opacities[] = {
        0.0f, 0.25f, 0.5f,
        0.75f, 1.0f, -0.5f,
        1.5f, NAN, INFINITY
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

            auto effect = RSTransitionEffect::Create()->Opacity(opacities[row * 3 + col]);
            auto transition = std::make_shared<RSTransition>(nullptr, effect, true);
            testNode->AddAnimation(transition);

            RSAnimationTimingProtocol protocol;
            protocol.SetDuration(1000);
            RSNode::Animate(RSGraphicTestDirector::Instance().GetRSUIContext(),
                protocol, RSAnimationTimingCurve::EASE_IN_OUT, [&]() {
                animModifier->SetPosition(1000.0f);
            });
        }
    }
}

/*
 * @tc.name: Transition_Matrix_RotationAngles_3x3
 * @tc.desc: Test different rotation angles in 3x3 matrix layout
 * @tc.type: FUNC
 */
GRAPHIC_TEST(TransitionTest, ANIMATION_TEST, Transition_Matrix_RotationAngles_3x3)
{
    // 创建3x3矩阵布局，测试不同旋转角度
    const float startX = 100.0f;
    const float startY = 200.0f;
    const float spacingX = 350.0f;
    const float spacingY = 350.0f;

    float angles[] = {
        0.0f, 45.0f, 90.0f,
        135.0f, 180.0f, 225.0f,
        270.0f, 315.0f, 360.0f
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

            auto effect = RSTransitionEffect::Create()->Rotate({ 0.0f, 0.0f, 1.0f, angles[row * 3 + col] });
            auto transition = std::make_shared<RSTransition>(nullptr, effect, true);
            testNode->AddAnimation(transition);

            RSAnimationTimingProtocol protocol;
            protocol.SetDuration(1000);
            RSNode::Animate(RSGraphicTestDirector::Instance().GetRSUIContext(),
                protocol, RSAnimationTimingCurve::EASE_IN_OUT, [&]() {
                animModifier->SetPosition(1000.0f);
            });
        }
    }
}

/*
 * @tc.name: Transition_Matrix_ScaleValues_3x3
 * @tc.desc: Test different scale values in 3x3 matrix layout
 * @tc.type: FUNC
 */
GRAPHIC_TEST(TransitionTest, ANIMATION_TEST, Transition_Matrix_ScaleValues_3x3)
{
    // 创建3x3矩阵布局，测试不同缩放值
    const float startX = 100.0f;
    const float startY = 200.0f;
    const float spacingX = 350.0f;
    const float spacingY = 350.0f;

    float scales[] = {
        0.0f, 0.25f, 0.5f,
        0.75f, 1.0f, 1.5f,
        2.0f, 5.0f, 10.0f
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

            float s = scales[row * 3 + col];
            auto effect = RSTransitionEffect::Create()->Scale({ s, s, 1.0f });
            auto transition = std::make_shared<RSTransition>(nullptr, effect, true);
            testNode->AddAnimation(transition);

            RSAnimationTimingProtocol protocol;
            protocol.SetDuration(1000);
            RSNode::Animate(RSGraphicTestDirector::Instance().GetRSUIContext(),
                protocol, RSAnimationTimingCurve::EASE_IN_OUT, [&]() {
                animModifier->SetPosition(1000.0f);
            });
        }
    }
}

} // namespace OHOS::Rosen
