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
 * @brief Creates a blur effect width direction angle and then add to the filter.
 *
 * @param filter The OH_Filter pointer will be operated.
 * @param radius The radius of the blur effect.
 * @param effectDirection The direction angle of blur effect: range 0 to 180,
 *        0 and 180 for horizontal direction, 90 for vertical direction,
 *        clamp to boundary if out of range: < 0 set to 0, > 180 set to 180.
 * @param tileMode The tileMode of the blur effect.
 * @return BlurWithDirection result code.
 *        {@link EFFECT_SUCCESS} if the operation is successful.
 *        {@link EFFECT_BAD_PARAMETER} if parameter is invalid.
 * @since 24
 * @version 1.0
 */
EffectErrorCode OH_Filter_BlurWithDirection(OH_Filter* filter, float radius, float effectDirection,
    EffectTileMode tileMode);

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
 * @brief Creates an effect blend the original image and transition image based on different types of masks.
 *        different mask types correspond to different structures,
 *        and generate different types of transition effects.
 *
 * @param filter The OH_Filter pointer will be operated.
 * @param transitionImage The {@link OH_PixelmapNative} pointer,
 *        which will be blended with the input image from {@link OH_Filter_CreateEffect}.
 * @param mask The structure pointer for mask parameter of the transition effect.
 *        Different types {@link EffectMaskType} of masks can be selected,
 *        and each type of mask defines a unique mask structure,
 *        for example, {@link EffectMaskType}:LINEAR_GRADIENT_MASK
 *        corresponds to {@link OH_Filter_LinearGradientMask}.
 * @param maskType The type {@link EffectMaskType} of the transition mask.
 * @param factor The factor of transition mask,
 *        the transparency of the mask will be affected by factor, which in turn affects blend.
 *        The value range is [0.0f, 1.0f], truncated beyond the range.
 * @param inverse if inverse of the transition effect, will present a completely opposite image blend effect.
 * @return Returns {@link EffectErrorCode}.
 * @since 24
 * @version 1.0
 */
EffectErrorCode OH_Filter_MaskTransition(OH_Filter* filter,
    OH_PixelmapNative* transitionImage, void* mask, EffectMaskType maskType, float factor, bool inverse);

/**
 * @brief Creates an effect blend the original image and transition image
 *        based on a customized deformable water droplet mask,
 *        and adds the water droplet filter based on the mask.
 *
 * @param filter The OH_Filter pointer will be operated.
 * @param transitionImage The {@link OH_PixelmapNative} pointer,
 *        which will be blended with the input image from {@link OH_Filter_CreateEffect}.
 * @param waterDropletParams The {@link OH_Filter_WaterDropletParams} pointer,
 *        which is used to affect the effect of water droplet.
 * @param inverse if inverse of the transition effect, will present a completely opposite image blend effect.
 * @return Returns {@link EffectErrorCode}.
 * @since 24
 * @version 1.0
 */
EffectErrorCode OH_Filter_WaterDropletTransition(OH_Filter* filter,
    OH_PixelmapNative* transitionImage, OH_Filter_WaterDropletParams* waterDropletParams, bool inverse);

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
 * @brief Add a flowing lens effect to EffectKit and provide corresponding NDK interfaces for
 *        application-side calls. Implement a filter effect similar to water ripples, supporting
 *        adjustable parameters such as flow velocity and thickness.
 *
 * @param filter The OH_Filter pointer will be operated.
 * @param waterGlassParams The {@link OH_Filter_WaterGlassDataParams} pointer to
 *        create a custom effect.
 * @return Returns {@link EffectErrorCode}.
 * @since 24
 * @version 1.0
 */
EffectErrorCode OH_Filter_WaterGlass(OH_Filter* filter, OH_Filter_WaterGlassDataParams* waterGlassParams);

/**
 * @brief Add a reeded glass filter effect to EffectKit, along with corresponding NDK interfaces
 *        for application-side calls. Implement a reeded glass refraction effect, supporting
 *        adjustable parameters such as grid count, refraction strength, dispersion strength,
 *        parallel beam intensity, point light source position, and intensity.
 *
 * @param filter The OH_Filter pointer will be operated.
 * @param reededGlassParams The {@link OH_Filter_ReededGlassDataParams} pointer
 *        to create a custom effect.
 * @return Returns {@link EffectErrorCode}.
 * @since 24
 * @version 1.0
 */
EffectErrorCode OH_Filter_ReededGlass(OH_Filter* filter, OH_Filter_ReededGlassDataParams* reededGlassParams);

/**
 * @brief Creates a effect with map color by brightness, and then add to the filter.
 *
 * @param filter The OH_Filter pointer will be operated.
 * @param params The {@link OH_Filter_MapColorByBrightnessParams} pointer, includes colors,
 *        the corresponding brightness positions to map, and color number.
 * @return Returns {@link EffectErrorCode}.
 * @since 24
 * @version 1.0
 */
EffectErrorCode OH_Filter_MapColorByBrightness(OH_Filter* filter, OH_Filter_MapColorByBrightnessParams* params);

/**
 * @brief Creates a effect with gamma correction, and then add to the filter.
 *
 * @param filter The OH_Filter pointer will be operated.
 * @param gamma Gamma correction coefficient: 1 means original image, < 1 makes image darker,
 *        > 1 makes it brighter; must be > 0, otherwise no effect.
 * @return Returns {@link EffectErrorCode}.
 * @since 24
 * @version 1.0
 */
EffectErrorCode OH_Filter_GammaCorrection(OH_Filter* filter, float gamma);

 /**
 * @brief Obtains the OH_NativeBuffer used to create a filter.
 * This function could transform the pixelmap with effect to a shared memory directly.
 * The user is suggested to align the size and format with the input pixelmap when creating the effect.
 *
 * @param filter The OH_Filter pointer will be operated.
 * @param nativeBuffer The OH_NativeBuffer pointer will be operated.
 * @return Returns {@link EffectErrorCode}.
 * @since 24
 * @version 1.0
 */
EffectErrorCode OH_Filter_GetEffectNativeBuffer(OH_Filter* filter, OH_NativeBuffer* dstNativeBuffer);

#ifdef __cplusplus
}
#endif
/** @} */
#endif
