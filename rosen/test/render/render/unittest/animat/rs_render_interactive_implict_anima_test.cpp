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
    RSContext context;
    auto animator = std::make_shared<RSRenderInteractiveImplictAnimator>(ANIMATOR_ID, context.weak_from_this());
    EXPECT_TRUE(animator != nullptr);
    std::vector<std::pair<NodeId, AnimationId>> animations1;
    animations1.emplace_back(0, 0);
    animator->AddAnimations(animations1);

    auto renderNode = std::make_shared<RSCanvasRenderNode>(NODE_ID);
    context.GetMutableNodeMap().RegisterRenderNode(renderNode);
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
    EXPECT_TRUE(animator != nullptr);
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
    EXPECT_TRUE(context != nullptr);
    auto animator = std::make_shared<RSRenderInteractiveImplictAnimator>(ANIMATOR_ID, context);
    EXPECT_TRUE(animator != nullptr);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(NODE_ID);
    EXPECT_TRUE(renderNode != nullptr);
    context->GetMutableNodeMap().RegisterRenderNode(renderNode);

    std::vector<std::pair<NodeId, AnimationId>> animations;
    animations.emplace_back(INVALID_NODE_ID, INVALID_ANIMATION_ID);
    animations.emplace_back(NODE_ID, INVALID_ANIMATION_ID);
    animator->AddAnimations(animations);
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest AddAnimations003 end";
}

/**
 * @tc.name: PauseAnimator001
 * @tc.desc: Verify the PauseAnimator
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorTest, PauseAnimator001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest PauseAnimator001 start";
    RSContext context;
    auto animator = std::make_shared<RSRenderInteractiveImplictAnimator>(ANIMATOR_ID, context.weak_from_this());
    EXPECT_TRUE(animator != nullptr);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(NODE_ID);
    context.GetMutableNodeMap().RegisterRenderNode(renderNode);
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    renderNode->GetAnimationManager().AddAnimation(renderCurveAnimation);

    std::vector<std::pair<NodeId, AnimationId>> animations;
    animations.emplace_back(NODE_ID, ANIMATION_ID);
    animator->AddAnimations(animations);
    animator->PauseAnimator();
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest PauseAnimator001 end";
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
    EXPECT_TRUE(animator != nullptr);
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
    EXPECT_TRUE(context != nullptr);
    auto animator = std::make_shared<RSRenderInteractiveImplictAnimator>(ANIMATOR_ID, context);
    EXPECT_TRUE(animator != nullptr);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(NODE_ID);
    EXPECT_TRUE(renderNode != nullptr);
    context->GetMutableNodeMap().RegisterRenderNode(renderNode);

    animator->animations_.emplace_back(INVALID_NODE_ID, INVALID_ANIMATION_ID);
    animator->PauseAnimator();
    animator->animations_.clear();
    animator->animations_.emplace_back(NODE_ID, INVALID_ANIMATION_ID);
    animator->PauseAnimator();
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest PauseAnimator003 end";
}

/**
 * @tc.name: ContinueAnimator001
 * @tc.desc: Verify the ContinueAnimator
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorTest, ContinueAnimator001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest ContinueAnimator001 start";
    RSContext context;
    auto animator = std::make_shared<RSRenderInteractiveImplictAnimator>(ANIMATOR_ID, context.weak_from_this());
    EXPECT_TRUE(animator != nullptr);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(NODE_ID);
    context.GetMutableNodeMap().RegisterRenderNode(renderNode);
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    renderNode->GetAnimationManager().AddAnimation(renderCurveAnimation);

    std::vector<std::pair<NodeId, AnimationId>> animations;
    animations.emplace_back(NODE_ID, ANIMATION_ID);
    animator->AddAnimations(animations);
    animator->ContinueAnimator();
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest ContinueAnimator001 end";
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
    EXPECT_TRUE(animator != nullptr);
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
    EXPECT_TRUE(context != nullptr);
    auto animator = std::make_shared<RSRenderInteractiveImplictAnimator>(ANIMATOR_ID, context);
    EXPECT_TRUE(animator != nullptr);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(NODE_ID);
    EXPECT_TRUE(renderNode != nullptr);
    context->GetMutableNodeMap().RegisterRenderNode(renderNode);

    animator->animations_.emplace_back(INVALID_NODE_ID, INVALID_ANIMATION_ID);
    animator->ContinueAnimator();
    animator->animations_.clear();
    animator->animations_.emplace_back(NODE_ID, INVALID_ANIMATION_ID);
    animator->ContinueAnimator();
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest ContinueAnimator003 end";
}

/**
 * @tc.name: FinishAnimator001
 * @tc.desc: Verify the FinishAnimator
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorTest, FinishAnimator001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest FinishAnimator001 start";
    RSContext context;
    auto animator = std::make_shared<RSRenderInteractiveImplictAnimator>(ANIMATOR_ID, context.weak_from_this());
    EXPECT_TRUE(animator != nullptr);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(NODE_ID);
    context.GetMutableNodeMap().RegisterRenderNode(renderNode);
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    renderNode->GetAnimationManager().AddAnimation(renderCurveAnimation);

    std::vector<std::pair<NodeId, AnimationId>> animations;
    animations.emplace_back(NODE_ID, ANIMATION_ID);
    animator->AddAnimations(animations);
    animator->FinishAnimator(RSInteractiveAnimationPosition::CURRENT);
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest FinishAnimator001 end";
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
    EXPECT_TRUE(animator != nullptr);
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
    EXPECT_TRUE(context != nullptr);
    auto animator = std::make_shared<RSRenderInteractiveImplictAnimator>(ANIMATOR_ID, context);
    EXPECT_TRUE(animator != nullptr);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(NODE_ID);
    EXPECT_TRUE(renderNode != nullptr);
    context->GetMutableNodeMap().RegisterRenderNode(renderNode);

    animator->animations_.emplace_back(INVALID_NODE_ID, INVALID_ANIMATION_ID);
    animator->FinishAnimator(RSInteractiveAnimationPosition::CURRENT);
    animator->animations_.clear();
    animator->animations_.emplace_back(NODE_ID, INVALID_ANIMATION_ID);
    animator->FinishAnimator(RSInteractiveAnimationPosition::CURRENT);
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest FinishAnimator003 end";
}

/**
 * @tc.name: ReverseAnimator001
 * @tc.desc: Verify the ReverseAnimator
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorTest, ReverseAnimator001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest ReverseAnimator001 start";
    RSContext context;
    auto animator = std::make_shared<RSRenderInteractiveImplictAnimator>(ANIMATOR_ID, context.weak_from_this());
    EXPECT_TRUE(animator != nullptr);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(NODE_ID);
    context.GetMutableNodeMap().RegisterRenderNode(renderNode);
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    renderNode->GetAnimationManager().AddAnimation(renderCurveAnimation);

    std::vector<std::pair<NodeId, AnimationId>> animations;
    animations.emplace_back(NODE_ID, ANIMATION_ID);
    animator->AddAnimations(animations);
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
    EXPECT_TRUE(animator != nullptr);
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
    EXPECT_TRUE(context != nullptr);
    auto animator = std::make_shared<RSRenderInteractiveImplictAnimator>(ANIMATOR_ID, context);
    EXPECT_TRUE(animator != nullptr);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(NODE_ID);
    EXPECT_TRUE(renderNode != nullptr);
    context->GetMutableNodeMap().RegisterRenderNode(renderNode);

    animator->animations_.emplace_back(INVALID_NODE_ID, INVALID_ANIMATION_ID);
    animator->ReverseAnimator();
    animator->animations_.clear();
    animator->animations_.emplace_back(NODE_ID, INVALID_ANIMATION_ID);
    animator->ReverseAnimator();
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest ReverseAnimator003 end";
}

/**
 * @tc.name: SetFractionAnimator001
 * @tc.desc: Verify the SetFractionAnimator
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveImplictAnimatorTest, SetFractionAnimator001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest SetFractionAnimator001 start";
    RSContext context;
    auto animator = std::make_shared<RSRenderInteractiveImplictAnimator>(ANIMATOR_ID, context.weak_from_this());
    EXPECT_TRUE(animator != nullptr);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(NODE_ID);
    context.GetMutableNodeMap().RegisterRenderNode(renderNode);
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    renderNode->GetAnimationManager().AddAnimation(renderCurveAnimation);

    std::vector<std::pair<NodeId, AnimationId>> animations;
    animations.emplace_back(NODE_ID, ANIMATION_ID);
    animator->AddAnimations(animations);
    animator->SetFractionAnimator(1.0f);
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest SetFractionAnimator001 end";
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
    EXPECT_TRUE(animator != nullptr);
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
    EXPECT_TRUE(context != nullptr);
    auto animator = std::make_shared<RSRenderInteractiveImplictAnimator>(ANIMATOR_ID, context);
    EXPECT_TRUE(animator != nullptr);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(NODE_ID);
    EXPECT_TRUE(renderNode != nullptr);
    context->GetMutableNodeMap().RegisterRenderNode(renderNode);

    animator->animations_.emplace_back(INVALID_NODE_ID, INVALID_ANIMATION_ID);
    animator->SetFractionAnimator(1.0f);
    animator->animations_.clear();
    animator->animations_.emplace_back(NODE_ID, INVALID_ANIMATION_ID);
    animator->SetFractionAnimator(1.0f);
    GTEST_LOG_(INFO) << "RSRenderInteractiveImplictAnimatorTest SetFractionAnimator003 end";
}
} // namespace Rosen
} // namespace OHOS
