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
#include "params/rs_render_params.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::DrawableV2;

namespace OHOS::Rosen {
constexpr NodeId DEFAULT_ID = 0xFFFF;
class RSNodeDrawableTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline NodeId id = DEFAULT_ID;
    static std::shared_ptr<RSRenderNodeDrawable> CreateDrawable();
};

void RSNodeDrawableTest::SetUpTestCase() {}
void RSNodeDrawableTest::TearDownTestCase() {}
void RSNodeDrawableTest::SetUp() {}
void RSNodeDrawableTest::TearDown() {}

std::shared_ptr<RSRenderNodeDrawable> RSNodeDrawableTest::CreateDrawable()
{
    auto renderNode = std::make_shared<RSRenderNode>(id);
    auto drawable = std::static_pointer_cast<RSRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(renderNode));
    return drawable;
}

/**
 * @tc.name: OpincCalculateBeforeExtTest
 * @tc.desc: Test OpincCalculateBefore
 * @tc.type: FUNC
 * @tc.require: issueI9SPVO
 */
HWTEST_F(RSNodeDrawableTest, OpincCalculateBeforeExtTest, TestSize.Level1)
{
    auto drawable = RSNodeDrawableTest::CreateDrawable();

    Drawing::Canvas canvas;
    RSRenderParams params(RSNodeDrawableTest::id);
    bool isOpincDropNodeExt = true;
    drawable->OpincCalculateBefore(canvas, params, isOpincDropNodeExt);

    drawable->rootNodeStragyType_ = NodeStrategyType::OPINC_AUTOCACHE;
    drawable->recordState_ = NodeRecordState::RECORD_CALCULATE;
    drawable->OpincCalculateBefore(canvas, params, isOpincDropNodeExt);
}

/**
 * @tc.name: CreateRenderNodeDrawableExtTest
 * @tc.desc: Test If RenderNodeDrawable Can Be Created
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSNodeDrawableTest, CreateRenderNodeDrawableExtTest, TestSize.Level1)
{
    NodeId id = 1;
    auto renderNode = std::make_shared<RSRenderNode>(id);
    auto drawable = RSRenderNodeDrawable::OnGenerate(renderNode);
    ASSERT_NE(drawable, nullptr);
}

/**
 * @tc.name: PreDrawableCacheStateExtTest
 * @tc.desc: Test result of PreDrawableCacheState
 * @tc.type: FUNC
 * @tc.require: issueI9SPVO
 */
HWTEST_F(RSNodeDrawableTest, PreDrawableCacheStateExtTest, TestSize.Level1)
{
    auto drawable = RSNodeDrawableTest::CreateDrawable();

    RSRenderParams params(RSNodeDrawableTest::id);
    params.OpincSetCacheChangeFlag(true, true);
    drawable->isOpincRootNode_ = true;
    bool isOpincDropNodeExt = true;
    ASSERT_TRUE(drawable->PreDrawableCacheState(params, isOpincDropNodeExt));

    params.OpincSetCacheChangeFlag(false, true);
    drawable->isOpincRootNode_ = false;
    isOpincDropNodeExt = false;
    ASSERT_FALSE(drawable->PreDrawableCacheState(params, isOpincDropNodeExt));
}

/**
 * @tc.name: OpincCalculateAfterExtTest
 * @tc.desc: Test OpincCalculateAfter
 * @tc.type: FUNC
 * @tc.require: issueI9SPVO
 */
HWTEST_F(RSNodeDrawableTest, OpincCalculateAfterExtTest, TestSize.Level1)
{
    auto drawable = RSNodeDrawableTest::CreateDrawable();

    Drawing::Canvas canvas;
    bool isOpincDropNodeExt = true;
    drawable->OpincCalculateAfter(canvas, isOpincDropNodeExt);

    drawable->isOpincCaculateStart_ = true;
    drawable->OpincCalculateAfter(canvas, isOpincDropNodeExt);
}

/**
 * @tc.name: ResumeOpincCanvasTranslateExtTest
 * @tc.desc: Test ResumeOpincCanvasTranslate
 * @tc.type: FUNC
 * @tc.require: issueI9SPVO
 */
HWTEST_F(RSNodeDrawableTest, ResumeOpincCanvasTranslateExtTest, TestSize.Level1)
{
    auto drawable = RSNodeDrawableTest::CreateDrawable();

    drawable->isDrawAreaEnable_ = DrawAreaEnableState::DRAW_AREA_INIT;
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    drawable->ResumeOpincCanvasTranslate(paintFilterCanvas);

    drawable->isDrawAreaEnable_ = DrawAreaEnableState::DRAW_AREA_ENABLE;
    drawable->ResumeOpincCanvasTranslate(paintFilterCanvas);
}

/**
 * @tc.name: OpincCanvasUnionTranslateExtTest
 * @tc.desc: Test OpincCanvasUnionTranslate
 * @tc.type: FUNC
 * @tc.require: issueI9SPVO
 */
HWTEST_F(RSNodeDrawableTest, OpincCanvasUnionTranslateExtTest, TestSize.Level1)
{
    auto drawable = RSNodeDrawableTest::CreateDrawable();

    drawable->isDrawAreaEnable_ = DrawAreaEnableState::DRAW_AREA_INIT;
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    drawable->OpincCanvasUnionTranslate(paintFilterCanvas);

    drawable->isDrawAreaEnable_ = DrawAreaEnableState::DRAW_AREA_ENABLE;
    drawable->OpincCanvasUnionTranslate(paintFilterCanvas);
}

/**
 * @tc.name: IsOpListDrawAreaEnableExtTest
 * @tc.desc: Test result of IsOpListDrawAreaEnable
 * @tc.type: FUNC
 * @tc.require: issueI9SPVO
 */
HWTEST_F(RSNodeDrawableTest, IsOpListDrawAreaEnableExtTest, TestSize.Level1)
{
    auto drawable = RSNodeDrawableTest::CreateDrawable();
    ASSERT_FALSE(drawable->IsOpListDrawAreaEnable());

    drawable->recordState_ = NodeRecordState::RECORD_CALCULATE;
    ASSERT_FALSE(drawable->IsOpListDrawAreaEnable());

    drawable->rootNodeStragyType_ = NodeStrategyType::OPINC_AUTOCACHE;
    ASSERT_TRUE(drawable->IsOpListDrawAreaEnable());
}

/**
 * @tc.name: DrawableCacheStateResetExtTest
 * @tc.desc: Test drawable cache state after reset
 * @tc.type: FUNC
 * @tc.require: issueI9SPVO
 */
HWTEST_F(RSNodeDrawableTest, DrawableCacheStateResetExtTest, TestSize.Level1)
{
    auto drawable = RSNodeDrawableTest::CreateDrawable();
    RSRenderParams params(RSNodeDrawableTest::id);
    drawable->DrawableCacheStateReset(params);

    ASSERT_EQ(drawable->isDrawAreaEnable_, DrawAreaEnableState::DRAW_AREA_INIT);
    ASSERT_EQ(drawable->rootNodeStragyType_, NodeStrategyType::CACHE_NONE);
    ASSERT_EQ(drawable->temNodeStragyType_, NodeStrategyType::CACHE_NONE);
    ASSERT_EQ(drawable->recordState_, NodeRecordState::RECORD_NONE);
    ASSERT_EQ(drawable->isOpincRootNode_, false);
    ASSERT_EQ(drawable->opCanCache_, false);
}

/**
 * @tc.name: NodeCacheStateDisableExtTest
 * @tc.desc: Test drawable state after disable cache state
 * @tc.type: FUNC
 * @tc.require: issueI9SPVO
 */
HWTEST_F(RSNodeDrawableTest, NodeCacheStateDisableExtTest, TestSize.Level1)
{
    auto drawable = RSNodeDrawableTest::CreateDrawable();

    drawable->opCanCache_ = false;
    drawable->NodeCacheStateDisable();
    ASSERT_EQ(drawable->recordState_, NodeRecordState::RECORD_DISABLE);
    ASSERT_EQ(drawable->rootNodeStragyType_, NodeStrategyType::CACHE_DISABLE);
    ASSERT_EQ(drawable->temNodeStragyType_, NodeStrategyType::CACHE_DISABLE);
    ASSERT_EQ(drawable->isDrawAreaEnable_, DrawAreaEnableState::DRAW_AREA_DISABLE);
    ASSERT_EQ(drawable->opCanCache_, false);

    drawable->opCanCache_ = true;
    drawable->NodeCacheStateDisable();
    ASSERT_EQ(drawable->opCanCache_, false);
}

/**
 * @tc.name: IsTranslateExtTest
 * @tc.desc: Test whether matrix is translate matrix
 * @tc.type: FUNC
 * @tc.require: issueI9SPVO
 */
HWTEST_F(RSNodeDrawableTest, IsTranslateExtTest, TestSize.Level1)
{
    auto drawable = RSNodeDrawableTest::CreateDrawable();

    Drawing::Matrix matrix;
    ASSERT_TRUE(drawable->IsTranslate(matrix));

    matrix.Translate(100, 200);
    ASSERT_TRUE(drawable->IsTranslate(matrix));

    matrix.SetScale(2, 2);
    ASSERT_FALSE(drawable->IsTranslate(matrix));
}

/**
 * @tc.name: BeforeDrawCacheFindRootNodeExtTest
 * @tc.desc: Test BeforeDrawCacheFindRootNode
 * @tc.type: FUNC
 * @tc.require: issueI9SPVO
 */
HWTEST_F(RSNodeDrawableTest, BeforeDrawCacheFindRootNodeExtTest, TestSize.Level1)
{
    auto drawable = RSNodeDrawableTest::CreateDrawable();

    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    RSRenderParams params(RSNodeDrawableTest::id);
    bool isOpincDropNodeExt = true;
    drawable->BeforeDrawCacheFindRootNode(paintFilterCanvas, params, isOpincDropNodeExt);

    params.isOpincRootFlag_ = true;
    params.SetCacheSize({100, 100});
    paintFilterCanvas.SetCacheType(Drawing::CacheType::ENABLED);
    drawable->BeforeDrawCacheFindRootNode(paintFilterCanvas, params, isOpincDropNodeExt);
}

/**
 * @tc.name: BeforeDrawCacheProcessChildNodeExtTest
 * @tc.desc: Test result of BeforeDrawCacheProcessChildNode
 * @tc.type: FUNC
 * @tc.require: issueI9SPVO
 */
HWTEST_F(RSNodeDrawableTest, BeforeDrawCacheProcessChildNodeExtTest, TestSize.Level1)
{
    auto drawable = RSNodeDrawableTest::CreateDrawable();

    RSRenderParams params(RSNodeDrawableTest::id);
    auto strategyType = NodeStrategyType::CACHE_NONE;
    params.isOpincRootFlag_ = true;
    ASSERT_TRUE(drawable->BeforeDrawCacheProcessChildNode(strategyType, params));

    strategyType = NodeStrategyType::DDGR_OPINC_DYNAMIC;
    ASSERT_FALSE(drawable->BeforeDrawCacheProcessChildNode(strategyType, params));

    drawable->recordState_ = NodeRecordState::RECORD_CACHED;
    drawable->rootNodeStragyType_ = NodeStrategyType::OPINC_AUTOCACHE;
    ASSERT_FALSE(drawable->BeforeDrawCacheProcessChildNode(strategyType, params));

    drawable->rootNodeStragyType_ = NodeStrategyType::CACHE_DISABLE;
    ASSERT_FALSE(drawable->BeforeDrawCacheProcessChildNode(strategyType, params));
}

/**
 * @tc.name: AfterDrawCacheExtTest
 * @tc.desc: Test AfterDrawCache
 * @tc.type: FUNC
 * @tc.require: issueI9SPVO
 */
HWTEST_F(RSNodeDrawableTest, AfterDrawCacheExtTest, TestSize.Level1)
{
    auto drawable = RSNodeDrawableTest::CreateDrawable();

    Drawing::Canvas canvas;
    RSRenderParams params(RSNodeDrawableTest::id);
    bool isOpincDropNodeExt = true;
    int opincRootTotalCount = 0;
    drawable->rootNodeStragyType_ = NodeStrategyType::OPINC_AUTOCACHE;
    drawable->recordState_ = NodeRecordState::RECORD_CALCULATE;
    NodeStrategyType cacheStragy = NodeStrategyType::CACHE_NONE;
    drawable->AfterDrawCache(cacheStragy, canvas, params, isOpincDropNodeExt, opincRootTotalCount);

    drawable->recordState_ = NodeRecordState::RECORD_CACHING;
    drawable->AfterDrawCache(cacheStragy, canvas, params, isOpincDropNodeExt, opincRootTotalCount);
}

/**
 * @tc.name: BeforeDrawCacheExtTest
 * @tc.desc: Test BeforeDrawCache
 * @tc.type: FUNC
 * @tc.require: issueI9SPVO
 */
HWTEST_F(RSNodeDrawableTest, BeforeDrawCacheExtTest, TestSize.Level1)
{
    auto drawable = RSNodeDrawableTest::CreateDrawable();

    Drawing::Canvas canvas;
    RSRenderParams params(RSNodeDrawableTest::id);
    bool isOpincDropNodeExt = true;
    drawable->recordState_ = NodeRecordState::RECORD_CACHED;
    NodeStrategyType strategyType = NodeStrategyType::DDGR_OPINC_DYNAMIC;
    drawable->BeforeDrawCache(strategyType, canvas, params, isOpincDropNodeExt);

    strategyType = NodeStrategyType::CACHE_NONE;
    drawable->recordState_ = NodeRecordState::RECORD_NONE;
    drawable->BeforeDrawCache(strategyType, canvas, params, isOpincDropNodeExt);
}

/**
 @tc.name: AfterDrawCacheWithScreenExtTest
 @tc.desc: Test AfterDrawCacheWithScreen delay cache
 @tc.type: FUNC
 @tc.require: issueIAL4RE
 */
HWTEST_F(RSNodeDrawableTest, AfterDrawCacheWithScreenExtTest, TestSize.Level1)
{
    auto drawable = RSNodeDrawableTest::CreateDrawable();
    Drawing::Canvas canvas;
    RSRenderParams params(RSNodeDrawableTest::id);
    bool isOpincDropNodeExt = true;
    int opincRootTotalCount = 0;
    drawable->rootNodeStragyType_ = NodeStrategyType::OPINC_AUTOCACHE;
    NodeStrategyType cacheStragy = NodeStrategyType::CACHE_NONE;
    drawable->recordState_ = NodeRecordState::RECORD_CACHING;
    RectI absRect = {10, 10, 10, 10};
    params.SetAbsDrawRect(absRect);
    drawable->AfterDrawCache(cacheStragy, canvas, params, isOpincDropNodeExt, opincRootTotalCount);
}

/**
 * @tc.name: DrawAutoCacheExtTest
 * @tc.desc: Test result of DrawAutoCache
 * @tc.type: FUNC
 * @tc.require: issueI9SPVO
 */
HWTEST_F(RSNodeDrawableTest, DrawAutoCacheExtTest, TestSize.Level1)
{
    auto drawable = RSNodeDrawableTest::CreateDrawable();

    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    Drawing::Image image;
    Drawing::SamplingOptions samplingOptions;
    drawable->opCanCache_ = true;
    drawable->isDrawAreaEnable_ = DrawAreaEnableState::DRAW_AREA_DISABLE;
    ASSERT_FALSE(drawable->DrawAutoCache(paintFilterCanvas, image, samplingOptions,
        Drawing::SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT));

    drawable->opCanCache_ = false;
    drawable->isDrawAreaEnable_ = DrawAreaEnableState::DRAW_AREA_ENABLE;
    ASSERT_FALSE(drawable->DrawAutoCache(paintFilterCanvas, image, samplingOptions,
        Drawing::SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT));

    drawable->opCanCache_ = false;
    drawable->isDrawAreaEnable_ = DrawAreaEnableState::DRAW_AREA_DISABLE;
    ASSERT_FALSE(drawable->DrawAutoCache(paintFilterCanvas, image, samplingOptions,
        Drawing::SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT));

    Drawing::OpListHandle::OpInfo opInfo = {
        .unionRect = Drawing::Rect { 100, 100, 300, 300 },
        .drawAreaRects = { Drawing::Rect { 100, 100, 300, 300 } }
    };
    drawable->opCanCache_ = true;
    drawable->opListDrawAreas_ = Drawing::OpListHandle(opInfo);
    drawable->isDrawAreaEnable_ = DrawAreaEnableState::DRAW_AREA_ENABLE;
    ASSERT_TRUE(drawable->DrawAutoCache(paintFilterCanvas, image, samplingOptions,
        Drawing::SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT));
}

/**
 * @tc.name: GenerateCacheIfNeedExtTest
 * @tc.desc: Test If GenerateCacheIfNeed Can Run
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSNodeDrawableTest, GenerateCacheIfNeedExtTest, TestSize.Level1)
{
    auto drawable = RSNodeDrawableTest::CreateDrawable();
    Drawing::Canvas canvas;
    RSRenderParams params(RSNodeDrawableTest::id);
    drawable->GenerateCacheIfNeed(canvas, params);
    ASSERT_FALSE(params.GetRSFreezeFlag());
    params.freezeFlag_ = true;
    drawable->GenerateCacheIfNeed(canvas, params);
    ASSERT_TRUE(params.GetRSFreezeFlag());
    drawable->isOpincMarkCached_ = true;
    drawable->GenerateCacheIfNeed(canvas, params);
    ASSERT_TRUE(drawable->OpincGetCachedMark());

    params.drawingCacheType_ = RSDrawingCacheType::FORCED_CACHE;
    drawable->GenerateCacheIfNeed(canvas, params);
    ASSERT_FALSE(drawable->cachedSurface_);
    drawable->cachedSurface_ = std::make_shared<Drawing::Surface>();
    drawable->GenerateCacheIfNeed(canvas, params);
    ASSERT_FALSE(params.ChildHasVisibleFilter());
    params.drawingCacheType_ = RSDrawingCacheType::TARGETED_CACHE;
    drawable->GenerateCacheIfNeed(canvas, params);
    ASSERT_TRUE(params.GetDrawingCacheType() == RSDrawingCacheType::TARGETED_CACHE);

    params.freezeFlag_ = false;
    params.isDrawingCacheChanged_ = true;
    drawable->drawingCacheUpdateTimeMap_[drawable->nodeId_] = 4;
    drawable->GenerateCacheIfNeed(canvas, params);
    ASSERT_FALSE(params.GetDrawingCacheChanged());
    params.isDrawingCacheChanged_ = false;
    drawable->GenerateCacheIfNeed(canvas, params);

    drawable->isOpincMarkCached_ = false;
    params.drawingCacheType_ = RSDrawingCacheType::FORCED_CACHE;
    drawable->GenerateCacheIfNeed(canvas, params);
    ASSERT_TRUE(!drawable->OpincGetCachedMark());
    params.freezeFlag_ = true;
    drawable->GenerateCacheIfNeed(canvas, params);
    ASSERT_TRUE(params.GetRSFreezeFlag());
    ASSERT_TRUE(params.GetDrawingCacheType() == RSDrawingCacheType::DISABLED_CACHE);
    params.childHasVisibleFilter_ = true;
    drawable->GenerateCacheIfNeed(canvas, params);
    ASSERT_TRUE(params.ChildHasVisibleFilter());
}

/**
 * @tc.name: DrawAutoCacheDfxExtTest
 * @tc.desc: Test DrawAutoCacheDfx
 * @tc.type: FUNC
 * @tc.require: issueI9SPVO
 */
HWTEST_F(RSNodeDrawableTest, DrawAutoCacheDfxExtTest, TestSize.Level1)
{
    auto drawable = RSNodeDrawableTest::CreateDrawable();

    std::vector<std::pair<RectI, std::string>> autoCacheRenderNodeInfos = {
        { RectI(100, 100, 200, 200), "" },
        { RectI(100, 100, 200, 200), "" }
    };
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    drawable->DrawAutoCacheDfx(paintFilterCanvas, autoCacheRenderNodeInfos);
}

/**
 * @tc.name: UpdateCacheInfoForDfxExtTest
 * @tc.desc: Test If UpdateCacheInfoForDfx Can Run
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSNodeDrawableTest, UpdateCacheInfoForDfxExtTest, TestSize.Level1)
{
    auto drawable = RSNodeDrawableTest::CreateDrawable();

    Drawing::Canvas canvas;
    RSRenderParams params(RSNodeDrawableTest::id);
    drawable->UpdateCacheInfoForDfx(canvas, params.GetBounds(), params.GetId());
    drawable->isDrawingCacheDfxEnabled_ = true;
    drawable->UpdateCacheInfoForDfx(canvas, params.GetBounds(), params.GetId());
    ASSERT_EQ(drawable->isDrawingCacheDfxEnabled_, true);
}

/**
 * @tc.name: CheckCacheTypeAndDrawExtTest
 * @tc.desc: Test If CheckCacheTypeAndDraw Can Run
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSNodeDrawableTest, CheckCacheTypeAndDrawExtTest, TestSize.Level1)
{
    auto drawable = RSNodeDrawableTest::CreateDrawable();
    Drawing::Canvas canvas;
    RSRenderParams params(RSNodeDrawableTest::id);
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

    auto curCanvas = static_cast<RSPaintFilterCanvas*>(&canvas);
    curCanvas->SetCacheType(RSPaintFilterCanvas::CacheType::DISABLED);
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
 * @tc.name: InitCachedSurfaceExtTest
 * @tc.desc: Test If InitCachedSurface Can Run
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSNodeDrawableTest, InitCachedSurfaceExtTest, TestSize.Level1)
{
    auto drawable = RSNodeDrawableTest::CreateDrawable();

    Drawing::Canvas canvas;
    RSRenderParams params(RSNodeDrawableTest::id);
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    drawable->InitCachedSurface(paintFilterCanvas.GetGPUContext().get(), params.GetCacheSize(), 0xFF);
    ASSERT_EQ(drawable->cachedSurface_, nullptr);
}

/**
 * @tc.name: DrawDfxForCacheInfoExtTest
 * @tc.desc: Test If DrawDfxForCacheInfo Can Run
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSNodeDrawableTest, DrawDfxForCacheInfoExtTest, TestSize.Level1)
{
    auto drawable = RSNodeDrawableTest::CreateDrawable();
    Drawing::Canvas canvas;
    RSRenderParams params(RSNodeDrawableTest::id);
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    drawable->DrawDfxForCacheInfo(paintFilterCanvas);
    ASSERT_FALSE(drawable->isDrawingCacheEnabled_);
    ASSERT_TRUE(drawable->isDrawingCacheDfxEnabled_);
    ASSERT_FALSE(drawable->autoCacheDrawingEnable_);
    ASSERT_FALSE(!drawable->isDrawingCacheDfxEnabled_);

    drawable->isDrawingCacheEnabled_ = true;
    drawable->autoCacheDrawingEnable_  = true;
    drawable->DrawDfxForCacheInfo(paintFilterCanvas);
    ASSERT_TRUE(drawable->isDrawingCacheEnabled_);

    drawable->isDrawingCacheDfxEnabled_ = false;
    drawable->DrawDfxForCacheInfo(paintFilterCanvas);
    ASSERT_FALSE(drawable->isDrawingCacheDfxEnabled_);
}

/**
 * @tc.name: GetCachedImageExtTest
 * @tc.desc: Test If GetCachedImage Can Run
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSNodeDrawableTest, GetCachedImageExtTest, TestSize.Level1)
{
    auto drawable = RSNodeDrawableTest::CreateDrawable();

    Drawing::Canvas canvas;
    RSRenderParams params(RSNodeDrawableTest::id);
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
 * @tc.name: NeedInitCachedSurfaceExtTest
 * @tc.desc: Test If NeedInitCachedSurface Can Run
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSNodeDrawableTest, NeedInitCachedSurfaceExtTest, TestSize.Level1)
{
    auto drawable = RSNodeDrawableTest::CreateDrawable();

    Drawing::Canvas canvas;
    RSRenderParams params(RSNodeDrawableTest::id);
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    drawable->isDrawAreaEnable_ = DrawAreaEnableState::DRAW_AREA_DISABLE;
    drawable->cachedSurface_ = nullptr;
    auto result = drawable->NeedInitCachedSurface(params.GetCacheSize());
    ASSERT_EQ(result, true);

    drawable->InitCachedSurface(paintFilterCanvas.GetGPUContext().get(), params.GetCacheSize(), 0xFF);
    drawable->isDrawAreaEnable_ = DrawAreaEnableState::DRAW_AREA_ENABLE;
    result = drawable->NeedInitCachedSurface(params.GetCacheSize());
}

/**
 * @tc.name: CheckIfNeedUpdateCacheExtTest
 * @tc.desc: Test If CheckIfNeedUpdateCache Can Run
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSNodeDrawableTest, CheckIfNeedUpdateCacheExtTest, TestSize.Level1)
{
    auto drawable = RSNodeDrawableTest::CreateDrawable();
    Drawing::Canvas canvas;
    RSRenderParams params(RSNodeDrawableTest::id);
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
    drawable->isDrawAreaEnable_ = DrawAreaEnableState::DRAW_AREA_ENABLE;
    Drawing::Rect unionRect(0.f, 0.f, 0.f, 0.f);
    drawable->opListDrawAreas_.opInfo_.unionRect = unionRect;
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
 * @tc.name: DrawCachedImageExtTest
 * @tc.desc: Test If DrawCachedImage Can Run
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSNodeDrawableTest, DrawCachedImageExtTest, TestSize.Level1)
{
    auto drawable = RSNodeDrawableTest::CreateDrawable();

    Drawing::Canvas canvas;
    RSRenderParams params(RSNodeDrawableTest::id);
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    drawable->DrawCachedImage(paintFilterCanvas, params.GetCacheSize());
    drawable->ClearCachedSurface();

    drawable->cachedSurface_ = std::make_shared<Drawing::Surface>();
    drawable->cachedImage_ = std::make_shared<Drawing::Image>();
    auto drawingCanvas = std::make_shared<Drawing::Canvas>();
    paintFilterCanvas.canvas_ = drawingCanvas.get();
    paintFilterCanvas.canvas_->gpuContext_ = std::make_shared<Drawing::GPUContext>();
    drawable->DrawCachedImage(paintFilterCanvas, params.GetCacheSize());
    drawable->ClearCachedSurface();
    ASSERT_FALSE(RSSystemProperties::GetRecordingEnabled());
    ASSERT_FALSE(drawable->IsComputeDrawAreaSucc());
}

/**
 * @tc.name: DrawExtTest
 * @tc.desc: Test If Draw Can Run
 * @tc.type: FUNC
 * @tc.require: issueIAEDYI
 */
HWTEST_F(RSNodeDrawableTest, DrawExtTest, TestSize.Level1)
{
    auto drawable = RSNodeDrawableTest::CreateDrawable();
    Drawing::Canvas canvas;
    drawable->Draw(canvas);
    ASSERT_FALSE(RSUniRenderThread::IsInCaptureProcess());

    CaptureParam param;
    param.isSnapshot_ = true;
    param.isMirror_ = true;
    RSUniRenderThread::SetCaptureParam(param);
    ASSERT_TRUE(RSUniRenderThread::IsInCaptureProcess());
}

/**
 * @tc.name: UpdateCacheSurfaceExtTest
 * @tc.desc: Test If UpdateCacheSurface Can Run
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSNodeDrawableTest, UpdateCacheSurfaceExtTest, TestSize.Level1)
{
    auto drawable = RSNodeDrawableTest::CreateDrawable();

    Drawing::Canvas canvas;
    const RSRenderParams params(RSNodeDrawableTest::id);
    RSPaintFilterCanvas paintFilterCanvas(&canvas);

    drawable->UpdateCacheSurface(canvas, params);
    ASSERT_EQ(drawable->cachedImage_, nullptr);
}

/**
 * @tc.name: TraverseSubTreeAndDrawFilterWithClipExtTest
 * @tc.desc: Test If TraverseSubTreeAndDrawFilterWithClip Can Run
 * @tc.type: FUNC
 * @tc.require: issueIAEDYI
 */
HWTEST_F(RSNodeDrawableTest, TraverseSubTreeAndDrawFilterWithClipExtTest, TestSize.Level1)
{
    auto drawable = RSNodeDrawableTest::CreateDrawable();

    Drawing::Canvas canvas;
    RSRenderParams params(RSNodeDrawableTest::id);
    drawable->TraverseSubTreeAndDrawFilterWithClip(canvas, params);
    ASSERT_TRUE(drawable->filterRects_.empty());

    Drawing::RectI rect;
    drawable->filterRects_.push_back(rect);
    drawable->TraverseSubTreeAndDrawFilterWithClip(canvas, params);
    ASSERT_FALSE(drawable->filterRects_.empty());
}
}
