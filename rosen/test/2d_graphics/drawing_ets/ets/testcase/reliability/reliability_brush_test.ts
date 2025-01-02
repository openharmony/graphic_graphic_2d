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
import { common2D, drawing } from '@kit.ArkGraphics2D';
import { TestBase } from '../../pages/testbase';
import { drawAll } from './reliability_common';

// detach brush后，再调用draw接口
export class ReliabilityDetachBrush extends TestBase {

  /*
  @CaseID:SUB_BASIC_GRAPHICS_SPECIAL_STABLE_TS_DRAWING_RELIABILITY_BRUSH_0100
  @Description:detach brush后，再调用draw接口
  @Step:
  1、运行脚本，循环执行hidumper命令查询内存
  2、执行自动化用例：
  detach brush后，再调用draw接口
  3、执行结束后等待3分钟
  4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
  @Result:
  1、程序运行正常，无crash
  2、内存平稳，没有持续增长，执行后内存回落到执行前；
  */
  public async OnTestFunction(canvas: drawing.Canvas) {
    let brush = new drawing.Brush();
    canvas.attachBrush(brush);
    canvas.detachBrush();
    drawAll(canvas);
  }
}

// attach brush后调用destroy接口，再调用draw接口
export class ReliabilityAttachBrush extends TestBase {

  /*
  @CaseID:SUB_BASIC_GRAPHICS_SPECIAL_STABLE_TS_DRAWING_RELIABILITY_BRUSH_0200
  @Description:attach brush后调用destroy接口，再调用draw接口
  @Step:
  1、运行脚本，循环执行hidumper命令查询内存
  2、执行自动化用例：
  attach brush后调用destroy接口，再调用draw接口
  3、执行结束后等待3分钟
  4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
  @Result:
  1、程序运行正常，无crash
  2、内存平稳，没有持续增长，执行后内存回落到执行前；
  */
  public async OnTestFunction(canvas: drawing.Canvas) {
    {
      let brush = new drawing.Brush();
      canvas.attachBrush(brush);
      canvas.detachBrush();
    }
    drawAll(canvas);
  }
}

// brush setColorFilter后销毁ColorFilter，再调用draw接口
export class ReliabilityBrushSetColorFilter extends TestBase {

  /*
  @CaseID:SUB_BASIC_GRAPHICS_SPECIAL_STABLE_TS_DRAWING_RELIABILITY_BRUSH_0300
  @Description:attach brush setColorFilter后销毁ColorFilter，再调用draw接口
  @Step:
  1、运行脚本，循环执行hidumper命令查询内存
  2、执行自动化用例：
  brush setColorFilter后销毁ColorFilter，再调用draw接口
  3、执行结束后等待3分钟
  4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
  @Result:
  1、程序运行正常，无crash
  2、内存平稳，没有持续增长，执行后内存回落到执行前；
  */
  public async OnTestFunction(canvas: drawing.Canvas) {
    let brush = new drawing.Brush();
    canvas.attachBrush(brush);
    // setColorFilter后销毁ColorFilter(超出作用域ColorFilter就被销毁了)
    {
      let colorFilter = drawing.ColorFilter.createSRGBGammaToLinear();
      brush.setColorFilter(colorFilter);
    }
    drawAll(canvas);
    canvas.detachBrush();
  }
}

// brush setMaskFilter后销毁MaskFilter，再调用draw接口
export class ReliabilityBrushSetMaskFilter extends TestBase {

  /*
  @CaseID:SUB_BASIC_GRAPHICS_SPECIAL_STABLE_TS_DRAWING_RELIABILITY_BRUSH_0400
  @Description:brush setMaskFilter后销毁MaskFilter，再调用draw接口
  @Step:
  1、运行脚本，循环执行hidumper命令查询内存
  2、执行自动化用例：
  brush setMaskFilter后销毁MaskFilter，再调用draw接口
  3、执行结束后等待3分钟
  4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
  @Result:
  1、程序运行正常，无crash
  2、内存平稳，没有持续增长，执行后内存回落到执行前；
  */
  public async OnTestFunction(canvas: drawing.Canvas) {
    let brush = new drawing.Brush();
    {
      let sima = 10;
      let blurType: drawing.BlurType = 0;
      let maskFilter = drawing.MaskFilter.createBlurMaskFilter(blurType, sima);
      brush.setMaskFilter(maskFilter);
    }
    drawAll(canvas);
    canvas.detachBrush();
  }
}

// brush setShadowLayer后销毁ShadowLayer，再调用draw接口
export class ReliabilityBrushSetShadowLayer extends TestBase {

  /*
  @CaseID:SUB_BASIC_GRAPHICS_SPECIAL_STABLE_TS_DRAWING_RELIABILITY_BRUSH_0500
  @Description:brush setShadowLayer后销毁ShadowLayer，再调用draw接口
  @Step:
  1、运行脚本，循环执行hidumper命令查询内存
  2、执行自动化用例：
  brush setShadowLayer后销毁ShadowLayer，再调用draw接口
  3、执行结束后等待3分钟
  4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
  @Result:
  1、程序运行正常，无crash
  2、内存平稳，没有持续增长，执行后内存回落到执行前；
  */
  public async OnTestFunction(canvas: drawing.Canvas) {
    let brush = new drawing.Brush();
    canvas.attachBrush(brush);
    {
      let shadowLayer = drawing.ShadowLayer;
      let blurRadius: number = 10;
      let x: number = 20;
      let y: number = 20;
      let color: common2D.Color = {
        alpha: 255,
        red: 168,
        green: 48,
        blue: 176,
      };
      shadowLayer.create(blurRadius, x, y, color);
      brush.setShadowLayer(shadowLayer);
    }
    drawAll(canvas);
    canvas.detachBrush();
  }
}

async function subWorker(canvas) {
  console.log('subWorker function start');
  let drawCount = 100;
  // 在这里执行子线程的逻辑
  for (let i = 0; i < drawCount; i++) {
    drawAll(canvas);
    console.log('function subWorker', i);
  }
  console.log('subWorker function end');
}

// draw过程中，detach brush
export class ReliabilityDetachBrushWorker extends TestBase {

  /*
  @CaseID:SUB_BASIC_GRAPHICS_SPECIAL_STABLE_TS_DRAWING_RELIABILITY_BRUSH_0600
  @Description:draw过程中，detach brush
  @Step:
  1、运行脚本，循环执行hidumper命令查询内存
  2、执行自动化用例：
  draw过程中，detach brush
  3、执行结束后等待3分钟
  4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
  @Result:
  1、程序运行正常，无crash
  2、内存平稳，没有持续增长，执行后内存回落到执行前；
  */
  public async OnTestFunction(canvas: drawing.Canvas) {
    console.log('function main start');
    subWorker(canvas);
    let drawCount = 100;
    for (let i = 0; i < drawCount; i++) {
      let brush = new drawing.Brush();
      canvas.attachBrush(brush);
      canvas.detachBrush();
      console.log('function main', i);
    }
    console.log('function main end');
  }
}

// draw过程中，brush调用destroy接口
export class ReliabilityAttachBrushWorker extends TestBase {

  /*
  @CaseID:SUB_BASIC_GRAPHICS_SPECIAL_STABLE_TS_DRAWING_RELIABILITY_BRUSH_0700
  @Description:draw过程中，brush调用destroy接口
  @Step:
  1、运行脚本，循环执行hidumper命令查询内存
  2、执行自动化用例：
  draw过程中，brush调用destroy接口
  3、执行结束后等待3分钟
  4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
  @Result:
  1、程序运行正常，无crash
  2、内存平稳，没有持续增长，执行后内存回落到执行前；
  */
  public async OnTestFunction(canvas: drawing.Canvas) {
    console.log('function main start');
    subWorker(canvas);
    {
      let drawCount = 100;
      for (let i = 0; i < drawCount; i++) {
        let brush = new drawing.Brush();
        canvas.attachBrush(brush);
        canvas.detachBrush();
        console.log('function main', i);
      }
    }
    console.log('function main end');
  }
}

// draw过程中，brush 销毁ColorFilter
export class ReliabilityBrushSetColorFilterWorker extends TestBase {

  /*
  @CaseID:SUB_BASIC_GRAPHICS_SPECIAL_STABLE_TS_DRAWING_RELIABILITY_BRUSH_0800
  @Description:draw过程中，brush 销毁ColorFilter
  @Step:
  1、运行脚本，循环执行hidumper命令查询内存
  2、执行自动化用例：
  draw过程中，brush 销毁ColorFilter
  3、执行结束后等待3分钟
  4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
  @Result:
  1、程序运行正常，无crash
  2、内存平稳，没有持续增长，执行后内存回落到执行前；
  */
  public async OnTestFunction(canvas: drawing.Canvas) {
    console.log('function main start');
    subWorker(canvas);
    let brush = new drawing.Brush();
    canvas.attachBrush(brush);
    {
      let drawCount = 100;
      for (let i = 0; i < drawCount; i++) {
        let colorFilter = drawing.ColorFilter.createSRGBGammaToLinear();
        brush.setColorFilter(colorFilter);
        console.log('function main', i);
      }
    }
    canvas.detachBrush();
    console.log('function main end');
  }
}

// draw过程中，brush 销毁MaskFilter
export class ReliabilityBrushSetMaskFilterWorker extends TestBase {

  /*
  @CaseID:SUB_BASIC_GRAPHICS_SPECIAL_STABLE_TS_DRAWING_RELIABILITY_BRUSH_0900
  @Description:draw过程中，brush 销毁MaskFilter
  @Step:
  1、运行脚本，循环执行hidumper命令查询内存
  2、执行自动化用例：
  draw过程中，brush 销毁MaskFilter
  3、执行结束后等待3分钟
  4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
  @Result:
  1、程序运行正常，无crash
  2、内存平稳，没有持续增长，执行后内存回落到执行前；
  */
  public async OnTestFunction(canvas: drawing.Canvas) {
    console.log('function main start');
    subWorker(canvas);
    let brush = new drawing.Brush();
    canvas.attachBrush(brush);
    {
      let drawCount = 100;
      for (let i = 0; i < drawCount; i++) {
        let sigma = 10;
        let blurType: drawing.BlurType = 0;
        let maskFilter = drawing.MaskFilter.createBlurMaskFilter(blurType, sigma);
        brush.setMaskFilter(maskFilter);
        console.log('function main', i);
      }
    }
    canvas.detachBrush();
    console.log('function main end');
  }
}

// draw过程中，brush 销毁ShadowLayer
export class ReliabilityBrushSetShadowLayerWorker extends TestBase {

  /*
  @CaseID:SUB_BASIC_GRAPHICS_SPECIAL_STABLE_TS_DRAWING_RELIABILITY_BRUSH_1000
  @Description:draw过程中，brush 销毁ShadowLayer
  @Step:
  1、运行脚本，循环执行hidumper命令查询内存
  2、执行自动化用例：
  draw过程中，brush 销毁ShadowLayer
  3、执行结束后等待3分钟
  4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
  @Result:
  1、程序运行正常，无crash
  2、内存平稳，没有持续增长，执行后内存回落到执行前；
  */
  public async OnTestFunction(canvas: drawing.Canvas) {
    console.log('function main start');
    subWorker(canvas);
    let brush = new drawing.Brush();
    canvas.attachBrush(brush);
    {
      let drawCount = 100;
      for (let i = 0; i < drawCount; i++) {
        let shadowLayer = drawing.ShadowLayer;
        let blurRadius: number = 10;
        let x: number = 20;
        let y: number = 20;
        let color: common2D.Color = {
          alpha: 255,
          red: 168,
          green: 48,
          blue: 176,
        };
        shadowLayer.create(blurRadius, x, y, color);
        brush.setShadowLayer(shadowLayer);
        console.log('function main', i);
      }
    }
    canvas.detachBrush();
    console.log('function main end');
  }
}