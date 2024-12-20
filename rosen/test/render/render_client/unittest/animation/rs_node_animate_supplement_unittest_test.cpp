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

#include "animation/rs_animation_timing_protocol.h"
#include "animation/rs_curve_animation.h"
#include "animation/rs_path_animation.h"
#include "animation/rs_spring_animation.h"
#include "animation/rs_transition.h"
#include "animation/rs_implicit_animation_param.h"
#include "animation/rs_animation_callback.h"
#include "render/rs_path.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using namespace ANIMATIONTEST;
class RSNodeAnimateUnitTest : public RSAnimationBaseTest {
};

class RSNodeTestMock : public RSNode {
public:
    explicit RSNodeTestMock(bool isRenderServiceNode) : RSNode(isRenderServiceNode) {};
    explicit RSNodeTestMock(bool isRenderServiceNode, NodeId id) : RSNode(isRenderServiceNode, id) {};

    void OnAddChildren() override
    {
        RSNode::OnAddChildren();
    };

    void OnRemoveChildren() override
    {
        RSNode::OnRemoveChildren();
    };

    std::vector<PropertyId> GetModifierIds()
    {
        return RSNode::GetModifierIds();
    };
};

/**
 * @tc.name: RSNodeAnimateSupplementTest001
 * @tc.desc: Verify the create RSNode
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimateUnitTest, RSNodeAnimateSupplementTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSNodeAnimateUnitTest RSNodeAnimateSupplementTest001 start";
    /**
     * @tc.steps: step1. init
     */
    auto animateNode = std::make_shared<RSNodeTestMock>(false);
    EXPECT_TRUE(animateNode != nullptr);

    RSAnimationTimingProtocol timeProtocol;
    RSAnimationTimingCurve curve;
    RSNode::OpenImplicitAnimation(timeProtocol, curve);
    RSNode::CloseImplicitAnimation();
    GTEST_LOG_(INFO) << "RSNodeAnimateUnitTest RSNodeAnimateSupplementTest001 end";
}

/**
 * @tc.name: RSNodeAnimateSupplementTest002
 * @tc.desc: Verify the animate
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimateUnitTest, RSNodeAnimateSupplementTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSNodeAnimateUnitTest RSNodeAnimateSupplementTest002 start";
    /**
     * @tc.steps: step1. init
     */
    auto animateNode = std::make_shared<RSNodeTestMock>(false);
    EXPECT_TRUE(animateNode != nullptr);
    RSAnimationTimingProtocol timeProtocol;
    RSAnimationTimingCurve curve;
    std::function<void()> callback = nullptr;
    RSNode::Animate(timeProtocol, curve, callback);
    callback = []() {
        std::cout << "RSNodeAnimateUnitTest RSNodeAnimateSupplementTest002 callback" << std::endl;
    };
    RSNode::Animate(timeProtocol, curve, callback);
    GTEST_LOG_(INFO) << "RSNodeAnimateUnitTest RSNodeAnimateSupplementTest002 end";
}

/**
 * @tc.name: RSNodeAnimateSupplementTest003
 * @tc.desc: Verify the addAnimation
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimateUnitTest, RSNodeAnimateSupplementTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSNodeAnimateUnitTest RSNodeAnimateSupplementTest003 start";
    /**
     * @tc.steps: step1. init
     */
    auto animateNode = std::make_shared<RSNodeTestMock>(false);
    EXPECT_TRUE(animateNode != nullptr);
    animateNode->AddAnimation(nullptr);
    auto property = std::make_shared<RSAnimatableProperty<float>>(0.0f);
    auto endProperty = std::make_shared<RSAnimatableProperty<float>>(1.0f);
    auto curveAnimation = std::make_shared<RSCurveAnimation>(property, endProperty);
    curveAnimation->SetDuration(300);
    animateNode->AddAnimation(curveAnimation);
    animateNode->AddAnimation(curveAnimation);
    auto property2 = std::make_shared<RSAnimatableProperty<float>>(0.0f);
    auto endProperty2 = std::make_shared<RSAnimatableProperty<float>>(1.0f);
    auto curveAnimation2 = std::make_shared<RSCurveAnimation>(property2, endProperty2);
    curveAnimation2->SetDuration(0);
    animateNode->AddAnimation(curveAnimation2);
    GTEST_LOG_(INFO) << "RSNodeAnimateUnitTest RSNodeAnimateSupplementTest003 end";
}

/**
 * @tc.name: RSNodeAnimateSupplementTest004
 * @tc.desc: Verify the removeAnimation
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimateUnitTest, RSNodeAnimateSupplementTest004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSNodeAnimateUnitTest RSNodeAnimateSupplementTest004 start";
    /**
     * @tc.steps: step1. init
     */
    auto animateNode = std::make_shared<RSNodeTestMock>(false);
    EXPECT_TRUE(animateNode != nullptr);
    animateNode->AddAnimation(nullptr);
    auto property = std::make_shared<RSAnimatableProperty<float>>(0.0f);
    auto endProperty = std::make_shared<RSAnimatableProperty<float>>(1.0f);
    auto curveAnimation = std::make_shared<RSCurveAnimation>(property, endProperty);
    animateNode->AddAnimation(curveAnimation);
    animateNode->RemoveAllAnimations();

    auto property2 = std::make_shared<RSAnimatableProperty<float>>(0.0f);
    auto endProperty2 = std::make_shared<RSAnimatableProperty<float>>(1.0f);
    auto curveAnimation2 = std::make_shared<RSCurveAnimation>(property2, endProperty2);
    animateNode->AddAnimation(curveAnimation2);
    animateNode->RemoveAnimation(nullptr);
    animateNode->RemoveAnimation(curveAnimation);
    animateNode->RemoveAnimation(curveAnimation2);
    GTEST_LOG_(INFO) << "RSNodeAnimateUnitTest RSNodeAnimateSupplementTest004 end";
}

/**
 * @tc.name: RSNodeAnimateSupplementTest005
 * @tc.desc: Verify the SetMotionPathOption
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimateUnitTest, RSNodeAnimateSupplementTest005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSNodeAnimateUnitTest RSNodeAnimateSupplementTest005 start";
    /**
     * @tc.steps: step1. init
     */
    auto animateNode = std::make_shared<RSNodeTestMock>(false);
    EXPECT_TRUE(animateNode != nullptr);
    animateNode->SetMotionPathOption(nullptr);
    auto motionPathOption = std::make_shared<RSMotionPathOption>(ANIMATION_PATH);
    auto property1 = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier1 = std::make_shared<RSBoundsModifier>(property1);
    animateNode->AddModifier(modifier1);
    auto property2 = std::make_shared<RSAnimatableProperty<float>>(0.0f);
    auto modifier2 = std::make_shared<RSAlphaModifier>(property2);
    animateNode->AddModifier(modifier2);
    animateNode->SetMotionPathOption(motionPathOption);
    GTEST_LOG_(INFO) << "RSNodeAnimateUnitTest RSNodeAnimateSupplementTest005 end";
}

/**
 * @tc.name: RSNodeAnimateSupplementTest006
 * @tc.desc: Verify the SetBounds
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimateUnitTest, RSNodeAnimateSupplementTest006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSNodeAnimateUnitTest RSNodeAnimateSupplementTest006 start";
    /**
     * @tc.steps: step1. init
     */
    auto animateNode = std::make_shared<RSNodeTestMock>(false);
    EXPECT_TRUE(animateNode != nullptr);
    animateNode->SetBoundsWidth(100.0f);
    animateNode->SetBoundsHeight(100.0f);
    auto modifier1 = std::make_shared<RSBoundsModifier>(nullptr);
    animateNode->AddModifier(modifier1);
    animateNode->SetBoundsWidth(100.0f);
    animateNode->SetBoundsHeight(100.0f);
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<RSBoundsModifier>(property);
    animateNode->AddModifier(modifier);
    animateNode->SetBoundsWidth(100.0f);
    animateNode->SetBoundsWidth(10.0f);
    animateNode->SetBoundsHeight(100.0f);
    animateNode->SetBoundsHeight(10.0f);
    GTEST_LOG_(INFO) << "RSNodeAnimateUnitTest RSNodeAnimateSupplementTest006 end";
}

/**
 * @tc.name: RSNodeAnimateSupplementTest007
 * @tc.desc: Verify the SetFrame
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimateUnitTest, RSNodeAnimateSupplementTest007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSNodeAnimateUnitTest RSNodeAnimateSupplementTest007 start";
    /**
     * @tc.steps: step1. init
     */
    auto animateNode = std::make_shared<RSNodeTestMock>(false);
    EXPECT_TRUE(animateNode != nullptr);
    animateNode->SetFramePositionX(100.0f);
    animateNode->SetFramePositionY(100.0f);
    auto modifier1 = std::make_shared<RSFrameModifier>(nullptr);
    animateNode->AddModifier(modifier1);
    animateNode->SetFramePositionX(100.0f);
    animateNode->SetFramePositionY(100.0f);
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<RSFrameModifier>(property);
    animateNode->AddModifier(modifier);
    animateNode->SetFramePositionX(100.0f);
    animateNode->SetFramePositionY(10.0f);
    animateNode->SetFramePositionX(100.0f);
    animateNode->SetFramePositionY(10.0f);
    GTEST_LOG_(INFO) << "RSNodeAnimateUnitTest RSNodeAnimateSupplementTest007 end";
}

/**
 * @tc.name: RSNodeAnimateSupplementTest008
 * @tc.desc: Verify the SetPivot
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimateUnitTest, RSNodeAnimateSupplementTest008, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSNodeAnimateUnitTest RSNodeAnimateSupplementTest008 start";
    /**
     * @tc.steps: step1. init
     */
    auto animateNode = std::make_shared<RSNodeTestMock>(false);
    EXPECT_TRUE(animateNode != nullptr);
    animateNode->SetPivotX(100.0f);
    animateNode->SetPivotY(100.0f);
    auto modifier1 = std::make_shared<RSFrameModifier>(nullptr);
    animateNode->AddModifier(modifier1);
    animateNode->SetPivotX(100.0f);
    animateNode->SetPivotY(100.0f);
    auto property = std::make_shared<RSAnimatableProperty<Vector2f>>(ANIMATION_NORMAL_SCALE);
    auto modifier = std::make_shared<RSFrameModifier>(property);
    animateNode->AddModifier(modifier);
    animateNode->SetPivotX(100.0f);
    animateNode->SetPivotY(10.0f);
    animateNode->SetPivotX(100.0f);
    animateNode->SetPivotY(10.0f);
    GTEST_LOG_(INFO) << "RSNodeAnimateUnitTest RSNodeAnimateSupplementTest008 end";
}

/**
 * @tc.name: RSNodeAnimateSupplementTest009
 * @tc.desc: Verify the SetMask
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimateUnitTest, RSNodeAnimateSupplementTest009, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSNodeAnimateUnitTest RSNodeAnimateSupplementTest009 start";
    /**
     * @tc.steps: step1. init
     */
    auto animateNode = std::make_shared<RSNodeTestMock>(false);
    EXPECT_TRUE(animateNode != nullptr);
    animateNode->SetMask(nullptr);
    GTEST_LOG_(INFO) << "RSNodeAnimateUnitTest RSNodeAnimateSupplementTest009 end";
}

/**
 * @tc.name: RSNodeAnimateSupplementTest010
 * @tc.desc: Verify the OnAddChildren
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimateUnitTest, RSNodeAnimateSupplementTest010, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSNodeAnimateUnitTest RSNodeAnimateSupplementTest010 start";
    /**
     * @tc.steps: step1. init
     */
    auto animateNode = std::make_shared<RSNodeTestMock>(false);
    EXPECT_TRUE(animateNode != nullptr);
    animateNode->SetVisible(false);
    animateNode->OnAddChildren();
    animateNode->OnRemoveChildren();
    animateNode->SetVisible(true);
    animateNode->OnAddChildren();
    animateNode->OnRemoveChildren();

    auto transitionEffect = RSTransitionEffect::Create()->Rotate(TRANSITION_EFFECT_ROTATE);
    EXPECT_TRUE(transitionEffect != nullptr);
    animateNode->SetTransitionEffect(transitionEffect);
    animateNode->SetVisible(false);
    animateNode->OnAddChildren();
    animateNode->OnRemoveChildren();
    animateNode->SetVisible(true);
    animateNode->OnAddChildren();
    animateNode->OnRemoveChildren();
    GTEST_LOG_(INFO) << "RSNodeAnimateUnitTest RSNodeAnimateSupplementTest010 end";
}

/**
 * @tc.name: RSNodeAnimateSupplementTest011
 * @tc.desc: Verify the AddModifier
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimateUnitTest, RSNodeAnimateSupplementTest011, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSNodeAnimateUnitTest RSNodeAnimateSupplementTest011 start";
    /**
     * @tc.steps: step1. init
     */
    auto animateNode = std::make_shared<RSNodeTestMock>(false);
    EXPECT_TRUE(animateNode != nullptr);
    animateNode->SetMotionPathOption(nullptr);
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<RSBoundsModifier>(property);
    animateNode->AddModifier(modifier);

    auto motionPathOption = std::make_shared<RSMotionPathOption>(ANIMATION_PATH);
    animateNode->SetMotionPathOption(motionPathOption);
    auto property2 = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier2 = std::make_shared<RSBoundsModifier>(property2);
    animateNode->AddModifier(modifier2);
    auto property3 = std::make_shared<RSAnimatableProperty<float>>(0.0f);
    auto modifier3 = std::make_shared<RSAlphaModifier>(property3);
    animateNode->AddModifier(modifier3);
    auto property4 = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier4 = std::make_shared<RSFrameModifier>(property4);
    animateNode->AddModifier(modifier4);
    auto property5 = std::make_shared<RSAnimatableProperty<Vector2f>>(ANIMATION_NORMAL_SCALE);
    auto modifier5 = std::make_shared<RSTranslateModifier>(property5);
    animateNode->AddModifier(modifier5);
    GTEST_LOG_(INFO) << "RSNodeAnimateUnitTest RSNodeAnimateSupplementTest011 end";
}

/**
 * @tc.name: RSNodeAnimateSupplementTest012
 * @tc.desc: Verify the RemoveModifier
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimateUnitTest, RSNodeAnimateSupplementTest012, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSNodeAnimateUnitTest RSNodeAnimateSupplementTest012 start";
    /**
     * @tc.steps: step1. init
     */
    auto animateNode = std::make_shared<RSNodeTestMock>(false);
    EXPECT_TRUE(animateNode != nullptr);
    animateNode->RemoveModifier(nullptr);
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<RSBoundsModifier>(property);
    animateNode->AddModifier(modifier);
    auto property2 = std::make_shared<RSAnimatableProperty<float>>(0.0f);
    auto modifier2 = std::make_shared<RSAlphaModifier>(property2);
    animateNode->RemoveModifier(modifier2);

    GTEST_LOG_(INFO) << "RSNodeAnimateUnitTest RSNodeAnimateSupplementTest012 end";
}

/**
 * @tc.name: RSNodeAnimateSupplementTest013
 * @tc.desc: Verify the DumpNode
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimateUnitTest, RSNodeAnimateSupplementTest013, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSNodeAnimateUnitTest RSNodeAnimateSupplementTest013 start";
    /**
     * @tc.steps: step1. init
     */
    auto animateNode = std::make_shared<RSNodeTestMock>(false);
    EXPECT_TRUE(animateNode != nullptr);
    animateNode->DumpNode(1);
    auto property = std::make_shared<RSAnimatableProperty<float>>(0.0f);
    auto endProperty = std::make_shared<RSAnimatableProperty<float>>(1.0f);
    auto curveAnimation = std::make_shared<RSCurveAnimation>(property, endProperty);
    animateNode->AddAnimation(curveAnimation);
    animateNode->DumpNode(1);
    GTEST_LOG_(INFO) << "RSNodeAnimateUnitTest RSNodeAnimateSupplementTest013 end";
}

/**
 * @tc.name: RSNodeAnimateSupplementTest014
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimateUnitTest, RSNodeAnimateSupplementTest014, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest RSNodeAnimateSupplementTest014 start";
    /**
     * @tc.steps: step1. init
     */
    auto animateNode = std::make_shared<RSNodeTestMock>(true);
    EXPECT_TRUE(animateNode != nullptr);
    RSAnimationTimingProtocol timeProtocol;
    timeProtocol.SetDuration(100);
    RSNode::OpenImplicitAnimation(timeProtocol, RSAnimationTimingCurve::LINEAR);
    RSNode::CloseImplicitAnimation();
    PropertyCallback callback1;
    std::weak_ptr<RSNode> weak = animateNode;
    callback1 = [weak]() {
        auto node2 = weak.lock();
        if (node2 == nullptr) {
            return;
        }
        node2->SetAlpha(0.2f);
    };
    RSNode::AddKeyFrame(0.1f, RSAnimationTimingCurve::LINEAR, callback1);
    RSNode::AddKeyFrame(0.2f, callback1);
    PropertyCallback callback2;
    RSNode::Animate(timeProtocol, RSAnimationTimingCurve::LINEAR, callback2);
    callback1 = [weak]() {
        auto node2 = weak.lock();
        if (node2 == nullptr) {
            return;
        }
        node2->SetAlpha(0.3f);
    };
    RSNode::Animate(timeProtocol, RSAnimationTimingCurve::LINEAR, callback2);
    GTEST_LOG_(INFO) << "RSAnimationTest RSNodeAnimateSupplementTest014 end";
}

/**
 * @tc.name: RSNodeAnimateSupplementTest015
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimateUnitTest, RSNodeAnimateSupplementTest015, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest RSNodeAnimateSupplementTest015 start";
    /**
     * @tc.steps: step1. init
     */
    auto animateNode = std::make_shared<RSNodeTestMock>(true);
    EXPECT_TRUE(animateNode != nullptr);
    std::shared_ptr<RSCurveAnimation> curveAnimation;
    animateNode->AddAnimation(curveAnimation);
    auto property = std::make_shared<RSAnimatableProperty<float>>(0.0f);
    auto endProperty = std::make_shared<RSAnimatableProperty<float>>(1.0f);
    curveAnimation = std::make_shared<RSCurveAnimation>(property, endProperty);
    curveAnimation->SetDuration(0);
    animateNode->AddAnimation(curveAnimation);
    animateNode->AddAnimation(curveAnimation);
    GTEST_LOG_(INFO) << "RSAnimationTest RSNodeAnimateSupplementTest015 end";
}

/**
 * @tc.name: RSNodeAnimateSupplementTest016
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimateUnitTest, RSNodeAnimateSupplementTest016, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest RSNodeAnimateSupplementTest016 start";
    /**
     * @tc.steps: step1. init
     */
    auto animateNode = std::make_shared<RSNodeTestMock>(true);
    EXPECT_TRUE(animateNode != nullptr);
    std::shared_ptr<RSCurveAnimation> curveAnimation;
    animateNode->RemoveAllAnimations();
    animateNode->RemoveAnimation(curveAnimation);
    auto property = std::make_shared<RSAnimatableProperty<float>>(0.0f);
    auto endProperty = std::make_shared<RSAnimatableProperty<float>>(1.0f);
    curveAnimation = std::make_shared<RSCurveAnimation>(property, endProperty);
    animateNode->RemoveAnimation(curveAnimation);
    GTEST_LOG_(INFO) << "RSAnimationTest RSNodeAnimateSupplementTest016 end";
}

/**
 * @tc.name: RSNodeAnimateSupplementTest017
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimateUnitTest, RSNodeAnimateSupplementTest017, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest RSNodeAnimateSupplementTest017 start";
    /**
     * @tc.steps: step1. init
     */
    auto animateNode = std::make_shared<RSNodeTestMock>(true);
    EXPECT_TRUE(animateNode != nullptr);
    std::shared_ptr<RSMotionPathOption> option = std::make_shared<RSMotionPathOption>("abc");
    animateNode->SetMotionPathOption(option);
    animateNode->SetBounds(1.f, 2.f, 3.f, 4.f);
    animateNode->SetMotionPathOption(option);
    Vector4f data(2.f, 3.f, 4.f, 5.f);
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(data);
    auto modifier = std::make_shared<RSBoundsModifier>(property);
    animateNode->AddModifier(modifier);
    animateNode->SetMotionPathOption(option);
    animateNode->SetAlphaOffscreen(true);
    GTEST_LOG_(INFO) << "RSAnimationTest RSNodeAnimateSupplementTest017 end";
}

/**
 * @tc.name: RSNodeAnimateSupplementTest018
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimateUnitTest, RSNodeAnimateSupplementTest018, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest RSNodeAnimateSupplementTest018 start";
    /**
     * @tc.steps: step1. init
     */
    auto animateNode = std::make_shared<RSNodeTestMock>(true);
    EXPECT_TRUE(animateNode != nullptr);
    Vector4f data(2.f, 3.f, 4.f, 5.f);
    auto property1 = std::make_shared<RSAnimatableProperty<Vector4f>>(data);
    auto modifier1 = std::make_shared<RSBoundsModifier>(property1);
    animateNode->AddModifier(modifier1);
    auto modifier2 = std::make_shared<RSFrameModifier>(property1);
    animateNode->AddModifier(modifier2);
    animateNode->SetBoundsWidth(10.f);
    animateNode->SetBoundsHeight(20.f);
    animateNode->SetFramePositionX(5.f);
    animateNode->SetFramePositionY(6.f);
    GTEST_LOG_(INFO) << "RSAnimationTest RSNodeAnimateSupplementTest018 end";
}

/**
 * @tc.name: RSNodeAnimateSupplementTest019
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimateUnitTest, RSNodeAnimateSupplementTest019, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest RSNodeAnimateSupplementTest019 start";
    /**
     * @tc.steps: step1. init
     */
    auto animateNode = std::make_shared<RSNodeTestMock>(true);
    EXPECT_TRUE(animateNode != nullptr);
    std::shared_ptr<RSMask> mask = std::make_shared<RSMask>();
    animateNode->SetMask(mask);
    std::shared_ptr<const RSTransitionEffect> effect = RSTransitionEffect::OPACITY;
    animateNode->SetTransitionEffect(effect);
    animateNode->SetVisible(true);
    animateNode->OnRemoveChildren();
    GTEST_LOG_(INFO) << "RSAnimationTest RSNodeAnimateSupplementTest019 end";
}

/**
 * @tc.name: RSNodeAnimateSupplementTest020
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimateUnitTest, RSNodeAnimateSupplementTest020, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest RSNodeAnimateSupplementTest020 start";
    /**
     * @tc.steps: step1. init
     */
    auto animateNode = std::make_shared<RSNodeTestMock>(true);
    EXPECT_TRUE(animateNode != nullptr);
    std::shared_ptr<RSMotionPathOption> option = std::make_shared<RSMotionPathOption>("abc");
    animateNode->SetMotionPathOption(option);
    Vector4f data(2.f, 3.f, 4.f, 5.f);
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(data);
    auto modifier = std::make_shared<RSBoundsModifier>(property);
    animateNode->AddModifier(modifier);
    animateNode->RemoveModifier(modifier);
    animateNode->RemoveModifier(modifier);
    GTEST_LOG_(INFO) << "RSAnimationTest RSNodeAnimateSupplementTest020 end";
}

/**
 * @tc.name: RSNodeAnimateSupplementTest021
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimateUnitTest, RSNodeAnimateSupplementTest021, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest RSNodeAnimateSupplementTest021 start";
    /**
     * @tc.steps: step1. init
     */
    auto animateNode = std::make_shared<RSNodeTestMock>(true);
    EXPECT_TRUE(animateNode != nullptr);
    auto property = std::make_shared<RSAnimatableProperty<float>>(1.f);
    auto modifier = std::make_shared<RSAlphaModifier>(property);
    animateNode->AddModifier(modifier);
    auto ids = animateNode->GetModifierIds();
    EXPECT_TRUE(!ids.empty());
    animateNode->DumpNode(10);
    GTEST_LOG_(INFO) << "RSAnimationTest RSNodeAnimateSupplementTest021 end";
}

/**
 * @tc.name: RSNodeAnimateSupplementTest022
 * @tc.desc: Verify the create RSNode
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimateUnitTest, RSNodeAnimateSupplementTest022, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSNodeAnimateUnitTest RSNodeAnimateSupplementTest022 start";
    /**
     * @tc.steps: step1. init
     */
    auto animateNode = std::make_shared<RSNodeTestMock>(false);
    EXPECT_TRUE(animateNode != nullptr);

    RSAnimationTimingProtocol timeProtocol;
    RSAnimationTimingCurve curve;
    RSNode::OpenImplicitAnimation(timeProtocol, curve);
    RSNode::CloseImplicitCancelAnimation();
    GTEST_LOG_(INFO) << "RSNodeAnimateUnitTest RSNodeAnimateSupplementTest022 end";
}
} // namespace Rosen
} // namespace OHOS