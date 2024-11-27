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

} // namespace OHOS::Rosen