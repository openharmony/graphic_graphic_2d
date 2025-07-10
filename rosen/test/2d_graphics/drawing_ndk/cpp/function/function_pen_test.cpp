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

#include "function/function_pen_test.h"

#include <native_color_space_manager/native_color_space_manager.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_shader_effect.h>

#include "common/log_common.h"

const int PEN_WIDTH = 5;

/*
@CaseID:SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_PEN_2001
@Description:OH_Drawing_PenSetShaderEffect参数shaderEffect传空
@Step:
    1、demo中第二个选择框覆盖所有模式（cpu-xcomponent、
    gpu-xcomponent、gpu-xnode）Testcase中输入，Testcase中输入
    functionPathRConicToTest，点击draw按钮
@Result:
    1、demo打开正常不会闪退，无crash产生，一个青色矩形
 */
void FunctionPenSetShaderEffectNullTest::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetWidth(pen, PEN_WIDTH);
    OH_Drawing_PenSetColor(pen, 0xff00ffff);
    OH_Drawing_PenSetShaderEffect(pen, nullptr);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    float left = 100;
    float top = 100;
    float right = 200;
    float bottom = 200;
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(left, top, right, bottom);
    OH_Drawing_CanvasDrawRect(canvas, rect);
    DRAWING_LOGI("FunctionPenSetShaderEffectNullTest OnTestFunction");
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PenDestroy(pen);
    OH_Drawing_RectDestroy(rect);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_PEN_2002
@Description:OH_Drawing_PenSetColor4f参数a/r/g/b传值
@Step:
    1、demo中第二个选择框覆盖所有模式（cpu-xcomponent、
    gpu-xcomponent、gpu-xnode）Testcase中输入，Testcase中输入
    functionPathRConicToTest，点击draw按钮
@Result:
    1、demo打开正常不会闪退，无crash产生，一个矩形
 */
void FunctionPenSetColor4fNullTest::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetWidth(pen, PEN_WIDTH);
    OH_NativeColorSpaceManager* colorSpaceManager = OH_NativeColorSpaceManager_CreateFromName(ColorSpaceName::DCI_P3);
    OH_Drawing_ErrorCode error = OH_Drawing_PenSetColor4f(pen, 0.2f, 0.4f, 1.0f, 0.1f, colorSpaceManager);
    DRAWING_LOGI("error = %{public}d", error);
    OH_Drawing_ErrorCode error2 = OH_Drawing_PenSetColor4f(pen, 0.2f, 0.4f, 1.0f, 0.3f, nullptr);
    DRAWING_LOGI("error2 = %{public}d", error2);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(100, 100, 500, 500); // 100, 100, 500, 500 for test
    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_CanvasDrawRect(canvas, rect);
    DRAWING_LOGI("FunctionPenSetColor4fNullTest OnTestFunction");
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PenDestroy(pen);
    OH_Drawing_RectDestroy(rect);
    OH_NativeColorSpaceManager_Destroy(colorSpaceManager);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_PEN_2003
@Description:OH_Drawing_PenGetAlphaFloat参数a传空
@Step:
    1、demo中第二个选择框覆盖所有模式（cpu-xcomponent、
    gpu-xcomponent、gpu-xnode）Testcase中输入，Testcase中输入
    functionPathRConicToTest，点击draw按钮
@Result:
    1、demo打开正常不会闪退，无crash产生，一个矩形
 */
void FunctionPenGetAlphaFloatNullTest::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetWidth(pen, PEN_WIDTH);
    OH_Drawing_ErrorCode error = OH_Drawing_PenGetAlphaFloat(pen, NULL);
    DRAWING_LOGI("error = %{public}d", error);
    OH_Drawing_PenSetColor4f(pen, 0.2f, 0.4f, 1.0f, 0.3f, nullptr);
    float a = 128.f;
    OH_Drawing_ErrorCode error2 = OH_Drawing_PenGetAlphaFloat(pen, &a);
    DRAWING_LOGI("error2 = %{public}d", error2);
    if (abs(a - 0.2f) < DRAW_FLOAT_MIN) {
        OH_Drawing_CanvasAttachPen(canvas, pen);
        OH_Drawing_Rect* rect = OH_Drawing_RectCreate(100, 100, 500, 500); // 100, 100, 500, 500 for test
        OH_Drawing_CanvasDrawRect(canvas, rect);
        DRAWING_LOGI("FunctionPenGetAlphaFloatNullTest OnTestFunction");
        OH_Drawing_CanvasDetachPen(canvas);
        OH_Drawing_RectDestroy(rect);
    }

    OH_Drawing_PenDestroy(pen);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_PEN_2004
@Description:OH_Drawing_PenGetAlphaFloat参数g传空
@Step:
    1、demo中第二个选择框覆盖所有模式（cpu-xcomponent、
    gpu-xcomponent、gpu-xnode）Testcase中输入，Testcase中输入
    functionPathRConicToTest，点击draw按钮
@Result:
    1、demo打开正常不会闪退，无crash产生，一个矩形
 */
void FunctionPenGetGreenFloatNullTest::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetWidth(pen, PEN_WIDTH);
    OH_Drawing_ErrorCode error = OH_Drawing_PenGetGreenFloat(pen, NULL);
    DRAWING_LOGI("error = %{public}d", error);
    OH_Drawing_PenSetColor4f(pen, 0.2f, 0.4f, 1.0f, 0.3f, nullptr);
    float g = 128.f;
    OH_Drawing_ErrorCode error2 = OH_Drawing_PenGetGreenFloat(pen, &g);
    DRAWING_LOGI("error2 = %{public}d", error2);
    if (abs(g - 1.0f) < DRAW_FLOAT_MIN) {
        OH_Drawing_CanvasAttachPen(canvas, pen);
        OH_Drawing_Rect* rect = OH_Drawing_RectCreate(100, 100, 500, 500); // 100, 100, 500, 500 for test
        OH_Drawing_CanvasDrawRect(canvas, rect);
        DRAWING_LOGI("FunctionPenGetGreenFloatNullTest OnTestFunction");
        OH_Drawing_CanvasDetachPen(canvas);
        OH_Drawing_RectDestroy(rect);
    }
    OH_Drawing_PenDestroy(pen);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_PEN_2005
@Description:OH_Drawing_PenGetAlphaFloat参数r传空
@Step:
    1、demo中第二个选择框覆盖所有模式（cpu-xcomponent、
    gpu-xcomponent、gpu-xnode）Testcase中输入，Testcase中输入
    functionPathRConicToTest，点击draw按钮
@Result:
    1、demo打开正常不会闪退，无crash产生，一个矩形
 */
void FunctionPenGetRedFloatNullTest::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetWidth(pen, PEN_WIDTH);
    OH_Drawing_ErrorCode error = OH_Drawing_PenGetRedFloat(pen, NULL);
    DRAWING_LOGI("error = %{public}d", error);
    OH_Drawing_PenSetColor4f(pen, 0.2f, 0.4f, 1.0f, 0.3f, nullptr);
    float r = 128.f;
    OH_Drawing_ErrorCode error2 = OH_Drawing_PenGetRedFloat(pen, &r);
    DRAWING_LOGI("error2 = %{public}d", error2);
    if (abs(r - 0.4f) < DRAW_FLOAT_MIN) {
        OH_Drawing_CanvasAttachPen(canvas, pen);
        OH_Drawing_Rect* rect = OH_Drawing_RectCreate(100, 100, 500, 500); // 100, 100, 500, 500 for test
        OH_Drawing_CanvasDrawRect(canvas, rect);
        DRAWING_LOGI("FunctionPenGetRedFloatNullTest OnTestFunction");
        OH_Drawing_CanvasDetachPen(canvas);
        OH_Drawing_RectDestroy(rect);
    }
    OH_Drawing_PenDestroy(pen);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_PEN_2006
@Description:OH_Drawing_PenGetBlueFloat参数b传空
@Step:
    1、demo中第二个选择框覆盖所有模式（cpu-xcomponent、
    gpu-xcomponent、gpu-xnode）Testcase中输入，Testcase中输入
    functionPathRConicToTest，点击draw按钮
@Result:
    1、demo打开正常不会闪退，无crash产生，一个矩形
 */
void FunctionPenGetBlueFloatNullTest::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetWidth(pen, PEN_WIDTH);
    OH_Drawing_ErrorCode error = OH_Drawing_PenGetBlueFloat(pen, NULL);
    DRAWING_LOGI("error = %{public}d", error);
    OH_Drawing_PenSetColor4f(pen, 0.2f, 0.4f, 1.0f, 0.3f, nullptr);
    float b = 128.f;
    OH_Drawing_ErrorCode error2 = OH_Drawing_PenGetBlueFloat(pen, &b);
    DRAWING_LOGI("error2 = %{public}d", error2);
    if (abs(b - 0.3f) < DRAW_FLOAT_MIN) {
        OH_Drawing_CanvasAttachPen(canvas, pen);
        OH_Drawing_Rect* rect = OH_Drawing_RectCreate(100, 100, 500, 500); // 100, 100, 500, 500 for test
        OH_Drawing_CanvasDrawRect(canvas, rect);
        DRAWING_LOGI("FunctionPenGetBlueFloatNullTest OnTestFunction");
        OH_Drawing_CanvasDetachPen(canvas);
        OH_Drawing_RectDestroy(rect);
    }
    OH_Drawing_PenDestroy(pen);
}