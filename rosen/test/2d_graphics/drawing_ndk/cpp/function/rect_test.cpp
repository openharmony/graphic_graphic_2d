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

#include "rect_test.h"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_filter.h>
#include <native_drawing/drawing_font.h>
#include <native_drawing/drawing_image.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_path_effect.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_region.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_sampling_options.h>
#include <native_drawing/drawing_shader_effect.h>
#include <native_drawing/drawing_text_blob.h>

#include "interface/canvas_test.h"
#include "test_common.h"

#include "common/log_common.h"
#include "common/log_draw.h"

struct TestRect {
    float left;
    float top;
    float right;
    float bottom;
};
static void DrawRect(OH_Drawing_Canvas* canvas, float left, float top, float right, float bottom)
{
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(0, 0, 0, 0);
    OH_Drawing_RectSetLeft(rect, left);
    OH_Drawing_RectSetTop(rect, top);
    OH_Drawing_RectSetRight(rect, right);
    OH_Drawing_RectSetBottom(rect, bottom);
    OH_Drawing_CanvasDrawRect(canvas, rect);
    int32_t space = 30; // 30 for test
    DRAWING_DRAW_TEXT_BLOB(canvas, 0, std::max(top, bottom) + space, "FunctionRectSetAndGet set left=%.1f,"
    "get left=%.1f;set top=%.1f,get top=%.1f;", left, OH_Drawing_RectGetLeft(rect), top, OH_Drawing_RectGetTop(rect));
    DRAWING_DRAW_TEXT_BLOB(canvas,  0, std::max(top, bottom) + space + space, "set right=%.1f,get right=%.1f,"
        "set bottom=%.1f, get bottom=%.1f;width=%.1f,height=%.1f", right, OH_Drawing_RectGetRight(rect),
        bottom, OH_Drawing_RectGetBottom(rect), OH_Drawing_RectGetWidth(rect),
        OH_Drawing_RectGetHeight(rect));
    OH_Drawing_RectDestroy(rect);
}
static bool DrawRectIntersect(OH_Drawing_Canvas* canvas, struct TestRect testRect1, struct TestRect testRect2)
{
    OH_Drawing_Rect* rect1 = OH_Drawing_RectCreate(testRect1.left, testRect1.top, testRect1.right, testRect1.bottom);
    OH_Drawing_Rect* rect2 = OH_Drawing_RectCreate(testRect2.left, testRect2.top, testRect2.right, testRect2.bottom);
    bool ret = OH_Drawing_RectIntersect(rect1, rect2);
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetColor(pen, 0xFF00FFFF);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_CanvasDrawRect(canvas, rect2);
    OH_Drawing_PenSetColor(pen, 0xFFFF00FF);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_CanvasDrawRect(canvas, rect1);
    OH_Drawing_PenDestroy(pen);
    OH_Drawing_RectDestroy(rect1);
    OH_Drawing_RectDestroy(rect2);
    return ret;
}

/*
@CaseID:SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_RECT_0201
@Description:OH_Drawing_RectCopy一半画布内，一半画布外
@Step:
    1、demo中第二个选择框覆盖所有模式（cpu-xcomponent、gpu-xcomponent、gpu-xnode）Testcase中输入，Testcase中输入rectcopy，点击draw按钮
@Result:
    1、demo打开正常不会闪退，无crash产生，一个蓝色矩形，具体效果参考备注中预期图片（图片名称和testcase名称保持一致）
 */
void FunctionRectCopy::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("FunctionRectCopy OnTestFunction");
    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(-10, -50, 200, 180); // -10, -50, 200, 200 for test
    OH_Drawing_Rect* rect1 =
        OH_Drawing_RectCreate(300, 200, 400, 100); // 300, 200, 400, 100 for test, will be override by rect
    OH_Drawing_RectCopy(rect, rect1);
    OH_Drawing_CanvasDrawRect(canvas, rect1);
    // 20 space
    DRAWING_DRAW_TEXT_BLOB(canvas, 0, OH_Drawing_RectGetBottom(rect) + 20, "FunctionRectCopy left=%.1f,top=%.1f,"
        "right=%.1f,bottom=%.1f", OH_Drawing_RectGetLeft(rect), OH_Drawing_RectGetTop(rect),
        OH_Drawing_RectGetRight(rect), OH_Drawing_RectGetBottom(rect));
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_RectDestroy(rect1);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_RECT_0301
@Description:OH_Drawing_RectGetWidth接口绘制图形，一半画布内，一半画布外
             OH_Drawing_RectGetWidth接口绘制图形全部画布外
             OH_Drawing_RectGetHeight接口绘制图形，一半画布内，一半画布外
             OH_Drawing_RectGetHeight接口绘制图形全部画布外
             OH_Drawing_RectGetBottom接口绘制图形，一半画布内，一半画布外
             OH_Drawing_RectGetBottom接口绘制图形全部画布外
             OH_Drawing_RectGetRight接口绘制图形，一半画布内，一半画布外
             OH_Drawing_RectGetRight接口绘制图形全部画布外
             OH_Drawing_RectGetTop接口绘制图形，一半画布内，一半画布外
             OH_Drawing_RectGetTop接口绘制图形全部画布外
             OH_Drawing_RectGetLeft接口绘制图形，一半画布内，一半画布外
             OH_Drawing_RectGetLeft接口绘制图形全部画布外
             OH_Drawing_RectSetBottom接口绘制图形，一半画布内，一半画布外
             OH_Drawing_RectSetBottom接口绘制图形全部画布外
             OH_Drawing_RectSetRight接口绘制图形，一半画布内，一半画布外
             OH_Drawing_RectSetRight接口绘制图形全部画布外
             OH_Drawing_RectSetTop接口绘制图形，一半画布内，一半画布外
             OH_Drawing_RectSetTop接口绘制图形全部画布外
             OH_Drawing_RectSetLeft接口绘制图形，一半画布内，一半画布外
             OH_Drawing_RectSetLeft接口绘制图形全部画布外
@Step:
    1、demo中第二个选择框覆盖所有模式（cpu-xcomponent、gpu-xcomponent、gpu-xnode）Testcase中输入，Testcase中输入rectsetleft，点击draw按钮
@Result:
    1、demo打开正常不会闪退，无crash产生，，具体效果参考备注中预期图片（图片名称和testcase名称保持一致，查看对应log正确）
 */
void FunctionRectSetAndGet::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("FunctionRectSetAndGet OnTestFunction");
    OH_Drawing_CanvasDetachBrush(canvas);
    DrawRect(canvas, -50, -30, 100, 100);  //-50,-30,100,100 test, rect part in canvas
    DrawRect(canvas, -30, 200, -100, 300); //-30,200,-100,300 test, rect out of canvas
}

/*
@CaseID:SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_RECT_1301
@Description:OH_Drawing_RectIntersect接口正常入参验证（大矩形包含小矩形，参数1，2互换验证)
             OH_Drawing_RectIntersect接口正常入参验证（左右相交判断，上下相交判断）
             OH_Drawing_RectIntersect接口正常入参验证（rect的left >right 的情况判断，top >bottom的情况判断）
             OH_Drawing_RectIntersect接口正常入参验证（rect1.left = rect2.right情况判断,rect1.top = rect2.bottom情况判断）
@Step:
    1、demo中第二个选择框覆盖所有模式（cpu-xcomponent、gpu-xcomponent、gpu-xnode）Testcase中输入，Testcase中输入rectintersect，点击draw按钮
@Result:
    1、demo打开正常不会闪退，无crash产生，4个正方形，日志打印wybrect2 and rect3 RectIntersect is true 具体效果参考备注中预期图片（图片名称和testcase名称保持一致）
 */
void FunctionRectIntersect::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("FunctionRectIntersect OnTestFunction");
    float space = 5;       // 5 space for test
    float left = 10;       // 10 left for test
    float itemWidth = 100; // 100 itemHeight for test
    float right = left + itemWidth;
    float top = space;      // 10 left for test
    float itemHeight = 100; // 100 itemHeight for test
    float bottom = top + itemHeight;
    bool res = DrawRectIntersect(
        canvas, { left, top, right, bottom }, { left + space, top + space, right - space, bottom - space });
    DRAWING_LOGI(
        "FunctionRectIntersect:A large rectangle contains a small rectangle%{public}s", res ? "true" : "false");
    DRAWING_DRAW_TEXT_BLOB(
        canvas, right, bottom, "A large rectangle contains a small rectangle:%s", res ? "true" : "false");
    top = bottom + space;
    bottom = top + itemHeight;
    res = DrawRectIntersect(
        canvas, { left + space, top + space, right - space, bottom - space }, { left, top, right, bottom });
    DRAWING_LOGI(
        "FunctionRectIntersect:A large rectangle contains a small rectangle%{public}s", res ? "true" : "false");
    DRAWING_DRAW_TEXT_BLOB(
        canvas, right, bottom, "A large rectangle contains a small rectangle:%s", res ? "true" : "false");
    top = bottom + space;
    bottom = top + itemHeight;
    res = DrawRectIntersect(canvas, { left, top, right, bottom }, { right - space, top, right + itemWidth, bottom });
    DRAWING_LOGI(
        "FunctionRectIntersect:Intersection judgment between left and right%{public}s", res ? "true" : "false");
    DRAWING_DRAW_TEXT_BLOB(canvas, left, bottom + space + space, "Intersection judgment between left and right:%s",
        res ? "true" : "false");
    top = bottom + space;
    bottom = top + itemHeight;
    res = DrawRectIntersect(canvas, { left, top, right, bottom }, { left, bottom - space, right, bottom + itemHeight });
    DRAWING_LOGI("FunctionRectIntersect:Intersection judgment of up and down%{public}s", res ? "true" : "false");
    DRAWING_DRAW_TEXT_BLOB(
        canvas, left, bottom - space - space, "Intersection judgment of up and down:%s", res ? "true" : "false");
    left = this->GetBitmapWidth() / 2; // 2, get middle of bitmap width, for test
    right = left + itemWidth;
    top = space;
    bottom = top + itemHeight;
    res = DrawRectIntersect(canvas, { right, top, left, bottom }, { left + space, top, right - space, bottom });
    DRAWING_LOGI("FunctionRectIntersect:left >right ?%{public}s", res ? "true" : "false");
    DRAWING_DRAW_TEXT_BLOB(canvas, left, bottom + space + space, "left >right ?:%s", res ? "true" : "false");
    top = bottom + space + itemHeight;
    bottom = top + itemHeight;
    res = DrawRectIntersect(canvas, { left, bottom, right, top }, { left, top + space, right, bottom - space });
    DRAWING_LOGI("FunctionRectIntersect:top >bottom ?%{public}s", res ? "true" : "false");
    DRAWING_DRAW_TEXT_BLOB(canvas, left, bottom + space + space, "top >bottom ?:%s", res ? "true" : "false");
}

/*
@CaseID:SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_REGION_0401
@Description:OH_Drawing_RegionSetRect接口rect设置一半画布内一半画布外
@Step:
    1、demo中第二个选择框覆盖所有模式（cpu-xcomponent、gpu-xcomponent、gpu-xnode）Testcase中输入，Testcase中输入regionsetrecterr1，点击draw按钮
@Result:
    1、demo打开正常不会闪退，无crash产生，
 */
void FunctionRegionSetRect::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("FunctionRegionSetRect OnTestFunction");
    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(-50, 0, 150, 200); // -50, 0, 150, 200 for test,part out of canvas
    OH_Drawing_Region* region = OH_Drawing_RegionCreate();
    OH_Drawing_RegionSetRect(region, rect);
    OH_Drawing_CanvasDrawRegion(canvas, region);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_RegionDestroy(region);
}
