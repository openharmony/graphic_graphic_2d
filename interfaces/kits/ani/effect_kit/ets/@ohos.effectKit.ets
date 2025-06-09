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

import { BusinessError } from '@ohos.base';
import image from '@ohos.multimedia.image';

namespace effectKit {
  loadLibrary('effectKit_ani')
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
  }

  export class FilterInternal implements Filter {
    private nativeObj: long = 0;

    constructor(context: long) {
      if (this.nativeObj == 0) {
        this.nativeObj = context;
      }
    }

    public native blurNative(radius: double): Filter;

    public native getEffectPixelMapNative(): image.PixelMap;

    public blur(radius: number): Filter {
      return this.blurNative(radius);
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
  }

  export native function createEffect(source: image.PixelMap): Filter;
}

export default effectKit;