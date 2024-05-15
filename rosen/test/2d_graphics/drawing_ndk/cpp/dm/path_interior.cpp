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

#include "path_interior.h"

#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_shader_effect.h>
#include <rdma/bnxt_re-abi.h>

#include "test_common.h"

#include "common/log_common.h"

enum {
    K_W = 770, // 770 是位图宽度
    K_H = 770, // 770 是位图高度
};

typedef struct {
    int insetFirst;
    int doEvenOdd;
    int outerRR;
    int innerRR;
    int outerCW;
    int innerCW;
} DRAW_PARAM;

namespace {
void show(OH_Drawing_Canvas* canvas, OH_Drawing_Path* path)
{
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_BrushSetAntiAlias(brush, true);
    bool hasInterior = false;
    OH_Drawing_BrushSetColor(brush, hasInterior ? color_to_565(0xFF8888FF) : 0xFF888888); // gray
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    OH_Drawing_CanvasDrawPath(canvas, path);
    OH_Drawing_CanvasDetachBrush(canvas);

    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetAntiAlias(pen, true);
    OH_Drawing_PenSetColor(pen, 0xFFFF0000);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_CanvasDrawPath(canvas, path);
    OH_Drawing_CanvasDetachPen(canvas);

    OH_Drawing_BrushDestroy(brush);
    OH_Drawing_PenDestroy(pen);
}

DrawRect inset(DrawRect& r)
{
    DrawRect rect = r;
    rect.Inset(r.Width() / 8, r.Height() / 8); // 8  rect控制
    return rect;
}

void draw_sence(OH_Drawing_Canvas* canvas, DRAW_PARAM& param, int& i)
{
    DrawRect rect = { 0, 0, 80, 80 };   // 0, 0, 80, 80 矩形参数
    const float rad = rect.Width() / 8; // 8 用于计算矩形的内边距
    int insetFirst = param.insetFirst;
    int doEvenOdd = param.doEvenOdd;
    int outerRR = param.outerRR;
    int innerRR = param.innerRR;
    int outerCW = param.outerCW;
    int innerCW = param.innerCW;

    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_PathSetFillType(path, doEvenOdd ? PATH_FILL_TYPE_EVEN_ODD : PATH_FILL_TYPE_WINDING);
    OH_Drawing_PathDirection outerDir = outerCW ? PATH_DIRECTION_CW : PATH_DIRECTION_CCW;
    OH_Drawing_PathDirection innerDir = innerCW ? PATH_DIRECTION_CW : PATH_DIRECTION_CCW;

    DrawRect rc = insetFirst ? inset(rect) : rect;
    OH_Drawing_Rect* r = OH_Drawing_RectCreate(rc.left, rc.top, rc.right, rc.bottom);
    OH_Drawing_RoundRect* rr = OH_Drawing_RoundRectCreate(r, rad, rad);
    if (outerRR) {
        OH_Drawing_PathAddRoundRect(path, rr, outerDir);
    } else {
        OH_Drawing_PathAddRect(path, rc.left, rc.top, rc.right, rc.bottom, outerDir);
    }
    OH_Drawing_RoundRectDestroy(rr);
    OH_Drawing_RectDestroy(r);

    rc = insetFirst ? rect : inset(rect);
    r = OH_Drawing_RectCreate(rc.left, rc.top, rc.right, rc.bottom);
    rr = OH_Drawing_RoundRectCreate(r, rad, rad);
    if (innerRR) {
        OH_Drawing_PathAddRoundRect(path, rr, innerDir);
    } else {
        OH_Drawing_PathAddRect(path, rc.left, rc.top, rc.right, rc.bottom, innerDir);
    }
    OH_Drawing_RoundRectDestroy(rr);
    OH_Drawing_RectDestroy(r);

    float dx = (i / 8) * rect.Width() * 6 / 5; //   (i / 8) * rect.Width() * 6 / 5 用于计算路径在画布上的偏移量
    float dy = (i % 8) * rect.Height() * 6 / 5; //   (i / 8) * rect.Height() * 6 / 5 用于计算路径在画布上的偏移量
    i++;
    OH_Drawing_PathOffset(path, path, dx, dy);
    show(canvas, path);
    OH_Drawing_PathDestroy(path);
}

void draw_sence_innerCW(OH_Drawing_Canvas* canvas, DRAW_PARAM& param, int& i)
{
    for (int innerCW = 0; innerCW <= 1; ++innerCW) { // 1 模式选择
        param.innerCW = innerCW;
        draw_sence(canvas, param, i);
    }
}

void draw_sence_outerCW(OH_Drawing_Canvas* canvas, DRAW_PARAM& param, int& i)
{
    for (int outerCW = 0; outerCW <= 1; ++outerCW) { // 1 模式选择
        param.outerCW = outerCW;
        draw_sence_innerCW(canvas, param, i);
    }
}

void draw_sence_innerRR(OH_Drawing_Canvas* canvas, DRAW_PARAM& param, int& i)
{
    for (int innerRR = 0; innerRR <= 1; ++innerRR) { // 1 模式选择
        param.innerRR = innerRR;
        draw_sence_outerCW(canvas, param, i);
    }
}

void draw_sence_outerRR(OH_Drawing_Canvas* canvas, DRAW_PARAM& param, int& i)
{
    for (int outerRR = 0; outerRR <= 1; ++outerRR) { // 1 模式选择
        param.outerRR = outerRR;
        draw_sence_innerRR(canvas, param, i);
    }
}

void draw_sence_doEvenOdd(OH_Drawing_Canvas* canvas, DRAW_PARAM& param, int& i)
{
    for (int doEvenOdd = 0; doEvenOdd <= 1; ++doEvenOdd) { // 1 模式选择
        param.doEvenOdd = doEvenOdd;
        draw_sence_outerRR(canvas, param, i);
    }
}

void draw_sence_insetFirst(OH_Drawing_Canvas* canvas, DRAW_PARAM& param, int& i)
{
    for (int insetFirst = 0; insetFirst <= 1; ++insetFirst) { // 1 模式选择
        param.insetFirst = insetFirst;
        draw_sence_doEvenOdd(canvas, param, i);
    }
}

} // namespace

PathInterior::PathInterior()
{
    bitmapWidth_ = K_W;
    bitmapHeight_ = K_H;
    fileName_ = "pathinterior";
}

void PathInterior::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_CanvasClear(canvas, 0xFFDDDDDD);     // gray
    OH_Drawing_CanvasTranslate(canvas, 8.5f, 8.5f); // 8.5f 定义了画布的平移量

    int i = 0; // 0 PathOffset

    DRAW_PARAM param = { 0 };
    draw_sence_insetFirst(canvas, param, i);
}
