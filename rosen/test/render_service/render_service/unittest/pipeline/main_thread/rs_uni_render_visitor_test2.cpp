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
#include "property/rs_point_light_manager.h"
#include "screen_manager/rs_screen.h"
#include "feature/occlusion_culling/rs_occlusion_handler.h"
#include "feature/opinc/rs_opinc_manager.h"
#include "feature/round_corner_display/rs_round_corner_display.h"
#include "feature/round_corner_display/rs_round_corner_display_manager.h"
#include "feature/uifirst/rs_uifirst_manager.h"
#include "feature/special_layer/rs_special_layer_utils.h"
#include "feature/window_keyframe/rs_window_keyframe_render_node.h"
#include "feature_cfg/graphic_feature_param_manager.h"
#include "gmock/gmock.h"
#include "surface_buffer_impl.h"

using namespace testing;
using namespace testing::ext;

namespace {
    const OHOS::Rosen::RectI DEFAULT_RECT = {0, 80, 1000, 1000};
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
    static sptr<RSScreenManager> screenManager_;
    static std::shared_ptr<AppExecFwk::EventRunner> runner_;
    static std::shared_ptr<AppExecFwk::EventHandler> handler_;
};


sptr<RSScreenManager> RSUniRenderVisitorTest::screenManager_ = sptr<RSScreenManager>::MakeSptr();
std::shared_ptr<AppExecFwk::EventRunner> RSUniRenderVisitorTest::runner_ = nullptr;
std::shared_ptr<AppExecFwk::EventHandler> RSUniRenderVisitorTest::handler_ = nullptr;

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
    runner_ = AppExecFwk::EventRunner::Create(true);
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
    runner_->Run();
    if (screenManager_ != nullptr) {
        screenManager_->Init(handler_);
    }
}

void RSUniRenderVisitorTest::TearDownTestCase()
{
    screenManager_->preprocessor_ = nullptr;
    runner_->Stop();
}

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

/**
 * @tc.name: UpdateDrawingCacheInfoBeforeChildrenTest001
 * @tc.desc: Test UpdateDrawingCacheInfoBeforeChildren while isDrawingCacheEnabled_ is false
 * @tc.type: FUNC
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateDrawingCacheInfoBeforeChildrenTest001, TestSize.Level2)
{
    auto node = std::make_shared<RSCanvasRenderNode>(1);
    ASSERT_NE(node, nullptr);

    auto visitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(visitor, nullptr);
    visitor->UpdateDrawingCacheInfoBeforeChildren(*node);
}

/**
 * @tc.name: UpdateDrawingCacheInfoBeforeChildrenTest002
 * @tc.desc: Test UpdateDrawingCacheInfoBeforeChildren while isDrawingCacheEnabled_ is true
 * @tc.type: FUNC
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateDrawingCacheInfoBeforeChildrenTest002, TestSize.Level2)
{
    auto node = std::make_shared<RSCanvasRenderNode>(1);
    ASSERT_NE(node, nullptr);
    auto visitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(visitor, nullptr);

    visitor->isDrawingCacheEnabled_ = true;
    visitor->UpdateDrawingCacheInfoBeforeChildren(*node);
}

/**
 * @tc.name: RSUniRenderVisitorHelperMethodsTest001
 * @tc.desc: Test RSUniRenderVisitor helper methods
 * @tc.type: FUNC
 */
HWTEST_F(RSUniRenderVisitorTest, RSUniRenderVisitorHelperMethodsTest001, TestSize.Level2)
{
    auto visitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(visitor, nullptr);

    uint16_t seq1 = visitor->IncreasePrepareSeq();
    uint16_t seq2 = visitor->IncreasePrepareSeq();
    ASSERT_EQ(seq2, seq1 + 1);

    uint16_t poseSeq1 = visitor->IncreasePostPrepareSeq();
    uint16_t poseSeq2 = visitor->IncreasePostPrepareSeq();
    ASSERT_EQ(poseSeq2, poseSeq1 + 1);
}

/**
 * @tc.name: MarkHardwareForcedDisabledTest001
 * @tc.desc: Test RSUniRenderVisitor MarkHardwareForcedDisabled
 * @tc.type: FUNC
 */
HWTEST_F(RSUniRenderVisitorTest, MarkHardwareForcedDisabledTest001, TestSize.Level2)
{
    auto visitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(visitor, nullptr);

    visitor->MarkHardwareForcedDisabled();
    ASSERT_FALSE(visitor->IsHardwareComposerEnabled());
}

/**
 * @tc.name: MarkFilterInForegroundFilterAndCheckNeedForceClearCacheTest001
 * @tc.desc: Test RSUniRenderVisitor MarkFilterInForegroundFilterAndCheckNeedForceClearCache
 * @tc.type: FUNC
 */
HWTEST_F(RSUniRenderVisitorTest, MarkFilterInForegroundFilterAndCheckNeedForceClearCacheTest001, TestSize.Level2)
{
    auto visitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(visitor, nullptr);

    constexpr NodeId nodeId = 1;
    RSEffectRenderNode effectNode(nodeId);
    visitor->MarkFilterInForegroundFilterAndCheckNeedForceClearCache(effectNode);
}

/**
 * @tc.name: RegisterHpaeCallbackTest001
 * @tc.desc: Test RSUniRenderVisitor RegisterHpaeCallback
 * @tc.type: FUNC
 */
HWTEST_F(RSUniRenderVisitorTest, RegisterHpaeCallbackTest001, TestSize.Level2)
{
    auto visitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(visitor, nullptr);

    constexpr NodeId nodeId = 1;
    RSRenderNode rsRenderNode(nodeId);
    visitor->RegisterHpaeCallback(rsRenderNode);
}

/**
 * @tc.name: PrepareForCrossNodeTest002
 * @tc.desc: Test PrepareForCrossNode
 * @tc.type: FUNC
 */
HWTEST_F(RSUniRenderVisitorTest, PrepareForCrossNodeTest002, TestSize.Level1)
{
    auto node = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(node, nullptr);
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->isCrossNodeOffscreenOn_ = CrossNodeOffScreenRenderDebugType::DISABLED;
    rsUniRenderVisitor->PrepareForCrossNode(*node);
}

/**
 * @tc.name: PrepareForCrossNodeTest003
 * @tc.desc: Test PrepareForCrossNode
 * @tc.type: FUNC
 */
HWTEST_F(RSUniRenderVisitorTest, PrepareForCrossNodeTest003, TestSize.Level1)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    NodeId id = 0;
    RSDisplayNodeConfig config;
    rsUniRenderVisitor->curLogicalDisplayNode_ = std::make_shared<RSLogicalDisplayRenderNode>(id, config);

    auto node = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(node, nullptr);
    node->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);
    node->isCrossNode_ = true;

    auto rsContext = std::make_shared<RSContext>();
    rsUniRenderVisitor->curScreenNode_ = std::make_shared<RSScreenRenderNode>(id, 0, rsContext);
    ASSERT_NE(rsUniRenderVisitor->curScreenNode_, nullptr);

    rsUniRenderVisitor->curScreenNode_->SetIsFirstVisitCrossNodeDisplay(true);
    rsUniRenderVisitor->PrepareForCrossNode(*node);
}

/**
 * @tc.name: UpdateBlackListRecordTest001
 * @tc.desc: Test UpdateBlackListRecord while node is leash window
 * @tc.type: FUNC
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateBlackListRecordTest001, TestSize.Level2)
{
    auto node = RSTestUtil::CreateSurfaceNode();
    node->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);
    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(screenManager, nullptr);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->screenState_ = ScreenState::PRODUCER_SURFACE_ENABLE;
    rsUniRenderVisitor->screenManager_ = screenManager;
    rsUniRenderVisitor->hasMirrorUsedInSpecialLayer_ = true;

    rsUniRenderVisitor->UpdateBlackListRecord(*node);
    ASSERT_TRUE(screenManager->GetBlackListVirtualScreenByNode(node->GetId()).empty());
}

/*
 * @tc.name: CheckSkipAndUpdateForegroundSurfaceRenderNode007
 * @tc.desc: Test RSUniRenderVisitorTest.CheckSkipAndUpdateForegroundSurfaceRenderNode while met skip condition for
 * first time
 * @tc.type: FUNC
 */
HWTEST_F(RSUniRenderVisitorTest, CheckSkipAndUpdateForegroundSurfaceRenderNode007, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    const_cast<SurfaceWindowType&>(surfaceNode->surfaceWindowType_) = SurfaceWindowType::SCB_GESTURE_BACK;

    auto isQuickSkip = rsUniRenderVisitor->CheckSkipAndUpdateForegroundSurfaceRenderNode(*surfaceNode);
    ASSERT_TRUE(!isQuickSkip);
}

/*
 * @tc.name: CheckSkipAndUpdateForegroundSurfaceRenderNode006
 * @tc.desc: Test RSUniRenderVisitorTest.CheckSkipAndUpdateForegroundSurfaceRenderNode while met skip
 * @tc.type: FUNC
 */
HWTEST_F(RSUniRenderVisitorTest, CheckSkipAndUpdateForegroundSurfaceRenderNode006, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetStableSkipReached(true);

    auto isQuickSkip = rsUniRenderVisitor->CheckSkipAndUpdateForegroundSurfaceRenderNode(*surfaceNode);
    ASSERT_TRUE(isQuickSkip);
}

/**
 * @tc.name: CheckCurtainScreenUsingStatusChangetTest001
 * @tc.desc: Test CheckCurtainScreenUsingStatusChange
 * @tc.type: FUNC
 */
HWTEST_F(RSUniRenderVisitorTest, CheckCurtainScreenUsingStatusChangetTest001, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    ASSERT_FALSE(rsUniRenderVisitor->CheckCurtainScreenUsingStatusChange());

    RSMainThread::Instance()->isCurtainScreenUsingStatusChanged_ = true;
    ASSERT_TRUE(rsUniRenderVisitor->CheckCurtainScreenUsingStatusChange());
}

/*
 * @tc.name: CheckQuickSkipSurfaceRenderNode0012
 * @tc.desc: Test RSUniRenderVisitorTest.CheckQuickSkipSurfaceRenderNode while is force prepare
 * @tc.type: FUNC
 */
HWTEST_F(RSUniRenderVisitorTest, CheckQuickSkipSurfaceRenderNode0012, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    surfaceNode->SetForcePrepare(true);
    const_cast<SurfaceWindowType&>(surfaceNode->surfaceWindowType_) = SurfaceWindowType::SYSTEM_SCB_WINDOW;

    auto isQuickSkip = rsUniRenderVisitor->CheckQuickSkipSurfaceRenderNode(*surfaceNode);
    ASSERT_TRUE(!isQuickSkip);
}

/*
 * @tc.name: CheckQuickSkipSurfaceRenderNode0013
 * @tc.desc: Test RSUniRenderVisitorTest.CheckQuickSkipSurfaceRenderNode while background surface node quick skip
 * @tc.type: FUNC
 */
HWTEST_F(RSUniRenderVisitorTest, CheckQuickSkipSurfaceRenderNode004, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);

    rsUniRenderVisitor->isBgWindowTraversalStarted_ = true;
    RectI rect = RectI(0, 0, 500, 500);
    auto occlusionRegion = Occlusion::Region(rect);
    rsUniRenderVisitor->accumulatedOcclusionRegion_.OrSelf(occlusionRegion);
    auto rsContext = std::make_shared<RSContext>();
    rsUniRenderVisitor->curScreenNode_ = std::make_shared<RSScreenRenderNode>(1, 0, rsContext);
    rsUniRenderVisitor->curScreenNode_->screenInfo_.width = 500;
    rsUniRenderVisitor->curScreenNode_->screenInfo_.height = 500;
    auto isQuickSkip = rsUniRenderVisitor->CheckQuickSkipSurfaceRenderNode(*surfaceNode);
    ASSERT_TRUE(isQuickSkip);
}

/*
 * @tc.name: CheckQuickSkipSurfaceRenderNode0014
 * @tc.desc: Test RSUniRenderVisitorTest.CheckQuickSkipSurfaceRenderNode while check foreground surface node
 * @tc.type: FUNC
 */
HWTEST_F(RSUniRenderVisitorTest, CheckQuickSkipSurfaceRenderNode0014, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    const_cast<SurfaceWindowType&>(surfaceNode->surfaceWindowType_) = SurfaceWindowType::SYSTEM_SCB_WINDOW;
    auto isQuickSkip = rsUniRenderVisitor->CheckQuickSkipSurfaceRenderNode(*surfaceNode);
    ASSERT_FALSE(isQuickSkip);

    surfaceNode->SetStableSkipReached(true);
    isQuickSkip = rsUniRenderVisitor->CheckQuickSkipSurfaceRenderNode(*surfaceNode);
    ASSERT_TRUE(isQuickSkip);
}

/*
 * @tc.name: CheckSkipBackgroundSurfaceRenderNode007
 * @tc.desc: Test RSUniRenderVisitorTest.CheckSkipBackgroundSurfaceRenderNode without full screen occlusion
 * @tc.type: FUNC
 */
HWTEST_F(RSUniRenderVisitorTest, CheckSkipBackgroundSurfaceRenderNode007, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    RectI rect = RectI(0, 0, 200, 200);
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);

    auto occlusionRegion = Occlusion::Region(rect);
    rsUniRenderVisitor->accumulatedOcclusionRegion_.OrSelf(occlusionRegion);
    auto rsContext = std::make_shared<RSContext>();
    rsUniRenderVisitor->curScreenNode_ = std::make_shared<RSScreenRenderNode>(22, 0, rsContext);
    rsUniRenderVisitor->curScreenNode_->screenInfo_.width = 300;
    rsUniRenderVisitor->curScreenNode_->screenInfo_.height = 300;
    auto isQuickSkip = rsUniRenderVisitor->CheckSkipBackgroundSurfaceRenderNode(*surfaceNode);
    ASSERT_TRUE(!isQuickSkip);
}

/*
 * @tc.name: QuickPrepareSurfaceRenderNode009
 * @tc.desc: Test RSUniRenderVisitorTest.QuickPrepareSurfaceRenderNode while surface node has security layer
 * @tc.type: FUNC
 */
HWTEST_F(RSUniRenderVisitorTest, QuickPrepareSurfaceRenderNode009, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetSecurityLayer(true);
    surfaceNode->SetIsOnTheTree(true, surfaceNode->GetId(), surfaceNode->GetId());
    surfaceNode->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;

    NodeId id = 5;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSLogicalDisplayRenderNode>(id, config);
    ASSERT_NE(displayNode, nullptr);
    displayNode->InitRenderParams();

    rsUniRenderVisitor->curLogicalDisplayNode_ = displayNode;
    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;
    auto rsContext = std::make_shared<RSContext>();
    rsUniRenderVisitor->curScreenNode_ = std::make_shared<RSScreenRenderNode>(1, 0, rsContext);
    rsUniRenderVisitor->curLogicalDisplayNode_->GetMultableSpecialLayerMgr().RemoveIds(SpecialLayerType::SKIP, id);
    rsUniRenderVisitor->QuickPrepareSurfaceRenderNode(*surfaceNode);
    ASSERT_EQ(rsUniRenderVisitor->curLogicalDisplayNode_->GetSpecialLayerMgr().Find(SpecialLayerType::HAS_SKIP), false);
}

/*
 * @tc.name: QuickPrepareSurfaceRenderNode007
 * @tc.desc: Test RSUniRenderVisitorTest.QuickPrepareSurfaceRenderNode while surface node is capture window
 * @tc.type: FUNC
 */
HWTEST_F(RSUniRenderVisitorTest, QuickPrepareSurfaceRenderNode007, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetIsOnTheTree(true, surfaceNode->GetId(), surfaceNode->GetId());
    surfaceNode->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    surfaceNode->name_ = CAPTURE_WINDOW_NAME;

    NodeId id = 10;
    auto rsContext = std::make_shared<RSContext>();
    auto displayNode = std::make_shared<RSScreenRenderNode>(id, 0, rsContext);
    ASSERT_NE(displayNode, nullptr);
    displayNode->AddChild(surfaceNode, 0);

    RSDisplayNodeConfig config;
    auto logicalDisplayNode = std::make_shared<RSLogicalDisplayRenderNode>(id, config);
    logicalDisplayNode->InitRenderParams();

    rsUniRenderVisitor->curScreenNode_ = displayNode;
    rsUniRenderVisitor->curLogicalDisplayNode_ = logicalDisplayNode;
    rsUniRenderVisitor->QuickPrepareSurfaceRenderNode(*surfaceNode);
    ASSERT_NE(rsUniRenderVisitor->curScreenNode_, nullptr);
}

/**
 * @tc.name: UpdateHwcNodeDirtyRegionAndCreateLayerTest005
 * @tc.desc: Test UpdateHwcNodeDirtyRegionAndCreateLayer for toplayer
 * @tc.type: FUNC
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeDirtyRegionAndCreateLayerTest005, TestSize.Level1)
{
    auto childNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    childNode->SetIsOnTheTree(true);
    childNode->SetLayerTop(true);
    node->AddChildHardwareEnabledNode(childNode);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    rsUniRenderVisitor->curScreenNode_ = std::make_shared<RSScreenRenderNode>(5, 0, rsContext->weak_from_this());
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

/*
 * @tc.name: QuickPrepareSurfaceRenderNode008
 * @tc.desc: Test RSUniRenderVisitorTest.QuickPrepareSurfaceRenderNode while surface node has skip layer
 * @tc.type: FUNC
 */
HWTEST_F(RSUniRenderVisitorTest, QuickPrepareSurfaceRenderNode008, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);

    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetSkipLayer(true);
    surfaceNode->SetIsOnTheTree(true, surfaceNode->GetId(), surfaceNode->GetId());
    surfaceNode->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;

    NodeId id = 22;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSLogicalDisplayRenderNode>(id, config);
    ASSERT_NE(displayNode, nullptr);
    displayNode->InitRenderParams();
    displayNode->AddChild(surfaceNode, 0);

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
 * @tc.name: CheckMergeDisplayDirtyByTransparent002
 * @tc.desc: Test CheckMergeDisplayDirtyByTransparent with transparent node
 * @tc.type: FUNC
 */
HWTEST_F(RSUniRenderVisitorTest, CheckMergeDisplayDirtyByTransparent002, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    config.id = 20; // 20 non-zero dummy configId
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    ASSERT_NE(rsSurfaceRenderNode->GetDirtyManager(), nullptr);
    rsSurfaceRenderNode->InitRenderParams();

    // 21 non-zero node id
    auto rsDisplayRenderNode = std::make_shared<RSScreenRenderNode>(21, 0, rsContext->weak_from_this());
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
 * @tc.name: CheckSkipBackgroundSurfaceRenderNode008
 * @tc.desc: Test RSUniRenderVisitorTest.CheckSkipBackgroundSurfaceRenderNode while full screen occlusion and visible
 * region is empty
 * @tc.type: FUNC
 */
HWTEST_F(RSUniRenderVisitorTest, CheckSkipBackgroundSurfaceRenderNode008, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    RectI rect = RectI(0, 0, 400, 400);

    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    auto occlusionRegion = Occlusion::Region(rect);
    rsUniRenderVisitor->accumulatedOcclusionRegion_.OrSelf(occlusionRegion);
    auto rsContext = std::make_shared<RSContext>();
    rsUniRenderVisitor->curScreenNode_ = std::make_shared<RSScreenRenderNode>(21, 0, rsContext);
    rsUniRenderVisitor->curScreenNode_->screenInfo_.width = 400;
    rsUniRenderVisitor->curScreenNode_->screenInfo_.height = 400;
    auto isQuickSkip = rsUniRenderVisitor->CheckSkipBackgroundSurfaceRenderNode(*surfaceNode);
    ASSERT_TRUE(isQuickSkip);
}

/**
 * @tc.name: CheckColorSpaceWithSelfDrawingNode002
 * @tc.desc: Test RSUniRenderVisitorTest.CheckColorSpaceWithSelfDrawingNode while node is not on the tree
 * @tc.type: FUNC
 */
HWTEST_F(RSUniRenderVisitorTest, CheckColorSpaceWithSelfDrawingNode002, TestSize.Level2)
{
    auto selfDrawingNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(selfDrawingNode, nullptr);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->CheckColorSpaceWithSelfDrawingNode(*selfDrawingNode);
}

/**
 * @tc.name: CheckColorSpaceWithSelfDrawingNode003
 * @tc.desc: Test RSUniRenderVisitorTest.CheckColorSpaceWithSelfDrawingNode while node is not hardware-forced disabled
 * @tc.type: FUNC
 */
HWTEST_F(RSUniRenderVisitorTest, CheckColorSpaceWithSelfDrawingNode003, TestSize.Level2)
{
    auto selfDrawingNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(selfDrawingNode, nullptr);
    selfDrawingNode->SetIsOnTheTree(true);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->CheckColorSpaceWithSelfDrawingNode(*selfDrawingNode);
}

/*
 * @tc.name: UpdateColorSpaceAfterHwcCalc_003
 * @tc.desc: Test UpdateColorSpaceAfterHwcCalc when selfDrawingNode is null.
 * @tc.type: FUNC
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateColorSpaceAfterHwcCalc_003, TestSize.Level2)
{
    std::shared_ptr<RSSurfaceRenderNode> selfDrawingNode = nullptr;
    RSMainThread::Instance()->selfDrawingNodes_.emplace_back(selfDrawingNode);

    NodeId id = 0;
    auto rsContext = std::make_shared<RSContext>();
    auto displayNode = std::make_shared<RSScreenRenderNode>(id, 0, rsContext);
    ASSERT_NE(displayNode, nullptr);

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    rsUniRenderVisitor->UpdateColorSpaceAfterHwcCalc(*displayNode);
}
} // namespace OHOS::Rosen
#endif // RS_ENABLE_UNI_RENDER