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
#include "alpha_gradients.h"
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_shader_effect.h>
#include "common/log_common.h"

enum {
    K_W = 640,
    K_H = 480,
};

AlphaGradients::AlphaGradients()
{
    bitmapWidth_ = K_W;
    bitmapHeight_ = K_H;
    fileName_ = "alphagradients";
}

AlphaGradients::~AlphaGradients() {}

void AlphaGradients::draw_grad(OH_Drawing_Canvas *canvas, DrawRect &r, uint32_t c0, uint32_t c1, bool doPreMul)
{
    if (doPreMul)
        c1 = 0xFFFFFFFF;
    uint32_t colors[] = {c0, c1};

    OH_Drawing_Brush *brush = OH_Drawing_BrushCreate();
    OH_Drawing_Pen *pen = OH_Drawing_PenCreate();

    OH_Drawing_Point *startPt = OH_Drawing_PointCreate(r.left, r.top);
    OH_Drawing_Point *endPt = OH_Drawing_PointCreate(r.right, r.bottom);

    static const float_t gPos[] = {0.0f, 1.0f};
    OH_Drawing_ShaderEffect *shaderEffect =
        OH_Drawing_ShaderEffectCreateLinearGradient(startPt, endPt, colors, gPos, 2, OH_Drawing_TileMode::CLAMP);

    OH_Drawing_Rect *rect = OH_Drawing_RectCreate(r.left, r.top, r.right, r.bottom);
    OH_Drawing_BrushSetShaderEffect(brush, shaderEffect);
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    OH_Drawing_CanvasDrawRect(canvas, rect);
    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_BrushSetShaderEffect(brush, nullptr);

    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_CanvasDrawRect(canvas, rect);
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PointDestroy(startPt);
    OH_Drawing_PointDestroy(endPt);
    OH_Drawing_ShaderEffectDestroy(shaderEffect);
    OH_Drawing_BrushDestroy(brush);
    OH_Drawing_PenDestroy(pen);
    OH_Drawing_RectDestroy(rect);
}

void AlphaGradients::OnTestFunction(OH_Drawing_Canvas *canvas)
{
    struct {
        uint32_t fColor0;
        uint32_t fColor1;
    } gRec[] = {
        {0xFFFFFFFF, 0x00000000},
        {0xFFFFFFFF, 0x00FF0000},
        {0xFFFFFFFF, 0x00FFFF00},
        {0xFFFFFFFF, 0x00FFFFFF},
        {0xFFFF0000, 0x00000000},
        {0xFFFF0000, 0x00FF0000},
        {0xFFFF0000, 0x00FFFF00},
        {0xFFFF0000, 0x00FFFFFF},
        {0xFF0000FF, 0x00000000},
        {0xFF0000FF, 0x00FF0000},
        {0xFF0000FF, 0x00FFFF00},
        {0xFF0000FF, 0x00FFFFFF},
    };

    DrawRect r = { 0, 0, 300, 30 };
    OH_Drawing_CanvasTranslate(canvas, 10, 10); // 10平移距离
    size_t gRec_size = sizeof(gRec) / sizeof(gRec[0]);

    for (int doPreMul = 0; doPreMul <= 1; ++doPreMul) {
        OH_Drawing_CanvasSave(canvas);
        for (size_t i = 0; i < gRec_size; ++i) {
            draw_grad(canvas, r, gRec[i].fColor0, gRec[i].fColor1, (doPreMul));
            OH_Drawing_CanvasTranslate(canvas, 0, r.Height() + 8); // r.Height() + 8距离
        }
        OH_Drawing_CanvasRestore(canvas);
        OH_Drawing_CanvasTranslate(canvas, r.Width() + 10, 0); // r.Width() + 10距离
    }
}
