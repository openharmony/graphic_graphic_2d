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
#include "drawable/rs_canvas_drawing_render_node_drawable.h"
#include "pipeline/rs_canvas_drawing_render_node.h"
#include "pipeline/rs_context.h"
#include "recording/draw_cmd_list.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::DrawableV2;

namespace OHOS::Rosen {
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

void RSCanvasDrawingRenderNodeDrawableTest::SetUpTestCase() {}
void RSCanvasDrawingRenderNodeDrawableTest::TearDownTestCase() {}
void RSCanvasDrawingRenderNodeDrawableTest::SetUp() {}
void RSCanvasDrawingRenderNodeDrawableTest::TearDown() {}

std::shared_ptr<RSCanvasDrawingRenderNodeDrawable> RSCanvasDrawingRenderNodeDrawableTest::CreateDrawable()
{
    auto rsContext = std::make_shared<RSContext>();
    auto renderNode = std::make_shared<RSRenderNode>(id, rsContext->weak_from_this());
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
}

#ifdef RS_ENABLE_VK
/**
 * @tc.name: ReleaseSurfaceVk
 * @tc.desc: Test If ReleaseSurfaceVk Can Run
 * @tc.type: FUNC
 * @tc.require: issueIB2B14
 */
HWTEST_F(RSCanvasDrawingRenderNodeDrawableTest, ReleaseSurfaceVkTest, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(0);
    auto drawable = std::make_shared<RSCanvasDrawingRenderNodeDrawable>(std::move(node));
    int width = 10;
    int height = 10;
    auto res = drawable->ReleaseSurfaceVk(width, height);
    EXPECT_TRUE(res);

    drawable->backendTexture_ = Drawing::BackendTexture(false);
    res = drawable->ReleaseSurfaceVk(width, height);
    EXPECT_TRUE(res);
}
#endif

/**
 * @tc.name: OnDrawWithoutChildren
 * @tc.desc: OnDrawWithoutChildren
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasDrawingRenderNodeDrawableTest, OnDrawWithoutChildren, TestSize.Level1)
{
    auto drawable = RSCanvasDrawingRenderNodeDrawableTest::CreateDrawable();

    Drawing::Canvas canvas;
    drawable->renderParams_->shouldPaint_ = true;
    drawable->renderParams_->contentEmpty_ = false;
    RSUniRenderThread::GetCaptureParam().endNodeId_ = drawable->renderParams_->GetId();
    canvas.SetUICapture(true);
    drawable->OnDraw(canvas);
    ASSERT_FALSE(drawable->renderParams_->GetCanvasDrawingSurfaceChanged());
    RSUniRenderThread::GetCaptureParam().endNodeId_ = INVALID_NODEID;
    drawable->renderParams_->canvasDrawingNodeSurfaceChanged_ = true;
    drawable->OnDraw(canvas);
    ASSERT_TRUE(drawable->renderParams_->GetCanvasDrawingSurfaceChanged());
}

/**
 * @tc.name: DrawRenderContent
 * @tc.desc: Test If DrawRenderContent Can Run
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSCanvasDrawingRenderNodeDrawableTest, DrawRenderContentTest, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(0);
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
    auto node = std::make_shared<RSRenderNode>(0);
    auto drawable = std::make_shared<RSCanvasDrawingRenderNodeDrawable>(std::move(node));
    drawable->PostPlaybackInCorrespondThread();
    ASSERT_FALSE(drawable->canvas_);

    drawable->renderParams_ = std::make_unique<RSRenderParams>(0);
    drawable->PostPlaybackInCorrespondThread();
    ASSERT_FALSE(drawable->canvas_);
}

/**
 * @tc.name: InitSurface
 * @tc.desc: Test If InitSurface Can Run
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSCanvasDrawingRenderNodeDrawableTest, InitSurfaceTest, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(0);
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
    auto node = std::make_shared<RSRenderNode>(0);
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
    auto node = std::make_shared<RSRenderNode>(0);
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
    auto node = std::make_shared<RSRenderNode>(0);
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

    drawable->curThreadInfo_.first = INVALID_NODEID;
    drawable->FlushForGL(width, height, context, nodeId, rscanvas);
    ASSERT_FALSE(drawable->backendTexture_.IsValid());

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
    auto node = std::make_shared<RSRenderNode>(0);
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
    auto node = std::make_shared<RSRenderNode>(0);
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

    drawable->curThreadInfo_.first = INVALID_NODEID;
    drawable->Flush(width, height, context, id, canvas);
    EXPECT_EQ(drawable->recordingCanvas_, nullptr);

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
    auto node = std::make_shared<RSRenderNode>(0);
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
}

/**
 * @tc.name: ResetSurface
 * @tc.desc: Test If ResetSurface Can Run
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSCanvasDrawingRenderNodeDrawableTest, ResetSurfaceTest001, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(0);
    auto drawable = std::make_shared<RSCanvasDrawingRenderNodeDrawable>(std::move(node));
    drawable->ResetSurface();
    EXPECT_EQ(drawable->surface_, nullptr);

    drawable->surface_ = std::make_shared<Drawing::Surface>();
    drawable->ResetSurface();
    EXPECT_EQ(drawable->recordingCanvas_, nullptr);

    drawable->preThreadInfo_.second = [](std::shared_ptr<Drawing::Surface> surface) {};
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
    auto node = std::make_shared<RSRenderNode>(0);
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
}

/**
 * @tc.name: GetPixelmap
 * @tc.desc: Test If GetPixelmap Can Run
 * @tc.type: FUNC
 * @tc.require: issueIAFX7M
 */
HWTEST_F(RSCanvasDrawingRenderNodeDrawableTest, GetPixelmapTest, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(0);
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
    tid = drawable->preThreadInfo_.first;
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
}

/**
 * @tc.name: DrawCaptureImage
 * @tc.desc: Test If DrawCaptureImage Can Run
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSCanvasDrawingRenderNodeDrawableTest, DrawCaptureImageTest, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(0);
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
    auto node = std::make_shared<RSRenderNode>(0);
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
 * @tc.name: ResetSurfaceForGL
 * @tc.desc: Test If ResetSurfaceForGL Can Run
 * @tc.type: FUNC
 * @tc.require: issueIAFX7M
 */
HWTEST_F(RSCanvasDrawingRenderNodeDrawableTest, ResetSurfaceForGLTest, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(0);
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
    auto node = std::make_shared<RSRenderNode>(0);
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
    auto node = std::make_shared<RSRenderNode>(0);
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
    auto node = std::make_shared<RSRenderNode>(0);
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
#endif
}
