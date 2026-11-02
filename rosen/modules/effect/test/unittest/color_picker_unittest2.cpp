/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>

#include "color_picker.h"
#include "color.h"
#include "pixel_map.h"
#include "effect_errors.h"
#include <vector>

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Media;

namespace OHOS {
namespace Rosen {

class ColorPickerUnittest2 : public testing::Test {
public:
    static void SetUpTestCase() {};
    static void TearDownTestCase() {};
    void SetUp() override {};
    void TearDown() override {};
};

/**
 * @tc.name: GetLargestProportionColor001
 * @tc.desc: GetLargestProportionColor with empty featureColors_ returns ERR_EFFECT_INVALID_VALUE
 * @tc.type: FUNC
 */
HWTEST_F(ColorPickerUnittest2, GetLargestProportionColor001, TestSize.Level1)
{
    Media::InitializationOptions opts;
    opts.size.width = 200;
    opts.size.height = 150;
    opts.editable = true;
    std::unique_ptr<Media::PixelMap> pixmap = Media::PixelMap::Create(opts);
    uint32_t errorCode = SUCCESS;
    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_NE(pColorPicker, nullptr);

    pColorPicker->featureColors_.clear();
    ColorManager::Color color;
    uint32_t ret = pColorPicker->GetLargestProportionColor(color);
    EXPECT_EQ(ret, ERR_EFFECT_INVALID_VALUE);
}

/**
 * @tc.name: GetHighestSaturationColor001
 * @tc.desc: GetHighestSaturationColor with empty featureColors_ returns ERR_EFFECT_INVALID_VALUE
 * @tc.type: FUNC
 */
HWTEST_F(ColorPickerUnittest2, GetHighestSaturationColor001, TestSize.Level1)
{
    Media::InitializationOptions opts;
    opts.size.width = 200;
    opts.size.height = 150;
    opts.editable = true;
    std::unique_ptr<Media::PixelMap> pixmap = Media::PixelMap::Create(opts);
    uint32_t errorCode = SUCCESS;
    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_NE(pColorPicker, nullptr);

    pColorPicker->featureColors_.clear();
    ColorManager::Color color;
    uint32_t ret = pColorPicker->GetHighestSaturationColor(color);
    EXPECT_EQ(ret, ERR_EFFECT_INVALID_VALUE);
}

/**
 * @tc.name: GetAverageColor002
 * @tc.desc: GetAverageColor with empty featureColors_ returns ERR_EFFECT_INVALID_VALUE
 * @tc.type: FUNC
 */
HWTEST_F(ColorPickerUnittest2, GetAverageColor002, TestSize.Level1)
{
    Media::InitializationOptions opts;
    opts.size.width = 200;
    opts.size.height = 150;
    opts.editable = true;
    std::unique_ptr<Media::PixelMap> pixmap = Media::PixelMap::Create(opts);
    uint32_t errorCode = SUCCESS;
    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_NE(pColorPicker, nullptr);

    pColorPicker->featureColors_.clear();
    ColorManager::Color color;
    uint32_t ret = pColorPicker->GetAverageColor(color);
    EXPECT_EQ(ret, ERR_EFFECT_INVALID_VALUE);
}

/**
 * @tc.name: IsBlackOrWhiteOrGrayColor001
 * @tc.desc: IsBlackOrWhiteOrGrayColor with red color returns false
 * @tc.type: FUNC
 */
HWTEST_F(ColorPickerUnittest2, IsBlackOrWhiteOrGrayColor001, TestSize.Level1)
{
    Media::InitializationOptions opts;
    opts.size.width = 200;
    opts.size.height = 150;
    opts.editable = true;
    std::unique_ptr<Media::PixelMap> pixmap = Media::PixelMap::Create(opts);
    uint32_t errorCode = SUCCESS;
    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_NE(pColorPicker, nullptr);

    EXPECT_FALSE(pColorPicker->IsBlackOrWhiteOrGrayColor(0xFFFF0000));
}

/**
 * @tc.name: IsBlackOrWhiteOrGrayColor002
 * @tc.desc: IsBlackOrWhiteOrGrayColor with black color returns true
 * @tc.type: FUNC
 */
HWTEST_F(ColorPickerUnittest2, IsBlackOrWhiteOrGrayColor002, TestSize.Level1)
{
    Media::InitializationOptions opts;
    opts.size.width = 200;
    opts.size.height = 150;
    opts.editable = true;
    std::unique_ptr<Media::PixelMap> pixmap = Media::PixelMap::Create(opts);
    uint32_t errorCode = SUCCESS;
    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_NE(pColorPicker, nullptr);

    EXPECT_TRUE(pColorPicker->IsBlackOrWhiteOrGrayColor(0xFF000000));
}

/**
 * @tc.name: IsBlackOrWhiteOrGrayColor003
 * @tc.desc: IsBlackOrWhiteOrGrayColor with gray color returns true
 * @tc.type: FUNC
 */
HWTEST_F(ColorPickerUnittest2, IsBlackOrWhiteOrGrayColor003, TestSize.Level1)
{
    Media::InitializationOptions opts;
    opts.size.width = 200;
    opts.size.height = 150;
    opts.editable = true;
    std::unique_ptr<Media::PixelMap> pixmap = Media::PixelMap::Create(opts);
    uint32_t errorCode = SUCCESS;
    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_NE(pColorPicker, nullptr);

    EXPECT_TRUE(pColorPicker->IsBlackOrWhiteOrGrayColor(0xFF808080));
}

/**
 * @tc.name: GetMorandiBackgroundColor001
 * @tc.desc: GetMorandiBackgroundColor with non-BWG dominant color
 * @tc.type: FUNC
 */
HWTEST_F(ColorPickerUnittest2, GetMorandiBackgroundColor001, TestSize.Level1)
{
    Media::InitializationOptions opts;
    opts.size.width = 200;
    opts.size.height = 150;
    opts.editable = true;
    std::unique_ptr<Media::PixelMap> pixmap = Media::PixelMap::Create(opts);
    uint32_t errorCode = SUCCESS;
    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_NE(pColorPicker, nullptr);

    std::vector<std::pair<uint32_t, uint32_t>> featureColors;
    featureColors.emplace_back(std::pair<uint32_t, uint32_t>(0xFFFF0000, 10));
    pColorPicker->featureColors_ = std::move(featureColors);

    ColorManager::Color color;
    uint32_t ret = pColorPicker->GetMorandiBackgroundColor(color);
    EXPECT_EQ(ret, SUCCESS);
}

/**
 * @tc.name: GetMorandiBackgroundColor002
 * @tc.desc: GetMorandiBackgroundColor with BWG dominant and non-BWG secondary color
 * @tc.type: FUNC
 */
HWTEST_F(ColorPickerUnittest2, GetMorandiBackgroundColor002, TestSize.Level1)
{
    Media::InitializationOptions opts;
    opts.size.width = 200;
    opts.size.height = 150;
    opts.editable = true;
    std::unique_ptr<Media::PixelMap> pixmap = Media::PixelMap::Create(opts);
    uint32_t errorCode = SUCCESS;
    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_NE(pColorPicker, nullptr);

    std::vector<std::pair<uint32_t, uint32_t>> featureColors;
    featureColors.emplace_back(std::pair<uint32_t, uint32_t>(0xFF000000, 10));
    featureColors.emplace_back(std::pair<uint32_t, uint32_t>(0xFFFF0000, 5));
    pColorPicker->featureColors_ = std::move(featureColors);

    ColorManager::Color color;
    uint32_t ret = pColorPicker->GetMorandiBackgroundColor(color);
    EXPECT_EQ(ret, SUCCESS);
}

/**
 * @tc.name: GetMorandiBackgroundColor003
 * @tc.desc: GetMorandiBackgroundColor with BWG dominant and all BWG colors
 * @tc.type: FUNC
 */
HWTEST_F(ColorPickerUnittest2, GetMorandiBackgroundColor003, TestSize.Level1)
{
    Media::InitializationOptions opts;
    opts.size.width = 200;
    opts.size.height = 150;
    opts.editable = true;
    std::unique_ptr<Media::PixelMap> pixmap = Media::PixelMap::Create(opts);
    uint32_t errorCode = SUCCESS;
    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_NE(pColorPicker, nullptr);

    std::vector<std::pair<uint32_t, uint32_t>> featureColors;
    featureColors.emplace_back(std::pair<uint32_t, uint32_t>(0xFF000000, 10));
    featureColors.emplace_back(std::pair<uint32_t, uint32_t>(0xFFFFFFFF, 5));
    pColorPicker->featureColors_ = std::move(featureColors);

    ColorManager::Color color;
    uint32_t ret = pColorPicker->GetMorandiBackgroundColor(color);
    EXPECT_EQ(ret, SUCCESS);
}

/**
 * @tc.name: GetMorandiShadowColor002
 * @tc.desc: GetMorandiShadowColor with empty featureColors_ returns ERR_EFFECT_INVALID_VALUE
 * @tc.type: FUNC
 */
HWTEST_F(ColorPickerUnittest2, GetMorandiShadowColor002, TestSize.Level1)
{
    Media::InitializationOptions opts;
    opts.size.width = 200;
    opts.size.height = 150;
    opts.editable = true;
    std::unique_ptr<Media::PixelMap> pixmap = Media::PixelMap::Create(opts);
    uint32_t errorCode = SUCCESS;
    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_NE(pColorPicker, nullptr);

    pColorPicker->featureColors_.clear();
    ColorManager::Color color;
    uint32_t ret = pColorPicker->GetMorandiShadowColor(color);
    EXPECT_EQ(ret, ERR_EFFECT_INVALID_VALUE);
}

/**
 * @tc.name: GetMorandiShadowColor003
 * @tc.desc: GetMorandiShadowColor with non-BWG dominant color
 * @tc.type: FUNC
 */
HWTEST_F(ColorPickerUnittest2, GetMorandiShadowColor003, TestSize.Level1)
{
    Media::InitializationOptions opts;
    opts.size.width = 200;
    opts.size.height = 150;
    opts.editable = true;
    std::unique_ptr<Media::PixelMap> pixmap = Media::PixelMap::Create(opts);
    uint32_t errorCode = SUCCESS;
    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_NE(pColorPicker, nullptr);

    std::vector<std::pair<uint32_t, uint32_t>> featureColors;
    featureColors.emplace_back(std::pair<uint32_t, uint32_t>(0xFFFF0000, 10));
    pColorPicker->featureColors_ = std::move(featureColors);

    ColorManager::Color color;
    uint32_t ret = pColorPicker->GetMorandiShadowColor(color);
    EXPECT_EQ(ret, SUCCESS);
}

/**
 * @tc.name: GetMorandiShadowColor004
 * @tc.desc: GetMorandiShadowColor with BWG dominant and non-BWG secondary color
 * @tc.type: FUNC
 */
HWTEST_F(ColorPickerUnittest2, GetMorandiShadowColor004, TestSize.Level1)
{
    Media::InitializationOptions opts;
    opts.size.width = 200;
    opts.size.height = 150;
    opts.editable = true;
    std::unique_ptr<Media::PixelMap> pixmap = Media::PixelMap::Create(opts);
    uint32_t errorCode = SUCCESS;
    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_NE(pColorPicker, nullptr);

    std::vector<std::pair<uint32_t, uint32_t>> featureColors;
    featureColors.emplace_back(std::pair<uint32_t, uint32_t>(0xFF000000, 10));
    featureColors.emplace_back(std::pair<uint32_t, uint32_t>(0xFFFF0000, 5));
    pColorPicker->featureColors_ = std::move(featureColors);

    ColorManager::Color color;
    uint32_t ret = pColorPicker->GetMorandiShadowColor(color);
    EXPECT_EQ(ret, SUCCESS);
}

/**
 * @tc.name: GetMorandiShadowColor005
 * @tc.desc: GetMorandiShadowColor with BWG dominant and all BWG colors
 * @tc.type: FUNC
 */
HWTEST_F(ColorPickerUnittest2, GetMorandiShadowColor005, TestSize.Level1)
{
    Media::InitializationOptions opts;
    opts.size.width = 200;
    opts.size.height = 150;
    opts.editable = true;
    std::unique_ptr<Media::PixelMap> pixmap = Media::PixelMap::Create(opts);
    uint32_t errorCode = SUCCESS;
    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_NE(pColorPicker, nullptr);

    std::vector<std::pair<uint32_t, uint32_t>> featureColors;
    featureColors.emplace_back(std::pair<uint32_t, uint32_t>(0xFF000000, 10));
    featureColors.emplace_back(std::pair<uint32_t, uint32_t>(0xFFFFFFFF, 5));
    pColorPicker->featureColors_ = std::move(featureColors);

    ColorManager::Color color;
    uint32_t ret = pColorPicker->GetMorandiShadowColor(color);
    EXPECT_EQ(ret, SUCCESS);
}

/**
 * @tc.name: GetDeepenImmersionColor002
 * @tc.desc: GetDeepenImmersionColor with hsv.v >= 40
 * @tc.type: FUNC
 */
HWTEST_F(ColorPickerUnittest2, GetDeepenImmersionColor002, TestSize.Level1)
{
    Media::InitializationOptions opts;
    opts.size.width = 200;
    opts.size.height = 150;
    opts.editable = true;
    std::unique_ptr<Media::PixelMap> pixmap = Media::PixelMap::Create(opts);
    uint32_t errorCode = SUCCESS;
    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_NE(pColorPicker, nullptr);

    std::vector<std::pair<uint32_t, uint32_t>> featureColors;
    featureColors.emplace_back(std::pair<uint32_t, uint32_t>(0xFFFF0000, 10));
    pColorPicker->featureColors_ = std::move(featureColors);

    ColorManager::Color color;
    uint32_t ret = pColorPicker->GetDeepenImmersionColor(color);
    EXPECT_EQ(ret, SUCCESS);
}

/**
 * @tc.name: GetDeepenImmersionColor003
 * @tc.desc: GetDeepenImmersionColor with 20 <= hsv.v < 40
 * @tc.type: FUNC
 */
HWTEST_F(ColorPickerUnittest2, GetDeepenImmersionColor003, TestSize.Level1)
{
    Media::InitializationOptions opts;
    opts.size.width = 200;
    opts.size.height = 150;
    opts.editable = true;
    std::unique_ptr<Media::PixelMap> pixmap = Media::PixelMap::Create(opts);
    uint32_t errorCode = SUCCESS;
    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_NE(pColorPicker, nullptr);

    std::vector<std::pair<uint32_t, uint32_t>> featureColors;
    featureColors.emplace_back(std::pair<uint32_t, uint32_t>(0xFF4D0000, 10));
    pColorPicker->featureColors_ = std::move(featureColors);

    ColorManager::Color color;
    uint32_t ret = pColorPicker->GetDeepenImmersionColor(color);
    EXPECT_EQ(ret, SUCCESS);
}

/**
 * @tc.name: GetDeepenImmersionColor004
 * @tc.desc: GetDeepenImmersionColor with hsv.v < 20
 * @tc.type: FUNC
 */
HWTEST_F(ColorPickerUnittest2, GetDeepenImmersionColor004, TestSize.Level1)
{
    Media::InitializationOptions opts;
    opts.size.width = 200;
    opts.size.height = 150;
    opts.editable = true;
    std::unique_ptr<Media::PixelMap> pixmap = Media::PixelMap::Create(opts);
    uint32_t errorCode = SUCCESS;
    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_NE(pColorPicker, nullptr);

    std::vector<std::pair<uint32_t, uint32_t>> featureColors;
    featureColors.emplace_back(std::pair<uint32_t, uint32_t>(0xFF0A0000, 10));
    pColorPicker->featureColors_ = std::move(featureColors);

    ColorManager::Color color;
    uint32_t ret = pColorPicker->GetDeepenImmersionColor(color);
    EXPECT_EQ(ret, SUCCESS);
}

/**
 * @tc.name: GetDeepenImmersionColor005
 * @tc.desc: GetDeepenImmersionColor with no dominant color returns black
 * @tc.type: FUNC
 */
HWTEST_F(ColorPickerUnittest2, GetDeepenImmersionColor005, TestSize.Level1)
{
    Media::InitializationOptions opts;
    opts.size.width = 200;
    opts.size.height = 150;
    opts.editable = true;
    std::unique_ptr<Media::PixelMap> pixmap = Media::PixelMap::Create(opts);
    uint32_t errorCode = SUCCESS;
    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_NE(pColorPicker, nullptr);

    std::vector<std::pair<uint32_t, uint32_t>> featureColors;
    featureColors.emplace_back(std::pair<uint32_t, uint32_t>(0xFFFF0000, 10));
    featureColors.emplace_back(std::pair<uint32_t, uint32_t>(0xFF00FF00, 10));
    pColorPicker->featureColors_ = std::move(featureColors);
    pColorPicker->colorValLen_ = 200;

    ColorManager::Color color;
    uint32_t ret = pColorPicker->GetDeepenImmersionColor(color);
    EXPECT_EQ(ret, SUCCESS);
}

/**
 * @tc.name: GetImmersiveBackgroundColor002
 * @tc.desc: GetImmersiveBackgroundColor with valid featureColors_
 * @tc.type: FUNC
 */
HWTEST_F(ColorPickerUnittest2, GetImmersiveBackgroundColor002, TestSize.Level1)
{
    Media::InitializationOptions opts;
    opts.size.width = 200;
    opts.size.height = 150;
    opts.editable = true;
    std::unique_ptr<Media::PixelMap> pixmap = Media::PixelMap::Create(opts);
    uint32_t errorCode = SUCCESS;
    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_NE(pColorPicker, nullptr);

    std::vector<std::pair<uint32_t, uint32_t>> featureColors;
    featureColors.emplace_back(std::pair<uint32_t, uint32_t>(0xFFFF0000, 10));
    featureColors.emplace_back(std::pair<uint32_t, uint32_t>(0xFF00FF00, 5));
    pColorPicker->featureColors_ = std::move(featureColors);
    pColorPicker->colorValLen_ = 15;

    ColorManager::Color color;
    uint32_t ret = pColorPicker->GetImmersiveBackgroundColor(color);
    EXPECT_EQ(ret, SUCCESS);
}

/**
 * @tc.name: GetImmersiveForegroundColor002
 * @tc.desc: GetImmersiveForegroundColor with valid featureColors_
 * @tc.type: FUNC
 */
HWTEST_F(ColorPickerUnittest2, GetImmersiveForegroundColor002, TestSize.Level1)
{
    Media::InitializationOptions opts;
    opts.size.width = 200;
    opts.size.height = 150;
    opts.editable = true;
    std::unique_ptr<Media::PixelMap> pixmap = Media::PixelMap::Create(opts);
    uint32_t errorCode = SUCCESS;
    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_NE(pColorPicker, nullptr);

    std::vector<std::pair<uint32_t, uint32_t>> featureColors;
    featureColors.emplace_back(std::pair<uint32_t, uint32_t>(0xFFFF0000, 10));
    featureColors.emplace_back(std::pair<uint32_t, uint32_t>(0xFF00FF00, 5));
    pColorPicker->featureColors_ = std::move(featureColors);
    pColorPicker->colorValLen_ = 15;

    ColorManager::Color color;
    uint32_t ret = pColorPicker->GetImmersiveForegroundColor(color);
    EXPECT_EQ(ret, SUCCESS);
}

/**
 * @tc.name: DiscriminatePictureLightDegree003
 * @tc.desc: DiscriminatePictureLightDegree degree is EXTREMELY_FLOWERY_PICTURE
 * @tc.type: FUNC
 */
HWTEST_F(ColorPickerUnittest2, DiscriminatePictureLightDegree003, TestSize.Level1)
{
    Media::InitializationOptions opts;
    opts.size.width = 200;
    opts.size.height = 150;
    opts.editable = true;
    std::unique_ptr<Media::PixelMap> pixmap = Media::PixelMap::Create(opts);
    uint32_t errorCode = SUCCESS;
    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_NE(pColorPicker, nullptr);

    std::vector<std::pair<uint32_t, uint32_t>> featureColors;
    featureColors.emplace_back(std::pair<uint32_t, uint32_t>(0xFFFF0000, 10));
    pColorPicker->featureColors_ = std::move(featureColors);

    pColorPicker->grayMsd_ = 7000;
    pColorPicker->contrastToWhite_ = 1.0;
    PictureLightColorDegree degree = UNKOWN_LIGHT_COLOR_DEGREE_PICTURE;
    pColorPicker->DiscriminatePictureLightDegree(degree);
    EXPECT_EQ(degree, EXTREMELY_FLOWERY_PICTURE);
}

/**
 * @tc.name: DiscriminatePictureLightDegree004
 * @tc.desc: DiscriminatePictureLightDegree degree is EXTREMELY_LIGHT_COLOR_PICTURE
 * @tc.type: FUNC
 */
HWTEST_F(ColorPickerUnittest2, DiscriminatePictureLightDegree004, TestSize.Level1)
{
    Media::InitializationOptions opts;
    opts.size.width = 200;
    opts.size.height = 150;
    opts.editable = true;
    std::unique_ptr<Media::PixelMap> pixmap = Media::PixelMap::Create(opts);
    uint32_t errorCode = SUCCESS;
    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_NE(pColorPicker, nullptr);

    std::vector<std::pair<uint32_t, uint32_t>> featureColors;
    featureColors.emplace_back(std::pair<uint32_t, uint32_t>(0xFFFF0000, 10));
    pColorPicker->featureColors_ = std::move(featureColors);

    pColorPicker->grayMsd_ = 5000;
    pColorPicker->contrastToWhite_ = 1.0;
    PictureLightColorDegree degree = UNKOWN_LIGHT_COLOR_DEGREE_PICTURE;
    pColorPicker->DiscriminatePictureLightDegree(degree);
    EXPECT_EQ(degree, EXTREMELY_LIGHT_COLOR_PICTURE);
}

/**
 * @tc.name: DiscriminatePictureLightDegree005
 * @tc.desc: DiscriminatePictureLightDegree degree is LIGHT_COLOR_PICTURE
 * @tc.type: FUNC
 */
HWTEST_F(ColorPickerUnittest2, DiscriminatePictureLightDegree005, TestSize.Level1)
{
    Media::InitializationOptions opts;
    opts.size.width = 200;
    opts.size.height = 150;
    opts.editable = true;
    std::unique_ptr<Media::PixelMap> pixmap = Media::PixelMap::Create(opts);
    uint32_t errorCode = SUCCESS;
    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_NE(pColorPicker, nullptr);

    std::vector<std::pair<uint32_t, uint32_t>> featureColors;
    featureColors.emplace_back(std::pair<uint32_t, uint32_t>(0xFFFF0000, 10));
    pColorPicker->featureColors_ = std::move(featureColors);

    pColorPicker->grayMsd_ = 5000;
    pColorPicker->contrastToWhite_ = 1.7;
    PictureLightColorDegree degree = UNKOWN_LIGHT_COLOR_DEGREE_PICTURE;
    pColorPicker->DiscriminatePictureLightDegree(degree);
    EXPECT_EQ(degree, LIGHT_COLOR_PICTURE);
}

/**
 * @tc.name: DiscriminatePictureLightDegree006
 * @tc.desc: DiscriminatePictureLightDegree degree is FLOWERY_PICTURE
 * @tc.type: FUNC
 */
HWTEST_F(ColorPickerUnittest2, DiscriminatePictureLightDegree006, TestSize.Level1)
{
    Media::InitializationOptions opts;
    opts.size.width = 200;
    opts.size.height = 150;
    opts.editable = true;
    std::unique_ptr<Media::PixelMap> pixmap = Media::PixelMap::Create(opts);
    uint32_t errorCode = SUCCESS;
    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_NE(pColorPicker, nullptr);

    std::vector<std::pair<uint32_t, uint32_t>> featureColors;
    featureColors.emplace_back(std::pair<uint32_t, uint32_t>(0xFFFF0000, 10));
    pColorPicker->featureColors_ = std::move(featureColors);

    pColorPicker->grayMsd_ = 5000;
    pColorPicker->contrastToWhite_ = 3.0;
    PictureLightColorDegree degree = UNKOWN_LIGHT_COLOR_DEGREE_PICTURE;
    pColorPicker->DiscriminatePictureLightDegree(degree);
    EXPECT_EQ(degree, FLOWERY_PICTURE);
}

/**
 * @tc.name: DiscriminatePictureLightDegree007
 * @tc.desc: DiscriminatePictureLightDegree degree is EXTREMELY_DARK_COLOR_PICTURE
 * @tc.type: FUNC
 */
HWTEST_F(ColorPickerUnittest2, DiscriminatePictureLightDegree007, TestSize.Level1)
{
    Media::InitializationOptions opts;
    opts.size.width = 200;
    opts.size.height = 150;
    opts.editable = true;
    std::unique_ptr<Media::PixelMap> pixmap = Media::PixelMap::Create(opts);
    uint32_t errorCode = SUCCESS;
    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_NE(pColorPicker, nullptr);

    std::vector<std::pair<uint32_t, uint32_t>> featureColors;
    featureColors.emplace_back(std::pair<uint32_t, uint32_t>(0xFFFF0000, 10));
    pColorPicker->featureColors_ = std::move(featureColors);

    pColorPicker->grayMsd_ = 2000;
    pColorPicker->contrastToWhite_ = 9.0;
    PictureLightColorDegree degree = UNKOWN_LIGHT_COLOR_DEGREE_PICTURE;
    pColorPicker->DiscriminatePictureLightDegree(degree);
    EXPECT_EQ(degree, EXTREMELY_DARK_COLOR_PICTURE);
}

/**
 * @tc.name: GetReverseColor002
 * @tc.desc: GetReverseColor with EXTREMELY_LIGHT_COLOR_PICTURE returns black
 * @tc.type: FUNC
 */
HWTEST_F(ColorPickerUnittest2, GetReverseColor002, TestSize.Level1)
{
    Media::InitializationOptions opts;
    opts.size.width = 200;
    opts.size.height = 150;
    opts.editable = true;
    std::unique_ptr<Media::PixelMap> pixmap = Media::PixelMap::Create(opts);
    uint32_t errorCode = SUCCESS;
    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_NE(pColorPicker, nullptr);

    std::vector<std::pair<uint32_t, uint32_t>> featureColors;
    featureColors.emplace_back(std::pair<uint32_t, uint32_t>(0xFFFF0000, 10));
    pColorPicker->featureColors_ = std::move(featureColors);

    pColorPicker->grayMsd_ = 5000;
    pColorPicker->contrastToWhite_ = 1.0;
    ColorManager::Color color;
    uint32_t ret = pColorPicker->GetReverseColor(color);
    EXPECT_EQ(ret, SUCCESS);
    EXPECT_TRUE(color.ColorEqual(ColorManager::Color(0xFF000000U)));
}

/**
 * @tc.name: GetReverseColor003
 * @tc.desc: GetReverseColor with non-EXTREMELY_LIGHT returns white
 * @tc.type: FUNC
 */
HWTEST_F(ColorPickerUnittest2, GetReverseColor003, TestSize.Level1)
{
    Media::InitializationOptions opts;
    opts.size.width = 200;
    opts.size.height = 150;
    opts.editable = true;
    std::unique_ptr<Media::PixelMap> pixmap = Media::PixelMap::Create(opts);
    uint32_t errorCode = SUCCESS;
    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_NE(pColorPicker, nullptr);

    std::vector<std::pair<uint32_t, uint32_t>> featureColors;
    featureColors.emplace_back(std::pair<uint32_t, uint32_t>(0xFFFF0000, 10));
    pColorPicker->featureColors_ = std::move(featureColors);

    pColorPicker->grayMsd_ = 5000;
    pColorPicker->contrastToWhite_ = 3.0;
    ColorManager::Color color;
    uint32_t ret = pColorPicker->GetReverseColor(color);
    EXPECT_EQ(ret, SUCCESS);
    EXPECT_TRUE(color.ColorEqual(ColorManager::Color(0xFFFFFFFFU)));
}

/**
 * @tc.name: GetTopProportionColors001
 * @tc.desc: GetTopProportionColors with empty featureColors_ returns empty vector
 * @tc.type: FUNC
 */
HWTEST_F(ColorPickerUnittest2, GetTopProportionColors001, TestSize.Level1)
{
    Media::InitializationOptions opts;
    opts.size.width = 200;
    opts.size.height = 150;
    opts.editable = true;
    std::unique_ptr<Media::PixelMap> pixmap = Media::PixelMap::Create(opts);
    uint32_t errorCode = SUCCESS;
    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_NE(pColorPicker, nullptr);

    pColorPicker->featureColors_.clear();
    auto colors = pColorPicker->GetTopProportionColors(5);
    EXPECT_EQ(colors.size(), 0u);
}

/**
 * @tc.name: GetTopProportion001
 * @tc.desc: GetTopProportion with multiple feature colors
 * @tc.type: FUNC
 */
HWTEST_F(ColorPickerUnittest2, GetTopProportion001, TestSize.Level1)
{
    Media::InitializationOptions opts;
    opts.size.width = 200;
    opts.size.height = 150;
    opts.editable = true;
    std::unique_ptr<Media::PixelMap> pixmap = Media::PixelMap::Create(opts);
    uint32_t errorCode = SUCCESS;
    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_NE(pColorPicker, nullptr);

    std::vector<std::pair<uint32_t, uint32_t>> featureColors;
    featureColors.emplace_back(std::pair<uint32_t, uint32_t>(0xFFFF0000, 60));
    featureColors.emplace_back(std::pair<uint32_t, uint32_t>(0xFF00FF00, 40));
    pColorPicker->featureColors_ = std::move(featureColors);
    pColorPicker->colorValLen_ = 100;

    auto percentages = pColorPicker->GetTopProportion(2);
    EXPECT_EQ(percentages.size(), 2u);
    EXPECT_GT(percentages[0], percentages[1]);
}

/**
 * @tc.name: ComplexityDegree002
 * @tc.desc: ComplexityDegree boundary with grayVariance at threshold and low complexity
 * @tc.type: FUNC
 */
HWTEST_F(ColorPickerUnittest2, ComplexityDegree002, TestSize.Level1)
{
    Media::InitializationOptions opts;
    opts.size.width = 200;
    opts.size.height = 150;
    opts.editable = true;
    std::unique_ptr<Media::PixelMap> pixmap = Media::PixelMap::Create(opts);
    uint32_t errorCode = SUCCESS;
    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_NE(pColorPicker, nullptr);

    std::vector<std::pair<uint32_t, uint32_t>> featureColors;
    featureColors.emplace_back(std::pair<uint32_t, uint32_t>(0xFFFF0000, 10));
    pColorPicker->featureColors_ = std::move(featureColors);

    pColorPicker->grayMsd_ = 6000;
    pColorPicker->distinctColorCount_ = 500;
    PictureComplexityDegree degree = UNKNOWN_COMPLEXITY_DEGREE_PICTURE;
    pColorPicker->ComplexityDegree(degree);
    EXPECT_EQ(degree, VERY_FLOWERY_PICTURE);
}

/**
 * @tc.name: ShadeDegree002
 * @tc.desc: ShadeDegree boundary at EXTREMELY_DARK threshold
 * @tc.type: FUNC
 */
HWTEST_F(ColorPickerUnittest2, ShadeDegree002, TestSize.Level1)
{
    Media::InitializationOptions opts;
    opts.size.width = 200;
    opts.size.height = 150;
    opts.editable = true;
    std::unique_ptr<Media::PixelMap> pixmap = Media::PixelMap::Create(opts);
    uint32_t errorCode = SUCCESS;
    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_NE(pColorPicker, nullptr);

    std::vector<std::pair<uint32_t, uint32_t>> featureColors;
    featureColors.emplace_back(std::pair<uint32_t, uint32_t>(0xFFFF0000, 10));
    pColorPicker->featureColors_ = std::move(featureColors);

    pColorPicker->contrastToWhite_ = 13.0;
    PictureShadeDegree degree = UNKNOWN_SHADE_DEGREE_PICTURE;
    pColorPicker->ShadeDegree(degree);
    EXPECT_EQ(degree, DARK_PICTURE);
}

/**
 * @tc.name: GetAlphaZeroTransparentProportion001
 * @tc.desc: GetAlphaZeroTransparentProportion with non-zero fullyTransparentPixelNum_
 * @tc.type: FUNC
 */
HWTEST_F(ColorPickerUnittest2, GetAlphaZeroTransparentProportion001, TestSize.Level1)
{
    Media::InitializationOptions opts;
    opts.size.width = 200;
    opts.size.height = 150;
    opts.editable = true;
    std::unique_ptr<Media::PixelMap> pixmap = Media::PixelMap::Create(opts);
    uint32_t errorCode = SUCCESS;
    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_NE(pColorPicker, nullptr);

    pColorPicker->fullyTransparentPixelNum_ = 5000;
    double proportion = pColorPicker->GetAlphaZeroTransparentProportion();
    EXPECT_DOUBLE_EQ(proportion, 0.5);
}
} // namespace Rosen
} // namespace OHOS
