/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

#include "pen_test.h"

#include <multimedia/image_framework/image_pixel_map_mdk.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_pen.h>
#include <native_color_space_manager/native_color_space_manager.h>

#include "test_common.h"

#include "common/log_common.h"

void PenReset::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetColor(pen, DRAW_COLORRED);
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_PenReset(pen);
    }
    OH_Drawing_PenSetWidth(pen, 10); // 10 width
    OH_Drawing_PenSetColor(pen, DRAW_COLORGREEN);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_CanvasDrawLine(canvas, 0, 0, 100, 100); // 100 宽高
    OH_Drawing_PenDestroy(pen);
}

void PenSetColor4f::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetWidth(pen, 10); // 10 width
    ColorSpaceName colorSpaceName = ColorSpaceName::SRGB;
    OH_NativeColorSpaceManager *colorSpaceManager = OH_NativeColorSpaceManager_CreateFromName(colorSpaceName);
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(400, 400, 500, 500); // 400, 400, 500, 500 矩形参数;
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_PenSetColor4f(pen, 0.5f, 0.5f, 0.5f, 0.5f, colorSpaceManager);
    }
    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_CanvasDrawRect(canvas, rect);
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_PenDestroy(pen);
    OH_NativeColorSpaceManager_Destroy(colorSpaceManager);
}

void PenGetAlphaFloat::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetWidth(pen, 10); // 10 width
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(400, 400, 500, 500); // 400, 400, 500, 500 矩形参数;
    for (int i = 0; i < testCount_; i++) {
        float a = 0.1f;
        OH_Drawing_PenGetAlphaFloat(pen, &a);
    }
    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_CanvasDrawRect(canvas, rect);
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_PenDestroy(pen);
}

void PenGetRedFloat::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetWidth(pen, 10); // 10 width
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(400, 400, 500, 500); // 400, 400, 500, 500 矩形参数;
    for (int i = 0; i < testCount_; i++) {
        float r = 0.1f;
        OH_Drawing_PenGetRedFloat(pen, &r);
    }
    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_CanvasDrawRect(canvas, rect);
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_PenDestroy(pen);
}

void PenGetBlueFloat::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetWidth(pen, 10); // 10 width
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(400, 400, 500, 500); // 400, 400, 500, 500 矩形参数;
    for (int i = 0; i < testCount_; i++) {
        float b = 0.1f;
        OH_Drawing_PenGetBlueFloat(pen, &b);
    }
    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_CanvasDrawRect(canvas, rect);
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_PenDestroy(pen);
}

void PenGetGreenFloat::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetWidth(pen, 10); // 10 width
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(400, 400, 500, 500); // 400, 400, 500, 500 矩形参数;
    for (int i = 0; i < testCount_; i++) {
        float g = 0.1f;
        OH_Drawing_PenGetGreenFloat(pen, &g);
    }
    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_CanvasDrawRect(canvas, rect);
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_PenDestroy(pen);
}