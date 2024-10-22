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
class RSAnimatCmdTest : public testing::Test {
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

void RSAnimatCmdTest::SetUpTestCase() {}
void RSAnimatCmdTest::TearDownTestCase() {}
void RSAnimatCmdTest::SetUp() {}
void RSAnimatCmdTest::TearDown() {}
void RSAnimatCmdTest::TestProcessor(NodeId nodeId, AnimationId animId, AnimationCallbackEvent event)
{
    nodeId_ = nodeId;
    animationId_ = animId;
}

/**
 * @tc.name: CreateParticleAnimation001
 * @tc.desc: CreateParticleAnimation test.
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimatCmdExtTest, CreateParticleAnimationExtTest001, TestSize.Level1)
{
    RSContext context;
    NodeId targetId = static_cast<NodeId>(-1);
    std::shared_ptr<RSRenderParticleAnimation> animation = nullptr;
    AnimatCommandHelper::CreateParticleAnimation(context, targetId, animation);

    NodeId id = static_cast<NodeId>(1);
    RSContext context2;

    std::shared_ptr<RSBaseRenderNode> node = std::make_shared<RSBaseRenderNode>(id);
    context2.GetMutableNodeMap().RegisterRenderNode(node);
    AnimatCommandHelper::CreateParticleAnimation(context2, id, nullptr);

    std::shared_ptr<RSRenderParticleAnimation> animation2 = std::make_shared<RSRenderParticleAnimation>();
    AnimatCommandHelper::CreateParticleAnimation(context2, id, animation2);

    AnimatCommandHelper::CancelAnimation(context2, id, 0);
}

/**
 * @tc.name: TestRSAnimatCmdExtTest001
 * @tc.desc: AnimationFinishCallback test.
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimatCmdExtTest, TestRSAnimatCmdExtTest001, TestSize.Level1)
{
    RSContext context;
    NodeId targetId = static_cast<NodeId>(-1);
    AnimationId animId = static_cast<AnimationId>(1);
    AnimationCallbackEvent event = static_cast<AnimationCallbackEvent>(1);
    AnimatCommandHelper::SetAnimationCallbackProcessor(TestProcessor);
    AnimatCommandHelper::AnimationCallback(context, targetId, animId, event);
    ASSERT_EQ(nodeId_, targetId);
    ASSERT_EQ(animationId_, animId);
}

/**
 * @tc.name: CreateAnimationExtTest001
 * @tc.desc: test results of CreateAnimation
 * @tc.type: FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RSAnimatCmdExtTest, CreateAnimationExtTest001, TestSize.Level1)
{
    RSContext context;
    NodeId targetId = 1;
    std::shared_ptr<RSRenderAnimation> animation = nullptr;
    AnimatCommandHelper::CreateAnimation(context, targetId, animation);
    EXPECT_TRUE(targetId);

    animation = std::make_shared<RSRenderAnimation>();
    AnimatCommandHelper::CreateAnimation(context, targetId, animation);
    auto nod = context.GetNodeMap().GetRenderNode<RSRenderNode>(targetId);
    EXPECT_TRUE(targetId == 1);

    targetId = 0;
    auto node = context.GetNodeMap().GetRenderNode<RSRenderNode>(targetId);
    PropertyId id = 0;
    auto property = std::shared_ptr<RSRenderProperty<Drawing::Matrix>>();
    auto modifier = std::make_shared<RSGeometryTransRenderModifier>(property);
    node->modifiers_[id] = modifier;
    AnimatCommandHelper::CreateAnimation(context, targetId, animation);
    EXPECT_TRUE(modifier != nullptr);

    node->modifiers_.clear();
    id = 1;
    node->modifiers_[id] = modifier;
    AnimatCommandHelper::CreateAnimation(context, targetId, animation);
    EXPECT_TRUE(modifier != nullptr);
}

/**
 * @tc.name: AnimationCallbackExtTest001
 * @tc.desc: test results of AnimationCallback
 * @tc.type: FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(RSAnimatCmdExtTest, AnimationCallbackExtTest001, TestSize.Level1)
{
    RSContext context;
    NodeId targetId = static_cast<NodeId>(-1);
    AnimationId animId = static_cast<AnimationId>(1);
    AnimationCallbackEvent event = static_cast<AnimationCallbackEvent>(1);
    AnimatCommandHelper::AnimationCallback(context, targetId, animId, event);
    EXPECT_TRUE(targetId == -1);

    AnimatCommandHelper::AnimationCallbackProcessor processor = [](NodeId nodeId, AnimationId animId,
                                                                       AnimationCallbackEvent event) {};
    AnimatCommandHelper::SetAnimationCallbackProcessor(processor);
    AnimatCommandHelper::AnimationCallback(context, targetId, animId, event);
    EXPECT_TRUE(targetId == -1);
}

/**
 * @tc.name: CancelAnimationExtTest001
 * @tc.desc: test results of CancelAnimation
 * @tc.type: FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(RSAnimatCmdExtTest, CancelAnimationExtTest001, TestSize.Level1)
{
    RSContext context;
    NodeId targetId = 1;
    PropertyId propertyId = 1;
    AnimatCommandHelper::CancelAnimation(context, targetId, propertyId);
    EXPECT_TRUE(targetId);

    targetId = 0;
    AnimatCommandHelper::CancelAnimation(context, targetId, propertyId);
    EXPECT_TRUE(targetId == 0);
}

/**
 * @tc.name: CreateParticleAnimationExtTest002
 * @tc.desc: test results of CreateParticleAnimation
 * @tc.type: FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RSAnimatCmdExTest, CreateParticleAnimationExtTest002, TestSize.Level1)
{
    RSContext context;
    NodeId targetId = 1;
    std::shared_ptr<RSRenderParticleAnimation> animation = nullptr;
    AnimatCommandHelper::CreateParticleAnimation(context, targetId, animation);
    EXPECT_TRUE(targetId);

    animation = std::make_shared<RSRenderParticleAnimation>();
    AnimatCommandHelper::CreateParticleAnimation(context, targetId, animation);
    EXPECT_TRUE(targetId == 1);

    targetId = 0;
    AnimatCommandHelper::CreateParticleAnimation(context, targetId, animation);
    EXPECT_TRUE(animation != nullptr);
}

/**
 * @tc.name: InteractiveAnimatorExtTest001
 * @tc.desc: InteractiveAnimator test.
 * @tc.type: FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RSAnimatCmdExtTest, InteractiveAnimatorExtTest001, TestSize.Level1)
{
    RSContext context;
    NodeId targetId = 0;
    InteractiveImplictAnimatorId targetIdI = 0;
    float fraction = 1.f;
    std::vector<std::pair<NodeId, AnimationId>> animations;
    EXPECT_TRUE(context.GetInteractiveImplictAnimatorMap().GetInteractiveImplictAnimator(targetId) == nullptr);
    AnimatCommandHelper::DestoryInteractiveAnimator(context, targetIdI);
    AnimatCommandHelper::InteractiveAnimatorAddAnimations(context, targetId, animations);
    AnimatCommandHelper::PauseInteractiveAnimator(context, targetIdI);
    AnimatCommandHelper::ContinueInteractiveAnimator(context, targetIdI);
    AnimatCommandHelper::FinishInteractiveAnimator(context, targetIdI, RSInteractiveAnimationPosition::CURRENT);
    AnimatCommandHelper::ReverseInteractiveAnimator(context, targetIdI);
    AnimatCommandHelper::SetFractionInteractiveAnimator(context, targetIdI, fraction);
    AnimatCommandHelper::CreateInteractiveAnimator(context, targetId, animations, false);
    EXPECT_TRUE(context.GetInteractiveImplictAnimatorMap().GetInteractiveImplictAnimator(targetId) != nullptr);

    AnimatCommandHelper::InteractiveAnimatorAddAnimations(context, targetId, animations);
    AnimatCommandHelper::PauseInteractiveAnimator(context, targetIdI);
    AnimatCommandHelper::ContinueInteractiveAnimator(context, targetIdI);
    AnimatCommandHelper::FinishInteractiveAnimator(context, targetIdI, RSInteractiveAnimationPosition::CURRENT);
    AnimatCommandHelper::CreateInteractiveAnimator(context, targetId, animations, true);
    AnimatCommandHelper::ReverseInteractiveAnimator(context, targetIdI);
    AnimatCommandHelper::SetFractionInteractiveAnimator(context, targetIdI, fraction);
    AnimatCommandHelper::DestoryInteractiveAnimator(context, targetIdI);
    EXPECT_TRUE(context.GetInteractiveImplictAnimatorMap().GetInteractiveImplictAnimator(targetId) == nullptr);
}
} // namespace OHOS::Rosen
