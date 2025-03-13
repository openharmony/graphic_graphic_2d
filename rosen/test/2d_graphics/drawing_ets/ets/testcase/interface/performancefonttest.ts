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

export class PerformanceFontEnableSubpixel extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let font = new drawing.Font();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      font.enableSubpixel(true);
    }
  }
}

export class PerformanceFontEnableEmbolden extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let font = new drawing.Font();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      font.enableEmbolden(true);
    }
  }
}

export class PerformanceFontEnableLinearMetrics extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let font = new drawing.Font();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      font.enableLinearMetrics(true);
    }
  }
}

export class PerformanceFontSetBaselineSnap extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let font = new drawing.Font();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      font.setBaselineSnap(true);
    }
  }
}

export class PerformanceFontSetEmbeddedBitmaps extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let font = new drawing.Font();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      font.setEmbeddedBitmaps(true);
    }
  }
}

export class PerformanceFontSetForceAutoHinting extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let font = new drawing.Font();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      font.setForceAutoHinting(true);
    }
  }
}

export class PerformanceFontSetSize extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let font = new drawing.Font();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      font.setSize(100);
    }
  }
}

export class PerformanceFontSetTypeface extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let font = new drawing.Font();
    let type = new drawing.Typeface();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      font.setTypeface(type);
    }
  }
}

export class PerformanceFontGetTypeface extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let font = new drawing.Font();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      font.getTypeface();
    }
  }
}

export class PerformanceFontGetSize extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let font = new drawing.Font();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      font.getSize();
    }
  }
}

export class PerformanceFontGetMetrics extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let font = new drawing.Font();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      font.getMetrics();
    }
  }
}

export class PerformanceFontGetWidths extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let font = new drawing.Font();
    let num: number[] = [15, 16, 17];
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      font.getWidths(num);
    }
  }
}

export class PerformanceFontIsBaselineSnap extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let font = new drawing.Font();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      font.isBaselineSnap();
    }
  }
}

export class PerformanceFontIsEmbeddedBitmaps extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let font = new drawing.Font();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      font.isEmbeddedBitmaps();
    }
  }
}

export class PerformanceFontIsForceAutoHinting extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let font = new drawing.Font();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      font.isForceAutoHinting();
    }
  }
}

export class PerformanceFontMeasureSingleCharacter extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let font = new drawing.Font();
    let text = "h";
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      font.measureSingleCharacter(text);
    }
  }
}

export class PerformanceFontMeasureText extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let font = new drawing.Font();
    let text = "hello world";
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      font.measureText(text, drawing.TextEncoding.TEXT_ENCODING_UTF8);
    }
  }
}

export class PerformanceFontSetScaleX extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let font = new drawing.Font();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      font.setScaleX(15);
    }
  }
}

export class PerformanceFontSetSkewX extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let font = new drawing.Font();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      font.setSkewX(15);
    }
  }
}

export class PerformanceFontSetEdging extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let font = new drawing.Font();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      font.setEdging(drawing.FontEdging.ALIAS);
    }
  }
}

export class PerformanceFontSetHinting extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let font = new drawing.Font();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      font.setHinting(drawing.FontHinting.NONE);
    }
  }
}

export class PerformanceFontCountText extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let font = new drawing.Font();
    let text = "hello world";
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      font.countText(text);
    }
  }
}

export class PerformanceFontIsSubpixel extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let font = new drawing.Font();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      font.isSubpixel();
    }
  }
}

export class PerformanceFontIsLinearMetrics extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let font = new drawing.Font();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      font.isLinearMetrics();
    }
  }
}

export class PerformanceFontGetSkewX extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let font = new drawing.Font();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      font.getSkewX();
    }
  }
}

export class PerformanceFontIsEmbolden extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let font = new drawing.Font();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      font.isEmbolden();
    }
  }
}

export class PerformanceFontGetScaleX extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let font = new drawing.Font();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      font.getScaleX();
    }
  }
}

export class PerformanceFontGetHinting extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let font = new drawing.Font();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      font.getHinting();
    }
  }
}

export class PerformanceFontGetEdging extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let font = new drawing.Font();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      font.getEdging();
    }
  }
}

export class PerformanceFontTextToGlyphs extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let font = new drawing.Font();
    let text = "hello world";
    let num = text.length;
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      font.textToGlyphs(text, num);
    }
  }
}
