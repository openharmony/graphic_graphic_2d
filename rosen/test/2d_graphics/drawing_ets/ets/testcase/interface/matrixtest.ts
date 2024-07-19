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
import { OHRandom } from '../../utils/OHRandom';
import { TestBase, TestFunctionStyleType, StyleType } from '../../pages/testbase';

const TAG = '[BezierBench]';

function displayTestResult(canvas: drawing.Canvas, isPassed : Boolean) {
  let testResult : string = isPassed ? "PASSED" : "FAILED. Please check the console logs";
  let font : drawing.Font = new drawing.Font();
  font.setSize(50);
  let blob : drawing.TextBlob = drawing.TextBlob.makeFromString(testResult, font, drawing.TextEncoding.TEXT_ENCODING_UTF8);
  canvas.drawTextBlob(blob, 10, 100);
}

function checkMatrix(actualMatrix : drawing.Matrix, expectedValues: Array<number>): boolean {
  // returns FALSE if matrix has unexpected element value
  let failure: boolean = false;
  let fractionDigits: number = 5;
  for (let i = 0; i < expectedValues.length; i++) {
    if (actualMatrix.getValue(i).toFixed(fractionDigits) != expectedValues[i].toFixed(fractionDigits) ) {
      failure = true;
      console.log(TAG, "The matrix[" + i + "] is incorrect (actual: " + actualMatrix.getValue(i)
        + ", expected: " + expectedValues[i] + ")");
    }
  }
  return !failure;
}

export class MatrixGetValue extends TestBase {
  public constructor() {
    super();
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    let expectedValues: Array<number> = [1, 0, 0, 0, 1, 0, 0, 0, 1];
    let matrix = new drawing.Matrix();

    let failure: boolean = false;
    for (let i = 0; i < expectedValues.length; i++) {
      if (matrix.getValue(i) != expectedValues[i]) {
        failure = true;
        console.log(TAG, "The matrix[" + i + "] is incorrect (actual: " + matrix.getValue(i)
          + ", expected: " + expectedValues[i] + ")");
      }
    }
    displayTestResult(canvas, !failure);
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let matrix = new drawing.Matrix();
    for (let i = 0; i < this.testCount_; i++) {
      matrix.getValue(0);
    }
    displayTestResult(canvas, true);
  }
}

export class MatrixGetAll extends TestBase {
  public constructor() {
    super();
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    let failure: boolean = false;
    let matrix = new drawing.Matrix();
    let expectedValues: Array<number> = [1, 0, 0, 0, 1, 0, 0, 0, 1];

    let actualValues = matrix.getAll();
    if (actualValues.length != expectedValues.length) {
      failure = true;
      console.log(TAG, "The length of returned array is incorrect (actual: " + actualValues.length
        + ", expected: " + expectedValues.length  + ")");
    } else {
      for (let i = 0; i < actualValues.length; i++) {
        if (actualValues[i] != expectedValues[i]) {
          failure = true;
          console.log(TAG, "The matrix[" + i + "] is incorrect (actual: " + actualValues[i]
            + ", expected: " + expectedValues[i] + ")");
        }
      }
    }
    displayTestResult(canvas, !failure);
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let matrix = new drawing.Matrix();
    for (let i = 0; i < this.testCount_; i++) {
      let matrixBuffer = matrix.getAll();
    }
    displayTestResult(canvas, true);
  }
}

export class MatrixPostRotate extends TestBase {

  public constructor() {
    super();
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    let matrix = new drawing.Matrix();
    let degree: number = 2;
    let px: number = 3;
    let py: number = 4;
    matrix.postRotate(degree, px, py);

    let c = Math.cos(degree * Math.PI / 180);
    let s = Math.sin(degree * Math.PI / 180);
    let dx = s * py + (1 - c) * px;
    let dy = -s * px + (1 - c) * py;
    let expectedValues: Array<number> = [c, -s, dx, s, c, dy, 0, 0, 1];

    displayTestResult(canvas, checkMatrix(matrix, expectedValues));
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let matrix = new drawing.Matrix();
    for (let i = 0; i < this.testCount_; i++) {
      matrix.postRotate(2, 3, 4);
    }
    displayTestResult(canvas, true);
  }
}

export class MatrixPostTranslate extends TestBase {

  public constructor() {
    super();
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    let matrix = new drawing.Matrix();
    let dx: number = 3;
    let dy: number = 4;
    matrix.postTranslate(dx, dy);

    let expectedValues: Array<number> = [1, 0, dx, 0, 1, dy, 0, 0, 1];
    displayTestResult(canvas, checkMatrix(matrix, expectedValues));
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let matrix = new drawing.Matrix();
    for (let i = 0; i < this.testCount_; i++) {
      matrix.postTranslate(3, 4);
    }
    displayTestResult(canvas, true);
  }
}

export class MatrixReset extends TestBase {

  public constructor() {
    super();
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    let expectedValues: Array<number> = [1, 0, 0, 0, 1, 0, 0, 0, 1];
    let matrix = new drawing.Matrix();
    matrix.postRotate(2, 3, 4);
    matrix.reset();
    displayTestResult(canvas, checkMatrix(matrix, expectedValues));
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let matrix = new drawing.Matrix();
    for (let i = 0; i < this.testCount_; i++) {
      matrix.reset();
    }
    displayTestResult(canvas, true);
  }
}

export class MatrixSetPolyToPoly extends TestBase {

  public constructor() {
    super();
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    let srcPoints: Array<common2D.Point> = [ { x: 200, y: 20 }, { x: 200, y: 60 } ];
    let dstPoints: Array<common2D.Point> = [ { x:0, y: 0 }, { x:0, y: 0 } ];
    let brush = new drawing.Brush();
    let color: common2D.Color = { alpha: 255, red: 0, green: 255, blue: 0 };
    let matrix = new drawing.Matrix();
    canvas.setMatrix(matrix);
    brush.setColor(color);
    canvas.attachBrush(brush);
    console.log(TAG, "matrix.setPolyToPoly1");
    let result: boolean = false;
    try {
      if (matrix.setPolyToPoly(srcPoints, dstPoints, 2)) {
        console.log(TAG, "canvas.drawLine(dstPoints");
        canvas.drawLine(dstPoints[0].x, dstPoints[0].y, dstPoints[1].x, dstPoints[1].y);
        result = true;
      }
    } catch(err) {
      console.error("Matrix.setPolyToPoly exception: ", err.name, ":", err.message, err.stack);
    }
    canvas.detachBrush();
    if (!result) {
      displayTestResult(canvas, false);
    }
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let matrix = new drawing.Matrix();
    let srcPoints: Array<common2D.Point> = [ { x: 200, y: 20 }, { x: 200, y: 60 } ];
    let dstPoints: Array<common2D.Point> = [ { x:0, y: 0 }, { x:0, y: 0 } ];
    let result: boolean = true;
    for (let i = 0; i < this.testCount_; i++) {
      try {
        matrix.setPolyToPoly(srcPoints, dstPoints, 2);
      } catch(err) {
        console.error("Matrix.setPolyToPoly exception: ", err.name, ":", err.message, err.stack);
        result = false;
      }
    }
    displayTestResult(canvas, result);
  }
}

export class MatrixSetRectToRect extends TestBase {

  public constructor() {
    super();
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    let src: common2D.Rect = { left: 100, top: 20, right: 130, bottom: 60 };
    let dst: common2D.Rect = { left: 100, top: 80, right: 130, bottom: 120 };
    let stf: drawing.ScaleToFit = drawing.ScaleToFit.FILL_SCALE_TO_FIT;
    let brush = new drawing.Brush();
    let color: common2D.Color = { alpha: 255, red: 0, green: 255, blue: 0 };
    let matrix = new drawing.Matrix();
    canvas.setMatrix(matrix);
    brush.setColor(color);
    canvas.attachBrush(brush);
    console.log(TAG, "matrix.setRectToRect1");
    if (matrix.setRectToRect(src, dst, stf)) {
        console.log(TAG, "canvas.drawRect");
        canvas.drawRect(dst);
    }
    canvas.detachBrush();
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let matrix = new drawing.Matrix();
    let src: common2D.Rect = { left: 100, top: 20, right: 130, bottom: 60 };
    let dst: common2D.Rect = { left: 100, top: 80, right: 130, bottom: 120 };
    let stf: drawing.ScaleToFit = drawing.ScaleToFit.FILL_SCALE_TO_FIT;
    for (let i = 0; i < this.testCount_; i++) {
      matrix.setRectToRect(src, dst, stf);
    }
    displayTestResult(canvas, true);
  }
}

export class MatrixPreScale extends TestBase {

  public constructor() {
    super();
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    let matrix = new drawing.Matrix();
    let sx: number = 2;
    let sy: number = 0.5;
    let px: number = 1;
    let py: number = 1;

    let dx = px - sx * px;
    let dy = py - sy * py;
    matrix.preScale(sx, sy, px, py);

    let expectedValues: Array<number> = [sx, 0, dx, 0, sy, dy, 0, 0, 1];
    displayTestResult(canvas, checkMatrix(matrix, expectedValues));
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let matrix = new drawing.Matrix();
    for (let i = 0; i < this.testCount_; i++) {
      matrix.preScale(2, 0.5, 0, 0);
    }
    displayTestResult(canvas, true);
  }
}

export class MatrixPreTranslate extends TestBase {

  public constructor() {
    super();
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    let matrix = new drawing.Matrix();
    let dx: number = 3;
    let dy: number = 4;
    matrix.preTranslate(dx, dy);

    let expectedValues: Array<number> = [1, 0, dx, 0, 1, dy, 0, 0, 1];
    displayTestResult(canvas, checkMatrix(matrix, expectedValues));
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let matrix = new drawing.Matrix();
    for (let i = 0; i < this.testCount_; i++) {
      matrix.preTranslate(3, 4);
    }
    displayTestResult(canvas, true);
  }
}

export class MatrixPreRotate extends TestBase {

  public constructor() {
    super();
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    let matrix = new drawing.Matrix();
    let degree: number = 2;
    let px: number = 3;
    let py: number = 4;
    matrix.preRotate(degree, px, py);

    let c = Math.cos(degree * Math.PI / 180);
    let s = Math.sin(degree * Math.PI / 180);
    let dx = s * py + (1 - c) * px;
    let dy = -s * px + (1 - c) * py;
    let expectedValues: Array<number> = [c, -s, dx, s, c, dy, 0, 0, 1];

    displayTestResult(canvas, checkMatrix(matrix, expectedValues));
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let matrix = new drawing.Matrix();
    for (let i = 0; i < this.testCount_; i++) {
      matrix.postRotate(2, 3, 4);
    }
    displayTestResult(canvas, true);
  }
}

export class MatrixPostScale extends TestBase {

  public constructor() {
    super();
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    let matrix = new drawing.Matrix();
    let sx: number = 2;
    let sy: number = 0.5;
    let px: number = 1;
    let py: number = 1;

    let dx = px - sx * px;
    let dy = py - sy * py;
    matrix.preScale(sx, sy, px, py);

    let expectedValues: Array<number> = [sx, 0, dx, 0, sy, dy, 0, 0, 1];
    displayTestResult(canvas, checkMatrix(matrix, expectedValues));
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let matrix = new drawing.Matrix();
    for (let i = 0; i < this.testCount_; i++) {
      matrix.postScale(2, 0.5, 0, 0);
    }
    displayTestResult(canvas, true);
  }
}

export class MatrixMapRect extends TestBase {
  public constructor() {
    super();
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    let dst: common2D.Rect = { left: 100, top: 20, right: 130, bottom: 60 };
    let src: common2D.Rect = { left: 100, top: 80, right: 130, bottom: 120 };
    let brush = new drawing.Brush();
    let color: common2D.Color = { alpha: 255, red: 0, green: 255, blue: 0 };
    let matrix = new drawing.Matrix();
    canvas.setMatrix(matrix);
    brush.setColor(color);
    canvas.attachBrush(brush);
    console.log(TAG, "matrix.mapRect");
    if (matrix.mapRect(dst, src)) {
      console.log(TAG, "canvas.drawRect");
      canvas.drawRect(dst);
    }
    canvas.detachBrush();
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let matrix = new drawing.Matrix();
    let src: common2D.Rect = { left: 100, top: 20, right: 130, bottom: 60 };
    let dst: common2D.Rect = { left: 100, top: 80, right: 130, bottom: 120 };
    for (let i = 0; i < this.testCount_; i++) {
      matrix.mapRect(dst, src);
    }
    displayTestResult(canvas, true);
  }
}

export class MatrixMapPoints extends TestBase {

  public constructor(){
    super();
  }

  public OnTestPerformance(canvas: drawing.Canvas) {

    let rand: OHRandom = new OHRandom();
    let src: Array<common2D.Point> = [];
    let pointsCount: number = 1000;
    for (let i = 0; i < pointsCount; i++) {
      src.push({x: rand.nextUScalar1() * 640, y: rand.nextUScalar1() * 480});
    }
    let matrix = new drawing.Matrix();

    for (let i = 0; i < this.testCount_; i++) {
      let dst: Array<common2D.Point> = matrix.mapPoints(src);
    }
    displayTestResult(canvas, true);
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    {
      let matrix = new drawing.Matrix();
      canvas.setMatrix(matrix);

      let src: Array<common2D.Point> = [{x: 30, y:50}, {x: 10, y:15}, {x: 5, y:40}];
      let dst: Array<common2D.Point> = matrix.mapPoints(src);

      if (dst.length != 3) {
        console.error(TAG, 'Invalid dst length!');
        displayTestResult(canvas, false);
      } else {
        let pen = new drawing.Pen();
        this.ApplyPenStyle(pen, TestFunctionStyleType.DRAW_STYLE_TYPE_1);
        let brush = new drawing.Brush();
        this.ApplyBrushStyle (brush, TestFunctionStyleType.DRAW_STYLE_TYPE_1)
        canvas.attachPen(pen);
        canvas.attachBrush(brush);
        dst.forEach(point => {
          canvas.drawPoint(point.x, point.y)
        });
        canvas.detachPen();
        canvas.detachBrush();
      }
    }
  }
}
