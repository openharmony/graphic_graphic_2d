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

import drawing from "@ohos.graphics.drawing";
import common2D from "@ohos.graphics.common2D";
import {TestBase, StyleType} from '../../pages/testbase';

export class PerformanceTextBlobMakeFromString extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE){
    super();
    this.styleType_ = styleType;
  }
  public OnTestPerformance(canvas: drawing.Canvas) {
    const font: drawing.Font = new drawing.Font();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      const textBlob = drawing.TextBlob.makeFromString("drawing", font, drawing.TextEncoding.TEXT_ENCODING_UTF8);
    }
  }
}

export class PerformanceTextBlobMakeFromRunBuffer extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE){
    super();
    this.styleType_ = styleType;
  }
  public OnTestPerformance(canvas: drawing.Canvas) {
    let font  = new drawing.Font();
    let runBuffer : Array<drawing.TextBlobRunBuffer> = [
      { glyph: 65, positionX: 0, positionY: 0 },
      { glyph: 227, positionX: 14.9 * 2, positionY: 0 },
      { glyph: 283, positionX: 25.84 * 2, positionY: 0 },
      { glyph: 283, positionX: 30.62 * 2, positionY: 0 },
      { glyph: 299, positionX: 35.4 * 2, positionY: 0},
      { glyph: 2, positionX: 47.22 * 2, positionY: 0},
      { glyph: 94, positionX: 52.62 * 2, positionY: 0},
      { glyph: 37, positionX: 67.42 * 2, positionY: 0},
      { glyph: 84, positionX: 81.70 * 2, positionY: 0},
    ];
    let rect: common2D.Rect = {left: 0, top: 0, right: 100, bottom: 100};
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      drawing.TextBlob.makeFromRunBuffer(runBuffer, font, rect);
    }
  }
}

export class PerformanceTextBlobMakeFromPosText extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE){
    super();
    this.styleType_ = styleType;
  }
  public OnTestPerformance(canvas: drawing.Canvas) {
    let text : string = 'makeFromPosText';
    let font : drawing.Font = new drawing.Font();
    let points : common2D.Point[] = [];
    for (let i = 0; i !== text.length; ++i) {
      points.push({ x: i * 35, y: i * 35 });
    }
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      drawing.TextBlob.makeFromPosText(text, text.length, points, font);
    }
  }
}