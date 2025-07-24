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
 * @tc.name: OpincCalculateBefore
 * @tc.desc: Test OpincCalculateBefore
 * @tc.type: FUNC
 * @tc.require: issueI9SPVO
 */
HWTEST_F(RSRenderNodeDrawableTest, OpincCalculateBefore, TestSize.Level1)
{
    auto drawable = RSRenderNodeDrawableTest::CreateDrawable();
    ASSERT_NE(drawable, nullptr);
    Drawing::Canvas canvas;
    RSRenderParams params(RSRenderNodeDrawableTest::id);
    bool isOpincDropNodeExt = true;
    drawable->OpincCalculateBefore(canvas, params, isOpincDropNodeExt);

    drawable->rootNodeStragyType_ = NodeStrategyType::OPINC_AUTOCACHE;
    drawable->recordState_ = NodeRecordState::RECORD_CALCULATE;
    drawable->OpincCalculateBefore(canvas, params, isOpincDropNodeExt);
}

/**
 * @tc.name: OpincCalculateAfter
 * @tc.desc: Test OpincCalculateAfter
 * @tc.type: FUNC
 * @tc.require: issueI9SPVO
 */
HWTEST_F(RSRenderNodeDrawableTest, OpincCalculateAfter, TestSize.Level1)
{
    auto drawable = RSRenderNodeDrawableTest::CreateDrawable();
    ASSERT_NE(drawable, nullptr);
    Drawing::Canvas canvas;
    bool isOpincDropNodeExt = true;
    drawable->OpincCalculateAfter(canvas, isOpincDropNodeExt);

    drawable->isOpincCaculateStart_ = true;
    drawable->OpincCalculateAfter(canvas, isOpincDropNodeExt);
}

/**
 * @tc.name: PreDrawableCacheState
 * @tc.desc: Test result of PreDrawableCacheState
 * @tc.type: FUNC
 * @tc.require: issueI9SPVO
 */
HWTEST_F(RSRenderNodeDrawableTest, PreDrawableCacheState, TestSize.Level1)
{
    auto drawable = RSRenderNodeDrawableTest::CreateDrawable();

    RSRenderParams params(RSRenderNodeDrawableTest::id);
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
 * @tc.name: OpincCanvasUnionTranslate
 * @tc.desc: Test OpincCanvasUnionTranslate
 * @tc.type: FUNC
 * @tc.require: issueI9SPVO
 */
HWTEST_F(RSRenderNodeDrawableTest, OpincCanvasUnionTranslate, TestSize.Level1)
{
    auto drawable = RSRenderNodeDrawableTest::CreateDrawable();

    drawable->isDrawAreaEnable_ = DrawAreaEnableState::DRAW_AREA_INIT;
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    drawable->OpincCanvasUnionTranslate(paintFilterCanvas);

    drawable->isDrawAreaEnable_ = DrawAreaEnableState::DRAW_AREA_ENABLE;
    drawable->OpincCanvasUnionTranslate(paintFilterCanvas);
    ASSERT_TRUE(drawable->IsComputeDrawAreaSucc());
}

/**
 * @tc.name: ResumeOpincCanvasTranslate
 * @tc.desc: Test ResumeOpincCanvasTranslate
 * @tc.type: FUNC
 * @tc.require: issueI9SPVO
 */
HWTEST_F(RSRenderNodeDrawableTest, ResumeOpincCanvasTranslate, TestSize.Level1)
{
    auto drawable = RSRenderNodeDrawableTest::CreateDrawable();

    drawable->isDrawAreaEnable_ = DrawAreaEnableState::DRAW_AREA_INIT;
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    drawable->ResumeOpincCanvasTranslate(paintFilterCanvas);

    drawable->isDrawAreaEnable_ = DrawAreaEnableState::DRAW_AREA_ENABLE;
    drawable->ResumeOpincCanvasTranslate(paintFilterCanvas);
    ASSERT_TRUE(drawable->IsComputeDrawAreaSucc());
}

/**
 * @tc.name: DrawableCacheStateReset
 * @tc.desc: Test drawable cache state after reset
 * @tc.type: FUNC
 * @tc.require: issueI9SPVO
 */
HWTEST_F(RSRenderNodeDrawableTest, DrawableCacheStateReset, TestSize.Level1)
{
    auto drawable = RSRenderNodeDrawableTest::CreateDrawable();
    RSRenderParams params(RSRenderNodeDrawableTest::id);
    drawable->DrawableCacheStateReset(params);

    ASSERT_EQ(drawable->isDrawAreaEnable_, DrawAreaEnableState::DRAW_AREA_INIT);
    ASSERT_EQ(drawable->rootNodeStragyType_, NodeStrategyType::CACHE_NONE);
    ASSERT_EQ(drawable->temNodeStragyType_, NodeStrategyType::CACHE_NONE);
    ASSERT_EQ(drawable->recordState_, NodeRecordState::RECORD_NONE);
    ASSERT_EQ(drawable->isOpincRootNode_, false);
    ASSERT_EQ(drawable->opCanCache_, false);
}

/**
 * @tc.name: IsOpListDrawAreaEnable
 * @tc.desc: Test result of IsOpListDrawAreaEnable
 * @tc.type: FUNC
 * @tc.require: issueI9SPVO
 */
HWTEST_F(RSRenderNodeDrawableTest, IsOpListDrawAreaEnable, TestSize.Level1)
{
    auto drawable = RSRenderNodeDrawableTest::CreateDrawable();
    ASSERT_FALSE(drawable->IsOpListDrawAreaEnable());

    drawable->recordState_ = NodeRecordState::RECORD_CALCULATE;
    ASSERT_FALSE(drawable->IsOpListDrawAreaEnable());

    drawable->rootNodeStragyType_ = NodeStrategyType::OPINC_AUTOCACHE;
    ASSERT_TRUE(drawable->IsOpListDrawAreaEnable());
}

/**
 * @tc.name: IsTranslate
 * @tc.desc: Test whether matrix is translate matrix
 * @tc.type: FUNC
 * @tc.require: issueI9SPVO
 */
HWTEST_F(RSRenderNodeDrawableTest, IsTranslate, TestSize.Level1)
{
    auto drawable = RSRenderNodeDrawableTest::CreateDrawable();

    Drawing::Matrix matrix;
    ASSERT_TRUE(drawable->IsTranslate(matrix));

    matrix.Translate(100, 200);
    ASSERT_TRUE(drawable->IsTranslate(matrix));

    matrix.SetScale(2, 2);
    ASSERT_FALSE(drawable->IsTranslate(matrix));
}

/**
 * @tc.name: NodeCacheStateDisable
 * @tc.desc: Test drawable state after disable cache state
 * @tc.type: FUNC
 * @tc.require: issueI9SPVO
 */
HWTEST_F(RSRenderNodeDrawableTest, NodeCacheStateDisable, TestSize.Level1)
{
    auto drawable = RSRenderNodeDrawableTest::CreateDrawable();

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
 * @tc.name: BeforeDrawCacheProcessChildNode
 * @tc.desc: Test result of BeforeDrawCacheProcessChildNode
 * @tc.type: FUNC
 * @tc.require: issueI9SPVO
 */
HWTEST_F(RSRenderNodeDrawableTest, BeforeDrawCacheProcessChildNode, TestSize.Level1)
{
    auto drawable = RSRenderNodeDrawableTest::CreateDrawable();

    RSRenderParams params(RSRenderNodeDrawableTest::id);
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
 * @tc.name: BeforeDrawCacheFindRootNode
 * @tc.desc: Test BeforeDrawCacheFindRootNode
 * @tc.type: FUNC
 * @tc.require: issueI9SPVO
 */
HWTEST_F(RSRenderNodeDrawableTest, BeforeDrawCacheFindRootNode, TestSize.Level1)
{
    auto drawable = RSRenderNodeDrawableTest::CreateDrawable();
    ASSERT_NE(drawable, nullptr);

    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    RSRenderParams params(RSRenderNodeDrawableTest::id);
    bool isOpincDropNodeExt = true;
    drawable->BeforeDrawCacheFindRootNode(paintFilterCanvas, params, isOpincDropNodeExt);

    params.isOpincRootFlag_ = true;
    params.SetCacheSize({100, 100});
    paintFilterCanvas.SetCacheType(Drawing::CacheType::ENABLED);
    drawable->BeforeDrawCacheFindRootNode(paintFilterCanvas, params, isOpincDropNodeExt);
}

/**
 * @tc.name: BeforeDrawCache
 * @tc.desc: Test BeforeDrawCache
 * @tc.type: FUNC
 * @tc.require: issueI9SPVO
 */
HWTEST_F(RSRenderNodeDrawableTest, BeforeDrawCache, TestSize.Level1)
{
    auto drawable = RSRenderNodeDrawableTest::CreateDrawable();
    ASSERT_NE(drawable, nullptr);

    Drawing::Canvas canvas;
    RSRenderParams params(RSRenderNodeDrawableTest::id);
    bool isOpincDropNodeExt = true;
    drawable->recordState_ = NodeRecordState::RECORD_CACHED;
    NodeStrategyType strategyType = NodeStrategyType::DDGR_OPINC_DYNAMIC;
    drawable->BeforeDrawCache(strategyType, canvas, params, isOpincDropNodeExt);

    strategyType = NodeStrategyType::CACHE_NONE;
    drawable->recordState_ = NodeRecordState::RECORD_NONE;
    drawable->BeforeDrawCache(strategyType, canvas, params, isOpincDropNodeExt);
    ASSERT_EQ(drawable->temNodeStragyType_, NodeStrategyType::CACHE_NONE);
}

/**
 * @tc.name: AfterDrawCache
 * @tc.desc: Test AfterDrawCache
 * @tc.type: FUNC
 * @tc.require: issueI9SPVO
 */
HWTEST_F(RSRenderNodeDrawableTest, AfterDrawCache, TestSize.Level1)
{
    auto drawable = RSRenderNodeDrawableTest::CreateDrawable();
    ASSERT_NE(drawable, nullptr);

    Drawing::Canvas canvas;
    RSRenderParams params(RSRenderNodeDrawableTest::id);
    bool isOpincDropNodeExt = true;
    int opincRootTotalCount = 0;
    drawable->rootNodeStragyType_ = NodeStrategyType::OPINC_AUTOCACHE;
    drawable->recordState_ = NodeRecordState::RECORD_CALCULATE;
    NodeStrategyType cacheStragy = NodeStrategyType::CACHE_NONE;
    drawable->AfterDrawCache(cacheStragy, canvas, params, isOpincDropNodeExt, opincRootTotalCount);
    ASSERT_EQ(drawable->isDrawAreaEnable_, DrawAreaEnableState::DRAW_AREA_INIT);

    drawable->recordState_ = NodeRecordState::RECORD_CACHING;
    drawable->AfterDrawCache(cacheStragy, canvas, params, isOpincDropNodeExt, opincRootTotalCount);
    ASSERT_TRUE(!drawable->isOpincMarkCached_);
}

/**
 * @tc.name: DrawAutoCache
 * @tc.desc: Test result of DrawAutoCache
 * @tc.type: FUNC
 * @tc.require: issueI9SPVO
 */
HWTEST_F(RSRenderNodeDrawableTest, DrawAutoCache, TestSize.Level1)
{
    auto drawable = RSRenderNodeDrawableTest::CreateDrawable();
    ASSERT_NE(drawable, nullptr);

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
 @tc.name: AfterDrawCacheWithScreen
 @tc.desc: Test AfterDrawCacheWithScreen delay cache
 @tc.type: FUNC
 @tc.require: issueIAL4RE
 */
HWTEST_F(RSRenderNodeDrawableTest, AfterDrawCacheWithScreen, TestSize.Level1)
{
    auto drawable = RSRenderNodeDrawableTest::CreateDrawable();
    ASSERT_NE(drawable, nullptr);

    Drawing::Canvas canvas;
    RSRenderParams params(RSRenderNodeDrawableTest::id);
    bool isOpincDropNodeExt = true;
    int opincRootTotalCount = 0;
    drawable->rootNodeStragyType_ = NodeStrategyType::OPINC_AUTOCACHE;
    NodeStrategyType cacheStragy = NodeStrategyType::CACHE_NONE;
    drawable->recordState_ = NodeRecordState::RECORD_CACHING;
    RectI absRect = {10, 10, 10, 10};
    params.SetAbsDrawRect(absRect);
    drawable->AfterDrawCache(cacheStragy, canvas, params, isOpincDropNodeExt, opincRootTotalCount);
    ASSERT_TRUE(!drawable->isOpincMarkCached_);
}

/**
 * @tc.name: DrawAutoCacheDfx
 * @tc.desc: Test DrawAutoCacheDfx
 * @tc.type: FUNC
 * @tc.require: issueI9SPVO
 */
HWTEST_F(RSRenderNodeDrawableTest, DrawAutoCacheDfx, TestSize.Level1)
{
    auto drawable = RSRenderNodeDrawableTest::CreateDrawable();
    ASSERT_NE(drawable, nullptr);

    std::vector<std::pair<RectI, std::string>> autoCacheRenderNodeInfos = {
        { RectI(100, 100, 200, 200), "" },
        { RectI(100, 100, 200, 200), "" }
    };
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    drawable->DrawAutoCacheDfx(paintFilterCanvas, autoCacheRenderNodeInfos);
    ASSERT_TRUE(drawable->GetOpListUnionArea().IsEmpty());
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
    ASSERT_FALSE(params.GetDrawingCacheType() == RSDrawingCacheType::TARGETED_CACHE);

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
    ASSERT_EQ(drawable->isDrawingCacheDfxEnabled_, true);
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
    drawable->isDrawAreaEnable_ = DrawAreaEnableState::DRAW_AREA_DISABLE;
    drawable->cachedSurface_ = nullptr;
    auto result = drawable->NeedInitCachedSurface(params.GetCacheSize());
    ASSERT_EQ(result, true);

    drawable->InitCachedSurface(paintFilterCanvas.GetGPUContext().get(), params.GetCacheSize(), 0xFF);
    drawable->isDrawAreaEnable_ = DrawAreaEnableState::DRAW_AREA_ENABLE;
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
    drawable->isDrawAreaEnable_ = DrawAreaEnableState::DRAW_AREA_ENABLE;
    drawable->DrawCachedImage(paintFilterCanvas2, params.GetCacheSize());
    drawable->ClearCachedSurface();
    drawable->isDrawAreaEnable_ = DrawAreaEnableState::DRAW_AREA_INIT;

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
    drawable->ProcessedNodeCountInc();
    CaptureParam param;
    param.isSingleSurface_ = true;
    RSUniRenderThread::SetCaptureParam(param);
    drawable->SnapshotProcessedNodeCountInc();
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
}
