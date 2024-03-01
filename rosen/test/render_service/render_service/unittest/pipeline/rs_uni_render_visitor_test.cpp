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
#include "pipeline/rs_uni_render_visitor.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_proxy_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_uni_render_engine.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "pipeline/rs_effect_render_node.h"
#include "pipeline/rs_processor_factory.h"
#include "pipeline/rs_uni_render_util.h"
#include "rs_test_util.h"
#include "system/rs_system_parameters.h"
#include "draw/color.h"
#include "pipeline/rs_main_thread.h"
#include "mock/mock_matrix.h"

using namespace testing;
using namespace testing::ext;

namespace {
    const std::string DEFAULT_NODE_NAME = "1";
    const std::string INVALID_NODE_NAME = "2";
    const int DEFAULT_DIRTY_REGION_WIDTH = 10;
    const int DEFAULT_DIRTY_REGION_HEIGHT = 10;
    constexpr uint32_t DEFAULT_CANVAS_WIDTH = 800;
    constexpr uint32_t DEFAULT_CANVAS_HEIGHT = 600;
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
}
void RSUniRenderVisitorTest::TearDownTestCase() {}
void RSUniRenderVisitorTest::SetUp() {}
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
 * @tc.name: PrepareRootRenderNodeTest
 * @tc.desc: Test RSUniRenderVisitorTest.PrepareRootRenderNodeTest
 * @tc.type: FUNC
 * @tc.require: issueI79U8E
 */
HWTEST_F(RSUniRenderVisitorTest, PrepareRootRenderNodeTest, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    auto rsRootRenderNode = std::make_shared<RSRootRenderNode>(0, rsContext->weak_from_this());
    ASSERT_NE(rsRootRenderNode, nullptr);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->drivenInfo_ = std::make_unique<DrivenInfo>();
    ASSERT_NE(rsUniRenderVisitor->drivenInfo_, nullptr);
    
    rsUniRenderVisitor->PrepareRootRenderNode(*rsRootRenderNode);
    rsUniRenderVisitor->drivenInfo_->drivenUniTreePrepareMode = DrivenUniTreePrepareMode::PREPARE_DRIVEN_NODE_AFTER;
    rsUniRenderVisitor->PrepareRootRenderNode(*rsRootRenderNode);
    rsUniRenderVisitor->curSurfaceDirtyManager_ = nullptr;
    rsUniRenderVisitor->PrepareRootRenderNode(*rsRootRenderNode);
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

    RSSurfaceRenderNodeConfig config;
    config.id = 10;
    config.name = "scbScreenNode";
    auto scbScreenNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    ASSERT_NE(scbScreenNode, nullptr);
    scbScreenNode->SetSurfaceNodeType(RSSurfaceNodeType::SCB_SCREEN_NODE);

    config.id = 11;
    config.name = "leashWindowNode";
    auto leashWindowNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    ASSERT_NE(leashWindowNode, nullptr);
    leashWindowNode->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);

    rsCanvasRenderNode->SetParent(std::weak_ptr<RSSurfaceRenderNode>(leashWindowNode));
    rsUniRenderVisitor->PrepareCanvasRenderNode(*rsCanvasRenderNode);

    rsCanvasRenderNode->SetParent(std::weak_ptr<RSSurfaceRenderNode>(scbScreenNode));
    RSDisplayNodeConfig displayConfig;
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(20, displayConfig, rsContext->weak_from_this());
    rsUniRenderVisitor->curDisplayDirtyManager_ = rsDisplayRenderNode->GetDirtyManager();
    auto pairedNode = RSTestUtil::CreateSurfaceNode();
    rsCanvasRenderNode->sharedTransitionParam_ = {rsCanvasRenderNode->GetId(), pairedNode};
    rsUniRenderVisitor->drivenInfo_ = std::make_unique<DrivenInfo>();
    ASSERT_NE(rsUniRenderVisitor->drivenInfo_, nullptr);
    rsUniRenderVisitor->currentVisitDisplay_ = 0;
    rsUniRenderVisitor->drivenInfo_->isPrepareLeashWinSubTree = true;
    rsCanvasRenderNode->isMarkDriven_ = true;
    rsCanvasRenderNode->isMarkDrivenRender_ = true;
    rsCanvasRenderNode->isContentChanged_ = true;
    rsUniRenderVisitor->PrepareCanvasRenderNode(*rsCanvasRenderNode);

    rsUniRenderVisitor->drivenInfo_->drivenUniTreePrepareMode = DrivenUniTreePrepareMode::PREPARE_DRIVEN_NODE_AFTER;
    rsUniRenderVisitor->PrepareCanvasRenderNode(*rsCanvasRenderNode);

    rsUniRenderVisitor->drivenInfo_->drivenUniTreePrepareMode = DrivenUniTreePrepareMode::PREPARE_DRIVEN_NODE_BEFORE;
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
    std::vector<RectI> dirtyRects;
    dirtyRects.push_back(rect);
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
    rsUniRenderVisitor->dirtyFlag_ = true;
    auto rsBaseRenderNode = std::make_shared<RSBaseRenderNode>(10, rsContext->weak_from_this());
    rsUniRenderVisitor->logicParentNode_ = rsBaseRenderNode->weak_from_this();
    rsUniRenderVisitor->PrepareProxyRenderNode(*rsProxyRenderNode);
    auto drawingCanvas = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(drawingCanvas, nullptr);
    rsUniRenderVisitor->canvas_ = std::make_unique<RSPaintFilterCanvas>(drawingCanvas.get());
    system::SetParameter("persist.sys.graphic.proxyNodeDebugEnabled", "0");
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
    config.id = 11;
    config.name = "leashWindowTestNode";
    auto leashWindowNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    leashWindowNode->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);
    
    RSDisplayNodeConfig displayConfig;
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(12, displayConfig, rsContext->weak_from_this());
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
    defaultSurfaceRenderNode->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    
    RSDisplayNodeConfig displayConfig;
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(11, displayConfig, rsContext->weak_from_this());
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
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();

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
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(11, displayConfig, rsContext->weak_from_this());
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();

    Occlusion::Rect rect{0, 80, 2560, 1600};
    Occlusion::Region region{rect};
    VisibleData vData;
    std::map<uint32_t, RSVisibleLevel> pidVisMap;

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
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(11, displayConfig, rsContext->weak_from_this());
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
    Occlusion::Rect rect{0, 80, 2560, 1600};
    Occlusion::Region region{rect};
    VisibleData vData;
    std::map<uint32_t, RSVisibleLevel> pidVisMap;
    rsSurfaceRenderNode1->SetVisibleRegionRecursive(region, vData, pidVisMap);

    config.id = 11;
    auto rsSurfaceRenderNode2 = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    Occlusion::Rect rect2{100, 100, 500, 1500};
    Occlusion::Region region2{rect2};
    rsSurfaceRenderNode2->SetVisibleRegionRecursive(region2, vData, pidVisMap);

    RSDisplayNodeConfig displayConfig;
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(9, displayConfig, rsContext->weak_from_this());
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
 * @tc.name: CopyPropertyForParallelVisitorTest
 * @tc.desc: Test RSUniRenderVisitorTest.CopyPropertyForParallelVisitorTest
 * @tc.type: FUNC
 * @tc.require: issueI79KM8
 */
HWTEST_F(RSUniRenderVisitorTest, CopyPropertyForParallelVisitorTest, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    auto newRsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    newRsUniRenderVisitor->CopyPropertyForParallelVisitor(rsUniRenderVisitor.get());
    ASSERT_EQ(rsUniRenderVisitor->doAnimate_, newRsUniRenderVisitor->doAnimate_);
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
 * @tc.name: DrawTargetSurfaceDirtyRegionForDFX002
 * @tc.desc: Test RSUniRenderVisitorTest.DrawTargetSurfaceDirtyRegionForDFX
 * @tc.type: FUNC
 * @tc.require: issueI79KM8
 */
HWTEST_F(RSUniRenderVisitorTest, DrawTargetSurfaceDirtyRegionForDFX002, TestSize.Level1)
{
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(id, config);
    auto surfaceNodeNull = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNodeNull, nullptr);
    surfaceNodeNull = nullptr;
    node->curAllSurfaces_.push_back(surfaceNodeNull);
    auto surfaceNodeNotApp = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNodeNotApp, nullptr);
    surfaceNodeNotApp->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_NODE;
    node->curAllSurfaces_.push_back(surfaceNodeNotApp);
    auto surfaceNodeMain = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNodeMain, nullptr);
    surfaceNodeMain->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    surfaceNodeMain->name_ = DEFAULT_NODE_NAME;
    node->curAllSurfaces_.push_back(surfaceNodeMain);
    auto surfaceNodeMain2 = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNodeMain2, nullptr);
    surfaceNodeMain2->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    surfaceNodeMain2->name_ = INVALID_NODE_NAME;
    node->curAllSurfaces_.push_back(surfaceNodeMain2);
    
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    auto& vs = rsUniRenderVisitor->dfxTargetSurfaceNames_;
    vs.push_back(DEFAULT_NODE_NAME);
    ASSERT_EQ(true, rsUniRenderVisitor->CheckIfSurfaceTargetedForDFX(surfaceNodeMain->name_));
    ASSERT_EQ(false, rsUniRenderVisitor->CheckIfSurfaceTargetedForDFX(surfaceNodeMain2->name_));
    rsUniRenderVisitor->DrawTargetSurfaceDirtyRegionForDFX(*node);
}

/**
 * @tc.name: DrawTargetSurfaceDirtyRegionForDFX003
 * @tc.desc: Test RSUniRenderVisitorTest.DrawTargetSurfaceDirtyRegionForDFX
 * @tc.type: FUNC
 * @tc.require: issueI79KM8
 */
HWTEST_F(RSUniRenderVisitorTest, DrawTargetSurfaceDirtyRegionForDFX003, TestSize.Level1)
{
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(id, config);
    auto surfaceNodeMain = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNodeMain, nullptr);
    surfaceNodeMain->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    surfaceNodeMain->name_ = DEFAULT_NODE_NAME;
    node->curAllSurfaces_.push_back(surfaceNodeMain);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    auto& vs = rsUniRenderVisitor->dfxTargetSurfaceNames_;
    vs.push_back(DEFAULT_NODE_NAME);
    ASSERT_EQ(true, rsUniRenderVisitor->CheckIfSurfaceTargetedForDFX(surfaceNodeMain->name_));
    rsUniRenderVisitor->dirtyRegionDebugType_ = DirtyRegionDebugType::DISABLED;
    rsUniRenderVisitor->DrawTargetSurfaceDirtyRegionForDFX(*node);
}

/**
 * @tc.name: DrawTargetSurfaceDirtyRegionForDFX004
 * @tc.desc: Test RSUniRenderVisitorTest.DrawTargetSurfaceDirtyRegionForDFX
 * @tc.type: FUNC
 * @tc.require: issueI79KM8
 */
HWTEST_F(RSUniRenderVisitorTest, DrawTargetSurfaceDirtyRegionForDFX004, TestSize.Level1)
{
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(id, config);
    auto surfaceNodeMain = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNodeMain, nullptr);
    surfaceNodeMain->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    surfaceNodeMain->name_ = DEFAULT_NODE_NAME;
    auto dirtyManager = surfaceNodeMain->GetDirtyManager();
    ASSERT_NE(dirtyManager, nullptr);
    dirtyManager->dirtyCanvasNodeInfo_.resize(DirtyRegionType::TYPE_AMOUNT);
    dirtyManager->dirtySurfaceNodeInfo_.resize(DirtyRegionType::TYPE_AMOUNT);
    node->curAllSurfaces_.push_back(surfaceNodeMain);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    auto& vs = rsUniRenderVisitor->dfxTargetSurfaceNames_;
    vs.push_back(DEFAULT_NODE_NAME);
    ASSERT_EQ(true, rsUniRenderVisitor->CheckIfSurfaceTargetedForDFX(surfaceNodeMain->name_));
    rsUniRenderVisitor->dirtyRegionDebugType_ = DirtyRegionDebugType::UPDATE_DIRTY_REGION;
    rsUniRenderVisitor->DrawTargetSurfaceDirtyRegionForDFX(*node);
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
 * @tc.name: SetSurfaceGlobalAlignedDirtyRegion001
 * @tc.desc: Test RSUniRenderVisitorTest.SetSurfaceGlobalAlignedDirtyRegion isDirtyRegionAlignedEnable_ is false
 * @tc.type: FUNC
 * @tc.require: issueI79KM8
 */
HWTEST_F(RSUniRenderVisitorTest, SetSurfaceGlobalAlignedDirtyRegion001, TestSize.Level1)
{
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(id, config);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->isDirtyRegionAlignedEnable_ = false;
    rsUniRenderVisitor->SetSurfaceGlobalAlignedDirtyRegion(node, Occlusion::Region(RectI(0, 0, 0, 0)));
}

/**
 * @tc.name: SetSurfaceGlobalAlignedDirtyRegion002
 * @tc.desc: Test RSUniRenderVisitorTest.SetSurfaceGlobalAlignedDirtyRegion isDirtyRegionAlignedEnable_ is false
 * @tc.type: FUNC
 * @tc.require: issueI79KM8
 */
HWTEST_F(RSUniRenderVisitorTest, SetSurfaceGlobalAlignedDirtyRegion002, TestSize.Level1)
{
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(id, config);
    ASSERT_NE(node, nullptr);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->isDirtyRegionAlignedEnable_ = true;

    std::shared_ptr<RSSurfaceRenderNode> surfaceNodeNull = nullptr;
    node->curAllSurfaces_.push_back(surfaceNodeNull);

    auto surfaceNodeDefault = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNodeDefault, nullptr);
    surfaceNodeDefault->nodeType_ = RSSurfaceNodeType::DEFAULT;
    node->curAllSurfaces_.push_back(surfaceNodeDefault);

    auto surfaceNodeMain = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNodeMain, nullptr);
    surfaceNodeMain->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    node->curAllSurfaces_.push_back(surfaceNodeMain);

    rsUniRenderVisitor->SetSurfaceGlobalAlignedDirtyRegion(node, Occlusion::Region(RectI(0, 0, 0, 0)));
    rsUniRenderVisitor->SetSurfaceGlobalAlignedDirtyRegion(node, Occlusion::Region(RectI(0,
        0, DEFAULT_DIRTY_REGION_WIDTH, DEFAULT_DIRTY_REGION_HEIGHT)));
}

/**
 * @tc.name: AlignGlobalAndSurfaceDirtyRegions001
 * @tc.desc: Test RSUniRenderVisitorTest.AlignGlobalAndSurfaceDirtyRegions
 * @tc.type: FUNC
 * @tc.require: issueI79KM8
 */
HWTEST_F(RSUniRenderVisitorTest, AlignGlobalAndSurfaceDirtyRegions001, TestSize.Level1)
{
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(id, config);
    ASSERT_NE(node, nullptr);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->isDirtyRegionAlignedEnable_ = true;

    std::shared_ptr<RSSurfaceRenderNode> surfaceNodeNull = nullptr;
    node->curAllSurfaces_.push_back(surfaceNodeNull);

    auto surfaceNodeDefault = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNodeDefault, nullptr);
    surfaceNodeDefault->nodeType_ = RSSurfaceNodeType::DEFAULT;
    node->curAllSurfaces_.push_back(surfaceNodeDefault);

    auto surfaceNodeMain = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNodeMain, nullptr);
    surfaceNodeMain->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    node->curAllSurfaces_.push_back(surfaceNodeMain);

    rsUniRenderVisitor->AlignGlobalAndSurfaceDirtyRegions(node);
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
    rsUniRenderVisitor->renderEngine_ = std::make_shared<RSUniRenderEngine>();
    rsUniRenderVisitor->renderEngine_->Init();
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
    rsUniRenderVisitor->renderEngine_ = std::make_shared<RSUniRenderEngine>();
    rsUniRenderVisitor->renderEngine_->Init();
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
    rsUniRenderVisitor->renderEngine_ = std::make_shared<RSUniRenderEngine>();
    rsUniRenderVisitor->renderEngine_->Init();
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
    rsUniRenderVisitor->renderEngine_ = std::make_shared<RSUniRenderEngine>();
    rsUniRenderVisitor->renderEngine_->Init();
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
 * @tc.name: GenerateNodeContentCache001
 * @tc.desc: Test RSUniRenderVisitorTest.GenerateNodeContentCache when surfaceNode is null
 * @tc.type: FUNC
 * @tc.require: issueI79KM8
 */
HWTEST_F(RSUniRenderVisitorTest, GenerateNodeContentCache001, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->renderEngine_ = std::make_shared<RSUniRenderEngine>();
    rsUniRenderVisitor->renderEngine_->Init();

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
    rsUniRenderVisitor->renderEngine_ = std::make_shared<RSUniRenderEngine>();
    rsUniRenderVisitor->renderEngine_->Init();

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
    ASSERT_NE(rsUniRenderVisitor->curDisplayNode_, nullptr);
    NodeId id2 = 2;
    auto node = std::make_shared<RSEffectRenderNode>(id2, rsContext->weak_from_this());
    rsUniRenderVisitor->PrepareEffectRenderNode(*node);
}

/**
 * @tc.name: CopyForParallelPrepare001
 * @tc.desc: Test RSUniRenderVisitorTest.CopyForParallelPrepare api
 * @tc.type: FUNC
 * @tc.require: issueI79KM8
 */
HWTEST_F(RSUniRenderVisitorTest, CopyForParallelPrepare001, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsUniRenderVisitorCopy = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitorCopy, nullptr);
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    RSDisplayNodeConfig displayConfig;
    config.id = 10;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(20, displayConfig, rsContext->weak_from_this());
    rsSurfaceRenderNode->SetSrcRect(RectI(0, 0, 10, 10));
    rsDisplayRenderNode->AddChild(rsSurfaceRenderNode, -1);
    ASSERT_NE(rsDisplayRenderNode->GetDirtyManager(), nullptr);
    rsUniRenderVisitorCopy->PrepareDisplayRenderNode(*rsDisplayRenderNode);
    rsUniRenderVisitorCopy->dirtySurfaceNodeMap_.emplace(rsSurfaceRenderNode->GetId(), rsSurfaceRenderNode);
    rsUniRenderVisitor->CopyForParallelPrepare(rsUniRenderVisitorCopy);
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
 * @tc.name: ProcessParallelDisplayRenderNode001
 * @tc.desc: Test RSUniRenderVisitorTest.ProcessParallelDisplayRenderNode api
 * @tc.type: FUNC
 * @tc.require: issueI79KM8
 */
HWTEST_F(RSUniRenderVisitorTest, ProcessParallelDisplayRenderNode001, TestSize.Level1)
{
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(id, config);
    ASSERT_NE(node, nullptr);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->ProcessParallelDisplayRenderNode(*node);
}

/**
 * @tc.name: SetSurfaceGlobalDirtyRegion001
 * @tc.desc: Test RSUniRenderVisitorTest.SetSurfaceGlobalDirtyRegion api
 * @tc.type: FUNC
 * @tc.require: issueI79KM8
 */
HWTEST_F(RSUniRenderVisitorTest, SetSurfaceGlobalDirtyRegion001, TestSize.Level1)
{
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(id, config);
    ASSERT_NE(node, nullptr);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->SetSurfaceGlobalDirtyRegion(node);
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
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->ProcessEffectRenderNode(node);
}

/**
 * @tc.name: PrepareOffscreenRender001
 * @tc.desc: Test RSUniRenderVisitorTest.PrepareOffscreenRender api
 * @tc.type: FUNC
 * @tc.require: issueI79KM8
 */
HWTEST_F(RSUniRenderVisitorTest, PrepareOffscreenRender001, TestSize.Level1)
{
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(id, config);
    ASSERT_NE(node, nullptr);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->PrepareOffscreenRender(*node);
}

/**
 * @tc.name: FinishOffscreenRender001
 * @tc.desc: Test RSUniRenderVisitorTest.FinishOffscreenRender api
 * @tc.type: FUNC
 * @tc.require: issueI79KM8
 */
HWTEST_F(RSUniRenderVisitorTest, FinishOffscreenRender001, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->FinishOffscreenRender(false);

    auto drawingCanvas = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(drawingCanvas, nullptr);
    rsUniRenderVisitor->canvas_ = std::make_unique<RSPaintFilterCanvas>(drawingCanvas.get());
    rsUniRenderVisitor->canvasBackup_ = std::make_unique<RSPaintFilterCanvas>(drawingCanvas.get());
}

/**
 * @tc.name: ParallelRenderEnableHardwareComposer001
 * @tc.desc: Test RSUniRenderVisitorTest.ParallelRenderEnableHardwareComposer api
 * @tc.type: FUNC
 * @tc.require: issueI79KM8
 */
HWTEST_F(RSUniRenderVisitorTest, ParallelRenderEnableHardwareComposer001, TestSize.Level1)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->ParallelRenderEnableHardwareComposer(*surfaceNode);
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
    rsUniRenderVisitor->isParallel_ = false;

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
 * @tc.name: CheckIfSurfaceRenderNodeNeedProcess003
 * @tc.desc: Test RSUniRenderVisitorTest.CheckIfSurfaceRenderNodeNeedProcess for different types of filteredAppSet
 * @tc.type: FUNC
 * @tc.require: issueI8FSLX
 */
HWTEST_F(RSUniRenderVisitorTest, CheckIfSurfaceRenderNodeNeedProcess003, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    ScreenInfo info;

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->screenInfo_ = info;

    bool keepFilterCache;
    // filteredAppSet is empty
    ASSERT_TRUE(rsUniRenderVisitor->CheckIfSurfaceRenderNodeNeedProcess(*surfaceNode, keepFilterCache));
    // filteredAppSet isn't empty and don't contain this surface node's id
    rsUniRenderVisitor->screenInfo_.filteredAppSet.insert(surfaceNode->GetId() + 1);
    ASSERT_FALSE(rsUniRenderVisitor->CheckIfSurfaceRenderNodeNeedProcess(*surfaceNode, keepFilterCache));
    // filteredAppSet isn't empty and contain this surface node's id
    rsUniRenderVisitor->screenInfo_.filteredAppSet.insert(surfaceNode->GetId());
    ASSERT_TRUE(rsUniRenderVisitor->CheckIfSurfaceRenderNodeNeedProcess(*surfaceNode, keepFilterCache));
}

/**
 * @tc.name: PrepareSharedTransitionNode001
 * @tc.desc: Test RSUniRenderVisitorTest.PrepareSharedTransitionNode while paired node is already destroyed
 * @tc.type: FUNC
 * @tc.require: issueI8MVJ6
 */
HWTEST_F(RSUniRenderVisitorTest, PrepareSharedTransitionNode001, TestSize.Level2)
{
    auto node = RSTestUtil::CreateSurfaceNode();
    auto pairedNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(node, nullptr);
    ASSERT_NE(pairedNode, nullptr);

    node->sharedTransitionParam_ = {node->GetId(), pairedNode};
    pairedNode.reset();

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->PrepareSharedTransitionNode(*node);
    ASSERT_EQ(node->GetSharedTransitionParam(), std::nullopt);
}

/**
 * @tc.name: PrepareSharedTransitionNode002
 * @tc.desc: Test RSUniRenderVisitorTest.PrepareSharedTransitionNode while paired node is not a transition node
 * @tc.type: FUNC
 * @tc.require: issueI8MVJ6
 */
HWTEST_F(RSUniRenderVisitorTest, PrepareSharedTransitionNode002, TestSize.Level2)
{
    auto node = RSTestUtil::CreateSurfaceNode();
    auto pairedNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(node, nullptr);
    ASSERT_NE(pairedNode, nullptr);

    node->sharedTransitionParam_ = {node->GetId(), pairedNode};
    pairedNode->SetIsOnTheTree(true);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->PrepareSharedTransitionNode(*node);
    ASSERT_EQ(node->GetSharedTransitionParam(), std::nullopt);
}

/**
 * @tc.name: PrepareSharedTransitionNode003
 * @tc.desc: Test RSUniRenderVisitorTest.PrepareSharedTransitionNode while paired node is not paired with this node
 * @tc.type: FUNC
 * @tc.require: issueI8MVJ6
 */
HWTEST_F(RSUniRenderVisitorTest, PrepareSharedTransitionNode003, TestSize.Level2)
{
    auto node = RSTestUtil::CreateSurfaceNode();
    auto pairedNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(node, nullptr);
    ASSERT_NE(pairedNode, nullptr);

    node->sharedTransitionParam_ = {node->GetId(), pairedNode};
    pairedNode->sharedTransitionParam_ = {pairedNode->GetId(), pairedNode};
    pairedNode->SetIsOnTheTree(true);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->PrepareSharedTransitionNode(*node);
    ASSERT_EQ(node->GetSharedTransitionParam(), std::nullopt);
}

/**
 * @tc.name: PrepareSharedTransitionNode004
 * @tc.desc: Test RSUniRenderVisitorTest.PrepareSharedTransitionNode while transition node match
 * @tc.type: FUNC
 * @tc.require: issueI8MVJ6
 */
HWTEST_F(RSUniRenderVisitorTest, PrepareSharedTransitionNode004, TestSize.Level2)
{
    auto node = RSTestUtil::CreateSurfaceNode();
    auto pairedNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(node, nullptr);
    ASSERT_NE(pairedNode, nullptr);

    node->sharedTransitionParam_ = {node->GetId(), pairedNode};
    pairedNode->sharedTransitionParam_ = {node->GetId(), pairedNode};
    pairedNode->SetIsOnTheTree(true);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->PrepareSharedTransitionNode(*node);
    ASSERT_EQ(rsUniRenderVisitor->prepareClipRect_, RectI(0, 0, INT_MAX, INT_MAX));
}

/**
 * @tc.name: PrepareSharedTransitionNode005
 * @tc.desc: Test mark curSurfaceNode while PrepareSharedTransitionNode
 * @tc.type: FUNC
 * @tc.require: issueI8MVJ6
 */
HWTEST_F(RSUniRenderVisitorTest, PrepareSharedTransitionNode005, TestSize.Level2)
{
    auto node = RSTestUtil::CreateSurfaceNode();
    auto pairedNode = RSTestUtil::CreateSurfaceNode();
    auto curSurfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(node, nullptr);
    ASSERT_NE(pairedNode, nullptr);
    ASSERT_NE(curSurfaceNode, nullptr);

    node->sharedTransitionParam_ = {node->GetId(), pairedNode};
    pairedNode->sharedTransitionParam_ = {node->GetId(), pairedNode};
    pairedNode->SetIsOnTheTree(true);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curSurfaceNode_ = curSurfaceNode;
    rsUniRenderVisitor->PrepareSharedTransitionNode(*node);
    ASSERT_TRUE(curSurfaceNode->GetHasSharedTransitionNode());
}

/**
 * @tc.name: PrepareSharedTransitionNode006
 * @tc.desc: Test mark curSurfaceNode's parent while PrepareSharedTransitionNode
 * @tc.type: FUNC
 * @tc.require: issueI8MVJ6
 */
HWTEST_F(RSUniRenderVisitorTest, PrepareSharedTransitionNode006, TestSize.Level2)
{
    auto node = RSTestUtil::CreateSurfaceNode();
    auto pairedNode = RSTestUtil::CreateSurfaceNode();
    auto curSurfaceNode = RSTestUtil::CreateSurfaceNode();
    auto parentNode = RSTestUtil::CreateSurfaceNode();

    ASSERT_NE(node, nullptr);
    ASSERT_NE(pairedNode, nullptr);
    ASSERT_NE(curSurfaceNode, nullptr);
    ASSERT_NE(parentNode, nullptr);

    node->sharedTransitionParam_ = {node->GetId(), pairedNode};
    pairedNode->sharedTransitionParam_ = {node->GetId(), pairedNode};
    pairedNode->SetIsOnTheTree(true);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curSurfaceNode_ = curSurfaceNode;

    parentNode->AddChild(curSurfaceNode);
    // while curSurfaceNode's parent isn't LEASH_WINDOW_NODE, shouldn't mark it
    rsUniRenderVisitor->PrepareSharedTransitionNode(*node);
    ASSERT_FALSE(parentNode->GetHasSharedTransitionNode());
    // while curSurfaceNode's parent is LEASH_WINDOW_NODE, should mark it
    parentNode->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);
    rsUniRenderVisitor->PrepareSharedTransitionNode(*node);
    ASSERT_TRUE(parentNode->GetHasSharedTransitionNode());
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

/*
 * @tc.name: ResetCurSurfaceInfoAsUpperSurfaceParent001
 * @tc.desc: Reset but keep single node's surfaceInfo
 * @tc.type: FUNC
 * @tc.require: issuesI8MQCS
 */
HWTEST_F(RSUniRenderVisitorTest, ResetCurSurfaceInfoAsUpperSurfaceParent001, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    std::weak_ptr<RSSurfaceRenderNode> upperSurfaceNode;
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetParent(upperSurfaceNode);
    
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;

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

    rsUniRenderVisitor->ResetCurSurfaceInfoAsUpperSurfaceParent(*surfaceNode);
    ASSERT_EQ(rsUniRenderVisitor->curSurfaceNode_, surfaceNode);
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
    surfaceNode->SetIsOnTheTree(true);
    surfaceNode->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;

    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    ASSERT_NE(displayNode, nullptr);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->isQuickSkipPreparationEnabled_ = true;
    rsUniRenderVisitor->curDisplayNode_ = displayNode;
    rsUniRenderVisitor->displayHasSecSurface_[rsUniRenderVisitor->currentVisitDisplay_] = false;
    rsUniRenderVisitor->PrepareSurfaceRenderNode(*surfaceNode);
    ASSERT_EQ(rsUniRenderVisitor->displayHasSecSurface_[rsUniRenderVisitor->currentVisitDisplay_], true);
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
 * @tc.name: ProcessSharedTransitionNode001
 * @tc.desc: Test RSUniRenderVisitorTest.ProcessSharedTransitionNode while
 *           node's sharedTransitionParam isn't empty
 * @tc.type: FUNC
 * @tc.require: issueI7UGLR
 */
HWTEST_F(RSUniRenderVisitorTest, ProcessSharedTransitionNode001, TestSize.Level2)
{
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto transitionNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(node, nullptr);
    ASSERT_NE(transitionNode, nullptr);
    node->sharedTransitionParam_ = {transitionNode->GetId(), transitionNode};
    transitionNode->SetIsOnTheTree(true);
    
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    auto drawingCanvas = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    ASSERT_NE(drawingCanvas, nullptr);
    rsUniRenderVisitor->canvas_ = std::make_unique<RSPaintFilterCanvas>(drawingCanvas.get());

    ASSERT_FALSE(rsUniRenderVisitor->ProcessSharedTransitionNode(*node));
}

/**
 * @tc.name: ProcessSharedTransitionNode002
 * @tc.desc: Test RSUniRenderVisitorTest.ProcessSharedTransitionNode while
 *           render visitor just init node cache
 * @tc.type: FUNC
 * @tc.require: issueI7UGLR
 */
HWTEST_F(RSUniRenderVisitorTest, ProcessSharedTransitionNode002, TestSize.Level2)
{
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto transitionNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(node, nullptr);
    ASSERT_NE(transitionNode, nullptr);
    node->sharedTransitionParam_ = {transitionNode->GetId(), transitionNode};
    node->SetDrawingCacheType(RSDrawingCacheType::FORCED_CACHE);
    transitionNode->SetIsOnTheTree(true);
    
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    auto drawingCanvas = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    ASSERT_NE(drawingCanvas, nullptr);
    rsUniRenderVisitor->canvas_ = std::make_unique<RSPaintFilterCanvas>(drawingCanvas.get());
    rsUniRenderVisitor->InitNodeCache(*node);

    ASSERT_FALSE(rsUniRenderVisitor->ProcessSharedTransitionNode(*node));
}

/**
 * @tc.name: ProcessSharedTransitionNode003
 * @tc.desc: Test RSUniRenderVisitorTest.ProcessSharedTransitionNode while
 *           transition node's GlobalAlpha equal to zero
 * @tc.type: FUNC
 * @tc.require: issueI7UGLR
 */
HWTEST_F(RSUniRenderVisitorTest, ProcessSharedTransitionNode003, TestSize.Level2)
{
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto transitionNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(node, nullptr);
    ASSERT_NE(transitionNode, nullptr);
    node->sharedTransitionParam_ = {transitionNode->GetId(), transitionNode};
    transitionNode->SetGlobalAlpha(0.0f);
    
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    ASSERT_TRUE(rsUniRenderVisitor->ProcessSharedTransitionNode(*node));
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

    ASSERT_NE(drawingCanvas, nullptr);
    rsUniRenderVisitor->canvas_ = std::make_unique<RSPaintFilterCanvas>(drawingCanvas.get());
    rsUniRenderVisitor->DrawCurrentRefreshRate(0, 0);
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
 * @tc.name: GetCacheImageFromMirrorNode001
 * @tc.desc: Test RSUniRenderVisitorTest.GetCacheImageFromMirrorNode while
 *           cacheImage == nullptr
 * @tc.type: FUNC
 * @tc.require: issueI7UGLR
 */
HWTEST_F(RSUniRenderVisitorTest, GetCacheImageFromMirrorNode001, TestSize.Level2)
{
    RSDisplayNodeConfig displayConfig;
    auto rsContext = std::make_shared<RSContext>();
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(20, displayConfig, rsContext->weak_from_this());
    ASSERT_NE(rsDisplayRenderNode, nullptr);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    RSMainThread::Instance()->doParallelComposition_ = true;
    ASSERT_EQ(rsUniRenderVisitor->GetCacheImageFromMirrorNode(rsDisplayRenderNode), nullptr);
}

/**
 * @tc.name: DrawSurfaceLayer001
 * @tc.desc: Test RSUniRenderVisitorTest.DrawSurfaceLayer while
 *           displayNode has child
 * @tc.type: FUNC
 * @tc.require: issueI7UGLR
 */
HWTEST_F(RSUniRenderVisitorTest, DrawSurfaceLayer001, TestSize.Level2)
{
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    ASSERT_NE(displayNode, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    displayNode->AddChild(surfaceNode);

    std::list<std::shared_ptr<RSSurfaceRenderNode>> mainThreadNodes;
    std::list<std::shared_ptr<RSSurfaceRenderNode>> subThreadNodes;
    RSUniRenderUtil::AssignWindowNodes(displayNode, mainThreadNodes, subThreadNodes);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    auto drawingCanvas = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    ASSERT_NE(drawingCanvas, nullptr);
    rsUniRenderVisitor->canvas_ = std::make_unique<RSPaintFilterCanvas>(drawingCanvas.get());
    rsUniRenderVisitor->renderEngine_ = std::make_shared<RSUniRenderEngine>();
    rsUniRenderVisitor->renderEngine_->Init();
    rsUniRenderVisitor->DrawSurfaceLayer(displayNode, subThreadNodes);
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
 * @tc.name: DrawEffectRenderNodeForDFX002
 * @tc.desc: Test RSUniRenderVisitorTest.DrawEffectRenderNodeForDFX while
 *           rect map is not empty.
 * @tc.type: FUNC
 * @tc.require: issueI8WJXC
 */
HWTEST_F(RSUniRenderVisitorTest, DrawEffectRenderNodeForDFX002, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    auto drawingCanvas = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(drawingCanvas, nullptr);
    rsUniRenderVisitor->canvas_ = std::make_unique<RSPaintFilterCanvas>(drawingCanvas.get());

    rsUniRenderVisitor->renderEngine_ = std::make_shared<RSUniRenderEngine>();
    rsUniRenderVisitor->renderEngine_->Init();
 
    NodeId nodeId = 0;
    std::weak_ptr<RSContext> context;
    RSEffectRenderNode rsEffectRenderNode(nodeId, context);
 
    RectI rect1(0, 0, 1, 1);
    rsUniRenderVisitor->nodesUseEffectForDfx_.emplace_back(rect1);
    RectI rect2(2, 2, 3, 3);
    rsUniRenderVisitor->nodesUseEffectFallbackForDfx_.emplace_back(rect2);
 
    rsUniRenderVisitor->effectNodeMapForDfx_[nodeId].first = rsUniRenderVisitor->nodesUseEffectForDfx_;
    rsUniRenderVisitor->effectNodeMapForDfx_[nodeId].second = rsUniRenderVisitor->nodesUseEffectFallbackForDfx_;
 
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

    rsUniRenderVisitor->renderEngine_ = std::make_shared<RSUniRenderEngine>();
    rsUniRenderVisitor->renderEngine_->Init();

    NodeId id = 0;
    auto node = std::make_shared<RSCanvasRenderNode>(id);
    ASSERT_NE(node, nullptr);
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
    auto manager = std::make_shared<RSDirtyRegionManager>();
    NodeId id = 1;
    auto node = std::make_shared<RSSurfaceRenderNode>(id);
    rsUniRenderVisitor->SaveCurSurface(manager, node);
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
    auto manager = std::make_shared<RSDirtyRegionManager>();
    NodeId id = 1;
    auto node = std::make_shared<RSSurfaceRenderNode>(id);
    rsUniRenderVisitor->SaveCurSurface(manager, node);
    rsUniRenderVisitor->RestoreCurSurface(manager, node);
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
    parent->AddChild(child);
    child->SetChildHasFilter(true);
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
    // prepare curAllsurfaces_
    node->curAllSurfaces_.push_back(nullptr);
    auto node2 = RSTestUtil::CreateSurfaceNode();
    node->curAllSurfaces_.push_back(node2);
    auto node3 = RSTestUtil::CreateSurfaceNode();
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
} // OHOS::Rosen
