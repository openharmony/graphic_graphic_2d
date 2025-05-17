/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#include "shadereffect_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "get_object.h"

#include "drawing_color_filter.h"
#include "drawing_image.h"
#include "drawing_image_filter.h"
#include "drawing_matrix.h"
#include "drawing_point.h"
#include "drawing_sampling_options.h"
#include "drawing_shader_effect.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t DATA_MIN_SIZE = 2;
constexpr size_t FILTER_MODE_ENUM_SIZE = 2;
constexpr size_t BLEND_MODE_ENUN_SIZE = 30;
constexpr size_t MIP_MAP_MODE_ENUM_SIZE = 3;
constexpr size_t TILE_MODE_ENUM_SIZE = 4;
constexpr uint32_t MAX_ARRAY_SIZE = 5000;
} // namespace

namespace Drawing {
void NativeShaderEffectTest001(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint32_t color = GetObject<uint32_t>();
    OH_Drawing_ShaderEffect* shaderEffect = OH_Drawing_ShaderEffectCreateColorShader(color);

    float x = GetObject<float>();
    float y = GetObject<float>();
    OH_Drawing_Point* startPt = OH_Drawing_PointCreate(x, y);
    OH_Drawing_Point* endPt = OH_Drawing_PointCreate(GetObject<float>(), GetObject<float>());
    uint32_t format = GetObject<uint32_t>() % MAX_ARRAY_SIZE;
    uint32_t* colors = new uint32_t[format];
    for (size_t i = 0; i < format; i++) {
        colors[i] = GetObject<uint32_t>();
    }
    float* pos = new float[format];
    for (size_t i = 0; i < format; i++) {
        pos[i] = GetObject<float>();
    }
    uint32_t tileMode = GetObject<uint32_t>();
    OH_Drawing_ShaderEffect* shaderEffectTwo = OH_Drawing_ShaderEffectCreateLinearGradient(startPt, endPt, colors, pos,
        format, static_cast<OH_Drawing_TileMode>(tileMode % TILE_MODE_ENUM_SIZE));
    OH_Drawing_ShaderEffect* shaderEffectThree = OH_Drawing_ShaderEffectCreateLinearGradient(nullptr, endPt, colors,
        pos, format, static_cast<OH_Drawing_TileMode>(tileMode % TILE_MODE_ENUM_SIZE));
    if (colors != nullptr) {
        delete[] colors;
        colors = nullptr;
    }
    if (pos != nullptr) {
        delete[] pos;
        pos = nullptr;
    }
    OH_Drawing_PointDestroy(startPt);
    OH_Drawing_PointDestroy(endPt);
    OH_Drawing_ShaderEffectDestroy(shaderEffect);
    OH_Drawing_ShaderEffectDestroy(shaderEffectTwo);
    OH_Drawing_ShaderEffectDestroy(shaderEffectThree);
}

void NativeShaderEffectTest002(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint32_t tileMode = GetObject<uint32_t>();
    uint32_t format = GetObject<uint32_t>() % MAX_ARRAY_SIZE;
    uint32_t* colors = new uint32_t[format];
    for (size_t i = 0; i < format; i++) {
        colors[i] = GetObject<uint32_t>();
    }
    float* pos = new float[format];
    for (size_t i = 0; i < format; i++) {
        pos[i] = GetObject<float>();
    }
    OH_Drawing_Point2D startPoint = {GetObject<float>(), GetObject<float>()};
    OH_Drawing_Point2D endPoint = {GetObject<float>(), GetObject<float>()};
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreate();
    OH_Drawing_ShaderEffect* shaderEffectThree = OH_Drawing_ShaderEffectCreateLinearGradientWithLocalMatrix(&startPoint,
        &endPoint, colors, pos, format, static_cast<OH_Drawing_TileMode>(tileMode % TILE_MODE_ENUM_SIZE), matrix);
    OH_Drawing_ShaderEffect* shaderEffectFour = OH_Drawing_ShaderEffectCreateLinearGradientWithLocalMatrix(nullptr,
        &endPoint, colors, pos, format, static_cast<OH_Drawing_TileMode>(GetObject<uint32_t>() % TILE_MODE_ENUM_SIZE),
        matrix);
    OH_Drawing_MatrixDestroy(matrix);
    OH_Drawing_ShaderEffectDestroy(shaderEffectThree);
    OH_Drawing_ShaderEffectDestroy(shaderEffectFour);

    if (colors != nullptr) {
        delete[] colors;
        colors = nullptr;
    }
    if (pos != nullptr) {
        delete[] pos;
        pos = nullptr;
    }
}

void NativeShaderEffectTest003(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    float x = GetObject<float>();
    float y = GetObject<float>();
    float radius = GetObject<float>();
    uint32_t tileMode = GetObject<uint32_t>();
    uint32_t format = GetObject<uint32_t>() % MAX_ARRAY_SIZE;

    uint32_t* colors = new uint32_t[format];
    for (size_t i = 0; i < format; i++) {
        colors[i] = GetObject<uint32_t>();
    }
    float* pos = new float[format];
    for (size_t i = 0; i < format; i++) {
        pos[i] = GetObject<float>();
    }

    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreate();
    OH_Drawing_Point* centerPt = OH_Drawing_PointCreate(x, y);
    OH_Drawing_ShaderEffect* shaderEffectFour = OH_Drawing_ShaderEffectCreateRadialGradient(centerPt, radius,
        colors, pos, format, static_cast<OH_Drawing_TileMode>(tileMode % TILE_MODE_ENUM_SIZE));
    OH_Drawing_ShaderEffect* shaderEffectFive = OH_Drawing_ShaderEffectCreateRadialGradient(nullptr, radius,
        colors, pos, format, static_cast<OH_Drawing_TileMode>(GetObject<uint32_t>() % TILE_MODE_ENUM_SIZE));
    OH_Drawing_ShaderEffect* shaderEffectSix = OH_Drawing_ShaderEffectCreateSweepGradient(centerPt, colors,
        pos, format, static_cast<OH_Drawing_TileMode>(GetObject<uint32_t>() % TILE_MODE_ENUM_SIZE));
    OH_Drawing_ShaderEffect* shaderEffectSeven = OH_Drawing_ShaderEffectCreateSweepGradient(nullptr, colors,
        pos, format, static_cast<OH_Drawing_TileMode>(GetObject<uint32_t>() % TILE_MODE_ENUM_SIZE));
    if (colors != nullptr) {
        delete[] colors;
        colors = nullptr;
    }
    if (pos != nullptr) {
        delete[] pos;
        pos = nullptr;
    }
    OH_Drawing_PointDestroy(centerPt);
    OH_Drawing_MatrixDestroy(matrix);
    OH_Drawing_ShaderEffectDestroy(shaderEffectFour);
    OH_Drawing_ShaderEffectDestroy(shaderEffectFive);
    OH_Drawing_ShaderEffectDestroy(shaderEffectSix);
    OH_Drawing_ShaderEffectDestroy(shaderEffectSeven);
}

void NativeShaderEffectTest004(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    float radius = GetObject<float>();
    uint32_t tileMode = GetObject<uint32_t>();
    uint32_t format = GetObject<uint32_t>() % MAX_ARRAY_SIZE;
    uint32_t* colors = new uint32_t[format];
    for (size_t i = 0; i < format; i++) {
        colors[i] = GetObject<uint32_t>();
    }
    float* pos = new float[format];
    for (size_t i = 0; i < format; i++) {
        pos[i] = GetObject<float>();
    }
    OH_Drawing_Point2D centerPoint = {GetObject<float>(), GetObject<float>()};
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreate();
    OH_Drawing_ShaderEffect* shaderEffectFive = OH_Drawing_ShaderEffectCreateRadialGradientWithLocalMatrix(&centerPoint,
        radius, colors, pos, format, static_cast<OH_Drawing_TileMode>(tileMode % TILE_MODE_ENUM_SIZE), matrix);
    OH_Drawing_ShaderEffect* shaderEffectSix = OH_Drawing_ShaderEffectCreateRadialGradientWithLocalMatrix(nullptr,
        radius, colors, pos, format, static_cast<OH_Drawing_TileMode>(GetObject<uint32_t>() % TILE_MODE_ENUM_SIZE),
        matrix);
    OH_Drawing_Image* image = OH_Drawing_ImageCreate();
    uint32_t filterMode = GetObject<uint32_t>();
    uint32_t mipmapMode = GetObject<uint32_t>();
    OH_Drawing_SamplingOptions* samplingOptions = OH_Drawing_SamplingOptionsCreate(
        static_cast<OH_Drawing_FilterMode>(filterMode % FILTER_MODE_ENUM_SIZE),
        static_cast<OH_Drawing_MipmapMode>(mipmapMode % MIP_MAP_MODE_ENUM_SIZE));
    OH_Drawing_ShaderEffect* shaderEffectSeven = OH_Drawing_ShaderEffectCreateImageShader(image,
        static_cast<OH_Drawing_TileMode>(GetObject<uint32_t>() % TILE_MODE_ENUM_SIZE),
        static_cast<OH_Drawing_TileMode>(GetObject<uint32_t>() % TILE_MODE_ENUM_SIZE), samplingOptions, matrix);
    OH_Drawing_ShaderEffect* shaderEffectEight = OH_Drawing_ShaderEffectCreateImageShader(nullptr,
        static_cast<OH_Drawing_TileMode>(GetObject<uint32_t>() % TILE_MODE_ENUM_SIZE),
        static_cast<OH_Drawing_TileMode>(GetObject<uint32_t>() % TILE_MODE_ENUM_SIZE), samplingOptions, matrix);
    OH_Drawing_ShaderEffect* shaderEffectNine = OH_Drawing_ShaderEffectCreateImageShader(image,
        static_cast<OH_Drawing_TileMode>(GetObject<uint32_t>() % TILE_MODE_ENUM_SIZE),
        static_cast<OH_Drawing_TileMode>(GetObject<uint32_t>() % TILE_MODE_ENUM_SIZE), samplingOptions, nullptr);
    if (colors != nullptr) {
        delete[] colors;
        colors = nullptr;
    }
    if (pos != nullptr) {
        delete[] pos;
        pos = nullptr;
    }
    OH_Drawing_MatrixDestroy(matrix);
    OH_Drawing_ImageDestroy(image);
    OH_Drawing_SamplingOptionsDestroy(samplingOptions);
    OH_Drawing_ShaderEffectDestroy(shaderEffectFive);
    OH_Drawing_ShaderEffectDestroy(shaderEffectSix);
    OH_Drawing_ShaderEffectDestroy(shaderEffectSeven);
    OH_Drawing_ShaderEffectDestroy(shaderEffectEight);
    OH_Drawing_ShaderEffectDestroy(shaderEffectNine);
}

void NativeShaderEffectTest005(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    uint32_t tileMode = GetObject<uint32_t>();
    float startRadius = GetObject<float>();
    float endRadius = GetObject<float>();
    uint32_t format = GetObject<uint32_t>() % MAX_ARRAY_SIZE;
    OH_Drawing_Point2D startPoint = {GetObject<float>(), GetObject<float>()};
    OH_Drawing_Point2D endPoint = {GetObject<float>(), GetObject<float>()};
    uint32_t* colors = new uint32_t[format];
    for (size_t i = 0; i < format; i++) {
        colors[i] = GetObject<uint32_t>();
    }
    float* pos = new float[format];
    for (size_t i = 0; i < format; i++) {
        pos[i] = GetObject<float>();
    }
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreate();
    OH_Drawing_ShaderEffect* ShaderEffectEight = OH_Drawing_ShaderEffectCreateTwoPointConicalGradient(&startPoint,
        startRadius, &endPoint, endRadius, colors, pos, format,
        static_cast<OH_Drawing_TileMode>(tileMode % TILE_MODE_ENUM_SIZE), matrix);
    OH_Drawing_ShaderEffect* ShaderEffectNine = OH_Drawing_ShaderEffectCreateTwoPointConicalGradient(nullptr,
        startRadius, &endPoint, endRadius, colors, pos, format,
        static_cast<OH_Drawing_TileMode>(tileMode % TILE_MODE_ENUM_SIZE), matrix);

    if (colors != nullptr) {
        delete[] colors;
        colors = nullptr;
    }
    if (pos != nullptr) {
        delete[] pos;
        pos = nullptr;
    }

    OH_Drawing_MatrixDestroy(matrix);
    OH_Drawing_ShaderEffectDestroy(ShaderEffectEight);
    OH_Drawing_ShaderEffectDestroy(ShaderEffectNine);
}

void NativeShaderEffectTest006(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    float x = GetObject<float>();
    float y = GetObject<float>();
    uint32_t tileMode = GetObject<uint32_t>();
    uint32_t format = GetObject<uint32_t>() % MAX_ARRAY_SIZE;

    uint32_t* colors = new uint32_t[format];
    for (size_t i = 0; i < format; i++) {
        colors[i] = GetObject<uint32_t>();
    }
    float* pos = new float[format];
    for (size_t i = 0; i < format; i++) {
        pos[i] = GetObject<float>();
    }

    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreate();
    OH_Drawing_Point* centerPt = OH_Drawing_PointCreate(x, y);
    OH_Drawing_ShaderEffect* shaderEffectOne = OH_Drawing_ShaderEffectCreateSweepGradientWithLocalMatrix(centerPt,
        colors, pos, format, static_cast<OH_Drawing_TileMode>(tileMode % TILE_MODE_ENUM_SIZE), nullptr);
    OH_Drawing_ShaderEffect* shaderEffectTwo = OH_Drawing_ShaderEffectCreateSweepGradientWithLocalMatrix(nullptr,
        colors, pos, format, static_cast<OH_Drawing_TileMode>(tileMode % TILE_MODE_ENUM_SIZE), nullptr);
    OH_Drawing_ShaderEffect* shaderEffectThree = OH_Drawing_ShaderEffectCreateSweepGradientWithLocalMatrix(centerPt,
        colors, pos, format, static_cast<OH_Drawing_TileMode>(tileMode % TILE_MODE_ENUM_SIZE), matrix);
    OH_Drawing_ShaderEffect* shaderEffectFour = OH_Drawing_ShaderEffectCreateSweepGradientWithLocalMatrix(nullptr,
        colors, pos, format, static_cast<OH_Drawing_TileMode>(tileMode % TILE_MODE_ENUM_SIZE), matrix);
    if (colors != nullptr) {
        delete[] colors;
        colors = nullptr;
    }
    if (pos != nullptr) {
        delete[] pos;
        pos = nullptr;
    }
    OH_Drawing_PointDestroy(centerPt);
    OH_Drawing_MatrixDestroy(matrix);
    OH_Drawing_ShaderEffectDestroy(shaderEffectOne);
    OH_Drawing_ShaderEffectDestroy(shaderEffectTwo);
    OH_Drawing_ShaderEffectDestroy(shaderEffectThree);
    OH_Drawing_ShaderEffectDestroy(shaderEffectFour);
}

void NativeShaderEffectTest007(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    uint32_t blendMode = GetObject<uint32_t>();
    uint32_t color = GetObject<uint32_t>();
    uint32_t colorTwo = GetObject<uint32_t>();
    OH_Drawing_ShaderEffect* srcEffect = OH_Drawing_ShaderEffectCreateColorShader(color);
    if (srcEffect == nullptr) {
        return;
    }
    OH_Drawing_ShaderEffect* dstEffect = OH_Drawing_ShaderEffectCreateColorShader(colorTwo);
    if (dstEffect == nullptr) {
        OH_Drawing_ShaderEffectDestroy(srcEffect);
        return;
    }
    OH_Drawing_ShaderEffect* effect = OH_Drawing_ShaderEffectCreateCompose(nullptr, srcEffect,
        static_cast<OH_Drawing_BlendMode>(blendMode % BLEND_MODE_ENUN_SIZE));

    effect = OH_Drawing_ShaderEffectCreateCompose(dstEffect, nullptr,
        static_cast<OH_Drawing_BlendMode>(blendMode % BLEND_MODE_ENUN_SIZE));

    OH_Drawing_BlendMode mode = static_cast<OH_Drawing_BlendMode>(blendMode % BLEND_MODE_ENUN_SIZE);
    effect = OH_Drawing_ShaderEffectCreateCompose(dstEffect, srcEffect, mode);
    if (effect == nullptr) {
        OH_Drawing_ShaderEffectDestroy(srcEffect);
        OH_Drawing_ShaderEffectDestroy(dstEffect);
        return;
    }

    OH_Drawing_ShaderEffectDestroy(srcEffect);
    OH_Drawing_ShaderEffectDestroy(dstEffect);
    OH_Drawing_ShaderEffectDestroy(effect);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::NativeShaderEffectTest001(data, size);
    OHOS::Rosen::Drawing::NativeShaderEffectTest002(data, size);
    OHOS::Rosen::Drawing::NativeShaderEffectTest003(data, size);
    OHOS::Rosen::Drawing::NativeShaderEffectTest004(data, size);
    OHOS::Rosen::Drawing::NativeShaderEffectTest005(data, size);
    return 0;
}
