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
  export type Blender = BrightnessBlender;
  export enum TileMode {
    CLAMP,
    REPEAT,
    MIRROR,
    DECAL
  }

  export enum FlyMode {
    BOTTOM,
    TOP
  }

  export enum WaterRippleMode {
    SMALL2MEDIUM_RECV,
    SMALL2MEDIUM_SEND,
    SMALL2SMALL,
    MINI_RECV
  }

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
  export class BrightnessBlenderInternal implements BrightnessBlender {
    cubicRate: number;
    quadraticRate: number;
    linearRate: number;
    degree: number;
    saturation: number;
    positiveCoefficient: [number, number, number];
    negativeCoefficient: [number, number, number];
    fraction: number;
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
  export interface Filter {
    pixelStretch(stretchSizes: Array<number>, tileMode: TileMode): Filter;
    blur(blurRadius: number): Filter;
    flyInFlyOutEffect(degree: number, flyMode: FlyMode ): Filter;
    waterRipple(progress: number, waveCount: number, x: number, y: number, rippleMode: WaterRippleMode): Filter;
    distort(distortionK: number): Filter;
  }
  export class FilterInternal implements Filter {
    private filterNativeObj: long = 0;
    constructor(context: long) {
      this.filterNativeObj = context;
    }
    public native pixelStretchNative(stretchSizes: Array<number>, tileMode: TileMode): Filter;
    public native blurNative(blurRadius: number): Filter;
    public native flyInFlyOutEffectNative(degree: number, flyMode: FlyMode ): Filter;
    public native waterRippleNative(progress: number, waveCount: number, 
                                    x: number, y: number, 
                                    rippleMode: WaterRippleMode): Filter;
    public native distortNative(distortionK: number): Filter;

    public pixelStretch(stretchSizes: Array<number>, tileMode: TileMode): Filter {
      return this.pixelStretchNative(stretchSizes, tileMode);
    }
    public blur(blurRadius: number): Filter {
      return this.blurNative(blurRadius);
    }
    public flyInFlyOutEffect(degree: number, flyMode: FlyMode ): Filter{
      return this.flyInFlyOutEffectNative(degree, flyMode);
    }
    public waterRipple(progress: number, waveCount: number, x: number, y: number, rippleMode: WaterRippleMode): Filter{
      return this.waterRippleNative(progress, waveCount, x, y, rippleMode);
    }
    public distort(distortionK: number): Filter{
      return this.distortNative(distortionK);
    }
  }
  export native function createEffect(): VisualEffect;
  export native function createBrightnessBlender(param: BrightnessBlenderParam): BrightnessBlender;
  export native function createFilter(): Filter;
}

export default uiEffect;