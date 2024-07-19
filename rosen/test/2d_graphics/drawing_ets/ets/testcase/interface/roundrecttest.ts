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
import { TestBase } from '../../pages/testbase';
import { OHRandom } from '../../utils/OHRandom';

export class RoundRectTest extends TestBase {

  rand_: OHRandom | undefined;
  roundRect_: drawing.RoundRect;
  cornerPos_: drawing.CornerPos;
  x_: number | 0xFF;
  y_: number | 0xFF;
  dx_: number | 0xFF;
  dy_: number | 0xFF;
  pen_: drawing.Pen;
  brush_: drawing.Brush;

  public constructor() {
    super();
    this.rand_ = new OHRandom();
    this.x_ = this.rand_.nextULessThan(10);
    this.y_ = this.rand_.nextULessThan(10);
    this.dx_ = this.rand_.nextULessThan(100);
    this.dy_ = this.rand_.nextULessThan(100);
    this.roundRect_ = new drawing.RoundRect({left: 0, top: 0, right: 300, bottom: 300}, 50, 50);
    this.pen_ = new  drawing.Pen();
    this.brush_ = new drawing.Brush();
    this.pen_.setColor({ alpha: 255, red: 255, green: 0, blue: 0 });
    this.brush_.setColor({ alpha: 255, red: 255, green: 0, blue: 0 });
  }

  public static printResults(canvas: drawing.Canvas, isPassed: Boolean) {
    let font: drawing.Font = new drawing.Font();
    font.setSize(50);
    let blob: drawing.TextBlob = drawing.TextBlob.makeFromString(isPassed ? "Passed": "Not Passed",
      font, drawing.TextEncoding.TEXT_ENCODING_UTF8);
    canvas.drawTextBlob(blob, 10, 50);
  }
}

export class RoundRectSetCornerTest extends RoundRectTest {

  public constructor() {
    super();
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    canvas.attachPen(this.pen_);
    canvas.attachBrush(this.brush_);
    canvas.drawRoundRect(this.roundRect_);
    canvas.detachPen();
    canvas.detachBrush();

    this.pen_.setColor({ alpha: 255, red: 0, green: 255, blue: 0 });
    this.brush_.setColor({ alpha: 255, red: 0, green: 255, blue: 0 });

    try {
      this.roundRect_.setCorner(drawing.CornerPos.BOTTOM_RIGHT_POS, 150, 150);
    } catch (e) {
      console.log("setCorner exception " + e);
      RoundRectTest.printResults(canvas, false);
    }

    canvas.attachPen(this.pen_);
    canvas.attachBrush(this.brush_);
    canvas.drawRoundRect(this.roundRect_);
    canvas.detachPen();
    canvas.detachBrush();
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    for (let i = 0; i < this.testCount_; i++) {
      this.roundRect_.setCorner(this.cornerPos_, this.x_, this.y_);
    }
    RoundRectTest.printResults(canvas, true);
  }
}

export class RoundRectGetCornerTest extends RoundRectTest {

  public constructor() {
    super();
    this.roundRect_.setCorner(this.cornerPos_, this.x_, this.y_);
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    {
      canvas.attachPen(this.pen_);
      canvas.attachBrush(this.brush_);
      canvas.drawRoundRect(this.roundRect_);
      canvas.detachPen();
      canvas.detachBrush();

      this.pen_.setColor({ alpha: 255, red: 0, green: 255, blue: 0 });
      this.brush_.setColor({ alpha: 255, red: 0, green: 255, blue: 0 });
      
      this.roundRect_.setCorner(drawing.CornerPos.BOTTOM_LEFT_POS, 150, 150);
      let cornerRadius
      try {
        cornerRadius = this.roundRect_.getCorner(drawing.CornerPos.BOTTOM_LEFT_POS);
      } catch (e) {
        console.log("getCorner exception " + e);
        RoundRectTest.printResults(canvas, false);
      }
      this.roundRect_.setCorner(drawing.CornerPos.BOTTOM_RIGHT_POS, cornerRadius.x, cornerRadius.y);

      canvas.attachPen(this.pen_);
      canvas.attachBrush(this.brush_);
      canvas.drawRoundRect(this.roundRect_);
      canvas.detachPen();
      canvas.detachBrush();
    }
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    for (let i = 0; i < this.testCount_; i++) {
      this.roundRect_.getCorner(this.cornerPos_);
    }
    RoundRectTest.printResults(canvas, true);
  }
}

export class RoundRectOffsetTest extends RoundRectTest {

  public constructor() {
    super();
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    canvas.attachPen(this.pen_);
    canvas.attachBrush(this.brush_);
    canvas.drawRoundRect(this.roundRect_);
    canvas.detachPen();
    canvas.detachBrush();

    this.pen_.setColor({ alpha: 255, red: 0, green: 255, blue: 0 });
    this.brush_.setColor({ alpha: 255, red: 0, green: 255, blue: 0 });

    try {
      this.roundRect_.offset(this.dx_, this.dy_);
    } catch (e) {
      console.log("offset exception " + e);
      RoundRectTest.printResults(canvas, false);
    }

    canvas.attachPen(this.pen_);
    canvas.attachBrush(this.brush_);
    canvas.drawRoundRect(this.roundRect_);
    canvas.detachPen();
    canvas.detachBrush();
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    for (let i = 0; i < this.testCount_; i++) {
      this.roundRect_.offset(this.dx_, this.dy_);
    }
    RoundRectTest.printResults(canvas, true);
  }
}
