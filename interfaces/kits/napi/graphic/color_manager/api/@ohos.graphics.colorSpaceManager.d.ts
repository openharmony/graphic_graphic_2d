/*
* Copyright (C) 2022 Huawei Device Co., Ltd.
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

import { AsyncCallback } from './basic';

/**
 * colorSpaceManager
 * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
 */
declare namespace colorSpaceManager {
  /**
   * Enumerates color space types.
   * @since 9
   * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
   */
  enum ColorSpace {
    /**
     * Indicates an unknown color space.
     * @since 9
     * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
     */
    UNKNOWN = 0,

    /**
     * Indicates the color space based on Adobe RGB (1998).
     * @since 9
     * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
     */
    ADOBE_RGB_1998 = 1,

    /**
     * Indicates the color space based on SMPTE RP 431-2-2007 and IEC 61966-2.1:1999.
     * @since 9
     * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
     */
    DCI_P3 = 2,

    /**
     * Indicates the color space based on SMPTE RP 431-2-2007 and IEC 61966-2.1:1999.
     * @since 9
     * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
     */
    DISPLAY_P3 = 3,

    /**
     * Indicates the standard red green blue (SRGB) color space based on IEC 61966-2.1:1999.
     * @since 9
     * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
     */
    SRGB = 4,

    /**
     * Indicates a customized color space.
     * @since 9
     * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
     */
    CUSTOM = 5,
  }

  /**
   * Describes the primary colors red, green, blue and white point coordinated as (x, y)
   * in color space, in terms of real world chromaticities.
   * @since 9
   * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
   */
  interface ColorSpacePrimaries {
    /**
     * Coordinate value x of red color
     * @since 9
     * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
     */
    redX: number;

    /**
     * Coordinate value y of red color
     * @since 9
     * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
     */
    redY: number;

    /**
     * Coordinate value x of green color
     * @since 9
     * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
     */
    greenX: number;

    /**
     * Coordinate value y of green color
     * @since 9
     * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
     */
    greenY: number;

    /**
     * Coordinate value x of blue color
     * @since 9
     * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
     */
    blueX: number;

    /**
     * Coordinate value y of blue color
     * @since 9
     * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
     */
    blueY: number;

    /**
     * Coordinate value x of white point
     * @since 9
     * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
     */
    whitePointX: number;

    /**
     * Coordinate value y of white point
     * @since 9
     * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
     */
    whitePointY: number;
  }

  /**
   * Defines a color space object and manages its key information
   * @since 9
   * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
   */
  interface ColorSpaceManager {
    /**
     * Get the name of color space type.
     * @since 9
     * @throws {BusinessError} 18600001 - If param value is abnormal
     */
    getColorSpaceName(): ColorSpace;
  
    /**
     * Get white point(x, y) of color space.
     * @since 9
     * @throws {BusinessError} 18600001 - If param value is abnormal
     */
    getWhitePoint(): Array<number>;
  
    /**
     * Get gamma value of color space.
     * @since 9
     * @throws {BusinessError} 18600001 - If param value is abnormal
     */
    getGamma(): number;
  }

  /**
   * Create a color space manager by provided color space type.
   * @param colorSpaceName Indicates the type of color space
   * @since 9
   * @throws {BusinessError} 401 - If param is invalid
   * @throws {BusinessError} 18600001 - If param value is abnormal
   */
  function create(colorSpaceName: ColorSpace): ColorSpaceManager;

  /**
   * Create a customized color space manager by its color primaries and gamma value
   * @param primaries Indicates the customized color primaries
   * @param gamma Indicates display gamma value
   * @since 9
   * @throws {BusinessError} 401 - If param is invalid
   * @throws {BusinessError} 18600001 - If param value is abnormal
   */
  function create(primaries: ColorSpacePrimaries, gamma: number): ColorSpaceManager;
}

export default colorSpaceManager;