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

#include "recording/recording_canvas.h"
#include "recording/recording_path.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class RecordingCanvasTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RecordingCanvasTest::SetUpTestCase() {}
void RecordingCanvasTest::TearDownTestCase() {}
void RecordingCanvasTest::SetUp() {}
void RecordingCanvasTest::TearDown() {}

/**
 * @tc.name: DrawPoint001
 * @tc.desc: Test the playback of the DrawPoint function.
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RecordingCanvasTest, DrawPoint001, TestSize.Level1)
{
    auto recordingCanvas = std::make_shared<RecordingCanvas>(10, 20);
    EXPECT_TRUE(recordingCanvas != nullptr);
    Point point(10.0f, 20.0f);
    recordingCanvas->DrawPoint(point);
    auto drawCmdList = recordingCanvas->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList != nullptr);
    Canvas canvas;
    drawCmdList->Playback(canvas);
}

/**
 * @tc.name: DrawLine001
 * @tc.desc: Test the playback of the DrawLine function.
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RecordingCanvasTest, DrawLine001, TestSize.Level1)
{
    auto recordingCanvas = std::make_shared<RecordingCanvas>(10, 20);
    EXPECT_TRUE(recordingCanvas != nullptr);
    Point startPoint(10.0f, 20.0f);
    Point endPoint(30.0f, 20.0f);
    recordingCanvas->DrawLine(startPoint, endPoint);
    auto drawCmdList = recordingCanvas->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList != nullptr);
    Canvas canvas;
    drawCmdList->Playback(canvas);
}

/**
 * @tc.name: DrawRect001
 * @tc.desc: Test the playback of the DrawRect function.
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RecordingCanvasTest, DrawRect001, TestSize.Level1)
{
    auto recordingCanvas = std::make_shared<RecordingCanvas>(10, 20);
    EXPECT_TRUE(recordingCanvas != nullptr);
    Rect rect(0.0f, 0.0f, 10.0f, 20.0f);
    recordingCanvas->DrawRect(rect);
    auto drawCmdList = recordingCanvas->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList != nullptr);
    Canvas canvas;
    drawCmdList->Playback(canvas);
}

/**
 * @tc.name: DrawRoundRect001
 * @tc.desc: Test the playback of the DrawRoundRect function.
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RecordingCanvasTest, DrawRoundRect001, TestSize.Level1)
{
    auto recordingCanvas = std::make_shared<RecordingCanvas>(10, 20);
    EXPECT_TRUE(recordingCanvas != nullptr);
    Rect rect(0.0f, 0.0f, 10.0f, 20.0f);
    RoundRect roundRect(rect, 1.0f, 1.0f);
    recordingCanvas->DrawRoundRect(roundRect);
    auto drawCmdList = recordingCanvas->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList != nullptr);
    Canvas canvas;
    drawCmdList->Playback(canvas);
}

/**
 * @tc.name: DrawNestedRoundRect001
 * @tc.desc: Test the playback of the DrawNestedRoundRect function.
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RecordingCanvasTest, DrawNestedRoundRect001, TestSize.Level1)
{
    auto recordingCanvas = std::make_shared<RecordingCanvas>(10, 20);
    EXPECT_TRUE(recordingCanvas != nullptr);
    Rect rect1(0.0f, 0.0f, 10.0f, 20.0f);
    RoundRect roundRect1(rect1, 1.0f, 1.0f);
    Rect rect2(0.0f, 0.0f, 5.0f, 10.0f);
    RoundRect roundRect2(rect2, 1.0f, 1.0f);
    recordingCanvas->DrawNestedRoundRect(roundRect1, roundRect2);
    auto drawCmdList = recordingCanvas->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList != nullptr);
    Canvas canvas;
    drawCmdList->Playback(canvas);
}

/**
 * @tc.name: DrawArc001
 * @tc.desc: Test the playback of the DrawArc function.
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RecordingCanvasTest, DrawArc001, TestSize.Level1)
{
    auto recordingCanvas = std::make_shared<RecordingCanvas>(10, 20);
    EXPECT_TRUE(recordingCanvas != nullptr);
    Rect rect(0.0f, 0.0f, 10.0f, 20.0f);
    recordingCanvas->DrawArc(rect, 0.0f, 90.0f);
    auto drawCmdList = recordingCanvas->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList != nullptr);
    Canvas canvas;
    drawCmdList->Playback(canvas);
}

/**
 * @tc.name: DrawPie001
 * @tc.desc: Test the playback of the DrawPie function.
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RecordingCanvasTest, DrawPie001, TestSize.Level1)
{
    auto recordingCanvas = std::make_shared<RecordingCanvas>(10, 20);
    EXPECT_TRUE(recordingCanvas != nullptr);
    Rect rect(0.0f, 0.0f, 10.0f, 20.0f);
    recordingCanvas->DrawPie(rect, 0.0f, 90.0f);
    auto drawCmdList = recordingCanvas->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList != nullptr);
    Canvas canvas;
    drawCmdList->Playback(canvas);
}

/**
 * @tc.name: DrawOval001
 * @tc.desc: Test the playback of the DrawOval function.
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RecordingCanvasTest, DrawOval001, TestSize.Level1)
{
    auto recordingCanvas = std::make_shared<RecordingCanvas>(10, 20);
    EXPECT_TRUE(recordingCanvas != nullptr);
    Rect rect(0.0f, 0.0f, 10.0f, 20.0f);
    recordingCanvas->DrawOval(rect);
    auto drawCmdList = recordingCanvas->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList != nullptr);
    Canvas canvas;
    drawCmdList->Playback(canvas);
}

/**
 * @tc.name: DrawCircle001
 * @tc.desc: Test the playback of the DrawCircle function.
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RecordingCanvasTest, DrawCircle001, TestSize.Level1)
{
    auto recordingCanvas = std::make_shared<RecordingCanvas>(10, 20);
    EXPECT_TRUE(recordingCanvas != nullptr);
    Point centerpoint(10.0f, 20.0f);
    recordingCanvas->DrawCircle(centerpoint, 10.0f);
    auto drawCmdList = recordingCanvas->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList != nullptr);
    Canvas canvas;
    drawCmdList->Playback(canvas);
}

/**
 * @tc.name: DrawPath001
 * @tc.desc: Test the playback of the DrawPath function.
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RecordingCanvasTest, DrawPath001, TestSize.Level1)
{
    auto recordingCanvas = std::make_shared<RecordingCanvas>(10, 20);
    EXPECT_TRUE(recordingCanvas != nullptr);
    RecordingPath path;
    recordingCanvas->DrawPath(path);
    auto drawCmdList = recordingCanvas->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList != nullptr);
    Canvas canvas;
    drawCmdList->Playback(canvas);
}

/**
 * @tc.name: DrawBackground001
 * @tc.desc: Test the playback of the DrawBackground function.
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RecordingCanvasTest, DrawBackground001, TestSize.Level1)
{
    auto recordingCanvas = std::make_shared<RecordingCanvas>(10, 20);
    EXPECT_TRUE(recordingCanvas != nullptr);
    Brush brush(Color::COLOR_RED);
    recordingCanvas->DrawBackground(brush);
    auto drawCmdList = recordingCanvas->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList != nullptr);
    Canvas canvas;
    drawCmdList->Playback(canvas);
}

/**
 * @tc.name: DrawShadow001
 * @tc.desc: Test the playback of the DrawShadow function.
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RecordingCanvasTest, DrawShadow001, TestSize.Level1)
{
    auto recordingCanvas = std::make_shared<RecordingCanvas>(10, 20);
    EXPECT_TRUE(recordingCanvas != nullptr);
    RecordingPath path;
    Point3 planeParams(1.0f, 0.0f, 0.0f);
    Point3 devLightPos(1.0f, 1.0f, 1.0f);
    recordingCanvas->DrawShadow(
        path, planeParams, devLightPos, 1.0f, Color::COLOR_BLACK, Color::COLOR_BLUE, ShadowFlags::NONE);
    auto drawCmdList = recordingCanvas->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList != nullptr);
    Canvas canvas;
    drawCmdList->Playback(canvas);
}

/**
 * @tc.name: DrawRegion001
 * @tc.desc: Test the playback of the DrawRegion function.
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RecordingCanvasTest, DrawRegion001, TestSize.Level1)
{
    auto recordingCanvas = std::make_shared<RecordingCanvas>(10, 20);
    EXPECT_TRUE(recordingCanvas != nullptr);
    Region region;
    recordingCanvas->DrawRegion(region);
    auto drawCmdList = recordingCanvas->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList != nullptr);
    Canvas canvas;
    drawCmdList->Playback(canvas);
}

/**
 * @tc.name: DrawBitmap001
 * @tc.desc: Test the playback of the DrawBitmap function.
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RecordingCanvasTest, DrawBitmap001, TestSize.Level1)
{
    auto recordingCanvas = std::make_shared<RecordingCanvas>(10, 20);
    EXPECT_TRUE(recordingCanvas != nullptr);
    Bitmap bitmap;
    BitmapFormat bitmapFormat { COLORTYPE_RGBA_8888, ALPHATYPE_OPAQUE };
    bitmap.Build(15, 15, bitmapFormat);
    recordingCanvas->DrawBitmap(bitmap, 10.0f, 10.0f);
    auto drawCmdList = recordingCanvas->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList != nullptr);
    Canvas canvas;
    drawCmdList->Playback(canvas);
}

/**
 * @tc.name: DrawImage001
 * @tc.desc: Test the playback of the DrawImage function.
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RecordingCanvasTest, DrawImage001, TestSize.Level1)
{
    auto recordingCanvas = std::make_shared<RecordingCanvas>(10, 20);
    EXPECT_TRUE(recordingCanvas != nullptr);
    Image image;
    SamplingOptions samplingOptions;
    recordingCanvas->DrawImage(image, 10.0f, 10.0f, samplingOptions);
    auto drawCmdList = recordingCanvas->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList != nullptr);
    Canvas canvas;
    drawCmdList->Playback(canvas);
}

/**
 * @tc.name: DrawImageRect001
 * @tc.desc: Test the playback of the DrawImageRect function.
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RecordingCanvasTest, DrawImageRect001, TestSize.Level1)
{
    auto recordingCanvas = std::make_shared<RecordingCanvas>(10, 20);
    EXPECT_TRUE(recordingCanvas != nullptr);
    Image image;
    Rect dstRect(0.0f, 0.0f, 10.0f, 20.0f);
    SamplingOptions samplingOptions;
    recordingCanvas->DrawImageRect(image, dstRect, samplingOptions);
    auto drawCmdList = recordingCanvas->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList != nullptr);
    Canvas canvas;
    drawCmdList->Playback(canvas);
}

/**
 * @tc.name: DrawPicture001
 * @tc.desc: Test the playback of the DrawPicture function.
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RecordingCanvasTest, DrawPicture001, TestSize.Level1)
{
    auto recordingCanvas = std::make_shared<RecordingCanvas>(10, 20);
    EXPECT_TRUE(recordingCanvas != nullptr);
    Picture pic;
    recordingCanvas->DrawPicture(pic);
    auto drawCmdList = recordingCanvas->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList != nullptr);
    Canvas canvas;
    drawCmdList->Playback(canvas);
}

/**
 * @tc.name: ClipRect001
 * @tc.desc: Test the playback of the ClipRect function.
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RecordingCanvasTest, ClipRect001, TestSize.Level1)
{
    auto recordingCanvas = std::make_shared<RecordingCanvas>(10, 20);
    EXPECT_TRUE(recordingCanvas != nullptr);
    Rect rect(0.0f, 0.0f, 10.0f, 20.0f);
    recordingCanvas->ClipRect(rect, ClipOp::DIFFERENCE, true);
    auto drawCmdList = recordingCanvas->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList != nullptr);
    Canvas canvas;
    drawCmdList->Playback(canvas);
}

/**
 * @tc.name: ClipRoundRect001
 * @tc.desc: Test the playback of the ClipRoundRect function.
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RecordingCanvasTest, ClipRoundRect001, TestSize.Level1)
{
    auto recordingCanvas = std::make_shared<RecordingCanvas>(10, 20);
    EXPECT_TRUE(recordingCanvas != nullptr);
    Rect rect(0.0f, 0.0f, 10.0f, 20.0f);
    RoundRect roundRect(rect, 1.0f, 1.0f);
    recordingCanvas->ClipRoundRect(roundRect, ClipOp::DIFFERENCE, true);
    auto drawCmdList = recordingCanvas->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList != nullptr);
    Canvas canvas;
    drawCmdList->Playback(canvas);
}

/**
 * @tc.name: ClipPath001
 * @tc.desc: Test the playback of the ClipPath function.
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RecordingCanvasTest, ClipPath001, TestSize.Level1)
{
    auto recordingCanvas = std::make_shared<RecordingCanvas>(10, 20);
    EXPECT_TRUE(recordingCanvas != nullptr);
    RecordingPath path;
    recordingCanvas->ClipPath(path, ClipOp::DIFFERENCE, true);
    auto drawCmdList = recordingCanvas->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList != nullptr);
    Canvas canvas;
    drawCmdList->Playback(canvas);
}

/**
 * @tc.name: SetMatrix001
 * @tc.desc: Test the playback of the SetMatrix function.
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RecordingCanvasTest, SetMatrix001, TestSize.Level1)
{
    auto recordingCanvas = std::make_shared<RecordingCanvas>(10, 20);
    EXPECT_TRUE(recordingCanvas != nullptr);
    Matrix matrix;
    recordingCanvas->SetMatrix(matrix);
    auto drawCmdList = recordingCanvas->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList != nullptr);
    Canvas canvas;
    drawCmdList->Playback(canvas);
}

/**
 * @tc.name: ResetMatrix001
 * @tc.desc: Test the playback of the ResetMatrix function.
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RecordingCanvasTest, ResetMatrix001, TestSize.Level1)
{
    auto recordingCanvas = std::make_shared<RecordingCanvas>(10, 20);
    EXPECT_TRUE(recordingCanvas != nullptr);
    Matrix matrix;
    recordingCanvas->SetMatrix(matrix);
    recordingCanvas->ResetMatrix();
    auto drawCmdList = recordingCanvas->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList != nullptr);
    Canvas canvas;
    drawCmdList->Playback(canvas);
}

/**
 * @tc.name: ConcatMatrix001
 * @tc.desc: Test the playback of the ConcatMatrix function.
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RecordingCanvasTest, ConcatMatrix001, TestSize.Level1)
{
    auto recordingCanvas = std::make_shared<RecordingCanvas>(10, 20);
    EXPECT_TRUE(recordingCanvas != nullptr);
    Matrix matrix;
    recordingCanvas->ConcatMatrix(matrix);
    auto drawCmdList = recordingCanvas->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList != nullptr);
    Canvas canvas;
    drawCmdList->Playback(canvas);
}

/**
 * @tc.name: Translate001
 * @tc.desc: Test the playback of the Translate function.
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RecordingCanvasTest, Translate001, TestSize.Level1)
{
    auto recordingCanvas = std::make_shared<RecordingCanvas>(10, 20);
    EXPECT_TRUE(recordingCanvas != nullptr);
    recordingCanvas->Translate(1.0f, 1.0f);
    auto drawCmdList = recordingCanvas->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList != nullptr);
    Canvas canvas;
    drawCmdList->Playback(canvas);
}

/**
 * @tc.name: Scale001
 * @tc.desc: Test the playback of the Scale function.
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RecordingCanvasTest, Scale001, TestSize.Level1)
{
    auto recordingCanvas = std::make_shared<RecordingCanvas>(10, 20);
    EXPECT_TRUE(recordingCanvas != nullptr);
    recordingCanvas->Scale(1.0f, 1.0f);
    auto drawCmdList = recordingCanvas->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList != nullptr);
    Canvas canvas;
    drawCmdList->Playback(canvas);
}

/**
 * @tc.name: Rotate002
 * @tc.desc: Test the playback of the Rotate function.
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RecordingCanvasTest, Rotate002, TestSize.Level1)
{
    auto recordingCanvas = std::make_shared<RecordingCanvas>(10, 20);
    EXPECT_TRUE(recordingCanvas != nullptr);
    recordingCanvas->Rotate(60.0f, 10.0f, 10.0f);
    auto drawCmdList = recordingCanvas->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList != nullptr);
    Canvas canvas;
    drawCmdList->Playback(canvas);
}

/**
 * @tc.name: Shear001
 * @tc.desc: Test the playback of the Shear function.
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RecordingCanvasTest, Shear001, TestSize.Level1)
{
    auto recordingCanvas = std::make_shared<RecordingCanvas>(10, 20);
    EXPECT_TRUE(recordingCanvas != nullptr);
    recordingCanvas->Shear(10.0f, 10.0f);
    auto drawCmdList = recordingCanvas->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList != nullptr);
    Canvas canvas;
    drawCmdList->Playback(canvas);
}

/**
 * @tc.name: Flush001
 * @tc.desc: Test the playback of the Flush function.
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RecordingCanvasTest, Flush001, TestSize.Level1)
{
    auto recordingCanvas = std::make_shared<RecordingCanvas>(10, 20);
    EXPECT_TRUE(recordingCanvas != nullptr);
    recordingCanvas->Flush();
    auto drawCmdList = recordingCanvas->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList != nullptr);
    Canvas canvas;
    drawCmdList->Playback(canvas);
}

/**
 * @tc.name: Clear001
 * @tc.desc: Test the playback of the Clear function.
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RecordingCanvasTest, Clear001, TestSize.Level1)
{
    auto recordingCanvas = std::make_shared<RecordingCanvas>(10, 20);
    EXPECT_TRUE(recordingCanvas != nullptr);
    recordingCanvas->Clear(Color::COLOR_BLUE);
    auto drawCmdList = recordingCanvas->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList != nullptr);
    Canvas canvas;
    drawCmdList->Playback(canvas);
}

/**
 * @tc.name: Save001
 * @tc.desc: Test the playback of the Save function.
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RecordingCanvasTest, Save001, TestSize.Level1)
{
    auto recordingCanvas = std::make_shared<RecordingCanvas>(10, 20);
    EXPECT_TRUE(recordingCanvas != nullptr);
    recordingCanvas->Save();
    auto drawCmdList = recordingCanvas->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList != nullptr);
    Canvas canvas;
    drawCmdList->Playback(canvas);
}

/**
 * @tc.name: SaveLayer001
 * @tc.desc: Test the playback of the SaveLayer function.
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RecordingCanvasTest, SaveLayer001, TestSize.Level1)
{
    auto recordingCanvas = std::make_shared<RecordingCanvas>(10, 20);
    EXPECT_TRUE(recordingCanvas != nullptr);
    Rect rect(0.0f, 0.0f, 10.0f, 20.0f);
    Brush brush;
    uint32_t saveLayerFlags = 0;
    SaveLayerOps saveLayerRec(&rect, &brush, saveLayerFlags);
    recordingCanvas->SaveLayer(saveLayerRec);
    auto drawCmdList = recordingCanvas->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList != nullptr);
    Canvas canvas;
    drawCmdList->Playback(canvas);
}

/**
 * @tc.name: Restore001
 * @tc.desc: Test the playback of the Restore function.
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RecordingCanvasTest, Restore001, TestSize.Level1)
{
    auto recordingCanvas = std::make_shared<RecordingCanvas>(10, 20);
    EXPECT_TRUE(recordingCanvas != nullptr);
    recordingCanvas->Restore();
    auto drawCmdList = recordingCanvas->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList != nullptr);
    Canvas canvas;
    drawCmdList->Playback(canvas);
}

/**
 * @tc.name: AttachPen001
 * @tc.desc: Test the playback of the AttachPen function.
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RecordingCanvasTest, AttachPen001, TestSize.Level1)
{
    auto recordingCanvas = std::make_shared<RecordingCanvas>(10, 20);
    EXPECT_TRUE(recordingCanvas != nullptr);
    Pen pen(Color::COLOR_GREEN);
    recordingCanvas->AttachPen(pen);
    auto drawCmdList = recordingCanvas->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList != nullptr);
    Canvas canvas;
    drawCmdList->Playback(canvas);
}

/**
 * @tc.name: DetachPen001
 * @tc.desc: Test the playback of the DetachPen function.
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RecordingCanvasTest, DetachPen001, TestSize.Level1)
{
    auto recordingCanvas = std::make_shared<RecordingCanvas>(10, 20);
    EXPECT_TRUE(recordingCanvas != nullptr);
    recordingCanvas->DetachPen();
    auto drawCmdList = recordingCanvas->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList != nullptr);
    Canvas canvas;
    drawCmdList->Playback(canvas);
}

/**
 * @tc.name: AttachBrush001
 * @tc.desc: Test the playback of the AttachBrush function.
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RecordingCanvasTest, AttachBrush001, TestSize.Level1)
{
    auto recordingCanvas = std::make_shared<RecordingCanvas>(10, 20);
    EXPECT_TRUE(recordingCanvas != nullptr);
    Brush brush(Color::COLOR_GREEN);
    recordingCanvas->AttachBrush(brush);
    auto drawCmdList = recordingCanvas->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList != nullptr);
    Canvas canvas;
    drawCmdList->Playback(canvas);
}

/**
 * @tc.name: DetachBrush001
 * @tc.desc: Test the playback of the DetachBrush function.
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RecordingCanvasTest, DetachBrush001, TestSize.Level1)
{
    auto recordingCanvas = std::make_shared<RecordingCanvas>(10, 20);
    EXPECT_TRUE(recordingCanvas != nullptr);
    recordingCanvas->DetachBrush();
    auto drawCmdList = recordingCanvas->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList != nullptr);
    Canvas canvas;
    drawCmdList->Playback(canvas);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS