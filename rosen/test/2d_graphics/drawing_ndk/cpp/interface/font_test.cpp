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
#include "font_test.h"

#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_font.h>
#include <native_drawing/drawing_rect.h>

#include "common/log_common.h"

void FontCountText::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Brush* DemoRef = OH_Drawing_BrushCreate();
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    int length;
    char* text = "hello word";
    for (int i = 0; i < testCount_; i++) {
        length = OH_Drawing_FontCountText(font, text, strlen(text), OH_Drawing_TextEncoding::TEXT_ENCODING_UTF8);
    }
    OH_Drawing_Rect* rect1 = OH_Drawing_RectCreate(
        length * 10, length * 10, length * 20, length * 20); // length*10, length*10, length*20, length*20 矩形参数
    OH_Drawing_CanvasDrawRect(canvas, rect1);
    OH_Drawing_RectDestroy(rect1);
}
