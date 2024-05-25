/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include "limit_number.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_processor_factory.h"
#include "pipeline/rs_uni_render_engine.h"
#include "pipeline/rs_uni_render_virtual_processor.h"
#include "common/rs_obj_abs_geometry.h"
#include "pipeline/round_corner_display/rs_rcd_surface_render_node.h"
#include "pipeline/rs_render_engine.h"

using namespace testing;
using namespace testing::ext;

namespace {
    constexpr uint32_t DEFAULT_CANVAS_WIDTH = 800;
    constexpr uint32_t DEFAULT_CANVAS_HEIGHT = 600;
}
namespace OHOS::Rosen {
class RSUniRenderVirtualProcessorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSUniRenderVirtualProcessorTest::SetUpTestCase() {}
void RSUniRenderVirtualProcessorTest::TearDownTestCase() {}
void RSUniRenderVirtualProcessorTest::SetUp() {}
void RSUniRenderVirtualProcessorTest::TearDown() {}

/**
 * @tc.name: CreateAndDestroy
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSUniRenderVirtualProcessorTest, CreateAndDestroy, TestSize.Level2)
{
    // The best way to create RSUniRenderVirtualProcessor.
    auto type = RSDisplayRenderNode::CompositeType::UNI_RENDER_MIRROR_COMPOSITE;
    auto processor = RSProcessorFactory::CreateProcessor(type);
    auto rsUniRenderVirtualProcessor = std::static_pointer_cast<RSUniRenderVirtualProcessor>(processor);
    ASSERT_NE(nullptr, rsUniRenderVirtualProcessor);
}

/**
 * @tc.name: Init001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSUniRenderVirtualProcessorTest, Init001, TestSize.Level2)
{
    RSDisplayNodeConfig config;
    NodeId id = 0;
    int32_t offsetX = 0;
    int32_t offsetY = 0;
    RSDisplayRenderNode rsDisplayRenderNode(id, config);
    auto processor = RSProcessorFactory::CreateProcessor(RSDisplayRenderNode::CompositeType::
        UNI_RENDER_MIRROR_COMPOSITE);
    auto& uniRenderThread = RSUniRenderThread::Instance();
    uniRenderThread.uniRenderEngine_ = std::make_shared<RSUniRenderEngine>();
    auto renderEngine = uniRenderThread.GetRenderEngine();
    ASSERT_NE(nullptr, processor);
    ASSERT_EQ(false, processor->Init(rsDisplayRenderNode, offsetX, offsetY, INVALID_SCREEN_ID, renderEngine));
}

/**
 * @tc.name: ProcessSurface
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSUniRenderVirtualProcessorTest, ProcessSurface, TestSize.Level2)
{
    RSSurfaceRenderNodeConfig config;
    RSSurfaceRenderNode rsSurfaceRenderNode(config);
    auto processor = RSProcessorFactory::CreateProcessor(RSDisplayRenderNode::CompositeType::
        UNI_RENDER_MIRROR_COMPOSITE);
    processor->ProcessSurface(rsSurfaceRenderNode);
}

/**
 * @tc.name: ProcessDisplaySurface
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSUniRenderVirtualProcessorTest, ProcessDisplaySurface, TestSize.Level2)
{
    RSDisplayNodeConfig config;
    NodeId id = 0;
    RSDisplayRenderNode rsDisplayRenderNode(id, config);
    auto processor = RSProcessorFactory::CreateProcessor(RSDisplayRenderNode::CompositeType::
        UNI_RENDER_MIRROR_COMPOSITE);
    ASSERT_NE(nullptr, processor);
    processor->ProcessDisplaySurface(rsDisplayRenderNode);
}

/**
 * @tc.name: PostProcess
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSUniRenderVirtualProcessorTest, PostProcess, TestSize.Level2)
{
    auto processor = RSProcessorFactory::CreateProcessor(RSDisplayRenderNode::CompositeType::
        UNI_RENDER_MIRROR_COMPOSITE);
    processor->PostProcess();
}

/**
 * @tc.name: OriginScreenRotation
 * @tc.desc: OriginScreenRotation Test
 * @tc.type: FUNC
 * @tc.require: issueI992VW
 */
HWTEST_F(RSUniRenderVirtualProcessorTest, OriginScreenRotation, TestSize.Level2)
{
    auto processor = RSProcessorFactory::CreateProcessor(RSDisplayRenderNode::CompositeType::
        UNI_RENDER_MIRROR_COMPOSITE);
    auto virtualProcessor = std::static_pointer_cast<RSUniRenderVirtualProcessor>(processor);
    ASSERT_NE(nullptr, virtualProcessor);
    auto drawingCanvas = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(nullptr, drawingCanvas);
    virtualProcessor->canvas_ = std::make_unique<RSPaintFilterCanvas>(drawingCanvas.get());
    ASSERT_NE(nullptr, virtualProcessor->canvas_);
    virtualProcessor->OriginScreenRotation(ScreenRotation::ROTATION_0, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    virtualProcessor->OriginScreenRotation(ScreenRotation::ROTATION_90, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    virtualProcessor->OriginScreenRotation(ScreenRotation::ROTATION_180, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    virtualProcessor->OriginScreenRotation(ScreenRotation::ROTATION_270, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
}

/**
 * @tc.name: RotateMirrorCanvasIfNeed
 * @tc.desc: RotateMirrorCanvasIfNeed Test
 * @tc.type: FUNC
 * @tc.require: issueI992VW
 */
HWTEST_F(RSUniRenderVirtualProcessorTest, RotateMirrorCanvasIfNeed, TestSize.Level2)
{
    auto processor = RSProcessorFactory::CreateProcessor(RSDisplayRenderNode::CompositeType::
        UNI_RENDER_MIRROR_COMPOSITE);
    auto virtualProcessor = std::static_pointer_cast<RSUniRenderVirtualProcessor>(processor);
    ASSERT_NE(nullptr, virtualProcessor);
    RSDisplayNodeConfig config;
    NodeId id = 0;
    RSDisplayRenderNode rsDisplayRenderNode(id, config);
    auto drawingCanvas = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(nullptr, drawingCanvas);
    virtualProcessor->canvas_ = std::make_unique<RSPaintFilterCanvas>(drawingCanvas.get());
    ASSERT_NE(nullptr, virtualProcessor->canvas_);
    rsDisplayRenderNode.screenRotation_ = ScreenRotation::ROTATION_0;
    virtualProcessor->RotateMirrorCanvasIfNeed(rsDisplayRenderNode, true);
    rsDisplayRenderNode.screenRotation_ = ScreenRotation::ROTATION_90;
    virtualProcessor->RotateMirrorCanvasIfNeed(rsDisplayRenderNode, true);
    rsDisplayRenderNode.screenRotation_ = ScreenRotation::ROTATION_180;
    virtualProcessor->RotateMirrorCanvasIfNeed(rsDisplayRenderNode, true);
    rsDisplayRenderNode.screenRotation_ = ScreenRotation::ROTATION_270;
    virtualProcessor->RotateMirrorCanvasIfNeed(rsDisplayRenderNode, true);
}

/**
 * @tc.name: ScaleMirrorIfNeed
 * @tc.desc: ScaleMirrorIfNeed Test
 * @tc.type: FUNC
 * @tc.require: issueI992VW
 */
HWTEST_F(RSUniRenderVirtualProcessorTest, ScaleMirrorIfNeed, TestSize.Level2)
{
    auto processor = RSProcessorFactory::CreateProcessor(RSDisplayRenderNode::CompositeType::
        UNI_RENDER_MIRROR_COMPOSITE);
    auto virtualProcessor = std::static_pointer_cast<RSUniRenderVirtualProcessor>(processor);
    ASSERT_NE(nullptr, virtualProcessor);
    RSDisplayNodeConfig config;
    NodeId id = 0;
    RSDisplayRenderNode rsDisplayRenderNode(id, config);
    auto drawingCanvas = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(nullptr, drawingCanvas);
    virtualProcessor->canvas_ = std::make_unique<RSPaintFilterCanvas>(drawingCanvas.get());
    ASSERT_NE(nullptr, virtualProcessor->canvas_);
    virtualProcessor->scaleMode_ = ScreenScaleMode::FILL_MODE;
    virtualProcessor->ScaleMirrorIfNeed(rsDisplayRenderNode);

    virtualProcessor->mainWidth_ = DEFAULT_CANVAS_WIDTH;
    virtualProcessor->mainHeight_ = DEFAULT_CANVAS_HEIGHT;
    virtualProcessor->mirrorWidth_ = DEFAULT_CANVAS_WIDTH / 2;
    virtualProcessor->mirrorHeight_ = DEFAULT_CANVAS_HEIGHT / 2;
    virtualProcessor->ScaleMirrorIfNeed(rsDisplayRenderNode);

    virtualProcessor->scaleMode_ = ScreenScaleMode::UNISCALE_MODE;
    virtualProcessor->ScaleMirrorIfNeed(rsDisplayRenderNode);
}

/**
 * @tc.name: JudgeResolution
 * @tc.desc: JudgeResolution Test
 * @tc.type: FUNC
 * @tc.require: issueI992VW
 */
HWTEST_F(RSUniRenderVirtualProcessorTest, JudgeResolution, TestSize.Level2)
{
    auto processor = RSProcessorFactory::CreateProcessor(RSDisplayRenderNode::CompositeType::
        UNI_RENDER_MIRROR_COMPOSITE);
    auto virtualProcessor = std::static_pointer_cast<RSUniRenderVirtualProcessor>(processor);
    ASSERT_NE(nullptr, virtualProcessor);
    RSDisplayNodeConfig config;
    NodeId id = 0;
    RSDisplayRenderNode rsDisplayRenderNode(id, config);
    rsDisplayRenderNode.SetOriginScreenRotation(ScreenRotation::ROTATION_90);
    virtualProcessor->JudgeResolution(rsDisplayRenderNode);
    rsDisplayRenderNode.SetOriginScreenRotation(ScreenRotation::ROTATION_270);
    virtualProcessor->JudgeResolution(rsDisplayRenderNode);
}

/**
 * @tc.name: CanvasAdjustment
 * @tc.desc: CanvasAdjustment Test
 * @tc.type: FUNC
 * @tc.require: issueI992VW
 */
HWTEST_F(RSUniRenderVirtualProcessorTest, CanvasAdjustment, TestSize.Level2)
{
    auto processor = RSProcessorFactory::CreateProcessor(RSDisplayRenderNode::CompositeType::
        UNI_RENDER_MIRROR_COMPOSITE);
    auto virtualProcessor = std::static_pointer_cast<RSUniRenderVirtualProcessor>(processor);
    ASSERT_NE(nullptr, virtualProcessor);
    auto drawingCanvas = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(nullptr, drawingCanvas);
    virtualProcessor->canvas_ = std::make_unique<RSPaintFilterCanvas>(drawingCanvas.get());
    ASSERT_NE(nullptr, virtualProcessor->canvas_);
    RSDisplayNodeConfig config;
    NodeId id = 0;
    RSDisplayRenderNode rsDisplayRenderNode(id, config);
    virtualProcessor->CanvasAdjustment(rsDisplayRenderNode, false);
    auto geometry = std::make_shared<RSObjAbsGeometry>();
    ASSERT_NE(nullptr, geometry);
    rsDisplayRenderNode.renderContent_->renderProperties_.boundsGeo_ = geometry;
    virtualProcessor->CanvasAdjustment(rsDisplayRenderNode, true);
}

/**
 * @tc.name: Fill
 * @tc.desc: Fill Test
 * @tc.type: FUNC
 * @tc.require: issueI992VW
 */
HWTEST_F(RSUniRenderVirtualProcessorTest, Fill, TestSize.Level2)
{
    auto processor = RSProcessorFactory::CreateProcessor(RSDisplayRenderNode::CompositeType::
        UNI_RENDER_MIRROR_COMPOSITE);
    auto virtualProcessor = std::static_pointer_cast<RSUniRenderVirtualProcessor>(processor);
    ASSERT_NE(nullptr, virtualProcessor);
    auto drawingCanvas = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(nullptr, drawingCanvas);
    auto canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    virtualProcessor->Fill(*canvas, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT,
        DEFAULT_CANVAS_WIDTH / 2, DEFAULT_CANVAS_HEIGHT / 2);
}

/**
 * @tc.name: UniScale
 * @tc.desc: UniScale Test
 * @tc.type: FUNC
 * @tc.require: issueI992VW
 */
HWTEST_F(RSUniRenderVirtualProcessorTest, UniScale, TestSize.Level2)
{
    auto processor = RSProcessorFactory::CreateProcessor(RSDisplayRenderNode::CompositeType::
        UNI_RENDER_MIRROR_COMPOSITE);
    auto virtualProcessor = std::static_pointer_cast<RSUniRenderVirtualProcessor>(processor);
    ASSERT_NE(nullptr, virtualProcessor);
    auto drawingCanvas = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(nullptr, drawingCanvas);
    auto canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    virtualProcessor->UniScale(*canvas, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT,
        DEFAULT_CANVAS_WIDTH / 3, DEFAULT_CANVAS_HEIGHT / 2);
    virtualProcessor->UniScale(*canvas, DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT,
        DEFAULT_CANVAS_WIDTH / 2, DEFAULT_CANVAS_HEIGHT / 3);
}

/**
 * @tc.name: ProcessRcdSurfaceTest
 * @tc.desc: Verify function ProcessRcdSurface
 * @tc.type:FUNC
 * @tc.require:issuesI9KRF1
 */
HWTEST_F(RSUniRenderVirtualProcessorTest, ProcessRcdSurfaceTest, TestSize.Level2)
{
    auto processor = std::make_shared<RSUniRenderVirtualProcessor>();
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[0];
    RCDSurfaceType type = RCDSurfaceType::INVALID;
    RSRcdSurfaceRenderNode node(nodeId, type);
    processor->ProcessRcdSurface(node);
    EXPECT_FALSE(processor->forceCPU_);
}

/**
 * @tc.name: ProcessDisplaySurfaceTest
 * @tc.desc: Verify function ProcessDisplaySurface
 * @tc.type:FUNC
 * @tc.require:issuesI9KRF1
 */
HWTEST_F(RSUniRenderVirtualProcessorTest, ProcessDisplaySurfaceTest, TestSize.Level2)
{
    RSDisplayNodeConfig config;
    NodeId id = 0;
    RSDisplayRenderNode rsDisplayRenderNode(id, config);
    auto processor = std::make_shared<RSUniRenderVirtualProcessor>();
    processor->ProcessDisplaySurface(rsDisplayRenderNode);
    Drawing::Canvas canvas;
    processor->canvas_ = std::make_unique<RSPaintFilterCanvas>(&canvas);
    rsDisplayRenderNode.buffer_.buffer = OHOS::SurfaceBuffer::Create();
    processor->ProcessDisplaySurface(rsDisplayRenderNode);
    EXPECT_FALSE(processor->forceCPU_);
}
} // namespace OHOS::Rosen
