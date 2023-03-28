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
#include "pipeline/rs_render_engine.h"
#include "rs_test_util.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderEngineTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderEngineTest::SetUpTestCase() {}
void RSRenderEngineTest::TearDownTestCase() {}
void RSRenderEngineTest::SetUp() {}
void RSRenderEngineTest::TearDown() {}

/**
 * @tc.name: DrawSurfaceNodeWithParams001
 * @tc.desc: test DrawSurfaceNodeWithParams when useCPU is false
 * @tc.type: FUNC
 * @tc.require: issueI6R34I
 */
HWTEST(RSRenderEngineTest, DrawSurfaceNodeWithParams001, TestSize.Level1)
{
    auto renderEngine = std::make_shared<RSRenderEngine>();
    std::unique_ptr<SkCanvas> skCanvas = std::make_unique<SkCanvas>(10, 10);
    std::shared_ptr<RSPaintFilterCanvas> canvas = std::make_shared<RSPaintFilterCanvas>(skCanvas.get());
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto param = RSDividedRenderUtil::CreateBufferDrawParam(*node);
    param.useCPU = false;
    renderEngine->DrawSurfaceNodeWithParams(*canvas, *node, param, nullptr, nullptr);
    ASSERT_NE(canvas, nullptr);
}

/**
 * @tc.name: DrawSurfaceNodeWithParams002
 * @tc.desc: test DrawSurfaceNodeWithParams when useCPU is true
 * @tc.type: FUNC
 * @tc.require: issueI6R34I
 */
HWTEST(RSRenderEngineTest, DrawSurfaceNodeWithParams002, TestSize.Level1)
{
    auto renderEngine = std::make_shared<RSRenderEngine>();
    std::unique_ptr<SkCanvas> skCanvas = std::make_unique<SkCanvas>(10, 10);
    std::shared_ptr<RSPaintFilterCanvas> canvas = std::make_shared<RSPaintFilterCanvas>(skCanvas.get());
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto param = RSDividedRenderUtil::CreateBufferDrawParam(*node);
    param.useCPU = true;
    renderEngine->DrawSurfaceNodeWithParams(*canvas, *node, param, nullptr, nullptr);
    ASSERT_NE(canvas, nullptr);
}

/**
 * @tc.name: DrawLayers001
 * @tc.desc: test DrawLayers with different GraphicCompositionType
 * @tc.type: FUNC
 * @tc.require: issueI6R34I
 */
HWTEST(RSRenderEngineTest, DrawLayers001, TestSize.Level1)
{
    auto renderEngine = std::make_shared<RSRenderEngine>();
    std::unique_ptr<SkCanvas> skCanvas = std::make_unique<SkCanvas>(10, 10);
    std::shared_ptr<RSPaintFilterCanvas> canvas = std::make_shared<RSPaintFilterCanvas>(skCanvas.get());
    std::vector<LayerInfoPtr> layers;
    layers.emplace_back(nullptr);
    LayerInfoPtr layer1 = HdiLayerInfo::CreateHdiLayerInfo();
    layer1->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    layers.emplace_back(layer1);
    LayerInfoPtr layer2 = HdiLayerInfo::CreateHdiLayerInfo();
    layer2->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE_CLEAR);
    layers.emplace_back(layer2);
    LayerInfoPtr layer3 = HdiLayerInfo::CreateHdiLayerInfo();
    layer3->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT);
    layers.emplace_back(layer3);
    renderEngine->DrawLayers(*canvas, layers, false, 0.0f);
    ASSERT_NE(canvas, nullptr);
}

/**
 * @tc.name: DrawWithParams
 * @tc.desc: test DrawWithParams when preProcessFunc and postProcessFunc are null
 * @tc.type: FUNC
 * @tc.require: issueI6R34I
 */
HWTEST(RSRenderEngineTest, DrawWithParams, TestSize.Level1)
{
    auto renderEngine = std::make_shared<RSRenderEngine>();
    std::unique_ptr<SkCanvas> skCanvas = std::make_unique<SkCanvas>(10, 10);
    std::shared_ptr<RSPaintFilterCanvas> canvas = std::make_shared<RSPaintFilterCanvas>(skCanvas.get());
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto param = RSDividedRenderUtil::CreateBufferDrawParam(*node);
    renderEngine->DrawWithParams(*canvas, param, nullptr, nullptr);
}

/**
 * @tc.name: RSSurfaceNodeCommonPreProcess
 * @tc.desc: test RSSurfaceNodeCommonPreProcess without filter, expect no error
 * @tc.type: FUNC
 * @tc.require: issueI6R34I
 */
HWTEST(RSRenderEngineTest, RSSurfaceNodeCommonPreProcess, TestSize.Level1)
{
    auto renderEngine = std::make_shared<RSRenderEngine>();
    std::unique_ptr<SkCanvas> skCanvas = std::make_unique<SkCanvas>(10, 10);
    std::shared_ptr<RSPaintFilterCanvas> canvas = std::make_shared<RSPaintFilterCanvas>(skCanvas.get());
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto param = RSDividedRenderUtil::CreateBufferDrawParam(*node);
    renderEngine->RSSurfaceNodeCommonPreProcess(*node, *canvas, param);
    ASSERT_NE(canvas, nullptr);
}

/**
 * @tc.name: RSSurfaceNodeCommonPostProcess
 * @tc.desc: test RSSurfaceNodeCommonPostProcess without filter, expect no error
 * @tc.type: FUNC
 * @tc.require: issueI6R34I
 */
HWTEST(RSRenderEngineTest, RSSurfaceNodeCommonPostProcess, TestSize.Level1)
{
    auto renderEngine = std::make_shared<RSRenderEngine>();
    std::unique_ptr<SkCanvas> skCanvas = std::make_unique<SkCanvas>(10, 10);
    std::shared_ptr<RSPaintFilterCanvas> canvas = std::make_shared<RSPaintFilterCanvas>(skCanvas.get());
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto param = RSDividedRenderUtil::CreateBufferDrawParam(*node);
    renderEngine->RSSurfaceNodeCommonPostProcess(*node, *canvas, param);
    ASSERT_NE(canvas, nullptr);
}

/**
 * @tc.name: ClipHoleForLayer
 * @tc.desc: test ClipHoleForLayer, expect no error
 * @tc.type: FUNC
 * @tc.require: issueI6R34I
 */
HWTEST(RSRenderEngineTest, ClipHoleForLayer, TestSize.Level1)
{
    auto renderEngine = std::make_shared<RSRenderEngine>();
    std::unique_ptr<SkCanvas> skCanvas = std::make_unique<SkCanvas>(10, 10);
    std::shared_ptr<RSPaintFilterCanvas> canvas = std::make_shared<RSPaintFilterCanvas>(skCanvas.get());
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    renderEngine->ClipHoleForLayer(*canvas, *node);
    ASSERT_NE(canvas, nullptr);
}

/**
 * @tc.name: SetColorFilterModeToPaint
 * @tc.desc: test SetColorFilterModeToPaint, expect no error
 * @tc.type: FUNC
 * @tc.require: issueI6R34I
 */
HWTEST(RSRenderEngineTest, SetColorFilterModeToPaint, TestSize.Level1)
{
    auto renderEngine = std::make_shared<RSRenderEngine>();
    std::unique_ptr<SkPaint> skPaint = std::make_unique<SkPaint>();
    renderEngine->SetColorFilterModeToPaint(*skPaint);
    ASSERT_NE(skPaint, nullptr);
}
}