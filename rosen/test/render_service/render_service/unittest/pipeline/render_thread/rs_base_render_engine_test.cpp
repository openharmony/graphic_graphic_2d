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
#include "pipeline/render_thread/rs_base_render_engine.h"
#include "pipeline/render_thread/rs_render_engine.h"
#include "v2_1/cm_color_space.h"
#include "foundation/graphic/graphic_2d/rosen/test/render_service/render_service/unittest/pipeline/rs_test_util.h"
#include "recording/recording_canvas.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSBaseRenderEngineUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSBaseRenderEngineUnitTest::SetUpTestCase()
{
    RSTestUtil::InitRenderNodeGC();
}
void RSBaseRenderEngineUnitTest::TearDownTestCase() {}
void RSBaseRenderEngineUnitTest::SetUp() {}
void RSBaseRenderEngineUnitTest::TearDown() {}

/**
 * @tc.name: SetHighContrast_001
 * @tc.desc: Test SetHighContrast, input false, expect RSBaseRenderEngine::IsHighContrastEnabled() to be same as input
 * @tc.type: FUNC
 * @tc.require: issueI6GJ1Z
 */
HWTEST_F(RSBaseRenderEngineUnitTest, SetHighContrast_001, TestSize.Level1)
{
    bool contrastEnabled = false;
    RSBaseRenderEngine::SetHighContrast(contrastEnabled);
    ASSERT_EQ(contrastEnabled, RSBaseRenderEngine::IsHighContrastEnabled());
}

/**
 * @tc.name: SetHighContrast_002
 * @tc.desc: Test SetHighContrast, input true, expect RSBaseRenderEngine::IsHighContrastEnabled() to be same as input
 * @tc.type: FUNC
 * @tc.require: issueI6GJ1Z
 */
HWTEST_F(RSBaseRenderEngineUnitTest, SetHighContrast_002, TestSize.Level1)
{
    bool contrastEnabled = true;
    RSBaseRenderEngine::SetHighContrast(contrastEnabled);
    ASSERT_EQ(contrastEnabled, RSBaseRenderEngine::IsHighContrastEnabled());
}

/**
 * @tc.name: NeedForceCPU001
 * @tc.desc: Test NeedForceCPU
 * @tc.type: FUNC
 * @tc.require: issueI6QM6E
 */
HWTEST_F(RSBaseRenderEngineUnitTest, NeedForceCPU001, TestSize.Level1)
{
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto buffer = node->GetRSSurfaceHandler()->GetBuffer();

    std::vector<LayerInfoPtr> layers;
    layers.emplace_back(nullptr);
    bool ret = RSBaseRenderEngine::NeedForceCPU(layers);
    ASSERT_EQ(false, ret);

    layers.clear();
    LayerInfoPtr layer = HdiLayerInfo::CreateHdiLayerInfo();
    layers.emplace_back(layer);
    ret = RSBaseRenderEngine::NeedForceCPU(layers);
    ASSERT_EQ(false, ret);
}

/**
 * @tc.name: NeedForceCPU002
 * @tc.desc: Test NeedForceCPU
 * @tc.type: FUNC
 * @tc.require: issueI6QM6E
 */
HWTEST_F(RSBaseRenderEngineUnitTest, NeedForceCPU002, TestSize.Level1)
{
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto buffer = node->GetRSSurfaceHandler()->GetBuffer();

    std::vector<LayerInfoPtr> layers;
    LayerInfoPtr layer = HdiLayerInfo::CreateHdiLayerInfo();
    layer->SetBuffer(buffer, node->GetRSSurfaceHandler()->GetAcquireFence());
    layers.emplace_back(layer);
    bool ret = RSBaseRenderEngine::NeedForceCPU(layers);
    ASSERT_EQ(false, ret);

#ifndef RS_ENABLE_EGLIMAGE
    buffer->SetSurfaceBufferColorGamut(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    buffer->GetBufferHandle()->format = GraphicPixelFormat::GRAPHIC_PIXEL_FMT_YCBCR_420_SP;
    ret = RSBaseRenderEngine::NeedForceCPU(layers);
    ASSERT_EQ(true, ret);

    buffer->GetBufferHandle()->format = GraphicPixelFormat::GRAPHIC_PIXEL_FMT_YCRCB_420_SP;
    ret = RSBaseRenderEngine::NeedForceCPU(layers);
    ASSERT_EQ(true, ret);
#endif
}

/**
 * @tc.name: DrawDisplayNodeWithParams001
 * @tc.desc: Test DrawDisplayNodeWithParams
 * @tc.type: FUNC
 * @tc.require: issueI6QM6E
 */
HWTEST_F(RSBaseRenderEngineUnitTest, DrawDisplayNodeWithParams001, TestSize.Level1)
{
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(id, config);
    BufferDrawParam param;

    if (RSSystemProperties::IsUseVulkan()) {
        auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
        param.buffer = surfaceNode->GetRSSurfaceHandler()->GetBuffer();

        auto renderEngine = std::make_shared<RSRenderEngine>();
        renderEngine->Init(true);
        auto drawingRecordingCanvas = std::make_unique<Drawing::RecordingCanvas>(10, 10);
        drawingRecordingCanvas->SetGrRecordingContext(renderEngine->GetRenderContext()->GetSharedDrGPUContext());
        auto recordingCanvas = std::make_shared<RSPaintFilterCanvas>(drawingRecordingCanvas.get());
        ASSERT_NE(recordingCanvas, nullptr);
        renderEngine->DrawDisplayNodeWithParams(*recordingCanvas, *node, param);

        param.useCPU = true;
        renderEngine->DrawDisplayNodeWithParams(*recordingCanvas, *node, param);
    } else {
        auto renderEngine = std::make_shared<RSRenderEngine>();
        std::unique_ptr<Drawing::Canvas> drawingCanvas = std::make_unique<Drawing::Canvas>(10, 10);
        std::shared_ptr<RSPaintFilterCanvas> canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
        ASSERT_NE(canvas, nullptr);
        renderEngine->DrawDisplayNodeWithParams(*canvas, *node, param);

        param.useCPU = true;
        renderEngine->DrawDisplayNodeWithParams(*canvas, *node, param);
    }
}

#ifdef RS_ENABLE_EGLIMAGE
/**
 * @tc.name: CreateEglImageFromBuffer001
 * @tc.desc: Test CreateEglImageFromBuffer
 * @tc.type: FUNC
 * @tc.require: issueI6QM6E
 */
HWTEST_F(RSBaseRenderEngineUnitTest, CreateEglImageFromBuffer001, TestSize.Level1)
{
    if (!RSSystemProperties::IsUseVulkan()) {
        auto renderEngine = std::make_shared<RSRenderEngine>();
        renderEngine->Init();
        auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
        std::unique_ptr<Drawing::Canvas> drawingCanvas = std::make_unique<Drawing::Canvas>(10, 10);
        std::shared_ptr<RSPaintFilterCanvas> canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
        auto img = renderEngine->CreateEglImageFromBuffer(*canvas, nullptr, nullptr);
        ASSERT_EQ(nullptr, img);
        [[maybe_unused]] auto grContext = canvas->GetGPUContext();
        grContext = nullptr;
        img = renderEngine->CreateEglImageFromBuffer(*canvas, node->GetRSSurfaceHandler()->GetBuffer(), nullptr);
        ASSERT_EQ(nullptr, img);
    }
}

HWTEST_F(RSBaseRenderEngineUnitTest, DrawImageRect, TestSize.Level1)
{
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintCanvase(&canvas);
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    BufferDrawParam params;
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    params.buffer = surfaceNode->GetRSSurfaceHandler()->GetBuffer();
    Drawing::Rect srcRect(0.0f, 0.0f, 10, 20);
    Drawing::Rect dstRect(0.0f, 0.0f, 10, 20);
    Drawing::Brush paint;
    params.srcRect = srcRect;
    params.dstRect = dstRect;
    params.paint = paint;
    Drawing::SamplingOptions samplingOptions(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NEAREST);
    auto renderEngine = std::make_shared<RSRenderEngine>();
    ASSERT_NE(renderEngine, nullptr);
    renderEngine->DrawImageRect(paintCanvase, image, params, samplingOptions);
    ASSERT_NE(image, nullptr);
}

/**
 * @tc.name: RegisterDeleteBufferListener001
 * @tc.desc: Test RegisterDeleteBufferListener
 * @tc.type: FUNC
 * @tc.require: issueI6QM6E
 */
HWTEST_F(RSBaseRenderEngineUnitTest, RegisterDeleteBufferListener001, TestSize.Level1)
{
    auto renderEngine = std::make_shared<RSRenderEngine>();
    renderEngine->RegisterDeleteBufferListener(nullptr, true);
    renderEngine->RegisterDeleteBufferListener(nullptr, false);
#ifdef RS_ENABLE_VK
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(node, nullptr);
    renderEngine->RegisterDeleteBufferListener(node->GetRSSurfaceHandler()->GetConsumer(), true);
    renderEngine->RegisterDeleteBufferListener(node->GetRSSurfaceHandler()->GetConsumer(), false);
#endif
}
#endif

#ifdef USE_VIDEO_PROCESSING_ENGINE
/**
 * @tc.name: ConvertColorGamutToDrawingColorSpace
 * @tc.desc: Test ConvertColorGamutToDrawingColorSpace
 * @tc.type: FUNC
 * @tc.require: issueI6GJ1Z
 */
HWTEST_F(RSBaseRenderEngineUnitTest, ConvertColorGamutToDrawingColorSpace, TestSize.Level1)
{
    std::shared_ptr<Drawing::ColorSpace> colorSpace;
    colorSpace = RSBaseRenderEngine::ConvertColorGamutToDrawingColorSpace(GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
    ASSERT_NE(colorSpace, nullptr);
    colorSpace = RSBaseRenderEngine::ConvertColorGamutToDrawingColorSpace(GRAPHIC_COLOR_GAMUT_ADOBE_RGB);
    ASSERT_NE(colorSpace, nullptr);
    colorSpace = RSBaseRenderEngine::ConvertColorGamutToDrawingColorSpace(GRAPHIC_COLOR_GAMUT_BT2020);
    ASSERT_NE(colorSpace, nullptr);
    colorSpace = RSBaseRenderEngine::ConvertColorGamutToDrawingColorSpace(GRAPHIC_COLOR_GAMUT_DISPLAY_BT2020);
    ASSERT_NE(colorSpace, nullptr);
}

/**
 * @tc.name: ConvertDrawingColorSpaceToSpaceInfo
 * @tc.desc: Test ConvertDrawingColorSpaceToSpaceInfo
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderEngineUnitTest, ConvertDrawingColorSpaceToSpaceInfo, TestSize.Level1)
{
    using namespace HDI::Display::Graphic::Common::V1_0;
    std::shared_ptr<Drawing::ColorSpace> colorSpace;
    CM_ColorSpaceInfo colorSpaceInfo;

    colorSpace = Drawing::ColorSpace::CreateSRGB();
    EXPECT_TRUE(RSBaseRenderEngine::ConvertDrawingColorSpaceToSpaceInfo(colorSpace, colorSpaceInfo));
    EXPECT_EQ(colorSpaceInfo.primaries, COLORPRIMARIES_SRGB);
    EXPECT_EQ(colorSpaceInfo.transfunc, TRANSFUNC_SRGB);

    colorSpace = Drawing::ColorSpace::CreateRGB(
        Drawing::CMSTransferFuncType::SRGB, Drawing::CMSMatrixType::DCIP3);
    EXPECT_TRUE(RSBaseRenderEngine::ConvertDrawingColorSpaceToSpaceInfo(colorSpace, colorSpaceInfo));
    EXPECT_EQ(colorSpaceInfo.primaries, COLORPRIMARIES_P3_D65);
    EXPECT_EQ(colorSpaceInfo.transfunc, TRANSFUNC_SRGB);

    colorSpace = Drawing::ColorSpace::CreateRGB(
        Drawing::CMSTransferFuncType::REC2020, Drawing::CMSMatrixType::REC2020);
    EXPECT_FALSE(RSBaseRenderEngine::ConvertDrawingColorSpaceToSpaceInfo(colorSpace, colorSpaceInfo));
}

/**
 * @tc.name: GetCanvasColorSpace
 * @tc.desc: Test GetCanvasColorSpace
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderEngineUnitTest, GetCanvasColorSpace, TestSize.Level1)
{
    std::unique_ptr<Drawing::Canvas> drawingCanvas = std::make_unique<Drawing::Canvas>(10, 10);
    std::shared_ptr<RSPaintFilterCanvas> canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    ASSERT_NE(canvas, nullptr);
    EXPECT_EQ(RSBaseRenderEngine::GetCanvasColorSpace(canvas.get()), nullptr);
}

/**
 * @tc.name: CreateImageFromBuffer
 * @tc.desc: Test CreateImageFromBuffer
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderEngineUnitTest, CreateImageFromBuffer, TestSize.Level1)
{
    auto renderEngine = std::make_shared<RSRenderEngine>();
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintCanvase(&canvas);
    BufferDrawParam params;
    VideoInfo videoInfo;
    EXPECT_EQ(renderEngine->CreateImageFromBuffer(paintCanvase, params, videoInfo), nullptr);
}
#endif

/**
 * @tc.name: NeedBilinearInterpolation
 * @tc.desc: Test NeedBilinearInterpolation
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderEngineUnitTest, NeedBilinearInterpolation, TestSize.Level1)
{
    BufferDrawParam params;
    params.useBilinearInterpolation = false;
    Drawing::Matrix matrix;
    ASSERT_FALSE(RSRenderEngine::NeedBilinearInterpolation(params, matrix));
    params.useBilinearInterpolation = true;
    params.srcRect = Drawing::Rect(0.0f, 0.0f, 10, 20);
    params.dstRect = Drawing::Rect(0.0f, 0.0f, 15, 20);
    ASSERT_TRUE(RSRenderEngine::NeedBilinearInterpolation(params, matrix));
    params.srcRect = Drawing::Rect(0.0f, 0.0f, 10, 20);
    params.dstRect = Drawing::Rect(0.0f, 0.0f, 10, 25);
    ASSERT_TRUE(RSRenderEngine::NeedBilinearInterpolation(params, matrix));
    params.srcRect = Drawing::Rect(0.0f, 0.0f, 10, 20);
    params.dstRect = Drawing::Rect(0.0f, 0.0f, 10, 20);
    matrix.Set(Drawing::Matrix::SCALE_X, 1);
    matrix.Set(Drawing::Matrix::SCALE_Y, 1);
    ASSERT_FALSE(RSRenderEngine::NeedBilinearInterpolation(params, matrix));
    matrix.Set(Drawing::Matrix::SCALE_X, 1);
    matrix.Set(Drawing::Matrix::SCALE_Y, 0);
    ASSERT_TRUE(RSRenderEngine::NeedBilinearInterpolation(params, matrix));
    matrix.Set(Drawing::Matrix::SCALE_X, 0);
    matrix.Set(Drawing::Matrix::SCALE_Y, 1);
    ASSERT_TRUE(RSRenderEngine::NeedBilinearInterpolation(params, matrix));
    matrix.Set(Drawing::Matrix::SCALE_X, 0);
    matrix.Set(Drawing::Matrix::SCALE_Y, 0);
    matrix.Set(Drawing::Matrix::SKEW_X, 1);
    matrix.Set(Drawing::Matrix::SKEW_Y, 1);
    ASSERT_FALSE(RSRenderEngine::NeedBilinearInterpolation(params, matrix));
    matrix.Set(Drawing::Matrix::SKEW_X, 1);
    matrix.Set(Drawing::Matrix::SKEW_Y, 0);
    ASSERT_TRUE(RSRenderEngine::NeedBilinearInterpolation(params, matrix));
    matrix.Set(Drawing::Matrix::SKEW_X, 0);
    matrix.Set(Drawing::Matrix::SKEW_Y, 1);
    ASSERT_TRUE(RSRenderEngine::NeedBilinearInterpolation(params, matrix));
    matrix.Set(Drawing::Matrix::SKEW_X, 0);
    matrix.Set(Drawing::Matrix::SKEW_Y, 1);
    matrix.Set(Drawing::Matrix::SCALE_X, 0);
    matrix.Set(Drawing::Matrix::SCALE_Y, 1);
    ASSERT_TRUE(RSRenderEngine::NeedBilinearInterpolation(params, matrix));
}

/**
 * @tc.name: CheckAndVerifyDamageRegion001
 * @tc.desc: Test CheckAndVerifyDamageRegion
 * @tc.type: FUNC
 * @tc.require:issueICEK44
 */
HWTEST_F(RSBaseRenderEngineUnitTest, CheckAndVerifyDamageRegion001, TestSize.Level1)
{
    auto renderFrame = std::make_unique<RSRenderFrame>(nullptr, nullptr);
    ASSERT_NE(renderFrame, nullptr);
    std::vector<RectI> rects = {};
    RectI rectI{0, 0, 1, 1};
    ASSERT_EQ(rects, renderFrame->CheckAndVerifyDamageRegion(rects, rectI));
    rects.push_back(rectI);
    ASSERT_EQ(rects, renderFrame->CheckAndVerifyDamageRegion(rects, rectI));
}
}
