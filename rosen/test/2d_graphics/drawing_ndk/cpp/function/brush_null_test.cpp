/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "brush_null_test.h"

#include <cstddef>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_filter.h>
#include <native_drawing/drawing_font.h>
#include <native_drawing/drawing_image.h>
#include <native_drawing/drawing_mask_filter.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_path_effect.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_color_space_manager/native_color_space_manager.h>
#include "test_common.h"

#include "common/log_common.h"
/*
@CaseID:SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_BRUSH_1001
@Description:OH_Drawing_BrushSetColor接口color传null
@Step:
    1、demo中第二个选择框选择（cpu-xcomponent、gpu-xcomponent、gpu-xnode）Testcase中输入functionBrushSetColor，点击draw按钮
@Result:
    1、页面无绘制
 */
void FunctionBrushSetColor::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("FunctionBrushSetColor OnTestFunction");
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_BrushSetColor(brush, NULL);
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(0, 0, 200, 200); // 200, 200 for test
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    OH_Drawing_CanvasDrawRect(canvas, rect);
    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_RectDestroy(rect);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_BRUSH_1301
@Description:OH_Drawing_BrushSetShaderEffect参数shaderEffect传null
@Step:
    1、demo中第二个选择框选择（cpu-xcomponent、gpu-xcomponent、gpu-xnode）Testcase中输入functionBrushSetShaderEffect，点击draw按钮
@Result:
    1、画布上出现1个(400, 400, 500, 500)绿色的矩形
 */
void FunctionBrushSetShaderEffect::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("FunctionBrushSetShaderEffect OnTestFunction");
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_BrushSetShaderEffect(brush, NULL);
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(400, 400, 500, 500); // 400, 400, 500, 500 for test
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    OH_Drawing_CanvasDrawRect(canvas, rect);
    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_RectDestroy(rect);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_BRUSH_1401
@Description:OH_Drawing_BrushSetColor4f参数a/r/g/b传值
@Step:
    1、demo中第二个选择框选择（cpu-xcomponent、gpu-xcomponent、gpu-xnode）Testcase中输入functionBrushSetColor4f，点击draw按钮
@Result:
    1、demo打开正常不会闪退，无crash产生，一个矩形
 */
void FunctionBrushSetColor4f::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("FunctionBrushSetColor4f OnTestFunction");
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    float a = 0.1f;
    float r = 1.0f;
    float g = 0.4f;
    float b = 0.2f;
    OH_NativeColorSpaceManager* colorSpaceManager = OH_NativeColorSpaceManager_CreateFromName(ColorSpaceName::DCI_P3);
    OH_Drawing_ErrorCode error = OH_Drawing_BrushSetColor4f(brush, a, r, g, b, colorSpaceManager);
    DRAWING_LOGI("error = %{public}d", error);
    OH_Drawing_ErrorCode error2 = OH_Drawing_BrushSetColor4f(brush, 0.2f, 0.4f, 1.0f, 0.3f, nullptr);
    DRAWING_LOGI("error2 = %{public}d", error2);
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(100, 100, 500, 500); // 100, 100, 500, 500 for test
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    OH_Drawing_CanvasDrawRect(canvas, rect);
    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_BrushDestroy(brush);
    OH_NativeColorSpaceManager_Destroy(colorSpaceManager);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_BRUSH_1402
@Description:OH_Drawing_BrushGetAlphaFloat接口a传null
@Step:
    1、demo中第二个选择框选择（cpu-xcomponent、gpu-xcomponent、gpu-xnode）Testcase中输入functionBrushSetColor4f，点击draw按钮
@Result:
    1、demo打开正常不会闪退，无crash产生，一个矩形
 */
void FunctionBrushGetAlphaFloat::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("FunctionBrushGetAlphaFloat OnTestFunction");
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_ErrorCode error = OH_Drawing_BrushGetAlphaFloat(brush, NULL);
    DRAWING_LOGI("error = %{public}d", error);
    OH_Drawing_BrushSetColor4f(brush, 0.2f, 0.4f, 1.0f, 0.3f, nullptr);
    float a = 128.f;
    OH_Drawing_ErrorCode error2 = OH_Drawing_BrushGetAlphaFloat(brush, &a);
    DRAWING_LOGI("error2 = %{public}d", error2);
    if (abs(a - 0.2f) < DRAW_FLOAT_MIN) {
        OH_Drawing_Rect* rect = OH_Drawing_RectCreate(100, 100, 500, 500); // 100, 100, 500, 500 for test
        OH_Drawing_CanvasAttachBrush(canvas, brush);
        OH_Drawing_CanvasDrawRect(canvas, rect);
        OH_Drawing_CanvasDetachBrush(canvas);
        OH_Drawing_RectDestroy(rect);
    }
    OH_Drawing_BrushDestroy(brush);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_BRUSH_1403
@Description:OH_Drawing_BrushGetRedFloat接口r传null
@Step:
    1、demo中第二个选择框选择（cpu-xcomponent、gpu-xcomponent、gpu-xnode）Testcase中输入functionBrushSetColor4f，点击draw按钮
@Result:
    1、demo打开正常不会闪退，无crash产生，一个矩形
 */
void FunctionBrushGetRedFloat::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("FunctionBrushGetRedFloat OnTestFunction");
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_ErrorCode error = OH_Drawing_BrushGetRedFloat(brush, NULL);
    DRAWING_LOGI("error = %{public}d", error);
    OH_Drawing_BrushSetColor4f(brush, 0.2f, 0.4f, 1.0f, 0.3f, nullptr);
    float r = 128.f;
    OH_Drawing_ErrorCode error2 = OH_Drawing_BrushGetRedFloat(brush, &r);
    DRAWING_LOGI("error2 = %{public}d", error2);
    if (abs(r - 0.4f) < DRAW_FLOAT_MIN) {
        OH_Drawing_Rect* rect = OH_Drawing_RectCreate(100, 100, 500, 500); // 100, 100, 500, 500 for test
        OH_Drawing_CanvasAttachBrush(canvas, brush);
        OH_Drawing_CanvasDrawRect(canvas, rect);
        OH_Drawing_CanvasDetachBrush(canvas);
        OH_Drawing_RectDestroy(rect);
    }
    OH_Drawing_BrushDestroy(brush);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_BRUSH_1404
@Description:OH_Drawing_BrushGetBlueFloat接口b传null
@Step:
    1、demo中第二个选择框选择（cpu-xcomponent、gpu-xcomponent、gpu-xnode）Testcase中输入functionBrushSetColor4f，点击draw按钮
@Result:
    1、demo打开正常不会闪退，无crash产生，一个矩形
 */
void FunctionBrushGetBlueFloat::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("FunctionBrushGetBlueFloat OnTestFunction");
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_ErrorCode error = OH_Drawing_BrushGetBlueFloat(brush, NULL);
    DRAWING_LOGI("error = %{public}d", error);
    float b = 128.f;
    OH_Drawing_BrushSetColor4f(brush, 0.2f, 0.4f, 1.0f, 0.3f, nullptr);
    OH_Drawing_ErrorCode error2 = OH_Drawing_BrushGetBlueFloat(brush, &b);
    DRAWING_LOGI("error2 = %{public}d", error2);
    if (abs(b - 0.3f) < DRAW_FLOAT_MIN) {
        OH_Drawing_Rect* rect = OH_Drawing_RectCreate(100, 100, 500, 500); // 100, 100, 500, 500 for test
        OH_Drawing_CanvasAttachBrush(canvas, brush);
        OH_Drawing_CanvasDrawRect(canvas, rect);
        OH_Drawing_CanvasDetachBrush(canvas);
        OH_Drawing_RectDestroy(rect);
    }
    OH_Drawing_BrushDestroy(brush);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_BRUSH_1405
@Description:OH_Drawing_BrushGetGreenFloat接口g传null
@Step:
    1、demo中第二个选择框选择（cpu-xcomponent、gpu-xcomponent、gpu-xnode）Testcase中输入functionBrushSetColor4f，点击draw按钮
@Result:
    1、demo打开正常不会闪退，无crash产生，一个矩形
 */
void FunctionBrushGetGreenFloat::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("FunctionBrushGetGreenFloat OnTestFunction");
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_ErrorCode error = OH_Drawing_BrushGetGreenFloat(brush, NULL);
    DRAWING_LOGI("error = %{public}d", error);
    float g = 128.f;
    OH_Drawing_BrushSetColor4f(brush, 0.2f, 0.4f, 1.0f, 0.3f, nullptr);
    OH_Drawing_ErrorCode error2 = OH_Drawing_BrushGetGreenFloat(brush, &g);
    DRAWING_LOGI("error2 = %{public}d", error2);
    if (abs(g - 1.0f) < DRAW_FLOAT_MIN) {
        OH_Drawing_Rect* rect = OH_Drawing_RectCreate(100, 100, 500, 500); // 100, 100, 500, 500 for test
        OH_Drawing_CanvasAttachBrush(canvas, brush);
        OH_Drawing_CanvasDrawRect(canvas, rect);
        OH_Drawing_CanvasDetachBrush(canvas);
        OH_Drawing_RectDestroy(rect);
    }
    OH_Drawing_BrushDestroy(brush);
}