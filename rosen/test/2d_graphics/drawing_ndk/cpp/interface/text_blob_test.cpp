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

#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_filter.h>
#include <native_drawing/drawing_font.h>
#include <native_drawing/drawing_image.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_sampling_options.h>
#include <native_drawing/drawing_shader_effect.h>
#include <native_drawing/drawing_text_blob.h>

#include "test_common.h"

#include "common/log_common.h"

void TextBlobBuilderCreate::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    const int count = 9; // 9 表示要创建的文本块中字符的数量
    uint16_t glyphs[9] = { 65, 227, 283, 283, 299, 2, 94, 37,
        84 }; // 65, 227, 283, 283, 299, 2, 94, 37, 84  这些数字是Unicode字符编码，代表要绘制的字符
    float posX[9] = { 0, 14.9 * 2, 25.84 * 2, 30.62 * 2, 35.4 * 2, 47.22 * 2, 52.62 * 2, 67.42 * 2,
        81.7 * 2 }; // 0, 14.9 * 2, 25.84 * 2, 30.62 * 2, 35.4 * 2, 47.22 * 2, 52.62 * 2, 67.42 * 2, 81.7 * 2
                    // 这些数字定义了每个字符在文本块中的位置
    float posY[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 }; //  0, 0, 0, 0, 0, 0, 0, 0, 0 这些数字定义了每个字符在文本块中的位置
    TestRend rand;
    float l;
    float t;
    float r;
    float b;
    for (int i = 0; i < testCount_; i++) {
        l = rand.nextULessThan(bitmapWidth_);
        t = rand.nextULessThan(bitmapHeight_);
        r = l + rand.nextULessThan(bitmapWidth_);
        b = t + rand.nextULessThan(bitmapHeight_);
        OH_Drawing_TextBlobBuilder* builder = OH_Drawing_TextBlobBuilderCreate();
        OH_Drawing_Rect* rect = OH_Drawing_RectCreate(l, t, r, b);
        const OH_Drawing_RunBuffer* buffer = OH_Drawing_TextBlobBuilderAllocRunPos(builder, font, count, rect);
        for (int idx = 0; idx < count; idx++) {
            buffer->glyphs[idx] = glyphs[idx];
            buffer->pos[idx * 2] = posX[idx];     // 2  用于位置计算
            buffer->pos[idx * 2 + 1] = posY[idx]; // 2,1  用于位置计算
        }
        OH_Drawing_TextBlob* blob = OH_Drawing_TextBlobBuilderMake(builder);
        OH_Drawing_CanvasDrawTextBlob(
            canvas, blob, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
        OH_Drawing_TextBlobBuilderDestroy(builder);
        OH_Drawing_RectDestroy(rect);
        OH_Drawing_TextBlobDestroy(blob);
    }
    OH_Drawing_FontDestroy(font);
}

void TextBlobCreateFromText::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    std::string text = "TextBlobCreateFromText";
    int len = text.length();
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    TestRend rand;
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_TextBlob* blob = OH_Drawing_TextBlobCreateFromText(text.c_str(), len, font, TEXT_ENCODING_UTF8);
        OH_Drawing_CanvasDrawTextBlob(
            canvas, blob, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
        OH_Drawing_TextBlobDestroy(blob);
    }
    OH_Drawing_FontDestroy(font);
}

void TextBlobCreateFromPosText::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    std::string text = "TextBlobCreateFromPosText";
    int len = text.length();
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    TestRend rand;
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_Point2D pt;
        pt.x = rand.nextULessThan(bitmapWidth_);
        pt.y = rand.nextULessThan(bitmapHeight_);
        OH_Drawing_TextBlob* blob =
            OH_Drawing_TextBlobCreateFromPosText(text.c_str(), len, &pt, font, TEXT_ENCODING_UTF8);
        OH_Drawing_CanvasDrawTextBlob(
            canvas, blob, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
        OH_Drawing_TextBlobDestroy(blob);
    }
    OH_Drawing_FontDestroy(font);
}

void TextBlobCreateFromString::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    std::string text = "TextBlobCreateFromString";
    int len = text.length();
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    TestRend rand;
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_TextBlob* blob = OH_Drawing_TextBlobCreateFromString(text.c_str(), font, TEXT_ENCODING_UTF8);
        OH_Drawing_CanvasDrawTextBlob(
            canvas, blob, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
        OH_Drawing_TextBlobDestroy(blob);
    }
    OH_Drawing_FontDestroy(font);
}

void TextBlobGetBounds::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    std::string text = "TextBlobGetBounds";
    int len = text.length();
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    TestRend rand;
    OH_Drawing_TextBlob* blob = OH_Drawing_TextBlobCreateFromString(text.c_str(), font, TEXT_ENCODING_UTF8);
    OH_Drawing_CanvasDrawTextBlob(canvas, blob, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
    OH_Drawing_Rect* r = OH_Drawing_RectCreate(0, 0, 0, 0); // 0, 0, 0, 0  这些数字用于创建矩形
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_TextBlobGetBounds(blob, r);
    }
    DRAWING_LOGI("TextBlobGetBounds r = { %{public}f,%{public}f,%{public}f,%{public}f}", OH_Drawing_RectGetLeft(r),
        OH_Drawing_RectGetTop(r), OH_Drawing_RectGetRight(r), OH_Drawing_RectGetBottom(r));
    OH_Drawing_RectDestroy(r);
    OH_Drawing_TextBlobDestroy(blob);
    OH_Drawing_FontDestroy(font);
}
void TextBlobBuilderAllocRunPos::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    TestRend rand;
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    const int count = 9;
    OH_Drawing_TextBlobBuilder* builder = OH_Drawing_TextBlobBuilderCreate();
    uint16_t glyphs[9] = { 65, 227, 283, 283, 299, 2, 94, 37, 84 }; // 65, 227, 283, 283, 299, 2, 94, 37, 84字形符号数组
    float posX[9] = { 0, 14.9 * 2, 25.84 * 2, 30.62 * 2, 35.4 * 2, 47.22 * 2, 52.62 * 2, 67.42 * 2,
        81.7 * 2 }; // 14.9 * 2, 25.84 * 2, 30.62 * 2, 35.4 * 2, 47.22 * 2, 52.62 * 2, 67.42 * 2, 81.7 * 2 位置数组
    float posY[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 }; //
    const OH_Drawing_RunBuffer* buffer;

    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(10, 10, 300, 300); // 10, 10, 300, 300 创建矩形
    for (int i = 0; i < testCount_; i++) {
        buffer = OH_Drawing_TextBlobBuilderAllocRunPos(builder, font, count, rect);
    }
    for (int idx = 0; idx < count; idx++) {
        buffer->glyphs[idx] = glyphs[idx];
        buffer->pos[idx * 2] = posX[idx];     // 2 设置字形位置
        buffer->pos[idx * 2 + 1] = posY[idx]; // 2 1 设置字形位置
    }
    OH_Drawing_TextBlob* blob = OH_Drawing_TextBlobBuilderMake(builder);
    OH_Drawing_CanvasDrawTextBlob(canvas, blob, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));

    OH_Drawing_RectDestroy(rect);
    OH_Drawing_TextBlobDestroy(blob);
    OH_Drawing_TextBlobBuilderDestroy(builder);
    OH_Drawing_FontDestroy(font);
}