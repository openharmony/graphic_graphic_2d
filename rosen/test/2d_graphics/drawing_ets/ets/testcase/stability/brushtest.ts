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

enum BrushAllFunction {
  SET_COLOR = 0,
  SET_ANTI_ALIAS,
  SET_ALPHA,
  SET_COLOR_FILTER,
  SET_IMAGE_FILTER,
  SET_MASK_FILTER,
  SET_SHADER_EFFECT,
  SET_SHADOW_LAYER,
  SET_SET_BLEND_MODE,
  GET_COLOR,
  IS_ANTI_ALIAS,
  GET_ALPHA,
  GET_COLOR_FILTER,
  RESET,
  MAX
}

enum BrushOperationFunction {
  GET_COLOR,
  IS_ANTI_ALIAS,
  GET_ALPHA,
  GET_COLOR_FILTER,
  RESET,
  MAX
}

export class StabilityBrushNew extends TestBase {
  /*
  @CaseID:SUB_BASIC_GRAPHICS_SPECIAL_STABLE_TS_DRAWING_BRUSH_0100
  @Description:brush new-置空，循环调用1000次
  @Step:
  1、运行脚本，循环执行hidumper命令查询内存
  2、执行自动化用例：
  brush new-置空，循环调用1000次
  3、执行结束后等待3分钟
  4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
  @Result:
  1、程序运行正常，无crash
  2、内存平稳，没有持续增长，执行后内存回落到执行前；
  */
  public async OnTestStability(canvas: drawing.Canvas) {
    console.log("BrushNew OnTestStability");
    for (let i = 0; i < this.testCount_; i++) {
      let brush = new drawing.Brush();
      let brush1 = new drawing.Brush(brush);
    }
  }
}

export class StabilityBrushInvoke extends TestBase {
  private colorFilter: drawing.ColorFilter = new drawing.ColorFilter();
  private imageFilter: drawing.ImageFilter = new drawing.ImageFilter();
  private maskFilter: drawing.MaskFilter = new drawing.MaskFilter();
  private shadowLayer: drawing.ShadowLayer = new drawing.ShadowLayer();
  private colorShader: drawing.ShaderEffect = new drawing.ShaderEffect();
  private gradientEffect: drawing.ShaderEffect = new drawing.ShaderEffect();
  private gradientEffect2: drawing.ShaderEffect = new drawing.ShaderEffect();
  private gradientEffect3: drawing.ShaderEffect = new drawing.ShaderEffect();

  static functions_: Map<BrushAllFunction, (brushInvoke: StabilityBrushInvoke, brush: drawing.Brush) => void> = new Map([
    [BrushAllFunction.SET_COLOR, (brushInvoke: StabilityBrushInvoke, brush: drawing.Brush) => {
      brushInvoke.BrushSetColorTest(brush)
    }],
    [BrushAllFunction.SET_ANTI_ALIAS, (brushInvoke: StabilityBrushInvoke, brush: drawing.Brush) => {
      brushInvoke.BrushSetAntiAliasTest(brush)
    }],
    [BrushAllFunction.SET_ALPHA, (brushInvoke: StabilityBrushInvoke, brush: drawing.Brush) => {
      brushInvoke.BrushSetAlphaTest(brush)
    }],
    [BrushAllFunction.SET_COLOR_FILTER, (brushInvoke: StabilityBrushInvoke, brush: drawing.Brush) => {
      brushInvoke.BrushSetColorFilterTest(brush)
    }],
    [BrushAllFunction.SET_IMAGE_FILTER, (brushInvoke: StabilityBrushInvoke, brush: drawing.Brush) => {
      brushInvoke.BrushSetImageFilterTest(brush)
    }],
    [BrushAllFunction.SET_MASK_FILTER, (brushInvoke: StabilityBrushInvoke, brush: drawing.Brush) => {
      brushInvoke.BrushSetMaskFilterTest(brush)
    }],
    [BrushAllFunction.SET_SHADER_EFFECT, (brushInvoke: StabilityBrushInvoke, brush: drawing.Brush) => {
      brushInvoke.BrushSetShaderEffectTest(brush)
    }],
    [BrushAllFunction.SET_SHADOW_LAYER, (brushInvoke: StabilityBrushInvoke, brush: drawing.Brush) => {
      brushInvoke.BrushSetShadowLayerTest(brush)
    }],
    [BrushAllFunction.SET_SET_BLEND_MODE, (brushInvoke: StabilityBrushInvoke, brush: drawing.Brush) => {
      brushInvoke.BrushSetBlendModTest(brush)
    }],
    [BrushAllFunction.GET_COLOR, (brushInvoke: StabilityBrushInvoke, brush: drawing.Brush) => {
      brushInvoke.BrushGetColorTest(brush)
    }],
    [BrushAllFunction.IS_ANTI_ALIAS, (brushInvoke: StabilityBrushInvoke, brush: drawing.Brush) => {
      brushInvoke.BrushIsAntiAliasTest(brush)
    }],
    [BrushAllFunction.GET_ALPHA, (brushInvoke: StabilityBrushInvoke, brush: drawing.Brush) => {
      brushInvoke.BrushGetAlphaTest(brush)
    }],
    [BrushAllFunction.GET_COLOR_FILTER, (brushInvoke: StabilityBrushInvoke, brush: drawing.Brush) => {
      brushInvoke.BrushGetColorFilterTest(brush)
    }],
    [BrushAllFunction.RESET, (brushInvoke: StabilityBrushInvoke, brush: drawing.Brush) => {
      brushInvoke.BrushResetTest(brush)
    }],
  ]);

  CreateConicalGradientEffect(): drawing.ShaderEffect {
    const startPoint: common2D.Point =
      { x: OHRandom.GetRandomNum(0, this.width_), y: OHRandom.GetRandomNum(0, this.height_) }; //中心点坐标
    const startRadius = OHRandom.GetRandomNum(0, 360);
    const endPoint: common2D.Point =
      { x: OHRandom.GetRandomNum(0, this.width_), y: OHRandom.GetRandomNum(0, this.height_) }; //中心点坐标
    const endRadius = OHRandom.GetRandomNum(0, 360);
    const colors = [0xFF0000FF, 0xFFFF0000]; // 1蓝色到红色的渐变 (ARGB)
    const tileMode = drawing.TileMode.CLAMP; // 平铺模式为 CLAMP
    const positions = [0.0, 1.0];
    this.gradientEffect = drawing.ShaderEffect.createConicalGradient(
      startPoint,
      startRadius,
      endPoint,
      endRadius,
      colors,
      tileMode,
      positions,
      null
    );
    return this.gradientEffect;
  }

  CreateGradientEffect(): drawing.ShaderEffect {
    const colors2 = [0xFF0000FF, 0xFFFF0000]; // 1蓝色到红色的渐变 (ARGB)
    const centerPoint2: common2D.Point =
      { x: OHRandom.GetRandomNum(0, this.width_), y: OHRandom.GetRandomNum(0, this.height_) }; //中心点坐标
    const radius2 = OHRandom.GetRandomNum(0, this.width_ / 2);
    const tileMode2 = drawing.TileMode.REPEAT;
    const positions2 = [0.0, 1.0];
    this.gradientEffect2 = drawing.ShaderEffect.createRadialGradient(
      centerPoint2,
      radius2,
      colors2,
      tileMode2,
      positions2,
      null
    );
    return this.gradientEffect2
  }

  CreateSweepGradientEffect(): drawing.ShaderEffect {
    const colors3 = [0xFF0000FF, 0xFFFF0000]; // 1蓝色到红色的渐变 (ARGB)
    const centerPoint3: common2D.Point =
      { x: OHRandom.GetRandomNum(0, this.width_), y: OHRandom.GetRandomNum(0, this.height_) }; //中心点坐标
    const tileMode3 = drawing.TileMode.REPEAT;
    const startAngle = OHRandom.GetRandomNum(0, 360); // 开始角度
    const endAngle = OHRandom.GetRandomNum(0, 360); // 结束角度
    const positions3 = [0.0, 1.0];
    this.gradientEffect3 = drawing.ShaderEffect.createSweepGradient(
      centerPoint3,
      colors3,
      tileMode3,
      startAngle,
      endAngle,
      positions3,
      null
    );
    return this.gradientEffect3;
  }

  /*
  @CaseID:SUB_BASIC_GRAPHICS_SPECIAL_STABLE_TS_DRAWING_BRUSH_0200
  @Description:Brush new-相关接口全调用-置空循环
  @Step:
  1、运行脚本，循环执行hidumper命令查询内存
  2、执行自动化用例：
  new brush-相关接口全调用-置空，循环调用1000次
  3、执行结束后等待3分钟
  4、结束脚本运行，抓取的内存值转成折线图，观察内存变化"
  @Result:
  1、程序运行正常，无crash
  2、内存平稳，没有持续增长，执行后内存回落到执行前；
  */
  public async OnTestStability(canvas: drawing.Canvas) {
    console.log("BrushInvoke OnTestStability");
    this.colorFilter = drawing.ColorFilter.createBlendModeColorFilter({
      alpha: OHRandom.GetRandomNum(0, 0xFF),
      red: OHRandom.GetRandomNum(0, 0xFF),
      green: OHRandom.GetRandomNum(0, 0xFF),
      blue: OHRandom.GetRandomNum(0, 0xFF)
    }, OHRandom.GetRandomNum(0, 0xFF) % (drawing.BlendMode.LUMINOSITY + 1));
    this.imageFilter = drawing.ImageFilter.createFromColorFilter(this.colorFilter);
    this.maskFilter =
      drawing.MaskFilter.createBlurMaskFilter(OHRandom.GetRandomNum(0, drawing.BlurType.INNER),
        OHRandom.GetRandomNum(1, 0xFFFFFFFF));
    let min = Math.min(this.width_, this.height_);
    this.shadowLayer =
      drawing.ShadowLayer.create(OHRandom.GetRandomNum(1, min / 2), OHRandom.GetRandomNum(0, this.width_),
        OHRandom.GetRandomNum(0, this.height_), {
        alpha: OHRandom.GetRandomNum(0, 0xFF),
        red: OHRandom.GetRandomNum(0, 0xFF),
        green: OHRandom.GetRandomNum(0, 0xFF),
        blue: OHRandom.GetRandomNum(0, 0xFF)
      });
    this.colorShader = drawing.ShaderEffect.createColorShader(0xFF00FF00); // 绿色，完全不透明;
    this.gradientEffect = this.CreateConicalGradientEffect();
    this.gradientEffect2 = this.CreateGradientEffect();
    this.gradientEffect3 = this.CreateSweepGradientEffect();
    for (let i = 0; i < this.testCount_; i++) {
      let brush = new drawing.Brush();
      for (let j = 0; j < BrushAllFunction.MAX; j++) {
        StabilityBrushInvoke.functions_.get(j)(this, brush);
      }
    }
  }

  BrushSetColorTest(brush: drawing.Brush) {
    brush.setColor({
      alpha: OHRandom.GetRandomNum(0, 0xFF),
      red: OHRandom.GetRandomNum(0, 0xFF),
      green: OHRandom.GetRandomNum(0, 0xFF),
      blue: OHRandom.GetRandomNum(0, 0xFF)
    });
    // todo: setColor(color: number): void;此方法还未转测
    // brush.setColor(OHRandom.GetRandomNum(0, 0xFF), OHRandom.GetRandomNum(0, 0xFF),
    //   OHRandom.GetRandomNum(0, 0xFF), OHRandom.GetRandomNum(0, 0xFF));
  }

  BrushSetAntiAliasTest(brush: drawing.Brush) {
    brush.setAntiAlias(OHRandom.GetRandomNum(0, 1) % 2 == 0);
  }

  BrushSetAlphaTest(brush: drawing.Brush) {
    brush.setAlpha(OHRandom.GetRandomNum(0, 0xFF));
  }

  BrushSetColorFilterTest(brush: drawing.Brush) {
    brush.setColorFilter(this.colorFilter);
  }

  BrushSetImageFilterTest(brush: drawing.Brush) {
    brush.setImageFilter(this.imageFilter);
  }

  BrushSetMaskFilterTest(brush: drawing.Brush) {
    brush.setMaskFilter(this.maskFilter);
  }

  BrushSetShadowLayerTest(brush: drawing.Brush) {
    brush.setShadowLayer(this.shadowLayer);
  }

  BrushSetShaderEffectTest(brush: drawing.Brush) {
    //1.创建具有单一颜色的着色器
    brush.setShaderEffect(this.colorShader);
    //2.createConicalGradient创建着色器，在给定两个圆之间生成渐变。
    brush.setShaderEffect(this.gradientEffect);
    //3.创建着色器，在给定圆心和半径的情况下生成径向渐变。 从起点到终点颜色从内到外进行圆形渐变（从中间向外拉）被称为径向渐变。
    brush.setShaderEffect(this.gradientEffect2);
    //4.createSweepGradient
    brush.setShaderEffect(this.gradientEffect3);
  }

  BrushSetBlendModTest(brush: drawing.Brush) {
    brush.setBlendMode(OHRandom.GetRandomNum(0, drawing.BlendMode.LUMINOSITY));
  }

  BrushGetColorTest(brush: drawing.Brush) {
    // todo: setColor(color: number): void;此方法还未转测
    // brush.setColor(100);
    let color: common2D.Color = { alpha: 22, red: 255, green: 0, blue: 88 };
    brush.setColor(color);
    brush.getColor();
  }

  BrushIsAntiAliasTest(brush: drawing.Brush) {
    brush.isAntiAlias();
  }

  BrushGetAlphaTest(brush: drawing.Brush) {
    brush.getAlpha();
  }

  BrushGetColorFilterTest(brush: drawing.Brush) {
    brush.getColorFilter();
  }

  BrushResetTest(brush: drawing.Brush) {
    brush.reset();
  }
}

export class StabilityBrushRandInvoke extends TestBase {
  static functions_: Map<BrushOperationFunction, (brushInvoke: StabilityBrushRandInvoke, brush: drawing.Brush) => void> =
    new Map([
      [BrushOperationFunction.GET_COLOR, (brushRandInvoke: StabilityBrushRandInvoke, brush: drawing.Brush) => {
        brushRandInvoke.BrushGetColorTest(brush)
      }],
      [BrushOperationFunction.IS_ANTI_ALIAS, (brushRandInvoke: StabilityBrushRandInvoke, brush: drawing.Brush) => {
        brushRandInvoke.BrushIsAntiAliasTest(brush)
      }],
      [BrushOperationFunction.GET_ALPHA, (brushRandInvoke: StabilityBrushRandInvoke, brush: drawing.Brush) => {
        brushRandInvoke.BrushGetAlphaTest(brush)
      }],
      [BrushOperationFunction.GET_COLOR_FILTER, (brushRandInvoke: StabilityBrushRandInvoke, brush: drawing.Brush) => {
        brushRandInvoke.BrushGetColorFilterTest(brush)
      }],
      [BrushOperationFunction.RESET, (brushRandInvoke: StabilityBrushRandInvoke, brush: drawing.Brush) => {
        brushRandInvoke.BrushResetTest(brush)
      }],
    ]);

  /*
  @CaseID:SUB_BASIC_GRAPHICS_SPECIAL_STABLE_TS_DRAWING_BRUSH_0300
  @Description:Brush相关操作接口随机循环调用
  @Step:
  1、运行脚本，循环执行hidumper命令查询内存
  2、执行自动化用例：
  new brush
  brush相关操作接口调用，循环调用1000次
  brush置空
  3、执行结束后等待3分钟
  4、结束脚本运行，抓取的内存值转成折线图，观察内存变化"
  @Result:
  1、程序运行正常，无crash
  2、内存平稳，没有持续增长，执行后内存回落到执行前；
  */
  public async OnTestStability(canvas: drawing.Canvas) {
    console.log("BrushRandInvoke OnTestStability");
    let brush = new drawing.Brush();
    for (let i = 0; i < this.testCount_; i++) {
      StabilityBrushRandInvoke.functions_.get(OHRandom.GetRandomNum(0, BrushOperationFunction.MAX - 1))(this, brush);
    }
  }

  BrushGetColorTest(brush: drawing.Brush) {
    brush.getColor();
  }

  BrushIsAntiAliasTest(brush: drawing.Brush) {
    brush.isAntiAlias();
  }

  BrushGetAlphaTest(brush: drawing.Brush) {
    brush.getAlpha();
  }

  BrushGetColorFilterTest(brush: drawing.Brush) {
    brush.getColorFilter();
  }

  BrushResetTest(brush: drawing.Brush) {
    brush.reset();
  }
}