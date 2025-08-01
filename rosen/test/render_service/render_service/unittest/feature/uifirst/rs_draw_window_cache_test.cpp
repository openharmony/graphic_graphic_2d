/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "drawable/rs_surface_render_node_drawable.h"
#include "feature/uifirst/rs_draw_window_cache.h"
#include "params/rs_render_thread_params.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_test_util.h"
#include "pipeline/rs_surface_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSDrawWindowCacheTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSDrawWindowCacheTest::SetUpTestCase() {}
void RSDrawWindowCacheTest::TearDownTestCase() {}
void RSDrawWindowCacheTest::SetUp() {}
void RSDrawWindowCacheTest::TearDown() {}

/**
 * @tc.name: HasCache
 * @tc.desc: Test HasCache
 * @tc.type: FUNC
 * @tc.require: issueIAVLLE
 */
HWTEST_F(RSDrawWindowCacheTest, HasCache, TestSize.Level1)
{
    RSDrawWindowCache drawWindowCache;
    ASSERT_FALSE(drawWindowCache.HasCache());

    drawWindowCache.image_ = std::make_shared<Drawing::Image>();
    ASSERT_TRUE(drawWindowCache.HasCache());
}

/**
 * @tc.name: ClearCache
 * @tc.desc: Test ClearCache
 * @tc.type: FUNC
 * @tc.require: issueIAVLLE
 */
HWTEST_F(RSDrawWindowCacheTest, ClearCache, TestSize.Level1)
{
    RSDrawWindowCache drawWindowCache;
    drawWindowCache.image_ = std::make_shared<Drawing::Image>();
    drawWindowCache.ClearCache();
    ASSERT_FALSE(drawWindowCache.HasCache());
}

/**
 * @tc.name: DealWithCachedWindow001
 * @tc.desc: Test DealWithCachedWindow
 * @tc.type: FUNC
 * @tc.require: issueIAVLLE
 */
HWTEST_F(RSDrawWindowCacheTest, DealWithCachedWindow001, TestSize.Level1)
{
    RSDrawWindowCache drawWindowCache;
    DrawableV2::RSSurfaceRenderNodeDrawable* surfaceDrawable = nullptr;
    RSSurfaceRenderParams surfaceParams(1);
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    auto uniParams = std::make_shared<RSRenderThreadParams>();
    ASSERT_NE(uniParams, nullptr);
    ASSERT_FALSE(drawWindowCache.DealWithCachedWindow(surfaceDrawable,
        canvas, surfaceParams, *uniParams));

    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(1);
    ASSERT_NE(surfaceNode, nullptr);
    auto drawable = DrawableV2::RSSurfaceRenderNodeDrawable::OnGenerate(surfaceNode);
    ASSERT_NE(drawable, nullptr);
    surfaceDrawable = static_cast<DrawableV2::RSSurfaceRenderNodeDrawable*>(drawable);
    ASSERT_NE(surfaceDrawable, nullptr);

    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    int32_t width = 100;
    int32_t height = 30;
    bmp.Build(width, height, format);
    bmp.ClearWithColor(Drawing::Color::COLOR_RED);
    drawWindowCache.image_ = bmp.MakeImage();
    surfaceParams.SetUifirstNodeEnableParam(MultiThreadCacheType::NONFOCUS_WINDOW);
    ASSERT_TRUE(drawWindowCache.DealWithCachedWindow(surfaceDrawable, canvas, surfaceParams, *uniParams));
    /* OnGlobalPositionEnabled is true */
    surfaceParams.isGlobalPositionEnabled_ = true;
    ASSERT_TRUE(drawWindowCache.DealWithCachedWindow(surfaceDrawable, canvas, surfaceParams, *uniParams));
    // To set matrix is singular matrix
    surfaceParams.matrix_.SetMatrix(1, 2, 3, 2, 4, 6, 3, 6, 9);
    ASSERT_TRUE(drawWindowCache.DealWithCachedWindow(surfaceDrawable, canvas, surfaceParams, *uniParams));

    // test cross-node dfx
    surfaceParams.isCrossNode_ = true;
    uniParams->isCrossNodeOffscreenOn_ = CrossNodeOffScreenRenderDebugType::ENABLE_DFX;
    ASSERT_TRUE(drawWindowCache.DealWithCachedWindow(surfaceDrawable, canvas, surfaceParams, *uniParams));

    uniParams->isCrossNodeOffscreenOn_ = CrossNodeOffScreenRenderDebugType::DISABLED;
    ASSERT_TRUE(drawWindowCache.DealWithCachedWindow(surfaceDrawable, canvas, surfaceParams, *uniParams));
}

/**
 * @tc.name: DealWithCachedWindow002
 * @tc.desc: Test DealWithCachedWindow
 * @tc.type: FUNC
 * @tc.require: issueICCSTG
 */
HWTEST_F(RSDrawWindowCacheTest, DealWithCachedWindow002, TestSize.Level1)
{
    RSDrawWindowCache drawWindowCache;
    drawWindowCache.image_ = std::make_shared<Drawing::Image>();
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    RSRenderThreadParams uniParams;

    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(1);
    auto surfaceDrawable = static_cast<DrawableV2::RSSurfaceRenderNodeDrawable*>(
        DrawableV2::RSSurfaceRenderNodeDrawable::OnGenerate(surfaceNode));
    ASSERT_NE(surfaceDrawable, nullptr);

    // test non-cross-node should clear cache when uifirst disabled
    RSSurfaceRenderParams surfaceParams(1);
    surfaceParams.isCrossNode_ = false;
    surfaceParams.SetIsCloned(false);
    surfaceParams.SetUifirstNodeEnableParam(MultiThreadCacheType::NONE);

    surfaceParams.SetNeedCacheSurface(true);
    // not clear cache
    drawWindowCache.DealWithCachedWindow(surfaceDrawable, canvas, surfaceParams, uniParams);
    ASSERT_TRUE(drawWindowCache.image_ != nullptr);

    surfaceParams.SetNeedCacheSurface(false);
    // clear cache
    drawWindowCache.DealWithCachedWindow(surfaceDrawable, canvas, surfaceParams, uniParams);
    ASSERT_TRUE(drawWindowCache.image_ == nullptr);

    // test cross-node don't reuse cache
    surfaceParams.isCrossNode_ = true;
    uniParams.SetIsMirrorScreen(true);
    uniParams.hasDisplayHdrOn_ = false;
    uniParams.SetIsFirstVisitCrossNodeDisplay(false);
    ASSERT_FALSE(drawWindowCache.DealWithCachedWindow(surfaceDrawable, canvas, surfaceParams, uniParams));

    // test cross-node don't reuse cache
    surfaceParams.isCrossNode_ = true;
    uniParams.SetIsMirrorScreen(false);
    uniParams.hasDisplayHdrOn_ = true;
    uniParams.SetIsFirstVisitCrossNodeDisplay(false);
    ASSERT_FALSE(drawWindowCache.DealWithCachedWindow(surfaceDrawable, canvas, surfaceParams, uniParams));

    // test cross-node don't reuse cache
    surfaceParams.isCrossNode_ = true;
    uniParams.SetIsMirrorScreen(false);
    uniParams.hasDisplayHdrOn_ = false;
    uniParams.SetIsFirstVisitCrossNodeDisplay(true);
    ASSERT_FALSE(drawWindowCache.DealWithCachedWindow(surfaceDrawable, canvas, surfaceParams, uniParams));

    // test invalid cache size
    surfaceParams.isCrossNode_ = true;
    uniParams.SetIsMirrorScreen(false);
    uniParams.hasDisplayHdrOn_ = false;
    uniParams.SetIsFirstVisitCrossNodeDisplay(false);
    drawWindowCache.image_ = std::make_shared<Drawing::Image>();
    ASSERT_FALSE(drawWindowCache.DealWithCachedWindow(surfaceDrawable, canvas, surfaceParams, uniParams));
}

/**
 * @tc.name: DrawAndCacheWindowContent
 * @tc.desc: Test DrawAndCacheWindowContent
 * @tc.type: FUNC
 * @tc.require: issueIAVLLE
 */
HWTEST_F(RSDrawWindowCacheTest, DrawAndCacheWindowContent, TestSize.Level1)
{
    RSDrawWindowCache drawWindowCache;
    DrawableV2::RSSurfaceRenderNodeDrawable* surfaceDrawable = nullptr;
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    int32_t canvasSize = 100;
    Drawing::Rect bounds = {0, 0, canvasSize, canvasSize};
    drawWindowCache.DrawAndCacheWindowContent(surfaceDrawable, canvas, bounds);
    ASSERT_FALSE(drawWindowCache.HasCache());

    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(1);
    ASSERT_NE(surfaceNode, nullptr);
    auto drawable = DrawableV2::RSSurfaceRenderNodeDrawable::OnGenerate(surfaceNode);
    ASSERT_NE(drawable, nullptr);
    surfaceDrawable = static_cast<DrawableV2::RSSurfaceRenderNodeDrawable*>(drawable);
    ASSERT_NE(surfaceDrawable, nullptr);

    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    int32_t width = 100;
    int32_t height = 30;
    bmp.Build(width, height, format);
    bmp.ClearWithColor(Drawing::Color::COLOR_RED);
    drawWindowCache.image_ = bmp.MakeImage();
    drawWindowCache.DrawAndCacheWindowContent(surfaceDrawable, canvas, bounds);
    ASSERT_TRUE(drawWindowCache.HasCache());
}

/**
 * @tc.name: DrawCrossNodeOffscreenDFX
 * @tc.desc: Test DrawCrossNodeOffscreenDFX
 * @tc.type: FUNC
 * @tc.require: issueIB6QW7
 */
HWTEST_F(RSDrawWindowCacheTest, DrawCrossNodeOffscreenDFX, TestSize.Level1)
{
    RSDrawWindowCache drawWindowCache;
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    RSSurfaceRenderParams surfaceParams(1);
    surfaceParams.isCrossNode_ = true;
    ASSERT_TRUE(surfaceParams.IsCrossNode());
    RSRenderThreadParams uniParams;
    uniParams.isCrossNodeOffscreenOn_ = CrossNodeOffScreenRenderDebugType::ENABLE_DFX;
    // rgba: Alpha 128, green 128, blue 128
    Drawing::Color color(0, 128, 128, 128);
    drawWindowCache.image_ = std::make_shared<Drawing::Image>();
    drawWindowCache.DrawCrossNodeOffscreenDFX(canvas, surfaceParams, uniParams, color);
}
}
