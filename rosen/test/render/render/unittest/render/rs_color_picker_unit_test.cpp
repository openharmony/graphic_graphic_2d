/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include "include/render/rs_color_picker.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSColorPickerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    std::shared_ptr<Drawing::Pixmap> pixmap = nullptr;
    std::shared_ptr<RSColorPicker> picker;
};

void RSColorPickerTest::SetUpTestCase() {}
void RSColorPickerTest::TearDownTestCase() {}
void RSColorPickerTest::SetUp()
{
    picker = std::make_shared<RSColorPicker>(pixmap);
}
void RSColorPickerTest::TearDown()
{
    picker.reset();
}

/**
 * @tc.name: CreateColorPickerTest
 * @tc.desc: Verify function CreateColorPicker
 * @tc.type:FUNC
 * @tc.require: issueI9I9D1
 */
HWTEST_F(RSColorPickerTest, CreateColorPickerTest, TestSize.Level1)
{
    uint32_t errorCode = 0;
    EXPECT_EQ(picker->CreateColorPicker(pixmap, errorCode), nullptr);
    Drawing::ImageInfo imageInfo(
        1, 1, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_UNKNOWN, nullptr);
    int addr = 1;
    size_t rowBytes = 1;
    pixmap = std::make_shared<Drawing::Pixmap>(imageInfo, &addr, rowBytes);
    auto picker2 = std::make_shared<RSColorPicker>(pixmap);
    EXPECT_NE(picker2->CreateColorPicker(pixmap, errorCode), nullptr);
}

/**
 * @tc.name: CreateColorPickerTest002
 * @tc.desc: Verify function CreateColorPicker
 * @tc.type:FUNC
 * @tc.require: issueI9I9D1
 */
HWTEST_F(RSColorPickerTest, CreateColorPickerTest002, TestSize.Level1)
{
    uint32_t errorCode = 0;
    double coordinates = 1.0;
    EXPECT_EQ(picker->CreateColorPicker(pixmap, &coordinates, errorCode), nullptr);
    Drawing::ImageInfo imageInfo(
        1, 1, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_UNKNOWN, nullptr);
    int addr = 1;
    size_t rowBytes = 1;
    pixmap = std::make_shared<Drawing::Pixmap>(imageInfo, &addr, rowBytes);
    auto picker2 = std::make_shared<RSColorPicker>(pixmap);
    EXPECT_NE(picker2->CreateColorPicker(pixmap, &coordinates, errorCode), nullptr);
}

/**
 * @tc.name: GetLargestProportionColorTest
 * @tc.desc: Verify function GetLargestProportionColor
 * @tc.type:FUNC
 * @tc.require: issueI9I9D1
 */
HWTEST_F(RSColorPickerTest, GetLargestProportionColorTest, TestSize.Level1)
{
    Drawing::ColorQuad color;
    EXPECT_EQ(picker->GetLargestProportionColor(color), 2);
    Drawing::ImageInfo imageInfo(
        1, 1, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_UNKNOWN, nullptr);
    int addr = 1;
    size_t rowBytes = 1;
    pixmap = std::make_shared<Drawing::Pixmap>(imageInfo, &addr, rowBytes);
    auto picker2 = std::make_shared<RSColorPicker>(pixmap);
    picker2->GetNFeatureColors(1);
    EXPECT_EQ(picker2->GetLargestProportionColor(color), 0);
}

/**
 * @tc.name: GetHighestSaturationColorTest
 * @tc.desc: Verify function GetHighestSaturationColor
 * @tc.type:FUNC
 * @tc.require: issueI9I9D1
 */
HWTEST_F(RSColorPickerTest, GetHighestSaturationColorTest, TestSize.Level1)
{
    Drawing::ColorQuad color;
    // for test
    EXPECT_EQ(picker->GetHighestSaturationColor(color), 2);
    Drawing::ImageInfo imageInfo(
        1, 1, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_UNKNOWN, nullptr);
    int addr = 1;
    size_t rowBytes = 1;
    pixmap = std::make_shared<Drawing::Pixmap>(imageInfo, &addr, rowBytes);
    auto picker2 = std::make_shared<RSColorPicker>(pixmap);
    picker2->GetNFeatureColors(1);
    EXPECT_EQ(picker2->GetHighestSaturationColor(color), 0);
}

/**
 * @tc.name: GetAverageColorTest
 * @tc.desc: Verify function GetAverageColor
 * @tc.type:FUNC
 * @tc.require: issueI9I9D1
 */
HWTEST_F(RSColorPickerTest, GetAverageColorTest, TestSize.Level1)
{
    Drawing::ColorQuad color;
    EXPECT_EQ(picker->GetAverageColor(color), RS_COLOR_PICKER_ERR_EFFECT_INVALID_VALUE);
    Drawing::ImageInfo imageInfo(
        1, 1, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_UNKNOWN, nullptr);
    int addr = 1;
    size_t rowBytes = 1;
    pixmap = std::make_shared<Drawing::Pixmap>(imageInfo, &addr, rowBytes);
    auto picker2 = std::make_shared<RSColorPicker>(pixmap);
    picker2->GetNFeatureColors(1);
    EXPECT_EQ(picker2->GetAverageColor(color), RS_COLOR_PICKER_SUCCESS);
}

/**
 * @tc.name: IsBlackOrWhiteOrGrayColorTest
 * @tc.desc: Verify function IsBlackOrWhiteOrGrayColor
 * @tc.type:FUNC
 * @tc.require: issueI9I9D1
 */
HWTEST_F(RSColorPickerTest, IsBlackOrWhiteOrGrayColorTest, TestSize.Level1)
{
    // for test
    EXPECT_FALSE(picker->IsBlackOrWhiteOrGrayColor(255));
    EXPECT_TRUE(picker->IsBlackOrWhiteOrGrayColor(1));
}

/**
 * @tc.name: IsEqualsTest
 * @tc.desc: Verify function IsEquals
 * @tc.type:FUNC
 * @tc.require: issueI9I9D1
 */
HWTEST_F(RSColorPickerTest, IsEqualsTest, TestSize.Level1)
{
    EXPECT_FALSE(picker->IsEquals(0.0, 1.0));
}

/**
 * @tc.name: RGB2HSVTest
 * @tc.desc: Verify function RGB2HSV
 * @tc.type:FUNC
 * @tc.require: issueI9I9D1
 */
HWTEST_F(RSColorPickerTest, RGB2HSVTest, TestSize.Level1)
{
    HSV hsv = picker->RGB2HSV(0);
    EXPECT_EQ(hsv.s, 0);
    hsv = picker->RGB2HSV(1);
    // for test
    EXPECT_EQ(hsv.s, 100);
}

/**
 * @tc.name: AdjustHSVToDefinedItervalTest
 * @tc.desc: Verify function AdjustHSVToDefinedIterval
 * @tc.type:FUNC
 * @tc.require: issueI9I9D1
 */
HWTEST_F(RSColorPickerTest, AdjustHSVToDefinedItervalTest, TestSize.Level1)
{
    HSV hsv;
    // for test
    hsv.h = 361;
    hsv.s = 101;
    hsv.v = 101;
    picker->AdjustHSVToDefinedIterval(hsv);
    hsv.h = -1;
    hsv.s = -1;
    hsv.v = -1;
    picker->AdjustHSVToDefinedIterval(hsv);
    EXPECT_EQ(hsv.v, 0);
}

/**
 * @tc.name: HSVtoRGBTest
 * @tc.desc: Verify function HSVtoRGB
 * @tc.type:FUNC
 * @tc.require: issueI9I9D1
 */
HWTEST_F(RSColorPickerTest, HSVtoRGBTest, TestSize.Level1)
{
    HSV hsv;
    hsv.h = -1;
    hsv.s = -1;
    hsv.v = -1;
    EXPECT_EQ(picker->HSVtoRGB(hsv), 0);
    // for test
    hsv.h = 60;
    EXPECT_EQ(picker->HSVtoRGB(hsv), 0);
    // for test
    hsv.h = 120;
    EXPECT_EQ(picker->HSVtoRGB(hsv), 0);
    // for test
    hsv.h = 180;
    EXPECT_EQ(picker->HSVtoRGB(hsv), 0);
    // for test
    hsv.h = 240;
    EXPECT_EQ(picker->HSVtoRGB(hsv), 0);
    // for test
    hsv.h = 300;
    EXPECT_EQ(picker->HSVtoRGB(hsv), 0);
}
} // namespace OHOS::Rosen
