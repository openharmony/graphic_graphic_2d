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
#include "modifier/rs_property_modifier.h"
#include "render/rs_path.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using namespace ANIMATIONTEST;
class RSNodeAnimateTest : public RSAnimationBaseTest {
};

class RSNodeMock : public RSNode {
public:
    explicit RSNodeMock(bool isRenderServiceNode) : RSNode(isRenderServiceNode) {};
    explicit RSNodeMock(bool isRenderServiceNode, NodeId id) : RSNode(isRenderServiceNode, id) {};

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
HWTEST_F(RSNodeAnimateTest, RSNodeAnimateSupplementTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSNodeAnimateTest RSNodeAnimateSupplementTest001 start";
    /**
     * @tc.steps: step1. init
     */
    auto node = std::make_shared<RSNodeMock>(false);
    EXPECT_TRUE(node != nullptr);

    RSAnimationTimingProtocol protocol;
    RSAnimationTimingCurve curve;
    RSNode::OpenImplicitAnimation(protocol, curve);
    RSNode::CloseImplicitAnimation();
    GTEST_LOG_(INFO) << "RSNodeAnimateTest RSNodeAnimateSupplementTest001 end";
}

/**
 * @tc.name: RSNodeAnimateSupplementTest002
 * @tc.desc: Verify the animate
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimateTest, RSNodeAnimateSupplementTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSNodeAnimateTest RSNodeAnimateSupplementTest002 start";
    /**
     * @tc.steps: step1. init
     */
    auto node = std::make_shared<RSNodeMock>(false);
    EXPECT_TRUE(node != nullptr);
    RSAnimationTimingProtocol protocol;
    RSAnimationTimingCurve curve;
    std::function<void()> callback = nullptr;
    RSNode::Animate(protocol, curve, callback);
    callback = []() {
        std::cout << "RSNodeAnimateTest RSNodeAnimateSupplementTest002 callback" << std::endl;
    };
    RSNode::Animate(protocol, curve, callback);
    GTEST_LOG_(INFO) << "RSNodeAnimateTest RSNodeAnimateSupplementTest002 end";
}

/**
 * @tc.name: RSNodeAnimateSupplementTest003
 * @tc.desc: Verify the addAnimation
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimateTest, RSNodeAnimateSupplementTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSNodeAnimateTest RSNodeAnimateSupplementTest003 start";
    /**
     * @tc.steps: step1. init
     */
    auto node = std::make_shared<RSNodeMock>(false);
    EXPECT_TRUE(node != nullptr);
    node->AddAnimation(nullptr);
    auto property = std::make_shared<RSAnimatableProperty<float>>(0.0f);
    auto endProperty = std::make_shared<RSAnimatableProperty<float>>(1.0f);
    auto curveAnimation = std::make_shared<RSCurveAnimation>(property, endProperty);
    curveAnimation->SetDuration(300);
    node->AddAnimation(curveAnimation);
    node->AddAnimation(curveAnimation);
    auto property2 = std::make_shared<RSAnimatableProperty<float>>(0.0f);
    auto endProperty2 = std::make_shared<RSAnimatableProperty<float>>(1.0f);
    auto curveAnimation2 = std::make_shared<RSCurveAnimation>(property2, endProperty2);
    curveAnimation2->SetDuration(0);
    node->AddAnimation(curveAnimation2);
    GTEST_LOG_(INFO) << "RSNodeAnimateTest RSNodeAnimateSupplementTest003 end";
}

/**
 * @tc.name: RSNodeAnimateSupplementTest004
 * @tc.desc: Verify the removeAnimation
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimateTest, RSNodeAnimateSupplementTest004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSNodeAnimateTest RSNodeAnimateSupplementTest004 start";
    /**
     * @tc.steps: step1. init
     */
    auto node = std::make_shared<RSNodeMock>(false);
    EXPECT_TRUE(node != nullptr);
    node->AddAnimation(nullptr);
    auto property = std::make_shared<RSAnimatableProperty<float>>(0.0f);
    auto endProperty = std::make_shared<RSAnimatableProperty<float>>(1.0f);
    auto curveAnimation = std::make_shared<RSCurveAnimation>(property, endProperty);
    node->AddAnimation(curveAnimation);
    node->RemoveAllAnimations();

    auto property2 = std::make_shared<RSAnimatableProperty<float>>(0.0f);
    auto endProperty2 = std::make_shared<RSAnimatableProperty<float>>(1.0f);
    auto curveAnimation2 = std::make_shared<RSCurveAnimation>(property2, endProperty2);
    node->AddAnimation(curveAnimation2);
    node->RemoveAnimation(nullptr);
    node->RemoveAnimation(curveAnimation);
    node->RemoveAnimation(curveAnimation2);
    GTEST_LOG_(INFO) << "RSNodeAnimateTest RSNodeAnimateSupplementTest004 end";
}

#ifndef MODIFIER_NG
/**
 * @tc.name: RSNodeAnimateSupplementTest005
 * @tc.desc: Verify the SetMotionPathOption
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimateTest, RSNodeAnimateSupplementTest005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSNodeAnimateTest RSNodeAnimateSupplementTest005 start";
    /**
     * @tc.steps: step1. init
     */
    auto node = std::make_shared<RSNodeMock>(false);
    EXPECT_TRUE(node != nullptr);
    node->SetMotionPathOption(nullptr);
    auto motionPathOption = std::make_shared<RSMotionPathOption>(ANIMATION_PATH);
    auto property1 = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier1 = std::make_shared<RSBoundsModifier>(property1);
    node->AddModifier(modifier1);
    auto property2 = std::make_shared<RSAnimatableProperty<float>>(0.0f);
    auto modifier2 = std::make_shared<RSAlphaModifier>(property2);
    node->AddModifier(modifier2);
    node->SetMotionPathOption(motionPathOption);
    GTEST_LOG_(INFO) << "RSNodeAnimateTest RSNodeAnimateSupplementTest005 end";
}

/**
 * @tc.name: RSNodeAnimateSupplementTest006
 * @tc.desc: Verify the SetBounds
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimateTest, RSNodeAnimateSupplementTest006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSNodeAnimateTest RSNodeAnimateSupplementTest006 start";
    /**
     * @tc.steps: step1. init
     */
    auto node = std::make_shared<RSNodeMock>(false);
    EXPECT_TRUE(node != nullptr);
    node->SetBoundsWidth(100.0f);
    node->SetBoundsHeight(100.0f);
    auto modifier1 = std::make_shared<RSBoundsModifier>(nullptr);
    node->AddModifier(modifier1);
    node->SetBoundsWidth(100.0f);
    node->SetBoundsHeight(100.0f);
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<RSBoundsModifier>(property);
    node->AddModifier(modifier);
    node->SetBoundsWidth(100.0f);
    node->SetBoundsWidth(10.0f);
    node->SetBoundsHeight(100.0f);
    node->SetBoundsHeight(10.0f);
    GTEST_LOG_(INFO) << "RSNodeAnimateTest RSNodeAnimateSupplementTest006 end";
}

/**
 * @tc.name: RSNodeAnimateSupplementTest007
 * @tc.desc: Verify the SetFrame
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimateTest, RSNodeAnimateSupplementTest007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSNodeAnimateTest RSNodeAnimateSupplementTest007 start";
    /**
     * @tc.steps: step1. init
     */
    auto node = std::make_shared<RSNodeMock>(false);
    EXPECT_TRUE(node != nullptr);
    node->SetFramePositionX(100.0f);
    node->SetFramePositionY(100.0f);
    auto modifier1 = std::make_shared<RSFrameModifier>(nullptr);
    node->AddModifier(modifier1);
    node->SetFramePositionX(100.0f);
    node->SetFramePositionY(100.0f);
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<RSFrameModifier>(property);
    node->AddModifier(modifier);
    node->SetFramePositionX(100.0f);
    node->SetFramePositionY(10.0f);
    node->SetFramePositionX(100.0f);
    node->SetFramePositionY(10.0f);
    GTEST_LOG_(INFO) << "RSNodeAnimateTest RSNodeAnimateSupplementTest007 end";
}

/**
 * @tc.name: RSNodeAnimateSupplementTest008
 * @tc.desc: Verify the SetPivot
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimateTest, RSNodeAnimateSupplementTest008, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSNodeAnimateTest RSNodeAnimateSupplementTest008 start";
    /**
     * @tc.steps: step1. init
     */
    auto node = std::make_shared<RSNodeMock>(false);
    EXPECT_TRUE(node != nullptr);
    node->SetPivotX(100.0f);
    node->SetPivotY(100.0f);
    auto modifier1 = std::make_shared<RSFrameModifier>(nullptr);
    node->AddModifier(modifier1);
    node->SetPivotX(100.0f);
    node->SetPivotY(100.0f);
    auto property = std::make_shared<RSAnimatableProperty<Vector2f>>(ANIMATION_NORMAL_SCALE);
    auto modifier = std::make_shared<RSFrameModifier>(property);
    node->AddModifier(modifier);
    node->SetPivotX(100.0f);
    node->SetPivotY(10.0f);
    node->SetPivotX(100.0f);
    node->SetPivotY(10.0f);
    GTEST_LOG_(INFO) << "RSNodeAnimateTest RSNodeAnimateSupplementTest008 end";
}
#endif

/**
 * @tc.name: RSNodeAnimateSupplementTest009
 * @tc.desc: Verify the SetMask
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimateTest, RSNodeAnimateSupplementTest009, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSNodeAnimateTest RSNodeAnimateSupplementTest009 start";
    /**
     * @tc.steps: step1. init
     */
    auto node = std::make_shared<RSNodeMock>(false);
    EXPECT_TRUE(node != nullptr);
    node->SetMask(nullptr);
    GTEST_LOG_(INFO) << "RSNodeAnimateTest RSNodeAnimateSupplementTest009 end";
}

/**
 * @tc.name: RSNodeAnimateSupplementTest010
 * @tc.desc: Verify the OnAddChildren
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimateTest, RSNodeAnimateSupplementTest010, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSNodeAnimateTest RSNodeAnimateSupplementTest010 start";
    /**
     * @tc.steps: step1. init
     */
    auto node = std::make_shared<RSNodeMock>(false);
    EXPECT_TRUE(node != nullptr);
    node->SetVisible(false);
    node->OnAddChildren();
    node->OnRemoveChildren();
    node->SetVisible(true);
    node->OnAddChildren();
    node->OnRemoveChildren();

    auto transitionEffect = RSTransitionEffect::Create()->Rotate(TRANSITION_EFFECT_ROTATE);
    EXPECT_TRUE(transitionEffect != nullptr);
    node->SetTransitionEffect(transitionEffect);
    node->SetVisible(false);
    node->OnAddChildren();
    node->OnRemoveChildren();
    node->SetVisible(true);
    node->OnAddChildren();
    node->OnRemoveChildren();
    GTEST_LOG_(INFO) << "RSNodeAnimateTest RSNodeAnimateSupplementTest010 end";
}

#ifndef MODIFIER_NG
/**
 * @tc.name: RSNodeAnimateSupplementTest011
 * @tc.desc: Verify the AddModifier
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimateTest, RSNodeAnimateSupplementTest011, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSNodeAnimateTest RSNodeAnimateSupplementTest011 start";
    /**
     * @tc.steps: step1. init
     */
    auto node = std::make_shared<RSNodeMock>(false);
    EXPECT_TRUE(node != nullptr);
    node->SetMotionPathOption(nullptr);
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<RSBoundsModifier>(property);
    node->AddModifier(modifier);

    auto motionPathOption = std::make_shared<RSMotionPathOption>(ANIMATION_PATH);
    node->SetMotionPathOption(motionPathOption);
    auto property2 = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier2 = std::make_shared<RSBoundsModifier>(property2);
    node->AddModifier(modifier2);
    auto property3 = std::make_shared<RSAnimatableProperty<float>>(0.0f);
    auto modifier3 = std::make_shared<RSAlphaModifier>(property3);
    node->AddModifier(modifier3);
    auto property4 = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier4 = std::make_shared<RSFrameModifier>(property4);
    node->AddModifier(modifier4);
    auto property5 = std::make_shared<RSAnimatableProperty<Vector2f>>(ANIMATION_NORMAL_SCALE);
    auto modifier5 = std::make_shared<RSTranslateModifier>(property5);
    node->AddModifier(modifier5);
    GTEST_LOG_(INFO) << "RSNodeAnimateTest RSNodeAnimateSupplementTest011 end";
}

/**
 * @tc.name: RSNodeAnimateSupplementTest012
 * @tc.desc: Verify the RemoveModifier
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimateTest, RSNodeAnimateSupplementTest012, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSNodeAnimateTest RSNodeAnimateSupplementTest012 start";
    /**
     * @tc.steps: step1. init
     */
    auto node = std::make_shared<RSNodeMock>(false);
    EXPECT_TRUE(node != nullptr);
    node->RemoveModifier(nullptr);
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto modifier = std::make_shared<RSBoundsModifier>(property);
    node->AddModifier(modifier);
    auto property2 = std::make_shared<RSAnimatableProperty<float>>(0.0f);
    auto modifier2 = std::make_shared<RSAlphaModifier>(property2);
    node->RemoveModifier(modifier2);

    GTEST_LOG_(INFO) << "RSNodeAnimateTest RSNodeAnimateSupplementTest012 end";
}
#endif

/**
 * @tc.name: RSNodeAnimateSupplementTest013
 * @tc.desc: Verify the DumpNode
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimateTest, RSNodeAnimateSupplementTest013, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSNodeAnimateTest RSNodeAnimateSupplementTest013 start";
    /**
     * @tc.steps: step1. init
     */
    auto node = std::make_shared<RSNodeMock>(false);
    EXPECT_TRUE(node != nullptr);
    node->DumpNode(1);
    auto property = std::make_shared<RSAnimatableProperty<float>>(0.0f);
    auto endProperty = std::make_shared<RSAnimatableProperty<float>>(1.0f);
    auto curveAnimation = std::make_shared<RSCurveAnimation>(property, endProperty);
    node->AddAnimation(curveAnimation);
    node->DumpNode(1);
    GTEST_LOG_(INFO) << "RSNodeAnimateTest RSNodeAnimateSupplementTest013 end";
}

/**
 * @tc.name: RSNodeAnimateSupplementTest014
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimateTest, RSNodeAnimateSupplementTest014, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest RSNodeAnimateSupplementTest014 start";
    /**
     * @tc.steps: step1. init
     */
    auto node = std::make_shared<RSNodeMock>(true);
    EXPECT_TRUE(node != nullptr);
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(100);
    RSNode::OpenImplicitAnimation(protocol, RSAnimationTimingCurve::LINEAR);
    RSNode::CloseImplicitAnimation();
    PropertyCallback callback1;
    std::weak_ptr<RSNode> weak = node;
    callback1 = [weak]() {
        auto node2 = weak.lock();
        if (node2) {
            node2->SetAlpha(0.2f);
        }
    };
    RSNode::AddKeyFrame(0.1f, RSAnimationTimingCurve::LINEAR, callback1);
    RSNode::AddKeyFrame(0.2f, callback1);
    PropertyCallback callback2;
    RSNode::Animate(protocol, RSAnimationTimingCurve::LINEAR, callback2);
    callback1 = [weak]() {
        auto node2 = weak.lock();
        if (node2) {
            node2->SetAlpha(0.3f);
        }
    };
    RSNode::Animate(protocol, RSAnimationTimingCurve::LINEAR, callback2);
    GTEST_LOG_(INFO) << "RSAnimationTest RSNodeAnimateSupplementTest014 end";
}

/**
 * @tc.name: RSNodeAnimateSupplementTest015
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimateTest, RSNodeAnimateSupplementTest015, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest RSNodeAnimateSupplementTest015 start";
    /**
     * @tc.steps: step1. init
     */
    auto node = std::make_shared<RSNodeMock>(true);
    EXPECT_TRUE(node != nullptr);
    std::shared_ptr<RSCurveAnimation> curveAnimation;
    node->AddAnimation(curveAnimation);
    auto property = std::make_shared<RSAnimatableProperty<float>>(0.0f);
    auto endProperty = std::make_shared<RSAnimatableProperty<float>>(1.0f);
    curveAnimation = std::make_shared<RSCurveAnimation>(property, endProperty);
    curveAnimation->SetDuration(0);
    node->AddAnimation(curveAnimation);
    node->AddAnimation(curveAnimation);
    GTEST_LOG_(INFO) << "RSAnimationTest RSNodeAnimateSupplementTest015 end";
}

/**
 * @tc.name: RSNodeAnimateSupplementTest016
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimateTest, RSNodeAnimateSupplementTest016, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest RSNodeAnimateSupplementTest016 start";
    /**
     * @tc.steps: step1. init
     */
    auto node = std::make_shared<RSNodeMock>(true);
    EXPECT_TRUE(node != nullptr);
    std::shared_ptr<RSCurveAnimation> curveAnimation;
    node->RemoveAllAnimations();
    node->RemoveAnimation(curveAnimation);
    auto property = std::make_shared<RSAnimatableProperty<float>>(0.0f);
    auto endProperty = std::make_shared<RSAnimatableProperty<float>>(1.0f);
    curveAnimation = std::make_shared<RSCurveAnimation>(property, endProperty);
    node->RemoveAnimation(curveAnimation);
    GTEST_LOG_(INFO) << "RSAnimationTest RSNodeAnimateSupplementTest016 end";
}

#ifndef MODIFIER_NG
/**
 * @tc.name: RSNodeAnimateSupplementTest017
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimateTest, RSNodeAnimateSupplementTest017, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest RSNodeAnimateSupplementTest017 start";
    /**
     * @tc.steps: step1. init
     */
    auto node = std::make_shared<RSNodeMock>(true);
    EXPECT_TRUE(node != nullptr);
    std::shared_ptr<RSMotionPathOption> option = std::make_shared<RSMotionPathOption>("abc");
    node->SetMotionPathOption(option);
    node->SetBounds(1.f, 2.f, 3.f, 4.f);
    node->SetMotionPathOption(option);
    Vector4f data(2.f, 3.f, 4.f, 5.f);
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(data);
    auto modifier = std::make_shared<RSBoundsModifier>(property);
    node->AddModifier(modifier);
    node->SetMotionPathOption(option);
    node->SetAlphaOffscreen(true);
    GTEST_LOG_(INFO) << "RSAnimationTest RSNodeAnimateSupplementTest017 end";
}

/**
 * @tc.name: RSNodeAnimateSupplementTest018
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimateTest, RSNodeAnimateSupplementTest018, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest RSNodeAnimateSupplementTest018 start";
    /**
     * @tc.steps: step1. init
     */
    auto node = std::make_shared<RSNodeMock>(true);
    EXPECT_TRUE(node != nullptr);
    Vector4f data(2.f, 3.f, 4.f, 5.f);
    auto property1 = std::make_shared<RSAnimatableProperty<Vector4f>>(data);
    auto modifier1 = std::make_shared<RSBoundsModifier>(property1);
    node->AddModifier(modifier1);
    auto modifier2 = std::make_shared<RSFrameModifier>(property1);
    node->AddModifier(modifier2);
    node->SetBoundsWidth(10.f);
    node->SetBoundsHeight(20.f);
    node->SetFramePositionX(5.f);
    node->SetFramePositionY(6.f);
    GTEST_LOG_(INFO) << "RSAnimationTest RSNodeAnimateSupplementTest018 end";
}
#endif

/**
 * @tc.name: RSNodeAnimateSupplementTest019
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimateTest, RSNodeAnimateSupplementTest019, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest RSNodeAnimateSupplementTest019 start";
    /**
     * @tc.steps: step1. init
     */
    auto node = std::make_shared<RSNodeMock>(true);
    EXPECT_TRUE(node != nullptr);
    std::shared_ptr<RSMask> mask = std::make_shared<RSMask>();
    node->SetMask(mask);
    std::shared_ptr<const RSTransitionEffect> effect = RSTransitionEffect::OPACITY;
    node->SetTransitionEffect(effect);
    node->SetVisible(true);
    node->OnRemoveChildren();
    GTEST_LOG_(INFO) << "RSAnimationTest RSNodeAnimateSupplementTest019 end";
}

#ifndef MODIFIER_NG
/**
 * @tc.name: RSNodeAnimateSupplementTest020
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimateTest, RSNodeAnimateSupplementTest020, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest RSNodeAnimateSupplementTest020 start";
    /**
     * @tc.steps: step1. init
     */
    auto node = std::make_shared<RSNodeMock>(true);
    EXPECT_TRUE(node != nullptr);
    std::shared_ptr<RSMotionPathOption> option = std::make_shared<RSMotionPathOption>("abc");
    node->SetMotionPathOption(option);
    Vector4f data(2.f, 3.f, 4.f, 5.f);
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(data);
    auto modifier = std::make_shared<RSBoundsModifier>(property);
    node->AddModifier(modifier);
    node->RemoveModifier(modifier);
    node->RemoveModifier(modifier);
    GTEST_LOG_(INFO) << "RSAnimationTest RSNodeAnimateSupplementTest020 end";
}

/**
 * @tc.name: RSNodeAnimateSupplementTest021
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimateTest, RSNodeAnimateSupplementTest021, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest RSNodeAnimateSupplementTest021 start";
    /**
     * @tc.steps: step1. init
     */
    auto node = std::make_shared<RSNodeMock>(true);
    EXPECT_TRUE(node != nullptr);
    auto property = std::make_shared<RSAnimatableProperty<float>>(1.f);
    auto modifier = std::make_shared<RSAlphaModifier>(property);
    node->AddModifier(modifier);
    auto ids = node->GetModifierIds();
    EXPECT_TRUE(!ids.empty());
    node->DumpNode(10);
    GTEST_LOG_(INFO) << "RSAnimationTest RSNodeAnimateSupplementTest021 end";
}
#endif

/**
 * @tc.name: RSNodeAnimateSupplementTest022
 * @tc.desc: Verify the create RSNode
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimateTest, RSNodeAnimateSupplementTest022, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSNodeAnimateTest RSNodeAnimateSupplementTest022 start";
    /**
     * @tc.steps: step1. init
     */
    auto node = std::make_shared<RSNodeMock>(false);
    EXPECT_TRUE(node != nullptr);

    RSAnimationTimingProtocol protocol;
    RSAnimationTimingCurve curve;
    RSNode::OpenImplicitAnimation(protocol, curve);
    RSNode::CloseImplicitCancelAnimation();
    GTEST_LOG_(INFO) << "RSNodeAnimateTest RSNodeAnimateSupplementTest022 end";
}

/**
 * @tc.name: RSNodeAnimateSupplementTest023
 * @tc.desc: Verify the setcallback of Animation
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimateTest, RSNodeAnimateSupplementTest023, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationTest RSNodeAnimateSupplementTest023 start";
    /**
     * @tc.steps: step1. init
     */
    auto node = std::make_shared<RSNodeMock>(true);
    EXPECT_TRUE(node != nullptr);
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(100);
    RSNode::OpenImplicitAnimation(protocol, RSAnimationTimingCurve::LINEAR);
    RSNode::CloseImplicitAnimation();
    PropertyCallback callback1;
    std::weak_ptr<RSNode> weak = node;
    callback1 = [weak]() {
        auto node2 = weak.lock();
        if (node2) {
            node2->SetAlpha(0.2f);
        }
    };
    RSNode::AddKeyFrame(nullptr, 0.1f, RSAnimationTimingCurve::LINEAR, callback1);
    RSNode::AddKeyFrame(nullptr, 0.2f, callback1);
    PropertyCallback callback2;
    RSNode::Animate(protocol, RSAnimationTimingCurve::LINEAR, callback2);
    callback1 = [weak]() {
        auto node2 = weak.lock();
        if (node2) {
            node2->SetAlpha(0.3f);
        }
    };
    RSNode::Animate(protocol, RSAnimationTimingCurve::LINEAR, callback2);
    GTEST_LOG_(INFO) << "RSAnimationTest RSNodeAnimateSupplementTest023 end";
}

/**
 * @tc.name: RSNodeAnimateSupplementTest024
 * @tc.desc: Verify CloseImplicitCancelAnimationReturnStatus
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimateTest, RSNodeAnimateSupplementTest024, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSNodeAnimateTest RSNodeAnimateSupplementTest024 start";
    auto rsUIContext = std::make_shared<RSUIContext>();
    auto ret = RSNode::CloseImplicitCancelAnimationReturnStatus(rsUIContext);
    EXPECT_EQ(ret, CancelAnimationStatus::NO_OPEN_CLOSURE);
    GTEST_LOG_(INFO) << "RSNodeAnimateTest RSNodeAnimateSupplementTest024 end";
}

/**
 * @tc.name: RSNodeAnimateSupplementTest025
 * @tc.desc: Verify CloseImplicitCancelAnimationReturnStatus
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimateTest, RSNodeAnimateSupplementTest025, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSNodeAnimateTest RSNodeAnimateSupplementTest025 start";
    RSAnimationTimingProtocol protocol;
    RSAnimationTimingCurve curve;
    RSNode::OpenImplicitAnimation(protocol, curve);
    auto ret = RSNode::CloseImplicitCancelAnimationReturnStatus();
    EXPECT_EQ(ret, CancelAnimationStatus::INCORRECT_PARAM_TYPE);
    GTEST_LOG_(INFO) << "RSNodeAnimateTest RSNodeAnimateSupplementTest025 end";
}

/**
 * @tc.name: RSNodeAnimateSupplementTest025
 * @tc.desc: Verify CloseImplicitCancelAnimationReturnStatus
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimateTest, RSNodeAnimateSupplementTest026, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSNodeAnimateTest RSNodeAnimateSupplementTest026 start";
    RSAnimationTimingProtocol protocol;
    RSAnimationTimingCurve curve;
    protocol.SetDuration(0);
    RSNode::OpenImplicitAnimation(protocol, curve);
    auto ret = RSNode::CloseImplicitCancelAnimationReturnStatus();
    EXPECT_EQ(ret, CancelAnimationStatus::EMPTY_PENDING_SYNC_LIST);
    GTEST_LOG_(INFO) << "RSNodeAnimateTest RSNodeAnimateSupplementTest026 end";
}

#ifndef MODIFIER_NG
/**
 * @tc.name: RSNodeAnimateSupplementTest026
 * @tc.desc: Verify CloseImplicitCancelAnimationReturnStatus
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimateTest, RSNodeAnimateSupplementTest027, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSNodeAnimateTest RSNodeAnimateSupplementTest027 start";
    auto node = std::make_shared<RSNodeMock>(false);
    EXPECT_TRUE(node != nullptr);
    auto alphaProperty = std::make_shared<RSAnimatableProperty<float>>(0.1f);
    auto alphaModifier = std::make_shared<RSAlphaModifier>(alphaProperty);
    node->AddModifier(alphaModifier);

    RSAnimationTimingProtocol protocol;
    RSAnimationTimingCurve curve;
    RSNode::Animate(protocol, curve, [&]() {
        alphaProperty->Set(1.f);
    });
    protocol.SetDuration(0);
    RSNode::OpenImplicitAnimation(protocol, curve);
    alphaProperty->RequestCancelAnimation();
    auto ret = RSNode::CloseImplicitCancelAnimationReturnStatus();
    EXPECT_EQ(ret, CancelAnimationStatus::SUCCESS);
    GTEST_LOG_(INFO) << "RSNodeAnimateTest RSNodeAnimateSupplementTest027 end";
}
#endif

/**
 * @tc.name: RSNodeAnimateSupplementTest028
 * @tc.desc: Verify the RSNode::CloseImplicitCancelAnimation with multi-instance
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeAnimateTest, RSNodeAnimateSupplementTest028, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSNodeAnimateTest RSNodeAnimateSupplementTest028 start";
    RSAnimationTimingProtocol protocol;
    RSAnimationTimingCurve curve;
    auto rsUIContext = std::make_shared<RSUIContext>();
    RSNode::OpenImplicitAnimation(rsUIContext, protocol, curve);
    auto ret = RSNode::CloseImplicitCancelAnimation(rsUIContext);
    EXPECT_FALSE(ret);
    GTEST_LOG_(INFO) << "RSNodeAnimateTest RSNodeAnimateSupplementTest028 end";
}
} // namespace Rosen
} // namespace OHOS