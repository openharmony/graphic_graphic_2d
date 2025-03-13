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

// detach pen后，再调用draw接口
export class ReliabilityDetachPen extends TestBase {

  /*
  @CaseID:SUB_BASIC_GRAPHICS_SPECIAL_STABLE_TS_DRAWING_RELIABILITY_PEN_0100
  @Description:detach pen后，再调用draw接口
  @Step:
  1、运行脚本，循环执行hidumper命令查询内存
  2、执行自动化用例：
  detach pen后，再调用draw接口
  3、执行结束后等待3分钟
  4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
  @Result:
  1、程序运行正常，无crash
  2、内存平稳，没有持续增长，执行后内存回落到执行前；
  */
  public async OnTestFunction(canvas: drawing.Canvas) {
    let pen = new drawing.Pen();
    canvas.attachPen(pen);
    canvas.detachPen();
    drawAll(canvas);
  }
}

// attach pen后调用destroy接口，再调用draw接口
export class ReliabilityAttachPen extends TestBase {

  /*
  @CaseID:SUB_BASIC_GRAPHICS_SPECIAL_STABLE_TS_DRAWING_RELIABILITY_PEN_0200
  @Description:attach pen后调用destroy接口，再调用draw接口
  @Step:
  1、运行脚本，循环执行hidumper命令查询内存
  2、执行自动化用例：
  attach pen后调用destroy接口，再调用draw接口
  3、执行结束后等待3分钟
  4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
  @Result:
  1、程序运行正常，无crash
  2、内存平稳，没有持续增长，执行后内存回落到执行前；
  */
  public async OnTestFunction(canvas: drawing.Canvas) {
    {
      let pen = new drawing.Pen();
      canvas.attachPen(pen);
      canvas.detachPen();
    }
    drawAll(canvas);
  }
}

// pen setColorFilter后销毁ColorFilter，再调用draw接口
export class ReliabilityPenSetColorFilter extends TestBase {

  /*
  @CaseID:SUB_BASIC_GRAPHICS_SPECIAL_STABLE_TS_DRAWING_RELIABILITY_PEN_0300
  @Description:pen setColorFilter后销毁ColorFilter，再调用draw接口
  @Step:
  1、运行脚本，循环执行hidumper命令查询内存
  2、执行自动化用例：
  pen setColorFilter后销毁ColorFilter，再调用draw接口
  3、执行结束后等待3分钟
  4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
  @Result:
  1、程序运行正常，无crash
  2、内存平稳，没有持续增长，执行后内存回落到执行前；
  */
  public async OnTestFunction(canvas: drawing.Canvas) {
    let pen = new drawing.Pen();
    canvas.attachPen(pen);
    // setColorFilter后销毁ColorFilter(超出作用域ColorFilter就被销毁了)
    {
      let colorFilter = drawing.ColorFilter.createSRGBGammaToLinear();
      pen.setColorFilter(colorFilter);
    }
    drawAll(canvas);
    canvas.detachPen();
  }
}

// pen setMaskFilter1后销毁MaskFilter，再调用draw接口
export class ReliabilityPenSetMaskFilter extends TestBase {

  /*
  @CaseID:SUB_BASIC_GRAPHICS_SPECIAL_STABLE_TS_DRAWING_RELIABILITY_PEN_0400
  @Description:pen setMaskFilter1后销毁MaskFilter，再调用draw接口
  @Step:
  1、运行脚本，循环执行hidumper命令查询内存
  2、执行自动化用例：
  pen setMaskFilter1后销毁MaskFilter，再调用draw接口
  3、执行结束后等待3分钟
  4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
  @Result:
  1、程序运行正常，无crash
  2、内存平稳，没有持续增长，执行后内存回落到执行前；
  */
  public async OnTestFunction(canvas: drawing.Canvas) {
    let pen = new drawing.Pen();
    canvas.attachPen(pen);
    {
      let sima = 10;
      let blurType: drawing.BlurType = 0;
      let maskFilter = drawing.MaskFilter.createBlurMaskFilter(blurType, sima);
      pen.setMaskFilter(maskFilter);
    }
    drawAll(canvas);
    canvas.detachPen();
  }
}

// pen SetPathEffect后销毁PathEffect，再调用draw接口
export class ReliabilityPenSetPathEffect extends TestBase {

  /*
  @CaseID:SUB_BASIC_GRAPHICS_SPECIAL_STABLE_TS_DRAWING_RELIABILITY_PEN_0500
  @Description:pen SetPathEffect后销毁PathEffect，再调用draw接口
  @Step:
  1、运行脚本，循环执行hidumper命令查询内存
  2、执行自动化用例：
  pen SetPathEffect后销毁PathEffect，再调用draw接口
  3、执行结束后等待3分钟
  4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
  @Result:
  1、程序运行正常，无crash
  2、内存平稳，没有持续增长，执行后内存回落到执行前；
  */
  public async OnTestFunction(canvas: drawing.Canvas) {
    let pen = new drawing.Pen();
    canvas.attachPen(pen);
    {
      let pathEffect = drawing.ShaderEffect.createColorShader(0x55ff0000);
      pen.setPathEffect(pathEffect);
    }
    drawAll(canvas);
    canvas.detachPen();
  }
}

// pen setShadowLayer后销毁ShadowLayer，再调用draw接口
export class ReliabilityPenSetShadowLayer extends TestBase {

  /*
  @CaseID:SUB_BASIC_GRAPHICS_SPECIAL_STABLE_TS_DRAWING_RELIABILITY_PEN_0600
  @Description:pen setShadowLayer后销毁ShadowLayer，再调用draw接口
  @Step:
  1、运行脚本，循环执行hidumper命令查询内存
  2、执行自动化用例：
  pen setShadowLayer后销毁ShadowLayer，再调用draw接口
  3、执行结束后等待3分钟
  4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
  @Result:
  1、程序运行正常，无crash
  2、内存平稳，没有持续增长，执行后内存回落到执行前；
  */
  public async OnTestFunction(canvas: drawing.Canvas) {
    let pen = new drawing.Pen();
    canvas.attachPen(pen);
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
      pen.setShadowLayer(shadowLayer);
    }
    drawAll(canvas);
    canvas.detachPen();
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

// draw过程中，detach pen
export class ReliabilityDetachPenWorker extends TestBase {

  /*
  @CaseID:SUB_BASIC_GRAPHICS_SPECIAL_STABLE_TS_DRAWING_RELIABILITY_PEN_0700
  @Description:draw过程中，detach pen
  @Step:
  1、运行脚本，循环执行hidumper命令查询内存
  2、执行自动化用例：
  draw过程中，detach pen
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
      let pen = new drawing.Pen();
      canvas.attachPen(pen);
      canvas.detachPen();
      console.log('function main', i);
    }
    console.log('function main end');
  }
}

// draw过程中，pen调用destroy接口
export class ReliabilityAttachPenWorker extends TestBase {

  /*
  @CaseID:SUB_BASIC_GRAPHICS_SPECIAL_STABLE_TS_DRAWING_RELIABILITY_PEN_0800
  @Description:draw过程中，pen调用destroy接口
  @Step:
  1、运行脚本，循环执行hidumper命令查询内存
  2、执行自动化用例：
  draw过程中，pen调用destroy接口
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
        let pen = new drawing.Pen();
        canvas.attachPen(pen);
        canvas.detachPen();
        console.log('function main', i);
      }
    }
    console.log('function main end');
  }
}

// draw过程中，pen 销毁ColorFilter
export class ReliabilityPenSetColorFilterWorker extends TestBase {

  /*
  @CaseID:SUB_BASIC_GRAPHICS_SPECIAL_STABLE_TS_DRAWING_RELIABILITY_PEN_0900
  @Description:draw过程中，pen 销毁ColorFilter
  @Step:
  1、运行脚本，循环执行hidumper命令查询内存
  2、执行自动化用例：
  draw过程中，pen 销毁ColorFilter
  3、执行结束后等待3分钟
  4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
  @Result:
  1、程序运行正常，无crash
  2、内存平稳，没有持续增长，执行后内存回落到执行前；
  */
  public async OnTestFunction(canvas: drawing.Canvas) {
    console.log('function main start');
    subWorker(canvas);
    let pen = new drawing.Pen();
    canvas.attachPen(pen);
    {
      let drawCount = 100;
      for (let i = 0; i < drawCount; i++) {
        let colorFilter = drawing.ColorFilter.createSRGBGammaToLinear();
        pen.setColorFilter(colorFilter);
        console.log('function main', i);
      }
    }
    canvas.detachPen();
    console.log('function main end');
  }
}

// draw过程中，pen 销毁MaskFilter
export class ReliabilityPenSetMaskFilterWorker extends TestBase {

  /*
  @CaseID:SUB_BASIC_GRAPHICS_SPECIAL_STABLE_TS_DRAWING_RELIABILITY_PEN_1000
  @Description:draw过程中，pen 销毁MaskFilter
  @Step:
  1、运行脚本，循环执行hidumper命令查询内存
  2、执行自动化用例：
  draw过程中，pen 销毁MaskFilter
  3、执行结束后等待3分钟
  4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
  @Result:
  1、程序运行正常，无crash
  2、内存平稳，没有持续增长，执行后内存回落到执行前；
  */
  public async OnTestFunction(canvas: drawing.Canvas) {
    console.log('function main start');
    subWorker(canvas);
    let pen = new drawing.Pen();
    canvas.attachPen(pen);
    {
      let drawCount = 100;
      for (let i = 0; i < drawCount; i++) {
        let sigma = 10;
        let blurType: drawing.BlurType = 0;
        let maskFilter = drawing.MaskFilter.createBlurMaskFilter(blurType, sigma);
        pen.setMaskFilter(maskFilter);
        console.log('function main', i);
      }
    }
    canvas.detachPen();
    console.log('function main end');
  }
}

// draw过程中，pen 销毁PathEffect
export class ReliabilityPenSetPathEffectWorker extends TestBase {

  /*
  @CaseID:SUB_BASIC_GRAPHICS_SPECIAL_STABLE_TS_DRAWING_RELIABILITY_PEN_1100
  @Description:draw过程中，pen 销毁PathEffect
  @Step:
  1、运行脚本，循环执行hidumper命令查询内存
  2、执行自动化用例：
  draw过程中，pen 销毁PathEffect
  3、执行结束后等待3分钟
  4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
  @Result:
  1、程序运行正常，无crash
  2、内存平稳，没有持续增长，执行后内存回落到执行前；
  */
  public async OnTestFunction(canvas: drawing.Canvas) {
    console.log('function main start');
    subWorker(canvas);
    let pen = new drawing.Pen();
    canvas.attachPen(pen);
    {
      let drawCount = 100;
      for (let i = 0; i < drawCount; i++) {
        let pathEffect = drawing.ShaderEffect.createColorShader(0x55ff0000);
        pen.setPathEffect(pathEffect);
        console.log('function main', i);
      }
    }
    canvas.detachPen();
    console.log('function main end');
  }
}

// draw过程中，pen 销毁ShadowLayer
export class ReliabilityPenSetShadowLayerWorker extends TestBase {

  /*
  @CaseID:SUB_BASIC_GRAPHICS_SPECIAL_STABLE_TS_DRAWING_RELIABILITY_PEN_1200
  @Description:draw过程中，pen 销毁ShadowLayer
  @Step:
  1、运行脚本，循环执行hidumper命令查询内存
  2、执行自动化用例：
  draw过程中，pen 销毁ShadowLayer
  3、执行结束后等待3分钟
  4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
  @Result:
  1、程序运行正常，无crash
  2、内存平稳，没有持续增长，执行后内存回落到执行前；
  */
  public async OnTestFunction(canvas: drawing.Canvas) {
    console.log('function main start');
    subWorker(canvas);
    let pen = new drawing.Pen();
    canvas.attachPen(pen);
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
        pen.setShadowLayer(shadowLayer);
        console.log('function main', i);
      }
    }
    canvas.detachPen();
    console.log('function main end');
  }
}