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
#include "drawable/rs_display_render_node_drawable.h"
#include "params/rs_render_params.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_processor_factory.h"
#include "pipeline/rs_uni_render_engine.h"
#include "pipeline/rs_uni_render_virtual_processor.h"
#include "common/rs_obj_abs_geometry.h"
#include "pipeline/round_corner_display/rs_rcd_surface_render_node.h"
#include "pipeline/rs_render_engine.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::DrawableV2;

namespace {
    constexpr uint32_t DEFAULT_CANVAS_WIDTH = 800;
    constexpr uint32_t DEFAULT_CANVAS_HEIGHT = 600;
}
namespace OHOS::Rosen {
class RSUniRenderVirtualProcessorTest : public testing::Test {
public:
    std::shared_ptr<RSProcessor> processor_ = nullptr;
    std::shared_ptr<RSUniRenderVirtualProcessor> virtualProcessor_ = nullptr;
    std::shared_ptr<Drawing::Canvas> drawingCanvas_ = nullptr;
    std::shared_ptr<RSDisplayRenderNode> rsDisplayRenderNode_ = nullptr;
    RSDisplayRenderNodeDrawable* displayDrawable_ = nullptr;

    RSDisplayNodeConfig config_;
    NodeId nodeId_ = 0;

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSUniRenderVirtualProcessorTest::SetUpTestCase() {}
void RSUniRenderVirtualProcessorTest::TearDownTestCase() {}
void RSUniRenderVirtualProcessorTest::SetUp()
{
    processor_ = RSProcessorFactory::CreateProcessor(RSDisplayRenderNode::CompositeType::UNI_RENDER_MIRROR_COMPOSITE);
    ASSERT_NE(processor_, nullptr);
    virtualProcessor_ = std::static_pointer_cast<RSUniRenderVirtualProcessor>(processor_);
    ASSERT_NE(virtualProcessor_, nullptr);

    drawingCanvas_ = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(drawingCanvas_, nullptr);
    virtualProcessor_->canvas_ = std::make_unique<RSPaintFilterCanvas>(drawingCanvas_.get());
    ASSERT_NE(virtualProcessor_->canvas_, nullptr);

    rsDisplayRenderNode_ = std::make_shared<RSDisplayRenderNode>(nodeId_, config_);
    ASSERT_NE(rsDisplayRenderNode_, nullptr);
    displayDrawable_ =
        static_cast<RSDisplayRenderNodeDrawable*>(RSDisplayRenderNodeDrawable::OnGenerate(rsDisplayRenderNode_));
    ASSERT_NE(displayDrawable_, nullptr);
    displayDrawable_->renderParams_ = std::make_unique<RSRenderParams>(nodeId_);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);
}
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
    virtualProcessor->ScaleMirrorIfNeed(rsDisplayRenderNode, *virtualProcessor->canvas_);

    virtualProcessor->virtualScreenWidth_ = DEFAULT_CANVAS_WIDTH;
    virtualProcessor->virtualScreenHeight_ = DEFAULT_CANVAS_HEIGHT;
    virtualProcessor->mirroredScreenWidth_ = DEFAULT_CANVAS_WIDTH / 2;
    virtualProcessor->mirroredScreenHeight_ = DEFAULT_CANVAS_HEIGHT / 2;
    virtualProcessor->ScaleMirrorIfNeed(rsDisplayRenderNode, *virtualProcessor->canvas_);

    virtualProcessor->scaleMode_ = ScreenScaleMode::UNISCALE_MODE;
    virtualProcessor->ScaleMirrorIfNeed(rsDisplayRenderNode, *virtualProcessor->canvas_);
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
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(id, config);
    ASSERT_NE(rsDisplayRenderNode, nullptr);
    auto processor = std::make_shared<RSUniRenderVirtualProcessor>();
    ASSERT_NE(processor, nullptr);
    processor->ProcessDisplaySurface(*rsDisplayRenderNode);
    Drawing::Canvas canvas;
    processor->canvas_ = std::make_unique<RSPaintFilterCanvas>(&canvas);
    auto displayDrawable =
        static_cast<RSDisplayRenderNodeDrawable*>(RSDisplayRenderNodeDrawable::OnGenerate(rsDisplayRenderNode));
    auto surfaceHandler = displayDrawable->GetRSSurfaceHandlerOnDraw();
    surfaceHandler->buffer_.buffer = OHOS::SurfaceBuffer::Create();
    processor->ProcessDisplaySurface(*rsDisplayRenderNode);
    EXPECT_FALSE(processor->forceCPU_);
}

/**
 * @tc.name: CanvasInit_001
 * @tc.desc: CanvasInit Test, displayDrawable isFirstTimeToProcessor
 * @tc.type:FUNC
 * @tc.require:issuesIAJ4FW
 */
HWTEST_F(RSUniRenderVirtualProcessorTest, CanvasInit_001, TestSize.Level2)
{
    displayDrawable_->isFirstTimeToProcessor_ = true;
    virtualProcessor_->CanvasInit(*displayDrawable_);
}

/**
 * @tc.name: CanvasInit_002
 * @tc.desc: CanvasInit Test, displayDrawable_ is not FirstTimeToProcessor, canvasRotation of virtualProcessor_ is true
 * @tc.type:FUNC
 * @tc.require:issuesIAJ4FW
 */
HWTEST_F(RSUniRenderVirtualProcessorTest, CanvasInit_002, TestSize.Level2)
{
    displayDrawable_->isFirstTimeToProcessor_ = false;
    virtualProcessor_->canvasRotation_ = true;
    virtualProcessor_->CanvasInit(*displayDrawable_);
}

/**
 * @tc.name: GetBufferAge_001
 * @tc.desc: GetBufferAge Test, renderFrame_ not null, expect 0 when targetSurface_ and surfaceFrame_ are both null
 * @tc.type:FUNC
 * @tc.require:issuesIAJ4FW
 */
HWTEST_F(RSUniRenderVirtualProcessorTest, GetBufferAge_001, TestSize.Level2)
{
    virtualProcessor_->renderFrame_ = std::make_unique<RSRenderFrame>(nullptr, nullptr);
    ASSERT_NE(virtualProcessor_->renderFrame_, nullptr);

    auto ret = virtualProcessor_->GetBufferAge();
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: SetDirtyInfo_001
 * @tc.desc: SetDirtyInfo Test, renderFrame_ is null
 * @tc.type:FUNC
 * @tc.require:issuesIAJ4FW
 */
HWTEST_F(RSUniRenderVirtualProcessorTest, SetDirtyInfo_001, TestSize.Level2)
{
    virtualProcessor_->renderFrame_ = nullptr;
    std::vector<RectI> damageRegion {};
    virtualProcessor_->SetDirtyInfo(damageRegion);
}

/**
 * @tc.name: SetDirtyInfo_002
 * @tc.desc: SetDirtyInfo Test, renderFrame_ not nul, SetRoiRegionToCodec(damageRegion) != GSERROR_OK
 * @tc.type:FUNC
 * @tc.require:issuesIAJ4FW
 */
HWTEST_F(RSUniRenderVirtualProcessorTest, SetDirtyInfo_002, TestSize.Level2)
{
    virtualProcessor_->renderFrame_ = std::make_unique<RSRenderFrame>(nullptr, nullptr);
    ASSERT_NE(virtualProcessor_->renderFrame_, nullptr);
    std::vector<RectI> damageRegion {};
    ASSERT_EQ(virtualProcessor_->SetRoiRegionToCodec(damageRegion), GSERROR_INVALID_ARGUMENTS);

    virtualProcessor_->SetDirtyInfo(damageRegion);
}

/**
 * @tc.name: CalculateTransform_001
 * @tc.desc: CalculateTransform Test, isExpand_ is false, return directly
 * @tc.type:FUNC
 * @tc.require:issuesIAJ4FW
 */
HWTEST_F(RSUniRenderVirtualProcessorTest, CalculateTransform_001, TestSize.Level2)
{
    virtualProcessor_->isExpand_ = false;

    virtualProcessor_->CalculateTransform(*rsDisplayRenderNode_);
}

/**
 * @tc.name: CalculateTransform_002
 * @tc.desc: CalculateTransform Test, isExpand_ is true, renderDrawable_ is null , return directly
 * @tc.type:FUNC
 * @tc.require:issuesIAJ4FW
 */
HWTEST_F(RSUniRenderVirtualProcessorTest, CalculateTransform_002, TestSize.Level2)
{
    virtualProcessor_->isExpand_ = true;
    rsDisplayRenderNode_->renderDrawable_ = nullptr;
    ASSERT_EQ(rsDisplayRenderNode_->GetRenderDrawable(), nullptr);

    virtualProcessor_->CalculateTransform(*rsDisplayRenderNode_);
}

/**
 * @tc.name: CalculateTransform_003
 * @tc.desc: CalculateTransform Test,isExpand_ is true, renderDrawable_ is not null,and canvas_ is null
 * @tc.type:FUNC
 * @tc.require:issuesIAJ4FW
 */
HWTEST_F(RSUniRenderVirtualProcessorTest, CalculateTransform_003, TestSize.Level2)
{
    virtualProcessor_->isExpand_ = true;
    auto rsRenderNode = std::make_shared<RSRenderNode>(nodeId_);
    rsDisplayRenderNode_->renderDrawable_ = std::make_shared<DrawableV2::RSRenderNodeDrawable>(rsRenderNode);
    ASSERT_NE(rsDisplayRenderNode_->GetRenderDrawable(), nullptr);

    virtualProcessor_->canvas_ = nullptr;
    virtualProcessor_->CalculateTransform(*rsDisplayRenderNode_);
}

/**
 * @tc.name: CalculateTransform_004
 * @tc.desc: CalculateTransform Test,isExpand_ is true, renderDrawable_ is not null,and canvas_ not null
 * @tc.type:FUNC
 * @tc.require:issuesIAJ4FW
 */
HWTEST_F(RSUniRenderVirtualProcessorTest, CalculateTransform_004, TestSize.Level2)
{
    virtualProcessor_->isExpand_ = true;
    auto rsRenderNode = std::make_shared<RSRenderNode>(nodeId_);
    rsDisplayRenderNode_->renderDrawable_ = std::make_shared<DrawableV2::RSRenderNodeDrawable>(rsRenderNode);
    ASSERT_NE(rsDisplayRenderNode_->GetRenderDrawable(), nullptr);

    ASSERT_NE(virtualProcessor_->canvas_, nullptr);
    virtualProcessor_->CalculateTransform(*rsDisplayRenderNode_);
}

/**
 * @tc.name: ProcessDisplaySurfaceForRenderThread_001
 * @tc.desc: ProcessDisplaySurfaceForRenderThread Test, isExpand_ is true, return directly
 * @tc.type:FUNC
 * @tc.require:issuesIAJ4FW
 */
HWTEST_F(RSUniRenderVirtualProcessorTest, ProcessDisplaySurfaceForRenderThread_001, TestSize.Level2)
{
    virtualProcessor_->isExpand_ = true;
    virtualProcessor_->ProcessDisplaySurfaceForRenderThread(*displayDrawable_);
}

/**
 * @tc.name: ProcessDisplaySurfaceForRenderThread_002
 * @tc.desc: ProcessDisplaySurfaceForRenderThread Test, isExpand_ is false, canvas and Buffer are not null ,
 * renderEngine_  is not null, return
 * @tc.type:FUNC
 * @tc.require:issuesIAJ4FW
 */
HWTEST_F(RSUniRenderVirtualProcessorTest, ProcessDisplaySurfaceForRenderThread_002, TestSize.Level2)
{
    virtualProcessor_->isExpand_ = false;
    ASSERT_NE(virtualProcessor_->canvas_, nullptr);
    auto surfaceHandler = displayDrawable_->GetRSSurfaceHandlerOnDraw();
    surfaceHandler->buffer_.buffer = OHOS::SurfaceBuffer::Create();
    processor_->ProcessDisplaySurface(*rsDisplayRenderNode_);
    ASSERT_NE(displayDrawable_->GetRSSurfaceHandlerOnDraw()->GetBuffer(), nullptr);

    virtualProcessor_->renderEngine_ = RSUniRenderThread::Instance().GetRenderEngine();
    ASSERT_NE(virtualProcessor_->renderEngine_, nullptr);
    virtualProcessor_->ProcessDisplaySurfaceForRenderThread(*displayDrawable_);
}

/**
 * @tc.name: ProcessDisplaySurfaceForRenderThread_003
 * @tc.desc: ProcessDisplaySurfaceForRenderThread Test, isExpand_ is false, canvas_ is null, buffer not null, return
 * directly
 * @tc.type:FUNC
 * @tc.require:issuesIAJ4FW
 */
HWTEST_F(RSUniRenderVirtualProcessorTest, ProcessDisplaySurfaceForRenderThread_003, TestSize.Level2)
{
    virtualProcessor_->isExpand_ = false;
    virtualProcessor_->canvas_ = nullptr;
    auto surfaceHandler = displayDrawable_->GetRSSurfaceHandlerOnDraw();
    surfaceHandler->buffer_.buffer = OHOS::SurfaceBuffer::Create();
    processor_->ProcessDisplaySurface(*rsDisplayRenderNode_);
    ASSERT_NE(displayDrawable_->GetRSSurfaceHandlerOnDraw()->GetBuffer(), nullptr);
    virtualProcessor_->ProcessDisplaySurfaceForRenderThread(*displayDrawable_);
}

/**
 * @tc.name: ProcessDisplaySurfaceForRenderThread_004
 * @tc.desc: ProcessDisplaySurfaceForRenderThread Test, isExpand_ is false, canvas_ is null, buffer is null, return
 * directly
 * @tc.type:FUNC
 * @tc.require:issuesIAJ4FW
 */
HWTEST_F(RSUniRenderVirtualProcessorTest, ProcessDisplaySurfaceForRenderThread_004, TestSize.Level2)
{
    virtualProcessor_->isExpand_ = false;
    virtualProcessor_->canvas_ = nullptr;
    auto surfaceHandler = displayDrawable_->GetRSSurfaceHandlerOnDraw();
    surfaceHandler->buffer_.buffer = nullptr;
    ASSERT_EQ(displayDrawable_->GetRSSurfaceHandlerOnDraw()->GetBuffer(), nullptr);
    virtualProcessor_->ProcessDisplaySurfaceForRenderThread(*displayDrawable_);
}

/**
 * @tc.name: ProcessDisplaySurfaceForRenderThread_005
 * @tc.desc: ProcessDisplaySurfaceForRenderThread Test, isExpand_ is false, canvas_ is not null, buffer is null, return
 * directly
 * @tc.type:FUNC
 * @tc.require:issuesIAJ4FW
 */
HWTEST_F(RSUniRenderVirtualProcessorTest, ProcessDisplaySurfaceForRenderThread_005, TestSize.Level2)
{
    virtualProcessor_->isExpand_ = false;
    ASSERT_NE(nullptr, virtualProcessor_->canvas_);
    auto surfaceHandler = displayDrawable_->GetRSSurfaceHandlerOnDraw();
    surfaceHandler->buffer_.buffer = nullptr;
    ASSERT_EQ(displayDrawable_->GetRSSurfaceHandlerOnDraw()->GetBuffer(), nullptr);
    virtualProcessor_->ProcessDisplaySurfaceForRenderThread(*displayDrawable_);
}
} // namespace OHOS::Rosen