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
import {TestBase, TestFunctionStyleType, StyleType} from '../../pages/testbase';
import { N, OHRandom } from '../../utils/OHRandom';
import globalThis from '../../utils/globalThis'
import ArrayList from '@ohos.util.ArrayList';
import image from '@ohos.multimedia.image';

const TAG = '[BezierBench]';

export class CanvasDrawRect extends TestBase {

  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE){
    super();
    this.styleType_ = styleType;
  }
  public OnTestPerformance(canvas: drawing.Canvas) {
    console.log(TAG, 'xyj DRAW_ STYLE_COMPLEX end');
    for (let i = 0; i < this.testCount_; i++) {
      let l = i % this.width_;
      let t = (i + 100) % this.height_;
      let r = ((l + 100) > this.width_) ? this.width_ : (l + 100);
      let b = ((t + 100) > this.height_) ? this.height_ : (t + 100);
      //针对rect，每次的绘制起点位置，绘制的宽高大小需要不一致
      let rect: common2D.Rect = {left: l, top: t, right: r, bottom: b};
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
}

export class CanvasDrawLine extends TestBase {

  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE){
    super();
    this.styleType_ = styleType;
  }
  public OnTestPerformance(canvas: drawing.Canvas) {
    let rand: OHRandom = new OHRandom();
    for (let i = 0; i < this.testCount_; i++) {
      let x1: number = rand.nextULessThan(this.width_);
      let y1: number = rand.nextULessThan(this.height_);
      let x2: number = rand.nextULessThan(this.width_);
      let y2: number = rand.nextULessThan(this.height_);
      canvas.drawLine(x1, y1, x2, y2);
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

export class CanvasDrawPath extends TestBase {

  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE){
    super();
    this.styleType_ = styleType;
  }
  public OnTestPerformance(canvas: drawing.Canvas) {
    let path: drawing.Path = new drawing.Path();
    path.moveTo(0, 0);
    let rand: OHRandom = new OHRandom();
    path.lineTo(rand.nextRangeF(0, 720), rand.nextRangeF(0, 720));
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

export class CanvasDrawPoint extends TestBase {

  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE){
    super();
    this.styleType_ = styleType;
  }
  public OnTestPerformance(canvas: drawing.Canvas) {
    let rand: OHRandom = new OHRandom();
    for (let i = 0; i < this.testCount_; i++) {
      let x: number = rand.nextUScalar1() * 640;
      let y: number = rand.nextUScalar1() * 480;
      canvas.drawPoint(x, y);
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
  public async OnTestPerformance(canvas: drawing.Canvas) {
    let rand: OHRandom = new OHRandom();
    const colors: ArrayBuffer = new ArrayBuffer(200 * 200 * 4);
    let opts: image.InitializationOptions = {
      alphaType: image.AlphaType.OPAQUE,
      pixelFormat: image.PixelMapFormat.BGRA_8888,
      size: { width: 200, height: 200 }
    }

    let pixelMap: image.PixelMap = await image.createPixelMap(colors, opts);
    let canvasBitmap: drawing.Canvas = new drawing.Canvas(pixelMap);
    const backgroundColor: common2D.Color = { alpha: 0xff, red: 0, green: 0xff, blue: 0 };
    canvasBitmap.drawColor(backgroundColor);
    for (let i = 0; i < this.testCount_; i++) {
      let x: number = rand.nextULessThan(this.width_);
      let y: number = rand.nextULessThan(this.height_);
      canvas.drawImage(pixelMap, x, y);
    }
  }
  
  public OnTestFunction(canvas: drawing.Canvas) {

    //接口调用，功能测试.cpu/gpu调用接口一致
    {
      let pen = new drawing.Pen();
      this.ApplyPenStyle(pen, TestFunctionStyleType.DRAW_STYLE_TYPE_3)
      canvas.attachPen(pen)
      let pixelMap: image.PixelMap = globalThis.getInstance().getPixelMap("test.jpg")
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
      let pixelMap: image.PixelMap = globalThis.getInstance().getPixelMap("test.jpg")
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
      let pixelMap: image.PixelMap = globalThis.getInstance().getPixelMap("test.jpg")
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
      let pixelMap: image.PixelMap = globalThis.getInstance().getPixelMap("test.jpg")
      canvas.drawImage(pixelMap, 140, 17.8);
      canvas.detachPen()
      canvas.detachBrush()
    }
  }
}

export class CanvasDrawCircle extends TestBase {

  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE){
    super();
    this.styleType_ = styleType;
  }
  public OnTestPerformance(canvas: drawing.Canvas) {
    let rand: OHRandom = new OHRandom();
    for (let i = 0; i < this.testCount_; i++) {
      let x: number = rand.nextULessThan(this.width_);
      let y: number = rand.nextULessThan(this.height_);
      let r: number = rand.nextULessThan(this.height_);
      canvas.drawCircle(x, y, r);
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

export class CanvasDrawTextBlob extends TestBase {

  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE){
    super();
    this.styleType_ = styleType;
  }
  public OnTestPerformance(canvas: drawing.Canvas) {
    let rand: OHRandom = new OHRandom();
    let text: string = 'textblob';
    const font: drawing.Font = new drawing.Font();
    font.setSize(20);
    const textBlob = drawing.TextBlob.makeFromString(text, font, drawing.TextEncoding.TEXT_ENCODING_UTF8);
    for (let i = 0; i < this.testCount_; i++) {
      let x: number = rand.nextRangeF(0, this.width_);
      let y: number = rand.nextRangeF(0, this.height_);
      canvas.drawTextBlob(textBlob, x, y);
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
  public async OnTestPerformance(canvas: drawing.Canvas) {
    const colorBuffer: ArrayBuffer = new ArrayBuffer(500 * 500 * 4);
    let opts: image.InitializationOptions = {
      alphaType: image.AlphaType.OPAQUE,
      editable: true,
      pixelFormat: image.PixelMapFormat.BGRA_8888,
      size: { width: 500, height: 500 }
    }
    let pixelMap: image.PixelMap = await image.createPixelMap(colorBuffer, opts);
    const brush = new drawing.Brush();
    canvas.attachBrush(brush);
    for (let i = 0; i < this.testCount_; i++) {
      let verts: Array<number> = [100, 100, 200, 100, 150, 200, 200, 200];
      // let colors: Array<number> = [0x00ff0088, 0x00ff0088, 0x00ff0088, 0x00ff0088];
      // canvas.drawPixelMapMesh(pixelMap, 1, 1, verts, 0, null, 0);
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