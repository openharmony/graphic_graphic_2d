/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "font_measure_text_test.h"

#include <cstddef>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_color_filter.h>
#include <native_drawing/drawing_filter.h>
#include <native_drawing/drawing_font.h>
#include <native_drawing/drawing_image.h>
#include <native_drawing/drawing_mask_filter.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_path_effect.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_region.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_sampling_options.h>
#include <native_drawing/drawing_text_blob.h>

#include "test_common.h"

#include "common/log_common.h"
typedef struct {
    const char* text;
    OH_Drawing_TextEncoding textEncoding;
    OH_Drawing_Rect* bounds;
    float textWidth;
    float x;
    float y;
} TextBlobTest;

static void DrawTextBlob(OH_Drawing_Canvas* canvas, TextBlobTest textBlobTest)
{
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    OH_Drawing_FontSetTextSize(font, 50); // 50 for test
    OH_Drawing_FontMeasureText(font, textBlobTest.text, strlen(textBlobTest.text), textBlobTest.textEncoding,
        textBlobTest.bounds, &textBlobTest.textWidth);
    DRAWING_LOGI("textwidth,boundsh,boundsw,%{public}f,%{public}f,%{public}f", textBlobTest.textWidth,
        OH_Drawing_RectGetHeight(textBlobTest.bounds), OH_Drawing_RectGetWidth(textBlobTest.bounds));
    OH_Drawing_TextBlob* textblob = OH_Drawing_TextBlobCreateFromText(textBlobTest.text, strlen(textBlobTest.text),
        font, TEXT_ENCODING_UTF8);
    OH_Drawing_CanvasDrawTextBlob(canvas, textblob, textBlobTest.x, textBlobTest.y);
    OH_Drawing_FontDestroy(font);
    OH_Drawing_TextBlobDestroy(textblob);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_FONT_2901
@Description:
    1、OH_Drawing_FontMeasureText接口正常入参验证,第五个参数传空
    2、OH_Drawing_FontMeasureText接口正常入参验证,第四个参数枚举值各种情况
    3、OH_Drawing_FontMeasureText接口正常入参验证,第五个参数边界框设置小于文本大小
    4、OH_Drawing_FontMeasureText接口正常入参验证，text传入特殊字符
    5、OH_Drawing_FontMeasureText接口正常入参验证，text传入中文
@Step:
    1、demo中第二个选择框选择（cpu-xcomponent、gpu-xcomponent、gpu-xnode）Testcase中输入functionFontMeasureText，点击draw按钮
@Result:
    1、具体效果可参考备注中的文件
 */
void FunctionFontMeasureText::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("FunctionFontMeasureText OnTestFunction");
    const char* text = "hello world";
    // OH_Drawing_FontMeasureText接口正常入参验证,第五个参数传空
    DrawTextBlob(canvas, {text, TEXT_ENCODING_UTF8, nullptr, 50, 100, 100}); // 50, 100, 100 fot test
    // OH_Drawing_FontMeasureText接口正常入参验证,第四个参数枚举值各种情况
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(0, 0, 400, 400);          // 0, 0, 400, 400 fot test
    DrawTextBlob(canvas, {text, TEXT_ENCODING_UTF8, rect, 50, 100, 150});     // 50, 100, 150 fot test
    DrawTextBlob(canvas, {text, TEXT_ENCODING_UTF16, rect, 50, 100, 200});    // 50, 100, 200 fot test
    DrawTextBlob(canvas, {text, TEXT_ENCODING_UTF32, rect, 50, 100, 250});    // 50, 100, 250 fot test
    DrawTextBlob(canvas, {text, TEXT_ENCODING_GLYPH_ID, rect, 50, 100, 300}); // 50, 100, 300 fot test
    // OH_Drawing_FontMeasureText接口正常入参验证,第五个参数边界框设置小于文本大小
    rect = OH_Drawing_RectCreate(0, 0, 50, 50);                         // 0, 0, 50, 50 fot test
    DrawTextBlob(canvas, {text, TEXT_ENCODING_UTF8, rect, 80, 100, 350}); // 80, 100, 350 fot test
    // OH_Drawing_FontMeasureText接口正常入参验证，text传入特殊字符
    text = "$^*)^%)*^@%&^*";
    DrawTextBlob(canvas, {text, TEXT_ENCODING_UTF8, rect, 50, 100, 400}); // 50, 100, 400 fot test
    // OH_Drawing_FontMeasureText接口正常入参验证，text传入中文
    text = "中文";
    DrawTextBlob(canvas, {text, TEXT_ENCODING_UTF8, rect, 50, 100, 450}); // 50, 100, 450 fot test
    OH_Drawing_RectDestroy(rect);
}
