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
#include "points.h"

#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_shader_effect.h>

#include "test_common.h"

#include "common/log_common.h"

enum {
    K_W = 640, // 640 是位图宽度
    K_H = 490, // 490 是位图高度
};
Points::Points()
{
    bitmapWidth_ = K_W;
    bitmapHeight_ = K_H;
    fileName_ = "points";
}

void Points::fill_pts(OH_Drawing_Point2D pts[], size_t n, TestRend* rand)
{
    for (size_t i = 0; i < n; i++) {
        // Compute these independently and store in variables, rather
        // than in the parameter-passing expression, to get consistent
        // evaluation order across compilers.
        float y = rand->nextUScalar1() * 480;
        float x = rand->nextUScalar1() * 640;
        pts[i].x = x;
        pts[i].y = y;
    }
}

void Points::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_CanvasTranslate(canvas, 1, 1); // 1,1 用于平移画布
    TestRend rand;
    const size_t n = 99; // 99 要绘制的点的数量
    OH_Drawing_Pen* pen0 = OH_Drawing_PenCreate();
    OH_Drawing_Brush* brush1 = OH_Drawing_BrushCreate();
    OH_Drawing_Pen* pen2 = OH_Drawing_PenCreate();
    OH_Drawing_Brush* brush3 = OH_Drawing_BrushCreate();

    OH_Drawing_PenSetColor(pen0, DRAW_COLORRED);
    OH_Drawing_BrushSetColor(brush1, DRAW_COLORGREEN);
    OH_Drawing_PenSetColor(pen2, DRAW_COLORBLUE);
    OH_Drawing_BrushSetColor(brush3, DRAW_COLORWHITE);

    OH_Drawing_PenSetWidth(pen0, 4); // 4 画笔pen0的宽度
    OH_Drawing_PenSetCap(pen2, LINE_ROUND_CAP);
    OH_Drawing_PenSetWidth(pen2, 6); // 6 画笔pen2的宽度

    OH_Drawing_Point2D pts[n];
    fill_pts(pts, n, &rand);

    OH_Drawing_CanvasAttachPen(canvas, pen0);
    OH_Drawing_CanvasDrawPoints(canvas, POINT_MODE_POLYGON, n, pts);
    OH_Drawing_CanvasDetachPen(canvas);

    OH_Drawing_CanvasAttachBrush(canvas, brush1);
    OH_Drawing_CanvasDrawPoints(canvas, POINT_MODE_LINES, n, pts);
    OH_Drawing_CanvasDetachBrush(canvas);

    OH_Drawing_CanvasAttachPen(canvas, pen2);
    OH_Drawing_CanvasDrawPoints(canvas, POINT_MODE_POINTS, n, pts);
    OH_Drawing_CanvasDetachPen(canvas);

    OH_Drawing_CanvasAttachBrush(canvas, brush3);
    OH_Drawing_CanvasDrawPoints(canvas, POINT_MODE_POINTS, n, pts);
    OH_Drawing_CanvasDetachBrush(canvas);

    OH_Drawing_PenDestroy(pen0);
    OH_Drawing_PenDestroy(pen2);
    OH_Drawing_BrushDestroy(brush1);
    OH_Drawing_BrushDestroy(brush3);
}
