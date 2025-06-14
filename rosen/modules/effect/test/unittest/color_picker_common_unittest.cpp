/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "color_picker_common_unittest.h"

#include "color.h"
#include "color_picker.h"
#include "effect_errors.h"
#include "hilog/log.h"
#include "image_source.h"
#include "pixel_map.h"
#include "test_picture_files.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Media;
using namespace OHOS::HiviewDFX;

static constexpr OHOS::HiviewDFX::HiLogLabel LABEL_TEST = { LOG_CORE, LOG_DOMAIN, "ColorPickerCommonTest" };

namespace OHOS {
namespace Rosen {

std::shared_ptr<ColorPickerCommon> ColorPickerCommonUnittest::CreateColorPicker()
{
    size_t bufferSize = 0;
    uint8_t* buffer = GetJpgBuffer(bufferSize);
    if (buffer == nullptr) {
        return nullptr;
    }

    uint32_t errorCode = 0;
    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(buffer, bufferSize, opts, errorCode);
    if ((errorCode != SUCCESS) || (imageSource == nullptr)) {
        return nullptr;
    }

    DecodeOptions decodeOpts;
    std::unique_ptr<PixelMap> pixmap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    if ((errorCode != SUCCESS) || (pixmap == nullptr)) {
        return nullptr;
    }

    return ColorPickerCommon::CreateColorPicker(std::move(pixmap), errorCode);
}
/**
 * @tc.name: CreateColorPickerFromPixelmapTest001
 * @tc.desc: Ensure the ability of creating color picker from pixelmap.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ColorPickerCommonUnittest, CreateColorPickerFromPixelmapTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ColorPickerCommonUnittest CreateColorPickerFromPixelmapTest001 start";
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
    std::shared_ptr<ColorPickerCommon> pColorPicker =
        ColorPickerCommon::CreateColorPicker(std::move(pixmap), errorCode);
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
HWTEST_F(ColorPickerCommonUnittest, CreateColorPickerFromPixelmapTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ColorPickerCommonUnittest CreateColorPickerFromPixelmapTest002 start";
    size_t bufferSize = 0;
    uint8_t* buffer = GetPngBuffer(bufferSize);
    ASSERT_NE(buffer, nullptr);

    /**
     * @tc.steps: step1. Create a ImageSource
     */
    uint32_t errorCode = 0;
    SourceOptions opts;
    opts.formatHint = "image/png";
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(buffer, bufferSize, opts, errorCode);
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
    std::shared_ptr<ColorPickerCommon> pColorPicker =
        ColorPickerCommon::CreateColorPicker(std::move(pixmap), errorCode);
    EXPECT_NE(pColorPicker, nullptr);
}

/**
 * @tc.name: CreateColorPickerFromPixelmapTest003
 * @tc.desc: Ensure the ability of creating effect chain from config file.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ColorPickerCommonUnittest, CreateColorPickerFromPixelmapTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ColorPickerCommonUnittest CreateColorPickerFromPixelmapTest003 start";
    /**
     * @tc.steps: step1. Create a pixelMap
     */
    std::unique_ptr<Media::PixelMap> pixmap = nullptr;

    /**
     * @tc.steps: step2. Call create From pixelMap
     */
    uint32_t errorCode = SUCCESS;
    std::shared_ptr<ColorPickerCommon> pColorPicker =
        ColorPickerCommon::CreateColorPicker(std::move(pixmap), errorCode);
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
HWTEST_F(ColorPickerCommonUnittest, CreateColorPickerFromPixelmapTest004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ColorPickerCommonUnittest CreateColorPickerFromPixelmapTest004 start";
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
    std::vector<double> region = { 0, 0, 0.5, 0.5 };
    std::shared_ptr<ColorPickerCommon> pColorPicker =
        ColorPickerCommon::CreateColorPicker(std::move(pixmap), region, errorCode);
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
HWTEST_F(ColorPickerCommonUnittest, CreateColorPickerFromPixelmapTest005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ColorPickerCommonUnittest CreateColorPickerFromPixelmapTest005 start";
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
    std::vector<double> region = { 0, 0, 0.5, 0.5 };
    std::shared_ptr<ColorPickerCommon> pColorPicker =
        ColorPickerCommon::CreateColorPicker(std::move(pixmap), region, errorCode);
    EXPECT_NE(pColorPicker, nullptr);
}

/**
 * @tc.name: GetMainColorTest001
 * @tc.desc: Ensure the ability of creating effect chain from config file.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ColorPickerCommonUnittest, GetMainColorTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ColorPickerCommonUnittest GetMainColorTest001 start";
    size_t bufferSize = 0;
    uint8_t* buffer = GetJpgBuffer(bufferSize);
    ASSERT_NE(buffer, nullptr);

    /**
     * @tc.steps: step1. create image source by correct jpeg file path and jpeg format hit.
     * @tc.expected: step1. create image source success.
     */
    uint32_t errorCode = 0;
    SourceOptions opts;
    opts.formatHint = "image/jpeg";
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(buffer, bufferSize, opts, errorCode);
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
    std::shared_ptr<ColorPickerCommon> pColorPicker =
        ColorPickerCommon::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    EXPECT_NE(pColorPicker, nullptr);

    /**
     * @tc.steps: step3. Get main color from pixmap
     */
    ColorManager::Color color;
    errorCode = pColorPicker->GetMainColor(color);
    HiLog::Info(LABEL_TEST, "get main color t1[rgba]=%{public}f,%{public}f,%{public}f,%{public}f", color.r, color.g,
        color.b, color.a);
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
HWTEST_F(ColorPickerCommonUnittest, GetMainColorTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ColorPickerCommonUnittest GetMainColorTest002 start";
    size_t bufferSize = 0;
    uint8_t* buffer = GetPngBuffer(bufferSize);
    ASSERT_NE(buffer, nullptr);

    /**
     * @tc.steps: step1. Create a ImageSource
     */
    uint32_t errorCode = 0;
    SourceOptions opts;
    opts.formatHint = "image/png";
    std::unique_ptr<ImageSource> imageSource = ImageSource::CreateImageSource(buffer, bufferSize, opts, errorCode);
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
    std::shared_ptr<ColorPickerCommon> pColorPicker =
        ColorPickerCommon::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(pColorPicker, nullptr);

    /**
     * @tc.steps: step4. Get main color from pixmap
     */
    ColorManager::Color color;
    errorCode = pColorPicker->GetMainColor(color);
    HiLog::Info(LABEL_TEST, "get main color t2[rgba]=%{public}f,%{public}f,%{public}f,%{public}f", color.r, color.g,
        color.b, color.a);
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
HWTEST_F(ColorPickerCommonUnittest, GetMainColorTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ColorPickerCommonUnittest GetMainColorTest003 start";
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
    std::shared_ptr<ColorPickerCommon> pColorPicker =
        ColorPickerCommon::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    ASSERT_NE(pColorPicker, nullptr);

    /**
     * @tc.steps: step3. Get main color from pixmap
     */
    ColorManager::Color color;
    errorCode = pColorPicker->GetMainColor(color);
    HiLog::Info(LABEL_TEST, "get main color t3[rgba]=%{public}f,%{public}f,%{public}f,%{public}f", color.r, color.g,
        color.b, color.a);
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
HWTEST_F(ColorPickerCommonUnittest, GetLargestProportionColor, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ColorPickerCommonUnittest GetLargestProportionColor start";
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

    std::shared_ptr<ColorPickerCommon> pColorPicker =
        ColorPickerCommon::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    EXPECT_NE(pColorPicker, nullptr);

    ColorManager::Color color;
    errorCode = pColorPicker->GetLargestProportionColor(color);
    HiLog::Info(LABEL_TEST, "get largest proportion color [rgba]=%{public}f,%{public}f,%{public}f,%{public}f", color.r,
        color.g, color.b, color.a);
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
HWTEST_F(ColorPickerCommonUnittest, GetHighestSaturationColor, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ColorPickerCommonUnittest GetHighestSaturationColor start";
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

    std::shared_ptr<ColorPickerCommon> pColorPicker =
        ColorPickerCommon::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    EXPECT_NE(pColorPicker, nullptr);

    ColorManager::Color color;
    errorCode = pColorPicker->GetHighestSaturationColor(color);
    HiLog::Info(LABEL_TEST, "get highest saturation color [rgba]=%{public}f,%{public}f,%{public}f,%{public}f", color.r,
        color.g, color.b, color.a);
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
HWTEST_F(ColorPickerCommonUnittest, GetAverageColor, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ColorPickerCommonUnittest GetAverageColor start";
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

    std::shared_ptr<ColorPickerCommon> pColorPicker =
        ColorPickerCommon::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    EXPECT_NE(pColorPicker, nullptr);

    ColorManager::Color color;
    errorCode = pColorPicker->GetAverageColor(color);
    HiLog::Info(LABEL_TEST, "get average color [rgba]=%{public}f,%{public}f,%{public}f,%{public}f", color.r, color.g,
        color.b, color.a);
    ASSERT_EQ(errorCode, SUCCESS);
    bool ret = color.ColorEqual(ColorManager::Color(0.972549f, 0.784314f, 0.0313726f, 1.f));
    EXPECT_EQ(true, ret);
}

/**
 * @tc.name: IsBlackOrWhiteOrGrayColor
 * @tc.desc: Ensure the ability of creating effect chain from config file.
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(ColorPickerCommonUnittest, IsBlackOrWhiteOrGrayColor, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ColorPickerCommonUnittest IsBlackOrWhiteOrGrayColor start";
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

    std::shared_ptr<ColorPickerCommon> pColorPicker =
        ColorPickerCommon::CreateColorPicker(std::move(pixmap), errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    EXPECT_NE(pColorPicker, nullptr);

    bool judgeRst = pColorPicker->IsBlackOrWhiteOrGrayColor(0xFFFFFFFF, errorCode);
    ASSERT_EQ(errorCode, SUCCESS);
    HiLog::Info(LABEL_TEST, "get largest proportion color result=%{public}d", judgeRst);
    ASSERT_EQ(judgeRst, true);
}
} // namespace Rosen
} // namespace OHOS