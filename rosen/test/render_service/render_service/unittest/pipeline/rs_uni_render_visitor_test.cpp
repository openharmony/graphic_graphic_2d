/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include <memory>

#include "gtest/gtest.h"
#include "limit_number.h"
#include "mock/mock_matrix.h"
#include "rs_test_util.h"
#include "system/rs_system_parameters.h"

#include "draw/color.h"
#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_effect_render_node.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_processor_factory.h"
#include "pipeline/rs_proxy_render_node.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_render_thread.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_uni_render_engine.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "pipeline/rs_uni_render_thread.h"
#include "pipeline/rs_uni_render_util.h"
#include "pipeline/rs_uni_render_visitor.h"

using namespace testing;
using namespace testing::ext;

namespace {
    const std::string DEFAULT_NODE_NAME = "1";
    const std::string INVALID_NODE_NAME = "2";
    const int DEFAULT_DIRTY_REGION_WIDTH = 10;
    const int DEFAULT_DIRTY_REGION_HEIGHT = 10;
    constexpr uint32_t DEFAULT_CANVAS_WIDTH = 800;
    constexpr uint32_t DEFAULT_CANVAS_HEIGHT = 600;
    constexpr uint32_t RECT_DEFAULT_SIZE = 100;
    constexpr int ROTATION_90 = 90;
    const OHOS::Rosen::RectI DEFAULT_RECT = {0, 80, 1000, 1000};
    const OHOS::Rosen::RectI DEFAULT_FILTER_RECT = {0, 0, 500, 500};
    const std::string CAPTURE_WINDOW_NAME = "CapsuleWindow";
    constexpr int MAX_ALPHA = 255;
    constexpr OHOS::Rosen::NodeId DEFAULT_NODE_ID = 100;
}

namespace OHOS::Rosen {
class RSUniRenderVisitorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline Mock::MatrixMock* matrixMock_;
};

void RSUniRenderVisitorTest::SetUpTestCase()
{
    matrixMock_ = Mock::MatrixMock::GetInstance();
    EXPECT_CALL(*matrixMock_, GetMinMaxScales(_)).WillOnce(testing::Return(false));
    RSTestUtil::InitRenderNodeGC();
}
void RSUniRenderVisitorTest::TearDownTestCase() {}
void RSUniRenderVisitorTest::SetUp()
{
    if (RSUniRenderJudgement::IsUniRender()) {
        auto& uniRenderThread = RSUniRenderThread::Instance();
        uniRenderThread.uniRenderEngine_ = std::make_shared<RSUniRenderEngine>();
    }
}
void RSUniRenderVisitorTest::TearDown()
{
    system::SetParameter("rosen.dirtyregiondebug.enabled", "0");
    system::SetParameter("rosen.uni.partialrender.enabled", "4");
    system::GetParameter("rosen.dirtyregiondebug.surfacenames", "0");
}

/*
 * @tc.name: UpdateSurfaceRenderNodeScaleTest
 * @tc.desc: Test RSUniRenderVisitorTest.UpdateSurfaceRenderNodeScaleTest
 * @tc.type: FUNC
 * @tc.require: issueI79U8E
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateSurfaceRenderNodeScaleTest, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    RSSurfaceRenderNodeConfig config;
    config.id = 11;     // leash window config id
    config.name = "leashWindowNode";
    auto leashWindowNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    ASSERT_NE(leashWindowNode, nullptr);
    leashWindowNode->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);

    auto deviceTypeStr = system::GetParameter("const.product.devicetype", "pc");
    system::SetParameter("const.product.devicetype", "pc");
    RSMainThread::Instance()->SetDeviceType();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->UpdateSurfaceRenderNodeScale(*leashWindowNode);
    system::SetParameter("const.product.devicetype", deviceTypeStr);
    RSMainThread::Instance()->SetDeviceType();
}

/*
 * @tc.name: CheckLuminanceStatusChangeTest
 * @tc.desc: Test RSUniRenderVisitorTest.CheckLuminanceStatusChangeTest
 * @tc.type: FUNC
 * @tc.require: issueI79U8E
 */
HWTEST_F(RSUniRenderVisitorTest, CheckLuminanceStatusChangeTest, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    RSMainThread::Instance()->SetLuminanceChangingStatus(true);
    ASSERT_EQ(rsUniRenderVisitor->CheckLuminanceStatusChange(), true);
    RSMainThread::Instance()->SetLuminanceChangingStatus(false);
    ASSERT_EQ(rsUniRenderVisitor->CheckLuminanceStatusChange(), false);
}

/*
 * @tc.name: PrepareCanvasRenderNodeTest
 * @tc.desc: Test RSUniRenderVisitorTest.PrepareCanvasRenderNodeTest
 * @tc.type: FUNC
 * @tc.require: issueI79U8E
 */
HWTEST_F(RSUniRenderVisitorTest, PrepareCanvasRenderNodeTest, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    auto rsCanvasRenderNode = std::make_shared<RSCanvasRenderNode>(1, rsContext->weak_from_this());
    ASSERT_NE(rsCanvasRenderNode, nullptr);
    rsCanvasRenderNode->stagingRenderParams_ = std::make_unique<RSRenderParams>(rsCanvasRenderNode->GetId());

    RSSurfaceRenderNodeConfig config;
    config.id = 10;
    config.name = "scbScreenNode";
    auto scbScreenNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    ASSERT_NE(scbScreenNode, nullptr);
    scbScreenNode->SetSurfaceNodeType(RSSurfaceNodeType::SCB_SCREEN_NODE);
    scbScreenNode->stagingRenderParams_ = std::make_unique<RSRenderParams>(scbScreenNode->GetId());

    config.id = 11;
    config.name = "leashWindowNode";
    auto leashWindowNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    ASSERT_NE(leashWindowNode, nullptr);
    leashWindowNode->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);
    leashWindowNode->stagingRenderParams_ = std::make_unique<RSRenderParams>(leashWindowNode->GetId());

    rsCanvasRenderNode->SetParent(std::weak_ptr<RSSurfaceRenderNode>(leashWindowNode));
    rsUniRenderVisitor->PrepareCanvasRenderNode(*rsCanvasRenderNode);

    rsCanvasRenderNode->SetParent(std::weak_ptr<RSSurfaceRenderNode>(scbScreenNode));
    RSDisplayNodeConfig displayConfig;
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(20, displayConfig, rsContext->weak_from_this());
    rsUniRenderVisitor->curDisplayDirtyManager_ = rsDisplayRenderNode->GetDirtyManager();
    rsUniRenderVisitor->currentVisitDisplay_ = 0;
    rsUniRenderVisitor->PrepareCanvasRenderNode(*rsCanvasRenderNode);
}

/*
 * @tc.name: DrawCacheRegionForDFX001
 * @tc.desc: Test RSUniRenderVisitorTest.DrawCacheRegionForDFX001
 * @tc.type: FUNC
 * @tc.require: issueI79U8E
 */
HWTEST_F(RSUniRenderVisitorTest, DrawCacheRegionForDFX001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    RectI rect(0, 0, 0, 0);
    std::map<NodeId, RectI> dirtyRects;
    dirtyRects[0] = rect;
    rsUniRenderVisitor->DrawCacheRegionForDFX(dirtyRects);
}

/*
 * @tc.name: DrawEffectRenderNodeForDFX003
 * @tc.desc: Test RSUniRenderVisitorTest.DrawEffectRenderNodeForDFX003
 * @tc.type: FUNC
 * @tc.require: issueI79U8E
 */
HWTEST_F(RSUniRenderVisitorTest, DrawEffectRenderNodeForDFX003, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    RectI rect(0, 0, 0, 0);
    std::vector<RectI> nodesUseEffectFallbackForDfx;
    nodesUseEffectFallbackForDfx.push_back(rect);
    std::vector<RectI> nodesUseEffectForDfx;
    nodesUseEffectForDfx.push_back(rect);
    NodeId id = 0;
    std::weak_ptr<RSContext> context;
    std::shared_ptr<RSEffectRenderNode> node = std::make_shared<RSEffectRenderNode>(id, context);
    ASSERT_NE(node, nullptr);
    rsUniRenderVisitor->effectNodeMapForDfx_[node->GetId()].first = nodesUseEffectForDfx;
    rsUniRenderVisitor->effectNodeMapForDfx_[node->GetId()].second = nodesUseEffectFallbackForDfx;
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().RegisterRenderNode(node);
    rsUniRenderVisitor->DrawEffectRenderNodeForDFX();
}

/*
 * @tc.name: PrepareChildren001
 * @tc.desc: PrepareChildren Test
 * @tc.type: FUNC
 * @tc.require: issueI79U8E
 */
HWTEST_F(RSUniRenderVisitorTest, PrepareChildren001, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    auto rsBaseRenderNode = std::make_shared<RSBaseRenderNode>(10, rsContext->weak_from_this());
    rsBaseRenderNode->InitRenderParams();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->PrepareChildren(*rsBaseRenderNode);
}

/*
 * @tc.name: SetHardwareEnabledNodes001
 * @tc.desc: SetHardwareEnabledNodes Test
 * @tc.type: FUNC
 * @tc.require: issueI79U8E
 */
HWTEST_F(RSUniRenderVisitorTest, SetHardwareEnabledNodes001, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    auto rsRootRenderNode = std::make_shared<RSRootRenderNode>(10, rsContext->weak_from_this());
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->DoDirectComposition(rsRootRenderNode);
}

/*
 * @tc.name: DrawAllSurfaceOpaqueRegionForDFX001
 * @tc.desc: DrawAllSurfaceOpaqueRegionForDFX Test
 * @tc.type: FUNC
 * @tc.require: issueI79U8E
 */
HWTEST_F(RSUniRenderVisitorTest, DrawAllSurfaceOpaqueRegionForDFX001, TestSize.Level1)
{
    system::SetParameter("rosen.uni.opaqueregiondebug", "1");
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    RSDisplayNodeConfig displayConfig;
    config.id = 10;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(20, displayConfig, rsContext->weak_from_this());
    rsSurfaceRenderNode->InitRenderParams();
    rsDisplayRenderNode->InitRenderParams();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsSurfaceRenderNode->SetSrcRect(RectI(0, 0, 10, 10));
    rsDisplayRenderNode->AddChild(rsSurfaceRenderNode, -1);
    ASSERT_NE(rsDisplayRenderNode->GetDirtyManager(), nullptr);
    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    ASSERT_NE(screenManager, nullptr);
    rsDisplayRenderNode->SetScreenId(0);
    auto rsScreen = std::make_unique<impl::RSScreen>(0, false, HdiOutput::CreateHdiOutput(0), nullptr);
    screenManager->MockHdiScreenConnected(rsScreen);
    rsUniRenderVisitor->PrepareDisplayRenderNode(*rsDisplayRenderNode);
    rsUniRenderVisitor->ProcessDisplayRenderNode(*rsDisplayRenderNode);
    system::SetParameter("rosen.uni.opaqueregiondebug", "0");
}

/*
 * @tc.name: DrawTargetSurfaceDirtyRegionForDFX001
 * @tc.desc: DrawTargetSurfaceDirtyRegionForDFX Test
 * @tc.type: FUNC
 * @tc.require: issueI79U8E
 */
HWTEST_F(RSUniRenderVisitorTest, DrawTargetSurfaceDirtyRegionForDFX001, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    RSDisplayNodeConfig displayConfig;
    config.name = "SurfaceDirtyDFX";
    config.id = 10;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(20, displayConfig, rsContext->weak_from_this());
    rsSurfaceRenderNode->InitRenderParams();
    rsDisplayRenderNode->InitRenderParams();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsSurfaceRenderNode->SetSrcRect(RectI(0, 0, 10, 10));
    rsDisplayRenderNode->AddChild(rsSurfaceRenderNode, -1);
    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    ASSERT_NE(screenManager, nullptr);
    rsDisplayRenderNode->SetScreenId(0);
    auto rsScreen = std::make_unique<impl::RSScreen>(0, false, HdiOutput::CreateHdiOutput(0), nullptr);
    screenManager->MockHdiScreenConnected(rsScreen);
    rsUniRenderVisitor->PrepareDisplayRenderNode(*rsDisplayRenderNode);
    system::SetParameter("rosen.dirtyregiondebug.surfacenames", "SurfaceDirtyDFX");
    rsUniRenderVisitor->ProcessDisplayRenderNode(*rsDisplayRenderNode);
    system::SetParameter("rosen.dirtyregiondebug.surfacenames", "0");
}

HWTEST_F(RSUniRenderVisitorTest, PrepareProxyRenderNode001, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig config;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config);
    std::weak_ptr<RSSurfaceRenderNode> rsSurfaceRenderNodeW(rsSurfaceRenderNode);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    auto rsContext = std::make_shared<RSContext>();

    NodeId id = 0;
    NodeId targetID = 0;
    std::shared_ptr<RSProxyRenderNode> rsProxyRenderNode(
        new RSProxyRenderNode(id, rsSurfaceRenderNodeW, targetID, rsContext->weak_from_this()));
    rsUniRenderVisitor->PrepareProxyRenderNode(*rsProxyRenderNode);
    rsUniRenderVisitor->ProcessProxyRenderNode(*rsProxyRenderNode);

    config.id = 1;
    auto rsSurfaceRenderNodeS = std::make_shared<RSSurfaceRenderNode>(config);
    rsSurfaceRenderNodeS->AddChild(rsProxyRenderNode, 1);
    rsProxyRenderNode->Prepare(rsUniRenderVisitor);
}

HWTEST_F(RSUniRenderVisitorTest, RSDisplayRenderNode001, TestSize.Level1)
{
    RSDisplayNodeConfig config;
    auto rsContext = std::make_shared<RSContext>();
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(0, config, rsContext->weak_from_this());
    rsDisplayRenderNode->InitRenderParams();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();

    rsDisplayRenderNode->Prepare(rsUniRenderVisitor);
    rsDisplayRenderNode->Process(rsUniRenderVisitor);
}

/*
 * @tc.name: CheckQuickSkipPrepareParamSetAndGetValid001
 * @tc.desc: Check if param set and get apis are valid.
 * @tc.type: FUNC
 * @tc.require: I67FKA
*/
HWTEST_F(RSUniRenderVisitorTest, CheckQuickSkipPrepareParamSetAndGetValid001, TestSize.Level1)
{
    int defaultParam = (int)RSSystemParameters::GetQuickSkipPrepareType();
    (void)system::SetParameter("rosen.quickskipprepare.enabled", "0");
    int param = (int)RSSystemParameters::GetQuickSkipPrepareType();
    ASSERT_EQ(param, 0);
    (void)system::SetParameter("rosen.quickskipprepare.enabled", "1");
    param = (int)RSSystemParameters::GetQuickSkipPrepareType();
    ASSERT_EQ(param, 1);

    NodeId testId = 10;
    pid_t pid = ExtractPid(testId);
    ASSERT_EQ(pid, 0);
    const int paddingDigit = 32;
    NodeId testPid = testId << paddingDigit;
    pid = ExtractPid(testPid);
    ASSERT_EQ(pid, testId);
    (void)system::SetParameter("rosen.quickskipprepare.enabled", std::to_string(defaultParam));
}

/*
 * @tc.name: CheckSurfaceRenderNodeNotStatic001
 * @tc.desc: Generate not static surface render node(self drawing, leash window) and execute preparation step.
 *           Get trace and check corresponding node's preparation exists and no 'Skip' info.
 * @tc.type: FUNC
 * @tc.require: I67FKA
*/
HWTEST_F(RSUniRenderVisitorTest, CheckSurfaceRenderNodeNotStatic001, TestSize.Level1)
{
    int defaultParam = (int)RSSystemParameters::GetQuickSkipPrepareType();
    (void)system::SetParameter("rosen.quickskipprepare.enabled", "1");

    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    config.id = 10;
    config.name = "selfDrawTestNode";
    auto selfDrawSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    selfDrawSurfaceRenderNode->SetSurfaceNodeType(RSSurfaceNodeType::SELF_DRAWING_NODE);
    selfDrawSurfaceRenderNode->InitRenderParams();
    config.id = 11;
    config.name = "leashWindowTestNode";
    auto leashWindowNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    leashWindowNode->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);
    leashWindowNode->InitRenderParams();

    RSDisplayNodeConfig displayConfig;
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(12, displayConfig, rsContext->weak_from_this());
    rsDisplayRenderNode->InitRenderParams();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsDisplayRenderNode->AddChild(selfDrawSurfaceRenderNode, -1);
    rsDisplayRenderNode->AddChild(leashWindowNode, -1);
    // execute add child
    rsUniRenderVisitor->PrepareDisplayRenderNode(*rsDisplayRenderNode);
    rsUniRenderVisitor->ProcessDisplayRenderNode(*rsDisplayRenderNode);
    // test if skip testNode
    rsUniRenderVisitor->PrepareDisplayRenderNode(*rsDisplayRenderNode);
    (void)system::SetParameter("rosen.quickskipprepare.enabled", std::to_string(defaultParam));
}

/*
 * @tc.name: CheckSurfaceRenderNodeStatic001
 * @tc.desc: Generate static surface render node(app window node) and execute preparation step.
 *           Get trace and check corresponding node's preparation and 'Skip' info exist.
 * @tc.type: FUNC
 * @tc.require: I67FKA
*/
HWTEST_F(RSUniRenderVisitorTest, CheckSurfaceRenderNodeStatic001, TestSize.Level1)
{
    int defaultParam = (int)RSSystemParameters::GetQuickSkipPrepareType();
    (void)system::SetParameter("rosen.quickskipprepare.enabled", "1");

    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    config.id = 10;
    config.name = "appWindowTestNode";
    auto defaultSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    defaultSurfaceRenderNode->InitRenderParams();
    defaultSurfaceRenderNode->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);

    RSDisplayNodeConfig displayConfig;
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(11, displayConfig, rsContext->weak_from_this());
    rsDisplayRenderNode->InitRenderParams();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsDisplayRenderNode->AddChild(defaultSurfaceRenderNode, -1);
    // execute add child
    rsUniRenderVisitor->PrepareDisplayRenderNode(*rsDisplayRenderNode);
    rsUniRenderVisitor->ProcessDisplayRenderNode(*rsDisplayRenderNode);
    // test if skip testNode
    rsUniRenderVisitor->PrepareDisplayRenderNode(*rsDisplayRenderNode);
    (void)system::SetParameter("rosen.quickskipprepare.enabled", std::to_string(defaultParam));
}

HWTEST_F(RSUniRenderVisitorTest, ProcessRootRenderNode001, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    auto rsRootRenderNode = std::make_shared<RSRootRenderNode>(0, rsContext->weak_from_this());
    rsRootRenderNode->InitRenderParams();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->curSurfaceNode_ = RSTestUtil::CreateSurfaceNode();
    rsUniRenderVisitor->PrepareRootRenderNode(*rsRootRenderNode);
    rsUniRenderVisitor->ProcessRootRenderNode(*rsRootRenderNode);

    auto& property = rsRootRenderNode->GetMutableRenderProperties();
    property.SetVisible(false);
    rsUniRenderVisitor->PrepareRootRenderNode(*rsRootRenderNode);

    rsUniRenderVisitor->dirtyFlag_ = true;
    rsUniRenderVisitor->ProcessRootRenderNode(*rsRootRenderNode);
}

/*
 * @tc.name: CalcDirtyDisplayRegion
 * @tc.desc: Set surface to transparent, add a canvas node to create a transparent dirty region
 * @tc.type: FUNC
 * @tc.require: I68IPR
*/
HWTEST_F(RSUniRenderVisitorTest, CalcDirtyDisplayRegion, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    RSDisplayNodeConfig displayConfig;
    config.id = 10;
    auto rsCanvasRenderNode = std::make_shared<RSCanvasRenderNode>(1, rsContext->weak_from_this());
    rsCanvasRenderNode->InitRenderParams();
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    rsSurfaceRenderNode->InitRenderParams();
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(11, displayConfig, rsContext->weak_from_this());
    rsDisplayRenderNode->InitRenderParams();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();

    Occlusion::Rect rect{0, 80, 2560, 1600};
    Occlusion::Region region{rect};
    VisibleData vData;
    std::map<NodeId, RSVisibleLevel> pidVisMap;

    auto partialRenderType = RSSystemProperties::GetUniPartialRenderEnabled();
    auto isPartialRenderEnabled = (partialRenderType != PartialRenderType::DISABLED);
    ASSERT_EQ(isPartialRenderEnabled, true);

    // set surface to transparent, add a canvas node to create a transparent dirty region
    rsSurfaceRenderNode->SetAbilityBGAlpha(0);
    rsSurfaceRenderNode->SetSrcRect(RectI(0, 80, 2560, 1600));
    rsDisplayRenderNode->AddChild(rsSurfaceRenderNode, -1);
    rsSurfaceRenderNode->AddChild(rsCanvasRenderNode, -1);

    rsUniRenderVisitor->PrepareDisplayRenderNode(*rsDisplayRenderNode);
    rsSurfaceRenderNode->SetVisibleRegionRecursive(region, vData, pidVisMap);
    rsUniRenderVisitor->ProcessDisplayRenderNode(*rsDisplayRenderNode);
}

/*
 * @tc.name: CalcDirtyRegionForFilterNode
 * @tc.desc: Create a filter effect to test filter node
 * @tc.type: FUNC
 * @tc.require: I68IPR
*/
HWTEST_F(RSUniRenderVisitorTest, CalcDirtyRegionForFilterNode, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    RSDisplayNodeConfig displayConfig;
    config.id = 10;
    auto rsCanvasRenderNode = std::make_shared<RSCanvasRenderNode>(1, rsContext->weak_from_this());
    rsCanvasRenderNode->InitRenderParams();
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    rsSurfaceRenderNode->InitRenderParams();
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(11, displayConfig, rsContext->weak_from_this());
    rsDisplayRenderNode->InitRenderParams();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();

    rsSurfaceRenderNode->SetAbilityBGAlpha(0);
    rsSurfaceRenderNode->SetSrcRect(RectI(0, 80, 2560, 1600));
    // create a filter effect
    float blurRadiusX = 30.0f;
    float blurRadiusY = 30.0f;
    auto filter = RSFilter::CreateBlurFilter(blurRadiusX, blurRadiusY);
    rsCanvasRenderNode->GetMutableRenderProperties().SetFilter(filter);
    rsDisplayRenderNode->AddChild(rsSurfaceRenderNode, -1);
    rsSurfaceRenderNode->AddChild(rsCanvasRenderNode, -1);
    rsUniRenderVisitor->PrepareDisplayRenderNode(*rsDisplayRenderNode);
    rsUniRenderVisitor->ProcessDisplayRenderNode(*rsDisplayRenderNode);
}

/*
 * @tc.name: SetSurfafaceGlobalDirtyRegion
 * @tc.desc: Add two surfacenode child to test global dirty region
 * @tc.type: FUNC
 * @tc.require: I68IPR
*/
HWTEST_F(RSUniRenderVisitorTest, SetSurfafaceGlobalDirtyRegion, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    config.id = 10;
    auto rsSurfaceRenderNode1 = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    rsSurfaceRenderNode1->InitRenderParams();
    Occlusion::Rect rect{0, 80, 2560, 1600};
    Occlusion::Region region{rect};
    VisibleData vData;
    std::map<NodeId, RSVisibleLevel> pidVisMap;
    rsSurfaceRenderNode1->SetVisibleRegionRecursive(region, vData, pidVisMap);

    config.id = 11;
    auto rsSurfaceRenderNode2 = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    rsSurfaceRenderNode2->InitRenderParams();
    Occlusion::Rect rect2{100, 100, 500, 1500};
    Occlusion::Region region2{rect2};
    rsSurfaceRenderNode2->SetVisibleRegionRecursive(region2, vData, pidVisMap);

    RSDisplayNodeConfig displayConfig;
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(9, displayConfig, rsContext->weak_from_this());
    rsDisplayRenderNode->InitRenderParams();
    rsDisplayRenderNode->AddChild(rsSurfaceRenderNode1, -1);
    rsDisplayRenderNode->AddChild(rsSurfaceRenderNode2, -1);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->PrepareDisplayRenderNode(*rsDisplayRenderNode);
    rsUniRenderVisitor->ProcessDisplayRenderNode(*rsDisplayRenderNode);
}

/**
 * @tc.name: CopyVisitorInfosTest
 * @tc.desc: Test RSUniRenderVisitorTest.CopyVisitorInfosTest
 * @tc.type: FUNC
 * @tc.require: issueI79KM8
 */
HWTEST_F(RSUniRenderVisitorTest, CopyVisitorInfosTest, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    auto newRsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    newRsUniRenderVisitor->CopyVisitorInfos(rsUniRenderVisitor);
    ASSERT_EQ(rsUniRenderVisitor->currentVisitDisplay_, newRsUniRenderVisitor->currentVisitDisplay_);
}

/**
 * @tc.name: ClearTransparentBeforeSaveLayerTest
 * @tc.desc: Test RSUniRenderVisitorTest.ClearTransparentBeforeSaveLayerTest
 * @tc.type: FUNC
 * @tc.require: issueI79KM8
 */
HWTEST_F(RSUniRenderVisitorTest, ClearTransparentBeforeSaveLayerTest, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->isHardwareForcedDisabled_ = true;
    rsUniRenderVisitor->doAnimate_ = false;
    rsUniRenderVisitor->ClearTransparentBeforeSaveLayer();
    ASSERT_EQ(false, rsUniRenderVisitor->IsHardwareComposerEnabled());

    auto rsUniRenderVisitor2 = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor2, nullptr);
    rsUniRenderVisitor2->isHardwareForcedDisabled_ = true;
    rsUniRenderVisitor2->doAnimate_ = false;
    auto drawingCanvas = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(drawingCanvas, nullptr);
    rsUniRenderVisitor2->canvas_ = std::make_unique<RSPaintFilterCanvas>(drawingCanvas.get());
    RSSurfaceRenderNodeConfig config;
    std::vector<std::shared_ptr<RSSurfaceRenderNode>> hardwareEnabledNodes;
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(config);
    ASSERT_NE(node, nullptr);
    RectI DstRect(0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    node->SetDstRect(DstRect);
    hardwareEnabledNodes.emplace_back(node);
    std::shared_ptr<RSSurfaceRenderNode> node2 = std::make_shared<RSSurfaceRenderNode>(config);
    ASSERT_NE(node2, nullptr);
    hardwareEnabledNodes.emplace_back(node2);
    std::shared_ptr<RSSurfaceRenderNode> node3 = std::make_shared<RSSurfaceRenderNode>(config);
    ASSERT_NE(node3, nullptr);
    node3->shouldPaint_ = false;
    hardwareEnabledNodes.emplace_back(node3);
    rsUniRenderVisitor2->SetHardwareEnabledNodes(hardwareEnabledNodes);
    rsUniRenderVisitor->ClearTransparentBeforeSaveLayer();
}

/**
 * @tc.name: MarkSubHardwareEnableNodeStateTest001
 * @tc.desc: Test RSUniRenderVisitorTest.MarkSubHardwareEnableNodeStateTest
 * @tc.type: FUNC
 * @tc.require: issueI79KM8
 */
HWTEST_F(RSUniRenderVisitorTest, MarkSubHardwareEnableNodeStateTest001, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    config.id = 10;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    rsSurfaceRenderNode->InitRenderParams();
    rsSurfaceRenderNode->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_NODE;
    rsSurfaceRenderNode->name_ = "";
    rsUniRenderVisitor->MarkSubHardwareEnableNodeState(*rsSurfaceRenderNode);
}

/**
 * @tc.name: MarkSubHardwareEnableNodeStateTest002
 * @tc.desc: Test RSUniRenderVisitorTest.MarkSubHardwareEnableNodeStateTest
 * @tc.type: FUNC
 * @tc.require: issueI79KM8
 */
HWTEST_F(RSUniRenderVisitorTest, MarkSubHardwareEnableNodeStateTest002, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig config;
    config.id = 10;
    auto rsContext = std::make_shared<RSContext>();
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    rsSurfaceRenderNode->nodeType_ = RSSurfaceNodeType::ABILITY_COMPONENT_NODE;
    ASSERT_EQ(true, rsSurfaceRenderNode->IsAbilityComponent());

    auto newRsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->hardwareEnabledNodes_.emplace_back(newRsSurfaceRenderNode);
    rsUniRenderVisitor->MarkSubHardwareEnableNodeState(*rsSurfaceRenderNode);
}

/**
 * @tc.name: DrawAllSurfaceOpaqueRegionForDFX002
 * @tc.desc: Test RSUniRenderVisitorTest.DrawAllSurfaceOpaqueRegionForDFX when nodetype is SELF_DRAWING_NODE
 * @tc.type: FUNC
 * @tc.require: issueI79KM8
 */
HWTEST_F(RSUniRenderVisitorTest, DrawAllSurfaceOpaqueRegionForDFX002, TestSize.Level1)
{
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(id, config);
    auto surfaceNodeMain = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNodeMain, nullptr);
    surfaceNodeMain->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    node->curAllSurfaces_.push_back(surfaceNodeMain);
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_NODE;
    node->curAllSurfaces_.push_back(surfaceNode);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->DrawAllSurfaceOpaqueRegionForDFX(*node);
}

/**
 * @tc.name: DrawAndTraceSingleDirtyRegionTypeForDFX001
 * @tc.desc: Test RSUniRenderVisitorTest.DrawAndTraceSingleDirtyRegionTypeForDFX when dirtyType is out of range
 * @tc.type: FUNC
 * @tc.require: issueI79KM8
 */
HWTEST_F(RSUniRenderVisitorTest, DrawAndTraceSingleDirtyRegionTypeForDFX001, TestSize.Level1)
{
    auto surfaceNodeMain = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNodeMain, nullptr);
    surfaceNodeMain->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    auto dirtyManager = surfaceNodeMain->GetDirtyManager();
    ASSERT_NE(dirtyManager, nullptr);
    dirtyManager->dirtyCanvasNodeInfo_.resize(DirtyRegionType::TYPE_AMOUNT);
    dirtyManager->dirtySurfaceNodeInfo_.resize(DirtyRegionType::TYPE_AMOUNT);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->DrawAndTraceSingleDirtyRegionTypeForDFX(*surfaceNodeMain, DirtyRegionType::TYPE_AMOUNT, true);
}

/**
 * @tc.name: DrawAndTraceSingleDirtyRegionTypeForDFX002
 * @tc.desc: Test RSUniRenderVisitorTest.DrawAndTraceSingleDirtyRegionTypeForDFX when dirtyManager is nullptr
 * @tc.type: FUNC
 * @tc.require: issueI79KM8
 */
HWTEST_F(RSUniRenderVisitorTest, DrawAndTraceSingleDirtyRegionTypeForDFX002, TestSize.Level1)
{
    auto surfaceNodeMain = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNodeMain, nullptr);
    surfaceNodeMain->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    surfaceNodeMain->dirtyManager_ = nullptr;
    auto dirtyManager = surfaceNodeMain->GetDirtyManager();
    ASSERT_EQ(dirtyManager, nullptr);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->DrawAndTraceSingleDirtyRegionTypeForDFX(*surfaceNodeMain,
        DirtyRegionType::UPDATE_DIRTY_REGION, true);
}

/**
 * @tc.name: DrawAndTraceSingleDirtyRegionTypeForDFX003
 * @tc.desc: Test RSUniRenderVisitorTest.DrawAndTraceSingleDirtyRegionTypeForDFX when isDrawn is true
 * @tc.type: FUNC
 * @tc.require: issueI79KM8
 */
HWTEST_F(RSUniRenderVisitorTest, DrawAndTraceSingleDirtyRegionTypeForDFX003, TestSize.Level1)
{
    auto surfaceNodeMain = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNodeMain, nullptr);
    surfaceNodeMain->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    auto dirtyManager = surfaceNodeMain->GetDirtyManager();
    ASSERT_NE(dirtyManager, nullptr);
    NodeId id = 0;
    RectI rect(0, 0, 0, 0);
    dirtyManager->dirtyCanvasNodeInfo_.resize(DirtyRegionType::TYPE_AMOUNT);
    dirtyManager->dirtyCanvasNodeInfo_[DirtyRegionType::UPDATE_DIRTY_REGION].emplace(std::make_pair(id, rect));
    dirtyManager->dirtySurfaceNodeInfo_.resize(DirtyRegionType::TYPE_AMOUNT);
    dirtyManager->dirtySurfaceNodeInfo_[DirtyRegionType::UPDATE_DIRTY_REGION].emplace(std::make_pair(id, rect));
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->DrawAndTraceSingleDirtyRegionTypeForDFX(*surfaceNodeMain,
        DirtyRegionType::UPDATE_DIRTY_REGION, true);
}

/**
 * @tc.name: DrawAndTraceSingleDirtyRegionTypeForDFX004
 * @tc.desc: Test RSUniRenderVisitorTest.DrawAndTraceSingleDirtyRegionTypeForDFX when isDrawn is false
 * @tc.type: FUNC
 * @tc.require: issueI79KM8
 */
HWTEST_F(RSUniRenderVisitorTest, DrawAndTraceSingleDirtyRegionTypeForDFX004, TestSize.Level1)
{
    auto surfaceNodeMain = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNodeMain, nullptr);
    surfaceNodeMain->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    auto dirtyManager = surfaceNodeMain->GetDirtyManager();
    ASSERT_NE(dirtyManager, nullptr);
    dirtyManager->dirtyCanvasNodeInfo_.resize(DirtyRegionType::TYPE_AMOUNT);
    dirtyManager->dirtySurfaceNodeInfo_.resize(DirtyRegionType::TYPE_AMOUNT);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->DrawAndTraceSingleDirtyRegionTypeForDFX(*surfaceNodeMain,
        DirtyRegionType::UPDATE_DIRTY_REGION, false);
}

/**
 * @tc.name: DrawDetailedTypesOfDirtyRegionForDFX001
 * @tc.desc: Test RSUniRenderVisitorTest.DrawDetailedTypesOfDirtyRegionForDFX
 * @tc.type: FUNC
 * @tc.require: issueI79KM8
 */
HWTEST_F(RSUniRenderVisitorTest, DrawDetailedTypesOfDirtyRegionForDFX001, TestSize.Level1)
{
    auto surfaceNodeMain = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNodeMain, nullptr);
    auto dirtyManager = surfaceNodeMain->GetDirtyManager();
    ASSERT_NE(dirtyManager, nullptr);
    dirtyManager->dirtyCanvasNodeInfo_.resize(DirtyRegionType::TYPE_AMOUNT);
    dirtyManager->dirtySurfaceNodeInfo_.resize(DirtyRegionType::TYPE_AMOUNT);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->dirtyRegionDebugType_ = DirtyRegionDebugType::DISABLED;
    ASSERT_EQ(false, rsUniRenderVisitor->DrawDetailedTypesOfDirtyRegionForDFX(*surfaceNodeMain));
    rsUniRenderVisitor->dirtyRegionDebugType_ = DirtyRegionDebugType::CUR_DIRTY_DETAIL_ONLY_TRACE;
    ASSERT_EQ(true, rsUniRenderVisitor->DrawDetailedTypesOfDirtyRegionForDFX(*surfaceNodeMain));
    rsUniRenderVisitor->dirtyRegionDebugType_ = DirtyRegionDebugType::UPDATE_DIRTY_REGION;
    ASSERT_EQ(true, rsUniRenderVisitor->DrawDetailedTypesOfDirtyRegionForDFX(*surfaceNodeMain));
}

/**
 * @tc.name: CalcDirtyRegionForFilterNode001
 * @tc.desc: Test RSUniRenderVisitorTest.CalcDirtyRegionForFilterNode when dirtyManager_ of displayNode is nullptr
 * @tc.type: FUNC
 * @tc.require: issueI79KM8
 */
HWTEST_F(RSUniRenderVisitorTest, CalcDirtyRegionForFilterNode001, TestSize.Level1)
{
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(id, config);
    ASSERT_NE(node, nullptr);
    node->dirtyManager_ = nullptr;
    ASSERT_EQ(node->GetDirtyManager(), nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->dirtyManager_ = nullptr;
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    RectI rect(0, 0, DEFAULT_DIRTY_REGION_WIDTH, DEFAULT_DIRTY_REGION_HEIGHT);
    rsUniRenderVisitor->CalcDirtyRegionForFilterNode(rect, surfaceNode, node);
}

/**
 * @tc.name: CalcDirtyRegionForFilterNode002
 * @tc.desc: Test RSUniRenderVisitorTest.CalcDirtyRegionForFilterNode when dirtyManager_ of surfaceNode is nullptr
 * @tc.type: FUNC
 * @tc.require: issueI79KM8
 */
HWTEST_F(RSUniRenderVisitorTest, CalcDirtyRegionForFilterNode002, TestSize.Level1)
{
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(id, config);
    ASSERT_NE(node, nullptr);
    ASSERT_NE(node->GetDirtyManager(), nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->dirtyManager_ = nullptr;
    ASSERT_EQ(surfaceNode->GetDirtyManager(), nullptr);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    RectI rect(0, 0, DEFAULT_DIRTY_REGION_WIDTH, DEFAULT_DIRTY_REGION_HEIGHT);
    rsUniRenderVisitor->CalcDirtyRegionForFilterNode(rect, surfaceNode, node);
}

/**
 * @tc.name: CalcDirtyRegionForFilterNode003
 * @tc.desc: Test RSUniRenderVisitorTest.CalcDirtyRegionForFilterNode
 * @tc.type: FUNC
 * @tc.require: issueI79KM8
 */
HWTEST_F(RSUniRenderVisitorTest, CalcDirtyRegionForFilterNode003, TestSize.Level1)
{
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(id, config);
    ASSERT_NE(node, nullptr);
    auto dpDirtyManager = node->GetDirtyManager();
    ASSERT_NE(dpDirtyManager, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    auto sfDirtyManager = surfaceNode->GetDirtyManager();
    ASSERT_NE(sfDirtyManager, nullptr);
    sfDirtyManager->dirtyCanvasNodeInfo_.resize(DirtyRegionType::TYPE_AMOUNT);
    sfDirtyManager->dirtySurfaceNodeInfo_.resize(DirtyRegionType::TYPE_AMOUNT);
    dpDirtyManager->dirtyRegion_ = RectI(0, 0, 0, 0);
    sfDirtyManager->dirtyRegion_ = RectI(0, 0, 0, 0);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    RectI rect(0, 0, DEFAULT_DIRTY_REGION_WIDTH, DEFAULT_DIRTY_REGION_HEIGHT);
    rsUniRenderVisitor->CalcDirtyRegionForFilterNode(rect, surfaceNode, node);
    dpDirtyManager->dirtyRegion_ = RectI(0, 0, DEFAULT_DIRTY_REGION_WIDTH, DEFAULT_DIRTY_REGION_HEIGHT);
    sfDirtyManager->dirtyRegion_ = RectI(0, 0, DEFAULT_DIRTY_REGION_WIDTH, DEFAULT_DIRTY_REGION_HEIGHT);
    rsUniRenderVisitor->CalcDirtyRegionForFilterNode(rect, surfaceNode, node);
    dpDirtyManager->dirtyRegion_ = RectI(0, 0, 0, 0);
    std::shared_ptr<RSSurfaceRenderNode> surfaceNodeNull = nullptr;
    node->curAllSurfaces_.push_back(surfaceNodeNull);
    auto surfaceNodeNotApp = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNodeNotApp, nullptr);
    surfaceNodeNotApp->nodeType_ = RSSurfaceNodeType::ABILITY_COMPONENT_NODE;
    node->curAllSurfaces_.push_back(surfaceNodeNotApp);
    auto surfaceNodeAppWithEmptyRegion = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNodeAppWithEmptyRegion, nullptr);
    surfaceNodeAppWithEmptyRegion->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    ASSERT_NE(surfaceNodeAppWithEmptyRegion->GetDirtyManager(), nullptr);
    surfaceNodeAppWithEmptyRegion->GetDirtyManager()->dirtyRegion_ = RectI(0, 0, 0, 0);
    node->curAllSurfaces_.push_back(surfaceNodeAppWithEmptyRegion);
    auto surfaceNodeApp = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNodeApp, nullptr);
    surfaceNodeApp->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    ASSERT_NE(surfaceNodeApp->GetDirtyManager(), nullptr);
    surfaceNodeApp->GetDirtyManager()->dirtyRegion_ =
        RectI(0, 0, DEFAULT_DIRTY_REGION_WIDTH, DEFAULT_DIRTY_REGION_HEIGHT);
    node->curAllSurfaces_.push_back(surfaceNodeApp);
    node->curAllSurfaces_.push_back(surfaceNode);
    rsUniRenderVisitor->CalcDirtyRegionForFilterNode(rect, surfaceNode, node);
}

/**
 * @tc.name: CalcDirtyRegionForFilterNode004
 * @tc.desc: Test RSUniRenderVisitorTest.CalcDirtyRegionForFilterNode
 * @tc.type: FUNC
 * @tc.require: issueI79KM8
 */
HWTEST_F(RSUniRenderVisitorTest, CalcDirtyRegionForFilterNode004, TestSize.Level1)
{
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(id, config);
    ASSERT_NE(node, nullptr);
    auto dpDirtyManager = node->GetDirtyManager();
    ASSERT_NE(dpDirtyManager, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    auto sfDirtyManager = surfaceNode->GetDirtyManager();
    ASSERT_NE(sfDirtyManager, nullptr);
    sfDirtyManager->dirtyCanvasNodeInfo_.resize(DirtyRegionType::TYPE_AMOUNT);
    sfDirtyManager->dirtySurfaceNodeInfo_.resize(DirtyRegionType::TYPE_AMOUNT);
    RectI rect(0, 0, DEFAULT_DIRTY_REGION_WIDTH, DEFAULT_DIRTY_REGION_HEIGHT);
    sfDirtyManager->dirtyRegion_ = RectI(0, 0, DEFAULT_DIRTY_REGION_WIDTH, DEFAULT_DIRTY_REGION_HEIGHT);
    dpDirtyManager->dirtyRegion_ = RectI(0, 0, 0, 0);
    auto surfaceNodeApp = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNodeApp, nullptr);
    surfaceNodeApp->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    ASSERT_NE(surfaceNodeApp->GetDirtyManager(), nullptr);
    surfaceNodeApp->GetDirtyManager()->dirtyRegion_ =
        RectI(0, 0, DEFAULT_DIRTY_REGION_WIDTH, DEFAULT_DIRTY_REGION_HEIGHT);
    surfaceNodeApp->visibleRegion_ = Occlusion::Region(RectI(0, 0, 0, 0));
    node->curAllSurfaces_.push_back(surfaceNodeApp);

    auto surfaceNodeAppOutOfRange = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNodeAppOutOfRange, nullptr);
    surfaceNodeAppOutOfRange->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    ASSERT_NE(surfaceNodeAppOutOfRange->GetDirtyManager(), nullptr);
    surfaceNodeAppOutOfRange->GetDirtyManager()->dirtyRegion_ = RectI(DEFAULT_DIRTY_REGION_WIDTH,
        DEFAULT_DIRTY_REGION_HEIGHT, DEFAULT_DIRTY_REGION_WIDTH, DEFAULT_DIRTY_REGION_HEIGHT);
    surfaceNodeAppOutOfRange->visibleRegion_ =
        Occlusion::Region(RectI(0, 0, DEFAULT_DIRTY_REGION_WIDTH, DEFAULT_DIRTY_REGION_HEIGHT));
    node->curAllSurfaces_.push_back(surfaceNodeAppOutOfRange);
    auto dRect = surfaceNodeAppOutOfRange->GetDirtyManager()->GetDirtyRegion();

    auto surfaceNodeAppIntersect = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNodeAppIntersect, nullptr);
    surfaceNodeAppIntersect->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    ASSERT_NE(surfaceNodeAppIntersect->GetDirtyManager(), nullptr);
    surfaceNodeAppIntersect->GetDirtyManager()->dirtyRegion_ =
        RectI(0, 0, DEFAULT_DIRTY_REGION_WIDTH, DEFAULT_DIRTY_REGION_HEIGHT);
    surfaceNodeAppIntersect->visibleRegion_ =
        Occlusion::Region(RectI(0, 0, DEFAULT_DIRTY_REGION_WIDTH, DEFAULT_DIRTY_REGION_HEIGHT));
    node->curAllSurfaces_.push_back(surfaceNodeAppIntersect);
    dRect = surfaceNodeAppIntersect->GetDirtyManager()->GetDirtyRegion();
    node->curAllSurfaces_.push_back(surfaceNode);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->CalcDirtyRegionForFilterNode(rect, surfaceNode, node);
}

/*
 * @tc.name: IsWatermarkFlagChanged
 * @tc.desc: Test RSUniRenderVisitorTest.IsWatermarkFlagChanged test
 * @tc.type: FUNC
 * @tc.require: issuesIA8LNR
 */
HWTEST_F(RSUniRenderVisitorTest, IsWatermarkFlagChanged, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->IsWatermarkFlagChanged();
}

/**
 * @tc.name: CalcDirtyFilterRegion001
 * @tc.desc: Test RSUniRenderVisitorTest.CalcDirtyFilterRegion when disPlayNode or disPlayNode.dirtyManager_ is null
 * @tc.type: FUNC
 * @tc.require: issueI79KM8
 */
HWTEST_F(RSUniRenderVisitorTest, CalcDirtyFilterRegion001, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    NodeId id = 0;
    RSDisplayNodeConfig config;
    std::shared_ptr<RSDisplayRenderNode> node = nullptr;
    rsUniRenderVisitor->CalcDirtyFilterRegion(node);
    node = std::make_shared<RSDisplayRenderNode>(id, config);
    ASSERT_NE(node, nullptr);
    node->dirtyManager_ = nullptr;
    rsUniRenderVisitor->CalcDirtyFilterRegion(node);
}

/**
 * @tc.name: CalcDirtyFilterRegion002
 * @tc.desc: Test RSUniRenderVisitorTest.CalcDirtyFilterRegion when disPlayNode or disPlayNode.dirtyManager_ is null
 * @tc.type: FUNC
 * @tc.require: issueI79KM8
 */
HWTEST_F(RSUniRenderVisitorTest, CalcDirtyFilterRegion002, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(id, config);
    std::shared_ptr<RSSurfaceRenderNode> surfaceNodeNull = nullptr;
    node->curAllSurfaces_.push_back(surfaceNodeNull);
    auto surfaceNodeMain = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNodeMain, nullptr);
    surfaceNodeMain->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    auto surfaceNodeChild = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNodeChild, nullptr);
    surfaceNodeChild->oldDirtyInSurface_ = RectI(0, 0, DEFAULT_DIRTY_REGION_WIDTH, DEFAULT_DIRTY_REGION_HEIGHT);
    surfaceNodeMain->visibleRegion_ =
        Occlusion::Region(RectI(0, 0, DEFAULT_DIRTY_REGION_WIDTH, DEFAULT_DIRTY_REGION_HEIGHT));
    surfaceNodeMain->AddChildHardwareEnabledNode(surfaceNodeChild);
    auto surfaceNodeDirtyNull = RSTestUtil::CreateSurfaceNode();
    surfaceNodeDirtyNull->dirtyManager_ = nullptr;
    surfaceNodeDirtyNull->oldDirtyInSurface_ = RectI(0, 0, DEFAULT_DIRTY_REGION_WIDTH, DEFAULT_DIRTY_REGION_HEIGHT);
    surfaceNodeMain->AddChildHardwareEnabledNode(surfaceNodeDirtyNull);
    auto surfaceNodeChildLastFrame = RSTestUtil::CreateSurfaceNode();
    surfaceNodeChildLastFrame->isLastFrameHardwareEnabled_ = true;
    surfaceNodeChildLastFrame->oldDirtyInSurface_ = RectI(0, 0,
        DEFAULT_DIRTY_REGION_WIDTH, DEFAULT_DIRTY_REGION_HEIGHT);
    surfaceNodeMain->AddChildHardwareEnabledNode(surfaceNodeChildLastFrame);
    node->curAllSurfaces_.push_back(surfaceNodeMain);
    rsUniRenderVisitor->CalcDirtyFilterRegion(node);
}

/**
 * @tc.name: ProcessSurfaceRenderNode001
 * @tc.desc: Test RSUniRenderVisitorTest.ProcessSurfaceRenderNode when displaynode is null
 * @tc.type: FUNC
 * @tc.require: issueI79KM8
 */
HWTEST_F(RSUniRenderVisitorTest, ProcessSurfaceRenderNode001, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->isUIFirst_ = true;
    rsUniRenderVisitor->isSubThread_ = true;
    std::shared_ptr<RSDisplayRenderNode> node = nullptr;

    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetParent(node);
    auto drawingCanvas = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(drawingCanvas, nullptr);
    rsUniRenderVisitor->canvas_ = std::make_unique<RSPaintFilterCanvas>(drawingCanvas.get());
    rsUniRenderVisitor->ProcessSurfaceRenderNode(*surfaceNode);
}

/**
 * @tc.name: ProcessSurfaceRenderNode002
 * @tc.desc: Test RSUniRenderVisitorTest.ProcessSurfaceRenderNode with isSubThread_ and isUIFirst_
 * @tc.type: FUNC
 * @tc.require: issueI79KM8
 */
HWTEST_F(RSUniRenderVisitorTest, ProcessSurfaceRenderNode002, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->isUIFirst_ = true;
    rsUniRenderVisitor->isSubThread_ = true;
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(id, config);

    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    auto drawingCanvas = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(drawingCanvas, nullptr);
    rsUniRenderVisitor->canvas_ = std::make_unique<RSPaintFilterCanvas>(drawingCanvas.get());
    surfaceNode->SetParent(node);
    rsUniRenderVisitor->ProcessSurfaceRenderNode(*surfaceNode);
    rsUniRenderVisitor->isSubThread_ = false;
    rsUniRenderVisitor->ProcessSurfaceRenderNode(*surfaceNode);
    rsUniRenderVisitor->isUIFirst_ = false;
    rsUniRenderVisitor->ProcessSurfaceRenderNode(*surfaceNode);
}

/**
 * @tc.name: ProcessSurfaceRenderNode003
 * @tc.desc: Test RSUniRenderVisitorTest.ProcessSurfaceRenderNode with skipLayer
 * @tc.type: FUNC
 * @tc.require: issueI80HL4
 */
HWTEST_F(RSUniRenderVisitorTest, ProcessSurfaceRenderNode003, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->isUIFirst_ = true;
    rsUniRenderVisitor->isSubThread_ = true;
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(id, config);
    node->SetSecurityDisplay(true);
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetSkipLayer(true);
    auto drawingCanvas = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(drawingCanvas, nullptr);
    rsUniRenderVisitor->canvas_ = std::make_unique<RSPaintFilterCanvas>(drawingCanvas.get());
    surfaceNode->SetParent(node);
    rsUniRenderVisitor->ProcessSurfaceRenderNode(*surfaceNode);
}

/**
 * @tc.name: ProcessSurfaceRenderNode004
 * @tc.desc: Test RSUniRenderVisitorTest.ProcessSurfaceRenderNode with securityLayer
 * @tc.type: FUNC
 * @tc.require: issueI80HL4
 */
HWTEST_F(RSUniRenderVisitorTest, ProcessSurfaceRenderNode004, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->isUIFirst_ = true;
    rsUniRenderVisitor->isSubThread_ = true;
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(id, config);
    node->SetSecurityDisplay(true);
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetSecurityLayer(true);
    auto drawingCanvas = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(drawingCanvas, nullptr);
    rsUniRenderVisitor->canvas_ = std::make_unique<RSPaintFilterCanvas>(drawingCanvas.get());
    surfaceNode->SetParent(node);
    rsUniRenderVisitor->ProcessSurfaceRenderNode(*surfaceNode);
}

/**
 * @tc.name: ProcessSurfaceRenderNode005
 * @tc.desc: Test RSUniRenderVisitorTest.ProcessSurfaceRenderNode with capture window in directly render
 * @tc.type: FUNC
 * @tc.require: issueI80HL4
 */
HWTEST_F(RSUniRenderVisitorTest, ProcessSurfaceRenderNode005, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->isUIFirst_ = true;
    rsUniRenderVisitor->isSubThread_ = true;
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(id, config);
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    auto drawingCanvas = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(drawingCanvas, nullptr);
    rsUniRenderVisitor->canvas_ = std::make_unique<RSPaintFilterCanvas>(drawingCanvas.get());
    surfaceNode->SetParent(node);
    surfaceNode->name_ = CAPTURE_WINDOW_NAME;
    rsUniRenderVisitor->ProcessSurfaceRenderNode(*surfaceNode);
}

/**
 * @tc.name: ProcessSurfaceRenderNode006
 * @tc.desc: Test RSUniRenderVisitorTest.ProcessSurfaceRenderNode with capture window in offscreen render
 * @tc.type: FUNC
 * @tc.require: issueI80HL4
 */
HWTEST_F(RSUniRenderVisitorTest, ProcessSurfaceRenderNode006, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->isUIFirst_ = true;
    rsUniRenderVisitor->isSubThread_ = true;
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(id, config);
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    auto drawingCanvas = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(drawingCanvas, nullptr);
    rsUniRenderVisitor->canvas_ = std::make_unique<RSPaintFilterCanvas>(drawingCanvas.get());
    surfaceNode->SetParent(node);
    surfaceNode->name_ = CAPTURE_WINDOW_NAME;
    rsUniRenderVisitor->ProcessSurfaceRenderNode(*surfaceNode);
}

/**
 * @tc.name: ProcessSurfaceRenderNode007
 * @tc.desc: Test RSUniRenderVisitorTest.ProcessSurfaceRenderNode while surface node has protected layer
 * @tc.type: FUNC
 * @tc.require: issueI7ZSC2
 */
HWTEST_F(RSUniRenderVisitorTest, ProcessSurfaceRenderNode007, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    rsUniRenderVisitor->isUIFirst_ = true;
    rsUniRenderVisitor->isSubThread_ = true;
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(id, config);
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);

    auto drawingCanvas = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(drawingCanvas, nullptr);
    rsUniRenderVisitor->canvas_ = std::make_unique<RSPaintFilterCanvas>(drawingCanvas.get());
    surfaceNode->SetParent(node);
    surfaceNode->SetProtectedLayer(true);
    rsUniRenderVisitor->ProcessSurfaceRenderNode(*surfaceNode);
}

/**
 * @tc.name: ProcessSurfaceRenderNode008
 * @tc.desc: Test RSUniRenderVisitorTest.ProcessSurfaceRenderNode while surface node does not have protected layer
 * @tc.type: FUNC
 * @tc.require: issueI7ZSC2
 */
HWTEST_F(RSUniRenderVisitorTest, ProcessSurfaceRenderNode008, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    rsUniRenderVisitor->isUIFirst_ = true;
    rsUniRenderVisitor->isSubThread_ = true;
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(id, config);
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);

    auto drawingCanvas = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(drawingCanvas, nullptr);
    rsUniRenderVisitor->canvas_ = std::make_unique<RSPaintFilterCanvas>(drawingCanvas.get());
    surfaceNode->SetParent(node);
    surfaceNode->SetProtectedLayer(false);
    rsUniRenderVisitor->ProcessSurfaceRenderNode(*surfaceNode);
}

/**
 * @tc.name: GenerateNodeContentCache001
 * @tc.desc: Test RSUniRenderVisitorTest.GenerateNodeContentCache when surfaceNode is null
 * @tc.type: FUNC
 * @tc.require: issueI79KM8
 */
HWTEST_F(RSUniRenderVisitorTest, GenerateNodeContentCache001, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->drawingCacheType_ = RSDrawingCacheType::DISABLED_CACHE;
    ASSERT_EQ(rsUniRenderVisitor->GenerateNodeContentCache(*surfaceNode), false);
}

/**
 * @tc.name: GenerateNodeContentCache002
 * @tc.desc: Test RSUniRenderVisitorTest.GenerateNodeContentCache with isOnTheTree_
 * @tc.type: FUNC
 * @tc.require: issueI79KM8
 */
HWTEST_F(RSUniRenderVisitorTest, GenerateNodeContentCache002, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->isOnTheTree_ = true;
    ASSERT_EQ(rsUniRenderVisitor->GenerateNodeContentCache(*surfaceNode), false);

    auto surfaceNode2 = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode2, nullptr);
    surfaceNode2->isOnTheTree_ = false;
    ASSERT_EQ(rsUniRenderVisitor->GenerateNodeContentCache(*surfaceNode2), false);
}

/**
 * @tc.name: PrepareEffectRenderNode001
 * @tc.desc: Test RSUniRenderVisitorTest.PrepareEffectRenderNode api
 * @tc.type: FUNC
 * @tc.require: issueI79KM8
 */
HWTEST_F(RSUniRenderVisitorTest, PrepareEffectRenderNode001, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    NodeId id1 = 1;
    RSDisplayNodeConfig displayConfig;
    auto rsContext = std::make_shared<RSContext>();
    rsUniRenderVisitor->curDisplayNode_ = std::make_shared<RSDisplayRenderNode>(id1,
        displayConfig, rsContext->weak_from_this());
    rsUniRenderVisitor->curDisplayNode_->InitRenderParams();
    ASSERT_NE(rsUniRenderVisitor->curDisplayNode_, nullptr);
    NodeId id2 = 2;
    auto node = std::make_shared<RSEffectRenderNode>(id2, rsContext->weak_from_this());
    node->InitRenderParams();
    rsUniRenderVisitor->PrepareEffectRenderNode(*node);
}

/**
 * @tc.name: DrawDirtyRectForDFX001
 * @tc.desc: Test RSUniRenderVisitorTest.DrawDirtyRectForDFX api
 * @tc.type: FUNC
 * @tc.require: issueI79KM8
 */
HWTEST_F(RSUniRenderVisitorTest, DrawDirtyRectForDFX001, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    RectI rect(0, 0, 0, 0);
    Drawing::Color color;
    RSUniRenderVisitor::RSPaintStyle fillType = RSUniRenderVisitor::RSPaintStyle::FILL;
    rsUniRenderVisitor->DrawDirtyRectForDFX(rect, color, fillType, 0, 0);
}

/**
 * @tc.name: DrawDirtyRegionForDFX001
 * @tc.desc: Test RSUniRenderVisitorTest.DrawDirtyRegionForDFX api
 * @tc.type: FUNC
 * @tc.require: issueI79KM8
 */
HWTEST_F(RSUniRenderVisitorTest, DrawDirtyRegionForDFX001, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    RectI rect(0, 0, 0, 0);
    std::vector<RectI> dirtyRects;
    dirtyRects.push_back(rect);
    rsUniRenderVisitor->DrawDirtyRegionForDFX(dirtyRects);
}

/**
 * @tc.name: DrawAllSurfaceDirtyRegionForDFX001
 * @tc.desc: Test RSUniRenderVisitorTest.DrawAllSurfaceDirtyRegionForDFX api
 * @tc.type: FUNC
 * @tc.require: issueI79KM8
 */
HWTEST_F(RSUniRenderVisitorTest, DrawAllSurfaceDirtyRegionForDFX001, TestSize.Level1)
{
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(id, config);
    ASSERT_NE(node, nullptr);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    Occlusion::Region region(RectI(0, 0, 0, 0));
    rsUniRenderVisitor->DrawAllSurfaceDirtyRegionForDFX(*node, region);
}

/**
 * @tc.name: DrawSurfaceOpaqueRegionForDFX001
 * @tc.desc: Test RSUniRenderVisitorTest.DrawSurfaceOpaqueRegionForDFX api
 * @tc.type: FUNC
 * @tc.require: issueI79KM8
 */
HWTEST_F(RSUniRenderVisitorTest, DrawSurfaceOpaqueRegionForDFX001, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    Occlusion::Rect rect{0, 0, 0, 0};
    Occlusion::Region region{rect};
    surfaceNode->opaqueRegion_ = region;
    rsUniRenderVisitor->DrawSurfaceOpaqueRegionForDFX(*surfaceNode);
}

/**
 * @tc.name: DrawSpherize001
 * @tc.desc: Test RSUniRenderVisitorTest.DrawSpherize api
 * @tc.type: FUNC
 * @tc.require: issueI79KM8
 */
HWTEST_F(RSUniRenderVisitorTest, DrawSpherize001, TestSize.Level1)
{
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(id, config);
    node->InitRenderParams();
    ASSERT_NE(node, nullptr);
    auto drawingCanvas = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(drawingCanvas, nullptr);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->canvas_ = std::make_unique<RSPaintFilterCanvas>(drawingCanvas.get());
    rsUniRenderVisitor->DrawSpherize(*node);
}

/**
 * @tc.name: InitNodeCache001
 * @tc.desc: Test RSUniRenderVisitorTest.InitNodeCache api
 * @tc.type: FUNC
 * @tc.require: issueI79KM8
 */
HWTEST_F(RSUniRenderVisitorTest, InitNodeCache001, TestSize.Level1)
{
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(id, config);
    node->InitRenderParams();
    ASSERT_NE(node, nullptr);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->InitNodeCache(*node);
}

/**
 * @tc.name: UpdateCacheRenderNodeMap001
 * @tc.desc: Test RSUniRenderVisitorTest.UpdateCacheRenderNodeMap api
 * @tc.type: FUNC
 * @tc.require: issueI79KM8
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateCacheRenderNodeMap001, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(1, rsContext->weak_from_this());
    ASSERT_NE(canvasNode, nullptr);
    canvasNode->InitRenderParams();

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->UpdateCacheRenderNodeMap(*canvasNode);
}

/**
 * @tc.name: ProcessEffectRenderNode001
 * @tc.desc: Test RSUniRenderVisitorTest.ProcessEffectRenderNode api
 * @tc.type: FUNC
 * @tc.require: issueI79KM8
 */
HWTEST_F(RSUniRenderVisitorTest, ProcessEffectRenderNode001, TestSize.Level1)
{
    NodeId id = 0;
    RSEffectRenderNode node(id);
    node.stagingRenderParams_ = std::make_unique<RSRenderParams>(node.GetId());
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->ProcessEffectRenderNode(node);
}

/**
 * @tc.name: ClosePartialRenderWhenAnimatingWindows001
 * @tc.desc: Test RSUniRenderVisitorTest.ClosePartialRenderWhenAnimatingWindows api
 * @tc.type: FUNC
 * @tc.require: issueI79KM8
 */
HWTEST_F(RSUniRenderVisitorTest, ClosePartialRenderWhenAnimatingWindows001, TestSize.Level1)
{
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(id, config);
    node->InitRenderParams();
    ASSERT_NE(node, nullptr);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->ClosePartialRenderWhenAnimatingWindows(node);
}

/**
 * @tc.name: CheckColorSpace001
 * @tc.desc: Test RSUniRenderVisitorTest.CheckColorSpace while
 *           app Window node's color space is not equal to GRAPHIC_COLOR_GAMUT_SRGB
 * @tc.type: FUNC
 * @tc.require: issueI7O6WO
 */
HWTEST_F(RSUniRenderVisitorTest, CheckColorSpace001, TestSize.Level2)
{
    auto appWindowNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(appWindowNode, nullptr);
    appWindowNode->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    appWindowNode->SetColorSpace(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->colorGamutModes_.push_back(
        static_cast<ScreenColorGamut>(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3));
    rsUniRenderVisitor->CheckColorSpace(*appWindowNode);
    ASSERT_EQ(rsUniRenderVisitor->newColorSpace_, appWindowNode->GetColorSpace());
}

/**
 * @tc.name: DoDirectComposition001
 * @tc.desc: Test RSUniRenderVisitorTest.DoDirectComposition while the render visitor is hardware disabled
 * @tc.type: FUNC
 * @tc.require: issueI7O6WO
 */
HWTEST_F(RSUniRenderVisitorTest, DoDirectComposition001, TestSize.Level2)
{
    std::shared_ptr<RSContext> context = std::make_shared<RSContext>();
    const std::shared_ptr<RSBaseRenderNode> rootNode = context->GetGlobalRootRenderNode();
    ASSERT_NE(rootNode, nullptr);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    ASSERT_EQ(rsUniRenderVisitor->DoDirectComposition(rootNode), false);
}

/**
 * @tc.name: DoDirectComposition002
 * @tc.desc: Test RSUniRenderVisitorTest.DoDirectComposition while the root node doesn't have child
 * @tc.type: FUNC
 * @tc.require: issueI7O6WO
 */
HWTEST_F(RSUniRenderVisitorTest, DoDirectComposition002, TestSize.Level2)
{
    std::shared_ptr<RSContext> context = std::make_shared<RSContext>();
    const std::shared_ptr<RSBaseRenderNode> rootNode = context->GetGlobalRootRenderNode();
    ASSERT_NE(rootNode, nullptr);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    ASSERT_EQ(rsUniRenderVisitor->DoDirectComposition(rootNode), false);
}

/**
 * @tc.name: DoDirectComposition003
 * @tc.desc: Test RSUniRenderVisitorTest.DoDirectComposition while the root node has child
 * @tc.type: FUNC
 * @tc.require: issueI7O6WO
 */
HWTEST_F(RSUniRenderVisitorTest, DoDirectComposition003, TestSize.Level2)
{
    std::shared_ptr<RSContext> context = std::make_shared<RSContext>();
    const std::shared_ptr<RSBaseRenderNode> rootNode = context->GetGlobalRootRenderNode();
    NodeId id = 1;
    RSDisplayNodeConfig config;
    auto childDisplayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    ASSERT_NE(rootNode, nullptr);
    ASSERT_NE(childDisplayNode, nullptr);
    rootNode->AddChild(childDisplayNode, 0);
    childDisplayNode->SetCompositeType(RSDisplayRenderNode::CompositeType::SOFTWARE_COMPOSITE);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    ASSERT_EQ(rsUniRenderVisitor->DoDirectComposition(rootNode), false);
}

/**
 * @tc.name: DoDirectComposition004
 * @tc.desc: Test RSUniRenderVisitorTest.DoDirectComposition while
 *           the display node's composite type is UNI_RENDER_COMPOSITE
 * @tc.type: FUNC
 * @tc.require: issueI7O6WO
 */
HWTEST_F(RSUniRenderVisitorTest, DoDirectComposition004, TestSize.Level2)
{
    std::shared_ptr<RSContext> context = std::make_shared<RSContext>();
    const std::shared_ptr<RSBaseRenderNode> rootNode = context->GetGlobalRootRenderNode();
    NodeId id = 1;
    RSDisplayNodeConfig config;
    auto childDisplayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    ASSERT_NE(rootNode, nullptr);
    ASSERT_NE(childDisplayNode, nullptr);
    rootNode->AddChild(childDisplayNode, 0);
    childDisplayNode->SetCompositeType(RSDisplayRenderNode::CompositeType::UNI_RENDER_COMPOSITE);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    ASSERT_EQ(rsUniRenderVisitor->DoDirectComposition(rootNode), false);
}

/**
 * @tc.name: AdjustLocalZOrder001
 * @tc.desc: Test RSUniRenderVisitorTest.AdjustLocalZOrder for HardwareEnabled node
 * @tc.type: FUNC
 * @tc.require: issuesI7RNL4
 */
HWTEST_F(RSUniRenderVisitorTest, AdjustLocalZOrder001, TestSize.Level2)
{
    auto appWindowNode = RSTestUtil::CreateSurfaceNode();
    auto ChildHardwareEnabledNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(appWindowNode, nullptr);
    ASSERT_NE(ChildHardwareEnabledNode, nullptr);
    appWindowNode->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    appWindowNode->AddChildHardwareEnabledNode(ChildHardwareEnabledNode);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    rsUniRenderVisitor->CollectAppNodeForHwc(appWindowNode);
    ASSERT_NE(rsUniRenderVisitor->appWindowNodesInZOrder_.size(), 0);
}

/**
 * @tc.name: UpdateHardwareEnableList001
 * @tc.desc: Test RSUniRenderVisitorTest.UpdateHardwareEnableList while filterRects is empty
 * @tc.type: FUNC
 * @tc.require: issuesI7RNL4
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHardwareEnableList001, TestSize.Level2)
{
    auto firstNode = RSTestUtil::CreateSurfaceNode();
    auto secondNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(firstNode, nullptr);
    ASSERT_NE(secondNode, nullptr);
    RectI DstRect(0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    firstNode->SetDstRect(DstRect);

    using SurfaceDirtyMgrPair = std::pair<std::shared_ptr<RSSurfaceRenderNode>, std::shared_ptr<RSSurfaceRenderNode>>;
    vector<SurfaceDirtyMgrPair> validHwcNodes;
    vector<RectI> filterRects;
    validHwcNodes.push_back({firstNode, secondNode});

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    ASSERT_TRUE(rsUniRenderVisitor->UpdateHardwareEnableList(filterRects, validHwcNodes).IsEmpty());
}

/**
 * @tc.name: UpdateHardwareEnableList002
 * @tc.desc: Test RSUniRenderVisitorTest.UpdateHardwareEnableList while
 *           filterRects doesn't intersect with HWC node's rect
 * @tc.type: FUNC
 * @tc.require: issuesI7RNL4
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHardwareEnableList002, TestSize.Level2)
{
    auto firstNode = RSTestUtil::CreateSurfaceNode();
    auto secondNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(firstNode, nullptr);
    ASSERT_NE(secondNode, nullptr);
    RectI DstRect(0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    firstNode->SetDstRect(DstRect);

    using SurfaceDirtyMgrPair = std::pair<std::shared_ptr<RSSurfaceRenderNode>, std::shared_ptr<RSSurfaceRenderNode>>;
    vector<SurfaceDirtyMgrPair> validHwcNodes;
    vector<RectI> filterRects;
    validHwcNodes.push_back({firstNode, secondNode});
    RectI filterRect(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    filterRects.push_back(filterRect);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    ASSERT_TRUE(rsUniRenderVisitor->UpdateHardwareEnableList(filterRects, validHwcNodes).IsEmpty());
}

/**
 * @tc.name: UpdateHardwareEnableList003
 * @tc.desc: Test RSUniRenderVisitorTest.UpdateHardwareEnableList while
 *           filterRects intersects with with HWC node's rect
 * @tc.type: FUNC
 * @tc.require: issuesI7RNL4
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHardwareEnableList003, TestSize.Level2)
{
    auto firstNode = RSTestUtil::CreateSurfaceNode();
    auto secondNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(firstNode, nullptr);
    ASSERT_NE(secondNode, nullptr);
    RectI DstRect(0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    firstNode->SetDstRect(DstRect);

    using SurfaceDirtyMgrPair = std::pair<std::shared_ptr<RSSurfaceRenderNode>, std::shared_ptr<RSSurfaceRenderNode>>;
    vector<SurfaceDirtyMgrPair> validHwcNodes;
    vector<RectI> filterRects;
    validHwcNodes.push_back({firstNode, secondNode});
    RectI filterRect = DstRect;
    filterRects.push_back(filterRect);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    ASSERT_FALSE(rsUniRenderVisitor->UpdateHardwareEnableList(filterRects, validHwcNodes).IsEmpty());
}

/**
 * @tc.name: UpdateHardwareEnableList004
 * @tc.desc: Test RSUniRenderVisitorTest.UpdateHardwareEnableList while
 *           filterRects intersects with LastFrameHardwareEnabled node's rect
 * @tc.type: FUNC
 * @tc.require: issuesI7RNL4
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHardwareEnableList004, TestSize.Level2)
{
    auto firstNode = RSTestUtil::CreateSurfaceNode();
    auto secondNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(firstNode, nullptr);
    ASSERT_NE(secondNode, nullptr);
    RectI DstRect(0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    firstNode->SetDstRect(DstRect);
    firstNode->isLastFrameHardwareEnabled_ = true;

    using SurfaceDirtyMgrPair = std::pair<std::shared_ptr<RSSurfaceRenderNode>, std::shared_ptr<RSSurfaceRenderNode>>;
    vector<SurfaceDirtyMgrPair> validHwcNodes;
    vector<RectI> filterRects;
    validHwcNodes.push_back({firstNode, secondNode});
    RectI filterRect = DstRect;
    filterRects.push_back(filterRect);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    NodeId id = 1;
    RSDisplayNodeConfig config;
    rsUniRenderVisitor->curDisplayNode_ = std::make_shared<RSDisplayRenderNode>(id, config);
    rsUniRenderVisitor->UpdateHardwareEnableList(filterRects, validHwcNodes);
}

/**
 * @tc.name: AddContainerDirtyToGlobalDirty001
 * @tc.desc: Test RSUniRenderVisitorTest.AddContainerDirtyToGlobalDirty while
             currentFrameDirtyRegion_ doesn't intersect with containerRegion_
 * @tc.type: FUNC
 * @tc.require: issuesI7RNL4
 */
HWTEST_F(RSUniRenderVisitorTest, AddContainerDirtyToGlobalDirty001, TestSize.Level2)
{
    NodeId id = 1;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(displayNode, nullptr);
    ASSERT_NE(surfaceNode, nullptr);
    displayNode->InitRenderParams();
    displayNode->GetCurAllSurfaces().push_back(surfaceNode);
    surfaceNode->containerConfig_.hasContainerWindow_ = true;
    Occlusion::Rect rect = Occlusion::Rect(0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    surfaceNode->GetContainerRegion().GetBoundRef() = rect;

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->AddContainerDirtyToGlobalDirty(displayNode);
    ASSERT_TRUE(displayNode->GetDirtyManager()->GetCurrentFrameDirtyRegion().IsEmpty());
}

/**
 * @tc.name: AddContainerDirtyToGlobalDirty002
 * @tc.desc: Test RSUniRenderVisitorTest.AddContainerDirtyToGlobalDirty while
 *           currentFrameDirtyRegion_ intersect with containerRegion_
 * @tc.type: FUNC
 * @tc.require: issuesI7RNL4
 */
HWTEST_F(RSUniRenderVisitorTest, AddContainerDirtyToGlobalDirty002, TestSize.Level2)
{
    NodeId id = 1;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(displayNode, nullptr);
    ASSERT_NE(surfaceNode, nullptr);
    displayNode->GetCurAllSurfaces().push_back(surfaceNode);
    surfaceNode->containerConfig_.hasContainerWindow_ = true;
    Occlusion::Rect rect = Occlusion::Rect(0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    surfaceNode->GetContainerRegion().GetBoundRef() = rect;
    surfaceNode->GetContainerRegion().GetRegionRectsRef().push_back(rect);
    surfaceNode->GetDirtyManager()->MergeDirtyRect(RectI(0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT));

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->AddContainerDirtyToGlobalDirty(displayNode);
    ASSERT_FALSE(displayNode->GetDirtyManager()->GetCurrentFrameDirtyRegion().IsEmpty());
}

/**
 * @tc.name: CheckIfSurfaceRenderNodeNeedProcess001
 * @tc.desc: Test RSUniRenderVisitorTest.CheckIfSurfaceRenderNodeNeedProcess for skip layer
 * @tc.type: FUNC
 * @tc.require: issuesI7RNL4
 */
HWTEST_F(RSUniRenderVisitorTest, CheckIfSurfaceRenderNodeNeedProcess001, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetSkipLayer(true);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->isSecurityDisplay_ = true;
    bool keepFilterCache = false;
    ASSERT_FALSE(rsUniRenderVisitor->CheckIfSurfaceRenderNodeNeedProcess(*surfaceNode, keepFilterCache));
}

/**
 * @tc.name: CheckIfSurfaceRenderNodeNeedProcess002
 * @tc.desc: Test RSUniRenderVisitorTest.CheckIfSurfaceRenderNodeNeedProcess for empty ability component
 * @tc.type: FUNC
 * @tc.require: issuesI7RNL4
 */
HWTEST_F(RSUniRenderVisitorTest, CheckIfSurfaceRenderNodeNeedProcess002, TestSize.Level2)
{
    auto abilityComponentNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(abilityComponentNode, nullptr);
    abilityComponentNode->GetMutableRenderProperties().SetVisible(true);
    abilityComponentNode->SetSurfaceNodeType(RSSurfaceNodeType::ABILITY_COMPONENT_NODE);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    bool keepFilterCache = false;
    ASSERT_FALSE(rsUniRenderVisitor->CheckIfSurfaceRenderNodeNeedProcess(*abilityComponentNode, keepFilterCache));
}

/**
 * @tc.name: ClosePartialRenderWhenAnimatingWindows002
 * @tc.desc: Test RSUniRenderVisitorTest.ClosePartialRenderWhenAnimatingWindows while
 *           appWindowNum_ isn't bigger than PHONE_MAX_APP_WINDOW_NUM
 * @tc.type: FUNC
 * @tc.require: issuesI7SAJC
 */
HWTEST_F(RSUniRenderVisitorTest, ClosePartialRenderWhenAnimatingWindows002, TestSize.Level2)
{
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    ASSERT_NE(displayNode, nullptr);
    RectI rect(0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    displayNode->GetDirtyManager()->surfaceRect_ = rect;

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->SetAnimateState(true);
    rsUniRenderVisitor->SetAppWindowNum(1);
    rsUniRenderVisitor->ClosePartialRenderWhenAnimatingWindows(displayNode);
    ASSERT_TRUE(displayNode->GetDirtyManager()->GetCurrentFrameDirtyRegion().IsEmpty());
}

/**
 * @tc.name: ClosePartialRenderWhenAnimatingWindows003
 * @tc.desc: Test RSUniRenderVisitorTest.ClosePartialRenderWhenAnimatingWindows while
 *           appWindowNum_ is bigger than PHONE_MAX_APP_WINDOW_NUM
 * @tc.type: FUNC
 * @tc.require: issuesI7SAJC
 */
HWTEST_F(RSUniRenderVisitorTest, ClosePartialRenderWhenAnimatingWindows003, TestSize.Level2)
{
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    ASSERT_NE(displayNode, nullptr);
    RectI rect(0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    displayNode->GetDirtyManager()->surfaceRect_ = rect;

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->SetAnimateState(true);
    rsUniRenderVisitor->SetAppWindowNum(2);
    rsUniRenderVisitor->ClosePartialRenderWhenAnimatingWindows(displayNode);
    ASSERT_EQ(displayNode->GetDirtyManager()->GetCurrentFrameDirtyRegion(), rect);
}

/**
 * @tc.name: DrawChildRenderNode001
 * @tc.desc: Test RSUniRenderVisitorTest.DrawChildRenderNode api while
 *           the canvas node's cacheType_ is ANIMATE_PROPERTY
 * @tc.type: FUNC
 * @tc.require: issuesI7SAJC
 */
HWTEST_F(RSUniRenderVisitorTest, DrawChildRenderNode001, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(1, rsContext->weak_from_this());
    ASSERT_NE(canvasNode, nullptr);
    canvasNode->SetCacheType(CacheType::ANIMATE_PROPERTY);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    auto drawingCanvas = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    ASSERT_NE(drawingCanvas, nullptr);
    rsUniRenderVisitor->canvas_ = std::make_unique<RSPaintFilterCanvas>(drawingCanvas.get());
    rsUniRenderVisitor->DrawChildRenderNode(*canvasNode);
}

/*
 * @tc.name: MarkSubHardwareEnableNodeState003
 * @tc.desc: Test RSUniRenderVisitorTest.MarkSubHardwareEnableNodeStateTest for self drawing node
 * @tc.type: FUNC
 * @tc.require: issuesI7SAJC
 */
HWTEST_F(RSUniRenderVisitorTest, MarkSubHardwareEnableNodeState003, TestSize.Level2)
{
    auto selfDrawingNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(selfDrawingNode, nullptr);
    selfDrawingNode->SetSurfaceNodeType(RSSurfaceNodeType::SELF_DRAWING_NODE);
    selfDrawingNode->SetHardwareEnabled(true);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->MarkSubHardwareEnableNodeState(*selfDrawingNode);
    ASSERT_TRUE(selfDrawingNode->IsHardwareForcedDisabled());
}

/*
 * @tc.name: MarkSubHardwareEnableNodeState004
 * @tc.desc: Test RSUniRenderVisitorTest.MarkSubHardwareEnableNodeStateTest for app window node's child
 * @tc.type: FUNC
 * @tc.require: issuesI7SAJC
 */
HWTEST_F(RSUniRenderVisitorTest, MarkSubHardwareEnableNodeState004, TestSize.Level2)
{
    auto appWindowNode = RSTestUtil::CreateSurfaceNode();
    auto hardwareEnabledNode = RSTestUtil::CreateSurfaceNode();
    appWindowNode->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    appWindowNode->AddChildHardwareEnabledNode(hardwareEnabledNode);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->MarkSubHardwareEnableNodeState(*appWindowNode);
    ASSERT_TRUE(hardwareEnabledNode->IsHardwareForcedDisabled());
}

/*
 * @tc.name: PrepareTypesOfSurfaceRenderNodeBeforeUpdate001
 * @tc.desc: Test RSUniRenderVisitorTest.PrepareTypesOfSurfaceRenderNodeBeforeUpdateTest for DFX enabled
 * @tc.type: FUNC
 * @tc.require: issuesI7SAJC
 */
HWTEST_F(RSUniRenderVisitorTest, PrepareTypesOfSurfaceRenderNodeBeforeUpdate001, TestSize.Level2)
{
    auto leashWindowNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(leashWindowNode, nullptr);
    leashWindowNode->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->isTargetDirtyRegionDfxEnabled_ = true;
    rsUniRenderVisitor->dfxTargetSurfaceNames_.push_back(leashWindowNode->GetName());
    rsUniRenderVisitor->PrepareTypesOfSurfaceRenderNodeBeforeUpdate(*leashWindowNode);
    ASSERT_TRUE(rsUniRenderVisitor->curSurfaceDirtyManager_->IsTargetForDfx());
}

/*
 * @tc.name: PrepareTypesOfSurfaceRenderNodeBeforeUpdate002
 * @tc.desc: Test RSUniRenderVisitorTest.PrepareTypesOfSurfaceRenderNodeBeforeUpdateTest for self drawing node
 * @tc.type: FUNC
 * @tc.require: issuesI7SAJC
 */
HWTEST_F(RSUniRenderVisitorTest, PrepareTypesOfSurfaceRenderNodeBeforeUpdate002, TestSize.Level2)
{
    auto selfDrawingNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(selfDrawingNode, nullptr);
    ASSERT_NE(surfaceNode, nullptr);
    selfDrawingNode->SetSurfaceNodeType(RSSurfaceNodeType::SELF_DRAWING_NODE);
    selfDrawingNode->SetHardwareEnabled(true);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;

    rsUniRenderVisitor->PrepareTypesOfSurfaceRenderNodeBeforeUpdate(*selfDrawingNode);
    ASSERT_FALSE(rsUniRenderVisitor->curSurfaceNode_->GetChildHardwareEnabledNodes().empty());
}

/**
 * @tc.name: CheckColorSpaceWithSelfDrawingNode001
 * @tc.desc: Test RSUniRenderVisitorTest.CheckColorSpaceWithSelfDrawingNode while
 *           selfDrawingNode's color space is not equal to GRAPHIC_COLOR_GAMUT_SRGB,
 *           and this node will be drawn with gpu
 * @tc.type: FUNC
 * @tc.require: issueIAW3W0
 */
HWTEST_F(RSUniRenderVisitorTest, CheckColorSpaceWithSelfDrawingNode001, TestSize.Level2)
{
    auto selfDrawingNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(selfDrawingNode, nullptr);
    selfDrawingNode->SetHardwareForcedDisabledState(true);
    selfDrawingNode->SetColorSpace(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
    selfDrawingNode->SetIsOnTheTree(true);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->CheckColorSpaceWithSelfDrawingNode(*selfDrawingNode);
    ASSERT_EQ(rsUniRenderVisitor->newColorSpace_, selfDrawingNode->GetColorSpace());
}

/**
 * @tc.name: CheckColorSpaceWithSelfDrawingNode002
 * @tc.desc: Test RSUniRenderVisitorTest.CheckColorSpaceWithSelfDrawingNode while
 *           selfDrawingNode's color space is not equal to GRAPHIC_COLOR_GAMUT_SRGB,
 *           and this node will not be drawn with gpu
 * @tc.type: FUNC
 * @tc.require: issueIAW3W0
 */
HWTEST_F(RSUniRenderVisitorTest, CheckColorSpaceWithSelfDrawingNode002, TestSize.Level2)
{
    auto selfDrawingNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(selfDrawingNode, nullptr);
    selfDrawingNode->SetProtectedLayer(true);
    selfDrawingNode->SetColorSpace(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
    selfDrawingNode->SetIsOnTheTree(true);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->CheckColorSpaceWithSelfDrawingNode(*selfDrawingNode);
    ASSERT_NE(rsUniRenderVisitor->newColorSpace_, selfDrawingNode->GetColorSpace());
}


#ifndef ROSEN_CROSS_PLATFORM
/**
 * @tc.name: UpdateColorSpaceWithMetadata
 * @tc.desc: test results of UpdateColorSpaceWithMetadata, if node has no buffer
 * @tc.type: FUNC
 * @tc.require: issueIAW3W0
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateColorSpaceWithMetadata001, TestSize.Level1)
{
    // register instance root node
    auto instanceRootNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(instanceRootNode, nullptr);
    auto rsContext = std::make_shared<RSContext>();
    auto& nodeMap = rsContext->GetMutableNodeMap();
    NodeId instanceRootNodeId = instanceRootNode->GetId();
    pid_t instanceRootNodePid = ExtractPid(instanceRootNodeId);
    nodeMap.renderNodeMap_[instanceRootNodePid][instanceRootNodeId] = instanceRootNode;
    // create subsurface node
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->context_ = rsContext;
    surfaceNode->instanceRootNodeId_ = instanceRootNode->GetId();

    ASSERT_NE(surfaceNode->GetInstanceRootNode(), nullptr);
    surfaceNode->UpdateColorSpaceWithMetadata();
    ASSERT_EQ(surfaceNode->GetColorSpace(), GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
}

/**
 * @tc.name: UpdateColorSpaceWithMetadata
 * @tc.desc: test results of UpdateColorSpaceWithMetadata, if node has buffer
 * @tc.type: FUNC
 * @tc.require: issueIAW3W0
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateColorSpaceWithMetadata002, TestSize.Level1)
{
    // register instance root node
    auto instanceRootNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(instanceRootNode, nullptr);
    auto rsContext = std::make_shared<RSContext>();
    auto& nodeMap = rsContext->GetMutableNodeMap();
    NodeId instanceRootNodeId = instanceRootNode->GetId();
    pid_t instanceRootNodePid = ExtractPid(instanceRootNodeId);
    nodeMap.renderNodeMap_[instanceRootNodePid][instanceRootNodeId] = instanceRootNode;
    // create subsurface node
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    const sptr<SurfaceBuffer>& buffer = surfaceNode->GetRSSurfaceHandler()->GetBuffer();
    ASSERT_NE(buffer, nullptr);
    surfaceNode->context_ = rsContext;
    surfaceNode->instanceRootNodeId_ = instanceRootNode->GetId();

    ASSERT_NE(surfaceNode->GetInstanceRootNode(), nullptr);
    surfaceNode->UpdateColorSpaceWithMetadata();
    ASSERT_EQ(surfaceNode->GetColorSpace(), GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
}
#endif

/*
 * @tc.name: UpdateColorSpaceAfterHwcCalc_001
 * @tc.desc: Test UpdateColorSpaceAfterHwcCalc when there is a P3 selfDrawingNode.
 * @tc.type: FUNC
 * @tc.require: issueIAW3W0
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateColorSpaceAfterHwcCalc_001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto selfDrawingNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(selfDrawingNode, nullptr);
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    ASSERT_NE(displayNode, nullptr);
    selfDrawingNode->SetAncestorDisplayNode(displayNode);
    selfDrawingNode->SetHardwareForcedDisabledState(true);
    selfDrawingNode->SetColorSpace(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3);

    rsUniRenderVisitor->UpdateColorSpaceAfterHwcCalc(*displayNode);
    ASSERT_EQ(rsUniRenderVisitor->newColorSpace_, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
}

/*
 * @tc.name: ResetCurSurfaceInfoAsUpperSurfaceParent001
 * @tc.desc: Reset but keep single node's surfaceInfo
 * @tc.type: FUNC
 * @tc.require: issuesI8MQCS
 */
HWTEST_F(RSUniRenderVisitorTest, ResetCurSurfaceInfoAsUpperSurfaceParent001, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto upperSurfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    ASSERT_NE(upperSurfaceNode, nullptr);
    surfaceNode->SetParent(upperSurfaceNode);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;

    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, config);

    rsUniRenderVisitor->curDisplayNode_ = displayNode;
    rsUniRenderVisitor->ResetCurSurfaceInfoAsUpperSurfaceParent(*surfaceNode);
    ASSERT_EQ(rsUniRenderVisitor->curSurfaceNode_, surfaceNode);
}

/*
 * @tc.name: ResetCurSurfaceInfoAsUpperSurfaceParent002
 * @tc.desc: Reset but keep node's surfaceInfo since upper surface's InstanceRootNode is not registered
 * @tc.type: FUNC
 * @tc.require: issuesI8MQCS
 */
HWTEST_F(RSUniRenderVisitorTest, ResetCurSurfaceInfoAsUpperSurfaceParent002, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto upperSurfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    ASSERT_NE(upperSurfaceNode, nullptr);
    upperSurfaceNode->instanceRootNodeId_ = upperSurfaceNode->GetId();
    ASSERT_EQ(upperSurfaceNode->GetInstanceRootNode(), nullptr);
    surfaceNode->SetParent(std::weak_ptr<RSSurfaceRenderNode>(upperSurfaceNode));

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;

    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, config);

    rsUniRenderVisitor->curDisplayNode_ = displayNode;
    rsUniRenderVisitor->ResetCurSurfaceInfoAsUpperSurfaceParent(*surfaceNode);
    ASSERT_EQ(rsUniRenderVisitor->curSurfaceNode_, surfaceNode);
}

/*
 * @tc.name: UpdateHwcNodeEnableByNodeBelow
 * @tc.desc: Test RSUniRenderVistorTest.UpdateHwcNodeEnableByNodeBelow
 * @tc.type: FUNC
 * @tc.require: issuesI8MQCS
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnableByNodeBelow, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    
    NodeId displayNodeId = 1;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(displayNodeId, config);

    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    auto hwcNode1 = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(hwcNode1, nullptr);
    surfaceConfig.id = 2;
    auto hwcNode2 = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(hwcNode2, nullptr);
    hwcNode1->SetIsOnTheTree(true);
    hwcNode2->SetIsOnTheTree(false);
    surfaceNode->AddChildHardwareEnabledNode(hwcNode1);
    surfaceNode->AddChildHardwareEnabledNode(hwcNode2);

    displayNode->curMainAndLeashSurfaceNodes_.push_back(surfaceNode);
    rsUniRenderVisitor->curDisplayNode_ = displayNode;
    rsUniRenderVisitor->UpdateHwcNodeEnableByNodeBelow();
}

/*
 * @tc.name: PrepareSurfaceRenderNode001
 * @tc.desc: Test RSUniRenderVisitorTest.PrepareSurfaceRenderNode while surface node has security layer
 * @tc.type: FUNC
 * @tc.require: issuesI7SAJC
 */
HWTEST_F(RSUniRenderVisitorTest, PrepareSurfaceRenderNode001, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetSecurityLayer(true);
    surfaceNode->SetIsOnTheTree(true, surfaceNode->GetId(), surfaceNode->GetId());
    surfaceNode->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;

    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    ASSERT_NE(displayNode, nullptr);
    displayNode->InitRenderParams();

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->isQuickSkipPreparationEnabled_ = true;
    rsUniRenderVisitor->curDisplayNode_ = displayNode;
    rsUniRenderVisitor->displayHasSecSurface_[rsUniRenderVisitor->currentVisitDisplay_] = false;
    rsUniRenderVisitor->PrepareSurfaceRenderNode(*surfaceNode);
    ASSERT_EQ(rsUniRenderVisitor->displayHasSecSurface_[rsUniRenderVisitor->currentVisitDisplay_], true);
}

/*
 * @tc.name: PrepareSurfaceRenderNode002
 * @tc.desc: Test RSUniRenderVisitorTest.PrepareSurfaceRenderNode while surface node has skip layer
 * @tc.type: FUNC
 * @tc.require: issuesI7SAJC
 */
HWTEST_F(RSUniRenderVisitorTest, PrepareSurfaceRenderNode002, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetSkipLayer(true);
    surfaceNode->SetIsOnTheTree(true, surfaceNode->GetId(), surfaceNode->GetId());
    surfaceNode->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;

    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    ASSERT_NE(displayNode, nullptr);
    displayNode->InitRenderParams();

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->isQuickSkipPreparationEnabled_ = true;
    rsUniRenderVisitor->curDisplayNode_ = displayNode;
    rsUniRenderVisitor->displayHasSkipSurface_[rsUniRenderVisitor->currentVisitDisplay_] = false;
    rsUniRenderVisitor->PrepareSurfaceRenderNode(*surfaceNode);
    ASSERT_EQ(rsUniRenderVisitor->displayHasSkipSurface_[rsUniRenderVisitor->currentVisitDisplay_], true);
}

/*
 * @tc.name: PrepareSurfaceRenderNode003
 * @tc.desc: Test RSUniRenderVisitorTest.PrepareSurfaceRenderNode while surface node is capture window
 * @tc.type: FUNC
 * @tc.require: issuesI7SAJC
 */
HWTEST_F(RSUniRenderVisitorTest, PrepareSurfaceRenderNode003, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetIsOnTheTree(true, surfaceNode->GetId(), surfaceNode->GetId());
    surfaceNode->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    surfaceNode->name_ = CAPTURE_WINDOW_NAME;

    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    ASSERT_NE(displayNode, nullptr);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->isQuickSkipPreparationEnabled_ = true;
    rsUniRenderVisitor->curDisplayNode_ = displayNode;
    rsUniRenderVisitor->hasCaptureWindow_[rsUniRenderVisitor->currentVisitDisplay_] = false;
    rsUniRenderVisitor->PrepareSurfaceRenderNode(*surfaceNode);
    ASSERT_EQ(rsUniRenderVisitor->hasCaptureWindow_[rsUniRenderVisitor->currentVisitDisplay_], true);
}

/*
 * @tc.name: PrepareSurfaceRenderNode004
 * @tc.desc: Test RSUniRenderVisitorTest.PrepareSurfaceRenderNode while surface node has protected layer
 * @tc.type: FUNC
 * @tc.require: issueI7ZSC2
 */
HWTEST_F(RSUniRenderVisitorTest, PrepareSurfaceRenderNode004, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetIsOnTheTree(true, surfaceNode->GetId(), surfaceNode->GetId());
    surfaceNode->SetProtectedLayer(true);

    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    ASSERT_NE(displayNode, nullptr);
    displayNode->InitRenderParams();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->isQuickSkipPreparationEnabled_ = true;
    rsUniRenderVisitor->curDisplayNode_ = displayNode;
    rsUniRenderVisitor->displayHasProtectedSurface_[rsUniRenderVisitor->currentVisitDisplay_] = true;
    rsUniRenderVisitor->PrepareSurfaceRenderNode(*surfaceNode);
}

/*
 * @tc.name: PrepareSurfaceRenderNode005
 * @tc.desc: Test RSUniRenderVisitorTest.PrepareSurfaceRenderNode while surface node does not have protected layer
 * @tc.type: FUNC
 * @tc.require: issueI7ZSC2
 */
HWTEST_F(RSUniRenderVisitorTest, PrepareSurfaceRenderNode005, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);

    surfaceNode->SetIsOnTheTree(true, surfaceNode->GetId(), surfaceNode->GetId());
    surfaceNode->SetProtectedLayer(false);

    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    ASSERT_NE(displayNode, nullptr);
    displayNode->InitRenderParams();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->isQuickSkipPreparationEnabled_ = true;
    rsUniRenderVisitor->curDisplayNode_ = displayNode;
    rsUniRenderVisitor->displayHasProtectedSurface_[rsUniRenderVisitor->currentVisitDisplay_] = false;
    rsUniRenderVisitor->PrepareSurfaceRenderNode(*surfaceNode);
}

/*
 * @tc.name: PrepareDisplayRenderNode001
 * @tc.desc: Test RSUniRenderVisitorTest.PrepareDisplayRenderNode while dsiplay node has security surface
 * @tc.type: FUNC
 * @tc.require: issuesI7SAJC
 */
HWTEST_F(RSUniRenderVisitorTest, PrepareDisplayRenderNode001, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetSecurityLayer(true);
    surfaceNode->SetIsOnTheTree(true, surfaceNode->GetId(), surfaceNode->GetId());
    surfaceNode->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;

    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    ASSERT_NE(displayNode, nullptr);
    displayNode->InitRenderParams();
    ASSERT_NE(displayNode->GetDirtyManager(), nullptr);
    displayNode->AddChild(surfaceNode, 0);
    displayNode->GenerateFullChildrenList();

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    ASSERT_NE(CreateOrGetScreenManager(), nullptr);
    rsUniRenderVisitor->isQuickSkipPreparationEnabled_ = true;
    rsUniRenderVisitor->curDisplayNode_ = displayNode;
    rsUniRenderVisitor->displayHasSecSurface_[rsUniRenderVisitor->currentVisitDisplay_] = false;

    rsUniRenderVisitor->PrepareDisplayRenderNode(*displayNode);
    ASSERT_EQ(rsUniRenderVisitor->displayHasSecSurface_[rsUniRenderVisitor->currentVisitDisplay_], true);
}

/*
 * @tc.name: PrepareDisplayRenderNode002
 * @tc.desc: Test RSUniRenderVisitorTest.PrepareDisplayRenderNode while dsiplay node has skip surface
 * @tc.type: FUNC
 * @tc.require: issuesI7SAJC
 */
HWTEST_F(RSUniRenderVisitorTest, PrepareDisplayRenderNode002, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetSkipLayer(true);
    surfaceNode->SetIsOnTheTree(true, surfaceNode->GetId(), surfaceNode->GetId());
    surfaceNode->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;

    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    ASSERT_NE(displayNode, nullptr);
    displayNode->InitRenderParams();
    ASSERT_NE(displayNode->GetDirtyManager(), nullptr);
    displayNode->AddChild(surfaceNode, 0);
    displayNode->GenerateFullChildrenList();

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    ASSERT_NE(CreateOrGetScreenManager(), nullptr);
    rsUniRenderVisitor->isQuickSkipPreparationEnabled_ = true;
    rsUniRenderVisitor->curDisplayNode_ = displayNode;
    rsUniRenderVisitor->displayHasSkipSurface_[rsUniRenderVisitor->currentVisitDisplay_] = false;

    rsUniRenderVisitor->PrepareDisplayRenderNode(*displayNode);
    ASSERT_EQ(rsUniRenderVisitor->displayHasSkipSurface_[rsUniRenderVisitor->currentVisitDisplay_], true);
}

/*
 * @tc.name: PrepareDisplayRenderNode003
 * @tc.desc: Test RSUniRenderVisitorTest.PrepareDisplayRenderNode while dsiplay node has capture window surface
 * @tc.type: FUNC
 * @tc.require: issuesI7SAJC
 */
HWTEST_F(RSUniRenderVisitorTest, PrepareDisplayRenderNode003, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetIsOnTheTree(true, surfaceNode->GetId(), surfaceNode->GetId());
    surfaceNode->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    surfaceNode->name_ = CAPTURE_WINDOW_NAME;

    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    ASSERT_NE(displayNode, nullptr);
    displayNode->InitRenderParams();
    ASSERT_NE(displayNode->GetDirtyManager(), nullptr);
    displayNode->AddChild(surfaceNode, 0);
    displayNode->GenerateFullChildrenList();

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    ASSERT_NE(CreateOrGetScreenManager(), nullptr);
    rsUniRenderVisitor->isQuickSkipPreparationEnabled_ = true;
    rsUniRenderVisitor->curDisplayNode_ = displayNode;
    rsUniRenderVisitor->hasCaptureWindow_[rsUniRenderVisitor->currentVisitDisplay_] = false;

    rsUniRenderVisitor->PrepareDisplayRenderNode(*displayNode);
    ASSERT_EQ(rsUniRenderVisitor->hasCaptureWindow_[rsUniRenderVisitor->currentVisitDisplay_], true);
}

/*
 * @tc.name: AssignGlobalZOrderAndCreateLayer001
 * @tc.desc: Test RSUniRenderVisitorTest.AssignGlobalZOrderAndCreateLayerTest
 *           while appWindowNodesInZOrder_ is empty
 * @tc.type: FUNC
 * @tc.require: issuesI7SAJC
 */
HWTEST_F(RSUniRenderVisitorTest, AssignGlobalZOrderAndCreateLayer001, TestSize.Level2)
{
    auto hardwareEnabledNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(hardwareEnabledNode, nullptr);
    hardwareEnabledNode->InitRenderParams();

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->hardwareEnabledNodes_.push_back(hardwareEnabledNode);

    std::vector<std::shared_ptr<RSSurfaceRenderNode>> nodeList;
    auto oldZOrder = rsUniRenderVisitor->globalZOrder_;
    rsUniRenderVisitor->AssignGlobalZOrderAndCreateLayer(nodeList);
    ASSERT_EQ(rsUniRenderVisitor->globalZOrder_, oldZOrder);
}

/*
 * @tc.name: AssignGlobalZOrderAndCreateLayer002
 * @tc.desc: Test RSUniRenderVisitorTest.AssignGlobalZOrderAndCreateLayerTest
 *           while app window nodes don't have child
 * @tc.type: FUNC
 * @tc.require: issuesI7SAJC
 */
HWTEST_F(RSUniRenderVisitorTest, AssignGlobalZOrderAndCreateLayer002, TestSize.Level2)
{
    auto hardwareEnabledNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto appWindowNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(hardwareEnabledNode, nullptr);
    ASSERT_NE(appWindowNode, nullptr);
    hardwareEnabledNode->InitRenderParams();
    appWindowNode->InitRenderParams();
    appWindowNode->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->hardwareEnabledNodes_.push_back(hardwareEnabledNode);

    std::vector<std::shared_ptr<RSSurfaceRenderNode>> nodeList;
    nodeList.push_back(appWindowNode);
    auto oldZOrder = rsUniRenderVisitor->globalZOrder_;
    rsUniRenderVisitor->AssignGlobalZOrderAndCreateLayer(nodeList);
    ASSERT_EQ(rsUniRenderVisitor->globalZOrder_, oldZOrder);
}

/*
 * @tc.name: AssignGlobalZOrderAndCreateLayer003
 * @tc.desc: Test RSUniRenderVisitorTest.AssignGlobalZOrderAndCreateLayerTest
 *           while app window nodes's child is HardwareForcedDisabled
 * @tc.type: FUNC
 * @tc.require: issuesI7SAJC
 */
HWTEST_F(RSUniRenderVisitorTest, AssignGlobalZOrderAndCreateLayer003, TestSize.Level2)
{
    auto hardwareEnabledNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto appWindowNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto childNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(hardwareEnabledNode, nullptr);
    ASSERT_NE(appWindowNode, nullptr);
    ASSERT_NE(childNode, nullptr);
    hardwareEnabledNode->InitRenderParams();
    appWindowNode->InitRenderParams();
    childNode->InitRenderParams();
    appWindowNode->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    appWindowNode->AddChildHardwareEnabledNode(childNode);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->hardwareEnabledNodes_.push_back(hardwareEnabledNode);

    std::vector<std::shared_ptr<RSSurfaceRenderNode>> nodeList;
    nodeList.push_back(appWindowNode);
    auto oldZOrder = rsUniRenderVisitor->globalZOrder_;
    rsUniRenderVisitor->AssignGlobalZOrderAndCreateLayer(nodeList);
    ASSERT_EQ(rsUniRenderVisitor->globalZOrder_, oldZOrder);
}

/*
 * @tc.name: AssignGlobalZOrderAndCreateLayer004
 * @tc.desc: Test RSUniRenderVisitorTest.AssignGlobalZOrderAndCreateLayerTest
 *           while app window nodes's child isn't HardwareForcedDisabled
 * @tc.type: FUNC
 * @tc.require: issuesI7SAJC
 */
HWTEST_F(RSUniRenderVisitorTest, AssignGlobalZOrderAndCreateLayer004, TestSize.Level2)
{
    auto hardwareEnabledNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto appWindowNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto childNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(hardwareEnabledNode, nullptr);
    ASSERT_NE(appWindowNode, nullptr);
    ASSERT_NE(childNode, nullptr);
    hardwareEnabledNode->InitRenderParams();
    appWindowNode->InitRenderParams();
    childNode->InitRenderParams();
    appWindowNode->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    appWindowNode->AddChildHardwareEnabledNode(childNode);
    childNode->dstRect_ = RectI(0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    childNode->isOnTheTree_ = true;

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->processor_ = RSProcessorFactory::CreateProcessor(
        RSDisplayRenderNode::CompositeType::UNI_RENDER_COMPOSITE);
    rsUniRenderVisitor->hardwareEnabledNodes_.push_back(hardwareEnabledNode);

    std::vector<std::shared_ptr<RSSurfaceRenderNode>> nodeList;
    nodeList.push_back(appWindowNode);
    auto oldZOrder = rsUniRenderVisitor->globalZOrder_;
    rsUniRenderVisitor->AssignGlobalZOrderAndCreateLayer(nodeList);
    ASSERT_NE(rsUniRenderVisitor->globalZOrder_, oldZOrder);
}

/*
 * @tc.name: CalcDirtyDisplayRegion002
 * @tc.desc: Test RSUniRenderVisitorTest.CalcDirtyDisplayRegion while surface node's shadow is valid
 * @tc.type: FUNC
 * @tc.require: issuesI7T9RE
 */
HWTEST_F(RSUniRenderVisitorTest, CalcDirtyDisplayRegion002, TestSize.Level2)
{
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(displayNode, nullptr);
    ASSERT_NE(surfaceNode, nullptr);
    displayNode->InitRenderParams();
    surfaceNode->InitRenderParams();
    displayNode->GetCurAllSurfaces().push_back(surfaceNode);
    surfaceNode->GetMutableRenderProperties().SetShadowRadius(1.0f);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->CalcDirtyDisplayRegion(displayNode);
    ASSERT_FALSE(surfaceNode->IsShadowValidLastFrame());
}

/*
 * @tc.name: CalcDirtyDisplayRegion003
 * @tc.desc: Test RSUniRenderVisitorTest.CalcDirtyDisplayRegion while
 *           surface node's shadow is valid and its shadow dirty rect isn't empty
 * @tc.type: FUNC
 * @tc.require: issuesI7T9RE
 */
HWTEST_F(RSUniRenderVisitorTest, CalcDirtyDisplayRegion003, TestSize.Level2)
{
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(displayNode, nullptr);
    ASSERT_NE(surfaceNode, nullptr);
    displayNode->GetCurAllSurfaces().push_back(surfaceNode);
    surfaceNode->GetMutableRenderProperties().SetShadowRadius(1.0f);
    RectI rect=RectI(0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    surfaceNode->oldDirtyInSurface_ = rect;
    surfaceNode->GetDirtyManager()->MergeDirtyRect(rect);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->CalcDirtyDisplayRegion(displayNode);
    ASSERT_EQ(displayNode->GetDirtyManager()->GetCurrentFrameDirtyRegion(), rect);
}

/*
 * @tc.name: CalcDirtyDisplayRegion004
 * @tc.desc: Test RSUniRenderVisitorTest.CalcDirtyDisplayRegion while
 *           surface node is shadow valid last frame
 * @tc.type: FUNC
 * @tc.require: issuesI7T9RE
 */
HWTEST_F(RSUniRenderVisitorTest, CalcDirtyDisplayRegion004, TestSize.Level2)
{
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(displayNode, nullptr);
    ASSERT_NE(surfaceNode, nullptr);
    displayNode->GetCurAllSurfaces().push_back(surfaceNode);
    surfaceNode->SetShadowValidLastFrame(true);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->CalcDirtyDisplayRegion(displayNode);
    ASSERT_FALSE(surfaceNode->IsShadowValidLastFrame());
}

/*
 * @tc.name: CalcDirtyDisplayRegion005
 * @tc.desc: Test RSUniRenderVisitorTest.CalcDirtyDisplayRegion while
 *           surface node is shadow valid last frame and its shadow dirty rect isn't empty
 * @tc.type: FUNC
 * @tc.require: issuesI7T9RE
 */
HWTEST_F(RSUniRenderVisitorTest, CalcDirtyDisplayRegion005, TestSize.Level2)
{
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(displayNode, nullptr);
    ASSERT_NE(surfaceNode, nullptr);
    displayNode->GetCurAllSurfaces().push_back(surfaceNode);
    surfaceNode->SetShadowValidLastFrame(true);
    RectI rect=RectI(0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    surfaceNode->oldDirtyInSurface_ = rect;
    surfaceNode->GetDirtyManager()->MergeDirtyRect(rect);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->CalcDirtyDisplayRegion(displayNode);
    ASSERT_EQ(displayNode->GetDirtyManager()->GetCurrentFrameDirtyRegion(), rect);
}

/*
 * @tc.name: UpdateHardwareNodeStatusBasedOnFilter001
 * @tc.desc: Test RSUniRenderVisitorTest.UpdateHardwareNodeStatusBasedOnFilter while
 *           prevHwcEnabledNodes first node's rect intersect app window node's is empty
 * @tc.type: FUNC
 * @tc.require: issuesI7T9RE
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHardwareNodeStatusBasedOnFilter001, TestSize.Level2)
{
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    auto appWindowNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto childNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(displayNode, nullptr);
    ASSERT_NE(appWindowNode, nullptr);
    ASSERT_NE(childNode, nullptr);
    appWindowNode->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    appWindowNode->AddChildHardwareEnabledNode(childNode);
    auto dirtyManager = displayNode->GetDirtyManager();

    using SurfaceDirtyMgrPair = std::pair<std::shared_ptr<RSSurfaceRenderNode>,
        std::shared_ptr<RSSurfaceRenderNode>>;
    vector<SurfaceDirtyMgrPair> prevHwcEnabledNodes;
    auto firstNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto secondNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(firstNode, nullptr);
    ASSERT_NE(secondNode, nullptr);
    prevHwcEnabledNodes.push_back({firstNode, secondNode});

    appWindowNode->visibleRegion_ =
        Occlusion::Rect(0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    RectI rect=RectI(0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    childNode->oldDirtyInSurface_ = rect;

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->UpdateHardwareNodeStatusBasedOnFilter(appWindowNode,
        prevHwcEnabledNodes, dirtyManager);
    ASSERT_NE(prevHwcEnabledNodes.size(), 1);
}

/*
 * @tc.name: UpdateHardwareNodeStatusBasedOnFilter002
 * @tc.desc: Test RSUniRenderVisitorTest.UpdateHardwareNodeStatusBasedOnFilter while
 *           prevHwcEnabledNodes first node's rect intersect app window node's isn't empty
 * @tc.type: FUNC
 * @tc.require: issuesI7T9RE
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHardwareNodeStatusBasedOnFilter002, TestSize.Level2)
{NodeId id = 0;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    auto appWindowNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto childNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(displayNode, nullptr);
    ASSERT_NE(appWindowNode, nullptr);
    ASSERT_NE(childNode, nullptr);
    appWindowNode->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    appWindowNode->AddChildHardwareEnabledNode(childNode);
    auto dirtyManager = displayNode->GetDirtyManager();

    using SurfaceDirtyMgrPair = std::pair<std::shared_ptr<RSSurfaceRenderNode>,
        std::shared_ptr<RSSurfaceRenderNode>>;
    vector<SurfaceDirtyMgrPair> prevHwcEnabledNodes;
    auto firstNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto secondNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(firstNode, nullptr);
    ASSERT_NE(secondNode, nullptr);
    prevHwcEnabledNodes.push_back({firstNode, secondNode});

    appWindowNode->visibleRegion_ =
        Occlusion::Rect(0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    RectI rect=RectI(0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    childNode->oldDirtyInSurface_ = rect;
    childNode->SetDstRect(rect);
    firstNode->SetDstRect(rect);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->UpdateHardwareNodeStatusBasedOnFilter(appWindowNode,
        prevHwcEnabledNodes, dirtyManager);
    ASSERT_EQ(prevHwcEnabledNodes.size(), 1);
}

/**
 * @tc.name: UpdateSubTreeInCache001
 * @tc.desc: Test RSUniRenderVisitorTest.UpdateSubTreeInCache while
 *           node doesn't have Child
 * @tc.type: FUNC
 * @tc.require: issueI7UGLR
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateSubTreeInCache001, TestSize.Level2)
{
    NodeId id = 0;
    auto node = std::make_shared<RSRenderNode>(id);

    ASSERT_NE(node, nullptr);
    node->InitRenderParams();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->UpdateSubTreeInCache(node, *node->GetSortedChildren());
    ASSERT_EQ(node->GetDrawingCacheChanged(), false);
}

/**
 * @tc.name: UpdateSubTreeInCache002
 * @tc.desc: Test RSUniRenderVisitorTest.UpdateSubTreeInCache while
 *           node has Child
 * @tc.type: FUNC
 * @tc.require: issueI7UGLR
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateSubTreeInCache002, TestSize.Level2)
{
    NodeId id = 0;
    auto node = std::make_shared<RSRenderNode>(id);
    ASSERT_NE(node, nullptr);
    node->InitRenderParams();
    NodeId id2 = 1;
    auto child = std::make_shared<RSRenderNode>(id2);
    ASSERT_NE(child, nullptr);

    node->AddChild(child);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->UpdateSubTreeInCache(node, *node->GetSortedChildren());
    ASSERT_EQ(node->GetDrawingCacheChanged(), false);
}


/**
 * @tc.name: DrawTargetSurfaceVisibleRegionForDFX001
 * @tc.desc: Test RSUniRenderVisitorTest.DrawTargetSurfaceVisibleRegionForDFX while
 *           RSDisplayRenderNode is the same as displafConfig set
 * @tc.type: FUNC
 * @tc.require: issueI7UGLR
 */
HWTEST_F(RSUniRenderVisitorTest, DrawTargetSurfaceVisibleRegionForDFX001, TestSize.Level2)
{
    RSDisplayNodeConfig displayConfig;
    auto rsContext = std::make_shared<RSContext>();
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(20, displayConfig, rsContext->weak_from_this());
    ASSERT_NE(rsDisplayRenderNode, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    rsDisplayRenderNode->GetCurAllSurfaces().push_back(surfaceNode);
    rsDisplayRenderNode->GetCurAllSurfaces().push_back(nullptr);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->dfxTargetSurfaceNames_.emplace_back(surfaceNode->GetName());
    Occlusion::Rect rect{0, 0, 0, 0};
    Occlusion::Region region{rect};
    surfaceNode->visibleRegion_ = region;
    rsUniRenderVisitor->DrawTargetSurfaceVisibleRegionForDFX(*rsDisplayRenderNode);
}

/**
 * @tc.name: DrawCurrentRefreshRate001
 * @tc.desc: Test RSUniRenderVisitorTest.DrawCurrentRefreshRate while
 *           currentRefreshRate is equal to 0
 * @tc.type: FUNC
 * @tc.require: issueI7UGLR
 */
HWTEST_F(RSUniRenderVisitorTest, DrawCurrentRefreshRate001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();

    auto drawingCanvas = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    RSDisplayNodeConfig displayConfig;
    auto rsContext = std::make_shared<RSContext>();
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(20, displayConfig, rsContext->weak_from_this());
    ASSERT_NE(rsDisplayRenderNode, nullptr);

    ASSERT_NE(drawingCanvas, nullptr);
    rsUniRenderVisitor->canvas_ = std::make_unique<RSPaintFilterCanvas>(drawingCanvas.get());
    rsUniRenderVisitor->DrawCurrentRefreshRate(0, 0, *rsDisplayRenderNode);
}


/**
 * @tc.name: ProcessChildrenForScreenRecordingOptimization001
 * @tc.desc: Test RSUniRenderVisitorTest.ProcessChildrenForScreenRecordingOptimization while
 *           isSubThread_ is equal to false
 * @tc.type: FUNC
 * @tc.require: issueI7UGLR
 */
HWTEST_F(RSUniRenderVisitorTest, ProcessChildrenForScreenRecordingOptimization001, TestSize.Level2)
{
    NodeId id = 0;
    RSDisplayNodeConfig displayConfig;
    auto rsContext = std::make_shared<RSContext>();
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(20, displayConfig, rsContext->weak_from_this());
    ASSERT_NE(rsDisplayRenderNode, nullptr);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->ProcessChildrenForScreenRecordingOptimization(*rsDisplayRenderNode, id);
}

/**
 * @tc.name: ProcessChildrenForScreenRecordingOptimization002
 * @tc.desc: Test RSUniRenderVisitorTest.ProcessChildrenForScreenRecordingOptimization while
 *           isSubThread_ is equal to true
 * @tc.type: FUNC
 * @tc.require: issueI7UGLR
 */
HWTEST_F(RSUniRenderVisitorTest, ProcessChildrenForScreenRecordingOptimization002, TestSize.Level2)
{
    NodeId id = 0;
    RSDisplayNodeConfig displayConfig;
    auto rsContext = std::make_shared<RSContext>();
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(20, displayConfig, rsContext->weak_from_this());
    ASSERT_NE(rsDisplayRenderNode, nullptr);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->isSubThread_ = true;
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->ProcessChildrenForScreenRecordingOptimization(*rsDisplayRenderNode, id);
}

/**
 * @tc.name: ProcessChildInner001
 * @tc.desc: Test RSUniRenderVisitorTest.ProcessChildInner while
 *           node has child
 * @tc.type: FUNC
 * @tc.require: issueI7UGLR
 */
HWTEST_F(RSUniRenderVisitorTest, ProcessChildInner001, TestSize.Level2)
{
    NodeId id = 0;
    NodeId id2 = 1;
    RSRenderNode *node = new RSRenderNode(id2);
    auto child = std::make_shared<RSRenderNode>(id);
    ASSERT_NE(child, nullptr);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->ProcessChildInner(*node, child);
}

/**
 * @tc.name: SwitchColorFilterDrawing001
 * @tc.desc: Test RSUniRenderVisitorTest.SwitchColorFilterDrawing while
 *           currentSaveCount == 0
 * @tc.type: FUNC
 * @tc.require: issueI7UGLR
 */
HWTEST_F(RSUniRenderVisitorTest, SwitchColorFilterDrawing001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->SwitchColorFilterDrawing(0);
}

/**
 * @tc.name: FindInstanceChildOfDisplay001
 * @tc.desc: Test FindInstanceChildOfDisplay while node is nullptr
 * @tc.type: FUNC
 * @tc.require: issueI904G4
 */
HWTEST_F(RSUniRenderVisitorTest, FindInstanceChildOfDisplay001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    ASSERT_EQ(rsUniRenderVisitor->FindInstanceChildOfDisplay(nullptr), INVALID_NODEID);
}

/**
 * @tc.name: FindInstanceChildOfDisplay002
 * @tc.desc: Test FindInstanceChildOfDisplay while node's parent is nullptr
 * @tc.type: FUNC
 * @tc.require: issueI904G4
 */
HWTEST_F(RSUniRenderVisitorTest, FindInstanceChildOfDisplay002, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->GetParent().reset();

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    ASSERT_EQ(rsUniRenderVisitor->FindInstanceChildOfDisplay(surfaceNode), INVALID_NODEID);
}

/**
 * @tc.name: FindInstanceChildOfDisplay003
 * @tc.desc: Test FindInstanceChildOfDisplay while node's parent is display node
 * @tc.type: FUNC
 * @tc.require: issueI904G4
 */
HWTEST_F(RSUniRenderVisitorTest, FindInstanceChildOfDisplay003, TestSize.Level2)
{
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    ASSERT_NE(displayNode, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    displayNode->AddChild(surfaceNode);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    ASSERT_EQ(rsUniRenderVisitor->FindInstanceChildOfDisplay(surfaceNode), surfaceNode->GetId());
}

/**
 * @tc.name: FindInstanceChildOfDisplay004
 * @tc.desc: Test FindInstanceChildOfDisplay while node's parent isn't display node
 * @tc.type: FUNC
 * @tc.require: issueI904G4
 */
HWTEST_F(RSUniRenderVisitorTest, FindInstanceChildOfDisplay004, TestSize.Level2)
{
    RSDisplayNodeConfig config;
    NodeId id = 0;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(id++);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(displayNode, nullptr);
    ASSERT_NE(canvasNode, nullptr);
    ASSERT_NE(surfaceNode, nullptr);

    displayNode->AddChild(canvasNode);
    canvasNode->AddChild(surfaceNode);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    ASSERT_EQ(rsUniRenderVisitor->FindInstanceChildOfDisplay(surfaceNode), canvasNode->GetId());
}

/**
 * @tc.name: CheckIfNeedResetRotate001
 * @tc.desc: Test CheckIfNeedResetRotate while canvas is null
 * @tc.type: FUNC
 * @tc.require: issueI981R9
 */
HWTEST_F(RSUniRenderVisitorTest, CheckIfNeedResetRotate001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    rsUniRenderVisitor->canvas_ = nullptr;
    ASSERT_EQ(rsUniRenderVisitor->CheckIfNeedResetRotate(), true);
}

/**
 * @tc.name: CheckIfNeedResetRotate002
 * @tc.desc: Test CheckIfNeedResetRotate for different rotate degree in canvas
 * @tc.type: FUNC
 * @tc.require: issueI981R9
 */
HWTEST_F(RSUniRenderVisitorTest, CheckIfNeedResetRotate002, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto drawingCanvas = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(drawingCanvas, nullptr);
    rsUniRenderVisitor->canvas_ = std::make_unique<RSPaintFilterCanvas>(drawingCanvas.get());

    // canvas rotate degree = 0
    ASSERT_EQ(rsUniRenderVisitor->CheckIfNeedResetRotate(), false);
    // canvas rotate degree = 90
    rsUniRenderVisitor->canvas_->Rotate(ROTATION_90, 0, 0);
    ASSERT_EQ(rsUniRenderVisitor->CheckIfNeedResetRotate(), true);
    // canvas rotate degree = 180
    rsUniRenderVisitor->canvas_->Rotate(ROTATION_90, 0, 0);
    ASSERT_EQ(rsUniRenderVisitor->CheckIfNeedResetRotate(), true);
    // canvas rotate degree = 270
    rsUniRenderVisitor->canvas_->Rotate(ROTATION_90, 0, 0);
    ASSERT_EQ(rsUniRenderVisitor->CheckIfNeedResetRotate(), true);
    // canvas rotate degree = 360
    rsUniRenderVisitor->canvas_->Rotate(ROTATION_90, 0, 0);
    ASSERT_EQ(rsUniRenderVisitor->CheckIfNeedResetRotate(), false);
}

/**
 * @tc.name: CheckIfNeedResetRotate003
 * @tc.desc: Test CheckIfNeedResetRotate for different rotate degree in display node
 * @tc.type: FUNC
 * @tc.require: issueI981R9
 */
HWTEST_F(RSUniRenderVisitorTest, CheckIfNeedResetRotate003, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto drawingCanvas = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(drawingCanvas, nullptr);
    rsUniRenderVisitor->canvas_ = std::make_unique<RSPaintFilterCanvas>(drawingCanvas.get());

    // display node rotate degree = 0
    rsUniRenderVisitor->displayNodeMatrix_ = Drawing::Matrix();
    ASSERT_EQ(rsUniRenderVisitor->CheckIfNeedResetRotate(), false);
    // display node rotate degree = 90
    rsUniRenderVisitor->displayNodeMatrix_.value().PostRotate(ROTATION_90, 0, 0);
    ASSERT_EQ(rsUniRenderVisitor->CheckIfNeedResetRotate(), true);
    // display node rotate degree = 180
    rsUniRenderVisitor->displayNodeMatrix_.value().PostRotate(ROTATION_90, 0, 0);
    ASSERT_EQ(rsUniRenderVisitor->CheckIfNeedResetRotate(), true);
    // display node rotate degree = 270
    rsUniRenderVisitor->displayNodeMatrix_.value().PostRotate(ROTATION_90, 0, 0);
    ASSERT_EQ(rsUniRenderVisitor->CheckIfNeedResetRotate(), true);
    // display node rotate degree = 360
    rsUniRenderVisitor->displayNodeMatrix_.value().PostRotate(ROTATION_90, 0, 0);
    ASSERT_EQ(rsUniRenderVisitor->CheckIfNeedResetRotate(), false);
}

/**
 * @tc.name: CheckIfNeedResetRotate004
 * @tc.desc: Test CheckIfNeedResetRotate for different rotate degree in canvas and display node
 * @tc.type: FUNC
 * @tc.require: issueI981R9
 */
HWTEST_F(RSUniRenderVisitorTest, CheckIfNeedResetRotate004, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto drawingCanvas = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(drawingCanvas, nullptr);
    rsUniRenderVisitor->canvas_ = std::make_unique<RSPaintFilterCanvas>(drawingCanvas.get());

    // canvas rotate degree = 90
    rsUniRenderVisitor->canvas_->Rotate(ROTATION_90, 0, 0);

    // display node rotate degree = 0
    rsUniRenderVisitor->displayNodeMatrix_ = Drawing::Matrix();
    ASSERT_EQ(rsUniRenderVisitor->CheckIfNeedResetRotate(), true);
    // display node rotate degree = 90
    rsUniRenderVisitor->displayNodeMatrix_.value().PostRotate(ROTATION_90, 0, 0);
    ASSERT_EQ(rsUniRenderVisitor->CheckIfNeedResetRotate(), true);
    // display node rotate degree = 180
    rsUniRenderVisitor->displayNodeMatrix_.value().PostRotate(ROTATION_90, 0, 0);
    ASSERT_EQ(rsUniRenderVisitor->CheckIfNeedResetRotate(), true);
    // display node rotate degree = 270
    rsUniRenderVisitor->displayNodeMatrix_.value().PostRotate(ROTATION_90, 0, 0);
    ASSERT_EQ(rsUniRenderVisitor->CheckIfNeedResetRotate(), false);
}

/**
 * @tc.name: DrawEffectRenderNodeForDFX001
 * @tc.desc: Test RSUniRenderVisitorTest.DrawEffectRenderNodeForDFX while
 *           rect map is empty.
 * @tc.type: FUNC
 * @tc.require: issueI8WJXC
 */
HWTEST_F(RSUniRenderVisitorTest, DrawEffectRenderNodeForDFX001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->DrawEffectRenderNodeForDFX();
}

/**
 * @tc.name: ProcessShadowFirst001
 * @tc.desc: Test RSUniRenderVisitorTest.ProcessShadowFirst while SetUseShadowBatching true.
 * @tc.type:FUNC
 * @tc.require:issueI8WJXC
 */
HWTEST_F(RSUniRenderVisitorTest, ProcessShadowFirst001, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    auto drawingCanvas = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(drawingCanvas, nullptr);
    rsUniRenderVisitor->canvas_ = std::make_unique<RSPaintFilterCanvas>(drawingCanvas.get());

    NodeId id = 0;
    auto node = std::make_shared<RSCanvasRenderNode>(id);
    ASSERT_NE(node, nullptr);
    node->InitRenderParams();
    NodeId id1 = 1;
    auto child1 = std::make_shared<RSCanvasRenderNode>(id1);
    ASSERT_NE(child1, nullptr);
    NodeId id2 = 2;
    auto child2 = std::make_shared<RSCanvasRenderNode>(id2);
    ASSERT_NE(child2, nullptr);
    node->AddChild(child1);
    node->AddChild(child2);

    auto& prop1 = child1->GetMutableRenderProperties();
    prop1.SetShadowRadius(100.0);

    auto& prop2 = child2->GetMutableRenderProperties();
    prop2.SetShadowRadius(100.0);

    auto& property = node->GetMutableRenderProperties();
    property.SetUseShadowBatching(true);
    const auto& prop =  node->GetRenderProperties();
    ASSERT_TRUE(prop.GetUseShadowBatching());
    rsUniRenderVisitor->ProcessShadowFirst(*node, true);
}

/**
 * @tc.name: UpdateSubTreeInCache003
 * @tc.desc: Test RSUniRenderVisitorTest.UpdateSubTreeInCache, with diffrent children type
 * @tc.type: FUNC
 * @tc.require: issueI7UGLR
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateSubTreeInCache003, TestSize.Level2)
{
    NodeId id = 0;
    auto node = std::make_shared<RSRenderNode>(id);
    node->InitRenderParams();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    // prepare child lists
    auto child1 = std::make_shared<RSSurfaceRenderNode>(++id);
    auto child2 = std::make_shared<RSEffectRenderNode>(++id);
    std::shared_ptr<RSRenderNode> child3 = nullptr;
    std::vector<RSRenderNode::SharedPtr> children;
    children.push_back(child1);
    children.push_back(child2);
    children.push_back(child3);
    rsUniRenderVisitor->UpdateSubTreeInCache(node, *node->GetSortedChildren());
}

/**
 * @tc.name: PrepareEffectNodeIfCacheReuse001
 * @tc.desc: Test PrepareEffectNodeIfCacheReuse, effectNode nullptr
 * @tc.type: FUNC
 * @tc.require: issueI7UGLR
 */
HWTEST_F(RSUniRenderVisitorTest, PrepareEffectNodeIfCacheReuse001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    RSDisplayNodeConfig displayConfig;
    auto rsContext = std::make_shared<RSContext>();
    NodeId id = 1;
    rsUniRenderVisitor->curDisplayNode_ = std::make_shared<RSDisplayRenderNode>(++id,
        displayConfig, rsContext->weak_from_this());
    ASSERT_NE(rsUniRenderVisitor->curDisplayNode_, nullptr);
    // prepare node
    auto node = std::make_shared<RSRenderNode>(++id);
    node->InitRenderParams();
    std::shared_ptr<RSEffectRenderNode> effectNode = nullptr;
    rsUniRenderVisitor->PrepareEffectNodeIfCacheReuse(node, effectNode);
}

/**
 * @tc.name: PrepareEffectNodeIfCacheReuse002
 * @tc.desc: Test RSUniRenderVisitorTest.PrepareEffectNodeIfCacheReuse002, with dirty manager
 * @tc.type: FUNC
 * @tc.require: issueI7UGLR
 */
HWTEST_F(RSUniRenderVisitorTest, PrepareEffectNodeIfCacheReuse002, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    RSDisplayNodeConfig displayConfig;
    auto rsContext = std::make_shared<RSContext>();
    NodeId id = 1;
    rsUniRenderVisitor->curDisplayNode_ = std::make_shared<RSDisplayRenderNode>(++id,
        displayConfig, rsContext->weak_from_this());
    ASSERT_NE(rsUniRenderVisitor->curDisplayNode_, nullptr);
    // prepare node
    auto node = std::make_shared<RSRenderNode>(++id);
    node->InitRenderParams();
    rsUniRenderVisitor->curSurfaceDirtyManager_ = std::make_shared<RSDirtyRegionManager>();
    auto effectNode = std::make_shared<RSEffectRenderNode>(++id);
    rsUniRenderVisitor->PrepareEffectNodeIfCacheReuse(node, effectNode);
}

/**
 * @tc.name: PrepareEffectNodeIfCacheReuse003
 * @tc.desc: Test RSUniRenderVisitorTest.PrepareEffectNodeIfCacheReuse003, needFilter_ equals true
 * @tc.type: FUNC
 * @tc.require: issueI7UGLR
 */
HWTEST_F(RSUniRenderVisitorTest, PrepareEffectNodeIfCacheReuse003, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    RSDisplayNodeConfig displayConfig;
    auto rsContext = std::make_shared<RSContext>();
    NodeId id = 1;
    rsUniRenderVisitor->curDisplayNode_ = std::make_shared<RSDisplayRenderNode>(++id,
        displayConfig, rsContext->weak_from_this());
    ASSERT_NE(rsUniRenderVisitor->curDisplayNode_, nullptr);
    // prepare node
    auto node = std::make_shared<RSRenderNode>(++id);
    node->InitRenderParams();
    rsUniRenderVisitor->curSurfaceDirtyManager_ = std::make_shared<RSDirtyRegionManager>();
    auto effectNode = std::make_shared<RSEffectRenderNode>(++id);
    effectNode->GetMutableRenderProperties().needFilter_ = true;
    rsUniRenderVisitor->PrepareEffectNodeIfCacheReuse(node, effectNode);
}

/**
 * @tc.name: PrepareEffectNodeIfCacheReuse004
 * @tc.desc: Test RSUniRenderVisitorTest.PrepareEffectNodeIfCacheReuse004
 *           curSurfaceNode_->GetId() equals effectNode->GetInstanceRootNodeId()
 * @tc.type: FUNC
 * @tc.require: issueI7UGLR
 */
HWTEST_F(RSUniRenderVisitorTest, PrepareEffectNodeIfCacheReuse004, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    RSDisplayNodeConfig displayConfig;
    auto rsContext = std::make_shared<RSContext>();
    NodeId id = 1;
    rsUniRenderVisitor->curDisplayNode_ = std::make_shared<RSDisplayRenderNode>(++id,
        displayConfig, rsContext->weak_from_this());
    ASSERT_NE(rsUniRenderVisitor->curDisplayNode_, nullptr);
    // prepare node
    auto node = std::make_shared<RSRenderNode>(++id);
    node->InitRenderParams();
    rsUniRenderVisitor->curSurfaceDirtyManager_ = std::make_shared<RSDirtyRegionManager>();
    auto effectNode = std::make_shared<RSEffectRenderNode>(++id);
    effectNode->GetMutableRenderProperties().needFilter_ = true;
    id = 0x0FFF;
    effectNode->instanceRootNodeId_ = id;
    rsUniRenderVisitor->curSurfaceNode_ = std::make_shared<RSSurfaceRenderNode>(id);
    rsUniRenderVisitor->PrepareEffectNodeIfCacheReuse(node, effectNode);
}

/**
 * @tc.name: DisableNodeCacheInSetting001
 * @tc.desc: Test RSUniRenderVisitorTest.DisableNodeCacheInSetting001
 * @tc.type: FUNC
 * @tc.require: issueI7UGLR
 */
HWTEST_F(RSUniRenderVisitorTest, DisableNodeCacheInSetting001, TestSize.Level2)
{
    NodeId id = 0;
    auto node = std::make_shared<RSRenderNode>(id);
    node->InitRenderParams();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->DisableNodeCacheInSetting(*node);
}

/**
 * @tc.name: DisableNodeCacheInSetting002
 * @tc.desc: Test RSUniRenderVisitorTest.DisableNodeCacheInSetting002, TARGETED_CACHE
 * @tc.type: FUNC
 * @tc.require: issueI7UGLR
 */
HWTEST_F(RSUniRenderVisitorTest, DisableNodeCacheInSetting002, TestSize.Level2)
{
    NodeId id = 0;
    auto node = std::make_shared<RSRenderNode>(id);
    node->InitRenderParams();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    node->drawingCacheType_ = RSDrawingCacheType::TARGETED_CACHE;
    rsUniRenderVisitor->DisableNodeCacheInSetting(*node);
}

/**
 * @tc.name: DisableNodeCacheInSetting003
 * @tc.desc: Test RSUniRenderVisitorTest.DisableNodeCacheInSetting003
 *           firstVisitedCache_ = INVALID_NODEID
 * @tc.type: FUNC
 * @tc.require: issueI7UGLR
 */
HWTEST_F(RSUniRenderVisitorTest, DisableNodeCacheInSetting003, TestSize.Level2)
{
    NodeId id = 0;
    auto node = std::make_shared<RSRenderNode>(id);
    node->InitRenderParams();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->firstVisitedCache_ = INVALID_NODEID;
    node->drawingCacheType_ = RSDrawingCacheType::TARGETED_CACHE;
    rsUniRenderVisitor->DisableNodeCacheInSetting(*node);
}

/**
 * @tc.name: IsFirstVisitedCacheForced
 * @tc.desc: Test RSUniRenderVisitorTest.IsFirstVisitedCacheForced
 * @tc.type: FUNC
 * @tc.require: issueI7UGLR
 */
HWTEST_F(RSUniRenderVisitorTest, IsFirstVisitedCacheForced, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->firstVisitedCache_ = INVALID_NODEID;
    ASSERT_FALSE(rsUniRenderVisitor->IsFirstVisitedCacheForced());
    rsUniRenderVisitor->firstVisitedCache_ = 1;
    ASSERT_FALSE(rsUniRenderVisitor->IsFirstVisitedCacheForced());
}

/**
 * @tc.name: SaveCurSurface
 * @tc.desc: Test RSUniRenderVisitorTest.SaveCurSurface
 * @tc.type: FUNC
 * @tc.require: issueI7UGLR
 */
HWTEST_F(RSUniRenderVisitorTest, SaveCurSurface, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->isSubSurfaceEnabled_ = true;
    rsUniRenderVisitor->curSurfaceDirtyManager_ = std::make_shared<RSDirtyRegionManager>();
    rsUniRenderVisitor->curSurfaceNode_ = std::make_shared<RSSurfaceRenderNode>(1);
    rsUniRenderVisitor->SaveCurSurface();
}

/**
 * @tc.name: RestoreCurSurface
 * @tc.desc: Test RSUniRenderVisitorTest.RestoreCurSurface
 * @tc.type: FUNC
 * @tc.require: issueI7UGLR
 */
HWTEST_F(RSUniRenderVisitorTest, RestoreCurSurface, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->isSubSurfaceEnabled_ = true;
    rsUniRenderVisitor->curSurfaceDirtyManager_ = std::make_shared<RSDirtyRegionManager>();
    rsUniRenderVisitor->curSurfaceNode_ = std::make_shared<RSSurfaceRenderNode>(1);
    rsUniRenderVisitor->SaveCurSurface();
    rsUniRenderVisitor->RestoreCurSurface();
}

/**
 * @tc.name: PrepareSubSurfaceNodes
 * @tc.desc: Test RSUniRenderVisitorTest.PrepareSubSurfaceNodes
 * @tc.type: FUNC
 * @tc.require: issueI7UGLR
 */
HWTEST_F(RSUniRenderVisitorTest, PrepareSubSurfaceNodes, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->isSubSurfaceEnabled_ = true;
    NodeId id = 1;
    auto node = std::make_shared<RSSurfaceRenderNode>(id);
    node->InitRenderParams();
    std::vector<std::weak_ptr<RSRenderNode>> ptrs;
    id = 2;
    auto subNode1 = std::make_shared<RSSurfaceRenderNode>(id);
    RSDisplayNodeConfig config;
    id = 3;
    auto subNode2 = std::make_shared<RSDisplayRenderNode>(id, config);
    ptrs.push_back(subNode1);
    ptrs.push_back(subNode2);
    node->subSurfaceNodes_[1] = ptrs;
    rsUniRenderVisitor->PrepareSubSurfaceNodes(*node);
}

/**
 * @tc.name: ProcessSubSurfaceNodes
 * @tc.desc: Test RSUniRenderVisitorTest.ProcessSubSurfaceNodes
 * @tc.type: FUNC
 * @tc.require: issueI7UGLR
 */
HWTEST_F(RSUniRenderVisitorTest, ProcessSubSurfaceNodes, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->isSubSurfaceEnabled_ = true;
    NodeId id = 1;
    auto node = std::make_shared<RSSurfaceRenderNode>(id);
    node->InitRenderParams();
    std::vector<std::weak_ptr<RSRenderNode>> ptrs;
    id = 2;
    auto subNode1 = std::make_shared<RSSurfaceRenderNode>(id);
    RSDisplayNodeConfig config;
    id = 3;
    auto subNode2 = std::make_shared<RSDisplayRenderNode>(id, config);
    ptrs.push_back(subNode1);
    ptrs.push_back(subNode2);
    node->subSurfaceNodes_[node->GetId()] = ptrs;
    rsUniRenderVisitor->ProcessSubSurfaceNodes(*node);
}

/**
 * @tc.name: SetNodeCacheChangeStatus
 * @tc.desc: Test RSUniRenderVisitorTest.SetNodeCacheChangeStatus
 *           node doesn't have Child
 * @tc.type: FUNC
 * @tc.require: issueI7UGLR
 */
HWTEST_F(RSUniRenderVisitorTest, SetNodeCacheChangeStatus, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    NodeId id = 1;
    auto parent = std::make_shared<RSSurfaceRenderNode>(id);
    auto child = std::make_shared<RSSurfaceRenderNode>(++id);
    parent->InitRenderParams();
    child->InitRenderParams();
    parent->AddChild(child);
    child->SetChildHasVisibleFilter(true);
    rsUniRenderVisitor->isDrawingCacheEnabled_ = true;
    child->SetDrawingCacheType(RSDrawingCacheType::TARGETED_CACHE);
    rsUniRenderVisitor->curCacheFilterRects_.push({});
    rsUniRenderVisitor->SetNodeCacheChangeStatus(*child);
}

/**
 * @tc.name: ClearTransparentBeforeSaveLayerTest002
 * @tc.desc: Test RSUniRenderVisitorTest.ClearTransparentBeforeSaveLayerTest002
 * @tc.type: FUNC
 * @tc.require: issueI7UGLR
 */
HWTEST_F(RSUniRenderVisitorTest, ClearTransparentBeforeSaveLayerTest002, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->isHardwareForcedDisabled_ = false;
    NodeId id = 1;
    auto node = std::make_shared<RSSurfaceRenderNode>(id);
    node->InitRenderParams();
    rsUniRenderVisitor->hardwareEnabledNodes_.push_back(node);
    rsUniRenderVisitor->ClearTransparentBeforeSaveLayer();
}

/**
 * @tc.name: CheckIfUIFirstSurfaceContentReusable001
 * @tc.desc: Test CheckIfUIFirstSurfaceContentReusable001, nullptr
 * @tc.type: FUNC
 * @tc.require: issueI7UGLR
 */
HWTEST_F(RSUniRenderVisitorTest, CheckIfUIFirstSurfaceContentReusable001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    bool assigned = false;
    std::shared_ptr<RSSurfaceRenderNode> node = nullptr;
    rsUniRenderVisitor->CheckIfUIFirstSurfaceContentReusable(node, assigned);
}

/**
 * @tc.name: CheckIfUIFirstSurfaceContentReusable002
 * @tc.desc: Test CheckIfUIFirstSurfaceContentReusable002, without UIFirst
 * @tc.type: FUNC
 * @tc.require: issueI7UGLR
 */
HWTEST_F(RSUniRenderVisitorTest, CheckIfUIFirstSurfaceContentReusable002, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->isUIFirst_ = false;
    bool assigned = false;
    NodeId id = 1;
    auto node = std::make_shared<RSSurfaceRenderNode>(id);
    node->InitRenderParams();
    rsUniRenderVisitor->CheckIfUIFirstSurfaceContentReusable(node, assigned);
}

/**
 * @tc.name: PrepareTypesOfSurfaceRenderNodeAfterUpdate001
 * @tc.desc: Test PrepareTypesOfSurfaceRenderNodeAfterUpdate001
 * @tc.type: FUNC
 * @tc.require: issueI7UGLR
 */
HWTEST_F(RSUniRenderVisitorTest, PrepareTypesOfSurfaceRenderNodeAfterUpdate001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    NodeId id = 1;
    auto node = std::make_shared<RSSurfaceRenderNode>(id);
    node->InitRenderParams();
    rsUniRenderVisitor->PrepareTypesOfSurfaceRenderNodeAfterUpdate(*node);
}

/**
 * @tc.name: PrepareTypesOfSurfaceRenderNodeAfterUpdate002
 * @tc.desc: Test PrepareTypesOfSurfaceRenderNodeAfterUpdate002, with non empty parent
 * @tc.type: FUNC
 * @tc.require: issueI7UGLR
 */
HWTEST_F(RSUniRenderVisitorTest, PrepareTypesOfSurfaceRenderNodeAfterUpdate002, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    NodeId id = 1;
    auto parent = std::make_shared<RSSurfaceRenderNode>(id);
    auto child = std::make_shared<RSSurfaceRenderNode>(id+1);
    parent->InitRenderParams();
    child->InitRenderParams();
    parent->AddChild(child);
    child->GetMutableRenderProperties().needFilter_ = true;
    rsUniRenderVisitor->PrepareTypesOfSurfaceRenderNodeAfterUpdate(*child);
}

/**
 * @tc.name: DrawTargetSurfaceVisibleRegionForDFX002
 * @tc.desc: Test DrawTargetSurfaceVisibleRegionForDFX, with non empty curAllSurfaces_
 * @tc.type: FUNC
 * @tc.require: issueI7UGLR
 */
HWTEST_F(RSUniRenderVisitorTest, DrawTargetSurfaceVisibleRegionForDFX002, TestSize.Level2)
{
    RSDisplayNodeConfig config;
    auto rsContext = std::make_shared<RSContext>();
    NodeId id = 1;
    auto node = std::make_shared<RSDisplayRenderNode>(id, config, rsContext->weak_from_this());
    node->InitRenderParams();
    // prepare curAllsurfaces_
    node->curAllSurfaces_.push_back(nullptr);
    auto node2 = RSTestUtil::CreateSurfaceNode();
    node2->InitRenderParams();
    node->curAllSurfaces_.push_back(node2);
    auto node3 = RSTestUtil::CreateSurfaceNode();
    node3->InitRenderParams();
    node3->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    node->curAllSurfaces_.push_back(node3);
    // run
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->DrawTargetSurfaceVisibleRegionForDFX(*node);
}

/**
 * @tc.name: IsNotDirtyHardwareEnabledTopSurface
 * @tc.desc: Test IsNotDirtyHardwareEnabledTopSurface, SELF_DRAWING_WINDOW_NODE
 * @tc.type: FUNC
 * @tc.require: issueI7UGLR
 */
HWTEST_F(RSUniRenderVisitorTest, IsNotDirtyHardwareEnabledTopSurface, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto node = RSTestUtil::CreateSurfaceNode();
    node->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_WINDOW_NODE;
    node->name_ = "pointer window";
    // run
    rsUniRenderVisitor->IsNotDirtyHardwareEnabledTopSurface(node);
}

/**
 * @tc.name: CalcChildFilterNodeDirtyRegion001
 * @tc.desc: Test CalcChildFilterNodeDirtyRegion, with two nullptr
 * @tc.type: FUNC
 * @tc.require: issueI7UGLR
 */
HWTEST_F(RSUniRenderVisitorTest, CalcChildFilterNodeDirtyRegion001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    std::shared_ptr<RSSurfaceRenderNode> node1 = nullptr;
    std::shared_ptr<RSDisplayRenderNode> node2 = nullptr;
    rsUniRenderVisitor->CalcChildFilterNodeDirtyRegion(node1, node2);
}

/**
 * @tc.name: CalcChildFilterNodeDirtyRegion002
 * @tc.desc: Test CalcChildFilterNodeDirtyRegion, with two nullptr
 * @tc.type: FUNC
 * @tc.require: issueI7UGLR
 */
HWTEST_F(RSUniRenderVisitorTest, CalcChildFilterNodeDirtyRegion002, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    RSSurfaceRenderNodeConfig surfaceConfig;
    RSDisplayNodeConfig displayConfig;
    surfaceConfig.id = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    surfaceNode->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    RectI rect{0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT};
    surfaceNode->childrenFilterRects_.push_back(rect);
    NodeId id = 2;
    auto childNode = std::make_shared<RSRenderNode>(id);
    surfaceNode->AddChild(childNode);
    auto displayNode = std::make_shared<RSDisplayRenderNode>(++id, displayConfig);
    rsUniRenderVisitor->CalcChildFilterNodeDirtyRegion(surfaceNode, displayNode);
}

/**
 * @tc.name: CalcSurfaceFilterNodeDirtyRegion001
 * @tc.desc: Test CalcChildFilterNodeDirtyRegion001, with nullptr
 * @tc.type: FUNC
 * @tc.require: issueI7UGLR
 */
HWTEST_F(RSUniRenderVisitorTest, CalcSurfaceFilterNodeDirtyRegion001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    std::shared_ptr<RSSurfaceRenderNode> node1 = nullptr;
    std::shared_ptr<RSDisplayRenderNode> node2 = nullptr;
    rsUniRenderVisitor->CalcSurfaceFilterNodeDirtyRegion(node1, node2);
}

/**
 * @tc.name: CalcSurfaceFilterNodeDirtyRegion002
 * @tc.desc: Test CalcChildFilterNodeDirtyRegion002
 * @tc.type: FUNC
 * @tc.require: issueI7UGLR
 */
HWTEST_F(RSUniRenderVisitorTest, CalcSurfaceFilterNodeDirtyRegion002, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    RSSurfaceRenderNodeConfig surfaceConfig;
    RSDisplayNodeConfig displayConfig;
    surfaceConfig.id = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    surfaceNode->GetMutableRenderProperties().needFilter_ = true;
    NodeId id = 2;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, displayConfig);
    rsUniRenderVisitor->CalcSurfaceFilterNodeDirtyRegion(surfaceNode, displayNode);
}

/**
 * @tc.name: SetHasSharedTransitionNode001
 * @tc.desc: Test SetHasSharedTransitionNode for leash window node
 * @tc.type: FUNC
 * @tc.require: issueI98VTC
 */
HWTEST_F(RSUniRenderVisitorTest, SetHasSharedTransitionNode001, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->SetHasSharedTransitionNode(*surfaceNode, true);
    ASSERT_FALSE(surfaceNode->GetHasSharedTransitionNode());
}

/**
 * @tc.name: SetHasSharedTransitionNode002
 * @tc.desc: Test SetHasSharedTransitionNode for app window node
 * @tc.type: FUNC
 * @tc.require: issueI98VTC
 */
HWTEST_F(RSUniRenderVisitorTest, SetHasSharedTransitionNode002, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->SetHasSharedTransitionNode(*surfaceNode, true);
    ASSERT_TRUE(surfaceNode->GetHasSharedTransitionNode());
}

/**
 * @tc.name: SetHasSharedTransitionNode003
 * @tc.desc: Test SetHasSharedTransitionNode while node's parent isn't leash window node
 * @tc.type: FUNC
 * @tc.require: issueI98VTC
 */
HWTEST_F(RSUniRenderVisitorTest, SetHasSharedTransitionNode003, TestSize.Level2)
{
    auto parentNode = RSTestUtil::CreateSurfaceNode();
    auto node = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(parentNode, nullptr);
    ASSERT_NE(node, nullptr);
    parentNode->AddChild(node);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->SetHasSharedTransitionNode(*node, true);
    ASSERT_FALSE(parentNode->GetHasSharedTransitionNode());
}

/**
 * @tc.name: SetHasSharedTransitionNode004
 * @tc.desc: Test SetHasSharedTransitionNode while node's parent is leash window node
 * @tc.type: FUNC
 * @tc.require: issueI98VTC
 */
HWTEST_F(RSUniRenderVisitorTest, SetHasSharedTransitionNode004, TestSize.Level2)
{
    auto parentNode = RSTestUtil::CreateSurfaceNode();
    auto node = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(parentNode, nullptr);
    ASSERT_NE(node, nullptr);
    parentNode->AddChild(node);
    parentNode->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->SetHasSharedTransitionNode(*node, true);
    ASSERT_TRUE(parentNode->GetHasSharedTransitionNode());
}

/**
 * @tc.name: DrawCurtainScreen001
 * @tc.desc: Test DrawCurtainScreen while curtain screen is on
 * @tc.type: FUNC
 * @tc.require: issueI9ABGS
 */
HWTEST_F(RSUniRenderVisitorTest, DrawCurtainScreen001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto drawingCanvas = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(drawingCanvas, nullptr);

    rsUniRenderVisitor->canvas_ = std::make_unique<RSPaintFilterCanvas>(drawingCanvas.get());
    rsUniRenderVisitor->isCurtainScreenOn_ = true;
    rsUniRenderVisitor->DrawCurtainScreen();
}

/**
 * @tc.name: DrawCurtainScreen002
 * @tc.desc: Test DrawCurtainScreen while curtain screen is off
 * @tc.type: FUNC
 * @tc.require: issueI9ABGS
 */
HWTEST_F(RSUniRenderVisitorTest, DrawCurtainScreen002, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto drawingCanvas = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(drawingCanvas, nullptr);

    rsUniRenderVisitor->canvas_ = std::make_unique<RSPaintFilterCanvas>(drawingCanvas.get());
    rsUniRenderVisitor->isCurtainScreenOn_ = false;
    rsUniRenderVisitor->DrawCurtainScreen();
}

/**
 * @tc.name: ClipRegion001
 * @tc.desc: Test ClipRegion, with empty region
 * @tc.type: FUNC
 * @tc.require: issueI9ABGS
 */
HWTEST_F(RSUniRenderVisitorTest, ClipRegion001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto drawingCanvas = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(drawingCanvas, nullptr);
    auto paintFilterCanvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    Drawing::Region region;
    rsUniRenderVisitor->ClipRegion(paintFilterCanvas, region);
}

/**
 * @tc.name: ClipRegion002
 * @tc.desc: Test ClipRegion, with rect region
 * @tc.type: FUNC
 * @tc.require: issueI9ABGS
 */
HWTEST_F(RSUniRenderVisitorTest, ClipRegion002, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto drawingCanvas = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(drawingCanvas, nullptr);
    auto paintFilterCanvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());

    Drawing::Region rectRegion;
    constexpr int rectWidth = 100;
    constexpr int rectHeight = 100;
    rectRegion.SetRect(Drawing::RectI(0, 0, rectWidth, rectHeight));
    Drawing::Region region;
    region.Op(rectRegion, Drawing::RegionOp::UNION);

    rsUniRenderVisitor->ClipRegion(paintFilterCanvas, region);
}

/**
 * @tc.name: ClipRegion003
 * @tc.desc: Test ClipRegion, with non-rect region
 * @tc.type: FUNC
 * @tc.require: issueI9ABGS
 */
HWTEST_F(RSUniRenderVisitorTest, ClipRegion003, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto drawingCanvas = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(drawingCanvas, nullptr);
    auto paintFilterCanvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());

    Drawing::Region region;
    Drawing::Region firstRectRegion;
    constexpr int firstRectWidth = 100;
    constexpr int firstRectHeight = 100;
    firstRectRegion.SetRect(Drawing::RectI(0, 0, firstRectWidth, firstRectHeight));
    region.Op(firstRectRegion, Drawing::RegionOp::UNION);
    Drawing::Region secondRectRegion;
    constexpr int secondRectWidth = 200;
    constexpr int secondRectHeight = 50;
    secondRectRegion.SetRect(Drawing::RectI(0, 0, secondRectWidth, secondRectHeight));
    region.Op(secondRectRegion, Drawing::RegionOp::UNION);

    rsUniRenderVisitor->ClipRegion(paintFilterCanvas, region);
}

/*
 * @tc.name: CheckMergeFilterDirtyByIntersectWithDirty001
 * @tc.desc: Test CheckMergeFilterDirtyByIntersectWithDirty
 * @tc.type: FUNC
 * @tc.require: issueIAO5GW
*/
HWTEST_F(RSUniRenderVisitorTest, CheckMergeFilterDirtyByIntersectWithDirty001, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    RSDisplayNodeConfig displayConfig;
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(DEFAULT_NODE_ID,
        displayConfig, rsContext->weak_from_this());
    ASSERT_NE(rsDisplayRenderNode, nullptr);
    ASSERT_NE(rsDisplayRenderNode->GetDirtyManager(), nullptr);
    rsDisplayRenderNode->InitRenderParams();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->InitDisplayInfo(*rsDisplayRenderNode);

    OcclusionRectISet filterSet;
    NodeId filterNodeId = DEFAULT_NODE_ID;
    // 1.filterSet not empty, currentFrameDirty empty
    filterSet.insert({filterNodeId, DEFAULT_RECT});
    rsUniRenderVisitor->CheckMergeFilterDirtyByIntersectWithDirty(filterSet, true);
    ASSERT_EQ(rsDisplayRenderNode->GetDirtyManager()->GetCurrentFrameDirtyRegion().IsEmpty(), true);

    // 2.filterSet not empty and intersect with currentFrameDirty dirty not changed after merge
    filterSet.insert({filterNodeId, DEFAULT_RECT});
    rsDisplayRenderNode->GetDirtyManager()->MergeDirtyRect(DEFAULT_RECT);
    rsUniRenderVisitor->CheckMergeFilterDirtyByIntersectWithDirty(filterSet, true);
    ASSERT_EQ(rsDisplayRenderNode->GetDirtyManager()->GetCurrentFrameDirtyRegion().IsEmpty(), false);

    // 3.filterSet not empty and intersect with currentFrameDirty, dirty changed after merge
    filterSet.insert({filterNodeId, DEFAULT_FILTER_RECT});
    rsUniRenderVisitor->CheckMergeFilterDirtyByIntersectWithDirty(filterSet, true);
    bool isRectEqual = (rsDisplayRenderNode->GetDirtyManager()->GetCurrentFrameDirtyRegion() == DEFAULT_RECT);
    ASSERT_EQ(isRectEqual, false);
}

/*
 * @tc.name: CheckMergeSurfaceDirtysForDisplay001
 * @tc.desc: Test CheckMergeSurfaceDirtysForDisplay with transparent node
 * @tc.type: FUNC
 * @tc.require: issueI9RR2Y
*/
HWTEST_F(RSUniRenderVisitorTest, CheckMergeSurfaceDirtysForDisplay001, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    RSDisplayNodeConfig displayConfig;
    config.id = 10; // 10 non-zero dummy configId
    auto rsCanvasRenderNode = std::make_shared<RSCanvasRenderNode>(1, rsContext->weak_from_this());
    ASSERT_NE(rsCanvasRenderNode, nullptr);
    rsCanvasRenderNode->InitRenderParams();
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    rsSurfaceRenderNode->InitRenderParams();
    // 11 non-zero node id
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(11, displayConfig, rsContext->weak_from_this());
    ASSERT_NE(rsDisplayRenderNode, nullptr);
    rsDisplayRenderNode->InitRenderParams();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();

    // set surface to transparent, add a canvas node to create a transparent dirty region
    rsSurfaceRenderNode->SetAbilityBGAlpha(0);
    // 80, 2560, 1600. dummy value  used to create rectangle with non-zero dimension
    rsSurfaceRenderNode->SetSrcRect(RectI(0, 80, 2560, 1600));
    rsDisplayRenderNode->AddChild(rsSurfaceRenderNode, -1);
    rsSurfaceRenderNode->AddChild(rsCanvasRenderNode, -1);

    rsUniRenderVisitor->PrepareDisplayRenderNode(*rsDisplayRenderNode);

    rsUniRenderVisitor->CheckMergeSurfaceDirtysForDisplay(rsSurfaceRenderNode);
    ASSERT_EQ(rsUniRenderVisitor->curDisplayDirtyManager_->dirtyRegion_.left_, 0);
}


/*
 * @tc.name: UpdateDisplayDirtyAndExtendVisibleRegion
 * @tc.desc: Test UpdateDisplayDirtyAndExtendVisibleRegion
 * @tc.type: FUNC
 * @tc.require: issueIAN75I
*/
HWTEST_F(RSUniRenderVisitorTest, UpdateDisplayDirtyAndExtendVisibleRegion, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->UpdateDisplayDirtyAndExtendVisibleRegion();

    RSDisplayNodeConfig displayConfig;
    auto rsContext = std::make_shared<RSContext>();
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(5, displayConfig, rsContext->weak_from_this());
    ASSERT_NE(rsDisplayRenderNode, nullptr);
    ASSERT_NE(rsDisplayRenderNode->GetDirtyManager(), nullptr);
    rsDisplayRenderNode->InitRenderParams();
    rsUniRenderVisitor->QuickPrepareDisplayRenderNode(*rsDisplayRenderNode);
    RSSurfaceRenderNodeConfig surfaceConfig;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    rsDisplayRenderNode->RecordMainAndLeashSurfaces(surfaceNode);

    auto canvasNode = std::make_shared<RSCanvasRenderNode>(1, rsContext->weak_from_this());
    ASSERT_NE(canvasNode, nullptr);
    auto& property = canvasNode->GetMutableRenderProperties();
    property.SetLightUpEffect(0.2f);
    property.UpdateFilter();
    surfaceNode->UpdateVisibleFilterChild(*canvasNode);
    auto visibleFilterChildren = surfaceNode->GetVisibleFilterChild();
    ASSERT_NE(visibleFilterChildren.size(), 0);
    rsUniRenderVisitor->UpdateDisplayDirtyAndExtendVisibleRegion();

    auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();
    nodeMap.RegisterRenderNode(canvasNode);
    auto& filterNode = nodeMap.GetRenderNode<RSRenderNode>(canvasNode->GetId());
    ASSERT_NE(filterNode, nullptr);
    rsUniRenderVisitor->UpdateDisplayDirtyAndExtendVisibleRegion();

    Occlusion::Region region{ Occlusion::Rect{ 0, 0, 100, 100 } };
    surfaceNode->SetVisibleRegion(region);
    canvasNode->SetOldDirtyInSurface({ 50, 50, 70, 70 });
    rsUniRenderVisitor->UpdateDisplayDirtyAndExtendVisibleRegion();
}

/*
 * @tc.name: CheckMergeDisplayDirtyByTransparent001
 * @tc.desc: Test CheckMergeDisplayDirtyByTransparent with transparent node
 * @tc.type: FUNC
 * @tc.require: issueIAD6OW
*/
HWTEST_F(RSUniRenderVisitorTest, CheckMergeDisplayDirtyByTransparent001, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    RSDisplayNodeConfig displayConfig;
    config.id = 10; // 10 non-zero dummy configId
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    ASSERT_NE(rsSurfaceRenderNode->GetDirtyManager(), nullptr);
    rsSurfaceRenderNode->InitRenderParams();
    // 11 non-zero node id
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(11, displayConfig, rsContext->weak_from_this());
    ASSERT_NE(rsDisplayRenderNode, nullptr);
    ASSERT_NE(rsDisplayRenderNode->GetDirtyManager(), nullptr);
    rsDisplayRenderNode->InitRenderParams();
    rsDisplayRenderNode->AddChild(rsSurfaceRenderNode, -1);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->curDisplayNode_ = rsDisplayRenderNode;
    // set surfaceNode to transparent, create a transparent dirty region
    rsSurfaceRenderNode->SetAbilityBGAlpha(0);
    rsSurfaceRenderNode->SetSrcRect(DEFAULT_RECT);
    rsSurfaceRenderNode->GetDirtyManager()->MergeDirtyRect(DEFAULT_RECT);
    Occlusion::Region visibleRegion{DEFAULT_RECT};
    rsSurfaceRenderNode->SetVisibleRegion(visibleRegion);
    auto dirtyRect = rsSurfaceRenderNode->GetDirtyManager()->GetCurrentFrameDirtyRegion();
    ASSERT_TRUE(rsSurfaceRenderNode->GetVisibleRegion().IsIntersectWith(dirtyRect));
    rsSurfaceRenderNode->oldDirtyInSurface_ = DEFAULT_RECT;

    rsUniRenderVisitor->CheckMergeDisplayDirtyByTransparent(*rsSurfaceRenderNode);
    ASSERT_EQ(rsDisplayRenderNode->GetDirtyManager()->GetCurrentFrameDirtyRegion().IsEmpty(), false);
}

/*
 * @tc.name: CheckMergeDisplayDirtyByZorderChanged001
 * @tc.desc: Test CheckMergeDisplayDirtyByZorderChanged with Z order change node
 * @tc.type: FUNC
 * @tc.require: issueIAD6OW
*/
HWTEST_F(RSUniRenderVisitorTest, CheckMergeDisplayDirtyByZorderChanged001, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    RSDisplayNodeConfig displayConfig;
    config.id = 10; // 10 non-zero dummy configId
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    ASSERT_NE(rsSurfaceRenderNode->GetDirtyManager(), nullptr);
    rsSurfaceRenderNode->InitRenderParams();
    // 11 non-zero node id
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(11, displayConfig, rsContext->weak_from_this());
    ASSERT_NE(rsDisplayRenderNode, nullptr);
    ASSERT_NE(rsDisplayRenderNode->GetDirtyManager(), nullptr);
    rsDisplayRenderNode->InitRenderParams();
    rsDisplayRenderNode->AddChild(rsSurfaceRenderNode, -1);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->curDisplayNode_ = rsDisplayRenderNode;
    // set surfaceNode Zorder change
    rsSurfaceRenderNode->zOrderChanged_ = true;
    rsSurfaceRenderNode->oldDirtyInSurface_ = DEFAULT_RECT;

    rsUniRenderVisitor->CheckMergeDisplayDirtyByZorderChanged(*rsSurfaceRenderNode);
    ASSERT_EQ(rsDisplayRenderNode->GetDirtyManager()->GetCurrentFrameDirtyRegion().IsEmpty(), false);
}

/*
 * @tc.name: CheckMergeDisplayDirtyByPosChanged001
 * @tc.desc: Test CheckMergeDisplayDirtyByPosChanged with position change
 * @tc.type: FUNC
 * @tc.require: issueIAD6OW
*/
HWTEST_F(RSUniRenderVisitorTest, CheckMergeDisplayDirtyByPosChanged001, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    RSDisplayNodeConfig displayConfig;
    config.id = 10; // 10 non-zero dummy configId
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    ASSERT_NE(rsSurfaceRenderNode->GetDirtyManager(), nullptr);
    rsSurfaceRenderNode->InitRenderParams();
    // 11 non-zero node id
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(11, displayConfig, rsContext->weak_from_this());
    ASSERT_NE(rsDisplayRenderNode, nullptr);
    ASSERT_NE(rsDisplayRenderNode->GetDirtyManager(), nullptr);
    rsDisplayRenderNode->InitRenderParams();
    rsDisplayRenderNode->AddChild(rsSurfaceRenderNode, -1);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->curDisplayNode_ = rsDisplayRenderNode;
    // set surfaceNode pos change
    rsDisplayRenderNode->UpdateSurfaceNodePos(config.id, RectI());
    rsDisplayRenderNode->ClearCurrentSurfacePos();
    rsDisplayRenderNode->UpdateSurfaceNodePos(config.id, DEFAULT_RECT);

    rsUniRenderVisitor->CheckMergeDisplayDirtyByPosChanged(*rsSurfaceRenderNode);
    ASSERT_EQ(rsDisplayRenderNode->GetDirtyManager()->GetCurrentFrameDirtyRegion().IsEmpty(), false);
}

/*
 * @tc.name: CheckMergeDisplayDirtyByShadowChanged001
 * @tc.desc: Test CheckMergeDisplayDirtyByShadowChanged with shadow change
 * @tc.type: FUNC
 * @tc.require: issueIAD6OW
*/
HWTEST_F(RSUniRenderVisitorTest, CheckMergeDisplayDirtyByShadowChanged001, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    RSDisplayNodeConfig displayConfig;
    config.id = 10; // 10 non-zero dummy configId
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    ASSERT_NE(rsSurfaceRenderNode->GetDirtyManager(), nullptr);
    rsSurfaceRenderNode->InitRenderParams();
    // 11 non-zero node id
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(11, displayConfig, rsContext->weak_from_this());
    ASSERT_NE(rsDisplayRenderNode, nullptr);
    ASSERT_NE(rsDisplayRenderNode->GetDirtyManager(), nullptr);
    rsDisplayRenderNode->InitRenderParams();
    rsDisplayRenderNode->AddChild(rsSurfaceRenderNode, -1);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->curDisplayNode_ = rsDisplayRenderNode;
    // set surfaceNode shadow change
    rsSurfaceRenderNode->isShadowValidLastFrame_ = true;
    rsSurfaceRenderNode->GetDirtyManager()->MergeDirtyRect(DEFAULT_RECT);
    rsSurfaceRenderNode->oldDirtyInSurface_ = DEFAULT_RECT;

    rsUniRenderVisitor->CheckMergeDisplayDirtyByShadowChanged(*rsSurfaceRenderNode);
    ASSERT_EQ(rsDisplayRenderNode->GetDirtyManager()->GetCurrentFrameDirtyRegion().IsEmpty(), false);
}

/*
 * @tc.name: UpdateSurfaceDirtyAndGlobalDirty001
 * @tc.desc: Test UpdateSurfaceDirtyAndGlobalDirty
 * @tc.type: FUNC
 * @tc.require: issueI9RR2Y
*/
HWTEST_F(RSUniRenderVisitorTest, UpdateSurfaceDirtyAndGlobalDirty001, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    RSDisplayNodeConfig displayConfig;
    config.id = 10;
    auto rsCanvasRenderNode = std::make_shared<RSCanvasRenderNode>(1, rsContext->weak_from_this());
    ASSERT_NE(rsCanvasRenderNode, nullptr);
    rsCanvasRenderNode->InitRenderParams();
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    rsSurfaceRenderNode->InitRenderParams();
    // 11 non-zero node id
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(11, displayConfig, rsContext->weak_from_this());
    ASSERT_NE(rsDisplayRenderNode, nullptr);
    rsDisplayRenderNode->InitRenderParams();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();

    // set surface to transparent, add a canvas node to create a transparent dirty region
    rsSurfaceRenderNode->SetAbilityBGAlpha(0);
    // 80, 2560, 1600. dummy value used to create rectangle with non-zero dimension
    rsSurfaceRenderNode->SetSrcRect(RectI(0, 80, 2560, 1600));
    rsDisplayRenderNode->AddChild(rsSurfaceRenderNode, -1);
    rsSurfaceRenderNode->AddChild(rsCanvasRenderNode, -1);

    rsUniRenderVisitor->PrepareDisplayRenderNode(*rsDisplayRenderNode);

    rsUniRenderVisitor->UpdateSurfaceDirtyAndGlobalDirty();
    ASSERT_EQ(rsUniRenderVisitor->curDisplayDirtyManager_->dirtyRegion_.left_, 0);
}

/**
 * @tc.name: UpdateHwcNodeEnableByRotateAndAlpha001
 * @tc.desc: Test UpdateHwcNodeEnableByRotateAndAlpha for empty node
 * @tc.type: FUNC
 * @tc.require: issueI9RR2Y
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnableByRotateAndAlpha001, TestSize.Level2)
{
    auto node = RSTestUtil::CreateSurfaceNode();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();

    rsUniRenderVisitor->UpdateHwcNodeEnableByRotateAndAlpha(node);
    ASSERT_FALSE(node->isHardwareForcedDisabled_);
}

/**
 * @tc.name: UpdateHwcNodeEnableByHwcNodeBelowSelfInApp001
 * @tc.desc: Test UpdateHwcNodeEnableByHwcNodeBelowSelfInApp with empty Rect
 * @tc.type: FUNC
 * @tc.require: issueI9RR2Y
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnableByHwcNodeBelowSelfInApp001, TestSize.Level2)
{
    auto node = RSTestUtil::CreateSurfaceNode();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    std::vector<RectI> hwcRects;
    hwcRects.emplace_back(0, 0, 0, 0);
    rsUniRenderVisitor->UpdateHwcNodeEnableByHwcNodeBelowSelfInApp(hwcRects, node);
    ASSERT_FALSE(node->isHardwareForcedDisabled_);
}

/**
 * @tc.name: UpdateHwcNodeEnableBySrcRect001
 * @tc.desc: Test UpdateHwcNodeEnableBySrcRect with empty node
 * @tc.type: FUNC
 * @tc.require: issueI9RR2Y
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnableBySrcRect001, TestSize.Level2)
{
    auto node = RSTestUtil::CreateSurfaceNode();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();

    rsUniRenderVisitor->UpdateHwcNodeEnableBySrcRect(*node);
    ASSERT_FALSE(node->isHardwareForcedDisabledBySrcRect_);
}

/**
 * @tc.name: UpdateDstRect001
 * @tc.desc: Test UpdateDstRect with empty rect
 * @tc.type: FUNC
 * @tc.require: issueI9RR2Y
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateDstRect001, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    RSDisplayNodeConfig displayConfig;
    config.id = 10;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    rsSurfaceRenderNode->InitRenderParams();
    // 11 non-zero node id
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(11, displayConfig, rsContext->weak_from_this());
    rsDisplayRenderNode->InitRenderParams();
    ASSERT_NE(rsDisplayRenderNode, nullptr);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->InitDisplayInfo(*rsDisplayRenderNode);

    RectI absRect(0, 0, 0, 0);
    RectI clipRect(0, 0, 0, 0);
    rsUniRenderVisitor->UpdateDstRect(*rsSurfaceRenderNode, absRect, clipRect);
    ASSERT_EQ(rsSurfaceRenderNode->GetDstRect().left_, 0);
}

/**
 * @tc.name: UpdateSrcRect001
 * @tc.desc: Test UpdateSrcRect with empty matrix
 * @tc.type: FUNC
 * @tc.require: issueI9RR2Y
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateSrcRect001, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    RSDisplayNodeConfig displayConfig;
    config.id = 10;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    rsSurfaceRenderNode->InitRenderParams();
    // 11 non-zero node id
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(11, displayConfig, rsContext->weak_from_this());
    ASSERT_NE(rsDisplayRenderNode, nullptr);
    rsDisplayRenderNode->InitRenderParams();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->InitDisplayInfo(*rsDisplayRenderNode);

    Drawing::Matrix absMatrix;
    RectI absRect(0, 0, 0, 0);
    rsSurfaceRenderNode->GetMutableRenderProperties().SetBounds({0, 0, 0, 0});
    rsUniRenderVisitor->UpdateSrcRect(*rsSurfaceRenderNode, absMatrix, absRect);
    ASSERT_EQ(rsSurfaceRenderNode->GetSrcRect().left_, 0);
}

/**
 * @tc.name: BeforeUpdateSurfaceDirtyCalc001
 * @tc.desc: Test BeforeUpdateSurfaceDirtyCalc with empty node
 * @tc.type: FUNC
 * @tc.require: issueIABP1V
 */
HWTEST_F(RSUniRenderVisitorTest, BeforeUpdateSurfaceDirtyCalc001, TestSize.Level2)
{
    auto node = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(node, nullptr);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    ASSERT_TRUE(rsUniRenderVisitor->BeforeUpdateSurfaceDirtyCalc(*node));
}

/**
 * @tc.name: BeforeUpdateSurfaceDirtyCalc002
 * @tc.desc: Test BeforeUpdateSurfaceDirtyCalc with nonEmpty node
 * @tc.type: FUNC
 * @tc.require: issueIABP1V
 */
HWTEST_F(RSUniRenderVisitorTest, BeforeUpdateSurfaceDirtyCalc002, TestSize.Level2)
{
    auto node = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(node, nullptr);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    node->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);
    ASSERT_TRUE(rsUniRenderVisitor->BeforeUpdateSurfaceDirtyCalc(*node));
    node->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    ASSERT_TRUE(rsUniRenderVisitor->BeforeUpdateSurfaceDirtyCalc(*node));
    node->SetNodeName("CapsuleWindow");
    ASSERT_TRUE(rsUniRenderVisitor->BeforeUpdateSurfaceDirtyCalc(*node));
}

/**
 * @tc.name: SurfaceOcclusionCallbackToWMS001
 * @tc.desc: Test SurfaceOcclusionCallbackToWMS with default constructed visitor
 * @tc.type: FUNC
 * @tc.require: issueIABP1V
 */
HWTEST_F(RSUniRenderVisitorTest, SurfaceOcclusionCallbackToWMS001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->SurfaceOcclusionCallbackToWMS();
}

/**
 * @tc.name: GetCurrentBlackList001
 * @tc.desc: Test GetCurrentBlackList with default constructed visitor
 * @tc.type: FUNC
 * @tc.require: issuesIAMODH
 */
HWTEST_F(RSUniRenderVisitorTest, GetCurrentBlackList001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    EXPECT_TRUE(rsUniRenderVisitor->GetCurrentBlackList().empty());
}

/**
 * @tc.name: GetCurrentBlackList
 * @tc.desc: Test GetCurrentBlackList002, screenManager_ && curDisplayNode_ != nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAMODH
 */
HWTEST_F(RSUniRenderVisitorTest, GetCurrentBlackList002, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->screenManager_ = CreateOrGetScreenManager();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    NodeId displayNodeId = 3;
    RSDisplayNodeConfig config;
    rsUniRenderVisitor->curDisplayNode_ = std::make_shared<RSDisplayRenderNode>(displayNodeId, config);
    rsUniRenderVisitor->curDisplayNode_->InitRenderParams();
    ASSERT_EQ(rsUniRenderVisitor->GetCurrentBlackList().size(), 0);
}

/**
 * @tc.name: NeedPrepareChindrenInReverseOrder001
 * @tc.desc: Test NeedPrepareChindrenInReverseOrder with default constructed visitor
 * @tc.type: FUNC
 * @tc.require: issueIABP1V
 */
HWTEST_F(RSUniRenderVisitorTest, NeedPrepareChindrenInReverseOrder001, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    auto rsBaseRenderNode = std::make_shared<RSBaseRenderNode>(10, rsContext->weak_from_this());
    ASSERT_NE(rsBaseRenderNode, nullptr);
    rsBaseRenderNode->InitRenderParams();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    ASSERT_FALSE(rsUniRenderVisitor->NeedPrepareChindrenInReverseOrder(*rsBaseRenderNode));
}

/**
 * @tc.name: NeedPrepareChindrenInReverseOrder002
 * @tc.desc: Test NeedPrepareChindrenInReverseOrder with different type nodes
 * @tc.type: FUNC
 * @tc.require: issueIABP1V
 */
HWTEST_F(RSUniRenderVisitorTest, NeedPrepareChindrenInReverseOrder002, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    auto rsBaseRenderNode = std::make_shared<RSBaseRenderNode>(10, rsContext->weak_from_this());
    ASSERT_NE(rsBaseRenderNode, nullptr);
    rsBaseRenderNode->InitRenderParams();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    auto node1 = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(node1, nullptr);
    node1->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);

    node1->AddChild(rsBaseRenderNode);
    ASSERT_TRUE(rsUniRenderVisitor->NeedPrepareChindrenInReverseOrder(*node1));

    auto node2 = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(node2, nullptr);
    node2->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);

    node1->RemoveChild(rsBaseRenderNode);
    node1->AddChild(node2);
    ASSERT_TRUE(rsUniRenderVisitor->NeedPrepareChindrenInReverseOrder(*node1));

    node1->AddChild(rsBaseRenderNode);
    ASSERT_TRUE(rsUniRenderVisitor->NeedPrepareChindrenInReverseOrder(*node1));
}

/**
 * @tc.name: IsLeashAndHasMainSubNode001
 * @tc.desc: Test IsLeashAndHasMainSubNode with empty node
 * @tc.type: FUNC
 * @tc.require: issueI9RR2Y
 */
HWTEST_F(RSUniRenderVisitorTest, IsLeashAndHasMainSubNode001, TestSize.Level2)
{
    auto node = RSTestUtil::CreateSurfaceNode();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();

    ASSERT_FALSE(rsUniRenderVisitor->IsLeashAndHasMainSubNode(*node));
}

/**
 * @tc.name: CalculateOcclusion001
 * @tc.desc: Test CalculateOcclusion with empty node
 * @tc.type: FUNC
 * @tc.require: issueI9RR2Y
 */
HWTEST_F(RSUniRenderVisitorTest, CalculateOcclusion001, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    RSDisplayNodeConfig displayConfig;
    config.id = 10;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    rsSurfaceRenderNode->InitRenderParams();
    // 11 non-zero node id
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(11, displayConfig, rsContext->weak_from_this());
    rsDisplayRenderNode->InitRenderParams();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->InitDisplayInfo(*rsDisplayRenderNode);

    rsUniRenderVisitor->CalculateOcclusion(*rsSurfaceRenderNode);
    ASSERT_FALSE(rsUniRenderVisitor->needRecalculateOcclusion_);
}

/**
 * @tc.name: QuickPrepareDisplayRenderNode001
 * @tc.desc: Test QuickPrepareDisplayRenderNode with display node
 * @tc.type: FUNC
 * @tc.require: issueI9RR2Y
 */
HWTEST_F(RSUniRenderVisitorTest, QuickPrepareDisplayRenderNode001, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    RSDisplayNodeConfig displayConfig;
    // 11 non-zero node id
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(11, displayConfig, rsContext->weak_from_this());
    rsDisplayRenderNode->InitRenderParams();

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();

    rsUniRenderVisitor->QuickPrepareDisplayRenderNode(*rsDisplayRenderNode);
    ASSERT_FALSE(rsUniRenderVisitor->ancestorNodeHasAnimation_);
}

/**
 * @tc.name: IsSubTreeOccluded001
 * @tc.desc: Test IsSubTreeOccluded with empty node
 * @tc.type: FUNC
 * @tc.require: issueI9RR2Y
 */
HWTEST_F(RSUniRenderVisitorTest, IsSubTreeOccluded001, TestSize.Level2)
{
    auto node = RSTestUtil::CreateSurfaceNode();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();

    rsUniRenderVisitor->IsSubTreeOccluded(*node);
    ASSERT_EQ(node->dirtyStatus_, RSRenderNode::NodeDirty::CLEAN);
}

/**
 * @tc.name: UpdateSurfaceRenderNodeRotate001
 * @tc.desc: Test UpdateSurfaceRenderNodeRotate with empty node
 * @tc.type: FUNC
 * @tc.require: issueI9RR2Y
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateSurfaceRenderNodeRotate001, TestSize.Level2)
{
    auto node = RSTestUtil::CreateSurfaceNode();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();

    rsUniRenderVisitor->UpdateSurfaceRenderNodeRotate(*node);
    ASSERT_FALSE(node->isRotating_);
}

/*
 * @tc.name: IsOutOfScreenRegion001
 * @tc.desc: Test RSUniRenderVisitorTest.IsOutOfScreenRegion while UniRenderVisitor has no canvas
 * @tc.type: FUNC
 * @tc.require: issuesI9SIK7
 */
HWTEST_F(RSUniRenderVisitorTest, IsOutOfScreenRegion001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    RectI rect(0, 0, 0, 0);

    ASSERT_FALSE(rsUniRenderVisitor->IsOutOfScreenRegion(rect));
}

/*
 * @tc.name: IsOutOfScreenRegion002
 * @tc.desc: Test RSUniRenderVisitorTest.IsOutOfScreenRegion while canvas within screen
 * @tc.type: FUNC
 * @tc.require: issuesI9SIK7
 */
HWTEST_F(RSUniRenderVisitorTest, IsOutOfScreenRegion002, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto drawingCanvas = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(drawingCanvas, nullptr);
    rsUniRenderVisitor->canvas_ = std::make_unique<RSPaintFilterCanvas>(drawingCanvas.get());

    RectI rect(0, 0, RECT_DEFAULT_SIZE, RECT_DEFAULT_SIZE);
    ASSERT_FALSE(rsUniRenderVisitor->IsOutOfScreenRegion(rect));
}

/*
 * @tc.name: PrepareIsOutOfScreenRegion003
 * @tc.desc: Test RSUniRenderVisitorTest.IsOutOfScreenRegion while cavas's position out of screen
 * @tc.type: FUNC
 * @tc.require: issuesI9SIK7
 */
HWTEST_F(RSUniRenderVisitorTest, IsOutOfScreenRegion003, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto drawingCanvas = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(drawingCanvas, nullptr);
    rsUniRenderVisitor->canvas_ = std::make_unique<RSPaintFilterCanvas>(drawingCanvas.get());

    RectI rect(1000, 1000, RECT_DEFAULT_SIZE, RECT_DEFAULT_SIZE);
    ASSERT_TRUE(rsUniRenderVisitor->IsOutOfScreenRegion(rect));
}

/*
 * @tc.name: PrepareIsOutOfScreenRegion004
 * @tc.desc: Test RSUniRenderVisitorTest.IsOutOfScreenRegion while cavas width out screen
 * @tc.type: FUNC
 * @tc.require: issuesI9SIK7
 */
HWTEST_F(RSUniRenderVisitorTest, IsOutOfScreenRegion004, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto drawingCanvas = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(drawingCanvas, nullptr);
    rsUniRenderVisitor->canvas_ = std::make_unique<RSPaintFilterCanvas>(drawingCanvas.get());

    int leftPos = 800;
    int topPos = 1000;
    int widthValue = 1000;
    int heightValue = 300;
    RectI rect(leftPos, topPos, widthValue, heightValue);
    ASSERT_TRUE(rsUniRenderVisitor->IsOutOfScreenRegion(rect));
}

/*
 * @tc.name: ScaleMirrorIfNeedForWiredScreen
 * @tc.desc: Test RSUniRenderVisitorTest.ScaleMirrorIfNeedForWiredScreen while rotation true
 * @tc.type: FUNC
 * @tc.require: issuesI9SIK7
 */
HWTEST_F(RSUniRenderVisitorTest, ScaleMirrorIfNeedForWiredScreen001, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    RSDisplayNodeConfig displayConfig;
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(10, displayConfig, rsContext->weak_from_this());
    ASSERT_NE(rsDisplayRenderNode, nullptr);
    rsDisplayRenderNode->InitRenderParams();

    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(screenManager, nullptr);
    std::string name = "virtualScreen";
    uint32_t width = 480;
    uint32_t height = 320;
    uint32_t mirrorHeight = 800;
    uint32_t mirrorWidth = 600;

    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);
    auto id = screenManager->CreateVirtualScreen(name, width, height, psurface);
    ASSERT_NE(INVALID_SCREEN_ID, id);

    RSDisplayNodeConfig config1;
    auto mirrorNode = std::make_shared<RSDisplayRenderNode>(id, config1);
    ASSERT_NE(mirrorNode, nullptr);
    mirrorNode->SetScreenRotation(ScreenRotation::ROTATION_0);
    ASSERT_NE(mirrorNode->renderContent_, nullptr);
    mirrorNode->renderContent_->renderProperties_.SetBoundsHeight(mirrorHeight);
    mirrorNode->renderContent_->renderProperties_.SetBoundsWidth(mirrorWidth);
    rsDisplayRenderNode->mirrorSource_ = mirrorNode;

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto drawingCanvas = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(drawingCanvas, nullptr);
    rsUniRenderVisitor->canvas_ = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    rsUniRenderVisitor->ScaleMirrorIfNeedForWiredScreen(*rsDisplayRenderNode, true);

    screenManager->RemoveVirtualScreen(id);
}

/*
 * @tc.name: ScaleMirrorIfNeedForWiredScreen
 * @tc.desc: Test RSUniRenderVisitorTest.ScaleMirrorIfNeedForWiredScreen while rotation false
 * @tc.type: FUNC
 * @tc.require: issuesI9SIK7
 */
HWTEST_F(RSUniRenderVisitorTest, ScaleMirrorIfNeedForWiredScreen002, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    RSDisplayNodeConfig displayConfig;
    NodeId displayId = 10;
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(displayId, displayConfig,
        rsContext->weak_from_this());
    ASSERT_NE(rsDisplayRenderNode, nullptr);
    rsDisplayRenderNode->InitRenderParams();

    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(screenManager, nullptr);
    std::string name = "virtualScreen";
    uint32_t width = 480;
    uint32_t height = 320;

    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);
    auto id = screenManager->CreateVirtualScreen(name, width, height, psurface);
    ASSERT_NE(INVALID_SCREEN_ID, id);

    RSDisplayNodeConfig config1;
    auto mirrorNode = std::make_shared<RSDisplayRenderNode>(id, config1);
    ASSERT_NE(mirrorNode, nullptr);
    mirrorNode->SetScreenRotation(ScreenRotation::ROTATION_0);
    ASSERT_NE(mirrorNode->renderContent_, nullptr);
    mirrorNode->renderContent_->renderProperties_.SetBoundsHeight(800);
    mirrorNode->renderContent_->renderProperties_.SetBoundsWidth(600);
    rsDisplayRenderNode->mirrorSource_ = mirrorNode;

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto drawingCanvas = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(drawingCanvas, nullptr);
    rsUniRenderVisitor->canvas_ = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    rsUniRenderVisitor->ScaleMirrorIfNeedForWiredScreen(*rsDisplayRenderNode, false);

    screenManager->RemoveVirtualScreen(id);
}

/*
 * @tc.name: RotateMirrorCanvasIfNeedForWiredScreen
 * @tc.desc: Test RSUniRenderVisitorTest.RotateMirrorCanvasIfNeedForWiredScreen with rotation_90
 * @tc.type: FUNC
 * @tc.require: issuesI9SIK7
 */
HWTEST_F(RSUniRenderVisitorTest, RotateMirrorCanvasIfNeedForWiredScreen001, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    RSDisplayNodeConfig displayConfig;
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(10, displayConfig, rsContext->weak_from_this());
    rsDisplayRenderNode->InitRenderParams();
    ASSERT_NE(rsDisplayRenderNode, nullptr);

    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(screenManager, nullptr);
    std::string name = "virtualScreen";
    uint32_t width = 480;
    uint32_t height = 320;

    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);
    auto id = screenManager->CreateVirtualScreen(name, width, height, psurface);
    ASSERT_NE(INVALID_SCREEN_ID, id);

    RSDisplayNodeConfig config1;
    auto mirrorNode = std::make_shared<RSDisplayRenderNode>(id, config1);
    ASSERT_NE(mirrorNode, nullptr);
    mirrorNode->SetScreenRotation(ScreenRotation::ROTATION_90);
    rsDisplayRenderNode->mirrorSource_ = mirrorNode;

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto drawingCanvas = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(drawingCanvas, nullptr);
    rsUniRenderVisitor->canvas_ = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    rsUniRenderVisitor->RotateMirrorCanvasIfNeedForWiredScreen(*rsDisplayRenderNode);

    mirrorNode->SetScreenRotation(ScreenRotation::ROTATION_180);
    rsDisplayRenderNode->mirrorSource_ = mirrorNode;
    rsUniRenderVisitor->RotateMirrorCanvasIfNeedForWiredScreen(*rsDisplayRenderNode);

    mirrorNode->SetScreenRotation(ScreenRotation::ROTATION_270);
    rsDisplayRenderNode->mirrorSource_ = mirrorNode;
    rsUniRenderVisitor->RotateMirrorCanvasIfNeedForWiredScreen(*rsDisplayRenderNode);

    screenManager->RemoveVirtualScreen(id);
}

/*
 * @tc.name: ScaleMirrorIfNeed
 * @tc.desc: Test ScaleMirrorIfNeed with cavasRotation_true ScreenScaleMode::FILL_MODE and rotation_90
 * @tc.type: FUNC
 * @tc.require: issuesI9SIK7
 */
HWTEST_F(RSUniRenderVisitorTest, ScaleMirrorIfNeed001, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    RSDisplayNodeConfig displayConfig;
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(10, displayConfig, rsContext->weak_from_this());
    rsDisplayRenderNode->InitRenderParams();
    ASSERT_NE(rsDisplayRenderNode, nullptr);

    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(screenManager, nullptr);
    std::string name = "virtualScreen";
    uint32_t width = 480;
    uint32_t height = 320;

    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);
    auto id = screenManager->CreateVirtualScreen(name, width, height, psurface);
    ASSERT_NE(INVALID_SCREEN_ID, id);
    screenManager->SetVirtualMirrorScreenScaleMode(rsDisplayRenderNode->GetScreenId(), ScreenScaleMode::FILL_MODE);

    RSDisplayNodeConfig config1;
    auto mirrorNode = std::make_shared<RSDisplayRenderNode>(id, config1);
    ASSERT_NE(mirrorNode, nullptr);
    mirrorNode->SetScreenRotation(ScreenRotation::ROTATION_90);
    ASSERT_NE(mirrorNode->renderContent_, nullptr);
    mirrorNode->renderContent_->renderProperties_.SetBoundsHeight(800);
    mirrorNode->renderContent_->renderProperties_.SetBoundsWidth(600);
    rsDisplayRenderNode->mirrorSource_ = mirrorNode;

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto drawingCanvas = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(drawingCanvas, nullptr);
    rsUniRenderVisitor->canvas_ = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    rsUniRenderVisitor->ScaleMirrorIfNeed(*rsDisplayRenderNode, true);

    screenManager->RemoveVirtualScreen(id);
}

/*
 * @tc.name: ScaleMirrorIfNeed
 * @tc.desc: Test ScaleMirrorIfNeed with cavasRotation_false ScreenScaleMode::FILL_MODE and rotation_90
 * @tc.type: FUNC
 * @tc.require: issuesI9SIK7
 */
HWTEST_F(RSUniRenderVisitorTest, ScaleMirrorIfNeed002, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    RSDisplayNodeConfig displayConfig;
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(10, displayConfig, rsContext->weak_from_this());
    rsDisplayRenderNode->InitRenderParams();
    ASSERT_NE(rsDisplayRenderNode, nullptr);

    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(screenManager, nullptr);
    std::string name = "virtualScreen";
    uint32_t width = 480;
    uint32_t height = 320;

    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);
    auto id = screenManager->CreateVirtualScreen(name, width, height, psurface);
    ASSERT_NE(INVALID_SCREEN_ID, id);
    screenManager->SetVirtualMirrorScreenScaleMode(rsDisplayRenderNode->GetScreenId(), ScreenScaleMode::FILL_MODE);

    RSDisplayNodeConfig config1;
    auto mirrorNode = std::make_shared<RSDisplayRenderNode>(id, config1);
    ASSERT_NE(mirrorNode, nullptr);
    mirrorNode->SetScreenRotation(ScreenRotation::ROTATION_90);
    ASSERT_NE(mirrorNode->renderContent_, nullptr);
    mirrorNode->renderContent_->renderProperties_.SetBoundsHeight(800);
    mirrorNode->renderContent_->renderProperties_.SetBoundsWidth(600);
    rsDisplayRenderNode->mirrorSource_ = mirrorNode;

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto drawingCanvas = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(drawingCanvas, nullptr);
    rsUniRenderVisitor->canvas_ = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    rsUniRenderVisitor->ScaleMirrorIfNeed(*rsDisplayRenderNode, false);

    screenManager->RemoveVirtualScreen(id);
}

/*
 * @tc.name: ScaleMirrorIfNeed
 * @tc.desc: Test ScaleMirrorIfNeed with cavasRotation_false ScreenScaleMode::UNISCALE_MODE and rotation_270
 * @tc.type: FUNC
 * @tc.require: issuesI9SIK7
 */
HWTEST_F(RSUniRenderVisitorTest, ScaleMirrorIfNeed003, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    RSDisplayNodeConfig displayConfig;
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(10, displayConfig, rsContext->weak_from_this());
    rsDisplayRenderNode->InitRenderParams();
    ASSERT_NE(rsDisplayRenderNode, nullptr);

    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(screenManager, nullptr);
    std::string name = "virtualScreen";
    uint32_t width = 480;
    uint32_t height = 320;

    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);
    auto id = screenManager->CreateVirtualScreen(name, width, height, psurface);
    ASSERT_NE(INVALID_SCREEN_ID, id);
    screenManager->SetVirtualMirrorScreenScaleMode(rsDisplayRenderNode->GetScreenId(), ScreenScaleMode::UNISCALE_MODE);

    RSDisplayNodeConfig config1;
    auto mirrorNode = std::make_shared<RSDisplayRenderNode>(id, config1);
    ASSERT_NE(mirrorNode, nullptr);
    mirrorNode->SetScreenRotation(ScreenRotation::ROTATION_90);
    ASSERT_NE(mirrorNode->renderContent_, nullptr);
    mirrorNode->renderContent_->renderProperties_.SetBoundsHeight(800);
    mirrorNode->renderContent_->renderProperties_.SetBoundsWidth(600);
    rsDisplayRenderNode->mirrorSource_ = mirrorNode;

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto drawingCanvas = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(drawingCanvas, nullptr);
    rsUniRenderVisitor->canvas_ = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    rsUniRenderVisitor->ScaleMirrorIfNeed(*rsDisplayRenderNode, true);

    screenManager->RemoveVirtualScreen(id);
}

/*
 * @tc.name: UpdateSrcRectForHwcNode
 * @tc.desc: Test RSUniRenderVisitorTest.UpdateSrcRectForHwcNode while GRAPHIC_FLIP_H_ROT90 and isProtected_true
 * @tc.type: FUNC
 * @tc.require: issuesI9V0N7
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateSrcRectForHwcNode001, TestSize.Level2)
{
    auto rsSurfaceRenderNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    ASSERT_NE(rsSurfaceRenderNode->GetRSSurfaceHandler()->GetConsumer(), nullptr);
    uint32_t left = 0;
    uint32_t top = 0;
    uint32_t width = 400;
    uint32_t height = 600;
    RectI dstRect{left, top, width, height};
    rsSurfaceRenderNode->SetSrcRect(dstRect);
    rsSurfaceRenderNode->GetRSSurfaceHandler()->
        GetConsumer()->SetTransform(GraphicTransformType::GRAPHIC_FLIP_H_ROT90);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto drawingCanvas = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(drawingCanvas, nullptr);
    rsUniRenderVisitor->canvas_ = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    ASSERT_TRUE(rsUniRenderVisitor->UpdateSrcRectForHwcNode(*rsSurfaceRenderNode, true));
}

/*
 * @tc.name: UpdateSrcRectForHwcNode
 * @tc.desc: Test RSUniRenderVisitorTest.UpdateSrcRectForHwcNode while GRAPHIC_FLIP_H_ROT90 and isProtected_false
 * @tc.type: FUNC
 * @tc.require: issuesI9V0N7
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateSrcRectForHwcNode002, TestSize.Level2)
{
    auto rsSurfaceRenderNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    ASSERT_NE(rsSurfaceRenderNode->GetRSSurfaceHandler()->GetConsumer(), nullptr);
    uint32_t left = 0;
    uint32_t top = 0;
    uint32_t width = 400;
    uint32_t height = 600;
    RectI dstRect{left, top, width, height};
    rsSurfaceRenderNode->SetSrcRect(dstRect);
    rsSurfaceRenderNode->GetRSSurfaceHandler()->
        GetConsumer()->SetTransform(GraphicTransformType::GRAPHIC_FLIP_H_ROT90);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto drawingCanvas = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(drawingCanvas, nullptr);
    rsUniRenderVisitor->canvas_ = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    ASSERT_TRUE(rsUniRenderVisitor->UpdateSrcRectForHwcNode(*rsSurfaceRenderNode, false));
}

/*
 * @tc.name: UpdateSrcRectForHwcNode
 * @tc.desc: Test RSUniRenderVisitorTest.UpdateSrcRectForHwcNode while GRAPHIC_FLIP_H_ROT180 and isProtected_false
 * @tc.type: FUNC
 * @tc.require: issuesI9V0N7
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateSrcRectForHwcNode003, TestSize.Level2)
{
    auto rsSurfaceRenderNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    ASSERT_NE(rsSurfaceRenderNode->GetRSSurfaceHandler()->GetConsumer(), nullptr);
    uint32_t left = 0;
    uint32_t top = 0;
    uint32_t width = 400;
    uint32_t height = 600;
    RectI dstRect{left, top, width, height};
    rsSurfaceRenderNode->SetSrcRect(dstRect);
    rsSurfaceRenderNode->GetRSSurfaceHandler()->
        GetConsumer()->SetTransform(GraphicTransformType::GRAPHIC_FLIP_H_ROT180);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto drawingCanvas = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(drawingCanvas, nullptr);
    rsUniRenderVisitor->canvas_ = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    ASSERT_TRUE(rsUniRenderVisitor->UpdateSrcRectForHwcNode(*rsSurfaceRenderNode, false));
}

/*
 * @tc.name: CheckAndSetNodeCacheType
 * @tc.desc: Test RSUniRenderVisitorTest.CheckAndSetNodeCacheType with CacheType::NONE
 * @tc.type: FUNC
 * @tc.require: issuesI9V0N7
 */
HWTEST_F(RSUniRenderVisitorTest, CheckAndSetNodeCacheType001, TestSize.Level2)
{
    NodeId id = 0;
    auto node = std::make_shared<RSRenderNode>(id);
    ASSERT_NE(node, nullptr);
    node->InitRenderParams();
    node->SetCacheType(CacheType::CONTENT);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->CheckAndSetNodeCacheType(*node);
    ASSERT_EQ(node->GetCacheType(), CacheType::NONE);
}

/*
 * @tc.name: UpdateHwcNodeEnableByFilterRect
 * @tc.desc: Test RSUniRenderVisitorTest.UpdateHwcNodeEnableByFilterRect with intersect rect
 * @tc.type: FUNC
 * @tc.require: issuesI9V0N7
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnableByFilterRect001, TestSize.Level2)
{
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    auto surfaceNode1 = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode1, nullptr);
    surfaceConfig.id = 2;
    auto surfaceNode2 = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode2, nullptr);

    uint32_t left = 0;
    uint32_t top = 0;
    uint32_t width = 300;
    uint32_t height = 300;
    RectI rect{left, top, width, height};
    surfaceNode2->SetDstRect(rect);
    surfaceNode2->renderContent_->renderProperties_.boundsGeo_->absRect_ = rect;
    surfaceNode1->AddChildHardwareEnabledNode(surfaceNode2);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->UpdateHwcNodeEnableByFilterRect(surfaceNode1, rect);
    ASSERT_TRUE(surfaceNode2->IsHardwareForcedDisabled());
}

/*
 * @tc.name: UpdateHwcNodeEnableByFilterRect
 * @tc.desc: Test RSUniRenderVisitorTest.UpdateHwcNodeEnableByFilterRect with empty rect
 * @tc.type: FUNC
 * @tc.require: issuesI9V0N7
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnableByFilterRect002, TestSize.Level2)
{
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    auto surfaceNode1 = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode1, nullptr);

    uint32_t left = 0;
    uint32_t top = 0;
    uint32_t width = 0;
    uint32_t height = 0;
    RectI rect{left, top, width, height};
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->UpdateHwcNodeEnableByFilterRect(surfaceNode1, rect);
}

/*
 * @tc.name: UpdateHwcNodeEnableByFilterRect
 * @tc.desc: Test RSUniRenderVisitorTest.UpdateHwcNodeEnableByFilterRect with no hwcNode
 * @tc.type: FUNC
 * @tc.require: issuesI9V0N7
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnableByFilterRect003, TestSize.Level2)
{
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode, nullptr);

    uint32_t left = 0;
    uint32_t top = 0;
    uint32_t width = 300;
    uint32_t height = 300;
    RectI rect{left, top, width, height};
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->UpdateHwcNodeEnableByFilterRect(surfaceNode, rect);
}

/*
 * @tc.name: CheckMergeGlobalFilterForDisplay
 * @tc.desc: Test RSUniRenderVisitorTest.CheckMergeGlobalFilterForDisplay
 * @tc.type: FUNC
 * @tc.require: issuesI9V0N7
 */
HWTEST_F(RSUniRenderVisitorTest, CheckMergeGlobalFilterForDisplay001, TestSize.Level2)
{
    auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();
    NodeId id0 = 0;
    pid_t pid0 = ExtractPid(id0);
    nodeMap.renderNodeMap_[pid0][id0] = nullptr;
    NodeId id1 = 1;
    pid_t pid1 = ExtractPid(id1);
    auto node1 = std::make_shared<RSRenderNode>(id1);
    nodeMap.renderNodeMap_[pid1][id1] = node1;
    NodeId id2 = 2;
    pid_t pid2 = ExtractPid(id2);
    auto node2 = std::make_shared<RSRenderNode>(id2);
    nodeMap.renderNodeMap_[pid2][id2] = node2;

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    uint32_t left = 0;
    uint32_t top = 0;
    uint32_t width = 300;
    uint32_t height = 300;
    RectI rect{left, top, width, height};
    rsUniRenderVisitor->containerFilter_.insert({node2->GetId(), rect});
    NodeId displayNodeId = 3;
    RSDisplayNodeConfig config;
    rsUniRenderVisitor->curDisplayNode_ = std::make_shared<RSDisplayRenderNode>(displayNodeId, config);
    auto dirtyRegion = Occlusion::Region{ Occlusion::Rect{ rect } };
    rsUniRenderVisitor->CheckMergeGlobalFilterForDisplay(dirtyRegion);
}

/*
 * @tc.name: UpdateOccludedStatusWithFilterNode
 * @tc.desc: Test RSUniRenderVisitorTest.UpdateOccludedStatusWithFilterNode while surface node nullptr
 * @tc.type: FUNC
 * @tc.require: issuesI9V0N7
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateOccludedStatusWithFilterNode001, TestSize.Level2)
{
    std::shared_ptr<RSSurfaceRenderNode> surfaceNode = nullptr;
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->UpdateOccludedStatusWithFilterNode(surfaceNode);
}

/*
 * @tc.name: UpdateOccludedStatusWithFilterNode
 * @tc.desc: Test RSUniRenderVisitorTest.UpdateOccludedStatusWithFilterNode with surfaceNode
 * @tc.type: FUNC
 * @tc.require: issuesI9V0N7
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateOccludedStatusWithFilterNode002, TestSize.Level2)
{
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    auto surfaceNode1 = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode1, nullptr);
    std::shared_ptr<RSFilter> filter = RSFilter::CreateBlurFilter(1.0f, 1.0f);
    ASSERT_NE(surfaceNode1->renderContent_, nullptr);
    surfaceNode1->renderContent_->renderProperties_.SetBackgroundFilter(filter);
    surfaceNode1->isOccludedByFilterCache_ = true;
    auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();
    NodeId id1 = 2;
    auto filterNode1 = std::make_shared<RSRenderNode>(id1);
    ASSERT_NE(filterNode1, nullptr);
    pid_t pid1 = ExtractPid(id1);
    nodeMap.renderNodeMap_[pid1][id1] = filterNode1;
    NodeId id2 = 3;
    pid_t pid2 = ExtractPid(id2);
    auto filterNode2 = std::make_shared<RSRenderNode>(id2);
    ASSERT_NE(filterNode2, nullptr);
    ASSERT_NE(filterNode2->renderContent_, nullptr);
    filterNode2->renderContent_->renderProperties_.SetBackgroundFilter(filter);
    nodeMap.renderNodeMap_[pid2][id2] = filterNode2;
    surfaceNode1->visibleFilterChild_.emplace_back(filterNode1->GetId());
    surfaceNode1->visibleFilterChild_.emplace_back(filterNode2->GetId());

    ASSERT_FALSE(filterNode1->isOccluded_);
    ASSERT_FALSE(filterNode2->isOccluded_);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->UpdateOccludedStatusWithFilterNode(surfaceNode1);
    ASSERT_TRUE(filterNode2->isOccluded_);
}

/*
 * @tc.name: MarkBlurIntersectWithDRM
 * @tc.desc: Test RSUniRenderVisitorTest.MarkBlurIntersectWithDRM001 while surface node nullptr
 * @tc.type: FUNC
 * @tc.require: issuesIAQZ4I
 */
HWTEST_F(RSUniRenderVisitorTest, MarkBlurIntersectWithDRM001, TestSize.Level2)
{
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode, nullptr);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->MarkBlurIntersectWithDRM(surfaceNode);
}

/*
 * @tc.name: CheckMergeDisplayDirtyByTransparentFilter
 * @tc.desc: Test RSUniRenderVisitorTest.CheckMergeDisplayDirtyByTransparentFilter with mainWindow
 * @tc.type: FUNC
 * @tc.require: issuesI9V0N7
 */
HWTEST_F(RSUniRenderVisitorTest, CheckMergeDisplayDirtyByTransparentFilter001, TestSize.Level2)
{
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    ASSERT_TRUE(surfaceNode->GetVisibleRegion().IsEmpty());

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    uint32_t left = 0;
    uint32_t top = 0;
    uint32_t width = 260;
    uint32_t height = 600;
    RectI rect{left, top, width, height};
    Occlusion::Region region{rect};
    NodeId displayNodeId = 2;
    RSDisplayNodeConfig config;
    rsUniRenderVisitor->curDisplayNode_ = std::make_shared<RSDisplayRenderNode>(displayNodeId, config);
    rsUniRenderVisitor->curDisplayNode_->InitRenderParams();
    ASSERT_NE(rsUniRenderVisitor->curDisplayNode_, nullptr);
    rsUniRenderVisitor->CheckMergeDisplayDirtyByTransparentFilter(surfaceNode, region);
}

/*
 * @tc.name: CheckMergeDisplayDirtyByTransparentFilter
 * @tc.desc: Test RSUniRenderVisitorTest.CheckMergeDisplayDirtyByTransparentFilter
 * @tc.type: FUNC
 * @tc.require: issuesI9V0N7
 */
HWTEST_F(RSUniRenderVisitorTest, CheckMergeDisplayDirtyByTransparentFilter002, TestSize.Level2)
{
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetSurfaceNodeType(RSSurfaceNodeType::ABILITY_COMPONENT_NODE);
    auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();
    NodeId id0 = 0;
    pid_t pid0 = ExtractPid(id0);
    nodeMap.renderNodeMap_[pid0][id0] = nullptr;
    NodeId id1 = 1;
    pid_t pid1 = ExtractPid(id1);
    auto node1 = std::make_shared<RSRenderNode>(id1);
    nodeMap.renderNodeMap_[pid1][id1] = node1;
    NodeId id2 = 2;
    pid_t pid2 = ExtractPid(id2);
    auto node2 = std::make_shared<RSRenderNode>(id2);
    nodeMap.renderNodeMap_[pid2][id2] = node2;

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    uint32_t left1 = 0;
    uint32_t top1 = 0;
    uint32_t width1 = 260;
    uint32_t height1 = 600;
    RectI rect1{left1, top1, width1, height1};
    Occlusion::Region region1{rect1};
    uint32_t left2 = 0;
    uint32_t top2 = 0;
    uint32_t width2 = 200;
    uint32_t height2 = 300;
    RectI rect2{left2, top2, width2, height2};
    rsUniRenderVisitor->transparentCleanFilter_[surfaceNode->GetId()].push_back({id0, rect2});
    rsUniRenderVisitor->transparentCleanFilter_[surfaceNode->GetId()].push_back({id1, rect2});
    rsUniRenderVisitor->transparentCleanFilter_[surfaceNode->GetId()].push_back({id2, rect2});
    float blurRadiusX = 1.0f;
    float blurRadiusY = 1.0f;
    std::shared_ptr<RSFilter> filter = RSFilter::CreateBlurFilter(blurRadiusX, blurRadiusY);
    ASSERT_NE(node2->renderContent_, nullptr);
    node2->renderContent_->renderProperties_.SetBackgroundFilter(filter);
    node2->renderContent_->renderProperties_.SetFilter(filter);
    NodeId displayNodeId = 3;
    RSDisplayNodeConfig config;
    rsUniRenderVisitor->curDisplayNode_ = std::make_shared<RSDisplayRenderNode>(displayNodeId, config);
    rsUniRenderVisitor->curDisplayNode_->InitRenderParams();
    ASSERT_NE(rsUniRenderVisitor->curDisplayNode_, nullptr);

    rsUniRenderVisitor->CheckMergeDisplayDirtyByTransparentFilter(surfaceNode, region1);
}

/*
 * @tc.name: CheckMergeDisplayDirtyByTransparentRegions
 * @tc.desc: Test RSUniRenderVisitorTest.CheckMergeDisplayDirtyByTransparentRegions with container window
 * @tc.type: FUNC
 * @tc.require: issuesI9V0N7
 */
HWTEST_F(RSUniRenderVisitorTest, CheckMergeDisplayDirtyByTransparentRegions001, TestSize.Level2)
{
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode, nullptr);
    uint32_t left1 = 0;
    uint32_t top1 = 0;
    uint32_t width1 = 200;
    uint32_t height1 = 300;
    RectI rect{left1, top1, width1, height1};
    surfaceNode->GetDirtyManager()->SetCurrentFrameDirtyRect(rect);
    surfaceNode->containerConfig_.hasContainerWindow_ = true;
    uint32_t left2 = 0;
    uint32_t top2 = 0;
    uint32_t width2 = 260;
    uint32_t height2 = 600;
    RectI rect1{left2, top2, width2, height2};
    Occlusion::Region region1{rect1};
    surfaceNode->containerRegion_ = region1;

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    NodeId displayNodeId = 3;
    RSDisplayNodeConfig config;
    rsUniRenderVisitor->curDisplayNode_ = std::make_shared<RSDisplayRenderNode>(displayNodeId, config);
    rsUniRenderVisitor->curDisplayNode_->InitRenderParams();
    ASSERT_NE(rsUniRenderVisitor->curDisplayNode_, nullptr);
    rsUniRenderVisitor->CheckMergeDisplayDirtyByTransparentRegions(*surfaceNode);
}

/*
 * @tc.name: CheckMergeDisplayDirtyByTransparentRegions
 * @tc.desc: Test RSUniRenderVisitorTest.CheckMergeDisplayDirtyByTransparentRegions with no container window
 * @tc.type: FUNC
 * @tc.require: issuesI9V0N7
 */
HWTEST_F(RSUniRenderVisitorTest, CheckMergeDisplayDirtyByTransparentRegions002, TestSize.Level2)
{
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->isNodeDirty_ = true;
    uint32_t left = 0;
    uint32_t top = 0;
    uint32_t width = 260;
    uint32_t height = 600;
    Occlusion::Rect rect1{left, top, width, height};
    Occlusion::Region region1{rect1};
    surfaceNode->transparentRegion_ = region1;

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    NodeId displayNodeId = 3;
    RSDisplayNodeConfig config;
    rsUniRenderVisitor->curDisplayNode_ = std::make_shared<RSDisplayRenderNode>(displayNodeId, config);
    rsUniRenderVisitor->curDisplayNode_->InitRenderParams();
    ASSERT_NE(rsUniRenderVisitor->curDisplayNode_, nullptr);
    rsUniRenderVisitor->CheckMergeDisplayDirtyByTransparentRegions(*surfaceNode);
}

/**
 * @tc.name: UpdateHwcNodeInfoForAppNode
 * @tc.desc: Test RSUniRenderVisitorTest.UpdateHwcNodeInfoForAppNode with not nullptr
 * @tc.type: FUNC
 * @tc.require: issuesIAE6YM
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeInfoForAppNode, TestSize.Level2)
{
    auto rsSurfaceRenderNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(rsSurfaceRenderNode, nullptr);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->UpdateHwcNodeInfoForAppNode(*rsSurfaceRenderNode);
}

/**
 * @tc.name: UpdateHwcNodeRectInSkippedSubTree
 * @tc.desc: Test RSUnitRenderVisitorTest.UpdateHwcNodeRectInSkippedSubTree with not nullptr
 * @tc.type: FUNC
 * @tc.require: issuesIAE6YM
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeRectInSkippedSubTree, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    int id = 0;
    auto node = std::make_shared<RSRenderNode>(id);
    rsUniRenderVisitor->UpdateHwcNodeRectInSkippedSubTree(*node);
}

/**
 * @tc.name: UpdateHwcNodeEnableByGlobalFilter
 * @tc.desc: Test RSUnitRenderVisitorTest.UpdateHwcNodeEnableByGlobalFilter with not nullptr
 * @tc.type: FUNC
 * @tc.require: issuesIAE6YM
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnableByGlobalFilter, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    NodeId id = 0;
    std::weak_ptr<RSContext> context;
    auto node = std::make_shared<RSSurfaceRenderNode>(id, context);
    ASSERT_NE(node, nullptr);
    NodeId displayNodeId = 3;
    RSDisplayNodeConfig config;
    rsUniRenderVisitor->curDisplayNode_ = std::make_shared<RSDisplayRenderNode>(displayNodeId, config);
    rsUniRenderVisitor->curDisplayNode_->InitRenderParams();

    rsUniRenderVisitor->UpdateHwcNodeEnableByGlobalFilter(node);
}

/**
 * @tc.name: CollectEffectInfo001
 * @tc.desc: Test RSUnitRenderVisitorTest.CollectEffectInfo with not parent node.
 * @tc.type: FUNC
 * @tc.require: issueIAG8BF
 */
HWTEST_F(RSUniRenderVisitorTest, CollectEffectInfo001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    constexpr NodeId nodeId = 1;
    auto node = std::make_shared<RSRenderNode>(nodeId);
    ASSERT_NE(node, nullptr);
    rsUniRenderVisitor->CollectEffectInfo(*node);
}

/**
 * @tc.name: CollectEffectInfo002
 * @tc.desc: Test RSUnitRenderVisitorTest.CollectEffectInfo with parent node, need filter
 * @tc.type: FUNC
 * @tc.require: issueIAG8BF
 */
HWTEST_F(RSUniRenderVisitorTest, CollectEffectInfo002, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    constexpr NodeId nodeId = 1;
    constexpr NodeId parentNodeId = 2;
    auto node = std::make_shared<RSRenderNode>(nodeId);
    ASSERT_NE(node, nullptr);
    auto parent = std::make_shared<RSRenderNode>(parentNodeId);
    ASSERT_NE(parent, nullptr);
    node->InitRenderParams();
    parent->InitRenderParams();
    parent->AddChild(node);
    node->GetMutableRenderProperties().needFilter_ = true;
    rsUniRenderVisitor->CollectEffectInfo(*node);
    ASSERT_TRUE(parent->ChildHasVisibleFilter());
}

/**
 * @tc.name: CollectEffectInfo003
 * @tc.desc: Test RSUnitRenderVisitorTest.CollectEffectInfo with parent node, useEffect
 * @tc.type: FUNC
 * @tc.require: issueIAG8BF
 */
HWTEST_F(RSUniRenderVisitorTest, CollectEffectInfo003, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    constexpr NodeId nodeId = 1;
    constexpr NodeId parentNodeId = 2;
    auto node = std::make_shared<RSRenderNode>(nodeId);
    ASSERT_NE(node, nullptr);
    auto parent = std::make_shared<RSRenderNode>(parentNodeId);
    ASSERT_NE(parent, nullptr);
    node->InitRenderParams();
    parent->InitRenderParams();
    parent->AddChild(node);
    node->GetMutableRenderProperties().useEffect_ = true;
    rsUniRenderVisitor->CollectEffectInfo(*node);
    ASSERT_TRUE(parent->ChildHasVisibleEffect());
}

/*
 * @tc.name: CheckIsGpuOverDrawBufferOptimizeNode001
 * @tc.desc: Verify function CheckIsGpuOverDrawBufferOptimizeNode while node has no child
 * @tc.type: FUNC
 * @tc.require: issuesIAE0Q3
 */
HWTEST_F(RSUniRenderVisitorTest, CheckIsGpuOverDrawBufferOptimizeNode001, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    RSSurfaceRenderNodeConfig config;
    config.id = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    ASSERT_NE(surfaceNode, nullptr);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->CheckIsGpuOverDrawBufferOptimizeNode(*surfaceNode);
    ASSERT_FALSE(surfaceNode->IsGpuOverDrawBufferOptimizeNode());
}

/*
 * @tc.name: CheckIsGpuOverDrawBufferOptimizeNode002
 * @tc.desc: Verify function CheckIsGpuOverDrawBufferOptimizeNode while node has child
 * @tc.type: FUNC
 * @tc.require: issuesIAE0Q3
 */
HWTEST_F(RSUniRenderVisitorTest, CheckIsGpuOverDrawBufferOptimizeNode002, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    RSSurfaceRenderNodeConfig config;
    config.id = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_NODE;
    surfaceNode->SetIsScale(true);

    NodeId childNodeId = config.id + 1;
    auto childNode = std::make_shared<RSSurfaceRenderNode>(childNodeId);
    surfaceNode->AddChild(childNode, -1);
    surfaceNode->GenerateFullChildrenList();

    NodeId rootNodeId = childNodeId + 1;
    auto rootNode = std::make_shared<RSRenderNode>(rootNodeId);
    childNode->AddChild(rootNode, -1);

    NodeId canvasNodeId = rootNodeId + 1;
    auto canvasNode = std::make_shared<RSRenderNode>(canvasNodeId);
    auto& properties = canvasNode->GetMutableRenderProperties();
    Color color(0, 0, 0, MAX_ALPHA);
    properties.SetBackgroundColor(color);
    rootNode->AddChild(canvasNode, -1);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curCornerRadius_ = {1.0f, 1.0f, 1.0f, 1.0f};
    rsUniRenderVisitor->CheckIsGpuOverDrawBufferOptimizeNode(*surfaceNode);
    ASSERT_TRUE(surfaceNode->IsGpuOverDrawBufferOptimizeNode());
}

/*
 * @tc.name: CollectFilterInfoAndUpdateDirty001
 * @tc.desc: Verify function CollectFilterInfoAndUpdateDirty without curSurfaceNode_
 * @tc.type: FUNC
 * @tc.require: issuesIAE0Q3
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
    rsUniRenderVisitor->CollectFilterInfoAndUpdateDirty(*node, *dirtyManager, rect, rect);
    ASSERT_FALSE(rsUniRenderVisitor->containerFilter_.empty());
}

/*
 * @tc.name: CollectFilterInfoAndUpdateDirty002
 * @tc.desc: Verify function CollectFilterInfoAndUpdateDirty with non-transparent curSurfaceNode_
 * @tc.type: FUNC
 * @tc.require: issuesIAE0Q3
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
    rsUniRenderVisitor->CollectFilterInfoAndUpdateDirty(*node, *dirtyManager, rect, rect);
    ASSERT_FALSE(rsUniRenderVisitor->globalFilter_.empty());
}

/*
 * @tc.name: CollectFilterInfoAndUpdateDirty003
 * @tc.desc: Verify function CollectFilterInfoAndUpdateDirty with transparent curSurfaceNode_
 * @tc.type: FUNC
 * @tc.require: issuesIAE0Q3
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
    rsUniRenderVisitor->CollectFilterInfoAndUpdateDirty(*node, *dirtyManager, rect, rect);
    ASSERT_FALSE(rsUniRenderVisitor->transparentCleanFilter_.empty());
}

/**
 * @tc.name: UpdateHardwareStateByHwcNodeBackgroundAlpha
 * @tc.desc: Test RSUnitRenderVisitorTest.UpdateHardwareStateByHwcNodeBackgroundAlpha
 * @tc.type: FUNC
 * @tc.require: IAHFXD
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHardwareStateByHwcNodeBackgroundAlpha, TestSize.Level1)
{
    std::vector<std::weak_ptr<RSSurfaceRenderNode>> hwcNodes;
    std::weak_ptr<RSSurfaceRenderNode> hwcNode;
    hwcNodes.push_back(hwcNode);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->UpdateHardwareStateByHwcNodeBackgroundAlpha(hwcNodes);
}

/**
 * @tc.name: IsNodeAboveInsideOfNodeBelow
 * @tc.desc: Test RSUnitRenderVisitorTest.IsNodeAboveInsideOfNodeBelow
 * @tc.type: FUNC
 * @tc.require: IAHFXD
 */
HWTEST_F(RSUniRenderVisitorTest, IsNodeAboveInsideOfNodeBelow, TestSize.Level1)
{
    const RectI rectAbove;
    std::list<RectI> hwcNodeRectList;
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->IsNodeAboveInsideOfNodeBelow(rectAbove, hwcNodeRectList);
}

/**
 * @tc.name: IsFirstFrameOfOverdrawSwitch
 * @tc.desc: Test IsFirstFrameOfOverdrawSwitch
 * @tc.type: FUNC
 * @tc.require: issueIAJSIS
 */
HWTEST_F(RSUniRenderVisitorTest, IsFirstFrameOfOverdrawSwitch, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    ASSERT_EQ(rsUniRenderVisitor->IsFirstFrameOfOverdrawSwitch(), false);
}

} // OHOS::Rosen