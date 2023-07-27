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

#define private public

#include "draw/canvas.h"
#include "gtest/gtest.h"
#include "pixel_map.h"
#include "recording/adaptive_image_helper.h"
#include "utils/log.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class AdaptiveImageHelperTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void AdaptiveImageHelperTest::SetUpTestCase() {}
void AdaptiveImageHelperTest::TearDownTestCase() {}
void AdaptiveImageHelperTest::SetUp() {}
void AdaptiveImageHelperTest::TearDown() {}

static bool IsEqualToOriginRect(Rect dstRect)
{
    Rect originRect = {};
    if (originRect == dstRect) {
        return true;
    } else {
        return false;
    }
}

/**
 * @tc.name: GetDstRect001
 * @tc.desc: Test the GetDstRect function.
 * @tc.type: FUNC
 * @tc.require: I7OD2P
 */
HWTEST_F(AdaptiveImageHelperTest, GetDstRect001, TestSize.Level1)
{
    AdaptiveImageInfo rsImageInfo = {};
    rsImageInfo.scale = 1.0;
    rsImageInfo.fitNum = static_cast<int32_t>(ImageFit::FILL);
    Rect dstRect = AdaptiveImageHelper::GetDstRect(rsImageInfo, 4.0, 2.0, 1.0, 1.0);
    EXPECT_FALSE(IsEqualToOriginRect(dstRect));

    rsImageInfo.fitNum = static_cast<int32_t>(ImageFit::CONTAIN);
    dstRect = AdaptiveImageHelper::GetDstRect(rsImageInfo, 4.0, 2.0, 1.0, 1.0);
    EXPECT_FALSE(IsEqualToOriginRect(dstRect));

    rsImageInfo.fitNum = static_cast<int32_t>(ImageFit::COVER);
    dstRect = AdaptiveImageHelper::GetDstRect(rsImageInfo, 4.0, 2.0, 1.0, 1.0);
    EXPECT_FALSE(IsEqualToOriginRect(dstRect));

    rsImageInfo.fitNum = static_cast<int32_t>(ImageFit::FIT_WIDTH);
    dstRect = AdaptiveImageHelper::GetDstRect(rsImageInfo, 4.0, 2.0, 1.0, 1.0);
    EXPECT_FALSE(IsEqualToOriginRect(dstRect));

    rsImageInfo.fitNum = static_cast<int32_t>(ImageFit::FIT_HEIGHT);
    dstRect = AdaptiveImageHelper::GetDstRect(rsImageInfo, 4.0, 2.0, 1.0, 1.0);
    EXPECT_FALSE(IsEqualToOriginRect(dstRect));

    rsImageInfo.fitNum = static_cast<int32_t>(ImageFit::NONE);
    dstRect = AdaptiveImageHelper::GetDstRect(rsImageInfo, 4.0, 2.0, 1.0, 1.0);
    EXPECT_FALSE(IsEqualToOriginRect(dstRect));

    rsImageInfo.fitNum = static_cast<int32_t>(ImageFit::SCALE_DOWN);
    dstRect = AdaptiveImageHelper::GetDstRect(rsImageInfo, 4.0, 2.0, 1.0, 1.0);
    EXPECT_FALSE(IsEqualToOriginRect(dstRect));

    rsImageInfo.fitNum = static_cast<int32_t>(ImageFit::TOP_LEFT);
    dstRect = AdaptiveImageHelper::GetDstRect(rsImageInfo, 4.0, 2.0, 1.0, 1.0);
    EXPECT_FALSE(IsEqualToOriginRect(dstRect));

    rsImageInfo.fitNum = 8;
    dstRect = AdaptiveImageHelper::GetDstRect(rsImageInfo, 4.0, 2.0, 1.0, 1.0);
    EXPECT_FALSE(IsEqualToOriginRect(dstRect));
}

/**
 * @tc.name: GetDstRect002
 * @tc.desc: Test the GetDstRect function.
 * @tc.type: FUNC
 * @tc.require: I7OD2P
 */
HWTEST_F(AdaptiveImageHelperTest, GetDstRect002, TestSize.Level1)
{
    AdaptiveImageInfo rsImageInfo = {};
    rsImageInfo.scale = 0;
    Rect dstRect = AdaptiveImageHelper::GetDstRect(rsImageInfo, 4.0, 2.0, 1.0, 1.0);
    EXPECT_TRUE(IsEqualToOriginRect(dstRect));
}

/**
 * @tc.name: GetDstRect003
 * @tc.desc: Test the GetDstRect function.
 * @tc.type: FUNC
 * @tc.require: I7OD2P
 */
HWTEST_F(AdaptiveImageHelperTest, GetDstRect003, TestSize.Level1)
{
    AdaptiveImageInfo rsImageInfo = {};
    rsImageInfo.scale = 1;
    float srcRectWidth = 0;
    Rect dstRect = AdaptiveImageHelper::GetDstRect(rsImageInfo, srcRectWidth, 2.0, 1.0, 1.0);
    EXPECT_TRUE(IsEqualToOriginRect(dstRect));

    float srcRectHeight = 0;
    dstRect = AdaptiveImageHelper::GetDstRect(rsImageInfo, 2.0, srcRectHeight, 1.0, 1.0);
    EXPECT_TRUE(IsEqualToOriginRect(dstRect));
}

/**
 * @tc.name: ApplyCanvasClip001
 * @tc.desc: Test the ApplyCanvasClip function.
 * @tc.type: FUNC
 * @tc.require: I7OD2P
 */
HWTEST_F(AdaptiveImageHelperTest, ApplyCanvasClip001, TestSize.Level1)
{
    AdaptiveImageInfo rsImageInfo = {};
    Canvas canvas;
    Rect rect;
    rsImageInfo.repeatNum = static_cast<int32_t>(ImageRepeat::NO_REPEAT);
    AdaptiveImageHelper::ApplyCanvasClip(canvas, rect, rsImageInfo, 1.0, 1.0);
}

/**
 * @tc.name: ApplyCanvasClip002
 * @tc.desc: Test the ApplyCanvasClip function.
 * @tc.type: FUNC
 * @tc.require: I7OD2P
 */
HWTEST_F(AdaptiveImageHelperTest, ApplyCanvasClip002, TestSize.Level1)
{
    Canvas canvas;
    Rect rect(2.0, 2.0, 2.0, 2.0);
    AdaptiveImageInfo rsImageInfo = {};
    rsImageInfo.scale = 1.0;
    rsImageInfo.fitNum = static_cast<int32_t>(ImageFit::FILL);
    rsImageInfo.repeatNum = static_cast<int32_t>(ImageRepeat::REPEAT);
    AdaptiveImageHelper::ApplyCanvasClip(canvas, rect, rsImageInfo, 4.0, 2.0);
}

/**
 * @tc.name: DrawImage001
 * @tc.desc: Test the DrawImage function.
 * @tc.type: FUNC
 * @tc.require: I7OD2P
 */
HWTEST_F(AdaptiveImageHelperTest, DrawImage001, TestSize.Level1)
{
    Canvas canvas;
    Rect rect = {};
    std::shared_ptr<Image> image = nullptr;
    AdaptiveImageInfo rsImageInfo = {};
    SamplingOptions smapling;
    AdaptiveImageHelper::DrawImage(canvas, rect, image, rsImageInfo, smapling);
}

/**
 * @tc.name: DrawImage002
 * @tc.desc: Test the DrawImage function.
 * @tc.type: FUNC
 * @tc.require: I7OD2P
 */
HWTEST_F(AdaptiveImageHelperTest, DrawImage002, TestSize.Level1)
{
    Canvas canvas;
    Rect rect = {};
    std::shared_ptr<Data> data = nullptr;
    AdaptiveImageInfo rsImageInfo = {};
    SamplingOptions smapling;
    AdaptiveImageHelper::DrawImage(canvas, rect, data, rsImageInfo, smapling);
}

/**
 * @tc.name: DrawPixelMap001
 * @tc.desc: Test the DrawPixelMap function.
 * @tc.type: FUNC
 * @tc.require: I7OD2P
 */
HWTEST_F(AdaptiveImageHelperTest, DrawPixelMap001, TestSize.Level1)
{
    Canvas canvas;
    Rect rect = {};
    std::shared_ptr<Media::PixelMap> pixelMap = nullptr;
    AdaptiveImageInfo rsImageInfo = {};
    SamplingOptions smapling;
    AdaptiveImageHelper::DrawPixelMap(canvas, rect, pixelMap, rsImageInfo, smapling);
}

/**
 * @tc.name: GetRectCropMultiple001
 * @tc.desc: Test the GetRectCropMultiple function.
 * @tc.type: FUNC
 * @tc.require: I7OD2P
 */
HWTEST_F(AdaptiveImageHelperTest, GetRectCropMultiple001, TestSize.Level1)
{
    Canvas canvas;
    Rect rect(0.01, 0.01, 0.01, 0.01);
    Rect dstRect(0.01, 0.02, 0.01, 0.02);
    int32_t repeatNum = static_cast<int32_t>(ImageRepeat::REPEAT);
    BoundaryRect boundaryRect = {};
    AdaptiveImageHelper::GetRectCropMultiple(rect, repeatNum, dstRect, boundaryRect);
}

/**
 * @tc.name: GetRectCropMultiple002
 * @tc.desc: Test the GetRectCropMultiple function.
 * @tc.type: FUNC
 * @tc.require: I7OD2P
 */
HWTEST_F(AdaptiveImageHelperTest, GetRectCropMultiple002, TestSize.Level1)
{
    Canvas canvas;
    Rect rect(0.01, 0.01, 0.01, 0.01);
    Rect dstRect(0.01, 0.02, 0.01, 0.02);
    int32_t repeatNum = static_cast<int32_t>(ImageRepeat::REPEAT_X);
    BoundaryRect boundaryRect = {};
    AdaptiveImageHelper::GetRectCropMultiple(rect, repeatNum, dstRect, boundaryRect);
}

/**
 * @tc.name: GetRectCropMultiple003
 * @tc.desc: Test the GetRectCropMultiple function.
 * @tc.type: FUNC
 * @tc.require: I7OD2P
 */
HWTEST_F(AdaptiveImageHelperTest, GetRectCropMultiple003, TestSize.Level1)
{
    Canvas canvas;
    Rect rect(0.01, 0.01, 0.01, 0.01);
    Rect dstRect(0.01, 0.02, 0.01, 0.02);
    int32_t repeatNum = static_cast<int32_t>(ImageRepeat::REPEAT_Y);
    BoundaryRect boundaryRect = {};
    AdaptiveImageHelper::GetRectCropMultiple(rect, repeatNum, dstRect, boundaryRect);
}

/**
 * @tc.name: DrawImageRepeatRect001
 * @tc.desc: Test the DrawImageRepeatRect function.
 * @tc.type: FUNC
 * @tc.require: I7OD2P
 */
HWTEST_F(AdaptiveImageHelperTest, DrawImageRepeatRect001, TestSize.Level1)
{
    Canvas canvas;
    Rect rect(4.0, 4.0, 2.0, 2.0);
    Bitmap bmp;
    BitmapFormat format { COLORTYPE_RGBA_8888, ALPHATYPE_OPAQUE };
    bmp.Build(10, 10, format);
    Image image;
    image.BuildFromBitmap(bmp);
    auto image1 = std::make_shared<Image>(image);
    AdaptiveImageInfo rsImageInfo = {};
    SamplingOptions smapling;
    rsImageInfo.scale = 1.0;
    rsImageInfo.fitNum = static_cast<int32_t>(ImageFit::FILL);
    rsImageInfo.repeatNum = static_cast<int32_t>(ImageRepeat::REPEAT);
    AdaptiveImageHelper::DrawImageRepeatRect(canvas, rect, image1, rsImageInfo, smapling);
}

/**
 * @tc.name: DrawImageRepeatRect002
 * @tc.desc: Test the DrawImageRepeatRect function.
 * @tc.type: FUNC
 * @tc.require: I7OD2P
 */
HWTEST_F(AdaptiveImageHelperTest, DrawImageRepeatRect002, TestSize.Level1)
{
    Canvas canvas;
    Rect rect = {};
    Bitmap bmp;
    BitmapFormat format { COLORTYPE_RGBA_8888, ALPHATYPE_OPAQUE };
    bmp.Build(10, 10, format);
    Image image;
    image.BuildFromBitmap(bmp);
    auto image1 = std::make_shared<Image>(image);
    AdaptiveImageInfo rsImageInfo = {};
    SamplingOptions smapling;
    rsImageInfo.scale = 0;
    AdaptiveImageHelper::DrawImageRepeatRect(canvas, rect, image1, rsImageInfo, smapling);
}

/**
 * @tc.name: DrawImageRepeatRect003
 * @tc.desc: Test the DrawImageRepeatRect function.
 * @tc.type: FUNC
 * @tc.require: I7OD2P
 */
HWTEST_F(AdaptiveImageHelperTest, DrawImageRepeatRect003, TestSize.Level1)
{
    Canvas canvas;
    Rect rect(4.0, 4.0, 2.0, 2.0);
    std::shared_ptr<Data> data = nullptr;
    AdaptiveImageInfo rsImageInfo = {};
    SamplingOptions smapling;
    rsImageInfo.scale = 1.0;
    rsImageInfo.fitNum = static_cast<int32_t>(ImageFit::FILL);
    rsImageInfo.repeatNum = static_cast<int32_t>(ImageRepeat::REPEAT);

    AdaptiveImageHelper::DrawImageRepeatRect(canvas, rect, data, rsImageInfo, smapling);
}

/**
 * @tc.name: DrawImageRepeatRect004
 * @tc.desc: Test the DrawImageRepeatRect function.
 * @tc.type: FUNC
 * @tc.require: I7OD2P
 */
HWTEST_F(AdaptiveImageHelperTest, DrawImageRepeatRect004, TestSize.Level1)
{
    Canvas canvas;
    Rect rect = {};
    std::shared_ptr<Data> data = nullptr;
    AdaptiveImageInfo rsImageInfo = {};
    SamplingOptions smapling;
    rsImageInfo.scale = 0;
    AdaptiveImageHelper::DrawImageRepeatRect(canvas, rect, data, rsImageInfo, smapling);
}

/**
 * @tc.name: DrawPixelMapRepeatRect001
 * @tc.desc: Test the DrawPixelMapRepeatRect function.
 * @tc.type: FUNC
 * @tc.require: I7OD2P
 */
HWTEST_F(AdaptiveImageHelperTest, DrawPixelMapRepeatRect001, TestSize.Level1)
{
    Canvas canvas;
    Rect rect(4.0, 4.0, 2.0, 2.0);
    Media::InitializationOptions opts;
    opts.size.width = 200;
    opts.size.height = 150;
    opts.editable = true;
    std::shared_ptr<Media::PixelMap> pixelMap = Media::PixelMap::Create(opts);
    AdaptiveImageInfo rsImageInfo = {};
    SamplingOptions smapling;
    rsImageInfo.scale = 1.0;
    rsImageInfo.fitNum = static_cast<int32_t>(ImageFit::FILL);
    rsImageInfo.repeatNum = static_cast<int32_t>(ImageRepeat::REPEAT);
    AdaptiveImageHelper::DrawPixelMapRepeatRect(canvas, rect, pixelMap, rsImageInfo, smapling);
}

/**
 * @tc.name: DrawPixelMapRepeatRect002
 * @tc.desc: Test the DrawPixelMapRepeatRect function.
 * @tc.type: FUNC
 * @tc.require: I7OD2P
 */
HWTEST_F(AdaptiveImageHelperTest, DrawPixelMapRepeatRect002, TestSize.Level1)
{
    Canvas canvas;
    Rect rect = {};
    Media::InitializationOptions opts;
    opts.size.width = 200;
    opts.size.height = 150;
    opts.editable = true;
    std::shared_ptr<Media::PixelMap> pixelMap = Media::PixelMap::Create(opts);
    AdaptiveImageInfo rsImageInfo = {};
    SamplingOptions smapling;
    rsImageInfo.scale = 0;
    AdaptiveImageHelper::DrawPixelMapRepeatRect(canvas, rect, pixelMap, rsImageInfo, smapling);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
