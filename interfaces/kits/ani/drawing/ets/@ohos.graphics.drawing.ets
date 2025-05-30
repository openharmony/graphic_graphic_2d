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

class Cleaner {
  private ptr: long = 0;
  private className: string;
  constructor(className: string, ptr: long) {
    this.ptr = ptr;
    this.className = className;
  }
  native clean(): void;
}

let destroyRegister = new FinalizationRegistry<Cleaner>((cleaner: Cleaner)=>{
  cleaner.clean();
});
let unregisterToken = new object();

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
    private cleaner: Cleaner | null = null;

    registerCleaner(ptr: long): void {
      this.cleaner = new Cleaner("SamplingOptions", ptr);
      destroyRegister.register(this, this.cleaner!, unregisterToken);
    }

    constructor() {
      this.constructorNative();
      this.registerCleaner(this.nativeObj);
    };

    constructor(filterMode: FilterMode) {
      this.constructorNative(filterMode);
      this.registerCleaner(this.nativeObj);
    };

    /**
     * Constructor for the samplingOptions.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native constructorNative(): void;
    /**
     * Constructor for the samplingOptions with filter mode.
     * @param { FilterMode } filterMode - Storage filter mode.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native constructorNative(filterMode: FilterMode): void;
  }

  export class Canvas {
    static { loadLibrary("drawing_ani"); }
    private nativeObj : long = 0;
    private cleaner: Cleaner | null = null;

    registerCleaner(ptr: long): void {
      this.cleaner = new Cleaner("Canvas", ptr);
      destroyRegister.register(this, this.cleaner!, unregisterToken);
    }

    constructor(pixelmap: image.PixelMap) {
      this.constructorNative(pixelmap);
      this.registerCleaner(this.nativeObj);
    };

    /**
     * Constructor for the Canvas.
     * @param { image.PixelMap } pixelmap - PixelMap.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    native constructorNative(pixelmap: image.PixelMap): void;

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
    private cleaner: Cleaner | null = null;

    registerCleaner(ptr: long): void {
      this.cleaner = new Cleaner("ColorFilter", ptr);
      destroyRegister.register(this, this.cleaner!, unregisterToken);
    }

    bindNativePtr(ptr: long): void {
      if (this.nativeObj == 0) {
        this.nativeObj = ptr;
        this.registerCleaner(this.nativeObj);
      }
    }

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
    private cleaner: Cleaner | null = null;

    registerCleaner(ptr: long): void {
      this.cleaner = new Cleaner("Pen", ptr);
      destroyRegister.register(this, this.cleaner!, unregisterToken);
    }

    constructor() {
      this.constructorNative();
      this.registerCleaner(this.nativeObj);
    };

    constructor(pen: Pen) {
      this.constructorNative(pen);
      this.registerCleaner(this.nativeObj);
    };

    /**
     * Constructor for the pen.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native constructorNative(): void;

    /**
     * Constructor for the pen from an existing pen object pen.
     * @param { Pen } pen - Indicates the Pen object.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native constructorNative(pen: Pen): void;

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
    private cleaner: Cleaner | null = null;

    registerCleaner(ptr: long): void {
      this.cleaner = new Cleaner("Brush", ptr);
      destroyRegister.register(this, this.cleaner!, unregisterToken);
    }

    constructor() {
      this.constructorNative();
      this.registerCleaner(this.nativeObj);
    };

    constructor(brush: Brush) {
      this.constructorNative(brush);
      this.registerCleaner(this.nativeObj);
    };

    /**
     * Constructor for the Brush.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native constructorNative(): void;

    /**
     * Constructor for the Brush from an existing brush object brush.
     * @param { Brush } brush - Indicates the Brush object.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native constructorNative(brush: Brush): void;

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
    private cleaner: Cleaner | null = null;

    registerCleaner(ptr: long): void {
      this.cleaner = new Cleaner("Typeface", ptr);
      destroyRegister.register(this, this.cleaner!, unregisterToken);
    }

    bindNativePtr(ptr: long): void {
      if (this.nativeObj == 0) {
        this.nativeObj = ptr;
        this.registerCleaner(this.nativeObj);
      }
    }

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

     /**
     * Generate typeface from file and TypefaceArguments.
     * @param { string } filePath - file path for typeface.
     * @param { TypefaceArguments } typefaceArguments - TypefaceArguments for typeface.
     * @returns { Typeface } Typeface.
     * @static
     * @syscap SystemCapability.Graphics.Drawing
     * @crossplatform
     * @since 20
     */
    static native makeFromFileWithArguments(filePath: string, typefaceArguments: TypefaceArguments): Typeface;
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
    private cleaner: Cleaner | null = null;

    registerCleaner(ptr: long): void {
      this.cleaner = new Cleaner("Font", ptr);
      destroyRegister.register(this, this.cleaner!, unregisterToken);
    }

    constructor() {
      this.constructorNative();
      this.registerCleaner(this.nativeObj);
    };

    /**
     * Constructor for the Font.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    native constructorNative(): void;

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
    private cleaner: Cleaner | null = null;

    registerCleaner(ptr: long): void {
      this.cleaner = new Cleaner("Lattice", ptr);
      destroyRegister.register(this, this.cleaner!, unregisterToken);
    }

    bindNativePtr(ptr: long): void {
      if (this.nativeObj == 0) {
        this.nativeObj = ptr;
        this.registerCleaner(this.nativeObj);
      }
    }

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
   * Implements a matrix.
   *
   * @syscap SystemCapability.Graphics.Drawing
   * @since 12
   */
  export class Matrix {
    static { loadLibrary("drawing_ani"); }
    private nativeObj: long = 0;
    private cleaner: Cleaner | null = null;

    registerCleaner(ptr: long): void {
      this.cleaner = new Cleaner("Matrix", ptr);
      destroyRegister.register(this, this.cleaner!, unregisterToken);
    }

    constructor() {
      this.constructorNative();
      this.registerCleaner(this.nativeObj);
    };

    constructor(matrix: Matrix) {
      this.constructorNative(matrix);
      this.registerCleaner(this.nativeObj);
    };

    /**
     * Creates a Matrix object.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native constructorNative(): void;

    /**
     * Creates a deep copy of the specified matrix object.
     * @param { Matrix } matrix - The matrix object to copy.
     * @syscap SystemCapability.Graphics.Drawing
     * @crossplatform
     * @since 20
     */
    native constructorNative(matrix: Matrix): void;

    /**
     * Resets this matrix to an identity matrix.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native reset(): void;

    /**
     * Sets this matrix as an identity matrix and translates it by a given distance (dx, dy).
     * @param { number } dx - Horizontal distance to translate. A positive number indicates a translation towards the positive direction of the X axis,
     * and a negative number indicates a translation towards the negative direction of the X axis. The value is a floating point number.
     * @param { number } dy - Vertical distance to translate. A positive number indicates a translation towards the positive direction of the Y axis,
     * and a negative number indicates a translation towards the negative direction of the Y axis. The value is a floating point number.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native setTranslation(dx: number, dy: number): void;

    /**
     * Obtains the value of a given index in this matrix. The index ranges from 0 to 8.
     * @param { number } index - Index. The value is an integer ranging from 0 to 8.
     * @returns { number } Returns value corresponding to index.Returns 0 if out of range.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types; 3. Parameter verification failed.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native getValue(index: number): number;
  }

  /**
   * Implements a path operation iterator. You can read path operation instructions by traversing the iterator.
   *
   * @syscap SystemCapability.Graphics.Drawing
   * @since 18
   */
  export class PathIterator {
    static { loadLibrary("drawing_ani"); }
    private nativeObj: long = 0;
    private cleaner: Cleaner | null = null;

    registerCleaner(ptr: long): void {
      this.cleaner = new Cleaner("PathIterator", ptr);
      destroyRegister.register(this, this.cleaner!, unregisterToken);
    }

    constructor(path: Path) {
      this.constructorNative(path);
      this.registerCleaner(this.nativeObj);
    };

    /**
     * Creates an iterator and binds it with a path.
     * @param { Path } path - Path object bound to the iterator.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 18
     */
    native constructorNative(path: Path): void;
  }

  /**
   * A compound geometric path consisting of line segments, arcs, quadratic Bezier curves, and cubic Bezier curves.
   *
   * @syscap SystemCapability.Graphics.Drawing
   * @since 11
   */
  export class Path {
    static { loadLibrary("drawing_ani"); }
    private nativeObj: long = 0;
    private cleaner: Cleaner | null = null;

    registerCleaner(ptr: long): void {
      this.cleaner = new Cleaner("Path", ptr);
      destroyRegister.register(this, this.cleaner!, unregisterToken);
    }

    constructor() {
      this.constructorNative();
      this.registerCleaner(this.nativeObj);
    };

    constructor(path: Path) {
      this.constructorNative(path);
      this.registerCleaner(this.nativeObj);
    };

    /**
     * Constructs a path.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native constructorNative(): void;

    /**
     * Constructs a copy of an existing path.
     * @param { Path } path - Path to copy.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native constructorNative(path: Path): void;

    /**
     * Draws an arc to this path using angle arc mode. This mode first defines a rectangle and takes its inscribed ellipse.
     * Then, it specifies a start angle and a sweep angle. The arc is the portion of the ellipse's circumference defined by the start angle
     * and the sweep angle. By default, a line segment from the last point of the path to the start point of the arc is also added.
     * @param { number } x1 - X coordinate of the upper left corner of the rectangle. The value is a floating point number.
     * @param { number } y1 - Y coordinate of the upper left corner of the rectangle. The value is a floating point number.
     * @param { number } x2 - X coordinate of the lower right corner of the rectangle. The value is a floating point number.
     * @param { number } y2 - Y coordinate of the lower right corner of the rectangle. The value is a floating point number.
     * @param { number } startDeg - Start angle. The start direction (0°) of the angle is the positive direction of the X axis.
     * @param { number } sweepDeg - Angle to sweep, in degrees. A positive number indicates a clockwise sweep,
     * and a negative value indicates a counterclockwise swipe. The actual swipe degree is the modulo operation result of the input parameter by 360.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    native arcTo(x1: number, y1: number, x2: number, y2: number, startDeg: number, sweepDeg: number): void;

    /**
     * Resets the path data.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    native reset(): void;
  }

  /**
   * Describes a region, which is used to describe the region where the shape can be drawn.
   * @syscap SystemCapability.Graphics.Drawing
   * @since 12
   */
  export class Region {
    static { loadLibrary("drawing_ani"); }
    private nativeObj: long = 0;
    private cleaner: Cleaner | null = null;

    registerCleaner(ptr: long): void {
      this.cleaner = new Cleaner("Region", ptr);
      destroyRegister.register(this, this.cleaner!, unregisterToken);
    }

    constructor() {
      this.constructorNative();
      this.registerCleaner(this.nativeObj);
    };

    constructor(region: Region) {
      this.constructorNative(region);
      this.registerCleaner(this.nativeObj);
    };

    constructor(left: number, top: number, right: number, bottom: number) {
      this.constructorNative(left, top, right, bottom);
      this.registerCleaner(this.nativeObj);
    };

    /**
     * Creates an empty region.
     * @syscap SystemCapability.Graphics.Drawing
     * @crossplatform
     * @since 20
     */
    native constructorNative(): void;

    /**
     * Creates a deep copy of the specified region object.
     * @param { Region } region - The region object to copy.
     * @syscap SystemCapability.Graphics.Drawing
     * @crossplatform
     * @since 20
     */
    native constructorNative(region: Region): void;

    /**
     * Creates a region with a rectangle.
     * @param { number } left - Indicates the left edge of the rectangle.
     * @param { number } top - Indicates the top edge of the rectangle.
     * @param { number } right - Indicates the right edge of the rectangle.
     * @param { number } bottom - Indicates the bottom edge of the rectangle.
     * @syscap SystemCapability.Graphics.Drawing
     * @crossplatform
     * @since 20
     */
    native constructorNative(left: number, top: number, right: number, bottom: number): void;
  }

  /**
   * Implements a rounded rectangle.
   *
   * @syscap SystemCapability.Graphics.Drawing
   * @since 12
   */
  export class RoundRect {
    static { loadLibrary("drawing_ani"); }
    private nativeObj: long = 0;
    private cleaner: Cleaner | null = null;

    registerCleaner(ptr: long): void {
      this.cleaner = new Cleaner("RoundRect", ptr);
      destroyRegister.register(this, this.cleaner!, unregisterToken);
    }

    constructor(roundRect: RoundRect) {
      this.constructorNative(roundRect);
      this.registerCleaner(this.nativeObj);
    };

    constructor(rect: common2D.Rect, xRadii: number, yRadii: number) {
      this.constructorNative(rect, xRadii, yRadii);
      this.registerCleaner(this.nativeObj);
    };

    /**
     * Creates a deep copy of the specified round rect object.
     * @param { RoundRect } roundRect - The round rect object to copy.
     * @syscap SystemCapability.Graphics.Drawing
     * @crossplatform
     * @since 20
     */
    native constructorNative(roundRect: RoundRect): void;

    /**
     * A constructor used to create a RoundRect object. A rounded rectangle is created when both xRadii and yRadii are greater than 0. Otherwise, only a rectangle is created.
     * @param { common2D.Rect } rect - Rectangle that encloses the rounded rectangle to create.
     * @param { number } xRadii - Radius of the rounded corner on the X axis. The value is a floating point number. A negative number is invalid.
     * @param { number } yRadii - Radius of the rounded corner on the Y axis. The value is a floating point number. A negative number is invalid.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native constructorNative(rect: common2D.Rect, xRadii: number, yRadii: number): void;
  }

  /**
   * Provides an interface to the drawing, and describe the arguments for a font.
   * @syscap SystemCapability.Graphics.Drawing
   * @crossplatform
   * @since 20
   */
  export class TypefaceArguments {
    static { loadLibrary("drawing_ani"); }
    private nativeObj: long = 0;
    private cleaner: Cleaner | null = null;

    registerCleaner(ptr: long): void {
      this.cleaner = new Cleaner("TypefaceArguments", ptr);
      destroyRegister.register(this, this.cleaner!, unregisterToken);
    }

    constructor() {
      this.constructorNative();
      this.registerCleaner(this.nativeObj);
    };

    /**
     * Constructor for the TypefaceArguments.
     * @syscap SystemCapability.Graphics.Drawing
     * @crossplatform
     * @since 20
     */
    native constructorNative(): void;
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
