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

#include "v2/effect_filter.h"

#include "filter/filter.h"

#include "utils/log.h"

namespace {
inline Filter* UnwrapFilter(OH_Graphic2D_EffectKit_Filter* filter)
{
    return reinterpret_cast<Filter*>(filter);
}

inline OH_Graphic2D_EffectKit_Filter* WrapFilter(Filter* filter)
{
    return reinterpret_cast<OH_Graphic2D_EffectKit_Filter*>(filter);
}
}

OH_Graphic2D_EffectKit_ErrorCode OH_Graphic2D_EffectKit_CreateFilter(
    OH_PixelmapNative* pixelmap, OH_Graphic2D_EffectKit_Filter** filter)
{
    if (pixelmap == nullptr || filter == nullptr) {
        return OH_GRAPHIC2D_EFFECTKIT_BAD_PARAMETER;
    }
    auto inner = new Filter(pixelmap->GetInnerPixelmap());
    if (inner == nullptr) {
        return OH_GRAPHIC2D_EFFECTKIT_BAD_PARAMETER;
    }
    *filter = WrapFilter(inner);
    return OH_GRAPHIC2D_EFFECTKIT_SUCCESS;
}

OH_Graphic2D_EffectKit_ErrorCode OH_Graphic2D_EffectKit_DestroyFilter(
    OH_Graphic2D_EffectKit_Filter* filter)
{
    if (filter == nullptr) {
        return OH_GRAPHIC2D_EFFECTKIT_BAD_PARAMETER;
    }
    delete UnwrapFilter(filter);
    return OH_GRAPHIC2D_EFFECTKIT_SUCCESS;
}

OH_Graphic2D_EffectKit_ErrorCode OH_Graphic2D_EffectKit_Blur(
    OH_Graphic2D_EffectKit_Filter* filter, float radiusInPx)
{
    if (filter == nullptr) {
        return OH_GRAPHIC2D_EFFECTKIT_BAD_PARAMETER;
    }
    if (!UnwrapFilter(filter)->Blur(radiusInPx)) {
        return OH_GRAPHIC2D_EFFECTKIT_RENDER_ERROR;
    }
    return OH_GRAPHIC2D_EFFECTKIT_SUCCESS;
}

OH_Graphic2D_EffectKit_ErrorCode OH_Graphic2D_EffectKit_BlurWithTileMode(
    OH_Graphic2D_EffectKit_Filter* filter, float radiusInPx,
    OH_Graphic2D_EffectKit_TileMode tileMode)
{
    if (filter == nullptr) {
        return OH_GRAPHIC2D_EFFECTKIT_BAD_PARAMETER;
    }
    Drawing::TileMode drawingTileMode = static_cast<Drawing::TileMode>(tileMode);
    if (!UnwrapFilter(filter)->Blur(radiusInPx, drawingTileMode)) {
        return OH_GRAPHIC2D_EFFECTKIT_RENDER_ERROR;
    }
    return OH_GRAPHIC2D_EFFECTKIT_SUCCESS;
}

OH_Graphic2D_EffectKit_ErrorCode OH_Graphic2D_EffectKit_Brighten(
    OH_Graphic2D_EffectKit_Filter* filter, float brightness)
{
    if (filter == nullptr) {
        return OH_GRAPHIC2D_EFFECTKIT_BAD_PARAMETER;
    }
    if (!UnwrapFilter(filter)->Brightness(brightness)) {
        return OH_GRAPHIC2D_EFFECTKIT_RENDER_ERROR;
    }
    return OH_GRAPHIC2D_EFFECTKIT_SUCCESS;
}

OH_Graphic2D_EffectKit_ErrorCode OH_Graphic2D_EffectKit_GrayScale(
    OH_Graphic2D_EffectKit_Filter* filter)
{
    if (filter == nullptr) {
        return OH_GRAPHIC2D_EFFECTKIT_BAD_PARAMETER;
    }
    if (!UnwrapFilter(filter)->Grayscale()) {
        return OH_GRAPHIC2D_EFFECTKIT_RENDER_ERROR;
    }
    return OH_GRAPHIC2D_EFFECTKIT_SUCCESS;
}

OH_Graphic2D_EffectKit_ErrorCode OH_Graphic2D_EffectKit_Invert(
    OH_Graphic2D_EffectKit_Filter* filter)
{
    if (filter == nullptr) {
        return OH_GRAPHIC2D_EFFECTKIT_BAD_PARAMETER;
    }
    if (!UnwrapFilter(filter)->Invert()) {
        return OH_GRAPHIC2D_EFFECTKIT_RENDER_ERROR;
    }
    return OH_GRAPHIC2D_EFFECTKIT_SUCCESS;
}

OH_Graphic2D_EffectKit_ErrorCode OH_Graphic2D_EffectKit_SetColorMatrix(
    OH_Graphic2D_EffectKit_Filter* filter,
    const OH_Graphic2D_EffectKit_ColorMatrix* matrix)
{
    if (filter == nullptr || matrix == nullptr) {
        return OH_GRAPHIC2D_EFFECTKIT_BAD_PARAMETER;
    }
    Drawing::ColorMatrix colorMatrix;
    float matrixArr[Drawing::ColorMatrix::MATRIX_SIZE] = { 0 };
    for (size_t i = 0; i < Drawing::ColorMatrix::MATRIX_SIZE; i++) {
        matrixArr[i] = matrix->val[i];
    }
    colorMatrix.SetArray(matrixArr);
    if (!UnwrapFilter(filter)->SetColorMatrix(colorMatrix)) {
        return OH_GRAPHIC2D_EFFECTKIT_RENDER_ERROR;
    }
    return OH_GRAPHIC2D_EFFECTKIT_SUCCESS;
}

OH_Graphic2D_EffectKit_ErrorCode OH_Graphic2D_EffectKit_AcquireEffectPixelMap(
    OH_Graphic2D_EffectKit_Filter* filter, OH_PixelmapNative** pixelmap)
{
    if (filter == nullptr || pixelmap == nullptr) {
        return OH_GRAPHIC2D_EFFECTKIT_BAD_PARAMETER;
    }
    *pixelmap = new OH_PixelmapNative(UnwrapFilter(filter)->GetPixelMap());
    if (*pixelmap == nullptr) {
        return OH_GRAPHIC2D_EFFECTKIT_BAD_PARAMETER;
    }
    return OH_GRAPHIC2D_EFFECTKIT_SUCCESS;
}
