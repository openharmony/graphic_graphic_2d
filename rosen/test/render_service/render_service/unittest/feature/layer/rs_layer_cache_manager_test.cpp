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
#include "dirty_region/rs_optimize_canvas_dirty_collector.h"
#include "feature/layer/rs_layer_cache_manager.h"
#include "feature/opinc/rs_opinc_manager.h"
#include "offscreen_render/rs_offscreen_render_thread.h"
#include "parameters.h"
#include "skia_adapter/skia_canvas.h"

#include "common/rs_common_def.h"
#include "common/rs_obj_abs_geometry.h"
#include "drawable/rs_property_drawable.h"
#include "drawable/rs_property_drawable_background.h"
#include "drawable/rs_property_drawable_foreground.h"
#include "modifier_ng/custom/rs_custom_modifier.h"
#include "params/rs_render_params.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "pipeline/render_thread/rs_uni_render_util.h"
#include "pipeline/rs_canvas_drawing_render_node.h"
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_dirty_region_manager.h"
#include "pipeline/rs_logical_display_render_node.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_screen_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "render/rs_filter.h"
#include "draw/canvas.h"
#include "image/gpu_context.h"

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
 * @tc.name: HandleLayerDrawablesTest001
 * @tc.desc: Test HandleLayerDrawables
 * @tc.type: FUNC
 * @tc.require: issues/22969
 */
HWTEST_F(RSLayerCacheManagerTest, HandleLayerDrawablesTest001, TestSize.Level1)
{
    auto& layerCacheManager = RSLayerCacheManager::Instance();
    EXPECT_TRUE(layerCacheManager.layerDrawables_.empty());

    Drawing::Canvas canvas;
    auto curCanvas = std::make_shared<RSPaintFilterCanvas>(&canvas);

    layerCacheManager.HandleLayerDrawables(*curCanvas);

    NodeId nodeId = 0;
    auto node = std::make_shared<RSCanvasRenderNode>(0);
    auto drawable = std::static_pointer_cast<DrawableV2::RSCanvasRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(node));
    canvas.gpuContext_ = std::make_shared<Drawing::GPUContext>();

    EXPECT_TRUE(curCanvas->GetGPUContext() != nullptr);

    drawable->GetRenderParams()->isOpincSuggestFlag_ = false;
    drawable->isDrawingCacheEnabled_ = false;
    drawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    layerCacheManager.layerDrawables_.emplace_back(drawable);
    layerCacheManager.HandleLayerDrawables(*curCanvas);
    drawable->renderParams_ = nullptr;
    layerCacheManager.layerDrawables_.emplace_back(drawable);
    layerCacheManager.HandleLayerDrawables(*curCanvas);
}

/**
 * @tc.name: TryPrepareLayerCacheTest001
 * @tc.desc: Test TryPrepareLayerCache
 * @tc.type: FUNC
 * @tc.require: issues/22969
 */
HWTEST_F(RSLayerCacheManagerTest, TryPrepareLayerCacheTest001, TestSize.Level1)
{
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(0);
    auto canvasDrawable = std::static_pointer_cast<DrawableV2::RSCanvasRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(canvasNode));
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
    EXPECT_TRUE(layerCacheManager.layerDrawables_.empty());
    layerCacheManager.TryPrepareLayerCache(canvasDrawable, *canvas);
    ASSERT_NE(canvasDrawable->renderParams_, nullptr);

    canvas->SetSubTreeParallelState(RSPaintFilterCanvas::SubTreeStatus::SUBTREE_QUICK_DRAW_STATE);
    layerCacheManager.TryPrepareLayerCache(canvasDrawable, *canvas);
    ASSERT_NE(canvasDrawable->renderParams_, nullptr);
}

/**
 * @tc.name: TryPrepareLayerCacheTest002
 * @tc.desc: Test TryPrepareLayerCache
 * @tc.type: FUNC
 * @tc.require: issues/22969
 */
HWTEST_F(RSLayerCacheManagerTest, TryPrepareLayerCacheTest002, TestSize.Level2)
{
    NodeId nodeId = 0;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(0);
    auto canvasDrawable = std::static_pointer_cast<DrawableV2::RSCanvasRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(canvasNode));
    ASSERT_NE(canvasDrawable, nullptr);

    auto params = std::make_unique<RSRenderThreadParams>();
    RSUniRenderThread::Instance().Sync(std::move(params));
    RSUniRenderThread::captureParam_.isMirror_ = true;
    std::unordered_set<NodeId> whiteList = { nodeId };
    RSUniRenderThread::Instance().SetWhiteList(whiteList);

    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    AutoSpecialLayerStateRecover whiteListRecover(INVALID_NODEID);
    auto& layerCacheManager = RSLayerCacheManager::Instance();
    EXPECT_TRUE(layerCacheManager.layerDrawables_.empty());
    layerCacheManager.TryPrepareLayerCache(canvasDrawable, canvas);

    // restore
    RSUniRenderThread::Instance().Sync(std::make_unique<RSRenderThreadParams>());
}

/**
 * @tc.name: TryPrepareLayerCacheTest003
 * @tc.desc: Test TryPrepareLayerCache
 * @tc.type: FUNC
 * @tc.require: issues/22969
 */
HWTEST_F(RSLayerCacheManagerTest, TryPrepareLayerCacheTest003, TestSize.Level2)
{
    NodeId nodeId = 0;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(0);
    auto canvasDrawable = std::static_pointer_cast<DrawableV2::RSCanvasRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(canvasNode));
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
    EXPECT_TRUE(layerCacheManager.layerDrawables_.empty());
    layerCacheManager.TryPrepareLayerCache(canvasDrawable, canvas);
    ASSERT_FALSE(canvasDrawable->SkipDrawByWhiteList(canvas));

    // restore
    RSUniRenderThread::Instance().Sync(std::make_unique<RSRenderThreadParams>());
}

/**
 * @tc.name: TryPrepareLayerCacheTest004
 * @tc.desc: Test TryPrepareLayerCache
 * @tc.type: FUNC
 * @tc.require: issues/22969
 */
HWTEST_F(RSLayerCacheManagerTest, TryPrepareLayerCacheTest004, TestSize.Level2)
{
    NodeId nodeId = 0;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(0);
    auto canvasDrawable = std::static_pointer_cast<DrawableV2::RSCanvasRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(canvasNode));
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
    EXPECT_TRUE(layerCacheManager.layerDrawables_.empty());
    layerCacheManager.TryPrepareLayerCache(canvasDrawable, canvas);
    ASSERT_FALSE(canvasDrawable->SkipDrawByWhiteList(canvas));

    // restore
    RSUniRenderThread::Instance().Sync(std::make_unique<RSRenderThreadParams>());
}

/**
 * @tc.name: TryPrepareLayerCacheTest005
 * @tc.desc: Test TryPrepareLayerCache
 * @tc.type: FUNC
 * @tc.require: issues/22969
 */
HWTEST_F(RSLayerCacheManagerTest, TryPrepareLayerCacheTest005, TestSize.Level2)
{
    NodeId nodeId = 0;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(0);
    auto canvasDrawable = std::static_pointer_cast<DrawableV2::RSCanvasRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(canvasNode));
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
    EXPECT_TRUE(layerCacheManager.layerDrawables_.empty());
    layerCacheManager.TryPrepareLayerCache(canvasDrawable, canvas);
    EXPECT_EQ(canvasDrawable->GetOpincDrawCache().GetNodeCacheType(), NodeStrategyType::CACHE_NONE);

    // restore
    RSUniRenderThread::Instance().Sync(std::make_unique<RSRenderThreadParams>());
}

/**
 * @tc.name: TryPrepareLayerCacheTest006
 * @tc.desc: Test TryPrepareLayerCache
 * @tc.type: FUNC
 * @tc.require: issues/22969
 */
HWTEST_F(RSLayerCacheManagerTest, TryPrepareLayerCacheTest006, TestSize.Level2)
{
    {
        NodeId nodeId = 0;
        auto canvasNode = std::make_shared<RSCanvasRenderNode>(0);
        auto canvasDrawable = std::static_pointer_cast<DrawableV2::RSCanvasRenderNodeDrawable>(
            DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(canvasNode));

        Drawing::Canvas drawingCanvas;
        RSPaintFilterCanvas canvas(&drawingCanvas);

        canvasDrawable->isDrawingCacheEnabled_ = true;
        canvas.isUICapture_ = false;
        auto& captureParam = RSUniRenderThread::GetCaptureParam();
        NodeId id = 100;
        captureParam.endNodeId_ = id;
        captureParam.endNodeId_ = id;
        canvasDrawable->nodeId_ = id;
        canvasDrawable->renderParams_ = nullptr;
        auto& layerCacheManager = RSLayerCacheManager::Instance();
        EXPECT_TRUE(layerCacheManager.layerDrawables_.empty());
        layerCacheManager.TryPrepareLayerCache(canvasDrawable, canvas);
    }

    {
        NodeId nodeId = 0;
        auto canvasNode = std::make_shared<RSCanvasRenderNode>(0);
        auto canvasDrawable = std::static_pointer_cast<DrawableV2::RSCanvasRenderNodeDrawable>(
            DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(canvasNode));

        Drawing::Canvas drawingCanvas;
        RSPaintFilterCanvas canvas(&drawingCanvas);

        canvasDrawable->isDrawingCacheEnabled_ = true;
        canvasDrawable->renderParams_ = nullptr;
        auto& layerCacheManager = RSLayerCacheManager::Instance();
        EXPECT_TRUE(layerCacheManager.layerDrawables_.empty());
        layerCacheManager.TryPrepareLayerCache(canvasDrawable, canvas);
    }
}

/**
 * @tc.name: TryPrepareLayerCacheTest007
 * @tc.desc: Test TryPrepareLayerCache
 * @tc.type: FUNC
 * @tc.require: issues/22969
 */
HWTEST_F(RSLayerCacheManagerTest, TryPrepareLayerCacheTest007, TestSize.Level2)
{
    {
        NodeId nodeId = 0;
        auto canvasNode = std::make_shared<RSCanvasRenderNode>(0);
        auto canvasDrawable = std::static_pointer_cast<DrawableV2::RSCanvasRenderNodeDrawable>(
            DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(canvasNode));

        Drawing::Canvas drawingCanvas;
        RSPaintFilterCanvas canvas(&drawingCanvas);

        canvasDrawable->isDrawingCacheEnabled_ = true;
        canvasDrawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
        canvasDrawable->renderParams_->hdrStatus_ = HdrStatus::HDR_EFFECT;
        canvasDrawable->renderParams_->shouldPaint_ = true;
        canvasDrawable->renderParams_->contentEmpty_ = false;
        auto& layerCacheManager = RSLayerCacheManager::Instance();
        EXPECT_TRUE(layerCacheManager.layerDrawables_.empty());
        layerCacheManager.TryPrepareLayerCache(canvasDrawable, canvas);
    }
}

/**
 * @tc.name: TryPrepareLayerCacheTest008
 * @tc.desc: Test TryPrepareLayerCache
 * @tc.type: FUNC
 * @tc.require: issues/22969
 */
HWTEST_F(RSLayerCacheManagerTest, TryPrepareLayerCacheTest008, TestSize.Level2)
{
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    NodeId nodeId = 0;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(0);
    auto canvasDrawable = std::static_pointer_cast<DrawableV2::RSCanvasRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(canvasNode));

    auto& layerCacheManager = RSLayerCacheManager::Instance();
    EXPECT_TRUE(layerCacheManager.layerDrawables_.empty());
    std::shared_ptr<DrawableV2::RSCanvasRenderNodeDrawable> drawable1;
    EXPECT_TRUE(!drawable1);
    layerCacheManager.TryPrepareLayerCache(drawable1, canvas);
}

/**
 * @tc.name: TryPrepareLayerCacheTest009
 * @tc.desc: Test TryPrepareLayerCache
 * @tc.type: FUNC
 * @tc.require: issues/22969
 */
HWTEST_F(RSLayerCacheManagerTest, TryPrepareLayerCacheTest009, TestSize.Level2)
{
    NodeId nodeId = 0;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(0);
    auto canvasDrawable = std::static_pointer_cast<DrawableV2::RSCanvasRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(canvasNode));

    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);

    canvasDrawable->isDrawingCacheEnabled_ = true;
    canvas.isUICapture_ = true;
    auto& captureParam = RSUniRenderThread::GetCaptureParam();
    NodeId id = 100;
    captureParam.endNodeId_ = id;
    canvasDrawable->nodeId_ = id;
    canvasDrawable->renderParams_ = nullptr;
    bool shouldPaint =
        canvasDrawable->ShouldPaint() || (canvas.GetUICapture() && canvasDrawable->IsUiRangeCaptureEndNode());
    EXPECT_TRUE(shouldPaint);
    EXPECT_TRUE(canvasDrawable->isDrawingCacheEnabled_);
    EXPECT_TRUE(canvasDrawable->GetRenderParams() == nullptr);
    EXPECT_TRUE(canvasDrawable);
    auto& layerCacheManager = RSLayerCacheManager::Instance();
    EXPECT_TRUE(layerCacheManager.layerDrawables_.empty());
    layerCacheManager.TryPrepareLayerCache(canvasDrawable, canvas);
}

/**
 * @tc.name: TryPrepareLayerCacheTest011
 * @tc.desc: Test TryPrepareLayerCache
 * @tc.type: FUNC
 * @tc.require: issues/22969
 */
HWTEST_F(RSLayerCacheManagerTest, TryPrepareLayerCacheTest011, TestSize.Level2)
{
    NodeId nodeId = 0;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(0);
    auto canvasDrawable = std::static_pointer_cast<DrawableV2::RSCanvasRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(canvasNode));

    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);

    canvasDrawable->isDrawingCacheEnabled_ = true;

    // drawable->ShouldPaint() == false
    canvasDrawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    canvasDrawable->renderParams_->shouldPaint_ = false;
    canvasDrawable->renderParams_->contentEmpty_ = false;
    // canvas.GetUICapture() == false
    canvas.isUICapture_ = false;
    // drawable->IsUiRangeCaptureEndNode() == false
    auto& captureParam = RSUniRenderThread::GetCaptureParam();
    NodeId id = 100;
    NodeId id1 = 101;
    captureParam.endNodeId_ = id;
    canvasDrawable->nodeId_ = id1;

    auto& layerCacheManager = RSLayerCacheManager::Instance();
    EXPECT_TRUE(layerCacheManager.layerDrawables_.empty());
    layerCacheManager.TryPrepareLayerCache(canvasDrawable, canvas);
}

/**
 * @tc.name: TryPrepareLayerCacheTest012
 * @tc.desc: Test TryPrepareLayerCache
 * @tc.type: FUNC
 * @tc.require: issues/22969
 */
HWTEST_F(RSLayerCacheManagerTest, TryPrepareLayerCacheTest012, TestSize.Level2)
{
    NodeId nodeId = 0;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(0);
    auto canvasDrawable = std::static_pointer_cast<DrawableV2::RSCanvasRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(canvasNode));

    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);

    canvasDrawable->isDrawingCacheEnabled_ = true;

    // drawable->ShouldPaint() == false
    canvasDrawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    canvasDrawable->renderParams_->shouldPaint_ = false;
    canvasDrawable->renderParams_->contentEmpty_ = false;
    // canvas.GetUICapture() == false
    canvas.isUICapture_ = false;
    // drawable->IsUiRangeCaptureEndNode() == true
    auto& captureParam = RSUniRenderThread::GetCaptureParam();
    NodeId id = 100;
    captureParam.endNodeId_ = id;
    canvasDrawable->nodeId_ = id;

    auto& layerCacheManager = RSLayerCacheManager::Instance();
    EXPECT_TRUE(layerCacheManager.layerDrawables_.empty());
    layerCacheManager.TryPrepareLayerCache(canvasDrawable, canvas);
}

/**
 * @tc.name: TryPrepareLayerCacheTest013
 * @tc.desc: Test TryPrepareLayerCache
 * @tc.type: FUNC
 * @tc.require: issues/22969
 */
HWTEST_F(RSLayerCacheManagerTest, TryPrepareLayerCacheTest013, TestSize.Level2)
{
    NodeId nodeId = 0;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(0);
    auto canvasDrawable = std::static_pointer_cast<DrawableV2::RSCanvasRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(canvasNode));

    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);

    canvasDrawable->isDrawingCacheEnabled_ = true;

    // drawable->ShouldPaint() == false
    canvasDrawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    canvasDrawable->renderParams_->shouldPaint_ = false;
    canvasDrawable->renderParams_->contentEmpty_ = false;
    // canvas.GetUICapture() == true
    canvas.isUICapture_ = true;
    // drawable->IsUiRangeCaptureEndNode() == false
    auto& captureParam = RSUniRenderThread::GetCaptureParam();
    NodeId id = 100;
    NodeId id1 = 101;
    captureParam.endNodeId_ = id;
    canvasDrawable->nodeId_ = id1;

    auto& layerCacheManager = RSLayerCacheManager::Instance();
    EXPECT_TRUE(layerCacheManager.layerDrawables_.empty());
    layerCacheManager.TryPrepareLayerCache(canvasDrawable, canvas);
}

/**
 * @tc.name: TryPrepareLayerCacheTest014
 * @tc.desc: Test TryPrepareLayerCache
 * @tc.type: FUNC
 * @tc.require: issues/22969
 */
HWTEST_F(RSLayerCacheManagerTest, TryPrepareLayerCacheTest014, TestSize.Level2)
{
    NodeId nodeId = 0;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(0);
    auto canvasDrawable = std::static_pointer_cast<DrawableV2::RSCanvasRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(canvasNode));

    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);

    canvasDrawable->isDrawingCacheEnabled_ = true;

    // drawable->ShouldPaint() == false
    canvasDrawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    canvasDrawable->renderParams_->shouldPaint_ = false;
    canvasDrawable->renderParams_->contentEmpty_ = false;
    // canvas.GetUICapture() == true
    canvas.isUICapture_ = true;
    // drawable->IsUiRangeCaptureEndNode() == true
    auto& captureParam = RSUniRenderThread::GetCaptureParam();
    NodeId id = 100;
    captureParam.endNodeId_ = id;
    canvasDrawable->nodeId_ = id;

    auto& layerCacheManager = RSLayerCacheManager::Instance();
    EXPECT_TRUE(layerCacheManager.layerDrawables_.empty());
    layerCacheManager.TryPrepareLayerCache(canvasDrawable, canvas);
}

/**
 * @tc.name: TryPrepareLayerCacheTest015
 * @tc.desc: Test TryPrepareLayerCache
 * @tc.type: FUNC
 * @tc.require: issues/22969
 */
HWTEST_F(RSLayerCacheManagerTest, TryPrepareLayerCacheTest015, TestSize.Level2)
{
    NodeId nodeId = 0;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(0);
    auto canvasDrawable = std::static_pointer_cast<DrawableV2::RSCanvasRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(canvasNode));

    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);

    canvasDrawable->isDrawingCacheEnabled_ = true;

    // drawable->ShouldPaint() == true
    canvasDrawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    canvasDrawable->renderParams_->shouldPaint_ = true;
    canvasDrawable->renderParams_->contentEmpty_ = false;
    // canvas.GetUICapture() == true
    canvas.isUICapture_ = true;
    // drawable->IsUiRangeCaptureEndNode() == true
    auto& captureParam = RSUniRenderThread::GetCaptureParam();
    NodeId id = 100;
    captureParam.endNodeId_ = id;
    canvasDrawable->nodeId_ = id;

    auto& layerCacheManager = RSLayerCacheManager::Instance();
    EXPECT_TRUE(layerCacheManager.layerDrawables_.empty());
    layerCacheManager.TryPrepareLayerCache(canvasDrawable, canvas);
}

/**
 * @tc.name: TryPrepareLayerCacheTest016
 * @tc.desc: Test TryPrepareLayerCache
 * @tc.type: FUNC
 * @tc.require: issues/22969
 */
HWTEST_F(RSLayerCacheManagerTest, TryPrepareLayerCacheTest016, TestSize.Level2)
{
    NodeId nodeId = 0;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(0);
    auto canvasDrawable = std::static_pointer_cast<DrawableV2::RSCanvasRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(canvasNode));

    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);

    canvasDrawable->isDrawingCacheEnabled_ = true;

    // drawable->ShouldPaint() == true
    canvasDrawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    canvasDrawable->renderParams_->shouldPaint_ = true;
    canvasDrawable->renderParams_->contentEmpty_ = false;
    // canvas.GetUICapture() == true
    canvas.isUICapture_ = true;
    // drawable->IsUiRangeCaptureEndNode() == false
    auto& captureParam = RSUniRenderThread::GetCaptureParam();
    NodeId id = 100;
    NodeId id1 = 101;
    canvasDrawable->nodeId_ = id1;

    auto& layerCacheManager = RSLayerCacheManager::Instance();
    EXPECT_TRUE(layerCacheManager.layerDrawables_.empty());
    layerCacheManager.TryPrepareLayerCache(canvasDrawable, canvas);
}

/**
 * @tc.name: TryPrepareLayerCacheTest017
 * @tc.desc: Test TryPrepareLayerCache
 * @tc.type: FUNC
 * @tc.require: issues/22969
 */
HWTEST_F(RSLayerCacheManagerTest, TryPrepareLayerCacheTest017, TestSize.Level2)
{
    NodeId nodeId = 0;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(0);
    auto canvasDrawable = std::static_pointer_cast<DrawableV2::RSCanvasRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(canvasNode));

    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);

    canvasDrawable->isDrawingCacheEnabled_ = true;

    // drawable->ShouldPaint() == true
    canvasDrawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    canvasDrawable->renderParams_->shouldPaint_ = true;
    canvasDrawable->renderParams_->contentEmpty_ = false;
    // canvas.GetUICapture() == false
    canvas.isUICapture_ = false;
    // drawable->IsUiRangeCaptureEndNode() == true
    auto& captureParam = RSUniRenderThread::GetCaptureParam();
    NodeId id = 100;
    captureParam.endNodeId_ = id;
    canvasDrawable->nodeId_ = id;

    auto& layerCacheManager = RSLayerCacheManager::Instance();
    EXPECT_TRUE(layerCacheManager.layerDrawables_.empty());
    layerCacheManager.TryPrepareLayerCache(canvasDrawable, canvas);
}

/**
 * @tc.name: TryPrepareLayerCacheTest018
 * @tc.desc: Test TryPrepareLayerCache
 * @tc.type: FUNC
 * @tc.require: issues/22969
 */
HWTEST_F(RSLayerCacheManagerTest, TryPrepareLayerCacheTest018, TestSize.Level2)
{
    NodeId nodeId = 0;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(0);
    auto canvasDrawable = std::static_pointer_cast<DrawableV2::RSCanvasRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(canvasNode));

    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);

    canvasDrawable->isDrawingCacheEnabled_ = true;

    // drawable->ShouldPaint() == true
    canvasDrawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    canvasDrawable->renderParams_->shouldPaint_ = true;
    canvasDrawable->renderParams_->contentEmpty_ = false;
    // canvas.GetUICapture() == false
    canvas.isUICapture_ = false;
    // drawable->IsUiRangeCaptureEndNode() == false
    auto& captureParam = RSUniRenderThread::GetCaptureParam();
    NodeId id = 100;
    NodeId id1 = 101;
    captureParam.endNodeId_ = id;
    canvasDrawable->nodeId_ = id1;

    auto& layerCacheManager = RSLayerCacheManager::Instance();
    EXPECT_TRUE(layerCacheManager.layerDrawables_.empty());
    layerCacheManager.TryPrepareLayerCache(canvasDrawable, canvas);
}

/**
 * @tc.name: GetLayerDebugEnabledTest
 * @tc.desc: Test GetLayerDebugEnabled
 * @tc.type: FUNC
 * @tc.require: issues/22969
 */
HWTEST_F(RSLayerCacheManagerTest, GetLayerDebugEnabledTest, TestSize.Level1)
{
    NodeId nodeId = 0;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(0);
    auto canvasDrawable = std::static_pointer_cast<DrawableV2::RSCanvasRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(canvasNode));

    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    canvasDrawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);

    auto& layerCacheManager = OHOS::Rosen::RSLayerCacheManager::Instance();
    layerCacheManager.LayerCacheRegionDfx(canvasDrawable, canvas);

    const std::string debugKey = "rosen.graphic.layerDebugEnabled";
    const std::string oldDebugValue = system::GetParameter(debugKey, "0");

    (void)system::SetParameter(debugKey, "1");

    EXPECT_TRUE(RSSystemProperties::GetLayerDebugEnabled());

    layerCacheManager.LayerCacheRegionDfx(canvasDrawable, canvas);

    (void)system::SetParameter(debugKey, oldDebugValue);
}

/**
 * @tc.name: ShouldEnableLayerCacheTest
 * @tc.desc: Test ShouldEnableLayerCache
 * @tc.type: FUNC
 * @tc.require: issues/22969
 */
HWTEST_F(RSLayerCacheManagerTest, ShouldEnableLayerCacheTest, TestSize.Level1)
{
    NodeId nodeId = 0;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(0);
    auto canvasDrawable = std::static_pointer_cast<DrawableV2::RSCanvasRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(canvasNode));

    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    canvasDrawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);

    auto& layerCacheManager = OHOS::Rosen::RSLayerCacheManager::Instance();

    float gpuMem = 200, totalRSMem = 20;
    EXPECT_FALSE(layerCacheManager.ShouldEnableLayerCache(canvasDrawable, gpuMem, totalRSMem));

    gpuMem = 0;
    totalRSMem = 40;
    EXPECT_TRUE(layerCacheManager.ShouldEnableLayerCache(canvasDrawable, gpuMem, totalRSMem));

    Drawing::RectF frameRect(0.0f, 0.0f, 1000.0f, 1000.0f);
    canvasDrawable->renderParams_->SetFrameRect(frameRect);
    EXPECT_FALSE(layerCacheManager.ShouldEnableLayerCache(canvasDrawable, gpuMem, totalRSMem));
}

/**
 * @tc.name: LayerCacheRegionDfxTest
 * @tc.desc: Test LayerCacheRegionDfx
 * @tc.type: FUNC
 * @tc.require: issues/22969
 */
HWTEST_F(RSLayerCacheManagerTest, LayerCacheRegionDfxTest, TestSize.Level1)
{
    NodeId nodeId = 0;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(0);
    auto canvasDrawable = std::static_pointer_cast<DrawableV2::RSCanvasRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(canvasNode));

    Drawing::Canvas drawingCanvas;
    drawingCanvas.gpuContext_ = std::make_shared<Drawing::GPUContext>();
    RSPaintFilterCanvas canvas(&drawingCanvas);
    canvasDrawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    EXPECT_TRUE(canvasDrawable->renderParams_ != nullptr);

    const std::string debugKey = "rosen.graphic.layerDebugEnabled";
    const std::string oldDebugValue = system::GetParameter(debugKey, "0");
    (void)system::SetParameter(debugKey, "0");
    EXPECT_TRUE(!RSSystemProperties::GetLayerDebugEnabled());

    auto& layerCacheManager = OHOS::Rosen::RSLayerCacheManager::Instance();
    layerCacheManager.LayerCacheRegionDfx(canvasDrawable, canvas);

    (void)system::SetParameter(debugKey, "1");
    EXPECT_TRUE(RSSystemProperties::GetLayerDebugEnabled());
    EXPECT_TRUE(layerCacheManager.layerDrawables_.empty());
    (void)system::SetParameter(debugKey, oldDebugValue);
}
} // namespace Rosen
} // namespace OHOS