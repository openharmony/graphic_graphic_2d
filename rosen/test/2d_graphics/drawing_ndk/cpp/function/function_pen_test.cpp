/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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