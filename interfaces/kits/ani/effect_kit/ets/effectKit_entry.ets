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
    size: { width: 10, height: 10 },
    editable: true,
    pixelFormat: image.PixelMapFormat.RGBA_8888,
  };

  let pixelMap: image.PixelMap = image.createPixelMapSync(opts);
  if (pixelMap == undefined) {
    console.println('Create image.createPixelMapSync failed');
    console.println('Test effectKit end');
    return;
  }
  console.println('Create image.createPixelMapSync success');

  let headFilter: effectKit.Filter = effectKit.createEffect(pixelMap);
  if (headFilter == undefined) {
    console.println('Test createEffect failed');
  } else {
    console.println('Test createEffect success');
  }

  effectKit.createColorPicker(pixelMap).then((colorPicker: effectKit.ColorPicker) => {
    if (colorPicker != undefined) {
      console.println(`Test createColorPicker (Promise) success`);

      console.println('Testing getAverageColor');
      let avgColor = colorPicker.getAverageColor();
      if (avgColor !== undefined && avgColor !== null) {
        console.println(`Simple Test: getAverageColor success. Color: ${JSON.stringify(avgColor)}`);
      } else {
        console.error('Simple Test: getAverageColor returned undefined or null.');
        console.println('Simple Test: getAverageColor returned undefined or null.');
      }

      console.println('Testing getMainColor');
      let mainColor = colorPicker.getMainColor();
      if (mainColor !== undefined && mainColor !== null) {
        console.println(`Simple Test: getMainColor success. Color: ${JSON.stringify(mainColor)}`);
      } else {
        console.error('Simple Test: getMainColor returned undefined or null.');
        console.println('Simple Test: getMainColor returned undefined or null.');
      }

      console.println('Testing getMainColorSync');
      let mainColorSync = colorPicker.getMainColorSync();
      if (mainColorSync !== undefined && mainColorSync !== null) {
        console.println(`Simple Test: getMainColorSync success. Color: ${JSON.stringify(mainColorSync)}`);
      } else {
        console.error('Simple Test: getMainColorSync returned undefined or null.');
        console.println('Simple Test: getMainColorSync returned undefined or null.');
      }
    }
  }).catch((ex: Object | null | undefined) => {
    console.error('Test createColorPicker (Promise) failed');
    console.println('Test createColorPicker (Promise) failed');
  });

  console.println('Test effectKit end');
}