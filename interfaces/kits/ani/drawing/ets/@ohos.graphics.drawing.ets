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
  loadLibrary("drawing_ani_core");
  class Cleaner {
    static { loadLibrary("drawing_ani_core"); }
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
   * Enumerate blending modes for colors.
   * Blend is a operation that use 4 components(red, green, blue, alpha) to generate
   * a new color from two colors(source, destination).
   * @enum { int }
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
   * Enumerates the directions of a closed contour.
   * @enum { int }
   * @syscap SystemCapability.Graphics.Drawing
   * @since 20
   */
  export enum PathDirection {
    /**
     * Adds a closed contour clockwise.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    CLOCKWISE = 0,

    /**
     * Adds a closed contour counterclockwise.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    COUNTER_CLOCKWISE = 1,
  }

  /**
   * Enumerates the fill types of a path.
   * @enum { int }
   * @syscap SystemCapability.Graphics.Drawing
   * @since 20
   */
  export enum PathFillType {
    /**
     * Specifies that "inside" is computed by a non-zero sum of signed edge crossings. Specifically, draws a point and emits a ray in any direction.
     * A count is used to record the number of intersection points of the ray and path, and the initial count is 0.
     * When encountering a clockwise intersection point (the path passes from the left to the right of the ray), the count increases by 1.
     * When encountering a counterclockwise intersection point (the path passes from the right to the left of the ray), the count decreases by 1.
     * If the final count is not 0, the point is inside the path and needs to be colored. If the final count is 0, the point is not colored.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    WINDING = 0,

    /**
     * Specifies that "inside" is computed by an odd number of edge crossings. Specifically, draws a point and emits a ray in any direction.
     * If the number of intersection points of the ray and path is an odd number, the point is considered to be inside the path and needs to be colored.
     * If the number is an even number, the point is not colored.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    EVEN_ODD = 1,

    /**
     * Same as WINDING, but draws outside of the path, rather than inside.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    INVERSE_WINDING = 2,

    /**
     * Same as EVEN_ODD, but draws outside of the path, rather than inside.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    INVERSE_EVEN_ODD = 3,
  }

  /**
  * Enumerates the dimensions of matrix information in path measurement.
  * It is often used in animation scenarios where objects move along a path.
  * @enum { int }
  * @syscap SystemCapability.Graphics.Drawing
  * @since 20
  */
  export enum PathMeasureMatrixFlags {
    /**
     * Matrix corresponding to the position information.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    GET_POSITION_MATRIX = 0,

    /**
     * Matrix corresponding to the tangent information.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    GET_TANGENT_MATRIX = 1,

    /**
     * Matrix corresponding to the position and tangent information.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    GET_POSITION_AND_TANGENT_MATRIX = 2,
  }

  /**
   * Enumerates of operations when two paths are combined.
   * @enum { int }
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
   * Provides an interface to the drawing, and samplingOptions used when sampling from the image.
   * @syscap SystemCapability.Graphics.Drawing
   * @since 12
   */
  export class SamplingOptions {
    static { loadLibrary("drawing_ani_core"); }
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

    static samplingOptionsTransferStatic(input: Any): Object {
      let samplingOptions: SamplingOptions = new SamplingOptions();
      return SamplingOptions.samplingOptionsTransferStaticNative(samplingOptions, ESValue.wrap(input));
    }

    private static native samplingOptionsTransferStaticNative(samplingOptions: SamplingOptions, input: ESValue): Object;

    static samplingOptionsTransferDynamic(input: Object): Any {
      let samplingOptions = input as SamplingOptions;
      if (!samplingOptions) {
        return null;
      }
      let addr = SamplingOptions.getSamplingOptionsAddr(samplingOptions);
      if (addr == 0) {
        return null;
      }
      let module = ESValue.load("@ohos.graphics.drawing");
      let hasProperty: boolean = module.hasProperty("SamplingOptions");
      if (!hasProperty) {
        return null;
      }
      let cls = module.getProperty("SamplingOptions");
      hasProperty = cls.hasProperty("__createTransfer__");
      if (hasProperty) {
        let initFunc = cls.getProperty("__createTransfer__");
        let obj = initFunc.invoke(ESObject.wrap(addr));
        return obj.unwrap();
      }
      return null;
    }

    private static native getSamplingOptionsAddr(input: SamplingOptions): long;
  }

  export class Canvas {
    static { loadLibrary("drawing_ani_core"); }
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

    static canvasTransferStatic(input: Any): Object {
        return Canvas.canvasTransferStaticNative(ESValue.wrap(input));
    }

    private static native canvasTransferStaticNative(input: ESValue): Object;

    static canvasTransferDynamic(input: Object): Any {
      let canvas = input as Canvas;
      if (!canvas) {
        return null;
      }
      let canvasAddr = Canvas.getCanvasAddr(canvas);
      if (canvasAddr == 0) {
        return null;
      }
      let pixelMapAddr = Canvas.getPixelMapAddr(canvas);
      let owned = Canvas.getOwned(canvas);
      let module = ESValue.load("@ohos.graphics.drawing");
      let hasProperty: boolean = module.hasProperty("Canvas");
      if (!hasProperty) {
        return null;
      }
      let cls = module.getProperty("Canvas");
      hasProperty = cls.hasProperty("__createTransfer__");
      if (hasProperty) {
        let initFunc = cls.getProperty("__createTransfer__");
        let obj = initFunc.invoke(ESObject.wrap(canvasAddr), ESObject.wrap(pixelMapAddr), owned);
        return obj.unwrap();
      }
      return null;
    }

    private static native getCanvasAddr(input: Canvas): long;
    private static native getPixelMapAddr(input: Canvas): long;
    private static native getOwned(input: Canvas): boolean;

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
     * @param { double } left - Indicates the left position of the rectangle.
     * @param { double } top - Indicates the top position of the rectangle.
     * @param { double } right - Indicates the right position of the rectangle.
     * @param { double } bottom - Indicates the bottom position of the rectangle.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native drawRect(left: double, top: double, right: double, bottom: double): void;

    /**
     * Draws a rounded rectangle.
     * @param { RoundRect } roundRect - Indicates the RectRound object.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native drawRoundRect(roundRect: RoundRect): void;

    /**
     * Draws two nested rounded rectangles. The outer rectangle boundary must contain the inner rectangle boundary.
     * Otherwise, there is no drawing effect.
     * @param { RoundRect } outer - Outer rounded rectangle.
     * @param { RoundRect } inner - Inner rounded rectangle.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native drawNestedRoundRect(outer: RoundRect, inner: RoundRect): void;

    /**
     * Uses a brush to fill the drawable area of the canvas.
     * @param { Brush } brush - Indicates the Brush object.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native drawBackground(brush: Brush): void;

    /**
     * Draws a spot shadow and uses a given path to outline the ambient shadow.
     * @param { Path } path - Path object, which is used to outline the shadow.
     * @param { common2D.Point3d } planeParams - 3D vector,
     *     which is used to determine the z-axis offset of an occluder relative to the canvas,
     *     based on its x and y coordinates.
     * @param { common2D.Point3d } devLightPos - Position of the light relative to the canvas.
     * @param { double } lightRadius - Radius of the light. The value is a floating point number.
     * @param { common2D.Color } ambientColor - Color of the ambient shadow.
     * @param { common2D.Color } spotColor - Color of the spot shadow.
     * @param { ShadowFlag } flag - Shadow flag.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     *     <br>2. Incorrect parameter types; 3. Parameter verification failed.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native drawShadow(path: Path, planeParams: common2D.Point3d, devLightPos: common2D.Point3d, lightRadius: double,
      ambientColor: common2D.Color, spotColor: common2D.Color, flag: ShadowFlag) : void;

    /**
     * Draws a spot shadow and uses a given path to outline the ambient shadow.
     * @param { Path } path - Path object, which is used to outline the shadow.
     * @param { common2D.Point3d } planeParams - 3D vector, which is used to calculate the offset in the Z axis.
     * @param { common2D.Point3d } devLightPos - Position of the light relative to the canvas.
     * @param { double } lightRadius - Radius of the light. The value is a floating point number.
     * @param { common2D.Color | int } ambientColor - Ambient shadow color,
     *     represented by a 32-bit unsigned integer in hexadecimal ARGB format.
     * @param { common2D.Color | int } spotColor - Spot shadow color,
     *     represented by a 32-bit unsigned integer in hexadecimal ARGB format.
     * @param { ShadowFlag } flag - Indicates the flag to control opaque occluder, shadow, and light position.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     *     <br>2. Incorrect parameter types; 3. Parameter verification failed.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 18
     */

    native drawShadow(path: Path, planeParams: common2D.Point3d, devLightPos: common2D.Point3d, lightRadius: double,
      ambientColor: common2D.Color | int, spotColor: common2D.Color | int, flag: ShadowFlag) : void;

    /**
     * Draws a circle. If the radius is less than or equal to zero, nothing is drawn.
     * By default, black is used for filling.
     * @param { double } x - X coordinate of the center of the circle. The value is a floating point number.
     * @param { double } y - Y coordinate of the center of the circle. The value is a floating point number.
     * @param { double } radius - Radius of the circle. The value is a floating point number greater than 0.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     *     <br>2. Incorrect parameter types; 3. Parameter verification failed.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    native drawCircle(x: double, y: double, radius: double): void;

    /**
     * Draw a pixelmap, with the upper left corner at (left, top).
     * @param { image.PixelMap } pixelmap - PixelMap.
     * @param { double } left - Left side of image.
     * @param { double } top - Top side of image.
     * @throws { BusinessError } 401 - Parameter error.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    /**
     * Draws an image. The coordinates of the upper left corner of the image are (left, top).
     * @param { image.PixelMap } pixelmap - PixelMap.
     * @param { double } left - X coordinate of the upper left corner of the image. The value is a floating point number.
     * @param { double } top - Y coordinate of the upper left corner of the image. The value is a floating point number.
     * @param { SamplingOptions } samplingOptions - Sampling options. By default, the SamplingOptions object created using the no-argument constructor is used.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native drawImage(pixelmap: image.PixelMap, left: double, top: double, samplingOptions?: SamplingOptions): void;

    /**
     * Splits an image into multiple sections based on the lattice object's configuration and draws each section into the specified target rectangle on the canvas.
     * The intersections of even-numbered rows and columns (starting from 0) are fixed points.
     * If the fixed lattice area fits within the target rectangle, it will be drawn without scaling.
     * Otherwise, it will be scaled proportionally to fit the target rectangle.
     * Any remaining space will be filled by stretching or compressing the remaining sections to cover the entire target rectangle.
     * @param { image.PixelMap } pixelmap - The source image.
     * @param { Lattice } lattice - The area of source image.
     * @param { common2D.Rect } dstRect - The area of destination canvas.
     * @param { FilterMode } filterMode - Filter mode.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types; 3. Parameter verification failed.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 18
     */
    native drawImageLattice(pixelmap: image.PixelMap, lattice: Lattice, dstRect: common2D.Rect,
      filterMode: FilterMode): void;

    /**
     * Splits an image into nine sections using two horizontal and two vertical lines: four edge sections, four corner sections, and a central section.
     * If the four corner sections are smaller than the target rectangle, they will be drawn in the target rectangle without scaling.
     * Otherwise, they will be scaled to fit the target rectangle. Any remaining space will be filled by stretching or
     * compressing the other five sections to cover the entire target rectangle.
     * @param { image.PixelMap } pixelmap - PixelMap to split.
     * @param { common2D.Rect } center - Central rectangle that divides the image into nine sections by extending its four edges.
     * @param { common2D.Rect } dstRect - Target rectangle drawn on the canvas.
     * @param { FilterMode } filterMode - Filter mode.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types; 3. Parameter verification failed.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 18
     */
    native drawImageNine(pixelmap: image.PixelMap, center: common2D.Rect, dstRect: common2D.Rect,
      filterMode: FilterMode): void;

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
     * Draws a portion of an image onto a specified area of the canvas.
     * @param { image.PixelMap } pixelmap - The source image.
     * @param { common2D.Rect } srcRect - Rectangle object, which specifies the portion of the image to draw.
     * @param { common2D.Rect } dstRect - Rectangle object, which specifies the area of the canvas onto which the image will be drawn.
     * @param { SamplingOptions } samplingOptions - Sampling options. By default, the SamplingOptions object created using the no-argument constructor is used.
     * @param { SrcRectConstraint } constraint - Constraint type of the source rectangle. The default value is STRICT.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native drawImageRectWithSrc(pixelmap: image.PixelMap, srcRect: common2D.Rect, dstRect: common2D.Rect,
      samplingOptions?: SamplingOptions, constraint?: SrcRectConstraint): void;

    /**
     * Draws the background color.
     * @param { common2D.Color } color - The range of color channels must be [0, 255].
     * @param { BlendMode } blendMode - Used to combine source color and destination. The default value is SRC_OVER.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types; 3. Parameter verification failed.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    native drawColor(color: common2D.Color, blendMode?: BlendMode): void;

    /**
     * Fills the drawable area of the canvas with the specified color and blend mode. This API provides better performance and is recommended.
     * @param { int } alpha - Alpha channel value of the color in ARGB format.
     * The value is an integer ranging from 0 to 255. Any passed-in floating point number is rounded down.
     * @param { int } red - Red channel value of the color in ARGB format.
     * The value is an integer ranging from 0 to 255. Any passed-in floating point number is rounded down.
     * @param { int } green - Green channel value of the color in ARGB format.
     * The value is an integer ranging from 0 to 255. Any passed-in floating point number is rounded down.
     * @param { int } blue - Blue channel value of the color in ARGB format.
     * The value is an integer ranging from 0 to 255. Any passed-in floating point number is rounded down.
     * @param { BlendMode } blendMode - Blend mode. The default mode is SRC_OVER.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types; 3. Parameter verification failed.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native drawColor(alpha: int, red: int, green: int, blue: int, blendMode?: BlendMode): void;

    /**
     * Fills the drawable area of the canvas with the specified color and blend mode.
     * @param { int } color - Color in hexadecimal ARGB format.
     * @param { BlendMode } blendMode - Blend mode. The default mode is SRC_OVER.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types; 3. Parameter verification failed.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 18
     */
    native drawColor(color: int, blendMode?: BlendMode): void;

    /**
     * Draws an oval on the canvas, where the shape and position of the oval are defined by its bounding rectangle.
     * @param { common2D.Rect } oval - Rectangle. The oval inscribed within the rectangle is the oval to draw.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native drawOval(oval: common2D.Rect): void;

    /**
     * Draws an arc on the canvas, with the start angle and sweep angle specified.
     * If the absolute value of the sweep angle exceeds 360 degrees, an ellipse is drawn.
     * @param { common2D.Rect } arc - Rectangular boundary that encapsulates the oval including the arc.
     * @param { double } startAngle - Start angle, in degrees. The value is a floating point number.
     * When the degree is 0, the start point is located at the right end of the oval.
     * A positive number indicates that the start point is placed clockwise,
     * and a negative number indicates that the start point is placed counterclockwise.
     * @param { double } sweepAngle - Angle to sweep, in degrees. The value is a floating point number.
     * A positive number indicates a clockwise sweep, and a negative value indicates a counterclockwise swipe.
     * The valid range is from -360 degrees to 360 degrees. If the absolute value of the sweep angle exceeds 360 degrees,
     * an ellipse is drawn.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native drawArc(arc: common2D.Rect, startAngle: double, sweepAngle: double): void;

    /**
     * Draws an arc on the canvas. It enables you to define the start angle, sweep angle,
     * and whether the arc's endpoints should connect to its center.
     * @param { common2D.Rect } arc - Rectangular boundary that encapsulates the oval including the arc.
     * @param { double } startAngle - Start angle, in degrees. The value is a floating point number.
     * When the degree is 0, the start point is located at the right end of the oval.
     * A positive number indicates that the start point is placed clockwise,
     * and a negative number indicates that the start point is placed counterclockwise.
     * @param { double } sweepAngle - Angle to sweep, in degrees. The value is a floating point number.
     * A positive number indicates a clockwise sweep, and a negative value indicates a counterclockwise swipe.
     * The swipe angle can exceed 360 degrees, and a complete ellipse is drawn.
     * @param { boolean } useCenter - Whether the start point and end point of the arc are connected to its center.
     * The value true means that they are connected to the center; the value false means the opposite.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 18
     */
    native drawArcWithCenter(arc: common2D.Rect, startAngle: double, sweepAngle: double, useCenter: boolean): void;

    /**
     * Draw a point.
     * @param { double } x - X coordinate of the point. The value is a floating point number.
     * @param { double } y - Y coordinate of the point. The value is a floating point number.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    native drawPoint(x: double, y: double): void;

    /**
     * Draws a group of points, line segments, or polygons on the canvas, with the specified drawing mode. An array is used to hold these points.
     * @param { Array<common2D.Point> } points - Array that holds the points to draw. The length cannot be 0.
     * @param { PointMode } mode - Mode in which the points are drawn. The default value is drawing.PointMode.POINTS.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types; 3. Parameter verification failed.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native drawPoints(points: Array<common2D.Point>, mode?: PointMode): void;

    /**
     * Draws a custom path, which contains a set of path outlines. Each path outline can be open or closed.
     * @param { Path } path - Path object to draw.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    native drawPath(path: Path): void;

    /**
     * Draws a line segment from the start point to the end point. If the coordinates of the start point are the same as those of the end point,
     * nothing is drawn.
     * @param { double } x0 - X coordinate of the start point of the line segment. The value is a floating point number.
     * @param { double } y0 - Y coordinate of the start point of the line segment. The value is a floating point number.
     * @param { double } x1 - X coordinate of the end point of the line segment. The value is a floating point number.
     * @param { double } y1 - Y coordinate of the end point of the line segment. The value is a floating point number.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    native drawLine(x0: double, y0: double, x1: double, y1: double): void;

    /**
     * Draws a single character. If the typeface of the current font does not support the character to draw,
     * the system typeface is used to draw the character.
     * @param { string } text - Single character to draw. The length of the string must be 1.
     * @param { Font } font - Font object.
     * @param { double } x - X coordinate of the left point (red point in the figure below) of the character baseline (blue line in the figure below).
     * The value is a floating point number.
     * @param { double } y - Y coordinate of the left point (red point in the figure below) of the character baseline (blue line in the figure below).
     * The value is a floating point number.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types; 3. Parameter verification failed.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native drawSingleCharacter(text: string, font: Font, x: double, y: double): void;

    /**
     * Draws the pixelmap base on the mesh which is evenly distributed across the pixelmap.
     * @param { image.PixelMap } pixelmap - The pixelmap to draw using the mesh.
     * @param { int } meshWidth - The number of columns in the mesh.
     * @param { int } meshHeight - The number of rows in the mesh.
     * @param { Array<double> } vertices - Array of vertices, specifying where the mesh should be drawn.
     * @param { int } vertOffset - Number of vert elements to skip before drawing.
     * @param { Array<int> } colors - Array of colors, specifying a color at each vertex.
     * @param { int } colorOffset - Number of color elements to skip before drawing.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native drawPixelMapMesh(pixelmap: image.PixelMap, meshWidth: int, meshHeight: int,
      vertices: Array<double>, vertOffset: int, colors: Array<int>, colorOffset: int): void;

    /**
     * Draws a region.
     * @param { Region } region - Region to draw.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native drawRegion(region: Region): void;

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
     * @returns { int } Return the number of saved states.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native save(): int;

    /**
     * Saves matrix and clip, and allocates a bitmap for subsequent drawing.
     * Calling restore discards changes to matrix and clip, and draws the bitmap.
     * @param { common2D.Rect | null} rect - Optional layer size. The default value is null.
     * @param { Brush | null} brush - Optional brush effect used to draw the layer. The default value is null.
     * @returns { long } Return the number of saved states before this call.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: Mandatory parameters are left unspecified.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native saveLayer(rect?: common2D.Rect | null, brush?: Brush | null): long;

    /**
     * Clears the canvas with a given color. This API has the same effect as drawcolor.
     * @param { common2D.Color } color - Color in ARGB format. Each color channel is an integer ranging from 0 to 255.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native clear(color: common2D.Color): void;

    /**
     * Clears the canvas with a given color.
     * @param { common2D.Color | int } color - Color, represented by an unsigned integer in hexadecimal ARGB format.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 18
     */
    native clear(color: common2D.Color | int): void;

    /**
     * Restores the canvas status (canvas matrix) saved on the top of the stack.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native restore(): void;

    /**
     * Restores the canvas state (canvas matrix and clipping area) to a specified number.
     * @param { int } count - Depth of the canvas statuses to restore.
     * The value is an integer. If the value is less than or equal to 1, the canvas is restored to the initial state.
     * If the value is greater than the number of canvas statuses that have been saved, no operation is performed.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native restoreToCount(count: int): void;

    /**
     * Gets the number of the canvas status (canvas matrix) saved in the stack.
     * @returns { int } Return represent depth of save state stack.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native getSaveCount(): int;

    /**
     * Obtains the canvas width.
     * @returns { int } Canvas width. The value is a floating point number.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native getWidth(): int;

    /**
     * Obtains the canvas height.
     * @returns { int } Canvas height. The value is a floating point number.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native getHeight(): int;

    /**
     * Obtains the bounds of the cropping region of the canvas.
     * @returns { common2D.Rect } Rect object.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native getLocalClipBounds(): common2D.Rect;

    /**
     * Obtains the canvas matrix.
     * @returns { Matrix } Canvas matrix.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native getTotalMatrix(): Matrix;

    /**
     * Applies a scaling matrix on top of the current canvas matrix (identity matrix by default).
     * Subsequent drawing and clipping operations will automatically have a scaling effect applied to the shapes and positions.
     * @param { double } sx - Scale ratio on the X axis. The value is a floating point number.
     * @param { double } sy - Scale ratio on the Y axis. The value is a floating point number.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native scale(sx: double, sy: double): void;

    /**
     * Applies a skewing matrix on top of the current canvas matrix (identity matrix by default).
     * Subsequent drawing and clipping operations will automatically have a skewing effect applied to the shapes and positions.
     * @param { double } sx - Amount of tilt on the X axis. The value is a floating point number.
     * A positive number tilts the drawing rightwards along the positive direction of the Y axis,
     * and a negative number tilts the drawing leftwards along the positive direction of the Y axis.
     * @param { double } sy - Amount of tilt on the Y axis. The value is a floating point number.
     * A positive number tilts the drawing downwards along the positive direction of the X axis,
     * and a negative number tilts the drawing upwards along the positive direction of the X axis.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native skew(sx: double, sy: double) : void;

    /**
     * Rotates by degrees, positive degrees rotates clockwise.
     * @param { double } degrees - Indicates the amount to rotate, in degrees.
     * @param { double } sx - Indicates the x-axis value of the point to rotate about.
     * @param { double } sy - Indicates the y-axis value of the point to rotate about.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native rotate(degrees: double, sx: double, sy: double) : void;

    /**
     * Applies a translation matrix on top of the current canvas matrix (identity matrix by default).
     * Subsequent drawing and clipping operations will automatically have a translation effect applied to the shapes and positions.
     * @param { double } dx - Distance to translate on the X axis. The value is a floating point number.
     * @param { double } dy - Distance to translate on the Y axis. The value is a floating point number.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native translate(dx: double, dy: double): void;

    /**
     * Clips the drawable area of the canvas using a custom path.
     * @param { Path } path - To combine with clip.
     * @param { ClipOp } clipOp - Clip mode. The default value is INTERSECT.
     * @param { boolean } doAntiAlias - Whether to enable anti-aliasing. The value true means to enable anti-aliasing,
     * and false means the opposite. The default value is false.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native clipPath(path: Path, clipOp?: ClipOp, doAntiAlias?: boolean): void;

    /**
     * Clips the drawable area of the canvas using a rectangle.
     * @param { common2D.Rect } rect - To combine with clipping area.
     * @param { ClipOp } clipOp - Clip mode. The default value is INTERSECT.
     * @param { boolean } doAntiAlias - Whether to enable anti-aliasing. The value true means to enable anti-aliasing,
     * and false means the opposite. The default value is false.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native clipRect(rect: common2D.Rect, clipOp?: ClipOp, doAntiAlias?: boolean): void;

    /**
     * Multiplies the current canvas matrix by the incoming matrix on the left. This API does not affect previous drawing operations,
     * but subsequent drawing and clipping operations will be influenced by this matrix in terms of shape and position.
     * @param { Matrix } matrix - Declares functions related to the matrix object in the drawing module.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native concatMatrix(matrix: Matrix): void;
  
    /**
     * Clips a region on the canvas.
     * @param { Region } region - Region object, which indicates the range to clip.
     * @param { ClipOp } clipOp - Clipping mode. The default value is INTERSECT.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native clipRegion(region: Region, clipOp?: ClipOp): void;

    /**
     * Clips a rounded rectangle on the canvas.
     * @param { RoundRect } roundRect - To combine with clipping area.
     * @param { ClipOp } clipOp - Clipping mode. The default value is INTERSECT.
     * @param { boolean } doAntiAlias - Whether to enable anti-aliasing. The value true means to enable anti-aliasing,
     * and false means the opposite. The default value is false.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native clipRoundRect(roundRect: RoundRect, clipOp?: ClipOp, doAntiAlias?: boolean): void;

    /**
     * Checks whether the region that can be drawn is empty after clipping.
     * @returns { boolean } Returns true if drawable area is empty.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native isClipEmpty(): boolean;

    /**
     * Sets a matrix for the canvas. Subsequent drawing and clipping operations will be affected by this matrix in terms of shape and position.
     * @param { Matrix } matrix - Declares functions related to the matrix object in the drawing module.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native setMatrix(matrix: Matrix): void;

    /**
     * Resets the matrix of this canvas to an identity matrix.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native resetMatrix(): void;

    /**
     * Checks whether the path is not intersecting with the canvas area. The canvas area includes its boundaries.
     * @param { Path } path - Path to draw.
     * @returns { boolean } Returns true if path is not intersect; returns false otherwise.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 18
     */
    native quickRejectPath(path: Path): boolean;

    /**
     * Checks whether the rectangle is not intersecting with the canvas area. The canvas area includes its boundaries.
     * @param { common2D.Rect } rect - Rectangle to determines.
     * @returns { boolean } Returns true if rect and region is not intersect; returns false otherwise.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 18
     */
    native quickRejectRect(rect: common2D.Rect): boolean;
  }

  /**
   * Enumerates the constraint types of the source rectangle.
   *
   * @enum { int }
   * @syscap SystemCapability.Graphics.Drawing
   * @since 12
   */
  export enum SrcRectConstraint {

    /**
     * The sampling range is strictly confined to the source rectangle, resulting in a slow sampling speed.
     *
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    STRICT = 0,

    /**
     * The sampling range is not limited to the source rectangle and can extend beyond it, allowing for a high sampling speed.
     *
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    FAST = 1
  }

  /**
   * Enumerates clip operations.
   *
   * @enum { int }
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
   * Provides settings for strokes during drawing.
   * @syscap SystemCapability.Graphics.Drawing
   * @since 11
   */
  export class Pen {
    static { loadLibrary("drawing_ani_core"); }
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

    static penTransferStatic(input: Any): Object {
      let pen: Pen = new Pen();
      return Pen.penTransferStaticNative(pen, ESValue.wrap(input));
    }

    private static native penTransferStaticNative(pen: Pen, input: ESValue): Object;

    static penTransferDynamic(input: Object): Any {
      let pen = input as Pen;
      if (!pen) {
        return null;
      }
      let addr = Pen.getPenAddr(pen);
      if (addr == 0) {
        return null;
      }
      let module = ESValue.load("@ohos.graphics.drawing");
      let hasProperty: boolean = module.hasProperty("Pen");
      if (!hasProperty) {
        return null;
      }
      let cls = module.getProperty("Pen");
      hasProperty = cls.hasProperty("__createTransfer__");
      if (hasProperty) {
        let initFunc = cls.getProperty("__createTransfer__");
        let obj = initFunc.invoke(ESObject.wrap(addr));
        return obj.unwrap();
      }
      return null;
    }

    private static native getPenAddr(input: Pen): long;

    /**
    * Replaces alpha, leaving RGB
    *
    * @param { int } alpha - Alpha channel of color. The range of alpha must be [0, 255].
    * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
    * <br>2. Incorrect parameter types; 3. Parameter verification failed.
    * @syscap SystemCapability.Graphics.Drawing
    * @since 11
    */
    native setAlpha(alpha: int): void;

    /**
     * Obtains the alpha of a pen. The alpha is used by the pen to outline a shape.
     * @returns { int } Returns a 8-bit variable that describes the alpha.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native getAlpha(): int;

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
    static { loadLibrary("drawing_ani_core"); }
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

    static brushTransferStatic(input: Any): Object {
      let brush: Brush = new Brush();
      return Brush.brushTransferStaticNative(brush, ESValue.wrap(input));
    }

    private static native brushTransferStaticNative(brush: Brush, input: ESValue): Object;

    static brushTransferDynamic(input: Object): Any {
      let brush = input as Brush;
      if (!brush) {
        return null;
      }
      let addr = Brush.getBrushAddr(brush);
      if (addr == 0) {
        return null;
      }
      let module = ESValue.load("@ohos.graphics.drawing");
      let hasProperty: boolean = module.hasProperty("Brush");
      if (!hasProperty) {
        return null;
      }
      let cls = module.getProperty("Brush");
      hasProperty = cls.hasProperty("__createTransfer__");
      if (hasProperty) {
        let initFunc = cls.getProperty("__createTransfer__");
        let obj = initFunc.invoke(ESObject.wrap(addr));
        return obj.unwrap();
      }
      return null;
    }

    private static native getBrushAddr(input: Brush): long;

    /**
     * Replaces alpha, leaving RGB
     * @param { int } alpha - Alpha channel of color. The range of alpha must be [0, 255].
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types; 3. Parameter verification failed.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    native setAlpha(alpha: int): void;

    /**
     * Obtains the alpha of a brush. The alpha is used by the brush to fill in a shape.
     * @returns { int } Returns a 8-bit variable that describes the alpha.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native getAlpha(): int;

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
    static { loadLibrary("drawing_ani_core"); }
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

    constructor() {
      this.constructorNative();
      this.registerCleaner(this.nativeObj);
    };

    /**
     * Constructor for the Typeface.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    native constructorNative(): void;

    static typefaceTransferStatic(input: Any): Object {
      return Typeface.typefaceTransferStaticNative(ESValue.wrap(input));
    }

    private static native typefaceTransferStaticNative(input: ESValue): Object;

    static typefaceTransferDynamic(input: Object): Any {
      let typeface = input as Typeface;
      if (!typeface) {
        return null;
      }
      let addr = Typeface.getTypefaceAddr(typeface);
      if (addr == 0) {
        return null;
      }
      let module = ESValue.load("@ohos.graphics.drawing");
      let hasProperty: boolean = module.hasProperty("Typeface");
      if (!hasProperty) {
        return null;
      }
      let cls = module.getProperty("Typeface");
      hasProperty = cls.hasProperty("__createTransfer__");
      if (hasProperty) {
        let initFunc = cls.getProperty("__createTransfer__");
        let obj = initFunc.invoke(ESObject.wrap(addr));
        return obj.unwrap();
      }
      return null;
    }

    private static native getTypefaceAddr(input: Typeface): long;

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
     * @since 20
     */
    static native makeFromFileWithArguments(filePath: string, typefaceArguments: TypefaceArguments): Typeface;
  }

  /**
   * Enumerates the font edging types.
   *
   * @enum { int }
   * @syscap SystemCapability.Graphics.Drawing
   * @since 20
   */
  export enum FontEdging {
    /**
     * No anti-aliasing processing is used.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    ALIAS = 0,

    /**
     * Uses anti-aliasing to smooth the jagged edges.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    ANTI_ALIAS = 1,

    /**
     * Uses sub-pixel anti-aliasing to provide a smoother effect for jagged edges.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    SUBPIXEL_ANTI_ALIAS = 2,
  }

  /**
   * Enumerates the font hinting types.
   *
   * @enum { int }
   * @syscap SystemCapability.Graphics.Drawing
   * @since 20
   */
  export enum FontHinting {
    /**
     * No font hinting is used.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    NONE = 0,

    /**
     * Slight font hinting is used to improve contrast.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    SLIGHT = 1,

    /**
     * Normal font hinting is used to improve contrast.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    NORMAL = 2,

    /**
     * Full font hinting is used to improve contrast.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    FULL = 3,
  }

  /**
   * Font controls options applied when drawing and measuring text.
   *
   * @syscap SystemCapability.Graphics.Drawing
   * @since 11
   */
  export class Font {
    static { loadLibrary("drawing_ani_core"); }
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

    static fontTransferStatic(input: Any): Object {
      let font: Font = new Font();
      return Font.fontTransferStaticNative(font, ESValue.wrap(input));
    }

    private static native fontTransferStaticNative(font: Font, input: ESValue): Object;

    static fontTransferDynamic(input: Object): Any {
      let font = input as Font;
      if (!font) {
        return null;
      }
      let addr = Font.getFontAddr(font);
      if (addr == 0) {
        return null;
      }
      let module = ESValue.load("@ohos.graphics.drawing");
      let hasProperty: boolean = module.hasProperty("Font");
      if (!hasProperty) {
        return null;
      }
      let cls = module.getProperty("Font");
      hasProperty = cls.hasProperty("__createTransfer__");
      if (hasProperty) {
        let initFunc = cls.getProperty("__createTransfer__");
        let obj = initFunc.invoke(ESObject.wrap(addr));
        return obj.unwrap();
      }
      return null;
    }

    private static native getFontAddr(input: Font): long;

    /**
     * Sets text size in points. Has no effect if textSize is not greater than or equal to zero.
     * @param { double } textSize - Typographic height of text. The height of the text must be greater than 0.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types; 3. Parameter verification failed.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    native setSize(textSize: double): void;

    /**
     * Obtains the text size.
     * @returns { double } Text size.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    native getSize(): double;

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
   * Enumerates the font measurement flags, which is used to specify whether a field in the font measurement information is valid.
   * @enum { int }
   * @syscap SystemCapability.Graphics.Drawing
   * @since 20
   */
  export enum FontMetricsFlags {
    /**
     * The underlineThickness field in the FontMetrics struct is valid.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    UNDERLINE_THICKNESS_VALID = 1 << 0,

    /**
     * The underlinePosition field in the FontMetrics struct is valid.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    UNDERLINE_POSITION_VALID = 1 << 1,

    /**
     * The strikethroughThickness field in the FontMetrics struct is valid.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    STRIKETHROUGH_THICKNESS_VALID = 1 << 2,

    /**
     * The strikethroughPosition field in the FontMetrics struct is valid.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    STRIKETHROUGH_POSITION_VALID = 1 << 3,

    /**
     * The boundary metrics (such as top, bottom, xMin, and xMax) in the FontMetrics struct are invalid.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    BOUNDS_INVALID = 1 << 4,
  }

  /**
   * The metrics of an Font.
   * @typedef FontMetrics
   * @syscap SystemCapability.Graphics.Drawing
   * @since 11
   */
  export interface FontMetrics {
    /**
     * Font measurement flags that are valid.
     * @type { ?FontMetricsFlags }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    flags?: FontMetricsFlags;

    /**
     * Maximum range above the glyph bounding box.
     * @type { double }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    top: double;

    /**
     * Distance Retained Above Baseline.
     * @type { double }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    ascent: double;

    /**
     * The distance that remains below the baseline.
     * @type { double }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    descent: double;

    /**
     * Maximum range below the glyph bounding box.
     * @type { double }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    bottom: double;

    /**
     * Line Spacing.
     * @type { double }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    leading: double;

    /**
     * Average character width.
     * @type { ?double }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
     avgCharWidth?: double;

     /**
      * Maximum character width.
      * @type { ?double }
      * @syscap SystemCapability.Graphics.Drawing
      * @since 20
      */
     maxCharWidth?: double;

     /**
      * Horizontal distance from the leftmost edge of any glyph bounding box to the origin.
      * This value is usually less than 0, indicating the minimum horizontal coordinate across all glyph bounding boxes.
      * @type { ?double }
      * @syscap SystemCapability.Graphics.Drawing
      * @since 20
      */
     xMin?: double;

     /**
      * Horizontal distance from the rightmost edge of any glyph bounding box to the origin.
      * The value is a positive number, indicating the maximum horizontal coordinate across all glyph bounding boxes.
      * @type { ?double }
      * @syscap SystemCapability.Graphics.Drawing
      * @since 20
      */
     xMax?: double;

     /**
      * Height of the lowercase letter x. The value is usually a negative value.
      * @type { ?double }
      * @syscap SystemCapability.Graphics.Drawing
      * @since 20
      */
     xHeight?: double;

     /**
      * Height of a capital letter. The value is usually a negative value.
      * @type { ?double }
      * @syscap SystemCapability.Graphics.Drawing
      * @since 20
      */
     capHeight?: double;

     /**
      * Thickness of the underline.
      * @type { ?double }
      * @syscap SystemCapability.Graphics.Drawing
      * @since 20
      */
     underlineThickness?: double;

     /**
      * Vertical distance from the baseline to the top of the underline. The value is usually a positive number.
      * @type { ?double }
      * @syscap SystemCapability.Graphics.Drawing
      * @since 20
      */
     underlinePosition?: double;

     /**
      * Thickness of the strikethrough.
      * @type { ?double }
      * @syscap SystemCapability.Graphics.Drawing
      * @since 20
      */
     strikethroughThickness?: double;

     /**
      * Vertical distance from the baseline to the bottom of the strikethrough. The value is usually a negative value.
      * @type { ?double }
      * @syscap SystemCapability.Graphics.Drawing
      * @since 20
      */
     strikethroughPosition?: double;
  }

  /**
   * The metrics of an Font.
   * @typedef FontMetricsInner
   * @syscap SystemCapability.Graphics.Drawing
   * @since 11
   */
  export class FontMetricsInner implements FontMetrics {
    /**
     * Font measurement flags that are valid.
     * @type { ?FontMetricsFlags }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    flags?: FontMetricsFlags;

    /**
     * Maximum range above the glyph bounding box.
     * @type { double }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    top: double = 0;

    /**
     * Distance Retained Above Baseline.
     * @type { double }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    ascent: double = 0;

    /**
     * The distance that remains below the baseline.
     * @type { double }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    descent: double = 0;

    /**
     * Maximum range below the glyph bounding box.
     * @type { double }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    bottom: double = 0;

    /**
     * Line Spacing.
     * @type { double }
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    leading: double = 0;
    constructor() {}

    constructor(
      top: double,
      ascent: double,
      descent: double,
      bottom: double,
      leading: double,
    ) {
      this.top = top;
      this.ascent = ascent;
      this.descent = descent;
      this.bottom = bottom;
      this.leading = leading;
    }

     avgCharWidth?: double;
     maxCharWidth?: double;
     xMin?: double;
     xMax?: double;
     xHeight?: double;
     capHeight?: double;
     underlineThickness?: double;
     underlinePosition?: double;
     strikethroughThickness?: double;
     strikethroughPosition?: double;
  }

  /**
   * Lattice is the class for dividing an image into grids.
   * @syscap SystemCapability.Graphics.Drawing
   * @since 12
   */
  export class Lattice {
    static { loadLibrary("drawing_ani_core"); }
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
    
    static latticeTransferStatic(input: Any): Object {
      return Lattice.latticeTransferStaticNative(ESValue.wrap(input));
    }

    private static native latticeTransferStaticNative(input: ESValue): Object;
    
    static latticeTransferDynamic(input: Object): Any {
      let lattice = input as Lattice;
      if (!lattice) {
        return null;
      }
      let addr = Lattice.getLatticeAddr(lattice);
      if (addr == 0) {
        return null;
      }
      let module = ESValue.load("@ohos.graphics.drawing");
      let hasProperty: boolean = module.hasProperty("Lattice");
      if (!hasProperty) {
        return null;
      }
      let cls = module.getProperty("Lattice");
      hasProperty = cls.hasProperty("__createTransfer__");
      if (hasProperty) {
        let initFunc = cls.getProperty("__createTransfer__");
        let obj = initFunc.invoke(ESObject.wrap(addr));
        return obj.unwrap();
      }
      return null;
    }

    private static native getLatticeAddr(input: Lattice): long;

    /**
     * Divide an image into a rectangular grid. Grid entries on even columns and even rows are fixed;
     * these entries are always drawn at their original size if the destination is large enough. If the destination
     * side is too small to hold the fixed entries, all fixed entries are scaled down to fit.
     * The grid entries not on even columns and rows are scaled to fit the remaining space, if any.
     * @param { Array<int> } xDivs - X coordinate of values used to divide the image.
     * @param { Array<int> } yDivs - Y coordinate of values used to divide the image.
     * @param { int } fXCount - Number of x coordinates. Must be >= 0.
     * @param { int } fYCount - Number of y coordinates. Must be >= 0.
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
    static native createImageLattice(xDivs: Array<int>, yDivs: Array<int>, fXCount: int, fYCount: int,
      fBounds?: common2D.Rect | null, fRectTypes?: Array<RectType> | null, fColors?: Array<common2D.Color> | Array<int> | null): Lattice;
  }

  /**
   * Implements a matrix.
   *
   * @syscap SystemCapability.Graphics.Drawing
   * @since 12
   */
  export class Matrix {
    static { loadLibrary("drawing_ani_core"); }
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
     * @since 20
     */
    native constructorNative(matrix: Matrix): void;

    static matrixTransferStatic(input: Any): Object {
      let matrix: Matrix = new Matrix();
      return Matrix.matrixTransferStaticNative(matrix, ESValue.wrap(input));
    }

    private static native matrixTransferStaticNative(matrix: Matrix, input: ESValue): Object;

    static matrixTransferDynamic(input: Object): Any {
      let matrix = input as Matrix;
      if (!matrix) {
        return null;
      }
      let addr = Matrix.getMatrixAddr(matrix);
      if (addr == 0) {
        return null;
      }
      let module = ESValue.load("@ohos.graphics.drawing");
      let hasProperty: boolean = module.hasProperty("Matrix");
      if (!hasProperty) {
        return null;
      }
      let cls = module.getProperty("Matrix");
      hasProperty = cls.hasProperty("__createTransfer__");
      if (hasProperty) {
        let initFunc = cls.getProperty("__createTransfer__");
        let obj = initFunc.invoke(ESObject.wrap(addr));
        return obj.unwrap();
      }
      return null;
    }
    private static native getMatrixAddr(input: Matrix): long;

    /**
     * Sets this matrix as an identity matrix and rotates it by a given degree around the rotation point (px, py).
     * @param { double } degree - Angle to rotate, in degrees. A positive number indicates a clockwise rotation,
     * and a negative number indicates a counterclockwise rotation. The value is a floating point number.
     * @param { double } px - X coordinate of the rotation point. The value is a floating point number.
     * @param { double } py - Y coordinate of the rotation point. The value is a floating point number.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    native setRotation(degree: double, px: double, py: double): void;

    /**
     * Sets this matrix as an identity matrix and scales it with the coefficients (sx, sy) at the scale point (px, py).
     * @param { double } sx - Scale coefficient along the X axis. If a negative number is passed in,
     * the matrix is mirrored around y = px before being scaled. The value is a floating point number.
     * @param { double } sy - Scale coefficient along the Y axis. If a negative number is passed in,
     * the matrix is mirrored around x = py before being scaled. The value is a floating point number.
     * @param { double } px - X coordinate of the scale point. The value is a floating point number.
     * @param { double } py - Y coordinate of the scale point. The value is a floating point number.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    native setScale(sx: double, sy: double, px: double, py: double): void;

    /**
     * Sets this matrix as an identity matrix and translates it by a given distance (dx, dy).
     * @param { double } dx - Horizontal distance to translate. A positive number indicates a translation towards the positive direction of the X axis,
     * and a negative number indicates a translation towards the negative direction of the X axis. The value is a floating point number.
     * @param { double } dy - Vertical distance to translate. A positive number indicates a translation towards the positive direction of the Y axis,
     * and a negative number indicates a translation towards the negative direction of the Y axis. The value is a floating point number.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    native setTranslation(dx: double, dy: double): void;

    /**
     * Sets parameters for this matrix.
     * @param { Array<double> } values - Each value in the array represents the following parameters:
     * values[0] - horizontal scale factor to store.
     * values[1] - horizontal skew factor to store.
     * values[2] - horizontal translation to store.
     * values[3] - vertical skew factor to store.
     * values[4] - vertical scale factor to store.
     * values[5] - vertical translation to store.
     * values[6] - input x-axis values perspective factor to store.
     * values[7] - input y-axis values perspective factor to store.
     * values[8] - perspective scale factor to store.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types; 3. Parameter verification failed.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    native setMatrix(values: Array<double>): void;

    /**
     * Preconcats the existing matrix with the passed-in matrix.
     * @param { Matrix } matrix - Matrix object, which is on the right of a multiplication expression.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    native preConcat(matrix: Matrix): void;

    /**
     * Checks whether this matrix is equal to another matrix.
     * @param { Matrix } matrix - Matrix to compare.
     * @returns { boolean } Returns true if the two matrices are equal; returns false otherwise.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    native isEqual(matrix: Matrix): boolean;

    /**
     * Inverts this matrix and returns the result.
     * @param { Matrix } matrix - Matrix object used to store the inverted matrix.
     * @returns { boolean } Check result. The value true means that the matrix is revertible and the matrix object is set to its inverse,
     * and false means that the matrix is not revertible and the matrix object remains unchanged.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    native invert(matrix: Matrix): boolean;

    /**
     * Checks whether this matrix is an identity matrix.
     * @returns { Boolean } Returns true if matrix is identity; returns false otherwise.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    native isIdentity(): Boolean;

    /**
     * Obtains the value of a given index in this matrix. The index ranges from 0 to 8.
     * @param { int } index - Index. The value is an integer ranging from 0 to 8.
     * @returns { double } Returns value corresponding to index.Returns 0 if out of range.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types; 3. Parameter verification failed.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    native getValue(index: int): double;

    /**
     * Post multiplies this matrix by a matrix that is derived from an identity matrix after it has been rotated by a
     * given degree around the rotation point (px, py).
     * @param { double } degree - Angle to rotate, in degrees. A positive number indicates a clockwise rotation,
     * and a negative number indicates a counterclockwise rotation. The value is a floating point number.
     * @param { double } px - X coordinate of the rotation point. The value is a floating point number.
     * @param { double } py - Y coordinate of the rotation point. The value is a floating point number.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    native postRotate(degree: double, px: double, py: double): void;

    /**
     * Post multiplies this matrix by a matrix that is derived from an identity matrix after it has been
     * scaled with the coefficient (sx, sy) at the scale point (px, py).
     * @param { double } sx - Scale coefficient along the X axis. If a negative number is passed in,
     * the matrix is mirrored around y = px before being scaled. The value is a floating point number.
     * @param { double } sy - Scale coefficient along the Y axis. If a negative number is passed in,
     * the matrix is mirrored around x = py before being scaled. The value is a floating point number.
     * @param { double } px - X coordinate of the scale point. The value is a floating point number.
     * @param { double } py - Y coordinate of the scale point. The value is a floating point number.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    native postScale(sx: double, sy: double, px: double, py: double): void;

    /**
     * Post multiplies this matrix by a matrix that is derived from an identity matrix after it has been translated by a given distance (dx, dy).
     * @param { double } dx - Horizontal distance to translate. A positive number indicates a translation towards the positive direction of the X axis,
     * and a negative number indicates a translation towards the negative direction of the X axis. The value is a floating point number.
     * @param { double } dy - Vertical distance to translate. A positive number indicates a translation towards the positive direction of the Y axis,
     * and a negative number indicates a translation towards the negative direction of the Y axis. The value is a floating point number.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    native postTranslate(dx: double, dy: double): void;

    /**
     * Premultiplies this matrix by a matrix that is derived from an identity matrix after it has been rotated by a
     * given degree around the rotation point (px, py).
     * @param { double } degree - Angle to rotate, in degrees. A positive number indicates a clockwise rotation,
     * and a negative number indicates a counterclockwise rotation. The value is a floating point number.
     * @param { double } px - X coordinate of the rotation point. The value is a floating point number.
     * @param { double } py - Y coordinate of the rotation point. The value is a floating point number.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    native preRotate(degree: double, px: double, py: double): void;

    /**
     * Premultiplies this matrix by a matrix that is derived from an identity matrix after it has been scaled with the
     * coefficient (sx, sy) at the scale point (px, py).
     * @param { double } sx - Scale coefficient along the X axis. If a negative number is passed in,
     * the matrix is mirrored around y = px before being scaled. The value is a floating point number.
     * @param { double } sy - Scale coefficient along the Y axis. If a negative number is passed in,
     * the matrix is mirrored around x = py before being scaled. The value is a floating point number.
     * @param { double } px - X coordinate of the scale point. The value is a floating point number.
     * @param { double } py - Y coordinate of the scale point. The value is a floating point number.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    native preScale(sx: double, sy: double, px: double, py: double): void;

    /**
     * Premultiplies this matrix by a matrix that is derived from an identity matrix after it has been translated by a given distance (dx, dy).
     * @param { double } dx - Horizontal distance to translate. A positive number indicates a translation towards the positive direction of the X axis,
     * and a negative number indicates a translation towards the negative direction of the X axis. The value is a floating point number.
     * @param { double } dy - Vertical distance to translate. A positive number indicates a translation towards the positive direction of the Y axis,
     * and a negative number indicates a translation towards the negative direction of the Y axis. The value is a floating point number.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    native preTranslate(dx: double, dy: double): void;

    /**
     * Resets this matrix to an identity matrix.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native reset(): void;

    /**
     * Maps a source point array to a destination point array by means of matrix transformation.
     * @param { Array<common2D.Point> } src - Array of source points.
     * @returns { Array<common2D.Point> } Return mapped points array.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    native mapPoints(src: Array<common2D.Point>): Array<common2D.Point>;

    /**
     * Obtains all element values of this matrix.
     * @returns { Array<double> } nine scalar values contained by Matrix.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    native getAll(): Array<double>;

    /**
     * ets the destination rectangle to the bounding rectangle of the shape obtained after transforming the source rectangle
     * with a matrix transformation. As shown in the figure below, the blue rectangle represents the source rectangle,
     * and the yellow rectangle is the shape obtained after a matrix transformation is applied to the source rectangle.
     * Since the edges of the yellow rectangle are not aligned with the coordinate axes, it cannot be represented by a rectangle object.
     * To address this issue, a destination rectangle (black rectangle) is defined as the bounding rectangle.
     * @param { common2D.Rect } dst - Rectangle object, which is used to store the bounding rectangle.
     * @param { common2D.Rect } src - Source rectangle.
     * @returns { boolean } Returns true if the mapped src is equal to the dst; returns false is not equal.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    native mapRect(dst: common2D.Rect, src: common2D.Rect): boolean;

    /**
     * Sets this matrix to a transformation matrix that maps a source rectangle to a destination rectangle.
     * @param { common2D.Rect } src - Source rectangle.
     * @param { common2D.Rect } dst - Destination rectangle.
     * @param { ScaleToFit } scaleToFit - Mapping mode from the source rectangle to the target rectangle.
     * @returns { boolean } Check result. The value true means that the matrix can represent the mapping, and false means the opposite.
     * If either the width or the height of the source rectangle is less than or equal to 0, the API returns false
     * and sets the matrix to an identity matrix. If either the width or height of the destination rectangle is less than or equal to 0,
     * the API returns true and sets the matrix to a matrix with all values 0, except for a perspective scaling coefficient of 1.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types; 3. Parameter verification failed.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    native setRectToRect(src: common2D.Rect, dst: common2D.Rect, scaleToFit: ScaleToFit): boolean;

    /**
     * Sets this matrix to a transformation matrix that maps the source point array to the destination point array.
     * Both the number of source points and that of destination points must be in the range [0, 4].
     * @param { Array<common2D.Point> } src - Array of source points. The array length must be the same as the value of count.
     * @param { Array<common2D.Point> } dst - Array of destination points. The array length must be the same as the value of count.
     * @param { int } count - Number of points in each array. The value is an integer.
     * @returns { boolean } Returns true if Matrix was constructed successfully
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20.
     */
    native setPolyToPoly(src: Array<common2D.Point>, dst: Array<common2D.Point>, count: int): boolean;
  }

  /**
   * Enumerates the modes of scaling a source rectangle into a destination rectangle.
   * @enum { int }
   * @syscap SystemCapability.Graphics.Drawing
   * @since 20
   */
  export enum ScaleToFit {
    /**
     * Scales the source rectangle to completely fill the destination rectangle, potentially changing the aspect ratio of the source rectangle.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    FILL_SCALE_TO_FIT = 0,

    /**
     * Scales the source rectangle, preserving its aspect ratio, to align it to the upper left corner of the destination rectangle.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    START_SCALE_TO_FIT = 1,

    /**
     * Scales the source rectangle, preserving its aspect ratio, to align it to the center of the destination rectangle.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    CENTER_SCALE_TO_FIT = 2,

    /**
     * Scales the source rectangle, preserving its aspect ratio, to align it to the lower right corner of the destination rectangle.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    END_SCALE_TO_FIT = 3
  }

  /**
   * Implements a mask filter.
   * @syscap SystemCapability.Graphics.Drawing
   * @since 20
   */
  export class MaskFilter {
    static { loadLibrary("drawing_ani_core"); }
    private nativeObj : long = 0;
    private cleaner: Cleaner | null = null;

    registerCleaner(ptr: long): void {
      this.cleaner = new Cleaner("MaskFilter", ptr);
      destroyRegister.register(this, this.cleaner!, unregisterToken);
    }

    bindNativePtr(ptr: long): void {
      if (this.nativeObj == 0) {
        this.nativeObj = ptr;
        this.registerCleaner(this.nativeObj);
      }
    }

    /**
     * Creates a mask filter with a blur effect.
     * @param { BlurType } blurType - Blur type.
     * @param { double } sigma - Standard deviation of the Gaussian blur to apply. The value must be a floating point number greater than 0.
     * @returns { MaskFilter | undefined } MaskFilter object.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types; 3. Parameter verification failed.
     * @static
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    static native createBlurMaskFilter(blurType: BlurType, sigma: double): MaskFilter | undefined;
  }

  /**
   * Enumerates the styles of the dashed path effect.
   * @enum { int }
   * @syscap SystemCapability.Graphics.Drawing
   * @since 20
   */
   export enum PathDashStyle {
    /**
     * Translates only, not rotating with the path.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    TRANSLATE = 0,

    /**
     * Rotates with the path.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    ROTATE = 1,

    /**
     * Rotates with the path and stretches or compresses at turns to enhance smoothness.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    MORPH = 2,
  }

  /**
   * Implements a path effect.
   * @syscap SystemCapability.Graphics.Drawing
   * @since 20
   */
  export class PathEffect {
    static { loadLibrary("drawing_ani_core"); }
    private nativeObj : long = 0;
    private cleaner: Cleaner | null = null;

    registerCleaner(ptr: long): void {
      this.cleaner = new Cleaner("PathEffect", ptr);
      destroyRegister.register(this, this.cleaner!, unregisterToken);
    }

    bindNativePtr(ptr: long): void {
      if (this.nativeObj == 0) {
        this.nativeObj = ptr;
        this.registerCleaner(this.nativeObj);
      }
    }

    /**
     * Creates a PathEffect object that converts a path into a dotted line.
     * @param { Array<double> } intervals - Array of ON and OFF lengths of dotted lines.
     * The number of arrays must be an even number and be greater than or equal to 2.
     * @param { double } phase - Offset used during drawing. The value is a floating point number.
     * @returns { PathEffect | undefined } PathEffect object.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types; 3. Parameter verification failed.
     * @static
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    static native createDashPathEffect(intervals: Array<double>, phase: double): PathEffect | undefined;

    /**
     * Creates a path effect that transforms the sharp angle between line segments into a rounded corner with the specified radius.
     * @param { double } radius - Radius of the rounded corner. The value must be greater than 0. The value is a floating point number.
     * @returns { PathEffect | undefined } PathEffect object.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types; 3. Parameter verification failed.
     * @static
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    static native createCornerPathEffect(radius: double): PathEffect | undefined;

    /**
     * Creates an effect that segments the path and scatters the segments in an irregular pattern along the path.
     * @param { double } segLength - Distance along the path at which each segment is fragmented. The value is a floating point number.
     * If a negative number or the value 0 is passed in, no effect is created.
     * @param { double } dev - Maximum amount by which the end points of the segments can be randomly displaced during rendering.
     * The value is a floating-point number.
     * @param { int } seedAssist - Optional parameter to assist in generating a pseudo-random seed for the effect.
     * The default value is 0, and the value is a 32-bit unsigned integer.
     * @returns { PathEffect | undefined } PathEffect object.
     * @static
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    static native createDiscretePathEffect(segLength: double, dev: double, seedAssist?: int): PathEffect | undefined;

     /**
      * Creates a path effect by sequentially applying the inner effect and then the outer effect.
      * @param { PathEffect } outer - Path effect that is applied second, overlaying the first effect.
      * @param { PathEffect } inner - Inner path effect that is applied first.
      * @returns { PathEffect | undefined } PathEffect object.
      * @static
      * @syscap SystemCapability.Graphics.Drawing
      * @since 20
      */
    static native createComposePathEffect(outer: PathEffect, inner: PathEffect): PathEffect | undefined;

     /**
     * Creates a dashed path effect based on the shape described by a path.
     * @param { Path } path - Path that defines the shape to be used for filling each dash in the pattern.
     * @param { double } advance - Distance between two consecutive dashes. The value is a floating point number greater than 0.
     * Otherwise, an error code is thrown.
     * @param { double } phase - Starting offset of the dash pattern. The value is a floating point number.
     * The actual offset used is the absolute value of this value modulo the value of advance.
     * @param { PathDashStyle } style - Style of the dashed path effect.
     * @returns { PathEffect | undefined } PathEffect object.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types; 3. Parameter verification failed.
     * @static
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    static native createPathDashEffect(path: Path, advance: double, phase: double, style: PathDashStyle): PathEffect | undefined;

     /**
      * Creates an overlay path effect based on two distinct path effects.
      * Different from createComposePathEffect, this API applies each effect separately and then displays them as a simple overlay.
      * @param { PathEffect } firstPathEffect - First path effect.
      * @param { PathEffect } secondPathEffect - Second path effect.
      * @returns { PathEffect | undefined } PathEffect object.
      * @static
      * @syscap SystemCapability.Graphics.Drawing
      * @since 20
      */
    static native createSumPathEffect(firstPathEffect: PathEffect, secondPathEffect: PathEffect): PathEffect | undefined;
  }

  /**
   * Enumerates the tile modes of the shader effect.
   * @enum { int }
   * @syscap SystemCapability.Graphics.Drawing
   * @since 20
   */
  export enum TileMode {
    /**
     * Replicates the edge color if the shader effect draws outside of its original boundary.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    CLAMP = 0,

    /**
     * Repeats the shader effect in both horizontal and vertical directions.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    REPEAT = 1,

    /**
     * Repeats the shader effect in both horizontal and vertical directions, alternating mirror images.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    MIRROR = 2,

    /**
     * Renders the shader effect only within the original boundary.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    DECAL = 3,
  }

  /**
   * Defines a color filter.
   *
   * @syscap SystemCapability.Graphics.Drawing
   * @since 20
   */
  export class ColorFilter {
    static { loadLibrary("drawing_ani_core"); }
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

    static colorFilterTransferStatic(input: Any): Object {
      return ColorFilter.colorFilterTransferStaticNative(ESValue.wrap(input));
    }

    private static native colorFilterTransferStaticNative(input: ESValue): Object;

    static colorFilterTransferDynamic(input: Object): Any {
      let colorFilter = input as ColorFilter;
      if (!colorFilter) {
        return null;
      }
      let addr = ColorFilter.getColorFilterAddr(colorFilter);
      if (addr == 0) {
        return null;
      }
      let module = ESValue.load("@ohos.graphics.drawing");
      let hasProperty: boolean = module.hasProperty("ColorFilter");
      if (!hasProperty) {
        return null;
      }
      let cls = module.getProperty("ColorFilter");
      hasProperty = cls.hasProperty("__createTransfer__");
      if (hasProperty) {
        let initFunc = cls.getProperty("__createTransfer__");
        let obj = initFunc.invoke(ESObject.wrap(addr));
        return obj.unwrap();
      }
      return null;
    }

    private static native getColorFilterAddr(input: ColorFilter): long;

    /**
     * Creates a ColorFilter object with a given color and blend mode.
     * @param { common2D.Color } color - Color in ARGB format. The value of each color channel is an integer ranging from 0 to 255.
     * @param { BlendMode } mode - Blend mode.
     * @returns { ColorFilter } Colorfilter object.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types; 3. Parameter verification failed.
     * @static
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    static native createBlendModeColorFilter(color: common2D.Color, mode: BlendMode): ColorFilter;

    /**
     * Creates a ColorFilter object with a given color and blend mode.
     * @param { common2D.Color | int } color - Color, represented by an unsigned integer in hexadecimal ARGB format.
     * @param { BlendMode } mode - Blend mode.
     * @returns { ColorFilter } Colorfilter object.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types; 3. Parameter verification failed.
     * @static
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    static native createBlendModeColorFilter(color: common2D.Color | int, mode: BlendMode): ColorFilter;

    /**
     * Creates a ColorFilter object by combining another two color filters.
     * @param { ColorFilter } outer - Color filter that takes effect later in the new filter.
     * @param { ColorFilter } inner - Color filter that takes effect first in the new filter.
     * @returns { ColorFilter | undefined } Colorfilter object.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @static
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    static native createComposeColorFilter(outer: ColorFilter, inner: ColorFilter): ColorFilter | undefined;

    /**
     * Creates a ColorFilter object that applies the sRGB gamma curve to the RGB channels.
     * @returns { ColorFilter | undefined } Colorfilter object.
     * @static
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    static native createLinearToSRGBGamma(): ColorFilter | undefined;

    /**
     * Creates a ColorFilter object that applies the RGB channels to the sRGB gamma curve.
     * @returns { ColorFilter | undefined } Colorfilter object.
     * @static
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    static native createSRGBGammaToLinear(): ColorFilter | undefined;

    /**
     * Creates a ColorFilter object that multiplies the luma into the alpha channel and sets the RGB channels to zero.
     * @returns {  | undefined } Colorfilter.
     * @static
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    static native createLumaColorFilter(): ColorFilter | undefined;

    /**
     * Creates a color filter object with a 4*5 color matrix.
     * @param { Array<double> } matrix - An array of 20 numbers, indicating the 4*5 matrix.
     * @returns { ColorFilter | undefined } Colorfilter object.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types; 3. Parameter verification failed.
     * @static
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    static native createMatrixColorFilter(matrix: Array<double>): ColorFilter | undefined;
  }

  /**
   * Implements an image filter.
   *
   * @syscap SystemCapability.Graphics.Drawing
   * @since 20
   */
  export class ImageFilter {
    static { loadLibrary("drawing_ani_core"); }
    private nativeObj : long = 0;
    private cleaner: Cleaner | null = null;

    registerCleaner(ptr: long): void {
      this.cleaner = new Cleaner("ImageFilter", ptr);
      destroyRegister.register(this, this.cleaner!, unregisterToken);
    }

    bindNativePtr(ptr: long): void {
      if (this.nativeObj == 0) {
        this.nativeObj = ptr;
        this.registerCleaner(this.nativeObj);
      }
    }

    /**
     * Creates an image filter with a given blur effect.
     * @param { double } sigmaX - Standard deviation of the Gaussian blur along the X axis. The value must be a floating point number greater than 0.
     * @param { double } sigmaY - Standard deviation of the Gaussian blur along the Y axis. The value must be a floating point number greater than 0.
     * @param { TileMode } tileMode - Tile mode to apply to the edges.
     * @param { ImageFilter | null } imageFilter - Filter to which the image filter will be applied.
     * The default value is null, indicating that the image filter is directly applied to the original image.
     * @returns { ImageFilter } ImageFilter object.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types; 3. Parameter verification failed.
     * @static
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    static native createBlurImageFilter(sigmaX: double, sigmaY: double,
      tileMode: TileMode, imageFilter?: ImageFilter | null): ImageFilter;

    /**
     * Creates an image filter object with a given color filter effect.
     * @param { ColorFilter } colorFilter - Color filter.
     * @param { ImageFilter | null } imageFilter - Filter to which the image filter will be applied.
     *     The default value is null,
     * indicating that the image filter is directly applied to the original image.
     * @returns { ImageFilter | undefined } ImageFilter object.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @static
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    static native createFromColorFilter(colorFilter: ColorFilter,
      imageFilter?: ImageFilter | null): ImageFilter | undefined;
  }

  /**
   * Enumerates the join styles of a pen. The join style defines the shape of the joints of a polyline segment drawn by the pen.
   * @enum { int }
   * @syscap SystemCapability.Graphics.Drawing
   * @since 20
   */
  export enum JoinStyle {
    /**
     * Mitered corner. If the angle of a polyline is small, its miter length may be inappropriate.
     * In this case, you need to use the miter limit to limit the miter length.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    MITER_JOIN = 0,

    /**
     * Round corner.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    ROUND_JOIN = 1,

    /**
     * Bevel corner.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    BEVEL_JOIN = 2
  }

  /**
   * Enumerates the cap styles of a pen. The cap style defines the style of both ends of a line segment drawn by the pen.
   * @enum { int }
   * @syscap SystemCapability.Graphics.Drawing
   * @since 20
   */
  export enum CapStyle {
    /**
     * There is no cap style. Both ends of the line segment are cut off square.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    FLAT_CAP = 0,

    /**
     * Square cap style. Both ends have a square, the height of which is half of the width of the line segment, with the same width.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    SQUARE_CAP = 1,

    /**
     * Round cap style. Both ends have a semicircle centered, the diameter of which is the same as the width of the line segment.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    ROUND_CAP = 2
  }

  /**
   * Enumerates the blur types of a mask filter.
   * @enum { int }
   * @syscap SystemCapability.Graphics.Drawing
   * @since 20
   */
  export enum BlurType {
    /**
     * Both the outer edges and the inner solid parts are blurred.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    NORMAL = 0,

    /**
     * The inner solid part remains unchanged, while only the outer edges are blurred.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    SOLID = 1,

    /**
     * Only the outer edges are blurred, with the inner solid part being fully transparent.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    OUTER = 2,

    /**
     * Only the inner solid part is blurred, while the outer edges remain sharp.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    INNER = 3
  }

  /**
   * Enumerates the path operation types contained in an iterator. It is used to read path operation instructions.
   * @enum { int }
   * @syscap SystemCapability.Graphics.Drawing
   * @since 20
   */
  export enum PathIteratorVerb {
    /**
     * Sets the start point.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    MOVE = 0,

    /**
     * Adds a line segment.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    LINE = 1,

    /**
     * Adds a quadratic Bezier curve for smooth transitions.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    QUAD = 2,

    /**
     * Adds a conic curve.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    CONIC = 3,

    /**
     * Adds a cubic Bezier curve for smooth transitions.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    CUBIC = 4,

    /**
     * Closes a path.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    CLOSE = 5,

    /**
     * The path setting is complete.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    DONE = CLOSE + 1,
  }

  /**
   * Implements a path operation iterator. You can read path operation instructions by traversing the iterator.
   *
   * @syscap SystemCapability.Graphics.Drawing
   * @since 18
   */
  export class PathIterator {
    static { loadLibrary("drawing_ani_core"); }
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

    /**
     * Retrieves the next operation in this path and moves the iterator to that operation.
     * @param { Array<common2D.Point> } points - Indicates the point array.
     * @param { int } offset - Indicates the offset into the array where entries should be placed. The default value is 0.
     * @returns { PathIteratorVerb | undefined } Returns the next verb in this iterator's path.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types; 3. Parameter verification failed.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    native next(points: Array<common2D.Point>, offset?: int): PathIteratorVerb | undefined;

    /**
     * Retrieves the next operation in this path, without moving the iterator.
     * @returns { PathIteratorVerb | undefined } Returns the next verb in the iteration.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    native peek(): PathIteratorVerb | undefined;

    /**
     * Checks whether there is a next operation in the path operation iterator.
     * @returns { boolean } Returns true if there are more elements to be iterated through, false otherwise.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    native hasNext(): boolean;
  }

  /**
   * A compound geometric path consisting of line segments, arcs, quadratic Bezier curves, and cubic Bezier curves.
   *
   * @syscap SystemCapability.Graphics.Drawing
   * @since 11
   */
  export class Path {
    static { loadLibrary("drawing_ani_core"); }
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

    static pathTransferStatic(input: Any): Object {
      let path: Path = new Path();
      return Path.pathTransferStaticNative(path, ESValue.wrap(input));
    }

    private static native pathTransferStaticNative(output: Path, input: ESValue): Object;

    static pathTransferDynamic(input: Object): Any {
      let path = input as Path;
      if (!path) {
        return null;
      }
      let addr = Path.getPathAddr(path);
      if (addr == 0) {
        return null;
      }
      let module = ESValue.load("@ohos.graphics.drawing");
      let hasProperty: boolean = module.hasProperty("Path");
      if (!hasProperty) {
        return null;
      }
      let cls = module.getProperty("Path");
      hasProperty = cls.hasProperty("__createTransfer__");
      if (hasProperty) {
        let initFunc = cls.getProperty("__createTransfer__");
        let obj = initFunc.invoke(ESObject.wrap(addr));
        return obj.unwrap();
      }
      return null;
    }

    private static native getPathAddr(input: Path): long;

    /**
     * Sets the start point of this path.
     * @param { double } x - X coordinate of the start point. The value is a floating point number.
     * @param { double } y - Y coordinate of the start point. The value is a floating point number.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    native moveTo(x: double, y: double): void;

    /**
     * Draws a line segment from the last point of this path to the target point. If the path is empty, the start point (0, 0) is used.
     * @param { double } x - X coordinate of the target point. The value is a floating point number.
     * @param { double } y - Y coordinate of the target point. The value is a floating point number.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    native lineTo(x: double, y: double): void;

    /**
     * Draws an arc to this path using angle arc mode. This mode first defines a rectangle and takes its inscribed ellipse.
     * Then, it specifies a start angle and a sweep angle. The arc is the portion of the ellipse's circumference defined by the start angle
     * and the sweep angle. By default, a line segment from the last point of the path to the start point of the arc is also added.
     * @param { double } x1 - X coordinate of the upper left corner of the rectangle. The value is a floating point number.
     * @param { double } y1 - Y coordinate of the upper left corner of the rectangle. The value is a floating point number.
     * @param { double } x2 - X coordinate of the lower right corner of the rectangle. The value is a floating point number.
     * @param { double } y2 - Y coordinate of the lower right corner of the rectangle. The value is a floating point number.
     * @param { double } startDeg - Start angle. The start direction (0°) of the angle is the positive direction of the X axis.
     * @param { double } sweepDeg - Angle to sweep, in degrees. A positive number indicates a clockwise sweep,
     * and a negative value indicates a counterclockwise swipe. The actual swipe degree is the modulo operation result of the input parameter by 360.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    native arcTo(x1: double, y1: double, x2: double, y2: double, startDeg: double, sweepDeg: double): void;

    /**
     * Draws a quadratic Bezier curve from the last point of this path to the target point. If the path is empty, the start point (0, 0) is used.
     * @param { double } ctrlX - X coordinate of the control point. The value is a floating point number.
     * @param { double } ctrlY - Y coordinate of the control point. The value is a floating point number.
     * @param { double } endX - X coordinate of the target point. The value is a floating point number.
     * @param { double } endY - Y coordinate of the target point. The value is a floating point number.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    native quadTo(ctrlX: double, ctrlY: double, endX: double, endY: double): void;

    /**
     * Draws a conic curve from the last point of this path to the target point. If the path is empty, the start point (0, 0) is used.
     * @param { double } ctrlX - X coordinate of the control point. The value is a floating point number.
     * @param { double } ctrlY - Y coordinate of the control point. The value is a floating point number.
     * @param { double } endX - X coordinate of the target point. The value is a floating point number.
     * @param { double } endY - Y coordinate of the target point. The value is a floating point number.
     * @param { double } weight - Weight of the curve, which determines its shape. The larger the value,
     * the closer of the curve to the control point. If the value is less than or equal to 0,
     * this API has the same effect as lineTo. If the value is 1, it has the same effect as quadTo. The value is a floating point number.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    native conicTo(ctrlX: double, ctrlY: double, endX: double, endY: double, weight: double): void;

    /**
     * Draws a cubic Bezier curve from the last point of this path to the target point. If the path is empty, the start point (0, 0) is used.
     * @param { double } ctrlX1 - X coordinate of the first control point. The value is a floating point number.
     * @param { double } ctrlY1 - Y coordinate of the first control point. The value is a floating point number.
     * @param { double } ctrlX2 - X coordinate of the second control point. The value is a floating point number.
     * @param { double } ctrlY2 - Y coordinate of the second control point. The value is a floating point number.
     * @param { double } endX - X coordinate of the target point. The value is a floating point number.
     * @param { double } endY - Y coordinate of the target point. The value is a floating point number.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    native cubicTo(ctrlX1: double, ctrlY1: double, ctrlX2: double, ctrlY2: double, endX: double, endY: double): void;

    /**
     * Sets the start position relative to the last point of this path. If the path is empty, the start point (0, 0) is used.
     * @param { double } dx - X offset of the start point relative to the last point. A positive number indicates a rightward shift from the last point,
     * and a negative number indicates a leftward shift from the last point. The value is a floating point number.
     * @param { double } dy - Y offset of the start point relative to the last point. A positive number indicates an upward shift from the last point,
     * and a negative number indicates a downward shift from the last point. The value is a floating point number.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    native rMoveTo(dx: double, dy: double): void;

    /**
     * Draws a line segment from the last point of this path to a point relative to the last point. If the path is empty, the start point (0, 0) is used.
     * @param { double } dx - X offset of the target point relative to the last point. A positive number indicates a rightward shift from the last point,
     * and a negative number indicates a leftward shift from the last point. The value is a floating point number.
     * @param { double } dy - Y offset of the target point relative to the last point. A positive number indicates an upward shift from the last point,
     * and a negative number indicates a downward shift from the last point. The value is a floating point number.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    native rLineTo(dx: double, dy: double): void;

    /**
     * Draws a quadratic Bezier curve from the last point of this path to a point relative to the last point.
     * If the path is empty, the start point (0, 0) is used.
     * @param { double } dx1 - X offset of the control point relative to the last point. A positive number indicates a rightward shift from the last point,
     * and a negative number indicates a leftward shift from the last point. The value is a floating point number.
     * @param { double } dy1 - Y offset of the control point relative to the last point. A positive number indicates an upward shift from the last point,
     * and a negative number indicates a downward shift from the last point. The value is a floating point number.
     * @param { double } dx2 - X offset of the target point relative to the last point. A positive number indicates a rightward shift from the last point,
     * and a negative number indicates a leftward shift from the last point. The value is a floating point number.
     * @param { double } dy2 - Y offset of the target point relative to the last point. A positive number indicates an upward shift from the last point,
     * and a negative number indicates a downward shift from the last point. The value is a floating point number.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    native rQuadTo(dx1: double, dy1: double, dx2: double, dy2: double): void;

    /**
     * Draws a conic curve from the last point of this path to a point relative to the last point. If the path is empty, the start point (0, 0) is used.
     * @param { double } ctrlX - X offset of the control point relative to the last point. A positive number indicates a rightward shift from the last point,
     * and a negative number indicates a leftward shift from the last point. The value is a floating point number.
     * @param { double } ctrlY - Y offset of the control point relative to the last point. A positive number indicates an upward shift from the last point,
     * and a negative number indicates a downward shift from the last point. The value is a floating point number.
     * @param { double } endX - X offset of the target point relative to the last point. A positive number indicates a rightward shift from the last point,
     * and a negative number indicates a leftward shift from the last point. The value is a floating point number.
     * @param { double } endY - Y offset of the target point relative to the last point. A positive number indicates an upward shift from the last point,
     * and a negative number indicates a downward shift from the last point. The value is a floating point number.
     * @param { double } weight - Weight of the curve, which determines its shape. The larger the value, the closer of the curve to the control point.
     * If the value is less than or equal to 0, this API is equivalent to rLineTo, that is, adding a line segment from the last point of the path
     * to the target point. If the value is 1, this API is equivalent to rQuadTo. The value is a floating point number.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    native rConicTo(ctrlX: double, ctrlY: double, endX: double, endY: double, weight: double): void;
  
      /**
     * Draws a cubic Bezier curve from the last point of this path to a point relative to the last point.
     * If the path is empty, the start point (0, 0) is used.
     * @param { double } ctrlX1 - X offset of the first control point relative to the last point. A positive number indicates a rightward shift
     * from the last point, and a negative number indicates a leftward shift from the last point. The value is a floating point number.
     * @param { double } ctrlY1 - Y offset of the first control point relative to the last point. A positive number indicates an upward shift
     * from the last point, and a negative number indicates a downward shift from the last point. The value is a floating point number.
     * @param { double } ctrlX2 - X offset of the second control point relative to the last point. A positive number indicates a rightward shift
     * from the last point, and a negative number indicates a leftward shift from the last point. The value is a floating point number.
     * @param { double } ctrlY2 - Y offset of the second control point relative to the last point. A positive number indicates an upward shift
     * from the last point, and a negative number indicates a downward shift from the last point. The value is a floating point number.
     * @param { double } endX - X offset of the target point relative to the last point. A positive number indicates a rightward shift
     * from the last point, and a negative number indicates a leftward shift from the last point. The value is a floating point number.
     * @param { double } endY - Y offset of the target point relative to the last point. A positive number indicates an upward shift
     * from the last point, and a negative number indicates a downward shift from the last point. The value is a floating point number.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    native rCubicTo(ctrlX1: double, ctrlY1: double, ctrlX2: double, ctrlY2: double, endX: double, endY: double): void;

    /**
     * Adds a polygon to this path.
     * @param { Array<common2D.Point> } points - Array that holds the vertex coordinates of the polygon.
     * @param { boolean } close - Whether to close the path, that is, whether to add a line segment from the start point
     * to the end point of the path. The value true means to close the path, and false means the opposite.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    native addPolygon(points: Array<common2D.Point>, close: boolean): void;

    /**
     * Combines this path with the passed-in path based on the specified operation mode.
     * @param { Path } path - Path object, which will be combined with the current path.
     * @param { PathOp } pathOp - Operation mode.
     * @returns { boolean } boolean - Result of the path combination result. The value true means that the path combination is successful,
     * and false means the opposite.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types; 3. Parameter verification failed.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    native op(path: Path, pathOp: PathOp): boolean;

    /**
     * Adds an arc to this path.
     *
     * When startAngle and sweepAngle meet the following conditions, an oval instead of an arc is added:
     *
     * The result of startAngle modulo 90 is close to 0.
     * The value of sweepAngle is not in the range of (-360, 360).
     *
     * In other cases, this API adds an arc by applying the result of sweepAngle modulo 360 to the path.
     * @param { common2D.Rect } rect - Rectangular boundary that encapsulates the oval including the arc.
     * @param { double } startAngle - Start angle of the arc, in degrees. The value 0 indicates the positive direction of the X axis.
     * The value is a floating point number.
     * @param { double } sweepAngle - Angle to sweep, in degrees. A positive number indicates a clockwise sweep,
     * and a negative number indicates a counterclockwise sweep. The value is a floating point number.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    native addArc(rect: common2D.Rect, startAngle: double, sweepAngle: double): void;

    /**
     * Adds a circle to this path in the specified direction. The start point of the circle is (x + radius, y).
     * @param { double } x - X coordinate of the center of the circle. The value is a floating point number.
     * @param { double } y - Y coordinate of the center of the circle. The value is a floating point number.
     * @param { double } radius - Radius of the circle. The value is a floating point number.
     * If the value is less than or equal to 0, there is no effect.
     * @param { PathDirection } pathDirection - Direction of the path. The default direction is clockwise.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types; 3. Parameter verification failed.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    native addCircle(x: double, y: double, radius: double, pathDirection?: PathDirection): void;

    /**
     * Adds the inscribed ellipse of a rectangle to this path in the specified direction.
     * @param { common2D.Rect } rect - Rectangular boundary of the oval.
     * @param { int } start - Start point of the oval, where 0, 1, 2, and 3 correspond to the upper, right, lower, and left points, respectively.
     * The value is an integer greater than or equal to 0. If the value is greater than or equal to 4, the remainder of 4 is used.
     * @param { PathDirection } pathDirection - Direction of the path. The default direction is clockwise.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types; 3. Parameter verification failed.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    native addOval(rect: common2D.Rect, start: int, pathDirection?: PathDirection): void;

    /**
     * Adds a rectangle to this path in the specified direction. The start point is the upper left corner of the rectangle.
     * @param { common2D.Rect } rect - Rectangle.
     * @param { PathDirection } pathDirection - Direction of the path. The default direction is clockwise.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types; 3. Parameter verification failed.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    native addRect(rect: common2D.Rect, pathDirection?: PathDirection): void;

    /**
     * Adds a rounded rectangle to this path in the specified direction. When the path direction is clockwise,
     * the start point is at the intersection of the rounded rectangle's left boundary and its lower left corner.
     * When the path direction is counterclockwise, the start point is at the intersection point
     * between the left boundary and the upper left corner.
     * @param { RoundRect } roundRect - Rounded rectangle.
     * @param { PathDirection } pathDirection - The default value is CLOCKWISE.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types; 3. Parameter verification failed.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    native addRoundRect(roundRect: RoundRect, pathDirection?: PathDirection): void;

    /**
     * Transforms the points in a path by a matrix and stores the resulting path in the current Path object.
     * @param { Path } path - Source Path object.
     * @param { Matrix | null } matrix - Matrix object. The default value is an identity matrix.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    native addPath(path: Path, matrix?: Matrix | null): void;

    /**
     * Transforms the points in this path by a matrix.
     * @param { Matrix } matrix - Matrix object.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    native transform(matrix: Matrix): void;

    /**
     * Checks whether a coordinate point is included in this path. For details, see PathFillType.
     * @param { double } x - X coordinate. The value is a floating point number.
     * @param { double } y - Y coordinate. The value is a floating point number.
     * @returns { boolean } Check result. The value true means that the coordinate point is included in the path, and false means the opposite.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    native contains(x: double, y: double): boolean;

    /**
     * Sets the fill type of this path. The fill type determines how "inside" of the path is drawn.
     * For example, when the fill type Winding is used, "inside" of the path is determined by a non-zero sum of signed edge crossings.
     * When EvenOdd is used, "inside" of the path is determined by an odd number of edge crossings.
     * @param { PathFillType } pathFillType - Fill type of the path.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types; 3. Parameter verification failed.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    native setFillType(pathFillType: PathFillType): void;

    /**
     * Obtains the minimum bounding rectangle that encloses this path.
     * @returns { common2D.Rect | undefined } Rect object.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    native getBounds(): common2D.Rect | undefined;

    /**
     * Closes this path by adding a line segment from the start point to the last point of the path.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    native close(): void;

    /**
     * Offsets this path by specified distances along the X axis and Y axis and stores the resulting path in the Path object returned.
     * @param { double } dx - X offset. A positive number indicates an offset towards the positive direction of the X axis,
     * and a negative number indicates an offset towards the negative direction of the X axis. The value is a floating point number.
     * @param { double } dy - Y offset. A positive number indicates an offset towards the positive direction of the Y axis,
     * and a negative number indicates an offset towards the negative direction of the Y axis. The value is a floating point number.
     * @returns { Path } New path generated.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    native offset(dx: double, dy: double): Path | undefined;

    /**
     * Resets the path data.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 11
     */
    native reset(): void;

    /**
     * Obtains the path length.
     * @param { boolean } forceClosed - Whether the path is measured as a closed path.
     * The value true means that the path is considered closed during measurement,
     * and false means that the path is measured based on the actual closed status.
     * @returns { double } Return path length.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    native getLength(forceClosed: boolean): double;

    /**
     * Obtains the coordinates and tangent at a distance from the start point of this path.
     *
     * @param { boolean } forceClosed - Whether the path is measured as a closed path.
     * The value true means that the path is considered closed during measurement,
     * and false means that the path is measured based on the actual closed status.
     * @param { double } distance - Distance from the start point. If a negative number is passed in,
     *     the value 0 is used. If a value greater than the path length is passed in, the path length is used.
     *     The value is a floating point number.
     * @param { common2D.Point } position - Coordinates obtained.
     * @param { common2D.Point } tangent - Tangent obtained, where tangent.x and tangent.y represent the cosine
     * and sine of the tangent of the point, respectively.
     * @returns { boolean } - Check result. The value true means that they are obtained, and false means the opposite.
     * The values of position and tangent are not changed.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    native getPositionAndTangent(forceClosed: boolean, distance: double, position: common2D.Point, tangent: common2D.Point): boolean;

    /**
     * Extracts a segment of this path and appends it to a destination path.
     *
     * @param { boolean } forceClosed - Whether the path is measured as a closed path.
     *     The value true means that the path is considered closed
     *     during measurement, and false means that the path is measured based on the actual closed status.
     * @param { double } start - Distance from the start point of the path to the start point of the segment.
     *     If it is less than 0, it defaults to 0.
     *     If it is greater than or equal to stop, the extraction fails. The value is a floating point number.
     * @param { double } stop - Distance from the start point of the path to the end point of the segment.
     *     If it is less than or equal to start,
     *     the extraction fails. If it is greater than the path length, it defaults to the path length.
     *     The value is a floating point number.
     * @param { boolean } startWithMoveTo - Whether to execute moveto in
     *     the destination path to move to its start point.
     *     The value true means to move to the start point, and false means the opposite.
     * @param { Path } dst - Destination path. If the extraction succeeds,
     *     the segment is appended to the path. If the extraction fails, nothing changes.
     * @returns { boolean } - Extraction result.
     *     The value **true** means that the extraction is successful, and **false** means the opposite.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    native getSegment(forceClosed: boolean, start: double, stop: double, startWithMoveTo: boolean, dst: Path): boolean;

    /**
     * Checks whether a path is closed.
     * 
     * @returns { boolean } - Check result. The value true means that the path is closed, and false means the opposite.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    native isClosed(): boolean;

    /**
     * Obtains a transformation matrix at a specific position along the path, which represents the coordinates and orientation of that point.
     * 
     * @param { boolean } forceClosed - Whether the path is measured as a closed path.
     *     The value true means that the path is considered closed
     *     during measurement, and false means that the path is measured based on the actual closed status.
     * @param { double } distance - Distance from the start point.
     *     If a negative number is passed in, the value 0 is used.
     *     If a value greater than the path length is passed in, the path length is used.
     *     The value is a floating point number.
     * @param { Matrix } matrix - Matrix object used to store the matrix obtained.
     * @param { PathMeasureMatrixFlags } flags - Type of the matrix information obtained.
     * @returns { boolean } - Result indicating whether the transformation matrix is obtained.
     *     The value true means that the operation is successful, and false means the opposite.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: Mandatory parameters are left unspecified.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    native getMatrix(forceClosed: boolean, distance: double, matrix: Matrix, flags: PathMeasureMatrixFlags): boolean;

    /**
     * Parses the path represented by an SVG string.
     *
     * @param { string } str - String in SVG format, which is used to describe the path.
     * @returns { boolean } Result of the parsing operation.
     *     The value true means that the operation is successful, and false means the opposite.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: Mandatory parameters are left unspecified.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    native buildFromSvgString(str: string): boolean;

    /**
     * Obtains the operation iterator of this path.
     *
     * @returns { PathIterator | undefined } Indicates the pointer to an pathIterator object.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    native getPathIterator(): PathIterator | undefined;
  }

  /**
   * Enumerates the modes for drawing multiple points in an array.
   * @enum { int }
   * @syscap SystemCapability.Graphics.Drawing
   * @since 12
   */
  export enum PointMode {
    /**
     * Draws each point separately.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    POINTS = 0,

    /**
     * Draws every two points as a line segment.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    LINES = 1,

    /**
     * Draws an array of points as an open polygon.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    POLYGON = 2,
  }

  /**
   * Enumerates the filter modes.
   * @enum { int }
   * @syscap SystemCapability.Graphics.Drawing
   * @since 20
   */
  export enum FilterMode {

    /**
     * Nearest filter mode.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    FILTER_MODE_NEAREST = 0,

    /**
     * Linear filter mode.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    FILTER_MODE_LINEAR = 1,
  }

  /**
   * Enumerates the shadow drawing behaviors.
   * @enum { int }
   * @syscap SystemCapability.Graphics.Drawing
   * @since 12
   */
  export enum ShadowFlag {
    /**
     * None of the flags are enabled.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    NONE = 0,

    /**
     * The occluder is transparent.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    TRANSPARENT_OCCLUDER = 1,

    /**
     * Only the geometric shadow effect is used.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    GEOMETRIC_ONLY = 2,

    /**
     * All the flags are enabled.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    ALL = 3,
  }

  /**
   * Describes a region, which is used to describe the region where the shape can be drawn.
   * @syscap SystemCapability.Graphics.Drawing
   * @since 12
   */
  export class Region {
    static { loadLibrary("drawing_ani_core"); }
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

    constructor(left: int, top: int, right: int, bottom: int) {
      this.constructorNative(left, top, right, bottom);
      this.registerCleaner(this.nativeObj);
    };

    /**
     * Creates an empty region.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    native constructorNative(): void;

    /**
     * Creates a deep copy of the specified region object.
     * @param { Region } region - The region object to copy.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    native constructorNative(region: Region): void;

    /**
     * Creates a region with a rectangle.
     * @param { int } left - Indicates the left edge of the rectangle.
     * @param { int } top - Indicates the top edge of the rectangle.
     * @param { int } right - Indicates the right edge of the rectangle.
     * @param { int } bottom - Indicates the bottom edge of the rectangle.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    native constructorNative(left: int, top: int, right: int, bottom: int): void;

    static regionTransferStatic(input: Any): Object {
      let region: Region = new Region();
      return Region.regionTransferStaticNative(region, ESValue.wrap(input));
    }

    private static native regionTransferStaticNative(region: Region, input: ESValue): Object;

    static regionTransferDynamic(input: Object): Any {
      let region = input as Region;
      if (!region) {
        return null;
      }
      let addr = Region.getRegionAddr(region);
      if (addr == 0) {
        return null;
      }
      let module = ESValue.load("@ohos.graphics.drawing");
      let hasProperty: boolean = module.hasProperty("Region");
      if (!hasProperty) {
        return null;
      }
      let cls = module.getProperty("Region");
      hasProperty = cls.hasProperty("__createTransfer__");
      if (hasProperty) {
        let initFunc = cls.getProperty("__createTransfer__");
        let obj = initFunc.invoke(ESObject.wrap(addr));
        return obj.unwrap();
      }
      return null;
    }

    private static native getRegionAddr(input: Region): long;
  }

  /**
   * Implements a rounded rectangle.
   *
   * @syscap SystemCapability.Graphics.Drawing
   * @since 12
   */
  export class RoundRect {
    static { loadLibrary("drawing_ani_core"); }
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

    constructor(rect: common2D.Rect, xRadii: double, yRadii: double) {
      this.constructorNative(rect, xRadii, yRadii);
      this.registerCleaner(this.nativeObj);
    };

    /**
     * Creates a deep copy of the specified round rect object.
     * @param { RoundRect } roundRect - The round rect object to copy.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    native constructorNative(roundRect: RoundRect): void;

    /**
     * A constructor used to create a RoundRect object. A rounded rectangle is created when both xRadii and yRadii are greater than 0. Otherwise, only a rectangle is created.
     * @param { common2D.Rect } rect - Rectangle that encloses the rounded rectangle to create.
     * @param { double } xRadii - Radius of the rounded corner on the X axis. The value is a floating point number. A negative number is invalid.
     * @param { double } yRadii - Radius of the rounded corner on the Y axis. The value is a floating point number. A negative number is invalid.
     * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified;
     * <br>2. Incorrect parameter types.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 12
     */
    native constructorNative(rect: common2D.Rect, xRadii: double, yRadii: double): void;

    static roundRectTransferStatic(input: Any): Object {
      return RoundRect.roundRectTransferStaticNative(ESValue.wrap(input));
    }

    private static native roundRectTransferStaticNative(input: ESValue): Object;

    static roundRectTransferDynamic(input: Object): Any {
      let roundRect = input as RoundRect;
      if (!roundRect) {
        return null;
      }
      let addr = RoundRect.getRoundRectAddr(roundRect);
      if (addr == 0) {
        return null;
      }
      let module = ESValue.load("@ohos.graphics.drawing");
      let hasProperty: boolean = module.hasProperty("RoundRect");
      if (!hasProperty) {
        return null;
      }
      let cls = module.getProperty("RoundRect");
      hasProperty = cls.hasProperty("__createTransfer__");
      if (hasProperty) {
        let initFunc = cls.getProperty("__createTransfer__");
        let obj = initFunc.invoke(ESObject.wrap(addr));
        return obj.unwrap();
      }
      return null;
    }

    private static native getRoundRectAddr(input: RoundRect): long;
  }

  /**
   * Provides an interface to the drawing, and describe the arguments for a font.
   * @syscap SystemCapability.Graphics.Drawing
   * @since 20
   */
  export class TypefaceArguments {
    static { loadLibrary("drawing_ani_core"); }
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
     * @since 20
     */
    native constructorNative(): void;

    /**
     * Adds variation axis for the TypefaceArguments.
     * @param { string } axis  - Indicates the axis tag, which must contain four ASCII characters.
     * @param { double } value  - Indicates the value of the axis field.
     * @throws { BusinessError } 25900001 - Parameter error. Possible causes: Incorrect parameter range.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    native addVariation(axis: string, value: double): void;
  }

  /**
   * Enumerate rect types. Optional setting per rectangular grid entry to make it transparent,
   * or to fill the grid entry with a color. only used in Lattice.
   * @enum { int }
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
   * @enum { int }
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

  /**
   * Enumerates the corner positions of a rounded rectangle.
   *
   * @enum { int }
   * @syscap SystemCapability.Graphics.Drawing
   * @since 20
   */
  export enum CornerPos {
    /**
     * Top left corner of the rounded rectangle.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    TOP_LEFT_POS = 0,

    /**
     * Top right corner of the rounded rectangle.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    TOP_RIGHT_POS = 1,

    /**
     * Bottom right corner of the rounded rectangle.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    BOTTOM_RIGHT_POS = 2,

    /**
     * Bottom left corner of the rounded rectangle.
     * @syscap SystemCapability.Graphics.Drawing
     * @since 20
     */
    BOTTOM_LEFT_POS = 3
  }
}

export default drawing;
