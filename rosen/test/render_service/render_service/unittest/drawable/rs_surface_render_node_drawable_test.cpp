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
#include "pipeline/render_thread/rs_render_engine.h"
#include "pipeline/render_thread/rs_uni_render_engine.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "params/rs_render_thread_params.h"
#include "gfx/fps_info/rs_surface_fps_manager.h"

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
    static inline Occlusion::Rect DEFAULT_RECT{0, 80, 500, 500};
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
    surfaceDrawable_->OnCapture(*canvas_);
    // should paint
    drawable_->renderParams_->shouldPaint_ = true;
    drawable_->renderParams_->contentEmpty_ = false;
    surfaceDrawable_->OnCapture(*canvas_);
}

/**
 * @tc.name: CaptureSurface001
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
    RSRenderThreadParamsManager::Instance().renderThreadParams_ = std::make_unique<RSRenderThreadParams>();
    auto& uniParams = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    ASSERT_NE(uniParams, nullptr);

    surfaceParams->rsSurfaceNodeType_ = RSSurfaceNodeType::SELF_DRAWING_NODE;
    surfaceParams->isSpherizeValid_ = false;
    surfaceParams->isAttractionValid_ = false;
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    surfaceDrawable_->hasHdrPresent_ = true;
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    ASSERT_TRUE(!surfaceParams->IsAttractionValid());

    surfaceParams->GetMultableSpecialLayerMgr().AddIds(SpecialLayerType::PROTECTED, 1);
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    surfaceParams->GetMultableSpecialLayerMgr().AddIds(SpecialLayerType::SKIP, 1);
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    surfaceParams->GetMultableSpecialLayerMgr().AddIds(SpecialLayerType::SNAPSHOT_SKIP, 1);
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    surfaceParams->GetMultableSpecialLayerMgr().AddIds(SpecialLayerType::SECURITY, 1);
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    uniParams->SetSecExemption(true);
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    uniParams->SetSecExemption(false);
    ASSERT_TRUE(surfaceParams->GetSpecialLayerMgr().Find(SpecialLayerType::HAS_SKIP));
    ASSERT_TRUE(surfaceParams->GetSpecialLayerMgr().Find(SpecialLayerType::HAS_SNAPSHOT_SKIP));
    ASSERT_TRUE(surfaceParams->GetSpecialLayerMgr().Find(SpecialLayerType::HAS_PROTECTED));
    ASSERT_TRUE(surfaceParams->GetSpecialLayerMgr().Find(SpecialLayerType::HAS_SECURITY));
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);

    RSUniRenderThread::SetCaptureParam(CaptureParam());
    RSRenderThreadParamsManager::Instance().renderThreadParams_ = std::make_unique<RSRenderThreadParams>();
}

#ifdef USE_VIDEO_PROCESSING_ENGINE
/**
 * @tc.name: DealWithHdr
 * @tc.desc: Test DealWithHdr
 * @tc.type: FUNC
 * @tc.require: issueIAEDYI
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, DealWithHdr, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    surfaceDrawable_->DealWithHdr(*renderNode_, *surfaceParams);
}
#endif

/**
 * @tc.name: CaptureSurface002
 * @tc.desc: Test CaptureSurface, special case: security/protected layer.
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, CaptureSurface002, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    surfaceParams->GetMultableSpecialLayerMgr().Set(SpecialLayerType::SECURITY, true);
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    surfaceParams->GetMultableSpecialLayerMgr().Set(SpecialLayerType::SECURITY, false);
    surfaceParams->GetMultableSpecialLayerMgr().Set(SpecialLayerType::PROTECTED, true);
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
}

/**
 * @tc.name: CaptureSurface003
 * @tc.desc: Test CaptureSurface, special layer cases for screen mirrorring.
 * @tc.type: FUNC
 * @tc.require: #IBNN3I
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, CaptureSurface003, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    RSRenderThreadParamsManager::Instance().renderThreadParams_ = std::make_unique<RSRenderThreadParams>();
    auto& uniParams = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    ASSERT_NE(uniParams, nullptr);

    surfaceParams->GetMultableSpecialLayerMgr().Set(SpecialLayerType::SKIP, true);
    ASSERT_TRUE(surfaceParams->GetSpecialLayerMgr().Find(SpecialLayerType::SKIP));
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    surfaceParams->GetMultableSpecialLayerMgr().Set(SpecialLayerType::SNAPSHOT_SKIP, true);
    ASSERT_TRUE(surfaceParams->GetSpecialLayerMgr().Find(SpecialLayerType::SNAPSHOT_SKIP));
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    surfaceParams->GetMultableSpecialLayerMgr().Set(SpecialLayerType::PROTECTED, true);
    ASSERT_TRUE(surfaceParams->GetSpecialLayerMgr().Find(SpecialLayerType::PROTECTED));
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    surfaceParams->GetMultableSpecialLayerMgr().Set(SpecialLayerType::SECURITY, true);
    ASSERT_TRUE(surfaceParams->GetSpecialLayerMgr().Find(SpecialLayerType::SECURITY));
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    uniParams->SetSecExemption(true);
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    uniParams->SetSecExemption(false);
    surfaceParams->GetMultableSpecialLayerMgr().Set(SpecialLayerType::SKIP, false);
    surfaceParams->GetMultableSpecialLayerMgr().Set(SpecialLayerType::SNAPSHOT_SKIP, false);
    surfaceParams->GetMultableSpecialLayerMgr().Set(SpecialLayerType::SECURITY, false);
    surfaceParams->GetMultableSpecialLayerMgr().Set(SpecialLayerType::PROTECTED, false);
    ASSERT_FALSE(surfaceParams->GetSpecialLayerMgr().Find(SpecialLayerType::SKIP));
    ASSERT_FALSE(surfaceParams->GetSpecialLayerMgr().Find(SpecialLayerType::SNAPSHOT_SKIP));
    ASSERT_FALSE(surfaceParams->GetSpecialLayerMgr().Find(SpecialLayerType::SECURITY));
    ASSERT_FALSE(surfaceParams->GetSpecialLayerMgr().Find(SpecialLayerType::PROTECTED));

    RSUniRenderThread::SetCaptureParam(CaptureParam());
    RSRenderThreadParamsManager::Instance().renderThreadParams_ = std::make_unique<RSRenderThreadParams>();
}

/**
 * @tc.name: CaptureSurface004
 * @tc.desc: Test CaptureSurface, special layer cases for capture single surface.
 * @tc.type: FUNC
 * @tc.require: #IBNN3I
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, CaptureSurface004, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    RSRenderThreadParamsManager::Instance().renderThreadParams_ = std::make_unique<RSRenderThreadParams>();
    auto& uniParams = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    ASSERT_NE(uniParams, nullptr);

    CaptureParam captureParam;
    captureParam.isSingleSurface_ = true;
    RSUniRenderThread::SetCaptureParam(captureParam);
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    surfaceParams->GetMultableSpecialLayerMgr().Set(SpecialLayerType::SKIP, true);
    ASSERT_TRUE(surfaceParams->GetSpecialLayerMgr().Find(SpecialLayerType::SKIP));
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    surfaceParams->GetMultableSpecialLayerMgr().Set(SpecialLayerType::SNAPSHOT_SKIP, true);
    ASSERT_TRUE(surfaceParams->GetSpecialLayerMgr().Find(SpecialLayerType::SNAPSHOT_SKIP));
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    surfaceParams->GetMultableSpecialLayerMgr().Set(SpecialLayerType::SECURITY, true);
    ASSERT_TRUE(surfaceParams->GetSpecialLayerMgr().Find(SpecialLayerType::SECURITY));
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    uniParams->SetSecExemption(true);
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    uniParams->SetSecExemption(false);
    surfaceParams->GetMultableSpecialLayerMgr().Set(SpecialLayerType::SKIP, false);
    surfaceParams->GetMultableSpecialLayerMgr().Set(SpecialLayerType::SNAPSHOT_SKIP, false);
    surfaceParams->GetMultableSpecialLayerMgr().Set(SpecialLayerType::SECURITY, false);
    ASSERT_FALSE(surfaceParams->GetSpecialLayerMgr().Find(SpecialLayerType::SKIP));
    ASSERT_FALSE(surfaceParams->GetSpecialLayerMgr().Find(SpecialLayerType::SNAPSHOT_SKIP));
    ASSERT_FALSE(surfaceParams->GetSpecialLayerMgr().Find(SpecialLayerType::SECURITY));

    captureParam.isNeedBlur_ = true;
    RSUniRenderThread::SetCaptureParam(captureParam);
    surfaceParams->GetMultableSpecialLayerMgr().Set(SpecialLayerType::SECURITY, true);
    ASSERT_TRUE(surfaceParams->GetSpecialLayerMgr().Find(SpecialLayerType::SECURITY));
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    uniParams->SetSecExemption(true);
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    uniParams->SetSecExemption(false);
    surfaceParams->GetMultableSpecialLayerMgr().Set(SpecialLayerType::SECURITY, false);
    ASSERT_FALSE(surfaceParams->GetSpecialLayerMgr().Find(SpecialLayerType::SECURITY));

    RSUniRenderThread::SetCaptureParam(CaptureParam());
    RSRenderThreadParamsManager::Instance().renderThreadParams_ = std::make_unique<RSRenderThreadParams>();
}


/**
 * @tc.name: CaptureSurface005
 * @tc.desc: Test CaptureSurface, special layer cases for screenshot.
 * @tc.type: FUNC
 * @tc.require: #IBNN3I
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, CaptureSurface005, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    RSRenderThreadParamsManager::Instance().renderThreadParams_ = std::make_unique<RSRenderThreadParams>();
    auto& uniParams = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    ASSERT_NE(uniParams, nullptr);

    CaptureParam captureParam;
    captureParam.isSnapshot_ = true;
    captureParam.isSingleSurface_ = false;
    RSUniRenderThread::SetCaptureParam(captureParam);
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    surfaceParams->GetMultableSpecialLayerMgr().Set(SpecialLayerType::SKIP, true);
    ASSERT_TRUE(surfaceParams->GetSpecialLayerMgr().Find(SpecialLayerType::SKIP));
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    surfaceParams->GetMultableSpecialLayerMgr().Set(SpecialLayerType::SNAPSHOT_SKIP, true);
    ASSERT_TRUE(surfaceParams->GetSpecialLayerMgr().Find(SpecialLayerType::SNAPSHOT_SKIP));
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    surfaceParams->GetMultableSpecialLayerMgr().Set(SpecialLayerType::SECURITY, true);
    ASSERT_TRUE(surfaceParams->GetSpecialLayerMgr().Find(SpecialLayerType::SECURITY));
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    uniParams->SetSecExemption(true);
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    uniParams->SetSecExemption(false);
    surfaceParams->GetMultableSpecialLayerMgr().Set(SpecialLayerType::SKIP, false);
    surfaceParams->GetMultableSpecialLayerMgr().Set(SpecialLayerType::SNAPSHOT_SKIP, false);
    surfaceParams->GetMultableSpecialLayerMgr().Set(SpecialLayerType::SECURITY, false);
    ASSERT_FALSE(surfaceParams->GetSpecialLayerMgr().Find(SpecialLayerType::SKIP));
    ASSERT_FALSE(surfaceParams->GetSpecialLayerMgr().Find(SpecialLayerType::SNAPSHOT_SKIP));
    ASSERT_FALSE(surfaceParams->GetSpecialLayerMgr().Find(SpecialLayerType::SECURITY));

    RSUniRenderThread::SetCaptureParam(CaptureParam());
    RSRenderThreadParamsManager::Instance().renderThreadParams_ = std::make_unique<RSRenderThreadParams>();
}

/**
 * @tc.name: CrossDisplaySurfaceDirtyRegionConversion
 * @tc.desc: Test CrossDisplaySurfaceDirtyRegionConversion, if node is cross-display, the surface dirty will be offset.
 * @tc.type: FUNC
 * @tc.require: issueIB670G
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, CrossDisplaySurfaceDirtyRegionConversion_001, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    ASSERT_NE(surfaceDrawable_->GetRenderParams(), nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    auto uniParams = std::make_shared<RSRenderThreadParams>();
    ASSERT_NE(uniParams, nullptr);

    NodeId curDisplayId = 1;
    Drawing::Matrix matrix;
    matrix.PreTranslate(DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE);
    surfaceParams->crossNodeSkipDisplayConversionMatrices_.insert({curDisplayId, matrix});
    uniParams->SetCurrentVisitDisplayDrawableId(curDisplayId);
    RectI surfaceDirtyRect = { DEFAULT_RECT.left_, DEFAULT_RECT.top_,
        DEFAULT_RECT.right_ - DEFAULT_RECT.left_, DEFAULT_RECT.bottom_ - DEFAULT_RECT.top_ };;

    // if surface node is not cross-display node, nothing will happen.
    surfaceDrawable_->GetRenderParams()->SetFirstLevelCrossNode(false);
    surfaceDrawable_->CrossDisplaySurfaceDirtyRegionConversion(*uniParams, *surfaceParams, surfaceDirtyRect);
    ASSERT_EQ(surfaceDirtyRect.GetTop(), DEFAULT_RECT.top_);
    // if surface node is not cross-display node, the surface dirty region will be offset.
    surfaceDrawable_->GetRenderParams()->SetFirstLevelCrossNode(true);
    surfaceDrawable_->CrossDisplaySurfaceDirtyRegionConversion(*uniParams, *surfaceParams, surfaceDirtyRect);
    ASSERT_NE(surfaceDirtyRect.GetTop(), DEFAULT_RECT.top_);
}

/**
 * @tc.name: CalculateVisibleDirtyRegion
 * @tc.desc: Test CalculateVisibleDirtyRegion
 * @tc.type: FUNC
 * @tc.require: #IA940V
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, CalculateVisibleDirtyRegion, TestSize.Level1)
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

    Occlusion::Region region1(DEFAULT_RECT);
    surfaceParams->SetVisibleRegion(region1);
    uniParams->SetOcclusionEnabled(false);
    surfaceDrawable_->globalDirtyRegion_ = region1;
    surfaceDrawable_->CalculateVisibleDirtyRegion(*uniParams, *surfaceParams, *surfaceDrawable_, false);
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
    surfaceDrawable_->offscreenSurface_ = std::make_shared<Drawing::Surface>();
    ASSERT_TRUE(surfaceDrawable_->PrepareOffscreenRender());

    surfaceDrawable_->curCanvas_->surface_ = nullptr;
    ASSERT_FALSE(surfaceDrawable_->PrepareOffscreenRender());
}

/**
 * @tc.name: PrepareOffscreenRender
 * @tc.desc: Test PrepareOffscreenRender
 * @tc.type: FUNC
 * @tc.require: issueIAEDYI
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, PrepareOffscreenRenderTest001, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    std::shared_ptr<Drawing::Surface> surface = Drawing::Surface::MakeRasterN32Premul(100, 100);
    ASSERT_NE(surface, nullptr);
    RSPaintFilterCanvas paintFilterCanvas(surface.get());
    surfaceDrawable_->curCanvas_ = &paintFilterCanvas;
    surfaceDrawable_->offscreenSurface_ = std::make_shared<Drawing::Surface>();
    ASSERT_TRUE(surfaceDrawable_->PrepareOffscreenRender());
    ASSERT_TRUE(surfaceDrawable_->curCanvas_->GetSurface());
}

/**
 * @tc.name: PrepareOffscreenRender
 * @tc.desc: Test PrepareOffscreenRender
 * @tc.type: FUNC
 * @tc.require: issueIAEDYI
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, PrepareOffscreenRenderTest002, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    std::shared_ptr<Drawing::Surface> surface = Drawing::Surface::MakeRasterN32Premul(100, 100);
    ASSERT_NE(surface, nullptr);
    RSPaintFilterCanvas paintFilterCanvas(surface.get());
    surfaceDrawable_->curCanvas_ = &paintFilterCanvas;
    surfaceDrawable_->maxRenderSize_ = 200; // for test
    surfaceDrawable_->offscreenSurface_ = std::make_shared<Drawing::Surface>();
    ASSERT_TRUE(surfaceDrawable_->PrepareOffscreenRender());
    ASSERT_TRUE(surfaceDrawable_->curCanvas_->GetSurface());
}

/**
 * @tc.name: BufferFormatNeedUpdate
 * @tc.desc: Test BufferFormatNeedUpdate
 * @tc.type: FUNC
 * @tc.require: issueIAEDYI
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, BufferFormatNeedUpdateTest, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    std::shared_ptr<Drawing::Surface> surface = Drawing::Surface::MakeRasterN32Premul(100, 100);
    ASSERT_NE(surface, nullptr);
    RSPaintFilterCanvas paintFilterCanvas(surface.get());
    surfaceDrawable_->curCanvas_ = &paintFilterCanvas;
    EXPECT_TRUE(surfaceDrawable_->BufferFormatNeedUpdate(surface, true));
    EXPECT_FALSE(surfaceDrawable_->BufferFormatNeedUpdate(surface, false));
}

/**
 * @tc.name: IsHardwareEnabled
 * @tc.desc: Test IsHardwareEnabled
 * @tc.type: FUNC
 * @tc.require: issueIAEDYI
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, IsHardwareEnabled, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    RSUniRenderThread::Instance().Sync(std::make_unique<RSRenderThreadParams>());
    ASSERT_FALSE(surfaceDrawable_->IsHardwareEnabled());

    auto nodePtr = std::make_shared<RSRenderNode>(0);
    ASSERT_NE(nodePtr, nullptr);
    auto rsSurfaceRenderNode = DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(nodePtr);
    RSRenderThreadParamsManager::Instance().renderThreadParams_ = std::make_unique<RSRenderThreadParams>();
    RSUniRenderThread::Instance().GetRSRenderThreadParams()->hardwareEnabledTypeDrawables_.push_back(
        std::make_pair(0, rsSurfaceRenderNode));
    ASSERT_FALSE(surfaceDrawable_->IsHardwareEnabled());

    auto rsRenderNode = std::make_shared<RSRenderNode>(0);
    ASSERT_NE(rsRenderNode, nullptr);
    auto surfaceRenderNode = DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(rsRenderNode);
    surfaceRenderNode->renderParams_ = std::make_unique<RSRenderParams>(0);
    RSUniRenderThread::Instance().GetRSRenderThreadParams()->hardwareEnabledTypeDrawables_.push_back(
        std::make_pair(0, surfaceRenderNode));
    ASSERT_FALSE(surfaceDrawable_->IsHardwareEnabled());
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

    Occlusion::Region rect;
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

    std::shared_ptr<RSDirtyRegionManager> manager = surfaceDrawable_->GetSyncDirtyManager();
    ASSERT_NE(manager, nullptr);
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
    const Occlusion::Region rect{Occlusion::Rect(0, 0, 100, 100)};
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

    Vector4f vct(1.f, 1.f, 1.f, 1.f); // for test
    surfaceParams->overDrawBufferNodeCornerRadius_ = vct;
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
 * @tc.name: HasCornerRadius
 * @tc.desc: Test HasCornerRadius
 * @tc.type: FUNC
 * @tc.require: issueIAEDYI
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, HasCornerRadiusTest, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    const auto params01 = surfaceParams;
    ASSERT_FALSE(surfaceDrawable_->HasCornerRadius(*params01));

    surfaceParams->rrect_.radius_[0].y_ = 4.f;
    const auto params02 = surfaceParams;
    ASSERT_TRUE(surfaceDrawable_->HasCornerRadius(*params02));

    surfaceParams->rrect_.radius_[0].x_ = 0.f;
    const auto params03 = surfaceParams;
    ASSERT_TRUE(surfaceDrawable_->HasCornerRadius(*params03));
}

/**
 * @tc.name: DealWithUIFirstCache
 * @tc.desc: Test DealWithUIFirstCache
 * @tc.type: FUNC
 * @tc.require: issueIAEDYI
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, DealWithUIFirstCacheTest, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    auto uniParams = std::make_shared<RSRenderThreadParams>();
    ASSERT_FALSE(surfaceDrawable_->DealWithUIFirstCache(*canvas_, *surfaceParams, *uniParams));

    surfaceParams->uiFirstFlag_ = MultiThreadCacheType::ARKTS_CARD;
    ASSERT_TRUE(surfaceDrawable_->DealWithUIFirstCache(*canvas_, *surfaceParams, *uniParams));

    RSUniRenderThread::GetCaptureParam().isSnapshot_ = false;
    uniParams->isUIFirstDebugEnable_ = true;
    ASSERT_TRUE(surfaceDrawable_->DealWithUIFirstCache(*canvas_, *surfaceParams, *uniParams));

    surfaceParams->uifirstUseStarting_ = 1;
    ASSERT_TRUE(surfaceDrawable_->DealWithUIFirstCache(*canvas_, *surfaceParams, *uniParams));
}

/**
 * @tc.name: DealWithUIFirstCache001
 * @tc.desc: Test DealWithUIFirstCache while capture but don't has cache texture
 * @tc.type: FUNC
 * @tc.require: issueIANDBE
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, DealWithUIFirstCache002, TestSize.Level2)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    auto uniParams = std::make_shared<RSRenderThreadParams>();
    
    RSUniRenderThread::GetCaptureParam().isSnapshot_ = true;
    ASSERT_FALSE(surfaceDrawable_->DealWithUIFirstCache(*canvas_, *surfaceParams, *uniParams));
}

/**
 * @tc.name: OnGeneralProcess
 * @tc.desc: Test OnGeneralProcess
 * @tc.type: FUNC
 * @tc.require: issueIAEDYI
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, OnGeneralProcessTest, TestSize.Level1)
{
    auto& rtThread = RSUniRenderThread::Instance();
    if (!rtThread.GetRSRenderThreadParams()) {
        rtThread.Sync(std::make_unique<RSRenderThreadParams>());
    }
    if (!rtThread.uniRenderEngine_) {
        rtThread.uniRenderEngine_ = std::make_shared<RSRenderEngine>();
    }

    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    auto uniParams = std::make_shared<RSRenderThreadParams>();
    ASSERT_NE(uniParams, nullptr);
    surfaceDrawable_->OnGeneralProcess(canvas, *surfaceParams, *uniParams, false);
    EXPECT_FALSE(surfaceParams->GetBuffer());
    surfaceDrawable_->OnGeneralProcess(canvas, *surfaceParams, *uniParams, true);
    surfaceParams->buffer_ = OHOS::SurfaceBuffer::Create();
    surfaceDrawable_->OnGeneralProcess(canvas, *surfaceParams, *uniParams, true);
}

/**
 * @tc.name: RecordTimestamp
 * @tc.desc: Test RecordTimestamp
 * @tc.type: FUNC
 * @tc.require: IBE7GI
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, RecordTimestamp, TestSize.Level1)
{
    uint32_t seqNum = 0;
    uint64_t currentTime = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    std::string name("surfacefps");
    auto& surfaceFpsManager = RSSurfaceFpsManager::GetInstance();
    EXPECT_FALSE(surfaceFpsManager.RecordPresentTime(DEFAULT_ID, currentTime, seqNum));
    surfaceFpsManager.RegisterSurfaceFps(DEFAULT_ID, name);
    EXPECT_FALSE(surfaceFpsManager.RecordPresentTime(DEFAULT_ID, currentTime, seqNum));
    seqNum = 1;
    EXPECT_TRUE(surfaceFpsManager.RecordPresentTime(DEFAULT_ID, currentTime, seqNum));
    surfaceFpsManager.UnregisterSurfaceFps(DEFAULT_ID);
}

/**
 * @tc.name: CheckIfSurfaceSkipInMirror001
 * @tc.desc: Test CheckIfSurfaceSkipInMirror for main screen
 * @tc.type: FUNC
 * @tc.require: issueIANDBE
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, CheckIfSurfaceSkipInMirror001, TestSize.Level2)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    auto params = std::make_unique<RSRenderThreadParams>();
    params->SetIsMirrorScreen(false);
    RSUniRenderThread::Instance().Sync(std::move(params));
    ASSERT_FALSE(surfaceDrawable_->CheckIfSurfaceSkipInMirror(*surfaceParams));
}

/**
 * @tc.name: CheckIfSurfaceSkipInMirror002
 * @tc.desc: Test CheckIfSurfaceSkipInMirror while don't has white list and black list
 * @tc.type: FUNC
 * @tc.require: issueIANDBE
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, CheckIfSurfaceSkipInMirror002, TestSize.Level2)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    auto params = std::make_unique<RSRenderThreadParams>();
    params->SetIsMirrorScreen(true);
    RSUniRenderThread::Instance().Sync(std::move(params));
    ASSERT_FALSE(surfaceDrawable_->CheckIfSurfaceSkipInMirror(*surfaceParams));
}

/**
 * @tc.name: CheckIfSurfaceSkipInMirror003
 * @tc.desc: Test CheckIfSurfaceSkipInMirror for node in black list
 * @tc.type: FUNC
 * @tc.require: issueIANDBE
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, CheckIfSurfaceSkipInMirror003, TestSize.Level2)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    std::unordered_set<NodeId> blackList = {surfaceParams->GetId()};
    RSUniRenderThread::Instance().SetBlackList(blackList);

    auto params = std::make_unique<RSRenderThreadParams>();
    params->SetIsMirrorScreen(true);
    RSUniRenderThread::Instance().Sync(std::move(params));
    ASSERT_TRUE(surfaceDrawable_->CheckIfSurfaceSkipInMirror(*surfaceParams));
}

/**
 * @tc.name: CheckIfSurfaceSkipInMirror004
 * @tc.desc: Test CheckIfSurfaceSkipInMirror while white list isn't empty and node not in white list
 * @tc.type: FUNC
 * @tc.require: issueIANDBE
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, CheckIfSurfaceSkipInMirror004, TestSize.Level2)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    std::unordered_set<NodeId> whiteList = {surfaceParams->GetId() + 1};
    RSUniRenderThread::Instance().SetWhiteList(whiteList);

    auto params = std::make_unique<RSRenderThreadParams>();
    params->SetIsMirrorScreen(true);
    RSUniRenderThread::Instance().Sync(std::move(params));
    ASSERT_TRUE(surfaceDrawable_->CheckIfSurfaceSkipInMirror(*surfaceParams));
    RSUniRenderThread::Instance().Sync(std::make_unique<RSRenderThreadParams>());
}


/**
 * @tc.name: SetVirtualScreenWhiteListRootId001
 * @tc.desc: Test SetVirtualScreenWhiteListRootId while node's id not in white list
 * @tc.type: FUNC
 * @tc.require: issueIANDBE
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, SetVirtualScreenWhiteListRootId001, TestSize.Level2)
{
    ASSERT_NE(renderNode_, nullptr);
    std::unordered_set<NodeId> whiteList = {renderNode_->GetId() + 1};
    
    ASSERT_NE(surfaceDrawable_, nullptr);
    surfaceDrawable_->SetVirtualScreenWhiteListRootId(whiteList, renderNode_->GetId());
    ASSERT_EQ(RSUniRenderThread::GetCaptureParam().rootIdInWhiteList_, INVALID_NODEID);
}

/**
 * @tc.name: SetVirtualScreenWhiteListRootId002
 * @tc.desc: Test SetVirtualScreenWhiteListRootId while node's id in white list
 * @tc.type: FUNC
 * @tc.require: issueIANDBE
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, SetVirtualScreenWhiteListRootId002, TestSize.Level2)
{
    ASSERT_NE(renderNode_, nullptr);
    std::unordered_set<NodeId> whiteList = {renderNode_->GetId()};
    
    ASSERT_NE(surfaceDrawable_, nullptr);
    surfaceDrawable_->SetVirtualScreenWhiteListRootId(whiteList, renderNode_->GetId());
    ASSERT_EQ(RSUniRenderThread::GetCaptureParam().rootIdInWhiteList_, renderNode_->GetId());
}

/**
 * @tc.name: SetVirtualScreenWhiteListRootId003
 * @tc.desc: Test SetVirtualScreenWhiteListRootId while rootIdInWhiteList has been set
 * @tc.type: FUNC
 * @tc.require: issueIANDBE
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, SetVirtualScreenWhiteListRootId003, TestSize.Level2)
{
    ASSERT_NE(renderNode_, nullptr);
    std::unordered_set<NodeId> whiteList = {renderNode_->GetId(), renderNode_->GetId() +1};
    
    ASSERT_NE(surfaceDrawable_, nullptr);
    surfaceDrawable_->SetVirtualScreenWhiteListRootId(whiteList, renderNode_->GetId());
    surfaceDrawable_->SetVirtualScreenWhiteListRootId(whiteList, renderNode_->GetId() + 1);
    ASSERT_EQ(RSUniRenderThread::GetCaptureParam().rootIdInWhiteList_, renderNode_->GetId());
}

/**
 * @tc.name: ResetVirtualScreenWhiteListRootId001
 * @tc.desc: Test ResetVirtualScreenWhiteListRootId while id equals rootIdInWhiteList_
 * @tc.type: FUNC
 * @tc.require: issueIANDBE
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, ResetVirtualScreenWhiteListRootId001, TestSize.Level2)
{
    ASSERT_NE(renderNode_, nullptr);
    std::unordered_set<NodeId> whiteList = {renderNode_->GetId()};
    
    ASSERT_NE(surfaceDrawable_, nullptr);
    surfaceDrawable_->SetVirtualScreenWhiteListRootId(whiteList, renderNode_->GetId());
    surfaceDrawable_->ResetVirtualScreenWhiteListRootId(renderNode_->GetId());
    ASSERT_EQ(RSUniRenderThread::GetCaptureParam().rootIdInWhiteList_, INVALID_NODEID);
}

/**
 * @tc.name: SetSubThreadSkip001
 * @tc.desc: Test SetSubThreadSkip
 * @tc.type: FUNC
 * @tc.require: issueIANDBE
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, SetSubThreadSkip001, TestSize.Level2)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    
    surfaceDrawable_->SetSubThreadSkip(true);
    ASSERT_TRUE(surfaceDrawable_->IsSubThreadSkip());
}

/**
 * @tc.name: SetAndGet
 * @tc.desc: Test SetAndGet
 * @tc.type: FUNC
 * @tc.require: issueIAEDYI
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, SetAndGetTest001, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    Occlusion::Region region = surfaceDrawable_->GetVisibleDirtyRegion();
    EXPECT_TRUE(region.IsEmpty());
    Occlusion::Region OccRegion;
    surfaceDrawable_->SetVisibleDirtyRegion(OccRegion);
    surfaceDrawable_->SetAlignedVisibleDirtyRegion(OccRegion);
    Occlusion::Region rect;
    surfaceDrawable_->SetGlobalDirtyRegion(rect);
    surfaceDrawable_->SetDirtyRegionAlignedEnable(true);
    surfaceDrawable_->SetDirtyRegionBelowCurrentLayer(OccRegion);
    surfaceDrawable_->GetSyncDirtyManager();
}

/**
 * @tc.name: SetAndGet
 * @tc.desc: Test SetAndGet
 * @tc.type: FUNC
 * @tc.require: issueIAEDYI
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, SetAndGetTest002, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    surfaceDrawable_->renderParams_ = std::make_unique<RSRenderParams>(0);
    Occlusion::Region OccRegion;
    surfaceDrawable_->SetVisibleDirtyRegion(OccRegion);
    Occlusion::Region region = surfaceDrawable_->GetVisibleDirtyRegion();
    EXPECT_TRUE(region.IsEmpty());

    surfaceDrawable_->SetAlignedVisibleDirtyRegion(OccRegion);
    Occlusion::Region rect;
    surfaceDrawable_->SetGlobalDirtyRegion(rect);
    surfaceDrawable_->SetDirtyRegionAlignedEnable(true);
    surfaceDrawable_->SetDirtyRegionBelowCurrentLayer(OccRegion);
    ASSERT_NE(surfaceDrawable_->GetSyncDirtyManager(), nullptr);
}

/**
 * @tc.name: DrawSelfDrawingNodeBuffer
 * @tc.desc: Test DrawSelfDrawingNodeBuffer
 * @tc.type: FUNC
 * @tc.require: issueIAOJHQ
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, DrawSelfDrawingNodeBufferTest, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    BufferDrawParam params;
    RSUniRenderThread::Instance().uniRenderEngine_ = std::make_shared<RSRenderEngine>();
    surfaceDrawable_->DrawSelfDrawingNodeBuffer(canvas, *surfaceParams, params);

    Color color(255, 255, 255);
    surfaceParams->backgroundColor_ = color;
    surfaceDrawable_->DrawSelfDrawingNodeBuffer(canvas, *surfaceParams, params);
    surfaceDrawable_->DrawSelfDrawingNodeBuffer(canvas, *surfaceParams, params);
    surfaceParams->rrect_.radius_[0].x_ = 1.f;
    surfaceDrawable_->DrawSelfDrawingNodeBuffer(canvas, *surfaceParams, params);
    surfaceParams->selfDrawingType_ = SelfDrawingNodeType::VIDEO;
    surfaceDrawable_->DrawSelfDrawingNodeBuffer(canvas, *surfaceParams, params);
    ASSERT_FALSE(surfaceParams->GetHardwareEnabled());
}

/**
 * @tc.name: DealWithSelfDrawingNodeBuffer
 * @tc.desc: Test DealWithSelfDrawingNodeBuffer
 * @tc.type: FUNC
 * @tc.require: issueIAOJHQ
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, DealWithSelfDrawingNodeBufferTest001, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    surfaceParams->isInFixedRotation_ = false;
    surfaceDrawable_->DealWithSelfDrawingNodeBuffer(canvas, *surfaceParams);
    ASSERT_FALSE(surfaceParams->GetHardwareEnabled());
    ASSERT_FALSE(surfaceParams->IsInFixedRotation());

    surfaceParams->isInFixedRotation_ = true;
    surfaceDrawable_->DealWithSelfDrawingNodeBuffer(canvas, *surfaceParams);
    surfaceParams->isHardwareEnabled_ = true;
    surfaceDrawable_->name_ = "test";
    surfaceDrawable_->surfaceNodeType_ = RSSurfaceNodeType::DEFAULT;
    surfaceParams->isLayerTop_ = false;
    surfaceDrawable_->DealWithSelfDrawingNodeBuffer(canvas, *surfaceParams);
    ASSERT_TRUE(surfaceParams->GetHardwareEnabled());

    surfaceParams->isHardCursor_ = true;
    surfaceDrawable_->DealWithSelfDrawingNodeBuffer(canvas, *surfaceParams);
    ASSERT_TRUE(surfaceParams->GetHardCursorStatus());
    ASSERT_FALSE(surfaceDrawable_->IsHardwareEnabledTopSurface());

    surfaceParams->isLayerTop_ = true;
    surfaceDrawable_->DealWithSelfDrawingNodeBuffer(canvas, *surfaceParams);
    ASSERT_TRUE(surfaceParams->IsLayerTop());
    surfaceDrawable_->surfaceNodeType_ = RSSurfaceNodeType::SELF_DRAWING_WINDOW_NODE;
    surfaceDrawable_->name_ = "pointer window";
    surfaceDrawable_->DealWithSelfDrawingNodeBuffer(canvas, *surfaceParams);
}

/**
 * @tc.name: DrawCloneNode
 * @tc.desc: Test DrawCloneNode while node is not clone
 * @tc.type: FUNC
 * @tc.require: issueIBH7WD
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, DrawCloneNode, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    surfaceParams->isCloneNode_ = false;
    RSRenderThreadParams uniParams;
    auto result = surfaceDrawable_->DrawCloneNode(canvas, uniParams, *surfaceParams, false);
    ASSERT_FALSE(result);
}

/**
 * @tc.name: DrawCloneNode
 * @tc.desc: Test DrawCloneNode001 while node is clone
 * @tc.type: FUNC
 * @tc.require: issueIBKU7U
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, DrawCloneNode001, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    surfaceParams->isCloneNode_ = true;
    surfaceParams->isClonedNodeOnTheTree_ = false;
    RSRenderThreadParams uniParams;

    auto result = surfaceDrawable_->DrawCloneNode(canvas, uniParams, *surfaceParams, false);
    ASSERT_FALSE(result);

    surfaceParams->isClonedNodeOnTheTree_ = true;
    result = surfaceDrawable_->DrawCloneNode(canvas, uniParams, *surfaceParams, false);
    ASSERT_FALSE(result);
    DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr clonedNodeRenderDrawableSharedPtr(drawable_);
    DrawableV2::RSRenderNodeDrawableAdapter::WeakPtr clonedNodeRenderDrawable(clonedNodeRenderDrawableSharedPtr);
    surfaceParams->clonedNodeRenderDrawable_ = clonedNodeRenderDrawable;
    result = surfaceDrawable_->DrawCloneNode(canvas, uniParams, *surfaceParams, true);
    ASSERT_TRUE(result);
}

/**
 * @tc.name: CheckDrawAndCacheWindowContentTest
 * @tc.desc: Test OnGeneralProcessAndCache
 * @tc.type: FUNC
 * @tc.require: issueB2YOV
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, CheckDrawAndCacheWindowContentTest, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    surfaceParams->SetNeedCacheSurface(false);
    surfaceParams->isCrossNode_ = false;

    auto uniParams = std::make_shared<RSRenderThreadParams>();
    ASSERT_NE(uniParams, nullptr);

    uniParams->SetIsFirstVisitCrossNodeDisplay(false);
    ASSERT_FALSE(surfaceDrawable_->CheckDrawAndCacheWindowContent(*surfaceParams, *uniParams));

    surfaceParams->SetNeedCacheSurface(true);
    ASSERT_TRUE(surfaceDrawable_->CheckDrawAndCacheWindowContent(*surfaceParams, *uniParams));

    surfaceParams->isCrossNode_ = true;
    ASSERT_FALSE(surfaceDrawable_->CheckDrawAndCacheWindowContent(*surfaceParams, *uniParams));

    uniParams->SetIsFirstVisitCrossNodeDisplay(true);
    ASSERT_FALSE(surfaceDrawable_->CheckDrawAndCacheWindowContent(*surfaceParams, *uniParams));

    RSUniRenderThread::captureParam_.isSnapshot_ = true;
    ASSERT_FALSE(RSUniRenderThread::IsExpandScreenMode());
    ASSERT_FALSE(surfaceDrawable_->CheckDrawAndCacheWindowContent(*surfaceParams, *uniParams));
}

/**
 * @tc.name: OnGeneralProcessAndCache
 * @tc.desc: Test OnGeneralProcessAndCache
 * @tc.type: FUNC
 * @tc.require: issueIAVLLE
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, OnGeneralProcessAndCache, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable_->GetRenderParams().get());
    ASSERT_NE(surfaceParams, nullptr);
    surfaceParams->SetFrameRect({0, 0, DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE});
    surfaceParams->SetNeedCacheSurface(true);
    std::shared_ptr<Drawing::Surface> surface =
        Drawing::Surface::MakeRasterN32Premul(DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE);
    ASSERT_NE(surface, nullptr);
    RSPaintFilterCanvas canvas(surface.get());
    auto uniParams = std::make_shared<RSRenderThreadParams>();
    ASSERT_NE(uniParams, nullptr);
    surfaceDrawable_->OnGeneralProcess(canvas, *surfaceParams, *uniParams, false);
    ASSERT_TRUE(surfaceDrawable_->drawWindowCache_.HasCache());
}
}
