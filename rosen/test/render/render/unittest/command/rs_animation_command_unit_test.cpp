/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "gtest/gtest.h"
#include "include/command/rs_animation_command.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSAnimationCommandUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static void TestProcessor(NodeId, AnimationId, AnimationCallbackEvent);
protected:
    static inline NodeId nodeId_;
    static inline AnimationId animationId_;
};

void RSAnimationCommandUnitTest::SetUpTestCase() {}
void RSAnimationCommandUnitTest::TearDownTestCase() {}
void RSAnimationCommandUnitTest::SetUp() {}
void RSAnimationCommandUnitTest::TearDown() {}
void RSAnimationCommandUnitTest::TestProcessor(NodeId nodeId, AnimationId animId, AnimationCallbackEvent event)
{
    nodeId_ = nodeId;
    animationId_ = animId;
}

/**
 * @tc.name: TestRSAnimationCommand01
 * @tc.desc: Verify function RSAnimationCommand
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSAnimationCommandUnitTest, TestRSAnimationCommand01, TestSize.Level1)
{
    RSContext testContext;
    NodeId targetId = static_cast<NodeId>(-1);
    AnimationId animId = static_cast<AnimationId>(1);
    AnimationCallbackEvent event = static_cast<AnimationCallbackEvent>(1);
    AnimationCommandHelper::SetAnimationCallbackProcessor(TestProcessor);
    AnimationCommandHelper::AnimationCallback(testContext, targetId, animId, event);
    ASSERT_EQ(nodeId_, targetId);
    ASSERT_EQ(animationId_, animId);
}

/**
 * @tc.name: TestCreateParticleAnimation01
 * @tc.desc: Verify function CreateParticleAnimation
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSAnimationCommandUnitTest, TestCreateParticleAnimation01, TestSize.Level1)
{
    RSContext testContext;
    NodeId targetId = static_cast<NodeId>(-1);
    std::shared_ptr<RSRenderParticleAnimation> animation = nullptr;
    AnimationCommandHelper::CreateParticleAnimation(testContext, targetId, animation);
    ASSERT_EQ(targetId, static_cast<NodeId>(-1));

    NodeId id = static_cast<NodeId>(1);
    RSContext testContext2;
    ASSERT_EQ(id, static_cast<NodeId>(1));

    std::shared_ptr<RSBaseRenderNode> node = std::make_shared<RSBaseRenderNode>(id);
    testContext2.GetMutableNodeMap().RegisterRenderNode(node);
    AnimationCommandHelper::CreateParticleAnimation(testContext2, id, nullptr);

    std::shared_ptr<RSRenderParticleAnimation> animation2 = std::make_shared<RSRenderParticleAnimation>();
    AnimationCommandHelper::CreateParticleAnimation(testContext2, id, animation2);

    AnimationCommandHelper::CancelAnimation(testContext2, id, 0);
    EXPECT_NE(node, nullptr);
}

/**
 * @tc.name: TestAnimationCallback01
 * @tc.desc: Verify function AnimationCallback
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSAnimationCommandUnitTest, TestAnimationCallback01, TestSize.Level1)
{
    RSContext testContext;
    NodeId targetId = static_cast<NodeId>(-1);
    AnimationId animId = static_cast<AnimationId>(1);
    AnimationCallbackEvent event = static_cast<AnimationCallbackEvent>(1);
    AnimationCommandHelper::AnimationCallback(testContext, targetId, animId, event);
    EXPECT_TRUE(targetId == -1);

    AnimationCommandHelper::AnimationCallbackProcessor processor = [](NodeId nodeId, AnimationId animId,
                                                                       AnimationCallbackEvent event) {};
    AnimationCommandHelper::SetAnimationCallbackProcessor(processor);
    AnimationCommandHelper::AnimationCallback(testContext, targetId, animId, event);
    EXPECT_TRUE(targetId == -1);
}

/**
 * @tc.name: TestCreateAnimation01
 * @tc.desc: Verify function AnimationCallback
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSAnimationCommandUnitTest, TestCreateAnimation01, TestSize.Level1)
{
    RSContext testContext;
    NodeId targetId = 1;
    std::shared_ptr<RSRenderAnimation> animation = nullptr;
    AnimationCommandHelper::CreateAnimation(testContext, targetId, animation);
    EXPECT_TRUE(targetId);

    animation = std::make_shared<RSRenderAnimation>();
    AnimationCommandHelper::CreateAnimation(testContext, targetId, animation);
    auto nod = testContext.GetNodeMap().GetRenderNode<RSRenderNode>(targetId);
    EXPECT_TRUE(targetId == 1);

    targetId = 0;
    auto node = testContext.GetNodeMap().GetRenderNode<RSRenderNode>(targetId);
    PropertyId id = 0;
    auto property = std::shared_ptr<RSRenderProperty<Drawing::Matrix>>();
    auto modifier = std::make_shared<RSGeometryTransRenderModifier>(property);
    node->modifiers_[id] = modifier;
    AnimationCommandHelper::CreateAnimation(testContext, targetId, animation);
    EXPECT_TRUE(modifier != nullptr);

    node->modifiers_.clear();
    id = 1;
    node->modifiers_[id] = modifier;
    AnimationCommandHelper::CreateAnimation(testContext, targetId, animation);
    EXPECT_TRUE(modifier != nullptr);
}

/**
 * @tc.name: TestCreateParticleAnimation02
 * @tc.desc: Verify function CreateParticleAnimation
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSAnimationCommandUnitTest, TestCreateParticleAnimation02, TestSize.Level1)
{
    RSContext testContext;
    NodeId targetId = 1;
    std::shared_ptr<RSRenderParticleAnimation> animation = nullptr;
    AnimationCommandHelper::CreateParticleAnimation(testContext, targetId, animation);
    EXPECT_TRUE(targetId);

    animation = std::make_shared<RSRenderParticleAnimation>();
    AnimationCommandHelper::CreateParticleAnimation(testContext, targetId, animation);
    EXPECT_TRUE(targetId == 1);

    targetId = 0;
    AnimationCommandHelper::CreateParticleAnimation(testContext, targetId, animation);
    EXPECT_TRUE(animation != nullptr);
}

/**
 * @tc.name: TestCancelAnimation01
 * @tc.desc: Verify function CancelAnimation
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSAnimationCommandUnitTest, TestCancelAnimation01, TestSize.Level1)
{
    RSContext testContext;
    NodeId targetId = 1;
    PropertyId propertyId = 1;
    AnimationCommandHelper::CancelAnimation(testContext, targetId, propertyId);
    EXPECT_TRUE(targetId);

    targetId = 0;
    AnimationCommandHelper::CancelAnimation(testContext, targetId, propertyId);
    EXPECT_TRUE(targetId == 0);
}

/**
 * @tc.name: TestInteractiveAnimator01
 * @tc.desc: Verify function InteractiveAnimator
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSAnimationCommandUnitTest, TestInteractiveAnimator01, TestSize.Level1)
{
    RSContext testContext;
    NodeId targetId = 0;
    InteractiveImplictAnimatorId targetIdI = 0;
    float fraction = 1.f;
    std::vector<std::pair<NodeId, AnimationId>> animations;
    EXPECT_TRUE(testContext.GetInteractiveImplictAnimatorMap().GetInteractiveImplictAnimator(targetId) == nullptr);
    AnimationCommandHelper::DestoryInteractiveAnimator(testContext, targetIdI);
    AnimationCommandHelper::InteractiveAnimatorAddAnimations(testContext, targetId, animations);
    AnimationCommandHelper::PauseInteractiveAnimator(testContext, targetIdI);
    AnimationCommandHelper::ContinueInteractiveAnimator(testContext, targetIdI);
    AnimationCommandHelper::FinishInteractiveAnimator(testContext, targetIdI, RSInteractiveAnimationPosition::CURRENT);
    AnimationCommandHelper::ReverseInteractiveAnimator(testContext, targetIdI);
    AnimationCommandHelper::SetFractionInteractiveAnimator(testContext, targetIdI, fraction);
    AnimationCommandHelper::CreateInteractiveAnimator(testContext, targetId, animations, false);
    EXPECT_TRUE(testContext.GetInteractiveImplictAnimatorMap().GetInteractiveImplictAnimator(targetId) != nullptr);

    AnimationCommandHelper::InteractiveAnimatorAddAnimations(testContext, targetId, animations);
    AnimationCommandHelper::PauseInteractiveAnimator(testContext, targetIdI);
    AnimationCommandHelper::ContinueInteractiveAnimator(testContext, targetIdI);
    AnimationCommandHelper::FinishInteractiveAnimator(testContext, targetIdI, RSInteractiveAnimationPosition::CURRENT);
    AnimationCommandHelper::CreateInteractiveAnimator(testContext, targetId, animations, true);
    AnimationCommandHelper::ReverseInteractiveAnimator(testContext, targetIdI);
    AnimationCommandHelper::SetFractionInteractiveAnimator(testContext, targetIdI, fraction);
    AnimationCommandHelper::DestoryInteractiveAnimator(testContext, targetIdI);
    EXPECT_TRUE(testContext.GetInteractiveImplictAnimatorMap().GetInteractiveImplictAnimator(targetId) == nullptr);
}
} // namespace OHOS::Rosen
