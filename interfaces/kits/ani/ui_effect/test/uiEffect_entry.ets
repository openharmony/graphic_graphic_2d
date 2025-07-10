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
 
function main() {
  console.log('Test uiEffect start');
  let visualEffect: uiEffect.VisualEffect = uiEffect.createEffect();
  if (visualEffect == undefined) {
    console.log('createEffect failed');
  } else {
    console.log("createEffect SUCCEED");
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
    console.log("createBrightnessBlender failed");
  } else {
    console.log("createBrightnessBlender SUCCEED, value is correct");
  }
  let result: uiEffect.VisualEffect = visualEffect.backgroundColorBlender(brightnessBlender);
  if (result == undefined) {
    console.log('backgroundColorBlender failed');
  } else {
    console.log("backgroundColorBlender SUCCEED");
  }
  console.log('Test uiEffect end');
}