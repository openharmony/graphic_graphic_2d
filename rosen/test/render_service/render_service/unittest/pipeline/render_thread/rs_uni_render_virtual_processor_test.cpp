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
#include "drawable/rs_screen_render_node_drawable.h"
#include "params/rs_render_params.h"
#include "pipeline/render_thread/rs_render_engine.h"
#include "pipeline/render_thread/rs_uni_render_engine.h"
#include "pipeline/render_thread/rs_uni_render_virtual_processor.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/rs_processor_factory.h"
#include "common/rs_obj_abs_geometry.h"
#include "feature/round_corner_display/rs_rcd_surface_render_node.h"
#include "foundation/graphic/graphic_2d/rosen/test/render_service/render_service/unittest/pipeline/rs_test_util.h"
#include "params/rs_screen_render_params.h"
#include "platform/ohos/backend/rs_surface_ohos_gl.h"
#include "platform/ohos/backend/rs_surface_ohos_raster.h"
#include "surface_buffer_impl.h"

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
    std::shared_ptr<RSScreenRenderNode> RSScreenRenderNode_ = nullptr;
    RSScreenRenderNodeDrawable* displayDrawable_ = nullptr;
    RSLogicalDisplayRenderNodeDrawable* logicalDisplayDrawable_ = nullptr;

    RSDisplayNodeConfig config_;
    NodeId nodeId_ = 0;

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    class RSSurfaceOhosRasterTest : public RSSurfaceOhosRaster {
    public:
        explicit RSSurfaceOhosRasterTest(const sptr<Surface>& producer);
        ~RSSurfaceOhosRasterTest() = default;
        sptr<SurfaceBuffer> GetCurrentBuffer() override;
    };
};

void RSUniRenderVirtualProcessorTest::SetUpTestCase()
{
#ifdef RS_ENABLE_VK
    RsVulkanContext::SetRecyclable(false);
#endif
}
void RSUniRenderVirtualProcessorTest::TearDownTestCase() {}
void RSUniRenderVirtualProcessorTest::SetUp()
{
    processor_ = RSProcessorFactory::CreateProcessor(CompositeType::UNI_RENDER_MIRROR_COMPOSITE);
    ASSERT_NE(processor_, nullptr);
    virtualProcessor_ = std::static_pointer_cast<RSUniRenderVirtualProcessor>(processor_);
    ASSERT_NE(virtualProcessor_, nullptr);

    drawingCanvas_ = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    ASSERT_NE(drawingCanvas_, nullptr);
    virtualProcessor_->canvas_ = std::make_unique<RSPaintFilterCanvas>(drawingCanvas_.get());
    ASSERT_NE(virtualProcessor_->canvas_, nullptr);

    RSScreenRenderNode_ = std::make_shared<RSScreenRenderNode>(nodeId_, 1);
    ASSERT_NE(RSScreenRenderNode_, nullptr);
    displayDrawable_ =
        static_cast<RSScreenRenderNodeDrawable*>(RSScreenRenderNodeDrawable::OnGenerate(RSScreenRenderNode_));
    ASSERT_NE(displayDrawable_, nullptr);
    displayDrawable_->renderParams_ = std::make_unique<RSRenderParams>(nodeId_);
    ASSERT_NE(displayDrawable_->renderParams_, nullptr);
    RSTestUtil::InitRenderNodeGC();
}
void RSUniRenderVirtualProcessorTest::TearDown() {}
RSUniRenderVirtualProcessorTest::RSSurfaceOhosRasterTest::RSSurfaceOhosRasterTest(const sptr<Surface>& producer)
    : RSSurfaceOhosRaster(producer) {}

sptr<SurfaceBuffer> RSUniRenderVirtualProcessorTest::RSSurfaceOhosRasterTest::GetCurrentBuffer()
{
    return new SurfaceBufferImpl();
}

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
    auto type = CompositeType::UNI_RENDER_MIRROR_COMPOSITE;
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
    NodeId id = 0;
    int32_t offsetX = 0;
    int32_t offsetY = 0;
    RSScreenRenderNode RSScreenRenderNode(id, 1);
    auto processor = RSProcessorFactory::CreateProcessor(CompositeType::
        UNI_RENDER_MIRROR_COMPOSITE);
    auto& uniRenderThread = RSUniRenderThread::Instance();
    uniRenderThread.uniRenderEngine_ = std::make_shared<RSUniRenderEngine>();
    auto renderEngine = uniRenderThread.GetRenderEngine();
    ASSERT_NE(nullptr, processor);
    ASSERT_EQ(false, processor->Init(RSScreenRenderNode, offsetX, offsetY, INVALID_SCREEN_ID, renderEngine));
}

/**
 * @tc.name: InitForRenderThread001
 * @tc.desc: Test RSUniRenderVirtualProcessorTest.InitForRenderThread when main screen set wide color gamut or sRGB
 * @tc.type:FUNC
 * @tc.require: issueIB7AGV
 */
HWTEST_F(RSUniRenderVirtualProcessorTest, InitForRenderThread001, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    auto surface = Surface::CreateSurfaceAsConsumer("test_surface");
    ASSERT_NE(surface, nullptr);
    auto screenId = screenManager->CreateVirtualScreen("virtual_screen", 10, 10, surface, 0UL, 0, {});
    screenManager->SetVirtualScreenStatus(screenId, VirtualScreenStatus::VIRTUAL_SCREEN_PLAY);

    NodeId mainNodeId = 1;
    NodeId virtualNodeId = 2;
    auto mainNode = std::make_shared<RSScreenRenderNode>(mainNodeId, 1);
    mainNode->InitRenderParams();
    auto virtualNode = std::make_shared<RSScreenRenderNode>(virtualNodeId, 1);
    virtualNode->InitRenderParams();
    ASSERT_NE(mainNode->renderDrawable_, nullptr);
    ASSERT_NE(virtualNode->renderDrawable_, nullptr);
    ASSERT_NE(mainNode->renderDrawable_->renderParams_, nullptr);
    ASSERT_NE(virtualNode->renderDrawable_->renderParams_, nullptr);

    auto mainRenderDrawable = static_cast<RSScreenRenderNodeDrawable* >(mainNode->renderDrawable_.get());
    auto virtualRenderDrawable = static_cast<RSScreenRenderNodeDrawable* >(virtualNode->renderDrawable_.get());
    ASSERT_NE(mainRenderDrawable, nullptr);
    ASSERT_NE(virtualRenderDrawable, nullptr);
    auto mainRenderParams = static_cast<RSScreenRenderParams* >(mainRenderDrawable->GetRenderParams().get());
    auto virtualRenderParams = static_cast<RSScreenRenderParams* >(virtualRenderDrawable->GetRenderParams().get());
    ASSERT_NE(mainRenderParams, nullptr);
    ASSERT_NE(virtualRenderParams, nullptr);
    virtualRenderParams->mirrorSourceDrawable_ = mainNode->renderDrawable_;

    auto renderEngine = RSUniRenderThread::Instance().GetRenderEngine();
    ASSERT_NE(renderEngine, nullptr);
    renderEngine->Init();

    virtualRenderParams->newColorSpace_ = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3;
    mainRenderParams->newColorSpace_ = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3;
    auto processor = RSProcessorFactory::CreateProcessor(CompositeType::
        UNI_RENDER_MIRROR_COMPOSITE);
    auto virtualProcessor = std::static_pointer_cast<RSUniRenderVirtualProcessor>(processor);
    ASSERT_NE(virtualProcessor, nullptr);
    virtualProcessor->InitForRenderThread(*virtualRenderDrawable, renderEngine);
    ASSERT_EQ(virtualProcessor->renderFrameConfig_.colorGamut, mainRenderParams->GetNewColorSpace());

    mainRenderParams->newColorSpace_ = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB;
    auto newProcessor = RSProcessorFactory::CreateProcessor(CompositeType::
        UNI_RENDER_MIRROR_COMPOSITE);
    auto newVirtualProcessor = std::static_pointer_cast<RSUniRenderVirtualProcessor>(newProcessor);
    ASSERT_NE(newVirtualProcessor, nullptr);
    newVirtualProcessor->InitForRenderThread(*virtualRenderDrawable, renderEngine);
    ASSERT_EQ(newVirtualProcessor->renderFrameConfig_.colorGamut, mainRenderParams->GetNewColorSpace());
}

/**
 * @tc.name: InitForRenderThread002
 * @tc.desc: Test RSUniRenderVirtualProcessorTest.InitForRenderThread autoBufferRotation is on
 * @tc.type:FUNC
 * @tc.require: issueICGA54
 */
HWTEST_F(RSUniRenderVirtualProcessorTest, InitForRenderThread002, TestSize.Level1)
{
    auto screenManager = CreateOrGetScreenManager();
    auto surface = Surface::CreateSurfaceAsConsumer("test_surface");
    ASSERT_NE(surface, nullptr);
    auto screenId = screenManager->CreateVirtualScreen("virtual_screen", 10, 10, surface, 0UL, 0, {});
    screenManager->SetVirtualScreenStatus(screenId, VirtualScreenStatus::VIRTUAL_SCREEN_PLAY);

    NodeId mainNodeId = 1;
    NodeId virtualNodeId = 2;
    auto mainNode = std::make_shared<RSScreenRenderNode>(mainNodeId, 1);
    mainNode->InitRenderParams();
    auto virtualNode = std::make_shared<RSScreenRenderNode>(virtualNodeId, 1);
    virtualNode->InitRenderParams();
    ASSERT_NE(mainNode->renderDrawable_, nullptr);
    ASSERT_NE(virtualNode->renderDrawable_, nullptr);
    ASSERT_NE(mainNode->renderDrawable_->renderParams_, nullptr);
    ASSERT_NE(virtualNode->renderDrawable_->renderParams_, nullptr);

    auto mainRenderDrawable = static_cast<RSScreenRenderNodeDrawable* >(mainNode->renderDrawable_.get());
    auto virtualRenderDrawable = static_cast<RSScreenRenderNodeDrawable* >(virtualNode->renderDrawable_.get());
    ASSERT_NE(mainRenderDrawable, nullptr);
    ASSERT_NE(virtualRenderDrawable, nullptr);
    auto mainRenderParams = static_cast<RSScreenRenderParams* >(mainRenderDrawable->GetRenderParams().get());
    auto virtualRenderParams = static_cast<RSScreenRenderParams* >(virtualRenderDrawable->GetRenderParams().get());
    ASSERT_NE(mainRenderParams, nullptr);
    ASSERT_NE(virtualRenderParams, nullptr);
    virtualRenderParams->mirrorSourceDrawable_ = mainNode->renderDrawable_;

    auto renderEngine = RSUniRenderThread::Instance().GetRenderEngine();
    ASSERT_NE(renderEngine, nullptr);
    renderEngine->Init();

    auto processor = RSProcessorFactory::CreateProcessor(CompositeType::
        UNI_RENDER_MIRROR_COMPOSITE);
    auto virtualProcessor = std::static_pointer_cast<RSUniRenderVirtualProcessor>(processor);
    ASSERT_NE(virtualProcessor, nullptr);

    virtualProcessor->InitForRenderThread(*mainRenderDrawable, renderEngine);
    virtualProcessor->InitForRenderThread(*virtualRenderDrawable, renderEngine);
    
    screenManager->SetVirtualScreenAutoRotation(screenId, true);
    virtualProcessor->InitForRenderThread(*virtualRenderDrawable, renderEngine);

    virtualRenderDrawable->SetFirstBufferRotation(ScreenRotation::ROTATION_90);
    virtualProcessor->InitForRenderThread(*virtualRenderDrawable, renderEngine);
    virtualProcessor->InitForRenderThread(*mainRenderDrawable, renderEngine);
    mainRenderParams->SetNewColorSpace(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
    virtualProcessor->InitForRenderThread(*mainRenderDrawable, renderEngine);
    screenManager->RemoveVirtualScreen(screenId);
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
    auto processor = RSProcessorFactory::CreateProcessor(CompositeType::
        UNI_RENDER_MIRROR_COMPOSITE);
    ASSERT_NE(processor, nullptr);
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
    NodeId id = 0;
    RSScreenRenderNode screenRenderNode(id, 1);
    auto processor = RSProcessorFactory::CreateProcessor(CompositeType::
        UNI_RENDER_MIRROR_COMPOSITE);
    ASSERT_NE(nullptr, processor);
    processor->ProcessScreenSurface(screenRenderNode);
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
    auto processor = RSProcessorFactory::CreateProcessor(CompositeType::
        UNI_RENDER_MIRROR_COMPOSITE);
    ASSERT_NE(processor, nullptr);
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
    auto processor = RSProcessorFactory::CreateProcessor(CompositeType::
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
 * @tc.name: Fill
 * @tc.desc: Fill Test
 * @tc.type: FUNC
 * @tc.require: issueI992VW
 */
HWTEST_F(RSUniRenderVirtualProcessorTest, Fill, TestSize.Level2)
{
    auto processor = RSProcessorFactory::CreateProcessor(CompositeType::
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
    auto processor = RSProcessorFactory::CreateProcessor(CompositeType::
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
    auto node = RSRcdSurfaceRenderNode::Create(nodeId, type);
    processor->ProcessRcdSurface(*node);
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
    NodeId id = 0;
    auto screenRenderNode = std::make_shared<RSScreenRenderNode>(id, 1);
    ASSERT_NE(screenRenderNode, nullptr);
    auto processor = std::make_shared<RSUniRenderVirtualProcessor>();
    ASSERT_NE(processor, nullptr);
    processor->ProcessScreenSurface(*screenRenderNode);
    Drawing::Canvas canvas;
    processor->canvas_ = std::make_unique<RSPaintFilterCanvas>(&canvas);
    auto displayDrawable =
        static_cast<RSScreenRenderNodeDrawable*>(RSScreenRenderNodeDrawable::OnGenerate(screenRenderNode));
    auto surfaceHandler = displayDrawable->GetRSSurfaceHandlerOnDraw();
    surfaceHandler->buffer_.buffer = OHOS::SurfaceBuffer::Create();
    processor->ProcessScreenSurface(*screenRenderNode);
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
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(virtualProcessor_, nullptr);
    virtualProcessor_->CanvasInit(*logicalDisplayDrawable_);
}

/**
 * @tc.name: CanvasInit_002
 * @tc.desc: CanvasInit Test, displayDrawable_ is not FirstTimeToProcessor, canvasRotation of virtualProcessor_ is true
 * @tc.type:FUNC
 * @tc.require:issuesIAJ4FW
 */
HWTEST_F(RSUniRenderVirtualProcessorTest, CanvasInit_002, TestSize.Level2)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(virtualProcessor_, nullptr);
    virtualProcessor_->canvasRotation_ = true;
    virtualProcessor_->CanvasInit(*logicalDisplayDrawable_);
}

/**
 * @tc.name: CanvasInit_003
 * @tc.desc: CanvasInit Test, autoBufferRotation conditions
 * @tc.type:FUNC
 * @tc.require:issueICGA54
 */
HWTEST_F(RSUniRenderVirtualProcessorTest, CanvasInit_003, TestSize.Level2)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(virtualProcessor_, nullptr);
    virtualProcessor_->canvasRotation_ = false;
    virtualProcessor_->CanvasInit(*logicalDisplayDrawable_);

    virtualProcessor_->autoBufferRotation_ = true;
    virtualProcessor_->CanvasInit(*logicalDisplayDrawable_);
}

/**
 * @tc.name: CheckIfBufferSizeNeedChangeTest
 * @tc.desc: CheckIfBufferSizeNeedChange Test
 * @tc.type:FUNC
 * @tc.require:issueICGA54
 */
HWTEST_F(RSUniRenderVirtualProcessorTest, CheckIfBufferSizeNeedChangeTest, TestSize.Level2)
{
    ASSERT_NE(displayDrawable_, nullptr);
    ASSERT_NE(virtualProcessor_, nullptr);
    EXPECT_TRUE(virtualProcessor_->CheckIfBufferSizeNeedChange(
        ScreenRotation::ROTATION_0, ScreenRotation::ROTATION_90));
    EXPECT_FALSE(virtualProcessor_->CheckIfBufferSizeNeedChange(
        ScreenRotation::ROTATION_0, ScreenRotation::ROTATION_0));
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
    ASSERT_NE(virtualProcessor_, nullptr);
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
 * @tc.name: SetRoiRegionToCodec_001
 * @tc.desc: test SetRoiRegionToCodec while renderFrame_ is nullptr
 * @tc.type:FUNC
 * @tc.require:issuesIBHR8N
 */
HWTEST_F(RSUniRenderVirtualProcessorTest, SetRoiRegionToCodec001, TestSize.Level2)
{
    ASSERT_NE(virtualProcessor_, nullptr);
    virtualProcessor_->renderFrame_ = nullptr;

    std::vector<RectI> damageRegion {};
    ASSERT_EQ(virtualProcessor_->SetRoiRegionToCodec(damageRegion), GSERROR_INVALID_ARGUMENTS);
}

/**
 * @tc.name: ProcessDisplaySurfaceForRenderThread_001
 * @tc.desc: ProcessDisplaySurfaceForRenderThread Test, isExpand_ is true, return directly
 * @tc.type:FUNC
 * @tc.require:issuesIAJ4FW
 */
HWTEST_F(RSUniRenderVirtualProcessorTest, ProcessDisplaySurfaceForRenderThread_001, TestSize.Level2)
{
    ASSERT_NE(virtualProcessor_, nullptr);
    virtualProcessor_->ProcessScreenSurfaceForRenderThread(*displayDrawable_);
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
    ASSERT_NE(virtualProcessor_->canvas_, nullptr);
    auto surfaceHandler = displayDrawable_->GetRSSurfaceHandlerOnDraw();
    surfaceHandler->buffer_.buffer = OHOS::SurfaceBuffer::Create();
    ASSERT_NE(displayDrawable_->GetRSSurfaceHandlerOnDraw()->GetBuffer(), nullptr);

    virtualProcessor_->renderEngine_ = RSUniRenderThread::Instance().GetRenderEngine();
    ASSERT_NE(virtualProcessor_->renderEngine_, nullptr);
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
    virtualProcessor_->canvas_ = nullptr;
    auto surfaceHandler = displayDrawable_->GetRSSurfaceHandlerOnDraw();
    surfaceHandler->buffer_.buffer = OHOS::SurfaceBuffer::Create();
    processor_->ProcessScreenSurface(*RSScreenRenderNode_);
    ASSERT_NE(displayDrawable_->GetRSSurfaceHandlerOnDraw()->GetBuffer(), nullptr);
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
    virtualProcessor_->canvas_ = nullptr;
    auto surfaceHandler = displayDrawable_->GetRSSurfaceHandlerOnDraw();
    surfaceHandler->buffer_.buffer = nullptr;
    ASSERT_EQ(displayDrawable_->GetRSSurfaceHandlerOnDraw()->GetBuffer(), nullptr);
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
    ASSERT_NE(nullptr, virtualProcessor_->canvas_);
    auto surfaceHandler = displayDrawable_->GetRSSurfaceHandlerOnDraw();
    surfaceHandler->buffer_.buffer = nullptr;
    ASSERT_EQ(displayDrawable_->GetRSSurfaceHandlerOnDraw()->GetBuffer(), nullptr);
}

/**
 * @tc.name: CanvasClipRegionForUniscaleMode
 * @tc.desc: Canvas do clipRegion, When performing uniscale on a virtual screen.
 * @tc.type:FUNC
 * @tc.require:issuesIAOEPL
 */
HWTEST_F(RSUniRenderVirtualProcessorTest, CanvasClipRegionForUniscaleMode, TestSize.Level2)
{
    auto processor = RSProcessorFactory::CreateProcessor(CompositeType::
        UNI_RENDER_MIRROR_COMPOSITE);
    auto virtualProcessor = std::static_pointer_cast<RSUniRenderVirtualProcessor>(processor);
    ASSERT_NE(nullptr, virtualProcessor);
    virtualProcessor_->CanvasClipRegionForUniscaleMode();
}

/**
 * @tc.name: ProcessCacheImage
 * @tc.desc: draw virtual screen by cache image.
 * @tc.type:FUNC
 * @tc.require:issuesIBIPST
 */
HWTEST_F(RSUniRenderVirtualProcessorTest, ProcessCacheImage, TestSize.Level2)
{
    auto processor = RSProcessorFactory::CreateProcessor(CompositeType::
        UNI_RENDER_MIRROR_COMPOSITE);
    auto virtualProcessor = std::static_pointer_cast<RSUniRenderVirtualProcessor>(processor);
    ASSERT_NE(nullptr, virtualProcessor);
    
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

    virtualProcessor->ProcessCacheImage(*image);
}

/**
 * @tc.name: EnableSlrScale
 * @tc.desc: test enable slr scale switch.
 * @tc.type:FUNC
 * @tc.require:issuesIBJLYR
 */
HWTEST_F(RSUniRenderVirtualProcessorTest, EnableSlrScale, TestSize.Level2)
{
    auto processor = RSProcessorFactory::CreateProcessor(CompositeType::
        UNI_RENDER_MIRROR_COMPOSITE);
    auto virtualProcessor = std::static_pointer_cast<RSUniRenderVirtualProcessor>(processor);
    virtualProcessor->EnableSlrScale();
    ASSERT_NE(nullptr, virtualProcessor);
}

/**
 * @tc.name: SetSpecialLayerType
 * @tc.desc: test draw virtual mirror screen by copy.
 * @tc.type:FUNC
 * @tc.require:issuesIBKZFK
 */
HWTEST_F(RSUniRenderVirtualProcessorTest, SetSpecialLayerType, TestSize.Level2)
{
    auto processor = RSProcessorFactory::CreateProcessor(CompositeType::
        UNI_RENDER_MIRROR_COMPOSITE);
    auto virtualProcessor = std::static_pointer_cast<RSUniRenderVirtualProcessor>(processor);
    ASSERT_NE(nullptr, virtualProcessor);
    virtualProcessor->SetDrawVirtualMirrorCopy(true);
}

/**
 * @tc.name: SetColorSpaceForMetadata
 * @tc.desc: SetColorSpaceForMetadata test.
 * @tc.type:FUNC
 * @tc.require:issuesIBKZFK
 */
HWTEST_F(RSUniRenderVirtualProcessorTest, SetColorSpaceForMetadata, TestSize.Level2)
{
    auto processor = RSProcessorFactory::CreateProcessor(CompositeType::
        UNI_RENDER_MIRROR_COMPOSITE);
    auto virtualProcessor = std::static_pointer_cast<RSUniRenderVirtualProcessor>(processor);
    EXPECT_NE(nullptr, virtualProcessor);
    GraphicColorGamut colorGamut1 = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3;

    virtualProcessor->renderFrame_ = nullptr;
    auto res = virtualProcessor->SetColorSpaceForMetadata(colorGamut1);
    EXPECT_EQ(GSERROR_INVALID_ARGUMENTS, res);

    virtualProcessor->renderFrame_ = std::make_unique<RSRenderFrame>(nullptr, nullptr);
    res = virtualProcessor->SetColorSpaceForMetadata(colorGamut1);
    EXPECT_EQ(GSERROR_INVALID_ARGUMENTS, res);

    auto csurf = IConsumerSurface::Create();
    auto producer = csurf->GetProducer();
    auto pSurface = Surface::CreateSurfaceAsProducer(producer);
    std::shared_ptr<RSSurfaceOhos> rsSurface1 = std::make_shared<RSSurfaceOhosRaster>(pSurface);
    EXPECT_NE(nullptr, rsSurface1);

    virtualProcessor->renderFrame_ = std::make_unique<RSRenderFrame>(rsSurface1, nullptr);
    res = virtualProcessor->SetColorSpaceForMetadata(colorGamut1);
    EXPECT_EQ(GSERROR_NO_BUFFER, res);

    std::shared_ptr<RSSurfaceOhos> rsSurface2 = std::make_shared<RSSurfaceOhosRasterTest>(pSurface);
    EXPECT_NE(nullptr, rsSurface2);
    GraphicColorGamut colorGamut2 = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DCI_P3;
    virtualProcessor->renderFrame_ = std::make_unique<RSRenderFrame>(rsSurface2, nullptr);
    res = virtualProcessor->SetColorSpaceForMetadata(colorGamut2);
    EXPECT_EQ(GSERROR_OK, res);

    res = virtualProcessor->SetColorSpaceForMetadata(colorGamut1);
    EXPECT_EQ(GSERROR_NOT_INIT, res);
}
/**
 * @tc.name: SetRoiRegionToCodec002
 * @tc.desc: SetRoiRegionToCodec002 test.
 * @tc.type:FUNC
 * @tc.require:issuesIBKZFK
 */
HWTEST_F(RSUniRenderVirtualProcessorTest, SetRoiRegionToCodec002, TestSize.Level2)
{
    auto processor = RSProcessorFactory::CreateProcessor(CompositeType::
        UNI_RENDER_MIRROR_COMPOSITE);
    auto virtualProcessor = std::static_pointer_cast<RSUniRenderVirtualProcessor>(processor);
    EXPECT_NE(nullptr, virtualProcessor);
    auto csurf = IConsumerSurface::Create();
    auto producer = csurf->GetProducer();
    auto pSurface = Surface::CreateSurfaceAsProducer(producer);
    std::shared_ptr<RSSurfaceOhos> rsSurface1 = std::make_shared<RSSurfaceOhosRaster>(pSurface);
    EXPECT_NE(nullptr, rsSurface1);

    virtualProcessor->renderFrame_ = std::make_unique<RSRenderFrame>(rsSurface1, nullptr);


    RectI rect = RectI(1, 2, 3, 4);
    std::vector<RectI> vRect = { rect };
    auto res = virtualProcessor->SetRoiRegionToCodec(vRect);

    EXPECT_EQ(GSERROR_NO_BUFFER, res);

    std::shared_ptr<RSSurfaceOhos> rsSurface2 = std::make_shared<RSSurfaceOhosRasterTest>(pSurface);
    EXPECT_NE(nullptr, rsSurface2);

    virtualProcessor->renderFrame_ = std::make_unique<RSRenderFrame>(rsSurface2, nullptr);
    res = virtualProcessor->SetRoiRegionToCodec(vRect);
    EXPECT_EQ(GSERROR_NOT_INIT, res);

    vRect.emplace_back(rect);
    res = virtualProcessor->SetRoiRegionToCodec(vRect);
    EXPECT_EQ(GSERROR_NOT_INIT, res);
}
} // namespace OHOS::Rosen