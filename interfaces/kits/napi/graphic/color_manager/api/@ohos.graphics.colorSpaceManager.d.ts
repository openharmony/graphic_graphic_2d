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
   * Describes the primary colors red, green, blue and white point
   * in each color space map(x, y), in terms of real world chromaticities.
   * @since 9
   * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
   */
  interface ColorSpacePrimaries {
    /**
     * x value of red color
     * @since 9
     * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
     */
    redX: number;

    /**
     * y value of red color
     * @since 9
     * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
     */
    redY: number;

    /**
     * x value of green color
     * @since 9
     * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
     */
    greenX: number;

    /**
     * y value of green color
     * @since 9
     * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
     */
    greenY: number;

    /**
     * x value of blue color
     * @since 9
     * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
     */
    blueX: number;

    /**
     * y value of blue color
     * @since 9
     * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
     */
    blueY: number;

    /**
     * x value of white point
     * @since 9
     * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
     */
    whitePointX: number;

    /**
     * y value of white point
     * @since 9
     * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
     */
    whitePointY: number;
  }

  /**
   * color space manager, created by color space infomation
   * @since 9
   * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
   */
  interface ColorSpaceManager {
    /**
     * Get color space type.
     * @since 9
     */
    getColorSpaceName(): ColorSpace;
  
    /**
     * Get white point(x, y) of color space.
     * @since 9
     */
    getWhitePoint(): Array<number>;
  
    /**
     * Get gamma value of color space.
     * @since 9
     */
    getGamma(): number;
  }

  /**
   * Create a color space manager by proviced color space type.
   * @param colorSpaceName Indicates the type of color space
   * @since 9
   */
  function create(colorSpaceName: ColorSpace): ColorSpaceManager;

  /**
   * Create a customized color space manager by its color primaries and gamma value
   * @param primaries Indicates the customized color primaries
   * @param gamma Indicates display gamma value
   * @since 9
   */
  function create(primaries: ColorSpacePrimaries, gamma: number): ColorSpaceManager;
 }

 export default colorSpaceManager;