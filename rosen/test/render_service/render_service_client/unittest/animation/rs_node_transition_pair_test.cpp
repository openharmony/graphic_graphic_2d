/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include <sys/types.h>
#include <unistd.h>

#include "gtest/gtest.h"

#include "animation/rs_animation.h"
#include "animation/rs_animation_timing_curve.h"
#include "animation/rs_animation_timing_protocol.h"
#include "animation/rs_render_animation.h"
#include "common/rs_vector2.h"
#include "modifier/rs_property.h"
#include "pipeline/rs_node_map_v2.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_ui_context.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class RSRenderAnimationMock : public RSRenderAnimation {
public:
    RSRenderAnimationMock() : RSRenderAnimation() {}
    explicit RSRenderAnimationMock(AnimationId id) : RSRenderAnimation(id) {}
    ~RSRenderAnimationMock() override = default;
    void RebuildPropertyValue(float fraction) override {}
};

class RSNodeTransitionPairTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    std::shared_ptr<RSUIContext> CreateRSUIContext();
};

void RSNodeTransitionPairTest::SetUpTestCase() {}
void RSNodeTransitionPairTest::TearDownTestCase() {}
void RSNodeTransitionPairTest::SetUp() {}
void RSNodeTransitionPairTest::TearDown() {}

std::shared_ptr<RSUIContext> RSNodeTransitionPairTest::CreateRSUIContext()
{
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto rsUIContext = std::make_shared<RSUIContext>(0, connectToRenderRemote);
    rsUIContext->SetUITaskRunner([](const std::function<void()>& task, uint32_t delay) { task(); });
    return rsUIContext;
}

/**
 * @tc.name: RegisterTransitionPair_StoreTransitionPairInfo001
 * @tc.desc: Verify RegisterTransitionPair stores transitionPairInfo_ on both inNode and outNode
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeTransitionPairTest, RegisterTransitionPair_StoreTransitionPairInfo001, TestSize.Level1)
{
    auto rsUIContext = CreateRSUIContext();
    auto inNode = RSCanvasNode::Create();
    auto outNode = RSCanvasNode::Create();
    rsUIContext->GetMutableNodeMap().RegisterNode(inNode);
    rsUIContext->GetMutableNodeMap().RegisterNode(outNode);

    NodeId inNodeId = inNode->GetId();
    NodeId outNodeId = outNode->GetId();
    RSNode::RegisterTransitionPair(rsUIContext, inNodeId, outNodeId, true);

    ASSERT_TRUE(inNode->transitionPairInfo_.has_value());
    EXPECT_EQ(inNode->transitionPairInfo_->inNodeId, inNodeId);
    EXPECT_EQ(inNode->transitionPairInfo_->outNodeId, outNodeId);
    EXPECT_TRUE(inNode->transitionPairInfo_->isInSameWindow);

    ASSERT_TRUE(outNode->transitionPairInfo_.has_value());
    EXPECT_EQ(outNode->transitionPairInfo_->inNodeId, inNodeId);
    EXPECT_EQ(outNode->transitionPairInfo_->outNodeId, outNodeId);
    EXPECT_TRUE(outNode->transitionPairInfo_->isInSameWindow);
}

/**
 * @tc.name: RegisterTransitionPair_StoreTransitionPairInfo002
 * @tc.desc: Verify RegisterTransitionPair with isInSameWindow=false stores correctly
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeTransitionPairTest, RegisterTransitionPair_StoreTransitionPairInfo002, TestSize.Level1)
{
    auto rsUIContext = CreateRSUIContext();
    auto inNode = RSCanvasNode::Create();
    auto outNode = RSCanvasNode::Create();
    rsUIContext->GetMutableNodeMap().RegisterNode(inNode);
    rsUIContext->GetMutableNodeMap().RegisterNode(outNode);

    NodeId inNodeId = inNode->GetId();
    NodeId outNodeId = outNode->GetId();
    RSNode::RegisterTransitionPair(rsUIContext, inNodeId, outNodeId, false);

    ASSERT_TRUE(inNode->transitionPairInfo_.has_value());
    EXPECT_FALSE(inNode->transitionPairInfo_->isInSameWindow);

    ASSERT_TRUE(outNode->transitionPairInfo_.has_value());
    EXPECT_FALSE(outNode->transitionPairInfo_->isInSameWindow);
}

/**
 * @tc.name: RegisterTransitionPair_StoreTransitionPairInfo003
 * @tc.desc: Verify RegisterTransitionPair does not crash when nodes not in nodeMap
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeTransitionPairTest, RegisterTransitionPair_StoreTransitionPairInfo003, TestSize.Level1)
{
    auto rsUIContext = CreateRSUIContext();
    NodeId inNodeId = 100;
    NodeId outNodeId = 200;

    RSNode::RegisterTransitionPair(rsUIContext, inNodeId, outNodeId, true);
    EXPECT_NE(rsUIContext->GetRSTransaction(), nullptr);
}

/**
 * @tc.name: RegisterTransitionPair_StoreTransitionPairInfo004
 * @tc.desc: Verify RegisterTransitionPair with null RSUIContext does not crash
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeTransitionPairTest, RegisterTransitionPair_StoreTransitionPairInfo004, TestSize.Level1)
{
    auto inNode = RSCanvasNode::Create();
    NodeId inNodeId = inNode->GetId();
    NodeId outNodeId = 999;

    RSNode::RegisterTransitionPair(nullptr, inNodeId, outNodeId, true);
    EXPECT_FALSE(inNode->transitionPairInfo_.has_value());
}

/**
 * @tc.name: RegisterTransitionPair_StoreTransitionPairInfo005
 * @tc.desc: Verify RegisterTransitionPair only stores on inNode when outNode not in nodeMap
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeTransitionPairTest, RegisterTransitionPair_StoreTransitionPairInfo005, TestSize.Level1)
{
    auto rsUIContext = CreateRSUIContext();
    auto inNode = RSCanvasNode::Create();
    rsUIContext->GetMutableNodeMap().RegisterNode(inNode);
    NodeId outNodeId = 99999;

    RSNode::RegisterTransitionPair(rsUIContext, inNode->GetId(), outNodeId, true);

    ASSERT_TRUE(inNode->transitionPairInfo_.has_value());
    EXPECT_EQ(inNode->transitionPairInfo_->outNodeId, outNodeId);
}

/**
 * @tc.name: UnregisterTransitionPair_ClearTransitionPairInfo001
 * @tc.desc: Verify UnregisterTransitionPair clears transitionPairInfo_ on both nodes
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeTransitionPairTest, UnregisterTransitionPair_ClearTransitionPairInfo001, TestSize.Level1)
{
    auto rsUIContext = CreateRSUIContext();
    auto inNode = RSCanvasNode::Create();
    auto outNode = RSCanvasNode::Create();
    rsUIContext->GetMutableNodeMap().RegisterNode(inNode);
    rsUIContext->GetMutableNodeMap().RegisterNode(outNode);

    NodeId inNodeId = inNode->GetId();
    NodeId outNodeId = outNode->GetId();
    RSNode::RegisterTransitionPair(rsUIContext, inNodeId, outNodeId, true);
    ASSERT_TRUE(inNode->transitionPairInfo_.has_value());
    ASSERT_TRUE(outNode->transitionPairInfo_.has_value());

    RSNode::UnregisterTransitionPair(rsUIContext, inNodeId, outNodeId);
    EXPECT_FALSE(inNode->transitionPairInfo_.has_value());
    EXPECT_FALSE(outNode->transitionPairInfo_.has_value());
}

/**
 * @tc.name: UnregisterTransitionPair_ClearTransitionPairInfo002
 * @tc.desc: Verify UnregisterTransitionPair with null RSUIContext does not crash
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeTransitionPairTest, UnregisterTransitionPair_ClearTransitionPairInfo002, TestSize.Level1)
{
    auto rsUIContext = CreateRSUIContext();
    auto inNode = RSCanvasNode::Create();
    auto outNode = RSCanvasNode::Create();
    rsUIContext->GetMutableNodeMap().RegisterNode(inNode);
    rsUIContext->GetMutableNodeMap().RegisterNode(outNode);

    RSNode::RegisterTransitionPair(rsUIContext, inNode->GetId(), outNode->GetId(), true);

    RSNode::UnregisterTransitionPair(nullptr, inNode->GetId(), outNode->GetId());
    EXPECT_TRUE(inNode->transitionPairInfo_.has_value());
    EXPECT_TRUE(outNode->transitionPairInfo_.has_value());
}

/**
 * @tc.name: ResetTransitionPairInfoBoth001
 * @tc.desc: Verify ResetTransitionPairInfoBoth clears both nodes' transitionPairInfo_
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeTransitionPairTest, ResetTransitionPairInfoBoth001, TestSize.Level1)
{
    auto rsUIContext = CreateRSUIContext();
    auto inNode = RSCanvasNode::Create();
    auto outNode = RSCanvasNode::Create();
    rsUIContext->GetMutableNodeMap().RegisterNode(inNode);
    rsUIContext->GetMutableNodeMap().RegisterNode(outNode);
    inNode->rsUIContext_ = rsUIContext;
    outNode->rsUIContext_ = rsUIContext;

    RSNode::RegisterTransitionPair(rsUIContext, inNode->GetId(), outNode->GetId(), true);
    ASSERT_TRUE(inNode->transitionPairInfo_.has_value());
    ASSERT_TRUE(outNode->transitionPairInfo_.has_value());

    inNode->ResetTransitionPairInfoBoth();
    EXPECT_FALSE(inNode->transitionPairInfo_.has_value());
    EXPECT_FALSE(outNode->transitionPairInfo_.has_value());
}

/**
 * @tc.name: ResetTransitionPairInfoBoth002
 * @tc.desc: Verify ResetTransitionPairInfoBoth from outNode clears both nodes
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeTransitionPairTest, ResetTransitionPairInfoBoth002, TestSize.Level1)
{
    auto rsUIContext = CreateRSUIContext();
    auto inNode = RSCanvasNode::Create();
    auto outNode = RSCanvasNode::Create();
    rsUIContext->GetMutableNodeMap().RegisterNode(inNode);
    rsUIContext->GetMutableNodeMap().RegisterNode(outNode);
    inNode->rsUIContext_ = rsUIContext;
    outNode->rsUIContext_ = rsUIContext;

    RSNode::RegisterTransitionPair(rsUIContext, inNode->GetId(), outNode->GetId(), true);

    outNode->ResetTransitionPairInfoBoth();
    EXPECT_FALSE(inNode->transitionPairInfo_.has_value());
    EXPECT_FALSE(outNode->transitionPairInfo_.has_value());
}

/**
 * @tc.name: ResetTransitionPairInfoBoth003
 * @tc.desc: Verify ResetTransitionPairInfoBoth when transitionPairInfo_ is empty
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeTransitionPairTest, ResetTransitionPairInfoBoth003, TestSize.Level1)
{
    auto rsUIContext = CreateRSUIContext();
    auto node = RSCanvasNode::Create();
    rsUIContext->GetMutableNodeMap().RegisterNode(node);
    node->rsUIContext_ = rsUIContext;

    EXPECT_FALSE(node->transitionPairInfo_.has_value());
    node->ResetTransitionPairInfoBoth();
    EXPECT_FALSE(node->transitionPairInfo_.has_value());
}

/**
 * @tc.name: ResetTransitionPairInfoBoth004
 * @tc.desc: Verify ResetTransitionPairInfoBoth when other node not in nodeMap
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeTransitionPairTest, ResetTransitionPairInfoBoth004, TestSize.Level1)
{
    auto rsUIContext = CreateRSUIContext();
    auto inNode = RSCanvasNode::Create();
    rsUIContext->GetMutableNodeMap().RegisterNode(inNode);
    inNode->rsUIContext_ = rsUIContext;

    inNode->transitionPairInfo_ = RSNode::TransitionPairInfo{ inNode->GetId(), 99999, true };
    ASSERT_TRUE(inNode->transitionPairInfo_.has_value());

    inNode->ResetTransitionPairInfoBoth();
    EXPECT_FALSE(inNode->transitionPairInfo_.has_value());
}

/**
 * @tc.name: ResetTransitionPairInfoBoth005
 * @tc.desc: Verify ResetTransitionPairInfoBoth when rsUIContext is null
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeTransitionPairTest, ResetTransitionPairInfoBoth005, TestSize.Level1)
{
    auto inNode = RSCanvasNode::Create();
    inNode->transitionPairInfo_ = RSNode::TransitionPairInfo{ 1, 2, true };
    ASSERT_TRUE(inNode->transitionPairInfo_.has_value());

    inNode->ResetTransitionPairInfoBoth();
    EXPECT_FALSE(inNode->transitionPairInfo_.has_value());
}

/**
 * @tc.name: SetSandBox_ClearTransitionPairInfo001
 * @tc.desc: Verify SetSandBox with nullopt clears transitionPairInfo_ on both nodes
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeTransitionPairTest, SetSandBox_ClearTransitionPairInfo001, TestSize.Level1)
{
    auto rsUIContext = CreateRSUIContext();
    auto inNode = RSCanvasNode::Create();
    auto outNode = RSCanvasNode::Create();
    rsUIContext->GetMutableNodeMap().RegisterNode(inNode);
    rsUIContext->GetMutableNodeMap().RegisterNode(outNode);
    inNode->rsUIContext_ = rsUIContext;
    outNode->rsUIContext_ = rsUIContext;

    RSNode::RegisterTransitionPair(rsUIContext, inNode->GetId(), outNode->GetId(), true);
    ASSERT_TRUE(inNode->transitionPairInfo_.has_value());
    ASSERT_TRUE(outNode->transitionPairInfo_.has_value());

    inNode->SetSandBox(std::nullopt);
    EXPECT_FALSE(inNode->transitionPairInfo_.has_value());
    EXPECT_FALSE(outNode->transitionPairInfo_.has_value());
}

/**
 * @tc.name: SetSandBox_ClearTransitionPairInfo002
 * @tc.desc: Verify SetSandBox with value does not clear transitionPairInfo_
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeTransitionPairTest, SetSandBox_ClearTransitionPairInfo002, TestSize.Level1)
{
    auto rsUIContext = CreateRSUIContext();
    auto inNode = RSCanvasNode::Create();
    auto outNode = RSCanvasNode::Create();
    rsUIContext->GetMutableNodeMap().RegisterNode(inNode);
    rsUIContext->GetMutableNodeMap().RegisterNode(outNode);
    inNode->rsUIContext_ = rsUIContext;

    RSNode::RegisterTransitionPair(rsUIContext, inNode->GetId(), outNode->GetId(), true);
    ASSERT_TRUE(inNode->transitionPairInfo_.has_value());

    inNode->SetSandBox(Vector2f(10.0f, 20.0f));
    EXPECT_TRUE(inNode->transitionPairInfo_.has_value());
}

/**
 * @tc.name: RebuildTransitionPairInRender001
 * @tc.desc: Verify RebuildTransitionPairInRender skips when transitionPairInfo_ is empty
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeTransitionPairTest, RebuildTransitionPairInRender001, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    EXPECT_FALSE(node->transitionPairInfo_.has_value());

    node->RebuildTransitionPairInRender();
    EXPECT_FALSE(node->transitionPairInfo_.has_value());
}

/**
 * @tc.name: RebuildTransitionPairInRender002
 * @tc.desc: Verify RebuildTransitionPairInRender re-registers pair and preserves transitionPairInfo_
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeTransitionPairTest, RebuildTransitionPairInRender002, TestSize.Level1)
{
    auto rsUIContext = CreateRSUIContext();
    auto inNode = RSCanvasNode::Create();
    auto outNode = RSCanvasNode::Create();
    rsUIContext->GetMutableNodeMap().RegisterNode(inNode);
    rsUIContext->GetMutableNodeMap().RegisterNode(outNode);
    inNode->rsUIContext_ = rsUIContext;

    RSNode::RegisterTransitionPair(rsUIContext, inNode->GetId(), outNode->GetId(), true);
    ASSERT_TRUE(inNode->transitionPairInfo_.has_value());

    inNode->RebuildTransitionPairInRender();
    EXPECT_TRUE(inNode->transitionPairInfo_.has_value());
    ASSERT_NE(rsUIContext->GetRSTransaction(), nullptr);
}

/**
 * @tc.name: RebuildAnimationInRender_WithTransitionPair001
 * @tc.desc: Verify RebuildAnimationInRender calls RebuildTransitionPairInRender for infinite animation
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeTransitionPairTest, RebuildAnimationInRender_WithTransitionPair001, TestSize.Level1)
{
    auto rsUIContext = CreateRSUIContext();
    auto inNode = RSCanvasNode::Create();
    auto outNode = RSCanvasNode::Create();
    rsUIContext->GetMutableNodeMap().RegisterNode(inNode);
    rsUIContext->GetMutableNodeMap().RegisterNode(outNode);
    inNode->rsUIContext_ = rsUIContext;

    RSNode::RegisterTransitionPair(rsUIContext, inNode->GetId(), outNode->GetId(), true);
    ASSERT_TRUE(inNode->transitionPairInfo_.has_value());

    AnimationId id = 1;
    auto animation = std::make_shared<RSDummyAnimation>(rsUIContext);
    animation->SetRepeatCount(-1);
    inNode->animations_.insert({ id, animation });

    inNode->RebuildAnimationInRender();
    EXPECT_TRUE(inNode->transitionPairInfo_.has_value());
    ASSERT_NE(rsUIContext->GetRSTransaction(), nullptr);
}

/**
 * @tc.name: RebuildAnimationInRender_WithTransitionPair002
 * @tc.desc: Verify RebuildAnimationInRender skips RebuildTransitionPairInRender for finite animation
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeTransitionPairTest, RebuildAnimationInRender_WithTransitionPair002, TestSize.Level1)
{
    auto rsUIContext = CreateRSUIContext();
    auto inNode = RSCanvasNode::Create();
    rsUIContext->GetMutableNodeMap().RegisterNode(inNode);
    inNode->rsUIContext_ = rsUIContext;

    inNode->transitionPairInfo_ = RSNode::TransitionPairInfo{ inNode->GetId(), 99999, true };
    ASSERT_TRUE(inNode->transitionPairInfo_.has_value());

    AnimationId id = 1;
    auto animation = std::make_shared<RSDummyAnimation>(rsUIContext);
    animation->SetRepeatCount(1);
    inNode->animations_.insert({ id, animation });

    inNode->RebuildAnimationInRender();
    EXPECT_TRUE(inNode->transitionPairInfo_.has_value());
}

/**
 * @tc.name: RebuildAnimationInRender_WithTransitionPair003
 * @tc.desc: Verify RebuildAnimationInRender skips RebuildTransitionPairInRender for UI animation
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeTransitionPairTest, RebuildAnimationInRender_WithTransitionPair003, TestSize.Level1)
{
    auto rsUIContext = CreateRSUIContext();
    auto inNode = RSCanvasNode::Create();
    rsUIContext->GetMutableNodeMap().RegisterNode(inNode);
    inNode->rsUIContext_ = rsUIContext;

    inNode->transitionPairInfo_ = RSNode::TransitionPairInfo{ inNode->GetId(), 99999, true };
    ASSERT_TRUE(inNode->transitionPairInfo_.has_value());

    AnimationId id = 1;
    auto animation = std::make_shared<RSDummyAnimation>(rsUIContext);
    animation->SetRepeatCount(-1);
    animation->uiAnimation_ = std::make_shared<RSRenderAnimationMock>();
    inNode->animations_.insert({ id, animation });

    inNode->RebuildAnimationInRender();
    EXPECT_TRUE(inNode->transitionPairInfo_.has_value());
}

/**
 * @tc.name: RebuildAnimationInRender_WithTransitionPair004
 * @tc.desc: Verify RebuildAnimationInRender skips RebuildTransitionPairInRender with no transitionPairInfo
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeTransitionPairTest, RebuildAnimationInRender_WithTransitionPair004, TestSize.Level1)
{
    auto rsUIContext = CreateRSUIContext();
    auto inNode = RSCanvasNode::Create();
    inNode->rsUIContext_ = rsUIContext;

    AnimationId id = 1;
    auto animation = std::make_shared<RSDummyAnimation>(rsUIContext);
    animation->SetRepeatCount(-1);
    inNode->animations_.insert({ id, animation });

    inNode->RebuildAnimationInRender();
    EXPECT_FALSE(inNode->transitionPairInfo_.has_value());
}

/**
 * @tc.name: RebuildAnimationInRender_WithTransitionPair005
 * @tc.desc: Verify RebuildAnimationInRender with empty animations does not call RebuildTransitionPairInRender
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeTransitionPairTest, RebuildAnimationInRender_WithTransitionPair005, TestSize.Level1)
{
    auto rsUIContext = CreateRSUIContext();
    auto inNode = RSCanvasNode::Create();
    inNode->rsUIContext_ = rsUIContext;

    inNode->transitionPairInfo_ = RSNode::TransitionPairInfo{ inNode->GetId(), 99999, true };
    ASSERT_TRUE(inNode->transitionPairInfo_.has_value());
    inNode->animations_.clear();

    inNode->RebuildAnimationInRender();
    EXPECT_TRUE(inNode->transitionPairInfo_.has_value());
}

/**
 * @tc.name: TransitionPairInfo_BothNodesRebuild001
 * @tc.desc: Verify both nodes can rebuild transition pair when both have transitionPairInfo_
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeTransitionPairTest, TransitionPairInfo_BothNodesRebuild001, TestSize.Level1)
{
    auto rsUIContext = CreateRSUIContext();
    auto inNode = RSCanvasNode::Create();
    auto outNode = RSCanvasNode::Create();
    rsUIContext->GetMutableNodeMap().RegisterNode(inNode);
    rsUIContext->GetMutableNodeMap().RegisterNode(outNode);
    inNode->rsUIContext_ = rsUIContext;
    outNode->rsUIContext_ = rsUIContext;

    RSNode::RegisterTransitionPair(rsUIContext, inNode->GetId(), outNode->GetId(), true);
    ASSERT_TRUE(inNode->transitionPairInfo_.has_value());
    ASSERT_TRUE(outNode->transitionPairInfo_.has_value());

    inNode->RebuildTransitionPairInRender();
    EXPECT_TRUE(inNode->transitionPairInfo_.has_value());

    outNode->RebuildTransitionPairInRender();
    EXPECT_TRUE(outNode->transitionPairInfo_.has_value());
}

/**
 * @tc.name: TransitionPairInfo_BothNodesRebuild002
 * @tc.desc: Verify transitionPairInfo_ persists after multiple RebuildTransitionPairInRender calls
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeTransitionPairTest, TransitionPairInfo_BothNodesRebuild002, TestSize.Level1)
{
    auto rsUIContext = CreateRSUIContext();
    auto inNode = RSCanvasNode::Create();
    auto outNode = RSCanvasNode::Create();
    rsUIContext->GetMutableNodeMap().RegisterNode(inNode);
    rsUIContext->GetMutableNodeMap().RegisterNode(outNode);
    inNode->rsUIContext_ = rsUIContext;

    RSNode::RegisterTransitionPair(rsUIContext, inNode->GetId(), outNode->GetId(), false);

    inNode->RebuildTransitionPairInRender();
    EXPECT_TRUE(inNode->transitionPairInfo_.has_value());
    EXPECT_EQ(inNode->transitionPairInfo_->inNodeId, inNode->GetId());

    inNode->RebuildTransitionPairInRender();
    EXPECT_TRUE(inNode->transitionPairInfo_.has_value());
    EXPECT_EQ(inNode->transitionPairInfo_->inNodeId, inNode->GetId());
}

} // namespace Rosen
