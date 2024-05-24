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

#include "typeface_test.h"

#include <bits/alltypes.h>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_filter.h>
#include <native_drawing/drawing_font.h>
#include <native_drawing/drawing_image.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_memory_stream.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_path_effect.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_sampling_options.h>
#include <native_drawing/drawing_shader_effect.h>
#include <native_drawing/drawing_text_blob.h>
#include <native_drawing/drawing_typeface.h>

#include "test_common.h"
#include "typeface_test.h"

#include "common/log_common.h"

const float_t NUM_50 = (50);
const float_t NUM_100 = (100);
const float_t NUM_200 = (200);

void TypefaceCreateDefault::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    const char* text = "hello";
    size_t len = strlen(text);
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    OH_Drawing_Typeface* typeface;
    OH_Drawing_FontSetTextSize(font, NUM_50);
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    for (int i = 0; i < testCount_; i++) {
        typeface = OH_Drawing_TypefaceCreateDefault();
        OH_Drawing_TypefaceDestroy(typeface);
    }
    typeface = OH_Drawing_TypefaceCreateDefault();
    OH_Drawing_FontSetTypeface(font, typeface);

    OH_Drawing_TextBlob* textblob = OH_Drawing_TextBlobCreateFromText(text, len, font, TEXT_ENCODING_UTF8);

    OH_Drawing_CanvasAttachPen(canvas, pen);

    OH_Drawing_CanvasDrawTextBlob(canvas, textblob, NUM_200, NUM_200);
    OH_Drawing_TextBlobDestroy(textblob);
    OH_Drawing_FontDestroy(font);
    OH_Drawing_PenDestroy(pen);
    OH_Drawing_TypefaceDestroy(typeface);
}

void TypefaceCreateFromFile::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    const char* text = "hello";
    size_t len = strlen(text);
    const char path[] = "D:/pic/MyApplication_c_perf_demo/entry/src/main/cpp/hello.cpp";
    int index = 10;
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    OH_Drawing_FontSetTextSize(font, NUM_50);

    OH_Drawing_Typeface* typeface;
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    for (int i = 0; i < testCount_; i++) {
        typeface = OH_Drawing_TypefaceCreateFromFile(path, index);
        OH_Drawing_TypefaceDestroy(typeface);
    }
    typeface = OH_Drawing_TypefaceCreateFromFile(path, index);
    OH_Drawing_FontSetTypeface(font, typeface);
    OH_Drawing_TextBlob* textblob = OH_Drawing_TextBlobCreateFromText(text, len, font, TEXT_ENCODING_UTF8);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_CanvasDrawTextBlob(canvas, textblob, NUM_100, NUM_100);

    OH_Drawing_TypefaceDestroy(typeface);
    OH_Drawing_FontDestroy(font);
    OH_Drawing_PenDestroy(pen);
    OH_Drawing_TypefaceDestroy(typeface);
}

void TypefaceCreateFromStream::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    OH_Drawing_FontSetTextSize(font, NUM_50);

    char data[] = "Hello, World!";
    size_t length = sizeof(data) - 1;

    OH_Drawing_MemoryStream* memoryStream;
    OH_Drawing_Typeface* typeface;
    for (int i = 0; i < testCount_; i++) {
        memoryStream = OH_Drawing_MemoryStreamCreate(data, length, true);
        typeface = OH_Drawing_TypefaceCreateFromStream(memoryStream, 0);
        OH_Drawing_TypefaceDestroy(typeface);
        OH_Drawing_MemoryStreamDestroy(memoryStream);
    }
    memoryStream = OH_Drawing_MemoryStreamCreate(data, length, true);
    typeface = OH_Drawing_TypefaceCreateFromStream(memoryStream, 0);

    OH_Drawing_FontSetTypeface(font, typeface);
    OH_Drawing_TextBlob* textblob = OH_Drawing_TextBlobCreateFromText(data, length, font, TEXT_ENCODING_UTF8);
    OH_Drawing_CanvasDrawTextBlob(canvas, textblob, NUM_100, NUM_100);

    OH_Drawing_TextBlobDestroy(textblob);
    OH_Drawing_TypefaceDestroy(typeface);
    OH_Drawing_MemoryStreamDestroy(memoryStream);
    OH_Drawing_FontDestroy(font);
}