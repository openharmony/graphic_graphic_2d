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
  console.log('Test effectKit start');
  const opts: image.InitializationOptions = {
    size: { width: 10, height: 10 },
    editable: true,
    pixelFormat: image.PixelMapFormat.RGBA_8888,
  };

  let pixelMap: image.PixelMap = image.createPixelMapSync(opts);
  if (pixelMap == undefined) {
    console.log('Create image.createPixelMapSync failed');
    console.log('Test effectKit end');
    return;
  }
  console.log('Create image.createPixelMapSync success');

  let headFilter: effectKit.Filter = effectKit.createEffect(pixelMap);
  if (headFilter == undefined) {
    console.log('Test createEffect failed');
  } else {
    console.log('Test createEffect success');
  }

  effectKit.createColorPicker(pixelMap).then((colorPicker: effectKit.ColorPicker) => {
    if (colorPicker != undefined) {
      console.log(`Test createColorPicker (Promise) success`);
      
      console.log('Testing getAverageColor');
      let avgColor = colorPicker.getAverageColor();
      if (avgColor !== undefined && avgColor !== null) {
        console.log(`Simple Test: getAverageColor success. Color: ${JSON.stringify(avgColor)}`);
      } else {
        console.log('Simple Test: getAverageColor returned undefined or null.');
      }

      console.log('Testing getMainColor');
      let mainColor = colorPicker.getMainColor();
      if (mainColor !== undefined && mainColor !== null) {
        console.log(`Simple Test: getMainColor success. Color: ${JSON.stringify(mainColor)}`);
      } else {
        console.log('Simple Test: getMainColor returned undefined or null.');
      }

      console.log('Testing getMainColorSync');
      let mainColorSync = colorPicker.getMainColorSync();
      if (mainColorSync !== undefined && mainColorSync !== null) {
        console.log(`Simple Test: getMainColorSync success. Color: ${JSON.stringify(mainColorSync)}`);
      } else {
        console.log('Simple Test: getMainColorSync returned undefined or null.');
      }
      
    }
  }).catch((ex: Object | null | undefined) => {
    console.log('Test createColorPicker (Promise) failed');
  });
  
  effectKit.createColorPicker(pixelMap, [0, 0, 5, 5]).then((colorPicker: effectKit.ColorPicker) => {
    if (colorPicker != undefined) {
      console.log(`Test createColorPicker (Promise, region) success`);
    }
  }).catch((ex: Object | null | undefined) => {
    console.log('Test createColorPicker (Promise, region) failed');
  });
  
  effectKit.createColorPicker(pixelMap, (error, colorPicker: effectKit.ColorPicker | undefined) => {
    if (error && error.code != 0) {
      console.log('Test createColorPicker (Callback) failed');
    } else if (colorPicker != undefined) {
      console.log('Test createColorPicker (Callback) success');
    } else {
      console.log('Test createColorPicker (Callback) failed (colorPicker = undefined)');
    }
  });

  effectKit.createColorPicker(pixelMap, [0, 0, 5, 5], (error, colorPicker: effectKit.ColorPicker | undefined) => {
    if (error && error.code != 0) {
      console.log('Test createColorPicker (Callback, region) failed');
    } else if (colorPicker != undefined) {
      console.log('Test createColorPicker (Callback, region) success');
    } else {
      console.log('Test createColorPicker (Callback, region) failed (colorPicker = undefined)');
    }
  });
  
  console.log('Test effectKit end');
}