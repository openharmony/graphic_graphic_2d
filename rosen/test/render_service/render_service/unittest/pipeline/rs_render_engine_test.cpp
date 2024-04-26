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
#include "pipeline/rs_surface_capture_task.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "recording/recording_canvas.h"
#include "pipeline/rs_uni_render_engine.h"

using namespace testing;
using namespace testing::ext;

namespace {
    constexpr float DEFAULT_BOUNDS_WIDTH = 100.f;
    constexpr uint32_t DEFAULT_CANVAS_WIDTH = 800;
    constexpr uint32_t DEFAULT_CANVAS_HEIGHT = 600;
    constexpr float DEFAULT_CANVAS_SCALE = 1.0f;
    constexpr uint32_t DEFAULT_DRAWING_CANVAS_WIDTH = 10;
    constexpr uint32_t DEFAULT_DRAWING_CANVAS_HEIGHT = 10;
}

namespace OHOS::Rosen {
class RSRenderEngineTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

std::shared_ptr<RSSurfaceCaptureVisitor> visitor_;
std::shared_ptr<Drawing::Canvas> drawingCanvas_;
std::shared_ptr<Drawing::RecordingCanvas> drawingRecordingCanvas_ = nullptr;

void RSRenderEngineTest::SetUpTestCase()
{
    drawingCanvas_ = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    bool isUnirender = RSUniRenderJudgement::IsUniRender();
    visitor_ = std::make_shared<RSSurfaceCaptureVisitor>(DEFAULT_CANVAS_SCALE, DEFAULT_CANVAS_SCALE, isUnirender);
    if (visitor_ == nullptr) {
        return;
    }
    visitor_->renderEngine_ = std::make_shared<RSUniRenderEngine>();
    visitor_->renderEngine_->Init();
    if (!RSSystemProperties::IsUseVulkan()) {
        visitor_->canvas_ = std::make_unique<RSPaintFilterCanvas>(drawingCanvas_.get());
    } else {
        const int canvasWidth = 10;
        const int canvasHeight = 10;
        drawingRecordingCanvas_ = std::make_shared<Drawing::RecordingCanvas>(canvasWidth, canvasHeight);
        visitor_->canvas_ = std::make_unique<RSPaintFilterCanvas>(drawingRecordingCanvas_.get());
    }
}
void RSRenderEngineTest::TearDownTestCase()
{
    visitor_ = nullptr;
    drawingCanvas_ = nullptr;
}
void RSRenderEngineTest::SetUp() {}

void RSRenderEngineTest::TearDown() {}

/**
 * @tc.name: DrawSurfaceNodeWithParams001
 * @tc.desc: test DrawSurfaceNodeWithParams when useCPU is false
 * @tc.type: FUNC
 * @tc.require: issueI6R34I
 */
HWTEST_F(RSRenderEngineTest, DrawSurfaceNodeWithParams001, TestSize.Level1)
{
    auto renderEngine = std::make_shared<RSRenderEngine>();
    renderEngine->Init();
    auto drawingCanvas = std::make_unique<Drawing::Canvas>(DEFAULT_DRAWING_CANVAS_WIDTH, DEFAULT_DRAWING_CANVAS_HEIGHT);
    std::shared_ptr<RSPaintFilterCanvas> canvas = nullptr;
    if (Drawing::SystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        Drawing::SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        auto drawingRecordingCanvas =
            std::make_unique<Drawing::RecordingCanvas>(DEFAULT_DRAWING_CANVAS_WIDTH, DEFAULT_DRAWING_CANVAS_HEIGHT);
        drawingRecordingCanvas->SetGrRecordingContext(renderEngine->GetRenderContext()->GetSharedDrGPUContext());
        canvas = std::make_shared<RSPaintFilterCanvas>(std::move(drawingRecordingCanvas).get());
    } else {
        canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    }
    if (!RSUniRenderJudgement::IsUniRender()) {
        auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
        auto param = RSDividedRenderUtil::CreateBufferDrawParam(*node);
        param.useCPU = true;
        renderEngine->DrawSurfaceNodeWithParams(*canvas, *node, param, nullptr, nullptr);
    }
    ASSERT_NE(canvas, nullptr);
}

/**
 * @tc.name: DrawSurfaceNodeWithParams002
 * @tc.desc: test DrawSurfaceNodeWithParams when useCPU is true
 * @tc.type: FUNC
 * @tc.require: issueI6R34I
 */
HWTEST_F(RSRenderEngineTest, DrawSurfaceNodeWithParams002, TestSize.Level1)
{
    auto renderEngine = std::make_shared<RSRenderEngine>();
    renderEngine->Init();
    auto drawingCanvas = std::make_unique<Drawing::Canvas>(DEFAULT_DRAWING_CANVAS_WIDTH, DEFAULT_DRAWING_CANVAS_HEIGHT);
    std::shared_ptr<RSPaintFilterCanvas> canvas = nullptr;
    if (Drawing::SystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        Drawing::SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        auto drawingRecordingCanvas =
            std::make_unique<Drawing::RecordingCanvas>(DEFAULT_DRAWING_CANVAS_WIDTH, DEFAULT_DRAWING_CANVAS_HEIGHT);
        drawingRecordingCanvas->SetGrRecordingContext(renderEngine->GetRenderContext()->GetSharedDrGPUContext());
        canvas = std::make_shared<RSPaintFilterCanvas>(std::move(drawingRecordingCanvas).get());
    } else {
        canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    }
    if (!RSUniRenderJudgement::IsUniRender()) {
        auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
        auto param = RSDividedRenderUtil::CreateBufferDrawParam(*node);
        param.useCPU = true;
        renderEngine->DrawSurfaceNodeWithParams(*canvas, *node, param, nullptr, nullptr);
    }
    ASSERT_NE(canvas, nullptr);
}

/**
 * @tc.name: DrawLayers001
 * @tc.desc: test DrawLayers with different GraphicCompositionType
 * @tc.type: FUNC
 * @tc.require: issueI6R34I
 */
HWTEST_F(RSRenderEngineTest, DrawLayers001, TestSize.Level1)
{
    auto renderEngine = std::make_shared<RSRenderEngine>();
    renderEngine->Init();
    auto drawingCanvas = std::make_unique<Drawing::Canvas>(DEFAULT_DRAWING_CANVAS_WIDTH, DEFAULT_DRAWING_CANVAS_HEIGHT);
    std::shared_ptr<RSPaintFilterCanvas> canvas = nullptr;
    if (Drawing::SystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        Drawing::SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        auto drawingRecordingCanvas =
            std::make_unique<Drawing::RecordingCanvas>(DEFAULT_DRAWING_CANVAS_WIDTH, DEFAULT_DRAWING_CANVAS_HEIGHT);
        drawingRecordingCanvas->SetGrRecordingContext(renderEngine->GetRenderContext()->GetSharedDrGPUContext());
        canvas = std::make_shared<RSPaintFilterCanvas>(std::move(drawingRecordingCanvas).get());
    } else {
        canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    }
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
 * @tc.name: DrawWithParams001
 * @tc.desc: test DrawWithParams when preProcessFunc and postProcessFunc are null
 * @tc.type: FUNC
 * @tc.require: issueI6R34I
 */
HWTEST_F(RSRenderEngineTest, DrawWithParams001, TestSize.Level1)
{
    if (!RSUniRenderJudgement::IsUniRender()) {
        auto renderEngine = std::make_shared<RSRenderEngine>();
        std::shared_ptr<Drawing::RecordingCanvas> drawingRecordingCanvas = nullptr;
        std::unique_ptr<Drawing::Canvas> drawingCanvas =
            std::make_unique<Drawing::Canvas>(DEFAULT_DRAWING_CANVAS_WIDTH, DEFAULT_DRAWING_CANVAS_HEIGHT);
        std::shared_ptr<RSPaintFilterCanvas> canvas = nullptr;
        if (!RSSystemProperties::IsUseVulkan()) {
            canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
        } else {
            renderEngine->Init();
            const int canvasWidth = 10;
            const int canvasHeight = 10;
            drawingRecordingCanvas = std::make_shared<Drawing::RecordingCanvas>(canvasWidth, canvasHeight);
            drawingRecordingCanvas->SetGrRecordingContext(renderEngine->GetRenderContext()->GetSharedDrGPUContext());
            visitor_->canvas_ = std::make_unique<RSPaintFilterCanvas>(std::move(drawingRecordingCanvas).get());
        }
        auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
        auto param = RSDividedRenderUtil::CreateBufferDrawParam(*node);
        renderEngine->DrawWithParams(*canvas, param, nullptr, nullptr);
    }
}

/**
 * @tc.name: DrawWithParams002
 * @tc.desc: test DrawWithParams when param.setColorDilter are false
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderEngineTest, DrawWithParams002, TestSize.Level1)
{
    auto renderEngine = std::make_shared<RSRenderEngine>();
    std::unique_ptr<Drawing::Canvas> drawingCanvas = std::make_unique<Drawing::Canvas>(10, 10);
    std::shared_ptr<RSPaintFilterCanvas> canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto param = RSDividedRenderUtil::CreateBufferDrawParam(*node);
    param.setColorFilter = false;
    renderEngine->DrawWithParams(*canvas, param, nullptr, nullptr);
}

/**
 * @tc.name: DrawWithParams003
 * @tc.desc: test DrawWithParams when param.setColorDilter are true
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderEngineTest, DrawWithParams003, TestSize.Level1)
{
    auto renderEngine = std::make_shared<RSRenderEngine>();
    std::unique_ptr<Drawing::Canvas> drawingCanvas = std::make_unique<Drawing::Canvas>(10, 10);
    std::shared_ptr<RSPaintFilterCanvas> canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto param = RSDividedRenderUtil::CreateBufferDrawParam(*node);
    param.setColorFilter = true;
    renderEngine->DrawWithParams(*canvas, param, nullptr, nullptr);
}

/**
 * @tc.name: DrawWithParams004
 * @tc.desc: test DrawWithParams when param.useCPU are true
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderEngineTest, DrawWithParams004, TestSize.Level1)
{
    auto renderEngine = std::make_shared<RSRenderEngine>();
    std::unique_ptr<Drawing::Canvas> drawingCanvas = std::make_unique<Drawing::Canvas>(10, 10);
    std::shared_ptr<RSPaintFilterCanvas> canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto param = RSDividedRenderUtil::CreateBufferDrawParam(*node);
    param.useCPU = true;
    renderEngine->DrawWithParams(*canvas, param, nullptr, nullptr);
}

/**
 * @tc.name: DrawWithParams005
 * @tc.desc: test DrawWithParams when param.useCPU are true
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderEngineTest, DrawWithParams005, TestSize.Level1)
{
    auto renderEngine = std::make_shared<RSRenderEngine>();
    std::unique_ptr<Drawing::Canvas> drawingCanvas = std::make_unique<Drawing::Canvas>(10, 10);
    std::shared_ptr<RSPaintFilterCanvas> canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto param = RSDividedRenderUtil::CreateBufferDrawParam(*node);
    param.useCPU = false;
    renderEngine->DrawWithParams(*canvas, param, nullptr, nullptr);
}

/**
 * @tc.name: RSSurfaceNodeCommonPreProcess
 * @tc.desc: test RSSurfaceNodeCommonPreProcess without filter, expect no error
 * @tc.type: FUNC
 * @tc.require: issueI6R34I
 */
HWTEST_F(RSRenderEngineTest, RSSurfaceNodeCommonPreProcess, TestSize.Level1)
{
    if (!RSUniRenderJudgement::IsUniRender()) {
        auto renderEngine = std::make_shared<RSRenderEngine>();
        std::unique_ptr<Drawing::Canvas> drawingCanvas = std::make_unique<Drawing::Canvas>(10, 10);
        std::shared_ptr<RSPaintFilterCanvas> canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
        auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
        auto param = RSDividedRenderUtil::CreateBufferDrawParam(*node);
        renderEngine->RSSurfaceNodeCommonPreProcess(*node, *canvas, param);
        ASSERT_NE(canvas, nullptr);
    }
}

/**
 * @tc.name: RSSurfaceNodeCommonPostProcess
 * @tc.desc: test RSSurfaceNodeCommonPostProcess without filter, expect no error
 * @tc.type: FUNC
 * @tc.require: issueI6R34I
 */
HWTEST_F(RSRenderEngineTest, RSSurfaceNodeCommonPostProcess, TestSize.Level1)
{
    if (!RSUniRenderJudgement::IsUniRender()) {
        auto renderEngine = std::make_shared<RSRenderEngine>();
        std::unique_ptr<Drawing::Canvas> drawingCanvas = std::make_unique<Drawing::Canvas>(10, 10);
        std::shared_ptr<RSPaintFilterCanvas> canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
        auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
        auto param = RSDividedRenderUtil::CreateBufferDrawParam(*node);
        renderEngine->RSSurfaceNodeCommonPostProcess(*node, *canvas, param);
        ASSERT_NE(canvas, nullptr);
    }
}

/**
 * @tc.name: ClipHoleForLayer
 * @tc.desc: test ClipHoleForLayer, expect no error
 * @tc.type: FUNC
 * @tc.require: issueI6R34I
 */
HWTEST_F(RSRenderEngineTest, ClipHoleForLayer, TestSize.Level1)
{
    auto renderEngine = std::make_shared<RSRenderEngine>();
    renderEngine->Init();
    auto drawingCanvas = std::make_unique<Drawing::Canvas>(DEFAULT_DRAWING_CANVAS_WIDTH, DEFAULT_DRAWING_CANVAS_HEIGHT);
    std::shared_ptr<RSPaintFilterCanvas> canvas = nullptr;
    canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
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
HWTEST_F(RSRenderEngineTest, SetColorFilterModeToPaint, TestSize.Level1)
{
    auto renderEngine = std::make_shared<RSRenderEngine>();
    renderEngine->Init();
    std::unique_ptr<Drawing::Brush> brush = std::make_unique<Drawing::Brush>();
    renderEngine->SetColorFilterModeToPaint(*brush);
    ASSERT_NE(brush, nullptr);
}

/*
 * @tc.name: CaptureSurfaceInDisplayWithUni006
 * @tc.desc: Test RSRenderEngineTest.CaptureSurfaceInDisplayWithUni
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RSRenderEngineTest, CaptureSurfaceInDisplayWithUni006, Function | SmallTest | Level2)
{
    bool isUnirender = RSUniRenderJudgement::IsUniRender();
    ASSERT_NE(nullptr, visitor_);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    surfaceNode->SetSurfaceNodeType(RSSurfaceNodeType::SELF_DRAWING_NODE);
    surfaceNode->SetSecurityLayer(true);
    if (isUnirender) {
        visitor_->CaptureSurfaceInDisplayWithUni(*surfaceNode);
    }
}

/*
 * @tc.name: CaptureSurfaceInDisplayWithUni004
 * @tc.desc: Test RSRenderEngineTest.CaptureSurfaceInDisplayWithUni
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RSRenderEngineTest, CaptureSurfaceInDisplayWithUni004, Function | SmallTest | Level2)
{
    bool isUnirender = RSUniRenderJudgement::IsUniRender();
    ASSERT_NE(nullptr, visitor_);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    surfaceNode->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    surfaceNode->SetSecurityLayer(false);
    if (isUnirender) {
        visitor_->CaptureSurfaceInDisplayWithUni(*surfaceNode);
    }
}

/*
 * @tc.name: CaptureSurfaceInDisplayWithUni001
 * @tc.desc: Test RSRenderEngineTest.CaptureSurfaceInDisplayWithUni
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RSRenderEngineTest, CaptureSurfaceInDisplayWithUni001, Function | SmallTest | Level2)
{
    bool isUnirender = RSUniRenderJudgement::IsUniRender();
    ASSERT_NE(nullptr, visitor_);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    surfaceNode->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    surfaceNode->SetSecurityLayer(true);
    if (isUnirender) {
        visitor_->CaptureSurfaceInDisplayWithUni(*surfaceNode);
    }
}

/*
 * @tc.name: CaptureSurfaceInDisplayWithUni002
 * @tc.desc: Test RSRenderEngineTest.CaptureSurfaceInDisplayWithUni
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RSRenderEngineTest, CaptureSurfaceInDisplayWithUni002, Function | SmallTest | Level2)
{
    bool isUnirender = RSUniRenderJudgement::IsUniRender();
    ASSERT_NE(nullptr, visitor_);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    surfaceNode->SetSurfaceNodeType(RSSurfaceNodeType::SELF_DRAWING_NODE);
    surfaceNode->SetSecurityLayer(false);
    if (isUnirender) {
        if (RSSystemParameters::GetRsSurfaceCaptureType() ==
            RsSurfaceCaptureType::RS_SURFACE_CAPTURE_TYPE_MAIN_THREAD) {
            visitor_->CaptureSurfaceInDisplayWithUni(*surfaceNode);
        }
    }
}

/*
 * @tc.name: ProcessSurfaceRenderNodeWithUni001
 * @tc.desc: Test RSRenderEngineTest.ProcessSurfaceRenderNodeWithUni
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RSRenderEngineTest, ProcessSurfaceRenderNodeWithUni001, Function | SmallTest | Level2)
{
    bool isUnirender = RSUniRenderJudgement::IsUniRender();
    ASSERT_NE(nullptr, visitor_);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    surfaceNode->GetMutableRenderProperties().SetBackgroundFilter(nullptr);
    if (isUnirender) {
        if (RSSystemParameters::GetRsSurfaceCaptureType() ==
            RsSurfaceCaptureType::RS_SURFACE_CAPTURE_TYPE_MAIN_THREAD) {
            visitor_->CaptureSurfaceInDisplayWithUni(*surfaceNode);
        }
    }
}

/*
 * @tc.name: ProcessSurfaceRenderNodeWithUni002
 * @tc.desc: Test RSRenderEngineTest.ProcessSurfaceRenderNodeWithUni
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RSRenderEngineTest, ProcessSurfaceRenderNodeWithUni002, Function | SmallTest | Level2)
{
    bool isUnirender = RSUniRenderJudgement::IsUniRender();
    ASSERT_NE(nullptr, visitor_);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    visitor_->isDisplayNode_ = true;
    if (isUnirender) {
        if (RSSystemParameters::GetRsSurfaceCaptureType() ==
            RsSurfaceCaptureType::RS_SURFACE_CAPTURE_TYPE_MAIN_THREAD) {
            visitor_->CaptureSurfaceInDisplayWithUni(*surfaceNode);
        }
    }
}

/*
 * @tc.name: ProcessSurfaceRenderNodeWithUni003
 * @tc.desc: Test RSRenderEngineTest.ProcessSurfaceRenderNodeWithUni
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RSRenderEngineTest, ProcessSurfaceRenderNodeWithUni003, Function | SmallTest | Level2)
{
    bool isUnirender = RSUniRenderJudgement::IsUniRender();
    ASSERT_NE(nullptr, visitor_);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    visitor_->isDisplayNode_ = false;
    if (isUnirender) {
        if (RSSystemParameters::GetRsSurfaceCaptureType() ==
            RsSurfaceCaptureType::RS_SURFACE_CAPTURE_TYPE_MAIN_THREAD) {
            visitor_->CaptureSurfaceInDisplayWithUni(*surfaceNode);
        }
    }
}

/*
 * @tc.name: CaptureSingleSurfaceNodeWithoutUni001
 * @tc.desc: Test RSRenderEngineTest.CaptureSingleSurfaceNodeWithoutUni
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RSRenderEngineTest, CaptureSingleSurfaceNodeWithoutUni001, Function | SmallTest | Level2)
{
    bool isUnirender = RSUniRenderJudgement::IsUniRender();
    ASSERT_NE(nullptr, visitor_);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    surfaceNode->SetSecurityLayer(true);
    if (isUnirender) {
        if (RSSystemParameters::GetRsSurfaceCaptureType() ==
            RsSurfaceCaptureType::RS_SURFACE_CAPTURE_TYPE_MAIN_THREAD) {
            visitor_->CaptureSingleSurfaceNodeWithoutUni(*surfaceNode);
        }
    }
}

/*
 * @tc.name: CaptureSingleSurfaceNodeWithoutUni002
 * @tc.desc: Test RSRenderEngineTest.CaptureSingleSurfaceNodeWithoutUni
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RSRenderEngineTest, CaptureSingleSurfaceNodeWithoutUni002, Function | SmallTest | Level2)
{
    bool isUnirender = RSUniRenderJudgement::IsUniRender();
    ASSERT_NE(nullptr, visitor_);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    surfaceNode->SetSecurityLayer(false);
    if (isUnirender) {
        if (RSSystemParameters::GetRsSurfaceCaptureType() ==
            RsSurfaceCaptureType::RS_SURFACE_CAPTURE_TYPE_MAIN_THREAD) {
            visitor_->CaptureSingleSurfaceNodeWithoutUni(*surfaceNode);
        }
    }
}

/*
 * @tc.name: CaptureSurfaceInDisplayWithoutUni001
 * @tc.desc: Test RSRenderEngineTest.CaptureSurfaceInDisplayWithoutUni
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RSRenderEngineTest, CaptureSurfaceInDisplayWithoutUni001, Function | SmallTest | Level2)
{
    bool isUnirender = RSUniRenderJudgement::IsUniRender();
    ASSERT_NE(nullptr, visitor_);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    surfaceNode->SetSecurityLayer(false);
    if (!isUnirender) {
        visitor_->CaptureSingleSurfaceNodeWithoutUni(*surfaceNode);
    }
}

/*
 * @tc.name: ProcessSurfaceRenderNode006
 * @tc.desc: Test RSRenderEngineTest.ProcessSurfaceRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RSRenderEngineTest, ProcessSurfaceRenderNode006, Function | SmallTest | Level2)
{
    bool isUnirender = RSUniRenderJudgement::IsUniRender();
    ASSERT_NE(nullptr, visitor_);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    surfaceNode->GetMutableRenderProperties().SetVisible(true);
    surfaceNode->GetMutableRenderProperties().SetAlpha(DEFAULT_BOUNDS_WIDTH);
    if (!isUnirender) {
        visitor_->ProcessSurfaceRenderNodeWithoutUni(*surfaceNode);
    }
}

/*
 * @tc.name: ProcessSurfaceRenderNode007
 * @tc.desc: Test RSRenderEngineTest.ProcessSurfaceRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RSRenderEngineTest, ProcessSurfaceRenderNode007, Function | SmallTest | Level2)
{
    bool isUnirender = RSUniRenderJudgement::IsUniRender();
    ASSERT_NE(nullptr, visitor_);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    surfaceNode->GetMutableRenderProperties().SetVisible(true);
    surfaceNode->GetMutableRenderProperties().SetAlpha(.0f);
    if (isUnirender) {
        if (RSSystemParameters::GetRsSurfaceCaptureType() ==
            RsSurfaceCaptureType::RS_SURFACE_CAPTURE_TYPE_MAIN_THREAD) {
            visitor_->ProcessSurfaceRenderNode(*surfaceNode);
        }
    }
}

/*
 * @tc.name: ProcessSurfaceRenderNode008
 * @tc.desc: Test RSRenderEngineTest.ProcessSurfaceRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RSRenderEngineTest, ProcessSurfaceRenderNode008, Function | SmallTest | Level2)
{
    bool isUnirender = RSUniRenderJudgement::IsUniRender();
    ASSERT_NE(nullptr, visitor_);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    visitor_->canvas_ = nullptr;
    if (isUnirender) {
        if (RSSystemParameters::GetRsSurfaceCaptureType() ==
            RsSurfaceCaptureType::RS_SURFACE_CAPTURE_TYPE_MAIN_THREAD) {
            visitor_->ProcessSurfaceRenderNode(*surfaceNode);
        }
    }
}

/*
 * @tc.name: CaptureSingleSurfaceNodeWithUni001
 * @tc.desc: Test RSRenderEngineTest.CaptureSingleSurfaceNodeWithUni when SecurityLayer is true
 * @tc.type: FUNC
 * @tc.require: issueI7G9F0
 */
HWTEST_F(RSRenderEngineTest, CaptureSingleSurfaceNodeWithUni001, Function | SmallTest | Level2)
{
    bool isUnirender = RSUniRenderJudgement::IsUniRender();
    ASSERT_NE(nullptr, visitor_);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(nullptr, surfaceNode);
    surfaceNode->SetSurfaceNodeType(RSSurfaceNodeType::SELF_DRAWING_NODE);
    surfaceNode->SetSecurityLayer(true);
    if (isUnirender) {
        if (RSSystemParameters::GetRsSurfaceCaptureType() ==
            RsSurfaceCaptureType::RS_SURFACE_CAPTURE_TYPE_MAIN_THREAD) {
            visitor_->CaptureSingleSurfaceNodeWithUni(*surfaceNode);
        }
    }
}

/*
 * @tc.name: CaptureSingleSurfaceNodeWithUni002
 * @tc.desc: Test RSRenderEngineTest.CaptureSingleSurfaceNodeWithUni when SecurityLayer is false
 * @tc.type: FUNC
 * @tc.require: issueI7G9F0
 */
HWTEST_F(RSRenderEngineTest, CaptureSingleSurfaceNodeWithUni002, Function | SmallTest | Level2)
{
    bool isUnirender = RSUniRenderJudgement::IsUniRender();
    ASSERT_NE(nullptr, visitor_);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(nullptr, surfaceNode);
    surfaceNode->SetSurfaceNodeType(RSSurfaceNodeType::SELF_DRAWING_NODE);
    surfaceNode->SetSecurityLayer(false);
    Vector4f cornerRadius(1.f, 2.f, 0.f, 0.f);
    surfaceNode->GetMutableRenderProperties().SetCornerRadius(cornerRadius);
    std::shared_ptr<RSFilter> filter = RSFilter::CreateBlurFilter(5.0f, 5.0f);
    surfaceNode->GetMutableRenderProperties().SetFilter(filter);
    if (isUnirender &&
        RSSystemParameters::GetRsSurfaceCaptureType() == RsSurfaceCaptureType::RS_SURFACE_CAPTURE_TYPE_MAIN_THREAD) {
        visitor_->CaptureSingleSurfaceNodeWithUni(*surfaceNode);
    }
}
}