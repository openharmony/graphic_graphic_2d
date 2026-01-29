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