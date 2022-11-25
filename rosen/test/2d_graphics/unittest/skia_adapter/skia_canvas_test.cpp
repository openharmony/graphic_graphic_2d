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
    SkiaCanvas skiaCanvas;
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
    SkiaCanvas skiaCanvas;
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
    SkiaCanvas skiaCanvas;
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
    SkiaCanvas skiaCanvas;
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
    SkiaCanvas skiaCanvas;
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
    SkiaCanvas skiaCanvas;
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
    SkiaCanvas skiaCanvas;
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
    SkiaCanvas skiaCanvas;
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
    SkiaCanvas skiaCanvas;
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
    SkiaCanvas skiaCanvas;
    skiaCanvas.DrawPath(path);
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
    SkiaCanvas skiaCanvas;
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
    SkiaCanvas skiaCanvas;
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
    SkiaCanvas skiaCanvas;
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
    SkiaCanvas skiaCanvas;
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
 * @tc.name: ClipPath001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(SkiaCanvasTest, ClipPath001, TestSize.Level1)
{
    Path path;
    SkiaCanvas skiaCanvas;
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
    SkiaCanvas skiaCanvas;
    skiaCanvas.SetMatrix(matrix);
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
    SkiaCanvas skiaCanvas;
    skiaCanvas.ConcatMatrix(matrix);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS