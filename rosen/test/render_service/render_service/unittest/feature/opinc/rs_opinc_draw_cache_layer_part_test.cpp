/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "feature/opinc/rs_layer_part_draw_cache.h"
#include "params/rs_canvas_drawing_render_params.h"
#include "parameters.h"
#include "platform/common/rs_system_properties.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
constexpr int32_t DIRTY_LEFT = 10;
constexpr int32_t DIRTY_TOP = 10;
constexpr int32_t DIRTY_WIDTH = 100;
constexpr int32_t DIRTY_HEIGHT = 100;
constexpr int32_t TEST_NODE_COUNT = 5;
constexpr int32_t CACHE_WIDTH = 200;
constexpr int32_t CACHE_HEIGHT = 200;
constexpr int32_t PARTIAL_DIRTY_LEFT = 35;
constexpr int32_t PARTIAL_DIRTY_TOP = 45;
constexpr int32_t PARTIAL_DIRTY_WIDTH = 25;
constexpr int32_t PARTIAL_DIRTY_HEIGHT = 15;
constexpr int32_t FULL_CACHE_REGION_LEFT = 0;
constexpr int32_t FULL_CACHE_REGION_TOP = 0;
constexpr int32_t FULL_CACHE_REGION_WIDTH = 220;
constexpr int32_t FULL_CACHE_REGION_HEIGHT = 220;
constexpr int32_t PARTIAL_CACHE_REGION_LEFT = 50;
constexpr int32_t PARTIAL_CACHE_REGION_TOP = 70;
constexpr int32_t PARTIAL_CACHE_REGION_WIDTH = 50;
constexpr int32_t PARTIAL_CACHE_REGION_HEIGHT = 30;
constexpr int32_t WARMUP_PUSH_COUNT = 4;
constexpr int32_t DEBUG_OFF = 0;
constexpr int32_t DEBUG_ON = 1;
const std::string DEBUG_KEY = "rosen.layerPartRenderDfx.enabled";
const RectI DEFAULT_DIRTY_RECT = { DIRTY_LEFT, DIRTY_TOP, DIRTY_WIDTH, DIRTY_HEIGHT };
const RectI PARTIAL_DIRTY_RECT = {
    PARTIAL_DIRTY_LEFT, PARTIAL_DIRTY_TOP, PARTIAL_DIRTY_WIDTH, PARTIAL_DIRTY_HEIGHT
};
const RectI FULL_CACHE_REGION = {
    FULL_CACHE_REGION_LEFT, FULL_CACHE_REGION_TOP, FULL_CACHE_REGION_WIDTH, FULL_CACHE_REGION_HEIGHT
};
const RectI PARTIAL_CACHE_REGION = {
    PARTIAL_CACHE_REGION_LEFT, PARTIAL_CACHE_REGION_TOP, PARTIAL_CACHE_REGION_WIDTH, PARTIAL_CACHE_REGION_HEIGHT
};
}

class RSOpincDrawCacheLayerPartTest : public testing::Test {
public:
    static std::shared_ptr<RSCanvasDrawingRenderParams> CreateRenderParams(bool enabled)
    {
        auto params = std::make_shared<RSCanvasDrawingRenderParams>(GenerateUniqueNodeIdForRS());
        params->SetLayerPartRenderEnabled(enabled);
        params->SetLayerPartRenderCurrentFrameDirtyRegion(DEFAULT_DIRTY_RECT);
        params->SetAbsDrawRect(DEFAULT_DIRTY_RECT);
        params->SetCacheSize({ CACHE_WIDTH, CACHE_HEIGHT });
        return params;
    }
};

/**
 * @tc.name: CachePushLayerPartRenderDirtyRegionInvertFailed
 * @tc.desc: Verify RSLayerPartDrawCache push returns early when matrix inversion fails
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincDrawCacheLayerPartTest, CachePushLayerPartRenderDirtyRegionInvertFailed, TestSize.Level1)
{
    DrawableV2::RSLayerPartDrawCache layerPartDrawCache;
    Drawing::Canvas curCanvas;
    Drawing::Canvas cacheCanvas;
    RSPaintFilterCanvas curPaintFilterCanvas(&curCanvas);
    RSPaintFilterCanvas cachePaintFilterCanvas(&cacheCanvas);
    auto params = CreateRenderParams(true);
    curPaintFilterCanvas.Scale(0.0f, 0.0f);

    layerPartDrawCache.PushLayerPartRenderDirtyRegion(*params,
        curPaintFilterCanvas, cachePaintFilterCanvas, TEST_NODE_COUNT);

    EXPECT_TRUE(cachePaintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());
}

/**
 * @tc.name: CachePushLayerPartRenderDirtyRegionMapRectFailed
 * @tc.desc: Verify RSLayerPartDrawCache push returns early when inverse matrix map rect fails
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincDrawCacheLayerPartTest, CachePushLayerPartRenderDirtyRegionMapRectFailed, TestSize.Level1)
{
    DrawableV2::RSLayerPartDrawCache layerPartDrawCache;
    Drawing::Canvas curCanvas;
    Drawing::Canvas cacheCanvas;
    RSPaintFilterCanvas curPaintFilterCanvas(&curCanvas);
    RSPaintFilterCanvas cachePaintFilterCanvas(&cacheCanvas);
    auto params = CreateRenderParams(true);
    curPaintFilterCanvas.Rotate(45.0f, 0.0f, 0.0f);

    layerPartDrawCache.PushLayerPartRenderDirtyRegion(*params,
        curPaintFilterCanvas, cachePaintFilterCanvas, TEST_NODE_COUNT);

    EXPECT_TRUE(cachePaintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());
}

/**
 * @tc.name: CachePushLayerPartRenderDirtyRegionAbsDrawRectEmpty
 * @tc.desc: Verify RSLayerPartDrawCache push returns early when mapped abs draw rect is empty
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincDrawCacheLayerPartTest, CachePushLayerPartRenderDirtyRegionAbsDrawRectEmpty, TestSize.Level1)
{
    DrawableV2::RSLayerPartDrawCache layerPartDrawCache;
    Drawing::Canvas curCanvas;
    Drawing::Canvas cacheCanvas;
    RSPaintFilterCanvas curPaintFilterCanvas(&curCanvas);
    RSPaintFilterCanvas cachePaintFilterCanvas(&cacheCanvas);
    auto params = CreateRenderParams(true);
    params->SetAbsDrawRect(RectI(DIRTY_LEFT, DIRTY_TOP, 0, DIRTY_HEIGHT));

    layerPartDrawCache.PushLayerPartRenderDirtyRegion(*params,
        curPaintFilterCanvas, cachePaintFilterCanvas, TEST_NODE_COUNT);

    EXPECT_TRUE(cachePaintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());
}

/**
 * @tc.name: CachePushLayerPartRenderDirtyRegionSwitchToCurrentFrameDirty
 * @tc.desc: Verify dirty source switches after warmup count
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincDrawCacheLayerPartTest, CachePushLayerPartRenderDirtyRegionSwitchToCurrentFrameDirty,
    TestSize.Level1)
{
    DrawableV2::RSLayerPartDrawCache layerPartDrawCache;
    Drawing::Canvas curCanvas;
    Drawing::Canvas cacheCanvas;
    RSPaintFilterCanvas curPaintFilterCanvas(&curCanvas);
    RSPaintFilterCanvas cachePaintFilterCanvas(&cacheCanvas);
    auto params = CreateRenderParams(true);
    params->SetLayerPartRenderCurrentFrameDirtyRegion(PARTIAL_DIRTY_RECT);

    for (int32_t i = 0; i < WARMUP_PUSH_COUNT; ++i) {
        layerPartDrawCache.PushLayerPartRenderDirtyRegion(*params,
            curPaintFilterCanvas, cachePaintFilterCanvas, TEST_NODE_COUNT);
        ASSERT_FALSE(cachePaintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());
        auto warmupBounds = cachePaintFilterCanvas.GetCurLayerPartRenderDirtyRegion().GetBounds();
        EXPECT_EQ(warmupBounds.GetLeft(), FULL_CACHE_REGION_LEFT);
        EXPECT_EQ(warmupBounds.GetTop(), FULL_CACHE_REGION_TOP);
        EXPECT_EQ(warmupBounds.GetWidth(), FULL_CACHE_REGION_WIDTH);
        EXPECT_EQ(warmupBounds.GetHeight(), FULL_CACHE_REGION_HEIGHT);
        layerPartDrawCache.PopLayerPartRenderDirtyRegion(cachePaintFilterCanvas);
    }

    layerPartDrawCache.PushLayerPartRenderDirtyRegion(*params,
        curPaintFilterCanvas, cachePaintFilterCanvas, TEST_NODE_COUNT);
    ASSERT_FALSE(cachePaintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());
    auto switchedBounds = cachePaintFilterCanvas.GetCurLayerPartRenderDirtyRegion().GetBounds();
    EXPECT_EQ(switchedBounds.GetLeft(), PARTIAL_CACHE_REGION_LEFT);
    EXPECT_EQ(switchedBounds.GetTop(), PARTIAL_CACHE_REGION_TOP);
    EXPECT_EQ(switchedBounds.GetWidth(), PARTIAL_CACHE_REGION_WIDTH);
    EXPECT_EQ(switchedBounds.GetHeight(), PARTIAL_CACHE_REGION_HEIGHT);
}

/**
 * @tc.name: CacheLayerPartRenderClipDirtyRegionWithEmptyDirty
 * @tc.desc: Verify clip returns when dirty region is empty
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincDrawCacheLayerPartTest, CacheLayerPartRenderClipDirtyRegionWithEmptyDirty, TestSize.Level1)
{
    DrawableV2::RSLayerPartDrawCache layerPartDrawCache;
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);

    layerPartDrawCache.LayerPartRenderClipDirtyRegion(paintFilterCanvas);

    EXPECT_TRUE(paintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());
}

/**
 * @tc.name: CacheLayerPartRenderClipDirtyRegionWithNonEmptyDirty
 * @tc.desc: Verify clip works when dirty region is non-empty
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincDrawCacheLayerPartTest, CacheLayerPartRenderClipDirtyRegionWithNonEmptyDirty, TestSize.Level1)
{
    DrawableV2::RSLayerPartDrawCache layerPartDrawCache;
    Drawing::Canvas curCanvas;
    Drawing::Canvas cacheCanvas;
    RSPaintFilterCanvas curPaintFilterCanvas(&curCanvas);
    RSPaintFilterCanvas cachePaintFilterCanvas(&cacheCanvas);
    auto params = CreateRenderParams(true);

    layerPartDrawCache.PushLayerPartRenderDirtyRegion(*params,
        curPaintFilterCanvas, cachePaintFilterCanvas, TEST_NODE_COUNT);
    layerPartDrawCache.LayerPartRenderClipDirtyRegion(cachePaintFilterCanvas);

    EXPECT_FALSE(cachePaintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());
}

/**
 * @tc.name: CachePopLayerPartRenderDirtyRegionDebugDisabled
 * @tc.desc: Verify pop branch with debug disabled
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincDrawCacheLayerPartTest, CachePopLayerPartRenderDirtyRegionDebugDisabled, TestSize.Level1)
{
    DrawableV2::RSLayerPartDrawCache layerPartDrawCache;
    Drawing::Canvas curCanvas;
    Drawing::Canvas cacheCanvas;
    RSPaintFilterCanvas curPaintFilterCanvas(&curCanvas);
    RSPaintFilterCanvas cachePaintFilterCanvas(&cacheCanvas);
    auto params = CreateRenderParams(true);
    const std::string oldDebugValue = system::GetParameter(DEBUG_KEY, std::to_string(DEBUG_OFF));
    (void)system::SetParameter(DEBUG_KEY, std::to_string(DEBUG_OFF));

    layerPartDrawCache.PushLayerPartRenderDirtyRegion(*params,
        curPaintFilterCanvas, cachePaintFilterCanvas, TEST_NODE_COUNT);
    ASSERT_FALSE(cachePaintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());

    layerPartDrawCache.PopLayerPartRenderDirtyRegion(cachePaintFilterCanvas);
    EXPECT_TRUE(cachePaintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());
    EXPECT_FALSE(RSSystemProperties::GetLayerPartRenderDebugEnabled());

    (void)system::SetParameter(DEBUG_KEY, oldDebugValue);
}

/**
 * @tc.name: CachePopLayerPartRenderDirtyRegionDebugEnabled
 * @tc.desc: Verify pop branch with debug enabled
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincDrawCacheLayerPartTest, CachePopLayerPartRenderDirtyRegionDebugEnabled, TestSize.Level1)
{
    DrawableV2::RSLayerPartDrawCache layerPartDrawCache;
    Drawing::Canvas curCanvas;
    Drawing::Canvas cacheCanvas;
    RSPaintFilterCanvas curPaintFilterCanvas(&curCanvas);
    RSPaintFilterCanvas cachePaintFilterCanvas(&cacheCanvas);
    auto params = CreateRenderParams(true);
    const std::string oldDebugValue = system::GetParameter(DEBUG_KEY, std::to_string(DEBUG_OFF));
    (void)system::SetParameter(DEBUG_KEY, std::to_string(DEBUG_ON));
    EXPECT_TRUE(RSSystemProperties::GetLayerPartRenderDebugEnabled());

    layerPartDrawCache.PushLayerPartRenderDirtyRegion(*params,
        curPaintFilterCanvas, cachePaintFilterCanvas, TEST_NODE_COUNT);
    ASSERT_FALSE(cachePaintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());
    layerPartDrawCache.PopLayerPartRenderDirtyRegion(cachePaintFilterCanvas);

    EXPECT_TRUE(cachePaintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());
    (void)system::SetParameter(DEBUG_KEY, oldDebugValue);
}

/**
 * @tc.name: CacheResetAndClearRestoreWarmup
 * @tc.desc: Verify ResetUnchangeCount and Clear both restore warmup behavior
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincDrawCacheLayerPartTest, CacheResetAndClearRestoreWarmup, TestSize.Level1)
{
    DrawableV2::RSLayerPartDrawCache layerPartDrawCache;
    Drawing::Canvas curCanvas;
    Drawing::Canvas cacheCanvas;
    RSPaintFilterCanvas curPaintFilterCanvas(&curCanvas);
    RSPaintFilterCanvas cachePaintFilterCanvas(&cacheCanvas);
    auto params = CreateRenderParams(true);
    params->SetLayerPartRenderCurrentFrameDirtyRegion(PARTIAL_DIRTY_RECT);

    for (int32_t i = 0; i <= WARMUP_PUSH_COUNT; ++i) {
        layerPartDrawCache.PushLayerPartRenderDirtyRegion(*params,
            curPaintFilterCanvas, cachePaintFilterCanvas, TEST_NODE_COUNT);
        ASSERT_FALSE(cachePaintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());
        layerPartDrawCache.PopLayerPartRenderDirtyRegion(cachePaintFilterCanvas);
    }

    layerPartDrawCache.ResetUnchangeCount();
    layerPartDrawCache.PushLayerPartRenderDirtyRegion(*params,
        curPaintFilterCanvas, cachePaintFilterCanvas, TEST_NODE_COUNT);
    auto resetBounds = cachePaintFilterCanvas.GetCurLayerPartRenderDirtyRegion().GetBounds();
    EXPECT_EQ(resetBounds.GetLeft(), FULL_CACHE_REGION_LEFT);
    EXPECT_EQ(resetBounds.GetTop(), FULL_CACHE_REGION_TOP);
    EXPECT_EQ(resetBounds.GetWidth(), FULL_CACHE_REGION_WIDTH);
    EXPECT_EQ(resetBounds.GetHeight(), FULL_CACHE_REGION_HEIGHT);
    layerPartDrawCache.PopLayerPartRenderDirtyRegion(cachePaintFilterCanvas);

    layerPartDrawCache.Clear();
    layerPartDrawCache.PushLayerPartRenderDirtyRegion(*params,
        curPaintFilterCanvas, cachePaintFilterCanvas, TEST_NODE_COUNT);
    auto clearBounds = cachePaintFilterCanvas.GetCurLayerPartRenderDirtyRegion().GetBounds();
    EXPECT_EQ(clearBounds.GetLeft(), FULL_CACHE_REGION_LEFT);
    EXPECT_EQ(clearBounds.GetTop(), FULL_CACHE_REGION_TOP);
    EXPECT_EQ(clearBounds.GetWidth(), FULL_CACHE_REGION_WIDTH);
    EXPECT_EQ(clearBounds.GetHeight(), FULL_CACHE_REGION_HEIGHT);
}

} // namespace OHOS::Rosen
