/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
 * @addtogroup InnerApiDemo
 * @{
 *
 * @brief A brief module description.
 *
 * The module Detailed description.
 */

/**
 * @file inner_api_demo.h
 *
 * @brief A brief file description.
 *
 * The file Detailed description.
 */
#ifndef INNER_API_DEMO_H
#define INNER_API_DEMO_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Defines a colorspace manager.
 */
typedef struct OH_InnerApiDemoMgr OH_InnerApiDemoMgr;

/**
 * @brief Enumerates color space types.
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
    /** Indicates a customized color space. */
    CUSTOM = 5,
} Enum1;

/**
 * @brief Describes the primary colors red, green, blue and white point coordinated as (x, y)
 * in color space, in terms of real world chromaticities.
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
} SpacePrimaries;

/**
 * @brief Indicates white point coordinated as (x, y) return array.
 */
typedef struct {
    /** Indicates white point return array */
    float arr[2];
} WhitePointArray;

/**
 * @brief Creates a <b>InnerApiDemoMgr</b> instance by Enum1.
 * A new <b>InnerApiDemoMgr</b> instance is created each time this function is called.
 *
 * @param Enum1 Indicates the NativeColorSpace connection name.
 * @return Returns the pointer to the <b>InnerApiDemoMgr</b> instance created.
 * Creation failed, cause memory shortage.
 */
OH_InnerApiDemoMgr* CreateFromName(Enum1 enum);

/**
 * @brief Creates a <b>InnerApiDemoMgr</b> instance by primaries and gamma.
 * A new <b>InnerApiDemoMgr</b> instance is created each time this function is called.
 *
 * @param primaries Indicates the NativeColorSpace connection primaries.
 * @param gamma Indicates the NativeColorSpace connection gamma.
 * @return Returns the pointer to the <b>InnerApiDemoMgr</b> instance created.
 * Creation failed, cause memory shortage.
 */
OH_InnerApiDemoMgr* CreateFromPrimariesAndGamma(SpacePrimaries primaries, float gamma);

/**
 * @brief Delete the InnerApiDemoMgr instance.
 *
 * @param mgr Indicates the pointer to a <b>InnerApiDemoMgr</b> instance.
 */
void Destroy(OH_InnerApiDemoMgr* mgr);

/**
 * @brief Get colorSpace name.
 *
 * @param mgr Indicates the pointer to a <b>InnerApiDemoMgr</b> instance.
 * @return Returns value, return value > 0 && value <= 25, success, return value == 0 , failed.
 */
int GetEnum1(OH_InnerApiDemoMgr* mgr);

/**
 * @brief Get white point.
 *
 * @param mgr Indicates the pointer to a <b>InnerApiDemoMgr</b> instance.
 * @return Returns float array, return array = <0.f, 0.f>, failed, otherwise, true.
 */
WhitePointArray GetWhitePoint(OH_InnerApiDemoMgr* mgr);

/**
 * @brief Get gamma.
 *
 * @param mgr Indicates the pointer to a <b>InnerApiDemoMgr</b> instance.
 * @return Returns float, return value == 0.f, failed, otherwise, true.
 */
float GetGamma(OH_InnerApiDemoMgr* mgr);

#ifdef __cplusplus
}
#endif
/** @} */
#endif