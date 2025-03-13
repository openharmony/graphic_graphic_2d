/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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
#include "recording/recording_canvas.h"
#include "rs_test_util.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSBaseRenderEngineUnitTest : public testing::Test {
public:
    void SetUp() override;
    void TearDown() override;
    static void SetUpTestCase();
    static void TearDownTestCase();
};

void RSBaseRenderEngineUnitTest::SetUpTestCase()
{
    RSTestUtil::InitRenderNodeGC();
}
void RSBaseRenderEngineUnitTest::SetUp() {}
void RSBaseRenderEngineUnitTest::TearDown() {}
void RSBaseRenderEngineUnitTest::TearDownTestCase() {}

/**
 * @tc.name: NeedForceCPU001_test
 * @tc.desc: Test NeedForceCPU
 * @tc.type: FUNC
 * @tc.require: issueI6SDSD
 */
HWTEST(RSBaseRenderEngineUnitTest, NeedForceCPU001_test, TestSize.Level1)
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
 * @tc.name: NeedForceCPU002_test
 * @tc.desc: Test NeedForceCPU
 * @tc.type: FUNC
 * @tc.require: issueI6SDSD
 */
HWTEST(RSBaseRenderEngineUnitTest, NeedForceCPU002_test, TestSize.Level1)
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
 * @tc.name: SetHighContrast_001_test
 * @tc.desc: Test SetHighContrast, input false, expect RSBaseRenderEngine::IsHighContrastEnabled() to be same as input
 * @tc.type: FUNC
 * @tc.require: issueI6GFGG
 */
HWTEST(RSBaseRenderEngineUnitTest, SetHighContrast_001_test, TestSize.Level1)
{
    bool contrastEnabled = false;
    RSBaseRenderEngine::SetHighContrast(contrastEnabled);
    ASSERT_EQ(contrastEnabled, RSBaseRenderEngine::IsHighContrastEnabled());
}

/**
 * @tc.name: SetHighContrast_002_test
 * @tc.desc: Test SetHighContrast, input true, expect RSBaseRenderEngine::IsHighContrastEnabled() to be same as input
 * @tc.type: FUNC
 * @tc.require: issueI6GDFG
 */
HWTEST(RSBaseRenderEngineUnitTest, SetHighContrast_002_test, TestSize.Level1)
{
    bool contrastEnabled = true;
    RSBaseRenderEngine::SetHighContrast(contrastEnabled);
    ASSERT_EQ(contrastEnabled, RSBaseRenderEngine::IsHighContrastEnabled());
}

#ifdef RS_ENABLE_EGLIMAGE
HWTEST(RSBaseRenderEngineUnitTest, DrawImageRect_test, TestSize.Level1)
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
 * @tc.name: RegisterDeleteBufferListener001_test
 * @tc.desc: Test RegisterDeleteBufferListener
 * @tc.type: FUNC
 * @tc.require: issueI6FDFG
 */
HWTEST(RSBaseRenderEngineUnitTest, RegisterDeleteBufferListener001_test, TestSize.Level1)
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

/**
 * @tc.name: CreateEglImageFromBuffer001_test
 * @tc.desc: Test CreateEglImageFromBuffer
 * @tc.type: FUNC
 * @tc.require: issueI6FDFG
 */
HWTEST(RSBaseRenderEngineUnitTest, CreateEglImageFromBuffer001_test, TestSize.Level1)
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

#endif

#ifdef USE_VIDEO_PROCESSING_ENGINE
/**
 * @tc.name: ConvertColorGamutToDrawingColorSpace_test
 * @tc.desc: Test ConvertColorGamutToDrawingColorSpace
 * @tc.type: FUNC
 * @tc.require: issueI6GJ1Z
 */
HWTEST(RSBaseRenderEngineUnitTest, ConvertColorGamutToDrawingColorSpace_test, TestSize.Level1)
{
    std::shared_ptr<Drawing::ColorSpace> colorSpace;
    colorSpace = RSBaseRenderEngine::ConvertColorGamutToDrawingColorSpace(GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
    ASSERT_NE(colorSpace, nullptr);
    colorSpace = RSBaseRenderEngine::ConvertColorGamutToDrawingColorSpace(GRAPHIC_COLOR_GAMUT_ADOBE_RGB);
    ASSERT_NE(colorSpace, nullptr);
    colorSpace = RSBaseRenderEngine::ConvertColorGamutToDrawingColorSpace(GRAPHIC_COLOR_GAMUT_BT2020);
    ASSERT_NE(colorSpace, nullptr);
    colorSpace = RSBaseRenderEngine::ConvertColorGamutToDrawingColorSpace(GRAPHIC_COLOR_GAMUT_DISPLAY_BT2020);
    ASSERT_EQ(colorSpace, nullptr);
}

/**
 * @tc.name: ConvertDrawingColorSpaceToSpaceInfo_test
 * @tc.desc: Test ConvertDrawingColorSpaceToSpaceInfo
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST(RSBaseRenderEngineUnitTest, ConvertDrawingColorSpaceToSpaceInfo_test, TestSize.Level1)
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
 * @tc.name: GetCanvasColorSpace_test
 * @tc.desc: Test GetCanvasColorSpace
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST(RSBaseRenderEngineUnitTest, GetCanvasColorSpace_test, TestSize.Level1)
{
    std::unique_ptr<Drawing::Canvas> drawingCanvas = std::make_unique<Drawing::Canvas>(10, 10);
    std::shared_ptr<RSPaintFilterCanvas> canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    ASSERT_NE(canvas, nullptr);
    EXPECT_EQ(RSBaseRenderEngine::GetCanvasColorSpace(canvas.get()), nullptr);
}
#endif

/**
 * @tc.name: DrawDisplayNodeWithParams001_test
 * @tc.desc: Test DrawDisplayNodeWithParams
 * @tc.type: FUNC
 * @tc.require: issueI6FDFG
 */
HWTEST(RSBaseRenderEngineUnitTest, DrawDisplayNodeWithParams001_test, TestSize.Level1)
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

}