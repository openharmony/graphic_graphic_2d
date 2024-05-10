/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

import window from '@ohos.window';
import image from '@ohos.multimedia.image';
import { BusinessError } from '@ohos.base';
import fs from '@ohos.file.fs'

const TAG = '[DrawingTest]';
let MyWindow : window.Window | undefined = undefined;

export class Global{
  static setWindow(myWindow: window.Window) {
    MyWindow = myWindow;
  }

  static sleep(ms) {
    return new Promise(resolve=>setTimeout(resolve, ms));
  }

  static async windowSnapShort(dir: string, fileName: string) {
    if (MyWindow == null || MyWindow == undefined) {
      console.log(TAG, 'MyWindow is invalid');
      return;
    }

    await this.windowDisableSystemBar();
    await this.sleep(1000);

    await MyWindow.snapshot().then((pixelMap: image.PixelMap) => {
      this.savePixmap(dir, fileName, pixelMap);
      pixelMap.release(); // PixelMap使用完后及时释放内存
    }).catch((err: BusinessError) => {
      console.error(TAG, 'Failed to snapshot window. Cause:' + JSON.stringify(err));
    });
    this.windowEnableSystemBar();
  }

  static async savePixmap(dir: string, fileName: string, pixelMap: image.PixelMap) {
      console.info(TAG, 'savePixmap Pixel bytes number: ' + pixelMap.getPixelBytesNumber());
      const path : string = dir + "/" + fileName + ".png";
      let file = fs.openSync(path, fs.OpenMode.CREATE | fs.OpenMode.READ_WRITE);
      const imagePackerApi: image.ImagePacker = image.createImagePacker();
      let packOpts : image.PackingOption = { format:"image/png", quality:100};
      imagePackerApi.packToFile(pixelMap, file.fd, packOpts).then(() => {
        console.log(TAG, 'Successfully packaged images');
      }).catch((error : BusinessError) => {
        console.log(TAG, 'Failed to pack the image. And the error is: ' + error);
      })
  }

  static windowEnableSystemBar() {
    if (MyWindow == null || MyWindow == undefined) {
      console.log(TAG, 'MyWindow is invalid');
      return;
    }

    MyWindow.setWindowSystemBarEnable(['navigation', 'status']).then(()=>{
      console.log(TAG, 'setWindowColorSpace ', 'navigation', 'status')
    })
  }

  static async windowDisableSystemBar() {
    if (MyWindow == null || MyWindow == undefined) {
      console.log(TAG, 'MyWindow is invalid');
      return;
    }

    await MyWindow.setWindowSystemBarEnable(['navigation']).then(()=>{
      console.log(TAG, 'setWindowColorSpace ', 'navigation')
    })
  }
}
