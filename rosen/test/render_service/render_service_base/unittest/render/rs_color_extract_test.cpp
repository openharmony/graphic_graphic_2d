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
#include "include/render/rs_color_extract.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSColorExtractTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    std::shared_ptr<Drawing::Pixmap> pixmap = nullptr;
    std::shared_ptr<RSColorExtract> extract;
};

void RSColorExtractTest::SetUpTestCase() {}
void RSColorExtractTest::TearDownTestCase() {}
void RSColorExtractTest::SetUp()
{
    extract = std::make_shared<RSColorExtract>(pixmap);
}
void RSColorExtractTest::TearDown()
{
    extract.reset();
}

/**
 * @tc.name: QuantizedRedTest
 * @tc.desc: Verify function QuantizedRed
 * @tc.type:FUNC
 * @tc.require: issueI9I9D1
 */
HWTEST_F(RSColorExtractTest, QuantizedRedTest, TestSize.Level1)
{
    auto pixmapTest = std::make_shared<Drawing::Pixmap>();
    auto extractTest = std::make_shared<RSColorExtract>(pixmapTest);
    EXPECT_EQ(extractTest->QuantizedRed(0), 0);
}

/**
 * @tc.name: QuantizedGreenTest
 * @tc.desc: Verify function QuantizedGreen
 * @tc.type:FUNC
 * @tc.require: issueI9I9D1
 */
HWTEST_F(RSColorExtractTest, QuantizedGreenTest, TestSize.Level1)
{
    EXPECT_EQ(extract->QuantizedGreen(0), 0);
}

/**
 * @tc.name: QuantizedBlueTest
 * @tc.desc: Verify function QuantizedBlue
 * @tc.type:FUNC
 * @tc.require: issueI9I9D1
 */
HWTEST_F(RSColorExtractTest, QuantizedBlueTest, TestSize.Level1)
{
    EXPECT_EQ(extract->QuantizedBlue(0), 0);
}

/**
 * @tc.name: ModifyWordWidthTest
 * @tc.desc: Verify function ModifyWordWidth
 * @tc.type:FUNC
 * @tc.require: issueI9I9D1
 */
HWTEST_F(RSColorExtractTest, ModifyWordWidthTest, TestSize.Level1)
{
    EXPECT_EQ(extract->ModifyWordWidth(0, 0, 1), 0);
    EXPECT_EQ(extract->ModifyWordWidth(0, 1, 0), 0);
}

/**
 * @tc.name: GetARGB32ColorRTest
 * @tc.desc: Verify function GetARGB32ColorR
 * @tc.type:FUNC
 * @tc.require: issueI9I9D1
 */
HWTEST_F(RSColorExtractTest, GetARGB32ColorRTest, TestSize.Level1)
{
    EXPECT_EQ(extract->GetARGB32ColorR(0), 0);
}

/**
 * @tc.name: GetARGB32ColorGTest
 * @tc.desc: Verify function GetARGB32ColorG
 * @tc.type:FUNC
 * @tc.require: issueI9I9D1
 */
HWTEST_F(RSColorExtractTest, GetARGB32ColorGTest, TestSize.Level1)
{
    EXPECT_EQ(extract->GetARGB32ColorG(0), 0);
}

/**
 * @tc.name: GetARGB32ColorBTest
 * @tc.desc: Verify function GetARGB32ColorB
 * @tc.type:FUNC
 * @tc.require: issueI9I9D1
 */
HWTEST_F(RSColorExtractTest, GetARGB32ColorBTest, TestSize.Level1)
{
    EXPECT_EQ(extract->GetARGB32ColorB(0), 0);
}

/**
 * @tc.name: QuantizeFromRGB888Test
 * @tc.desc: Verify function QuantizeFromRGB888
 * @tc.type:FUNC
 * @tc.require: issueI9I9D1
 */
HWTEST_F(RSColorExtractTest, QuantizeFromRGB888Test, TestSize.Level1)
{
    EXPECT_EQ(extract->QuantizeFromRGB888(0), 0);
}

/**
 * @tc.name: ApproximateToRGB888Test
 * @tc.desc: Verify function ApproximateToRGB888
 * @tc.type:FUNC
 * @tc.require: issueI9I9D1
 */
HWTEST_F(RSColorExtractTest, ApproximateToRGB888Test, TestSize.Level1)
{
    EXPECT_EQ(extract->ApproximateToRGB888(0, 0, 0), 0);
}

/**
 * @tc.name: ApproximateToRGB888Test002
 * @tc.desc: Verify function ApproximateToRGB888
 * @tc.type:FUNC
 * @tc.require: issueI9I9D1
 */
HWTEST_F(RSColorExtractTest, ApproximateToRGB888Test002, TestSize.Level1)
{
    EXPECT_EQ(extract->ApproximateToRGB888(0), 0);
}

/**
 * @tc.name: QuantizePixelsTest
 * @tc.desc: Verify function QuantizePixels
 * @tc.type:FUNC
 * @tc.require: issueI9I9D1
 */
HWTEST_F(RSColorExtractTest, QuantizePixelsTest, TestSize.Level1)
{
    EXPECT_FALSE(extract->QuantizePixels(0).empty());
}

/**
 * @tc.name: SplitBoxesTest
 * @tc.desc: Verify function SplitBoxes
 * @tc.type:FUNC
 * @tc.require: issueI9I9D1
 */
HWTEST_F(RSColorExtractTest, SplitBoxesTest, TestSize.Level1)
{
    std::priority_queue<RSColorExtract::VBox, std::vector<RSColorExtract::VBox>, std::less<RSColorExtract::VBox>>
        colorVBoxPriorityQueue;
    RSColorExtract colorExtract(pixmap);
    RSColorExtract::VBox colorExtractionBox(1, 0, &colorExtract);
    colorVBoxPriorityQueue.push(colorExtractionBox);
    colorExtract.SplitBoxes(colorVBoxPriorityQueue, 0);
    EXPECT_FALSE(colorVBoxPriorityQueue.empty());
    extract->SplitBoxes(colorVBoxPriorityQueue, 2);
    EXPECT_TRUE(colorVBoxPriorityQueue.empty());
}

/**
 * @tc.name: cmpTest
 * @tc.desc: Verify function cmp
 * @tc.type:FUNC
 * @tc.require: issueI9I9D1
 */
HWTEST_F(RSColorExtractTest, cmpTest, TestSize.Level1)
{
    // for test
    std::pair<uint32_t, uint32_t> firstPair(1, 2);
    std::pair<uint32_t, uint32_t> secondPair(2, 1);
    EXPECT_TRUE(extract->cmp(firstPair, secondPair));
}

/**
 * @tc.name: GenerateAverageColorsTest
 * @tc.desc: Verify function GenerateAverageColors
 * @tc.type:FUNC
 * @tc.require: issueI9I9D1
 */
HWTEST_F(RSColorExtractTest, GenerateAverageColorsTest, TestSize.Level1)
{
    std::priority_queue<RSColorExtract::VBox, std::vector<RSColorExtract::VBox>, std::less<RSColorExtract::VBox>>
        colorVBoxPriorityQueue;
    EXPECT_TRUE(extract->GenerateAverageColors(colorVBoxPriorityQueue).empty());
    RSColorExtract colorExtract(pixmap);
    RSColorExtract::VBox colorExtractionBox(1, 0, &colorExtract);
    colorVBoxPriorityQueue.push(colorExtractionBox);
    EXPECT_FALSE(extract->GenerateAverageColors(colorVBoxPriorityQueue).empty());
}

/**
 * @tc.name: SetFeatureColorNumTest
 * @tc.desc: Verify function SetFeatureColorNum
 * @tc.type:FUNC
 * @tc.require: issueI9I9D1
 */
HWTEST_F(RSColorExtractTest, SetFeatureColorNumTest, TestSize.Level1)
{
    extract->SetFeatureColorNum(0);
    EXPECT_EQ(extract->specifiedFeatureColorNum_, 0);
}

/**
 * @tc.name: Rgb2GrayTest
 * @tc.desc: Verify function Rgb2Gray
 * @tc.type:FUNC
 * @tc.require: issueI9I9D1
 */
HWTEST_F(RSColorExtractTest, Rgb2GrayTest, TestSize.Level1)
{
    EXPECT_EQ(extract->Rgb2Gray(0), 0);
}

/**
 * @tc.name: CalcGrayMsdTest
 * @tc.desc: Verify function CalcGrayMsd
 * @tc.type:FUNC
 * @tc.require: issueI9I9D1
 */
HWTEST_F(RSColorExtractTest, CalcGrayMsdTest, TestSize.Level1)
{
    EXPECT_EQ(extract->CalcGrayMsd(), 0);
    Drawing::ImageInfo imageInfo(
        1, 1, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_UNKNOWN, nullptr);
    int addr = 1;
    size_t rowBytes = 1;
    pixmap = std::make_shared<Drawing::Pixmap>(imageInfo, &addr, rowBytes);
    auto extract2 = std::make_shared<RSColorExtract>(pixmap);
    EXPECT_EQ(extract2->CalcGrayMsd(), 0);
}

/**
 * @tc.name: NormalizeRgbTest
 * @tc.desc: Verify function NormalizeRgb
 * @tc.type:FUNC
 * @tc.require: issueI9I9D1
 */
HWTEST_F(RSColorExtractTest, NormalizeRgbTest, TestSize.Level1)
{
    // for test
    float expectedNormalizedRgbFor255 = pow((1.0 + 0.055) / 1.055, 2.4);
    EXPECT_EQ(extract->NormalizeRgb(0), 0.f);
    EXPECT_EQ(extract->NormalizeRgb(255), expectedNormalizedRgbFor255);
}

/**
 * @tc.name: CalcRelativeLumTest
 * @tc.desc: Verify function CalcRelativeLum
 * @tc.type:FUNC
 * @tc.require: issueI9I9D1
 */
HWTEST_F(RSColorExtractTest, CalcRelativeLumTest, TestSize.Level1)
{
    EXPECT_EQ(extract->CalcRelativeLum(0), 0.f);
}

/**
 * @tc.name: CalcContrastToWhiteTest
 * @tc.desc: Verify function CalcContrastToWhite
 * @tc.type:FUNC
 * @tc.require: issueI9I9D1
 */
HWTEST_F(RSColorExtractTest, CalcContrastToWhiteTest, TestSize.Level1)
{
    EXPECT_EQ(extract->CalcContrastToWhite(), 0.f);
    Drawing::ImageInfo imageInfo(
        1, 1, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_UNKNOWN, nullptr);
    int addr = 1;
    size_t rowBytes = 1;
    pixmap = std::make_shared<Drawing::Pixmap>(imageInfo, &addr, rowBytes);
    auto extract2 = std::make_shared<RSColorExtract>(pixmap);
    EXPECT_NE(extract2->CalcContrastToWhite(), 0.f);
}

/**
 * @tc.name: GetNFeatureColorsTest
 * @tc.desc: Verify function GetNFeatureColors
 * @tc.type:FUNC
 * @tc.require: issueI9I9D1
 */
HWTEST_F(RSColorExtractTest, GetNFeatureColorsTest, TestSize.Level1)
{
    extract->GetNFeatureColors(0);
    EXPECT_EQ(extract->colorValLen_, 0);
    Drawing::ImageInfo imageInfo(
        1, 1, Drawing::ColorType::COLORTYPE_UNKNOWN, Drawing::AlphaType::ALPHATYPE_UNKNOWN, nullptr);
    int addr = 1;
    size_t rowBytes = 1;
    pixmap = std::make_shared<Drawing::Pixmap>(imageInfo, &addr, rowBytes);
    auto extract2 = std::make_shared<RSColorExtract>(pixmap);
    extract2->GetNFeatureColors(0);
    extract2->GetNFeatureColors(1);
    EXPECT_NE(extract2->colorValLen_, 0);
}

} // namespace OHOS::Rosen