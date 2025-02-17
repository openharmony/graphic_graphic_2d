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
#include "pipeline/render_thread/rs_uni_render_engine.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_surface_render_node.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::DrawableV2;

namespace OHOS::Rosen {
constexpr int32_t DEFAULT_CANVAS_SIZE = 100;
constexpr NodeId DEFAULT_ID = 0xFFFF;

class RSRenderDrawableTest : public testing::Test {
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

void RSRenderDrawableTest::SetUpTestCase() {}
void RSRenderDrawableTest::TearDownTestCase() {}
void RSRenderDrawableTest::SetUp()
{
    renderNode_ = std::make_shared<RSSurfaceRenderNode>(DEFAULT_ID);
    if (!renderNode_) {
        RS_LOGE("RSRenderDrawableTest: failed to create surface node.");
    }
    drawable_ = RSSurfaceRenderNodeDrawable::OnGenerate(renderNode_);
    if (drawable_) {
        drawable_->renderParams_ = std::make_unique<RSSurfaceRenderParams>(DEFAULT_ID);
        surfaceDrawable_ = static_cast<RSSurfaceRenderNodeDrawable*>(drawable_);
        if (!drawable_->renderParams_) {
            RS_LOGE("RSRenderDrawableTest: failed to init render params.");
        }
    }
    drawingCanvas_ = std::make_unique<Drawing::Canvas>(DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE);
    if (drawingCanvas_) {
        canvas_ = std::make_shared<RSPaintFilterCanvas>(drawingCanvas_.get());
    }
}
void RSRenderDrawableTest::TearDown() {}

/**
 * @tc.name: OnDrawExtTest
 * @tc.desc: Test OnDraw
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSRenderDrawableTest, OnDrawExtTest, TestSize.Level1)
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
 * @tc.name: CreateSurfaceRenderNodeDrawableExtTest
 * @tc.desc: Test If SurfaceRenderNodeDrawable Can Be Created
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSRenderDrawableTest, CreateSurfaceRenderNodeDrawableExtTest, TestSize.Level1)
{
    NodeId id = 1;
    auto renderNode = std::make_shared<RSSurfaceRenderNode>(id);
    auto drawable = RSSurfaceRenderNodeDrawable::OnGenerate(renderNode);
    ASSERT_NE(drawable, nullptr);
}

/**
 * @tc.name: MergeDirtyRegionBelowCurSurfaceExtTest
 * @tc.desc: Test MergeDirtyRegionBelowCurSurface, dirty region expected not empty.
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSRenderDrawableTest, MergeDirtyRegionBelowCurSurfaceExtTest001, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto uniParams = std::make_shared<RSRenderThreadParams>();
    ASSERT_NE(uniParams, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    Drawing::Region region;
    surfaceParams->isMainWindowType_ = true;
    surfaceDrawable_->MergeDirtyRegionBelowCurSurface(*uniParams, region);
    ASSERT_TRUE(surfaceParams->IsMainWindowType());
}

/**
 * @tc.name: MergeDirtyRegionBelowCurSurfaceExtTest
 * @tc.desc: Test MergeDirtyRegionBelowCurSurface, default case, empty dirty region
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSRenderDrawableTest, MergeDirtyRegionBelowCurSurfaceExtTest002, TestSize.Level1)
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
 * @tc.name: MergeDirtyRegionBelowCurSurfaceExtTest
 * @tc.desc: Test MergeDirtyRegionBelowCurSurface, dirty region expected not empty.
 * @tc.type: FUNC
 * @tc.require: issueIAEDYI
 */
HWTEST_F(RSRenderDrawableTest, MergeDirtyRegionBelowCurSurfaceExtTest003, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto uniParams = std::make_shared<RSRenderThreadParams>();
    ASSERT_NE(uniParams, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    Drawing::Region region;
    surfaceParams->isMainWindowType_ = true;
    Occlusion::Rect rect(1, 2, 3, 4);
    Occlusion::Rect bound(1, 2, 3, 4);
    surfaceParams->visibleRegion_.rects_.push_back(rect);
    surfaceParams->visibleRegion_.bound_ = bound;
    surfaceDrawable_->MergeDirtyRegionBelowCurSurface(*uniParams, region);
    ASSERT_FALSE(surfaceParams->GetVisibleRegion().IsEmpty());
}

/**
 * @tc.name: MergeDirtyRegionBelowCurSurfaceExtTest
 * @tc.desc: Test MergeDirtyRegionBelowCurSurface, dirty region expected not empty.
 * @tc.type: FUNC
 * @tc.require: issueIAEDYI
 */
HWTEST_F(RSRenderDrawableTest, MergeDirtyRegionBelowCurSurfaceExtTest004, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto uniParams = std::make_shared<RSRenderThreadParams>();
    ASSERT_NE(uniParams, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    Drawing::Region region;
    surfaceParams->isMainWindowType_ = false;
    surfaceParams->isLeashWindow_ = true;
    surfaceDrawable_->MergeDirtyRegionBelowCurSurface(*uniParams, region);
    ASSERT_TRUE(surfaceParams->GetVisibleRegion().IsEmpty());
}

/**
 * @tc.name: MergeDirtyRegionBelowCurSurfaceExtTest
 * @tc.desc: Test MergeDirtyRegionBelowCurSurface, dirty region expected not empty.
 * @tc.type: FUNC
 * @tc.require: issueIAEDYI
 */
HWTEST_F(RSRenderDrawableTest, MergeDirtyRegionBelowCurSurfaceExtTest005, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto uniParams = std::make_shared<RSRenderThreadParams>();
    ASSERT_NE(uniParams, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    Drawing::Region region;
    surfaceParams->isMainWindowType_ = true;
    surfaceParams->isLeashWindow_ = true;
    surfaceParams->isParentScaling_ = false;
    surfaceParams->isSubSurfaceNode_ = true;
    uniParams->accumulatedDirtyRegion_ = Occlusion::Rect { 0, 0, DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE };
    surfaceParams->visibleRegion_ = Occlusion::Rect { 0, 0, DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE };
    Occlusion::Rect rect(1, 2, 3, 4);
    Occlusion::Rect bound(1, 2, 3, 4);
    surfaceParams->visibleRegion_.rects_.push_back(rect);
    surfaceParams->visibleRegion_.bound_ = bound;
    surfaceParams->uiFirstFlag_ = MultiThreadCacheType::ARKTS_CARD;
    surfaceDrawable_->MergeDirtyRegionBelowCurSurface(*uniParams, region);
    ASSERT_FALSE(surfaceParams->GetVisibleRegion().IsEmpty());
}

/**
 * @tc.name: MergeDirtyRegionBelowCurSurfaceExtTest
 * @tc.desc: Test MergeDirtyRegionBelowCurSurface, dirty region expected not empty.
 * @tc.type: FUNC
 * @tc.require: issueIAEDYI
 */
HWTEST_F(RSRenderDrawableTest, MergeDirtyRegionBelowCurSurfaceExtTest006, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto uniParams = std::make_shared<RSRenderThreadParams>();
    ASSERT_NE(uniParams, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    Drawing::Region region;
    surfaceParams->isMainWindowType_ = true;
    surfaceParams->isLeashWindow_ = true;
    surfaceParams->isParentScaling_ = true;
    surfaceParams->isSubSurfaceNode_ = true;
    Occlusion::Rect rect(1, 2, 3, 4);
    Occlusion::Rect bound(1, 2, 3, 4);
    surfaceParams->visibleRegion_.rects_.push_back(rect);
    surfaceParams->visibleRegion_.bound_ = bound;
    ASSERT_FALSE(surfaceParams->GetVisibleRegion().IsEmpty());
    uniParams->accumulatedDirtyRegion_ = Occlusion::Rect { 0, 0, DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE };
    surfaceDrawable_->MergeDirtyRegionBelowCurSurface(*uniParams, region);
    ASSERT_FALSE(surfaceParams->GetVisibleRegion().IsEmpty());
}

/**
 * @tc.name: MergeDirtyRegionBelowCurSurfaceExtTest
 * @tc.desc: Test MergeDirtyRegionBelowCurSurface, dirty region expected not empty.
 * @tc.type: FUNC
 * @tc.require: issueIAEDYI
 */
HWTEST_F(RSRenderDrawableTest, MergeDirtyRegionBelowCurSurfaceExtTest007, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto uniParams = std::make_shared<RSRenderThreadParams>();
    ASSERT_NE(uniParams, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    Drawing::Region region;
    surfaceParams->isMainWindowType_ = false;
    surfaceParams->isLeashWindow_ = true;
    surfaceParams->isParentScaling_ = false;
    surfaceParams->isSubSurfaceNode_ = true;
    uniParams->accumulatedDirtyRegion_ = Occlusion::Rect { 0, 0, DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE };
    surfaceParams->visibleRegion_ = Occlusion::Rect { 0, 0, DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE };
    Occlusion::Rect rect(1, 2, 3, 4);
    Occlusion::Rect bound(1, 2, 3, 4);
    surfaceParams->visibleRegion_.rects_.push_back(rect);
    surfaceParams->visibleRegion_.bound_ = bound;
    uniParams->accumulatedDirtyRegion_.rects_.push_back(rect);
    uniParams->accumulatedDirtyRegion_.bound_ = bound;
    surfaceDrawable_->MergeDirtyRegionBelowCurSurface(*uniParams, region);
    ASSERT_FALSE(surfaceParams->GetVisibleRegion().IsEmpty());
}

/**
 * @tc.name: MergeDirtyRegionBelowCurSurfaceExtTest
 * @tc.desc: Test MergeDirtyRegionBelowCurSurface, dirty region expected not empty.
 * @tc.type: FUNC
 * @tc.require: issueIAEDYI
 */
HWTEST_F(RSRenderDrawableTest, MergeDirtyRegionBelowCurSurfaceExtTest008, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto uniParams = std::make_shared<RSRenderThreadParams>();
    ASSERT_NE(uniParams, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    Drawing::Region region;
    surfaceParams->isMainWindowType_ = false;
    surfaceParams->isLeashWindow_ = true;
    surfaceParams->isParentScaling_ = true;
    surfaceParams->isSubSurfaceNode_ = false;
    uniParams->accumulatedDirtyRegion_ = Occlusion::Rect { 0, 0, DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE };
    surfaceParams->visibleRegion_ = Occlusion::Rect { 0, 0, DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE };
    surfaceDrawable_->MergeDirtyRegionBelowCurSurface(*uniParams, region);
    ASSERT_FALSE(surfaceParams->GetVisibleRegion().IsEmpty());
}

/**
 * @tc.name: MergeDirtyRegionBelowCurSurfaceExtTest
 * @tc.desc: Test MergeDirtyRegionBelowCurSurface, dirty region expected not empty.
 * @tc.type: FUNC
 * @tc.require: issueIAEDYI
 */
HWTEST_F(RSRenderDrawableTest, MergeDirtyRegionBelowCurSurfaceExtTest009, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto uniParams = std::make_shared<RSRenderThreadParams>();
    ASSERT_NE(uniParams, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    Drawing::Region region;
    surfaceParams->isMainWindowType_ = false;
    surfaceParams->isLeashWindow_ = true;
    surfaceParams->isParentScaling_ = false;
    surfaceParams->isSubSurfaceNode_ = false;
    uniParams->accumulatedDirtyRegion_ = Occlusion::Rect{0, 0, DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE};
    surfaceParams->transparentRegion_ = Occlusion::Rect{0, 0, DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE};
    surfaceParams->SetVisibleRegion(Occlusion::Rect{0, 0, DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE});
    Occlusion::Rect rect(1, 2, 3, 4);
    Occlusion::Rect bound(1, 2, 3, 4);
    surfaceParams->transparentRegion_.rects_.push_back(rect);
    surfaceParams->transparentRegion_.bound_ = bound;
    uniParams->accumulatedDirtyRegion_.rects_.push_back(rect);
    uniParams->accumulatedDirtyRegion_.bound_ = bound;
    surfaceDrawable_->MergeDirtyRegionBelowCurSurface(*uniParams, region);
    ASSERT_FALSE(surfaceParams->GetTransparentRegion().IsEmpty());
    ASSERT_TRUE(region.IsEmpty());
}

/**
 * @tc.name: CaptureSurfaceSingleTest
 * @tc.desc: Test CaptureSurface, special case: security/protected layer.
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSRenderDrawableTest, CaptureSurfaceSingleTest001, TestSize.Level1)
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
 * @tc.name: CaptureSurfaceSingleTest
 * @tc.desc: Test CaptureSurface, default case
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSRenderDrawableTest, CaptureSurfaceSingleTest002, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    surfaceParams->rsSurfaceNodeType_ = RSSurfaceNodeType::SELF_DRAWING_NODE;
    surfaceParams->isSpherizeValid_ = false;
    surfaceParams->isAttractionValid_ = false;
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    surfaceDrawable_->hasHdrPresent_ = true;
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    ASSERT_TRUE(!surfaceParams->IsAttractionValid());

    surfaceParams->protectedLayerIds_.insert(1);
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    surfaceParams->skipLayerIds_.insert(1);
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    surfaceParams->securityLayerIds_.insert(1);
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    ASSERT_TRUE(surfaceParams->HasSkipLayer());
    RSUniRenderThread::Instance().renderThreadParams_ = std::make_unique<RSRenderThreadParams>();
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);

    surfaceParams->isProtectedLayer_ = true;
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    surfaceParams->isSkipLayer_ = true;
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    surfaceParams->isSecurityLayer_ = true;
    CaptureParam param;
    param.isSingleSurface_ = true;
    RSUniRenderThread::SetCaptureParam(param);
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    ASSERT_TRUE(surfaceParams->GetIsSecurityLayer());
}

#ifdef USE_VIDEO_PROCESSING_ENGINE
/**
 * @tc.name: DealWithHdrExtTest
 * @tc.desc: Test DealWithHdr
 * @tc.type: FUNC
 * @tc.require: issueIAEDYI
 */
HWTEST_F(RSRenderDrawableTest, DealWithHdrExtTest, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    surfaceDrawable_->DealWithHdr(*renderNode_, *surfaceParams);
}
#endif


/**
 * @tc.name: OnCaptureSingleTest
 * @tc.desc: Test OnCapture
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSRenderDrawableTest, OnCaptureSingleTest, TestSize.Level1)
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
 * @tc.name: PrepareOffscreenRenderExtTest
 * @tc.desc: Test PrepareOffscreenRender
 * @tc.type: FUNC
 * @tc.require: #IA940V
 */
HWTEST_F(RSRenderDrawableTest, PrepareOffscreenRenderExtTest, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    std::shared_ptr<Drawing::Surface> surface = Drawing::Surface::MakeRasterN32Premul(100, 100);
    ASSERT_NE(surface, nullptr);
    RSPaintFilterCanvas paintFilterCanvas(surface.get());
    surfaceDrawable_->curCanvas_ = &paintFilterCanvas;
    ASSERT_TRUE(surfaceDrawable_->PrepareOffscreenRender());

    surfaceDrawable_->curCanvas_->surface_ = nullptr;
    ASSERT_FALSE(surfaceDrawable_->PrepareOffscreenRender());
}

/**
 * @tc.name: CalculateVisibleDirtyRegionExtTest
 * @tc.desc: Test CalculateVisibleDirtyRegion
 * @tc.type: FUNC
 * @tc.require: #IA940V
 */
HWTEST_F(RSRenderDrawableTest, CalculateVisibleDirtyRegionExtTest, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    auto uniParams = std::make_shared<RSRenderThreadParams>();

    surfaceParams->isMainWindowType_ = false;
    surfaceParams->isLeashWindow_ = true;
    surfaceParams->isAppWindow_ = false;
    Drawing::Region result = surfaceDrawable_->CalculateVisibleDirtyRegion(*uniParams,
        *surfaceParams, *surfaceDrawable_, true);
    ASSERT_TRUE(result.IsEmpty());

    surfaceParams->isMainWindowType_ = true;
    surfaceParams->isLeashWindow_ = false;
    surfaceParams->isAppWindow_ = false;
    result = surfaceDrawable_->CalculateVisibleDirtyRegion(*uniParams, *surfaceParams, *surfaceDrawable_, true);
    ASSERT_FALSE(result.IsEmpty());

    uniParams->SetOcclusionEnabled(true);
    Occlusion::Region region;
    surfaceParams->SetVisibleRegion(region);
    result = surfaceDrawable_->CalculateVisibleDirtyRegion(*uniParams, *surfaceParams, *surfaceDrawable_, false);
    ASSERT_TRUE(result.IsEmpty());
}

/**
 * @tc.name: PrepareOffscreenRender
 * @tc.desc: Test PrepareOffscreenRender
 * @tc.type: FUNC
 * @tc.require: issueIAEDYI
 */
HWTEST_F(RSRenderDrawableTest, PrepareOffscreenRenderSingleTest001, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    std::shared_ptr<Drawing::Surface> surface = Drawing::Surface::MakeRasterN32Premul(100, 100);
    ASSERT_NE(surface, nullptr);
    RSPaintFilterCanvas paintFilterCanvas(surface.get());
    surfaceDrawable_->curCanvas_ = &paintFilterCanvas;
    surfaceDrawable_->maxRenderSize_ = 200; // for test
    surfaceDrawable_->offscreenSurface_ = nullptr;
    ASSERT_TRUE(surfaceDrawable_->PrepareOffscreenRender());
    ASSERT_TRUE(surfaceDrawable_->curCanvas_->GetSurface());
}

/**
 * @tc.name: PrepareOffscreenRenderSingleTest
 * @tc.desc: Test PrepareOffscreenRender
 * @tc.type: FUNC
 * @tc.require: issueIAEDYI
 */
HWTEST_F(RSRenderDrawableTest, PrepareOffscreenRenderSingleTest002, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    std::shared_ptr<Drawing::Surface> surface = Drawing::Surface::MakeRasterN32Premul(100, 100);
    ASSERT_NE(surface, nullptr);
    RSPaintFilterCanvas paintFilterCanvas(surface.get());
    surfaceDrawable_->curCanvas_ = &paintFilterCanvas;
    surfaceDrawable_->offscreenSurface_ = nullptr;
    ASSERT_TRUE(surfaceDrawable_->PrepareOffscreenRender());
    ASSERT_TRUE(surfaceDrawable_->curCanvas_->GetSurface());
}

/**
 * @tc.name: FinishOffscreenRender
 * @tc.desc: Test FinishOffscreenRender
 * @tc.type: FUNC
 * @tc.require: #IA940V
 */
HWTEST_F(RSRenderDrawableTest, FinishOffscreenRenderExtTest, TestSize.Level1)
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
 * @tc.name: IsHardwareEnabledExtTest
 * @tc.desc: Test IsHardwareEnabled
 * @tc.type: FUNC
 * @tc.require: issueIAEDYI
 */
HWTEST_F(RSRenderDrawableTest, IsHardwareEnabledExtTest, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    RSUniRenderThread::Instance().renderThreadParams_ = std::make_unique<RSRenderThreadParams>();
    ASSERT_FALSE(surfaceDrawable_->IsHardwareEnabled());

    auto nodePtr = std::make_shared<RSRenderNode>(0);
    ASSERT_NE(nodePtr, nullptr);
    auto rsSurfaceRenderNode = DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(nodePtr);
    RSUniRenderThread::Instance().renderThreadParams_->hardwareEnabledTypeDrawables_.push_back(rsSurfaceRenderNode);
    ASSERT_FALSE(surfaceDrawable_->IsHardwareEnabled());

    auto rsRenderNode = std::make_shared<RSRenderNode>(0);
    ASSERT_NE(rsRenderNode, nullptr);
    auto surfaceRenderNode = DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(rsRenderNode);
    surfaceRenderNode->renderParams_ = std::make_unique<RSRenderParams>(0);
    RSUniRenderThread::Instance().renderThreadParams_->hardwareEnabledTypeDrawables_.push_back(surfaceRenderNode);
    ASSERT_FALSE(surfaceDrawable_->IsHardwareEnabled());
}

/**
 * @tc.name: GetVisibleDirtyRegion
 * @tc.desc: Test GetVisibleDirtyRegion
 * @tc.type: FUNC
 * @tc.require: #IA940V
 */
HWTEST_F(RSRenderDrawableTest, GetVisibleDirtyRegionExtTest, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);

    Occlusion::Region region(Occlusion::Rect{0, 0, 100, 100});
    surfaceDrawable_->SetVisibleDirtyRegion(region);
    ASSERT_FALSE(surfaceDrawable_->GetVisibleDirtyRegion().IsEmpty());
}

/**
 * @tc.name: DrawUIFirstDfxExtTest
 * @tc.desc: Test DrawUIFirstDfx
 * @tc.type: FUNC
 * @tc.require: #IA940V
 */
HWTEST_F(RSRenderDrawableTest, DrawUIFirstDfxExtTest, TestSize.Level1)
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
 * @tc.name: SetVisibleDirtyRegionSingleTest
 * @tc.desc: Test SetVisibleDirtyRegion
 * @tc.type: FUNC
 * @tc.require: #IA940V
 */
HWTEST_F(RSRenderDrawableTest, SetVisibleDirtyRegionSingleTest, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);

    Occlusion::Region region(Occlusion::Rect{0, 0, 100, 100});
    surfaceDrawable_->SetVisibleDirtyRegion(region);
    ASSERT_FALSE(surfaceDrawable_->GetVisibleDirtyRegion().IsEmpty());

    surfaceDrawable_->renderNode_ = std::weak_ptr<const RSRenderNode>();
    surfaceDrawable_->SetVisibleDirtyRegion(region);
}
}
