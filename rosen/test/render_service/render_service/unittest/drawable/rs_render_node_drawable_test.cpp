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
#include "drawable/rs_render_node_drawable.h"
#include "render/rs_drawing_filter.h"
#include "render/rs_effect_luminance_manager.h"
#include "params/rs_render_params.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "pipeline/rs_paint_filter_canvas.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::DrawableV2;

namespace OHOS::Rosen {
constexpr NodeId DEFAULT_ID = 0xFFFF;
class RSRenderNodeDrawableTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline NodeId id = DEFAULT_ID;
    static std::shared_ptr<RSRenderNodeDrawable> CreateDrawable();
};

void RSRenderNodeDrawableTest::SetUpTestCase() {}
void RSRenderNodeDrawableTest::TearDownTestCase() {}
void RSRenderNodeDrawableTest::SetUp() {}
void RSRenderNodeDrawableTest::TearDown() {}

std::shared_ptr<RSRenderNodeDrawable> RSRenderNodeDrawableTest::CreateDrawable()
{
    auto renderNode = std::make_shared<RSRenderNode>(id);
    auto drawable = std::static_pointer_cast<RSRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(renderNode));
    return drawable;
}

/**
 * @tc.name: CreateRenderNodeDrawableTest
 * @tc.desc: Test If RenderNodeDrawable Can Be Created
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSRenderNodeDrawableTest, CreateRenderNodeDrawable, TestSize.Level1)
{
    NodeId id = 1;
    auto renderNode = std::make_shared<RSRenderNode>(id);
    auto drawable = RSRenderNodeDrawable::OnGenerate(renderNode);
    ASSERT_NE(drawable, nullptr);
}

/**
 * @tc.name: GenerateCacheIfNeed
 * @tc.desc: Test If GenerateCacheIfNeed Can Run
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSRenderNodeDrawableTest, GenerateCacheIfNeedTest, TestSize.Level1)
{
    auto drawable = RSRenderNodeDrawableTest::CreateDrawable();
    Drawing::Canvas canvas;
    RSRenderParams params(RSRenderNodeDrawableTest::id);
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    drawable->InitCachedSurface(paintFilterCanvas.GetGPUContext().get(), params.GetCacheSize(), 0xFF);
    drawable->GenerateCacheIfNeed(canvas, params);
    ASSERT_FALSE(params.GetRSFreezeFlag());
    params.freezeFlag_ = false;
    drawable->GenerateCacheIfNeed(canvas, params);
    ASSERT_FALSE(params.GetRSFreezeFlag());
    drawable->opincDrawCache_.isOpincMarkCached_ = true;
    drawable->GenerateCacheIfNeed(canvas, params);
    ASSERT_TRUE(drawable->opincDrawCache_.OpincGetCachedMark());

    params.drawingCacheType_ = RSDrawingCacheType::FORCED_CACHE;
    drawable->GenerateCacheIfNeed(canvas, params);
    ASSERT_FALSE(drawable->cachedSurface_);
    drawable->cachedSurface_ = std::make_shared<Drawing::Surface>();
    drawable->GenerateCacheIfNeed(canvas, params);
    ASSERT_FALSE(params.ChildHasVisibleFilter());
    params.drawingCacheType_ = RSDrawingCacheType::TARGETED_CACHE;
    drawable->GenerateCacheIfNeed(canvas, params);
    ASSERT_FALSE(params.GetDrawingCacheType() == RSDrawingCacheType::TARGETED_CACHE);

    params.freezeFlag_ = false;
    params.isDrawingCacheChanged_ = true;
    drawable->drawingCacheUpdateTimeMap_[drawable->nodeId_] = 4;
    drawable->GenerateCacheIfNeed(canvas, params);
    ASSERT_FALSE(params.GetDrawingCacheChanged());
    params.isDrawingCacheChanged_ = false;
    drawable->GenerateCacheIfNeed(canvas, params);

    drawable->opincDrawCache_.isOpincMarkCached_ = false;
    params.drawingCacheType_ = RSDrawingCacheType::FORCED_CACHE;
    drawable->GenerateCacheIfNeed(canvas, params);
    ASSERT_TRUE(!drawable->opincDrawCache_.OpincGetCachedMark());
    params.freezeFlag_ = false;
    drawable->GenerateCacheIfNeed(canvas, params);
    ASSERT_FALSE(params.GetRSFreezeFlag());
    ASSERT_TRUE(params.GetDrawingCacheType() == RSDrawingCacheType::DISABLED_CACHE);
    params.childHasVisibleFilter_ = true;
    drawable->GenerateCacheIfNeed(canvas, params);
    ASSERT_TRUE(params.ChildHasVisibleFilter());
}

/**
 * @tc.name: CheckCacheTypeAndDraw
 * @tc.desc: Test If CheckCacheTypeAndDraw Can Run
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSRenderNodeDrawableTest, CheckCacheTypeAndDrawTest, TestSize.Level1)
{
    auto drawable = RSRenderNodeDrawableTest::CreateDrawable();
    Drawing::Canvas canvas;
    RSRenderParams params(RSRenderNodeDrawableTest::id);
    drawable->CheckCacheTypeAndDraw(canvas, params);
    params.childHasVisibleFilter_ = true;
    drawable->CheckCacheTypeAndDraw(canvas, params);
    ASSERT_TRUE(params.ChildHasVisibleFilter());

    params.drawingCacheType_ = RSDrawingCacheType::FORCED_CACHE;
    params.foregroundFilterCache_ = std::make_shared<RSFilter>();
    drawable->CheckCacheTypeAndDraw(canvas, params);
    ASSERT_TRUE(params.GetForegroundFilterCache());
    params.foregroundFilterCache_ = nullptr;
    drawable->CheckCacheTypeAndDraw(canvas, params);
    ASSERT_TRUE(params.GetForegroundFilterCache() == nullptr);

    drawable->drawBlurForCache_ = true;
    params.drawingCacheType_ = RSDrawingCacheType::DISABLED_CACHE;
    drawable->CheckCacheTypeAndDraw(canvas, params);
    params.childHasVisibleFilter_ = false;
    drawable->CheckCacheTypeAndDraw(canvas, params);
    params.childHasVisibleEffect_ = true;
    drawable->CheckCacheTypeAndDraw(canvas, params);
    ASSERT_FALSE(!params.ChildHasVisibleEffect());

    drawable->CheckCacheTypeAndDraw(canvas, params);
    ASSERT_FALSE(drawable->HasFilterOrEffect());
    drawable->drawCmdIndex_.shadowIndex_ = 1;
    drawable->CheckCacheTypeAndDraw(canvas, params);
    ASSERT_TRUE(drawable->HasFilterOrEffect());
    RSRenderNodeDrawable::isOffScreenWithClipHole_ = true;
    params.foregroundFilterCache_ = std::make_shared<RSFilter>();
    drawable->CheckCacheTypeAndDraw(canvas, params);
    RSRenderNodeDrawable::isOffScreenWithClipHole_ = false;
    ASSERT_TRUE(params.GetForegroundFilterCache());

    drawable->SetCacheType(DrawableCacheType::NONE);
    drawable->CheckCacheTypeAndDraw(canvas, params);
    drawable->SetCacheType(DrawableCacheType::CONTENT);
    drawable->CheckCacheTypeAndDraw(canvas, params);
    params.drawingCacheIncludeProperty_ = true;
    drawable->CheckCacheTypeAndDraw(canvas, params);
    params.foregroundFilterCache_ = nullptr;
    drawable->CheckCacheTypeAndDraw(canvas, params);
    ASSERT_TRUE(params.GetForegroundFilterCache() == nullptr);
}

/**
 * @tc.name: UpdateCacheInfoForDfx
 * @tc.desc: Test If UpdateCacheInfoForDfx Can Run
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSRenderNodeDrawableTest, UpdateCacheInfoForDfxTest, TestSize.Level1)
{
    auto drawable = RSRenderNodeDrawableTest::CreateDrawable();

    Drawing::Canvas canvas;
    RSRenderParams params(RSRenderNodeDrawableTest::id);
    drawable->UpdateCacheInfoForDfx(canvas, params.GetBounds(), params.GetId());
    drawable->isDrawingCacheDfxEnabled_ = true;
    drawable->UpdateCacheInfoForDfx(canvas, params.GetBounds(), params.GetId());
    ASSERT_EQ(drawable->isDrawingCacheEnabled_, false);
}

/**
 * @tc.name: DrawDfxForCacheInfo
 * @tc.desc: Test If DrawDfxForCacheInfo Can Run
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSRenderNodeDrawableTest, DrawDfxForCacheInfoTest, TestSize.Level1)
{
    auto drawable = RSRenderNodeDrawableTest::CreateDrawable();
    Drawing::Canvas canvas;
    RSRenderParams params(RSRenderNodeDrawableTest::id);
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    auto renderParams = std::make_unique<RSRenderParams>(0);
    drawable->DrawDfxForCacheInfo(paintFilterCanvas, renderParams);
    ASSERT_FALSE(drawable->isDrawingCacheEnabled_);
    ASSERT_TRUE(drawable->isDrawingCacheDfxEnabled_);
    ASSERT_FALSE(drawable->autoCacheDrawingEnable_);
    ASSERT_FALSE(!drawable->isDrawingCacheDfxEnabled_);

    drawable->isDrawingCacheEnabled_ = true;
    drawable->autoCacheDrawingEnable_ = true;
    drawable->DrawDfxForCacheInfo(paintFilterCanvas, renderParams);
    ASSERT_TRUE(drawable->isDrawingCacheEnabled_);

    drawable->isDrawingCacheDfxEnabled_ = false;
    drawable->DrawDfxForCacheInfo(paintFilterCanvas, renderParams);
    ASSERT_FALSE(drawable->isDrawingCacheDfxEnabled_);
}

/**
 * @tc.name: InitCachedSurface
 * @tc.desc: Test If InitCachedSurface Can Run
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSRenderNodeDrawableTest, InitCachedSurfaceTest, TestSize.Level1)
{
    auto drawable = RSRenderNodeDrawableTest::CreateDrawable();

    Drawing::Canvas canvas;
    RSRenderParams params(RSRenderNodeDrawableTest::id);
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    drawable->InitCachedSurface(paintFilterCanvas.GetGPUContext().get(), params.GetCacheSize(), 0xFF);
    ASSERT_EQ(drawable->cachedSurface_, nullptr);
}

/**
 * @tc.name: NeedInitCachedSurface
 * @tc.desc: Test If NeedInitCachedSurface Can Run
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSRenderNodeDrawableTest, NeedInitCachedSurfaceTest, TestSize.Level1)
{
    auto drawable = RSRenderNodeDrawableTest::CreateDrawable();

    Drawing::Canvas canvas;
    RSRenderParams params(RSRenderNodeDrawableTest::id);
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    drawable->opincDrawCache_.isDrawAreaEnable_ = DrawAreaEnableState::DRAW_AREA_DISABLE;
    drawable->cachedSurface_ = nullptr;
    auto result = drawable->NeedInitCachedSurface(params.GetCacheSize());
    ASSERT_EQ(result, true);

    drawable->InitCachedSurface(paintFilterCanvas.GetGPUContext().get(), params.GetCacheSize(), 0xFF);
    drawable->opincDrawCache_.isDrawAreaEnable_ = DrawAreaEnableState::DRAW_AREA_ENABLE;
    result = drawable->NeedInitCachedSurface(params.GetCacheSize());
}

/**
 * @tc.name: GetCachedImage
 * @tc.desc: Test If GetCachedImage Can Run
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSRenderNodeDrawableTest, GetCachedImageTest, TestSize.Level1)
{
    auto drawable = RSRenderNodeDrawableTest::CreateDrawable();

    Drawing::Canvas canvas;
    RSRenderParams params(RSRenderNodeDrawableTest::id);
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    drawable->cachedSurface_ = nullptr;
    auto result = drawable->GetCachedImage(paintFilterCanvas);
    ASSERT_EQ(result, nullptr);

    drawable->InitCachedSurface(paintFilterCanvas.GetGPUContext().get(), params.GetCacheSize(), 0xFF);
    result = drawable->GetCachedImage(paintFilterCanvas);
    ASSERT_EQ(result, nullptr);

    drawable->cachedSurface_ = std::make_shared<Drawing::Surface>();
    drawable->cachedImage_ = std::make_shared<Drawing::Image>();
    auto drawingCanvas = std::make_shared<Drawing::Canvas>();
    paintFilterCanvas.canvas_ = drawingCanvas.get();
    paintFilterCanvas.canvas_->gpuContext_ = std::make_shared<Drawing::GPUContext>();
    result = drawable->GetCachedImage(paintFilterCanvas);
    ASSERT_EQ(result, nullptr);
}

/**
 * @tc.name: DrawCachedImage
 * @tc.desc: Test If DrawCachedImage Can Run
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSRenderNodeDrawableTest, DrawCachedImageTest, TestSize.Level1)
{
    auto drawable = RSRenderNodeDrawableTest::CreateDrawable();

    Drawing::Canvas canvas;
    RSRenderParams params(RSRenderNodeDrawableTest::id);
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    drawable->DrawCachedImage(paintFilterCanvas, params.GetCacheSize());
    drawable->ClearCachedSurface();

    Drawing::Canvas canvas1(0, 0); // width and height
    RSPaintFilterCanvas paintFilterCanvas1(&canvas1);
    drawable->DrawCachedImage(paintFilterCanvas1, params.GetCacheSize());
    drawable->ClearCachedSurface();

    Drawing::Canvas canvas2(10, 10); // width and height
    RSPaintFilterCanvas paintFilterCanvas2(&canvas2);
    drawable->DrawCachedImage(paintFilterCanvas2, params.GetCacheSize());

    auto rsFilter = std::make_shared<RSFilter>();
    drawable->DrawCachedImage(paintFilterCanvas2, params.GetCacheSize(), rsFilter);
    drawable->opincDrawCache_.isDrawAreaEnable_ = DrawAreaEnableState::DRAW_AREA_ENABLE;
    drawable->DrawCachedImage(paintFilterCanvas2, params.GetCacheSize());
    drawable->ClearCachedSurface();
    drawable->opincDrawCache_.isDrawAreaEnable_ = DrawAreaEnableState::DRAW_AREA_INIT;

    drawable->cachedSurface_ = std::make_shared<Drawing::Surface>();
    drawable->cachedImage_ = std::make_shared<Drawing::Image>();
    auto drawingCanvas = std::make_shared<Drawing::Canvas>();
    paintFilterCanvas.canvas_ = drawingCanvas.get();
    paintFilterCanvas.canvas_->gpuContext_ = std::make_shared<Drawing::GPUContext>();
    drawable->DrawCachedImage(paintFilterCanvas, params.GetCacheSize());
    drawable->ClearCachedSurface();
    ASSERT_FALSE(RSSystemProperties::GetRecordingEnabled());
    ASSERT_FALSE(drawable->opincDrawCache_.IsComputeDrawAreaSucc());
}

/**
 * @tc.name: CheckIfNeedUpdateCache
 * @tc.desc: Test If CheckIfNeedUpdateCache Can Run
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSRenderNodeDrawableTest, CheckIfNeedUpdateCacheTest, TestSize.Level1)
{
    auto drawable = RSRenderNodeDrawableTest::CreateDrawable();
    Drawing::Canvas canvas;
    RSRenderParams params(RSRenderNodeDrawableTest::id);
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    int32_t updateTimes = 0;
    auto result = drawable->CheckIfNeedUpdateCache(params, updateTimes);
    ASSERT_FALSE(result);

    params.cacheSize_.x_ = 1.f;
    params.drawingCacheType_ = RSDrawingCacheType::TARGETED_CACHE;
    updateTimes = 0;
    result = drawable->CheckIfNeedUpdateCache(params, updateTimes);
    ASSERT_FALSE(result);
    params.cacheSize_.y_ = 1.f;
    params.needFilter_ = true;
    updateTimes = 0;
    result = drawable->CheckIfNeedUpdateCache(params, updateTimes);
    ASSERT_TRUE(result);

    drawable->cachedSurface_ = std::make_shared<Drawing::Surface>();
    drawable->cachedSurface_->cachedCanvas_ = std::make_shared<Drawing::Canvas>();
    drawable->opincDrawCache_.isDrawAreaEnable_ = DrawAreaEnableState::DRAW_AREA_ENABLE;
    Drawing::Rect unionRect(0.f, 0.f, 0.f, 0.f);
    drawable->opincDrawCache_.opListDrawAreas_.opInfo_.unionRect = unionRect;
    updateTimes = 0;
    result = drawable->CheckIfNeedUpdateCache(params, updateTimes);
    ASSERT_TRUE(result);

    drawable->drawingCacheUpdateTimeMap_.clear();
    updateTimes = 1;
    result = drawable->CheckIfNeedUpdateCache(params, updateTimes);
    ASSERT_FALSE(result);
    updateTimes = 1;
    params.isDrawingCacheChanged_ = true;
    result = drawable->CheckIfNeedUpdateCache(params, updateTimes);
    ASSERT_TRUE(result);

    params.drawingCacheIncludeProperty_ = true;
    params.drawingCacheType_ = RSDrawingCacheType::TARGETED_CACHE;
    params.needFilter_ = true;
    updateTimes = 0;
    result = drawable->CheckIfNeedUpdateCache(params, updateTimes);
    ASSERT_FALSE(result);
    params.freezeFlag_ = true;
    updateTimes = 0;
    result = drawable->CheckIfNeedUpdateCache(params, updateTimes);
    ASSERT_TRUE(result);
}

/**
 * @tc.name: Draw
 * @tc.desc: Test If Draw Can Run
 * @tc.type: FUNC
 * @tc.require: issueIAEDYI
 */
HWTEST_F(RSRenderNodeDrawableTest, DrawTest, TestSize.Level1)
{
    auto drawable = RSRenderNodeDrawableTest::CreateDrawable();
    Drawing::Canvas canvas;
    drawable->Draw(canvas);
    ASSERT_FALSE(RSUniRenderThread::IsInCaptureProcess());

    CaptureParam param;
    param.isSnapshot_ = true;
    RSUniRenderThread::SetCaptureParam(param);
    ASSERT_TRUE(RSUniRenderThread::IsInCaptureProcess());
}

/**
 * @tc.name: DrawWithoutChild
 * @tc.desc: DrawWithoutChild
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeDrawableTest, DrawWithoutChild, TestSize.Level1)
{
    auto drawable = RSRenderNodeDrawableTest::CreateDrawable();
    Drawing::Canvas canvas;
    canvas.SetUICapture(true);
    NodeId id = 1;
    RSUniRenderThread::GetCaptureParam().endNodeId_ = id;
    drawable->OnDraw(canvas);
    ASSERT_TRUE(RSUniRenderThread::IsInCaptureProcess());

    CaptureParam param;
    param.isSnapshot_ = true;
    RSUniRenderThread::GetCaptureParam().endNodeId_ = INVALID_NODEID;
    RSUniRenderThread::SetCaptureParam(param);
    ASSERT_TRUE(RSUniRenderThread::IsInCaptureProcess());
}

/**
 * @tc.name: TraverseSubTreeAndDrawFilterWithClip
 * @tc.desc: Test If TraverseSubTreeAndDrawFilterWithClip Can Run
 * @tc.type: FUNC
 * @tc.require: issueIAEDYI
 */
HWTEST_F(RSRenderNodeDrawableTest, TraverseSubTreeAndDrawFilterWithClipTest, TestSize.Level1)
{
    auto drawable = RSRenderNodeDrawableTest::CreateDrawable();

    Drawing::Canvas canvas;
    RSRenderParams params(RSRenderNodeDrawableTest::id);
    drawable->TraverseSubTreeAndDrawFilterWithClip(canvas, params);
    ASSERT_TRUE(drawable->filterInfoVec_.empty());

    Drawing::RectI rect;
    Drawing::Matrix matrix;
    drawable->filterInfoVec_.emplace_back(RSRenderNodeDrawableAdapter::FilterNodeInfo(0, matrix, { rect }));
    drawable->TraverseSubTreeAndDrawFilterWithClip(canvas, params);
    ASSERT_FALSE(drawable->filterInfoVec_.empty());
}


/**
 * @tc.name: DrawWithoutNodeGroupCache
 * @tc.desc: Test If DrawWithoutNodeGroupCache Can Run
 * @tc.type: FUNC
 * @tc.require: issueIAVPAJ
 */
HWTEST_F(RSRenderNodeDrawableTest, DrawWithoutNodeGroupCache, TestSize.Level1)
{
    auto drawable = RSRenderNodeDrawableTest::CreateDrawable();
    Drawing::Canvas canvas;
    RSRenderParams params(RSRenderNodeDrawableTest::id);
    DrawableCacheType originalCacheType = DrawableCacheType::CONTENT;
    drawable->DrawWithoutNodeGroupCache(canvas, params, originalCacheType);
    ASSERT_TRUE(drawable->GetCacheType() == DrawableCacheType::CONTENT);

    NodeId id = 1;
    auto rootRenderNode = std::make_shared<RSRenderNode>(id);
    auto rootDrawable = RSRenderNodeDrawable::OnGenerate(rootRenderNode);
    drawable->drawBlurForCache_ = true;
    drawable->curDrawingCacheRoot_ = rootDrawable;
    drawable->DrawWithoutNodeGroupCache(canvas, params, originalCacheType);
    ASSERT_TRUE(drawable->GetCacheType() == DrawableCacheType::CONTENT);
}

/**
 * @tc.name: ClearDrawingCacheDataMapTest
 * @tc.desc: Test If ClearDrawingCacheDataMap Can Run
 * @tc.type: FUNC
 * @tc.require: issueIAVPAJ
 */
HWTEST_F(RSRenderNodeDrawableTest, ClearDrawingCacheDataMapTest, TestSize.Level1)
{
    auto renderNode1 = std::make_shared<RSRenderNode>(1);
    auto renderNode2 = std::make_shared<RSRenderNode>(2);

    auto drawable1 = std::static_pointer_cast<RSRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(renderNode1));
    auto drawable2 = std::static_pointer_cast<RSRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(renderNode2));
    EXPECT_NE(drawable1, nullptr);
    EXPECT_NE(drawable2, nullptr);

    RSRenderNodeDrawable::drawingCacheUpdateTimeMap_[drawable1->GetId()]++;
    RSRenderNodeDrawable::drawingCacheUpdateTimeMap_[drawable2->GetId()]++;
    EXPECT_EQ(RSRenderNodeDrawable::drawingCacheUpdateTimeMap_[drawable1->GetId()], 1);
    EXPECT_EQ(RSRenderNodeDrawable::drawingCacheUpdateTimeMap_[drawable2->GetId()], 1);

    drawable1->ClearDrawingCacheDataMap();
    EXPECT_EQ(RSRenderNodeDrawable::drawingCacheUpdateTimeMap_.count(id), 0);
    EXPECT_EQ(RSRenderNodeDrawable::drawingCacheUpdateTimeMap_.count(drawable2->GetId()), 1);
}

/**
 * @tc.name: ClearDrawingCacheContiUpdateTimeMapTest
 * @tc.desc: Test If ClearDrawingCacheContiUpdateTimeMap Can Run
 * @tc.type: FUNC
 * @tc.require: issueIAVPAJ
 */
HWTEST_F(RSRenderNodeDrawableTest, ClearDrawingCacheContiUpdateTimeMapTest, TestSize.Level1)
{
    auto renderNode1 = std::make_shared<RSRenderNode>(1);
    auto renderNode2 = std::make_shared<RSRenderNode>(2);

    auto drawable1 = std::static_pointer_cast<RSRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(renderNode1));
    auto drawable2 = std::static_pointer_cast<RSRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(renderNode2));
    EXPECT_NE(drawable1, nullptr);
    EXPECT_NE(drawable2, nullptr);

    RSRenderNodeDrawable::drawingCacheContinuousUpdateTimeMap_[drawable1->GetId()]++;
    RSRenderNodeDrawable::drawingCacheContinuousUpdateTimeMap_[drawable2->GetId()]++;
    EXPECT_EQ(RSRenderNodeDrawable::drawingCacheContinuousUpdateTimeMap_[drawable1->GetId()], 1);
    EXPECT_EQ(RSRenderNodeDrawable::drawingCacheContinuousUpdateTimeMap_[drawable2->GetId()], 1);

    drawable1->ClearDrawingCacheContiUpdateTimeMap();
    EXPECT_EQ(RSRenderNodeDrawable::drawingCacheContinuousUpdateTimeMap_.count(id), 0);
    EXPECT_EQ(RSRenderNodeDrawable::drawingCacheContinuousUpdateTimeMap_.count(drawable2->GetId()), 1);
}

/**
 * @tc.name: SkipCulledNodeOrEntireSubtree001
 * @tc.desc: Test SkipCulledNodeOrEntireSubtree with node can not be skipped
 * @tc.type: FUNC
 * @tc.require: issueICA6FQ
 */
HWTEST_F(RSRenderNodeDrawableTest, SkipCulledNodeOrEntireSubtree001, TestSize.Level1)
{
    auto drawable = RSRenderNodeDrawableTest::CreateDrawable();
    ASSERT_NE(drawable, nullptr);
    drawable->SetOcclusionCullingEnabled(true);
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);

    std::unordered_set<NodeId> culledNodes{INVALID_NODEID};
    paintFilterCanvas.SetCulledNodes(std::move(culledNodes));
    std::unordered_set<NodeId> culledEntireSubtree{INVALID_NODEID};
    paintFilterCanvas.SetCulledEntireSubtree(std::move(culledEntireSubtree));

    Drawing::Rect bounds;
    EXPECT_EQ(drawable->SkipCulledNodeOrEntireSubtree(paintFilterCanvas, bounds), false);
}

/**
 * @tc.name: SkipCulledNodeOrEntireSubtree003
 * @tc.desc: Test SkipCulledNodeOrEntireSubtree with entire subtree can be skipped
 * @tc.type: FUNC
 * @tc.require: issueICA6FQ
 */
HWTEST_F(RSRenderNodeDrawableTest, SkipCulledNodeOrEntireSubtree003, TestSize.Level1)
{
    auto drawable = RSRenderNodeDrawableTest::CreateDrawable();
    ASSERT_NE(drawable, nullptr);
    drawable->SetOcclusionCullingEnabled(true);
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);

    std::unordered_set<NodeId> culledEntireSubtree{drawable->GetId()};
    paintFilterCanvas.SetCulledEntireSubtree(std::move(culledEntireSubtree));

    Drawing::Rect bounds;
    EXPECT_EQ(drawable->SkipCulledNodeOrEntireSubtree(paintFilterCanvas, bounds), true);
}

/**
 * @tc.name: CheckRegionAndDrawWithoutFilter
 * @tc.desc: Test If CheckRegionAndDrawWithoutFilter Can Run
 * @tc.type: FUNC
 * @tc.require: issueIAVPAJ
 */
HWTEST_F(RSRenderNodeDrawableTest, CheckRegionAndDrawWithoutFilter, TestSize.Level1)
{
    auto drawable = RSRenderNodeDrawableTest::CreateDrawable();
    Drawing::Canvas canvas;
    RSRenderParams params(RSRenderNodeDrawableTest::id);
    Drawing::RectI rect;
    Drawing::Matrix matrix;
    std::vector<RSRenderNodeDrawableAdapter::FilterNodeInfo> filterInfoVec = {
        RSRenderNodeDrawableAdapter::FilterNodeInfo(0, matrix, { rect })
    };
    drawable->filterInfoVec_ = filterInfoVec;
    drawable->CheckRegionAndDrawWithoutFilter(filterInfoVec, canvas, params);
    ASSERT_FALSE(drawable->filterInfoVec_.empty());

    NodeId id = 1;
    auto rootRenderNode = std::make_shared<RSRenderNode>(id);
    auto rootDrawable = RSRenderNodeDrawable::OnGenerate(rootRenderNode);
    drawable->curDrawingCacheRoot_ = rootDrawable;

    drawable->CheckRegionAndDrawWithoutFilter(filterInfoVec, canvas, params);
    ASSERT_FALSE(drawable->filterInfoVec_.empty());
}

/**
 * @tc.name: CheckRegionAndDrawWithFilter
 * @tc.desc: Test If CheckRegionAndDrawWithFilter Can Run
 * @tc.type: FUNC
 * @tc.require: issueIAVPAJ
 */
HWTEST_F(RSRenderNodeDrawableTest, CheckRegionAndDrawWithFilter, TestSize.Level1)
{
    auto drawable = RSRenderNodeDrawableTest::CreateDrawable();
    Drawing::Canvas canvas;
    RSRenderParams params(RSRenderNodeDrawableTest::id);
    Drawing::RectI rect;
    Drawing::Matrix matrix;
    const std::vector<RSRenderNodeDrawableAdapter::FilterNodeInfo> filterInfoVec = {
        RSRenderNodeDrawableAdapter::FilterNodeInfo(0, matrix, { rect })
    };
    drawable->filterInfoVec_ = filterInfoVec;
    auto begin = std::find_if(filterInfoVec.begin(), filterInfoVec.end(),
        [nodeId = 0](const auto& item) -> bool { return item.nodeId_ == nodeId; });
    drawable->CheckRegionAndDrawWithFilter(begin, filterInfoVec, canvas, params);
    ASSERT_FALSE(drawable->filterInfoVec_.empty());

    NodeId id = 1;
    auto rootRenderNode = std::make_shared<RSRenderNode>(id);
    auto rootDrawable = RSRenderNodeDrawable::OnGenerate(rootRenderNode);
    drawable->curDrawingCacheRoot_ = rootDrawable;
    drawable->CheckRegionAndDrawWithFilter(begin, filterInfoVec, canvas, params);
    ASSERT_FALSE(drawable->filterInfoVec_.empty());
}

/**
 * @tc.name: UpdateCacheSurface
 * @tc.desc: Test If UpdateCacheSurface Can Run
 * @tc.type: FUNC
 * @tc.require: issueIB1KMY
 */
HWTEST_F(RSRenderNodeDrawableTest, UpdateCacheSurfaceTest, TestSize.Level1)
{
    auto drawable = RSRenderNodeDrawableTest::CreateDrawable();
    Drawing::Canvas canvas;
    RSRenderParams params(RSRenderNodeDrawableTest::id);
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    drawable->UpdateCacheSurface(paintFilterCanvas, params);
    ASSERT_FALSE(params.GetRSFreezeFlag());

    Drawing::Canvas canvas1(10, 10); // width and height
    RSPaintFilterCanvas paintFilterCanvas1(&canvas1);
    drawable->UpdateCacheSurface(paintFilterCanvas1, params);
    ASSERT_EQ(params.foregroundFilterCache_, nullptr);

    params.freezeFlag_ = true;
    bool includeProperty = true;
    params.SetDrawingCacheIncludeProperty(includeProperty);
    drawable->UpdateCacheSurface(paintFilterCanvas1, params);
    ASSERT_EQ(params.foregroundFilterCache_, nullptr);
    ASSERT_TRUE(params.GetRSFreezeFlag());


    params.foregroundFilterCache_ = std::make_shared<RSFilter>();
    drawable->UpdateCacheSurface(paintFilterCanvas1, params);
    ASSERT_NE(params.foregroundFilterCache_, nullptr);
}

/**
 * @tc.name: DealWithWhiteListNodes001
 * @tc.desc: Test If DealWithWhiteListNodes Can Run
 * @tc.type: FUNC
 * @tc.require: issueICF7P6
 */
HWTEST_F(RSRenderNodeDrawableTest, DealWithWhiteListNodes001, TestSize.Level1)
{
    NodeId nodeId = 1;
    auto node = std::make_shared<RSRenderNode>(nodeId);
    auto drawable = std::make_shared<RSRenderNodeDrawable>(std::move(node));
    int width = 1024;
    int height = 1920;
    Drawing::Canvas canvas(width, height);
    CaptureParam params;
    params.isMirror_ = true;
    std::unordered_set<NodeId> whiteList = {nodeId};
    RSUniRenderThread::Instance().SetWhiteList(whiteList);
    params.rootIdInWhiteList_ = INVALID_NODEID;
    RSUniRenderThread::SetCaptureParam(params);

    drawable->renderParams_ = nullptr;
    ASSERT_TRUE(drawable->DealWithWhiteListNodes(canvas));
    ASSERT_EQ(drawable->GetRenderParams(), nullptr);
    drawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    ASSERT_TRUE(drawable->DealWithWhiteListNodes(canvas));

    ScreenId screenid = 1;
    params.virtualScreenId_ = screenid;
    std::unordered_map<ScreenId, bool> info;
    info[screenid] = true;
    drawable->renderParams_->SetVirtualScreenWhiteListInfo(info);
    RSUniRenderThread::SetCaptureParam(params);
    ASSERT_TRUE(drawable->DealWithWhiteListNodes(canvas));

    info.clear();
    info[screenid + 1] = true;
    drawable->renderParams_->SetVirtualScreenWhiteListInfo(info);
    ASSERT_TRUE(drawable->DealWithWhiteListNodes(canvas));
}

/**
 * @tc.name: DealWithWhiteListNodes002
 * @tc.desc: Test If DealWithWhiteListNodes Can Run
 * @tc.type: FUNC
 * @tc.require: issueICF7P6
 */
HWTEST_F(RSRenderNodeDrawableTest, DealWithWhiteListNodes002, TestSize.Level1)
{
    NodeId nodeId = 1;
    auto node = std::make_shared<RSRenderNode>(nodeId);
    auto drawable = std::make_shared<RSRenderNodeDrawable>(std::move(node));
    int width = 1024;
    int height = 1920;
    Drawing::Canvas canvas(width, height);
    drawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    CaptureParam params;
    params.isMirror_ = true;
    std::unordered_set<NodeId> whiteList = {nodeId};
    RSUniRenderThread::Instance().SetWhiteList(whiteList);
    params.rootIdInWhiteList_ = nodeId;
    RSUniRenderThread::SetCaptureParam(params);
    ASSERT_FALSE(drawable->DealWithWhiteListNodes(canvas));

    params.isMirror_ = true;
    RSUniRenderThread::Instance().SetWhiteList({});
    RSUniRenderThread::SetCaptureParam(params);
    ASSERT_FALSE(drawable->DealWithWhiteListNodes(canvas));

    params.isMirror_ = false;
    RSUniRenderThread::SetCaptureParam(params);
    ASSERT_FALSE(drawable->DealWithWhiteListNodes(canvas));
}

/**
 * @tc.name: ProcessedNodeCount
 * @tc.desc: Test ProcessedNodeCount
 * @tc.type: FUNC
 * @tc.require: issueIB1KMY
 */
HWTEST_F(RSRenderNodeDrawableTest, ProcessedNodeCountTest, TestSize.Level1)
{
    auto drawable = RSRenderNodeDrawableTest::CreateDrawable();

    drawable->ClearTotalProcessedNodeCount();
    drawable->ClearSnapshotProcessedNodeCount();
    drawable->ClearProcessedNodeCount();
    drawable->TotalProcessedNodeCountInc();
    CaptureParam param;
    param.isSingleSurface_ = true;
    RSUniRenderThread::SetCaptureParam(param);
    drawable->SnapshotProcessedNodeCountInc();
    drawable->ProcessedNodeCountInc();
    ASSERT_EQ(drawable->GetTotalProcessedNodeCount(), 1);
    ASSERT_EQ(drawable->GetSnapshotProcessedNodeCount(), 1);
    ASSERT_EQ(drawable->GetProcessedNodeCount(), 1);

    drawable->ClearTotalProcessedNodeCount();
    drawable->ClearSnapshotProcessedNodeCount();
    drawable->ClearProcessedNodeCount();
    ASSERT_EQ(drawable->GetTotalProcessedNodeCount(), 0);
    ASSERT_EQ(drawable->GetSnapshotProcessedNodeCount(), 0);
    ASSERT_EQ(drawable->GetProcessedNodeCount(), 0);
}

/**
 * @tc.name: UpdateFilterDisplayHeadroomTest
 * @tc.desc: Test UpdateFilterDisplayHeadroom
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeDrawableTest, UpdateFilterDisplayHeadroomTest, TestSize.Level1)
{
    NodeId id = 0;
    auto drawable = RSRenderNodeDrawableTest::CreateDrawable();
    RSEffectLuminanceManager::GetInstance().SetDisplayHeadroom(id, 1.5f);

    Drawing::Canvas canvas;
    drawable->UpdateFilterDisplayHeadroom(canvas);

    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    paintFilterCanvas.SetScreenId(id);
    drawable->UpdateFilterDisplayHeadroom(paintFilterCanvas);
    EXPECT_NE(drawable->GetRenderParams(), nullptr);

    const auto& params = drawable->GetRenderParams();
    auto filter = std::make_shared<RSDrawingFilter>(std::make_shared<RSRenderFilterParaBase>());
    params->foregroundFilterCache_ = filter;
    params->backgroundFilter_ = filter;

    drawable->UpdateFilterDisplayHeadroom(paintFilterCanvas);
    EXPECT_EQ(filter->shaderFilters_.size(), 1);
}

/**
 * @tc.name: OnDrawTest
 * @tc.desc: Test OnDraw
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeDrawableTest, OnDrawTest, TestSize.Level1)
{
    auto drawable = RSRenderNodeDrawableTest::CreateDrawable();
    Drawing::Canvas canvas;
    auto pCanvas = std::make_shared<RSPaintFilterCanvas>(&canvas);
    ASSERT_TRUE(pCanvas != nullptr);

    drawable->OnDraw(*pCanvas);
    pCanvas->SetQuickDraw(true);
    drawable->OnDraw(*pCanvas);
}
}
