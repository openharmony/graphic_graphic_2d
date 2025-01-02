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

#include <parameters.h>

#include "gtest/gtest.h"
#include "limit_number.h"

#include "platform/ohos/overdraw/rs_overdraw_controller.h"
#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_node_map.h"
#include "pipeline/rs_proxy_render_node.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_render_thread.h"
#include "pipeline/rs_render_thread_visitor.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "platform/common/rs_system_properties.h"
#include "ui/rs_surface_node.h"
#include "pipeline/rs_effect_render_node.h"
#include "pipeline/rs_dirty_region_manager.h"
#include "property/rs_properties.h"
#include "common/rs_obj_abs_geometry.h"
#include "platform/ohos/backend/rs_surface_frame_ohos_gl.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RsRenderThreadVisitorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RsRenderThreadVisitorTest::SetUpTestCase() {}
void RsRenderThreadVisitorTest::TearDownTestCase() {}
void RsRenderThreadVisitorTest::SetUp() {}
void RsRenderThreadVisitorTest::TearDown() {}

/**
 * @tc.name: PrepareRootRenderNode01
 * @tc.desc: test results of PrepareRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RsRenderThreadVisitorTest, PrepareRootRenderNode01, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[1];
    auto node = std::make_shared<RSRootRenderNode>(nodeId);
    auto rsRenderThreadVisitor = std::make_shared<RSRenderThreadVisitor>();
    rsRenderThreadVisitor->PrepareRootRenderNode(*node);
}

/**
 * @tc.name: PrepareRootRenderNode02
 * @tc.desc: test results of PrepareRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RsRenderThreadVisitorTest, PrepareRootRenderNode02, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[1];
    auto node = std::make_shared<RSRootRenderNode>(nodeId);
    auto rsRenderThreadVisitor = std::make_shared<RSRenderThreadVisitor>();
    rsRenderThreadVisitor->PrepareRootRenderNode(*node);

    RSSurfaceNodeConfig config;
    auto surfaceNode = RSSurfaceNode::Create(config);
    node->AttachRSSurfaceNode(surfaceNode->GetId());
    node->SetEnableRender(false);
    rsRenderThreadVisitor->PrepareRootRenderNode(*node);
    node->SetEnableRender(true);
    node->UpdateSuggestedBufferSize(0, 0);
    rsRenderThreadVisitor->PrepareRootRenderNode(*node);
    node->UpdateSuggestedBufferSize(0, 1);
    rsRenderThreadVisitor->PrepareRootRenderNode(*node);
    node->UpdateSuggestedBufferSize(1, 0);
    rsRenderThreadVisitor->PrepareRootRenderNode(*node);
    node->UpdateSuggestedBufferSize(1, 1);
    rsRenderThreadVisitor->PrepareRootRenderNode(*node);
}

/**
 * @tc.name: PrepareRootRenderNode03
 * @tc.desc: test results of PrepareRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RsRenderThreadVisitorTest, PrepareRootRenderNode03, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[2];
    auto node = std::make_shared<RSRootRenderNode>(nodeId);
    auto rsRenderThreadVisitor = std::make_shared<RSRenderThreadVisitor>();
    rsRenderThreadVisitor->PrepareRootRenderNode(*node);
}

/**
 * @tc.name: PrepareRootRenderNode04
 * @tc.desc: test results of PrepareRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RsRenderThreadVisitorTest, PrepareRootRenderNode04, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[3];
    auto node = std::make_shared<RSRootRenderNode>(nodeId);
    auto rsRenderThreadVisitor = std::make_shared<RSRenderThreadVisitor>();
    rsRenderThreadVisitor->PrepareRootRenderNode(*node);
}

/**
 * @tc.name: PrepareRootRenderNode05
 * @tc.desc: test results of PrepareRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI9TXX3
 */
HWTEST_F(RsRenderThreadVisitorTest, PrepareRootRenderNode05, TestSize.Level1)
{
    RSRenderThreadVisitor rsRenderThreadVisitor;
    RSRootRenderNode node(0);
    rsRenderThreadVisitor.PrepareRootRenderNode(node);
    EXPECT_TRUE(rsRenderThreadVisitor.IsValidRootRenderNode(node) == false);

    node.surfaceNodeId_ = 1;
    RSSurfaceNodeConfig config;
    std::shared_ptr<RSSurfaceNode> rsNode = std::make_shared<RSSurfaceNode>(config, true);
    rsNode->id_ = node.GetRSSurfaceNodeId();
    RSNodeMap::MutableInstance().RegisterNode(rsNode);
    node.enableRender_ = true;
    node.suggestedBufferWidth_ = 1.f;
    node.suggestedBufferHeight_ = 1.f;
    rsRenderThreadVisitor.PrepareRootRenderNode(node);
    EXPECT_TRUE(rsRenderThreadVisitor.IsValidRootRenderNode(node));

    rsRenderThreadVisitor.isIdle_ = false;
    rsRenderThreadVisitor.PrepareRootRenderNode(node);
    EXPECT_TRUE(rsRenderThreadVisitor.IsValidRootRenderNode(node));
}

/**
 * @tc.name: PrepareChildren01
 * @tc.desc: test results of PrepareChildren
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RsRenderThreadVisitorTest, PrepareChildren01, TestSize.Level1)
{
    /**
     * @tc.steps: step1. PrepareChildren
     */
    RSSurfaceRenderNodeConfig config;
    RSSurfaceRenderNode rsSurfaceRenderNode(config);
    std::shared_ptr rsRenderThreadVisitor = std::make_shared<RSRenderThreadVisitor>();
    rsRenderThreadVisitor->PrepareChildren(rsSurfaceRenderNode);
    config.id = 1; // for test
    auto surfaceRenderNode2 = std::make_shared<RSSurfaceRenderNode>(config);
    rsSurfaceRenderNode.AddChild(surfaceRenderNode2, -1);
    rsRenderThreadVisitor->PrepareChildren(rsSurfaceRenderNode);
}

/**
 * @tc.name: PrepareCanvasRenderNode01
 * @tc.desc: test results of PrepareCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RsRenderThreadVisitorTest, PrepareCanvasRenderNode01, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[1];
    auto node = std::make_shared<RSCanvasRenderNode>(nodeId);
    std::shared_ptr<RSRenderThreadVisitor> rsRenderThreadVisitor = std::make_shared<RSRenderThreadVisitor>();
    rsRenderThreadVisitor->PrepareCanvasRenderNode(*node);
}

/**
 * @tc.name: PrepareCanvasRenderNode02
 * @tc.desc: test results of PrepareCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RsRenderThreadVisitorTest, PrepareCanvasRenderNode02, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[1];
    auto node = std::make_shared<RSCanvasRenderNode>(nodeId);
    node->GetMutableRenderProperties().SetAlpha(0);
    node->GetMutableRenderProperties().SetVisible(false);
    std::shared_ptr<RSRenderThreadVisitor> rsRenderThreadVisitor = std::make_shared<RSRenderThreadVisitor>();
    rsRenderThreadVisitor->PrepareCanvasRenderNode(*node);
}

/**
 * @tc.name: PrepareCanvasRenderNode03
 * @tc.desc: test results of PrepareCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RsRenderThreadVisitorTest, PrepareCanvasRenderNode03, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[1];
    auto node = std::make_shared<RSCanvasRenderNode>(nodeId);
    RSSurfaceRenderNodeConfig config;
    auto surfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config);
    node->AddChild(surfaceRenderNode, -1);
    std::shared_ptr<RSRenderThreadVisitor> rsRenderThreadVisitor = std::make_shared<RSRenderThreadVisitor>();
    rsRenderThreadVisitor->PrepareCanvasRenderNode(*node);
}

/**
 * @tc.name: PrepareCanvasRenderNode04
 * @tc.desc: test results of PrepareCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RsRenderThreadVisitorTest, PrepareCanvasRenderNode04, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[3];
    auto node = std::make_shared<RSCanvasRenderNode>(nodeId);
    auto rsRenderThreadVisitor = std::make_shared<RSRenderThreadVisitor>();
    rsRenderThreadVisitor->SetPartialRenderStatus(PartialRenderType::SET_DAMAGE_AND_DROP_OP, false);
    rsRenderThreadVisitor->PrepareCanvasRenderNode(*node);
}

/**
 * @tc.name: PrepareCanvasRenderNode05
 * @tc.desc: test results of PrepareCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RsRenderThreadVisitorTest, PrepareCanvasRenderNode05, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[4];
    auto node = std::make_shared<RSCanvasRenderNode>(nodeId);
    auto rsRenderThreadVisitor = std::make_shared<RSRenderThreadVisitor>();
    rsRenderThreadVisitor->PrepareCanvasRenderNode(*node);
}

/**
 * @tc.name: PrepareCanvasRenderNode06
 * @tc.desc: test results of PrepareCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RsRenderThreadVisitorTest, PrepareCanvasRenderNode06, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[4];
    std::shared_ptr<RSCanvasRenderNode> node = std::make_shared<RSCanvasRenderNode>(nodeId);
    constexpr NodeId nodeId2 = TestSrc::limitNumber::Uint64[2];
    std::shared_ptr<RSRootRenderNode> rootnode = std::make_shared<RSRootRenderNode>(nodeId2);
    rootnode->AddChild(node);
    auto rsRenderThreadVisitor = std::make_shared<RSRenderThreadVisitor>();
    rsRenderThreadVisitor->PrepareCanvasRenderNode(*node);
}

/**
 * @tc.name: PrepareCanvasRenderNode07
 * @tc.desc: test results of PrepareCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI9TXX3
 */
HWTEST_F(RsRenderThreadVisitorTest, PrepareCanvasRenderNode07, TestSize.Level1)
{
    RSRenderThreadVisitor rsRenderThreadVisitor;
    RSCanvasRenderNode node(0);
    rsRenderThreadVisitor.PrepareCanvasRenderNode(node);
    EXPECT_TRUE(rsRenderThreadVisitor.curDirtyManager_ != nullptr);

    node.shouldPaint_ = true;
    rsRenderThreadVisitor.PrepareCanvasRenderNode(node);
    EXPECT_TRUE(node.shouldPaint_);

    node.isDirtyRegionUpdated_ = false;
    rsRenderThreadVisitor.PrepareCanvasRenderNode(node);
    EXPECT_TRUE(!node.isDirtyRegionUpdated_);

    rsRenderThreadVisitor.curDirtyManager_->debugRegionEnabled_[DebugRegionType::CURRENT_SUB] = true;
    rsRenderThreadVisitor.PrepareCanvasRenderNode(node);
    EXPECT_TRUE(!node.isDirtyRegionUpdated_);

    node.isDirtyRegionUpdated_ = true;
    rsRenderThreadVisitor.PrepareCanvasRenderNode(node);
    EXPECT_TRUE(!node.isDirtyRegionUpdated_);

    rsRenderThreadVisitor.curDirtyManager_ = nullptr;
    rsRenderThreadVisitor.PrepareCanvasRenderNode(node);
    EXPECT_TRUE(rsRenderThreadVisitor.curDirtyManager_ == nullptr);

    node.shouldPaint_ = false;
    rsRenderThreadVisitor.PrepareCanvasRenderNode(node);
    EXPECT_TRUE(!node.shouldPaint_);
}

/**
 * @tc.name: PrepareSurfaceRenderNode01
 * @tc.desc: test results of PrepareSurfaceRenderNode
 * @tc.type: FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RsRenderThreadVisitorTest, PrepareSurfaceRenderNode01, TestSize.Level1)
{
    RSRenderThreadVisitor rsRenderThreadVisitor;
    RSSurfaceRenderNode node(0);
    std::shared_ptr<RSSurfaceRenderNode> sharedPtr = std::make_shared<RSSurfaceRenderNode>(0);
    node.parent_ = sharedPtr;
    rsRenderThreadVisitor.PrepareSurfaceRenderNode(node);
    EXPECT_TRUE(rsRenderThreadVisitor.curDirtyManager_ != nullptr);
    EXPECT_FALSE(node.IsNotifyRTBufferAvailable());
}

/**
 * @tc.name: PrepareSurfaceRenderNode02
 * @tc.desc: test results of PrepareSurfaceRenderNode
 * @tc.type: FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RsRenderThreadVisitorTest, PrepareSurfaceRenderNode02, TestSize.Level1)
{
    RSRenderThreadVisitor rsRenderThreadVisitor;
    RSSurfaceRenderNode node(0);
    std::shared_ptr<RSSurfaceRenderNode> sharedPtr = std::make_shared<RSSurfaceRenderNode>(0);
    node.parent_ = sharedPtr;
    node.isNotifyRTBufferAvailablePre_ = true;
    rsRenderThreadVisitor.PrepareSurfaceRenderNode(node);
    EXPECT_TRUE(rsRenderThreadVisitor.curDirtyManager_ != nullptr);
    EXPECT_TRUE(node.IsNotifyRTBufferAvailablePre());
}

/**
 * @tc.name: PrepareSurfaceRenderNode03
 * @tc.desc: test results of PrepareSurfaceRenderNode
 * @tc.type: FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RsRenderThreadVisitorTest, PrepareSurfaceRenderNode03, TestSize.Level1)
{
    RSRenderThreadVisitor rsRenderThreadVisitor;
    RSSurfaceRenderNode node(0);
    std::shared_ptr<RSSurfaceRenderNode> sharedPtr = std::make_shared<RSSurfaceRenderNode>(0);
    node.isTextureExportNode_ = true;
    RSProperties& properties = sharedPtr->GetMutableRenderProperties();
    properties.frameGeo_.SetWidth(1.f);
    properties.frameGeo_.SetHeight(1.f);
    node.parent_ = sharedPtr;
    rsRenderThreadVisitor.PrepareSurfaceRenderNode(node);
    EXPECT_TRUE(node.GetIsTextureExportNode());
    EXPECT_TRUE(rsRenderThreadVisitor.curDirtyManager_ != nullptr);
}

/**
 * @tc.name: PrepareSurfaceRenderNode04
 * @tc.desc: test results of PrepareSurfaceRenderNode
 * @tc.type: FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RsRenderThreadVisitorTest, PrepareSurfaceRenderNode04, TestSize.Level1)
{
    RSRenderThreadVisitor rsRenderThreadVisitor;
    RSSurfaceRenderNode node(0);
    std::shared_ptr<RSSurfaceRenderNode> sharedPtr = std::make_shared<RSSurfaceRenderNode>(0);
    node.isDirtyRegionUpdated_ = true;
    node.parent_ = sharedPtr;
    rsRenderThreadVisitor.PrepareSurfaceRenderNode(node);
    EXPECT_FALSE(node.IsDirtyRegionUpdated());
    EXPECT_TRUE(rsRenderThreadVisitor.curDirtyManager_ != nullptr);
}

/**
 * @tc.name: PrepareSurfaceRenderNode05
 * @tc.desc: test results of PrepareSurfaceRenderNode
 * @tc.type: FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RsRenderThreadVisitorTest, PrepareSurfaceRenderNode05, TestSize.Level1)
{
    RSRenderThreadVisitor rsRenderThreadVisitor;
    RSSurfaceRenderNode node(0);
    std::shared_ptr<RSSurfaceRenderNode> sharedPtr = std::make_shared<RSSurfaceRenderNode>(0);
    node.isDirtyRegionUpdated_ = true;
    rsRenderThreadVisitor.curDirtyManager_->debugRegionEnabled_[DebugRegionType::CURRENT_SUB] = 1;
    node.parent_ = sharedPtr;
    rsRenderThreadVisitor.PrepareSurfaceRenderNode(node);
    EXPECT_TRUE(rsRenderThreadVisitor.curDirtyManager_ != nullptr);
    EXPECT_FALSE(node.IsDirtyRegionUpdated());
}

/**
 * @tc.name: PrepareSurfaceRenderNode06
 * @tc.desc: test results of PrepareSurfaceRenderNode
 * @tc.type: FUNC
 * @tc.require: issueIAJ76O
 */
HWTEST_F(RsRenderThreadVisitorTest, PrepareSurfaceRenderNode06, TestSize.Level1)
{
    RSRenderThreadVisitor rsRenderThreadVisitor;
    RSSurfaceRenderNode node(0);
    rsRenderThreadVisitor.curDirtyManager_ = nullptr;
    rsRenderThreadVisitor.PrepareSurfaceRenderNode(node);
    EXPECT_TRUE(rsRenderThreadVisitor.curDirtyManager_ == nullptr);
}

/**
 * @tc.name: PrepareSurfaceRenderNode07
 * @tc.desc: test results of PrepareSurfaceRenderNode
 * @tc.type: FUNC
 * @tc.require: issueIAJ76O
 */
HWTEST_F(RsRenderThreadVisitorTest, PrepareSurfaceRenderNode07, TestSize.Level1)
{
    RSRenderThreadVisitor rsRenderThreadVisitor;
    RSSurfaceRenderNode node(0);
    auto nodeParent = node.GetParent().lock();
    nodeParent.reset();
    rsRenderThreadVisitor.PrepareSurfaceRenderNode(node);
    EXPECT_TRUE(rsRenderThreadVisitor.curDirtyManager_ != nullptr);
}

/**
 * @tc.name: ProcessChildren01
 * @tc.desc: test results of ProcessChildren
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RsRenderThreadVisitorTest, ProcessChildren01, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig config;
    RSSurfaceRenderNode rsSurfaceRenderNode(config);
    std::shared_ptr rsRenderThreadVisitor = std::make_shared<RSRenderThreadVisitor>();
    rsRenderThreadVisitor->ProcessChildren(rsSurfaceRenderNode);
    config.id = 1;
    auto surfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config);
    rsSurfaceRenderNode.AddChild(surfaceRenderNode, -1);
    rsRenderThreadVisitor->ProcessChildren(rsSurfaceRenderNode);
}

/**
 * @tc.name: ProcessProxyRenderNode01
 * @tc.desc: test results of ProcessProxyRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RsRenderThreadVisitorTest, ProcessProxyRenderNode01, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig config;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config);
    std::weak_ptr<RSSurfaceRenderNode> rsSurfaceRenderNodeW(rsSurfaceRenderNode);

    auto rsContext = std::make_shared<RSContext>();
    NodeId id = 0;
    NodeId targetID = 0;
    std::shared_ptr<RSProxyRenderNode> rsProxyRenderNode(
        new RSProxyRenderNode(id, rsSurfaceRenderNodeW, targetID, rsContext->weak_from_this()));

    RSRenderThreadVisitor rsRenderThreadVisitor;
    rsRenderThreadVisitor.ProcessProxyRenderNode(*rsProxyRenderNode);
    EXPECT_TRUE(rsRenderThreadVisitor.canvas_ == nullptr);

    Drawing::Canvas canvas;
    rsRenderThreadVisitor.canvas_ = std::make_shared<RSPaintFilterCanvas>(&canvas);
    rsRenderThreadVisitor.ProcessProxyRenderNode(*rsProxyRenderNode);
    EXPECT_TRUE(rsRenderThreadVisitor.canvas_ != nullptr);
}

/**
 * @tc.name: ProcessCanvasRenderNode01
 * @tc.desc: test results of ProcessCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RsRenderThreadVisitorTest, ProcessCanvasRenderNode01, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[1];
    RSCanvasRenderNode node(nodeId);
    node.GetMutableRenderProperties().SetAlpha(0);
    RSRenderThreadVisitor rsRenderThreadVisitor;
    rsRenderThreadVisitor.ProcessCanvasRenderNode(node);
}

/**
 * @tc.name: ProcessCanvasRenderNode02
 * @tc.desc: test results of ProcessCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RsRenderThreadVisitorTest, ProcessCanvasRenderNode02, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[1];
    RSCanvasRenderNode node(nodeId);
    RSRenderThreadVisitor rsRenderThreadVisitor;
    rsRenderThreadVisitor.ProcessCanvasRenderNode(node);
}

/**
 * @tc.name: ProcessCanvasRenderNode03
 * @tc.desc: test results of ProcessCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RsRenderThreadVisitorTest, ProcessCanvasRenderNode03, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[2];
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSCanvasRenderNode>(nodeId, rsContext->weak_from_this());
    constexpr NodeId nodeId2 = 10;
    RSRootRenderNode node2(nodeId2);
    node2.AddChild(node, -1);
    node2.SetEnableRender(false);
    std::shared_ptr rsRenderThreadVisitor = std::make_shared<RSRenderThreadVisitor>();
    rsRenderThreadVisitor->ProcessRootRenderNode(node2);
    rsRenderThreadVisitor->ProcessCanvasRenderNode(node2);
}

/**
 * @tc.name: ProcessCanvasRenderNode04
 * @tc.desc: test results of ProcessCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RsRenderThreadVisitorTest, ProcessCanvasRenderNode04, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[3];
    RSCanvasRenderNode node(nodeId);
    RSRenderThreadVisitor rsRenderThreadVisitor;
    rsRenderThreadVisitor.ProcessCanvasRenderNode(node);
}

/**
 * @tc.name: ProcessCanvasRenderNode05
 * @tc.desc: test results of ProcessCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI9TXX3
 */
HWTEST_F(RsRenderThreadVisitorTest, ProcessCanvasRenderNode05, TestSize.Level1)
{
    RSCanvasRenderNode node(0);
    RSRenderThreadVisitor rsRenderThreadVisitor;
    node.shouldPaint_ = false;
    rsRenderThreadVisitor.ProcessCanvasRenderNode(node);
    EXPECT_TRUE(!node.shouldPaint_);

    node.shouldPaint_ = true;
    rsRenderThreadVisitor.ProcessCanvasRenderNode(node);
    EXPECT_TRUE(node.shouldPaint_);

    Drawing::Canvas canvas;
    rsRenderThreadVisitor.canvas_ = std::make_shared<RSPaintFilterCanvas>(&canvas);
    rsRenderThreadVisitor.ProcessCanvasRenderNode(node);
    EXPECT_TRUE(rsRenderThreadVisitor.canvas_ != nullptr);

    rsRenderThreadVisitor.isOpDropped_ = true;
    rsRenderThreadVisitor.ProcessCanvasRenderNode(node);
    EXPECT_TRUE(rsRenderThreadVisitor.isOpDropped_);

    rsRenderThreadVisitor.isOpDropped_ = false;
    auto& properties = node.GetMutableRenderProperties();
    properties.isSpherizeValid_ = true;
    rsRenderThreadVisitor.ProcessCanvasRenderNode(node);
    EXPECT_TRUE(properties.isSpherizeValid_);

    node.cacheCompletedSurface_ = std::make_shared<Drawing::Surface>();
    rsRenderThreadVisitor.ProcessCanvasRenderNode(node);
    EXPECT_TRUE(properties.isSpherizeValid_);

    node.cacheSurface_ = std::make_shared<Drawing::Surface>();
    node.cacheSurface_->cachedCanvas_ = std::make_shared<Drawing::Canvas>();
    rsRenderThreadVisitor.ProcessCanvasRenderNode(node);
    EXPECT_TRUE(properties.isSpherizeValid_);

    node.cacheCompletedSurface_ = nullptr;
    rsRenderThreadVisitor.ProcessCanvasRenderNode(node);
    EXPECT_TRUE(properties.isSpherizeValid_);

    properties.isSpherizeValid_ = false;
    node.cacheCompletedSurface_ = std::make_shared<Drawing::Surface>();
    rsRenderThreadVisitor.ProcessCanvasRenderNode(node);
    EXPECT_TRUE(!properties.isSpherizeValid_);
}

/**
 * @tc.name: ProcessRootRenderNode01
 * @tc.desc: test results of ProcessRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RsRenderThreadVisitorTest, ProcessRootRenderNode01, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[1];
    RSRootRenderNode node(nodeId);
    RSRenderThreadVisitor rsRenderThreadVisitor;
    rsRenderThreadVisitor.ProcessRootRenderNode(node);
}

/**
 * @tc.name: ProcessRootRenderNode02
 * @tc.desc: test results of ProcessRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RsRenderThreadVisitorTest, ProcessRootRenderNode02, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[1];
    RSRootRenderNode node(nodeId);
    node.UpdateSuggestedBufferSize(0, 0);
    RSRenderThreadVisitor rsRenderThreadVisitor;
    rsRenderThreadVisitor.ProcessRootRenderNode(node);
}

/**
 * @tc.name: ProcessRootRenderNode03
 * @tc.desc: test results of ProcessRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RsRenderThreadVisitorTest, ProcessRootRenderNode03, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[1];
    RSRootRenderNode node(nodeId);
    node.SetEnableRender(false);
    RSRenderThreadVisitor rsRenderThreadVisitor;
    rsRenderThreadVisitor.ProcessRootRenderNode(node);
}

/**
 * @tc.name: ProcessRootRenderNode04
 * @tc.desc: test results of ProcessRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RsRenderThreadVisitorTest, ProcessRootRenderNode04, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[2];
    RSRootRenderNode node(nodeId);
    node.SetEnableRender(false);
    RSSurfaceNodeConfig config;
    auto surfaceNode = RSSurfaceNode::Create(config);
    node.AttachRSSurfaceNode(surfaceNode->GetId());
    node.UpdateSuggestedBufferSize(10, 10);
    RSRenderThreadVisitor rsRenderThreadVisitor;
    rsRenderThreadVisitor.ProcessRootRenderNode(node);
    RSOverdrawController::GetInstance().SetEnable(false);
    rsRenderThreadVisitor.ProcessRootRenderNode(node);
    rsRenderThreadVisitor.SetPartialRenderStatus(PartialRenderType::DISABLED, true);
    rsRenderThreadVisitor.ProcessRootRenderNode(node);
}

/**
 * @tc.name: ProcessRootRenderNode05
 * @tc.desc: test results of ProcessRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RsRenderThreadVisitorTest, ProcessRootRenderNode05, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[3];
    RSRootRenderNode node(nodeId);
    node.SetEnableRender(false);
    RSRenderThreadVisitor rsRenderThreadVisitor;
    rsRenderThreadVisitor.ProcessRootRenderNode(node);
}

/**
 * @tc.name: ProcessRootRenderNode06
 * @tc.desc: test results of ProcessRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RsRenderThreadVisitorTest, ProcessRootRenderNode06, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[4];
    RSRootRenderNode node(nodeId);
    node.SetEnableRender(false);
    RSRenderThreadVisitor rsRenderThreadVisitor;
    rsRenderThreadVisitor.ProcessRootRenderNode(node);
}

/**
 * @tc.name: ProcessRootRenderNode07
 * @tc.desc: test results of ProcessRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RsRenderThreadVisitorTest, ProcessRootRenderNode07, TestSize.Level1)
{
    system::SetParameter("rosen.uni.partialrender.enabled", "0");
    int param = (int)RSSystemProperties::GetDirtyRegionDebugType();
    ASSERT_EQ(param, 0);
    RSSurfaceNodeConfig surfaceNodeConfig;
    auto surfacenode = RSSurfaceNode::Create(surfaceNodeConfig);
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[4];
    auto rootnode = std::make_shared<RSRootRenderNode>(nodeId);
    rootnode->InitRenderParams();
    rootnode->AttachRSSurfaceNode(surfacenode->GetId());
    RSNodeMap::MutableInstance().RegisterNode(surfacenode);
    auto rsRenderThreadVisitor = std::make_shared<RSRenderThreadVisitor>();
    rootnode->Prepare(rsRenderThreadVisitor);
    rootnode->Process(rsRenderThreadVisitor);
    rsRenderThreadVisitor->SetPartialRenderStatus(PartialRenderType::SET_DAMAGE, true);
    constexpr NodeId nodeId2 = TestSrc::limitNumber::Uint64[1];
    auto canvasnode = std::make_shared<RSCanvasRenderNode>(nodeId2);
    canvasnode->InitRenderParams();
    canvasnode->GetMutableRenderProperties().SetAlpha(1.f);
    rootnode->AddChild(canvasnode, -1);
    RSSurfaceRenderNodeConfig config;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config);
    rsSurfaceRenderNode->InitRenderParams();
    rsSurfaceRenderNode->GetMutableRenderProperties().SetAlpha(1.f);
    rsSurfaceRenderNode->NotifyRTBufferAvailable();
    canvasnode->AddChild(rsSurfaceRenderNode, -1);
    rootnode->Process(rsRenderThreadVisitor);
}

/**
 * @tc.name: ProcessRootRenderNode08
 * @tc.desc: test results of ProcessRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RsRenderThreadVisitorTest, ProcessRootRenderNode08, TestSize.Level1)
{
    RSRenderThreadVisitor visitor;
    RSRootRenderNode node(0);
    visitor.isIdle_ = false;
    visitor.ProcessRootRenderNode(node);
    EXPECT_FALSE(visitor.isIdle_);

    visitor.isIdle_ = true;
    visitor.ProcessRootRenderNode(node);
    EXPECT_TRUE(visitor.isIdle_);

    node.surfaceNodeId_ = 1;
    RSSurfaceNodeConfig config;
    std::shared_ptr<RSSurfaceNode> rsNode = std::make_shared<RSSurfaceNode>(config, true);
    rsNode->id_ = node.GetRSSurfaceNodeId();
    RSNodeMap::MutableInstance().RegisterNode(rsNode);
    node.enableRender_ = true;
    node.suggestedBufferWidth_ = 1.f;
    node.suggestedBufferHeight_ = 1.f;
    visitor.ProcessRootRenderNode(node);
    EXPECT_TRUE(visitor.IsValidRootRenderNode(node));

    visitor.isOpDropped_ = true;
    visitor.ProcessRootRenderNode(node);
    EXPECT_TRUE(visitor.IsValidRootRenderNode(node));

    visitor.curDirtyManager_ = std::make_shared<RSDirtyRegionManager>();
    visitor.ProcessRootRenderNode(node);
    EXPECT_TRUE(visitor.curDirtyManager_);

    visitor.curDirtyManager_->currentFrameDirtyRegion_.width_ = 1;
    visitor.curDirtyManager_->currentFrameDirtyRegion_.height_ = 1;
    visitor.ProcessRootRenderNode(node);
    EXPECT_FALSE(visitor.curDirtyManager_->GetCurrentFrameDirtyRegion().IsEmpty());

    visitor.isRenderForced_ = true;
    visitor.ProcessRootRenderNode(node);
    EXPECT_TRUE(visitor.isRenderForced_);

    visitor.isOpDropped_ = false;
    visitor.ProcessRootRenderNode(node);
    EXPECT_FALSE(visitor.isOpDropped_);
}

/**
 * @tc.name: ProcessSurfaceRenderNode01
 * @tc.desc: test results of ProcessSurfaceRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RsRenderThreadVisitorTest, ProcessSurfaceRenderNode01, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig config;
    auto node = std::make_shared<RSSurfaceRenderNode>(config);
    RSRenderThreadVisitor rsRenderThreadVisitor;
    rsRenderThreadVisitor.ProcessSurfaceRenderNode(*node);
    EXPECT_TRUE(rsRenderThreadVisitor.canvas_ == nullptr);

    // Use non-default constructor only to trigger creation of clip rectangle for canvas,
    // because the test calls RSRenderThreadVisitor::ProcessSurfaceRenderNode method,
    // which in turn tries to get clip boundaries via RSPaintFilterCanvas::GetLocalClipBounds.
    // This way the test won't exit the function under test early, and can do a deeper traversal of the code.
    Drawing::Canvas canvas(10, 10);
    rsRenderThreadVisitor.canvas_ = std::make_shared<RSPaintFilterCanvas>(&canvas);
    rsRenderThreadVisitor.ProcessSurfaceRenderNode(*node);
    EXPECT_TRUE(rsRenderThreadVisitor.canvas_ != nullptr);

    node->shouldPaint_ = false;
    rsRenderThreadVisitor.ProcessSurfaceRenderNode(*node);
    EXPECT_TRUE(!node->shouldPaint_);

    node->shouldPaint_ = true;
    rsRenderThreadVisitor.childSurfaceNodeIds_.clear();
    rsRenderThreadVisitor.ProcessSurfaceRenderNode(*node);
    EXPECT_TRUE(!rsRenderThreadVisitor.childSurfaceNodeIds_.empty());

    node->isTextureExportNode_ = true;
    rsRenderThreadVisitor.childSurfaceNodeIds_.push_back(0);
    rsRenderThreadVisitor.ProcessSurfaceRenderNode(*node);
    EXPECT_TRUE(!rsRenderThreadVisitor.childSurfaceNodeIds_.empty());
}

/**
 * @tc.name: ProcessSurfaceRenderNode02
 * @tc.desc: test results of ProcessSurfaceRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RsRenderThreadVisitorTest, ProcessSurfaceRenderNode02, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig config;
    auto rsContext = std::make_shared<RSContext>();
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[4];
    RSRootRenderNode node(nodeId);
    node.AddChild(rsSurfaceRenderNode, -1);
    node.UpdateSuggestedBufferSize(10, 10);
    rsSurfaceRenderNode->GetMutableRenderProperties().SetVisible(false);
    std::shared_ptr rsRenderThreadVisitor = std::make_shared<RSRenderThreadVisitor>();
    rsRenderThreadVisitor->ProcessRootRenderNode(node);
    rsRenderThreadVisitor->ProcessSurfaceRenderNode(*rsSurfaceRenderNode);
}

/**
 * @tc.name: ProcessSurfaceRenderNode03
 * @tc.desc: test results of ProcessSurfaceRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RsRenderThreadVisitorTest, ProcessSurfaceRenderNode03, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig config;
    RSSurfaceRenderNode rsSurfaceRenderNode(config);

    config.id = 1;
    auto node = std::make_shared<RSSurfaceRenderNode>(config);
    rsSurfaceRenderNode.AddChild(node, -1);
    std::shared_ptr rsRenderThreadVisitor = std::make_shared<RSRenderThreadVisitor>();
    rsRenderThreadVisitor->ProcessSurfaceRenderNode(rsSurfaceRenderNode);
}

/**
 * @tc.name: SetPartialRenderStatus01
 * @tc.desc: test results of GetPartialRenderEnabled
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RsRenderThreadVisitorTest, SetPartialRenderStatus01, TestSize.Level1)
{
    RSRenderThreadVisitor rsRenderThreadVisitor;
    rsRenderThreadVisitor.SetPartialRenderStatus(RSSystemProperties::GetPartialRenderEnabled(), false);
    EXPECT_EQ(rsRenderThreadVisitor.partialRenderStatus_, PartialRenderType::SET_DAMAGE_AND_DROP_OP);
}

/**
 * @tc.name: SetPartialRenderStatus02
 * @tc.desc: test results of GetPartialRenderEnabled
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RsRenderThreadVisitorTest, SetPartialRenderStatus02, TestSize.Level1)
{
    RSRenderThreadVisitor rsRenderThreadVisitor;
    rsRenderThreadVisitor.SetPartialRenderStatus(RSSystemProperties::GetPartialRenderEnabled(), true);
    EXPECT_EQ(rsRenderThreadVisitor.partialRenderStatus_, PartialRenderType::SET_DAMAGE_AND_DROP_OP);
}

/**
 * @tc.name: SetPartialRenderStatus03
 * @tc.desc: test results of GetPartialRenderEnabled
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RsRenderThreadVisitorTest, SetPartialRenderStatus03, TestSize.Level1)
{
    RSRenderThreadVisitor rsRenderThreadVisitor;
    rsRenderThreadVisitor.SetPartialRenderStatus(RSSystemProperties::GetUniPartialRenderEnabled(), true);
    EXPECT_EQ(rsRenderThreadVisitor.partialRenderStatus_, PartialRenderType::DISABLED);
}

/**
 * @tc.name: SetPartialRenderStatus04
 * @tc.desc: test results of GetPartialRenderEnabled
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RsRenderThreadVisitorTest, SetPartialRenderStatus04, TestSize.Level1)
{
    RSRenderThreadVisitor rsRenderThreadVisitor;
    rsRenderThreadVisitor.SetPartialRenderStatus(RSSystemProperties::GetUniPartialRenderEnabled(), false);
    EXPECT_EQ(rsRenderThreadVisitor.partialRenderStatus_, PartialRenderType::DISABLED);
}

/**
 * @tc.name: SetPartialRenderStatus05
 * @tc.desc: test results of GetPartialRenderEnabled
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RsRenderThreadVisitorTest, SetPartialRenderStatus05, TestSize.Level1)
{
    RSRenderThreadVisitor rsRenderThreadVisitor;
    rsRenderThreadVisitor.SetPartialRenderStatus(RSSystemProperties::GetUniPartialRenderEnabled(), true);
    EXPECT_EQ(rsRenderThreadVisitor.partialRenderStatus_, PartialRenderType::DISABLED);
}

/**
 * @tc.name: SetPartialRenderStatus06
 * @tc.desc: test results of GetPartialRenderEnabled
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RsRenderThreadVisitorTest, SetPartialRenderStatus06, TestSize.Level1)
{
    RSRenderThreadVisitor rsRenderThreadVisitor;
    rsRenderThreadVisitor.SetPartialRenderStatus(PartialRenderType::DISABLED, true);
    rsRenderThreadVisitor.SetPartialRenderStatus(PartialRenderType::SET_DAMAGE_AND_DROP_OP, true);
    EXPECT_EQ(rsRenderThreadVisitor.partialRenderStatus_, PartialRenderType::SET_DAMAGE_AND_DROP_OP);
}

/**
 * @tc.name: IsValidRootRenderNode01
 * @tc.desc: test results of IsValidRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RsRenderThreadVisitorTest, IsValidRootRenderNode01, TestSize.Level1)
{
    RSRenderThreadVisitor rsRenderThreadVisitor;
    RSRootRenderNode node(0);
    bool res = rsRenderThreadVisitor.IsValidRootRenderNode(node);
    EXPECT_EQ(res, false);

    node.surfaceNodeId_ = 1;
    RSSurfaceNodeConfig config;
    std::shared_ptr<RSSurfaceNode> rsNode = std::make_shared<RSSurfaceNode>(config, true);
    rsNode->id_ = node.GetRSSurfaceNodeId();
    RSNodeMap::MutableInstance().RegisterNode(rsNode);
    node.enableRender_ = false;
    res = rsRenderThreadVisitor.IsValidRootRenderNode(node);
    EXPECT_EQ(res, false);

    node.enableRender_ = true;
    res = rsRenderThreadVisitor.IsValidRootRenderNode(node);
    EXPECT_EQ(res, false);

    node.suggestedBufferHeight_ = 0.f;
    res = rsRenderThreadVisitor.IsValidRootRenderNode(node);
    EXPECT_EQ(res, false);

    node.suggestedBufferWidth_ = 1.f;
    res = rsRenderThreadVisitor.IsValidRootRenderNode(node);
    EXPECT_EQ(res, false);

    node.suggestedBufferHeight_ = 0.f;
    res = rsRenderThreadVisitor.IsValidRootRenderNode(node);
    EXPECT_EQ(res, false);

    node.suggestedBufferHeight_ = 1.f;
    res = rsRenderThreadVisitor.IsValidRootRenderNode(node);
    EXPECT_EQ(res, true);
}

/**
 * @tc.name: ResetAndPrepareChildrenNode01
 * @tc.desc: test results of ResetAndPrepareChildrenNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RsRenderThreadVisitorTest, ResetAndPrepareChildrenNode01, TestSize.Level1)
{
    RSRenderThreadVisitor rsRenderThreadVisitor;
    RSRenderNode node(0);
    std::shared_ptr<RSBaseRenderNode> nodeParent;
    rsRenderThreadVisitor.ResetAndPrepareChildrenNode(node, nodeParent);
    EXPECT_TRUE(node.HasRemovedChild() == false);
    EXPECT_TRUE(rsRenderThreadVisitor.curDirtyManager_ != nullptr);

    node.hasRemovedChild_ = true;
    rsRenderThreadVisitor.ResetAndPrepareChildrenNode(node, nodeParent);
    EXPECT_TRUE(node.hasRemovedChild_ == false);

    rsRenderThreadVisitor.curDirtyManager_ = nullptr;
    rsRenderThreadVisitor.ResetAndPrepareChildrenNode(node, nodeParent);
    EXPECT_TRUE(rsRenderThreadVisitor.curDirtyManager_ == nullptr);

    node.hasRemovedChild_ = true;
    rsRenderThreadVisitor.ResetAndPrepareChildrenNode(node, nodeParent);
    EXPECT_TRUE(node.hasRemovedChild_);
}

/**
 * @tc.name: PrepareEffectRenderNode01
 * @tc.desc: test results of PrepareEffectRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RsRenderThreadVisitorTest, PrepareEffectRenderNode01, TestSize.Level1)
{
    RSRenderThreadVisitor rsRenderThreadVisitor;
    RSEffectRenderNode node(0);
    rsRenderThreadVisitor.PrepareEffectRenderNode(node);
    EXPECT_TRUE(rsRenderThreadVisitor.curDirtyManager_ != nullptr);

    node.shouldPaint_ = true;
    rsRenderThreadVisitor.PrepareEffectRenderNode(node);
    EXPECT_TRUE(node.shouldPaint_);

    rsRenderThreadVisitor.curDirtyManager_ = nullptr;
    rsRenderThreadVisitor.PrepareEffectRenderNode(node);
    EXPECT_TRUE(rsRenderThreadVisitor.curDirtyManager_ == nullptr);

    node.shouldPaint_ = false;
    rsRenderThreadVisitor.PrepareEffectRenderNode(node);
    EXPECT_TRUE(!node.shouldPaint_);
}

/**
 * @tc.name: DrawRectOnCanvas01
 * @tc.desc: test results of DrawRectOnCanvas
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RsRenderThreadVisitorTest, DrawRectOnCanvas01, TestSize.Level1)
{
    RSRenderThreadVisitor visitor;
    RectI dirtyRect;
    Drawing::ColorQuad color = 0;
    RSRenderThreadVisitor::RSPaintStyle fillType;
    fillType = RSRenderThreadVisitor::RSPaintStyle::FILL;
    visitor.DrawRectOnCanvas(dirtyRect, color, fillType, 1.f, 1);
    EXPECT_EQ(dirtyRect.IsEmpty(), true);

    RectI rect(1, 1, 1, 1); // for test
    dirtyRect = rect;
    Drawing::Canvas canvas;
    visitor.canvas_ = std::make_shared<RSPaintFilterCanvas>(&canvas);
    visitor.DrawRectOnCanvas(dirtyRect, color, fillType, 1.f, 1);
    EXPECT_EQ(dirtyRect.IsEmpty(), false);

    fillType = RSRenderThreadVisitor::RSPaintStyle::STROKE;
    visitor.DrawRectOnCanvas(dirtyRect, color, fillType, 1.f, 1);
    EXPECT_EQ(dirtyRect.IsEmpty(), false);
}

/**
 * @tc.name: DrawDirtyRegion01
 * @tc.desc: test results of DrawDirtyRegion
 * @tc.type: FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RsRenderThreadVisitorTest, DrawDirtyRegion01, TestSize.Level1)
{
    RSRenderThreadVisitor visitor;
    visitor.curDirtyManager_->debugRegionEnabled_[DebugRegionType::MULTI_HISTORY] = 1;
    visitor.curDirtyManager_->dirtyRegion_ = RectI();
    visitor.DrawDirtyRegion();
    EXPECT_EQ(visitor.curDirtyManager_->debugRegionEnabled_.empty(), false);

    RectI rect(1, 1, 1, 1);
    auto dirtyRect = RectI();
    visitor.curDirtyManager_->dirtyRegion_ = rect;
    Drawing::Canvas canvas;
    visitor.canvas_ = std::make_shared<RSPaintFilterCanvas>(&canvas);
    visitor.DrawDirtyRegion();
    EXPECT_TRUE(!visitor.curDirtyManager_->dirtyRegion_.IsEmpty());

    visitor.curDirtyManager_->debugRegionEnabled_[DebugRegionType::MULTI_HISTORY] = 0;
    visitor.curDirtyManager_->debugRegionEnabled_[DebugRegionType::CURRENT_WHOLE] = 1;
    visitor.DrawDirtyRegion();
    EXPECT_EQ(visitor.curDirtyManager_->debugRegionEnabled_.empty(), false);

    visitor.curDirtyManager_->dirtyRegion_ = RectI();
    visitor.DrawDirtyRegion();
    EXPECT_TRUE(visitor.curDirtyManager_->dirtyRegion_.IsEmpty());

    visitor.curDirtyManager_->debugRegionEnabled_[DebugRegionType::CURRENT_WHOLE] = 0;
    visitor.curDirtyManager_->debugRegionEnabled_[DebugRegionType::CURRENT_SUB] = 1;
    std::map<NodeId, RectI> newMap;
    newMap[0] = rect;
    visitor.curDirtyManager_->dirtyCanvasNodeInfo_.push_back(newMap);
    visitor.curDirtyManager_->dirtySurfaceNodeInfo_.push_back(newMap);
    visitor.DrawDirtyRegion();
    EXPECT_EQ(visitor.curDirtyManager_->debugRegionEnabled_.empty(), false);

    visitor.curDirtyManager_->debugRegionEnabled_[DebugRegionType::CURRENT_SUB] = 0;
    visitor.DrawDirtyRegion();
    EXPECT_EQ(visitor.curDirtyManager_->debugRegionEnabled_.empty(), false);
}

/**
 * @tc.name: UpdateDirtyAndSetEGLDamageRegion01
 * @tc.desc: test results of UpdateDirtyAndSetEGLDamageRegion
 * @tc.type: FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RsRenderThreadVisitorTest, UpdateDirtyAndSetEGLDamageRegion01, TestSize.Level1)
{
    RSRenderThreadVisitor visitor;
    visitor.isEglSetDamageRegion_ = true;
    std::unique_ptr<RSSurfaceFrame> surfaceFrame = std::make_unique<RSSurfaceFrameOhosGl>(1, 1);
    visitor.curDirtyManager_ = std::make_shared<RSDirtyRegionManager>();
    visitor.UpdateDirtyAndSetEGLDamageRegion(surfaceFrame);
    EXPECT_TRUE(surfaceFrame);

    visitor.isEglSetDamageRegion_ = false;
    visitor.UpdateDirtyAndSetEGLDamageRegion(surfaceFrame);
    EXPECT_TRUE(surfaceFrame);
}

/**
 * @tc.name: ProcessShadowFirst01
 * @tc.desc: test results of ProcessShadowFirst
 * @tc.type: FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RsRenderThreadVisitorTest, ProcessShadowFirst01, TestSize.Level1)
{
    RSRenderThreadVisitor visitor;
    RSRenderNode node(0);
    auto& properties = node.GetMutableRenderProperties();
    visitor.ProcessShadowFirst(node);
    EXPECT_EQ(properties.GetUseShadowBatching(), false);

    properties.useShadowBatching_ = true;
    visitor.ProcessShadowFirst(node);
    EXPECT_EQ(properties.GetUseShadowBatching(), true);

    auto fullChildrenList = std::make_shared<std::vector<std::shared_ptr<RSRenderNode>>>();
    auto canvasRenderNodeChild = std::make_shared<RSCanvasRenderNode>(0);
    fullChildrenList->emplace_back(canvasRenderNodeChild);
    node.fullChildrenList_ = std::move(fullChildrenList);
    Drawing::Canvas drawingCanvas;
    visitor.canvas_ = std::make_shared<RSPaintFilterCanvas>(&drawingCanvas);
    visitor.ProcessShadowFirst(node);
    EXPECT_EQ(properties.GetUseShadowBatching(), true);
}

/**
 * @tc.name: UpdateAnimatePropertyCacheSurface01
 * @tc.desc: test results of UpdateAnimatePropertyCacheSurface
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RsRenderThreadVisitorTest, UpdateAnimatePropertyCacheSurface01, TestSize.Level1)
{
    RSRenderThreadVisitor visitor;
    RSRenderNode node(0);
    bool res = visitor.UpdateAnimatePropertyCacheSurface(node);
    EXPECT_EQ(res, false);

    node.cacheSurface_ = std::make_shared<Drawing::Surface>();
    node.cacheSurface_->cachedCanvas_ = std::make_shared<Drawing::Canvas>();
    Drawing::Canvas canvas;
    visitor.canvas_ = std::make_shared<RSPaintFilterCanvas>(&canvas);
    res = visitor.UpdateAnimatePropertyCacheSurface(node);
    EXPECT_TRUE(res);
}

/**
 * @tc.name: ProcessEffectRenderNode01
 * @tc.desc: test results of ProcessEffectRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RsRenderThreadVisitorTest, ProcessEffectRenderNode01, TestSize.Level1)
{
    RSRenderThreadVisitor visitor;
    RSEffectRenderNode node(0);
    visitor.ProcessEffectRenderNode(node);
    EXPECT_EQ(visitor.canvas_, nullptr);

    Drawing::Canvas canvas;
    visitor.canvas_ = std::make_shared<RSPaintFilterCanvas>(&canvas);
    visitor.ProcessEffectRenderNode(node);
    EXPECT_NE(visitor.canvas_, nullptr);

    node.shouldPaint_ = false;
    visitor.ProcessEffectRenderNode(node);
    EXPECT_EQ(node.shouldPaint_, false);
}

/**
 * @tc.name: CacRotationFromTransformType01
 * @tc.desc: test results of CacRotationFromTransformType
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RsRenderThreadVisitorTest, CacRotationFromTransformType01, TestSize.Level1)
{
    RSRenderThreadVisitor visitor;
    RectF bounds(1.f, 1.f, 1.f, 1.f); // for test
    GraphicTransformType transform = GraphicTransformType::GRAPHIC_FLIP_H_ROT90;
    visitor.CacRotationFromTransformType(transform, bounds);
    EXPECT_EQ(bounds.IsEmpty(), false);

    transform = GraphicTransformType::GRAPHIC_FLIP_V_ROT90;
    visitor.CacRotationFromTransformType(transform, bounds);
    EXPECT_EQ(bounds.IsEmpty(), false);

    transform = GraphicTransformType::GRAPHIC_FLIP_H_ROT180;
    visitor.CacRotationFromTransformType(transform, bounds);
    EXPECT_EQ(bounds.IsEmpty(), false);

    transform = GraphicTransformType::GRAPHIC_FLIP_V_ROT180;
    visitor.CacRotationFromTransformType(transform, bounds);
    EXPECT_EQ(bounds.IsEmpty(), false);

    transform = GraphicTransformType::GRAPHIC_FLIP_H_ROT270;
    visitor.CacRotationFromTransformType(transform, bounds);
    EXPECT_EQ(bounds.IsEmpty(), false);

    transform = GraphicTransformType::GRAPHIC_FLIP_V_ROT270;
    visitor.CacRotationFromTransformType(transform, bounds);
    EXPECT_EQ(bounds.IsEmpty(), false);

    transform = GraphicTransformType::GRAPHIC_FLIP_H;
    visitor.CacRotationFromTransformType(transform, bounds);
    EXPECT_EQ(bounds.IsEmpty(), false);
}

/**
 * @tc.name: ProcessSurfaceViewInRT01
 * @tc.desc: test results of ProcessSurfaceViewInRT
 * @tc.type: FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RsRenderThreadVisitorTest, ProcessSurfaceViewInRT01, TestSize.Level1)
{
    RSRenderThreadVisitor visitor;
    RSSurfaceRenderNodeConfig config;
    RSSurfaceRenderNode node(config);
    Drawing::Canvas canvas;
    visitor.canvas_ = std::make_shared<RSPaintFilterCanvas>(&canvas);
    visitor.ProcessSurfaceViewInRT(node);
    EXPECT_TRUE(visitor.canvas_);
}

/**
 * @tc.name: ClipHoleForSurfaceNode01
 * @tc.desc: test results of ClipHoleForSurfaceNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RsRenderThreadVisitorTest, ClipHoleForSurfaceNode01, TestSize.Level1)
{
    RSRenderThreadVisitor visitor;
    RSSurfaceRenderNode node(0);
    auto& properties = node.GetMutableRenderProperties();
    properties.boundsGeo_->SetX(1.f);
    properties.boundsGeo_->SetY(1.f);
    properties.boundsGeo_->SetWidth(3.f);
    properties.boundsGeo_->SetHeight(3.f);
    Drawing::Canvas canvas;
    visitor.canvas_ = std::make_shared<RSPaintFilterCanvas>(&canvas);
    std::function<void(float, float, float, float)> fun = [](
        float a, float b, float c, float d) {};
    visitor.surfaceCallbacks_[0] = fun;
    visitor.ClipHoleForSurfaceNode(node);
    EXPECT_TRUE(!visitor.surfaceCallbacks_.empty());

    properties.decoration_ = std::make_optional<Decoration>();
    properties.decoration_->backgroundColor_ = RgbPalette::Transparent();
    visitor.ClipHoleForSurfaceNode(node);
    EXPECT_TRUE(!visitor.surfaceCallbacks_.empty());

    Color color(1, 1, 1, 1); // for test
    properties.decoration_->backgroundColor_ = color;
    visitor.ClipHoleForSurfaceNode(node);
    EXPECT_TRUE(!visitor.surfaceCallbacks_.empty());

    node.isNotifyRTBufferAvailable_ = true;
    visitor.ClipHoleForSurfaceNode(node);
    EXPECT_TRUE(node.isNotifyRTBufferAvailable_);
}

/**
 * @tc.name: SendCommandFromRT01
 * @tc.desc: test results of SendCommandFromRT
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RsRenderThreadVisitorTest, SendCommandFromRT01, TestSize.Level1)
{
    RSRenderThreadVisitor visitor;
    std::unique_ptr<RSCommand> command;
    visitor.SendCommandFromRT(command, 1, FollowType::NONE);
    EXPECT_TRUE(command == nullptr);
}

/**
 * @tc.name: ProcessOtherSurfaceRenderNode01
 * @tc.desc: test results of ProcessOtherSurfaceRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RsRenderThreadVisitorTest, ProcessOtherSurfaceRenderNode01, TestSize.Level1)
{
    RSRenderThreadVisitor visitor;
    RSSurfaceRenderNode node(0);
    Drawing::Canvas canvas;
    visitor.canvas_ = std::make_shared<RSPaintFilterCanvas>(&canvas);
    visitor.ProcessSurfaceRenderNode(node);
    EXPECT_EQ(visitor.childSurfaceNodeIds_.size(), 0);
}

/**
 * @tc.name: GetFlipTransform01
 * @tc.desc: test results of GetFlipTransform
 * @tc.type: FUNC
 * @tc.require: issueIAJ76O
 */
HWTEST_F(RsRenderThreadVisitorTest, GetFlipTransform01, TestSize.Level1)
{
    RSRenderThreadVisitor visitor;
    GraphicTransformType transform = GraphicTransformType::GRAPHIC_FLIP_H_ROT90;
    auto res = visitor.GetFlipTransform(transform);
    EXPECT_TRUE(res == GraphicTransformType::GRAPHIC_FLIP_H);

    transform = GraphicTransformType::GRAPHIC_FLIP_H_ROT180;
    res = visitor.GetFlipTransform(transform);
    EXPECT_TRUE(res == GraphicTransformType::GRAPHIC_FLIP_H);

    transform = GraphicTransformType::GRAPHIC_FLIP_H_ROT270;
    res = visitor.GetFlipTransform(transform);
    EXPECT_TRUE(res == GraphicTransformType::GRAPHIC_FLIP_H);

    transform = GraphicTransformType::GRAPHIC_FLIP_V_ROT90;
    res = visitor.GetFlipTransform(transform);
    EXPECT_TRUE(res == GraphicTransformType::GRAPHIC_FLIP_V);

    transform = GraphicTransformType::GRAPHIC_FLIP_V_ROT180;
    res = visitor.GetFlipTransform(transform);
    EXPECT_TRUE(res == GraphicTransformType::GRAPHIC_FLIP_V);

    transform = GraphicTransformType::GRAPHIC_FLIP_V_ROT270;
    res = visitor.GetFlipTransform(transform);
    EXPECT_TRUE(res == GraphicTransformType::GRAPHIC_FLIP_V);

    transform = GraphicTransformType::GRAPHIC_ROTATE_NONE;
    res = visitor.GetFlipTransform(transform);
    EXPECT_TRUE(res == GraphicTransformType::GRAPHIC_ROTATE_NONE);
}

/**
 * @tc.name: FlipMatrix01
 * @tc.desc: test results of FlipMatrix
 * @tc.type: FUNC
 * @tc.require: issueIAJ76O
 */
HWTEST_F(RsRenderThreadVisitorTest, FlipMatrix01, TestSize.Level1)
{
    RSRenderThreadVisitor visitor;
    GraphicTransformType transform = GraphicTransformType::GRAPHIC_ROTATE_NONE;
    Drawing::Matrix matrix;
    RectF bounds;
    visitor.FlipMatrix(transform, matrix, bounds);
    EXPECT_TRUE(transform == GraphicTransformType::GRAPHIC_ROTATE_NONE);

    transform = GraphicTransformType::GRAPHIC_FLIP_H;
    visitor.FlipMatrix(transform, matrix, bounds);
    EXPECT_TRUE(transform == GraphicTransformType::GRAPHIC_FLIP_H);

    transform = GraphicTransformType::GRAPHIC_FLIP_V;
    visitor.FlipMatrix(transform, matrix, bounds);
    EXPECT_TRUE(transform == GraphicTransformType::GRAPHIC_FLIP_V);
}
} // namespace OHOS::Rosen