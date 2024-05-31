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
#include <include/core/SkDrawable.h>
#include <include/core/SkMatrix.h>
#include <include/core/SkPath.h>
#include <include/core/SkPicture.h>
#include <include/core/SkRRect.h>
#include <include/core/SkRegion.h>
#include <include/core/SkTextBlob.h>
#include <test_header.h>

#include "platform/ohos/overdraw/rs_cpu_overdraw_canvas_listener.h"
#include "platform/ohos/overdraw/rs_listened_canvas.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
constexpr float SET_XORY1 = 1.0f;
constexpr float SET_XORY2 = 2.0f;
constexpr uint32_t SET_SIZE = 2;
constexpr uint32_t SET_TOP = 100;
constexpr uint32_t SET_BOTTOM = 100;
class RSCPUOverdrawCanvasListenerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSCPUOverdrawCanvasListenerTest::SetUpTestCase() {}
void RSCPUOverdrawCanvasListenerTest::TearDownTestCase() {}
void RSCPUOverdrawCanvasListenerTest::SetUp() {}
void RSCPUOverdrawCanvasListenerTest::TearDown() {}

class MockDrawingCanvas : public Drawing::Canvas {
public:
    MOCK_METHOD1(DrawRegion, void(const Drawing::Region& region));
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
 * Function: mock draw 3 regions that do not intersect each other
 * Type: Function
 * EnvConditions: N/A
 * CaseDescription: 1. new mock MockDrawingCanvas
 *                  2. expect MockDrawingCanvas call DrawRegion once
 *                  3. new RSCPUOverdrawCanvasListener from MockDrawingCanvas
 *                  4. call RSCPUOverdrawCanvasListener's onDrawRect 3 times
 *                      - rect{  0,   0, 100, 100}
 *                      - rect{200, 200, 100, 100}
 *                      - rect{400, 400, 100, 100}
 */
HWTEST_F(RSCPUOverdrawCanvasListenerTest, NoIntersect, Function | SmallTest | Level2)
{
    PART("CaseDescription")
    {
        std::unique_ptr<MockDrawingCanvas> mockDrawingCanvas = nullptr;
        STEP("1. new mock MockDrawingCanvas")
        {
            mockDrawingCanvas = std::make_unique<MockDrawingCanvas>();
        }

        STEP("2. expect MockDrawingCanvas call DrawRegion once")
        {
            EXPECT_CALL(*mockDrawingCanvas, DrawRegion(_)).Times(1);
        }

        std::unique_ptr<RSCPUOverdrawCanvasListener> rsOverdrawCanvasListener = nullptr;
        STEP("3. new RSCPUOverdrawCanvasListener from MockDrawingCanvas")
        {
            rsOverdrawCanvasListener = std::make_unique<RSCPUOverdrawCanvasListener>(*mockDrawingCanvas);
        }

        STEP("4. call RSCPUOverdrawCanvasListener's onDrawRect 3 times")
        {
            rsOverdrawCanvasListener->DrawRect(Drawing::Rect(0, 0, 100, 100));
            rsOverdrawCanvasListener->DrawRect(Drawing::Rect(200, 200, 300, 300));
            rsOverdrawCanvasListener->DrawRect(Drawing::Rect(400, 400, 500, 500));
            rsOverdrawCanvasListener->Draw();
        }
    }
}

/**
 * @tc.name: DrawText001
 * @tc.desc: Draw Test
 * @tc.type: FUNC
 * @tc.require: issueI9NDED
 */
HWTEST_F(RSCPUOverdrawCanvasListenerTest, DrawText001, TestSize.Level1)
{
    // configuration AppendRegion functions condition
    Drawing::Canvas canvas;
    RSCPUOverdrawCanvasListener canvasListener(canvas);
    Drawing::Region region;

    canvasListener.regions_.emplace(0, region);
    canvasListener.regions_.emplace(1, region);
    ASSERT_EQ(canvasListener.regions_.size(), SET_SIZE);

    canvasListener.Draw();
}

/**
 * @tc.name: DrawTextBlobAndAppendRegionTest002
 * @tc.desc: DrawTextBlob and AppendRegion Test
 * @tc.type: FUNC
 * @tc.require: issueI9NDED
 */
HWTEST_F(RSCPUOverdrawCanvasListenerTest, DrawTextBlobAndAppendRegionTest002, TestSize.Level1)
{
    // configuration AppendRegion functions condition
    Drawing::Canvas canvas;
    RSCPUOverdrawCanvasListener canvasListener(canvas);
    Drawing::Region region;
    canvasListener.regions_.emplace(0, region);
    canvasListener.regions_.emplace(1, region);
    ASSERT_EQ(canvasListener.regions_.size(), SET_SIZE);

    // TextBlob is nullptr
    Drawing::TextBlob* blobText1 = nullptr;
    canvasListener.DrawTextBlob(blobText1, SET_XORY1, SET_XORY1);

    // TextBlob is not nullptr
    std::shared_ptr<TextBlobImplTest> textBlobImplTest = std::make_shared<TextBlobImplTest>();
    EXPECT_TRUE(textBlobImplTest);
    Drawing::TextBlob blobText2(textBlobImplTest);
    EXPECT_TRUE(blobText2.Bounds());
    canvasListener.DrawTextBlob(&blobText2, SET_XORY1, SET_XORY1);
}

/**
 * @tc.name: RSCPUOverdrawCanvasShapeDrawerTest003
 * @tc.desc: RSCPUOverdrawCanvasShapeDrawer Test
 * @tc.type: FUNC
 * @tc.require: issueI9NDED
 */
HWTEST_F(RSCPUOverdrawCanvasListenerTest, RSCPUOverdrawCanvasShapeDrawerTest003, TestSize.Level1)
{
    Drawing::Canvas canvas;
    RSCPUOverdrawCanvasListener canvasListener(canvas);

    canvasListener.DrawPoint(Drawing::Point(SET_XORY1, SET_XORY1));
    canvasListener.DrawLine(Drawing::Point(SET_XORY1, SET_XORY1), Drawing::Point(SET_XORY2, SET_XORY2));

    Drawing::RoundRect roundRectTest1;
    canvasListener.DrawRoundRect(roundRectTest1);

    Drawing::RoundRect roundRectTest2;
    Drawing::RoundRect roundRectTest3;
    canvasListener.DrawNestedRoundRect(roundRectTest2, roundRectTest3);
}

/**
 * @tc.name: RSCPUOverdrawCanvasRendererTest004
 * @tc.desc: RSCPUOverdrawCanvasRenderer Test
 * @tc.type: FUNC
 * @tc.require: issueI9NDED
 */
HWTEST_F(RSCPUOverdrawCanvasListenerTest, RSCPUOverdrawCanvasRendererTest004, TestSize.Level1)
{
    Drawing::Canvas canvas;
    RSCPUOverdrawCanvasListener canvasListener(canvas);
    canvasListener.DrawArc(Drawing::Rect(0, 0, SET_TOP, SET_BOTTOM), SET_XORY1, SET_XORY1);

    canvasListener.DrawPie(Drawing::Rect(0, 0, SET_TOP, SET_BOTTOM), SET_XORY2, SET_XORY2);

    canvasListener.DrawOval(Drawing::Rect(0, 0, SET_TOP, SET_BOTTOM));

    canvasListener.DrawCircle(Drawing::Point(SET_XORY1, SET_XORY1), SET_XORY1);

    Drawing::Path path;
    canvasListener.DrawPath(path);
}

/**
 * @tc.name: DrawRegionWithSpecifiedRegionTest005
 * @tc.desc: DrawRegionWithSpecifiedRegion Test
 * @tc.type: FUNC
 * @tc.require: issueI9NDED
 */
HWTEST_F(RSCPUOverdrawCanvasListenerTest, DrawRegionWithSpecifiedRegionTest005, TestSize.Level1)
{
    // The calling function is empty internally
    Drawing::Canvas canvas;
    RSCPUOverdrawCanvasListener canvasListener(canvas);
    Drawing::Brush brush;
    canvasListener.DrawBackground(brush);

    Drawing::Path path;
    Drawing::Point3 planeParams;
    Drawing::Point3 devLightPos;
    Drawing::Color ambientColor;
    Drawing::Color spotColor;
    canvasListener.DrawShadow(
        path, planeParams, devLightPos, SET_XORY1, ambientColor, spotColor, Drawing::ShadowFlags::NONE);

    Drawing::Region region;
    canvasListener.DrawRegion(region);

    Drawing::Picture picture;
    canvasListener.DrawPicture(picture);

    canvasListener.Clear(Drawing::Color::COLOR_BLACK);
}

/**
 * @tc.name: DrawImageRectanglesTest006
 * @tc.desc: DrawImageRectangles Test
 * @tc.type: FUNC
 * @tc.require: issueI9NDED
 */
HWTEST_F(RSCPUOverdrawCanvasListenerTest, DrawImageRectanglesTest006, TestSize.Level1)
{
    Drawing::Canvas canvas;
    RSCPUOverdrawCanvasListener canvasListener(canvas);

    Drawing::Bitmap bitmap;
    canvasListener.DrawBitmap(bitmap, SET_XORY1, SET_XORY1);

    Drawing::Image imageTest1;
    Drawing::SamplingOptions samplingTest1;
    canvasListener.DrawImage(imageTest1, SET_XORY2, SET_XORY2, samplingTest1);

    Drawing::Image imageTest2;
    Drawing::SamplingOptions samplingTest2;
    canvasListener.DrawImageRect(imageTest2, Drawing::Rect(0, 0, SET_TOP, SET_BOTTOM),
        Drawing::Rect(0, 0, SET_TOP, SET_BOTTOM), samplingTest2,
        Drawing::SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT);

    Drawing::Image imageTest3;
    Drawing::SamplingOptions samplingTest3;
    canvasListener.DrawImageRect(imageTest3, Drawing::Rect(0, 0, SET_TOP, SET_BOTTOM), samplingTest3);
}
} // namespace Rosen
} // namespace OHOS
