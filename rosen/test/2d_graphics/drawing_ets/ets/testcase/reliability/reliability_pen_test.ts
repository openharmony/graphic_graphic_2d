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
  public async OnTestFunction(canvas: drawing.Canvas) {
    let pen = new drawing.Pen();
    canvas.attachPen(pen);
    canvas.detachPen();
    drawAll(canvas);
  }
}

// attach pen后调用destroy接口，再调用draw接口
export class ReliabilityAttachPen extends TestBase {
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