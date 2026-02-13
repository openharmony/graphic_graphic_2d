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

#include <stddef.h>
#include <stdint.h>

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
 * @brief Defines a NativeBuffer.
 *
 * @since 24
 * @version 1.0
 */
typedef struct OH_NativeBuffer OH_NativeBuffer;

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

/**
 * @brief Specify parameters for the water ripple effect filter, including wave shape, lighting conditions, refraction
 *        frequency, and intensity variation at different positions.
 *
 * @since 24
 * @version 1.0
 */
typedef struct {
    /** Wave center position. When = (0.0f, 0.0f), the point is at the center of the control.
     *  When = (0.5f, 0.5f), the point is at the bottom-right corner of the control.
     *  Can extend beyond the control.
     *  Range: No limitation.
     */
    OH_Filter_Vec2 waveCenter;

    /** Wave motion source position. When = (0.0f, 0.0f), the point is at the center of the control.
     *  When = (0.5f, 0.5f), the point is at the bottom-right corner of the control.
     *  Can extend beyond the control.
     *  Range: No limitation.
     */
    OH_Filter_Vec2 waveSourceXY;

    /** Degree of waveform distortion in the X and Y directions from the center point.
     *  Range: [(0.0f, 0.0f), (1.0f, 1.0f)]
     */
    OH_Filter_Vec2 waveDistortXY;

    /** Water wave density. Higher density results in more waves, lower density results in fewer and thicker waves.
     *  Range: [(0.0f, 0.0f), (100.0f, 100.0f)]
     */
    OH_Filter_Vec2 waveDensityXY;

    /** Water wave strength.
     *  Range: [0.0f, 10.0f]
     */
    float waveStrength;

    /** Water wave lighting strength.
     *  Range: [0.0f, 10.0f]
     */
    float waveLightStrength;

    /** Refraction strength. When the background is very blurry, it is recommended to set this parameter to 0.0f.
     *  Range: [0.0f, 10.0f]
     */
    float waveRefraction;

    /** Water wave specular intensity, reflection strength.
     *  Range: [0.0f, 1.0f]
     */
    float waveSpecular;

    /** Frequency of water wave light and shadow changes. Higher values create more of a sparkling effect.
     *  Range: [0.0f, 10.0f]
     */
    float waveFrequency;

    /** Degree of distortion in the water wave shape. Higher values result in more intense and irregular shapes.
     *  Range: [0.0f, 2.0f]
     */
    float waveShapeDistortion;

    /** Water wave refraction frequency. Higher values result in faster frequency.
     *  Range: [0.0f, 1.0f]
     */
    float waveNoiseStrength;

    /** Inner edge size of the mask. When 0.0f, there is no mask occlusion; when 1.0f, the entire control effect is
     *  occluded, resulting in no effect.
     *  Range: [(0.0f, 0.0f), (1.0f, 1.0f)]
     */
    OH_Filter_Vec2 waveMaskSize;

    /** Transition degree of the water wave mask edge. Higher values cause the mask to spread more toward the edges.
     * When 0.0f, the mask does not spread and has no transition, stopping at waveMaskSize;
     * when 1.0f, it spreads to the boundary, stopping at [1.0f,1.0f].
     * Range: [(0.0f,0.0f),(1.0f,1.0f)]
     */
    float waveMaskRadius;

    /** Corner radius outside the edge. When 0.0f, it is a rectangle;
     *  when > 0.0f, it is a rounded rectangle.
     *  Range: [0.0f, 1.0f]
     */
    float borderRadius;

    /** Internal thickness of the edge.
     *  Range: [0.0f, 1.0f]
     */
    float borderThickness;

    /** Weakening range of the edge. When 0.0f, there is no edge weakening;
     *  when 1.0f, the edge weakening range covers the entire control.
     *  Refers to the distance from the edge to the center.
     *  Range: [0.0f, 1.0f]
     */
    float borderScope;

    /** Transition degree of weakening within the edge mask weakening range.
     *  Smaller values result in more obvious weakening; larger values result in less obvious weakening.
     *  Range: [0.0f, 1.0f]
     */
    float borderStrength;

    /** Wave progress, animation progress.
     * Range: [0.0f, +∞)
     */
    float progress;
} OH_Filter_WaterGlassDataParams;

/**
 * @brief Specify the parameters for the reeded glass filter, including grid count, refraction strength, dispersion
 *        strength, parallel beam intensity, point light source position, and intensity.
 *
 * @since 24
 * @version 1.0
 */
typedef struct {
    /** Higher values result in stronger refraction; 0.0f means no refraction.
     *  Range: [0.0f, 1.0f]
     */
    float refractionFactor;

    /** Higher values result in stronger dispersion; 0.0f means no dispersion.
     * Dispersion adjustment has no effect when refractionFactor is 0.0f.
     * Range: [0.0f, 1.0f]
     */
    float dispersionStrength;

    /** Higher values result in greater roughness.
     *  Range: [0.0f, 1.0f]
     */
    float roughness;

    /** Higher frequency results in finer grain.
     *  Range: [0.0f, 1.0f]
     */
    float noiseFrequency;

    /** Specifies the number of grids.
     *  Range: [0.0f, 100.0f]
     */
    uint8_t horizontalPatternNumber;

    /** Specifies overall color saturation; 0.0f results in a black-and-white image.
     *  Range: [0.0f, 2.0f]
     */
    float saturationFactor;

    /** Specifies the light intensity of grid gaps.
     *  Range: [0.0f, 1.0f]
     */
    float borderLightStrength;

    /** Specifies the light width of grid gaps; 0.0f means no light, 1.0f means light smoothly fills the grid.
     *  Range: [0.0f, 1.0f]
     */
    float borderLightWidth;

    /** Follows OH_Filter_Color; alpha is meaningless.
     *  Range: [0.0f, 1.0f]
     */
    OH_Filter_Color pointLightColor;

    /** Specifies position of point light 1.0f; within [0.0f,1.0f] is inside the UI component, outside is beyond the UI
     *  component.
     *  Range: No limitation
     */
    OH_Filter_Vec2 pointLight1Position;

    /** Specifies intensity of point light 1.0f.
     *  Range: [0.0f, 1.0f]
     */
    float pointLight1Strength;

    /** Specifies position of point light 2.0f; within [0.0f,1.0f] is inside the UI component, outside is beyond the UI
     *  component.
     *  Range: No limitation
     */
    OH_Filter_Vec2 pointLight2Position;

    /** Specifies intensity of point light 2.0f.
     *  Range: [0.0f, 1.0f]
     */
    float pointLight2Strength;
} OH_Filter_ReededGlassDataParams;
#ifdef __cplusplus
}
#endif
/** @} */
#endif