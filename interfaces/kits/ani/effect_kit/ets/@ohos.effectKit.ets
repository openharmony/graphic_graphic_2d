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
// 导入完整异步请求包
import { AsyncCallback, Callback, BusinessError } from '@ohos.base';
import image from '@ohos.multimedia.image';

// RejectString
type RejectString = (e: BusinessError<string>) => void;

namespace effectKit {
  loadLibrary('effectKit_ani')

  // 增加TileMode枚举，后续方法中需要使用
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

  //新增Color接口
  export interface Color {
    alpha: number;
    blue: number;
    green: number;
    red: number;
  }

  export class ColorInternal implements Color {
    alpha: number;
    blue: number;
    green: number;
    red: number;
  }
  
  export interface Filter {
    /**
    * A blur effect is added to the image.
    * @param { number } radius - The degree of blur, the value is measured in pixels.
    * @returns { Filter } Filters for the current effect have been added.
    * @syscap SystemCapability.Multimedia.Image.Core
    * @since 9
    */
    /**
    * A blur effect is added to the image.
    * @param { number } radius - The degree of blur, the value is measured in pixels.
    * @returns { Filter } Filters for the current effect have been added.
    * @syscap SystemCapability.Multimedia.Image.Core
    * @form
    * @atomicservice
    * @since 12
    */
    blur(radius: number): Filter;

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
    
    // 增加五个方法
    /**
     * Adds the blur effect to the filter linked list, and returns the head node of the linked list.
     * @param { number } radius - Blur radius, in pixels. The blur effect is proportional to the configured value.
     *  A larger value indicates a more obvious effect.
     * @param { TileMode } tileMode - Tile mode of the shader effect. The blur effect of image edges is affected. Currently,
     *  only CPU rendering is supported. Therefore, the tile mode supports only DECAL.
     * @returns { Filter } Final image effect.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 14
     */
    blur(radius: number, tileMode: TileMode): Filter;

    /**
     * A Brightness effect is added to the image.
     * @param { number } bright - The degree of light and darkness,the value range is 0 to 1.
     * @returns { Filter } Filters for the current effect have been added.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 9
     */
    /**
     * A Brightness effect is added to the image.
     * @param { number } bright - The degree of light and darkness,the value range is 0 to 1.
     * @returns { Filter } Filters for the current effect have been added.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @form
     * @atomicservice
     * @since 12
     */
    /**
     * Adds the brightness effect to the filter linked list, and returns the head node of the linked list.
     * @param { number } bright - Brightness value, ranging from 0 to 1. When the value is 0, the image brightness remains unchanged.
     * @returns { Filter } Final image effect.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @form
     * @atomicservice
     * @since 14
     */
    brightness(bright: number): Filter;

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
     * @param { Array<number> } colorMatrix - A matrix of 5x4 size for create effect filter.
     * @returns { Filter } Filters for the current effect have been added.
     * @throws { BusinessError } 401 - Input parameter error.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @since 12
     */
    /**
     * Adds a custom effect to the filter linked list, and returns the head node of the linked list.
     *
     * @param { Array<number> } colorMatrix - Custom color matrix.
     * A 5 x 4 matrix can be created. The value range of the matrix element is [0, 1],
     * where 0 indicates that the color channel is not involved in the calculation,
     * and 1 indicates that the color channel is involved in the calculation and retains the original weight.
     * @returns { Filter } Final image effect.
     * @throws { BusinessError } 401 - Input parameter error.
     * @syscap SystemCapability.Multimedia.Image.Core
     * @crossplatform
     * @since 14
     */
    setColorMatrix(colorMatrix: Array<number>): Filter;

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

  // 删除重复colorpicker接口声明与实现

  export class FilterInternal implements Filter {
    private nativeObj: long = 0;

    constructor(context:long) {
		this.nativeObj = context;
    }

    public native blurNative(radius: number): Filter;

    public native grayscaleNative(): Filter;

    public native getEffectPixelMapNative(): image.PixelMap;

    // 增加五种方法native函数
    public native blurNative(radius: number, tileMode: TileMode): Filter;
    public native brightnessNative(bright: number): Filter;
    public native invertNative(): Filter;
    public native setColorMatrixNative(colorMatrix: Array<number>): Filter;
    public native getPixelMapNative(): image.PixelMap;

    public blur(radius: number): Filter {
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

    // 封装五种方法
    public blur(radius: number, tileMode: TileMode): Filter {
      return this.blurNative(radius, tileMode);
    }
    public brightness(bright: number): Filter {
      return this.brightnessNative(bright);
    }
    public invert(): Filter {
      return this.invertNative();
    }
    public setColorMatrix(colorMatrix: Array<number>): Filter {
      return this.setColorMatrixNative(colorMatrix);
    }
    public getPixelMap(): image.PixelMap {
      return this.getPixelMapNative();
    }
  }

  export native function createEffect(source: image.PixelMap): Filter;


  // 修改namespace下重载函数写法
  native function createColorPicker1(source: image.PixelMap): ColorPicker
  native function createColorPicker2(source: image.PixelMap, region: Array<number>): ColorPicker

  export function createColorPicker(source: image.PixelMap): Promise<ColorPicker> {
    return new Promise<ColorPicker>((resolve : (v: ColorPicker) => void, reject: RejectString) => {
      let cb = (): ColorPicker => { return createColorPicker1(source) };
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
      });
    });
  }

  export function createColorPicker(source: image.PixelMap, region: Array<number>): Promise<ColorPicker> {
    return new Promise<ColorPicker>((resolve : (v: ColorPicker) => void, reject: RejectString) => {
      let cb = (): ColorPicker => { return createColorPicker2(source, region) };
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
      });
    });
  }

 
}

export default effectKit;