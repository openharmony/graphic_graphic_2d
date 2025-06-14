/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "color_picker_unittest.h"
#include "color_picker.h"
#include "color.h"
#include "image_source.h"
#include "pixel_map.h"
#include "effect_errors.h"
#include "hilog/log.h"
#include "test_picture_files.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Media;
using namespace OHOS::HiviewDFX;

static constexpr OHOS::HiviewDFX::HiLogLabel LABEL_TEST = {
    LOG_CORE, LOG_DOMAIN, "ColorPickerTest"
};

namespace OHOS {
namespace Rosen {

std::shared_ptr<ColorPicker> ColorPickerUnittest::CreateColorPicker()
{
    size_t bufferSize = 0;
    uint8_t *buffer = GetJpgBuffer(bufferSize);
    if (buffer == nullptr) {
        return nullptr;
    }

    uint32_t errorCode = 0;
    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    std::unique_ptr<ImageSource> imageSource =
        ImageSource::CreateImageSource(buffer, bufferSize, opts, errorCode);
    if ((errorCode != SUCCESS) || (imageSource == nullptr)) {
        return nullptr;
    }

    DecodeOptions decodeOpts;
    std::unique_ptr<PixelMap> pixmap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    if ((errorCode != SUCCESS) || (pixmap == nullptr)) {
        return nullptr;
    }

    return ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
}
/**
 * @tc.name: CreateColorPickerFromPixelmapTest001
 * @tc.desc: Ensure the ability of creating color picker from pixelmap.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ColorPickerUnittest, CreateColorPickerFromPixelmapTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ColorPickerUnittest CreateColorPickerFromPixelmapTest001 start";
    /**
     * @tc.steps: step1. Create a pixelmap
     */
    Media::InitializationOptions opts;
    opts.size.width = 200;
    opts.size.height = 150;
    opts.editable = true;
    std::unique_ptr<Media::PixelMap> pixmap = Media::PixelMap::Create(opts);

    /**
     * @tc.steps: step2. Call create From pixelMap
     */
    uint32_t errorCode = SUCCESS;
    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    EXPECT_NE(pColorPicker, nullptr);
}

/**
 * @tc.name: CreateColorPickerFromPixelmapTest002
 * @tc.desc: Ensure the ability of creating color picker from pixelmap.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ColorPickerUnittest, CreateColorPickerFromPixelmapTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ColorPickerUnittest CreateColorPickerFromPixelmapTest002 start";
    size_t bufferSize = 0;
    uint8_t *buffer = GetPngBuffer(bufferSize);
    ASSERT_NE(buffer, nullptr);
    
    /**
     * @tc.steps: step1. Create a ImageSource
     */
    uint32_t errorCode = 0;
    SourceOptions opts;
    opts.formatHint = "image/png";
    std::unique_ptr<ImageSource> imageSource =
        ImageSource::CreateImageSource(buffer, bufferSize, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);

    /**
     * @tc.steps: step2. decode image source to pixel map by default decode options
     * @tc.expected: step2. decode image source to pixel map success.
     */
    DecodeOptions decodeOpts;
    std::unique_ptr<PixelMap> pixmap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    HiLog::Debug(LABEL_TEST, "create pixel map error code=%{public}u.", errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(pixmap.get(), nullptr);

    /**
     * @tc.steps: step3. Call create From pixelMap
     */
    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    EXPECT_NE(pColorPicker, nullptr);
}

/**
 * @tc.name: CreateColorPickerFromPixelmapTest003
 * @tc.desc: Ensure the ability of creating effect chain from config file.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ColorPickerUnittest, CreateColorPickerFromPixelmapTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ColorPickerUnittest CreateColorPickerFromPixelmapTest003 start";
    /**
     * @tc.steps: step1. Create a pixelMap
     */
    std::unique_ptr<Media::PixelMap> pixmap = nullptr;

    /**
     * @tc.steps: step2. Call create From pixelMap
     */
    uint32_t errorCode = SUCCESS;
    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_EQ(errorCode, ERR_EFFECT_INVALID_VALUE);
    EXPECT_EQ(pColorPicker, nullptr);
}

/**
 * @tc.name: CreateColorPickerFromPixelmapTest004
 * @tc.desc: Ensure the ability of creating color picker from pixelmap.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ColorPickerUnittest, CreateColorPickerFromPixelmapTest004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ColorPickerUnittest CreateColorPickerFromPixelmapTest004 start";
    /**
     * @tc.steps: step1. Create a pixelmap
     */
    Media::InitializationOptions opts;
    opts.size.width = 200;
    opts.size.height = 150;
    opts.editable = true;
    std::unique_ptr<Media::PixelMap> pixmap = Media::PixelMap::Create(opts);

    /**
     * @tc.steps: step2. Call create From pixelMap
     */
    uint32_t errorCode = SUCCESS;
    double region[4] = {0, 0, 0.5, 0.5};
    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), region, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    EXPECT_NE(pColorPicker, nullptr);
}

/**
 * @tc.name: CreateColorPickerFromPixelmapTest005
 * @tc.desc: Ensure the ability of creating color picker from pixelmap.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ColorPickerUnittest, CreateColorPickerFromPixelmapTest005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ColorPickerUnittest CreateColorPickerFromPixelmapTest005 start";
    /**
     * @tc.steps: step1. Create a pixelmap
     */
    Media::InitializationOptions opts;
    opts.size.width = 200;
    opts.size.height = 150;
    opts.editable = true;
    std::unique_ptr<Media::PixelMap> pixmap = Media::PixelMap::Create(opts);

    /**
     * @tc.steps: step2. Call create From pixelMap
     */
    uint32_t errorCode = SUCCESS;
    double region[4] = {0, 0.5, 0.5, 0.5};
    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), region, errorCode);
    ASSERT_EQ(pColorPicker->colorValLen_, 0);
    EXPECT_NE(pColorPicker, nullptr);
}

/**
 * @tc.name: GetMainColorTest001
 * @tc.desc: Ensure the ability of creating effect chain from config file.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ColorPickerUnittest, GetMainColorTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ColorPickerUnittest GetMainColorTest001 start";
    size_t bufferSize = 0;
    uint8_t *buffer = GetJpgBuffer(bufferSize);
    ASSERT_NE(buffer, nullptr);

    /**
     * @tc.steps: step1. create image source by correct jpeg file path and jpeg format hit.
     * @tc.expected: step1. create image source success.
     */
    uint32_t errorCode = 0;
    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    std::unique_ptr<ImageSource> imageSource =
        ImageSource::CreateImageSource(buffer, bufferSize, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(imageSource.get(), nullptr);

    /**
     * @tc.steps: step2. decode image source to pixel map by default decode options
     * @tc.expected: step2. decode image source to pixel map success.
     */
    DecodeOptions decodeOpts;
    std::unique_ptr<PixelMap> pixmap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    HiLog::Debug(LABEL_TEST, "create pixel map error code=%{public}u.", errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(pixmap.get(), nullptr);

    /**
     * @tc.steps: step2. Call create From pixelMap
     */
    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    EXPECT_NE(pColorPicker, nullptr);

    /**
     * @tc.steps: step3. Get main color from pixmap
     */
    ColorManager::Color color;
    errorCode = pColorPicker->GetMainColor(color);
    HiLog::Info(LABEL_TEST, "get main color t1[rgba]=%{public}f,%{public}f,%{public}f,%{public}f",
                color.r, color.g, color.b, color.a);
    ASSERT_EQ(errorCode, SUCCESS);
    bool ret = color.ColorEqual(ColorManager::Color(1.f, 0.788235f, 0.050980f, 1.f));
    EXPECT_EQ(true, ret);
}

/**
 * @tc.name: GetMainColorTest002
 * @tc.desc: Ensure the ability of creating effect chain from config file.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ColorPickerUnittest, GetMainColorTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ColorPickerUnittest GetMainColorTest002 start";
    size_t bufferSize = 0;
    uint8_t *buffer = GetPngBuffer(bufferSize);
    ASSERT_NE(buffer, nullptr);

    /**
     * @tc.steps: step1. Create a ImageSource
     */
    uint32_t errorCode = 0;
    SourceOptions opts;
    opts.formatHint = "image/png";
    std::unique_ptr<ImageSource> imageSource =
        ImageSource::CreateImageSource(buffer, bufferSize, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(imageSource.get(), nullptr);

    /**
     * @tc.steps: step2. decode image source to pixel map by default decode options
     * @tc.expected: step2. decode image source to pixel map success.
     */
    DecodeOptions decodeOpts;
    std::unique_ptr<PixelMap> pixmap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    HiLog::Debug(LABEL_TEST, "create pixel map error code=%{public}u.", errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(pixmap.get(), nullptr);

    /**
     * @tc.steps: step3. Call create From pixelMap
     */
    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(pColorPicker, nullptr);

    /**
     * @tc.steps: step4. Get main color from pixmap
     */
    ColorManager::Color color;
    errorCode = pColorPicker->GetMainColor(color);
    HiLog::Info(LABEL_TEST, "get main color t2[rgba]=%{public}f,%{public}f,%{public}f,%{public}f",
                color.r, color.g, color.b, color.a);
    ASSERT_EQ(errorCode, SUCCESS);
    bool ret = color.ColorEqual(ColorManager::Color(1.f, 1.f, 1.f, 1.f));
    EXPECT_EQ(true, ret);
}

/**
 * @tc.name: GetMainColorTest003
 * @tc.desc: Ensure the ability of creating effect chain from config file.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ColorPickerUnittest, GetMainColorTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ColorPickerUnittest GetMainColorTest003 start";
    /**
     * @tc.steps: step1. Create a pixelMap
     */
    Media::InitializationOptions opts;
    opts.size.width = 200;
    opts.size.height = 100;
    opts.editable = true;
    std::unique_ptr<Media::PixelMap> pixmap = Media::PixelMap::Create(opts);

    /**
     * @tc.steps: step2. Call create From pixelMap
     */
    uint32_t errorCode = SUCCESS;
    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(pColorPicker, nullptr);

    /**
     * @tc.steps: step3. Get main color from pixmap
     */
    ColorManager::Color color;
    errorCode = pColorPicker->GetMainColor(color);
    HiLog::Info(LABEL_TEST, "get main color t3[rgba]=%{public}f,%{public}f,%{public}f,%{public}f",
                color.r, color.g, color.b, color.a);
    ASSERT_EQ(errorCode, SUCCESS);
    bool ret = color.ColorEqual(ColorManager::Color(0x00000000U));
    EXPECT_EQ(true, ret);
}

/**
 * @tc.name: GetLargestProportionColor
 * @tc.desc: Ensure the ability of creating effect chain from config file.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ColorPickerUnittest, GetLargestProportionColor, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ColorPickerUnittest GetLargestProportionColor start";
    size_t bufferSize = 0;
    uint8_t *buffer = GetJpgBuffer(bufferSize);
    ASSERT_NE(buffer, nullptr);

    uint32_t errorCode = 0;
    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    std::unique_ptr<ImageSource> imageSource =
        ImageSource::CreateImageSource(buffer, bufferSize, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(imageSource.get(), nullptr);

    DecodeOptions decodeOpts;
    std::unique_ptr<PixelMap> pixmap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    HiLog::Debug(LABEL_TEST, "create pixel map error code=%{public}u.", errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(pixmap.get(), nullptr);

    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    EXPECT_NE(pColorPicker, nullptr);

    ColorManager::Color color;
    errorCode = pColorPicker->GetLargestProportionColor(color);
    HiLog::Info(LABEL_TEST, "get largest proportion color [rgba]=%{public}f,%{public}f,%{public}f,%{public}f",
                color.r, color.g, color.b, color.a);
    ASSERT_EQ(errorCode, SUCCESS);
    bool ret = color.ColorEqual(ColorManager::Color(0.972549f, 0.784314f, 0.0313726f, 1.f));
    EXPECT_EQ(true, ret);
}

/**
 * @tc.name: GetHighestSaturationColor
 * @tc.desc: Ensure the ability of creating effect chain from config file.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ColorPickerUnittest, GetHighestSaturationColor, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ColorPickerUnittest GetHighestSaturationColor start";
    size_t bufferSize = 0;
    uint8_t *buffer = GetJpgBuffer(bufferSize);
    ASSERT_NE(buffer, nullptr);

    uint32_t errorCode = 0;
    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    std::unique_ptr<ImageSource> imageSource =
        ImageSource::CreateImageSource(buffer, bufferSize, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(imageSource.get(), nullptr);

    DecodeOptions decodeOpts;
    std::unique_ptr<PixelMap> pixmap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    HiLog::Debug(LABEL_TEST, "create pixel map error code=%{public}u.", errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(pixmap.get(), nullptr);

    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    EXPECT_NE(pColorPicker, nullptr);

    ColorManager::Color color;
    errorCode = pColorPicker->GetHighestSaturationColor(color);
    HiLog::Info(LABEL_TEST, "get highest saturation color [rgba]=%{public}f,%{public}f,%{public}f,%{public}f",
                color.r, color.g, color.b, color.a);
    ASSERT_EQ(errorCode, SUCCESS);
    bool ret = color.ColorEqual(ColorManager::Color(0.972549f, 0.784314f, 0.0313726f, 1.f));
    EXPECT_EQ(true, ret);
}

/**
 * @tc.name: GetAverageColor
 * @tc.desc: Ensure the ability of creating effect chain from config file.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ColorPickerUnittest, GetAverageColor, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ColorPickerUnittest GetAverageColor start";
    size_t bufferSize = 0;
    uint8_t *buffer = GetJpgBuffer(bufferSize);
    ASSERT_NE(buffer, nullptr);

    uint32_t errorCode = 0;
    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    std::unique_ptr<ImageSource> imageSource =
        ImageSource::CreateImageSource(buffer, bufferSize, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(imageSource.get(), nullptr);

    DecodeOptions decodeOpts;
    std::unique_ptr<PixelMap> pixmap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    HiLog::Debug(LABEL_TEST, "create pixel map error code=%{public}u.", errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(pixmap.get(), nullptr);

    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    EXPECT_NE(pColorPicker, nullptr);

    ColorManager::Color color;
    errorCode = pColorPicker->GetAverageColor(color);
    HiLog::Info(LABEL_TEST, "get average color [rgba]=%{public}f,%{public}f,%{public}f,%{public}f",
                color.r, color.g, color.b, color.a);
    ASSERT_EQ(errorCode, SUCCESS);
    bool ret = color.ColorEqual(ColorManager::Color(0.972549f, 0.784314f, 0.0313726f, 1.f));
    EXPECT_EQ(true, ret);
}

/**
 * @tc.name: GetAverageColor001
 * @tc.desc: GetAverageColor is ERR_EFFECT_INVALID_VALUE
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ColorPickerUnittest, GetAverageColor001, TestSize.Level1)
{
    size_t bufferSize = 0;
    uint8_t* buffer = GetJpgBuffer(bufferSize);
    ASSERT_NE(buffer, nullptr);

    uint32_t errorCode = 0;
    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(buffer, bufferSize, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(imageSource.get(), nullptr);

    DecodeOptions decodeOpts;
    std::unique_ptr<PixelMap> pixmap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    ASSERT_NE(pixmap.get(), nullptr);

    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    EXPECT_NE(pColorPicker, nullptr);

    ColorManager::Color color;
    std::vector<std::pair<uint32_t, uint32_t>> featureColors;
    featureColors.emplace_back(std::pair<uint32_t, uint32_t>(1, 0));
    pColorPicker->featureColors_ = std::move(featureColors);
    errorCode = pColorPicker->GetAverageColor(color);
    ASSERT_EQ(errorCode, ERR_EFFECT_INVALID_VALUE);
}

/**
 * @tc.name: CalcGrayVariance001
 * @tc.desc: CalcGrayVariance is ERR_EFFECT_INVALID_VALUE
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ColorPickerUnittest, CalcGrayVariance001, TestSize.Level1)
{
    size_t bufferSize = 0;
    uint8_t* buffer = GetJpgBuffer(bufferSize);
    ASSERT_NE(buffer, nullptr);

    uint32_t errorCode = 0;
    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(buffer, bufferSize, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(imageSource.get(), nullptr);

    DecodeOptions decodeOpts;
    std::unique_ptr<PixelMap> pixmap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    HiLog::Debug(LABEL_TEST, "create pixel map error code=%{public}u.", errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(pixmap.get(), nullptr);

    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    EXPECT_NE(pColorPicker, nullptr);

    ColorManager::Color color;
    errorCode = pColorPicker->GetAverageColor(color);
    EXPECT_EQ(errorCode, SUCCESS);

    uint32_t ret = pColorPicker->CalcGrayVariance();
    EXPECT_EQ(ret, SUCCESS);
}

/**
 * @tc.name: CalcContrastRatioWithWhite001
 * @tc.desc: lightColorDegree is 0
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ColorPickerUnittest, CalcContrastRatioWithWhite001, TestSize.Level1)
{
    size_t bufferSize = 0;
    uint8_t* buffer = GetJpgBuffer(bufferSize);
    ASSERT_NE(buffer, nullptr);

    uint32_t errorCode = 0;
    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(buffer, bufferSize, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(imageSource.get(), nullptr);

    DecodeOptions decodeOpts;
    std::unique_ptr<PixelMap> pixmap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    HiLog::Debug(LABEL_TEST, "create pixel map error code=%{public}u.", errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(pixmap.get(), nullptr);

    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    EXPECT_NE(pColorPicker, nullptr);

    std::vector<std::pair<uint32_t, uint32_t>> featureColors;
    featureColors.emplace_back(std::pair<uint32_t, uint32_t>(1, 0));
    pColorPicker->featureColors_ = std::move(featureColors);
    pColorPicker->colorValLen_ = 1;
    int ret = pColorPicker->CalcContrastRatioWithWhite();
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.name: DiscriminatePitureLightDegreee001
 * @tc.desc: DiscriminatePitureLightDegree degree is DARK_COLOR_PICTURE
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ColorPickerUnittest, DiscriminatePitureLightDegreee001, TestSize.Level1)
{
    size_t bufferSize = 0;
    uint8_t* buffer = GetJpgBuffer(bufferSize);
    ASSERT_NE(buffer, nullptr);

    uint32_t errorCode = 0;
    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(buffer, bufferSize, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(imageSource.get(), nullptr);

    DecodeOptions decodeOpts;
    std::unique_ptr<PixelMap> pixmap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    HiLog::Debug(LABEL_TEST, "create pixel map error code=%{public}u.", errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(pixmap.get(), nullptr);

    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    EXPECT_NE(pColorPicker, nullptr);

    pColorPicker->grayMsd_ = 5000;
    pColorPicker->contrastToWhite_ = 9;
    PictureLightColorDegree degree = EXTREMELY_LIGHT_COLOR_PICTURE;
    pColorPicker->DiscriminatePitureLightDegree(degree);
    EXPECT_EQ(degree, DARK_COLOR_PICTURE);
}

/**
 * @tc.name: DiscriminatePitureLightDegreee002
 * @tc.desc: DiscriminatePitureLightDegree degree is ERR_EFFECT_INVALID_VALUE
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ColorPickerUnittest, DiscriminatePitureLightDegreee002, TestSize.Level1)
{
    size_t bufferSize = 0;
    uint8_t* buffer = GetJpgBuffer(bufferSize);
    ASSERT_NE(buffer, nullptr);

    uint32_t errorCode = 0;
    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(buffer, bufferSize, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(imageSource.get(), nullptr);

    DecodeOptions decodeOpts;
    std::unique_ptr<PixelMap> pixmap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    HiLog::Debug(LABEL_TEST, "create pixel map error code=%{public}u.", errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(pixmap.get(), nullptr);

    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    EXPECT_NE(pColorPicker, nullptr);

    pColorPicker->featureColors_.clear();
    pColorPicker->grayMsd_ = 5000;
    pColorPicker->contrastToWhite_ = 9;
    PictureLightColorDegree degree = EXTREMELY_LIGHT_COLOR_PICTURE;
    uint32_t ret = pColorPicker->DiscriminatePitureLightDegree(degree);
    EXPECT_EQ(ret, ERR_EFFECT_INVALID_VALUE);
}

/**
 * @tc.name: GetReverseColor001
 * @tc.desc: GetReverseColor is ERR_EFFECT_INVALID_VALUE
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ColorPickerUnittest, GetReverseColor001, TestSize.Level1)
{
    size_t bufferSize = 0;
    uint8_t* buffer = GetJpgBuffer(bufferSize);
    ASSERT_NE(buffer, nullptr);

    uint32_t errorCode = 0;
    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(buffer, bufferSize, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(imageSource.get(), nullptr);

    DecodeOptions decodeOpts;
    std::unique_ptr<PixelMap> pixmap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    HiLog::Debug(LABEL_TEST, "create pixel map error code=%{public}u.", errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(pixmap.get(), nullptr);

    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    EXPECT_NE(pColorPicker, nullptr);

    pColorPicker->featureColors_.clear();
    ColorManager::Color color;
    uint32_t ret = pColorPicker->GetReverseColor(color);
    EXPECT_EQ(ret, ERR_EFFECT_INVALID_VALUE);
}

/**
 * @tc.name: GetMorandiShadowColor001
 * @tc.desc: GetMorandiBackgroundColor is ERR_EFFECT_INVALID_VALUE.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ColorPickerUnittest, GetMorandiShadowColor001, TestSize.Level1)
{
    size_t bufferSize = 0;
    uint8_t* buffer = GetJpgBuffer(bufferSize);
    ASSERT_NE(buffer, nullptr);

    uint32_t errorCode = 0;
    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(buffer, bufferSize, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(imageSource.get(), nullptr);

    DecodeOptions decodeOpts;
    std::unique_ptr<PixelMap> pixmap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    HiLog::Debug(LABEL_TEST, "create pixel map error code=%{public}u.", errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(pixmap.get(), nullptr);

    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    EXPECT_NE(pColorPicker, nullptr);

    std::vector<std::pair<uint32_t, uint32_t>> featureColors;
    featureColors.emplace_back(std::pair<uint32_t, uint32_t>(0, 1));
    pColorPicker->featureColors_ = std::move(featureColors);
    for (int r = 0; r <= 255; ++r) {
        for (int g = 0; g <= 255; ++g) {
            for (int b = 0; b <= 255; ++b) {
                ColorManager::Color cl(r, g, b, 0.f);
                pColorPicker->GetMorandiBackgroundColor(cl);
            }
        }
    }

    for (int r = 0; r <= 10; ++r) {
        for (int g = 0; g <= 10; ++g) {
            for (int b = 0; b <= 10; ++b) {
                ColorManager::Color cl(r / 10, g / 10, b / 10, 1);
                pColorPicker->GetMorandiBackgroundColor(cl);
            }
        }
    }

    pColorPicker->featureColors_.clear();
    ColorManager::Color cl;
    uint32_t ret = pColorPicker->GetMorandiBackgroundColor(cl);
    EXPECT_EQ(ret, ERR_EFFECT_INVALID_VALUE);
}

/**
 * @tc.name: GenerateMorandiShadowColor002
 * @tc.desc: hsv.h > 60 && hsv.h <= 190
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ColorPickerUnittest, GenerateMorandiShadowColor002, TestSize.Level1)
{
    size_t bufferSize = 0;
    uint8_t* buffer = GetJpgBuffer(bufferSize);
    ASSERT_NE(buffer, nullptr);

    uint32_t errorCode = 0;
    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(buffer, bufferSize, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(imageSource.get(), nullptr);

    DecodeOptions decodeOpts;
    std::unique_ptr<PixelMap> pixmap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    HiLog::Debug(LABEL_TEST, "create pixel map error code=%{public}u.", errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(pixmap.get(), nullptr);

    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    EXPECT_NE(pColorPicker, nullptr);

    HSV hsv2 = { 100, 0, 0 };
    pColorPicker->GenerateMorandiShadowColor(hsv2);
}

/**
 * @tc.name: GenerateMorandiShadowColor003
 * @tc.desc: hsv.h > 190 && hsv.h <= 270
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ColorPickerUnittest, GenerateMorandiShadowColor003, TestSize.Level1)
{
    size_t bufferSize = 0;
    uint8_t* buffer = GetJpgBuffer(bufferSize);
    ASSERT_NE(buffer, nullptr);

    uint32_t errorCode = 0;
    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(buffer, bufferSize, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(imageSource.get(), nullptr);

    DecodeOptions decodeOpts;
    std::unique_ptr<PixelMap> pixmap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    HiLog::Debug(LABEL_TEST, "create pixel map error code=%{public}u.", errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(pixmap.get(), nullptr);

    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    EXPECT_NE(pColorPicker, nullptr);

    HSV hsv2 = { 200, 0, 0 };
    pColorPicker->GenerateMorandiShadowColor(hsv2);
}

/**
 * @tc.name: GenerateMorandiShadowColor004
 * @tc.desc: else
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ColorPickerUnittest, GenerateMorandiShadowColor004, TestSize.Level1)
{
    size_t bufferSize = 0;
    uint8_t* buffer = GetJpgBuffer(bufferSize);
    ASSERT_NE(buffer, nullptr);

    uint32_t errorCode = 0;
    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(buffer, bufferSize, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(imageSource.get(), nullptr);

    DecodeOptions decodeOpts;
    std::unique_ptr<PixelMap> pixmap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    HiLog::Debug(LABEL_TEST, "create pixel map error code=%{public}u.", errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(pixmap.get(), nullptr);

    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    EXPECT_NE(pColorPicker, nullptr);

    HSV hsv2 = { 300, 0, 0 };
    pColorPicker->GenerateMorandiShadowColor(hsv2);
}

/**
 * @tc.name: DiscriminateDarkOrBrightColor001
 * @tc.desc: DiscriminateDarkOrBrightColor mode is LOW_SATURATION_BRIGHT_COLOR
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ColorPickerUnittest, DiscriminateDarkOrBrightColor001, TestSize.Level1)
{
    size_t bufferSize = 0;
    uint8_t* buffer = GetJpgBuffer(bufferSize);
    ASSERT_NE(buffer, nullptr);

    uint32_t errorCode = 0;
    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(buffer, bufferSize, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(imageSource.get(), nullptr);

    DecodeOptions decodeOpts;
    std::unique_ptr<PixelMap> pixmap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    HiLog::Debug(LABEL_TEST, "create pixel map error code=%{public}u.", errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(pixmap.get(), nullptr);

    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    EXPECT_NE(pColorPicker, nullptr);

    HSV hsv2 = { 30, 30, 90 };
    ColorBrightnessMode mode = pColorPicker->DiscriminateDarkOrBrightColor(hsv2);
    EXPECT_EQ(mode, ColorBrightnessMode::LOW_SATURATION_BRIGHT_COLOR);
}

/**
 * @tc.name: DiscriminateDarkOrBrightColor002
 * @tc.desc: DiscriminateDarkOrBrightColor mode is LOW_SATURATION_BRIGHT_COLOR
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ColorPickerUnittest, DiscriminateDarkOrBrightColor002, TestSize.Level1)
{
    size_t bufferSize = 0;
    uint8_t* buffer = GetJpgBuffer(bufferSize);
    ASSERT_NE(buffer, nullptr);

    uint32_t errorCode = 0;
    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(buffer, bufferSize, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(imageSource.get(), nullptr);

    DecodeOptions decodeOpts;
    std::unique_ptr<PixelMap> pixmap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    HiLog::Debug(LABEL_TEST, "create pixel map error code=%{public}u.", errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(pixmap.get(), nullptr);

    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    EXPECT_NE(pColorPicker, nullptr);

    HSV hsv2 = { 100, 20, 90 };
    ColorBrightnessMode mode = pColorPicker->DiscriminateDarkOrBrightColor(hsv2);
    EXPECT_EQ(mode, ColorBrightnessMode::LOW_SATURATION_BRIGHT_COLOR);
}

/**
 * @tc.name: DiscriminateDarkOrBrightColor003
 * @tc.desc: DiscriminateDarkOrBrightColor mode is DARK_COLOR
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ColorPickerUnittest, DiscriminateDarkOrBrightColor003, TestSize.Level1)
{
    size_t bufferSize = 0;
    uint8_t* buffer = GetJpgBuffer(bufferSize);
    ASSERT_NE(buffer, nullptr);

    uint32_t errorCode = 0;
    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(buffer, bufferSize, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(imageSource.get(), nullptr);

    DecodeOptions decodeOpts;
    std::unique_ptr<PixelMap> pixmap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    HiLog::Debug(LABEL_TEST, "create pixel map error code=%{public}u.", errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(pixmap.get(), nullptr);

    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    EXPECT_NE(pColorPicker, nullptr);

    HSV hsv2 = { 100, 50, 90 };
    ColorBrightnessMode mode = pColorPicker->DiscriminateDarkOrBrightColor(hsv2);
    EXPECT_EQ(mode, ColorBrightnessMode::DARK_COLOR);
}

/**
 * @tc.name: DiscriminateDarkOrBrightColor004
 * @tc.desc: DiscriminateDarkOrBrightColor mode is LOW_SATURATION_BRIGHT_COLOR
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ColorPickerUnittest, DiscriminateDarkOrBrightColor004, TestSize.Level1)
{
    size_t bufferSize = 0;
    uint8_t* buffer = GetJpgBuffer(bufferSize);
    ASSERT_NE(buffer, nullptr);

    uint32_t errorCode = 0;
    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(buffer, bufferSize, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(imageSource.get(), nullptr);

    DecodeOptions decodeOpts;
    std::unique_ptr<PixelMap> pixmap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    HiLog::Debug(LABEL_TEST, "create pixel map error code=%{public}u.", errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(pixmap.get(), nullptr);

    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    EXPECT_NE(pColorPicker, nullptr);

    HSV hsv2 = { 100, 20, 90 };
    ColorBrightnessMode mode = pColorPicker->DiscriminateDarkOrBrightColor(hsv2);
    EXPECT_EQ(mode, ColorBrightnessMode::LOW_SATURATION_BRIGHT_COLOR);
}

/**
 * @tc.name: DiscriminateDarkOrBrightColor005
 * @tc.desc: Ensure the ability of creating effect chain from config file.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ColorPickerUnittest, DiscriminateDarkOrBrightColor005, TestSize.Level1)
{
    size_t bufferSize = 0;
    uint8_t* buffer = GetJpgBuffer(bufferSize);
    ASSERT_NE(buffer, nullptr);

    uint32_t errorCode = 0;
    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(buffer, bufferSize, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(imageSource.get(), nullptr);

    DecodeOptions decodeOpts;
    std::unique_ptr<PixelMap> pixmap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    HiLog::Debug(LABEL_TEST, "create pixel map error code=%{public}u.", errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(pixmap.get(), nullptr);

    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    EXPECT_NE(pColorPicker, nullptr);

    HSV hsv2 = { 10, 60, 90 };
    ColorBrightnessMode mode = pColorPicker->DiscriminateDarkOrBrightColor(hsv2);
    EXPECT_EQ(mode, ColorBrightnessMode::HIGH_SATURATION_BRIGHT_COLOR);
}

/**
 * @tc.name: AdjustToBasicColor001
 * @tc.desc: AdjustToBasicColor  x <= y
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ColorPickerUnittest, AdjustToBasicColor001, TestSize.Level1)
{
    size_t bufferSize = 0;
    uint8_t* buffer = GetJpgBuffer(bufferSize);
    ASSERT_NE(buffer, nullptr);

    uint32_t errorCode = 0;
    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(buffer, bufferSize, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(imageSource.get(), nullptr);

    DecodeOptions decodeOpts;
    std::unique_ptr<PixelMap> pixmap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    HiLog::Debug(LABEL_TEST, "create pixel map error code=%{public}u.", errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(pixmap.get(), nullptr);

    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    EXPECT_NE(pColorPicker, nullptr);

    HSV hsv2 = { 10, 10, 10 };
    double basicS = 20.f;
    double basicV = 20.f;
    pColorPicker->AdjustToBasicColor(hsv2, basicS, basicV);
}

/**
 * @tc.name: ProcessToDarkColor001
 * @tc.desc: hsv.h > 69 && hsv.h <= 189
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ColorPickerUnittest, ProcessToDarkColor001, TestSize.Level1)
{
    size_t bufferSize = 0;
    uint8_t* buffer = GetJpgBuffer(bufferSize);
    ASSERT_NE(buffer, nullptr);

    uint32_t errorCode = 0;
    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(buffer, bufferSize, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(imageSource.get(), nullptr);

    DecodeOptions decodeOpts;
    std::unique_ptr<PixelMap> pixmap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    HiLog::Debug(LABEL_TEST, "create pixel map error code=%{public}u.", errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(pixmap.get(), nullptr);

    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    EXPECT_NE(pColorPicker, nullptr);

    HSV hsv2 = { 80, 10, 10 };
    pColorPicker->ProcessToDarkColor(hsv2);
}

/**
 * @tc.name: ProcessToDarkColor002
 * @tc.desc: hsv.h > 189 && hsv.h <= 269
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ColorPickerUnittest, ProcessToDarkColor002, TestSize.Level1)
{
    size_t bufferSize = 0;
    uint8_t* buffer = GetJpgBuffer(bufferSize);
    ASSERT_NE(buffer, nullptr);

    uint32_t errorCode = 0;
    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(buffer, bufferSize, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(imageSource.get(), nullptr);

    DecodeOptions decodeOpts;
    std::unique_ptr<PixelMap> pixmap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    HiLog::Debug(LABEL_TEST, "create pixel map error code=%{public}u.", errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(pixmap.get(), nullptr);

    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    EXPECT_NE(pColorPicker, nullptr);

    HSV hsv2 = { 190, 10, 10 };
    pColorPicker->ProcessToDarkColor(hsv2);
}

/**
 * @tc.name: ProcessToDarkColor003
 * @tc.desc:else
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ColorPickerUnittest, ProcessToDarkColor003, TestSize.Level1)
{
    size_t bufferSize = 0;
    uint8_t* buffer = GetJpgBuffer(bufferSize);
    ASSERT_NE(buffer, nullptr);

    uint32_t errorCode = 0;
    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(buffer, bufferSize, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(imageSource.get(), nullptr);

    DecodeOptions decodeOpts;
    std::unique_ptr<PixelMap> pixmap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(pixmap.get(), nullptr);

    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    EXPECT_NE(pColorPicker, nullptr);

    HSV hsv2 = { 280, 10, 10 };
    pColorPicker->ProcessToDarkColor(hsv2);
}

/**
 * @tc.name: AdjustLowSaturationBrightColor_001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ColorPickerUnittest, AdjustLowSaturationBrightColor_001, TestSize.Level1)
{
    size_t bufferSize = 0;
    uint8_t* buffer = GetJpgBuffer(bufferSize);
    ASSERT_NE(buffer, nullptr);

    uint32_t errorCode = 0;
    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(buffer, bufferSize, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(imageSource.get(), nullptr);

    DecodeOptions decodeOpts;
    std::unique_ptr<PixelMap> pixmap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    HiLog::Debug(LABEL_TEST, "create pixel map error code=%{public}u.", errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(pixmap.get(), nullptr);

    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    EXPECT_NE(pColorPicker, nullptr);

    HSV colorHsv = { 30, 5, 90 };
    HSV mainHsv = { 30, 30, 90 };
    HSV secondaryHsv = { 30, 30, 90 };

    const std::pair<uint32_t, uint32_t> primaryColor;
    const std::pair<uint32_t, uint32_t> secondaryColor;
    pColorPicker->AdjustLowSaturationBrightColor(colorHsv, mainHsv, secondaryHsv, primaryColor, secondaryColor);
}

/**
 * @tc.name: AdjustLowSaturationBrightColor002
 * @tc.desc: primaryColor.second - secondaryColor.second < colorValLen_ / 10.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ColorPickerUnittest, AdjustLowSaturationBrightColor002, TestSize.Level1)
{
    size_t bufferSize = 0;
    uint8_t* buffer = GetJpgBuffer(bufferSize);
    ASSERT_NE(buffer, nullptr);

    uint32_t errorCode = 0;
    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(buffer, bufferSize, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(imageSource.get(), nullptr);

    DecodeOptions decodeOpts;
    std::unique_ptr<PixelMap> pixmap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    HiLog::Debug(LABEL_TEST, "create pixel map error code=%{public}u.", errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(pixmap.get(), nullptr);

    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    EXPECT_NE(pColorPicker, nullptr);

    HSV colorHsv = { 30, 5, 90 };
    HSV mainHsv = { 30, 30, 90 };
    HSV secondaryHsv = { 100, 50, 90 };

    std::pair<uint32_t, uint32_t> primaryColor(0, 20);
    std::pair<uint32_t, uint32_t> secondaryColor(1, 10);
    pColorPicker->colorValLen_ = 200;
    pColorPicker->AdjustLowSaturationBrightColor(colorHsv, mainHsv, secondaryHsv, primaryColor, secondaryColor);
}

/**
 * @tc.name: GetImmersiveBackgroundColor001
 * @tc.desc: featureColors_ is empty.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ColorPickerUnittest, GetImmersiveBackgroundColor001, TestSize.Level1)
{
    size_t bufferSize = 0;
    uint8_t* buffer = GetJpgBuffer(bufferSize);
    ASSERT_NE(buffer, nullptr);

    uint32_t errorCode = 0;
    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(buffer, bufferSize, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(imageSource.get(), nullptr);

    DecodeOptions decodeOpts;
    std::unique_ptr<PixelMap> pixmap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    HiLog::Debug(LABEL_TEST, "create pixel map error code=%{public}u.", errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(pixmap.get(), nullptr);

    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    EXPECT_NE(pColorPicker, nullptr);

    ColorManager::Color color;
    pColorPicker->featureColors_.clear();
    uint32_t ret = pColorPicker->GetImmersiveBackgroundColor(color);
    EXPECT_EQ(ret, ERR_EFFECT_INVALID_VALUE);
}

/**
 * @tc.name: GetImmersiveForegroundColor001
 * @tc.desc: featureColors_ is empty.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ColorPickerUnittest, GetImmersiveForegroundColor001, TestSize.Level1)
{
    size_t bufferSize = 0;
    uint8_t* buffer = GetJpgBuffer(bufferSize);
    ASSERT_NE(buffer, nullptr);

    uint32_t errorCode = 0;
    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(buffer, bufferSize, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(imageSource.get(), nullptr);

    DecodeOptions decodeOpts;
    std::unique_ptr<PixelMap> pixmap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    HiLog::Debug(LABEL_TEST, "create pixel map error code=%{public}u.", errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(pixmap.get(), nullptr);

    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    EXPECT_NE(pColorPicker, nullptr);

    ColorManager::Color color;
    pColorPicker->featureColors_.clear();
    uint32_t ret = pColorPicker->GetImmersiveForegroundColor(color);
    EXPECT_EQ(ret, ERR_EFFECT_INVALID_VALUE);
}

/**
 * @tc.name: GetDominantColor001
 * @tc.desc: featureColors_ is empty.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ColorPickerUnittest, GetDominantColor001, TestSize.Level1)
{
    size_t bufferSize = 0;
    uint8_t* buffer = GetJpgBuffer(bufferSize);
    ASSERT_NE(buffer, nullptr);

    uint32_t errorCode = 0;
    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(buffer, bufferSize, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(imageSource.get(), nullptr);

    DecodeOptions decodeOpts;
    std::unique_ptr<PixelMap> pixmap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    HiLog::Debug(LABEL_TEST, "create pixel map error code=%{public}u.", errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(pixmap.get(), nullptr);

    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    EXPECT_NE(pColorPicker, nullptr);

    pColorPicker->featureColors_.clear();
    std::pair<uint32_t, uint32_t> primaryColor;
    std::pair<uint32_t, uint32_t> secondaryColor;
    bool ret = pColorPicker->GetDominantColor(primaryColor, secondaryColor);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name: GetDominantColor002
 * @tc.desc: featureColors_ size is 2.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ColorPickerUnittest, GetDominantColor002, TestSize.Level1)
{
    size_t bufferSize = 0;
    uint8_t* buffer = GetJpgBuffer(bufferSize);
    ASSERT_NE(buffer, nullptr);

    uint32_t errorCode = 0;
    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(buffer, bufferSize, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(imageSource.get(), nullptr);

    DecodeOptions decodeOpts;
    std::unique_ptr<PixelMap> pixmap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    HiLog::Debug(LABEL_TEST, "create pixel map error code=%{public}u.", errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(pixmap.get(), nullptr);

    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    EXPECT_NE(pColorPicker, nullptr);

    std::pair<uint32_t, uint32_t> featureColor;
    featureColor.first = 0;
    featureColor.second = 20;
    pColorPicker->featureColors_.emplace_back(featureColor);
    featureColor.first = 1;
    featureColor.second = 10;
    pColorPicker->featureColors_.emplace_back(featureColor);
    pColorPicker->colorValLen_ = 20;
    std::pair<uint32_t, uint32_t> primaryColor;
    std::pair<uint32_t, uint32_t> secondaryColor;
    bool ret = pColorPicker->GetDominantColor(primaryColor, secondaryColor);
    EXPECT_EQ(ret, true);

    pColorPicker->featureColors_.clear();
    featureColor.first = 0;
    featureColor.second = 10;
    pColorPicker->featureColors_.emplace_back(featureColor);
    featureColor.first = 1;
    featureColor.second = 10;
    pColorPicker->featureColors_.emplace_back(featureColor);
    ret = pColorPicker->GetDominantColor(primaryColor, secondaryColor);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name: GetDeepenImmersionColor001
 * @tc.desc: featureColors_ is empty.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ColorPickerUnittest, GetDeepenImmersionColor001, TestSize.Level1)
{
    size_t bufferSize = 0;
    uint8_t* buffer = GetJpgBuffer(bufferSize);
    ASSERT_NE(buffer, nullptr);

    uint32_t errorCode = 0;
    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(buffer, bufferSize, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(imageSource.get(), nullptr);

    DecodeOptions decodeOpts;
    std::unique_ptr<PixelMap> pixmap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    HiLog::Debug(LABEL_TEST, "create pixel map error code=%{public}u.", errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(pixmap.get(), nullptr);

    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    EXPECT_NE(pColorPicker, nullptr);

    pColorPicker->featureColors_.clear();
    ColorManager::Color color;
    uint32_t ret = pColorPicker->GetDeepenImmersionColor(color);
    EXPECT_EQ(ret, ERR_EFFECT_INVALID_VALUE);
}

/**
 * @tc.name: IsBlackOrWhiteOrGrayColor
 * @tc.desc: Ensure the ability of creating effect chain from config file.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ColorPickerUnittest, IsBlackOrWhiteOrGrayColor, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ColorPickerUnittest IsBlackOrWhiteOrGrayColor start";
    size_t bufferSize = 0;
    uint8_t *buffer = GetJpgBuffer(bufferSize);
    ASSERT_NE(buffer, nullptr);

    uint32_t errorCode = 0;
    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    std::unique_ptr<ImageSource> imageSource =
        ImageSource::CreateImageSource(buffer, bufferSize, opts, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(imageSource.get(), nullptr);

    DecodeOptions decodeOpts;
    std::unique_ptr<PixelMap> pixmap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    HiLog::Debug(LABEL_TEST, "create pixel map error code=%{public}u.", errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(pixmap.get(), nullptr);

    std::shared_ptr<ColorPicker> pColorPicker = ColorPicker::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    EXPECT_NE(pColorPicker, nullptr);

    bool judgeRst = pColorPicker->IsBlackOrWhiteOrGrayColor(0xFFFFFFFF);
    HiLog::Info(LABEL_TEST, "get largest proportion color result=%{public}d", judgeRst);
    ASSERT_EQ(judgeRst, true);
}

/**
 * @tc.name: AdjustHSVToDefinedInterval
 * @tc.desc: check hsv to defined interval.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ColorPickerUnittest, AdjustHSVToDefinedInterval, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ColorPickerUnittest AdjustHSVToDefinedInterval start";

    std::shared_ptr<ColorPicker> pColorPicker = CreateColorPicker();
    ASSERT_NE(pColorPicker, nullptr);

    HSV gHsv {361, 101, 101}; // 361,101,101 invalid hsv
    pColorPicker->AdjustHSVToDefinedInterval(gHsv);
    EXPECT_EQ(gHsv.h, 360); // 360 is valid hue
    EXPECT_EQ(gHsv.s, 100); // 100 is valid saturation
    EXPECT_EQ(gHsv.v, 100); // 100 is valid value

    HSV lHsv {-1, -1, -1}; // -1, -1, -1 invalid hsv
    pColorPicker->AdjustHSVToDefinedInterval(lHsv);
    EXPECT_EQ(lHsv.h, 0); // 0 is valid hue
    EXPECT_EQ(lHsv.s, 0); // 0 is valid saturation
    EXPECT_EQ(lHsv.v, 0); // 0 is valid value

    GTEST_LOG_(INFO) << "ColorPickerUnittest AdjustHSVToDefinedInterval end";
}
} // namespace Rosen
} // namespace OHOS
