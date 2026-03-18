/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "consumer_surface.h"
#include "feature/occlusion_culling/rs_occlusion_handler.h"
#include "feature/opinc/rs_opinc_manager.h"
#include "feature/round_corner_display/rs_round_corner_display.h"
#include "feature/round_corner_display/rs_round_corner_display_manager.h"
#include "feature/uifirst/rs_uifirst_manager.h"
#include "feature/window_keyframe/rs_window_keyframe_render_node.h"
#include "feature_cfg/graphic_feature_param_manager.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "limit_number.h"
#include "monitor/self_drawing_node_monitor.h"
#include "surface_buffer_impl.h"
#include "system/rs_system_parameters.h"

#include "common/rs_common_hook.h"
#include "draw/color.h"
#include "drawable/rs_color_picker_drawable.h"
#include "drawable/rs_screen_render_node_drawable.h"
#include "drawable/rs_surface_render_node_drawable.h"
#include "modifier_ng/appearance/rs_behind_window_filter_render_modifier.h"
#include "pipeline/hardware_thread/rs_realtime_refresh_rate_manager.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/main_thread/rs_uni_render_visitor.h"
#include "pipeline/mock/mock_matrix.h"
#include "pipeline/mock/mock_rs_luminance_control.h"
#include "pipeline/render_thread/rs_uni_render_engine.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "pipeline/render_thread/rs_uni_render_util.h"
#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_effect_render_node.h"
#include "pipeline/rs_processor_factory.h"
#include "pipeline/rs_proxy_render_node.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_render_thread.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_screen_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_test_util.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "pipeline/rs_union_render_node.h"
#include "property/rs_point_light_manager.h"
#include "screen_manager/rs_screen.h"

using namespace testing;
using namespace testing::ext;

namespace {
constexpr uint32_t DEFAULT_CANVAS_WIDTH = 810;
constexpr uint32_t DEFAULT_CANVAS_HEIGHT = 610;
const OHOS::Rosen::RectI DEFAULT_RECT = { 0, 81, 1001, 1001 };
const OHOS::Rosen::RectI DEFAULT_FILTER_RECT = { 0, 0, 501, 501 };
constexpr int MAX_ALPHA = 255;
constexpr int SCREEN_WIDTH = 3121;
constexpr int SCREEN_HEIGHT = 1081;
constexpr OHOS::Rosen::NodeId DEFAULT_NODE_ID = 101;
const OHOS::Rosen::RectI DEFAULT_SCREEN_RECT = { 0, 0, 1001, 1001 };
} // namespace

namespace OHOS::Rosen {
class RenderChildrenDrawableAdapter : public RSDrawable {
public:
    RenderChildrenDrawableAdapter() = default;
    ~RenderChildrenDrawableAdapter() override = default;
    bool OnUpdate(const RSRenderNode& content) override
    {
        return true;
    }
    void OnSync() override {}
    void OnDraw(Drawing::Canvas* canvas, const Drawing::Rect* rect) const override {}

private:
    bool OnSharedTransition(const std::shared_ptr<RSRenderNode>& node)
    {
        return true;
    }
    friend class RSRenderNode;
};

class UniRenderVisitorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline Mock::MatrixMock* matrixMock_;
    ScreenId CreateVirtualScreen(sptr<RSScreenManager> screenManager);
    static void InitTestSurfaceNodeAndScreenInfo(
        std::shared_ptr<RSSurfaceRenderNode>& surfaceNode, std::shared_ptr<RSUniRenderVisitor>& rSUniRenderVisitor);
    std::shared_ptr<RSUniRenderVisitor> InitRSUniRenderVisitor();
};

class MockSurfaceRenderNode : public RSSurfaceRenderNode {
public:
    explicit MockSurfaceRenderNode(
        NodeId id, const std::weak_ptr<RSContext>& context = {}, bool isTextureExportNode = false)
        : RSSurfaceRenderNode(id, context, isTextureExportNode)
    {}
    ~MockSurfaceRenderNode() override {}
    MOCK_METHOD1(CheckParticipateInOcclusion, bool(bool));
    MOCK_CONST_METHOD0(NeedDrawBehindWindow, bool());
    MOCK_CONST_METHOD0(GetFilterRect, RectI());
    MOCK_METHOD0(CheckIfOcclusionChanged, bool());
};

void UniRenderVisitorTest::SetUpTestCase()
{
    matrixMock_ = Mock::MatrixMock::GetInstance();
    EXPECT_CALL(*matrixMock_, GetMinMaxScales(_)).WillOnce(testing::Return(false));
    RSTestUtil::InitRenderNodeGC();
}
void UniRenderVisitorTest::TearDownTestCase() {}
void UniRenderVisitorTest::SetUp()
{
    if (RSUniRenderJudgement::IsUniRender()) {
        auto& uniRenderThread = RSUniRenderThread::Instance();
        uniRenderThread.uniRenderEngine_ = std::make_shared<RSUniRenderEngine>();
    }
}
void UniRenderVisitorTest::TearDown()
{
    system::SetParameter("rosen.dirtyregiondebug.enabled", "0");
    system::SetParameter("rosen.uni.partialrender.enabled", "4");
    system::GetParameter("rosen.dirtyregiondebug.surfacenames", "0");
}

ScreenId UniRenderVisitorTest::CreateVirtualScreen(sptr<RSScreenManager> screenManager)
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
    std::vector<NodeId> whiteList = { 1 };

    auto screenId = screenManager->CreateVirtualScreen(name, width, height, psurface, INVALID_SCREEN_ID, -1, whiteList);
    if (screenId == INVALID_SCREEN_ID) {
        RS_LOGE("create virtual screen failed");
        return INVALID_SCREEN_ID;
    }
    return screenId;
}

void UniRenderVisitorTest::InitTestSurfaceNodeAndScreenInfo(
    std::shared_ptr<RSSurfaceRenderNode>& surfaceNode, std::shared_ptr<RSUniRenderVisitor>& rsUniRenderVisitor)
{
    surfaceNode->SetNodeHasBackgroundColorAlpha(true);
    surfaceNode->SetHardwareEnableHint(true);
    surfaceNode->SetDstRect({ 10, 1, 100, 100 });
    surfaceNode->SetIsOntheTreeOnlyFlag(true);
    Occlusion::Region region1({ 100, 50, 1000, 1500 });
    surfaceNode->SetVisibleRegion(region1);

    NodeId screenNodeId = 10;
    rsUniRenderVisitor->curScreenNode_ = std::make_shared<RSScreenRenderNode>(screenNodeId, 0);

    ScreenInfo screenInfo;
    screenInfo.width = SCREEN_WIDTH;
    screenInfo.height = SCREEN_HEIGHT;
    rsUniRenderVisitor->curScreenNode_->screenInfo_ = screenInfo;
}

std::shared_ptr<RSUniRenderVisitor> UniRenderVisitorTest::InitRSUniRenderVisitor()
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
 * @tc.name: ProcessFilterNodeObscured002
 * @tc.desc: Test ProcessFilterNodeObscured, childNode not use effect
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(UniRenderVisitorTest, ProcessFilterNodeObscured002, TestSize.Level1)
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
 * @tc.name: ProcessFilterNodeObscured
 * @tc.desc: Test ProcessFilterNodeObscured
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(UniRenderVisitorTest, ProcessFilterNodeObscured, TestSize.Level1)
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
    Occlusion::Region region { Occlusion::Rect { 0, 0, 100, 100 } };
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
 * @tc.name: AfterUpdateSurfaceDirtyCalc_001
 * @tc.desc: AfterUpdateSurfaceDirtyCalc Test，property.GetBoundsGeometry() not null
 * node.IsHardwareEnabledType() && node.GetZorderChanged() && curSurfaceNode_ is true
 * node is not LeashOrMainWindow and not MainWindowType
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(UniRenderVisitorTest, AfterUpdateSurfaceDirtyCalc_001, TestSize.Level2)
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
 * @tc.name: UpdateHwcNodeInfoForAppNode_001
 * @tc.desc: UpdateHwcNodeInfoForAppNode Test, node.GetNeedCollectHwcNode() is true
 * node.IsHardwareEnabledType() is false
 * curSurfaceNode_ is nullptr
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(UniRenderVisitorTest, UpdateHwcNodeInfoForAppNode_001, TestSize.Level2)
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
 * @tc.name: AfterUpdateSurfaceDirtyCalc_002
 * @tc.desc: AfterUpdateSurfaceDirtyCalc Test，property.GetBoundsGeometry() not null
 * node.IsHardwareEnabledType() && node.GetZorderChanged() && curSurfaceNode_ is true
 * node is LeashOrMainWindow and is not MainWindowType
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(UniRenderVisitorTest, AfterUpdateSurfaceDirtyCalc_002, TestSize.Level2)
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
 * @tc.name: UpdateHwcNodeInfoForAppNode_002
 * @tc.desc: UpdateHwcNodeInfoForAppNode Test, node.GetNeedCollectHwcNode() is true
 * node.IsHardwareEnabledType() is true
 * curSurfaceNode_ is nullptr
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(UniRenderVisitorTest, UpdateHwcNodeInfoForAppNode_002, TestSize.Level2)
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

/*
 * @tc.name: UpdateSurfaceRenderNodeScaleTest
 * @tc.desc: Test UniRenderVisitorTest.UpdateSurfaceRenderNodeScaleTest
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(UniRenderVisitorTest, UpdateSurfaceRenderNodeScaleTest, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    RSSurfaceRenderNodeConfig config;
    config.id = 11; // leash window config id
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

/**
 * @tc.name: UpdateHwcNodeInfoForAppNode_003
 * @tc.desc: UpdateHwcNodeInfoForAppNode Test, node.GetNeedCollectHwcNode() is true
 * node.IsHardwareEnabledType() is false
 * curSurfaceNode_ is not nullptr
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(UniRenderVisitorTest, UpdateHwcNodeInfoForAppNode_003, TestSize.Level2)
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
 * @tc.name: CheckLuminanceStatusChangeTest
 * @tc.desc: Test UniRenderVisitorTest.CheckLuminanceStatusChangeTest
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(UniRenderVisitorTest, CheckLuminanceStatusChangeTest, TestSize.Level2)
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
 * @tc.name: PrepareChildren001
 * @tc.desc: PrepareChildren Test
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(UniRenderVisitorTest, PrepareChildren001, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    auto rsBaseRenderNode = std::make_shared<RSBaseRenderNode>(10, rsContext->weak_from_this());
    rsBaseRenderNode->InitRenderParams();
    ASSERT_NE(rsBaseRenderNode->stagingRenderParams_, nullptr);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->PrepareChildren(*rsBaseRenderNode);
}

/*
 * @tc.name: PrepareCanvasRenderNodeTest
 * @tc.desc: Test UniRenderVisitorTest.PrepareCanvasRenderNodeTest
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(UniRenderVisitorTest, PrepareCanvasRenderNodeTest, TestSize.Level2)
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
 * @tc.name: RSScreenRenderNode001
 * @tc.desc: Test RSScreenRenderNode
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(UniRenderVisitorTest, RSScreenRenderNode001, TestSize.Level1)
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
 * @tc.name: PrepareProxyRenderNode001
 * @tc.desc: Test PrepareProxyRenderNode
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(UniRenderVisitorTest, PrepareProxyRenderNode001, TestSize.Level1)
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

/*
 * @tc.name: CheckQuickSkipPrepareParamSetAndGetValid001
 * @tc.desc: Check if param set and get apis are valid.
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(UniRenderVisitorTest, CheckQuickSkipPrepareParamSetAndGetValid001, TestSize.Level1)
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
 * @tc.name: CheckSurfaceRenderNodeStatic001
 * @tc.desc: Generate static surface render node(app window node) and execute preparation step.
 *           Get trace and check corresponding node's preparation and 'Skip' info exist.
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(UniRenderVisitorTest, CheckSurfaceRenderNodeStatic001, TestSize.Level1)
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

/*
 * @tc.name: CheckSurfaceRenderNodeNotStatic001
 * @tc.desc: Generate not static surface render node(self drawing, leash window) and execute preparation step.
 *           Get trace and check corresponding node's preparation exists and no 'Skip' info.
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(UniRenderVisitorTest, CheckSurfaceRenderNodeNotStatic001, TestSize.Level1)
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
 * @tc.name: CalcDirtyDisplayRegion
 * @tc.desc: Set surface to transparent, add a canvas node to create a transparent dirty region
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(UniRenderVisitorTest, CalcDirtyDisplayRegion, TestSize.Level1)
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

    Occlusion::Rect rect { 0, 80, 2560, 1600 };
    Occlusion::Region region { rect };
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
 * @tc.name: PrepareRootRenderNode001
 * @tc.desc: Test PrepareRootRenderNode
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(UniRenderVisitorTest, PrepareRootRenderNode001, TestSize.Level1)
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
 * @tc.name: CalcDirtyRegionForFilterNode
 * @tc.desc: Create a filter effect to test filter node
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(UniRenderVisitorTest, CalcDirtyRegionForFilterNode, TestSize.Level1)
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
 * @tc.name: CollectTopOcclusionSurfacesInfo002
 * @tc.desc: test CollectTopOcclusionSurfacesInfo
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(UniRenderVisitorTest, CollectTopOcclusionSurfacesInfo002, TestSize.Level2)
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
 * @tc.name: CollectTopOcclusionSurfacesInfo001
 * @tc.desc: test CollectTopOcclusionSurfacesInfo
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(UniRenderVisitorTest, CollectTopOcclusionSurfacesInfo001, TestSize.Level2)
{
    RSSurfaceRenderNodeConfig config;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config);
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->CollectTopOcclusionSurfacesInfo(*rsSurfaceRenderNode, false);
}

/**
 * @tc.name: CollectTopOcclusionSurfacesInfo003
 * @tc.desc: test CollectTopOcclusionSurfacesInfo
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(UniRenderVisitorTest, CollectTopOcclusionSurfacesInfo003, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    NodeId id = 0;
    RSDisplayNodeConfig displayNodeConfig;
    rsUniRenderVisitor->curLogicalDisplayNode_ = std::make_shared<RSLogicalDisplayRenderNode>(id, displayNodeConfig);
    rsUniRenderVisitor->isStencilPixelOcclusionCullingEnabled_ = true;
    rsUniRenderVisitor->occlusionSurfaceOrder_ = TOP_OCCLUSION_SURFACES_NUM;
    constexpr int defaultRegionSize { 100 };
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
    FilterDirtyRegionInfo filterInfo = { .intersectRegion_ =
                                             Occlusion::Rect { 0, 0, defaultRegionSize, defaultRegionSize },
        .filterDirty_ = Occlusion::Rect { 0, 0, defaultRegionSize, defaultRegionSize } };
    surfaceNode2->GetDirtyManager()->GetFilterCollector().CollectFilterDirtyRegionInfo(filterInfo, false);

    rsUniRenderVisitor->CollectTopOcclusionSurfacesInfo(*surfaceNode2, false);
    EXPECT_EQ(parentSurfaceNode2->stencilVal_, (TOP_OCCLUSION_SURFACES_NUM - 1) * OCCLUSION_ENABLE_SCENE_NUM + 1);
    EXPECT_EQ(rsUniRenderVisitor->occlusionSurfaceOrder_, DEFAULT_OCCLUSION_SURFACE_ORDER);
}

/*
 * @tc.name: GetSurfaceTransparentFilterRegion
 * @tc.desc: Test UniRenderVisitorTest.GetSurfaceTransparentFilterRegion
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(UniRenderVisitorTest, GetSurfaceTransparentFilterRegion, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->dirtyManager_ = std::make_shared<RSDirtyRegionManager>();
    ASSERT_NE(surfaceNode->dirtyManager_, nullptr);

    // default, surface node is transparent.
    Occlusion::Region filterRegion = Occlusion::Region(Occlusion::Rect(DEFAULT_RECT));
    FilterDirtyRegionInfo filterInfo = { .intersectRegion_ = filterRegion, .filterDirty_ = filterRegion };
    surfaceNode->dirtyManager_->GetFilterCollector().CollectFilterDirtyRegionInfo(filterInfo, false);
    ASSERT_FALSE(rsUniRenderVisitor->GetSurfaceTransparentFilterRegion(*surfaceNode).IsEmpty());
    // if non-transparent, get empty region.
    surfaceNode->abilityBgAlpha_ = 255;
    surfaceNode->globalAlpha_ = 1.f;
    ASSERT_TRUE(rsUniRenderVisitor->GetSurfaceTransparentFilterRegion(*surfaceNode).IsEmpty());
}

/**
 * @tc.name: CollectTopOcclusionSurfacesInfo004
 * @tc.desc: test CollectTopOcclusionSurfacesInfo with first level cross node
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(UniRenderVisitorTest, CollectTopOcclusionSurfacesInfo004, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    NodeId id = 0;
    rsUniRenderVisitor->isStencilPixelOcclusionCullingEnabled_ = true;
    rsUniRenderVisitor->occlusionSurfaceOrder_ = TOP_OCCLUSION_SURFACES_NUM;
    constexpr int defaultRegionSize { 100 };
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
 * @tc.name: CheckMergeFilterDirtyWithPreDirty_001
 * @tc.desc: Test UniRenderVisitorTest.CheckMergeFilterDirtyWithPreDirty, test nullptr side case.
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(UniRenderVisitorTest, CheckMergeFilterDirtyWithPreDirty_001, TestSize.Level2)
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
 * @tc.name: CheckMergeFilterDirtyWithPreDirty_003
 * @tc.desc: Test difference between opauqe/transparent surface node when intersect with accumulated dirty.
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(UniRenderVisitorTest, CheckMergeFilterDirtyWithPreDirty_003, TestSize.Level2)
{
    ASSERT_NE(RSMainThread::Instance(), nullptr);
    auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    auto dirtyManager = std::make_shared<RSDirtyRegionManager>();
    // mock different filter, case1: no below dirty. case2: below dirty.
    NodeId id = 1;
    nodeMap.UnregisterRenderNode(id);
    auto filterNode1 = std::make_shared<RSRenderNode>(id);
    filterNode1->GetMutableRenderProperties().backgroundFilter_ = std::make_shared<RSFilter>();
    filterNode1->GetMutableRenderProperties().boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
    filterNode1->GetMutableRenderProperties().boundsGeo_->absRect_ = DEFAULT_FILTER_RECT;

    // register filter node
    nodeMap.RegisterRenderNode(filterNode1);
    FilterDirtyRegionInfo filterInfo1 = { .id_ = filterNode1->GetId(),
        .filterDirty_ = Occlusion::Rect(DEFAULT_FILTER_RECT) };

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

/*
 * @tc.name: CheckMergeFilterDirtyWithPreDirty_002
 * @tc.desc: Test UniRenderVisitorTest.CheckMergeFilterDirtyWithPreDirty, test if intersect.
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(UniRenderVisitorTest, CheckMergeFilterDirtyWithPreDirty_002, TestSize.Level2)
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

/**
 * @tc.name: InitializeOcclusionHandler001
 * @tc.desc: test InitializeOcclusionHandler with the switch is enabled or not enabled
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(UniRenderVisitorTest, InitializeOcclusionHandler001, TestSize.Level2)
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
    EXPECT_EQ(rsUniRenderVisitor->curOcclusionHandler_, surfaceNode->GetOcclusionParams()->GetOcclusionHandler());
}

/**
 * @tc.name: UpdateDrawingCacheInfoAfterChildren_CloseInnerGroup001
 * @tc.desc: Test closing inner render group - condition: [true, true, true]
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(UniRenderVisitorTest, UpdateDrawingCacheInfoAfterChildren_CloseInnerGroup001, TestSize.Level2)
{
    auto visitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(visitor, nullptr);

    auto ancestorNode = std::make_shared<RSCanvasRenderNode>(1);
    ASSERT_NE(ancestorNode, nullptr);
    ancestorNode->InitRenderParams();
    ancestorNode->SetDrawingCacheType(RSDrawingCacheType::FORCED_CACHE);

    auto testNode = std::make_shared<RSCanvasRenderNode>(2);
    ASSERT_NE(testNode, nullptr);
    testNode->InitRenderParams();
    testNode->SetDrawingCacheType(RSDrawingCacheType::FORCED_CACHE);
    testNode->SetChildHasVisibleFilter(true);

    visitor->isDrawingCacheEnabled_ = true;
    visitor->AddRenderGroupCacheRoot(*ancestorNode);
    visitor->UpdateDrawingCacheInfoAfterChildren(*testNode);

    EXPECT_EQ(testNode->GetDrawingCacheType(), RSDrawingCacheType::DISABLED_CACHE);
}

/**
 * @tc.name: CollectSubTreeAndProcessOcclusion001
 * @tc.desc: test CollectSubTreeAndProcessOcclusion
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(UniRenderVisitorTest, CollectSubTreeAndProcessOcclusion001, TestSize.Level2)
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

/**
 * @tc.name: UpdateDrawingCacheInfoAfterChildren_NoCloseNoFilter002
 * @tc.desc: Test not closing inner render group - condition: [true, true, false]
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(UniRenderVisitorTest, UpdateDrawingCacheInfoAfterChildren_NoCloseNoFilter002, TestSize.Level2)
{
    auto visitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(visitor, nullptr);

    auto ancestorNode = std::make_shared<RSCanvasRenderNode>(1);
    ASSERT_NE(ancestorNode, nullptr);
    ancestorNode->InitRenderParams();
    ancestorNode->SetDrawingCacheType(RSDrawingCacheType::FORCED_CACHE);

    auto testNode = std::make_shared<RSCanvasRenderNode>(2);
    ASSERT_NE(testNode, nullptr);
    testNode->InitRenderParams();
    testNode->SetDrawingCacheType(RSDrawingCacheType::FORCED_CACHE);
    testNode->SetChildHasVisibleFilter(false);
    testNode->SetChildHasVisibleEffect(false);

    visitor->isDrawingCacheEnabled_ = true;
    visitor->AddRenderGroupCacheRoot(*ancestorNode);
    visitor->UpdateDrawingCacheInfoAfterChildren(*testNode);

    EXPECT_NE(testNode->GetDrawingCacheType(), RSDrawingCacheType::DISABLED_CACHE);
}

/**
 * @tc.name: UpdateDrawingCacheInfoAfterChildren_NoCloseNoAncestorNoFilter004
 * @tc.desc: Test not closing inner render group - condition: [true, false, false]
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(UniRenderVisitorTest, UpdateDrawingCacheInfoAfterChildren_NoCloseNoAncestorNoFilter004, TestSize.Level2)
{
    auto visitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(visitor, nullptr);

    auto testNode = std::make_shared<RSCanvasRenderNode>(1);
    ASSERT_NE(testNode, nullptr);
    testNode->InitRenderParams();
    testNode->SetDrawingCacheType(RSDrawingCacheType::FORCED_CACHE);
    testNode->SetChildHasVisibleFilter(false);
    testNode->SetChildHasVisibleEffect(false);

    visitor->isDrawingCacheEnabled_ = true;
    visitor->UpdateDrawingCacheInfoAfterChildren(*testNode);

    EXPECT_NE(testNode->GetDrawingCacheType(), RSDrawingCacheType::DISABLED_CACHE);
}

/**
 * @tc.name: UpdateDrawingCacheInfoAfterChildren_NoCloseNoAncestor003
 * @tc.desc: Test not closing inner render group - condition: [true, false, true]
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(UniRenderVisitorTest, UpdateDrawingCacheInfoAfterChildren_NoCloseNoAncestor003, TestSize.Level2)
{
    auto visitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(visitor, nullptr);

    auto testNode = std::make_shared<RSCanvasRenderNode>(1);
    ASSERT_NE(testNode, nullptr);
    testNode->InitRenderParams();
    testNode->SetDrawingCacheType(RSDrawingCacheType::FORCED_CACHE);
    testNode->SetChildHasVisibleFilter(true);

    visitor->isDrawingCacheEnabled_ = true;
    visitor->UpdateDrawingCacheInfoAfterChildren(*testNode);

    EXPECT_NE(testNode->GetDrawingCacheType(), RSDrawingCacheType::DISABLED_CACHE);
}

/**
 * @tc.name: UpdateDrawingCacheInfoAfterChildren_NoCloseAlreadyDisabled005
 * @tc.desc: Test not closing inner render group - condition: [false, true, true]
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(UniRenderVisitorTest, UpdateDrawingCacheInfoAfterChildren_NoCloseAlreadyDisabled005, TestSize.Level2)
{
    auto visitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(visitor, nullptr);

    auto ancestorNode = std::make_shared<RSCanvasRenderNode>(1);
    ASSERT_NE(ancestorNode, nullptr);
    ancestorNode->InitRenderParams();
    ancestorNode->SetDrawingCacheType(RSDrawingCacheType::FORCED_CACHE);

    auto testNode = std::make_shared<RSCanvasRenderNode>(2);
    ASSERT_NE(testNode, nullptr);
    testNode->InitRenderParams();
    testNode->SetDrawingCacheType(RSDrawingCacheType::DISABLED_CACHE);
    testNode->SetChildHasVisibleFilter(true);

    visitor->isDrawingCacheEnabled_ = true;
    visitor->AddRenderGroupCacheRoot(*ancestorNode);
    visitor->UpdateDrawingCacheInfoAfterChildren(*testNode);

    EXPECT_EQ(testNode->GetDrawingCacheType(), RSDrawingCacheType::DISABLED_CACHE);
}

/**
 * @tc.name: UpdateDrawingCacheInfoAfterChildren_NoCloseDisabledNoAncestor007
 * @tc.desc: Test not closing inner render group - condition: [false, false, true]
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(UniRenderVisitorTest, UpdateDrawingCacheInfoAfterChildren_NoCloseDisabledNoAncestor007, TestSize.Level2)
{
    auto visitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(visitor, nullptr);

    auto testNode = std::make_shared<RSCanvasRenderNode>(1);
    ASSERT_NE(testNode, nullptr);
    testNode->InitRenderParams();
    testNode->SetDrawingCacheType(RSDrawingCacheType::DISABLED_CACHE);
    testNode->SetChildHasVisibleFilter(true);

    visitor->isDrawingCacheEnabled_ = true;
    visitor->UpdateDrawingCacheInfoAfterChildren(*testNode);

    EXPECT_EQ(testNode->GetDrawingCacheType(), RSDrawingCacheType::DISABLED_CACHE);
}

/**
 * @tc.name: UpdateDrawingCacheInfoAfterChildren_NoCloseDisabledNoFilter006
 * @tc.desc: Test not closing inner render group - condition: [false, true, false]
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(UniRenderVisitorTest, UpdateDrawingCacheInfoAfterChildren_NoCloseDisabledNoFilter006, TestSize.Level2)
{
    auto visitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(visitor, nullptr);

    auto ancestorNode = std::make_shared<RSCanvasRenderNode>(1);
    ASSERT_NE(ancestorNode, nullptr);
    ancestorNode->InitRenderParams();
    ancestorNode->SetDrawingCacheType(RSDrawingCacheType::FORCED_CACHE);

    auto testNode = std::make_shared<RSCanvasRenderNode>(2);
    ASSERT_NE(testNode, nullptr);
    testNode->InitRenderParams();
    testNode->SetDrawingCacheType(RSDrawingCacheType::DISABLED_CACHE);
    testNode->SetChildHasVisibleFilter(false);
    testNode->SetChildHasVisibleEffect(false);

    visitor->isDrawingCacheEnabled_ = true;
    visitor->AddRenderGroupCacheRoot(*ancestorNode);
    visitor->UpdateDrawingCacheInfoAfterChildren(*testNode);

    EXPECT_EQ(testNode->GetDrawingCacheType(), RSDrawingCacheType::DISABLED_CACHE);
}

/**
 * @tc.name: UpdateDrawingCacheInfoAfterChildren_NoCloseAllFalse008
 * @tc.desc: Test not closing inner render group - condition: [false, false, false]
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(UniRenderVisitorTest, UpdateDrawingCacheInfoAfterChildren_NoCloseAllFalse008, TestSize.Level2)
{
    auto visitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(visitor, nullptr);

    auto testNode = std::make_shared<RSCanvasRenderNode>(1);
    ASSERT_NE(testNode, nullptr);
    testNode->InitRenderParams();
    testNode->SetDrawingCacheType(RSDrawingCacheType::DISABLED_CACHE);
    testNode->SetChildHasVisibleFilter(false);
    testNode->SetChildHasVisibleEffect(false);

    visitor->isDrawingCacheEnabled_ = true;
    visitor->UpdateDrawingCacheInfoAfterChildren(*testNode);

    EXPECT_EQ(testNode->GetDrawingCacheType(), RSDrawingCacheType::DISABLED_CACHE);
}

/*
 * @tc.name: IsWatermarkFlagChanged
 * @tc.desc: Test UniRenderVisitorTest.IsWatermarkFlagChanged test
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(UniRenderVisitorTest, IsWatermarkFlagChanged, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->IsWatermarkFlagChanged();
}

/*
 * @tc.name: SetSurfafaceGlobalDirtyRegion
 * @tc.desc: Add two surfacenode child to test global dirty region
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(UniRenderVisitorTest, SetSurfafaceGlobalDirtyRegion, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    config.id = 10;
    auto rsSurfaceRenderNode1 = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    rsSurfaceRenderNode1->InitRenderParams();
    Occlusion::Rect rect { 0, 80, 2560, 1600 };
    Occlusion::Region region { rect };
    VisibleData vData;
    rsSurfaceRenderNode1->SetVisibleRegionRecursive(region, vData);

    config.id = 11;
    auto rsSurfaceRenderNode2 = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    rsSurfaceRenderNode2->InitRenderParams();
    Occlusion::Rect rect2 { 100, 100, 500, 1500 };
    Occlusion::Region region2 { rect2 };
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
} // namespace OHOS::Rosen