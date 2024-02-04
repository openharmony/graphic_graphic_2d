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

#include "pixel_map.h"
#include "recording/recording_canvas.h"
#include "draw/path.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
const int CANAS_WIDTH = 10;
const int CANAS_HEIGHT = 20;
const float RADIUS = 1.0f;
const float ANGLE = 90.f;
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
 * @tc.require: I7K0BS
 */
HWTEST_F(RecordingCanvasTest, DrawPoint001, TestSize.Level1)
{
    auto recordingCanvas1 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT);
    auto recordingCanvas2 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT, false);
    EXPECT_TRUE(recordingCanvas1 != nullptr && recordingCanvas2 != nullptr);

    Point point(CANAS_WIDTH, CANAS_HEIGHT);
    recordingCanvas1->DrawPoint(point);
    recordingCanvas2->DrawPoint(point);

    Brush brush(Color::COLOR_GREEN);
    recordingCanvas1->AttachBrush(brush);
    recordingCanvas2->AttachBrush(brush);
    recordingCanvas1->DrawPoint(point);
    recordingCanvas2->DrawPoint(point);

    Pen pen(Color::COLOR_GREEN);
    recordingCanvas1->AttachPen(pen);
    recordingCanvas2->AttachPen(pen);
    recordingCanvas1->DrawPoint(point);
    recordingCanvas2->DrawPoint(point);

    recordingCanvas1->DetachBrush();
    recordingCanvas2->DetachBrush();
    recordingCanvas1->DrawPoint(point);
    recordingCanvas2->DrawPoint(point);

    auto drawCmdList1 = recordingCanvas1->GetDrawCmdList();
    auto drawCmdList2 = recordingCanvas2->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList1 != nullptr && drawCmdList2 != nullptr);
    Canvas canvas;
    drawCmdList1->Playback(canvas);
    drawCmdList2->Playback(canvas);
}

/**
 * @tc.name: DrawPoints001
 * @tc.desc: Test the playback of the DrawPoints function.
 * @tc.type: FUNC
 * @tc.require: I7K0BS
 */
HWTEST_F(RecordingCanvasTest, DrawPoints001, TestSize.Level1)
{
    auto recordingCanvas1 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT);
    auto recordingCanvas2 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT, false);
    EXPECT_TRUE(recordingCanvas1 != nullptr && recordingCanvas2 != nullptr);

    std::vector<Point> points = {{CANAS_WIDTH, CANAS_WIDTH}, {CANAS_HEIGHT, CANAS_HEIGHT}};
    recordingCanvas1->DrawPoints(PointMode::LINES_POINTMODE, points.size(), points.data());
    recordingCanvas2->DrawPoints(PointMode::LINES_POINTMODE, points.size(), points.data());

    Brush brush(Color::COLOR_GREEN);
    recordingCanvas1->AttachBrush(brush);
    recordingCanvas2->AttachBrush(brush);
    recordingCanvas1->DrawPoints(PointMode::LINES_POINTMODE, points.size(), points.data());
    recordingCanvas2->DrawPoints(PointMode::LINES_POINTMODE, points.size(), points.data());

    Pen pen(Color::COLOR_GREEN);
    recordingCanvas1->AttachPen(pen);
    recordingCanvas2->AttachPen(pen);
    recordingCanvas1->DrawPoints(PointMode::LINES_POINTMODE, points.size(), points.data());
    recordingCanvas2->DrawPoints(PointMode::LINES_POINTMODE, points.size(), points.data());

    recordingCanvas1->DetachBrush();
    recordingCanvas2->DetachBrush();
    recordingCanvas1->DrawPoints(PointMode::LINES_POINTMODE, points.size(), points.data());
    recordingCanvas2->DrawPoints(PointMode::LINES_POINTMODE, points.size(), points.data());
    
    auto drawCmdList1 = recordingCanvas1->GetDrawCmdList();
    auto drawCmdList2 = recordingCanvas2->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList1 != nullptr && drawCmdList2 != nullptr);
    Canvas canvas;
    drawCmdList1->Playback(canvas);
    drawCmdList2->Playback(canvas);
}

/**
 * @tc.name: DrawLine001
 * @tc.desc: Test the playback of the DrawLine function.
 * @tc.type: FUNC
 * @tc.require: I7K0BS
 */
HWTEST_F(RecordingCanvasTest, DrawLine001, TestSize.Level1)
{
    auto recordingCanvas1 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT);
    auto recordingCanvas2 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT, false);
    EXPECT_TRUE(recordingCanvas1 != nullptr && recordingCanvas2 != nullptr);

    Point startPoint(CANAS_WIDTH, CANAS_WIDTH);
    Point endPoint(CANAS_HEIGHT, CANAS_HEIGHT);
    recordingCanvas1->DrawLine(startPoint, endPoint);
    recordingCanvas2->DrawLine(startPoint, endPoint);

    Brush brush(Color::COLOR_GREEN);
    recordingCanvas1->AttachBrush(brush);
    recordingCanvas2->AttachBrush(brush);
    recordingCanvas1->DrawLine(startPoint, endPoint);
    recordingCanvas2->DrawLine(startPoint, endPoint);

    Pen pen(Color::COLOR_GREEN);
    recordingCanvas1->AttachPen(pen);
    recordingCanvas2->AttachPen(pen);
    recordingCanvas1->DrawLine(startPoint, endPoint);
    recordingCanvas2->DrawLine(startPoint, endPoint);

    recordingCanvas1->DetachBrush();
    recordingCanvas2->DetachBrush();
    recordingCanvas1->DrawLine(startPoint, endPoint);
    recordingCanvas2->DrawLine(startPoint, endPoint);

    auto drawCmdList1 = recordingCanvas1->GetDrawCmdList();
    auto drawCmdList2 = recordingCanvas2->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList1 != nullptr && drawCmdList2 != nullptr);
    Canvas canvas;
    drawCmdList1->Playback(canvas);
    drawCmdList2->Playback(canvas);
}

/**
 * @tc.name: DrawRect001
 * @tc.desc: Test the playback of the DrawRect function.
 * @tc.type: FUNC
 * @tc.require: I7K0BS
 */
HWTEST_F(RecordingCanvasTest, DrawRect001, TestSize.Level1)
{
    auto recordingCanvas1 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT);
    auto recordingCanvas2 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT, false);
    EXPECT_TRUE(recordingCanvas1 != nullptr && recordingCanvas2 != nullptr);

    Rect rect(0.0f, 0.0f, CANAS_WIDTH, CANAS_HEIGHT);
    recordingCanvas1->DrawRect(rect);
    recordingCanvas2->DrawRect(rect);

    Brush brush(Color::COLOR_GREEN);
    recordingCanvas1->AttachBrush(brush);
    recordingCanvas2->AttachBrush(brush);
    recordingCanvas1->DrawRect(rect);
    recordingCanvas2->DrawRect(rect);

    Pen pen(Color::COLOR_GREEN);
    recordingCanvas1->AttachPen(pen);
    recordingCanvas2->AttachPen(pen);
    recordingCanvas1->DrawRect(rect);
    recordingCanvas2->DrawRect(rect);

    recordingCanvas1->DetachBrush();
    recordingCanvas2->DetachBrush();
    recordingCanvas1->DrawRect(rect);
    recordingCanvas2->DrawRect(rect);

    auto drawCmdList1 = recordingCanvas1->GetDrawCmdList();
    auto drawCmdList2 = recordingCanvas2->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList1 != nullptr && drawCmdList2 != nullptr);
    Canvas canvas;
    drawCmdList1->Playback(canvas);
    drawCmdList2->Playback(canvas);
}

/**
 * @tc.name: DrawRoundRect001
 * @tc.desc: Test the playback of the DrawRoundRect function.
 * @tc.type: FUNC
 * @tc.require: I7K0BS
 */
HWTEST_F(RecordingCanvasTest, DrawRoundRect001, TestSize.Level1)
{
    auto recordingCanvas1 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT);
    auto recordingCanvas2 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT, false);
    EXPECT_TRUE(recordingCanvas1 != nullptr && recordingCanvas2 != nullptr);
    Rect rect(0.0f, 0.0f, CANAS_WIDTH, CANAS_HEIGHT);
    RoundRect roundRect(rect, RADIUS, RADIUS);
    recordingCanvas1->DrawRoundRect(roundRect);
    recordingCanvas2->DrawRoundRect(roundRect);

    Brush brush(Color::COLOR_GREEN);
    recordingCanvas1->AttachBrush(brush);
    recordingCanvas2->AttachBrush(brush);
    recordingCanvas1->DrawRoundRect(roundRect);
    recordingCanvas2->DrawRoundRect(roundRect);

    Pen pen(Color::COLOR_GREEN);
    recordingCanvas1->AttachPen(pen);
    recordingCanvas2->AttachPen(pen);
    recordingCanvas1->DrawRoundRect(roundRect);
    recordingCanvas2->DrawRoundRect(roundRect);

    recordingCanvas1->DetachBrush();
    recordingCanvas2->DetachBrush();
    recordingCanvas1->DrawRoundRect(roundRect);
    recordingCanvas2->DrawRoundRect(roundRect);

    auto drawCmdList1 = recordingCanvas1->GetDrawCmdList();
    auto drawCmdList2 = recordingCanvas2->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList1 != nullptr && drawCmdList2 != nullptr);
    Canvas canvas;
    drawCmdList1->Playback(canvas);
    drawCmdList2->Playback(canvas);
}

/**
 * @tc.name: DrawNestedRoundRect001
 * @tc.desc: Test the playback of the DrawNestedRoundRect function.
 * @tc.type: FUNC
 * @tc.require: I7K0BS
 */
HWTEST_F(RecordingCanvasTest, DrawNestedRoundRect001, TestSize.Level1)
{
    auto recordingCanvas1 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT);
    auto recordingCanvas2 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT, false);
    EXPECT_TRUE(recordingCanvas1 != nullptr && recordingCanvas2 != nullptr);
    Rect rect1(0.0f, 0.0f, CANAS_HEIGHT, CANAS_HEIGHT);
    RoundRect roundRect1(rect1, RADIUS, RADIUS);
    Rect rect2(0.0f, 0.0f, CANAS_WIDTH, CANAS_WIDTH);
    RoundRect roundRect2(rect2, RADIUS, RADIUS);
    recordingCanvas1->DrawNestedRoundRect(roundRect1, roundRect2);
    recordingCanvas2->DrawNestedRoundRect(roundRect1, roundRect2);

    Brush brush(Color::COLOR_GREEN);
    recordingCanvas1->AttachBrush(brush);
    recordingCanvas2->AttachBrush(brush);
    recordingCanvas1->DrawNestedRoundRect(roundRect1, roundRect2);
    recordingCanvas2->DrawNestedRoundRect(roundRect1, roundRect2);

    Pen pen(Color::COLOR_GREEN);
    recordingCanvas1->AttachPen(pen);
    recordingCanvas2->AttachPen(pen);
    recordingCanvas1->DrawNestedRoundRect(roundRect1, roundRect2);
    recordingCanvas2->DrawNestedRoundRect(roundRect1, roundRect2);

    recordingCanvas1->DetachBrush();
    recordingCanvas2->DetachBrush();
    recordingCanvas1->DrawNestedRoundRect(roundRect1, roundRect2);
    recordingCanvas2->DrawNestedRoundRect(roundRect1, roundRect2);

    auto drawCmdList1 = recordingCanvas1->GetDrawCmdList();
    auto drawCmdList2 = recordingCanvas2->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList1 != nullptr && drawCmdList2 != nullptr);
    Canvas canvas;
    drawCmdList1->Playback(canvas);
    drawCmdList2->Playback(canvas);
}

/**
 * @tc.name: DrawArc001
 * @tc.desc: Test the playback of the DrawArc function.
 * @tc.type: FUNC
 * @tc.require: I7K0BS
 */
HWTEST_F(RecordingCanvasTest, DrawArc001, TestSize.Level1)
{
    auto recordingCanvas1 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT);
    auto recordingCanvas2 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT, false);
    EXPECT_TRUE(recordingCanvas1 != nullptr && recordingCanvas2 != nullptr);
    Rect rect(0.0f, 0.0f, CANAS_WIDTH, CANAS_HEIGHT);
    recordingCanvas1->DrawArc(rect, 0.0f, ANGLE);
    recordingCanvas2->DrawArc(rect, 0.0f, ANGLE);

    Brush brush(Color::COLOR_GREEN);
    recordingCanvas1->AttachBrush(brush);
    recordingCanvas2->AttachBrush(brush);
    recordingCanvas1->DrawArc(rect, 0.0f, ANGLE);
    recordingCanvas2->DrawArc(rect, 0.0f, ANGLE);

    Pen pen(Color::COLOR_GREEN);
    recordingCanvas1->AttachPen(pen);
    recordingCanvas2->AttachPen(pen);
    recordingCanvas1->DrawArc(rect, 0.0f, ANGLE);
    recordingCanvas2->DrawArc(rect, 0.0f, ANGLE);

    recordingCanvas1->DetachBrush();
    recordingCanvas2->DetachBrush();
    recordingCanvas1->DrawArc(rect, 0.0f, ANGLE);
    recordingCanvas2->DrawArc(rect, 0.0f, ANGLE);

    auto drawCmdList1 = recordingCanvas1->GetDrawCmdList();
    auto drawCmdList2 = recordingCanvas2->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList1 != nullptr && drawCmdList2 != nullptr);
    Canvas canvas;
    drawCmdList1->Playback(canvas);
    drawCmdList2->Playback(canvas);
}

/**
 * @tc.name: DrawPie001
 * @tc.desc: Test the playback of the DrawPie function.
 * @tc.type: FUNC
 * @tc.require: I7K0BS
 */
HWTEST_F(RecordingCanvasTest, DrawPie001, TestSize.Level1)
{
    auto recordingCanvas1 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT);
    auto recordingCanvas2 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT, false);
    EXPECT_TRUE(recordingCanvas1 != nullptr && recordingCanvas2 != nullptr);
    Rect rect(0.0f, 0.0f, CANAS_WIDTH, CANAS_HEIGHT);
    recordingCanvas1->DrawPie(rect, 0.0f, ANGLE);
    recordingCanvas2->DrawPie(rect, 0.0f, ANGLE);

    Brush brush(Color::COLOR_GREEN);
    recordingCanvas1->AttachBrush(brush);
    recordingCanvas2->AttachBrush(brush);
    recordingCanvas1->DrawPie(rect, 0.0f, ANGLE);
    recordingCanvas2->DrawPie(rect, 0.0f, ANGLE);

    Pen pen(Color::COLOR_GREEN);
    recordingCanvas1->AttachPen(pen);
    recordingCanvas2->AttachPen(pen);
    recordingCanvas1->DrawPie(rect, 0.0f, ANGLE);
    recordingCanvas2->DrawPie(rect, 0.0f, ANGLE);

    recordingCanvas1->DetachBrush();
    recordingCanvas2->DetachBrush();
    recordingCanvas1->DrawPie(rect, 0.0f, ANGLE);
    recordingCanvas2->DrawPie(rect, 0.0f, ANGLE);

    auto drawCmdList1 = recordingCanvas1->GetDrawCmdList();
    auto drawCmdList2 = recordingCanvas2->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList1 != nullptr && drawCmdList2 != nullptr);
    Canvas canvas;
    drawCmdList1->Playback(canvas);
    drawCmdList2->Playback(canvas);
}

/**
 * @tc.name: DrawOval001
 * @tc.desc: Test the playback of the DrawOval function.
 * @tc.type: FUNC
 * @tc.require: I7K0BS
 */
HWTEST_F(RecordingCanvasTest, DrawOval001, TestSize.Level1)
{
    auto recordingCanvas1 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT);
    auto recordingCanvas2 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT, false);
    EXPECT_TRUE(recordingCanvas1 != nullptr && recordingCanvas2 != nullptr);
    Rect rect(0.0f, 0.0f, CANAS_WIDTH, CANAS_HEIGHT);
    recordingCanvas1->DrawOval(rect);
    recordingCanvas2->DrawOval(rect);

    Brush brush(Color::COLOR_GREEN);
    recordingCanvas1->AttachBrush(brush);
    recordingCanvas2->AttachBrush(brush);
    recordingCanvas1->DrawOval(rect);
    recordingCanvas2->DrawOval(rect);

    Pen pen(Color::COLOR_GREEN);
    recordingCanvas1->AttachPen(pen);
    recordingCanvas2->AttachPen(pen);
    recordingCanvas1->DrawOval(rect);
    recordingCanvas2->DrawOval(rect);

    recordingCanvas1->DetachBrush();
    recordingCanvas2->DetachBrush();
    recordingCanvas1->DrawOval(rect);
    recordingCanvas2->DrawOval(rect);

    auto drawCmdList1 = recordingCanvas1->GetDrawCmdList();
    auto drawCmdList2 = recordingCanvas2->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList1 != nullptr && drawCmdList2 != nullptr);
    Canvas canvas;
    drawCmdList1->Playback(canvas);
    drawCmdList2->Playback(canvas);
}

/**
 * @tc.name: DrawCircle001
 * @tc.desc: Test the playback of the DrawCircle function.
 * @tc.type: FUNC
 * @tc.require: I7K0BS
 */
HWTEST_F(RecordingCanvasTest, DrawCircle001, TestSize.Level1)
{
    auto recordingCanvas1 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT);
    auto recordingCanvas2 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT, false);
    EXPECT_TRUE(recordingCanvas1 != nullptr && recordingCanvas2 != nullptr);
    Point centerpoint(CANAS_WIDTH, CANAS_HEIGHT);
    recordingCanvas1->DrawCircle(centerpoint, CANAS_WIDTH);
    recordingCanvas2->DrawCircle(centerpoint, CANAS_WIDTH);

    Brush brush(Color::COLOR_GREEN);
    recordingCanvas1->AttachBrush(brush);
    recordingCanvas2->AttachBrush(brush);
    recordingCanvas1->DrawCircle(centerpoint, CANAS_WIDTH);
    recordingCanvas2->DrawCircle(centerpoint, CANAS_WIDTH);

    Pen pen(Color::COLOR_GREEN);
    recordingCanvas1->AttachPen(pen);
    recordingCanvas2->AttachPen(pen);
    recordingCanvas1->DrawCircle(centerpoint, CANAS_WIDTH);
    recordingCanvas2->DrawCircle(centerpoint, CANAS_WIDTH);

    recordingCanvas1->DetachBrush();
    recordingCanvas2->DetachBrush();
    recordingCanvas1->DrawCircle(centerpoint, CANAS_WIDTH);
    recordingCanvas2->DrawCircle(centerpoint, CANAS_WIDTH);

    auto drawCmdList1 = recordingCanvas1->GetDrawCmdList();
    auto drawCmdList2 = recordingCanvas2->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList1 != nullptr && drawCmdList2 != nullptr);
    Canvas canvas;
    drawCmdList1->Playback(canvas);
    drawCmdList2->Playback(canvas);
}

/**
 * @tc.name: DrawPath001
 * @tc.desc: Test the playback of the DrawPath function.
 * @tc.type: FUNC
 * @tc.require: I7K0BS
 */
HWTEST_F(RecordingCanvasTest, DrawPath001, TestSize.Level1)
{
    auto recordingCanvas1 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT);
    auto recordingCanvas2 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT, false);
    EXPECT_TRUE(recordingCanvas1 != nullptr && recordingCanvas2 != nullptr);
    Path path;
    path.AddRect(Rect(0, 0, CANAS_WIDTH, CANAS_HEIGHT));
    recordingCanvas1->DrawPath(path);
    recordingCanvas2->DrawPath(path);

    Brush brush(Color::COLOR_GREEN);
    recordingCanvas1->AttachBrush(brush);
    recordingCanvas2->AttachBrush(brush);
    recordingCanvas1->DrawPath(path);
    recordingCanvas2->DrawPath(path);

    Pen pen(Color::COLOR_GREEN);
    recordingCanvas1->AttachPen(pen);
    recordingCanvas2->AttachPen(pen);
    recordingCanvas1->DrawPath(path);
    recordingCanvas2->DrawPath(path);

    recordingCanvas1->DetachBrush();
    recordingCanvas2->DetachBrush();
    recordingCanvas1->DrawPath(path);
    recordingCanvas2->DrawPath(path);

    auto drawCmdList1 = recordingCanvas1->GetDrawCmdList();
    auto drawCmdList2 = recordingCanvas2->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList1 != nullptr && drawCmdList2 != nullptr);
    Canvas canvas;
    drawCmdList1->Playback(canvas);
    drawCmdList2->Playback(canvas);
}

/**
 * @tc.name: DrawBackground001
 * @tc.desc: Test the playback of the DrawBackground function.
 * @tc.type: FUNC
 * @tc.require: I7K0BS
 */
HWTEST_F(RecordingCanvasTest, DrawBackground001, TestSize.Level1)
{
    auto recordingCanvas1 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT);
    auto recordingCanvas2 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT, false);
    EXPECT_TRUE(recordingCanvas1 != nullptr && recordingCanvas2 != nullptr);
    Brush brush(Color::COLOR_RED);
    recordingCanvas1->DrawBackground(brush);
    recordingCanvas2->DrawBackground(brush);
    auto drawCmdList1 = recordingCanvas1->GetDrawCmdList();
    auto drawCmdList2 = recordingCanvas2->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList1 != nullptr && drawCmdList2 != nullptr);
    Canvas canvas;
    drawCmdList1->Playback(canvas);
    drawCmdList2->Playback(canvas);
}

/**
 * @tc.name: DrawShadow001
 * @tc.desc: Test the playback of the DrawShadow function.
 * @tc.type: FUNC
 * @tc.require: I7K0BS
 */
HWTEST_F(RecordingCanvasTest, DrawShadow001, TestSize.Level1)
{
    auto recordingCanvas1 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT);
    auto recordingCanvas2 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT, false);
    EXPECT_TRUE(recordingCanvas1 != nullptr && recordingCanvas2 != nullptr);
    Path path;
    Point3 planeParams(1.0f, 0.0f, 0.0f);
    Point3 devLightPos(1.0f, 1.0f, 1.0f);
    recordingCanvas1->DrawShadow(
        path, planeParams, devLightPos, 1.0f, Color::COLOR_BLACK, Color::COLOR_BLUE, ShadowFlags::NONE);
    recordingCanvas2->DrawShadow(
        path, planeParams, devLightPos, 1.0f, Color::COLOR_BLACK, Color::COLOR_BLUE, ShadowFlags::NONE);
    auto drawCmdList1 = recordingCanvas1->GetDrawCmdList();
    auto drawCmdList2 = recordingCanvas2->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList1 != nullptr && drawCmdList2 != nullptr);
    Canvas canvas;
    drawCmdList1->Playback(canvas);
    drawCmdList2->Playback(canvas);
}

/**
 * @tc.name: DrawRegion001
 * @tc.desc: Test the playback of the DrawRegion function.
 * @tc.type: FUNC
 * @tc.require: I7K0BS
 */
HWTEST_F(RecordingCanvasTest, DrawRegion001, TestSize.Level1)
{
    auto recordingCanvas1 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT);
    auto recordingCanvas2 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT, false);
    EXPECT_TRUE(recordingCanvas1 != nullptr && recordingCanvas2 != nullptr);
    Region region;
    region.SetRect(RectI(0, 0, CANAS_WIDTH, CANAS_HEIGHT));
    recordingCanvas1->DrawRegion(region);
    recordingCanvas2->DrawRegion(region);

    Brush brush(Color::COLOR_GREEN);
    recordingCanvas1->AttachBrush(brush);
    recordingCanvas2->AttachBrush(brush);
    recordingCanvas1->DrawRegion(region);
    recordingCanvas2->DrawRegion(region);

    Pen pen(Color::COLOR_GREEN);
    recordingCanvas1->AttachPen(pen);
    recordingCanvas2->AttachPen(pen);
    recordingCanvas1->DrawRegion(region);
    recordingCanvas2->DrawRegion(region);

    recordingCanvas1->DetachBrush();
    recordingCanvas2->DetachBrush();
    recordingCanvas1->DrawRegion(region);
    recordingCanvas2->DrawRegion(region);

    auto drawCmdList1 = recordingCanvas1->GetDrawCmdList();
    auto drawCmdList2 = recordingCanvas2->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList1 != nullptr && drawCmdList2 != nullptr);
    Canvas canvas;
    drawCmdList1->Playback(canvas);
    drawCmdList2->Playback(canvas);
}

/**
 * @tc.name: DrawColor001
 * @tc.desc: Test the playback of the DrawColor function.
 * @tc.type: FUNC
 * @tc.require: I7K0BS
 */
HWTEST_F(RecordingCanvasTest, DrawColor001, TestSize.Level1)
{
    auto recordingCanvas1 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT);
    auto recordingCanvas2 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT, false);
    EXPECT_TRUE(recordingCanvas1 != nullptr && recordingCanvas2 != nullptr);
    recordingCanvas1->DrawColor(Color::COLOR_RED);
    recordingCanvas2->DrawColor(Color::COLOR_RED);
    auto drawCmdList1 = recordingCanvas1->GetDrawCmdList();
    auto drawCmdList2 = recordingCanvas2->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList1 != nullptr && drawCmdList2 != nullptr);
    Canvas canvas;
    drawCmdList1->Playback(canvas);
    drawCmdList2->Playback(canvas);
}

/**
 * @tc.name: DrawBitmap001
 * @tc.desc: Test the playback of the DrawBitmap function.
 * @tc.type: FUNC
 * @tc.require: I7K0BS
 */
HWTEST_F(RecordingCanvasTest, DrawBitmap001, TestSize.Level1)
{
    auto recordingCanvas1 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT);
    auto recordingCanvas2 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT, false);
    EXPECT_TRUE(recordingCanvas1 != nullptr && recordingCanvas2 != nullptr);
    Bitmap bitmap;
    BitmapFormat bitmapFormat { COLORTYPE_RGBA_8888, ALPHATYPE_OPAQUE };
    bitmap.Build(CANAS_WIDTH, CANAS_WIDTH, bitmapFormat);
    recordingCanvas1->DrawBitmap(bitmap, CANAS_WIDTH, CANAS_WIDTH);
    recordingCanvas2->DrawBitmap(bitmap, CANAS_WIDTH, CANAS_WIDTH);

    Brush brush(Color::COLOR_GREEN);
    recordingCanvas1->AttachBrush(brush);
    recordingCanvas2->AttachBrush(brush);
    recordingCanvas1->DrawBitmap(bitmap, CANAS_WIDTH, CANAS_WIDTH);
    recordingCanvas2->DrawBitmap(bitmap, CANAS_WIDTH, CANAS_WIDTH);

    Pen pen(Color::COLOR_GREEN);
    recordingCanvas1->AttachPen(pen);
    recordingCanvas2->AttachPen(pen);
    recordingCanvas1->DrawBitmap(bitmap, CANAS_WIDTH, CANAS_WIDTH);
    recordingCanvas2->DrawBitmap(bitmap, CANAS_WIDTH, CANAS_WIDTH);

    recordingCanvas1->DetachBrush();
    recordingCanvas2->DetachBrush();
    recordingCanvas1->DrawBitmap(bitmap, CANAS_WIDTH, CANAS_WIDTH);
    recordingCanvas2->DrawBitmap(bitmap, CANAS_WIDTH, CANAS_WIDTH);

    auto drawCmdList1 = recordingCanvas1->GetDrawCmdList();
    auto drawCmdList2 = recordingCanvas2->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList1 != nullptr && drawCmdList2 != nullptr);
    Canvas canvas;
    drawCmdList1->Playback(canvas);
    drawCmdList2->Playback(canvas);
}

/**
 * @tc.name: DrawImage001
 * @tc.desc: Test the playback of the DrawImage function.
 * @tc.type: FUNC
 * @tc.require: I7K0BS
 */
HWTEST_F(RecordingCanvasTest, DrawImage001, TestSize.Level1)
{
    auto recordingCanvas1 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT);
    auto recordingCanvas2 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT, false);
    EXPECT_TRUE(recordingCanvas1 != nullptr && recordingCanvas2 != nullptr);
    Bitmap bitmap;
    BitmapFormat bitmapFormat { COLORTYPE_RGBA_8888, ALPHATYPE_OPAQUE };
    bitmap.Build(CANAS_HEIGHT, CANAS_HEIGHT, bitmapFormat);
    Image image;
    image.BuildFromBitmap(bitmap);
    SamplingOptions samplingOptions;
    recordingCanvas1->DrawImage(image, CANAS_WIDTH, CANAS_WIDTH, samplingOptions);
    recordingCanvas2->DrawImage(image, CANAS_WIDTH, CANAS_WIDTH, samplingOptions);

    Brush brush(Color::COLOR_GREEN);
    recordingCanvas1->AttachBrush(brush);
    recordingCanvas2->AttachBrush(brush);
    recordingCanvas1->DrawImage(image, CANAS_WIDTH, CANAS_WIDTH, samplingOptions);
    recordingCanvas2->DrawImage(image, CANAS_WIDTH, CANAS_WIDTH, samplingOptions);

    Pen pen(Color::COLOR_GREEN);
    recordingCanvas1->AttachPen(pen);
    recordingCanvas2->AttachPen(pen);
    recordingCanvas1->DrawImage(image, CANAS_WIDTH, CANAS_WIDTH, samplingOptions);
    recordingCanvas2->DrawImage(image, CANAS_WIDTH, CANAS_WIDTH, samplingOptions);

    recordingCanvas1->DetachBrush();
    recordingCanvas2->DetachBrush();
    recordingCanvas1->DrawImage(image, CANAS_WIDTH, CANAS_WIDTH, samplingOptions);
    recordingCanvas2->DrawImage(image, CANAS_WIDTH, CANAS_WIDTH, samplingOptions);

    auto drawCmdList1 = recordingCanvas1->GetDrawCmdList();
    auto drawCmdList2 = recordingCanvas2->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList1 != nullptr && drawCmdList2 != nullptr);
    Canvas canvas;
    drawCmdList1->Playback(canvas);
    drawCmdList2->Playback(canvas);
}

/**
 * @tc.name: DrawImage002
 * @tc.desc: Test the playback of the DrawImage function.
 * @tc.type: FUNC
 * @tc.require: I7OAIR
 */
HWTEST_F(RecordingCanvasTest, DrawImage002, TestSize.Level1)
{
    auto recordingCanvas1 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT);
    auto recordingCanvas2 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT, false);
    EXPECT_TRUE(recordingCanvas1 != nullptr && recordingCanvas2 != nullptr);
    Bitmap bitmap;
    BitmapFormat bitmapFormat { COLORTYPE_RGBA_8888, ALPHATYPE_OPAQUE };
    bitmap.Build(CANAS_HEIGHT, CANAS_HEIGHT, bitmapFormat);
    auto image = std::make_shared<Image>();
    image->BuildFromBitmap(bitmap);
    AdaptiveImageInfo rsImageInfo;
    SamplingOptions smapling;
    recordingCanvas1->DrawImage(image, nullptr, rsImageInfo, smapling);
    recordingCanvas2->DrawImage(image, nullptr, rsImageInfo, smapling);

    Brush brush(Color::COLOR_GREEN);
    recordingCanvas1->AttachBrush(brush);
    recordingCanvas2->AttachBrush(brush);
    recordingCanvas1->DrawImage(image, nullptr, rsImageInfo, smapling);
    recordingCanvas2->DrawImage(image, nullptr, rsImageInfo, smapling);

    Pen pen(Color::COLOR_GREEN);
    recordingCanvas1->AttachPen(pen);
    recordingCanvas2->AttachPen(pen);
    recordingCanvas1->DrawImage(image, nullptr, rsImageInfo, smapling);
    recordingCanvas2->DrawImage(image, nullptr, rsImageInfo, smapling);

    recordingCanvas1->DetachBrush();
    recordingCanvas2->DetachBrush();
    recordingCanvas1->DrawImage(image, nullptr, rsImageInfo, smapling);
    recordingCanvas2->DrawImage(image, nullptr, rsImageInfo, smapling);

    auto drawCmdList1 = recordingCanvas1->GetDrawCmdList();
    auto drawCmdList2 = recordingCanvas2->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList1 != nullptr && drawCmdList2 != nullptr);
    Canvas canvas;
    drawCmdList1->Playback(canvas);
    drawCmdList2->Playback(canvas);
}

/**
 * @tc.name: DrawImage003
 * @tc.desc: Test the playback of the DrawImage function.
 * @tc.type: FUNC
 * @tc.require: I7OAIR
 */
HWTEST_F(RecordingCanvasTest, DrawImage003, TestSize.Level1)
{
    auto recordingCanvas1 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT);
    auto recordingCanvas2 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT, false);
    EXPECT_TRUE(recordingCanvas1 != nullptr && recordingCanvas2 != nullptr);
    auto data = std::make_shared<Data>();
    AdaptiveImageInfo rsImageInfo;
    SamplingOptions smapling;
    recordingCanvas1->DrawImage(nullptr, data, rsImageInfo, smapling);
    recordingCanvas2->DrawImage(nullptr, data, rsImageInfo, smapling);

    Brush brush(Color::COLOR_GREEN);
    recordingCanvas1->AttachBrush(brush);
    recordingCanvas2->AttachBrush(brush);
    recordingCanvas1->DrawImage(nullptr, data, rsImageInfo, smapling);
    recordingCanvas2->DrawImage(nullptr, data, rsImageInfo, smapling);

    Pen pen(Color::COLOR_GREEN);
    recordingCanvas1->AttachPen(pen);
    recordingCanvas2->AttachPen(pen);
    recordingCanvas1->DrawImage(nullptr, data, rsImageInfo, smapling);
    recordingCanvas2->DrawImage(nullptr, data, rsImageInfo, smapling);

    recordingCanvas1->DetachBrush();
    recordingCanvas2->DetachBrush();
    recordingCanvas1->DrawImage(nullptr, data, rsImageInfo, smapling);
    recordingCanvas2->DrawImage(nullptr, data, rsImageInfo, smapling);

    auto drawCmdList1 = recordingCanvas1->GetDrawCmdList();
    auto drawCmdList2 = recordingCanvas2->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList1 != nullptr && drawCmdList2 != nullptr);
    Canvas canvas;
    drawCmdList1->Playback(canvas);
    drawCmdList2->Playback(canvas);
}

/**
 * @tc.name: DrawImage004
 * @tc.desc: Test the playback of the DrawImage function.
 * @tc.type: FUNC
 * @tc.require: I7OAIR
 */
HWTEST_F(RecordingCanvasTest, DrawImage004, TestSize.Level1)
{
    auto recordingCanvas = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT);
    EXPECT_TRUE(recordingCanvas != nullptr);
    AdaptiveImageInfo rsImageInfo;
    SamplingOptions smapling;
    recordingCanvas->DrawImage(nullptr, nullptr, rsImageInfo, smapling);
    auto drawCmdList = recordingCanvas->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList != nullptr);
    Canvas canvas;
    drawCmdList->Playback(canvas);
}

/**
 * @tc.name: DrawImageRect001
 * @tc.desc: Test the playback of the DrawImageRect function.
 * @tc.type: FUNC
 * @tc.require: I7K0BS
 */
HWTEST_F(RecordingCanvasTest, DrawImageRect001, TestSize.Level1)
{
    auto recordingCanvas1 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT);
    auto recordingCanvas2 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT, false);
    EXPECT_TRUE(recordingCanvas1 != nullptr && recordingCanvas2 != nullptr);
    Bitmap bitmap;
    BitmapFormat bitmapFormat { COLORTYPE_RGBA_8888, ALPHATYPE_OPAQUE };
    bitmap.Build(CANAS_HEIGHT, CANAS_HEIGHT, bitmapFormat);
    Image image;
    image.BuildFromBitmap(bitmap);
    Rect dstRect(0.0f, 0.0f, CANAS_WIDTH, CANAS_HEIGHT);
    SamplingOptions samplingOptions;
    recordingCanvas1->DrawImageRect(image, dstRect, samplingOptions);
    recordingCanvas2->DrawImageRect(image, dstRect, samplingOptions);

    Brush brush(Color::COLOR_GREEN);
    recordingCanvas1->AttachBrush(brush);
    recordingCanvas2->AttachBrush(brush);
    recordingCanvas1->DrawImageRect(image, dstRect, samplingOptions);
    recordingCanvas2->DrawImageRect(image, dstRect, samplingOptions);

    Pen pen(Color::COLOR_GREEN);
    recordingCanvas1->AttachPen(pen);
    recordingCanvas2->AttachPen(pen);
    recordingCanvas1->DrawImageRect(image, dstRect, samplingOptions);
    recordingCanvas2->DrawImageRect(image, dstRect, samplingOptions);

    recordingCanvas1->DetachBrush();
    recordingCanvas2->DetachBrush();
    recordingCanvas1->DrawImageRect(image, dstRect, samplingOptions);
    recordingCanvas2->DrawImageRect(image, dstRect, samplingOptions);

    auto drawCmdList1 = recordingCanvas1->GetDrawCmdList();
    auto drawCmdList2 = recordingCanvas2->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList1 != nullptr && drawCmdList2 != nullptr);
    Canvas canvas;
    drawCmdList1->Playback(canvas);
    drawCmdList2->Playback(canvas);
}

/**
 * @tc.name: DrawImageRect002
 * @tc.desc: Test the playback of the DrawImageRect function.
 * @tc.type: FUNC
 * @tc.require: I7K0BS
 */
HWTEST_F(RecordingCanvasTest, DrawImageRect002, TestSize.Level1)
{
    auto recordingCanvas1 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT);
    auto recordingCanvas2 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT, false);
    EXPECT_TRUE(recordingCanvas1 != nullptr && recordingCanvas2 != nullptr);
    Bitmap bitmap;
    BitmapFormat bitmapFormat { COLORTYPE_RGBA_8888, ALPHATYPE_OPAQUE };
    bitmap.Build(CANAS_WIDTH, CANAS_WIDTH, bitmapFormat);
    Image image;
    image.BuildFromBitmap(bitmap);
    Rect srcRect(0.0f, 0.0f, CANAS_WIDTH, CANAS_HEIGHT);
    Rect dstRect(0.0f, 0.0f, CANAS_WIDTH, CANAS_HEIGHT);
    SamplingOptions samplingOptions;
    recordingCanvas1->DrawImageRect(image, srcRect, dstRect, samplingOptions);
    recordingCanvas2->DrawImageRect(image, srcRect, dstRect, samplingOptions);

    Brush brush(Color::COLOR_GREEN);
    recordingCanvas1->AttachBrush(brush);
    recordingCanvas2->AttachBrush(brush);
    recordingCanvas1->DrawImageRect(image, srcRect, dstRect, samplingOptions);
    recordingCanvas2->DrawImageRect(image, srcRect, dstRect, samplingOptions);

    Pen pen(Color::COLOR_GREEN);
    recordingCanvas1->AttachPen(pen);
    recordingCanvas2->AttachPen(pen);
    recordingCanvas1->DrawImageRect(image, srcRect, dstRect, samplingOptions);
    recordingCanvas2->DrawImageRect(image, srcRect, dstRect, samplingOptions);

    recordingCanvas1->DetachBrush();
    recordingCanvas2->DetachBrush();
    recordingCanvas1->DrawImageRect(image, srcRect, dstRect, samplingOptions);
    recordingCanvas2->DrawImageRect(image, srcRect, dstRect, samplingOptions);

    auto drawCmdList1 = recordingCanvas1->GetDrawCmdList();
    auto drawCmdList2 = recordingCanvas2->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList1 != nullptr && drawCmdList2 != nullptr);
    Canvas canvas;
    drawCmdList1->Playback(canvas);
    drawCmdList2->Playback(canvas);
}

/**
 * @tc.name: DrawPicture001
 * @tc.desc: Test the playback of the DrawPicture function.
 * @tc.type: FUNC
 * @tc.require: I7K0BS
 */
HWTEST_F(RecordingCanvasTest, DrawPicture001, TestSize.Level1)
{
    auto recordingCanvas1 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT);
    auto recordingCanvas2 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT, false);
    EXPECT_TRUE(recordingCanvas1 != nullptr && recordingCanvas2 != nullptr);
    Picture pic;
    recordingCanvas1->DrawPicture(pic);
    recordingCanvas2->DrawPicture(pic);
    auto drawCmdList1 = recordingCanvas1->GetDrawCmdList();
    auto drawCmdList2 = recordingCanvas2->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList1 != nullptr && drawCmdList2 != nullptr);
    Canvas canvas;
    drawCmdList1->Playback(canvas);
    drawCmdList2->Playback(canvas);
}

/**
 * @tc.name: ClipRect001
 * @tc.desc: Test the playback of the ClipRect function.
 * @tc.type: FUNC
 * @tc.require: I7K0BS
 */
HWTEST_F(RecordingCanvasTest, ClipRect001, TestSize.Level1)
{
    auto recordingCanvas1 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT);
    auto recordingCanvas2 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT, false);
    EXPECT_TRUE(recordingCanvas1 != nullptr && recordingCanvas2 != nullptr);
    Rect rect(0.0f, 0.0f, CANAS_WIDTH, CANAS_HEIGHT);
    recordingCanvas1->ClipRect(rect, ClipOp::DIFFERENCE, true);
    recordingCanvas2->ClipRect(rect, ClipOp::DIFFERENCE, true);
    auto drawCmdList1 = recordingCanvas1->GetDrawCmdList();
    auto drawCmdList2 = recordingCanvas2->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList1 != nullptr && drawCmdList2 != nullptr);
    Canvas canvas;
    drawCmdList1->Playback(canvas);
    drawCmdList2->Playback(canvas);
}

/**
 * @tc.name: ClipIRect001
 * @tc.desc: Test the playback of the ClipRect function.
 * @tc.type: FUNC
 * @tc.require: I7K0BS
 */
HWTEST_F(RecordingCanvasTest, ClipIRect001, TestSize.Level1)
{
    auto recordingCanvas1 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT);
    auto recordingCanvas2 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT, false);
    EXPECT_TRUE(recordingCanvas1 != nullptr && recordingCanvas2 != nullptr);
    RectI rect(0, 0, 10, 20);
    recordingCanvas1->ClipIRect(rect);
    recordingCanvas2->ClipIRect(rect);
    auto drawCmdList1 = recordingCanvas1->GetDrawCmdList();
    auto drawCmdList2 = recordingCanvas2->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList1 != nullptr && drawCmdList2 != nullptr);
    Canvas canvas;
    drawCmdList1->Playback(canvas);
    drawCmdList2->Playback(canvas);
}

/**
 * @tc.name: ClipRoundRect001
 * @tc.desc: Test the playback of the ClipRoundRect function.
 * @tc.type: FUNC
 * @tc.require: I7K0BS
 */
HWTEST_F(RecordingCanvasTest, ClipRoundRect001, TestSize.Level1)
{
    auto recordingCanvas1 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT);
    auto recordingCanvas2 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT, false);
    EXPECT_TRUE(recordingCanvas1 != nullptr && recordingCanvas2 != nullptr);
    Rect rect(0.0f, 0.0f, CANAS_WIDTH, CANAS_HEIGHT);
    RoundRect roundRect(rect, RADIUS, RADIUS);
    recordingCanvas1->ClipRoundRect(roundRect, ClipOp::DIFFERENCE, true);
    recordingCanvas2->ClipRoundRect(roundRect, ClipOp::DIFFERENCE, true);
    auto drawCmdList1 = recordingCanvas1->GetDrawCmdList();
    auto drawCmdList2 = recordingCanvas2->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList1 != nullptr && drawCmdList2 != nullptr);
    Canvas canvas;
    drawCmdList1->Playback(canvas);
    drawCmdList2->Playback(canvas);
}

/**
 * @tc.name: ClipRoundRect002
 * @tc.desc: Test the playback of the ClipRoundRect function.
 * @tc.type: FUNC
 * @tc.require: I7K0BS
 */
HWTEST_F(RecordingCanvasTest, ClipRoundRect002, TestSize.Level1)
{
    auto recordingCanvas1 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT);
    auto recordingCanvas2 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT, false);
    EXPECT_TRUE(recordingCanvas1 != nullptr && recordingCanvas2 != nullptr);
    Rect rect(0.0f, 0.0f, CANAS_WIDTH, CANAS_HEIGHT);
    std::vector<Point> radii = { {RADIUS, RADIUS}, {RADIUS, RADIUS}, {RADIUS, RADIUS}, {RADIUS, RADIUS} };
    recordingCanvas1->ClipRoundRect(rect, radii, true);
    recordingCanvas2->ClipRoundRect(rect, radii, true);
    auto drawCmdList1 = recordingCanvas1->GetDrawCmdList();
    auto drawCmdList2 = recordingCanvas2->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList1 != nullptr && drawCmdList2 != nullptr);
    Canvas canvas;
    drawCmdList1->Playback(canvas);
    drawCmdList2->Playback(canvas);
}

/**
 * @tc.name: ClipPath001
 * @tc.desc: Test the playback of the ClipPath function.
 * @tc.type: FUNC
 * @tc.require: I7K0BS
 */
HWTEST_F(RecordingCanvasTest, ClipPath001, TestSize.Level1)
{
    auto recordingCanvas1 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT);
    auto recordingCanvas2 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT, false);
    EXPECT_TRUE(recordingCanvas1 != nullptr && recordingCanvas2 != nullptr);
    Path path;
    path.AddRect(Rect(0, 0, CANAS_WIDTH, CANAS_HEIGHT));
    recordingCanvas1->ClipPath(path, ClipOp::DIFFERENCE, true);
    recordingCanvas2->ClipPath(path, ClipOp::DIFFERENCE, true);
    auto drawCmdList1 = recordingCanvas1->GetDrawCmdList();
    auto drawCmdList2 = recordingCanvas2->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList1 != nullptr && drawCmdList2 != nullptr);
    Canvas canvas;
    drawCmdList1->Playback(canvas);
    drawCmdList2->Playback(canvas);
}

/**
 * @tc.name: ClipRegion001
 * @tc.desc: Test the playback of the ClipRegion function.
 * @tc.type: FUNC
 * @tc.require: I7K0BS
 */
HWTEST_F(RecordingCanvasTest, ClipRegion001, TestSize.Level1)
{
    auto recordingCanvas1 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT);
    auto recordingCanvas2 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT, false);
    EXPECT_TRUE(recordingCanvas1 != nullptr && recordingCanvas2 != nullptr);
    Region region;
    region.SetRect(RectI(0, 0, CANAS_WIDTH, CANAS_HEIGHT));
    recordingCanvas1->ClipRegion(region);
    recordingCanvas2->ClipRegion(region);
    auto drawCmdList1 = recordingCanvas1->GetDrawCmdList();
    auto drawCmdList2 = recordingCanvas2->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList1 != nullptr && drawCmdList2 != nullptr);
    Canvas canvas;
    drawCmdList1->Playback(canvas);
    drawCmdList2->Playback(canvas);
}

/**
 * @tc.name: SetMatrix001
 * @tc.desc: Test the playback of the SetMatrix function.
 * @tc.type: FUNC
 * @tc.require: I7K0BS
 */
HWTEST_F(RecordingCanvasTest, SetMatrix001, TestSize.Level1)
{
    auto recordingCanvas1 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT);
    auto recordingCanvas2 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT, false);
    EXPECT_TRUE(recordingCanvas1 != nullptr && recordingCanvas2 != nullptr);
    Matrix matrix;
    recordingCanvas1->SetMatrix(matrix);
    recordingCanvas2->SetMatrix(matrix);
    auto drawCmdList1 = recordingCanvas1->GetDrawCmdList();
    auto drawCmdList2 = recordingCanvas2->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList1 != nullptr && drawCmdList2 != nullptr);
    Canvas canvas;
    drawCmdList1->Playback(canvas);
    drawCmdList2->Playback(canvas);
}

/**
 * @tc.name: ResetMatrix001
 * @tc.desc: Test the playback of the ResetMatrix function.
 * @tc.type: FUNC
 * @tc.require: I7K0BS
 */
HWTEST_F(RecordingCanvasTest, ResetMatrix001, TestSize.Level1)
{
    auto recordingCanvas1 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT);
    auto recordingCanvas2 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT, false);
    EXPECT_TRUE(recordingCanvas1 != nullptr && recordingCanvas2 != nullptr);
    recordingCanvas1->ResetMatrix();
    recordingCanvas2->ResetMatrix();
    auto drawCmdList1 = recordingCanvas1->GetDrawCmdList();
    auto drawCmdList2 = recordingCanvas2->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList1 != nullptr && drawCmdList2 != nullptr);
    Canvas canvas;
    drawCmdList1->Playback(canvas);
    drawCmdList2->Playback(canvas);
}

/**
 * @tc.name: ConcatMatrix001
 * @tc.desc: Test the playback of the ConcatMatrix function.
 * @tc.type: FUNC
 * @tc.require: I7K0BS
 */
HWTEST_F(RecordingCanvasTest, ConcatMatrix001, TestSize.Level1)
{
    auto recordingCanvas1 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT);
    auto recordingCanvas2 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT, false);
    EXPECT_TRUE(recordingCanvas1 != nullptr && recordingCanvas2 != nullptr);
    Matrix matrix;
    matrix.Translate(CANAS_WIDTH, CANAS_WIDTH);
    recordingCanvas1->ConcatMatrix(matrix);
    recordingCanvas2->ConcatMatrix(matrix);
    auto drawCmdList1 = recordingCanvas1->GetDrawCmdList();
    auto drawCmdList2 = recordingCanvas2->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList1 != nullptr && drawCmdList2 != nullptr);
    Canvas canvas;
    drawCmdList1->Playback(canvas);
    drawCmdList2->Playback(canvas);
}

/**
 * @tc.name: Translate001
 * @tc.desc: Test the playback of the Translate function.
 * @tc.type: FUNC
 * @tc.require: I7K0BS
 */
HWTEST_F(RecordingCanvasTest, Translate001, TestSize.Level1)
{
    auto recordingCanvas1 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT);
    auto recordingCanvas2 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT, false);
    EXPECT_TRUE(recordingCanvas1 != nullptr && recordingCanvas2 != nullptr);
    recordingCanvas1->Translate(CANAS_WIDTH, CANAS_WIDTH);
    recordingCanvas2->Translate(CANAS_WIDTH, CANAS_WIDTH);
    auto drawCmdList1 = recordingCanvas1->GetDrawCmdList();
    auto drawCmdList2 = recordingCanvas2->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList1 != nullptr && drawCmdList2 != nullptr);
    Canvas canvas;
    drawCmdList1->Playback(canvas);
    drawCmdList2->Playback(canvas);
}

/**
 * @tc.name: Scale001
 * @tc.desc: Test the playback of the Scale function.
 * @tc.type: FUNC
 * @tc.require: I7K0BS
 */
HWTEST_F(RecordingCanvasTest, Scale001, TestSize.Level1)
{
    auto recordingCanvas1 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT);
    auto recordingCanvas2 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT, false);
    EXPECT_TRUE(recordingCanvas1 != nullptr && recordingCanvas2 != nullptr);
    recordingCanvas1->Scale(2.0f, 1.0f);
    recordingCanvas2->Scale(2.0f, 1.0f);
    auto drawCmdList1 = recordingCanvas1->GetDrawCmdList();
    auto drawCmdList2 = recordingCanvas2->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList1 != nullptr && drawCmdList2 != nullptr);
    Canvas canvas;
    drawCmdList1->Playback(canvas);
    drawCmdList2->Playback(canvas);
}

/**
 * @tc.name: Rotate001
 * @tc.desc: Test the playback of the Rotate function.
 * @tc.type: FUNC
 * @tc.require: I7K0BS
 */
HWTEST_F(RecordingCanvasTest, Rotate001, TestSize.Level1)
{
    auto recordingCanvas1 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT);
    auto recordingCanvas2 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT, false);
    EXPECT_TRUE(recordingCanvas1 != nullptr && recordingCanvas2 != nullptr);
    recordingCanvas1->Rotate(ANGLE, CANAS_WIDTH, CANAS_WIDTH);
    recordingCanvas2->Rotate(ANGLE, CANAS_WIDTH, CANAS_WIDTH);
    auto drawCmdList1 = recordingCanvas1->GetDrawCmdList();
    auto drawCmdList2 = recordingCanvas2->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList1 != nullptr && drawCmdList2 != nullptr);
    Canvas canvas;
    drawCmdList1->Playback(canvas);
    drawCmdList2->Playback(canvas);
}

/**
 * @tc.name: Shear001
 * @tc.desc: Test the playback of the Shear function.
 * @tc.type: FUNC
 * @tc.require: I7K0BS
 */
HWTEST_F(RecordingCanvasTest, Shear001, TestSize.Level1)
{
    auto recordingCanvas1 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT);
    auto recordingCanvas2 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT, false);
    EXPECT_TRUE(recordingCanvas1 != nullptr && recordingCanvas2 != nullptr);
    recordingCanvas1->Shear(CANAS_WIDTH, CANAS_WIDTH);
    recordingCanvas2->Shear(CANAS_WIDTH, CANAS_WIDTH);
    auto drawCmdList1 = recordingCanvas1->GetDrawCmdList();
    auto drawCmdList2 = recordingCanvas2->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList1 != nullptr && drawCmdList2 != nullptr);
    Canvas canvas;
    drawCmdList1->Playback(canvas);
    drawCmdList2->Playback(canvas);
}

/**
 * @tc.name: Flush001
 * @tc.desc: Test the playback of the Flush function.
 * @tc.type: FUNC
 * @tc.require: I7K0BS
 */
HWTEST_F(RecordingCanvasTest, Flush001, TestSize.Level1)
{
    auto recordingCanvas1 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT);
    auto recordingCanvas2 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT, false);
    EXPECT_TRUE(recordingCanvas1 != nullptr && recordingCanvas2 != nullptr);
    recordingCanvas1->Flush();
    recordingCanvas2->Flush();
    auto drawCmdList1 = recordingCanvas1->GetDrawCmdList();
    auto drawCmdList2 = recordingCanvas2->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList1 != nullptr && drawCmdList2 != nullptr);
    Canvas canvas;
    drawCmdList1->Playback(canvas);
    drawCmdList2->Playback(canvas);
}

/**
 * @tc.name: Clear001
 * @tc.desc: Test the playback of the Clear function.
 * @tc.type: FUNC
 * @tc.require: I7K0BS
 */
HWTEST_F(RecordingCanvasTest, Clear001, TestSize.Level1)
{
    auto recordingCanvas1 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT);
    auto recordingCanvas2 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT, false);
    EXPECT_TRUE(recordingCanvas1 != nullptr && recordingCanvas2 != nullptr);
    recordingCanvas1->Clear(Color::COLOR_BLUE);
    recordingCanvas2->Clear(Color::COLOR_BLUE);
    auto drawCmdList1 = recordingCanvas1->GetDrawCmdList();
    auto drawCmdList2 = recordingCanvas2->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList1 != nullptr && drawCmdList2 != nullptr);
    Canvas canvas;
    drawCmdList1->Playback(canvas);
    drawCmdList2->Playback(canvas);
}

/**
 * @tc.name: SaveRestore001
 * @tc.desc: Test the playback of the Save function.
 * @tc.type: FUNC
 * @tc.require: I7K0BS
 */
HWTEST_F(RecordingCanvasTest, SaveRestore001, TestSize.Level1)
{
    auto recordingCanvas1 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT);
    auto recordingCanvas2 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT, false);
    EXPECT_TRUE(recordingCanvas1 != nullptr && recordingCanvas2 != nullptr);
    recordingCanvas1->Save();
    recordingCanvas2->Save();
    Rect rect(0.0f, 0.0f, CANAS_WIDTH, CANAS_HEIGHT);
    recordingCanvas1->ClipRect(rect, ClipOp::DIFFERENCE, true);
    recordingCanvas2->ClipRect(rect, ClipOp::DIFFERENCE, true);
    recordingCanvas1->Restore();
    recordingCanvas2->Restore();
    auto drawCmdList1 = recordingCanvas1->GetDrawCmdList();
    auto drawCmdList2 = recordingCanvas2->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList1 != nullptr && drawCmdList2 != nullptr);
    Canvas canvas;
    drawCmdList1->Playback(canvas);
    drawCmdList2->Playback(canvas);
}

/**
 * @tc.name: SaveRestore002
 * @tc.desc: Test the playback of the Save function.
 * @tc.type: FUNC
 * @tc.require: I7K0BS
 */
HWTEST_F(RecordingCanvasTest, SaveRestore002, TestSize.Level1)
{
    auto recordingCanvas1 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT);
    auto recordingCanvas2 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT, false);
    EXPECT_TRUE(recordingCanvas1 != nullptr && recordingCanvas2 != nullptr);
    recordingCanvas1->Save();
    recordingCanvas2->Save();
    EXPECT_TRUE(recordingCanvas1->GetSaveCount() == 1 && recordingCanvas2->GetSaveCount() == 1);
    recordingCanvas1->Restore();
    recordingCanvas2->Restore();
    auto drawCmdList1 = recordingCanvas1->GetDrawCmdList();
    auto drawCmdList2 = recordingCanvas2->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList1 != nullptr && drawCmdList2 != nullptr);
    Canvas canvas;
    drawCmdList1->Playback(canvas);
    drawCmdList2->Playback(canvas);
}

/**
 * @tc.name: SaveLayer001
 * @tc.desc: Test the playback of the SaveLayer function.
 * @tc.type: FUNC
 * @tc.require: I7K0BS
 */
HWTEST_F(RecordingCanvasTest, SaveLayer001, TestSize.Level1)
{
    auto recordingCanvas1 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT);
    auto recordingCanvas2 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT, false);
    EXPECT_TRUE(recordingCanvas1 != nullptr && recordingCanvas2 != nullptr);
    Rect rect(0.0f, 0.0f, CANAS_WIDTH, CANAS_HEIGHT);
    Brush brush;
    uint32_t saveLayerFlags = 0;
    SaveLayerOps saveLayerRec(&rect, &brush, saveLayerFlags);
    recordingCanvas1->SaveLayer(saveLayerRec);
    recordingCanvas2->SaveLayer(saveLayerRec);
    auto drawCmdList1 = recordingCanvas1->GetDrawCmdList();
    auto drawCmdList2 = recordingCanvas2->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList1 != nullptr && drawCmdList2 != nullptr);
    Canvas canvas;
    drawCmdList1->Playback(canvas);
    drawCmdList2->Playback(canvas);
}

/**
 * @tc.name: Restore001
 * @tc.desc: Test the playback of the Restore function.
 * @tc.type: FUNC
 * @tc.require: I7K0BS
 */
HWTEST_F(RecordingCanvasTest, Restore001, TestSize.Level1)
{
    auto recordingCanvas = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT);
    EXPECT_TRUE(recordingCanvas != nullptr);
    recordingCanvas->Restore();
    auto drawCmdList = recordingCanvas->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList != nullptr);
    Canvas canvas;
    drawCmdList->Playback(canvas);
}

/**
 * @tc.name: Discard001
 * @tc.desc: Test the playback of the Discard function.
 * @tc.type: FUNC
 * @tc.require: I7K0BS
 */
HWTEST_F(RecordingCanvasTest, Discard001, TestSize.Level1)
{
    auto recordingCanvas1 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT);
    auto recordingCanvas2 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT, false);
    EXPECT_TRUE(recordingCanvas1 != nullptr && recordingCanvas2 != nullptr);
    recordingCanvas1->Discard();
    recordingCanvas2->Discard();
    auto drawCmdList1 = recordingCanvas1->GetDrawCmdList();
    auto drawCmdList2 = recordingCanvas2->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList1 != nullptr && drawCmdList2 != nullptr);
    Canvas canvas;
    drawCmdList1->Playback(canvas);
    drawCmdList2->Playback(canvas);
}

/**
 * @tc.name: ClipAdaptiveRoundRect001
 * @tc.desc: Test the playback of the ClipAdaptiveRoundRect function.
 * @tc.type: FUNC
 * @tc.require: I7OAIR
 */
HWTEST_F(RecordingCanvasTest, ClipAdaptiveRoundRect001, TestSize.Level1)
{
    auto recordingCanvas1 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT);
    auto recordingCanvas2 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT, false);
    EXPECT_TRUE(recordingCanvas1 != nullptr && recordingCanvas2 != nullptr);
    std::vector<Point> radiusXY = { { 1, 3 } };
    recordingCanvas1->ClipAdaptiveRoundRect(radiusXY);
    recordingCanvas2->ClipAdaptiveRoundRect(radiusXY);
    auto drawCmdList1 = recordingCanvas1->GetDrawCmdList();
    auto drawCmdList2 = recordingCanvas2->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList1 != nullptr && drawCmdList2 != nullptr);
    Canvas canvas;
    Rect rect(0.0f, 0.0f, CANAS_WIDTH, CANAS_HEIGHT);
    drawCmdList1->Playback(canvas, &rect);
    drawCmdList2->Playback(canvas, &rect);
}


/**
 * @tc.name: DrawPixelMap001
 * @tc.desc: Test the playback of the DrawPixelMap function.
 * @tc.type: FUNC
 * @tc.require: I7OAIR
 */
HWTEST_F(RecordingCanvasTest, DrawPixelMap001, TestSize.Level1)
{
    auto recordingCanvas1 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT);
    auto recordingCanvas2 = std::make_shared<RecordingCanvas>(CANAS_WIDTH, CANAS_HEIGHT, false);
    EXPECT_TRUE(recordingCanvas1 != nullptr && recordingCanvas2 != nullptr);
    Media::InitializationOptions opts;
    std::shared_ptr<Media::PixelMap> pixelMap = Media::PixelMap::Create(opts);
    AdaptiveImageInfo rsImageInfo;
    SamplingOptions smapling;
    recordingCanvas1->DrawPixelMap(pixelMap, rsImageInfo, smapling);
    recordingCanvas2->DrawPixelMap(pixelMap, rsImageInfo, smapling);
    auto drawCmdList1 = recordingCanvas1->GetDrawCmdList();
    auto drawCmdList2 = recordingCanvas2->GetDrawCmdList();
    EXPECT_TRUE(drawCmdList1 != nullptr && drawCmdList2 != nullptr);
    Canvas canvas;
    drawCmdList1->Playback(canvas);
    drawCmdList2->Playback(canvas);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS