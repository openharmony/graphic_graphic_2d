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

#include "text_blob_test.h"

#include <cstdint>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_filter.h>
#include <native_drawing/drawing_font.h>
#include <native_drawing/drawing_image.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_path_effect.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_region.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_sampling_options.h>
#include <native_drawing/drawing_shader_effect.h>
#include <native_drawing/drawing_shadow_layer.h>
#include <native_drawing/drawing_text_blob.h>
#include <native_drawing/drawing_types.h>

#include "test_common.h"

#include "common/log_common.h"

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_TEXT_BLOB_0100
@Description:TextBlobBuilderCreate-TextBlobBuilderMake-TextBlobDestroy-TextBlobBuilderDestroy循环
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2、TextBlob、TextBlobBuilder创建和销毁操作接口循环调用
    3、执行结束后等待3分钟
    4、结束脚本运行，抓取的内存值转成折线图，观察内存变换
@Result:
    1、程序运行正常，无crash
    2、内存平稳，没有持续增长，执行后内存回落的到执行前；
 */
void StabilityTextBlobBuilderCreate::OnTestStability(OH_Drawing_Canvas* canvas)
{
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_TextBlobBuilder* textBlobBuilder = OH_Drawing_TextBlobBuilderCreate();
        OH_Drawing_TextBlob* textBlob = OH_Drawing_TextBlobBuilderMake(textBlobBuilder);
        OH_Drawing_TextBlobDestroy(textBlob);
        OH_Drawing_TextBlobBuilderDestroy(textBlobBuilder);
    }
}

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_TEXT_BLOB_0200
@Description:TextBlobCreateFromText-TextBlobDestroy循环
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2. 创建Font
    3、TextBlob创建和销毁操作接口循环调用
    4、销毁Font
    5、执行结束后等待3分钟
    6、结束脚本运行，抓取的内存值转成折线图，观察内存变换
@Result:
    1、程序运行正常，无crash
    2、内存平稳，没有持续增长，执行后内存回落的到执行前；
 */
void StabilityTextBlobCreateFromText::OnTestStability(OH_Drawing_Canvas* canvas)
{
    const char* text = "hello%&(*$#&**(*(";
    size_t len = strlen(text);
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_TextBlob* textblob = OH_Drawing_TextBlobCreateFromText(
            text, len, font, DrawGetEnum(TEXT_ENCODING_UTF8, TEXT_ENCODING_GLYPH_ID, i));
        OH_Drawing_TextBlobDestroy(textblob);
    }
    OH_Drawing_FontDestroy(font);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_TEXT_BLOB_0400
@Description:TextBlobCreateFromString-TextBlobDestroy循环
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2. 创建Font
    3、TextBlob创建和销毁操作接口循环调用
    4、销毁Font
    5、执行结束后等待3分钟
    6、结束脚本运行，抓取的内存值转成折线图，观察内存变换
@Result:
    1、程序运行正常，无crash
    2、内存平稳，没有持续增长，执行后内存回落的到执行前；
 */
void StabilityTextBlobCreateFromString::OnTestStability(OH_Drawing_Canvas* canvas)
{
    const char* text = "hello%&(*$#&**(*(";
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_TextBlob* textblob =
            OH_Drawing_TextBlobCreateFromString(text, font, DrawGetEnum(TEXT_ENCODING_UTF8, TEXT_ENCODING_GLYPH_ID, i));
        OH_Drawing_TextBlobDestroy(textblob);
    }
    OH_Drawing_FontDestroy(font);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_TEXT_BLOB_0300
@Description:TextBlobCreateFromPosText-TextBlobDestroy循环
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2. 创建Font
    3、TextBlob创建和销毁操作接口循环调用
    4、销毁Font
    5、执行结束后等待3分钟
    6、结束脚本运行，抓取的内存值转成折线图，观察内存变换
@Result:
    1、程序运行正常，无crash
    2、内存平稳，没有持续增长，执行后内存回落的到执行前；
 */
void StabilityTextBlobCreateFromPosText::OnTestStability(OH_Drawing_Canvas* canvas)
{
    TestRend rand;
    const char* text = "hello%&(*$#&**(*(";
    size_t len = strlen(text);
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_Point2D pos = { rand.nextRangeF(0, bitmapWidth_), rand.nextRangeF(0, bitmapHeight_) };
        OH_Drawing_TextBlob* textblob = OH_Drawing_TextBlobCreateFromPosText(
            text, len, &pos, font, DrawGetEnum(TEXT_ENCODING_UTF8, TEXT_ENCODING_GLYPH_ID, i));
        OH_Drawing_TextBlobDestroy(textblob);
    }
    OH_Drawing_FontDestroy(font);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_TEXT_BLOB_0500
@Description:TextBlobBuilderAllocRunPos-TextBlobBuilderDestroy循环
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2. 创建Font和Rect
    3、TextBlob相关操作接口循环调用
    4、销毁Font和Rect
    5、执行结束后等待3分钟
    6、结束脚本运行，抓取的内存值转成折线图，观察内存变换
@Result:
    1、程序运行正常，无crash
    2、内存平稳，没有持续增长，执行后内存回落的到执行前；
 */
void StabilityTextBlobBuilderAllocRunPos::OnTestStability(OH_Drawing_Canvas* canvas)
{
    TestRend rand;
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapWidth_),
        rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapWidth_));
    for (int i = 0; i < testCount_; i++) {
        int32_t count = (int32_t)rand.nextULessThan(bitmapHeight_);
        OH_Drawing_TextBlobBuilder* textBlobBuilder = OH_Drawing_TextBlobBuilderCreate();
        const OH_Drawing_RunBuffer* buffer = OH_Drawing_TextBlobBuilderAllocRunPos(textBlobBuilder, font, count, rect);
        OH_Drawing_TextBlobBuilderDestroy(textBlobBuilder);
    }
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_FontDestroy(font);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_TEXT_BLOB_0600
@Description:TextBlobCreate-相关配置接口全调用-TextBlobDestroy循环
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2. 创建Rect和TextBlob
    3、TextBlobGetBounds和OH_Drawing_TextBlobUniqueID相关操作接口循环调用
    4、销毁RectTextBlob
    5、执行结束后等待3分钟
    6、结束脚本运行，抓取的内存值转成折线图，观察内存变换
@Result:
    1、程序运行正常，无crash
    2、内存平稳，没有持续增长，执行后内存回落的到执行前；
 */
void StabilityTextBlobRandomFunc::OnTestStability(OH_Drawing_Canvas* canvas)
{
    TestRend rand;
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    const char* text = "hello%&(*$#&**(*(你好";
    size_t len = strlen(text);
    OH_Drawing_Rect* bound = OH_Drawing_RectCreate(rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapWidth_),
        rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapWidth_));
    OH_Drawing_TextBlob* textblob = OH_Drawing_TextBlobCreateFromText(
        text, len, font, DrawGetEnum(TEXT_ENCODING_UTF8, TEXT_ENCODING_GLYPH_ID, rand.nextU()));
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_TextBlobGetBounds(textblob, bound);
        OH_Drawing_TextBlobUniqueID(textblob);
    }
    OH_Drawing_TextBlobDestroy(textblob);
    OH_Drawing_RectDestroy(bound);
    OH_Drawing_FontDestroy(font);
}