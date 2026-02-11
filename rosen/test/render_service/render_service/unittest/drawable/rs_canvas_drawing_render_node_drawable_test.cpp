/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include "common/rs_common_def.h"
#include "drawable/rs_canvas_drawing_render_node_drawable.h"
#include "feature_cfg/feature_param/performance_feature/node_mem_release_param.h"
#include "params/rs_canvas_drawing_render_params.h"
#include "pipeline/rs_paint_filter_canvas.h"
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
#include "memory/rs_canvas_dma_buffer_cache.h"
#include "pipeline/main_thread/rs_main_thread.h"
#endif
#include "pipeline/render_thread/rs_render_engine.h"
#include "pipeline/rs_canvas_drawing_render_node.h"
#include "pipeline/rs_context.h"
#include "platform/ohos/backend/surface_buffer_utils.h"
#include "recording/draw_cmd_list.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::DrawableV2;

namespace OHOS::Rosen {
static constexpr uint64_t TEST_ID = 126;
constexpr NodeId DEFAULT_ID = 0xFFFF;
class RSCanvasDrawingRenderNodeDrawableTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline NodeId id = DEFAULT_ID;
    static std::shared_ptr<RSCanvasDrawingRenderNodeDrawable> CreateDrawable();
};

void RSCanvasDrawingRenderNodeDrawableTest::SetUpTestCase()
{
#ifdef RS_ENABLE_VK
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN) {
        RsVulkanContext::GetSingleton();
    }
#endif
}
void RSCanvasDrawingRenderNodeDrawableTest::TearDownTestCase() {}
void RSCanvasDrawingRenderNodeDrawableTest::SetUp() {}
void RSCanvasDrawingRenderNodeDrawableTest::TearDown() {}

std::shared_ptr<RSCanvasDrawingRenderNodeDrawable> RSCanvasDrawingRenderNodeDrawableTest::CreateDrawable()
{
    auto rsContext = std::make_shared<RSContext>();
    auto renderNode = std::make_shared<RSCanvasDrawingRenderNode>(id, rsContext->weak_from_this());
    auto drawable = std::static_pointer_cast<RSCanvasDrawingRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(renderNode));
    return drawable;
}

/**
 * @tc.name: CreateCanvasDrawingRenderNodeDrawableTest
 * @tc.desc: Test If CreateCanvasDrawingRenderNodeDrawable Can Be Created
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSCanvasDrawingRenderNodeDrawableTest, CreateCanvasDrawingRenderNodeDrawable, TestSize.Level1)
{
    NodeId nodeId = 1;
    auto rsContext = std::make_shared<RSContext>();
    auto canvasDrawingNode = std::make_shared<RSCanvasDrawingRenderNode>(nodeId, rsContext->weak_from_this());
    auto drawable = RSCanvasDrawingRenderNodeDrawable::OnGenerate(canvasDrawingNode);
    ASSERT_NE(drawable, nullptr);
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
    std::string info;
    drawable->DumpSubDrawableTree(info);
    if (NodeMemReleaseParam::IsCanvasDrawingNodeDMAMemEnabled() && RSUniRenderJudgement::IsUniRender() &&
        RSSystemProperties::GetCanvasDrawingNodePreAllocateDmaEnabled()) {
        ASSERT_EQ(info, ", dmaAllocationCount:0, dmaFallbackCount:0");
    } else {
        ASSERT_EQ(info, "");
    }
#endif
}

#ifdef RS_ENABLE_VK
/**
 * @tc.name: ReleaseSurfaceVK
 * @tc.desc: Test If ReleaseSurfaceVK Can Run
 * @tc.type: FUNC
 * @tc.require: issueIB2B14
 */
HWTEST_F(RSCanvasDrawingRenderNodeDrawableTest, ReleaseSurfaceVKTest, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSCanvasDrawingRenderNode>(0, rsContext->weak_from_this());
    auto drawable = std::make_shared<RSCanvasDrawingRenderNodeDrawable>(std::move(node));
    int width = 10;
    int height = 10;
    bool isDmaBackendTexture = false;
    auto res = drawable->ReleaseSurfaceVK(width, height, isDmaBackendTexture);
    EXPECT_TRUE(res);

    drawable->backendTexture_ = Drawing::BackendTexture(false);
    res = drawable->ReleaseSurfaceVK(width, height, isDmaBackendTexture);
    EXPECT_TRUE(res);
}
#endif

/**
 * @tc.name: OnDraw
 * @tc.desc: Test If OnDraw Can Run
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSCanvasDrawingRenderNodeDrawableTest, OnDrawTest, TestSize.Level1)
{
    auto drawable = RSCanvasDrawingRenderNodeDrawableTest::CreateDrawable();
    drawable->renderParams_ = std::make_unique<RSRenderParams>(0);

    Drawing::Canvas rawCanvas;
    RSPaintFilterCanvas canvas(&rawCanvas);
    drawable->OnDraw(canvas);
    ASSERT_FALSE(drawable->ShouldPaint());
    drawable->renderParams_->shouldPaint_ = true;
    drawable->renderParams_->contentEmpty_ = false;
    drawable->OnDraw(canvas);
    ASSERT_TRUE(drawable->ShouldPaint());
    ASSERT_FALSE(drawable->renderParams_->GetCanvasDrawingSurfaceChanged());

    drawable->renderParams_->canvasDrawingNodeSurfaceChanged_ = true;
    drawable->OnDraw(canvas);
    ASSERT_TRUE(drawable->ShouldPaint());
    ASSERT_FALSE(drawable->renderParams_->GetCanvasDrawingSurfaceChanged());

    rawCanvas.recordingState_ = true;
    RSUniRenderThread::Instance().Sync(std::make_unique<RSRenderThreadParams>());
    drawable->OnDraw(canvas);
    RSUniRenderThread::Instance().GetRSRenderThreadParams()->isOpDropped_ = true;
    drawable->OnDraw(canvas);
    ASSERT_NE(drawable->renderParams_, nullptr);
    RSUniRenderThread::Instance().Sync(nullptr);
}

/**
 * @tc.name: OnDrawWithoutChildren
 * @tc.desc: OnDrawWithoutChildren
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasDrawingRenderNodeDrawableTest, OnDrawWithoutChildren, TestSize.Level1)
{
    auto drawable = RSCanvasDrawingRenderNodeDrawableTest::CreateDrawable();
    drawable->renderParams_ = std::make_unique<RSRenderParams>(0);

    Drawing::Canvas rawCanvas;
    RSPaintFilterCanvas canvas(&rawCanvas);
    drawable->renderParams_->shouldPaint_ = true;
    drawable->renderParams_->contentEmpty_ = false;
    RSUniRenderThread::GetCaptureParam().endNodeId_ = drawable->renderParams_->GetId();
    rawCanvas.SetUICapture(true);
    drawable->OnDraw(canvas);
    ASSERT_FALSE(drawable->renderParams_->GetCanvasDrawingSurfaceChanged());
    RSUniRenderThread::GetCaptureParam().endNodeId_ = INVALID_NODEID;
    drawable->renderParams_->canvasDrawingNodeSurfaceChanged_ = true;
    drawable->OnDraw(canvas);
    ASSERT_FALSE(drawable->renderParams_->GetCanvasDrawingSurfaceChanged());
}

/**
 * @tc.name: DrawRenderContent
 * @tc.desc: Test If DrawRenderContent Can Run
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSCanvasDrawingRenderNodeDrawableTest, DrawRenderContentTest, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSCanvasDrawingRenderNode>(0, rsContext->weak_from_this());
    auto drawable = std::make_shared<RSCanvasDrawingRenderNodeDrawable>(std::move(node));
    Drawing::Canvas canvas;
    const Drawing::Rect dst(1.0f, 1.0f, 1.0f, 1.0f);
    drawable->renderParams_ = std::make_unique<RSRenderParams>(0);
    RSUniRenderThread::Instance().Sync(std::make_unique<RSRenderThreadParams>());
    drawable->DrawRenderContent(canvas, dst);

    drawable->renderParams_->frameRect_ = { 0.f, 0.f, 1.f, 1.f }; // for test
    drawable->renderParams_->frameGravity_ = Gravity::CENTER;
    drawable->DrawRenderContent(canvas, dst);
    EXPECT_EQ(drawable->image_, nullptr);

    drawable->image_ = std::make_shared<Drawing::Image>();
    drawable->DrawRenderContent(canvas, dst);
    EXPECT_NE(drawable->image_, nullptr);
    RSUniRenderThread::Instance().Sync(nullptr);
}

/**
 * @tc.name: PlaybackInCorrespondThread
 * @tc.desc: Test If PlaybackInCorrespondThread Can Run
 * @tc.type: FUNC
 * @tc.require: issueIAFX7M
 */
HWTEST_F(RSCanvasDrawingRenderNodeDrawableTest, PlaybackInCorrespondThreadTest, TestSize.Level1)
{
    NodeId nodeId = 1;
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSCanvasDrawingRenderNode>(nodeId, rsContext->weak_from_this());
    auto drawable = std::make_shared<RSCanvasDrawingRenderNodeDrawable>(std::move(node));
    drawable->PostPlaybackInCorrespondThread();
    ASSERT_FALSE(drawable->canvas_);

    drawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    drawable->PostPlaybackInCorrespondThread();
    ASSERT_FALSE(drawable->canvas_);

    drawable->needDraw_ = true;
    drawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    drawable->PostPlaybackInCorrespondThread();
    ASSERT_FALSE(drawable->canvas_);

    auto canvas = std::make_shared<Drawing::Canvas>();
    drawable->canvas_ = std::make_shared<RSPaintFilterCanvas>(canvas.get());
    drawable->PostPlaybackInCorrespondThread();
    ASSERT_TRUE(drawable->canvas_);

    canvas->gpuContext_ = std::make_shared<Drawing::GPUContext>();
    drawable->PostPlaybackInCorrespondThread();
    ASSERT_TRUE(drawable->canvas_);

    auto surface_ = std::make_shared<Drawing::Surface>();
    drawable->curThreadInfo_.second(surface_);
    ASSERT_TRUE(surface_);
}

/**
 * @tc.name: InitSurface
 * @tc.desc: Test If InitSurface Can Run
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSCanvasDrawingRenderNodeDrawableTest, InitSurfaceTest, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSCanvasDrawingRenderNode>(0, rsContext->weak_from_this());
    auto drawable = std::make_shared<RSCanvasDrawingRenderNodeDrawable>(std::move(node));
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    int width = 1;
    int height = 1;
    drawable->surface_ = nullptr;
    auto result = drawable->InitSurface(width, height, canvas);
    EXPECT_EQ(result, true);

    drawable->surface_ = std::make_shared<Drawing::Surface>();
    drawable->surface_->cachedCanvas_ = std::make_shared<Drawing::Canvas>(0, 0);
    result = drawable->InitSurface(width, height, canvas);
    EXPECT_EQ(result, true);
}

/**
 * @tc.name: InitSurfaceForVK
 * @tc.desc: Test If InitSurfaceForVK Can Run
 * @tc.type: FUNC
 * @tc.require: issueIAFX7M
 */
HWTEST_F(RSCanvasDrawingRenderNodeDrawableTest, InitSurfaceForVKTest, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSCanvasDrawingRenderNode>(0, rsContext->weak_from_this());
    auto drawable = std::make_shared<RSCanvasDrawingRenderNodeDrawable>(std::move(node));
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    int width = 1;
    int height = 1;
    drawable->surface_ = std::make_shared<Drawing::Surface>();
    drawable->surface_->cachedCanvas_ = std::make_shared<Drawing::Canvas>(0, 0);
    bool result = drawable->InitSurfaceForVK(width, height, canvas);
    EXPECT_EQ(result, true);
}

/**
 * @tc.name: InitSurfaceForGL
 * @tc.desc: Test InitSurfaceForGL Can Run
 * @tc.type: FUNC
 * @tc.require: issueIB1KMY
 */
HWTEST_F(RSCanvasDrawingRenderNodeDrawableTest, InitSurfaceForGLTest, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSCanvasDrawingRenderNode>(0, rsContext->weak_from_this());
    auto drawable = std::make_shared<RSCanvasDrawingRenderNodeDrawable>(std::move(node));
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    int width = 1;
    int height = 1;
    drawable->surface_ = std::make_shared<Drawing::Surface>();
    drawable->surface_->cachedCanvas_ = std::make_shared<Drawing::Canvas>(0, 0);
    bool result = drawable->InitSurfaceForGL(width, height, canvas);
    EXPECT_EQ(result, true);
}

/**
 * @tc.name: FlushForGL
 * @tc.desc: Test If FlushForGL Can Run
 * @tc.type: FUNC
 * @tc.require: issueIAFX7M
 */
HWTEST_F(RSCanvasDrawingRenderNodeDrawableTest, FlushForGLTest, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSCanvasDrawingRenderNode>(0, rsContext->weak_from_this());
    auto drawable = std::make_shared<RSCanvasDrawingRenderNodeDrawable>(std::move(node));
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas rscanvas(&drawingCanvas);
    float width = 1.f;
    float height = 1.f;
    NodeId nodeId = 0;
    std::shared_ptr<RSContext> context = std::make_shared<RSContext>();
    drawable->surface_ = std::make_shared<Drawing::Surface>();
    drawable->surface_->cachedCanvas_ = std::make_shared<Drawing::Canvas>(0, 0);
    drawable->image_ = std::make_shared<Drawing::Image>();
    drawable->FlushForGL(width, height, context, nodeId, rscanvas);
    ASSERT_FALSE(drawable->recordingCanvas_);

#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    drawable->curThreadInfo_.first = INVALID_NODEID;
    drawable->FlushForGL(width, height, context, nodeId, rscanvas);
    ASSERT_FALSE(drawable->backendTexture_.IsValid());
#endif

    drawable->backendTexture_.isValid_ = true;
    drawable->FlushForGL(width, height, context, nodeId, rscanvas);
    ASSERT_TRUE(drawable->backendTexture_.IsValid());

    drawable->recordingCanvas_ = std::make_unique<ExtendRecordingCanvas>(0, 0);
    drawable->FlushForGL(width, height, context, nodeId, rscanvas);
    drawable->recordingCanvas_->cmdList_ =
        std::make_shared<Drawing::DrawCmdList>(Drawing::DrawCmdList::UnmarshalMode::DEFERRED);
    drawable->FlushForGL(width, height, context, nodeId, rscanvas);
    Drawing::Paint paint;
    std::shared_ptr<Drawing::DrawWithPaintOpItem> item =
        std::make_shared<Drawing::DrawWithPaintOpItem>(paint, Drawing::DrawOpItem::Type::OPITEM_HEAD);
    drawable->recordingCanvas_->cmdList_->drawOpItems_.push_back(item);
    drawable->FlushForGL(width, height, context, nodeId, rscanvas);
    ASSERT_TRUE(drawable->recordingCanvas_->GetDrawCmdList());
}

/**
 * @tc.name: FlushForVK
 * @tc.desc: Test If FlushForVK Can Run
 * @tc.type: FUNC
 * @tc.require: issueIAFX7M
 */
HWTEST_F(RSCanvasDrawingRenderNodeDrawableTest, FlushForVKTest, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSCanvasDrawingRenderNode>(0, rsContext->weak_from_this());
    auto drawable = std::make_shared<RSCanvasDrawingRenderNodeDrawable>(std::move(node));
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas rscanvas(&drawingCanvas);
    float width = 1.f;
    float height = 1.f;
    NodeId nodeId = 0;
    std::shared_ptr<RSContext> context = std::make_shared<RSContext>();
    drawable->surface_ = std::make_shared<Drawing::Surface>();
    drawable->surface_->cachedCanvas_ = std::make_shared<Drawing::Canvas>(0, 0);
    drawable->image_ = std::make_shared<Drawing::Image>();
    drawable->FlushForVK(width, height, context, nodeId, rscanvas);
    ASSERT_FALSE(drawable->recordingCanvas_);

    NodeMemReleaseParam::SetCanvasDrawingNodeDMAMemEnabled(false);
    ASSERT_FALSE(NodeMemReleaseParam::IsCanvasDrawingNodeDMAMemEnabled());
    drawable->FlushForVK(width, height, context, nodeId, rscanvas);
    NodeMemReleaseParam::SetCanvasDrawingNodeDMAMemEnabled(true);

    drawable->recordingCanvas_ = std::make_shared<ExtendRecordingCanvas>(width, height, false);
    drawable->FlushForVK(width, height, context, nodeId, rscanvas);
    drawable->recordingCanvas_->cmdList_ =
        std::make_shared<Drawing::DrawCmdList>(Drawing::DrawCmdList::UnmarshalMode::DEFERRED);
    drawable->FlushForVK(width, height, context, nodeId, rscanvas);
    Drawing::Paint paint;
    std::shared_ptr<Drawing::DrawWithPaintOpItem> item =
        std::make_shared<Drawing::DrawWithPaintOpItem>(paint, Drawing::DrawOpItem::Type::OPITEM_HEAD);
    drawable->recordingCanvas_->cmdList_->drawOpItems_.push_back(item);
    drawable->FlushForVK(width, height, context, nodeId, rscanvas);
    ASSERT_TRUE(drawable->recordingCanvas_->GetDrawCmdList());
}

/**
 * @tc.name: Flush
 * @tc.desc: Test If Flush Can Run
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSCanvasDrawingRenderNodeDrawableTest, FlushTest, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSCanvasDrawingRenderNode>(0, rsContext->weak_from_this());
    auto drawable = std::make_shared<RSCanvasDrawingRenderNodeDrawable>(std::move(node));
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    int width = 1;
    float height = 1.0;
    std::shared_ptr<RSContext> context = std::make_shared<RSContext>();
    drawable->surface_ = std::make_shared<Drawing::Surface>();
    drawable->captureImage_ = std::make_shared<Drawing::Image>();
    drawable->image_ = std::make_shared<Drawing::Image>();
    drawable->Flush(width, height, context, id, canvas);
    EXPECT_EQ(drawable->recordingCanvas_, nullptr);

#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    drawable->curThreadInfo_.first = INVALID_NODEID;
    drawable->Flush(width, height, context, id, canvas);
    EXPECT_EQ(drawable->recordingCanvas_, nullptr);
#endif

    drawable->recordingCanvas_ = std::make_unique<ExtendRecordingCanvas>(0, 0);
    drawable->Flush(width, height, context, id, canvas);
    EXPECT_EQ(drawable->canvas_, nullptr);

    drawable->recordingCanvas_->cmdList_ = std::make_shared<Drawing::DrawCmdList>();
    Drawing::Paint paint;
    std::shared_ptr<Drawing::DrawWithPaintOpItem> item =
        std::make_shared<Drawing::DrawWithPaintOpItem>(paint, Drawing::DrawOpItem::Type::OPITEM_HEAD);
    drawable->recordingCanvas_->cmdList_->drawOpItems_.push_back(item);
    drawable->Flush(width, height, context, id, canvas);
    EXPECT_NE(drawable->canvas_, nullptr);

    drawable->recordingCanvas_ = nullptr;
    drawable->canvas_ = nullptr;
    drawable->Flush(width, height, context, id, canvas);
}

/**
 * @tc.name: ProcessCPURenderInBackgroundThread
 * @tc.desc: Test If ProcessCPURenderInBackgroundThread Can Run
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSCanvasDrawingRenderNodeDrawableTest, ProcessCPURenderInBackgroundThreadTest, TestSize.Level1)
{
    RSRenderNodeDrawable::Registrar registrar;
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSCanvasDrawingRenderNode>(1, rsContext->weak_from_this());
    node->GetRenderParams();
    EXPECT_NE(node->renderDrawable_, nullptr);

    auto drawable = std::make_shared<RSCanvasDrawingRenderNodeDrawable>(std::move(node));
    std::shared_ptr<RSContext> ctx = std::make_shared<RSContext>();
    char dataBuffer[1];
    const void* ptr = dataBuffer;
    size_t size = sizeof(dataBuffer);
    Drawing::CmdListData data = std::make_pair(ptr, size);
    bool isCopy = false;
    std::shared_ptr<Drawing::DrawCmdList> drawCmdList = Drawing::DrawCmdList::CreateFromData(data, isCopy);
    drawable->ProcessCPURenderInBackgroundThread(drawCmdList, ctx, id);
    ASSERT_EQ(drawable->image_, nullptr);

    Drawing::Paint paint;
    auto drawOpItem = std::make_shared<Drawing::DrawWithPaintOpItem>(paint, 0);
    EXPECT_TRUE(drawCmdList->IsEmpty());

    drawable->ProcessCPURenderInBackgroundThread(drawCmdList, ctx, id);
    drawCmdList->AddDrawOp(std::move(drawOpItem));
    EXPECT_FALSE(drawCmdList->IsEmpty());

    drawable->surface_ = std::make_shared<Drawing::Surface>();
    drawable->ProcessCPURenderInBackgroundThread(drawCmdList, ctx, id);
    ASSERT_NE(drawable->surface_, nullptr);

    RSRenderNodeDrawableAdapter::WeakPtr wNode = drawable;
    RSRenderNodeDrawableAdapter::RenderNodeDrawableCache_.emplace(id, wNode);
    drawOpItem = std::make_shared<Drawing::DrawWithPaintOpItem>(paint, 0);
    drawCmdList->AddDrawOp(std::move(drawOpItem));
    drawable->ProcessCPURenderInBackgroundThread(drawCmdList, ctx, id);
    EXPECT_FALSE(drawCmdList->IsEmpty());
}

/**
 * @tc.name: OnCaptureTest
 * @tc.desc: Test If OnCapture Can Run
 * @tc.type: FUNC
 * @tc.require: issueICF7P6
 */
HWTEST_F(RSCanvasDrawingRenderNodeDrawableTest, OnCapture001, TestSize.Level1)
{
    NodeId nodeId = 1;
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSCanvasDrawingRenderNode>(nodeId, rsContext->weak_from_this());
    auto drawable = std::make_shared<RSCanvasDrawingRenderNodeDrawable>(std::move(node));
    int width = 1024;
    int height = 1920;
    Drawing::Canvas canvas(width, height);
    drawable->renderParams_ = nullptr;
    drawable->OnCapture(canvas);
    ASSERT_FALSE(drawable->ShouldPaint());
    drawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    drawable->OnCapture(canvas);

    CaptureParam params;
    drawable->renderParams_ = nullptr;
    params.isMirror_ = true;
    AutoSpecialLayerStateRecover whiteListRecover(INVALID_NODEID);
    std::unordered_set<NodeId> whiteList = {nodeId};
    RSUniRenderThread::Instance().SetWhiteList(whiteList);
    RSUniRenderThread::SetCaptureParam(params);
    drawable->OnCapture(canvas);
    ASSERT_FALSE(drawable->ShouldPaint());
}

/**
 * @tc.name: OnCaptureTest002
 * @tc.desc: Test If OnCapture Can Run
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasDrawingRenderNodeDrawableTest, OnCaptureTest002, TestSize.Level1)
{
    // set render thread param
    auto uniParams = std::make_unique<RSRenderThreadParams>();
    uniParams->SetSecurityDisplay(true);
    RSUniRenderThread::Instance().Sync(std::move(uniParams));

    NodeId nodeId = 0;
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSCanvasDrawingRenderNode>(nodeId, rsContext->weak_from_this());
    auto drawable = std::make_shared<RSCanvasDrawingRenderNodeDrawable>(std::move(node));
    ASSERT_NE(drawable, nullptr);
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    drawable->renderParams_ = nullptr;
    drawable->OnCapture(canvas);
    drawable->isDrawingCacheEnabled_ = false;
    drawable->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    ASSERT_TRUE(drawable->GetRenderParams());
    drawable->renderParams_->shouldPaint_ = true;
    drawable->renderParams_->contentEmpty_ = false;
    ASSERT_FALSE(drawable->isDrawingCacheEnabled_);
    ASSERT_TRUE(drawable->GetRenderParams());
    drawable->OnCapture(canvas);
    ASSERT_TRUE(drawable->ShouldPaint());
    nodeId = TEST_ID;
    RSUniRenderThread::GetCaptureParam().endNodeId_ = TEST_ID;
    canvas.SetUICapture(true);
    drawable->OnCapture(canvas);
    drawable->OnDraw(canvas);
    ASSERT_TRUE(drawable->ShouldPaint());
    RSUniRenderThread::GetCaptureParam().endNodeId_ = INVALID_NODEID;
    RSUniRenderThread::GetCaptureParam().captureFinished_ = true;
    drawable->OnCapture(canvas);
    drawable->OnDraw(canvas);
    ASSERT_TRUE(drawable->ShouldPaint());
}

/**
 * @tc.name: ResetSurface
 * @tc.desc: Test If ResetSurface Can Run
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSCanvasDrawingRenderNodeDrawableTest, ResetSurfaceTest001, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSCanvasDrawingRenderNode>(0, rsContext->weak_from_this());
    auto drawable = std::make_shared<RSCanvasDrawingRenderNodeDrawable>(std::move(node));
    drawable->ResetSurface();
    EXPECT_EQ(drawable->surface_, nullptr);

    drawable->surface_ = std::make_shared<Drawing::Surface>();
    drawable->ResetSurface();
    EXPECT_EQ(drawable->recordingCanvas_, nullptr);

#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    drawable->preThreadInfo_.second = [](std::shared_ptr<Drawing::Surface> surface) {};
#endif
    drawable->surface_ = std::make_shared<Drawing::Surface>();
    drawable->ResetSurface();
    EXPECT_EQ(drawable->recordingCanvas_, nullptr);

    drawable->surface_ = nullptr;
    drawable->ResetSurface();
    EXPECT_EQ(drawable->recordingCanvas_, nullptr);
}

/**
 * @tc.name: GetBitmap
 * @tc.desc: Test If GetBitmap Can Run
 * @tc.type: FUNC
 * @tc.require: issueIAFX7M
 */
HWTEST_F(RSCanvasDrawingRenderNodeDrawableTest, GetBitmapTest, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSCanvasDrawingRenderNode>(0, rsContext->weak_from_this());
    auto node2 = node;
    auto drawable = std::make_shared<RSCanvasDrawingRenderNodeDrawable>(std::move(node));
    Drawing::GPUContext* grContext = nullptr;
    drawable->GetBitmap(grContext);
    EXPECT_EQ(drawable->image_, nullptr);

    drawable->image_ = std::make_shared<Drawing::Image>();
    drawable->GetBitmap(grContext);
    EXPECT_NE(drawable->image_, nullptr);

    auto gpuContext = std::make_shared<Drawing::GPUContext>();
    drawable->GetBitmap(gpuContext.get());
    EXPECT_NE(gpuContext, nullptr);

    EXPECT_TRUE(node2 != nullptr);
    node2->cachedOpCount_ = 1;
    drawable->GetBitmap(gpuContext.get());
    EXPECT_NE(gpuContext, nullptr);
}

/**
 * @tc.name: GetPixelmap
 * @tc.desc: Test If GetPixelmap Can Run
 * @tc.type: FUNC
 * @tc.require: issueIAFX7M
 */
HWTEST_F(RSCanvasDrawingRenderNodeDrawableTest, GetPixelmapTest, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSCanvasDrawingRenderNode>(0, rsContext->weak_from_this());
    auto node2 = node;
    auto drawable = std::make_shared<RSCanvasDrawingRenderNodeDrawable>(std::move(node));
    std::shared_ptr<Media::PixelMap> pixelmap = nullptr;
    Drawing::Rect* rect = nullptr;
    uint64_t tid = INVALID_NODEID;
    std::shared_ptr<Drawing::DrawCmdList> drawCmdList = nullptr;
    bool res = drawable->GetPixelmap(pixelmap, rect, tid, drawCmdList);
    ASSERT_FALSE(res);
    pixelmap = std::make_shared<Media::PixelMap>();
    res = drawable->GetPixelmap(pixelmap, rect, tid, drawCmdList);
    ASSERT_FALSE(res);

    auto rrect = std::make_shared<Drawing::Rect>(0.f, 0.f, 1.f, 1.f);
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    tid = drawable->preThreadInfo_.first;
#else
    tid = UNI_RENDER_THREAD_INDEX;
#endif
    auto canvas = std::make_shared<Drawing::Canvas>();
    drawable->canvas_ = std::make_shared<RSPaintFilterCanvas>(canvas.get());
    res = drawable->GetPixelmap(pixelmap, rrect.get(), tid, drawCmdList);
    drawable->image_ = std::make_shared<Drawing::Image>();
    res = drawable->GetPixelmap(pixelmap, rrect.get(), tid, drawCmdList);
    ASSERT_FALSE(res);

    drawable->surface_ = std::make_shared<Drawing::Surface>();
    res = drawable->GetPixelmap(pixelmap, rrect.get(), tid, drawCmdList);
    ASSERT_FALSE(res);

    drawCmdList = std::make_shared<Drawing::DrawCmdList>();
#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    res = drawable->GetPixelmap(pixelmap, rrect.get(), tid, drawCmdList);
    ASSERT_FALSE(res);
    drawable->canvas_->gpuContext_ = std::make_shared<Drawing::GPUContext>();
#endif
    res = drawable->GetPixelmap(pixelmap, rrect.get(), tid, drawCmdList);
    ASSERT_FALSE(res);
    ASSERT_TRUE(node2 != nullptr);
    node2->cachedOpCount_ = 1;
    res = drawable->GetPixelmap(pixelmap, rrect.get(), tid, drawCmdList);
    ASSERT_FALSE(res);
}

/**
 * @tc.name: DrawCaptureImage
 * @tc.desc: Test If DrawCaptureImage Can Run
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSCanvasDrawingRenderNodeDrawableTest, DrawCaptureImageTest, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSCanvasDrawingRenderNode>(0, rsContext->weak_from_this());
    auto drawable = std::make_shared<RSCanvasDrawingRenderNodeDrawable>(std::move(node));
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    drawable->DrawCaptureImage(canvas);
    EXPECT_EQ(drawable->image_, nullptr);

    drawable->image_ = std::make_shared<Drawing::Image>();
    drawable->DrawCaptureImage(canvas);
    EXPECT_NE(drawable->image_, nullptr);

    drawable->backendTexture_.isValid_ = true;
    drawable->DrawCaptureImage(canvas);
    EXPECT_TRUE(drawable->backendTexture_.IsValid());
    EXPECT_EQ(canvas.GetGPUContext(), nullptr);
}

/**
 * @tc.name: ResetSurface
 * @tc.desc: Test If ResetSurface Can Run
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSCanvasDrawingRenderNodeDrawableTest, ResetSurfaceTest002, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSCanvasDrawingRenderNode>(0, rsContext->weak_from_this());
    auto drawable = std::make_shared<RSCanvasDrawingRenderNodeDrawable>(std::move(node));
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    int width = 1;
    int height = 1;
    canvas.recordingState_ = true;
    auto result = drawable->ResetSurfaceForGL(width, height, canvas);
    EXPECT_EQ(result, true);
    auto resultVK = drawable->ResetSurfaceForVK(width, height, canvas);
    EXPECT_EQ(resultVK, true);

    canvas.recordingState_ = false;
    drawable->image_ = std::make_shared<Drawing::Image>();
    result = drawable->ResetSurfaceForGL(width, height, canvas);
    EXPECT_EQ(result, true);
    resultVK = drawable->ResetSurfaceForVK(width, height, canvas);
    EXPECT_EQ(resultVK, true);
}

/**
 * @tc.name: ResetSurface
 * @tc.desc: Test If ResetSurface Can Run
 * @tc.type: FUNC
 * @tc.require: #ICDBD1
 */
HWTEST_F(RSCanvasDrawingRenderNodeDrawableTest, ResetSurfaceTest003, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSCanvasDrawingRenderNode>(0, rsContext->weak_from_this());
    auto drawable = std::make_shared<RSCanvasDrawingRenderNodeDrawable>(std::move(node));
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    int width = 1;
    int height = 1;
    canvas.recordingState_ = true;
    auto result = drawable->ResetSurfaceForGL(width, height, canvas);
    EXPECT_EQ(result, true);

    drawable->renderParams_ = std::make_unique<RSRenderParams>(0);
    drawable->renderParams_->surfaceParams_.colorSpace = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3;
    ASSERT_TRUE(drawable->GetRenderParams());
    auto resultVK = drawable->ResetSurfaceForVK(width, height, canvas);
    EXPECT_EQ(resultVK, true);
}

/**
 * @tc.name: ResetSurfaceForGL
 * @tc.desc: Test If ResetSurfaceForGL Can Run
 * @tc.type: FUNC
 * @tc.require: issueIAFX7M
 */
HWTEST_F(RSCanvasDrawingRenderNodeDrawableTest, ResetSurfaceForGLTest, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSCanvasDrawingRenderNode>(0, rsContext->weak_from_this());
    auto drawable = std::make_shared<RSCanvasDrawingRenderNodeDrawable>(std::move(node));
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    int width = 1;
    int height = 1;
    canvas.recordingState_ = true;
    bool result = drawable->ResetSurfaceForGL(width, height, canvas);
    ASSERT_TRUE(result);
#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    canvas.recordingState_ = false;
    auto drawCanvas = std::make_shared<Drawing::Canvas>();
    canvas.canvas_ = drawCanvas.get();
    result = drawable->ResetSurfaceForGL(width, height, canvas);
    ASSERT_TRUE(result);
    canvas.canvas_->gpuContext_ = std::make_shared<Drawing::GPUContext>();
    ASSERT_TRUE(canvas.GetGPUContext());
    drawable->backendTexture_.isValid_ = true;
    result = drawable->ResetSurfaceForGL(width, height, canvas);
    ASSERT_TRUE(result);
#endif
}

#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
/**
 * @tc.name: ReuseBackendTexture
 * @tc.desc: Test If ReuseBackendTexture Can Run
 * @tc.type: FUNC
 * @tc.require: issueIAFX7M
 */
HWTEST_F(RSCanvasDrawingRenderNodeDrawableTest, ReuseBackendTextureTest, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSCanvasDrawingRenderNode>(0, rsContext->weak_from_this());
    auto drawable = std::make_shared<RSCanvasDrawingRenderNodeDrawable>(std::move(node));
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    int width = 1;
    int height = 1;
    auto drawCanvas = std::make_shared<Drawing::Canvas>();
    drawable->canvas_ = std::make_shared<RSPaintFilterCanvas>(drawCanvas.get());
    drawable->surface_ = std::make_shared<Drawing::Surface>();
    bool res = drawable->ReuseBackendTexture(width, height, canvas);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: GetCurrentContextAndImage
 * @tc.desc: Test If GetCurrentContextAndImage Can Run
 * @tc.type: FUNC
 * @tc.require: issueIAFX7M
 */
HWTEST_F(RSCanvasDrawingRenderNodeDrawableTest, GetCurrentContextAndImageTest, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSCanvasDrawingRenderNode>(0, rsContext->weak_from_this());
    auto drawable = std::make_shared<RSCanvasDrawingRenderNodeDrawable>(std::move(node));
    std::shared_ptr<Drawing::GPUContext> grContext;
    std::shared_ptr<Drawing::Image> image;
    uint64_t tid = INVALID_NODEID;
    auto drawCanvas = std::make_shared<Drawing::Canvas>();
    drawable->canvas_ = std::make_shared<RSPaintFilterCanvas>(drawCanvas.get());
    drawable->surface_ = std::make_shared<Drawing::Surface>();
    drawable->image_ = std::make_shared<Drawing::Image>();
    bool res = drawable->GetCurrentContextAndImage(grContext, image, tid);
    ASSERT_FALSE(res);

    tid = drawable->preThreadInfo_.first;
    res = drawable->GetCurrentContextAndImage(grContext, image, tid);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: ResetSurfaceWithTexture
 * @tc.desc: Test If ResetSurfaceWithTexture Can Run
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSCanvasDrawingRenderNodeDrawableTest, ResetSurfaceWithTextureTest, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSCanvasDrawingRenderNode>(0, rsContext->weak_from_this());
    auto drawable = std::make_shared<RSCanvasDrawingRenderNodeDrawable>(std::move(node));
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    int width = 1;
    int height = 1;
    auto drawCanvas = std::make_shared<Drawing::Canvas>();
    drawable->canvas_ = std::make_shared<RSPaintFilterCanvas>(drawCanvas.get());
    drawable->surface_ = std::make_shared<Drawing::Surface>();
    drawable->image_ = std::make_shared<Drawing::Image>();
    auto result = drawable->ResetSurfaceWithTexture(width, height, canvas);
    ASSERT_EQ(result, false);
}

/**
 * @tc.name: OnDraw001
 * @tc.desc: Test OnDraw while isn't security display
 * @tc.type: FUNC
 * @tc.require: issue20602
 */
HWTEST_F(RSCanvasDrawingRenderNodeDrawableTest, OnDraw001, TestSize.Level2)
{
    auto drawable = RSCanvasDrawingRenderNodeDrawableTest::CreateDrawable();
    drawable->renderParams_ = std::make_unique<RSRenderParams>(0);
    drawable->renderParams_->shouldPaint_ = true;

    auto params = std::make_unique<RSRenderThreadParams>();
    params->SetSecurityDisplay(false);
    RSUniRenderThread::Instance().Sync(std::move(params));
    RSUniRenderThread::Instance().SetWhiteList({});
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    drawable->OnDraw(canvas);
    ASSERT_FALSE(drawable->SkipDrawByWhiteList(canvas));

    // restore
    RSUniRenderThread::Instance().Sync(std::make_unique<RSRenderThreadParams>());
}

/**
 * @tc.name: OnDraw002
 * @tc.desc: Test OnDraw while skip draw by white list
 * @tc.type: FUNC
 * @tc.require: issue20602
 */
HWTEST_F(RSCanvasDrawingRenderNodeDrawableTest, OnDraw002, TestSize.Level2)
{
    auto drawable = RSCanvasDrawingRenderNodeDrawableTest::CreateDrawable();
    drawable->renderParams_ = std::make_unique<RSRenderParams>(0);
    drawable->renderParams_->shouldPaint_ = true;

    auto params = std::make_unique<RSRenderThreadParams>();
    params->SetSecurityDisplay(true);
    RSUniRenderThread::Instance().Sync(std::move(params));
    std::unordered_set<NodeId> whiteList = {drawable->nodeId_};
    RSUniRenderThread::Instance().SetWhiteList(whiteList);

    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    AutoSpecialLayerStateRecover whiteListRecover(INVALID_NODEID);
    ASSERT_TRUE(drawable->SkipDrawByWhiteList(canvas));
    drawable->OnDraw(canvas);
    // restore
    RSUniRenderThread::Instance().Sync(std::make_unique<RSRenderThreadParams>());
}

/**
 * @tc.name: OnDraw003
 * @tc.desc: Test OnDraw while list is empty
 * @tc.type: FUNC
 * @tc.require: issue20602
 */
HWTEST_F(RSCanvasDrawingRenderNodeDrawableTest, OnDraw003, TestSize.Level2)
{
    auto drawable = RSCanvasDrawingRenderNodeDrawableTest::CreateDrawable();
    drawable->renderParams_ = std::make_unique<RSRenderParams>(0);
    drawable->renderParams_->shouldPaint_ = true;

    auto params = std::make_unique<RSRenderThreadParams>();
    params->SetSecurityDisplay(true);
    RSUniRenderThread::Instance().Sync(std::move(params));
    RSUniRenderThread::Instance().SetWhiteList({});

    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    ASSERT_FALSE(drawable->SkipDrawByWhiteList(canvas));
    drawable->OnDraw(canvas);

    // restore
    RSUniRenderThread::Instance().Sync(std::make_unique<RSRenderThreadParams>());
}

/**
 * @tc.name: OnDraw004
 * @tc.desc: Test OnDraw while renderThreadParams_ is nullptr
 * @tc.type: FUNC
 * @tc.require: issue20602
 */
HWTEST_F(RSCanvasDrawingRenderNodeDrawableTest, OnDraw004, TestSize.Level2)
{
    auto drawable = RSCanvasDrawingRenderNodeDrawableTest::CreateDrawable();
    drawable->renderParams_ = std::make_unique<RSRenderParams>(0);
    drawable->renderParams_->shouldPaint_ = true;

    RSUniRenderThread::Instance().Sync(nullptr);
    RSUniRenderThread::Instance().SetWhiteList({});

    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    ASSERT_FALSE(drawable->SkipDrawByWhiteList(canvas));
    drawable->OnDraw(canvas);

    // restore
    RSUniRenderThread::Instance().Sync(std::make_unique<RSRenderThreadParams>());
}

/**
 * @tc.name: OnDraw005
 * @tc.desc: Test OnDraw while opincBlockNodeSkip is false or true
 * @tc.type: FUNC
 * @tc.require: issue20602
 */
HWTEST_F(RSCanvasDrawingRenderNodeDrawableTest, OnDraw005, TestSize.Level2)
{
    auto drawable = RSCanvasDrawingRenderNodeDrawableTest::CreateDrawable();
    drawable->renderParams_ = std::make_unique<RSRenderParams>(0);
    drawable->renderParams_->shouldPaint_ = true;
    OHOS::Rosen::RectT<float> localDrawRect(0.0f, 0.0f, 100.0f, 100.0f);
    drawable->renderParams_->localDrawRect_ = localDrawRect;

    RSUniRenderThread::Instance().Sync(nullptr);
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    Drawing::Region region;
    canvas.PushDirtyRegion(region);
    RSOpincDrawCache::SetOpincBlockNodeSkip(false);
    drawable->OnDraw(canvas);
    EXPECT_NE(drawable->GetDrawSkipType(), DrawSkipType::OCCLUSION_SKIP);

    RSOpincDrawCache::SetOpincBlockNodeSkip(true);
    drawable->OnDraw(canvas);

    // restore
    RSUniRenderThread::Instance().Sync(std::make_unique<RSRenderThreadParams>());
}
#endif

#if defined(RS_ENABLE_GPU) && defined(RS_ENABLE_PARALLEL_RENDER)
/**
 * @tc.name: CheckAndSetThreadIdx
 * @tc.desc: Test If CheckAndSetThreadIdx Can Run
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasDrawingRenderNodeDrawableTest, CheckAndSetThreadIdxTest, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSCanvasDrawingRenderNode>(0, rsContext->weak_from_this());
    auto drawable = std::make_shared<RSCanvasDrawingRenderNodeDrawable>(std::move(node));
    ASSERT_EQ(drawable->CheckAndSetThreadIdx(), UNI_MAIN_THREAD_INDEX);
    RSUniRenderThread::Instance().tid_ = gettid();
    ASSERT_EQ(drawable->CheckAndSetThreadIdx(), UNI_RENDER_THREAD_INDEX);
}
#endif

/**
 * @tc.name: ResetSurfaceforPlayback
 * @tc.desc: Test If ResetSurfaceforPlayback Can Run
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasDrawingRenderNodeDrawableTest, ResetSurfaceforPlaybackTest, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSCanvasDrawingRenderNode>(0, rsContext->weak_from_this());
    auto drawable = std::make_shared<RSCanvasDrawingRenderNodeDrawable>(std::move(node));
    RSUniRenderThread& uniRenderThread = RSUniRenderThread::Instance();
    uniRenderThread.uniRenderEngine_ = std::make_shared<RSRenderEngine>();
    uniRenderThread.uniRenderEngine_->renderContext_ = RenderContext::Create();
    drawable->ResetSurfaceforPlayback(10, 10);
    ASSERT_EQ(drawable->canvas_, nullptr);
    uniRenderThread.uniRenderEngine_->renderContext_->drGPUContext_ = std::make_shared<Drawing::GPUContext>();
    drawable->renderParams_ = std::make_unique<RSCanvasDrawingRenderParams>(1);
    drawable->ResetSurfaceforPlayback(10, 10);
    ASSERT_NE(drawable->canvas_, nullptr);
    auto canvas = std::make_shared<Drawing::Canvas>();
    drawable->canvas_ = std::make_shared<RSPaintFilterCanvas>(canvas.get());
    drawable->ResetSurfaceforPlayback(10, 10);
    ASSERT_NE(drawable->surface_, nullptr);

    auto rsContext2 = std::make_shared<RSContext>();
    auto node2 = std::make_shared<RSCanvasDrawingRenderNode>(1, rsContext2->weak_from_this());
    auto drawable2 = std::make_shared<RSCanvasDrawingRenderNodeDrawable>(std::move(node2));
    drawable2->ResetSurfaceforPlayback(10, 10);
    ASSERT_NE(drawable2->canvas_, nullptr);
}

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
/**
 * @tc.name: CreateDmaBackendTextureTest001
 * @tc.desc: Test If CreateDmaBackendTexture Can Run
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasDrawingRenderNodeDrawableTest, CreateDmaBackendTextureTest001, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSCanvasDrawingRenderNode>(1, rsContext->weak_from_this());
    auto drawable = std::make_shared<RSCanvasDrawingRenderNodeDrawable>(std::move(node));
    auto ret = drawable->CreateDmaBackendTexture(1, 100, 100);
    ASSERT_EQ(ret, false);
    drawable->renderParams_ = std::make_unique<RSCanvasDrawingRenderParams>(1);
    drawable->renderParams_->SetCanvasDrawingResetSurfaceIndex(1);
    ret = drawable->CreateDmaBackendTexture(1, 100, 100);
    ASSERT_EQ(ret, false);
    ret = drawable->CreateDmaBackendTexture(1, 100, 100);
    ASSERT_EQ(ret, false);
    auto rsContext1 = std::make_shared<RSContext>();
    auto node1 = std::make_shared<RSCanvasDrawingRenderNode>(1, rsContext1->weak_from_this());
    RSCanvasDmaBufferCache::GetInstance().pendingBufferMap_.clear();
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().RegisterRenderNode(node1);
    node1->stagingRenderParams_->canvasDrawingResetSurfaceIndex_ = 1;
    sptr<SurfaceBuffer> buffer = SurfaceBuffer::Create();
    RSCanvasDmaBufferCache::GetInstance().AddPendingBuffer(1, buffer, 1);
    ret = drawable->CreateDmaBackendTexture(1, 100, 100);
    ASSERT_EQ(ret, false);
    RSCanvasDmaBufferCache::GetInstance().pendingBufferMap_.clear();
    buffer = SurfaceBufferUtils::CreateCanvasSurfaceBuffer(1, 100, 100);
    ASSERT_NE(buffer, nullptr);
    RSCanvasDmaBufferCache::GetInstance().AddPendingBuffer(1, buffer, 1);
    ret = drawable->CreateDmaBackendTexture(1, 100, 100);
    ASSERT_EQ(ret, RSSystemProperties::GetCanvasDrawingNodePreAllocateDmaEnabled() &&
        NodeMemReleaseParam::IsCanvasDrawingNodeDMAMemEnabled());
    drawable->backendTexture_ = {};
    bool isDmaBackendTexture = false;
    ret = drawable->ReleaseSurfaceVK(100, 100, isDmaBackendTexture);
    ASSERT_EQ(ret, true);
    Drawing::Canvas drawingCanvas;
    drawingCanvas.gpuContext_ = std::make_shared<Drawing::GPUContext>();
    RSPaintFilterCanvas canvas(&drawingCanvas);
    canvas.SetRecordingState(false);
    ret = drawable->ResetSurfaceForVK(10000, 10000, canvas);
    ASSERT_EQ(ret, true);
    drawable->ResetSurface();
    ASSERT_EQ(drawable->surface_, nullptr);
}

/**
 * @tc.name: CreateDmaBackendTextureTest002
 * @tc.desc: Test If CreateDmaBackendTexture Can Run
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasDrawingRenderNodeDrawableTest, CreateDmaBackendTextureTest002, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSCanvasDrawingRenderNode>(1, rsContext->weak_from_this());
    auto drawable = std::make_shared<RSCanvasDrawingRenderNodeDrawable>(std::move(node));
    drawable->renderParams_ = nullptr;
    Drawing::Canvas drawingCanvas;
    drawingCanvas.gpuContext_ = std::make_shared<Drawing::GPUContext>();
    RSPaintFilterCanvas canvas(&drawingCanvas);
    auto ret = drawable->ResetSurfaceForVK(10000, 10000, canvas);
    ASSERT_EQ(ret, true);
    auto buffer = SurfaceBufferUtils::CreateCanvasSurfaceBuffer(1, 100, 100);
    RSCanvasDmaBufferCache::GetInstance().AddPendingBuffer(2, buffer, 2);
    drawable->renderParams_ = std::make_unique<RSCanvasDrawingRenderParams>(2);
    drawable->renderParams_->SetCanvasDrawingResetSurfaceIndex(2);
    drawable->backendTexture_ = {};
    bool isDmaBackendTexture = false;
    ret = drawable->ReleaseSurfaceVK(100, 100, isDmaBackendTexture);
    ASSERT_EQ(ret, true);
    drawable->ResetSurface();
    ASSERT_EQ(drawable->surface_, nullptr);
    drawable->backendTexture_ = {};
    ret = drawable->ReleaseSurfaceVK(10000, 10001, isDmaBackendTexture);
    ASSERT_EQ(ret, false);
    ASSERT_EQ(isDmaBackendTexture, false);
}

/**
 * @tc.name: ReleaseDmaSurfaceBufferTest
 * @tc.desc: Test If ReleaseDmaSurfaceBuffer Can Run
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasDrawingRenderNodeDrawableTest, ReleaseDmaSurfaceBufferTest, TestSize.Level1)
{
    auto& bufferCache = RSCanvasDmaBufferCache::GetInstance();
    sptr<SurfaceBuffer> buffer = SurfaceBuffer::Create();
    bufferCache.pendingBufferMap_.clear();
    bufferCache.AddPendingBuffer(1, buffer, 1);
    auto& nodeBufferMap = bufferCache.pendingBufferMap_[1].second;
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSCanvasDrawingRenderNode>(1, rsContext->weak_from_this());
    auto drawable = std::make_shared<RSCanvasDrawingRenderNodeDrawable>(std::move(node));
    drawable->ReleaseDmaSurfaceBuffer(true);
    ASSERT_EQ(drawable->renderParams_, nullptr);
    ASSERT_EQ(nodeBufferMap.empty(), false);
    drawable->renderParams_ = std::make_unique<RSCanvasDrawingRenderParams>(1);
    drawable->renderParams_->SetCanvasDrawingResetSurfaceIndex(1);
    drawable->ReleaseDmaSurfaceBuffer(true);
    ASSERT_NE(drawable->renderParams_, nullptr);
    ASSERT_EQ(nodeBufferMap.empty(), false);
    drawable->ReleaseDmaSurfaceBuffer(false);
    ASSERT_EQ(nodeBufferMap.empty(), RSSystemProperties::GetCanvasDrawingNodePreAllocateDmaEnabled() &&
        NodeMemReleaseParam::IsCanvasDrawingNodeDMAMemEnabled());
}

/**
 * @tc.name: ClearCustomResourceWithDmaBufferTest001
 * @tc.desc: Test ClearCustomResource with pending buffers
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasDrawingRenderNodeDrawableTest, ClearCustomResourceWithDmaBufferTest001, TestSize.Level1)
{
    auto& bufferCache = RSCanvasDmaBufferCache::GetInstance();
    NodeId nodeId = 1;
    // Test Case 1: Clear custom resource with pending buffers in cache
    {
        bufferCache.pendingBufferMap_.clear();
        sptr<SurfaceBuffer> buffer = SurfaceBuffer::Create();
        bufferCache.AddPendingBuffer(nodeId, buffer, 1);
        ASSERT_GT(bufferCache.pendingBufferMap_[nodeId].second.size(), 0);
        auto rsContext = std::make_shared<RSContext>();
        auto node = std::make_shared<RSCanvasDrawingRenderNode>(nodeId, rsContext->weak_from_this());
        auto drawable = std::make_shared<RSCanvasDrawingRenderNodeDrawable>(std::move(node));
        drawable->preAllocateDmaEnabled_ = true;
        drawable->ClearCustomResource();
        ASSERT_EQ(bufferCache.pendingBufferMap_[nodeId].second.size(), 0);
    }
    // Test Case 2: Clear custom resource without pending buffers
    {
        bufferCache.pendingBufferMap_.clear();
        ASSERT_EQ(bufferCache.pendingBufferMap_.size(), 0);
        auto rsContext = std::make_shared<RSContext>();
        auto node = std::make_shared<RSCanvasDrawingRenderNode>(nodeId, rsContext->weak_from_this());
        auto drawable = std::make_shared<RSCanvasDrawingRenderNodeDrawable>(std::move(node));
        drawable->preAllocateDmaEnabled_ = true;
        drawable->ClearCustomResource();
        ASSERT_EQ(bufferCache.pendingBufferMap_.size(), 0);
    }
}

/**
 * @tc.name: ClearCustomResourceWithDmaBufferTest002
 * @tc.desc: Test ClearCustomResource with multiple buffers and disabled flag
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasDrawingRenderNodeDrawableTest, ClearCustomResourceWithDmaBufferTest002, TestSize.Level1)
{
    auto& bufferCache = RSCanvasDmaBufferCache::GetInstance();
    NodeId nodeId = 1;
    // Test Case 3: Clear custom resource with multiple pending buffers
    {
        bufferCache.pendingBufferMap_.clear();
        sptr<SurfaceBuffer> buffer1 = SurfaceBuffer::Create();
        sptr<SurfaceBuffer> buffer2 = SurfaceBuffer::Create();
        bufferCache.AddPendingBuffer(nodeId, buffer1, 1);
        bufferCache.AddPendingBuffer(nodeId, buffer2, 2);
        ASSERT_EQ(bufferCache.pendingBufferMap_[nodeId].second.size(), 2);
        auto rsContext = std::make_shared<RSContext>();
        auto node = std::make_shared<RSCanvasDrawingRenderNode>(nodeId, rsContext->weak_from_this());
        auto drawable = std::make_shared<RSCanvasDrawingRenderNodeDrawable>(std::move(node));
        drawable->preAllocateDmaEnabled_ = true;
        drawable->ClearCustomResource();
        ASSERT_EQ(bufferCache.pendingBufferMap_[nodeId].second.size(), 0);
    }
    // Test Case 4: Clear custom resource when preAllocateDmaEnabled_ is false
    {
        bufferCache.pendingBufferMap_.clear();
        sptr<SurfaceBuffer> buffer = SurfaceBuffer::Create();
        bufferCache.AddPendingBuffer(nodeId, buffer, 1);
        ASSERT_GT(bufferCache.pendingBufferMap_[nodeId].second.size(), 0);
        auto rsContext = std::make_shared<RSContext>();
        auto node = std::make_shared<RSCanvasDrawingRenderNode>(nodeId, rsContext->weak_from_this());
        auto drawable = std::make_shared<RSCanvasDrawingRenderNodeDrawable>(std::move(node));
        drawable->preAllocateDmaEnabled_ = false;
        drawable->ClearCustomResource();
        ASSERT_EQ(bufferCache.pendingBufferMap_[nodeId].second.size(), 0);
    }
}
#endif

/**
 * @tc.name: GetGpuContextTest
 * @tc.desc: Test If GetGpuContext Can Run
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasDrawingRenderNodeDrawableTest, GetGpuContextTest, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSCanvasDrawingRenderNode>(1, rsContext->weak_from_this());
    auto drawable = std::make_shared<RSCanvasDrawingRenderNodeDrawable>(std::move(node));
    RSUniRenderThread& uniRenderThread = RSUniRenderThread::Instance();
    uniRenderThread.tid_ = gettid();
    uniRenderThread.uniRenderEngine_ = std::make_shared<RSRenderEngine>();
    uniRenderThread.uniRenderEngine_->renderContext_ = RenderContext::Create();
    uniRenderThread.uniRenderEngine_->renderContext_->drGPUContext_ = std::make_shared<Drawing::GPUContext>();
    auto context = drawable->GetGpuContext();
    ASSERT_NE(context, nullptr);

    // Initialize surface_ before calling Flush
    drawable->surface_ = std::make_shared<Drawing::Surface>();
    Drawing::Canvas canvas;
    RSPaintFilterCanvas rsCanvas(&canvas);
    const Drawing::Rect rect(1.0f, 1.0f, 1.0f, 1.0f);
    drawable->DrawRenderContent(canvas, rect);
    drawable->Flush(1, 1, rsContext, id, rsCanvas);
    drawable->renderParams_ = std::make_unique<RSRenderParams>(0);
    drawable->DrawRenderContent(canvas, rect);
    drawable->Flush(1, 1, rsContext, id, rsCanvas);
    context = drawable->GetGpuContext();
    ASSERT_NE(context, nullptr);
}

#ifdef RS_ENABLE_VK
/**
 * @tc.name: CheckBackendTextureTest
 * @tc.desc: Test If CheckBackendTexture Can Run
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasDrawingRenderNodeDrawableTest, CheckBackendTextureTest, TestSize.Level1)
{
    auto node = std::make_shared<RSCanvasDrawingRenderNode>(1);
    auto drawable = std::make_shared<RSCanvasDrawingRenderNodeDrawable>(std::move(node));
    bool ret = drawable->CheckBackendTexture(false, 100, 100, 1);
    ASSERT_FALSE(ret);
}

/**
 * @tc.name: CreateGpuSurfaceTest
 * @tc.desc: Test If CreateGpuSurface Can Run
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasDrawingRenderNodeDrawableTest, CreateGpuSurfaceTest, TestSize.Level1)
{
    auto node = std::make_shared<RSCanvasDrawingRenderNode>(1);
    auto drawable = std::make_shared<RSCanvasDrawingRenderNodeDrawable>(std::move(node));
    auto gpuContext = std::make_shared<Drawing::GPUContext>();
    Drawing::ImageInfo imageInfo =
        Drawing::ImageInfo { 100, 100, Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL, nullptr };
    bool newVulkanCleanupHelper = false;
    drawable->CreateGpuSurface(imageInfo, gpuContext, newVulkanCleanupHelper, false);
    ASSERT_EQ(drawable->surface_, nullptr);

    imageInfo.width_ = 20000;
    imageInfo.height_ = 20000;
    drawable->CreateGpuSurface(imageInfo, gpuContext, newVulkanCleanupHelper, true);
    ASSERT_EQ(drawable->surface_, nullptr);

    imageInfo.width_ = 100;
    imageInfo.height_ = 100;
    drawable->backendTexture_ = {};
    drawable->CreateGpuSurface(imageInfo, nullptr, newVulkanCleanupHelper, false);
    ASSERT_EQ(drawable->surface_, nullptr);

    drawable->backendTexture_ = NativeBufferUtils::MakeBackendTexture(imageInfo.width_, imageInfo.height_, getpid());
    drawable->CreateGpuSurface(imageInfo, nullptr, newVulkanCleanupHelper, false);
    ASSERT_EQ(drawable->surface_, nullptr);

    drawable->ResetResource();
    drawable->backendTexture_ = NativeBufferUtils::MakeBackendTexture(imageInfo.width_, imageInfo.height_, getpid());
    auto vkTextureInfo = drawable->backendTexture_.GetTextureInfo().GetVKTextureInfo();
    drawable->vulkanCleanupHelper_ = new NativeBufferUtils::VulkanCleanupHelper(RsVulkanContext::GetSingleton(),
        vkTextureInfo->vkImage, vkTextureInfo->vkAlloc.memory, vkTextureInfo->vkAlloc.statName);
    newVulkanCleanupHelper = false;
    drawable->CreateGpuSurface(imageInfo, gpuContext, newVulkanCleanupHelper, false);
    ASSERT_EQ(drawable->surface_, nullptr);
}
#endif
}
