/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <include/core/SkData.h>
#include <include/core/SkDrawable.h>
#include <include/core/SkMatrix.h>
#include <include/core/SkPath.h>
#include <include/core/SkPicture.h>
#include <include/core/SkRRect.h>
#include <include/core/SkRect.h>
#include <include/core/SkRegion.h>
#include <include/core/SkTextBlob.h>
#include <src/core/SkDrawShadowInfo.h>
#include <test_header.h>

#include "platform/ohos/overdraw/rs_canvas_listener.h"
#include "platform/ohos/overdraw/rs_listened_canvas.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
constexpr float SET_XORY1 = 1.0f;
constexpr float SET_XORY2 = 2.0f;
constexpr float SET_RADIUS = 1.0f;
constexpr float SET_START_ANGLE = 1.0f;
constexpr float SET_SWEEP_ANGLE = 1.0f;
constexpr uint32_t SET_TOP = 100;
constexpr uint32_t SET_BOTTOM = 100;
class RSListenedCanvasTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSListenedCanvasTest::SetUpTestCase() {}
void RSListenedCanvasTest::TearDownTestCase() {}
void RSListenedCanvasTest::SetUp() {}
void RSListenedCanvasTest::TearDown() {}

class MockDrawingCanvas : public Drawing::Canvas {
public:
    MOCK_METHOD1(DrawRegion, void(const Drawing::Region& region));
};

class MockRSPaintFilterCanvas : public RSPaintFilterCanvas {
public:
    explicit MockRSPaintFilterCanvas(Drawing::Canvas* canvas) : RSPaintFilterCanvas(canvas) {}
    MOCK_METHOD1(DrawRect, void(const Drawing::Rect& rect));
};

class MockRSCanvasListener : public RSCanvasListener {
public:
    explicit MockRSCanvasListener(Drawing::Canvas& canvas) : RSCanvasListener(canvas) {}
    MOCK_METHOD1(DrawRect, void(const Drawing::Rect& rect));
};

class TextBlobImplTest : public Drawing::TextBlobImpl {
public:
    TextBlobImplTest() {}
    // Corrected constructor
    std::shared_ptr<Drawing::Data> Serialize(void* ctx) const override
    {
        std::shared_ptr<Drawing::Data> data = std::make_shared<Drawing::Data>();
        EXPECT_TRUE(data);
        return data;
    }

    std::shared_ptr<Drawing::Rect> Bounds() const override
    {
        std::shared_ptr<Drawing::Rect> rect = std::make_shared<Drawing::Rect>();
        EXPECT_TRUE(rect);
        return rect;
    }
    Drawing::AdapterType GetType() const override
    {
        return Drawing::AdapterType::SKIA_ADAPTER;
    }
    uint32_t UniqueID() const override
    {
        static uint32_t idCounter = 0;
        return idCounter++;
    }
};

/*
 * Function: request will pass through listened canvas
 * Type: Function
 * EnvConditions: N/A
 * CaseDescription: 1. create mock MockRSPaintFilterCanvas
 *                  2. expect MockRSPaintFilterCanvas call drawRect once
 *                  3. create RSListenedCanvas from MockRSPaintFilterCanvas
 *                  4. call RSListenedCanvas's drawRect
 */
HWTEST_F(RSListenedCanvasTest, RequestPassThrough, Function | SmallTest | Level2)
{
    PART("CaseDescription")
    {
        auto mockDrawingCanvas = std::make_unique<MockDrawingCanvas>();
        std::shared_ptr<MockRSPaintFilterCanvas> mockRSPaintFilterCanvas = nullptr;
        STEP("1. create mock MockRSPaintFilterCanvas")
        {
            mockRSPaintFilterCanvas = std::make_shared<MockRSPaintFilterCanvas>(mockDrawingCanvas.get());
        }

        Drawing::Rect rect = Drawing::Rect(1, 2, 4, 6);
        STEP("2. expect MockRSPaintFilterCanvas call drawRect once")
        {
            EXPECT_CALL(*mockRSPaintFilterCanvas, DrawRect(rect)).Times(1);
        }

        std::shared_ptr<RSListenedCanvas> listenedCanvas = nullptr;
        STEP("3. create RSListenedCanvas from MockRSPaintFilterCanvas")
        {
            listenedCanvas = std::make_shared<RSListenedCanvas>(*mockRSPaintFilterCanvas);
        }

        STEP("4. call RSListenedCanvas's drawRect")
        {
            listenedCanvas->DrawRect(rect);
        }
    }
}

/*
 * Function: request will split to listener
 * Type: Function
 * EnvConditions: N/A
 * CaseDescription: 1. create mock MockRSPaintFilterCanvas
 *                  2. create mock MockRSCanvasListener
 *                  3. expect MockRSCanvasListener call drawRect once
 *                  4. create RSListenedCanvas from MockRSPaintFilterCanvas
 *                  5. call RSListenedCanvas's drawRect
 */
HWTEST_F(RSListenedCanvasTest, RequestSplitToListener, Function | SmallTest | Level2)
{
    PART("CaseDescription")
    {
        auto mockDrawingCanvas = std::make_unique<MockDrawingCanvas>();
        std::shared_ptr<MockRSPaintFilterCanvas> mockRSPaintFilterCanvas = nullptr;
        STEP("1. create mock MockRSPaintFilterCanvas")
        {
            mockRSPaintFilterCanvas = std::make_shared<MockRSPaintFilterCanvas>(mockDrawingCanvas.get());
        }

        std::shared_ptr<MockRSCanvasListener> mockRSCanvasListener = nullptr;
        STEP("2. create mock MockRSCanvasListener")
        {
            mockRSCanvasListener = std::make_shared<MockRSCanvasListener>(*mockRSPaintFilterCanvas);
        }

        Drawing::Rect rect = Drawing::Rect(1, 2, 4, 6);
        STEP("3. expect MockRSCanvasListener call drawRect once")
        {
            EXPECT_CALL(*mockRSPaintFilterCanvas, DrawRect(rect)).Times(1);
            EXPECT_CALL(*mockRSCanvasListener, DrawRect(rect)).Times(1);
        }

        std::shared_ptr<RSListenedCanvas> listenedCanvas = nullptr;
        STEP("4. create RSListenedCanvas from MockRSPaintFilterCanvas")
        {
            listenedCanvas = std::make_shared<RSListenedCanvas>(*mockRSPaintFilterCanvas);
            listenedCanvas->SetListener(mockRSCanvasListener);
        }

        STEP("5. call RSListenedCanvas's drawRect")
        {
            listenedCanvas->DrawRect(rect);
        }
    }
}

/**
 * @tc.name: onDrawOval001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSListenedCanvasTest, onDrawOval001, TestSize.Level1)
{
    MockDrawingCanvas canvas;
    Drawing::Rect rect;
    RSListenedCanvas listenedCanvas(canvas);
    auto listener = std::make_shared<MockRSCanvasListener>(canvas);
    listenedCanvas.DrawOval(rect);
    listenedCanvas.SetListener(listener);
    listenedCanvas.DrawOval(rect);
}

/**
 * @tc.name: onDrawArc001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSListenedCanvasTest, onDrawArc001, TestSize.Level1)
{
    MockDrawingCanvas canvas;
    Drawing::Rect rect;
    Drawing::scalar startAngle = 0.1;
    Drawing::scalar sweepAngle = 0.2;
    RSListenedCanvas listenedCanvas(canvas);
    auto listener = std::make_shared<MockRSCanvasListener>(canvas);
    listenedCanvas.DrawArc(rect, startAngle, sweepAngle);
    listenedCanvas.SetListener(listener);
    listenedCanvas.DrawArc(rect, startAngle, sweepAngle);
}

/**
 * @tc.name: onDrawPath001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSListenedCanvasTest, onDrawPath001, TestSize.Level1)
{
    MockDrawingCanvas canvas;
    Drawing::Path path;
    RSListenedCanvas listenedCanvas(canvas);
    auto listener = std::make_shared<MockRSCanvasListener>(canvas);
    listenedCanvas.DrawPath(path);
    listenedCanvas.SetListener(listener);
    listenedCanvas.DrawPath(path);
}

/**
 * @tc.name: DrawRegion001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:issueI9NDED
 */
HWTEST_F(RSListenedCanvasTest, DrawRegion001, TestSize.Level1)
{
    Drawing::Canvas canvasTest1;
    RSListenedCanvas listenedCanvas(canvasTest1);
    EXPECT_FALSE(listenedCanvas.listener_);

    Drawing::Canvas canvasTest2;
    std::shared_ptr<RSCanvasListener> canvasListener = std::make_shared<RSCanvasListener>(canvasTest2);
    EXPECT_TRUE(canvasListener);

    Drawing::Region region;
    listenedCanvas.DrawRegion(region);
    listenedCanvas.SetListener(canvasListener);
    EXPECT_TRUE(listenedCanvas.listener_);
    listenedCanvas.DrawRegion(region);
}

/**
 * @tc.name: onDrawPoints001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSListenedCanvasTest, onDrawPoints001, TestSize.Level1)
{
    MockDrawingCanvas canvas;
    Drawing::scalar x = 0.1;
    Drawing::scalar y = 0.2;
    auto point = Rosen::Drawing::Point(x, y);
    RSListenedCanvas listenedCanvas(canvas);
    listenedCanvas.DrawPoint(point);
    auto listener = std::make_shared<MockRSCanvasListener>(canvas);
    listenedCanvas.SetListener(listener);
    listenedCanvas.DrawPoint(point);
}

/**
 * @tc.name: ListenerIsNullptrTest001
 * @tc.desc: ListenerIsNullptr Test
 * @tc.type:FUNC
 * @tc.require:issueI9NDED
 */
HWTEST_F(RSListenedCanvasTest, ListenerIsNullptrTest001, TestSize.Level1)
{
    // When listenedCanvas.listener_ is null, no operation will be performed on the RSListenedCanvas object, so writing
    // the relevant functions together will not have any impact. If there is an operation on listenedCanvas.listener_
    // later, pay attention to modifying the test case
    Drawing::Canvas canvas;
    RSListenedCanvas listenedCanvas(canvas);
    EXPECT_FALSE(listenedCanvas.listener_);

    listenedCanvas.DrawLine(Drawing::Point(SET_XORY1, SET_XORY1), Drawing::Point(SET_XORY2, SET_XORY1));

    Drawing::Rect rect = Drawing::Rect(0, 0, SET_TOP, SET_BOTTOM);
    listenedCanvas.DrawRect(rect);

    Drawing::RoundRect roundRect2;
    listenedCanvas.DrawRoundRect(roundRect2);

    Drawing::RoundRect roundRect21;
    Drawing::RoundRect roundRect22;
    listenedCanvas.DrawNestedRoundRect(roundRect21, roundRect22);

    listenedCanvas.DrawPie(Drawing::Rect(0, 0, SET_TOP, SET_BOTTOM), SET_START_ANGLE, SET_SWEEP_ANGLE);

    listenedCanvas.DrawCircle(Drawing::Point(SET_XORY1, SET_XORY1), SET_RADIUS);

    Drawing::Brush brush2;
    listenedCanvas.DrawBackground(brush2);

    Drawing::Path path;
    Drawing::Point3 planeParams;
    Drawing::Point3 devLightPos;
    Drawing::Color ambientColor;
    Drawing::Color spotColor;
    listenedCanvas.DrawShadow(
        path, planeParams, devLightPos, SET_XORY1, ambientColor, spotColor, Drawing::ShadowFlags::NONE);

    Drawing::Picture picture;
    listenedCanvas.DrawPicture(picture);
    listenedCanvas.Clear(Drawing::Color::COLOR_BLACK);

    Drawing::Bitmap bitmap;
    listenedCanvas.DrawBitmap(bitmap, SET_XORY1, SET_XORY1);

    std::shared_ptr<TextBlobImplTest> textBlobImplTest = std::make_shared<TextBlobImplTest>();
    EXPECT_TRUE(textBlobImplTest);
    Drawing::TextBlob blob(textBlobImplTest);
    listenedCanvas.DrawTextBlob(&blob, SET_XORY1, SET_XORY1);

    Drawing::Image imageTest1;
    Drawing::SamplingOptions samplingTest1;
    listenedCanvas.DrawImage(imageTest1, SET_XORY1, SET_XORY1, samplingTest1);

    Drawing::Image imageTest2;
    Drawing::SamplingOptions samplingTest2;
    listenedCanvas.DrawImageRect(imageTest2, Drawing::Rect(0, 0, SET_TOP, SET_BOTTOM),
        Drawing::Rect(0, 0, SET_TOP, SET_BOTTOM), samplingTest2,
        Drawing::SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT);

    Drawing::Image imageTest3;
    Drawing::SamplingOptions samplingTest3;
    listenedCanvas.DrawImageRect(imageTest3, Drawing::Rect(0, 0, SET_TOP, SET_BOTTOM), samplingTest3);

    Drawing::Pen pen;
    listenedCanvas.AttachPen(pen);

    Drawing::Brush brush;
    listenedCanvas.AttachBrush(brush);

    listenedCanvas.DetachPen();
    listenedCanvas.DetachBrush();
}

/**
 * @tc.name: DrawingFunctionsTest002
 * @tc.desc: DrawingFunctions Test
 * @tc.type:FUNC
 * @tc.require:issueI9NDED
 */
HWTEST_F(RSListenedCanvasTest, DrawingFunctionsTest002, TestSize.Level1)
{
    Drawing::Canvas canvas;
    RSListenedCanvas listenedCanvas(canvas);
    Drawing::Canvas canvasTest;
    std::shared_ptr<RSCanvasListener> canvasListener = std::make_shared<RSCanvasListener>(canvasTest);
    EXPECT_TRUE(canvasListener);
    listenedCanvas.SetListener(canvasListener);
    EXPECT_TRUE(listenedCanvas.listener_);

    Drawing::RoundRect roundRectTest1;
    listenedCanvas.DrawRoundRect(roundRectTest1);

    Drawing::RoundRect roundRectTest2;
    Drawing::RoundRect roundRectTest3;
    listenedCanvas.DrawNestedRoundRect(roundRectTest2, roundRectTest3);

    listenedCanvas.DrawPie(Drawing::Rect(0, 0, SET_TOP, SET_BOTTOM), SET_START_ANGLE, SET_SWEEP_ANGLE);

    listenedCanvas.DrawCircle(Drawing::Point(SET_XORY1, SET_XORY1), SET_RADIUS);

    Drawing::Brush brush;
    listenedCanvas.DrawBackground(brush);

    Drawing::Path path;
    Drawing::Point3 planeParams;
    Drawing::Point3 devLightPos;
    Drawing::Color ambientColor;
    Drawing::Color spotColor;
    listenedCanvas.DrawShadow(
        path, planeParams, devLightPos, SET_XORY1, ambientColor, spotColor, Drawing::ShadowFlags::NONE);

    Drawing::Picture picture;
    listenedCanvas.DrawPicture(picture);

    listenedCanvas.Clear(Drawing::Color::COLOR_BLACK);
}

/**
 * @tc.name: DrawingFunctionsTest003
 * @tc.desc: DrawingFunctions Test
 * @tc.type:FUNC
 * @tc.require:issueI9NDED
 */
HWTEST_F(RSListenedCanvasTest, DrawingFunctionsTest003, TestSize.Level1)
{
    Drawing::Canvas canvas;
    RSListenedCanvas listenedCanvas(canvas);
    Drawing::Canvas canvasTest;
    std::shared_ptr<RSCanvasListener> canvasListener = std::make_shared<RSCanvasListener>(canvasTest);
    EXPECT_TRUE(canvasListener);
    listenedCanvas.SetListener(canvasListener);
    EXPECT_TRUE(listenedCanvas.listener_);

    listenedCanvas.DrawLine(Drawing::Point(SET_XORY1, SET_XORY1), Drawing::Point(SET_XORY2, SET_XORY1));

    Drawing::Rect rect = Drawing::Rect(0, 0, SET_TOP, SET_BOTTOM);
    listenedCanvas.DrawRect(rect);
}

/**
 * @tc.name: BitmapFunctionsTest004
 * @tc.desc: BitmapFunctions Test
 * @tc.type:FUNC
 * @tc.require:issueI9NDED
 */
HWTEST_F(RSListenedCanvasTest, BitmapFunctionsTest004, TestSize.Level1)
{
    Drawing::Canvas canvas;
    RSListenedCanvas listenedCanvas(canvas);
    Drawing::Canvas canvasTest;
    std::shared_ptr<RSCanvasListener> canvasListener = std::make_shared<RSCanvasListener>(canvasTest);
    EXPECT_TRUE(canvasListener);
    listenedCanvas.SetListener(canvasListener);
    EXPECT_TRUE(listenedCanvas.listener_);

    Drawing::Bitmap bitmap;
    listenedCanvas.DrawBitmap(bitmap, SET_XORY1, SET_XORY1);
}

/**
 * @tc.name: TextBlobFunctionsTest005
 * @tc.desc: TextBlobFunctions Test
 * @tc.type:FUNC
 * @tc.require:issueI9NDED
 */
HWTEST_F(RSListenedCanvasTest, TextBlobFunctionsTest005, TestSize.Level1)
{
    Drawing::Canvas canvas;
    RSListenedCanvas listenedCanvas(canvas);
    Drawing::Canvas canvasTest;
    std::shared_ptr<RSCanvasListener> canvasListener = std::make_shared<RSCanvasListener>(canvasTest);
    EXPECT_TRUE(canvasListener);
    listenedCanvas.SetListener(canvasListener);
    EXPECT_TRUE(listenedCanvas.listener_);

    std::shared_ptr<TextBlobImplTest> textBlobImplTest = std::make_shared<TextBlobImplTest>();
    EXPECT_TRUE(textBlobImplTest);
    Drawing::TextBlob blob(textBlobImplTest);
    listenedCanvas.DrawTextBlob(&blob, SET_XORY1, SET_XORY1);
}

/**
 * @tc.name: ImageFunctionsTestTest005
 * @tc.desc: ImageFunctionsTest Test
 * @tc.type:FUNC
 * @tc.require:issueI9NDED
 */
HWTEST_F(RSListenedCanvasTest, ImageFunctionsTest006, TestSize.Level1)
{
    Drawing::Canvas canvas;
    RSListenedCanvas listenedCanvas(canvas);
    Drawing::Canvas canvasTest;
    std::shared_ptr<RSCanvasListener> canvasListener = std::make_shared<RSCanvasListener>(canvasTest);
    EXPECT_TRUE(canvasListener);
    listenedCanvas.SetListener(canvasListener);
    EXPECT_TRUE(listenedCanvas.listener_);

    Drawing::Image imageTest1;
    Drawing::SamplingOptions samplingTest1;
    listenedCanvas.DrawImage(imageTest1, SET_XORY1, SET_XORY1, samplingTest1);

    Drawing::Image imageTest2;
    Drawing::SamplingOptions samplingTest2;
    listenedCanvas.DrawImageRect(imageTest2, Drawing::Rect(0, 0, SET_TOP, SET_BOTTOM),
        Drawing::Rect(0, 0, SET_TOP, SET_BOTTOM), samplingTest2,
        Drawing::SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT);

    Drawing::Image imageTest3;
    Drawing::SamplingOptions samplingTest3;
    listenedCanvas.DrawImageRect(imageTest3, Drawing::Rect(0, 0, SET_TOP, SET_BOTTOM), samplingTest3);
}

/**
 * @tc.name: PenAndBrushFunctionsTest007
 * @tc.desc: PenAndBrushFunctions Test
 * @tc.type:FUNC
 * @tc.require:issueI9NDED
 */
HWTEST_F(RSListenedCanvasTest, PenAndBrushFunctionsTest007, TestSize.Level1)
{
    Drawing::Canvas canvas;
    RSListenedCanvas listenedCanvas(canvas);
    Drawing::Canvas canvasTest;
    std::shared_ptr<RSCanvasListener> canvasListener = std::make_shared<RSCanvasListener>(canvasTest);
    EXPECT_TRUE(canvasListener);
    listenedCanvas.SetListener(canvasListener);
    EXPECT_TRUE(listenedCanvas.listener_);

    Drawing::Pen pen;
    listenedCanvas.AttachPen(pen);

    Drawing::Brush brush;
    listenedCanvas.AttachBrush(brush);

    listenedCanvas.DetachPen();
    listenedCanvas.DetachBrush();
}
} // namespace Rosen
} // namespace OHOS
