/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include <cstddef>
#include "gtest/gtest.h"
#include "skia_adapter/skia_canvas.h"
#include "draw/core_canvas.h"
#include "skia_adapter/skia_canvas_autocache.h"
#include "skia_adapter/skia_oplist_handle.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaCanvasTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SkiaCanvasTest::SetUpTestCase() {}
void SkiaCanvasTest::TearDownTestCase() {}
void SkiaCanvasTest::SetUp() {}
void SkiaCanvasTest::TearDown() {}

/**
 * @tc.name: Bind001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(SkiaCanvasTest, Bind001, TestSize.Level1)
{
    Bitmap bitmap;
    SkiaCanvas skiaCanvas;
    skiaCanvas.Bind(bitmap);

    skiaCanvas.ImportSkCanvas(nullptr);
    skiaCanvas.Bind(bitmap);
}

/**
 * @tc.name: DrawPoint001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(SkiaCanvasTest, DrawPoint001, TestSize.Level1)
{
    Point point;
    Paint paint;
    paint.SetStyle(Paint::PaintStyle::PAINT_FILL);
    SkiaCanvas skiaCanvas;
    skiaCanvas.AttachPaint(paint);
    skiaCanvas.DrawPoint(point);

    skiaCanvas.ImportSkCanvas(nullptr);
    skiaCanvas.AttachPaint(paint);
    skiaCanvas.DrawPoint(point);
}

/**
 * @tc.name: DrawLine001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(SkiaCanvasTest, DrawLine001, TestSize.Level1)
{
    Point startPt;
    Point endPt;
    Paint paint;
    paint.SetStyle(Paint::PaintStyle::PAINT_FILL);
    SkiaCanvas skiaCanvas;
    skiaCanvas.AttachPaint(paint);
    skiaCanvas.DrawLine(startPt, endPt);

    skiaCanvas.ImportSkCanvas(nullptr);
    skiaCanvas.DrawLine(startPt, endPt);
}

/**
 * @tc.name: DrawRect001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(SkiaCanvasTest, DrawRect001, TestSize.Level1)
{
    Rect rect;
    Paint paint;
    paint.SetStyle(Paint::PaintStyle::PAINT_FILL);
    SkiaCanvas skiaCanvas;
    skiaCanvas.AttachPaint(paint);
    skiaCanvas.DrawRect(rect);

    skiaCanvas.ImportSkCanvas(nullptr);
    skiaCanvas.DrawRect(rect);
}

/**
 * @tc.name: DrawRoundRect001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(SkiaCanvasTest, DrawRoundRect001, TestSize.Level1)
{
    RoundRect roundRect;
    Paint paint;
    paint.SetStyle(Paint::PaintStyle::PAINT_FILL);
    SkiaCanvas skiaCanvas;
    skiaCanvas.AttachPaint(paint);
    skiaCanvas.DrawRoundRect(roundRect);

    skiaCanvas.ImportSkCanvas(nullptr);
    skiaCanvas.DrawRoundRect(roundRect);
}

/**
 * @tc.name: DrawNestedRoundRect001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(SkiaCanvasTest, DrawNestedRoundRect001, TestSize.Level1)
{
    RoundRect outer;
    RoundRect inner;
    Paint paint;
    paint.SetStyle(Paint::PaintStyle::PAINT_FILL);
    SkiaCanvas skiaCanvas;
    skiaCanvas.AttachPaint(paint);
    skiaCanvas.DrawNestedRoundRect(outer, inner);

    skiaCanvas.ImportSkCanvas(nullptr);
    skiaCanvas.DrawNestedRoundRect(outer, inner);
}

/**
 * @tc.name: DrawArc001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(SkiaCanvasTest, DrawArc001, TestSize.Level1)
{
    Rect oval;
    scalar startAngle = 30.0f;
    scalar sweepAngle = 45.0f;
    Paint paint;
    paint.SetStyle(Paint::PaintStyle::PAINT_FILL);
    SkiaCanvas skiaCanvas;
    skiaCanvas.AttachPaint(paint);
    skiaCanvas.DrawArc(oval, startAngle, sweepAngle);

    skiaCanvas.ImportSkCanvas(nullptr);
    skiaCanvas.DrawArc(oval, startAngle, sweepAngle);
}

/**
 * @tc.name: DrawPie001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(SkiaCanvasTest, DrawPie001, TestSize.Level1)
{
    Rect oval;
    scalar startAngle = 45.0f;
    scalar sweepAngle = 60.0f;
    Paint paint;
    paint.SetStyle(Paint::PaintStyle::PAINT_FILL);
    SkiaCanvas skiaCanvas;
    skiaCanvas.AttachPaint(paint);
    skiaCanvas.DrawPie(oval, startAngle, sweepAngle);

    skiaCanvas.ImportSkCanvas(nullptr);
    skiaCanvas.DrawPie(oval, startAngle, sweepAngle);
}

/**
 * @tc.name: DrawOval001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(SkiaCanvasTest, DrawOval001, TestSize.Level1)
{
    Rect oval;
    Paint paint;
    paint.SetStyle(Paint::PaintStyle::PAINT_FILL);
    SkiaCanvas skiaCanvas;
    skiaCanvas.AttachPaint(paint);
    skiaCanvas.DrawOval(oval);

    skiaCanvas.ImportSkCanvas(nullptr);
    skiaCanvas.DrawOval(oval);
}

/**
 * @tc.name: DrawCircle001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(SkiaCanvasTest, DrawCircle001, TestSize.Level1)
{
    Point centerPt;
    scalar radius = 20.0f;
    Paint paint;
    paint.SetStyle(Paint::PaintStyle::PAINT_FILL);
    SkiaCanvas skiaCanvas;
    skiaCanvas.AttachPaint(paint);
    skiaCanvas.DrawCircle(centerPt, radius);

    skiaCanvas.ImportSkCanvas(nullptr);
    skiaCanvas.DrawCircle(centerPt, radius);
}

/**
 * @tc.name: DrawPath001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(SkiaCanvasTest, DrawPath001, TestSize.Level1)
{
    Path path;
    Paint paint;
    paint.SetStyle(Paint::PaintStyle::PAINT_FILL);
    SkiaCanvas skiaCanvas;
    skiaCanvas.AttachPaint(paint);
    skiaCanvas.DrawPath(path);

    skiaCanvas.ImportSkCanvas(nullptr);
    skiaCanvas.DrawPath(path);
}

/**
 * @tc.name: DrawBackground001
 * @tc.desc: Draw Background Test
 * @tc.type: FUNC
 * @tc.require: issuel#I6Q4ZH
 */
HWTEST_F(SkiaCanvasTest, DrawBackground001, TestSize.Level2)
{
    Brush brush;
    SkiaCanvas skiaCanvas;
    skiaCanvas.DrawBackground(brush);

    skiaCanvas.ImportSkCanvas(nullptr);
    skiaCanvas.DrawBackground(brush);
}

/**
 * @tc.name: DrawShadow001
 * @tc.desc: Draw Shadow Test
 * @tc.type: FUNC
 * @tc.require: issuel#I6Q4ZH
 */
HWTEST_F(SkiaCanvasTest, DrawShadow001, TestSize.Level2)
{
    Path path;
    Point3 planeParams;
    Point3 devLightPos;
    Color ambientColor;
    Color spotColor;
    SkiaCanvas skiaCanvas;
    skiaCanvas.DrawShadow(path, planeParams, devLightPos, 1.0f, ambientColor, spotColor, ShadowFlags::NONE);

    skiaCanvas.ImportSkCanvas(nullptr);
    skiaCanvas.DrawShadow(path, planeParams, devLightPos, 1.0f, ambientColor, spotColor, ShadowFlags::NONE);
}

/**
 * @tc.name: DrawBitmap001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(SkiaCanvasTest, DrawBitmap001, TestSize.Level1)
{
    Bitmap bitmap;
    scalar px = 60.0f;
    scalar py = 30.0f;
    Paint paint;
    paint.SetStyle(Paint::PaintStyle::PAINT_FILL);
    SkiaCanvas skiaCanvas;
    skiaCanvas.AttachPaint(paint);
    skiaCanvas.DrawBitmap(bitmap, px, py);

    skiaCanvas.ImportSkCanvas(nullptr);
    skiaCanvas.DrawBitmap(bitmap, px, py);
}

/**
 * @tc.name: DrawImage001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(SkiaCanvasTest, DrawImage001, TestSize.Level1)
{
    Image image;
    scalar px = 30.0f;
    scalar py = 65.0f;
    SamplingOptions sampling;
    Paint paint;
    paint.SetStyle(Paint::PaintStyle::PAINT_FILL);
    SkiaCanvas skiaCanvas;
    skiaCanvas.AttachPaint(paint);
    skiaCanvas.DrawImage(image, px, py, sampling);

    skiaCanvas.ImportSkCanvas(nullptr);
    skiaCanvas.DrawImage(image, px, py, sampling);
}

/**
 * @tc.name: DrawImageRect001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(SkiaCanvasTest, DrawImageRect001, TestSize.Level1)
{
    Image image;
    Rect src;
    Rect dst;
    SamplingOptions sampling;
    Paint paint;
    paint.SetStyle(Paint::PaintStyle::PAINT_FILL);
    SkiaCanvas skiaCanvas;
    skiaCanvas.AttachPaint(paint);
    skiaCanvas.DrawImageRect(image, src, dst, sampling, SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT);

    skiaCanvas.ImportSkCanvas(nullptr);
    skiaCanvas.DrawImageRect(image, src, dst, sampling, SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT);
}

/**
 * @tc.name: DrawImageRect002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(SkiaCanvasTest, DrawImageRect002, TestSize.Level1)
{
    Image image;
    Rect dst;
    SamplingOptions sampling;
    Paint paint;
    paint.SetStyle(Paint::PaintStyle::PAINT_FILL);
    SkiaCanvas skiaCanvas;
    skiaCanvas.AttachPaint(paint);
    skiaCanvas.DrawImageRect(image, dst, sampling);

    skiaCanvas.ImportSkCanvas(nullptr);
    skiaCanvas.DrawImageRect(image, dst, sampling);
}

/**
 * @tc.name: DrawPicture001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(SkiaCanvasTest, DrawPicture001, TestSize.Level1)
{
    Picture picture;
    SkiaCanvas skiaCanvas;
    skiaCanvas.DrawPicture(picture);

    skiaCanvas.ImportSkCanvas(nullptr);
    skiaCanvas.DrawPicture(picture);
}

/**
 * @tc.name: ClipRoundRect001
 * @tc.desc: Clip Round Rect Test
 * @tc.type: FUNC
 * @tc.require: issuel#I6Q4ZH
 */
HWTEST_F(SkiaCanvasTest, ClipRoundRect001, TestSize.Level2)
{
    RoundRect roundRect;
    SkiaCanvas skiaCanvas;
    skiaCanvas.ClipRoundRect(roundRect, ClipOp::DIFFERENCE, false);

    skiaCanvas.ImportSkCanvas(nullptr);
    skiaCanvas.ClipRoundRect(roundRect, ClipOp::DIFFERENCE, false);
}

/**
 * @tc.name: ClipPath001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(SkiaCanvasTest, ClipPath001, TestSize.Level1)
{
    Path path;
    Paint paint;
    paint.SetStyle(Paint::PaintStyle::PAINT_FILL);
    SkiaCanvas skiaCanvas;
    skiaCanvas.AttachPaint(paint);
    skiaCanvas.ClipPath(path, ClipOp::DIFFERENCE, false);

    skiaCanvas.ImportSkCanvas(nullptr);
    skiaCanvas.ClipPath(path, ClipOp::DIFFERENCE, false);
}

/**
 * @tc.name: SetMatrix001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(SkiaCanvasTest, SetMatrix001, TestSize.Level1)
{
    Matrix matrix;
    Paint paint;
    paint.SetStyle(Paint::PaintStyle::PAINT_FILL);
    SkiaCanvas skiaCanvas;
    skiaCanvas.AttachPaint(paint);
    skiaCanvas.SetMatrix(matrix);

    skiaCanvas.ImportSkCanvas(nullptr);
    skiaCanvas.SetMatrix(matrix);
}

/**
 * @tc.name: ResetMatrix001
 * @tc.desc: Reset Matrix Test
 * @tc.type: FUNC
 * @tc.require: issuel#I6Q4ZH
 */
HWTEST_F(SkiaCanvasTest, ResetMatrix001, TestSize.Level2)
{
    Matrix matrix;
    SkiaCanvas skiaCanvas;
    skiaCanvas.SetMatrix(matrix);
    skiaCanvas.ResetMatrix();

    skiaCanvas.ImportSkCanvas(nullptr);
    skiaCanvas.ResetMatrix();
}

/**
 * @tc.name: ConcatMatrix001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(SkiaCanvasTest, ConcatMatrix001, TestSize.Level1)
{
    Matrix matrix;
    Paint paint;
    paint.SetStyle(Paint::PaintStyle::PAINT_FILL);
    SkiaCanvas skiaCanvas;
    skiaCanvas.AttachPaint(paint);
    skiaCanvas.ConcatMatrix(matrix);

    skiaCanvas.ImportSkCanvas(nullptr);
    skiaCanvas.ConcatMatrix(matrix);
}

/**
 * @tc.name: Rotate001
 * @tc.desc: Rotate Test
 * @tc.type: FUNC
 * @tc.require: issuel#I6Q4ZH
 */
HWTEST_F(SkiaCanvasTest, Rotate001, TestSize.Level2)
{
    SkiaCanvas skiaCanvas;
    skiaCanvas.Rotate(0.1f, 0.2f, 0.3f);

    skiaCanvas.ImportSkCanvas(nullptr);
    skiaCanvas.Rotate(0.1f, 0.2f, 0.3f);
}

/**
 * @tc.name: Shear001
 * @tc.desc: Shear Test
 * @tc.type: FUNC
 * @tc.require: issuel#I6Q4ZH
 */
HWTEST_F(SkiaCanvasTest, Shear001, TestSize.Level2)
{
    SkiaCanvas skiaCanvas;
    skiaCanvas.Shear(0.5f, 0.5f);

    skiaCanvas.ImportSkCanvas(nullptr);
    skiaCanvas.Shear(0.5f, 0.5f);
}

/**
 * @tc.name: Flush001
 * @tc.desc: Flush Test
 * @tc.type: FUNC
 * @tc.require: issuel#I6Q4ZH
 */
HWTEST_F(SkiaCanvasTest, Flush001, TestSize.Level2)
{
    SkiaCanvas skiaCanvas;
    skiaCanvas.Flush();

    skiaCanvas.ImportSkCanvas(nullptr);
    skiaCanvas.Flush();
}

/**
 * @tc.name: SaveLayer001
 * @tc.desc: SaveLayer Test
 * @tc.type: FUNC
 * @tc.require: issuel#I6Q4ZH
 */
HWTEST_F(SkiaCanvasTest, SaveLayer001, TestSize.Level2)
{
    Rect rect;
    Brush brush;
    SaveLayerOps slo(&rect, &brush);
    SkiaCanvas skiaCanvas;
    skiaCanvas.SaveLayer(slo);

    skiaCanvas.ImportSkCanvas(nullptr);
    skiaCanvas.SaveLayer(slo);
}

/**
 * @tc.name: GetTotalMatrixTest001
 * @tc.desc: Test for geting the total matrix of SkiaCanvas to device.
 * @tc.type: FUNC
 * @tc.require: I782P9
 */
HWTEST_F(SkiaCanvasTest, GetTotalMatrixTest001, TestSize.Level1)
{
    auto skiaCanvas = std::make_shared<SkiaCanvas>();
    ASSERT_TRUE(skiaCanvas != nullptr);

    skiaCanvas->ImportSkCanvas(nullptr);
    auto matrix = skiaCanvas->GetTotalMatrix();
}

/**
 * @tc.name: GetLocalClipBoundsTest001
 * @tc.desc: Test for geting bounds of clip in local coordinates.
 * @tc.type: FUNC
 * @tc.require: I782P9
 */
HWTEST_F(SkiaCanvasTest, GetLocalClipBoundsTest001, TestSize.Level1)
{
    auto skiaCanvas = std::make_shared<SkiaCanvas>();
    ASSERT_TRUE(skiaCanvas != nullptr);

    skiaCanvas->ImportSkCanvas(nullptr);
    auto rect = skiaCanvas->GetLocalClipBounds();
}

/**
 * @tc.name: GetDeviceClipBoundsTest001
 * @tc.desc: Test for geting bounds of clip in device corrdinates.
 * @tc.type: FUNC
 * @tc.require: I782P9
 */
HWTEST_F(SkiaCanvasTest, GetDeviceClipBoundsTest001, TestSize.Level1)
{
    auto skiaCanvas = std::make_shared<SkiaCanvas>();
    ASSERT_TRUE(skiaCanvas != nullptr);

    skiaCanvas->ImportSkCanvas(nullptr);
    auto rect = skiaCanvas->GetDeviceClipBounds();
}

/**
 * @tc.name: GetRoundInDeviceClipBoundsTest001
 * @tc.desc: Test for geting bounds of clip in device corrdinates.
 * @tc.type: FUNC
 * @tc.require: I782P9
 */
HWTEST_F(SkiaCanvasTest, GetRoundInDeviceClipBoundsTest001, TestSize.Level1)
{
    auto skiaCanvas = std::make_shared<SkiaCanvas>();
    ASSERT_TRUE(skiaCanvas != nullptr);

    skiaCanvas->ImportSkCanvas(nullptr);
    auto rect = skiaCanvas->GetRoundInDeviceClipBounds();
}

#ifdef ACE_ENABLE_GPU
/**
 * @tc.name: GetGPUContextTest001
 * @tc.desc: Test for geting gpu context.
 * @tc.type: FUNC
 * @tc.require: I782P9
 */
HWTEST_F(SkiaCanvasTest, GetGPUContextTest001, TestSize.Level1)
{
    auto skiaCanvas = std::make_shared<SkiaCanvas>();
    ASSERT_TRUE(skiaCanvas != nullptr);

    skiaCanvas->ImportSkCanvas(nullptr);
    auto gpuContetxt = skiaCanvas->GetGPUContext();
}
#endif

/**
 * @tc.name: GetWidth001
 * @tc.desc: Test GetWidth
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaCanvasTest, GetWidth001, TestSize.Level1)
{
    auto skiaCanvas = std::make_shared<SkiaCanvas>(nullptr);
    ASSERT_TRUE(skiaCanvas != nullptr);
    ASSERT_TRUE(skiaCanvas->GetWidth() >= 0);
}

/**
 * @tc.name: GetHeight001
 * @tc.desc: Test GetHeight
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaCanvasTest, GetHeight001, TestSize.Level1)
{
    auto skiaCanvas = std::make_shared<SkiaCanvas>(nullptr);
    ASSERT_TRUE(skiaCanvas != nullptr);
    ASSERT_TRUE(skiaCanvas->GetHeight() >= 0);
}

/**
 * @tc.name: GetImageInfo001
 * @tc.desc: Test GetImageInfo
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaCanvasTest, GetImageInfo001, TestSize.Level1)
{
    auto skiaCanvas = std::make_shared<SkiaCanvas>(nullptr);
    ASSERT_TRUE(skiaCanvas != nullptr);
    ASSERT_TRUE(skiaCanvas->GetImageInfo().GetWidth() >= 0);
}

/**
 * @tc.name: ReadPixels001
 * @tc.desc: Test ReadPixels
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaCanvasTest, ReadPixels001, TestSize.Level1)
{
    auto skiaCanvas1 = std::make_shared<SkiaCanvas>(nullptr);
    ASSERT_TRUE(skiaCanvas1 != nullptr);
    ImageInfo imageInfo;
    ASSERT_TRUE(!skiaCanvas1->ReadPixels(imageInfo, nullptr, 0, 0, 0));
    Bitmap bitmap;
    ASSERT_TRUE(!skiaCanvas1->ReadPixels(bitmap, 0, 0));

    auto skiaCanvas2 = std::make_shared<SkiaCanvas>();
    ASSERT_TRUE(skiaCanvas2 != nullptr);
    ASSERT_TRUE(!skiaCanvas2->ReadPixels(imageInfo, nullptr, 0, 0, 0));
    ASSERT_TRUE(!skiaCanvas2->ReadPixels(bitmap, 0, 0));
}

/**
 * @tc.name: DrawPoints001
 * @tc.desc: Test DrawPoints
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaCanvasTest, DrawPoints001, TestSize.Level1)
{
    auto skiaCanvas = std::make_shared<SkiaCanvas>(nullptr);
    ASSERT_TRUE(skiaCanvas != nullptr);
    skiaCanvas->DrawPoints(PointMode::POINTS_POINTMODE, 0, {});
}

/**
 * @tc.name: DrawColor001
 * @tc.desc: Test DrawColor
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaCanvasTest, DrawColor001, TestSize.Level1)
{
    auto skiaCanvas = std::make_shared<SkiaCanvas>(nullptr);
    ASSERT_TRUE(skiaCanvas != nullptr);
    skiaCanvas->DrawColor(0xFF000000, BlendMode::COLOR_BURN);
}

/**
 * @tc.name: ClipRect001
 * @tc.desc: Test ClipRect
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaCanvasTest, ClipRect001, TestSize.Level1)
{
    auto skiaCanvas = std::make_shared<SkiaCanvas>(nullptr);
    ASSERT_TRUE(skiaCanvas != nullptr);
    Rect rect;
    skiaCanvas->ClipRect(rect, ClipOp::DIFFERENCE, true);
}

/**
 * @tc.name: ClipIRect001
 * @tc.desc: Test ClipIRect
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaCanvasTest, ClipIRect001, TestSize.Level1)
{
    auto skiaCanvas = std::make_shared<SkiaCanvas>(nullptr);
    ASSERT_TRUE(skiaCanvas != nullptr);
    RectI rect;
    skiaCanvas->ClipIRect(rect, ClipOp::DIFFERENCE);
}

/**
 * @tc.name: ClipRegion001
 * @tc.desc: Test ClipRegion
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaCanvasTest, ClipRegion001, TestSize.Level1)
{
    auto skiaCanvas = std::make_shared<SkiaCanvas>(nullptr);
    ASSERT_TRUE(skiaCanvas != nullptr);
    Region region;
    skiaCanvas->ClipRegion(region, ClipOp::DIFFERENCE);
}

/**
 * @tc.name: IsClipEmpty001
 * @tc.desc: Test IsClipEmpty
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaCanvasTest, IsClipEmpty001, TestSize.Level1)
{
    auto skiaCanvas = std::make_shared<SkiaCanvas>();
    ASSERT_TRUE(skiaCanvas != nullptr);
    ASSERT_TRUE(skiaCanvas->IsClipEmpty());
    auto skiaCanvas2 = std::make_shared<SkiaCanvas>(nullptr);
    ASSERT_TRUE(skiaCanvas2 != nullptr);
    ASSERT_TRUE(!skiaCanvas2->IsClipEmpty());
}

/**
 * @tc.name: IsClipRect001
 * @tc.desc: Test IsClipRect
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaCanvasTest, IsClipRect001, TestSize.Level1)
{
    auto skiaCanvas = std::make_shared<SkiaCanvas>();
    ASSERT_TRUE(skiaCanvas != nullptr);
    ASSERT_TRUE(!skiaCanvas->IsClipRect());
    auto skiaCanvas2 = std::make_shared<SkiaCanvas>(nullptr);
    ASSERT_TRUE(skiaCanvas2 != nullptr);
    ASSERT_TRUE(!skiaCanvas2->IsClipRect());
}

/**
 * @tc.name: QuickReject001
 * @tc.desc: Test QuickReject
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaCanvasTest, QuickReject001, TestSize.Level1)
{
    auto skiaCanvas = std::make_shared<SkiaCanvas>();
    ASSERT_TRUE(skiaCanvas != nullptr);
    Rect rect{0, 0, 1, 1};
    ASSERT_TRUE(skiaCanvas->QuickReject(rect));
    auto skiaCanvas2 = std::make_shared<SkiaCanvas>(nullptr);
    ASSERT_TRUE(skiaCanvas2 != nullptr);
    ASSERT_TRUE(!skiaCanvas2->QuickReject(rect));
}

/**
 * @tc.name: Translate001
 * @tc.desc: Test Translate
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaCanvasTest, Translate001, TestSize.Level1)
{
    auto skiaCanvas = std::make_shared<SkiaCanvas>(nullptr);
    ASSERT_TRUE(skiaCanvas != nullptr);
    skiaCanvas->Translate(1, 1);
}

/**
 * @tc.name: Scale001
 * @tc.desc: Test Scale
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaCanvasTest, Scale001, TestSize.Level1)
{
    auto skiaCanvas = std::make_shared<SkiaCanvas>(nullptr);
    ASSERT_TRUE(skiaCanvas != nullptr);
    skiaCanvas->Scale(1, 1);
}

/**
 * @tc.name: Clear001
 * @tc.desc: Test Clear
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaCanvasTest, Clear001, TestSize.Level1)
{
    auto skiaCanvas = std::make_shared<SkiaCanvas>(nullptr);
    ASSERT_TRUE(skiaCanvas != nullptr);
    skiaCanvas->Clear(0xFF000000); // 0xFF000000: color
}

/**
 * @tc.name: Save001
 * @tc.desc: Test Save
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaCanvasTest, Save001, TestSize.Level1)
{
    auto skiaCanvas = std::make_shared<SkiaCanvas>(nullptr);
    ASSERT_TRUE(skiaCanvas != nullptr);
    skiaCanvas->Save();
    skiaCanvas->Restore();
    ASSERT_TRUE(skiaCanvas->GetSaveCount() == 0);
}

/**
 * @tc.name: DrawSymbol001
 * @tc.desc: Test DrawSymbol
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaCanvasTest, DrawSymbol001, TestSize.Level1)
{
    auto skiaCanvas = std::make_shared<SkiaCanvas>();
    ASSERT_TRUE(skiaCanvas != nullptr);
    DrawingHMSymbolData drawingHMSymbolData;
    Path path;
    drawingHMSymbolData.path_ = path;
    DrawingRenderGroup group;
    DrawingGroupInfo info{{1, 1}, {1, 1}};
    group.groupInfos = {info};
    drawingHMSymbolData.symbolInfo_.renderGroups = {group};
    Point locate;
    skiaCanvas->DrawSymbol(drawingHMSymbolData, locate);
    skiaCanvas->ImportSkCanvas(nullptr);
    skiaCanvas->DrawSymbol(drawingHMSymbolData, locate);
}

/**
 * @tc.name: DrawTextBlob001
 * @tc.desc: Test DrawTextBlob
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaCanvasTest, DrawTextBlob001, TestSize.Level1)
{
    auto skiaCanvas = std::make_shared<SkiaCanvas>();
    ASSERT_TRUE(skiaCanvas != nullptr);
    skiaCanvas->DrawTextBlob(nullptr, 0, 0);
    Font font;
    auto textBlob = TextBlob::MakeFromString("11", font, TextEncoding::UTF8);
    skiaCanvas->DrawTextBlob(textBlob.get(), 0, 0);
    skiaCanvas->ImportSkCanvas(nullptr);
    skiaCanvas->DrawTextBlob(nullptr, 0, 0);
}

/**
 * @tc.name: DrawPatch001
 * @tc.desc: Test DrawPatch
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaCanvasTest, DrawPatch001, TestSize.Level1)
{
    auto skiaCanvas = std::make_shared<SkiaCanvas>();
    ASSERT_TRUE(skiaCanvas != nullptr);
    float width = 100.0;
    float height = 100.0;
    float segmentWidthOne = width / 3.0;
    float segmentWidthTwo = width / 3.0 * 2.0;
    float segmentHeightOne = height / 3.0;
    float segmentHeightTwo = height / 3.0 * 2.0;
    Point ctrlPoints[12] = {
        // top edge control points
        {0.0f, 0.0f}, {segmentWidthOne, 0.0f}, {segmentWidthTwo, 0.0f}, {width, 0.0f},
        // right edge control points
        {width, segmentHeightOne}, {width, segmentHeightTwo},
        // bottom edge control points
        {width, height}, {segmentWidthTwo, height}, {segmentWidthOne, height}, {0.0f, height},
        // left edge control points
        {0.0f, segmentHeightTwo}, {0.0f, segmentHeightOne}
    };
    ColorQuad colors[4] = {0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000};
    Point texCoords[4] = {
        {0.0f, 0.0f}, {width, 0.0f}, {width, height}, {0.0f, height}
    };
    skiaCanvas->DrawPatch(ctrlPoints, colors, texCoords, BlendMode::COLOR_BURN);
    skiaCanvas->DrawPatch(nullptr, nullptr, nullptr, BlendMode::COLOR_BURN);
    skiaCanvas->ImportSkCanvas(nullptr);
    skiaCanvas->DrawPatch(nullptr, nullptr, nullptr, BlendMode::COLOR_BURN);
}

/**
 * @tc.name: DrawVertices001
 * @tc.desc: Test DrawVertices
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaCanvasTest, DrawVertices001, TestSize.Level1)
{
    auto skiaCanvas = std::make_shared<SkiaCanvas>();
    ASSERT_TRUE(skiaCanvas != nullptr);
    Vertices vertices;
    skiaCanvas->DrawVertices(vertices, BlendMode::COLOR_BURN);
    skiaCanvas->ImportSkCanvas(nullptr);
    skiaCanvas->DrawVertices(vertices, BlendMode::COLOR_BURN);
}

/**
 * @tc.name: DrawImageNine001
 * @tc.desc: Test DrawImageNine
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaCanvasTest, DrawImageNine001, TestSize.Level1)
{
    auto skiaCanvas = std::make_shared<SkiaCanvas>();
    ASSERT_TRUE(skiaCanvas != nullptr);
    Image image;
    RectI center;
    Rect dst;
    Brush brush;
    skiaCanvas->DrawImageNine(&image, center, dst, FilterMode::LINEAR, &brush);
    skiaCanvas->DrawImageNine(&image, center, dst, FilterMode::LINEAR, nullptr);
}

/**
 * @tc.name: DrawImageLattice001
 * @tc.desc: Test DrawImageLattice
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaCanvasTest, DrawImageLattice001, TestSize.Level1)
{
    auto skiaCanvas = std::make_shared<SkiaCanvas>();
    ASSERT_TRUE(skiaCanvas != nullptr);
    Image image;
    Lattice lattice;
    Rect dst;
    Brush brush;
    skiaCanvas->DrawImageLattice(&image, lattice, dst, FilterMode::LINEAR, &brush);
    skiaCanvas->DrawImageLattice(&image, lattice, dst, FilterMode::LINEAR, nullptr);
}

/**
 * @tc.name: OpCalculateBeforeTest001
 * @tc.desc: Test OpCalculateBefore
 * @tc.type: FUNC
 * @tc.require: I9B0X4
 */
HWTEST_F(SkiaCanvasTest, OpCalculateBeforeTest001, TestSize.Level1)
{
    SkiaCanvas skiaCanvas;
    Matrix matrix;
    ASSERT_EQ(skiaCanvas.OpCalculateBefore(matrix), true);
}

/**
 * @tc.name: OpCalculateAfterTest001
 * @tc.desc: Test OpCalculateAfter
 * @tc.type: FUNC
 * @tc.require: I9B0X4
 */
HWTEST_F(SkiaCanvasTest, OpCalculateAfterTest001, TestSize.Level1)
{
    SkiaCanvas skiaCanvas;
    Rect rect;
    auto handle = skiaCanvas.OpCalculateAfter(rect);
    ASSERT_EQ(handle, nullptr);
}

/**
 * @tc.name: OpCalculateAfterTest002
 * @tc.desc: Test OpCalculateAfter
 * @tc.type: FUNC
 * @tc.require: I9B0X4
 */
HWTEST_F(SkiaCanvasTest, OpCalculateAfterTest002, TestSize.Level1)
{
    SkiaCanvas skiaCanvas;
    Matrix matrix;
    skiaCanvas.OpCalculateBefore(matrix);
    Rect rect;
    auto handle = skiaCanvas.OpCalculateAfter(rect);
    ASSERT_EQ(handle, nullptr);
}

/**
 * @tc.name: GetOpsNumTest001
 * @tc.desc: Test GetOpsNum
 * @tc.type: FUNC
 * @tc.require: I9B0X4
 */
HWTEST_F(SkiaCanvasTest, GetOpsNumTest001, TestSize.Level1)
{
    auto skiaCanvas = std::make_shared<SkCanvas>();
    ASSERT_NE(skiaCanvas, nullptr);
    Matrix matrix;
    auto m = matrix.GetImpl<SkiaMatrix>();
    ASSERT_NE(m, nullptr);
    auto tmp = std::make_shared<SkiaCanvasAutoCache>(skiaCanvas.get());
    ASSERT_NE(tmp, nullptr);
    tmp->Init(m->ExportSkiaMatrix());
    ASSERT_EQ(tmp->GetOpsNum(), 0);
}

/**
 * @tc.name: GetOpsPercentTest001
 * @tc.desc: Test GetOpsPercent
 * @tc.type: FUNC
 * @tc.require: I9B0X4
 */
HWTEST_F(SkiaCanvasTest, GetOpsPercentTest001, TestSize.Level1)
{
    auto skiaCanvas = std::make_shared<SkCanvas>();
    ASSERT_NE(skiaCanvas, nullptr);
    Matrix matrix;
    auto m = matrix.GetImpl<SkiaMatrix>();
    ASSERT_NE(m, nullptr);
    auto tmp = std::make_shared<SkiaCanvasAutoCache>(skiaCanvas.get());
    ASSERT_NE(tmp, nullptr);
    tmp->Init(m->ExportSkiaMatrix());
    ASSERT_EQ(tmp->GetOpsPercent(), 0);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS