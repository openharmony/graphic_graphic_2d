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
    Pen pen;
    SkiaCanvas skiaCanvas;
    skiaCanvas.AttachPen(pen);
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
    Pen pen;
    SkiaCanvas skiaCanvas;
    skiaCanvas.AttachPen(pen);
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
    Pen pen;
    SkiaCanvas skiaCanvas;
    skiaCanvas.AttachPen(pen);
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
    Pen pen;
    SkiaCanvas skiaCanvas;
    skiaCanvas.AttachPen(pen);
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
    Pen pen;
    SkiaCanvas skiaCanvas;
    skiaCanvas.AttachPen(pen);
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
    Pen pen;
    SkiaCanvas skiaCanvas;
    skiaCanvas.AttachPen(pen);
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
    Pen pen;
    SkiaCanvas skiaCanvas;
    skiaCanvas.AttachPen(pen);
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
    Pen pen;
    SkiaCanvas skiaCanvas;
    skiaCanvas.AttachPen(pen);
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
    Pen pen;
    SkiaCanvas skiaCanvas;
    skiaCanvas.AttachPen(pen);
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
    Pen pen;
    SkiaCanvas skiaCanvas;
    skiaCanvas.AttachPen(pen);
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
    Pen pen;
    SkiaCanvas skiaCanvas;
    skiaCanvas.AttachPen(pen);
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
    Pen pen;
    SkiaCanvas skiaCanvas;
    skiaCanvas.AttachPen(pen);
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
    Pen pen;
    SkiaCanvas skiaCanvas;
    skiaCanvas.AttachPen(pen);
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
    Pen pen;
    SkiaCanvas skiaCanvas;
    skiaCanvas.AttachPen(pen);
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
    skiaCanvas.ClipRoundRect(roundRect, ClipOp::DIFFERENCE);
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
    Pen pen;
    SkiaCanvas skiaCanvas;
    skiaCanvas.AttachPen(pen);
    skiaCanvas.ClipPath(path, ClipOp::DIFFERENCE);
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
    Pen pen;
    SkiaCanvas skiaCanvas;
    skiaCanvas.AttachPen(pen);
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
    Pen pen;
    SkiaCanvas skiaCanvas;
    skiaCanvas.AttachPen(pen);
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
    skiaCanvas.Rotate(0.5f);
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
    SkiaCanvas skiaCanvas;
    skiaCanvas.SaveLayer(rect, brush);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS