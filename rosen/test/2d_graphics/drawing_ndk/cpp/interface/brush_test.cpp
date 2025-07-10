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
#include "brush_test.h"

#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_font.h>
#include <native_drawing/drawing_rect.h>
#include <native_color_space_manager/native_color_space_manager.h>

#include "common/log_common.h"

void BrushReset::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Brush* DemoRef = OH_Drawing_BrushCreate();
    OH_Drawing_BrushSetColor(DemoRef, 0xff00ff00);
    OH_Drawing_CanvasAttachBrush(canvas, DemoRef);
    OH_Drawing_Rect* rect1 = OH_Drawing_RectCreate(400, 400, 500, 500); // 400, 400, 500, 500 矩形参数
    OH_Drawing_CanvasDrawRect(canvas, rect1);
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_BrushReset(DemoRef);
    }
    OH_Drawing_CanvasAttachBrush(canvas, DemoRef);
    OH_Drawing_Rect* rect2 = OH_Drawing_RectCreate(200, 200, 300, 300); // 200, 200, 300, 300 矩形参数
    OH_Drawing_CanvasDrawRect(canvas, rect2);
    OH_Drawing_RectDestroy(rect1);
    OH_Drawing_RectDestroy(rect2);
    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_BrushDestroy(DemoRef);
};

void BrushSetColor4f::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    ColorSpaceName colorSpaceName = ColorSpaceName::SRGB;
    OH_NativeColorSpaceManager *colorSpaceManager = OH_NativeColorSpaceManager_CreateFromName(colorSpaceName);
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(400, 400, 500, 500); // 400, 400, 500, 500 矩形参数;
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_BrushSetColor4f(brush, 0.5f, 0.5f, 0.5f, 0.5f, colorSpaceManager);
    }
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    OH_Drawing_CanvasDrawRect(canvas, rect);
    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_BrushDestroy(brush);
    OH_NativeColorSpaceManager_Destroy(colorSpaceManager);
}

void BrushGetAlphaFloat::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(400, 400, 500, 500); // 400, 400, 500, 500 矩形参数;
    for (int i = 0; i < testCount_; i++) {
        float a = 0.1f;
        OH_Drawing_BrushGetAlphaFloat(brush, &a);
    }
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    OH_Drawing_CanvasDrawRect(canvas, rect);
    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_BrushDestroy(brush);
}

void BrushGetRedFloat::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(400, 400, 500, 500); // 400, 400, 500, 500 矩形参数;
    for (int i = 0; i < testCount_; i++) {
        float r = 0.1f;
        OH_Drawing_BrushGetRedFloat(brush, &r);
    }
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    OH_Drawing_CanvasDrawRect(canvas, rect);
    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_BrushDestroy(brush);
}

void BrushGetBlueFloat::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(400, 400, 500, 500); // 400, 400, 500, 500 矩形参数;
    for (int i = 0; i < testCount_; i++) {
        float b = 0.1f;
        OH_Drawing_BrushGetBlueFloat(brush, &b);
    }
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    OH_Drawing_CanvasDrawRect(canvas, rect);
    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_BrushDestroy(brush);
}

void BrushGetGreenFloat::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(400, 400, 500, 500); // 400, 400, 500, 500 矩形参数;
    for (int i = 0; i < testCount_; i++) {
        float g = 0.1f;
        OH_Drawing_BrushGetGreenFloat(brush, &g);
    }
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    OH_Drawing_CanvasDrawRect(canvas, rect);
    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_BrushDestroy(brush);
}