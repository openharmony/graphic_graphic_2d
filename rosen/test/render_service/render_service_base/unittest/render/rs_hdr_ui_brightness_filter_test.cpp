/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "skia_image.h"
#include "skia_image_info.h"
#include "draw/canvas.h"
#include "draw/color.h"
#include "draw/surface.h"
#include "image/image_info.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "render/rs_hdr_ui_brightness_filter.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {

class RSHDRUIBrightnessFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSHDRUIBrightnessFilterTest::SetUpTestCase() {}
void RSHDRUIBrightnessFilterTest::TearDownTestCase() {}
void RSHDRUIBrightnessFilterTest::SetUp() {}
void RSHDRUIBrightnessFilterTest::TearDown() {}

/**
 * @tc.name: GetDescription
 * @tc.desc: test GetDescription
 * @tc.type: FUNC
 */
HWTEST_F(RSHDRUIBrightnessFilterTest, GetDescriptionTest, TestSize.Level1)
{
    RSHDRUIBrightnessFilter hdrUIBrightnessFilter(1.0f);
    std::string description = hdrUIBrightnessFilter.GetDescription();
    std::string result = "RSHDRUIBrightnessFilter " + std::to_string(hdrUIBrightnessFilter.GetHDRUIBrightness());
    EXPECT_EQ(description, result);
}

/**
 * @tc.name: GetHDRUIBrightness
 * @tc.desc: test GetHDRUIBrightness
 * @tc.type: FUNC
 */
HWTEST_F(RSHDRUIBrightnessFilterTest, GetHDRUIBrightnessTest, TestSize.Level1)
{
    RSHDRUIBrightnessFilter hdrUIBrightnessFilter(1.0f);
    EXPECT_EQ(hdrUIBrightnessFilter.GetHDRUIBrightness(), 1.0f);

    float hdrBrightness = 2.0f; // hdr brightness
    hdrUIBrightnessFilter.hdrUIBrightness_ = hdrBrightness;
    EXPECT_EQ(hdrUIBrightnessFilter.GetHDRUIBrightness(), hdrBrightness);
}

/**
 * @tc.name: DrawImageRect001
 * @tc.desc: test results of DrawImageRect
 * @tc.type: FUNC
 */
HWTEST_F(RSHDRUIBrightnessFilterTest, DrawImageRect001, TestSize.Level1)
{
    RSHDRUIBrightnessFilter hdrUIBrightnessFilter(1.0f);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    Drawing::Rect src;
    Drawing::Rect dst;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    hdrUIBrightnessFilter.hdrUIBrightness_ = 1.5f; // hdr brightness
    image = std::make_shared<Drawing::Image>();
    paintFilterCanvas.SetHdrOn(false);
    hdrUIBrightnessFilter.DrawImageRect(paintFilterCanvas, image, src, dst);
    EXPECT_TRUE(image);
}
} // namespace Rosen
} // namespace OHOS