/*
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
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

#include "drawing_shader_effect.h"

#include "drawing_canvas_utils.h"
#include "drawing_helper.h"
#include "native_pixel_map_manager.h"

#include "effect/shader_effect.h"
#include "effect/shader_effect_lazy.h"
#include "render/rs_pixel_map_util.h"
#include "render/rs_pixel_map_shader_obj.h"

using namespace OHOS;
using namespace Rosen;
using namespace Drawing;

static const Point* CastToPoint(const OH_Drawing_Point* cPoint)
{
    return reinterpret_cast<const Point*>(cPoint);
}

static const Point* CastToPoint(const OH_Drawing_Point2D* cPoint)
{
    return reinterpret_cast<const Point*>(cPoint);
}

static const Image& CastToImage(const OH_Drawing_Image& cImage)
{
    return reinterpret_cast<const Image&>(cImage);
}

static const SamplingOptions& CastToSamplingOptions(const OH_Drawing_SamplingOptions& cSamplingOptions)
{
    return reinterpret_cast<const SamplingOptions&>(cSamplingOptions);
}

static const Matrix* CastToMatrix(const OH_Drawing_Matrix* cMatrix)
{
    return reinterpret_cast<const Matrix*>(cMatrix);
}

static OH_Drawing_ShaderEffect* CastShaderEffect(std::shared_ptr<ShaderEffect> shaderEffect)
{
    NativeHandle<ShaderEffect>* shaderEffectHandle = new NativeHandle<ShaderEffect>;
    shaderEffectHandle->value = shaderEffect;
    return Helper::CastTo<NativeHandle<ShaderEffect>*, OH_Drawing_ShaderEffect*>(shaderEffectHandle);
}

OH_Drawing_ShaderEffect* OH_Drawing_ShaderEffectCreateColorShader(const uint32_t color)
{
    return CastShaderEffect(ShaderEffect::CreateColorShader(color));
}

OH_Drawing_ShaderEffect* OH_Drawing_ShaderEffectCreateLinearGradient(const OH_Drawing_Point* cStartPt,
    const OH_Drawing_Point* cEndPt, const uint32_t* colors, const float* pos, uint32_t size,
    OH_Drawing_TileMode cTileMode)
{
    if (cStartPt == nullptr || cEndPt == nullptr || colors == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return nullptr;
    }
    if (cTileMode < CLAMP || cTileMode > DECAL) {
        g_drawingErrorCode = OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE;
        return nullptr;
    }
    std::vector<ColorQuad> colorsVector;
    std::vector<scalar> posVector;
    for (uint32_t i = 0; i < size; i++) {
        colorsVector.emplace_back(colors[i]);
    }
    if (pos != nullptr) {
        for (uint32_t i = 0; i < size; i++) {
            posVector.emplace_back(pos[i]);
        }
    }
    return CastShaderEffect(ShaderEffect::CreateLinearGradient(
        *CastToPoint(cStartPt), *CastToPoint(cEndPt), colorsVector, posVector, static_cast<TileMode>(cTileMode)));
}

OH_Drawing_ShaderEffect* OH_Drawing_ShaderEffectCreateLinearGradientWithLocalMatrix(
    const OH_Drawing_Point2D* startPt, const OH_Drawing_Point2D* endPt, const uint32_t* colors, const float* pos,
    uint32_t size, OH_Drawing_TileMode cTileMode, const OH_Drawing_Matrix* cMatrix)
{
    if (startPt == nullptr || endPt == nullptr || colors == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return nullptr;
    }
    if (cTileMode < CLAMP || cTileMode > DECAL) {
        g_drawingErrorCode = OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE;
        return nullptr;
    }
    std::vector<ColorQuad> colorsVector;
    std::vector<scalar> posVector;
    for (uint32_t i = 0; i < size; i++) {
        colorsVector.emplace_back(colors[i]);
    }
    if (pos != nullptr) {
        for (uint32_t i = 0; i < size; i++) {
            posVector.emplace_back(pos[i]);
        }
    }
    return CastShaderEffect(ShaderEffect::CreateLinearGradient(
        *CastToPoint(startPt), *CastToPoint(endPt), colorsVector,
        posVector, static_cast<TileMode>(cTileMode), cMatrix ? CastToMatrix(cMatrix) : nullptr));
}

OH_Drawing_ShaderEffect* OH_Drawing_ShaderEffectCreateRadialGradient(const OH_Drawing_Point* cCenterPt, float radius,
    const uint32_t* colors, const float* pos, uint32_t size, OH_Drawing_TileMode cTileMode)
{
    if (cCenterPt == nullptr || colors == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return nullptr;
    }
    if (cTileMode < CLAMP || cTileMode > DECAL) {
        g_drawingErrorCode = OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE;
        return nullptr;
    }
    std::vector<ColorQuad> colorsVector;
    std::vector<scalar> posVector;
    for (uint32_t i = 0; i < size; i++) {
        colorsVector.emplace_back(colors[i]);
        if (pos) {
            posVector.emplace_back(pos[i]);
        }
    }
    return CastShaderEffect(ShaderEffect::CreateRadialGradient(
        *CastToPoint(cCenterPt), radius, colorsVector, posVector, static_cast<TileMode>(cTileMode)));
}

OH_Drawing_ShaderEffect* OH_Drawing_ShaderEffectCreateRadialGradientWithLocalMatrix(
    const OH_Drawing_Point2D* centerPt, float radius, const uint32_t* colors, const float* pos, uint32_t size,
    OH_Drawing_TileMode cTileMode, const OH_Drawing_Matrix* cMatrix)
{
    if (centerPt == nullptr || colors == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return nullptr;
    }
    if (cTileMode < CLAMP || cTileMode > DECAL) {
        g_drawingErrorCode = OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE;
        return nullptr;
    }
    std::vector<ColorQuad> colorsVector;
    std::vector<scalar> posVector;
    for (uint32_t i = 0; i < size; i++) {
        colorsVector.emplace_back(colors[i]);
        if (pos) {
            posVector.emplace_back(pos[i]);
        }
    }
    return CastShaderEffect(ShaderEffect::CreateRadialGradient(
        *CastToPoint(centerPt), radius, colorsVector, posVector,
        static_cast<TileMode>(cTileMode), cMatrix ? CastToMatrix(cMatrix) : nullptr));
}

OH_Drawing_ShaderEffect* OH_Drawing_ShaderEffectCreateSweepGradientWithLocalMatrix(
    const OH_Drawing_Point* centerPt, const uint32_t* colors, const float* pos, uint32_t size,
    OH_Drawing_TileMode tileMode, const OH_Drawing_Matrix* matrix)
{
    if (centerPt == nullptr || colors == nullptr) {
        return nullptr;
    }
    if (tileMode < CLAMP || tileMode > DECAL) {
        return nullptr;
    }
    std::vector<ColorQuad> colorsVector;
    std::vector<scalar> posVector;
    for (uint32_t i = 0; i < size; i++) {
        colorsVector.emplace_back(colors[i]);
        if (pos) {
            posVector.emplace_back(pos[i]);
        }
    }
    return CastShaderEffect(ShaderEffect::CreateSweepGradient(
        *CastToPoint(centerPt), colorsVector, posVector, static_cast<TileMode>(tileMode), 0,
        360, matrix ? CastToMatrix(matrix) : nullptr)); // 360: endAngle
}

OH_Drawing_ShaderEffect* OH_Drawing_ShaderEffectCreateSweepGradient(const OH_Drawing_Point* cCenterPt,
    const uint32_t* colors, const float* pos, uint32_t size, OH_Drawing_TileMode cTileMode)
{
    if (cCenterPt == nullptr || colors == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return nullptr;
    }
    if (cTileMode < CLAMP || cTileMode > DECAL) {
        g_drawingErrorCode = OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE;
        return nullptr;
    }
    std::vector<ColorQuad> colorsVector;
    std::vector<scalar> posVector;
    for (uint32_t i = 0; i < size; i++) {
        colorsVector.emplace_back(colors[i]);
        if (pos) {
            posVector.emplace_back(pos[i]);
        }
    }
    return CastShaderEffect(ShaderEffect::CreateSweepGradient(
        *CastToPoint(cCenterPt), colorsVector, posVector, static_cast<TileMode>(cTileMode), 0,
        360, nullptr)); // 360: endAngle
}

OH_Drawing_ShaderEffect* OH_Drawing_ShaderEffectCreateImageShader(OH_Drawing_Image* cImage, OH_Drawing_TileMode tileX,
    OH_Drawing_TileMode tileY, const OH_Drawing_SamplingOptions* cSampling, const OH_Drawing_Matrix* cMatrix)
{
    if (cImage == nullptr || cSampling == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return nullptr;
    }
    if (tileX < CLAMP || tileX > DECAL || tileY < CLAMP || tileY > DECAL) {
        g_drawingErrorCode = OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE;
        return nullptr;
    }
    if (cMatrix == nullptr) {
        Matrix matrix;
        return CastShaderEffect(ShaderEffect::CreateImageShader(
            CastToImage(*cImage), static_cast<TileMode>(tileX), static_cast<TileMode>(tileY),
            CastToSamplingOptions(*cSampling), matrix));
    }
    return CastShaderEffect(ShaderEffect::CreateImageShader(
        CastToImage(*cImage), static_cast<TileMode>(tileX), static_cast<TileMode>(tileY),
        CastToSamplingOptions(*cSampling), *CastToMatrix(cMatrix)));
}

OH_Drawing_ShaderEffect* OH_Drawing_ShaderEffectCreatePixelMapShader(OH_Drawing_PixelMap* pixelMap,
    OH_Drawing_TileMode tileX, OH_Drawing_TileMode tileY, const OH_Drawing_SamplingOptions* samplingOptions,
    const OH_Drawing_Matrix* matrix)
{
#ifdef OHOS_PLATFORM
    if (pixelMap == nullptr || samplingOptions == nullptr) {
        return nullptr;
    }
    if (tileX < CLAMP || tileX > DECAL || tileY < CLAMP || tileY > DECAL) {
        return nullptr;
    }

    std::shared_ptr<Media::PixelMap> pixelMapPtr = OHOS::Rosen::GetPixelMapFromNativePixelMap(pixelMap);
    if (!pixelMapPtr) {
        return nullptr;
    }

    Matrix defaultMatrix;
    const Matrix& matrixRef = matrix ? *CastToMatrix(matrix) : defaultMatrix;

    return CastShaderEffect(RSPixelMapShaderObj::CreatePixelMapShader(
        pixelMapPtr, static_cast<TileMode>(tileX), static_cast<TileMode>(tileY),
        CastToSamplingOptions(*samplingOptions), matrixRef));
#else
    return nullptr;
#endif
}

OH_Drawing_ShaderEffect* OH_Drawing_ShaderEffectCreateTwoPointConicalGradient(const OH_Drawing_Point2D* startPt,
    float startRadius, const OH_Drawing_Point2D* endPt, float endRadius, const uint32_t* colors, const float* pos,
    uint32_t size, OH_Drawing_TileMode cTileMode, const OH_Drawing_Matrix* cMatrix)
{
    if (startPt == nullptr || endPt == nullptr || colors == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return nullptr;
    }
    if (cTileMode < CLAMP || cTileMode > DECAL) {
        g_drawingErrorCode = OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE;
        return nullptr;
    }
    std::vector<ColorQuad> colorsVector;
    std::vector<scalar> posVector;
    for (uint32_t i = 0; i < size; i++) {
        colorsVector.emplace_back(colors[i]);
        if (pos) {
            posVector.emplace_back(pos[i]);
        }
    }
    return CastShaderEffect(ShaderEffect::CreateTwoPointConical(
        *CastToPoint(startPt), startRadius, *CastToPoint(endPt), endRadius, colorsVector, posVector,
        static_cast<TileMode>(cTileMode), cMatrix ? CastToMatrix(cMatrix) : nullptr));
}

OH_Drawing_ShaderEffect* OH_Drawing_ShaderEffectCreateCompose(OH_Drawing_ShaderEffect* dst,
    OH_Drawing_ShaderEffect* src, OH_Drawing_BlendMode mode)
{
    if (dst == nullptr || src == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return nullptr;
    }
    if (mode < BLEND_MODE_CLEAR || mode > BLEND_MODE_LUMINOSITY) {
        g_drawingErrorCode = OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE;
        return nullptr;
    }
    auto dstHandle = Helper::CastTo<OH_Drawing_ShaderEffect*, NativeHandle<ShaderEffect>*>(dst);
    auto srcHandle = Helper::CastTo<OH_Drawing_ShaderEffect*, NativeHandle<ShaderEffect>*>(src);
    if (dstHandle->value.get() == nullptr || srcHandle->value.get() == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return nullptr;
    }
    return CastShaderEffect(ShaderEffectLazy::CreateBlendShader(dstHandle->value,
        srcHandle->value, static_cast<BlendMode>(mode)));
}

void OH_Drawing_ShaderEffectDestroy(OH_Drawing_ShaderEffect* cShaderEffect)
{
    if (!cShaderEffect) {
        return;
    }
    delete Helper::CastTo<OH_Drawing_ShaderEffect*, NativeHandle<ShaderEffect>*>(cShaderEffect);
}