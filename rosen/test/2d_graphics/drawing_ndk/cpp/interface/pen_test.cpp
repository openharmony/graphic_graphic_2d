//
// Created on 2024/5/6.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".
#include "pen_test.h"

#include <multimedia/image_framework/image_pixel_map_mdk.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_pen.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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