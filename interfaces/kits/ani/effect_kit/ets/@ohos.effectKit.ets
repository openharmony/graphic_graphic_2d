/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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
// Import the complete async request package
import { AsyncCallback, Callback, BusinessError } from '@ohos.base';
import image from '@ohos.multimedia.image';

// RejectString
type RejectString = (e: BusinessError<string>) => void;

namespace effectKit {
  loadLibrary('effectKit_ani')

  export enum TileMode {
    CLAMP,
    REPEAT,
    MIRROR,
    DECAL
  }

  /**
   * The Filter of FilterChain.
   * @typedef Filter
   * @syscap SystemCapability.Multimedia.Image.Core
   * @since 9
   */
  /**
   * The Filter of FilterChain.
   * @typedef Filter
   * @syscap SystemCapability.Multimedia.Image.Core
   * @form
   * @atomicservice
   * @since 12
   */

  export interface Color {
    red: int;
    green: int;
    blue: int;
    alpha: int;
  }

  export class ColorInternal implements Color {
    red: int;
    green: int;
    blue: int;
    alpha: int;
  }
  
  export interface Filter {
    /**
    * A blur effect is added to the image.
    * @param { double } radius - The degree of blur, the value is measured in pixels.
    * @returns { Filter } Filters for the current effect have been added.
    * @syscap SystemCapability.Multimedia.Image.Core
    * @since 9
    */
    /**
    * A blur effect is added to the image.
    * @param { double } radius - The degree of blur, the value is measured in pixels.
    * @returns { Filter } Filters for the current effect have been added.
    * @syscap SystemCapability.Multimedia.Image.Core
    * @form
    * @atomicservice
    * @since 12
    */
    blur(radius: double): Filter;

    /**
    * A Grayscale effect is added to the image.
    * @returns { Filter } Filters for the current effect have been added.
    * @syscap SystemCapability.Multimedia.Image.Core
    * @since 9
    */
    /**
    * A Grayscale effect is added to the image.
    * @returns { Filter } Filters for the current effect have been added.
    * @syscap SystemCapability.Multimedia.Image.Core
    * @form
    * @atomicservice
    * @since 12
    */
    /**
    * A Grayscale effect is added to the image.
    * @returns { Filter } Filters for the current effect have been added.
    * @syscap SystemCapability.Multimedia.Image.Core
    * @form
    * @atomicservice
    * @since 14
    */
    grayscale(): Filter;

    /**
    * Gets the PixelMap where all filter effects have been added to the image.
    * @returns { Promise<image.PixelMap> } - returns the PixelMap generated.
    * @syscap SystemCapability.Multimedia.Image.Core
    * @since 11
    */
    /**
    * Gets the PixelMap where all filter effects have been added to the image.
    * @returns { Promise<image.PixelMap> } - returns the PixelMap generated.
    * @syscap SystemCapability.Multimedia.Image.Core
    * @form
    * @atomicservice
    * @since 12
    */
    getEffectPixelMap(): Promise<image.PixelMap>;

    /**
    * Gets the PixelMap where all filter effects have been added to the image.
    * @param { boolean } useCpuRender - The value indicates whether to use CPU rendering.
    * @returns { Promise<image.PixelMap> } - returns the PixelMap generated.
    * @syscap SystemCapability.Multimedia.Image.Core
    * @since 11
    */
    /**
    * Gets the PixelMap where all filter effects have been added to the image.
    * @param { boolean } useCpuRender - The value indicates whether to use CPU rendering.
    * @returns { Promise<image.PixelMap> } - returns the PixelMap generated.
    * @syscap SystemCapability.Multimedia.Image.Core
    * @form
    * @atomicservice
    * @since 12
    */
    getEffectPixelMap(useCpuRender: boolean): Promise<image.PixelMap>;
    
    /**
     * Adds the blur effect to the filter linked list, and returns the head node of the linked list.
     * @param { double } radius - Blur radius, in pixels. The blur effect is proportional to the configured value.
     * A larger value indicates a more obvious effect.
     * @param { TileMode } tileMode - Tile mode of the shader effect. The blur effect of image edges is affected. Currently,
     * only CPU rendering is supported. Therefore, the tile mode supports only DECAL.
     * @returns { Filter } Final image effect.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 14
     */
    blur(radius: double, tileMode: TileMode): Filter;

    /**
     * A Brightness effect is added to the image.
     * @param { double } bright - The degree of light and darkness,the value range is 0 to 1.
     * @returns { Filter } Filters for the current effect have been added.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 9
     */
    /**
     * A Brightness effect is added to the image.
     * @param { double } bright - The degree of light and darkness,the value range is 0 to 1.
     * @returns { Filter } Filters for the current effect have been added.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @form
     * @atomicservice
     * @since 12
     */
    /**
     * Adds the brightness effect to the filter linked list, and returns the head node of the linked list.
     * @param { double } bright - Brightness value, ranging from 0 to 1. When the value is 0, the image brightness remains unchanged.
     * @returns { Filter } Final image effect.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @form
     * @atomicservice
     * @since 14
     */
    brightness(bright: double): Filter;

    /**
     * A invert effect is added to the image.
     * @returns { Filter } Filters for the current effect have been added.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 12
     */
    /**
     * Adds the inversion effect to the filter linked list, and returns the head node of the linked list.
     * @returns { Filter } Final image effect.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 14
     */
    invert(): Filter;

    /**
     * A custom effect is added to the image.
     *
     * @param { Array<double> } colorMatrix - A matrix of 5x4 size for create effect filter.
     * @returns { Filter } Filters for the current effect have been added.
     * @throws { BusinessError } 401 - Input parameter error.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 12
     */
    /**
     * Adds a custom effect to the filter linked list, and returns the head node of the linked list.
     *
     * @param { Array<double> } colorMatrix - Custom color matrix.
     * A 5 x 4 matrix can be created. The value range of the matrix element is [0, 1],
     * where 0 indicates that the color channel is not involved in the calculation,
     * and 1 indicates that the color channel is involved in the calculation and retains the original weight.
     * @returns { Filter } Final image effect.
     * @throws { BusinessError } 401 - Input parameter error.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 14
     */
    setColorMatrix(colorMatrix: Array<double>): Filter;

    /**
     * Adds the outline and shadow effects to the effect chain and returns the head node of the chain. This effect
     * extracts the contour of non-transparent areas in the image to generate an outline, and generates a projection
     * based on the shadow configuration.
     *
     * > **NOTE**
     * >
     * > The input image should be a cutout image containing only a single subject with a clear contour, where the
     * > subject area is fully opaque and the background area is fully transparent. Input images that do not meet
     * > this standard may produce abnormal effects.
     *
     * @param { double } outlineWidth - Outline width, in px. The subject extends outward with a white edge of the
     * width specified by outlineWidth, which is automatically truncated beyond the image range. Value range:
     * [0, +∞). Values outside the value range are clamped to the boundary value.
     * @param { ShadowOptions } [shadowOptions] - Optional. Shadow configuration parameters. When not specified,
     * the default shadow configuration is used.
     * @returns { Filter } Returns the Filter instance with the added effects, for further adding effects or
     * obtaining the processed image.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @systemapi
     * @stagemodelonly
     * @since 26.1.0 dynamic&static
     */
    outline(outlineWidth: double, shadowOptions?: ShadowOptions): Filter;

    /**
     * Adds the halftone, outline, and shadow effects to the effect chain and returns the head node of the chain.
     * This effect divides the original image into a regular grid and generates halftone dots based on the sampled
     * color and its brightness at the center of each grid. Each grid has a white background, and the dots consist
     * of a black inner circle and a colored ring.
     *
     * > **NOTE**
     * >
     * > The input image should be a cutout image containing only a single subject with a clear contour, where the
     * > subject area is fully opaque and the background area is fully transparent. Input images that do not meet
     * > this standard may produce abnormal effects.
     *
     * @param { HalftoneParam } halftoneParam - Halftone configuration parameters.
     * @param { double } [outlineWidth] - Optional. Outline width, in px. The subject extends outward with a white
     * edge of the width specified by outlineWidth, which is automatically truncated beyond the image range.
     * Value range: [0, +∞). Values outside the value range are clamped to the boundary value. Default value: 0.0.
     * @param { ShadowOptions } [shadowOptions] - Optional. Shadow configuration parameters. When not specified,
     * the default shadow configuration is used.
     * @returns { Filter } Returns the Filter instance with the added effects, for further adding effects or
     * obtaining the processed image.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @systemapi
     * @stagemodelonly
     * @since 26.1.0 dynamic&static
     */
    halftone(halftoneParam: HalftoneParam, outlineWidth?: double, shadowOptions?: ShadowOptions): Filter;

    /**
     * Obtains image.PixelMap of the source image to which the filter linked list is added.
     * @returns { image.PixelMap } image.PixelMap.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 9
     * @deprecated since 11
     * @useinstead effectKit.Filter#getEffectPixelMap
     */
    getPixelMap(): image.PixelMap;

  }

  /**
   * Shadow configuration parameters.
   *
   * > **NOTE**
   * >
   * > Shadows beyond the image range are truncated.
   *
   * @syscap SystemCapability.Multimedia.Image.Core
   * @systemapi
   * @stagemodelonly
   * @since 26.1.0 dynamic&static
   */
  export interface ShadowOptions {
    /**
     * Optional. Shadow blur radius, in px. Value range: [0, +∞). Default value: 0. Values outside the value range
     * are clamped to the boundary value. When the value is 0, no shadow effect is applied. A larger value produces
     * a softer shadow edge.
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @systemapi
     * @stagemodelonly
     * @since 26.1.0 dynamic&static
     */
    radius?: double;
    /**
     * Optional. Light direction vector. Default value: (0, 0). Value range: (-∞, +∞). The positive X-axis direction
     * is to the right, and the positive Y-axis direction is downward. The light direction vector indicates the
     * direction of shadow offset. When the vector is (0, 0), the shadow does not shift for any shadowDistance value.
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @systemapi
     * @stagemodelonly
     * @since 26.1.0 dynamic&static
     */
    lightDirection?: Array<double>;
    /**
     * Optional. Shadow offset distance, in px. Value range: [0, +∞). Default value: 0. Values outside the value
     * range are clamped to the boundary value. This parameter determines the shadow offset distance, and together
     * with lightDirection, determines the shadow coverage area. When the value is 0, the shadow does not shift
     * for any lightDirection value.
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @systemapi
     * @stagemodelonly
     * @since 26.1.0 dynamic&static
     */
    shadowDistance?: double;
    /**
     * Optional. Shadow color. Default value: black {red:0, green:0, blue:0, alpha:26}. The shadow opacity can be
     * set through the Alpha channel.
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @systemapi
     * @stagemodelonly
     * @since 26.1.0 dynamic&static
     */
    color?: Array<double>;
  }

  /**
   * Halftone configuration parameters.
   *
   * @syscap SystemCapability.Multimedia.Image.Core
   * @systemapi
   * @stagemodelonly
   * @since 26.1.0 dynamic&static
   */
  export interface HalftoneParam {
    /**
     * Grid cell side length, i.e., the center distance between adjacent dots, in px. Value range: [3, +∞). Values
     * outside the value range are clamped to the boundary value. A larger value produces larger overall dot size
     * and sparser arrangement.
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @systemapi
     * @stagemodelonly
     * @since 26.1.0 dynamic&static
     */
    gridSize: double;
    /**
     * Optional. Ratio of the outer circle diameter upper limit to the grid side length. Value range: [0, 1].
     * Default value: 1. Values outside the value range are clamped to the boundary value. The outer circle
     * diameter is truncated by maxRadiusRatio * gridSize. When the value is 1, the maximum allowed outer circle
     * diameter equals the grid side length.
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @systemapi
     * @stagemodelonly
     * @since 26.1.0 dynamic&static
     */
    maxRadiusRatio?: double;
    /**
     * Optional. Influence coefficient of brightness on the outer circle radius. Value range: [0, +∞). Default
     * value: 0. Values outside the value range are clamped to the boundary value. The actual outer circle radius
     * is clamp(1 - lum * lumCoeff, 0, maxRadiusRatio) * gridSize / 2, where lum is the brightness of the sampled
     * color at the grid center. This value is obtained from sampling and does not need to be set. When other
     * parameters remain unchanged, a larger lumCoeff produces a stronger shrinking effect of brightness on the
     * outer circle radius.
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @systemapi
     * @stagemodelonly
     * @since 26.1.0 dynamic&static
     */
    lumCoeff?: double;
    /**
     * Optional. Colored ring width coefficient. Value range: [0, 1]. Default value: 0.5. Values outside the value
     * range are clamped to the boundary value. The set width is colorWidthRatio * gridSize / 2. The black inner
     * circle radius is the actual outer circle radius minus this width, and when the result is less than 0, it is
     * treated as 0. When the value is 0, there is no colored ring. When the set width is greater than or equal to
     * the outer circle radius, the effect is equivalent to the entire area within the outer circle radius being
     * colored.
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @systemapi
     * @stagemodelonly
     * @since 26.1.0 dynamic&static
     */
    colorWidthRatio?: double;
    /**
     * Optional. Saturation interpolation coefficient of the colored area, used to interpolate between the grayscale
     * color corresponding to the sampled color and the sampled color. Value range: [0, 1]. Default value: 1.
     * Values outside the value range are clamped to the boundary value. When the value is 0, the grayscale color
     * is used. When the value is 1, the sampled color is preserved.
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @systemapi
     * @stagemodelonly
     * @since 26.1.0 dynamic&static
     */
    saturation?: double;
    /**
     * Optional. Ratio of the edge transition band half-width to gridSize. Value range: [0, +∞). Default value: 0.
     * Values outside the value range are clamped to the boundary value. With the inner circle and outer circle
     * boundaries as centers, it extends smoothness * gridSize to both sides respectively, forming a transition
     * band with a total width of 2 * smoothness * gridSize. The inner circle edge is processed first, then the
     * outer circle edge. When the value is 0, there is no transition band and the edge has no smooth transition.
     * A larger value produces a wider transition band and softer edges.
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @systemapi
     * @stagemodelonly
     * @since 26.1.0 dynamic&static
     */
    smoothness?: double;
    /**
     * Optional. Rotation angle of the grid coordinates, with counterclockwise as the positive direction.
     * Unit: rad (radians). Used to adjust the dot arrangement direction. Value range: (-∞, +∞). Default value: 0.
     * When the value is 0, the grid does not rotate, and the dot arrangement direction is horizontal to the right.
     *
     * @syscap SystemCapability.Multimedia.Image.Core
     * @systemapi
     * @stagemodelonly
     * @since 26.1.0 dynamic&static
     */
    angle?: double;
  }


  export class FilterInternal implements Filter {
    static {
	  loadLibrary('effectKit_ani')
    }

    private nativeObj: long = 0;

    constructor(context:long) {
      this.nativeObj = context;
    }

    public native blurNative(radius: double): Filter;

    public native grayscaleNative(): Filter;

    public native getEffectPixelMapNative(useCpuRender: boolean): image.PixelMap;

    public native blurNative(radius: double, tileMode: TileMode): Filter;

    public native brightnessNative(bright: double): Filter;

    public native invertNative(): Filter;

    public native setColorMatrixNative(colorMatrix: Array<double>): Filter;

    public native getPixelMapNative(): image.PixelMap;

    public native outlineNative(outlineWidth: double, shadowOptions: ShadowOptions): Filter;

    public native halftoneNative(halftoneParam: HalftoneParam, outlineWidth: double,
      shadowOptions: ShadowOptions): Filter;

    private static native kitTransferStaticNative(input: ESValue): Object;

	private static native kitTransferDynamicNative(nativeObj: long): ESValue;

    public blur(radius: double): Filter {
      return this.blurNative(radius);
    }

    public grayscale(): Filter {
      return this.grayscaleNative();
    }

    public getEffectPixelMap(): Promise<image.PixelMap> {
      return new Promise<image.PixelMap>((resolve: (value: image.PixelMap) => void,
        reject: (error: BusinessError) => void) => {
        taskpool.execute((): image.PixelMap => {
          let pixelMap = this.getEffectPixelMapNative(false);
          return pixelMap;
        }).then((ret: Any) => {
          resolve(ret as image.PixelMap);
        }).catch((err: Any) => {
          reject(err as BusinessError);
        });
      });
    }

    public getEffectPixelMap(useCpuRender: boolean): Promise<image.PixelMap> {
      return new Promise<image.PixelMap>((resolve: (value: image.PixelMap) => void,
        reject: (error: BusinessError) => void) => {
        taskpool.execute((): image.PixelMap => {
          let pixelMap = this.getEffectPixelMapNative(useCpuRender);
          return pixelMap;
        }).then((ret: Any) => {
          resolve(ret as image.PixelMap);
        }).catch((err: Any) => {
          reject(err as BusinessError);
        });
      });
    }

    public blur(radius: double, tileMode: TileMode): Filter {
      return this.blurNative(radius, tileMode);
    }
    public brightness(bright: double): Filter {
      return this.brightnessNative(bright);
    }
    public invert(): Filter {
      return this.invertNative();
    }
    public setColorMatrix(colorMatrix: Array<double>): Filter {
      return this.setColorMatrixNative(colorMatrix);
    }
    public getPixelMap(): image.PixelMap {
      return this.getPixelMapNative();
    }
    public outline(outlineWidth: double, shadowOptions?: ShadowOptions): Filter {
      return this.outlineNative(outlineWidth, shadowOptions ?? ({} as ShadowOptions));
    }
    public halftone(halftoneParam: HalftoneParam, outlineWidth?: double, shadowOptions?: ShadowOptions): Filter {
      let width = outlineWidth !== undefined ? outlineWidth : 0.0;
      return this.halftoneNative(halftoneParam, width, shadowOptions ?? ({} as ShadowOptions));
    }
    public static kitTransferStatic(input: Any): Object {
	  return FilterInternal.kitTransferStaticNative(ESValue.wrap(input));
	}   
	public static kitTransferDynamic(input: Object): Any {
	  return FilterInternal.kitTransferDynamicNative((input as FilterInternal).nativeObj).unwrap();
	}
  }

  export native function createEffect(source: image.PixelMap): Filter;


  export interface ColorPicker {
    getMainColor(): Promise<Color>;
    getMainColorSync(): Color;
    getLargestProportionColor(): Color;
    getTopProportionColors(colorCount: int): Array<Color | null>;
    getHighestSaturationColor(): Color;
    getAverageColor(): Color;
    isBlackOrWhiteOrGrayColor(color: long): boolean;
    getMorandiShadowColor(): Color;
    getDeepenImmersionColor(): Color;
    getImmersiveBackgroundColor(): Color;
    getImmersiveForegroundColor(): Color;
    discriminatePictureLightDegree(): PictureLightDegree;
    getReverseColor(): Color;
  }

  export enum PictureLightDegree {
    UNKOWN_LIGHT_COLOR_DEGREE_PICTURE = 0,
    EXTREMELY_LIGHT_COLOR_PICTURE = 1,
    LIGHT_COLOR_PICTURE = 2,
    DARK_COLOR_PICTURE = 3,
    EXTREMELY_DARK_COLOR_PICTURE = 4,
    FLOWERY_PICTURE = 5,
    EXTREMELY_FLOWERY_PICTURE = 6,
  }

  export class ColorPickerInternal implements ColorPicker {
    static {
	  loadLibrary('effectKit_ani')
	}

    private nativeObj: long = 0;

    constructor(context:long) {
      this.nativeObj = context;
    }

    public native getMainColorSyncNative(): Color;
    public native getLargestProportionColorNative(): Color;
    public native getTopProportionColorsNative(colorCount: int): Array<Color | null>;
    public native getHighestSaturationColorNative(): Color;
    public native getAverageColorNative(): Color;
    public native isBlackOrWhiteOrGrayColorNative(color: long): boolean;
    public native getMorandiShadowColorNative(): Color;
    public native getDeepenImmersionColorNative(): Color;
    public native getImmersiveBackgroundColorNative(): Color;
    public native getImmersiveForegroundColorNative(): Color;
    public native discriminatePictureLightDegreeNative(): PictureLightDegree;
    public native getReverseColorNative(): Color;
    private static native kitTransferStaticNative(input: ESValue): Object;
	private static native kitTransferDynamicNative(nativeObj: long): ESValue;

    public getMainColor(): Promise<Color> {
      return new Promise<Color>((resolve: (value: Color) => void,
        reject: (error: BusinessError) => void) => {
        taskpool.execute((): Color => {
          let color = this.getMainColorSync();
          return color;
        }).then((ret: Any) => {
          resolve(ret as Color);
        }).catch((err: Any) => {
          reject(err as BusinessError);
        });
      });
    }
    public getMainColorSync(): Color {
      return this.getMainColorSyncNative();
    }
    public getLargestProportionColor(): Color {
      return this.getLargestProportionColorNative();
    }
    public getTopProportionColors(colorCount: int): Array<Color | null> {
      return this.getTopProportionColorsNative(colorCount);
    }
    public getHighestSaturationColor(): Color {
      return this.getHighestSaturationColorNative();
    }
    public getAverageColor(): Color {
      return this.getAverageColorNative();
    }
    public isBlackOrWhiteOrGrayColor(color: long): boolean {
      return this.isBlackOrWhiteOrGrayColorNative(color);
    }
    public getMorandiShadowColor(): Color {
      return this.getMorandiShadowColorNative();
    }
    public getDeepenImmersionColor(): Color {
      return this.getDeepenImmersionColorNative();
    }
    public getImmersiveBackgroundColor(): Color {
      return this.getImmersiveBackgroundColorNative();
    }
    public getImmersiveForegroundColor(): Color {
      return this.getImmersiveForegroundColorNative();
    }
    public discriminatePictureLightDegree(): PictureLightDegree {
      return this.discriminatePictureLightDegreeNative();
    }
    public getReverseColor(): Color {
      return this.getReverseColorNative();
    }
    public static kitTransferStatic(input: Any): Object {
	  return ColorPickerInternal.kitTransferStaticNative(ESValue.wrap(input));
	}
	public static kitTransferDynamic(input: Object): Any {
	  return ColorPickerInternal.kitTransferDynamicNative((input as ColorPickerInternal).nativeObj).unwrap();
	}
  }

  native function createColorPickerNormal(source: image.PixelMap): ColorPicker
  native function createColorPickerWithRegion(source: image.PixelMap, region: Array<double>): ColorPicker

  export function createColorPicker(source: image.PixelMap): Promise<ColorPicker> {
    return new Promise<ColorPicker>((resolve : (v: ColorPicker) => void, reject: RejectString) => {
      let cb = (): ColorPicker => {
        return createColorPickerNormal(source)
      };
      taskpool.execute(cb).then((ret: Any): void => {
        if (ret === null || ret === undefined) {
          let ret1: BusinessError<string> = {
            code: -1,
            data: "Operation failed",
            name: "",
            message: ""
          };
          reject(ret1);
        } else {
          let colorpicker = ret as ColorPicker;
          resolve(colorpicker);
        }
      }).catch((err: Any) => {
        reject(err as BusinessError<string>);   
      });
    });
  }

  export function createColorPicker(source: image.PixelMap, region: Array<double>): Promise<ColorPicker> {
    return new Promise<ColorPicker>((resolve : (v: ColorPicker) => void, reject: RejectString) => {
      let cb = (): ColorPicker => { return createColorPickerWithRegion(source, region) };
      taskpool.execute(cb).then((ret: Any): void => {
        if (ret === null || ret === undefined) {
          let ret1: BusinessError<string> = {
            code: -1,
            data: "Operation failed",
            name: "",
            message: ""
          };
          reject(ret1);
        } else {
          let colorpicker = ret as ColorPicker;
          resolve(colorpicker);
        }
      }).catch((err: Any) => {
        reject(err as BusinessError<string>);
      });
    });
  }

  export function createColorPicker(source: image.PixelMap, callback: AsyncCallback<ColorPicker | undefined>): void {
    try {
      const result = createColorPickerNormal(source);
      if (result) {
        callback(null, result);
      } else {
        const error: BusinessError = {
          code: -1,
          name: "EffectKitNativeError",
          message: "Failed to create ColorPicker. Native method returned null or undefined."
        };
        callback(error, undefined);
      }
    } catch (e) {
      const error: BusinessError = {
        code: (e as BusinessError).code || -1,
        name: (e as BusinessError).name || "EffectKitNativeError",
        message: (e as BusinessError).message || "An unexpected native error occurred."
      };
      callback(error, undefined);
    }

  }

  export function createColorPicker(source: image.PixelMap, region: Array<double>, callback: AsyncCallback<ColorPicker | undefined>): void {
    try {
      const result = createColorPickerWithRegion(source, region);
      if (result) {
        callback(null, result);
      } else {
        const error: BusinessError = {
          code: -1,
          name: "EffectKitNativeError",
          message: "Failed to create ColorPicker with region. Native method returned null or undefined."
        };
        callback(error, undefined);
      }
    } catch (e) {
      const error: BusinessError = {
        code: (e as BusinessError).code || -1,
        name: (e as BusinessError).name || "EffectKitNativeError",
        message: (e as BusinessError).message || "An unexpected native error occurred."
      };
      callback(error, undefined);
    }
  }
}

export default effectKit;