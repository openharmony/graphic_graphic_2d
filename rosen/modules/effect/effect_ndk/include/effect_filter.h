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

/**
 * @addtogroup effectKit
 * @{
 *
 * @brief Provides APIs for obtaining effect filter and information.
 *
 * @syscap SystemCapability.Multimedia.Image.Core
 * @since 12
 */

/**
 * @file effect_filter.h
 *
 * @brief Declares the APIs that can access a effect filter.
 *
 * @library libnative_effect.so
 * @syscap SystemCapability.Multimedia.Image.Core
 * @since 12
 */

#ifndef C_INCLUDE_EFFECT_FILTER_H
#define C_INCLUDE_EFFECT_FILTER_H

#include "effect_types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Creates an <b>OH_Filter</b> object.
 *
 * @syscap SystemCapability.Multimedia.Image.Core
 * @param pixelmap The pixelmap pointer to create filter.
 * @param filter The OH_Filter pointer will be operated.
 * @return Returns {@link EffectErrorCode}.
 * @since 12
 * @version 1.0
 */
EffectErrorCode OH_Filter_CreateEffect(OH_PixelmapNative* pixelmap, OH_Filter** filter);

/**
 * @brief Release an <b>OH_Filter</b> object.
 *
 * @syscap SystemCapability.Multimedia.Image.Core
 * @param filter The OH_Filter pointer will be operated.
 * @return Returns {@link EffectErrorCode}
 * @since 12
 * @version 1.0
 */
EffectErrorCode OH_Filter_Release(OH_Filter* filter);

/**
 * @brief Creates a blur effect and then add to the filter.
 *
 * @syscap SystemCapability.Multimedia.Image.Core
 * @param filter The OH_Filter pointer will be operated.
 * @param radius The radius of the blur effect.
 * @return Returns {@link EffectErrorCode}.
 * @since 12
 * @version 1.0
 */
EffectErrorCode OH_Filter_Blur(OH_Filter* filter, float radius);

/**
 * @brief Creates a blur effect and then add to the filter.
 *
 * @syscap SystemCapability.Multimedia.Image.Core
 * @param filter The OH_Filter pointer will be operated.
 * @param radius The radius of the blur effect.
 * @param tileMode The tileMode of the blur effect.
 * @return BlurWithTileMode result code.
 *        {@link EFFECT_SUCCESS} if the operation is successful.
 *        {@link EFFECT_BAD_PARAMETER} if parameter is invalid.
 * @since 14
 */
EffectErrorCode OH_Filter_BlurWithTileMode(OH_Filter* filter, float radius, EffectTileMode tileMode);

/**
 * @brief Creates a brighten effect and then add to the filter.
 *
 * @syscap SystemCapability.Multimedia.Image.Core
 * @param filter The OH_Filter pointer will be operated.
 * @param brightness The brightness of the brighten effect.
 * @return Returns {@link EffectErrorCode}.
 * @since 12
 * @version 1.0
 */
EffectErrorCode OH_Filter_Brighten(OH_Filter* filter, float brightness);

/**
 * @brief Creates a gray scale effect and then add to the filter.
 *
 * @syscap SystemCapability.Multimedia.Image.Core
 * @param filter The OH_Filter pointer will be operated.
 * @return Returns {@link EffectErrorCode}.
 * @since 12
 * @version 1.0
 */
EffectErrorCode OH_Filter_GrayScale(OH_Filter* filter);

/**
 * @brief Creates a invert effect and then add to the filter.
 *
 * @syscap SystemCapability.Multimedia.Image.Core
 * @param filter The OH_Filter pointer will be operated.
 * @return Returns {@link EffectErrorCode}.
 * @since 12
 * @version 1.0
 */
EffectErrorCode OH_Filter_Invert(OH_Filter* filter);

/**
 * @brief Creates a effect with a matrix and then add to the filter.
 *
 * @syscap SystemCapability.Multimedia.Image.Core
 * @param filter The OH_Filter pointer will be operated.
 * @param matrix The {@link OH_Filter_ColorMatrix} pointer to create a custom effect.
 * @return Returns {@link EffectErrorCode}.
 * @since 12
 * @version 1.0
 */
EffectErrorCode OH_Filter_SetColorMatrix(OH_Filter* filter, OH_Filter_ColorMatrix* matrix);

/**
 * @brief Get a pixelmap with the filter effect.
 *
 * @syscap SystemCapability.Multimedia.Image.Core
 * @param filter The OH_Filter pointer will be operated.
 * @param pixelmap The pixelmap pointer wiil be operated.
 * @return Returns {@link EffectErrorCode}.
 * @since 12
 * @version 1.0
 */
EffectErrorCode OH_Filter_GetEffectPixelMap(OH_Filter* filter, OH_PixelmapNative** pixelmap);

/**
 * @brief Get a pixelmap with the filter effect.
 *
 * @syscap SystemCapability.Multimedia.Image.Core
 * @param filter The OH_Filter pointer will be operated.
 * @param pixelmap The pixelmap pointer will be operated which requires the user to release after using it.
 * @param useCpuRender Whether to use cpu or gpu to render, if true, use cpu, else use gpu.
 * @return Returns {@link EffectErrorCode}.
 * @since 20
 * @version 1.0
 */
EffectErrorCode OH_Filter_GetEffectPixelMapExt(OH_Filter* filter, OH_PixelmapNative** pixelmap, bool useCpuRender);

#ifdef __cplusplus
}
#endif
/** @} */
#endif
