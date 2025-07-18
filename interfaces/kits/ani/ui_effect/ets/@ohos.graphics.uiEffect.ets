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

export interface BrightnessBlenderParam {
  cubicRate: number;
  quadraticRate: number;
  linearRate: number;
  degree: number;
  saturation: number;
  positiveCoefficient: [number, number, number];
  negativeCoefficient: [number, number, number];
  fraction: number;
}
namespace uiEffect {
  loadLibrary('uiEffect_ani')
  export type Blender = BrightnessBlender | HdrBrightnessBlender;
  export interface BrightnessBlender {
    cubicRate: number;
    quadraticRate: number;
    linearRate: number;
    degree: number;
    saturation: number;
    positiveCoefficient: [number, number, number];
    negativeCoefficient: [number, number, number];
    fraction: number;
  }
  export interface HdrBrightnessBlender extends BrightnessBlender {}
  export class BrightnessBlenderInternal implements BrightnessBlender {
    private brightnessBlenderNativeObj: long = 0;
    constructor(context: long) {
      this.brightnessBlenderNativeObj = context;
    }
    cubicRate: number;
    quadraticRate: number;
    linearRate: number;
    degree: number;
    saturation: number;
    positiveCoefficient: [number, number, number];
    negativeCoefficient: [number, number, number];
    fraction: number;
  }
  export class HdrBrightnessBlenderInternal extends BrightnessBlenderInternal implements HdrBrightnessBlender {
    constructor(context: long) {
      super(context);
    }
  }
  export interface VisualEffect {
    backgroundColorBlender(blender: BrightnessBlender): VisualEffect;
  }
  export class VisualEffectInternal implements VisualEffect {
    private visualEffectNativeObj: long = 0;
    constructor(context: long) {
      this.visualEffectNativeObj = context;
    }
    public native backgroundColorBlenderNative(blender: BrightnessBlender): VisualEffect;
    public backgroundColorBlender(blender: BrightnessBlender): VisualEffect {
      return this.backgroundColorBlenderNative(blender);
    }
  }  
  export native function createEffect(): VisualEffect;
  export native function createBrightnessBlender(param: BrightnessBlenderParam): BrightnessBlender;
  export native function createHdrBrightnessBlender(param: BrightnessBlenderParam): HdrBrightnessBlender;
}

export default uiEffect;