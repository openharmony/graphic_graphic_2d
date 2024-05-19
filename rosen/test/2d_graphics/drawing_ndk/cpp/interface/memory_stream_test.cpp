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

#include "memory_stream_test.h"

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

#include "test_common.h"

#include "common/log_common.h"

void MemoryStreamCreate::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    TestRend rand;
    float l = rand.nextULessThan(bitmapWidth_);
    float t = rand.nextULessThan(bitmapHeight_);
    float r = l + rand.nextULessThan(bitmapWidth_);
    float b = t + rand.nextULessThan(bitmapHeight_);
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(l, t, r, b);
    unsigned char data[] = { 0x01, 0x02, 0x03, 0x04, 0x05 };
    size_t length = sizeof(data);
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    OH_Drawing_MemoryStream* memoryStream = nullptr;
    for (int i = 0; i < testCount_; i++) {
        memoryStream = OH_Drawing_MemoryStreamCreate(data, length, copyData);
        OH_Drawing_MemoryStreamDestroy(memoryStream);
    }
    OH_Drawing_CanvasDrawRect(canvas, rect);
    OH_Drawing_FontDestroy(font);
    OH_Drawing_RectDestroy(rect);
}
