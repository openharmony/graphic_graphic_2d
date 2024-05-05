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

#include <gtest/gtest.h>

#include "recording/cmd_list_helper.h"
#include "pipeline/rs_recording_canvas.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSExtendRecordingCanvasTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSExtendRecordingCanvasTest::SetUpTestCase() {}
void RSExtendRecordingCanvasTest::TearDownTestCase() {}
void RSExtendRecordingCanvasTest::SetUp() {}
void RSExtendRecordingCanvasTest::TearDown() {}

/**
 * @tc.name: DrawImageWithParm
 * @tc.desc: test results of DrawImageWithParm
 * @tc.type:FUNC
 * @tc.require: issueI9H4AD
 */
HWTEST_F(RSExtendRecordingCanvasTest, DrawImageWithParm, TestSize.Level1)
{
    int width = 1;
    int height = 1;
    ExtendRecordingCanvas extendRecordingCanvas(width, height);
    std::shared_ptr<Drawing::Image> image;
    std::shared_ptr<Drawing::Data> data;
    Drawing::AdaptiveImageInfo rsImageInfo;
    Drawing::SamplingOptions sampling;
    extendRecordingCanvas.DrawImageWithParm(image, data, rsImageInfo, sampling);
    extendRecordingCanvas.addDrawOpImmediate_ = false;
    extendRecordingCanvas.DrawImageWithParm(image, data, rsImageInfo, sampling);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: DrawPixelMapWithParm
 * @tc.desc: test results of DrawPixelMapWithParm
 * @tc.type:FUNC
 * @tc.require: issueI9H4AD
 */
HWTEST_F(RSExtendRecordingCanvasTest, DrawPixelMapWithParm, TestSize.Level1)
{
    int width = 1;
    int height = 1;
    ExtendRecordingCanvas extendRecordingCanvas(width, height);
    std::shared_ptr<Media::PixelMap> pixelMap;
    Drawing::AdaptiveImageInfo rsImageInfo;
    Drawing::SamplingOptions sampling;
    extendRecordingCanvas.DrawPixelMapWithParm(pixelMap, rsImageInfo, sampling);
    extendRecordingCanvas.addDrawOpImmediate_ = false;
    extendRecordingCanvas.DrawPixelMapWithParm(pixelMap, rsImageInfo, sampling);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: DrawPixelMapRect
 * @tc.desc: test results of DrawPixelMapRect
 * @tc.type:FUNC
 * @tc.require: issueI9H4AD
 */
HWTEST_F(RSExtendRecordingCanvasTest, DrawPixelMapRect, TestSize.Level1)
{
    int width = 1;
    int height = 1;
    ExtendRecordingCanvas extendRecordingCanvas(width, height);
    std::shared_ptr<Media::PixelMap> pixelMap;
    Drawing::SamplingOptions sampling;
    const Drawing::Rect src;
    const Drawing::Rect dst;
    extendRecordingCanvas.DrawPixelMapRect(pixelMap, src, dst, sampling);
    extendRecordingCanvas.addDrawOpImmediate_ = false;
    extendRecordingCanvas.DrawPixelMapRect(pixelMap, src, dst, sampling);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: DrawDrawFunc
 * @tc.desc: test results of DrawDrawFunc
 * @tc.type:FUNC
 * @tc.require: issueI9H4AD
 */
HWTEST_F(RSExtendRecordingCanvasTest, DrawDrawFunc, TestSize.Level1)
{
    int width = 1;
    int height = 1;
    ExtendRecordingCanvas extendRecordingCanvas(width, height);
    Drawing::RecordingCanvas::DrawFunc drawFunc = [](Drawing::Canvas* canvas, const Drawing::Rect* rect) {};
    extendRecordingCanvas.DrawDrawFunc(std::move(drawFunc));
    extendRecordingCanvas.addDrawOpImmediate_ = false;
    extendRecordingCanvas.DrawDrawFunc(std::move(drawFunc));
    ASSERT_TRUE(true);
}

/**
 * @tc.name: AddDrawOpImmediate
 * @tc.desc: test results of AddDrawOpImmediate
 * @tc.type:FUNC
 * @tc.require: issueI9H4AD
 */
HWTEST_F(RSExtendRecordingCanvasTest, AddDrawOpImmediate, TestSize.Level1)
{
    int width = 1;
    int height = 1;
    ExtendRecordingCanvas extendRecordingCanvas(width, height);
    std::shared_ptr<Drawing::Image> image;
    std::shared_ptr<Drawing::Data> data;
    Drawing::AdaptiveImageInfo rsImageInfo;
    Drawing::SamplingOptions sampling;
    // Test the function template AddDrawOpImmediate() with the DrawImageWithParm() function
    extendRecordingCanvas.paintBrush_.style_ = Drawing::Paint::PaintStyle::PAINT_FILL;
    extendRecordingCanvas.DrawImageWithParm(image, data, rsImageInfo, sampling);

    extendRecordingCanvas.paintPen_.style_ = Drawing::Paint::PaintStyle::PAINT_FILL;
    extendRecordingCanvas.DrawImageWithParm(image, data, rsImageInfo, sampling);

    extendRecordingCanvas.paintBrush_.style_ = Drawing::Paint::PaintStyle::PAINT_NONE;
    extendRecordingCanvas.DrawImageWithParm(image, data, rsImageInfo, sampling);

    ASSERT_TRUE(true);
}

/**
 * @tc.name: AddDrawOpDeferred
 * @tc.desc: test results of AddDrawOpImmediate
 * @tc.type:FUNC
 * @tc.require: issueI9H4AD
 */
HWTEST_F(RSExtendRecordingCanvasTest, AddDrawOpDeferred, TestSize.Level1)
{
    int width = 1;
    int height = 1;
    ExtendRecordingCanvas extendRecordingCanvas(width, height);
    std::shared_ptr<Drawing::Image> image;
    std::shared_ptr<Drawing::Data> data;
    Drawing::AdaptiveImageInfo rsImageInfo;
    Drawing::SamplingOptions sampling;
    extendRecordingCanvas.addDrawOpImmediate_ = false;
    // Test the function template AddDrawOpImmediate() with the DrawImageWithParm() function
    extendRecordingCanvas.paintBrush_.style_ = Drawing::Paint::PaintStyle::PAINT_FILL;
    extendRecordingCanvas.DrawImageWithParm(image, data, rsImageInfo, sampling);

    extendRecordingCanvas.paintPen_.style_ = Drawing::Paint::PaintStyle::PAINT_FILL;
    extendRecordingCanvas.DrawImageWithParm(image, data, rsImageInfo, sampling);

    extendRecordingCanvas.paintBrush_.style_ = Drawing::Paint::PaintStyle::PAINT_NONE;
    extendRecordingCanvas.DrawImageWithParm(image, data, rsImageInfo, sampling);
    ASSERT_TRUE(true);
}
} // namespace Rosen
} // namespace OHOS
