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
import { TestBase } from '../../pages/testbase';
import { OHRandom } from '../../utils/OHRandom';

enum PathAllFunction {
  MOVE_TO = 0,
  LINE_TO,
  ARC_TO,
  QUAD_TO,
  CONIC_TO,
  CUBIC_TO,
  R_QUAD_TO,
  R_CONIC_TO,
  R_CUBIC_TO,
  ADD_POLYGON,
  OP,
  ADD_ARC,
  ADD_CIRCLE,
  ADD_OVAL,
  ADD_RECT,
  ADD_ROUND_RECT,
  ADD_PATH,
  TRANSFORM,
  CONTAINS,
  SET_FILL_TYPE,
  GET_BOUNDS,
  CLOSE,
  OFFSET,
  RESET,
  GET_LENGTH,
  GET_POSITION_AND_TANGENT,
  IS_CLOSED,
  GET_MATRIX,
  BUILD_FROM_SVG_STRING,
  MAX
}

enum PathOperationFunction {
  MOVE_TO = 0,
  LINE_TO,
  ARC_TO,
  QUAD_TO,
  CONIC_TO,
  CUBIC_TO,
  R_QUAD_TO,
  R_CONIC_TO,
  R_CUBIC_TO,
  ADD_POLYGON,
  OP,
  ADD_ARC,
  ADD_CIRCLE,
  ADD_OVAL,
  ADD_RECT,
  ADD_ROUND_RECT,
  ADD_PATH,
  TRANSFORM,
  CONTAINS,
  GET_BOUNDS,
  CLOSE,
  OFFSET,
  GET_LENGTH,
  GET_POSITION_AND_TANGENT,
  IS_CLOSED,
  GET_MATRIX,
  MAX
}

export class StabilityPathNew extends TestBase {

  /*
  @CaseID:SUB_BASIC_GRAPHICS_SPECIAL_STABLE_TS_DRAWING_PATH_0100
  @Description:Path new-置空循环
  @Step:
  1、运行脚本，循环执行hidumper命令查询内存
  2、执行自动化用例：
  path new-置空，循环调用1000次
  3、执行结束后等待3分钟
  4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
  @Result:
  1、程序运行正常，无crash
  2、内存平稳，没有持续增长，执行后内存回落到执行前；
  */
  public async OnTestStability(canvas: drawing.Canvas) {
    console.log("StabilityPathNew OnTestStability");
    for (let i = 0; i < this.testCount_; i++) {
      let path = new drawing.Path();
    }
  }
}

export class StabilityPathInvoke extends TestBase {
  private index = 0;
  private pointsNull: Array<common2D.Point> = new Array;
  private points: Array<common2D.Point> = new Array;
  private rect: common2D.Rect;
  private roundRect: drawing.RoundRect;
  private matrix = new drawing.Matrix();
  private pathAddPath = new drawing.Path();
  private pointGetPositionTset: common2D.Point = { x: 1, y: 2 };
  static functions_: Map<PathAllFunction,(pathInvoke: StabilityPathInvoke, path: drawing.Path) => void> =  new Map([
    [PathAllFunction.MOVE_TO, (pathInvoke: StabilityPathInvoke, path: drawing.Path) => {
      pathInvoke.PathMoveToTest(path)}],
    [PathAllFunction.LINE_TO, (pathInvoke: StabilityPathInvoke, path: drawing.Path) => {
      pathInvoke.PathLineToTest(path)}],
    [PathAllFunction.ARC_TO, (pathInvoke: StabilityPathInvoke, path: drawing.Path) => {
      pathInvoke.PathArcToTest(path)}],
    [PathAllFunction.QUAD_TO, (pathInvoke: StabilityPathInvoke, path: drawing.Path) => {
      pathInvoke.PathQuadToTest(path)}],
    [PathAllFunction.CONIC_TO, (pathInvoke: StabilityPathInvoke, path: drawing.Path) => {
      pathInvoke.PathConicToTest(path)}],
    [PathAllFunction.CUBIC_TO, (pathInvoke: StabilityPathInvoke, path: drawing.Path) => {
      pathInvoke.PathCubicTo(path)}],
    [PathAllFunction.R_QUAD_TO, (pathInvoke: StabilityPathInvoke, path: drawing.Path) => {
      pathInvoke.PathRQuadToTest(path)}],
    [PathAllFunction.R_CONIC_TO, (pathInvoke: StabilityPathInvoke, path: drawing.Path) => {
      pathInvoke.PathRConicToTest(path)}],
    [PathAllFunction.R_CUBIC_TO, (pathInvoke: StabilityPathInvoke, path: drawing.Path) => {
      pathInvoke.PathRCubicToTest(path)}],
    [PathAllFunction.ADD_POLYGON, (pathInvoke: StabilityPathInvoke, path: drawing.Path) => {
      pathInvoke.PathAddPolygonTest(path)}],
    [PathAllFunction.OP, (pathInvoke: StabilityPathInvoke, path: drawing.Path) => {
      pathInvoke.PathOpTest(path)}],
    [PathAllFunction.ADD_ARC, (pathInvoke: StabilityPathInvoke, path: drawing.Path) => {
      pathInvoke.PathAddArcTest(path)}],
    [PathAllFunction.ADD_CIRCLE, (pathInvoke: StabilityPathInvoke, path: drawing.Path) => {
      pathInvoke.PathAddCircleTest(path)}],
    [PathAllFunction.ADD_OVAL, (pathInvoke: StabilityPathInvoke, path: drawing.Path) => {
      pathInvoke.PathAddOvalTest(path)}],
    [PathAllFunction.ADD_RECT, (pathInvoke: StabilityPathInvoke, path: drawing.Path) => {
      pathInvoke.PathAddRectTest(path)}],
    [PathAllFunction.ADD_ROUND_RECT, (pathInvoke: StabilityPathInvoke, path: drawing.Path) => {
      pathInvoke.PathAddRoundRectTest(path)}],
    [PathAllFunction.ADD_PATH, (pathInvoke: StabilityPathInvoke, path: drawing.Path) => {
      pathInvoke.PathAddPathTest(path)}],
    [PathAllFunction.TRANSFORM, (pathInvoke: StabilityPathInvoke, path: drawing.Path) => {
      pathInvoke.PathTransformTest(path)}],
    [PathAllFunction.CONTAINS, (pathInvoke: StabilityPathInvoke, path: drawing.Path) => {
      pathInvoke.PathContainsTest(path)}],
    [PathAllFunction.SET_FILL_TYPE, (pathInvoke: StabilityPathInvoke, path: drawing.Path) => {
      pathInvoke.PathSetFillTypeTest(path)}],
    [PathAllFunction.GET_BOUNDS, (pathInvoke: StabilityPathInvoke, path: drawing.Path) => {
      pathInvoke.PathGetBoundsTest(path)}],
    [PathAllFunction.CLOSE, (pathInvoke: StabilityPathInvoke, path: drawing.Path) => {
      pathInvoke.PathCloseTest(path)}],
    [PathAllFunction.OFFSET, (pathInvoke: StabilityPathInvoke, path: drawing.Path) => {
      pathInvoke.PathOffesetTest(path)}],
    [PathAllFunction.RESET, (pathInvoke: StabilityPathInvoke, path: drawing.Path) => {
      pathInvoke.PathResetTest(path)}],
    [PathAllFunction.GET_LENGTH, (pathInvoke: StabilityPathInvoke, path: drawing.Path) => {
      pathInvoke.PathGetLengthTest(path)}],
    [PathAllFunction.GET_POSITION_AND_TANGENT, (pathInvoke: StabilityPathInvoke, path: drawing.Path) => {
      pathInvoke.PathGetPositionAndTangentTest(path)}],
    [PathAllFunction.IS_CLOSED, (pathInvoke: StabilityPathInvoke, path: drawing.Path) => {
      pathInvoke.PathIsClosedTest(path)}],
    [PathAllFunction.GET_MATRIX, (pathInvoke: StabilityPathInvoke, path: drawing.Path) => {
      pathInvoke.PathGetMatrixTest(path)}],
    [PathAllFunction.BUILD_FROM_SVG_STRING, (pathInvoke: StabilityPathInvoke, path: drawing.Path) => {
      pathInvoke.PathBuildFromSvgStringTest(path)}],
  ]);

  /*
  @CaseID:SUB_BASIC_GRAPHICS_SPECIAL_STABLE_TS_DRAWING_PATH_0200
  @Description:Path new-相关接口全调用-置空循环
  @Step:
  1、运行脚本，循环执行hidumper命令查询内存
  2、执行自动化用例：
  new path-相关接口全调用-置空，循环调用1000次
  3、执行结束后等待3分钟
  4、结束脚本运行，抓取的内存值转成折线图，观察内存变化"
  @Result:
  1、程序运行正常，无crash
  2、内存平稳，没有持续增长，执行后内存回落到执行前；
  */
  public async OnTestStability(canvas: drawing.Canvas) {
    console.log("StabilityPathInvoke OnTestStability");
    this.matrix.setScale(1, 2, 1, 1);
    this.rect = {
      left: OHRandom.GetRandomNum(0, this.width_),
      right: OHRandom.GetRandomNum(0, this.width_),
      top: OHRandom.GetRandomNum(0, this.height_),
      bottom: OHRandom.GetRandomNum(0, this.height_)
    };
    this.roundRect =
      new drawing.RoundRect(this.rect, OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.height_));
    this.points = [ {x: OHRandom.GetRandomNum(0, this.width_), y: OHRandom.GetRandomNum(0, this.width_)} ];
    for (let i = 0; i < this.testCount_; i++) {
      let path = new drawing.Path();
      this.index = i;
      for ( let j = 0; j < PathAllFunction.MAX; j++) {
        StabilityPathInvoke.functions_.get(j)(this, path);
      }
    }
  }

  PathMoveToTest(path: drawing.Path) {
    path.moveTo(OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.height_));
  }

  PathLineToTest(path: drawing.Path) {
    path.lineTo(OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.height_));
  }

  PathArcToTest(path: drawing.Path) {
    path.arcTo(OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.height_),
      OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.height_),
      OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.height_));
  }

  PathQuadToTest(path: drawing.Path) {
    path.quadTo(OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.height_),
      OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.height_));
  }

  PathConicToTest(path: drawing.Path) {
    path.conicTo(OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.height_),
      OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.height_),
      OHRandom.GetRandomNum(0, this.height_));
  }

  PathCubicTo(path: drawing.Path) {
    path.cubicTo(OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.height_),
      OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.height_),
      OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.height_));
  }

  PathRQuadToTest(path: drawing.Path) {
    path.rQuadTo(this.index, OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.height_),
      OHRandom.GetRandomNum(0, this.width_));
  }

  PathRConicToTest(path: drawing.Path) {
    path.rConicTo(OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.height_),
      OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.height_),
      OHRandom.GetRandomNum(0, this.width_));
  }

  PathRCubicToTest(path: drawing.Path) {
    path.rCubicTo(OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.height_),
      OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.height_),
      OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.height_));
  }

  PathAddPolygonTest(path: drawing.Path) {
    path.addPolygon(this.pointsNull, this.index % 2 == 0);
    path.addPolygon(this.points, this.index % 2 == 0);
  }

  PathOpTest(path: drawing.Path) {
    let pathOp = this.index % (drawing.PathOp.REVERSE_DIFFERENCE + 1);
    path.op(path, pathOp);
  }

  PathAddArcTest(path: drawing.Path) {
    path.addArc(this.rect, OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.height_));
  }

  PathAddCircleTest(path: drawing.Path) {
    let pathDirection = this.index % (drawing.PathDirection.COUNTER_CLOCKWISE + 1);
    path.addCircle(OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.height_),
      OHRandom.GetRandomNum(0, this.width_));
    path.addCircle(OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.height_),
      OHRandom.GetRandomNum(0, this.width_), pathDirection);
  }
  
  PathAddOvalTest(path: drawing.Path) {
    let pathDirection = this.index % (drawing.PathDirection.COUNTER_CLOCKWISE + 1);
    path.addOval(this.rect, OHRandom.GetRandomNum(0, this.width_));
    path.addOval(this.rect, OHRandom.GetRandomNum(0, this.width_), pathDirection);
  }

  PathAddRectTest(path: drawing.Path) {
    let pathDirection = this.index % (drawing.PathDirection.COUNTER_CLOCKWISE + 1);
    path.addRect(this.rect);
    path.addRect(this.rect, pathDirection);
  }
  
  PathAddRoundRectTest(path: drawing.Path) {
    let pathDirection = this.index % (drawing.PathDirection.COUNTER_CLOCKWISE + 1);
    path.addRoundRect(this.roundRect);
    path.addRoundRect(this.roundRect, pathDirection);
  }

  PathAddPathTest(path: drawing.Path) {
    path.addPath(this.pathAddPath);
    path.addPath(this.pathAddPath, this.matrix);
  }

  PathTransformTest(path: drawing.Path) {
    path.transform(this.matrix);
  }

  PathContainsTest(path: drawing.Path) {
    path.contains(OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.height_));
  }

  PathSetFillTypeTest(path: drawing.Path) {
    path.setFillType(this.index % (drawing.PathFillType.INVERSE_EVEN_ODD + 1));
  }

  PathGetBoundsTest(path: drawing.Path) {
    path.getBounds();
  }

  PathCloseTest(path: drawing.Path) {
    path.close();
  }

  PathOffesetTest(path: drawing.Path) {
    path.offset(OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.height_));
  }

  PathResetTest(path: drawing.Path) {
    path.reset();
  }

  PathGetLengthTest(path: drawing.Path) {
    let forceClosed = this.index % 2 == 0; // 2 this.index % 2 == 0 mean is forceClosed
    path.getLength(forceClosed);
  }
  
  PathGetPositionAndTangentTest(path: drawing.Path) {
    let forceClosed = this.index % 2 == 0; // 2 this.index % 2 == 0 mean is forceClosed
    try {
      path.getPositionAndTangent(forceClosed, 0,
        this.pointGetPositionTset,
        { x: OHRandom.GetRandomNum(0, this.width_), y: OHRandom.GetRandomNum(0, this.height_) });
    } catch (error) {
      console.log("currentError" + error);
    }
  }

  PathIsClosedTest(path: drawing.Path) {
    path.isClosed();
  }

  PathGetMatrixTest(path: drawing.Path) {
    let forceClosed = this.index % 2 == 0; // 2 this.index % 2 == 0 mean is forceClosed
    try {
      path.getMatrix(forceClosed, OHRandom.GetRandomNum(0, this.width_), this.matrix,
        drawing.PathMeasureMatrixFlags.GET_POSITION_MATRIX);
    } catch (error) {
      console.log("currentError" + error);
    }
  }

  PathBuildFromSvgStringTest(path: drawing.Path) {
    let svgStr: string = "M150 100 L75 300 L225 300 Z";
    path.buildFromSvgString(svgStr);
  }
}

export class StabilityPathRandInvoke extends TestBase {
  private index = 0;
  private rect: common2D.Rect;
  private pointsNull: Array<common2D.Point> = new Array;
  private points: Array<common2D.Point> = new Array;
  private roundRect: drawing.RoundRect;
  private pathAddPath = new drawing.Path();
  private matrix = new drawing.Matrix();
  private pointGetPositionTset: common2D.Point = { x: 1, y: 2 };
  static functions_: 
    Map<PathOperationFunction,(pathRandInvoke: StabilityPathRandInvoke, path: drawing.Path) => void> = new Map([
      [PathOperationFunction.MOVE_TO, (pathRandInvoke: StabilityPathRandInvoke, path: drawing.Path) => {
        pathRandInvoke.PathMoveToTest(path)}],
      [PathOperationFunction.LINE_TO, (pathRandInvoke: StabilityPathRandInvoke, path: drawing.Path) => {
        pathRandInvoke.PathLineToTest(path)}],
      [PathOperationFunction.ARC_TO, (pathRandInvoke: StabilityPathRandInvoke, path: drawing.Path) => {
        pathRandInvoke.PathArcToTest(path)}],
      [PathOperationFunction.QUAD_TO, (pathRandInvoke: StabilityPathRandInvoke, path: drawing.Path) => {
        pathRandInvoke.PathQuadToTest(path)}],
      [PathOperationFunction.CONIC_TO, (pathRandInvoke: StabilityPathRandInvoke, path: drawing.Path) => {
        pathRandInvoke.PathConicToTest(path)}],
      [PathOperationFunction.CUBIC_TO, (pathRandInvoke: StabilityPathRandInvoke, path: drawing.Path) => {
        pathRandInvoke.PathCubicTo(path)}],
      [PathOperationFunction.R_QUAD_TO, (pathRandInvoke: StabilityPathRandInvoke, path: drawing.Path) => {
        pathRandInvoke.PathRQuadToTest(path)}],
      [PathOperationFunction.R_CONIC_TO, (pathRandInvoke: StabilityPathRandInvoke, path: drawing.Path) => {
        pathRandInvoke.PathRConicToTest(path)}],
      [PathOperationFunction.R_CUBIC_TO, (pathRandInvoke: StabilityPathRandInvoke, path: drawing.Path) => {
        pathRandInvoke.PathRCubicToTest(path)}],
      [PathOperationFunction.ADD_POLYGON, (pathRandInvoke: StabilityPathRandInvoke, path: drawing.Path) => {
        pathRandInvoke.PathAddPolygonTest(path)}],
      [PathOperationFunction.OP, (pathRandInvoke: StabilityPathRandInvoke, path: drawing.Path) => {
        pathRandInvoke.PathOpTest(path)}],
      [PathOperationFunction.ADD_ARC, (pathRandInvoke: StabilityPathRandInvoke, path: drawing.Path) => {
        pathRandInvoke.PathAddArcTest(path)}],
      [PathOperationFunction.ADD_CIRCLE, (pathRandInvoke: StabilityPathRandInvoke, path: drawing.Path) => {
        pathRandInvoke.PathAddCircleTest(path)}],
      [PathOperationFunction.ADD_OVAL, (pathRandInvoke: StabilityPathRandInvoke, path: drawing.Path) => {
        pathRandInvoke.PathAddOvalTest(path)}],
      [PathOperationFunction.ADD_RECT, (pathRandInvoke: StabilityPathRandInvoke, path: drawing.Path) => {
        pathRandInvoke.PathAddRectTest(path)}],
      [PathOperationFunction.ADD_ROUND_RECT, (pathRandInvoke: StabilityPathRandInvoke, path: drawing.Path) => {
        pathRandInvoke.PathAddRoundRectTest(path)}],
      [PathOperationFunction.ADD_PATH, (pathRandInvoke: StabilityPathRandInvoke, path: drawing.Path) => {
        pathRandInvoke.PathAddPathTest(path)}],
      [PathOperationFunction.TRANSFORM, (pathRandInvoke: StabilityPathRandInvoke, path: drawing.Path) => {
        pathRandInvoke.PathTransformTest(path)}],
      [PathOperationFunction.CONTAINS, (pathRandInvoke: StabilityPathRandInvoke, path: drawing.Path) => {
        pathRandInvoke.PathContainsTest(path)}],
      [PathOperationFunction.GET_BOUNDS, (pathRandInvoke: StabilityPathRandInvoke, path: drawing.Path) => {
        pathRandInvoke.PathGetBoundsTest(path)}],
      [PathOperationFunction.CLOSE, (pathRandInvoke: StabilityPathRandInvoke, path: drawing.Path) => {
        pathRandInvoke.PathCloseTest(path)}],
      [PathOperationFunction.OFFSET, (pathRandInvoke: StabilityPathRandInvoke, path: drawing.Path) => {
        pathRandInvoke.PathOffesetTest(path)}],
      [PathOperationFunction.GET_LENGTH, (pathRandInvoke: StabilityPathRandInvoke, path: drawing.Path) => {
        pathRandInvoke.PathGetLengthTest(path)}],
      [PathOperationFunction.GET_POSITION_AND_TANGENT, (pathRandInvoke: StabilityPathRandInvoke, path: drawing.Path) => {
        pathRandInvoke.PathGetPositionAndTangentTest(path)}],
      [PathOperationFunction.IS_CLOSED, (pathRandInvoke: StabilityPathRandInvoke, path: drawing.Path) => {
        pathRandInvoke.PathIsClosedTest(path)}],
      [PathOperationFunction.GET_MATRIX, (pathRandInvoke: StabilityPathRandInvoke, path: drawing.Path) => {
        pathRandInvoke.PathGetMatrixTest(path)}]
  ]);

  /*
  @CaseID:SUB_BASIC_GRAPHICS_SPECIAL_STABLE_TS_DRAWING_PATH_0300
  @Description:Path相关操作接口随机循环调用
  @Step:
  1、运行脚本，循环执行hidumper命令查询内存
  2、执行自动化用例：
  new path
  path相关操作接口调用，循环调用1000次
  path置空
  3、执行结束后等待3分钟
  4、结束脚本运行，抓取的内存值转成折线图，观察内存变化"
  @Result:
  1、程序运行正常，无crash
  2、内存平稳，没有持续增长，执行后内存回落到执行前；
  */
  public async OnTestStability(canvas: drawing.Canvas) {
    console.log("StabilityPathRandInvoke OnTestStability");
    this.matrix.setScale(1, 2, 1, 1);
    this.rect = {
      left: OHRandom.GetRandomNum(0, this.width_),
      right: OHRandom.GetRandomNum(0, this.width_),
      top: OHRandom.GetRandomNum(0, this.height_),
      bottom: OHRandom.GetRandomNum(0, this.height_)
    };
    this.roundRect =
      new drawing.RoundRect(this.rect, OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.height_));
    this.points = [ {x: OHRandom.GetRandomNum(0, this.width_), y: OHRandom.GetRandomNum(0, this.width_)} ];

    let path = new drawing.Path();

    for (let i = 0; i < this.testCount_; i++) {
      this.index = i;
      StabilityPathRandInvoke.functions_.get(OHRandom.GetRandomNum(0, PathOperationFunction.MAX - 1))(this, path);
    }
  }

  PathMoveToTest(path: drawing.Path) {
    path.moveTo(OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.height_));
  }

  PathLineToTest(path: drawing.Path) {
    path.lineTo(OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.height_));
  }

  PathArcToTest(path: drawing.Path) {
    path.arcTo(OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.height_),
      OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.height_),
      OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.height_));
  }

  PathQuadToTest(path: drawing.Path) {
    path.quadTo(OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.height_),
      OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.height_));
  }

  PathConicToTest(path: drawing.Path) {
    path.conicTo(OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.height_),
      OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.height_),
      OHRandom.GetRandomNum(0, this.height_));
  }

  PathCubicTo(path: drawing.Path) {
    path.cubicTo(OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.height_),
      OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.height_),
      OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.height_));
  }

  PathRQuadToTest(path: drawing.Path) {
    path.rQuadTo(this.index, OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.height_),
      OHRandom.GetRandomNum(0, this.width_));
  }

  PathRConicToTest(path: drawing.Path) {
    path.rConicTo(OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.height_),
      OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.height_),
      OHRandom.GetRandomNum(0, this.width_));
  }

  PathRCubicToTest(path: drawing.Path) {
    path.rCubicTo(OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.height_),
      OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.height_),
      OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.height_));
  }

  PathAddPolygonTest(path: drawing.Path) {
    path.addPolygon(this.pointsNull, this.index % 2 == 0);
    path.addPolygon(this.points, this.index % 2 == 0);
  }

  PathOpTest(path: drawing.Path) {
    let pathOp = this.index % (drawing.PathOp.REVERSE_DIFFERENCE + 1);
    path.op(path, pathOp);
  }

  PathAddArcTest(path: drawing.Path) {
    path.addArc(this.rect, OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.height_));
  }

  PathAddCircleTest(path: drawing.Path) {
    let pathDirection = this.index % (drawing.PathDirection.COUNTER_CLOCKWISE + 1);
    path.addCircle(OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.height_),
      OHRandom.GetRandomNum(0, this.width_));
    path.addCircle(OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.height_),
      OHRandom.GetRandomNum(0, this.width_), pathDirection);
  }
  
  PathAddOvalTest(path: drawing.Path) {
    let pathDirection = this.index % (drawing.PathDirection.COUNTER_CLOCKWISE + 1);
    path.addOval(this.rect, OHRandom.GetRandomNum(0, this.width_));
    path.addOval(this.rect, OHRandom.GetRandomNum(0, this.width_), pathDirection);
  }

  PathAddRectTest(path: drawing.Path) {
    let pathDirection = this.index % (drawing.PathDirection.COUNTER_CLOCKWISE + 1);
    path.addRect(this.rect);
    path.addRect(this.rect, pathDirection);
  }
  
  PathAddRoundRectTest(path: drawing.Path) {
    let pathDirection = this.index % (drawing.PathDirection.COUNTER_CLOCKWISE + 1);
    path.addRoundRect(this.roundRect);
    path.addRoundRect(this.roundRect, pathDirection);
  }

  PathAddPathTest(path: drawing.Path) {
    path.addPath(this.pathAddPath);
    path.addPath(this.pathAddPath, this.matrix);
  }

  PathTransformTest(path: drawing.Path) {
    path.transform(this.matrix);
  }

  PathContainsTest(path: drawing.Path) {
    path.contains(OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.height_));
  }

  PathGetBoundsTest(path: drawing.Path) {
    path.getBounds();
  }

  PathCloseTest(path: drawing.Path) {
    path.close();
  }

  PathOffesetTest(path: drawing.Path) {
    path.offset(OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.height_));
  }

  PathGetLengthTest(path: drawing.Path) {
    let forceClosed = this.index % 2 == 0; // 2 this.index % 2 == 0 mean is forceClosed
    path.getLength(forceClosed);
  }
  
  PathGetPositionAndTangentTest(path: drawing.Path) {
    let forceClosed = this.index % 2 == 0; // 2 this.index % 2 == 0 mean is forceClosed
    try {
      path.getPositionAndTangent(forceClosed, 0,
        this.pointGetPositionTset,
        { x: OHRandom.GetRandomNum(0, this.width_), y: OHRandom.GetRandomNum(0, this.height_) });
    } catch (error) {
      console.log("currentError" + error);
    }
  }

  PathIsClosedTest(path: drawing.Path) {
    path.isClosed();
  }

  PathGetMatrixTest(path: drawing.Path) {
    let forceClosed = this.index % 2 == 0; // 2 this.index % 2 == 0 mean is forceClosed
    try {
      path.getMatrix(forceClosed, OHRandom.GetRandomNum(0, this.width_), this.matrix,
        drawing.PathMeasureMatrixFlags.GET_POSITION_MATRIX);
    } catch (error) {
      console.log("currentError" + error);
    }
  }
}