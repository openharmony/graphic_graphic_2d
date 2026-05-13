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

#include "drawable/rs_render_node_drawable.h"
#include "feature/opinc/rs_layer_part_draw_cache_helper.h"
#include "params/rs_canvas_drawing_render_params.h"
#include "pipeline/rs_render_node.h"

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
constexpr int32_t WARMUP_PUSH_COUNT = 4;
constexpr int32_t FULL_CACHE_REGION_LEFT = 0;
constexpr int32_t FULL_CACHE_REGION_TOP = 0;
constexpr int32_t FULL_CACHE_REGION_WIDTH = 220;
constexpr int32_t FULL_CACHE_REGION_HEIGHT = 220;
constexpr int32_t PARTIAL_CACHE_REGION_LEFT = 50;
constexpr int32_t PARTIAL_CACHE_REGION_TOP = 70;
constexpr int32_t PARTIAL_CACHE_REGION_WIDTH = 50;
constexpr int32_t PARTIAL_CACHE_REGION_HEIGHT = 30;
const RectI DEFAULT_DIRTY_RECT = { DIRTY_LEFT, DIRTY_TOP, DIRTY_WIDTH, DIRTY_HEIGHT };
const RectI PARTIAL_DIRTY_RECT = {
    PARTIAL_DIRTY_LEFT, PARTIAL_DIRTY_TOP, PARTIAL_DIRTY_WIDTH, PARTIAL_DIRTY_HEIGHT
};
}

class RSLayerPartDrawCacheHelperTest : public testing::Test {
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

    static std::shared_ptr<DrawableV2::RSRenderNodeDrawable> CreateDrawable()
    {
        auto renderNode = std::make_shared<RSRenderNode>(GenerateUniqueNodeIdForRS());
        return std::static_pointer_cast<DrawableV2::RSRenderNodeDrawable>(
            DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(renderNode));
    }
};

/**
 * @tc.name: HelperPushLayerPartRenderDirtyRegionBranches
 * @tc.desc: Verify helper push branches: disabled, null cache, and valid cache
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSLayerPartDrawCacheHelperTest, HelperPushLayerPartRenderDirtyRegionBranches, TestSize.Level1)
{
    auto drawable = CreateDrawable();
    ASSERT_NE(drawable, nullptr);
    Drawing::Canvas curCanvas;
    Drawing::Canvas cacheCanvas;
    RSPaintFilterCanvas curPaintFilterCanvas(&curCanvas);
    RSPaintFilterCanvas cachePaintFilterCanvas(&cacheCanvas);
    auto enabledParams = CreateRenderParams(true);
    auto disabledParams = CreateRenderParams(false);

    DrawableV2::RSLayerPartDrawCacheHelper::PushLayerPartRenderDirtyRegion(*drawable,
        *disabledParams, curPaintFilterCanvas, cachePaintFilterCanvas, TEST_NODE_COUNT);
    EXPECT_TRUE(cachePaintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());

    DrawableV2::RSLayerPartDrawCacheHelper::PushLayerPartRenderDirtyRegion(*drawable,
        *enabledParams, curPaintFilterCanvas, cachePaintFilterCanvas, TEST_NODE_COUNT);
    EXPECT_TRUE(cachePaintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());

    DrawableV2::RSLayerPartDrawCacheHelper::ResetUpdateLayerPartRenderCache(*drawable);
    DrawableV2::RSLayerPartDrawCacheHelper::PushLayerPartRenderDirtyRegion(*drawable,
        *enabledParams, curPaintFilterCanvas, cachePaintFilterCanvas, TEST_NODE_COUNT);
    EXPECT_FALSE(cachePaintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());
}

/**
 * @tc.name: HelperLayerPartRenderClipDirtyRegionBranches
 * @tc.desc: Verify helper clip branches: disabled, null cache, and valid cache
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSLayerPartDrawCacheHelperTest, HelperLayerPartRenderClipDirtyRegionBranches, TestSize.Level1)
{
    auto drawable = CreateDrawable();
    ASSERT_NE(drawable, nullptr);
    Drawing::Canvas curCanvas;
    Drawing::Canvas cacheCanvas;
    RSPaintFilterCanvas curPaintFilterCanvas(&curCanvas);
    RSPaintFilterCanvas cachePaintFilterCanvas(&cacheCanvas);
    auto enabledParams = CreateRenderParams(true);
    auto disabledParams = CreateRenderParams(false);

    DrawableV2::RSLayerPartDrawCacheHelper::LayerPartRenderClipDirtyRegion(*drawable, *disabledParams,
        cachePaintFilterCanvas);
    EXPECT_TRUE(cachePaintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());

    DrawableV2::RSLayerPartDrawCacheHelper::LayerPartRenderClipDirtyRegion(*drawable, *enabledParams,
        cachePaintFilterCanvas);
    EXPECT_TRUE(cachePaintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());

    DrawableV2::RSLayerPartDrawCacheHelper::ResetUpdateLayerPartRenderCache(*drawable);
    DrawableV2::RSLayerPartDrawCacheHelper::PushLayerPartRenderDirtyRegion(*drawable, *enabledParams,
        curPaintFilterCanvas, cachePaintFilterCanvas, TEST_NODE_COUNT);
    DrawableV2::RSLayerPartDrawCacheHelper::LayerPartRenderClipDirtyRegion(*drawable, *enabledParams,
        cachePaintFilterCanvas);
    EXPECT_FALSE(cachePaintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());
}

/**
 * @tc.name: HelperPopLayerPartRenderDirtyRegionBranches
 * @tc.desc: Verify helper pop branches: disabled, empty stack, null cache, and valid pop
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSLayerPartDrawCacheHelperTest, HelperPopLayerPartRenderDirtyRegionBranches, TestSize.Level1)
{
    auto drawableNoCache = CreateDrawable();
    ASSERT_NE(drawableNoCache, nullptr);
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    auto enabledParams = CreateRenderParams(true);
    auto disabledParams = CreateRenderParams(false);

    DrawableV2::RSLayerPartDrawCacheHelper::PopLayerPartRenderDirtyRegion(*drawableNoCache,
        *disabledParams, paintFilterCanvas);
    EXPECT_TRUE(paintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());

    DrawableV2::RSLayerPartDrawCacheHelper::PopLayerPartRenderDirtyRegion(*drawableNoCache,
        *enabledParams, paintFilterCanvas);
    EXPECT_TRUE(paintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());

    Drawing::Region dirtyRegion;
    dirtyRegion.SetRect(Drawing::RectI(DEFAULT_DIRTY_RECT.GetLeft(), DEFAULT_DIRTY_RECT.GetTop(),
        DEFAULT_DIRTY_RECT.GetWidth(), DEFAULT_DIRTY_RECT.GetHeight()));
    paintFilterCanvas.PushLayerPartRenderDirtyRegion(dirtyRegion);
    EXPECT_FALSE(paintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());

    DrawableV2::RSLayerPartDrawCacheHelper::PopLayerPartRenderDirtyRegion(*drawableNoCache,
        *enabledParams, paintFilterCanvas);
    EXPECT_FALSE(paintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());

    auto drawableWithCache = CreateDrawable();
    ASSERT_NE(drawableWithCache, nullptr);
    Drawing::Canvas curCanvas;
    Drawing::Canvas cacheCanvas;
    RSPaintFilterCanvas curPaintFilterCanvas(&curCanvas);
    RSPaintFilterCanvas cachePaintFilterCanvas(&cacheCanvas);

    DrawableV2::RSLayerPartDrawCacheHelper::ResetUpdateLayerPartRenderCache(*drawableWithCache);
    DrawableV2::RSLayerPartDrawCacheHelper::PushLayerPartRenderDirtyRegion(*drawableWithCache,
        *enabledParams, curPaintFilterCanvas, cachePaintFilterCanvas, TEST_NODE_COUNT);
    ASSERT_FALSE(cachePaintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());
    DrawableV2::RSLayerPartDrawCacheHelper::PopLayerPartRenderDirtyRegion(*drawableWithCache,
        *enabledParams, cachePaintFilterCanvas);
    EXPECT_TRUE(cachePaintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());
}

/**
 * @tc.name: HelperResetUpdateLayerPartRenderCacheCreatesCache
 * @tc.desc: Verify helper reset creates cache and resets warmup
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSLayerPartDrawCacheHelperTest, HelperResetUpdateLayerPartRenderCacheCreatesCache, TestSize.Level1)
{
    auto drawable = CreateDrawable();
    ASSERT_NE(drawable, nullptr);
    EXPECT_EQ(drawable->TryGetLayerPartDrawCachePtr(), nullptr);

    DrawableV2::RSLayerPartDrawCacheHelper::ResetUpdateLayerPartRenderCache(*drawable);
    EXPECT_NE(drawable->TryGetLayerPartDrawCachePtr(), nullptr);
}

/**
 * @tc.name: HelperResetUpdateLayerPartRenderCacheResetsWarmup
 * @tc.desc: Verify helper reset restores warmup branch after partial-dirty branch has been used
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSLayerPartDrawCacheHelperTest, HelperResetUpdateLayerPartRenderCacheResetsWarmup, TestSize.Level1)
{
    auto drawable = CreateDrawable();
    ASSERT_NE(drawable, nullptr);
    Drawing::Canvas curCanvas;
    Drawing::Canvas cacheCanvas;
    RSPaintFilterCanvas curPaintFilterCanvas(&curCanvas);
    RSPaintFilterCanvas cachePaintFilterCanvas(&cacheCanvas);
    auto params = CreateRenderParams(true);
    params->SetLayerPartRenderCurrentFrameDirtyRegion(PARTIAL_DIRTY_RECT);

    DrawableV2::RSLayerPartDrawCacheHelper::ResetUpdateLayerPartRenderCache(*drawable);
    for (int32_t i = 0; i <= WARMUP_PUSH_COUNT; ++i) {
        DrawableV2::RSLayerPartDrawCacheHelper::PushLayerPartRenderDirtyRegion(*drawable,
            *params, curPaintFilterCanvas, cachePaintFilterCanvas, TEST_NODE_COUNT);
        ASSERT_FALSE(cachePaintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());
        DrawableV2::RSLayerPartDrawCacheHelper::PopLayerPartRenderDirtyRegion(*drawable,
            *params, cachePaintFilterCanvas);
    }

    DrawableV2::RSLayerPartDrawCacheHelper::PushLayerPartRenderDirtyRegion(*drawable,
        *params, curPaintFilterCanvas, cachePaintFilterCanvas, TEST_NODE_COUNT);
    ASSERT_FALSE(cachePaintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());
    auto switchedBounds = cachePaintFilterCanvas.GetCurLayerPartRenderDirtyRegion().GetBounds();
    EXPECT_EQ(switchedBounds.GetLeft(), PARTIAL_CACHE_REGION_LEFT);
    EXPECT_EQ(switchedBounds.GetTop(), PARTIAL_CACHE_REGION_TOP);
    EXPECT_EQ(switchedBounds.GetWidth(), PARTIAL_CACHE_REGION_WIDTH);
    EXPECT_EQ(switchedBounds.GetHeight(), PARTIAL_CACHE_REGION_HEIGHT);
    DrawableV2::RSLayerPartDrawCacheHelper::PopLayerPartRenderDirtyRegion(*drawable,
        *params, cachePaintFilterCanvas);

    DrawableV2::RSLayerPartDrawCacheHelper::ResetUpdateLayerPartRenderCache(*drawable);
    DrawableV2::RSLayerPartDrawCacheHelper::PushLayerPartRenderDirtyRegion(*drawable,
        *params, curPaintFilterCanvas, cachePaintFilterCanvas, TEST_NODE_COUNT);
    ASSERT_FALSE(cachePaintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());
    auto resetBounds = cachePaintFilterCanvas.GetCurLayerPartRenderDirtyRegion().GetBounds();
    EXPECT_EQ(resetBounds.GetLeft(), FULL_CACHE_REGION_LEFT);
    EXPECT_EQ(resetBounds.GetTop(), FULL_CACHE_REGION_TOP);
    EXPECT_EQ(resetBounds.GetWidth(), FULL_CACHE_REGION_WIDTH);
    EXPECT_EQ(resetBounds.GetHeight(), FULL_CACHE_REGION_HEIGHT);
}

/**
 * @tc.name: HelperPushLayerPartRenderDirtyRegionDisabledAfterCacheCreated
 * @tc.desc: Verify disabled branch still short-circuits after cache has been created
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSLayerPartDrawCacheHelperTest, HelperPushLayerPartRenderDirtyRegionDisabledAfterCacheCreated,
    TestSize.Level1)
{
    auto drawable = CreateDrawable();
    ASSERT_NE(drawable, nullptr);
    Drawing::Canvas curCanvas;
    Drawing::Canvas cacheCanvas;
    RSPaintFilterCanvas curPaintFilterCanvas(&curCanvas);
    RSPaintFilterCanvas cachePaintFilterCanvas(&cacheCanvas);
    auto enabledParams = CreateRenderParams(true);
    auto disabledParams = CreateRenderParams(false);

    DrawableV2::RSLayerPartDrawCacheHelper::ResetUpdateLayerPartRenderCache(*drawable);
    DrawableV2::RSLayerPartDrawCacheHelper::PushLayerPartRenderDirtyRegion(*drawable,
        *enabledParams, curPaintFilterCanvas, cachePaintFilterCanvas, TEST_NODE_COUNT);
    ASSERT_FALSE(cachePaintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());
    DrawableV2::RSLayerPartDrawCacheHelper::PopLayerPartRenderDirtyRegion(*drawable,
        *enabledParams, cachePaintFilterCanvas);
    ASSERT_TRUE(cachePaintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());

    DrawableV2::RSLayerPartDrawCacheHelper::PushLayerPartRenderDirtyRegion(*drawable,
        *disabledParams, curPaintFilterCanvas, cachePaintFilterCanvas, TEST_NODE_COUNT);
    EXPECT_TRUE(cachePaintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());
}

/**
 * @tc.name: HelperLayerPartRenderClipDirtyRegionDisabledAfterCacheCreated
 * @tc.desc: Verify clip disabled branch short-circuits even after cache has been created
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSLayerPartDrawCacheHelperTest, HelperLayerPartRenderClipDirtyRegionDisabledAfterCacheCreated,
    TestSize.Level1)
{
    auto drawable = CreateDrawable();
    ASSERT_NE(drawable, nullptr);
    Drawing::Canvas curCanvas;
    Drawing::Canvas cacheCanvas;
    RSPaintFilterCanvas curPaintFilterCanvas(&curCanvas);
    RSPaintFilterCanvas cachePaintFilterCanvas(&cacheCanvas);
    auto enabledParams = CreateRenderParams(true);
    auto disabledParams = CreateRenderParams(false);

    DrawableV2::RSLayerPartDrawCacheHelper::ResetUpdateLayerPartRenderCache(*drawable);
    DrawableV2::RSLayerPartDrawCacheHelper::PushLayerPartRenderDirtyRegion(*drawable,
        *enabledParams, curPaintFilterCanvas, cachePaintFilterCanvas, TEST_NODE_COUNT);
    ASSERT_FALSE(cachePaintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());

    DrawableV2::RSLayerPartDrawCacheHelper::LayerPartRenderClipDirtyRegion(*drawable,
        *disabledParams, cachePaintFilterCanvas);
    EXPECT_FALSE(cachePaintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());
}

/**
 * @tc.name: HelperPopLayerPartRenderDirtyRegionEmptyStackAfterCacheCreated
 * @tc.desc: Verify pop empty-stack branch short-circuits even when cache exists
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSLayerPartDrawCacheHelperTest, HelperPopLayerPartRenderDirtyRegionEmptyStackAfterCacheCreated,
    TestSize.Level1)
{
    auto drawable = CreateDrawable();
    ASSERT_NE(drawable, nullptr);
    Drawing::Canvas curCanvas;
    Drawing::Canvas cacheCanvas;
    RSPaintFilterCanvas curPaintFilterCanvas(&curCanvas);
    RSPaintFilterCanvas cachePaintFilterCanvas(&cacheCanvas);
    auto enabledParams = CreateRenderParams(true);

    DrawableV2::RSLayerPartDrawCacheHelper::ResetUpdateLayerPartRenderCache(*drawable);
    ASSERT_NE(drawable->TryGetLayerPartDrawCachePtr(), nullptr);

    DrawableV2::RSLayerPartDrawCacheHelper::PopLayerPartRenderDirtyRegion(*drawable,
        *enabledParams, cachePaintFilterCanvas);
    EXPECT_TRUE(cachePaintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());
    EXPECT_NE(drawable->TryGetLayerPartDrawCachePtr(), nullptr);

    DrawableV2::RSLayerPartDrawCacheHelper::PushLayerPartRenderDirtyRegion(*drawable,
        *enabledParams, curPaintFilterCanvas, cachePaintFilterCanvas, TEST_NODE_COUNT);
    ASSERT_FALSE(cachePaintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());
    DrawableV2::RSLayerPartDrawCacheHelper::PopLayerPartRenderDirtyRegion(*drawable,
        *enabledParams, cachePaintFilterCanvas);
    EXPECT_TRUE(cachePaintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());
}

/**
 * @tc.name: HelperNoCacheShortCircuitBranches
 * @tc.desc: Verify helper short-circuits when cache pointer is null for push/clip/pop
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSLayerPartDrawCacheHelperTest, HelperNoCacheShortCircuitBranches, TestSize.Level1)
{
    auto drawable = CreateDrawable();
    ASSERT_NE(drawable, nullptr);
    ASSERT_EQ(drawable->TryGetLayerPartDrawCachePtr(), nullptr);

    Drawing::Canvas curCanvas;
    Drawing::Canvas cacheCanvas;
    RSPaintFilterCanvas curPaintFilterCanvas(&curCanvas);
    RSPaintFilterCanvas cachePaintFilterCanvas(&cacheCanvas);
    auto enabledParams = CreateRenderParams(true);

    DrawableV2::RSLayerPartDrawCacheHelper::PushLayerPartRenderDirtyRegion(*drawable,
        *enabledParams, curPaintFilterCanvas, cachePaintFilterCanvas, TEST_NODE_COUNT);
    EXPECT_EQ(drawable->TryGetLayerPartDrawCachePtr(), nullptr);
    EXPECT_TRUE(cachePaintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());

    DrawableV2::RSLayerPartDrawCacheHelper::LayerPartRenderClipDirtyRegion(*drawable,
        *enabledParams, cachePaintFilterCanvas);
    EXPECT_EQ(drawable->TryGetLayerPartDrawCachePtr(), nullptr);

    Drawing::Region dirtyRegion;
    dirtyRegion.SetRect(Drawing::RectI(DEFAULT_DIRTY_RECT.GetLeft(), DEFAULT_DIRTY_RECT.GetTop(),
        DEFAULT_DIRTY_RECT.GetWidth(), DEFAULT_DIRTY_RECT.GetHeight()));
    cachePaintFilterCanvas.PushLayerPartRenderDirtyRegion(dirtyRegion);
    EXPECT_FALSE(cachePaintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());

    DrawableV2::RSLayerPartDrawCacheHelper::PopLayerPartRenderDirtyRegion(*drawable,
        *enabledParams, cachePaintFilterCanvas);
    EXPECT_EQ(drawable->TryGetLayerPartDrawCachePtr(), nullptr);
    EXPECT_FALSE(cachePaintFilterCanvas.IsLayerPartRenderDirtyRegionStackEmpty());
}
} // namespace OHOS::Rosen
