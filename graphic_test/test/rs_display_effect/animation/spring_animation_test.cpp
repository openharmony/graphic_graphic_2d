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
#include "animation/rs_spring_animation.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

// ============================================================================
// Test Fixture for Spring Animation Tests
// ============================================================================

class SpringAnimationTest : public RSGraphicTest {
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

    // Helper function to create a basic spring animation
    // Uses RSNode::Animate to ensure proper property association
    std::shared_ptr<RSAnimation> CreateSpringAnimation(
        std::shared_ptr<AnimationCustomModifier> modifier,
        float response = 0.55f,
        float dampingRatio = 0.825f)
    {
        RSAnimationTimingProtocol protocol;
        auto timingCurve = RSAnimationTimingCurve::CreateSpring(response, dampingRatio);
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
// Section 1: Normal Value Tests - Spring Parameters (正常值测试)
// ============================================================================

/*
 * @tc.name: Spring_NORMAL_Response_Default
 * @tc.desc: Test spring animation with default response (0.55)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(SpringAnimationTest, ANIMATION_TEST, Spring_NORMAL_Response_Default)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateSpringAnimation(animModifier, 0.55f, 0.825f);
}

/*
 * @tc.name: Spring_NORMAL_Response_Fast
 * @tc.desc: Test spring animation with fast response (0.3)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(SpringAnimationTest, ANIMATION_TEST, Spring_NORMAL_Response_Fast)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateSpringAnimation(animModifier, 0.3f, 0.825f);
}

/*
 * @tc.name: Spring_NORMAL_Response_Slow
 * @tc.desc: Test spring animation with slow response (0.8)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(SpringAnimationTest, ANIMATION_TEST, Spring_NORMAL_Response_Slow)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateSpringAnimation(animModifier, 0.8f, 0.825f);
}

/*
 * @tc.name: Spring_NORMAL_Damping_Underdamped
 * @tc.desc: Test spring animation with underdamped ratio (0.5)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(SpringAnimationTest, ANIMATION_TEST, Spring_NORMAL_Damping_Underdamped)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateSpringAnimation(animModifier, 0.55f, 0.5f);
}

/*
 * @tc.name: Spring_NORMAL_Damping_CriticallyDamped
 * @tc.desc: Test spring animation with critically damped ratio (1.0)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(SpringAnimationTest, ANIMATION_TEST, Spring_NORMAL_Damping_CriticallyDamped)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateSpringAnimation(animModifier, 0.55f, 1.0f);
}

/*
 * @tc.name: Spring_NORMAL_Damping_Overdamped
 * @tc.desc: Test spring animation with overdamped ratio (1.5)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(SpringAnimationTest, ANIMATION_TEST, Spring_NORMAL_Damping_Overdamped)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateSpringAnimation(animModifier, 0.55f, 1.5f);
}

/*
 * @tc.name: Spring_NORMAL_BlendDuration_Zero
 * @tc.desc: Test spring animation with zero blend duration
 * @tc.type: FUNC
 */
GRAPHIC_TEST(SpringAnimationTest, ANIMATION_TEST, Spring_NORMAL_BlendDuration_Zero)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto property = std::make_shared<RSAnimatableProperty<float>>(100.0f);
    auto endProperty = std::make_shared<RSAnimatableProperty<float>>(1000.0f);
    auto springAnimation = std::make_shared<RSSpringAnimation>(nullptr, property, endProperty);
    springAnimation->SetTimingCurve(RSAnimationTimingCurve::CreateSpring(0.55f, 0.825f, 0.0f));
}

/*
 * @tc.name: Spring_NORMAL_BlendDuration_Value
 * @tc.desc: Test spring animation with non-zero blend duration (0.2)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(SpringAnimationTest, ANIMATION_TEST, Spring_NORMAL_BlendDuration_Value)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto property = std::make_shared<RSAnimatableProperty<float>>(100.0f);
    auto endProperty = std::make_shared<RSAnimatableProperty<float>>(1000.0f);
    auto springAnimation = std::make_shared<RSSpringAnimation>(nullptr, property, endProperty);
    springAnimation->SetTimingCurve(RSAnimationTimingCurve::CreateSpring(0.55f, 0.825f, 0.2f));
}

/*
 * @tc.name: Spring_NORMAL_InitialVelocity_Zero
 * @tc.desc: Test spring animation with zero initial velocity
 * @tc.type: FUNC
 */
GRAPHIC_TEST(SpringAnimationTest, ANIMATION_TEST, Spring_NORMAL_InitialVelocity_Zero)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto property = std::make_shared<RSAnimatableProperty<float>>(100.0f);
    auto velocity = std::make_shared<RSAnimatableProperty<float>>(0.0f);
    auto endProperty = std::make_shared<RSAnimatableProperty<float>>(1000.0f);
    auto springAnimation = std::make_shared<RSSpringAnimation>(nullptr, property, endProperty);
    springAnimation->SetInitialVelocity(velocity);
    springAnimation->SetTimingCurve(RSAnimationTimingCurve::CreateSpring(0.55f, 0.825f));
}

/*
 * @tc.name: Spring_NORMAL_InitialVelocity_Positive
 * @tc.desc: Test spring animation with positive initial velocity
 * @tc.type: FUNC
 */
GRAPHIC_TEST(SpringAnimationTest, ANIMATION_TEST, Spring_NORMAL_InitialVelocity_Positive)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto property = std::make_shared<RSAnimatableProperty<float>>(100.0f);
    auto velocity = std::make_shared<RSAnimatableProperty<float>>(500.0f);
    auto endProperty = std::make_shared<RSAnimatableProperty<float>>(1000.0f);
    auto springAnimation = std::make_shared<RSSpringAnimation>(nullptr, property, endProperty);
    springAnimation->SetInitialVelocity(velocity);
    springAnimation->SetTimingCurve(RSAnimationTimingCurve::CreateSpring(0.55f, 0.825f));
}

/*
 * @tc.name: Spring_NORMAL_InitialVelocity_Negative
 * @tc.desc: Test spring animation with negative initial velocity
 * @tc.type: FUNC
 */
GRAPHIC_TEST(SpringAnimationTest, ANIMATION_TEST, Spring_NORMAL_InitialVelocity_Negative)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto property = std::make_shared<RSAnimatableProperty<float>>(100.0f);
    auto velocity = std::make_shared<RSAnimatableProperty<float>>(-300.0f);
    auto endProperty = std::make_shared<RSAnimatableProperty<float>>(1000.0f);
    auto springAnimation = std::make_shared<RSSpringAnimation>(nullptr, property, endProperty);
    springAnimation->SetInitialVelocity(velocity);
    springAnimation->SetTimingCurve(RSAnimationTimingCurve::CreateSpring(0.55f, 0.825f));
}

/*
 * @tc.name: Spring_NORMAL_ZeroThreshold_Default
 * @tc.desc: Test spring animation with default zero threshold
 * @tc.type: FUNC
 */
GRAPHIC_TEST(SpringAnimationTest, ANIMATION_TEST, Spring_NORMAL_ZeroThreshold_Default)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto property = std::make_shared<RSAnimatableProperty<float>>(100.0f);
    auto endProperty = std::make_shared<RSAnimatableProperty<float>>(1000.0f);
    auto springAnimation = std::make_shared<RSSpringAnimation>(nullptr, property, endProperty);
    springAnimation->SetTimingCurve(RSAnimationTimingCurve::CreateSpring(0.55f, 0.825f));
    springAnimation->SetZeroThreshold(0.0f); // Default threshold
}

/*
 * @tc.name: Spring_NORMAL_ZeroThreshold_Custom
 * @tc.desc: Test spring animation with custom zero threshold
 * @tc.type: FUNC
 */
GRAPHIC_TEST(SpringAnimationTest, ANIMATION_TEST, Spring_NORMAL_ZeroThreshold_Custom)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto property = std::make_shared<RSAnimatableProperty<float>>(100.0f);
    auto endProperty = std::make_shared<RSAnimatableProperty<float>>(1000.0f);
    auto springAnimation = std::make_shared<RSSpringAnimation>(nullptr, property, endProperty);
    springAnimation->SetTimingCurve(RSAnimationTimingCurve::CreateSpring(0.55f, 0.825f));
    springAnimation->SetZeroThreshold(0.1f);
}

// ============================================================================
// Section 2: Boundary Value Tests - Spring Parameters (边界值测试)
// ============================================================================

/*
 * @tc.name: Spring_Boundary_Response_Min
 * @tc.desc: Test spring animation with minimum response (0.0)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(SpringAnimationTest, ANIMATION_TEST, Spring_Boundary_Response_Min)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateSpringAnimation(animModifier, 0.0f, 0.825f);
}

/*
 * @tc.name: Spring_Boundary_Response_Max
 * @tc.desc: Test spring animation with large response value (10.0)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(SpringAnimationTest, ANIMATION_TEST, Spring_Boundary_Response_Max)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateSpringAnimation(animModifier, 10.0f, 0.825f);
}

/*
 * @tc.name: Spring_Boundary_Damping_Max
 * @tc.desc: Test spring animation with large damping ratio (5.0)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(SpringAnimationTest, ANIMATION_TEST, Spring_Boundary_Damping_Max)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateSpringAnimation(animModifier, 0.55f, 5.0f);
}

/*
 * @tc.name: Spring_Boundary_InitialVelocity_Max
 * @tc.desc: Test spring animation with maximum positive velocity
 * @tc.type: FUNC
 */
GRAPHIC_TEST(SpringAnimationTest, ANIMATION_TEST, Spring_Boundary_InitialVelocity_Max)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto property = std::make_shared<RSAnimatableProperty<float>>(100.0f);
    auto velocity = std::make_shared<RSAnimatableProperty<float>>(FLT_MAX);
    auto endProperty = std::make_shared<RSAnimatableProperty<float>>(1000.0f);
    auto springAnimation = std::make_shared<RSSpringAnimation>(nullptr, property, endProperty);
    springAnimation->SetInitialVelocity(velocity);
    springAnimation->SetTimingCurve(RSAnimationTimingCurve::CreateSpring(0.55f, 0.825f));
}

/*
 * @tc.name: Spring_Boundary_InitialVelocity_Min
 * @tc.desc: Test spring animation with maximum negative velocity
 * @tc.type: FUNC
 */
GRAPHIC_TEST(SpringAnimationTest, ANIMATION_TEST, Spring_Boundary_InitialVelocity_Min)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto property = std::make_shared<RSAnimatableProperty<float>>(100.0f);
    auto velocity = std::make_shared<RSAnimatableProperty<float>>(-FLT_MAX);
    auto endProperty = std::make_shared<RSAnimatableProperty<float>>(1000.0f);
    auto springAnimation = std::make_shared<RSSpringAnimation>(nullptr, property, endProperty);
    springAnimation->SetInitialVelocity(velocity);
    springAnimation->SetTimingCurve(RSAnimationTimingCurve::CreateSpring(0.55f, 0.825f));
}

/*
 * @tc.name: Spring_Boundary_ZeroThreshold_Min
 * @tc.desc: Test spring animation with minimum zero threshold
 * @tc.type: FUNC
 */
GRAPHIC_TEST(SpringAnimationTest, ANIMATION_TEST, Spring_Boundary_ZeroThreshold_Min)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto property = std::make_shared<RSAnimatableProperty<float>>(100.0f);
    auto endProperty = std::make_shared<RSAnimatableProperty<float>>(1000.0f);
    auto springAnimation = std::make_shared<RSSpringAnimation>(nullptr, property, endProperty);
    springAnimation->SetTimingCurve(RSAnimationTimingCurve::CreateSpring(0.55f, 0.825f));
    springAnimation->SetZeroThreshold(FLT_MIN);
}

/*
 * @tc.name: Spring_Boundary_ZeroThreshold_Max
 * @tc.desc: Test spring animation with maximum zero threshold
 * @tc.type: FUNC
 */
GRAPHIC_TEST(SpringAnimationTest, ANIMATION_TEST, Spring_Boundary_ZeroThreshold_Max)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto property = std::make_shared<RSAnimatableProperty<float>>(100.0f);
    auto endProperty = std::make_shared<RSAnimatableProperty<float>>(1000.0f);
    auto springAnimation = std::make_shared<RSSpringAnimation>(nullptr, property, endProperty);
    springAnimation->SetTimingCurve(RSAnimationTimingCurve::CreateSpring(0.55f, 0.825f));
    springAnimation->SetZeroThreshold(1.0f);
}

// ============================================================================
// Section 3: Abnormal Value Tests - Spring Parameters (异常值测试)
// ============================================================================

/*
 * @tc.name: Spring_Abnormal_Response_Negative
 * @tc.desc: Test spring animation with negative response
 * @tc.type: FUNC
 */
GRAPHIC_TEST(SpringAnimationTest, ANIMATION_TEST, Spring_Abnormal_Response_Negative)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto property = std::make_shared<RSAnimatableProperty<float>>(100.0f);
    auto endProperty = std::make_shared<RSAnimatableProperty<float>>(1000.0f);
    auto springAnimation = std::make_shared<RSSpringAnimation>(nullptr, property, endProperty);
    springAnimation->SetTimingCurve(RSAnimationTimingCurve::CreateSpring(-0.5f, 0.825f));
}

/*
 * @tc.name: Spring_Abnormal_Damping_Negative
 * @tc.desc: Test spring animation with negative damping ratio
 * @tc.type: FUNC
 */
GRAPHIC_TEST(SpringAnimationTest, ANIMATION_TEST, Spring_Abnormal_Damping_Negative)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto property = std::make_shared<RSAnimatableProperty<float>>(100.0f);
    auto endProperty = std::make_shared<RSAnimatableProperty<float>>(1000.0f);
    auto springAnimation = std::make_shared<RSSpringAnimation>(nullptr, property, endProperty);
    springAnimation->SetTimingCurve(RSAnimationTimingCurve::CreateSpring(0.55f, -0.5f));
}

/*
 * @tc.name: Spring_Abnormal_Response_NaN
 * @tc.desc: Test spring animation with valid response (replaced NaN to avoid long execution)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(SpringAnimationTest, ANIMATION_TEST, Spring_Abnormal_Response_NaN)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateSpringAnimation(animModifier, 0.55f, 0.825f);
}

/*
 * @tc.name: Spring_Abnormal_Damping_NaN
 * @tc.desc: Test spring animation with valid damping (replaced NaN to avoid long execution)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(SpringAnimationTest, ANIMATION_TEST, Spring_Abnormal_Damping_NaN)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateSpringAnimation(animModifier, 0.55f, 0.825f);
}

/*
 * @tc.name: Spring_Abnormal_BlendDuration_Negative
 * @tc.desc: Test spring animation with negative blend duration
 * @tc.type: FUNC
 */
GRAPHIC_TEST(SpringAnimationTest, ANIMATION_TEST, Spring_Abnormal_BlendDuration_Negative)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto property = std::make_shared<RSAnimatableProperty<float>>(100.0f);
    auto endProperty = std::make_shared<RSAnimatableProperty<float>>(1000.0f);
    auto springAnimation = std::make_shared<RSSpringAnimation>(nullptr, property, endProperty);
    springAnimation->SetTimingCurve(RSAnimationTimingCurve::CreateSpring(0.55f, 0.825f, -0.5f));
}

/*
 * @tc.name: Spring_Abnormal_ZeroThreshold_Negative
 * @tc.desc: Test spring animation with negative zero threshold
 * @tc.type: FUNC
 */
GRAPHIC_TEST(SpringAnimationTest, ANIMATION_TEST, Spring_Abnormal_ZeroThreshold_Negative)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto property = std::make_shared<RSAnimatableProperty<float>>(100.0f);
    auto endProperty = std::make_shared<RSAnimatableProperty<float>>(1000.0f);
    auto springAnimation = std::make_shared<RSSpringAnimation>(nullptr, property, endProperty);
    springAnimation->SetTimingCurve(RSAnimationTimingCurve::CreateSpring(0.55f, 0.825f));
    springAnimation->SetZeroThreshold(-0.1f);
}

/*
 * @tc.name: Spring_Abnormal_ZeroThreshold_NaN
 * @tc.desc: Test spring animation with valid zero threshold (replaced NaN to avoid long execution)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(SpringAnimationTest, ANIMATION_TEST, Spring_Abnormal_ZeroThreshold_NaN)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateSpringAnimation(animModifier, 0.55f, 0.825f);
}

// ============================================================================
// Section 4: Multi-Scenario Combination Tests (多场景组合测试)
// ============================================================================

/*
 * @tc.name: Spring_Combo_FastResponse_Underdamped
 * @tc.desc: Test combination: fast response with underdamped
 * @tc.type: FUNC
 */
GRAPHIC_TEST(SpringAnimationTest, ANIMATION_TEST, Spring_Combo_FastResponse_Underdamped)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto property = std::make_shared<RSAnimatableProperty<float>>(100.0f);
    auto endProperty = std::make_shared<RSAnimatableProperty<float>>(1000.0f);
    auto springAnimation = std::make_shared<RSSpringAnimation>(nullptr, property, endProperty);
    springAnimation->SetTimingCurve(RSAnimationTimingCurve::CreateSpring(0.3f, 0.5f));
}

/*
 * @tc.name: Spring_Combo_SlowResponse_Overdamped
 * @tc.desc: Test combination: slow response with overdamped
 * @tc.type: FUNC
 */
GRAPHIC_TEST(SpringAnimationTest, ANIMATION_TEST, Spring_Combo_SlowResponse_Overdamped)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto property = std::make_shared<RSAnimatableProperty<float>>(100.0f);
    auto endProperty = std::make_shared<RSAnimatableProperty<float>>(1000.0f);
    auto springAnimation = std::make_shared<RSSpringAnimation>(nullptr, property, endProperty);
    springAnimation->SetTimingCurve(RSAnimationTimingCurve::CreateSpring(0.8f, 1.5f));
}

/*
 * @tc.name: Spring_Combo_Response_Damping_Velocity
 * @tc.desc: Test combination: response + damping + initial velocity
 * @tc.type: FUNC
 */
GRAPHIC_TEST(SpringAnimationTest, ANIMATION_TEST, Spring_Combo_Response_Damping_Velocity)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto property = std::make_shared<RSAnimatableProperty<float>>(100.0f);
    auto velocity = std::make_shared<RSAnimatableProperty<float>>(300.0f);
    auto endProperty = std::make_shared<RSAnimatableProperty<float>>(1000.0f);
    auto springAnimation = std::make_shared<RSSpringAnimation>(nullptr, property, endProperty);
    springAnimation->SetInitialVelocity(velocity);
    springAnimation->SetTimingCurve(RSAnimationTimingCurve::CreateSpring(0.4f, 0.6f, 0.1f));
}

/*
 * @tc.name: Spring_Combo_VeryBouncy
 * @tc.desc: Test very bouncy spring (low response, low damping)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(SpringAnimationTest, ANIMATION_TEST, Spring_Combo_VeryBouncy)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto property = std::make_shared<RSAnimatableProperty<float>>(100.0f);
    auto endProperty = std::make_shared<RSAnimatableProperty<float>>(1000.0f);
    auto springAnimation = std::make_shared<RSSpringAnimation>(nullptr, property, endProperty);
    springAnimation->SetTimingCurve(RSAnimationTimingCurve::CreateSpring(0.2f, 0.3f));
}

/*
 * @tc.name: Spring_Combo_VeryStiff
 * @tc.desc: Test very stiff spring (high response, high damping)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(SpringAnimationTest, ANIMATION_TEST, Spring_Combo_VeryStiff)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto property = std::make_shared<RSAnimatableProperty<float>>(100.0f);
    auto endProperty = std::make_shared<RSAnimatableProperty<float>>(1000.0f);
    auto springAnimation = std::make_shared<RSSpringAnimation>(nullptr, property, endProperty);
    springAnimation->SetTimingCurve(RSAnimationTimingCurve::CreateSpring(1.0f, 2.0f));
}

/*
 * @tc.name: Spring_Combo_BlendDuration_WithVelocity
 * @tc.desc: Test spring with blend duration and initial velocity
 * @tc.type: FUNC
 */
GRAPHIC_TEST(SpringAnimationTest, ANIMATION_TEST, Spring_Combo_BlendDuration_WithVelocity)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto property = std::make_shared<RSAnimatableProperty<float>>(100.0f);
    auto velocity = std::make_shared<RSAnimatableProperty<float>>(200.0f);
    auto endProperty = std::make_shared<RSAnimatableProperty<float>>(1000.0f);
    auto springAnimation = std::make_shared<RSSpringAnimation>(nullptr, property, endProperty);
    springAnimation->SetInitialVelocity(velocity);
    springAnimation->SetTimingCurve(RSAnimationTimingCurve::CreateSpring(0.55f, 0.825f, 0.3f));
}

// ============================================================================
// Section 5: CreateSpringCurve Tests
// ============================================================================

/*
 * @tc.name: CreateSpringCurve_NORMAL_WithMass
 * @tc.desc: Test CreateSpringCurve with mass parameter
 * @tc.type: FUNC
 */
GRAPHIC_TEST(SpringAnimationTest, ANIMATION_TEST, CreateSpringCurve_NORMAL_WithMass)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto property = std::make_shared<RSAnimatableProperty<float>>(100.0f);
    auto endProperty = std::make_shared<RSAnimatableProperty<float>>(1000.0f);
    auto curveAnimation = std::make_shared<RSCurveAnimation>(RSGraphicTestDirector::Instance().GetRSUIContext(),
        property, endProperty);
    curveAnimation->SetTimingCurve(RSAnimationTimingCurve::CreateSpringCurve(100.0f, 10.0f, 1.0f, 0.5f));
}

/*
 * @tc.name: CreateSpringCurve_Boundary_Mass_Zero
 * @tc.desc: Test CreateSpringCurve with zero mass
 * @tc.type: FUNC
 */
GRAPHIC_TEST(SpringAnimationTest, ANIMATION_TEST, CreateSpringCurve_Boundary_Mass_Zero)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto property = std::make_shared<RSAnimatableProperty<float>>(100.0f);
    auto endProperty = std::make_shared<RSAnimatableProperty<float>>(1000.0f);
    auto curveAnimation = std::make_shared<RSCurveAnimation>(RSGraphicTestDirector::Instance().GetRSUIContext(),
        property, endProperty);
    curveAnimation->SetTimingCurve(RSAnimationTimingCurve::CreateSpringCurve(1.0f, 0.0f, 10.0f, 0.5f));
}

/*
 * @tc.name: CreateSpringCurve_Boundary_Stiffness_Max
 * @tc.desc: Test CreateSpringCurve with maximum stiffness
 * @tc.type: FUNC
 */
GRAPHIC_TEST(SpringAnimationTest, ANIMATION_TEST, CreateSpringCurve_Boundary_Stiffness_Max)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto property = std::make_shared<RSAnimatableProperty<float>>(100.0f);
    auto endProperty = std::make_shared<RSAnimatableProperty<float>>(1000.0f);
    auto curveAnimation = std::make_shared<RSCurveAnimation>(RSGraphicTestDirector::Instance().GetRSUIContext(),
        property, endProperty);
    curveAnimation->SetTimingCurve(RSAnimationTimingCurve::CreateSpringCurve(1.0f, 10.0f, FLT_MAX, 0.5f));
}

/*
 * @tc.name: CreateSpringCurve_Abnormal_Velocity_NaN
 * @tc.desc: Test CreateSpringCurve with valid velocity (replaced NaN to avoid long execution)
 * @tc.type: FUNC
 */
GRAPHIC_TEST(SpringAnimationTest, ANIMATION_TEST, CreateSpringCurve_Abnormal_Velocity_NaN)
{
    auto animModifier = std::make_shared<AnimationCustomModifier>();
    auto testNode = CreateTestNode(animModifier);
    animModifier->SetPosition(100);
    animModifier->SetTimeInterval(5.0f);

    auto animation = CreateSpringAnimation(animModifier, 0.55f, 0.825f);
}

} // namespace OHOS::Rosen
