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

#include "gtest/gtest.h"
#include "drawable/rs_display_render_node_drawable.h"
#include "drawable/rs_surface_render_node_drawable.h"
#include "params/rs_render_thread_params.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_uni_render_thread.h"
#include "params/rs_render_thread_params.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::DrawableV2;

namespace OHOS::Rosen {
constexpr int32_t DEFAULT_CANVAS_SIZE = 100;
constexpr NodeId DEFAULT_ID = 0xFFFF;

class RSSurfaceRenderNodeDrawableTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    std::shared_ptr<RSSurfaceRenderNode> renderNode_;
    RSRenderNodeDrawableAdapter* drawable_ = nullptr;
    RSSurfaceRenderNodeDrawable* surfaceDrawable_ = nullptr;
    std::shared_ptr<Drawing::Canvas> drawingCanvas_;
    std::shared_ptr<RSPaintFilterCanvas> canvas_;
};

void RSSurfaceRenderNodeDrawableTest::SetUpTestCase() {}
void RSSurfaceRenderNodeDrawableTest::TearDownTestCase() {}
void RSSurfaceRenderNodeDrawableTest::SetUp()
{
    renderNode_ = std::make_shared<RSSurfaceRenderNode>(DEFAULT_ID);
    if (!renderNode_) {
        RS_LOGE("RSSurfaceRenderNodeDrawableTest: failed to create surface node.");
    }
    drawable_ = RSSurfaceRenderNodeDrawable::OnGenerate(renderNode_);
    if (drawable_) {
        drawable_->renderParams_ = std::make_unique<RSSurfaceRenderParams>(DEFAULT_ID);
        surfaceDrawable_ = static_cast<RSSurfaceRenderNodeDrawable*>(drawable_);
        if (!drawable_->renderParams_) {
            RS_LOGE("RSSurfaceRenderNodeDrawableTest: failed to init render params.");
        }
    }
    drawingCanvas_ = std::make_unique<Drawing::Canvas>(DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE);
    if (drawingCanvas_) {
        canvas_ = std::make_shared<RSPaintFilterCanvas>(drawingCanvas_.get());
    }
}
void RSSurfaceRenderNodeDrawableTest::TearDown() {}

/**
 * @tc.name: CreateSurfaceRenderNodeDrawableTest
 * @tc.desc: Test If SurfaceRenderNodeDrawable Can Be Created
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, CreateSurfaceRenderNodeDrawable, TestSize.Level1)
{
    NodeId id = 1;
    auto renderNode = std::make_shared<RSSurfaceRenderNode>(id);
    auto drawable = RSSurfaceRenderNodeDrawable::OnGenerate(renderNode);
    ASSERT_NE(drawable, nullptr);
}

/**
 * @tc.name: FindInstanceChildOfDisplay
 * @tc.desc: Test FindInstanceChildOfDisplay, early return case
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, FindInstanceChildOfDisplay001, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    ASSERT_EQ(surfaceDrawable_->FindInstanceChildOfDisplay(nullptr), INVALID_NODEID);

    NodeId id = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(id);
    ASSERT_EQ(surfaceDrawable_->FindInstanceChildOfDisplay(surfaceNode), INVALID_NODEID);
}

/**
 * @tc.name: FindInstanceChildOfDisplay
 * @tc.desc: Test FindInstanceChildOfDisplay, if surface parent is display
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, FindInstanceChildOfDisplay002, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    RSDisplayNodeConfig config;
    NodeId displayId = 1;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(displayId, config);
    NodeId surfaceId = 2;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceId);
    displayNode->AddChild(surfaceNode);

    ASSERT_EQ(surfaceDrawable_->FindInstanceChildOfDisplay(surfaceNode), surfaceId);
}

/**
 * @tc.name: CacheImgForCapture
 * @tc.desc: Test CacheImgForCapture
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, CacheImgForCapture, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    RSDisplayNodeConfig config;
    NodeId displayId = 1;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(displayId, config);
    auto displayDrawable =
        std::static_pointer_cast<RSDisplayRenderNodeDrawable>(RSRenderNodeDrawableAdapter::OnGenerate(displayNode));
    ASSERT_NE(displayDrawable, nullptr);
    std::shared_ptr<Drawing::Surface> surface = Drawing::Surface::MakeRasterN32Premul(100, 100);
    ASSERT_NE(surface, nullptr);
    RSPaintFilterCanvas paintFilterCanvas(surface.get());
    surfaceDrawable_->CacheImgForCapture(paintFilterCanvas, *displayDrawable);
}

/**
 * @tc.name: OnDraw
 * @tc.desc: Test OnDraw
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, OnDraw, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    // default case, shouldpaint == false
    ASSERT_NE(drawable_->renderParams_, nullptr);
    surfaceDrawable_->OnDraw(*drawingCanvas_);
    // if should paint
    drawable_->renderParams_->shouldPaint_ = true;
    drawable_->renderParams_->contentEmpty_ = false;
    surfaceDrawable_->OnDraw(*drawingCanvas_);
}

/**
 * @tc.name: MergeDirtyRegionBelowCurSurface
 * @tc.desc: Test MergeDirtyRegionBelowCurSurface, default case, empty dirty region
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, MergeDirtyRegionBelowCurSurface001, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto uniParams = std::make_shared<RSRenderThreadParams>();
    ASSERT_NE(uniParams, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    Drawing::Region region;
    surfaceDrawable_->MergeDirtyRegionBelowCurSurface(*uniParams, region);
    ASSERT_TRUE(region.IsEmpty());
}

/**
 * @tc.name: MergeDirtyRegionBelowCurSurface
 * @tc.desc: Test MergeDirtyRegionBelowCurSurface, dirty region expected not empty.
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, MergeDirtyRegionBelowCurSurface002, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto uniParams = std::make_shared<RSRenderThreadParams>();
    ASSERT_NE(uniParams, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    Drawing::Region region;
    surfaceParams->isMainWindowType_ = true;
    surfaceParams->visibleRegion_ = Occlusion::Rect{0, 0, DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE};
    surfaceParams->transparentRegion_ = Occlusion::Rect{0, 0, DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE};
    uniParams->accumulatedDirtyRegion_ = Occlusion::Rect{0, 0, DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE};
    surfaceDrawable_->MergeDirtyRegionBelowCurSurface(*uniParams, region);
    ASSERT_FALSE(region.IsEmpty());
}

/**
 * @tc.name: OnCapture
 * @tc.desc: Test OnCapture
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, OnCapture, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    ASSERT_NE(drawable_->renderParams_, nullptr);
    // default, should not paint
    surfaceDrawable_->OnCapture(*drawingCanvas_);
    // should paint
    drawable_->renderParams_->shouldPaint_ = true;
    drawable_->renderParams_->contentEmpty_ = false;
    surfaceDrawable_->OnCapture(*drawingCanvas_);
}

/**
 * @tc.name: EnableRecordingOptimization
 * @tc.desc: Test EnableRecordingOptimization
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, EnableRecordingOptimization, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = drawable_->renderParams_ ?
        static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get()) : nullptr;
    ASSERT_NE(surfaceParams, nullptr);
    ASSERT_FALSE(surfaceDrawable_->EnableRecordingOptimization(*surfaceParams));
}

/**
 * @tc.name: CaptureSurface
 * @tc.desc: Test CaptureSurface, default case
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, CaptureSurface001, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
}

/**
 * @tc.name: CaptureSurface
 * @tc.desc: Test CaptureSurface, special case: security/protected layer.
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, CaptureSurface002, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    surfaceParams->isSecurityLayer_ = true;
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    surfaceParams->isSecurityLayer_ = false;
    surfaceParams->isProtectedLayer_ = true;
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
}

/**
 * @tc.name: PrepareOffscreenRender
 * @tc.desc: Test PrepareOffscreenRender
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, CheckIfNeedResetRotate, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    ASSERT_FALSE(surfaceDrawable_->CheckIfNeedResetRotate(paintFilterCanvas));
}

/**
 * @tc.name: CalculateVisibleRegion
 * @tc.desc: Test CalculateVisibleRegion
 * @tc.type: FUNC
 * @tc.require: #IA940V
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, CalculateVisibleRegion, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    auto uniParams = std::make_shared<RSRenderThreadParams>();

    surfaceParams->isMainWindowType_ = false;
    surfaceParams->isLeashWindow_ = true;
    surfaceParams->isAppWindow_ = false;
    Drawing::Region result = surfaceDrawable_->CalculateVisibleRegion(*uniParams,
        *surfaceParams, *surfaceDrawable_, true);
    ASSERT_TRUE(result.IsEmpty());

    surfaceParams->isMainWindowType_ = true;
    surfaceParams->isLeashWindow_ = false;
    surfaceParams->isAppWindow_ = false;
    result = surfaceDrawable_->CalculateVisibleRegion(*uniParams, *surfaceParams, *surfaceDrawable_, true);
    ASSERT_FALSE(result.IsEmpty());

    uniParams->SetOcclusionEnabled(true);
    Occlusion::Region region;
    surfaceParams->SetVisibleRegion(region);
    result = surfaceDrawable_->CalculateVisibleRegion(*uniParams, *surfaceParams, *surfaceDrawable_, false);
    ASSERT_TRUE(result.IsEmpty());
}

/**
 * @tc.name: PrepareOffscreenRender
 * @tc.desc: Test PrepareOffscreenRender
 * @tc.type: FUNC
 * @tc.require: #IA940V
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, PrepareOffscreenRender, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    std::shared_ptr<Drawing::Surface> surface = Drawing::Surface::MakeRasterN32Premul(100, 100);
    ASSERT_NE(surface, nullptr);
    RSPaintFilterCanvas paintFilterCanvas(surface.get());
    surfaceDrawable_->curCanvas_ = &paintFilterCanvas;
    ASSERT_TRUE(surfaceDrawable_->PrepareOffscreenRender());
}

/**
 * @tc.name: FinishOffscreenRender
 * @tc.desc: Test FinishOffscreenRender
 * @tc.type: FUNC
 * @tc.require: #IA940V
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, FinishOffscreenRender, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    Drawing::SamplingOptions samping;
    surfaceDrawable_->FinishOffscreenRender(samping);
    Drawing::Canvas canvas;
    RSPaintFilterCanvas backupCanvas(&canvas);
    surfaceDrawable_->canvasBackup_ = &backupCanvas;

    Drawing::Canvas canvas2;
    RSPaintFilterCanvas curCanvas(&canvas2);
    surfaceDrawable_->curCanvas_ = &curCanvas;
    surfaceDrawable_->curCanvas_->Save();
    surfaceDrawable_->offscreenSurface_ = Drawing::Surface::MakeRasterN32Premul(100, 100);
    surfaceDrawable_->FinishOffscreenRender(samping);
    ASSERT_NE(surfaceDrawable_->curCanvas_, nullptr);
}

/**
 * @tc.name: DrawUIFirstDfx
 * @tc.desc: Test DrawUIFirstDfx
 * @tc.type: FUNC
 * @tc.require: #IA940V
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, DrawUIFirstDfx, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);

    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);

    surfaceDrawable_->DrawUIFirstDfx(paintFilterCanvas, MultiThreadCacheType::ARKTS_CARD, *surfaceParams, true);
    surfaceDrawable_->DrawUIFirstDfx(paintFilterCanvas, MultiThreadCacheType::LEASH_WINDOW, *surfaceParams, true);
    surfaceDrawable_->DrawUIFirstDfx(paintFilterCanvas, MultiThreadCacheType::LEASH_WINDOW, *surfaceParams, false);
}

/**
 * @tc.name: GetVisibleDirtyRegion
 * @tc.desc: Test GetVisibleDirtyRegion
 * @tc.type: FUNC
 * @tc.require: #IA940V
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, GetVisibleDirtyRegion, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);

    Occlusion::Region region(Occlusion::Rect{0, 0, 100, 100});
    surfaceDrawable_->SetVisibleDirtyRegion(region);
    ASSERT_FALSE(surfaceDrawable_->GetVisibleDirtyRegion().IsEmpty());

    surfaceDrawable_->renderNode_ = std::weak_ptr<const RSRenderNode>();
    ASSERT_TRUE(surfaceDrawable_->GetVisibleDirtyRegion().IsEmpty());
}

/**
 * @tc.name: SetVisibleDirtyRegion
 * @tc.desc: Test SetVisibleDirtyRegion
 * @tc.type: FUNC
 * @tc.require: #IA940V
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, SetVisibleDirtyRegion, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);

    Occlusion::Region region(Occlusion::Rect{0, 0, 100, 100});
    surfaceDrawable_->SetVisibleDirtyRegion(region);
    ASSERT_FALSE(surfaceDrawable_->GetVisibleDirtyRegion().IsEmpty());

    surfaceDrawable_->renderNode_ = std::weak_ptr<const RSRenderNode>();
    surfaceDrawable_->SetVisibleDirtyRegion(region);
}

/**
 * @tc.name: SetAlignedVisibleDirtyRegion
 * @tc.desc: Test SetAlignedVisibleDirtyRegion
 * @tc.type: FUNC
 * @tc.require: #IA940V
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, SetAlignedVisibleDirtyRegion, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);

    Occlusion::Region region(Occlusion::Rect{0, 0, 100, 100});
    surfaceDrawable_->SetAlignedVisibleDirtyRegion(region);

    surfaceDrawable_->renderNode_ = std::weak_ptr<const RSRenderNode>();
    surfaceDrawable_->SetAlignedVisibleDirtyRegion(region);
}

/**
 * @tc.name: SetGlobalDirtyRegion
 * @tc.desc: Test SetGlobalDirtyRegion
 * @tc.type: FUNC
 * @tc.require: #IA940V
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, SetGlobalDirtyRegion, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);

    RectI rect;
    surfaceDrawable_->SetGlobalDirtyRegion(rect);

    surfaceDrawable_->renderNode_ = std::weak_ptr<const RSRenderNode>();
    surfaceDrawable_->SetGlobalDirtyRegion(rect);
}
/**
 * @tc.name: SetDirtyRegionBelowCurrentLayer
 * @tc.desc: Test SetDirtyRegionBelowCurrentLayer
 * @tc.type: FUNC
 * @tc.require: #IA940V
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, SetDirtyRegionBelowCurrentLayer, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    ASSERT_NE(renderNode_, nullptr);

    Occlusion::Region region;
    surfaceDrawable_->SetDirtyRegionBelowCurrentLayer(region);

    surfaceDrawable_->renderNode_ = std::weak_ptr<const RSRenderNode>();
    surfaceDrawable_->SetDirtyRegionBelowCurrentLayer(region);
}

/**
 * @tc.name: GetSyncDirtyManager
 * @tc.desc: Test GetSyncDirtyManager
 * @tc.type: FUNC
 * @tc.require: #IA940V
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, GetSyncDirtyManager, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    ASSERT_NE(renderNode_, nullptr);

    surfaceDrawable_->renderNode_ = std::weak_ptr<const RSRenderNode>();
    surfaceDrawable_->GetSyncDirtyManager();
    ASSERT_EQ(renderNode_, nullptr);
}

/**
 * @tc.name: GetSyncDirtyManager
 * @tc.desc: Test GetSyncDirtyManager
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, GetSyncDirtyManagerTest, TestSize.Level1)
{
    auto renderNode = std::make_shared<RSSurfaceRenderNode>(DEFAULT_ID-1);
    ASSERT_NE(renderNode, nullptr);
    auto drawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(renderNode));
    ASSERT_NE(drawable, nullptr);
    drawable->GetSyncDirtyManager();
    renderNode = nullptr;
    drawable->GetSyncDirtyManager();
}

/**
 * @tc.name: SetDirtyRegionBelowCurrentLayer
 * @tc.desc: Test SetDirtyRegionBelowCurrentLayer
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, SetDirtyRegionBelowCurrentLayerTest, TestSize.Level1)
{
    auto renderNode = std::make_shared<RSSurfaceRenderNode>(DEFAULT_ID-1);
    ASSERT_NE(renderNode, nullptr);
    auto drawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(renderNode));
    ASSERT_NE(drawable, nullptr);
    Occlusion::Region VisibleDirtyRegion;
    drawable->SetDirtyRegionBelowCurrentLayer(VisibleDirtyRegion);
    renderNode = nullptr;
    drawable->SetDirtyRegionBelowCurrentLayer(VisibleDirtyRegion);
}

/**
 * @tc.name: SetDirtyRegionAlignedEnable
 * @tc.desc: Test SetDirtyRegionAlignedEnable
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, SetDirtyRegionAlignedEnableTest, TestSize.Level1)
{
    auto renderNode = std::make_shared<RSSurfaceRenderNode>(DEFAULT_ID-1);
    ASSERT_NE(renderNode, nullptr);
    auto drawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(renderNode));
    ASSERT_NE(drawable, nullptr);
    drawable->SetDirtyRegionAlignedEnable(true);
    renderNode = nullptr;
    drawable->SetDirtyRegionAlignedEnable(true);
}

/**
 * @tc.name: SetGlobalDirtyRegion
 * @tc.desc: Test SetGlobalDirtyRegion
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, SetGlobalDirtyRegionTest, TestSize.Level1)
{
    auto renderNode = std::make_shared<RSSurfaceRenderNode>(DEFAULT_ID-1);
    ASSERT_NE(renderNode, nullptr);
    auto drawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(renderNode));
    ASSERT_NE(drawable, nullptr);
    const RectI rect = RectI(0, 0, 100, 100);
    drawable->SetGlobalDirtyRegion(rect);
    renderNode = nullptr;
    drawable->SetGlobalDirtyRegion(rect);
}

/**
 * @tc.name: GetVisibleDirtyRegion
 * @tc.desc: Test GetVisibleDirtyRegion
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, GetVisibleDirtyRegionTest, TestSize.Level1)
{
    auto renderNode = std::make_shared<RSSurfaceRenderNode>(DEFAULT_ID-1);
    ASSERT_NE(renderNode, nullptr);
    auto drawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(renderNode));
    ASSERT_NE(drawable, nullptr);
    drawable->GetVisibleDirtyRegion();
    renderNode = nullptr;
    drawable->GetVisibleDirtyRegion();
}

/**
 * @tc.name: EnableGpuOverDrawDrawBufferOptimization
 * @tc.desc: Test EnableGpuOverDrawDrawBufferOptimization
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, EnableGpuOverDrawDrawBufferOptimizationTest, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    Drawing::Canvas drawingCanvas;
    surfaceDrawable_->EnableGpuOverDrawDrawBufferOptimization(drawingCanvas, surfaceParams);
}

/**
 * @tc.name: DrawUIFirstDfx
 * @tc.desc: Test DrawUIFirstDfx
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, DrawUIFirstDfxTest, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);

    MultiThreadCacheType enableType = MultiThreadCacheType::ARKTS_CARD;
    surfaceDrawable_->DrawUIFirstDfx(canvas, enableType, *surfaceParams, true);

    enableType = MultiThreadCacheType::LEASH_WINDOW;
    surfaceDrawable_->DrawUIFirstDfx(canvas, enableType, *surfaceParams, true);
    surfaceDrawable_->DrawUIFirstDfx(canvas, enableType, *surfaceParams, false);
}

/**
 * @tc.name: DealWithSelfDrawingNodeBuffer
 * @tc.desc: Test DealWithSelfDrawingNodeBuffer
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, DealWithSelfDrawingNodeBufferTest, TestSize.Level1)
{
    ASSERT_NE(renderNode_, nullptr);
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    auto& rtThread = RSUniRenderThread::Instance();
    rtThread.captureParam_ = CaptureParam();

    surfaceParams->isHardwareEnabled_ = true;
    const auto params01 = surfaceParams;
    surfaceDrawable_->DealWithSelfDrawingNodeBuffer(canvas, *params01);

    rtThread.captureParam_.isSnapshot_ = false;
    surfaceParams->isHardwareEnabled_ = false;
    const auto params02 = surfaceParams;
    surfaceDrawable_->DealWithSelfDrawingNodeBuffer(canvas, *params02);
}
}
