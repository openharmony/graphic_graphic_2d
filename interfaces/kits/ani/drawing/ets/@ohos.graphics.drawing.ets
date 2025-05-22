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
  loadLibrary("drawing_ani");
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
   * Enumerates of operations when two paths are combined.
   * @enum { number }
   * @syscap SystemCapability.Graphics.Drawing
   * @since 12
   */
  export enum PathOp {
    /**
     * Difference operation.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    DIFFERENCE = 0,

    /**
     * Intersect operation.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    INTERSECT = 1,

    /**
     * Union operation.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    UNION = 2,

    /**
     * Xor operation.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    XOR = 3,

    /**
     * Reverse difference operation.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    REVERSE_DIFFERENCE = 4,
  }

  /**
   * Enumerates storage filter mode.
   * @enum { number }
   * @syscap SystemCapability.Graphics.Drawing
   * @since 12
   */
  export enum FilterMode {
    /**
     * Single sample point (nearest neighbor).
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    FILTER_MODE_NEAREST = 0,

    /**
     * Interpolate between 2x2 sample points (bilinear interpolation).
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    FILTER_MODE_LINEAR = 1,
  }

  /**
   * Provides an interface to the drawing, and samplingOptions used when sampling from the image.
   * @syscap SystemCapability.Graphics.Drawing
   * @since 12
   */
  export class SamplingOptions {
    static { loadLibrary("drawing_ani"); }
    private nativeObj : long = 0;
    /**
     * Constructor for the samplingOptions.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native constructor();
    /**
     * Constructor for the samplingOptions with filter mode.
     * @param { FilterMode } filterMode - Storage filter mode.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native constructor(filterMode: FilterMode);
  }

  export class Canvas {
    static { loadLibrary("drawing_ani"); }
    private nativeObj : long = 0;
    /**
     * Constructor for the Canvas.
     * @param { image.PixelMap } pixelmap - PixelMap.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    native constructor(pixelmap: image.PixelMap);

    /**
     * If rectangle is stroked, use pen to stroke width describes the line thickness,
     * else use brush to fill the rectangle.
     * @param { common2D.Rect } rect - Rectangle to draw.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    native drawRect(rect: common2D.Rect): void;

    /**
     * If rectangle is stroked, use pen to stroke width describes the line thickness,
     * else use brush to fill the rectangle.
     * @param { number } left - Indicates the left position of the rectangle.
     * @param { number } top - Indicates the top position of the rectangle.
     * @param { number } right - Indicates the right position of the rectangle.
     * @param { number } bottom - Indicates the bottom position of the rectangle.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native drawRect(left: number, top: number, right: number, bottom: number): void;

    /**
     * Draws the specified source image onto the canvas,
     * scaled and translated to the destination rectangle.
     * @param { image.PixelMap } pixelmap - The source image.
     * @param { common2D.Rect } dstRect - The area of destination canvas.
     * @param { SamplingOptions } samplingOptions - SamplingOptions used to describe the sampling mode.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native drawImageRect(pixelmap: image.PixelMap, dstRect: common2D.Rect, samplingOptions?: SamplingOptions): void;

    /**
     * Draws the pixelmap base on the mesh which is evenly distributed across the pixelmap.
     * @param { image.PixelMap } pixelmap - The pixelmap to draw using the mesh.
     * @param { number } meshWidth - The number of columns in the mesh.
     * @param { number } meshHeight - The number of rows in the mesh.
     * @param { Array<number> } vertices - Array of vertices, specifying where the mesh should be drawn.
     * @param { number } vertOffset - Number of vert elements to skip before drawing.
     * @param { Array<number> } colors - Array of colors, specifying a color at each vertex.
     * @param { number } colorOffset - Number of color elements to skip before drawing.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native drawPixelMapMesh(pixelmap: image.PixelMap, meshWidth: number, meshHeight: number,
      vertices: Array<number>, vertOffset: number, colors: Array<number>, colorOffset: number): void;

    /**
     * Set pen to a canvas.
     * @param { Pen } pen - object.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    native attachPen(pen: Pen): void;

    /**
     * Set brush to a canvas.
     * @param { Brush } brush - Object.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    native attachBrush(brush: Brush): void;

    /**
     * Unset pen to a canvas.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    native detachPen(): void;

    /**
     * Unset brush to a canvas.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    native detachBrush(): void;

    /**
     * Saves the current canvas status (canvas matrix) to the top of the stack.
     * @returns { number } Return the number of saved states.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native save(): number;

    /**
     * Saves matrix and clip, and allocates a bitmap for subsequent drawing.
     * Calling restore discards changes to matrix and clip, and draws the bitmap.
     * @param { common2D.Rect | null} rect - Optional layer size. The default value is null.
     * @param { Brush | null} brush - Optional brush effect used to draw the layer. The default value is null.
     * @returns { number } Return the number of saved states before this call.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: Mandatory parameters are left unspecified.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native saveLayer(rect?: common2D.Rect | null, brush?: Brush | null): number;

    /**
     * Restores the canvas status (canvas matrix) saved on the top of the stack.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native restore(): void;

    /**
     * Gets the number of the canvas status (canvas matrix) saved in the stack.
     * @returns { number } Return represent depth of save state stack.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native getSaveCount(): number;

    /**
     * Rotates by degrees, positive degrees rotates clockwise.
     * @param { number } degrees - Indicates the amount to rotate, in degrees.
     * @param { number } sx - Indicates the x-axis value of the point to rotate about.
     * @param { number } sy - Indicates the y-axis value of the point to rotate about.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native rotate(degrees: number, sx: number, sy: number) : void;
  }

  /**
   * Enumerates clip operations.
   *
   * @enum { number }
   * @syscap SystemCapability.Graphics.Drawing
   * @since 12
   */
  export enum ClipOp {
    /**
     * Clips with difference.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    DIFFERENCE = 0,
    /**
     * Clips with intersection.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    INTERSECT = 1,
  }

  /**
   * ColorFilters are optional objects in the drawing pipeline.
   *
   * @syscap SystemCapability.Graphics.Drawing
   * @since 11
   */
  export class ColorFilter {
    static { loadLibrary("drawing_ani"); }
    private nativeObj : long = 0;
    /**
     * Makes a color filter with the give n ARGB color of hexadecimal format and blend mode.
     * @param { common2D.Color | number } color - Number must be ARGB color of hexadecimal format.
     * @param { BlendMode } mode - BlendMode.
     * @returns { ColorFilter } Colorfilter object.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types; 3. Parameter verification failed.
     * @static
     * @syscap SystemCapability.Graphics.Drawing
     * @since 18
     */
    static native createBlendModeColorFilter(color: common2D.Color | number, mode: BlendMode): ColorFilter;
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
    * Sets ColorFilter to pen
    *
    * @param { ColorFilter } filter - ColorFilter to apply to subsequent draw.
    * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
    * <br>2. Incorrect parameter types.
    * @syscap SystemCapability.Graphics.Drawing
    * @since 11
    */
    native setColorFilter(filter: ColorFilter): void;

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
     * Sets ColorFilter to brush
     * @param { ColorFilter } filter - ColorFilter to apply to subsequent draw.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    native setColorFilter(filter: ColorFilter): void;

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

  /**
   * The Typeface class specifies the typeface and intrinsic style of a font.
   *
   * @syscap SystemCapability.Graphics.Drawing
   * @since 11
   */
  export class Typeface {
    static { loadLibrary("drawing_ani"); }
    private nativeObj: long = 0;

    /**
     * Get the family name for this typeface.
     * @returns { string } Family name.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    native getFamilyName(): string;

    /**
     * Generate typeface from file.
     * @param { string } filePath - file path for typeface.
     * @returns { Typeface } Typeface.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
     static native makeFromFile(filePath: string): Typeface;
  }

  /**
   * Font controls options applied when drawing and measuring text.
   *
   * @syscap SystemCapability.Graphics.Drawing
   * @since 11
   */
  export class Font {
    static { loadLibrary("drawing_ani"); }
    private nativeObj: long = 0;

    /**
     * Constructor for the Font.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    native constructor();

    /**
     * Sets text size in points. Has no effect if textSize is not greater than or equal to zero.
     * @param { number } textSize - Typographic height of text. The height of the text must be greater than 0.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types; 3. Parameter verification failed.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    native setSize(textSize: number): void;

    /**
     * Obtains the text size.
     * @returns { number } Text size.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    native getSize(): number;

    /**
     * Sets Typeface to font.
     * @param { Typeface } typeface - Font and style used to draw text.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    native setTypeface(typeface: Typeface): void;

    /**
     * Get Typeface to font.
     * @returns { Typeface } Typeface.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    native getTypeface(): Typeface;

    /**
     * Get fontMetrics associated with typeface.
     * @returns { FontMetrics } The fontMetrics value returned to the caller.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    native getMetrics(): FontMetrics;
  }

  /**
   * The metrics of an Font.
   * @typedef FontMetrics
   * @syscap SystemCapability.Graphics.Drawing
   * @since 11
   */
  export interface FontMetrics {
    /**
     * Maximum range above the glyph bounding box.
     * @type { number }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    top: number;

    /**
     * Distance Retained Above Baseline.
     * @type { number }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    ascent: number;

    /**
     * The distance that remains below the baseline.
     * @type { number }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    descent: number;

    /**
     * Maximum range below the glyph bounding box.
     * @type { number }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    bottom: number;

    /**
     * Line Spacing.
     * @type { number }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    leading: number;
  }

  /**
   * The metrics of an Font.
   * @typedef FontMetrics
   * @syscap SystemCapability.Graphics.Drawing
   * @since 11
   */
  export class FontMetricsInner implements FontMetrics {
    /**
     * Maximum range above the glyph bounding box.
     * @type { number }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    top: number = 0;

    /**
     * Distance Retained Above Baseline.
     * @type { number }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    ascent: number = 0;

    /**
     * The distance that remains below the baseline.
     * @type { number }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    descent: number = 0;

    /**
     * Maximum range below the glyph bounding box.
     * @type { number }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    bottom: number = 0;

    /**
     * Line Spacing.
     * @type { number }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    leading: number = 0;
  }

  /**
   * Lattice is the class for dividing an image into grids.
   * @syscap SystemCapability.Graphics.Drawing
   * @since 12
   */
  export class Lattice {
    static { loadLibrary("drawing_ani"); }
    private nativeObj: long = 0;
    /**
     * Divide an image into a rectangular grid. Grid entries on even columns and even rows are fixed;
     * these entries are always drawn at their original size if the destination is large enough. If the destination
     * side is too small to hold the fixed entries, all fixed entries are scaled down to fit.
     * The grid entries not on even columns and rows are scaled to fit the remaining space, if any.
     * @param { Array<number> } xDivs - X coordinate of values used to divide the image.
     * @param { Array<number> } yDivs - Y coordinate of values used to divide the image.
     * @param { number } fXCount - Number of x coordinates. Must be >= 0.
     * @param { number } fYCount - Number of y coordinates. Must be >= 0.
     * @param { common2D.Rect | null } fBounds - Source bounds to draw from. The default value is null.
     * @param { Array<RectType> | null } fRectTypes - Array of fill types. The default value is null.
     * @param { Array<common2D.Color> | null } fColors - Array of colors. The default value is null.
     * @returns { Lattice } Lattice object.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types; 3. Parameter verification failed.
     * @static
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    static native createImageLattice(xDivs: Array<number>, yDivs: Array<number>, fXCount: number, fYCount: number,
      fBounds?: common2D.Rect | null, fRectTypes?: Array<RectType> | null, fColors?: Array<common2D.Color> | Array<number> | null): Lattice;
  }

  /**
   * Enumerate rect types. Optional setting per rectangular grid entry to make it transparent,
   * or to fill the grid entry with a color. only used in Lattice.
   * @enum { number }
   * @syscap SystemCapability.Graphics.Drawing
   * @since 12
   */
  export enum RectType {
    /**
     * Draws image into lattice rect.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    DEFAULT = 0,

    /**
     * Skips lattice rect by making it transparent.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    TRANSPARENT = 1,

    /**
     * Draws one of fColors into lattice rect.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    FIXEDCOLOR = 2
  }

   /**
   * Enumerates of operations when two regions are combined.
   * @enum { number }
   * @syscap SystemCapability.Graphics.Drawing
   * @since 12
   */
  export enum RegionOp {
    /**
     * Difference operation.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    DIFFERENCE = 0,

    /**
     * Intersect operation.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    INTERSECT = 1,

    /**
     * Union operation.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    UNION = 2,

    /**
     * Xor operation.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    XOR = 3,

    /**
     * Reverse difference operation.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    REVERSE_DIFFERENCE = 4,

    /**
     * Replace operation.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    REPLACE = 5
  }
}

export default drawing;
