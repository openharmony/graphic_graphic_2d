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
import common2D from '@ohos.graphics.common2D';
import {TestBase, StyleType} from '../../pages/testbase';
import { N, OHRandom } from '../../utils/OHRandom';

const TAG = '[BezierBench]';


export class MakeFromRunBuffer extends TestBase {

  public constructor(styleType: number) {
    super();
    this.styleType_ = styleType;
  }
  public OnTestPerformance(canvas: drawing.Canvas) {
    let rand: OHRandom = new OHRandom();
    const font: drawing.Font = new drawing.Font();
    for (let i = 0; i < this.testCount_; i++) {
      let l: number = rand.nextULessThan(this.width_);
      let t: number = rand.nextULessThan(this.height_);
      let r: number = l + rand.nextULessThan(this.width_);
      let b: number = l + rand.nextULessThan(this.height_);
      let rect: common2D.Rect = {left: l, top: t, right: r, bottom: b};
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
      const textBlob: drawing.TextBlob = drawing.TextBlob.makeFromRunBuffer(runBuffer, font, rect);
      canvas.drawTextBlob(textBlob, rand.nextULessThan(this.width_),rand.nextULessThan(this.height_));
    }
  }
}

export class MakeFromString extends TestBase {

  public constructor(styleType: number) {
    super();
    this.styleType_ = styleType;
  }
  public OnTestPerformance(canvas: drawing.Canvas) {
    let rand: OHRandom = new OHRandom();
    const text: string = 'TextBlobCreateFromString';
    const font: drawing.Font = new drawing.Font();
    for (let i = 0; i < this.testCount_; i++) {
      const textBlob: drawing.TextBlob = drawing.TextBlob.makeFromString(text, font, drawing.TextEncoding.TEXT_ENCODING_UTF8);
      canvas.drawTextBlob(textBlob, rand.nextULessThan(this.width_),rand.nextULessThan(this.height_));
    }
  }
}

export class TextBlobBounds extends TestBase {

  public constructor(styleType: number) {
    super();
    this.styleType_ = styleType;
  }
  public OnTestPerformance(canvas: drawing.Canvas) {
    let rand: OHRandom = new OHRandom();
    const text: string = 'TextBlobGetBounds';
    const font: drawing.Font = new drawing.Font();
    for (let i = 0; i < this.testCount_; i++) {
      const textBlob: drawing.TextBlob = drawing.TextBlob.makeFromString(text, font, drawing.TextEncoding.TEXT_ENCODING_UTF8);
      canvas.drawTextBlob(textBlob, rand.nextULessThan(this.width_),rand.nextULessThan(this.height_));
      let bounds:common2D.Rect = textBlob.bounds();
      // canvas.drawRect(bounds);
    }
  }
}



