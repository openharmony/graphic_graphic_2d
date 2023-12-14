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

#ifndef USE_ROSEN_DRAWING
class MockSkCanvas : public SkCanvas {
public:
    MOCK_METHOD2(onDrawRegion, void(const SkRegion& region, const SkPaint& paint));
};

class MockRSPaintFilterCanvas : public RSPaintFilterCanvas {
public:
    explicit MockRSPaintFilterCanvas(SkCanvas* canvas) : RSPaintFilterCanvas(canvas) {}
    MOCK_METHOD2(onDrawRect, void(const SkRect& rect, const SkPaint& paint));
};

class MockRSCanvasListener : public RSCanvasListener {
public:
    explicit MockRSCanvasListener(SkCanvas& canvas) : RSCanvasListener(canvas) {}
    MOCK_METHOD2(onDrawRect, void(const SkRect& rect, const SkPaint& paint));
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
        auto mockSkCanvas = std::make_unique<MockSkCanvas>();
        std::shared_ptr<MockRSPaintFilterCanvas> mockRSPaintFilterCanvas = nullptr;
        STEP("1. create mock MockRSPaintFilterCanvas")
        {
            mockRSPaintFilterCanvas = std::make_shared<MockRSPaintFilterCanvas>(mockSkCanvas.get());
        }

        SkRect rect = SkRect::MakeXYWH(1, 2, 3, 4);
        STEP("2. expect MockRSPaintFilterCanvas call drawRect once")
        {
            EXPECT_CALL(*mockRSPaintFilterCanvas, onDrawRect(rect, _)).Times(1);
        }

        std::shared_ptr<RSListenedCanvas> listenedCanvas = nullptr;
        STEP("3. create RSListenedCanvas from MockRSPaintFilterCanvas")
        {
            listenedCanvas = std::make_shared<RSListenedCanvas>(mockRSPaintFilterCanvas.get());
        }

        STEP("4. call RSListenedCanvas's drawRect")
        {
            listenedCanvas->drawRect(rect, {});
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
        auto mockSkCanvas = std::make_unique<MockSkCanvas>();
        std::shared_ptr<MockRSPaintFilterCanvas> mockRSPaintFilterCanvas = nullptr;
        STEP("1. create mock MockRSPaintFilterCanvas")
        {
            mockRSPaintFilterCanvas = std::make_shared<MockRSPaintFilterCanvas>(mockSkCanvas.get());
        }

        std::shared_ptr<MockRSCanvasListener> mockRSCanvasListener = nullptr;
        STEP("2. create mock MockRSCanvasListener")
        {
            mockRSCanvasListener = std::make_shared<MockRSCanvasListener>(*mockRSPaintFilterCanvas);
        }

        SkRect rect = SkRect::MakeXYWH(1, 2, 3, 4);
        STEP("3. expect MockRSCanvasListener call drawRect once")
        {
            EXPECT_CALL(*mockRSPaintFilterCanvas, onDrawRect(rect, _)).Times(1);
            EXPECT_CALL(*mockRSCanvasListener, onDrawRect(rect, _)).Times(1);
        }

        std::shared_ptr<RSListenedCanvas> listenedCanvas = nullptr;
        STEP("4. create RSListenedCanvas from MockRSPaintFilterCanvas")
        {
            listenedCanvas = std::make_shared<RSListenedCanvas>(mockRSPaintFilterCanvas.get());
            listenedCanvas->SetListener(mockRSCanvasListener);
        }

        STEP("5. call RSListenedCanvas's drawRect")
        {
            listenedCanvas->drawRect(rect, {});
        }
    }
}

/**
 * @tc.name: onDrawPaint001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSListenedCanvasTest, onDrawPaint001, TestSize.Level1)
{
    MockSkCanvas skCanvas;
    SkPaint skPaint;
    RSListenedCanvas listenedCanvas(&skCanvas);
    listenedCanvas.onDrawPaint(skPaint);
}

/**
 * @tc.name: onDrawPaint002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSListenedCanvasTest, onDrawPaint002, TestSize.Level1)
{
    MockSkCanvas skCanvas;
    SkPaint skPaint;
    RSListenedCanvas listenedCanvas(&skCanvas);
    auto listener = std::make_shared<MockRSCanvasListener>(skCanvas);
    listenedCanvas.SetListener(listener);
    listenedCanvas.onDrawPaint(skPaint);
}

/**
 * @tc.name: onDrawRRect001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSListenedCanvasTest, onDrawRRect001, TestSize.Level1)
{
    MockSkCanvas skCanvas;
    SkPaint skPaint;
    SkRRect skRRect;
    RSListenedCanvas listenedCanvas(&skCanvas);
    auto listener = std::make_shared<MockRSCanvasListener>(skCanvas);
    listenedCanvas.onDrawRRect(skRRect, skPaint);
    listenedCanvas.SetListener(listener);
    listenedCanvas.onDrawRRect(skRRect, skPaint);
}

/**
 * @tc.name: onDrawDRRect001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSListenedCanvasTest, onDrawDRRect001, TestSize.Level1)
{
    MockSkCanvas skCanvas;
    SkPaint skPaint;
    SkRRect outer;
    SkRRect inner;
    RSListenedCanvas listenedCanvas(&skCanvas);
    auto listener = std::make_shared<MockRSCanvasListener>(skCanvas);
    listenedCanvas.onDrawDRRect(outer, inner, skPaint);
    listenedCanvas.SetListener(listener);
    listenedCanvas.onDrawDRRect(outer, inner, skPaint);
}

/**
 * @tc.name: onDrawOval001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSListenedCanvasTest, onDrawOval001, TestSize.Level1)
{
    MockSkCanvas skCanvas;
    SkPaint skPaint;
    SkRect rect;
    RSListenedCanvas listenedCanvas(&skCanvas);
    auto listener = std::make_shared<MockRSCanvasListener>(skCanvas);
    listenedCanvas.onDrawOval(rect, skPaint);
    listenedCanvas.SetListener(listener);
    listenedCanvas.onDrawOval(rect, skPaint);
}

/**
 * @tc.name: onDrawArc001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSListenedCanvasTest, onDrawArc001, TestSize.Level1)
{
    MockSkCanvas skCanvas;
    SkPaint skPaint;
    SkRect rect;
    SkScalar startAngle = 0.1;
    SkScalar sweepAngle = 0.2;
    bool useCenter = false;
    RSListenedCanvas listenedCanvas(&skCanvas);
    auto listener = std::make_shared<MockRSCanvasListener>(skCanvas);
    listenedCanvas.onDrawArc(rect, startAngle, sweepAngle, useCenter, skPaint);
    listenedCanvas.SetListener(listener);
    listenedCanvas.onDrawArc(rect, startAngle, sweepAngle, useCenter, skPaint);
}

/**
 * @tc.name: onDrawPath001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSListenedCanvasTest, onDrawPath001, TestSize.Level1)
{
    MockSkCanvas skCanvas;
    SkPaint paint;
    SkPath path;
    RSListenedCanvas listenedCanvas(&skCanvas);
    auto listener = std::make_shared<MockRSCanvasListener>(skCanvas);
    listenedCanvas.onDrawPath(path, paint);
    listenedCanvas.SetListener(listener);
    listenedCanvas.onDrawPath(path, paint);
}

/**
 * @tc.name: onDrawRegion001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSListenedCanvasTest, onDrawRegion001, TestSize.Level1)
{
    MockSkCanvas skCanvas;
    SkPaint paint;
    SkRegion region;
    RSListenedCanvas listenedCanvas(&skCanvas);
    auto listener = std::make_shared<MockRSCanvasListener>(skCanvas);
    listenedCanvas.onDrawRegion(region, paint);
    listenedCanvas.SetListener(listener);
    listenedCanvas.onDrawRegion(region, paint);
}

/**
 * @tc.name: onDrawPoints001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSListenedCanvasTest, onDrawPoints001, TestSize.Level1)
{
    MockSkCanvas skCanvas;
    SkPaint paint;
    size_t count = 1;
    SkScalar x = 0.1;
    SkScalar y = 0.2;
    SkPoint point = SkPoint::Make(x, y);
    SkPoint pts[] = { point };
    RSListenedCanvas listenedCanvas(&skCanvas);
    listenedCanvas.onDrawPoints(SkCanvas::PointMode::kLines_PointMode, count, pts, paint);
    auto listener = std::make_shared<MockRSCanvasListener>(skCanvas);
    listenedCanvas.SetListener(listener);
    listenedCanvas.onDrawPoints(SkCanvas::PointMode::kLines_PointMode, count, pts, paint);
}

/**
 * @tc.name: onDrawShadowRec001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSListenedCanvasTest, onDrawShadowRec001, TestSize.Level1)
{
    MockSkCanvas skCanvas;
    SkPath path;
    SkDrawShadowRec rect;
    RSListenedCanvas listenedCanvas(&skCanvas);
    listenedCanvas.onDrawShadowRec(path, rect);
    auto listener = std::make_shared<MockRSCanvasListener>(skCanvas);
    listenedCanvas.SetListener(listener);
    listenedCanvas.onDrawShadowRec(path, rect);
}

/**
 * @tc.name: onDrawTextBlob001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSListenedCanvasTest, onDrawTextBlob001, TestSize.Level1)
{
    string out = "Hello";
    sk_sp<SkTextBlob> SkTextBlob = SkTextBlob::MakeFromString(out.c_str(), SkFont(nullptr, 24.0f, 1.0f, 0.0f));
    SkPaint paint;
    MockSkCanvas skCanvas;
    RSListenedCanvas listenedCanvas(&skCanvas);
    listenedCanvas.onDrawTextBlob(SkTextBlob.get(), 1.f, 1.f, paint);
    auto listener = std::make_shared<MockRSCanvasListener>(skCanvas);
    listenedCanvas.SetListener(listener);
    listenedCanvas.onDrawTextBlob(SkTextBlob.get(), 1.f, 1.f, paint);
}

/**
 * @tc.name: onDrawAnnotation001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSListenedCanvasTest, onDrawAnnotation001, TestSize.Level1)
{
    MockSkCanvas skCanvas;
    RSListenedCanvas listenedCanvas(&skCanvas);
    SkRect rect = SkRect::MakeWH(1.f, 2.f);
    const char key[] = { '1', '2' };
    sk_sp<SkData> value = SkData::MakeEmpty();
    listenedCanvas.onDrawAnnotation(rect, key, value.get());
    auto listener = std::make_shared<MockRSCanvasListener>(skCanvas);
    listenedCanvas.SetListener(listener);
    listenedCanvas.onDrawAnnotation(rect, key, value.get());
}

/**
 * @tc.name: onDrawDrawable001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSListenedCanvasTest, onDrawDrawable001, TestSize.Level1)
{
    MockSkCanvas skCanvas;
    RSListenedCanvas listenedCanvas(&skCanvas);
    char data[] = { '0', '0' };
    sk_sp<SkDrawable> drawable = SkDrawable::Deserialize(data, sizeof(data));
    SkMatrix matrix = SkMatrix::I();
    listenedCanvas.onDrawDrawable(drawable.get(), &matrix);
    auto listener = std::make_shared<MockRSCanvasListener>(skCanvas);
    listenedCanvas.SetListener(listener);
    listenedCanvas.onDrawDrawable(drawable.get(), &matrix);
}

/**
 * @tc.name: onDrawPicture001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSListenedCanvasTest, onDrawPicture001, TestSize.Level1)
{
    MockSkCanvas skCanvas;
    RSListenedCanvas listenedCanvas(&skCanvas);
    SkRect rect = SkRect::MakeXYWH(1.f, 1.f, 2.f, 3.f);
    sk_sp<SkPicture> skpicture = SkPicture::MakePlaceholder(rect);
    SkMatrix matrix = SkMatrix::I();
    SkPaint paint;
    listenedCanvas.onDrawPicture(skpicture.get(), &matrix, &paint);
    auto listener = std::make_shared<MockRSCanvasListener>(skCanvas);
    listenedCanvas.SetListener(listener);
    listenedCanvas.onDrawPicture(skpicture.get(), &matrix, &paint);
}

#else
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
 * @tc.name: onDrawRegion001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSListenedCanvasTest, onDrawRegion001, TestSize.Level1)
{
    MockDrawingCanvas canvas;
    Drawing::Region region;
    RSListenedCanvas listenedCanvas(canvas);
    auto listener = std::make_shared<MockRSCanvasListener>(canvas);
    listenedCanvas.DrawRegion(region);
    listenedCanvas.SetListener(listener);
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
#endif
} // namespace Rosen
} // namespace OHOS
