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

#include "pen_test.h"

#include <multimedia/image_framework/image_pixel_map_mdk.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_pen.h>
#include <cstdio.h>
#include <cstdlib.h>
#include <ctime.h>

#include "test_common.h"

#include "common/log_common.h"

void PenReset::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    TestRend rand;
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetColor(pen, DRAW_COLORRED);
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_PenReset(pen);
    }
    OH_Drawing_PenSetWidth(pen, 10); // 10 width
    OH_Drawing_PenSetColor(pen, DRAW_COLORGREEN);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    float x1 = rand.nextULessThan(bitmapWidth_);
    float y1 = rand.nextULessThan(bitmapHeight_);
    float x2 = rand.nextULessThan(bitmapWidth_);
    float y2 = rand.nextULessThan(bitmapHeight_);
    OH_Drawing_CanvasDrawLine(canvas, x1, y1, x2, y2);
    OH_Drawing_PenDestroy(pen);
}