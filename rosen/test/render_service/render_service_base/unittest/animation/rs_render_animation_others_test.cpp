/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

#include "animation/rs_animation_timing_protocol.h"
#include "animation/rs_render_curve_animation.h"
#include "animation/rs_render_interactive_implict_animator.h"
#include "animation/rs_render_interactive_implict_animator_map.h"
#include "modifier/rs_render_property.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_root_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

// Mock class for RSRenderAnimation to test protected methods
class RSRenderAnimationMock : public RSRenderAnimation {
public:
    static constexpr uint64_t ANIMATION_ID = 12345;
    RSRenderAnimationMock() : RSRenderAnimationMock(ANIMATION_ID) {}
    explicit RSRenderAnimationMock(AnimationId id) : RSRenderAnimation(id) {}
    ~RSRenderAnimationMock() override = default;

    void Pause()
    {
        RSRenderAnimation::Pause();
    }
    void Resume()
    {
        RSRenderAnimation::Resume();
    }
    void Start()
    {
        RSRenderAnimation::Start();
    }
    bool IsPaused() const
    {
        return RSRenderAnimation::IsPaused();
    }
    bool IsRunning() const
    {
        return RSRenderAnimation::IsRunning();
    }
    bool IsGroupAnimationChild() const
    {
        return RSRenderAnimation::IsGroupAnimationChild();
    }
    void SetGroupAnimator(const std::shared_ptr<RSRenderTimeDrivenGroupAnimator>& groupAnimator)
    {
        RSRenderAnimation::SetGroupAnimator(groupAnimator);
    }
    void RemoveFromGroupAnimator()
    {
        RSRenderAnimation::RemoveFromGroupAnimator();
    }
};

class RSRenderAnimationOthersTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderAnimationOthersTest::SetUpTestCase() {}
void RSRenderAnimationOthersTest::TearDownTestCase() {}
void RSRenderAnimationOthersTest::SetUp() {}
void RSRenderAnimationOthersTest::TearDown() {}

/**
 * @tc.name: FallbackAnimationsToRoot001
 * @tc.desc: Test FallbackAnimationsToRoot with empty animations
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderAnimationOthersTest, FallbackAnimationsToRoot001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderAnimationOthersTest FallbackAnimationsToRoot001 start";
    auto context = std::make_shared<RSContext>();
    auto rootNode = std::make_shared<RSRootRenderNode>(0);
    context->GetMutableNodeMap().RegisterRenderNode(rootNode);

    RSRenderNode node(1, context);

    // Cover branch: animations_.empty() -> return early
    node.FallbackAnimationsToRoot();

    // Verify no crash
    GTEST_LOG_(INFO) << "RSRenderAnimationOthersTest FallbackAnimationsToRoot001 end";
}

/**
 * @tc.name: FallbackAnimationsToRoot002
 * @tc.desc: Test FallbackAnimationsToRoot with null context
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderAnimationOthersTest, FallbackAnimationsToRoot002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderAnimationOthersTest FallbackAnimationsToRoot002 start";
    RSRenderNode node(1); // No context

    // Cover branch: context == nullptr -> return early
    node.FallbackAnimationsToRoot();

    // Verify no crash
    GTEST_LOG_(INFO) << "RSRenderAnimationOthersTest FallbackAnimationsToRoot002 end";
}

/**
 * @tc.name: FallbackAnimationsToRoot003
 * @tc.desc: Test FallbackAnimationsToRoot with null fallback target
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderAnimationOthersTest, FallbackAnimationsToRoot003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderAnimationOthersTest FallbackAnimationsToRoot003 start";
    auto context = std::make_shared<RSContext>();
    // Don't register root node, so GetAnimationFallbackNode returns null

    RSRenderNode node(1, context);

    // Cover branch: target == nullptr -> return early
    node.FallbackAnimationsToRoot();

    // Verify no crash
    GTEST_LOG_(INFO) << "RSRenderAnimationOthersTest FallbackAnimationsToRoot003 end";
}

/**
 * @tc.name: FallbackAnimationsToRoot004
 * @tc.desc: Test FallbackAnimationsToRoot with normal animation
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderAnimationOthersTest, FallbackAnimationsToRoot004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderAnimationOthersTest FallbackAnimationsToRoot004 start";
    auto context = std::make_shared<RSContext>();

    // Get the existing fallback node from nodeMap (created by RSRenderNodeMap constructor)
    auto& nodeMap = context->GetMutableNodeMap();
    auto fallbackNode = nodeMap.GetAnimationFallbackNode();
    ASSERT_NE(fallbackNode, nullptr);

    // Add an initial animation to fallbackNode to initialize animationManager_
    auto initAnimation = std::make_shared<RSRenderAnimationMock>(0);
    fallbackNode->AddAnimation(initAnimation);

    RSRenderNode node(1, context);

    // Create and add an animation
    auto animation = std::make_shared<RSRenderAnimationMock>();
    node.AddAnimation(animation);

    // Cover branch: animation is not group child -> skip if block
    auto animationManager = fallbackNode->GetAnimationManager();
    ASSERT_NE(animationManager, nullptr);
    size_t beforeCount = animationManager->GetAnimations().size();
    node.FallbackAnimationsToRoot();
    size_t afterCount = animationManager->GetAnimations().size();

    // Animation should be moved to fallback node
    EXPECT_EQ(afterCount, beforeCount + 1);

    GTEST_LOG_(INFO) << "RSRenderAnimationOthersTest FallbackAnimationsToRoot004 end";
}

/**
 * @tc.name: FallbackAnimationsToRoot005
 * @tc.desc: Test FallbackAnimationsToRoot with group animation child
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderAnimationOthersTest, FallbackAnimationsToRoot005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderAnimationOthersTest FallbackAnimationsToRoot005 start";
    auto context = std::make_shared<RSContext>();

    // Get the existing fallback node from nodeMap (created by RSRenderNodeMap constructor)
    auto& nodeMap = context->GetMutableNodeMap();
    auto fallbackNode = nodeMap.GetAnimationFallbackNode();
    ASSERT_NE(fallbackNode, nullptr);

    // Add an initial animation to fallbackNode to initialize animationManager_
    auto initAnimation = std::make_shared<RSRenderAnimationMock>(0);
    fallbackNode->AddAnimation(initAnimation);

    RSRenderNode node(1, context);

    // Create a group animator
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);
    auto groupAnimator = std::make_shared<RSRenderTimeDrivenGroupAnimator>(10001, context, timingProtocol);

    // Create animation and set as group child
    auto animation = std::make_shared<RSRenderAnimationMock>();
    animation->SetGroupAnimator(groupAnimator);

    // Add animation to node
    node.AddAnimation(animation);

    // Cover branch: IsGroupAnimationChild() == true -> call Attach and RemoveFromGroupAnimator
    auto animationManager = fallbackNode->GetAnimationManager();
    ASSERT_NE(animationManager, nullptr);
    size_t beforeCount = animationManager->GetAnimations().size();
    node.FallbackAnimationsToRoot();
    size_t afterCount = animationManager->GetAnimations().size();

    // Animation should be moved to fallback node
    EXPECT_EQ(afterCount, beforeCount + 1);

    GTEST_LOG_(INFO) << "RSRenderAnimationOthersTest FallbackAnimationsToRoot005 end";
}

/**
 * @tc.name: FallbackAnimationsToRoot006
 * @tc.desc: Test FallbackAnimationsToRoot with paused animation
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderAnimationOthersTest, FallbackAnimationsToRoot006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderAnimationOthersTest FallbackAnimationsToRoot006 start";
    auto context = std::make_shared<RSContext>();

    // Get the existing fallback node from nodeMap (created by RSRenderNodeMap constructor)
    auto& nodeMap = context->GetMutableNodeMap();
    auto fallbackNode = nodeMap.GetAnimationFallbackNode();
    ASSERT_NE(fallbackNode, nullptr);

    // Add an initial animation to fallbackNode to initialize animationManager_
    auto initAnimation = std::make_shared<RSRenderAnimationMock>(0);
    fallbackNode->AddAnimation(initAnimation);

    RSRenderNode node(1, context);

    // Create, add, and pause an animation
    auto animation = std::make_shared<RSRenderAnimationMock>();
    animation->Start();
    animation->Pause();
    node.AddAnimation(animation);

    // Cover branch: IsPaused() == true -> call Resume
    auto animationManager = fallbackNode->GetAnimationManager();
    ASSERT_NE(animationManager, nullptr);
    size_t beforeCount = animationManager->GetAnimations().size();
    node.FallbackAnimationsToRoot();
    size_t afterCount = animationManager->GetAnimations().size();

    // Animation should be moved to fallback node and resumed
    EXPECT_EQ(afterCount, beforeCount + 1);

    GTEST_LOG_(INFO) << "RSRenderAnimationOthersTest FallbackAnimationsToRoot006 end";
}

/**
 * @tc.name: FallbackAnimationsToRoot007
 * @tc.desc: Test FallbackAnimationsToRoot with mixed animations
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderAnimationOthersTest, FallbackAnimationsToRoot007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderAnimationOthersTest FallbackAnimationsToRoot007 start";
    auto context = std::make_shared<RSContext>();

    // Get the existing fallback node from nodeMap (created by RSRenderNodeMap constructor)
    auto& nodeMap = context->GetMutableNodeMap();
    auto fallbackNode = nodeMap.GetAnimationFallbackNode();
    ASSERT_NE(fallbackNode, nullptr);

    // Add an initial animation to fallbackNode to initialize animationManager_
    auto initAnimation = std::make_shared<RSRenderAnimationMock>(0);
    fallbackNode->AddAnimation(initAnimation);

    RSRenderNode node(1, context);

    // Create a group animator
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);
    auto groupAnimator = std::make_shared<RSRenderTimeDrivenGroupAnimator>(10001, context, timingProtocol);

    // Add multiple animations: normal, group child, paused
    auto animation1 = std::make_shared<RSRenderAnimationMock>(1001);
    node.AddAnimation(animation1);

    auto animation2 = std::make_shared<RSRenderAnimationMock>(1002);
    animation2->SetGroupAnimator(groupAnimator);
    node.AddAnimation(animation2);

    auto animation3 = std::make_shared<RSRenderAnimationMock>(1003);
    animation3->Start();
    animation3->Pause();
    node.AddAnimation(animation3);

    // Cover all branches in one test
    auto animationManager = fallbackNode->GetAnimationManager();
    ASSERT_NE(animationManager, nullptr);
    size_t beforeCount = animationManager->GetAnimations().size();
    node.FallbackAnimationsToRoot();
    size_t afterCount = animationManager->GetAnimations().size();

    // All animations should be moved to fallback node
    EXPECT_EQ(afterCount, beforeCount + 3);

    GTEST_LOG_(INFO) << "RSRenderAnimationOthersTest FallbackAnimationsToRoot007 end";
}

/**
 * @tc.name: FallbackAnimationsToRoot008
 * @tc.desc: Test FallbackAnimationsToRoot calls Attach and RemoveFromGroupAnimator for group child
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderAnimationOthersTest, FallbackAnimationsToRoot008, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderAnimationOthersTest FallbackAnimationsToRoot008 start";
    auto context = std::make_shared<RSContext>();

    RSRenderNode node(1, context);

    // Create a group animator and register it
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);
    auto groupAnimator = std::make_shared<RSRenderTimeDrivenGroupAnimator>(
        10001, context, timingProtocol);
    context->GetInteractiveImplictAnimatorMap().RegisterInteractiveImplictAnimator(groupAnimator);

    // Create animation and set as group child
    auto animation = std::make_shared<RSRenderAnimationMock>();
    animation->SetGroupAnimator(groupAnimator);
    node.AddAnimation(animation);

    // Verify group animator exists before fallback
    auto groupBefore = context->GetInteractiveImplictAnimatorMap().GetInteractiveImplictAnimator(10001);
    EXPECT_TRUE(groupBefore != nullptr);

    // Get the fallback node by accessing private member directly
    auto& nodeMap = context->GetMutableNodeMap();
    auto& fallbackNode = nodeMap.renderNodeMap_[0][0];

    // Cover branch: Attach(target.get()) and RemoveFromGroupAnimator() are called
    node.FallbackAnimationsToRoot();

    // After RemoveFromGroupAnimator, animation should no longer be associated with group
    // Verify animation was moved to fallback node
    ASSERT_NE(fallbackNode->GetAnimationManager(), nullptr);
    EXPECT_FALSE(fallbackNode->GetAnimationManager()->GetAnimations().empty());

    GTEST_LOG_(INFO) << "RSRenderAnimationOthersTest FallbackAnimationsToRoot008 end";
}

/**
 * @tc.name: FallbackAnimationsToRoot_FinishInfiniteAnimation
 * @tc.desc: Test FallbackAnimationsToRoot with infinite repeat animation (repeatCount = -1).
 *           This is a new branch added in commit 974b560f: finish infinite animations instead of moving to root.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderAnimationOthersTest, FallbackAnimationsToRoot_FinishInfiniteAnimation, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderAnimationOthersTest FallbackAnimationsToRoot_FinishInfiniteAnimation start";

    auto context = std::make_shared<RSContext>();
    auto rootNode = std::make_shared<RSRootRenderNode>(0);
    context->GetMutableNodeMap().RegisterRenderNode(rootNode);

    RSRenderNode node(1, context);

    auto animation = std::make_shared<RSRenderAnimationMock>();
    animation->SetRepeatCount(-1);
    animation->Start();
    node.AddAnimation(animation);

    EXPECT_TRUE(animation->IsRunning());
    EXPECT_EQ(animation->GetRepeatCount(), -1);

    node.FallbackAnimationsToRoot();

    EXPECT_FALSE(animation->IsRunning());
    EXPECT_TRUE(animation->IsFinished());

    auto fallbackNode = context->GetNodeMap().GetAnimationFallbackNode();
    ASSERT_NE(fallbackNode, nullptr);
    auto animationManager = fallbackNode->GetAnimationManager();
    EXPECT_EQ(animationManager, nullptr);

    GTEST_LOG_(INFO) << "RSRenderAnimationOthersTest FallbackAnimationsToRoot_FinishInfiniteAnimation end";
}

/**
 * @tc.name: FallbackAnimationsToRoot_MoveFiniteAnimation
 * @tc.desc: Test FallbackAnimationsToRoot with finite repeat animation.
 *           Test that finite animations are moved to root (existing behavior, for contrast).
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderAnimationOthersTest, FallbackAnimationsToRoot_MoveFiniteAnimation, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderAnimationOthersTest FallbackAnimationsToRoot_MoveFiniteAnimation start";

    auto context = std::make_shared<RSContext>();
    auto rootNode = std::make_shared<RSRootRenderNode>(0);
    context->GetMutableNodeMap().RegisterRenderNode(rootNode);

    RSRenderNode node(1, context);

    auto animation = std::make_shared<RSRenderAnimationMock>();
    animation->SetRepeatCount(1);
    animation->Start();
    node.AddAnimation(animation);

    EXPECT_TRUE(animation->IsRunning());
    EXPECT_EQ(animation->GetRepeatCount(), 1);

    node.FallbackAnimationsToRoot();

    auto fallbackNode = context->GetNodeMap().GetAnimationFallbackNode();
    ASSERT_NE(fallbackNode, nullptr);
    auto animationManager = fallbackNode->GetAnimationManager();
    ASSERT_NE(animationManager, nullptr);
    EXPECT_FALSE(animationManager->GetAnimations().empty());

    GTEST_LOG_(INFO) << "RSRenderAnimationOthersTest FallbackAnimationsToRoot_MoveFiniteAnimation end";
}

/**
 * @tc.name: GetOrCreateAnimationManager001
 * @tc.desc: Verify GetOrCreateAnimationManager creates AnimationManager when none exists
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderAnimationOthersTest, GetOrCreateAnimationManager001, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(1);
    ASSERT_EQ(node->GetAnimationManager(), nullptr);
    auto manager = node->GetOrCreateAnimationManager();
    ASSERT_NE(manager, nullptr);
    EXPECT_EQ(node->GetAnimationManager(), manager);
}

/**
 * @tc.name: GetOrCreateAnimationManager002
 * @tc.desc: Verify GetOrCreateAnimationManager returns existing AnimationManager
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderAnimationOthersTest, GetOrCreateAnimationManager002, TestSize.Level1)
{
    auto context = std::make_shared<RSContext>();
    auto node = std::make_shared<RSRenderNode>(1, context);
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto animation = std::make_shared<RSRenderCurveAnimation>(1, 1, property, property1, property2);
    node->AddAnimation(animation);
    auto existingManager = node->GetAnimationManager();
    ASSERT_NE(existingManager, nullptr);
    auto manager = node->GetOrCreateAnimationManager();
    EXPECT_EQ(manager, existingManager);
}

/**
 * @tc.name: RSRenderNode_Animate_ResetManagerBothEmpty_001
 * @tc.desc: Verify AnimationManager is reset when both animations_ and
 *           pendingCancelAnimation_ are empty after Animate
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderAnimationOthersTest,
    RSRenderNode_Animate_ResetManagerBothEmpty_001, TestSize.Level1)
{
    auto context = std::make_shared<RSContext>();
    auto node = std::make_shared<RSRenderNode>(1, context);
    auto manager = node->GetOrCreateAnimationManager();
    ASSERT_NE(manager, nullptr);
    EXPECT_TRUE(manager->animations_.empty());
    EXPECT_TRUE(manager->pendingCancelAnimation_.empty());

    node->SetIsOnTheTree(true);
    int64_t minLeftDelayTime = 0;
    int64_t nextFrameTime = 0;
    node->Animate(0, minLeftDelayTime, nextFrameTime);

    EXPECT_EQ(node->GetAnimationManager(), nullptr);
}

/**
 * @tc.name: RSRenderNode_Animate_PendingCancelPreventsReset_002
 * @tc.desc: Verify AnimationManager is NOT reset when pendingCancelAnimation_
 *           is non-empty after Animate, even if animations_ is empty
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderAnimationOthersTest,
    RSRenderNode_Animate_PendingCancelPreventsReset_002, TestSize.Level1)
{
    auto context = std::make_shared<RSContext>();
    auto node = std::make_shared<RSRenderNode>(1, context);
    auto manager = node->GetOrCreateAnimationManager();
    ASSERT_NE(manager, nullptr);

    manager->AttemptCancelAnimationByAnimationId({99999});
    EXPECT_TRUE(manager->animations_.empty());
    EXPECT_FALSE(manager->pendingCancelAnimation_.empty());

    node->SetIsOnTheTree(true);
    int64_t minLeftDelayTime = 0;
    int64_t nextFrameTime = 0;
    node->Animate(0, minLeftDelayTime, nextFrameTime);

    EXPECT_NE(node->GetAnimationManager(), nullptr);
}

/**
 * @tc.name: RSRenderNode_Animate_ActiveAnimationPreventsReset_003
 * @tc.desc: Verify AnimationManager is NOT reset when animations_ is
 *           non-empty after Animate (short-circuit on first condition)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderAnimationOthersTest,
    RSRenderNode_Animate_ActiveAnimationPreventsReset_003, TestSize.Level1)
{
    auto context = std::make_shared<RSContext>();
    auto node = std::make_shared<RSRenderNode>(1, context);
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto animation = std::make_shared<RSRenderCurveAnimation>(1, 1, property, property1, property2);
    node->AddAnimation(animation);

    auto manager = node->GetAnimationManager();
    ASSERT_NE(manager, nullptr);
    EXPECT_FALSE(manager->animations_.empty());

    node->SetIsOnTheTree(true);
    int64_t minLeftDelayTime = 0;
    int64_t nextFrameTime = 0;
    node->Animate(0, minLeftDelayTime, nextFrameTime);

    EXPECT_NE(node->GetAnimationManager(), nullptr);
}

} // namespace Rosen
} // namespace OHOS
