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
#include "pipeline/rs_render_engine.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_uni_render_thread.h"
#include "params/rs_render_thread_params.h"
#include "pipeline/rs_uni_render_engine.h"

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

    surfaceDrawable_->renderParams_ = nullptr;
    surfaceDrawable_->MergeDirtyRegionBelowCurSurface(*uniParams, region);
    surfaceDrawable_->renderParams_ = std::make_unique<RSSurfaceRenderParams>(surfaceDrawable_->nodeId_);
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
    surfaceParams->windowInfo_.isMainWindowType_ = true;
    surfaceDrawable_->MergeDirtyRegionBelowCurSurface(*uniParams, region);
    ASSERT_TRUE(surfaceParams->IsMainWindowType());
}

/**
 * @tc.name: MergeDirtyRegionBelowCurSurface
 * @tc.desc: Test MergeDirtyRegionBelowCurSurface, dirty region expected not empty.
 * @tc.type: FUNC
 * @tc.require: issueIAEDYI
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, MergeDirtyRegionBelowCurSurface003, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto uniParams = std::make_shared<RSRenderThreadParams>();
    ASSERT_NE(uniParams, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    Drawing::Region region;
    surfaceParams->windowInfo_.isMainWindowType_ = true;
    Occlusion::Rect rect(1, 2, 3, 4);
    Occlusion::Rect bound(1, 2, 3, 4);
    surfaceParams->visibleRegion_.rects_.push_back(rect);
    surfaceParams->visibleRegion_.bound_ = bound;
    surfaceDrawable_->MergeDirtyRegionBelowCurSurface(*uniParams, region);
    ASSERT_FALSE(surfaceParams->GetVisibleRegion().IsEmpty());
}

/**
 * @tc.name: MergeDirtyRegionBelowCurSurface
 * @tc.desc: Test MergeDirtyRegionBelowCurSurface, dirty region expected not empty.
 * @tc.type: FUNC
 * @tc.require: issueIAEDYI
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, MergeDirtyRegionBelowCurSurface004, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto uniParams = std::make_shared<RSRenderThreadParams>();
    ASSERT_NE(uniParams, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    Drawing::Region region;
    surfaceParams->windowInfo_.isMainWindowType_ = false;
    surfaceParams->windowInfo_.isLeashWindow_ = true;
    surfaceDrawable_->MergeDirtyRegionBelowCurSurface(*uniParams, region);
    ASSERT_TRUE(surfaceParams->GetVisibleRegion().IsEmpty());
}

/**
 * @tc.name: MergeDirtyRegionBelowCurSurface
 * @tc.desc: Test MergeDirtyRegionBelowCurSurface, dirty region expected not empty.
 * @tc.type: FUNC
 * @tc.require: issueIAEDYI
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, MergeDirtyRegionBelowCurSurface005, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto uniParams = std::make_shared<RSRenderThreadParams>();
    ASSERT_NE(uniParams, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    Drawing::Region region;
    surfaceParams->windowInfo_.isMainWindowType_ = true;
    surfaceParams->windowInfo_.isLeashWindow_ = true;
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
 * @tc.name: MergeDirtyRegionBelowCurSurface
 * @tc.desc: Test MergeDirtyRegionBelowCurSurface, dirty region expected not empty.
 * @tc.type: FUNC
 * @tc.require: issueIAEDYI
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, MergeDirtyRegionBelowCurSurface006, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto uniParams = std::make_shared<RSRenderThreadParams>();
    ASSERT_NE(uniParams, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    Drawing::Region region;
    surfaceParams->windowInfo_.isMainWindowType_ = true;
    surfaceParams->windowInfo_.isLeashWindow_ = true;
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
 * @tc.name: MergeDirtyRegionBelowCurSurface
 * @tc.desc: Test MergeDirtyRegionBelowCurSurface, dirty region expected not empty.
 * @tc.type: FUNC
 * @tc.require: issueIAEDYI
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, MergeDirtyRegionBelowCurSurface007, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto uniParams = std::make_shared<RSRenderThreadParams>();
    ASSERT_NE(uniParams, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    Drawing::Region region;
    surfaceParams->windowInfo_.isMainWindowType_ = false;
    surfaceParams->windowInfo_.isLeashWindow_ = true;
    surfaceParams->isParentScaling_ = true;
    surfaceParams->isSubSurfaceNode_ = false;
    uniParams->accumulatedDirtyRegion_ = Occlusion::Rect { 0, 0, DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE };
    surfaceParams->visibleRegion_ = Occlusion::Rect { 0, 0, DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE };
    surfaceDrawable_->MergeDirtyRegionBelowCurSurface(*uniParams, region);
    ASSERT_FALSE(surfaceParams->GetVisibleRegion().IsEmpty());
}

/**
 * @tc.name: MergeDirtyRegionBelowCurSurface
 * @tc.desc: Test MergeDirtyRegionBelowCurSurface, dirty region expected not empty.
 * @tc.type: FUNC
 * @tc.require: issueIAEDYI
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, MergeDirtyRegionBelowCurSurface008, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto uniParams = std::make_shared<RSRenderThreadParams>();
    ASSERT_NE(uniParams, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    Drawing::Region region;
    surfaceParams->windowInfo_.isMainWindowType_ = false;
    surfaceParams->windowInfo_.isLeashWindow_ = true;
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
 * @tc.name: MergeDirtyRegionBelowCurSurface
 * @tc.desc: Test MergeDirtyRegionBelowCurSurface, dirty region expected not empty.
 * @tc.type: FUNC
 * @tc.require: issueIAEDYI
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, MergeDirtyRegionBelowCurSurface009, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto uniParams = std::make_shared<RSRenderThreadParams>();
    ASSERT_NE(uniParams, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    Drawing::Region region;
    surfaceParams->windowInfo_.isMainWindowType_ = false;
    surfaceParams->windowInfo_.isLeashWindow_ = true;
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
    surfaceDrawable_->OnCapture(*canvas_);
    // should paint
    drawable_->renderParams_->shouldPaint_ = true;
    drawable_->renderParams_->contentEmpty_ = false;
    surfaceDrawable_->OnCapture(*canvas_);
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
    surfaceParams->rsSurfaceNodeType_ = RSSurfaceNodeType::SELF_DRAWING_NODE;
    surfaceParams->isSpherizeValid_ = false;
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    surfaceDrawable_->hasHdrPresent_ = true;
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);

    surfaceParams->protectedLayerIds_.insert(1);
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    surfaceParams->skipLayerIds_.insert(1);
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    surfaceParams->snapshotSkipLayerIds_.insert(1);
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    surfaceParams->securityLayerIds_.insert(1);
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    ASSERT_TRUE(surfaceParams->HasSkipLayer());
    ASSERT_TRUE(surfaceParams->HasSnapshotSkipLayer());
    RSUniRenderThread::Instance().Sync(std::make_unique<RSRenderThreadParams>());
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);

    surfaceParams->isProtectedLayer_ = true;
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    surfaceParams->isSkipLayer_ = true;
    surfaceDrawable_->CaptureSurface(*canvas_, *surfaceParams);
    surfaceParams->isSnapshotSkipLayer_ = true;
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
 * @tc.name: MergeSubSurfaceNodesDirtyRegionForMainWindow_001
 * @tc.desc: Test MergeSubSurfaceNodesDirtyRegionForMainWindow, if a surface has no sub-surface, early return.
 * @tc.type: FUNC
 * @tc.require: issueIB2HV9
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, MergeSubSurfaceNodesDirtyRegionForMainWindow_001, TestSize.Level1)
{
    // main surface
    ASSERT_NE(drawable_, nullptr);
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);

    Occlusion::Region resultRegion;
    surfaceDrawable_->MergeSubSurfaceNodesDirtyRegionForMainWindow(*surfaceParams, resultRegion);
    ASSERT_TRUE(resultRegion.IsEmpty());
}

/**
 * @tc.name: MergeSubSurfaceNodesDirtyRegionForMainWindow_002
 * @tc.desc: Test MergeSubSurfaceNodesDirtyRegionForMainWindow, if a surface has subsurface dirty, it should be counted.
 * @tc.type: FUNC
 * @tc.require: issueIB2HV9
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, MergeSubSurfaceNodesDirtyRegionForMainWindow_002, TestSize.Level1)
{
    // main surface
    ASSERT_NE(drawable_, nullptr);
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    // child surface
    NodeId subSurfaceId = 1;
    auto subSurfaceNode = std::make_shared<RSSurfaceRenderNode>(subSurfaceId);
    ASSERT_NE(subSurfaceNode, nullptr);
    RSRenderNodeDrawableAdapter::SharedPtr subNodeDrawable(RSSurfaceRenderNodeDrawable::OnGenerate(subSurfaceNode));
    ASSERT_NE(subNodeDrawable, nullptr);
    auto subSurfaceDrawable = static_cast<RSSurfaceRenderNodeDrawable*>(subNodeDrawable.get());
    ASSERT_NE(subSurfaceDrawable, nullptr);
    subSurfaceDrawable->syncDirtyManager_->dirtyRegion_ = {
        DEFAULT_RECT.left_, DEFAULT_RECT.top_,
        DEFAULT_RECT.right_ - DEFAULT_RECT.left_, DEFAULT_RECT.bottom_ - DEFAULT_RECT.top_ };
    RSRenderNodeDrawableAdapter::RenderNodeDrawableCache_.emplace(subSurfaceId, subNodeDrawable);
    surfaceParams->allSubSurfaceNodeIds_.insert(subSurfaceId);

    Occlusion::Region resultRegion;
    surfaceDrawable_->MergeSubSurfaceNodesDirtyRegionForMainWindow(*surfaceParams, resultRegion);
    ASSERT_FALSE(resultRegion.IsEmpty());
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

    surfaceParams->SetWindowInfo(false, true, false);
    Drawing::Region result = surfaceDrawable_->CalculateVisibleDirtyRegion(*uniParams,
        *surfaceParams, *surfaceDrawable_, true);
    ASSERT_TRUE(result.IsEmpty());

    surfaceParams->SetWindowInfo(true, false, false);
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
 * @tc.name: DealWithSelfDrawingNodeBuffer
 * @tc.desc: Test DealWithSelfDrawingNodeBuffer
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, DealWithSelfDrawingNodeBufferTest, TestSize.Level1)
{
    ASSERT_NE(renderNode_, nullptr);
    ASSERT_NE(surfaceDrawable_, nullptr);
    RSUniRenderThread::GetCaptureParam().isSnapshot_ = true;
    std::shared_ptr<Drawing::Surface> drawingSurface = std::make_shared<Drawing::Surface>();
    canvas_->surface_ = drawingSurface.get();
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    const auto params01 = surfaceParams;
    RSUniRenderThread::Instance().uniRenderEngine_ = std::make_shared<RSUniRenderEngine>();
    surfaceParams->isHardwareEnabled_ = true;
    CaptureParam param;
    param.isSnapshot_ = false;
    param.isMirror_ = false;
    RSUniRenderThread::SetCaptureParam(param);
    surfaceDrawable_->DealWithSelfDrawingNodeBuffer(*canvas_, *params01);
    ASSERT_FALSE(surfaceDrawable_->IsHardwareEnabledTopSurface());
    ASSERT_TRUE(surfaceParams->GetHardwareEnabled());

    renderNode_->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_WINDOW_NODE;
    renderNode_->name_ = "pointer window"; // for test
    const auto params03 = surfaceParams;
    surfaceDrawable_->DealWithSelfDrawingNodeBuffer(*canvas_, *params03);
    ASSERT_TRUE(renderNode_->IsHardwareEnabledTopSurface());
}

/**
 * @tc.name: DrawSelfDrawingNodeBuffer
 * @tc.desc: Test DrawSelfDrawingNodeBuffer
 * @tc.type: FUNC
 * @tc.require: issueIAEDYI
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, DrawSelfDrawingNodeBufferTest, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    const auto params01 = surfaceParams;
    BufferDrawParam params;
    RSUniRenderThread::Instance().uniRenderEngine_ = std::make_shared<RSUniRenderEngine>();
    surfaceDrawable_->DrawSelfDrawingNodeBuffer(*canvas_, *params01, params);
    ASSERT_TRUE(surfaceParams->GetSelfDrawingNodeType() != SelfDrawingNodeType::VIDEO);

    Color color(255, 255, 255, 255); // for test
    surfaceParams->backgroundColor_ = color;
    const auto params02 = surfaceParams;
    surfaceDrawable_->DrawSelfDrawingNodeBuffer(*canvas_, *params02, params);
    ASSERT_TRUE(surfaceParams->GetSelfDrawingNodeType() != SelfDrawingNodeType::VIDEO);
    ASSERT_FALSE(surfaceDrawable_->HasCornerRadius(*params02));

    surfaceParams->rrect_.radius_[0].x_ = 0.f;
    surfaceParams->rrect_.radius_[0].y_ = 4.f;
    const auto params03 = surfaceParams;
    surfaceDrawable_->DrawSelfDrawingNodeBuffer(*canvas_, *params03, params);
    ASSERT_TRUE(surfaceDrawable_->HasCornerRadius(*params03));
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
    surfaceDrawable_->OnGeneralProcess(canvas, *surfaceParams, false);
    EXPECT_FALSE(surfaceParams->GetBuffer());
    surfaceDrawable_->OnGeneralProcess(canvas, *surfaceParams, true);
}

/**
 * @tc.name: DealWithUIFirstCache001
 * @tc.desc: Test DealWithUIFirstCache while capture but don't has cache texture
 * @tc.type: FUNC
 * @tc.require: issueIAOTZ3
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
 * @tc.name: CheckIfSurfaceSkipInMirror001
 * @tc.desc: Test CheckIfSurfaceSkipInMirror for main screen
 * @tc.type: FUNC
 * @tc.require: issueIAOTZ3
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, CheckIfSurfaceSkipInMirror001, TestSize.Level2)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    RSUniRenderThread::GetCaptureParam().isMirror_ = false;
    ASSERT_FALSE(surfaceDrawable_->CheckIfSurfaceSkipInMirror(*surfaceParams));
}

/**
 * @tc.name: CheckIfSurfaceSkipInMirror002
 * @tc.desc: Test CheckIfSurfaceSkipInMirror while don't has white list and black list
 * @tc.type: FUNC
 * @tc.require: issueIAOTZ3
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, CheckIfSurfaceSkipInMirror002, TestSize.Level2)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    RSUniRenderThread::GetCaptureParam().isMirror_ = true;
    ASSERT_FALSE(surfaceDrawable_->CheckIfSurfaceSkipInMirror(*surfaceParams));
}

/**
 * @tc.name: CheckIfSurfaceSkipInMirror003
 * @tc.desc: Test CheckIfSurfaceSkipInMirror for node in black list
 * @tc.type: FUNC
 * @tc.require: issueIAOTZ3
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, CheckIfSurfaceSkipInMirror003, TestSize.Level2)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    std::unordered_set<NodeId> blackList = {surfaceParams->GetId()};
    RSUniRenderThread::Instance().SetBlackList(blackList);

    RSUniRenderThread::GetCaptureParam().isMirror_ = true;
    ASSERT_TRUE(surfaceDrawable_->CheckIfSurfaceSkipInMirror(*surfaceParams));
}

/**
 * @tc.name: CheckIfSurfaceSkipInMirror004
 * @tc.desc: Test CheckIfSurfaceSkipInMirror while white list isn't empty and node not in white list
 * @tc.type: FUNC
 * @tc.require: issueIAOTZ3
 */
HWTEST_F(RSSurfaceRenderNodeDrawableTest, CheckIfSurfaceSkipInMirror004, TestSize.Level2)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    auto uniParams = std::make_shared<RSRenderThreadParams>();
    std::unordered_set<NodeId> whiteList = {surfaceParams->GetId() + 1};
    RSUniRenderThread::Instance().SetWhiteList(whiteList);

    RSUniRenderThread::GetCaptureParam().isMirror_ = true;
    ASSERT_TRUE(surfaceDrawable_->CheckIfSurfaceSkipInMirror(*surfaceParams));
}

/**
 * @tc.name: SetVirtualScreenWhiteListRootId001
 * @tc.desc: Test SetVirtualScreenWhiteListRootId while node's id not in white list
 * @tc.type: FUNC
 * @tc.require: issueIAOTZ3
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
 * @tc.require: issueIAOTZ3
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
 * @tc.require: issueIAOTZ3
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
 * @tc.require: issueIAOTZ3
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
 * @tc.require: issueIAOTZ3
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
 * @tc.name: DealWithSelfDrawingNodeBuffer
 * @tc.desc: Test DealWithSelfDrawingNodeBuffer
 * @tc.type: FUNC
 * @tc.require: issueIAOTZ3
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
    surfaceDrawable_->DealWithSelfDrawingNodeBuffer(canvas, *surfaceParams);
    ASSERT_TRUE(surfaceParams->GetHardwareEnabled());

    surfaceDrawable_->DealWithSelfDrawingNodeBuffer(canvas, *surfaceParams);
    surfaceDrawable_->surfaceNodeType_ = RSSurfaceNodeType::SELF_DRAWING_WINDOW_NODE;
    surfaceDrawable_->name_ = "pointer window";
    surfaceDrawable_->DealWithSelfDrawingNodeBuffer(canvas, *surfaceParams);
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
    surfaceDrawable_->OnGeneralProcess(canvas, *surfaceParams, false);
    ASSERT_TRUE(surfaceDrawable_->drawWindowCache_.HasCache());
}
}
