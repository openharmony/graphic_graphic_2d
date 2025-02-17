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
#include "recording/recording_canvas.h"
#include "rs_test_util.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSBaseRenderEngineExtUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSBaseRenderEngineExtUnitTest::SetUpTestCase()
{
    RSTestUtil::InitRenderNodeGC();
}
void RSBaseRenderEngineExtUnitTest::TearDownTestCase() {}
void RSBaseRenderEngineExtUnitTest::SetUp() {}
void RSBaseRenderEngineExtUnitTest::TearDown() {}

/**
 * @tc.name: SetHighContrast_01
 * @tc.desc: Test SetHighContrast, input false, expect RSBaseRenderEngine::IsHighContrastEnabled() to be same as input
 * @tc.type: FUNC
 * @tc.require: issueI6GJ1Z
 */
HWTEST(RSBaseRenderEngineExtUnitTest, SetHighContrast_01, TestSize.Level1)
{
    bool contrastEnabled = false;
    RSBaseRenderEngine::SetHighContrast(contrastEnabled);
    ASSERT_EQ(contrastEnabled, RSBaseRenderEngine::IsHighContrastEnabled());
}

/**
 * @tc.name: SetHighContrast_02
 * @tc.desc: Test SetHighContrast, input true, expect RSBaseRenderEngine::IsHighContrastEnabled() to be same as input
 * @tc.type: FUNC
 * @tc.require: issueI6GJ1Z
 */
HWTEST(RSBaseRenderEngineExtUnitTest, SetHighContrast_02, TestSize.Level1)
{
    bool contrastEnabled = true;
    RSBaseRenderEngine::SetHighContrast(contrastEnabled);
    ASSERT_EQ(contrastEnabled, RSBaseRenderEngine::IsHighContrastEnabled());
}

/**
 * @tc.name: DrawDisplayNodeWithParams01
 * @tc.desc: Test DrawDisplayNodeWithParams
 * @tc.type: FUNC
 * @tc.require: issueI6QM6E
 */
HWTEST(RSBaseRenderEngineExtUnitTest, DrawDisplayNodeWithParams01, TestSize.Level1)
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

/**
 * @tc.name: CheckIsHdrSurfaceBuffer01
 * @tc.desc: Test CheckIsHdrSurfaceBuffer
 * @tc.type: FUNC
 * @tc.require: issueI6QM6E
 */
HWTEST(RSBaseRenderEngineExtUnitTest, CheckIsHdrSurfaceBuffer01, TestSize.Level1)
{
    auto renderEngine = std::make_shared<RSRenderEngine>();
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    if (node->GetRSSurfaceHandler() == nullptr) {
        return;
    }
    auto buffer = node->GetRSSurfaceHandler()->GetBuffer();
    if (buffer == nullptr || buffer->GetBufferHandle() == nullptr) {
        return;
    }
    buffer->GetBufferHandle()->format = GraphicPixelFormat::GRAPHIC_PIXEL_FMT_YCBCR_420_SP;
    bool ret = renderEngine->CheckIsHdrSurfaceBuffer(buffer);
    ASSERT_EQ(ret, false);
    buffer->GetBufferHandle()->format = GraphicPixelFormat::GRAPHIC_PIXEL_FMT_YCBCR_P010;
    ret = renderEngine->CheckIsHdrSurfaceBuffer(buffer);
    ASSERT_EQ(ret, false);
}

#ifdef RS_ENABLE_EGLIMAGE
/**
 * @tc.name: CreateEglImageFromBuffer01
 * @tc.desc: Test CreateEglImageFromBuffer
 * @tc.type: FUNC
 * @tc.require: issueI6QM6E
 */
HWTEST(RSBaseRenderEngineExtUnitTest, CreateEglImageFromBuffer01, TestSize.Level1)
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

HWTEST(RSBaseRenderEngineExtUnitTest, DrawImageRect, TestSize.Level1)
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
 * @tc.name: RegisterDeleteBufferListener01
 * @tc.desc: Test RegisterDeleteBufferListener
 * @tc.type: FUNC
 * @tc.require: issueI6QM6E
 */
HWTEST(RSBaseRenderEngineExtUnitTest, RegisterDeleteBufferListener01, TestSize.Level1)
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
HWTEST(RSBaseRenderEngineExtUnitTest, ConvertColorGamutToDrawingColorSpace, TestSize.Level1)
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
#endif
}

/**
 * @tc.name: NeedForceCPU01
 * @tc.desc: Test NeedForceCPU
 * @tc.type: FUNC
 * @tc.require: issueI6QM6E
 */
HWTEST(RSBaseRenderEngineExtUnitTest, NeedForceCPU01, TestSize.Level1)
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
 * @tc.name: NeedForceCPU02
 * @tc.desc: Test NeedForceCPU
 * @tc.type: FUNC
 * @tc.require: issueI6QM6E
 */
HWTEST(RSBaseRenderEngineExtUnitTest, NeedForceCPU02, TestSize.Level1)
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
