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
#include "feature/opinc/rs_opinc_manager.h"
#include "feature/round_corner_display/rs_round_corner_display.h"
#include "feature/round_corner_display/rs_round_corner_display_manager.h"
#include "feature/window_keyframe/rs_window_keyframe_render_node.h"
#include "feature_cfg/graphic_feature_param_manager.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "limit_number.h"
#include "surface_buffer_impl.h"
#include "system/rs_system_parameters.h"

#include "common/rs_common_hook.h"
#include "draw/color.h"
#include "drawable/rs_color_picker_drawable.h"
#include "drawable/rs_screen_render_node_drawable.h"
#include "modifier_ng/appearance/rs_behind_window_filter_render_modifier.h"
#include "pipeline/hardware_thread/rs_realtime_refresh_rate_manager.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/main_thread/rs_uni_render_visitor.h"
#include "pipeline/mock/mock_rs_luminance_control.h"
#include "pipeline/render_thread/rs_uni_render_engine.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "pipeline/render_thread/rs_uni_render_util.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_effect_render_node.h"
#include "pipeline/rs_processor_factory.h"
#include "pipeline/rs_proxy_render_node.h"
#include "pipeline/rs_render_thread.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_screen_render_node.h"
#include "pipeline/rs_test_util.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "pipeline/rs_union_render_node.h"
#include "screen_manager/rs_screen.h"

using namespace testing;
using namespace testing::ext;

namespace {
constexpr uint32_t DEFAULT_CANVAS_WIDTH = 802;
constexpr uint32_t DEFAULT_CANVAS_HEIGHT = 602;
const OHOS::Rosen::RectI DEFAULT_RECT = { 0, 82, 1002, 1002 };
const OHOS::Rosen::RectI DEFAULT_FILTER_RECT = { 0, 0, 502, 502 };
constexpr int MAX_ALPHA = 255;
constexpr int SCREEN_WIDTH = 3122;
constexpr int SCREEN_HEIGHT = 1082;
constexpr OHOS::Rosen::NodeId DEFAULT_NODE_ID = 102;
const OHOS::Rosen::RectI DEFAULT_SCREEN_RECT = { 0, 0, 1002, 1002 };
} // namespace

namespace OHOS::Rosen {
class RSSecondDrawableAdapter : public RSDrawable {
public:
    RSSecondDrawableAdapter() = default;
    ~RSSecondDrawableAdapter() override = default;
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

class SecondRenderVisitorTest : public testing::Test {
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

class MockSecondRenderNode : public RSSurfaceRenderNode {
public:
    explicit MockSecondRenderNode(
        NodeId id, const std::weak_ptr<RSContext>& context = {}, bool isTextureExportNode = false)
        : RSSurfaceRenderNode(id, context, isTextureExportNode)
    {}
    ~MockSecondRenderNode() override {}
    MOCK_METHOD1(CheckParticipateInOcclusion, bool(bool));
    MOCK_CONST_METHOD0(NeedDrawBehindWindow, bool());
    MOCK_CONST_METHOD0(GetFilterRect, RectI());
    MOCK_METHOD0(CheckIfOcclusionChanged, bool());
};

void SecondRenderVisitorTest::SetUpTestCase()
{
    matrixMock_ = Mock::MatrixMock::GetInstance();
    EXPECT_CALL(*matrixMock_, GetMinMaxScales(_)).WillOnce(testing::Return(false));
    RSTestUtil::InitRenderNodeGC();
}

void SecondRenderVisitorTest::TearDownTestCase() {}

void SecondRenderVisitorTest::SetUp()
{
    if (RSUniRenderJudgement::IsUniRender()) {
        auto& uniRenderThread = RSUniRenderThread::Instance();
        uniRenderThread.uniRenderEngine_ = std::make_shared<RSUniRenderEngine>();
    }
}

void SecondRenderVisitorTest::TearDown()
{
    system::SetParameter("rosen.dirtyregiondebug.enabled", "0");
    system::SetParameter("rosen.uni.partialrender.enabled", "4");
    system::GetParameter("rosen.dirtyregiondebug.surfacenames", "0");
}

ScreenId SecondRenderVisitorTest::CreateVirtualScreen(sptr<RSScreenManager> screenManager)
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

void SecondRenderVisitorTest::InitTestSurfaceNodeAndScreenInfo(
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

std::shared_ptr<RSUniRenderVisitor> SecondRenderVisitorTest::InitRSUniRenderVisitor()
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
 * @tc.name: ProcessSurfaceRenderNode001
 * @tc.desc: Test SecondRenderVisitorTest.ProcessSurfaceRenderNode when displaynode is null
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(SecondRenderVisitorTest, ProcessSurfaceRenderNode001, TestSize.Level1)
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

/*
 * @tc.name: CheckPixelFormat
 * @tc.desc: Test SecondRenderVisitorTest.CheckPixelFormat test
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(SecondRenderVisitorTest, CheckPixelFormat, TestSize.Level1)
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
 * @tc.name: ProcessSurfaceRenderNode002
 * @tc.desc: Test SecondRenderVisitorTest.ProcessSurfaceRenderNode with isUIFirst_
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(SecondRenderVisitorTest, ProcessSurfaceRenderNode002, TestSize.Level1)
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
 * @tc.name: ProcessSurfaceRenderNode004
 * @tc.desc: Test SecondRenderVisitorTest.ProcessSurfaceRenderNode with securityLayer
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(SecondRenderVisitorTest, ProcessSurfaceRenderNode004, TestSize.Level1)
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
 * @tc.name: ProcessSurfaceRenderNode003
 * @tc.desc: Test SecondRenderVisitorTest.ProcessSurfaceRenderNode with skipLayer
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(SecondRenderVisitorTest, ProcessSurfaceRenderNode003, TestSize.Level1)
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
 * @tc.name: ProcessSurfaceRenderNode005
 * @tc.desc: Test SecondRenderVisitorTest.ProcessSurfaceRenderNode with capture window in directly render
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(SecondRenderVisitorTest, ProcessSurfaceRenderNode005, TestSize.Level1)
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
 * @tc.desc: Test SecondRenderVisitorTest.ProcessSurfaceRenderNode while surface node has protected layer
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(SecondRenderVisitorTest, ProcessSurfaceRenderNode007, TestSize.Level1)
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
 * @tc.name: ProcessSurfaceRenderNode006
 * @tc.desc: Test SecondRenderVisitorTest.ProcessSurfaceRenderNode with capture window in offscreen render
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(SecondRenderVisitorTest, ProcessSurfaceRenderNode006, TestSize.Level1)
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
 * @tc.name: ProcessSurfaceRenderNode008
 * @tc.desc: Test SecondRenderVisitorTest.ProcessSurfaceRenderNode while surface node does not have protected layer
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(SecondRenderVisitorTest, ProcessSurfaceRenderNode008, TestSize.Level1)
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
 * @tc.desc: Test SecondRenderVisitorTest.PrepareEffectRenderNode api
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(SecondRenderVisitorTest, PrepareEffectRenderNode001, TestSize.Level1)
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
 * @tc.name: CheckColorSpace001
 * @tc.desc: Test SecondRenderVisitorTest.CheckColorSpace while
 *           app Window node's color space is not equal to GRAPHIC_COLOR_GAMUT_SRGB
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(SecondRenderVisitorTest, CheckColorSpace001, TestSize.Level2)
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
 * @tc.name: ProcessEffectRenderNode001
 * @tc.desc: Test SecondRenderVisitorTest.ProcessEffectRenderNode api
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(SecondRenderVisitorTest, ProcessEffectRenderNode001, TestSize.Level1)
{
    NodeId id = 0;
    RSEffectRenderNode node(id);
    node.stagingRenderParams_ = std::make_unique<RSRenderParams>(node.GetId());
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->ProcessEffectRenderNode(node);
}

/**
 * @tc.name: UpdateBlackListRecord001
 * @tc.desc: Test UpdateBlackListRecord
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(SecondRenderVisitorTest, UpdateBlackListRecord001, TestSize.Level1)
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
 * @tc.name: UpdateBlackListRecord004
 * @tc.desc: Test UpdateBlackListRecord while hasVirtualDisplay and screenManager is valid
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(SecondRenderVisitorTest, UpdateBlackListRecord004, TestSize.Level2)
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
 * @tc.name: UpdateBlackListRecord003
 * @tc.desc: Test UpdateBlackListRecord while hasVirtualDisplay
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(SecondRenderVisitorTest, UpdateBlackListRecord003, TestSize.Level2)
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
 * @tc.name: UpdateBlackListRecord005
 * @tc.desc: Test UpdateBlackListRecord while has virtual mirror display and screenManager is valid
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(SecondRenderVisitorTest, UpdateBlackListRecord005, TestSize.Level2)
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
 * @tc.desc: Test PrepareForCloneNode while node is clone
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(SecondRenderVisitorTest, PrepareForCloneNode001, TestSize.Level1)
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
 * @tc.name: PrepareForCloneNode
 * @tc.desc: Test PrepareForCloneNode while node is not clone
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(SecondRenderVisitorTest, PrepareForCloneNode, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto surfaceRenderNode = RSSurfaceRenderNode(1);

    auto result = rsUniRenderVisitor->PrepareForCloneNode(surfaceRenderNode);
    ASSERT_FALSE(result);
}

/**
 * @tc.name: UpdateInfoForClonedNode
 * @tc.desc: Test UpdateInfoForClonedNode
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(SecondRenderVisitorTest, UpdateInfoForClonedNode, TestSize.Level1)
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
 * @tc.name: PrepareForMultiScreenViewSurfaceNode001
 * @tc.desc: Test PrepareForMultiScreenViewSurfaceNode when sourceNode and sourceNodeDrawable is null
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(SecondRenderVisitorTest, PrepareForMultiScreenViewSurfaceNode001, TestSize.Level1)
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
 * @tc.name: PrepareForCloneNode
 * @tc.desc: Test PrepareForCloneNode while node is clone
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(SecondRenderVisitorTest, PrepareForCloneNode002, TestSize.Level1)
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
 * @tc.name: PrepareForMultiScreenViewSurfaceNode002
 * @tc.desc: Test PrepareForMultiScreenViewSurfaceNode when sourceNodeDrawable is null
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(SecondRenderVisitorTest, PrepareForMultiScreenViewSurfaceNode002, TestSize.Level1)
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
 * @tc.name: PrepareForMultiScreenViewDisplayNode001
 * @tc.desc: Test Pre when targetNode and targetNodeDrawable is null
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(SecondRenderVisitorTest, PrepareForMultiScreenViewDisplayNode001, TestSize.Level1)
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
 * @tc.name: PrepareForMultiScreenViewSurfaceNode003
 * @tc.desc: Test PrepareForMultiScreenViewSurfaceNode success
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(SecondRenderVisitorTest, PrepareForMultiScreenViewSurfaceNode003, TestSize.Level1)
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
 * @tc.name: PrepareForMultiScreenViewDisplayNode002
 * @tc.desc: Test PrepareForMultiScreenViewSurfaceNode when targetNodeDrawable is null
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(SecondRenderVisitorTest, PrepareForMultiScreenViewDisplayNode002, TestSize.Level1)
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
 * @tc.name: PrepareForCrossNodeTest
 * @tc.desc: Test PrepareForCrossNode
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(SecondRenderVisitorTest, PrepareForCrossNodeTest, TestSize.Level1)
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
 * @tc.name: PrepareForMultiScreenViewDisplayNode003
 * @tc.desc: Test PrepareForMultiScreenViewSurfaceNode success
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(SecondRenderVisitorTest, PrepareForMultiScreenViewDisplayNode003, TestSize.Level1)
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
 * @tc.name: DealWithSpecialLayer
 * @tc.desc: Test DealWithSpecialLayer
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(SecondRenderVisitorTest, DealWithSpecialLayer, TestSize.Level1)
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
 * @tc.name: ResetCrossNodesVisitedStatusTest
 * @tc.desc: Test ResetCrossNodesVisitedStatus
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(SecondRenderVisitorTest, ResetCrossNodesVisitedStatusTest, TestSize.Level1)
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
 * @tc.name: CheckSkipCrossNodeTest
 * @tc.desc: Test CheckSkipCrossNode
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(SecondRenderVisitorTest, CheckSkipCrossNodeTest, TestSize.Level1)
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
 * @tc.name: HandleColorGamuts001
 * @tc.desc: HandleColorGamuts for virtual screen
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(SecondRenderVisitorTest, HandleColorGamuts001, TestSize.Level2)
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
 * @tc.name: HandleColorGamuts003
 * @tc.desc: Test HandleColorGamuts
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(SecondRenderVisitorTest, HandleColorGamuts003, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto screenNode = std::make_shared<RSScreenRenderNode>(0, 0);

    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    ASSERT_NE(screenManager, nullptr);

    ScreenId screenId = 1;
    auto rsScreen = std::make_shared<RSScreen>(HdiOutput::CreateHdiOutput(screenId));
    rsScreen->property_.SetConnectionType(ScreenConnectionType::DISPLAY_CONNECTION_TYPE_EXTERNAL);
    rsScreen->supportedPhysicalColorGamuts_ = { COLOR_GAMUT_SRGB, COLOR_GAMUT_DCI_P3 };
    rsUniRenderVisitor->screenManager_->MockHdiScreenConnected(rsScreen);
    screenNode->screenId_ = screenId;
    auto virtualScreenId = screenManager->CreateVirtualScreen("virtual screen 003", 0, 0, nullptr);
    screenNode->screenInfo_ = screenManager->QueryScreenInfo(virtualScreenId);
    ASSERT_NE(INVALID_SCREEN_ID, virtualScreenId);

    GraphicColorGamut originalColorSpace = GRAPHIC_COLOR_GAMUT_DISPLAY_P3;
    screenNode->SetColorSpace(originalColorSpace);
    rsScreen->supportedPhysicalColorGamuts_ = { COLOR_GAMUT_NATIVE, COLOR_GAMUT_DISPLAY_P3 };
    rsUniRenderVisitor->HandleColorGamuts(*screenNode);
    ScreenColorGamut screenColorGamut;
    screenManager->GetScreenColorGamut(screenNode->GetScreenId(), screenColorGamut);
    ASSERT_NE(screenNode->GetColorSpace(), static_cast<GraphicColorGamut>(screenColorGamut));
}

/**
 * @tc.name: HandleColorGamuts002
 * @tc.desc: Test HandleColorGamuts
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(SecondRenderVisitorTest, HandleColorGamuts002, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto screenNode = std::make_shared<RSScreenRenderNode>(0, 0);

    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    ASSERT_NE(screenManager, nullptr);

    ScreenId screenId = 1;
    auto rsScreen = std::make_shared<RSScreen>(HdiOutput::CreateHdiOutput(screenId));
    rsScreen->property_.SetConnectionType(ScreenConnectionType::DISPLAY_CONNECTION_TYPE_EXTERNAL);
    rsScreen->supportedPhysicalColorGamuts_ = { COLOR_GAMUT_SRGB, COLOR_GAMUT_DCI_P3 };
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
 * @tc.name: HandleColorGamuts004
 * @tc.desc: Test HandleColorGamuts
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(SecondRenderVisitorTest, HandleColorGamuts004, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto screenNode = std::make_shared<RSScreenRenderNode>(0, 0);

    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    ASSERT_NE(screenManager, nullptr);

    ScreenId screenId = 1;
    auto rsScreen = std::make_shared<RSScreen>(HdiOutput::CreateHdiOutput(screenId));
    rsScreen->property_.SetConnectionType(ScreenConnectionType::DISPLAY_CONNECTION_TYPE_EXTERNAL);
    rsScreen->supportedPhysicalColorGamuts_ = { COLOR_GAMUT_SRGB, COLOR_GAMUT_DCI_P3 };
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
 * @tc.name: CheckColorSpaceWithSelfDrawingNode001
 * @tc.desc: Test SecondRenderVisitorTest.CheckColorSpaceWithSelfDrawingNode while
 *           selfDrawingNode's color space is equal to GRAPHIC_COLOR_GAMUT_DISPLAY_P3,
 *           and this node will be drawn with gpu
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(SecondRenderVisitorTest, CheckColorSpaceWithSelfDrawingNode001, TestSize.Level2)
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

/**
 * @tc.name: HandleColorGamuts005
 * @tc.desc: Test HandleColorGamuts
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(SecondRenderVisitorTest, HandleColorGamuts005, TestSize.Level2)
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
    rsScreen->supportedPhysicalColorGamuts_ = { COLOR_GAMUT_SRGB, COLOR_GAMUT_DCI_P3 };
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

#ifndef ROSEN_CROSS_PLATFORM
/**
 * @tc.name: UpdateColorSpaceWithMetadata
 * @tc.desc: test results of UpdateColorSpaceWithMetadata, if node has buffer
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(SecondRenderVisitorTest, UpdateColorSpaceWithMetadata002, TestSize.Level1)
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

/**
 * @tc.name: UpdateColorSpaceWithMetadata
 * @tc.desc: test results of UpdateColorSpaceWithMetadata, if node has no buffer
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(SecondRenderVisitorTest, UpdateColorSpaceWithMetadata001, TestSize.Level1)
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
#endif

/*
 * @tc.name: ResetCurSurfaceInfoAsUpperSurfaceParent001
 * @tc.desc: Reset but keep single node's surfaceInfo
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(SecondRenderVisitorTest, ResetCurSurfaceInfoAsUpperSurfaceParent001, TestSize.Level2)
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
 * @tc.name: UpdateColorSpaceAfterHwcCalc_001
 * @tc.desc: Test UpdateColorSpaceAfterHwcCalc when there is a P3 selfDrawingNode.
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(SecondRenderVisitorTest, UpdateColorSpaceAfterHwcCalc_001, TestSize.Level2)
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
 * @tc.name: ResetCurSurfaceInfoAsUpperSurfaceParent002
 * @tc.desc: Reset but keep node's surfaceInfo since upper surface's InstanceRootNode is not registered
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(SecondRenderVisitorTest, ResetCurSurfaceInfoAsUpperSurfaceParent002, TestSize.Level2)
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
 * @tc.name: UpdateCornerRadiusInfoForDRM
 * @tc.desc: Test RSUniRenderVistorTest.UpdateCornerRadiusInfoForDRM
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(SecondRenderVisitorTest, UpdateCornerRadiusInfoForDRM, TestSize.Level2)
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

    instanceRootNode->absDrawRect_ = { -10, -10, 100, 100 };
    instanceRootNode->SetGlobalCornerRadius({ -10, -10, 100, 100 });
    rsUniRenderVisitor->UpdateCornerRadiusInfoForDRM(surfaceNode, hwcRects);

    surfaceNode->GetInstanceRootNode()->renderProperties_.SetBounds({ 0, 0, 1, 1 });
    Drawing::Matrix matrix;
    matrix.SetMatrix(1, 2, 3, 4, 5, 6, 7, 8, 9);
    surfaceNode->SetTotalMatrix(matrix);
    surfaceNode->selfDrawRect_ = { 0, 0, 1, 1 };
    rsUniRenderVisitor->UpdateCornerRadiusInfoForDRM(surfaceNode, hwcRects);
}

/*
 * @tc.name: ResetCurSurfaceInfoAsUpperSurfaceParent003
 * @tc.desc: Reset curSurfaceInfo when upper surface is leash/main window
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(SecondRenderVisitorTest, ResetCurSurfaceInfoAsUpperSurfaceParent003, TestSize.Level2)
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
 * @tc.name: CheckIfRoundCornerIntersectDRM
 * @tc.desc: Test CheckIfRoundCornerIntersectDRM
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(SecondRenderVisitorTest, CheckIfRoundCornerIntersectDRM, TestSize.Level2)
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
    instanceRootNode->absDrawRect_ = { -10, -10, 1000, 1000 };
    instanceRootNode->SetGlobalCornerRadius({ -10, -10, 1000, 1000 });
    hwcNode->GetInstanceRootNode()->renderProperties_.SetBounds({ 0, 0, 1, 1 });
    Drawing::Matrix matrix = Drawing::Matrix();
    matrix.SetMatrix(1, 2, 3, 4, 5, 6, 7, 8, 9);
    hwcNode->SetTotalMatrix(matrix);
    hwcNode->selfDrawRect_ = { 0, 0, 1, 1 };

    auto instanceNode = hwcNode->GetInstanceRootNode()
                            ? hwcNode->GetInstanceRootNode()->ReinterpretCastTo<RSSurfaceRenderNode>()
                            : nullptr;
    auto hwcGeo = hwcNode->GetRenderProperties().GetBoundsGeometry();
    auto instanceAbsRect = instanceNode->GetAbsDrawRect();
    auto instanceCornerRadius = instanceNode->GetGlobalCornerRadius();
    std::vector<float> ratioVector = { 0.0f, 0.0f, 0.0f, 0.0f };
    auto hwcAbsRect = hwcGeo->MapRect(hwcNode->GetSelfDrawRect(), hwcNode->GetTotalMatrix());
    hwcAbsRect = hwcAbsRect.IntersectRect(instanceAbsRect);
    auto ratio = static_cast<float>(instanceAbsRect.GetWidth()) / instanceNode->GetRenderProperties().GetBoundsWidth();
    bool result = rsUniRenderVisitor->CheckIfRoundCornerIntersectDRM(
        ratio, ratioVector, instanceCornerRadius, instanceAbsRect, hwcAbsRect);
    ASSERT_TRUE(result);
}

/*
 * @tc.name: QuickPrepareSurfaceRenderNode002
 * @tc.desc: Test SecondRenderVisitorTest.QuickPrepareSurfaceRenderNode while surface node has skip layer
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(SecondRenderVisitorTest, QuickPrepareSurfaceRenderNode002, TestSize.Level2)
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
    rsUniRenderVisitor->curLogicalDisplayNode_->GetMultableSpecialLayerMgr().RemoveIds(
        SpecialLayerType::SKIP, surfaceNode->GetId());
    ASSERT_EQ(rsUniRenderVisitor->curLogicalDisplayNode_->GetSpecialLayerMgr().Find(SpecialLayerType::SKIP), false);
}

/*
 * @tc.name: QuickPrepareSurfaceRenderNode001
 * @tc.desc: Test SecondRenderVisitorTest.QuickPrepareSurfaceRenderNode while surface node has security layer
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(SecondRenderVisitorTest, QuickPrepareSurfaceRenderNode001, TestSize.Level2)
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
 * @tc.name: QuickPrepareSurfaceRenderNode003
 * @tc.desc: Test SecondRenderVisitorTest.QuickPrepareSurfaceRenderNode while surface node is capture window
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(SecondRenderVisitorTest, QuickPrepareSurfaceRenderNode003, TestSize.Level2)
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
 * @tc.name: PrepareSurfaceRenderNode005
 * @tc.desc: Test SecondRenderVisitorTest.PrepareSurfaceRenderNode while surface node does not have protected layer
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(SecondRenderVisitorTest, PrepareSurfaceRenderNode005, TestSize.Level2)
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

/*
 * @tc.name: QuickPrepareSurfaceRenderNode004
 * @tc.desc: Test SecondRenderVisitorTest.QuickPrepareSurfaceRenderNode while surface node has protected layer
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(SecondRenderVisitorTest, QuickPrepareSurfaceRenderNode004, TestSize.Level2)
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

/**
 * @tc.name: QuickPrepareScreenRenderNode005
 * @tc.desc: Test QuickPrepareScreenRenderNode
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(SecondRenderVisitorTest, QuickPrepareScreenRenderNode005, TestSize.Level2)
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
 * @tc.name: CheckSkipBackgroundSurfaceRenderNode001
 * @tc.desc: Test SecondRenderVisitorTest.CheckSkipBackgroundSurfaceRenderNode while has visible region
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(SecondRenderVisitorTest, CheckSkipBackgroundSurfaceRenderNode001, TestSize.Level2)
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
 * @tc.name: QuickPrepareSurfaceRenderNode006
 * @tc.desc: Test SecondRenderVisitorTest.QuickPrepareSurfaceRenderNode while surface node should be skipped
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(SecondRenderVisitorTest, QuickPrepareSurfaceRenderNode006, TestSize.Level2)
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
 * @tc.name: CheckSkipBackgroundSurfaceRenderNode002
 * @tc.desc: Test SecondRenderVisitorTest.CheckSkipBackgroundSurfaceRenderNode while tree state changed dirty
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(SecondRenderVisitorTest, CheckSkipBackgroundSurfaceRenderNode002, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetTreeStateChangeDirty(true);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    auto isQuickSkip = rsUniRenderVisitor->CheckSkipBackgroundSurfaceRenderNode(*surfaceNode);
    ASSERT_TRUE(!isQuickSkip);
}

/*
 * @tc.name: CheckSkipBackgroundSurfaceRenderNode004
 * @tc.desc: Test SecondRenderVisitorTest.CheckSkipBackgroundSurfaceRenderNode while curScreenNode_ is nullptr
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(SecondRenderVisitorTest, CheckSkipBackgroundSurfaceRenderNode004, TestSize.Level2)
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
 * @tc.name: CheckSkipBackgroundSurfaceRenderNode003
 * @tc.desc: Test SecondRenderVisitorTest.CheckSkipBackgroundSurfaceRenderNode while accumulatedOcclusionRegion_ is
 * empty
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(SecondRenderVisitorTest, CheckSkipBackgroundSurfaceRenderNode003, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    rsUniRenderVisitor->accumulatedOcclusionRegion_.Reset();
    auto isQuickSkip = rsUniRenderVisitor->CheckSkipBackgroundSurfaceRenderNode(*surfaceNode);
    ASSERT_TRUE(!isQuickSkip);
}

/*
 * @tc.name: CheckSkipBackgroundSurfaceRenderNode005
 * @tc.desc: Test SecondRenderVisitorTest.CheckSkipBackgroundSurfaceRenderNode without full screen occlusion
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(SecondRenderVisitorTest, CheckSkipBackgroundSurfaceRenderNode005, TestSize.Level2)
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
 * @tc.name: CheckSkipAndUpdateForegroundSurfaceRenderNode001
 * @tc.desc: Test SecondRenderVisitorTest.CheckSkipAndUpdateForegroundSurfaceRenderNode while surfaceWindowType_ is
 * SCB_SCREEN_LOCK
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(SecondRenderVisitorTest, CheckSkipAndUpdateForegroundSurfaceRenderNode001, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    const_cast<SurfaceWindowType&>(surfaceNode->surfaceWindowType_) = SurfaceWindowType::SCB_SCREEN_LOCK;

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    auto isQuickSkip = rsUniRenderVisitor->CheckSkipAndUpdateForegroundSurfaceRenderNode(*surfaceNode);
    ASSERT_TRUE(!isQuickSkip);
}

/*
 * @tc.name: CheckSkipBackgroundSurfaceRenderNode006
 * @tc.desc: Test SecondRenderVisitorTest.CheckSkipBackgroundSurfaceRenderNode while full screen occlusion and visible
 * region is empty
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(SecondRenderVisitorTest, CheckSkipBackgroundSurfaceRenderNode006, TestSize.Level2)
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
 * @tc.name: CheckSkipAndUpdateForegroundSurfaceRenderNode002
 * @tc.desc: Test SecondRenderVisitorTest.CheckSkipAndUpdateForegroundSurfaceRenderNode while window is opaque
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(SecondRenderVisitorTest, CheckSkipAndUpdateForegroundSurfaceRenderNode002, TestSize.Level2)
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
 * @tc.name: CheckSkipAndUpdateForegroundSurfaceRenderNode004
 * @tc.desc: Test SecondRenderVisitorTest.CheckSkipAndUpdateForegroundSurfaceRenderNode while met skip, and child has no
 * visible filter
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(SecondRenderVisitorTest, CheckSkipAndUpdateForegroundSurfaceRenderNode004, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetStableSkipReached(true);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    auto isQuickSkip = rsUniRenderVisitor->CheckSkipAndUpdateForegroundSurfaceRenderNode(*surfaceNode);
    ASSERT_TRUE(isQuickSkip);
}

/*
 * @tc.name: CheckSkipAndUpdateForegroundSurfaceRenderNode003
 * @tc.desc: Test SecondRenderVisitorTest.CheckSkipAndUpdateForegroundSurfaceRenderNode while met skip condition for
 * first time
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(SecondRenderVisitorTest, CheckSkipAndUpdateForegroundSurfaceRenderNode003, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    const_cast<SurfaceWindowType&>(surfaceNode->surfaceWindowType_) = SurfaceWindowType::SCB_GESTURE_BACK;

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    auto isQuickSkip = rsUniRenderVisitor->CheckSkipAndUpdateForegroundSurfaceRenderNode(*surfaceNode);
    ASSERT_TRUE(!isQuickSkip);
}

/*
 * @tc.name: CheckSkipAndUpdateForegroundSurfaceRenderNode005
 * @tc.desc: Test SecondRenderVisitorTest.CheckSkipAndUpdateForegroundSurfaceRenderNode while parent tree is dirty
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(SecondRenderVisitorTest, CheckSkipAndUpdateForegroundSurfaceRenderNode005, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetParentTreeDirty(true);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    auto isQuickSkip = rsUniRenderVisitor->CheckSkipAndUpdateForegroundSurfaceRenderNode(*surfaceNode);
    ASSERT_TRUE(isQuickSkip);
}

/*
 * @tc.name: CheckQuickSkipSurfaceRenderNode002
 * @tc.desc: Test SecondRenderVisitorTest.CheckQuickSkipSurfaceRenderNode while surface node is dirty
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(SecondRenderVisitorTest, CheckQuickSkipSurfaceRenderNode002, TestSize.Level2)
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
 * @tc.name: CheckQuickSkipSurfaceRenderNode001
 * @tc.desc: Test SecondRenderVisitorTest.CheckQuickSkipSurfaceRenderNode while surface node is not app window or has
 * sub surface node
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(SecondRenderVisitorTest, CheckQuickSkipSurfaceRenderNode001, TestSize.Level2)
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
 * @tc.name: CheckQuickSkipSurfaceRenderNode003
 * @tc.desc: Test SecondRenderVisitorTest.CheckQuickSkipSurfaceRenderNode while is force prepare
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(SecondRenderVisitorTest, CheckQuickSkipSurfaceRenderNode003, TestSize.Level2)
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
 * @tc.name: CheckQuickSkipSurfaceRenderNode005
 * @tc.desc: Test SecondRenderVisitorTest.CheckQuickSkipSurfaceRenderNode while check foreground surface node
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(SecondRenderVisitorTest, CheckQuickSkipSurfaceRenderNode005, TestSize.Level2)
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
 * @tc.name: CheckQuickSkipSurfaceRenderNode004
 * @tc.desc: Test SecondRenderVisitorTest.CheckQuickSkipSurfaceRenderNode while background surface node quick skip
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(SecondRenderVisitorTest, CheckQuickSkipSurfaceRenderNode004, TestSize.Level2)
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
 * @tc.name: CheckQuickSkipSurfaceRenderNode006
 * @tc.desc: Test SecondRenderVisitorTest.CheckQuickSkipSurfaceRenderNode while surfaceWindowType_ is DEFAULT_WINDOW
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(SecondRenderVisitorTest, CheckQuickSkipSurfaceRenderNode006, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    auto isQuickSkip = rsUniRenderVisitor->CheckQuickSkipSurfaceRenderNode(*surfaceNode);
    ASSERT_TRUE(!isQuickSkip);
}

/*
 * @tc.name: CheckQuickSkipSurfaceRenderNode008
 * @tc.desc: Test SecondRenderVisitorTest.CheckQuickSkipSurfaceRenderNode while surfaceWindowType_ is SCB_DROPDOWN_PANEL
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(SecondRenderVisitorTest, CheckQuickSkipSurfaceRenderNode008, TestSize.Level2)
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
 * @tc.name: CheckQuickSkipSurfaceRenderNode007
 * @tc.desc: Test SecondRenderVisitorTest.CheckQuickSkipSurfaceRenderNode while surfaceWindowType_ is SCB_DESKTOP
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(SecondRenderVisitorTest, CheckQuickSkipSurfaceRenderNode007, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    const_cast<SurfaceWindowType&>(surfaceNode->surfaceWindowType_) = SurfaceWindowType::SCB_DESKTOP;

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    auto isQuickSkip = rsUniRenderVisitor->CheckQuickSkipSurfaceRenderNode(*surfaceNode);
    ASSERT_TRUE(!isQuickSkip);
}
} // namespace OHOS::Rosen