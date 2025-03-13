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
class RSRenderInteractiveAnimationTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static constexpr InteractiveImplictAnimatorId ANIMATOR_TEST_ID = 100051;
    static constexpr NodeId NODE_TEST_ID = 15515;
    static constexpr AnimationId ANIMATION_TEST_ID = 98648;
    static constexpr uint64_t PROPERTY_TEST_ID = 102145;
    static constexpr NodeId INVALID_NODE_TEST_ID = std::numeric_limits<uint64_t>::max();
    static constexpr AnimationId INVALID_ANIMATION_TEST_ID = std::numeric_limits<uint64_t>::max();
};

void RSRenderInteractiveAnimationTest::SetUpTestCase() {}
void RSRenderInteractiveAnimationTest::TearDownTestCase() {}
void RSRenderInteractiveAnimationTest::SetUp() {}
void RSRenderInteractiveAnimationTest::TearDown() {}

/**
 * @tc.name: AddAnimations001
 * @tc.desc: Verify the AddAnimations
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveAnimationTest, AddAnimations001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveAnimationTest AddAnimations001 start";
    RSContext context;
    auto animator = std::make_shared<RSRenderInteractiveImplictAnimator>(ANIMATOR_TEST_ID, context.weak_from_this());
    EXPECT_TRUE(animator != nullptr);
    std::vector<std::pair<NodeId, AnimationId>> animations1;
    animations1.emplace_back(0, 0);
    animator->AddAnimations(animations1);

    auto renderNode = std::make_shared<RSCanvasRenderNode>(NODE_TEST_ID);
    context.GetMutableNodeMap().RegisterRenderNode(renderNode);
    std::vector<std::pair<NodeId, AnimationId>> animations2;
    animations2.emplace_back(NODE_TEST_ID, 0);
    animator->AddAnimations(animations2);

    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.2f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.3f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.4f);
    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_TEST_ID, PROPERTY_TEST_ID, property, property1, property2);
    renderNode->GetAnimationManager().AddAnimation(renderCurveAnimation);
    std::vector<std::pair<NodeId, AnimationId>> animations3;
    animations3.emplace_back(NODE_TEST_ID, ANIMATION_TEST_ID);
    animator->AddAnimations(animations3);
    GTEST_LOG_(INFO) << "RSRenderInteractiveAnimationTest AddAnimations001 end";
}

/**
 * @tc.name: AddAnimations002
 * @tc.desc: Verify the AddAnimations
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveAnimationTest, AddAnimations002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveAnimationTest AddAnimations002 start";
    auto context = std::make_shared<RSContext>();
    auto animatorUt = std::make_shared<RSRenderInteractiveImplictAnimator>(ANIMATOR_TEST_ID, context);
    EXPECT_TRUE(animator != nullptr);
    context = nullptr;
    std::vector<std::pair<NodeId, AnimationId>> animations1;
    animations1.emplace_back(0, 0);
    animatorUt->AddAnimations(animations1);
    GTEST_LOG_(INFO) << "RSRenderInteractiveAnimationTest AddAnimations002 end";
}

/**
 * @tc.name: AddAnimations003
 * @tc.desc: Cover branch: 1.node is nullptr 2.animation is nullptr
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveAnimationTest, AddAnimations003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveAnimationTest AddAnimations003 start";
    auto context = std::make_shared<RSContext>();
    EXPECT_TRUE(context != nullptr);
    auto animatorUt = std::make_shared<RSRenderInteractiveImplictAnimator>(ANIMATOR_TEST_ID, context);
    EXPECT_TRUE(animator != nullptr);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(NODE_TEST_ID);
    EXPECT_TRUE(renderNode != nullptr);
    context->GetMutableNodeMap().RegisterRenderNode(renderNode);

    std::vector<std::pair<NodeId, AnimationId>> animations;
    animations.emplace_back(INVALID_NODE_TEST_ID, INVALID_ANIMATION_TEST_ID);
    animations.emplace_back(NODE_TEST_ID, INVALID_ANIMATION_TEST_ID);
    animatorUt->AddAnimations(animations);
    GTEST_LOG_(INFO) << "RSRenderInteractiveAnimationTest AddAnimations003 end";
}

/**
 * @tc.name: PauseAnimator001
 * @tc.desc: Verify the PauseAnimator
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveAnimationTest, PauseAnimator001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveAnimationTest PauseAnimator001 start";
    RSContext context;
    auto animatorUt = std::make_shared<RSRenderInteractiveImplictAnimator>(ANIMATOR_TEST_ID, context.weak_from_this());
    EXPECT_TRUE(animatorUt != nullptr);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(NODE_TEST_ID);
    context.GetMutableNodeMap().RegisterRenderNode(renderNode);
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.4f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.3f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.67f);
    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_TEST_ID, PROPERTY_TEST_ID, property, property1, property2);
    renderNode->GetAnimationManager().AddAnimation(renderCurveAnimation);

    std::vector<std::pair<NodeId, AnimationId>> animations;
    animations.emplace_back(NODE_TEST_ID, ANIMATION_TEST_ID);
    animatorUt->AddAnimations(animations);
    animatorUt->PauseAnimator();
    GTEST_LOG_(INFO) << "RSRenderInteractiveAnimationTest PauseAnimator001 end";
}

/**
 * @tc.name: PauseAnimator002
 * @tc.desc: Verify the PauseAnimator
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveAnimationTest, PauseAnimator002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveAnimationTest PauseAnimator002 start";
    auto context = std::make_shared<RSContext>();
    auto animatorUt = std::make_shared<RSRenderInteractiveImplictAnimator>(ANIMATOR_TEST_ID, context);
    EXPECT_TRUE(animatorUt != nullptr);
    context = nullptr;
    animatorUt->PauseAnimator();
    GTEST_LOG_(INFO) << "RSRenderInteractiveAnimationTest PauseAnimator002 end";
}

/**
 * @tc.name: PauseAnimator003
 * @tc.desc: Cover branch: 1.node is nullptr 2.animation is nullptr
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveAnimationTest, PauseAnimator003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveAnimationTest PauseAnimator003 start";
    auto context = std::make_shared<RSContext>();
    EXPECT_TRUE(context != nullptr);
    auto animatorUt = std::make_shared<RSRenderInteractiveImplictAnimator>(ANIMATOR_TEST_ID, context);
    EXPECT_TRUE(animator != nullptr);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(NODE_TEST_ID);
    EXPECT_TRUE(renderNode != nullptr);
    context->GetMutableNodeMap().RegisterRenderNode(renderNode);

    animatorUt->animations_.emplace_back(INVALID_NODE_TEST_ID, INVALID_ANIMATION_TEST_ID);
    animatorUt->PauseAnimator();
    animatorUt->animations_.clear();
    animatorUt->animations_.emplace_back(NODE_TEST_ID, INVALID_ANIMATION_TEST_ID);
    animatorUt->PauseAnimator();
    GTEST_LOG_(INFO) << "RSRenderInteractiveAnimationTest PauseAnimator003 end";
}

/**
 * @tc.name: ContinueAnimator001
 * @tc.desc: Verify the ContinueAnimator
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveAnimationTest, ContinueAnimator001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveAnimationTest ContinueAnimator001 start";
    RSContext context;
    auto animatorUt = std::make_shared<RSRenderInteractiveImplictAnimator>(ANIMATOR_TEST_ID, context.weak_from_this());
    EXPECT_TRUE(animatorUt != nullptr);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(NODE_TEST_ID);
    context.GetMutableNodeMap().RegisterRenderNode(renderNode);
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.5f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.78f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.33f);
    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_TEST_ID, PROPERTY_TEST_ID, property, property1, property2);
    renderNode->GetAnimationManager().AddAnimation(renderCurveAnimation);

    std::vector<std::pair<NodeId, AnimationId>> animations;
    animations.emplace_back(NODE_TEST_ID, ANIMATION_TEST_ID);
    animatorUt->AddAnimations(animations);
    animatorUt->ContinueAnimator();
    GTEST_LOG_(INFO) << "RSRenderInteractiveAnimationTest ContinueAnimator001 end";
}

/**
 * @tc.name: ContinueAnimator002
 * @tc.desc: Verify the ContinueAnimator
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveAnimationTest, ContinueAnimator002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveAnimationTest ContinueAnimator002 start";
    auto context = std::make_shared<RSContext>();
    auto animatorUt = std::make_shared<RSRenderInteractiveImplictAnimator>(ANIMATOR_TEST_ID, context);
    EXPECT_TRUE(animatorUt != nullptr);
    context = nullptr;
    animatorUt->ContinueAnimator();
    GTEST_LOG_(INFO) << "RSRenderInteractiveAnimationTest ContinueAnimator002 end";
}

/**
 * @tc.name: ContinueAnimator003
 * @tc.desc: Cover branch: 1.node is nullptr 2.animation is nullptr
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveAnimationTest, ContinueAnimator003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveAnimationTest ContinueAnimator003 start";
    auto context = std::make_shared<RSContext>();
    EXPECT_TRUE(context != nullptr);
    auto animatorUt = std::make_shared<RSRenderInteractiveImplictAnimator>(ANIMATOR_TEST_ID, context);
    EXPECT_TRUE(animatorUt != nullptr);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(NODE_TEST_ID);
    EXPECT_TRUE(renderNode != nullptr);
    context->GetMutableNodeMap().RegisterRenderNode(renderNode);

    animatorUt->animations_.emplace_back(INVALID_NODE_TEST_ID, INVALID_ANIMATION_TEST_ID);
    animatorUt->ContinueAnimator();
    animatorUt->animations_.clear();
    animatorUt->animations_.emplace_back(NODE_TEST_ID, INVALID_ANIMATION_TEST_ID);
    animatorUt->ContinueAnimator();
    GTEST_LOG_(INFO) << "RSRenderInteractiveAnimationTest ContinueAnimator003 end";
}

/**
 * @tc.name: FinishAnimator001
 * @tc.desc: Verify the FinishAnimator
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveAnimationTest, FinishAnimator001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveAnimationTest FinishAnimator001 start";
    RSContext context;
    auto animatorUt = std::make_shared<RSRenderInteractiveImplictAnimator>(ANIMATOR_TEST_ID, context.weak_from_this());
    EXPECT_TRUE(animatorUt != nullptr);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(NODE_TEST_ID);
    context.GetMutableNodeMap().RegisterRenderNode(renderNode);
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.34f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.3f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.44f);
    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_TEST_ID, PROPERTY_TEST_ID, property, property1, property2);
    renderNode->GetAnimationManager().AddAnimation(renderCurveAnimation);

    std::vector<std::pair<NodeId, AnimationId>> animations;
    animations.emplace_back(NODE_TEST_ID, ANIMATION_TEST_ID);
    animatorUt->AddAnimations(animations);
    animatorUt->FinishAnimator(RSInteractiveAnimationPosition::CURRENT);
    GTEST_LOG_(INFO) << "RSRenderInteractiveAnimationTest FinishAnimator001 end";
}

/**
 * @tc.name: FinishAnimator002
 * @tc.desc: Verify the FinishAnimator
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveAnimationTest, FinishAnimator002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveAnimationTest FinishAnimator002 start";
    auto context = std::make_shared<RSContext>();
    auto animatorUt = std::make_shared<RSRenderInteractiveImplictAnimator>(ANIMATOR_TEST_ID, context);
    EXPECT_TRUE(animatorUt != nullptr);
    context = nullptr;
    animatorUt->FinishAnimator(RSInteractiveAnimationPosition::CURRENT);
    GTEST_LOG_(INFO) << "RSRenderInteractiveAnimationTest FinishAnimator002 end";
}

/**
 * @tc.name: FinishAnimator003
 * @tc.desc: Cover branch: 1.node is nullptr 2.animation is nullptr
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveAnimationTest, FinishAnimator003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveAnimationTest FinishAnimator003 start";
    auto context = std::make_shared<RSContext>();
    EXPECT_TRUE(context != nullptr);
    auto animatorUt = std::make_shared<RSRenderInteractiveImplictAnimator>(ANIMATOR_TEST_ID, context);
    EXPECT_TRUE(animatorUt != nullptr);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(NODE_TEST_ID);
    EXPECT_TRUE(renderNode != nullptr);
    context->GetMutableNodeMap().RegisterRenderNode(renderNode);

    animatorUt->animations_.emplace_back(INVALID_NODE_TEST_ID, INVALID_ANIMATION_TEST_ID);
    animatorUt->FinishAnimator(RSInteractiveAnimationPosition::CURRENT);
    animatorUt->animations_.clear();
    animatorUt->animations_.emplace_back(NODE_TEST_ID, INVALID_ANIMATION_TEST_ID);
    animatorUt->FinishAnimator(RSInteractiveAnimationPosition::CURRENT);
    GTEST_LOG_(INFO) << "RSRenderInteractiveAnimationTest FinishAnimator003 end";
}

/**
 * @tc.name: ReverseAnimator001
 * @tc.desc: Verify the ReverseAnimator
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveAnimationTest, ReverseAnimator001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveAnimationTest ReverseAnimator001 start";
    RSContext context;
    auto animatorUt = std::make_shared<RSRenderInteractiveImplictAnimator>(ANIMATOR_TEST_ID, context.weak_from_this());
    EXPECT_TRUE(animatorUt != nullptr);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(NODE_TEST_ID);
    context.GetMutableNodeMap().RegisterRenderNode(renderNode);
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.44f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.5f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.3434f);
    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_TEST_ID, PROPERTY_TEST_ID, property, property1, property2);
    renderNode->GetAnimationManager().AddAnimation(renderCurveAnimation);

    std::vector<std::pair<NodeId, AnimationId>> animations;
    animations.emplace_back(NODE_TEST_ID, ANIMATION_TEST_ID);
    animatorUt->AddAnimations(animations);
    animatorUt->ReverseAnimator();
    GTEST_LOG_(INFO) << "RSRenderInteractiveAnimationTest ReverseAnimator001 end";
}

/**
 * @tc.name: ReverseAnimator002
 * @tc.desc: Verify the ReverseAnimator
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveAnimationTest, ReverseAnimator002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveAnimationTest ReverseAnimator002 start";
    auto context = std::make_shared<RSContext>();
    auto animatorUt = std::make_shared<RSRenderInteractiveImplictAnimator>(ANIMATOR_TEST_ID, context);
    EXPECT_TRUE(animatorUt != nullptr);
    context = nullptr;
    animatorUt->ReverseAnimator();
    GTEST_LOG_(INFO) << "RSRenderInteractiveAnimationTest ReverseAnimator002 end";
}

/**
 * @tc.name: ReverseAnimator003
 * @tc.desc: Cover branch: 1.node is nullptr 2.animation is nullptr
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveAnimationTest, ReverseAnimator003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveAnimationTest ReverseAnimator003 start";
    auto context = std::make_shared<RSContext>();
    EXPECT_TRUE(context != nullptr);
    auto animatorUt = std::make_shared<RSRenderInteractiveImplictAnimator>(ANIMATOR_TEST_ID, context);
    EXPECT_TRUE(animatorUt != nullptr);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(NODE_TEST_ID);
    EXPECT_TRUE(renderNode != nullptr);
    context->GetMutableNodeMap().RegisterRenderNode(renderNode);

    animatorUt->animations_.emplace_back(INVALID_NODE_TEST_ID, INVALID_ANIMATION_TEST_ID);
    animatorUt->ReverseAnimator();
    animatorUt->animations_.clear();
    animatorUt->animations_.emplace_back(NODE_TEST_ID, INVALID_ANIMATION_TEST_ID);
    animatorUt->ReverseAnimator();
    GTEST_LOG_(INFO) << "RSRenderInteractiveAnimationTest ReverseAnimator003 end";
}

/**
 * @tc.name: SetFractionAnimator001
 * @tc.desc: Verify the SetFractionAnimator
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveAnimationTest, SetFractionAnimator001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveAnimationTest SetFractionAnimator001 start";
    RSContext context;
    auto animatorUt = std::make_shared<RSRenderInteractiveImplictAnimator>(ANIMATOR_TEST_ID, context.weak_from_this());
    EXPECT_TRUE(animatorUt != nullptr);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(NODE_TEST_ID);
    context.GetMutableNodeMap().RegisterRenderNode(renderNode);
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.33f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.4f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.55f);
    auto renderCurveAnimation = std::make_shared<RSRenderCurveAnimation>(
        ANIMATION_TEST_ID, PROPERTY_TEST_ID, property, property1, property2);
    renderNode->GetAnimationManager().AddAnimation(renderCurveAnimation);

    std::vector<std::pair<NodeId, AnimationId>> animations;
    animations.emplace_back(NODE_TEST_ID, ANIMATION_TEST_ID);
    animatorUt->AddAnimations(animations);
    animatorUt->SetFractionAnimator(1.0f);
    GTEST_LOG_(INFO) << "RSRenderInteractiveAnimationTest SetFractionAnimator001 end";
}

/**
 * @tc.name: SetFractionAnimator002
 * @tc.desc: Verify the SetFractionAnimator
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveAnimationTest, SetFractionAnimator002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveAnimationTest SetFractionAnimator002 start";
    auto context = std::make_shared<RSContext>();
    auto animatorUt = std::make_shared<RSRenderInteractiveImplictAnimator>(ANIMATOR_TEST_ID, context);
    EXPECT_TRUE(animatorUt != nullptr);
    context = nullptr;
    animatorUt->SetFractionAnimator(1.0f);
    GTEST_LOG_(INFO) << "RSRenderInteractiveAnimationTest SetFractionAnimator002 end";
}

/**
 * @tc.name: SetFractionAnimator003
 * @tc.desc: Cover branch: 1.node is nullptr 2.animation is nullptr
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderInteractiveAnimationTest, SetFractionAnimator003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderInteractiveAnimationTest SetFractionAnimator003 start";
    auto context = std::make_shared<RSContext>();
    EXPECT_TRUE(context != nullptr);
    auto animatorUt = std::make_shared<RSRenderInteractiveImplictAnimator>(ANIMATOR_TEST_ID, context);
    EXPECT_TRUE(animatorUt != nullptr);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(NODE_TEST_ID);
    EXPECT_TRUE(renderNode != nullptr);
    context->GetMutableNodeMap().RegisterRenderNode(renderNode);

    animatorUt->animations_.emplace_back(INVALID_NODE_TEST_ID, INVALID_ANIMATION_TEST_ID);
    animatorUt->SetFractionAnimator(1.0f);
    animatorUt->animations_.clear();
    animatorUt->animations_.emplace_back(NODE_TEST_ID, INVALID_ANIMATION_TEST_ID);
    animatorUt->SetFractionAnimator(1.0f);
    GTEST_LOG_(INFO) << "RSRenderInteractiveAnimationTest SetFractionAnimator003 end";
}
} // namespace Rosen
} // namespace OHOS
