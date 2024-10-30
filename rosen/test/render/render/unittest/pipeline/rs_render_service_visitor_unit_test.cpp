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
#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_processor_factory.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_render_service_visitor.h"
#include "pipeline/rs_render_thread.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_uni_render_engine.h"
#include "pipeline/rs_uni_render_thread.h"
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
/*
 * @tc.name: CreateAndDestroy1
 * @tc.desc: Test RsRenderServiceVisitorTest.RSRenderServiceVisitor
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RsRenderServiceVisitorTest, CreateAndDestroy1, TestSize.Level1)
{
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
}

/**
 * @tc.name: PrepareDisplayRenderNode1
 * @tc.desc: Test RsRenderServiceVisitorTest.PrepareDisplayRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RsRenderServiceVisitorTest, PrepareDisplayRenderNode1, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[1];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(nodeId, config);
    rsRenderServiceVisitor->PrepareDisplayRenderNode(*node);
}

/**
 * @tc.name: PrepareChildren1
 * @tc.desc: Test RsRenderServiceVisitorTest.PrepareChildren
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RsRenderServiceVisitorTest, PrepareChildren1, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig config;
    RSSurfaceRenderNode rsSurfaceRenderNode(config);
    rsSurfaceRenderNode.stagingRenderParams_ = std::make_unique<RSSurfaceRenderParams>(rsSurfaceRenderNode.GetId());
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    rsRenderServiceVisitor->PrepareChildren(rsSurfaceRenderNode);
}

/**
 * @tc.name: PrepareDisplayRenderNode02
 * @tc.desc: Test RsRenderServiceVisitorTest.PrepareDisplayRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RsRenderServiceVisitorTest, PrepareDisplayRenderNode02, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[2];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(nodeId, config);
    rsRenderServiceVisitor->PrepareDisplayRenderNode(*node);
}

/**
 * @tc.name: PrepareDisplayRenderNode03
 * @tc.desc: Test RsRenderServiceVisitorTest.PrepareDisplayRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RsRenderServiceVisitorTest, PrepareDisplayRenderNode03, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[3];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(nodeId, config);
    rsRenderServiceVisitor->PrepareDisplayRenderNode(*node);
}

/**
 * @tc.name: PrepareDisplayRenderNode04
 * @tc.desc: Test RsRenderServiceVisitorTest.PrepareDisplayRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RsRenderServiceVisitorTest, PrepareDisplayRenderNode04, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[4];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(nodeId, config);
    rsRenderServiceVisitor->PrepareDisplayRenderNode(*node);
}

/**
 * @tc.name: PrepareDisplayRenderNode05
 * @tc.desc: Test RsRenderServiceVisitorTest.PrepareDisplayRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RsRenderServiceVisitorTest, PrepareDisplayRenderNode05, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[5];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(nodeId, config);
    rsRenderServiceVisitor->PrepareDisplayRenderNode(*node);
}

/**
 * @tc.name: PrepareDisplayRenderNode06
 * @tc.desc: Test RsRenderServiceVisitorTest.PrepareDisplayRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RsRenderServiceVisitorTest, PrepareDisplayRenderNode06, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[6];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(nodeId, config);
    rsRenderServiceVisitor->PrepareDisplayRenderNode(*node);
}

/**
 * @tc.name: PrepareDisplayRenderNode07
 * @tc.desc: Test RsRenderServiceVisitorTest.PrepareDisplayRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RsRenderServiceVisitorTest, PrepareDisplayRenderNode07, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[7];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(nodeId, config);
    rsRenderServiceVisitor->PrepareDisplayRenderNode(*node);
}

/**
 * @tc.name: PrepareDisplayRenderNode08
 * @tc.desc: Test RsRenderServiceVisitorTest.PrepareDisplayRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RsRenderServiceVisitorTest, PrepareDisplayRenderNode08, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[0];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(nodeId, config);
    rsRenderServiceVisitor->PrepareDisplayRenderNode(*node);
}

/**
 * @tc.name: PrepareDisplayRenderNode09
 * @tc.desc: Test RsRenderServiceVisitorTest.PrepareDisplayRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RsRenderServiceVisitorTest, PrepareDisplayRenderNode09, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[1];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
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
 * @tc.require: issueI614P1
 */
HWTEST_F(RsRenderServiceVisitorTest, PrepareDisplayRenderNode10, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[2];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
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
 * @tc.require: issueI614P1
 */
HWTEST_F(RsRenderServiceVisitorTest, PrepareDisplayRenderNode11, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[3];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSDisplayNodeConfig config;
    config.isMirrored = true;
    auto node = std::make_shared<RSDisplayRenderNode>(nodeId, config);
    rsRenderServiceVisitor->PrepareDisplayRenderNode(*node);
}

/**
 * @tc.name: PrepareDisplayRenderNode12
 * @tc.desc: Test RsRenderServiceVisitorTest.PrepareDisplayRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
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
 * @tc.require: issueI614P1
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
 * @tc.name: PrepareSurfaceRenderNode1
 * @tc.desc: Test RsRenderServiceVisitorTest.PrepareSurfaceRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RsRenderServiceVisitorTest, PrepareSurfaceRenderNode1, TestSize.Level1)
{
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
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
    RSSurfaceRenderNodeConfig config;
    RSSurfaceRenderNode rsSurfaceRenderNode(config);
    rsRenderServiceVisitor->isSecurityDisplay_ = true;
    rsSurfaceRenderNode.SetSkipLayer(true);
    ASSERT_EQ(true, rsSurfaceRenderNode.GetSkipLayer());
    rsRenderServiceVisitor->PrepareSurfaceRenderNode(rsSurfaceRenderNode);
}

/**
 * @tc.name: PrepareCanvasRenderNode1
 * @tc.desc: Test RsRenderServiceVisitorTest.PrepareCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RsRenderServiceVisitorTest, PrepareCanvasRenderNode1, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[1];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSCanvasRenderNode node(nodeId);
    node.stagingRenderParams_ = std::make_unique<RSRenderParams>(node.GetId());
    rsRenderServiceVisitor->PrepareCanvasRenderNode(node);
}

/**
 * @tc.name: PrepareCanvasRenderNode02
 * @tc.desc: Test RsRenderServiceVisitorTest.PrepareCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RsRenderServiceVisitorTest, PrepareCanvasRenderNode02, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[2];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSCanvasRenderNode node(nodeId);
    rsRenderServiceVisitor->PrepareCanvasRenderNode(node);
}

/**
 * @tc.name: PrepareCanvasRenderNode03
 * @tc.desc: Test RsRenderServiceVisitorTest.PrepareCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RsRenderServiceVisitorTest, PrepareCanvasRenderNode03, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[3];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSCanvasRenderNode node(nodeId);
    rsRenderServiceVisitor->PrepareCanvasRenderNode(node);
}

/**
 * @tc.name: PrepareCanvasRenderNode04
 * @tc.desc: Test RsRenderServiceVisitorTest.PrepareCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RsRenderServiceVisitorTest, PrepareCanvasRenderNode04, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[4];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSCanvasRenderNode node(nodeId);
    rsRenderServiceVisitor->PrepareCanvasRenderNode(node);
}

/**
 * @tc.name: PrepareCanvasRenderNode05
 * @tc.desc: Test RsRenderServiceVisitorTest.PrepareCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RsRenderServiceVisitorTest, PrepareCanvasRenderNode05, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[5];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSCanvasRenderNode node(nodeId);
    rsRenderServiceVisitor->PrepareCanvasRenderNode(node);
}

/**
 * @tc.name: PrepareCanvasRenderNode06
 * @tc.desc: Test RsRenderServiceVisitorTest.PrepareCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RsRenderServiceVisitorTest, PrepareCanvasRenderNode06, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[6];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSCanvasRenderNode node(nodeId);
    rsRenderServiceVisitor->PrepareCanvasRenderNode(node);
}

/**
 * @tc.name: PrepareCanvasRenderNode07
 * @tc.desc: Test RsRenderServiceVisitorTest.PrepareCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RsRenderServiceVisitorTest, PrepareCanvasRenderNode07, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[7];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSCanvasRenderNode node(nodeId);
    rsRenderServiceVisitor->PrepareCanvasRenderNode(node);
}

/**
 * @tc.name: PrepareCanvasRenderNode08
 * @tc.desc: Test RsRenderServiceVisitorTest.PrepareCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1r:
 */
HWTEST_F(RsRenderServiceVisitorTest, PrepareCanvasRenderNode08, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[0];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSCanvasRenderNode node(nodeId);
    rsRenderServiceVisitor->PrepareCanvasRenderNode(node);
}

/**
 * @tc.name: PrepareRootRenderNode1
 * @tc.desc: Test RsRenderServiceVisitorTest.PrepareRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RsRenderServiceVisitorTest, PrepareRootRenderNode1, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[1];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSRootRenderNode node(nodeId);
    rsRenderServiceVisitor->PrepareRootRenderNode(node);
}

/**
 * @tc.name: PrepareRootRenderNode02
 * @tc.desc: Test RsRenderServiceVisitorTest.PrepareRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RsRenderServiceVisitorTest, PrepareRootRenderNode02, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[2];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSRootRenderNode node(nodeId);
    rsRenderServiceVisitor->PrepareRootRenderNode(node);
}

/**
 * @tc.name: PrepareRootRenderNode03
 * @tc.desc: Test RsRenderServiceVisitorTest.PrepareRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RsRenderServiceVisitorTest, PrepareRootRenderNode03, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[3];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSRootRenderNode node(nodeId);
    rsRenderServiceVisitor->PrepareRootRenderNode(node);
}

/**
 * @tc.name: PrepareRootRenderNode04
 * @tc.desc: Test RsRenderServiceVisitorTest.PrepareRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RsRenderServiceVisitorTest, PrepareRootRenderNode04, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[4];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSRootRenderNode node(nodeId);
    rsRenderServiceVisitor->PrepareRootRenderNode(node);
}

/**
 * @tc.name: PrepareRootRenderNode05
 * @tc.desc: Test RsRenderServiceVisitorTest.PrepareRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RsRenderServiceVisitorTest, PrepareRootRenderNode05, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[5];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSRootRenderNode node(nodeId);
    rsRenderServiceVisitor->PrepareRootRenderNode(node);
}

/**
 * @tc.name: PrepareRootRenderNode06
 * @tc.desc: Test RsRenderServiceVisitorTest.PrepareRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RsRenderServiceVisitorTest, PrepareRootRenderNode06, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[6];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSRootRenderNode node(nodeId);
    rsRenderServiceVisitor->PrepareRootRenderNode(node);
}

/**
 * @tc.name: PrepareRootRenderNode07
 * @tc.desc: Test RsRenderServiceVisitorTest.PrepareRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RsRenderServiceVisitorTest, PrepareRootRenderNode07, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[7];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSRootRenderNode node(nodeId);
    rsRenderServiceVisitor->PrepareRootRenderNode(node);
}

/**
 * @tc.name: PrepareRootRenderNode08
 * @tc.desc: Test RsRenderServiceVisitorTest.PrepareRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RsRenderServiceVisitorTest, PrepareRootRenderNode08, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[0];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSRootRenderNode node(nodeId);
    rsRenderServiceVisitor->PrepareRootRenderNode(node);
}

/**
 * @tc.name: ProcessChildren1
 * @tc.desc: Test RsRenderServiceVisitorTest.ProcessChildren
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RsRenderServiceVisitorTest, ProcessChildren1, TestSize.Level1)
{
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSSurfaceRenderNodeConfig config;
    RSSurfaceRenderNode rsSurfaceRenderNode(config);
    rsRenderServiceVisitor->ProcessChildren(rsSurfaceRenderNode);
}

/*
 * @tc.name: ProcessChildren02
 * @tc.desc: Test RsRenderServiceVisitorTest.ProcessChildren
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RsRenderServiceVisitorTest, ProcessChildren02, TestSize.Level1)
{
    RSRenderServiceVisitor rsRenderServiceVisitor(true);
    RSSurfaceRenderNodeConfig config;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config);
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    rsRenderServiceVisitor.ProcessChildren(*rsSurfaceRenderNode);
}

/**
 * @tc.name: ProcessDisplayRenderNode1
 * @tc.desc: Test RsRenderServiceVisitorTest.ProcessDisplayRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RsRenderServiceVisitorTest, ProcessDisplayRenderNode1, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[1];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(nodeId, config);
    node->stagingRenderParams_ = std::make_unique<RSRenderParams>(node->GetId());
    rsRenderServiceVisitor->ProcessDisplayRenderNode(*node);
}

/**
 * @tc.name: ProcessDisplayRenderNode02
 * @tc.desc: Test RsRenderServiceVisitorTest.ProcessDisplayRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RsRenderServiceVisitorTest, ProcessDisplayRenderNode02, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[2];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(nodeId, config);
    node->stagingRenderParams_ = std::make_unique<RSRenderParams>(node->GetId());
    rsRenderServiceVisitor->ProcessDisplayRenderNode(*node);
}

/**
 * @tc.name: ProcessDisplayRenderNode03
 * @tc.desc: Test RsRenderServiceVisitorTest.ProcessDisplayRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RsRenderServiceVisitorTest, ProcessDisplayRenderNode03, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[3];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(nodeId, config);
    rsRenderServiceVisitor->ProcessDisplayRenderNode(*node);
}

/**
 * @tc.name: ProcessDisplayRenderNode04
 * @tc.desc: Test RsRenderServiceVisitorTest.ProcessDisplayRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RsRenderServiceVisitorTest, ProcessDisplayRenderNode04, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[4];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(nodeId, config);
    rsRenderServiceVisitor->ProcessDisplayRenderNode(*node);
}

/**
 * @tc.name: ProcessDisplayRenderNode05
 * @tc.desc: Test RsRenderServiceVisitorTest.ProcessDisplayRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RsRenderServiceVisitorTest, ProcessDisplayRenderNode05, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[5];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(nodeId, config);
    rsRenderServiceVisitor->ProcessDisplayRenderNode(*node);
}

/**
 * @tc.name: ProcessDisplayRenderNode06
 * @tc.desc: Test RsRenderServiceVisitorTest.ProcessDisplayRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RsRenderServiceVisitorTest, ProcessDisplayRenderNode06, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[6];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(nodeId, config);
    rsRenderServiceVisitor->ProcessDisplayRenderNode(*node);
}

/**
 * @tc.name: ProcessDisplayRenderNode07
 * @tc.desc: Test RsRenderServiceVisitorTest.ProcessDisplayRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RsRenderServiceVisitorTest, ProcessDisplayRenderNode07, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[7];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(nodeId, config);
    rsRenderServiceVisitor->ProcessDisplayRenderNode(*node);
}

/**
 * @tc.name: ProcessDisplayRenderNode08
 * @tc.desc: Test RsRenderServiceVisitorTest.ProcessDisplayRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RsRenderServiceVisitorTest, ProcessDisplayRenderNode08, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[0];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(nodeId, config);
    rsRenderServiceVisitor->ProcessDisplayRenderNode(*node);
}

/**
 * @tc.name: ProcessDisplayRenderNode09
 * @tc.desc: Test RsRenderServiceVisitorTest.ProcessDisplayRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RsRenderServiceVisitorTest, ProcessDisplayRenderNode09, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[0];
    RSRenderServiceVisitor rsRenderServiceVisitor(true);
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(screenManager, nullptr);
    auto rsScreen = std::make_unique<impl::RSScreen>(0, false, HdiOutput::CreateHdiOutput(0), nullptr);
    ASSERT_NE(rsScreen, nullptr);
    auto csurface = IConsumerSurface::Create();
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    rsScreen->SetProducerSurface(psurface);
    rsScreen->SetScreenSkipFrameInterval(1);
    screenManager->MockHdiScreenConnected(rsScreen);

    RSDisplayNodeConfig config;
    config.screenId = 0;
    auto node = std::make_shared<RSDisplayRenderNode>(nodeId, config);
    rsRenderServiceVisitor.ProcessDisplayRenderNode(*node);
}

/**
 * @tc.name: ProcessDisplayRenderNode10
 * @tc.desc: Test RsRenderServiceVisitorTest.ProcessDisplayRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RsRenderServiceVisitorTest, ProcessDisplayRenderNode10, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[0];
    RSRenderServiceVisitor rsRenderServiceVisitor(true);
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(screenManager, nullptr);

    auto rsScreen = std::make_unique<impl::RSScreen>(0, false, HdiOutput::CreateHdiOutput(0), nullptr);
    auto csurface = IConsumerSurface::Create();
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    rsScreen->SetProducerSurface(psurface);
    rsScreen->SetScreenSkipFrameInterval(1);
    screenManager->MockHdiScreenConnected(rsScreen);

    RSDisplayNodeConfig config;
    config.isMirrored = true;
    auto mirroredNode = std::make_shared<RSDisplayRenderNode>(nodeId, config);
    RSDisplayNodeConfig config1;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(TestSrc::limitNumber::Uint64[6], config1);
    mirroredNode->SetMirrorSource(displayNode);
    rsRenderServiceVisitor.ProcessDisplayRenderNode(*mirroredNode);
}

/**
 * @tc.name: ProcessSurfaceRenderNode1
 * @tc.desc: Test RsRenderServiceVisitorTest.ProcessSurfaceRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RsRenderServiceVisitorTest, ProcessSurfaceRenderNode1, TestSize.Level1)
{
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSSurfaceRenderNodeConfig config;
    RSSurfaceRenderNode rsSurfaceRenderNode(config);
    rsRenderServiceVisitor->ProcessSurfaceRenderNode(rsSurfaceRenderNode);
}

/**
 * @tc.name: ProcessSurfaceRenderNode02
 * @tc.desc: Test RsRenderServiceVisitorTest.ProcessSurfaceRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RsRenderServiceVisitorTest, ProcessSurfaceRenderNode02, TestSize.Level1)
{
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSSurfaceRenderNodeConfig config;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config);
    rsRenderServiceVisitor->processor_ =
        RSProcessorFactory::CreateProcessor(RSDisplayRenderNode::CompositeType::HARDWARE_COMPOSITE);
    rsRenderServiceVisitor->ProcessSurfaceRenderNode(*rsSurfaceRenderNode);
}

/**
 * @tc.name: ProcessSurfaceRenderNode03
 * @tc.desc: Test RsRenderServiceVisitorTest.ProcessSurfaceRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RsRenderServiceVisitorTest, ProcessSurfaceRenderNode03, TestSize.Level1)
{
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSSurfaceRenderNodeConfig config;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config);
    rsRenderServiceVisitor->processor_ =
        RSProcessorFactory::CreateProcessor(RSDisplayRenderNode::CompositeType::HARDWARE_COMPOSITE);
    rsRenderServiceVisitor->isSecurityDisplay_ = true;
    rsSurfaceRenderNode->SetSecurityLayer(true);
    rsRenderServiceVisitor->ProcessSurfaceRenderNode(*rsSurfaceRenderNode);
}

/**
 * @tc.name: ProcessSurfaceRenderNode04
 * @tc.desc: Test RsRenderServiceVisitorTest.ProcessSurfaceRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RsRenderServiceVisitorTest, ProcessSurfaceRenderNode04, TestSize.Level1)
{
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSSurfaceRenderNodeConfig config;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config);
    rsRenderServiceVisitor->processor_ =
        RSProcessorFactory::CreateProcessor(RSDisplayRenderNode::CompositeType::HARDWARE_COMPOSITE);
    rsSurfaceRenderNode->GetMutableRenderProperties().SetAlpha(0.0f);
    rsRenderServiceVisitor->ProcessSurfaceRenderNode(*rsSurfaceRenderNode);
}

/**
 * @tc.name: ProcessSurfaceRenderNode05
 * @tc.desc: Test RsRenderServiceVisitorTest.ProcessSurfaceRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI80HL4
 */
HWTEST_F(RsRenderServiceVisitorTest, ProcessSurfaceRenderNode05, TestSize.Level1)
{
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSSurfaceRenderNodeConfig config;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config);
    rsRenderServiceVisitor->processor_ =
        RSProcessorFactory::CreateProcessor(RSDisplayRenderNode::CompositeType::HARDWARE_COMPOSITE);
    rsSurfaceRenderNode->SetSkipLayer(true);
    ASSERT_EQ(true, rsSurfaceRenderNode->GetSkipLayer());
    rsRenderServiceVisitor->ProcessSurfaceRenderNode(*rsSurfaceRenderNode);
}

/**
 * @tc.name: ProcessCanvasRenderNode1
 * @tc.desc: Test RsRenderServiceVisitorTest.ProcessCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RsRenderServiceVisitorTest, ProcessCanvasRenderNode1, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[1];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSCanvasRenderNode node(nodeId);
    rsRenderServiceVisitor->ProcessCanvasRenderNode(node);
}

/**
 * @tc.name: ProcessCanvasRenderNode02
 * @tc.desc: Test RsRenderServiceVisitorTest.ProcessCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RsRenderServiceVisitorTest, ProcessCanvasRenderNode02, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[2];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSCanvasRenderNode node(nodeId);
    rsRenderServiceVisitor->ProcessCanvasRenderNode(node);
}

/**
 * @tc.name: ProcessCanvasRenderNode03
 * @tc.desc: Test RsRenderServiceVisitorTest.ProcessCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RsRenderServiceVisitorTest, ProcessCanvasRenderNode03, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[3];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSCanvasRenderNode node(nodeId);
    rsRenderServiceVisitor->ProcessCanvasRenderNode(node);
}

/**
 * @tc.name: ProcessCanvasRenderNode04
 * @tc.desc: Test RsRenderServiceVisitorTest.ProcessCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RsRenderServiceVisitorTest, ProcessCanvasRenderNode04, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[4];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSCanvasRenderNode node(nodeId);
    rsRenderServiceVisitor->ProcessCanvasRenderNode(node);
}

/**
 * @tc.name: ProcessCanvasRenderNode05
 * @tc.desc: Test RsRenderServiceVisitorTest.ProcessCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RsRenderServiceVisitorTest, ProcessCanvasRenderNode05, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[5];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSCanvasRenderNode node(nodeId);
    rsRenderServiceVisitor->ProcessCanvasRenderNode(node);
}

/**
 * @tc.name: ProcessCanvasRenderNode06
 * @tc.desc: Test RsRenderServiceVisitorTest.ProcessCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RsRenderServiceVisitorTest, ProcessCanvasRenderNode06, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[6];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSCanvasRenderNode node(nodeId);
    rsRenderServiceVisitor->ProcessCanvasRenderNode(node);
}

/**
 * @tc.name: ProcessCanvasRenderNode07
 * @tc.desc: Test RsRenderServiceVisitorTest.ProcessCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RsRenderServiceVisitorTest, ProcessCanvasRenderNode07, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[7];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSCanvasRenderNode node(nodeId);
    rsRenderServiceVisitor->ProcessCanvasRenderNode(node);
}

/**
 * @tc.name: ProcessCanvasRenderNode08
 * @tc.desc: Test RsRenderServiceVisitorTest.ProcessCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RsRenderServiceVisitorTest, ProcessCanvasRenderNode08, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[0];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSCanvasRenderNode node(nodeId);
    rsRenderServiceVisitor->ProcessCanvasRenderNode(node);
}

/**
 * @tc.name: ProcessRootRenderNode1
 * @tc.desc: Test RsRenderServiceVisitorTest.ProcessRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RsRenderServiceVisitorTest, ProcessRootRenderNode1, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[1];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSRootRenderNode node(nodeId);
    rsRenderServiceVisitor->ProcessRootRenderNode(node);
}

/**
 * @tc.name: ProcessRootRenderNode02
 * @tc.desc: Test RsRenderServiceVisitorTest.ProcessRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RsRenderServiceVisitorTest, ProcessRootRenderNode02, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[2];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSRootRenderNode node(nodeId);
    rsRenderServiceVisitor->ProcessRootRenderNode(node);
}

/**
 * @tc.name: ProcessRootRenderNode03
 * @tc.desc: Test RsRenderServiceVisitorTest.ProcessRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RsRenderServiceVisitorTest, ProcessRootRenderNode03, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[3];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSRootRenderNode node(nodeId);
    rsRenderServiceVisitor->ProcessRootRenderNode(node);
}

/**
 * @tc.name: ProcessRootRenderNode04
 * @tc.desc: Test RsRenderServiceVisitorTest.ProcessRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RsRenderServiceVisitorTest, ProcessRootRenderNode04, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[4];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSRootRenderNode node(nodeId);
    rsRenderServiceVisitor->ProcessRootRenderNode(node);
}

/**
 * @tc.name: ProcessRootRenderNode05
 * @tc.desc: Test RsRenderServiceVisitorTest.ProcessRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RsRenderServiceVisitorTest, ProcessRootRenderNode05, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[5];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSRootRenderNode node(nodeId);
    rsRenderServiceVisitor->ProcessRootRenderNode(node);
}

/**
 * @tc.name: ProcessRootRenderNode06
 * @tc.desc: Test RsRenderServiceVisitorTest.ProcessRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RsRenderServiceVisitorTest, ProcessRootRenderNode06, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[6];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSRootRenderNode node(nodeId);
    rsRenderServiceVisitor->ProcessRootRenderNode(node);
}

/**
 * @tc.name: ProcessRootRenderNode07
 * @tc.desc: Test RsRenderServiceVisitorTest.ProcessRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RsRenderServiceVisitorTest, ProcessRootRenderNode07, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[7];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSRootRenderNode node(nodeId);
    rsRenderServiceVisitor->ProcessRootRenderNode(node);
}

/**
 * @tc.name: ProcessRootRenderNode08
 * @tc.desc: Test RsRenderServiceVisitorTest.ProcessRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RsRenderServiceVisitorTest, ProcessRootRenderNode08, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[0];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSRootRenderNode node(nodeId);
    rsRenderServiceVisitor->ProcessRootRenderNode(node);
}

/**
 * @tc.name: SetAnimateState1
 * @tc.desc: Test RsRenderServiceVisitorTest.SetAnimateState
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RsRenderServiceVisitorTest, SetAnimateState1, TestSize.Level1)
{
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    rsRenderServiceVisitor->SetAnimateState(true);
    ASSERT_TRUE(rsRenderServiceVisitor->doAnimate_);
}

/**
 * @tc.name: ShouldForceSerial1
 * @tc.desc: Test RsRenderServiceVisitorTest.ShouldForceSerial
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RsRenderServiceVisitorTest, ShouldForceSerial1, TestSize.Level1)
{
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    ASSERT_EQ(false, rsRenderServiceVisitor->ShouldForceSerial());
}
} // namespace OHOS::Rosen
