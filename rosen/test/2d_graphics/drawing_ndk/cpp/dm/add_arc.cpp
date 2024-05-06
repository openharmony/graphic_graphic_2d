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
#include "add_arc.h"
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_shader_effect.h>
#include <native_drawing/drawing_point.h>
#include "test_common.h"
#include "common/log_common.h"
#include <cmath>
#include <unistd.h>


#define R 400

struct RECT {
    float x;
    float y;
    float w;
    float h;
    void Inset(float dx, float dy)
    {
        x += dx;
        y += dy;
        w -= 2 * dx; // 2宽
        h -= 2 * dy; // 2高
    }
};


AddArcMeas::AddArcMeas()
{
    // file gm/addarc.cpp
    bitmapWidth_ = 2 * R + 40;  // 2 * R + 40宽度
    bitmapHeight_ = 2 * R + 40; // 2 * R + 40高度
    fileName_ = "addarc_meas";
}

AddArcMeas::~AddArcMeas() {}

void AddArcMeas::OnTestFunction(OH_Drawing_Canvas *canvas)
{
    OH_Drawing_CanvasTranslate(canvas, R + 20, R + 20); // 20距离
    OH_Drawing_Pen *pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetAntiAlias(pen, true);
    OH_Drawing_CanvasAttachPen(canvas, pen);

    OH_Drawing_Pen *measPen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetAntiAlias(measPen, true);
    OH_Drawing_PenSetColor(measPen, 0xFFFF0000);

    OH_Drawing_Rect *oval = OH_Drawing_RectCreate(-R, -R, R, R);
    OH_Drawing_CanvasDrawOval(canvas, oval);
    OH_Drawing_CanvasDetachPen(canvas);

    for (float deg = 0; deg < 360; deg += 10) { // 360,10 cout
        const float rad = (deg) / 180 * M_PI;
        float rx = cos(rad) * R;
        float ry = sin(rad) * R;
        OH_Drawing_CanvasAttachPen(canvas, pen);
        OH_Drawing_CanvasDrawLine(canvas, 0, 0, rx, ry);

        OH_Drawing_Path *meas = OH_Drawing_PathCreate();
        OH_Drawing_PathAddArc(meas, oval, 0, deg);
        float arcLen = rad * R;

        OH_Drawing_CanvasAttachPen(canvas, measPen);
        if (deg != 0)
            // 没有getPosTan工具函数，先用已有的point替代
            OH_Drawing_CanvasDrawLine(canvas, 0, 0, rx, ry);
        OH_Drawing_PathDestroy(meas);
    }
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PenDestroy(pen);
    OH_Drawing_PenDestroy(measPen);
    OH_Drawing_RectDestroy(oval);
    pen = nullptr;
    measPen = nullptr;
}

AddArc::AddArc()
{
    bitmapWidth_ = 1040;  // 1040宽度
    bitmapHeight_ = 1040; // 1040高度
    fileName_ = "addarc";
}

void AddArc::OnTestFunction(OH_Drawing_Canvas *canvas)
{
    OH_Drawing_CanvasTranslate(canvas, 20, 20); // 20距离
    RECT rc = { 0, 0, 1000, 1000 };

    OH_Drawing_Pen *pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetAntiAlias(pen, true);
    OH_Drawing_PenSetWidth(pen, 15); // 15宽度

    const float inset = OH_Drawing_PenGetWidth(pen) + 4; // 4 cout
    const float sweepAngle = 345;
    TestRend rand;
    float sign = 1;

    while (rc.w > OH_Drawing_PenGetWidth(pen) * 3) {                            // 3 cout
        OH_Drawing_PenSetColor(pen, color_to_565(rand.nextU() | (0xFF << 24))); // 24颜色偏移
        OH_Drawing_CanvasAttachPen(canvas, pen);
        float startAngle = rand.nextUScalar1() * 360;

        float speed = sqrtf(16 / rc.w) * 0.5f;
        OnAnimate();
        startAngle += fRotate * 360 * speed * sign; // 360 cout
        OH_Drawing_Path *path = OH_Drawing_PathCreate();
        OH_Drawing_Rect *r = OH_Drawing_RectCreate(rc.x, rc.y, rc.x + rc.w, rc.y + rc.h);
        OH_Drawing_PathAddArc(path, r, startAngle, sweepAngle);
        OH_Drawing_CanvasDrawPath(canvas, path);
        rc.Inset(inset, inset);
        sign = -sign;
        OH_Drawing_RectDestroy(r);
        OH_Drawing_PathDestroy(path);
    }
    OH_Drawing_PenDestroy(pen);
    pen = nullptr;
}

bool AddArc::OnAnimate()
{
    // dm中这个函数通过onIdle调用，目前drawing测试框架还没有提供这个接口，因此画出的图形在缺口方向上和dm的不同
    // 通过对源代码运行对这个值的打印，发现是0，经过运行结果对比，发现缺口方向是对的
    fRotate = 0;
    return true;
}