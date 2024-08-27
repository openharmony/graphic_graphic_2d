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
import { ColorFilterCreateMatrix } from '../interface/colorfiltertests';
import { drawAll } from './reliability_common';

// detach brush后，再调用draw接口
export class ReliabilityDetachBrush extends TestBase {
  public async OnTestFunction(canvas: drawing.Canvas) {
    let brush = new drawing.Brush();
    canvas.attachBrush(brush);
    canvas.detachBrush();
    drawAll(canvas);
  }
}

// attach brush后调用destroy接口，再调用draw接口
export class ReliabilityAttachBrush extends TestBase {
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