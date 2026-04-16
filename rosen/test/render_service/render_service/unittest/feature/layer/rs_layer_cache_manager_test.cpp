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

#include "feature/layer/rs_layer_cache_manager.h"

#include "gtest/gtest.h"

#include "common/rs_common_def.h"
#include "common/rs_obj_abs_geometry.h"
#include "dirty_region/rs_optimize_canvas_dirty_collector.h"
#include "drawable/rs_color_picker_drawable.h"
#include "drawable/rs_property_drawable.h"
#include "drawable/rs_property_drawable_background.h"
#include "drawable/rs_property_drawable_foreground.h"
#include "feature/opinc/rs_opinc_manager.h"
#include "modifier_ng/custom/rs_custom_modifier.h"
#include "offscreen_render/rs_offscreen_render_thread.h"
#include "params/rs_render_params.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "pipeline/render_thread/rs_uni_render_util.h"
#include "pipeline/rs_canvas_drawing_render_node.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_dirty_region_manager.h"
#include "pipeline/rs_logical_display_render_node.h"
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_screen_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "render/rs_filter.h"
#include "skia_adapter/skia_canvas.h"
#include "parameters.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
constexpr NodeId DEFAULT_ID = 10086;

class RSLayerCacheManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline NodeId id = DEFAULT_ID;
};

void RSLayerCacheManagerTest::SetUpTestCase() {}
void RSLayerCacheManagerTest::TearDownTestCase() {}
void RSLayerCacheManagerTest::SetUp() {}
void RSLayerCacheManagerTest::TearDown() {}

/**
 * @tc.name: InstanceTest
 * @tc.desc: Test Instance
 * @tc.type: FUNC
 * @tc.require: issues/22969
 */
HWTEST_F(RSLayerCacheManagerTest, InstanceTest, TestSize.Level1)
{
    auto& layerCacheManager = RSLayerCacheManager::Instance();
    EXPECT_TRUE(layerCacheManager.layerDrawables_.empty());
}

/**
 * @tc.name: HandleLayerDrawablesTest
 * @tc.desc: Test HandleLayerDrawables
 * @tc.type: FUNC
 * @tc.require: issues/22969
 */
HWTEST_F(RSLayerCacheManagerTest, HandleLayerDrawablesTest, TestSize.Level1)
{
    auto& layerCacheManager = RSLayerCacheManager::Instance();
    EXPECT_TRUE(layerCacheManager.layerDrawables_.empty());
    Drawing::Canvas canvas;
    layerCacheManager.HandleLayerDrawables(canvas);
    NodeId nodeId = 0;
    auto node = std::make_shared<RSRenderNode>(nodeId);
    auto drawable = std::make_shared<DrawableV2::RSCanvasRenderNodeDrawable>(std::move(node));

    drawable->GetRenderParams()->isOpincSuggestFlag_ = false;
    drawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    layerCacheManager.layerDrawables_.emplace_back(drawable);
    layerCacheManager.HandleLayerDrawables(canvas);
    drawable->renderParams_ = nullptr;
    layerCacheManager.layerDrawables_.emplace_back(drawable);
    layerCacheManager.HandleLayerDrawables(canvas);
}

/**
 * @tc.name: TryPrepareLayerCacheTest
 * @tc.desc: Test If TryPrepareLayerCache Can Run
 * @tc.type: FUNC
 * @tc.require: issues/22969
 */
HWTEST(RSLayerCacheManagerTest, TryPrepareLayerCacheTest, TestSize.Level1)
{
    NodeId nodeId = 0;
    auto node = std::make_shared<RSRenderNode>(nodeId);
    auto drawable = std::make_shared<DrawableV2::RSCanvasRenderNodeDrawable>(std::move(node));
    ASSERT_NE(drawable, nullptr);
    Drawing::Canvas canvas;
    drawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    auto& layerCacheManager = RSLayerCacheManager::Instance();
    layerCacheManager.TryPrepareLayerCache(drawable, canvas);
    ASSERT_TRUE(drawable->GetRenderParams());

    DrawableV2::RSOpincDrawCache::SetOpincBlockNodeSkip(false);
    drawable->renderParams_->isOpincStateChanged_ = false;
    drawable->renderParams_->startingWindowFlag_ = false;
    layerCacheManager.TryPrepareLayerCache(drawable, canvas);
    ASSERT_TRUE(drawable->GetRenderParams());

    drawable->isOpDropped_ = false;
    drawable->isDrawingCacheEnabled_ = true;
    RSOpincManager::Instance().SetOPIncSwitch(false);
    drawable->SetDrawBlurForCache(false);
    layerCacheManager.TryPrepareLayerCache(drawable, canvas);
    ASSERT_TRUE(drawable->isDrawingCacheEnabled_);

    drawable->SetDrawBlurForCache(true);
    layerCacheManager.TryPrepareLayerCache(drawable, canvas);
    ASSERT_TRUE(drawable->isDrawingCacheEnabled_);
}

/**
 * @tc.name: TryPrepareLayerCacheTest001
 * @tc.desc: Test TryPrepareLayerCache
 * @tc.type: FUNC
 * @tc.require: issues/22969
 */
HWTEST(RSLayerCacheManagerTest, TryPrepareLayerCacheTest001, TestSize.Level1)
{
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(0);
    auto canvasDrawable = std::make_shared<DrawableV2::RSCanvasRenderNodeDrawable>(std::move(canvasNode));
    ASSERT_NE(canvasDrawable, nullptr);
    canvasDrawable->renderParams_ = std::make_unique<RSRenderParams>(1);
    canvasDrawable->renderParams_->shouldPaint_ = true;
    canvasDrawable->renderParams_->contentEmpty_ = false;
    canvasDrawable->renderParams_->startingWindowFlag_ = false;
    canvasDrawable->SetOcclusionCullingEnabled(false);
    auto drawingCanvas = std::make_unique<Drawing::Canvas>(1, 1);
    ASSERT_TRUE(drawingCanvas);
    auto canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    ASSERT_TRUE(canvas);
    auto& layerCacheManager = RSLayerCacheManager::Instance();
    layerCacheManager.TryPrepareLayerCache(canvasDrawable, *canvas);
    ASSERT_NE(canvasDrawable->renderParams_, nullptr);
    canvas->SetSubTreeParallelState(RSPaintFilterCanvas::SubTreeStatus::SUBTREE_QUICK_DRAW_STATE);
    layerCacheManager.TryPrepareLayerCache(canvasDrawable, *canvas);
    ASSERT_NE(canvasDrawable->renderParams_, nullptr);
}

/**
 * @tc.name: TryPrepareLayerCacheTest002
 * @tc.desc: Test TryPrepareLayerCache while skip draw by white list
 * @tc.type: FUNC
 * @tc.require: issues/22969
 */
HWTEST(RSLayerCacheManagerTest, TryPrepareLayerCacheTest002, TestSize.Level2)
{
    NodeId nodeId = 0;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(nodeId);
    auto canvasDrawable = std::make_shared<DrawableV2::RSCanvasRenderNodeDrawable>(std::move(canvasNode));
    ASSERT_NE(canvasDrawable, nullptr);

    auto params = std::make_unique<RSRenderThreadParams>();
    RSUniRenderThread::Instance().Sync(std::move(params));
    RSUniRenderThread::captureParam_.isMirror_ = true;
    std::unordered_set<NodeId> whiteList = {nodeId};
    RSUniRenderThread::Instance().SetWhiteList(whiteList);

    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    AutoSpecialLayerStateRecover whiteListRecover(INVALID_NODEID);
    auto& layerCacheManager = RSLayerCacheManager::Instance();
    layerCacheManager.TryPrepareLayerCache(canvasDrawable, canvas);

    // restore
    RSUniRenderThread::Instance().Sync(std::make_unique<RSRenderThreadParams>());
}

/**
 * @tc.name: TryPrepareLayerCache003
 * @tc.desc: Test TryPrepareLayerCache while isn't security display
 * @tc.type: FUNC
 * @tc.require: issues/22969
 */
HWTEST(RSLayerCacheManagerTest, TryPrepareLayerCacheTest003, TestSize.Level2)
{
    NodeId nodeId = 0;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(nodeId);
    auto canvasDrawable = std::make_shared<DrawableV2::RSCanvasRenderNodeDrawable>(std::move(canvasNode));
    ASSERT_NE(canvasDrawable, nullptr);
    canvasDrawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    canvasDrawable->renderParams_->shouldPaint_ = true;
    canvasDrawable->renderParams_->startingWindowFlag_ = false;

    // set render thread param
    auto uniParams = std::make_unique<RSRenderThreadParams>();
    uniParams->SetSecurityDisplay(false);
    RSUniRenderThread::Instance().Sync(std::move(uniParams));
    RSUniRenderThread::Instance().SetWhiteList({});

    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    auto& layerCacheManager = RSLayerCacheManager::Instance();
    layerCacheManager.TryPrepareLayerCache(canvasDrawable, canvas);
    ASSERT_FALSE(canvasDrawable->SkipDrawByWhiteList(canvas));

    // restore
    RSUniRenderThread::Instance().Sync(std::make_unique<RSRenderThreadParams>());
}

/**
 * @tc.name: TryPrepareLayerCache004
 * @tc.desc: Test TryPrepareLayerCache while renderThreadParams_ is nullptr
 * @tc.type: FUNC
 * @tc.require: issues/22969
 */
HWTEST(RSLayerCacheManagerTest, TryPrepareLayerCacheTest004, TestSize.Level2)
{
    NodeId nodeId = 0;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(nodeId);
    auto canvasDrawable = std::make_shared<DrawableV2::RSCanvasRenderNodeDrawable>(std::move(canvasNode));
    ASSERT_NE(canvasDrawable, nullptr);
    canvasDrawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    canvasDrawable->renderParams_->shouldPaint_ = true;
    canvasDrawable->renderParams_->startingWindowFlag_ = false;

    // set render thread param
    RSUniRenderThread::Instance().Sync(nullptr);
    RSUniRenderThread::Instance().SetWhiteList({});

    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    canvasDrawable->renderParams_->UpdateHDRStatus(HdrStatus::HDR_EFFECT, true);
    auto& layerCacheManager = RSLayerCacheManager::Instance();
    layerCacheManager.TryPrepareLayerCache(canvasDrawable, canvas);
    ASSERT_FALSE(canvasDrawable->SkipDrawByWhiteList(canvas));

    // restore
    RSUniRenderThread::Instance().Sync(std::make_unique<RSRenderThreadParams>());
}

/**
 * @tc.name: TryPrepareLayerCache005
 * @tc.desc: Test TryPrepareLayerCache while isDrawingCacheEnabled_ is true
 * @tc.type: FUNC
 * @tc.require: issues/22969
 */
HWTEST(RSLayerCacheManagerTest, TryPrepareLayerCacheTest005, TestSize.Level2)
{
    NodeId nodeId = 0;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(nodeId);
    auto canvasDrawable = std::make_shared<DrawableV2::RSCanvasRenderNodeDrawable>(std::move(canvasNode));
    ASSERT_NE(canvasDrawable, nullptr);
    canvasDrawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    canvasDrawable->renderParams_->shouldPaint_ = true;
    canvasDrawable->renderParams_->startingWindowFlag_ = false;

    canvasDrawable->SetOcclusionCullingEnabled(false);
    DrawableV2::RSRenderNodeDrawable::InitDfxForCacheInfo();

    // set render thread param
    auto uniParams = std::make_unique<RSRenderThreadParams>();
    uniParams->SetSecurityDisplay(false);
    RSUniRenderThread::Instance().Sync(std::move(uniParams));

    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    auto& layerCacheManager = RSLayerCacheManager::Instance();
    layerCacheManager.TryPrepareLayerCache(canvasDrawable, canvas);
    EXPECT_EQ(canvasDrawable->GetOpincDrawCache().GetNodeCacheType(), NodeStrategyType::CACHE_NONE);

    // restore
    RSUniRenderThread::Instance().Sync(std::make_unique<RSRenderThreadParams>());
}
} // namespace Rosen
} // namespace OHOS