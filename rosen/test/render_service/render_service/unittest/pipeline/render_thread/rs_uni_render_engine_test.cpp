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
#include "foundation/graphic/graphic_2d/rosen/test/render_service/render_service/unittest/pipeline/rs_test_util.h"
#include "pipeline/render_thread/rs_divided_render_util.h"
#include "pipeline/render_thread/rs_uni_render_engine.h"
#include "platform/common/rs_system_properties.h"
#include "recording/recording_canvas.h"
#include "screen_manager/rs_screen_manager.h"

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
#ifdef RS_ENABLE_VK
    RsVulkanContext::SetRecyclable(false);
#endif
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
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("layer3");
    layer3->SetSurface(cSurface);

    if (RSSystemProperties::IsUseVulkan()) {
        layer1->SetBuffer(buffer, surfaceNode->GetRSSurfaceHandler()->GetAcquireFence());
        layer2->SetBuffer(buffer, surfaceNode->GetRSSurfaceHandler()->GetAcquireFence());
        layer3->SetBuffer(buffer, surfaceNode->GetRSSurfaceHandler()->GetAcquireFence());
    }

    layers.emplace_back(layer1);
    layers.emplace_back(layer2);
    layers.emplace_back(layer3);
    ScreenInfo screenInfo;
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
    LayerInfoPtr layer = HdiLayerInfo::CreateHdiLayerInfo();
    BufferDrawParam param;
    param.useCPU = false;
    if (RSSystemProperties::IsUseVulkan()) {
        param.buffer = surfaceNode->GetRSSurfaceHandler()->GetBuffer();
    }
    uniRenderEngine->DrawHdiLayerWithParams(*canvas, layer, param);

    param.useCPU = true;
    uniRenderEngine->DrawHdiLayerWithParams(*canvas, layer, param);
}
} // namespace OHOS::Rosen