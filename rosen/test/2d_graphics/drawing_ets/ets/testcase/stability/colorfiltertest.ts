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
import { TestBase } from '../../pages/testbase';

const TAG = '[BezierBench]';

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_STABLE_TS_DRAWING_COLORFILTER_0100
@Description:createBlendModeColorFilter-置空循环
@Step:
1、运行脚本，循环执行hidumper命令查询内存
2、执行自动化用例:
createBlendModeColorFilter-置空，循环调用1000次
3、执行结束后等待3分钟
4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
@Result:
1、程序运行正常，无crash
2、内存平稳，没有持续增长，执行后内存回落到执行前；
*/
export class StabilityCreateBlendModeColorFilter extends TestBase {
  public async OnTestStability(canvas: drawing.Canvas) {
    console.log("CreateBlendModeColorFilter OnTestStability");
    for (let i = 0; i < this.testCount_; i++) {
      // todo: static createBlendModeColorFilter(color: number, mode: BlendMode): ColorFilter;此方法还未转测
      //let colorFilter = drawing.ColorFilter.createBlendModeColorFilter(0x66FFFF00, i % (drawing.BlendMode.SRC + 1));
      let colorFilter = drawing.ColorFilter.createBlendModeColorFilter({
        alpha: 66,
        red: 255,
        green: 255,
        blue: 0
      }, i % (drawing.BlendMode.SRC + 1));
    }
  }
}

export class StabilityCreateComposeColorFilter extends TestBase {

  /*
  @CaseID:SUB_BASIC_GRAPHICS_SPECIAL_STABLE_TS_DRAWING_COLORFILTER_0200
  @Description:createComposeColorFilter-置空循环
  @Step:
  1、运行脚本，循环执行hidumper命令查询内存
  2、执行自动化用例：
  createComposeColorFilter-置空，循环调用1000次
  3、执行结束后等待3分钟
  4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
  @Result:
  1、程序运行正常，无crash
  2、内存平稳，没有持续增长，执行后内存回落到执行前；
  */
  public async OnTestStability(canvas: drawing.Canvas) {
    console.log("CreateComposeColorFilter OnTestStability");
    for (let i = 0; i < this.testCount_; i++) {
      // todo: static createBlendModeColorFilter(color: number, mode: BlendMode): ColorFilter;此方法还未转测
      // let colorFilter1 = drawing.ColorFilter.createBlendModeColorFilter(0xFFFFFF00, i % (drawing.BlendMode.SRC + 1));
      // let colorFilter2 = drawing.ColorFilter.createBlendModeColorFilter(0xFF00FF00, i % (drawing.BlendMode.DST + 1));
      let colorFilter1 = drawing.ColorFilter.createBlendModeColorFilter({
        alpha: 255,
        red: 255,
        green: 255,
        blue: 0
      }, i % (drawing.BlendMode.SRC + 1));
      let colorFilter2 = drawing.ColorFilter.createBlendModeColorFilter({
        alpha: 255,
        red: 0,
        green: 255,
        blue: 0
      }, i % (drawing.BlendMode.SRC + 1));
      let colorFilter = drawing.ColorFilter.createComposeColorFilter(colorFilter1, colorFilter2);
    }
  }
}

export class StabilityCreateLinearToSRGBGamma extends TestBase {

  /*
  @CaseID:SUB_BASIC_GRAPHICS_SPECIAL_STABLE_TS_DRAWING_COLORFILTER_0300
  @Description:createLinearToSRGBGamma-置空循环
  @Step:
  1、运行脚本，循环执行hidumper命令查询内存
  2、执行自动化用例：
  createLinearToSRGBGamma-置空循环，循环调用1000次
  3、执行结束后等待3分钟
  4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
  @Result:
  1、程序运行正常，无crash
  2、内存平稳，没有持续增长，执行后内存回落到执行前；
  */
  public async OnTestStability(canvas: drawing.Canvas) {
    console.log("CreateLinearToSRGBGamma OnTestStability");
    for (let i = 0; i < this.testCount_; i++) {
      let colorFilter = drawing.ColorFilter.createLinearToSRGBGamma();
    }
  }
}


export class StabilityCreateSRGBGammaToLinear extends TestBase {

  /*
  @CaseID:SUB_BASIC_GRAPHICS_SPECIAL_STABLE_TS_DRAWING_COLORFILTER_0400
  @Description:createSRGBGammaToLinear-置空循环
  @Step:
  1、运行脚本，循环执行hidumper命令查询内存
  2、执行自动化用例：
  createSRGBGammaToLinear-置空循环，循环调用1000次
  3、执行结束后等待3分钟
  4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
  @Result:
  1、程序运行正常，无crash
  2、内存平稳，没有持续增长，执行后内存回落到执行前；
  */
  public async OnTestStability(canvas: drawing.Canvas) {
    console.log("CreateSRGBGammaToLinear OnTestStability");
    for (let i = 0; i < this.testCount_; i++) {
      let colorFilter = drawing.ColorFilter.createSRGBGammaToLinear();
    }
  }
}

export class StabilityCreateLumaColorFilter extends TestBase {

  /*
  @CaseID:SUB_BASIC_GRAPHICS_SPECIAL_STABLE_TS_DRAWING_COLORFILTER_0500
  @Description:createLumaColorFilter-置空循环
  @Step:
  1、运行脚本，循环执行hidumper命令查询内存
  2、执行自动化用例：
  createLumaColorFilter-置空循环，循环调用1000次
  3、执行结束后等待3分钟
  4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
  @Result:
  1、程序运行正常，无crash
  2、内存平稳，没有持续增长，执行后内存回落到执行前；
  */
  public async OnTestStability(canvas: drawing.Canvas) {
    console.log("CreateLumaColorFilter OnTestStability");
    for (let i = 0; i < this.testCount_; i++) {
      let colorFilter = drawing.ColorFilter.createLumaColorFilter();
    }
  }
}
