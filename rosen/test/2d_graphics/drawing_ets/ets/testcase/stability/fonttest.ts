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
import { OHRandom } from '../../utils/OHRandom';

enum FontAllFunction {
  ENABLE_SUBPIXEL,
  ENABLE_EMBOLDEN,
  ENABLE_LINEAR_METRICS,
  SET_SIZE,
  GET_SIZE,
  SET_TYPEFACE,
  GET_TYPEFACE,
  GET_METRICS,
  MEASURE_TEXT,
  MEASURE_SINGLE_CHARACTER,
  SET_SCALE_X,
  GET_SCALE_X,
  SET_SKEW_X,
  GET_SKEW_X,
  SET_EDGING,
  GET_EDGING,
  SET_HINTING,
  GET_HINTING,
  COUNT_TEXT,
  SET_BASELINE_SNAP,
  IS_BASELINE_SNAP,
  SET_EMBEDDED_BITMAPS,
  IS_EMBEDDED_BITMAPS,
  SET_FORCE_AUTO_HINTING,
  IS_FORCE_AUTO_HINTING,
  GET_WIDTHS,
  TEXT_TO_GLYPHS,
  IS_SUBPIXEL,
  IS_EMBOLDEN,
  IS_LINEAR_METRICS,
  MAX
}

enum FontOperationFunction {
  GET_SIZE,
  GET_TYPEFACE,
  GET_METRICS,
  MEASURE_TEXT,
  MEASURE_SINGLE_CHARACTER,
  GET_SCALE_X,
  GET_SKEW_X,
  GET_EDGING,
  GET_HINTING,
  COUNT_TEXT,
  IS_BASELINE_SNAP,
  IS_EMBEDDED_BITMAPS,
  IS_FORCE_AUTO_HINTING,
  GET_WIDTHS,
  TEXT_TO_GLYPHS,
  IS_SUBPIXEL,
  IS_EMBOLDEN,
  IS_LINEAR_METRICS,
  MAX
}

export class StabilityFontNew extends TestBase {

  /*
  @CaseID:SUB_BASIC_GRAPHICS_SPECIAL_STABLE_TS_DRAWING_FONT_0100
  @Description:Font new-置空循环
  @Step:
  1、运行脚本，循环执行hidumper命令查询内存
  2、执行自动化用例：
  font new-置空，循环调用1000次
  3、执行结束后等待3分钟
  4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
  @Result:
  1、程序运行正常，无crash
  2、内存平稳，没有持续增长，执行后内存回落到执行前；
  */
  public async OnTestStability(canvas: drawing.Canvas) {
    console.log("FontNew OnTestStability")
    for (let i = 0; i < this.testCount_; i++) {
      let font = new drawing.Font();
    }
  }
}

export class StabilityFontAllInvoke extends TestBase {
  private index = 0;
  private typeface = new drawing.Typeface();
  private value = [OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.height_)];
  static functions_: Map<FontAllFunction, (FontAllInvoke: StabilityFontAllInvoke, font: drawing.Font) => void> = new Map([
    [FontAllFunction.ENABLE_SUBPIXEL, (fontAllInvoke: StabilityFontAllInvoke, font: drawing.Font) => {
      fontAllInvoke.FontEnableSubpixelTest(font)
    }],
    [FontAllFunction.ENABLE_EMBOLDEN, (fontAllInvoke: StabilityFontAllInvoke, font: drawing.Font) => {
      fontAllInvoke.FontEnableEmboldenTest(font)
    }],
    [FontAllFunction.ENABLE_LINEAR_METRICS, (fontAllInvoke: StabilityFontAllInvoke, font: drawing.Font) => {
      fontAllInvoke.FontEnableLinearMetricsTest(font)
    }],
    [FontAllFunction.SET_SIZE, (fontAllInvoke: StabilityFontAllInvoke, font: drawing.Font) => {
      fontAllInvoke.FontSetSizeTest(font)
    }],
    [FontAllFunction.GET_SIZE, (fontAllInvoke: StabilityFontAllInvoke, font: drawing.Font) => {
      fontAllInvoke.FontGetSizeTest(font)
    }],
    [FontAllFunction.SET_TYPEFACE, (fontAllInvoke: StabilityFontAllInvoke, font: drawing.Font) => {
      fontAllInvoke.FontSetTypefaceTest(font)
    }],
    [FontAllFunction.GET_TYPEFACE, (fontAllInvoke: StabilityFontAllInvoke, font: drawing.Font) => {
      fontAllInvoke.FontGetTypefaceTest(font)
    }],
    [FontAllFunction.GET_METRICS, (fontAllInvoke: StabilityFontAllInvoke, font: drawing.Font) => {
      fontAllInvoke.FontGetMetricsTest(font)
    }],
    [FontAllFunction.MEASURE_TEXT, (fontAllInvoke: StabilityFontAllInvoke, font: drawing.Font) => {
      fontAllInvoke.FontMeasureTextTest(font)
    }],
    [FontAllFunction.MEASURE_SINGLE_CHARACTER, (fontAllInvoke: StabilityFontAllInvoke, font: drawing.Font) => {
      fontAllInvoke.FontMeasureSingleCharacterTest(font)
    }],
    [FontAllFunction.SET_SCALE_X, (fontAllInvoke: StabilityFontAllInvoke, font: drawing.Font) => {
      fontAllInvoke.FontSetScaleXTest(font)
    }],
    [FontAllFunction.GET_SCALE_X, (fontAllInvoke: StabilityFontAllInvoke, font: drawing.Font) => {
      fontAllInvoke.FontGetScaleXTest(font)
    }],
    [FontAllFunction.SET_SKEW_X, (fontAllInvoke: StabilityFontAllInvoke, font: drawing.Font) => {
      fontAllInvoke.FontSetSkewXTest(font)
    }],
    [FontAllFunction.GET_SKEW_X, (fontAllInvoke: StabilityFontAllInvoke, font: drawing.Font) => {
      fontAllInvoke.FontGetSkewXTest(font)
    }],
    [FontAllFunction.SET_EDGING, (fontAllInvoke: StabilityFontAllInvoke, font: drawing.Font) => {
      fontAllInvoke.FontSetEdgingTest(font)
    }],
    [FontAllFunction.GET_EDGING, (fontAllInvoke: StabilityFontAllInvoke, font: drawing.Font) => {
      fontAllInvoke.FontGetEdgingTest(font)
    }],
    [FontAllFunction.SET_HINTING, (fontAllInvoke: StabilityFontAllInvoke, font: drawing.Font) => {
      fontAllInvoke.FontSetHintingTest(font)
    }],
    [FontAllFunction.GET_HINTING, (fontAllInvoke: StabilityFontAllInvoke, font: drawing.Font) => {
      fontAllInvoke.FontGetHintingTest(font)
    }],
    [FontAllFunction.COUNT_TEXT, (fontAllInvoke: StabilityFontAllInvoke, font: drawing.Font) => {
      fontAllInvoke.FontCountTextTest(font)
    }],
    [FontAllFunction.SET_BASELINE_SNAP, (fontAllInvoke: StabilityFontAllInvoke, font: drawing.Font) => {
      fontAllInvoke.FontSetBaselineSnapTest(font)
    }],
    [FontAllFunction.IS_BASELINE_SNAP, (fontAllInvoke: StabilityFontAllInvoke, font: drawing.Font) => {
      fontAllInvoke.FontIsBaselineSnapTest(font)
    }],
    [FontAllFunction.SET_EMBEDDED_BITMAPS, (fontAllInvoke: StabilityFontAllInvoke, font: drawing.Font) => {
      fontAllInvoke.FontSetEmbeddedBitmapsTest(font)
    }],
    [FontAllFunction.IS_EMBEDDED_BITMAPS, (fontAllInvoke: StabilityFontAllInvoke, font: drawing.Font) => {
      fontAllInvoke.FontIsEmbeddedBitmapsTest(font)
    }],
    [FontAllFunction.SET_FORCE_AUTO_HINTING, (fontAllInvoke: StabilityFontAllInvoke, font: drawing.Font) => {
      fontAllInvoke.FontSetForceAutoHintingTest(font)
    }],
    [FontAllFunction.IS_FORCE_AUTO_HINTING, (fontAllInvoke: StabilityFontAllInvoke, font: drawing.Font) => {
      fontAllInvoke.FontIsForceAutoHintingTest(font)
    }],
    [FontAllFunction.GET_WIDTHS, (fontAllInvoke: StabilityFontAllInvoke, font: drawing.Font) => {
      fontAllInvoke.FontGetWidthsTest(font)
    }],
    [FontAllFunction.TEXT_TO_GLYPHS, (fontAllInvoke: StabilityFontAllInvoke, font: drawing.Font) => {
      fontAllInvoke.FontTextToGlyphsTest(font)
    }],
    [FontAllFunction.IS_SUBPIXEL, (fontAllInvoke: StabilityFontAllInvoke, font: drawing.Font) => {
      fontAllInvoke.FontIsSubpixelTest(font)
    }],
    [FontAllFunction.IS_EMBOLDEN, (fontAllInvoke: StabilityFontAllInvoke, font: drawing.Font) => {
      fontAllInvoke.FontIsEmboldenTest(font)
    }],
    [FontAllFunction.IS_LINEAR_METRICS, (fontAllInvoke: StabilityFontAllInvoke, font: drawing.Font) => {
      fontAllInvoke.FontIsLinearMetricsTest(font)
    }],
  ]);

  /*
  @CaseID:SUB_BASIC_GRAPHICS_SPECIAL_STABLE_TS_DRAWING_FONT_0200
  @Description:Font new-相关接口全调用-置空循环
  @Step:
  1、运行脚本，循环执行hidumper命令查询内存
  2、执行自动化用例：
  new font-相关接口全调用-置空，循环调用1000次
  3、执行结束后等待3分钟
  4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
  @Result:
  1、程序运行正常，无crash
  2、内存平稳，没有持续增长，执行后内存回落到执行前；
  */
  public async OnTestStability(canvas: drawing.Canvas) {
    console.log("FontAllInvoke OnTestStability");
    for (let i = 0; i < this.testCount_; i++) {
      this.index = i;
      for (let j = 0; j < FontAllFunction.MAX; j++) {
        let font = new drawing.Font();
        StabilityFontAllInvoke.functions_.get(j)(this, font);
      }
    }
  }

  FontEnableSubpixelTest(font: drawing.Font) {
    font.enableSubpixel(this.index % 2 == 0);
  }

  FontEnableEmboldenTest(font: drawing.Font) {
    font.enableEmbolden(this.index % 2 == 0);
  }

  FontEnableLinearMetricsTest(font: drawing.Font) {
    font.enableLinearMetrics(this.index % 2 == 0);
  }

  FontSetSizeTest(font: drawing.Font) {
    font.setSize(OHRandom.GetRandomNum(0, this.width_));
  }

  FontGetSizeTest(font: drawing.Font) {
    font.getSize();
  }

  FontSetTypefaceTest(font: drawing.Font) {
    font.setTypeface(this.typeface);
  }

  FontGetTypefaceTest(font: drawing.Font) {
    font.getTypeface();
  }

  FontGetMetricsTest(font: drawing.Font) {
    font.getMetrics();
  }

  FontMeasureTextTest(font: drawing.Font) {
    font.measureText("drawing", drawing.TextEncoding.TEXT_ENCODING_UTF8);
  }

  FontMeasureSingleCharacterTest(font: drawing.Font) {
    font.measureSingleCharacter("A");
  }

  FontSetScaleXTest(font: drawing.Font) {
    font.setScaleX(OHRandom.GetRandomNum(0, this.width_));
  }

  FontGetScaleXTest(font: drawing.Font) {
    font.getScaleX();
  }

  FontSetSkewXTest(font: drawing.Font) {
    font.setSkewX(OHRandom.GetRandomNum(0, this.width_));
  }

  FontGetSkewXTest(font: drawing.Font) {
    font.getSkewX();
  }

  FontSetEdgingTest(font: drawing.Font) {
    font.setEdging(this.index % 3);
  }

  FontGetEdgingTest(font: drawing.Font) {
    font.getEdging();
  }

  FontSetHintingTest(font: drawing.Font) {
    font.setHinting(this.index % 4);
  }

  FontGetHintingTest(font: drawing.Font) {
    font.getEdging();
  }

  FontCountTextTest(font: drawing.Font) {
    font.countText("text");
  }

  FontSetBaselineSnapTest(font: drawing.Font) {
    font.setBaselineSnap(this.index % 2 == 0);
  }

  FontIsBaselineSnapTest(font: drawing.Font) {
    font.isBaselineSnap();
  }

  FontSetEmbeddedBitmapsTest(font: drawing.Font) {
    font.setEmbeddedBitmaps(this.index % 2 == 0);
  }

  FontIsEmbeddedBitmapsTest(font: drawing.Font) {
    font.isEmbeddedBitmaps();
  }

  FontSetForceAutoHintingTest(font: drawing.Font) {
    font.setForceAutoHinting(this.index % 2 == 0);
  }

  FontIsForceAutoHintingTest(font: drawing.Font) {
    font.isForceAutoHinting();
  }

  FontGetWidthsTest(font: drawing.Font) {
    font.getWidths(this.value);
  }

  FontTextToGlyphsTest(font: drawing.Font) {
    font.textToGlyphs("text");
  }

  FontIsSubpixelTest(font: drawing.Font) {
    font.isSubpixel();
  }

  FontIsEmboldenTest(font: drawing.Font) {
    font.isEmbolden();
  }

  FontIsLinearMetricsTest(font: drawing.Font) {
    font.isLinearMetrics();
  }
}

export class StabilityFontRandInvoke extends TestBase {
  private index = 0;
  private value = [OHRandom.GetRandomNum(0, this.width_), OHRandom.GetRandomNum(0, this.height_)];
  static functions_: Map<FontOperationFunction, (FontRandInvoke: StabilityFontRandInvoke, font: drawing.Font) => void> =
    new Map([
      [FontOperationFunction.GET_SIZE, (fontRandInvoke: StabilityFontRandInvoke, font: drawing.Font) => {
        fontRandInvoke.FontGetSizeTest(font)
      }],
      [FontOperationFunction.GET_TYPEFACE, (fontRandInvoke: StabilityFontRandInvoke, font: drawing.Font) => {
        fontRandInvoke.FontGetTypefaceTest(font)
      }],
      [FontOperationFunction.GET_METRICS, (fontRandInvoke: StabilityFontRandInvoke, font: drawing.Font) => {
        fontRandInvoke.FontGetMetricsTest(font)
      }],
      [FontOperationFunction.MEASURE_TEXT, (fontRandInvoke: StabilityFontRandInvoke, font: drawing.Font) => {
        fontRandInvoke.FontMeasureTextTest(font)
      }],
      [FontOperationFunction.MEASURE_SINGLE_CHARACTER, (fontRandInvoke: StabilityFontRandInvoke, font: drawing.Font) => {
        fontRandInvoke.FontMeasureSingleCharacterTest(font)
      }],
      [FontOperationFunction.GET_SCALE_X, (fontRandInvoke: StabilityFontRandInvoke, font: drawing.Font) => {
        fontRandInvoke.FontGetScaleXTest(font)
      }],
      [FontOperationFunction.GET_SKEW_X, (fontRandInvoke: StabilityFontRandInvoke, font: drawing.Font) => {
        fontRandInvoke.FontGetSkewXTest(font)
      }],
      [FontOperationFunction.GET_EDGING, (fontRandInvoke: StabilityFontRandInvoke, font: drawing.Font) => {
        fontRandInvoke.FontGetEdgingTest(font)
      }],
      [FontOperationFunction.GET_HINTING, (fontRandInvoke: StabilityFontRandInvoke, font: drawing.Font) => {
        fontRandInvoke.FontGetHintingTest(font)
      }],
      [FontOperationFunction.COUNT_TEXT, (fontRandInvoke: StabilityFontRandInvoke, font: drawing.Font) => {
        fontRandInvoke.FontCountTextTest(font)
      }],
      [FontOperationFunction.IS_BASELINE_SNAP, (fontRandInvoke: StabilityFontRandInvoke, font: drawing.Font) => {
        fontRandInvoke.FontIsBaselineSnapTest(font)
      }],
      [FontOperationFunction.IS_EMBEDDED_BITMAPS, (fontRandInvoke: StabilityFontRandInvoke, font: drawing.Font) => {
        fontRandInvoke.FontIsEmbeddedBitmapsTest(font)
      }],
      [FontOperationFunction.IS_FORCE_AUTO_HINTING, (fontRandInvoke: StabilityFontRandInvoke, font: drawing.Font) => {
        fontRandInvoke.FontIsForceAutoHintingTest(font)
      }],
      [FontOperationFunction.GET_WIDTHS, (fontRandInvoke: StabilityFontRandInvoke, font: drawing.Font) => {
        fontRandInvoke.FontGetWidthsTest(font)
      }],
      [FontOperationFunction.TEXT_TO_GLYPHS, (fontRandInvoke: StabilityFontRandInvoke, font: drawing.Font) => {
        fontRandInvoke.FontTextToGlyphsTest(font)
      }],
      [FontOperationFunction.IS_SUBPIXEL, (fontRandInvoke: StabilityFontRandInvoke, font: drawing.Font) => {
        fontRandInvoke.FontIsSubpixelTest(font)
      }],
      [FontOperationFunction.IS_EMBOLDEN, (fontRandInvoke: StabilityFontRandInvoke, font: drawing.Font) => {
        fontRandInvoke.FontIsEmboldenTest(font)
      }],
      [FontOperationFunction.IS_LINEAR_METRICS, (fontRandInvoke: StabilityFontRandInvoke, font: drawing.Font) => {
        fontRandInvoke.FontIsLinearMetricsTest(font)
      }],
    ])

  /*
  @CaseID:SUB_BASIC_GRAPHICS_SPECIAL_STABLE_TS_DRAWING_FONT_0300
  @Description:Font相关操作接口随机循环调用
  @Step:
  1、运行脚本，循环执行hidumper命令查询内存
  2、执行自动化用例：
  new font
  font相关操作接口调用，循环调用1000次
  font置空
  3、执行结束后等待3分钟
  4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
  @Result:
  1、程序运行正常，无crash
  2、内存平稳，没有持续增长，执行后内存回落到执行前；
  */
  public async OnTestStability(canvas: drawing.Canvas) {
    console.log("FontRandInvoke OnTestStability");
    let font = new drawing.Font();
    for (let i = 0; i < this.testCount_; i++) {
      StabilityFontRandInvoke.functions_.get(OHRandom.GetRandomNum(0, FontOperationFunction.MAX -1))(this, font);
    }
  }

  FontSetSizeTest(font: drawing.Font) {
    font.setSize(OHRandom.GetRandomNum(0, this.width_));
  }

  FontGetSizeTest(font: drawing.Font) {
    font.getSize();
  }

  FontGetTypefaceTest(font: drawing.Font) {
    font.getTypeface();
  }

  FontGetMetricsTest(font: drawing.Font) {
    font.getMetrics();
  }

  FontMeasureTextTest(font: drawing.Font) {
    font.measureText("drawing", drawing.TextEncoding.TEXT_ENCODING_UTF8);
  }

  FontMeasureSingleCharacterTest(font: drawing.Font) {
    font.measureSingleCharacter("A");
  }

  FontGetScaleXTest(font: drawing.Font) {
    font.getScaleX();
  }

  FontGetSkewXTest(font: drawing.Font) {
    font.getSkewX();
  }

  FontGetEdgingTest(font: drawing.Font) {
    font.getEdging();
  }

  FontGetHintingTest(font: drawing.Font) {
    font.getHinting();
  }

  FontCountTextTest(font: drawing.Font) {
    font.countText("text");
  }

  FontIsBaselineSnapTest(font: drawing.Font) {
    font.isBaselineSnap();
  }

  FontIsEmbeddedBitmapsTest(font: drawing.Font) {
    font.isEmbeddedBitmaps();
  }

  FontSetForceAutoHintingTest(font: drawing.Font) {
    font.setForceAutoHinting(this.index % 2 == 0);
  }

  FontIsForceAutoHintingTest(font: drawing.Font) {
    font.isForceAutoHinting();
  }

  FontGetWidthsTest(font: drawing.Font) {
    font.getWidths(this.value);
  }

  FontTextToGlyphsTest(font: drawing.Font) {
    font.textToGlyphs("text");
  }

  FontIsSubpixelTest(font: drawing.Font) {
    font.isSubpixel();
  }

  FontIsEmboldenTest(font: drawing.Font) {
    font.isEmbolden();
  }

  FontIsLinearMetricsTest(font: drawing.Font) {
    font.isLinearMetrics();
  }
}