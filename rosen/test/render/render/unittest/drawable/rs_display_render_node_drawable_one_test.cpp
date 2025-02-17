/*
 * Copyright (c) 2024-2024 Huawei Device Co., Ltd.
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

#include "gtest/gtest.h"
#include "pipeline/hardware_thread/rs_hardware_thread.h"
#include "pipeline/render_thread/rs_render_engine.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "pipeline/render_thread/rs_uni_render_util.h"
#include "pipeline/render_thread/rs_uni_render_virtual_processor.h"
#include "pipeline/rs_uifirst_manager.h"
#include "params/rs_render_thread_params.h"
#include "pipeline/rs_display_render_node.h"
#include "params/rs_display_render_params.h"
#include "platform/drawing/rs_surface_converter.h"
#include "drawable/rs_display_render_node_drawable.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::DrawableV2;

namespace OHOS::Rosen {
namespace {
constexpr NodeId DEFAULT_ID = 0xFFFF;
constexpr int32_t DEFAULT_CANVAS_SIZE = 100;
constexpr NodeId DEFAULT_RENDER_NODE_ID = 11;
constexpr NodeId DEFAULT_DISPLAY_NODE_ID = 9;
constexpr NodeId DEFAULT_SURFACE_NODE_ID = 10;
}
class RSDisplayRenderNodeDrawableTest : public testing::Test {
public:
    std::shared_ptr<RSDisplayRenderNode> renderNode_;
    std::shared_ptr<RSDisplayRenderNode> mirroredNode_;
    RSRenderNodeDrawableAdapter* drawable_ = nullptr;
    RSRenderNodeDrawableAdapter* mirroredDrawable_ = nullptr;
    RSDisplayRenderNodeDrawable* displayDrawable_ = nullptr;
    RSDisplayRenderNodeDrawable* mirroredDisplayDrawable_ = nullptr;

    std::shared_ptr<Drawing::Canvas> drawingCanvas_ = nullptr;
    std::shared_ptr<Drawing::Canvas> drawingCanvasForMirror_ = nullptr;

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline NodeId id = DEFAULT_ID;
};

void RSDisplayRenderNodeDrawableTest::SetUpTestCase() {}
void RSDisplayRenderNodeDrawableTest::TearDownTestCase() {}
void RSDisplayRenderNodeDrawableTest::SetUp()
{
    RSDisplayNodeConfig config;
    renderNode_ = std::make_shared<RSDisplayRenderNode>(DEFAULT_ID, config);
    mirroredNode_ = std::make_shared<RSDisplayRenderNode>(DEFAULT_ID + 1, config);
    if (!renderNode_ || !mirroredNode_) {
        RS_LOGE("RSDisplayRenderNodeDrawableTest: failed to create display node.");
        return;
    }

    drawable_ = RSDisplayRenderNodeDrawable::OnGenerate(renderNode_);
    mirroredDrawable_ = RSDisplayRenderNodeDrawable::OnGenerate(mirroredNode_);
    if (!drawable_ || !mirroredDrawable_) {
        RS_LOGE("RSDisplayRenderNodeDrawableTest: failed to create RSRenderNodeDrawableAdapter.");
        return;
    }

    displayDrawable_ = static_cast<RSDisplayRenderNodeDrawable*>(drawable_);
    mirroredDisplayDrawable_ = static_cast<RSDisplayRenderNodeDrawable*>(mirroredDrawable_);
    if (!displayDrawable_ || !mirroredDisplayDrawable_) {
        RS_LOGE("RSDisplayRenderNodeDrawableTest: failed to create drawable.");
        return;
    }

    displayDrawable_->renderParams_ = std::make_unique<RSDisplayRenderParams>(id);
    mirroredDisplayDrawable_->renderParams_ = std::make_unique<RSDisplayRenderParams>(id + 1);
    if (!drawable_->renderParams_ || !mirroredDrawable_->renderParams_) {
        RS_LOGE("RSDisplayRenderNodeDrawableTest: failed to init render params.");
        return;
    }

    // generate canvas for displayDrawable_
    drawingCanvas_ = std::make_unique<Drawing::Canvas>(DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE);
    if (drawingCanvas_) {
        displayDrawable_->curCanvas_ = std::make_shared<RSPaintFilterCanvas>(drawingCanvas_.get());
    }
    // generate canvas for mirroredDisplayDrawable_
    drawingCanvasForMirror_ = std::make_unique<Drawing::Canvas>(DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE);
    if (drawingCanvasForMirror_) {
        mirroredDisplayDrawable_->curCanvas_ = std::make_shared<RSPaintFilterCanvas>(drawingCanvasForMirror_.get());
    }
}
void RSDisplayRenderNodeDrawableTest::TearDown() {}

/**
 * @tc.name: CreateDisplayRenderNodeDrawableTest001
 * @tc.desc: Test If DisplayRenderNodeDrawable Can Be Created
 * @tc.type: FUNC
 * @tc.require: #I8FGDG
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, CreateDisplayRenderNodeDrawableTest001, TestSize.Level1)
{
    RSDisplayNodeConfig config;
    NodeId id = 1;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    auto drawable = RSDisplayRenderNodeDrawable::OnGenerate(displayNode);
    ASSERT_NE(drawable, nullptr);
}

/**
 * @tc.name: PrepareOffscreenRenderTest001
 * @tc.desc: Test PrepareOffscreenRender, if offscreenWidth/offscreenHeight were not initialized.
 * @tc.type: FUNC
 * @tc.require: #I8FGDG
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, PrepareOffscreenRenderTest001, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(renderNode_, nullptr);
    displayDrawable_->PrepareOffscreenRender(*displayDrawable_);

    auto params = static_cast<RSDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    auto type = system::GetParameter("const.window.foldscreen.type", "");
    system::SetParameter("const.window.foldscreen.type", "1");
    params->isRotationChanged_ = true;
    params->frameRect_ = { 0.f, 0.f, 1.f, 0.f };
    displayDrawable_->PrepareOffscreenRender(*displayDrawable_);
    ASSERT_TRUE(params->IsRotationChanged());

    params->frameRect_ = { 0.f, 0.f, 1.f, 1.f };
    displayDrawable_->PrepareOffscreenRender(*displayDrawable_);
    ASSERT_FALSE(displayDrawable_->curCanvas_->GetSurface());

    auto surface = std::make_shared<Drawing::Surface>();
    displayDrawable_->curCanvas_->surface_ = surface.get();
    displayDrawable_->PrepareOffscreenRender(*displayDrawable_);
    ASSERT_TRUE(displayDrawable_->curCanvas_->GetSurface());
    system::SetParameter("const.window.foldscreen.type", type);
}

/**
 * @tc.name: PrepareOffscreenRenderTest002
 * @tc.desc: Test PrepareOffscreenRender, offscreenWidth/offscreenHeight is/are correctly initialized.
 * @tc.type: FUNC
 * @tc.require: #I8FGDG
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, PrepareOffscreenRenderTest002, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(renderNode_, nullptr);
    renderNode_->GetMutableRenderProperties().SetFrameWidth(DEFAULT_CANVAS_SIZE);
    renderNode_->GetMutableRenderProperties().SetFrameHeight(DEFAULT_CANVAS_SIZE);
    displayDrawable_->PrepareOffscreenRender(*displayDrawable_);
}

/**
 * @tc.name: InitTranslateForWallpaperTest001
 * @tc.desc: Test InitTranslateForWallpaper
 * @tc.type: FUNC
 * @tc.require: #IB5JZQ
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, InitTranslateForWallpaperTest001, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    system::SetParameter("const.cache.optimize.rotate.enable", "true");
    auto params = static_cast<RSDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    ASSERT_NE(params, nullptr);
    params->frameRect_ = {0.f, 0.f, 100.f, 100.f};
    params->screenInfo_.width = 100;
    params->screenInfo_.height = 100;
    displayDrawable_->InitTranslateForWallpaper();
    auto& rtThread = RSUniRenderThread::Instance();
}

/**
 * @tc.name: DrawCurtainScreenTest001
 * @tc.desc: Test DrawCurtainScreen
 * @tc.type: FUNC
 * @tc.require: #I8FGDG
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, DrawCurtainScreenTest001, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    displayDrawable_->DrawCurtainScreen();
    ASSERT_TRUE(!RSUniRenderThread::Instance().IsCurtainScreenOn());

    auto params = std::make_unique<RSRenderThreadParams>();
    params->isCurtainScreenOn_ = true;
    RSUniRenderThread::Instance().Sync(move(params));
    RSUniRenderThread::Instance().GetRSRenderThreadParams()->isCurtainScreenOn_ = true;
    displayDrawable_->DrawCurtainScreen();
    ASSERT_TRUE(RSUniRenderThread::Instance().IsCurtainScreenOn());
    RSUniRenderThread::Instance().GetRSRenderThreadParams()->isCurtainScreenOn_ = false;
}

/**
 * @tc.name: ClearTransparentBeforeSaveLayerTest001
 * @tc.desc: Test ClearTransparentBeforeSaveLayer, with two surface with/without param initialization
 * @tc.type: FUNC
 * @tc.require: #I8FGDG
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, ClearTransparentBeforeSaveLayerTest001, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
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
    rtThread.GetRSRenderThreadParams()->hardwareEnabledTypeDrawables_.push_back(drawable1);
    rtThread.GetRSRenderThreadParams()->hardwareEnabledTypeDrawables_.push_back(drawable2);
    ASSERT_NE(renderNode_, nullptr);
    renderNode_->GetMutableRenderProperties().SetFrameWidth(DEFAULT_CANVAS_SIZE);
    renderNode_->GetMutableRenderProperties().SetFrameHeight(DEFAULT_CANVAS_SIZE);
    displayDrawable_->ClearTransparentBeforeSaveLayer();
}

/**
 * @tc.name: DrawWatermarkIfNeedTest001
 * @tc.desc: Test DrawWatermarkIfNeed
 * @tc.type: FUNC
 * @tc.require: #I8FGDG
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, DrawWatermarkIfNeedTest001, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->curCanvas_, nullptr);
    auto params = static_cast<RSDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    ASSERT_TRUE(params);
    if (params) {
        displayDrawable_->DrawWatermarkIfNeed(*params, *displayDrawable_->curCanvas_);
    }

    ASSERT_TRUE(RSUniRenderThread::Instance().GetRSRenderThreadParams());

    RSUniRenderThread::Instance().GetRSRenderThreadParams()->watermarkFlag_ = true;
    displayDrawable_->DrawWatermarkIfNeed(*params, *displayDrawable_->curCanvas_);

    RSUniRenderThread::Instance().GetRSRenderThreadParams()->watermarkImg_ = std::make_shared<Drawing::Image>();
    displayDrawable_->DrawWatermarkIfNeed(*params, *displayDrawable_->curCanvas_);
    RSUniRenderThread::Instance().GetRSRenderThreadParams()->watermarkFlag_ = false;
    RSUniRenderThread::Instance().GetRSRenderThreadParams()->watermarkImg_ = nullptr;
}

/**
 * @tc.name: CalculateVirtualDirtyForWiredScreenTest001
 * @tc.desc: Test CalculateVirtualDirtyForWiredScreen, without mirrorNode
 * @tc.type: FUNC
 * @tc.require: #SD46SV
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, CalculateVirtualDirtyForWiredScreenTest001, TestSize.Level1)
{
    ASSERT_NE(renderNode_, nullptr);
    ASSERT_NE(displayDrawable_, nullptr);

    auto params = std::make_shared<RSDisplayRenderParams>(0);
    ASSERT_NE(params, nullptr);
    auto renderFrame = std::make_unique<RSRenderFrame>(nullptr, nullptr);
    ASSERT_NE(renderFrame, nullptr);
    Drawing::Matrix canvasMatrix;
    auto damageRects = displayDrawable_->CalculateVirtualDirtyForWiredScreen(
        renderFrame, *params, canvasMatrix);
    ASSERT_EQ(damageRects.size(), 0);

    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(DEFAULT_DISPLAY_NODE_ID, config);
    params->mirrorSourceDrawable_ = DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(node);
    damageRects = displayDrawable_->CalculateVirtualDirtyForWiredScreen(renderFrame, *params, canvasMatrix);
    ASSERT_EQ(damageRects.size(), 0);
}

/**
 * @tc.name: CalculateVirtualDirtyForWiredScreenTest002
 * @tc.desc: Test CalculateVirtualDirtyForWiredScreen, isVirtualDirtyEnabled_ false
 * @tc.type: FUNC
 * @tc.require: #SD46SV
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, CalculateVirtualDirtyForWiredScreenTest002, TestSize.Level1)
{
    ASSERT_NE(renderNode_, nullptr);
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(mirroredNode_, nullptr);

    auto params = std::make_shared<RSDisplayRenderParams>(0);
    ASSERT_NE(params, nullptr);
    auto renderFrame = std::make_unique<RSRenderFrame>(nullptr, nullptr);
    ASSERT_NE(renderFrame, nullptr);
    Drawing::Matrix canvasMatrix;
    params->mirrorSourceDrawable_ = mirroredNode_->GetRenderDrawable();
    auto& rtThread = RSUniRenderThread::Instance();
    if (rtThread.GetRSRenderThreadParams()) {
        rtThread.GetRSRenderThreadParams()->isVirtualDirtyEnabled_ = false;
    }
    auto damageRects = displayDrawable_->CalculateVirtualDirtyForWiredScreen(
        renderFrame, *params, canvasMatrix);
    ASSERT_EQ(damageRects.size(), 0);
}

/**
 * @tc.name: CalculateVirtualDirtyForWiredScreenTest003
 * @tc.desc: Test CalculateVirtualDirtyForWiredScreen, without syncDirtyManager
 * @tc.type: FUNC
 * @tc.require: #SD46SV
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, CalculateVirtualDirtyForWiredScreenTest003, TestSize.Level1)
{
    ASSERT_NE(renderNode_, nullptr);
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(mirroredNode_, nullptr);

    auto params = std::make_shared<RSDisplayRenderParams>(0);
    ASSERT_NE(params, nullptr);
    auto renderFrame = std::make_unique<RSRenderFrame>(nullptr, nullptr);
    ASSERT_NE(renderFrame, nullptr);
    Drawing::Matrix canvasMatrix;
    params->mirrorSourceDrawable_ = mirroredNode_->GetRenderDrawable();
    auto& rtThread = RSUniRenderThread::Instance();
    if (rtThread.GetRSRenderThreadParams()) {
        rtThread.GetRSRenderThreadParams()->isVirtualDirtyEnabled_ = true;
    }
    auto damageRects = displayDrawable_->CalculateVirtualDirtyForWiredScreen(
        renderFrame, *params, canvasMatrix);
    ASSERT_EQ(damageRects.size(), 0);
}

/**
 * @tc.name: CalculateVirtualDirtyForWiredScreenTest004
 * @tc.desc: Test CalculateVirtualDirtyForWiredScreen, canvasMatrix not equals to lastMatrix_
 * @tc.type: FUNC
 * @tc.require: #SD46SV
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, CalculateVirtualDirtyForWiredScreenTest004, TestSize.Level1)
{
    ASSERT_NE(renderNode_, nullptr);
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(mirroredNode_, nullptr);

    auto params = std::make_shared<RSDisplayRenderParams>(0);
    ASSERT_NE(params, nullptr);
    auto renderFrame = std::make_unique<RSRenderFrame>(nullptr, nullptr);
    ASSERT_NE(renderFrame, nullptr);
    Drawing::Matrix canvasMatrix;
    params->mirrorSourceDrawable_ = mirroredNode_->GetRenderDrawable();
    auto& rtThread = RSUniRenderThread::Instance();
    if (rtThread.GetRSRenderThreadParams()) {
        rtThread.GetRSRenderThreadParams()->isVirtualDirtyEnabled_ = true;
    }
    displayDrawable_->syncDirtyManager_ = std::make_shared<RSDirtyRegionManager>(false);
    const Drawing::scalar scale = 100.0f;
    canvasMatrix.SetScale(scale, scale);
    auto damageRects = displayDrawable_->CalculateVirtualDirtyForWiredScreen(
        renderFrame, *params, canvasMatrix);
    ASSERT_EQ(damageRects.size(), 0);
}

/**
 * @tc.name: CalculateVirtualDirtyForWiredScreenTest005
 * @tc.desc: Test CalculateVirtualDirtyForWiredScreen, extraDirty is not empty
 * @tc.type: FUNC
 * @tc.require: #SD46SV
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, CalculateVirtualDirtyForWiredScreenTest005, TestSize.Level1)
{
    ASSERT_NE(renderNode_, nullptr);
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(mirroredNode_, nullptr);

    auto params = std::make_shared<RSDisplayRenderParams>(0);
    ASSERT_NE(params, nullptr);
    auto renderFrame = std::make_unique<RSRenderFrame>(nullptr, nullptr);
    ASSERT_NE(renderFrame, nullptr);
    Drawing::Matrix canvasMatrix;
    params->mirrorSourceDrawable_ = mirroredNode_->GetRenderDrawable();
    auto& rtThread = RSUniRenderThread::Instance();
    if (rtThread.GetRSRenderThreadParams()) {
        rtThread.GetRSRenderThreadParams()->isVirtualDirtyEnabled_ = true;
    }
    displayDrawable_->syncDirtyManager_ = std::make_shared<RSDirtyRegionManager>(false);
    displayDrawable_->syncDirtyManager_->dirtyRegion_ = RectI(0, 0, DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE);
    auto damageRects = displayDrawable_->CalculateVirtualDirtyForWiredScreen(renderFrame, *params, canvasMatrix);
    ASSERT_EQ(damageRects.size(), 0);
}

/**
 * @tc.name: CalculateVirtualDirtyForWiredScreenTest006
 * @tc.desc: Test CalculateVirtualDirtyForWiredScreen, extraDirty is not empty
 * @tc.type: FUNC
 * @tc.require: #SD46SV
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, CalculateVirtualDirtyForWiredScreenTest006, TestSize.Level1)
{
    ASSERT_NE(renderNode_, nullptr);
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(mirroredNode_, nullptr);

    auto params = std::make_shared<RSDisplayRenderParams>(0);
    ASSERT_NE(params, nullptr);
    auto renderFrame = std::make_unique<RSRenderFrame>(nullptr, nullptr);
    ASSERT_NE(renderFrame, nullptr);
    Drawing::Matrix canvasMatrix;
    params->mirrorSourceDrawable_ = mirroredNode_->GetRenderDrawable();
    auto& rtThread = RSUniRenderThread::Instance();
    if (rtThread.GetRSRenderThreadParams()) {
        rtThread.GetRSRenderThreadParams()->isVirtualDirtyEnabled_ = true;
        rtThread.GetRSRenderThreadParams()->isVirtualDirtyDfxEnabled_ = true;
    }
    displayDrawable_->syncDirtyManager_ = std::make_shared<RSDirtyRegionManager>(false);
    auto damageRects = displayDrawable_->CalculateVirtualDirtyForWiredScreen(
        renderFrame, *params, canvasMatrix);
    ASSERT_EQ(damageRects.size(), 0);
}

/**
 * @tc.name: RenderOverDrawTest001
 * @tc.desc: Test RenderOverDraw
 * @tc.type: FUNC
 * @tc.require: #IB1MHX
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, RenderOverDrawTest001, TestSize.Level1)
{
    ASSERT_NE(renderNode_, nullptr);
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);

    displayDrawable_->RenderOverDraw();
    // generate canvas for displayDrawable_
    drawingCanvas_ = std::make_unique<Drawing::Canvas>(DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE);
    if (drawingCanvas_) {
        displayDrawable_->curCanvas_ = std::make_shared<RSPaintFilterCanvas>(drawingCanvas_.get());
    }
    RSUniRenderThread::Instance().GetRSRenderThreadParams()->isOverDrawEnabled_ = true;
    displayDrawable_->RenderOverDraw();
}

/**
 * @tc.name: DrawExpandScreenTest001
 * @tc.desc: Test DrawExpandScreen
 * @tc.type: FUNC
 * @tc.require: #I8FGDG
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, DrawExpandScreenTest001, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);

    auto virtualProcesser = new RSUniRenderVirtualProcessor();
    displayDrawable_->DrawExpandScreen(*virtualProcesser);
}

/**
 * @tc.name: HardCursorCreateLayerTest001
 * @tc.desc: Test HardCursorCreateLayer
 * @tc.type: FUNC
 * @tc.require: #IAX2SN
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, HardCursorCreateLayerTest001, TestSize.Level1)
{
    ASSERT_NE(renderNode_, nullptr);
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);

    auto params = static_cast<RSDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    ASSERT_NE(params, nullptr);
    auto processor = RSProcessorFactory::CreateProcessor(params->GetCompositeType());
    ASSERT_NE(processor, nullptr);

    auto result = displayDrawable_->HardCursorCreateLayer(processor);
    ASSERT_EQ(result, false);

    HardCursorInfo hardInfo;
    hardInfo.id = 1;
    auto renderNode = std::make_shared<RSRenderNode>(hardInfo.id);
    hardInfo.drawablePtr = RSRenderNodeDrawableAdapter::OnGenerate(renderNode);
    EXPECT_NE(hardInfo.drawablePtr, nullptr);
    result = displayDrawable_->HardCursorCreateLayer(processor);
    ASSERT_EQ(result, false);

    NodeId id = 1;
    hardInfo.drawablePtr->renderParams_ = std::make_unique<RSRenderParams>(id);
    result = displayDrawable_->HardCursorCreateLayer(processor);
    ASSERT_EQ(result, false);
}

/**
 * @tc.name: CheckAndUpdateFilterCacheOcclusionTest001
 * @tc.desc: Test CheckAndUpdateFilterCacheOcclusion
 * @tc.type: FUNC
 * @tc.require: issueIAL2EA
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, CheckAndUpdateFilterCacheOcclusionTest001, TestSize.Level1)
{
    ASSERT_NE(renderNode_, nullptr);
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);

    std::shared_ptr<RSSurfaceRenderNode> surfaceNode = std::make_shared<RSSurfaceRenderNode>(DEFAULT_SURFACE_NODE_ID);
    ASSERT_NE(surfaceNode, nullptr);

    auto surfaceDrawableAdapter = RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode);
    ASSERT_NE(surfaceDrawableAdapter, nullptr);

    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawableAdapter->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    auto screenInfo = surfaceDrawableAdapter->renderParams_->GetScreenInfo();
    auto params = static_cast<RSDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    ASSERT_NE(params, nullptr);
    vector<std::shared_ptr<RSRenderNodeDrawableAdapter>> allSurfaceDrawable{surfaceDrawableAdapter};
    params->SetAllMainAndLeashSurfaceDrawables(allSurfaceDrawable);

    surfaceParams->isMainWindowType_ = false;
    RSDisplayRenderNodeDrawable::CheckAndUpdateFilterCacheOcclusion(*params, screenInfo);

    surfaceParams->isMainWindowType_ = true;
    RSDisplayRenderNodeDrawable::CheckAndUpdateFilterCacheOcclusion(*params, screenInfo);
}

/**
 * @tc.name: CheckFilterCacheFullyCoveredTest001
 * @tc.desc: Test CheckFilterCacheFullyCovered
 * @tc.type: FUNC
 * @tc.require: issueIAL2EA
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, CheckFilterCacheFullyCoveredTest001, TestSize.Level1)
{
    ASSERT_NE(renderNode_, nullptr);
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);

    auto surfaceParams = std::make_unique<RSSurfaceRenderParams>(DEFAULT_SURFACE_NODE_ID);
    ASSERT_NE(surfaceParams, nullptr);
    surfaceParams->visibleFilterChild_ = {DEFAULT_RENDER_NODE_ID};

    std::shared_ptr<RSRenderNode> renderNode = std::make_shared<RSRenderNode>(DEFAULT_RENDER_NODE_ID);
    ASSERT_NE(renderNode, nullptr);
    auto renderDrawableAdapter = RSRenderNodeDrawableAdapter::OnGenerate(renderNode);
    ASSERT_NE(renderDrawableAdapter, nullptr);
    renderDrawableAdapter->renderParams_ = std::make_unique<RSRenderParams>(DEFAULT_RENDER_NODE_ID);
    ASSERT_NE(renderDrawableAdapter->renderParams_, nullptr);
    RectI screenRect{0, 0, 0, 0};

    renderDrawableAdapter->renderParams_->SetHasBlurFilter(false);
    RSDisplayRenderNodeDrawable::CheckFilterCacheFullyCovered(*surfaceParams, screenRect);

    surfaceParams->isTransparent_ = true;
    surfaceParams->isRotating_ = true;
    RSDisplayRenderNodeDrawable::CheckFilterCacheFullyCovered(*surfaceParams, screenRect);

    surfaceParams->isRotating_ = false;
    RSDisplayRenderNodeDrawable::CheckFilterCacheFullyCovered(*surfaceParams, screenRect);

    surfaceParams->isTransparent_ = false;
    renderDrawableAdapter->renderParams_->SetHasBlurFilter(true);
    RSDisplayRenderNodeDrawable::CheckFilterCacheFullyCovered(*surfaceParams, screenRect);

    renderDrawableAdapter->renderParams_->SetEffectNodeShouldPaint(false);
    RSDisplayRenderNodeDrawable::CheckFilterCacheFullyCovered(*surfaceParams, screenRect);

    renderDrawableAdapter->renderParams_->SetNodeType(RSRenderNodeType::EFFECT_NODE);
    RSDisplayRenderNodeDrawable::CheckFilterCacheFullyCovered(*surfaceParams, screenRect);

    renderDrawableAdapter->renderParams_->SetHasGlobalCorner(true);
    RSDisplayRenderNodeDrawable::CheckFilterCacheFullyCovered(*surfaceParams, screenRect);

    renderDrawableAdapter->renderParams_->SetGlobalAlpha(0.f);
    RSDisplayRenderNodeDrawable::CheckFilterCacheFullyCovered(*surfaceParams, screenRect);

    renderDrawableAdapter->renderParams_->SetHasBlurFilter(true);
    renderDrawableAdapter->renderParams_->SetGlobalAlpha(1.f);
    renderDrawableAdapter->renderParams_->SetHasGlobalCorner(false);
    renderDrawableAdapter->renderParams_->SetNodeType(RSRenderNodeType::CANVAS_NODE);
    renderDrawableAdapter->renderParams_->SetEffectNodeShouldPaint(true);
    RSDisplayRenderNodeDrawable::CheckFilterCacheFullyCovered(*surfaceParams, screenRect);
}

/**
 * @tc.name: OnDrawTest001
 * @tc.desc: Test OnDraw
 * @tc.type: FUNC
 * @tc.require: #I8FGDG
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, OnDrawTest001, TestSize.Level1)
{
    ASSERT_NE(displayDrawable_, nullptr);
    Drawing::Canvas canvas;
    displayDrawable_->OnDraw(canvas);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);

    HardCursorInfo hardInfo;
    hardInfo.id = 1;
    auto renderNode = std::make_shared<RSRenderNode>(hardInfo.id);
    hardInfo.drawablePtr = RSRenderNodeDrawableAdapter::OnGenerate(renderNode);
    displayDrawable_->OnDraw(canvas);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);
}

/**
 * @tc.name: CheckDisplayNodeSkipTest001
 * @tc.desc: Test CheckDisplayNodeSkip
 * @tc.type: FUNC
 * @tc.require: #I8FGDG
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, CheckDisplayNodeSkipTest001, TestSize.Level1)
{
    ASSERT_NE(renderNode_, nullptr);
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);
    
    auto params = static_cast<RSDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    ASSERT_NE(params, nullptr);
    auto processor = RSProcessorFactory::CreateProcessor(params->GetCompositeType());
    RSUniRenderThread::Instance().Sync(std::make_unique<RSRenderThreadParams>());
    auto result = displayDrawable_->CheckDisplayNodeSkip(*params, processor);
    ASSERT_EQ(result, true);

    std::shared_ptr<RSSurfaceRenderNodeDrawable> drawable = nullptr;
    RSUifirstManager::Instance().pendingPostDrawables_.push_back(drawable);
    result = displayDrawable_->CheckDisplayNodeSkip(*params, processor);
    ASSERT_EQ(result, false);

    RSUniRenderThread::Instance().uniRenderEngine_ = std::make_shared<RSRenderEngine>();
    RSUniRenderThread::Instance().GetRSRenderThreadParams()->isForceCommitLayer_ = true;
    result = displayDrawable_->CheckDisplayNodeSkip(*params, processor);
    ASSERT_EQ(result, true);

    RSMainThread::Instance()->isDirty_ = true;
    result = displayDrawable_->CheckDisplayNodeSkip(*params, processor);
    ASSERT_EQ(result, false);

    RSUifirstManager::Instance().hasDoneNode_ = true;
    result = displayDrawable_->CheckDisplayNodeSkip(*params, processor);
    ASSERT_EQ(result, false);

    params->isMainAndLeashSurfaceDirty_ = true;
    result = displayDrawable_->CheckDisplayNodeSkip(*params, processor);
    ASSERT_EQ(result, false);

    displayDrawable_->syncDirtyManager_->currentFrameDirtyRegion_ = { 0, 0, 1, 1 };
    displayDrawable_->syncDirtyManager_->debugRect_ = { 1, 1, 1, 1 };
    result = displayDrawable_->CheckDisplayNodeSkip(*params, processor);
    ASSERT_EQ(result, false);
    RSUniRenderThread::Instance().uniRenderEngine_ = nullptr;
    RSUniRenderThread::Instance().GetRSRenderThreadParams()->isForceCommitLayer_ = false;
    RSMainThread::Instance()->isDirty_ = false;
    RSUifirstManager::Instance().hasDoneNode_ = false;
    RSUifirstManager::Instance().pendingPostDrawables_.clear();
}

/**
 * @tc.name: DrawMirrorScreenTest001
 * @tc.desc: Test DrawMirrorScreen
 * @tc.type: FUNC
 * @tc.require: #I8FGDG
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, DrawMirrorScreenTest001, TestSize.Level1)
{
    ASSERT_NE(renderNode_, nullptr);
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);
    ASSERT_NE(mirroredNode_, nullptr);

    auto& rtThread = RSUniRenderThread::Instance();
    if (!rtThread.GetRSRenderThreadParams()) {
        rtThread.Sync(std::make_unique<RSRenderThreadParams>());
    }
    rtThread.GetRSRenderThreadParams()->isVirtualDirtyEnabled_ = true;
    CaptureParam param;
    param.isSingleSurface_ = true;
    rtThread.SetCaptureParam(param);

    auto params = static_cast<RSDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    if (mirroredNode_->GetRenderDrawable() == nullptr) {
        mirroredNode_->renderDrawable_ = DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(mirroredNode_);
    }
    params->mirrorSourceDrawable_ = mirroredNode_->GetRenderDrawable();
    auto processor = RSProcessorFactory::CreateProcessor(params->GetCompositeType());
    displayDrawable_->DrawMirrorScreen(*params, processor);
    sleep(1);
}

/**
 * @tc.name: CalculateVirtualDirtyTest001
 * @tc.desc: Test CalculateVirtualDirty
 * @tc.type: FUNC
 * @tc.require: #I8FGDG
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, CalculateVirtualDirtyTest001, TestSize.Level1)
{
    ASSERT_NE(renderNode_, nullptr);
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);
    ASSERT_NE(mirroredNode_, nullptr);

    auto& rtThread = RSUniRenderThread::Instance();
    if (!rtThread.GetRSRenderThreadParams()) {
        rtThread.Sync(std::make_unique<RSRenderThreadParams>());
    }
    displayDrawable_->PrepareOffscreenRender(*displayDrawable_);
    auto params = static_cast<RSDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    if (mirroredNode_->GetRenderDrawable() == nullptr) {
        mirroredNode_->renderDrawable_ = DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(mirroredNode_);
    }
    params->mirrorSourceDrawable_ = mirroredNode_->GetRenderDrawable();
    auto processor = RSProcessorFactory::CreateProcessor(params->GetCompositeType());
    auto virtualProcesser = std::make_shared<RSUniRenderVirtualProcessor>();
    Drawing::Matrix matrix;
    displayDrawable_->CalculateVirtualDirty(virtualProcesser, *params, matrix);
    sleep(1);
}

/**
 * @tc.name: DrawMirrorTest001
 * @tc.desc: Test DrawMirror
 * @tc.type: FUNC
 * @tc.require: #I8FGDG
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, DrawMirrorTest001, TestSize.Level1)
{
    ASSERT_NE(renderNode_, nullptr);
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);

    displayDrawable_->PrepareOffscreenRender(*displayDrawable_);
    auto params = static_cast<RSDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    auto processor = RSProcessorFactory::CreateProcessor(params->GetCompositeType());
    auto virtualProcesser = std::make_shared<RSUniRenderVirtualProcessor>();
    auto uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams().get();

    displayDrawable_->DrawMirror(*params, virtualProcesser,
        &RSDisplayRenderNodeDrawable::OnCapture, *uniParam);
    displayDrawable_->DrawMirror(*params, virtualProcesser,
        &RSDisplayRenderNodeDrawable::DrawHardwareEnabledNodes, *uniParam);
}

/**
 * @tc.name: WiredScreenProjectionTest001
 * @tc.desc: Test WiredScreenProjection
 * @tc.type: FUNC
 * @tc.require: #I8FGDG
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, WiredScreenProjectionTest001, TestSize.Level1)
{
    ASSERT_NE(renderNode_, nullptr);
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);
    
    auto params = static_cast<RSDisplayRenderParams*>(displayDrawable_->GetRenderParams().get());
    auto processor = RSProcessorFactory::CreateProcessor(params->GetCompositeType());
    auto virtualProcesser = std::make_shared<RSUniRenderVirtualProcessor>();
    displayDrawable_->WiredScreenProjection(*params, virtualProcesser);
}

/**
 * @tc.name: SkipFrameByIntervalTest003
 * @tc.desc: test SkipFrameByInterval for skipFrameInterval 1
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, SkipFrameByIntervalTest003, TestSize.Level1)
{
    uint32_t refreshRate = 60; // 60hz
    uint32_t skipFrameInterval = 1;
    ASSERT_FALSE(displayDrawable_->SkipFrameByInterval(refreshRate, skipFrameInterval));
}

/**
 * @tc.name: SkipFrameByIntervalTest004
 * @tc.desc: test SkipFrameByInterval for time within skipFrameInterval 2
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, SkipFrameByIntervalTest004, TestSize.Level1)
{
    uint32_t refreshRate = 60; // 60hz
    uint32_t skipFrameInterval = 2; // skipFrameInterval 2
    displayDrawable_->SkipFrameByInterval(refreshRate, skipFrameInterval);
    ASSERT_TRUE(displayDrawable_->SkipFrameByInterval(refreshRate, skipFrameInterval));
}

/**
 * @tc.name: SkipFrameByIntervalTest001
 * @tc.desc: test SkipFrameByInterval for refreshRate 0 and skipFrameInterval 0
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, SkipFrameByIntervalTest001, TestSize.Level1)
{
    uint32_t refreshRate = 0;
    uint32_t skipFrameInterval = 0;
    ASSERT_FALSE(displayDrawable_->SkipFrameByInterval(refreshRate, skipFrameInterval));
}

/**
 * @tc.name: SkipFrameByIntervalTest002
 * @tc.desc: test SkipFrameByInterval for skipFrameInterval 0
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSDisplayRenderNodeDrawableTest, SkipFrameByIntervalTest002, TestSize.Level1)
{
    uint32_t refreshRate = 60; // 60hz
    uint32_t skipFrameInterval = 0;
    ASSERT_FALSE(displayDrawable_->SkipFrameByInterval(refreshRate, skipFrameInterval));
}
}