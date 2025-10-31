/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include "feature/window_keyframe/rs_window_keyframe_render_node.h"
#include "drawable/rs_render_node_drawable_adapter.h"
#include "params/rs_render_params.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_draw_cmd.h"
#include "render_thread/rs_render_thread_visitor.h"
#include "platform/common/rs_log.h"
#include "property/rs_properties_painter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class MockNodeDrawable : public DrawableV2::RSRenderNodeDrawableAdapter {
public:
    explicit MockNodeDrawable(std::shared_ptr<const RSRenderNode> node)
    : RSRenderNodeDrawableAdapter(std::move(node)) {}
    void Draw(Drawing::Canvas& canvas) override {}
};

class RSWindowKeyFrameRenderNodeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

private:
    std::shared_ptr<RSWindowKeyFrameRenderNode> keyFrameRenderNode_;

    constexpr static NodeId DEFAULT_KEYFRAMENODE_ID = 1;
    constexpr static NodeId DEFAULT_LINKEDNODE_ID = DEFAULT_KEYFRAMENODE_ID + 1;
};

void RSWindowKeyFrameRenderNodeTest::SetUpTestCase() {}
void RSWindowKeyFrameRenderNodeTest::TearDownTestCase() {}
void RSWindowKeyFrameRenderNodeTest::SetUp()
{
    keyFrameRenderNode_ = std::make_shared<RSWindowKeyFrameRenderNode>(DEFAULT_KEYFRAMENODE_ID);
    ASSERT_NE(keyFrameRenderNode_, nullptr);
}

void RSWindowKeyFrameRenderNodeTest::TearDown()
{
    keyFrameRenderNode_.reset();
    RSWindowKeyFrameRenderNode::ClearLinkedNodeInfo();
}

/**
 * @tc.name: OnTreeStateChanged
 * @tc.desc: test OnTreeStateChanged
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSWindowKeyFrameRenderNodeTest, OnTreeStateChanged, TestSize.Level2)
{
    keyFrameRenderNode_->isOnTheTree_ = false;
    keyFrameRenderNode_->OnTreeStateChanged();
    EXPECT_FALSE(keyFrameRenderNode_->IsOnTheTree());

    keyFrameRenderNode_->isOnTheTree_ = true;
    keyFrameRenderNode_->OnTreeStateChanged();
    EXPECT_TRUE(keyFrameRenderNode_->IsOnTheTree());
}

/**
 * @tc.name: QuickPrepare001
 * @tc.desc: test results of QuickPrepare
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSWindowKeyFrameRenderNodeTest, QuickPrepare001, TestSize.Level1)
{
    std::shared_ptr<RSNodeVisitor> visitor;
    ASSERT_EQ(visitor, nullptr);
    keyFrameRenderNode_->QuickPrepare(visitor);

    visitor = std::make_shared<RSRenderThreadVisitor>();
    ASSERT_NE(visitor, nullptr);
    keyFrameRenderNode_->QuickPrepare(visitor);

    EXPECT_EQ(keyFrameRenderNode_->GetType(), RSRenderNodeType::WINDOW_KEYFRAME_NODE);
}

/**
 * @tc.name: SetLinkedNodeId
 * @tc.desc: test SetLinkedNodeId
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSWindowKeyFrameRenderNodeTest, SetLinkedNodeId, TestSize.Level1)
{
    EXPECT_EQ(keyFrameRenderNode_->GetLinkedNodeId(), INVALID_NODEID);
    keyFrameRenderNode_->SetLinkedNodeId(DEFAULT_LINKEDNODE_ID);
    EXPECT_EQ(keyFrameRenderNode_->GetLinkedNodeId(), DEFAULT_LINKEDNODE_ID);
}

/**
 * @tc.name: CollectLinkedNodeInfo
 * @tc.desc: test CollectLinkedNodeInfo
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSWindowKeyFrameRenderNodeTest, CollectLinkedNodeInfo, TestSize.Level1)
{
    keyFrameRenderNode_->SetLinkedNodeId(DEFAULT_LINKEDNODE_ID);
    keyFrameRenderNode_->CollectLinkedNodeInfo();
    EXPECT_EQ(keyFrameRenderNode_->GetLinkedNodeCount(), 1);

    keyFrameRenderNode_->ClearLinkedNodeInfo();
    EXPECT_EQ(keyFrameRenderNode_->GetLinkedNodeCount(), 0);
}

/**
 * @tc.name: ResetLinkedWindowKeyFrameInfo
 * @tc.desc: test ResetLinkedWindowKeyFrameInfo
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSWindowKeyFrameRenderNodeTest, ResetLinkedWindowKeyFrameInfo, TestSize.Level1)
{
    auto linkedNode = std::make_shared<RSRootRenderNode>(DEFAULT_LINKEDNODE_ID);
    ASSERT_NE(linkedNode, nullptr);
    keyFrameRenderNode_->ResetLinkedWindowKeyFrameInfo(*linkedNode);

    linkedNode->stagingRenderParams_ = std::make_unique<RSRenderParams>(DEFAULT_LINKEDNODE_ID);
    ASSERT_NE(linkedNode->stagingRenderParams_, nullptr);

    keyFrameRenderNode_->ResetLinkedWindowKeyFrameInfo(*linkedNode);
    EXPECT_TRUE(linkedNode->stagingRenderParams_->GetWindowKeyFrameNodeDrawable().expired());
}

/**
 * @tc.name: PrepareLinkedNodeOffscreen
 * @tc.desc: test PrepareLinkedNodeOffscreen
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSWindowKeyFrameRenderNodeTest, PrepareLinkedNodeOffscreen, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);

    NodeId appNodeId = 10;
    auto appNode = std::make_shared<RSSurfaceRenderNode>(appNodeId, rsContext->weak_from_this());
    ASSERT_NE(appNode, nullptr);
    appNode->InitRenderParams();
    EXPECT_FALSE(RSWindowKeyFrameRenderNode::PrepareLinkedNodeOffscreen(*appNode, *rsContext));

    auto rootNode = std::make_shared<RSRootRenderNode>(DEFAULT_LINKEDNODE_ID, rsContext->weak_from_this());
    ASSERT_NE(rootNode, nullptr);
    keyFrameRenderNode_->SetLinkedNodeId(DEFAULT_LINKEDNODE_ID);
    keyFrameRenderNode_->CollectLinkedNodeInfo();
    EXPECT_FALSE(RSWindowKeyFrameRenderNode::PrepareLinkedNodeOffscreen(*appNode, *rsContext));

    auto& nodeMap = rsContext->GetMutableNodeMap();
    nodeMap.RegisterRenderNode(rootNode);
    EXPECT_FALSE(RSWindowKeyFrameRenderNode::PrepareLinkedNodeOffscreen(*appNode, *rsContext));

    nodeMap.RegisterRenderNode(keyFrameRenderNode_);
    EXPECT_FALSE(RSWindowKeyFrameRenderNode::PrepareLinkedNodeOffscreen(*appNode, *rsContext));

    auto otherAppNode = std::make_shared<RSSurfaceRenderNode>(appNodeId + 1, rsContext->weak_from_this());
    ASSERT_NE(appNode, nullptr);
    rootNode->parent_ = otherAppNode;
    EXPECT_FALSE(RSWindowKeyFrameRenderNode::PrepareLinkedNodeOffscreen(*appNode, *rsContext));

    rootNode->parent_ = appNode;
    EXPECT_FALSE(RSWindowKeyFrameRenderNode::PrepareLinkedNodeOffscreen(*appNode, *rsContext));

    keyFrameRenderNode_->stagingRenderParams_.reset();
    rootNode->stagingRenderParams_.reset();
    keyFrameRenderNode_->renderDrawable_ = std::make_shared<MockNodeDrawable>(keyFrameRenderNode_);
    ASSERT_NE(keyFrameRenderNode_->renderDrawable_, nullptr);
    EXPECT_FALSE(RSWindowKeyFrameRenderNode::PrepareLinkedNodeOffscreen(*appNode, *rsContext));

    keyFrameRenderNode_->stagingRenderParams_ = std::make_unique<RSRenderParams>(DEFAULT_KEYFRAMENODE_ID);
    ASSERT_NE(keyFrameRenderNode_->stagingRenderParams_, nullptr);
    EXPECT_FALSE(RSWindowKeyFrameRenderNode::PrepareLinkedNodeOffscreen(*appNode, *rsContext));

    rootNode->stagingRenderParams_ = std::make_unique<RSRenderParams>(DEFAULT_LINKEDNODE_ID);
    ASSERT_NE(rootNode->stagingRenderParams_, nullptr);
    keyFrameRenderNode_->addedToPendingSyncList_ = true;
    rootNode->addedToPendingSyncList_ = true;
    EXPECT_TRUE(RSWindowKeyFrameRenderNode::PrepareLinkedNodeOffscreen(*appNode, *rsContext));
}

/**
 * @tc.name: UpdateKeyFrameBehindWindowRegion
 * @tc.desc: test UpdateKeyFrameBehindWindowRegion
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSWindowKeyFrameRenderNodeTest, UpdateKeyFrameBehindWindowRegion, TestSize.Level1)
{
    RectI behindWndRect(0, 0, 1, 1);
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);

    NodeId appNodeId = 10;
    auto appNode = std::make_shared<RSSurfaceRenderNode>(appNodeId, rsContext->weak_from_this());
    ASSERT_NE(appNode, nullptr);
    appNode->InitRenderParams();
    EXPECT_FALSE(RSWindowKeyFrameRenderNode::UpdateKeyFrameBehindWindowRegion(*appNode, *rsContext, behindWndRect));

    auto rootNode = std::make_shared<RSRootRenderNode>(DEFAULT_LINKEDNODE_ID, rsContext->weak_from_this());
    ASSERT_NE(rootNode, nullptr);
    keyFrameRenderNode_->SetLinkedNodeId(DEFAULT_LINKEDNODE_ID);
    keyFrameRenderNode_->CollectLinkedNodeInfo();
    EXPECT_FALSE(RSWindowKeyFrameRenderNode::UpdateKeyFrameBehindWindowRegion(*appNode, *rsContext, behindWndRect));

    auto& nodeMap = rsContext->GetMutableNodeMap();
    nodeMap.RegisterRenderNode(rootNode);
    EXPECT_FALSE(RSWindowKeyFrameRenderNode::UpdateKeyFrameBehindWindowRegion(*appNode, *rsContext, behindWndRect));

    nodeMap.RegisterRenderNode(keyFrameRenderNode_);
    EXPECT_FALSE(RSWindowKeyFrameRenderNode::UpdateKeyFrameBehindWindowRegion(*appNode, *rsContext, behindWndRect));

    auto otherAppNode = std::make_shared<RSSurfaceRenderNode>(appNodeId + 1, rsContext->weak_from_this());
    ASSERT_NE(otherAppNode, nullptr);
    rootNode->parent_ = otherAppNode;
    EXPECT_FALSE(RSWindowKeyFrameRenderNode::UpdateKeyFrameBehindWindowRegion(*appNode, *rsContext, behindWndRect));

    rootNode->parent_ = appNode;
    EXPECT_FALSE(RSWindowKeyFrameRenderNode::UpdateKeyFrameBehindWindowRegion(*appNode, *rsContext, behindWndRect));

    rootNode->GetMutableRenderProperties().SetBoundsWidth(1.0f);
    EXPECT_FALSE(RSWindowKeyFrameRenderNode::UpdateKeyFrameBehindWindowRegion(*appNode, *rsContext, behindWndRect));

    rootNode->GetMutableRenderProperties().SetBoundsHeight(1.0f);
    EXPECT_FALSE(RSWindowKeyFrameRenderNode::UpdateKeyFrameBehindWindowRegion(*appNode, *rsContext, behindWndRect));

    keyFrameRenderNode_->GetMutableRenderProperties().SetBoundsWidth(2.0f);
    EXPECT_FALSE(RSWindowKeyFrameRenderNode::UpdateKeyFrameBehindWindowRegion(*appNode, *rsContext, behindWndRect));

    keyFrameRenderNode_->GetMutableRenderProperties().SetBoundsHeight(3.0f);
    EXPECT_TRUE(RSWindowKeyFrameRenderNode::UpdateKeyFrameBehindWindowRegion(*appNode, *rsContext, behindWndRect));
    EXPECT_EQ(behindWndRect.GetWidth(), 2);
    EXPECT_EQ(behindWndRect.GetHeight(), 3);
}

} // namespace OHOS::Rosen
