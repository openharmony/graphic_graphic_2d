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
 * @brief Defines 4-channel Color.
 *
 * @since 24
 * @version 1.0
 */
typedef struct {
    /** red channel */
    float red;
    /** green channel */
    float green;
    /** blue channel */
    float blue;
    /** alpha channel */
    float alpha;
} OH_Filter_Color;

/**
 * @brief Defines MapColorByBrightness params for create effect filter.
 *
 * @since 24
 * @version 1.0
 */
typedef struct {
    /** The numbers of colors and positions must be the same,
     *  the red, green, blur and alpha of OH_Filter_Color must be > 0.
     */
    OH_Filter_Color* colors;
    /** Specify the position where the color map brightness is set,
     *  the value rangle 0.0f to 1.0f, < 0.0f set to 0.0f, > 1.0f set to 1.0f.
     */
    float* positions;
    /** The number of available colors and position, must to be > 0, otherwise no effect,
     *  take first 5 value of color and position if number > 5.
     */
    uint8_t colorsNum;
} OH_Filter_MapColorByBrightnessParams;

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

/**
 * @brief Defines a effect filter mask type.
 *
 * @since 24
 */
typedef enum {
    /** mask type linear gradient */
    LINEAR_GRADIENT_MASK = 0,
    /** mask type radial gradient */
    RADIAL_GRADIENT_MASK,
    /** mask type undefined */
    MAX
} EffectMaskType;

/**
 * @brief Defines Vector 2d.
 *
 * @since 24
 * @version 1.0
 */
typedef struct {
    /** The value of the x-axis in a two-dimensional vector */
    float x;
    /** The value of the y-axis in a two-dimensional vector */
    float y;
} OH_Filter_Vec2;

/**
 * @brief Create a single segment linear gradient or multi segment linear gradient mask effect Mask
 *        instance by inputting parameters such as the starting point, ending point,
 *        and transition point and color of the linear gradient.
 *        The specific effect is determined by the input parameters.
 *
 * @since 24
 * @version 1.0
 */
typedef struct {
    /** The start position of the linear gradient mask, The value range is 0-1, truncated beyond the range */
    OH_Filter_Vec2 startPosition;
    /** The end position of the linear gradient mask, The value range is 0-1, truncated beyond the range */
    OH_Filter_Vec2 endPosition;
    /** A two-dimensional array, where each member is a two-dimensional vector,
     * in each vector (x, y), x represents color with a value range of [0,1],
     * y represents position with a value range of [0,1]; Two adjacent array members form a gradient region;
     * The position attribute y in array members must be incremented.
     */
    OH_Filter_Vec2* fractionStops;
    /** The number of array members of fractionStops,
     * The valid value range of this parameter is [2, 12].
     * If the value is out of the range, the {@link EffectErrorCode} error will be returned during the calling.
     */
    uint32_t fractionStopsLength;
} OH_Filter_LinearGradientMask;

/**
 * @brief Create an elliptical mask effect Mask instance by inputting the position,
 *        major and minor axes, and shape parameters of the ellipse center point.
 *        The specific effect is determined by the input parameters.
 *
 * @since 24
 * @version 1.0
 */
typedef struct {
    /** The center position of the radial gradient mask,
     * [0,0] is the upper left corner of the component, and [1,1] is the lower right corner of the component.
     * The value range is [-10.0f, 10.0f], truncated beyond the range.
     */
    OH_Filter_Vec2 center;
    /** The major axis of the ellipse corresponds to the widthof the component at 1.0.
     * The value range is [0.0f, 10.0f], truncated beyond the range.
     */
    float radiusX;
    /** The major axis of the ellipse corresponds to the height of the component at 1.0.
     * The value range is [0.0f, 10.0f], truncated beyond the range.
     */
    float radiusY;
    /** A two-dimensional array, where each member is a two-dimensional vector,
     * in each vector (x, y), x represents color with a value range of [0.0f, 1.0f], truncated beyond the range;
     * y represents position with a value range of [0.0f, 1.0f], truncated beyond the range;
     * Two adjacent array members form a gradient region;
     * The position attribute y in array members must be incremented.
     */
    OH_Filter_Vec2* fractionStops;
    /** The number of array members of fractionStops,
     * The valid value range of this parameter is [2, 12].
     * If the value is out of the range, the {@link EffectErrorCode} error will be returned during the calling.
     */
    uint32_t fractionStopsLength;
} OH_Filter_RadialGradientMask;

/**
 * @brief Specify parameters related to water droplet filters,
 *        including the shape of water droplets, lighting effects,
 *        distortion effects on the original image, etc.
 *
 * @since 24
 * @version 1.0
 */
typedef struct {
    /** radius of water droplet, The value range is [0.0f, 10.0f], truncated beyond the range.*/
    float radius;
    /** water droplet transition fade width, The value range is [0.0f, 1.0f], truncated beyond the range. */
    float transitionFadeWidth;
    /** water droplet distortion intensity, The value range is [0.0f, 1.0f], truncated beyond the range. */
    float distortionIntensity;
    /** water droplet distortion thickness, The value range is [0.0f, 1.0f], truncated beyond the range. */
    float distortionThickness;
    /** water droplet light strength, The value range is [0.0f, 1.0f], truncated beyond the range. */
    float lightStrength;
    /** water droplet light softness, The value range is [0.0f, 1.0f], truncated beyond the range. */
    float lightSoftness;
    /** water droplet x-axis noise (deformation) frequency,
     * The value range is [0.0f, 10.0f], truncated beyond the range.
     */
    float noiseScaleX;
    /** water droplet y-axis noise (deformation) frequency,
     * The value range is [0.0f, 10.0f], truncated beyond the range.
     */
    float noiseScaleY;
    /** water droplet x-axis noise (deformation) strength,
     * The value range is [0.0f, 10.0f], truncated beyond the range.
     */
    float noiseStrengthX;
    /** water droplet y-axis noise (deformation) strength,
     * The value range is [0.0f, 10.0f], truncated beyond the range.
     */
    float noiseStrengthY;
    /** Animation progress, controlling the deformation state of the droplet shape. truncated beyond the range.
     * The value range is >= 0, if < 0.0f, cut to 0.0f.
     */
    float progress;
} OH_Filter_WaterDropletParams;

#ifdef __cplusplus
}
#endif
/** @} */
#endif