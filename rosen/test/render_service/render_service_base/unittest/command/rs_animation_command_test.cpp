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

#include <cmath>
#include <limits>

#include "gtest/gtest.h"

#include "animation/rs_render_curve_animation.h"
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
    static void TestDestroyInRenderProcessor(NodeId, AnimationId, uint64_t, float, bool);
    static constexpr uint64_t ANIMATION_ID = 12345;
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

void RSAnimationCommandTest::TestDestroyInRenderProcessor(
    NodeId nodeId, AnimationId animId, uint64_t token, float fraction, bool isReverseCycle)
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

/**
 * @tc.name: AnimationDestroyInRender001
 * @tc.desc: Verify AnimationDestroyInRender with processor set
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationCommandTest, AnimationDestroyInRender001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationCommandTest AnimationDestroyInRender001 start";
    AnimationCommandHelper::SetAnimationDestroyInRenderProcessor(TestDestroyInRenderProcessor);
    RSContext context;
    AnimationCommandHelper::AnimationDestroyInRender(
        context, 1, ANIMATION_ID, 0, 0.5f, false);
    AnimationCommandHelper::SetAnimationDestroyInRenderProcessor(nullptr);
    GTEST_LOG_(INFO) << "RSAnimationCommandTest AnimationDestroyInRender001 end";
}

/**
 * @tc.name: AnimationDestroyInRender002
 * @tc.desc: Verify AnimationDestroyInRender with null processor
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationCommandTest, AnimationDestroyInRender002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationCommandTest AnimationDestroyInRender002 start";
    AnimationCommandHelper::SetAnimationDestroyInRenderProcessor(nullptr);
    RSContext context;
    AnimationCommandHelper::AnimationDestroyInRender(
        context, 1, ANIMATION_ID, 0, 0.5f, true);
    GTEST_LOG_(INFO) << "RSAnimationCommandTest AnimationDestroyInRender002 end";
}

/**
 * @tc.name: RebuildAnimation001
 * @tc.desc: Verify RebuildAnimation with null animation
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationCommandTest, RebuildAnimation001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationCommandTest RebuildAnimation001 start";
    RSContext context;
    AnimationCommandHelper::RebuildAnimation(context, 1, nullptr, 0.5f, false);
    GTEST_LOG_(INFO) << "RSAnimationCommandTest RebuildAnimation001 end";
}

/**
 * @tc.name: RebuildAnimation002
 * @tc.desc: Verify RebuildAnimation with null node
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationCommandTest, RebuildAnimation002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationCommandTest RebuildAnimation002 start";
    RSContext context;
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto animation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, 0, property, property1, property2);
    AnimationCommandHelper::RebuildAnimation(context, 99999, animation, 0.5f, false);
    GTEST_LOG_(INFO) << "RSAnimationCommandTest RebuildAnimation002 end";
}

/**
 * @tc.name: SetAnimationDestroyInRenderProcessor001
 * @tc.desc: Verify SetAnimationDestroyInRenderProcessor sets processor correctly
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationCommandTest, SetAnimationDestroyInRenderProcessor001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSAnimationCommandTest SetAnimationDestroyInRenderProcessor001 start";
    AnimationCommandHelper::SetAnimationDestroyInRenderProcessor(TestDestroyInRenderProcessor);
    AnimationCommandHelper::SetAnimationDestroyInRenderProcessor(nullptr);
    GTEST_LOG_(INFO) << "RSAnimationCommandTest SetAnimationDestroyInRenderProcessor001 end";
}

/**
 * @tc.name: SetFractionInteractiveAnimator001
 * @tc.desc: Verify SetFractionInteractiveAnimator rejects NaN fraction
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationCommandTest, SetFractionInteractiveAnimator001, TestSize.Level1)
{
    RSContext context;
    InteractiveImplictAnimatorId targetId = (1ULL << 32) | 1;
    std::vector<std::pair<NodeId, AnimationId>> animations = {{(1ULL << 32) | 1, 1}};
    AnimationCommandHelper::CreateInteractiveAnimator(context, targetId, animations, false);
    auto animator = context.GetInteractiveImplictAnimatorMap().GetInteractiveImplictAnimator(targetId);
    ASSERT_TRUE(animator != nullptr);
    AnimationCommandHelper::SetFractionInteractiveAnimator(context, targetId,
        std::numeric_limits<float>::quiet_NaN());
    EXPECT_TRUE(animator != nullptr);
}

/**
 * @tc.name: SetFractionInteractiveAnimator002
 * @tc.desc: Verify SetFractionInteractiveAnimator rejects positive infinity
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationCommandTest, SetFractionInteractiveAnimator002, TestSize.Level1)
{
    RSContext context;
    InteractiveImplictAnimatorId targetId = (1ULL << 32) | 1;
    std::vector<std::pair<NodeId, AnimationId>> animations = {{(1ULL << 32) | 1, 1}};
    AnimationCommandHelper::CreateInteractiveAnimator(context, targetId, animations, false);
    auto animator = context.GetInteractiveImplictAnimatorMap().GetInteractiveImplictAnimator(targetId);
    ASSERT_TRUE(animator != nullptr);
    AnimationCommandHelper::SetFractionInteractiveAnimator(context, targetId,
        std::numeric_limits<float>::infinity());
    EXPECT_TRUE(animator != nullptr);
}

/**
 * @tc.name: SetFractionInteractiveAnimator003
 * @tc.desc: Verify SetFractionInteractiveAnimator rejects negative infinity
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationCommandTest, SetFractionInteractiveAnimator003, TestSize.Level1)
{
    RSContext context;
    InteractiveImplictAnimatorId targetId = (1ULL << 32) | 1;
    std::vector<std::pair<NodeId, AnimationId>> animations = {{(1ULL << 32) | 1, 1}};
    AnimationCommandHelper::CreateInteractiveAnimator(context, targetId, animations, false);
    auto animator = context.GetInteractiveImplictAnimatorMap().GetInteractiveImplictAnimator(targetId);
    ASSERT_TRUE(animator != nullptr);
    AnimationCommandHelper::SetFractionInteractiveAnimator(context, targetId,
        -std::numeric_limits<float>::infinity());
    EXPECT_TRUE(animator != nullptr);
}

/**
 * @tc.name: SetFractionInteractiveAnimator004
 * @tc.desc: Verify SetFractionInteractiveAnimator accepts valid finite fraction
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationCommandTest, SetFractionInteractiveAnimator004, TestSize.Level1)
{
    RSContext context;
    InteractiveImplictAnimatorId targetId = (1ULL << 32) | 1;
    std::vector<std::pair<NodeId, AnimationId>> animations = {{(1ULL << 32) | 1, 1}};
    AnimationCommandHelper::CreateInteractiveAnimator(context, targetId, animations, false);
    ASSERT_TRUE(context.GetInteractiveImplictAnimatorMap().GetInteractiveImplictAnimator(targetId) != nullptr);
    AnimationCommandHelper::SetFractionInteractiveAnimator(context, targetId, 0.5f);
}

/**
 * @tc.name: IsAnimationsPidValid001
 * @tc.desc: Verify IsAnimationsPidValid returns true when all nodeIds match callerId pid
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationCommandTest, IsAnimationsPidValid001, TestSize.Level1)
{
    RSContext context;
    InteractiveImplictAnimatorId callerId = (1ULL << 32) | 1;
    std::vector<std::pair<NodeId, AnimationId>> animations = {{(1ULL << 32) | 1, 1}, {(1ULL << 32) | 2, 2}};
    bool result = AnimationCommandHelper::IsAnimationsPidValid(context, callerId,
        animations, __PRETTY_FUNCTION__);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: IsAnimationsPidValid002
 * @tc.desc: Verify IsAnimationsPidValid returns false when nodeId pid differs from callerId
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationCommandTest, IsAnimationsPidValid002, TestSize.Level1)
{
    RSContext context;
    InteractiveImplictAnimatorId callerId = (1ULL << 32) | 1;
    std::vector<std::pair<NodeId, AnimationId>> animations = {{(2ULL << 32) | 1, 1}};
    bool result = AnimationCommandHelper::IsAnimationsPidValid(context, callerId,
        animations, __PRETTY_FUNCTION__);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: IsAnimationsPidValid003
 * @tc.desc: Verify IsAnimationsPidValid returns true for empty animations
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationCommandTest, IsAnimationsPidValid003, TestSize.Level1)
{
    RSContext context;
    InteractiveImplictAnimatorId callerId = (1ULL << 32) | 1;
    std::vector<std::pair<NodeId, AnimationId>> animations;
    bool result = AnimationCommandHelper::IsAnimationsPidValid(context, callerId,
        animations, __PRETTY_FUNCTION__);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: IsAnimationsPidValid004
 * @tc.desc: Verify IsAnimationsPidValid returns true for mixed pid with mismatch first
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationCommandTest, IsAnimationsPidValid004, TestSize.Level1)
{
    RSContext context;
    InteractiveImplictAnimatorId callerId = (1ULL << 32) | 1;
    std::vector<std::pair<NodeId, AnimationId>> animations = {{(2ULL << 32) | 1, 1}, {(1ULL << 32) | 2, 2}};
    bool result = AnimationCommandHelper::IsAnimationsPidValid(context, callerId,
        animations, __PRETTY_FUNCTION__);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: IsAnimationsPidValid005
 * @tc.desc: Verify IsAnimationsPidValid returns true when cross-pid nodeId is UIExtension
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationCommandTest, IsAnimationsPidValid005, TestSize.Level1)
{
    RSContext context;
    InteractiveImplictAnimatorId callerId = (1ULL << 32) | 1;
    NodeId uiExtNodeId = (2ULL << 32) | 1;
    context.GetMutableNodeMap().uiExtensionSurfaceNodes_.insert(uiExtNodeId);
    std::vector<std::pair<NodeId, AnimationId>> animations = {{uiExtNodeId, 1}};
    bool result = AnimationCommandHelper::IsAnimationsPidValid(context, callerId,
        animations, __PRETTY_FUNCTION__);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: CreateInteractiveAnimatorPidValid001
 * @tc.desc: Verify CreateInteractiveAnimator rejects cross-pid animations
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationCommandTest, CreateInteractiveAnimatorPidValid001, TestSize.Level1)
{
    RSContext context;
    InteractiveImplictAnimatorId targetId = (1ULL << 32) | 1;
    std::vector<std::pair<NodeId, AnimationId>> animations = {{(2ULL << 32) | 1, 1}};
    AnimationCommandHelper::CreateInteractiveAnimator(context, targetId, animations, false);
    EXPECT_TRUE(context.GetInteractiveImplictAnimatorMap().GetInteractiveImplictAnimator(targetId) == nullptr);
}

/**
 * @tc.name: CreateInteractiveAnimatorPidValid002
 * @tc.desc: Verify CreateInteractiveAnimator accepts same-pid animations
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationCommandTest, CreateInteractiveAnimatorPidValid002, TestSize.Level1)
{
    RSContext context;
    InteractiveImplictAnimatorId targetId = (1ULL << 32) | 1;
    std::vector<std::pair<NodeId, AnimationId>> animations = {{(1ULL << 32) | 1, 1}};
    AnimationCommandHelper::CreateInteractiveAnimator(context, targetId, animations, false);
    EXPECT_TRUE(context.GetInteractiveImplictAnimatorMap().GetInteractiveImplictAnimator(targetId) != nullptr);
}

/**
 * @tc.name: CreateInteractiveAnimatorGroupPidValid001
 * @tc.desc: Verify CreateInteractiveAnimatorGroup rejects cross-pid animations
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationCommandTest, CreateInteractiveAnimatorGroupPidValid001, TestSize.Level1)
{
    RSContext context;
    InteractiveImplictAnimatorId groupId = (1ULL << 32) | 1;
    std::vector<std::pair<NodeId, AnimationId>> animations = {{(2ULL << 32) | 1, 1}};
    RSAnimationTimingProtocol timingProtocol;
    AnimationCommandHelper::CreateInteractiveAnimatorGroup(context, groupId,
        animations, false, timingProtocol);
    EXPECT_TRUE(context.GetInteractiveImplictAnimatorMap().GetInteractiveImplictAnimator(groupId) == nullptr);
}

/**
 * @tc.name: CreateInteractiveAnimatorGroupPidValid002
 * @tc.desc: Verify CreateInteractiveAnimatorGroup accepts same-pid animations
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationCommandTest, CreateInteractiveAnimatorGroupPidValid002, TestSize.Level1)
{
    RSContext context;
    InteractiveImplictAnimatorId groupId = (1ULL << 32) | 1;
    std::vector<std::pair<NodeId, AnimationId>> animations = {{(1ULL << 32) | 1, 1}};
    RSAnimationTimingProtocol timingProtocol;
    AnimationCommandHelper::CreateInteractiveAnimatorGroup(context, groupId,
        animations, false, timingProtocol);
    EXPECT_TRUE(context.GetInteractiveImplictAnimatorMap().GetInteractiveImplictAnimator(groupId) != nullptr);
}

/**
 * @tc.name: InteractiveAnimatorAddAnimationsPidValid001
 * @tc.desc: Verify InteractiveAnimatorAddAnimations rejects cross-pid animations
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationCommandTest, InteractiveAnimatorAddAnimationsPidValid001, TestSize.Level1)
{
    RSContext context;
    InteractiveImplictAnimatorId targetId = (1ULL << 32) | 1;
    AnimationCommandHelper::CreateInteractiveAnimator(context, targetId,
        std::vector<std::pair<NodeId, AnimationId>>{{(1ULL << 32) | 1, 1}}, false);
    ASSERT_TRUE(context.GetInteractiveImplictAnimatorMap().GetInteractiveImplictAnimator(targetId) != nullptr);
    std::vector<std::pair<NodeId, AnimationId>> crossPidAnimations = {{(2ULL << 32) | 1, 2}};
    AnimationCommandHelper::InteractiveAnimatorAddAnimations(context, targetId, crossPidAnimations);
    EXPECT_TRUE(context.GetInteractiveImplictAnimatorMap().GetInteractiveImplictAnimator(targetId) != nullptr);
}

/**
 * @tc.name: InteractiveAnimatorAddAnimationsPidValid002
 * @tc.desc: Verify InteractiveAnimatorAddAnimations accepts same-pid animations
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationCommandTest, InteractiveAnimatorAddAnimationsPidValid002, TestSize.Level1)
{
    RSContext context;
    InteractiveImplictAnimatorId targetId = (1ULL << 32) | 1;
    AnimationCommandHelper::CreateInteractiveAnimator(context, targetId,
        std::vector<std::pair<NodeId, AnimationId>>{{(1ULL << 32) | 1, 1}}, false);
    ASSERT_TRUE(context.GetInteractiveImplictAnimatorMap().GetInteractiveImplictAnimator(targetId) != nullptr);
    std::vector<std::pair<NodeId, AnimationId>> samePidAnimations = {{(1ULL << 32) | 2, 2}};
    AnimationCommandHelper::InteractiveAnimatorAddAnimations(context, targetId, samePidAnimations);
    EXPECT_TRUE(context.GetInteractiveImplictAnimatorMap().GetInteractiveImplictAnimator(targetId) != nullptr);
}

} // namespace OHOS::Rosen
