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
import uiEffect from '@ohos.graphics.uiEffect';
import {TestBase, TestFunctionStyleType, StyleType} from '../../pages/testbase';
import { N, OHRandom } from '../../utils/OHRandom';
import globalThis from '../../utils/globalThis'
import ArrayList from '@ohos.util.ArrayList';
import image from '@ohos.multimedia.image';

const TAG = '[BezierBench]';

// Print test result: Passed or Not Passed
function printResults(canvas: drawing.Canvas, isPassed: Boolean) {
  let font: drawing.Font = new drawing.Font();
  font.setSize(50);
  let blob: drawing.TextBlob = drawing.TextBlob.makeFromString(isPassed ? "Passed": "Not Passed",
    font, drawing.TextEncoding.TEXT_ENCODING_UTF8);
  canvas.drawTextBlob(blob, 10, 50);
}


export class CanvasDrawRect extends TestBase {

  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE){
    super();
    this.styleType_ = styleType;
  }
  public OnTestPerformance(canvas: drawing.Canvas) {
    console.log(TAG, 'draw performance end');
    // 保证性能测试稳定性: 绘制100，100的矩形
    let rect: common2D.Rect = {left: 0, top: 0, right: 100, bottom: 100};
    for (let i = 0; i < this.testCount_; i++) {
      canvas.drawRect(rect);
    }
  }
  
  public OnTestFunction(canvas: drawing.Canvas) {
    //接口调用，功能测试.cpu/gpu调用接口一致
    {
      let rect: common2D.Rect = { left: 10, top: 10, right: 100, bottom: 100 };
      let pen = new drawing.Pen();
      this.ApplyPenStyle(pen, TestFunctionStyleType.DRAW_STYLE_TYPE_1)
      canvas.attachPen(pen)
      canvas.drawRect(rect)
      canvas.detachPen()
    }
    
    {
      let rect: common2D.Rect = { left: 15.5, top: 110.7, right: 235.7, bottom: 150.9 };
      let pen = new drawing.Pen();
      this.ApplyPenStyle(pen, TestFunctionStyleType.DRAW_STYLE_TYPE_2)
      let brush = new drawing.Brush();
      this.ApplyBrushStyle(brush, TestFunctionStyleType.DRAW_STYLE_TYPE_2)
      canvas.attachPen(pen)
      canvas.attachBrush(brush)
      canvas.drawRect(rect)
      canvas.detachPen()
      canvas.detachBrush()
    }

    {
      let rect: common2D.Rect = { left: 2.7, top: 175.7, right: 450.7, bottom: 315.9 };
      let pen = new drawing.Pen();
      this.ApplyPenStyle(pen, TestFunctionStyleType.DRAW_STYLE_TYPE_3)
      let brush = new drawing.Brush();
      this.ApplyBrushStyle (brush, TestFunctionStyleType.DRAW_STYLE_TYPE_3)
      canvas.attachPen(pen)
      canvas.attachBrush(brush)
      canvas.drawRect(rect)
      canvas.detachPen()
      canvas.detachBrush()
    }

    {
      let rect: common2D.Rect = { left: -150.1, top: 250, right: 2000.7, bottom: 2956.9 };
      let pen = new drawing.Pen();
      this.ApplyPenStyle(pen, TestFunctionStyleType.DRAW_STYLE_TYPE_4)
      let brush = new drawing.Brush();
      this.ApplyBrushStyle (brush, TestFunctionStyleType.DRAW_STYLE_TYPE_4)
      canvas.attachPen(pen)
      canvas.attachBrush(brush)
      canvas.drawRect(rect)
      canvas.detachPen()
      canvas.detachBrush()
    }
  }

  public async OnTestStability(canvas: drawing.Canvas) {
    for (let i = 0; i < this.testCount_; i++) {
      let rect: common2D.Rect = { left: 10, top: 10, right: 100, bottom: 100 };
      let pen = new drawing.Pen();
      this.ApplyPenStyle(pen, TestFunctionStyleType.DRAW_STYLE_TYPE_1)
      canvas.attachPen(pen)
      canvas.drawRect(rect)
      canvas.detachPen()
    }
  }
}

export class CanvasDrawLine extends TestBase {

  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE){
    super();
    this.styleType_ = styleType;
  }
  public OnTestPerformance(canvas: drawing.Canvas) {
    // 保证性能测试稳定性: 绘制（0, 0) (100, 100)的直线
    for (let i = 0; i < this.testCount_; i++) {
      canvas.drawLine(0, 0, 100, 100); // 0, 0, 100, 100 创建矩形
    }
  }
  
  public OnTestFunction(canvas: drawing.Canvas) {
    //接口调用，功能测试.cpu/gpu调用接口一致
    {
      let pen = new drawing.Pen();
      this.ApplyPenStyle(pen, TestFunctionStyleType.DRAW_STYLE_TYPE_1)
      canvas.attachPen(pen)
      canvas.drawLine(10, 10, 100, 100)
      canvas.detachPen()
    }
    
    {
      let pen = new drawing.Pen();
      this.ApplyPenStyle(pen, TestFunctionStyleType.DRAW_STYLE_TYPE_2)
      let brush = new drawing.Brush();
      this.ApplyBrushStyle(brush, TestFunctionStyleType.DRAW_STYLE_TYPE_2)
      canvas.attachPen(pen)
      canvas.attachBrush(brush)
      canvas.drawLine(15.5, 110.7, 235.7, 150.9)
      canvas.detachPen()
      canvas.detachBrush()
    }

    {
      let pen = new drawing.Pen();
      this.ApplyPenStyle(pen, TestFunctionStyleType.DRAW_STYLE_TYPE_3)
      let brush = new drawing.Brush();
      this.ApplyBrushStyle (brush, TestFunctionStyleType.DRAW_STYLE_TYPE_3)
      canvas.attachPen(pen)
      canvas.attachBrush(brush)
      canvas.drawLine(2.7, 175.7, 450.7, 315.9)
      canvas.detachPen()
      canvas.detachBrush()
    }

    {
      let pen = new drawing.Pen();
      this.ApplyPenStyle(pen, TestFunctionStyleType.DRAW_STYLE_TYPE_4)
      let brush = new drawing.Brush();
      this.ApplyBrushStyle (brush, TestFunctionStyleType.DRAW_STYLE_TYPE_4)
      canvas.attachPen(pen)
      canvas.attachBrush(brush)
      canvas.drawLine(-150.1, 250, 2000.7, 2956.9)
      canvas.detachPen()
      canvas.detachBrush()
    }
  }
}

export class CanvasCreateLattice extends TestBase {
  public constructor(){
    super();
  }
  public OnTestPerformance(canvas: drawing.Canvas) {
    let xDivs : Array<number> = [1, 2, 4];
    let yDivs : Array<number> = [1, 2, 4];
    let colorArray :Array<number>=[0xffffff,0x444444,0x999999,0xffffff,0x444444,0x999999,0xffffff,0x444444,0x999999,0x444444,0x999999,0xffffff,0x444444,0x999999,0xffffff,0x444444];
    for (let i = 0; i < this.testCount_; i++) {
      let lattice = drawing.Lattice.createImageLattice(xDivs, yDivs, 3, 3,null,null,colorArray);
    }
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    {
      let xDivs : Array<number> = [1, 2, 4];
      let yDivs : Array<number> = [1, 2, 4];
      let colorArray :Array<number>=[0xffffff,0x444444,0x999999,0xffffff,0x444444,0x999999,0xffffff,0x444444,0x999999,0x444444,0x999999,0xffffff,0x444444,0x999999,0xffffff,0x444444];
      let color : common2D.Color = { alpha: 0xFF, red: 0x55, green: 0xFF, blue: 0x44 };
      let colorArray1 :Array<common2D.Color>=[color,color,color,color,color,color,color,color,color,color,color,color,color,color,color,color]
      let lattice = drawing.Lattice.createImageLattice(xDivs, yDivs, 3, 3,null,null,colorArray);
    }
  }
}

export class CanvasDrawShadow extends TestBase {
  public constructor(){
    super();
  }
  public OnTestPerformance(canvas: drawing.Canvas) {
    const path = new drawing.Path();
    let point1 : common2D.Point3d = {x: 100, y: 80, z:80};
    let point2 : common2D.Point3d = {x: 200, y: 10, z:40};
    let shadowFlag : drawing.ShadowFlag = drawing.ShadowFlag.ALL;
    for (let i = 0; i < this.testCount_; i++) {
      canvas.drawShadow(path, point1, point2, 30, 0xFF0000FF, 0xFFFF0000, shadowFlag);
    }
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    {
      const path = new drawing.Path();
      path.addCircle(350, 300, 100, drawing.PathDirection.CLOCKWISE);
      let point1 : common2D.Point3d = {x: 100, y: 80, z:80};
      let point2 : common2D.Point3d = {x: 200, y: 10, z:40};
      let shadowFlag : drawing.ShadowFlag = drawing.ShadowFlag.ALL;
      canvas.drawShadow(path, point1, point2, 30, 0xFF0000FF, 0xFFFF0000, shadowFlag);
    }
  }
}

export class CanvasClear extends TestBase {
  public constructor(){
    super();
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    for(let i = 0; i < this.testCount_; i++) {
      canvas.clear(0xffff0000);
    }
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    canvas.clear({ alpha: 255, red: 255, green: 0, blue: 0});

    canvas.clear(0xffff0000);
  }
}

export class CanvasDrawPath extends TestBase {

  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE){
    super();
    this.styleType_ = styleType;
  }
  public OnTestPerformance(canvas: drawing.Canvas) {
    // 保证性能测试稳定性: 绘制半弧的path
    let path: drawing.Path = new drawing.Path();
    path.arcTo(0, 0, 100, 100, 0, 180);
    path.close;
    for (let i = 0; i < this.testCount_; i++) {
      canvas.drawPath(path);
    }
  }
  
  public OnTestFunction(canvas: drawing.Canvas) {
    //接口调用，功能测试.cpu/gpu调用接口一致

    const path = new drawing.Path();
    {
      let pen = new drawing.Pen();
      this.ApplyPenStyle(pen, TestFunctionStyleType.DRAW_STYLE_TYPE_1)
      let brush = new drawing.Brush();
      this.ApplyBrushStyle(brush, TestFunctionStyleType.DRAW_STYLE_TYPE_1)
      canvas.attachPen(pen)
      canvas.attachBrush(brush)


      path.moveTo(10, 12.5)
      path.lineTo(112, 67.81)
      for (let i = 0; i < 5; i++) {
        path.lineTo(112 + i * 12, 67.81 + i * 23.4)
      }
      path.close()
      canvas.drawPath(path)
      canvas.detachPen()
      canvas.detachBrush()
    }
    
    {
      let pen = new drawing.Pen();
      this.ApplyPenStyle(pen, TestFunctionStyleType.DRAW_STYLE_TYPE_2)
      let brush = new drawing.Brush();
      this.ApplyBrushStyle(brush, TestFunctionStyleType.DRAW_STYLE_TYPE_2)
      canvas.attachPen(pen)
      canvas.attachBrush(brush)
      path.reset()
      path.arcTo(323.7, 87.69, 438.3, 195.81, -81, 167.3)
      canvas.drawPath(path)
      path.reset()
      path.arcTo(469.7, 168.69, 537.41, 328.4, -127.36, 36.5 )
      canvas.drawPath(path)

      path.reset()
      path.moveTo(500, 563.9)
      path.quadTo(416.3, 376.81, 515.3, 465.81)
      canvas.drawPath(path)
      canvas.detachPen()
      canvas.detachBrush()
    }

    {
      let pen = new drawing.Pen();
      this.ApplyPenStyle(pen, TestFunctionStyleType.DRAW_STYLE_TYPE_3)
      let brush = new drawing.Brush();
      this.ApplyBrushStyle (brush, TestFunctionStyleType.DRAW_STYLE_TYPE_3)
      canvas.attachPen(pen)
      canvas.attachBrush(brush)
      path.reset()
      path.cubicTo(17.6, 168, 216.9, 316.8, 378.16, 698.4)
      canvas.drawPath(path)
      canvas.detachPen()
      canvas.detachBrush()
    }

    {
      let pen = new drawing.Pen();
      this.ApplyPenStyle(pen, TestFunctionStyleType.DRAW_STYLE_TYPE_4)
      let brush = new drawing.Brush();
      this.ApplyBrushStyle (brush, TestFunctionStyleType.DRAW_STYLE_TYPE_4)
      canvas.attachPen(pen)
      canvas.attachBrush(brush)
      path.reset()
      path.moveTo(2136.47, 6417.4)
      path.lineTo(-57, -30.41)
      canvas.drawPath(path)
      canvas.detachPen()
      canvas.detachBrush()
    }
  }
}

export class CanvasIsClipEmpty extends TestBase {

  public constructor(){
    super();
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    const path = new drawing.Path();

    canvas.save();
    canvas.clipPath(path);
    for (let i = 0; i < this.testCount_; i++) {
      canvas.isClipEmpty();
    }
    canvas.restore();
    printResults(canvas, true);
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    let failure: boolean = false;
    const path = new drawing.Path();

    canvas.save();
    if (canvas.isClipEmpty()) {
      console.log(TAG, "canvas.isClipEmpty() returned TRUE for empty clipPath");
      failure = true;
    }

    canvas.clipPath(path);
    if (!(canvas.isClipEmpty())) {
      console.log(TAG, "canvas.isClipEmpty() returned FALSE for non-empty clipPath");
      failure = true;
    }

    canvas.restore();
    if (canvas.isClipEmpty()) {
      console.log(TAG, "canvas.isClipEmpty() returned TRUE for empty (restored) clipPath");
      failure = true;
    }
    printResults(canvas, !failure);
  }
}

export class CanvasDrawPoint extends TestBase {

  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE){
    super();
    this.styleType_ = styleType;
  }
  public OnTestPerformance(canvas: drawing.Canvas) {
    // 保证性能测试稳定性: 绘制 (10, 10)的点
    for (let i = 0; i < this.testCount_; i++) {
      canvas.drawPoint(10, 10);
    }
  }
  
  public OnTestFunction(canvas: drawing.Canvas) {
    //接口调用，功能测试.cpu/gpu调用接口一致
    {
      let pen = new drawing.Pen();
      this.ApplyPenStyle(pen, TestFunctionStyleType.DRAW_STYLE_TYPE_1)
      let brush = new drawing.Brush();
      this.ApplyBrushStyle(brush, TestFunctionStyleType.DRAW_STYLE_TYPE_1)
      canvas.attachPen(pen)
      canvas.attachBrush(brush)
      canvas.drawPoint(50, 50)
      canvas.detachPen()
      canvas.detachBrush()
    }
    
    {
      let pen = new drawing.Pen();
      this.ApplyPenStyle(pen, TestFunctionStyleType.DRAW_STYLE_TYPE_2)
      let brush = new drawing.Brush();
      this.ApplyBrushStyle(brush, TestFunctionStyleType.DRAW_STYLE_TYPE_2)
      canvas.attachPen(pen)
      canvas.attachBrush(brush)
      canvas.drawPoint(100.6, 86.6)
      canvas.detachPen()
      canvas.detachBrush()
    }

    {
      let pen = new drawing.Pen();
      this.ApplyPenStyle(pen, TestFunctionStyleType.DRAW_STYLE_TYPE_3)
      let brush = new drawing.Brush();
      this.ApplyBrushStyle (brush, TestFunctionStyleType.DRAW_STYLE_TYPE_3)
      canvas.attachPen(pen)
      canvas.attachBrush(brush)
      canvas.drawPoint(163.6, 110.6)
      canvas.detachPen()
      canvas.detachBrush()
    }

    {
      let pen = new drawing.Pen();
      this.ApplyPenStyle(pen, TestFunctionStyleType.DRAW_STYLE_TYPE_4)
      let brush = new drawing.Brush();
      this.ApplyBrushStyle (brush, TestFunctionStyleType.DRAW_STYLE_TYPE_4)
      canvas.attachPen(pen)
      canvas.attachBrush(brush)
      canvas.drawPoint(-1.5, 168.6)
      canvas.detachPen()
      canvas.detachBrush()
    }
  }
}

export class CanvasDrawImage extends TestBase {

  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }
  public OnTestPerformance(canvas: drawing.Canvas) {
    // 保证性能测试稳定性: 绘制test_1图片
    let pixelMap: image.PixelMap = globalThis.getInstance().getPixelMap("test_1.jpg")
    for (let i = 0; i < this.testCount_; i++) {
      canvas.drawImage(pixelMap, 0, 0);
    }
  }
  
  public OnTestFunction(canvas: drawing.Canvas) {

    //接口调用，功能测试.cpu/gpu调用接口一致
    {
      let pen = new drawing.Pen();
      this.ApplyPenStyle(pen, TestFunctionStyleType.DRAW_STYLE_TYPE_3)
      canvas.attachPen(pen)
      let pixelMap: image.PixelMap = globalThis.getInstance().getPixelMap("test_1.jpg")
      canvas.drawImage(pixelMap, 0, 0);
      canvas.detachPen()
    }
    
    {
      let pen = new drawing.Pen();
      this.ApplyPenStyle(pen, TestFunctionStyleType.DRAW_STYLE_TYPE_2)
      let brush = new drawing.Brush();
      this.ApplyBrushStyle(brush, TestFunctionStyleType.DRAW_STYLE_TYPE_2)
      canvas.attachPen(pen)
      canvas.attachBrush(brush)
      let pixelMap: image.PixelMap = globalThis.getInstance().getPixelMap("test_2.jpg")
      canvas.drawImage(pixelMap, 100, 150);
      canvas.detachPen()
      canvas.detachBrush()
    }

    {
      let pen = new drawing.Pen();
      this.ApplyPenStyle(pen, TestFunctionStyleType.DRAW_STYLE_TYPE_3)
      let brush = new drawing.Brush();
      this.ApplyBrushStyle (brush, TestFunctionStyleType.DRAW_STYLE_TYPE_3)
      canvas.attachPen(pen)
      canvas.attachBrush(brush)
      let pixelMap: image.PixelMap = globalThis.getInstance().getPixelMap("test_3.jpg")
      canvas.drawImage(pixelMap, 200, 233.97);
      canvas.detachPen()
      canvas.detachBrush()
    }

    {
      let pen = new drawing.Pen();
      this.ApplyPenStyle(pen, TestFunctionStyleType.DRAW_STYLE_TYPE_4)
      let brush = new drawing.Brush();
      this.ApplyBrushStyle (brush, TestFunctionStyleType.DRAW_STYLE_TYPE_4)
      canvas.attachPen(pen)
      canvas.attachBrush(brush)
      let pixelMap: image.PixelMap = globalThis.getInstance().getPixelMap("test_4.jpg")
      canvas.drawImage(pixelMap, 140, 17.8);
      canvas.detachPen()
      canvas.detachBrush()
    }
  }
}

export class CanvasDrawImageRect extends TestBase {

  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    var problem: boolean = false;
    let pixelMap: image.PixelMap = globalThis.getInstance().getPixelMap("test_1.jpg");
    let pmSize: image.Size = pixelMap.getImageInfoSync().size;
    let rect: common2D.Rect = { left: 20, top: 20, right: (pmSize.width / 4), bottom: (pmSize.height / 4) };
    for (let i = 0; i < this.testCount_; i++) {
      try {
        canvas.drawImageRect(pixelMap, rect);
      } catch (err) {
        console.error("CanvasDrawImageRect.OnTestFunction: exception on test 1: ", err.name, ":", err.message, err.stack);
        problem = true;
      }
    }
    printResults(canvas, !problem);
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    var problem: boolean = false;
    try {
      let pen = new drawing.Pen();
      this.ApplyPenStyle(pen, TestFunctionStyleType.DRAW_STYLE_TYPE_3);
      canvas.attachPen(pen);
      let pixelMap: image.PixelMap = globalThis.getInstance().getPixelMap("test_1.jpg");
      let pmSize: image.Size = pixelMap.getImageInfoSync().size;
      let rect: common2D.Rect = { left: 0, top: 0, right: pmSize.width, bottom: pmSize.height };
      canvas.drawImageRect(pixelMap, rect);
      canvas.detachPen();
    } catch (err) {
      console.error("CanvasDrawImageRect.OnTestFunction: exception on test 1: ", err.name, ":", err.message, err.stack);
      problem = true;
    }

    try {
      let pen = new drawing.Pen();
      this.ApplyPenStyle(pen, TestFunctionStyleType.DRAW_STYLE_TYPE_2);
      let brush = new drawing.Brush();
      this.ApplyBrushStyle(brush, TestFunctionStyleType.DRAW_STYLE_TYPE_2);
      canvas.attachPen(pen);
      canvas.attachBrush(brush);
      let pixelMap: image.PixelMap = globalThis.getInstance().getPixelMap("test_2.jpg");
      let pmSize: image.Size = pixelMap.getImageInfoSync().size;
      let rect: common2D.Rect = { left: 100, top: 150, right: pmSize.width, bottom: pmSize.height };
      canvas.drawImageRect(pixelMap, rect);
      canvas.detachPen();
      canvas.detachBrush();
    } catch (err) {
      console.error("CanvasDrawImageRect.OnTestFunction: exception on test 2: ", err.name, ":", err.message, err.stack);
      problem = true;
    }

    try {
      let pen = new drawing.Pen();
      this.ApplyPenStyle(pen, TestFunctionStyleType.DRAW_STYLE_TYPE_3);
      let brush = new drawing.Brush();
      this.ApplyBrushStyle (brush, TestFunctionStyleType.DRAW_STYLE_TYPE_3);
      canvas.attachPen(pen);
      canvas.attachBrush(brush);
      let pixelMap: image.PixelMap = globalThis.getInstance().getPixelMap("test_3.jpg");
      let pmSize: image.Size = pixelMap.getImageInfoSync().size;
      let rect: common2D.Rect = { left: 200, top: 233.97, right: pmSize.width, bottom: pmSize.height };
      canvas.drawImageRect(pixelMap, rect);
      canvas.detachPen();
      canvas.detachBrush();
    } catch (err) {
      console.error("CanvasDrawImageRect.OnTestFunction: exception on test 3: ", err.name, ":", err.message, err.stack);
      problem = true;
    }

    try {
      let pen = new drawing.Pen();
      this.ApplyPenStyle(pen, TestFunctionStyleType.DRAW_STYLE_TYPE_4);
      let brush = new drawing.Brush();
      this.ApplyBrushStyle (brush, TestFunctionStyleType.DRAW_STYLE_TYPE_4);
      canvas.attachPen(pen);
      canvas.attachBrush(brush);
      let pixelMap: image.PixelMap = globalThis.getInstance().getPixelMap("test_4.jpg");
      let pmSize: image.Size = pixelMap.getImageInfoSync().size;
      let rect: common2D.Rect = { left: 140, top: 17.8, right: pmSize.width, bottom: pmSize.height };
      canvas.drawImageRect(pixelMap, rect);
      canvas.detachPen();
      canvas.detachBrush();
    } catch (err) {
      console.error("CanvasDrawImageRect.OnTestFunction: exception on test 4: ", err.name, ":", err.message, err.stack);
      problem = true;
    }
    printResults(canvas, !problem);
  }
}

export class CanvasDrawImageRectWithSrc extends TestBase {

  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }
  public OnTestPerformance(canvas: drawing.Canvas) {
    var problem: boolean = false;
    let pixelMap: image.PixelMap = globalThis.getInstance().getPixelMap("test_1.jpg");
    let pmSize: image.Size = pixelMap.getImageInfoSync().size;
    let srcRect: common2D.Rect = { left: 0, top: 0, right: (pmSize.width / 2), bottom: (pmSize.height / 2) };
    let dstRect: common2D.Rect = { left: 20, top: 20, right: (pmSize.width / 4), bottom: (pmSize.height / 4) };
    let opts = new drawing.SamplingOptions(drawing.FilterMode.FILTER_MODE_LINEAR);
    let constraint: drawing.SrcRectConstraint;
    for (let i = 0; i < this.testCount_; i++) {
      constraint = ((i % 2) == 0) ? drawing.SrcRectConstraint.STRICT : drawing.SrcRectConstraint.FAST;
      try {
        canvas.drawImageRectWithSrc(pixelMap, srcRect, dstRect, opts, constraint);
      } catch (err) {
        console.error("CanvasDrawImageRectWithSrc.OnTestFunction: exception on test 1: ",
          err.name, ":", err.message, err.stack);
        problem = true;
      }
    }
    printResults(canvas, !problem);
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    var problem: boolean = false;

    try {
      let pen = new drawing.Pen();
      this.ApplyPenStyle(pen, TestFunctionStyleType.DRAW_STYLE_TYPE_3);
      canvas.attachPen(pen);
      let pixelMap: image.PixelMap = globalThis.getInstance().getPixelMap("test_1.jpg");
      let pmSize: image.Size = pixelMap.getImageInfoSync().size;
      let srcRect: common2D.Rect = { left: 0, top: 0, right: (pmSize.width / 2), bottom: (pmSize.height / 2) };
      let dstRect: common2D.Rect = { left: 0, top: 0, right: pmSize.width, bottom: pmSize.height };
      canvas.drawImageRectWithSrc(pixelMap, srcRect, dstRect);
      canvas.detachPen();
    } catch (err) {
      console.error("CanvasDrawImageRectWithSrc.OnTestFunction: exception on test 1: ",
        err.name, ":", err.message, err.stack);
      problem = true;
    }

    try {
      let pen = new drawing.Pen();
      this.ApplyPenStyle(pen, TestFunctionStyleType.DRAW_STYLE_TYPE_2);
      let brush = new drawing.Brush();
      this.ApplyBrushStyle(brush, TestFunctionStyleType.DRAW_STYLE_TYPE_2);
      canvas.attachPen(pen);
      canvas.attachBrush(brush);
      let pixelMap: image.PixelMap = globalThis.getInstance().getPixelMap("test_2.jpg");
      let pmSize: image.Size = pixelMap.getImageInfoSync().size;
      let srcRect: common2D.Rect = { left: 0, top: 0, right: (pmSize.width / 2), bottom: (pmSize.height / 2) };
      let dstRect: common2D.Rect = { left: 100, top: 150, right: pmSize.width, bottom: pmSize.height };
      canvas.drawImageRectWithSrc(pixelMap, srcRect, dstRect);
      canvas.detachPen();
      canvas.detachBrush();
    } catch (err) {
      console.error("CanvasDrawImageRectWithSrc.OnTestFunction: exception on test 2: ",
        err.name, ":", err.message, err.stack);
      problem = true;
    }

    try {
      let pen = new drawing.Pen();
      this.ApplyPenStyle(pen, TestFunctionStyleType.DRAW_STYLE_TYPE_3);
      let brush = new drawing.Brush();
      this.ApplyBrushStyle (brush, TestFunctionStyleType.DRAW_STYLE_TYPE_3);
      canvas.attachPen(pen);
      canvas.attachBrush(brush);
      let pixelMap: image.PixelMap = globalThis.getInstance().getPixelMap("test_3.jpg");
      let pmSize: image.Size = pixelMap.getImageInfoSync().size;
      let srcRect: common2D.Rect = { left: 0, top: 0, right: (pmSize.width / 2), bottom: (pmSize.height / 2) };
      let dstRect: common2D.Rect = { left: 200, top: 233.97, right: pmSize.width, bottom: pmSize.height };
      canvas.drawImageRectWithSrc(pixelMap, srcRect, dstRect);
      canvas.detachPen();
      canvas.detachBrush();
    } catch (err) {
      console.error("CanvasDrawImageRectWithSrc.OnTestFunction: exception on test 3: ",
        err.name, ":", err.message, err.stack);
      problem = true;
    }

    try {
      let pen = new drawing.Pen();
      this.ApplyPenStyle(pen, TestFunctionStyleType.DRAW_STYLE_TYPE_4);
      let brush = new drawing.Brush();
      this.ApplyBrushStyle (brush, TestFunctionStyleType.DRAW_STYLE_TYPE_4);
      canvas.attachPen(pen);
      canvas.attachBrush(brush);
      let pixelMap: image.PixelMap = globalThis.getInstance().getPixelMap("test_4.jpg");
      let pmSize: image.Size = pixelMap.getImageInfoSync().size;
      let srcRect: common2D.Rect = { left: 0, top: 0, right: (pmSize.width / 2), bottom: (pmSize.height / 2) };
      let dstRect: common2D.Rect = { left: 140, top: 17.8, right: pmSize.width, bottom: pmSize.height };
      canvas.drawImageRectWithSrc(pixelMap, srcRect, dstRect);
      canvas.detachPen();
      canvas.detachBrush();
    } catch (err) {
      console.error("CanvasDrawImageRectWithSrc.OnTestFunction: exception on test 4: ",
        err.name, ":", err.message, err.stack);
      problem = true;
    }
    printResults(canvas, !problem);
  }
}

export class CanvasDrawCircle extends TestBase {

  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE){
    super();
    this.styleType_ = styleType;
  }
  public OnTestPerformance(canvas: drawing.Canvas) {
    // 保证性能测试稳定性: 绘制圆心（200, 200）半径100的圆
    for (let i = 0; i < this.testCount_; i++) {
      canvas.drawCircle(200, 200, 100);
    }
  }
  
  public OnTestFunction(canvas: drawing.Canvas) {
    //接口调用，功能测试.cpu/gpu调用接口一致
    {
      let pen = new drawing.Pen();
      this.ApplyPenStyle(pen, TestFunctionStyleType.DRAW_STYLE_TYPE_1)
      let brush = new drawing.Brush();
      this.ApplyBrushStyle(brush, TestFunctionStyleType.DRAW_STYLE_TYPE_1)
      canvas.attachPen(pen)
      canvas.attachBrush(brush)
      canvas.drawCircle(10, 10, 50)
      canvas.detachPen()
      canvas.detachBrush()
    }
    
    {
      let pen = new drawing.Pen();
      this.ApplyPenStyle(pen, TestFunctionStyleType.DRAW_STYLE_TYPE_2)
      let brush = new drawing.Brush();
      this.ApplyBrushStyle(brush, TestFunctionStyleType.DRAW_STYLE_TYPE_2)
      canvas.attachPen(pen)
      canvas.attachBrush(brush)
      canvas.drawCircle(102.6, 67.6, 60.8)
      canvas.detachPen()
      canvas.detachBrush()
    }

    {
      let pen = new drawing.Pen();
      this.ApplyPenStyle(pen, TestFunctionStyleType.DRAW_STYLE_TYPE_3)
      let brush = new drawing.Brush();
      this.ApplyBrushStyle (brush, TestFunctionStyleType.DRAW_STYLE_TYPE_3)
      canvas.attachPen(pen)
      canvas.attachBrush(brush)
      canvas.drawCircle(350.6, 238.6, 265.8)
      canvas.detachPen()
      canvas.detachBrush()
    }

    {
      let pen = new drawing.Pen();
      this.ApplyPenStyle(pen, TestFunctionStyleType.DRAW_STYLE_TYPE_4)
      let brush = new drawing.Brush();
      this.ApplyBrushStyle (brush, TestFunctionStyleType.DRAW_STYLE_TYPE_4)
      canvas.attachPen(pen)
      canvas.attachBrush(brush)
      canvas.drawCircle(50.6, 496.6, 67.8)
      canvas.detachPen()
      canvas.detachBrush()
    }
  }

}

export class BrushGetColorFilter extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let clr: common2D.Color = { alpha: 255, red: 0, green: 255, blue: 0 };
    let brush = new drawing.Brush();
    let setColorFilter = drawing.ColorFilter.createSRGBGammaToLinear();
    brush.setColor(clr);
    brush.setColorFilter(setColorFilter);
    var problem: Boolean = false;
    for (let i = 0; i < this.testCount_; i++) {
      try {
        let brushColorFilter = brush.getColorFilter();
        if (brushColorFilter == null) {
          console.log("BrushGetColorFilter failed: getColorFilter is null");
          problem = true;
          break;
        }
      } catch (exc) {
        console.log("BrushGetColorFilter exception: " + exc);
        problem = true;
        break;
      }
    }
    printResults(canvas, !problem);
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    var problem: Boolean = false;
    let brush = new drawing.Brush();
    let color: common2D.Color = { alpha: 80, red: 255, green: 255, blue: 0 };
    let cf = drawing.ColorFilter.createLumaColorFilter();
    try {
      brush.setColor(color);
      brush.setColorFilter(cf);
      let cf2 = brush.getColorFilter();
      if (cf2 == null) {
        console.log("BrushGetColorFilter failed: ColorFilter for brush is null");
        problem = true;
      }
      canvas.attachBrush(brush);
      canvas.drawCircle(200, 550, 50);
      canvas.detachBrush();

      let brush2 = new drawing.Brush();
      brush2.setColor(color);
      brush2.setColorFilter(cf2);
      canvas.attachBrush(brush2);
      canvas.drawCircle(400, 550, 50);
      canvas.detachBrush();
    } catch (exc) {
      console.log("BrushGetColorFilter exception: " + exc);
      problem = true;
    }
    printResults(canvas, !problem);
  }
}

export class BrushSetImageFilter extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let brush = new drawing.Brush();
    let clr: common2D.Color = { alpha: 255, red: 255, green: 255, blue: 0 };
    let imgFilter = drawing.ImageFilter.createBlurImageFilter(5, 10, drawing.TileMode.CLAMP, null);
    brush.setColor(clr);
    var problem: Boolean = false;
    for (let i = 0; i < this.testCount_; i++) {
      try {
        brush.setImageFilter(imgFilter);
      } catch (exc) {
        console.log("BrushSetImageFilter exception: " + exc);
        problem = true;
        break;
      }
    }
    printResults(canvas, !problem);
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    var problem: Boolean = false;
    try {
      {
        let imgFilter1 = drawing.ImageFilter.createBlurImageFilter(5, 10, drawing.TileMode.CLAMP, null);
        let brush = new drawing.Brush();
        let color: common2D.Color = { alpha: 255, red: 255, green: 0, blue: 0 };
        brush.setColor(color);
        brush.setAntiAlias(true);
        brush.setImageFilter(imgFilter1);
        canvas.attachBrush(brush);
        canvas.drawCircle(150, 250, 50);
        canvas.detachBrush();
      }
      {
        let imgFilter1 = drawing.ImageFilter.createBlurImageFilter(10, 5, drawing.TileMode.REPEAT, null);
        let brush = new drawing.Brush();
        let color: common2D.Color = { alpha: 255, red: 0, green: 255, blue: 0 };
        brush.setColor(color);
        brush.setAntiAlias(true);
        brush.setImageFilter(imgFilter1);
        canvas.attachBrush(brush);
        canvas.drawCircle(400, 250, 50);
        canvas.detachBrush();
      }
      {
        let imgFilter1 = drawing.ImageFilter.createBlurImageFilter(10, 5, drawing.TileMode.MIRROR, null);
        let cf = drawing.ColorFilter.createSRGBGammaToLinear();
        let brush = new drawing.Brush();
        let color: common2D.Color = { alpha: 255, red: 0, green: 255, blue: 255 };
        brush.setColor(color);
        brush.setAntiAlias(true);
        brush.setColorFilter(cf);
        brush.setImageFilter(imgFilter1);
        canvas.attachBrush(brush);
        canvas.drawCircle(150, 550, 50);
        canvas.detachBrush();
      }
      {
        let imgFilter1 = drawing.ImageFilter.createBlurImageFilter(5, 10, drawing.TileMode.DECAL, null);
        let cf = drawing.ColorFilter.createSRGBGammaToLinear();
        let brush = new drawing.Brush();
        let color: common2D.Color = { alpha: 255, red: 255, green: 255, blue: 0 };
        brush.setColor(color);
        brush.setAntiAlias(true);
        brush.setColorFilter(cf);
        brush.setImageFilter(imgFilter1);
        canvas.attachBrush(brush);
        canvas.drawCircle(400, 550, 50);
        canvas.detachBrush();
      }
    } catch (exc) {
      console.log("BrushSetImageFilter exception: " + exc);
      problem = true;
    }
    printResults(canvas, !problem);
  }
}

export class PenGetColorFilter extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let clr: common2D.Color = { alpha: 255, red: 0, green: 255, blue: 0 };
    let pen = new drawing.Pen();
    let setColorFilter = drawing.ColorFilter.createSRGBGammaToLinear();
    pen.setColor(clr);
    pen.setColorFilter(setColorFilter);
    var problem: Boolean = false;
    for (let i = 0; i < this.testCount_; i++) {
      try {
        let penColorFilter = pen.getColorFilter();
        if (penColorFilter == null) {
          console.log("PenGetColorFilter failed: getColorFilter is null");
          problem = true;
          break;
        }
      } catch (exc) {
        console.log("PenGetColorFilter exception: " + exc);
        problem = true;
        break;
      }
    }
    printResults(canvas, !problem);
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    var problem: Boolean = false;
    let pen = new drawing.Pen();
    let color: common2D.Color = { alpha: 80, red: 255, green: 255, blue: 0 };
    let cf = drawing.ColorFilter.createLumaColorFilter();
    try {
      pen.setColor(color);
      pen.setColorFilter(cf);
      pen.setStrokeWidth(30);
      let cf2 = pen.getColorFilter();
      if (cf2 == null) {
        console.log("PenGetColorFilter failed: ColorFilter for pen is null");
        problem = true;
      }
      canvas.attachPen(pen);
      canvas.drawCircle(200, 550, 50);
      canvas.detachPen();

      let pen2 = new drawing.Pen();
      pen2.setColor(color);
      pen2.setColorFilter(cf2);
      pen2.setStrokeWidth(30);
      canvas.attachPen(pen2);
      canvas.drawCircle(400, 550, 50);
      canvas.detachPen();
    } catch (exc) {
      console.log("PenGetColorFilter exception: " + exc);
      problem = true;
    }
    printResults(canvas, !problem);
  }
}

export class PenSetImageFilter extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let pen = new drawing.Pen();
    let clr: common2D.Color = { alpha: 255, red: 255, green: 255, blue: 0 };
    let imgFilter = drawing.ImageFilter.createBlurImageFilter(5, 10, drawing.TileMode.CLAMP, null);
    pen.setColor(clr);
    var problem: Boolean = false;
    for (let i = 0; i < this.testCount_; i++) {
      try {
        pen.setImageFilter(imgFilter);
      } catch (exc) {
        console.log("PenSetImageFilter exception: " + exc);
        problem = true;
        break;
      }
    }
    printResults(canvas, !problem);
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    var problem: Boolean = false;
    try {
      {
        let imgFilter1 = drawing.ImageFilter.createBlurImageFilter(5, 10, drawing.TileMode.CLAMP, null);
        let clr: common2D.Color = { alpha: 255, red: 0, green: 255, blue: 0 };
        let cf = drawing.ColorFilter.createSRGBGammaToLinear();
        let imgFilter2 = drawing.ImageFilter.createFromColorFilter(cf, imgFilter1);
        let pen = new drawing.Pen();
        pen.setColor(clr);
        pen.setStrokeWidth(20);
        pen.setColorFilter(cf);
        pen.setImageFilter(imgFilter2);
        canvas.attachPen(pen);
        canvas.drawCircle(150, 150, 50);
        canvas.detachPen();
      }
      {
        let imgFilter1 = drawing.ImageFilter.createBlurImageFilter(10, 5, drawing.TileMode.REPEAT, null);
        let clr: common2D.Color = { alpha: 255, red: 0, green: 0, blue: 255 };
        let cf = drawing.ColorFilter.createSRGBGammaToLinear();
        let imgFilter2 = drawing.ImageFilter.createFromColorFilter(cf, imgFilter1);
        let pen = new drawing.Pen();
        pen.setColor(clr);
        pen.setStrokeWidth(20);
        pen.setColorFilter(cf);
        pen.setImageFilter(imgFilter2);
        canvas.attachPen(pen);
        canvas.drawCircle(400, 150, 50);
        canvas.detachPen();
      }
      {
        let imgFilter1 = drawing.ImageFilter.createBlurImageFilter(10, 5, drawing.TileMode.MIRROR, null);
        let clr: common2D.Color = { alpha: 255, red: 255, green: 255, blue: 0 };
        let cf = drawing.ColorFilter.createSRGBGammaToLinear();
        let imgFilter2 = drawing.ImageFilter.createFromColorFilter(cf, imgFilter1);
        let pen = new drawing.Pen();
        pen.setColor(clr);
        pen.setStrokeWidth(20);
        pen.setImageFilter(imgFilter2);
        canvas.attachPen(pen);
        canvas.drawCircle(150, 450, 50);
        canvas.detachPen();
      }
      {
        let imgFilter1 = drawing.ImageFilter.createBlurImageFilter(5, 10, drawing.TileMode.DECAL, null);
        let clr: common2D.Color = { alpha: 255, red: 255, green: 0, blue: 255 };
        let cf = drawing.ColorFilter.createSRGBGammaToLinear();
        let imgFilter2 = drawing.ImageFilter.createFromColorFilter(cf, imgFilter1);
        let pen = new drawing.Pen();
        pen.setColor(clr);
        pen.setStrokeWidth(20);
        pen.setImageFilter(imgFilter2);
        canvas.attachPen(pen);
        canvas.drawCircle(400, 450, 50);
        canvas.detachPen();
      }
    } catch (exc) {
      console.log("PenSetImageFilter exception: " + exc);
      problem = true;
    }
    printResults(canvas, !problem);
  }
}

export class CreateBlurImageFilter extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    var problem: Boolean = false;
    for (let i = 0; i < this.testCount_; i++) {
      try {
        let imgFilter = drawing.ImageFilter.createBlurImageFilter(5, 10, drawing.TileMode.CLAMP, null);
        if (imgFilter == null) {
          console.log("CreateBlurImageFilter failed: createBlurImageFilter is null");
          problem = true;
          break;
        }
      } catch (exc) {
        console.log("CreateBlurImageFilter exception: " + exc);
        problem = true;
        break;
      }
    }
    printResults(canvas, !problem);
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    var problem: Boolean = false
    try {
      {
        let imgFilter1 = drawing.ImageFilter.createBlurImageFilter(5, 10, drawing.TileMode.CLAMP, null);
        if (imgFilter1 == null) {
          console.log("CreateBlurImageFilter failed: createBlurImageFilter is null");
          problem = true;
        }
        let brush = new drawing.Brush();
        let color: common2D.Color = { alpha: 255, red: 255, green: 0, blue: 0 };
        brush.setColor(color);
        brush.setAntiAlias(true);
        brush.setImageFilter(imgFilter1);
        canvas.attachBrush(brush);
        canvas.drawCircle(150, 250, 50);
        canvas.detachBrush();
      }
      {
        let imgFilter1 = drawing.ImageFilter.createBlurImageFilter(10, 5, drawing.TileMode.REPEAT, null);
        if (imgFilter1 == null) {
          console.log("CreateBlurImageFilter failed: createBlurImageFilter is null");
          problem = true;
        }
        let brush = new drawing.Brush();
        let color: common2D.Color = { alpha: 255, red: 0, green: 255, blue: 0 };
        brush.setColor(color);
        brush.setAntiAlias(true);
        brush.setImageFilter(imgFilter1);
        canvas.attachBrush(brush);
        canvas.drawCircle(400, 250, 50);
        canvas.detachBrush();
      }
      {
        let imgFilter1 = drawing.ImageFilter.createBlurImageFilter(10, 5, drawing.TileMode.MIRROR, null);
        if (imgFilter1 == null) {
          console.log("CreateBlurImageFilter failed: createBlurImageFilter is null");
          problem = true;
        }
        let cf = drawing.ColorFilter.createSRGBGammaToLinear();
        let brush = new drawing.Brush();
        let color: common2D.Color = { alpha: 255, red: 0, green: 255, blue: 255 };
        brush.setColor(color);
        brush.setAntiAlias(true);
        brush.setColorFilter(cf);
        brush.setImageFilter(imgFilter1);
        canvas.attachBrush(brush);
        canvas.drawCircle(150, 550, 50);
        canvas.detachBrush();
      }
      {
        let imgFilter1 = drawing.ImageFilter.createBlurImageFilter(5, 10, drawing.TileMode.DECAL, null);
        if (imgFilter1 == null) {
          console.log("CreateBlurImageFilter failed: createBlurImageFilter is null");
          problem = true;
        }
        let cf = drawing.ColorFilter.createSRGBGammaToLinear();
        let brush = new drawing.Brush();
        let color: common2D.Color = { alpha: 255, red: 255, green: 255, blue: 0 };
        brush.setColor(color);
        brush.setAntiAlias(true);
        brush.setColorFilter(cf);
        brush.setImageFilter(imgFilter1);
        canvas.attachBrush(brush);
        canvas.drawCircle(400, 550, 50);
        canvas.detachBrush();
      }
    } catch (exc) {
      console.log("CreateBlurImageFilter exception: " + exc);
      problem = true;
    }
    printResults(canvas, !problem);
  }
}

export class CreateColorImageFilter extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let colorFilter = drawing.ColorFilter.createSRGBGammaToLinear();
    let imgFilter1 = drawing.ImageFilter.createBlurImageFilter(5, 10, drawing.TileMode.CLAMP, null);
    var problem: Boolean = false;
    for (let i = 0; i < this.testCount_; i++) {
      try {
        let imgFilter2 = drawing.ImageFilter.createFromColorFilter(colorFilter, imgFilter1);
        if (imgFilter2 == null) {
          console.log("CreateColorImageFilter failed: createFromColorFilter is null");
          problem = true;
          break;
        }
      } catch (exc) {
        console.log("CreateColorImageFilter exception: " + exc);
        problem = true;
        break;
      }
    }
    printResults(canvas, !problem);
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    var problem: Boolean = false;
    try {
      {
        let imgFilter1 = drawing.ImageFilter.createBlurImageFilter(5, 10, drawing.TileMode.CLAMP, null);
        let brush = new drawing.Brush();
        let color: common2D.Color = { alpha: 255, red: 255, green: 0, blue: 0 };
        brush.setColor(color);
        brush.setAntiAlias(true);
        brush.setImageFilter(imgFilter1);
        canvas.attachBrush(brush);
        canvas.drawCircle(150, 250, 50);
        canvas.detachBrush();

        let clr: common2D.Color = { alpha: 255, red: 0, green: 255, blue: 0 };
        let cf = drawing.ColorFilter.createSRGBGammaToLinear();
        let imgFilter2 = drawing.ImageFilter.createFromColorFilter(cf, imgFilter1);
        if (imgFilter2 == null) {
          console.log("CreateColorImageFilter failed: createFromColorFilter is null");
          problem = true;
        }
        let pen = new drawing.Pen();
        pen.setColor(clr);
        pen.setStrokeWidth(20);
        pen.setColorFilter(cf);
        pen.setImageFilter(imgFilter2);
        canvas.attachPen(pen);
        canvas.drawCircle(150, 150, 50);
        canvas.detachPen();
      }
      {
        let imgFilter1 = drawing.ImageFilter.createBlurImageFilter(10, 5, drawing.TileMode.REPEAT, null);
        let brush = new drawing.Brush();
        let color: common2D.Color = { alpha: 255, red: 0, green: 255, blue: 0 };
        brush.setColor(color);
        brush.setAntiAlias(true);
        brush.setImageFilter(imgFilter1);
        canvas.attachBrush(brush);
        canvas.drawCircle(400, 250, 50);
        canvas.detachBrush();

        let clr: common2D.Color = { alpha: 255, red: 0, green: 0, blue: 255 };
        let cf = drawing.ColorFilter.createSRGBGammaToLinear();
        let imgFilter2 = drawing.ImageFilter.createFromColorFilter(cf, imgFilter1);
        if (imgFilter2 == null) {
          console.log("CreateColorImageFilter failed: createFromColorFilter is null");
          problem = true;
        }
        let pen = new drawing.Pen();
        pen.setColor(clr);
        pen.setStrokeWidth(20);
        pen.setColorFilter(cf);
        pen.setImageFilter(imgFilter2);
        canvas.attachPen(pen);
        canvas.drawCircle(400, 150, 50);
        canvas.detachPen();
      }
      {
        let imgFilter1 = drawing.ImageFilter.createBlurImageFilter(10, 5, drawing.TileMode.MIRROR, null);
        let clr: common2D.Color = { alpha: 255, red: 255, green: 255, blue: 0 };
        let cf = drawing.ColorFilter.createSRGBGammaToLinear();
        let imgFilter2 = drawing.ImageFilter.createFromColorFilter(cf, imgFilter1);
        if (imgFilter2 == null) {
          console.log("CreateColorImageFilter failed: createFromColorFilter is null");
          problem = true;
        }
        let pen = new drawing.Pen();
        pen.setColor(clr);
        pen.setStrokeWidth(20);
        pen.setImageFilter(imgFilter2);
        canvas.attachPen(pen);
        canvas.drawCircle(150, 450, 50);
        canvas.detachPen();

        let brush = new drawing.Brush();
        let color: common2D.Color = { alpha: 255, red: 0, green: 255, blue: 255 };
        brush.setColor(color);
        brush.setAntiAlias(true);
        brush.setColorFilter(cf);
        brush.setImageFilter(imgFilter1);
        canvas.attachBrush(brush);
        canvas.drawCircle(150, 550, 50);
        canvas.detachBrush();
      }
      {
        let imgFilter1 = drawing.ImageFilter.createBlurImageFilter(5, 10, drawing.TileMode.DECAL, null);
        let clr: common2D.Color = { alpha: 255, red: 255, green: 0, blue: 255 };
        let cf = drawing.ColorFilter.createSRGBGammaToLinear();
        let imgFilter2 = drawing.ImageFilter.createFromColorFilter(cf, imgFilter1);
        if (imgFilter2 == null) {
          console.log("CreateColorImageFilter failed: createFromColorFilter is null");
          problem = true;
        }
        let pen = new drawing.Pen();
        pen.setColor(clr);
        pen.setStrokeWidth(20);
        pen.setImageFilter(imgFilter2);
        canvas.attachPen(pen);
        canvas.drawCircle(400, 450, 50);
        canvas.detachPen();

        let brush = new drawing.Brush();
        let color: common2D.Color = { alpha: 255, red: 255, green: 255, blue: 0 };
        brush.setColor(color);
        brush.setAntiAlias(true);
        brush.setColorFilter(cf);
        brush.setImageFilter(imgFilter1);
        canvas.attachBrush(brush);
        canvas.drawCircle(400, 550, 50);
        canvas.detachBrush();
      }
    } catch (exc) {
      console.log("CreateColorImageFilter exception: " + exc);
      problem = true;
    }
    printResults(canvas, !problem);
  }
}

export class CanvasDrawTextBlob extends TestBase {

  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE){
    super();
    this.styleType_ = styleType;
  }
  public OnTestPerformance(canvas: drawing.Canvas) {
    // 保证性能测试稳定性: 绘制'textblob'文字
    let text: string = 'textblob';
    const font: drawing.Font = new drawing.Font();
    font.setSize(20);
    const textBlob = drawing.TextBlob.makeFromString(text, font, drawing.TextEncoding.TEXT_ENCODING_UTF8);
    for (let i = 0; i < this.testCount_; i++) {
      canvas.drawTextBlob(textBlob, 0, 0);
    }
  }
  
  public OnTestFunction(canvas: drawing.Canvas) {
    //接口调用，功能测试.cpu/gpu调用接口一致
    {
      let brush = new drawing.Brush();
      this.ApplyBrushStyle(brush, TestFunctionStyleType.DRAW_STYLE_TYPE_1)
      canvas.attachBrush(brush)
      let font : drawing.Font = new drawing.Font();
      let text : string = 'Hello World, This is test code!';
      font.setTypeface(new drawing.Typeface());
      font.enableSubpixel(true);
      let blob : drawing.TextBlob = drawing.TextBlob.makeFromString(text, font, drawing.TextEncoding.TEXT_ENCODING_UTF8);
      canvas.drawTextBlob(blob, 500, 50);
      canvas.detachBrush()
    }
    
    {
      let pen = new drawing.Pen();
      this.ApplyPenStyle(pen, TestFunctionStyleType.DRAW_STYLE_TYPE_3)
      let brush = new drawing.Brush();
      this.ApplyBrushStyle(brush, TestFunctionStyleType.DRAW_STYLE_TYPE_2)
      canvas.attachPen(pen)
      canvas.attachBrush(brush)
      let font : drawing.Font = new drawing.Font();
      let text : string = '你好呀,这是测试代码!';
      font.setTypeface(new drawing.Typeface());
      font.enableSubpixel(true)
      font.enableEmbolden(true)
      font.setSize(10)
      let blob : drawing.TextBlob = drawing.TextBlob.makeFromString(text, font, drawing.TextEncoding.TEXT_ENCODING_UTF8);
      canvas.drawTextBlob(blob, 100, 100);
      canvas.detachPen()
      canvas.detachBrush()
    }

    {
      let pen = new drawing.Pen();
      this.ApplyPenStyle(pen, TestFunctionStyleType.DRAW_STYLE_TYPE_3)
      let brush = new drawing.Brush();
      this.ApplyBrushStyle (brush, TestFunctionStyleType.DRAW_STYLE_TYPE_3)
      canvas.attachPen(pen)
      canvas.attachBrush(brush)

      let font : drawing.Font = new drawing.Font();
      let text : string = 'Hello World,这是测试代码!';
      font.setTypeface(new drawing.Typeface());
      font.enableSubpixel(false)
      font.enableEmbolden(false)
      font.enableLinearMetrics(false)
      font.setSize(30)
      let blob : drawing.TextBlob = drawing.TextBlob.makeFromString(text, font, drawing.TextEncoding.TEXT_ENCODING_UTF8);
      canvas.drawTextBlob(blob, 183.15, 238);

      canvas.detachPen()
      canvas.detachBrush()
    }

    {
      let pen = new drawing.Pen();
      this.ApplyPenStyle(pen, TestFunctionStyleType.DRAW_STYLE_TYPE_4)
      let brush = new drawing.Brush();
      this.ApplyBrushStyle (brush, TestFunctionStyleType.DRAW_STYLE_TYPE_4)
      canvas.attachPen(pen)
      canvas.attachBrush(brush)

      let font : drawing.Font = new drawing.Font();
      let text : string = 'Hello World,这是测试代码!';
      font.setTypeface(new drawing.Typeface());
      font.enableSubpixel(false)
      font.enableEmbolden(false)
      font.enableLinearMetrics(false)
      font.setSize(5)
      let blob : drawing.TextBlob = drawing.TextBlob.makeFromString(text, font, drawing.TextEncoding.TEXT_ENCODING_UTF8);
      canvas.drawTextBlob(blob, 183.15, 379.4);

      canvas.detachPen()
      canvas.detachBrush()
    }
  }
}

export class CanvasDrawPixelMapMesh extends TestBase {

  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE){
    super();
    this.styleType_ = styleType;
  }
  public OnTestPerformance(canvas: drawing.Canvas) {
    // 保证性能测试稳定性: 绘制1*1PixelMapMesh
    let pixelMap: image.PixelMap = globalThis.getInstance().getPixelMap("test_1.jpg")
    const brush = new drawing.Brush();
    canvas.attachBrush(brush);
    let verts: Array<number> = [100, 100, 200, 100, 150, 200, 200, 200];
    let colors: Array<number> = [0x00ff0088, 0x00ff0088, 0x00ff0088, 0x00ff0088];
    for (let i = 0; i < this.testCount_; i++) {
      canvas.drawPixelMapMesh(pixelMap, 1, 1, verts, 0, null, 0);
    }
    canvas.detachBrush();
  }
}

export class CanvasDrawColor extends TestBase {

  public constructor() {
    // 根据需求，如果与默认值不一样，请继承重写
    super();   
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    //接口调用，功能测试.cpu/gpu调用接口一致

    {
      let pen = new drawing.Pen();
      this.ApplyPenStyle(pen, TestFunctionStyleType.DRAW_STYLE_TYPE_2)
      let brush = new drawing.Brush();
      this.ApplyBrushStyle(brush, TestFunctionStyleType.DRAW_STYLE_TYPE_2)
      canvas.attachPen(pen)
      canvas.attachBrush(brush)
      canvas.drawCircle(102.6, 67.6, 60.8)
      canvas.detachPen()
      canvas.detachBrush()
    }
    
    {
      let rect: common2D.Rect = { left: 2.7, top: 175.7, right: 450.7, bottom: 315.9 };
      let pen = new drawing.Pen();
      this.ApplyPenStyle(pen, TestFunctionStyleType.DRAW_STYLE_TYPE_3)
      let brush = new drawing.Brush();
      this.ApplyBrushStyle (brush, TestFunctionStyleType.DRAW_STYLE_TYPE_3)
      canvas.attachPen(pen)
      canvas.attachBrush(brush)
      canvas.drawRect(rect)
      canvas.detachPen()
      canvas.detachBrush()
    }

    {
      let color: common2D.Color = { alpha: 255, red: 168, green: 48, blue: 176 };
      canvas.drawColor(color, drawing.BlendMode.MULTIPLY)
    }

  }

}

export class CanvasResetMatrix extends TestBase {
  public constructor() {
    super();
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let matrix = new drawing.Matrix();
    canvas.setMatrix(matrix);
    for (let i = 0; i < this.testCount_; i++) {
      canvas.resetMatrix();
    }
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    let rect1: common2D.Rect = { left: 10, top: 20, right: 30, bottom: 60 };
    let rect2: common2D.Rect = { left: 30, top: 20, right: 50, bottom: 60 };
    let brush = new drawing.Brush();
    let color: common2D.Color = { alpha: 255, red: 0, green: 255, blue: 0 };
    brush.setColor(color);
    canvas.attachBrush(brush);

    canvas.scale(4, 6);
    canvas.drawRect(rect1);

    brush.setColor({ alpha: 255, red: 0, green: 0, blue: 255 });
    canvas.attachBrush(brush);
    canvas.scale(2, 3);
    canvas.drawRect(rect2);

    canvas.resetMatrix();

    brush.setColor({ alpha: 255, red: 255, green: 0, blue: 0 });
    canvas.attachBrush(brush);
    canvas.scale(4, 6);
    canvas.drawRect(rect2);

    canvas.detachBrush();
    canvas.resetMatrix();
  }
}

export class CanvasSetMatrix extends TestBase {
  public constructor() {
    super();
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let matrix = new drawing.Matrix();
    for (let i = 0; i < this.testCount_; i++) {
      canvas.setMatrix(matrix);
    }
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    {
      let rect: common2D.Rect = { left: 10, top: 20, right: 30, bottom: 60 };
      let brush = new drawing.Brush();
      let color: common2D.Color = { alpha: 255, red: 255, green: 0, blue: 0 };
      let matrix = new drawing.Matrix();
      matrix.preRotate(-30, 10, 20)
      brush.setColor(color)
      canvas.attachBrush(brush)
      canvas.drawRect(rect)
      canvas.detachBrush()
      brush.setColor({ alpha: 255, red: 0, green: 255, blue: 0 })
      canvas.attachBrush(brush)
      canvas.setMatrix(matrix)
      canvas.drawRect(rect)
      canvas.detachBrush()
    }

    {
      let rect: common2D.Rect = { left: 10, top: 20, right: 30, bottom: 60 };
      let brush = new drawing.Brush();
      let color: common2D.Color = { alpha: 255, red: 255, green: 0, blue: 0 };
      let matrix = new drawing.Matrix();
      matrix.preTranslate(0, 60)
      canvas.setMatrix(matrix)
      matrix.preScale(2, 0.5, 0, 0)
      brush.setColor(color)
      canvas.attachBrush(brush)
      canvas.drawRect(rect)
      canvas.detachBrush()
      brush.setColor({ alpha: 255, red: 0, green: 255, blue: 0 })
      canvas.attachBrush(brush)
      canvas.setMatrix(matrix)
      canvas.drawRect(rect)
      canvas.detachBrush()
    }
  }
}

export class CanvasClipRoundRect extends TestBase {

  public constructor(){
    super();
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let rect: common2D.Rect = { left: 2.7, top: 175.7, right: 450.7, bottom: 315.9 };

    let rand: OHRandom = new OHRandom();
      for (let i = 0; i < this.testCount_; i++) {
        let l = i % this.width_;
        let t = (i + 100) % this.height_;
        let r = ((l + 100) > this.width_) ? this.width_ : (l + 100);
        let b = ((t + 100) > this.height_) ? this.height_ : (t + 100);
        let rect: common2D.Rect = {left: l, top: t, right: r, bottom: b};
        let roundRect = new drawing.RoundRect(rect, 10, 10);
        canvas.clipRoundRect(roundRect);
      }
  }
  
  public OnTestFunction(canvas: drawing.Canvas) {
      let pen = new drawing.Pen();
      this.ApplyPenStyle(pen, TestFunctionStyleType.DRAW_STYLE_TYPE_3);
      let brush = new drawing.Brush();
      this.ApplyBrushStyle (brush, TestFunctionStyleType.DRAW_STYLE_TYPE_3);
      canvas.attachPen(pen);
      canvas.attachBrush(brush);

      let rectOne: common2D.Rect = { left: 10, top: 100, right: 200, bottom: 300 };
      let roundRectOne = new drawing.RoundRect(rectOne, 10, 10);
      canvas.clipRoundRect(roundRectOne);
      canvas.drawRect(rectOne);

      let rectTwo: common2D.Rect = { left: 100, top: 200, right: 300, bottom: 400 };
      let roundRectTwo = new drawing.RoundRect(rectTwo, 10, 10);
      canvas.clipRoundRect(roundRectTwo, drawing.ClipOp.INTERSECT);
      canvas.drawRect(rectTwo);

      let rectThree: common2D.Rect = { left: 200, top: 300, right: 400, bottom: 500 };
      let roundRectThree = new drawing.RoundRect(rectThree, 10, 10);
      canvas.clipRoundRect(roundRectThree, drawing.ClipOp.INTERSECT);
      canvas.drawRect(rectThree);

      canvas.detachPen();
      canvas.detachBrush();
    }
}

export class FontIsSubpixel extends TestBase {

  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let font: drawing.Font = new drawing.Font();
    font.enableSubpixel(true);

    for (let i = 0; i < this.testCount_; i++) {
      font.isSubpixel();
    }
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    let brush = new drawing.Brush();
    canvas.attachBrush(brush);
    let font: drawing.Font = new drawing.Font();
    let text: string = 'Hello World, This is test code <isSubpixel>';
    font.setSize(30);
    font.enableSubpixel(true);

    if (font.isSubpixel()) {
      let blob: drawing.TextBlob = drawing.TextBlob.makeFromString(text, font, drawing.TextEncoding.TEXT_ENCODING_UTF8);
      canvas.drawTextBlob(blob, 50, 50);
    } else {
      console.error("isSubpixel() expected: true, actual: false");
    }

    canvas.detachBrush();
  }
}

export class FontIsLinearMetrics extends TestBase {

  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let font: drawing.Font = new drawing.Font();
    font.enableLinearMetrics(true);

    for (let i = 0; i < this.testCount_; i++) {
      font.isLinearMetrics();
    }
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    let brush = new drawing.Brush();
    canvas.attachBrush(brush);
    let font: drawing.Font = new drawing.Font();
    let text: string = 'Hello World, This is test code <isLinearMetrics>';
    font.setSize(30);
    font.enableLinearMetrics(true);

    if (font.isLinearMetrics()) {
      let blob: drawing.TextBlob = drawing.TextBlob.makeFromString(text, font, drawing.TextEncoding.TEXT_ENCODING_UTF8);
      canvas.drawTextBlob(blob, 50, 50);
    } else {
      console.error("isLinearMetrics() expected: true, actual: false");
    }

    canvas.detachBrush();
  }
}

export class FontIsEmbolden extends TestBase {

  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let font: drawing.Font = new drawing.Font();
    font.enableEmbolden(true);

    for (let i = 0; i < this.testCount_; i++) {
      font.isEmbolden();
    }
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    let brush = new drawing.Brush();
    canvas.attachBrush(brush);
    let font: drawing.Font = new drawing.Font();
    let text: string = 'Hello World, This is test code <isEmbolden>';
    font.setSize(30);
    font.enableEmbolden(true);

    if (font.isEmbolden()) {
      let blob: drawing.TextBlob = drawing.TextBlob.makeFromString(text, font, drawing.TextEncoding.TEXT_ENCODING_UTF8);
      canvas.drawTextBlob(blob, 50, 50);
    } else {
      console.error("isEmbolden() expected: true, actual: false");
    }

    canvas.detachBrush();
  }
}

export class FontGetSkewX extends TestBase {

  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let font: drawing.Font = new drawing.Font();
    font.setSkewX(-20);

    for (let i = 0; i < this.testCount_; i++) {
      font.getSkewX();
    }
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    let brush = new drawing.Brush();
    canvas.attachBrush(brush);
    let font: drawing.Font = new drawing.Font();
    let text: string = 'Hello World, This is test code <getSkewX>';
    font.setSize(30);
    font.setSkewX(-1);

    if (font.getSkewX() == -1) {
      let blob: drawing.TextBlob = drawing.TextBlob.makeFromString(text, font, drawing.TextEncoding.TEXT_ENCODING_UTF8);
      canvas.drawTextBlob(blob, 50, 50);
    } else {
      console.error("getSkewX() expected -1, actual " + font.getSkewX());
    }

    canvas.detachBrush();
  }
}

export class FontGetScaleX extends TestBase {

  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let font: drawing.Font = new drawing.Font();
    font.setScaleX(2);

    for (let i = 0; i < this.testCount_; i++) {
      font.getScaleX();
    }
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    let brush = new drawing.Brush();
    canvas.attachBrush(brush);
    let font: drawing.Font = new drawing.Font();
    let text: string = 'Hello World, This is test code <getSkewX>';
    font.setSize(30);
    font.setScaleX(2);

    if (font.getScaleX() == 2) {
      let blob: drawing.TextBlob = drawing.TextBlob.makeFromString(text, font, drawing.TextEncoding.TEXT_ENCODING_UTF8);
      canvas.drawTextBlob(blob, 50, 50);
    } else {
      console.error("getScaleX() expected 2, actual " + font.getScaleX());
    }

    canvas.detachBrush();
  }
}

export class FontGetHinting extends TestBase {

  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let font: drawing.Font = new drawing.Font();

    for (let i = 0; i < this.testCount_; i++) {
      font.getHinting();
    }
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    let brush = new drawing.Brush();
    canvas.attachBrush(brush);
    let font: drawing.Font = new drawing.Font();
    font.setSize(30);
    let text: string = 'Hello World, This is test code <getHinting = ' + font.getHinting() + '>';
    let blob: drawing.TextBlob = drawing.TextBlob.makeFromString(text, font, drawing.TextEncoding.TEXT_ENCODING_UTF8);
    canvas.drawTextBlob(blob, 50, 50);
    canvas.detachBrush();
  }
}

export class FontGetEdging extends TestBase {

  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let font: drawing.Font = new drawing.Font();

    for (let i = 0; i < this.testCount_; i++) {
      font.getEdging();
    }
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    let brush = new drawing.Brush();
    canvas.attachBrush(brush);
    let font: drawing.Font = new drawing.Font();
    font.setSize(30);
    let text: string = 'Hello World, This is test code <getEdging = ' + font.getEdging() + '>';
    let blob: drawing.TextBlob = drawing.TextBlob.makeFromString(text, font, drawing.TextEncoding.TEXT_ENCODING_UTF8);
    canvas.drawTextBlob(blob, 50, 50);
    canvas.detachBrush();
  }
}

export class CanvasClipRegion extends TestBase {

  region_: drawing.Region | undefined;

  public constructor(){
    super();
    this.region_ = new drawing.Region();
    this.region_.setRect(0, 0, 500, 500);
  }

  public OnTestFunction(canvas: drawing.Canvas): void {
    let pen = new drawing.Pen();
    let brush = new drawing.Brush();
    pen.setColor({ alpha: 255, red: 255, green: 0, blue: 0 });
    brush.setColor({ alpha: 255, red: 255, green: 0, blue: 0 });
    canvas.attachPen(pen);
    canvas.attachBrush(brush);
    canvas.drawRegion(this.region_);
    canvas.detachPen();
    canvas.detachBrush();

    let region1 = new drawing.Region();
    region1.setRect(0, 0, 250, 250);
    pen.setColor({ alpha: 255, red: 0, green: 255, blue: 0 });
    brush.setColor({ alpha: 255, red: 0, green: 255, blue: 0 });
    canvas.attachPen(pen);
    canvas.attachBrush(brush);
    try {
      canvas.clipRegion(region1);
    } catch (e) {
      console.log("clipRegion exception: " + e);
      printResults(canvas, false);
    }
    canvas.drawRegion(this.region_);
    canvas.detachPen();
    canvas.detachBrush();

    let region2 = new drawing.Region();
    region2.setRect(0, 0, 125, 125);
    pen.setColor({ alpha: 255, red: 0, green: 0, blue: 255 });
    brush.setColor({ alpha: 255, red: 0, green: 0, blue: 255 });
    canvas.attachPen(pen);
    canvas.attachBrush(brush);
    try {
      canvas.clipRegion(region2);
    } catch (e) {
      console.log("clipRegion exception: " + e);
      printResults(canvas, false);
    }
    canvas.drawRegion(this.region_);
    canvas.detachPen();
    canvas.detachBrush();
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    for (let i = 0; i < this.testCount_; i++) {
      canvas.clipRegion(this.region_);
    }
    printResults(canvas, true);
  }
}

export class CanvasSetColor extends TestBase {

  public constructor() {
    super();
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    {
      let pen = new drawing.Pen();
      let rect: common2D.Rect = { left: 0, top: 0, right: 100, bottom: 100 };
      pen.setColor(0xff000000);
      pen.setStrokeWidth(10);
      canvas.attachPen(pen);
      canvas.drawRect(rect);
      canvas.detachPen();
    }

    {
      let pen = new drawing.Pen();
      let rect: common2D.Rect = { left: 0, top: 150, right: 100, bottom: 250 };
      let color: common2D.Color = { alpha:255, red: 255, green: 0, blue: 0 };
      pen.setColor(color);
      pen.setStrokeWidth(10);
      canvas.attachPen(pen);
      canvas.drawRect(rect);
      canvas.detachPen();
    }

    {
      let brush = new drawing.Brush();
      let rect: common2D.Rect = { left: 110, top: 10, right: 200, bottom: 100 };
      brush.setColor(0xffffaa00);
      canvas.attachBrush(brush);
      canvas.drawRect(rect);
      canvas.detachBrush();
    }

    {
      let brush = new drawing.Brush();
      let rect: common2D.Rect = { left: 110, top: 160, right: 200, bottom: 250 };
      let color: common2D.Color = { alpha:255, red: 255, green: 170, blue: 0 };
      brush.setColor(color);
      canvas.attachBrush(brush);
      canvas.drawRect(rect);
      canvas.detachBrush();
    }

    {
      let rect: common2D.Rect = { left: 210, top: 10, right: 300, bottom: 100};
      canvas.save();
      canvas.clipRect(rect);
      canvas.drawColor(0xffff0000);
      canvas.restore();
    }

    {
      let rect: common2D.Rect = { left: 210, top: 10, right: 300, bottom: 250};
      canvas.save();
      canvas.clipRect(rect);
      let color: common2D.Color = { alpha:255, red: 255, green: 0, blue: 0 };
      canvas.drawColor(color);
      canvas.restore();
    }

    {
      let font = new drawing.Font();
      font.setSize(50);
      let textBlob = drawing.TextBlob.makeFromString("hello", font, drawing.TextEncoding.TEXT_ENCODING_UTF8);
      let pen = new drawing.Pen();
      let color: common2D.Color = { alpha:255, red: 0, green: 255, blue: 0 };
      let shadowLayer = drawing.ShadowLayer.create(10, -3, 3, color);
      pen.setShadowLayer(shadowLayer);
      canvas.attachPen(pen);
      canvas.drawTextBlob(textBlob, 10, 400);
      canvas.detachPen();
    }

    {
      let font = new drawing.Font();
      font.setSize(50);
      let textBlob = drawing.TextBlob.makeFromString("hello", font, drawing.TextEncoding.TEXT_ENCODING_UTF8);
      let pen = new drawing.Pen();
      let shadowLayer = drawing.ShadowLayer.create(10, -3, 3, 0xff00ff00);
      pen.setShadowLayer(shadowLayer);
      canvas.attachPen(pen);
      canvas.drawTextBlob(textBlob, 10, 500);
      canvas.detachPen();
    }

    {
      let pen = new drawing.Pen();
      let color: common2D.Color = { alpha:255, red: 0, green: 255, blue: 0 };
      let colorfilter = drawing.ColorFilter.createBlendModeColorFilter(color, drawing.BlendMode.SRC);
      pen.setColorFilter(colorfilter);
      pen.setStrokeWidth(10);
      canvas.attachPen(pen);
      canvas.drawRect({ left: 350, right: 400, top: 350, bottom: 400});
      canvas.detachPen();
    }

    {
      let pen = new drawing.Pen();
      let colorfilter = drawing.ColorFilter.createBlendModeColorFilter(0xff00ff00, drawing.BlendMode.SRC);
      pen.setColorFilter(colorfilter);
      pen.setStrokeWidth(10);
      canvas.attachPen(pen);
      canvas.drawRect({ left: 350, right: 400, top: 450, bottom: 500});
      canvas.detachPen();
    }
  }

}

export class CanvasQuickRejectPath extends TestBase {
  public constructor() {
    super();
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    for (let i = 0; i < this.testCount_; i++) {
      let path: drawing.Path = new drawing.Path();
      path.moveTo(100 + i * 0.01, 100 + i * 0.01);
      path.lineTo(150 + i * 0.01, 50 + i * 0.01);
      canvas.quickRejectPath(path);
    }
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    {
      let path = new drawing.Path();
      path.moveTo(5, 5)
      path.lineTo(10, 10)

      if (canvas.quickRejectPath(path)) {
        console.log(TAG, 'CanvasQuickRejectPath true');
      } else {
        console.log(TAG, 'CanvasQuickRejectPath false');
      }
    }

    {
      let path = new drawing.Path();
      path.moveTo(-5, -5)
      path.lineTo(0, 10)

      if (canvas.quickRejectPath(path)) {
        console.log(TAG, 'CanvasQuickRejectPath true');
      } else {
        console.log(TAG, 'CanvasQuickRejectPath false');
      }
    }

    {
      let path = new drawing.Path();
      path.moveTo(-5, -5)
      path.lineTo(-5, 10)

      if (canvas.quickRejectPath(path)) {
        console.log(TAG, 'CanvasQuickRejectPath true');
      } else {
        console.log(TAG, 'CanvasQuickRejectPath false');
      }
    }

    {
      let path = new drawing.Path();
      path.moveTo(10, -5)
      path.lineTo(-5, 10)

      if (canvas.quickRejectPath(path)) {
        console.log(TAG, 'CanvasQuickRejectPath true');
      } else {
        console.log(TAG, 'CanvasQuickRejectPath false');
      }
    }
  }
}

export class CanvasQuickRejectRect extends TestBase {
  public constructor() {
    super();
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    for (let i = 0; i < this.testCount_; i++) {
      let rect: common2D.Rect = { left: 100 + i * 0.01, top: 100 + i * 0.01, right: 300 + i * 0.01, bottom: 300 + i * 0.01 };
      canvas.quickRejectRect(rect);
    }
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    {
      let rect: common2D.Rect = { left: 10, top: 10, right: 150, bottom: 60 };
      if (canvas.quickRejectRect(rect)) {
        console.log(TAG, 'CanvasQuickRejectRect true');
      } else {
        console.log(TAG, 'CanvasQuickRejectRect false');
      }
    }

    {
      let rect: common2D.Rect = { left: -20, top: 30, right: 0, bottom: 10 };
      if (canvas.quickRejectRect(rect)) {
        console.log(TAG, 'CanvasQuickRejectRect true');
      } else {
        console.log(TAG, 'CanvasQuickRejectRect false');
      }
    }

    {
      let rect: common2D.Rect = { left: -20, top: -20, right: -10, bottom: -10 };
      if (canvas.quickRejectRect(rect)) {
        console.log(TAG, 'CanvasQuickRejectRect true');
      } else {
        console.log(TAG, 'CanvasQuickRejectRect false');
      }
    }

    {
      let rect: common2D.Rect = { left: 999000, top: 999000, right: 999999, bottom: 999999 };
      if (canvas.quickRejectRect(rect)) {
        console.log(TAG, 'CanvasQuickRejectRect true');
      } else {
        console.log(TAG, 'CanvasQuickRejectRect false');
      }
    }

    {
      let rect: common2D.Rect = { left: -1000, top: -1000, right: 999999, bottom: 999999 };
      if (canvas.quickRejectRect(rect)) {
        console.log(TAG, 'CanvasQuickRejectRect true');
      } else {
        console.log(TAG, 'CanvasQuickRejectRect false');
      }
    }
  }
}

export class CanvasDrawArcWithCenter extends TestBase {
  public constructor() {
    super();
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    for (let i = 0; i < this.testCount_; i++) {
      let rect: common2D.Rect = { left: 100 + i * 0.01, top: 100 + i * 0.01, right: 300 + i * 0.01, bottom: 300 + i * 0.01 };
      canvas.drawArcWithCenter(rect, 0, 180, true);
    }
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    let pen = new drawing.Pen();
    pen.setAntiAlias(true);
    pen.setColor({ alpha: 255, red: 0, green: 0, blue: 255});
    pen.setStrokeWidth(5.0);
    canvas.attachPen(pen);
    {
      let rect: common2D.Rect = {left: 20, top: 20, right: 120, bottom: 120};
      canvas.drawArcWithCenter(rect, 80, 280, true);
    }

    {
      let rect: common2D.Rect = {left: 20, top: 140, right: 120, bottom: 240};
      canvas.drawArcWithCenter(rect, -90, 180, false);
    }

    {
      let rect: common2D.Rect = {left: 20, top: 260, right: 220, bottom: 360};
      canvas.drawArcWithCenter(rect, 40, -555, true);
    }

    {
      let rect: common2D.Rect = {left: 20, top: 380, right: 220, bottom: 480};
      canvas.drawArcWithCenter(rect, 40, 666, false);
    }
  }
}

export class CanvasDrawImageNine extends TestBase {
  public constructor() {
    super();
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let pixelMap: image.PixelMap = globalThis.getInstance().getPixelMap("test_3.jpg") //462,712
    let center: common2D.Rect = { left: 30, top: 100, right: 432, bottom: 200 };
    let dst: common2D.Rect = { left: 0, top: 0, right: 500, bottom: 500 };
    for (let i = 0; i < this.testCount_; i++) {
      canvas.drawImageNine(pixelMap, center, dst, drawing.FilterMode.FILTER_MODE_NEAREST);
    }
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    let pixelMap: image.PixelMap = globalThis.getInstance().getPixelMap("test_3.jpg") //462,712
    canvas.drawImage(pixelMap, 0, 0);
    {
      let center: common2D.Rect = { left: 100, top: 100, right: 402, bottom: 500 };
      let dst: common2D.Rect = { left: 480, top: 10, right: 1000, bottom: 1500 };
      canvas.drawImageNine(pixelMap, center, dst, drawing.FilterMode.FILTER_MODE_NEAREST);
    }

    {
      let center: common2D.Rect = { left: 100, top: 100, right: 402, bottom: 500 };
      let dst: common2D.Rect = { left: 30, top: 800, right: 190, bottom: 1312 };
      canvas.drawImageNine(pixelMap, center, dst, drawing.FilterMode.FILTER_MODE_NEAREST);
    }

    {
      let center: common2D.Rect = { left: 100, top: 100, right: 402, bottom: 500 };
      let dst: common2D.Rect = { left: 230, top: 800, right: 350, bottom: 1200 };
      canvas.drawImageNine(pixelMap, center, dst, drawing.FilterMode.FILTER_MODE_NEAREST);
    }
  }
}

export class CanvasDrawImageLattice extends TestBase {
  public constructor() {
    super();
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    let pixelMap: image.PixelMap = globalThis.getInstance().getPixelMap("test_3.jpg") //462,712
    let xDivs: Array<number> = [30, 432];
    let yDivs: Array<number> = [100, 200];
    let lattice = drawing.Lattice.createImageLattice(xDivs, yDivs, 2, 2);
    let dst: common2D.Rect = { left: 0, top: 0, right: 500, bottom: 500 };
    for (let i = 0; i < this.testCount_; i++) {
      canvas.drawImageLattice(pixelMap, lattice, dst, drawing.FilterMode.FILTER_MODE_NEAREST);
    }
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    let pixelMap: image.PixelMap = globalThis.getInstance().getPixelMap("test_3.jpg"); //462,712
    canvas.drawImage(pixelMap, 0, 0);

    {
      let xDivs: Array<number> = [30, 432];
      let yDivs: Array<number> = [30, 300, 450, 682];
      let lattice = drawing.Lattice.createImageLattice(xDivs, yDivs, 2, 4);
      let dst: common2D.Rect = { left: 640, top: 10, right: 1300, bottom: 1500 };
      canvas.drawImageLattice(pixelMap, lattice, dst, drawing.FilterMode.FILTER_MODE_NEAREST);
    }

    {
      let xDivs: Array<number> = [30, 100, 432];
      let yDivs: Array<number> = [30, 200, 450, 500, 682];
      let lattice = drawing.Lattice.createImageLattice(xDivs, yDivs, 3, 5);
      let dst: common2D.Rect = { left: 30, top: 800, right: 362, bottom: 1262 };
      canvas.drawImageLattice(pixelMap, lattice, dst, drawing.FilterMode.FILTER_MODE_NEAREST);
    }

    {
      let xDivs: Array<number> = [30, 100, 432];
      let yDivs: Array<number> = [30, 200, 450, 500, 682];
      let lattice = drawing.Lattice.createImageLattice(xDivs, yDivs, 3, 5);
      let dst: common2D.Rect = { left: 400, top: 800, right: 600, bottom: 1000 };
      canvas.drawImageLattice(pixelMap, lattice, dst, drawing.FilterMode.FILTER_MODE_NEAREST);
    }
  }
}
