/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
import { TestBase } from '../../pages/testbase';

const TAG = '[BezierBench]';

export class PathIteratorConstructor extends TestBase {
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
    let iter: drawing.PathIterator = new drawing.PathIterator(path);
    console.log("PathIteratorConstructor: iter create success with path");
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
      let iter: drawing.PathIterator = new drawing.PathIterator(path);
    }
    let iter: drawing.PathIterator = new drawing.PathIterator(path);
    console.log("PathIteratorConstructor: iter create success with path");
    canvas.drawPath(path);
  }
}

export class PathIteratorNext extends TestBase {
  public constructor() {
    super();
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    {
      let pen = new drawing.Pen();
      pen.setAntiAlias(true);
      pen.setColor({ alpha: 0xFF, red: 0x00, green: 0x00, blue: 0xFF });
      pen.setStrokeWidth(5.0);
      canvas.attachPen(pen);
      let path: drawing.Path = new drawing.Path();
      path.lineTo(100, 200);
      path.quadTo(100, 200, 400, 100);
      path.conicTo(400, 100, 500, 400, 10);
      let iter: drawing.PathIterator = new drawing.PathIterator(path);
      let verbStr: Array<string> = ["MOVE", "LINE", "QUAD", "CONIC", "CUBIC", "CLOSE", "DONE"];
      let pointCount: Array<number> = [1, 2, 3, 4, 4, 0, 0];
      let points: Array<common2D.Point> = [{ x: 0, y: 0 }, { x: 0, y: 0 }, { x: 0, y: 0 }, { x: 0, y: 0 }, { x: 0, y: 0 }, { x: 0, y: 0 }, { x: 0, y: 0 }, { x: 0, y: 0 }];
      let offset = 4;
      let verb = iter.next(points, offset);
      let verb1 = iter.next(points, offset);
      let verb2 = iter.next(points, offset);
      let verb3 = iter.next(points, offset);
      console.log("PathIteratorNext: verb = ", verb);
      console.log("PathIteratorNext: verb1 = ", verb1);
      console.log("PathIteratorNext: verb2 = ", verb2);
      console.log("PathIteratorNext: verb3 = ", verb3);
      let outputMessage: string = "pathIteratorNext: ";
      outputMessage += "verb3 =" + verbStr[verb3] + "; has " + pointCount[verb3] + " pairs: ";
      for (let j = 0; j < pointCount[verb3] + offset; j++) {
        outputMessage += "[" + points[j].x + ", " + points[j].y + "]";
      }
      console.log(outputMessage);
      canvas.drawPath(path);
    }

    {
      let pen = new drawing.Pen();
      pen.setAntiAlias(true);
      pen.setColor({ alpha: 0xFF, red: 0x00, green: 0xFF, blue: 0x00 });
      pen.setStrokeWidth(5.0);
      canvas.attachPen(pen);
      let path: drawing.Path = new drawing.Path();
      path.moveTo(100, 20);
      path.lineTo(200, 300);
      let iter: drawing.PathIterator = new drawing.PathIterator(path);
      let verbStr: Array<string> = ["MOVE", "LINE", "QUAD", "CONIC", "CUBIC", "CLOSE", "DONE"];
      let pointCount: Array<number> = [1, 2, 3, 4, 4, 0, 0];
      let points: Array<common2D.Point> = [{ x: 0, y: 0 }, { x: 0, y: 0 }, { x: 0, y: 0 }, { x: 0, y: 0 }];
      let offset = 0;
      let verb = iter.next(points, offset);
      console.log("PathIteratorNext: verb = ", verb);
      let outputMessage: string = "pathIteratorNext: ";
      outputMessage += "verb =" + verbStr[verb] + "; has " + pointCount[verb] + " pairs: ";
      for (let j = 0; j < pointCount[verb] + offset; j++) {
        outputMessage += "[" + points[j].x + ", " + points[j].y + "]";
      }
      console.log(outputMessage);
      canvas.drawPath(path);
    }

    {
      let pen = new drawing.Pen();
      pen.setAntiAlias(true);
      pen.setColor({ alpha: 0xFF, red: 0xFF, green: 0x00, blue: 0x00 });
      pen.setStrokeWidth(5.0);
      canvas.attachPen(pen);
      let path: drawing.Path = new drawing.Path();
      path.quadTo(100, 150, 100, 100);
      let iter: drawing.PathIterator = new drawing.PathIterator(path);
      let verbStr: Array<string> = ["MOVE", "LINE", "QUAD", "CONIC", "CUBIC", "CLOSE", "DONE"];
      let pointCount: Array<number> = [1, 2, 3, 4, 4, 0, 0];
      let points: Array<common2D.Point> = [{ x: 0, y: 0 }, { x: 0, y: 0 }, { x: 0, y: 0 }, { x: 0, y: 0 }, { x: 0, y: 0 }];
      let offset = 1;
      let verb = iter.next(points, offset);
      let verb1 = iter.next(points, offset);
      console.log("PathIteratorNext: verb = ", verb);
      console.log("PathIteratorNext: verb1 = ", verb1);
      let outputMessage: string = "pathIteratorNext: ";
      outputMessage += "verb1 =" + verbStr[verb1] + "; has " + pointCount[verb1] + " pairs: ";
      for (let j = 0; j < pointCount[verb1] + offset; j++) {
        outputMessage += "[" + points[j].x + ", " + points[j].y + "]";
      }
      console.log(outputMessage);
      canvas.drawPath(path);
    }
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let pen = new drawing.Pen();
    pen.setAntiAlias(true);
    pen.setColor({ alpha: 0xFF, red: 0xFF, green: 0x00, blue: 0x00 });
    pen.setStrokeWidth(5.0);
    canvas.attachPen(pen);
    let path: drawing.Path = new drawing.Path();
    path.moveTo(10, 20);
    path.lineTo(100, 200);
    let iter: drawing.PathIterator = new drawing.PathIterator(path);
    let verbStr: Array<string> = ["MOVE", "LINE", "QUAD", "CONIC", "CUBIC", "CLOSE", "DONE"];
    let pointCount: Array<number> = [1, 2, 3, 4, 4, 0, 0];
    let points: Array<common2D.Point> = [{ x: 0, y: 0 }, { x: 0, y: 0 }, { x: 0, y: 0 }, { x: 0, y: 0 }];
    let offset = 0;
    for (let i = 0; i < this.testCount_; i++) {
      let verb = iter.next(points, offset);
    }
    let verb = iter.next(points, offset);
    console.log("PathIteratorNext: verb = ", verb);
    let outputMessage: string = "pathIteratorNext: ";
    outputMessage += "verb =" + verbStr[verb] + "; has " + pointCount[verb] + " pairs: ";
    for (let j = 0; j < pointCount[verb] + offset; j++) {
      outputMessage += "[" + points[j].x + ", " + points[j].y + "]";
    }
    console.log(outputMessage);
    canvas.drawPath(path);
  }
}

export class PathIteratorPeek extends TestBase {
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
    let iter: drawing.PathIterator = new drawing.PathIterator(path);
    let points: Array<common2D.Point> = [{ x: 0, y: 0 }, { x: 0, y: 0 }, { x: 0, y: 0 }, { x: 0, y: 0 }];
    let verb = iter.next(points, 0);
    let verb1 = iter.peek();
    console.log("PathIteratorPeek: verb = ", verb);
    console.log("PathIteratorPeek: verb1 = ", verb1);
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
    let iter: drawing.PathIterator = new drawing.PathIterator(path);
    for (let i = 0; i < this.testCount_; i++) {
      let verb = iter.peek();
    }
    let verb = iter.peek();
    console.log("PathIteratorPeek: verb = ", verb);
    canvas.drawPath(path);
  }
}

export class PathIteratorHasNext extends TestBase {
  public constructor() {
    super();
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    {
      let pen = new drawing.Pen();
      pen.setAntiAlias(true);
      pen.setColor({ alpha: 0xFF, red: 0xFF, green: 0x00, blue: 0x00 });
      pen.setStrokeWidth(5.0);
      canvas.attachPen(pen);
      let path: drawing.Path = new drawing.Path();
      path.moveTo(0, 0);
      let iter: drawing.PathIterator = new drawing.PathIterator(path);
      let points: Array<common2D.Point> = [{ x: 0, y: 0 }, { x: 0, y: 0 }, { x: 0, y: 0 }, { x: 0, y: 0 }];
      let verb = iter.next(points, 0);
      console.log("PathIteratorHasNext: verb = ", verb);
      let hasNext = iter.hasNext();
      console.log("PathIteratorHasNext: hasNext = ", hasNext);
      canvas.drawPath(path);
    }

    {
      let pen = new drawing.Pen();
      pen.setAntiAlias(true);
      pen.setColor({ alpha: 0xFF, red: 0xFF, green: 0x00, blue: 0x00 });
      pen.setStrokeWidth(5.0);
      canvas.attachPen(pen);
      let path: drawing.Path = new drawing.Path();
      path.moveTo(0, 0);
      path.lineTo(100, 100);
      let iter: drawing.PathIterator = new drawing.PathIterator(path);
      let points: Array<common2D.Point> = [{ x: 0, y: 0 }, { x: 0, y: 0 }, { x: 0, y: 0 }, { x: 0, y: 0 }];
      let verb = iter.next(points, 0);
      console.log("PathIteratorHasNext: verb = ", verb);
      let hasNext = iter.hasNext();
      console.log("PathIteratorHasNext: hasNext = ", hasNext);
      canvas.drawPath(path);
    }
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
    let iter: drawing.PathIterator = new drawing.PathIterator(path);
    let points: Array<common2D.Point> = [{ x: 0, y: 0 }, { x: 0, y: 0 }, { x: 0, y: 0 }, { x: 0, y: 0 }];
    let verb = iter.next(points, 0);
    console.log("PathIteratorHasNext: verb = ", verb);
    for (let i = 0; i < this.testCount_; i++) {
      let hasNext = iter.hasNext();
    }
    let hasNext = iter.hasNext();
    console.log("PathIteratorHasNext: hasNext = ", hasNext);
    canvas.drawPath(path);
  }
}
