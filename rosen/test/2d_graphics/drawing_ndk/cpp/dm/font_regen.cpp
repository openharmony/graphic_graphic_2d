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
#include "font_regen.h"

#include <array>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_font.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_text_blob.h>

#include "test_common.h"

#include "common/log_common.h"

enum { WIDTH = 512, HEIGHT = 512, N = 10 };
OH_Drawing_TextBlob* make_blob(const std::string& text, OH_Drawing_Font* font)
{
    size_t len = text.length();
    std::array<float, N> pos;
    OH_Drawing_TextBlobBuilder* builder = OH_Drawing_TextBlobBuilderCreate();
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(0, 0, WIDTH, HEIGHT); // 0, 0 矩阵对象创建参数

    int count = OH_Drawing_FontCountText(font, text.c_str(), len, OH_Drawing_TextEncoding::TEXT_ENCODING_UTF8);
    OH_Drawing_TextBlobBuilderAllocRunPos(builder, font, count, rect);
    OH_Drawing_TextBlob* blob = OH_Drawing_TextBlobBuilderMake(builder);
    OH_Drawing_TextBlobBuilderDestroy(builder);
    OH_Drawing_RectDestroy(rect);
    return blob;
}

BadApple::BadApple()
{
    bitmapWidth_ = WIDTH;
    bitmapHeight_ = HEIGHT;
    fileName_ = "badapple";
}

void BadApple::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    static const std::string kTexts[] = {
        ("Meet"),
        ("iPad Pro"),
    };
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    float textSize = 256; //  256 FontSetTextSize 参数
    OH_Drawing_FontSetTextSize(font, textSize);
    OH_Drawing_TextBlob* fBlobs[3];         // OH_Drawing_TextBlob* 3个元素
    fBlobs[0] = make_blob(kTexts[0], font); // return OH_Drawing_TextBlobBuilderMake(builder)
    fBlobs[1] = make_blob(kTexts[1], font); // return OH_Drawing_TextBlobBuilderMake(builder)

    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetColor(pen, 0xFF111111);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_CanvasDrawTextBlob(canvas, fBlobs[0], 10, 260); // 10, 260 CanvasDrawTextBlob参数
    OH_Drawing_CanvasDrawTextBlob(canvas, fBlobs[1], 10, 500); // 10, 500 CanvasDrawTextBlob参数

    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PenDestroy(pen);
    OH_Drawing_TextBlobDestroy(fBlobs[0]);
    OH_Drawing_TextBlobDestroy(fBlobs[1]);
    OH_Drawing_FontDestroy(font);
}
