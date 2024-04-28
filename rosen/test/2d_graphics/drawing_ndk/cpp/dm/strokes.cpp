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

#include "strokes.h"
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

enum {
    kW = 400,
    kH = 400,
    N = 50,
};

namespace {

void rnd_rect(DrawRect &r, OH_Drawing_Pen *pen, TestRend &rand) {
    float x = rand.nextUScalar1() * kW;
    float y = rand.nextUScalar1() * kH;
    float w = rand.nextUScalar1() * (kW >> 2);
    float h = rand.nextUScalar1() * (kH >> 2);
    float hoffset = rand.nextUScalar1();
    float woffset = rand.nextUScalar1();

    r.setXYWH(x, y, w, h);
    r.offset(-w / 2 + woffset, -h / 2 + hoffset);

    OH_Drawing_PenSetColor(pen, rand.nextU() | 0xFF000000);
}
} // namespace

Strokes2::Strokes2() {
    // skia dm file gm/fontregen.cpp
    bitmapWidth_ = kW;
    bitmapHeight_ = kH * 2;
    fileName_ = "strokes_poly";
}

void Strokes2::OnTestFunction(OH_Drawing_Canvas *canvas) {
    // DRAWING_LOGI("OnTestFunction path = %{public}s",__FILE_NAME__);
    TestRend rand;
    fPath = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(fPath, 0, 0);
    for (int i = 0; i < 13; i++) {
        float x = rand.nextUScalar1() * (kW >> 1);
        float y = rand.nextUScalar1() * (kH >> 1);
        OH_Drawing_PathLineTo(fPath, x, y);
    }

    OH_Drawing_CanvasClear(canvas, 0xFFFFFFFF);
    OH_Drawing_Pen *pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetWidth(pen, 4.5f);

    for (int y = 0; y < 2; y++) {
        OH_Drawing_PenSetAntiAlias(pen, !!y);
        OH_Drawing_CanvasSave(canvas);
        OH_Drawing_CanvasTranslate(canvas, 0, y * kH);
        OH_Drawing_Rect *rect = OH_Drawing_RectCreate(2, 2, kW - 2, kH - 2);
        OH_Drawing_CanvasClipRect(canvas, rect, OH_Drawing_CanvasClipOp::INTERSECT, false);
        TestRend randEx;
        for (int i = 0; i < N / 2; i++) {
            DrawRect r;
            rnd_rect(r, pen, randEx);
            OH_Drawing_CanvasAttachPen(canvas, pen);
            OH_Drawing_CanvasRotate(canvas, 15.f, kW / 2, kH / 2);
            OH_Drawing_CanvasDrawPath(canvas, fPath);
            OH_Drawing_CanvasDetachPen(canvas);
        }
        OH_Drawing_RectDestroy(rect);
        OH_Drawing_CanvasRestore(canvas);
    }
    OH_Drawing_PenDestroy(pen);
    OH_Drawing_PathDestroy(fPath);
}
