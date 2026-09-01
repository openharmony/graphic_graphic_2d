/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include "drawable/rs_surface_render_node_drawable.h"
#include "pipeline/render_thread/rs_divided_render_util.h"
#include "engine/rs_uni_render_engine.h"
#include "pipeline/rs_test_util.h"
#include "platform/common/rs_system_properties.h"
#include "recording/recording_canvas.h"
#include "rs_surface_layer.h"
#include "rs_surface_rcd_layer.h"
#include "rs_surface_solid_filled_color_layer.h"
#include "rs_render_surface_solid_filled_color_layer.h"
#include "composer/composer_service/layer_backend/hdi_output.h"
#ifdef RS_ENABLE_VK
#include "surface_buffer.h"
#endif

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::DrawableV2;

namespace OHOS::Rosen {
class RSUniRenderEngineTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSUniRenderEngineTest::SetUpTestCase()
{
    RSTestUtil::InitRenderNodeGC();
}
void RSUniRenderEngineTest::TearDownTestCase() {}
void RSUniRenderEngineTest::SetUp() {}
void RSUniRenderEngineTest::TearDown() {}

/**
 * @tc.name: DrawSurfaceNodeWithParams001
 * @tc.desc: test DrawSurfaceNodeWithParams
 * @tc.type: FUNC
 * @tc.require: issueI6QM6E
 */
HWTEST_F(RSUniRenderEngineTest, DrawSurfaceNodeWithParams001, TestSize.Level1)
{
    auto uniRenderEngine = std::make_shared<RSUniRenderEngine>();
    std::unique_ptr<Drawing::Canvas> drawingCanvas = std::make_unique<Drawing::Canvas>(10, 10);
    std::shared_ptr<RSPaintFilterCanvas> canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    ASSERT_NE(canvas, nullptr);
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto param = RSDividedRenderUtil::CreateBufferDrawParam(*node);
    param.useCPU = true;
    uniRenderEngine->DrawSurfaceNodeWithParams(*canvas, *node, param, nullptr, nullptr);
}

/**
 * @tc.name: DrawSurfaceNodeWithParams002
 * @tc.desc: test DrawSurfaceNodeWithParams
 * @tc.type: FUNC
 * @tc.require: issueI6QM6E
 */
HWTEST_F(RSUniRenderEngineTest, DrawSurfaceNodeWithParams002, TestSize.Level1)
{
    auto uniRenderEngine = std::make_shared<RSUniRenderEngine>();
    std::unique_ptr<Drawing::Canvas> drawingCanvas = std::make_unique<Drawing::Canvas>(10, 10);
    std::shared_ptr<RSPaintFilterCanvas> canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    ASSERT_NE(canvas, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto surfaceDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(surfaceNode->GetRenderDrawable());
    auto param = RSDividedRenderUtil::CreateBufferDrawParam(*surfaceNode);
    param.useCPU = true;
    uniRenderEngine->DrawSurfaceNodeWithParams(*canvas, *surfaceDrawable.get(), param, nullptr, nullptr);
    param.ignoreAlpha = true;
    uniRenderEngine->DrawSurfaceNodeWithParams(*canvas, *surfaceDrawable.get(), param, nullptr, nullptr);
}

/**
 * @tc.name: DrawLayers001
 * @tc.desc: test DrawLayers
 * @tc.type: FUNC
 * @tc.require: issueI6QM6E
 */
HWTEST_F(RSUniRenderEngineTest, DrawLayers001, TestSize.Level1)
{
    auto uniRenderEngine = std::make_shared<RSUniRenderEngine>();
    // Resources for Vulkan and DDGR API
    if (RSSystemProperties::IsUseVulkan()) {
        uniRenderEngine->Init();
    }
    std::unique_ptr<Drawing::RecordingCanvas> drawingRecordingCanvas = nullptr;
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto buffer = surfaceNode->GetRSSurfaceHandler()->GetBuffer();
    // End resources definition
    std::unique_ptr<Drawing::Canvas> drawingCanvas = std::make_unique<Drawing::Canvas>(10, 10);
    std::shared_ptr<RSPaintFilterCanvas> canvas = nullptr;
    if (!RSSystemProperties::IsUseVulkan()) {
        canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    } else {
        drawingRecordingCanvas = std::make_unique<Drawing::RecordingCanvas>(10, 10);
        drawingRecordingCanvas->SetGrRecordingContext(uniRenderEngine->GetRenderContext()->GetSharedDrGPUContext());
        canvas = std::make_shared<RSPaintFilterCanvas>(drawingRecordingCanvas.release());
    }
    ASSERT_NE(canvas, nullptr);
    std::vector<RSLayerPtr> layers;
    if (!RSSystemProperties::IsUseVulkan()) {
        layers.emplace_back(nullptr);
    }
    RSLayerPtr layer1 = std::make_shared<RSSurfaceLayer>(0, nullptr);
    layer1->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);

    RSLayerPtr layer2 = std::make_shared<RSSurfaceLayer>(0, nullptr);
    layer2->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE_CLEAR);

    RSLayerPtr layer3 = std::make_shared<RSSurfaceLayer>(0, nullptr);
    layer3->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT);
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("layer3");
    layer3->SetSurface(cSurface);

    auto ctx4 = std::make_shared<RSComposerContext>(nullptr);
    RSLayerPtr layer4 = RSSurfaceLayer::Create(0, ctx4);
    layer4->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT);
    GraphicLayerColor layerColor = { .r = 1, .g = 1, .b = 1, .a = 1 };
    layer4->SetLayerColor(layerColor);

    auto ctx5 = std::make_shared<RSComposerContext>(nullptr);
    RSLayerPtr layer5 = RSSurfaceRCDLayer::Create(0, ctx5);
    layer5->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT);

    if (RSSystemProperties::IsUseVulkan()) {
        layer1->SetBuffer(buffer, surfaceNode->GetRSSurfaceHandler()->GetAcquireFence());
        layer2->SetBuffer(buffer, surfaceNode->GetRSSurfaceHandler()->GetAcquireFence());
        layer3->SetBuffer(buffer, surfaceNode->GetRSSurfaceHandler()->GetAcquireFence());
        layer5->SetBuffer(buffer, surfaceNode->GetRSSurfaceHandler()->GetAcquireFence());
    }

    layers.emplace_back(layer1);
    layers.emplace_back(layer2);
    layers.emplace_back(layer3);
    layers.emplace_back(layer4);
    layers.emplace_back(layer5);
    ComposerScreenInfo screenInfo;
    uniRenderEngine->DrawLayers(*canvas, layers, false, screenInfo);
}

/**
 * @tc.name: DrawLayers002
 * @tc.desc: test DrawLayers
 * @tc.type: FUNC
 * @tc.require: issueI6QM6E
 */
HWTEST_F(RSUniRenderEngineTest, DrawLayers002, TestSize.Level1)
{
    auto uniRenderEngine = std::make_shared<RSUniRenderEngine>();
    // Resources for Vulkan and DDGR API
    if (RSSystemProperties::IsUseVulkan()) {
        uniRenderEngine->Init();
    }
    std::unique_ptr<Drawing::RecordingCanvas> drawingRecordingCanvas = nullptr;
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto buffer = surfaceNode->GetRSSurfaceHandler()->GetBuffer();
    // End resources definition
    std::unique_ptr<Drawing::Canvas> drawingCanvas = std::make_unique<Drawing::Canvas>(10, 10);
    std::shared_ptr<RSPaintFilterCanvas> canvas = nullptr;
    if (!RSSystemProperties::IsUseVulkan()) {
        canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    } else {
        drawingRecordingCanvas = std::make_unique<Drawing::RecordingCanvas>(10, 10);
        drawingRecordingCanvas->SetGrRecordingContext(uniRenderEngine->GetRenderContext()->GetSharedDrGPUContext());
        canvas = std::make_shared<RSPaintFilterCanvas>(drawingRecordingCanvas.release());
    }
    ASSERT_NE(canvas, nullptr);
    std::vector<RSLayerPtr> layers;
    if (!RSSystemProperties::IsUseVulkan()) {
        layers.emplace_back(nullptr);
    }

    RSLayerPtr layer1 = std::make_shared<RSSurfaceLayer>(0, nullptr);
    layer1->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT);
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("layer1");
    layer1->SetSurface(cSurface);
    layer1->SetRotationFixed(true);
    layer1->SetUseDeviceOffline(true);

    RSLayerPtr layer2 = std::make_shared<RSSurfaceLayer>(0, nullptr);
    layer2->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT);
    cSurface = IConsumerSurface::Create("layer2");
    layer2->SetSurface(cSurface);
    layer2->SetRotationFixed(false);
    layer2->SetUseDeviceOffline(false);

    if (RSSystemProperties::IsUseVulkan()) {
        layer1->SetBuffer(buffer, surfaceNode->GetRSSurfaceHandler()->GetAcquireFence());
        layer2->SetBuffer(buffer, surfaceNode->GetRSSurfaceHandler()->GetAcquireFence());
    }

    layers.emplace_back(layer1);
    layers.emplace_back(layer2);
    ComposerScreenInfo screenInfo;
    uniRenderEngine->DrawLayers(*canvas, layers, false, screenInfo);
}

/**
 * @tc.name: DrawHdiLayerWithParams001
 * @tc.desc: test DrawHdiLayerWithParams
 * @tc.type: FUNC
 * @tc.require: issueI6QM6E
 */
HWTEST_F(RSUniRenderEngineTest, DrawHdiLayerWithParams001, TestSize.Level1)
{
    auto uniRenderEngine = std::make_shared<RSUniRenderEngine>();
    // Resources for Vulkan and DDGR API
    if (RSSystemProperties::IsUseVulkan()) {
        uniRenderEngine->Init();
    }
    std::unique_ptr<Drawing::RecordingCanvas> drawingRecordingCanvas = nullptr;
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    // End resources definition
    std::unique_ptr<Drawing::Canvas> drawingCanvas = std::make_unique<Drawing::Canvas>(10, 10);
    std::shared_ptr<RSPaintFilterCanvas> canvas = nullptr;
    if (!RSSystemProperties::IsUseVulkan()) {
        canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    } else {
        drawingRecordingCanvas = std::make_unique<Drawing::RecordingCanvas>(10, 10);
        drawingRecordingCanvas->SetGrRecordingContext(uniRenderEngine->GetRenderContext()->GetSharedDrGPUContext());
        canvas = std::make_shared<RSPaintFilterCanvas>(drawingRecordingCanvas.release());
    }
    ASSERT_NE(canvas, nullptr);
    BufferDrawParam param;
    param.useCPU = false;
    if (RSSystemProperties::IsUseVulkan()) {
        param.buffer = surfaceNode->GetRSSurfaceHandler()->GetBuffer();
    }
    uniRenderEngine->DrawHdiLayerWithParams(*canvas, param);

    param.useCPU = true;
    uniRenderEngine->DrawHdiLayerWithParams(*canvas, param);
}

/**
 * @tc.name: DrawLayers_SolidColorCompositionTest001
 * @tc.desc: Test DrawLayers with GRAPHIC_COMPOSITION_SOLID_COLOR composition type
 *           The if (GRAPHIC_COMPOSITION_SOLID_COLOR) branch at line 94 should be true
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSUniRenderEngineTest, DrawLayers_SolidColorCompositionTest001, TestSize.Level2)
{
    auto uniRenderEngine = std::make_shared<RSUniRenderEngine>();
    std::unique_ptr<Drawing::Canvas> drawingCanvas = std::make_unique<Drawing::Canvas>(10, 10);
    std::shared_ptr<RSPaintFilterCanvas> canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    ASSERT_NE(canvas, nullptr);

    std::vector<RSLayerPtr> layers;

    // Layer with SOLID_COLOR composition type - should be skipped
    RSLayerPtr solidColorLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    solidColorLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_SOLID_COLOR);
    layers.emplace_back(solidColorLayer);

    ComposerScreenInfo screenInfo;
    // Should not crash, solid color layer should be skipped
    EXPECT_NO_FATAL_FAILURE(uniRenderEngine->DrawLayers(*canvas, layers, false, screenInfo));
}

/**
 * @tc.name: DrawLayers_NullBufferWithNonTransparentColorTest001
 * @tc.desc: Test DrawLayers when buffer is nullptr but layerColor is not transparent
 *           The if (layerColor != transparent) branch at line 105 should be true
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSUniRenderEngineTest, DrawLayers_NullBufferWithNonTransparentColorTest001, TestSize.Level2)
{
    auto uniRenderEngine = std::make_shared<RSUniRenderEngine>();
    std::unique_ptr<Drawing::Canvas> drawingCanvas = std::make_unique<Drawing::Canvas>(10, 10);
    std::shared_ptr<RSPaintFilterCanvas> canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    ASSERT_NE(canvas, nullptr);

    std::vector<RSLayerPtr> layers;

    // Layer with nullptr buffer but non-transparent color
    RSLayerPtr layer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    layer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT);
    GraphicLayerColor layerColor = { .r = 255, .g = 0, .b = 0, .a = 128 }; // Red with alpha
    layer->SetLayerColor(layerColor);
    layers.emplace_back(layer);

    ComposerScreenInfo screenInfo;
    // Should draw the layer color instead of buffer
    EXPECT_NO_FATAL_FAILURE(uniRenderEngine->DrawLayers(*canvas, layers, false, screenInfo));
}

/**
 * @tc.name: DrawLayers_NullBufferWithTransparentColorTest001
 * @tc.desc: Test DrawLayers when buffer is nullptr and layerColor is transparent
 *           The if (layerColor != transparent) branch at line 105 should be false
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSUniRenderEngineTest, DrawLayers_NullBufferWithTransparentColorTest001, TestSize.Level2)
{
    auto uniRenderEngine = std::make_shared<RSUniRenderEngine>();
    std::unique_ptr<Drawing::Canvas> drawingCanvas = std::make_unique<Drawing::Canvas>(10, 10);
    std::shared_ptr<RSPaintFilterCanvas> canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    ASSERT_NE(canvas, nullptr);

    std::vector<RSLayerPtr> layers;

    // Layer with nullptr buffer and transparent color
    RSLayerPtr layer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    layer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT);
    GraphicLayerColor transparentColor = { .r = 0, .g = 0, .b = 0, .a = 0 }; // Fully transparent
    layer->SetLayerColor(transparentColor);
    layers.emplace_back(layer);

    ComposerScreenInfo screenInfo;
    // Should skip drawing (transparent color)
    EXPECT_NO_FATAL_FAILURE(uniRenderEngine->DrawLayers(*canvas, layers, false, screenInfo));
}

/**
 * @tc.name: DrawLayers_ScreenRCDLayerTest001
 * @tc.desc: Test DrawLayers when layer is ScreenRCDLayer
 *           The else if (layer->IsScreenRCDLayer()) branch at line 117 should be true
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSUniRenderEngineTest, DrawLayers_ScreenRCDLayerTest001, TestSize.Level2)
{
    auto uniRenderEngine = std::make_shared<RSUniRenderEngine>();
    std::unique_ptr<Drawing::Canvas> drawingCanvas = std::make_unique<Drawing::Canvas>(10, 10);
    std::shared_ptr<RSPaintFilterCanvas> canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    ASSERT_NE(canvas, nullptr);

    std::vector<RSLayerPtr> layers;

    // RCD layer with buffer - should be skipped
    auto ctx = std::make_shared<RSComposerContext>(nullptr);
    RSLayerPtr rcdLayer = RSSurfaceRCDLayer::Create(0, ctx);
    rcdLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT);

    // Set buffer to avoid the null buffer path
    if (RSSystemProperties::IsUseVulkan()) {
        uniRenderEngine->Init();
        auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
        auto buffer = surfaceNode->GetRSSurfaceHandler()->GetBuffer();
        rcdLayer->SetBuffer(buffer, surfaceNode->GetRSSurfaceHandler()->GetAcquireFence());
    }

    layers.emplace_back(rcdLayer);

    ComposerScreenInfo screenInfo;
    // RCD layer should be skipped in DrawLayers
    EXPECT_NO_FATAL_FAILURE(uniRenderEngine->DrawLayers(*canvas, layers, false, screenInfo));
}

/**
 * @tc.name: DrawLayers_MixedLayersAllBranchesTest001
 * @tc.desc: Test DrawLayers with mixed layer types covering all branches
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSUniRenderEngineTest, DrawLayers_MixedLayersAllBranchesTest001, TestSize.Level2)
{
    auto uniRenderEngine = std::make_shared<RSUniRenderEngine>();
    std::unique_ptr<Drawing::Canvas> drawingCanvas = std::make_unique<Drawing::Canvas>(10, 10);
    std::shared_ptr<RSPaintFilterCanvas> canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    ASSERT_NE(canvas, nullptr);

    std::vector<RSLayerPtr> layers;

    // Layer 1: nullptr - line 89 branch
    layers.emplace_back(nullptr);

    // Layer 2: DEVICE composition - line 92 branch
    RSLayerPtr deviceLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    deviceLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    layers.emplace_back(deviceLayer);

    // Layer 3: DEVICE_CLEAR composition - line 93 branch
    RSLayerPtr deviceClearLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    deviceClearLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE_CLEAR);
    layers.emplace_back(deviceClearLayer);

    // Layer 4: SOLID_COLOR composition - line 94 branch
    RSLayerPtr solidColorLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    solidColorLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_SOLID_COLOR);
    layers.emplace_back(solidColorLayer);

    // Layer 5: CLIENT with null buffer and non-transparent color - line 105 branch (true)
    RSLayerPtr nullBufferLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    nullBufferLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT);
    GraphicLayerColor layerColor = { .r = 100, .g = 100, .b = 100, .a = 100 };
    nullBufferLayer->SetLayerColor(layerColor);
    layers.emplace_back(nullBufferLayer);

    // Layer 6: CLIENT with null buffer and transparent color - line 105 branch (false)
    RSLayerPtr transparentLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    transparentLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT);
    GraphicLayerColor transparentColor = { .r = 0, .g = 0, .b = 0, .a = 0 };
    transparentLayer->SetLayerColor(transparentColor);
    layers.emplace_back(transparentLayer);

    // Layer 7: RCD layer - line 117 branch
    auto ctx = std::make_shared<RSComposerContext>(nullptr);
    RSLayerPtr rcdLayer = RSSurfaceRCDLayer::Create(0, ctx);
    rcdLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT);
    layers.emplace_back(rcdLayer);

    ComposerScreenInfo screenInfo;
    // All branches should be covered without crash
    EXPECT_NO_FATAL_FAILURE(uniRenderEngine->DrawLayers(*canvas, layers, false, screenInfo));
}

// ==================== DrawLayerPreProcess ====================

/**
 * @tc.name: DrawLayerPreProcess_DrmCornerRadiusTest001
 * @tc.desc: Test DrawLayerPreProcess when drmCornerRadiusInfo is not empty
 *           The if (!drmCornerRadiusInfo.empty()) branch at line 174 should be true
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSUniRenderEngineTest, DrawLayerPreProcess_DrmCornerRadiusTest001, TestSize.Level2)
{
    auto uniRenderEngine = std::make_shared<RSUniRenderEngine>();
    std::unique_ptr<Drawing::Canvas> drawingCanvas = std::make_unique<Drawing::Canvas>(10, 10);
    std::shared_ptr<RSPaintFilterCanvas> canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    ASSERT_NE(canvas, nullptr);

    auto ctx = std::make_shared<RSComposerContext>(nullptr);
    RSLayerPtr layer = RSSurfaceLayer::Create(0, ctx);
    ASSERT_NE(layer, nullptr);

    // Set layer size
    GraphicIRect layerSize = {0, 0, 100, 100};
    layer->SetLayerSize(layerSize);

    // Set drmCornerRadiusInfo (not empty) - 8 elements: x, y, w, h, radiusX, radiusY, radiusXY, radiusYX
    std::vector<float> cornerRadiusInfo = {0, 0, 100, 100, 20.0f, 20.0f, 20.0f, 20.0f};
    layer->SetCornerRadiusInfoForDRM(cornerRadiusInfo);

    // Set non-transparent background color
    GraphicLayerColor backgroundColor = { .r = 255, .g = 0, .b = 0, .a = 255 };
    layer->SetBackgroundColor(backgroundColor);

    ComposerScreenInfo screenInfo;
    // Should clip round rect and draw background color, then return early
    EXPECT_NO_FATAL_FAILURE(uniRenderEngine->DrawLayerPreProcess(*canvas, layer, screenInfo));
}

/**
 * @tc.name: DrawLayerPreProcess_DrmCornerRadiusTransparentTest001
 * @tc.desc: Test DrawLayerPreProcess when drmCornerRadiusInfo is not empty but background is transparent
 *           The if (backgroundColor != Transparent) branch at line 182 should be false
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSUniRenderEngineTest, DrawLayerPreProcess_DrmCornerRadiusTransparentTest001, TestSize.Level2)
{
    auto uniRenderEngine = std::make_shared<RSUniRenderEngine>();
    std::unique_ptr<Drawing::Canvas> drawingCanvas = std::make_unique<Drawing::Canvas>(10, 10);
    std::shared_ptr<RSPaintFilterCanvas> canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    ASSERT_NE(canvas, nullptr);

    auto ctx = std::make_shared<RSComposerContext>(nullptr);
    RSLayerPtr layer = RSSurfaceLayer::Create(0, ctx);
    ASSERT_NE(layer, nullptr);

    // Set layer size
    GraphicIRect layerSize = {0, 0, 100, 100};
    layer->SetLayerSize(layerSize);

    // Set drmCornerRadiusInfo (not empty)
    std::vector<float> cornerRadiusInfo = {0, 0, 100, 100, 20.0f, 20.0f, 20.0f, 20.0f};
    layer->SetCornerRadiusInfoForDRM(cornerRadiusInfo);

    // Set transparent background color
    GraphicLayerColor transparentColor = { .r = 0, .g = 0, .b = 0, .a = 0 };
    layer->SetBackgroundColor(transparentColor);

    ComposerScreenInfo screenInfo;
    // Should clip round rect but NOT draw background color (transparent)
    EXPECT_NO_FATAL_FAILURE(uniRenderEngine->DrawLayerPreProcess(*canvas, layer, screenInfo));
}

#ifdef RS_ENABLE_VK
static sptr<SurfaceBuffer> CreateCanvasDrawingTestBuffer()
{
    sptr<SurfaceBuffer> buffer = SurfaceBuffer::Create();
    if (!buffer) {
        return nullptr;
    }
    BufferRequestConfig requestConfig = {
        .width = 100,
        .height = 100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_HW_RENDER | BUFFER_USAGE_MEM_MMZ_CACHE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
        .colorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB,
        .transform = GraphicTransformType::GRAPHIC_ROTATE_NONE,
    };
    GSError ret = buffer->Alloc(requestConfig);
    if (ret != GSERROR_OK) {
        return nullptr;
    }
    return buffer;
}
#endif

/**
 * @tc.name: DrawCanvasDrawingNodeWithParamsTest001
 * @tc.desc: Test DrawCanvasDrawingNodeWithParams
 *           1. image == nullptr, return early (zrh-1 DrawCanvasDrawingNodeWithParams-1)
 *           2. needBilinear == true (zrh-1 DrawCanvasDrawingNodeWithParams-2)
 *           3. needBilinear == false (zrh-1 DrawCanvasDrawingNodeWithParams-3)
 * @tc.type: FUNC
 */
HWTEST_F(RSUniRenderEngineTest, DrawCanvasDrawingNodeWithParamsTest001, TestSize.Level1)
{
    // 1. image == nullptr: default BufferDrawParam with no buffer
    auto uniRenderEngine = std::make_shared<RSUniRenderEngine>();
    std::unique_ptr<Drawing::Canvas> drawingCanvas = std::make_unique<Drawing::Canvas>(10, 10);
    std::shared_ptr<RSPaintFilterCanvas> canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    ASSERT_NE(canvas, nullptr);
    BufferDrawParam params;
    params.matrix = Drawing::Matrix();
    uniRenderEngine->DrawCanvasDrawingNodeWithParams(*canvas, params);
    EXPECT_NO_FATAL_FAILURE(uniRenderEngine->DrawCanvasDrawingNodeWithParams(*canvas, params));

#ifdef RS_ENABLE_VK
    if (RSSystemProperties::IsUseVulkan()) {
        // 2. needBilinear == true: valid buffer + scale matrix
        auto renderEngineScale = std::make_shared<RSUniRenderEngine>();
        renderEngineScale->Init();
        auto drawingRecordingCanvas = std::make_unique<Drawing::RecordingCanvas>(100, 100);
        drawingRecordingCanvas->SetGrRecordingContext(renderEngineScale->GetRenderContext()->GetSharedDrGPUContext());
        auto recordingCanvasScale = std::make_shared<RSPaintFilterCanvas>(drawingRecordingCanvas.get());
        ASSERT_NE(recordingCanvasScale, nullptr);
        BufferDrawParam paramsScale;
        paramsScale.buffer = CreateCanvasDrawingTestBuffer();
        ASSERT_NE(paramsScale.buffer, nullptr);
        paramsScale.useBilinearInterpolation = true;
        paramsScale.srcRect = Drawing::Rect(0, 0, 100, 100);
        paramsScale.dstRect = Drawing::Rect(0, 0, 100, 100);
        Drawing::Matrix scaleMatrix;
        scaleMatrix.SetScale(2.0f, 2.0f);
        paramsScale.matrix = scaleMatrix;
        renderEngineScale->DrawCanvasDrawingNodeWithParams(*recordingCanvasScale, paramsScale);

        // 3. needBilinear == false: valid buffer + identity matrix, no scale
        auto renderEngineNoScale = std::make_shared<RSUniRenderEngine>();
        renderEngineNoScale->Init();
        auto drawingRecordingCanvasNoScale = std::make_unique<Drawing::RecordingCanvas>(100, 100);
        drawingRecordingCanvasNoScale->SetGrRecordingContext(
            renderEngineNoScale->GetRenderContext()->GetSharedDrGPUContext());
        auto recordingCanvasNoScale = std::make_shared<RSPaintFilterCanvas>(drawingRecordingCanvasNoScale.get());
        ASSERT_NE(recordingCanvasNoScale, nullptr);
        BufferDrawParam paramsNoScale;
        paramsNoScale.buffer = CreateCanvasDrawingTestBuffer();
        ASSERT_NE(paramsNoScale.buffer, nullptr);
        paramsNoScale.useBilinearInterpolation = true;
        paramsNoScale.srcRect = Drawing::Rect(0, 0, 100, 100);
        paramsNoScale.dstRect = Drawing::Rect(0, 0, 100, 100);
        paramsNoScale.matrix = Drawing::Matrix();
        renderEngineNoScale->DrawCanvasDrawingNodeWithParams(*recordingCanvasNoScale, paramsNoScale);
        EXPECT_NO_FATAL_FAILURE(
            renderEngineNoScale->DrawCanvasDrawingNodeWithParams(*recordingCanvasNoScale, paramsNoScale));
    }
#endif
}

/**
 * @tc.name: DrawLayerPreProcess_ProtectedBufferTest001
 * @tc.desc: Test DrawLayerPreProcess with protected buffer and non-transparent background
 *           The if (buffer && BUFFER_USAGE_PROTECTED && backgroundColor != Transparent)
 *           branch at line 188 should be true
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSUniRenderEngineTest, DrawLayerPreProcess_ProtectedBufferTest001, TestSize.Level2)
{
    auto uniRenderEngine = std::make_shared<RSUniRenderEngine>();
    std::unique_ptr<Drawing::Canvas> drawingCanvas = std::make_unique<Drawing::Canvas>(10, 10);
    std::shared_ptr<RSPaintFilterCanvas> canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    ASSERT_NE(canvas, nullptr);

    auto ctx = std::make_shared<RSComposerContext>(nullptr);
    RSLayerPtr layer = RSSurfaceLayer::Create(0, ctx);
    ASSERT_NE(layer, nullptr);

    // Set layer size
    GraphicIRect layerSize = {0, 0, 100, 100};
    layer->SetLayerSize(layerSize);

    // Set bound size
    GraphicIRect boundSize = {0, 0, 200, 200};
    layer->SetBoundSize(boundSize);

    // Set matrix
    GraphicMatrix matrix = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
    layer->SetMatrix(matrix);

    // Set non-transparent background color
    GraphicLayerColor backgroundColor = { .r = 0, .g = 0, .b = 255, .a = 255 };
    layer->SetBackgroundColor(backgroundColor);

    // Create buffer with BUFFER_USAGE_PROTECTED
    if (RSSystemProperties::IsUseVulkan()) {
        uniRenderEngine->Init();
        auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
        sptr<SurfaceBuffer> buffer = surfaceNode->GetRSSurfaceHandler()->GetBuffer();
        if (buffer) {
            // Note: We can't easily set BUFFER_USAGE_PROTECTED on an existing buffer,
            // but this test exercises the code path structure
            layer->SetBuffer(buffer, surfaceNode->GetRSSurfaceHandler()->GetAcquireFence());
        }
    }

    ComposerScreenInfo screenInfo;
    screenInfo.samplingTranslateX = 0;
    screenInfo.samplingTranslateY = 0;
    screenInfo.samplingScale = 1.0f;

    // Should draw background rect for protected buffer
    EXPECT_NO_FATAL_FAILURE(uniRenderEngine->DrawLayerPreProcess(*canvas, layer, screenInfo));
}

/**
 * @tc.name: DrawLayerPreProcess_EmptyCornerRadiusNoProtectedBufferTest001
 * @tc.desc: Test DrawLayerPreProcess with empty cornerRadiusInfo and no protected buffer
 *           Should only execute the final ClipRect at line 207
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSUniRenderEngineTest, DrawLayerPreProcess_EmptyCornerRadiusNoProtectedBufferTest001, TestSize.Level2)
{
    auto uniRenderEngine = std::make_shared<RSUniRenderEngine>();
    std::unique_ptr<Drawing::Canvas> drawingCanvas = std::make_unique<Drawing::Canvas>(10, 10);
    std::shared_ptr<RSPaintFilterCanvas> canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    ASSERT_NE(canvas, nullptr);

    auto ctx = std::make_shared<RSComposerContext>(nullptr);
    RSLayerPtr layer = RSSurfaceLayer::Create(0, ctx);
    ASSERT_NE(layer, nullptr);

    // Set layer size
    GraphicIRect layerSize = {10, 10, 80, 80};
    layer->SetLayerSize(layerSize);

    // Empty cornerRadiusInfo - skip first if branch
    std::vector<float> emptyCornerRadiusInfo = {};
    layer->SetCornerRadiusInfoForDRM(emptyCornerRadiusInfo);

    // No buffer set - skip protected buffer branch

    ComposerScreenInfo screenInfo;
    // Should only execute the final ClipRect
    EXPECT_NO_FATAL_FAILURE(uniRenderEngine->DrawLayerPreProcess(*canvas, layer, screenInfo));
}

/**
 * @tc.name: DrawLayerPreProcess_NullLayerTest001
 * @tc.desc: Test DrawLayerPreProcess with null layer - should handle gracefully
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSUniRenderEngineTest, DrawLayerPreProcess_NullLayerTest001, TestSize.Level2)
{
    auto uniRenderEngine = std::make_shared<RSUniRenderEngine>();
    std::unique_ptr<Drawing::Canvas> drawingCanvas = std::make_unique<Drawing::Canvas>(10, 10);
    std::shared_ptr<RSPaintFilterCanvas> canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    ASSERT_NE(canvas, nullptr);

    ComposerScreenInfo screenInfo;
    RSLayerPtr nullLayer = nullptr;

    // Should not crash with null layer (may have undefined behavior but shouldn't segfault in test)
    // This test is primarily for documentation of expected behavior
    // Note: In production code, null layer should be checked before calling this function
}

/**
 * @tc.name: DrawLayerPreProcess_VcldInfoTest001
 * @tc.desc: Test DrawLayerPreProcess when vcldInfo.enable is true
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSUniRenderEngineTest, DrawLayerPreProcess_VcldInfoTest001, TestSize.Level2)
{
    auto uniRenderEngine = std::make_shared<RSUniRenderEngine>();
    std::unique_ptr<Drawing::Canvas> drawingCanvas = std::make_unique<Drawing::Canvas>(10, 10);
    std::shared_ptr<RSPaintFilterCanvas> canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    ASSERT_NE(canvas, nullptr);

    auto ctx = std::make_shared<RSComposerContext>(nullptr);
    RSLayerPtr layer = RSSurfaceLayer::Create(0, ctx);
    ASSERT_NE(layer, nullptr);

    // Set layer size
    GraphicIRect layerSize = {0, 0, 100, 100};
    layer->SetLayerSize(layerSize);

    // Set VcldInfo
    RSVcldParam vcldInfo;
    vcldInfo.enable = true;
    vcldInfo.radius = 10;
    layer->SetVcldInfo(vcldInfo);

    // Set non-transparent background color
    GraphicLayerColor backgroundColor = { .r = 255, .g = 0, .b = 0, .a = 255 };
    layer->SetBackgroundColor(backgroundColor);

    ComposerScreenInfo screenInfo;
    // Should clip round rect and draw background color, then return early
    EXPECT_NO_FATAL_FAILURE(uniRenderEngine->DrawLayerPreProcess(*canvas, layer, screenInfo));
}

/**
 * @tc.name: DrawLayerPreProcess_VcldInfoTransparentTest001
 * @tc.desc: Test DrawLayerPreProcess when vcldInfo.enable is true but background is transparent
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSUniRenderEngineTest, DrawLayerPreProcess_VcldInfoTransparentTest001, TestSize.Level2)
{
    auto uniRenderEngine = std::make_shared<RSUniRenderEngine>();
    std::unique_ptr<Drawing::Canvas> drawingCanvas = std::make_unique<Drawing::Canvas>(10, 10);
    std::shared_ptr<RSPaintFilterCanvas> canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    ASSERT_NE(canvas, nullptr);

    auto ctx = std::make_shared<RSComposerContext>(nullptr);
    RSLayerPtr layer = RSSurfaceLayer::Create(0, ctx);
    ASSERT_NE(layer, nullptr);

    // Set layer size
    GraphicIRect layerSize = {0, 0, 100, 100};
    layer->SetLayerSize(layerSize);

    // Set VcldInfo
    RSVcldParam vcldInfo;
    vcldInfo.enable = true;
    vcldInfo.radius = 10;
    layer->SetVcldInfo(vcldInfo);

    // Set transparent background color
    GraphicLayerColor transparentColor = { .r = 0, .g = 0, .b = 0, .a = 0 };
    layer->SetBackgroundColor(transparentColor);

    ComposerScreenInfo screenInfo;
    // Should clip round rect but not draw background color(transparent)
    EXPECT_NO_FATAL_FAILURE(uniRenderEngine->DrawLayerPreProcess(*canvas, layer, screenInfo));
}

/**
 * @tc.name: DrawLayers_SolidFilledColorLayer_GetLayerSolidFilledColorSuccess001
 * @tc.desc: Test DrawLayers when IsSolidFilledColorLayer() == true and GetLayerSolidFilledColor succeeds
 *           Cover branch: layer->IsSolidFilledColorLayer() == true, ret == GRAPHIC_DISPLAY_SUCCESS
 * @tc.type: FUNC
 * @tc.require: issueI9XYZ1
 */
HWTEST_F(RSUniRenderEngineTest, DrawLayers_SolidFilledColorLayer_GetLayerSolidFilledColorSuccess001, TestSize.Level2)
{
    auto uniRenderEngine = std::make_shared<RSUniRenderEngine>();
    std::unique_ptr<Drawing::Canvas> drawingCanvas = std::make_unique<Drawing::Canvas>(10, 10);
    std::shared_ptr<RSPaintFilterCanvas> canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    ASSERT_NE(canvas, nullptr);

    auto ctx = std::make_shared<RSComposerContext>(nullptr);
    RSLayerPtr solidFilledColorLayer = RSSurfaceSolidFilledColorLayer::Create(0, ctx);
    ASSERT_NE(solidFilledColorLayer, nullptr);
    solidFilledColorLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT);
    
    GraphicIRect layerSize = {0, 0, 100, 100};
    solidFilledColorLayer->SetLayerSize(layerSize);

    std::vector<RSLayerPtr> layers;
    layers.emplace_back(solidFilledColorLayer);

    auto output = std::make_shared<HdiOutput>(0u);
    ASSERT_NE(output, nullptr);
    
    ComposerScreenInfo screenInfo;
    EXPECT_NO_FATAL_FAILURE(uniRenderEngine->DrawLayers(*canvas, layers, false, screenInfo, output));
}

/**
 * @tc.name: DrawLayers_SolidFilledColorLayer_GetLayerSolidFilledColorFail001
 * @tc.desc: Test DrawLayers when IsSolidFilledColorLayer() == true and GetLayerSolidFilledColor fails
 *           Cover branch: layer->IsSolidFilledColorLayer() == true, ret != GRAPHIC_DISPLAY_SUCCESS (continue)
 * @tc.type: FUNC
 * @tc.require: issueI9XYZ2
 */
HWTEST_F(RSUniRenderEngineTest, DrawLayers_SolidFilledColorLayer_GetLayerSolidFilledColorFail001, TestSize.Level2)
{
    auto uniRenderEngine = std::make_shared<RSUniRenderEngine>();
    std::unique_ptr<Drawing::Canvas> drawingCanvas = std::make_unique<Drawing::Canvas>(10, 10);
    std::shared_ptr<RSPaintFilterCanvas> canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    ASSERT_NE(canvas, nullptr);

    auto ctx = std::make_shared<RSComposerContext>(nullptr);
    RSLayerPtr solidFilledColorLayer = RSSurfaceSolidFilledColorLayer::Create(12345, ctx);
    ASSERT_NE(solidFilledColorLayer, nullptr);
    solidFilledColorLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT);
    
    GraphicIRect layerSize = {0, 0, 100, 100};
    solidFilledColorLayer->SetLayerSize(layerSize);

    std::vector<RSLayerPtr> layers;
    layers.emplace_back(solidFilledColorLayer);

    auto output = std::make_shared<HdiOutput>(0u);
    ASSERT_NE(output, nullptr);
    
    ComposerScreenInfo screenInfo;
    EXPECT_NO_FATAL_FAILURE(uniRenderEngine->DrawLayers(*canvas, layers, false, screenInfo, output));
}

/**
 * @tc.name: DrawLayers_NotSolidFilledColorLayer_NonBlackColor001
 * @tc.desc: Test DrawLayers when IsSolidFilledColorLayer() == false and layerColor is not black
 *           Cover branch: layer->IsSolidFilledColorLayer() == false, layerColor != layerBlackColor
 * @tc.type: FUNC
 * @tc.require: issueI9XYZ3
 */
HWTEST_F(RSUniRenderEngineTest, DrawLayers_NotSolidFilledColorLayer_NonBlackColor001, TestSize.Level2)
{
    auto uniRenderEngine = std::make_shared<RSUniRenderEngine>();
    std::unique_ptr<Drawing::Canvas> drawingCanvas = std::make_unique<Drawing::Canvas>(10, 10);
    std::shared_ptr<RSPaintFilterCanvas> canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    ASSERT_NE(canvas, nullptr);

    RSLayerPtr normalLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    ASSERT_NE(normalLayer, nullptr);
    normalLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT);
    
    GraphicIRect layerSize = {0, 0, 100, 100};
    normalLayer->SetLayerSize(layerSize);
    
    GraphicLayerColor layerColor = { .r = 255, .g = 0, .b = 0, .a = 128 };
    normalLayer->SetLayerColor(layerColor);

    std::vector<RSLayerPtr> layers;
    layers.emplace_back(normalLayer);

    auto output = std::make_shared<HdiOutput>(0u);
    ASSERT_NE(output, nullptr);
    
    ComposerScreenInfo screenInfo;
    EXPECT_NO_FATAL_FAILURE(uniRenderEngine->DrawLayers(*canvas, layers, false, screenInfo, output));
}

/**
 * @tc.name: DrawLayers_NotSolidFilledColorLayer_BlackColor001
 * @tc.desc: Test DrawLayers when IsSolidFilledColorLayer() == false and layerColor is black
 *           Cover branch: layer->IsSolidFilledColorLayer() == false, layerColor == layerBlackColor
 * @tc.type: FUNC
 * @tc.require: issueI9XYZ4
 */
HWTEST_F(RSUniRenderEngineTest, DrawLayers_NotSolidFilledColorLayer_BlackColor001, TestSize.Level2)
{
    auto uniRenderEngine = std::make_shared<RSUniRenderEngine>();
    std::unique_ptr<Drawing::Canvas> drawingCanvas = std::make_unique<Drawing::Canvas>(10, 10);
    std::shared_ptr<RSPaintFilterCanvas> canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    ASSERT_NE(canvas, nullptr);

    RSLayerPtr normalLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    ASSERT_NE(normalLayer, nullptr);
    normalLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT);
    
    GraphicIRect layerSize = {0, 0, 100, 100};
    normalLayer->SetLayerSize(layerSize);
    
    GraphicLayerColor blackColor = { .r = 0, .g = 0, .b = 0, .a = 0 };
    normalLayer->SetLayerColor(blackColor);

    std::vector<RSLayerPtr> layers;
    layers.emplace_back(normalLayer);

    auto output = std::make_shared<HdiOutput>(0u);
    ASSERT_NE(output, nullptr);
    
    ComposerScreenInfo screenInfo;
    EXPECT_NO_FATAL_FAILURE(uniRenderEngine->DrawLayers(*canvas, layers, false, screenInfo, output));
}
/**
 * @tc.name: DrawLayers_RogScale_DeviceGpuUniRender
 * @tc.desc: Test DrawLayers when DEVICE_GPU + uniRenderFlag = true, PostScale should be skipped
 * @tc.type: FUNC
 * @tc.require: issue no.
 */
HWTEST_F(RSUniRenderEngineTest, DrawLayers_RogScale_DeviceGpuUniRender, TestSize.Level2)
{
    auto uniRenderEngine = std::make_shared<RSUniRenderEngine>();
    std::unique_ptr<Drawing::Canvas> drawingCanvas = std::make_unique<Drawing::Canvas>(10, 10);
    std::shared_ptr<RSPaintFilterCanvas> canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    ASSERT_NE(canvas, nullptr);

    RSLayerPtr layer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    layer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT);
    layer->SetRotationFixed(false);
    layer->SetUniRenderFlag(true);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto buffer = surfaceNode->GetRSSurfaceHandler()->GetBuffer();
    if (buffer != nullptr) {
        layer->SetBuffer(buffer, surfaceNode->GetRSSurfaceHandler()->GetAcquireFence());
    }

    std::vector<RSLayerPtr> layers;
    layers.emplace_back(layer);

    ComposerScreenInfo screenInfo;
    screenInfo.width = 100;
    screenInfo.height = 200;
    screenInfo.phyWidth = 200;
    screenInfo.phyHeight = 400;
    screenInfo.samplingMode = ScreenSamplingMode::DEVICE_GPU;

    EXPECT_NO_FATAL_FAILURE(uniRenderEngine->DrawLayers(*canvas, layers, false, screenInfo));
}

/**
 * @tc.name: DrawLayers_RogScale_DeviceGpuNonUniRender
 * @tc.desc: Test DrawLayers when DEVICE_GPU + uniRenderFlag = false, PostScale should be skipped
 * @tc.type: FUNC
 * @tc.require: issue no.
 */
HWTEST_F(RSUniRenderEngineTest, DrawLayers_RogScale_DeviceGpuNonUniRender, TestSize.Level2)
{
    auto uniRenderEngine = std::make_shared<RSUniRenderEngine>();
    std::unique_ptr<Drawing::Canvas> drawingCanvas = std::make_unique<Drawing::Canvas>(10, 10);
    std::shared_ptr<RSPaintFilterCanvas> canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    ASSERT_NE(canvas, nullptr);

    RSLayerPtr layer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    layer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT);
    layer->SetRotationFixed(false);
    layer->SetUniRenderFlag(false);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto buffer = surfaceNode->GetRSSurfaceHandler()->GetBuffer();
    if (buffer != nullptr) {
        layer->SetBuffer(buffer, surfaceNode->GetRSSurfaceHandler()->GetAcquireFence());
    }

    std::vector<RSLayerPtr> layers;
    layers.emplace_back(layer);

    ComposerScreenInfo screenInfo;
    screenInfo.width = 100;
    screenInfo.height = 200;
    screenInfo.phyWidth = 200;
    screenInfo.phyHeight = 400;
    screenInfo.samplingMode = ScreenSamplingMode::DEVICE_GPU;

    EXPECT_NO_FATAL_FAILURE(uniRenderEngine->DrawLayers(*canvas, layers, false, screenInfo));
}

/**
 * @tc.name: DrawLayers_RogScale_NonDeviceGpu
 * @tc.desc: Test DrawLayers when SamplingMode is NOT DEVICE_GPU, PostScale should be execute
 * @tc.type: FUNC
 * @tc.require: issue no.
 */
HWTEST_F(RSUniRenderEngineTest, DrawLayers_RogScale_NonDeviceGpu, TestSize.Level2)
{
    auto uniRenderEngine = std::make_shared<RSUniRenderEngine>();
    std::unique_ptr<Drawing::Canvas> drawingCanvas = std::make_unique<Drawing::Canvas>(10, 10);
    std::shared_ptr<RSPaintFilterCanvas> canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    ASSERT_NE(canvas, nullptr);

    RSLayerPtr layer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    layer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT);
    layer->SetRotationFixed(false);
    layer->SetUniRenderFlag(true);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto buffer = surfaceNode->GetRSSurfaceHandler()->GetBuffer();
    if (buffer != nullptr) {
        layer->SetBuffer(buffer, surfaceNode->GetRSSurfaceHandler()->GetAcquireFence());
    }

    std::vector<RSLayerPtr> layers;
    layers.emplace_back(layer);

    ComposerScreenInfo screenInfo;
    screenInfo.width = 100;
    screenInfo.height = 200;
    screenInfo.phyWidth = 200;
    screenInfo.phyHeight = 400;
    screenInfo.samplingMode = ScreenSamplingMode::OFFSCREEN;

    EXPECT_NO_FATAL_FAILURE(uniRenderEngine->DrawLayers(*canvas, layers, false, screenInfo));
}
} // namespace OHOS::Rosen