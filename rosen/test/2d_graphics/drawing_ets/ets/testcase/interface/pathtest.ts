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
import {TestBase, StyleType} from '../../pages/testbase';
import { N, OHRandom } from '../../utils/OHRandom';

const TAG = '[BezierBench]';


export class PathLineTo extends TestBase {

  public constructor(styleType: number) {
    super();
    this.styleType_ = styleType;
  }
  public OnTestPerformance(canvas: drawing.Canvas) {
    console.log(TAG, 'xyj DRAW_ STYLE_COMPLEX end');
    let rand = new OHRandom();
    let path:drawing.Path = new drawing.Path();
    path.moveTo(0, 0);
    for (let i = 0; i < this.testCount_; i++) {
      let x: number = rand.nextUScalar1() * 300;
      let y: number = rand.nextUScalar1() * 300;
      path.lineTo(x, y);
    }
    canvas.drawPath(path);
  }
}

export class PathArcTo extends TestBase {

  public constructor(styleType: number) {
    super();
    this.styleType_ = styleType;
  }
  public OnTestPerformance(canvas: drawing.Canvas) {
    let rand: OHRandom = new OHRandom();
    let path:drawing.Path = new drawing.Path();
    path.moveTo(0, 0);
    for (let i = 0; i < this.testCount_; i++) {
      let x1: number = rand.nextUScalar1() * 300;
      let y1: number = rand.nextUScalar1() * 300;
      let x2: number = rand.nextUScalar1() * 300;
      let y2: number = rand.nextUScalar1() * 300;
      path.arcTo(x1, y1, x2, y2, 0, 0);
    }
    canvas.drawPath(path);
  }
}

export class PathQuadTo extends TestBase {

  public constructor(styleType: number) {
    super();
    this.styleType_ = styleType;
  }
  public OnTestPerformance(canvas: drawing.Canvas) {
    let rand: OHRandom = new OHRandom();
    let path:drawing.Path = new drawing.Path();
    path.moveTo(1, 0);
    for (let i = 0; i < this.testCount_; i++) {
      let ctrlX: number = rand.nextUScalar1() * 300;
      let ctrlY: number = rand.nextUScalar1() * 300;
      let endX: number = rand.nextUScalar1() * 300;
      let endY: number = rand.nextUScalar1() * 300;
      path.quadTo(ctrlX, ctrlY, endX, endY);
    }
    canvas.drawPath(path);
  }
}

export class PathCubicTo extends TestBase {

  public constructor(styleType: number) {
    super();
    this.styleType_ = styleType;
  }
  public OnTestPerformance(canvas: drawing.Canvas) {
    let rand: OHRandom = new OHRandom();
    let path:drawing.Path = new drawing.Path();
    path.moveTo(0, 0);
    for (let i = 0; i < this.testCount_; i++) {
      let ctrlX1: number = rand.nextUScalar1() * 150;
      let ctrlY1: number = rand.nextUScalar1() * 150;
      let ctrlX2: number = rand.nextUScalar1() * 150;
      let ctrlY2: number = rand.nextUScalar1() * 150;
      let endX: number = rand.nextUScalar1() * 150;
      let endY: number = rand.nextUScalar1() * 150;
      path.cubicTo(ctrlX1, ctrlY1, ctrlX2, ctrlY2, endX, endY);
    }
    canvas.drawPath(path);
  }
}

export class PathClose extends TestBase {

  public constructor(styleType: number) {
    super();
    this.styleType_ = styleType;
  }
  public OnTestPerformance(canvas: drawing.Canvas) {
    let rand: OHRandom = new OHRandom();
    let path: drawing.Path = new drawing.Path();
    for (let i = 0; i < this.testCount_; i++) {
      path.moveTo(rand.nextULessThan(this.width_), rand.nextULessThan(this.height_));
      path.lineTo(rand.nextULessThan(this.width_), rand.nextULessThan(this.height_));
      path.lineTo(rand.nextULessThan(this.width_), rand.nextULessThan(this.height_));
      path.close();
      path.reset();
    }
    path.moveTo(rand.nextULessThan(this.width_), rand.nextULessThan(this.height_));
    path.lineTo(rand.nextULessThan(this.width_), rand.nextULessThan(this.height_));
    path.lineTo(rand.nextULessThan(this.width_), rand.nextULessThan(this.height_));
    path.close();
    canvas.drawPath(path);
  }
}

export class PathReset extends TestBase {

  public constructor(styleType: number) {
    super();
    this.styleType_ = styleType;
  }
  public OnTestPerformance(canvas: drawing.Canvas) {
    let rand: OHRandom = new OHRandom();
    let path: drawing.Path = new drawing.Path();
    let x: number = rand.nextULessThan(this.width_);
    let y: number = rand.nextULessThan(this.height_);
    for (let i = 0; i < this.testCount_; i++) {
      path.moveTo(0, 0);
      path.lineTo(x, y);
      path.reset();
    }
    path.moveTo(0, 0);
    path.lineTo(x, y);
    path.close();
    canvas.drawPath(path);
  }
}