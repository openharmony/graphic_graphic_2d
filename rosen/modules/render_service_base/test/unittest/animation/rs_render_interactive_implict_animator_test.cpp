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

#include "animation/rs_render_curve_animation.h"
#include "animation/rs_render_interactive_implict_animator.h"
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_context.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSRenderInteractiveImplictAnimatorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static constexpr InteractiveImplictAnimatorId ANIMATOR_ID = 10001;
    static constexpr NodeId NODE_ID = 10002;
    static constexpr AnimationId ANIMATION_ID = 10003;
    static constexpr uint64_t PROPERTY_ID = 10004;
    static constexpr NodeId INVALID_NODE_ID = std::numeric_limits<uint64_t>::max();
    static constexpr AnimationId INVALID_ANIMATION_ID = std::numeric_limits<uint64_t>::max();
};

void RSRenderInteractiveImplictAnimatorTest::SetUpTestCase() {}
void RSRenderInteractiveImplictAnimatorTest::TearDownTestCase() {}
void RSRenderInteractiveImplictAnimatorTest::SetUp() {}
void RSRenderInteractiveImplictAnimatorTest::TearDown() {}

/**
 * @tc.name: AddAnimations001
 * @tc.desc: Verify the AddAnimations
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorTest, AddAnimations001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest AddAnimations001 start";
    auto context = std::make_shared<RSContext>();
    auto animator = std::make_shared<RSRenderInteractiveImplictAnimator>(ANIMATOR_ID, context);
    ASSERT_TRUE(animator != nullptr);
    std::vector<std::pair<NodeId, AnimationId>> animations1;
    animations1.emplace_back(0, 0);
    animator->AddAnimations(animations1);

    auto renderNode = std::make_shared<RSCanvasRenderNode>(NODE_ID);
    context->GetMutableNodeMap().RegisterRenderNode(renderNode);
    std::vector<std::pair<NodeId, AnimationId>> animations2;
    animations2.emplace_back(NODE_ID, 0);
    animator->AddAnimations(animations2);

    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    renderNode->GetAnimationManager().AddAnimation(renderCurveAnimation);
    std::vector<std::pair<NodeId, AnimationId>> animations3;
    animations3.emplace_back(NODE_ID, ANIMATION_ID);
    animator->AddAnimations(animations3);
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest AddAnimations001 end";
}

/**
 * @tc.name: AddAnimations002
 * @tc.desc: Verify the AddAnimations
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorTest, AddAnimations002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest AddAnimations002 start";
    auto context = std::make_shared<RSContext>();
    auto animator = std::make_shared<RSRenderInteractiveImplictAnimator>(ANIMATOR_ID, context);
    ASSERT_TRUE(animator != nullptr);
    context = nullptr;
    std::vector<std::pair<NodeId, AnimationId>> animations1;
    animations1.emplace_back(0, 0);
    animator->AddAnimations(animations1);
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest AddAnimations002 end";
}

/**
 * @tc.name: AddAnimations003
 * @tc.desc: Cover branch: 1.node is nullptr 2.animation is nullptr
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorTest, AddAnimations003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest AddAnimations003 start";
    auto context = std::make_shared<RSContext>();
    ASSERT_TRUE(context != nullptr);
    auto animator = std::make_shared<RSRenderInteractiveImplictAnimator>(ANIMATOR_ID, context);
    ASSERT_TRUE(animator != nullptr);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(NODE_ID);
    ASSERT_TRUE(renderNode != nullptr);
    context->GetMutableNodeMap().RegisterRenderNode(renderNode);

    std::vector<std::pair<NodeId, AnimationId>> animations;
    animations.emplace_back(INVALID_NODE_ID, INVALID_ANIMATION_ID);
    animations.emplace_back(NODE_ID, INVALID_ANIMATION_ID);
    animator->AddAnimations(animations);
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest AddAnimations003 end";
}

/**
 * @tc.name: PauseAnimator001
 * @tc.desc: Cover branch: animation valid in PauseAnimator
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorTest, PauseAnimator001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest PauseAnimator001 start";
    auto context = std::make_shared<RSContext>();
    auto animator = std::make_shared<RSRenderInteractiveImplictAnimator>(ANIMATOR_ID, context);
    ASSERT_TRUE(animator != nullptr);

    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);

    animator->cachedAnimations_.emplace_back(renderCurveAnimation);

    animator->PauseAnimator();
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest PauseAnimator001 end";
}

/**
 * @tc.name: PauseAnimator004
 * @tc.desc: Cover branch: animation is nullptr in cachedAnimations_ loop
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorTest, PauseAnimator004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest PauseAnimator004 start";
    auto context = std::make_shared<RSContext>();
    auto animator = std::make_shared<RSRenderInteractiveImplictAnimator>(ANIMATOR_ID, context);
    ASSERT_TRUE(animator != nullptr);

    animator->cachedAnimations_.emplace_back(std::weak_ptr<RSRenderAnimation>());

    animator->PauseAnimator();
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest PauseAnimator004 end";
}

/**
 * @tc.name: PauseAnimator002
 * @tc.desc: Verify the PauseAnimator
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorTest, PauseAnimator002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest PauseAnimator002 start";
    auto context = std::make_shared<RSContext>();
    auto animator = std::make_shared<RSRenderInteractiveImplictAnimator>(ANIMATOR_ID, context);
    ASSERT_TRUE(animator != nullptr);
    context = nullptr;
    animator->PauseAnimator();
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest PauseAnimator002 end";
}

/**
 * @tc.name: PauseAnimator003
 * @tc.desc: Cover branch: 1.node is nullptr 2.animation is nullptr
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorTest, PauseAnimator003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest PauseAnimator003 start";
    auto context = std::make_shared<RSContext>();
    ASSERT_TRUE(context != nullptr);
    auto animator = std::make_shared<RSRenderInteractiveImplictAnimator>(ANIMATOR_ID, context);
    ASSERT_TRUE(animator != nullptr);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(NODE_ID);
    ASSERT_TRUE(renderNode != nullptr);
    context->GetMutableNodeMap().RegisterRenderNode(renderNode);

    // Test with invalid IDs
    std::vector<std::pair<NodeId, AnimationId>> animations1;
    animations1.emplace_back(INVALID_NODE_ID, INVALID_ANIMATION_ID);
    animator->AddAnimations(animations1);
    animator->PauseAnimator();

    // Test with valid node but invalid animation
    std::vector<std::pair<NodeId, AnimationId>> animations2;
    animations2.emplace_back(NODE_ID, INVALID_ANIMATION_ID);
    animator->AddAnimations(animations2);
    animator->PauseAnimator();
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest PauseAnimator003 end";
}

/**
 * @tc.name: ContinueAnimator001
 * @tc.desc: Cover branch: node is nullptr in ContinueAnimator loop
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorTest, ContinueAnimator001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest ContinueAnimator001 start";
    auto context = std::make_shared<RSContext>();
    auto animator = std::make_shared<RSRenderInteractiveImplictAnimator>(ANIMATOR_ID, context);
    ASSERT_TRUE(animator != nullptr);

    auto renderNode = std::make_shared<RSCanvasRenderNode>(NODE_ID);
    context->GetMutableNodeMap().RegisterRenderNode(renderNode);

    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);

    renderCurveAnimation->targetId_ = INVALID_NODE_ID;

    animator->cachedAnimations_.emplace_back(renderCurveAnimation);

    animator->ContinueAnimator();
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest ContinueAnimator001 end";
}

/**
 * @tc.name: ContinueAnimator005
 * @tc.desc: Cover branch: animation is nullptr in cachedAnimations_ loop
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorTest, ContinueAnimator005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest ContinueAnimator005 start";
    auto context = std::make_shared<RSContext>();
    auto animator = std::make_shared<RSRenderInteractiveImplictAnimator>(ANIMATOR_ID, context);
    ASSERT_TRUE(animator != nullptr);

    animator->cachedAnimations_.emplace_back(std::weak_ptr<RSRenderAnimation>());

    animator->ContinueAnimator();
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest ContinueAnimator005 end";
}

/**
 * @tc.name: ContinueAnimator002
 * @tc.desc: Verify the ContinueAnimator
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorTest, ContinueAnimator002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest ContinueAnimator002 start";
    auto context = std::make_shared<RSContext>();
    auto animator = std::make_shared<RSRenderInteractiveImplictAnimator>(ANIMATOR_ID, context);
    ASSERT_TRUE(animator != nullptr);
    context = nullptr;
    animator->ContinueAnimator();
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest ContinueAnimator002 end";
}

/**
 * @tc.name: ContinueAnimator003
 * @tc.desc: Cover branch: 1.node is nullptr 2.animation is nullptr
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorTest, ContinueAnimator003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest ContinueAnimator003 start";
    auto context = std::make_shared<RSContext>();
    ASSERT_TRUE(context != nullptr);
    auto animator = std::make_shared<RSRenderInteractiveImplictAnimator>(ANIMATOR_ID, context);
    ASSERT_TRUE(animator != nullptr);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(NODE_ID);
    ASSERT_TRUE(renderNode != nullptr);
    context->GetMutableNodeMap().RegisterRenderNode(renderNode);

    // Test with invalid IDs
    std::vector<std::pair<NodeId, AnimationId>> animations1;
    animations1.emplace_back(INVALID_NODE_ID, INVALID_ANIMATION_ID);
    animator->AddAnimations(animations1);
    animator->ContinueAnimator();

    // Test with valid node but invalid animation
    std::vector<std::pair<NodeId, AnimationId>> animations2;
    animations2.emplace_back(NODE_ID, INVALID_ANIMATION_ID);
    animator->AddAnimations(animations2);
    animator->ContinueAnimator();
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest ContinueAnimator003 end";
}

/**
 * @tc.name: ContinueAnimator004
 * @tc.desc: Cover branch: node is not nullptr (RegisterAnimatingRenderNode called)
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorTest, ContinueAnimator004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest ContinueAnimator004 start";
    auto context = std::make_shared<RSContext>();
    auto animator = std::make_shared<RSRenderInteractiveImplictAnimator>(ANIMATOR_ID, context);
    ASSERT_TRUE(animator != nullptr);

    auto renderNode = std::make_shared<RSCanvasRenderNode>(NODE_ID);
    context->GetMutableNodeMap().RegisterRenderNode(renderNode);

    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);

    renderCurveAnimation->targetId_ = NODE_ID;

    animator->cachedAnimations_.emplace_back(renderCurveAnimation);

    animator->ContinueAnimator();

    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest ContinueAnimator004 end";
}

/**
 * @tc.name: FinishAnimator001
 * @tc.desc: Verify the FinishAnimator
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorTest, FinishAnimator001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest FinishAnimator001 start";
    auto context = std::make_shared<RSContext>();
    auto animator = std::make_shared<RSRenderInteractiveImplictAnimator>(ANIMATOR_ID, context);
    ASSERT_TRUE(animator != nullptr);

    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);

    animator->cachedAnimations_.emplace_back(renderCurveAnimation);

    animator->FinishAnimator(RSInteractiveAnimationPosition::CURRENT);
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest FinishAnimator001 end";
}

/**
 * @tc.name: FinishAnimator004
 * @tc.desc: Cover branch: animation is nullptr in cachedAnimations_ loop
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorTest, FinishAnimator004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest FinishAnimator004 start";
    auto context = std::make_shared<RSContext>();
    auto animator = std::make_shared<RSRenderInteractiveImplictAnimator>(ANIMATOR_ID, context);
    ASSERT_TRUE(animator != nullptr);

    animator->cachedAnimations_.emplace_back(std::weak_ptr<RSRenderAnimation>());

    animator->FinishAnimator(RSInteractiveAnimationPosition::CURRENT);
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest FinishAnimator004 end";
}

/**
 * @tc.name: FinishAnimator002
 * @tc.desc: Verify the FinishAnimator
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorTest, FinishAnimator002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest FinishAnimator002 start";
    auto context = std::make_shared<RSContext>();
    auto animator = std::make_shared<RSRenderInteractiveImplictAnimator>(ANIMATOR_ID, context);
    ASSERT_TRUE(animator != nullptr);
    context = nullptr;
    animator->FinishAnimator(RSInteractiveAnimationPosition::CURRENT);
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest FinishAnimator002 end";
}

/**
 * @tc.name: FinishAnimator003
 * @tc.desc: Cover branch: 1.node is nullptr 2.animation is nullptr
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorTest, FinishAnimator003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest FinishAnimator003 start";
    auto context = std::make_shared<RSContext>();
    ASSERT_TRUE(context != nullptr);
    auto animator = std::make_shared<RSRenderInteractiveImplictAnimator>(ANIMATOR_ID, context);
    ASSERT_TRUE(animator != nullptr);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(NODE_ID);
    ASSERT_TRUE(renderNode != nullptr);
    context->GetMutableNodeMap().RegisterRenderNode(renderNode);

    // Test with invalid IDs
    std::vector<std::pair<NodeId, AnimationId>> animations1;
    animations1.emplace_back(INVALID_NODE_ID, INVALID_ANIMATION_ID);
    animator->AddAnimations(animations1);
    animator->FinishAnimator(RSInteractiveAnimationPosition::CURRENT);

    // Test with valid node but invalid animation
    std::vector<std::pair<NodeId, AnimationId>> animations2;
    animations2.emplace_back(NODE_ID, INVALID_ANIMATION_ID);
    animator->AddAnimations(animations2);
    animator->FinishAnimator(RSInteractiveAnimationPosition::CURRENT);
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest FinishAnimator003 end";
}

/**
 * @tc.name: ReverseAnimator001
 * @tc.desc: Cover branch: node is nullptr in ReverseAnimator loop
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorTest, ReverseAnimator001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest ReverseAnimator001 start";
    auto context = std::make_shared<RSContext>();
    auto animator = std::make_shared<RSRenderInteractiveImplictAnimator>(ANIMATOR_ID, context);
    ASSERT_TRUE(animator != nullptr);

    auto renderNode = std::make_shared<RSCanvasRenderNode>(NODE_ID);
    context->GetMutableNodeMap().RegisterRenderNode(renderNode);

    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);

    renderCurveAnimation->targetId_ = INVALID_NODE_ID;

    animator->cachedAnimations_.emplace_back(renderCurveAnimation);

    animator->ReverseAnimator();
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest ReverseAnimator001 end";
}

/**
 * @tc.name: ReverseAnimator002
 * @tc.desc: Verify the ReverseAnimator
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorTest, ReverseAnimator002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest ReverseAnimator002 start";
    auto context = std::make_shared<RSContext>();
    auto animator = std::make_shared<RSRenderInteractiveImplictAnimator>(ANIMATOR_ID, context);
    ASSERT_TRUE(animator != nullptr);
    context = nullptr;
    animator->ReverseAnimator();
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest ReverseAnimator002 end";
}

/**
 * @tc.name: ReverseAnimator003
 * @tc.desc: Cover branch: 1.node is nullptr 2.animation is nullptr
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorTest, ReverseAnimator003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest ReverseAnimator003 start";
    auto context = std::make_shared<RSContext>();
    ASSERT_TRUE(context != nullptr);
    auto animator = std::make_shared<RSRenderInteractiveImplictAnimator>(ANIMATOR_ID, context);
    ASSERT_TRUE(animator != nullptr);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(NODE_ID);
    ASSERT_TRUE(renderNode != nullptr);
    context->GetMutableNodeMap().RegisterRenderNode(renderNode);

    // Test with invalid IDs
    std::vector<std::pair<NodeId, AnimationId>> animations1;
    animations1.emplace_back(INVALID_NODE_ID, INVALID_ANIMATION_ID);
    animator->AddAnimations(animations1);
    animator->ReverseAnimator();

    // Test with valid node but invalid animation
    std::vector<std::pair<NodeId, AnimationId>> animations2;
    animations2.emplace_back(NODE_ID, INVALID_ANIMATION_ID);
    animator->AddAnimations(animations2);
    animator->ReverseAnimator();
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest ReverseAnimator003 end";
}

/**
 * @tc.name: ReverseAnimator004
 * @tc.desc: Cover branch: node is not nullptr (RegisterAnimatingRenderNode called)
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorTest, ReverseAnimator004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest ReverseAnimator004 start";
    auto context = std::make_shared<RSContext>();
    auto animator = std::make_shared<RSRenderInteractiveImplictAnimator>(ANIMATOR_ID, context);
    ASSERT_TRUE(animator != nullptr);

    auto renderNode = std::make_shared<RSCanvasRenderNode>(NODE_ID);
    context->GetMutableNodeMap().RegisterRenderNode(renderNode);

    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);

    renderCurveAnimation->targetId_ = NODE_ID;

    animator->cachedAnimations_.emplace_back(renderCurveAnimation);

    animator->ReverseAnimator();

    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest ReverseAnimator004 end";
}

/**
 * @tc.name: ReverseAnimator005
 * @tc.desc: Cover branch: animation is nullptr in cachedAnimations_ loop
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorTest, ReverseAnimator005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest ReverseAnimator005 start";
    auto context = std::make_shared<RSContext>();
    auto animator = std::make_shared<RSRenderInteractiveImplictAnimator>(ANIMATOR_ID, context);
    ASSERT_TRUE(animator != nullptr);

    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);

    animator->cachedAnimations_.emplace_back(renderCurveAnimation);
    animator->cachedAnimations_.emplace_back(std::weak_ptr<RSRenderAnimation>());

    animator->ReverseAnimator();

    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest ReverseAnimator005 end";
}

/**
 * @tc.name: SetFractionAnimator001
 * @tc.desc: Verify the SetFractionAnimator
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorTest, SetFractionAnimator001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest SetFractionAnimator001 start";
    auto context = std::make_shared<RSContext>();
    auto animator = std::make_shared<RSRenderInteractiveImplictAnimator>(ANIMATOR_ID, context);
    ASSERT_TRUE(animator != nullptr);

    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);

    animator->cachedAnimations_.emplace_back(renderCurveAnimation);

    animator->SetFractionAnimator(1.0f);
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest SetFractionAnimator001 end";
}

/**
 * @tc.name: SetFractionAnimator004
 * @tc.desc: Cover branch: animation is nullptr in cachedAnimations_ loop
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorTest, SetFractionAnimator004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest SetFractionAnimator004 start";
    auto context = std::make_shared<RSContext>();
    auto animator = std::make_shared<RSRenderInteractiveImplictAnimator>(ANIMATOR_ID, context);
    ASSERT_TRUE(animator != nullptr);

    animator->cachedAnimations_.emplace_back(std::weak_ptr<RSRenderAnimation>());

    animator->SetFractionAnimator(1.0f);
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest SetFractionAnimator004 end";
}

/**
 * @tc.name: SetFractionAnimator002
 * @tc.desc: Verify the SetFractionAnimator
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorTest, SetFractionAnimator002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest SetFractionAnimator002 start";
    auto context = std::make_shared<RSContext>();
    auto animator = std::make_shared<RSRenderInteractiveImplictAnimator>(ANIMATOR_ID, context);
    ASSERT_TRUE(animator != nullptr);
    context = nullptr;
    animator->SetFractionAnimator(1.0f);
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest SetFractionAnimator002 end";
}

/**
 * @tc.name: SetFractionAnimator003
 * @tc.desc: Cover branch: 1.node is nullptr 2.animation is nullptr
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorTest, SetFractionAnimator003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest SetFractionAnimator003 start";
    auto context = std::make_shared<RSContext>();
    ASSERT_TRUE(context != nullptr);
    auto animator = std::make_shared<RSRenderInteractiveImplictAnimator>(ANIMATOR_ID, context);
    ASSERT_TRUE(animator != nullptr);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(NODE_ID);
    ASSERT_TRUE(renderNode != nullptr);
    context->GetMutableNodeMap().RegisterRenderNode(renderNode);

    // Test with invalid IDs
    std::vector<std::pair<NodeId, AnimationId>> animations1;
    animations1.emplace_back(INVALID_NODE_ID, INVALID_ANIMATION_ID);
    animator->AddAnimations(animations1);
    animator->SetFractionAnimator(1.0f);

    // Test with valid node but invalid animation
    std::vector<std::pair<NodeId, AnimationId>> animations2;
    animations2.emplace_back(NODE_ID, INVALID_ANIMATION_ID);
    animator->AddAnimations(animations2);
    animator->SetFractionAnimator(1.0f);
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest SetFractionAnimator003 end";
}

/**
 * @tc.name: TimeDrivenGroupAnimator_StartAnimator001
 * @tc.desc: Cover branch: StartAnimator when already started (should return early)
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorTest, TimeDrivenGroupAnimator_StartAnimator001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest TimeDrivenGroupAnimator_StartAnimator001 start";
    auto context = std::make_shared<RSContext>();
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);

    auto animator = std::make_shared<RSRenderTimeDrivenGroupAnimator>(ANIMATOR_ID, context, timingProtocol);
    ASSERT_TRUE(animator != nullptr);

    animator->StartAnimator();
    EXPECT_TRUE(animator->IsRunning());

    animator->StartAnimator();
    EXPECT_TRUE(animator->IsRunning());

    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest TimeDrivenGroupAnimator_StartAnimator001 end";
}

/**
 * @tc.name: TimeDrivenGroupAnimator_StartAnimator002
 * @tc.desc: Cover branch: StartAnimator when not started (should set RUNNING state)
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorTest, TimeDrivenGroupAnimator_StartAnimator002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest TimeDrivenGroupAnimator_StartAnimator002 start";
    auto context = std::make_shared<RSContext>();
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);

    auto animator = std::make_shared<RSRenderTimeDrivenGroupAnimator>(ANIMATOR_ID, context, timingProtocol);
    ASSERT_TRUE(animator != nullptr);

    EXPECT_FALSE(animator->IsRunning());
    animator->StartAnimator();
    EXPECT_TRUE(animator->IsRunning());

    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest TimeDrivenGroupAnimator_StartAnimator002 end";
}

/**
 * @tc.name: TimeDrivenGroupAnimator_PauseAnimator001
 * @tc.desc: Cover branch: PauseAnimator with state != RUNNING (should return early)
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorTest, TimeDrivenGroupAnimator_PauseAnimator001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest TimeDrivenGroupAnimator_PauseAnimator001 start";
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);

    // Create shared_ptr context
    auto context = std::make_shared<RSContext>();

    auto animator = std::make_shared<RSRenderTimeDrivenGroupAnimator>(
        ANIMATOR_ID, context, timingProtocol);
    ASSERT_TRUE(animator != nullptr);

    // Try to pause from INITIALIZED state (not RUNNING)
    animator->PauseAnimator();
    // Should return early without changing state
    EXPECT_FALSE(animator->IsRunning());

    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest TimeDrivenGroupAnimator_PauseAnimator001 end";
}

/**
 * @tc.name: TimeDrivenGroupAnimator_PauseAnimator002
 * @tc.desc: Cover branch: PauseAnimator with state = RUNNING and context != nullptr
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorTest, TimeDrivenGroupAnimator_PauseAnimator002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest TimeDrivenGroupAnimator_PauseAnimator002 start";
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);

    auto context = std::make_shared<RSContext>();
    auto animator = std::make_shared<RSRenderTimeDrivenGroupAnimator>(ANIMATOR_ID, context, timingProtocol);
    ASSERT_TRUE(animator != nullptr);

    animator->state_ = GroupAnimatorState::RUNNING;
    animator->cachedAnimations_.clear();

    animator->PauseAnimator();

    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest TimeDrivenGroupAnimator_PauseAnimator002 end";
}

/**
 * @tc.name: TimeDrivenGroupAnimator_PauseAnimator003
 * @tc.desc: Cover branch: context is nullptr in PauseAnimator
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorTest, TimeDrivenGroupAnimator_PauseAnimator003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest TimeDrivenGroupAnimator_PauseAnimator003 start";
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);

    auto context = std::make_shared<RSContext>();
    auto animator = std::make_shared<RSRenderTimeDrivenGroupAnimator>(ANIMATOR_ID, context, timingProtocol);
    ASSERT_TRUE(animator != nullptr);

    animator->state_ = GroupAnimatorState::RUNNING;
    context = nullptr;

    animator->PauseAnimator();

    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest TimeDrivenGroupAnimator_PauseAnimator003 end";
}

/**
 * @tc.name: TimeDrivenGroupAnimator_PauseAnimator004
 * @tc.desc: Cover branch: animation is nullptr in cachedAnimations_ loop
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorTest, TimeDrivenGroupAnimator_PauseAnimator004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest TimeDrivenGroupAnimator_PauseAnimator004 start";
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);

    auto context = std::make_shared<RSContext>();
    auto animator = std::make_shared<RSRenderTimeDrivenGroupAnimator>(ANIMATOR_ID, context, timingProtocol);
    ASSERT_TRUE(animator != nullptr);

    animator->cachedAnimations_.emplace_back(std::weak_ptr<RSRenderAnimation>());
    animator->state_ = GroupAnimatorState::RUNNING;

    animator->PauseAnimator();

    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest TimeDrivenGroupAnimator_PauseAnimator004 end";
}

/**
 * @tc.name: TimeDrivenGroupAnimator_PauseAnimator005
 * @tc.desc: Cover branch: animation IsRunning() in PauseAnimator
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorTest, TimeDrivenGroupAnimator_PauseAnimator005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest TimeDrivenGroupAnimator_PauseAnimator005 start";
    auto context = std::make_shared<RSContext>();
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);

    auto animator = std::make_shared<RSRenderTimeDrivenGroupAnimator>(ANIMATOR_ID, context, timingProtocol);
    ASSERT_TRUE(animator != nullptr);

    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    renderCurveAnimation->state_ = AnimationState::RUNNING;

    animator->cachedAnimations_.emplace_back(renderCurveAnimation);
    animator->state_ = GroupAnimatorState::RUNNING;

    animator->PauseAnimator();

    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest TimeDrivenGroupAnimator_PauseAnimator005 end";
}

/**
 * @tc.name: TimeDrivenGroupAnimator_PauseAnimator006
 * @tc.desc: Cover branch: animation IsGroupWaiting() in PauseAnimator
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorTest, TimeDrivenGroupAnimator_PauseAnimator006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest TimeDrivenGroupAnimator_PauseAnimator006 start";
    auto context = std::make_shared<RSContext>();
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);

    auto animator = std::make_shared<RSRenderTimeDrivenGroupAnimator>(ANIMATOR_ID, context, timingProtocol);
    ASSERT_TRUE(animator != nullptr);

    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    renderCurveAnimation->state_ = AnimationState::GROUP_WAITING;

    animator->cachedAnimations_.emplace_back(renderCurveAnimation);
    animator->state_ = GroupAnimatorState::RUNNING;

    animator->PauseAnimator();

    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest TimeDrivenGroupAnimator_PauseAnimator006 end";
}

/**
 * @tc.desc: Cover branch: ContinueAnimator with state != PAUSED (should return early)
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorTest, TimeDrivenGroupAnimator_ContinueAnimator001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest TimeDrivenGroupAnimator_ContinueAnimator001 start";
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);

    auto context = std::make_shared<RSContext>();
    auto animator = std::make_shared<RSRenderTimeDrivenGroupAnimator>(ANIMATOR_ID, context, timingProtocol);
    ASSERT_TRUE(animator != nullptr);

    animator->state_ = GroupAnimatorState::RUNNING;

    animator->ContinueAnimator();

    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest TimeDrivenGroupAnimator_ContinueAnimator001 end";
}

/**
 * @tc.name: TimeDrivenGroupAnimator_ContinueAnimator002
 * @tc.desc: Cover branch: ContinueAnimator with state = PAUSED (should set RUNNING state)
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorTest, TimeDrivenGroupAnimator_ContinueAnimator002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest TimeDrivenGroupAnimator_ContinueAnimator002 start";
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);

    auto context = std::make_shared<RSContext>();
    auto animator = std::make_shared<RSRenderTimeDrivenGroupAnimator>(ANIMATOR_ID, context, timingProtocol);
    ASSERT_TRUE(animator != nullptr);

    animator->state_ = GroupAnimatorState::PAUSED;

    animator->ContinueAnimator();

    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest TimeDrivenGroupAnimator_ContinueAnimator002 end";
}

/**
 * @tc.name: TimeDrivenGroupAnimator_ContinueAnimator003
 * @tc.desc: Cover branch: context is nullptr in ContinueAnimator
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorTest, TimeDrivenGroupAnimator_ContinueAnimator003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest TimeDrivenGroupAnimator_ContinueAnimator003 start";
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);

    auto context = std::make_shared<RSContext>();
    auto animator = std::make_shared<RSRenderTimeDrivenGroupAnimator>(ANIMATOR_ID, context, timingProtocol);
    ASSERT_TRUE(animator != nullptr);

    animator->state_ = GroupAnimatorState::PAUSED;

    context = nullptr;
    animator->ContinueAnimator();

    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest TimeDrivenGroupAnimator_ContinueAnimator003 end";
}

/**
 * @tc.name: TimeDrivenGroupAnimator_ContinueAnimator004
 * @tc.desc: Cover branch: animation is nullptr in cachedAnimations_ loop
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorTest, TimeDrivenGroupAnimator_ContinueAnimator004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest TimeDrivenGroupAnimator_ContinueAnimator004 start";
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);

    auto context = std::make_shared<RSContext>();
    auto animator = std::make_shared<RSRenderTimeDrivenGroupAnimator>(ANIMATOR_ID, context, timingProtocol);
    ASSERT_TRUE(animator != nullptr);

    animator->cachedAnimations_.emplace_back(std::weak_ptr<RSRenderAnimation>());
    animator->state_ = GroupAnimatorState::PAUSED;

    animator->ContinueAnimator();

    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest TimeDrivenGroupAnimator_ContinueAnimator004 end";
}

/**
 * @tc.name: TimeDrivenGroupAnimator_ContinueAnimator005
 * @tc.desc: Cover branch: animation in groupWaitingAnimationIds_ (ResumeGroupWaiting)
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorTest, TimeDrivenGroupAnimator_ContinueAnimator005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest TimeDrivenGroupAnimator_ContinueAnimator005 start";
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);

    auto context = std::make_shared<RSContext>();
    auto animator = std::make_shared<RSRenderTimeDrivenGroupAnimator>(ANIMATOR_ID, context, timingProtocol);
    ASSERT_TRUE(animator != nullptr);

    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);

    animator->cachedAnimations_.emplace_back(renderCurveAnimation);
    animator->groupWaitingAnimationIds_.insert(ANIMATION_ID);
    animator->state_ = GroupAnimatorState::PAUSED;

    animator->ContinueAnimator();

    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest TimeDrivenGroupAnimator_ContinueAnimator005 end";
}

/**
 * @tc.name: TimeDrivenGroupAnimator_ContinueAnimator006
 * @tc.desc: Cover branch: animation not in groupWaitingAnimationIds_ (Resume)
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorTest, TimeDrivenGroupAnimator_ContinueAnimator006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest TimeDrivenGroupAnimator_ContinueAnimator006 start";
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);

    auto context = std::make_shared<RSContext>();
    auto animator = std::make_shared<RSRenderTimeDrivenGroupAnimator>(ANIMATOR_ID, context, timingProtocol);
    ASSERT_TRUE(animator != nullptr);

    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);

    animator->cachedAnimations_.emplace_back(renderCurveAnimation);
    animator->state_ = GroupAnimatorState::PAUSED;

    animator->ContinueAnimator();

    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest TimeDrivenGroupAnimator_ContinueAnimator006 end";
}

/**
 * @tc.name: TimeDrivenGroupAnimator_ContinueAnimator007
 * @tc.desc: Cover branch: node is nullptr in ContinueAnimator loop
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorTest, TimeDrivenGroupAnimator_ContinueAnimator007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest TimeDrivenGroupAnimator_ContinueAnimator007 start";
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);

    auto context = std::make_shared<RSContext>();
    auto animator = std::make_shared<RSRenderTimeDrivenGroupAnimator>(ANIMATOR_ID, context, timingProtocol);
    ASSERT_TRUE(animator != nullptr);

    auto renderNode = std::make_shared<RSCanvasRenderNode>(NODE_ID);
    context->GetMutableNodeMap().RegisterRenderNode(renderNode);

    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    renderCurveAnimation->targetId_ = INVALID_NODE_ID;

    animator->cachedAnimations_.emplace_back(renderCurveAnimation);
    animator->state_ = GroupAnimatorState::PAUSED;

    animator->ContinueAnimator();

    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest TimeDrivenGroupAnimator_ContinueAnimator007 end";
}

/**
 * @tc.name: TimeDrivenGroupAnimator_ContinueAnimator008
 * @tc.desc: Cover branch: node is not nullptr in ContinueAnimator loop
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorTest, TimeDrivenGroupAnimator_ContinueAnimator008, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest TimeDrivenGroupAnimator_ContinueAnimator008 start";
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);

    auto context = std::make_shared<RSContext>();
    auto animator = std::make_shared<RSRenderTimeDrivenGroupAnimator>(ANIMATOR_ID, context, timingProtocol);
    ASSERT_TRUE(animator != nullptr);

    auto renderNode = std::make_shared<RSCanvasRenderNode>(NODE_ID);
    context->GetMutableNodeMap().RegisterRenderNode(renderNode);

    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    renderCurveAnimation->targetId_ = NODE_ID;

    animator->cachedAnimations_.emplace_back(renderCurveAnimation);
    animator->state_ = GroupAnimatorState::PAUSED;

    animator->ContinueAnimator();

    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest TimeDrivenGroupAnimator_ContinueAnimator008 end";
}

/**
 * @tc.name: TimeDrivenGroupAnimator_OnAnimate001
 * @tc.desc: Cover branch: OnAnimate with state = FINISHED
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorTest, TimeDrivenGroupAnimator_OnAnimate001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest TimeDrivenGroupAnimator_OnAnimate001 start";
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);

    auto context = std::make_shared<RSContext>();
    auto animator = std::make_shared<RSRenderTimeDrivenGroupAnimator>(ANIMATOR_ID, context, timingProtocol);
    ASSERT_TRUE(animator != nullptr);

    animator->state_ = GroupAnimatorState::FINISHED;

    int64_t minLeftDelayTime = 0;
    animator->OnAnimate(1000, minLeftDelayTime);

    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest TimeDrivenGroupAnimator_OnAnimate001 end";
}

/**
 * @tc.name: TimeDrivenGroupAnimator_OnAnimate002
 * @tc.desc: Cover branch: OnAnimate with state = PAUSED (should return early)
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorTest, TimeDrivenGroupAnimator_OnAnimate002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest TimeDrivenGroupAnimator_OnAnimate002 start";
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);

    auto context = std::make_shared<RSContext>();
    auto animator = std::make_shared<RSRenderTimeDrivenGroupAnimator>(ANIMATOR_ID, context, timingProtocol);
    ASSERT_TRUE(animator != nullptr);

    animator->state_ = GroupAnimatorState::PAUSED;

    int64_t minLeftDelayTime = 0;
    animator->OnAnimate(100, minLeftDelayTime);

    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest TimeDrivenGroupAnimator_OnAnimate002 end";
}

/**
 * @tc.name: TimeDrivenGroupAnimator_OnAnimate003
 * @tc.desc: Cover branch: OnAnimate with needUpdateStartTime_ = true
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorTest, TimeDrivenGroupAnimator_OnAnimate003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest TimeDrivenGroupAnimator_OnAnimate003 start";
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);

    auto context = std::make_shared<RSContext>();
    auto animator = std::make_shared<RSRenderTimeDrivenGroupAnimator>(ANIMATOR_ID, context, timingProtocol);
    ASSERT_TRUE(animator != nullptr);

    animator->state_ = GroupAnimatorState::RUNNING;
    animator->needUpdateStartTime_ = true;

    int64_t minLeftDelayTime = 0;
    animator->OnAnimate(100, minLeftDelayTime);

    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest TimeDrivenGroupAnimator_OnAnimate003 end";
}

/**
 * @tc.name: TimeDrivenGroupAnimator_RemoveActiveChildAnimation001
 * @tc.desc: Cover branch: RemoveActiveChildAnimation with empty activeChildAnimations_
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorTest, TimeDrivenGroupAnimator_RemoveActiveChildAnimation001, TestSize.Level1)
{
    auto context = std::make_shared<RSContext>();
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);

    auto animator = std::make_shared<RSRenderTimeDrivenGroupAnimator>(ANIMATOR_ID, context, timingProtocol);
    ASSERT_TRUE(animator != nullptr);

    animator->RemoveActiveChildAnimation(ANIMATION_ID);
}

/**
 * @tc.name: TimeDrivenGroupAnimator_RemoveActiveChildAnimation002
 * @tc.desc: Cover branch: RemoveActiveChildAnimation with non-existent animationId (it == end)
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorTest, TimeDrivenGroupAnimator_RemoveActiveChildAnimation002, TestSize.Level1)
{
    auto context = std::make_shared<RSContext>();
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);

    auto animator = std::make_shared<RSRenderTimeDrivenGroupAnimator>(ANIMATOR_ID, context, timingProtocol);
    ASSERT_TRUE(animator != nullptr);

    animator->activeChildAnimations_.insert(ANIMATION_ID);

    animator->RemoveActiveChildAnimation(INVALID_ANIMATION_ID);
}

/**
 * @tc.name: TimeDrivenGroupAnimator_RemoveActiveChildAnimation003
 * @tc.desc: Cover branch: RemoveActiveChildAnimation and activeChildAnimations_ becomes empty, state -> FINISHED
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorTest, TimeDrivenGroupAnimator_RemoveActiveChildAnimation003, TestSize.Level1)
{
    auto context = std::make_shared<RSContext>();
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);

    auto animator = std::make_shared<RSRenderTimeDrivenGroupAnimator>(ANIMATOR_ID, context, timingProtocol);
    ASSERT_TRUE(animator != nullptr);

    animator->activeChildAnimations_.insert(ANIMATION_ID);

    animator->RemoveActiveChildAnimation(ANIMATION_ID);
    EXPECT_TRUE(animator->activeChildAnimations_.empty());
}

/**
 * @tc.name: TimeDrivenGroupAnimator_RemoveActiveChildAnimation004
 * @tc.desc: Cover branch: RemoveActiveChildAnimation and activeChildAnimations_ not empty after remove
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorTest, TimeDrivenGroupAnimator_RemoveActiveChildAnimation004, TestSize.Level1)
{
    auto context = std::make_shared<RSContext>();
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);

    auto animator = std::make_shared<RSRenderTimeDrivenGroupAnimator>(ANIMATOR_ID, context, timingProtocol);
    ASSERT_TRUE(animator != nullptr);

    animator->activeChildAnimations_.insert(ANIMATION_ID);
    animator->activeChildAnimations_.insert(ANIMATION_ID + 1);

    animator->RemoveActiveChildAnimation(ANIMATION_ID);
    EXPECT_FALSE(animator->activeChildAnimations_.empty());
    EXPECT_EQ(animator->activeChildAnimations_.size(), 1);
}

/**
 * @tc.name: TimeDrivenGroupAnimator_ResetChildAnimations001
 * @tc.desc: Cover branch: ResetChildAnimations with remainingRepeatCount == 0 (return early)
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorTest, TimeDrivenGroupAnimator_ResetChildAnimations001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest TimeDrivenGroupAnimator_ResetChildAnimations001 start";
    auto context = std::make_shared<RSContext>();
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);
    timingProtocol.SetRepeatCount(1);

    auto animator = std::make_shared<RSRenderTimeDrivenGroupAnimator>(ANIMATOR_ID, context, timingProtocol);
    ASSERT_TRUE(animator != nullptr);

    animator->animationFraction_.currentRepeatCount_ = 1;

    animator->ResetChildAnimations();

    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest TimeDrivenGroupAnimator_ResetChildAnimations001 end";
}

/**
 * @tc.name: TimeDrivenGroupAnimator_ResetChildAnimations002
 * @tc.desc: Cover branch: ResetChildAnimations with AutoReverse true (FlipDirection)
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorTest, TimeDrivenGroupAnimator_ResetChildAnimations002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest TimeDrivenGroupAnimator_ResetChildAnimations002 start";
    auto context = std::make_shared<RSContext>();
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);
    timingProtocol.SetRepeatCount(2);
    timingProtocol.SetAutoReverse(true);

    auto animator = std::make_shared<RSRenderTimeDrivenGroupAnimator>(ANIMATOR_ID, context, timingProtocol);
    ASSERT_TRUE(animator != nullptr);

    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);

    animator->cachedAnimations_.emplace_back(renderCurveAnimation);
    animator->animationFraction_.currentRepeatCount_ = 1;

    animator->ResetChildAnimations();

    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest TimeDrivenGroupAnimator_ResetChildAnimations002 end";
}

/**
 * @tc.name: TimeDrivenGroupAnimator_ResetChildAnimations003
 * @tc.desc: Cover branch: context is nullptr in ResetChildAnimations
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorTest, TimeDrivenGroupAnimator_ResetChildAnimations003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest TimeDrivenGroupAnimator_ResetChildAnimations003 start";
    auto context = std::make_shared<RSContext>();
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);
    timingProtocol.SetRepeatCount(2);

    auto animator = std::make_shared<RSRenderTimeDrivenGroupAnimator>(ANIMATOR_ID, context, timingProtocol);
    ASSERT_TRUE(animator != nullptr);

    animator->animationFraction_.currentRepeatCount_ = 1;
    context = nullptr;

    animator->ResetChildAnimations();

    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest TimeDrivenGroupAnimator_ResetChildAnimations003 end";
}

/**
 * @tc.name: TimeDrivenGroupAnimator_ResetChildAnimations004
 * @tc.desc: Cover branch: ResetChildAnimations with AutoReverse false (Restart)
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorTest, TimeDrivenGroupAnimator_ResetChildAnimations004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest TimeDrivenGroupAnimator_ResetChildAnimations004 start";
    auto context = std::make_shared<RSContext>();
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);
    timingProtocol.SetRepeatCount(2);
    timingProtocol.SetAutoReverse(false);

    auto animator = std::make_shared<RSRenderTimeDrivenGroupAnimator>(ANIMATOR_ID, context, timingProtocol);
    ASSERT_TRUE(animator != nullptr);

    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);

    animator->cachedAnimations_.emplace_back(renderCurveAnimation);
    animator->animationFraction_.currentRepeatCount_ = 1;

    animator->ResetChildAnimations();

    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest TimeDrivenGroupAnimator_ResetChildAnimations004 end";
}

/**
 * @tc.name: TimeDrivenGroupAnimator_ResetChildAnimations005
 * @tc.desc: Cover branch: animation is nullptr in cachedAnimations_ loop
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorTest, TimeDrivenGroupAnimator_ResetChildAnimations005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest TimeDrivenGroupAnimator_ResetChildAnimations005 start";
    auto context = std::make_shared<RSContext>();
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);
    timingProtocol.SetRepeatCount(2);

    auto animator = std::make_shared<RSRenderTimeDrivenGroupAnimator>(ANIMATOR_ID, context, timingProtocol);
    ASSERT_TRUE(animator != nullptr);

    animator->cachedAnimations_.emplace_back(std::weak_ptr<RSRenderAnimation>());
    animator->animationFraction_.currentRepeatCount_ = 1;

    animator->ResetChildAnimations();

    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest TimeDrivenGroupAnimator_ResetChildAnimations005 end";
}

/**
 * @tc.name: TimeDrivenGroupAnimator_StartChildAnimations001
 * @tc.desc: Cover branch: StartChildAnimations with activeChildAnimations_ not empty (return early)
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorTest, TimeDrivenGroupAnimator_StartChildAnimations001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest TimeDrivenGroupAnimator_StartChildAnimations001 start";
    auto context = std::make_shared<RSContext>();
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);

    auto animator = std::make_shared<RSRenderTimeDrivenGroupAnimator>(ANIMATOR_ID, context, timingProtocol);
    ASSERT_TRUE(animator != nullptr);

    animator->activeChildAnimations_.insert(ANIMATION_ID);

    animator->StartChildAnimations();

    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest TimeDrivenGroupAnimator_StartChildAnimations001 end";
}

/**
 * @tc.name: TimeDrivenGroupAnimator_StartChildAnimations002
 * @tc.desc: Cover branch: StartChildAnimations with node not nullptr
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorTest, TimeDrivenGroupAnimator_StartChildAnimations002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest TimeDrivenGroupAnimator_StartChildAnimations002 start";
    auto context = std::make_shared<RSContext>();
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);

    auto animator = std::make_shared<RSRenderTimeDrivenGroupAnimator>(ANIMATOR_ID, context, timingProtocol);
    ASSERT_TRUE(animator != nullptr);

    auto renderNode = std::make_shared<RSCanvasRenderNode>(NODE_ID);
    context->GetMutableNodeMap().RegisterRenderNode(renderNode);

    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    renderCurveAnimation->targetId_ = NODE_ID;

    animator->cachedAnimations_.emplace_back(renderCurveAnimation);

    animator->StartChildAnimations();

    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest TimeDrivenGroupAnimator_StartChildAnimations002 end";
}

/**
 * @tc.name: TimeDrivenGroupAnimator_StartChildAnimations003
 * @tc.desc: Cover branch: animation is nullptr in cachedAnimations_ loop
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorTest, TimeDrivenGroupAnimator_StartChildAnimations003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest TimeDrivenGroupAnimator_StartChildAnimations003 start";
    auto context = std::make_shared<RSContext>();
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);

    auto animator = std::make_shared<RSRenderTimeDrivenGroupAnimator>(ANIMATOR_ID, context, timingProtocol);
    ASSERT_TRUE(animator != nullptr);

    animator->cachedAnimations_.emplace_back(std::weak_ptr<RSRenderAnimation>());

    animator->StartChildAnimations();

    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest TimeDrivenGroupAnimator_StartChildAnimations003 end";
}

/**
 * @tc.name: TimeDrivenGroupAnimator_StartChildAnimations004
 * @tc.desc: Cover branch: node is nullptr in StartChildAnimations loop
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorTest, TimeDrivenGroupAnimator_StartChildAnimations004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest TimeDrivenGroupAnimator_StartChildAnimations004 start";
    auto context = std::make_shared<RSContext>();
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);

    auto animator = std::make_shared<RSRenderTimeDrivenGroupAnimator>(ANIMATOR_ID, context, timingProtocol);
    ASSERT_TRUE(animator != nullptr);

    auto renderNode = std::make_shared<RSCanvasRenderNode>(NODE_ID);
    context->GetMutableNodeMap().RegisterRenderNode(renderNode);

    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    renderCurveAnimation->targetId_ = INVALID_NODE_ID;

    animator->cachedAnimations_.emplace_back(renderCurveAnimation);

    animator->StartChildAnimations();

    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest TimeDrivenGroupAnimator_StartChildAnimations004 end";
}

/**
 * @tc.name: TimeDrivenGroupAnimator_FinishAnimator001
 * @tc.desc: Cover branch: FinishAnimator (TimeDriven) with node nullptr
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorTest, TimeDrivenGroupAnimator_FinishAnimator001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest TimeDrivenGroupAnimator_FinishAnimator001 start";
    auto context = std::make_shared<RSContext>();
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);

    auto animator = std::make_shared<RSRenderTimeDrivenGroupAnimator>(ANIMATOR_ID, context, timingProtocol);
    ASSERT_TRUE(animator != nullptr);

    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    renderCurveAnimation->targetId_ = NODE_ID;

    animator->cachedAnimations_.emplace_back(renderCurveAnimation);

    animator->FinishAnimator(RSInteractiveAnimationPosition::END);

    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest TimeDrivenGroupAnimator_FinishAnimator001 end";
}

/**
 * @tc.name: TimeDrivenGroupAnimator_FinishAnimator002
 * @tc.desc: Cover branch: animation is nullptr in cachedAnimations_ loop
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorTest, TimeDrivenGroupAnimator_FinishAnimator002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest TimeDrivenGroupAnimator_FinishAnimator002 start";
    auto context = std::make_shared<RSContext>();
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);

    auto animator = std::make_shared<RSRenderTimeDrivenGroupAnimator>(ANIMATOR_ID, context, timingProtocol);
    ASSERT_TRUE(animator != nullptr);

    animator->cachedAnimations_.emplace_back(std::weak_ptr<RSRenderAnimation>());

    animator->FinishAnimator(RSInteractiveAnimationPosition::END);

    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest TimeDrivenGroupAnimator_FinishAnimator002 end";
}

/**
 * @tc.name: TimeDrivenGroupAnimator_FinishAnimator003
 * @tc.desc: Cover branch: context is nullptr in FinishAnimator
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorTest, TimeDrivenGroupAnimator_FinishAnimator003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest TimeDrivenGroupAnimator_FinishAnimator003 start";
    auto context = std::make_shared<RSContext>();
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);

    auto animator = std::make_shared<RSRenderTimeDrivenGroupAnimator>(ANIMATOR_ID, context, timingProtocol);
    ASSERT_TRUE(animator != nullptr);

    animator->cachedAnimations_.emplace_back(std::weak_ptr<RSRenderAnimation>());
    context = nullptr;

    animator->FinishAnimator(RSInteractiveAnimationPosition::END);

    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest TimeDrivenGroupAnimator_FinishAnimator003 end";
}

/**
 * @tc.name: TimeDrivenGroupAnimator_FinishAnimator004
 * @tc.desc: Cover branch: node is not nullptr in FinishAnimator loop
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorTest, TimeDrivenGroupAnimator_FinishAnimator004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest TimeDrivenGroupAnimator_FinishAnimator004 start";
    auto context = std::make_shared<RSContext>();
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);

    auto animator = std::make_shared<RSRenderTimeDrivenGroupAnimator>(ANIMATOR_ID, context, timingProtocol);
    ASSERT_TRUE(animator != nullptr);

    auto renderNode = std::make_shared<RSCanvasRenderNode>(NODE_ID);
    context->GetMutableNodeMap().RegisterRenderNode(renderNode);

    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    renderCurveAnimation->targetId_ = NODE_ID;
    renderNode->GetAnimationManager().AddAnimation(renderCurveAnimation);

    animator->cachedAnimations_.emplace_back(renderCurveAnimation);

    animator->FinishAnimator(RSInteractiveAnimationPosition::END);

    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest TimeDrivenGroupAnimator_FinishAnimator004 end";
}

/**
 * @tc.name: TimeDrivenGroupAnimator_UpdateFraction001
 * @tc.desc: Cover branch: isRepeatFinished = true in UpdateFraction
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorTest, TimeDrivenGroupAnimator_UpdateFraction001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest TimeDrivenGroupAnimator_UpdateFraction001 start";
    auto context = std::make_shared<RSContext>();
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(100);
    timingProtocol.SetRepeatCount(2);

    auto animator = std::make_shared<RSRenderTimeDrivenGroupAnimator>(ANIMATOR_ID, context, timingProtocol);
    ASSERT_TRUE(animator != nullptr);

    animator->state_ = GroupAnimatorState::RUNNING;
    animator->needUpdateStartTime_ = false;
    animator->animationFraction_.SetLastFrameTime(0);
    animator->animationFraction_.runningTime_ = 150000000;
    animator->animationFraction_.currentRepeatCount_ = 0;
    animator->animationFraction_.direction_ = ForwardDirection::NORMAL;
    animator->animationFraction_.isRepeatCallbackEnable_ = true;

    int64_t minLeftDelayTime = 0;
    animator->UpdateFraction(100000000, minLeftDelayTime);

    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest TimeDrivenGroupAnimator_UpdateFraction001 end";
}

/**
 * @tc.name: TimeDrivenGroupAnimator_UpdateFraction002
 * @tc.desc: Cover branch: isInStartDelay = true in UpdateFraction
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorTest, TimeDrivenGroupAnimator_UpdateFraction002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest TimeDrivenGroupAnimator_UpdateFraction002 start";
    auto context = std::make_shared<RSContext>();
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(1000);
    timingProtocol.SetStartDelay(500);

    auto animator = std::make_shared<RSRenderTimeDrivenGroupAnimator>(ANIMATOR_ID, context, timingProtocol);
    ASSERT_TRUE(animator != nullptr);

    animator->state_ = GroupAnimatorState::RUNNING;
    animator->needUpdateStartTime_ = false;
    animator->animationFraction_.SetLastFrameTime(0);
    animator->animationFraction_.runningTime_ = 100000000;

    int64_t minLeftDelayTime = 1000000000;
    animator->UpdateFraction(100000000, minLeftDelayTime);

    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest TimeDrivenGroupAnimator_UpdateFraction002 end";
}

/**
 * @tc.name: TimeDrivenGroupAnimator_UpdateFraction003
 * @tc.desc: Cover branch: isFinished = true in UpdateFraction
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorTest, TimeDrivenGroupAnimator_UpdateFraction003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest TimeDrivenGroupAnimator_UpdateFraction003 start";
    auto context = std::make_shared<RSContext>();
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetDuration(100);

    auto animator = std::make_shared<RSRenderTimeDrivenGroupAnimator>(ANIMATOR_ID, context, timingProtocol);
    ASSERT_TRUE(animator != nullptr);

    animator->state_ = GroupAnimatorState::RUNNING;
    animator->needUpdateStartTime_ = false;
    animator->animationFraction_.SetLastFrameTime(0);
    animator->animationFraction_.runningTime_ = 200000000;
    animator->animationFraction_.direction_ = ForwardDirection::NORMAL;

    animator->cachedAnimations_.clear();

    int64_t minLeftDelayTime = 0;
    animator->UpdateFraction(200000000, minLeftDelayTime);

    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest TimeDrivenGroupAnimator_UpdateFraction003 end";
}

} // namespace Rosen
} // namespace OHOS
