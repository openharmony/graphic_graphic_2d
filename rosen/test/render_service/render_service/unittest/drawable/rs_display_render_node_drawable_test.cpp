/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <parameters.h>

#include "feature/uifirst/rs_uifirst_manager.h"
#include "gtest/gtest.h"

#include "drawable/rs_logical_display_render_node_drawable.h"
#include "drawable/rs_screen_render_node_drawable.h"
#include "params/rs_logical_display_render_params.h"
#include "params/rs_render_thread_params.h"
#include "params/rs_screen_render_params.h"
#include "pipeline/hardware_thread/rs_hardware_thread.h"
#include "pipeline/render_thread/rs_render_engine.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "pipeline/render_thread/rs_uni_render_util.h"
#include "pipeline/render_thread/rs_uni_render_virtual_processor.h"
#include "pipeline/rs_screen_render_node.h"
#include "platform/drawing/rs_surface_converter.h"
#include "render/rs_pixel_map_util.h"
#include "screen_manager/rs_screen.h"
// xml parser
#include "feature/anco_manager/rs_anco_manager.h"
#include "feature/hpae/rs_hpae_manager.h"
#include "graphic_feature_param_manager.h"
using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::DrawableV2;

namespace OHOS::Rosen {
namespace {
constexpr int32_t DEFAULT_CANVAS_SIZE = 100;
constexpr NodeId DEFAULT_ID = 0xFFFF;
constexpr ScreenId DEFAULT_SCREEN_ID = 0xFFFF;
constexpr NodeId DEFAULT_SURFACE_NODE_ID = 10;
constexpr NodeId DEFAULT_RENDER_NODE_ID = 11;
} // namespace
class RSScreenRenderNodeDrawableTest : public testing::Test {
public:
    std::shared_ptr<RSScreenRenderNode> renderNode_;
    std::shared_ptr<RSScreenRenderNode> mirroredNode_;
    std::shared_ptr<RSLogicalDisplayRenderNode> displayRenderNode_;
    std::shared_ptr<RSLogicalDisplayRenderNode> mirroredDisplayNode_;
    RSScreenRenderNodeDrawable* screenDrawable_ = nullptr;
    RSScreenRenderNodeDrawable* mirroredScreenDrawable_ = nullptr;
    RSLogicalDisplayRenderNodeDrawable* displayDrawable_ = nullptr;
    RSLogicalDisplayRenderNodeDrawable* mirroredDisplayDrawable_ = nullptr;

    std::shared_ptr<Drawing::Canvas> drawingCanvas_ = nullptr;
    std::shared_ptr<Drawing::Canvas> drawingCanvasForMirror_ = nullptr;

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline NodeId id = DEFAULT_ID;
};

void RSScreenRenderNodeDrawableTest::SetUpTestCase()
{
#ifdef RS_ENABLE_VK
    RsVulkanContext::SetRecyclable(false);
#endif
    // init vsync, default 60hz
    auto vsyncGenerator = sptr<impl::VSyncGenerator>::MakeSptr();
    RSMainThread::Instance()->vsyncGenerator_ = VSyncGenerator;
}

void RSScreenRenderNodeDrawableTest::TearDownTestCase() {}
void RSScreenRenderNodeDrawableTest::SetUp()
{
    id++;
    auto context = std::make_shared<RSContext>();
    renderNode_ = std::make_shared<RSScreenRenderNode>(DEFAULT_ID, DEFAULT_SCREEN_ID, context);
    mirroredNode_ = std::make_shared<RSScreenRenderNode>(DEFAULT_ID + 1, DEFAULT_SCREEN_ID + 1, context);
    RSDisplayNodeConfig config;
    displayRenderNode_ = std::make_shared<RSLogicalDisplayRenderNode>(DEFAULT_ID + 2, config);
    mirroredDisplayNode_ = std::make_shared<RSLogicalDisplayRenderNode>(DEFAULT_ID + 3, config);
    if (!renderNode_ || !mirroredNode_ || !displayRenderNode_ || !mirroredDisplayNode_) {
        RS_LOGE("RSScreenRenderNodeDrawableTest: failed to create display node.");
        return;
    }

    // init RSScreen
    auto screenManager = CreateOrGetScreenManager();
    auto output = std::make_shared<HdiOutput>(renderNode_->GetScreenId());
    auto rsScreen = std::make_shared<impl::RSScreen>(renderNode_->GetScreenId(), false, output, nullptr);
    screenManager->MockHdiScreenConnected(rsScreen);
    auto mirroredOutput = std::make_shared<HdiOutput>(mirroredNode_->GetScreenId());
    auto mirroredRsScreen = std::make_shared<impl::RSScreen>(mirroredNode_->GetScreenId(), false, output, nullptr);
    screenManager->MockHdiScreenConnected(mirroredRsScreen);

    renderNode_->AddChild(displayRenderNode_);
    mirroredNode_->AddChild(mirroredDisplayNode_);
    renderNode_->InitRenderParams();
    mirroredNode_->InitRenderParams();
    displayRenderNode_->InitRenderParams();
    mirroredDisplayNode_->InitRenderParams();

    screenDrawable_ = std::static_pointer_cast<RSScreenRenderNodeDrawable>(renderNode_->GetRenderDrawable()).get();
    mirroredScreenDrawable_ =
        std::static_pointer_cast<RSScreenRenderNodeDrawable>(mirroredNode_->GetRenderDrawable()).get();
    displayDrawable_ =
        std::static_pointer_cast<RSLogicalDisplayRenderNodeDrawable>(displayRenderNode_->GetRenderDrawable()).get();
    mirroredDisplayDrawable_ =
        std::static_pointer_cast<RSLogicalDisplayRenderNodeDrawable>(mirroredDisplayNode_->GetRenderDrawable()).get();
    if (!screenDrawable_ ||！mirroredScreenDrawable_ || !displayDrawable_ || !mirroredDisplayDrawable_) {
        RS_LOGE("RSScreenRenderNodeDrawableTest: failed to create drawable.");
        return;
    }
    screenDrawable_->renderParams = std::make_unique<RSScreenRenderParams>(id);
    mirroredScreenDrawable_->renderParams = std::make_unique<RSScreenRenderParams>(id + 1);
    displayDrawable_->renderParams = std::make_unique<RSLogicalDisplayRenderParams>(id + 2);
    mirroredDisplayDrawable_->renderParams = std::make_unique<RSLogicalDisplayRenderParams>(id + 3);

    auto params = static_cast<RSScreenRenderParams*>(screenDrawable_->GetRenderParams().get());
    params->mirrorSourceDrawable_ = mirroredNode_->GetRenderDrawable();
    params->childDisplayCount_ = 1;
    ScreenInfo screenInfo;
    screenInfo.id = renderNode_->GetScreenId();
    params->screenInfo_ = screenInfo;

    auto mirroredParams = static_cast<RSScreenRenderParams*>(mirroredScreenDrawable_->GetRenderParams().get());
    mirroredParams->childDisplayCount_ = 1;
    ScreenInfo mirroredScreenInfo;
    mirroredScreenInfo.id = mirroredNode_->GetScreenId();
    mirroredParams->screenInfo_ = mirroredScreenInfo;

    // generate canvas for screenDrawable_
    drawingCanvas_ = std::make_unique<Drawing::Canvas>(DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE);
    if (drawingCanvas_) {
        screenDrawable_->curCanvas_ = std::make_shared<RSPaintFilterCanvas>(drawingCanvas_.get());
    }
    // generate canvas for mirroredScreenDrawable_
    drawingCanvasForMirror_ = std::make_unique<Drawing::Canvas>(DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE);
    if (drawingCanvasForMirror_) {
        mirroredScreenDrawable_->curCanvas_ = std::make_shared<RSPaintFilterCanvas>(drawingCanvasForMirror_.get());
    }
}
void RSScreenRenderNodeDrawableTest::TearDown() {}

/**
 * @tc.name: CreateDisplayRenderNodeDrawableTest
 * @tc.desc: Test If DisplayRenderNodeDrawable Can Be Created
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, CreateDisplayRenderNodeDrawable, TestSize.Level1)
{
    NodeId id = 1;
    auto context = std::make_shared<RSContext>();
    auto displayNode = std::make_shared<RSScreenRenderNode>(id, 0, context);
    auto drawable = RSScreenRenderNodeDrawable::OnGenerate(displayNode);
    ASSERT_NE(drawable, nullptr);
}

/**
 * @tc.name: PrepareOffscreenRender001
 * @tc.desc: Test PrepareOffscreenRender, if offscreenWidth/offscreenHeight were not initialized.
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, PrepareOffscreenRender001, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);
    ASSERT_NE(renderNode_, nullptr);
    screenDrawable_->PrepareOffscreenRender(*screenDrawable_);

    auto type = RotateOffScreenParam::GetRotateOffScreenscreenNodeEnable();
    RotateOffScreenParam::SetRotateOffScreenDisplayNodeEnable(true);

    auto params = static_cast<RSScreenRenderParams*>(screenDrawable_->GetRenderParams().get());

    params->frameRect_ = { 0.f, 0.f, 1.f, 0.f };
    screenDrawable_->PrepareOffscreenRender(*screenDrawable_);
    ASSERT_FALSE(params->IsRotationChanged());

    params->frameRect_ = { 0.f, 0.f, 1.f, 1.f };
    screenDrawable_->PrepareOffscreenRender(*screenDrawable_);
    ASSERT_FALSE(screenDrawable_->curCanvas_->GetSurface());

    auto surface = std::make_shared<Drawing::Surface>();
    screenDrawable_->curCanvas_->surface_ = surface.get();
    screenDrawable_->PrepareOffscreenRender(*screenDrawable_);
    ASSERT_TRUE(screenDrawable_->curCanvas_->GetSurface());
    RotateOffScreenParam::SetRotateOffScreenDisplayNodeEnable(type);
}

/**
 * @tc.name: PrepareOffscreenRender002
 * @tc.desc: Test PrepareOffscreenRender, offscreenWidth/offscreenHeight is/are correctly initialized.
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, PrepareOffscreenRender002, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);
    ASSERT_NE(renderNode_, nullptr);
    renderNode_->GetMutableRenderProperties().SetFrameWidth(DEFAULT_CANVAS_SIZE);
    renderNode_->GetMutableRenderProperties().SetFrameHeight(DEFAULT_CANVAS_SIZE);
    screenDrawable_->PrepareOffscreenRender(*screenDrawable_);
}

/**
 * @tc.name: ClearTransparentBeforeSaveLayer
 * @tc.desc: Test ClearTransparentBeforeSaveLayer, with two surface with/without param initialization
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, ClearTransparentBeforeSaveLayer, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);
    auto& rtThread = RSUniRenderThread::Instance();
    if (!rtThread.GetRSRenderThreadParams()) {
        RSRenderThreadParamsManager::Instance().renderThreadParams_ = std::make_unique<RSRenderThreadParams>();
    }
    NodeId id = 1;
    auto surfaceNode1 = std::make_shared<RSSurfaceRenderNode>(id);
    auto drawable1 = RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode1);
    id = 2;
    auto surfaceNode2 = std::make_shared<RSSurfaceRenderNode>(id);
    auto drawable2 = RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode2);
    surfaceNode2->InitRenderParams();
    rtThread.GetRSRenderThreadParams()->hardwareEnabledTypeDrawables_.push_back(std::make_tuple(1, 0, drawable1));
    rtThread.GetRSRenderThreadParams()->hardwareEnabledTypeDrawables_.push_back(std::make_tuple(1, 0, drawable2));
    ASSERT_NE(renderNode_, nullptr);
    renderNode_->GetMutableRenderProperties().SetFrameWidth(DEFAULT_CANVAS_SIZE);
    renderNode_->GetMutableRenderProperties().SetFrameHeight(DEFAULT_CANVAS_SIZE);
    screenDrawable_->ClearTransparentBeforeSaveLayer();
}

/**
 * @tc.name: DrawCurtainScreen
 * @tc.desc: Test DrawCurtainScreen
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, DrawCurtainScreen, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);
    screenDrawable_->DrawCurtainScreen();
    ASSERT_TRUE(!RSUniRenderThread::Instance().IsCurtainScreenOn());

    auto params = std::make_unique<RSRenderThreadParams>();
    params->isCurtainScreenOn_ = true;
    RSUniRenderThread::Instance().Sync(move(params));
    RSUniRenderThread::Instance().GetRSRenderThreadParams()->isCurtainScreenOn_ = true;
    screenDrawable_->DrawCurtainScreen();
    ASSERT_TRUE(RSUniRenderThread::Instance().IsCurtainScreenOn());
    RSUniRenderThread::Instance().GetRSRenderThreadParams()->isCurtainScreenOn_ = false;
}

/**
 * @tc.name: RenderOverDraw
 * @tc.desc: Test RenderOverDraw
 * @tc.type: FUNC
 * @tc.require: #IB1MHX
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, RenderOverDraw, TestSize.Level1)
{
    ASSERT_NE(renderNode_, nullptr);
    ASSERT_NE(screenDrawable_, nullptr);
    ASSERT_NE(screenDrawable_->renderParams_, nullptr);

    screenDrawable_->RenderOverDraw();
    // generate canvas for screenDrawable_
    drawingCanvas_ = std::make_unique<Drawing::Canvas>(DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE);
    if (drawingCanvas_) {
        screenDrawable_->curCanvas_ = std::make_shared<RSPaintFilterCanvas>(drawingCanvas_.get());
    }
    RSUniRenderThread::Instance().GetRSRenderThreadParams()->isOverDrawEnabled_ = true;
    screenDrawable_->RenderOverDraw();
}

/**
 * @tc.name: HardCursorCreateLayer
 * @tc.desc: Test HardCursorCreateLayer
 * @tc.type: FUNC
 * @tc.require: #IAX2SN
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, HardCursorCreateLayerTest, TestSize.Level1)
{
    ASSERT_NE(renderNode_, nullptr);
    ASSERT_NE(screenDrawable_, nullptr);
    ASSERT_NE(screenDrawable_->renderParams_, nullptr);

    auto params = static_cast<RSScreenRenderParams*>(screenDrawable_->GetRenderParams().get());
    ASSERT_NE(params, nullptr);
    auto processor = RSProcessorFactory::CreateProcessor(params->GetCompositeType());
    ASSERT_NE(processor, nullptr);

    auto result = screenDrawable_->HardCursorCreateLayer(processor);
    ASSERT_EQ(result, false);

    NodeId nodeId = 1;
    auto renderNode = std::make_shared<RSRenderNode>(nodeId);
    auto drawablePtr = RSRenderNodeDrawableAdapter::OnGenerate(renderNode);
    EXPECT_NE(drawablePtr, nullptr);
    RSUniRenderThread::Instance().GetRSRenderThreadParams()->hardCursorDrawableVec_ = { { nodeId, 0, drawablePtr } };
    result = screenDrawable_->HardCursorCreateLayer(processor);
    ASSERT_EQ(result, false);

    NodeId id = 1;
    drawablePtr->renderParams_ = std::make_unique<RSRenderParams>(id);
    result = screenDrawable_->HardCursorCreateLayer(processor);
    ASSERT_EQ(result, false);
}

/**
 * @tc.name: CheckScreenNodeSkip
 * @tc.desc: Test CheckScreenNodeSkip
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, CheckScreenNodeSkipTest, TestSize.Level1)
{
    ASSERT_NE(renderNode_, nullptr);
    ASSERT_NE(screenDrawable_, nullptr);
    ASSERT_NE(screenDrawable_->renderParams_, nullptr);

    auto params = static_cast<RSScreenRenderParams*>(screenDrawable_->GetRenderParams().get());
    ASSERT_NE(params, nullptr);
    auto processor = RSProcessorFactory::CreateProcessor(params->GetCompositeType());
    RSUniRenderThread::Instance().Sync(std::make_unique<RSRenderThreadParams>());
    auto result = screenDrawable_->CheckScreenNodeSkip(*params, processor);
    ASSERT_EQ(result, true);

    RSUniRenderThread::Instance().uniRenderEngine_ = std::make_shared<RSRenderEngine>();
    RSUniRenderThread::Instance().GetRSRenderThreadParams()->forceCommitReason_ = 1;
    result = screenDrawable_->CheckScreenNodeSkip(*params, processor);
    ASSERT_EQ(result, true);

    RSMainThread::Instance()->SetDirtyFlag(true);
    if (RSMainThread::Instance()->GetDirtyFlag()) {
        result = screenDrawable_->CheckScreenNodeSkip(*params, processor);
        ASSERT_EQ(result, false);
    }

    RSUifirstManager::Instance().hasForceUpdateNode_ = true;
    result = screenDrawable_->CheckScreenNodeSkip(*params, processor);
    ASSERT_EQ(result, false);

    params->isMainAndLeashSurfaceDirty_ = true;
    result = screenDrawable_->CheckScreenNodeSkip(*params, processor);
    ASSERT_EQ(result, false);

    screenDrawable_->syncDirtyManager_->currentFrameDirtyRegion_ = { 0, 0, 1, 1 };
    screenDrawable_->syncDirtyManager_->debugRect_ = { 1, 1, 1, 1 };
    result = screenDrawable_->CheckScreenNodeSkip(*params, processor);
    ASSERT_EQ(result, false);
    RSUniRenderThread::Instance().uniRenderEngine_ = nullptr;
    RSUniRenderThread::Instance().GetRSRenderThreadParams()->forceCommitReason_ = 0;
    RSMainThread::Instance()->isDirty_ = false;
    RSUifirstManager::Instance().hasForceUpdateNode_ = false;

    params->isHDRStatusChanged_ = true;
    result = screenDrawable_->CheckScreenNodeSkip(*params, processor);
    EXPECT_EQ(result, false);
}

/**
 * @tc.name: CheckAndUpdateFilterCacheOcclusion
 * @tc.desc: Test CheckAndUpdateFilterCacheOcclusion
 * @tc.type: FUNC
 * @tc.require: issueIAL2EA
 */
HWTEST_F(RSscreenRenderNodeDrawableTest, CheckAndUpdateFilterCacheOcclusionTest, TestSize.Level1)
{
    ASSERT_NE(renderNode_, nullptr);
    ASSERT_NE(screenDrawable_, nullptr);
    ASSERT_NE(screenDrawable_->renderParams_, nullptr);

    std::shared_ptr<RSSurfaceRenderNode> surfaceNode = std::make_shared<RSSurfaceRenderNode>(DEFAULT_SURFACE_NODE_ID);
    ASSERT_NE(surfaceNode, nullptr);

    auto surfaceDrawableAdapter = RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode);
    ASSERT_NE(surfaceDrawableAdapter, nullptr);

    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawableAdapter->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    ScreenInfo screenInfo = {};
    auto params = static_cast<RSScreenRenderParams*>(screenDrawable_->GetRenderParams().get());
    ASSERT_NE(params, nullptr);
    vector<std::shared_ptr<RSRenderNodeDrawableAdapter>> allSurfaceDrawable { surfaceDrawableAdapter };
    params->SetAllMainAndLeashSurfaceDrawables(allSurfaceDrawable);

    surfaceParams->windowInfo_.isMainWindowType_ = false;
    RSScreenRenderNodeDrawable::CheckAndUpdateFilterCacheOcclusion(*params, screenInfo);

    surfaceParams->windowInfo_.isMainWindowType_ = true;
    RSScreenRenderNodeDrawable::CheckAndUpdateFilterCacheOcclusion(*params, screenInfo);
}

/**
 * @tc.name: CheckFilterCacheFullyCovered
 * @tc.desc: Test CheckFilterCacheFullyCovered
 * @tc.type: FUNC
 * @tc.require: issueIAL2EA
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, CheckFilterCacheFullyCoveredTest, TestSize.Level1)
{
    ASSERT_NE(renderNode_, nullptr);
    ASSERT_NE(screenDrawable_, nullptr);
    ASSERT_NE(screenDrawable_->renderParams_, nullptr);

    auto surfaceParams = std::make_unique<RSSurfaceRenderParams>(DEFAULT_SURFACE_NODE_ID);
    ASSERT_NE(surfaceParams, nullptr);
    surfaceParams->visibleFilterChild_ = { DEFAULT_RENDER_NODE_ID };

    std::shared_ptr<RSRenderNode> renderNode = std::make_shared<RSRenderNode>(DEFAULT_RENDER_NODE_ID);
    ASSERT_NE(renderNode, nullptr);
    auto renderDrawableAdapter = RSRenderNodeDrawableAdapter::OnGenerate(renderNode);
    ASSERT_NE(renderDrawableAdapter, nullptr);
    renderDrawableAdapter->renderParams_ = std::make_unique<RSRenderParams>(DEFAULT_RENDER_NODE_ID);
    ASSERT_NE(renderDrawableAdapter->renderParams_, nullptr);
    RectI screenRect { 0, 0, 0, 0 };

    renderDrawableAdapter->renderParams_->SetHasBlurFilter(false);
    RSScreenRenderNodeDrawable::CheckFilterCacheFullyCovered(*surfaceParams, screenRect);

    surfaceParams->isTransparent_ = true;
    surfaceParams->isRotating_ = true;
    RSScreenRenderNodeDrawable::CheckFilterCacheFullyCovered(*surfaceParams, screenRect);

    surfaceParams->isRotating_ = false;
    RSScreenRenderNodeDrawable::CheckFilterCacheFullyCovered(*surfaceParams, screenRect);

    surfaceParams->isTransparent_ = false;
    renderDrawableAdapter->renderParams_->SetHasBlurFilter(true);
    RSScreenRenderNodeDrawable::CheckFilterCacheFullyCovered(*surfaceParams, screenRect);

    renderDrawableAdapter->renderParams_->SetEffectNodeShouldPaint(false);
    RSScreenRenderNodeDrawable::CheckFilterCacheFullyCovered(*surfaceParams, screenRect);

    renderDrawableAdapter->renderParams_->SetNodeType(RSRenderNodeType::EFFECT_NODE);
    RSScreenRenderNodeDrawable::CheckFilterCacheFullyCovered(*surfaceParams, screenRect);

    renderDrawableAdapter->renderParams_->SetHasGlobalCorner(true);
    RSScreenRenderNodeDrawable::CheckFilterCacheFullyCovered(*surfaceParams, screenRect);

    renderDrawableAdapter->renderParams_->SetGlobalAlpha(0.f);
    RSScreenRenderNodeDrawable::CheckFilterCacheFullyCovered(*surfaceParams, screenRect);

    renderDrawableAdapter->renderParams_->SetHasBlurFilter(true);
    renderDrawableAdapter->renderParams_->SetGlobalAlpha(1.f);
    renderDrawableAdapter->renderParams_->SetHasGlobalCorner(false);
    renderDrawableAdapter->renderParams_->SetNodeType(RSRenderNodeType::CANVAS_NODE);
    renderDrawableAdapter->renderParams_->SetEffectNodeShouldPaint(true);
    RSScreenRenderNodeDrawable::CheckFilterCacheFullyCovered(*surfaceParams, screenRect);
}

/**
 * @tc.name: OnDraw001
 * @tc.desc: Test OnDraw when renderParams_ is/not nullptr
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, OnDrawTest001, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);
    Drawing::Canvas canvas;
    // when renderParams_ is not nullptr default
    screenDrawable_->OnDraw(canvas);
    ASSERT_NE(screenDrawable_->GerDrawSkipType(), DrawSkipType::RENDER_PARAMS_OR_UNI_PARAMS_NULL);

    // when renderParams_ is nullptr
    screenDrawable_->renderParams_ = nullptr;
    screenDrawable_->OnDraw(canvas);
    ASSERT_EQ(screenDrawable_->GetDrawSkipType(), DrawSkipType::RENDER_PARAMS_OR_UNI_PARAMS_NULL);
}

/**
 * @tc.name: OnDrawTest002
 * @tc.desc: Test OnDrawTest002 when GetChildDisplayCount is/not 0
 * @tc.type: FUNC
 * @tc.require: issueICCV9N
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, OnDrawTest002, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);
    Drawing::Canvas canvas;
    // when GetChildDisplayCount is 1 default
    auto params = static_cast<RSScreenRenderParams*>(screenDrawable_->GetRenderParams().get());
    screenDrawable_->OnDraw(canvas);
    EXPECT_NE(screenDrawable_->GetDrawSkipType(), DrawSkipType::NO_DISPLAY_NODE);
    // when GetChildDisplayCount is 0
    params->childDisplayCount_ = 0;
    screenDrawable_->OnDraw(canvas);
    EXPECT_EQ(screenDrawable_->GetDrawSkipType(), DrawSkipType::NO_DISPLAY_NODE);
}

/**
 * @tc.name: OnDraw003
 * @tc.desc: Test OnDraw when realTid equal RSUniRenderThread::Instance().GetId() is true/false
 * @tc.type: FUNC
 * @tc.require: issueICCV9N
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, OnDrawTest003, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);
    Drawing::Canvas canvas;
    pid_t realTid = gettid();
    // when realTid == RSUniRenderThread::Instance().GetTId()
    RSUniRenderThread::Instance().tid = realTid;
    auto renderEngine = std::make_shared<RSRenderEngine>();
    auto renderContext = std::make_shared<RenderContext>();
    renderEngine->renderContext_ = renderContext;
    RSUniRenderThread::Instance().uniRenderEngine_ = renderEngine;
    screenDrawable_->OnDraw(canvas);
    RSUniRenderThread::Instance().uniRenderEngine_.reset();
    // when realTid != RSUniRenderThread::Instance().GetTId()
    RSUniRenderThread::Instance().tid_ = realTid + 1;
    screenDrawable_->OnDraw(canvas);
}

/**
 * @tc.name: OnDrawTest004
 * @tc.desc: Test OnDraw
 * @tc.type: FUNC
 * @tc.require: wz
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, OnDrawTest004, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);
    Drawing::Canvas canvas;
    // when skipFrameStrategy not equal SKIP_FRAME_BY_REFRESH_RATE
    auto params = static_cast<RSScreenRenderParams*>(screenDrawable_->GetRenderParams().get());
    ScreenInfo screenInfo1;
    screenInfo1.skipFrameStrategy = SKIP_FRAME_BY_INTERVAL;
    params->screenInfo_ = screenInfo1;
    screenDrawable_->OnDraw(canvas);

    // when skipFrameStrategy equal SKIP_FRAME_BY_REFRESH_RATE
    ScreenInfo screenInfo2;
    screenInfo2.skipFrameStrategy = SKIP_FRAME_BY_REFRESH_RATE;
    params->screenInfo_ = screenInfo2;
    screenDrawable_->OnDraw(canvas);

    // when curScreenInfo.isEqualVsyncPeriod not equal isEqualVsyncPeriod
    ScreenInfo screenInfo3;
    screenInfo3.skipFrameStrategy = SKIP_FRAME_BY_REFRESH_RATE;
    screenInfo3.isEqualVsyncPeriod = true;
    auto vsyncRefreshRate = RSMainThread::Instance()->GetVsyncRefreshRate();
    screenInfo3.expectedRefreshRate = vsyncRefreshRate;
    params->screenInfo_ = screenInfo3;
    screenDrawable_->OnDraw(canvas);

    // when curScreenInfo.isEqualVsyncPeriod equal isEqualVsyncPeriod
    ScreenInfo screenInfo4;
    screenInfo4.skipFrameStrategy = SKIP_FRAME_BY_REFRESH_RATE;
    screenInfo4.isEqualVsyncPeriod = false;
    screenInfo4.expectedRefreshRate = vsyncRefreshRate;
    params->screenInfo_ = screenInfo4;
    screenDrawable_->OnDraw(canvas);
}

/**
 * @tc.name: OnDraw005
 * @tc.desc: Test OnDraw when isVirtualExpandCompositeis true/false
 * @tc.type: FUNC
 * @tc.require: issueICCV9N
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, OnDrawTest005, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);
    Drawing::Canvas canvas;
    auto params = static_cast<RSScreenRenderParams*>(screenDrawable_->GetRenderParams().get());
    // when isVirtualExpandComposite is false
    params->compositeType_ = CompositeType::UNI_RENDER_COMPOSITE;
    screenDrawable_->OnDraw(canvas);
    // when isVirtualExpandComposite is true
    params->compositeType_ = CompositeType::UNI_RENDER_EXPAND_COMPOSITE;
    screenDrawable_->OnDraw(canvas);
    params->isAccumulatedDirty_ = true;
    screenDrawable_->OnDraw(canvas);
}

/**
 * @tc.name: OnDraw006
 * @tc.desc: Test OnDraw when SkipFrame is true/false
 * @tc.type: FUNC
 * @tc.require: issueICCV9N
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, OnDrawTest006, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);
    Drawing::Canvas canvas;
    auto params = static_cast<RSScreenRenderParams*>(screenDrawable_->GetRenderParams().get());
    ScreenInfo screenInfo;
    screenInfo.skipFrameStrategy = SKIP_FRAME_BY_INTERVAL;
    auto currentTime1 =
        std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch())
            .count();
    screenDrawable_->lastRefreshTime_ = currentTime1;
    screenInfo.skipFrameInterval = 1;
    params->screenInfo_ = screenInfo;
    screenDrawable_->OnDraw(canvas);
    EXPECT_FALSE(screenDrawable_->SkipFrameByInterval(
        RSMainThread::Instance()->GetVsyncRefreshRate(), screenInfo.skipFrameInterval));

    auto currentTime2 =
        std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch())
            .count();
    screenDrawable_->lastRefreshTime_ = currentTime2;
    screenInfo.skipFrameInterval = 100;
    params->screenInfo_ = screenInfo;
    screenDrawable_->OnDraw(canvas);
    EXPECT_TRUE(screenDrawable_->SkipFrameByInterval(
        RSMainThread::Instance()->GetVsyncRefreshRate(), screenInfo.skipFrameInterval));
}

/**
 * @tc.name: OnDraw007
 * @tc.desc: Test OnDraw when SkipFrame is true/false
 * @tc.type: FUNC
 * @tc.require: issueICCV9N
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, OnDrawTest007, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);
    Drawing::Canvas canvas;
    auto params = static_cast<RSScreenRenderParams*>(screenDrawable_->GetRenderParams().get());
    ScreenInfo screenInfo;
    // when isEqualVsyncPeriod is false;
    screenInfo.isEqualVsyncPeriod = false;
    params->screenInfo_ = screenInfo;
    screenDrawable_->OnDraw(canvas);
    EXPECT_FALSE(params->GetScreenInfo().isEqualVsyncPeriod);
    // when isEqualVsyncPeriod is true;
    screenInfo.isEqualVsyncPeriod = true;
    params->screenInfo_ = screenInfo;
    screenDrawable_->OnDraw(canvas);
    EXPECT_TRUE(params->GetScreenInfo().isEqualVsyncPeriod);
}

/**
 * @tc.name: OnDraw008
 * @tc.desc: Test OnDraw when SkipFrame is true/false
 * @tc.type: FUNC
 * @tc.require: issueICCV9N
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, OnDrawTest008, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);
    Drawing::Canvas canvas;
    auto params = static_cast<RSScreenRenderParams*>(screenDrawable_->GetRenderParams().get());
    params->mirrorSourceDrawable_.reset();
    params->compositeType_ = CompositeType::UNI_RENDER_COMPOSITE;
    screenDrawable_->OnDraw(canvas);
}

/**
 * @tc.name: OnDraw009
 * @tc.desc: Test OnDraw when SkipFrame is true/false
 * @tc.type: FUNC
 * @tc.require: issueICCV9N
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, OnDrawTest009, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);
    Drawing::Canvas canvas;
    auto params = static_cast<RSScreenRenderParams*>(screenDrawable_->GetRenderParams().get());
    params->mirrorSourceDrawable_.reset();
    params->compositeType_ = CompositeType::UNI_RENDER_COMPOSITE;

    screenDrawable_->OnDraw(canvas);
    EXPECT_EQ(screenDrawable_->drawSkipType_, DrawSkipType::RENDER_ENGINE_NULL);

    auto renderEngine = std::make_shared<RSRenderEngine>();
    RSUniRenderThread::Instance().uniRenderEngine_ = renderEngine;
    RSUniRenderThread::Instance().uniRenderEngine_->Init();
    screenDrawable_->OnDraw(canvas);
    EXPECT_NE(screenDrawable_->drawSkipType_, DrawSkipType::RENDER_ENGINE_NULL);
    EXPECT_EQ(screenDrawable_->drawSkipType_, DrawSkipType::REQUEST_FRAME_FAIL);
    // when enableVisibleRect is true;
    auto screenInfo = params->GetScreenInfo();
    screenInfo.enableVisibleRect = true;
    params->screenInfo_ = screenInfo;

    auto screenManager = CreateOrGetScreenManager();
    const Rect& visibleRect = { 1, 1, 1, 1 };
    screenManager->SetMirrorScreenVisibleRect(screenInfo.id, visibleRect);
    screenDrawable_->OnDraw(canvas);
    EXPECT_EQ(RSUnirenderThread::Instance().GetVisibleRect().left_, visibleRect.x);
    EXPECT_EQ(screenDrawable_->drawSkipType_, DrawSkipType::REQUEST_FRAME_FAIL);
    // when comositeType is not UNI_RENDER_MIRROR_COMPOSITE
    params->compositeType_ = CompositeType::UNI_RENDER_MIRROR_COMPOSITE;
    screenDrawable_->OnDraw(canvas);
}

/**
 * @tc.name: OnDraw010
 * @tc.desc: Test OnDraw when SkipFrame is true/false
 * @tc.type: FUNC
 * @tc.require: issueICCV9N
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, OnDrawTest010, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);
    Drawing::Canvas canvas;
    auto params = static_cast<RSScreenRenderParams*>(screenDrawable_->GetRenderParams().get());
    params->mirrorSourceDrawable_.reset();
    params->compositeType_ = CompositeType::UNI_RENDER_EXPAND_COMPOSITE;
    params->isAccumulatedDirty_ = true;

    auto renderEngine = std::make_shared<RSRenderEngine>();
    RSUniRenderThread::Instance().uniRenderEngine_ = renderEngine;
    RSUniRenderThread::Instance().uniRenderEngine_->Init();
    EXPECT_EQ(params->GetMirrorSourceDrawable().lock(), nullptr);
    EXPECT_EQ(params->GetCompositeType(), CompositeType::UNI_RENDER_EXPAND_COMPOSITE);

    screenDrawable_->OnDraw(canvas);
    EXPECT_EQ(screenDrawable_->cacheImgForCapture_, nullptr);
    RSSurfaceRenderNodeConfig config;
    config.id = id;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(config);
    surfaceNode->InitRenderParams();
    ASSERT_NE(surfaceNode, nullptr);

    auto surfaceDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(surfaceNode->GetRenderDrawable());
    ASSERT_NE(surfaceDrawable, nullptr);
    params->targetSurfaceRenderNodeDrawable_ = surfaceDrawable;
    screenDrawable_->OnDraw(canvas);
    EXPECT_EQ(screenDrawable_->cacheImgForCapture_, nullptr);
    // when hasMirrorScreen is true
    params->targetSurfaceRenderNodeDrawable_.reset();
    params->hasMirrorScreen_ = true;
    screenDrawable_->OnDraw(canvas);
    EXPECT_EQ(screenDrawable_->cacheImgForCapture_, nullptr);
}

/**
 * @tc.name: OnDraw011
 * @tc.desc: Test OnDraw when SkipFrame is true/false
 * @tc.type: FUNC
 * @tc.require: issueICCV9N
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, OnDrawTest011, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);
    Drawing::Canvas canvas;
    auto params = static_cast<RSScreenRenderParams*>(screenDrawable_->GetRenderParams().get());
    params->compositeType_ = CompositeType::UNI_RENDER_EXPAND_COMPOSITE;
    screenDrawable_->OnDraw(canvas);
}

/**
 * @tc.name: OnDraw012
 * @tc.desc: Test OnDraw when SkipFrame is true/false
 * @tc.type: FUNC
 * @tc.require: issueICCV9N
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, OnDrawTest012, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);
    Drawing::Canvas canvas;
    auto params = static_cast<RSScreenRenderParams*>(screenDrawable_->GetRenderParams().get());
    params->mirrorSourceDrawable_.reset();
    EXPECT_EQ(params->GetMirrorSourceDrawable().lock(), nullptr);
    params->SetHDRPresent(true);
    screenDrawable_->OnDraw(canvas);
    EXPECT_TRUE(params->GetHDRPresent());
    params->SetHDRPresent(false);
    screenDrawable_->OnDraw(canvas);
    EXPECT_FALSE(params->GetHDRPresent());
}

/**
 * @tc.name: OnDraw013
 * @tc.desc: Test OnDraw when SkipFrame is true/false
 * @tc.type: FUNC
 * @tc.require: issueICCV9N
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, OnDrawTest013, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);
    Drawing::Canvas canvas;
    auto params = static_cast<RSScreenRenderParams*>(screenDrawable_->GetRenderParams().get());
    params->mirrorSourceDrawable_.reset();
    EXPECT_EQ(params->GetMirrorSourceDrawable().lock(), nullptr);

    screenDrawable_->OnDraw(canvas);
    EXPECT_EQ(screenDrawable_->drawSkipType_, DrawSkipType::REQUEST_FRAME_FAIL);

    auto renderEngine = std::make_shared<RSRenderEngine>();
    auto renderContext = std::make_shared<RenderContext>();
    renderEngine->renderContext_ = renderContext;
    RSUniRenderThread::Instance().uniRenderEngine_ = renderEngine;
    RSUniRenderThread::Instance().uniRenderEngine_->Init();
    screenDrawable_->OnDraw(canvas);
    EXPECT_EQ(screenDrawable_->drawSkipType_, DrawSkipType::REQUEST_FRAME_FAIL);
}

/**
 * @tc.name: CalculateVirtualDirty
 * @tc.desc: Test CalculateVirtualDirty
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, CalculateVirtualDirtyTest, TestSize.Level1)
{
    ASSERT_NE(renderNode_, nullptr);
    ASSERT_NE(screenDrawable_, nullptr);
    ASSERT_NE(screenDrawable_->renderParams_, nullptr);
    ASSERT_NE(mirroredNode_, nullptr);

    auto& rtThread = RSUniRenderThread::Instance();
    if (!rtThread.GetRSRenderThreadParams()) {
        rtThread.Sync(std::make_unique<RSRenderThreadParams>());
    }
    screenDrawable_->PrepareOffscreenRender(*screenDrawable_);
    auto params = static_cast<RSScreenRenderParams*>(screenDrawable_->GetRenderParams().get());
    if (mirroredNode_->GetRenderDrawable() == nullptr) {
        mirroredNode_->renderDrawable_ = DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(mirroredNode_);
    }
    params->mirrorSourceDrawable_ = mirroredNode_->GetRenderDrawable();
    auto processor = RSProcessorFactory::CreateProcessor(params->GetCompositeType());
    auto virtualProcesser = std::make_shared<RSUniRenderVirtualProcessor>();
    Drawing::Matrix matrix;
    sleep(1);
}

/**
 * @tc.name: ClearCanvasStencilTest001
 * @tc.desc: Test ClearCanvasStencilTest
 * @tc.type: FUNC
 * @tc.require: #IBO35Y
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, ClearCanvasStencilTest001, TestSize.Level1)
{
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);

    auto params = static_cast<RSScreenRenderParams*>(screenDrawable_->GetRenderParams().get());
    ASSERT_NE(params, nullptr);

    auto uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams().get();
    ASSERT_NE(uniParam, nullptr);

    ASSERT_NE(screenDrawable_, nullptr);
    screenDrawable_->ClearCanvasStencil(canvas, *params, *uniParam);

    screenDrawable_->syncDirtyManager_ = std::make_shared<RSDirtyRegionManager>(false);
    ASSERT_NE(screenDrawable_->syncDirtyManager_, nullptr);
    auto dirtyManager = screenDrawable_->GetSyncDirtyManager();
    ASSERT_NE(dirtyManager, nullptr);
    Occlusion::Region allDirtyRegion { Occlusion::Rect { dirtyManager->GetDirtyRegion() } };
    auto alignedRegion = allDirtyRegion.GetAlignedRegion(128);
    ASSERT_TRUE(alignedRegion.IsEmpty());

    uniparam->isStencilPixelOcclusionCullingEnabled_ = true;
    screenDrawable_->ClearCanvasStencil(canvas, *params, *uniParam);
    ASSERT_TRUE(params->topSurfaceOpaqueRects_.empty());
    params->topSurfaceOpaqueRects_.emplace_back(Occlusion::Rect { dirtyManager->GetDirtyRegion() });
    screenDrawable_->ClearCanvasStencil(canvas, *params, *uniParam);
    ASSERT_FALSE(params->topSurfaceOpaqueRects_.empty());
}

/**
 * @tc.name: SkipFrameByIntervalTest001
 * @tc.desc: test SkipFrameByInterval for refreshRate 0 and skipFrameInterval 0
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, SkipFrameByIntervalTest001, TestSize.Level1)
{
    uint32_t refreshRate = 0;
    uint32_t skipFrameInterval = 0;
    ASSERT_FALSE(screenDrawable_->SkipFrameByInterval(refreshRate, skipFrameInterval));
}

/**
 * @tc.name: SkipFrameByIntervalTest002
 * @tc.desc: test SkipFrameByInterval for skipFrameInterval 0
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, SkipFrameByIntervalTest002, TestSize.Level1)
{
    uint32_t refreshRate = 60; // 60hz
    uint32_t skipFrameInterval = 0;
    ASSERT_FALSE(screenDrawable_->SkipFrameByInterval(refreshRate, skipFrameInterval));
}

/**
 * @tc.name: SkipFrameByIntervalTest003
 * @tc.desc: test SkipFrameByInterval for skipFrameInterval 1
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, SkipFrameByIntervalTest003, TestSize.Level1)
{
    uint32_t refreshRate = 60; // 60hz
    uint32_t skipFrameInterval = 1;
    ASSERT_FALSE(screenDrawable_->SkipFrameByInterval(refreshRate, skipFrameInterval));
}

/**
 * @tc.name: SkipFrameByIntervalTest004
 * @tc.desc: test SkipFrameByInterval for time within skipFrameInterval 2
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, SkipFrameByIntervalTest004, TestSize.Level1)
{
    uint32_t refreshRate = 60;      // 60hz
    uint32_t skipFrameInterval = 2; // skipFrameInterval 2
    screenDrawable_->SkipFrameByInterval(refreshRate, skipFrameInterval);
    ASSERT_TRUE(screenDrawable_->SkipFrameByInterval(refreshRate, skipFrameInterval));
}

/**
 * @tc.name: SkipFrameByIntervalTest005
 * @tc.desc: test SkipFrameByInterval for time over skipFrameInterval 2
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, SkipFrameByIntervalTest005, TestSize.Level1)
{
    uint32_t refreshRate = 60;      // 60hz
    uint32_t skipFrameInterval = 2; // skipFrameInterval 2
    screenDrawable_->SkipFrameByInterval(refreshRate, skipFrameInterval);
    usleep(50000); // 50000us == 50ms
    ASSERT_FALSE(screenDrawable_->SkipFrameByInterval(refreshRate, skipFrameInterval));
}

/**
 * @tc.name: SkipFrameByIntervalTest006
 * @tc.desc: test SkipFrameByInterval for time within skipFrameInterval 6
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, SkipFrameByIntervalTest006, TestSize.Level1)
{
    uint32_t refreshRate = 60;      // 60hz
    uint32_t skipFrameInterval = 6; // skipFrameInterval 6
    screenDrawable_->SkipFrameByInterval(refreshRate, skipFrameInterval);
    usleep(50000); // 50000us == 50ms
    ASSERT_TRUE(screenDrawable_->SkipFrameByInterval(refreshRate, skipFrameInterval));
}

/**
 * @tc.name: SkipFrameByIntervalTest007
 * @tc.desc: test SkipFrameByInterval for time over skipFrameInterval 6
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, SkipFrameByIntervalTest007, TestSize.Level1)
{
    uint32_t refreshRate = 60;      // 60hz
    uint32_t skipFrameInterval = 6; // skipFrameInterval 6
    screenDrawable_->SkipFrameByInterval(refreshRate, skipFrameInterval);
    usleep(150000); // 150000us == 150ms
    ASSERT_FALSE(screenDrawable_->SkipFrameByInterval(refreshRate, skipFrameInterval));
}

/**
 * @tc.name: SkipFrameByRefreshRateTest001
 * @tc.desc: test SkipFrameByRefreshRate with expectedRefreshRate 0
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, SkipFrameByRefreshRateTest001, TestSize.Level1)
{
    uint32_t refreshRate = 60; // 60hz
    uint32_t expectedRefreshRate = 0;
    ASSERT_FALSE(screenDrawable_->SkipFrameByRefreshRate(refreshRate, expectedRefreshRate));
}

/**
 * @tc.name: SkipFrameByRefreshRateTest002
 * @tc.desc: test SkipFrameByRefreshRate with expectedRefreshRate UINT32_MAX
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, SkipFrameByRefreshRateTest002, TestSize.Level1)
{
    uint32_t refreshRate = 60; // 60hz
    uint32_t expectedRefreshRate = UINT32_MAX;
    ASSERT_FALSE(screenDrawable_->SkipFrameByRefreshRate(refreshRate, expectedRefreshRate));
}

/**
 * @tc.name: SkipFrameByRefreshRateTest003
 * @tc.desc: test SkipFrameByRefreshRate with expectedRefreshRate 60
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, SkipFrameByRefreshRateTest003, TestSize.Level1)
{
    uint32_t refreshRate = 60;         // 60hz
    uint32_t expectedRefreshRate = 60; // 60hz
    screenDrawable_->SkipFrameByRefreshRate(refreshRate, expectedRefreshRate);
    usleep(5000); // 5000us == 5ms
    ASSERT_FALSE(screenDrawable_->SkipFrameByRefreshRate(refreshRate, expectedRefreshRate));
}

/**
 * @tc.name: SkipFrameByRefreshRateTest004
 * @tc.desc: test SkipFrameByRefreshRate with expectedRefreshRate 60
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, SkipFrameByRefreshRateTest004, TestSize.Level1)
{
    uint32_t refreshRate = 60;         // 60hz
    uint32_t expectedRefreshRate = 60; // 60hz
    screenDrawable_->SkipFrameByRefreshRate(refreshRate, expectedRefreshRate);
    usleep(100000); // 100000us == 100ms
    ASSERT_FALSE(screenDrawable_->SkipFrameByRefreshRate(refreshRate, expectedRefreshRate));
}

/**
 * @tc.name: SkipFrameTest001
 * @tc.desc: test SkipFrame with SKIP_FRAME_BY_INTERVAL
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, SkipFrameTest001, TestSize.Level1)
{
    uint32_t refreshRate = 60; // 60hz
    ScreenInfo screenInfo;
    screenInfo.skipFrameStrategy = SKIP_FRAME_BY_INTERVAL;
    screenInfo.skipFrameInterval = 2; // skipFrameInterval 2
    screenDrawable_->SkipFrame(refreshRate, screenInfo);
    usleep(5000); // 5000us == 5ms
    ASSERT_TRUE(screenDrawable_->SkipFrame(refreshRate, screenInfo));
}

/**
 * @tc.name: SkipFrameTest002
 * @tc.desc: test SkipFrame with SKIP_FRAME_BY_REFRESH_RATE
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, SkipFrameTest002, TestSize.Level1)
{
    uint32_t refreshRate = 60; // 60hz
    ScreenInfo screenInfo;
    screenInfo.skipFrameStrategy = SKIP_FRAME_BY_REFRESH_RATE;
    screenInfo.expectedRefreshRate = 30; // expectedRefreshRate 30
    screenDrawable_->SkipFrame(refreshRate, screenInfo);
    usleep(5000); // 5000us == 5ms
    ASSERT_TRUE(screenDrawable_->SkipFrame(refreshRate, screenInfo));
}

/**
 * @tc.name: SkipFrameIrregularRefreshRateTest001
 * @tc.desc: test SkipFrame with SKIP_FRAME_BY_REFRESH_RATE
 * @tc.type:FUNC
 * @tc.require:
 * CaseDescription: 1. preSetup: refreshRate is 60hz
 *                  2. operation: test SkipFrame interface with irregular refresh rate --- 1hz
 *                  3. result: actual refresh rate is about 1hz(20% fluctuation is allowed)
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, SkipFrameIrregularRefreshRateTest001, TestSize.Level1)
{
    uint32_t refreshRate = 60;                            // 60hz
    uint32_t refreshRateInterval = 1000000 / refreshRate; // 1000000us == 1s
    ScreenInfo screenInfo;
    screenInfo.skipFrameStrategy = SKIP_FRAME_BY_REFRESH_RATE;
    screenInfo.expectedRefreshRate = 1; // expectedRefreshRate 1
    int actualRefreshRateCount = 0;
    uint64_t startTime = static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch())
            .count());
    uint64_t curTime = startTime;
    while (curTime - startTime < 100000000) {
        RSMainThread::Instance()->curTime_ = curTime;
        bool skipFrame = screenDrawable_->SkipFrame(refreshRate, screenInfo);
        if (!skipFrame) {
            actualRefreshRateCount++;
        }
        usleep(refreshRateInterval);
        curTime = static_cast<uint64_t>(
            std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch())
                .count());
    }
    ASSERT_TRUE((actualRefreshRateCount == 1 || actualRefreshRateCount == 2));
}

/**
 * @tc.name: SkipFrameIrregularRefreshRateTest002
 * @tc.desc: test SkipFrame with SKIP_FRAME_BY_REFRESH_RATE
 * @tc.type:FUNC
 * @tc.require:
 * CaseDescription: 1. preSetup: refreshRate is 60hz
 *                  2. operation: test SkipFrame interface with irregular refresh rate --- 5hz
 *                  3. result: actual refresh rate is about 5hz(20% fluctuation is allowed)
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, SkipFrameIrregularRefreshRateTest002, TestSize.Level1)
{
    uint32_t refreshRate = 60;                            // 60hz
    uint32_t refreshRateInterval = 1000000 / refreshRate; // 1000000us == 1s
    ScreenInfo screenInfo;
    screenInfo.skipFrameStrategy = SKIP_FRAME_BY_REFRESH_RATE;
    screenInfo.expectedRefreshRate = 5; // expectedRefreshRate 5
    int actualRefreshRateCount = 0;
    uint64_t startTime = static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch())
            .count());
    uint64_t curTime = startTime;
    while (curTime - startTime < 1000000000) {
        RSMainThread::Instance()->curTime_ = curTime;
        bool skipFrame = screenDrawable_->SkipFrame(refreshRate, screenInfo);
        if (!skipFrame) {
            actualRefreshRateCount++;
        }
        usleep(refreshRateInterval);
        curTime = static_cast<uint64_t>(
            std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch())
                .count());
    }
    ASSERT_LE(abs(static_cast<int32_t>(actualRefreshRateCount - screenInfo.expectedRefreshRate)),
        (screenInfo.expectedRefreshRate * 20 / 100));
}

/**
 * @tc.name: SkipFrameIrregularRefreshRateTest003
 * @tc.desc: test SkipFrame with SKIP_FRAME_BY_REFRESH_RATE
 * @tc.type:FUNC
 * @tc.require:
 * CaseDescription: 1. preSetup: refreshRate is 60hz
 *                  2. operation: test SkipFrame interface with irregular refresh rate --- 15hz
 *                  3. result: actual refresh rate is about 15hz(20% fluctuation is allowed)
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, SkipFrameIrregularRefreshRateTest003, TestSize.Level1)
{
    uint32_t refreshRate = 60;                            // 60hz
    uint32_t refreshRateInterval = 1000000 / refreshRate; // 1000000us == 1s
    ScreenInfo screenInfo;
    screenInfo.skipFrameStrategy = SKIP_FRAME_BY_REFRESH_RATE;
    screenInfo.expectedRefreshRate = 15; // expectedRefreshRate 15
    int actualRefreshRateCount = 0;
    uint64_t startTime = static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch())
            .count());
    uint64_t curTime = startTime;
    while (curTime - startTime < 1000000000) {
        RSMainThread::Instance()->curTime_ = curTime;
        bool skipFrame = screenDrawable_->SkipFrame(refreshRate, screenInfo);
        if (!skipFrame) {
            actualRefreshRateCount++;
        }
        usleep(refreshRateInterval);
        curTime = static_cast<uint64_t>(
            std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch())
                .count());
    }
    ASSERT_LE(abs(static_cast<int32_t>(actualRefreshRateCount - screenInfo.expectedRefreshRate)),
        (screenInfo.expectedRefreshRate * 20 / 100));
}

/**
 * @tc.name: SkipFrameIrregularRefreshRateTest004
 * @tc.desc: test SkipFrame with SKIP_FRAME_BY_REFRESH_RATE
 * @tc.type:FUNC
 * @tc.require:
 * CaseDescription: 1. preSetup: refreshRate is 60hz
 *                  2. operation: test SkipFrame interface with irregular refresh rate --- 20hz
 *                  3. result: actual refresh rate is about 20hz(20% fluctuation is allowed)
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, SkipFrameIrregularRefreshRateTest004, TestSize.Level1)
{
    uint32_t refreshRate = 60;                            // 60hz
    uint32_t refreshRateInterval = 1000000 / refreshRate; // 1000000us == 1s
    ScreenInfo screenInfo;
    screenInfo.skipFrameStrategy = SKIP_FRAME_BY_REFRESH_RATE;
    screenInfo.expectedRefreshRate = 20; // expectedRefreshRate 20
    int actualRefreshRateCount = 0;
    uint64_t startTime = static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch())
            .count());
    uint64_t curTime = startTime;
    while (curTime - startTime < 1000000000) {
        RSMainThread::Instance()->curTime_ = curTime;
        bool skipFrame = screenDrawable_->SkipFrame(refreshRate, screenInfo);
        if (!skipFrame) {
            actualRefreshRateCount++;
        }
        usleep(refreshRateInterval);
        curTime = static_cast<uint64_t>(
            std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch())
                .count());
    }
    ASSERT_LE(abs(static_cast<int32_t>(actualRefreshRateCount - screenInfo.expectedRefreshRate)),
        (screenInfo.expectedRefreshRate * 20 / 100));
}

/**
 * @tc.name: SkipFrameIrregularRefreshRateTest005
 * @tc.desc: test SkipFrame with SKIP_FRAME_BY_REFRESH_RATE
 * @tc.type:FUNC
 * @tc.require:
 * CaseDescription: 1. preSetup: refreshRate is 60hz
 *                  2. operation: test SkipFrame interface with irregular refresh rate --- 25hz
 *                  3. result: actual refresh rate is about 25hz(20% fluctuation is allowed)
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, SkipFrameIrregularRefreshRateTest005, TestSize.Level1)
{
    uint32_t refreshRate = 60;                            // 60hz
    uint32_t refreshRateInterval = 1000000 / refreshRate; // 1000000us == 1s
    ScreenInfo screenInfo;
    screenInfo.skipFrameStrategy = SKIP_FRAME_BY_REFRESH_RATE;
    screenInfo.expectedRefreshRate = 25; // expectedRefreshRate 25
    int actualRefreshRateCount = 0;
    uint64_t startTime = static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch())
            .count());
    uint64_t curTime = startTime;
    while (curTime - startTime < 1000000000) {
        RSMainThread::Instance()->curTime_ = curTime;
        bool skipFrame = screenDrawable_->SkipFrame(refreshRate, screenInfo);
        if (!skipFrame) {
            actualRefreshRateCount++;
        }
        usleep(refreshRateInterval);
        curTime = static_cast<uint64_t>(
            std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch())
                .count());
    }
    ASSERT_LE(abs(static_cast<int32_t>(actualRefreshRateCount - screenInfo.expectedRefreshRate)),
        (screenInfo.expectedRefreshRate * 20 / 100));
}

/**
 * @tc.name: SkipFrameIrregularRefreshRateTest006
 * @tc.desc: test SkipFrame with SKIP_FRAME_BY_REFRESH_RATE
 * @tc.type:FUNC
 * @tc.require:
 * CaseDescription: 1. preSetup: refreshRate is 60hz
 *                  2. operation: test SkipFrame interface with irregular refresh rate --- 30hz
 *                  3. result: actual refresh rate is about 30hz(20% fluctuation is allowed)
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, SkipFrameIrregularRefreshRateTest006, TestSize.Level1)
{
    uint32_t refreshRate = 60;                            // 60hz
    uint32_t refreshRateInterval = 1000000 / refreshRate; // 1000000us == 1s
    ScreenInfo screenInfo;
    screenInfo.skipFrameStrategy = SKIP_FRAME_BY_REFRESH_RATE;
    screenInfo.expectedRefreshRate = 30; // expectedRefreshRate 30
    int actualRefreshRateCount = 0;
    uint64_t startTime = static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch())
            .count());
    uint64_t curTime = startTime;
    while (curTime - startTime < 1000000000) {
        RSMainThread::Instance()->curTime_ = curTime;
        bool skipFrame = screenDrawable_->SkipFrame(refreshRate, screenInfo);
        if (!skipFrame) {
            actualRefreshRateCount++;
        }
        usleep(refreshRateInterval);
        curTime = static_cast<uint64_t>(
            std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch())
                .count());
    }
    ASSERT_LE(abs(static_cast<int32_t>(actualRefreshRateCount - screenInfo.expectedRefreshRate)),
        (screenInfo.expectedRefreshRate * 20 / 100));
}

/**
 * @tc.name: SkipFrameIrregularRefreshRateTest007
 * @tc.desc: test SkipFrame with SKIP_FRAME_BY_REFRESH_RATE
 * @tc.type:FUNC
 * @tc.require:
 * CaseDescription: 1. preSetup: refreshRate is 60hz
 *                  2. operation: test SkipFrame interface with irregular refresh rate --- 40hz
 *                  3. result: actual refresh rate is about 40hz(20% fluctuation is allowed)
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, SkipFrameIrregularRefreshRateTest007, TestSize.Level1)
{
    uint32_t refreshRate = 60;                            // 60hz
    uint32_t refreshRateInterval = 1000000 / refreshRate; // 1000000us == 1s
    ScreenInfo screenInfo;
    screenInfo.skipFrameStrategy = SKIP_FRAME_BY_REFRESH_RATE;
    screenInfo.expectedRefreshRate = 40; // expectedRefreshRate 40
    int actualRefreshRateCount = 0;
    uint64_t startTime = static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch())
            .count());
    uint64_t curTime = startTime;
    while (curTime - startTime < 1000000000) {
        RSMainThread::Instance()->curTime_ = curTime;
        bool skipFrame = screenDrawable_->SkipFrame(refreshRate, screenInfo);
        if (!skipFrame) {
            actualRefreshRateCount++;
        }
        usleep(refreshRateInterval);
        curTime = static_cast<uint64_t>(
            std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch())
                .count());
    }
    ASSERT_LE(abs(static_cast<int32_t>(actualRefreshRateCount - screenInfo.expectedRefreshRate)),
        (screenInfo.expectedRefreshRate * 20 / 100));
}

/**
 * @tc.name: SkipFrameIrregularRefreshRateTest008
 * @tc.desc: test SkipFrame with SKIP_FRAME_BY_REFRESH_RATE
 * @tc.type:FUNC
 * @tc.require:
 * CaseDescription: 1. preSetup: refreshRate is 60hz
 *                  2. operation: test SkipFrame interface with irregular refresh rate --- 45hz
 *                  3. result: actual refresh rate is about 45hz(20% fluctuation is allowed)
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, SkipFrameIrregularRefreshRateTest008, TestSize.Level1)
{
    uint32_t refreshRate = 60;                            // 60hz
    uint32_t refreshRateInterval = 1000000 / refreshRate; // 1000000us == 1s
    ScreenInfo screenInfo;
    screenInfo.skipFrameStrategy = SKIP_FRAME_BY_REFRESH_RATE;
    screenInfo.expectedRefreshRate = 45; // expectedRefreshRate 45
    int actualRefreshRateCount = 0;
    uint64_t startTime = static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch())
            .count());
    uint64_t curTime = startTime;
    while (curTime - startTime < 1000000000) {
        RSMainThread::Instance()->curTime_ = curTime;
        bool skipFrame = screenDrawable_->SkipFrame(refreshRate, screenInfo);
        if (!skipFrame) {
            actualRefreshRateCount++;
        }
        usleep(refreshRateInterval);
        curTime = static_cast<uint64_t>(
            std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch())
                .count());
    }
    ASSERT_LE(abs(static_cast<int32_t>(actualRefreshRateCount - screenInfo.expectedRefreshRate)),
        (screenInfo.expectedRefreshRate * 20 / 100));
}

/**
 * @tc.name: SkipFrameIrregularRefreshRateTest009
 * @tc.desc: test SkipFrame with SKIP_FRAME_BY_REFRESH_RATE
 * @tc.type:FUNC
 * @tc.require:
 * CaseDescription: 1. preSetup: refreshRate is 60hz
 *                  2. operation: test SkipFrame interface with irregular refresh rate --- 55hz
 *                  3. result: actual refresh rate is about 55hz(20% fluctuation is allowed)
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, SkipFrameIrregularRefreshRateTest009, TestSize.Level1)
{
    uint32_t refreshRate = 60;                            // 60hz
    uint32_t refreshRateInterval = 1000000 / refreshRate; // 1000000us == 1s
    ScreenInfo screenInfo;
    screenInfo.skipFrameStrategy = SKIP_FRAME_BY_REFRESH_RATE;
    screenInfo.expectedRefreshRate = 55; // expectedRefreshRate 55
    int actualRefreshRateCount = 0;
    uint64_t startTime = static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch())
            .count());
    uint64_t curTime = startTime;
    while (curTime - startTime < 1000000000) {
        RSMainThread::Instance()->curTime_ = curTime;
        bool skipFrame = screenDrawable_->SkipFrame(refreshRate, screenInfo);
        if (!skipFrame) {
            actualRefreshRateCount++;
        }
        usleep(refreshRateInterval);
        curTime = static_cast<uint64_t>(
            std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch())
                .count());
    }
    ASSERT_LE(abs(static_cast<int32_t>(actualRefreshRateCount - screenInfo.expectedRefreshRate)),
        (screenInfo.expectedRefreshRate * 20 / 100));
}

/**
 * @tc.name: SkipFrameIrregularRefreshRateTest010
 * @tc.desc: test SkipFrame with SKIP_FRAME_BY_REFRESH_RATE
 * @tc.type:FUNC
 * @tc.require:
 * CaseDescription: 1. preSetup: refreshRate is 60hz
 *                  2. operation: test SkipFrame interface with irregular refresh rate --- 0hz
 *                  3. result: 0hz is not allowed, the actual refresh rate will be 60hz(20% fluctuation is allowed)
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, SkipFrameIrregularRefreshRateTest010, TestSize.Level1)
{
    uint32_t refreshRate = 60;                            // 60hz
    uint32_t refreshRateInterval = 1000000 / refreshRate; // 1000000us == 1s
    ScreenInfo screenInfo;
    screenInfo.skipFrameStrategy = SKIP_FRAME_BY_REFRESH_RATE;
    screenInfo.expectedRefreshRate = 0; // expectedRefreshRate 0
    int actualRefreshRateCount = 0;
    uint64_t startTime = static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch())
            .count());
    uint64_t curTime = startTime;
    while (curTime - startTime < 1000000000) {
        RSMainThread::Instance()->curTime_ = curTime;
        bool skipFrame = screenDrawable_->SkipFrame(refreshRate, screenInfo);
        if (!skipFrame) {
            actualRefreshRateCount++;
        }
        usleep(refreshRateInterval);
        curTime = static_cast<uint64_t>(
            std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch())
                .count());
    }
    ASSERT_LE(abs(static_cast<int32_t>(actualRefreshRateCount - refreshRate)), (refreshRate * 20 / 100));
}

/**
 * @tc.name: SkipFrameIrregularRefreshRateTest011
 * @tc.desc: test SkipFrame with SKIP_FRAME_BY_REFRESH_RATE
 * @tc.type:FUNC
 * @tc.require:
 * CaseDescription: 1. preSetup: refreshRate is 60hz
 *                  2. operation: test SkipFrame interface with irregular refresh rate --- 60hz
 *                  3. result: actual refresh rate is about 60hz(20% fluctuation is allowed)
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, SkipFrameIrregularRefreshRateTest011, TestSize.Level1)
{
    uint32_t refreshRate = 60;                            // 60hz
    uint32_t refreshRateInterval = 1000000 / refreshRate; // 1000000us == 1s
    ScreenInfo screenInfo;
    screenInfo.skipFrameStrategy = SKIP_FRAME_BY_REFRESH_RATE;
    screenInfo.expectedRefreshRate = 60; // expectedRefreshRate 60
    int actualRefreshRateCount = 0;
    uint64_t startTime = static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch())
            .count());
    uint64_t curTime = startTime;
    while (curTime - startTime < 1000000000) {
        RSMainThread::Instance()->curTime_ = curTime;
        bool skipFrame = screenDrawable_->SkipFrame(refreshRate, screenInfo);
        if (!skipFrame) {
            actualRefreshRateCount++;
        }
        usleep(refreshRateInterval);
        curTime = static_cast<uint64_t>(
            std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch())
                .count());
    }
    ASSERT_LE(abs(static_cast<int32_t>(actualRefreshRateCount - refreshRate)), (refreshRate * 20 / 100));
}

/**
 * @tc.name: SkipFrameIrregularRefreshRateTest012
 * @tc.desc: test SkipFrame with SKIP_FRAME_BY_REFRESH_RATE
 * @tc.type:FUNC
 * @tc.require:
 * CaseDescription: 1. preSetup: refreshRate is 60hz
 *                  2. operation: test SkipFrame interface with irregular refresh rate --- 61hz
 *                  3. result: actual refresh rate is about 60hz(20% fluctuation is allowed)
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, SkipFrameIrregularRefreshRateTest012, TestSize.Level1)
{
    uint32_t refreshRate = 60;                            // 60hz
    uint32_t refreshRateInterval = 1000000 / refreshRate; // 1000000us == 1s
    ScreenInfo screenInfo;
    screenInfo.skipFrameStrategy = SKIP_FRAME_BY_REFRESH_RATE;
    screenInfo.expectedRefreshRate = 61; // expectedRefreshRate 61
    int actualRefreshRateCount = 0;
    uint64_t startTime = static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch())
            .count());
    uint64_t curTime = startTime;
    while (curTime - startTime < 1000000000) {
        RSMainThread::Instance()->curTime_ = curTime;
        bool skipFrame = screenDrawable_->SkipFrame(refreshRate, screenInfo);
        if (!skipFrame) {
            actualRefreshRateCount++;
        }
        usleep(refreshRateInterval);
        curTime = static_cast<uint64_t>(
            std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch())
                .count());
    }
    ASSERT_LE(abs(static_cast<int32_t>(actualRefreshRateCount - refreshRate)), (refreshRate * 20 / 100));
}

/**
 * @tc.name: SkipFrameIrregularRefreshRateTest013
 * @tc.desc: test SkipFrame with SKIP_FRAME_BY_REFRESH_RATE
 * @tc.type:FUNC
 * @tc.require:
 * CaseDescription: 1. preSetup: refreshRate is 60hz
 *                  2. operation: test SkipFrame interface with irregular refresh rate --- UINT32_MAX hz
 *                  3. result: actual refresh rate is about 60hz(20% fluctuation is allowed)
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, SkipFrameIrregularRefreshRateTest013, TestSize.Level1)
{
    uint32_t refreshRate = 60;                            // 60hz
    uint32_t refreshRateInterval = 1000000 / refreshRate; // 1000000us == 1s
    ScreenInfo screenInfo;
    screenInfo.skipFrameStrategy = SKIP_FRAME_BY_REFRESH_RATE;
    screenInfo.expectedRefreshRate = UINT32_MAX; // expectedRefreshRate UINT32_MAX
    int actualRefreshRateCount = 0;
    uint64_t startTime = static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch())
            .count());
    uint64_t curTime = startTime;
    while (curTime - startTime < 1000000000) {
        RSMainThread::Instance()->curTime_ = curTime;
        bool skipFrame = screenDrawable_->SkipFrame(refreshRate, screenInfo);
        if (!skipFrame) {
            actualRefreshRateCount++;
        }
        usleep(refreshRateInterval);
        curTime = static_cast<uint64_t>(
            std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch())
                .count());
    }
    ASSERT_LE(abs(static_cast<int32_t>(actualRefreshRateCount - refreshRate)), (refreshRate * 20 / 100));
}

/**
 * @tc.name: SkipFrameIrregularRefreshRateTest014
 * @tc.desc: test SkipFrame with SKIP_FRAME_BY_REFRESH_RATE
 * @tc.type:FUNC
 * @tc.require:
 * CaseDescription: 1. preSetup: refreshRate is 60hz
 *                  2. operation: test SkipFrame interface with irregular refresh rate --- UINT32_MAX+1 hz
 *                  3. result: actual refresh rate is about 60hz(20% fluctuation is allowed)
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, SkipFrameIrregularRefreshRateTest014, TestSize.Level1)
{
    uint32_t refreshRate = 60;                            // 60hz
    uint32_t refreshRateInterval = 1000000 / refreshRate; // 1000000us == 1s
    ScreenInfo screenInfo;
    screenInfo.skipFrameStrategy = SKIP_FRAME_BY_REFRESH_RATE;
    screenInfo.expectedRefreshRate = UINT32_MAX + 1; // expectedRefreshRate UINT32_MAX + 1
    int actualRefreshRateCount = 0;
    uint64_t startTime = static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch())
            .count());
    uint64_t curTime = startTime;
    while (curTime - startTime < 1000000000) {
        RSMainThread::Instance()->curTime_ = curTime;
        bool skipFrame = screenDrawable_->SkipFrame(refreshRate, screenInfo);
        if (!skipFrame) {
            actualRefreshRateCount++;
        }
        usleep(refreshRateInterval);
        curTime = static_cast<uint64_t>(
            std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch())
                .count());
    }
    ASSERT_LE(abs(static_cast<int32_t>(actualRefreshRateCount - refreshRate)), (refreshRate * 20 / 100));
}

/**
 * @tc.name: MakeBrightnessAdjustmentShader
 * @tc.desc: Test MakeBrightnessAdjustmentShader
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, MakeBrightnessAdjustmentShader, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);
    auto image = std::make_shared<Drawing::Image>();
    Drawing::SamplingOptions sampling;
    ASSERT_NE(screenDrawable_->MakeBrightnessAdjustmentShader(image, sampling, 0.5f), nullptr);
}

/**
 * @tc.name: FinishOffscreenRender
 * @tc.desc: Test FinishOffscreenRender
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, FinishOffscreenRender, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);
    ASSERT_NE(screenDrawable_->renderParams_, nullptr);
    Drawing::SamplingOptions sampling;
    screenDrawable_->FinishOffscreenRender(sampling);
    ASSERT_FALSE(screenDrawable_->canvasBackup_);
}

/**
 * @tc.name: CreateSurface
 * @tc.desc: Test CreateSurface
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, CreateSurface, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);
    sptr<IBufferConsumerListener> listener;
    bool res = screenDrawable_->CreateSurface(listener);
    ASSERT_FALSE(screenDrawable_->surfaceHandler_->GetConsumer());
    ASSERT_FALSE(screenDrawable_->surface_);
    ASSERT_FALSE(res);

    screenDrawable_->surface_ = nullptr;
    ASSERT_FALSE(screenDrawable_->surface_);
    res = screenDrawable_->CreateSurface(listener);
    ASSERT_FALSE(screenDrawable_->surface_);
    ASSERT_FALSE(res);

    screenDrawable_->surfaceHandler_->consumer_ = nullptr;
    ASSERT_FALSE(screenDrawable_->surfaceHandler_->GetConsumer());
    res = screenDrawable_->CreateSurface(listener);
    ASSERT_FALSE(screenDrawable_->surfaceHandler_->GetConsumer());
    ASSERT_FALSE(res);
}

/**
 * @tc.name: SkipFrameByInterval
 * @tc.desc: Test SkipFrameByInterval
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, SkipFrameByInterval, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);
    bool res = screenDrawable_->SkipFrameByInterval(0, 2);
    ASSERT_FALSE(res);
    res = screenDrawable_->SkipFrameByInterval(1, 1);
    ASSERT_FALSE(res);
    res = screenDrawable_->SkipFrameByInterval(1, 2);
    ASSERT_FALSE(res);
}

/**
 * @tc.name: EnablescRGBForP3AndUiFirstTest
 * @tc.desc: Test ScRGB For P3 Controller
 * @tc.type: FUNC
 * @tc.require: issueIAWIC7
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, EnablescRGBForP3AndUiFirstTest, TestSize.Level2)
{
    ASSERT_NE(screenDrawable_, nullptr);
    auto currentGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB;
    auto result = screenDrawable_->EnablescRGBForP3AndUiFirst(currentGamut);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: RenderOverDraw_Disabled
 * @tc.desc: Test RenderOverDraw when overdraw is disabled
 * @tc.type: FUNC
 * @tc.require: issueIAWIC7
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, RenderOverDraw_Disabled, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);
    auto& uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    if (!uniParam) {
        RSUniRenderThread::Instance().Sync(std::make_unique<RSRenderThreadParams>());
    }
    if (uniParam) {
        uniParam->isOverDrawEnabled_ = false;
    }
    screenDrawable_->RenderOverDraw();
}

/**
 * @tc.name: RenderOverDraw_NullCanvas
 * @tc.desc: Test RenderOverDraw when Canvas is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAWIC7
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, RenderOverDraw_NullCanvas, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);
    screenDrawable_->curCanvas_ = nullptr;
    screenDrawable_->RenderOverDraw();
}

/**
 * @tc.name: RenderOverDraw_Enabled
 * @tc.desc: Test RenderOverDraw when overdraw is enabled but no gpu context
 * @tc.type: FUNC
 * @tc.require: issueIAWIC7
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, RenderOverDraw_Enabled, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);
    auto& uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    if (!uniParam) {
        RSUniRenderThread::Instance().Sync(std::make_unique<RSRenderThreadParams>());
    }
    if (uniParam) {
        uniParam->isOverDrawEnabled_ = true;
    }
    screenDrawable_->RenderOverDraw();
}

/**
 * @tc.name: PostClearMemoryTask
 * @tc.desc: Test PostClearMemoryTask
 * @tc.type: FUNC
 * @tc.require: issueIAWIC7
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, PostClearMemoryTask, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);
    screenDrawable_->postClearMemoryTask();
}

/**
 * @tc.name: SetScreenNodeSkipFlag_True
 * @tc.desc: Test SetScreenNodeSkipFlag with true flag
 * @tc.type: FUNC
 * @tc.require: issueIAWIC7
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, SetScreenNodeSkipFlag_True, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);
    RSRenderThreadParams params;
    screenDrawable_->SetScreenNodeSkipFlag(params, true);
}

/**
 * @tc.name: SetScreenNodeSkipFlag_False
 * @tc.desc: Test SetScreenNodeSkipFlag with false flag
 * @tc.type: FUNC
 * @tc.require: issueIAWIC7
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, SetScreenNodeSkipFlag_False, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);
    RSRenderThreadParams params;
    screenDrawable_->SetScreenNodeSkipFlag(params, false);
}

/**
 * @tc.name: CheckFilterCacheFullyCovered_Transparent
 * @tc.desc: Test CheckFilterCacheFullyCovered with Transparent surface
 * @tc.type: FUNC
 * @tc.require: issueIAWIC7
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, CheckFilterCacheFullyCovered_Transparent, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);
    RSSurfaceRenderParams surfaceParams(DEFAULT_SURFACE_NODE_ID);
    surfaceParams.isTransparent_ = true;
    RectI screenRect(0, 0, 100, 100);
    RSScreenRenderNodeDrawable::CheckFilterCacheFullyCovered(surfaceParams, screenRect);
    EXPECT_FALSE(surfaceParams.GetFilterCacheFullyCovered());
}

/**
 * @tc.name: CheckFilterCacheFullyCovered_Opaque
 * @tc.desc: Test CheckFilterCacheFullyCovered with opaque surface
 * @tc.type: FUNC
 * @tc.require: issueIAWIC7
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, CheckFilterCacheFullyCovered_Opaque, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);
    RSSurfaceRenderParams surfaceParams(DEFAULT_SURFACE_NODE_ID);
    surfaceParams.isTransparent_ = false;
    RectI screenRect(0, 0, 100, 100);
    RSScreenRenderNodeDrawable::CheckFilterCacheFullyCovered(surfaceParams, screenRect);
    EXPECT_FALSE(surfaceParams.GetFilterCacheFullyCovered());
}

/**
 * @tc.name: CheckFilterCacheFullyCovered_Rotating
 * @tc.desc: Test CheckFilterCacheFullyCovered with Rotating surface
 * @tc.type: FUNC
 * @tc.require: issueIAWIC7
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, CheckFilterCacheFullyCovered_Rotating, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);
    RSSurfaceRenderParams surfaceParams(DEFAULT_SURFACE_NODE_ID);
    surfaceParams.isTransparent_ = true;
    surfaceParams.isRotating_ = true;
    RectI screenRect(0, 0, 100, 100);
    RSScreenRenderNodeDrawable::CheckFilterCacheFullyCovered(surfaceParams, screenRect);
    EXPECT_FALSE(surfaceParams.GetFilterCacheFullyCovered());
}

/**
 * @tc.name: CheckAndUpdateFilterCacheOcclusion
 * @tc.desc: Test CheckAndUpdateFilterCacheOcclusion
 * @tc.type: FUNC
 * @tc.require: issueIAWIC7
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, CheckAndUpdateFilterCacheOcclusion, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);
    auto params = static_cast<RSScreenRenderParams*>(screenDrawable_->GetRenderParams().get());
    ASSERT_NE(params, nullptr);
    ScreenInfo screenInfo;
    screenInfo.width = 100;
    screenInfo.height = 100;
    RSScreenRenderNodeDrawable::CheckAndUpdateFilterCacheOcclusion(*params, screenInfo);
}

/**
 * @tc.name: GetBufferAge
 * @tc.desc: Test GetBufferAge
 * @tc.type: FUNC
 * @tc.require: issueIAWIC7
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, GetBufferAge, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);
    int32_t age = screenDrawable_->GetBufferAge();
    EXPECT_EQ(age, 0);
}

/**
 * @tc.name: SetDamageRegion
 * @tc.desc: Test SetDamageRegion
 * @tc.type: FUNC
 * @tc.require: issueIAWIC7
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, SetDamageRegion, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);
    std::vector<RectI> rects = { RectI(0, 0, 10, 10), RectI(10, 10, 20, 20) };
    screenDrawable_->SetDamageRegion(rects);
}

/**
 * @tc.name: SetDamageRegion_Empty
 * @tc.desc: Test SetDamageRegion_Empty
 * @tc.type: FUNC
 * @tc.require: issueIAWIC7
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, SetDamageRegion_Empty, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);
    std::vector<RectI> rects;
    screenDrawable_->SetDamageRegion(rects);
}

/**
 * @tc.name: FinishHdrDraw
 * @tc.desc: Test FinishHdrDraw
 * @tc.type: FUNC
 * @tc.require: issueIAWIC7
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, FinishHdrDraw, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);
    Drawing::Brush brush;
    screenDrawable_->FinishHdrDraw(brush, 1.0f);
}

/**
 * @tc.name: FinishHdrDraw_DifferentRatio
 * @tc.desc: Test FinishHdrDraw with different ratio
 * @tc.type: FUNC
 * @tc.require: issueIAWIC7
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, FinishHdrDraw_DifferentRatio, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);
    Drawing::Brush brush;
    screenDrawable_->FinishHdrDraw(brush, 0.5f);
}

/**
 * @tc.name: EnablescRGBForP3AndUiFirst_SRGB
 * @tc.desc: Test EnablescRGBForP3AndUiFirst with SRGB
 * @tc.type: FUNC
 * @tc.require: issueIAWIC7
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, EnablescRGBForP3AndUiFirst_SRGB, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);
    bool result = screenDrawable_->EnablescRGBForP3AndUiFirst(GRAPHIC_COLOR_GAMUT_SRGB);
    EXPECT_EQ(result, false);
}

/**
 * @tc.name: EnablescRGBForP3AndUiFirst_P3
 * @tc.desc: Test EnablescRGBForP3AndUiFirst with P3
 * @tc.type: FUNC
 * @tc.require: issueIAWIC7
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, EnablescRGBForP3AndUiFirst_P3, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);
    bool result = screenDrawable_->EnablescRGBForP3AndUiFirst(GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
    EXPECT_EQ(result, false);
}

/**
 * @tc.name: PrepareOffscreenRender_InvalidSize
 * @tc.desc: Test PrepareOffscreenRender WITH Invalid SCREEN Size
 * @tc.type: FUNC
 * @tc.require: issueIAWIC7
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, PrepareOffscreenRender_InvalidSize, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);
    auto params = static_cast<RSScreenRenderParams*>(screenDrawable_->GetRenderParams().get());
    ASSERT_NE(params, nullptr);
    ScreenInfo& screenInfo = params->screenInfo_;
    screenInfo.width = 0;
    screenInfo.height = 0;
    screenDrawable_->PrepareOffscreenRender(*screenDrawable_, false, false);
}

/**
 * @tc.name: PrepareOffscreenRender_ValidSize
 * @tc.desc: Test PrepareOffscreenRender WITH Invalid SCREEN Size
 * @tc.type: FUNC
 * @tc.require: issueIAWIC7
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, PrepareOffscreenRender_ValidSize, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);
    auto params = static_cast<RSScreenRenderParams*>(screenDrawable_->GetRenderParams().get());
    ASSERT_NE(params, nullptr);
    ScreenInfo& screenInfo = params->screenInfo_;
    screenInfo.width = 100;
    screenInfo.height = 100;
    screenDrawable_->curCanvas_ = std::make_shared<RSPaintFilterCanvas>(drawingCanvas_.get());
    screenDrawable_->PrepareOffscreenRender(*screenDrawable_, false, false);
}

/**
 * @tc.name: PrepareOffscreenRender_NullSurface
 * @tc.desc: Test PrepareOffscreenRender when current surface is null
 * @tc.type: FUNC
 * @tc.require: issueIAWIC7
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, PrepareOffscreenRender_NullSurface, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);
    auto params = static_cast<RSScreenRenderParams*>(screenDrawable_->GetRenderParams().get());
    ASSERT_NE(params, nullptr);
    ScreenInfo& screenInfo = params->screenInfo_;
    screenInfo.width = 100;
    screenInfo.height = 100;
    auto drawingCanvas = std::make_unique<Drawing::Canvas>(100, 100);
    screenDrawable_->curCanvas_ = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    screenDrawable_->PrepareOffscreenRender(*screenDrawable_, false, false);
}

/**
 * @tc.name: MakeBrightnessAdjustmentShader001
 * @tc.desc: Test MakeBrightnessAdjustmentShader001
 * @tc.type: FUNC
 * @tc.require: issueIAWIC7
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, MakeBrightnessAdjustmentShader001, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);
    auto image = std::make_shared<Drawing::Image>();
    Drawing::SamplingOptions sampling;
    auto shader = screenDrawable_->MakeBrightnessAdjustmentShader(image, sampling, 0.8f);
}

/**
 * @tc.name: FinishOffscreenRender_NullBackup
 * @tc.desc: Test FinishOffscreenRender_NullBackup when canvasBackup_ is null
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, FinishOffscreenRender_NullBackup, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);
    Drawing::SamplingOptions sampling;
    screenDrawable_->FinishOffscreenRender(sampling, 1.0f);
    ASSERT_FALSE(screenDrawable_->canvasBackup_);
}

/**
 * @tc.name: FinishOffscreenRender_NulloffscreenSurface
 * @tc.desc: Test FinishOffscreenRender_NullBackup when offscreenSurface_ is null
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, FinishOffscreenRender_NulloffscreenSurface, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);
    screenDrawable_->canvasBackup_ = std::make_shared<RSPaintFilterCanvas>(drawingCanvas_.get());
    screenDrawable_->offscreenSurface_ = nullptr;
    Drawing::SamplingOptions sampling;
    screenDrawable_->FinishOffscreenRender(sampling, 1.0f);
    ASSERT_FALSE(screenDrawable_->canvasBackup_);
}

/**
 * @tc.name: FinishOffscreenRender_ScreenSurface
 * @tc.desc: Test FinishOffscreenRender_NullBackup when offscreenSurface_
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, FinishOffscreenRender_ScreenSurface, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);
    screenDrawable_->canvasBackup_ = std::make_shared<RSPaintFilterCanvas>(drawingCanvas_.get());
    screenDrawable_->offscreenSurface_ = std::make_shared<Drawing::Surface>();
    Drawing::SamplingOptions sampling;
    screenDrawable_->FinishOffscreenRender(sampling, 1.0f);
    ASSERT_EQ(screenDrawable_->offscreenSurface_->GetImageSnapshot(), nullptr);
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat bitmapFormat { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_OPAQUE };
    bitmap.Build(10, 10, bitmapFormat);
    ASSERT_TRUE(screenDrawable_->offscreenSurface_->Bind(bitmap));
    screenDrawable_->FinishOffscreenRender(sampling, 1.0f);
}

/**
 * @tc.name: FinishOffscreenRender_ScreenSurface
 * @tc.desc: Test FinishOffscreenRender_NullBackup when offscreenSurface_
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, SkipFrame, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);
    screenDrawable_->canvasBackup_ = std::make_shared<RSPaintFilterCanvas>(drawingCanvas_.get());
    screenDrawable_->offscreenSurface_ = std::make_shared<Drawing::Surface>();
    Drawing::SamplingOptions sampling;
    screenDrawable_->FinishOffscreenRender(sampling, 1.0f);
    ASSERT_EQ(screenDrawable_->offscreenSurface_->GetImageSnapshot(), nullptr);
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat bitmapFormat { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_OPAQUE };
    bitmap.Build(10, 10, bitmapFormat);
    ASSERT_TRUE(screenDrawable_->offscreenSurface_->Bind(bitmap));
    screenDrawable_->FinishOffscreenRender(sampling, 1.0f);
}

/**
 * @tc.name: SkipFrameByInterval_InvalidParams
 * @tc.desc: Test SkipFrameByInterval with invalid params
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, SkipFrameByInterval_InvalidParams, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);

    // test with refreshrate =0
    bool result = screenDrawable_->SkipFrameByInterval(0, 2);
    EXPECT_FALSE(result);

    // test with SkipFrameInterval =0
    result = screenDrawable_->SkipFrameByInterval(60, 0);
    EXPECT_FALSE(result);

    // test with SkipFrameInterval =1
    result = screenDrawable_->SkipFrameByInterval(60, 1);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: SkipFrameByInterval_ValidParams
 * @tc.desc: Test SkipFrameByInterval with valid params
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, SkipFrameByInterval_ValidParams, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);

    // first call should return false and set last RefreshTime_
    bool result = screenDrawable_->SkipFrameByInterval(60, 2);
    EXPECT_FALSE(result);

    // immediate second call should return true(skip)
    result = screenDrawable_->SkipFrameByInterval(60, 2);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: SkipFrameByRefreshRate_InvalidParams
 * @tc.desc: Test SkipFrameByRefreshRate with Invalid params
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, SkipFrameByRefreshRate_InvalidParams, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);

    // test with refresh rates 0
    bool result = screenDrawable_->SkipFrameByRefreshRate(0, 30);
    EXPECT_FALSE(result);

    // test with expect  refresh rates
    result = screenDrawable_->SkipFrameByRefreshRate(60, 0);
    EXPECT_FALSE(result);

    // test with same refresh rates
    result = screenDrawable_->SkipFrameByRefreshRate(60, 60);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: SkipFrameByRefreshRate_InvalidParams
 * @tc.desc: Test SkipFrameByRefreshRate with valid params
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, SkipFrameByRefreshRate_ValidParams, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);

    screenDrawable_->lastRefreshTime_ = 0;

    bool result = screenDrawable_->SkipFrameByRefreshRate(60, 30);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: SkipFrameByInterval_ValidParams
 * @tc.desc: Test SkipFrameByInterval with valid params
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, SkipFrame_ByInterval, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);

    ScreenInfo screenInfo;
    screenInfo.skipFrameStrategy = SKIP_FRAME_BY_INTERVAL;
    screenInfo.skipFrameInterval = 2;

    bool result = screenDrawable_->SkipFrame(60, screenInfo);
    EXPECT_EQ(result, false);
}

/**
 * @tc.name: SkipFrameBy_RefreshRate
 * @tc.desc: Test SkipFrameBy_RefreshRate
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, SkipFrame_ByRefreshRate, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);

    ScreenInfo screenInfo;
    screenInfo.skipFrameStrategy = SKIP_FRAME_BY_REFRESH_RATE;
    screenInfo.expectedRefreshRate = 30;

    bool result = screenDrawable_->SkipFrame(60, screenInfo);
    EXPECT_EQ(result, false);
}

/**
 * @tc.name: SkipFrameBy_DefaultStrategy
 * @tc.desc: Test SkipFrameBy_DefaultStrategy
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, SkipFrame_DefaultStrategy, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);

    ScreenInfo screenInfo;
    screenInfo.skipFrameStrategy = static_cast<SkipFrameStrategy>(999);

    bool result = screenDrawable_->SkipFrame(60, screenInfo);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: DrawCurtainScreen_Off
 * @tc.desc: Test DrawCurtainScreen_Off
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, DrawCurtainScreen_Off, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);

    // Ensure curtain screen is off
    auto& uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    if (!uniParam) {
        RSUniRenderThread::Instance().Sync(std::make_unique<RSRenderThreadParams>());
    }

    if (uniParam) {
        uniParam->isCurtainScreenOn_ = false;
    }
    screenDrawable_->DrawCurtainScreen();

    if (uniParam) {
        uniParam->isCurtainScreenOn_ = true;
    }
    screenDrawable_->DrawCurtainScreen();
}

/**
 * @tc.name: SkipFrameBy_RefreshRate
 * @tc.desc: Test SkipFrameBy_RefreshRate
 * @tc.type: FUNC
 * @tc.require: issueIAGR5V
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, DrawCurtainScreen_NullCanvas, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);

    screenDrawable_->curCanvas_ = nullptr;
    screenDrawable_->DrawCurtainScreen();

    // create cavans without surface
    auto drawingCanvas = std::make_unique<Drawing::Canvas>(100, 100);
    screenDrawable_->curCanvas_ = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    screenDrawable_->DrawCurtainScreen();
}

/**
 * @tc.name: ClearTransparentBeforeSaveLayer_NullBackup
 * @tc.desc: Test ClearTransparentBeforeSaveLayer, NullBackup
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, ClearTransparentBeforeSaveLayer_NullBackup, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);

    screenDrawable_->canvasBackup_ = nullptr;
    screenDrawable_->ClearTransparentBeforeSaveLayer();
}

/**
 * @tc.name: ClearTransparentBeforeSaveLayer_NullBackup
 * @tc.desc: Test ClearTransparentBeforeSaveLayer, NullBackup
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, ClearTransparentBeforeSaveLayer_NullRenderParams, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);

    screenDrawable_->canvasBackup_ = std::make_shared<RSPaintFilterCanvas>(drawingCanvas_.get());
    screenDrawable_->renderParams_ = nullptr;
    screenDrawable_->ClearTransparentBeforeSaveLayer();
}

/**
 * @tc.name: ClearTransparentBeforeSaveLayer_ValidParams
 * @tc.desc: Test ClearTransparentBeforeSaveLayer_ValidParams, NullBackup
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, ClearTransparentBeforeSaveLayer_ValidParams, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);

    screenDrawable_->canvasBackup_ = std::make_shared<RSPaintFilterCanvas>(drawingCanvas_.get());

    // init uniparam
    auto& uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    if (!uniParam) {
        RSUniRenderThread::Instance().Sync(std::make_unique<RSRenderThreadParams>());
    }
    screenDrawable_->ClearTransparentBeforeSaveLayer();
}

/**
 * @tc.name: RequestFrame_NullRenderEngine
 * @tc.desc: Test RequestFrame_NullRenderEngine
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, RequestFrame_NullRenderEngine, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);

    auto params = static_cast<RSScreenRenderParams*>(screenDrawable_->GetRenderParams().get());
    ASSERT_NE(params, nullptr);

    // ensure render engine is null
    RSUniRenderThread::Instance().uniRenderEngine_ = nullptr;

    auto processor = RSProcessorFactory::CreateProcessor(CompositeType::HARDWARE_COMPOSITE);
    auto renderFrame = screenDrawable_->RequestFrame(*params, processor);
    EXPECT_EQ(renderFrame, nullptr);
}

/**
 * @tc.name: CheckScreenNodeSkip_DirtyFrame
 * @tc.desc: Test CheckScreenNodeSkip_DirtyFrame
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, CheckScreenNodeSkip_DirtyFrame, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);

    auto params = static_cast<RSScreenRenderParams*>(screenDrawable_->GetRenderParams().get());
    ASSERT_NE(params, nullptr);

    // set dirty flag
    RSMainThread::Instance()->SetDirtyFlag(true);

    auto processor = RSProcessorFactory::CreateProcessor(CompositeType::HARDWARE_COMPOSITE);
    bool result = screenDrawable_->CheckScreenNodeSkip(*params, processor);
    EXPECT_FALSE(result);

    // reset dirty flag
    RSMainThread::Instance()->SetDirtyFlag(false);
}

/**
 * @tc.name: CheckScreenNodeSkip_HDRStausChanged
 * @tc.desc: Test CheckScreenNodeSkip_HDRStausChanged
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSScreenRenderNodeDrawableTest, CheckScreenNodeSkip_HDRStausChanged, TestSize.Level1)
{
    ASSERT_NE(screenDrawable_, nullptr);

    auto params = static_cast<RSScreenRenderParams*>(screenDrawable_->GetRenderParams().get());
    ASSERT_NE(params, nullptr);

    // set hdr status changed
    params->isHDRStatusChanged_ = true;

    auto processor = RSProcessorFactory::CreateProcessor(CompositeType::HARDWARE_COMPOSITE);
    bool result = screenDrawable_->CheckScreenNodeSkip(*params, processor);
    EXPECT_FALSE(result);
}
} // namespace OHOS::Rosen
