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
 * @tc.name: IsBlackOrWhiteOrGrayColorTest001
 * @tc.desc: Verify function IsBlackOrWhiteOrGrayColor
 * @tc.type:FUNC
 */
HWTEST_F(RSColorPickerTest, IsBlackOrWhiteOrGrayColorTest001, TestSize.Level1)
{
    // for test
    EXPECT_TRUE(picker->IsBlackOrWhiteOrGrayColor(0x00000001));
    EXPECT_FALSE(picker->IsBlackOrWhiteOrGrayColor(0x0000ffff));
    EXPECT_TRUE(picker->IsBlackOrWhiteOrGrayColor(0x00ffffff));
    EXPECT_FALSE(picker->IsBlackOrWhiteOrGrayColor(0x000000ff));
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
 * @tc.name: IsEqualsTest002
 * @tc.desc: Verify function IsEquals
 * @tc.type:FUNC
 */
HWTEST_F(RSColorPickerTest, IsEqualsTest002, TestSize.Level1)
{
    EXPECT_TRUE(picker->IsEquals(1.0, 1.0));
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

/**
 * @tc.name: GetAverageColorDirectTest001
 * @tc.desc: Verify function GetAverageColorDirect with null pixmap
 * @tc.type:FUNC
 */
HWTEST_F(RSColorPickerTest, GetAverageColorDirectTest001, TestSize.Level1)
{
    Drawing::ColorQuad color;
    std::shared_ptr<Drawing::Pixmap> nullPixmap = nullptr;
    EXPECT_EQ(RSColorPicker::GetAverageColorDirect(nullPixmap, color), RS_COLOR_PICKER_ERR_EFFECT_INVALID_VALUE);
}

/**
 * @tc.name: GetAverageColorDirectTest002
 * @tc.desc: Verify function GetAverageColorDirect with invalid dimensions
 * @tc.type:FUNC
 */
HWTEST_F(RSColorPickerTest, GetAverageColorDirectTest002, TestSize.Level1)
{
    Drawing::ColorQuad color;
    Drawing::ImageInfo imageInfo(
        0, 0, Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_PREMUL, nullptr);
    int addr = 1;
    size_t rowBytes = 1;
    auto invalidPixmap = std::make_shared<Drawing::Pixmap>(imageInfo, &addr, rowBytes);
    EXPECT_EQ(RSColorPicker::GetAverageColorDirect(invalidPixmap, color), RS_COLOR_PICKER_ERR_EFFECT_INVALID_VALUE);
}

/**
 * @tc.name: GetAverageColorDirectTest003
 * @tc.desc: Verify function GetAverageColorDirect with pure yellow color (0xFFFF00)
 * @tc.type:FUNC
 */
HWTEST_F(RSColorPickerTest, GetAverageColorDirectTest003, TestSize.Level1)
{
    Drawing::ColorQuad color;
    constexpr int width = 10;
    constexpr int height = 10;

    // Create a pixmap filled with pure yellow (0xFFFF00)
    Drawing::ImageInfo imageInfo(
        width, height, Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_PREMUL, nullptr);
    std::vector<uint32_t> pixels(width * height, 0xFFFFFF00); // ARGB: 0xFF (A), 0xFF (R), 0xFF (G), 0x00 (B)
    auto yellowPixmap = std::make_shared<Drawing::Pixmap>(imageInfo, pixels.data(), width * sizeof(uint32_t));

    EXPECT_EQ(RSColorPicker::GetAverageColorDirect(yellowPixmap, color), RS_COLOR_PICKER_SUCCESS);

    // Verify the color is exactly 0xFFFFFF00 (no quantization loss)
    EXPECT_EQ(color, 0xFFFFFF00);
    EXPECT_EQ(Drawing::Color::ColorQuadGetA(color), 0xFF);
    EXPECT_EQ(Drawing::Color::ColorQuadGetR(color), 0xFF);
    EXPECT_EQ(Drawing::Color::ColorQuadGetG(color), 0xFF);
    EXPECT_EQ(Drawing::Color::ColorQuadGetB(color), 0x00);
}

/**
 * @tc.name: GetAverageColorDirectTest004
 * @tc.desc: Verify function GetAverageColorDirect with mixed colors
 * @tc.type:FUNC
 */
HWTEST_F(RSColorPickerTest, GetAverageColorDirectTest004, TestSize.Level1)
{
    Drawing::ColorQuad color;
    constexpr int width = 2;
    constexpr int height = 2;

    // Create a 2x2 pixmap with different colors
    Drawing::ImageInfo imageInfo(
        width, height, Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_PREMUL, nullptr);
    std::vector<uint32_t> pixels = {
        0xFFFF0000, // Red
        0xFF00FF00, // Green
        0xFF0000FF, // Blue
        0xFFFFFFFF  // White
    };
    auto mixedPixmap = std::make_shared<Drawing::Pixmap>(imageInfo, pixels.data(), width * sizeof(uint32_t));

    EXPECT_EQ(RSColorPicker::GetAverageColorDirect(mixedPixmap, color), RS_COLOR_PICKER_SUCCESS);

    // Average: R=(255+0+0+255)/4=127, G=(0+255+0+255)/4=127, B=(0+0+255+255)/4=127
    EXPECT_EQ(Drawing::Color::ColorQuadGetA(color), 0xFF);
    EXPECT_EQ(Drawing::Color::ColorQuadGetR(color), 127);
    EXPECT_EQ(Drawing::Color::ColorQuadGetG(color), 127);
    EXPECT_EQ(Drawing::Color::ColorQuadGetB(color), 127);
}

/**
 * @tc.name: GetAverageColorDirectTest005
 * @tc.desc: Verify function GetAverageColorDirect with size limit
 * @tc.type:FUNC
 */
HWTEST_F(RSColorPickerTest, GetAverageColorDirectTest005, TestSize.Level1)
{
    Drawing::ColorQuad color;
    constexpr int width = 2000;
    constexpr int height = 2000; // 4M pixels, exceeds 1M limit

    // Create a large pixmap that exceeds the size limit
    Drawing::ImageInfo imageInfo(
        width, height, Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_PREMUL, nullptr);
    std::vector<uint32_t> pixels(width * height, 0xFFFFFFFF);
    auto largePixmap = std::make_shared<Drawing::Pixmap>(imageInfo, pixels.data(), width * sizeof(uint32_t));

    // Should fail due to size limit
    EXPECT_EQ(RSColorPicker::GetAverageColorDirect(largePixmap, color), RS_COLOR_PICKER_ERR_EFFECT_INVALID_VALUE);
}

/**
 * @tc.name: GetAverageColorDirectTest006
 * @tc.desc: Verify function GetAverageColorDirect with maximum allowed size
 * @tc.type:FUNC
 */
HWTEST_F(RSColorPickerTest, GetAverageColorDirectTest006, TestSize.Level1)
{
    Drawing::ColorQuad color;
    constexpr int width = 1000;
    constexpr int height = 1000; // Exactly 1M pixels, at the limit

    // Create a pixmap at the maximum allowed size
    Drawing::ImageInfo imageInfo(
        width, height, Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_PREMUL, nullptr);
    std::vector<uint32_t> pixels(width * height, 0xFF808080); // Gray
    auto maxPixmap = std::make_shared<Drawing::Pixmap>(imageInfo, pixels.data(), width * sizeof(uint32_t));

    // Should succeed at exactly the limit
    EXPECT_EQ(RSColorPicker::GetAverageColorDirect(maxPixmap, color), RS_COLOR_PICKER_SUCCESS);
    EXPECT_EQ(Drawing::Color::ColorQuadGetR(color), 0x80);
    EXPECT_EQ(Drawing::Color::ColorQuadGetG(color), 0x80);
    EXPECT_EQ(Drawing::Color::ColorQuadGetB(color), 0x80);
}

/**
 * @tc.name: GetAverageColorDirectTest007
 * @tc.desc: Verify function GetAverageColorDirect preserves full precision (no quantization)
 * @tc.type:FUNC
 */
HWTEST_F(RSColorPickerTest, GetAverageColorDirectTest007, TestSize.Level1)
{
    Drawing::ColorQuad color;
    constexpr int width = 10;
    constexpr int height = 10;

    // Create a pixmap with color 0xFFF8F800 (the buggy quantized yellow)
    Drawing::ImageInfo imageInfo(
        width, height, Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_PREMUL, nullptr);
    std::vector<uint32_t> pixels(width * height, 0xFFF8F800);
    auto pixmap248 = std::make_shared<Drawing::Pixmap>(imageInfo, pixels.data(), width * sizeof(uint32_t));

    EXPECT_EQ(RSColorPicker::GetAverageColorDirect(pixmap248, color), RS_COLOR_PICKER_SUCCESS);

    // Should preserve 0xF8 exactly, not convert to 0xFF
    EXPECT_EQ(Drawing::Color::ColorQuadGetR(color), 0xF8);
    EXPECT_EQ(Drawing::Color::ColorQuadGetG(color), 0xF8);
    EXPECT_EQ(Drawing::Color::ColorQuadGetB(color), 0x00);
}
} // namespace OHOS::Rosen
