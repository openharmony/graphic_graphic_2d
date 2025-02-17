/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

export class PathIsClosed extends TestBase {
  public constructor(styleType: number) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    let path: drawing.Path = new drawing.Path();
    path.moveTo(0, 0);
    path.lineTo(0, this.height_);
    path.lineTo(this.width_, 0);
    path.close();
    path.isClosed();
    canvas.drawPath(path);
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let rand: OHRandom = new OHRandom();
    let path: drawing.Path = new drawing.Path();
    for (let i = 0; i < this.testCount_; i++) {
      let isNeedClose = rand.nextULessThan(2);
      if (isNeedClose === 1) {
        path.close();
      }
      path.isClosed();
    }
    canvas.drawPath(path);
  }
}

export class PathGetPositionAndTangent extends TestBase {
  public constructor(styleType: number) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    let path: drawing.Path = new drawing.Path();
    path.moveTo(0, 0);
    path.lineTo(100, 100);
    path.getPositionAndTangent(false, 0.1, { x: 0.5, y: 0.3 }, { x: 3.5, y: 3.3 });
    canvas.drawPath(path);
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let rand: OHRandom = new OHRandom();
    let path: drawing.Path = new drawing.Path();
    for (let i = 0; i < this.testCount_; i++) {
      let posx = rand.nextUScalar1() * 300;
      let posy = rand.nextUScalar1() * 300;
      let tanx = rand.nextUScalar1() * 300;
      let tany = rand.nextUScalar1() * 300;

      path.getPositionAndTangent(false, 10, { x: posx, y: posy }, { x: tanx, y: tany });
    }
    canvas.drawPath(path);
  }
}

export class PathGetMatrix extends TestBase {
  public constructor(styleType: number) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestFunction(canvas: drawing.Canvas) {
      let path: drawing.Path = new drawing.Path();
      path.moveTo(0, 0);
      path.lineTo(0, this.height_);
      path.lineTo(this.width_, 0);
      let matrix = new drawing.Matrix();
      path.getMatrix(false, 10, matrix, drawing.PathMeasureMatrixFlags.GET_POSITION_MATRIX);
      canvas.drawPath(path);
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let rand: OHRandom = new OHRandom();
    let path: drawing.Path = new drawing.Path();
    let matrix = new drawing.Matrix();
    path.moveTo(rand.nextULessThan(this.width_), rand.nextULessThan(this.height_));
    path.lineTo(rand.nextULessThan(this.width_), rand.nextULessThan(this.height_));
    path.lineTo(rand.nextULessThan(this.width_), rand.nextULessThan(this.height_));

    for (let i = 0; i < this.testCount_; i++) {
      path.getMatrix(false, 10, matrix, drawing.PathMeasureMatrixFlags.GET_POSITION_MATRIX);
    }
    canvas.drawPath(path);
  }
}

export class PathBuildFromSvgString extends TestBase {
  public constructor(styleType: number) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestFunction(canvas: drawing.Canvas) {
      let path: drawing.Path = new drawing.Path();
      let svgStr: string =  "M150 100 L75 300 L225 300 Z";
      path.buildFromSvgString(svgStr);
      canvas.drawPath(path);
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let rand: OHRandom = new OHRandom();
    let path: drawing.Path = new drawing.Path();

    for (let i = 0; i < this.testCount_; i++) {
      let x1 = rand.nextULessThan(this.width_);
      let y1 = rand.nextULessThan(this.height_);
      let x2 = rand.nextULessThan(this.width_);
      let y2 = rand.nextULessThan(this.height_);
      let x3 = rand.nextULessThan(this.width_);
      let y3 = rand.nextULessThan(this.height_);

      let svgStr: string =  `M${x1} ${y1} L${x2} ${y2} L${x3} ${y3} Z`;
      path.buildFromSvgString(svgStr);
    }
    canvas.drawPath(path);
  }
}

export class PathConstructor extends TestBase {
  public constructor() {
    super();
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    let path: drawing.Path = new drawing.Path();
    path.moveTo(0, 0);
    path.lineTo(0, this.height_);
    path.lineTo(this.width_, 0);
    path.close();

    let path2 : drawing.Path;
    try {
      path2 = new drawing.Path(path);
    } catch(err) {
      console.error("Path.constructor(Path) exception: ", err.name, ":", err.message, err.stack);
      this.displayTestResult(canvas, false);
      return;
    }

    this.displayTestResult(canvas, (path2.getBounds().left == path.getBounds().left &&
      path2.getBounds().top == path.getBounds().top &&
      path2.getBounds().right == path.getBounds().right &&
      path2.getBounds().bottom == path.getBounds().bottom &&
      path2.getLength(true) == path.getLength(true)));
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let path: drawing.Path = new drawing.Path();
    path.moveTo(0, 0);
    path.lineTo(0, this.height_);
    path.lineTo(this.width_, 0);
    path.close();
    let path2: drawing.Path;
    for (let i = 0; i < this.testCount_; i++) {
      try {
        path2 = new drawing.Path(path);
      } catch(err) {
        console.error("Path.constructor(Path) exception: ", err.name, ":", err.message, err.stack);
        this.displayTestResult(canvas, false);
        return;
      }
    }
    this.displayTestResult(canvas, (path2.getBounds().left == path.getBounds().left &&
      path2.getBounds().top == path.getBounds().top &&
      path2.getBounds().right == path.getBounds().right &&
      path2.getBounds().bottom == path.getBounds().bottom &&
      path2.getLength(true) == path.getLength(true)));
  }

  private displayTestResult(canvas: drawing.Canvas, isPassed : Boolean) {
    let testResult : string = isPassed ? "PASSED" : "FAILED. Please check the console logs";
    let font : drawing.Font = new drawing.Font();
    font.setSize(50);
    let blob : drawing.TextBlob = drawing.TextBlob.makeFromString(testResult, font, drawing.TextEncoding.TEXT_ENCODING_UTF8);
    canvas.drawTextBlob(blob, 10, 100);
  }
}

export class PathGetPathIterator extends TestBase {
  public constructor() {
    super();
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    let pen = new drawing.Pen();
    pen.setAntiAlias(true);
    pen.setColor({ alpha: 0xFF, red: 0xFF, green: 0x00, blue: 0x00 });
    pen.setStrokeWidth(5.0);
    canvas.attachPen(pen);
    let path: drawing.Path = new drawing.Path();
    path.moveTo(0, 0);
    path.lineTo(100, 100);
    let iter = path.getPathIterator();
    let verb = iter.peek();
    console.log("PathGetPathIterator: verb = ", verb);
    canvas.drawPath(path);
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let pen = new drawing.Pen();
    pen.setAntiAlias(true);
    pen.setColor({ alpha: 0xFF, red: 0xFF, green: 0x00, blue: 0x00 });
    pen.setStrokeWidth(5.0);
    canvas.attachPen(pen);
    let path: drawing.Path = new drawing.Path();
    path.moveTo(0, 0);
    path.lineTo(100, 100);
    for (let i = 0; i < this.testCount_; i++) {
      path.getPathIterator();
    }
    let iter = path.getPathIterator();
    let verb = iter.peek();
    console.log("PathGetPathIterator: verb = ", verb);
    canvas.drawPath(path);
  }
}