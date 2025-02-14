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

export class PathEffectCreateDiscretePathEffect extends TestBase {
  public constructor() {
    super();
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    {
      let pen = new drawing.Pen();
      pen.setColor({ alpha: 0xFF, red: 0xFF, green: 0x00, blue: 0x00 });
      pen.setAntiAlias(true);
      pen.setStrokeWidth(5.0);
      let rect: common2D.Rect = { left: 20, top: 20, right: 100, bottom: 100 };
      let pathEffect: drawing.PathEffect = drawing.PathEffect.createDiscretePathEffect(100, -50, 0);
      pen.setPathEffect(pathEffect);
      canvas.attachPen(pen);
      canvas.drawRect(rect);
      canvas.detachPen();
    }

    {
      let pen = new drawing.Pen();
      pen.setColor({ alpha: 0xFF, red: 0x00, green: 0xFF, blue: 0x00 });
      pen.setAntiAlias(true);
      pen.setStrokeWidth(5.0);
      let rect: common2D.Rect = { left: 150, top: 20, right: 300, bottom: 100 };
      let pathEffect: drawing.PathEffect = drawing.PathEffect.createDiscretePathEffect(100, 0, 0);
      pen.setPathEffect(pathEffect);
      canvas.attachPen(pen);
      canvas.drawRect(rect);
      canvas.detachPen();
    }

    {
      let pen = new drawing.Pen();
      pen.setColor({ alpha: 0xFF, red: 0x00, green: 0x00, blue: 0xFF });
      pen.setAntiAlias(true);
      pen.setStrokeWidth(5.0);
      let rect: common2D.Rect = { left: 20, top: 150, right: 100, bottom: 300 };
      let pathEffect: drawing.PathEffect = drawing.PathEffect.createDiscretePathEffect(100, 50, 0);
      pen.setPathEffect(pathEffect);
      canvas.attachPen(pen);
      canvas.drawRect(rect);
      canvas.detachPen();
    }

    {
      let pen = new drawing.Pen();
      pen.setColor({ alpha: 0xFF, red: 0xFF, green: 0x00, blue: 0xFF });
      pen.setAntiAlias(true);
      pen.setStrokeWidth(5.0);
      let rect: common2D.Rect = { left: 150, top: 150, right: 300, bottom: 300 };
      let pathEffect: drawing.PathEffect = drawing.PathEffect.createDiscretePathEffect(100, 50, -10);
      pen.setPathEffect(pathEffect);
      canvas.attachPen(pen);
      canvas.drawRect(rect);
      canvas.detachPen();
    }

    {
      let pen = new drawing.Pen();
      pen.setColor({ alpha: 0xFF, red: 0x00, green: 0xFF, blue: 0xFF });
      pen.setAntiAlias(true);
      pen.setStrokeWidth(5.0);
      let rect: common2D.Rect = { left: 200, top: 350, right: 300, bottom: 500 };
      let pathEffect: drawing.PathEffect = drawing.PathEffect.createDiscretePathEffect(100, 50, 10);
      pen.setPathEffect(pathEffect);
      canvas.attachPen(pen);
      canvas.drawRect(rect);
      canvas.detachPen();
    }
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let pen = new drawing.Pen();
    pen.setColor({ alpha: 0xFF, red: 0xFF, green: 0x00, blue: 0x00 });
    pen.setAntiAlias(true);
    pen.setStrokeWidth(5.0);
    let rect: common2D.Rect = { left: 200, top: 350, right: 300, bottom: 500 };
    for (let i = 0; i < this.testCount_; i++) {
      let pathEffect: drawing.PathEffect = drawing.PathEffect.createDiscretePathEffect(100, 50, 10);
    }
    let pathEffect: drawing.PathEffect = drawing.PathEffect.createDiscretePathEffect(100, 50, 10);
    pen.setPathEffect(pathEffect);
    canvas.attachPen(pen);
    canvas.drawRect(rect);
    canvas.detachPen();
  }
}

export class PathEffectCreateComposePathEffect extends TestBase {
  public constructor() {
    super();
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    try {
      {
        let pen = new drawing.Pen();
        pen.setColor({ alpha: 0xFF, red: 0xFF, green: 0x00, blue: 0x00 });
        pen.setAntiAlias(true);
        pen.setStrokeWidth(5.0);
        let radius = 100;
        let pathEffect1: drawing.PathEffect = drawing.PathEffect.createCornerPathEffect(radius);
        let pathEffect2: drawing.PathEffect = drawing.PathEffect.createDiscretePathEffect(100, 50, 0);
        let pathEffect3: drawing.PathEffect = drawing.PathEffect.createComposePathEffect(pathEffect1, pathEffect2);
        pen.setPathEffect(pathEffect3);
        canvas.attachPen(pen);
        let rect: common2D.Rect = { left: 100, top: 100, right: 400, bottom: 400 };
        canvas.drawRect(rect);
        canvas.detachPen();
      }

      {
        let pen = new drawing.Pen();
        pen.setColor({ alpha: 0xFF, red: 0xFF, green: 0x00, blue: 0x00 });
        pen.setAntiAlias(true);
        pen.setStrokeWidth(5.0);
        let pathEffect2: drawing.PathEffect = drawing.PathEffect.createDiscretePathEffect(100, 50, 0);
        let pathEffect3: drawing.PathEffect = drawing.PathEffect.createComposePathEffect(null, pathEffect2);
        pen.setPathEffect(pathEffect3);
        canvas.attachPen(pen);
        let rect: common2D.Rect = { left: 100, top: 100, right: 400, bottom: 400 };
        canvas.drawRect(rect);
        canvas.detachPen();
      }

      {
        let pen = new drawing.Pen();
        pen.setColor({ alpha: 0xFF, red: 0xFF, green: 0x00, blue: 0x00 });
        pen.setAntiAlias(true);
        pen.setStrokeWidth(5.0);
        let radius = 100;
        let pathEffect1: drawing.PathEffect = drawing.PathEffect.createCornerPathEffect(radius);
        let pathEffect3: drawing.PathEffect = drawing.PathEffect.createComposePathEffect(pathEffect1, null);
        pen.setPathEffect(pathEffect3);
        canvas.attachPen(pen);
        let rect: common2D.Rect = { left: 100, top: 100, right: 400, bottom: 400 };
        canvas.drawRect(rect);
        canvas.detachPen();
      }
    } catch (exc) {
      console.log("PathEffectCreateComposePathEffect exception: " + exc);
    }
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let pen = new drawing.Pen();
    pen.setColor({ alpha: 0xFF, red: 0xFF, green: 0x00, blue: 0x00 });
    pen.setAntiAlias(true);
    pen.setStrokeWidth(5.0);
    let radius = 100;
    let rect: common2D.Rect = { left: 100, top: 100, right: 400, bottom: 400 };
    let pathEffect1: drawing.PathEffect = drawing.PathEffect.createCornerPathEffect(radius);
    let pathEffect2: drawing.PathEffect = drawing.PathEffect.createDiscretePathEffect(100, 50, 0);
    for (let i = 0; i < this.testCount_; i++) {
      let pathEffect3: drawing.PathEffect = drawing.PathEffect.createComposePathEffect(pathEffect1, pathEffect2);
    }
    let pathEffect3: drawing.PathEffect = drawing.PathEffect.createComposePathEffect(pathEffect1, pathEffect2);
    pen.setPathEffect(pathEffect3);
    canvas.attachPen(pen);
    canvas.drawRect(rect);
    canvas.detachPen();
  }
}

export class PathEffectCreatePathDashEffect extends TestBase {
  public constructor() {
    super();
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    try {
      {
        let pen = new drawing.Pen();
        pen.setAntiAlias(true);
        pen.setColor({ alpha: 0xFF, red: 0xFF, green: 0x00, blue: 0x00 });
        pen.setStrokeWidth(5.0);
        pen.setAntiAlias(true);
        const path = new drawing.Path();
        path.moveTo(10, 10);
        path.lineTo(200, 200);
        let rect: common2D.Rect = { left: 0, top: 0, right: 10, bottom: 30 };
        path.addRect(rect, drawing.PathDirection.CLOCKWISE);
        let patheffect1: drawing.PathEffect = drawing.PathEffect.createPathDashEffect(path, 40, 10, drawing.PathDashStyle.TRANSLATE);
        pen.setPathEffect(patheffect1);
        let rect1: common2D.Rect = { left: 10, top: 150, right: 100, bottom: 250 };
        canvas.attachPen(pen);
        canvas.drawRect(rect1);
        canvas.detachPen();
      }

      {
        let pen = new drawing.Pen();
        pen.setAntiAlias(true);
        pen.setColor({ alpha: 0xFF, red: 0x00, green: 0xFF, blue: 0x00 });
        pen.setStrokeWidth(5.0);
        pen.setAntiAlias(true);
        const path = new drawing.Path();
        path.moveTo(10, 10);
        path.lineTo(200, 200);
        let rect: common2D.Rect = { left: 0, top: 0, right: 10, bottom: 30 };
        path.addRect(rect, drawing.PathDirection.CLOCKWISE);
        let patheffect: drawing.PathEffect = drawing.PathEffect.createPathDashEffect(path, 40, 0, drawing.PathDashStyle.ROTATE);
        pen.setPathEffect(patheffect);
        let rect1: common2D.Rect = { left: 150, top: 150, right: 250, bottom: 250 };
        canvas.attachPen(pen);
        canvas.drawRect(rect1);
        canvas.detachPen();
      }

      {
        let pen = new drawing.Pen();
        pen.setAntiAlias(true);
        pen.setColor({ alpha: 0xFF, red: 0x00, green: 0x00, blue: 0xFF });
        pen.setStrokeWidth(5.0);
        pen.setAntiAlias(true);
        const path = new drawing.Path();
        path.moveTo(10, 10);
        path.lineTo(200, 200);
        let rect: common2D.Rect = { left: 0, top: 0, right: 10, bottom: 30 };
        path.addRect(rect, drawing.PathDirection.CLOCKWISE);
        let patheffect: drawing.PathEffect = drawing.PathEffect.createPathDashEffect(path, 40, 0, drawing.PathDashStyle.MORPH);
        pen.setPathEffect(patheffect);
        let rect1: common2D.Rect = { left: 250, top: 250, right: 400, bottom: 400 };
        canvas.attachPen(pen);
        canvas.drawRect(rect1);
        canvas.detachPen();
      }

      {
        let pen = new drawing.Pen();
        pen.setAntiAlias(true);
        pen.setColor({ alpha: 0xFF, red: 0xFF, green: 0x00, blue: 0x00 });
        pen.setStrokeWidth(5.0);
        pen.setAntiAlias(true);
        const path = new drawing.Path();
        path.moveTo(10, 10);
        path.lineTo(200, 200);
        let rect: common2D.Rect = { left: 0, top: 0, right: 10, bottom: 30 };
        path.addRect(rect, drawing.PathDirection.CLOCKWISE);
        let patheffect1: drawing.PathEffect = drawing.PathEffect.createPathDashEffect(path, 0, 0, drawing.PathDashStyle.TRANSLATE);
        pen.setPathEffect(patheffect1);
        let rect1: common2D.Rect = { left: 10, top: 10, right: 100, bottom: 100 };
        canvas.attachPen(pen);
        canvas.drawRect(rect1);
        canvas.detachPen();
      }

      {
        let pen = new drawing.Pen();
        pen.setAntiAlias(true);
        pen.setColor({ alpha: 0xFF, red: 0xFF, green: 0x00, blue: 0x00 });
        pen.setStrokeWidth(5.0);
        pen.setAntiAlias(true);
        const path = new drawing.Path();
        path.moveTo(10, 10);
        path.lineTo(200, 200);
        let rect: common2D.Rect = { left: 0, top: 0, right: 10, bottom: 30 };
        path.addRect(rect, drawing.PathDirection.CLOCKWISE);
        let patheffect1: drawing.PathEffect = drawing.PathEffect.createPathDashEffect(path, -10, 0, drawing.PathDashStyle.TRANSLATE);
        pen.setPathEffect(patheffect1);
        let rect1: common2D.Rect = { left: 150, top: 10, right: 250, bottom: 100 };
        canvas.attachPen(pen);
        canvas.drawRect(rect1);
        canvas.detachPen();
      }
    } catch (exc) {
      console.log("PathEffectCreatePathDashEffect exception: " + exc);
    }
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let pen = new drawing.Pen();
    pen.setAntiAlias(true);
    pen.setColor({ alpha: 0xFF, red: 0x00, green: 0x00, blue: 0xFF });
    pen.setStrokeWidth(5.0);
    canvas.attachPen(pen);
    pen.setAntiAlias(true);
    const path = new drawing.Path();
    path.moveTo(10, 10);
    path.lineTo(200, 200);
    let rect: common2D.Rect = { left: 0, top: 0, right: 10, bottom: 30 };
    path.addRect(rect, drawing.PathDirection.CLOCKWISE);
    for (let i = 0; i < this.testCount_; i++) {
      let patheffect: drawing.PathEffect = drawing.PathEffect.createPathDashEffect(path, 40, 0, drawing.PathDashStyle.TRANSLATE);
    }
    let patheffect: drawing.PathEffect = drawing.PathEffect.createPathDashEffect(path, 40, 0, drawing.PathDashStyle.TRANSLATE);
    pen.setPathEffect(patheffect);
    let rect1: common2D.Rect = { left: 250, top: 250, right: 400, bottom: 400 };
    canvas.attachPen(pen);
    canvas.drawRect(rect1);
    canvas.detachPen();
  }
}

export class PathEffectCreateSumPathEffect extends TestBase {
  public constructor() {
    super();
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    try {
      {
        let pen = new drawing.Pen();
        pen.setColor({ alpha: 0xFF, red: 0xFF, green: 0x00, blue: 0x00 });
        pen.setAntiAlias(true);
        pen.setStrokeWidth(5.0);
        const path = new drawing.Path();
        path.moveTo(10, 10);
        path.lineTo(200, 200);
        let rect: common2D.Rect = { left: 0, top: 0, right: 10, bottom: 30 };
        path.addRect(rect, drawing.PathDirection.CLOCKWISE);
        let pathEffect1: drawing.PathEffect = drawing.PathEffect.createPathDashEffect(path, 20, 30, drawing.PathDashStyle.TRANSLATE);
        let pathEffect2: drawing.PathEffect = drawing.PathEffect.createPathDashEffect(path, 30, 80, drawing.PathDashStyle.ROTATE);
        let pathEffect3: drawing.PathEffect = drawing.PathEffect.createSumPathEffect(pathEffect1, pathEffect2);
        pen.setPathEffect(pathEffect3);
        let rect1: common2D.Rect = { left: 250, top: 250, right: 400, bottom: 400 };
        canvas.attachPen(pen);
        canvas.drawRect(rect1);
        canvas.detachPen();
      }

      {
        let pen = new drawing.Pen();
        pen.setColor({ alpha: 0xFF, red: 0xFF, green: 0x00, blue: 0x00 });
        pen.setAntiAlias(true);
        pen.setStrokeWidth(5.0);
        const path = new drawing.Path();
        path.moveTo(10, 10);
        path.lineTo(200, 200);
        let rect: common2D.Rect = { left: 0, top: 0, right: 10, bottom: 30 };
        path.addRect(rect, drawing.PathDirection.CLOCKWISE);
        let pathEffect2: drawing.PathEffect = drawing.PathEffect.createPathDashEffect(path, 30, 80, drawing.PathDashStyle.ROTATE);
        let pathEffect3: drawing.PathEffect = drawing.PathEffect.createSumPathEffect(null, pathEffect2);
        pen.setPathEffect(pathEffect3);
        let rect1: common2D.Rect = { left: 250, top: 250, right: 400, bottom: 400 };
        canvas.attachPen(pen);
        canvas.drawRect(rect1);
        canvas.detachPen();
      }

      {
        let pen = new drawing.Pen();
        pen.setColor({ alpha: 0xFF, red: 0xFF, green: 0x00, blue: 0x00 });
        pen.setAntiAlias(true);
        pen.setStrokeWidth(5.0);
        const path = new drawing.Path();
        path.moveTo(10, 10);
        path.lineTo(200, 200);
        let rect: common2D.Rect = { left: 0, top: 0, right: 10, bottom: 30 };
        path.addRect(rect, drawing.PathDirection.CLOCKWISE);
        let pathEffect1: drawing.PathEffect = drawing.PathEffect.createPathDashEffect(path, 20, 30, drawing.PathDashStyle.TRANSLATE);
        let pathEffect3: drawing.PathEffect = drawing.PathEffect.createSumPathEffect(pathEffect1, null);
        pen.setPathEffect(pathEffect3);
        let rect1: common2D.Rect = { left: 250, top: 250, right: 400, bottom: 400 };
        canvas.attachPen(pen);
        canvas.drawRect(rect1);
        canvas.detachPen();
      }
    } catch (exc) {
      console.log("PathEffectCreateSumPathEffect exception: " + exc);
    }
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let pen = new drawing.Pen();
    pen.setColor({ alpha: 0xFF, red: 0xFF, green: 0x00, blue: 0x00 });
    pen.setAntiAlias(true);
    pen.setStrokeWidth(5.0);
    const path = new drawing.Path();
    path.moveTo(10, 10);
    path.lineTo(200, 200);
    let rect: common2D.Rect = { left: 0, top: 0, right: 10, bottom: 30 };
    path.addRect(rect, drawing.PathDirection.CLOCKWISE);
    let pathEffect1: drawing.PathEffect = drawing.PathEffect.createPathDashEffect(path, 20, 30, drawing.PathDashStyle.TRANSLATE);
    let pathEffect2: drawing.PathEffect = drawing.PathEffect.createPathDashEffect(path, 30, 80, drawing.PathDashStyle.ROTATE);
    for (let i = 0; i < this.testCount_; i++) {
      let pathEffect3: drawing.PathEffect = drawing.PathEffect.createSumPathEffect(pathEffect1, pathEffect2);
    }
    let pathEffect3: drawing.PathEffect = drawing.PathEffect.createSumPathEffect(pathEffect1, pathEffect2);
    pen.setPathEffect(pathEffect3);
    let rect1: common2D.Rect = { left: 250, top: 250, right: 400, bottom: 400 };
    canvas.attachPen(pen);
    canvas.drawRect(rect1);
    canvas.detachPen();
  }
}
