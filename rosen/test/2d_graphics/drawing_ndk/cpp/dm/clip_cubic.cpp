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
#include "clip_cubic.h"

#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_rect.h>

#include "test_common.h"

#include "common/log_common.h"

const float W = 100;
const float H = 240;

ClipCubic::ClipCubic()
{
    // 对标 aaclip.cpp
    bitmapWidth_ = 400;  // 400宽度
    bitmapHeight_ = 410; // 410高度
    fileName_ = "clipcubic";
}

void DoDraw(OH_Drawing_Canvas* canvas, const OH_Drawing_Path* path)
{
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();

    OH_Drawing_BrushSetAntiAlias(brush, true);
    OH_Drawing_PenSetAntiAlias(pen, true);
    OH_Drawing_BrushSetColor(brush, OH_Drawing_ColorSetArgb(0xFF, 0xCC, 0xCC, 0xCC));
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    OH_Drawing_CanvasDrawPath(canvas, path);
    OH_Drawing_CanvasDetachBrush(canvas);

    OH_Drawing_PenSetColor(pen, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0x00, 0x00));
    OH_Drawing_PenSetAntiAlias(pen, true);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_CanvasDrawPath(canvas, path);
    OH_Drawing_CanvasDetachPen(canvas);

    OH_Drawing_PenDestroy(pen);
    OH_Drawing_BrushDestroy(brush);
}

void DrawAndClip(OH_Drawing_Canvas* canvas, const OH_Drawing_Path* path, float dx, float dy)
{
    uint32_t fSaveCount = 0;
    fSaveCount = OH_Drawing_CanvasGetSaveCount(canvas);
    DRAWING_LOGI("get fSaveCount=%{public}d ", fSaveCount);
    OH_Drawing_CanvasSave(canvas);

    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(0, H / 4, W, H / 4 + H / 2); // 4,2正方形
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_BrushSetColor(brush, color_to_565(0xFF8888FF));
    OH_Drawing_CanvasAttachBrush(canvas, brush);

    OH_Drawing_CanvasDrawRect(canvas, rect);
    OH_Drawing_CanvasDetachBrush(canvas);
    DoDraw(canvas, path);

    OH_Drawing_CanvasTranslate(canvas, dx, dy);

    OH_Drawing_CanvasAttachBrush(canvas, brush);
    OH_Drawing_CanvasDrawRect(canvas, rect);
    OH_Drawing_CanvasClipRect(canvas, rect, OH_Drawing_CanvasClipOp::INTERSECT, false);
    OH_Drawing_CanvasDetachBrush(canvas);
    DoDraw(canvas, path);

    OH_Drawing_CanvasRestoreToCount(canvas, fSaveCount);
    DRAWING_LOGI("ClipCubicGm::GetSaveCount count=%{public}d", fSaveCount);

    OH_Drawing_RectDestroy(rect);
    OH_Drawing_BrushDestroy(brush);
}

void ClipCubic::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Path* fVPath = OH_Drawing_PathCreate();
    OH_Drawing_Path* fHPath = OH_Drawing_PathCreate();

    OH_Drawing_PathMoveTo(fVPath, W, 0);
    OH_Drawing_PathCubicTo(fVPath, W, H - 10, 0, 10, 0, H); // 10坐标
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixRotate(matrix, 90, W / 2, H / 2); // 90 2 坐标

    OH_Drawing_PathTransformWithPerspectiveClip(fVPath, matrix, fHPath, true);

    OH_Drawing_CanvasTranslate(canvas, 80, 10); // 80,10距离
    DrawAndClip(canvas, fVPath, 200, 0);        // 200坐标
    OH_Drawing_CanvasTranslate(canvas, 0, 200); // 200距离
    DrawAndClip(canvas, fHPath, 200, 0);        // 200坐标

    OH_Drawing_PathDestroy(fVPath);
    OH_Drawing_PathDestroy(fHPath);
    OH_Drawing_MatrixDestroy(matrix);
}