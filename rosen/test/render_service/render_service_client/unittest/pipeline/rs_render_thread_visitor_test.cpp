/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderThreadVisitorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderThreadVisitorTest::SetUpTestCase() {}
void RSRenderThreadVisitorTest::TearDownTestCase() {}
void RSRenderThreadVisitorTest::SetUp() {}
void RSRenderThreadVisitorTest::TearDown() {}

/**
 * @tc.name: PrepareChildren001
 * @tc.desc: test results of PrepareChildren
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSRenderThreadVisitorTest, PrepareChildren001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. PrepareChildren
     */
    RSSurfaceRenderNodeConfig config;
    RSSurfaceRenderNode rsSurfaceRenderNode(config);
    std::shared_ptr rsRenderThreadVisitor = std::make_shared<RSRenderThreadVisitor>();
    rsRenderThreadVisitor->PrepareChildren(rsSurfaceRenderNode);
    config.id = 1;
    auto surfaceRenderNode2 = std::make_shared<RSSurfaceRenderNode>(config);
    rsSurfaceRenderNode.AddChild(surfaceRenderNode2, -1);
    rsRenderThreadVisitor->PrepareChildren(rsSurfaceRenderNode);
}

/**
 * @tc.name: PrepareCanvasRenderNode001
 * @tc.desc: test results of PrepareCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSRenderThreadVisitorTest, PrepareCanvasRenderNode001, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[1];
    auto node = std::make_shared<RSCanvasRenderNode>(nodeId);
    std::shared_ptr<RSRenderThreadVisitor> rsRenderThreadVisitor = std::make_shared<RSRenderThreadVisitor>();
    rsRenderThreadVisitor->PrepareCanvasRenderNode(*node);
}

/**
 * @tc.name: PrepareCanvasRenderNode002
 * @tc.desc: test results of PrepareCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSRenderThreadVisitorTest, PrepareCanvasRenderNode002, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[1];
    auto node = std::make_shared<RSCanvasRenderNode>(nodeId);
    node->GetMutableRenderProperties().SetAlpha(0);
    node->GetMutableRenderProperties().SetVisible(false);
    std::shared_ptr<RSRenderThreadVisitor> rsRenderThreadVisitor = std::make_shared<RSRenderThreadVisitor>();
    rsRenderThreadVisitor->PrepareCanvasRenderNode(*node);
}

/**
 * @tc.name: PrepareCanvasRenderNode003
 * @tc.desc: test results of PrepareCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSRenderThreadVisitorTest, PrepareCanvasRenderNode003, TestSize.Level1)
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
 * @tc.name: PrepareCanvasRenderNode004
 * @tc.desc: test results of PrepareCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSRenderThreadVisitorTest, PrepareCanvasRenderNode004, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[3];
    auto node = std::make_shared<RSCanvasRenderNode>(nodeId);
    auto rsRenderThreadVisitor = std::make_shared<RSRenderThreadVisitor>();
    rsRenderThreadVisitor->SetPartialRenderStatus(PartialRenderType::SET_DAMAGE_AND_DROP_OP, false);
    rsRenderThreadVisitor->PrepareCanvasRenderNode(*node);
}

/**
 * @tc.name: PrepareCanvasRenderNode005
 * @tc.desc: test results of PrepareCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSRenderThreadVisitorTest, PrepareCanvasRenderNode005, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[4];
    auto node = std::make_shared<RSCanvasRenderNode>(nodeId);
    auto rsRenderThreadVisitor = std::make_shared<RSRenderThreadVisitor>();
    rsRenderThreadVisitor->PrepareCanvasRenderNode(*node);
}

/**
 * @tc.name: PrepareCanvasRenderNode006
 * @tc.desc: test results of PrepareCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSRenderThreadVisitorTest, PrepareCanvasRenderNode006, TestSize.Level1)
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
 * @tc.name: PrepareRootRenderNode001
 * @tc.desc: test results of PrepareRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSRenderThreadVisitorTest, PrepareRootRenderNode001, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[1];
    auto node = std::make_shared<RSRootRenderNode>(nodeId);
    auto rsRenderThreadVisitor = std::make_shared<RSRenderThreadVisitor>();
    rsRenderThreadVisitor->PrepareRootRenderNode(*node);
}

/**
 * @tc.name: PrepareRootRenderNode002
 * @tc.desc: test results of PrepareRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSRenderThreadVisitorTest, PrepareRootRenderNode002, TestSize.Level1)
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
 * @tc.name: PrepareRootRenderNode003
 * @tc.desc: test results of PrepareRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSRenderThreadVisitorTest, PrepareRootRenderNode003, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[2];
    auto node = std::make_shared<RSRootRenderNode>(nodeId);
    auto rsRenderThreadVisitor = std::make_shared<RSRenderThreadVisitor>();
    rsRenderThreadVisitor->PrepareRootRenderNode(*node);
}

/**
 * @tc.name: PrepareRootRenderNode004
 * @tc.desc: test results of PrepareRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSRenderThreadVisitorTest, PrepareRootRenderNode004, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[3];
    auto node = std::make_shared<RSRootRenderNode>(nodeId);
    auto rsRenderThreadVisitor = std::make_shared<RSRenderThreadVisitor>();
    rsRenderThreadVisitor->PrepareRootRenderNode(*node);
}

/**
 * @tc.name: PrepareSurfaceRenderNode001
 * @tc.desc: test results of PrepareSurfaceRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSRenderThreadVisitorTest, PrepareSurfaceRenderNode001, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig config;
    auto surfaceRenderNode1 = std::make_shared<RSSurfaceRenderNode>(config);

    config.id = 1;
    auto surfaceRenderNode2 = std::make_shared<RSSurfaceRenderNode>(config);
    surfaceRenderNode1->AddChild(surfaceRenderNode2, -1);
    RSRenderThreadVisitor rsRenderThreadVisitor;
    rsRenderThreadVisitor.PrepareSurfaceRenderNode(*surfaceRenderNode2);
}

/**
 * @tc.name: PrepareSurfaceRenderNode002
 * @tc.desc: test results of PrepareSurfaceRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSRenderThreadVisitorTest, PrepareSurfaceRenderNode002, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig config;
    auto surfaceRenderNode1 = std::make_shared<RSSurfaceRenderNode>(config);

    config.id = 1;
    auto surfaceRenderNode2 = std::make_shared<RSSurfaceRenderNode>(config);
    surfaceRenderNode1->AddChild(surfaceRenderNode2, -1);
    RSRenderThreadVisitor rsRenderThreadVisitor;
    rsRenderThreadVisitor.PrepareSurfaceRenderNode(*surfaceRenderNode2);
    surfaceRenderNode2->GetMutableRenderProperties().SetClipToBounds(true);
    surfaceRenderNode2->GetMutableRenderProperties().SetBoundsWidth(10);
    surfaceRenderNode2->GetMutableRenderProperties().SetBoundsHeight(10);
    rsRenderThreadVisitor.PrepareSurfaceRenderNode(*surfaceRenderNode2);
}

/**
 * @tc.name: ProcessChildren001
 * @tc.desc: test results of ProcessChildren
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSRenderThreadVisitorTest, ProcessChildren001, TestSize.Level1)
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
 * @tc.name: ProcessCanvasRenderNode001
 * @tc.desc: test results of ProcessCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSRenderThreadVisitorTest, ProcessCanvasRenderNode001, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[1];
    RSCanvasRenderNode node(nodeId);
    node.GetMutableRenderProperties().SetAlpha(0);
    RSRenderThreadVisitor rsRenderThreadVisitor;
    rsRenderThreadVisitor.ProcessCanvasRenderNode(node);
}

/**
 * @tc.name: ProcessCanvasRenderNode002
 * @tc.desc: test results of ProcessCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSRenderThreadVisitorTest, ProcessCanvasRenderNode002, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[1];
    RSCanvasRenderNode node(nodeId);
    RSRenderThreadVisitor rsRenderThreadVisitor;
    rsRenderThreadVisitor.ProcessCanvasRenderNode(node);
}

/**
 * @tc.name: ProcessCanvasRenderNode003
 * @tc.desc: test results of ProcessCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSRenderThreadVisitorTest, ProcessCanvasRenderNode003, TestSize.Level1)
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
 * @tc.name: ProcessCanvasRenderNode004
 * @tc.desc: test results of ProcessCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSRenderThreadVisitorTest, ProcessCanvasRenderNode004, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[3];
    RSCanvasRenderNode node(nodeId);
    RSRenderThreadVisitor rsRenderThreadVisitor;
    rsRenderThreadVisitor.ProcessCanvasRenderNode(node);
}

/**
 * @tc.name: ProcessProxyRenderNode001
 * @tc.desc: test results of ProcessProxyRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSRenderThreadVisitorTest, ProcessProxyRenderNode, TestSize.Level1)
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
}

/**
 * @tc.name: ProcessRootRenderNode001
 * @tc.desc: test results of ProcessRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSRenderThreadVisitorTest, ProcessRootRenderNode001, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[1];
    RSRootRenderNode node(nodeId);
    RSRenderThreadVisitor rsRenderThreadVisitor;
    rsRenderThreadVisitor.ProcessRootRenderNode(node);
}

/**
 * @tc.name: ProcessRootRenderNode002
 * @tc.desc: test results of ProcessRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSRenderThreadVisitorTest, ProcessRootRenderNode002, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[1];
    RSRootRenderNode node(nodeId);
    node.UpdateSuggestedBufferSize(0, 0);
    RSRenderThreadVisitor rsRenderThreadVisitor;
    rsRenderThreadVisitor.ProcessRootRenderNode(node);
}

/**
 * @tc.name: ProcessRootRenderNode003
 * @tc.desc: test results of ProcessRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSRenderThreadVisitorTest, ProcessRootRenderNode003, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[1];
    RSRootRenderNode node(nodeId);
    node.SetEnableRender(false);
    RSRenderThreadVisitor rsRenderThreadVisitor;
    rsRenderThreadVisitor.ProcessRootRenderNode(node);
}

/**
 * @tc.name: ProcessRootRenderNode004
 * @tc.desc: test results of ProcessRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSRenderThreadVisitorTest, ProcessRootRenderNode004, TestSize.Level1)
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
 * @tc.name: ProcessRootRenderNode005
 * @tc.desc: test results of ProcessRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSRenderThreadVisitorTest, ProcessRootRenderNode005, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[3];
    RSRootRenderNode node(nodeId);
    node.SetEnableRender(false);
    RSRenderThreadVisitor rsRenderThreadVisitor;
    rsRenderThreadVisitor.ProcessRootRenderNode(node);
}

/**
 * @tc.name: ProcessRootRenderNode006
 * @tc.desc: test results of ProcessRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSRenderThreadVisitorTest, ProcessRootRenderNode006, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[4];
    RSRootRenderNode node(nodeId);
    node.SetEnableRender(false);
    RSRenderThreadVisitor rsRenderThreadVisitor;
    rsRenderThreadVisitor.ProcessRootRenderNode(node);
}

/**
 * @tc.name: ProcessRootRenderNode007
 * @tc.desc: test results of ProcessRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSRenderThreadVisitorTest, ProcessRootRenderNode007, TestSize.Level1)
{
    system::SetParameter("rosen.uni.partialrender.enabled", "0");
    int param = (int)RSSystemProperties::GetDirtyRegionDebugType();
    ASSERT_EQ(param, 0);
    RSSurfaceNodeConfig surfaceNodeConfig;
    auto surfacenode = RSSurfaceNode::Create(surfaceNodeConfig);
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[4];
    auto rootnode = std::make_shared<RSRootRenderNode>(nodeId);
    rootnode->AttachRSSurfaceNode(surfacenode->GetId());
    RSNodeMap::MutableInstance().RegisterNode(surfacenode);
    auto rsRenderThreadVisitor = std::make_shared<RSRenderThreadVisitor>();
    rootnode->Prepare(rsRenderThreadVisitor);
    rootnode->Process(rsRenderThreadVisitor);
    rsRenderThreadVisitor->SetPartialRenderStatus(PartialRenderType::SET_DAMAGE, true);
    rootnode->UpdateSuggestedBufferSize(10, 10);
    constexpr NodeId nodeId2 = TestSrc::limitNumber::Uint64[1];
    auto canvasnode = std::make_shared<RSCanvasRenderNode>(nodeId2);
    canvasnode->GetMutableRenderProperties().SetAlpha(1.f);
    rootnode->AddChild(canvasnode, -1);
    RSSurfaceRenderNodeConfig config;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config);
    rsSurfaceRenderNode->GetMutableRenderProperties().SetAlpha(1.f);
    rsSurfaceRenderNode->NotifyRTBufferAvailable();
    canvasnode->AddChild(rsSurfaceRenderNode, -1);
    rootnode->Prepare(rsRenderThreadVisitor);
    rootnode->Process(rsRenderThreadVisitor);
}

/**
 * @tc.name: ProcessSurfaceRenderNode001
 * @tc.desc: test results of ProcessSurfaceRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSRenderThreadVisitorTest, ProcessSurfaceRenderNode001, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig config;
    RSSurfaceRenderNode rsSurfaceRenderNode(config);
    RSRenderThreadVisitor rsRenderThreadVisitor;
    rsRenderThreadVisitor.ProcessSurfaceRenderNode(rsSurfaceRenderNode);
}

/**
 * @tc.name: ProcessSurfaceRenderNode002
 * @tc.desc: test results of ProcessSurfaceRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSRenderThreadVisitorTest, ProcessSurfaceRenderNode002, TestSize.Level1)
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
 * @tc.name: ProcessSurfaceRenderNode003
 * @tc.desc: test results of ProcessSurfaceRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSRenderThreadVisitorTest, ProcessSurfaceRenderNode003, TestSize.Level1)
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
 * @tc.name: SetPartialRenderStatus001
 * @tc.desc: test results of GetPartialRenderEnabled
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSRenderThreadVisitorTest, SetPartialRenderStatus001, TestSize.Level1)
{
    RSRenderThreadVisitor rsRenderThreadVisitor;
    rsRenderThreadVisitor.SetPartialRenderStatus(RSSystemProperties::GetPartialRenderEnabled(), false);
}

/**
 * @tc.name: SetPartialRenderStatus002
 * @tc.desc: test results of GetPartialRenderEnabled
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSRenderThreadVisitorTest, SetPartialRenderStatus002, TestSize.Level1)
{
    RSRenderThreadVisitor rsRenderThreadVisitor;
    rsRenderThreadVisitor.SetPartialRenderStatus(RSSystemProperties::GetPartialRenderEnabled(), true);
}

/**
 * @tc.name: SetPartialRenderStatus003
 * @tc.desc: test results of GetPartialRenderEnabled
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSRenderThreadVisitorTest, SetPartialRenderStatus003, TestSize.Level1)
{
    RSRenderThreadVisitor rsRenderThreadVisitor;
    rsRenderThreadVisitor.SetPartialRenderStatus(RSSystemProperties::GetUniPartialRenderEnabled(), true);
}

/**
 * @tc.name: SetPartialRenderStatus004
 * @tc.desc: test results of GetPartialRenderEnabled
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSRenderThreadVisitorTest, SetPartialRenderStatus004, TestSize.Level1)
{
    RSRenderThreadVisitor rsRenderThreadVisitor;
    rsRenderThreadVisitor.SetPartialRenderStatus(RSSystemProperties::GetUniPartialRenderEnabled(), false);
}

/**
 * @tc.name: SetPartialRenderStatus005
 * @tc.desc: test results of GetPartialRenderEnabled
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSRenderThreadVisitorTest, SetPartialRenderStatus005, TestSize.Level1)
{
    RSRenderThreadVisitor rsRenderThreadVisitor;
    rsRenderThreadVisitor.SetPartialRenderStatus(RSSystemProperties::GetUniPartialRenderEnabled(), true);
}

/**
 * @tc.name: SetPartialRenderStatus006
 * @tc.desc: test results of GetPartialRenderEnabled
 * @tc.type: FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSRenderThreadVisitorTest, SetPartialRenderStatus006, TestSize.Level1)
{
    RSRenderThreadVisitor rsRenderThreadVisitor;
    rsRenderThreadVisitor.SetPartialRenderStatus(PartialRenderType::DISABLED, true);
    rsRenderThreadVisitor.SetPartialRenderStatus(PartialRenderType::SET_DAMAGE_AND_DROP_OP, true);
}
} // namespace OHOS::Rosen