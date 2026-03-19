/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "gtest/gtest.h"

#include "feature/opinc/rs_opinc_draw_cache.h"
#include "feature/opinc/rs_opinc_manager.h"
#include "params/rs_render_params.h"
#include "pipeline/rs_dirty_region_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
constexpr NodeId DEFAULT_ID = 0xFFFF;
constexpr int64_t CACHE_MEM = 100;
constexpr int32_t LAYER_PART_RENDER_DIRTY_OFFSET = 10;
constexpr int32_t LAYER_PART_RENDER_DIRTY_SIZE = 100;
constexpr int32_t LAYER_PART_RENDER_NODE_COUNT = 5;

class RSOpincDrawCacheTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline NodeId id = DEFAULT_ID;
};

void RSOpincDrawCacheTest::SetUpTestCase() {}
void RSOpincDrawCacheTest::TearDownTestCase() {}
void RSOpincDrawCacheTest::SetUp() {}
void RSOpincDrawCacheTest::TearDown() {}


/**
 * @tc.name: GetOpincCacheMaxWidth
 * @tc.desc: Test GetOpincCacheMaxWidth
 * @tc.type: FUNC
 * @tc.require: issueIC87OJ
 */
HWTEST_F(RSOpincDrawCacheTest, GetOpincCacheMaxWidth, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    ASSERT_EQ(opincDrawCache.GetOpincCacheMaxWidth(), 0);
}

/**
 * @tc.name: GetOpincCacheMaxHeight
 * @tc.desc: Test GetOpincCacheMaxHeight
 * @tc.type: FUNC
 * @tc.require: issueIC87OJ
 */
HWTEST_F(RSOpincDrawCacheTest, GetOpincCacheMaxHeight, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    ASSERT_EQ(opincDrawCache.GetOpincCacheMaxHeight(), 0);
}

/**
 * @tc.name: OpincCalculateBefore
 * @tc.desc: Test OpincCalculateBefore
 * @tc.type: FUNC
 * @tc.require: issueI9SPVO
 */
HWTEST_F(RSOpincDrawCacheTest, OpincCalculateBefore, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    Drawing::Canvas canvas;
    RSRenderParams params(id);
    DrawableV2::RSOpincDrawCache::SetOpincBlockNodeSkip(true);
    RSOpincManager::Instance().SetOPIncSwitch(true);
    opincDrawCache.OpincCalculateBefore(canvas, params);
    ASSERT_FALSE(opincDrawCache.isOpincCaculateStart_);

    RSOpincManager::Instance().SetOPIncSwitch(false);
    opincDrawCache.OpincCalculateBefore(canvas, params);
    ASSERT_FALSE(opincDrawCache.isOpincCaculateStart_);

    opincDrawCache.rootNodeStragyType_ = NodeStrategyType::OPINC_AUTOCACHE;
    opincDrawCache.recordState_ = NodeRecordState::RECORD_CALCULATE;

    RSOpincManager::Instance().SetOPIncSwitch(true);
    opincDrawCache.OpincCalculateBefore(canvas, params);
    ASSERT_TRUE(opincDrawCache.isOpincCaculateStart_);

    RSOpincManager::Instance().SetOPIncSwitch(false);
    opincDrawCache.OpincCalculateBefore(canvas, params);
    ASSERT_FALSE(opincDrawCache.isOpincCaculateStart_);
}
 
/**
 * @tc.name: OpincCalculateAfter
 * @tc.desc: Test OpincCalculateAfter
 * @tc.type: FUNC
 * @tc.require: issueI9SPVO
 */
HWTEST_F(RSOpincDrawCacheTest, OpincCalculateAfter, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    Drawing::Canvas canvas;
    DrawableV2::RSOpincDrawCache::SetOpincBlockNodeSkip(true);
    opincDrawCache.OpincCalculateAfter(canvas);
    ASSERT_FALSE(opincDrawCache.isOpincCaculateStart_);

    opincDrawCache.isOpincCaculateStart_ = true;
    opincDrawCache.OpincCalculateAfter(canvas);
    ASSERT_FALSE(opincDrawCache.isOpincCaculateStart_);
}
 
/**
 * @tc.name: PreDrawableCacheState
 * @tc.desc: Test result of PreDrawableCacheState
 * @tc.type: FUNC
 * @tc.require: issueI9SPVO
 */
HWTEST_F(RSOpincDrawCacheTest, PreDrawableCacheState, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    RSRenderParams params(id);
    params.OpincSetCacheChangeFlag(true, true);
    opincDrawCache.isOpincRootNode_ = true;
    DrawableV2::RSOpincDrawCache::SetOpincBlockNodeSkip(true);
    ASSERT_TRUE(opincDrawCache.PreDrawableCacheState(params));

    params.OpincSetCacheChangeFlag(false, true);
    opincDrawCache.isOpincRootNode_ = false;
    DrawableV2::RSOpincDrawCache::SetOpincBlockNodeSkip(false);
    ASSERT_FALSE(opincDrawCache.PreDrawableCacheState(params));
}
 
/**
 * @tc.name: OpincCanvasUnionTranslate
 * @tc.desc: Test OpincCanvasUnionTranslate
 * @tc.type: FUNC
 * @tc.require: issueI9SPVO
 */
HWTEST_F(RSOpincDrawCacheTest, OpincCanvasUnionTranslate, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    opincDrawCache.isDrawAreaEnable_ = DrawAreaEnableState::DRAW_AREA_INIT;
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    opincDrawCache.OpincCanvasUnionTranslate(paintFilterCanvas);

    opincDrawCache.isDrawAreaEnable_ = DrawAreaEnableState::DRAW_AREA_ENABLE;
    opincDrawCache.OpincCanvasUnionTranslate(paintFilterCanvas);
    ASSERT_TRUE(opincDrawCache.IsComputeDrawAreaSucc());
}
 
/**
 * @tc.name: ResumeOpincCanvasTranslate
 * @tc.desc: Test ResumeOpincCanvasTranslate
 * @tc.type: FUNC
 * @tc.require: issueI9SPVO
 */
HWTEST_F(RSOpincDrawCacheTest, ResumeOpincCanvasTranslate, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    opincDrawCache.isDrawAreaEnable_ = DrawAreaEnableState::DRAW_AREA_INIT;
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    opincDrawCache.ResumeOpincCanvasTranslate(paintFilterCanvas);

    opincDrawCache.isDrawAreaEnable_ = DrawAreaEnableState::DRAW_AREA_ENABLE;
    opincDrawCache.ResumeOpincCanvasTranslate(paintFilterCanvas);
    ASSERT_TRUE(opincDrawCache.IsComputeDrawAreaSucc());
}
 
/**
 * @tc.name: DrawableCacheStateReset
 * @tc.desc: Test drawable cache state after reset
 * @tc.type: FUNC
 * @tc.require: issueI9SPVO
 */
HWTEST_F(RSOpincDrawCacheTest, DrawableCacheStateReset, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    RSRenderParams params(id);
    opincDrawCache.DrawableCacheStateReset(params);

    ASSERT_EQ(opincDrawCache.isDrawAreaEnable_, DrawAreaEnableState::DRAW_AREA_INIT);
    ASSERT_EQ(opincDrawCache.rootNodeStragyType_, NodeStrategyType::CACHE_NONE);
    ASSERT_EQ(opincDrawCache.temNodeStragyType_, NodeStrategyType::CACHE_NONE);
    ASSERT_EQ(opincDrawCache.recordState_, NodeRecordState::RECORD_NONE);
    ASSERT_EQ(opincDrawCache.isOpincRootNode_, false);
    ASSERT_EQ(opincDrawCache.opCanCache_, false);
}
 
/**
 * @tc.name: IsOpListDrawAreaEnable
 * @tc.desc: Test result of IsOpListDrawAreaEnable
 * @tc.type: FUNC
 * @tc.require: issueI9SPVO
 */
HWTEST_F(RSOpincDrawCacheTest, IsOpListDrawAreaEnable, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    ASSERT_FALSE(opincDrawCache.IsOpListDrawAreaEnable());

    opincDrawCache.recordState_ = NodeRecordState::RECORD_CALCULATE;
    ASSERT_FALSE(opincDrawCache.IsOpListDrawAreaEnable());

    opincDrawCache.rootNodeStragyType_ = NodeStrategyType::OPINC_AUTOCACHE;
    ASSERT_TRUE(opincDrawCache.IsOpListDrawAreaEnable());
}
 
/**
 * @tc.name: IsTranslate
 * @tc.desc: Test whether matrix is translate matrix
 * @tc.type: FUNC
 * @tc.require: issueI9SPVO
 */
HWTEST_F(RSOpincDrawCacheTest, IsTranslate, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    Drawing::Matrix matrix;
    ASSERT_TRUE(opincDrawCache.IsTranslate(matrix));

    matrix.Translate(100, 200);
    ASSERT_TRUE(opincDrawCache.IsTranslate(matrix));

    matrix.SetScale(2, 2);
    ASSERT_FALSE(opincDrawCache.IsTranslate(matrix));
}
 
/**
 * @tc.name: NodeCacheStateDisable
 * @tc.desc: Test drawable state after disable cache state
 * @tc.type: FUNC
 * @tc.require: issueI9SPVO
 */
HWTEST_F(RSOpincDrawCacheTest, NodeCacheStateDisable, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;

    opincDrawCache.opCanCache_ = false;
    opincDrawCache.NodeCacheStateDisable();
    ASSERT_EQ(opincDrawCache.recordState_, NodeRecordState::RECORD_DISABLE);
    ASSERT_EQ(opincDrawCache.rootNodeStragyType_, NodeStrategyType::CACHE_DISABLE);
    ASSERT_EQ(opincDrawCache.temNodeStragyType_, NodeStrategyType::CACHE_DISABLE);
    ASSERT_EQ(opincDrawCache.isDrawAreaEnable_, DrawAreaEnableState::DRAW_AREA_DISABLE);
    ASSERT_EQ(opincDrawCache.opCanCache_, false);

    opincDrawCache.opCanCache_ = true;
    opincDrawCache.NodeCacheStateDisable();
    ASSERT_EQ(opincDrawCache.opCanCache_, false);
}
 
/**
 * @tc.name: BeforeDrawCacheProcessChildNode
 * @tc.desc: Test result of BeforeDrawCacheProcessChildNode
 * @tc.type: FUNC
 * @tc.require: issueI9SPVO
 */
HWTEST_F(RSOpincDrawCacheTest, BeforeDrawCacheProcessChildNode, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    RSRenderParams params(id);
    DrawableV2::RSOpincDrawCache::SetNodeCacheType(NodeStrategyType::CACHE_NONE);
    params.isOpincRootFlag_ = true;
    ASSERT_TRUE(opincDrawCache.BeforeDrawCacheProcessChildNode(params));

    DrawableV2::RSOpincDrawCache::SetNodeCacheType(NodeStrategyType::DDGR_OPINC_DYNAMIC);
    ASSERT_TRUE(opincDrawCache.BeforeDrawCacheProcessChildNode(params));

    opincDrawCache.recordState_ = NodeRecordState::RECORD_CACHED;
    opincDrawCache.rootNodeStragyType_ = NodeStrategyType::OPINC_AUTOCACHE;
    ASSERT_TRUE(opincDrawCache.BeforeDrawCacheProcessChildNode(params));

    opincDrawCache.rootNodeStragyType_ = NodeStrategyType::CACHE_DISABLE;
    ASSERT_TRUE(opincDrawCache.BeforeDrawCacheProcessChildNode(params));
}
 
/**
 * @tc.name: BeforeDrawCacheFindRootNode
 * @tc.desc: Test BeforeDrawCacheFindRootNode
 * @tc.type: FUNC
 * @tc.require: issueI9SPVO
 */
HWTEST_F(RSOpincDrawCacheTest, BeforeDrawCacheFindRootNode, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    RSRenderParams params(id);
    DrawableV2::RSOpincDrawCache::SetOpincBlockNodeSkip(true);
    params.SetCacheSize({100, 100});
    paintFilterCanvas.SetCacheType(Drawing::CacheType::ENABLED);

    RSOpincManager::Instance().SetOPIncSwitch(true);
    params.isOpincRootFlag_ = true;
    opincDrawCache.BeforeDrawCacheFindRootNode(paintFilterCanvas, params);
    ASSERT_TRUE(opincDrawCache.recordState_ != NodeRecordState::RECORD_CALCULATE);

    params.isOpincRootFlag_ = false;
    opincDrawCache.BeforeDrawCacheFindRootNode(paintFilterCanvas, params);
    ASSERT_TRUE(opincDrawCache.recordState_ != NodeRecordState::RECORD_CALCULATE);

    RSOpincManager::Instance().SetOPIncSwitch(false);
    params.isOpincRootFlag_ = true;
    opincDrawCache.BeforeDrawCacheFindRootNode(paintFilterCanvas, params);
    ASSERT_TRUE(opincDrawCache.recordState_ != NodeRecordState::RECORD_CALCULATE);

    params.isOpincRootFlag_ = false;
    opincDrawCache.BeforeDrawCacheFindRootNode(paintFilterCanvas, params);
    ASSERT_TRUE(opincDrawCache.recordState_ != NodeRecordState::RECORD_CALCULATE);
}
 
/**
 * @tc.name: BeforeDrawCache
 * @tc.desc: Test BeforeDrawCache
 * @tc.type: FUNC
 * @tc.require: issueI9SPVO
 */
HWTEST_F(RSOpincDrawCacheTest, BeforeDrawCache, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    Drawing::Canvas canvas;
    RSRenderParams params(id);
    DrawableV2::RSOpincDrawCache::SetOpincBlockNodeSkip(true);
    RSOpincManager::Instance().SetOPIncSwitch(true);
    opincDrawCache.recordState_ = NodeRecordState::RECORD_CACHED;
    opincDrawCache.nodeCacheType_ = NodeStrategyType::DDGR_OPINC_DYNAMIC;
    opincDrawCache.BeforeDrawCache(canvas, params);
    ASSERT_FALSE(opincDrawCache.isOpincCaculateStart_);

    opincDrawCache.rootNodeStragyType_ = NodeStrategyType::OPINC_AUTOCACHE;
    opincDrawCache.nodeCacheType_ =  NodeStrategyType::OPINC_AUTOCACHE;
    opincDrawCache.recordState_ = NodeRecordState::RECORD_CALCULATE;
    opincDrawCache.BeforeDrawCache(canvas, params);
    ASSERT_TRUE(opincDrawCache.isOpincCaculateStart_);

    RSOpincManager::Instance().SetOPIncSwitch(false);
    opincDrawCache.isOpincCaculateStart_ = false;
    opincDrawCache.recordState_ = NodeRecordState::RECORD_CACHED;
    opincDrawCache.nodeCacheType_ = NodeStrategyType::DDGR_OPINC_DYNAMIC;
    opincDrawCache.BeforeDrawCache(canvas, params);
    ASSERT_FALSE(opincDrawCache.isOpincCaculateStart_);

    opincDrawCache.nodeCacheType_ = NodeStrategyType::CACHE_NONE;
    opincDrawCache.recordState_ = NodeRecordState::RECORD_NONE;
    opincDrawCache.BeforeDrawCache(canvas, params);
    ASSERT_FALSE(opincDrawCache.isOpincCaculateStart_);

    RSOpincManager::Instance().SetOPIncSwitch(true);
    opincDrawCache.nodeCacheType_ = NodeStrategyType::CACHE_NONE;
    params.isOpincRootFlag_ = true;
    opincDrawCache.recordState_ = NodeRecordState::RECORD_NONE;
    opincDrawCache.BeforeDrawCache(canvas, params);
    ASSERT_FALSE(opincDrawCache.isOpincCaculateStart_);
}

/**
 * @tc.name: IsOpincNodeInScreenRect
 * @tc.desc: Test IsOpincNodeInScreenRect
 * @tc.type: FUNC
 * @tc.require: issueIC87OJ
 */
HWTEST_F(RSOpincDrawCacheTest, IsOpincNodeInScreenRect, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    Drawing::Canvas canvas;
    RSRenderParams params(id);
    ASSERT_FALSE(opincDrawCache.IsOpincNodeInScreenRect(params));

    RectI absRect = {10, 10, 10, 10};
    params.SetAbsDrawRect(absRect);
    ASSERT_FALSE(opincDrawCache.IsOpincNodeInScreenRect(params));
}
 
/**
 * @tc.name: AfterDrawCache
 * @tc.desc: Test AfterDrawCache
 * @tc.type: FUNC
 * @tc.require: issueI9SPVO
 */
HWTEST_F(RSOpincDrawCacheTest, AfterDrawCache, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    Drawing::Canvas canvas;
    RSRenderParams params(id);
    DrawableV2::RSOpincDrawCache::SetOpincBlockNodeSkip(true);
    DrawableV2::RSOpincDrawCache::opincRootNodeCount_ = 3;

    RSOpincManager::Instance().SetOPIncSwitch(false);
    opincDrawCache.recordState_ = NodeRecordState::RECORD_CACHING;
    opincDrawCache.AfterDrawCache(canvas, params);
    ASSERT_TRUE(!opincDrawCache.isOpincMarkCached_);

    RSOpincManager::Instance().SetOPIncSwitch(true);
    opincDrawCache.rootNodeStragyType_ = NodeStrategyType::OPINC_AUTOCACHE;
    opincDrawCache.recordState_ = NodeRecordState::RECORD_CALCULATE;
    opincDrawCache.isDrawAreaEnable_ = DrawAreaEnableState::DRAW_AREA_ENABLE;
    opincDrawCache.AfterDrawCache(canvas, params);
    ASSERT_TRUE(opincDrawCache.recordState_ == NodeRecordState::RECORD_CACHING);

    opincDrawCache.rootNodeStragyType_ = NodeStrategyType::OPINC_AUTOCACHE;
    opincDrawCache.recordState_ = NodeRecordState::RECORD_CALCULATE;
    opincDrawCache.isDrawAreaEnable_ = DrawAreaEnableState::DRAW_AREA_DISABLE;
    opincDrawCache.AfterDrawCache(canvas, params);
    ASSERT_TRUE(opincDrawCache.recordState_ == NodeRecordState::RECORD_DISABLE);

    opincDrawCache.rootNodeStragyType_ = NodeStrategyType::OPINC_AUTOCACHE;
    opincDrawCache.recordState_ = NodeRecordState::RECORD_CALCULATE;
    opincDrawCache.isDrawAreaEnable_ = DrawAreaEnableState::DRAW_AREA_INIT;
    opincDrawCache.AfterDrawCache(canvas, params);
    ASSERT_TRUE(opincDrawCache.recordState_ == NodeRecordState::RECORD_CALCULATE);

    opincDrawCache.recordState_ = NodeRecordState::RECORD_CACHING;
    opincDrawCache.AfterDrawCache(canvas, params);
    ASSERT_TRUE(opincDrawCache.recordState_ == NodeRecordState::RECORD_CACHING);

    auto drawingCanvas = std::make_shared<Drawing::Canvas>();
    auto paintFilterCanvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    float alpha = 0.5f;
    paintFilterCanvas->SetAlpha(alpha);
    auto canvasAlpha = std::static_pointer_cast<Drawing::Canvas>(paintFilterCanvas);

    opincDrawCache.rootNodeStragyType_ = NodeStrategyType::OPINC_AUTOCACHE;
    opincDrawCache.recordState_ = NodeRecordState::RECORD_CALCULATE;
    opincDrawCache.isDrawAreaEnable_ = DrawAreaEnableState::DRAW_AREA_ENABLE;
    opincDrawCache.AfterDrawCache(*canvasAlpha.get(), params);
    ASSERT_TRUE(opincDrawCache.recordState_ == NodeRecordState::RECORD_CALCULATE);

    opincDrawCache.rootNodeStragyType_ = NodeStrategyType::OPINC_AUTOCACHE;
    opincDrawCache.recordState_ = NodeRecordState::RECORD_CALCULATE;
    opincDrawCache.isDrawAreaEnable_ = DrawAreaEnableState::DRAW_AREA_DISABLE;
    opincDrawCache.AfterDrawCache(*canvasAlpha.get(), params);
    ASSERT_TRUE(opincDrawCache.recordState_ == NodeRecordState::RECORD_DISABLE);

    opincDrawCache.rootNodeStragyType_ = NodeStrategyType::OPINC_AUTOCACHE;
    opincDrawCache.recordState_ = NodeRecordState::RECORD_CACHING;
    int64_t cacheMem = static_cast<int64_t>(opincDrawCache.screenRectInfo_.GetWidth()) *
        static_cast<int64_t>(opincDrawCache.screenRectInfo_.GetHeight()) * SCREEN_RATIO + 1;
    RSOpincManager::Instance().AddOpincCacheMem(cacheMem);
    EXPECT_EQ(opincDrawCache.IsOpincCacheMemExceedThreshold(), true);
    opincDrawCache.AfterDrawCache(*canvasAlpha.get(), params);
    ASSERT_TRUE(opincDrawCache.recordState_ == NodeRecordState::RECORD_CACHING);

    RSOpincManager::Instance().ReduceOpincCacheMem(cacheMem);
    EXPECT_EQ(opincDrawCache.IsOpincCacheMemExceedThreshold(), false);
    opincDrawCache.AfterDrawCache(*canvasAlpha.get(), params);
    ASSERT_TRUE(opincDrawCache.recordState_ == NodeRecordState::RECORD_CACHING);

    DrawableV2::RSOpincDrawCache::ClearOpincRootNodeCount();
    opincDrawCache.isOpincMarkCached_ = true;
    EXPECT_EQ(opincDrawCache.OpincGetCachedMark(), true);
    opincDrawCache.AfterDrawCache(*canvasAlpha.get(), params);
    ASSERT_TRUE(opincDrawCache.recordState_ == NodeRecordState::RECORD_CACHING);

    opincDrawCache.isOpincMarkCached_ = false;
    opincDrawCache.AfterDrawCache(*canvasAlpha.get(), params);
    ASSERT_TRUE(opincDrawCache.recordState_ == NodeRecordState::RECORD_CACHING);

    RectI absRect = {10, 10, 100, 100};
    params.SetAbsDrawRect(absRect);
    EXPECT_EQ(opincDrawCache.IsOpincNodeInScreenRect(params), false);
    opincDrawCache.AfterDrawCache(*canvasAlpha.get(), params);
    ASSERT_TRUE(opincDrawCache.recordState_ == NodeRecordState::RECORD_CACHING);

    opincDrawCache.isOpincCaculateStart_ = false;
    DrawableV2::RSOpincDrawCache::SetScreenRectInfo({0, 0, 100, 200});
    absRect = {50, 50, 100, 100};
    params.SetAbsDrawRect(absRect);
    EXPECT_EQ(opincDrawCache.IsOpincNodeInScreenRect(params), true);
    opincDrawCache.AfterDrawCache(*canvasAlpha.get(), params);
    ASSERT_TRUE(opincDrawCache.recordState_ == NodeRecordState::RECORD_CACHED);
    DrawableV2::RSOpincDrawCache::SetScreenRectInfo({0, 0, 0, 0});
}

/**
 * @tc.name: AfterDrawCacheWithScreen
 * @tc.desc: Test AfterDrawCacheWithScreen delay cache
 * @tc.type: FUNC
 * @tc.require: issueIAL4RE
 */
HWTEST_F(RSOpincDrawCacheTest, AfterDrawCacheWithScreen, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    Drawing::Canvas canvas;
    RSRenderParams params(id);
    DrawableV2::RSOpincDrawCache::SetOpincBlockNodeSkip(true);
    DrawableV2::RSOpincDrawCache::ClearOpincRootNodeCount();
    opincDrawCache.rootNodeStragyType_ = NodeStrategyType::OPINC_AUTOCACHE;
    opincDrawCache.nodeCacheType_ = NodeStrategyType::CACHE_NONE;
    opincDrawCache.recordState_ = NodeRecordState::RECORD_CACHING;
    RectI absRect = {10, 10, 10, 10};
    params.SetAbsDrawRect(absRect);
    opincDrawCache.AfterDrawCache(canvas, params);
    ASSERT_TRUE(!opincDrawCache.isOpincMarkCached_);
}
 
/**
 * @tc.name: DrawAutoCache
 * @tc.desc: Test result of DrawAutoCache
 * @tc.type: FUNC
 * @tc.require: issueI9SPVO
 */
HWTEST_F(RSOpincDrawCacheTest, DrawAutoCache, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    Drawing::Image image;
    Drawing::SamplingOptions samplingOptions;
    opincDrawCache.opCanCache_ = true;
    opincDrawCache.isDrawAreaEnable_ = DrawAreaEnableState::DRAW_AREA_DISABLE;
    ASSERT_FALSE(opincDrawCache.DrawAutoCache(paintFilterCanvas, image, samplingOptions,
        Drawing::SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT));

    opincDrawCache.opCanCache_ = false;
    opincDrawCache.isDrawAreaEnable_ = DrawAreaEnableState::DRAW_AREA_ENABLE;
    ASSERT_FALSE(opincDrawCache.DrawAutoCache(paintFilterCanvas, image, samplingOptions,
        Drawing::SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT));

    opincDrawCache.opCanCache_ = false;
    opincDrawCache.isDrawAreaEnable_ = DrawAreaEnableState::DRAW_AREA_DISABLE;
    ASSERT_FALSE(opincDrawCache.DrawAutoCache(paintFilterCanvas, image, samplingOptions,
        Drawing::SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT));

    Drawing::OpListHandle::OpInfo opInfo = {
        .unionRect = Drawing::Rect { 100, 100, 300, 300 },
        .drawAreaRects = { Drawing::Rect { 100, 100, 300, 300 } }
    };
    opincDrawCache.opCanCache_ = true;
    opincDrawCache.opListDrawAreas_ = Drawing::OpListHandle(opInfo);
    opincDrawCache.isDrawAreaEnable_ = DrawAreaEnableState::DRAW_AREA_ENABLE;
    ASSERT_TRUE(opincDrawCache.DrawAutoCache(paintFilterCanvas, image, samplingOptions,
        Drawing::SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT));
}

/**
 * @tc.name: DrawAutoCacheDfx
 * @tc.desc: Test DrawAutoCacheDfx
 * @tc.type: FUNC
 * @tc.require: issueI9SPVO
 */
HWTEST_F(RSOpincDrawCacheTest, DrawAutoCacheDfx, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    std::vector<std::pair<RectI, std::string>> autoCacheRenderNodeInfos = {
        { RectI(100, 100, 200, 200), "" },
        { RectI(100, 100, 200, 200), "" }
    };
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    opincDrawCache.DrawAutoCacheDfx(paintFilterCanvas, autoCacheRenderNodeInfos);
    ASSERT_TRUE(opincDrawCache.GetOpListUnionArea().IsEmpty());
}

/**
 * @tc.name: DrawOpincDisabledDfx
 * @tc.desc: Test DrawOpincDisabledDfx
 * @tc.type: FUNC
 * @tc.require: issueIC87OJ
 */
HWTEST_F(RSOpincDrawCacheTest, DrawOpincDisabledDfx, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    Drawing::Canvas canvas;
    RSRenderParams params(id);
    opincDrawCache.DrawOpincDisabledDfx(canvas, params);
    ASSERT_TRUE(opincDrawCache.GetOpListUnionArea().IsEmpty());
}

/**
 * @tc.name: AddOpincCacheMem
 * @tc.desc: Test AddOpincCacheMem
 * @tc.type: FUNC
 * @tc.require: issueIC87OJ
 */
HWTEST_F(RSOpincDrawCacheTest, AddOpincCacheMem, TestSize.Level1)
{
    int64_t cacheMem = RSOpincManager::Instance().GetOpincCacheMem();
    int32_t cacheCount = RSOpincManager::Instance().GetOpincCacheCount();
    DrawableV2::RSOpincDrawCache opincDrawCache;

    opincDrawCache.isAdd_ = false;
    opincDrawCache.opCanCache_ = false;
    opincDrawCache.AddOpincCacheMem(CACHE_MEM);
    EXPECT_EQ(RSOpincManager::Instance().GetOpincCacheMem(), cacheMem);
    EXPECT_EQ(RSOpincManager::Instance().GetOpincCacheCount(), cacheCount);

    opincDrawCache.opCanCache_ = true;
    opincDrawCache.AddOpincCacheMem(CACHE_MEM);
    EXPECT_EQ(RSOpincManager::Instance().GetOpincCacheMem(), cacheMem + CACHE_MEM);
    EXPECT_EQ(RSOpincManager::Instance().GetOpincCacheCount(), cacheCount + 1);
    EXPECT_EQ(opincDrawCache.isAdd_, true);
}

/**
 * @tc.name: ReduceOpincCacheMem
 * @tc.desc: Test ReduceOpincCacheMem
 * @tc.type: FUNC
 * @tc.require: issueIC87OJ
 */
HWTEST_F(RSOpincDrawCacheTest, ReduceOpincCacheMem, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;

    opincDrawCache.isAdd_ = false;
    opincDrawCache.ReduceOpincCacheMem(CACHE_MEM);
    EXPECT_EQ(opincDrawCache.isAdd_, false);

    opincDrawCache.isAdd_ = true;
    opincDrawCache.ReduceOpincCacheMem(CACHE_MEM);
    EXPECT_EQ(opincDrawCache.isAdd_, false);
}

/**
 * @tc.name: IsOpincCacheMemExceedThreshold
 * @tc.desc: Test IsOpincCacheMemExceedThreshold
 * @tc.type: FUNC
 * @tc.require: issueIC87OJ
 */
HWTEST_F(RSOpincDrawCacheTest, IsOpincCacheMemExceedThreshold, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    EXPECT_EQ(opincDrawCache.IsOpincCacheMemExceedThreshold(), false);
    int64_t cacheMem = static_cast<int64_t>(opincDrawCache.screenRectInfo_.GetWidth()) *
        static_cast<int64_t>(opincDrawCache.screenRectInfo_.GetHeight()) * SCREEN_RATIO + 1;
    RSOpincManager::Instance().AddOpincCacheMem(cacheMem);
    EXPECT_EQ(opincDrawCache.IsOpincCacheMemExceedThreshold(), true);
    RSOpincManager::Instance().ReduceOpincCacheMem(cacheMem);
}

/**
 * @tc.name: IsAutoCacheEnable
 * @tc.desc: Test IsAutoCacheEnable
 * @tc.type: FUNC
 * @tc.require: issueIC87OJ
 */
HWTEST_F(RSOpincDrawCacheTest, IsAutoCacheEnable, TestSize.Level1)
{
    RSOpincManager::Instance().SetOPIncSwitch(true);
    ASSERT_TRUE(DrawableV2::RSOpincDrawCache::IsAutoCacheEnable());

    RSOpincManager::Instance().SetOPIncSwitch(false);
    ASSERT_FALSE(DrawableV2::RSOpincDrawCache::IsAutoCacheEnable());
}

/**
 * @tc.name: IsAutoCacheDebugEnable
 * @tc.desc: Test IsAutoCacheDebugEnable
 * @tc.type: FUNC
 * @tc.require: issueIC87OJ
 */
HWTEST_F(RSOpincDrawCacheTest, IsAutoCacheDebugEnable, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    RSOpincManager::Instance().SetOPIncSwitch(true);
    bool debugEnable = opincDrawCache.IsAutoCacheDebugEnable();
    ASSERT_EQ(debugEnable, RSSystemProperties::GetAutoCacheDebugEnabled() && true);

    RSOpincManager::Instance().SetOPIncSwitch(false);
    debugEnable = opincDrawCache.IsAutoCacheDebugEnable();
    ASSERT_FALSE(debugEnable);
}

/**
 * @tc.name: GetOpincCacheMaxWidthWithScreen
 * @tc.desc: Test GetOpincCacheMaxWidth with screen rect set
 * @tc.type: FUNC
 * @tc.require: issueIC87OJ
 */
HWTEST_F(RSOpincDrawCacheTest, GetOpincCacheMaxWidthWithScreen, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    DrawableV2::RSOpincDrawCache::SetScreenRectInfo({0, 0, 1080, 1920});
    int32_t width = opincDrawCache.GetOpincCacheMaxWidth();
    ASSERT_GT(width, 0);
    ASSERT_GT(width, 1080);
    DrawableV2::RSOpincDrawCache::SetScreenRectInfo({0, 0, 0, 0});
}

/**
 * @tc.name: GetOpincCacheMaxHeightWithScreen
 * @tc.desc: Test GetOpincCacheMaxHeight with screen rect set
 * @tc.type: FUNC
 * @tc.require: issueIC87OJ
 */
HWTEST_F(RSOpincDrawCacheTest, GetOpincCacheMaxHeightWithScreen, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    DrawableV2::RSOpincDrawCache::SetScreenRectInfo({0, 0, 1080, 1920});
    int32_t height = opincDrawCache.GetOpincCacheMaxHeight();
    ASSERT_EQ(height, 1920);
    DrawableV2::RSOpincDrawCache::SetScreenRectInfo({0, 0, 0, 0});
}

/**
 * @tc.name: PushLayerPartRenderDirtyRegion
 * @tc.desc: Test PushLayerPartRenderDirtyRegion
 * @tc.type: FUNC
 * @tc.require: issueIC87OJ
 */
HWTEST_F(RSOpincDrawCacheTest, PushLayerPartRenderDirtyRegion, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    RSRenderParams params(id);
    auto dirtyManager = std::make_shared<RSDirtyRegionManager>();
    ASSERT_NE(dirtyManager, nullptr);

    RectI dirtyRect = { LAYER_PART_RENDER_DIRTY_OFFSET, LAYER_PART_RENDER_DIRTY_OFFSET,
        LAYER_PART_RENDER_DIRTY_SIZE, LAYER_PART_RENDER_DIRTY_SIZE };
    dirtyManager->SetLayerPartRenderCurrentFrameDirtyRegion(dirtyRect);
    dirtyManager->SetLayerPartRenderEnabled(true);
    ASSERT_TRUE(dirtyManager->GetLayerPartRenderEnabled());

    opincDrawCache.PushLayerPartRenderDirtyRegion(dirtyManager, params, paintFilterCanvas,
        LAYER_PART_RENDER_NODE_COUNT);
    SUCCEED();
}

/**
 * @tc.name: LayerPartRenderClipDirtyRegion
 * @tc.desc: Test LayerPartRenderClipDirtyRegion
 * @tc.type: FUNC
 * @tc.require: issueIC87OJ
 */
HWTEST_F(RSOpincDrawCacheTest, LayerPartRenderClipDirtyRegion, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    auto dirtyManager = std::make_shared<RSDirtyRegionManager>();
    ASSERT_NE(dirtyManager, nullptr);

    RectI dirtyRect = { LAYER_PART_RENDER_DIRTY_OFFSET, LAYER_PART_RENDER_DIRTY_OFFSET,
        LAYER_PART_RENDER_DIRTY_SIZE, LAYER_PART_RENDER_DIRTY_SIZE };
    dirtyManager->SetLayerPartRenderCurrentFrameDirtyRegion(dirtyRect);
    dirtyManager->SetLayerPartRenderEnabled(true);

    opincDrawCache.LayerPartRenderClipDirtyRegion(dirtyManager, paintFilterCanvas);
    ASSERT_TRUE(paintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());

    dirtyManager->SetLayerPartRenderEnabled(false);
    opincDrawCache.LayerPartRenderClipDirtyRegion(dirtyManager, paintFilterCanvas);
    ASSERT_TRUE(paintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());
}

/**
 * @tc.name: PopLayerPartRenderDirtyRegion
 * @tc.desc: Test PopLayerPartRenderDirtyRegion
 * @tc.type: FUNC
 * @tc.require: issueIC87OJ
 */
HWTEST_F(RSOpincDrawCacheTest, PopLayerPartRenderDirtyRegion, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    auto dirtyManager = std::make_shared<RSDirtyRegionManager>();
    ASSERT_NE(dirtyManager, nullptr);

    dirtyManager->SetLayerPartRenderEnabled(true);
    ASSERT_TRUE(dirtyManager->GetLayerPartRenderEnabled());
    opincDrawCache.PopLayerPartRenderDirtyRegion(dirtyManager, paintFilterCanvas);
    SUCCEED();
}

/**
 * @tc.name: IsOpincNodeInScreenRectBoundary
 * @tc.desc: Test IsOpincNodeInScreenRect with boundary conditions
 * @tc.type: FUNC
 * @tc.require: issueIC87OJ
 */
HWTEST_F(RSOpincDrawCacheTest, IsOpincNodeInScreenRectBoundary, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    RSRenderParams params(id);

    DrawableV2::RSOpincDrawCache::SetScreenRectInfo({0, 0, 100, 100});

    RectI absRect1 = {0, 0, 50, 50};
    params.SetAbsDrawRect(absRect1);
    ASSERT_TRUE(opincDrawCache.IsOpincNodeInScreenRect(params));

    RectI absRect2 = {50, 50, 50, 50};
    params.SetAbsDrawRect(absRect2);
    ASSERT_TRUE(opincDrawCache.IsOpincNodeInScreenRect(params));

    RectI absRect3 = {100, 100, 50, 50};
    params.SetAbsDrawRect(absRect3);
    ASSERT_TRUE(opincDrawCache.IsOpincNodeInScreenRect(params));

    RectI absRect4 = {-10, -10, 5, 5};
    params.SetAbsDrawRect(absRect4);
    ASSERT_FALSE(opincDrawCache.IsOpincNodeInScreenRect(params));

    DrawableV2::RSOpincDrawCache::SetScreenRectInfo({0, 0, 0, 0});
}

/**
 * @tc.name: IsTranslateWithEdgeCases
 * @tc.desc: Test IsTranslate with edge cases
 * @tc.type: FUNC
 * @tc.require: issueIC87OJ
 */
HWTEST_F(RSOpincDrawCacheTest, IsTranslateWithEdgeCases, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;

    Drawing::Matrix identityMatrix;
    ASSERT_TRUE(opincDrawCache.IsTranslate(identityMatrix));

    Drawing::Matrix translateMatrix;
    translateMatrix.Translate(1000.0f, 2000.0f);
    ASSERT_TRUE(opincDrawCache.IsTranslate(translateMatrix));

    Drawing::Matrix scaleMatrix;
    scaleMatrix.SetScale(1.0f, 1.0f);
    ASSERT_TRUE(opincDrawCache.IsTranslate(scaleMatrix));

    Drawing::Matrix rotateMatrix;
    rotateMatrix.Rotate(45.0f, 0.0f, 0.0f);
    ASSERT_FALSE(opincDrawCache.IsTranslate(rotateMatrix));

    Drawing::Matrix skewMatrix;
    skewMatrix.SetSkew(0.1f, 0.1f);
    ASSERT_FALSE(opincDrawCache.IsTranslate(skewMatrix));
}

/**
 * @tc.name: GetNodeCacheType
 * @tc.desc: Test GetNodeCacheType
 * @tc.type: FUNC
 * @tc.require: issueIC87OJ
 */
HWTEST_F(RSOpincDrawCacheTest, GetNodeCacheType, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache::SetNodeCacheType(NodeStrategyType::CACHE_NONE);
    ASSERT_EQ(DrawableV2::RSOpincDrawCache::GetNodeCacheType(), NodeStrategyType::CACHE_NONE);

    DrawableV2::RSOpincDrawCache::SetNodeCacheType(NodeStrategyType::OPINC_AUTOCACHE);
    ASSERT_EQ(DrawableV2::RSOpincDrawCache::GetNodeCacheType(), NodeStrategyType::OPINC_AUTOCACHE);

    DrawableV2::RSOpincDrawCache::SetNodeCacheType(NodeStrategyType::CACHE_DISABLE);
    ASSERT_EQ(DrawableV2::RSOpincDrawCache::GetNodeCacheType(), NodeStrategyType::CACHE_DISABLE);
}

/**
 * @tc.name: GetRecordState
 * @tc.desc: Test GetRecordState
 * @tc.type: FUNC
 * @tc.require: issueIC87OJ
 */
HWTEST_F(RSOpincDrawCacheTest, GetRecordState, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;

    opincDrawCache.recordState_ = NodeRecordState::RECORD_NONE;
    ASSERT_EQ(opincDrawCache.GetRecordState(), NodeRecordState::RECORD_NONE);

    opincDrawCache.recordState_ = NodeRecordState::RECORD_CALCULATE;
    ASSERT_EQ(opincDrawCache.GetRecordState(), NodeRecordState::RECORD_CALCULATE);

    opincDrawCache.recordState_ = NodeRecordState::RECORD_CACHING;
    ASSERT_EQ(opincDrawCache.GetRecordState(), NodeRecordState::RECORD_CACHING);

    opincDrawCache.recordState_ = NodeRecordState::RECORD_CACHED;
    ASSERT_EQ(opincDrawCache.GetRecordState(), NodeRecordState::RECORD_CACHED);
}

/**
 * @tc.name: GetRootNodeStrategyType
 * @tc.desc: Test GetRootNodeStrategyType
 * @tc.type: FUNC
 * @tc.require: issueIC87OJ
 */
HWTEST_F(RSOpincDrawCacheTest, GetRootNodeStrategyType, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;

    opincDrawCache.rootNodeStragyType_ = NodeStrategyType::CACHE_NONE;
    ASSERT_EQ(opincDrawCache.GetRootNodeStrategyType(), NodeStrategyType::CACHE_NONE);

    opincDrawCache.rootNodeStragyType_ = NodeStrategyType::OPINC_AUTOCACHE;
    ASSERT_EQ(opincDrawCache.GetRootNodeStrategyType(), NodeStrategyType::OPINC_AUTOCACHE);

    opincDrawCache.rootNodeStragyType_ = NodeStrategyType::CACHE_DISABLE;
    ASSERT_EQ(opincDrawCache.GetRootNodeStrategyType(), NodeStrategyType::CACHE_DISABLE);
}

/**
 * @tc.name: IsOpCanCache
 * @tc.desc: Test IsOpCanCache
 * @tc.type: FUNC
 * @tc.require: issueIC87OJ
 */
HWTEST_F(RSOpincDrawCacheTest, IsOpCanCache, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;

    opincDrawCache.opCanCache_ = false;
    ASSERT_FALSE(opincDrawCache.IsOpCanCache());

    opincDrawCache.opCanCache_ = true;
    ASSERT_TRUE(opincDrawCache.IsOpCanCache());
}

/**
 * @tc.name: GetDrawAreaEnableState
 * @tc.desc: Test GetDrawAreaEnableState
 * @tc.type: FUNC
 * @tc.require: issueIC87OJ
 */
HWTEST_F(RSOpincDrawCacheTest, GetDrawAreaEnableState, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;

    opincDrawCache.isDrawAreaEnable_ = DrawAreaEnableState::DRAW_AREA_INIT;
    ASSERT_EQ(opincDrawCache.GetDrawAreaEnableState(), DrawAreaEnableState::DRAW_AREA_INIT);

    opincDrawCache.isDrawAreaEnable_ = DrawAreaEnableState::DRAW_AREA_ENABLE;
    ASSERT_EQ(opincDrawCache.GetDrawAreaEnableState(), DrawAreaEnableState::DRAW_AREA_ENABLE);

    opincDrawCache.isDrawAreaEnable_ = DrawAreaEnableState::DRAW_AREA_DISABLE;
    ASSERT_EQ(opincDrawCache.GetDrawAreaEnableState(), DrawAreaEnableState::DRAW_AREA_DISABLE);
}

/**
 * @tc.name: GetOpincBlockNodeSkip
 * @tc.desc: Test GetOpincBlockNodeSkip
 * @tc.type: FUNC
 * @tc.require: issueIC87OJ
 */
HWTEST_F(RSOpincDrawCacheTest, GetOpincBlockNodeSkip, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache::SetOpincBlockNodeSkip(true);
    ASSERT_TRUE(DrawableV2::RSOpincDrawCache::GetOpincBlockNodeSkip());

    DrawableV2::RSOpincDrawCache::SetOpincBlockNodeSkip(false);
    ASSERT_FALSE(DrawableV2::RSOpincDrawCache::GetOpincBlockNodeSkip());
}

/**
 * @tc.name: BeforeDrawCacheFindRootNodeWithCacheSize
 * @tc.desc: Test BeforeDrawCacheFindRootNode with various cache sizes
 * @tc.type: FUNC
 * @tc.require: issueIC87OJ
 */
HWTEST_F(RSOpincDrawCacheTest, BeforeDrawCacheFindRootNodeWithCacheSize, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    RSRenderParams params(id);

    DrawableV2::RSOpincDrawCache::SetScreenRectInfo({0, 0, 1080, 1920});
    RSOpincManager::Instance().SetOPIncSwitch(true);
    params.isOpincRootFlag_ = true;
    paintFilterCanvas.SetCacheType(Drawing::CacheType::ENABLED);

    params.SetCacheSize({51, 51});
    opincDrawCache.BeforeDrawCacheFindRootNode(paintFilterCanvas, params);
    ASSERT_EQ(opincDrawCache.recordState_, NodeRecordState::RECORD_CALCULATE);

    opincDrawCache.recordState_ = NodeRecordState::RECORD_NONE;
    params.SetCacheSize({49, 49});
    opincDrawCache.BeforeDrawCacheFindRootNode(paintFilterCanvas, params);
    ASSERT_EQ(opincDrawCache.recordState_, NodeRecordState::RECORD_NONE);

    opincDrawCache.recordState_ = NodeRecordState::RECORD_NONE;
    params.SetCacheSize({2000, 2000});
    opincDrawCache.BeforeDrawCacheFindRootNode(paintFilterCanvas, params);
    ASSERT_EQ(opincDrawCache.recordState_, NodeRecordState::RECORD_NONE);

    DrawableV2::RSOpincDrawCache::SetScreenRectInfo({0, 0, 0, 0});
}

/**
 * @tc.name: DrawAutoCacheWithEmptyRects
 * @tc.desc: Test DrawAutoCache with empty rects
 * @tc.type: FUNC
 * @tc.require: issueIC87OJ
 */
HWTEST_F(RSOpincDrawCacheTest, DrawAutoCacheWithEmptyRects, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    Drawing::Image image;
    Drawing::SamplingOptions samplingOptions;

    opincDrawCache.opCanCache_ = true;
    opincDrawCache.isDrawAreaEnable_ = DrawAreaEnableState::DRAW_AREA_ENABLE;

    Drawing::OpListHandle::OpInfo opInfo1 = {
        .unionRect = Drawing::Rect {},
        .drawAreaRects = {}
    };
    opincDrawCache.opListDrawAreas_ = Drawing::OpListHandle(opInfo1);
    ASSERT_FALSE(opincDrawCache.DrawAutoCache(paintFilterCanvas, image, samplingOptions,
        Drawing::SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT));

    Drawing::OpListHandle::OpInfo opInfo2 = {
        .unionRect = Drawing::Rect { 100, 100, 300, 300 },
        .drawAreaRects = {}
    };
    opincDrawCache.opListDrawAreas_ = Drawing::OpListHandle(opInfo2);
    ASSERT_FALSE(opincDrawCache.DrawAutoCache(paintFilterCanvas, image, samplingOptions,
        Drawing::SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT));
}
} // namespace Rosen
} // namespace OHOS
