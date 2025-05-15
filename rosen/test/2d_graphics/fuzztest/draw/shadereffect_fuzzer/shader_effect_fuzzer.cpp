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

#include "shader_effect_fuzzer.h"
#include <cstddef>
#include <cstdint>
#include "get_object.h"
#include "effect/shader_effect.h"
#include "image/bitmap.h"
#include "image/image.h"
#include "utils/matrix.h"
#include "utils/scalar.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
namespace {
    constexpr uint32_t MAX_ARRAY_SIZE = 5000;
    constexpr size_t FUNCTYPE_SIZE = 4;
    constexpr size_t MATRIXTYPE_SIZE = 5;
    constexpr size_t SHADEREFFECTTYPE_SIZE = 11;
    constexpr size_t BLENDMODE_SIZE = 29;
    constexpr size_t TILEMODE_SIZE = 4;
    constexpr size_t FILTERMODE_SIZE = 2;
} // namespace

/*
 * 测试以下 ShaderEffect 接口：
 * 1. CreateColorShader(...)
 * 2. Deserialize(...)
 * 3. Serialize()
 * 4. GetType()
 * 5. GetDrawingType()
 */
bool ShaderEffectFuzzTest001(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    ColorQuad color = GetObject<ColorQuad>();
    std::shared_ptr<ShaderEffect> shaderEffect = ShaderEffect::CreateColorShader(color);
    auto dataVal = std::make_shared<Data>();
    size_t length = GetObject<size_t>() % MAX_ARRAY_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    dataVal->BuildWithoutCopy(dataText, length);
    shaderEffect->Deserialize(dataVal);
    shaderEffect->Serialize();
    shaderEffect->GetType();
    shaderEffect->GetDrawingType();
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }

    return true;
}

/*
 * 测试以下 ShaderEffect 接口：
 * 1. CreateColorShader(...)
 * 2. CreateColorSpaceShader(...)
 * 3. ShaderEffect(...)
 * 4. ShaderEffect(...)
 * 5. CreateBlendShader(...)
 */
bool ShaderEffectFuzzTest002(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint32_t color = GetObject<uint32_t>();
    std::shared_ptr<ShaderEffect> CreateColorShader= ShaderEffect::CreateColorShader(color);
    float redF = GetObject<float>();
    float greenF = GetObject<float>();
    float blueF = GetObject<float>();
    float alphaF = GetObject<float>();

    Color4f color4f {
        redF,
        greenF,
        blueF,
        alphaF,
    };
    uint32_t funcType = GetObject<uint32_t>();
    uint32_t matrixType = GetObject<uint32_t>();
    std::shared_ptr<ColorSpace> colorSpace = ColorSpace::CreateRGB(
        static_cast<CMSTransferFuncType>(funcType % FUNCTYPE_SIZE),
        static_cast<CMSMatrixType>(matrixType % MATRIXTYPE_SIZE));
    ShaderEffect::CreateColorSpaceShader(color4f, colorSpace);

    uint32_t t = GetObject<uint32_t>();
    ShaderEffect shaderEffect = ShaderEffect(static_cast<ShaderEffect::ShaderEffectType>(
        t % SHADEREFFECTTYPE_SIZE), color);
    uint32_t tTwo = GetObject<uint32_t>();
    ShaderEffect shaderEffectTwo = ShaderEffect(static_cast<ShaderEffect::ShaderEffectType>(
        tTwo % SHADEREFFECTTYPE_SIZE), color);
    uint32_t blendMode = GetObject<uint32_t>();
    ShaderEffect::CreateBlendShader(shaderEffect, shaderEffectTwo, static_cast<BlendMode>(blendMode % BLENDMODE_SIZE));
    return true;
}

/*
 * 测试以下 ShaderEffect 接口：
 * 1. CreateImageShader(...)
 * 2. CreatePictureShader(...)
 * 3. CreateLinearGradient(...)
 */
bool ShaderEffectFuzzTest003(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    g_data = data;
    g_size = size;
    g_pos = 0;
    Bitmap bitmap;
    int width = GetObject<int>() % MAX_ARRAY_SIZE;
    int height = GetObject<int>() % MAX_ARRAY_SIZE;
    BitmapFormat bitmapFormat = { COLORTYPE_ARGB_4444, ALPHATYPE_OPAQUE };
    bool ret = bitmap.Build(width, height, bitmapFormat);
    if (!ret) {
        return false;
    }
    Image image;
    bool retTwo = image.BuildFromBitmap(bitmap);
    if (!retTwo) {
        return false;
    }
    uint32_t tileMode = GetObject<uint32_t>();
    uint32_t fm = GetObject<uint32_t>();
    SamplingOptions samplingOptions = SamplingOptions(static_cast<FilterMode>(fm % FILTERMODE_SIZE));
    Matrix matrix;
    matrix.SetMatrix(GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>(),
        GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>());
    ShaderEffect::CreateImageShader(image, static_cast<TileMode>(tileMode % TILEMODE_SIZE),
        static_cast<TileMode>(tileMode % TILEMODE_SIZE), samplingOptions, matrix);
    Picture picture;
    auto dataVal = std::make_shared<Data>();
    size_t length = GetObject<size_t>() % MAX_ARRAY_SIZE;
    char* dataText = new char[length];
    if (dataText == nullptr) {
        return false;
    }
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataVal->BuildWithoutCopy(dataText, length);
    picture.Deserialize(dataVal);
    Rect rect { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    ShaderEffect::CreatePictureShader(picture, static_cast<TileMode>(tileMode % TILEMODE_SIZE),
        static_cast<TileMode>(tileMode % TILEMODE_SIZE), static_cast<FilterMode>(fm % FILTERMODE_SIZE), matrix, rect);
    Point startPt = PointF(GetObject<float>(), GetObject<float>());
    Point endPt = PointF(GetObject<float>(), GetObject<float>());
    std::vector<ColorQuad> colorQuad;
    ColorQuad colorOne = GetObject<ColorQuad>();
    ColorQuad colorTwo = GetObject<ColorQuad>();
    colorQuad.push_back(colorOne);
    colorQuad.push_back(colorTwo);
    std::vector<scalar> scalarNumbers;
    scalar scalarOne = GetObject<scalar>();
    scalar scalarTwo = GetObject<scalar>();
    scalarNumbers.push_back(scalarOne);
    scalarNumbers.push_back(scalarTwo);
    ShaderEffect::CreateLinearGradient(startPt, endPt, colorQuad,
        scalarNumbers, static_cast<TileMode>(tileMode % TILEMODE_SIZE), &matrix);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

/*
 * 测试以下 ShaderEffect 接口：
 * 1. CreateRadialGradient(...)
 * 2. CreateTwoPointConical(...)
 * 3. CreateSweepGradient(...)
 * 4. CreateLightUp(...)
 * 5. CreateExtendShader(...)
 * 6. GetType()
 */
bool ShaderEffectFuzzTest004(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    uint32_t tileMode = GetObject<uint32_t>();
    Matrix matrix;
    scalar scaleX = GetObject<scalar>();
    scalar skewX = GetObject<scalar>();
    scalar transX = GetObject<scalar>();
    scalar skewY = GetObject<scalar>();
    scalar scaleY = GetObject<scalar>();
    scalar transY = GetObject<scalar>();
    scalar persp0 = GetObject<scalar>();
    scalar persp1 = GetObject<scalar>();
    scalar persp2 = GetObject<scalar>();
    matrix.SetMatrix(scaleX, skewX, transX, skewY, scaleY, transY, persp0, persp1, persp2);
    Point startPt = PointF(GetObject<float>(), GetObject<float>());
    Point endPt = PointF(GetObject<float>(), GetObject<float>());
    std::vector<ColorQuad> colorQuad;
    ColorQuad colorOne = GetObject<ColorQuad>();
    ColorQuad colorTwo = GetObject<ColorQuad>();
    colorQuad.push_back(colorOne);
    colorQuad.push_back(colorTwo);
    std::vector<scalar> scalarNumbers;
    scalar scalarOne = GetObject<scalar>();
    scalar scalarTwo = GetObject<scalar>();
    scalarNumbers.push_back(scalarOne);
    scalarNumbers.push_back(scalarTwo);
    ShaderEffect::CreateRadialGradient(startPt, GetObject<scalar>(), colorQuad, scalarNumbers,
        static_cast<TileMode>(tileMode % TILEMODE_SIZE), &matrix);
    ShaderEffect::CreateTwoPointConical(startPt, GetObject<scalar>(), endPt, GetObject<scalar>(), colorQuad,
        scalarNumbers, static_cast<TileMode>(tileMode % TILEMODE_SIZE), &matrix);
    ShaderEffect::CreateSweepGradient(startPt, colorQuad, scalarNumbers,
        static_cast<TileMode>(tileMode % TILEMODE_SIZE), GetObject<scalar>(), GetObject<scalar>(), &matrix);
    uint32_t color = GetObject<uint32_t>();
    uint32_t t = GetObject<uint32_t>();
    float lightUpDeg = GetObject<float>();
    ShaderEffect shaderEffect = ShaderEffect(static_cast<ShaderEffect::ShaderEffectType>(t % SHADEREFFECTTYPE_SIZE),
        color);
    ShaderEffect::CreateLightUp(lightUpDeg, shaderEffect);
    ShaderEffect::CreateExtendShader(nullptr);
    shaderEffect.GetType();
    return true;
}

/*
 * 测试以下 ShaderEffect 接口：
 * 1. ShaderEffect(ShaderEffectType type, uint32_t color)
 * 2. ShaderEffect(ShaderEffectType type, Color4f color4f, std::shared_ptr<ColorSpace> colorSpace)
 * 3. ShaderEffect(ShaderEffectType type, ShaderEffect shaderEffect1, ShaderEffect shaderEffect2,
 *       BlendMode blendMode)
 * 4. ShaderEffect(ShaderEffectType type, const Image& image, TileMode tileModeX, TileMode tileModeY,
 *       const SamplingOptions& samplingOptions, const Matrix& matrix)
 * 5. ShaderEffect(ShaderEffectType type, const Picture& picture, TileMode tileModeX, TileMode tileModeY,
 *       FilterMode filterMode, const Matrix& matrix, const Rect& rect)
 */

bool ShaderEffectFuzzTest005(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    g_data = data;
    g_size = size;
    g_pos = 0;
    uint32_t color = GetObject<uint32_t>();
    uint32_t t = GetObject<uint32_t>();
    Color4f color4f { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    uint32_t funcType = GetObject<uint32_t>();
    uint32_t matrixType = GetObject<uint32_t>();
    ShaderEffect shaderEffect = ShaderEffect(static_cast<ShaderEffect::ShaderEffectType>(
        t % SHADEREFFECTTYPE_SIZE), color);
    std::shared_ptr<ColorSpace> colorSpace = ColorSpace::CreateRGB(
        static_cast<CMSTransferFuncType>(funcType % FUNCTYPE_SIZE),
        static_cast<CMSMatrixType>(matrixType % MATRIXTYPE_SIZE));
    ShaderEffect(static_cast<ShaderEffect::ShaderEffectType>(t % SHADEREFFECTTYPE_SIZE), color4f, colorSpace);
    uint32_t blendMode = GetObject<uint32_t>();
    uint32_t tileMode = GetObject<uint32_t>();
    ShaderEffect(static_cast<ShaderEffect::ShaderEffectType>(t % SHADEREFFECTTYPE_SIZE), shaderEffect,
        shaderEffect, static_cast<BlendMode>(blendMode % BLENDMODE_SIZE));
    Bitmap bitmap;
    int width = GetObject<int>() % MAX_ARRAY_SIZE;
    int height = GetObject<int>() % MAX_ARRAY_SIZE;
    BitmapFormat bitmapFormat = { COLORTYPE_ARGB_4444, ALPHATYPE_OPAQUE };
    bool ret = bitmap.Build(width, height, bitmapFormat);
    if (!ret) {
        return false;
    }
    Image image;
    bool retTwo = image.BuildFromBitmap(bitmap);
    if (!retTwo) {
        return false;
    }
    uint32_t fm = GetObject<uint32_t>();
    Matrix matrix;
    matrix.SetMatrix(GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>(),
        GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>());
    SamplingOptions samplingOptions = SamplingOptions(static_cast<FilterMode>(fm % FILTERMODE_SIZE));
    ShaderEffect(static_cast<ShaderEffect::ShaderEffectType>(t % SHADEREFFECTTYPE_SIZE), image,
        static_cast<TileMode>(tileMode % TILEMODE_SIZE), static_cast<TileMode>(tileMode % TILEMODE_SIZE),
        samplingOptions, matrix);
    Picture picture;
    auto dataVal = std::make_shared<Data>();
    size_t length = GetObject<size_t>() % MAX_ARRAY_SIZE;
    char* dataText = new char[length];
    if (dataText == nullptr) {
        return false;
    }
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataVal->BuildWithoutCopy(dataText, length);
    picture.Deserialize(dataVal);
    Rect rect { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    ShaderEffect(static_cast<ShaderEffect::ShaderEffectType>(t % SHADEREFFECTTYPE_SIZE), picture,
        static_cast<TileMode>(tileMode % TILEMODE_SIZE), static_cast<TileMode>(tileMode % TILEMODE_SIZE),
        static_cast<FilterMode>(fm % FILTERMODE_SIZE), matrix, rect);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::ShaderEffectFuzzTest001(data, size);
    OHOS::Rosen::Drawing::ShaderEffectFuzzTest002(data, size);
    OHOS::Rosen::Drawing::ShaderEffectFuzzTest003(data, size);
    OHOS::Rosen::Drawing::ShaderEffectFuzzTest004(data, size);
    OHOS::Rosen::Drawing::ShaderEffectFuzzTest005(data, size);
    return 0;
}
