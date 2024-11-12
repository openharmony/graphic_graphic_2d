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
import globalThis from '../../utils/globalThis'
import image from '@ohos.multimedia.image';

export class PerformanceCanvasDrawRect extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE){
    super();
    this.styleType_ = styleType;
  }
  public OnTestPerformance(canvas: drawing.Canvas) {
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      canvas.drawRect(0, 0, 100, 100);
    }
  }
}

export class PerformanceCanvasDrawCircle extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE){
    super();
    this.styleType_ = styleType;
  }
  public OnTestPerformance(canvas: drawing.Canvas) {
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      canvas.drawCircle(200, 200, 100);
    }
  }
}

export class PerformanceCanvasDrawLine extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE){
    super();
    this.styleType_ = styleType;
  }
  public OnTestPerformance(canvas: drawing.Canvas) {
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      canvas.drawLine(0, 0, 100, 100);
    }
  }
}

export class PerformanceCanvasDrawPath extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE){
    super();
    this.styleType_ = styleType;
  }
  public OnTestPerformance(canvas: drawing.Canvas) {
    let path = new drawing.Path;
    path.arcTo(0, 0, 100, 100, 0, 180)
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      canvas.drawPath(path);
    }
  }
}

export class PerformanceCanvasDrawTextBlob extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE){
    super();
    this.styleType_ = styleType;
  }
  public OnTestPerformance(canvas: drawing.Canvas) {
    let text : string = 'textblob';
    const font : drawing.Font = new drawing.Font();
    font.setSize(50)
    const textBlob = drawing.TextBlob.makeFromString(text, font, drawing.TextEncoding.TEXT_ENCODING_UTF8);
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      canvas.drawTextBlob(textBlob, 100, 100);
    }
  }
}

export class PerformanceCanvasPoint extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE){
    super();
    this.styleType_ = styleType;
  }
  public OnTestPerformance(canvas: drawing.Canvas) {
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      canvas.drawPoint(50, 50);
    }
  }
}

export class PerformanceCanvasDrawRectT extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let rect: common2D.Rect = { left: 10, right: 200, top: 100, bottom: 300 };
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      canvas.drawRect(rect);
    }
  }
}

export class PerformanceCanvasDrawImage extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let samplingOptions = new drawing.SamplingOptions();
    let pixelMap: image.PixelMap = globalThis.getInstance().getPixelMap("test_1.jpg");
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      canvas.drawImage(pixelMap, 10, 20, samplingOptions);
    }
  }
}

export class PerformanceCanvasDrawImageRect extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let pixelMap: image.PixelMap = globalThis.getInstance().getPixelMap("test_1.jpg");
    let rect: common2D.Rect = { left: 10, right: 200, top: 100, bottom: 300 };
    let samplingOptions = new drawing.SamplingOptions();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      canvas.drawImageRect(pixelMap, rect, samplingOptions);
    }
  }
}

export class PerformanceCanvasDrawImageRectWithSrc extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let pixelMap: image.PixelMap = globalThis.getInstance().getPixelMap("test_1.jpg");
    let srcRect: common2D.Rect = { left: 10, right: 200, top: 100, bottom: 300 };
    let dstRect: common2D.Rect = { left: 10, right: 200, top: 100, bottom: 300 };
    let samplingOptions = new drawing.SamplingOptions();

    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      canvas.drawImageRectWithSrc(pixelMap, srcRect, dstRect, samplingOptions, drawing.SrcRectConstraint.STRICT);
    }
  }
}

export class PerformanceCanvasDrawColor extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let color: common2D.Color = { alpha: 255, red: 168, green: 48, blue: 176 };
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      canvas.drawColor(color, drawing.BlendMode.SRC_IN);
    }
  }
}

export class PerformanceCanvasDrawColorT extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let color = 0xFF0000FF;
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      canvas.drawColor(color, drawing.BlendMode.SRC_IN);
    }
  }
}

export class PerformanceCanvasDrawColorThree extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let color = 0xFF0000FF;
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      canvas.drawColor(255, 100, 255, 120, drawing.BlendMode.SRC_IN);
    }
  }
}

export class PerformanceCanvasDrawOval extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let rect: common2D.Rect = { left: 10, right: 200, top: 100, bottom: 300 };
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      canvas.drawOval(rect);
    }
  }
}

export class PerformanceCanvasDrawArc extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let rect: common2D.Rect = { left: 10, right: 200, top: 100, bottom: 300 };
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      canvas.drawArc(rect, 10, 100);
    }
  }
}

export class PerformanceCanvasDrawPoints extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let pointsArray = new Array<common2D.Point>();
    const point1: common2D.Point = { x: 200, y: 200 };
    const point2: common2D.Point = { x: 400, y: 200 };
    const point3: common2D.Point = { x: 100, y: 400 };
    const point4: common2D.Point = { x: 300, y: 400 };
    pointsArray.push(point1);
    pointsArray.push(point2);
    pointsArray.push(point3);
    pointsArray.push(point4);
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      canvas.drawPoints(pointsArray, drawing.PointMode.POINTS);
    }
  }
}

export class PerformanceCanvasDrawSingleCharacter extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    const font = new drawing.Font();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      canvas.drawSingleCharacter("你", font, 100, 100);
    }
  }
}

export class PerformanceCanvasDrawPixelMapMesh extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let pixelMap: image.PixelMap = globalThis.getInstance().getPixelMap("test_1.jpg");
    let verts: Array<number> = [0, 0, 50, 0, 410, 0, 0, 180, 50, 180, 410, 180, 0, 360, 50, 360, 410, 360]; // 18
    let colors: Array<number> = [0xFF0000FF, 0xFF00EEFF];
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      canvas.drawPixelMapMesh(pixelMap, 2, 2, verts, 0, null, 0);
    }
  }
}

export class PerformanceCanvasAttachPen extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let pen = new drawing.Pen();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      canvas.attachPen(pen);
    }
  }
}

export class PerformanceCanvasAttachBrush extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let brush = new drawing.Brush();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      canvas.attachBrush(brush);
    }
  }
}

export class PerformanceCanvasDetachPen extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      canvas.detachPen();
    }
  }
}

export class PerformanceCanvasDetachBrush extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      canvas.detachBrush();
    }
  }
}

export class PerformanceCanvasSaveLayer extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let rect: common2D.Rect = { left: 10, right: 200, top: 100, bottom: 300 };
    let brush = new drawing.Brush();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      canvas.saveLayer(rect, brush);
    }
  }
}

export class PerformanceCanvasClear extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let color: common2D.Color = { alpha: 255, red: 168, green: 48, blue: 176 };
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      canvas.clear(color);
    }
  }
}

export class PerformanceCanvasGetWidth extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      canvas.getWidth();
    }
  }
}

export class PerformanceCanvasGetHeight extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      canvas.getHeight();
    }
  }
}

export class PerformanceCanvasConcatMatrix extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let matrix = new drawing.Matrix();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      canvas.concatMatrix(matrix);
    }
  }
}

export class PerformanceCanvasSetMatrix extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let matrix = new drawing.Matrix();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      canvas.setMatrix(matrix);
    }
  }
}

export class PerformanceCanvasResetMatrix extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      canvas.resetMatrix();
    }
  }
}

export class PerformanceCanvasSave extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      canvas.save();
    }
  }
}

export class PerformanceCanvasRestore extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      canvas.restore();
    }
  }
}

export class PerformanceCanvasRestoreToCount extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      canvas.restoreToCount(1);
    }
  }
}

export class PerformanceCanvasGetSaveCount extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      canvas.getSaveCount();
    }
  }
}

export class PerformanceCanvasScale extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      canvas.scale(10, 20);
    }
  }
}

export class PerformanceCanvasSkew extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      canvas.skew(10, 20);
    }
  }
}

export class PerformanceCanvasRotate extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      canvas.rotate(30, 50, 50);
    }
  }
}

export class PerformanceCanvasTranslate extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      canvas.translate(50, 50);
    }
  }
}

export class PerformanceCanvasClipPath extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let path = new drawing.Path();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      canvas.clipPath(path, drawing.ClipOp.DIFFERENCE, false);
    }
  }
}

export class PerformanceCanvasClipRegion extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let region = new drawing.Region();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      canvas.clipRegion(region, drawing.ClipOp.DIFFERENCE);
    }
  }
}

export class PerformanceCanvasClipRect extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let rect: common2D.Rect = { left: 10, right: 200, top: 100, bottom: 300 };
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      canvas.clipRect(rect, drawing.ClipOp.DIFFERENCE, false);
    }
  }
}

export class PerformanceCanvasClipRoundRect extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let rect: common2D.Rect = { left: 10, right: 200, top: 100, bottom: 300 };
    let roundRect = new drawing.RoundRect(rect, 5, 10);
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      canvas.clipRoundRect(roundRect, drawing.ClipOp.DIFFERENCE, false);
    }
  }
}

export class PerformanceCanvasDrawNestedRoundRect extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let rect: common2D.Rect = { left: 0, right: 50, top: 0, bottom: 100 };
    let roundRect = new drawing.RoundRect(rect, 5, 10);
    let rectOth: common2D.Rect = { left: 0, right: 100, top: 0, bottom: 200 };
    let roundRectOth = new drawing.RoundRect(rectOth, 5, 10);
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      canvas.drawNestedRoundRect(roundRectOth, roundRect);
    }
  }
}

export class PerformanceCanvasDrawRegion extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let region = new drawing.Region();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      canvas.drawRegion(region);
    }
  }
}

export class PerformanceCanvasDrawRoundRect extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let rect: common2D.Rect = { left: 0, right: 50, top: 0, bottom: 100 };
    let roundRect = new drawing.RoundRect(rect, 5, 10);
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      canvas.drawRoundRect(roundRect);
    }
  }
}

export class PerformanceCanvasDrawBackground extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let brush = new drawing.Brush();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      canvas.drawBackground(brush);
    }
  }
}

export class PerformanceCanvasGetLocalClipBounds extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      canvas.getLocalClipBounds();
    }
  }
}

export class PerformanceCanvasGetTotalMatrix extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      canvas.getTotalMatrix();
    }
  }
}

export class PerformanceCanvasIsClipEmpty extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      canvas.isClipEmpty();
    }
  }
}

export class PerformanceCanvasDrawShadow extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let path = new drawing.Path();
    let point1: common2D.Point3d = { x: 100, y: 80, z: 80 };
    let point2: common2D.Point3d = { x: 200, y: 10, z: 40 };
    let color1: common2D.Color = { alpha: 0xff, red: 0, green: 0, blue: 0xff };
    let color2: common2D.Color = { alpha: 0xff, red: 0xff, green: 0, blue: 0 };
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      canvas.drawShadow(path, point1, point2, 5, color1, color2, drawing.ShadowFlag.NONE);
    }
  }
}

export class PerformanceSamplingOptionsConstructor extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      let samplingOptions = new drawing.SamplingOptions();
    }
  }
}