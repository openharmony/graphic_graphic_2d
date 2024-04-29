/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "draw/canvas.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class CanvasTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void CanvasTest::SetUpTestCase() {}
void CanvasTest::TearDownTestCase() {}
void CanvasTest::SetUp() {}
void CanvasTest::TearDown() {}

/**
 * @tc.name: CreateAndDestroy001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(CanvasTest, CreateAndDestroy001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    EXPECT_TRUE(canvas != nullptr);
}

/**
 * @tc.name: CanvasBindTest001
 * @tc.desc: Test for bind Bitmap function.
 * @tc.type: FUNC
 * @tc.require: I719NQ
 */
HWTEST_F(CanvasTest, CanvasBindTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(canvas != nullptr);
    Bitmap bitmap;
    canvas->Bind(bitmap);
}

/**
 * @tc.name: CanvasGetTotalMatrixTest001
 * @tc.desc: Test for geting the total matrix of Canvas to device.
 * @tc.type: FUNC
 * @tc.require: I719NQ
 */
HWTEST_F(CanvasTest, CanvasGetTotalMatrixTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(canvas != nullptr);
    auto matrix = std::make_unique<Matrix>(canvas->GetTotalMatrix());
    EXPECT_TRUE(matrix != nullptr);
}

/**
 * @tc.name: CanvasGetLocalClipBoundsTest001
 * @tc.desc: Test for geting bounds of clip in local coordinates.
 * @tc.type: FUNC
 * @tc.require: I719NQ
 */
HWTEST_F(CanvasTest, CanvasGetLocalClipBoundsTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(canvas != nullptr);
    auto rect = std::make_unique<Rect>(canvas->GetLocalClipBounds());
    EXPECT_TRUE(rect != nullptr);
}

/**
 * @tc.name: CanvasGetDeviceClipBoundsTest001
 * @tc.desc: Test for geting bounds of clip in device corrdinates.
 * @tc.type: FUNC
 * @tc.require: I719NQ
 */
HWTEST_F(CanvasTest, CanvasGetDeviceClipBoundsTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(canvas != nullptr);
    auto rect = std::make_unique<RectI>(canvas->GetDeviceClipBounds());
    EXPECT_TRUE(rect != nullptr);
}

#ifdef ACE_ENABLE_GPU
/**
 * @tc.name: CanvasGetGPUContextTest001
 * @tc.desc: Test for geting gpu context.
 * @tc.type: FUNC
 * @tc.require: I782P9
 */
HWTEST_F(CanvasTest, CanvasGetGPUContextTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(canvas != nullptr);
    auto gpuContetxt = canvas->GetGPUContext();
    EXPECT_TRUE(gpuContetxt == nullptr);
}
#endif

/**
 * @tc.name: CanvasGetWidthTest001
 * @tc.desc: Test for geting width of Canvas.
 * @tc.type: FUNC
 * @tc.require: I719NQ
 */
HWTEST_F(CanvasTest, CanvasGetWidthTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(canvas != nullptr);
    auto rect = canvas->GetWidth();
    EXPECT_EQ(rect, 0);
}

/**
 * @tc.name: CanvasGetHeightTest001
 * @tc.desc: Test for geting height of Canvas.
 * @tc.type: FUNC
 * @tc.require: I719NQ
 */
HWTEST_F(CanvasTest, CanvasGetHeightTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(canvas != nullptr);
    auto rect = canvas->GetHeight();
    EXPECT_EQ(rect, 0);
}

/**
 * @tc.name: CanvasDrawPointTest001
 * @tc.desc: Test for DrawPoint function.
 * @tc.type: FUNC
 * @tc.require: I719NQ
 */
HWTEST_F(CanvasTest, CanvasDrawPointTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(canvas != nullptr);
    Point point(10.0f, 20.0f);
    canvas->DrawPoint(point);
}

/**
 * @tc.name: CanvasDrawLineTest001
 * @tc.desc: Test for DrawLine function.
 * @tc.type: FUNC
 * @tc.require: I719NQ
 */
HWTEST_F(CanvasTest, CanvasDrawLineTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(canvas != nullptr);
    Point startPoint(10.0f, 20.0f);
    Point endPoint(30.0f, 20.0f);
    canvas->DrawLine(startPoint, endPoint);
}

/**
 * @tc.name: CanvasDrawRectTest001
 * @tc.desc: Test for DrawRect function.
 * @tc.type: FUNC
 * @tc.require: I719NQ
 */
HWTEST_F(CanvasTest, CanvasDrawRectTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(canvas != nullptr);
    Rect rect(0.0f, 0.0f, 10.0f, 20.0f);
    canvas->DrawRect(rect);
}

/**
 * @tc.name: CanvasDrawRoundRectTest001
 * @tc.desc: Test for DrawRoundRect function.
 * @tc.type: FUNC
 * @tc.require: I719NQ
 */
HWTEST_F(CanvasTest, CanvasDrawRoundRectTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(canvas != nullptr);
    Rect rect(0.0f, 0.0f, 10.0f, 20.0f);
    RoundRect roundRect(rect, 1.0f, 1.0f);
    canvas->DrawRoundRect(roundRect);
}

/**
 * @tc.name: CanvasDrawNestedRoundRectTest001
 * @tc.desc: Test for DrawNestedRoundRect function.
 * @tc.type: FUNC
 * @tc.require: I719NQ
 */
HWTEST_F(CanvasTest, CanvasDrawNestedRoundRectTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(canvas != nullptr);
    Rect rect1(0.0f, 0.0f, 10.0f, 20.0f);
    RoundRect roundRect1(rect1, 1.0f, 1.0f);
    Rect rect2(0.0f, 0.0f, 5.0f, 10.0f);
    RoundRect roundRect2(rect2, 1.0f, 1.0f);
    canvas->DrawNestedRoundRect(roundRect1, roundRect2);
}

/**
 * @tc.name: CanvasDrawArcTest001
 * @tc.desc: Test for DrawArc function.
 * @tc.type: FUNC
 * @tc.require: I719NQ
 */
HWTEST_F(CanvasTest, CanvasDrawArcTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(canvas != nullptr);
    Rect rect(0.0f, 0.0f, 10.0f, 20.0f);
    canvas->DrawArc(rect, 0.0f, 90.0f);
}

/**
 * @tc.name: CanvasDrawPieTest001
 * @tc.desc: Test for DrawPie function.
 * @tc.type: FUNC
 * @tc.require: I719NQ
 */
HWTEST_F(CanvasTest, CanvasDrawPieTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(canvas != nullptr);
    Rect rect(0.0f, 0.0f, 10.0f, 20.0f);
    canvas->DrawPie(rect, 0.0f, 90.0f);
}

/**
 * @tc.name: CanvasDrawOvalTest001
 * @tc.desc: Test for DrawOval function.
 * @tc.type: FUNC
 * @tc.require: I719NQ
 */
HWTEST_F(CanvasTest, CanvasDrawOvalTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(canvas != nullptr);
    Rect rect(0.0f, 0.0f, 10.0f, 20.0f);
    canvas->DrawOval(rect);
}

/**
 * @tc.name: CanvasDrawCircleTest001
 * @tc.desc: Test for DrawOval function.
 * @tc.type: FUNC
 * @tc.require: I719NQ
 */
HWTEST_F(CanvasTest, CanvasDrawCircleTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(canvas != nullptr);
    Point centerpoint(10.0f, 20.0f);
    canvas->DrawCircle(centerpoint, 10.0f);
}

/**
 * @tc.name: CanvasDrawPathTest001
 * @tc.desc: Test for DrawPath function.
 * @tc.type: FUNC
 * @tc.require: I719NQ
 */
HWTEST_F(CanvasTest, CanvasDrawPathTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(canvas != nullptr);
    Path path;
    canvas->DrawPath(path);
}

/**
 * @tc.name: CanvasDrawBackgroundTest001
 * @tc.desc: Test for DrawBackground function.
 * @tc.type: FUNC
 * @tc.require: I719NQ
 */
HWTEST_F(CanvasTest, CanvasDrawBackgroundTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(canvas != nullptr);
    Brush brush(Color::COLOR_RED);
    canvas->DrawBackground(brush);
}

/**
 * @tc.name: CanvasDrawShadowTest001
 * @tc.desc: Test for DrawShadow function.
 * @tc.type: FUNC
 * @tc.require: I719NQ
 */
HWTEST_F(CanvasTest, CanvasDrawShadowTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(canvas != nullptr);
    Path path;
    Point3 planeParams(1.0f, 0.0f, 0.0f);
    Point3 devLightPos(1.0f, 1.0f, 1.0f);
    canvas->DrawShadow(path, planeParams, devLightPos, 1.0f, Color::COLOR_BLACK, Color::COLOR_BLUE, ShadowFlags::NONE);
}

/**
 * @tc.name: CanvasDrawRegionTest001
 * @tc.desc: Test for drawing Region on the Canvas.
 * @tc.type: FUNC
 * @tc.require: I719R9
 */
HWTEST_F(CanvasTest, CanvasDrawRegionTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(canvas != nullptr);
    Region region;
    canvas->DrawRegion(region);
}

/**
 * @tc.name: CanvasDrawAtlasTest001
 * @tc.desc: Test for drawing Atlas on the Canvas.
 * @tc.type: FUNC
 * @tc.require: I719R9
 */
HWTEST_F(CanvasTest, CanvasDrawAtlasTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(canvas != nullptr);
    Bitmap bitmap;
    BitmapFormat format {ColorType::COLORTYPE_RGBA_8888, ALPHATYPE_OPAQUE};
    bitmap.Build(200, 200, format);
    bitmap.ClearWithColor(Color::COLOR_WHITE);
    RSXform xform[] = { {25, 36, 2, 5}, {7, 8, 9, 12} };
    Rect rect[] = { {0, 0, 50, 50}, {50, 50, 100, 100}};
    canvas->DrawAtlas(bitmap.MakeImage().get(), xform, rect, nullptr, 2,
        BlendMode::SRC, SamplingOptions(), nullptr);
    Brush brush;
    ColorQuad colors[] = {0xffffffff, 0xff000000};
    Rect cullRect(0, 0, 100, 100);
    canvas->DrawAtlas(bitmap.MakeImage().get(), xform, rect, colors, 2,
        BlendMode::SRC, SamplingOptions(), &cullRect);
    canvas->DrawAtlas(bitmap.MakeImage().get(), xform, rect, colors, -10,
        BlendMode::SRC, SamplingOptions(), &cullRect);
    canvas->DrawAtlas(bitmap.MakeImage().get(), xform, rect, colors, 5000,
        BlendMode::SRC, SamplingOptions(), &cullRect);
    canvas->DrawAtlas(nullptr, xform, rect, colors, 2,
        BlendMode::SRC, SamplingOptions(), &cullRect);
}

/**
 * @tc.name: CanvasDrawBitmapTest001
 * @tc.desc: Test for drawing Bitmap on the Canvas.
 * @tc.type: FUNC
 * @tc.require: I719R9
 */
HWTEST_F(CanvasTest, CanvasDrawBitmapTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(canvas != nullptr);
    Bitmap bitmap;
    canvas->DrawBitmap(bitmap, 10.0f, 10.0f);
}

/**
 * @tc.name: CanvasDrawImageTest001
 * @tc.desc: Test for drawing image on the Canvas.
 * @tc.type: FUNC
 * @tc.require: I719R9
 */
HWTEST_F(CanvasTest, CanvasDrawImageTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(canvas != nullptr);
    Image image;
    SamplingOptions samplingOptions;
    canvas->DrawImage(image, 10.0f, 10.0f, samplingOptions);
}

/**
 * @tc.name: CanvasDrawImageRectTest001
 * @tc.desc: Test for DrawImageRect function.
 * @tc.type: FUNC
 * @tc.require: I719R9
 */
HWTEST_F(CanvasTest, CanvasDrawImageRectTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(canvas != nullptr);
    Image image;
    Rect srcRect(0.0f, 0.0f, 10.0f, 20.0f);
    Rect dstRect(0.0f, 0.0f, 10.0f, 20.0f);
    SamplingOptions samplingOptions;
    canvas->DrawImageRect(image, srcRect, dstRect, samplingOptions);
}

/**
 * @tc.name: CanvasDrawImageRectTest002
 * @tc.desc: Test for DrawImageRect function.
 * @tc.type: FUNC
 * @tc.require: I719R9
 */
HWTEST_F(CanvasTest, CanvasDrawImageRectTest002, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(canvas != nullptr);
    Image image;
    Rect dstRect(0.0f, 0.0f, 10.0f, 20.0f);
    SamplingOptions samplingOptions;
    canvas->DrawImageRect(image, dstRect, samplingOptions);
}

/**
 * @tc.name: CanvasDrawPictureTest001
 * @tc.desc: Test for DrawPicture function.
 * @tc.type: FUNC
 * @tc.require: I719R9
 */
HWTEST_F(CanvasTest, CanvasDrawPictureTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(canvas != nullptr);
    Picture pic;
    canvas->DrawPicture(pic);
}

/**
 * @tc.name: CanvasClipRectTest001
 * @tc.desc: Test replacing the clipping area with the intersection or difference between clipping area and Rect.
 * @tc.type: FUNC
 * @tc.require: I719R9
 */
HWTEST_F(CanvasTest, CanvasClipRectTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(canvas != nullptr);
    Rect rect(0.0f, 0.0f, 10.0f, 20.0f);
    canvas->ClipRect(rect, ClipOp::DIFFERENCE, true);
}

/**
 * @tc.name: CanvasClipRoundRectTest001
 * @tc.desc: Test replacing the clipping area with the intersection or difference of clipping area and Rect.
 * @tc.type: FUNC
 * @tc.require: I719R9
 */
HWTEST_F(CanvasTest, CanvasClipRoundRectTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(canvas != nullptr);
    Rect rect(0.0f, 0.0f, 10.0f, 20.0f);
    RoundRect roundRect(rect, 1.0f, 1.0f);
    canvas->ClipRoundRect(roundRect, ClipOp::DIFFERENCE, true);
}

/**
 * @tc.name: CanvasClipPathTest001
 * @tc.desc: Test replacing the clipping area with the intersection or difference between clipping area and path.
 * @tc.type: FUNC
 * @tc.require: I719R9
 */
HWTEST_F(CanvasTest, CanvasClipPathTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(canvas != nullptr);
    Path path;
    canvas->ClipPath(path, ClipOp::DIFFERENCE, true);
}

/**
 * @tc.name: CanvasSetMatrixTest001
 * @tc.desc: Test for SetMatrix function.
 * @tc.type: FUNC
 * @tc.require: I719R9
 */
HWTEST_F(CanvasTest, CanvasSetMatrixTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(canvas != nullptr);
    Matrix matrix;
    canvas->SetMatrix(matrix);
}

/**
 * @tc.name: CanvasResetMatrixTest001
 * @tc.desc: Test for ResetMatrix function.
 * @tc.type: FUNC
 * @tc.require: I719R9
 */
HWTEST_F(CanvasTest, CanvasResetMatrixTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(canvas != nullptr);
    Matrix matrix;
    canvas->SetMatrix(matrix);
    canvas->ResetMatrix();
}

/**
 * @tc.name: CanvasConcatMatrixTest001
 * @tc.desc: Test for ConcatMatrix function.
 * @tc.type: FUNC
 * @tc.require: I719R9
 */
HWTEST_F(CanvasTest, CanvasConcatMatrixTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(canvas != nullptr);
    Matrix matrix;
    canvas->ConcatMatrix(matrix);
}

/**
 * @tc.name: CanvasTranslateTest001
 * @tc.desc: Test for Translate function.
 * @tc.type: FUNC
 * @tc.require: I719R9
 */
HWTEST_F(CanvasTest, CanvasTranslateTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(canvas != nullptr);
    canvas->Translate(1.0f, 1.0f);
}

/**
 * @tc.name: CanvasScaleTest001
 * @tc.desc: Test for Scale function.
 * @tc.type: FUNC
 * @tc.require: I719R9
 */
HWTEST_F(CanvasTest, CanvasScaleTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(canvas != nullptr);
    canvas->Scale(1.0f, 1.0f);
}

/**
 * @tc.name: CanvasRotateTest001
 * @tc.desc: Test for Rotating Matrix by degrees.
 * @tc.type: FUNC
 * @tc.require: I719R9
 */
HWTEST_F(CanvasTest, CanvasRotateTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(canvas != nullptr);
    canvas->Rotate(60.0f);
}

/**
 * @tc.name: CanvasRotateTest002
 * @tc.desc: Test for Rotating Matrix by degrees.
 * @tc.type: FUNC
 * @tc.require: I719R9
 */
HWTEST_F(CanvasTest, CanvasRotateTest002, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(canvas != nullptr);
    canvas->Rotate(60.0f, 10.0f, 10.0f);
}

/**
 * @tc.name: CanvasShearTest001
 * @tc.desc: Test for Shear function.
 * @tc.type: FUNC
 * @tc.require: I719R9
 */
HWTEST_F(CanvasTest, CanvasShearTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(canvas != nullptr);
    canvas->Shear(10.0f, 10.0f);
}

/**
 * @tc.name: CanvasFlushTest001
 * @tc.desc: Test for Flush function.
 * @tc.type: FUNC
 * @tc.require: I719R9
 */
HWTEST_F(CanvasTest, CanvasFlushTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(canvas != nullptr);
    canvas->Flush();
}

/**
 * @tc.name: CanvasClearTest001
 * @tc.desc: Test for Clear function.
 * @tc.type: FUNC
 * @tc.require: I719R9
 */
HWTEST_F(CanvasTest, CanvasClearTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(canvas != nullptr);
    canvas->Clear(Color::COLOR_BLUE);
}

/**
 * @tc.name: CanvasSaveTest001
 * @tc.desc: Test for Save function.
 * @tc.type: FUNC
 * @tc.require: I719R9
 */
HWTEST_F(CanvasTest, CanvasSaveTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(canvas != nullptr);
    canvas->Save();
}

/**
 * @tc.name: CanvasSaveLayerTest001
 * @tc.desc: Test for saving Matrix and clipping area, and allocates Surface for subsequent drawing.
 * @tc.type: FUNC
 * @tc.require: I719U5
 */
HWTEST_F(CanvasTest, CanvasSaveLayerTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(canvas != nullptr);
    SaveLayerOps saveLayerOps;
    canvas->SaveLayer(saveLayerOps);
}

/**
 * @tc.name: CanvasSaveLayerTest002
 * @tc.desc: Test for saving Matrix and clipping area, and allocates Surface for subsequent drawing.
 * @tc.type: FUNC
 * @tc.require: I719U5
 */
HWTEST_F(CanvasTest, CanvasSaveLayerTest002, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(canvas != nullptr);
    Rect rect(0.0f, 0.0f, 10.0f, 20.0f);
    Brush brush;
    uint32_t saveLayerFlags = 0;
    SaveLayerOps saveLayerRec(&rect, &brush, saveLayerFlags);
    canvas->SaveLayer(saveLayerRec);
}

/**
 * @tc.name: CanvasRestoreTest001
 * @tc.desc: Test for Restore function.
 * @tc.type: FUNC
 * @tc.require: I719U5
 */
HWTEST_F(CanvasTest, CanvasRestoreTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(canvas != nullptr);
    Rect rect(0.0f, 0.0f, 10.0f, 20.0f);
    Brush brush;
    uint32_t saveLayerFlags = 0;
    SaveLayerOps saveLayerRec(&rect, &brush, saveLayerFlags);
    canvas->SaveLayer(saveLayerRec);
    canvas->Restore();
}

/**
 * @tc.name: CanvasGetSaveCountTest001
 * @tc.desc: Test for geting the number of saved states.
 * @tc.type: FUNC
 * @tc.require: I719U5
 */
HWTEST_F(CanvasTest, CanvasGetSaveCountTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(canvas != nullptr);
    canvas->Save();
    EXPECT_TRUE(2 == canvas->GetSaveCount());
}

/**
 * @tc.name: CanvasRestoreToCountTest001
 * @tc.desc: Test for restoring Canvas Matrix and clip value state to count.
 * @tc.type: FUNC
 * @tc.require: I719U5
 */
HWTEST_F(CanvasTest, CanvasRestoreToCountTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(canvas != nullptr);
    canvas->RestoreToCount(2);
}

/**
 * @tc.name: CanvasAttachAndDetachPenTest001
 * @tc.desc: Test for AttachPen and DetachPen functions.
 * @tc.type: FUNC
 * @tc.require: I719U5
 */
HWTEST_F(CanvasTest, CanvasAttachAndDetachPenTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(canvas != nullptr);
    Pen pen(Color::COLOR_GREEN);
    canvas->AttachPen(pen);
    canvas->DetachPen();
}

/**
 * @tc.name: CanvasAttachAndDetachBrushTest001
 * @tc.desc: Test for AttachBrush and DetachBrush functions.
 * @tc.type: FUNC
 * @tc.require: I719U5
 */
HWTEST_F(CanvasTest, CanvasAttachAndDetachBrushTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(canvas != nullptr);
    Brush brush(Color::COLOR_GREEN);
    canvas->AttachBrush(brush);
    canvas->DetachBrush();
}

/**
 * @tc.name: GetRecordingStateTest001
 * @tc.desc: Test for GetRecordingState functions.
 * @tc.type: FUNC
 * @tc.require: I719U5
 */
HWTEST_F(CanvasTest, GetRecordingStateTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(canvas != nullptr);
    canvas->SetRecordingState(true);
    bool state = canvas->GetRecordingState();
    ASSERT_TRUE(state);
}

/**
 * @tc.name: SetRecordingStateTest001
 * @tc.desc: Test for SetRecordingState functions.
 * @tc.type: FUNC
 * @tc.require: I719U5
 */
HWTEST_F(CanvasTest, SetRecordingStateTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(canvas != nullptr);
    canvas->SetRecordingState(false);
    bool state = canvas->GetRecordingState();
    ASSERT_TRUE(!state);
}

/**
 * @tc.name: GetDrawingTypeTest001
 * @tc.desc: Test for GetDrawingType functions.
 * @tc.type: FUNC
 * @tc.require: I719U5
 */
HWTEST_F(CanvasTest, GetDrawingTypeTest001, TestSize.Level1)
{
    std::shared_ptr<Drawing::OverDrawCanvas> overDrawCanvas;
    DrawingType type = overDrawCanvas->GetDrawingType();
    ASSERT_TRUE(type == DrawingType::OVER_DRAW);
}

/**
 * @tc.name: GetBounds001
 * @tc.desc: Test for geting the bounds of layer.
 * @tc.type: FUNC
 * @tc.require: I719U5
 */
HWTEST_F(CanvasTest, GetBounds001, TestSize.Level1)
{
    Rect rect(0.0f, 0.0f, 10.0f, 20.0f);
    Brush brush;
    uint32_t saveLayerFlags = 0;
    SaveLayerOps saveLayerRec(&rect, &brush, saveLayerFlags);
    auto ret = saveLayerRec.GetBounds();
    EXPECT_EQ(ret->GetLeft(), 0.0f);
    EXPECT_EQ(ret->GetBottom(), 20.0f);
}

/**
 * @tc.name: GetBrush001
 * @tc.desc: Test for geting the brush of layer.
 * @tc.type: FUNC
 * @tc.require: I719U5
 */
HWTEST_F(CanvasTest, GetBrush001, TestSize.Level1)
{
    Rect rect(0.0f, 0.0f, 10.0f, 20.0f);
    Brush brush;
    uint32_t saveLayerFlags = 0;
    SaveLayerOps saveLayerRec(&rect, &brush, saveLayerFlags);
    auto ret = saveLayerRec.GetBrush();
    EXPECT_TRUE(ret != nullptr);
}

/**
 * @tc.name: GetSaveLayerFlags001
 * @tc.desc: Test for geting the options to modify layer.
 * @tc.type: FUNC
 * @tc.require: I719U5
 */
HWTEST_F(CanvasTest, GetSaveLayerFlags001, TestSize.Level1)
{
    Rect rect(0.0f, 0.0f, 10.0f, 20.0f);
    Brush brush;
    uint32_t saveLayerFlags = 0;
    SaveLayerOps saveLayerRec(&rect, &brush, saveLayerFlags);
    auto ret = saveLayerRec.GetSaveLayerFlags();
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.name: AutoCanvasRestoreTest001
 * @tc.desc: Test for Creating AutoCanvasRestore;
 * @tc.type: FUNC
 * @tc.require: I719U5
 */
HWTEST_F(CanvasTest, AutoCanvasRestoreTest001, TestSize.Level1)
{
    Canvas canvas;
    bool doSave = true;
    auto autoCanvasRestore = std::make_unique<AutoCanvasRestore>(canvas, doSave);
    ASSERT_TRUE(autoCanvasRestore != nullptr);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
