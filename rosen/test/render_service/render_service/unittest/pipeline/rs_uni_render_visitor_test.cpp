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
#include "rs_test_util.h"
#include "system/rs_system_parameters.h"
#include "draw/color.h"

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
};

void RSUniRenderVisitorTest::SetUpTestCase() {}
void RSUniRenderVisitorTest::TearDownTestCase() {}
void RSUniRenderVisitorTest::SetUp() {}
void RSUniRenderVisitorTest::TearDown()
{
    system::SetParameter("rosen.dirtyregiondebug.enabled", "0");
    system::SetParameter("rosen.uni.partialrender.enabled", "4");
    system::GetParameter("rosen.dirtyregiondebug.surfacenames", "0");
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
    std::map<uint32_t, bool> pidVisMap;

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
    std::map<uint32_t, bool> pidVisMap;
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
    rsUniRenderVisitor->isHardwareForcedDisabled_ = true;
    rsUniRenderVisitor->doAnimate_ = false;
    rsUniRenderVisitor->isHardwareComposerEnabled_ = true;
    rsUniRenderVisitor->ClearTransparentBeforeSaveLayer();
    ASSERT_EQ(false, rsUniRenderVisitor->IsHardwareComposerEnabled());
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
    dirtyManager->dirtyCanvasNodeInfo_.resize(DirtyRegionType::TYPE_AMOUNT);
    dirtyManager->dirtySurfaceNodeInfo_.resize(DirtyRegionType::TYPE_AMOUNT);
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
    auto skCanvas = std::make_shared<SkCanvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(skCanvas, nullptr);
    rsUniRenderVisitor->canvas_ = std::make_unique<RSPaintFilterCanvas>(skCanvas.get());
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
    auto skCanvas = std::make_shared<SkCanvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(skCanvas, nullptr);
    rsUniRenderVisitor->canvas_ = std::make_unique<RSPaintFilterCanvas>(skCanvas.get());
    surfaceNode->SetParent(node);
    rsUniRenderVisitor->ProcessSurfaceRenderNode(*surfaceNode);
    rsUniRenderVisitor->isSubThread_ = false;
    rsUniRenderVisitor->ProcessSurfaceRenderNode(*surfaceNode);
    rsUniRenderVisitor->isUIFirst_ = false;
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
    NodeId id = 0;
    std::weak_ptr<RSContext> context;
    std::shared_ptr<RSEffectRenderNode> node = std::make_shared<RSEffectRenderNode>(id, context);
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
    rsUniRenderVisitor->CopyForParallelPrepare(rsUniRenderVisitorCopy);
}

#ifdef USE_ROSEN_DRAWING
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
    RSPaintStyle fillType = STROKE;
    rsUniRenderVisitor->DrawDirtyRectForDFX(rect, color, fillType, 0, 0);
}
#endif

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

    auto skCanvas = std::make_shared<SkCanvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(skCanvas, nullptr);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->canvas_ = std::make_unique<RSPaintFilterCanvas>(skCanvas.get());
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
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(id, config);
    ASSERT_NE(node, nullptr);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->UpdateCacheRenderNodeMap(*node);
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
 * @tc.name: RecordAppWindowNodeAndPostTask001
 * @tc.desc: Test RSUniRenderVisitorTest.RecordAppWindowNodeAndPostTask api
 * @tc.type: FUNC
 * @tc.require: issueI79KM8
 */
HWTEST_F(RSUniRenderVisitorTest, RecordAppWindowNodeAndPostTask001, TestSize.Level1)
{
    auto skCanvas = std::make_shared<SkCanvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(skCanvas, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->canvas_ = std::make_unique<RSPaintFilterCanvas>(skCanvas.get());
    rsUniRenderVisitor->RecordAppWindowNodeAndPostTask(*surfaceNode, 0, 0);
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
    rsUniRenderVisitor->FinishOffscreenRender();
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
    appWindowNode->SetColorSpace(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_ADOBE_RGB);
    
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->colorGamutModes_.push_back(
        static_cast<ScreenColorGamut>(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_ADOBE_RGB));
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
    rsUniRenderVisitor->isHardwareComposerEnabled_ = false;
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
    rsUniRenderVisitor->isHardwareComposerEnabled_ = true;
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
    rsUniRenderVisitor->isHardwareComposerEnabled_ = true;
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
    rsUniRenderVisitor->isHardwareComposerEnabled_ = true;
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
    rsUniRenderVisitor->isHardwareComposerEnabled_ = true;
    rsUniRenderVisitor->isParallel_ = false;

    rsUniRenderVisitor->AdjustLocalZOrder(appWindowNode);
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
    surfaceNode->GetContainerRegion().GetRegionRects().push_back(rect);
    surfaceNode->GetDirtyManager()->MergeDirtyRect(RectI(0, 0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT));
    
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->AddContainerDirtyToGlobalDirty(displayNode);
    ASSERT_FALSE(displayNode->GetDirtyManager()->GetCurrentFrameDirtyRegion().IsEmpty());
}

/**
 * @tc.name: CheckIfSurfaceRenderNodeNeedProcess001
 * @tc.desc: Test RSUniRenderVisitorTest.CheckIfSurfaceRenderNodeNeedProcess for security layer
 * @tc.type: FUNC
 * @tc.require: issuesI7RNL4
 */
HWTEST_F(RSUniRenderVisitorTest, CheckIfSurfaceRenderNodeNeedProcess001, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetSecurityLayer(true);
    
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->isSecurityDisplay_ = true;
    ASSERT_FALSE(rsUniRenderVisitor->CheckIfSurfaceRenderNodeNeedProcess(*surfaceNode));
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
    ASSERT_FALSE(rsUniRenderVisitor->CheckIfSurfaceRenderNodeNeedProcess(*abilityComponentNode));
}

/**
 * @tc.name: PrepareSharedTransitionNode001
 * @tc.desc: Test RSUniRenderVisitorTest.PrepareSharedTransitionNode for unpaired transition nodes
 * @tc.type: FUNC
 * @tc.require: issuesI7RNL4
 */
HWTEST_F(RSUniRenderVisitorTest, PrepareSharedTransitionNode001, TestSize.Level2)
{
    auto node = RSTestUtil::CreateSurfaceNode();
    auto transitionNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(node, nullptr);
    ASSERT_NE(transitionNode, nullptr);
    node->sharedTransitionParam_ = {node->GetId(), transitionNode};
    
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    auto skCanvas = std::make_shared<SkCanvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    ASSERT_NE(skCanvas, nullptr);
    rsUniRenderVisitor->canvas_ = std::make_unique<RSPaintFilterCanvas>(skCanvas.get());

    using RenderParam = std::tuple<std::shared_ptr<RSRenderNode>, float, std::optional<SkMatrix>>;
    RenderParam val { node->ReinterpretCastTo<RSRenderNode>(),
        rsUniRenderVisitor->canvas_->GetAlpha(), rsUniRenderVisitor->canvas_->getTotalMatrix() };
    rsUniRenderVisitor->unpairedTransitionNodes_[node->GetId()] = val;
    
    ASSERT_TRUE(rsUniRenderVisitor->PrepareSharedTransitionNode(*node));
}

/**
 * @tc.name: PrepareSharedTransitionNode002
 * @tc.desc: Test RSUniRenderVisitorTest.PrepareSharedTransitionNode while
 *           transition node ptr is nullptr
 * @tc.type: FUNC
 * @tc.require: issuesI7RNL4
 */
HWTEST_F(RSUniRenderVisitorTest, PrepareSharedTransitionNode002, TestSize.Level2)
{
    auto node = RSTestUtil::CreateSurfaceNode();
    auto destroyedNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(node, nullptr);
    ASSERT_NE(destroyedNode, nullptr);
    node->sharedTransitionParam_ = { node->GetId(), destroyedNode };
    destroyedNode.reset();

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    ASSERT_TRUE(rsUniRenderVisitor->PrepareSharedTransitionNode(*node));
    ASSERT_EQ(node->sharedTransitionParam_, std::nullopt);
}

/**
 * @tc.name: PrepareSharedTransitionNode003
 * @tc.desc: Test RSUniRenderVisitorTest.PrepareSharedTransitionNode while
 *           transition node's transition param is null
 * @tc.type: FUNC
 * @tc.require: issuesI7RNL4
 */
HWTEST_F(RSUniRenderVisitorTest, PrepareSharedTransitionNode003, TestSize.Level2)
{
    auto node = RSTestUtil::CreateSurfaceNode();
    auto transitionNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(node, nullptr);
    ASSERT_NE(transitionNode, nullptr);
    node->sharedTransitionParam_ = {node->GetId(), transitionNode};
    transitionNode->sharedTransitionParam_ = std::nullopt;

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->PrepareSharedTransitionNode(*node);
    ASSERT_EQ(node->sharedTransitionParam_, std::nullopt);
}

/**
 * @tc.name: PrepareSharedTransitionNode004
 * @tc.desc: Test RSUniRenderVisitorTest.PrepareSharedTransitionNode while transition node don't match
 * @tc.type: FUNC
 * @tc.require: issuesI7RNL4
 */
HWTEST_F(RSUniRenderVisitorTest, PrepareSharedTransitionNode004, TestSize.Level2)
{
    auto node = RSTestUtil::CreateSurfaceNode();
    auto transitionNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(node, nullptr);
    ASSERT_NE(transitionNode, nullptr);
    node->sharedTransitionParam_ = {node->GetId(), transitionNode};
    transitionNode->sharedTransitionParam_ = {transitionNode->GetId(), transitionNode};

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->PrepareSharedTransitionNode(*node);
    ASSERT_EQ(node->sharedTransitionParam_, std::nullopt);
}

/**
 * @tc.name: PrepareSharedTransitionNode005
 * @tc.desc: Test RSUniRenderVisitorTest.PrepareSharedTransitionNode while all sanity checks passed
 * @tc.type: FUNC
 * @tc.require: issuesI7RNL4
 */
HWTEST_F(RSUniRenderVisitorTest, PrepareSharedTransitionNode005, TestSize.Level2)
{
    auto node = RSTestUtil::CreateSurfaceNode();
    auto transitionNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(node, nullptr);
    ASSERT_NE(transitionNode, nullptr);
    node->sharedTransitionParam_ = {node->GetId(), transitionNode};
    transitionNode->sharedTransitionParam_ = {node->GetId(), transitionNode};
    
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    ASSERT_FALSE(rsUniRenderVisitor->PrepareSharedTransitionNode(*node));
}
} // OHOS::Rosen