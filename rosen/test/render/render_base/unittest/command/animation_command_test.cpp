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
class AnimationCommandTest : public testing::Test {
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

void AnimationCommandTest::SetUpTestCase() {}
void AnimationCommandTest::TearDownTestCase() {}
void AnimationCommandTest::SetUp() {}
void AnimationCommandTest::TearDown() {}
void AnimationCommandTest::TestProcessor(NodeId nodeId, AnimationId animId, AnimationCallbackEvent event)
{
    nodeId_ = nodeId;
    animationId_ = animId;
}

/**
 * @tc.name: InteractiveAnimator001
 * @tc.desc: InteractiveAnimator test.
 * @tc.type: FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(AnimationCommandTest, InteractiveAnimator01, TestSize.Level1)
{
    RSContext context;
    NodeId targetId = 0;
    InteractiveImplictAnimatorId targetIdI = 0;
    float fraction = 1.f;
    std::vector<std::pair<NodeId, AnimationId>> animations;
    EXPECT_TRUE(context.GetInteractiveImplictAnimatorMap().GetInteractiveImplictAnimator(targetId) == nullptr);
    
    AnimationCommandHelper::InteractiveAnimatorAddAnimations(context, targetId, animations);
    AnimationCommandHelper::PauseInteractiveAnimator(context, targetIdI);
    AnimationCommandHelper::ContinueInteractiveAnimator(context, targetIdI);
    AnimationCommandHelper::FinishInteractiveAnimator(context, targetIdI, RSInteractiveAnimationPosition::CURRENT);
    AnimationCommandHelper::CreateInteractiveAnimator(context, targetId, animations, true);
    AnimationCommandHelper::ReverseInteractiveAnimator(context, targetIdI);
    AnimationCommandHelper::SetFractionInteractiveAnimator(context, targetIdI, fraction);
    AnimationCommandHelper::DestoryInteractiveAnimator(context, targetIdI);
    EXPECT_TRUE(context.GetInteractiveImplictAnimatorMap().GetInteractiveImplictAnimator(targetId) == nullptr);

    AnimationCommandHelper::DestoryInteractiveAnimator(context, targetIdI);
    AnimationCommandHelper::InteractiveAnimatorAddAnimations(context, targetId, animations);
    AnimationCommandHelper::PauseInteractiveAnimator(context, targetIdI);
    AnimationCommandHelper::ContinueInteractiveAnimator(context, targetIdI);
    AnimationCommandHelper::FinishInteractiveAnimator(context, targetIdI, RSInteractiveAnimationPosition::CURRENT);
    AnimationCommandHelper::ReverseInteractiveAnimator(context, targetIdI);
    AnimationCommandHelper::SetFractionInteractiveAnimator(context, targetIdI, fraction);
    AnimationCommandHelper::CreateInteractiveAnimator(context, targetId, animations, false);
    EXPECT_TRUE(context.GetInteractiveImplictAnimatorMap().GetInteractiveImplictAnimator(targetId) != nullptr);
}

/**
 * @tc.name: CreateParticleAnimation001
 * @tc.desc: CreateParticleAnimation test.
 * @tc.type: FUNC
 */
HWTEST_F(AnimationCommandTest, CreateParticleAnimation01, TestSize.Level1)
{
    RSContext context;
    NodeId targetId = static_cast<NodeId>(-1);
    std::shared_ptr<RSRenderParticleAnimation> animation = nullptr;
    AnimationCommandHelper::CreateParticleAnimation(context, targetId, animation);

    NodeId id = static_cast<NodeId>(1);
    RSContext context2;

    std::shared_ptr<RSBaseRenderNode> node = std::make_shared<RSBaseRenderNode>(id);
    context2.GetMutableNodeMap().RegisterRenderNode(node);
    AnimationCommandHelper::CreateParticleAnimation(context2, id, nullptr);

    std::shared_ptr<RSRenderParticleAnimation> animation2 = std::make_shared<RSRenderParticleAnimation>();
    AnimationCommandHelper::CreateParticleAnimation(context2, id, animation2);

    AnimationCommandHelper::CancelAnimation(context2, id, 0);
    EXPECT_NE(context, nullptr);
}

/**
 * @tc.name: CreateParticleAnimation002
 * @tc.desc: test results of CreateParticleAnimation
 * @tc.type: FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(AnimationCommandTest, CreateParticleAnimation02, TestSize.Level1)
{
    RSContext context;
    NodeId targetId = 1;
    std::shared_ptr<RSRenderParticleAnimation> animation = nullptr;
    AnimationCommandHelper::CreateParticleAnimation(context, targetId, animation);
    EXPECT_TRUE(targetId);

    animation = std::make_shared<RSRenderParticleAnimation>();
    AnimationCommandHelper::CreateParticleAnimation(context, targetId, animation);
    EXPECT_TRUE(targetId == 1);

    targetId = 0;
    AnimationCommandHelper::CreateParticleAnimation(context, targetId, animation);
    EXPECT_TRUE(animation != nullptr);
}

/**
 * @tc.name: AnimationCallback001
 * @tc.desc: test results of AnimationCallback
 * @tc.type: FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(AnimationCommandTest, AnimationCallback01, TestSize.Level1)
{
    RSContext context;
    NodeId targetId = static_cast<NodeId>(-1);
    AnimationId animId = static_cast<AnimationId>(1);
    AnimationCallbackEvent event = static_cast<AnimationCallbackEvent>(1);
    AnimationCommandHelper::AnimationCallback(context, targetId, animId, event);
    EXPECT_TRUE(targetId == -1);

    AnimationCommandHelper::AnimationCallbackProcessor processor = [](NodeId nodeId, AnimationId animId,
                                                                       AnimationCallbackEvent event) {};
    AnimationCommandHelper::SetAnimationCallbackProcessor(processor);
    AnimationCommandHelper::AnimationCallback(context, targetId, animId, event);
    EXPECT_TRUE(targetId == -1);
}

/**
 * @tc.name: CreateAnimation001
 * @tc.desc: test results of CreateAnimation
 * @tc.type: FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(AnimationCommandTest, CreateAnimation01, TestSize.Level1)
{
    RSContext context;
    NodeId targetId = 1;
    std::shared_ptr<RSRenderAnimation> animation = nullptr;
    AnimationCommandHelper::CreateAnimation(context, targetId, animation);
    EXPECT_TRUE(targetId);

    animation = std::make_shared<RSRenderAnimation>();
    AnimationCommandHelper::CreateAnimation(context, targetId, animation);
    auto nod = context.GetNodeMap().GetRenderNode<RSRenderNode>(targetId);
    EXPECT_TRUE(targetId == 1);

    targetId = 0;
    auto node = context.GetNodeMap().GetRenderNode<RSRenderNode>(targetId);
    PropertyId id = 0;
    auto property = std::shared_ptr<RSRenderProperty<Drawing::Matrix>>();
    auto modifier = std::make_shared<RSGeometryTransRenderModifier>(property);
    node->modifiers_[id] = modifier;
    AnimationCommandHelper::CreateAnimation(context, targetId, animation);
    EXPECT_TRUE(modifier != nullptr);

    node->modifiers_.clear();
    id = 1;
    node->modifiers_[id] = modifier;
    AnimationCommandHelper::CreateAnimation(context, targetId, animation);
    EXPECT_TRUE(modifier != nullptr);
}

/**
 * @tc.name: CancelAnimation001
 * @tc.desc: test results of CancelAnimation
 * @tc.type: FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(AnimationCommandTest, CancelAnimation01, TestSize.Level1)
{
    RSContext context;
    NodeId targetId = 1;
    PropertyId propertyId = 1;
    AnimationCommandHelper::CancelAnimation(context, targetId, propertyId);
    EXPECT_TRUE(targetId);

    targetId = 0;
    AnimationCommandHelper::CancelAnimation(context, targetId, propertyId);
    EXPECT_TRUE(targetId == 0);
}

/**
 * @tc.name: TestRSAnimationCommand001
 * @tc.desc: AnimationFinishCallback test.
 * @tc.type: FUNC
 */
HWTEST_F(AnimationCommandTest, TestRSAnimationCommand01, TestSize.Level1)
{
    RSContext context;
    NodeId targetId = static_cast<NodeId>(-1);
    AnimationId animId = static_cast<AnimationId>(1);
    AnimationCallbackEvent event = static_cast<AnimationCallbackEvent>(1);
    AnimationCommandHelper::SetAnimationCallbackProcessor(TestProcessor);
    AnimationCommandHelper::AnimationCallback(context, targetId, animId, event);
    ASSERT_EQ(nodeId_, targetId);
    ASSERT_EQ(animationId_, animId);
}

} // namespace OHOS::Rosen
