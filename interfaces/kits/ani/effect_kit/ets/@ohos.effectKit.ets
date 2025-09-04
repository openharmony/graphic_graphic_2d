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
     * Obtains image.PixelMap of the source image to which the filter linked list is added.
     * @returns { image.PixelMap } image.PixelMap.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 9
     * @deprecated since 11
     * @useinstead effectKit.Filter#getEffectPixelMap
     */
    getPixelMap(): image.PixelMap;

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

    public native getEffectPixelMapNative(): image.PixelMap;

    public native blurNative(radius: double, tileMode: TileMode): Filter;

    public native brightnessNative(bright: double): Filter;

    public native invertNative(): Filter;

    public native setColorMatrixNative(colorMatrix: Array<double>): Filter;

    public native getPixelMapNative(): image.PixelMap;

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
          let pixelMap = this.getEffectPixelMapNative();
          return pixelMap;
        }).then((ret: NullishType) => {
          resolve(ret as image.PixelMap);
        }).catch((err: NullishType) => {
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
    isBlackOrWhiteOrGrayColor(color: int): boolean;
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
    public native isBlackOrWhiteOrGrayColorNative(color: int): boolean;
    private static native kitTransferStaticNative(input: ESValue): Object;
	private static native kitTransferDynamicNative(nativeObj: long): ESValue;

    public getMainColor(): Promise<Color> {
      return new Promise<Color>((resolve: (value: Color) => void,
        reject: (error: BusinessError) => void) => {
        taskpool.execute((): Color => {
          let color = this.getMainColorSync();
          return color;
        }).then((ret: NullishType) => {
          resolve(ret as Color);
        }).catch((err: NullishType) => {
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
    public isBlackOrWhiteOrGrayColor(color: int): boolean {
      return this.isBlackOrWhiteOrGrayColorNative(color);
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
      taskpool.execute(cb).then((ret: NullishType): void => {
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
      }).catch((err: NullishType) => {
        reject(err as BusinessError<string>);   
      });
    });
  }

  export function createColorPicker(source: image.PixelMap, region: Array<double>): Promise<ColorPicker> {
    return new Promise<ColorPicker>((resolve : (v: ColorPicker) => void, reject: RejectString) => {
      let cb = (): ColorPicker => { return createColorPickerWithRegion(source, region) };
      taskpool.execute(cb).then((ret: NullishType): void => {
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
      }).catch((err: NullishType) => {
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