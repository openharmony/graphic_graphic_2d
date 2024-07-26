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

/**
 * @tc.name: OnDraw
 * @tc.desc: Test If OnDraw Can Run
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSCanvasDrawingRenderNodeDrawableTest, OnDrawTest, TestSize.Level1)
{
    auto drawable = RSCanvasDrawingRenderNodeDrawableTest::CreateDrawable();

    Drawing::Canvas canvas;
    drawable->renderParams_->canvasDrawingNodeSurfaceChanged_ = true;
    drawable->OnDraw(canvas);
    drawable->PlaybackInCorrespondThread();
    ASSERT_NE(drawable->renderParams_, nullptr);
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
    EXPECT_EQ(drawable->canvas_, nullptr);
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

    canvas.recordingState_ = false;
    drawable->image_ = std::make_shared<Drawing::Image>();
    result = drawable->ResetSurfaceForGL(width, height, canvas);
    EXPECT_EQ(result, true);
}

#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
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
    drawable->canvas_ = std::make_shared<RSPaintFilterCanvas>(&drawingCanvas);
    drawable->surface_ = std::make_shared<Drawing::Surface>();
    drawable->image_ = std::make_shared<Drawing::Image>();
    auto result = drawable->ResetSurfaceWithTexture(width, height, canvas);
    ASSERT_EQ(result, false);
}
#endif
}
