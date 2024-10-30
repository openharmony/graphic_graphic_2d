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
#include "rs_test_util.h"

#include "drawable/rs_surface_render_node_drawable.h"
#include "params/rs_render_thread_params.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_draw_window_cache.h"
#include "pipeline/rs_surface_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RsDrawWindowCacheTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RsDrawWindowCacheTest::SetUpTestCase() {}
void RsDrawWindowCacheTest::TearDownTestCase() {}
void RsDrawWindowCacheTest::SetUp() {}
void RsDrawWindowCacheTest::TearDown() {}

/**
 * @tc.name: HasCache01
 * @tc.desc: Test HasCache
 * @tc.type: FUNC
 * @tc.require: issueIAVLLE
 */
HWTEST_F(RsDrawWindowCacheTest, HasCache01, TestSize.Level1)
{
    RSDrawWindowCache drawWindowCache;
    ASSERT_FALSE(drawWindowCache.HasCache());

    drawWindowCache.image_ = std::make_shared<Drawing::Image>();
    ASSERT_TRUE(drawWindowCache.HasCache());
}

/**
 * @tc.name: DrawAndCacheWindowContent01
 * @tc.desc: Test DrawAndCacheWindowContent
 * @tc.type: FUNC
 * @tc.require: issueIAVLLE
 */
HWTEST_F(RsDrawWindowCacheTest, DrawAndCacheWindowContent01, TestSize.Level1)
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
    surfaceDrawable->surfaceHandlerUiFirst_ = std::make_shared<RSSurfaceHandler>(1);

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
 * @tc.name: ClearCache01
 * @tc.desc: Test ClearCache
 * @tc.type: FUNC
 * @tc.require: issueIAVLLE
 */
HWTEST_F(RsDrawWindowCacheTest, ClearCache01, TestSize.Level1)
{
    RSDrawWindowCache drawWindowCache;
    drawWindowCache.image_ = std::make_shared<Drawing::Image>();
    drawWindowCache.ClearCache();
    ASSERT_FALSE(drawWindowCache.HasCache());
}

/**
 * @tc.name: DealWithCachedWindow01
 * @tc.desc: Test DealWithCachedWindow
 * @tc.type: FUNC
 * @tc.require: issueIAVLLE
 */
HWTEST_F(RsDrawWindowCacheTest, DealWithCachedWindow01, TestSize.Level1)
{
    RSDrawWindowCache drawWindowCache;
    DrawableV2::RSSurfaceRenderNodeDrawable* surfaceDrawable = nullptr;
    RSSurfaceRenderParams surfaceParams(1);
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    ASSERT_FALSE(drawWindowCache.DealWithCachedWindow(surfaceDrawable, canvas, surfaceParams));

    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(1);
    ASSERT_NE(surfaceNode, nullptr);
    auto drawable = DrawableV2::RSSurfaceRenderNodeDrawable::OnGenerate(surfaceNode);
    ASSERT_NE(drawable, nullptr);
    surfaceDrawable = static_cast<DrawableV2::RSSurfaceRenderNodeDrawable*>(drawable);
    ASSERT_NE(surfaceDrawable, nullptr);
    surfaceDrawable->surfaceHandlerUiFirst_ = std::make_shared<RSSurfaceHandler>(1);

    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    int32_t width = 100;
    int32_t height = 30;
    bmp.Build(width, height, format);
    bmp.ClearWithColor(Drawing::Color::COLOR_RED);
    drawWindowCache.image_ = bmp.MakeImage();
    surfaceParams.SetUifirstNodeEnableParam(MultiThreadCacheType::NONFOCUS_WINDOW);
    ASSERT_TRUE(drawWindowCache.DealWithCachedWindow(surfaceDrawable, canvas, surfaceParams));
}
}
