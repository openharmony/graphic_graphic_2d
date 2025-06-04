/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

import {
  TestBase,
  TestFunctionStyleType,
  StyleType,
} from "../../pages/testbase";
import { N, OHRandom } from "../../utils/OHRandom";
import { common2D, drawing, colorSpaceManager } from "@kit.ArkGraphics2D";
const TAG = "[BezierBench]";

// Print test result: Passed or Not Passed
function printResults(canvas: drawing.Canvas, isPassed: boolean) {
  let font: drawing.Font = new drawing.Font();
  font.setSize(50);
  let blob: drawing.TextBlob = drawing.TextBlob.makeFromString(
    isPassed ? "Passed" : "Not Passed",
    font,
    drawing.TextEncoding.TEXT_ENCODING_UTF8
  );
  canvas.drawTextBlob(blob, 10, 50);
}

function isEqualColor4f(
  color4f1: common2D.Color4f,
  color4f2: common2D.Color4f
): boolean {
  return (
    Math.abs(color4f1.alpha - color4f2.alpha) < 0.01 &&
    Math.abs(color4f1.red - color4f2.red) < 0.01 &&
    Math.abs(color4f1.green - color4f2.green) < 0.01 &&
    Math.abs(color4f1.blue - color4f2.blue) < 0.01
  );
}

function testColor4fBoundary(
  penOrBrush: drawing.Pen | drawing.Brush,
  color4f: common2D.Color4f
): boolean {
  let pass = true;
  try {
    penOrBrush.setColor4f(color4f, null);
  } catch (error) {
    console.log(TAG, "ColorSpacePenSetColor4fTest", error.code);
    pass = false;
  }
  return pass;
}

function testColor4fAbnormal(
  penOrBrush: drawing.Pen | drawing.Brush,
  color4f: common2D.Color4f
): boolean {
  let pass = true;
  penOrBrush.setColor4f(color4f, null);
  let color4f1 = penOrBrush.getColor4f();
  if (
    color4f1.alpha > 1.0 ||
    color4f1.alpha < 0 ||
    color4f1.red > 1.0 ||
    color4f1.red < 0 ||
    color4f1.green > 1.0 ||
    color4f1.green < 0 ||
    color4f1.blue > 1.0 ||
    color4f1.blue < 0
  ) {
    pass = false;
  }
  return pass;
}

function testGetColor4f(
  penOrBrush: drawing.Pen | drawing.Brush,
  colorSpace: colorSpaceManager.ColorSpaceManager | null,
  color4f: common2D.Color4f
): boolean {
  let pass = true;
  penOrBrush.setColor4f(color4f, colorSpace);
  let res = penOrBrush.getColor4f();
  pass = isEqualColor4f(color4f, res);
  return pass;
}

/*
 @CaseID:
 @Description:ColorSpacePenSetColor4f 功能测试 pen.setColor4f,colorspace前9个枚举
 */
export class ColorSpacePenSetColor4fTest extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }
  public OnTestPerformance(canvas: drawing.Canvas) {
    // 保证性能测试
    console.log(TAG, "ColorSpacePenSetColor4fTest");
    let pen = new drawing.Pen();
    let colorSpace = colorSpaceManager.create(
      colorSpaceManager.ColorSpace.BT2020_HLG
    );
    let color4f: common2D.Color4f = {
      alpha: 1,
      red: 0.5,
      green: 0.4,
      blue: 0.7,
    };
    for (let i = 0; i < this.testCount_; i++) {
      pen.setColor4f(color4f, colorSpace);
    }
    console.log(TAG, "ColorSpacePenSetColor4fTest22");
    printResults(canvas, true);
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    //接口调用，功能测试.cpu/gpu调用接口一致
    {
      console.log(TAG, "ColorSpacePenSetColor4fTest");
      let rect: common2D.Rect = { left: 10, top: 10, right: 100, bottom: 100 };
      let pen = new drawing.Pen();

      let color4f: common2D.Color4f = {
        alpha: 0.5,
        red: 0.5,
        green: 0.4,
        blue: 0.7,
      };

      pen.setColor4f(color4f, null);
      // colorSpaceManager
      const rowCount = 3; // 3 row count
      let row = 0;
      let column = 0;
      let colorSpace = null;
      for (
        let i = colorSpaceManager.ColorSpace.UNKNOWN + 1;
        i <= colorSpaceManager.ColorSpace.BT2020_HLG;
        i++
      ) {
        if (i != colorSpaceManager.ColorSpace.CUSTOM) {
          colorSpace = colorSpaceManager.create(i);
        }
        //unknown,custom, will crash when create
        pen.setColor4f(color4f, colorSpace);
        row = Math.floor((i - 1) / rowCount);
        column = (i - 1) % rowCount;
        canvas.attachPen(pen);
        let rect: common2D.Rect = {
          left: column * 200,
          top: row * 200,
          right: column * 200 + 198,
          bottom: row * 200 + 198,
        };
        canvas.drawRect(rect);
      }
      canvas.detachPen();
    }
  }

  public async OnTestStability(canvas: drawing.Canvas) {
    let pen = new drawing.Pen();
    let random = new OHRandom();
    let colorSpace = null;
    for (let i = 0; i < this.testCount_; i++) {
      let color4f: common2D.Color4f = {
        alpha: random.nextRangeF(0, 1.0),
        red: random.nextRangeF(0, 1.0),
        green: random.nextRangeF(0, 1.0),
        blue: random.nextRangeF(0, 1.0),
      };
      let i = random.nextRangeU(
        colorSpaceManager.ColorSpace.ADOBE_RGB_1998,
        colorSpaceManager.ColorSpace.LINEAR_BT2020
      );
      if (i != colorSpaceManager.ColorSpace.CUSTOM) {
        colorSpace = colorSpaceManager.create(i);
      }
      pen.setColor4f(color4f, colorSpace);
    }
    printResults(canvas, true);
  }
}

/*
 @CaseID:
 @Description:ColorSpacePenSetColor4f 功能测试 pen.setColor4f,colorspace中间9个枚举
 */
export class ColorSpacePenSetColor4f1Test extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }
  public OnTestPerformance(canvas: drawing.Canvas) {}

  public OnTestFunction(canvas: drawing.Canvas) {
    //接口调用，功能测试.cpu/gpu调用接口一致
    {
      let pen = new drawing.Pen();
      let color4f: common2D.Color4f = {
        alpha: 1,
        red: 0.5,
        green: 0.4,
        blue: 0.7,
      };
      let rowCount = 3;
      let row = 0;
      let column = 0;

      for (
        let i = colorSpaceManager.ColorSpace.BT2020_HLG;
        i <= colorSpaceManager.ColorSpace.BT601_EBU_LIMIT;
        i++
      ) {
        if (i == colorSpaceManager.ColorSpace.CUSTOM) {
          continue;
        }
        let colorSpace = colorSpaceManager.create(i);
        pen.setColor4f(color4f, colorSpace);
        row = Math.floor(
          (i - colorSpaceManager.ColorSpace.BT2020_HLG) / rowCount
        );
        column = (i - colorSpaceManager.ColorSpace.BT2020_HLG) % rowCount;
        canvas.attachPen(pen);
        let rect: common2D.Rect = {
          left: column * 200,
          top: row * 200,
          right: column * 200 + 198,
          bottom: row * 200 + 198,
        };
        canvas.drawRect(rect);
      }
      canvas.detachPen();
    }
  }

  public async OnTestStability(canvas: drawing.Canvas) {}
}

/*
 @CaseID:
 @Description:ColorSpacePenSetColor4f 功能测试 pen.setColor4f,colorspace最后几个
 */
export class ColorSpacePenSetColor4f2Test extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }
  public OnTestPerformance(canvas: drawing.Canvas) {}

  public OnTestFunction(canvas: drawing.Canvas) {
    //接口调用，功能测试.cpu/gpu调用接口一致
    {
      let pen = new drawing.Pen();
      let color4f: common2D.Color4f = {
        alpha: 1,
        red: 0.5,
        green: 0.4,
        blue: 0.7,
      };
      let rowCount = 3;
      let row = 0;
      let column = 0;

      for (
        let i = colorSpaceManager.ColorSpace.BT601_SMPTE_C_LIMIT;
        i <= colorSpaceManager.ColorSpace.LINEAR_BT2020;
        i++
      ) {
        let colorSpace = colorSpaceManager.create(i);
        pen.setColor4f(color4f, colorSpace);
        row = Math.floor(
          (i - colorSpaceManager.ColorSpace.BT601_SMPTE_C_LIMIT) / rowCount
        );
        column =
          (i - colorSpaceManager.ColorSpace.BT601_SMPTE_C_LIMIT) % rowCount;
        canvas.attachPen(pen);
        let rect: common2D.Rect = {
          left: column * 200,
          top: row * 200,
          right: column * 200 + 198,
          bottom: row * 200 + 198,
        };
        canvas.drawRect(rect);
      }
      canvas.detachPen();
    }
  }

  public async OnTestStability(canvas: drawing.Canvas) {}
}

/*
 @CaseID:
 @Description:ColorSpacePenSetColor4f 功能测试，pen.setColor4f,color4f超出范围值
 */
export class ColorSpacePenSetColor4fAbnormalTest extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {}

  public OnTestFunction(canvas: drawing.Canvas) {
    //接口调用，功能测试.cpu/gpu调用接口一致
    let pass = true;
    let pen = new drawing.Pen();
    if (
      !testColor4fAbnormal(pen, {
        alpha: 1.01,
        red: 0.5,
        green: 0.4,
        blue: 0.7,
      })
    ) {
      pass = false;
    }
    if (
      !testColor4fAbnormal(pen, {
        alpha: -0.01,
        red: 0.5,
        green: 0.4,
        blue: 0.7,
      })
    ) {
      pass = false;
    }
    if (
      !testColor4fAbnormal(pen, {
        alpha: 0.5,
        red: 1.01,
        green: 0.4,
        blue: 0.7,
      })
    ) {
      pass = false;
    }
    if (
      !testColor4fAbnormal(pen, {
        alpha: 0.5,
        red: -0.01,
        green: 0.4,
        blue: 0.7,
      })
    ) {
      pass = false;
    }
    if (
      !testColor4fAbnormal(pen, {
        alpha: 0.5,
        red: 0.5,
        green: 1.01,
        blue: 0.7,
      })
    ) {
      pass = false;
    }
    if (
      !testColor4fAbnormal(pen, {
        alpha: 0.5,
        red: 0.5,
        green: -0.01,
        blue: 0.7,
      })
    ) {
      pass = false;
    }
    if (
      !testColor4fAbnormal(pen, {
        alpha: 0.5,
        red: 0.5,
        green: 0.4,
        blue: 1.01,
      })
    ) {
      pass = false;
    }
    if (
      !testColor4fAbnormal(pen, {
        alpha: 0.5,
        red: 0.5,
        green: 0.4,
        blue: -0.01,
      })
    ) {
      pass = false;
    }
    printResults(canvas, pass);
  }

  public async OnTestStability(canvas: drawing.Canvas) {}
}

/*
 @CaseID:
 @Description:ColorSpacePenSetColor4f 功能测试，pen.setColor4f,color4f边界值，0.0,1.0
 */
export class ColorSpacePenSetColor4fBoundaryTest extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }
  public OnTestPerformance(canvas: drawing.Canvas) {}

  public OnTestFunction(canvas: drawing.Canvas) {
    //接口调用，功能测试.cpu/gpu调用接口一致
    let pass = true;
    console.log(TAG, "ColorSpacePenSetColor4fBoundaryTest");
    let pen = new drawing.Pen();
    if (
      !testColor4fBoundary(pen, { alpha: 0.0, red: 0.5, green: 0.5, blue: 0.7 })
    ) {
      pass = false;
    }
    if (
      !testColor4fBoundary(pen, { alpha: 1.0, red: 0.5, green: 0.5, blue: 0.7 })
    ) {
      pass = false;
    }
    if (
      !testColor4fBoundary(pen, { alpha: 0.5, red: 0.0, green: 0.5, blue: 0.7 })
    ) {
      pass = false;
    }
    if (
      !testColor4fBoundary(pen, { alpha: 0.5, red: 1.0, green: 0.5, blue: 0.7 })
    ) {
      pass = false;
    }
    if (
      !testColor4fBoundary(pen, { alpha: 0.5, red: 0.5, green: 0.0, blue: 0.7 })
    ) {
      pass = false;
    }
    if (
      !testColor4fBoundary(pen, { alpha: 0.5, red: 0.5, green: 1.0, blue: 0.7 })
    ) {
      pass = false;
    }
    if (
      !testColor4fBoundary(pen, { alpha: 0.5, red: 0.5, green: 0.5, blue: 0.0 })
    ) {
      pass = false;
    }
    if (
      !testColor4fBoundary(pen, { alpha: 0.5, red: 0.5, green: 0.5, blue: 1.0 })
    ) {
      pass = false;
    }
    printResults(canvas, pass);
  }

  public async OnTestStability(canvas: drawing.Canvas) {}
}

/*
 @CaseID:
 @Description:ColorSpacePenGetColor4fTest 功能测试，brush.getColor4f
 */
export class ColorSpacePenGetColor4fTest extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }
  public OnTestPerformance(canvas: drawing.Canvas) {}

  public OnTestFunction(canvas: drawing.Canvas) {
    //接口调用，功能测试.cpu/gpu调用接口一致
    let pass = true;
    console.log(TAG, "ColorSpacePenGetColor4fTest");
    let pen = new drawing.Pen();
    let random = new OHRandom();
    let colorSpace = null;
    for (let i = 0; i < 1000; i++) {
      let color4f: common2D.Color4f = {
        alpha: random.nextRangeF(0, 1.0),
        red: random.nextRangeF(0, 1.0),
        green: random.nextRangeF(0, 1.0),
        blue: random.nextRangeF(0, 1.0),
      };
      let i = random.nextRangeU(
        colorSpaceManager.ColorSpace.ADOBE_RGB_1998,
        colorSpaceManager.ColorSpace.LINEAR_BT2020
      );
      if (i != colorSpaceManager.ColorSpace.CUSTOM) {
        colorSpace = colorSpaceManager.create(i);
      }
      if (!testGetColor4f(pen, colorSpace, color4f)) {
        pass = false;
      }
    }
    printResults(canvas, pass);
  }
  public async OnTestStability(canvas: drawing.Canvas) {}
}

/*
 @CaseID:
 @Description:ColorSpaceBrushSetColor4fTest 功能测试brush setColor4f colorspace前9个枚举
 */
export class ColorSpaceBrushSetColor4fTest extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }
  public OnTestPerformance(canvas: drawing.Canvas) {
    // 保证性能测试
    let brush = new drawing.Brush();
    let colorSpace = colorSpaceManager.create(
      colorSpaceManager.ColorSpace.BT2020_HLG
    );
    let color4f: common2D.Color4f = {
      alpha: 1,
      red: 0.5,
      green: 0.4,
      blue: 0.7,
    };
    for (let i = 0; i < this.testCount_; i++) {
      brush.setColor4f(color4f, colorSpace);
    }
    printResults(canvas, true);
  }

  public OnTestFunction(canvas: drawing.Canvas) {
    //接口调用，功能测试.cpu/gpu调用接口一致
    {
      console.log(TAG, "ColorSpacePenSetColor4fTest");
      let brush = new drawing.Brush();

      let color4f: common2D.Color4f = {
        alpha: 0.5,
        red: 0.5,
        green: 0.4,
        blue: 0.7,
      };

      brush.setColor4f(color4f, null);
      const rowCount = 3; // 3 row count
      let row = 0;
      let column = 0;

      for (
        let i = colorSpaceManager.ColorSpace.UNKNOWN + 1;
        i <= colorSpaceManager.ColorSpace.BT2020_HLG;
        i++
      ) {
        if (i == colorSpaceManager.ColorSpace.CUSTOM) {
          continue;
        }
        //unknown,custom, will crash when create
        let colorSpace = colorSpaceManager.create(i);
        brush.setColor4f(color4f, colorSpace);
        row = Math.floor((i - 1) / rowCount);
        column = (i - 1) % rowCount;
        canvas.attachBrush(brush);
        let rect: common2D.Rect = {
          left: column * 200,
          top: row * 200,
          right: column * 200 + 198,
          bottom: row * 200 + 198,
        };
        canvas.drawRect(rect);
      }
      canvas.detachBrush();
    }
  }

  public async OnTestStability(canvas: drawing.Canvas) {
    let brush = new drawing.Brush();
    let random = new OHRandom();
    let colorSpace = null;
    for (let i = 0; i < this.testCount_; i++) {
      let color4f: common2D.Color4f = {
        alpha: random.nextRangeF(0, 1.0),
        red: random.nextRangeF(0, 1.0),
        green: random.nextRangeF(0, 1.0),
        blue: random.nextRangeF(0, 1.0),
      };
      let i = random.nextRangeU(
        colorSpaceManager.ColorSpace.ADOBE_RGB_1998,
        colorSpaceManager.ColorSpace.LINEAR_BT2020
      );
      if (i != colorSpaceManager.ColorSpace.CUSTOM) {
        colorSpace = colorSpaceManager.create(i);
      }
      brush.setColor4f(color4f, colorSpace);
    }
    printResults(canvas, true);
  }
}

/*
 @CaseID:
 @Description:ColorSpaceBrushSetColor4f 功能测试colorspace中间9个枚举
 */
export class ColorSpaceBrushSetColor4f1Test extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }
  public OnTestPerformance(canvas: drawing.Canvas) {}

  public OnTestFunction(canvas: drawing.Canvas) {
    //接口调用，功能测试.cpu/gpu调用接口一致
    {
      let brush = new drawing.Brush();
      let color4f: common2D.Color4f = {
        alpha: 1,
        red: 0.5,
        green: 0.4,
        blue: 0.7,
      };
      let rowCount = 3;
      let row = 0;
      let column = 0;

      for (
        let i = colorSpaceManager.ColorSpace.BT2020_HLG;
        i <= colorSpaceManager.ColorSpace.BT601_EBU_LIMIT;
        i++
      ) {
        let colorSpace = colorSpaceManager.create(i);
        brush.setColor4f(color4f, colorSpace);
        row = Math.floor(
          (i - colorSpaceManager.ColorSpace.BT2020_HLG) / rowCount
        );
        column = (i - colorSpaceManager.ColorSpace.BT2020_HLG) % rowCount;
        canvas.attachBrush(brush);
        let rect: common2D.Rect = {
          left: column * 200,
          top: row * 200,
          right: column * 200 + 198,
          bottom: row * 200 + 198,
        };
        canvas.drawRect(rect);
      }
      canvas.detachBrush();
    }
  }

  public async OnTestStability(canvas: drawing.Canvas) {}
}

/*
 @CaseID:
 @Description:ColorSpaceBrushSetColor4f2Test 功能测试colorspace最后几个
 */
export class ColorSpaceBrushSetColor4f2Test extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }
  public OnTestPerformance(canvas: drawing.Canvas) {}

  public OnTestFunction(canvas: drawing.Canvas) {
    //接口调用，功能测试.cpu/gpu调用接口一致
    {
      let brush = new drawing.Brush();
      let color4f: common2D.Color4f = {
        alpha: 1,
        red: 0.5,
        green: 0.4,
        blue: 0.7,
      };
      // colorSpaceManager
      let rowCount = 3;
      let row = 0;
      let column = 0;
      let colorSpace = null;
      for (
        let i = colorSpaceManager.ColorSpace.BT601_SMPTE_C_LIMIT;
        i <= colorSpaceManager.ColorSpace.LINEAR_BT2020;
        i++
      ) {
        if (i != colorSpaceManager.ColorSpace.CUSTOM) {
          //unknown,custom, will crash when create
          colorSpace = colorSpaceManager.create(i);
        }
        brush.setColor4f(color4f, colorSpace);
        row = Math.floor(
          (i - colorSpaceManager.ColorSpace.BT601_SMPTE_C_LIMIT) / rowCount
        );
        column =
          (i - colorSpaceManager.ColorSpace.BT601_SMPTE_C_LIMIT) % rowCount;
        canvas.attachBrush(brush);
        let rect: common2D.Rect = {
          left: column * 200,
          top: row * 200,
          right: column * 200 + 198,
          bottom: row * 200 + 198,
        };
        canvas.drawRect(rect);
      }
      canvas.detachBrush();
    }
  }

  public async OnTestStability(canvas: drawing.Canvas) {}
}

/*
 @CaseID:
 @Description:ColorSpaceBrushSetColor4fAbnormalTest 功能测试，color4f超出范围值
 */
export class ColorSpaceBrushSetColor4fAbnormalTest extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }

  public OnTestPerformance(canvas: drawing.Canvas) {}

  public OnTestFunction(canvas: drawing.Canvas) {
    //接口调用，功能测试.cpu/gpu调用接口一致
    let pass = true;
    let brush = new drawing.Brush();
    if (
      !testColor4fAbnormal(brush, {
        alpha: 1.01,
        red: 0.5,
        green: 0.4,
        blue: 0.7,
      })
    ) {
      pass = false;
    }
    if (
      !testColor4fAbnormal(brush, {
        alpha: -0.01,
        red: 0.5,
        green: 0.4,
        blue: 0.7,
      })
    ) {
      pass = false;
    }
    if (
      !testColor4fAbnormal(brush, {
        alpha: 0.5,
        red: 1.01,
        green: 0.4,
        blue: 0.7,
      })
    ) {
      pass = false;
    }
    if (
      !testColor4fAbnormal(brush, {
        alpha: 0.5,
        red: -0.01,
        green: 0.4,
        blue: 0.7,
      })
    ) {
      pass = false;
    }
    if (
      !testColor4fAbnormal(brush, {
        alpha: 0.5,
        red: 0.5,
        green: 1.01,
        blue: 0.7,
      })
    ) {
      pass = false;
    }
    if (
      !testColor4fAbnormal(brush, {
        alpha: 0.5,
        red: 0.5,
        green: -0.01,
        blue: 0.7,
      })
    ) {
      pass = false;
    }
    if (
      !testColor4fAbnormal(brush, {
        alpha: 0.5,
        red: 0.5,
        green: 0.4,
        blue: 1.01,
      })
    ) {
      pass = false;
    }
    if (
      !testColor4fAbnormal(brush, {
        alpha: 0.5,
        red: 0.5,
        green: 0.4,
        blue: -0.01,
      })
    ) {
      pass = false;
    }
    printResults(canvas, pass);
  }

  public async OnTestStability(canvas: drawing.Canvas) {}
}

/*
 @CaseID:
 @Description:ColorSpaceBrushSetColor4fBoundaryTest 功能测试，color4f边界值，0.0,1.0
 */
export class ColorSpaceBrushSetColor4fBoundaryTest extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }
  public OnTestPerformance(canvas: drawing.Canvas) {}

  public OnTestFunction(canvas: drawing.Canvas) {
    //接口调用，功能测试.cpu/gpu调用接口一致
    let pass = true;
    console.log(TAG, "ColorSpaceBrushSetColor4fBoundaryTest");
    let brush = new drawing.Brush();
    if (
      !testColor4fBoundary(brush, {
        alpha: 0.0,
        red: 0.5,
        green: 0.5,
        blue: 0.7,
      })
    ) {
      pass = false;
    }
    if (
      !testColor4fBoundary(brush, {
        alpha: 1.0,
        red: 0.5,
        green: 0.5,
        blue: 0.7,
      })
    ) {
      pass = false;
    }
    if (
      !testColor4fBoundary(brush, {
        alpha: 0.5,
        red: 0.0,
        green: 0.5,
        blue: 0.7,
      })
    ) {
      pass = false;
    }
    if (
      !testColor4fBoundary(brush, {
        alpha: 0.5,
        red: 1.0,
        green: 0.5,
        blue: 0.7,
      })
    ) {
      pass = false;
    }
    if (
      !testColor4fBoundary(brush, {
        alpha: 0.5,
        red: 0.5,
        green: 0.0,
        blue: 0.7,
      })
    ) {
      pass = false;
    }
    if (
      !testColor4fBoundary(brush, {
        alpha: 0.5,
        red: 0.5,
        green: 1.0,
        blue: 0.7,
      })
    ) {
      pass = false;
    }
    if (
      !testColor4fBoundary(brush, {
        alpha: 0.5,
        red: 0.5,
        green: 0.5,
        blue: 0.0,
      })
    ) {
      pass = false;
    }
    if (
      !testColor4fBoundary(brush, {
        alpha: 0.5,
        red: 0.5,
        green: 0.5,
        blue: 1.0,
      })
    ) {
      pass = false;
    }
    printResults(canvas, pass);
  }

  public async OnTestStability(canvas: drawing.Canvas) {}
}

/*
 @CaseID:
 @Description:ColorSpaceBrushGetColor4fTest 功能测试，获取colorspace4f
 */
export class ColorSpaceBrushGetColor4fTest extends TestBase {
  public constructor(styleType: number = StyleType.DRAW_STYLE_NONE) {
    super();
    this.styleType_ = styleType;
  }
  public OnTestPerformance(canvas: drawing.Canvas) {}

  public OnTestFunction(canvas: drawing.Canvas) {
    //接口调用，功能测试.cpu/gpu调用接口一致
    let pass = true;
    console.log(TAG, "ColorSpaceBrushGetColor4fTest");
    let brush = new drawing.Brush();
    let random = new OHRandom();
    for (let i = 0; i < 1000; i++) {
      let color4f: common2D.Color4f = {
        alpha: random.nextRangeF(0, 1.0),
        red: random.nextRangeF(0, 1.0),
        green: random.nextRangeF(0, 1.0),
        blue: random.nextRangeF(0, 1.0),
      };
      let colorSpace = null;
      let i = random.nextRangeU(
        colorSpaceManager.ColorSpace.ADOBE_RGB_1998,
        colorSpaceManager.ColorSpace.LINEAR_BT2020
      );
      if (i != colorSpaceManager.ColorSpace.CUSTOM) {
        colorSpace = colorSpaceManager.create(i);
      }
      if (!testGetColor4f(brush, colorSpace, color4f)) {
        pass = false;
      }
    }
    printResults(canvas, pass);
  }
  public async OnTestStability(canvas: drawing.Canvas) {}
}
