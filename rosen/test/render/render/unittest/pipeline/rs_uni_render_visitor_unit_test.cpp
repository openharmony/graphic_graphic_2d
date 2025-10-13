/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include "feature_cfg/graphic_feature_param_manager.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "limit_number.h"
#include "monitor/self_drawing_node_monitor.h"
#include "system/rs_system_parameters.h"

#include "common/rs_common_hook.h"
#include "draw/color.h"
#include "drawable/rs_screen_render_node_drawable.h"
#include "drawable/rs_surface_render_node_drawable.h"
#include "pipeline/hardware_thread/rs_realtime_refresh_rate_manager.h"
#include "pipeline/hwc/rs_uni_hwc_visitor.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/main_thread/rs_uni_render_visitor.h"
#include "pipeline/mock/mock_matrix.h"
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
#include "screen_manager/rs_screen.h"

using namespace testing;
using namespace testing::ext;

namespace {
constexpr uint32_t DEFAULT_CANVAS_WIDTH = 800;
constexpr uint32_t DEFAULT_CANVAS_HEIGHT = 600;
const OHOS::Rosen::RectI DEFAULT_RECT = { 0, 80, 1000, 1000 };
const OHOS::Rosen::RectI DEFAULT_FILTER_RECT = { 0, 0, 500, 500 };
const std::string CAPTURE_WINDOW_NAME = "CapsuleWindow";
constexpr int MAX_ALPHA = 255;
constexpr int SCREEN_WIDTH = 3120;
constexpr int SCREEN_HEIGHT = 1080;
constexpr OHOS::Rosen::NodeId DEFAULT_NODE_ID = 100;
const OHOS::Rosen::RectI DEFAULT_SCREEN_RECT = { 0, 0, 1000, 1000 };
} // namespace

namespace OHOS::Rosen {
class RSChildrenDrawableAdapter : public RSDrawable {
public:
    RSChildrenDrawableAdapter() = default;
    ~RSChildrenDrawableAdapter() override = default;
    bool OnUpdate(const RSRenderNode& content) override
    {
        return true;
    }
    void OnSync() override {}
    Drawing::RecordingCanvas::DrawFunc CreateDrawFunc() const override
    {
        auto ptr = std::static_pointer_cast<const RSChildrenDrawableAdapter>(shared_from_this());
        return [ptr](Drawing::Canvas* canvas, const Drawing::Rect* rect) {};
    }

private:
    bool OnSharedTransition(const std::shared_ptr<RSRenderNode>& node)
    {
        return true;
    }
    friend class RSRenderNode;
};

class RSUniRenderVisitorUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline Mock::MatrixMock* matrixMock_;
    ScreenId CreateVirtualScreen(sptr<RSScreenManager> screenManager);
    static void InitTestSurfaceNodeAndScreenInfo(
        std::shared_ptr<RSSurfaceRenderNode>& surfaceNode, std::shared_ptr<RSUniRenderVisitor>& rSUniRenderVisitor);
};

class MockRSSurfaceRenderNode : public RSSurfaceRenderNode {
public:
    explicit MockRSSurfaceRenderNode(
        NodeId id, const std::weak_ptr<RSContext>& context = {}, bool isTextureExportNode = false)
        : RSSurfaceRenderNode(id, context, isTextureExportNode)
    {}
    ~MockRSSurfaceRenderNode() override {}
    MOCK_METHOD0(CheckParticipateInOcclusion, bool());
    MOCK_CONST_METHOD0(NeedDrawBehindWindow, bool());
    MOCK_CONST_METHOD0(GetFilterRect, RectI());
    MOCK_METHOD0(CheckIfOcclusionChanged, bool());
};

void RSUniRenderVisitorUnitTest::SetUpTestCase()
{
    matrixMock_ = Mock::MatrixMock::GetInstance();
    EXPECT_CALL(*matrixMock_, GetMinMaxScales(_)).WillOnce(testing::Return(false));
    RSTestUtil::InitRenderNodeGC();
}
void RSUniRenderVisitorUnitTest::TearDownTestCase() {}
void RSUniRenderVisitorUnitTest::SetUp()
{
    if (RSUniRenderJudgement::IsUniRender()) {
        auto& uniRenderThread = RSUniRenderThread::Instance();
        uniRenderThread.uniRenderEngine_ = std::make_shared<RSUniRenderEngine>();
    }
}
void RSUniRenderVisitorUnitTest::TearDown()
{
    system::SetParameter("rosen.dirtyregiondebug.enabled", "0");
    system::SetParameter("rosen.uni.partialrender.enabled", "4");
    system::GetParameter("rosen.dirtyregiondebug.surfacenames", "0");
}

ScreenId RSUniRenderVisitorUnitTest::CreateVirtualScreen(sptr<RSScreenManager> screenManager)
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

void RSUniRenderVisitorUnitTest::InitTestSurfaceNodeAndScreenInfo(
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

/**
 * @tc.name: ProcessFilterNodeObscured
 * @tc.desc: Test ProcessFilterNodeObscured
 * @tc.type: FUNC
 * @tc.require: issueIB787L
 */
HWTEST_F(RSUniRenderVisitorUnitTest, ProcessFilterNodeObscured, TestSize.Level1)
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
    surfaceNode->renderProperties_.GetEffect().backgroundFilter_ = filter;
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
    filterNode1->renderProperties_.GetEffect().backgroundFilter_ = filter;
    filterNode3->renderProperties_.GetEffect().backgroundFilter_ = filter;
    filterNode4->renderProperties_.GetEffect().backgroundFilter_ = filter;
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
 * @tc.type:FUNC
 * @tc.require:issuesIAJO0U
 */
HWTEST_F(RSUniRenderVisitorUnitTest, AfterUpdateSurfaceDirtyCalc_001, TestSize.Level2)
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
HWTEST_F(RSUniRenderVisitorUnitTest, AfterUpdateSurfaceDirtyCalc_002, TestSize.Level2)
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
HWTEST_F(RSUniRenderVisitorUnitTest, UpdateHwcNodeInfoForAppNode_001, TestSize.Level2)
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
HWTEST_F(RSUniRenderVisitorUnitTest, UpdateHwcNodeInfoForAppNode_002, TestSize.Level2)
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
HWTEST_F(RSUniRenderVisitorUnitTest, UpdateHwcNodeInfoForAppNode_003, TestSize.Level2)
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
 * @tc.desc: Test RSUniRenderVisitorUnitTest.UpdateSurfaceRenderNodeScaleTest
 * @tc.type: FUNC
 * @tc.require: issueI79U8E
 */
HWTEST_F(RSUniRenderVisitorUnitTest, UpdateSurfaceRenderNodeScaleTest, TestSize.Level2)
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

/*
 * @tc.name: CheckLuminanceStatusChangeTest
 * @tc.desc: Test RSUniRenderVisitorUnitTest.CheckLuminanceStatusChangeTest
 * @tc.type: FUNC
 * @tc.require: issueI79U8E
 */
HWTEST_F(RSUniRenderVisitorUnitTest, CheckLuminanceStatusChangeTest, TestSize.Level2)
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
 * @tc.desc: Test RSUniRenderVisitorUnitTest.PrepareCanvasRenderNodeTest
 * @tc.type: FUNC
 * @tc.require: issueI79U8E
 */
HWTEST_F(RSUniRenderVisitorUnitTest, PrepareCanvasRenderNodeTest, TestSize.Level2)
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
HWTEST_F(RSUniRenderVisitorUnitTest, PrepareChildren001, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    auto rsBaseRenderNode = std::make_shared<RSBaseRenderNode>(10, rsContext->weak_from_this());
    rsBaseRenderNode->InitRenderParams();
    ASSERT_NE(rsBaseRenderNode->stagingRenderParams_, nullptr);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->PrepareChildren(*rsBaseRenderNode);
}

HWTEST_F(RSUniRenderVisitorUnitTest, PrepareProxyRenderNode001, TestSize.Level1)
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

HWTEST_F(RSUniRenderVisitorUnitTest, RSScreenRenderNode001, TestSize.Level1)
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
HWTEST_F(RSUniRenderVisitorUnitTest, CheckQuickSkipPrepareParamSetAndGetValid001, TestSize.Level1)
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
HWTEST_F(RSUniRenderVisitorUnitTest, CheckSurfaceRenderNodeNotStatic001, TestSize.Level1)
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
HWTEST_F(RSUniRenderVisitorUnitTest, CheckSurfaceRenderNodeStatic001, TestSize.Level1)
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

HWTEST_F(RSUniRenderVisitorUnitTest, PrepareRootRenderNode001, TestSize.Level1)
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
HWTEST_F(RSUniRenderVisitorUnitTest, CalcDirtyDisplayRegion, TestSize.Level1)
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
    std::map<NodeId, RSVisibleLevel> pidVisMap;

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
    rsSurfaceRenderNode->SetVisibleRegionRecursive(region, vData, pidVisMap);
    screenManager->RemoveVirtualScreen(screenId);
}

/*
 * @tc.name: CalcDirtyRegionForFilterNode
 * @tc.desc: Create a filter effect to test filter node
 * @tc.type: FUNC
 * @tc.require: I68IPR
 */
HWTEST_F(RSUniRenderVisitorUnitTest, CalcDirtyRegionForFilterNode, TestSize.Level1)
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
    rsCanvasRenderNode->GetMutableRenderProperties().GetEffect().filter_ = filter;
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
HWTEST_F(RSUniRenderVisitorUnitTest, CollectTopOcclusionSurfacesInfo001, TestSize.Level2)
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
HWTEST_F(RSUniRenderVisitorUnitTest, CollectTopOcclusionSurfacesInfo002, TestSize.Level2)
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
HWTEST_F(RSUniRenderVisitorUnitTest, CollectTopOcclusionSurfacesInfo003, TestSize.Level2)
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

/**
 * @tc.name: CollectTopOcclusionSurfacesInfo004
 * @tc.desc: test CollectTopOcclusionSurfacesInfo with first level cross node
 * @tc.type:FUNC
 * @tc.require:issuesIC88GA
 */
HWTEST_F(RSUniRenderVisitorUnitTest, CollectTopOcclusionSurfacesInfo004, TestSize.Level2)
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
 * @tc.name: GetSurfaceTransparentFilterRegion
 * @tc.desc: Test RSUniRenderVisitorUnitTest.GetSurfaceTransparentFilterRegion
 * @tc.type: FUNC
 * @tc.require: #issuesICCYYL
 */
HWTEST_F(RSUniRenderVisitorUnitTest, GetSurfaceTransparentFilterRegion, TestSize.Level2)
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

/*
 * @tc.name: CheckMergeFilterDirtyWithPreDirty_001
 * @tc.desc: Test RSUniRenderVisitorUnitTest.CheckMergeFilterDirtyWithPreDirty, test nullptr side case.
 * @tc.type: FUNC
 * @tc.require: #issuesICCYYL
 */
HWTEST_F(RSUniRenderVisitorUnitTest, CheckMergeFilterDirtyWithPreDirty_001, TestSize.Level2)
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
 * @tc.desc: Test RSUniRenderVisitorUnitTest.CheckMergeFilterDirtyWithPreDirty, test if intersect.
 * @tc.type: FUNC
 * @tc.require: #issuesICCYYL
 */
HWTEST_F(RSUniRenderVisitorUnitTest, CheckMergeFilterDirtyWithPreDirty_002, TestSize.Level2)
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
    filterNode1->GetMutableRenderProperties().GetEffect().backgroundFilter_ = std::make_shared<RSFilter>();
    filterNode2->GetMutableRenderProperties().GetEffect().needDrawBehindWindow_ = true;
    filterNode2->GetMutableRenderProperties().GetEffect().filter_ = std::make_shared<RSFilter>();

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
 * @tc.require: issueICA6FQ
 */
HWTEST_F(RSUniRenderVisitorUnitTest, InitializeOcclusionHandler001, TestSize.Level2)
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
 * @tc.name: CollectSubTreeAndProcessOcclusion001
 * @tc.desc: test CollectSubTreeAndProcessOcclusion
 * @tc.type: FUNC
 * @tc.require: issueICA6FQ
 */
HWTEST_F(RSUniRenderVisitorUnitTest, CollectSubTreeAndProcessOcclusion001, TestSize.Level2)
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
HWTEST_F(RSUniRenderVisitorUnitTest, SetSurfafaceGlobalDirtyRegion, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    config.id = 10;
    auto rsSurfaceRenderNode1 = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    rsSurfaceRenderNode1->InitRenderParams();
    Occlusion::Rect rect { 0, 80, 2560, 1600 };
    Occlusion::Region region { rect };
    VisibleData vData;
    std::map<NodeId, RSVisibleLevel> pidVisMap;
    rsSurfaceRenderNode1->SetVisibleRegionRecursive(region, vData, pidVisMap);

    config.id = 11;
    auto rsSurfaceRenderNode2 = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    rsSurfaceRenderNode2->InitRenderParams();
    Occlusion::Rect rect2 { 100, 100, 500, 1500 };
    Occlusion::Region region2 { rect2 };
    rsSurfaceRenderNode2->SetVisibleRegionRecursive(region2, vData, pidVisMap);

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
 * @tc.desc: Test RSUniRenderVisitorUnitTest.IsWatermarkFlagChanged test
 * @tc.type: FUNC
 * @tc.require: issuesIA8LNR
 */
HWTEST_F(RSUniRenderVisitorUnitTest, IsWatermarkFlagChanged, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->IsWatermarkFlagChanged();
}

/*
 * @tc.name: CheckPixelFormat
 * @tc.desc: Test RSUniRenderVisitorUnitTest.CheckPixelFormat test
 * @tc.type: FUNC
 * @tc.require: issuesIBSNHZ
 */
HWTEST_F(RSUniRenderVisitorUnitTest, CheckPixelFormat, TestSize.Level1)
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
    ASSERT_EQ(rsSurfaceRenderNode->IsContentDirty(), true);
}

/**
 * @tc.name: ProcessSurfaceRenderNode001
 * @tc.desc: Test RSUniRenderVisitorUnitTest.ProcessSurfaceRenderNode when displaynode is null
 * @tc.type: FUNC
 * @tc.require: issueI79KM8
 */
HWTEST_F(RSUniRenderVisitorUnitTest, ProcessSurfaceRenderNode001, TestSize.Level1)
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
 * @tc.desc: Test RSUniRenderVisitorUnitTest.ProcessSurfaceRenderNode with isUIFirst_
 * @tc.type: FUNC
 * @tc.require: issueI79KM8
 */
HWTEST_F(RSUniRenderVisitorUnitTest, ProcessSurfaceRenderNode002, TestSize.Level1)
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
 * @tc.desc: Test RSUniRenderVisitorUnitTest.ProcessSurfaceRenderNode with skipLayer
 * @tc.type: FUNC
 * @tc.require: issueI80HL4
 */
HWTEST_F(RSUniRenderVisitorUnitTest, ProcessSurfaceRenderNode003, TestSize.Level1)
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
 * @tc.desc: Test RSUniRenderVisitorUnitTest.ProcessSurfaceRenderNode with securityLayer
 * @tc.type: FUNC
 * @tc.require: issueI80HL4
 */
HWTEST_F(RSUniRenderVisitorUnitTest, ProcessSurfaceRenderNode004, TestSize.Level1)
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
 * @tc.desc: Test RSUniRenderVisitorUnitTest.ProcessSurfaceRenderNode with capture window in directly render
 * @tc.type: FUNC
 * @tc.require: issueI80HL4
 */
HWTEST_F(RSUniRenderVisitorUnitTest, ProcessSurfaceRenderNode005, TestSize.Level1)
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
 * @tc.desc: Test RSUniRenderVisitorUnitTest.ProcessSurfaceRenderNode with capture window in offscreen render
 * @tc.type: FUNC
 * @tc.require: issueI80HL4
 */
HWTEST_F(RSUniRenderVisitorUnitTest, ProcessSurfaceRenderNode006, TestSize.Level1)
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
 * @tc.desc: Test RSUniRenderVisitorUnitTest.ProcessSurfaceRenderNode while surface node has protected layer
 * @tc.type: FUNC
 * @tc.require: issueI7ZSC2
 */
HWTEST_F(RSUniRenderVisitorUnitTest, ProcessSurfaceRenderNode007, TestSize.Level1)
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
 * @tc.desc: Test RSUniRenderVisitorUnitTest.ProcessSurfaceRenderNode while surface node does not have protected layer
 * @tc.type: FUNC
 * @tc.require: issueI7ZSC2
 */
HWTEST_F(RSUniRenderVisitorUnitTest, ProcessSurfaceRenderNode008, TestSize.Level1)
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
 * @tc.desc: Test RSUniRenderVisitorUnitTest.PrepareEffectRenderNode api
 * @tc.type: FUNC
 * @tc.require: issueI79KM8
 */
HWTEST_F(RSUniRenderVisitorUnitTest, PrepareEffectRenderNode001, TestSize.Level1)
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
 * @tc.desc: Test RSUniRenderVisitorUnitTest.ProcessEffectRenderNode api
 * @tc.type: FUNC
 * @tc.require: issueI79KM8
 */
HWTEST_F(RSUniRenderVisitorUnitTest, ProcessEffectRenderNode001, TestSize.Level1)
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
 * @tc.desc: Test RSUniRenderVisitorUnitTest.CheckColorSpace while
 *           app Window node's color space is not equal to GRAPHIC_COLOR_GAMUT_SRGB
 * @tc.type: FUNC
 * @tc.require: issueI7O6WO
 */
HWTEST_F(RSUniRenderVisitorUnitTest, CheckColorSpace001, TestSize.Level2)
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
HWTEST_F(RSUniRenderVisitorUnitTest, UpdateBlackListRecord001, TestSize.Level1)
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

/**
 * @tc.name: PrepareForCloneNode
 * @tc.desc: Test PrepareForCloneNode while node is not clone
 * @tc.type: FUNC
 * @tc.require: issueIBH7WD
 */
HWTEST_F(RSUniRenderVisitorUnitTest, PrepareForCloneNode, TestSize.Level1)
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
HWTEST_F(RSUniRenderVisitorUnitTest, PrepareForCloneNode001, TestSize.Level1)
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
    surfaceRenderNode.SetClonedNodeInfo(surfaceRenderNodeCloned->GetId(), true);
    auto result = rsUniRenderVisitor->PrepareForCloneNode(surfaceRenderNode);
    ASSERT_TRUE(result);
}

/**
 * @tc.name: PrepareForMultiScreenViewSurfaceNode001
 * @tc.desc: Test PrepareForMultiScreenViewSurfaceNode when sourceNode and sourceNodeDrawable is null
 * @tc.type: FUNC
 * @tc.require: issueIBKU7U
 */
HWTEST_F(RSUniRenderVisitorUnitTest, PrepareForMultiScreenViewSurfaceNode001, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    auto surfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(1);
    surfaceRenderNode->SetSourceDisplayRenderNodeId(2);
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
HWTEST_F(RSUniRenderVisitorUnitTest, PrepareForMultiScreenViewSurfaceNode002, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    auto surfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(1);
    surfaceRenderNode->SetSourceDisplayRenderNodeId(2);
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
HWTEST_F(RSUniRenderVisitorUnitTest, PrepareForMultiScreenViewSurfaceNode003, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    auto surfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(1);
    surfaceRenderNode->SetSourceDisplayRenderNodeId(2);
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
HWTEST_F(RSUniRenderVisitorUnitTest, PrepareForMultiScreenViewDisplayNode001, TestSize.Level1)
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
HWTEST_F(RSUniRenderVisitorUnitTest, PrepareForMultiScreenViewDisplayNode002, TestSize.Level1)
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
HWTEST_F(RSUniRenderVisitorUnitTest, PrepareForMultiScreenViewDisplayNode003, TestSize.Level1)
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
HWTEST_F(RSUniRenderVisitorUnitTest, PrepareForCrossNodeTest, TestSize.Level1)
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
HWTEST_F(RSUniRenderVisitorUnitTest, DealWithSpecialLayer, TestSize.Level1)
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
HWTEST_F(RSUniRenderVisitorUnitTest, CheckSkipCrossNodeTest, TestSize.Level1)
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
HWTEST_F(RSUniRenderVisitorUnitTest, ResetCrossNodesVisitedStatusTest, TestSize.Level1)
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
HWTEST_F(RSUniRenderVisitorUnitTest, HandleColorGamuts001, TestSize.Level2)
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
 * @tc.name: CheckColorSpaceWithSelfDrawingNode001
 * @tc.desc: Test RSUniRenderVisitorUnitTest.CheckColorSpaceWithSelfDrawingNode while
 *           selfDrawingNode's color space is equal to GRAPHIC_COLOR_GAMUT_DISPLAY_P3,
 *           and this node will be drawn with gpu
 * @tc.type: FUNC
 * @tc.require: issueICGKPE
 */
HWTEST_F(RSUniRenderVisitorUnitTest, CheckColorSpaceWithSelfDrawingNode001, TestSize.Level2)
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
HWTEST_F(RSUniRenderVisitorUnitTest, UpdateColorSpaceWithMetadata001, TestSize.Level1)
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
HWTEST_F(RSUniRenderVisitorUnitTest, UpdateColorSpaceWithMetadata002, TestSize.Level1)
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
HWTEST_F(RSUniRenderVisitorUnitTest, UpdateColorSpaceAfterHwcCalc_001, TestSize.Level2)
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
HWTEST_F(RSUniRenderVisitorUnitTest, ResetCurSurfaceInfoAsUpperSurfaceParent001, TestSize.Level2)
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
HWTEST_F(RSUniRenderVisitorUnitTest, ResetCurSurfaceInfoAsUpperSurfaceParent002, TestSize.Level2)
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
} // namespace OHOS::Rosen