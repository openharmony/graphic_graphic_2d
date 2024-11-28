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

enum TextBlobAllFunction {
  MAKE_FROM_STRING,
  MAKE_FROM_POS_TEXT,
  MAKE_FROM_RUN_BUFFER,
  BOUNDS,
  UNIQUE_ID,
  MAX
}

enum TextBlobOperationFunction {
  BOUNDS,
  UNIQUE_ID,
  MAX
}

export class StabilityTextBlobMakeFromString extends TestBase {
  /*
  @CaseID:SUB_BASIC_GRAPHICS_SPECIAL_STABLE_TS_DRAWING_TEXTBLOB_0100
  @Description:TextBlob make-置空循环
  @Step:
  1、运行脚本，循环执行hidumper命令查询内存
  2、执行自动化用例：
  TextBlob makeFromString-置空，循环调用1000次
  3、执行结束后等待3分钟
  4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
  @Result:
  1、程序运行正常，无crash
  2、内存平稳，没有持续增长，执行后内存回落到执行前；
  */
  public async OnTestStability(canvas: drawing.Canvas) {
    console.log("TextBlobMakeFromString OnTestStability");
    const font = new drawing.Font();
    for (let i = 0; i < this.testCount_; i++) {
      drawing.TextBlob.makeFromString("drawing" + "你好" + ",.%&^*@#$", font,
        drawing.TextEncoding.TEXT_ENCODING_UTF8);
    }
  }
}

export class StabilityTextBlobMakeFromRunBuffer extends TestBase {
  /*
  @CaseID:SUB_BASIC_GRAPHICS_SPECIAL_STABLE_TS_DRAWING_TEXTBLOB_0400
  @Description:TextBlob make-置空循环
  @Step:
  1、运行脚本，循环执行hidumper命令查询内存
  2、执行自动化用例：
  TextBlob makeFromRunBuffer-置空，循环调用1000次
  3、执行结束后等待3分钟
  4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
  @Result:
  1、程序运行正常，无crash
  2、内存平稳，没有持续增长，执行后内存回落到执行前；
  */
  public async OnTestStability(canvas: drawing.Canvas) {
    console.log("TextBlobMakeFromRunBuffer OnTestStability");
    const font = new drawing.Font();
    let glypha = OHRandom.GetRandomNum(0, this.width_);
    let glyphb = OHRandom.GetRandomNum(0, this.height_);
    let positionxa = OHRandom.GetRandomNum(0, this.width_);
    let positionya = OHRandom.GetRandomNum(0, this.height_);
    let positionxb = OHRandom.GetRandomNum(0, this.width_);
    let positionyb = OHRandom.GetRandomNum(0, this.height_);
    let runBuffer: Array<drawing.TextBlobRunBuffer> = [
      { glyph: glypha, positionX: positionxa, positionY: positionya },
      { glyph: glyphb, positionX: positionxb, positionY: positionyb }
    ];
    for (let i = 0; i < this.testCount_; i++) {
      drawing.TextBlob.makeFromRunBuffer(runBuffer, font, null);
    }
  }
}

export class StabilityTextBlobMakeInvoke extends TestBase {
  private glypha = OHRandom.GetRandomNum(0, this.width_);
  private glyphb = OHRandom.GetRandomNum(0, this.height_);
  private positionxa = OHRandom.GetRandomNum(0, this.width_);
  private positionya = OHRandom.GetRandomNum(0, this.height_);
  private positionxb = OHRandom.GetRandomNum(0, this.width_);
  private positionyb = OHRandom.GetRandomNum(0, this.height_);
  private runBuffer: Array<drawing.TextBlobRunBuffer>;
  private points: Array<common2D.Point> = null;
  private font:drawing.Font = new drawing.Font();
  private text:string = "drawing" + "你好" + ",.%&^*@#$";

  static functions_: Map<TextBlobAllFunction, (textBlobMakeInvoke: StabilityTextBlobMakeInvoke, textBlob: drawing.TextBlob) => void> =
    new Map([
      [TextBlobAllFunction.MAKE_FROM_STRING, (textBlobMakeInvoke: StabilityTextBlobMakeInvoke, textBlob: drawing.TextBlob) => {
        textBlobMakeInvoke.TextBlobMakeFromStringTest(textBlob)
      }],
      [TextBlobAllFunction.MAKE_FROM_POS_TEXT, (textBlobMakeInvoke: StabilityTextBlobMakeInvoke, textBlob: drawing.TextBlob) => {
        textBlobMakeInvoke.TextBlobMakeFromPosTextTest(textBlob)
      }],
      [TextBlobAllFunction.MAKE_FROM_RUN_BUFFER, (textBlobMakeInvoke: StabilityTextBlobMakeInvoke, textBlob: drawing.TextBlob) => {
        textBlobMakeInvoke.TextBlobMakeFromRunBufferTest(textBlob)
      }],
      [TextBlobAllFunction.BOUNDS, (textBlobMakeInvoke: StabilityTextBlobMakeInvoke, textBlob: drawing.TextBlob) => {
        textBlobMakeInvoke.TextBlobBoundsTest(textBlob)
      }],
      [TextBlobAllFunction.UNIQUE_ID, (textBlobMakeInvoke: StabilityTextBlobMakeInvoke, textBlob: drawing.TextBlob) => {
        textBlobMakeInvoke.TextBlobUniqueIDTest(textBlob)
      }],
    ]);

  /*
  @CaseID:SUB_BASIC_GRAPHICS_SPECIAL_STABLE_TS_DRAWING_TEXTBLOB_0200
  @Description:TextBlob make-相关接口全调用-置空循环
  @Step:
  1、运行脚本，循环执行hidumper命令查询内存
  2、执行自动化用例：
  TextBlob make-相关接口全调用-置空，循环调用1000次
  3、执行结束后等待3分钟
  4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
  @Result:
  1、程序运行正常，无crash
  2、内存平稳，没有持续增长，执行后内存回落到执行前；
  */
  public async OnTestStability(canvas: drawing.Canvas) {
    console.log("TextBlobMakeALL OnTestStability");
    this.runBuffer = [
      { glyph: this.glypha, positionX: this.positionxa, positionY: this.positionya },
      { glyph: this.glyphb, positionX: this.positionxb, positionY: this.positionyb }
    ];
    this.points = new Array;
    for (let j = 0; j < this.text.length; j++) {
      this.points.push({ x: OHRandom.GetRandomNum(0, 720), y: OHRandom.GetRandomNum(0, 720) });
    }
    let textblob = new drawing.TextBlob();
    for (let i = 0; i < this.testCount_; i++) {
      for (let j = 0; j < TextBlobAllFunction.MAX; j++) {
        StabilityTextBlobMakeInvoke.functions_.get(j)(this, textblob);
      }
    }
  }

  TextBlobMakeFromStringTest(textBlob: drawing.TextBlob) {
    drawing.TextBlob.makeFromString("drawing" + "你好" + ",.%&^*@#$", this.font, drawing.TextEncoding.TEXT_ENCODING_UTF8);
  }

  TextBlobMakeFromPosTextTest(textBlob: drawing.TextBlob) {
    drawing.TextBlob.makeFromPosText(this.text, this.text.length, this.points, this.font);
  }

  TextBlobMakeFromRunBufferTest(textBlob: drawing.TextBlob) {
    drawing.TextBlob.makeFromRunBuffer(this.runBuffer, this.font, null);
  }

  TextBlobBoundsTest(textBlob: drawing.TextBlob) {
    textBlob.bounds();
  }

  TextBlobUniqueIDTest(textBlob: drawing.TextBlob) {
    textBlob.uniqueID();
  }
}

export class StabilityTextBlobRandInvoke extends TestBase {
  static functions_: Map<TextBlobOperationFunction, (textBlobRandInvoke: StabilityTextBlobRandInvoke,
    textBlob: drawing.TextBlob) => void> =
    new Map([
      [TextBlobOperationFunction.BOUNDS, (textBlobRandInvoke: StabilityTextBlobRandInvoke, textBlob: drawing.TextBlob) => {
        textBlobRandInvoke.TextBlobBoundsTest(textBlob)
      }],
      [TextBlobOperationFunction.UNIQUE_ID, (textBlobRandInvoke: StabilityTextBlobRandInvoke, textBlob: drawing.TextBlob) => {
        textBlobRandInvoke.TextBlobUniqueIDTest(textBlob)
      }],
    ]);

  /*
  @CaseID:SUB_BASIC_GRAPHICS_SPECIAL_STABLE_TS_DRAWING_TEXTBLOB_0300
  @Description:TextBlob相关操作接口随机循环调用
  @Step:
  1、运行脚本，循环执行hidumper命令查询内存
  2、执行自动化用例：
  TextBlob make
  TextBlob相关操作接口调用，循环调用1000次
  TextBlob置空
  3、执行结束后等待3分钟
  4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
  @Result:
  1、程序运行正常，无crash
  2、内存平稳，没有持续增长，执行后内存回落到执行前；
  */
  public async OnTestStability(canvas: drawing.Canvas) {
    console.log("TextBlobRandInvoke OnTestStability");
    const font = new drawing.Font();
    let textBlob = drawing.TextBlob.makeFromString("drawing" + "你好" + ",.%&^*@#$", font,
      drawing.TextEncoding.TEXT_ENCODING_UTF8);
    for (let i = 0; i < this.testCount_; i++) {
      StabilityTextBlobRandInvoke.functions_.get(OHRandom.GetRandomNum(0, TextBlobOperationFunction.MAX - 1))(this, textBlob);
    }
  }

  TextBlobBoundsTest(textBlob: drawing.TextBlob) {
    textBlob.bounds();
  }

  TextBlobUniqueIDTest(textBlob: drawing.TextBlob) {
    textBlob.uniqueID();
  }
}