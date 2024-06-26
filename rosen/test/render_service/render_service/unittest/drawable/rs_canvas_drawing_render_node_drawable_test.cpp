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
 * @tc.name: DrawRenderContent
 * @tc.desc: Test If DrawRenderContent Can Run
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSCanvasDrawingRenderNodeDrawableTest, DrawRenderContentTest, TestSize.Level1)
{
    auto drawable = RSCanvasDrawingRenderNodeDrawableTest::CreateDrawable();

    Drawing::Canvas canvas;
    const Drawing::Rect dst(1.0f, 1.0f, 1.0f, 1.0f);
    drawable->renderParams_->canvasDrawingNodeSurfaceChanged_ = true;
    drawable->DrawRenderContent(canvas, dst);

    drawable->image_ = std::make_shared<Drawing::Image>();
    drawable->DrawRenderContent(canvas, dst);
    ASSERT_NE(drawable->image_, nullptr);
}

/**
 * @tc.name: InitSurface
 * @tc.desc: Test If InitSurface Can Run
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSCanvasDrawingRenderNodeDrawableTest, InitSurfaceTest, TestSize.Level1)
{
    auto drawable = RSCanvasDrawingRenderNodeDrawableTest::CreateDrawable();

    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    int width = 1;
    int height = 1;
    drawable->InitSurface(width, height, canvas);

    drawable->surface_ = std::make_shared<Drawing::Surface>();
    auto result = drawable->InitSurface(width, height, canvas);
    ASSERT_EQ(result, true);
}

/**
 * @tc.name: Flush
 * @tc.desc: Test If Flush Can Run
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSCanvasDrawingRenderNodeDrawableTest, FlushTest, TestSize.Level1)
{
    auto drawable = RSCanvasDrawingRenderNodeDrawableTest::CreateDrawable();

    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    int width = 1;
    float height = 1.0;
    auto ctx = RSUniRenderThread::Instance().GetRSRenderThreadParams()->GetContext();
    if (ctx == nullptr) {
        return;
    }
    drawable->Flush(width, height, ctx, id, canvas);

    drawable->recordingCanvas_ = std::make_unique<ExtendRecordingCanvas>(0, 0);
    drawable->Flush(width, height, ctx, id, canvas);
    
    ASSERT_NE(drawable->canvas_, nullptr);
}

/**
 * @tc.name: ProcessCPURenderInBackgroundThread
 * @tc.desc: Test If ProcessCPURenderInBackgroundThread Can Run
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSCanvasDrawingRenderNodeDrawableTest, ProcessCPURenderInBackgroundThreadTest, TestSize.Level1)
{
    auto drawable = RSCanvasDrawingRenderNodeDrawableTest::CreateDrawable();

    auto ctx = RSUniRenderThread::Instance().GetRSRenderThreadParams()->GetContext();
    std::shared_ptr<Drawing::DrawCmdList> drawCmdList = std::make_shared<Drawing::DrawCmdList>();
    drawable->ProcessCPURenderInBackgroundThread(drawCmdList, ctx, id);
    ASSERT_EQ(drawable->image_, nullptr);

    drawable->surface_ = static_cast<DrawableV2::RSCanvasDrawingRenderNodeDrawable*>(
        RSRenderNodeDrawableAdapter::GetDrawableById(id).get())->surface_;
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
    auto drawable = RSCanvasDrawingRenderNodeDrawableTest::CreateDrawable();

    drawable->surface_ = std::make_shared<Drawing::Surface>();
    drawable->ResetSurface();
    ASSERT_EQ(drawable->image_, nullptr);
}

/**
 * @tc.name: DrawCaptureImage
 * @tc.desc: Test If DrawCaptureImage Can Run
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSCanvasDrawingRenderNodeDrawableTest, DrawCaptureImageTest, TestSize.Level1)
{
    auto drawable = RSCanvasDrawingRenderNodeDrawableTest::CreateDrawable();

    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    drawable->DrawCaptureImage(canvas);

    drawable->image_ = std::make_shared<Drawing::Image>();
    drawable->DrawCaptureImage(canvas);

    ASSERT_NE(drawable->image_, nullptr);
}

/**
 * @tc.name: ResetSurface
 * @tc.desc: Test If ResetSurface Can Run
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSCanvasDrawingRenderNodeDrawableTest, ResetSurfaceTest002, TestSize.Level1)
{
    auto drawable = RSCanvasDrawingRenderNodeDrawableTest::CreateDrawable();

    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    int width = 1;
    int height = 1;
    canvas.recordingState_ = true;
    auto result = drawable->ResetSurface(width, height, canvas);
    ASSERT_EQ(result, false);

    canvas.recordingState_ = false;
    drawable->image_ = std::make_shared<Drawing::Image>();
    result = drawable->ResetSurface(width, height, canvas);
    ASSERT_EQ(result, false);
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
    auto drawable = RSCanvasDrawingRenderNodeDrawableTest::CreateDrawable();

    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    int width = 1;
    int height = 1;

    auto result = drawable->ResetSurfaceWithTexture(width, height, canvas);
    ASSERT_EQ(result, false);
}
#endif
}
