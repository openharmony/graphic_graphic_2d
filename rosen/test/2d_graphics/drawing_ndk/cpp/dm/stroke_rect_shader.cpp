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
#include "stroke_rect_shader.h"

#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_shader_effect.h>
#include <native_drawing/drawing_rect.h>
#include "test_common.h"

#include "common/log_common.h"

void drawRect(OH_Drawing_Canvas* canvas, OH_Drawing_Pen* pen, bool aa)
{
    float kPad = 20;
    float kSize = 100;
    float fWidth = 10.f;
    float fMiter = 0.01f;
    OH_Drawing_Rect* kRect = OH_Drawing_RectCreate(0, 0, kSize, kSize);

    OH_Drawing_PenSetAntiAlias(pen, aa);
    OH_Drawing_CanvasSave(canvas);
    OH_Drawing_PenSetWidth(pen, fWidth);
    OH_Drawing_PenSetJoin(pen, OH_Drawing_PenLineJoinStyle::LINE_BEVEL_JOIN);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_CanvasDrawRect(canvas, kRect);
    OH_Drawing_CanvasTranslate(canvas, kSize + kPad, 0);

    OH_Drawing_PenSetJoin(pen, OH_Drawing_PenLineJoinStyle::LINE_MITER_JOIN);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_CanvasDrawRect(canvas, kRect);
    OH_Drawing_CanvasTranslate(canvas, kSize + kPad, 0);

    OH_Drawing_PenSetMiterLimit(pen, fMiter);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_CanvasDrawRect(canvas, kRect);
    OH_Drawing_CanvasTranslate(canvas, kSize + kPad, 0);

    OH_Drawing_PenSetJoin(pen, OH_Drawing_PenLineJoinStyle::LINE_ROUND_JOIN);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_CanvasDrawRect(canvas, kRect);
    OH_Drawing_CanvasTranslate(canvas, kSize + kPad, 0);

    OH_Drawing_PenSetWidth(pen, 0);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_CanvasDrawRect(canvas, kRect);

    OH_Drawing_CanvasRestore(canvas);
    OH_Drawing_CanvasTranslate(canvas, 0, kSize + kPad);

    OH_Drawing_RectDestroy(kRect);
}

void StrokeRectShader::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    float kSize = 100;
    DrawRect Rect = { 0, 0, kSize, kSize };
    OH_Drawing_Point* startPts = OH_Drawing_PointCreate(Rect.left, Rect.top);
    OH_Drawing_Point* endPts = OH_Drawing_PointCreate(Rect.right, Rect.bottom);
    const uint32_t kColors[] = {
        DRAW_COLORRED, // 0xFFFF0000
        DRAW_COLORBLUE, // 0xFF0000FF
    };

    int nCount = 2;
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_ShaderEffect* shaderEffect = OH_Drawing_ShaderEffectCreateLinearGradient(
        startPts, endPts, kColors, nullptr, nCount, OH_Drawing_TileMode::CLAMP);
    OH_Drawing_PenSetShaderEffect(pen, shaderEffect);

    float fHalf = 0.5f;

    OH_Drawing_CanvasTranslate(canvas, fHalf * (Rect.left + Rect.right), fHalf * (Rect.top + Rect.bottom));
    float kPad = 20;
    for (auto aa : { false, true }) {
        drawRect(canvas, pen, aa);
    }

    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_ShaderEffectDestroy(shaderEffect);
    OH_Drawing_PenDestroy(pen);
    OH_Drawing_PointDestroy(startPts);
    OH_Drawing_PointDestroy(endPts);
}