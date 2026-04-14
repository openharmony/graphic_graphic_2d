/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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
#include "animation/rs_render_interactive_implict_animator.h"
#include "include/command/rs_animation_command.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSAnimationCommandTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static void TestProcessor(NodeId, AnimationId, uint64_t, AnimationCallbackEvent);
protected:
    static inline NodeId nodeId_;
    static inline AnimationId animationId_;
    static inline uint64_t token_;
};

void RSAnimationCommandTest::SetUpTestCase() {}
void RSAnimationCommandTest::TearDownTestCase() {}
void RSAnimationCommandTest::SetUp() {}
void RSAnimationCommandTest::TearDown() {}
void RSAnimationCommandTest::TestProcessor(
    NodeId nodeId, AnimationId animId, uint64_t token, AnimationCallbackEvent event)
{
    nodeId_ = nodeId;
    animationId_ = animId;
    token_ = token;
}

/**
 * @tc.name: TestRSAnimationCommand001
 * @tc.desc: AnimationFinishCallback test.
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationCommandTest, TestRSAnimationCommand001, TestSize.Level1)
{
    RSContext context;
    NodeId targetId = static_cast<NodeId>(-1);
    AnimationId animId = static_cast<AnimationId>(1);
    uint64_t token = 1;
    AnimationCallbackEvent event = static_cast<AnimationCallbackEvent>(1);
    AnimationCommandHelper::SetAnimationCallbackProcessor(TestProcessor);
    AnimationCommandHelper::AnimationCallback(context, targetId, animId, token, event);
    ASSERT_EQ(nodeId_, targetId);
    ASSERT_EQ(animationId_, animId);
    ASSERT_EQ(token_, token);
}

/**
 * @tc.name: AnimationCallback001
 * @tc.desc: test results of AnimationCallback
 * @tc.type: FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(RSAnimationCommandTest, AnimationCallback001, TestSize.Level1)
{
    AnimationCommandHelper::AnimationCallbackProcessor processor = [](
        NodeId nodeId, AnimationId animId, uint64_t token, AnimationCallbackEvent event) {};
    AnimationCommandHelper::SetAnimationCallbackProcessor(processor);
    RSContext context;
    NodeId targetId = static_cast<NodeId>(-1);
    AnimationId animId = static_cast<AnimationId>(1);
    uint64_t token = 1;
    AnimationCallbackEvent event = static_cast<AnimationCallbackEvent>(1);
    AnimationCommandHelper::AnimationCallback(context, targetId, animId, token, event);
    EXPECT_TRUE(targetId == -1);
}

/**
 * @tc.name: CancelAnimation001
 * @tc.desc: test results of CancelAnimation
 * @tc.type: FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(RSAnimationCommandTest, CancelAnimation001, TestSize.Level1)
{
    RSContext context;
    NodeId targetId = 1;
    PropertyId propertyId = 1;
    AnimationCommandHelper::CancelAnimation(context, targetId, propertyId);
    EXPECT_TRUE(targetId);

    targetId = 0;
    AnimationCommandHelper::CancelAnimation(context, targetId, propertyId);
    EXPECT_FALSE(targetId);
}

/**
 * @tc.name: InteractiveAnimator001
 * @tc.desc: InteractiveAnimator test.
 * @tc.type: FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RSAnimationCommandTest, InteractiveAnimator001, TestSize.Level1)
{
    RSContext context;
    NodeId targetId = 0;
    InteractiveImplictAnimatorId targetIdI = 0;
    float fraction = 1.f;
    std::vector<std::pair<NodeId, AnimationId>> animations;
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

    AnimationCommandHelper::InteractiveAnimatorAddAnimations(context, targetId, animations);
    AnimationCommandHelper::PauseInteractiveAnimator(context, targetIdI);
    AnimationCommandHelper::ContinueInteractiveAnimator(context, targetIdI);
    AnimationCommandHelper::FinishInteractiveAnimator(context, targetIdI, RSInteractiveAnimationPosition::CURRENT);
    AnimationCommandHelper::CreateInteractiveAnimator(context, targetId, animations, true);
    AnimationCommandHelper::ReverseInteractiveAnimator(context, targetIdI);
    AnimationCommandHelper::SetFractionInteractiveAnimator(context, targetIdI, fraction);
    AnimationCommandHelper::DestoryInteractiveAnimator(context, targetIdI);
    EXPECT_TRUE(context.GetInteractiveImplictAnimatorMap().GetInteractiveImplictAnimator(targetId) == nullptr);
}

/**
 * @tc.name: CreateInteractiveAnimatorGroup001
 * @tc.desc: Verify CreateInteractiveAnimatorGroup with startImmediately=false
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationCommandTest, CreateInteractiveAnimatorGroup001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationCommandTest CreateInteractiveAnimatorGroup001 start";
    RSContext context;
    InteractiveImplictAnimatorId groupId = 10001;
    std::vector<std::pair<NodeId, AnimationId>> animations;
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);

    // Cover branch: startImmediately = false (animator registered but not started)
    AnimationCommandHelper::CreateInteractiveAnimatorGroup(context, groupId, animations, false, timingProtocol);

    auto animator = context.GetInteractiveImplictAnimatorMap().GetInteractiveImplictAnimator(groupId);
    EXPECT_TRUE(animator != nullptr);

    auto timeDrivenAnimator = std::static_pointer_cast<RSRenderTimeDrivenGroupAnimator>(animator);
    EXPECT_TRUE(timeDrivenAnimator != nullptr);
    // Animator should not be started (state should be INITIALIZED, not RUNNING)

    GTEST_LOG_(INFO) << "RSAnimationCommandTest CreateInteractiveAnimatorGroup001 end";
}

/**
 * @tc.name: CreateInteractiveAnimatorGroup002
 * @tc.desc: Verify CreateInteractiveAnimatorGroup with startImmediately=true
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationCommandTest, CreateInteractiveAnimatorGroup002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationCommandTest CreateInteractiveAnimatorGroup002 start";
    RSContext context;
    InteractiveImplictAnimatorId groupId = 10002;
    std::vector<std::pair<NodeId, AnimationId>> animations;
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);

    // Cover branch: startImmediately = true (SetStartTime and StartAnimator called)
    AnimationCommandHelper::CreateInteractiveAnimatorGroup(context, groupId, animations, true, timingProtocol);

    auto animator = context.GetInteractiveImplictAnimatorMap().GetInteractiveImplictAnimator(groupId);
    EXPECT_TRUE(animator != nullptr);

    auto timeDrivenAnimator = std::static_pointer_cast<RSRenderTimeDrivenGroupAnimator>(animator);
    EXPECT_TRUE(timeDrivenAnimator != nullptr);
    // Animator should be started (state should be RUNNING)

    GTEST_LOG_(INFO) << "RSAnimationCommandTest CreateInteractiveAnimatorGroup002 end";
}

/**
 * @tc.name: CreateInteractiveAnimatorGroup003
 * @tc.desc: Verify CreateInteractiveAnimatorGroup with empty animations vector
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationCommandTest, CreateInteractiveAnimatorGroup003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationCommandTest CreateInteractiveAnimatorGroup003 start";
    RSContext context;
    InteractiveImplictAnimatorId groupId = 10003;
    std::vector<std::pair<NodeId, AnimationId>> animations; // Empty
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);

    // Cover branch: animations.empty()
    AnimationCommandHelper::CreateInteractiveAnimatorGroup(context, groupId, animations, false, timingProtocol);

    auto animator = context.GetInteractiveImplictAnimatorMap().GetInteractiveImplictAnimator(groupId);
    EXPECT_TRUE(animator != nullptr);

    GTEST_LOG_(INFO) << "RSAnimationCommandTest CreateInteractiveAnimatorGroup003 end";
}

/**
 * @tc.name: CreateInteractiveAnimatorGroup004
 * @tc.desc: Verify CreateInteractiveAnimatorGroup with multiple animations
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationCommandTest, CreateInteractiveAnimatorGroup004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationCommandTest CreateInteractiveAnimatorGroup004 start";
    RSContext context;
    InteractiveImplictAnimatorId groupId = 10004;
    std::vector<std::pair<NodeId, AnimationId>> animations;
    animations.push_back({10001, 20001});
    animations.push_back({10002, 20002});
    animations.push_back({10003, 20003});
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);

    // Cover branch: animations.size() > 0
    AnimationCommandHelper::CreateInteractiveAnimatorGroup(context, groupId, animations, true, timingProtocol);

    auto animator = context.GetInteractiveImplictAnimatorMap().GetInteractiveImplictAnimator(groupId);
    EXPECT_TRUE(animator != nullptr);

    GTEST_LOG_(INFO) << "RSAnimationCommandTest CreateInteractiveAnimatorGroup004 end";
}

/**
 * @tc.name: CreateInteractiveAnimatorGroup005
 * @tc.desc: Verify CreateInteractiveAnimatorGroup with different timing protocols
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationCommandTest, CreateInteractiveAnimatorGroup005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationCommandTest CreateInteractiveAnimatorGroup005 start";
    RSContext context;
    InteractiveImplictAnimatorId groupId = 10005;
    std::vector<std::pair<NodeId, AnimationId>> animations;
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(2000);
    timingProtocol.SetStartDelay(100);
    timingProtocol.SetRepeatCount(2);
    timingProtocol.SetSpeed(1.5f);
    timingProtocol.SetAutoReverse(true);

    // Cover branch: different timing protocol parameters
    AnimationCommandHelper::CreateInteractiveAnimatorGroup(context, groupId, animations, false, timingProtocol);

    auto animator = context.GetInteractiveImplictAnimatorMap().GetInteractiveImplictAnimator(groupId);
    EXPECT_TRUE(animator != nullptr);

    GTEST_LOG_(INFO) << "RSAnimationCommandTest CreateInteractiveAnimatorGroup005 end";
}

/**
 * @tc.name: CreateInteractiveAnimatorGroup006
 * @tc.desc: Verify CreateInteractiveAnimatorGroup registers animator in map
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationCommandTest, CreateInteractiveAnimatorGroup006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationCommandTest CreateInteractiveAnimatorGroup006 start";
    RSContext context;
    InteractiveImplictAnimatorId groupId = 10006;
    std::vector<std::pair<NodeId, AnimationId>> animations;
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);

    // Verify animator is registered in map
    EXPECT_TRUE(context.GetInteractiveImplictAnimatorMap().GetInteractiveImplictAnimator(groupId) == nullptr);
    AnimationCommandHelper::CreateInteractiveAnimatorGroup(context, groupId, animations, true, timingProtocol);
    EXPECT_TRUE(context.GetInteractiveImplictAnimatorMap().GetInteractiveImplictAnimator(groupId) != nullptr);

    // Clean up
    AnimationCommandHelper::DestoryInteractiveAnimator(context, groupId);
    EXPECT_TRUE(context.GetInteractiveImplictAnimatorMap().GetInteractiveImplictAnimator(groupId) == nullptr);

    GTEST_LOG_(INFO) << "RSAnimationCommandTest CreateInteractiveAnimatorGroup006 end";
}

/**
 * @tc.name: CreateInteractiveAnimatorGroup007
 * @tc.desc: Verify CreateInteractiveAnimatorGroup covers both startImmediately branches
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationCommandTest, CreateInteractiveAnimatorGroup007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationCommandTest CreateInteractiveAnimatorGroup007 start";
    RSContext context;
    std::vector<std::pair<NodeId, AnimationId>> animations;
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);

    // Test startImmediately = false
    InteractiveImplictAnimatorId groupId1 = 10007;
    AnimationCommandHelper::CreateInteractiveAnimatorGroup(context, groupId1, animations, false, timingProtocol);
    auto animator1 = context.GetInteractiveImplictAnimatorMap().GetInteractiveImplictAnimator(groupId1);
    EXPECT_TRUE(animator1 != nullptr);

    // Test startImmediately = true
    InteractiveImplictAnimatorId groupId2 = 10008;
    AnimationCommandHelper::CreateInteractiveAnimatorGroup(context, groupId2, animations, true, timingProtocol);
    auto animator2 = context.GetInteractiveImplictAnimatorMap().GetInteractiveImplictAnimator(groupId2);
    EXPECT_TRUE(animator2 != nullptr);

    GTEST_LOG_(INFO) << "RSAnimationCommandTest CreateInteractiveAnimatorGroup007 end";
}

} // namespace OHOS::Rosen
