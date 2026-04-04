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

#include "feature/opinc/rs_opinc_draw_cache.h"
#include "params/rs_canvas_drawing_render_params.h"
#include "params/rs_render_params.h"
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
 * @tc.name: PushLayerPartRenderDirtyRegionDisabled
 * @tc.desc: Verify PushLayerPartRenderDirtyRegion returns early when layer-part render is disabled
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincDrawCacheLayerPartTest, PushLayerPartRenderDirtyRegionDisabled, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    auto params = CreateRenderParams(false);

    opincDrawCache.PushLayerPartRenderDirtyRegion(*params, paintFilterCanvas, paintFilterCanvas, TEST_NODE_COUNT);

    EXPECT_TRUE(paintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());
}

/**
 * @tc.name: PushLayerPartRenderDirtyRegionEnabled
 * @tc.desc: Verify PushLayerPartRenderDirtyRegion pushes current dirty region into stack
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincDrawCacheLayerPartTest, PushLayerPartRenderDirtyRegionEnabled, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    auto params = CreateRenderParams(true);

    opincDrawCache.PushLayerPartRenderDirtyRegion(*params, paintFilterCanvas, paintFilterCanvas, TEST_NODE_COUNT);

    EXPECT_FALSE(paintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());
}

/**
 * @tc.name: LayerPartRenderClipDirtyRegionDisabled
 * @tc.desc: Verify LayerPartRenderClipDirtyRegion returns early when layer-part render is disabled
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincDrawCacheLayerPartTest, LayerPartRenderClipDirtyRegionDisabled, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    auto params = CreateRenderParams(false);

    opincDrawCache.LayerPartRenderClipDirtyRegion(*params, paintFilterCanvas);

    EXPECT_TRUE(paintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());
}

/**
 * @tc.name: LayerPartRenderClipDirtyRegionWithEmptyDirty
 * @tc.desc: Verify LayerPartRenderClipDirtyRegion handles enabled state when dirty region is empty
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincDrawCacheLayerPartTest, LayerPartRenderClipDirtyRegionWithEmptyDirty, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    auto params = CreateRenderParams(true);

    opincDrawCache.LayerPartRenderClipDirtyRegion(*params, paintFilterCanvas);

    EXPECT_TRUE(paintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());
}

/**
 * @tc.name: LayerPartRenderClipDirtyRegionWithNonEmptyDirty
 * @tc.desc: Verify LayerPartRenderClipDirtyRegion handles enabled state when dirty region is non-empty
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincDrawCacheLayerPartTest, LayerPartRenderClipDirtyRegionWithNonEmptyDirty, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    auto params = CreateRenderParams(true);

    opincDrawCache.PushLayerPartRenderDirtyRegion(*params, paintFilterCanvas, paintFilterCanvas, TEST_NODE_COUNT);
    opincDrawCache.LayerPartRenderClipDirtyRegion(*params, paintFilterCanvas);

    EXPECT_FALSE(paintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());
}

/**
 * @tc.name: PopLayerPartRenderDirtyRegionDisabled
 * @tc.desc: Verify PopLayerPartRenderDirtyRegion returns early when layer-part render is disabled
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincDrawCacheLayerPartTest, PopLayerPartRenderDirtyRegionDisabled, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    auto enabledParams = CreateRenderParams(true);
    auto disabledParams = CreateRenderParams(false);

    opincDrawCache.PushLayerPartRenderDirtyRegion(*enabledParams,
        paintFilterCanvas, paintFilterCanvas, TEST_NODE_COUNT);
    EXPECT_FALSE(paintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());

    opincDrawCache.PopLayerPartRenderDirtyRegion(*disabledParams, paintFilterCanvas);

    EXPECT_FALSE(paintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());
}

/**
 * @tc.name: PopLayerPartRenderDirtyRegionEmptyStack
 * @tc.desc: Verify PopLayerPartRenderDirtyRegion handles empty stack gracefully
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincDrawCacheLayerPartTest, PopLayerPartRenderDirtyRegionEmptyStack, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    auto params = CreateRenderParams(true);

    opincDrawCache.PopLayerPartRenderDirtyRegion(*params, paintFilterCanvas);

    EXPECT_TRUE(paintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());
}

/**
 * @tc.name: PopLayerPartRenderDirtyRegionNonEmptyStack
 * @tc.desc: Verify PopLayerPartRenderDirtyRegion pops one layer-part dirty region when stack is non-empty
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincDrawCacheLayerPartTest, PopLayerPartRenderDirtyRegionNonEmptyStack, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    auto params = CreateRenderParams(true);

    opincDrawCache.PushLayerPartRenderDirtyRegion(*params, paintFilterCanvas, paintFilterCanvas, TEST_NODE_COUNT);
    EXPECT_FALSE(paintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());

    opincDrawCache.PopLayerPartRenderDirtyRegion(*params, paintFilterCanvas);

    EXPECT_TRUE(paintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());
}

/**
 * @tc.name: LayerDirtyRegionDfxWithDebugDisabled
 * @tc.desc: Verify LayerDirtyRegionDfx keeps no side effect when debug switch is disabled
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincDrawCacheLayerPartTest, LayerDirtyRegionDfxWithDebugDisabled, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    Drawing::RectI dirtyRect(DIRTY_LEFT, DIRTY_TOP, DIRTY_WIDTH, DIRTY_HEIGHT);
    const std::string debugKey = "rosen.layerPartRenderDfx.enabled";
    const std::string oldDebugValue = system::GetParameter(debugKey, "0");
    (void)system::SetParameter(debugKey, "0");

    opincDrawCache.LayerDirtyRegionDfx(paintFilterCanvas, dirtyRect);

    EXPECT_FALSE(RSSystemProperties::GetLayerPartRenderDebugEnabled());
    (void)system::SetParameter(debugKey, oldDebugValue);
}

/**
 * @tc.name: LayerDirtyRegionDfxWithDebugEnabled
 * @tc.desc: Verify LayerDirtyRegionDfx enters drawing branch when debug switch is enabled
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincDrawCacheLayerPartTest, LayerDirtyRegionDfxWithDebugEnabled, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    Drawing::RectI dirtyRect(DIRTY_LEFT, DIRTY_TOP, DIRTY_WIDTH, DIRTY_HEIGHT);
    const std::string debugKey = "rosen.layerPartRenderDfx.enabled";
    const std::string oldDebugValue = system::GetParameter(debugKey, "0");

    (void)system::SetParameter(debugKey, "1");
    EXPECT_TRUE(RSSystemProperties::GetLayerPartRenderDebugEnabled());

    opincDrawCache.LayerDirtyRegionDfx(paintFilterCanvas, dirtyRect);

    (void)system::SetParameter(debugKey, oldDebugValue);
}

/**
 * @tc.name: PushLayerPartRenderDirtyRegionInvertFailed
 * @tc.desc: Verify PushLayerPartRenderDirtyRegion returns early when current canvas matrix inversion fails
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincDrawCacheLayerPartTest, PushLayerPartRenderDirtyRegionInvertFailed, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    Drawing::Canvas curCanvas;
    Drawing::Canvas cacheCanvas;
    RSPaintFilterCanvas curPaintFilterCanvas(&curCanvas);
    RSPaintFilterCanvas cachePaintFilterCanvas(&cacheCanvas);
    auto params = CreateRenderParams(true);
    curPaintFilterCanvas.Scale(0.0f, 0.0f);

    opincDrawCache.PushLayerPartRenderDirtyRegion(*params,
        curPaintFilterCanvas, cachePaintFilterCanvas, TEST_NODE_COUNT);

    EXPECT_TRUE(cachePaintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());
}

/**
 * @tc.name: PushLayerPartRenderDirtyRegionMapRectFailed
 * @tc.desc: Verify PushLayerPartRenderDirtyRegion returns early when inverse matrix map rect fails
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincDrawCacheLayerPartTest, PushLayerPartRenderDirtyRegionMapRectFailed, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    Drawing::Canvas curCanvas;
    Drawing::Canvas cacheCanvas;
    RSPaintFilterCanvas curPaintFilterCanvas(&curCanvas);
    RSPaintFilterCanvas cachePaintFilterCanvas(&cacheCanvas);
    auto params = CreateRenderParams(true);
    curPaintFilterCanvas.Rotate(45.0f, 0.0f, 0.0f);

    opincDrawCache.PushLayerPartRenderDirtyRegion(*params,
        curPaintFilterCanvas, cachePaintFilterCanvas, TEST_NODE_COUNT);

    EXPECT_TRUE(cachePaintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());
}

/**
 * @tc.name: PushLayerPartRenderDirtyRegionAbsDrawRectEmpty
 * @tc.desc: Verify PushLayerPartRenderDirtyRegion returns early when mapped abs draw rect is empty
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincDrawCacheLayerPartTest, PushLayerPartRenderDirtyRegionAbsDrawRectEmpty, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    Drawing::Canvas curCanvas;
    Drawing::Canvas cacheCanvas;
    RSPaintFilterCanvas curPaintFilterCanvas(&curCanvas);
    RSPaintFilterCanvas cachePaintFilterCanvas(&cacheCanvas);
    auto params = CreateRenderParams(true);
    params->SetAbsDrawRect(RectI(DIRTY_LEFT, DIRTY_TOP, 0, DIRTY_HEIGHT));

    opincDrawCache.PushLayerPartRenderDirtyRegion(*params,
        curPaintFilterCanvas, cachePaintFilterCanvas, TEST_NODE_COUNT);

    EXPECT_TRUE(cachePaintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());
}

/**
 * @tc.name: PushLayerPartRenderDirtyRegionSwitchToCurrentFrameDirty
 * @tc.desc: Verify dirty source switches from abs draw rect to current-frame dirty after warmup count
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincDrawCacheLayerPartTest, PushLayerPartRenderDirtyRegionSwitchToCurrentFrameDirty, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    Drawing::Canvas curCanvas;
    Drawing::Canvas cacheCanvas;
    RSPaintFilterCanvas curPaintFilterCanvas(&curCanvas);
    RSPaintFilterCanvas cachePaintFilterCanvas(&cacheCanvas);
    auto params = CreateRenderParams(true);
    params->SetLayerPartRenderCurrentFrameDirtyRegion(PARTIAL_DIRTY_RECT);

    for (int32_t i = 0; i < WARMUP_PUSH_COUNT; ++i) {
        opincDrawCache.PushLayerPartRenderDirtyRegion(*params,
            curPaintFilterCanvas, cachePaintFilterCanvas, TEST_NODE_COUNT);
        ASSERT_FALSE(cachePaintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());
        auto warmupBounds = cachePaintFilterCanvas.GetCurLayerPartRenderDirtyRegion().GetBounds();
        EXPECT_EQ(warmupBounds.GetLeft(), FULL_CACHE_REGION_LEFT);
        EXPECT_EQ(warmupBounds.GetTop(), FULL_CACHE_REGION_TOP);
        EXPECT_EQ(warmupBounds.GetWidth(), FULL_CACHE_REGION_WIDTH);
        EXPECT_EQ(warmupBounds.GetHeight(), FULL_CACHE_REGION_HEIGHT);
        opincDrawCache.PopLayerPartRenderDirtyRegion(*params, cachePaintFilterCanvas);
    }

    opincDrawCache.PushLayerPartRenderDirtyRegion(*params,
        curPaintFilterCanvas, cachePaintFilterCanvas, TEST_NODE_COUNT);
    ASSERT_FALSE(cachePaintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());
    auto switchedBounds = cachePaintFilterCanvas.GetCurLayerPartRenderDirtyRegion().GetBounds();
    EXPECT_EQ(switchedBounds.GetLeft(), PARTIAL_CACHE_REGION_LEFT);
    EXPECT_EQ(switchedBounds.GetTop(), PARTIAL_CACHE_REGION_TOP);
    EXPECT_EQ(switchedBounds.GetWidth(), PARTIAL_CACHE_REGION_WIDTH);
    EXPECT_EQ(switchedBounds.GetHeight(), PARTIAL_CACHE_REGION_HEIGHT);
}

/**
 * @tc.name: ResetUpdateLayerPartRenderCacheResetsWarmup
 * @tc.desc: Verify ResetUpdateLayerPartRenderCache clears warmup count so next push uses abs draw rect again
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincDrawCacheLayerPartTest, ResetUpdateLayerPartRenderCacheResetsWarmup, TestSize.Level1)
{
    DrawableV2::RSOpincDrawCache opincDrawCache;
    Drawing::Canvas curCanvas;
    Drawing::Canvas cacheCanvas;
    RSPaintFilterCanvas curPaintFilterCanvas(&curCanvas);
    RSPaintFilterCanvas cachePaintFilterCanvas(&cacheCanvas);
    auto params = CreateRenderParams(true);
    params->SetLayerPartRenderCurrentFrameDirtyRegion(PARTIAL_DIRTY_RECT);

    for (int32_t i = 0; i <= WARMUP_PUSH_COUNT; ++i) {
        opincDrawCache.PushLayerPartRenderDirtyRegion(*params,
            curPaintFilterCanvas, cachePaintFilterCanvas, TEST_NODE_COUNT);
        ASSERT_FALSE(cachePaintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());
        opincDrawCache.PopLayerPartRenderDirtyRegion(*params, cachePaintFilterCanvas);
    }

    opincDrawCache.ResetUpdateLayerPartRenderCache();
    opincDrawCache.PushLayerPartRenderDirtyRegion(*params,
        curPaintFilterCanvas, cachePaintFilterCanvas, TEST_NODE_COUNT);

    ASSERT_FALSE(cachePaintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());
    auto resetBounds = cachePaintFilterCanvas.GetCurLayerPartRenderDirtyRegion().GetBounds();
    EXPECT_EQ(resetBounds.GetLeft(), FULL_CACHE_REGION_LEFT);
    EXPECT_EQ(resetBounds.GetTop(), FULL_CACHE_REGION_TOP);
    EXPECT_EQ(resetBounds.GetWidth(), FULL_CACHE_REGION_WIDTH);
    EXPECT_EQ(resetBounds.GetHeight(), FULL_CACHE_REGION_HEIGHT);
}

} // namespace OHOS::Rosen
