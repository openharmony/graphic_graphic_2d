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

function displayTestResult(canvas: drawing.Canvas, isPassed : Boolean) {
  let testResult : string = isPassed ? "PASSED" : "FAILED. Please check the console logs";
  let font : drawing.Font = new drawing.Font();
  font.setSize(50);
  let blob : drawing.TextBlob = drawing.TextBlob.makeFromString(testResult, font, drawing.TextEncoding.TEXT_ENCODING_UTF8);
  canvas.drawTextBlob(blob, 10, 100);
}

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
  private rand_: OHRandom = new OHRandom();
  private text_: string = 'TextBlobCreateFromString';
  private font_: drawing.Font = new drawing.Font();

  public constructor(styleType: number) {
    super();
    this.styleType_ = styleType;
    this.font_.setSize(80);
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let textBlob: drawing.TextBlob;
    for (let i = 0; i < this.testCount_; i++) {
      textBlob = drawing.TextBlob.makeFromString(this.text_, this.font_, drawing.TextEncoding.TEXT_ENCODING_UTF8);    
    }
    canvas.drawTextBlob(textBlob, this.rand_.nextULessThan(this.width_),this.rand_.nextULessThan(this.height_));
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    const textBlob: drawing.TextBlob = drawing.TextBlob.makeFromString(this.text_, this.font_, drawing.TextEncoding.TEXT_ENCODING_UTF8);
    canvas.drawTextBlob(textBlob, 10, 100);
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
    }
  }
}

export class MakeFromPosText extends TestBase {
  private rand_: OHRandom = new OHRandom();
  private points_: common2D.Point[] = [];
  private text_: string = 'TextBlobCreateFromPosText';
  private font_: drawing.Font = new drawing.Font();

  public constructor(styleType: number) {
    super();
    this.styleType_ = styleType;
    for (let i = 0; i !== this.text_.length; ++i) {
      this.points_.push({ x: i * 35, y: i * 35 });
    }
  }

  private getTextBlob() {
    return drawing.TextBlob.makeFromPosText(this.text_, this.points_.length, this.points_, this.font_);
  }
  public OnTestFunction(canvas: drawing.Canvas) {
    this.font_.setSize(50);
    canvas.drawTextBlob(this.getTextBlob(), 100, 100);
  }
  public OnTestPerformance(canvas: drawing.Canvas) {
    for (let i = 0; i < this.testCount_; i++) {
      canvas.drawTextBlob(this.getTextBlob(), this.rand_.nextULessThan(this.width_), this.rand_.nextULessThan(this.height_));
    }
  }
}

export class MakeUniqueId extends TestBase {
  private rand_: OHRandom = new OHRandom();
  private points_: common2D.Point[] = [];
  private text_: string = 'TextBlobUniqueId';
  private font_: drawing.Font = new drawing.Font();
  private textBlob_: drawing.TextBlob;

  public constructor() {
    super();
    this.font_.setSize(100);
    this.textBlob_ = drawing.TextBlob.makeFromString(this.text_, this.font_, drawing.TextEncoding.TEXT_ENCODING_UTF8);
  }
  public OnTestPerformance(canvas: drawing.Canvas) {
    for (let i = 0; i < this.testCount_; i++) {
      this.textBlob_.uniqueID();
    }
    canvas.drawTextBlob(this.textBlob_, 10, 100);
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    let result1: boolean = false;
    let id = this.textBlob_.uniqueID();
    {
      let same_id = this.textBlob_.uniqueID();
      if (id == same_id) {
        result1 = true;
      } else { 
        console.log(TAG, "The 1st uniqueID: " + id + ", the 2nd uniqueID for the same text blob: " + same_id);
      }
    }

    let result2: boolean = false;
    {
      let text: string = 'TextBlobUniqueIdNext';
      const textBlobNext: drawing.TextBlob = drawing.TextBlob.makeFromString(text, this.font_, drawing.TextEncoding.TEXT_ENCODING_UTF8);
      let next_id = textBlobNext.uniqueID();
      if (id != next_id) {
        result2 = true;
      } else {
        console.log(TAG, "The 1st uniqueID: " + id + ", the 2nd uniqueID for the same text blob: " + next_id);
      }
    }
    displayTestResult(canvas, result1 && result2);
  } 
}
