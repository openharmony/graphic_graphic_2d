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
import { TestBase, StyleType } from '../../pages/testbase';

export class PerformancePathMoveTo extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let path = new drawing.Path();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      path.moveTo(50, 50);
    }
  }
}

export class PerformancePathLineTo extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let path = new drawing.Path();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      path.lineTo(50, 50);
    }
  }
}

export class PerformancePathArcTo extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let path = new drawing.Path();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      path.arcTo(10, 20, 100, 150, 0, 90);
    }
  }
}

export class PerformancePathQuadTo extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let path = new drawing.Path();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      path.quadTo(10, 20, 100, 150);
    }
  }
}

export class PerformancePathConicTo extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let path = new drawing.Path();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      path.conicTo(10, 20, 100, 150, 10);
    }
  }
}

export class PerformancePathCubicTo extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let path = new drawing.Path();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      path.cubicTo(10, 100, 100, 0, 150, 100);
    }
  }
}

export class PerformancePathRMoveTo extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let path = new drawing.Path();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      path.rMoveTo(10, 100);
    }
  }
}

export class PerformancePathRLineTo extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let path = new drawing.Path();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      path.rLineTo(100, 100);
    }
  }
}

export class PerformancePathRQuadTo extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let path = new drawing.Path();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      path.rQuadTo(10, 20, 100, 150);
    }
  }
}

export class PerformancePathRConicTo extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let path = new drawing.Path();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      path.rConicTo(10, 20, 100, 150, 10);
    }
  }
}

export class PerformancePathRCubicTo extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let path = new drawing.Path();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      path.rCubicTo(10, 100, 100, 0, 150, 100);
    }
  }
}

export class PerformancePathAddPolygon extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let path = new drawing.Path();
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
      path.addPolygon(pointsArray, false);
    }
  }
}

export class PerformancePathOp extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let path = new drawing.Path();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      path.op(path, drawing.PathOp.DIFFERENCE);
    }
  }
}

export class PerformancePathAddArc extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let path = new drawing.Path();
    let rect: common2D.Rect = { left: 10, right: 200, top: 100, bottom: 300 };
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      path.addArc(rect, 10, 90);
    }
  }
}

export class PerformancePathAddCircle extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let path = new drawing.Path();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      path.addCircle(100, 200, 100, drawing.PathDirection.CLOCKWISE);
    }
  }
}

export class PerformancePathAddOval extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let path = new drawing.Path();
    let rect: common2D.Rect = { left: 10, right: 200, top: 100, bottom: 300 };
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      path.addOval(rect, 10, drawing.PathDirection.CLOCKWISE);
    }
  }
}

export class PerformancePathAddRect extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let path = new drawing.Path();
    let rect: common2D.Rect = { left: 10, right: 200, top: 100, bottom: 300 };
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      path.addRect(rect, drawing.PathDirection.CLOCKWISE);
    }
  }
}

export class PerformancePathAddRoundRect extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let path = new drawing.Path();
    let rect: common2D.Rect = { left: 10, right: 200, top: 100, bottom: 300 };
    let roundRect = new drawing.RoundRect(rect, 5, 10);
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      path.addRoundRect(roundRect, drawing.PathDirection.CLOCKWISE);
    }
  }
}

export class PerformancePathAddPath extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let path = new drawing.Path();
    let pathT = new drawing.Path();
    let matrix = new drawing.Matrix();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      path.addPath(pathT, matrix);
    }
  }
}

export class PerformancePathTransform extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let path = new drawing.Path();
    let matrix = new drawing.Matrix();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      path.transform(matrix);
    }
  }
}

export class PerformancePathContains extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let path = new drawing.Path();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      path.contains(10, 10);
    }
  }
}

export class PerformancePathSetFillType extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let path = new drawing.Path();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      path.setFillType(drawing.PathFillType.WINDING);
    }
  }
}

export class PerformancePathGetBounds extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let path = new drawing.Path();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      path.getBounds();
    }
  }
}

export class PerformancePathClose extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let path = new drawing.Path();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      path.close();
    }
  }
}

export class PerformancePathOffset extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let path = new drawing.Path();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      path.offset(100, 100);
    }
  }
}

export class PerformancePathReset extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let path = new drawing.Path();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      path.reset();
    }
  }
}

export class PerformancePathGetLength extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let path = new drawing.Path();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      path.getLength(true);
    }
  }
}

export class PerformancePathGetPositionAndTangent extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let path = new drawing.Path();
    let pointOne: common2D.Point = { x: 10, y: 20 };
    let pointTwo: common2D.Point = { x: 30, y: 40 };
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      path.getPositionAndTangent(false, 5, pointOne, pointTwo);
    }
  }
}

export class PerformancePathIsClosed extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let path = new drawing.Path();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      path.isClosed();
    }
  }
}

export class PerformancePathGetMatrix extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let path = new drawing.Path();
    let matrix = new drawing.Matrix();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      path.getMatrix(false, 10, matrix, drawing.PathMeasureMatrixFlags.GET_TANGENT_MATRIX);
    }
  }
}

export class PerformancePathBuildFromSvgString extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let path = new drawing.Path();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      path.buildFromSvgString("test");
    }
  }
}

export class PerformancePathConstructor extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      let path = new drawing.Path();
    }
  }
}

export class PerformancePathConstructorWithPath extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let path = new drawing.Path();
    this.StartPerformanceCaculate();
    for (let i = 0; i < this.testCount_; i++) {
      let pathOne = new drawing.Path(path);
    }
  }
}