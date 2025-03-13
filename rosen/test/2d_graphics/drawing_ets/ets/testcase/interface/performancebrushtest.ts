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
import common2D from '@ohos.graphics.common2D';
import { StyleType, TestBase } from '../../pages/testbase';

export class PerformanceBrushConstructor extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let brush = new drawing.Brush();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      brush.constructor();
    }
  }
}

export class PerformanceBrushSetColor extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let brush: drawing.Brush = new drawing.Brush();
    let color: common2D.Color = { alpha: 255, red: 1, green: 255, blue: 1 };
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      brush.setColor(color);
    }
  }
}

export class PerformanceBrushGetColor extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let brush: drawing.Brush = new drawing.Brush();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      brush.getColor();
    }
  }
}

export class PerformanceBrushSetAntiAlias extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let brush: drawing.Brush = new drawing.Brush();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      brush.setAntiAlias(true);
    }
  }
}

export class PerformanceBrushSetAlpha extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let brush: drawing.Brush = new drawing.Brush();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      brush.setAlpha(26);
    }
  }
}

export class PerformanceBrushGetAlpha extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let brush: drawing.Brush = new drawing.Brush();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      brush.getAlpha();
    }
  }
}

export class PerformanceBrushSetColorFilter extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let brush: drawing.Brush = new drawing.Brush();
    let filter = new drawing.ColorFilter();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      brush.setColorFilter(filter);
    }
  }
}

export class PerformanceBrushGetColorFilter extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let brush: drawing.Brush = new drawing.Brush();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      brush.getColorFilter();
    }
  }
}

export class PerformanceBrushSetMaskFilter extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let brush: drawing.Brush = new drawing.Brush();
    let filter = new drawing.MaskFilter();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      brush.setMaskFilter(filter);
    }
  }
}

export class PerformanceBrushSetImageFilter extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let brush: drawing.Brush = new drawing.Brush();
    let filter = new drawing.ImageFilter();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      brush.setImageFilter(filter);
    }
  }
}

export class PerformanceBrushSetShadowLayer extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let brush = new drawing.Brush();
    let shadowLayer = new drawing.ShadowLayer();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      brush.setShadowLayer(shadowLayer);
    }
  }
}

export class PerformanceBrushSetShaderEffect extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let brush = new drawing.Brush();
    let effect = new drawing.ShaderEffect();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      brush.setShaderEffect(effect);
    }
  }
}

export class PerformanceBrushSetBlendMode extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let brush = new drawing.Brush();
    let blend = drawing.BlendMode.CLEAR;
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      brush.setBlendMode(blend);
    }
  }
}

export class PerformanceBrushIsAntiAlias extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let brush = new drawing.Brush();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      brush.isAntiAlias();
    }
  }
}

export class PerformanceBrushReset extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let brush = new drawing.Brush();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      brush.reset();
    }
  }
}