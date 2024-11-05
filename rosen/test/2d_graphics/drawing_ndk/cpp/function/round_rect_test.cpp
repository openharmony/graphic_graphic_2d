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
#include "round_rect_test.h"

#include <cstdint>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_filter.h>
#include <native_drawing/drawing_image.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_path_effect.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_region.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_sampling_options.h>
#include <native_drawing/drawing_shader_effect.h>

#include "test_common.h"

#include "common/log_common.h"

typedef struct {
    float left;
    float top;
    float right;
    float bottom;
} TestRect;

static void DrawRoundRectOffset(
    OH_Drawing_Canvas* canvas, TestRect testRect, float dx, float dy)
{
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(testRect.left, testRect.top, testRect.right, testRect.bottom);
    // 5, 5 for test
    OH_Drawing_RoundRect* roundRect = OH_Drawing_RoundRectCreate(rect, 5, 5);
    OH_Drawing_RoundRectOffset(roundRect, dx, dy);
    OH_Drawing_CanvasDrawRoundRect(canvas, roundRect);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_RoundRectDestroy(roundRect);
}

static void DrawRoundRect(
    OH_Drawing_Canvas* canvas, TestRect testRect, float xRad, float yRad)
{
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(testRect.left, testRect.top, testRect.right, testRect.bottom);
    OH_Drawing_RoundRect* roundRect = OH_Drawing_RoundRectCreate(rect, xRad, yRad);
    OH_Drawing_CanvasDrawRoundRect(canvas, roundRect);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_RoundRectDestroy(roundRect);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_ROUND_RECT_0201
@Description:OH_Drawing_RoundRectOffset接口绘制图形一半在画布内，一半在画布外
             OH_Drawing_RoundRectOffset接口绘制图形全部在画布外
@Step:
    1、demo中第二个选择框覆盖所有模式（cpu-xcomponent、gpu-xcomponent、gpu-xnode），Testcase中输入roundrectoffset，点击draw按钮
@Result:
    1、demo打开正常不会闪退，无crash产生，具体效果参考备注中预期图片（图片名称和testcase名称保持一致）
 */

void FunctionRoundRectOffset::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("FunctionRoundRectOffset OnTestFunction");
    DrawRoundRectOffset(canvas, {-50, 30, 150, 80}, 0, 0);      // reference, -50, 30, 150, 80, 0, 0 for test;
    DrawRoundRectOffset(canvas, {300, 100, 400, 200}, 0, 0);    // reference,  300, 100, 400, 200, 0, 0 for test
    DrawRoundRectOffset(canvas, {-50, 100, 150, 200}, 20, 30);  // -50, 100, 150, 200, 20, 30 for test
    DrawRoundRectOffset(canvas, {100, -100, 300, -10}, 20, 30); // 100, -100, 300, -10, 20, 30 for test
}

/*
@CaseID:SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_ROUND_RECT_0301
@Description:OH_Drawing_RoundRectCreate接口异常入参验证传入正方形，参数2,3设置超过正方形边长一半
             OH_Drawing_RoundRectCreate接口正常入参验证传入长方形，参数2,3设置20,20
             OH_Drawing_RoundRectCreate接口异常入参验证传入长方形，参数2,3设置超过最短边长一半
             OH_Drawing_RoundRectCreate接口正常入参验证传入长方形，参数2,3设置20,80
             OH_Drawing_RoundRectCreate接口绘制图形一半在画布内，一半在画布外
             OH_Drawing_RoundRectCreate接口绘制图形全部在画布外
@Step:
    1、demo中第二个选择框覆盖所有模式（cpu-xcomponent、gpu-xcomponent、gpu-xnode），Testcase中输入roundrectoffset，点击draw按钮
@Result:
    1、demo打开正常不会闪退，无crash产生，具体效果参考备注中预期图片（图片名称和testcase名称保持一致）
 */
void FunctionRoundRectCreate::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("RoundRectCreate OnTestFunction");
    // 100, 470, 200, 570, 60, 60; for test first
    DrawRoundRect(canvas, {100, 470, 200, 570}, 60, 60);
    // 100, 360, 300, 460, 20, 20; for test second
    DrawRoundRect(canvas, {100, 360, 300, 460}, 20, 20);
    // 100, 250, 300, 350, 80, 80; for test third
    DrawRoundRect(canvas, {100, 250, 300, 350}, 80, 80);
    // 100, 100, 300, 200, 20, 80; for test forth
    DrawRoundRect(canvas, {100, 100, 300, 200}, 20, 80);
    // -100, -100, 100, 100, 5, 5; for test fifth
    DrawRoundRect(canvas, {-100, -100, 100, 100}, 5, 5);
    // -100, -100, -20, -20, 5, 5; for test sixth
    DrawRoundRect(canvas, {-100, -100, -20, -20}, 5, 5);
}
