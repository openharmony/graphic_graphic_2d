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

#include <new>

#include "filter/filter.h"

#include "utils/log.h"

namespace {
inline Filter* UnwrapFilter(OH_EffectKit_Filter* filter)
{
    return reinterpret_cast<Filter*>(filter);
}

inline OH_EffectKit_Filter* WrapFilter(Filter* filter)
{
    return reinterpret_cast<OH_EffectKit_Filter*>(filter);
}

inline OH_EffectKit_ErrorCode MapDrawingError(DrawingError error)
{
    switch (error) {
        case DrawingError::ERR_OK:
            return OH_EFFECTKIT_SUCCESS;
        case DrawingError::ERR_ILLEGAL_INPUT:
            return OH_EFFECTKIT_BAD_PARAMETER;
        case DrawingError::ERR_MEMORY:
            return OH_EFFECTKIT_UNKNOWN_ERROR;
        default:
            return OH_EFFECTKIT_UNSUPPORTED_OPERATION;
    }
}
}

OH_EffectKit_ErrorCode OH_EffectKit_CreateFilter(
    OH_PixelmapNative* pixelmap, OH_EffectKit_Filter** filter)
{
    if (pixelmap == nullptr || filter == nullptr) {
        return OH_EFFECTKIT_BAD_PARAMETER;
    }
    auto inner = new (std::nothrow) Filter(pixelmap->GetInnerPixelmap());
    if (inner == nullptr) {
        return OH_EFFECTKIT_UNKNOWN_ERROR;
    }
    *filter = WrapFilter(inner);
    return OH_EFFECTKIT_SUCCESS;
}

OH_EffectKit_ErrorCode OH_EffectKit_DestroyFilter(OH_EffectKit_Filter* filter)
{
    if (filter == nullptr) {
        return OH_EFFECTKIT_BAD_PARAMETER;
    }
    delete UnwrapFilter(filter);
    return OH_EFFECTKIT_SUCCESS;
}

OH_EffectKit_ErrorCode OH_EffectKit_Blur(
    OH_EffectKit_Filter* filter, float radiusInPx)
{
    if (filter == nullptr) {
        return OH_EFFECTKIT_BAD_PARAMETER;
    }
    if (radiusInPx < 0.0f) {
        return OH_EFFECTKIT_BAD_PARAMETER;
    }
    auto* inner = UnwrapFilter(filter);
    if (!inner->Blur(radiusInPx)) {
        return MapDrawingError(inner->GetLastError());
    }
    return OH_EFFECTKIT_SUCCESS;
}

OH_EffectKit_ErrorCode OH_EffectKit_BlurWithTileMode(
    OH_EffectKit_Filter* filter, float radiusInPx, OH_EffectKit_TileMode tileMode)
{
    if (filter == nullptr) {
        return OH_EFFECTKIT_BAD_PARAMETER;
    }
    if (radiusInPx < 0.0f) {
        return OH_EFFECTKIT_BAD_PARAMETER;
    }
    Drawing::TileMode drawingTileMode = static_cast<Drawing::TileMode>(tileMode);
    auto* inner = UnwrapFilter(filter);
    if (!inner->Blur(radiusInPx, drawingTileMode)) {
        return MapDrawingError(inner->GetLastError());
    }
    return OH_EFFECTKIT_SUCCESS;
}

OH_EffectKit_ErrorCode OH_EffectKit_Brighten(
    OH_EffectKit_Filter* filter, float brightness)
{
    if (filter == nullptr) {
        return OH_EFFECTKIT_BAD_PARAMETER;
    }
    if (brightness < 0.0f || brightness > 1.0f) {
        return OH_EFFECTKIT_BAD_PARAMETER;
    }
    auto* inner = UnwrapFilter(filter);
    if (!inner->Brightness(brightness)) {
        return MapDrawingError(inner->GetLastError());
    }
    return OH_EFFECTKIT_SUCCESS;
}

OH_EffectKit_ErrorCode OH_EffectKit_GrayScale(OH_EffectKit_Filter* filter)
{
    if (filter == nullptr) {
        return OH_EFFECTKIT_BAD_PARAMETER;
    }
    auto* inner = UnwrapFilter(filter);
    if (!inner->Grayscale()) {
        return MapDrawingError(inner->GetLastError());
    }
    return OH_EFFECTKIT_SUCCESS;
}

OH_EffectKit_ErrorCode OH_EffectKit_Invert(OH_EffectKit_Filter* filter)
{
    if (filter == nullptr) {
        return OH_EFFECTKIT_BAD_PARAMETER;
    }
    auto* inner = UnwrapFilter(filter);
    if (!inner->Invert()) {
        return MapDrawingError(inner->GetLastError());
    }
    return OH_EFFECTKIT_SUCCESS;
}

OH_EffectKit_ErrorCode OH_EffectKit_SetColorMatrix(
    OH_EffectKit_Filter* filter, const OH_EffectKit_ColorMatrix* matrix)
{
    if (filter == nullptr || matrix == nullptr) {
        return OH_EFFECTKIT_BAD_PARAMETER;
    }
    Drawing::ColorMatrix colorMatrix;
    float matrixArr[Drawing::ColorMatrix::MATRIX_SIZE] = { 0 };
    for (size_t i = 0; i < Drawing::ColorMatrix::MATRIX_SIZE; i++) {
        matrixArr[i] = matrix->val[i];
    }
    colorMatrix.SetArray(matrixArr);
    auto* inner = UnwrapFilter(filter);
    if (!inner->SetColorMatrix(colorMatrix)) {
        return MapDrawingError(inner->GetLastError());
    }
    return OH_EFFECTKIT_SUCCESS;
}

OH_EffectKit_ErrorCode OH_EffectKit_AcquireEffectPixelMap(
    OH_EffectKit_Filter* filter, OH_PixelmapNative** pixelmap)
{
    if (filter == nullptr || pixelmap == nullptr) {
        return OH_EFFECTKIT_BAD_PARAMETER;
    }
    auto* inner = UnwrapFilter(filter);
    auto innerPixelmap = inner->GetPixelMap();
    if (innerPixelmap == nullptr) {
        return MapDrawingError(inner->GetLastError());
    }
    *pixelmap = new (std::nothrow) OH_PixelmapNative(innerPixelmap);
    if (*pixelmap == nullptr) {
        return OH_EFFECTKIT_UNKNOWN_ERROR;
    }
    return OH_EFFECTKIT_SUCCESS;
}
