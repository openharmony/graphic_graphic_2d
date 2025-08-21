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

import effectKit from '@ohos.effectKit';
import image from '@ohos.multimedia.image';


function main() {
  console.log('Test effectKit start');
  const opts: image.InitializationOptions = {
    size: { width: 480, height: 360 },
    editable: true,
    pixelFormat: image.PixelMapFormat.BGRA_8888,
  };
  let pixelMap: image.PixelMap = image.createPixelMapSync(opts);
  if (pixelMap == undefined) {
    console.log('Create image.createPixelMapSync failed');
    return;
  }
  console.log('Create image.createPixelMapSync success');
  let headFilter: effectKit.Filter = effectKit.createEffect(pixelMap);
  if (headFilter == undefined) {
    console.log('Test createEffect failed');
    return;
  }
  console.log('Test createEffect success');
  let blurFilter: effectKit.Filter = headFilter.blur(10);
  if (blurFilter == undefined) {
    console.log('Test blur failed');
    return;
  }
  console.log('Test blur success');
  blurFilter.getEffectPixelMap().then((pixelMap: image.PixelMap) => {
    if (pixelMap != undefined) {
      console.log('Test getEffectPixelMap success, getPixelBytesNumber = ', pixelMap.getPixelBytesNumber());
    }
  }).catch((ex: Object | null | undefined) => {
    console.error('Test getEffectPixelMap failed');
  });
  console.log('Test effectKit end');
}