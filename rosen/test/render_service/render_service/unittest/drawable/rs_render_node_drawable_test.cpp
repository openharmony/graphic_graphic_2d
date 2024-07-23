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

    Drawing::Canvas canvas;
    RSRenderParams params(RSRenderNodeDrawableTest::id);
    bool isOpincDropNodeExt = true;
    drawable->recordState_ = NodeRecordState::RECORD_CACHED;
    NodeStrategyType strategyType = NodeStrategyType::DDGR_OPINC_DYNAMIC;
    drawable->BeforeDrawCache(strategyType, canvas, params, isOpincDropNodeExt);

    strategyType = NodeStrategyType::CACHE_NONE;
    drawable->recordState_ = NodeRecordState::RECORD_NONE;
    drawable->BeforeDrawCache(strategyType, canvas, params, isOpincDropNodeExt);
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

    Drawing::Canvas canvas;
    RSRenderParams params(RSRenderNodeDrawableTest::id);
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
 * @tc.name: DrawAutoCache
 * @tc.desc: Test result of DrawAutoCache
 * @tc.type: FUNC
 * @tc.require: issueI9SPVO
 */
HWTEST_F(RSRenderNodeDrawableTest, DrawAutoCache, TestSize.Level1)
{
    auto drawable = RSRenderNodeDrawableTest::CreateDrawable();

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
 * @tc.name: DrawAutoCacheDfx
 * @tc.desc: Test DrawAutoCacheDfx
 * @tc.type: FUNC
 * @tc.require: issueI9SPVO
 */
HWTEST_F(RSRenderNodeDrawableTest, DrawAutoCacheDfx, TestSize.Level1)
{
    auto drawable = RSRenderNodeDrawableTest::CreateDrawable();

    std::vector<std::pair<RectI, std::string>> autoCacheRenderNodeInfos = {
        { RectI(100, 100, 200, 200), "" },
        { RectI(100, 100, 200, 200), "" }
    };
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    drawable->DrawAutoCacheDfx(paintFilterCanvas, autoCacheRenderNodeInfos);
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
    params.freezeFlag_ = true;
    drawable->GenerateCacheIfNeed(canvas, params);
    ASSERT_EQ(params.GetRSFreezeFlag(), true);
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
    params.childHasVisibleFilter_ = true;
    params.drawingCacheType_ = RSDrawingCacheType::FORCED_CACHE;
    params.foregroundFilterCache_ = nullptr;
    drawable->CheckCacheTypeAndDraw(canvas, params);
    ASSERT_EQ(drawable->drawBlurForCache_, false);
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
    drawable->isDrawingCacheEnabled_ = true;
    drawable->isDrawingCacheDfxEnabled_ = true;
    drawable->DrawDfxForCacheInfo(paintFilterCanvas);
    ASSERT_EQ(drawable->isDrawingCacheDfxEnabled_, true);

    drawable->isDrawingCacheDfxEnabled_ = false;
    drawable->DrawDfxForCacheInfo(paintFilterCanvas);
    ASSERT_EQ(drawable->isDrawingCacheDfxEnabled_, false);
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
    auto result =drawable->GetCachedImage(paintFilterCanvas);
    ASSERT_EQ(result, nullptr);

    drawable->InitCachedSurface(paintFilterCanvas.GetGPUContext().get(), params.GetCacheSize(), 0xFF);
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
    params.freezeFlag_ = true;
    int32_t updateTimes = 0;
    drawable->CheckIfNeedUpdateCache(params, updateTimes);

    params.freezeFlag_ = false;
    params.isDrawingCacheChanged_ = true;
    updateTimes = 0;
    auto result = drawable->CheckIfNeedUpdateCache(params, updateTimes);
    ASSERT_EQ(result, true);
    
    params.isDrawingCacheChanged_ = false;
    updateTimes = 0;
    result = drawable->CheckIfNeedUpdateCache(params, updateTimes);
}

/**
 * @tc.name: UpdateCacheSurface
 * @tc.desc: Test If UpdateCacheSurface Can Run
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSRenderNodeDrawableTest, UpdateCacheSurfaceTest, TestSize.Level1)
{
    auto drawable = RSRenderNodeDrawableTest::CreateDrawable();

    Drawing::Canvas canvas;
    const RSRenderParams params(RSRenderNodeDrawableTest::id);
    RSPaintFilterCanvas paintFilterCanvas(&canvas);

    drawable->UpdateCacheSurface(canvas, params);
    ASSERT_EQ(drawable->cachedImage_, nullptr);
}
}
