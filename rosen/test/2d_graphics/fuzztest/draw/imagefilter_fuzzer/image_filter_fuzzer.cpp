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

#include "image_filter_fuzzer.h"
#include <cstddef>
#include <cstdint>
#include "get_object.h"
#include "effect/image_filter.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t FILTERTYPE_SIZE = 9;
constexpr size_t TITLEMODE_SIZE = 4;
constexpr size_t BLENDMODE_SIZE = 29;
constexpr size_t BLURTYPE_SIZE = 2;
constexpr size_t GRADIENTDIR_SIZE = 8;
} // namespace
namespace Drawing {

bool ImageFilterFuzzTest001(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<ColorFilter> colorFilter = ColorFilter::CreateLinearToSrgbGamma();
    scalar sigmaX = GetObject<scalar>();
    scalar sigmaY = GetObject<scalar>();
    std::shared_ptr<ImageFilter> imageFilter = ImageFilter::CreateColorBlurImageFilter(
        *colorFilter, sigmaX, sigmaY);
    scalar sigmaX2 = GetObject<scalar>();
    scalar sigmaY2 = GetObject<scalar>();
    Rect noCropRect = {
        -std::numeric_limits<scalar>::infinity(), -std::numeric_limits<scalar>::infinity(),
        std::numeric_limits<scalar>::infinity(), std::numeric_limits<scalar>::infinity()
    };
    imageFilter->InitWithColorBlur(*colorFilter, sigmaX2, sigmaY2, ImageBlurType::GAUSS, noCropRect);
    imageFilter->Serialize();
    imageFilter->Deserialize(nullptr);
    imageFilter->GetType();
    imageFilter->GetDrawingType();

    return true;
}

bool ImageFilterFuzzTest002(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<ColorFilter> colorFilter = ColorFilter::CreateLinearToSrgbGamma();
    scalar sigmaX = GetObject<scalar>();
    scalar sigmaY = GetObject<scalar>();
    std::shared_ptr<ImageFilter> imageFilterOne = ImageFilter::CreateColorBlurImageFilter(*colorFilter, sigmaX, sigmaY);
    uint32_t mode = GetObject<uint32_t>();
    std::shared_ptr<ImageFilter> imageFilterTwo = ImageFilter::CreateBlurImageFilter(sigmaX, sigmaY,
        static_cast<TileMode>(mode % TITLEMODE_SIZE), imageFilterOne);
    std::shared_ptr<ImageFilter> imageFilterThree = ImageFilter::CreateColorFilterImageFilter(*colorFilter,
        imageFilterOne);
    scalar dx = GetObject<scalar>();
    scalar dy = GetObject<scalar>();
    std::shared_ptr<ImageFilter> imageFilterFour = ImageFilter::CreateOffsetImageFilter(dx, dy, imageFilterOne);
    std::shared_ptr<ImageFilter> imageFilterFive = ImageFilter::CreateComposeImageFilter(imageFilterOne,
        imageFilterTwo);
    uint32_t blendMode = GetObject<uint32_t>();
    std::shared_ptr<ImageFilter> imageFilterSix = ImageFilter::CreateBlendImageFilter(
        static_cast<BlendMode>(blendMode % BLENDMODE_SIZE), imageFilterOne, imageFilterTwo);
    return true;
}

bool ImageFilterFuzzTest003(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    std::shared_ptr<ColorFilter> colorFilter = ColorFilter::CreateLinearToSrgbGamma();
    scalar sigmaX = GetObject<scalar>();
    scalar sigmaY = GetObject<scalar>();
    std::shared_ptr<ImageFilter> imageFilterOne = ImageFilter::CreateColorBlurImageFilter(*colorFilter, sigmaX, sigmaY);
    uint32_t mode = GetObject<uint32_t>();
    uint32_t type = GetObject<uint32_t>();
    scalar x = GetObject<scalar>();
    scalar y = GetObject<scalar>();
    ImageFilter imageFilterTwo = ImageFilter(static_cast<ImageFilter::FilterType>(type % FILTERTYPE_SIZE), x, y,
        imageFilterOne);
    uint32_t blurType = GetObject<uint32_t>();
    ImageFilter imageFilterThree = ImageFilter(static_cast<ImageFilter::FilterType>(type % FILTERTYPE_SIZE), x, y,
        static_cast<TileMode>(mode % TITLEMODE_SIZE), imageFilterOne,
        static_cast<ImageBlurType>(blurType % BLURTYPE_SIZE));
    ImageFilter imageFilterFour = ImageFilter(static_cast<ImageFilter::FilterType>(type % FILTERTYPE_SIZE),
        *colorFilter, imageFilterOne);
    ImageFilter imageFilterFive = ImageFilter(static_cast<ImageFilter::FilterType>(type % FILTERTYPE_SIZE),
        *colorFilter, x, y, static_cast<ImageBlurType>(blurType % BLURTYPE_SIZE));
    ImageFilter imageFilterSix = ImageFilter(static_cast<ImageFilter::FilterType>(type % FILTERTYPE_SIZE),
        imageFilterOne, imageFilterOne);
    ImageFilter imageFilterSeven = ImageFilter(static_cast<ImageFilter::FilterType>(type % FILTERTYPE_SIZE));
    uint32_t blendMode = GetObject<uint32_t>();
    ImageFilter imageFilterEight = ImageFilter(static_cast<ImageFilter::FilterType>(type % FILTERTYPE_SIZE),
        static_cast<BlendMode>(blendMode % BLENDMODE_SIZE), imageFilterOne, imageFilterOne);
    float left = GetObject<float>();
    float top = GetObject<float>();
    float right = GetObject<float>();
    float bottom = GetObject<float>();
    RectF rect {
        left,
        top,
        right,
        bottom,
    };
    ColorQuad colorQuad = GetObject<ColorQuad>();
    std::shared_ptr<ShaderEffect> shaderEffect = ShaderEffect::CreateColorShader(colorQuad);
    ImageFilter imageFilterNine = ImageFilter(static_cast<ImageFilter::FilterType>(type % FILTERTYPE_SIZE),
        shaderEffect, rect);
    return true;
}

bool ImageFilterFuzzTest004(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<ColorFilter> colorFilter = ColorFilter::CreateLinearToSrgbGamma();
    scalar sigmaX = GetObject<scalar>();
    scalar sigmaY = GetObject<scalar>();
    std::shared_ptr<ImageFilter> imageFilterOne = ImageFilter::CreateColorBlurImageFilter(*colorFilter, sigmaX, sigmaY);
    uint32_t mode = GetObject<uint32_t>();
    std::shared_ptr<ImageFilter> imageFilterTwo = ImageFilter::CreateBlurImageFilter(sigmaX, sigmaY,
        static_cast<TileMode>(mode % TITLEMODE_SIZE), imageFilterOne);
    std::shared_ptr<ImageFilter> imageFilterThree = ImageFilter::CreateColorFilterImageFilter(*colorFilter,
        imageFilterOne);
    std::vector<scalar> coefficients = {};
    bool enforcePMColor = GetObject<bool>();
    std::shared_ptr<ImageFilter> imageFilter = ImageFilter::CreateArithmeticImageFilter(coefficients,
        enforcePMColor, imageFilterOne, imageFilterTwo);
    float vOne = GetObject<float>();
    float vTwo = GetObject<float>();
    float vThree = GetObject<float>();
    float vFour = GetObject<float>();
    std::vector<std::pair<float, float>> fractionStops { {vOne, vTwo}, {vThree, vFour} };
    float radius = GetObject<float>();
    uint32_t direction = GetObject<uint32_t>();
    uint32_t blurType = GetObject<uint32_t>();
    std::shared_ptr<ImageFilter> imageFilterFour = ImageFilter::CreateGradientBlurImageFilter(radius, fractionStops,
        static_cast<GradientDir>(direction % GRADIENTDIR_SIZE), static_cast<GradientBlurType>(blurType % BLURTYPE_SIZE),
        imageFilterOne);
    uint32_t type = GetObject<uint32_t>();
    ImageFilter imageFilterFive = ImageFilter(static_cast<ImageFilter::FilterType>(type % FILTERTYPE_SIZE),
        coefficients, enforcePMColor, imageFilterOne, imageFilterTwo);
    ImageFilter imageFilterSix = ImageFilter(static_cast<ImageFilter::FilterType>(type % FILTERTYPE_SIZE),
        radius, fractionStops, static_cast<GradientDir>(direction % GRADIENTDIR_SIZE),
        static_cast<GradientBlurType>(blurType % BLURTYPE_SIZE), imageFilterOne);
    return true;
}

bool ImageFilterFuzzTest005(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    float left = GetObject<float>();
    float top = GetObject<float>();
    float right = GetObject<float>();
    float bottom = GetObject<float>();
    RectF rect {
        left,
        top,
        right,
        bottom,
    };
    ColorQuad colorQuad = GetObject<ColorQuad>();
    std::shared_ptr<ShaderEffect> shaderEffect = ShaderEffect::CreateColorShader(colorQuad);
    std::shared_ptr<ImageFilter> imageFilterOne = ImageFilter::CreateShaderImageFilter(shaderEffect, rect);
    return true;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::ImageFilterFuzzTest001(data, size);
    OHOS::Rosen::Drawing::ImageFilterFuzzTest002(data, size);
    OHOS::Rosen::Drawing::ImageFilterFuzzTest003(data, size);
    OHOS::Rosen::Drawing::ImageFilterFuzzTest004(data, size);
    OHOS::Rosen::Drawing::ImageFilterFuzzTest005(data, size);
    return 0;
}
