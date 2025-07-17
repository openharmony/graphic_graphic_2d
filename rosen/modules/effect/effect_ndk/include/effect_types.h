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
 * @file effect_types.h
 *
 * @brief Declares the data types for effect filter.
 *
 * @library libnative_effect.so
 * @syscap SystemCapability.Multimedia.Image.Core
 * @since 12
 */

#ifndef C_INCLUDE_EFFECT_TYPES_H
#define C_INCLUDE_EFFECT_TYPES_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Defines a effect filter.
 *
 * @since 12
 * @version 1.0
 */
typedef struct OH_Filter OH_Filter;

/**
 * @brief Defines a pixelmap.
 *
 * @since 12
 * @version 1.0
 */
typedef struct OH_PixelmapNative OH_PixelmapNative;

/**
 * @brief Defines a matrix for create effect filter.
 *
 * @since 12
 * @version 1.0
 */
struct OH_Filter_ColorMatrix {
    /** val mast be 5*4 */
    float val[20];
};

/**
 * @brief Defines a effect filter error code.
 *
 * @since 12
 * @version 1.0
 */
typedef enum {
    /** success */
    EFFECT_SUCCESS = 0,
    /** invalid parameter */
    EFFECT_BAD_PARAMETER = 401,
    /** unsupported operations */
    EFFECT_UNSUPPORTED_OPERATION = 7600201,
    /** unknown error */
    EFFECT_UNKNOWN_ERROR = 7600901,
} EffectErrorCode;

/**
 * @brief Defines a effect filter tile mode.
 *
 * @since 14
 */
typedef enum {
    /** Replicate the edge color if the shader draws outside of its original bounds */
    CLAMP = 0,
    /** Repeat the shader's image horizontally and vertically */
    REPEAT,
    /** Repeat the shader's image horizontally and vertically,
     *  alternating mirror images so that adjacent images always seam
     */
    MIRROR,
    /** Only draw within the original domain, return transparent-black everywhere else */
    DECAL,
} EffectTileMode;

#ifdef __cplusplus
}
#endif
/** @} */
#endif