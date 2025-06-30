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
#include "pipeline/rs_test_util.h"

#include "common/rs_obj_abs_geometry.h"
#include "pipeline/render_thread/rs_uni_render_engine.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_screen_render_node.h"
#include "pipeline/rs_processor_factory.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/main_thread/rs_render_service_visitor.h"
#include "pipeline/rs_render_thread.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "screen_manager/rs_screen_manager.h"
#include "screen_manager/rs_screen.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderServiceVisitorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderServiceVisitorTest::SetUpTestCase()
{
    RSTestUtil::InitRenderNodeGC();
}
void RSRenderServiceVisitorTest::TearDownTestCase() {}
void RSRenderServiceVisitorTest::SetUp()
{
    if (RSUniRenderJudgement::IsUniRender()) {
        auto& uniRenderThread = RSUniRenderThread::Instance();
        uniRenderThread.uniRenderEngine_ = std::make_shared<RSUniRenderEngine>();
    }
}
void RSRenderServiceVisitorTest::TearDown() {}

std::shared_ptr<RSRenderServiceVisitor> GetRenderServiceVisitor()
{
    auto visitor = std::make_shared<RSRenderServiceVisitor>();
    EXPECT_NE(visitor, nullptr);
    return visitor;
}
/*
 * @tc.name: CreateAndDestroy001
 * @tc.desc: Test RSRenderServiceVisitorTest.RSRenderServiceVisitor
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RSRenderServiceVisitorTest, CreateAndDestroy001, TestSize.Level1)
{
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    EXPECT_TRUE(rsRenderServiceVisitor != nullptr);
}

/**
 * @tc.name: PrepareChildren001
 * @tc.desc: Test RSRenderServiceVisitorTest.PrepareChildren
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RSRenderServiceVisitorTest, PrepareChildren001, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig config;
    RSSurfaceRenderNode rsSurfaceRenderNode(config);
    rsSurfaceRenderNode.stagingRenderParams_ = std::make_unique<RSSurfaceRenderParams>(rsSurfaceRenderNode.GetId());
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    rsRenderServiceVisitor->PrepareChildren(rsSurfaceRenderNode);
    EXPECT_NE(rsRenderServiceVisitor, nullptr);
}

/**
 * @tc.name: PrepareScreenRenderNode001
 * @tc.desc: Test RSRenderServiceVisitorTest.PrepareScreenRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RSRenderServiceVisitorTest, PrepareScreenRenderNode001, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[1];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    auto rsContext = std::make_shared<RSContext>();
    ScreenId screenId = 1;
    auto node = std::make_shared<RSScreenRenderNode>(nodeId, screenId, rsContext);
    EXPECT_NE(node, nullptr);
    rsRenderServiceVisitor->PrepareScreenRenderNode(*node);
}

/**
 * @tc.name: PrepareScreenRenderNode002
 * @tc.desc: Test RSRenderServiceVisitorTest.PrepareScreenRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RSRenderServiceVisitorTest, PrepareScreenRenderNode002, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[2];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    auto rsContext = std::make_shared<RSContext>();
    ScreenId screenId = 1;
    auto node = std::make_shared<RSScreenRenderNode>(nodeId, screenId, rsContext);
    EXPECT_NE(node, nullptr);
    rsRenderServiceVisitor->PrepareScreenRenderNode(*node);
}

/**
 * @tc.name: PrepareScreenRenderNode003
 * @tc.desc: Test RSRenderServiceVisitorTest.PrepareScreenRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RSRenderServiceVisitorTest, PrepareScreenRenderNode003, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[3];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    auto rsContext = std::make_shared<RSContext>();
    ScreenId screenId = 1;
    auto node = std::make_shared<RSScreenRenderNode>(nodeId, screenId, rsContext);
    EXPECT_NE(node, nullptr);
    rsRenderServiceVisitor->PrepareScreenRenderNode(*node);
}

/**
 * @tc.name: PrepareScreenRenderNode004
 * @tc.desc: Test RSRenderServiceVisitorTest.PrepareScreenRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RSRenderServiceVisitorTest, PrepareScreenRenderNode004, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[4];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    auto rsContext = std::make_shared<RSContext>();
    ScreenId screenId = 1;
    auto node = std::make_shared<RSScreenRenderNode>(nodeId, screenId, rsContext);
    EXPECT_NE(node, nullptr);
    rsRenderServiceVisitor->PrepareScreenRenderNode(*node);
}

/**
 * @tc.name: PrepareScreenRenderNode005
 * @tc.desc: Test RSRenderServiceVisitorTest.PrepareScreenRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RSRenderServiceVisitorTest, PrepareScreenRenderNode005, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[5];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    auto rsContext = std::make_shared<RSContext>();
    ScreenId screenId = 1;
    auto node = std::make_shared<RSScreenRenderNode>(nodeId, screenId, rsContext);
    EXPECT_NE(node, nullptr);
    rsRenderServiceVisitor->PrepareScreenRenderNode(*node);
}

/**
 * @tc.name: PrepareScreenRenderNode006
 * @tc.desc: Test RSRenderServiceVisitorTest.PrepareScreenRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RSRenderServiceVisitorTest, PrepareScreenRenderNode006, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[6];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    auto rsContext = std::make_shared<RSContext>();
    ScreenId screenId = 1;
    auto node = std::make_shared<RSScreenRenderNode>(nodeId, screenId, rsContext);
    EXPECT_NE(node, nullptr);
    rsRenderServiceVisitor->PrepareScreenRenderNode(*node);
}

/**
 * @tc.name: PrepareScreenRenderNode007
 * @tc.desc: Test RSRenderServiceVisitorTest.PrepareScreenRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RSRenderServiceVisitorTest, PrepareScreenRenderNode007, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[7];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    auto rsContext = std::make_shared<RSContext>();
    ScreenId screenId = 1;
    auto node = std::make_shared<RSScreenRenderNode>(nodeId, screenId, rsContext);
    EXPECT_NE(node, nullptr);
    rsRenderServiceVisitor->PrepareScreenRenderNode(*node);
}

/**
 * @tc.name: PrepareScreenRenderNode008
 * @tc.desc: Test RSRenderServiceVisitorTest.PrepareScreenRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RSRenderServiceVisitorTest, PrepareScreenRenderNode008, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[0];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    auto rsContext = std::make_shared<RSContext>();
    ScreenId screenId = 1;
    auto node = std::make_shared<RSScreenRenderNode>(nodeId, screenId, rsContext);
    EXPECT_NE(node, nullptr);
    rsRenderServiceVisitor->PrepareScreenRenderNode(*node);
}

/**
 * @tc.name: PrepareScreenRenderNode009
 * @tc.desc: Test RSRenderServiceVisitorTest.PrepareScreenRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RSRenderServiceVisitorTest, PrepareScreenRenderNode009, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[1];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    auto rsContext = std::make_shared<RSContext>();
    ScreenId screenId = 1;
    auto node = std::make_shared<RSScreenRenderNode>(nodeId, screenId, rsContext);
    auto& property = node->GetMutableRenderProperties();
    EXPECT_NE(&property, nullptr);
    property.SetBounds({ 0, 0, 400, 600 });
    property.SetFrameWidth(0);
    property.SetFrameHeight(0);
    auto& absGeo = (property.GetBoundsGeometry());
    absGeo->SetRotation(90);
    rsRenderServiceVisitor->PrepareScreenRenderNode(*node);
}

/**
 * @tc.name: PrepareScreenRenderNode010
 * @tc.desc: Test RSRenderServiceVisitorTest.PrepareScreenRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RSRenderServiceVisitorTest, PrepareScreenRenderNode010, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[2];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    auto rsContext = std::make_shared<RSContext>();
    ScreenId screenId = 1;
    auto node = std::make_shared<RSScreenRenderNode>(nodeId, screenId, rsContext);
    auto& property = node->GetMutableRenderProperties();
    EXPECT_NE(&property, nullptr);
    property.SetBounds({ 0, 0, 400, 600 });
    property.SetFrameWidth(0);
    property.SetFrameHeight(0);
    auto& absGeo = (property.GetBoundsGeometry());
    absGeo->SetRotation(270);
    rsRenderServiceVisitor->PrepareScreenRenderNode(*node);
}

/**
 * @tc.name: PrepareScreenRenderNode011
 * @tc.desc: Test RSRenderServiceVisitorTest.PrepareScreenRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RSRenderServiceVisitorTest, PrepareScreenRenderNode011, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[3];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    ScreenId screenId = 1;
    auto node = std::make_shared<RSScreenRenderNode>(nodeId, screenId, rsContext);
    rsRenderServiceVisitor->PrepareScreenRenderNode(*node);
}

/**
 * @tc.name: PrepareScreenRenderNode012
 * @tc.desc: Test RSRenderServiceVisitorTest.PrepareScreenRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RSRenderServiceVisitorTest, PrepareScreenRenderNode012, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[4];
    RSRenderServiceVisitor rsRenderServiceVisitor(true);
    auto rsContext = std::make_shared<RSContext>();
    ScreenId screenId = 1;
    auto node = std::make_shared<RSScreenRenderNode>(nodeId, screenId, rsContext);
    ASSERT_NE(node, nullptr);
    rsRenderServiceVisitor.PrepareScreenRenderNode(*node);
}

/**
 * @tc.name: PrepareScreenRenderNode013
 * @tc.desc: Test RSRenderServiceVisitorTest.PrepareScreenRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RSRenderServiceVisitorTest, PrepareScreenRenderNode013, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[5];
    RSRenderServiceVisitor rsRenderServiceVisitor(true);
    auto rsContext = std::make_shared<RSContext>();
    ScreenId screenId = 1;
    auto mirroredNode = std::make_shared<RSScreenRenderNode>(nodeId, screenId, rsContext);
    ASSERT_NE(mirroredNode, nullptr);
    auto rsContext1 = std::make_shared<RSContext>();
    ScreenId screenId1 = 2;
    auto displayNode = std::make_shared<RSScreenRenderNode>(TestSrc::limitNumber::Uint64[6], screenId1, rsContext1);
    ASSERT_NE(displayNode, nullptr);
    mirroredNode->SetMirrorSource(displayNode);
    rsRenderServiceVisitor.PrepareScreenRenderNode(*mirroredNode);
}

/**
 * @tc.name: PrepareSurfaceRenderNode001
 * @tc.desc: Test RSRenderServiceVisitorTest.PrepareSurfaceRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RSRenderServiceVisitorTest, PrepareSurfaceRenderNode001, TestSize.Level1)
{
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSSurfaceRenderNodeConfig config;
    RSSurfaceRenderNode rsSurfaceRenderNode(config);
    rsRenderServiceVisitor->PrepareSurfaceRenderNode(rsSurfaceRenderNode);
    EXPECT_TRUE(rsRenderServiceVisitor != nullptr);
}

/**
 * @tc.name: PrepareSurfaceRenderNode002
 * @tc.desc: Test RSRenderServiceVisitorTest.PrepareSurfaceRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI80HL4
 */
HWTEST_F(RSRenderServiceVisitorTest, PrepareSurfaceRenderNode002, TestSize.Level1)
{
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSSurfaceRenderNodeConfig config;
    RSSurfaceRenderNode rsSurfaceRenderNode(config);
    rsRenderServiceVisitor->isSecurityDisplay_ = true;
    rsSurfaceRenderNode.GetMultableSpecialLayerMgr().Set(SpecialLayerType::SKIP, true);
    ASSERT_EQ(true, rsSurfaceRenderNode.GetSpecialLayerMgr().Find(SpecialLayerType::SKIP));
    rsRenderServiceVisitor->PrepareSurfaceRenderNode(rsSurfaceRenderNode);
}

/**
 * @tc.name: PrepareCanvasRenderNode001
 * @tc.desc: Test RSRenderServiceVisitorTest.PrepareCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RSRenderServiceVisitorTest, PrepareCanvasRenderNode001, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[1];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSCanvasRenderNode node(nodeId);
    node.stagingRenderParams_ = std::make_unique<RSRenderParams>(node.GetId());
    rsRenderServiceVisitor->PrepareCanvasRenderNode(node);
    EXPECT_TRUE(rsRenderServiceVisitor != nullptr);
}

/**
 * @tc.name: PrepareCanvasRenderNode002
 * @tc.desc: Test RSRenderServiceVisitorTest.PrepareCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RSRenderServiceVisitorTest, PrepareCanvasRenderNode002, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[2];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSCanvasRenderNode node(nodeId);
    rsRenderServiceVisitor->PrepareCanvasRenderNode(node);
    EXPECT_TRUE(rsRenderServiceVisitor != nullptr);
}

/**
 * @tc.name: PrepareCanvasRenderNode003
 * @tc.desc: Test RSRenderServiceVisitorTest.PrepareCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RSRenderServiceVisitorTest, PrepareCanvasRenderNode003, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[3];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSCanvasRenderNode node(nodeId);
    rsRenderServiceVisitor->PrepareCanvasRenderNode(node);
    EXPECT_TRUE(rsRenderServiceVisitor != nullptr);
}

/**
 * @tc.name: PrepareCanvasRenderNode004
 * @tc.desc: Test RSRenderServiceVisitorTest.PrepareCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RSRenderServiceVisitorTest, PrepareCanvasRenderNode004, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[4];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSCanvasRenderNode node(nodeId);
    rsRenderServiceVisitor->PrepareCanvasRenderNode(node);
    EXPECT_TRUE(rsRenderServiceVisitor != nullptr);
}

/**
 * @tc.name: PrepareCanvasRenderNode005
 * @tc.desc: Test RSRenderServiceVisitorTest.PrepareCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RSRenderServiceVisitorTest, PrepareCanvasRenderNode005, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[5];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSCanvasRenderNode node(nodeId);
    rsRenderServiceVisitor->PrepareCanvasRenderNode(node);
    EXPECT_TRUE(rsRenderServiceVisitor != nullptr);
}

/**
 * @tc.name: PrepareCanvasRenderNode006
 * @tc.desc: Test RSRenderServiceVisitorTest.PrepareCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RSRenderServiceVisitorTest, PrepareCanvasRenderNode006, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[6];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSCanvasRenderNode node(nodeId);
    rsRenderServiceVisitor->PrepareCanvasRenderNode(node);
    EXPECT_TRUE(rsRenderServiceVisitor != nullptr);
}

/**
 * @tc.name: PrepareCanvasRenderNode007
 * @tc.desc: Test RSRenderServiceVisitorTest.PrepareCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RSRenderServiceVisitorTest, PrepareCanvasRenderNode007, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[7];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSCanvasRenderNode node(nodeId);
    rsRenderServiceVisitor->PrepareCanvasRenderNode(node);
    EXPECT_TRUE(rsRenderServiceVisitor != nullptr);
}

/**
 * @tc.name: PrepareCanvasRenderNode008
 * @tc.desc: Test RSRenderServiceVisitorTest.PrepareCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1r:
 */
HWTEST_F(RSRenderServiceVisitorTest, PrepareCanvasRenderNode008, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[0];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSCanvasRenderNode node(nodeId);
    rsRenderServiceVisitor->PrepareCanvasRenderNode(node);
    EXPECT_TRUE(rsRenderServiceVisitor != nullptr);
}

/**
 * @tc.name: PrepareRootRenderNode001
 * @tc.desc: Test RSRenderServiceVisitorTest.PrepareRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RSRenderServiceVisitorTest, PrepareRootRenderNode001, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[1];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSRootRenderNode node(nodeId);
    rsRenderServiceVisitor->PrepareRootRenderNode(node);
    EXPECT_TRUE(rsRenderServiceVisitor != nullptr);
}

/**
 * @tc.name: PrepareRootRenderNode002
 * @tc.desc: Test RSRenderServiceVisitorTest.PrepareRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RSRenderServiceVisitorTest, PrepareRootRenderNode002, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[2];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSRootRenderNode node(nodeId);
    rsRenderServiceVisitor->PrepareRootRenderNode(node);
    EXPECT_TRUE(rsRenderServiceVisitor != nullptr);
}

/**
 * @tc.name: PrepareRootRenderNode003
 * @tc.desc: Test RSRenderServiceVisitorTest.PrepareRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RSRenderServiceVisitorTest, PrepareRootRenderNode003, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[3];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSRootRenderNode node(nodeId);
    rsRenderServiceVisitor->PrepareRootRenderNode(node);
    EXPECT_TRUE(rsRenderServiceVisitor != nullptr);
}

/**
 * @tc.name: PrepareRootRenderNode004
 * @tc.desc: Test RSRenderServiceVisitorTest.PrepareRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RSRenderServiceVisitorTest, PrepareRootRenderNode004, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[4];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSRootRenderNode node(nodeId);
    rsRenderServiceVisitor->PrepareRootRenderNode(node);
    EXPECT_TRUE(rsRenderServiceVisitor != nullptr);
}

/**
 * @tc.name: PrepareRootRenderNode005
 * @tc.desc: Test RSRenderServiceVisitorTest.PrepareRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RSRenderServiceVisitorTest, PrepareRootRenderNode005, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[5];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSRootRenderNode node(nodeId);
    rsRenderServiceVisitor->PrepareRootRenderNode(node);
    EXPECT_TRUE(rsRenderServiceVisitor != nullptr);
}

/**
 * @tc.name: PrepareRootRenderNode006
 * @tc.desc: Test RSRenderServiceVisitorTest.PrepareRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RSRenderServiceVisitorTest, PrepareRootRenderNode006, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[6];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSRootRenderNode node(nodeId);
    rsRenderServiceVisitor->PrepareRootRenderNode(node);
    EXPECT_TRUE(rsRenderServiceVisitor != nullptr);
}

/**
 * @tc.name: PrepareRootRenderNode007
 * @tc.desc: Test RSRenderServiceVisitorTest.PrepareRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RSRenderServiceVisitorTest, PrepareRootRenderNode007, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[7];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSRootRenderNode node(nodeId);
    rsRenderServiceVisitor->PrepareRootRenderNode(node);
    EXPECT_TRUE(rsRenderServiceVisitor != nullptr);
}

/**
 * @tc.name: PrepareRootRenderNode008
 * @tc.desc: Test RSRenderServiceVisitorTest.PrepareRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RSRenderServiceVisitorTest, PrepareRootRenderNode008, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[0];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSRootRenderNode node(nodeId);
    rsRenderServiceVisitor->PrepareRootRenderNode(node);
    EXPECT_TRUE(rsRenderServiceVisitor != nullptr);
}

/**
 * @tc.name: ProcessChildren001
 * @tc.desc: Test RSRenderServiceVisitorTest.ProcessChildren
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RSRenderServiceVisitorTest, ProcessChildren001, TestSize.Level1)
{
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSSurfaceRenderNodeConfig config;
    RSSurfaceRenderNode rsSurfaceRenderNode(config);
    rsRenderServiceVisitor->ProcessChildren(rsSurfaceRenderNode);
}

/*
 * @tc.name: ProcessChildren002
 * @tc.desc: Test RSRenderServiceVisitorTest.ProcessChildren
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RSRenderServiceVisitorTest, ProcessChildren002, TestSize.Level1)
{
    RSRenderServiceVisitor rsRenderServiceVisitor(true);
    RSSurfaceRenderNodeConfig config;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config);
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    rsRenderServiceVisitor.ProcessChildren(*rsSurfaceRenderNode);
}

/**
 * @tc.name: ProcessScreenRenderNode001
 * @tc.desc: Test RSRenderServiceVisitorTest.ProcessScreenRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RSRenderServiceVisitorTest, ProcessScreenRenderNode001, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[1];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    auto rsContext = std::make_shared<RSContext>();
    ScreenId screenId = 1;
    auto node = std::make_shared<RSScreenRenderNode>(nodeId, screenId, rsContext);
    node->stagingRenderParams_ = std::make_unique<RSRenderParams>(node->GetId());
    rsRenderServiceVisitor->ProcessScreenRenderNode(*node);
    EXPECT_TRUE(rsRenderServiceVisitor != nullptr);
}

/**
 * @tc.name: ProcessScreenRenderNode002
 * @tc.desc: Test RSRenderServiceVisitorTest.ProcessScreenRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RSRenderServiceVisitorTest, ProcessScreenRenderNode002, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[2];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    auto rsContext = std::make_shared<RSContext>();
    ScreenId screenId = 1;
    auto node = std::make_shared<RSScreenRenderNode>(nodeId, screenId, rsContext);
    node->stagingRenderParams_ = std::make_unique<RSRenderParams>(node->GetId());
    rsRenderServiceVisitor->ProcessScreenRenderNode(*node);
    EXPECT_TRUE(rsRenderServiceVisitor != nullptr);
}

/**
 * @tc.name: ProcessScreenRenderNode003
 * @tc.desc: Test RSRenderServiceVisitorTest.ProcessScreenRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RSRenderServiceVisitorTest, ProcessScreenRenderNode003, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[3];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    auto rsContext = std::make_shared<RSContext>();
    ScreenId screenId = 1;
    auto node = std::make_shared<RSScreenRenderNode>(nodeId, screenId, rsContext);
    rsRenderServiceVisitor->ProcessScreenRenderNode(*node);
    EXPECT_TRUE(rsRenderServiceVisitor != nullptr);
}

/**
 * @tc.name: ProcessScreenRenderNode004
 * @tc.desc: Test RSRenderServiceVisitorTest.ProcessScreenRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RSRenderServiceVisitorTest, ProcessScreenRenderNode004, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[4];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    auto rsContext = std::make_shared<RSContext>();
    ScreenId screenId = 1;
    auto node = std::make_shared<RSScreenRenderNode>(nodeId, screenId, rsContext);
    rsRenderServiceVisitor->ProcessScreenRenderNode(*node);
    EXPECT_TRUE(rsRenderServiceVisitor != nullptr);
}

/**
 * @tc.name: ProcessScreenRenderNode005
 * @tc.desc: Test RSRenderServiceVisitorTest.ProcessScreenRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RSRenderServiceVisitorTest, ProcessScreenRenderNode005, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[5];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    auto rsContext = std::make_shared<RSContext>();
    ScreenId screenId = 1;
    auto node = std::make_shared<RSScreenRenderNode>(nodeId, screenId, rsContext);
    rsRenderServiceVisitor->ProcessScreenRenderNode(*node);
    EXPECT_TRUE(rsRenderServiceVisitor != nullptr);
}

/**
 * @tc.name: ProcessScreenRenderNode006
 * @tc.desc: Test RSRenderServiceVisitorTest.ProcessScreenRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RSRenderServiceVisitorTest, ProcessScreenRenderNode006, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[6];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    auto rsContext = std::make_shared<RSContext>();
    ScreenId screenId = 1;
    auto node = std::make_shared<RSScreenRenderNode>(nodeId, screenId, rsContext);
    rsRenderServiceVisitor->ProcessScreenRenderNode(*node);
    EXPECT_TRUE(rsRenderServiceVisitor != nullptr);
}

/**
 * @tc.name: ProcessScreenRenderNode007
 * @tc.desc: Test RSRenderServiceVisitorTest.ProcessScreenRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RSRenderServiceVisitorTest, ProcessScreenRenderNode007, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[7];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    auto rsContext = std::make_shared<RSContext>();
    ScreenId screenId = 1;
    auto node = std::make_shared<RSScreenRenderNode>(nodeId, screenId, rsContext);
    rsRenderServiceVisitor->ProcessScreenRenderNode(*node);
    EXPECT_TRUE(rsRenderServiceVisitor != nullptr);
}

/**
 * @tc.name: ProcessScreenRenderNode008
 * @tc.desc: Test RSRenderServiceVisitorTest.ProcessScreenRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RSRenderServiceVisitorTest, ProcessScreenRenderNode008, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[0];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    auto rsContext = std::make_shared<RSContext>();
    ScreenId screenId = 1;
    auto node = std::make_shared<RSScreenRenderNode>(nodeId, screenId, rsContext);
    rsRenderServiceVisitor->ProcessScreenRenderNode(*node);
    EXPECT_TRUE(rsRenderServiceVisitor != nullptr);
}

/**
 * @tc.name: ProcessScreenRenderNode009
 * @tc.desc: Test RSRenderServiceVisitorTest.ProcessScreenRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RSRenderServiceVisitorTest, ProcessScreenRenderNode009, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[0];
    RSRenderServiceVisitor rsRenderServiceVisitor(true);
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(screenManager, nullptr);
    auto rsScreen = std::make_shared<impl::RSScreen>(0, false, HdiOutput::CreateHdiOutput(0), nullptr);
    ASSERT_NE(rsScreen, nullptr);
    auto csurface = IConsumerSurface::Create();
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    rsScreen->SetProducerSurface(psurface);
    rsScreen->SetScreenSkipFrameInterval(1);
    screenManager->MockHdiScreenConnected(rsScreen);

    auto rsContext = std::make_shared<RSContext>();
    ScreenId screenId = 1;
    auto node = std::make_shared<RSScreenRenderNode>(nodeId, screenId, rsContext);
    rsRenderServiceVisitor.ProcessScreenRenderNode(*node);
}

/**
 * @tc.name: ProcessScreenRenderNode010
 * @tc.desc: Test RSRenderServiceVisitorTest.ProcessScreenRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RSRenderServiceVisitorTest, ProcessScreenRenderNode010, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[0];
    RSRenderServiceVisitor rsRenderServiceVisitor(true);
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(screenManager, nullptr);

    auto rsScreen = std::make_shared<impl::RSScreen>(0, false, HdiOutput::CreateHdiOutput(0), nullptr);
    auto csurface = IConsumerSurface::Create();
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    rsScreen->SetProducerSurface(psurface);
    rsScreen->SetScreenSkipFrameInterval(1);
    screenManager->MockHdiScreenConnected(rsScreen);

    ScreenId screenId = 0;
    auto rsContext = std::make_shared<RSContext>();
    auto mirroredNode = std::make_shared<RSScreenRenderNode>(nodeId, screenId, rsContext);
    ScreenId screenId1 = 1;
    auto rsContext1 = std::make_shared<RSContext>();
    auto displayNode = std::make_shared<RSScreenRenderNode>(TestSrc::limitNumber::Uint64[6], screenId1, rsContext1);
    mirroredNode->SetMirrorSource(displayNode);
    rsRenderServiceVisitor.ProcessScreenRenderNode(*mirroredNode);
}

/**
 * @tc.name: ProcessSurfaceRenderNode001
 * @tc.desc: Test RSRenderServiceVisitorTest.ProcessSurfaceRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RSRenderServiceVisitorTest, ProcessSurfaceRenderNode001, TestSize.Level1)
{
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSSurfaceRenderNodeConfig config;
    RSSurfaceRenderNode rsSurfaceRenderNode(config);
    rsRenderServiceVisitor->ProcessSurfaceRenderNode(rsSurfaceRenderNode);
    EXPECT_TRUE(rsRenderServiceVisitor != nullptr);
}

/**
 * @tc.name: ProcessSurfaceRenderNode002
 * @tc.desc: Test RSRenderServiceVisitorTest.ProcessSurfaceRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RSRenderServiceVisitorTest, ProcessSurfaceRenderNode002, TestSize.Level1)
{
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSSurfaceRenderNodeConfig config;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config);
    rsRenderServiceVisitor->processor_ =
        RSProcessorFactory::CreateProcessor(CompositeType::HARDWARE_COMPOSITE);
    rsRenderServiceVisitor->ProcessSurfaceRenderNode(*rsSurfaceRenderNode);
    EXPECT_TRUE(rsRenderServiceVisitor != nullptr);
}

/**
 * @tc.name: ProcessSurfaceRenderNode003
 * @tc.desc: Test RSRenderServiceVisitorTest.ProcessSurfaceRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RSRenderServiceVisitorTest, ProcessSurfaceRenderNode003, TestSize.Level1)
{
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSSurfaceRenderNodeConfig config;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config);
    rsRenderServiceVisitor->processor_ =
        RSProcessorFactory::CreateProcessor(CompositeType::HARDWARE_COMPOSITE);
    rsRenderServiceVisitor->isSecurityDisplay_ = true;
    rsSurfaceRenderNode->SetSecurityLayer(true);
    rsRenderServiceVisitor->ProcessSurfaceRenderNode(*rsSurfaceRenderNode);
    EXPECT_TRUE(rsRenderServiceVisitor != nullptr);
}

/**
 * @tc.name: ProcessSurfaceRenderNode004
 * @tc.desc: Test RSRenderServiceVisitorTest.ProcessSurfaceRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RSRenderServiceVisitorTest, ProcessSurfaceRenderNode004, TestSize.Level1)
{
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSSurfaceRenderNodeConfig config;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config);
    rsRenderServiceVisitor->processor_ =
        RSProcessorFactory::CreateProcessor(CompositeType::HARDWARE_COMPOSITE);
    rsSurfaceRenderNode->GetMutableRenderProperties().SetAlpha(0.0f);
    rsRenderServiceVisitor->ProcessSurfaceRenderNode(*rsSurfaceRenderNode);
    EXPECT_TRUE(rsRenderServiceVisitor != nullptr);
}

/**
 * @tc.name: ProcessSurfaceRenderNode005
 * @tc.desc: Test RSRenderServiceVisitorTest.ProcessSurfaceRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI80HL4
 */
HWTEST_F(RSRenderServiceVisitorTest, ProcessSurfaceRenderNode005, TestSize.Level1)
{
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSSurfaceRenderNodeConfig config;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config);
    rsRenderServiceVisitor->processor_ =
        RSProcessorFactory::CreateProcessor(CompositeType::HARDWARE_COMPOSITE);
    rsSurfaceRenderNode->GetMultableSpecialLayerMgr().Set(SpecialLayerType::SKIP, true);
    ASSERT_EQ(true, rsSurfaceRenderNode->GetSpecialLayerMgr().Find(SpecialLayerType::SKIP));
    rsRenderServiceVisitor->ProcessSurfaceRenderNode(*rsSurfaceRenderNode);
}

/**
 * @tc.name: ProcessCanvasRenderNode001
 * @tc.desc: Test RSRenderServiceVisitorTest.ProcessCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RSRenderServiceVisitorTest, ProcessCanvasRenderNode001, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[1];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSCanvasRenderNode node(nodeId);
    rsRenderServiceVisitor->ProcessCanvasRenderNode(node);
}

/**
 * @tc.name: ProcessCanvasRenderNode002
 * @tc.desc: Test RSRenderServiceVisitorTest.ProcessCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RSRenderServiceVisitorTest, ProcessCanvasRenderNode002, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[2];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSCanvasRenderNode node(nodeId);
    rsRenderServiceVisitor->ProcessCanvasRenderNode(node);
}

/**
 * @tc.name: ProcessCanvasRenderNode003
 * @tc.desc: Test RSRenderServiceVisitorTest.ProcessCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RSRenderServiceVisitorTest, ProcessCanvasRenderNode003, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[3];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSCanvasRenderNode node(nodeId);
    rsRenderServiceVisitor->ProcessCanvasRenderNode(node);
}

/**
 * @tc.name: ProcessCanvasRenderNode004
 * @tc.desc: Test RSRenderServiceVisitorTest.ProcessCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RSRenderServiceVisitorTest, ProcessCanvasRenderNode004, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[4];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSCanvasRenderNode node(nodeId);
    rsRenderServiceVisitor->ProcessCanvasRenderNode(node);
}

/**
 * @tc.name: ProcessCanvasRenderNode005
 * @tc.desc: Test RSRenderServiceVisitorTest.ProcessCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RSRenderServiceVisitorTest, ProcessCanvasRenderNode005, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[5];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSCanvasRenderNode node(nodeId);
    rsRenderServiceVisitor->ProcessCanvasRenderNode(node);
}

/**
 * @tc.name: ProcessCanvasRenderNode006
 * @tc.desc: Test RSRenderServiceVisitorTest.ProcessCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RSRenderServiceVisitorTest, ProcessCanvasRenderNode006, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[6];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSCanvasRenderNode node(nodeId);
    rsRenderServiceVisitor->ProcessCanvasRenderNode(node);
}

/**
 * @tc.name: ProcessCanvasRenderNode007
 * @tc.desc: Test RSRenderServiceVisitorTest.ProcessCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RSRenderServiceVisitorTest, ProcessCanvasRenderNode007, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[7];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSCanvasRenderNode node(nodeId);
    rsRenderServiceVisitor->ProcessCanvasRenderNode(node);
}

/**
 * @tc.name: ProcessCanvasRenderNode008
 * @tc.desc: Test RSRenderServiceVisitorTest.ProcessCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RSRenderServiceVisitorTest, ProcessCanvasRenderNode008, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[0];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSCanvasRenderNode node(nodeId);
    rsRenderServiceVisitor->ProcessCanvasRenderNode(node);
}

/**
 * @tc.name: ProcessRootRenderNode001
 * @tc.desc: Test RSRenderServiceVisitorTest.ProcessRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RSRenderServiceVisitorTest, ProcessRootRenderNode001, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[1];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSRootRenderNode node(nodeId);
    rsRenderServiceVisitor->ProcessRootRenderNode(node);
}

/**
 * @tc.name: ProcessRootRenderNode002
 * @tc.desc: Test RSRenderServiceVisitorTest.ProcessRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RSRenderServiceVisitorTest, ProcessRootRenderNode002, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[2];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSRootRenderNode node(nodeId);
    rsRenderServiceVisitor->ProcessRootRenderNode(node);
}

/**
 * @tc.name: ProcessRootRenderNode003
 * @tc.desc: Test RSRenderServiceVisitorTest.ProcessRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RSRenderServiceVisitorTest, ProcessRootRenderNode003, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[3];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSRootRenderNode node(nodeId);
    rsRenderServiceVisitor->ProcessRootRenderNode(node);
}

/**
 * @tc.name: ProcessRootRenderNode004
 * @tc.desc: Test RSRenderServiceVisitorTest.ProcessRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RSRenderServiceVisitorTest, ProcessRootRenderNode004, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[4];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSRootRenderNode node(nodeId);
    rsRenderServiceVisitor->ProcessRootRenderNode(node);
}

/**
 * @tc.name: ProcessRootRenderNode005
 * @tc.desc: Test RSRenderServiceVisitorTest.ProcessRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RSRenderServiceVisitorTest, ProcessRootRenderNode005, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[5];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSRootRenderNode node(nodeId);
    rsRenderServiceVisitor->ProcessRootRenderNode(node);
}

/**
 * @tc.name: ProcessRootRenderNode006
 * @tc.desc: Test RSRenderServiceVisitorTest.ProcessRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RSRenderServiceVisitorTest, ProcessRootRenderNode006, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[6];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSRootRenderNode node(nodeId);
    rsRenderServiceVisitor->ProcessRootRenderNode(node);
}

/**
 * @tc.name: ProcessRootRenderNode007
 * @tc.desc: Test RSRenderServiceVisitorTest.ProcessRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RSRenderServiceVisitorTest, ProcessRootRenderNode007, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[7];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSRootRenderNode node(nodeId);
    rsRenderServiceVisitor->ProcessRootRenderNode(node);
}

/**
 * @tc.name: ProcessRootRenderNode008
 * @tc.desc: Test RSRenderServiceVisitorTest.ProcessRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RSRenderServiceVisitorTest, ProcessRootRenderNode008, TestSize.Level1)
{
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[0];
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    RSRootRenderNode node(nodeId);
    rsRenderServiceVisitor->ProcessRootRenderNode(node);
}

/**
 * @tc.name: SetAnimateState
 * @tc.desc: Test RSRenderServiceVisitorTest.SetAnimateState
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RSRenderServiceVisitorTest, SetAnimateState, TestSize.Level1)
{
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    rsRenderServiceVisitor->SetAnimateState(true);
    ASSERT_TRUE(rsRenderServiceVisitor->doAnimate_);
}

/**
 * @tc.name: ShouldForceSerial
 * @tc.desc: Test RSRenderServiceVisitorTest.ShouldForceSerial
 * @tc.type: FUNC
 * @tc.require: issueI614P1
 */
HWTEST_F(RSRenderServiceVisitorTest, ShouldForceSerial, TestSize.Level1)
{
    auto rsRenderServiceVisitor = GetRenderServiceVisitor();
    ASSERT_EQ(false, rsRenderServiceVisitor->ShouldForceSerial());
}
} // namespace OHOS::Rosen
