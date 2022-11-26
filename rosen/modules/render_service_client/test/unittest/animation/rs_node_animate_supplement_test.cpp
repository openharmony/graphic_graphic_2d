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
    auto node = std::make_shared<RSNodeMock>(false, 0);
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
    auto node = std::make_shared<RSNodeMock>(false, 0);
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

} // namespace Rosen
} // namespace OHOS