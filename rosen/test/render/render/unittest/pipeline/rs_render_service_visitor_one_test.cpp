/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#include "limit_number.h"
#include "rs_test_util.h"

#include "common/rs_obj_abs_geometry.h"
#include "pipeline/render_thread/rs_uni_render_engine.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_processor_factory.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/main_thread/rs_render_service_visitor.h"
#include "pipeline/rs_render_thread.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "screen_manager/rs_screen_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RsRenderServiceVisitorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RsRenderServiceVisitorTest::SetUpTestCase()
{
    RSTestUtil::InitRenderNodeGC();
}
void RsRenderServiceVisitorTest::TearDownTestCase() {}
void RsRenderServiceVisitorTest::SetUp()
{
    if (RSUniRenderJudgement::IsUniRender()) {
        auto& uniRenderThread = RSUniRenderThread::Instance();
        uniRenderThread.uniRenderEngine_ = std::make_shared<RSUniRenderEngine>();
    }
}
void RsRenderServiceVisitorTest::TearDown() {}

std::shared_ptr<RSRenderServiceVisitor> GetRenderServiceVisitor()
{
    auto visitor = std::make_shared<RSRenderServiceVisitor>();
    EXPECT_NE(visitor, nullptr);
    return visitor;
}

/**
 * @tc.name: PrepareDisplayRenderNode1
 * @tc.desc: Test RsRenderServiceVisitorTest.PrepareDisplayRenderNode
 * @tc.type: FUNC
 * @tc.require: issueIBCEA2
 */
HWTEST_F(RsRenderServiceVisitorTest, PrepareDisplayRenderNode1, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[1];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    ASSERT_NE(rsRenderServiceVisitor, nullptr);
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(nodeId, config);
    rsRenderServiceVisitor->PrepareDisplayRenderNode(*node);
}

/**
 * @tc.name: PrepareChildren1
 * @tc.desc: Test RsRenderServiceVisitorTest.PrepareChildren
 * @tc.type: FUNC
 * @tc.require: issueIBCEA2
 */
HWTEST_F(RsRenderServiceVisitorTest, PrepareChildren1, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig config;
    RSSurfaceRenderNode rsSurfaceRenderNode(config);
    rsSurfaceRenderNode.stagingRenderParams_ = std::make_unique<RSSurfaceRenderParams>(rsSurfaceRenderNode.GetId());
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    ASSERT_NE(rsRenderServiceVisitor, nullptr);
    rsRenderServiceVisitor->PrepareChildren(rsSurfaceRenderNode);
}

/**
 * @tc.name: PrepareDisplayRenderNode02
 * @tc.desc: Test RsRenderServiceVisitorTest.PrepareDisplayRenderNode
 * @tc.type: FUNC
 * @tc.require: issueIBCEA2
 */
HWTEST_F(RsRenderServiceVisitorTest, PrepareDisplayRenderNode02, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[2];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    ASSERT_NE(rsRenderServiceVisitor, nullptr);
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(nodeId, config);
    rsRenderServiceVisitor->PrepareDisplayRenderNode(*node);
}

/**
 * @tc.name: PrepareDisplayRenderNode03
 * @tc.desc: Test RsRenderServiceVisitorTest.PrepareDisplayRenderNode
 * @tc.type: FUNC
 * @tc.require: issueIBCEA2
 */
HWTEST_F(RsRenderServiceVisitorTest, PrepareDisplayRenderNode03, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[3];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    ASSERT_NE(rsRenderServiceVisitor, nullptr);
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(nodeId, config);
    rsRenderServiceVisitor->PrepareDisplayRenderNode(*node);
}

/**
 * @tc.name: PrepareDisplayRenderNode04
 * @tc.desc: Test RsRenderServiceVisitorTest.PrepareDisplayRenderNode
 * @tc.type: FUNC
 * @tc.require: issueIBCEA2
 */
HWTEST_F(RsRenderServiceVisitorTest, PrepareDisplayRenderNode04, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[4];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    ASSERT_NE(rsRenderServiceVisitor, nullptr);
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(nodeId, config);
    rsRenderServiceVisitor->PrepareDisplayRenderNode(*node);
}

/**
 * @tc.name: PrepareDisplayRenderNode05
 * @tc.desc: Test RsRenderServiceVisitorTest.PrepareDisplayRenderNode
 * @tc.type: FUNC
 * @tc.require: issueIBCEA2
 */
HWTEST_F(RsRenderServiceVisitorTest, PrepareDisplayRenderNode05, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[5];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    ASSERT_NE(rsRenderServiceVisitor, nullptr);
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(nodeId, config);
    rsRenderServiceVisitor->PrepareDisplayRenderNode(*node);
}

/**
 * @tc.name: PrepareDisplayRenderNode06
 * @tc.desc: Test RsRenderServiceVisitorTest.PrepareDisplayRenderNode
 * @tc.type: FUNC
 * @tc.require: issueIBCEA2
 */
HWTEST_F(RsRenderServiceVisitorTest, PrepareDisplayRenderNode06, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[6];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    ASSERT_NE(rsRenderServiceVisitor, nullptr);
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(nodeId, config);
    rsRenderServiceVisitor->PrepareDisplayRenderNode(*node);
}

/**
 * @tc.name: PrepareDisplayRenderNode07
 * @tc.desc: Test RsRenderServiceVisitorTest.PrepareDisplayRenderNode
 * @tc.type: FUNC
 * @tc.require: issueIBCEA2
 */
HWTEST_F(RsRenderServiceVisitorTest, PrepareDisplayRenderNode07, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[7];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    ASSERT_NE(rsRenderServiceVisitor, nullptr);
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(nodeId, config);
    rsRenderServiceVisitor->PrepareDisplayRenderNode(*node);
}

/**
 * @tc.name: PrepareDisplayRenderNode08
 * @tc.desc: Test RsRenderServiceVisitorTest.PrepareDisplayRenderNode
 * @tc.type: FUNC
 * @tc.require: issueIBCEA2
 */
HWTEST_F(RsRenderServiceVisitorTest, PrepareDisplayRenderNode08, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[0];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    ASSERT_NE(rsRenderServiceVisitor, nullptr);
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(nodeId, config);
    rsRenderServiceVisitor->PrepareDisplayRenderNode(*node);
}

/**
 * @tc.name: PrepareDisplayRenderNode09
 * @tc.desc: Test RsRenderServiceVisitorTest.PrepareDisplayRenderNode
 * @tc.type: FUNC
 * @tc.require: issueIBCEA2
 */
HWTEST_F(RsRenderServiceVisitorTest, PrepareDisplayRenderNode09, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[1];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    ASSERT_NE(rsRenderServiceVisitor, nullptr);
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(nodeId, config);
    auto& property = node->GetMutableRenderProperties();
    EXPECT_NE(&property, nullptr);
    property.SetBounds({ 0, 0, 40, 60 });
    property.SetFrameWidth(0);
    property.SetFrameHeight(0);
    auto& absGeo = (property.GetBoundsGeometry());
    absGeo->SetRotation(90);
    rsRenderServiceVisitor->PrepareDisplayRenderNode(*node);
}

/**
 * @tc.name: PrepareDisplayRenderNode10
 * @tc.desc: Test RsRenderServiceVisitorTest.PrepareDisplayRenderNode
 * @tc.type: FUNC
 * @tc.require: issueIBCEA2
 */
HWTEST_F(RsRenderServiceVisitorTest, PrepareDisplayRenderNode10, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[2];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    ASSERT_NE(rsRenderServiceVisitor, nullptr);
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(nodeId, config);
    auto& property = node->GetMutableRenderProperties();
    EXPECT_NE(&property, nullptr);
    property.SetBounds({ 0, 0, 40, 60 });
    property.SetFrameWidth(0);
    property.SetFrameHeight(0);
    auto& absGeo = (property.GetBoundsGeometry());
    absGeo->SetRotation(270);
    rsRenderServiceVisitor->PrepareDisplayRenderNode(*node);
}

/**
 * @tc.name: PrepareDisplayRenderNode11
 * @tc.desc: Test RsRenderServiceVisitorTest.PrepareDisplayRenderNode
 * @tc.type: FUNC
 * @tc.require: issueIBCEA2
 */
HWTEST_F(RsRenderServiceVisitorTest, PrepareDisplayRenderNode11, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[3];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    ASSERT_NE(rsRenderServiceVisitor, nullptr);
    RSDisplayNodeConfig config;
    config.isMirrored = true;
    auto node = std::make_shared<RSDisplayRenderNode>(nodeId, config);
    rsRenderServiceVisitor->PrepareDisplayRenderNode(*node);
}


/**
 * @tc.name: PrepareSurfaceRenderNode1
 * @tc.desc: Test RsRenderServiceVisitorTest.PrepareSurfaceRenderNode
 * @tc.type: FUNC
 * @tc.require: issueIBCEA2
 */
HWTEST_F(RsRenderServiceVisitorTest, PrepareSurfaceRenderNode1, TestSize.Level1)
{
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    ASSERT_NE(rsRenderServiceVisitor, nullptr);
    RSSurfaceRenderNodeConfig config;
    RSSurfaceRenderNode rsSurfaceRenderNode(config);
    rsRenderServiceVisitor->PrepareSurfaceRenderNode(rsSurfaceRenderNode);
}

/**
 * @tc.name: PrepareSurfaceRenderNode02
 * @tc.desc: Test RsRenderServiceVisitorTest.PrepareSurfaceRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI80HL4
 */
HWTEST_F(RsRenderServiceVisitorTest, PrepareSurfaceRenderNode02, TestSize.Level1)
{
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    ASSERT_NE(rsRenderServiceVisitor, nullptr);
    RSSurfaceRenderNodeConfig config;
    RSSurfaceRenderNode rsSurfaceRenderNode(config);
    rsRenderServiceVisitor->isSecurityDisplay_ = true;
    rsSurfaceRenderNode.SetSkipLayer(true);
    ASSERT_EQ(true, rsSurfaceRenderNode.GetSkipLayer());
    rsRenderServiceVisitor->PrepareSurfaceRenderNode(rsSurfaceRenderNode);
}

/**
 * @tc.name: PrepareDisplayRenderNode12
 * @tc.desc: Test RsRenderServiceVisitorTest.PrepareDisplayRenderNode
 * @tc.type: FUNC
 * @tc.require: issueIBCEA2
 */
HWTEST_F(RsRenderServiceVisitorTest, PrepareDisplayRenderNode12, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[4];
    RSRenderServiceVisitor rsRenderServiceVisitor(true);
    RSDisplayNodeConfig config;
    config.isMirrored = true;
    auto node = std::make_shared<RSDisplayRenderNode>(nodeId, config);
    ASSERT_NE(node, nullptr);
    rsRenderServiceVisitor.PrepareDisplayRenderNode(*node);
}

/**
 * @tc.name: PrepareDisplayRenderNode13
 * @tc.desc: Test RsRenderServiceVisitorTest.PrepareDisplayRenderNode
 * @tc.type: FUNC
 * @tc.require: issueIBCEA2
 */
HWTEST_F(RsRenderServiceVisitorTest, PrepareDisplayRenderNode13, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[5];
    RSRenderServiceVisitor rsRenderServiceVisitor(true);
    RSDisplayNodeConfig config;
    config.isMirrored = true;
    auto mirroredNode = std::make_shared<RSDisplayRenderNode>(nodeId, config);
    ASSERT_NE(mirroredNode, nullptr);
    RSDisplayNodeConfig config1;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(TestSrc::limitNumber::Uint64[6], config1);
    ASSERT_NE(displayNode, nullptr);
    mirroredNode->SetMirrorSource(displayNode);
    rsRenderServiceVisitor.PrepareDisplayRenderNode(*mirroredNode);
}

/**
 * @tc.name: PrepareCanvasRenderNode1
 * @tc.desc: Test RsRenderServiceVisitorTest.PrepareCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueIBCEA2
 */
HWTEST_F(RsRenderServiceVisitorTest, PrepareCanvasRenderNode1, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[1];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    ASSERT_NE(rsRenderServiceVisitor, nullptr);
    RSCanvasRenderNode node(nodeId);
    node.stagingRenderParams_ = std::make_unique<RSRenderParams>(node.GetId());
    rsRenderServiceVisitor->PrepareCanvasRenderNode(node);
}

/**
 * @tc.name: PrepareCanvasRenderNode02
 * @tc.desc: Test RsRenderServiceVisitorTest.PrepareCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueIBCEA2
 */
HWTEST_F(RsRenderServiceVisitorTest, PrepareCanvasRenderNode02, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[2];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    ASSERT_NE(rsRenderServiceVisitor, nullptr);
    RSCanvasRenderNode node(nodeId);
    rsRenderServiceVisitor->PrepareCanvasRenderNode(node);
}

/**
 * @tc.name: PrepareCanvasRenderNode03
 * @tc.desc: Test RsRenderServiceVisitorTest.PrepareCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueIBCEA2
 */
HWTEST_F(RsRenderServiceVisitorTest, PrepareCanvasRenderNode03, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[3];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    ASSERT_NE(rsRenderServiceVisitor, nullptr);
    RSCanvasRenderNode node(nodeId);
    rsRenderServiceVisitor->PrepareCanvasRenderNode(node);
}

/**
 * @tc.name: PrepareCanvasRenderNode04
 * @tc.desc: Test RsRenderServiceVisitorTest.PrepareCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueIBCEA2
 */
HWTEST_F(RsRenderServiceVisitorTest, PrepareCanvasRenderNode04, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[4];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    ASSERT_NE(rsRenderServiceVisitor, nullptr);
    RSCanvasRenderNode node(nodeId);
    rsRenderServiceVisitor->PrepareCanvasRenderNode(node);
}

/**
 * @tc.name: PrepareCanvasRenderNode05
 * @tc.desc: Test RsRenderServiceVisitorTest.PrepareCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueIBCEA2
 */
HWTEST_F(RsRenderServiceVisitorTest, PrepareCanvasRenderNode05, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[5];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    ASSERT_NE(rsRenderServiceVisitor, nullptr);
    RSCanvasRenderNode node(nodeId);
    rsRenderServiceVisitor->PrepareCanvasRenderNode(node);
}

/**
 * @tc.name: PrepareCanvasRenderNode06
 * @tc.desc: Test RsRenderServiceVisitorTest.PrepareCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueIBCEA2
 */
HWTEST_F(RsRenderServiceVisitorTest, PrepareCanvasRenderNode06, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[6];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    ASSERT_NE(rsRenderServiceVisitor, nullptr);
    RSCanvasRenderNode node(nodeId);
    rsRenderServiceVisitor->PrepareCanvasRenderNode(node);
}

/**
 * @tc.name: PrepareCanvasRenderNode07
 * @tc.desc: Test RsRenderServiceVisitorTest.PrepareCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueIBCEA2
 */
HWTEST_F(RsRenderServiceVisitorTest, PrepareCanvasRenderNode07, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[7];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    ASSERT_NE(rsRenderServiceVisitor, nullptr);
    RSCanvasRenderNode node(nodeId);
    rsRenderServiceVisitor->PrepareCanvasRenderNode(node);
}

/**
 * @tc.name: PrepareCanvasRenderNode08
 * @tc.desc: Test RsRenderServiceVisitorTest.PrepareCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueIBCEA2
 */
HWTEST_F(RsRenderServiceVisitorTest, PrepareCanvasRenderNode08, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[0];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    ASSERT_NE(rsRenderServiceVisitor, nullptr);
    RSCanvasRenderNode node(nodeId);
    rsRenderServiceVisitor->PrepareCanvasRenderNode(node);
}


/**
 * @tc.name: ProcessDisplayRenderNode1
 * @tc.desc: Test RsRenderServiceVisitorTest.ProcessDisplayRenderNode
 * @tc.type: FUNC
 * @tc.require: issueIBCEA2
 */
HWTEST_F(RsRenderServiceVisitorTest, ProcessDisplayRenderNode1, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[1];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    ASSERT_NE(rsRenderServiceVisitor, nullptr);
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(nodeId, config);
    node->stagingRenderParams_ = std::make_unique<RSRenderParams>(node->GetId());
    rsRenderServiceVisitor->ProcessDisplayRenderNode(*node);
}

/**
 * @tc.name: ProcessDisplayRenderNode02
 * @tc.desc: Test RsRenderServiceVisitorTest.ProcessDisplayRenderNode
 * @tc.type: FUNC
 * @tc.require: issueIBCEA2
 */
HWTEST_F(RsRenderServiceVisitorTest, ProcessDisplayRenderNode02, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[2];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    ASSERT_NE(rsRenderServiceVisitor, nullptr);
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(nodeId, config);
    node->stagingRenderParams_ = std::make_unique<RSRenderParams>(node->GetId());
    rsRenderServiceVisitor->ProcessDisplayRenderNode(*node);
}

/**
 * @tc.name: PrepareRootRenderNode1
 * @tc.desc: Test RsRenderServiceVisitorTest.PrepareRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueIBCEA2
 */
HWTEST_F(RsRenderServiceVisitorTest, PrepareRootRenderNode1, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[1];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    ASSERT_NE(rsRenderServiceVisitor, nullptr);
    RSRootRenderNode node(nodeId);
    rsRenderServiceVisitor->PrepareRootRenderNode(node);
}

/**
 * @tc.name: PrepareRootRenderNode02
 * @tc.desc: Test RsRenderServiceVisitorTest.PrepareRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueIBCEA2
 */
HWTEST_F(RsRenderServiceVisitorTest, PrepareRootRenderNode02, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[2];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    ASSERT_NE(rsRenderServiceVisitor, nullptr);
    RSRootRenderNode node(nodeId);
    rsRenderServiceVisitor->PrepareRootRenderNode(node);
}

/**
 * @tc.name: PrepareRootRenderNode03
 * @tc.desc: Test RsRenderServiceVisitorTest.PrepareRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueIBCEA2
 */
HWTEST_F(RsRenderServiceVisitorTest, PrepareRootRenderNode03, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[3];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    ASSERT_NE(rsRenderServiceVisitor, nullptr);
    RSRootRenderNode node(nodeId);
    rsRenderServiceVisitor->PrepareRootRenderNode(node);
}

/**
 * @tc.name: PrepareRootRenderNode04
 * @tc.desc: Test RsRenderServiceVisitorTest.PrepareRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueIBCEA2
 */
HWTEST_F(RsRenderServiceVisitorTest, PrepareRootRenderNode04, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[4];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    ASSERT_NE(rsRenderServiceVisitor, nullptr);
    RSRootRenderNode node(nodeId);
    rsRenderServiceVisitor->PrepareRootRenderNode(node);
}

/**
 * @tc.name: PrepareRootRenderNode05
 * @tc.desc: Test RsRenderServiceVisitorTest.PrepareRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueIBCEA2
 */
HWTEST_F(RsRenderServiceVisitorTest, PrepareRootRenderNode05, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[5];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    ASSERT_NE(rsRenderServiceVisitor, nullptr);
    RSRootRenderNode node(nodeId);
    rsRenderServiceVisitor->PrepareRootRenderNode(node);
}

/**
 * @tc.name: PrepareRootRenderNode06
 * @tc.desc: Test RsRenderServiceVisitorTest.PrepareRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueIBCEA2
 */
HWTEST_F(RsRenderServiceVisitorTest, PrepareRootRenderNode06, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[6];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    ASSERT_NE(rsRenderServiceVisitor, nullptr);
    RSRootRenderNode node(nodeId);
    rsRenderServiceVisitor->PrepareRootRenderNode(node);
}

/**
 * @tc.name: PrepareRootRenderNode07
 * @tc.desc: Test RsRenderServiceVisitorTest.PrepareRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueIBCEA2
 */
HWTEST_F(RsRenderServiceVisitorTest, PrepareRootRenderNode07, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[7];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    ASSERT_NE(rsRenderServiceVisitor, nullptr);
    RSRootRenderNode node(nodeId);
    rsRenderServiceVisitor->PrepareRootRenderNode(node);
}

/**
 * @tc.name: PrepareRootRenderNode08
 * @tc.desc: Test RsRenderServiceVisitorTest.PrepareRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueIBCEA2
 */
HWTEST_F(RsRenderServiceVisitorTest, PrepareRootRenderNode08, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[0];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    ASSERT_NE(rsRenderServiceVisitor, nullptr);
    RSRootRenderNode node(nodeId);
    rsRenderServiceVisitor->PrepareRootRenderNode(node);
}

/**
 * @tc.name: ProcessChildren1
 * @tc.desc: Test RsRenderServiceVisitorTest.ProcessChildren
 * @tc.type: FUNC
 * @tc.require: issueIBCEA2
 */
HWTEST_F(RsRenderServiceVisitorTest, ProcessChildren1, TestSize.Level1)
{
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    ASSERT_NE(rsRenderServiceVisitor, nullptr);
    RSSurfaceRenderNodeConfig config;
    RSSurfaceRenderNode rsSurfaceRenderNode(config);
    rsRenderServiceVisitor->ProcessChildren(rsSurfaceRenderNode);
}

/*
 * @tc.name: ProcessChildren02
 * @tc.desc: Test RsRenderServiceVisitorTest.ProcessChildren
 * @tc.type: FUNC
 * @tc.require: issueIBCEA2
 */
HWTEST_F(RsRenderServiceVisitorTest, ProcessChildren02, TestSize.Level1)
{
    RSRenderServiceVisitor rsRenderServiceVisitor(true);
    RSSurfaceRenderNodeConfig config;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config);
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    rsRenderServiceVisitor.ProcessChildren(*rsSurfaceRenderNode);
}

/*
 * @tc.name: UpdateSurfaceDirtyAndGlobalDirty003
 * @tc.desc: Test while current frame dirty
 * @tc.type: FUNC
 * @tc.require: issuesIBD3VF
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateSurfaceDirtyAndGlobalDirty003, TestSize.Level2)
{
    NodeId id = 1;
    RSDisplayNodeConfig displayConfig;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, displayConfig);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    auto hwcNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(hwcNode, nullptr);
    surfaceNode->AddChildHardwareEnabledNode(hwcNode);
    displayNode->GetAllMainAndLeashSurfaces().push_back(surfaceNode);

    RsCommonHook::Instance().SetHardwareEnabledByBackgroundAlphaFlag(true);
    RsCommonHook::Instance().SetHardwareEnabledByHwcnodeBelowSelfInAppFlag(true);

    auto dirtyManager = surfaceNode->GetDirtyManager();
    dirtyManager->SetCurrentFrameDirtyRect(DEFAULT_RECT);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curDisplayNode_ = displayNode;
    rsUniRenderVisitor->UpdateSurfaceDirtyAndGlobalDirty();
    ASSERT_FALSE(surfaceNode->isHardwareForcedDisabled_);
}

/*
 * @tc.name: UpdateSurfaceDirtyAndGlobalDirty004
 * @tc.desc: Test while app window node skip in calculate global dirty
 * @tc.type: FUNC
 * @tc.require: issuesIBD3VF
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateSurfaceDirtyAndGlobalDirty004, TestSize.Level2)
{
    NodeId id = 1;
    RSDisplayNodeConfig displayConfig;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, displayConfig);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    auto hwcNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(hwcNode, nullptr);
    surfaceNode->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    surfaceNode->AddChildHardwareEnabledNode(hwcNode);
    displayNode->GetAllMainAndLeashSurfaces().push_back(surfaceNode);

    RsCommonHook::Instance().SetHardwareEnabledByBackgroundAlphaFlag(true);
    RsCommonHook::Instance().SetHardwareEnabledByHwcnodeBelowSelfInAppFlag(true);

    auto dirtyManager = surfaceNode->GetDirtyManager();
    dirtyManager->SetCurrentFrameDirtyRect(DEFAULT_RECT);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curDisplayNode_ = displayNode;
    rsUniRenderVisitor->UpdateSurfaceDirtyAndGlobalDirty();
    ASSERT_FALSE(surfaceNode->isHardwareForcedDisabled_);
}

/**
 * @tc.name: IsNodeAboveInsideOfNodeBelow
 * @tc.desc: Test RSUnitRenderVisitorTest.IsNodeAboveInsideOfNodeBelow
 * @tc.type: FUNC
 * @tc.require: issuesIBD3VF
 */
HWTEST_F(RSUniRenderVisitorTest, IsNodeAboveInsideOfNodeBelow, TestSize.Level1)
{
    const RectI rectAbove;
    std::list<RectI> hwcNodeRectList;
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    ASSERT_FALSE(rsUniRenderVisitor->IsNodeAboveInsideOfNodeBelow(rectAbove, hwcNodeRectList));
}

/**
 * @tc.name: UpdateHardwareStateByHwcNodeBackgroundAlpha001
 * @tc.desc: Test RSUnitRenderVisitorTest.UpdateHardwareStateByHwcNodeBackgroundAlpha
 * @tc.type: FUNC
 * @tc.require: issuesIBD3VF
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHardwareStateByHwcNodeBackgroundAlpha001, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode, nullptr);

    std::vector<std::weak_ptr<RSSurfaceRenderNode>> hwcNodes;
    hwcNodes.push_back(std::weak_ptr<RSSurfaceRenderNode>(surfaceNode));
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->UpdateHardwareStateByHwcNodeBackgroundAlpha(hwcNodes);
}

/**
 * @tc.name: UpdateHardwareStateByHwcNodeBackgroundAlpha002
 * @tc.desc: Test RSUnitRenderVisitorTest.UpdateHardwareStateByHwcNodeBackgroundAlpha
 * @tc.type: FUNC
 * @tc.require: issuesIBD3VF
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHardwareStateByHwcNodeBackgroundAlpha002, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetNodeHasBackgroundColorAlpha(true);
    surfaceNode->SetHardwareForcedDisabledState(true);

    std::vector<std::weak_ptr<RSSurfaceRenderNode>> hwcNodes;
    hwcNodes.push_back(std::weak_ptr<RSSurfaceRenderNode>(surfaceNode));
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->UpdateHardwareStateByHwcNodeBackgroundAlpha(hwcNodes);
}

/**
 * @tc.name: UpdateHardwareStateByHwcNodeBackgroundAlpha003
 * @tc.desc: Test RSUnitRenderVisitorTest.UpdateHardwareStateByHwcNodeBackgroundAlpha
 * @tc.type: FUNC
 * @tc.require: issuesIBD3VF
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHardwareStateByHwcNodeBackgroundAlpha003, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    auto surfaceNode1 = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode1, nullptr);
    auto surfaceNode2 = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode2, nullptr);
    surfaceNode2->SetNodeHasBackgroundColorAlpha(true);

    std::vector<std::weak_ptr<RSSurfaceRenderNode>> hwcNodes;
    hwcNodes.push_back(std::weak_ptr<RSSurfaceRenderNode>(surfaceNode1));
    hwcNodes.push_back(std::weak_ptr<RSSurfaceRenderNode>(surfaceNode2));
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->UpdateHardwareStateByHwcNodeBackgroundAlpha(hwcNodes);
}

/*
 * @tc.name: CollectFilterInfoAndUpdateDirty001
 * @tc.desc: Verify function CollectFilterInfoAndUpdateDirty without curSurfaceNode_
 * @tc.type: FUNC
 * @tc.require: issuesIBD3VF
 */
HWTEST_F(RSUniRenderVisitorTest, CollectFilterInfoAndUpdateDirty001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    auto dirtyManager = surfaceNode->GetDirtyManager();
    ASSERT_NE(dirtyManager, nullptr);
    NodeId nodeId = 1;
    auto node = std::make_shared<RSRenderNode>(nodeId);
    RectI rect(0, 0, 0, 0);

    ASSERT_TRUE(rsUniRenderVisitor->containerFilter_.empty());
    rsUniRenderVisitor->CollectFilterInfoAndUpdateDirty(*node, *dirtyManager, rect);
    ASSERT_FALSE(rsUniRenderVisitor->containerFilter_.empty());
}

/*
 * @tc.name: CollectFilterInfoAndUpdateDirty002
 * @tc.desc: Verify function CollectFilterInfoAndUpdateDirty with non-transparent curSurfaceNode_
 * @tc.type: FUNC
 * @tc.require: issuesIBD3VF
 */
HWTEST_F(RSUniRenderVisitorTest, CollectFilterInfoAndUpdateDirty002, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    NodeId surfaceNodeId = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceNodeId);
    surfaceNode->SetAbilityBGAlpha(MAX_ALPHA);
    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;

    auto dirtyManager = surfaceNode->GetDirtyManager();
    ASSERT_NE(dirtyManager, nullptr);
    NodeId nodeId = surfaceNodeId + 1;
    auto node = std::make_shared<RSRenderNode>(nodeId);
    RectI rect(0, 0, 0, 0);

    ASSERT_TRUE(rsUniRenderVisitor->globalFilter_.empty());
    rsUniRenderVisitor->CollectFilterInfoAndUpdateDirty(*node, *dirtyManager, rect);
    ASSERT_FALSE(rsUniRenderVisitor->globalFilter_.empty());
}

/*
 * @tc.name: CollectFilterInfoAndUpdateDirty003
 * @tc.desc: Verify function CollectFilterInfoAndUpdateDirty with transparent curSurfaceNode_
 * @tc.type: FUNC
 * @tc.require: issuesIBD3VF
 */
HWTEST_F(RSUniRenderVisitorTest, CollectFilterInfoAndUpdateDirty003, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    NodeId surfaceNodeId = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceNodeId);
    surfaceNode->SetAbilityBGAlpha(0);
    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;

    auto dirtyManager = surfaceNode->GetDirtyManager();
    ASSERT_NE(dirtyManager, nullptr);
    NodeId nodeId = surfaceNodeId + 1;
    auto node = std::make_shared<RSRenderNode>(nodeId);
    RectI rect(0, 0, 0, 0);

    ASSERT_TRUE(rsUniRenderVisitor->transparentCleanFilter_.empty());
    rsUniRenderVisitor->CollectFilterInfoAndUpdateDirty(*node, *dirtyManager, rect);
    ASSERT_FALSE(rsUniRenderVisitor->transparentCleanFilter_.empty());
}

/*
 * @tc.name: CollectFilterInfoAndUpdateDirty004
 * @tc.desc: Verify function CollectFilterInfoAndUpdateDirty with transparent(needDrawBehindWindow) curSurfaceNode
 * @tc.type: FUNC
 * @tc.require: issuesIBD3VF
 */
HWTEST_F(RSUniRenderVisitorTest, CollectFilterInfoAndUpdateDirty004, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    NodeId surfaceNodeId = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceNodeId);
    surfaceNode->renderContent_->renderProperties_.needDrawBehindWindow_ = true;
    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;

    auto dirtyManager = surfaceNode->GetDirtyManager();
    ASSERT_NE(dirtyManager, nullptr);
    RectI dirtyRegion(0, 0, 100, 100);
    dirtyManager->currentFrameDirtyRegion_ = dirtyRegion;
    RectI globalFilterRect(0, 0, 20, 20);
    rsUniRenderVisitor->transparentCleanFilter_ = {};
    rsUniRenderVisitor->curDisplayDirtyManager_ = std::make_shared<RSDirtyRegionManager>();
    rsUniRenderVisitor->curDisplayDirtyManager_->currentFrameDirtyRegion_ = {};

    ASSERT_TRUE(rsUniRenderVisitor->transparentCleanFilter_.empty());
    rsUniRenderVisitor->CollectFilterInfoAndUpdateDirty(*surfaceNode, *dirtyManager, globalFilterRect);
    ASSERT_FALSE(rsUniRenderVisitor->transparentCleanFilter_.empty());
}

/**
 * @tc.name: UpdateHardwareStateByHwcNodeBackgroundAlpha004
 * @tc.desc: Test RSUnitRenderVisitorTest.UpdateHardwareStateByHwcNodeBackgroundAlpha
 * @tc.type: FUNC
 * @tc.require: issuesIBD3VF
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHardwareStateByHwcNodeBackgroundAlpha004, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetNodeHasBackgroundColorAlpha(true);

    std::vector<std::weak_ptr<RSSurfaceRenderNode>> hwcNodes;
    hwcNodes.push_back(std::weak_ptr<RSSurfaceRenderNode>(surfaceNode));
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->UpdateHardwareStateByHwcNodeBackgroundAlpha(hwcNodes);
}

/**
 * @tc.name: UpdateHardwareStateByHwcNodeBackgroundAlpha005
 * @tc.desc: Test RSUnitRenderVisitorTest.UpdateHardwareStateByHwcNodeBackgroundAlpha
 * @tc.type: FUNC
 * @tc.require: issuesIBD3VF
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHardwareStateByHwcNodeBackgroundAlpha005, TestSize.Level1)
{
    std::vector<std::weak_ptr<RSSurfaceRenderNode>> hwcNodes;
    std::weak_ptr<RSSurfaceRenderNode> hwcNode;
    hwcNodes.push_back(hwcNode);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->UpdateHardwareStateByHwcNodeBackgroundAlpha(hwcNodes);
}

/**
 * @tc.name: UpdateHwcNodeEnableByGlobalFilter
 * @tc.desc: Test UpdateHwcNodeEnableByGlobalFilter, child node force disabled hardware.
 * @tc.type: FUNC
 * @tc.require: issuesIBD3VF
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnableByGlobalFilter002, TestSize.Level1)
{
    // create input args.
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    // create display node and surface node.
    RSDisplayNodeConfig config;
    NodeId displayId = 1;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(displayId, config);
    NodeId surfaceId = 2;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceId);
    auto childNode = std::make_shared<RSSurfaceRenderNode>(++surfaceId);
    childNode->isHardwareForcedDisabled_ = true;
    surfaceNode->AddChildHardwareEnabledNode(childNode);

    displayNode->curMainAndLeashSurfaceNodes_.push_back(surfaceNode);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curDisplayNode_ = displayNode;
    rsUniRenderVisitor->UpdateHwcNodeEnableByGlobalFilter(node);
    ASSERT_TRUE(childNode->isHardwareForcedDisabled_);
}

/**
 * @tc.name: UpdateHwcNodeEnableByGlobalFilter
 * @tc.desc: Test UpdateHwcNodeEnableByGlobalFilter, dirty filter found.
 * @tc.type: FUNC
 * @tc.require: issuesIBD3VF
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnableByGlobalFilter003, TestSize.Level1)
{
    // create input args.
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    // create display node and surface node.
    RSDisplayNodeConfig config;
    NodeId displayId = 1;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(displayId, config);
    NodeId surfaceId = 2;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceId);
    auto childNode = std::make_shared<RSSurfaceRenderNode>(++surfaceId);
    childNode->isHardwareForcedDisabled_ = false;
    childNode->dstRect_ = DEFAULT_RECT;
    surfaceNode->AddChildHardwareEnabledNode(childNode);

    displayNode->curMainAndLeashSurfaceNodes_.push_back(surfaceNode);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curDisplayNode_ = displayNode;
    rsUniRenderVisitor->transparentDirtyFilter_[node->GetId()].push_back(std::pair(node->GetId(), DEFAULT_RECT));
    rsUniRenderVisitor->UpdateHwcNodeEnableByGlobalFilter(node);
    ASSERT_TRUE(childNode->isHardwareForcedDisabled_);
}

/**
 * @tc.name: UpdateHwcNodeEnableByBufferSize
 * @tc.desc: Test UpdateHwcNodeEnableByBufferSize with rosen-web node / non-rosen-web node.
 * @tc.type: FUNC
 * @tc.require: issuesIBD3VF
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnableByBufferSize, TestSize.Level1)
{
    // create input args.
    auto node1 = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto node2 = RSTestUtil::CreateSurfaceNodeWithBuffer();
    node1->name_ = "RosenWeb_test";

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->UpdateHwcNodeEnableByBufferSize(*node1);
    rsUniRenderVisitor->UpdateHwcNodeEnableByBufferSize(*node2);
}

/**
 * @tc.name: UpdateHwcNodeDirtyRegionForApp001
 * @tc.desc: Test UpdateHwcNodeDirtyRegionForApp, current frame enable status different from last frame.
 * @tc.type: FUNC
 * @tc.require: issuesIBD3VF
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeDirtyRegionForApp001, TestSize.Level1)
{
    // create input args.
    auto appNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    appNode->dirtyManager_ = std::make_shared<RSDirtyRegionManager>();

    auto hwcNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    hwcNode->dstRect_ = DEFAULT_RECT;

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->UpdateHwcNodeDirtyRegionForApp(appNode, hwcNode);
    ASSERT_FALSE(appNode->GetDirtyManager()->GetCurrentFrameDirtyRegion().IsEmpty());
}

/**
 * @tc.name: UpdateHwcNodeDirtyRegionForApp002
 * @tc.desc: Test UpdateHwcNodeDirtyRegionForApp, current frame enable status different from last frame.
 * @tc.type: FUNC
 * @tc.require: issuesIBD3VF
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeDirtyRegionForApp002, TestSize.Level1)
{
    // create input args.
    auto appNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    appNode->dirtyManager_ = std::make_shared<RSDirtyRegionManager>();

    auto hwcNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    hwcNode->dstRect_ = DEFAULT_RECT;
    hwcNode->isLastFrameHardwareEnabled_ = true;

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->hasMirrorDisplay_ = true;
    hwcNode->GetRSSurfaceHandler()->SetCurrentFrameBufferConsumed();
    rsUniRenderVisitor->UpdateHwcNodeDirtyRegionForApp(appNode, hwcNode);
    ASSERT_FALSE(appNode->GetDirtyManager()->GetCurrentFrameDirtyRegion().IsEmpty());
}

/**
 * @tc.name: ProcessRootRenderNode1
 * @tc.desc: Test RsRenderServiceVisitorTest.ProcessRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issuesIBD3VF
 */
HWTEST_F(RsRenderServiceVisitorTest, ProcessRootRenderNode1, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[1];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    ASSERT_NE(rsRenderServiceVisitor, nullptr);
    RSRootRenderNode node(nodeId);
    rsRenderServiceVisitor->ProcessRootRenderNode(node);
}

/**
 * @tc.name: ProcessRootRenderNode02
 * @tc.desc: Test RsRenderServiceVisitorTest.ProcessRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issuesIBD3VF
 */
HWTEST_F(RsRenderServiceVisitorTest, ProcessRootRenderNode02, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[2];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    ASSERT_NE(rsRenderServiceVisitor, nullptr);
    RSRootRenderNode node(nodeId);
    rsRenderServiceVisitor->ProcessRootRenderNode(node);
}

/**
 * @tc.name: ProcessRootRenderNode03
 * @tc.desc: Test RsRenderServiceVisitorTest.ProcessRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issuesIBD3VF
 */
HWTEST_F(RsRenderServiceVisitorTest, ProcessRootRenderNode03, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[3];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    ASSERT_NE(rsRenderServiceVisitor, nullptr);
    RSRootRenderNode node(nodeId);
    rsRenderServiceVisitor->ProcessRootRenderNode(node);
}

/**
 * @tc.name: UpdateHwcNodeEnableByGlobalFilter
 * @tc.desc: Test UpdateHwcNodeEnableByGlobalFilter nullptr / eqeual nodeid / hwcNodes empty.
 * @tc.type: FUNC
 * @tc.require: issuesIBD3VF
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnableByGlobalFilter001, TestSize.Level1)
{
    // create input args.
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    // create display node.
    RSDisplayNodeConfig config;
    NodeId displayId = 1;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(displayId, config);
    displayNode->curMainAndLeashSurfaceNodes_.push_back(nullptr);
    displayNode->curMainAndLeashSurfaceNodes_.push_back(std::make_shared<RSSurfaceRenderNode>(node->GetId()));

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curDisplayNode_ = displayNode;
    rsUniRenderVisitor->UpdateHwcNodeEnableByGlobalFilter(node);
}

/**
 * @tc.name: UpdateHwcNodeDirtyRegionAndCreateLayer001
 * @tc.desc: Test UpdateHwcNodeDirtyRegionAndCreateLayer with default surface node (empty child).
 * @tc.type: FUNC
 * @tc.require: issuesIBD3VF
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeDirtyRegionAndCreateLayer001, TestSize.Level1)
{
    // create input args.
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->UpdateHwcNodeDirtyRegionAndCreateLayer(node);
}

/**
 * @tc.name: UpdateHwcNodeDirtyRegionAndCreateLayer002
 * @tc.desc: Test UpdateHwcNodeDirtyRegionAndCreateLayer with off-tree/on-tree child node.
 * @tc.type: FUNC
 * @tc.require: issuesIBD3VF
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeDirtyRegionAndCreateLayer002, TestSize.Level1)
{
    // create input args.
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    NodeId childId = 1;
    auto childNode1 = std::make_shared<RSSurfaceRenderNode>(childId);
    childNode1->InitRenderParams();
    childNode1->SetIsOnTheTree(false);
    auto childNode2 = std::make_shared<RSSurfaceRenderNode>(++childId);
    childNode2->SetIsOnTheTree(true);
    childNode2->InitRenderParams();
    auto childNode3 = RSTestUtil::CreateSurfaceNode();
    childNode3->SetIsOnTheTree(true);
    childNode3->SetLayerTop(true);
    auto childNode4 = RSTestUtil::CreateSurfaceNodeWithBuffer();
    childNode4->SetIsOnTheTree(true);
    childNode4->SetLayerTop(true);
    node->AddChildHardwareEnabledNode(childNode1);
    node->AddChildHardwareEnabledNode(childNode2);
    node->AddChildHardwareEnabledNode(childNode3);
    node->AddChildHardwareEnabledNode(childNode4);

    RSDisplayNodeConfig config;
    NodeId displayId = 1;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(displayId, config);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->hasUniRenderHdrSurface_ = true;
    rsUniRenderVisitor->curDisplayNode_ = displayNode;
    rsUniRenderVisitor->UpdateHwcNodeDirtyRegionAndCreateLayer(node);
}

/**
 * @tc.name: ProcessRootRenderNode04
 * @tc.desc: Test RsRenderServiceVisitorTest.ProcessRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issuesIBD3VF
 */
HWTEST_F(RsRenderServiceVisitorTest, ProcessRootRenderNode04, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[4];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    ASSERT_NE(rsRenderServiceVisitor, nullptr);
    RSRootRenderNode node(nodeId);
    rsRenderServiceVisitor->ProcessRootRenderNode(node);
}

/**
 * @tc.name: ProcessRootRenderNode05
 * @tc.desc: Test RsRenderServiceVisitorTest.ProcessRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issuesIBD3VF
 */
HWTEST_F(RsRenderServiceVisitorTest, ProcessRootRenderNode05, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[5];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    ASSERT_NE(rsRenderServiceVisitor, nullptr);
    RSRootRenderNode node(nodeId);
    rsRenderServiceVisitor->ProcessRootRenderNode(node);
}

/**
 * @tc.name: ProcessRootRenderNode06
 * @tc.desc: Test RsRenderServiceVisitorTest.ProcessRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issuesIBD3VF
 */
HWTEST_F(RsRenderServiceVisitorTest, ProcessRootRenderNode06, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[6];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    ASSERT_NE(rsRenderServiceVisitor, nullptr);
    RSRootRenderNode node(nodeId);
    rsRenderServiceVisitor->ProcessRootRenderNode(node);
}

/**
 * @tc.name: ProcessRootRenderNode07
 * @tc.desc: Test RsRenderServiceVisitorTest.ProcessRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issuesIBD3VF
 */
HWTEST_F(RsRenderServiceVisitorTest, ProcessRootRenderNode07, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[7];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    ASSERT_NE(rsRenderServiceVisitor, nullptr);
    RSRootRenderNode node(nodeId);
    rsRenderServiceVisitor->ProcessRootRenderNode(node);
}

/**
 * @tc.name: ProcessRootRenderNode08
 * @tc.desc: Test RsRenderServiceVisitorTest.ProcessRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issuesIBD3VF
 */
HWTEST_F(RsRenderServiceVisitorTest, ProcessRootRenderNode08, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[0];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    ASSERT_NE(rsRenderServiceVisitor, nullptr);
    RSRootRenderNode node(nodeId);
    rsRenderServiceVisitor->ProcessRootRenderNode(node);
}

/**
 * @tc.name: SetAnimateState1
 * @tc.desc: Test RsRenderServiceVisitorTest.SetAnimateState
 * @tc.type: FUNC
 * @tc.require: issuesIBD3VF
 */
HWTEST_F(RsRenderServiceVisitorTest, SetAnimateState1, TestSize.Level1)
{
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    ASSERT_NE(rsRenderServiceVisitor, nullptr);
    rsRenderServiceVisitor->SetAnimateState(true);
    ASSERT_TRUE(rsRenderServiceVisitor->doAnimate_);
}

/**
 * @tc.name: ShouldForceSerial1
 * @tc.desc: Test RsRenderServiceVisitorTest.ShouldForceSerial
 * @tc.type: FUNC
 * @tc.require: issuesIBD3VF
 */
HWTEST_F(RsRenderServiceVisitorTest, ShouldForceSerial1, TestSize.Level1)
{
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    ASSERT_EQ(false, rsRenderServiceVisitor->ShouldForceSerial());
}
}
