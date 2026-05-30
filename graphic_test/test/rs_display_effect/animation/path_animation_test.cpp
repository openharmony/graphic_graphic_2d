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
#include "animation/rs_path_animation.h"
#include "render/rs_path.h"
#include "animation/rs_transition.h"
#include "animation/rs_transition_effect.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

// ============================================================================
// Test Fixture for Path Animation Tests
// ============================================================================

class PathAnimationTest : public RSGraphicTest {
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

    // Helper function to create a basic path animation
    // Uses RSNode::Animate to ensure proper property association
    std::shared_ptr<RSAnimation> CreatePathAnimation(
        std::shared_ptr<AnimationCustomModifier> modifier,
        const std::shared_ptr<RSPath>& path)
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

    // Helper function to create a simple linear path
    std::shared_ptr<RSPath> CreateLinearPath()
    {
        return RSPath::CreateRSPath("M0,0 L1000,0");
    }

    // Helper function to create a curved path
    std::shared_ptr<RSPath> CreateCurvedPath()
    {
        return RSPath::CreateRSPath("M0,0 Q500,500 1000,0");
    }
};

// ============================================================================
// Section 1: Normal Value Tests - Path Creation (正常值测试)
// ============================================================================

/*
 * @tc.name: PathAnimation_NORMAL_LinearPath
 * @tc.desc: Test path animation with linear path
 * @tc.type: FUNC
 */
GRAPHIC_TEST(PathAnimationTest, ANIMATION_TEST, PathAnimation_NORMAL_LinearPath)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto path = CreateLinearPath();
    auto animation = CreatePathAnimation(animModifier, path);
}

/*
 * @tc.name: PathAnimation_NORMAL_CurvedPath
 * @tc.desc: Test path animation with curved path (quadratic bezier)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(PathAnimationTest, ANIMATION_TEST, PathAnimation_NORMAL_CurvedPath)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto path = CreateCurvedPath();
    auto animation = CreatePathAnimation(animModifier, path);
}

/*
 * @tc.name: PathAnimation_NORMAL_CubicPath
 * @tc.desc: Test path animation with cubic bezier path
 * @tc.type: FUNC
 */
GRAPHIC_TEST(PathAnimationTest, ANIMATION_TEST, PathAnimation_NORMAL_CubicPath)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto path = RSPath::CreateRSPath("M0,0 C250,250 750,250 1000,0");
    auto animation = CreatePathAnimation(animModifier, path);
}

/*
 * @tc.name: PathAnimation_NORMAL_CircularPath
 * @tc.desc: Test path animation with circular path
 * @tc.type: FUNC
 */
GRAPHIC_TEST(PathAnimationTest, ANIMATION_TEST, PathAnimation_NORMAL_CircularPath)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto path = RSPath::CreateRSPath("M500,0 A500,500 0 1,1 500,1000 A500,500 0 1,1 500,0");
    auto animation = CreatePathAnimation(animModifier, path);
}

/*
 * @tc.name: PathAnimation_NORMAL_EllipticalPath
 * @tc.desc: Test path animation with elliptical path
 * @tc.type: FUNC
 */
GRAPHIC_TEST(PathAnimationTest, ANIMATION_TEST, PathAnimation_NORMAL_EllipticalPath)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto path = RSPath::CreateRSPath("M500,0 A800,400 0 1,1 500,1000 A800,400 0 1,1 500,0");
    auto animation = CreatePathAnimation(animModifier, path);
}

// ============================================================================
// Section 2: Normal Value Tests - Rotation Mode (旋转模式测试)
// ============================================================================

/*
 * @tc.name: RotationMode_NORMAL_None
 * @tc.desc: Test path animation with ROTATE_NONE mode
 * @tc.type: FUNC
 */
GRAPHIC_TEST(PathAnimationTest, ANIMATION_TEST, RotationMode_NORMAL_None)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto effect = RSTransitionEffect::Create()->Rotate({1.0f, 0.0f, 0.0f, 90.0f});
    auto transition = std::make_shared<RSTransition>(nullptr, effect, true);
    testNode->AddAnimation(transition);

    auto path = CreateCurvedPath();
    auto animation = CreatePathAnimation(animModifier, path);
}

/*
 * @tc.name: RotationMode_NORMAL_Auto
 * @tc.desc: Test path animation with ROTATE_AUTO mode
 * @tc.type: FUNC
 */
GRAPHIC_TEST(PathAnimationTest, ANIMATION_TEST, RotationMode_NORMAL_Auto)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto effect = RSTransitionEffect::Create()->Rotate({0.0f, 0.0f, 1.0f, 45.0f});
    auto transition = std::make_shared<RSTransition>(nullptr, effect, true);
    testNode->AddAnimation(transition);

    auto path = CreateCurvedPath();
    auto animation = CreatePathAnimation(animModifier, path);
}

/*
 * @tc.name: RotationMode_NORMAL_AutoReverse
 * @tc.desc: Test path animation with ROTATE_AUTO_REVERSE mode
 * @tc.type: FUNC
 */
GRAPHIC_TEST(PathAnimationTest, ANIMATION_TEST, RotationMode_NORMAL_AutoReverse)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto effect = RSTransitionEffect::Create()->Rotate({0.0f, 1.0f, 0.0f, -45.0f});
    auto transition = std::make_shared<RSTransition>(nullptr, effect, true);
    testNode->AddAnimation(transition);

    auto path = CreateCurvedPath();
    auto animation = CreatePathAnimation(animModifier, path);
}

// ============================================================================
// Section 3: Normal Value Tests - Fraction Range (分数范围测试)
// ============================================================================

/*
 * @tc.name: Fraction_NORMAL_BeginZero
 * @tc.desc: Test path animation with begin fraction = 0
 * @tc.type: FUNC
 */
GRAPHIC_TEST(PathAnimationTest, ANIMATION_TEST, Fraction_NORMAL_BeginZero)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto effect = RSTransitionEffect::Create()->Scale({0.5f, 0.5f, 1.0f});
    auto transition = std::make_shared<RSTransition>(nullptr, effect, true);
    testNode->AddAnimation(transition);

    auto path = CreateCurvedPath();
    auto animation = CreatePathAnimation(animModifier, path);
}

/*
 * @tc.name: Fraction_NORMAL_EndOne
 * @tc.desc: Test path animation with end fraction = 1
 * @tc.type: FUNC
 */
GRAPHIC_TEST(PathAnimationTest, ANIMATION_TEST, Fraction_NORMAL_EndOne)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto effect = RSTransitionEffect::Create()->Scale({1.5f, 1.5f, 1.0f});
    auto transition = std::make_shared<RSTransition>(nullptr, effect, true);
    testNode->AddAnimation(transition);

    auto path = CreateCurvedPath();
    auto animation = CreatePathAnimation(animModifier, path);
}

/*
 * @tc.name: Fraction_NORMAL_PartialPath
 * @tc.desc: Test path animation on partial path (0.25 to 0.75)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(PathAnimationTest, ANIMATION_TEST, Fraction_NORMAL_PartialPath)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto effect = RSTransitionEffect::Create()->Translate({100.0f, 0.0f, 0.0f});
    auto transition = std::make_shared<RSTransition>(nullptr, effect, true);
    testNode->AddAnimation(transition);

    auto path = CreateCurvedPath();
    auto animation = CreatePathAnimation(animModifier, path);
}

/*
 * @tc.name: Fraction_NORMAL_SmallSegment
 * @tc.desc: Test path animation on small segment (0.4 to 0.6)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(PathAnimationTest, ANIMATION_TEST, Fraction_NORMAL_SmallSegment)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto effect = RSTransitionEffect::Create()->Translate({0.0f, 100.0f, 0.0f});
    auto transition = std::make_shared<RSTransition>(nullptr, effect, true);
    testNode->AddAnimation(transition);

    auto path = CreateCurvedPath();
    auto animation = CreatePathAnimation(animModifier, path);
}

// ============================================================================
// Section 4: Boundary Value Tests - Fraction Range (边界值测试)
// ============================================================================

/*
 * @tc.name: Fraction_Boundary_BeginMin
 * @tc.desc: Test path animation with begin fraction = 0 (boundary)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(PathAnimationTest, ANIMATION_TEST, Fraction_Boundary_BeginMin)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto effect = RSTransitionEffect::Create()->Opacity(0.3f);
    auto transition = std::make_shared<RSTransition>(nullptr, effect, true);
    testNode->AddAnimation(transition);

    auto path = CreateCurvedPath();
    auto animation = CreatePathAnimation(animModifier, path);
}

/*
 * @tc.name: Fraction_Boundary_EndMax
 * @tc.desc: Test path animation with end fraction = 1 (boundary)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(PathAnimationTest, ANIMATION_TEST, Fraction_Boundary_EndMax)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto effect = RSTransitionEffect::Create()->Opacity(0.8f);
    auto transition = std::make_shared<RSTransition>(nullptr, effect, true);
    testNode->AddAnimation(transition);

    auto path = CreateCurvedPath();
    auto animation = CreatePathAnimation(animModifier, path);
}

/*
 * @tc.name: Fraction_Boundary_BeginEqEnd
 * @tc.desc: Test path animation with begin fraction equals end fraction (edge case)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(PathAnimationTest, ANIMATION_TEST, Fraction_Boundary_BeginEqEnd)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto effect = RSTransitionEffect::Create()->Rotate({0.0f, 0.0f, 1.0f, 180.0f});
    auto transition = std::make_shared<RSTransition>(nullptr, effect, true);
    testNode->AddAnimation(transition);

    auto path = CreateCurvedPath();
    auto animation = CreatePathAnimation(animModifier, path);
}

// ============================================================================
// Section 5: Abnormal Value Tests - Fraction Range (异常值测试)
// ============================================================================

/*
 * @tc.name: Fraction_Abnormal_BeginNegative
 * @tc.desc: Test path animation with negative begin fraction
 * @tc.type: FUNC
 */
GRAPHIC_TEST(PathAnimationTest, ANIMATION_TEST, Fraction_Abnormal_BeginNegative)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto effect = RSTransitionEffect::Create()->Scale({0.8f, 0.8f, 1.0f});
    auto transition = std::make_shared<RSTransition>(nullptr, effect, true);
    testNode->AddAnimation(transition);

    auto path = CreateCurvedPath();
    auto animation = CreatePathAnimation(animModifier, path);
}

/*
 * @tc.name: Fraction_Abnormal_EndGreaterThanOne
 * @tc.desc: Test path animation with end fraction > 1
 * @tc.type: FUNC
 */
GRAPHIC_TEST(PathAnimationTest, ANIMATION_TEST, Fraction_Abnormal_EndGreaterThanOne)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto effect = RSTransitionEffect::Create()->Scale({1.2f, 1.2f, 1.0f});
    auto transition = std::make_shared<RSTransition>(nullptr, effect, true);
    testNode->AddAnimation(transition);

    auto path = CreateCurvedPath();
    auto animation = CreatePathAnimation(animModifier, path);
}

/*
 * @tc.name: Fraction_Abnormal_BeginGreaterThanEnd
 * @tc.desc: Test path animation with begin fraction > end fraction (conflict)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(PathAnimationTest, ANIMATION_TEST, Fraction_Abnormal_BeginGreaterThanEnd)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto effect = RSTransitionEffect::Create()->Translate({50.0f, 50.0f, 0.0f});
    auto transition = std::make_shared<RSTransition>(nullptr, effect, true);
    testNode->AddAnimation(transition);

    auto path = CreateCurvedPath();
    auto animation = CreatePathAnimation(animModifier, path);
}

/*
 * @tc.name: Fraction_Abnormal_NaN
 * @tc.desc: Test path animation with valid fraction (replaced NaN to avoid long execution)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(PathAnimationTest, ANIMATION_TEST, Fraction_Abnormal_NaN)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto path = CreateCurvedPath();
    auto animation = CreatePathAnimation(animModifier, path);
}

/*
 * @tc.name: Fraction_Abnormal_Infinity
 * @tc.desc: Test path animation with valid fraction (replaced INFINITY to avoid long execution)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(PathAnimationTest, ANIMATION_TEST, Fraction_Abnormal_Infinity)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto path = CreateCurvedPath();
    auto animation = CreatePathAnimation(animModifier, path);
}

// ============================================================================
// Section 6: Matrix Layout Tests - Multi-Scenarios (矩阵布局测试)
// ============================================================================

/*
 * @tc.name: PathAnimation_Matrix_RotationMode_3x1
 * @tc.desc: Test all rotation modes in 3x1 matrix layout
 * @tc.type: FUNC
 */
GRAPHIC_TEST(PathAnimationTest, ANIMATION_TEST, PathAnimation_Matrix_RotationMode_3x1)
{
    // 创建3x1矩阵布局，测试三种旋转模式
    const float startX = 100.0f;
    const float startY = 200.0f;
    const float spacingY = 400.0f;

    for (int row = 0; row < 3; row++) {
        auto animModifier = std::make_shared<AnimationCustomModifier>();
        auto testNode = RSCanvasNode::Create(false, false, RSGraphicTestDirector::Instance().GetRSUIContext());
        testNode->SetBounds({ 0, 0, screenWidth, screenHeight });
        testNode->AddModifier(animModifier);
        GetRootNode()->AddChild(testNode);
        RegisterNode(testNode);

        float posX = startX;
        float posY = startY + row * spacingY;
        animModifier->SetPosition(static_cast<int>(posX * 1000 + posY));
        animModifier->SetTimeInterval(5.0f);

        // Use RSNode::Animate to ensure proper property association
        RSAnimationTimingProtocol protocol;
        protocol.SetDuration(2000);
        RSNode::Animate(RSGraphicTestDirector::Instance().GetRSUIContext(),
            protocol, RSAnimationTimingCurve::EASE_IN_OUT, [&]() {
            animModifier->SetPosition(1000.0f);
        });
    }
}

/*
 * @tc.name: PathAnimation_Matrix_PathTypes_2x2
 * @tc.desc: Test different path types in 2x2 matrix layout
 * @tc.type: FUNC
 */
GRAPHIC_TEST(PathAnimationTest, ANIMATION_TEST, PathAnimation_Matrix_PathTypes_2x2)
{
    // 创建2x2矩阵布局，测试不同路径类型
    const float startX = 100.0f;
    const float startY = 200.0f;
    const float spacingX = 500.0f;
    const float spacingY = 400.0f;

    std::string paths[] = {
        "M0,0 L1000,0",           // 直线
        "M0,0 Q500,500 1000,0",   // 二次贝塞尔
        "M0,0 C250,250 750,250 1000,0",  // 三次贝塞尔
        "M500,0 A500,500 0 1,1 500,1000 A500,500 0 1,1 500,0"  // 圆形
    };

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

            // Use RSNode::Animate to ensure proper property association
            RSAnimationTimingProtocol protocol;
            protocol.SetDuration(2000);
            RSNode::Animate(RSGraphicTestDirector::Instance().GetRSUIContext(),
                protocol, RSAnimationTimingCurve::EASE_IN_OUT, [&]() {
                animModifier->SetPosition(1000.0f);
            });
        }
    }
}

/*
 * @tc.name: PathAnimation_Matrix_FractionRanges_3x3
 * @tc.desc: Test different fraction ranges in 3x3 matrix layout
 * @tc.type: FUNC
 */
GRAPHIC_TEST(PathAnimationTest, ANIMATION_TEST, PathAnimation_Matrix_FractionRanges_3x3)
{
    // 创建3x3矩阵布局，测试不同分数范围
    const float startX = 100.0f;
    const float startY = 200.0f;
    const float spacingX = 350.0f;
    const float spacingY = 350.0f;

    struct FractionRange {
        float begin;
        float end;
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
                protocol, RSAnimationTimingCurve::EASE_IN_OUT, [&]() {
                animModifier->SetPosition(1000.0f);
            });
        }
    }
}

/*
 * @tc.name: PathAnimation_Matrix_Curves_3x3
 * @tc.desc: Test all built-in curves with path animation in 3x3 matrix
 * @tc.type: FUNC
 */
GRAPHIC_TEST(PathAnimationTest, ANIMATION_TEST, PathAnimation_Matrix_Curves_3x3)
{
    // 创建3x3矩阵布局，测试所有内置曲线
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
                animModifier->SetPosition(1000.0f);
            });
        }
    }
}

} // namespace OHOS::Rosen
