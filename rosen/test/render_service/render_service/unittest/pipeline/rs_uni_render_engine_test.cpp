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
#include "pipeline/rs_divided_render_util.h"
#include "pipeline/rs_uni_render_engine.h"
#include "platform/common/rs_system_properties.h"
#include "recording/recording_canvas.h"
#include "rs_test_util.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSUniRenderEngineTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSUniRenderEngineTest::SetUpTestCase() {}
void RSUniRenderEngineTest::TearDownTestCase() {}
void RSUniRenderEngineTest::SetUp() {}
void RSUniRenderEngineTest::TearDown() {}

/**
 * @tc.name: DrawSurfaceNodeWithParams001
 * @tc.desc: test DrawSurfaceNodeWithParams
 * @tc.type: FUNC
 * @tc.require: issueI6QM6E
 */
HWTEST(RSUniRenderEngineTest, DrawSurfaceNodeWithParams001, TestSize.Level1)
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
HWTEST(RSUniRenderEngineTest, DrawSurfaceNodeWithParams002, TestSize.Level1)
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
 * @tc.name: DrawLayers001
 * @tc.desc: test DrawLayers
 * @tc.type: FUNC
 * @tc.require: issueI6QM6E
 */
HWTEST(RSUniRenderEngineTest, DrawLayers001, TestSize.Level1)
{
    auto uniRenderEngine = std::make_shared<RSUniRenderEngine>();
    // Resources for Vulkan and DDGR API
    if (RSSystemProperties::IsUseVulkan()) {
        uniRenderEngine->Init();
    }
    std::unique_ptr<Drawing::RecordingCanvas> drawingRecordingCanvas = nullptr;
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto buffer = surfaceNode->GetBuffer();
    // End resources definition
    std::unique_ptr<Drawing::Canvas> drawingCanvas = std::make_unique<Drawing::Canvas>(10, 10);
    std::shared_ptr<RSPaintFilterCanvas> canvas = nullptr;
    if (!RSSystemProperties::IsUseVulkan()) {
        canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    } else {
        drawingRecordingCanvas = std::make_unique<Drawing::RecordingCanvas>(10, 10);
        drawingRecordingCanvas->SetGrRecordingContext(uniRenderEngine->GetRenderContext()->GetSharedDrGPUContext());
        canvas = std::make_shared<RSPaintFilterCanvas>(std::move(drawingRecordingCanvas).get());
    }
    ASSERT_NE(canvas, nullptr);
    std::vector<LayerInfoPtr> layers;
    if (!RSSystemProperties::IsUseVulkan()) {
        layers.emplace_back(nullptr);
    }
    LayerInfoPtr layer1 = HdiLayerInfo::CreateHdiLayerInfo();
    layer1->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);

    LayerInfoPtr layer2 = HdiLayerInfo::CreateHdiLayerInfo();
    layer2->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE_CLEAR);

    LayerInfoPtr layer3 = HdiLayerInfo::CreateHdiLayerInfo();
    layer3->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT);

    if (RSSystemProperties::IsUseVulkan()) {
        layer1->SetBuffer(buffer, surfaceNode->GetAcquireFence());
        layer2->SetBuffer(buffer, surfaceNode->GetAcquireFence());
        layer3->SetBuffer(buffer, surfaceNode->GetAcquireFence());
    }

    layers.emplace_back(layer1);
    layers.emplace_back(layer2);
    layers.emplace_back(layer3);
    uniRenderEngine->DrawLayers(*canvas, layers, false, 0.0f);
}

/**
 * @tc.name: DrawHdiLayerWithParams001
 * @tc.desc: test DrawHdiLayerWithParams
 * @tc.type: FUNC
 * @tc.require: issueI6QM6E
 */
HWTEST(RSUniRenderEngineTest, DrawHdiLayerWithParams001, TestSize.Level1)
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
        canvas = std::make_shared<RSPaintFilterCanvas>(std::move(drawingRecordingCanvas).get());
    }
    ASSERT_NE(canvas, nullptr);
    LayerInfoPtr layer = HdiLayerInfo::CreateHdiLayerInfo();
    BufferDrawParam param;
    param.useCPU = false;
    if (RSSystemProperties::IsUseVulkan()) {
        param.buffer = surfaceNode->GetBuffer();
    }
    uniRenderEngine->DrawHdiLayerWithParams(*canvas, layer, param);

    param.useCPU = true;
    uniRenderEngine->DrawHdiLayerWithParams(*canvas, layer, param);
}
}