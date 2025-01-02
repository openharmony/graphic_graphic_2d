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
#include "filter_test.h"

#include <cstddef>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_color_filter.h>
#include <native_drawing/drawing_filter.h>
#include <native_drawing/drawing_font.h>
#include <native_drawing/drawing_image.h>
#include <native_drawing/drawing_mask_filter.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_path_effect.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_region.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_sampling_options.h>

#include "test_common.h"

#include "common/log_common.h"
/*
@CaseID:SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_FILTER_0301
@Description:OH_Drawing_FilterSetMaskFilter参数maskFilter入参为空
@Step:
    1、demo中第二个选择框选择（cpu-xcomponent、gpu-xcomponent、gpu-xnode）Testcase中输入functionFilterSetMaskFilter，点击draw按钮
@Result:
    1、页面绘制一个绿色的矩形
 */
void FunctionFilterSetMaskFilter::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("FunctionFilterSetMaskFilter OnTestFunction");
    float sigma = 10.f; // 10.f for test
    OH_Drawing_MaskFilter* maskFilter = OH_Drawing_MaskFilterCreateBlur(NORMAL, sigma, true);
    OH_Drawing_Filter* filter = OH_Drawing_FilterCreate();
    OH_Drawing_FilterSetMaskFilter(filter, NULL);
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_BrushSetColor(brush, 0xFF00FF00);
    OH_Drawing_BrushSetFilter(brush, filter);
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(0, 0, 100, 100); // 100, 100 for test
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    OH_Drawing_CanvasDrawRect(canvas, rect);
    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_MaskFilterDestroy(maskFilter);
    OH_Drawing_FilterDestroy(filter);
    OH_Drawing_BrushDestroy(brush);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_FILTER_0401
@Description:OH_Drawing_FilterSetColorFilter参数colorFilter入参为空
@Step:
    1、demo中第二个选择框选择（cpu-xcomponent、gpu-xcomponent、gpu-xnode）Testcase中输入functionFilterSetColorFilter，点击draw按钮
@Result:
    1、页面绘制一个绿色的矩形
 */
void FunctionFilterSetColorFilter::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("FunctionFilterSetColorFilter OnTestFunction");
    OH_Drawing_ColorFilter* colorFilter =
        OH_Drawing_ColorFilterCreateBlendMode(DRAW_COLORBLUE, OH_Drawing_BlendMode::BLEND_MODE_CLEAR);
    OH_Drawing_Filter* filter = OH_Drawing_FilterCreate();
    OH_Drawing_FilterSetColorFilter(filter, NULL);
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_BrushSetColor(brush, 0xFF00FF00);
    OH_Drawing_BrushSetFilter(brush, filter);
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(0, 0, 100, 100); // 100, 100 for test
    OH_Drawing_CanvasAttachBrush(canvas, brush);
    OH_Drawing_CanvasDrawRect(canvas, rect);
    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_ColorFilterDestroy(colorFilter);
    OH_Drawing_FilterDestroy(filter);
    OH_Drawing_BrushDestroy(brush);
}