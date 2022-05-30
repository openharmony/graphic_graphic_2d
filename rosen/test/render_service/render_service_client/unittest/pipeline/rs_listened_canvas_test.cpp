/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include <include/core/SkRegion.h>
#include <test_header.h>

#include "pipeline/overdraw/rs_canvas_listener.h"
#include "pipeline/overdraw/rs_listened_canvas.h"

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

class MockSkCanvas : public SkCanvas {
public:
    MOCK_METHOD2(onDrawRegion, void(const SkRegion& region, const SkPaint& paint));
};

class MockRSPaintFilterCanvas : public RSPaintFilterCanvas {
public:
    explicit MockRSPaintFilterCanvas(SkCanvas *canvas) : RSPaintFilterCanvas(canvas) {}
    MOCK_METHOD2(onDrawRect, void(const SkRect& rect, const SkPaint& paint));
};

class MockRSCanvasListener : public RSCanvasListener {
public:
    explicit MockRSCanvasListener(SkCanvas &canvas) : RSCanvasListener(canvas) {}
    MOCK_METHOD2(onDrawRect, void(const SkRect& rect, const SkPaint& paint));
};

/*
 * Function: request will pass through listened canvas
 * Type: Function
 * EnvConditions: N/A
 * CaseDescription: 1. new mock MockRSPaintFilterCanvas
 *                  2. expect MockRSPaintFilterCanvas call drawRect once
 *                  3. new RSListenedCanvas from MockRSPaintFilterCanvas
 *                  4. call RSListenedCanvas's drawRect
 *                  5. MockRSPaintFilterCanvas will delete automatically by delete RSListenedCanvas
 */
HWTEST_F(RSListenedCanvasTest, RequestPassThrough, Function | SmallTest | Level2)
{
    PART("CaseDescription") {
        auto mockSkCanvas = std::make_unique<MockSkCanvas>();
        MockRSPaintFilterCanvas *mockRSPaintFilterCanvas = nullptr;
        STEP("1. new mock MockRSPaintFilterCanvas") {
            mockRSPaintFilterCanvas = new MockRSPaintFilterCanvas(mockSkCanvas.get());
        }

        SkRect rect = SkRect::MakeXYWH(1, 2, 3, 4);
        STEP("2. expect MockRSPaintFilterCanvas call drawRect once") {
            EXPECT_CALL(*mockRSPaintFilterCanvas, onDrawRect(rect, _)).Times(1);
        }

        RSListenedCanvas *listenedCanvas = nullptr;
        STEP("3. new RSListenedCanvas from MockRSPaintFilterCanvas") {
            listenedCanvas = new RSListenedCanvas(mockRSPaintFilterCanvas);
        }

        STEP("4. call RSListenedCanvas's drawRect") {
            listenedCanvas->drawRect(rect, {});
        }

        STEP("5. MockRSPaintFilterCanvas will delete automatically") {
            delete listenedCanvas;
        }
    }
}

/*
 * Function: request will split to listener
 * Type: Function
 * EnvConditions: N/A
 * CaseDescription: 1. new mock MockRSPaintFilterCanvas
 *                  2. new mock MockRSCanvasListener
 *                  3. expect MockRSCanvasListener call drawRect once
 *                  4. new RSListenedCanvas from MockRSPaintFilterCanvas
 *                  5. call RSListenedCanvas's drawRect
 */
HWTEST_F(RSListenedCanvasTest, RequestSplitToListener, Function | SmallTest | Level2)
{
    PART("CaseDescription") {
        auto mockSkCanvas = std::make_unique<MockSkCanvas>();
        MockRSPaintFilterCanvas *mockRSPaintFilterCanvas = nullptr;
        STEP("1. new mock MockRSPaintFilterCanvas") {
            mockRSPaintFilterCanvas = new MockRSPaintFilterCanvas(mockSkCanvas.get());
        }

        std::shared_ptr<MockRSCanvasListener> mockRSCanvasListener = nullptr;
        STEP("2. new mock MockRSCanvasListener") {
            mockRSCanvasListener = std::make_shared<MockRSCanvasListener>(*mockRSPaintFilterCanvas);
        }

        SkRect rect = SkRect::MakeXYWH(1, 2, 3, 4);
        STEP("3. expect MockRSCanvasListener call drawRect once") {
            EXPECT_CALL(*mockRSPaintFilterCanvas, onDrawRect(rect, _)).Times(1);
            EXPECT_CALL(*mockRSCanvasListener, onDrawRect(rect, _)).Times(1);
        }

        RSListenedCanvas *listenedCanvas = nullptr;
        STEP("4. new RSListenedCanvas from MockRSPaintFilterCanvas") {
            listenedCanvas = new RSListenedCanvas(mockRSPaintFilterCanvas);
            listenedCanvas->SetListener(mockRSCanvasListener);
        }

        STEP("5. call RSListenedCanvas's drawRect") {
            listenedCanvas->drawRect(rect, {});
        }

        delete listenedCanvas;
    }
}
} // namespace Rosen
} // namespace OHOS
