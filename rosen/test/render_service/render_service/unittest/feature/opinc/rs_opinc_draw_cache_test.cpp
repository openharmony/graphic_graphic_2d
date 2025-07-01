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

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
constexpr NodeId DEFAULT_ID = 0xFFFF;

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
    bool isOpincDropNodeExt = true;
    RSOpincManager::Instance().SetOPIncSwitch(true);
    opincDrawCache.OpincCalculateBefore(canvas, params, isOpincDropNodeExt);
    ASSERT_FALSE(opincDrawCache.isOpincCaculateStart_);

    RSOpincManager::Instance().SetOPIncSwitch(false);
    opincDrawCache.OpincCalculateBefore(canvas, params, isOpincDropNodeExt);
    ASSERT_FALSE(opincDrawCache.isOpincCaculateStart_);

    opincDrawCache.rootNodeStragyType_ = NodeStrategyType::OPINC_AUTOCACHE;
    opincDrawCache.recordState_ = NodeRecordState::RECORD_CALCULATE;

    RSOpincManager::Instance().SetOPIncSwitch(true);
    opincDrawCache.OpincCalculateBefore(canvas, params, isOpincDropNodeExt);
    ASSERT_TRUE(opincDrawCache.isOpincCaculateStart_);

    RSOpincManager::Instance().SetOPIncSwitch(false);
    opincDrawCache.OpincCalculateBefore(canvas, params, isOpincDropNodeExt);
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
    bool isOpincDropNodeExt = true;
    opincDrawCache.OpincCalculateAfter(canvas, isOpincDropNodeExt);
    ASSERT_FALSE(opincDrawCache.isOpincCaculateStart_);

    opincDrawCache.isOpincCaculateStart_ = true;
    opincDrawCache.OpincCalculateAfter(canvas, isOpincDropNodeExt);
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
    bool isOpincDropNodeExt = true;
    ASSERT_TRUE(opincDrawCache.PreDrawableCacheState(params, isOpincDropNodeExt));

    params.OpincSetCacheChangeFlag(false, true);
    opincDrawCache.isOpincRootNode_ = false;
    isOpincDropNodeExt = false;
    ASSERT_FALSE(opincDrawCache.PreDrawableCacheState(params, isOpincDropNodeExt));
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
    bool isOpincDropNodeExt = true;

    params.SetCacheSize({100, 100});
    paintFilterCanvas.SetCacheType(Drawing::CacheType::ENABLED);

    RSOpincManager::Instance().SetOPIncSwitch(true);
    params.isOpincRootFlag_ = true;
    opincDrawCache.BeforeDrawCacheFindRootNode(paintFilterCanvas, params, isOpincDropNodeExt);
    ASSERT_TRUE(opincDrawCache.recordState_ != NodeRecordState::RECORD_CALCULATE);

    params.isOpincRootFlag_ = false;
    opincDrawCache.BeforeDrawCacheFindRootNode(paintFilterCanvas, params, isOpincDropNodeExt);
    ASSERT_TRUE(opincDrawCache.recordState_ != NodeRecordState::RECORD_CALCULATE);

    RSOpincManager::Instance().SetOPIncSwitch(false);
    params.isOpincRootFlag_ = true;
    opincDrawCache.BeforeDrawCacheFindRootNode(paintFilterCanvas, params, isOpincDropNodeExt);
    ASSERT_TRUE(opincDrawCache.recordState_ != NodeRecordState::RECORD_CALCULATE);

    params.isOpincRootFlag_ = false;
    opincDrawCache.BeforeDrawCacheFindRootNode(paintFilterCanvas, params, isOpincDropNodeExt);
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
    bool isOpincDropNodeExt = true;
    RSOpincManager::Instance().SetOPIncSwitch(true);
    opincDrawCache.recordState_ = NodeRecordState::RECORD_CACHED;
    opincDrawCache.nodeCacheType_ = NodeStrategyType::DDGR_OPINC_DYNAMIC;
    opincDrawCache.BeforeDrawCache(canvas, params, isOpincDropNodeExt);
    ASSERT_FALSE(opincDrawCache.isOpincCaculateStart_);

    opincDrawCache.rootNodeStragyType_ = NodeStrategyType::OPINC_AUTOCACHE;
    opincDrawCache.nodeCacheType_ =  NodeStrategyType::OPINC_AUTOCACHE;
    opincDrawCache.recordState_ =NodeRecordState::RECORD_CALCULATE;
    opincDrawCache.BeforeDrawCache(canvas, params, isOpincDropNodeExt);
    ASSERT_TRUE(opincDrawCache.isOpincCaculateStart_);

    RSOpincManager::Instance().SetOPIncSwitch(false);
    opincDrawCache.isOpincCaculateStart_ = false;
    opincDrawCache.recordState_ = NodeRecordState::RECORD_CACHED;
    opincDrawCache.nodeCacheType_ = NodeStrategyType::DDGR_OPINC_DYNAMIC;
    opincDrawCache.BeforeDrawCache(canvas, params, isOpincDropNodeExt);
    ASSERT_FALSE(opincDrawCache.isOpincCaculateStart_);

    opincDrawCache.nodeCacheType_ = NodeStrategyType::CACHE_NONE;
    opincDrawCache.recordState_ = NodeRecordState::RECORD_NONE;
    opincDrawCache.BeforeDrawCache(canvas, params, isOpincDropNodeExt);
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
    bool isOpincDropNodeExt = true;
    int opincRootTotalCount = 3;

    RSOpincManager::Instance().SetOPIncSwitch(false);
    opincDrawCache.recordState_ = NodeRecordState::RECORD_CACHING;
    opincDrawCache.AfterDrawCache(canvas, params, isOpincDropNodeExt, opincRootTotalCount);
    ASSERT_TRUE(!opincDrawCache.isOpincMarkCached_);

    RSOpincManager::Instance().SetOPIncSwitch(true);
    opincDrawCache.rootNodeStragyType_ = NodeStrategyType::OPINC_AUTOCACHE;
    opincDrawCache.recordState_ = NodeRecordState::RECORD_CALCULATE;
    opincDrawCache.isDrawAreaEnable_ = DrawAreaEnableState::DRAW_AREA_ENABLE;
    opincDrawCache.AfterDrawCache(canvas, params, isOpincDropNodeExt, opincRootTotalCount);
    ASSERT_TRUE(opincDrawCache.recordState_ == NodeRecordState::RECORD_CACHING);

    opincDrawCache.rootNodeStragyType_ = NodeStrategyType::OPINC_AUTOCACHE;
    opincDrawCache.recordState_ = NodeRecordState::RECORD_CALCULATE;
    opincDrawCache.isDrawAreaEnable_ = DrawAreaEnableState::DRAW_AREA_DISABLE;
    opincDrawCache.AfterDrawCache(canvas, params, isOpincDropNodeExt, opincRootTotalCount);
    ASSERT_TRUE(opincDrawCache.recordState_ == NodeRecordState::RECORD_DISABLE);

    opincDrawCache.rootNodeStragyType_ = NodeStrategyType::OPINC_AUTOCACHE;
    opincDrawCache.recordState_ = NodeRecordState::RECORD_CALCULATE;
    opincDrawCache.isDrawAreaEnable_ = DrawAreaEnableState::DRAW_AREA_INIT;
    opincDrawCache.AfterDrawCache(canvas, params, isOpincDropNodeExt, opincRootTotalCount);
    ASSERT_TRUE(opincDrawCache.recordState_ == NodeRecordState::RECORD_CALCULATE);

    opincDrawCache.recordState_ = NodeRecordState::RECORD_CACHING;
    opincDrawCache.AfterDrawCache(canvas, params, isOpincDropNodeExt, opincRootTotalCount);
    ASSERT_TRUE(opincDrawCache.recordState_ == NodeRecordState::RECORD_CACHING);

    auto drawingCanvas = new Drawing::Canvas();
    auto paintFilterCanvas = new RSPaintFilterCanvas(drawingCanvas);
    paintFilterCanvas->SetAlpha(0.5f);
    auto canvasAlpha = static_cast<Drawing::Canvas*>(paintFilterCanvas);

    opincDrawCache.rootNodeStragyType_ = NodeStrategyType::OPINC_AUTOCACHE;
    opincDrawCache.recordState_ = NodeRecordState::RECORD_CALCULATE;
    opincDrawCache.isDrawAreaEnable_ = DrawAreaEnableState::DRAW_AREA_ENABLE;
    opincDrawCache.AfterDrawCache(*canvasAlpha, params, isOpincDropNodeExt, opincRootTotalCount);
    ASSERT_TRUE(opincDrawCache.recordState_ == NodeRecordState::RECORD_CALCULATE);

    opincDrawCache.rootNodeStragyType_ = NodeStrategyType::OPINC_AUTOCACHE;
    opincDrawCache.recordState_ = NodeRecordState::RECORD_CALCULATE;
    opincDrawCache.isDrawAreaEnable_ = DrawAreaEnableState::DRAW_AREA_DISABLE;
    opincDrawCache.AfterDrawCache(*canvasAlpha, params, isOpincDropNodeExt, opincRootTotalCount);
    ASSERT_TRUE(opincDrawCache.recordState_ == NodeRecordState::RECORD_DISABLE);
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
    bool isOpincDropNodeExt = true;
    int opincRootTotalCount = 0;
    opincDrawCache.rootNodeStragyType_ = NodeStrategyType::OPINC_AUTOCACHE;
    opincDrawCache.nodeCacheType_ = NodeStrategyType::CACHE_NONE;
    opincDrawCache.recordState_ = NodeRecordState::RECORD_CACHING;
    RectI absRect = {10, 10, 10, 10};
    params.SetAbsDrawRect(absRect);
    opincDrawCache.AfterDrawCache(canvas, params, isOpincDropNodeExt, opincRootTotalCount);
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
} // namespace Rosen
} // namespace OHOS