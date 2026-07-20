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

#ifndef C_INCLUDE_EFFECT_FILTER_V2_H
#define C_INCLUDE_EFFECT_FILTER_V2_H

#include "v2/effect_types.h"
#ifdef __cplusplus
extern "C" {
#endif

OH_Graphic2D_EffectKit_ErrorCode OH_Graphic2D_EffectKit_CreateFilter(
    OH_PixelmapNative* pixelmap, OH_Graphic2D_EffectKit_Filter** filter);

OH_Graphic2D_EffectKit_ErrorCode OH_Graphic2D_EffectKit_DestroyFilter(
    OH_Graphic2D_EffectKit_Filter* filter);

OH_Graphic2D_EffectKit_ErrorCode OH_Graphic2D_EffectKit_Blur(
    OH_Graphic2D_EffectKit_Filter* filter, float radiusInPx);

OH_Graphic2D_EffectKit_ErrorCode OH_Graphic2D_EffectKit_BlurWithTileMode(
    OH_Graphic2D_EffectKit_Filter* filter, float radiusInPx,
    OH_Graphic2D_EffectKit_TileMode tileMode);

OH_Graphic2D_EffectKit_ErrorCode OH_Graphic2D_EffectKit_Brighten(
    OH_Graphic2D_EffectKit_Filter* filter, float brightness);

OH_Graphic2D_EffectKit_ErrorCode OH_Graphic2D_EffectKit_GrayScale(
    OH_Graphic2D_EffectKit_Filter* filter);

OH_Graphic2D_EffectKit_ErrorCode OH_Graphic2D_EffectKit_Invert(
    OH_Graphic2D_EffectKit_Filter* filter);

OH_Graphic2D_EffectKit_ErrorCode OH_Graphic2D_EffectKit_SetColorMatrix(
    OH_Graphic2D_EffectKit_Filter* filter,
    const OH_Graphic2D_EffectKit_ColorMatrix* matrix);

OH_Graphic2D_EffectKit_ErrorCode OH_Graphic2D_EffectKit_AcquireEffectPixelMap(
    OH_Graphic2D_EffectKit_Filter* filter, OH_PixelmapNative** pixelmap);

#ifdef __cplusplus
}
#endif

#endif
