/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
 * @file
 * @kit ArkGraphics2D
 */
/**
 * The date structure that provides the basis for graphics.
 *
 * @namespace common2D
 * @syscap SystemCapability.Graphics.Drawing
 * @since 11
 */
namespace common2D {
  /**
   * Provide a description in the form of color ARGB.
   * @typedef Color
   * @syscap SystemCapability.Graphics.Drawing
   * @since 11
   */
  export interface Color {
    /**
     * Alpha component of color, from 0 to 255.
     * @type { int }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    alpha: int;
    /**
     * Red component of color, from 0 to 255.
     * @type { int }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    red: int;
    /**
     * Green component of color, from 0 to 255.
     * @type { int }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    green: int;
    /**
     * Blue component of color, from 0 to 255.
     * @type { int }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    blue: int;
  }

  /**
   * Provides the definition of the rectangle.
   * @typedef Rect
   * @syscap SystemCapability.Graphics.Drawing
   * @since 11
   */
  export interface Rect {
    /**
     * Left Position of Rectangle.
     * @type { double }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    left: double;
    /**
     * Top side position of the rectangle
     * @type { double }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    top: double;
    /**
     * Right Position of Rectangle.
     * @type { double }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    right: double;
    /**
     * Position of the bottom side of the rectangle.
     * @type { double }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    bottom: double;
  }

  /**
   * Coordinates in the font layout.
   * @typedef Point
   * @syscap SystemCapability.Graphics.Drawing
   * @since 12
   */
  export interface Point {
    /**
     * X-axis coordinate.
     * @type { double }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    x: double;

    /**
     * Y-axis coordinate.
     * @type { double }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    y: double;
  }

  /**
   * Provides the definition of the point in 3D.
   *
   * @typedef Point3d
   * @extends Point
   * @syscap SystemCapability.Graphics.Drawing
   * @since 12
   */
  export interface Point3d extends Point {
    /**
     * Z-axis coordinate.
     * @type { double }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    z: double;
  }
  
}

export default common2D;