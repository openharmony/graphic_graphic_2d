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
#include "rs_test_util.h"

using namespace testing;
using namespace testing::ext;

namespace {
    const std::string DEFAULT_NODE_NAME = "1";
    const std::string INVALID_NODE_NAME = "2";
    const int DEFAULT_DIRTY_REGION_WIDTH = 10;
    const int DEFAULT_DIRTY_REGION_HEIGHT = 10;
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
 * @tc.name: PrepareBaseRenderNode001
 * @tc.desc: PrepareBaseRenderNode Test
 * @tc.type: FUNC
 * @tc.require: issueI79U8E
 */
HWTEST_F(RSUniRenderVisitorTest, PrepareBaseRenderNode001, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    auto rsBaseRenderNode = std::make_shared<RSBaseRenderNode>(10, rsContext->weak_from_this());
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->PrepareBaseRenderNode(*rsBaseRenderNode);
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
    int defaultParam = (int)RSSystemProperties::GetQuickSkipPrepareEnabled();
    (void)system::SetParameter("rosen.quickskipprepare.enabled", "0");
    int param = (int)RSSystemProperties::GetQuickSkipPrepareEnabled();
    ASSERT_EQ(param, 0);
    (void)system::SetParameter("rosen.quickskipprepare.enabled", "1");
    param = (int)RSSystemProperties::GetQuickSkipPrepareEnabled();
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
    int defaultParam = (int)RSSystemProperties::GetQuickSkipPrepareEnabled();
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
    int defaultParam = (int)RSSystemProperties::GetQuickSkipPrepareEnabled();
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
 * @tc.name: ParallelCompositionTest
 * @tc.desc: Test RSUniRenderVisitorTest.ParallelCompositionTest
 * @tc.type: FUNC
 * @tc.require: AR000HQ6GH
 */
HWTEST_F(RSUniRenderVisitorTest, ParallelCompositionTest, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    auto rsBaseRenderNode = std::make_shared<RSBaseRenderNode>(10, rsContext->weak_from_this());
    RSDisplayNodeConfig displayConfig1 = {
        .screenId = 0,
        .isMirrored = false,
        .mirrorNodeId = 0,
    };
    auto rsDisplayRenderNode1 = std::make_shared<RSDisplayRenderNode>(20, displayConfig1, rsContext->weak_from_this());
    rsBaseRenderNode->AddChild(rsDisplayRenderNode1);
    RSDisplayNodeConfig displayConfig2 = {
        .screenId = 1,
        .isMirrored = true,
        .mirrorNodeId = 20,
    };
    auto rsDisplayRenderNode2 = std::make_shared<RSDisplayRenderNode>(30, displayConfig2, rsContext->weak_from_this());
    rsBaseRenderNode->AddChild(rsDisplayRenderNode2);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    bool ret = rsUniRenderVisitor->ParallelComposition(rsBaseRenderNode);
    ASSERT_EQ(ret, true);
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
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    config.id = 10;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    rsSurfaceRenderNode->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_NODE;
    rsSurfaceRenderNode->name_ = "";
    rsUniRenderVisitor->MarkSubHardwareEnableNodeState(*rsSurfaceRenderNode);
    ASSERT_EQ(true, rsSurfaceRenderNode->IsHardwareEnabledType());
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
 * @tc.desc: Test RSUniRenderVisitorTest.DrawAllSurfaceOpaqueRegionForDFX when nodetype is RSSurfaceNodeType::SELF_DRAWING_NODE
 * @tc.type: FUNC
 * @tc.require: issueI79KM8
 */
HWTEST_F(RSUniRenderVisitorTest, DrawAllSurfaceOpaqueRegionForDFX002, TestSize.Level1)
{
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(id, config);
    auto surfaceNodeMain = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNodeMain, nullptr);
    surfaceNodeMain->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    node->curAllSurfaces_.push_back(surfaceNodeMain);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
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
    auto surfaceNodeNull = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNodeNull, nullptr);
    surfaceNodeNull = nullptr;
    node->curAllSurfaces_.push_back(surfaceNodeNull);
    auto surfaceNodeNotApp = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNodeNotApp, nullptr);
    surfaceNodeNotApp->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_NODE;
    node->curAllSurfaces_.push_back(surfaceNodeNotApp);
    auto surfaceNodeMain = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNodeMain, nullptr);
    surfaceNodeMain->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    surfaceNodeMain->name_ = DEFAULT_NODE_NAME;
    node->curAllSurfaces_.push_back(surfaceNodeMain);
    auto surfaceNodeMain2 = RSTestUtil::CreateSurfaceNodeWithBuffer();
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
    auto surfaceNodeMain = RSTestUtil::CreateSurfaceNodeWithBuffer();
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
    auto surfaceNodeMain = RSTestUtil::CreateSurfaceNodeWithBuffer();
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
    auto surfaceNodeMain = RSTestUtil::CreateSurfaceNodeWithBuffer();
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
    auto surfaceNodeMain = RSTestUtil::CreateSurfaceNodeWithBuffer();
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
    auto surfaceNodeMain = RSTestUtil::CreateSurfaceNodeWithBuffer();
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
    auto surfaceNodeMain = RSTestUtil::CreateSurfaceNodeWithBuffer();
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
    auto surfaceNodeMain = RSTestUtil::CreateSurfaceNodeWithBuffer();
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
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
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
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
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
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
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
    auto surfaceNodeNotApp = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNodeNotApp, nullptr);
    surfaceNodeNotApp->nodeType_ = RSSurfaceNodeType::ABILITY_COMPONENT_NODE;
    node->curAllSurfaces_.push_back(surfaceNodeNotApp);
    auto surfaceNodeAppWithEmptyRegion = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNodeAppWithEmptyRegion, nullptr);
    surfaceNodeAppWithEmptyRegion->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    ASSERT_NE(surfaceNodeAppWithEmptyRegion->GetDirtyManager(), nullptr);
    surfaceNodeAppWithEmptyRegion->GetDirtyManager()->dirtyRegion_ = RectI(0, 0, 0, 0);
    node->curAllSurfaces_.push_back(surfaceNodeAppWithEmptyRegion);
    auto surfaceNodeApp = RSTestUtil::CreateSurfaceNodeWithBuffer();
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
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    auto sfDirtyManager = surfaceNode->GetDirtyManager();
    ASSERT_NE(sfDirtyManager, nullptr);
    sfDirtyManager->dirtyCanvasNodeInfo_.resize(DirtyRegionType::TYPE_AMOUNT);
    sfDirtyManager->dirtySurfaceNodeInfo_.resize(DirtyRegionType::TYPE_AMOUNT);
    RectI rect(0, 0, DEFAULT_DIRTY_REGION_WIDTH, DEFAULT_DIRTY_REGION_HEIGHT);
    sfDirtyManager->dirtyRegion_ = RectI(0, 0, DEFAULT_DIRTY_REGION_WIDTH, DEFAULT_DIRTY_REGION_HEIGHT);
    dpDirtyManager->dirtyRegion_ = RectI(0, 0, 0, 0);
    auto surfaceNodeApp = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNodeApp, nullptr);
    surfaceNodeApp->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    ASSERT_NE(surfaceNodeApp->GetDirtyManager(), nullptr);
    surfaceNodeApp->GetDirtyManager()->dirtyRegion_ =
        RectI(0, 0, DEFAULT_DIRTY_REGION_WIDTH, DEFAULT_DIRTY_REGION_HEIGHT);
    surfaceNodeApp->visibleRegion_ = Occlusion::Region(RectI(0, 0, 0, 0));
    node->curAllSurfaces_.push_back(surfaceNodeApp);

    auto surfaceNodeAppOutOfRange = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNodeAppOutOfRange, nullptr);
    surfaceNodeAppOutOfRange->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    ASSERT_NE(surfaceNodeAppOutOfRange->GetDirtyManager(), nullptr);
    surfaceNodeAppOutOfRange->GetDirtyManager()->dirtyRegion_ = RectI(DEFAULT_DIRTY_REGION_WIDTH,
        DEFAULT_DIRTY_REGION_HEIGHT, DEFAULT_DIRTY_REGION_WIDTH, DEFAULT_DIRTY_REGION_HEIGHT);
    surfaceNodeAppOutOfRange->visibleRegion_ =
        Occlusion::Region(RectI(0, 0, DEFAULT_DIRTY_REGION_WIDTH, DEFAULT_DIRTY_REGION_HEIGHT));
    node->curAllSurfaces_.push_back(surfaceNodeAppOutOfRange);
    auto dRect = surfaceNodeAppOutOfRange->GetDirtyManager()->GetDirtyRegion();

    auto surfaceNodeAppIntersect = RSTestUtil::CreateSurfaceNodeWithBuffer();
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
} // OHOS::Rosen