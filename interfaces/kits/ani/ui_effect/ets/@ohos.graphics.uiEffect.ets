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
  cubicRate: double;
  quadraticRate: double;
  linearRate: double;
  degree: double;
  saturation: double;
  positiveCoefficient: [double, double, double];
  negativeCoefficient: [double, double, double];
  fraction: double;
}
namespace uiEffect {
  loadLibraryWithPermissionCheck('uiEffect_ani', "@ohos.graphics.uiEffect")
  export type Blender = BrightnessBlender | HdrBrightnessBlender;
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
    cubicRate: double;
    quadraticRate: double;
    linearRate: double;
    degree: double;
    saturation: double;
    positiveCoefficient: [double, double, double];
    negativeCoefficient: [double, double, double];
    fraction: double;
  }
  export interface HdrBrightnessBlender extends BrightnessBlender {}
  export class BrightnessBlenderInternal implements BrightnessBlender {
    private brightnessBlenderNativeObj: long = 0;
    constructor(context: long) {
      this.brightnessBlenderNativeObj = context;
    }
    cubicRate: double;
    quadraticRate: double;
    linearRate: double;
    degree: double;
    saturation: double;
    positiveCoefficient: [double, double, double];
    negativeCoefficient: [double, double, double];
    fraction: double;
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
  export interface Filter {
    pixelStretch(stretchSizes: Array<double>, tileMode: TileMode): Filter;
    blur(blurRadius: double): Filter;
    flyInFlyOutEffect(degree: double, flyMode: FlyMode ): Filter;
    waterRipple(progress: double, waveCount: int, x: double, y: double, rippleMode: WaterRippleMode): Filter;
    distort(distortionK: double): Filter;
  }
  export class FilterInternal implements Filter {
    private filterNativeObj: long = 0;
    private interface waterRipplePara {
      progress: double;
      waveCount: int;
      x: double;
      y: double;
      rippleMode: WaterRippleMode;
    }
    constructor(context: long) {
      this.filterNativeObj = context;
    }
    public native pixelStretchNative(stretchSizes: Array<double>, tileMode: TileMode): Filter;
    public native blurNative(blurRadius: double): Filter;
    public native flyInFlyOutEffectNative(degree: double, flyMode: FlyMode ): Filter;
    public native waterRippleNative(waterPara: waterRipplePara): Filter;
    public native distortNative(distortionK: double): Filter;

    public pixelStretch(stretchSizes: Array<double>, tileMode: TileMode): Filter {
      return this.pixelStretchNative(stretchSizes, tileMode);
    }
    public blur(blurRadius: double): Filter {
      return this.blurNative(blurRadius);
    }
    public flyInFlyOutEffect(degree: double, flyMode: FlyMode ): Filter{
      return this.flyInFlyOutEffectNative(degree, flyMode);
    }
    public waterRipple(progress: double, waveCount: int, x: double, y: double, rippleMode: WaterRippleMode): Filter{
      let waterPara: waterRipplePara = {
        progress: progress,
        waveCount: waveCount,
        x: x,
        y: y,
        rippleMode: rippleMode
      };
      return this.waterRippleNative(waterPara);
    }
    public distort(distortionK: double): Filter{
      return this.distortNative(distortionK);
    }
  }
  export native function createEffect(): VisualEffect;
  export native function createBrightnessBlender(param: BrightnessBlenderParam): BrightnessBlender;
  export native function createHdrBrightnessBlender(param: BrightnessBlenderParam): HdrBrightnessBlender;
  export native function createFilter(): Filter;
}

export default uiEffect;