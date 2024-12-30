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