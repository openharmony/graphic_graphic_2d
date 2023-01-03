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

using namespace testing;
using namespace testing::ext;

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

HWTEST_F(RSUniRenderVisitorTest, ProcessSurfaceRenderNode001, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    RSDisplayNodeConfig displayConfig;
    config.id = 10;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(11, displayConfig, rsContext->weak_from_this());
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();

    rsSurfaceRenderNode->SetSrcRect(RectI(0, 0, 10, 10));
    rsSurfaceRenderNode->SetAppFreeze(false);
    rsSurfaceRenderNode->SetSecurityLayer(true);
    rsDisplayRenderNode->AddChild(rsSurfaceRenderNode, -1);

    rsUniRenderVisitor->PrepareDisplayRenderNode(*rsDisplayRenderNode);
    rsUniRenderVisitor->ProcessDisplayRenderNode(*rsDisplayRenderNode);
    rsUniRenderVisitor->ProcessSurfaceRenderNode(*rsSurfaceRenderNode);

    rsUniRenderVisitor->SetAnimateState(true);
    rsUniRenderVisitor->PrepareDisplayRenderNode(*rsDisplayRenderNode);
    rsUniRenderVisitor->ProcessDisplayRenderNode(*rsDisplayRenderNode);
    rsUniRenderVisitor->ProcessSurfaceRenderNode(*rsSurfaceRenderNode);

    auto& propertyD = rsDisplayRenderNode->GetMutableRenderProperties();
    propertyD.SetVisible(false);
    auto& propertyS = rsSurfaceRenderNode->GetMutableRenderProperties();
    propertyS.SetVisible(false);
    rsUniRenderVisitor->PrepareDisplayRenderNode(*rsDisplayRenderNode);
    rsUniRenderVisitor->ProcessDisplayRenderNode(*rsDisplayRenderNode);
    rsUniRenderVisitor->ProcessSurfaceRenderNode(*rsSurfaceRenderNode);

    // SetSecurityDisplay for rsDisplayRenderNode
    rsDisplayRenderNode->SetSecurityDisplay(true);
    rsUniRenderVisitor->PrepareDisplayRenderNode(*rsDisplayRenderNode);
    rsUniRenderVisitor->ProcessDisplayRenderNode(*rsDisplayRenderNode);
    rsUniRenderVisitor->ProcessSurfaceRenderNode(*rsSurfaceRenderNode);
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

HWTEST_F(RSUniRenderVisitorTest, PrepareCavasRenderNode001, TestSize.Level1)
{
    system::SetParameter("rosen.dirtyregiondebug.enabled", "6");
    int param = (int)RSSystemProperties::GetDirtyRegionDebugType();
    ASSERT_EQ(param, 6);
    constexpr NodeId nodeId = 1;
    auto rsContext = std::make_shared<RSContext>();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    auto node = std::make_shared<RSCanvasRenderNode>(nodeId, rsContext->weak_from_this());
    RSSurfaceRenderNodeConfig config;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    rsSurfaceRenderNode->SetSurfaceNodeType(RSSurfaceNodeType::SELF_DRAWING_NODE);
    rsSurfaceRenderNode->AddChild(node, -1);
    rsUniRenderVisitor->ProcessSurfaceRenderNode(*rsSurfaceRenderNode);
    node->Prepare(rsUniRenderVisitor);
    node->Process(rsUniRenderVisitor);

    auto& property = node->GetMutableRenderProperties();
    property.SetVisible(false);
    node->Prepare(rsUniRenderVisitor);
    node->Process(rsUniRenderVisitor);

    std::shared_ptr<RSFilter> bgFilter = RSFilter::CreateBlurFilter(5.0f, 5.0f);
    property.SetBackgroundFilter(bgFilter);
}

HWTEST_F(RSUniRenderVisitorTest, PrepareCavasRenderNode002, TestSize.Level1)
{
    system::SetParameter("rosen.uni.partialrender.enabled", "0");
    int param = (int)RSSystemProperties::GetDirtyRegionDebugType();
    ASSERT_EQ(param, 0);

    system::SetParameter("rosen.dirtyregiondebug.surfacenames", "1");
    std::vector<std::string> dfxTargetSurfaceNames;
    bool isDirtyRegionDfxEnabled = (int)RSSystemProperties::GetTargetDirtyRegionDfxEnabled(dfxTargetSurfaceNames);
    ASSERT_EQ(isDirtyRegionDfxEnabled, true);

    constexpr NodeId nodeId = 2;
    auto rsContext = std::make_shared<RSContext>();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    auto node = std::make_shared<RSCanvasRenderNode>(nodeId, rsContext->weak_from_this());
    RSSurfaceRenderNodeConfig config;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    rsSurfaceRenderNode->SetSurfaceNodeType(RSSurfaceNodeType::SELF_DRAWING_NODE);
    rsSurfaceRenderNode->AddChild(node, -1);
    rsUniRenderVisitor->ProcessSurfaceRenderNode(*rsSurfaceRenderNode);

    node->Prepare(rsUniRenderVisitor);
    node->Process(rsUniRenderVisitor);
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

} // OHOS::Rosen