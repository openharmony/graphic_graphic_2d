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
#include "animation/rs_render_interactive_implict_animator.h"
#include "animation/rs_render_interactive_implict_animator_map.h"
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

    RSRenderNode node(1, context);

    // Create and add an animation
    auto animation = std::make_shared<RSRenderAnimationMock>();
    node.GetAnimationManager().AddAnimation(animation);

    // Get the fallback node by accessing private member directly (UT can access private members)
    auto& nodeMap = context->GetMutableNodeMap();
    auto& fallbackNode = nodeMap.renderNodeMap_[0][0];

    // Cover branch: animation is not group child -> skip if block
    size_t beforeCount = fallbackNode->GetAnimationManager().GetAnimations().size();
    node.FallbackAnimationsToRoot();
    size_t afterCount = fallbackNode->GetAnimationManager().GetAnimations().size();

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

    RSRenderNode node(1, context);

    // Create a group animator
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);
    auto groupAnimator = std::make_shared<RSRenderTimeDrivenGroupAnimator>(
        10001, context, timingProtocol);

    // Create animation and set as group child
    auto animation = std::make_shared<RSRenderAnimationMock>();
    animation->SetGroupAnimator(groupAnimator);

    // Add animation to node
    node.GetAnimationManager().AddAnimation(animation);

    // Get the fallback node by accessing private member directly
    auto& nodeMap = context->GetMutableNodeMap();
    auto& fallbackNode = nodeMap.renderNodeMap_[0][0];

    // Cover branch: IsGroupAnimationChild() == true -> call Attach and RemoveFromGroupAnimator
    size_t beforeCount = fallbackNode->GetAnimationManager().GetAnimations().size();
    node.FallbackAnimationsToRoot();
    size_t afterCount = fallbackNode->GetAnimationManager().GetAnimations().size();

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

    RSRenderNode node(1, context);

    // Create, add, and pause an animation
    auto animation = std::make_shared<RSRenderAnimationMock>();
    animation->Start();
    animation->Pause();
    node.GetAnimationManager().AddAnimation(animation);

    // Get the fallback node by accessing private member directly
    auto& nodeMap = context->GetMutableNodeMap();
    auto& fallbackNode = nodeMap.renderNodeMap_[0][0];

    // Cover branch: IsPaused() == true -> call Resume
    size_t beforeCount = fallbackNode->GetAnimationManager().GetAnimations().size();
    node.FallbackAnimationsToRoot();
    size_t afterCount = fallbackNode->GetAnimationManager().GetAnimations().size();

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

    RSRenderNode node(1, context);

    // Create a group animator
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);
    auto groupAnimator = std::make_shared<RSRenderTimeDrivenGroupAnimator>(
        10001, context, timingProtocol);

    // Add multiple animations: normal, group child, paused
    auto animation1 = std::make_shared<RSRenderAnimationMock>(1001);
    node.GetAnimationManager().AddAnimation(animation1);

    auto animation2 = std::make_shared<RSRenderAnimationMock>(1002);
    animation2->SetGroupAnimator(groupAnimator);
    node.GetAnimationManager().AddAnimation(animation2);

    auto animation3 = std::make_shared<RSRenderAnimationMock>(1003);
    animation3->Start();
    animation3->Pause();
    node.GetAnimationManager().AddAnimation(animation3);

    // Get the fallback node by accessing private member directly
    auto& nodeMap = context->GetMutableNodeMap();
    auto& fallbackNode = nodeMap.renderNodeMap_[0][0];

    // Cover all branches in one test
    size_t beforeCount = fallbackNode->GetAnimationManager().GetAnimations().size();
    node.FallbackAnimationsToRoot();
    size_t afterCount = fallbackNode->GetAnimationManager().GetAnimations().size();

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
    node.GetAnimationManager().AddAnimation(animation);

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
    EXPECT_FALSE(fallbackNode->GetAnimationManager().GetAnimations().empty());

    GTEST_LOG_(INFO) << "RSRenderAnimationOthersTest FallbackAnimationsToRoot008 end";
}

} // namespace Rosen
} // namespace OHOS
