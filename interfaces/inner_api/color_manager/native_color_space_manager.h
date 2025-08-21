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
 * @addtogroup NativeColorSpaceManager
 * @{
 *
 * @brief Provides the native colorSpaceManager capability.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
 * @since 13
 * @version 1.0
 */

/**
 * @file native_color_space_manager.h
 *
 * @brief Defines the functions for obtaining and using a native colorSpaceManager.
 *
 * @library libnative_color_space_manager.so
 * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
 * @since 13
 * @version 1.0
 */
#ifndef C_INCLUDE_NATIVE_COLOR_SPACE_MANAGER_H_
#define C_INCLUDE_NATIVE_COLOR_SPACE_MANAGER_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Defines a colorspace manager.
 * @since 13
 */
typedef struct OH_NativeColorSpaceManager OH_NativeColorSpaceManager;

/**
 * @brief Enumerates color space types.
 * @since 13
 */
typedef enum {
    /** Indicates an unknown color space. */
    NONE = 0,
    /** Indicates the color space based on Adobe RGB. */
    ADOBE_RGB = 1,
    /** Indicates the color space based on SMPTE RP 431-2-2007 and IEC 61966-2.1:1999. */
    DCI_P3 = 2,
    /** Indicates the color space based on SMPTE RP 431-2-2007 and IEC 61966-2.1:1999. */
    DISPLAY_P3 = 3,
    /** Indicates the standard red green blue (SRGB) color space based on IEC 61966-2.1:1999. */
    SRGB = 4,
    /** Indicates the color space based on ITU-R BT.709. */
    BT709 = 6,
    /** Indicates the color space based on ITU-R BT.601. */
    BT601_EBU = 7,
    /** Indicates the color space based on ITU-R BT.601. */
    BT601_SMPTE_C = 8,
    /** Indicates the color space based on ITU-R BT.2020. */
    BT2020_HLG = 9,
    /** Indicates the color space based on ITU-R BT.2020. */
    BT2020_PQ = 10,
    /** PRIMARIES_P3_D65 | TRANSFUNC_HLG | RANGE_FULL */
    P3_HLG = 11,
    /** PRIMARIES_P3_D65 | TRANSFUNC_PQ | RANGE_FULL */
    P3_PQ = 12,
    /** PRIMARIES_ADOBE_RGB | TRANSFUNC_ADOBE_RGB | RANGE_LIMIT */
    ADOBE_RGB_LIMIT = 13,
    /** PRIMARIES_P3_D65 | TRANSFUNC_SRGB | RANGE_LIMIT */
    DISPLAY_P3_LIMIT = 14,
    /** PRIMARIES_SRGB | TRANSFUNC_SRGB | RANGE_LIMIT */
    SRGB_LIMIT = 15,
    /** PRIMARIES_BT709 | TRANSFUNC_BT709 | RANGE_LIMIT */
    BT709_LIMIT = 16,
    /** PRIMARIES_BT601_P | TRANSFUNC_BT709 | RANGE_LIMIT */
    BT601_EBU_LIMIT = 17,
    /** PRIMARIES_BT601_N | TRANSFUNC_BT709 | RANGE_LIMIT */
    BT601_SMPTE_C_LIMIT = 18,
    /** PRIMARIES_BT2020 | TRANSFUNC_HLG | RANGE_LIMIT */
    BT2020_HLG_LIMIT = 19,
    /** PRIMARIES_BT2020 | TRANSFUNC_PQ | RANGE_LIMIT */
    BT2020_PQ_LIMIT = 20,
    /** PRIMARIES_P3_D65 | TRANSFUNC_HLG | RANGE_LIMIT */
    P3_HLG_LIMIT = 21,
    /** PRIMARIES_P3_D65 | TRANSFUNC_PQ | RANGE_LIMIT */
    P3_PQ_LIMIT = 22,
    /** PRIMARIES_P3_D65 | TRANSFUNC_LINEAR */
    LINEAR_P3 = 23,
    /** PRIMARIES_SRGB | TRANSFUNC_LINEAR */
    LINEAR_SRGB = 24,
    /** PRIMARIES_BT709 | TRANSFUNC_LINEAR */
    LINEAR_BT709 = LINEAR_SRGB,
    /** PRIMARIES_BT2020 | TRANSFUNC_LINEAR */
    LINEAR_BT2020 = 25,
    /** PRIMARIES_SRGB | TRANSFUNC_SRGB | RANGE_FULL */
    DISPLAY_SRGB = SRGB,
    /** PRIMARIES_P3_D65 | TRANSFUNC_SRGB | RANGE_FULL */
    DISPLAY_P3_SRGB = DISPLAY_P3,
    /** PRIMARIES_P3_D65 | TRANSFUNC_HLG | RANGE_FULL */
    DISPLAY_P3_HLG = P3_HLG,
    /** PRIMARIES_DISPLAY_P3 | TRANSFUNC_PQ | RANGE_FULL */
    DISPLAY_P3_PQ = P3_PQ,
    /** PRIMARIES_BT2020 | TRANSFUNC_SRGB | RANGE_FULL */
    DISPLAY_BT2020_SRGB = 26,
    /** Indicates a customized color space. */
    CUSTOM = 5,
} ColorSpaceName;

/**
 * @brief Describes the primary colors red, green, blue and white point coordinated as (x, y)
 * in color space, in terms of real world chromaticities.
 * @since 13
 */
typedef struct {
    /** Coordinate value x of red color */
    float rX;
    /** Coordinate value y of red color */
    float rY;
    /** Coordinate value x of green color */
    float gX;
    /** Coordinate value y of green color */
    float gY;
    /** Coordinate value x of blue color */
    float bX;
    /** Coordinate value y of blue color */
    float bY;
    /** Coordinate value x of white point */
    float wX;
    /** Coordinate value y of white point */
    float wY;
} ColorSpacePrimaries;

/**
 * @brief Indicates white point coordinated as (x, y) return array.
 * @since 13
 */
typedef struct {
    /** Indicates white point return array */
    float arr[2];
} WhitePointArray;

/**
 * @brief Creates a <b>NativeColorSpaceManager</b> instance by colorSpaceName.
 * A new <b>NativeColorSpaceManager</b> instance is created each time this function is called.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
 * @param colorSpaceName Indicates the NativeColorSpace connection name.
 * @return Returns the pointer to the <b>NativeColorSpaceManager</b> instance created.
 * Creation failed, cause memory shortage.
 * @since 13
 * @version 1.0
 */
OH_NativeColorSpaceManager* OH_NativeColorSpaceManager_CreateFromName(ColorSpaceName colorSpaceName);

/**
 * @brief Creates a <b>NativeColorSpaceManager</b> instance by primaries and gamma.
 * A new <b>NativeColorSpaceManager</b> instance is created each time this function is called.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
 * @param primaries Indicates the NativeColorSpace connection primaries.
 * @param gamma Indicates the NativeColorSpace connection gamma.
 * @return Returns the pointer to the <b>NativeColorSpaceManager</b> instance created.
 * Creation failed, cause memory shortage.
 * @since 13
 * @version 1.0
 */
OH_NativeColorSpaceManager* OH_NativeColorSpaceManager_CreateFromPrimariesAndGamma(
    ColorSpacePrimaries primaries, float gamma);

/**
 * @brief Delete the NativeColorSpaceManager instance.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
 * @param nativeColorSpaceManager Indicates the pointer to a <b>NativeColorSpaceManager</b> instance.
 * @since 13
 * @version 1.0
 */
void OH_NativeColorSpaceManager_Destroy(OH_NativeColorSpaceManager* nativeColorSpaceManager);

/**
 * @brief Get colorSpace name.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
 * @param nativeColorSpaceManager Indicates the pointer to a <b>NativeColorSpaceManager</b> instance.
 * @return Returns value, return value > 0 && value <= 25, success, return value == 0 , failed.
 * @since 13
 * @version 1.0
 */
int OH_NativeColorSpaceManager_GetColorSpaceName(
    OH_NativeColorSpaceManager* nativeColorSpaceManager);

/**
 * @brief Get white point.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
 * @param nativeColorSpaceManager Indicates the pointer to a <b>NativeColorSpaceManager</b> instance.
 * @return Returns float array, return array = <0.f, 0.f>, failed, otherwise, true.
 * @since 13
 * @version 1.0
 */
WhitePointArray OH_NativeColorSpaceManager_GetWhitePoint(
    OH_NativeColorSpaceManager* nativeColorSpaceManager);

/**
 * @brief Get gamma.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
 * @param nativeColorSpaceManager Indicates the pointer to a <b>NativeColorSpaceManager</b> instance.
 * @return Returns float, return value == 0.f, failed, otherwise, true.
 * @since 13
 * @version 1.0
 */
float OH_NativeColorSpaceManager_GetGamma(OH_NativeColorSpaceManager* nativeColorSpaceManager);

#ifdef __cplusplus
}
#endif
/** @} */
#endif