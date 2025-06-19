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

import effectKit from './@ohos.effectKit';
import image from '@ohos.multimedia.image';


function main() {
  console.println('Test effectKit start');
  const opts: image.InitializationOptions = {
    size: { width: 480, height: 360 },
    editable: true,
    pixelFormat: image.PixelMapFormat.BGRA_8888,
  };

  // 删除重复声明
  let pixelMap: image.PixelMap = image.createPixelMapSync(opts);
  if (pixelMap == undefined) {
    console.println('Create image.createPixelMapSync failed');
  }else{
    console.println('Create image.createPixelMapSync success');
  }

  let headFilter: effectKit.Filter = effectKit.createEffect(pixelMap);
  if (headFilter == undefined) {
    console.println('Test createEffect failed');
  }else{
    console.println('Test createEffect success');
  }

  let blurFilter: effectKit.Filter = headFilter.blur(10);
  if (blurFilter == undefined) {
    console.println('Test blur failed');
  }else{
    console.println('Test blur success');
  }

  blurFilter.getEffectPixelMap().then((pixelMap: image.PixelMap) => {
    if (pixelMap != undefined) {
      console.println(`Test getEffectPixelMap success, getPixelBytesNumber = ${pixelMap.getPixelBytesNumber()}`);
    }
  }).catch((ex: Object | null | undefined) => {
    console.error('Test getEffectPixelMap failed');
    console.println('Test getEffectPixelMap failed');
  });

  let blurResult: effectKit.Filter = headFilter.blur(0.5, effectKit.TileMode.CLAMP);
  if (blurResult == undefined) {
    console.println('Test blur failed');
  } else {
    console.println('Test blur success');
  }

  let brightnessResult: effectKit.Filter = headFilter.brightness(0.5);
  if (brightnessResult == undefined) {
    console.println('Test brightness failed');
  } else {
    console.println('Test brightness success');
  }
  
  let invertResult: effectKit.Filter = headFilter.invert();
  if (invertResult == undefined) {
    console.println('Test invert failed');
  } else {
    console.println('Test invert success');
  }
  
  let setColorMatrixResult: effectKit.Filter = headFilter.setColorMatrix([1,1,1,1,1, 1,1,1,1,1, 1,1,1,1,1, 1,1,1,1,1]);
  if (setColorMatrixResult == undefined) {
    console.println('setColorMatrix failed');
  } else {
    console.println('setColorMatrix success');
  }

  let getPixelMapResult: image.PixelMap = headFilter.getPixelMap();
  if (getPixelMapResult == undefined) {
    console.println('getPixelMap failed');
  } else {
    console.println('getPixelMap success');
  }

  console.println('Test effectKit end');
}