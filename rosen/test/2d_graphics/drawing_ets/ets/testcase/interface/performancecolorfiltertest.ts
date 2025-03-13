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

import drawing from '@ohos.graphics.drawing';
import { StyleType, TestBase } from '../../pages/testbase';
import { common2D } from '@kit.ArkGraphics2D';

export class PerformanceColorFilterCreateBlendModeColorFilter extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let color: common2D.Color = { alpha: 255, red: 168, green: 48, blue: 176 };
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      drawing.ColorFilter.createBlendModeColorFilter(color, 0);
    }
  }
}

export class PerformanceColorFilterCreateComposeColorFilter extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let filter1 = new drawing.ColorFilter();
    let filter2 = new drawing.ColorFilter();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      drawing.ColorFilter.createComposeColorFilter(filter1, filter2);
    }
  }
}


export class PerformanceColorFilterCreateLinearToSRGBGamma extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      drawing.ColorFilter.createLinearToSRGBGamma();
    }
  }
}

export class PerformanceColorFilterCreateSRGBGammaToLinear extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      drawing.ColorFilter.createSRGBGammaToLinear();
    }
  }
}

export class PerformanceColorFilterCreateLumaColorFilter extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      drawing.ColorFilter.createLumaColorFilter();
    }
  }
}

export class PerformanceColorFilterCreateMatrixColorFilter extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let matrix = [1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0];
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      drawing.ColorFilter.createMatrixColorFilter(matrix);
    }
  }
}

