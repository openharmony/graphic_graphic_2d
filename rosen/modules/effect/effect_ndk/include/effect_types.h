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
 * @brief Defines vector 2d.
 *
 * @since 24
 * @version 1.0
 */
typedef struct {
    /** The value of the x-axis in a tow-dimensional vector */
    float x;
    /** The value of the y-axis in a tow-dimensional vector */
    float y;
} OH_Vec2;

/**
 * @brief Specify parameters for the water ripple effect filter, including wave shape, lighting conditions, refraction
 *        frequency, and intensity variation at different positions.
 *
 * @since 24
 * @version 1.0
 */
typedef struct {
    /* Wave center position. When = (0.0, 0.0), the point is at the center of the control.
     * When = (0.5, 0.5), the point is at the bottom-right corner of the control.
     * Can extend beyond the control.*/
    OH_Vec2 waveCenter;

    /* Wave motion source position. When = (0.0, 0.0), the point is at the center of the control.
     * When = (0.5, 0.5), the point is at the bottom-right corner of the control.
     * Can extend beyond the control.*/
    OH_Vec2 waveSourceXY;

    /* Degree of waveform distortion in the X and Y directions from the center point.*/
    OH_Vec2 waveDistortXY;

    /* Water wave density. Higher density results in more waves, lower density results in fewer and thicker waves.*/
    OH_Vec2 waveDensityXY;

    /* Water wave strength.*/
    float waveStrength;

    /* Water wave lighting strength.*/
    float waveLightStrength;

    /* Refraction strength. When the background is very blurry, it is recommended to set this parameter to 0.*/
    float waveRefraction;

    /* Water wave specular intensity, reflection strength.*/
    float waveSpecular;

    /* Frequency of water wave light and shadow changes. Higher values create more of a sparkling effect.*/
    float waveFrequency;

    /* Degree of distortion in the water wave shape. Higher values result in more intense and irregular shapes.*/
    float waveShapeDistortion;

    /* Water wave refraction frequency. Higher values result in faster frequency.*/
    float waveNoiseStrength;

    /* Inner edge size of the mask. When 0, there is no mask occlusion; when 1, the entire control effect is occluded,
     * resulting in no effect.*/
    OH_Vec2 waveMaskSize;

    /* Transition degree of the water wave mask edge. Higher values cause the mask to spread more toward the edges.
     * When 0, the mask does not spread and has no transition, stopping at waveMaskSize;
     * When 1, it spreads to the boundary, stopping at [1,1].*/
    float waveMaskRadius;

    /* Corner radius outside the edge. When 0, it is a rectangle; when >0, it is a rounded rectangle.*/
    float borderRadius;

    /* Internal thickness of the edge.*/
    float borderThickness;

    /* Weakening range of the edge. When 0, there is no edge weakening; when 1, the edge weakening range covers the
     * entire control. Refers to the distance from the edge to the center.*/
    float borderScope;

    /* Transition degree of weakening within the edge mask weakening range. Smaller values result in more obvious
     * weakening; larger values result in less obvious weakening.*/
    float borderStrength;

    /* Wave progress, animation progress.*/
    float progress;
} OH_Filter_WaterGlassDataParams;

/**
 * @brief Specify the parameters for the reeded flass filter, including grid count, refraction strength, dispersion
 *        strength, parallel beam intensity, point light source position, and intensity.
 *
 * @since 24
 * @version 1.0
 */
typedef struct {
    /* Higher values result in stronger refraction; 0 means no refraction*/
    float refractionFactor;

    /* Higher values result in stronger dispersion; 0 means no dispersion. Dispersion adjustment has no effect when
     * refractionFactor is 0.*/
    float dispersionStrength;

    /* Higher values result in greater roughness*/
    float roughness;

    /* Higher frequency results in finer grain*/
    float noiseFrequency;

    /* Specifies the number of grids*/
    uint8_t horizontalPatternNumber;

    /* Specifies overall color saturation; 0 results in a black-and-white image*/
    float saturationFactor;

    /* Specifies the light intensity of grid gaps*/
    float borderLightStrength;

    /* Specifies the light width of grid gaps; 0 means no light, 1 means light smoothly fills the grid*/
    float borderLightWidth;

    /* Follows OH_Color; alpha is meaningless*/
    OH_Filter_Color pointLightColor;

    /* Specifies position of point light 1; within [0,1] is inside the UI component, outside is beyond the UI
     * component*/
    OH_Vec2 pointLight1Position;

    /* Specifies intensity of point light 1 */
    float pointLight1Strength;

    /* Specifies position of point light 2; within [0,1] is inside the UI component, outside is beyond the UI
     * component*/
    OH_Vec2 pointLight2Position;

    /* Specifies intensity of point light 2*/
    float pointLight2Strength;
} OH_Filter_ReededGlassDataParams;

#ifdef __cplusplus
}
#endif
/** @} */
#endif