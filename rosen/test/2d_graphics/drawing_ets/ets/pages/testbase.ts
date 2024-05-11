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

import common2D from "@ohos.graphics.common2D";
import drawing from "@ohos.graphics.drawing";
import image from '@ohos.multimedia.image';
import systemDateTime from '@ohos.systemDateTime';
import fs from '@ohos.file.fs'
import { BusinessError } from '@ohos.base';
import { Global } from './global';

const TAG = '[DrawingTest]';
export class TestBase {
  public constructor() {
    this.width_ = 720;
    this.height_ = 720;
    this.backgroundA_ = 0xFF;
    this.backgroundR_ = 0xFF;
    this.backgroundG_ = 0xFF;
    this.backgroundB_ = 0xFF;
    this.fileName_ = "undefined";
    this.testCount_ = 1;
    this.styleType_ = StyleType.DRAW_STYLE_NONE;
  }

  width_: number | 720;  //gpu离屏绘制默认创建的画布宽
  height_: number | 720; //gpu离屏绘制默认创建的画布高
  backgroundA_: number | 0xFF; //gpu离屏绘制默认创建的画布默认背景色，默认白色
  backgroundR_: number | 0xFF;
  backgroundG_: number | 0xFF;
  backgroundB_: number | 0xFF;
  fileName_: string | "default"; //gpu离屏绘制后保存的图片名字
  pixelMap_: image.PixelMap | undefined; //gpu离屏创建的pixmap
  canvas_: drawing.Canvas | undefined; //gpu离屏绘制canvas
  testCount_: number | 1; //性能功耗测试时候，关键接口的默认循环次数
  time_: number | 0; //性能测试耗时
  styleType_: number | 0; //性能测试耗时

  public async CreateBitmapCanvas() {
    console.log(TAG, 'CreateBitmapCanvas'+ this.width_ + ' * ' + this.height_);
    const color : ArrayBuffer = new ArrayBuffer(this.height_ * this.width_ * 4);  //需要创建的像素buffer大小，取值为：height * width *4
    let opts : image.InitializationOptions = { editable: true, pixelFormat: 3, size: { height: this.height_, width: this.width_ } }
    this.pixelMap_ = await image.createPixelMap(color, opts);
    if (this.pixelMap_ == null || this.pixelMap_ == undefined) {
      console.log(TAG, 'create pixelMap error');
      return;
    }

    //创建离屏canvas，与pixmap绑定
    this.canvas_ = new drawing.Canvas(this.pixelMap_);
    if (this.canvas_ == null || this.canvas_ == undefined) {
      console.log(TAG, 'create canvas_ error');
      return;
    } else {
      console.log(TAG, 'create canvas_ success');
    }

    //离屏canvas绘制背景颜色
    const backgroundColor: common2D.Color = { alpha: this.backgroundA_, red: this.backgroundR_, green: this.backgroundG_, blue: this.backgroundB_ };
    this.canvas_.drawColor(backgroundColor);
  }

  public BitmapCanvasToFile(dir: string) {
    if (this.pixelMap_ == null || this.pixelMap_ == undefined) {
      console.log(TAG, 'this.pixelMap_ is invalid');
      return;
    }
    Global.savePixmap(dir + "/cpu/", this.fileName_, this.pixelMap_);
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    //接口调用，功能测试
  }

  public OnTestPerformance(canvas: drawing.Canvas) {
    //接口重复调用，性能功耗测试
  }

  public OnTestFunctionCpu(canvas: drawing.Canvas) {
    //离屏canvas绘制图案
    this.OnTestFunction(canvas)
  }

  public OnTestPerformanceCpu(canvas: drawing.Canvas) {
    //离屏canvas绘制接口重复调用
    this.OnTestPerformance(canvas)
  }

  public OnTestFunctionGpuUpScreen(canvas: drawing.Canvas) {
    //gpu上屏幕绘制图案
    this.OnTestFunction(canvas)
  }

  public OnTestPerformanceGpuUpScreen(canvas: drawing.Canvas) {
    //gpu上屏幕绘制接口重复调用
    this.OnTestPerformance(canvas)
  }

  public async TestFunctionCpu(dir: string) {
    await this.CreateBitmapCanvas();
    if (this.canvas_ == null || this.canvas_ == undefined) {
      console.error(TAG, 'canvas_ is invalid');
      return;
    }
    this.OnTestFunctionCpu(this.canvas_);
    this.BitmapCanvasToFile(dir);
  }

  public SetTestCount(count: number) {
    this.testCount_ = count;
  }

  public async TestPerformanceCpu() {
    await this.CreateBitmapCanvas();
    if (this.canvas_ == null || this.canvas_ == undefined) {
      console.log(TAG, 'canvas_ is invalid');
      return;
    }
    this.StyleSettings(this.canvas_, this.styleType_);
    let startTime = systemDateTime.getUptime(systemDateTime.TimeType.STARTUP, false);
    console.log(TAG, 'DrawingApiTest Started: [' + startTime + ']');

    await this.OnTestPerformanceCpu(this.canvas_);

    let endTime = systemDateTime.getUptime(systemDateTime.TimeType.STARTUP, false);
    console.log(TAG, 'DrawingApiTest Finished: [' + endTime + ']');
    this.time_ = endTime - startTime;
    // log error is to avoid log loss
    console.error(TAG, 'DrawingApiTest TotalApiCallTime: [' + this.time_ + ']');
    console.error(TAG, 'DrawingApiTest TotalApiCallCount: [' + this.testCount_ + ']');
    this.StyleSettingsDestroy(this.canvas_);
  }

  public TestFunctionGpuUpScreen(canvas: drawing.Canvas) {
    if (canvas == null || canvas == undefined) {
      console.error(TAG, 'canvas is invalid');
      return;
    }
    this.OnTestFunctionGpuUpScreen(canvas);
  }

  public TestPerformanceGpuUpScreen(canvas: drawing.Canvas): number {
    if (canvas == null || canvas == undefined) {
      console.error(TAG, 'canvas is invalid');
      return;
    }

    this.StyleSettings(canvas, this.styleType_);
    let startTime = systemDateTime.getUptime(systemDateTime.TimeType.STARTUP, false);
    console.log(TAG, 'DrawingApiTest Started: [' + startTime + ']');

    this.OnTestPerformanceGpuUpScreen(canvas);

    let endTime = systemDateTime.getUptime(systemDateTime.TimeType.STARTUP, false);
    console.log(TAG, 'DrawingApiTest Finished: [' + endTime + ']');
    this.time_ = endTime - startTime;
    // log error is to avoid log loss
    console.error(TAG, 'DrawingApiTest TotalApiCallTime: [' + this.time_ + ']');
    console.error(TAG, 'DrawingApiTest TotalApiCallCount: [' + this.testCount_ + ']');
    console.log(TAG, 'TestPerformanceGpuUpScreen end');
    this.StyleSettingsDestroy(canvas);
    return this.time_;
  }
  public StyleSettings(canvas: drawing.Canvas, styleType: StyleType) {
    if (styleType == StyleType.DRAW_STYLE_COMPLEX) {
      let color: common2D.Color = { alpha: 255, red: 255, green: 0, blue: 0 };
      let filter = drawing.ColorFilter.createLinearToSRGBGamma();

      let brush = new drawing.Brush();
      brush.setColor(color);
      brush.setAntiAlias(true);
      brush.setAlpha(0xF0);
      brush.setColorFilter(filter);
      brush.setBlendMode(1);
      canvas.attachBrush(brush);

      let pen = new drawing.Pen();
      pen.setColor(color);
      pen.setStrokeWidth(5);
      pen.setAntiAlias(true);
      pen.setAlpha(0xF0);
      pen.setColorFilter(filter);
      pen.setBlendMode(1);
      pen.setDither(true);
      canvas.attachPen(pen);
    }
  }
  public StyleSettingsDestroy(canvas: drawing.Canvas) {
    if (canvas) {
      canvas.detachPen();
      canvas.detachBrush();
    }
  }

  public ApplyBrushStyle(brush: drawing.Brush, styleType: TestFunctionStyleType) {
    if (styleType == TestFunctionStyleType.DRAW_STYLE_TYPE_1) {      
      let color: common2D.Color = { alpha: 255, red: 255, green: 0, blue: 0 };
      brush.setColor(color)
      brush.setAntiAlias(true)
      brush.setBlendMode(drawing.BlendMode.SRC);
      let blendModeColor: common2D.Color = { alpha: 165, red: 198, green: 156, blue: 123 };
      let filter = drawing.ColorFilter.createBlendModeColorFilter(blendModeColor, drawing.BlendMode.SRC);
      brush.setColorFilter(filter);
    } else if (styleType == TestFunctionStyleType.DRAW_STYLE_TYPE_2) {
      let color: common2D.Color = { alpha: 200, red: 200, green: 125, blue: 98 };
      brush.setColor(color)
      brush.setAntiAlias(true)
      brush.setBlendMode(drawing.BlendMode.SRC_OVER);
      brush.setAlpha(160)
      let filter_out = drawing.ColorFilter.createLinearToSRGBGamma();
      let filter_iner = drawing.ColorFilter.createSRGBGammaToLinear();
      let filter = drawing.ColorFilter.createComposeColorFilter(filter_out, filter_iner);
      brush.setColorFilter(filter);
    } else if (styleType == TestFunctionStyleType.DRAW_STYLE_TYPE_3) {
      let color: common2D.Color = { alpha: 255, red: 120, green: 216, blue: 156 };
      brush.setColor(color)
      brush.setAntiAlias(false)
      brush.setBlendMode(drawing.BlendMode.SRC_OVER); 
      let filter = drawing.ColorFilter.createLumaColorFilter();
      brush.setColorFilter(filter);
    } else if  (styleType == TestFunctionStyleType.DRAW_STYLE_TYPE_4) {
      let color: common2D.Color = { alpha: 255, red: 213, green: 120, blue: 25 };
      brush.setColor(color)
      brush.setAntiAlias(false)
      brush.setBlendMode(drawing.BlendMode.MULTIPLY)
    }
  }

  public ApplyPenStyle(pen: drawing.Pen, styleType: TestFunctionStyleType) {
    if (styleType == TestFunctionStyleType.DRAW_STYLE_TYPE_1) {
      let color: common2D.Color = { alpha: 255, red: 0, green: 255, blue: 0 };
      pen.setColor(color)
      pen.setStrokeWidth(20)
      pen.setAntiAlias(true)
      pen.setDither(true)
      pen.setBlendMode(drawing.BlendMode.SRC);
      let blendModeColor: common2D.Color = { alpha: 165, red: 216, green: 231, blue: 56 };
      let filter = drawing.ColorFilter.createBlendModeColorFilter(blendModeColor, drawing.BlendMode.SRC);
      pen.setColorFilter(filter);
    } else if (styleType == TestFunctionStyleType.DRAW_STYLE_TYPE_2) {
      let color: common2D.Color = { alpha: 233, red: 159, green: 254, blue: 52 };
      pen.setColor(color)
      pen.setAntiAlias(true)
      pen.setAlpha(120)
      pen.setStrokeWidth(100)
      pen.setBlendMode(drawing.BlendMode.SRC_OVER); 
      let filter_out = drawing.ColorFilter.createLinearToSRGBGamma();
      let filter_iner = drawing.ColorFilter.createSRGBGammaToLinear();
      let filter = drawing.ColorFilter.createComposeColorFilter(filter_out, filter_iner);
      pen.setColorFilter(filter);
    } else if (styleType == TestFunctionStyleType.DRAW_STYLE_TYPE_3) {
      let color: common2D.Color = { alpha: 255, red: 216, green: 34, blue: 79 };
      pen.setColor(color)
      pen.setAntiAlias(false)
      pen.setBlendMode(drawing.BlendMode.SRC_OVER)
      let filter = drawing.ColorFilter.createLumaColorFilter()
      pen.setColorFilter(filter)
    } else if  (styleType == TestFunctionStyleType.DRAW_STYLE_TYPE_4) {
      let color: common2D.Color = { alpha: 255, red: 56, green: 234, blue: 156 };
      pen.setColor(color)
      pen.setAntiAlias(false)
      pen.setStrokeWidth(10)
      pen.setBlendMode(drawing.BlendMode.MULTIPLY)
    }
  }
}

export enum TestFunctionStyleType {
  DRAW_STYLE_TYPE_1 = 0,
  DRAW_STYLE_TYPE_2,
  DRAW_STYLE_TYPE_3,
  DRAW_STYLE_TYPE_4
}

export enum StyleType { //公共的pen，brush，filter等配置
  DRAW_STYLE_NONE = 0,
  DRAW_STYLE_COMPLEX, //最复杂的配置，会将所有配置加上，得出近似最恶劣的性能数据
  DRAW_STYLE_FUNCTION_TEST_1, //
}