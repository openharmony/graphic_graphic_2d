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
#include "dashing.h"
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_path_effect.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_shader_effect.h>
#include "common/log_common.h"
#include "test_common.h"

enum {
    K_W = 640,
    K_H = 340,
};

static void drawline(OH_Drawing_Canvas *canvas, int on, int off, OH_Drawing_Pen *pen, Dashings dashings)
{
    float intervals[] = {
        float(on),
        float(off),
    };
    OH_Drawing_PathEffect *effect = OH_Drawing_CreateDashPathEffect(intervals, 2, dashings.phase);
    OH_Drawing_PenSetPathEffect(pen, effect);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_CanvasDrawLine(canvas, dashings.startX, dashings.startY, dashings.finalX, dashings.finalY);
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PathEffectDestroy(effect);
}

static void show_zero_len_dash(OH_Drawing_Canvas *canvas)
{
    Dashings dashings;
    dashings.finalX = (0);
    OH_Drawing_Pen *pen = OH_Drawing_PenCreate();
    drawline(canvas, 2, 2, pen, dashings); // 2, 2int on, int off
    float width = 2;
    OH_Drawing_PenSetWidth(pen, width);
    OH_Drawing_CanvasTranslate(canvas, 0, 20); // 0, 20平移坐标
    drawline(canvas, 4, 4, pen, dashings);     // 4, 4int on, int off
    OH_Drawing_PenDestroy(pen);
}

static void show_giant_dash(OH_Drawing_Canvas *canvas)
{
    Dashings dashings;
    dashings.finalX = (20 * 1000); // 20, 1000 int on, int off
    OH_Drawing_Pen *pen = OH_Drawing_PenCreate();
    drawline(canvas, 1, 1, pen, dashings); // 1,20, 1000 int on, int off
    OH_Drawing_PenDestroy(pen);
}

Dashing::Dashing()
{
    // file gm/dashing.cpp
    bitmapWidth_ = K_W;
    bitmapHeight_ = K_H;
    fileName_ = "dashing";
}

Dashing::~Dashing() {}

void Dashing::OnTestFunction(OH_Drawing_Canvas *canvas)
{
    Dashings dashings;
    OH_Drawing_Pen *pen = OH_Drawing_PenCreate();
    OH_Drawing_CanvasTranslate(canvas, 20, 20); // 20, 20平移坐标
    OH_Drawing_CanvasTranslate(canvas, 0, 0.5); // 0, 0.5平移坐标
    struct Intervals {
        int fOnInterval;
        int fOffInterval;
    };
    for (int width = 0; width <= 2; ++width) { // 2宽度
        for (const Intervals &data : { Intervals { 1, 1 }, Intervals { 4, 1 } }) {
            for (bool aa : { false, true }) {
                int w = width * width * width;
                OH_Drawing_PenSetAntiAlias(pen, aa);
                OH_Drawing_PenSetWidth(pen, w);
                int scale = w ? w : 1;
                drawline(canvas, data.fOnInterval * scale, data.fOffInterval * scale, pen, dashings);
                OH_Drawing_CanvasTranslate(canvas, 0, 20); // 0, 20平移坐标
            }
        }
    }
    show_giant_dash(canvas);
    OH_Drawing_CanvasTranslate(canvas, 0, 20); // 0, 20平移坐标
    show_zero_len_dash(canvas);
    OH_Drawing_CanvasTranslate(canvas, 0, 20); // 0, 20平移坐标
    OH_Drawing_PenSetWidth(pen, 8);            // 8宽度
    drawline(canvas, 0, 0, pen, dashings);
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PenDestroy(pen);
}

void make_path_line(OH_Drawing_Path *path, DrawRect rect)
{
    OH_Drawing_PathMoveTo(path, rect.left, rect.top);
    OH_Drawing_PathLineTo(path, rect.right, rect.bottom);
}

void make_path_rect(OH_Drawing_Path *path, DrawRect rect)
{
    OH_Drawing_PathAddRect(path, rect.left, rect.top, rect.right, rect.bottom,
        OH_Drawing_PathDirection::PATH_DIRECTION_CW);
}

void make_path_oval(OH_Drawing_Path *path, DrawRect rect)
{
    OH_Drawing_PathAddOvalWithInitialPoint(path, OH_Drawing_RectCreate(rect.left, rect.top, rect.right, rect.bottom),
        45, // 45正方形
        OH_Drawing_PathDirection::PATH_DIRECTION_CW);
}

void make_path_star(OH_Drawing_Path *path, DrawRect rect)
{
    int n = 5; // star corner count
    float rad = -M_PI_2;
    const float drad = (n >> 1) * M_PI * 2 / n;

    DrawRect r = { 0, 0, 0, 0 };
    OH_Drawing_PathMoveTo(path, 0, -1.0);
    DrawPathGetBound(r, 0, -1.0);
    for (int i = 1; i < n; i++) {
        rad += drad;
        OH_Drawing_PathLineTo(path, cos(rad), sin(rad));
        DrawPathGetBound(r, cos(rad), sin(rad));
    }
    OH_Drawing_PathClose(path);
    OH_Drawing_Matrix *m = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixSetRectToRect(m, DrawCreateRect(r), DrawCreateRect(rect),
        OH_Drawing_ScaleToFit::SCALE_TO_FIT_CENTER);
    OH_Drawing_PathTransform(path, m);
}

Dashing2::Dashing2()
{
    // file gm/fontregen.cpp
    bitmapWidth_ = 640;  // 640宽度
    bitmapHeight_ = 480; // 480高度
    fileName_ = "dashing2";
}

Dashing2::~Dashing2() {}

void Dashing2::OnTestFunction(OH_Drawing_Canvas *canvas)
{
    constexpr int gIntervals[] = {
        3, // 3 dashes: each count [0] followed by intervals [1..count]
        2, 10, 10,
        4, 20, 5, 5, 5,
        2, 2, 2
    };

    void (*gProc[])(OH_Drawing_Path * path, DrawRect rect) = {
        make_path_line,
        make_path_rect,
        make_path_oval,
        make_path_star,
    };
    OH_Drawing_Pen *pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetAntiAlias(pen, true);
    OH_Drawing_PenSetWidth(pen, 6.0); // 6.0 宽度

    DrawRect bounds = { 0, 0, 120, 120 };
    bounds.Offset(20, 20); // 20 坐标
    float dx = bounds.Width() * 4.0 / 3;
    float dy = bounds.Height() * 4.0 / 3;

    const int *intervals = &gIntervals[1];
    for (int y = 0; y < gIntervals[0]; ++y) {
        float vals[sizeof(gIntervals) / sizeof(int)];
        int count = *intervals++;
        for (int i = 0; i < count; ++i) {
            vals[i] = *intervals++;
        }
        float phase = vals[0] / 2;
        OH_Drawing_PathEffect *dasheffect = OH_Drawing_CreateDashPathEffect(vals, count, phase);
        OH_Drawing_PenSetPathEffect(pen, dasheffect);
        OH_Drawing_CanvasAttachPen(canvas, pen);

        for (size_t x = 0; x < sizeof(gProc) / sizeof(gProc[0]); ++x) {
            OH_Drawing_Path *path = OH_Drawing_PathCreate();
            DrawRect r = bounds;
            r.Offset(x * dx, y * dy);
            gProc[x](path, r);
            OH_Drawing_CanvasDrawPath(canvas, path);
            OH_Drawing_PathDestroy(path);
        }
    }
}

Dashing4::Dashing4()
{
    // file gm/fontregen.cpp
    bitmapWidth_ = 640;   // 640宽度
    bitmapHeight_ = 1100; // 1100高度
    fileName_ = "dashing4";
}

Dashing4::~Dashing4() {}

void Dashing4::DashingNum(OH_Drawing_Canvas *canvas, OH_Drawing_Pen *pen)
{
    struct Intervals {
        int fOnInterval;
        int fOffInterval;
    };
    Dashings dashings;
    for (int width = 0; width <= 2; ++width) { // 2 max
        for (const Intervals &data : { Intervals { 1, 1 }, Intervals { 4, 2 }, Intervals { 0, 4 } }) {
            // test for zero length on interval.zero length intervals should draw.a line of squares or circles
            for (bool aa : { false, true }) {
                for (auto cap : { LINE_ROUND_CAP, LINE_SQUARE_CAP }) {
                    int w = width * width * width;
                    OH_Drawing_PenSetAntiAlias(pen, aa);
                    OH_Drawing_PenSetWidth(pen, w);
                    OH_Drawing_PenSetCap(pen, cap);
                    int scale = w ? w : 1;

                    drawline(canvas, data.fOnInterval * scale, data.fOffInterval * scale, pen, dashings);
                    OH_Drawing_CanvasTranslate(canvas, 0, 20); // 0, 20平移坐标
                }
            }
        }
    }
}

void Dashing4::DashingTow(OH_Drawing_Canvas *canvas, OH_Drawing_Pen *pen)
{
    for (int aa = 0; aa <= 1; ++aa) {
        OH_Drawing_PenSetAntiAlias(pen, aa);
        OH_Drawing_PenSetWidth(pen, 8.f);
        OH_Drawing_PenSetCap(pen, LINE_SQUARE_CAP);
        // Single dash element that is cut off at start and end
        {
            Dashings dashings;
            dashings.finalX = 20.0;                    // 20.0 坐标
            dashings.phase = 5.0;                      // 5.0 坐标
            drawline(canvas, 32, 16, pen, dashings);   // canvas, 32, 16, pen, 20.0, 0, 5.0 画线
            OH_Drawing_CanvasTranslate(canvas, 0, 20); // 0, 20平移坐标
        }

        {
            Dashings dashings;
            dashings.finalX = 56.0;                    // 56.0 坐标
            dashings.phase = 5.0;                      // 5.0 坐标
            // Two dash elements where each one is cut off at beginning and end respectively
            drawline(canvas, 32, 16, pen, dashings);   // canvas, 32, 16, pen, 56.0, 0, 5.0 画线
        }

        {
            OH_Drawing_CanvasTranslate(canvas, 0, 20); // 0, 20平移坐标
            dashings.finalX = 584.0;                   // 584.0 坐标
            dashings.phase = 5.0;                      // 5.0 坐标
            // Many dash elements where first and last are cut off at beginning and end respectively
            drawline(canvas, 32, 16, pen, dashings);   // canvas, 32, 16, pen, 584.0, 0, 5.0 画线
            OH_Drawing_CanvasTranslate(canvas, 0, 20); // 0, 20平移坐标
        }
        
        {
            Dashings dashings;
            // Diagonal dash line where src pnts are not axis aligned (as apposed to being diagonal from
            // a canvas rotation)
            dashings.finalX = 600.0;                   // 600.0 坐标
            dashings.finalY = 30.0;                    // 30.0 坐标
            drawline(canvas, 32, 16, pen, dashings);   // canvas, 32, 16, pen, 600.0, 30.0 画线
            OH_Drawing_CanvasTranslate(canvas, 0, 20); // 0, 20平移坐标
        }

        {
            Dashings dashings;
            dashings.finalX = 8.0;                     // 8.0 坐标
            dashings.finalY = 0.0;
            dashings.phase = 40.0; // 40.0 坐标
            // Case where only the off interval exists on the line. Thus nothing should be drawn
            drawline(canvas, 32, 16, pen, dashings);   // canvas, 32, 16, pen, 8.0, 0.0, 40.0 画线
            OH_Drawing_CanvasTranslate(canvas, 0, 20); // 0, 20平移坐标
        }
    }
}

void Dashing4::DashingThree(OH_Drawing_Canvas *canvas, OH_Drawing_Pen *pen)
{
    {
        Dashings dashings;
        OH_Drawing_CanvasTranslate(canvas, 5, 20); // 5, 20平移坐标
        OH_Drawing_PenSetAntiAlias(pen, true);
        OH_Drawing_PenSetCap(pen, LINE_ROUND_CAP);
        OH_Drawing_PenSetColor(pen, 0x44000000);
        OH_Drawing_PenSetWidth(pen, 40);        // 40宽度
        drawline(canvas, 0, 30, pen, dashings); // 0, 30 int on, int off
    }
    
    {
        Dashings dashings;
        OH_Drawing_CanvasTranslate(canvas, 0, 50); // 0, 50平移坐标
        OH_Drawing_PenSetCap(pen, LINE_SQUARE_CAP);
        drawline(canvas, 0, 30, pen, dashings); // 0, 30 int on, int off
    }

    {
        Dashings dashings;
        // Test we draw the cap when the line length is zero.
        OH_Drawing_CanvasTranslate(canvas, 0, 50); // 0, 50平移坐标
        OH_Drawing_PenSetCap(pen, LINE_ROUND_CAP);
        OH_Drawing_PenSetColor(pen, 0xFF000000);
        OH_Drawing_PenSetWidth(pen, 11); // 11宽度
        dashings.finalX = 0.0;
        drawline(canvas, 0, 30, pen, dashings); // 0, 30 int on, int off
    }

    {
        Dashings dashings;
        dashings.finalX = 0.0;
        OH_Drawing_CanvasTranslate(canvas, 100, 0); // 100, 0平移坐标
        drawline(canvas, 1, 30, pen, dashings);     // 1, 30 int on, int off
    }
}

void Dashing4::OnTestFunction(OH_Drawing_Canvas *canvas)
{
    OH_Drawing_Pen *pen = OH_Drawing_PenCreate();
    OH_Drawing_CanvasTranslate(canvas, 20, 20);   // 20, 20平移坐标
    OH_Drawing_CanvasTranslate(canvas, 0.5, 0.5); // 0.5, 0.5平移坐标
    DashingNum(canvas, pen);
    DashingTow(canvas, pen);
    DashingThree(canvas, pen);
    OH_Drawing_PenDestroy(pen);
}