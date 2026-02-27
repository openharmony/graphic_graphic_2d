/*
 * Copyright (c) 2022-2026 Huawei Device Co., Ltd.
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

#if defined(RS_ENABLE_UNI_RENDER)
#include <memory>

#include "common/rs_common_hook.h"
#include "gtest/gtest.h"
#include "limit_number.h"
#include "pipeline/mock/mock_matrix.h"
#include "pipeline/rs_test_util.h"
#include "system/rs_system_parameters.h"

#include "consumer_surface.h"
#include "draw/color.h"
#include "drawable/rs_color_picker_drawable.h"
#include "drawable/rs_screen_render_node_drawable.h"
#include "drawable/rs_surface_render_node_drawable.h"
#include "modifier_ng/appearance/rs_behind_window_filter_render_modifier.h"
#include "monitor/self_drawing_node_monitor.h"
#include "pipeline/hardware_thread/rs_realtime_refresh_rate_manager.h"
#include "pipeline/render_thread/rs_uni_render_engine.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "pipeline/render_thread/rs_uni_render_util.h"
#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_screen_render_node.h"
#include "pipeline/rs_effect_render_node.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/rs_processor_factory.h"
#include "pipeline/rs_proxy_render_node.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_render_thread.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "pipeline/rs_union_render_node.h"
#include "pipeline/mock/mock_rs_luminance_control.h"
#include "pipeline/main_thread/rs_uni_render_visitor.h"
#include "pipeline/hwc/rs_uni_hwc_visitor.h"
#include "property/rs_point_light_manager.h"
#include "screen_manager/rs_screen.h"
#include "feature/occlusion_culling/rs_occlusion_handler.h"
#include "feature/opinc/rs_opinc_manager.h"
#include "feature/round_corner_display/rs_round_corner_display.h"
#include "feature/round_corner_display/rs_round_corner_display_manager.h"
#include "feature/uifirst/rs_uifirst_manager.h"
#include "feature/window_keyframe/rs_window_keyframe_render_node.h"
#include "feature_cfg/graphic_feature_param_manager.h"
#include "gmock/gmock.h"
#include "surface_buffer_impl.h"

using namespace testing;
using namespace testing::ext;

namespace {
    constexpr uint32_t DEFAULT_CANVAS_WIDTH = 800;
    constexpr uint32_t DEFAULT_CANVAS_HEIGHT = 600;
    const OHOS::Rosen::RectI DEFAULT_RECT = {0, 80, 1000, 1000};
    const OHOS::Rosen::RectI DEFAULT_FILTER_RECT = {0, 0, 500, 500};
    constexpr int MAX_ALPHA = 255;
    constexpr int SCREEN_WIDTH = 3120;
    constexpr int SCREEN_HEIGHT = 1080;
    constexpr OHOS::Rosen::NodeId DEFAULT_NODE_ID = 100;
    const OHOS::Rosen::RectI DEFAULT_SCREEN_RECT = {0, 0, 1000, 1000};
}

namespace OHOS::Rosen {
class RSChildrenDrawableAdapter : public RSDrawable {
public:
    RSChildrenDrawableAdapter() = default;
    ~RSChildrenDrawableAdapter() override = default;
    bool OnUpdate(const RSRenderNode& content) override { return true; }
    void OnSync() override {}
    void OnDraw(Drawing::Canvas* canvas, const Drawing::Rect* rect) const override {}

private:
    bool OnSharedTransition(const std::shared_ptr<RSRenderNode>& node) { return true; }
    friend class RSRenderNode;
};

class RSUniRenderVisitorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline Mock::MatrixMock* matrixMock_;
    ScreenId CreateVirtualScreen(sptr<RSScreenManager> screenManager);
    static void InitTestSurfaceNodeAndScreenInfo(std::shared_ptr<RSSurfaceRenderNode>& surfaceNode,
        std::shared_ptr<RSUniRenderVisitor>& rSUniRenderVisitor);
    std::shared_ptr<RSUniRenderVisitor> InitRSUniRenderVisitor();
};

class MockRSSurfaceRenderNode : public RSSurfaceRenderNode {
public:
    explicit MockRSSurfaceRenderNode(NodeId id,
        const std::weak_ptr<RSContext>& context = {}, bool isTextureExportNode = false)
        : RSSurfaceRenderNode(id, context, isTextureExportNode) {}
    ~MockRSSurfaceRenderNode() override {}
    MOCK_METHOD1(CheckParticipateInOcclusion, bool(bool));
    MOCK_CONST_METHOD0(NeedDrawBehindWindow, bool());
    MOCK_CONST_METHOD0(GetFilterRect, RectI());
    MOCK_METHOD0(CheckIfOcclusionChanged, bool());
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

ScreenId RSUniRenderVisitorTest::CreateVirtualScreen(sptr<RSScreenManager> screenManager)
{
    if (screenManager == nullptr) {
        RS_LOGE("screen manager is nullptr");
        return INVALID_SCREEN_ID;
    }
    std::string name = "virtualScreen";
    uint32_t width = 480;
    uint32_t height = 320;

    auto csurface = IConsumerSurface::Create();
    if (csurface == nullptr) {
        RS_LOGE("consumer surface is nullptr");
        return INVALID_SCREEN_ID;
    }
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    if (psurface == nullptr) {
        RS_LOGE("producer surface is nullptr");
        return INVALID_SCREEN_ID;
    }
    std::vector<NodeId> whiteList = {1};

    auto screenId = screenManager->CreateVirtualScreen(
        name, width, height, psurface, INVALID_SCREEN_ID, -1, whiteList);
    if (screenId == INVALID_SCREEN_ID) {
        RS_LOGE("create virtual screen failed");
        return INVALID_SCREEN_ID;
    }
    return screenId;
}

void RSUniRenderVisitorTest::InitTestSurfaceNodeAndScreenInfo(
    std::shared_ptr<RSSurfaceRenderNode>& surfaceNode,
    std::shared_ptr<RSUniRenderVisitor>& rsUniRenderVisitor)
{
    surfaceNode->SetNodeHasBackgroundColorAlpha(true);
    surfaceNode->SetHardwareEnableHint(true);
    surfaceNode->SetDstRect({10, 1, 100, 100});
    surfaceNode->SetIsOntheTreeOnlyFlag(true);
    Occlusion::Region region1({100, 50, 1000, 1500});
    surfaceNode->SetVisibleRegion(region1);

    NodeId screenNodeId = 10;
    rsUniRenderVisitor->curScreenNode_ = std::make_shared<RSScreenRenderNode>(screenNodeId, 0);

    ScreenInfo screenInfo;
    screenInfo.width = SCREEN_WIDTH;
    screenInfo.height = SCREEN_HEIGHT;
    rsUniRenderVisitor->curScreenNode_->screenInfo_ = screenInfo;
}

std::shared_ptr<RSUniRenderVisitor> RSUniRenderVisitorTest::InitRSUniRenderVisitor()
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ScreenId screenId = 1;
    auto rsContext = std::make_shared<RSContext>();
    rsUniRenderVisitor->curScreenNode_ = std::make_shared<RSScreenRenderNode>(DEFAULT_NODE_ID, screenId, rsContext);
    NodeId id = 0;
    RSDisplayNodeConfig displayConfig;
    auto displayNode = std::make_shared<RSLogicalDisplayRenderNode>(id, displayConfig);
    rsUniRenderVisitor->curLogicalDisplayNode_ = displayNode;
    rsUniRenderVisitor->curScreenNode_->isFirstVisitCrossNodeDisplay_ = true;
    rsUniRenderVisitor->needRecalculateOcclusion_ = false;
    return rsUniRenderVisitor;
}

/**
 * @tc.name: ProcessFilterNodeObscured
 * @tc.desc: Test ProcessFilterNodeObscured
 * @tc.type: FUNC
 * @tc.require: issueIB787L
 */
HWTEST_F(RSUniRenderVisitorTest, ProcessFilterNodeObscured, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    std::shared_ptr<RSFilter> filter = RSFilter::CreateBlurFilter(1.0f, 1.0f);
    ASSERT_NE(filter, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    NodeId id = 1;
    ScreenId screenId = 0;
    auto rsContext = std::make_shared<RSContext>();
    rsUniRenderVisitor->curScreenNode_ = std::make_shared<RSScreenRenderNode>(id, screenId, rsContext);
    ASSERT_NE(rsUniRenderVisitor->curScreenNode_, nullptr);
    surfaceNode->renderProperties_.backgroundFilter_ = filter;
    Occlusion::Region extendRegion;
    Occlusion::Region region{ Occlusion::Rect{ 0, 0, 100, 100 } };
    surfaceNode->SetVisibleRegion(region);
    surfaceNode->GetDirtyManager()->SetCurrentFrameDirtyRect({ 0, 0, 100, 100 });
    auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();

    auto filterNode1 = std::make_shared<RSCanvasRenderNode>(++id);
    auto filterNode2 = std::make_shared<RSCanvasRenderNode>(++id);
    auto filterNode3 = std::make_shared<RSCanvasRenderNode>(++id);
    auto filterNode4 = std::make_shared<RSCanvasRenderNode>(++id);
    ASSERT_NE(filterNode1, nullptr);
    ASSERT_NE(filterNode2, nullptr);
    ASSERT_NE(filterNode3, nullptr);
    ASSERT_NE(filterNode4, nullptr);
    filterNode1->absDrawRect_ = DEFAULT_FILTER_RECT;
    filterNode3->SetOldDirtyInSurface({ 200, 200, 100, 100 });
    filterNode4->SetOldDirtyInSurface({ 0, 0, 300, 300 });
    nodeMap.renderNodeMap_.clear();
    nodeMap.RegisterRenderNode(filterNode1);
    nodeMap.RegisterRenderNode(filterNode3);
    nodeMap.RegisterRenderNode(filterNode4);
    filterNode1->renderProperties_.backgroundFilter_ = filter;
    filterNode3->renderProperties_.backgroundFilter_ = filter;
    filterNode4->renderProperties_.backgroundFilter_ = filter;
    surfaceNode->visibleFilterChild_.push_back(filterNode1->GetId());
    surfaceNode->visibleFilterChild_.push_back(filterNode2->GetId());
    surfaceNode->visibleFilterChild_.push_back(filterNode3->GetId());
    surfaceNode->visibleFilterChild_.push_back(filterNode4->GetId());
    surfaceNode->SetAbilityBGAlpha(MAX_ALPHA);

    ASSERT_FALSE(surfaceNode->IsTransparent());
    rsUniRenderVisitor->ProcessFilterNodeObscured(surfaceNode, extendRegion, nodeMap);
    nodeMap.UnregisterRenderNode(filterNode1->GetId());
    nodeMap.UnregisterRenderNode(filterNode3->GetId());
    nodeMap.UnregisterRenderNode(filterNode4->GetId());
}

/**
 * @tc.name: ProcessFilterNodeObscured002
 * @tc.desc: Test ProcessFilterNodeObscured, childNode not use effect
 * @tc.type: FUNC
 * @tc.require: issue20843
 */
HWTEST_F(RSUniRenderVisitorTest, ProcessFilterNodeObscured002, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    std::shared_ptr<RSFilter> filter = RSFilter::CreateBlurFilter(1.0f, 1.0f);
    ASSERT_NE(filter, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    NodeId id = 1;
    Occlusion::Region extendRegion;
    auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();

    auto filterNode1 = std::make_shared<RSEffectRenderNode>(++id);
    auto filterNode2 = std::make_shared<RSEffectRenderNode>(++id);
    auto filterNode3 = std::make_shared<RSCanvasRenderNode>(++id);
    ASSERT_NE(filterNode1, nullptr);
    ASSERT_NE(filterNode2, nullptr);
    ASSERT_NE(filterNode3, nullptr);
    nodeMap.renderNodeMap_.clear();
    nodeMap.RegisterRenderNode(filterNode1);
    nodeMap.RegisterRenderNode(filterNode2);
    nodeMap.RegisterRenderNode(filterNode3);
    filterNode1->visibleEffectChild_.clear();
    filterNode2->visibleEffectChild_.emplace(id);
    filterNode1->renderProperties_.backgroundFilter_ = filter;
    filterNode2->renderProperties_.backgroundFilter_ = filter;
    filterNode3->renderProperties_.backgroundFilter_ = filter;
    surfaceNode->visibleFilterChild_.push_back(filterNode1->GetId());
    surfaceNode->visibleFilterChild_.push_back(filterNode2->GetId());
    surfaceNode->visibleFilterChild_.push_back(filterNode3->GetId());
    surfaceNode->SetAbilityBGAlpha(MAX_ALPHA);

    rsUniRenderVisitor->ProcessFilterNodeObscured(surfaceNode, extendRegion, nodeMap);
    ASSERT_FALSE(surfaceNode->IsTransparent());
    nodeMap.UnregisterRenderNode(filterNode1->GetId());
    nodeMap.UnregisterRenderNode(filterNode2->GetId());
    nodeMap.UnregisterRenderNode(filterNode3->GetId());
}

/**
 * @tc.name: AfterUpdateSurfaceDirtyCalc_001
 * @tc.desc: AfterUpdateSurfaceDirtyCalc Test，property.GetBoundsGeometry() not null
 * node.IsHardwareEnabledType() && node.GetZorderChanged() && curSurfaceNode_ is true
 * node is not LeashOrMainWindow and not MainWindowType
 * @tc.type:FUNC
 * @tc.require:issuesIAJO0U
 */
HWTEST_F(RSUniRenderVisitorTest, AfterUpdateSurfaceDirtyCalc_001, TestSize.Level2)
{
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(node, nullptr);

    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);

    NodeId id = 1;
    auto rsContext = std::make_shared<RSContext>();
    auto screenNode = std::make_shared<RSScreenRenderNode>(id, 0, rsContext);
    ASSERT_NE(screenNode, nullptr);
    screenNode->InitRenderParams();

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;
    rsUniRenderVisitor->curScreenNode_ = screenNode;

    ASSERT_NE(node->GetRenderProperties().GetBoundsGeometry(), nullptr);

    node->isHardwareEnabledNode_ = true;
    node->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_NODE;
    ASSERT_TRUE(node->IsHardwareEnabledType());
    node->zOrderChanged_ = true;
    ASSERT_TRUE(node->GetZorderChanged());
    ASSERT_NE(rsUniRenderVisitor->curSurfaceNode_, nullptr);
    ASSERT_TRUE((node->IsHardwareEnabledType() && node->GetZorderChanged() && rsUniRenderVisitor->curSurfaceNode_));

    node->nodeType_ = RSSurfaceNodeType::UI_EXTENSION_COMMON_NODE;
    ASSERT_FALSE(node->IsLeashOrMainWindow());
    ASSERT_FALSE(node->IsMainWindowType());

    ASSERT_TRUE(rsUniRenderVisitor->AfterUpdateSurfaceDirtyCalc(*node));
}

/**
 * @tc.name: AfterUpdateSurfaceDirtyCalc_002
 * @tc.desc: AfterUpdateSurfaceDirtyCalc Test，property.GetBoundsGeometry() not null
 * node.IsHardwareEnabledType() && node.GetZorderChanged() && curSurfaceNode_ is true
 * node is LeashOrMainWindow and is not MainWindowType
 * @tc.type:FUNC
 * @tc.require:issuesIAJO0U
 */
HWTEST_F(RSUniRenderVisitorTest, AfterUpdateSurfaceDirtyCalc_002, TestSize.Level2)
{
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(node, nullptr);

    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);

    NodeId id = 1;
    auto rsContext = std::make_shared<RSContext>();
    auto screenNode = std::make_shared<RSScreenRenderNode>(id, 0, rsContext);
    ASSERT_NE(screenNode, nullptr);
    screenNode->InitRenderParams();

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;
    rsUniRenderVisitor->curScreenNode_ = screenNode;

    ASSERT_NE(node->GetRenderProperties().GetBoundsGeometry(), nullptr);

    node->isHardwareEnabledNode_ = true;
    node->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_NODE;
    ASSERT_TRUE(node->IsHardwareEnabledType());
    node->zOrderChanged_ = true;
    ASSERT_TRUE(node->GetZorderChanged());
    ASSERT_NE(rsUniRenderVisitor->curSurfaceNode_, nullptr);
    ASSERT_TRUE((node->IsHardwareEnabledType() && node->GetZorderChanged() && rsUniRenderVisitor->curSurfaceNode_));

    node->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    ASSERT_TRUE(node->IsLeashOrMainWindow());
    ASSERT_FALSE(node->IsMainWindowType());

    ASSERT_TRUE(rsUniRenderVisitor->AfterUpdateSurfaceDirtyCalc(*node));
}

/**
 * @tc.name: UpdateHwcNodeInfoForAppNode_001
 * @tc.desc: UpdateHwcNodeInfoForAppNode Test, node.GetNeedCollectHwcNode() is true
 * node.IsHardwareEnabledType() is false
 * curSurfaceNode_ is nullptr
 * @tc.type:FUNC
 * @tc.require:issuesIAJO0U
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeInfoForAppNode_001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    NodeId id = 1;
    auto node = std::make_shared<RSSurfaceRenderNode>(id);
    ASSERT_NE(node, nullptr);

    node->needCollectHwcNode_ = true;
    ASSERT_TRUE(node->GetNeedCollectHwcNode());
    ASSERT_FALSE(node->IsHardwareEnabledType());
    rsUniRenderVisitor->curSurfaceNode_ = nullptr;

    rsUniRenderVisitor->UpdateHwcNodeInfoForAppNode(*node);
}

/**
 * @tc.name: UpdateHwcNodeInfoForAppNode_002
 * @tc.desc: UpdateHwcNodeInfoForAppNode Test, node.GetNeedCollectHwcNode() is true
 * node.IsHardwareEnabledType() is true
 * curSurfaceNode_ is nullptr
 * @tc.type:FUNC
 * @tc.require:issuesIAJO0U
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeInfoForAppNode_002, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    NodeId id = 1;
    auto node = std::make_shared<RSSurfaceRenderNode>(id);
    ASSERT_NE(node, nullptr);

    node->needCollectHwcNode_ = true;
    ASSERT_TRUE(node->GetNeedCollectHwcNode());
    node->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_NODE;
    node->isHardwareEnabledNode_ = true;
    ASSERT_TRUE(node->IsHardwareEnabledType());
    rsUniRenderVisitor->curSurfaceNode_ = nullptr;

    rsUniRenderVisitor->UpdateHwcNodeInfoForAppNode(*node);
}

/**
 * @tc.name: UpdateHwcNodeInfoForAppNode_003
 * @tc.desc: UpdateHwcNodeInfoForAppNode Test, node.GetNeedCollectHwcNode() is true
 * node.IsHardwareEnabledType() is false
 * curSurfaceNode_ is not nullptr
 * @tc.type:FUNC
 * @tc.require:issuesIAJO0U
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeInfoForAppNode_003, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    NodeId id = 1;
    auto node = std::make_shared<RSSurfaceRenderNode>(id);
    ASSERT_NE(node, nullptr);

    node->needCollectHwcNode_ = true;
    ASSERT_TRUE(node->GetNeedCollectHwcNode());
    ASSERT_FALSE(node->IsHardwareEnabledType());
    rsUniRenderVisitor->curSurfaceNode_ = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(rsUniRenderVisitor->curSurfaceNode_, nullptr);

    rsUniRenderVisitor->UpdateHwcNodeInfoForAppNode(*node);
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

    auto uifirstType = RSUifirstManager::Instance().GetUiFirstType();
    RSUifirstManager::Instance().SetUiFirstType(2);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->UpdateSurfaceRenderNodeScale(*leashWindowNode);
    RSUifirstManager::Instance().SetUiFirstType((int)uifirstType);
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
    NodeId id = 1;
    auto rsContext = std::make_shared<RSContext>();
    rsUniRenderVisitor->curScreenNode_ = std::make_shared<RSScreenRenderNode>(id, 0, rsContext);
    ASSERT_NE(rsUniRenderVisitor->curScreenNode_, nullptr);
    for (int i = 0; i < 10; i++) {
        auto id = static_cast<ScreenId>(i);
        RSMainThread::Instance()->SetLuminanceChangingStatus(id, true);
        ASSERT_EQ(rsUniRenderVisitor->CheckLuminanceStatusChange(id), true);
        RSMainThread::Instance()->SetLuminanceChangingStatus(id, false);
        ASSERT_EQ(rsUniRenderVisitor->CheckLuminanceStatusChange(id), false);
    }
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
    auto rsDisplayRenderNode = std::make_shared<RSScreenRenderNode>(20, 0, rsContext->weak_from_this());
    rsUniRenderVisitor->curScreenDirtyManager_ = rsDisplayRenderNode->GetDirtyManager();
    rsUniRenderVisitor->PrepareCanvasRenderNode(*rsCanvasRenderNode);
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
    ASSERT_NE(rsBaseRenderNode->stagingRenderParams_, nullptr);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->PrepareChildren(*rsBaseRenderNode);
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
    ASSERT_NE(rsProxyRenderNode, nullptr);
    rsUniRenderVisitor->PrepareProxyRenderNode(*rsProxyRenderNode);
    rsUniRenderVisitor->ProcessProxyRenderNode(*rsProxyRenderNode);

    config.id = 1;
    auto rsSurfaceRenderNodeS = std::make_shared<RSSurfaceRenderNode>(config);
    rsSurfaceRenderNodeS->AddChild(rsProxyRenderNode, 1);
    ASSERT_FALSE(rsSurfaceRenderNodeS->children_.empty());
    rsProxyRenderNode->Prepare(rsUniRenderVisitor);
}

HWTEST_F(RSUniRenderVisitorTest, RSScreenRenderNode001, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    auto rsDisplayRenderNode = std::make_shared<RSScreenRenderNode>(0, 0, rsContext->weak_from_this());
    rsDisplayRenderNode->InitRenderParams();
    ASSERT_NE(rsDisplayRenderNode->stagingRenderParams_, nullptr);
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

    auto rsDisplayRenderNode = std::make_shared<RSScreenRenderNode>(12, 0, rsContext->weak_from_this());
    rsDisplayRenderNode->InitRenderParams();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsDisplayRenderNode->AddChild(selfDrawSurfaceRenderNode, -1);
    rsDisplayRenderNode->AddChild(leashWindowNode, -1);

    auto screenManager = CreateOrGetScreenManager();
    auto screenId = CreateVirtualScreen(screenManager);
    ASSERT_NE(screenId, INVALID_SCREEN_ID);
    
    rsDisplayRenderNode->stagingRenderParams_ = std::make_unique<RSScreenRenderParams>(screenId);
    ASSERT_NE(rsDisplayRenderNode->stagingRenderParams_, nullptr);
    rsUniRenderVisitor->curScreenNode_ = rsDisplayRenderNode;
    // execute add child
    rsUniRenderVisitor->QuickPrepareScreenRenderNode(*rsDisplayRenderNode);
    // test if skip testNode
    rsUniRenderVisitor->QuickPrepareScreenRenderNode(*rsDisplayRenderNode);
    screenManager->RemoveVirtualScreen(screenId);
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
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    config.id = 10;
    config.name = "appWindowTestNode";
    auto defaultSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    defaultSurfaceRenderNode->InitRenderParams();
    defaultSurfaceRenderNode->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);

    auto rsDisplayRenderNode = std::make_shared<RSScreenRenderNode>(11, 0, rsContext->weak_from_this());
    rsDisplayRenderNode->InitRenderParams();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsDisplayRenderNode->AddChild(defaultSurfaceRenderNode, -1);

    auto screenManager = CreateOrGetScreenManager();
    auto screenId = CreateVirtualScreen(screenManager);
    ASSERT_NE(screenId, INVALID_SCREEN_ID);
    
    rsDisplayRenderNode->stagingRenderParams_ = std::make_unique<RSScreenRenderParams>(screenId);
    ASSERT_NE(rsDisplayRenderNode->stagingRenderParams_, nullptr);
    rsUniRenderVisitor->curScreenNode_ = rsDisplayRenderNode;
    // execute add child
    rsUniRenderVisitor->QuickPrepareScreenRenderNode(*rsDisplayRenderNode);
    // test if skip testNode
    rsUniRenderVisitor->QuickPrepareScreenRenderNode(*rsDisplayRenderNode);
    screenManager->RemoveVirtualScreen(screenId);
}

HWTEST_F(RSUniRenderVisitorTest, PrepareRootRenderNode001, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    auto rsRootRenderNode = std::make_shared<RSRootRenderNode>(0, rsContext->weak_from_this());
    rsRootRenderNode->InitRenderParams();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->PrepareRootRenderNode(*rsRootRenderNode);
    EXPECT_EQ(rsUniRenderVisitor->curSurfaceNode_, nullptr);
    rsUniRenderVisitor->curSurfaceNode_ = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(rsUniRenderVisitor->curSurfaceNode_, nullptr);
    rsUniRenderVisitor->PrepareRootRenderNode(*rsRootRenderNode);
    rsUniRenderVisitor->curSurfaceNode_->AddChild(rsRootRenderNode);
    rsUniRenderVisitor->PrepareRootRenderNode(*rsRootRenderNode);

    auto& property = rsRootRenderNode->GetMutableRenderProperties();
    property.SetBoundsWidth(5);
    rsUniRenderVisitor->PrepareRootRenderNode(*rsRootRenderNode);
    rsUniRenderVisitor->dirtyFlag_ = true;
    rsUniRenderVisitor->PrepareRootRenderNode(*rsRootRenderNode);
    property.ResetDirty();
    rsUniRenderVisitor->PrepareRootRenderNode(*rsRootRenderNode);
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
    config.id = 10;
    auto rsCanvasRenderNode = std::make_shared<RSCanvasRenderNode>(1, rsContext->weak_from_this());
    rsCanvasRenderNode->InitRenderParams();
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    rsSurfaceRenderNode->InitRenderParams();
    auto rsDisplayRenderNode = std::make_shared<RSScreenRenderNode>(11, 0, rsContext->weak_from_this());
    rsDisplayRenderNode->InitRenderParams();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();

    Occlusion::Rect rect{0, 80, 2560, 1600};
    Occlusion::Region region{rect};
    VisibleData vData;

    auto partialRenderType = RSSystemProperties::GetUniPartialRenderEnabled();
    auto isPartialRenderEnabled = (partialRenderType != PartialRenderType::DISABLED);
    ASSERT_EQ(isPartialRenderEnabled, true);

    // set surface to transparent, add a canvas node to create a transparent dirty region
    rsSurfaceRenderNode->SetAbilityBGAlpha(0);
    rsSurfaceRenderNode->SetSrcRect(RectI(0, 80, 2560, 1600));
    rsDisplayRenderNode->AddChild(rsSurfaceRenderNode, -1);
    rsSurfaceRenderNode->AddChild(rsCanvasRenderNode, -1);

    auto screenManager = CreateOrGetScreenManager();
    auto screenId = CreateVirtualScreen(screenManager);
    ASSERT_NE(screenId, INVALID_SCREEN_ID);
    
    rsDisplayRenderNode->stagingRenderParams_ = std::make_unique<RSScreenRenderParams>(screenId);
    ASSERT_NE(rsDisplayRenderNode->stagingRenderParams_, nullptr);
    rsUniRenderVisitor->curScreenNode_ = rsDisplayRenderNode;

    rsUniRenderVisitor->QuickPrepareScreenRenderNode(*rsDisplayRenderNode);
    rsSurfaceRenderNode->SetVisibleRegionRecursive(region, vData);
    screenManager->RemoveVirtualScreen(screenId);
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
    config.id = 10;
    auto rsCanvasRenderNode = std::make_shared<RSCanvasRenderNode>(1, rsContext->weak_from_this());
    ASSERT_NE(rsCanvasRenderNode, nullptr);
    rsCanvasRenderNode->InitRenderParams();
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    rsSurfaceRenderNode->InitRenderParams();
    auto rsDisplayRenderNode = std::make_shared<RSScreenRenderNode>(11, 0, rsContext->weak_from_this());
    ASSERT_NE(rsDisplayRenderNode, nullptr);
    rsDisplayRenderNode->InitRenderParams();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();

    rsSurfaceRenderNode->SetAbilityBGAlpha(0);
    rsSurfaceRenderNode->SetSrcRect(RectI(0, 80, 2560, 1600));
    // create a filter effect
    float blurRadiusX = 30.0f;
    float blurRadiusY = 30.0f;
    auto filter = RSFilter::CreateBlurFilter(blurRadiusX, blurRadiusY);
    rsCanvasRenderNode->GetMutableRenderProperties().filter_ = filter;
    rsDisplayRenderNode->AddChild(rsSurfaceRenderNode, -1);
    rsSurfaceRenderNode->AddChild(rsCanvasRenderNode, -1);

    auto screenManager = CreateOrGetScreenManager();
    auto screenId = CreateVirtualScreen(screenManager);
    ASSERT_NE(screenId, INVALID_SCREEN_ID);
    
    rsDisplayRenderNode->stagingRenderParams_ = std::make_unique<RSScreenRenderParams>(screenId);
    ASSERT_NE(rsDisplayRenderNode->stagingRenderParams_, nullptr);
    rsUniRenderVisitor->curScreenNode_ = rsDisplayRenderNode;
    rsUniRenderVisitor->QuickPrepareScreenRenderNode(*rsDisplayRenderNode);
    screenManager->RemoveVirtualScreen(screenId);
}

/**
 * @tc.name: CollectTopOcclusionSurfacesInfo001
 * @tc.desc: test CollectTopOcclusionSurfacesInfo
 * @tc.type:FUNC
 * @tc.require:issuesIB035y
 */
HWTEST_F(RSUniRenderVisitorTest, CollectTopOcclusionSurfacesInfo001, TestSize.Level2)
{
    RSSurfaceRenderNodeConfig config;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config);
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->CollectTopOcclusionSurfacesInfo(*rsSurfaceRenderNode, false);
}

/**
 * @tc.name: CollectTopOcclusionSurfacesInfo002
 * @tc.desc: test CollectTopOcclusionSurfacesInfo
 * @tc.type:FUNC
 * @tc.require:issuesIC13ZS
 */
HWTEST_F(RSUniRenderVisitorTest, CollectTopOcclusionSurfacesInfo002, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    NodeId id = 0;
    RSDisplayNodeConfig displayNodeConfig;
    rsUniRenderVisitor->curLogicalDisplayNode_ = std::make_shared<RSLogicalDisplayRenderNode>(id, displayNodeConfig);
    rsUniRenderVisitor->isStencilPixelOcclusionCullingEnabled_ = true;
    rsUniRenderVisitor->occlusionSurfaceOrder_ = TOP_OCCLUSION_SURFACES_NUM;

    RSSurfaceRenderNodeConfig config;
    ++id;
    config.id = id;
    config.nodeType = RSSurfaceNodeType::APP_WINDOW_NODE;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(config);
    ++id;
    config.id = id;
    config.nodeType = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    auto parentSurfaceNode = std::make_shared<RSSurfaceRenderNode>(config);
    surfaceNode->SetParent(parentSurfaceNode);
    
    rsUniRenderVisitor->CollectTopOcclusionSurfacesInfo(*surfaceNode, false);
    EXPECT_EQ(parentSurfaceNode->stencilVal_, TOP_OCCLUSION_SURFACES_NUM * OCCLUSION_ENABLE_SCENE_NUM + 1);
}

/**
 * @tc.name: CollectTopOcclusionSurfacesInfo003
 * @tc.desc: test CollectTopOcclusionSurfacesInfo
 * @tc.type:FUNC
 * @tc.require:issuesIC13ZS
 */
HWTEST_F(RSUniRenderVisitorTest, CollectTopOcclusionSurfacesInfo003, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    NodeId id = 0;
    RSDisplayNodeConfig displayNodeConfig;
    rsUniRenderVisitor->curLogicalDisplayNode_ = std::make_shared<RSLogicalDisplayRenderNode>(id, displayNodeConfig);
    rsUniRenderVisitor->isStencilPixelOcclusionCullingEnabled_ = true;
    rsUniRenderVisitor->occlusionSurfaceOrder_ = TOP_OCCLUSION_SURFACES_NUM;
    constexpr int defaultRegionSize{100};
    Occlusion::Region defaultRegion(Occlusion::Rect(0, 0, defaultRegionSize, defaultRegionSize));

    RSSurfaceRenderNodeConfig config;
    ++id;
    config.id = id;
    config.nodeType = RSSurfaceNodeType::APP_WINDOW_NODE;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(config);
    ++id;
    config.id = id;
    config.nodeType = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    auto parentSurfaceNode = std::make_shared<RSSurfaceRenderNode>(config);
    surfaceNode->SetParent(parentSurfaceNode);
    surfaceNode->GetOpaqueRegion() = defaultRegion;

    rsUniRenderVisitor->CollectTopOcclusionSurfacesInfo(*surfaceNode, true);
    EXPECT_EQ(parentSurfaceNode->stencilVal_, TOP_OCCLUSION_SURFACES_NUM * OCCLUSION_ENABLE_SCENE_NUM);
    EXPECT_EQ(rsUniRenderVisitor->occlusionSurfaceOrder_, TOP_OCCLUSION_SURFACES_NUM - 1);

    ++id;
    config.id = id;
    config.nodeType = RSSurfaceNodeType::APP_WINDOW_NODE;
    auto surfaceNode2 = std::make_shared<RSSurfaceRenderNode>(config);
    ++id;
    config.id = id;
    config.nodeType = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    auto parentSurfaceNode2 = std::make_shared<RSSurfaceRenderNode>(config);
    surfaceNode2->SetParent(parentSurfaceNode2);
    // Applications with sidebar blur have both opaque areas and transparent blur areas
    surfaceNode->GetOpaqueRegion() = defaultRegion;
    EXPECT_TRUE(surfaceNode->IsTransparent());
    FilterDirtyRegionInfo filterInfo = {
        .intersectRegion_ = Occlusion::Rect{0, 0, defaultRegionSize, defaultRegionSize},
        .filterDirty_ = Occlusion::Rect{0, 0, defaultRegionSize, defaultRegionSize}
    };
    surfaceNode2->GetDirtyManager()->GetFilterCollector().CollectFilterDirtyRegionInfo(filterInfo, false);

    rsUniRenderVisitor->CollectTopOcclusionSurfacesInfo(*surfaceNode2, false);
    EXPECT_EQ(parentSurfaceNode2->stencilVal_, (TOP_OCCLUSION_SURFACES_NUM - 1) * OCCLUSION_ENABLE_SCENE_NUM + 1);
    EXPECT_EQ(rsUniRenderVisitor->occlusionSurfaceOrder_, DEFAULT_OCCLUSION_SURFACE_ORDER);
}

/**
 * @tc.name: CollectTopOcclusionSurfacesInfo004
 * @tc.desc: test CollectTopOcclusionSurfacesInfo with first level cross node 
 * @tc.type:FUNC
 * @tc.require:issuesIC88GA
 */
HWTEST_F(RSUniRenderVisitorTest, CollectTopOcclusionSurfacesInfo004, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    NodeId id = 0;
    rsUniRenderVisitor->isStencilPixelOcclusionCullingEnabled_ = true;
    rsUniRenderVisitor->occlusionSurfaceOrder_ = TOP_OCCLUSION_SURFACES_NUM;
    constexpr int defaultRegionSize{100};
    Occlusion::Region defaultRegion(Occlusion::Rect(0, 0, defaultRegionSize, defaultRegionSize));

    RSSurfaceRenderNodeConfig config;
    ++id;
    config.id = id;
    config.nodeType = RSSurfaceNodeType::APP_WINDOW_NODE;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(config);
    ++id;
    config.id = id;
    config.nodeType = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    auto parentSurfaceNode = std::make_shared<RSSurfaceRenderNode>(config);
    surfaceNode->SetParent(parentSurfaceNode);
    surfaceNode->GetOpaqueRegion() = defaultRegion;
    surfaceNode->SetFirstLevelCrossNode(true);

    rsUniRenderVisitor->curLogicalDisplayNode_ = nullptr;
    rsUniRenderVisitor->CollectTopOcclusionSurfacesInfo(*surfaceNode, true);

    RSDisplayNodeConfig displayNodeConfig;
    rsUniRenderVisitor->curLogicalDisplayNode_ = std::make_shared<RSLogicalDisplayRenderNode>(id, displayNodeConfig);
    rsUniRenderVisitor->CollectTopOcclusionSurfacesInfo(*surfaceNode, true);
    EXPECT_EQ(parentSurfaceNode->stencilVal_, DEFAULT_OCCLUSION_SURFACE_ORDER);
}

/*
 * @tc.name: GetSurfaceTransparentFilterRegion
 * @tc.desc: Test RSUniRenderVisitorTest.GetSurfaceTransparentFilterRegion
 * @tc.type: FUNC
 * @tc.require: #issuesICCYYL
 */
HWTEST_F(RSUniRenderVisitorTest, GetSurfaceTransparentFilterRegion, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->dirtyManager_ = std::make_shared<RSDirtyRegionManager>();
    ASSERT_NE(surfaceNode->dirtyManager_, nullptr);


    // default, surface node is transparent.
    Occlusion::Region filterRegion = Occlusion::Region(Occlusion::Rect(DEFAULT_RECT));
    FilterDirtyRegionInfo filterInfo = {
        .intersectRegion_ = filterRegion,
        .filterDirty_ = filterRegion
    };
    surfaceNode->dirtyManager_->GetFilterCollector().CollectFilterDirtyRegionInfo(filterInfo, false);
    ASSERT_FALSE(rsUniRenderVisitor->GetSurfaceTransparentFilterRegion(*surfaceNode).IsEmpty());
    // if non-transparent, get empty region.
    surfaceNode->abilityBgAlpha_ = 255;
    surfaceNode->globalAlpha_ = 1.f;
    ASSERT_TRUE(rsUniRenderVisitor->GetSurfaceTransparentFilterRegion(*surfaceNode).IsEmpty());
}

/*
 * @tc.name: CheckMergeFilterDirtyWithPreDirty_001
 * @tc.desc: Test RSUniRenderVisitorTest.CheckMergeFilterDirtyWithPreDirty, test nullptr side case.
 * @tc.type: FUNC
 * @tc.require: #issuesICCYYL
 */
HWTEST_F(RSUniRenderVisitorTest, CheckMergeFilterDirtyWithPreDirty_001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    auto dirtyManager = std::make_shared<RSDirtyRegionManager>();
    FilterDirtyRegionInfo filterInfo;
    dirtyManager->GetFilterCollector().CollectFilterDirtyRegionInfo(filterInfo, false);
    rsUniRenderVisitor->CheckMergeFilterDirtyWithPreDirty(
        dirtyManager, Occlusion::Region(), FilterDirtyType::CONTAINER_FILTER);
    ASSERT_TRUE(dirtyManager->GetCurrentFrameDirtyRegion().IsEmpty());
}

/*
 * @tc.name: CheckMergeFilterDirtyWithPreDirty_002
 * @tc.desc: Test RSUniRenderVisitorTest.CheckMergeFilterDirtyWithPreDirty, test if intersect.
 * @tc.type: FUNC
 * @tc.require: #issuesICCYYL
 */
HWTEST_F(RSUniRenderVisitorTest, CheckMergeFilterDirtyWithPreDirty_002, TestSize.Level2)
{
    ASSERT_NE(RSMainThread::Instance(), nullptr);
    auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    auto dirtyManager = std::make_shared<RSDirtyRegionManager>();
    // mock different filter, case1: no below dirty. case2: below dirty.
    NodeId id = 1;
    auto filterNode1 = std::make_shared<RSRenderNode>(++id);
    auto filterNode2 = std::make_shared<RSRenderNode>(++id);
    filterNode1->GetMutableRenderProperties().GetEffect().materialFilter_ = std::make_shared<RSFilter>();
    filterNode1->GetMutableRenderProperties().backgroundFilter_ = std::make_shared<RSFilter>();
    filterNode2->GetMutableRenderProperties().GetEffect().needDrawBehindWindow_ = true;
    filterNode2->GetMutableRenderProperties().filter_ = std::make_shared<RSFilter>();

    // register filter node
    nodeMap.RegisterRenderNode(filterNode1);
    nodeMap.RegisterRenderNode(filterNode2);
    FilterDirtyRegionInfo filterInfo1 = { .id_ = filterNode1->GetId() };
    FilterDirtyRegionInfo filterInfo2 = { .id_ = filterNode2->GetId() };
    dirtyManager->GetFilterCollector().CollectFilterDirtyRegionInfo(filterInfo1, false);
    dirtyManager->GetFilterCollector().CollectFilterDirtyRegionInfo(filterInfo2, false);

    rsUniRenderVisitor->CheckMergeFilterDirtyWithPreDirty(
        dirtyManager, Occlusion::Region(), FilterDirtyType::OPAQUE_SURFACE_FILTER);
    ASSERT_FALSE(dirtyManager->GetFilterCollector().GetFilterDirtyRegionInfoList(true).size() == 0);
}

/*
 * @tc.name: CheckMergeFilterDirtyWithPreDirty_003
 * @tc.desc: Test difference between opauqe/transparent surface node when intersect with accumulated dirty.
 * @tc.type: FUNC
 * @tc.require: #issues19848
 */
HWTEST_F(RSUniRenderVisitorTest, CheckMergeFilterDirtyWithPreDirty_003, TestSize.Level2)
{
    ASSERT_NE(RSMainThread::Instance(), nullptr);
    auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    auto dirtyManager = std::make_shared<RSDirtyRegionManager>();
    // mock different filter, case1: no below dirty. case2: below dirty.
    NodeId id = 1;
    nodeMap.UnregisterRenderNode(id);
    auto filterNode1 = std::make_shared<RSRenderNode>(id);
    filterNode1->GetMutableRenderProperties().backgroundFilter_ = std::make_shared< RSFilter>();
    filterNode1->GetMutableRenderProperties().boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
    filterNode1->GetMutableRenderProperties().boundsGeo_->absRect_ = DEFAULT_FILTER_RECT;

    // register filter node
    nodeMap.RegisterRenderNode(filterNode1);
    FilterDirtyRegionInfo filterInfo1 = {
        .id_ = filterNode1->GetId(),
        .filterDirty_ = Occlusion::Rect(DEFAULT_FILTER_RECT)
    };

    dirtyManager->GetFilterCollector().CollectFilterDirtyRegionInfo(filterInfo1, false);
    rsUniRenderVisitor->CheckMergeFilterDirtyWithPreDirty(
        dirtyManager, Occlusion::Region(Occlusion::Rect(DEFAULT_RECT)), FilterDirtyType::OPAQUE_SURFACE_FILTER);
    ASSERT_TRUE(dirtyManager->GetCurrentFrameDirtyRegion().IsEmpty());

    dirtyManager->GetFilterCollector().CollectFilterDirtyRegionInfo(filterInfo1, false);
    rsUniRenderVisitor->CheckMergeFilterDirtyWithPreDirty(
        dirtyManager, Occlusion::Region(Occlusion::Rect(DEFAULT_RECT)), FilterDirtyType::TRANSPARENT_SURFACE_FILTER);
    ASSERT_FALSE(dirtyManager->GetCurrentFrameDirtyRegion().IsEmpty());
    nodeMap.UnregisterRenderNode(id);
}

/**
 * @tc.name: InitializeOcclusionHandler001
 * @tc.desc: test InitializeOcclusionHandler with the switch is enabled or not enabled
 * @tc.type: FUNC
 * @tc.require: issueICA6FQ
 */
HWTEST_F(RSUniRenderVisitorTest, InitializeOcclusionHandler001, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();

    OcclusionCullingParam::SetIntraAppControlsLevelOcclusionCullingEnable(false);
    rsUniRenderVisitor->InitializeOcclusionHandler(*surfaceNode);
    EXPECT_EQ(rsUniRenderVisitor->curOcclusionHandler_, nullptr);

    OcclusionCullingParam::SetIntraAppControlsLevelOcclusionCullingEnable(true);
    rsUniRenderVisitor->InitializeOcclusionHandler(*surfaceNode);
    EXPECT_EQ(rsUniRenderVisitor->curOcclusionHandler_, nullptr);

    surfaceNode->GetOcclusionParams()->keyOcclusionNodeIds_.emplace(surfaceNode->GetId());
    rsUniRenderVisitor->InitializeOcclusionHandler(*surfaceNode);
    EXPECT_NE(rsUniRenderVisitor->curOcclusionHandler_, nullptr);

    rsUniRenderVisitor->InitializeOcclusionHandler(*surfaceNode);
    EXPECT_EQ(rsUniRenderVisitor->curOcclusionHandler_,
        surfaceNode->GetOcclusionParams()->GetOcclusionHandler());
}

/**
 * @tc.name: CollectSubTreeAndProcessOcclusion001
 * @tc.desc: test CollectSubTreeAndProcessOcclusion
 * @tc.type: FUNC
 * @tc.require: issueICA6FQ
 */
HWTEST_F(RSUniRenderVisitorTest, CollectSubTreeAndProcessOcclusion001, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->CollectSubTreeAndProcessOcclusion(*surfaceNode, true);

    OcclusionCullingParam::SetIntraAppControlsLevelOcclusionCullingEnable(true);
    surfaceNode->GetOcclusionParams()->keyOcclusionNodeIds_.emplace(surfaceNode->GetId());
    rsUniRenderVisitor->InitializeOcclusionHandler(*surfaceNode);
    ASSERT_NE(rsUniRenderVisitor->curOcclusionHandler_, nullptr);
    rsUniRenderVisitor->curOcclusionHandler_->rootNodeId_ = surfaceNode->GetId();

    auto rsContext = std::make_shared<RSContext>();
    auto canvasNodeId = surfaceNode->GetId();
    ++canvasNodeId;
    auto rsCanvasRenderNode = std::make_shared<RSCanvasRenderNode>(canvasNodeId, rsContext->weak_from_this());
    rsUniRenderVisitor->CollectSubTreeAndProcessOcclusion(*rsCanvasRenderNode, false);
    ASSERT_NE(rsUniRenderVisitor->curOcclusionHandler_, nullptr);
    rsUniRenderVisitor->CollectSubTreeAndProcessOcclusion(*surfaceNode, true);
    ASSERT_EQ(rsUniRenderVisitor->curOcclusionHandler_, nullptr);
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
    rsSurfaceRenderNode1->SetVisibleRegionRecursive(region, vData);

    config.id = 11;
    auto rsSurfaceRenderNode2 = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    rsSurfaceRenderNode2->InitRenderParams();
    Occlusion::Rect rect2{100, 100, 500, 1500};
    Occlusion::Region region2{rect2};
    rsSurfaceRenderNode2->SetVisibleRegionRecursive(region2, vData);

    auto rsDisplayRenderNode = std::make_shared<RSScreenRenderNode>(9, 0, rsContext->weak_from_this());
    rsDisplayRenderNode->InitRenderParams();
    rsDisplayRenderNode->AddChild(rsSurfaceRenderNode1, -1);
    rsDisplayRenderNode->AddChild(rsSurfaceRenderNode2, -1);
    ASSERT_FALSE(rsDisplayRenderNode->children_.empty());

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    auto screenManager = CreateOrGetScreenManager();
    auto screenId = CreateVirtualScreen(screenManager);
    ASSERT_NE(screenId, INVALID_SCREEN_ID);
    
    rsDisplayRenderNode->stagingRenderParams_ = std::make_unique<RSScreenRenderParams>(screenId);
    ASSERT_NE(rsDisplayRenderNode->stagingRenderParams_, nullptr);
    rsUniRenderVisitor->curScreenNode_ = rsDisplayRenderNode;
    rsUniRenderVisitor->QuickPrepareScreenRenderNode(*rsDisplayRenderNode);
    screenManager->RemoveVirtualScreen(screenId);
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

/*
 * @tc.name: CheckPixelFormat
 * @tc.desc: Test RSUniRenderVisitorTest.CheckPixelFormat test
 * @tc.type: FUNC
 * @tc.require: issuesIBSNHZ
 */
HWTEST_F(RSUniRenderVisitorTest, CheckPixelFormat, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig config;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config);
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->hasFingerprint_ = true;
    rsUniRenderVisitor->CheckPixelFormat(*rsSurfaceRenderNode);
    rsUniRenderVisitor->hasFingerprint_ = false;
    rsUniRenderVisitor->CheckPixelFormat(*rsSurfaceRenderNode);
    NodeId id = 1;
    auto rsContext = std::make_shared<RSContext>();
    rsUniRenderVisitor->curScreenNode_ = std::make_shared<RSScreenRenderNode>(id, 0, rsContext);
    ASSERT_NE(rsUniRenderVisitor->curScreenNode_, nullptr);
    rsSurfaceRenderNode->hasFingerprint_ = true;
    rsUniRenderVisitor->CheckPixelFormat(*rsSurfaceRenderNode);
    ASSERT_EQ(rsUniRenderVisitor->hasFingerprint_, true);
    rsSurfaceRenderNode->hasFingerprint_ = false;
    rsUniRenderVisitor->hasFingerprint_ = false;
    rsUniRenderVisitor->CheckPixelFormat(*rsSurfaceRenderNode);
    rsSurfaceRenderNode->hdrPhotoNum_ = 1;
    ASSERT_EQ(rsSurfaceRenderNode->IsContentDirty(), false);
    rsUniRenderVisitor->curScreenNode_->isLuminanceStatusChange_ = true;
    rsUniRenderVisitor->CheckPixelFormat(*rsSurfaceRenderNode);
    rsSurfaceRenderNode->hdrPhotoNum_ = 0;
    rsSurfaceRenderNode->hdrEffectNum_ = 1;
    rsUniRenderVisitor->CheckPixelFormat(*rsSurfaceRenderNode);
    rsSurfaceRenderNode->hdrPhotoNum_ = 0;
    rsSurfaceRenderNode->hdrEffectNum_ = 0;
    rsSurfaceRenderNode->hdrUIComponentNum_ = 1;
    rsUniRenderVisitor->CheckPixelFormat(*rsSurfaceRenderNode);
    ASSERT_EQ(rsSurfaceRenderNode->IsContentDirty(), true);
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
    std::shared_ptr<RSScreenRenderNode> node = nullptr;

    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetParent(node);
    auto drawingCanvas = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(drawingCanvas, nullptr);
    rsUniRenderVisitor->ProcessSurfaceRenderNode(*surfaceNode);
}

/**
 * @tc.name: ProcessSurfaceRenderNode002
 * @tc.desc: Test RSUniRenderVisitorTest.ProcessSurfaceRenderNode with isUIFirst_
 * @tc.type: FUNC
 * @tc.require: issueI79KM8
 */
HWTEST_F(RSUniRenderVisitorTest, ProcessSurfaceRenderNode002, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    NodeId id = 0;
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSScreenRenderNode>(id, 0, rsContext);

    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    auto drawingCanvas = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(drawingCanvas, nullptr);
    surfaceNode->SetParent(node);
    rsUniRenderVisitor->ProcessSurfaceRenderNode(*surfaceNode);
    rsUniRenderVisitor->ProcessSurfaceRenderNode(*surfaceNode);
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
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSLogicalDisplayRenderNode>(id, config);
    node->SetSecurityDisplay(true);
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetSkipLayer(true);
    auto drawingCanvas = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(drawingCanvas, nullptr);
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
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSLogicalDisplayRenderNode>(id, config);
    node->SetSecurityDisplay(true);
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetSecurityLayer(true);
    auto drawingCanvas = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(drawingCanvas, nullptr);
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
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSLogicalDisplayRenderNode>(id, config);
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    auto drawingCanvas = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(drawingCanvas, nullptr);
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
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSLogicalDisplayRenderNode>(id, config);
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    auto drawingCanvas = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(drawingCanvas, nullptr);
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

    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSLogicalDisplayRenderNode>(id, config);
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);

    auto drawingCanvas = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(drawingCanvas, nullptr);
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

    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSLogicalDisplayRenderNode>(id, config);
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);

    auto drawingCanvas = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(drawingCanvas, nullptr);
    surfaceNode->SetParent(node);
    surfaceNode->SetProtectedLayer(false);
    rsUniRenderVisitor->ProcessSurfaceRenderNode(*surfaceNode);
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
    auto rsContext = std::make_shared<RSContext>();
    rsUniRenderVisitor->curScreenNode_ = std::make_shared<RSScreenRenderNode>(id1, 0, rsContext->weak_from_this());
    rsUniRenderVisitor->curScreenNode_->InitRenderParams();
    ASSERT_NE(rsUniRenderVisitor->curScreenNode_, nullptr);
    NodeId id2 = 2;
    auto node = std::make_shared<RSEffectRenderNode>(id2, rsContext->weak_from_this());
    node->InitRenderParams();
    rsUniRenderVisitor->PrepareEffectRenderNode(*node);
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
    NodeId id = 0;
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsUniRenderVisitor->curScreenNode_, nullptr);
    rsUniRenderVisitor->CheckColorSpace(*appWindowNode);
    rsUniRenderVisitor->curScreenNode_ = std::make_shared<RSScreenRenderNode>(id, 0, rsContext);
    rsUniRenderVisitor->curScreenNode_->stagingRenderParams_ = std::make_unique<RSScreenRenderParams>(id);
    rsUniRenderVisitor->CheckColorSpace(*appWindowNode);
    ASSERT_EQ(rsUniRenderVisitor->curScreenNode_->GetColorSpace(), appWindowNode->GetColorSpace());
}

/**
 * @tc.name: UpdateBlackListRecord001
 * @tc.desc: Test UpdateBlackListRecord
 * @tc.type: FUNC
 * @tc.require: issueIC9I11
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateBlackListRecord001, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(screenManager, nullptr);
    auto id = screenManager->CreateVirtualScreen("virtualScreen01", 480, 320, nullptr);
    ScreenId screenId = id;
    auto rsDisplayRenderNode = std::make_shared<RSScreenRenderNode>(id, screenId, rsContext->weak_from_this());
    rsDisplayRenderNode->InitRenderParams();

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->screenManager_ = screenManager;
    ASSERT_NE(rsUniRenderVisitor->screenManager_, nullptr);

    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    rsSurfaceRenderNode->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);
    rsUniRenderVisitor->UpdateBlackListRecord(*rsSurfaceRenderNode);
}

/*
 * @tc.name: UpdateBlackListRecord003
 * @tc.desc: Test UpdateBlackListRecord while hasVirtualDisplay
 * @tc.type: FUNC
 * @tc.require: issueICWNX9
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateBlackListRecord003, TestSize.Level2)
{
    auto node = RSTestUtil::CreateSurfaceNode();
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(screenManager, nullptr);
    
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->screenState_ = ScreenState::PRODUCER_SURFACE_ENABLE;
    rsUniRenderVisitor->screenManager_ = nullptr;
    rsUniRenderVisitor->hasMirrorDisplay_ = false;
    rsUniRenderVisitor->UpdateBlackListRecord(*node);
    ASSERT_TRUE(screenManager->GetBlackListVirtualScreenByNode(node->GetId()).empty());
}

/*
 * @tc.name: UpdateBlackListRecord004
 * @tc.desc: Test UpdateBlackListRecord while hasVirtualDisplay and screenManager is valid
 * @tc.type: FUNC
 * @tc.require: issueICWNX9
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateBlackListRecord004, TestSize.Level2)
{
    auto node = RSTestUtil::CreateSurfaceNode();
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(screenManager, nullptr);
    
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->screenState_ = ScreenState::PRODUCER_SURFACE_ENABLE;
    rsUniRenderVisitor->screenManager_ = screenManager;
    rsUniRenderVisitor->hasMirrorDisplay_ = false;
    rsUniRenderVisitor->UpdateBlackListRecord(*node);
    ASSERT_TRUE(screenManager->GetBlackListVirtualScreenByNode(node->GetId()).empty());
}

/*
 * @tc.name: UpdateBlackListRecord005
 * @tc.desc: Test UpdateBlackListRecord while has virtual mirror display and screenManager is valid
 * @tc.type: FUNC
 * @tc.require: issueICWNX9
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateBlackListRecord005, TestSize.Level2)
{
    auto node = RSTestUtil::CreateSurfaceNode();
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(screenManager, nullptr);
    
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->screenState_ = ScreenState::PRODUCER_SURFACE_ENABLE;
    rsUniRenderVisitor->screenManager_ = screenManager;
    rsUniRenderVisitor->hasMirrorDisplay_ = true;
    rsUniRenderVisitor->UpdateBlackListRecord(*node);
    ASSERT_TRUE(screenManager->GetBlackListVirtualScreenByNode(node->GetId()).empty());
}

/**
 * @tc.name: PrepareForCloneNode
 * @tc.desc: Test PrepareForCloneNode while node is not clone
 * @tc.type: FUNC
 * @tc.require: issueIBH7WD
 */
HWTEST_F(RSUniRenderVisitorTest, PrepareForCloneNode, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto surfaceRenderNode = RSSurfaceRenderNode(1);

    auto result = rsUniRenderVisitor->PrepareForCloneNode(surfaceRenderNode);
    ASSERT_FALSE(result);
}

/**
 * @tc.name: PrepareForCloneNode
 * @tc.desc: Test PrepareForCloneNode while node is clone
 * @tc.type: FUNC
 * @tc.require: issueIBKU7U
 */
HWTEST_F(RSUniRenderVisitorTest, PrepareForCloneNode001, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    auto surfaceRenderNode = RSSurfaceRenderNode(1);
    auto surfaceRenderNodeCloned = std::make_shared<RSSurfaceRenderNode>(2);
    ASSERT_NE(surfaceRenderNodeCloned, nullptr);
    auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();
    nodeMap.renderNodeMap_.clear();
    nodeMap.RegisterRenderNode(surfaceRenderNodeCloned);
    auto rsRenderNode = std::make_shared<RSRenderNode>(9, rsContext);
    ASSERT_NE(rsRenderNode, nullptr);
    auto drawable_ = DrawableV2::RSRenderNodeDrawable::OnGenerate(rsRenderNode);
    DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr clonedNodeRenderDrawableSharedPtr(drawable_);
    surfaceRenderNodeCloned->renderDrawable_ = clonedNodeRenderDrawableSharedPtr;

    surfaceRenderNode.isCloneNode_ = true;
    surfaceRenderNode.SetClonedNodeInfo(surfaceRenderNodeCloned->GetId(), true, false);
    auto result = rsUniRenderVisitor->PrepareForCloneNode(surfaceRenderNode);
    ASSERT_FALSE(result);
}

/**
 * @tc.name: UpdateInfoForClonedNode
 * @tc.desc: Test UpdateInfoForClonedNode
 * @tc.type: FUNC
 * @tc.require: issueIBKU7U
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateInfoForClonedNode, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    auto surfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(1);
    auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();
    rsUniRenderVisitor->cloneNodeMap_[surfaceRenderNode->GetId() + 1];
    nodeMap.renderNodeMap_.clear();
    nodeMap.RegisterRenderNode(surfaceRenderNode);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceRenderNode->stagingRenderParams_.get());
    rsUniRenderVisitor->UpdateInfoForClonedNode(*surfaceRenderNode);
    ASSERT_FALSE(surfaceParams->GetNeedCacheSurface());
    rsUniRenderVisitor->cloneNodeMap_[surfaceRenderNode->GetId()];
    rsUniRenderVisitor->UpdateInfoForClonedNode(*surfaceRenderNode);
    ASSERT_TRUE(surfaceParams->GetNeedCacheSurface());
}

/**
 * @tc.name: PrepareForCloneNode
 * @tc.desc: Test PrepareForCloneNode while node is clone
 * @tc.type: FUNC
 * @tc.require: issueIBKU7U
 */
HWTEST_F(RSUniRenderVisitorTest, PrepareForCloneNode002, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsContext = RSMainThread::Instance()->GetContext().shared_from_this();
    ASSERT_NE(rsContext, nullptr);
    auto surfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(1, rsContext->weak_from_this());
    auto surfaceRenderNodeDrawable_ = DrawableV2::RSRenderNodeDrawable::OnGenerate(surfaceRenderNode);
    DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr surfaceRenderNodeDrawableSharedPtr(surfaceRenderNodeDrawable_);
    surfaceRenderNode->renderDrawable_ = surfaceRenderNodeDrawableSharedPtr;
    auto surfaceRenderNodeCloned = std::make_shared<RSSurfaceRenderNode>(2, rsContext->weak_from_this());
    ASSERT_NE(surfaceRenderNodeCloned, nullptr);
    auto& nodeMap = rsContext->GetMutableNodeMap();
    nodeMap.renderNodeMap_.clear();
    nodeMap.RegisterRenderNode(surfaceRenderNodeCloned);
    auto& clonedNode = nodeMap.GetRenderNode<RSSurfaceRenderNode>(surfaceRenderNodeCloned->GetId());
    ASSERT_NE(clonedNode, nullptr);
    auto rsRenderNode = std::make_shared<RSRenderNode>(9, rsContext);
    ASSERT_NE(rsRenderNode, nullptr);
    auto drawable_ = DrawableV2::RSRenderNodeDrawable::OnGenerate(rsRenderNode);
    DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr clonedNodeRenderDrawableSharedPtr(drawable_);
    surfaceRenderNodeCloned->renderDrawable_ = clonedNodeRenderDrawableSharedPtr;

    surfaceRenderNode->isCloneNode_ = true;
    surfaceRenderNode->SetClonedNodeInfo(surfaceRenderNodeCloned->GetId(), true, false);
    auto result = rsUniRenderVisitor->PrepareForCloneNode(*surfaceRenderNode);
    ASSERT_TRUE(result);

    surfaceRenderNodeCloned->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);
    result = rsUniRenderVisitor->PrepareForCloneNode(*surfaceRenderNode);
    ASSERT_FALSE(result);

    surfaceRenderNode->stagingRenderParams_ = std::make_unique<RSRenderParams>(surfaceRenderNode->GetId());
    surfaceRenderNode->SetRelated(true);
    ASSERT_TRUE(surfaceRenderNode->IsRelated());
    surfaceRenderNodeCloned->SetSurfaceNodeType(RSSurfaceNodeType::NODE_MAX);
    result = rsUniRenderVisitor->PrepareForCloneNode(*surfaceRenderNode);
    ASSERT_FALSE(result);
}

/**
 * @tc.name: PrepareForMultiScreenViewSurfaceNode001
 * @tc.desc: Test PrepareForMultiScreenViewSurfaceNode when sourceNode and sourceNodeDrawable is null
 * @tc.type: FUNC
 * @tc.require: issueIBKU7U
 */
HWTEST_F(RSUniRenderVisitorTest, PrepareForMultiScreenViewSurfaceNode001, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    auto surfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(1);
    surfaceRenderNode->SetSourceScreenRenderNodeId(2);
    auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();
    nodeMap.renderNodeMap_.clear();
    nodeMap.RegisterRenderNode(surfaceRenderNode);
    rsUniRenderVisitor->PrepareForMultiScreenViewSurfaceNode(*surfaceRenderNode);
}

/**
 * @tc.name: PrepareForMultiScreenViewSurfaceNode002
 * @tc.desc: Test PrepareForMultiScreenViewSurfaceNode when sourceNodeDrawable is null
 * @tc.type: FUNC
 * @tc.require: issueIBKU7U
 */
HWTEST_F(RSUniRenderVisitorTest, PrepareForMultiScreenViewSurfaceNode002, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    auto surfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(1);
    surfaceRenderNode->SetSourceScreenRenderNodeId(2);
    auto rsContext = std::make_shared<RSContext>();
    auto sourceDisplayRenderNode = std::make_shared<RSScreenRenderNode>(2, 0, rsContext);
    sourceDisplayRenderNode->renderDrawable_ = nullptr;
    auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();
    nodeMap.renderNodeMap_.clear();
    nodeMap.RegisterRenderNode(surfaceRenderNode);
    rsUniRenderVisitor->PrepareForMultiScreenViewSurfaceNode(*surfaceRenderNode);
}

/**
 * @tc.name: PrepareForMultiScreenViewSurfaceNode003
 * @tc.desc: Test PrepareForMultiScreenViewSurfaceNode success
 * @tc.type: FUNC
 * @tc.require: issueIBKU7U
 */
HWTEST_F(RSUniRenderVisitorTest, PrepareForMultiScreenViewSurfaceNode003, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    auto surfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(1);
    surfaceRenderNode->SetSourceScreenRenderNodeId(2);
    auto rsContext = std::make_shared<RSContext>();
    auto sourceDisplayRenderNode = std::make_shared<RSScreenRenderNode>(2, 0, rsContext);
    auto sourceDisplayRenderNodeDrawable =
        std::make_shared<DrawableV2::RSScreenRenderNodeDrawable>(sourceDisplayRenderNode);
    sourceDisplayRenderNode->renderDrawable_ = sourceDisplayRenderNodeDrawable;
    auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();
    nodeMap.renderNodeMap_.clear();
    nodeMap.RegisterRenderNode(surfaceRenderNode);
    rsUniRenderVisitor->PrepareForMultiScreenViewSurfaceNode(*surfaceRenderNode);
}

/**
 * @tc.name: PrepareForMultiScreenViewDisplayNode001
 * @tc.desc: Test Pre when targetNode and targetNodeDrawable is null
 * @tc.type: FUNC
 * @tc.require: issueIBKU7U
 */
HWTEST_F(RSUniRenderVisitorTest, PrepareForMultiScreenViewDisplayNode001, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    auto rsContext = std::make_shared<RSContext>();
    auto screenRenderNode = std::make_shared<RSScreenRenderNode>(1, 0, rsContext);
    screenRenderNode->SetTargetSurfaceRenderNodeId(2);
    auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();
    nodeMap.renderNodeMap_.clear();
    nodeMap.RegisterRenderNode(screenRenderNode);
    rsUniRenderVisitor->PrepareForMultiScreenViewDisplayNode(*screenRenderNode);
}

/**
 * @tc.name: PrepareForMultiScreenViewDisplayNode002
 * @tc.desc: Test PrepareForMultiScreenViewSurfaceNode when targetNodeDrawable is null
 * @tc.type: FUNC
 * @tc.require: issueIBKU7U
 */
HWTEST_F(RSUniRenderVisitorTest, PrepareForMultiScreenViewDisplayNode002, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    auto rsContext = std::make_shared<RSContext>();
    auto screenRenderNode = std::make_shared<RSScreenRenderNode>(1, 0, rsContext);
    screenRenderNode->SetTargetSurfaceRenderNodeId(2);
    auto targetSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(2);
    targetSurfaceRenderNode->renderDrawable_ = nullptr;
    auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();
    nodeMap.renderNodeMap_.clear();
    nodeMap.RegisterRenderNode(screenRenderNode);
    rsUniRenderVisitor->PrepareForMultiScreenViewDisplayNode(*screenRenderNode);
}

/**
 * @tc.name: PrepareForMultiScreenViewDisplayNode003
 * @tc.desc: Test PrepareForMultiScreenViewSurfaceNode success
 * @tc.type: FUNC
 * @tc.require: issueIBKU7U
 */
HWTEST_F(RSUniRenderVisitorTest, PrepareForMultiScreenViewDisplayNode003, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    auto rsContext = std::make_shared<RSContext>();
    auto screenRenderNode = std::make_shared<RSScreenRenderNode>(1, 0, rsContext);
    screenRenderNode->SetTargetSurfaceRenderNodeId(2);
    auto targetSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(2);
    auto targetSurfaceRenderNodeDrawable =
        std::make_shared<DrawableV2::RSSurfaceRenderNodeDrawable>(targetSurfaceRenderNode);
    targetSurfaceRenderNode->renderDrawable_ = targetSurfaceRenderNodeDrawable;
    auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();
    nodeMap.renderNodeMap_.clear();
    nodeMap.RegisterRenderNode(screenRenderNode);
    rsUniRenderVisitor->PrepareForMultiScreenViewDisplayNode(*screenRenderNode);
}

/**
 * @tc.name: PrepareForCrossNodeTest
 * @tc.desc: Test PrepareForCrossNode
 * @tc.type: FUNC
 * @tc.require: issueB2YOV
 */
HWTEST_F(RSUniRenderVisitorTest, PrepareForCrossNodeTest, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    NodeId id = 0;
    auto rsContext = std::make_shared<RSContext>();
    rsUniRenderVisitor->curScreenNode_ = std::make_shared<RSScreenRenderNode>(id, 0, rsContext);
    ASSERT_NE(rsUniRenderVisitor->curScreenNode_, nullptr);

    auto node = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(node, nullptr);
    node->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);
    node->isCrossNode_ = true;
    rsUniRenderVisitor->curScreenNode_->SetIsFirstVisitCrossNodeDisplay(true);
    rsUniRenderVisitor->PrepareForCrossNode(*node);
}

/**
 * @tc.name: DealWithSpecialLayer
 * @tc.desc: Test DealWithSpecialLayer
 * @tc.type: FUNC
 * @tc.require: issueICM2MQ
 */
HWTEST_F(RSUniRenderVisitorTest, DealWithSpecialLayer, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    NodeId displayNodeId = 1;
    RSDisplayNodeConfig config;
    rsUniRenderVisitor->curLogicalDisplayNode_ = std::make_shared<RSLogicalDisplayRenderNode>(displayNodeId, config);

    auto node = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(node, nullptr);
    node->SetSecurityLayer(true);
    rsUniRenderVisitor->DealWithSpecialLayer(*node);
    node->isCloneCrossNode_ = true;
    ASSERT_EQ(node->IsCloneCrossNode(), true);
    rsUniRenderVisitor->DealWithSpecialLayer(*node);
    ASSERT_TRUE(rsUniRenderVisitor->curLogicalDisplayNode_->GetSpecialLayerMgr().Find(SpecialLayerType::HAS_SECURITY));
    auto sourceNode = RSTestUtil::CreateSurfaceNode();
    node->sourceCrossNode_ = sourceNode;
    ASSERT_NE(node->GetSourceCrossNode().lock(), nullptr);
    rsUniRenderVisitor->DealWithSpecialLayer(*node);
    ASSERT_TRUE(rsUniRenderVisitor->curLogicalDisplayNode_->GetSpecialLayerMgr().Find(SpecialLayerType::HAS_SECURITY));
}
/**
 * @tc.name: CheckSkipCrossNodeTest
 * @tc.desc: Test CheckSkipCrossNode
 * @tc.type: FUNC
 * @tc.require: issueB2YOV
 */
HWTEST_F(RSUniRenderVisitorTest, CheckSkipCrossNodeTest, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto surfaceRenderNode = RSSurfaceRenderNode(1);
    surfaceRenderNode.stagingRenderParams_ = std::make_unique<RSRenderParams>(surfaceRenderNode.GetId());
    auto rsContext = std::make_shared<RSContext>();
    rsUniRenderVisitor->curScreenNode_ = nullptr;
    rsUniRenderVisitor->CheckSkipCrossNode(surfaceRenderNode);
    rsUniRenderVisitor->curScreenNode_ = std::make_shared<RSScreenRenderNode>(2, 0, rsContext);
    ASSERT_FALSE(rsUniRenderVisitor->CheckSkipCrossNode(surfaceRenderNode));
    surfaceRenderNode.isCrossNode_ = true;
    ASSERT_FALSE(rsUniRenderVisitor->CheckSkipCrossNode(surfaceRenderNode));
    ASSERT_TRUE(rsUniRenderVisitor->CheckSkipCrossNode(surfaceRenderNode));
}

/**
 * @tc.name: ResetCrossNodesVisitedStatusTest
 * @tc.desc: Test ResetCrossNodesVisitedStatus
 * @tc.type: FUNC
 * @tc.require: issueIBV3N4
 */
HWTEST_F(RSUniRenderVisitorTest, ResetCrossNodesVisitedStatusTest, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    NodeId id = 0;
    auto rsContext = std::make_shared<RSContext>();
    rsUniRenderVisitor->curScreenNode_ = std::make_shared<RSScreenRenderNode>(id, 0, rsContext);
    ASSERT_NE(rsUniRenderVisitor->curScreenNode_, nullptr);

    auto node = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(node, nullptr);
    auto cloneNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(cloneNode, nullptr);
    node->isCrossNode_ = true;
    cloneNode->isCloneCrossNode_ = true;
    cloneNode->sourceCrossNode_ = node;
    node->cloneCrossNodeVec_.push_back(cloneNode);
    auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();
    nodeMap.renderNodeMap_.clear();
    pid_t pid1 = ExtractPid(node->GetId());
    pid_t pid2 = ExtractPid(cloneNode->GetId());
    nodeMap.renderNodeMap_[pid1][node->GetId()] = node;
    nodeMap.renderNodeMap_[pid2][cloneNode->GetId()] = cloneNode;

    node->SetCrossNodeVisitedStatus(true);
    ASSERT_TRUE(cloneNode->HasVisitedCrossNode());
    rsUniRenderVisitor->hasVisitedCrossNodeIds_.push_back(node->GetId());
    ASSERT_EQ(rsUniRenderVisitor->hasVisitedCrossNodeIds_.size(), 1);
    rsUniRenderVisitor->ResetCrossNodesVisitedStatus();
    ASSERT_EQ(rsUniRenderVisitor->hasVisitedCrossNodeIds_.size(), 0);
    ASSERT_FALSE(node->HasVisitedCrossNode());
    ASSERT_FALSE(cloneNode->HasVisitedCrossNode());

    cloneNode->SetCrossNodeVisitedStatus(true);
    ASSERT_TRUE(node->HasVisitedCrossNode());
    rsUniRenderVisitor->hasVisitedCrossNodeIds_.push_back(cloneNode->GetId());
    ASSERT_EQ(rsUniRenderVisitor->hasVisitedCrossNodeIds_.size(), 1);
    rsUniRenderVisitor->ResetCrossNodesVisitedStatus();
    ASSERT_EQ(rsUniRenderVisitor->hasVisitedCrossNodeIds_.size(), 0);
    ASSERT_FALSE(node->HasVisitedCrossNode());
    ASSERT_FALSE(cloneNode->HasVisitedCrossNode());
    nodeMap.renderNodeMap_.clear();
}

/**
 * @tc.name: HandleColorGamuts001
 * @tc.desc: HandleColorGamuts for virtual screen
 * @tc.type: FUNC
 * @tc.require: issueIAJJ43
 */
HWTEST_F(RSUniRenderVisitorTest, HandleColorGamuts001, TestSize.Level2)
{
    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    ASSERT_NE(screenManager, nullptr);
    auto virtualScreenId = screenManager->CreateVirtualScreen("virtual screen 001", 0, 0, nullptr);
    ASSERT_NE(INVALID_SCREEN_ID, virtualScreenId);

    auto rsContext = std::make_shared<RSContext>();
    auto displayNode = std::make_shared<RSScreenRenderNode>(0, 0, rsContext->weak_from_this());
    
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    displayNode->SetColorSpace(GRAPHIC_COLOR_GAMUT_ADOBE_RGB);
    rsUniRenderVisitor->HandleColorGamuts(*displayNode);

    ScreenColorGamut screenColorGamut;
    screenManager->GetScreenColorGamut(displayNode->GetScreenId(), screenColorGamut);
    ASSERT_EQ(displayNode->GetColorSpace(), static_cast<GraphicColorGamut>(screenColorGamut));

    screenManager->RemoveVirtualScreen(virtualScreenId);
}

/**
 * @tc.name: HandleColorGamuts002
 * @tc.desc: Test HandleColorGamuts
 * @tc.type: FUNC
 * @tc.require: issueIAJJ43
 */
HWTEST_F(RSUniRenderVisitorTest, HandleColorGamuts002, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto screenNode = std::make_shared<RSScreenRenderNode>(0, 0);

    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    ASSERT_NE(screenManager, nullptr);

    ScreenId screenId = 1;
    auto rsScreen = std::make_shared<RSScreen>(HdiOutput::CreateHdiOutput(screenId));
    rsScreen->property_.SetConnectionType(ScreenConnectionType::DISPLAY_CONNECTION_TYPE_EXTERNAL);
    rsScreen->supportedPhysicalColorGamuts_ = {COLOR_GAMUT_SRGB, COLOR_GAMUT_DCI_P3};
    rsUniRenderVisitor->screenManager_->MockHdiScreenConnected(rsScreen);
    screenNode->screenId_ = screenId;
    auto virtualScreenId = screenManager->CreateVirtualScreen("virtual screen 002", 0, 0, nullptr);
    screenNode->screenInfo_ = screenManager->QueryScreenInfo(virtualScreenId);
    ASSERT_NE(INVALID_SCREEN_ID, virtualScreenId);

    GraphicColorGamut originalColorSpace = GRAPHIC_COLOR_GAMUT_SRGB;
    screenNode->SetColorSpace(originalColorSpace);
    rsUniRenderVisitor->HandleColorGamuts(*screenNode);
    ScreenColorGamut screenColorGamut;
    screenManager->GetScreenColorGamut(screenNode->GetScreenId(), screenColorGamut);
    ASSERT_EQ(screenNode->GetColorSpace(), static_cast<GraphicColorGamut>(screenColorGamut));
}

/**
 * @tc.name: HandleColorGamuts003
 * @tc.desc: Test HandleColorGamuts
 * @tc.type: FUNC
 * @tc.require: issueIAJJ43
 */
HWTEST_F(RSUniRenderVisitorTest, HandleColorGamuts003, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto screenNode = std::make_shared<RSScreenRenderNode>(0, 0);

    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    ASSERT_NE(screenManager, nullptr);

    ScreenId screenId = 1;
    auto rsScreen = std::make_shared<RSScreen>(HdiOutput::CreateHdiOutput(screenId));
    rsScreen->property_.SetConnectionType(ScreenConnectionType::DISPLAY_CONNECTION_TYPE_EXTERNAL);
    rsScreen->supportedPhysicalColorGamuts_ = {COLOR_GAMUT_SRGB, COLOR_GAMUT_DCI_P3};
    rsUniRenderVisitor->screenManager_->MockHdiScreenConnected(rsScreen);
    screenNode->screenId_ = screenId;
    auto virtualScreenId = screenManager->CreateVirtualScreen("virtual screen 003", 0, 0, nullptr);
    screenNode->screenInfo_ = screenManager->QueryScreenInfo(virtualScreenId);
    ASSERT_NE(INVALID_SCREEN_ID, virtualScreenId);

    GraphicColorGamut originalColorSpace = GRAPHIC_COLOR_GAMUT_DISPLAY_P3;
    screenNode->SetColorSpace(originalColorSpace);
    rsScreen->supportedPhysicalColorGamuts_ = {COLOR_GAMUT_NATIVE, COLOR_GAMUT_DISPLAY_P3};
    rsUniRenderVisitor->HandleColorGamuts(*screenNode);
    ScreenColorGamut screenColorGamut;
    screenManager->GetScreenColorGamut(screenNode->GetScreenId(), screenColorGamut);
    ASSERT_NE(screenNode->GetColorSpace(), static_cast<GraphicColorGamut>(screenColorGamut));
}

/**
 * @tc.name: HandleColorGamuts004
 * @tc.desc: Test HandleColorGamuts
 * @tc.type: FUNC
 * @tc.require: issueIAJJ43
 */
HWTEST_F(RSUniRenderVisitorTest, HandleColorGamuts004, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto screenNode = std::make_shared<RSScreenRenderNode>(0, 0);

    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    ASSERT_NE(screenManager, nullptr);

    ScreenId screenId = 1;
    auto rsScreen = std::make_shared<RSScreen>(HdiOutput::CreateHdiOutput(screenId));
    rsScreen->property_.SetConnectionType(ScreenConnectionType::DISPLAY_CONNECTION_TYPE_EXTERNAL);
    rsScreen->supportedPhysicalColorGamuts_ = {COLOR_GAMUT_SRGB, COLOR_GAMUT_DCI_P3};
    rsUniRenderVisitor->screenManager_->MockHdiScreenConnected(rsScreen);
    screenNode->screenId_ = screenId;
    auto virtualScreenId = screenManager->CreateVirtualScreen("virtual screen 004", 0, 0, nullptr);
    screenNode->screenInfo_ = screenManager->QueryScreenInfo(virtualScreenId);
    ASSERT_NE(INVALID_SCREEN_ID, virtualScreenId);

    GraphicColorGamut originalColorSpace = GRAPHIC_COLOR_GAMUT_DISPLAY_P3;
    screenNode->SetColorSpace(originalColorSpace);
    rsScreen->supportedPhysicalColorGamuts_ = {};
    rsUniRenderVisitor->HandleColorGamuts(*screenNode);
    ScreenColorGamut screenColorGamut;
    screenManager->GetScreenColorGamut(screenNode->GetScreenId(), screenColorGamut);
    ASSERT_NE(screenNode->GetColorSpace(), static_cast<GraphicColorGamut>(screenColorGamut));
}

/**
 * @tc.name: HandleColorGamuts005
 * @tc.desc: Test HandleColorGamuts
 * @tc.type: FUNC
 * @tc.require: issueIAJJ43
 */
HWTEST_F(RSUniRenderVisitorTest, HandleColorGamuts005, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto screenNode = std::make_shared<RSScreenRenderNode>(0, 0);

    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    ASSERT_NE(screenManager, nullptr);

    auto rsContext = std::make_shared<RSContext>();
    ScreenId screenId = 1;
    auto rsScreen = std::make_shared<RSScreen>(HdiOutput::CreateHdiOutput(screenId));
    rsScreen->property_.SetConnectionType(ScreenConnectionType::DISPLAY_CONNECTION_TYPE_EXTERNAL);
    rsScreen->supportedPhysicalColorGamuts_ = {COLOR_GAMUT_SRGB, COLOR_GAMUT_DCI_P3};
    rsUniRenderVisitor->screenManager_->MockHdiScreenConnected(rsScreen);
    auto displayNode = std::make_shared<RSScreenRenderNode>(TestSrc::limitNumber::Uint64[6], screenId, rsContext);
    ASSERT_NE(displayNode, nullptr);

    screenNode->SetMirrorSource(displayNode);
    screenNode->screenId_ = screenId;
    auto virtualScreenId = screenManager->CreateVirtualScreen("virtual screen 005", 0, 0, nullptr);
    screenNode->screenInfo_ = screenManager->QueryScreenInfo(virtualScreenId);
    ASSERT_NE(INVALID_SCREEN_ID, virtualScreenId);

    GraphicColorGamut originalColorSpace = GRAPHIC_COLOR_GAMUT_DISPLAY_P3;
    screenNode->SetColorSpace(originalColorSpace);
    rsScreen->supportedPhysicalColorGamuts_ = {};
    rsUniRenderVisitor->HandleColorGamuts(*screenNode);
    ScreenColorGamut screenColorGamut;
    screenManager->GetScreenColorGamut(screenNode->GetScreenId(), screenColorGamut);
    ASSERT_NE(screenNode->GetColorSpace(), static_cast<GraphicColorGamut>(screenColorGamut));
}


/**
 * @tc.name: CheckColorSpaceWithSelfDrawingNode001
 * @tc.desc: Test RSUniRenderVisitorTest.CheckColorSpaceWithSelfDrawingNode while
 *           selfDrawingNode's color space is equal to GRAPHIC_COLOR_GAMUT_DISPLAY_P3,
 *           and this node will be drawn with gpu
 * @tc.type: FUNC
 * @tc.require: issueICGKPE
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
    ASSERT_EQ(GRAPHIC_COLOR_GAMUT_DISPLAY_P3, selfDrawingNode->GetColorSpace());
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
    RSMainThread::Instance()->AddSelfDrawingNodes(selfDrawingNode);
    NodeId id = 0;
    auto rsContext = std::make_shared<RSContext>();
    auto displayNode = std::make_shared<RSScreenRenderNode>(id, 0, rsContext);
    ASSERT_NE(displayNode, nullptr);
    displayNode->stagingRenderParams_ = std::make_unique<RSScreenRenderParams>(id);
    displayNode->SetColorSpace(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    rsUniRenderVisitor->UpdateColorSpaceAfterHwcCalc(*displayNode);
    ASSERT_EQ(displayNode->GetColorSpace(), GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    auto rsRenderNode = std::make_shared<RSRenderNode>(++id);
    selfDrawingNode->SetAncestorScreenNode(rsRenderNode);
    rsUniRenderVisitor->UpdateColorSpaceAfterHwcCalc(*displayNode);
    ASSERT_EQ(displayNode->GetColorSpace(), GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    auto displayNode2 = std::make_shared<RSScreenRenderNode>(++id, 0, rsContext);
    selfDrawingNode->SetAncestorScreenNode(displayNode2);
    rsUniRenderVisitor->UpdateColorSpaceAfterHwcCalc(*displayNode);
    ASSERT_EQ(displayNode->GetColorSpace(), GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    selfDrawingNode->SetAncestorScreenNode(displayNode);
    selfDrawingNode->SetHardwareForcedDisabledState(true);
    selfDrawingNode->SetIsOnTheTree(true);
    selfDrawingNode->SetColorSpace(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
    rsUniRenderVisitor->UpdateColorSpaceAfterHwcCalc(*displayNode);
    ASSERT_EQ(displayNode->GetColorSpace(), GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
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
    auto rsContext = std::make_shared<RSContext>();
    auto displayNode = std::make_shared<RSScreenRenderNode>(id, 0, rsContext);

    rsUniRenderVisitor->curScreenNode_ = displayNode;
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
    auto rsContext = std::make_shared<RSContext>();
    auto displayNode = std::make_shared<RSScreenRenderNode>(id, 0, rsContext);

    rsUniRenderVisitor->curScreenNode_ = displayNode;
    rsUniRenderVisitor->ResetCurSurfaceInfoAsUpperSurfaceParent(*surfaceNode);
    ASSERT_EQ(rsUniRenderVisitor->curSurfaceNode_, surfaceNode);
}

/*
 * @tc.name: ResetCurSurfaceInfoAsUpperSurfaceParent003
 * @tc.desc: Reset curSurfaceInfo when upper surface is leash/main window
 * @tc.type: FUNC
 * @tc.require: issuesI8MQCS
 */
HWTEST_F(RSUniRenderVisitorTest, ResetCurSurfaceInfoAsUpperSurfaceParent003, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto upperSurfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    ASSERT_NE(upperSurfaceNode, nullptr);
    upperSurfaceNode->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    surfaceNode->SetParent(upperSurfaceNode);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;
    NodeId id = 0;
    auto rsContext = std::make_shared<RSContext>();
    auto displayNode = std::make_shared<RSScreenRenderNode>(id, 0, rsContext);
    rsUniRenderVisitor->curScreenNode_ = displayNode;
    rsUniRenderVisitor->ResetCurSurfaceInfoAsUpperSurfaceParent(*surfaceNode);
    ASSERT_EQ(rsUniRenderVisitor->curSurfaceNode_, surfaceNode);
}

/*
 * @tc.name: UpdateCornerRadiusInfoForDRM
 * @tc.desc: Test RSUniRenderVistorTest.UpdateCornerRadiusInfoForDRM
 * @tc.type: FUNC
 * @tc.require: issuesIBE1C8
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateCornerRadiusInfoForDRM, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    std::vector<RectI> hwcRects;

    rsUniRenderVisitor->UpdateCornerRadiusInfoForDRM(nullptr, hwcRects);

    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetProtectedLayer(true);

    rsUniRenderVisitor->UpdateCornerRadiusInfoForDRM(surfaceNode, hwcRects);

    // register instance root node
    auto instanceRootNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(instanceRootNode, nullptr);
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    auto& nodeMap = rsContext->GetMutableNodeMap();
    NodeId instanceRootNodeId = instanceRootNode->GetId();
    pid_t instanceRootNodePid = ExtractPid(instanceRootNodeId);
    nodeMap.renderNodeMap_[instanceRootNodePid][instanceRootNodeId] = instanceRootNode;

    // create surface node for UpdateCornerRadiusInfoForDRM
    surfaceNode->context_ = rsContext;
    surfaceNode->instanceRootNodeId_ = instanceRootNode->GetId();
    hwcRects.emplace_back(0, 0, 1, 1);
    rsUniRenderVisitor->UpdateCornerRadiusInfoForDRM(surfaceNode, hwcRects);

    instanceRootNode->absDrawRect_ = {-10, -10, 100, 100};
    instanceRootNode->SetGlobalCornerRadius({-10, -10, 100, 100});
    rsUniRenderVisitor->UpdateCornerRadiusInfoForDRM(surfaceNode, hwcRects);

    surfaceNode->GetInstanceRootNode()->renderProperties_.SetBounds({0, 0, 1, 1});
    Drawing::Matrix matrix;
    matrix.SetMatrix(1, 2, 3, 4, 5, 6, 7, 8, 9);
    surfaceNode->SetTotalMatrix(matrix);
    surfaceNode->selfDrawRect_ = {0, 0, 1, 1};
    rsUniRenderVisitor->UpdateCornerRadiusInfoForDRM(surfaceNode, hwcRects);
}

/*
 * @tc.name: CheckIfRoundCornerIntersectDRM
 * @tc.desc: Test CheckIfRoundCornerIntersectDRM
 * @tc.type: FUNC
 * @tc.require: issuesIBE1C8
 */
HWTEST_F(RSUniRenderVisitorTest, CheckIfRoundCornerIntersectDRM, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    auto hwcNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(hwcNode, nullptr);
    auto instanceRootNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(instanceRootNode, nullptr);
    NodeId instanceRootNodeId = instanceRootNode->GetId();
    pid_t instanceRootNodePid = ExtractPid(instanceRootNodeId);
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    auto& nodeMap = rsContext->GetMutableNodeMap();
    nodeMap.renderNodeMap_[instanceRootNodePid][instanceRootNodeId] = instanceRootNode;
    hwcNode->context_ = rsContext;
    hwcNode->instanceRootNodeId_ = instanceRootNode->GetId();
    instanceRootNode->absDrawRect_ = {-10, -10, 1000, 1000};
    instanceRootNode->SetGlobalCornerRadius({-10, -10, 1000, 1000});
    hwcNode->GetInstanceRootNode()->renderProperties_.SetBounds({0, 0, 1, 1});
    Drawing::Matrix matrix = Drawing::Matrix();
    matrix.SetMatrix(1, 2, 3, 4, 5, 6, 7, 8, 9);
    hwcNode->SetTotalMatrix(matrix);
    hwcNode->selfDrawRect_ = {0, 0, 1, 1};

    auto instanceNode = hwcNode->GetInstanceRootNode() ?
        hwcNode->GetInstanceRootNode()->ReinterpretCastTo<RSSurfaceRenderNode>() : nullptr;
    auto hwcGeo = hwcNode->GetRenderProperties().GetBoundsGeometry();
    auto instanceAbsRect = instanceNode->GetAbsDrawRect();
    auto instanceCornerRadius = instanceNode->GetGlobalCornerRadius();
    std::vector<float> ratioVector = { 0.0f, 0.0f, 0.0f, 0.0f };
    auto hwcAbsRect = hwcGeo->MapRect(hwcNode->GetSelfDrawRect(), hwcNode->GetTotalMatrix());
    hwcAbsRect = hwcAbsRect.IntersectRect(instanceAbsRect);
    auto ratio = static_cast<float>(instanceAbsRect.GetWidth()) /
        instanceNode->GetRenderProperties().GetBoundsWidth();
    bool result = rsUniRenderVisitor->CheckIfRoundCornerIntersectDRM(
        ratio, ratioVector, instanceCornerRadius, instanceAbsRect, hwcAbsRect);
    ASSERT_TRUE(result);
}

/*
 * @tc.name: QuickPrepareSurfaceRenderNode001
 * @tc.desc: Test RSUniRenderVisitorTest.QuickPrepareSurfaceRenderNode while surface node has security layer
 * @tc.type: FUNC
 * @tc.require: issuesI7SAJC
 */
HWTEST_F(RSUniRenderVisitorTest, QuickPrepareSurfaceRenderNode001, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetSecurityLayer(true);
    surfaceNode->SetIsOnTheTree(true, surfaceNode->GetId(), surfaceNode->GetId());
    surfaceNode->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;

    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSLogicalDisplayRenderNode>(id, config);
    ASSERT_NE(displayNode, nullptr);
    displayNode->InitRenderParams();

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curLogicalDisplayNode_ = displayNode;
    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;
    auto rsContext = std::make_shared<RSContext>();
    rsUniRenderVisitor->curScreenNode_ = std::make_shared<RSScreenRenderNode>(1, 0, rsContext);
    rsUniRenderVisitor->curLogicalDisplayNode_->GetMultableSpecialLayerMgr().RemoveIds(SpecialLayerType::SKIP, id);
    rsUniRenderVisitor->QuickPrepareSurfaceRenderNode(*surfaceNode);
    ASSERT_EQ(rsUniRenderVisitor->curLogicalDisplayNode_->GetSpecialLayerMgr().Find(SpecialLayerType::HAS_SKIP), false);
}

/*
 * @tc.name: QuickPrepareSurfaceRenderNode002
 * @tc.desc: Test RSUniRenderVisitorTest.QuickPrepareSurfaceRenderNode while surface node has skip layer
 * @tc.type: FUNC
 * @tc.require: issuesI7SAJC
 */
HWTEST_F(RSUniRenderVisitorTest, QuickPrepareSurfaceRenderNode002, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetSkipLayer(true);
    surfaceNode->SetIsOnTheTree(true, surfaceNode->GetId(), surfaceNode->GetId());
    surfaceNode->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;

    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSLogicalDisplayRenderNode>(id, config);
    ASSERT_NE(displayNode, nullptr);
    displayNode->InitRenderParams();
    displayNode->AddChild(surfaceNode, 0);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curLogicalDisplayNode_ = displayNode;
    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;
    auto rsContext = std::make_shared<RSContext>();
    rsUniRenderVisitor->curScreenNode_ = std::make_shared<RSScreenRenderNode>(1, 0, rsContext);
    rsUniRenderVisitor->QuickPrepareSurfaceRenderNode(*surfaceNode);
    rsUniRenderVisitor->curLogicalDisplayNode_->GetMultableSpecialLayerMgr().
        RemoveIds(SpecialLayerType::SKIP, surfaceNode->GetId());
    ASSERT_EQ(rsUniRenderVisitor->curLogicalDisplayNode_->GetSpecialLayerMgr().Find(SpecialLayerType::SKIP), false);
}

/*
 * @tc.name: QuickPrepareSurfaceRenderNode003
 * @tc.desc: Test RSUniRenderVisitorTest.QuickPrepareSurfaceRenderNode while surface node is capture window
 * @tc.type: FUNC
 * @tc.require: issuesI7SAJC
 */
HWTEST_F(RSUniRenderVisitorTest, QuickPrepareSurfaceRenderNode003, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetIsOnTheTree(true, surfaceNode->GetId(), surfaceNode->GetId());
    surfaceNode->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    surfaceNode->name_ = CAPTURE_WINDOW_NAME;

    NodeId id = 0;
    auto rsContext = std::make_shared<RSContext>();
    auto displayNode = std::make_shared<RSScreenRenderNode>(id, 0, rsContext);
    ASSERT_NE(displayNode, nullptr);
    displayNode->AddChild(surfaceNode, 0);

    RSDisplayNodeConfig config;
    auto logicalDisplayNode = std::make_shared<RSLogicalDisplayRenderNode>(id, config);
    logicalDisplayNode->InitRenderParams();

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curScreenNode_ = displayNode;
    rsUniRenderVisitor->curLogicalDisplayNode_ = logicalDisplayNode;
    rsUniRenderVisitor->QuickPrepareSurfaceRenderNode(*surfaceNode);
    ASSERT_NE(rsUniRenderVisitor->curScreenNode_, nullptr);
}

/*
 * @tc.name: QuickPrepareSurfaceRenderNode004
 * @tc.desc: Test RSUniRenderVisitorTest.QuickPrepareSurfaceRenderNode while surface node has protected layer
 * @tc.type: FUNC
 * @tc.require: issueI7ZSC2
 */
HWTEST_F(RSUniRenderVisitorTest, QuickPrepareSurfaceRenderNode004, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetIsOnTheTree(true, surfaceNode->GetId(), surfaceNode->GetId());
    surfaceNode->SetProtectedLayer(true);

    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSLogicalDisplayRenderNode>(id, config);
    ASSERT_NE(displayNode, nullptr);
    displayNode->InitRenderParams();
    displayNode->AddChild(surfaceNode, 0);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto screenManager = CreateOrGetScreenManager();
    rsUniRenderVisitor->rsScreenNodeNum_ = 2;

    auto rsContext = std::make_shared<RSContext>();
    rsUniRenderVisitor->curScreenNode_ = std::make_shared<RSScreenRenderNode>(1, 0, rsContext);
    rsUniRenderVisitor->curLogicalDisplayNode_ = displayNode;
    rsUniRenderVisitor->curLogicalDisplayNode_->GetMultableSpecialLayerMgr().RemoveIds(SpecialLayerType::PROTECTED, id);
    rsUniRenderVisitor->QuickPrepareSurfaceRenderNode(*surfaceNode);
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
    auto displayNode = std::make_shared<RSLogicalDisplayRenderNode>(id, config);
    ASSERT_NE(displayNode, nullptr);
    displayNode->InitRenderParams();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curLogicalDisplayNode_ = displayNode;
    rsUniRenderVisitor->curLogicalDisplayNode_->GetMultableSpecialLayerMgr().RemoveIds(SpecialLayerType::PROTECTED, id);
    rsUniRenderVisitor->PrepareSurfaceRenderNode(*surfaceNode);
}

/**
 * @tc.name: QuickPrepareScreenRenderNode005
 * @tc.desc: Test QuickPrepareScreenRenderNode
 * @tc.type: FUNC
 * @tc.require: issue20827
 */
HWTEST_F(RSUniRenderVisitorTest, QuickPrepareScreenRenderNode005, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    // 11 non-zero node id
    auto rsScreenRenderNode = std::make_shared<RSScreenRenderNode>(11, 0, rsContext->weak_from_this());
    rsScreenRenderNode->InitRenderParams();

    auto screenManager = CreateOrGetScreenManager();
    auto screenId = CreateVirtualScreen(screenManager);
    ASSERT_NE(screenId, INVALID_SCREEN_ID);

    auto parent = std::make_shared<RSRenderNode>(1);
    parent->InitRenderParams();
    rsScreenRenderNode->parent_ = parent;

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->curScreenNode_ = rsScreenRenderNode;
    rsUniRenderVisitor->QuickPrepareScreenRenderNode(*rsScreenRenderNode);
}

/*
 * @tc.name: QuickPrepareSurfaceRenderNode006
 * @tc.desc: Test RSUniRenderVisitorTest.QuickPrepareSurfaceRenderNode while surface node should be skipped
 * @tc.type: FUNC
 * @tc.require: issue20827
 */
HWTEST_F(RSUniRenderVisitorTest, QuickPrepareSurfaceRenderNode006, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    const_cast<SurfaceWindowType&>(surfaceNode->surfaceWindowType_) = SurfaceWindowType::SYSTEM_SCB_WINDOW;
    surfaceNode->SetStableSkipReached(true);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    auto isFoldScreen = RSSystemProperties::IsFoldScreenFlag();
    if (!isFoldScreen) {
        rsUniRenderVisitor->QuickPrepareSurfaceRenderNode(*surfaceNode);
    }
}

/*
 * @tc.name: CheckSkipBackgroundSurfaceRenderNode001
 * @tc.desc: Test RSUniRenderVisitorTest.CheckSkipBackgroundSurfaceRenderNode while has visible region
 * @tc.type: FUNC
 * @tc.require: issue20827
 */
HWTEST_F(RSUniRenderVisitorTest, CheckSkipBackgroundSurfaceRenderNode001, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);

    RectI rect = RectI(0, 0, 100, 100);
    auto occlusionRegion = Occlusion::Region(rect);
    surfaceNode->visibleRegion_ = occlusionRegion;

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    auto isQuickSkip = rsUniRenderVisitor->CheckSkipBackgroundSurfaceRenderNode(*surfaceNode);
    ASSERT_TRUE(!isQuickSkip);
}

/*
 * @tc.name: CheckSkipBackgroundSurfaceRenderNode002
 * @tc.desc: Test RSUniRenderVisitorTest.CheckSkipBackgroundSurfaceRenderNode while tree state changed dirty
 * @tc.type: FUNC
 * @tc.require: issue20827
 */
HWTEST_F(RSUniRenderVisitorTest, CheckSkipBackgroundSurfaceRenderNode002, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetTreeStateChangeDirty(true);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    auto isQuickSkip = rsUniRenderVisitor->CheckSkipBackgroundSurfaceRenderNode(*surfaceNode);
    ASSERT_TRUE(!isQuickSkip);
}

/*
 * @tc.name: CheckSkipBackgroundSurfaceRenderNode003
 * @tc.desc: Test RSUniRenderVisitorTest.CheckSkipBackgroundSurfaceRenderNode while accumulatedOcclusionRegion_ is empty
 * @tc.type: FUNC
 * @tc.require: issue20827
 */
HWTEST_F(RSUniRenderVisitorTest, CheckSkipBackgroundSurfaceRenderNode003, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->accumulatedOcclusionRegion_.Reset();
    auto isQuickSkip = rsUniRenderVisitor->CheckSkipBackgroundSurfaceRenderNode(*surfaceNode);
    ASSERT_TRUE(!isQuickSkip);
}

/*
 * @tc.name: CheckSkipBackgroundSurfaceRenderNode004
 * @tc.desc: Test RSUniRenderVisitorTest.CheckSkipBackgroundSurfaceRenderNode while curScreenNode_ is nullptr
 * @tc.type: FUNC
 * @tc.require: issue20827
 */
HWTEST_F(RSUniRenderVisitorTest, CheckSkipBackgroundSurfaceRenderNode004, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    RectI rect = RectI(0, 0, 100, 100);
    auto occlusionRegion = Occlusion::Region(rect);
    rsUniRenderVisitor->accumulatedOcclusionRegion_.OrSelf(occlusionRegion);
    rsUniRenderVisitor->curScreenNode_ = nullptr;
    auto isQuickSkip = rsUniRenderVisitor->CheckSkipBackgroundSurfaceRenderNode(*surfaceNode);
    ASSERT_TRUE(!isQuickSkip);
}

/*
 * @tc.name: CheckSkipBackgroundSurfaceRenderNode005
 * @tc.desc: Test RSUniRenderVisitorTest.CheckSkipBackgroundSurfaceRenderNode without full screen occlusion
 * @tc.type: FUNC
 * @tc.require: issue20827
 */
HWTEST_F(RSUniRenderVisitorTest, CheckSkipBackgroundSurfaceRenderNode005, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    RectI rect = RectI(0, 0, 100, 100);
    auto occlusionRegion = Occlusion::Region(rect);
    rsUniRenderVisitor->accumulatedOcclusionRegion_.OrSelf(occlusionRegion);
    auto rsContext = std::make_shared<RSContext>();
    rsUniRenderVisitor->curScreenNode_ = std::make_shared<RSScreenRenderNode>(1, 0, rsContext);
    rsUniRenderVisitor->curScreenNode_->screenInfo_.width = 200;
    rsUniRenderVisitor->curScreenNode_->screenInfo_.height = 200;
    auto isQuickSkip = rsUniRenderVisitor->CheckSkipBackgroundSurfaceRenderNode(*surfaceNode);
    ASSERT_TRUE(!isQuickSkip);
}

/*
 * @tc.name: CheckSkipBackgroundSurfaceRenderNode006
 * @tc.desc: Test RSUniRenderVisitorTest.CheckSkipBackgroundSurfaceRenderNode while full screen occlusion and visible
 * region is empty
 * @tc.type: FUNC
 * @tc.require: issue20827
 */
HWTEST_F(RSUniRenderVisitorTest, CheckSkipBackgroundSurfaceRenderNode006, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    RectI rect = RectI(0, 0, 100, 100);
    auto occlusionRegion = Occlusion::Region(rect);
    rsUniRenderVisitor->accumulatedOcclusionRegion_.OrSelf(occlusionRegion);
    auto rsContext = std::make_shared<RSContext>();
    rsUniRenderVisitor->curScreenNode_ = std::make_shared<RSScreenRenderNode>(1, 0, rsContext);
    rsUniRenderVisitor->curScreenNode_->screenInfo_.width = 100;
    rsUniRenderVisitor->curScreenNode_->screenInfo_.height = 100;
    auto isQuickSkip = rsUniRenderVisitor->CheckSkipBackgroundSurfaceRenderNode(*surfaceNode);
    ASSERT_TRUE(isQuickSkip);
}

/*
 * @tc.name: CheckSkipAndUpdateForegroundSurfaceRenderNode001
 * @tc.desc: Test RSUniRenderVisitorTest.CheckSkipAndUpdateForegroundSurfaceRenderNode while surfaceWindowType_ is
 * SCB_SCREEN_LOCK
 * @tc.type: FUNC
 * @tc.require: issue20827
 */
HWTEST_F(RSUniRenderVisitorTest, CheckSkipAndUpdateForegroundSurfaceRenderNode001, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    const_cast<SurfaceWindowType&>(surfaceNode->surfaceWindowType_) = SurfaceWindowType::SCB_SCREEN_LOCK;

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    auto isQuickSkip = rsUniRenderVisitor->CheckSkipAndUpdateForegroundSurfaceRenderNode(*surfaceNode);
    ASSERT_TRUE(!isQuickSkip);
}

/*
 * @tc.name: CheckSkipAndUpdateForegroundSurfaceRenderNode002
 * @tc.desc: Test RSUniRenderVisitorTest.CheckSkipAndUpdateForegroundSurfaceRenderNode while window is opaque
 * @tc.type: FUNC
 * @tc.require: issue20827
 */
HWTEST_F(RSUniRenderVisitorTest, CheckSkipAndUpdateForegroundSurfaceRenderNode002, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    const_cast<SurfaceWindowType&>(surfaceNode->surfaceWindowType_) = SurfaceWindowType::SYSTEM_SCB_WINDOW;
    surfaceNode->abilityBgAlpha_ = 255;
    surfaceNode->globalAlpha_ = 1.0f;

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    auto isQuickSkip = rsUniRenderVisitor->CheckSkipAndUpdateForegroundSurfaceRenderNode(*surfaceNode);
    ASSERT_TRUE(!isQuickSkip);
}

/*
 * @tc.name: CheckSkipAndUpdateForegroundSurfaceRenderNode003
 * @tc.desc: Test RSUniRenderVisitorTest.CheckSkipAndUpdateForegroundSurfaceRenderNode while met skip condition for
 * first time
 * @tc.type: FUNC
 * @tc.require: issue20827
 */
HWTEST_F(RSUniRenderVisitorTest, CheckSkipAndUpdateForegroundSurfaceRenderNode003, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    const_cast<SurfaceWindowType&>(surfaceNode->surfaceWindowType_) = SurfaceWindowType::SCB_GESTURE_BACK;

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    auto isQuickSkip = rsUniRenderVisitor->CheckSkipAndUpdateForegroundSurfaceRenderNode(*surfaceNode);
    ASSERT_TRUE(!isQuickSkip);
}

/*
 * @tc.name: CheckSkipAndUpdateForegroundSurfaceRenderNode004
 * @tc.desc: Test RSUniRenderVisitorTest.CheckSkipAndUpdateForegroundSurfaceRenderNode while met skip, and child has no
 * visible filter
 * @tc.type: FUNC
 * @tc.require: issue20827
 */
HWTEST_F(RSUniRenderVisitorTest, CheckSkipAndUpdateForegroundSurfaceRenderNode004, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetStableSkipReached(true);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    auto isQuickSkip = rsUniRenderVisitor->CheckSkipAndUpdateForegroundSurfaceRenderNode(*surfaceNode);
    ASSERT_TRUE(isQuickSkip);
}

/*
 * @tc.name: CheckSkipAndUpdateForegroundSurfaceRenderNode005
 * @tc.desc: Test RSUniRenderVisitorTest.CheckSkipAndUpdateForegroundSurfaceRenderNode while parent tree is dirty
 * @tc.type: FUNC
 * @tc.require: issue20827
 */
HWTEST_F(RSUniRenderVisitorTest, CheckSkipAndUpdateForegroundSurfaceRenderNode005, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetParentTreeDirty(true);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    auto isQuickSkip = rsUniRenderVisitor->CheckSkipAndUpdateForegroundSurfaceRenderNode(*surfaceNode);
    ASSERT_TRUE(isQuickSkip);
}

/*
 * @tc.name: CheckQuickSkipSurfaceRenderNode001
 * @tc.desc: Test RSUniRenderVisitorTest.CheckQuickSkipSurfaceRenderNode while surface node is not app window or has sub
 * surface node
 * @tc.type: FUNC
 * @tc.require: issue20827
 */
HWTEST_F(RSUniRenderVisitorTest, CheckQuickSkipSurfaceRenderNode001, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    const_cast<SurfaceWindowType&>(surfaceNode->surfaceWindowType_) = SurfaceWindowType::SYSTEM_SCB_WINDOW;

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    auto isQuickSkip = rsUniRenderVisitor->CheckQuickSkipSurfaceRenderNode(*surfaceNode);
    ASSERT_TRUE(!isQuickSkip);

    surfaceNode->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    auto rsContext = std::make_shared<RSContext>();
    auto subNodeTwo = std::make_shared<RSSurfaceRenderNode>(22, rsContext);
    surfaceNode->childSubSurfaceNodes_[subNodeTwo->GetId()] = subNodeTwo;
    isQuickSkip = rsUniRenderVisitor->CheckQuickSkipSurfaceRenderNode(*surfaceNode);
    ASSERT_TRUE(!isQuickSkip);
}

/*
 * @tc.name: CheckQuickSkipSurfaceRenderNode002
 * @tc.desc: Test RSUniRenderVisitorTest.CheckQuickSkipSurfaceRenderNode while surface node is dirty
 * @tc.type: FUNC
 * @tc.require: issue20827
 */
HWTEST_F(RSUniRenderVisitorTest, CheckQuickSkipSurfaceRenderNode002, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    surfaceNode->SetDirty(true);
    const_cast<SurfaceWindowType&>(surfaceNode->surfaceWindowType_) = SurfaceWindowType::SYSTEM_SCB_WINDOW;

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    auto isQuickSkip = rsUniRenderVisitor->CheckQuickSkipSurfaceRenderNode(*surfaceNode);
    ASSERT_TRUE(!isQuickSkip);
}

/*
 * @tc.name: CheckQuickSkipSurfaceRenderNode003
 * @tc.desc: Test RSUniRenderVisitorTest.CheckQuickSkipSurfaceRenderNode while is force prepare
 * @tc.type: FUNC
 * @tc.require: issue20827
 */
HWTEST_F(RSUniRenderVisitorTest, CheckQuickSkipSurfaceRenderNode003, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    surfaceNode->SetForcePrepare(true);
    const_cast<SurfaceWindowType&>(surfaceNode->surfaceWindowType_) = SurfaceWindowType::SYSTEM_SCB_WINDOW;

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    auto isQuickSkip = rsUniRenderVisitor->CheckQuickSkipSurfaceRenderNode(*surfaceNode);
    ASSERT_TRUE(!isQuickSkip);
}

/*
 * @tc.name: CheckQuickSkipSurfaceRenderNode004
 * @tc.desc: Test RSUniRenderVisitorTest.CheckQuickSkipSurfaceRenderNode while background surface node quick skip
 * @tc.type: FUNC
 * @tc.require: issue20827
 */
HWTEST_F(RSUniRenderVisitorTest, CheckQuickSkipSurfaceRenderNode004, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->isBgWindowTraversalStarted_ = true;
    RectI rect = RectI(0, 0, 100, 100);
    auto occlusionRegion = Occlusion::Region(rect);
    rsUniRenderVisitor->accumulatedOcclusionRegion_.OrSelf(occlusionRegion);
    auto rsContext = std::make_shared<RSContext>();
    rsUniRenderVisitor->curScreenNode_ = std::make_shared<RSScreenRenderNode>(1, 0, rsContext);
    rsUniRenderVisitor->curScreenNode_->screenInfo_.width = 100;
    rsUniRenderVisitor->curScreenNode_->screenInfo_.height = 100;
    auto isQuickSkip = rsUniRenderVisitor->CheckQuickSkipSurfaceRenderNode(*surfaceNode);
    ASSERT_TRUE(isQuickSkip);
}

/*
 * @tc.name: CheckQuickSkipSurfaceRenderNode005
 * @tc.desc: Test RSUniRenderVisitorTest.CheckQuickSkipSurfaceRenderNode while check foreground surface node
 * @tc.type: FUNC
 * @tc.require: issue20827
 */
HWTEST_F(RSUniRenderVisitorTest, CheckQuickSkipSurfaceRenderNode005, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    const_cast<SurfaceWindowType&>(surfaceNode->surfaceWindowType_) = SurfaceWindowType::SYSTEM_SCB_WINDOW;

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    auto isQuickSkip = rsUniRenderVisitor->CheckQuickSkipSurfaceRenderNode(*surfaceNode);
    ASSERT_FALSE(isQuickSkip);

    surfaceNode->SetStableSkipReached(true);
    isQuickSkip = rsUniRenderVisitor->CheckQuickSkipSurfaceRenderNode(*surfaceNode);
    ASSERT_TRUE(isQuickSkip);
}

/*
 * @tc.name: CheckQuickSkipSurfaceRenderNode006
 * @tc.desc: Test RSUniRenderVisitorTest.CheckQuickSkipSurfaceRenderNode while surfaceWindowType_ is DEFAULT_WINDOW
 * @tc.type: FUNC
 * @tc.require: issue20827
 */
HWTEST_F(RSUniRenderVisitorTest, CheckQuickSkipSurfaceRenderNode006, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    auto isQuickSkip = rsUniRenderVisitor->CheckQuickSkipSurfaceRenderNode(*surfaceNode);
    ASSERT_TRUE(!isQuickSkip);
}

/*
 * @tc.name: CheckQuickSkipSurfaceRenderNode007
 * @tc.desc: Test RSUniRenderVisitorTest.CheckQuickSkipSurfaceRenderNode while surfaceWindowType_ is SCB_DESKTOP
 * @tc.type: FUNC
 * @tc.require: issue20827
 */
HWTEST_F(RSUniRenderVisitorTest, CheckQuickSkipSurfaceRenderNode007, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    const_cast<SurfaceWindowType&>(surfaceNode->surfaceWindowType_) = SurfaceWindowType::SCB_DESKTOP;

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    auto isQuickSkip = rsUniRenderVisitor->CheckQuickSkipSurfaceRenderNode(*surfaceNode);
    ASSERT_TRUE(!isQuickSkip);
}

/*
 * @tc.name: CheckQuickSkipSurfaceRenderNode008
 * @tc.desc: Test RSUniRenderVisitorTest.CheckQuickSkipSurfaceRenderNode while surfaceWindowType_ is SCB_DROPDOWN_PANEL
 * @tc.type: FUNC
 * @tc.require: issue20827
 */
HWTEST_F(RSUniRenderVisitorTest, CheckQuickSkipSurfaceRenderNode008, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    const_cast<SurfaceWindowType&>(surfaceNode->surfaceWindowType_) = SurfaceWindowType::SCB_DROPDOWN_PANEL;

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    auto isQuickSkip = rsUniRenderVisitor->CheckQuickSkipSurfaceRenderNode(*surfaceNode);
    ASSERT_TRUE(!isQuickSkip);
}

/*
 * @tc.name: CheckQuickSkipSurfaceRenderNode009
 * @tc.desc: Test RSUniRenderVisitorTest.CheckQuickSkipSurfaceRenderNode while has sub hardware enabled node
 * @tc.type: FUNC
 * @tc.require: issue20827
 */
HWTEST_F(RSUniRenderVisitorTest, CheckQuickSkipSurfaceRenderNode009, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    const_cast<SurfaceWindowType&>(surfaceNode->surfaceWindowType_) = SurfaceWindowType::SYSTEM_SCB_WINDOW;
    surfaceNode->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);

    auto id = surfaceNode->GetId();
    std::weak_ptr<RSSurfaceRenderNode> childNode = std::make_shared<RSSurfaceRenderNode>(id + 1);
    surfaceNode->AddChildHardwareEnabledNode(childNode);
    ASSERT_FALSE(surfaceNode->GetChildHardwareEnabledNodes().empty());

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    auto isQuickSkip = rsUniRenderVisitor->CheckQuickSkipSurfaceRenderNode(*surfaceNode);
    ASSERT_FALSE(isQuickSkip);
}

/*
 * @tc.name: CheckQuickSkipSurfaceRenderNode0010
 * @tc.desc: Test RSUniRenderVisitorTest.CheckQuickSkipSurfaceRenderNode while foreground skip but
 * curScreenNode_ is nullptr, and child has visible filter but curDirtyManager is nullptr
 * @tc.type: FUNC
 * @tc.require: issue20827
 */
HWTEST_F(RSUniRenderVisitorTest, CheckQuickSkipSurfaceRenderNode0010, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    const_cast<SurfaceWindowType&>(surfaceNode->surfaceWindowType_) = SurfaceWindowType::SYSTEM_SCB_WINDOW;
    surfaceNode->SetStableSkipReached(true);
    surfaceNode->childHasVisibleFilter_ = true;
    surfaceNode->dirtyManager_ = nullptr;

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->curScreenNode_ = nullptr;

    auto isQuickSkip = rsUniRenderVisitor->CheckQuickSkipSurfaceRenderNode(*surfaceNode);
    ASSERT_TRUE(isQuickSkip);
}

/*
 * @tc.name: CheckQuickSkipSurfaceRenderNode0011
 * @tc.desc: Test RSUniRenderVisitorTest.CheckQuickSkipSurfaceRenderNode while foreground skip, and child has
 * visible filter
 * @tc.type: FUNC
 * @tc.require: issue20827
 */
HWTEST_F(RSUniRenderVisitorTest, CheckQuickSkipSurfaceRenderNode0011, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    const_cast<SurfaceWindowType&>(surfaceNode->surfaceWindowType_) = SurfaceWindowType::SYSTEM_SCB_WINDOW;
    surfaceNode->SetStableSkipReached(true);
    surfaceNode->childHasVisibleFilter_ = true;
    surfaceNode->dirtyManager_ = std::make_shared<RSDirtyRegionManager>();

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    auto rsContext = std::make_shared<RSContext>();
    rsUniRenderVisitor->curScreenNode_ = std::make_shared<RSScreenRenderNode>(1, 0, rsContext);

    auto isQuickSkip = rsUniRenderVisitor->CheckQuickSkipSurfaceRenderNode(*surfaceNode);
    ASSERT_TRUE(isQuickSkip);
}

/*
 * @tc.name: UpdateFilterRegionInSkippedSurfaceNode
 * @tc.desc: Test RSUniRenderVisitorTest.UpdateFilterRegionInSkippedSurfaceNode
 * @tc.type: FUNC
 * @tc.require: issue20827
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateFilterRegionInSkippedSurfaceNode, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    auto rsRootRenderNode = std::make_shared<RSSurfaceRenderNode>(0, rsContext->weak_from_this());
    ASSERT_NE(rsRootRenderNode, nullptr);
    rsRootRenderNode->InitRenderParams();

    auto canvasNode = std::make_shared<RSCanvasRenderNode>(1);
    ASSERT_NE(canvasNode, nullptr);
    canvasNode->GetMutableRenderProperties().backgroundFilter_ = std::make_shared<RSFilter>();
    canvasNode->GetMutableRenderProperties().needFilter_ = true;
    rsRootRenderNode->UpdateVisibleFilterChild(*canvasNode);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    RSDirtyRegionManager dirtyManager;
    rsUniRenderVisitor->UpdateFilterRegionInSkippedSurfaceNode(*rsRootRenderNode, dirtyManager);

    RSMainThread::Instance()->GetContext().GetMutableNodeMap().RegisterRenderNode(canvasNode);
    rsUniRenderVisitor->UpdateFilterRegionInSkippedSurfaceNode(*rsRootRenderNode, dirtyManager);
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
    config.id = 10; // 10 non-zero dummy configId
    auto rsCanvasRenderNode = std::make_shared<RSCanvasRenderNode>(1, rsContext->weak_from_this());
    ASSERT_NE(rsCanvasRenderNode, nullptr);
    rsCanvasRenderNode->InitRenderParams();
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    rsSurfaceRenderNode->InitRenderParams();
    // 11 non-zero node id
    auto rsDisplayRenderNode = std::make_shared<RSScreenRenderNode>(11, 0, rsContext->weak_from_this());
    ASSERT_NE(rsDisplayRenderNode, nullptr);
    ASSERT_NE(rsDisplayRenderNode->GetDirtyManager(), nullptr);
    rsDisplayRenderNode->InitRenderParams();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();

    // set surface to transparent, add a canvas node to create a transparent dirty region
    rsSurfaceRenderNode->SetAbilityBGAlpha(0);
    // 80, 2560, 1600. dummy value  used to create rectangle with non-zero dimension
    rsSurfaceRenderNode->SetSrcRect(RectI(0, 80, 2560, 1600));
    rsDisplayRenderNode->AddChild(rsSurfaceRenderNode, -1);
    rsSurfaceRenderNode->AddChild(rsCanvasRenderNode, -1);

    auto screenManager = CreateOrGetScreenManager();
    auto screenId = CreateVirtualScreen(screenManager);
    ASSERT_NE(screenId, INVALID_SCREEN_ID);
    
    rsDisplayRenderNode->stagingRenderParams_ = std::make_unique<RSScreenRenderParams>(screenId);
    ASSERT_NE(rsDisplayRenderNode->stagingRenderParams_, nullptr);
    rsUniRenderVisitor->curScreenNode_ = rsDisplayRenderNode;

    rsUniRenderVisitor->InitScreenInfo(*rsDisplayRenderNode);

    rsUniRenderVisitor->CheckMergeSurfaceDirtysForDisplay(rsSurfaceRenderNode);
    ASSERT_EQ(rsUniRenderVisitor->curScreenDirtyManager_->dirtyRegion_.left_, 0);
    screenManager->RemoveVirtualScreen(screenId);
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

    auto rsContext = std::make_shared<RSContext>();
    auto rsDisplayRenderNode = std::make_shared<RSScreenRenderNode>(5, 0, rsContext->weak_from_this());
    ASSERT_NE(rsDisplayRenderNode, nullptr);
    ASSERT_NE(rsDisplayRenderNode->GetDirtyManager(), nullptr);
    rsDisplayRenderNode->InitRenderParams();
    auto screenManager = CreateOrGetScreenManager();
    auto screenId = CreateVirtualScreen(screenManager);
    ASSERT_NE(screenId, INVALID_SCREEN_ID);
    
    rsDisplayRenderNode->stagingRenderParams_ = std::make_unique<RSScreenRenderParams>(screenId);
    ASSERT_NE(rsDisplayRenderNode->stagingRenderParams_, nullptr);
    rsUniRenderVisitor->curScreenNode_ = rsDisplayRenderNode;
    rsUniRenderVisitor->QuickPrepareScreenRenderNode(*rsDisplayRenderNode);
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
    screenManager->RemoveVirtualScreen(screenId);
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
    config.id = 10; // 10 non-zero dummy configId
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    ASSERT_NE(rsSurfaceRenderNode->GetDirtyManager(), nullptr);
    rsSurfaceRenderNode->InitRenderParams();
    // 11 non-zero node id
    auto rsDisplayRenderNode = std::make_shared<RSScreenRenderNode>(11, 0, rsContext->weak_from_this());
    ASSERT_NE(rsDisplayRenderNode, nullptr);
    ASSERT_NE(rsDisplayRenderNode->GetDirtyManager(), nullptr);
    rsDisplayRenderNode->InitRenderParams();
    rsDisplayRenderNode->AddChild(rsSurfaceRenderNode, -1);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->curScreenNode_ = rsDisplayRenderNode;
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
    config.id = 10; // 10 non-zero dummy configId
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    ASSERT_NE(rsSurfaceRenderNode->GetDirtyManager(), nullptr);
    rsSurfaceRenderNode->InitRenderParams();
    // 11 non-zero node id
    auto rsDisplayRenderNode = std::make_shared<RSScreenRenderNode>(11, 0, rsContext->weak_from_this());
    ASSERT_NE(rsDisplayRenderNode, nullptr);
    ASSERT_NE(rsDisplayRenderNode->GetDirtyManager(), nullptr);
    rsDisplayRenderNode->InitRenderParams();
    rsDisplayRenderNode->AddChild(rsSurfaceRenderNode, -1);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->curScreenNode_ = rsDisplayRenderNode;
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
    config.id = 10; // 10 non-zero dummy configId
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    ASSERT_NE(rsSurfaceRenderNode->GetDirtyManager(), nullptr);
    rsSurfaceRenderNode->InitRenderParams();
    // 11 non-zero node id
    auto rsDisplayRenderNode = std::make_shared<RSScreenRenderNode>(11, 0, rsContext->weak_from_this());
    ASSERT_NE(rsDisplayRenderNode, nullptr);
    ASSERT_NE(rsDisplayRenderNode->GetDirtyManager(), nullptr);
    rsDisplayRenderNode->InitRenderParams();
    rsDisplayRenderNode->AddChild(rsSurfaceRenderNode, -1);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->curScreenNode_ = rsDisplayRenderNode;
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
    config.id = 10; // 10 non-zero dummy configId
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    ASSERT_NE(rsSurfaceRenderNode->GetDirtyManager(), nullptr);
    rsSurfaceRenderNode->InitRenderParams();
    // 11 non-zero node id
    auto rsDisplayRenderNode = std::make_shared<RSScreenRenderNode>(11, 0, rsContext->weak_from_this());
    ASSERT_NE(rsDisplayRenderNode, nullptr);
    ASSERT_NE(rsDisplayRenderNode->GetDirtyManager(), nullptr);
    rsDisplayRenderNode->InitRenderParams();
    rsDisplayRenderNode->AddChild(rsSurfaceRenderNode, -1);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->curScreenNode_ = rsDisplayRenderNode;
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
    config.id = 10;
    auto rsCanvasRenderNode = std::make_shared<RSCanvasRenderNode>(1, rsContext->weak_from_this());
    ASSERT_NE(rsCanvasRenderNode, nullptr);
    rsCanvasRenderNode->InitRenderParams();
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    rsSurfaceRenderNode->InitRenderParams();
    // 11 non-zero node id
    auto rsDisplayRenderNode = std::make_shared<RSScreenRenderNode>(11, 0, rsContext->weak_from_this());
    ASSERT_NE(rsDisplayRenderNode, nullptr);
    rsDisplayRenderNode->InitRenderParams();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();

    // set surface to transparent, add a canvas node to create a transparent dirty region
    rsSurfaceRenderNode->SetAbilityBGAlpha(0);
    // 80, 2560, 1600. dummy value used to create rectangle with non-zero dimension
    rsSurfaceRenderNode->SetSrcRect(RectI(0, 80, 2560, 1600));
    rsDisplayRenderNode->AddChild(rsSurfaceRenderNode, -1);
    rsSurfaceRenderNode->AddChild(rsCanvasRenderNode, -1);
    auto screenManager = CreateOrGetScreenManager();
    auto screenId = CreateVirtualScreen(screenManager);
    ASSERT_NE(screenId, INVALID_SCREEN_ID);
    
    rsDisplayRenderNode->stagingRenderParams_ = std::make_unique<RSScreenRenderParams>(screenId);
    ASSERT_NE(rsDisplayRenderNode->stagingRenderParams_, nullptr);
    rsUniRenderVisitor->curScreenNode_ = rsDisplayRenderNode;

    rsUniRenderVisitor->QuickPrepareScreenRenderNode(*rsDisplayRenderNode);
    rsUniRenderVisitor->InitScreenInfo(*rsDisplayRenderNode);

    rsUniRenderVisitor->UpdateSurfaceDirtyAndGlobalDirty();
    ASSERT_EQ(rsUniRenderVisitor->curScreenDirtyManager_->dirtyRegion_.left_, 0);
    screenManager->RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: InitScreenInfo001
 * @tc.desc: Verify that the InitScreenInfo method correctly sets the needRecalculateOcclusion_ flag
 *           under different blacklist and whitelist configurations.
 * @tc.type: FUNC
 * @tc.require: issueICXRGY
 */
HWTEST_F(RSUniRenderVisitorTest, InitScreenInfo001, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    constexpr NodeId defaultNodeId{1};
    auto rsScreenRenderNode = std::make_shared<RSScreenRenderNode>(defaultNodeId, 0, rsContext->weak_from_this());
    rsScreenRenderNode->InitRenderParams();

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->curScreenNode_ = rsScreenRenderNode;
    rsUniRenderVisitor->screenManager_ = CreateOrGetScreenManager();
    ASSERT_NE(rsUniRenderVisitor->screenManager_, nullptr);

    rsUniRenderVisitor->allBlackList_ = rsUniRenderVisitor->screenManager_->GetAllBlackList();
    rsUniRenderVisitor->allWhiteList_ = rsUniRenderVisitor->screenManager_->GetAllWhiteList();
    rsUniRenderVisitor->InitScreenInfo(*rsScreenRenderNode);
    EXPECT_EQ(rsUniRenderVisitor->needRecalculateOcclusion_, false);

    constexpr NodeId nodeId{std::numeric_limits<NodeId>::max()};
    rsUniRenderVisitor->allBlackList_.emplace(nodeId);
    rsUniRenderVisitor->InitScreenInfo(*rsScreenRenderNode);
    EXPECT_EQ(rsUniRenderVisitor->needRecalculateOcclusion_, true);

    rsUniRenderVisitor->allBlackList_ = rsUniRenderVisitor->screenManager_->GetAllBlackList();
    rsUniRenderVisitor->allWhiteList_.emplace(nodeId);
    rsUniRenderVisitor->InitScreenInfo(*rsScreenRenderNode);
    rsUniRenderVisitor->allBlackList_.clear();
    rsUniRenderVisitor->allWhiteList_.clear();
    EXPECT_EQ(rsUniRenderVisitor->needRecalculateOcclusion_, true);
}

/**
 * @tc.name: BeforeUpdateSurfaceDirtyCalc001
 * @tc.desc: Test BeforeUpdateSurfaceDirtyCalc with empty node
 * @tc.type: FUNC
 * @tc.require: issueIABP1V
 */
HWTEST_F(RSUniRenderVisitorTest, BeforeUpdateSurfaceDirtyCalc001, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    auto rsDisplayRenderNode = std::make_shared<RSScreenRenderNode>(1, 0, rsContext->weak_from_this());
    ASSERT_NE(rsDisplayRenderNode, nullptr);
    rsDisplayRenderNode->InitRenderParams();

    auto node = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(node, nullptr);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto screenManager = CreateOrGetScreenManager();
    auto screenId = CreateVirtualScreen(screenManager);
    ASSERT_NE(screenId, INVALID_SCREEN_ID);
    
    rsDisplayRenderNode->stagingRenderParams_ = std::make_unique<RSScreenRenderParams>(screenId);
    ASSERT_NE(rsDisplayRenderNode->stagingRenderParams_, nullptr);
    rsUniRenderVisitor->curScreenNode_ = rsDisplayRenderNode;
    rsUniRenderVisitor->InitScreenInfo(*rsDisplayRenderNode);

    ASSERT_TRUE(rsUniRenderVisitor->BeforeUpdateSurfaceDirtyCalc(*node));
    screenManager->RemoveVirtualScreen(screenId);
}

/**
 * @tc.name: BeforeUpdateSurfaceDirtyCalc002
 * @tc.desc: Test BeforeUpdateSurfaceDirtyCalc with nonEmpty node
 * @tc.type: FUNC
 * @tc.require: issueIABP1V
 */
HWTEST_F(RSUniRenderVisitorTest, BeforeUpdateSurfaceDirtyCalc002, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    auto rsDisplayRenderNode = std::make_shared<RSScreenRenderNode>(1, 0, rsContext->weak_from_this());
    ASSERT_NE(rsDisplayRenderNode, nullptr);
    rsDisplayRenderNode->InitRenderParams();

    auto node = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(node, nullptr);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto screenManager = CreateOrGetScreenManager();
    auto screenId = CreateVirtualScreen(screenManager);
    ASSERT_NE(screenId, INVALID_SCREEN_ID);
    
    rsDisplayRenderNode->stagingRenderParams_ = std::make_unique<RSScreenRenderParams>(screenId);
    ASSERT_NE(rsDisplayRenderNode->stagingRenderParams_, nullptr);
    rsUniRenderVisitor->curScreenNode_ = rsDisplayRenderNode;
    rsUniRenderVisitor->InitScreenInfo(*rsDisplayRenderNode);

    node->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);
    ASSERT_TRUE(rsUniRenderVisitor->BeforeUpdateSurfaceDirtyCalc(*node));
    node->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    ASSERT_TRUE(rsUniRenderVisitor->BeforeUpdateSurfaceDirtyCalc(*node));
    node->SetNodeName("CapsuleWindow");
    ASSERT_TRUE(rsUniRenderVisitor->BeforeUpdateSurfaceDirtyCalc(*node));
    screenManager->RemoveVirtualScreen(screenId);
}

/**
 * @tc.name: BeforeUpdateSurfaceDirtyCalc003
 * @tc.desc: Test BeforeUpdateSurfaceDirtyCalc with nonEmpty node
 * @tc.type: FUNC
 * @tc.require: issueIABP1V
 */
HWTEST_F(RSUniRenderVisitorTest, BeforeUpdateSurfaceDirtyCalc003, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    auto rsDisplayRenderNode = std::make_shared<RSScreenRenderNode>(1, 0, rsContext->weak_from_this());
    ASSERT_NE(rsDisplayRenderNode, nullptr);
    rsDisplayRenderNode->InitRenderParams();

    auto node = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(node, nullptr);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto screenManager = CreateOrGetScreenManager();
    auto screenId = CreateVirtualScreen(screenManager);
    ASSERT_NE(screenId, INVALID_SCREEN_ID);
    
    rsDisplayRenderNode->stagingRenderParams_ = std::make_unique<RSLogicalDisplayRenderParams>(screenId);
    ASSERT_NE(rsDisplayRenderNode->stagingRenderParams_, nullptr);
    rsUniRenderVisitor->curScreenNode_ = rsDisplayRenderNode;
    rsUniRenderVisitor->InitScreenInfo(*rsDisplayRenderNode);

    bool isBufferReclaimEnable = BufferReclaimParam::GetInstance().IsBufferReclaimEnable();
    auto surfaceHandle = node->GetMutableRSSurfaceHandler();
    ASSERT_NE(surfaceHandle, nullptr);

    sptr<SurfaceBuffer> buffer = new SurfaceBufferImpl();
    BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
        .colorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3,
    };
    GSError ret = buffer->Alloc(requestConfig);
    ASSERT_EQ(ret, GSERROR_OK);
    surfaceHandle->SetBuffer(buffer, SyncFence::INVALID_FENCE, Rect(), 0);
    surfaceHandle->SetLastBufferId(buffer->GetBufferId());
    surfaceHandle->lastBufferReclaimNum_ = 4;
    EXPECT_TRUE(surfaceHandle->ReclaimLastBufferPrepare());
    EXPECT_TRUE(surfaceHandle->ReclaimLastBufferProcess());
    EXPECT_TRUE(buffer->IsReclaimed());


    node->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);
    BufferReclaimParam::GetInstance().SetBufferReclaimEnable(false);
    ASSERT_TRUE(rsUniRenderVisitor->BeforeUpdateSurfaceDirtyCalc(*node));
    EXPECT_TRUE(buffer->IsReclaimed());

    node->name_ = "RosenWeb";
    EXPECT_TRUE(node->IsRosenWeb());
    BufferReclaimParam::GetInstance().SetBufferReclaimEnable(true);
    ASSERT_TRUE(rsUniRenderVisitor->BeforeUpdateSurfaceDirtyCalc(*node));
    EXPECT_FALSE(buffer->IsReclaimed());

    screenManager->RemoveVirtualScreen(screenId);
    BufferReclaimParam::GetInstance().SetBufferReclaimEnable(isBufferReclaimEnable);
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
 * @tc.name: CalculateOpaqueAndTransparentRegion001
 * @tc.desc: Test CalculateOpaqueAndTransparentRegion with empty node
 * @tc.type: FUNC
 * @tc.require: issueIBCR0E
 */
HWTEST_F(RSUniRenderVisitorTest, CalculateOpaqueAndTransparentRegion001, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    config.id = 10;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    rsSurfaceRenderNode->InitRenderParams();
    // 11 non-zero node id
    auto rsDisplayRenderNode = std::make_shared<RSScreenRenderNode>(11, 0, rsContext->weak_from_this());
    rsDisplayRenderNode->InitRenderParams();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    auto screenManager = CreateOrGetScreenManager();
    auto screenId = CreateVirtualScreen(screenManager);
    ASSERT_NE(screenId, INVALID_SCREEN_ID);
    
    rsDisplayRenderNode->stagingRenderParams_ = std::make_unique<RSScreenRenderParams>(screenId);
    ASSERT_NE(rsDisplayRenderNode->stagingRenderParams_, nullptr);
    rsUniRenderVisitor->curScreenNode_ = rsDisplayRenderNode;
    rsUniRenderVisitor->InitScreenInfo(*rsDisplayRenderNode);

    rsUniRenderVisitor->CalculateOpaqueAndTransparentRegion(*rsSurfaceRenderNode);
    ASSERT_FALSE(rsUniRenderVisitor->needRecalculateOcclusion_);
    screenManager->RemoveVirtualScreen(screenId);
}

/**
 * @tc.name: QuickPrepareScreenRenderNode004
 * @tc.desc: Test QuickPrepareScreenRenderNode with display node
 * @tc.type: FUNC
 * @tc.require: issueI9RR2Y
 */
HWTEST_F(RSUniRenderVisitorTest, QuickPrepareScreenRenderNode004, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    // 11 non-zero node id
    auto rsDisplayRenderNode = std::make_shared<RSScreenRenderNode>(11, 0, rsContext->weak_from_this());
    rsDisplayRenderNode->InitRenderParams();

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();

    auto screenManager = CreateOrGetScreenManager();
    auto screenId = CreateVirtualScreen(screenManager);
    ASSERT_NE(screenId, INVALID_SCREEN_ID);
    
    rsDisplayRenderNode->stagingRenderParams_ = std::make_unique<RSScreenRenderParams>(screenId);
    ASSERT_NE(rsDisplayRenderNode->stagingRenderParams_, nullptr);
    rsUniRenderVisitor->curScreenNode_ = rsDisplayRenderNode;

    rsUniRenderVisitor->QuickPrepareScreenRenderNode(*rsDisplayRenderNode);
    ASSERT_FALSE(rsUniRenderVisitor->ancestorNodeHasAnimation_);
    screenManager->RemoveVirtualScreen(screenId);
}

/**
 * @tc.name: InitLogicalDisplayInfo001
 * @tc.desc: Test InitLogicalDisplayInfo
 * @tc.type: FUNC
 * @tc.require: issueICPT5N
 */
HWTEST_F(RSUniRenderVisitorTest, InitLogicalDisplayInfo001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    NodeId screenNodeId = 1;
    auto rsContext = std::make_shared<RSContext>();
    rsUniRenderVisitor->curScreenNode_ = std::make_shared<RSScreenRenderNode>(screenNodeId, 0, rsContext);
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSLogicalDisplayRenderNode>(id, config);

    rsUniRenderVisitor->InitLogicalDisplayInfo(*displayNode);
    EXPECT_EQ(rsUniRenderVisitor->occlusionSurfaceOrder_, TOP_OCCLUSION_SURFACES_NUM);
}

/**
 * @tc.name: UpdateVirtualDisplayInfo001
 * @tc.desc: Test UpdateVirtualDisplayInfo for no-mirror display
 * @tc.type: FUNC
 * @tc.require: issueIB1YAT
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateVirtualDisplayInfo001, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    RSDisplayNodeConfig config;
    // 11 non-zero node id
    auto rsDisplayRenderNode = std::make_shared<RSLogicalDisplayRenderNode>(11, config, rsContext->weak_from_this());
    rsDisplayRenderNode->InitRenderParams();
    ASSERT_EQ(rsDisplayRenderNode->IsMirrorDisplay(), false);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->UpdateVirtualDisplayInfo(*rsDisplayRenderNode);
}

/**
 * @tc.name: UpdateVirtualDisplayInfo002
 * @tc.desc: Test UpdateVirtualDisplayInfo for mirror display with abnormal params
 * @tc.type: FUNC
 * @tc.require: issueIB1YAT
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateVirtualDisplayInfo002, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    RSDisplayNodeConfig config;
    // 11 non-zero node id
    auto rsDisplayRenderNode = std::make_shared<RSLogicalDisplayRenderNode>(11, config, rsContext->weak_from_this());
    rsDisplayRenderNode->InitRenderParams();
    rsDisplayRenderNode->SetIsMirrorDisplay(true);
    ASSERT_EQ(rsDisplayRenderNode->GetMirrorSource().lock(), nullptr);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    auto screenManager = CreateOrGetScreenManager();
    auto screenId = CreateVirtualScreen(screenManager);
    rsUniRenderVisitor->curScreenNode_ = std::make_shared<RSScreenRenderNode>(screenId, 0, rsContext);
    rsUniRenderVisitor->curScreenNode_->InitRenderParams();
    ASSERT_EQ(rsUniRenderVisitor->screenManager_, nullptr);
    rsUniRenderVisitor->UpdateVirtualDisplayInfo(*rsDisplayRenderNode);

    // 12 non-zero node id
    auto mirrorNode = std::make_shared<RSLogicalDisplayRenderNode>(12, config, rsContext->weak_from_this());
    rsDisplayRenderNode->mirrorSource_ = mirrorNode;
    rsUniRenderVisitor->UpdateVirtualDisplayInfo(*rsDisplayRenderNode);
}

/**
 * @tc.name: UpdateVirtualDisplayInfo003
 * @tc.desc: Test UpdateVirtualDisplayInfo for mirror display with normal params
 * @tc.type: FUNC
 * @tc.require: issueIB1YAT
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateVirtualDisplayInfo003, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    RSDisplayNodeConfig displayConfig;
    auto screenManager = CreateOrGetScreenManager();
    // 480, 320 width and height for test
    auto id = screenManager->CreateVirtualScreen("virtualScreen01", 480, 320, nullptr);
    // 480, 320 width and height for test
    auto mirrorId = screenManager->CreateVirtualScreen("virtualScreen02", 480, 320, nullptr);

    displayConfig.screenId = id;
    auto rsDisplayRenderNode = std::make_shared<RSLogicalDisplayRenderNode>(
        id, displayConfig, rsContext->weak_from_this());
    rsDisplayRenderNode->InitRenderParams();
    rsDisplayRenderNode->SetIsMirrorDisplay(true);

    displayConfig.screenId = mirrorId;
    auto mirrorNode = std::make_shared<RSLogicalDisplayRenderNode>(
        mirrorId, displayConfig, rsContext->weak_from_this());
    mirrorNode->AddSecurityLayer(1);  // layerId for test
    mirrorNode->AddSecurityLayer(2);  // layerId for test
    mirrorNode->AddSecurityVisibleLayer(1);  // layerId for test
    mirrorNode->AddSecurityVisibleLayer(2);  // layerId for test
    rsDisplayRenderNode->mirrorSource_ = mirrorNode;

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();

    auto screenId = CreateVirtualScreen(screenManager);
    rsUniRenderVisitor->curScreenNode_ = std::make_shared<RSScreenRenderNode>(screenId, 0, rsContext);
    rsUniRenderVisitor->curScreenNode_->InitRenderParams();

    rsUniRenderVisitor->screenManager_ = screenManager;
    ASSERT_NE(rsUniRenderVisitor->screenManager_, nullptr);
    rsUniRenderVisitor->UpdateVirtualDisplayInfo(*rsDisplayRenderNode);

    screenManager->SetVirtualScreenSecurityExemptionList(id, {1});
    rsUniRenderVisitor->UpdateVirtualDisplayInfo(*rsDisplayRenderNode);

    screenManager->SetVirtualScreenSecurityExemptionList(id, {1, 2});  // layerId for test
    rsUniRenderVisitor->UpdateVirtualDisplayInfo(*rsDisplayRenderNode);

    screenManager->SetVirtualScreenSecurityExemptionList(id, {1, 2, 3});  // layerId for test
    rsUniRenderVisitor->UpdateVirtualDisplayInfo(*rsDisplayRenderNode);

    screenManager->SetVirtualScreenSecurityExemptionList(id, {1, 3});  // layerId for test
    rsUniRenderVisitor->UpdateVirtualDisplayInfo(*rsDisplayRenderNode);

    screenManager->SetMirrorScreenVisibleRect(id, {0, 0, 720, 1280});  // rect for test
    rsUniRenderVisitor->UpdateVirtualDisplayInfo(*rsDisplayRenderNode);
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
    NodeId screenNodeId = 3;
    auto rsContext = std::make_shared<RSContext>();
    rsUniRenderVisitor->curScreenNode_ = std::make_shared<RSScreenRenderNode>(screenNodeId, 0, rsContext);
    rsUniRenderVisitor->curScreenNode_->InitRenderParams();
    ASSERT_NE(rsUniRenderVisitor->curScreenNode_, nullptr);
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
    NodeId screenNodeId = 3;
    auto rsContext = std::make_shared<RSContext>();
    rsUniRenderVisitor->curScreenNode_ = std::make_shared<RSScreenRenderNode>(screenNodeId, 0, rsContext);
    rsUniRenderVisitor->curScreenNode_->InitRenderParams();
    ASSERT_NE(rsUniRenderVisitor->curScreenNode_, nullptr);
    rsUniRenderVisitor->CheckMergeDisplayDirtyByTransparentRegions(*surfaceNode);
}

/**
 * @tc.name: CollectFilterInCrossDisplayWindow
 * @tc.desc: Test CollectFilterInCrossDisplayWindow, if filter is non-transparent, collect to global filter.
 * @tc.type: FUNC
 * @tc.require: issueIB670G
 */
HWTEST_F(RSUniRenderVisitorTest, CollectFilterInCrossDisplayWindow_001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsSurfaceRenderNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    // set surface node fully opaque.
    rsSurfaceRenderNode->globalAlpha_ = 1.f;
    rsSurfaceRenderNode->abilityBgAlpha_ = MAX_ALPHA;
    NodeId id = 1;
    auto rsContext = std::make_shared<RSContext>();
    rsUniRenderVisitor->curScreenNode_ = std::make_shared<RSScreenRenderNode>(id, 0, rsContext);
    rsUniRenderVisitor->curScreenDirtyManager_ = std::make_shared<RSDirtyRegionManager>();
    // add filter node to parent surface.
    auto filterNode = std::make_shared<RSCanvasRenderNode>(++id);
    filterNode->absDrawRect_ = DEFAULT_FILTER_RECT;
    auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();
    nodeMap.renderNodeMap_.clear();
    nodeMap.RegisterRenderNode(filterNode);
    rsSurfaceRenderNode->visibleFilterChild_.push_back(filterNode->GetId());

    auto accumDirtyRegion = Occlusion::Region();
    // if node is not cross display, nothing collected.
    rsUniRenderVisitor->CollectFilterInCrossDisplayWindow(rsSurfaceRenderNode, accumDirtyRegion);
    ASSERT_TRUE(
        rsUniRenderVisitor->curScreenDirtyManager_->GetFilterCollector().GetFilterDirtyRegionInfoList(true).empty());

    // if node is cross display, collect to global filter.
    rsSurfaceRenderNode->SetFirstLevelCrossNode(true);
    rsUniRenderVisitor->curScreenNode_->SetIsFirstVisitCrossNodeDisplay(false);
    rsUniRenderVisitor->CollectFilterInCrossDisplayWindow(rsSurfaceRenderNode, accumDirtyRegion);
    ASSERT_FALSE(
        rsUniRenderVisitor->curScreenDirtyManager_->GetFilterCollector().GetFilterDirtyRegionInfoList(true).empty());

    rsSurfaceRenderNode->SetFirstLevelCrossNode(false);
    rsUniRenderVisitor->CollectFilterInCrossDisplayWindow(rsSurfaceRenderNode, accumDirtyRegion);

    rsSurfaceRenderNode->SetFirstLevelCrossNode(true);
    rsUniRenderVisitor->curScreenNode_->SetIsFirstVisitCrossNodeDisplay(true);
    rsUniRenderVisitor->CollectFilterInCrossDisplayWindow(rsSurfaceRenderNode, accumDirtyRegion);
}

/**
 * @tc.name: CollectFilterInCrossDisplayWindow
 * @tc.desc: Test CollectFilterInCrossDisplayWindow, if transparent, collect to global dirty or global filter.
 * @tc.type: FUNC
 * @tc.require: issueIB670G
 */
HWTEST_F(RSUniRenderVisitorTest, CollectFilterInCrossDisplayWindow_002, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsSurfaceRenderNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    // set surface node transparent.
    rsSurfaceRenderNode->globalAlpha_ = 0.f;
    rsSurfaceRenderNode->abilityBgAlpha_ = 0;
    NodeId id = 1;
    auto rsContext = std::make_shared<RSContext>();
    rsUniRenderVisitor->curScreenNode_ = std::make_shared<RSScreenRenderNode>(id, 0, rsContext);
    rsUniRenderVisitor->curScreenDirtyManager_ = std::make_shared<RSDirtyRegionManager>();
    // add filter node to parent surface.
    auto filterNode = std::make_shared<RSCanvasRenderNode>(++id);
    filterNode->absDrawRect_ = DEFAULT_FILTER_RECT;
    auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();
    nodeMap.renderNodeMap_.clear();
    nodeMap.RegisterRenderNode(filterNode);
    rsSurfaceRenderNode->visibleFilterChild_.push_back(filterNode->GetId());

    auto accumDirtyRegion = Occlusion::Region(Occlusion::Rect(DEFAULT_FILTER_RECT));

    // if node is cross display, collect to global dirty.
    rsSurfaceRenderNode->SetFirstLevelCrossNode(true);
    rsUniRenderVisitor->curScreenNode_->SetIsFirstVisitCrossNodeDisplay(false);
    rsUniRenderVisitor->CollectFilterInCrossDisplayWindow(rsSurfaceRenderNode, accumDirtyRegion);
    ASSERT_TRUE(
        rsUniRenderVisitor->curScreenDirtyManager_->GetFilterCollector().GetFilterDirtyRegionInfoList(true).empty());
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
    node->SetChildHasVisibleFilter(true);
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
    node->GetMutableRenderProperties().GetEffect().useEffect_ = true;
    rsUniRenderVisitor->CollectEffectInfo(*node);
    EXPECT_TRUE(parent->ChildHasVisibleEffect());
}

/**
 * @tc.name: CollectEffectInfo004
 * @tc.desc: Test RSUnitRenderVisitorTest.CollectEffectInfo with parent node, oldDirtyInSurface is not empty
 * @tc.type: FUNC
 * @tc.require: issueICTQF4
 */
HWTEST_F(RSUniRenderVisitorTest, CollectEffectInfo004, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    constexpr NodeId nodeId = 1;
    constexpr NodeId parentNodeId = 2;
    constexpr NodeId childNodeId = 3;
    constexpr NodeId nodeId2 = 4;
    auto node = std::make_shared<RSRenderNode>(nodeId);
    auto node2 = std::make_shared<RSRenderNode>(nodeId2);
    auto parent = std::make_shared<RSRenderNode>(parentNodeId);
    auto child = std::make_shared<RSRenderNode>(childNodeId);
    node->InitRenderParams();
    node2->InitRenderParams();
    parent->InitRenderParams();
    child->InitRenderParams();
    node->AddChild(child);
    parent->AddChild(node2);
    parent->AddChild(node);
    child->GetMutableRenderProperties().GetEffect().useEffect_ = true;
    child->SetOldDirtyInSurface(RectI(0, 0, 10, 10));
    node->GetMutableRenderProperties().GetEffect().useEffect_ = true;
    node2->childHasVisibleEffect_ = true;
    rsUniRenderVisitor->CollectEffectInfo(*child);
    rsUniRenderVisitor->CollectEffectInfo(*node);
    rsUniRenderVisitor->CollectEffectInfo(*node2);
    EXPECT_TRUE(node->ChildHasVisibleEffect());
    EXPECT_EQ(node->GetVisibleEffectChild().count(childNodeId), 1);
    EXPECT_TRUE(parent->ChildHasVisibleEffect());
    EXPECT_EQ(parent->GetVisibleEffectChild().count(nodeId), 1);
    EXPECT_EQ(parent->GetVisibleEffectChild().count(childNodeId), 1);
}

/**
 * @tc.name: CollectEffectInfo005
 * @tc.desc: Test RSUnitRenderVisitorTest.CollectEffectInfo with parent node, hasHarmonium
 * @tc.type: FUNC
 * @tc.require: issueIAG8BF
 */
HWTEST_F(RSUniRenderVisitorTest, CollectEffectInfo005, TestSize.Level2)
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

    node->GetMutableRenderProperties().hasHarmonium_ = true;
    node->SetOldDirtyInSurface(RectI(0, 0, 10, 10));
    rsUniRenderVisitor->CollectEffectInfo(*node);
    ASSERT_TRUE(parent->ChildHasVisibleEffect());

    parent->SetChildHasVisibleEffect(false);
    node->GetMutableRenderProperties().hasHarmonium_ = false;
    node->SetChildHasVisibleEffect(true);
    rsUniRenderVisitor->CollectEffectInfo(*node);
    ASSERT_TRUE(parent->ChildHasVisibleEffect());

    parent->SetChildHasVisibleEffect(false);
    node->GetMutableRenderProperties().hasHarmonium_ = false;
    node->SetChildHasVisibleEffect(false);
    rsUniRenderVisitor->CollectEffectInfo(*node);
    ASSERT_FALSE(parent->ChildHasVisibleEffect());
}

/**
 * @tc.name: CollectEffectInfo006
 * @tc.desc: Test RSUnitRenderVisitorTest.CollectEffectInfo with parent node, hasHDRContent
 * @tc.type: FUNC
 * @tc.require: issueIAG8BF
 */
HWTEST_F(RSUniRenderVisitorTest, CollectEffectInfo006, TestSize.Level2)
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

    node->SetChildHasVisibleHDRContent(false);
    node->UpdateHDRStatus(HdrStatus::HDR_PHOTO, true);
    rsUniRenderVisitor->CollectEffectInfo(*node);
    EXPECT_TRUE(parent->ChildHasVisibleHDRContent());

    node->SetChildHasVisibleHDRContent(true);
    parent->SetChildHasVisibleHDRContent(false);
    rsUniRenderVisitor->CollectEffectInfo(*node);
    EXPECT_TRUE(parent->ChildHasVisibleHDRContent());
}

/**
 * @tc.name: CollectEffectInfo007
 * @tc.desc: Test RSUnitRenderVisitorTest.CollectEffectInfo with parent node, hasVisibleIlluminated
 * @tc.type: FUNC
 * @tc.require: issueIAG8BF
 */
HWTEST_F(RSUniRenderVisitorTest, CollectEffectInfo007, TestSize.Level2)
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

    RSPointLightManager::Instance()->SetChildHasVisibleIlluminated(parent, false);
    node->GetMutableRenderProperties().SetIlluminatedType(static_cast<int>(IlluminatedType::BORDER_CONTENT));
    node->SetOldDirtyInSurface(RectI(0, 0, 10, 10));
    rsUniRenderVisitor->CollectEffectInfo(*node);
    ASSERT_FALSE(RSPointLightManager::Instance()->GetChildHasVisibleIlluminated(parent));

    RSPointLightManager::Instance()->SetChildHasVisibleIlluminated(parent, false);
    node->GetMutableRenderProperties().SetIlluminatedType(static_cast<int>(IlluminatedType::NONE));
    RSPointLightManager::Instance()->SetChildHasVisibleIlluminated(node, true);
    auto lightSource = std::make_shared<RSRenderNode>(3);
    RSPointLightManager::Instance()->RegisterLightSource(lightSource);
    rsUniRenderVisitor->CollectEffectInfo(*node);
    ASSERT_TRUE(RSPointLightManager::Instance()->GetChildHasVisibleIlluminated(parent));

    RSPointLightManager::Instance()->SetChildHasVisibleIlluminated(parent, false);
    node->GetMutableRenderProperties().SetIlluminatedType(static_cast<int>(IlluminatedType::NONE));
    RSPointLightManager::Instance()->SetChildHasVisibleIlluminated(node, false);
    rsUniRenderVisitor->CollectEffectInfo(*node);
    ASSERT_FALSE(RSPointLightManager::Instance()->GetChildHasVisibleIlluminated(parent));
}

/**
 * @tc.name: CollectEffectInfo008
 * @tc.desc: Test RSUnitRenderVisitorTest.CollectEffectInfo with parent node, SetHasChildExcludedFromNodeGroup
 * @tc.type: FUNC
 * @tc.require: issueIAG8BF
 */
HWTEST_F(RSUniRenderVisitorTest, CollectEffectInfo008, TestSize.Level2)
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

    node->ExcludedFromNodeGroup(false);
    node->SetHasChildExcludedFromNodeGroup(true);
    rsUniRenderVisitor->CollectEffectInfo(*node);
    EXPECT_TRUE(parent->HasChildExcludedFromNodeGroup());

    node->ExcludedFromNodeGroup(true);
    node->SetHasChildExcludedFromNodeGroup(false);
    rsUniRenderVisitor->CollectEffectInfo(*node);
    EXPECT_TRUE(parent->HasChildExcludedFromNodeGroup());

    parent->SetHasChildExcludedFromNodeGroup(false);
    node->ExcludedFromNodeGroup(false);
    node->SetHasChildExcludedFromNodeGroup(false);
    rsUniRenderVisitor->CollectEffectInfo(*node);
    EXPECT_FALSE(parent->HasChildExcludedFromNodeGroup());
}

/**
 * @tc.name: CollectEffectInfo009
 * @tc.desc: Test RSUnitRenderVisitorTest.CollectEffectInfo with parent node, ChildHasProtectedNode
 * @tc.type: FUNC
 * @tc.require: issue21180
 */
HWTEST_F(RSUniRenderVisitorTest, CollectEffectInfo009, TestSize.Level2)
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

    rsUniRenderVisitor->childHasProtectedNodeSet_.insert(nodeId);
    rsUniRenderVisitor->CollectEffectInfo(*node);
    EXPECT_TRUE(rsUniRenderVisitor->childHasProtectedNodeSet_.count(nodeId));
}

/**
 * @tc.name: CollectEffectInfo010
 * @tc.desc: Test RSUnitRenderVisitorTest.CollectEffectInfo with parent node, SetChildHasTranslate
 * @tc.type: FUNC
 * @tc.require: issueIAG8BF
 */
HWTEST_F(RSUniRenderVisitorTest, CollectEffectInfo010, TestSize.Level2)
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

    auto cacheRoot = std::make_shared<RSCanvasRenderNode>(10);
    cacheRoot->InitRenderParams();
    cacheRoot->SetDrawingCacheType(RSDrawingCacheType::FORCED_CACHE);
    // renderGroupCacheRoots_ is empty
    node->GetMutableRenderProperties().SetTranslate({0.0f, 0.0f});
    rsUniRenderVisitor->CollectEffectInfo(*node);
    EXPECT_FALSE(parent->stagingRenderParams_->ChildHasTranslateOnSqueeze());
    
    // renderGroupCacheRoots_ is not empty
    rsUniRenderVisitor->AddRenderGroupCacheRoot(*cacheRoot);

    node->GetMutableRenderProperties().SetTranslate({0.0f, 0.0f});
    rsUniRenderVisitor->CollectEffectInfo(*node);
    EXPECT_FALSE(parent->stagingRenderParams_->ChildHasTranslateOnSqueeze());

    node->GetMutableRenderProperties().SetTranslate({1.0f, 0.0f});
    rsUniRenderVisitor->CollectEffectInfo(*node);
    EXPECT_TRUE(parent->stagingRenderParams_->ChildHasTranslateOnSqueeze());

    node->GetMutableRenderProperties().SetTranslate({0.0f, 1.0f});
    rsUniRenderVisitor->CollectEffectInfo(*node);
    EXPECT_TRUE(parent->stagingRenderParams_->ChildHasTranslateOnSqueeze());

    node->GetMutableRenderProperties().SetTranslate({1.0f, 1.0f});
    rsUniRenderVisitor->CollectEffectInfo(*node);
    EXPECT_TRUE(parent->stagingRenderParams_->ChildHasTranslateOnSqueeze());
}

/**
 * @tc.name: SetRenderGroupSubTreeDirtyIfNeedTest
 * @tc.desc: Test SetRenderGroupSubTreeDirtyIfNeed
 * @tc.type: FUNC
 * @tc.require: issueIASE3Z
 */
HWTEST_F(RSUniRenderVisitorTest, SetRenderGroupSubTreeDirtyIfNeedTest, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->isDrawingCacheEnabled_ = true;

    auto node1 = std::make_shared<RSRenderNode>(1);
    node1->InitRenderParams();
    auto node2 = std::make_shared<RSRenderNode>(2);
    node2->InitRenderParams();
    node2->SetDirty();
    auto cacheRoot = std::make_shared<RSCanvasRenderNode>(10);
    cacheRoot->InitRenderParams();
    cacheRoot->SetDrawingCacheType(RSDrawingCacheType::FORCED_CACHE);

    // renderGroupCacheRoots_ is empty
    rsUniRenderVisitor->SetRenderGroupSubTreeDirtyIfNeed(*node1);
    EXPECT_FALSE(cacheRoot->IsRenderGroupSubTreeDirty());
    EXPECT_FALSE(rsUniRenderVisitor->hasMarkedRenderGroupSubTreeDirty_);

    // renderGroupCacheRoots_ is not empty, but subTree node is not dirty
    rsUniRenderVisitor->AddRenderGroupCacheRoot(*cacheRoot);
    rsUniRenderVisitor->SetRenderGroupSubTreeDirtyIfNeed(*node1);
    EXPECT_FALSE(cacheRoot->IsRenderGroupSubTreeDirty());
    EXPECT_FALSE(rsUniRenderVisitor->hasMarkedRenderGroupSubTreeDirty_);

    // subtree node dirty but excluded
    rsUniRenderVisitor->curExcludedRootNodeId_ = 10086;
    EXPECT_TRUE(rsUniRenderVisitor->IsOnRenderGroupExcludedSubTree());
    rsUniRenderVisitor->SetRenderGroupSubTreeDirtyIfNeed(*node2);
    EXPECT_FALSE(cacheRoot->IsRenderGroupSubTreeDirty());
    EXPECT_FALSE(rsUniRenderVisitor->hasMarkedRenderGroupSubTreeDirty_);

    // normal case
    rsUniRenderVisitor->curExcludedRootNodeId_ = INVALID_NODEID;
    rsUniRenderVisitor->SetRenderGroupSubTreeDirtyIfNeed(*node2);
    EXPECT_TRUE(cacheRoot->IsRenderGroupSubTreeDirty());
    EXPECT_TRUE(rsUniRenderVisitor->hasMarkedRenderGroupSubTreeDirty_);

    // already marked dirty
    rsUniRenderVisitor->SetRenderGroupSubTreeDirtyIfNeed(*node2);
    EXPECT_TRUE(cacheRoot->IsRenderGroupSubTreeDirty());
    EXPECT_TRUE(rsUniRenderVisitor->hasMarkedRenderGroupSubTreeDirty_);

    // pop cache root
    rsUniRenderVisitor->PopRenderGroupCacheRoot(*cacheRoot);
    EXPECT_TRUE(rsUniRenderVisitor->renderGroupCacheRoots_.empty());
}

/**
 * @tc.name: IsCurrentSubTreeForcePrepareTest
 * @tc.desc: Test IsCurrentSubTreeForcePrepare
 * @tc.type: FUNC
 * @tc.require: issueIASE3Z
 */
HWTEST_F(RSUniRenderVisitorTest, IsCurrentSubTreeForcePrepareTest001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->isDrawingCacheEnabled_ = true;
    bool& forcePrepare = rsUniRenderVisitor->isCurSubTreeForcePrepare_;

    auto node = std::make_shared<RSRenderNode>(1);
    node->InitRenderParams();
    node->SetRenderGroupExcludedStateChanged(true);
    {
        RSSubTreePrepareController controller(
            forcePrepare, rsUniRenderVisitor->IsCurrentSubTreeForcePrepare(*node));
        EXPECT_TRUE(forcePrepare);
    }
    EXPECT_FALSE(forcePrepare);
    EXPECT_FALSE(node->IsRenderGroupExcludedStateChanged());
    {
        RSSubTreePrepareController controller(
            forcePrepare, rsUniRenderVisitor->IsCurrentSubTreeForcePrepare(*node));
        EXPECT_FALSE(forcePrepare);
    }
}

/**
 * @tc.name: ForcePrepareSubTreeTest
 * @tc.desc: Test ForcePrepareSubTree
 * @tc.type: FUNC
 * @tc.require: issueIASE3Z
 */
HWTEST_F(RSUniRenderVisitorTest, ForcePrepareSubTreeTest, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    bool result = rsUniRenderVisitor->ForcePrepareSubTree();
    EXPECT_FALSE(result);

    rsUniRenderVisitor->isFirstFrameAfterScreenRotation_ = true;
    result = rsUniRenderVisitor->ForcePrepareSubTree();
    EXPECT_TRUE(result);

    rsUniRenderVisitor->isFirstFrameAfterScreenRotation_ = false;
    rsUniRenderVisitor->isCurSubTreeForcePrepare_ = true;
    result = rsUniRenderVisitor->ForcePrepareSubTree();
    EXPECT_TRUE(result);
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
    surfaceNode->SetIsScale(true);
    rsUniRenderVisitor->ancestorNodeHasAnimation_ = true;
    rsUniRenderVisitor->CheckIsGpuOverDrawBufferOptimizeNode(*surfaceNode);
    ASSERT_FALSE(surfaceNode->IsGpuOverDrawBufferOptimizeNode());
    rsUniRenderVisitor->ancestorNodeHasAnimation_ = false;
    rsUniRenderVisitor->CheckIsGpuOverDrawBufferOptimizeNode(*surfaceNode);
    ASSERT_FALSE(surfaceNode->IsGpuOverDrawBufferOptimizeNode());
    rsUniRenderVisitor->curCornerRadius_ = {1.f, 1.f, 1.f, 1.f};
    rsUniRenderVisitor->curAlpha_ = 0.f;
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
 * @tc.name: MergeRemovedChildDirtyRegion001
 * @tc.desc: Test MergeRemovedChildDirtyRegion while node has removed child which dirty rect is empty
 * @tc.type: FUNC
 * @tc.require: issueIAJJ43
 */
HWTEST_F(RSUniRenderVisitorTest, MergeRemovedChildDirtyRegion001, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    auto dirtyManager = surfaceNode->GetDirtyManager();
    ASSERT_NE(dirtyManager, nullptr);
    surfaceNode->hasRemovedChild_ = true;

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curSurfaceDirtyManager_ = dirtyManager;
    rsUniRenderVisitor->curSurfaceNode_ = nullptr;
    rsUniRenderVisitor->MergeRemovedChildDirtyRegion(*surfaceNode);
    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;
    rsUniRenderVisitor->MergeRemovedChildDirtyRegion(*surfaceNode);
    ASSERT_FALSE(surfaceNode->HasRemovedChild());
}

/*
 * @tc.name: MergeRemovedChildDirtyRegion002
 * @tc.desc: Test MergeRemovedChildDirtyRegion while has removed child which dirty rect isn't empty
 * @tc.type: FUNC
 * @tc.require: issueIAJJ43
 */
HWTEST_F(RSUniRenderVisitorTest, MergeRemovedChildDirtyRegion002, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    auto dirtyManager = surfaceNode->GetDirtyManager();
    ASSERT_NE(dirtyManager, nullptr);
    surfaceNode->hasRemovedChild_ = true;
    surfaceNode->childrenRect_ = DEFAULT_RECT;
    surfaceNode->removedChildrenRect_ = DEFAULT_RECT;

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;
    rsUniRenderVisitor->curSurfaceDirtyManager_ = dirtyManager;
    rsUniRenderVisitor->MergeRemovedChildDirtyRegion(*surfaceNode);
    ASSERT_EQ(dirtyManager->GetCurrentFrameDirtyRegion(), RectI(0, 0, 0, 0));
}

/*
 * @tc.name: MergeRemovedChildDirtyRegion003
 * @tc.desc: Test MergeRemovedChildDirtyRegion while curSurfaceDirtyManager_ is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAJJ43
 */
HWTEST_F(RSUniRenderVisitorTest, MergeRemovedChildDirtyRegion003, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->hasRemovedChild_ = true;
    surfaceNode->childrenRect_ = DEFAULT_RECT;
    surfaceNode->removedChildrenRect_ = DEFAULT_RECT;

    auto rsContext = std::make_shared<RSContext>();
    auto displayNode = std::make_shared<RSScreenRenderNode>(0, 0, rsContext->weak_from_this());
    auto dirtyManager = displayNode->GetDirtyManager();
    ASSERT_NE(dirtyManager, nullptr);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;
    rsUniRenderVisitor->curSurfaceDirtyManager_ = nullptr;
    rsUniRenderVisitor->curScreenDirtyManager_ = dirtyManager;
    rsUniRenderVisitor->MergeRemovedChildDirtyRegion(*surfaceNode);
    ASSERT_EQ(dirtyManager->GetCurrentFrameDirtyRegion(), RectI(0, 0, 0, 0));
}

/*
 * @tc.name: MergeRemovedChildDirtyRegion004
 * @tc.desc: Test MergeRemovedChildDirtyRegion while curSurfaceDirtyManager_ is not nullptr
 * @tc.type: FUNC
 * @tc.require: issuesIBSNHZ
 */
HWTEST_F(RSUniRenderVisitorTest, MergeRemovedChildDirtyRegion004, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    auto dirtyManager = surfaceNode->GetDirtyManager();
    ASSERT_NE(dirtyManager, nullptr);
    surfaceNode->hasRemovedChild_ = true;
    surfaceNode->childrenRect_ = DEFAULT_RECT;
    surfaceNode->removedChildrenRect_ = DEFAULT_RECT;
    surfaceNode->oldClipRect_ = DEFAULT_RECT;

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;
    rsUniRenderVisitor->curSurfaceDirtyManager_ = dirtyManager;
    rsUniRenderVisitor->MergeRemovedChildDirtyRegion(*surfaceNode, true);
    ASSERT_EQ(dirtyManager->GetCurrentFrameDirtyRegion(), DEFAULT_RECT);
}

/*
 * @tc.name: MergeRemovedChildDirtyRegion005
 * @tc.desc: Test MergeRemovedChildDirtyRegion while node's bounds geometry is nullptr
 * @tc.type: FUNC
 * @tc.require: issuesIBSNHZ
 */
HWTEST_F(RSUniRenderVisitorTest, MergeRemovedChildDirtyRegion005, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    auto dirtyManager = surfaceNode->GetDirtyManager();
    ASSERT_NE(dirtyManager, nullptr);
    surfaceNode->hasRemovedChild_ = true;
    surfaceNode->childrenRect_ = DEFAULT_RECT;
    surfaceNode->removedChildrenRect_ = DEFAULT_RECT;
    surfaceNode->oldClipRect_ = DEFAULT_RECT;
    surfaceNode->GetMutableRenderProperties().boundsGeo_ = nullptr;

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;
    rsUniRenderVisitor->curSurfaceDirtyManager_ = dirtyManager;
    rsUniRenderVisitor->MergeRemovedChildDirtyRegion(*surfaceNode, true);
    ASSERT_EQ(dirtyManager->GetCurrentFrameDirtyRegion(), DEFAULT_RECT);
}

/*
 * @tc.name: MergeRemovedChildDirtyRegion006
 * @tc.desc: Test MergeRemovedChildDirtyRegion while dirtyManager is target for DFX
 * @tc.type: FUNC
 * @tc.require: issuesIBSNHZ
 */
HWTEST_F(RSUniRenderVisitorTest, MergeRemovedChildDirtyRegion006, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    auto dirtyManager = surfaceNode->GetDirtyManager();
    ASSERT_NE(dirtyManager, nullptr);
    surfaceNode->hasRemovedChild_ = true;
    surfaceNode->childrenRect_ = DEFAULT_RECT;
    surfaceNode->removedChildrenRect_ = DEFAULT_RECT;
    surfaceNode->oldClipRect_ = DEFAULT_RECT;
    surfaceNode->GetDirtyManager()->MarkAsTargetForDfx();

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;
    rsUniRenderVisitor->curSurfaceDirtyManager_ = dirtyManager;
    rsUniRenderVisitor->MergeRemovedChildDirtyRegion(*surfaceNode, true);
    ASSERT_EQ(dirtyManager->GetCurrentFrameDirtyRegion(), DEFAULT_RECT);
}

/*
 * @tc.name: PrevalidateHwcNode001
 * @tc.desc: Test while prevalidate hwcNode disable
 * @tc.type: FUNC
 * @tc.require: issueIAJJ43
 */
HWTEST_F(RSUniRenderVisitorTest, PrevalidateHwcNode001, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    NodeId id = 1;
    auto rsContext = std::make_shared<RSContext>();
    auto displayNode = std::make_shared<RSScreenRenderNode>(id, 0, rsContext);
    displayNode->GetAllMainAndLeashSurfaces().push_back(surfaceNode);
    
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curScreenNode_ = displayNode;
    rsUniRenderVisitor->PrevalidateHwcNode();
    ASSERT_FALSE(surfaceNode->isHardwareForcedDisabled_);
}

/*
 * @tc.name: UpdateSurfaceDirtyAndGlobalDirty002
 * @tc.desc: Test while surface node has hardware enabled node as child
 * @tc.type: FUNC
 * @tc.require: issueIAJJ43
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateSurfaceDirtyAndGlobalDirty002, TestSize.Level2)
{
    NodeId id = 1;
    auto rsContext = std::make_shared<RSContext>();
    auto displayNode = std::make_shared<RSScreenRenderNode>(id, 0, rsContext);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    auto hwcNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(hwcNode, nullptr);
    surfaceNode->AddChildHardwareEnabledNode(hwcNode);
    displayNode->GetAllMainAndLeashSurfaces().push_back(surfaceNode);

    RsCommonHook::Instance().SetHardwareEnabledByBackgroundAlphaFlag(true);
    RsCommonHook::Instance().SetHardwareEnabledByHwcnodeBelowSelfInAppFlag(true);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curScreenNode_ = displayNode;
    rsUniRenderVisitor->UpdateSurfaceDirtyAndGlobalDirty();
    ASSERT_FALSE(surfaceNode->isHardwareForcedDisabled_);
}

/*
 * @tc.name: UpdateSurfaceDirtyAndGlobalDirty003
 * @tc.desc: Test while current frame dirty
 * @tc.type: FUNC
 * @tc.require: issueIAJJ43
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateSurfaceDirtyAndGlobalDirty003, TestSize.Level2)
{
    NodeId id = 1;
    auto rsContext = std::make_shared<RSContext>();
    auto displayNode = std::make_shared<RSScreenRenderNode>(id, 0, rsContext);
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
    rsUniRenderVisitor->curScreenNode_ = displayNode;
    rsUniRenderVisitor->UpdateSurfaceDirtyAndGlobalDirty();
    ASSERT_FALSE(surfaceNode->isHardwareForcedDisabled_);
}

/*
 * @tc.name: UpdateSurfaceDirtyAndGlobalDirty004
 * @tc.desc: Test while app window node skip in calculate global dirty
 * @tc.type: FUNC
 * @tc.require: issueIAJJ43
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateSurfaceDirtyAndGlobalDirty004, TestSize.Level2)
{
    NodeId id = 1;
    auto rsContext = std::make_shared<RSContext>();
    auto displayNode = std::make_shared<RSScreenRenderNode>(id, 0, rsContext);
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
    rsUniRenderVisitor->curScreenNode_ = displayNode;
    rsUniRenderVisitor->UpdateSurfaceDirtyAndGlobalDirty();
    ASSERT_FALSE(surfaceNode->isHardwareForcedDisabled_);
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

    rsUniRenderVisitor->CollectFilterInfoAndUpdateDirty(*node, *dirtyManager, rect, rect);
    ASSERT_FALSE(dirtyManager->GetFilterCollector().GetFilterDirtyRegionInfoList(false).empty());
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

    rsUniRenderVisitor->CollectFilterInfoAndUpdateDirty(*node, *dirtyManager, rect, rect);
    ASSERT_FALSE(dirtyManager->GetFilterCollector().GetFilterDirtyRegionInfoList(false).empty());
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

    rsUniRenderVisitor->CollectFilterInfoAndUpdateDirty(*node, *dirtyManager, rect, rect);
    ASSERT_FALSE(dirtyManager->GetFilterCollector().GetFilterDirtyRegionInfoList(false).empty());
}

/*
 * @tc.name: CollectFilterInfoAndUpdateDirty004
 * @tc.desc: Verify function CollectFilterInfoAndUpdateDirty with transparent(needDrawBehindWindow) curSurfaceNode
 * @tc.type: FUNC
 * @tc.require: issuesIAE0Q3
 */
HWTEST_F(RSUniRenderVisitorTest, CollectFilterInfoAndUpdateDirty004, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    NodeId surfaceNodeId = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceNodeId);
    surfaceNode->renderProperties_.GetEffect().needDrawBehindWindow_ = true;
    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;

    auto dirtyManager = surfaceNode->GetDirtyManager();
    ASSERT_NE(dirtyManager, nullptr);
    RectI dirtyRegion(0, 0, 100, 100);
    dirtyManager->currentFrameDirtyRegion_ = dirtyRegion;
    RectI globalFilterRect(0, 0, 20, 20);
    rsUniRenderVisitor->curScreenDirtyManager_ = std::make_shared<RSDirtyRegionManager>();
    rsUniRenderVisitor->curScreenDirtyManager_->currentFrameDirtyRegion_ = {};

    rsUniRenderVisitor->CollectFilterInfoAndUpdateDirty(*surfaceNode, *dirtyManager,
        globalFilterRect, globalFilterRect);
    ASSERT_FALSE(dirtyManager->GetFilterCollector().GetFilterDirtyRegionInfoList(false).empty());
}

/**
 * @tc.name: UpdateHwcNodeDirtyRegionAndCreateLayer
 * @tc.desc: Test UpdateHwcNodeDirtyRegionAndCreateLayer with default surface node (empty child).
 * @tc.type: FUNC
 * @tc.require: IAHFXD
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeDirtyRegionAndCreateLayer001, TestSize.Level1)
{
    // create input args.
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    std::vector<std::shared_ptr<RSSurfaceRenderNode>> topLayers;
    rsUniRenderVisitor->UpdateHwcNodeDirtyRegionAndCreateLayer(node, topLayers);
}

/**
 * @tc.name: UpdateHwcNodeDirtyRegionAndCreateLayer
 * @tc.desc: Test UpdateHwcNodeDirtyRegionAndCreateLayer with off-tree/on-tree child node.
 * @tc.type: FUNC
 * @tc.require: IAHFXD
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

    NodeId displayId = 1;
    auto rsContext = std::make_shared<RSContext>();
    auto displayNode = std::make_shared<RSScreenRenderNode>(displayId, 0, rsContext);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    displayNode->SetHasUniRenderHdrSurface(true);
    rsUniRenderVisitor->curScreenNode_ = displayNode;
    std::vector<std::shared_ptr<RSSurfaceRenderNode>> topLayers;
    rsUniRenderVisitor->UpdateHwcNodeDirtyRegionAndCreateLayer(node, topLayers);
}

/**
 * @tc.name: UpdateHwcNodeDirtyRegionAndCreateLayer
 * @tc.desc: Test UpdateHwcNodeDirtyRegionAndCreateLayer for toplayer
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeDirtyRegionAndCreateLayer003, TestSize.Level1)
{
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto childNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    childNode->SetIsOnTheTree(true);
    childNode->SetLayerTop(true);
    node->AddChildHardwareEnabledNode(childNode);
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    auto screenNode = std::make_shared<RSScreenRenderNode>(1, 0, rsContext->weak_from_this());

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curScreenNode_ = screenNode;
    EXPECT_NE(rsUniRenderVisitor->hasMirrorDisplay_, true);
    ASSERT_NE(childNode->GetRSSurfaceHandler(), nullptr);
    EXPECT_FALSE(childNode->GetRSSurfaceHandler()->IsCurrentFrameBufferConsumed());
    EXPECT_TRUE(node->GetVisibleRegion().IsEmpty());
    EXPECT_EQ(rsUniRenderVisitor->curScreenDirtyManager_, nullptr);
    std::vector<std::shared_ptr<RSSurfaceRenderNode>> topLayers;
    rsUniRenderVisitor->UpdateHwcNodeDirtyRegionAndCreateLayer(node, topLayers);

    rsUniRenderVisitor->hasMirrorDisplay_ = true;
    rsUniRenderVisitor->UpdateHwcNodeDirtyRegionAndCreateLayer(node, topLayers);

    auto handler = childNode->GetMutableRSSurfaceHandler();
    ASSERT_NE(handler, nullptr);
    handler->SetCurrentFrameBufferConsumed();
    EXPECT_TRUE(childNode->GetRSSurfaceHandler()->IsCurrentFrameBufferConsumed());
    rsUniRenderVisitor->UpdateHwcNodeDirtyRegionAndCreateLayer(node, topLayers);

    Occlusion::Rect r;
    node->visibleRegion_.rects_.push_back(r);
    node->visibleRegion_.bound_.left_ = 0;
    node->visibleRegion_.bound_.right_ = 1;
    node->visibleRegion_.bound_.top_ = 0;
    node->visibleRegion_.bound_.bottom_ = 1;
    EXPECT_FALSE(node->GetVisibleRegion().IsEmpty());
    rsUniRenderVisitor->UpdateHwcNodeDirtyRegionAndCreateLayer(node, topLayers);

    rsUniRenderVisitor->curScreenDirtyManager_ = std::make_shared<RSDirtyRegionManager>();
    EXPECT_NE(rsUniRenderVisitor->curScreenDirtyManager_, nullptr);
    rsUniRenderVisitor->UpdateHwcNodeDirtyRegionAndCreateLayer(node, topLayers);
}

/**
 * @tc.name: UpdateHwcNodeDirtyRegionForApp
 * @tc.desc: Test UpdateHwcNodeDirtyRegionForApp, current frame enable status different from last frame.
 * @tc.type: FUNC
 * @tc.require: IAHFXD
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
 * @tc.name: UpdateHwcNodeDirtyRegionForApp
 * @tc.desc: Test UpdateHwcNodeDirtyRegionForApp, current frame enable status different from last frame.
 * @tc.type: FUNC
 * @tc.require: IAHFXD
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
 * @tc.name: CalculateOpaqueAndTransparentRegion
 * @tc.desc: Test CalculateOpaqueAndTransparentRegion002, curScreenNode_ = nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBCR0E
 */
HWTEST_F(RSUniRenderVisitorTest, CalculateOpaqueAndTransparentRegion002, TestSize.Level1)
{
    auto node = RSTestUtil::CreateSurfaceNode();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curScreenNode_ = nullptr;
    rsUniRenderVisitor->CalculateOpaqueAndTransparentRegion(*node);
}

/**
 * @tc.name: CalculateOpaqueAndTransparentRegion
 * @tc.desc: Test CalculateOpaqueAndTransparentRegion003, isAllSurfaceVisibleDebugEnabled_ = true
 * @tc.type: FUNC
 * @tc.require: issueIBCR0E
 */
HWTEST_F(RSUniRenderVisitorTest, CalculateOpaqueAndTransparentRegion003, TestSize.Level1)
{
    auto node = RSTestUtil::CreateSurfaceNode();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsContext = std::make_shared<RSContext>();
    RSDisplayNodeConfig config;
    auto curScreenNode = std::make_shared<RSLogicalDisplayRenderNode>(0, config, rsContext->weak_from_this());
    curScreenNode->InitRenderParams();
    rsUniRenderVisitor->isAllSurfaceVisibleDebugEnabled_ = true;
    rsUniRenderVisitor->CalculateOpaqueAndTransparentRegion(*node);
}

/**
 * @tc.name: CollectOcclusionInfoForWMS
 * @tc.desc: Test CollectOcclusionInfoForWMS001, node.IsMainWindowType() return false
 * @tc.type: FUNC
 * @tc.require: issueIAJSIS
 */
HWTEST_F(RSUniRenderVisitorTest, CollectOcclusionInfoForWMS001, TestSize.Level1)
{
    auto node = RSTestUtil::CreateSurfaceNode();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    node->SetSurfaceNodeType(RSSurfaceNodeType::SCB_SCREEN_NODE);
    rsUniRenderVisitor->CollectOcclusionInfoForWMS(*node);
}

/**
 * @tc.name: CollectOcclusionInfoForWMS
 * @tc.desc: Test CollectOcclusionInfoForWMS002, instanceNode == nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAJSIS
 */
HWTEST_F(RSUniRenderVisitorTest, CollectOcclusionInfoForWMS002, TestSize.Level1)
{
    auto node = RSTestUtil::CreateSurfaceNode();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    node->SetSurfaceNodeType(RSSurfaceNodeType::DEFAULT);
    std::shared_ptr<RSContext> context = nullptr;
    node->context_ = context;
    rsUniRenderVisitor->CollectOcclusionInfoForWMS(*node);
}

/**
 * @tc.name: QuickPrepareEffectRenderNode
 * @tc.desc: Test QuickPrepareEffectRenderNode001, dirtyManager = nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAJSIS
 */
HWTEST_F(RSUniRenderVisitorTest, QuickPrepareEffectRenderNode001, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curSurfaceNode_ = RSTestUtil::CreateSurfaceNodeWithBuffer();
    rsUniRenderVisitor->curSurfaceDirtyManager_ = nullptr;
    NodeId id = 2;
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSEffectRenderNode>(id, rsContext->weak_from_this());
    node->InitRenderParams();
    rsUniRenderVisitor->QuickPrepareEffectRenderNode(*node);
}

/**
 * @tc.name: QuickPrepareEffectRenderNode
 * @tc.desc: Test QuickPrepareEffectRenderNode002, dirtyManager != nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAJSIS
 */
HWTEST_F(RSUniRenderVisitorTest, QuickPrepareEffectRenderNode002, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;
    auto dirtyManager = surfaceNode->GetDirtyManager();
    ASSERT_NE(dirtyManager, nullptr);
    rsUniRenderVisitor->curSurfaceDirtyManager_ = dirtyManager;
    NodeId id1 = 2;
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSEffectRenderNode>(id1, rsContext->weak_from_this());
    node->InitRenderParams();
    NodeId id2 = 0;
    RSDisplayNodeConfig config;
    auto curScreenNode = std::make_shared<RSLogicalDisplayRenderNode>(id2, config, rsContext->weak_from_this());
    curScreenNode->InitRenderParams();
    rsUniRenderVisitor->QuickPrepareEffectRenderNode(*node);
}

/**
 * @tc.name: UpdateSurfaceRenderNodeRotate002
 * @tc.desc: Test UpdateSurfaceRenderNodeRotate when IsMainWindowType is false
 * @tc.type: FUNC
 * @tc.require: issueIASE3Z
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateSurfaceRenderNodeRotate002, TestSize.Level2)
{
    auto node = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(node, nullptr);
    node->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    rsUniRenderVisitor->UpdateSurfaceRenderNodeRotate(*node);
    ASSERT_FALSE(node->isRotating_);
}

/**
 * @tc.name: IsSubTreeOccluded002
 * @tc.desc: Test IsSubTreeOccluded with empty node
 * @tc.type: FUNC
 * @tc.require: issueIASE3Z
 */
HWTEST_F(RSUniRenderVisitorTest, IsSubTreeOccluded002, TestSize.Level2)
{
    auto node = RSTestUtil::CreateSurfaceNode();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->isOcclusionEnabled_ = true;
    node->isSubTreeDirty_ = true;
    ASSERT_EQ(rsUniRenderVisitor->IsSubTreeOccluded(*node), true);
    ASSERT_EQ(node->dirtyStatus_, RSRenderNode::NodeDirty::CLEAN);
}

/**
 * @tc.name: QuickPrepareDisplayRenderNode001
 * @tc.desc: Test QuickPrepareScreenRenderNode with display node
 * @tc.type: FUNC
 * @tc.require: issueIASE3Z
 */
HWTEST_F(RSUniRenderVisitorTest, QuickPrepareDisplayRenderNode001, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    auto rsDisplayRenderNode = std::make_shared<RSScreenRenderNode>(11, 0, rsContext->weak_from_this());
    ASSERT_NE(rsDisplayRenderNode, nullptr);
    rsDisplayRenderNode->dirtyManager_ = std::make_shared<RSDirtyRegionManager>();
    ASSERT_NE(rsDisplayRenderNode->dirtyManager_, nullptr);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curScreenNode_ = rsDisplayRenderNode;
    auto screenManager = CreateOrGetScreenManager();
    auto screenId = CreateVirtualScreen(screenManager);
    ASSERT_NE(screenId, INVALID_SCREEN_ID);
    
    rsDisplayRenderNode->stagingRenderParams_ = std::make_unique<RSScreenRenderParams>(screenId);
    ASSERT_NE(rsDisplayRenderNode->stagingRenderParams_, nullptr);
    rsUniRenderVisitor->curScreenNode_ = rsDisplayRenderNode;
    rsUniRenderVisitor->QuickPrepareScreenRenderNode(*rsDisplayRenderNode);
    screenManager->RemoveVirtualScreen(screenId);
}

/**
 * @tc.name: QuickPrepareDisplayRenderNode002
 * @tc.desc: Test QuickPrepareScreenRenderNode with display node dirtyFlag_ = true
 * @tc.type: FUNC
 * @tc.require: issuesIC9MUF
 */
HWTEST_F(RSUniRenderVisitorTest, QuickPrepareDisplayRenderNode002, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    ScreenId id = 1;
    auto rsDisplayRenderNode = std::make_shared<RSScreenRenderNode>(11, id, rsContext->weak_from_this());
    rsDisplayRenderNode->InitRenderParams();
    ASSERT_NE(rsDisplayRenderNode, nullptr);
    rsDisplayRenderNode->dirtyManager_ = std::make_shared<RSDirtyRegionManager>();
    ASSERT_NE(rsDisplayRenderNode->dirtyManager_, nullptr);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curScreenNode_ = rsDisplayRenderNode;
    rsUniRenderVisitor->dirtyFlag_ = true;
    auto screenManager = CreateOrGetScreenManager();
    auto screenId = CreateVirtualScreen(screenManager);
    ASSERT_NE(screenId, INVALID_SCREEN_ID);
    
    rsDisplayRenderNode->stagingRenderParams_ = std::make_unique<RSScreenRenderParams>(screenId);
    ASSERT_NE(rsDisplayRenderNode->stagingRenderParams_, nullptr);
    rsUniRenderVisitor->curScreenNode_ = rsDisplayRenderNode;
    rsUniRenderVisitor->QuickPrepareScreenRenderNode(*rsDisplayRenderNode);
    screenManager->RemoveVirtualScreen(screenId);
}

/**
 * @tc.name: CheckFilterCacheNeedForceClearOrSave001
 * @tc.desc: Test CheckFilterCacheNeedForceClearOrSave with multi-RSRenderNode
 * @tc.type: FUNC
 * @tc.require: issueIASE3Z
 */
HWTEST_F(RSUniRenderVisitorTest, CheckFilterCacheNeedForceClearOrSave001, TestSize.Level2)
{
    auto node = std::make_shared<RSRenderNode>(1);
    ASSERT_NE(node, nullptr);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->CheckFilterCacheNeedForceClearOrSave(*node);

    std::shared_ptr<RSFilter> filter = RSFilter::CreateBlurFilter(1.0f, 1.0f);
    node->renderProperties_.backgroundFilter_ = filter;
    rsUniRenderVisitor->CheckFilterCacheNeedForceClearOrSave(*node);
}

/**
 * @tc.name: PrepareForUIFirstNode001
 * @tc.desc: Test PrepareForUIFirstNode with with uifirst card and renderGroupCacheRoots_
 * @tc.type: FUNC
 * @tc.require: issueIASE3Z
 */
HWTEST_F(RSUniRenderVisitorTest, PrepareForUIFirstNode001, TestSize.Level2)
{
    auto rsSurfaceRenderNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    ScreenId id = 1;
    auto rsDisplayRenderNode = std::make_shared<RSScreenRenderNode>(11, id, rsContext->weak_from_this());
    rsUniRenderVisitor->curScreenNode_ = rsDisplayRenderNode;

    // Create canvas render nodes with different cache types to test both branches
    auto canvasNodeForcedCache = std::make_shared<RSCanvasRenderNode>(101);
    auto canvasNodeTargetedCache = std::make_shared<RSCanvasRenderNode>(102);
    auto canvasNodeDisabledCache = std::make_shared<RSCanvasRenderNode>(103);
    ASSERT_NE(canvasNodeForcedCache, nullptr);
    ASSERT_NE(canvasNodeTargetedCache, nullptr);
    ASSERT_NE(canvasNodeDisabledCache, nullptr);

    // Set different cache types
    canvasNodeForcedCache->SetDrawingCacheType(RSDrawingCacheType::FORCED_CACHE);
    canvasNodeTargetedCache->SetDrawingCacheType(RSDrawingCacheType::TARGETED_CACHE);
    canvasNodeDisabledCache->SetDrawingCacheType(RSDrawingCacheType::DISABLED_CACHE);

    // Add nodes to renderGroupCacheRoots_
    rsUniRenderVisitor->renderGroupCacheRoots_[101] = canvasNodeForcedCache;
    rsUniRenderVisitor->renderGroupCacheRoots_[102] = canvasNodeTargetedCache;
    rsUniRenderVisitor->renderGroupCacheRoots_[103] = canvasNodeDisabledCache;

    // Verify initial state
    ASSERT_EQ(canvasNodeForcedCache->GetDrawingCacheType(), RSDrawingCacheType::FORCED_CACHE);
    ASSERT_EQ(canvasNodeTargetedCache->GetDrawingCacheType(), RSDrawingCacheType::TARGETED_CACHE);
    ASSERT_EQ(canvasNodeDisabledCache->GetDrawingCacheType(), RSDrawingCacheType::DISABLED_CACHE);

    RSUifirstManager::Instance().isUiFirstOn_ = true;
    RSUifirstManager::Instance().isCardUiFirstOn_ = true;
    rsSurfaceRenderNode->SetAncestorScreenNode(rsDisplayRenderNode->weak_from_this());
    rsSurfaceRenderNode->SetSurfaceNodeType(RSSurfaceNodeType::ABILITY_COMPONENT_NODE);
    rsSurfaceRenderNode->name_ = "ArkTSCardNode";
    rsSurfaceRenderNode->SetUIFirstSwitch(RSUIFirstSwitch::NONE);
    RSUifirstManager::Instance().entryViewNodeId_ = rsSurfaceRenderNode->GetId();
    RSUifirstManager::Instance().negativeScreenNodeId_ = rsSurfaceRenderNode->GetId();
    rsSurfaceRenderNode->instanceRootNodeId_ = rsSurfaceRenderNode->GetId();
    rsUniRenderVisitor->PrepareForUIFirstNode(*rsSurfaceRenderNode);

    // Verify: FORCED_CACHE changed to DISABLED_CACHE (true branch)
    EXPECT_EQ(canvasNodeForcedCache->GetDrawingCacheType(), RSDrawingCacheType::DISABLED_CACHE);
    // Verify: TARGETED_CACHE remains unchanged (false branch)
    EXPECT_EQ(canvasNodeTargetedCache->GetDrawingCacheType(), RSDrawingCacheType::TARGETED_CACHE);
    // Verify: DISABLED_CACHE remains unchanged (false branch)
    EXPECT_EQ(canvasNodeDisabledCache->GetDrawingCacheType(), RSDrawingCacheType::DISABLED_CACHE);

    // Clear renderGroupCacheRoots_ for next test
    rsUniRenderVisitor->renderGroupCacheRoots_.clear();
    rsSurfaceRenderNode->lastFrameUifirstFlag_ = MultiThreadCacheType::LEASH_WINDOW;
    rsSurfaceRenderNode->GetMultableSpecialLayerMgr().Set(SpecialLayerType::PROTECTED, true);
    rsUniRenderVisitor->PrepareForUIFirstNode(*rsSurfaceRenderNode);
    RSUifirstManager::Instance().isUiFirstOn_ = false;
}

/**
 * @tc.name: PrepareForUIFirstNode002
 * @tc.desc: Test PrepareForUIFirstNode with visible filter
 * @tc.type: FUNC
 * @tc.require: issue20192
 */
HWTEST_F(RSUniRenderVisitorTest, PrepareForUIFirstNode002, TestSize.Level2)
{
    RSSurfaceRenderNodeConfig config;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config);
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    ScreenId id = 1;
    auto rsScreenRenderNode = std::make_shared<RSScreenRenderNode>(11, id, rsContext->weak_from_this());
    rsUniRenderVisitor->curScreenNode_ = rsScreenRenderNode;

    rsSurfaceRenderNode->stagingRenderParams_ = std::make_unique<RSSurfaceRenderParams>(1);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(rsSurfaceRenderNode->stagingRenderParams_.get());
    RSUifirstManager::Instance().uifirstType_ = UiFirstCcmType::SINGLE;
    rsUniRenderVisitor->PrepareForUIFirstNode(*rsSurfaceRenderNode);
    ASSERT_TRUE(surfaceParams->GetUifirstVisibleFilterRect().IsEmpty());

    RSUifirstManager::Instance().uifirstType_ = UiFirstCcmType::MULTI;
    rsUniRenderVisitor->PrepareForUIFirstNode(*rsSurfaceRenderNode);
    ASSERT_TRUE(surfaceParams->GetUifirstVisibleFilterRect().IsEmpty());

    RSUifirstManager::Instance().isUiFirstOn_ = true;
    rsSurfaceRenderNode->SetUIFirstSwitch(RSUIFirstSwitch::FORCE_ENABLE);
    rsSurfaceRenderNode->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    rsUniRenderVisitor->PrepareForUIFirstNode(*rsSurfaceRenderNode);
    ASSERT_TRUE(surfaceParams->GetUifirstVisibleFilterRect().IsEmpty());
}

/**
 * @tc.name: UpdateNodeVisibleRegion001
 * @tc.desc: Test UpdateNodeVisibleRegion when needRecalculateOcclusion_ is true
 * @tc.type: FUNC
 * @tc.require: issueIASE3Z
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateNodeVisibleRegion001, TestSize.Level2)
{
    RSSurfaceRenderNodeConfig config;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config);
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    ScreenId id = 1;
    auto rsDisplayRenderNode = std::make_shared<RSScreenRenderNode>(11, id, rsContext->weak_from_this());
    rsUniRenderVisitor->curScreenNode_ = rsDisplayRenderNode;
    rsUniRenderVisitor->needRecalculateOcclusion_ = true;
    rsUniRenderVisitor->PrepareForUIFirstNode(*rsSurfaceRenderNode);
}

/**
 * @tc.name: UpdateNodeVisibleRegion002
 * @tc.desc: Visible region of app window out of screen depends on whether it's cross-screen. If so, its all-visible.
 * @tc.type: FUNC
 * @tc.require: issueIC84ZR
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateNodeVisibleRegion002, TestSize.Level2)
{
    auto rsSurfaceRenderNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    auto rsUniRenderVisitor = InitRSUniRenderVisitor();
    rsUniRenderVisitor->curScreenNode_ = nullptr;
    rsUniRenderVisitor->UpdateNodeVisibleRegion(*rsSurfaceRenderNode);
    ScreenId screenId = 1;
    auto rsContext = std::make_shared<RSContext>();
    rsUniRenderVisitor->curScreenNode_ = std::make_shared<RSScreenRenderNode>(DEFAULT_NODE_ID, screenId, rsContext);
    rsUniRenderVisitor->curScreenNode_->isFirstVisitCrossNodeDisplay_ = true;
    rsUniRenderVisitor->needRecalculateOcclusion_ = true;
    // test: if node is non-cross-display, its visibility will be cropped by screen.
    rsSurfaceRenderNode->oldDirtyInSurface_ = RectI();
    rsSurfaceRenderNode->absDrawRect_ = DEFAULT_RECT;
    rsUniRenderVisitor->UpdateNodeVisibleRegion(*rsSurfaceRenderNode);
    ASSERT_TRUE(rsSurfaceRenderNode->GetVisibleRegion().IsEmpty());
    // test: if node is cross-display, its visibility will not be cropped by screen.
    rsSurfaceRenderNode->isFirstLevelCrossNode_ = true;
    rsUniRenderVisitor->UpdateNodeVisibleRegion(*rsSurfaceRenderNode);
    ASSERT_FALSE(rsSurfaceRenderNode->GetVisibleRegion().IsEmpty());
    ASSERT_TRUE(rsSurfaceRenderNode->GetVisibleRegion().GetBound() == Occlusion::Rect(DEFAULT_RECT));
}

/**
 * @tc.name: UpdateNodeVisibleRegion003
 * @tc.desc: Test UpdateNodeVisibleRegion when IsBehindWindowOcclusionChanged() return true
 * @tc.type: FUNC
 * @tc.require: issueIC9HNQ
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateNodeVisibleRegion003, TestSize.Level2)
{
    RSSurfaceRenderNodeConfig config;
    auto rsSurfaceRenderNode = std::make_shared<MockRSSurfaceRenderNode>(config.id);
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    auto rsUniRenderVisitor = InitRSUniRenderVisitor();
    EXPECT_CALL(*rsSurfaceRenderNode, CheckIfOcclusionChanged()).WillRepeatedly(testing::Return(false));
    rsSurfaceRenderNode->behindWindowOcclusionChanged_ = true;
    rsSurfaceRenderNode->oldDirtyInSurface_ = RectI();
    rsSurfaceRenderNode->absDrawRect_ = DEFAULT_RECT;
    rsSurfaceRenderNode->isFirstLevelCrossNode_ = true;
    rsUniRenderVisitor->UpdateNodeVisibleRegion(*rsSurfaceRenderNode);
    ASSERT_FALSE(rsSurfaceRenderNode->GetVisibleRegion().IsEmpty());
    ASSERT_TRUE(rsSurfaceRenderNode->GetVisibleRegion().GetBound() == Occlusion::Rect(DEFAULT_RECT));
}

/**
 * @tc.name: UpdateNodeVisibleRegion004
 * @tc.desc: Test UpdateNodeVisibleRegion when IsBehindWindowOcclusionChanged() return false
 * @tc.type: FUNC
 * @tc.require: issueIC9HNQ
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateNodeVisibleRegion004, TestSize.Level2)
{
    RSSurfaceRenderNodeConfig config;
    auto rsSurfaceRenderNode = std::make_shared<MockRSSurfaceRenderNode>(config.id);
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    auto rsUniRenderVisitor = InitRSUniRenderVisitor();
    EXPECT_CALL(*rsSurfaceRenderNode, CheckIfOcclusionChanged()).WillRepeatedly(testing::Return(false));
    rsSurfaceRenderNode->behindWindowOcclusionChanged_ = false;
    rsSurfaceRenderNode->oldDirtyInSurface_ = RectI();
    rsSurfaceRenderNode->absDrawRect_ = DEFAULT_RECT;
    rsSurfaceRenderNode->isFirstLevelCrossNode_ = true;
    rsUniRenderVisitor->UpdateNodeVisibleRegion(*rsSurfaceRenderNode);
    ASSERT_TRUE(rsSurfaceRenderNode->GetVisibleRegion().IsEmpty());
}

/**
 * @tc.name: CalculateOpaqueAndTransparentRegion004
 * @tc.desc: Test CalculateOpaqueAndTransparentRegion with multi-rsSurfaceRenderNode
 * @tc.type: FUNC
 * @tc.require: issueIBCR0E
 */
HWTEST_F(RSUniRenderVisitorTest, CalculateOpaqueAndTransparentRegion004, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    RSSurfaceRenderNodeConfig config;
    config.id = 1;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    rsUniRenderVisitor->needRecalculateOcclusion_ = true;
    rsUniRenderVisitor->isAllSurfaceVisibleDebugEnabled_ = true;
    rsUniRenderVisitor->CalculateOpaqueAndTransparentRegion(*rsSurfaceRenderNode);
}

/**
 * @tc.name: CalculateOpaqueAndTransparentRegion005
 * @tc.desc: Test CalculateOpaqueAndTransparentRegion if need skip in expand screen
 * @tc.type: FUNC
 * @tc.require: issueIBCR0E
 */
HWTEST_F(RSUniRenderVisitorTest, CalculateOpaqueAndTransparentRegion005, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    RSSurfaceRenderNodeConfig config;
    config.id = 0;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    rsSurfaceRenderNode->InitRenderParams();

    auto rsDisplayRenderNode = std::make_shared<RSScreenRenderNode>(1, 0, rsContext->weak_from_this());
    ASSERT_NE(rsDisplayRenderNode, nullptr);
    rsDisplayRenderNode->InitRenderParams();

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    
    rsUniRenderVisitor->curScreenNode_ = rsDisplayRenderNode;
    rsUniRenderVisitor->InitScreenInfo(*rsDisplayRenderNode);
    ASSERT_NE(rsUniRenderVisitor->curScreenNode_, nullptr);
    rsUniRenderVisitor->curScreenNode_->SetIsFirstVisitCrossNodeDisplay(true);
    NodeId id = 0;
    RSDisplayNodeConfig displayNodeConfig;
    auto node = std::make_shared<RSLogicalDisplayRenderNode>(id, displayNodeConfig);
    rsUniRenderVisitor->curLogicalDisplayNode_ = node;
    node->InitRenderParams();
    rsUniRenderVisitor->CalculateOpaqueAndTransparentRegion(*rsSurfaceRenderNode);

    rsUniRenderVisitor->curScreenNode_->SetIsFirstVisitCrossNodeDisplay(false);
    rsUniRenderVisitor->CalculateOpaqueAndTransparentRegion(*rsSurfaceRenderNode);

    rsSurfaceRenderNode->isCrossNode_ = true;
    rsSurfaceRenderNode->firstLevelNodeId_ = config.id;
    rsUniRenderVisitor->CalculateOpaqueAndTransparentRegion(*rsSurfaceRenderNode);
}

/**
 * @tc.name: CalculateOpaqueAndTransparentRegion006
 * @tc.desc: Test CalculateOpaqueAndTransparentRegion, filter rect should be accumulated.
 * @tc.type: FUNC
 * @tc.require: issueIC9HNQ
 */
HWTEST_F(RSUniRenderVisitorTest, CalculateOpaqueAndTransparentRegion006, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    RSSurfaceRenderNodeConfig config;
    auto rsSurfaceRenderNode = std::make_shared<MockRSSurfaceRenderNode>(config.id);
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    rsSurfaceRenderNode->InitRenderParams();
    ScreenId screenId = 0;
    auto rsScreenRenderNode = std::make_shared<RSScreenRenderNode>(DEFAULT_NODE_ID, screenId, rsContext);
    ASSERT_NE(rsScreenRenderNode, nullptr);
    rsScreenRenderNode->InitRenderParams();

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->InitScreenInfo(*rsScreenRenderNode);
    ASSERT_NE(rsUniRenderVisitor->curScreenNode_, nullptr);
    rsUniRenderVisitor->curScreenNode_->SetIsFirstVisitCrossNodeDisplay(true);

    auto regionFilter = Occlusion::Region(Occlusion::Rect(DEFAULT_RECT));
    rsUniRenderVisitor->accumulatedOcclusionRegionBehindWindow_ = regionFilter;

    rsUniRenderVisitor->ancestorNodeHasAnimation_ = false;
    rsUniRenderVisitor->isAllSurfaceVisibleDebugEnabled_ = false;
    EXPECT_CALL(*rsSurfaceRenderNode, CheckParticipateInOcclusion(false)).WillRepeatedly(testing::Return(true));
    EXPECT_CALL(*rsSurfaceRenderNode, NeedDrawBehindWindow()).WillRepeatedly(testing::Return(true));
    EXPECT_CALL(*rsSurfaceRenderNode, GetFilterRect()).WillRepeatedly(testing::Return(DEFAULT_FILTER_RECT));

    NodeId id = 0;
    RSDisplayNodeConfig displayNodeConfig;
    auto node = std::make_shared<RSLogicalDisplayRenderNode>(id, displayNodeConfig);
    rsUniRenderVisitor->curLogicalDisplayNode_ = node;
    node->InitRenderParams();

    rsUniRenderVisitor->CalculateOpaqueAndTransparentRegion(*rsSurfaceRenderNode);
    ASSERT_FALSE(rsUniRenderVisitor->accumulatedOcclusionRegionBehindWindow_.Sub(regionFilter).IsEmpty());
}

/**
 * @tc.name: CalculateOpaqueAndTransparentRegion007
 * @tc.desc: Test CalculateOpaqueAndTransparentRegion when node.NeedDrawBehindWindow() return false.
 * @tc.type: FUNC
 * @tc.require: issueIC9HNQ
 */
HWTEST_F(RSUniRenderVisitorTest, CalculateOpaqueAndTransparentRegion007, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    RSSurfaceRenderNodeConfig config;
    auto rsSurfaceRenderNode = std::make_shared<MockRSSurfaceRenderNode>(config.id);
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    rsSurfaceRenderNode->InitRenderParams();
    ScreenId screenId = 0;
    auto rsScreenRenderNode = std::make_shared<RSScreenRenderNode>(DEFAULT_NODE_ID, screenId, rsContext);
    ASSERT_NE(rsScreenRenderNode, nullptr);
    rsScreenRenderNode->InitRenderParams();

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->InitScreenInfo(*rsScreenRenderNode);
    ASSERT_NE(rsUniRenderVisitor->curScreenNode_, nullptr);
    rsUniRenderVisitor->curScreenNode_->SetIsFirstVisitCrossNodeDisplay(true);

    auto regionFilter = Occlusion::Region(Occlusion::Rect(DEFAULT_RECT));
    rsUniRenderVisitor->accumulatedOcclusionRegionBehindWindow_ = regionFilter;

    rsUniRenderVisitor->ancestorNodeHasAnimation_ = false;
    rsUniRenderVisitor->isAllSurfaceVisibleDebugEnabled_ = false;
    EXPECT_CALL(*rsSurfaceRenderNode, CheckParticipateInOcclusion(false)).WillRepeatedly(testing::Return(true));
    EXPECT_CALL(*rsSurfaceRenderNode, NeedDrawBehindWindow()).WillRepeatedly(testing::Return(false));
    EXPECT_CALL(*rsSurfaceRenderNode, GetFilterRect()).WillRepeatedly(testing::Return(DEFAULT_FILTER_RECT));

    NodeId id = 0;
    RSDisplayNodeConfig displayNodeConfig;
    auto node = std::make_shared<RSLogicalDisplayRenderNode>(id, displayNodeConfig);
    rsUniRenderVisitor->curLogicalDisplayNode_ = node;
    node->InitRenderParams();

    rsUniRenderVisitor->CalculateOpaqueAndTransparentRegion(*rsSurfaceRenderNode);
    ASSERT_TRUE(rsUniRenderVisitor->accumulatedOcclusionRegionBehindWindow_.Sub(regionFilter).IsEmpty());
}

/**
 * @tc.name: QuickPrepareCanvasRenderNode001
 * @tc.desc: Test QuickPrepareCanvasRenderNode with multi-params
 * @tc.type: FUNC
 * @tc.require: issueIASE3Z
 */
HWTEST_F(RSUniRenderVisitorTest, QuickPrepareCanvasRenderNode001, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    auto rsCanvasRenderNode = std::make_shared<RSCanvasRenderNode>(1, rsContext->weak_from_this());
    ASSERT_NE(rsCanvasRenderNode, nullptr);
    rsCanvasRenderNode->InitRenderParams();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->QuickPrepareCanvasRenderNode(*rsCanvasRenderNode);

    rsUniRenderVisitor->isDrawingCacheEnabled_ = true;
    rsUniRenderVisitor->QuickPrepareCanvasRenderNode(*rsCanvasRenderNode);
    ASSERT_TRUE(RSOpincManager::Instance().OpincGetNodeSupportFlag(*rsCanvasRenderNode));
}

/**
 * @tc.name: IsLeashAndHasMainSubNode002
 * @tc.desc: Test IsLeashAndHasMainSubNode without nullptr
 * @tc.type: FUNC
 * @tc.require: issueIASE3Z
 */
HWTEST_F(RSUniRenderVisitorTest, IsLeashAndHasMainSubNode002, TestSize.Level2)
{
    auto node = RSTestUtil::CreateSurfaceNode();
    node->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    ASSERT_FALSE(rsUniRenderVisitor->IsLeashAndHasMainSubNode(*node));
}

/**
 * @tc.name: IsLeashAndHasMainSubNode003
 * @tc.desc: Test IsLeashAndHasMainSubNode with leashAndHasMainSubNode
 * @tc.type: FUNC
 * @tc.require: issueIB20EZ
 */
HWTEST_F(RSUniRenderVisitorTest, IsLeashAndHasMainSubNode003, TestSize.Level2)
{
    auto node = RSTestUtil::CreateSurfaceNode();
    node->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    surfaceNode->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    NodeId id = 1;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(id);
    ASSERT_NE(canvasNode, nullptr);
    node->AddChild(canvasNode);
    node->GenerateFullChildrenList();
    ASSERT_EQ(rsUniRenderVisitor->IsLeashAndHasMainSubNode(*node), false);
    node->AddChild(surfaceNode);
    node->GenerateFullChildrenList();
    ASSERT_EQ(rsUniRenderVisitor->IsLeashAndHasMainSubNode(*node), true);
}

/**
 * @tc.name: UpdateSubSurfaceNodeRectInSkippedSubTree
 * @tc.desc: Test UpdateSubSurfaceNodeRectInSkippedSubTree with multi-params
 * @tc.type: FUNC
 * @tc.require: issueIB20EZ
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateSubSurfaceNodeRectInSkippedSubTree, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curSurfaceNode_ = nullptr;
    NodeId id = 1;
    auto node = std::make_shared<RSRenderNode>(id);
    ASSERT_NE(node, nullptr);
    rsUniRenderVisitor->UpdateSubSurfaceNodeRectInSkippedSubTree(*node);

    rsUniRenderVisitor->curSurfaceNode_ = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(node->GetRenderProperties().GetBoundsGeometry(), nullptr);
    rsUniRenderVisitor->UpdateSubSurfaceNodeRectInSkippedSubTree(*node);
    rsUniRenderVisitor->curSurfaceDirtyManager_ = rsUniRenderVisitor->curSurfaceNode_->GetDirtyManager();
    NodeId nodeId = 0;
    std::shared_ptr<RSSurfaceRenderNode> sharedPtr = std::make_shared<RSSurfaceRenderNode>(0);
    RSSurfaceRenderNode::WeakPtr weakPtr = sharedPtr;
    rsUniRenderVisitor->curSurfaceNode_->childSubSurfaceNodes_.insert(std::make_pair(nodeId, weakPtr));
    nodeId = 1;
    std::shared_ptr<RSSurfaceRenderNode> node2 = nullptr;
    RSSurfaceRenderNode::WeakPtr weak = node2;
    rsUniRenderVisitor->curSurfaceNode_->childSubSurfaceNodes_.insert(std::make_pair(nodeId, node2));
    rsUniRenderVisitor->UpdateSubSurfaceNodeRectInSkippedSubTree(*node);
}

/**
 * @tc.name: UpdateDisplayZoomState
 * @tc.desc: Test UpdateDisplayZoomState
 * @tc.type: FUNC
 * @tc.require: issueIB20EZ
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateDisplayZoomState, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curScreenNode_ = nullptr;
    rsUniRenderVisitor->UpdateDisplayZoomState();

    NodeId id = 1;
    auto rsContext = std::make_shared<RSContext>();
    rsUniRenderVisitor->curScreenNode_ = std::make_shared<RSScreenRenderNode>(id, 0, rsContext);
    ASSERT_NE(rsUniRenderVisitor->curScreenNode_, nullptr);
    rsUniRenderVisitor->UpdateDisplayZoomState();
    ASSERT_EQ(rsUniRenderVisitor->curScreenNode_->IsZoomStateChange(), false);
}

/**
 * @tc.name: UpdateHwcNodeInfoForAppNode002
 * @tc.desc: Test UpdateHwcNodeInfoForAppNode with multi-params
 * @tc.type: FUNC
 * @tc.require: issueIASE3Z
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeInfoForAppNode002, TestSize.Level2)
{
    auto rsSurfaceRenderNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    rsSurfaceRenderNode->needCollectHwcNode_ = true;
    rsSurfaceRenderNode->isHardwareEnabledNode_ = true;
    rsSurfaceRenderNode->isFixRotationByUser_ = false;
    rsSurfaceRenderNode->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_NODE;
    NodeId surfaceNodeId = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceNodeId);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->needCollectHwcNode_ = true;
    surfaceNode->visibleRegion_.Reset();

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;
    rsUniRenderVisitor->hwcVisitor_->isHardwareForcedDisabled_ = true;
    rsUniRenderVisitor->UpdateHwcNodeInfoForAppNode(*rsSurfaceRenderNode);

    rsUniRenderVisitor->hwcVisitor_->isHardwareForcedDisabled_ = false;
    rsSurfaceRenderNode->dynamicHardwareEnable_ = false;
    rsSurfaceRenderNode->isFixRotationByUser_ = true;
    rsUniRenderVisitor->UpdateHwcNodeInfoForAppNode(*rsSurfaceRenderNode);

    rsSurfaceRenderNode->dynamicHardwareEnable_ = true;
    rsUniRenderVisitor->UpdateHwcNodeInfoForAppNode(*rsSurfaceRenderNode);
}

/**
 * @tc.name: UpdateHwcNodeInfoForAppNode003
 * @tc.desc: Test UpdateHwcNodeInfoForAppNode with multi-params
 * @tc.type: FUNC
 * @tc.require: issueIASE3Z
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeInfoForAppNode003, TestSize.Level2)
{
    auto rsSurfaceRenderNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    rsSurfaceRenderNode->needCollectHwcNode_ = true;
    rsSurfaceRenderNode->isHardwareEnabledNode_ = true;
    rsSurfaceRenderNode->isFixRotationByUser_ = false;
    rsSurfaceRenderNode->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_NODE;
    NodeId surfaceNodeId = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceNodeId);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->needCollectHwcNode_ = true;
    surfaceNode->visibleRegion_.Reset();

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;

    rsSurfaceRenderNode->HwcSurfaceRecorder().SetIntersectWithPreviousFilter(true);
    rsUniRenderVisitor->hwcVisitor_->isHardwareForcedDisabled_ = true;
    rsUniRenderVisitor->UpdateHwcNodeInfoForAppNode(*rsSurfaceRenderNode);
}

/**
 * @tc.name: UpdateHwcNodeInfoForAppNode007
 * @tc.desc: Test UpdateHwcNodeInfoForAppNode with multi-params
 * @tc.type: FUNC
 * @tc.require: issueIASE3Z
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeInfoForAppNode007, TestSize.Level2)
{
    auto rsSurfaceRenderNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    rsSurfaceRenderNode->needCollectHwcNode_ = false;
    rsSurfaceRenderNode->isHardwareEnabledNode_ = true;
    rsSurfaceRenderNode->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_NODE;
    rsSurfaceRenderNode->renderProperties_.boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
    NodeId surfaceNodeId = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceNodeId);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->needCollectHwcNode_ = true;
    Occlusion::Region region({0, 0, 100, 100});
    surfaceNode->SetVisibleRegion(region);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    NodeId screenNodeId = 0;
    ScreenId screenId = 0;
    auto rsContext = std::make_shared<RSContext>();
    rsUniRenderVisitor->curScreenNode_ = std::make_shared<RSScreenRenderNode>(screenNodeId, screenId, rsContext);
    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;
    rsUniRenderVisitor->hwcVisitor_->isHardwareForcedDisabled_ = false;
    
    rsSurfaceRenderNode->HwcSurfaceRecorder().SetIntersectWithPreviousFilter(true);
    rsSurfaceRenderNode->SetHardwareForcedDisabledState(true);
    rsUniRenderVisitor->UpdateHwcNodeInfoForAppNode(*rsSurfaceRenderNode);
    EXPECT_TRUE(rsSurfaceRenderNode->IsHardwareForcedDisabled());

    rsSurfaceRenderNode->HwcSurfaceRecorder().SetIntersectWithPreviousFilter(true);
    rsSurfaceRenderNode->SetHardwareForcedDisabledState(false);
    rsUniRenderVisitor->UpdateHwcNodeInfoForAppNode(*rsSurfaceRenderNode);
    EXPECT_TRUE(rsSurfaceRenderNode->IsHardwareForcedDisabled());

    rsSurfaceRenderNode->HwcSurfaceRecorder().SetIntersectWithPreviousFilter(false);
    rsSurfaceRenderNode->SetHardwareForcedDisabledState(true);
    rsUniRenderVisitor->UpdateHwcNodeInfoForAppNode(*rsSurfaceRenderNode);
    EXPECT_FALSE(rsSurfaceRenderNode->IsHardwareForcedDisabled());

    rsSurfaceRenderNode->HwcSurfaceRecorder().SetIntersectWithPreviousFilter(false);
    rsSurfaceRenderNode->SetHardwareForcedDisabledState(false);
    rsUniRenderVisitor->UpdateHwcNodeInfoForAppNode(*rsSurfaceRenderNode);
    EXPECT_FALSE(rsSurfaceRenderNode->IsHardwareForcedDisabled());
}

/**
 * @tc.name: QuickPrepareWindowKeyFrameRenderNode
 * @tc.desc: Test QuickPrepareWindowKeyFrameRenderNode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniRenderVisitorTest, QuickPrepareWindowKeyFrameRenderNode, TestSize.Level2)
{
    NodeId nodeId = 1;
    NodeId surfaceNodeId = 2;
    auto keyframeNode = std::make_shared<RSWindowKeyFrameRenderNode>(nodeId);
    keyframeNode->InitRenderParams();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(keyframeNode, nullptr);
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    keyframeNode->stagingRenderParams_ = std::make_unique<RSRenderParams>(nodeId);
 
    EXPECT_EQ(rsUniRenderVisitor->curSurfaceNode_, nullptr);
    rsUniRenderVisitor->QuickPrepareWindowKeyFrameRenderNode(*keyframeNode);
 
    rsUniRenderVisitor->curSurfaceNode_ = std::make_shared<RSSurfaceRenderNode>(surfaceNodeId);
    rsUniRenderVisitor->QuickPrepareWindowKeyFrameRenderNode(*keyframeNode);
    EXPECT_EQ(keyframeNode->GetLinkedNodeId(), INVALID_NODEID);
}

/**
 * @tc.name: QuickPrepareChildren
 * @tc.desc: Test QuickPrepareChildren
 * @tc.type: FUNC
 * @tc.require: issueIB7GLO
 */
HWTEST_F(RSUniRenderVisitorTest, QuickPrepareChildren, TestSize.Level2)
{
    auto rsSurfaceRenderNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    rsSurfaceRenderNode->lastFrameSubTreeSkipped_ = true;
    auto dirtyManager = rsSurfaceRenderNode->GetDirtyManager();
    ASSERT_NE(dirtyManager, nullptr);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curSurfaceNode_ = rsSurfaceRenderNode;
    rsUniRenderVisitor->QuickPrepareChildren(*rsSurfaceRenderNode);
}

/**
 * @tc.name: QuickPrepareChildren002
 * @tc.desc: Test QuickPrepareChildren
 * @tc.type: FUNC
 * @tc.require: issue20827
 */
HWTEST_F(RSUniRenderVisitorTest, QuickPrepareChildren002, TestSize.Level2)
{
    auto rsSurfaceRenderNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    rsSurfaceRenderNode->InitRenderParams();
    rsSurfaceRenderNode->lastFrameSubTreeSkipped_ = true;
    auto dirtyManager = rsSurfaceRenderNode->GetDirtyManager();
    ASSERT_NE(dirtyManager, nullptr);

    rsSurfaceRenderNode->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    rsSurfaceRenderNode->hasRemovedChild_ = true;
    rsSurfaceRenderNode->SetParentTreeDirty(true);

    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetIsOnTheTree(true, surfaceNode->GetId(), surfaceNode->GetId());
    surfaceNode->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    surfaceNode->name_ = "TestWindow";

    rsSurfaceRenderNode->AddChild(surfaceNode, 0);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->curSurfaceNode_ = rsSurfaceRenderNode;
    rsUniRenderVisitor->QuickPrepareChildren(*rsSurfaceRenderNode);
}

/**
 * @tc.name: ProcessAncoNode001
 * @tc.desc: Test ProcessAncoNode without buffer
 * @tc.type: FUNC
 * @tc.require: issueIB7GLO
 */
HWTEST_F(RSUniRenderVisitorTest, ProcessAncoNode001, TestSize.Level2)
{
    auto hwcNodePtr = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(hwcNodePtr, nullptr);
    hwcNodePtr->SetAncoFlags(static_cast<uint32_t>(AncoFlags::IS_ANCO_NODE));
    hwcNodePtr->SetGlobalAlpha(0.0f);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    bool ancoHasGpu = false;
    rsUniRenderVisitor->ProcessAncoNode(hwcNodePtr, ancoHasGpu);

    hwcNodePtr->SetGlobalAlpha(1.0f);
    rsUniRenderVisitor->ProcessAncoNode(hwcNodePtr, ancoHasGpu);
}

/**
 * @tc.name: ProcessAncoNode002
 * @tc.desc: Test ProcessAncoNode with buffer
 * @tc.type: FUNC
 * @tc.require: issueIB7GLO
 */
HWTEST_F(RSUniRenderVisitorTest, ProcessAncoNode002, TestSize.Level2)
{
    auto hwcNodePtr = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(hwcNodePtr, nullptr);
    hwcNodePtr->SetAncoFlags(static_cast<uint32_t>(AncoFlags::IS_ANCO_NODE));

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    bool ancoHasGpu = false;
    rsUniRenderVisitor->ProcessAncoNode(hwcNodePtr, ancoHasGpu);
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

/*
 * @tc.name: UpdateVirtualDisplayInfo001
 * @tc.desc: Test RSUniRenderVisitorTest.UpdateVirtualDisplayInfo001
 * @tc.type: FUNC
 * @tc.require: issueIB2P2F
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateVirtualScreenInfo001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->screenManager_ = CreateOrGetScreenManager();
    ASSERT_NE(rsUniRenderVisitor->screenManager_, nullptr);
    auto virtualScreenId = rsUniRenderVisitor->screenManager_->CreateVirtualScreen("virtual screen 001", 0, 0, nullptr);
    ASSERT_NE(INVALID_SCREEN_ID, virtualScreenId);

    RSDisplayNodeConfig config;
    auto rsContext = std::make_shared<RSContext>();
    auto displayNode = std::make_shared<RSLogicalDisplayRenderNode>(0, config, rsContext->weak_from_this());
    ASSERT_NE(displayNode, nullptr);
    displayNode->SetScreenId(virtualScreenId);
}

/*
 * @tc.name: UpdateVirtualDisplayInfo002
 * @tc.desc: Test RSUniRenderVisitorTest.UpdateVirtualDisplayInfo002
 * @tc.type: FUNC
 * @tc.require: issueIB2P2F
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateVirtualScreenInfo002, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->screenManager_ = CreateOrGetScreenManager();
    ASSERT_NE(rsUniRenderVisitor->screenManager_, nullptr);

    ScreenId screenId = 0;
    auto rsScreen = std::make_shared<RSScreen>(HdiOutput::CreateHdiOutput(screenId));
    rsScreen->property_.SetScreenType(EXTERNAL_TYPE_SCREEN);
    rsUniRenderVisitor->screenManager_->MockHdiScreenConnected(rsScreen);
    auto rsContext = std::make_shared<RSContext>();
    auto displayNode = std::make_shared<RSScreenRenderNode>(0, 0, rsContext->weak_from_this());
    ASSERT_NE(displayNode, nullptr);
    displayNode->screenId_ = screenId;
    rsUniRenderVisitor->curScreenNode_ = displayNode;
    rsUniRenderVisitor->CheckMergeScreenDirtyByRoundCornerDisplay();

    auto &rcdInstance = RSSingleton<RoundCornerDisplayManager>::GetInstance();
    NodeId id = displayNode->GetId();
    rcdInstance.AddRoundCornerDisplay(id);
    EXPECT_TRUE(rcdInstance.CheckExist(id));
    rcdInstance.rcdMap_[id]->rcdDirtyType_ = RoundCornerDirtyType::RCD_DIRTY_ALL;
    rcdInstance.rcdMap_[id]->hardInfo_.resourceChanged = true;
    rsUniRenderVisitor->CheckMergeScreenDirtyByRoundCornerDisplay();
}

/*
 * @tc.name: UpdateAncoNodeHWCDisabledState
 * @tc.desc: Test RSUniRenderVisitorTest.UpdateAncoNodeHWCDisabledState
 * @tc.type: FUNC
 * @tc.require: IAHFXD
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateAncoNodeHWCDisabledState, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto hwcNodePtr = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(hwcNodePtr, nullptr);
    hwcNodePtr->SetAncoFlags(static_cast<uint32_t>(AncoFlags::IS_ANCO_NODE));
    std::vector<std::shared_ptr<RSSurfaceRenderNode>> ancoNodes;
    ancoNodes.push_back(hwcNodePtr);
    rsUniRenderVisitor->UpdateAncoNodeHWCDisabledState(ancoNodes);
}

/*
 * @tc.name: UpdatePointWindowDirtyStatus001
 * @tc.desc: Test UpdatePointWindowDirtyStatus while pointSurfaceHandler isn't nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBE0XP
 */
HWTEST_F(RSUniRenderVisitorTest, UpdatePointWindowDirtyStatus001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(0);
    surfaceNode->InitRenderParams();
    surfaceNode->nodeType_ = RSSurfaceNodeType::CURSOR_NODE;
    surfaceNode->name_ = "pointer window";

    ASSERT_NE(surfaceNode->GetMutableRSSurfaceHandler(), nullptr);

    rsUniRenderVisitor->UpdatePointWindowDirtyStatus(surfaceNode);
    ASSERT_EQ(
        static_cast<float>(TopLayerZOrder::POINTER_WINDOW), surfaceNode->GetMutableRSSurfaceHandler()->globalZOrder_);
}

/*
 * @tc.name: MarkBlurIntersectWithDRM002
 * @tc.desc: Test MarkBlurIntersectWithDRM while appWindowNode isn't nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBE0XP
 */
HWTEST_F(RSUniRenderVisitorTest, MarkBlurIntersectWithDRM002, TestSize.Level2)
{
    DRMParam::AddWhiteList("SCBBannerNotification");
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    surfaceConfig.name = "SCBBannerNotification";
    surfaceConfig.surfaceWindowType = SurfaceWindowType::SCB_BANNER_NOTIFICATION;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->instanceRootNodeId_ = surfaceNode->GetId();
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().RegisterRenderNode(surfaceNode);
    std::shared_ptr<RSSurfaceRenderNode> drmNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(drmNode, nullptr);

    // let drm intersect with blur
    surfaceNode->GetFilterRegionInfo().filterRegion_ = RectT(0, 0, 1, 1);
    drmNode->GetRenderProperties().GetBoundsGeometry()->absRect_ = RectT(0, 0, 1, 1);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->drmNodes_.emplace_back(drmNode);
    rsUniRenderVisitor->MarkBlurIntersectWithDRM(surfaceNode);
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().UnregisterRenderNode(1);
}

/*
 * @tc.name: MarkBlurIntersectWithDRM003
 * @tc.desc: Test MarkBlurIntersectWithDRM while appWindowNode isn't nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBE0XP
 */
HWTEST_F(RSUniRenderVisitorTest, MarkBlurIntersectWithDRM003, TestSize.Level2)
{
    DRMParam::AddWhiteList("SCBBannerNotification");
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    surfaceConfig.name = "SCBBannerNotification";
    surfaceConfig.surfaceWindowType = SurfaceWindowType::SCB_BANNER_NOTIFICATION;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->instanceRootNodeId_ = surfaceNode->GetId();
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().RegisterRenderNode(surfaceNode);
    std::shared_ptr<RSSurfaceRenderNode> drmNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(drmNode, nullptr);

    // let drm not intersect with blur
    surfaceNode->GetFilterRegionInfo().filterRegion_ = RectT(2, 2, 3, 3);
    drmNode->GetRenderProperties().GetBoundsGeometry()->absRect_ = RectT(0, 0, 1, 1);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->drmNodes_.emplace_back(drmNode);
    rsUniRenderVisitor->MarkBlurIntersectWithDRM(surfaceNode);
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().UnregisterRenderNode(1);
}

/*
 * @tc.name: MarkBlurIntersectWithDRM004
 * @tc.desc: Test MarkBlurIntersectWithDRM while appWindowNode isn't nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBE0XP
 */
HWTEST_F(RSUniRenderVisitorTest, MarkBlurIntersectWithDRM004, TestSize.Level2)
{
    DRMParam::AddWhiteList("SCBBannerNotification");
    auto rsContext = std::make_shared<RSContext>();
    NodeId id = 1;
    auto effectNode = std::make_shared<RSEffectRenderNode>(id, rsContext->weak_from_this());
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 21;
    surfaceConfig.name = "SCBBannerNotification";
    surfaceConfig.surfaceWindowType = SurfaceWindowType::SCB_BANNER_NOTIFICATION;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode, nullptr);
    effectNode->instanceRootNodeId_ = surfaceNode->GetId();
    surfaceNode->instanceRootNodeId_ = surfaceNode->GetId();
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().RegisterRenderNode(surfaceNode);
    std::shared_ptr<RSSurfaceRenderNode> drmNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(drmNode, nullptr);

    // let drm intersect with blur
    surfaceNode->GetFilterRegionInfo().filterRegion_ = RectT(0, 0, 1, 1);
    drmNode->GetRenderProperties().GetBoundsGeometry()->absRect_ = RectT(0, 0, 1, 1);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->drmNodes_.emplace_back(drmNode);
    rsUniRenderVisitor->MarkBlurIntersectWithDRM(effectNode);
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().UnregisterRenderNode(21);
}

/*
 * @tc.name: MarkBlurIntersectWithDRM005
 * @tc.desc: Test MarkBlurIntersectWithDRM while appWindowNode isn't nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBE0XP
 */
HWTEST_F(RSUniRenderVisitorTest, MarkBlurIntersectWithDRM005, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    NodeId id = 1;
    auto effectNode = std::make_shared<RSEffectRenderNode>(id, rsContext->weak_from_this());
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 22;
    surfaceConfig.name = "SCBCloneWallpaper";
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode, nullptr);
    effectNode->instanceRootNodeId_ = surfaceNode->GetId();
    surfaceNode->instanceRootNodeId_ = surfaceNode->GetId();
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().RegisterRenderNode(surfaceNode);
    std::shared_ptr<RSSurfaceRenderNode> drmNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(drmNode, nullptr);

    // let drm intersect with blur
    surfaceNode->GetFilterRegionInfo().filterRegion_ = RectT(0, 0, 1, 1);
    drmNode->GetRenderProperties().GetBoundsGeometry()->absRect_ = RectT(0, 0, 1, 1);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->drmNodes_.emplace_back(drmNode);
    rsUniRenderVisitor->MarkBlurIntersectWithDRM(effectNode);
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().UnregisterRenderNode(22);
}

/*
 * @tc.name: IsValidInVirtualScreen001
 * @tc.desc: Test IsValidInVirtualScreen with normal layer
 * @tc.type: FUNC
 * @tc.require: issueIBR5DD
 */
HWTEST_F(RSUniRenderVisitorTest, IsValidInVirtualScreen001, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetIsOnTheTree(true, surfaceNode->GetId(), surfaceNode->GetId());
    surfaceNode->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->allBlackList_.clear();
    rsUniRenderVisitor->allWhiteList_.clear();
    EXPECT_EQ(rsUniRenderVisitor->IsValidInVirtualScreen(*surfaceNode), true);
}

/*
 * @tc.name: IsValidInVirtualScreen002
 * @tc.desc: Test IsValidInVirtualScreen with security layer
 * @tc.type: FUNC
 * @tc.require: issueIBR5DD
 */
HWTEST_F(RSUniRenderVisitorTest, IsValidInVirtualScreen002, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetSecurityLayer(true);
    surfaceNode->SetIsOnTheTree(true, surfaceNode->GetId(), surfaceNode->GetId());
    surfaceNode->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    EXPECT_EQ(rsUniRenderVisitor->IsValidInVirtualScreen(*surfaceNode), false);
}

/*
 * @tc.name: IsValidInVirtualScreen003
 * @tc.desc: Test IsValidInVirtualScreen with skip layer
 * @tc.type: FUNC
 * @tc.require: issueIBR5DD
 */
HWTEST_F(RSUniRenderVisitorTest, IsValidInVirtualScreen003, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetSkipLayer(true);
    surfaceNode->SetIsOnTheTree(true, surfaceNode->GetId(), surfaceNode->GetId());
    surfaceNode->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    EXPECT_EQ(rsUniRenderVisitor->IsValidInVirtualScreen(*surfaceNode), false);
}

/*
 * @tc.name: IsValidInVirtualScreen004
 * @tc.desc: Test IsValidInVirtualScreen with protected layer
 * @tc.type: FUNC
 * @tc.require: issueIBR5DD
 */
HWTEST_F(RSUniRenderVisitorTest, IsValidInVirtualScreen004, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetProtectedLayer(true);
    surfaceNode->SetIsOnTheTree(true, surfaceNode->GetId(), surfaceNode->GetId());
    surfaceNode->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    EXPECT_EQ(rsUniRenderVisitor->IsValidInVirtualScreen(*surfaceNode), false);
}

/*
 * @tc.name: IsValidInVirtualScreen005
 * @tc.desc: Test IsValidInVirtualScreen with snapshot skip layer
 * @tc.type: FUNC
 * @tc.require: issueIBR5DD
 */
HWTEST_F(RSUniRenderVisitorTest, IsValidInVirtualScreen005, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetSnapshotSkipLayer(true);
    surfaceNode->SetIsOnTheTree(true, surfaceNode->GetId(), surfaceNode->GetId());
    surfaceNode->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    EXPECT_EQ(rsUniRenderVisitor->IsValidInVirtualScreen(*surfaceNode), false);
}

/*
 * @tc.name: IsValidInVirtualScreen006
 * @tc.desc: Test IsValidInVirtualScreen with whitelist cast
 * @tc.type: FUNC
 * @tc.require: issueIBR5DD
 */
HWTEST_F(RSUniRenderVisitorTest, IsValidInVirtualScreen006, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetIsOnTheTree(true, surfaceNode->GetId(), surfaceNode->GetId());
    surfaceNode->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->allWhiteList_.emplace(surfaceNode->GetId());
    EXPECT_EQ(rsUniRenderVisitor->IsValidInVirtualScreen(*surfaceNode), false);
    rsUniRenderVisitor->allWhiteList_.clear();
}

/*
 * @tc.name: IsValidInVirtualScreen007
 * @tc.desc: Test IsValidInVirtualScreen with surface is in blacklist
 * @tc.type: FUNC
 * @tc.require: issueIBR5DD
 */
HWTEST_F(RSUniRenderVisitorTest, IsValidInVirtualScreen007, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetIsOnTheTree(true, surfaceNode->GetId(), surfaceNode->GetId());
    surfaceNode->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->allBlackList_.emplace(surfaceNode->GetId());
    EXPECT_EQ(rsUniRenderVisitor->IsValidInVirtualScreen(*surfaceNode), false);
    rsUniRenderVisitor->allBlackList_.clear();
}

/*
 * @tc.name: IsValidInVirtualScreen008
 * @tc.desc: Test IsValidInVirtualScreen with surface is not in blacklist
 * @tc.type: FUNC
 * @tc.require: issueIBR5DD
 */
HWTEST_F(RSUniRenderVisitorTest, IsValidInVirtualScreen008, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetIsOnTheTree(true, surfaceNode->GetId(), surfaceNode->GetId());
    surfaceNode->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto otherSurfaceNodeId = surfaceNode->GetId();
    ++otherSurfaceNodeId;
    rsUniRenderVisitor->allBlackList_.emplace(otherSurfaceNodeId);
    EXPECT_EQ(rsUniRenderVisitor->IsValidInVirtualScreen(*surfaceNode), true);
    rsUniRenderVisitor->allBlackList_.clear();
}

/*
 * @tc.name: IsValidInVirtualScreen009
 * @tc.desc: Test IsValidInVirtualScreen with is isSkipDrawInVirtualScreen_
 * @tc.type: FUNC
 * @tc.require: issueICNBRB
 */
HWTEST_F(RSUniRenderVisitorTest, IsValidInVirtualScreen009, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetSecurityLayer(true);
    surfaceNode->SetIsOnTheTree(true, surfaceNode->GetId(), surfaceNode->GetId());
    surfaceNode->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->isSkipDrawInVirtualScreen_ = true;
    EXPECT_EQ(rsUniRenderVisitor->IsValidInVirtualScreen(*surfaceNode), false);
    rsUniRenderVisitor->isSkipDrawInVirtualScreen_ = false;
    EXPECT_EQ(rsUniRenderVisitor->IsValidInVirtualScreen(*surfaceNode), false);
}

/*
 * @tc.name: CheckIfSkipDrawInVirtualScreen
 * @tc.desc: Test CheckIfSkipDrawInVirtualScreen with is isSkipDrawInVirtualScreen_
 * @tc.type: FUNC
 * @tc.require: issueICNBRB
 */
HWTEST_F(RSUniRenderVisitorTest, CheckIfSkipDrawInVirtualScreen, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    rsUniRenderVisitor->allBlackList_.emplace(surfaceNode->GetId());
    ASSERT_TRUE(rsUniRenderVisitor->CheckIfSkipDrawInVirtualScreen(*surfaceNode));
    rsUniRenderVisitor->allBlackList_.clear();
    NodeId leashPersistentId = 5;
    surfaceNode->SetLeashPersistentId(leashPersistentId);
    rsUniRenderVisitor->allBlackList_.emplace(surfaceNode->GetLeashPersistentId());
    ASSERT_TRUE(rsUniRenderVisitor->CheckIfSkipDrawInVirtualScreen(*surfaceNode));
    rsUniRenderVisitor->allBlackList_.clear();
}

/*
 * @tc.name: CheckFilterNodeInSkippedSubTreeNeedClearCache001
 * @tc.desc: Test CheckFilterNodeInSkippedSubTreeNeedClearCache while effectNode with fiter
 * @tc.type: FUNC
 * @tc.require: issueIBE0XP
 */
HWTEST_F(RSUniRenderVisitorTest, CheckFilterNodeInSkippedSubTreeNeedClearCache001, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    auto rsRootRenderNode = std::make_shared<RSSurfaceRenderNode>(0, rsContext->weak_from_this());
    ASSERT_NE(rsRootRenderNode, nullptr);
    rsRootRenderNode->InitRenderParams();
    auto effectNode = std::make_shared<RSEffectRenderNode>(1);
    ASSERT_NE(effectNode, nullptr);
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().RegisterRenderNode(effectNode);
    effectNode->GetMutableRenderProperties().backgroundFilter_ = std::make_shared<RSFilter>();
    effectNode->GetMutableRenderProperties().needFilter_ = true;
    rsRootRenderNode->UpdateVisibleFilterChild(*effectNode);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curScreenNode_ = std::make_shared<RSScreenRenderNode>(0, 0, rsContext->weak_from_this());
    ASSERT_NE(rsUniRenderVisitor->curScreenNode_, nullptr);
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSLogicalDisplayRenderNode>(id, config);
    rsUniRenderVisitor->curLogicalDisplayNode_ = node;
    node->InitRenderParams();

    RSDirtyRegionManager dirtyManager;
    rsUniRenderVisitor->CheckFilterNodeInSkippedSubTreeNeedClearCache(*rsRootRenderNode, dirtyManager);
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().UnregisterRenderNode(1);
}

/*
 * @tc.name: CheckFilterNodeInSkippedSubTreeNeedClearCache002
 * @tc.desc: Test CheckFilterNodeInSkippedSubTreeNeedClearCache while surfaceNode not included in the nodemap
 * @tc.type: FUNC
 * @tc.require: issueIBE0XP
 */
HWTEST_F(RSUniRenderVisitorTest, CheckFilterNodeInSkippedSubTreeNeedClearCache002, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    auto rsRootRenderNode = std::make_shared<RSSurfaceRenderNode>(0, rsContext->weak_from_this());
    ASSERT_NE(rsRootRenderNode, nullptr);
    rsRootRenderNode->InitRenderParams();

    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    surfaceNode->GetMutableRenderProperties().needFilter_ = true;
    rsRootRenderNode->UpdateVisibleFilterChild(*surfaceNode);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    NodeId id = 0;
    RSDisplayNodeConfig displayNodeConfig;
    auto node = std::make_shared<RSLogicalDisplayRenderNode>(id, displayNodeConfig);
    rsUniRenderVisitor->curLogicalDisplayNode_ = node;
    node->InitRenderParams();

    RSDirtyRegionManager dirtyManager;
    rsUniRenderVisitor->CheckFilterNodeInSkippedSubTreeNeedClearCache(*rsRootRenderNode, dirtyManager);
}

/*
 * @tc.name: SetHdrWhenMultiDisplayChangeTest
 * @tc.desc: Test SetHdrWhenMultiDisplayChangeTest
 * @tc.type: FUNC
 * @tc.require: issueIBF9OU
 */
HWTEST_F(RSUniRenderVisitorTest, SetHdrWhenMultiDisplayChangeTest, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->SetHdrWhenMultiDisplayChange();
}

/*
 * @tc.name: CheckFilterNodeInSkippedSubTreeNeedClearCache003
 * @tc.desc: Test CheckFilterNodeInSkippedSubTreeNeedClearCache while canvasNode with filter
 * @tc.type: FUNC
 * @tc.require: issuesIBE0XP
 */
HWTEST_F(RSUniRenderVisitorTest, CheckFilterNodeInSkippedSubTreeNeedClearCache003, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    auto rsRootRenderNode = std::make_shared<RSSurfaceRenderNode>(0, rsContext->weak_from_this());
    ASSERT_NE(rsRootRenderNode, nullptr);
    rsRootRenderNode->InitRenderParams();
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(1);
    ASSERT_NE(canvasNode, nullptr);
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().RegisterRenderNode(canvasNode);
    canvasNode->GetMutableRenderProperties().backgroundFilter_ = std::make_shared<RSFilter>();
    canvasNode->GetMutableRenderProperties().needFilter_ = true;
    rsRootRenderNode->UpdateVisibleFilterChild(*canvasNode);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curScreenNode_ = std::make_shared<RSScreenRenderNode>(0, 0, rsContext->weak_from_this());
    ASSERT_NE(rsUniRenderVisitor->curScreenNode_, nullptr);

    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSLogicalDisplayRenderNode>(id, config);
    rsUniRenderVisitor->curLogicalDisplayNode_ = node;
    node->InitRenderParams();

    RSDirtyRegionManager dirtyManager;
    rsUniRenderVisitor->CheckFilterNodeInSkippedSubTreeNeedClearCache(*rsRootRenderNode, dirtyManager);
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().UnregisterRenderNode(1);
}

/*
 * @tc.name: CheckFilterNodeInSkippedSubTreeNeedClearCache004
 * @tc.desc: Test CheckFilterNodeInSkippedSubTreeNeedClearCache
 * @tc.type: FUNC
 * @tc.require: issues21186
 */
HWTEST_F(RSUniRenderVisitorTest, CheckFilterNodeInSkippedSubTreeNeedClearCache004, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->curScreenNode_ = std::make_shared<RSScreenRenderNode>(0, 0, rsContext->weak_from_this());

    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSLogicalDisplayRenderNode>(id, config);
    rsUniRenderVisitor->curLogicalDisplayNode_ = node;
    node->InitRenderParams();

    auto rsRootRenderNode = std::make_shared<RSSurfaceRenderNode>(0, rsContext->weak_from_this());
    rsRootRenderNode->InitRenderParams();

    auto canvasNode = std::make_shared<RSCanvasRenderNode>(1);
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().RegisterRenderNode(canvasNode);
    canvasNode->GetMutableRenderProperties().backgroundFilter_ = std::make_shared<RSFilter>();
    canvasNode->GetMutableRenderProperties().needFilter_ = true;
    rsRootRenderNode->UpdateVisibleFilterChild(*canvasNode);

    auto effectNode = std::make_shared<RSEffectRenderNode>(2);
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().RegisterRenderNode(effectNode);
    canvasNode->GetMutableRenderProperties().backgroundFilter_ = std::make_shared<RSFilter>();
    canvasNode->GetMutableRenderProperties().needFilter_ = true;
    rsRootRenderNode->UpdateVisibleFilterChild(*effectNode);

    RSDirtyRegionManager dirtyManager;
    rsUniRenderVisitor->CheckFilterNodeInSkippedSubTreeNeedClearCache(*rsRootRenderNode, dirtyManager);
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().UnregisterRenderNode(1);
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().UnregisterRenderNode(2);

    EXPECT_FALSE(effectNode->ChildHasVisibleEffectWithoutEmptyRect());
}

/*
 * @tc.name: CheckFilterNodeInOccludedSkippedSubTreeNeedClearCache001
 * @tc.desc: Test CheckFilterNodeInOccludedSkippedSubTreeNeedClearCache
 * @tc.type: FUNC
 * @tc.require: issues21301
 */
HWTEST_F(RSUniRenderVisitorTest, CheckFilterNodeInOccludedSkippedSubTreeNeedClearCache001, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();

    auto rsRootRenderNode = std::make_shared<RSSurfaceRenderNode>(0, rsContext->weak_from_this());
    rsRootRenderNode->InitRenderParams();

    auto canvasNode1 = std::make_shared<RSCanvasRenderNode>(1);
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().RegisterRenderNode(canvasNode1);
    canvasNode1->GetMutableRenderProperties().GetEffect().useEffect_ = true;
    canvasNode1->GetMutableRenderProperties().needFilter_ = true;
    rsRootRenderNode->UpdateVisibleFilterChild(*canvasNode1);
    rsRootRenderNode->UpdateVisibleEffectChild(*canvasNode1);

    auto canvasNode2 = std::make_shared<RSCanvasRenderNode>(2);
    canvasNode2->GetMutableRenderProperties().GetEffect().useEffect_ = true;
    canvasNode2->GetMutableRenderProperties().needFilter_ = true;
    rsRootRenderNode->UpdateVisibleFilterChild(*canvasNode2);
    rsRootRenderNode->UpdateVisibleEffectChild(*canvasNode1);

    auto effectNode = std::make_shared<RSEffectRenderNode>(3);
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().RegisterRenderNode(effectNode);
    effectNode->GetMutableRenderProperties().backgroundFilter_ = std::make_shared<RSFilter>();
    effectNode->GetMutableRenderProperties().needFilter_ = true;
    rsRootRenderNode->UpdateVisibleFilterChild(*effectNode);

    RSDirtyRegionManager dirtyManager;
    rsUniRenderVisitor->CheckFilterNodeInOccludedSkippedSubTreeNeedClearCache(*rsRootRenderNode, dirtyManager);
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().UnregisterRenderNode(1);
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().UnregisterRenderNode(3);

    EXPECT_FALSE(dirtyManager.GetFilterCollector().GetFilterDirtyRegionInfoList(false).empty());
}

/*
 * @tc.name: TryNotifyUIBufferAvailable
 * @tc.desc: Test RSUniRenderVisitorTest.TryNotifyUIBufferAvailable test
 * @tc.type: FUNC
 * @tc.require: issuesIBSNHZ
 */
HWTEST_F(RSUniRenderVisitorTest, TryNotifyUIBufferAvailable, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->TryNotifyUIBufferAvailable();
    rsUniRenderVisitor->uiBufferAvailableId_.emplace_back(1);
    auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(1000);
    nodeMap.RegisterRenderNode(surfaceNode);
    ASSERT_NE(nodeMap.GetRenderNode<RSSurfaceRenderNode>(1000), nullptr);
    rsUniRenderVisitor->TryNotifyUIBufferAvailable();
    EXPECT_TRUE(surfaceNode->isNotifyUIBufferAvailable_);
}

/*
 * @tc.name: CheckMergeDebugRectforRefreshRate
 * @tc.desc: Test RSUniRenderVisitorTest.CheckMergeDebugRectforRefreshRate test
 * @tc.type: FUNC
 * @tc.require: issuesIBSNHZ
 */
HWTEST_F(RSUniRenderVisitorTest, CheckMergeDebugRectforRefreshRate, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsContext = std::make_shared<RSContext>();
    rsUniRenderVisitor->curScreenNode_ = std::make_shared<RSScreenRenderNode>(1, 0, rsContext);
    auto& instance = RSRealtimeRefreshRateManager::Instance();
    instance.showEnabled_ = true;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(0);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    std::vector<RSBaseRenderNode::SharedPtr> surfaces = {canvasNode, surfaceNode};
    ASSERT_NE(rsUniRenderVisitor->curScreenNode_, nullptr);
    ASSERT_NE(rsUniRenderVisitor->curScreenNode_->GetDirtyManager(), nullptr);
    rsUniRenderVisitor->CheckMergeDebugRectforRefreshRate(surfaces);
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 10;
    surfaceConfig.name = "SCBGestureBack";
    surfaceConfig.surfaceWindowType = SurfaceWindowType::SCB_GESTURE_BACK;
    auto surfaceNode2 = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    std::vector<RSBaseRenderNode::SharedPtr> surfaces2 = {surfaceNode2};
    rsUniRenderVisitor->CheckMergeDebugRectforRefreshRate(surfaces2);
    ASSERT_FALSE(rsUniRenderVisitor->curScreenNode_->GetDirtyManager()->currentFrameDirtyRegion_.IsEmpty());
    instance.showEnabled_ = false;
}

/*
 * @tc.name: SetUniRenderThreadParam
 * @tc.desc: Test RSUniRenderVisitorTest.SetUniRenderThreadParam test
 * @tc.type: FUNC
 * @tc.require: issuesIBSNHZ
 */
HWTEST_F(RSUniRenderVisitorTest, SetUniRenderThreadParam, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    std::unique_ptr<RSRenderThreadParams> renderThreadParams = std::make_unique<RSRenderThreadParams>();
    ASSERT_NE(renderThreadParams, nullptr);
    rsUniRenderVisitor->SetUniRenderThreadParam(renderThreadParams);
    EXPECT_FALSE(renderThreadParams->isPartialRenderEnabled_);
}


/*
 * @tc.name: CollectSelfDrawingNodeRectInfoTest
 * @tc.desc: Test CollectSelfDrawingNodeRectInfo
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniRenderVisitorTest, CollectSelfDrawingNodeRectInfoTest, TestSize.Level2)
{
    RSSurfaceRenderNode node(1);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->CollectSelfDrawingNodeRectInfo(node);
    ASSERT_EQ(SelfDrawingNodeMonitor::GetInstance().curRect_.size(), 0);
}

/*
 * @tc.name: UpdateAncoPrepareClip
 * @tc.desc: Test UpdateAncoPrepareClip001, clipToBounds_ & clipToFrame_ = true;
 * @tc.type: FUNC
 * @tc.require: issueIAJSIS
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateAncoPrepareClip001, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto hwcNodePtr = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(hwcNodePtr, nullptr);
    hwcNodePtr->SetAncoFlags(static_cast<uint32_t>(AncoFlags::ANCO_SFV_NODE));
    hwcNodePtr->GetMutableRenderProperties().clipToBounds_ = true;
    hwcNodePtr->GetMutableRenderProperties().clipToFrame_ = true;
    hwcNodePtr->SetAncoFlags(static_cast<uint32_t>(AncoFlags::IS_ANCO_NODE));
}

/*
 * @tc.name: UpdateHwcNodesIfVisibleForAppTest
 * @tc.desc: Test UpdateHwcNodesIfVisibleForAppTest;
 * @tc.type: FUNC
 * @tc.require: issueIC0AQO
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodesIfVisibleForAppTest, TestSize.Level2)
{
    bool hasVisibleHwcNodes = false;
    bool needForceUpdateHwcNodes = false;
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceConfig.id = 2;
    auto opacitySurfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(opacitySurfaceNode, nullptr);

    surfaceNode->SetNodeHasBackgroundColorAlpha(true);
    surfaceNode->SetHardwareEnableHint(true);
    surfaceNode->SetDstRect({10, 1, 100, 100});
    opacitySurfaceNode->SetDstRect({10, 1, 100, 100});
    opacitySurfaceNode->SetHardwareForcedDisabledState(true);
    surfaceNode->SetIsOntheTreeOnlyFlag(true);
    opacitySurfaceNode->SetIsOntheTreeOnlyFlag(true);
    Occlusion::Region region({100, 50, 1000, 1500});
    surfaceNode->SetVisibleRegion(region);

    std::vector<std::weak_ptr<RSSurfaceRenderNode>> hwcNodes;
    hwcNodes.push_back(std::weak_ptr<RSSurfaceRenderNode>(opacitySurfaceNode));
    hwcNodes.push_back(std::weak_ptr<RSSurfaceRenderNode>(surfaceNode));
    rsUniRenderVisitor->UpdateHwcNodesIfVisibleForApp(surfaceNode, hwcNodes, hasVisibleHwcNodes,
        needForceUpdateHwcNodes);
    EXPECT_FALSE(hasVisibleHwcNodes);

    Occlusion::Region region2({100, 1200, 1000, 1500});
    surfaceNode->SetVisibleRegion(region2);
    hwcNodes.clear();
    hwcNodes.push_back(std::weak_ptr<RSSurfaceRenderNode>(opacitySurfaceNode));
    hwcNodes.push_back(std::weak_ptr<RSSurfaceRenderNode>(surfaceNode));
    hasVisibleHwcNodes = false;
    rsUniRenderVisitor->UpdateHwcNodesIfVisibleForApp(surfaceNode, hwcNodes, hasVisibleHwcNodes,
        needForceUpdateHwcNodes);
    EXPECT_FALSE(hasVisibleHwcNodes);
}

/**
 * @tc.name: UpdateHwcNodesIfVisibleForAppTest002
 * @tc.desc: Test UpdateHwcNodesIfVisibleForAppTest002;
 * @tc.type: FUNC
 * @tc.require: issueIC0AQO
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodesIfVisibleForAppTest002, TestSize.Level2)
{
    bool hasVisibleHwcNodes = false;
    bool needForceUpdateHwcNodes = false;
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode, nullptr);
   
    InitTestSurfaceNodeAndScreenInfo(surfaceNode, rsUniRenderVisitor);

    std::vector<std::weak_ptr<RSSurfaceRenderNode>> hwcNodes;
    hwcNodes.push_back(std::weak_ptr<RSSurfaceRenderNode>(surfaceNode));

    surfaceNode->isHwcGlobalPositionEnabled_ = false;
    surfaceNode->isHwcCrossNode_ = false;
    surfaceNode->GetMultableSpecialLayerMgr().Set(SpecialLayerType::PROTECTED, false);
    surfaceNode->SetLayerTop(false);
    NodeId id = 0;
    surfaceNode->surfaceHandler_ = std::make_shared<RSSurfaceHandler>(id);

    Occlusion::Region region2({50, 50, 1000, 1500});
    surfaceNode->SetVisibleRegion(region2);
    
    rsUniRenderVisitor->UpdateHwcNodesIfVisibleForApp(surfaceNode, hwcNodes, hasVisibleHwcNodes,
        needForceUpdateHwcNodes);
    EXPECT_TRUE(hasVisibleHwcNodes);

    hasVisibleHwcNodes = false;
    Occlusion::Region emptyRegion({0, 0, 0, 0});
    surfaceNode->SetVisibleRegion(emptyRegion);
    rsUniRenderVisitor->UpdateHwcNodesIfVisibleForApp(surfaceNode, hwcNodes, hasVisibleHwcNodes,
        needForceUpdateHwcNodes);
    EXPECT_FALSE(hasVisibleHwcNodes);
}

/**
 * @tc.name: UpdateHwcNodesIfVisibleForAppTest003
 * @tc.desc: Test UpdateHwcNodesIfVisibleForAppTest003;
 * @tc.type: FUNC
 * @tc.require: issueIC0AQO
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodesIfVisibleForAppTest003, TestSize.Level2)
{
    bool hasVisibleHwcNodes = false;
    bool needForceUpdateHwcNodes = false;
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode, nullptr);
   
    InitTestSurfaceNodeAndScreenInfo(surfaceNode, rsUniRenderVisitor);

    std::vector<std::weak_ptr<RSSurfaceRenderNode>> hwcNodes;
    hwcNodes.push_back(std::weak_ptr<RSSurfaceRenderNode>(surfaceNode));
    NodeId id = 0;
    surfaceNode->surfaceHandler_ = std::make_shared<RSSurfaceHandler>(id);

    surfaceNode->isHwcGlobalPositionEnabled_ = true;
    surfaceNode->isHwcCrossNode_ = false;
    surfaceNode->GetMultableSpecialLayerMgr().Set(SpecialLayerType::PROTECTED, false);
    surfaceNode->SetLayerTop(false);
    rsUniRenderVisitor->UpdateHwcNodesIfVisibleForApp(surfaceNode, hwcNodes, hasVisibleHwcNodes,
        needForceUpdateHwcNodes);
    EXPECT_TRUE(needForceUpdateHwcNodes);

    needForceUpdateHwcNodes = false;
    surfaceNode->isHwcGlobalPositionEnabled_ = false;
    surfaceNode->isHwcCrossNode_ = true;
    surfaceNode->GetMultableSpecialLayerMgr().Set(SpecialLayerType::PROTECTED, false);
    surfaceNode->SetLayerTop(false);
    rsUniRenderVisitor->UpdateHwcNodesIfVisibleForApp(surfaceNode, hwcNodes, hasVisibleHwcNodes,
        needForceUpdateHwcNodes);
    EXPECT_TRUE(needForceUpdateHwcNodes);

    needForceUpdateHwcNodes = false;
    surfaceNode->isHwcGlobalPositionEnabled_ = false;
    surfaceNode->isHwcCrossNode_ = false;
    surfaceNode->GetMultableSpecialLayerMgr().Set(SpecialLayerType::PROTECTED, true);
    surfaceNode->SetLayerTop(false);
    rsUniRenderVisitor->UpdateHwcNodesIfVisibleForApp(surfaceNode, hwcNodes, hasVisibleHwcNodes,
        needForceUpdateHwcNodes);
    EXPECT_TRUE(needForceUpdateHwcNodes);

    needForceUpdateHwcNodes = false;
    surfaceNode->isHwcGlobalPositionEnabled_ = false;
    surfaceNode->isHwcCrossNode_ = false;
    surfaceNode->GetMultableSpecialLayerMgr().Set(SpecialLayerType::PROTECTED, false);
    surfaceNode->SetLayerTop(true);
    rsUniRenderVisitor->UpdateHwcNodesIfVisibleForApp(surfaceNode, hwcNodes, hasVisibleHwcNodes,
        needForceUpdateHwcNodes);
    EXPECT_TRUE(needForceUpdateHwcNodes);
}

/*
 * @tc.name: HandleTunnelLayerId001
 * @tc.desc: Test HandleTunnelLayerId
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniRenderVisitorTest, HandleTunnelLayerId001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
 
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode, nullptr);
 
    rsUniRenderVisitor->HandleTunnelLayerId(*surfaceNode);
    EXPECT_EQ(surfaceNode->GetTunnelLayerId(), 0);
}

/*
 * @tc.name: UpdateTopLayersDirtyStatusTest
 * @tc.desc: Test UpdateTopLayersDirtyStatus
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateTopLayersDirtyStatusTest, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);

    RSSurfaceRenderNodeConfig config;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    surfaceNode->stagingRenderParams_ = std::make_unique<RSSurfaceRenderParams>(surfaceNode->GetId());
    rsUniRenderVisitor->curScreenNode_ = std::make_shared<RSScreenRenderNode>(1, 0, rsContext);
    std::vector<std::shared_ptr<RSSurfaceRenderNode>> topLayers;
    topLayers.emplace_back(surfaceNode);
    rsUniRenderVisitor->UpdateTopLayersDirtyStatus(topLayers);
    auto surfaceHandler = surfaceNode->GetMutableRSSurfaceHandler();
    ASSERT_NE(surfaceHandler, nullptr);
    EXPECT_EQ(surfaceHandler->GetGlobalZOrder(), 1);
    topLayers.clear();

    auto surfaceNode1 = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    surfaceNode1->SetLayerTop(true);
    surfaceNode1->SetTopLayerZOrder(2);
    surfaceNode1->stagingRenderParams_ = std::make_unique<RSSurfaceRenderParams>(surfaceNode1->GetId());
    topLayers.emplace_back(surfaceNode1);
    rsUniRenderVisitor->UpdateTopLayersDirtyStatus(topLayers);
    auto surfaceHandler1 = surfaceNode->GetMutableRSSurfaceHandler();
    ASSERT_NE(surfaceHandler1, nullptr);
    EXPECT_EQ(surfaceHandler1->GetGlobalZOrder(), 2);
}

/*
 * @tc.name: ResetDisplayDirtyRegion
 * @tc.desc: Test function ResetDisplayDirtyRegion
 * @tc.type: FUNC
 * @tc.require: issueICMB9P
 */
HWTEST_F(RSUniRenderVisitorTest, ResetDisplayDirtyRegion, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->curScreenDirtyManager_ = std::make_shared<RSDirtyRegionManager>();
    NodeId id = 1;
    auto rsContext = std::make_shared<RSContext>();
    rsUniRenderVisitor->curScreenNode_ = std::make_shared<RSScreenRenderNode>(id, 0, rsContext);
    rsUniRenderVisitor->screenManager_ = CreateOrGetScreenManager();
    ASSERT_NE(rsUniRenderVisitor->screenManager_, nullptr);
    rsUniRenderVisitor->ResetDisplayDirtyRegion();
    ASSERT_EQ(rsUniRenderVisitor->curScreenDirtyManager_->dirtyRegion_.left_, 0);
    rsUniRenderVisitor->screenManager_ = nullptr;
    rsUniRenderVisitor->ResetDisplayDirtyRegion();
    ASSERT_EQ(rsUniRenderVisitor->curScreenDirtyManager_->dirtyRegion_.left_, 0);
    rsUniRenderVisitor->curScreenNode_ = nullptr;
    rsUniRenderVisitor->ResetDisplayDirtyRegion();
    ASSERT_EQ(rsUniRenderVisitor->curScreenDirtyManager_->dirtyRegion_.left_, 0);
}

/*
 * @tc.name: HandleTunnelLayerId002
 * @tc.desc: Test HandleTunnelLayerId002
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniRenderVisitorTest, HandleTunnelLayerId002, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
 
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceConfig);
    ASSERT_NE(surfaceNode, nullptr);
 
    surfaceNode->SetTunnelLayerId(1);
    rsUniRenderVisitor->HandleTunnelLayerId(*surfaceNode);
    const auto nodeParams = static_cast<RSSurfaceRenderParams*>(surfaceNode->GetStagingRenderParams().get());
    if (nodeParams != nullptr) {
        EXPECT_EQ(surfaceNode->GetTunnelLayerId(), 1);
    } else {
        EXPECT_EQ(surfaceNode->GetTunnelLayerId(), 0);
    }
}

/*
 * @tc.name: UpdateOffscreenCanvasNodeId001
 * @tc.desc: Test function UpdateOffscreenCanvasNodeId
 * @tc.type: FUNC
 * @tc.require: issueICB4RP
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateOffscreenCanvasNodeId001, TestSize.Level2)
{
    NodeId nodeId = 1;
    auto rsCanvasRenderNode = std::make_shared<RSCanvasRenderNode>(nodeId);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->UpdateOffscreenCanvasNodeId(*rsCanvasRenderNode);
    EXPECT_EQ(rsUniRenderVisitor->offscreenCanvasNodeId_, INVALID_NODEID);

    auto rsChildrenDrawable = std::make_shared<RSChildrenDrawableAdapter>();
    rsCanvasRenderNode->GetDrawableVec(__func__)[static_cast<uint32_t>(RSDrawableSlot::FOREGROUND_FILTER)] =
        std::move(rsChildrenDrawable);
    rsUniRenderVisitor->UpdateOffscreenCanvasNodeId(*rsCanvasRenderNode);
    EXPECT_EQ(rsUniRenderVisitor->offscreenCanvasNodeId_, nodeId);
}

/**
 * @tc.name: UpdateDstRectByGlobalPosition001
 * @tc.desc: Test function UpdateDstRectByGlobalPosition
 * @tc.type: FUNC
 * @tc.require: issueICB4RP
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateDstRectByGlobalPosition001, TestSize.Level2)
{
    NodeId id = 1;
    RSSurfaceRenderNode node(id);
    node.isHwcGlobalPositionEnabled_ = true;
    node.SetDstRect({0, 1000, 2440, 1080});
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto context = std::make_shared<RSContext>();
    constexpr NodeId DEFAULT_ID = 0xFFFF;
    rsUniRenderVisitor->curScreenNode_ = std::make_shared<RSScreenRenderNode>(DEFAULT_ID, 0, context);
    ASSERT_NE(rsUniRenderVisitor->curScreenNode_, nullptr);
    ScreenInfo screenInfo;
    screenInfo.width = 1440;
    screenInfo.height = 1080;
    rsUniRenderVisitor->curScreenNode_->SetScreenInfo(screenInfo);
    rsUniRenderVisitor->hwcVisitor_->UpdateDstRectByGlobalPosition(node);
    RectI expectedDstRect = {0, 1000, 2440, 1080};
    EXPECT_TRUE(node.GetDstRect() == expectedDstRect);
}

/*
 * @tc.name: PostPrepare001
 * @tc.desc: Test PostPrepare NeedDrawBehindWindow branch
 * @tc.type: FUNC
 * @tc.require: issueICMWYR
 */
HWTEST_F(RSUniRenderVisitorTest, PostPrepare001, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->dirtyManager_ = std::make_shared<RSDirtyRegionManager>();
    ASSERT_NE(surfaceNode->dirtyManager_, nullptr);
    auto& geoPtr = surfaceNode->renderProperties_.boundsGeo_;
    geoPtr = std::make_shared<RSObjAbsGeometry>();
    RectI rect(10, 10, 20, 20);
    geoPtr->absRect_ = rect;
    rsUniRenderVisitor->PostPrepare(*surfaceNode);
    EXPECT_TRUE(surfaceNode->GetFilterRect() == rect);

    Drawing::Matrix surfaceMatrix;
    surfaceMatrix.SetScale(0.5f, 0.5f);
    EXPECT_FALSE(surfaceMatrix.IsIdentity());
    surfaceNode->renderProperties_.boundsGeo_->absMatrix_ = surfaceMatrix;
    NodeId childId1 = 1000;
    auto context = RSMainThread::Instance()->GetContext().shared_from_this();
    auto child1 = std::make_shared<RSRenderNode>(childId1, context);
    context->nodeMap.RegisterRenderNode(child1);
    child1->renderProperties_.boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
    child1->selfDrawRect_ = RectF(20.f, 20.f, 30.f, 30.f);
    child1->renderProperties_.boundsGeo_->absMatrix_ = Drawing::Matrix();
    context->nodeMap.GetRenderNode<RSRenderNode>(childId1)->parent_ = surfaceNode;
    surfaceNode->childrenBlurBehindWindow_.emplace(childId1);
    auto modifier = std::make_shared<ModifierNG::RSBehindWindowFilterRenderModifier>();
    surfaceNode->AddModifier(modifier);
    ASSERT_TRUE(surfaceNode->NeedDrawBehindWindow());

    rsUniRenderVisitor->curSurfaceNode_ = nullptr;
    surfaceNode->isFirstLevelCrossNode_ = false;
    rsUniRenderVisitor->PostPrepare(*surfaceNode);
    EXPECT_TRUE(child1->renderProperties_.boundsGeo_->absMatrix_->IsIdentity());
    EXPECT_FALSE(surfaceNode->GetFilterRect() == rect);

    rsUniRenderVisitor->curSurfaceNode_ = nullptr;
    surfaceNode->isFirstLevelCrossNode_ = true;
    rsUniRenderVisitor->PostPrepare(*surfaceNode);
    EXPECT_TRUE(child1->renderProperties_.boundsGeo_->absMatrix_->IsIdentity());
    EXPECT_FALSE(surfaceNode->GetFilterRect() == rect);

    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;
    surfaceNode->isFirstLevelCrossNode_ = true;
    rsUniRenderVisitor->PostPrepare(*surfaceNode);
    EXPECT_TRUE(child1->renderProperties_.boundsGeo_->absMatrix_->IsIdentity());
    EXPECT_FALSE(surfaceNode->GetFilterRect() == rect);

    surfaceNode->isFirstLevelCrossNode_ = false;
    rsUniRenderVisitor->PostPrepare(*surfaceNode);
    auto child = context->nodeMap.GetRenderNode<RSRenderNode>(childId1);
    EXPECT_FALSE(child->renderProperties_.boundsGeo_->absMatrix_->IsIdentity());
    EXPECT_FALSE(surfaceNode->GetFilterRect() == rect);
    context->nodeMap.UnregisterRenderNode(childId1);
}

/*
 * @tc.name: PostPrepare002
 * @tc.desc: Test RSUnitRenderRenderVisitorTest.PostPrepare002
 * @tc.type: FUNC
 * @tc.require: issueIAG8BF
 */
HWTEST_F(RSUniRenderVisitorTest, PostPrepare002, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    auto child = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(child, nullptr);
    auto child2 = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(child2, nullptr);

    surfaceNode->InitRenderParams();
    child->InitRenderParams();
    child->isHardwareForcedDisabled_ = false;
    child2->InitRenderParams();
    child2->isHardwareForcedDisabled_ = true;
    surfaceNode->AddChildHardwareEnabledNode(child);
    surfaceNode->AddChildHardwareEnabledNode(child2);
    surfaceNode->AddChildHardwareEnabledNode(std::weak_ptr<RSSurfaceRenderNode>());
    surfaceNode->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;

    auto dirtyManager = surfaceNode->GetDirtyManager();
    ASSERT_NE(dirtyManager, nullptr);
    rsUniRenderVisitor->curSurfaceDirtyManager_ = dirtyManager;

    rsUniRenderVisitor->PostPrepare(*surfaceNode, false);
    ASSERT_FALSE(child->isHardwareForcedDisabled_);
    
    rsUniRenderVisitor->PostPrepare(*surfaceNode, true);
    ASSERT_TRUE(child->isHardwareForcedDisabled_);
    
    child->isHardwareForcedDisabled_ = false;
    Occlusion::Region visibleRegion(DEFAULT_RECT);
    surfaceNode->SetVisibleRegion(visibleRegion);
    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;
    
    rsUniRenderVisitor->PostPrepare(*surfaceNode, false);
    ASSERT_FALSE(child->isHardwareForcedDisabled_);
    
    rsUniRenderVisitor->PostPrepare(*surfaceNode, true);
    ASSERT_FALSE(child->isHardwareForcedDisabled_);
}

/*
 * @tc.name: UpdateChildBlurBehindWindowAbsMatrix
 * @tc.desc: Test UpdateChildBlurBehindWindowAbsMatrix
 * @tc.type: FUNC
 * @tc.require: issueICMWYR
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateChildBlurBehindWindowAbsMatrix, TestSize.Level1)
{
    NodeId id = 0;
    auto node = std::make_shared<RSRenderNode>(id);
    ASSERT_NE(node, nullptr);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->UpdateChildBlurBehindWindowAbsMatrix(*node);

    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    auto context = RSMainThread::Instance()->GetContext().shared_from_this();

    NodeId childId1 = 1001;
    NodeId childId2 = 1002;
    NodeId childId3 = 1003;
    auto child1 = std::make_shared<RSRenderNode>(childId1, context);
    auto child2 = std::make_shared<RSRenderNode>(childId2, context);
    auto child3 = std::make_shared<RSRenderNode>(childId3);
    surfaceNode->childrenBlurBehindWindow_.emplace(childId1);
    surfaceNode->childrenBlurBehindWindow_.emplace(childId2);
    surfaceNode->childrenBlurBehindWindow_.emplace(childId3);
    ASSERT_FALSE(surfaceNode->GetChildBlurBehindWindow().empty());

    context->nodeMap.RegisterRenderNode(child1);
    ASSERT_TRUE(context->nodeMap.GetRenderNode<RSRenderNode>(childId1) != nullptr);
    context->nodeMap.RegisterRenderNode(child2);
    ASSERT_TRUE(context->nodeMap.GetRenderNode<RSRenderNode>(childId2) != nullptr);

    child1->renderProperties_.boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
    child1->parent_ = surfaceNode;

    Drawing::Matrix surfaceMatrix;
    surfaceMatrix.SetScale(0.5f, 0.5f);
    EXPECT_FALSE(surfaceMatrix.IsIdentity());
    surfaceNode->renderProperties_.boundsGeo_->absMatrix_ = surfaceMatrix;
    Drawing::Matrix childMatrix;
    child1->renderProperties_.boundsGeo_->absMatrix_ = childMatrix;
    EXPECT_TRUE(child1->renderProperties_.boundsGeo_->absMatrix_->IsIdentity());
    child2->renderProperties_.boundsGeo_ = nullptr;

    rsUniRenderVisitor->UpdateChildBlurBehindWindowAbsMatrix(*surfaceNode);
    EXPECT_FALSE(child1->renderProperties_.boundsGeo_->absMatrix_->IsIdentity());
    context->nodeMap.UnregisterRenderNode(childId1);
    context->nodeMap.UnregisterRenderNode(childId2);
}

/*
 * @tc.name: QuickPrepareSufaceRenderNode005
 * @tc.desc: Test function QuickPrepareSufaceRenderNode
 * @tc.type: FUNC
 * @tc.require: issueICB4RP
 */
HWTEST_F(RSUniRenderVisitorTest, QuickPrepareSufaceRenderNode005, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetIsOnTheTree(true, surfaceNode->GetId(), surfaceNode->GetId());
    surfaceNode->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    surfaceNode->name_ = CAPTURE_WINDOW_NAME;

    NodeId id = 0;
    auto rsContext = std::make_shared<RSContext>();
    auto displayNode = std::make_shared<RSScreenRenderNode>(id, 0, rsContext);
    ASSERT_NE(displayNode, nullptr);
    displayNode->AddChild(surfaceNode, 0);

    RSDisplayNodeConfig config;
    auto logicalDisplayNode = std::make_shared<RSLogicalDisplayRenderNode>(id, config);
    logicalDisplayNode->InitRenderParams();

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curScreenNode_ = displayNode;
    rsUniRenderVisitor->curLogicalDisplayNode_ = logicalDisplayNode;
    surfaceNode->subThreadAssignable_ = true;
    rsUniRenderVisitor->QuickPrepareSurfaceRenderNode(*surfaceNode);
    surfaceNode->subThreadAssignable_ = false;
    rsUniRenderVisitor->QuickPrepareSurfaceRenderNode(*surfaceNode);
    RSMainThread::Instance()->focusNodeId_ = 0;
    RSMainThread::Instance()->focusLeashWindowId_ = 0;
    surfaceNode->id_ = 0;
    rsUniRenderVisitor->QuickPrepareSurfaceRenderNode(*surfaceNode);
    surfaceNode->id_ = 1;
    rsUniRenderVisitor->QuickPrepareSurfaceRenderNode(*surfaceNode);
    RSMainThread::Instance()->focusLeashWindowId_ = 1;
    rsUniRenderVisitor->QuickPrepareSurfaceRenderNode(*surfaceNode);
    ASSERT_NE(rsUniRenderVisitor->curScreenNode_, nullptr);
}

/*
 * @tc.name: CollectSurfaceLockLayer001
 * @tc.desc: Test function CollectSurfaceLockLayer
 * @tc.type: FUNC
 * @tc.require: issueICUQ08
 */
HWTEST_F(RSUniRenderVisitorTest, CollectSurfaceLockLayer001, TestSize.Level2)
{
    ASSERT_NE(RSMainThread::Instance(), nullptr);
    RSMainThread::Instance()->hasProtectedLayer_ = false;
    RSMainThread::Instance()->hasSurfaceLockLayer_ = false;
    RSMainThread::Instance()->SetHasSurfaceLockLayer(false);
    EXPECT_FALSE(RSMainThread::Instance()->HasDRMOrSurfaceLockLayer());
    NodeId id = 1;
    RSSurfaceRenderNode node(id);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->CollectSurfaceLockLayer(node);
    EXPECT_FALSE(RSMainThread::Instance()->HasDRMOrSurfaceLockLayer());

    node.childHardwareEnabledNodes_.resize(1);
    rsUniRenderVisitor->CollectSurfaceLockLayer(node);
    EXPECT_FALSE(RSMainThread::Instance()->HasDRMOrSurfaceLockLayer());

    node.childHardwareEnabledNodes_.clear();
    RSSurfaceRenderNodeConfig config;
    auto childNode = std::make_shared<RSSurfaceRenderNode>(config);
    ASSERT_NE(childNode, nullptr);
    childNode->isFixRotationByUser_ = true;
    childNode->SetIsOnTheTree(false);
    node.AddChildHardwareEnabledNode(std::weak_ptr<RSSurfaceRenderNode>(childNode));
    rsUniRenderVisitor->CollectSurfaceLockLayer(node);
    EXPECT_FALSE(RSMainThread::Instance()->HasDRMOrSurfaceLockLayer());
    childNode->SetIsOnTheTree(true);
    rsUniRenderVisitor->CollectSurfaceLockLayer(node);
    EXPECT_TRUE(RSMainThread::Instance()->HasDRMOrSurfaceLockLayer());
}

/*
 * @tc.name: CheckFilterNeedEnableDebug001
 * @tc.desc: Test function CheckFilterNeedEnableDebug
 * @tc.type: FUNC
 * @tc.require: issue20057
 */
HWTEST_F(RSUniRenderVisitorTest, CheckFilterNeedEnableDebug001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    NodeId id = 0;
    RSEffectRenderNode node(id);
    rsUniRenderVisitor->CheckFilterNeedEnableDebug(node, true);
    rsUniRenderVisitor->CheckFilterNeedEnableDebug(node, false);
    rsUniRenderVisitor->curSurfaceNode_ = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(rsUniRenderVisitor->curSurfaceNode_, nullptr);
    rsUniRenderVisitor->CheckFilterNeedEnableDebug(node, false);
    RSSurfaceRenderNodeConfig config;
    config.id = id;
    config.surfaceWindowType = SurfaceWindowType::SCB_DESKTOP;
    rsUniRenderVisitor->curSurfaceNode_ = std::make_shared<RSSurfaceRenderNode>(config);
    rsUniRenderVisitor->CheckFilterNeedEnableDebug(node, false);
}

/*
 * @tc.name: UpdateFixedSize
 * @tc.desc: Test function UpdateFixedSize for display node
 * @tc.type: FUNC
 * @tc.require: issue20599
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateFixedSize, TestSize.Level2)
{
    auto visitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(visitor, nullptr);
    NodeId id = 0;
    RSDisplayNodeConfig displayConfig;
    auto node = std::make_shared<RSLogicalDisplayRenderNode>(id, displayConfig);
    ASSERT_NE(node, nullptr);
    node->InitRenderParams();
    auto& geo = node->GetRenderProperties().GetBoundsGeometry();
    ASSERT_NE(geo, nullptr);

    float degree = 90;
    geo->SetRotation(degree);
    node->UpdateRotation();
    RSMainThread::Instance()->systemAnimatedScenes_ = SystemAnimatedScenes::OTHERS;
    visitor->UpdateFixedSize(*node);
    RSMainThread::Instance()->systemAnimatedScenes_ = SystemAnimatedScenes::SNAPSHOT_ROTATION;
    visitor->UpdateFixedSize(*node);
    degree = 1;
    geo->SetRotation(degree);
    node->UpdateRotation();
    RSMainThread::Instance()->systemAnimatedScenes_ = SystemAnimatedScenes::OTHERS;
    visitor->UpdateFixedSize(*node);
    RSMainThread::Instance()->systemAnimatedScenes_ = SystemAnimatedScenes::SNAPSHOT_ROTATION;
    visitor->UpdateFixedSize(*node);
    EXPECT_EQ(node->GetCompositeType(), CompositeType::HARDWARE_COMPOSITE);
}

/**
 * @tc.name: QuickPrepareUnionRenderNode001
 * @tc.desc: Test QuickPrepareUnionRenderNode, dirtyManager = nullptr
 * @tc.type: FUNC
 */
HWTEST_F(RSUniRenderVisitorTest, QuickPrepareUnionRenderNode001, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    EXPECT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curSurfaceNode_ = RSTestUtil::CreateSurfaceNodeWithBuffer();
    rsUniRenderVisitor->curSurfaceDirtyManager_ = nullptr;
    NodeId id = 1;
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSUnionRenderNode>(id, rsContext->weak_from_this());
    node->InitRenderParams();
    rsUniRenderVisitor->QuickPrepareUnionRenderNode(*node);
    ASSERT_EQ(rsUniRenderVisitor->curSurfaceDirtyManager_, nullptr);
}

/**
 * @tc.name: QuickPrepareUnionRenderNode002
 * @tc.desc: Test QuickPrepareUnionRenderNode, dirtyManager != nullptr
 * @tc.type: FUNC
 */
HWTEST_F(RSUniRenderVisitorTest, QuickPrepareUnionRenderNode002, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    EXPECT_NE(rsUniRenderVisitor, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;
    auto dirtyManager = surfaceNode->GetDirtyManager();
    EXPECT_NE(dirtyManager, nullptr);
    rsUniRenderVisitor->curSurfaceDirtyManager_ = dirtyManager;
    NodeId id = 1;
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSUnionRenderNode>(id, rsContext->weak_from_this());
    node->InitRenderParams();
    rsUniRenderVisitor->QuickPrepareUnionRenderNode(*node);
    ASSERT_NE(rsUniRenderVisitor->curSurfaceDirtyManager_, nullptr);
}

/**
 * @tc.name: DisableHardwareHdrTest001
 * @tc.desc: Test HandlePixelFormat with disable hardware hdr.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniRenderVisitorTest, DisableHardwareHdrTest001, TestSize.Level1)
{
    auto& originalInterface = RSLuminanceControl::Get().rSLuminanceControlInterface_;
    Mock::RSLuminanceControlInterfaceMock mockInterface;
    RSLuminanceControl::Get().rSLuminanceControlInterface_ = &mockInterface;
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    auto rsScreenRenderNode = std::make_shared<RSScreenRenderNode>(11, 0, rsContext->weak_from_this());
    ASSERT_NE(rsScreenRenderNode, nullptr);
    rsScreenRenderNode->dirtyManager_ = std::make_shared<RSDirtyRegionManager>();
    ASSERT_NE(rsScreenRenderNode->dirtyManager_, nullptr);
    auto screenManager = CreateOrGetScreenManager();
    auto screenId = CreateVirtualScreen(screenManager);
    ASSERT_NE(screenId, INVALID_SCREEN_ID);
    rsScreenRenderNode->stagingRenderParams_ = std::make_unique<RSScreenRenderParams>(screenId);
    rsScreenRenderNode->CollectHdrStatus(HdrStatus::HDR_PHOTO);
    mockInterface.isHardwareHdrDisabled_ = true;
    rsScreenRenderNode->GetMutableRenderProperties().SetHDRBrightnessFactor(0.0f);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curScreenNode_ = rsScreenRenderNode;

    std::shared_ptr<RSSurfaceRenderNode> drmNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(drmNode, nullptr);
    drmNode->GetRenderProperties().GetBoundsGeometry()->absRect_ = RectT(0, 0, 1, 1);
    rsUniRenderVisitor->drmNodes_.emplace_back(drmNode);
    rsUniRenderVisitor->QuickPrepareScreenRenderNode(*rsScreenRenderNode);
    ASSERT_EQ(mockInterface.hdrStatus_, HdrStatus::NO_HDR);
    rsScreenRenderNode->CollectHdrStatus(HdrStatus::HDR_PHOTO);
    rsScreenRenderNode->GetMutableRenderProperties().SetHDRBrightnessFactor(0.5f);
    rsUniRenderVisitor->QuickPrepareScreenRenderNode(*rsScreenRenderNode);
    ASSERT_EQ(mockInterface.hdrStatus_, HdrStatus::NO_HDR);
    if (!rsUniRenderVisitor->drmNodes_.empty()) {
        rsUniRenderVisitor->drmNodes_.pop_back();
    }
    rsScreenRenderNode->CollectHdrStatus(HdrStatus::HDR_PHOTO);
    rsUniRenderVisitor->HandlePixelFormat(*rsScreenRenderNode);
    ASSERT_EQ(mockInterface.hdrStatus_, HdrStatus::HDR_PHOTO);
    mockInterface.isHardwareHdrDisabled_ = false;
    rsScreenRenderNode->CollectHdrStatus(HdrStatus::HDR_PHOTO);
    rsUniRenderVisitor->HandlePixelFormat(*rsScreenRenderNode);
    ASSERT_EQ(mockInterface.hdrStatus_, HdrStatus::HDR_PHOTO);
    RSLuminanceControl::Get().rSLuminanceControlInterface_ = originalInterface;
}

/**
 * @tc.name: DisableHardwareHdrTest002
 * @tc.desc: Test HandlePixelFormat with disable hardware hdr.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniRenderVisitorTest, DisableHardwareHdrTest002, TestSize.Level1)
{
    auto& originalInterface = RSLuminanceControl::Get().rSLuminanceControlInterface_;
    Mock::RSLuminanceControlInterfaceMock mockInterface;
    RSLuminanceControl::Get().rSLuminanceControlInterface_ = &mockInterface;
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    auto rsScreenRenderNode = std::make_shared<RSScreenRenderNode>(11, 0, rsContext->weak_from_this());
    ASSERT_NE(rsScreenRenderNode, nullptr);
    rsScreenRenderNode->dirtyManager_ = std::make_shared<RSDirtyRegionManager>();
    ASSERT_NE(rsScreenRenderNode->dirtyManager_, nullptr);
    auto screenManager = CreateOrGetScreenManager();
    auto screenId = CreateVirtualScreen(screenManager);
    ASSERT_NE(screenId, INVALID_SCREEN_ID);
    rsScreenRenderNode->stagingRenderParams_ = std::make_unique<RSScreenRenderParams>(screenId);
    auto param = static_cast<RSScreenRenderParams*>(rsScreenRenderNode->stagingRenderParams_.get());
    rsScreenRenderNode->GetMutableRenderProperties().SetHDRBrightnessFactor(0.5f);
    rsScreenRenderNode->SetHasUniRenderHdrSurface(false);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curScreenNode_ = rsScreenRenderNode;

    mockInterface.isHdrOn_ = true;
    mockInterface.isHardwareHdrDisabled_ = false;
    rsScreenRenderNode->SetHDRPresent(true);
    rsUniRenderVisitor->QuickPrepareScreenRenderNode(*rsScreenRenderNode);
    ASSERT_FALSE(param->GetHDRPresent());
    mockInterface.isHardwareHdrDisabled_ = true;
    rsScreenRenderNode->SetHDRPresent(true);
    rsScreenRenderNode->SetHasUniRenderHdrSurface(false);
    rsUniRenderVisitor->QuickPrepareScreenRenderNode(*rsScreenRenderNode);
    ASSERT_TRUE(param->GetHDRPresent());
    rsScreenRenderNode->SetHDRPresent(true);
    rsScreenRenderNode->SetHasUniRenderHdrSurface(true);
    rsUniRenderVisitor->HandlePixelFormat(*rsScreenRenderNode);
    ASSERT_TRUE(param->GetHDRPresent());
    mockInterface.isHardwareHdrDisabled_ = false;
    rsScreenRenderNode->SetHDRPresent(true);
    rsScreenRenderNode->SetHasUniRenderHdrSurface(true);
    rsUniRenderVisitor->HandlePixelFormat(*rsScreenRenderNode);
    ASSERT_TRUE(param->GetHDRPresent());

    RSLuminanceControl::Get().rSLuminanceControlInterface_ = originalInterface;
}

/**
 * @tc.name: DisableHardwareHdrTest003
 * @tc.desc: Test IsHdrUseUnirender with disable hardware hdr.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniRenderVisitorTest, DisableHardwareHdrTest003, TestSize.Level1)
{
    // create input args.
    auto& originalInterface = RSLuminanceControl::Get().rSLuminanceControlInterface_;
    Mock::RSLuminanceControlInterfaceMock mockInterface;
    RSLuminanceControl::Get().rSLuminanceControlInterface_ = &mockInterface;
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    NodeId childId = 1;
    auto childNode1 = std::make_shared<RSSurfaceRenderNode>(childId);
    childNode1->SetIsOnTheTree(true);
    childNode1->InitRenderParams();
    node->AddChildHardwareEnabledNode(childNode1);
    NodeId displayId = 1;
    auto rsContext = std::make_shared<RSContext>();
    auto displayNode = std::make_shared<RSScreenRenderNode>(displayId, 0, rsContext);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->curScreenNode_ = displayNode;

    childNode1->GetMultableSpecialLayerMgr().Set(SpecialLayerType::PROTECTED, true);
    mockInterface.isHardwareHdrDisabled_ = false;
    mockInterface.isHdrOn_ = true;
    std::vector<std::shared_ptr<RSSurfaceRenderNode>> topLayers;
    rsUniRenderVisitor->UpdateHwcNodeDirtyRegionAndCreateLayer(node, topLayers);
    ASSERT_FALSE(childNode1->isHardwareForcedDisabled_);
    childNode1->GetMultableSpecialLayerMgr().Set(SpecialLayerType::PROTECTED, false);
    rsUniRenderVisitor->UpdateHwcNodeDirtyRegionAndCreateLayer(node, topLayers);
    ASSERT_FALSE(childNode1->isHardwareForcedDisabled_);
    mockInterface.isHardwareHdrDisabled_ = true;
    childNode1->GetMultableSpecialLayerMgr().Set(SpecialLayerType::PROTECTED, true);
    rsUniRenderVisitor->UpdateHwcNodeDirtyRegionAndCreateLayer(node, topLayers);
    ASSERT_FALSE(childNode1->isHardwareForcedDisabled_);
    childNode1->GetMultableSpecialLayerMgr().Set(SpecialLayerType::PROTECTED, false);
    rsUniRenderVisitor->hasFingerprint_ = true;
    rsUniRenderVisitor->UpdateHwcNodeDirtyRegionAndCreateLayer(node, topLayers);
    ASSERT_TRUE(childNode1->isHardwareForcedDisabled_);
    std::shared_ptr<RSSurfaceRenderNode> drmNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(drmNode, nullptr);
    drmNode->GetRenderProperties().GetBoundsGeometry()->absRect_ = RectT(0, 0, 1, 1);
    rsUniRenderVisitor->drmNodes_.emplace_back(drmNode);
    rsUniRenderVisitor->UpdateHwcNodeDirtyRegionAndCreateLayer(node, topLayers);
    ASSERT_TRUE(childNode1->isHardwareForcedDisabled_);
    displayNode->SetHasUniRenderHdrSurface(true);
    ASSERT_TRUE(childNode1->isHardwareForcedDisabled_);
    RSLuminanceControl::Get().rSLuminanceControlInterface_ = originalInterface;
}

/**
 * @tc.name: RenderGroupCacheTypeTest001
 * @tc.desc: Test RenderNodeGroup for CanvasRenderNode with RSRenderNode::NodeGroupType:NONE
 * @tc.type: FUNC
 * @tc.require: issueIASE3Z
 */
HWTEST_F(RSUniRenderVisitorTest, RenderGroupCacheTypeTest001, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    auto rsCanvasRenderNode = std::make_shared<RSCanvasRenderNode>(1, rsContext->weak_from_this());
    ASSERT_NE(rsCanvasRenderNode, nullptr);
    rsCanvasRenderNode->stagingRenderParams_ = std::make_unique<RSRenderParams>(rsCanvasRenderNode->GetId());
    RSRenderNode::NodeGroupType type = RSRenderNode::NodeGroupType::NONE;
    bool isNodeGroup = true;
    bool includeProperty = true;
    rsCanvasRenderNode->MarkNodeGroup(type, isNodeGroup, includeProperty);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->isDrawingCacheEnabled_ = true;
    rsUniRenderVisitor->QuickPrepareCanvasRenderNode(*rsCanvasRenderNode);

    RSDrawingCacheType drawingcacheType = rsCanvasRenderNode->GetDrawingCacheType();
    EXPECT_EQ(drawingcacheType, RSDrawingCacheType::DISABLED_CACHE);
}


/**
 * @tc.name: RenderGroupCacheTypeTest002
 * @tc.desc: Test RenderNodeGroup for CanvasRenderNode with RSRenderNode::NodeGroupType:GROUPED_BY_UI
 * @tc.type: FUNC
 * @tc.require: issueIASE3Z
 */
HWTEST_F(RSUniRenderVisitorTest, RenderGroupCacheTypeTest002, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    auto rsCanvasRenderNode = std::make_shared<RSCanvasRenderNode>(1, rsContext->weak_from_this());
    ASSERT_NE(rsCanvasRenderNode, nullptr);
    rsCanvasRenderNode->stagingRenderParams_ = std::make_unique<RSRenderParams>(rsCanvasRenderNode->GetId());
    RSRenderNode::NodeGroupType type = RSRenderNode::NodeGroupType::GROUPED_BY_UI;
    rsCanvasRenderNode->nodeGroupType_ = type;
    bool isNodeGroup = true;
    bool includeProperty = true;
    rsCanvasRenderNode->MarkNodeGroup(type, isNodeGroup, includeProperty);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->isDrawingCacheEnabled_ = true;
    rsUniRenderVisitor->QuickPrepareCanvasRenderNode(*rsCanvasRenderNode);

    RSDrawingCacheType drawingcacheType = rsCanvasRenderNode->GetDrawingCacheType();
    EXPECT_EQ(drawingcacheType, RSDrawingCacheType::TARGETED_CACHE);
}

/**
 * @tc.name: RenderGroupCacheTypeTest003
 * @tc.desc: Test RenderNodeGroup for CanvasRenderNode with RSRenderNode::NodeGroupType:GROUPED_BY_USER
 * @tc.type: FUNC
 * @tc.require: issueIASE3Z
 */
HWTEST_F(RSUniRenderVisitorTest, RenderGroupCacheTypeTest003, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    auto rsCanvasRenderNode = std::make_shared<RSCanvasRenderNode>(1, rsContext->weak_from_this());
    ASSERT_NE(rsCanvasRenderNode, nullptr);
    rsCanvasRenderNode->stagingRenderParams_ = std::make_unique<RSRenderParams>(rsCanvasRenderNode->GetId());
    RSRenderNode::NodeGroupType type = RSRenderNode::NodeGroupType::GROUPED_BY_USER;
    bool isNodeGroup = true;
    bool includeProperty = true;
    rsCanvasRenderNode->MarkNodeGroup(type, isNodeGroup, includeProperty);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->isDrawingCacheEnabled_ = true;
    rsUniRenderVisitor->QuickPrepareCanvasRenderNode(*rsCanvasRenderNode);

    RSDrawingCacheType drawingcacheType = rsCanvasRenderNode->GetDrawingCacheType();
    EXPECT_EQ(drawingcacheType, RSDrawingCacheType::FORCED_CACHE);
}

/**
 * @tc.name: RenderGroupCacheTypeTest004
 * @tc.desc: Test RenderNodeGroup for CanvasRenderNode with RSRenderNode::NodeGroupType:GROUPED_BY_FOREGROUND_FILTER
 * @tc.type: FUNC
 * @tc.require: issueIASE3Z
 */
HWTEST_F(RSUniRenderVisitorTest, RenderGroupCacheTypeTest004, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    auto rsCanvasRenderNode = std::make_shared<RSCanvasRenderNode>(1, rsContext->weak_from_this());
    ASSERT_NE(rsCanvasRenderNode, nullptr);
    rsCanvasRenderNode->stagingRenderParams_ = std::make_unique<RSRenderParams>(rsCanvasRenderNode->GetId());
    RSRenderNode::NodeGroupType type = RSRenderNode::NodeGroupType::GROUPED_BY_FOREGROUND_FILTER;
    bool isNodeGroup = true;
    bool includeProperty = true;
    rsCanvasRenderNode->MarkNodeGroup(type, isNodeGroup, includeProperty);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->isDrawingCacheEnabled_ = true;
    rsUniRenderVisitor->QuickPrepareCanvasRenderNode(*rsCanvasRenderNode);

    RSDrawingCacheType drawingcacheType = rsCanvasRenderNode->GetDrawingCacheType();
    EXPECT_EQ(drawingcacheType, RSDrawingCacheType::FOREGROUND_FILTER_CACHE);
}

/**
 * @tc.name: RenderGroupCacheTypeTest005
 * @tc.desc: Test RenderNodeGroup for UnionRenderNode with RSRenderNode::NodeGroupType:NONE
 * @tc.type: FUNC
 * @tc.require: issueIASE3Z
 */
HWTEST_F(RSUniRenderVisitorTest, RenderGroupCacheTypeTest005, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    auto rsUnionRenderNode = std::make_shared<RSUnionRenderNode>(1, rsContext->weak_from_this());
    ASSERT_NE(rsUnionRenderNode, nullptr);
    rsUnionRenderNode->stagingRenderParams_ = std::make_unique<RSRenderParams>(rsUnionRenderNode->GetId());
    RSRenderNode::NodeGroupType type = RSRenderNode::NodeGroupType::NONE;
    bool isNodeGroup = true;
    bool includeProperty = true;
    rsUnionRenderNode->MarkNodeGroup(type, isNodeGroup, includeProperty);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->isDrawingCacheEnabled_ = true;
    rsUniRenderVisitor->QuickPrepareUnionRenderNode(*rsUnionRenderNode);

    RSDrawingCacheType drawingcacheType = rsUnionRenderNode->GetDrawingCacheType();
    EXPECT_EQ(drawingcacheType, RSDrawingCacheType::DISABLED_CACHE);
}

/**
 * @tc.name: RenderGroupCacheTypeTest006
 * @tc.desc: Test RenderNodeGroup for UnionRenderNode with RSRenderNode::NodeGroupType:GROUPED_BY_UI
 * @tc.type: FUNC
 * @tc.require: issueIASE3Z
 */
HWTEST_F(RSUniRenderVisitorTest, RenderGroupCacheTypeTest006, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    auto rsUnionRenderNode = std::make_shared<RSUnionRenderNode>(1, rsContext->weak_from_this());
    ASSERT_NE(rsUnionRenderNode, nullptr);
    rsUnionRenderNode->stagingRenderParams_ = std::make_unique<RSRenderParams>(rsUnionRenderNode->GetId());
    RSRenderNode::NodeGroupType type = RSRenderNode::NodeGroupType::GROUPED_BY_UI;
    rsUnionRenderNode->nodeGroupType_ = type;
    bool isNodeGroup = true;
    bool includeProperty = true;
    rsUnionRenderNode->MarkNodeGroup(type, isNodeGroup, includeProperty);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->isDrawingCacheEnabled_ = true;
    rsUniRenderVisitor->QuickPrepareUnionRenderNode(*rsUnionRenderNode);

    RSDrawingCacheType drawingcacheType = rsUnionRenderNode->GetDrawingCacheType();
    EXPECT_EQ(drawingcacheType, RSDrawingCacheType::TARGETED_CACHE);
}

/**
 * @tc.name: RenderGroupCacheTypeTest007
 * @tc.desc: Test RenderNodeGroup for UnionRenderNode with RSRenderNode::NodeGroupType:GROUPED_BY_USER
 * @tc.type: FUNC
 * @tc.require: issueIASE3Z
 */
HWTEST_F(RSUniRenderVisitorTest, RenderGroupCacheTypeTest007, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    auto rsUnionRenderNode = std::make_shared<RSUnionRenderNode>(1, rsContext->weak_from_this());
    ASSERT_NE(rsUnionRenderNode, nullptr);
    rsUnionRenderNode->stagingRenderParams_ = std::make_unique<RSRenderParams>(rsUnionRenderNode->GetId());
    RSRenderNode::NodeGroupType type = RSRenderNode::NodeGroupType::GROUPED_BY_USER;
    bool isNodeGroup = true;
    bool includeProperty = true;
    rsUnionRenderNode->MarkNodeGroup(type, isNodeGroup, includeProperty);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->isDrawingCacheEnabled_ = true;
    rsUniRenderVisitor->QuickPrepareUnionRenderNode(*rsUnionRenderNode);

    RSDrawingCacheType drawingcacheType = rsUnionRenderNode->GetDrawingCacheType();
    EXPECT_EQ(drawingcacheType, RSDrawingCacheType::FORCED_CACHE);
}

/**
 * @tc.name: RenderGroupCacheTypeTest008
 * @tc.desc: Test RenderNodeGroup for UnionRenderNode with RSRenderNode::NodeGroupType:GROUPED_BY_FOREGROUND_FILTER
 * @tc.type: FUNC
 * @tc.require: issueIASE3Z
 */
HWTEST_F(RSUniRenderVisitorTest, RenderGroupCacheTypeTest008, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    auto rsUnionRenderNode = std::make_shared<RSUnionRenderNode>(1, rsContext->weak_from_this());
    ASSERT_NE(rsUnionRenderNode, nullptr);
    rsUnionRenderNode->stagingRenderParams_ = std::make_unique<RSRenderParams>(rsUnionRenderNode->GetId());
    RSRenderNode::NodeGroupType type = RSRenderNode::NodeGroupType::GROUPED_BY_FOREGROUND_FILTER;
    bool isNodeGroup = true;
    bool includeProperty = true;
    rsUnionRenderNode->MarkNodeGroup(type, isNodeGroup, includeProperty);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->isDrawingCacheEnabled_ = true;
    rsUniRenderVisitor->QuickPrepareUnionRenderNode(*rsUnionRenderNode);

    RSDrawingCacheType drawingcacheType = rsUnionRenderNode->GetDrawingCacheType();
    EXPECT_EQ(drawingcacheType, RSDrawingCacheType::FOREGROUND_FILTER_CACHE);
}

/**
 * @tc.name: RenderGroupCacheTypeTest009
 * @tc.desc: Test RenderNodeGroup for CanvasRenderNode with isNodeGroup:false,
 * RSRenderNode::NodeGroupType:GROUPED_BY_USER
 * @tc.type: FUNC
 * @tc.require: issueIASE3Z
 */
HWTEST_F(RSUniRenderVisitorTest, RenderGroupCacheTypeTest009, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    auto rsCanvasRenderNode = std::make_shared<RSCanvasRenderNode>(1, rsContext->weak_from_this());
    ASSERT_NE(rsCanvasRenderNode, nullptr);
    rsCanvasRenderNode->stagingRenderParams_ = std::make_unique<RSRenderParams>(rsCanvasRenderNode->GetId());
    RSRenderNode::NodeGroupType type = RSRenderNode::NodeGroupType::GROUPED_BY_USER;
    bool isNodeGroup = false;
    bool includeProperty = true;
    rsCanvasRenderNode->MarkNodeGroup(type, isNodeGroup, includeProperty);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->isDrawingCacheEnabled_ = true;
    rsUniRenderVisitor->QuickPrepareCanvasRenderNode(*rsCanvasRenderNode);

    RSDrawingCacheType drawingcacheType = rsCanvasRenderNode->GetDrawingCacheType();
    EXPECT_EQ(drawingcacheType, RSDrawingCacheType::DISABLED_CACHE);
}

/**
 * @tc.name: RenderGroupCacheTypeTest010
 * @tc.desc: Test RenderNodeGroup for UnionRenderNode with RSRenderNode::NodeGroupType:GROUPED_BY_USER, with
 * isNodeGroup:false
 * @tc.type: FUNC
 * @tc.require: issueIASE3Z
 */
HWTEST_F(RSUniRenderVisitorTest, RenderGroupCacheTypeTest010, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    auto rsUnionRenderNode = std::make_shared<RSUnionRenderNode>(1, rsContext->weak_from_this());
    ASSERT_NE(rsUnionRenderNode, nullptr);
    rsUnionRenderNode->stagingRenderParams_ = std::make_unique<RSRenderParams>(rsUnionRenderNode->GetId());
    RSRenderNode::NodeGroupType type = RSRenderNode::NodeGroupType::GROUPED_BY_USER;
    bool isNodeGroup = false;
    bool includeProperty = true;
    rsUnionRenderNode->MarkNodeGroup(type, isNodeGroup, includeProperty);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->isDrawingCacheEnabled_ = true;
    rsUniRenderVisitor->QuickPrepareUnionRenderNode(*rsUnionRenderNode);

    RSDrawingCacheType drawingcacheType = rsUnionRenderNode->GetDrawingCacheType();
    EXPECT_EQ(drawingcacheType, RSDrawingCacheType::DISABLED_CACHE);
}

/**
 * @tc.name: RenderGroupCacheTypeTest011
 * @tc.desc: Test RenderNodeGroup for EffectRenderNode with RSRenderNode::NodeGroupType:GROUPED_BY_USER, with
 * isNodeGroup:true
 * @tc.type: FUNC
 * @tc.require: issueIASE3Z
 */
HWTEST_F(RSUniRenderVisitorTest, RenderGroupCacheTypeTest011, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    auto rsEffectRenderNode = std::make_shared<RSEffectRenderNode>(1, rsContext->weak_from_this());
    ASSERT_NE(rsEffectRenderNode, nullptr);
    rsEffectRenderNode->stagingRenderParams_ = std::make_unique<RSRenderParams>(rsEffectRenderNode->GetId());
    RSRenderNode::NodeGroupType type = RSRenderNode::NodeGroupType::GROUPED_BY_USER;
    bool isNodeGroup = true;
    bool includeProperty = true;
    rsEffectRenderNode->MarkNodeGroup(type, isNodeGroup, includeProperty);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->isDrawingCacheEnabled_ = true;
    rsUniRenderVisitor->QuickPrepareEffectRenderNode(*rsEffectRenderNode);

    RSDrawingCacheType drawingcacheType = rsEffectRenderNode->GetDrawingCacheType();
    EXPECT_EQ(drawingcacheType, RSDrawingCacheType::DISABLED_CACHE);
}

/**
 * @tc.name: RenderGroupCacheTypeTest012
 * @tc.desc: Test RenderNodeGroup for WindowKeyFrameRenderNode with RSRenderNode::NodeGroupType:GROUPED_BY_USER, with
 * isNodeGroup:true
 * @tc.type: FUNC
 * @tc.require: issueIASE3Z
 */
HWTEST_F(RSUniRenderVisitorTest, RenderGroupCacheTypeTest012, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    auto rsWindowKeyFrameRenderNode = std::make_shared<RSWindowKeyFrameRenderNode>(1, rsContext->weak_from_this());
    ASSERT_NE(rsWindowKeyFrameRenderNode, nullptr);
    rsWindowKeyFrameRenderNode->stagingRenderParams_ =
        std::make_unique<RSRenderParams>(rsWindowKeyFrameRenderNode->GetId());
    RSRenderNode::NodeGroupType type = RSRenderNode::NodeGroupType::GROUPED_BY_USER;
    bool isNodeGroup = true;
    bool includeProperty = true;
    rsWindowKeyFrameRenderNode->MarkNodeGroup(type, isNodeGroup, includeProperty);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->isDrawingCacheEnabled_ = true;
    rsUniRenderVisitor->QuickPrepareWindowKeyFrameRenderNode(*rsWindowKeyFrameRenderNode);

    RSDrawingCacheType drawingcacheType = rsWindowKeyFrameRenderNode->GetDrawingCacheType();
    EXPECT_EQ(drawingcacheType, RSDrawingCacheType::DISABLED_CACHE);
}

/**
 * @tc.name: RenderGroupCacheTypeTest013
 * @tc.desc: Test RenderNodeGroup with MarkForegroundFilter for CanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueIASE3Z
 */
HWTEST_F(RSUniRenderVisitorTest, RenderGroupCacheTypeTest013, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    auto rsCanvasRenderNode = std::make_shared<RSCanvasRenderNode>(1, rsContext->weak_from_this());
    ASSERT_NE(rsCanvasRenderNode, nullptr);
    rsCanvasRenderNode->InitRenderParams();
    rsCanvasRenderNode->renderProperties_.SetForegroundFilterCache(std::make_shared<RSFilter>());
    rsCanvasRenderNode->MarkForegroundFilterCache();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->isDrawingCacheEnabled_ = true;
    rsUniRenderVisitor->QuickPrepareCanvasRenderNode(*rsCanvasRenderNode);

    RSDrawingCacheType drawingcacheType = rsCanvasRenderNode->GetDrawingCacheType();
    EXPECT_EQ(drawingcacheType, RSDrawingCacheType::FOREGROUND_FILTER_CACHE);
}

/**
 * @tc.name: RenderGroupCacheTypeTest014
 * @tc.desc: Test RenderNodeGroup with MarkForegroundFilter for CanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueIASE3Z
 */
HWTEST_F(RSUniRenderVisitorTest, RenderGroupCacheTypeTest014, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    auto rsCanvasRenderNode = std::make_shared<RSCanvasRenderNode>(1, rsContext->weak_from_this());
    ASSERT_NE(rsCanvasRenderNode, nullptr);
    rsCanvasRenderNode->InitRenderParams();
    rsCanvasRenderNode->nodeGroupType_ = RSRenderNode::NodeGroupType::GROUPED_BY_FOREGROUND_FILTER;
    rsCanvasRenderNode->MarkForegroundFilterCache();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->isDrawingCacheEnabled_ = true;
    rsUniRenderVisitor->QuickPrepareCanvasRenderNode(*rsCanvasRenderNode);

    RSDrawingCacheType drawingcacheType = rsCanvasRenderNode->GetDrawingCacheType();
    EXPECT_EQ(drawingcacheType, RSDrawingCacheType::DISABLED_CACHE);
}

/**
 * @tc.name: RenderGroupCacheTypeTest015
 * @tc.desc: Test RenderNodeGroup with MarkForegroundFilter for UnionRenderNode
 * @tc.type: FUNC
 * @tc.require: issueIASE3Z
 */
HWTEST_F(RSUniRenderVisitorTest, RenderGroupCacheTypeTest015, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    auto rsUnionRenderNode = std::make_shared<RSUnionRenderNode>(1, rsContext->weak_from_this());
    ASSERT_NE(rsUnionRenderNode, nullptr);
    rsUnionRenderNode->InitRenderParams();
    rsUnionRenderNode->renderProperties_.SetForegroundFilterCache(std::make_shared<RSFilter>());
    rsUnionRenderNode->MarkForegroundFilterCache();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->isDrawingCacheEnabled_ = true;
    rsUniRenderVisitor->QuickPrepareUnionRenderNode(*rsUnionRenderNode);

    RSDrawingCacheType drawingcacheType = rsUnionRenderNode->GetDrawingCacheType();
    EXPECT_EQ(drawingcacheType, RSDrawingCacheType::FOREGROUND_FILTER_CACHE);
}

/**
 * @tc.name: RenderGroupCacheTypeTest016
 * @tc.desc: Test RenderNodeGroup with MarkForegroundFilter for UnionRenderNode
 * @tc.type: FUNC
 * @tc.require: issueIASE3Z
 */
HWTEST_F(RSUniRenderVisitorTest, RenderGroupCacheTypeTest016, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    auto rsUnionRenderNode = std::make_shared<RSUnionRenderNode>(1, rsContext->weak_from_this());
    ASSERT_NE(rsUnionRenderNode, nullptr);
    rsUnionRenderNode->InitRenderParams();
    rsUnionRenderNode->nodeGroupType_ = RSRenderNode::NodeGroupType::GROUPED_BY_FOREGROUND_FILTER;
    rsUnionRenderNode->MarkForegroundFilterCache();
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->isDrawingCacheEnabled_ = true;
    rsUniRenderVisitor->QuickPrepareUnionRenderNode(*rsUnionRenderNode);

    RSDrawingCacheType drawingcacheType = rsUnionRenderNode->GetDrawingCacheType();
    EXPECT_EQ(drawingcacheType, RSDrawingCacheType::DISABLED_CACHE);
}


/**
 * @tc.name: DisableOccludedHwcNodeInSkippedSubTreeTest001
 * @tc.desc: Test DisableOccludedHwcNodeInSkippedSubTree with curSurfaceNode nullptr
 * @tc.type: FUNC
 * @tc.require: issue21170
 */
HWTEST_F(RSUniRenderVisitorTest, DisableOccludedHwcNodeInSkippedSubTreeTest001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    RSRenderNode node(0);
    rsUniRenderVisitor->DisableOccludedHwcNodeInSkippedSubTree(node);
}

/**
 * @tc.name: SubSurfaceOpaqueRegionFromAccumulatedDirtyRegion001
 * @tc.desc: IsFirstLevelCrossNode == true
 * @tc.type: FUNC
 * @tc.require: issue21439
 */
HWTEST_F(RSUniRenderVisitorTest, SubSurfaceOpaqueRegionFromAccumulatedDirtyRegion001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    EXPECT_NE(rsUniRenderVisitor, nullptr);
    auto rsContext = std::make_shared<RSContext>();
    auto screenNode = std::make_shared<RSScreenRenderNode>(0, 0, rsContext);
    rsUniRenderVisitor->curScreenNode_ = screenNode;
    rsUniRenderVisitor->curScreenNode_->isFirstVisitCrossNodeDisplay_ = true;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(1, rsContext);
    surfaceNode->renderProperties_.boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
    surfaceNode->dirtyManager_ = std::make_shared<RSDirtyRegionManager>();
    EXPECT_NE(surfaceNode->dirtyManager_, nullptr);
    surfaceNode->isFirstLevelCrossNode_ = true;
    surfaceNode->opaqueRegion_ = Occlusion::Region(RectI(0, 0, 5, 5));

    Occlusion::Region origin = Occlusion::Region(RectI(0, 0, 10, 10));
    Occlusion::Region accumulatedDirtyRegion = origin;
    rsUniRenderVisitor->SubSurfaceOpaqueRegionFromAccumulatedDirtyRegion(*surfaceNode, accumulatedDirtyRegion);
    EXPECT_EQ(accumulatedDirtyRegion.GetRegionInfo(), origin.GetRegionInfo());
}

/**
 * @tc.name: SubSurfaceOpaqueRegionFromAccumulatedDirtyRegion002
 * @tc.desc: IsFirstLevelCrossNode == false, IsMainWindowType == false
 * @tc.type: FUNC
 * @tc.require: issue21439
 */
HWTEST_F(RSUniRenderVisitorTest, SubSurfaceOpaqueRegionFromAccumulatedDirtyRegion002, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    EXPECT_NE(rsUniRenderVisitor, nullptr);
    auto rsContext = std::make_shared<RSContext>();
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(1, rsContext);
    surfaceNode->renderProperties_.boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
    surfaceNode->dirtyManager_ = std::make_shared<RSDirtyRegionManager>();
    EXPECT_NE(surfaceNode->dirtyManager_, nullptr);
    surfaceNode->isFirstLevelCrossNode_ = false;
    surfaceNode->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_NODE;
    surfaceNode->opaqueRegion_ = Occlusion::Region(RectI(0, 0, 5, 5));

    Occlusion::Region origin = Occlusion::Region(RectI(0, 0, 10, 10));
    Occlusion::Region accumulatedDirtyRegion = origin;
    rsUniRenderVisitor->SubSurfaceOpaqueRegionFromAccumulatedDirtyRegion(*surfaceNode, accumulatedDirtyRegion);
    EXPECT_EQ(accumulatedDirtyRegion.GetRegionInfo(), origin.GetRegionInfo());
}

/**
 * @tc.name: SubSurfaceOpaqueRegionFromAccumulatedDirtyRegion003
 * @tc.desc: IsFirstLevelCrossNode == false, IsMainWindowType == true, GetIsParticipateInOcclusion == false
 * @tc.type: FUNC
 * @tc.require: issue21439
 */
HWTEST_F(RSUniRenderVisitorTest, SubSurfaceOpaqueRegionFromAccumulatedDirtyRegion003, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    EXPECT_NE(rsUniRenderVisitor, nullptr);
    auto rsContext = std::make_shared<RSContext>();
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(1, rsContext);
    surfaceNode->renderProperties_.boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
    surfaceNode->dirtyManager_ = std::make_shared<RSDirtyRegionManager>();
    EXPECT_NE(surfaceNode->dirtyManager_, nullptr);
    surfaceNode->isFirstLevelCrossNode_ = false;
    surfaceNode->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    surfaceNode->isParticipateInOcclusion_ = false;
    surfaceNode->opaqueRegion_ = Occlusion::Region(RectI(0, 0, 5, 5));

    Occlusion::Region origin = Occlusion::Region(RectI(0, 0, 10, 10));
    Occlusion::Region accumulatedDirtyRegion = origin;
    rsUniRenderVisitor->SubSurfaceOpaqueRegionFromAccumulatedDirtyRegion(*surfaceNode, accumulatedDirtyRegion);
    EXPECT_EQ(accumulatedDirtyRegion.GetRegionInfo(), origin.GetRegionInfo());
}

/**
 * @tc.name: SubSurfaceOpaqueRegionFromAccumulatedDirtyRegion004
 * @tc.desc: IsFirstLevelCrossNode == false, IsMainWindowType == true, GetIsParticipateInOcclusion == true
 * @tc.type: FUNC
 * @tc.require: issue21439
 */
HWTEST_F(RSUniRenderVisitorTest, SubSurfaceOpaqueRegionFromAccumulatedDirtyRegion004, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    EXPECT_NE(rsUniRenderVisitor, nullptr);
    auto rsContext = std::make_shared<RSContext>();
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(1, rsContext);
    surfaceNode->renderProperties_.boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
    surfaceNode->dirtyManager_ = std::make_shared<RSDirtyRegionManager>();
    EXPECT_NE(surfaceNode->dirtyManager_, nullptr);
    surfaceNode->isFirstLevelCrossNode_ = false;
    surfaceNode->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    surfaceNode->isParticipateInOcclusion_ = true;
    surfaceNode->opaqueRegion_ = Occlusion::Region(RectI(0, 0, 5, 5));

    Occlusion::Region origin = Occlusion::Region(RectI(0, 0, 10, 10));
    Occlusion::Region accumulatedDirtyRegion = origin;
    rsUniRenderVisitor->SubSurfaceOpaqueRegionFromAccumulatedDirtyRegion(*surfaceNode, accumulatedDirtyRegion);
    origin.SubSelf(surfaceNode->opaqueRegion_);
    EXPECT_EQ(accumulatedDirtyRegion.GetRegionInfo(), origin.GetRegionInfo());
}

/**
 * @tc.name: HandleVirtualScreenColorGamut001
 * @tc.desc: Test HandleVirtualScreenColorGamut with virtual screen
 * @tc.type: FUNC
 * @tc.require: issueIAJJ43
 */
HWTEST_F(RSUniRenderVisitorTest, HandleVirtualScreenColorGamut001, TestSize.Level2)
{
    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    ASSERT_NE(screenManager, nullptr);
    auto virtualScreenId = screenManager->CreateVirtualScreen("virtual screen 002", 0, 0, nullptr);
    ASSERT_NE(INVALID_SCREEN_ID, virtualScreenId);

    auto rsContext = std::make_shared<RSContext>();
    auto displayNode = std::make_shared<RSScreenRenderNode>(virtualScreenId, 0, rsContext->weak_from_this());

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    displayNode->SetColorSpace(GRAPHIC_COLOR_GAMUT_ADOBE_RGB);
    rsUniRenderVisitor->HandleVirtualScreenColorGamut(*displayNode);

    ScreenColorGamut screenColorGamut;
    screenManager->GetScreenColorGamut(displayNode->GetScreenId(), screenColorGamut);
    ASSERT_EQ(displayNode->GetColorSpace(), static_cast<GraphicColorGamut>(screenColorGamut));

    screenManager->RemoveVirtualScreen(virtualScreenId);
}

/**
 * @tc.name: HandleVirtualScreenColorGamut002
 * @tc.desc: Test HandleVirtualScreenColorGamut with virtual screen
 * @tc.type: FUNC
 * @tc.require: issueIAJJ43
 */
HWTEST_F(RSUniRenderVisitorTest, HandleVirtualScreenColorGamut002, TestSize.Level2)
{
    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    ASSERT_NE(screenManager, nullptr);
    auto virtualScreenId = screenManager->CreateVirtualScreen("virtual screen 002", 0, 0, nullptr);
    ASSERT_NE(INVALID_SCREEN_ID, virtualScreenId);

    auto rsContext = std::make_shared<RSContext>();
    auto displayNode = std::make_shared<RSScreenRenderNode>(virtualScreenId, 0, rsContext->weak_from_this());

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    displayNode->SetColorSpace(GRAPHIC_COLOR_GAMUT_ADOBE_RGB);

    screenManager->RemoveVirtualScreen(virtualScreenId);
    rsUniRenderVisitor->HandleVirtualScreenColorGamut(*displayNode);

    ScreenColorGamut screenColorGamut;
    screenManager->GetScreenColorGamut(displayNode->GetScreenId(), screenColorGamut);
    ASSERT_EQ(displayNode->GetColorSpace(), static_cast<GraphicColorGamut>(screenColorGamut));
}

/**
 * @tc.name: HandleVirtualScreenColorGamut003
 * @tc.desc: Test HandleVirtualScreenColorGamut with invalid screenId
 * @tc.type: FUNC
 * @tc.require: issueIAJJ43
 */
HWTEST_F(RSUniRenderVisitorTest, HandleVirtualScreenColorGamut003, TestSize.Level2)
{
    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    ASSERT_NE(screenManager, nullptr);
    auto virtualScreenId = screenManager->CreateVirtualScreen("virtual screen 002", 0, 0, nullptr);
    ASSERT_NE(INVALID_SCREEN_ID, virtualScreenId);

    auto rsContext = std::make_shared<RSContext>();
    auto displayNode = std::make_shared<RSScreenRenderNode>(virtualScreenId, 0, rsContext->weak_from_this());

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    displayNode->SetColorSpace(GRAPHIC_COLOR_GAMUT_ADOBE_RGB);
    displayNode->screenId_ = -1;
    rsUniRenderVisitor->HandleVirtualScreenColorGamut(*displayNode);

    ScreenColorGamut screenColorGamut;
    screenManager->GetScreenColorGamut(displayNode->GetScreenId(), screenColorGamut);
    ASSERT_NE(displayNode->GetColorSpace(), static_cast<GraphicColorGamut>(screenColorGamut));
}

/**
 * @tc.name: IsWiredMirrorScreen001
 * @tc.desc: Test IsWiredMirrorScreen returns false when no mirror display
 * @tc.type: FUNC
 * @tc.require: issueIAJJ43
 */
HWTEST_F(RSUniRenderVisitorTest, IsWiredMirrorScreen001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto screenNode = std::make_shared<RSScreenRenderNode>(0, 0);

    bool result = rsUniRenderVisitor->IsWiredMirrorScreen(*screenNode);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: HandleWiredMirrorScreenColorGamut001
 * @tc.desc: Test HandleWiredMirrorScreenColorGamut with P3 disabled
 * @tc.type: FUNC
 * @tc.require: issueIAJJ43
 */
HWTEST_F(RSUniRenderVisitorTest, HandleWiredMirrorScreenColorGamut001, TestSize.Level2)
{
    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    ASSERT_NE(screenManager, nullptr);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto screenNode = std::make_shared<RSScreenRenderNode>(0, 0);
    screenNode->SetColorSpace(GRAPHIC_COLOR_GAMUT_DISPLAY_P3);

    rsUniRenderVisitor->HandleWiredMirrorScreenColorGamut(*screenNode);
    ScreenColorGamut screenColorGamut;
    screenManager->GetScreenColorGamut(screenNode->GetScreenId(), screenColorGamut);
    ASSERT_EQ(screenNode->GetColorSpace(), static_cast<GraphicColorGamut>(screenColorGamut));
}

/**
 * @tc.name: HandleWiredMirrorScreenColorGamut002
 * @tc.desc: Test HandleWiredMirrorScreenColorGamut without mirror source
 * @tc.type: FUNC
 * @tc.require: issueIAJJ43
 */
HWTEST_F(RSUniRenderVisitorTest, HandleWiredMirrorScreenColorGamut002, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto screenNode = std::make_shared<RSScreenRenderNode>(0, 0);
    screenNode->SetColorSpace(GRAPHIC_COLOR_GAMUT_SRGB);

    MultiScreenParam::SetMirrorDisplayCloseP3(false);
    rsUniRenderVisitor->HandleWiredMirrorScreenColorGamut(*screenNode);
    EXPECT_EQ(screenNode->GetColorSpace(), GRAPHIC_COLOR_GAMUT_SRGB);
}

/**
 * @tc.name: HandleWiredMirrorScreenColorGamut003
 * @tc.desc: Test HandleWiredMirrorScreenColorGamut with mirror source
 * @tc.type: FUNC
 * @tc.require: issueIAJJ43
 */
HWTEST_F(RSUniRenderVisitorTest, HandleWiredMirrorScreenColorGamut003, TestSize.Level2)
{
    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    ASSERT_NE(screenManager, nullptr);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsContext = std::make_shared<RSContext>();
    ScreenId screenId = 1;
    auto displayNode = std::make_shared<RSScreenRenderNode>(TestSrc::limitNumber::Uint64[6], screenId, rsContext);
    ASSERT_NE(displayNode, nullptr);
    auto screenNode = std::make_shared<RSScreenRenderNode>(0, 0);
    screenNode->SetColorSpace(GRAPHIC_COLOR_GAMUT_SRGB);
    MultiScreenParam::SetMirrorDisplayCloseP3(false);

    screenNode->mirrorSource_ = displayNode;
    rsUniRenderVisitor->HandleWiredMirrorScreenColorGamut(*screenNode);
    EXPECT_EQ(screenNode->GetColorSpace(), GRAPHIC_COLOR_GAMUT_SRGB);
}

/**
 * @tc.name: HandleWiredMirrorScreenColorGamut004
 * @tc.desc: Test HandleWiredMirrorScreenColorGamut with mirror source
 * @tc.type: FUNC
 * @tc.require: issueIAJJ43
 */
HWTEST_F(RSUniRenderVisitorTest, HandleWiredMirrorScreenColorGamut004, TestSize.Level2)
{
    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    ASSERT_NE(screenManager, nullptr);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsContext = std::make_shared<RSContext>();
    ScreenId screenId = 1;
    auto displayNode = std::make_shared<RSScreenRenderNode>(TestSrc::limitNumber::Uint64[6], screenId, rsContext);
    ASSERT_NE(displayNode, nullptr);
    auto screenNode = std::make_shared<RSScreenRenderNode>(0, 0);
    screenNode->SetColorSpace(GRAPHIC_COLOR_GAMUT_SRGB);
    MultiScreenParam::SetMirrorDisplayCloseP3(false);

    screenNode->mirrorSource_ = displayNode;
    screenNode->screenId_ = -1;
    rsUniRenderVisitor->HandleWiredMirrorScreenColorGamut(*screenNode);
    EXPECT_EQ(screenNode->GetColorSpace(), GRAPHIC_COLOR_GAMUT_SRGB);
}

/**
 * @tc.name: HandleWiredMirrorScreenColorGamut005
 * @tc.desc: Test HandleWiredMirrorScreenColorGamut with mirror source
 * @tc.type: FUNC
 * @tc.require: issueIAJJ43
 */
HWTEST_F(RSUniRenderVisitorTest, HandleWiredMirrorScreenColorGamut005, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsContext = std::make_shared<RSContext>();
    ScreenId screenId = 1;
    auto displayNode = std::make_shared<RSScreenRenderNode>(TestSrc::limitNumber::Uint64[6], screenId, rsContext);
    ASSERT_NE(displayNode, nullptr);
    auto screenNode = std::make_shared<RSScreenRenderNode>(0, 0);
    screenNode->SetColorSpace(GRAPHIC_COLOR_GAMUT_SRGB);

    MultiScreenParam::SetMirrorDisplayCloseP3(true);
    screenNode->mirrorSource_ = displayNode;
    screenNode->screenId_ = -1;
    rsUniRenderVisitor->HandleWiredMirrorScreenColorGamut(*screenNode);
    EXPECT_EQ(screenNode->GetColorSpace(), GRAPHIC_COLOR_GAMUT_SRGB);
}

/**
 * @tc.name: HandleWiredMirrorScreenColorGamut006
 * @tc.desc: Test HandleWiredMirrorScreenColorGamut with invalid screen id
 * @tc.type: FUNC
 * @tc.require: issueIAJJ43
 */
HWTEST_F(RSUniRenderVisitorTest, HandleWiredMirrorScreenColorGamut006, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsContext = std::make_shared<RSContext>();
    ScreenId screenId = 1;
    auto displayNode = std::make_shared<RSScreenRenderNode>(TestSrc::limitNumber::Uint64[6], screenId, rsContext);
    ASSERT_NE(displayNode, nullptr);
    auto screenNode = std::make_shared<RSScreenRenderNode>(0, 0);
    screenNode->SetMirrorSource(displayNode);
    screenNode->SetColorSpace(GRAPHIC_COLOR_GAMUT_SRGB);
    screenNode->screenId_ = -1;

    rsUniRenderVisitor->HandleWiredMirrorScreenColorGamut(*screenNode);
    EXPECT_EQ(screenNode->GetColorSpace(), GRAPHIC_COLOR_GAMUT_SRGB);
}

/**
 * @tc.name: IsWiredExtendedScreen001
 * @tc.desc: Test IsWiredExtendedScreen with external connection
 * @tc.type: FUNC
 * @tc.require: issueIAJJ43
 */
HWTEST_F(RSUniRenderVisitorTest, IsWiredExtendedScreen001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto screenNode = std::make_shared<RSScreenRenderNode>(0, 0);

    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    ASSERT_NE(screenManager, nullptr);

    ScreenId screenId = 1;
    auto rsScreen = std::make_shared<RSScreen>(HdiOutput::CreateHdiOutput(screenId));
    rsScreen->property_.SetConnectionType(DISPLAY_CONNECTION_TYPE_EXTERNAL);
    rsUniRenderVisitor->screenManager_->MockHdiScreenConnected(rsScreen);
    screenNode->screenId_ = screenId;

    bool result = rsUniRenderVisitor->IsWiredExtendedScreen(*screenNode);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: IsWiredExtendedScreen002
 * @tc.desc: Test IsWiredExtendedScreen with internal connection
 * @tc.type: FUNC
 * @tc.require: issueIAJJ43
 */
HWTEST_F(RSUniRenderVisitorTest, IsWiredExtendedScreen002, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto screenNode = std::make_shared<RSScreenRenderNode>(0, 0);

    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    ASSERT_NE(screenManager, nullptr);

    ScreenId screenId = 1;
    auto rsScreen = std::make_shared<RSScreen>(HdiOutput::CreateHdiOutput(screenId));
    rsScreen->property_.SetConnectionType(ScreenConnectionType::DISPLAY_CONNECTION_TYPE_INTERNAL);
    rsUniRenderVisitor->screenManager_ = CreateOrGetScreenManager();
    rsUniRenderVisitor->screenManager_->MockHdiScreenConnected(rsScreen);
    screenNode->screenId_ = screenId;

    bool result = rsUniRenderVisitor->IsWiredExtendedScreen(*screenNode);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: HandleWiredExtendedScreenColorGamut001
 * @tc.desc: Test HandleWiredExtendedScreenColorGamut with main screen
 * @tc.type: FUNC
 * @tc.require: issueIAJJ43
 */
HWTEST_F(RSUniRenderVisitorTest, HandleWiredExtendedScreenColorGamut001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto screenNode = std::make_shared<RSScreenRenderNode>(0, 0);
    screenNode->SetColorSpace(GRAPHIC_COLOR_GAMUT_DISPLAY_P3);

    ScreenId screenId = 1;
    auto rsScreen = std::make_shared<RSScreen>(HdiOutput::CreateHdiOutput(screenId));
    rsScreen->property_.SetConnectionType(ScreenConnectionType::DISPLAY_CONNECTION_TYPE_EXTERNAL);
    rsUniRenderVisitor->screenManager_ = CreateOrGetScreenManager();
    rsUniRenderVisitor->screenManager_->MockHdiScreenConnected(rsScreen);
    screenNode->screenId_ = screenId;

    GraphicColorGamut originalColorSpace = screenNode->GetColorSpace();
    rsUniRenderVisitor->HandleWiredExtendedScreenColorGamut(*screenNode);
    EXPECT_EQ(screenNode->GetColorSpace(), originalColorSpace);
}

/**
 * @tc.name: HandleWiredExtendedScreenColorGamut002
 * @tc.desc: Test HandleWiredExtendedScreenColorGamut with DisableP3OnWiredExtendedScreen = true
 * @tc.type: FUNC
 * @tc.require: issueIAJJ43
 */
HWTEST_F(RSUniRenderVisitorTest, HandleWiredExtendedScreenColorGamut002, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto screenNode = std::make_shared<RSScreenRenderNode>(0, 0);
    screenNode->SetColorSpace(GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
    ColorGamutParam::SetDisableP3OnWiredExtendedScreen(true);
    ScreenId screenId = 1;
    auto rsScreen = std::make_shared<RSScreen>(HdiOutput::CreateHdiOutput(screenId));
    rsScreen->property_.SetConnectionType(ScreenConnectionType::DISPLAY_CONNECTION_TYPE_EXTERNAL);
    rsUniRenderVisitor->screenManager_ = CreateOrGetScreenManager();
    rsUniRenderVisitor->screenManager_->MockHdiScreenConnected(rsScreen);
    screenNode->screenId_ = screenId;

    GraphicColorGamut originalColorSpace = screenNode->GetColorSpace();
    rsUniRenderVisitor->HandleWiredExtendedScreenColorGamut(*screenNode);
    EXPECT_EQ(screenNode->GetColorSpace(), originalColorSpace);
}

/**
 * @tc.name: HandleWiredExtendedScreenColorGamut003
 * @tc.desc: Test HandleWiredMirrorScreenColorGamut with invalid screen id
 * @tc.type: FUNC
 * @tc.require: issueIAJJ43
 */
HWTEST_F(RSUniRenderVisitorTest, HandleWiredExtendedScreenColorGamut003, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto rsContext = std::make_shared<RSContext>();
    ScreenId screenId = 1;
    auto displayNode = std::make_shared<RSScreenRenderNode>(TestSrc::limitNumber::Uint64[6], screenId, rsContext);
    ASSERT_NE(displayNode, nullptr);
    auto screenNode = std::make_shared<RSScreenRenderNode>(0, 0);
    screenNode->SetMirrorSource(displayNode);
    screenNode->SetColorSpace(GRAPHIC_COLOR_GAMUT_SRGB);

    rsUniRenderVisitor->HandleWiredExtendedScreenColorGamut(*screenNode);
    EXPECT_EQ(screenNode->GetColorSpace(), GRAPHIC_COLOR_GAMUT_SRGB);
}

/**
 * @tc.name: HandleWiredExtendedScreenColorGamut004
 * @tc.desc: Test HandleWiredExtendedScreenColorGamut with SRGB
 * @tc.type: FUNC
 * @tc.require: issueIAJJ43
 */
HWTEST_F(RSUniRenderVisitorTest, HandleWiredExtendedScreenColorGamut004, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto screenNode = std::make_shared<RSScreenRenderNode>(0, 0);
    screenNode->SetColorSpace(GRAPHIC_COLOR_GAMUT_SRGB);

    ScreenId screenId = 1;
    auto rsScreen = std::make_shared<RSScreen>(HdiOutput::CreateHdiOutput(screenId));
    rsScreen->property_.SetConnectionType(ScreenConnectionType::DISPLAY_CONNECTION_TYPE_EXTERNAL);
    rsUniRenderVisitor->screenManager_ = CreateOrGetScreenManager();
    rsUniRenderVisitor->screenManager_->MockHdiScreenConnected(rsScreen);
    screenNode->screenId_ = screenId;

    GraphicColorGamut originalColorSpace = screenNode->GetColorSpace();
    rsUniRenderVisitor->HandleWiredExtendedScreenColorGamut(*screenNode);
    EXPECT_EQ(screenNode->GetColorSpace(), originalColorSpace);
}

/**
 * @tc.name: HandleMainScreenColorGamut001
 * @tc.desc: Test HandleMainScreenColorGamut with internal screen
 * @tc.type: FUNC
 * @tc.require: issueIAJJ43
 */
HWTEST_F(RSUniRenderVisitorTest, HandleMainScreenColorGamut001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    auto screenNode = std::make_shared<RSScreenRenderNode>(0, 0);
    ScreenId screenId = 1;
    auto rsScreen = std::make_shared<RSScreen>(HdiOutput::CreateHdiOutput(screenId));
    rsScreen->property_.SetConnectionType(ScreenConnectionType::DISPLAY_CONNECTION_TYPE_INTERNAL);
    rsUniRenderVisitor->screenManager_ = CreateOrGetScreenManager();
    rsUniRenderVisitor->screenManager_->MockHdiScreenConnected(rsScreen);
    screenNode->screenId_ = screenId;
    screenNode->SetColorSpace(GRAPHIC_COLOR_GAMUT_DISPLAY_P3);

    rsUniRenderVisitor->HandleMainScreenColorGamut(*screenNode);
    EXPECT_NE(screenNode->GetColorSpace(), GRAPHIC_COLOR_GAMUT_ADOBE_RGB);
}

/**
 * @tc.name: HandleMainScreenColorGamut002
 * @tc.desc: Test HandleMainScreenColorGamut with external screen
 * @tc.type: FUNC
 */
HWTEST_F(RSUniRenderVisitorTest, HandleMainScreenColorGamut002, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto screenNode = std::make_shared<RSScreenRenderNode>(0, 0);

    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    ASSERT_NE(screenManager, nullptr);

    ScreenId screenId = 1;
    auto rsScreen = std::make_shared<RSScreen>(HdiOutput::CreateHdiOutput(screenId));
    rsScreen->property_.SetConnectionType(ScreenConnectionType::DISPLAY_CONNECTION_TYPE_EXTERNAL);
    rsUniRenderVisitor->screenManager_ = CreateOrGetScreenManager();
    rsUniRenderVisitor->screenManager_->MockHdiScreenConnected(rsScreen);
    screenNode->screenId_ = screenId;

    screenNode->SetColorSpace(GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
    rsUniRenderVisitor->HandleMainScreenColorGamut(*screenNode);
    EXPECT_NE(screenNode->GetColorSpace(), GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
}

/**
 * @tc.name: PrepareColorPickerDrawable001
 * @tc.desc: Test PrepareColorPickerDrawable with ColorPicker drawable
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniRenderVisitorTest, PrepareColorPickerDrawable001, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    // Set up absRect so GetBoundsGeometry()->GetAbsRect() returns valid rect
    RectI testRect(0, 0, 100, 100);
    surfaceNode->GetRenderProperties().GetBoundsGeometry()->absRect_ = testRect;

    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;
    rsUniRenderVisitor->hwcVisitor_->colorPickerHwcDisabledSurfaces_.clear();

    auto colorPickerDrawable = std::make_shared<DrawableV2::RSColorPickerDrawable>();
    // Set up stagingColorPicker_ with valid strategy and interval so Prepare() will set needColorPick to true
    colorPickerDrawable->stagingColorPicker_ =
        std::make_shared<ColorPickerParam>(ColorPlaceholder::FOREGROUND, ColorPickStrategyType::DOMINANT, 0);
    // Set lastUpdateTime_ so interval will be considered elapsed
    colorPickerDrawable->lastUpdateTime_ = 0;

    surfaceNode->GetDrawableVec(__func__)[static_cast<int8_t>(RSDrawableSlot::COLOR_PICKER)] = colorPickerDrawable;
    rsUniRenderVisitor->PrepareColorPickerDrawable(*surfaceNode);

    EXPECT_EQ(rsUniRenderVisitor->hwcVisitor_->colorPickerHwcDisabledSurfaces_.size(), 1u);
}

/**
 * @tc.name: PrepareColorPickerDrawable002
 * @tc.desc: Test PrepareColorPickerDrawable with needColorPick=false
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniRenderVisitorTest, PrepareColorPickerDrawable002, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;
    rsUniRenderVisitor->hwcVisitor_->colorPickerHwcDisabledSurfaces_.clear();

    auto colorPickerDrawable = std::make_shared<DrawableV2::RSColorPickerDrawable>();
    // Set up stagingColorPicker_ with NONE strategy so Prepare() will not set needColorPick
    colorPickerDrawable->stagingColorPicker_ =
        std::make_shared<ColorPickerParam>(ColorPlaceholder::NONE, ColorPickStrategyType::NONE, 0);

    surfaceNode->GetDrawableVec(__func__)[static_cast<int8_t>(RSDrawableSlot::COLOR_PICKER)] = colorPickerDrawable;
    rsUniRenderVisitor->PrepareColorPickerDrawable(*surfaceNode);

    EXPECT_TRUE(rsUniRenderVisitor->hwcVisitor_->colorPickerHwcDisabledSurfaces_.empty());
}

/**
 * @tc.name: PrepareColorPickerDrawable003
 * @tc.desc: Test PrepareColorPickerDrawable with null colorPickerDrawable
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniRenderVisitorTest, PrepareColorPickerDrawable003, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;

    // Clear the map before testing
    rsUniRenderVisitor->hwcVisitor_->colorPickerHwcDisabledSurfaces_.clear();

    // Call PrepareColorPickerDrawable with node that has no ColorPicker drawable
    rsUniRenderVisitor->PrepareColorPickerDrawable(*surfaceNode);

    // Should not crash and map should remain empty
    EXPECT_TRUE(rsUniRenderVisitor->hwcVisitor_->colorPickerHwcDisabledSurfaces_.empty());
}

/**
 * @tc.name: PrepareColorPickerDrawable004
 * @tc.desc: Test PrepareColorPickerDrawable with multiple surfaces
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniRenderVisitorTest, PrepareColorPickerDrawable004, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    auto surfaceNode1 = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto surfaceNode2 = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode1, nullptr);
    ASSERT_NE(surfaceNode2, nullptr);

    // Set up absRect for surface nodes so GetBoundsGeometry()->GetAbsRect() returns valid rect
    RectI testRect(0, 0, 100, 100);
    surfaceNode1->GetRenderProperties().GetBoundsGeometry()->absRect_ = testRect;
    surfaceNode2->GetRenderProperties().GetBoundsGeometry()->absRect_ = testRect;

    rsUniRenderVisitor->hwcVisitor_->colorPickerHwcDisabledSurfaces_.clear();

    auto colorPickerDrawable1 = std::make_shared<DrawableV2::RSColorPickerDrawable>();
    // Set up stagingColorPicker_ with valid strategy and interval so Prepare() will set needColorPick to true
    colorPickerDrawable1->stagingColorPicker_ =
        std::make_shared<ColorPickerParam>(ColorPlaceholder::FOREGROUND, ColorPickStrategyType::DOMINANT, 0);
    // Set lastUpdateTime_ so interval will be considered elapsed
    colorPickerDrawable1->lastUpdateTime_ = 0;

    surfaceNode1->GetDrawableVec(__func__)[static_cast<int8_t>(RSDrawableSlot::COLOR_PICKER)] = colorPickerDrawable1;
    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode1;
    rsUniRenderVisitor->PrepareColorPickerDrawable(*surfaceNode1);
    EXPECT_EQ(rsUniRenderVisitor->hwcVisitor_->colorPickerHwcDisabledSurfaces_.size(), 1u);

    auto colorPickerDrawable2 = std::make_shared<DrawableV2::RSColorPickerDrawable>();
    // Set up stagingColorPicker_ with valid strategy and interval so Prepare() will set needColorPick to true
    colorPickerDrawable2->stagingColorPicker_ =
        std::make_shared<ColorPickerParam>(ColorPlaceholder::FOREGROUND, ColorPickStrategyType::DOMINANT, 0);
    // Set lastUpdateTime_ so interval will be considered elapsed
    colorPickerDrawable2->lastUpdateTime_ = 0;

    surfaceNode2->GetDrawableVec(__func__)[static_cast<int8_t>(RSDrawableSlot::COLOR_PICKER)] = colorPickerDrawable2;
    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode2;
    rsUniRenderVisitor->PrepareColorPickerDrawable(*surfaceNode2);
    EXPECT_EQ(rsUniRenderVisitor->hwcVisitor_->colorPickerHwcDisabledSurfaces_.size(), 2u);

    EXPECT_NE(rsUniRenderVisitor->hwcVisitor_->colorPickerHwcDisabledSurfaces_.find(surfaceNode1->GetId()),
        rsUniRenderVisitor->hwcVisitor_->colorPickerHwcDisabledSurfaces_.end());
    EXPECT_NE(rsUniRenderVisitor->hwcVisitor_->colorPickerHwcDisabledSurfaces_.find(surfaceNode2->GetId()),
        rsUniRenderVisitor->hwcVisitor_->colorPickerHwcDisabledSurfaces_.end());
}

/**
 * @tc.name: CollectFilterInCrossDisplayWindow_ColorPickerOnlyNode001
 * @tc.desc: Test CollectFilterInCrossDisplayWindow skips color picker only nodes
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniRenderVisitorTest, CollectFilterInCrossDisplayWindow_ColorPickerOnlyNode001, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    auto rsContext = std::make_shared<RSContext>();
    NodeId screenId = 0;
    rsUniRenderVisitor->curScreenNode_ = std::make_shared<RSScreenRenderNode>(1, screenId, rsContext);
    ASSERT_NE(rsUniRenderVisitor->curScreenNode_, nullptr);

    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    // Create filter node with only ColorPickerDrawable (no real filter)
    NodeId filterNodeId = 100;
    auto filterNode = std::make_shared<RSRenderNode>(filterNodeId);
    ASSERT_NE(filterNode, nullptr);
    filterNode->GetRenderProperties().GetBoundsGeometry()->absRect_ = { 0, 0, 500, 500 };

    // Set up ColorPickerDrawable
    auto colorPickerDrawable = std::make_shared<DrawableV2::RSColorPickerDrawable>(false, 0);
    filterNode->GetDrawableVec(__func__)[static_cast<int8_t>(RSDrawableSlot::COLOR_PICKER)] = colorPickerDrawable;

    auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();
    nodeMap.RegisterRenderNode(filterNode);
    surfaceNode->visibleFilterChild_.push_back(filterNodeId);

    Occlusion::Region accumDirtyRegion;
    rsUniRenderVisitor->CollectFilterInCrossDisplayWindow(surfaceNode, accumDirtyRegion);

    // Color picker only node should be skipped, no accumulation expected
}

/**
 * @tc.name: UpdateFilterRegionInSkippedSurfaceNode_ColorPickerOnlyNode001
 * @tc.desc: Test UpdateFilterRegionInSkippedSurfaceNode skips color picker only nodes
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateFilterRegionInSkippedSurfaceNode_ColorPickerOnlyNode001, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    auto rsRootRenderNode = std::make_shared<RSSurfaceRenderNode>(0, rsContext->weak_from_this());
    ASSERT_NE(rsRootRenderNode, nullptr);
    rsRootRenderNode->InitRenderParams();

    // Create filter node with only ColorPickerDrawable
    auto filterNode = std::make_shared<RSCanvasRenderNode>(1);
    ASSERT_NE(filterNode, nullptr);
    filterNode->SetOldDirtyInSurface(RectI(100, 100, 500, 500));

    // Set up ColorPickerDrawable
    auto colorPickerDrawable = std::make_shared<DrawableV2::RSColorPickerDrawable>(false, 0);
    filterNode->GetDrawableVec(__func__)[static_cast<int8_t>(RSDrawableSlot::COLOR_PICKER)] = colorPickerDrawable;

    rsRootRenderNode->UpdateVisibleFilterChild(*filterNode);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    RSMainThread::Instance()->GetContext().GetMutableNodeMap().RegisterRenderNode(filterNode);

    RSDirtyRegionManager dirtyManager;
    rsUniRenderVisitor->UpdateFilterRegionInSkippedSurfaceNode(*rsRootRenderNode, dirtyManager);

    // Color picker only node should be skipped without errors
}

/**
 * @tc.name: CheckFilterNodeInSkippedSubTreeNeedClearCache_ColorPickerOnlyNode001
 * @tc.desc: Test CheckFilterNodeInSkippedSubTreeNeedClearCache skips color picker only nodes
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniRenderVisitorTest, CheckFilterNodeInSkippedSubTreeNeedClearCache_ColorPickerOnlyNode001, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    auto rsRootRenderNode = std::make_shared<RSRootRenderNode>(0, rsContext->weak_from_this());
    ASSERT_NE(rsRootRenderNode, nullptr);
    rsRootRenderNode->InitRenderParams();

    // Create filter node with only ColorPickerDrawable
    auto filterNode = std::make_shared<RSEffectRenderNode>(1);
    ASSERT_NE(filterNode, nullptr);
    filterNode->SetOldDirtyInSurface(RectI(100, 100, 500, 500));

    // Set up ColorPickerDrawable
    auto colorPickerDrawable = std::make_shared<DrawableV2::RSColorPickerDrawable>(false, 0);
    filterNode->GetDrawableVec(__func__)[static_cast<int8_t>(RSDrawableSlot::COLOR_PICKER)] = colorPickerDrawable;

    rsRootRenderNode->UpdateVisibleFilterChild(*filterNode);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    RSMainThread::Instance()->GetContext().GetMutableNodeMap().RegisterRenderNode(filterNode);

    RSDirtyRegionManager dirtyManager;
    rsUniRenderVisitor->CheckFilterNodeInSkippedSubTreeNeedClearCache(*rsRootRenderNode, dirtyManager);

    // Color picker only node should be skipped without errors
}

/**
 * @tc.name: CheckFilterNodeInSkippedSubTreeNeedClearCache_ColorPickerOnlyNode002
 * @tc.desc: Test CheckFilterNodeInSkippedSubTreeNeedClearCache skips color picker only nodes in effect loop
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniRenderVisitorTest, CheckFilterNodeInSkippedSubTreeNeedClearCache_ColorPickerOnlyNode002, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    auto rsRootRenderNode = std::make_shared<RSRootRenderNode>(0, rsContext->weak_from_this());
    ASSERT_NE(rsRootRenderNode, nullptr);
    rsRootRenderNode->InitRenderParams();

    // Create filter node with only ColorPickerDrawable
    auto filterNode = std::make_shared<RSEffectRenderNode>(1);
    ASSERT_NE(filterNode, nullptr);
    filterNode->SetOldDirtyInSurface(RectI(100, 100, 500, 500));

    // Set up ColorPickerDrawable
    auto colorPickerDrawable = std::make_shared<DrawableV2::RSColorPickerDrawable>(false, 0);
    filterNode->GetDrawableVec(__func__)[static_cast<int8_t>(RSDrawableSlot::COLOR_PICKER)] = colorPickerDrawable;

    // Add to visibleEffectChild
    filterNode->visibleEffectChild_.insert(0);

    rsRootRenderNode->UpdateVisibleFilterChild(*filterNode);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    RSMainThread::Instance()->GetContext().GetMutableNodeMap().RegisterRenderNode(filterNode);

    RSDirtyRegionManager dirtyManager;
    rsUniRenderVisitor->CheckFilterNodeInSkippedSubTreeNeedClearCache(*rsRootRenderNode, dirtyManager);

    // Color picker only node should be skipped without errors
}

/**
 * @tc.name: CheckFilterNodeInOccludedSkippedSubTreeNeedClearCache_ColorPickerOnlyNode001
 * @tc.desc: Test CheckFilterNodeInOccludedSkippedSubTreeNeedClearCache skips color picker only nodes
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniRenderVisitorTest, CheckFilterNodeInOccludedSkippedSubTreeNeedClearCache_ColorPickerOnlyNode001,
    TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    auto rsRootRenderNode = std::make_shared<RSRootRenderNode>(0, rsContext->weak_from_this());
    ASSERT_NE(rsRootRenderNode, nullptr);
    rsRootRenderNode->InitRenderParams();

    // Create filter node with only ColorPickerDrawable
    auto filterNode = std::make_shared<RSEffectRenderNode>(1);
    ASSERT_NE(filterNode, nullptr);
    filterNode->SetOldDirtyInSurface(RectI(100, 100, 500, 500));

    // Set up ColorPickerDrawable
    auto colorPickerDrawable = std::make_shared<DrawableV2::RSColorPickerDrawable>(false, 0);
    filterNode->GetDrawableVec(__func__)[static_cast<int8_t>(RSDrawableSlot::COLOR_PICKER)] = colorPickerDrawable;

    rsRootRenderNode->UpdateVisibleFilterChild(*filterNode);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    RSMainThread::Instance()->GetContext().GetMutableNodeMap().RegisterRenderNode(filterNode);

    RSDirtyRegionManager dirtyManager;
    rsUniRenderVisitor->CheckFilterNodeInOccludedSkippedSubTreeNeedClearCache(*rsRootRenderNode, dirtyManager);

    // Color picker only node should be skipped without errors
}
} // namespace OHOS::Rosen
#endif // RS_ENABLE_UNI_RENDER