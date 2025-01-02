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
import { TestBase, StyleType } from '../../pages/testbase';
import { N, OHRandom, W } from '../../utils/OHRandom';
import { image } from '@kit.ImageKit';

enum CanvasAllFunction {
  DRAW_RECT = 0,
  DRAW_ROUND_RECT,
  DRAW_NESTED_ROUND_RECT,
  DRAW_BACKGROUND,
  DRAW_SHADOW,
  DRAW_CIRCLE,
  DRAW_IMAGE,
  DRAW_IMAGE_RECT,
  DRAW_IMAGE_RECT_WITH_SRC,
  DRAW_COLOR,
  DRAW_OVAL,
  DRAW_ARC,
  DRAW_POINT,
  DRAW_POINTS,
  DRAW_PATH,
  DRAW_LINE,
  DRAW_SINGLE_CHARACTER,
  DRAW_TEXT_BLOB,
  DRAW_PIXEL_MAP_MESH,
  DRAW_REGION,
  ATTACH_PEN,
  ATTACH_BRUSH,
  DETACH_PEN,
  DETACH_BRUSH,
  SAVE,
  SAVE_LAYER,
  CLEAR,
  RESTORE,
  RESTORE_TO_COUNT,
  GET_WIDTH,
  GET_HEIGHT,
  GET_LOCAL_CLIP_BOUNDS,
  GET_TOTAL_MATRIX,
  SCALE,
  SKEW,
  ROTATE,
  TRANSLATE,
  CLIP_PATH,
  CLIP_RECT,
  CONCAT_MATRIX,
  CLIP_REGION,
  CLIP_ROUND_RECT,
  IS_CLIP_EMPTY,
  SET_MATRIX,
  RESET_MATRIX,
  MAX
}
enum CanvasOperationFunction {
  DRAW_RECT = 0,
  DRAW_ROUND_RECT,
  DRAW_NESTED_ROUND_RECT,
  DRAW_BACKGROUND,
  DRAW_SHADOW,
  DRAW_CIRCLE,
  DRAW_IMAGE,
  DRAW_IMAGE_RECT,
  DRAW_IMAGE_RECT_WITH_SRC,
  DRAW_COLOR,
  DRAW_OVAL,
  DRAW_ARC,
  DRAW_POINT,
  DRAW_POINTS,
  DRAW_PATH,
  DRAW_LINE,
  DRAW_SINGLE_CHARACTER,
  DRAW_TEXT_BLOB,
  DRAW_PIXEL_MAP_MESH,
  DRAW_REGION,
  SAVE,
  SAVE_LAYER,
  CLEAR,
  RESTORE,
  RESTORE_TO_COUNT,
  GET_WIDTH,
  GET_HEIGHT,
  GET_LOCAL_CLIP_BOUNDS,
  GET_TOTAL_MATRIX,
  SCALE,
  SKEW,
  ROTATE,
  TRANSLATE,
  CLIP_PATH,
  CLIP_RECT,
  CONCAT_MATRIX,
  CLIP_REGION,
  CLIP_ROUND_RECT,
  IS_CLIP_EMPTY,
  RESET_MATRIX,
  MAX
}

async function createPixelsMap(width: number, height: number): Promise<image.PixelMap> {
  const color: ArrayBuffer = new ArrayBuffer(width * height * 4);
  let opts: image.InitializationOptions = {
    editable: true,
    pixelFormat: 3,
    size: { height: height, width: width },
    alphaType: image.AlphaType.PREMUL,
    scaleMode: image.ScaleMode.FIT_TARGET_SIZE,
    srcPixelFormat: image.PixelMapFormat.BGRA_8888
  }
  let pixelsMap = await image.createPixelMap(color, opts);
  if (pixelsMap == null || pixelsMap == undefined) {
    console.log("canvas: createPixelMapSync failed")
  }
  return pixelsMap;
}

export class StabilityCanvasNew extends TestBase {
  /*
  @CaseID:SUB_BASIC_GRAPHICS_SPECIAL_STABLE_TS_DRAWING_CANVAS_0100
  @Description:Canvas new-置空循环
  @Step:
  1、运行脚本，循环执行hidumper命令查询内存
  2、执行自动化用例：new canvas-置空，循环调用1000次
  3、执行结束后等待3分钟
  4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
  @Result:
  1、程序运行正常，无crash
  2、内存平稳，没有持续增长，执行后内存回落到执行前；
  */
  public async OnTestStability(canvas: drawing.Canvas) {
    console.log("StabilityCanvasNew OnTestStability");
    let pixelsMap = await createPixelsMap(this.width_, this.height_);
    for (let i = 0; i < this.testCount_; i++) {
      let canvas = new drawing.Canvas(pixelsMap);
    }
  }
}

export class StabilityCanvasInvoke extends TestBase {
  private rect: common2D.Rect = null;
  private rect1: common2D.Rect = null;
  private roundRect: drawing.RoundRect = null;
  private outRoundRect: drawing.RoundRect = null;
  private brush: drawing.Brush = null;
  private path: drawing.Path = null;
  private samplingOptions: drawing.SamplingOptions = null;
  private points: Array<common2D.Point> = null;
  private font: drawing.Font = null;
  private region: drawing.Region = null;
  private pen: drawing.Pen = null;
  private matrix: drawing.Matrix = null;
  private vertices: Array<number> = null;
  private colors: Array<number> = null;
  private textBlob: drawing.TextBlob = null;
  private pixelmap: image.PixelMap = null;
  private alpha: number = null;
  private red: number = null;
  private green: number = null;
  private blue: number = null;
  private left: number = null;
  private top: number = null;
  private right: number = null;
  private bottom: number = null;
  static functions_: Map<CanvasAllFunction, (canvasInvoke: StabilityCanvasInvoke, canvas: drawing.Canvas) => void> = new Map([
    [CanvasAllFunction.DRAW_RECT, (canvasInvoke: StabilityCanvasInvoke, canvas: drawing.Canvas) => {
      canvasInvoke.CanvasDrawRectTest(canvas)
    }],
    [CanvasAllFunction.DRAW_ROUND_RECT, (canvasInvoke: StabilityCanvasInvoke, canvas: drawing.Canvas) => {
      canvasInvoke.CanvasDrawRoundRectTest(canvas)
    }],
    [CanvasAllFunction.DRAW_NESTED_ROUND_RECT, (canvasInvoke: StabilityCanvasInvoke, canvas: drawing.Canvas) => {
      canvasInvoke.CanvasDrawNestedRoundRectTest(canvas)
    }],
    [CanvasAllFunction.DRAW_BACKGROUND, (canvasInvoke: StabilityCanvasInvoke, canvas: drawing.Canvas) => {
      canvasInvoke.CanvasDrawBackgroundTest(canvas)
    }],
    [CanvasAllFunction.DRAW_SHADOW, (canvasInvoke: StabilityCanvasInvoke, canvas: drawing.Canvas) => {
      canvasInvoke.CanvasDrawShadowTest(canvas)
    }],
    [CanvasAllFunction.DRAW_CIRCLE, (canvasInvoke: StabilityCanvasInvoke, canvas: drawing.Canvas) => {
      canvasInvoke.CanvasDrawCircleTest(canvas)
    }],
    [CanvasAllFunction.DRAW_IMAGE, (canvasInvoke: StabilityCanvasInvoke, canvas: drawing.Canvas) => {
      canvasInvoke.CanvasDrawImageTest(canvas)
    }],
    [CanvasAllFunction.DRAW_IMAGE_RECT, (canvasInvoke: StabilityCanvasInvoke, canvas: drawing.Canvas) => {
      canvasInvoke.CanvasDrawImageRectTest(canvas)
    }],
    [CanvasAllFunction.DRAW_IMAGE_RECT_WITH_SRC, (canvasInvoke: StabilityCanvasInvoke, canvas: drawing.Canvas) => {
      canvasInvoke.CanvasDrawImageRectWithSrcTest(canvas)
    }],
    [CanvasAllFunction.DRAW_COLOR, (canvasInvoke: StabilityCanvasInvoke, canvas: drawing.Canvas) => {
      canvasInvoke.CanvasDrawColorTest(canvas)
    }],
    [CanvasAllFunction.DRAW_OVAL, (canvasInvoke: StabilityCanvasInvoke, canvas: drawing.Canvas) => {
      canvasInvoke.CanvasDrawOvalTest(canvas)
    }],
    [CanvasAllFunction.DRAW_ARC, (canvasInvoke: StabilityCanvasInvoke, canvas: drawing.Canvas) => {
      canvasInvoke.CanvasDrawArcTest(canvas)
    }],
    [CanvasAllFunction.DRAW_POINT, (canvasInvoke: StabilityCanvasInvoke, canvas: drawing.Canvas) => {
      canvasInvoke.CanvasDrawPointTest(canvas)
    }],
    [CanvasAllFunction.DRAW_POINTS, (canvasInvoke: StabilityCanvasInvoke, canvas: drawing.Canvas) => {
      canvasInvoke.CanvasDrawPointsTest(canvas)
    }],
    [CanvasAllFunction.DRAW_PATH, (canvasInvoke: StabilityCanvasInvoke, canvas: drawing.Canvas) => {
      canvasInvoke.CanvasDrawPathTest(canvas)
    }],
    [CanvasAllFunction.DRAW_LINE, (canvasInvoke: StabilityCanvasInvoke, canvas: drawing.Canvas) => {
      canvasInvoke.CanvasDrawLineTest(canvas)
    }],
    [CanvasAllFunction.DRAW_SINGLE_CHARACTER, (canvasInvoke: StabilityCanvasInvoke, canvas: drawing.Canvas) => {
      canvasInvoke.CanvasDrawSingleCharacterTest(canvas)
    }],
    [CanvasAllFunction.DRAW_TEXT_BLOB, (canvasInvoke: StabilityCanvasInvoke, canvas: drawing.Canvas) => {
      canvasInvoke.CanvasDrawTextBlobTest(canvas)
    }],
    [CanvasAllFunction.DRAW_PIXEL_MAP_MESH, (canvasInvoke: StabilityCanvasInvoke, canvas: drawing.Canvas) => {
      canvasInvoke.CanvasDrawPixelMapMeshTest(canvas)
    }],
    [CanvasAllFunction.DRAW_REGION, (canvasInvoke: StabilityCanvasInvoke, canvas: drawing.Canvas) => {
      canvasInvoke.CanvasDrawRegionTest(canvas)
    }],
    [CanvasAllFunction.ATTACH_PEN, (canvasInvoke: StabilityCanvasInvoke, canvas: drawing.Canvas) => {
      canvasInvoke.CanvasAttachPenTest(canvas)
    }],
    [CanvasAllFunction.ATTACH_BRUSH, (canvasInvoke: StabilityCanvasInvoke, canvas: drawing.Canvas) => {
      canvasInvoke.CanvasAttachBrushTest(canvas)
    }],
    [CanvasAllFunction.DETACH_PEN, (canvasInvoke: StabilityCanvasInvoke, canvas: drawing.Canvas) => {
      canvasInvoke.CanvasDetachPenTest(canvas)
    }],
    [CanvasAllFunction.DETACH_BRUSH, (canvasInvoke: StabilityCanvasInvoke, canvas: drawing.Canvas) => {
      canvasInvoke.CanvasDetachBrushTest(canvas)
    }],
    [CanvasAllFunction.SAVE, (canvasInvoke: StabilityCanvasInvoke, canvas: drawing.Canvas) => {
      canvasInvoke.CanvasSaveTest(canvas)
    }],
    [CanvasAllFunction.SAVE_LAYER, (canvasInvoke: StabilityCanvasInvoke, canvas: drawing.Canvas) => {
      canvasInvoke.CanvasSaveLayerTest(canvas)
    }],
    [CanvasAllFunction.CLEAR, (canvasInvoke: StabilityCanvasInvoke, canvas: drawing.Canvas) => {
      canvasInvoke.CanvasClearTest(canvas)
    }],
    [CanvasAllFunction.RESTORE, (canvasInvoke: StabilityCanvasInvoke, canvas: drawing.Canvas) => {
      canvasInvoke.CanvasRestoreTest(canvas)
    }],
    [CanvasAllFunction.RESTORE_TO_COUNT, (canvasInvoke: StabilityCanvasInvoke, canvas: drawing.Canvas) => {
      canvasInvoke.CanvasRestoreToCountTest(canvas)
    }],
    [CanvasAllFunction.GET_WIDTH, (canvasInvoke: StabilityCanvasInvoke, canvas: drawing.Canvas) => {
      canvasInvoke.CanvasGetWidthTest(canvas)
    }],
    [CanvasAllFunction.GET_HEIGHT, (canvasInvoke: StabilityCanvasInvoke, canvas: drawing.Canvas) => {
      canvasInvoke.CanvasGetHeightTest(canvas)
    }],
    [CanvasAllFunction.GET_LOCAL_CLIP_BOUNDS, (canvasInvoke: StabilityCanvasInvoke, canvas: drawing.Canvas) => {
      canvasInvoke.CanvasGetLocalClipBoundsTest(canvas)
    }],
    [CanvasAllFunction.GET_TOTAL_MATRIX, (canvasInvoke: StabilityCanvasInvoke, canvas: drawing.Canvas) => {
      canvasInvoke.CanvasGetTotalMatrixTest(canvas)
    }],
    [CanvasAllFunction.SCALE, (canvasInvoke: StabilityCanvasInvoke, canvas: drawing.Canvas) => {
      canvasInvoke.CanvasScaleTest(canvas)
    }],
    [CanvasAllFunction.SKEW, (canvasInvoke: StabilityCanvasInvoke, canvas: drawing.Canvas) => {
      canvasInvoke.CanvasSkewTest(canvas)
    }],
    [CanvasAllFunction.ROTATE, (canvasInvoke: StabilityCanvasInvoke, canvas: drawing.Canvas) => {
      canvasInvoke.CanvasRotateTest(canvas)
    }],
    [CanvasAllFunction.TRANSLATE, (canvasInvoke: StabilityCanvasInvoke, canvas: drawing.Canvas) => {
      canvasInvoke.CanvasTranslateTest(canvas)
    }],
    [CanvasAllFunction.CLIP_PATH, (canvasInvoke: StabilityCanvasInvoke, canvas: drawing.Canvas) => {
      canvasInvoke.CanvasClipPathTest(canvas)
    }],
    [CanvasAllFunction.CLIP_RECT, (canvasInvoke: StabilityCanvasInvoke, canvas: drawing.Canvas) => {
      canvasInvoke.CanvasClipRectTest(canvas)
    }],
    [CanvasAllFunction.CONCAT_MATRIX, (canvasInvoke: StabilityCanvasInvoke, canvas: drawing.Canvas) => {
      canvasInvoke.CanvasConcatMatrixTest(canvas)
    }],
    [CanvasAllFunction.CLIP_REGION, (canvasInvoke: StabilityCanvasInvoke, canvas: drawing.Canvas) => {
      canvasInvoke.CanvasClipRegionTest(canvas)
    }],
    [CanvasAllFunction.CLIP_ROUND_RECT, (canvasInvoke: StabilityCanvasInvoke, canvas: drawing.Canvas) => {
      canvasInvoke.CanvasClipRoundRectTest(canvas)
    }],
    [CanvasAllFunction.IS_CLIP_EMPTY, (canvasInvoke: StabilityCanvasInvoke, canvas: drawing.Canvas) => {
      canvasInvoke.CanvasIsClipEmptyTest(canvas)
    }],
    [CanvasAllFunction.SET_MATRIX, (canvasInvoke: StabilityCanvasInvoke, canvas: drawing.Canvas) => {
      canvasInvoke.CanvasSetMatrixTest(canvas)
    }],
    [CanvasAllFunction.RESET_MATRIX, (canvasInvoke: StabilityCanvasInvoke, canvas: drawing.Canvas) => {
      canvasInvoke.CanvasResetMatrixTest(canvas)
    }],
  ]);

  /*
  @CaseID:SUB_BASIC_GRAPHICS_SPECIAL_STABLE_TS_DRAWING_CANVAS_0200
  @Description:Canvas new-相关接口全调用-置空循环
  @Step:
  1、运行脚本，循环执行hidumper命令查询内存
  2、执行自动化用例：new canvas-相关接口全调用-置空，循环调用1000次
  3、执行结束后等待3分钟
  4、结束脚本运行，抓取的内存值转成折线图，观察内存变化"
  @Result:
  1、程序运行正常，无crash
  2、内存平稳，没有持续增长，执行后内存回落到执行前；
  */
  public async OnTestStability(canvas: drawing.Canvas) {
    console.log("StabilityCanvasInvoke OnTestStability");
    this.alpha = OHRandom.GetRandomNum(0, 0xFF);
    this.red = OHRandom.GetRandomNum(0, 0xFF);
    this.green = OHRandom.GetRandomNum(0, 0xFF);
    this.blue = OHRandom.GetRandomNum(0, 0xFF);
    this.left = OHRandom.GetRandomNum(0, this.width_);
    this.top = OHRandom.GetRandomNum(0, this.width_);
    this.right = OHRandom.GetRandomNum(0, this.width_);
    this.bottom = OHRandom.GetRandomNum(0, this.width_);
    this.rect = { left: this.alpha, right: this.red, top: this.green, bottom: this.blue };
    this.rect1 = { left: this.left, right: this.top, top: this.right, bottom: this.bottom };
    this.roundRect = new drawing.RoundRect(this.rect, OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.height_));
    this.outRoundRect = new drawing.RoundRect(this.rect1, OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.width_));
    this.brush = new drawing.Brush();
    this.path = new drawing.Path();
    this.samplingOptions = new drawing.SamplingOptions(OHRandom.GetRandomNum(0, drawing.FilterMode.FILTER_MODE_LINEAR));
    this.points = new Array;
    this.font = new drawing.Font();
    this.region = new drawing.Region();
    this.region.setRect(OHRandom.GetRandomNum(0, this.right), OHRandom.GetRandomNum(0, this.bottom), this.right, this.bottom);
    this.pen = new drawing.Pen();
    this.matrix = new drawing.Matrix();
    let array = new Array(9);
    for (let j = 0; j < 9; j++) {
      array[j] = OHRandom.GetRandomNum(0, this.width_);
    }
    this.matrix.setMatrix(array);
    this.vertices = new Array(((2 + 1) * (2 + 1) + 0) * 2);
    for (let j = 0; j < this.vertices.length; j++) {
      this.vertices[j] = OHRandom.GetRandomNum(0, this.width_);
    }
    this.colors = new Array((2 + 1) * (2 + 1) + 0);
    for (let j = 0; j < this.colors.length; j++) {
      this.colors[j] = OHRandom.GetRandomNum(0, this.width_);
    }
    this.textBlob = drawing.TextBlob.makeFromString("ni好 @￥#……！*@！（）！#*￥~——+{}“：《？》~!@#$%^&*()_+{}:<>?",
      this.font, OHRandom.GetRandomNum(0, drawing.TextEncoding.TEXT_ENCODING_GLYPH_ID));
    this.pixelmap = await createPixelsMap(this.width_, this.height_);
    for (let i = 0; i < this.testCount_; i++) {
      for (let j = 0; j < CanvasAllFunction.MAX; j++) {
        let canvas = new drawing.Canvas(this.pixelmap);
        StabilityCanvasInvoke.functions_.get(j)(this, canvas);
      }
    }
  }

  CanvasDrawRectTest(canvas: drawing.Canvas) {
    canvas.drawRect(this.rect);
    canvas.drawRect(OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.width_),
      OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.width_));
  }

  CanvasDrawRoundRectTest(canvas: drawing.Canvas) {
    canvas.drawRoundRect(this.roundRect);
  }

  CanvasDrawNestedRoundRectTest(canvas: drawing.Canvas) {
    canvas.drawNestedRoundRect(this.outRoundRect, this.roundRect);
  }

  CanvasDrawBackgroundTest(canvas: drawing.Canvas) {
    canvas.drawBackground(this.brush);
  }

  CanvasDrawShadowTest(canvas: drawing.Canvas) {
    canvas.drawShadow(this.path,
      { x: OHRandom.GetRandomNum(0, this.width_), y: OHRandom.GetRandomNum(0, this.width_), z: OHRandom.GetRandomNum(0, this.width_) },
      { x: OHRandom.GetRandomNum(0, this.width_), y: OHRandom.GetRandomNum(0, this.width_), z: OHRandom.GetRandomNum(0, this.width_) },
      OHRandom.GetRandomNum(0, this.width_),
      { alpha: this.alpha, red: this.red, green: this.green, blue: this.blue },
      { alpha: this.alpha, red: this.red, green: this.green, blue: this.blue },
      OHRandom.GetRandomNum(0, drawing.ShadowFlag.ALL));
  }

  CanvasDrawCircleTest(canvas: drawing.Canvas) {
    canvas.drawCircle(OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.width_),
      OHRandom.GetRandomNum(0, this.width_));
  }

  CanvasDrawImageTest(canvas: drawing.Canvas) {
    canvas.drawImage(this.pixelmap, OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.width_));
    canvas.drawImage(this.pixelmap, OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.width_), this.samplingOptions);
  }

  CanvasDrawImageRectTest(canvas: drawing.Canvas) {
    canvas.drawImageRect(this.pixelmap, { left: this.left, right: this.top, top: this.right, bottom: this.bottom });
    canvas.drawImageRect(this.pixelmap, { left: this.left, right: this.top, top: this.right, bottom: this.bottom }, this.samplingOptions);
  }

  CanvasDrawImageRectWithSrcTest(canvas: drawing.Canvas) {
    canvas.drawImageRectWithSrc(this.pixelmap, { left: this.left, right: this.top, top: this.right, bottom: this.bottom },
      { left: this.left, right: this.top, top: this.right, bottom: this.bottom });
    canvas.drawImageRectWithSrc(this.pixelmap, { left: this.left, right: this.top, top: this.right, bottom: this.bottom },
      { left: this.left, right: this.top, top: this.right, bottom: this.bottom }, this.samplingOptions);
    canvas.drawImageRectWithSrc(this.pixelmap, { left: this.left, right: this.top, top: this.right, bottom: this.bottom },
      { left: this.left, right: this.top, top: this.right, bottom: this.bottom }, this.samplingOptions, OHRandom.GetRandomNum(0, drawing.SrcRectConstraint.FAST));
  }

  CanvasDrawColorTest(canvas: drawing.Canvas) {
    canvas.drawColor({ alpha: this.alpha, red: this.red, green: this.green, blue: this.blue });
    canvas.drawColor({ alpha: this.alpha, red: this.red, green: this.green, blue: this.blue }, OHRandom.GetRandomNum(0, drawing.BlendMode.LUMINOSITY));
    // todo: drawColor(color: number, blendMode?: BlendMode): void;此方法还未转测
    //canvas.drawColor(this.alpha, this.red, this.green, this.blue);
    //canvas.drawColor(this.alpha, this.red, this.green, this.blue, OHRandom.GetRandomNum(0, drawing.BlendMode.LUMINOSITY));
  }

  CanvasDrawOvalTest(canvas: drawing.Canvas) {
    canvas.drawOval(this.rect);
  }

  CanvasDrawArcTest(canvas: drawing.Canvas) {
    canvas.drawArc(this.rect, OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.width_));
  }

  CanvasDrawPointTest(canvas: drawing.Canvas) {
    canvas.drawPoint(OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.width_));
  }

  CanvasDrawPointsTest(canvas: drawing.Canvas) {
    this.points.push({ x: OHRandom.GetRandomNum(0, this.width_), y: OHRandom.GetRandomNum(0, this.width_) });
    canvas.drawPoints(this.points);
    canvas.drawPoints(this.points, OHRandom.GetRandomNum(0, drawing.PointMode.POLYGON));
  }

  CanvasDrawPathTest(canvas: drawing.Canvas) {
    canvas.drawPath(this.path);
  }

  CanvasDrawLineTest(canvas: drawing.Canvas) {
    canvas.drawLine(OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.width_),
      OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.width_));
  }

  CanvasDrawSingleCharacterTest(canvas: drawing.Canvas) {
    canvas.drawSingleCharacter("中", this.font, 100, 100);
  }

  CanvasDrawTextBlobTest(canvas: drawing.Canvas) {
    canvas.drawTextBlob(this.textBlob, OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.width_));
  }

  CanvasDrawPixelMapMeshTest(canvas: drawing.Canvas) {
    canvas.attachBrush(this.brush);
    canvas.drawPixelMapMesh(this.pixelmap, 2, 2, this.vertices, 0, this.colors, 0);
  }

  CanvasDrawRegionTest(canvas: drawing.Canvas) {
    canvas.drawRegion(this.region);
  }

  CanvasAttachPenTest(canvas: drawing.Canvas) {
    canvas.attachPen(this.pen);
  }

  CanvasAttachBrushTest(canvas: drawing.Canvas) {
    canvas.attachBrush(this.brush);
  }

  CanvasDetachPenTest(canvas: drawing.Canvas) {
    canvas.detachPen();
  }

  CanvasDetachBrushTest(canvas: drawing.Canvas) {
    canvas.detachBrush();
  }

  CanvasSaveTest(canvas: drawing.Canvas) {
    canvas.save();
  }

  CanvasSaveLayerTest(canvas: drawing.Canvas) {
    canvas.saveLayer();
    canvas.saveLayer(this.rect);
    canvas.saveLayer(this.rect, this.brush);
  }

  CanvasClearTest(canvas: drawing.Canvas) {
    canvas.clear({ alpha: this.alpha, red: this.red, green: this.green, blue: this.blue });
  }

  CanvasRestoreTest(canvas: drawing.Canvas) {
    canvas.restore();
  }

  CanvasRestoreToCountTest(canvas: drawing.Canvas) {
    canvas.restoreToCount(canvas.save());
  }

  CanvasGetWidthTest(canvas: drawing.Canvas) {
    canvas.getWidth();
  }

  CanvasGetHeightTest(canvas: drawing.Canvas) {
    canvas.getHeight();
  }

  CanvasGetLocalClipBoundsTest(canvas: drawing.Canvas) {
    canvas.getLocalClipBounds();
  }

  CanvasGetTotalMatrixTest(canvas: drawing.Canvas) {
    canvas.getTotalMatrix();
  }

  CanvasScaleTest(canvas: drawing.Canvas) {
    canvas.scale(OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.width_));
  }

  CanvasSkewTest(canvas: drawing.Canvas) {
    canvas.skew(OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.width_));
  }

  CanvasRotateTest(canvas: drawing.Canvas) {
    canvas.rotate(OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.width_));
  }

  CanvasTranslateTest(canvas: drawing.Canvas) {
    canvas.translate(OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.width_));
  }

  CanvasClipPathTest(canvas: drawing.Canvas) {
    canvas.clipPath(this.path);
    canvas.clipPath(this.path, OHRandom.GetRandomNum(0, drawing.ClipOp.INTERSECT));
    canvas.clipPath(this.path, OHRandom.GetRandomNum(0, drawing.ClipOp.INTERSECT), OHRandom.GetRandomNum(0, drawing.ClipOp.INTERSECT) % 2 == 0);
  }

  CanvasClipRectTest(canvas: drawing.Canvas) {
    canvas.clipRect(this.rect);
    canvas.clipRect(this.rect, OHRandom.GetRandomNum(0, drawing.ClipOp.INTERSECT));
    canvas.clipRect(this.rect, OHRandom.GetRandomNum(0, drawing.ClipOp.INTERSECT), OHRandom.GetRandomNum(0, drawing.ClipOp.INTERSECT) % 2 == 0);
  }

  CanvasConcatMatrixTest(canvas: drawing.Canvas) {
    canvas.concatMatrix(this.matrix);
  }

  CanvasClipRegionTest(canvas: drawing.Canvas) {
    canvas.clipRegion(this.region);
    canvas.clipRegion(this.region, OHRandom.GetRandomNum(0, drawing.ClipOp.INTERSECT));
  }

  CanvasClipRoundRectTest(canvas: drawing.Canvas) {
    canvas.clipRoundRect(this.roundRect);
    canvas.clipRoundRect(this.roundRect, OHRandom.GetRandomNum(0, drawing.ClipOp.INTERSECT));
    canvas.clipRoundRect(this.roundRect, OHRandom.GetRandomNum(0, drawing.ClipOp.INTERSECT), OHRandom.GetRandomNum(0, drawing.ClipOp.INTERSECT) % 2 == 0);
  }

  CanvasIsClipEmptyTest(canvas: drawing.Canvas) {
    canvas.isClipEmpty();
  }

  CanvasSetMatrixTest(canvas: drawing.Canvas) {
    canvas.setMatrix(this.matrix);
  }

  CanvasResetMatrixTest(canvas: drawing.Canvas) {
    canvas.resetMatrix();
  }
}

export class StabilityCanvasRandInvoke extends TestBase {
  private rect: common2D.Rect = null;
  private rect1: common2D.Rect = null;
  private roundRect: drawing.RoundRect = null;
  private outRoundRect: drawing.RoundRect = null;
  private brush: drawing.Brush = null;
  private path: drawing.Path = null;
  private samplingOptions: drawing.SamplingOptions = null;
  private points: Array<common2D.Point> = null;
  private font: drawing.Font = null;
  private region: drawing.Region = null;
  private pen: drawing.Pen = null;
  private matrix: drawing.Matrix = null;
  private vertices: Array<number> = null;
  private colors: Array<number> = null;
  private textBlob: drawing.TextBlob = null;
  private pixelmap: image.PixelMap = null;
  private alpha: number = null;
  private red: number = null;
  private green: number = null;
  private blue: number = null;
  private left: number = null;
  private top: number = null;
  private right: number = null;
  private bottom: number = null;
  static functions_: Map<CanvasOperationFunction, (canvasRandInvoke: StabilityCanvasRandInvoke, canvas: drawing.Canvas) => void> = new Map([
    [CanvasOperationFunction.DRAW_RECT, (canvasRandInvoke: StabilityCanvasRandInvoke, canvas: drawing.Canvas) => {
      canvasRandInvoke.CanvasDrawRectTest(canvas)
    }],
    [CanvasOperationFunction.DRAW_ROUND_RECT, (canvasRandInvoke: StabilityCanvasRandInvoke, canvas: drawing.Canvas) => {
      canvasRandInvoke.CanvasDrawRoundRectTest(canvas)
    }],
    [CanvasOperationFunction.DRAW_NESTED_ROUND_RECT, (canvasRandInvoke: StabilityCanvasRandInvoke, canvas: drawing.Canvas) => {
      canvasRandInvoke.CanvasDrawNestedRoundRectTest(canvas)
    }],
    [CanvasOperationFunction.DRAW_BACKGROUND, (canvasRandInvoke: StabilityCanvasRandInvoke, canvas: drawing.Canvas) => {
      canvasRandInvoke.CanvasDrawBackgroundTest(canvas)
    }],
    [CanvasOperationFunction.DRAW_SHADOW, (canvasRandInvoke: StabilityCanvasRandInvoke, canvas: drawing.Canvas) => {
      canvasRandInvoke.CanvasDrawShadowTest(canvas)
    }],
    [CanvasOperationFunction.DRAW_CIRCLE, (canvasRandInvoke: StabilityCanvasRandInvoke, canvas: drawing.Canvas) => {
      canvasRandInvoke.CanvasDrawCircleTest(canvas)
    }],
    [CanvasOperationFunction.DRAW_IMAGE, (canvasRandInvoke: StabilityCanvasRandInvoke, canvas: drawing.Canvas) => {
      canvasRandInvoke.CanvasDrawImageTest(canvas)
    }],
    [CanvasOperationFunction.DRAW_IMAGE_RECT, (canvasRandInvoke: StabilityCanvasRandInvoke, canvas: drawing.Canvas) => {
      canvasRandInvoke.CanvasDrawImageRectTest(canvas)
    }],
    [CanvasOperationFunction.DRAW_IMAGE_RECT_WITH_SRC, (canvasRandInvoke: StabilityCanvasRandInvoke, canvas: drawing.Canvas) => {
      canvasRandInvoke.CanvasDrawImageRectWithSrcTest(canvas)
    }],
    [CanvasOperationFunction.DRAW_COLOR, (canvasRandInvoke: StabilityCanvasRandInvoke, canvas: drawing.Canvas) => {
      canvasRandInvoke.CanvasDrawColorTest(canvas)
    }],
    [CanvasOperationFunction.DRAW_OVAL, (canvasRandInvoke: StabilityCanvasRandInvoke, canvas: drawing.Canvas) => {
      canvasRandInvoke.CanvasDrawOvalTest(canvas)
    }],
    [CanvasOperationFunction.DRAW_ARC, (canvasRandInvoke: StabilityCanvasRandInvoke, canvas: drawing.Canvas) => {
      canvasRandInvoke.CanvasDrawArcTest(canvas)
    }],
    [CanvasOperationFunction.DRAW_POINT, (canvasRandInvoke: StabilityCanvasRandInvoke, canvas: drawing.Canvas) => {
      canvasRandInvoke.CanvasDrawPointTest(canvas)
    }],
    [CanvasOperationFunction.DRAW_POINTS, (canvasRandInvoke: StabilityCanvasRandInvoke, canvas: drawing.Canvas) => {
      canvasRandInvoke.CanvasDrawPointsTest(canvas)
    }],
    [CanvasOperationFunction.DRAW_PATH, (canvasRandInvoke: StabilityCanvasRandInvoke, canvas: drawing.Canvas) => {
      canvasRandInvoke.CanvasDrawPathTest(canvas)
    }],
    [CanvasOperationFunction.DRAW_LINE, (canvasRandInvoke: StabilityCanvasRandInvoke, canvas: drawing.Canvas) => {
      canvasRandInvoke.CanvasDrawLineTest(canvas)
    }],
    [CanvasOperationFunction.DRAW_SINGLE_CHARACTER, (canvasRandInvoke: StabilityCanvasRandInvoke, canvas: drawing.Canvas) => {
      canvasRandInvoke.CanvasDrawSingleCharacterTest(canvas)
    }],
    [CanvasOperationFunction.DRAW_TEXT_BLOB, (canvasRandInvoke: StabilityCanvasRandInvoke, canvas: drawing.Canvas) => {
      canvasRandInvoke.CanvasDrawTextBlobTest(canvas)
    }],
    [CanvasOperationFunction.DRAW_PIXEL_MAP_MESH, (canvasRandInvoke: StabilityCanvasRandInvoke, canvas: drawing.Canvas) => {
      canvasRandInvoke.CanvasDrawPixelMapMeshTest(canvas)
    }],
    [CanvasOperationFunction.DRAW_REGION, (canvasRandInvoke: StabilityCanvasRandInvoke, canvas: drawing.Canvas) => {
      canvasRandInvoke.CanvasDrawRegionTest(canvas)
    }],
    [CanvasOperationFunction.SAVE, (canvasRandInvoke: StabilityCanvasRandInvoke, canvas: drawing.Canvas) => {
      canvasRandInvoke.CanvasSaveTest(canvas)
    }],
    [CanvasOperationFunction.SAVE_LAYER, (canvasRandInvoke: StabilityCanvasRandInvoke, canvas: drawing.Canvas) => {
      canvasRandInvoke.CanvasSaveLayerTest(canvas)
    }],
    [CanvasOperationFunction.CLEAR, (canvasRandInvoke: StabilityCanvasRandInvoke, canvas: drawing.Canvas) => {
      canvasRandInvoke.CanvasClearTest(canvas)
    }],
    [CanvasOperationFunction.RESTORE, (canvasRandInvoke: StabilityCanvasRandInvoke, canvas: drawing.Canvas) => {
      canvasRandInvoke.CanvasRestoreTest(canvas)
    }],
    [CanvasOperationFunction.RESTORE_TO_COUNT, (canvasRandInvoke: StabilityCanvasRandInvoke, canvas: drawing.Canvas) => {
      canvasRandInvoke.CanvasRestoreToCountTest(canvas)
    }],
    [CanvasOperationFunction.GET_WIDTH, (canvasRandInvoke: StabilityCanvasRandInvoke, canvas: drawing.Canvas) => {
      canvasRandInvoke.CanvasGetWidthTest(canvas)
    }],
    [CanvasOperationFunction.GET_HEIGHT, (canvasRandInvoke: StabilityCanvasRandInvoke, canvas: drawing.Canvas) => {
      canvasRandInvoke.CanvasGetHeightTest(canvas)
    }],
    [CanvasOperationFunction.GET_LOCAL_CLIP_BOUNDS, (canvasRandInvoke: StabilityCanvasRandInvoke, canvas: drawing.Canvas) => {
      canvasRandInvoke.CanvasGetLocalClipBoundsTest(canvas)
    }],
    [CanvasOperationFunction.GET_TOTAL_MATRIX, (canvasRandInvoke: StabilityCanvasRandInvoke, canvas: drawing.Canvas) => {
      canvasRandInvoke.CanvasGetTotalMatrixTest(canvas)
    }],
    [CanvasOperationFunction.SCALE, (canvasRandInvoke: StabilityCanvasRandInvoke, canvas: drawing.Canvas) => {
      canvasRandInvoke.CanvasScaleTest(canvas)
    }],
    [CanvasOperationFunction.SKEW, (canvasRandInvoke: StabilityCanvasRandInvoke, canvas: drawing.Canvas) => {
      canvasRandInvoke.CanvasSkewTest(canvas)
    }],
    [CanvasOperationFunction.ROTATE, (canvasRandInvoke: StabilityCanvasRandInvoke, canvas: drawing.Canvas) => {
      canvasRandInvoke.CanvasRotateTest(canvas)
    }],
    [CanvasOperationFunction.TRANSLATE, (canvasRandInvoke: StabilityCanvasRandInvoke, canvas: drawing.Canvas) => {
      canvasRandInvoke.CanvasTranslateTest(canvas)
    }],
    [CanvasOperationFunction.CLIP_PATH, (canvasRandInvoke: StabilityCanvasRandInvoke, canvas: drawing.Canvas) => {
      canvasRandInvoke.CanvasClipPathTest(canvas)
    }],
    [CanvasOperationFunction.CLIP_RECT, (canvasRandInvoke: StabilityCanvasRandInvoke, canvas: drawing.Canvas) => {
      canvasRandInvoke.CanvasClipRectTest(canvas)
    }],
    [CanvasOperationFunction.CONCAT_MATRIX, (canvasRandInvoke: StabilityCanvasRandInvoke, canvas: drawing.Canvas) => {
      canvasRandInvoke.CanvasConcatMatrixTest(canvas)
    }],
    [CanvasOperationFunction.CLIP_REGION, (canvasRandInvoke: StabilityCanvasRandInvoke, canvas: drawing.Canvas) => {
      canvasRandInvoke.CanvasClipRegionTest(canvas)
    }],
    [CanvasOperationFunction.CLIP_ROUND_RECT, (canvasRandInvoke: StabilityCanvasRandInvoke, canvas: drawing.Canvas) => {
      canvasRandInvoke.CanvasClipRoundRectTest(canvas)
    }],
    [CanvasOperationFunction.IS_CLIP_EMPTY, (canvasRandInvoke: StabilityCanvasRandInvoke, canvas: drawing.Canvas) => {
      canvasRandInvoke.CanvasIsClipEmptyTest(canvas)
    }],
    [CanvasOperationFunction.RESET_MATRIX, (canvasRandInvoke: StabilityCanvasRandInvoke, canvas: drawing.Canvas) => {
      canvasRandInvoke.CanvasResetMatrixTest(canvas)
    }],
  ]);

  /*
  @CaseID:SUB_BASIC_GRAPHICS_SPECIAL_STABLE_TS_DRAWING_CANVAS_0300
  @Description:Canvas相关操作接口随机循环调用
  @Step:
  1、运行脚本，循环执行hidumper命令查询内存
  2、执行自动化用例：new canvas-Canvas相关操作接口调用，循环调用1000次-canvas置空
  3、执行结束后等待3分钟
  4、结束脚本运行，抓取的内存值转成折线图，观察内存变化"
  @Result:
      1、程序运行正常，无crash
      2、内存平稳，没有持续增长，执行后内存回落到执行前；
*/
  public async OnTestStability(canvas: drawing.Canvas) {
    console.log("StabilityCanvasRandInvoke OnTestStability");
    this.alpha = OHRandom.GetRandomNum(0, 0xFF);
    this.red = OHRandom.GetRandomNum(0, 0xFF);
    this.green = OHRandom.GetRandomNum(0, 0xFF);
    this.blue = OHRandom.GetRandomNum(0, 0xFF);
    this.left = OHRandom.GetRandomNum(0, this.width_);
    this.top = OHRandom.GetRandomNum(0, this.width_);
    this.right = OHRandom.GetRandomNum(0, this.width_);
    this.bottom = OHRandom.GetRandomNum(0, this.width_);
    this.rect = { left: this.alpha, right: this.red, top: this.green, bottom: this.blue };
    this.rect1 = { left: this.left, right: this.top, top: this.right, bottom: this.bottom };
    this.roundRect = new drawing.RoundRect(this.rect, OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.height_));
    this.outRoundRect = new drawing.RoundRect(this.rect1, OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.width_));
    this.brush = new drawing.Brush();
    this.path = new drawing.Path();
    this.samplingOptions = new drawing.SamplingOptions(OHRandom.GetRandomNum(0, drawing.FilterMode.FILTER_MODE_LINEAR));
    this.points = new Array;
    this.font = new drawing.Font();
    this.region = new drawing.Region();
    this.region.setRect(OHRandom.GetRandomNum(0, this.right), OHRandom.GetRandomNum(0, this.bottom), this.right, this.bottom);
    this.pen = new drawing.Pen();
    this.matrix = new drawing.Matrix();
    let array = new Array(9);
    for (let j = 0; j < 9; j++) {
      array[j] = OHRandom.GetRandomNum(0, this.width_);
    }
    this.matrix.setMatrix(array);
    this.vertices = new Array(((2 + 1) * (2 + 1) + 0) * 2);
    for (let j = 0; j < this.vertices.length; j++) {
      this.vertices[j] = OHRandom.GetRandomNum(0, this.width_);
    }
    this.colors = new Array((2 + 1) * (2 + 1) + 0);
    for (let j = 0; j < this.colors.length; j++) {
      this.colors[j] = OHRandom.GetRandomNum(0, this.width_);
    }
    this.textBlob = drawing.TextBlob.makeFromString("ni好 @￥#……！*@！（）！#*￥~——+{}“：《？》~!@#$%^&*()_+{}:<>?",
      this.font, OHRandom.GetRandomNum(0, drawing.TextEncoding.TEXT_ENCODING_GLYPH_ID));
    this.pixelmap = await createPixelsMap(this.width_, this.height_);
    let canvas1 = new drawing.Canvas(this.pixelmap);
    for (let i = 0; i < this.testCount_; i++) {
      StabilityCanvasRandInvoke.functions_.get(OHRandom.GetRandomNum(0, CanvasOperationFunction.MAX - 1))(this, canvas1);
    }
  }

  CanvasDrawRectTest(canvas: drawing.Canvas) {
    canvas.drawRect(this.rect);
    canvas.drawRect(OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.width_),
      OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.width_));
  }

  CanvasDrawRoundRectTest(canvas: drawing.Canvas) {
    canvas.drawRoundRect(this.roundRect);
  }

  CanvasDrawNestedRoundRectTest(canvas: drawing.Canvas) {
    canvas.drawNestedRoundRect(this.outRoundRect, this.roundRect);
  }

  CanvasDrawBackgroundTest(canvas: drawing.Canvas) {
    canvas.drawBackground(this.brush);
  }

  CanvasDrawShadowTest(canvas: drawing.Canvas) {
    canvas.drawShadow(this.path,
      { x: OHRandom.GetRandomNum(0, this.width_), y: OHRandom.GetRandomNum(0, this.width_), z: OHRandom.GetRandomNum(0, this.width_) },
      { x: OHRandom.GetRandomNum(0, this.width_), y: OHRandom.GetRandomNum(0, this.width_), z: OHRandom.GetRandomNum(0, this.width_) },
      OHRandom.GetRandomNum(0, this.width_),
      { alpha: this.alpha, red: this.red, green: this.green, blue: this.blue },
      { alpha: this.alpha, red: this.red, green: this.green, blue: this.blue },
      OHRandom.GetRandomNum(0, drawing.ShadowFlag.ALL));
  }

  CanvasDrawCircleTest(canvas: drawing.Canvas) {
    canvas.drawCircle(OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.width_),
      OHRandom.GetRandomNum(0, this.width_));
  }

  CanvasDrawImageTest(canvas: drawing.Canvas) {
    canvas.drawImage(this.pixelmap, OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.width_));
    canvas.drawImage(this.pixelmap, OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.width_), this.samplingOptions);
  }

  CanvasDrawImageRectTest(canvas: drawing.Canvas) {
    canvas.drawImageRect(this.pixelmap, { left: this.left, right: this.top, top: this.right, bottom: this.bottom });
    canvas.drawImageRect(this.pixelmap, { left: this.left, right: this.top, top: this.right, bottom: this.bottom }, this.samplingOptions);
  }

  CanvasDrawImageRectWithSrcTest(canvas: drawing.Canvas) {
    canvas.drawImageRectWithSrc(this.pixelmap, { left: this.left, right: this.top, top: this.right, bottom: this.bottom },
      { left: this.left, right: this.top, top: this.right, bottom: this.bottom });
    canvas.drawImageRectWithSrc(this.pixelmap, { left: this.left, right: this.top, top: this.right, bottom: this.bottom },
      { left: this.left, right: this.top, top: this.right, bottom: this.bottom }, this.samplingOptions);
    canvas.drawImageRectWithSrc(this.pixelmap, { left: this.left, right: this.top, top: this.right, bottom: this.bottom },
      { left: this.left, right: this.top, top: this.right, bottom: this.bottom }, this.samplingOptions, OHRandom.GetRandomNum(0, drawing.SrcRectConstraint.FAST));
  }

  CanvasDrawColorTest(canvas: drawing.Canvas) {
    canvas.drawColor({ alpha: this.alpha, red: this.red, green: this.green, blue: this.blue });
    canvas.drawColor({ alpha: this.alpha, red: this.red, green: this.green, blue: this.blue }, OHRandom.GetRandomNum(0, drawing.BlendMode.LUMINOSITY));
    // todo: drawColor(color: number, blendMode?: BlendMode): void;此方法还未转测
    //canvas.drawColor(this.alpha, this.red, this.green, this.blue);
    //canvas.drawColor(this.alpha, this.red, this.green, this.blue, OHRandom.GetRandomNum(0, drawing.BlendMode.LUMINOSITY));
  }

  CanvasDrawOvalTest(canvas: drawing.Canvas) {
    canvas.drawOval(this.rect);
  }

  CanvasDrawArcTest(canvas: drawing.Canvas) {
    canvas.drawArc(this.rect, OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.width_));
  }

  CanvasDrawPointTest(canvas: drawing.Canvas) {
    canvas.drawPoint(OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.width_));
  }

  CanvasDrawPointsTest(canvas: drawing.Canvas) {
    this.points.push({ x: OHRandom.GetRandomNum(0, this.width_), y: OHRandom.GetRandomNum(0, this.width_) });
    canvas.drawPoints(this.points);
    canvas.drawPoints(this.points, OHRandom.GetRandomNum(0, drawing.PointMode.POLYGON));
  }

  CanvasDrawPathTest(canvas: drawing.Canvas) {
    canvas.drawPath(this.path);
  }

  CanvasDrawLineTest(canvas: drawing.Canvas) {
    canvas.drawLine(OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.width_),
      OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.width_));
  }

  CanvasDrawSingleCharacterTest(canvas: drawing.Canvas) {
    canvas.drawSingleCharacter("中", this.font, 100, 100);
  }

  CanvasDrawTextBlobTest(canvas: drawing.Canvas) {
    canvas.drawTextBlob(this.textBlob, OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.width_));
  }

  CanvasDrawPixelMapMeshTest(canvas: drawing.Canvas) {
    canvas.attachBrush(this.brush);
    canvas.drawPixelMapMesh(this.pixelmap, 2, 2, this.vertices, 0, this.colors, 0);
  }

  CanvasDrawRegionTest(canvas: drawing.Canvas) {
    canvas.drawRegion(this.region);
  }

  CanvasSaveTest(canvas: drawing.Canvas) {
    canvas.save();
  }

  CanvasSaveLayerTest(canvas: drawing.Canvas) {
    canvas.saveLayer();
    canvas.saveLayer(this.rect);
    canvas.saveLayer(this.rect, this.brush);
  }

  CanvasClearTest(canvas: drawing.Canvas) {
    canvas.clear({ alpha: this.alpha, red: this.red, green: this.green, blue: this.blue });
  }

  CanvasRestoreTest(canvas: drawing.Canvas) {
    canvas.restore();
  }

  CanvasRestoreToCountTest(canvas: drawing.Canvas) {
    canvas.restoreToCount(canvas.save());
  }

  CanvasGetWidthTest(canvas: drawing.Canvas) {
    canvas.getWidth();
  }

  CanvasGetHeightTest(canvas: drawing.Canvas) {
    canvas.getHeight();
  }

  CanvasGetLocalClipBoundsTest(canvas: drawing.Canvas) {
    canvas.getLocalClipBounds();
  }

  CanvasGetTotalMatrixTest(canvas: drawing.Canvas) {
    canvas.getTotalMatrix();
  }

  CanvasScaleTest(canvas: drawing.Canvas) {
    canvas.scale(OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.width_));
  }

  CanvasSkewTest(canvas: drawing.Canvas) {
    canvas.skew(OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.width_));
  }

  CanvasRotateTest(canvas: drawing.Canvas) {
    canvas.rotate(OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.width_));
  }

  CanvasTranslateTest(canvas: drawing.Canvas) {
    canvas.translate(OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.width_));
  }

  CanvasClipPathTest(canvas: drawing.Canvas) {
    canvas.clipPath(this.path);
    canvas.clipPath(this.path, OHRandom.GetRandomNum(0, drawing.ClipOp.INTERSECT));
    canvas.clipPath(this.path, OHRandom.GetRandomNum(0, drawing.ClipOp.INTERSECT), OHRandom.GetRandomNum(0, drawing.ClipOp.INTERSECT) % 2 == 0);
  }

  CanvasClipRectTest(canvas: drawing.Canvas) {
    canvas.clipRect(this.rect);
    canvas.clipRect(this.rect, OHRandom.GetRandomNum(0, drawing.ClipOp.INTERSECT));
    canvas.clipRect(this.rect, OHRandom.GetRandomNum(0, drawing.ClipOp.INTERSECT), OHRandom.GetRandomNum(0, drawing.ClipOp.INTERSECT) % 2 == 0);
  }

  CanvasConcatMatrixTest(canvas: drawing.Canvas) {
    canvas.concatMatrix(this.matrix);
  }

  CanvasClipRegionTest(canvas: drawing.Canvas) {
    canvas.clipRegion(this.region);
    canvas.clipRegion(this.region, OHRandom.GetRandomNum(0, drawing.ClipOp.INTERSECT));
  }

  CanvasClipRoundRectTest(canvas: drawing.Canvas) {
    canvas.clipRoundRect(this.roundRect);
    canvas.clipRoundRect(this.roundRect, OHRandom.GetRandomNum(0, drawing.ClipOp.INTERSECT));
    canvas.clipRoundRect(this.roundRect, OHRandom.GetRandomNum(0, drawing.ClipOp.INTERSECT), OHRandom.GetRandomNum(0, drawing.ClipOp.INTERSECT) % 2 == 0);
  }

  CanvasIsClipEmptyTest(canvas: drawing.Canvas) {
    canvas.isClipEmpty();
  }

  CanvasResetMatrixTest(canvas: drawing.Canvas) {
    canvas.resetMatrix();
  }
}