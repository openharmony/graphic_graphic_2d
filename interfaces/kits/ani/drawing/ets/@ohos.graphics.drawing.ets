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
 * @file
 * @kit ArkGraphics2D
 */

import type common2D from '@ohos.graphics.common2D';
import image from '@ohos.multimedia.image';

/**
 * Provides functions such as 2D graphics rendering, text drawing, and image display.
 *
 * @namespace drawing
 * @syscap SystemCapability.Graphics.Drawing
 * @since 11
 */
namespace drawing {
  /**
   * Enumerate blending modes for colors.
   * Blend is a operation that use 4 components(red, green, blue, alpha) to generate
   * a new color from two colors(source, destination).
   * @enum { number }
   * @syscap SystemCapability.Graphics.Drawing
   * @since 11
   */
  export enum BlendMode {
    /**
     * Disable 4 regions(red, green, blue, alpha)
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    CLEAR = 0,
    /**
     * Use components of the source
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    SRC = 1,
    /**
     * Use components of the destination
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    DST = 2,
    /**
     * The source is placed above the destination.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    SRC_OVER = 3,
    /**
     * The Destination is placed above the source.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    DST_OVER = 4,
    /**
     * Use source replaces the destination, and will not exceed the boundaries of the destination
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    SRC_IN = 5,
    /**
     * Use destination, and will not exceed the boundaries of the source
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    DST_IN = 6,
    /**
     * Source is use in outside of the boundaries of the destination.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    SRC_OUT = 7,
    /**
     * Destination is use in outside of the boundaries of the source.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    DST_OUT = 8,
    /**
     * Source which overlaps the destination will replaces the destination.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    SRC_ATOP = 9,
    /**
     * Destination which overlaps the source will replaces the source.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    DST_ATOP = 10,
    /**
     * Combine regions where source and destination do not overlap.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    XOR = 11,
    /**
     * The sum of the source and destination.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    PLUS = 12,
    /**
     * All components are multiplied.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    MODULATE = 13,
    /**
     * Multiply the complement values of the background and source color values,
     * and then complement the result.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    SCREEN = 14,
    /**
     * Multiplies or screens the colors, depending on destination
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    OVERLAY = 15,
    /**
     * Choose a darker background and source color.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    DARKEN = 16,
    /**
     * Choose a lighter background and source color.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    LIGHTEN = 17,
    /**
     * Brightens destination color to reflect the source color.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    COLOR_DODGE = 18,
    /**
     * Darkens destination color to reflect the source color.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    COLOR_BURN = 19,
    /**
     * Multiplies or screens the colors, depending on source
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    HARD_LIGHT = 20,
    /**
     * Lightens or Darkens the colors, depending on the source.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    SOFT_LIGHT = 21,
    /**
     * Subtract the darker of the two colors from the brighter color.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    DIFFERENCE = 22,
    /**
     * Produces an effect similar to difference mode, but with lower contrast.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    EXCLUSION = 23,
    /**
     * Multiply the source color by the destination color and replace the destination.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    MULTIPLY = 24,
    /**
     * Use the hue of the source and the saturation and brightness of the destination.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    HUE = 25,
    /**
     * Use the saturation of the source and the hue and brightness of the destination.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    SATURATION = 26,
    /**
     * Use the hue and saturation of the source and the brightness of the destination.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    COLOR = 27,
    /**
     * Use the brightness of the source and the hue and saturation of the destination.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    LUMINOSITY = 28,
  }

  /**
   * Provides settings for strokes during drawing.
   * @syscap SystemCapability.Graphics.Drawing
   * @since 11
   */
  export class Pen {
    static { loadLibrary("drawing_ani"); }
    private nativeObj: long = 0;

    /**
     * Constructor for the pen.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native constructor();

    /**
     * Constructor for the pen from an existing pen object pen.
     * @param { Pen } pen - Indicates the Pen object.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native constructor(pen: Pen);

    /**
    * Replaces alpha, leaving RGB
    *
    * @param { number } alpha - Alpha channel of color. The range of alpha must be [0, 255].
    * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
    * <br>2. Incorrect parameter types; 3. Parameter verification failed.
    * @syscap SystemCapability.Graphics.Drawing
    * @since 11
    */
    native setAlpha(alpha: number): void;

    /**
     * Obtains the alpha of a pen. The alpha is used by the pen to outline a shape.
     * @returns { number } Returns a 8-bit variable that describes the alpha.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native getAlpha(): number;

    /**
    * Sets a blender that implements the specified blendmode enum.
    *
    * @param { BlendMode } mode - Blendmode.
    * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
    * <br>2. Incorrect parameter types; 3. Parameter verification failed.
    * @syscap SystemCapability.Graphics.Drawing
    * @since 11
    */
    native setBlendMode(mode: BlendMode): void;

    /**
     * Resets all pen contents to their initial values.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native reset(): void;
  }

  /**
   * Provides settings for brush fill when drawing.
   * @syscap SystemCapability.Graphics.Drawing
   * @since 11
   */
  export class Brush {
    static { loadLibrary("drawing_ani"); }
    private nativeObj: long = 0;

    /**
     * Constructor for the Brush.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native constructor();

    /**
     * Constructor for the Brush from an existing brush object brush.
     * @param { Brush } brush - Indicates the Brush object.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native constructor(brush: Brush);

    /**
     * Replaces alpha, leaving RGB
     * @param { number } alpha - Alpha channel of color. The range of alpha must be [0, 255].
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types; 3. Parameter verification failed.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    native setAlpha(alpha: number): void;

    /**
     * Obtains the alpha of a brush. The alpha is used by the brush to fill in a shape.
     * @returns { number } Returns a 8-bit variable that describes the alpha.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native getAlpha(): number;

    /**
     * Sets a blender that implements the specified blendmode enum.
     * @param { BlendMode } mode - Blendmode.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types; 3. Parameter verification failed.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    native setBlendMode(mode: BlendMode): void;

    /**
     * Resets all brush contents to their initial values.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native reset(): void;
  }
}

export default drawing;
