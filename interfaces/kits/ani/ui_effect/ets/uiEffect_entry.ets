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
 
import uiEffect from './@ohos.graphics.uiEffect';
 
function main() {
  console.println('Test uiEffect start');
  
  let visualEffect: uiEffect.VisualEffect = uiEffect.createEffect();
  if (visualEffect == undefined) {
    console.println('createEffect failed');
  } else {
    console.println("createEffect SUCCEED");
  }
  let brightnessBlender: uiEffect.BrightnessBlender = uiEffect.createBrightnessBlender({
    cubicRate: 0.5,
    quadraticRate: 0.5,
    linearRate: 0.5,
    degree: 0.5,
    saturation: 0.5,
    positiveCoefficient: [2.3, 4.5, 2.0],
    negativeCoefficient: [0.5, 2.0, 0.5],
    fraction: 0.5
  });
  if (brightnessBlender == undefined) {
    console.println("createBrightnessBlender failed");
  } else {
    console.println("createBrightnessBlender SUCCEED, value is correct");
  }
  let result: uiEffect.VisualEffect = visualEffect.backgroundColorBlender(brightnessBlender);
  if (result == undefined) {
    console.println('backgroundColorBlender failed');
  } else {
    console.println("backgroundColorBlender SUCCEED");
  }  
  let filter: uiEffect.Filter = uiEffect.createFilter();
  if (filter == undefined) {
    console.println('createFilter failed');
  } else {
    console.println('createFilter succ');
  }
  let blurResult: uiEffect.Filter = filter.blur(0.5);
  if (blurResult == undefined) {
    console.println('blur failed');
  } else {
    console.println('blur succ');
  }
  let pixelStretchResult: uiEffect.Filter = filter.pixelStretch([0.2, 0.2, 0.2, 0.2], uiEffect.TileMode.CLAMP);
  if (pixelStretchResult == undefined) {
    console.println('pixelStretch failed');
  } else {
    console.println('pixelStretch succ');
  }

  let flyResult: uiEffect.Filter = filter.flyInFlyOutEffect(0.5, uiEffect.FlyMode.TOP);
  if (flyResult == undefined) {
    console.println('fly failed');
  } else {
    console.println('fly succ');
  }
  let waterRippleResult: uiEffect.Filter = filter.waterRipple(0.2, 0.2, 0.2, 0.2, uiEffect.WaterRippleMode.SMALL2MEDIUM_RECV);
  if (waterRippleResult == undefined) {
    console.println('waterRipple failed');
  } else {
    console.println('waterRipple succ');
  }  
  let distortResult: uiEffect.Filter = filter.distort(0.5);
  if (distortResult == undefined) {
    console.println('distort failed');
  } else {
    console.println('distort succ');
  }  
  
  console.println('Test uiEffect end');
}