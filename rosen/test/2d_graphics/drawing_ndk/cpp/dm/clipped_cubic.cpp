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
#include "clipped_cubic.h"
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_brush.h>

enum {
    K_W = 1240,
    K_H = 390,
};

ClippedCubic::ClippedCubic()
{
    // file gm/cubicpaths.cpp
    bitmapWidth_ = K_W;
    bitmapHeight_ = K_H;
    fileName_ = "clippedcubic";
}

void ClippedCubic::OnTestFunction(OH_Drawing_Canvas *canvas)
{
    OH_Drawing_Path *path = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(path, float(0), float(0));
    OH_Drawing_PathCubicTo(path, float(140), float(150), float(40), // 140 150 40坐标
        float(10), float(170), float(150));                         // 10 170 150 坐标

    OH_Drawing_Rect *bounds = OH_Drawing_RectCreate(float(0), float(0), float(170), float(150)); // 170 150 坐标

    OH_Drawing_Brush *brush = OH_Drawing_BrushCreate();
    OH_Drawing_CanvasAttachBrush(canvas, brush);

    for (float dy = -1; dy <= 1; dy += 1) {
        OH_Drawing_CanvasSave(canvas);
        for (float dx = -1; dx <= 1; dx += 1) {
            OH_Drawing_CanvasSave(canvas);
            OH_Drawing_CanvasClipRect(canvas, bounds, INTERSECT, false);
            OH_Drawing_CanvasTranslate(canvas, dx, dy);
            OH_Drawing_CanvasDrawPath(canvas, path);
            OH_Drawing_CanvasRestore(canvas);
            OH_Drawing_CanvasTranslate(canvas, OH_Drawing_RectGetWidth(bounds), 0);
        }
        OH_Drawing_CanvasRestore(canvas);
        OH_Drawing_CanvasTranslate(canvas, 0, OH_Drawing_RectGetHeight(bounds));
    }

    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_BrushDestroy(brush);
    OH_Drawing_RectDestroy(bounds);
    OH_Drawing_PathDestroy(path);
}
