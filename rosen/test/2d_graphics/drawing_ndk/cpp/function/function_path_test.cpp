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

#include "function/function_path_test.h"

#include <cstdint>
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_types.h>

#include "common/log_common.h"

const int PEN_WIDTH = 5;

/*
@CaseID:SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_PATH_3301
@Description:
    1、OH_Drawing_PathAddRectWithInitialCorner接口正常入參验证第三个参数为PATH_DIRECTION_CCW，第四个参数设置为左上角1
    2、OH_Drawing_PathAddRectWithInitialCorner接口正常入參验证第三个参数为PATH_DIRECTION_CCW，第四个参数设置为左上角2
    3、OH_Drawing_PathAddRectWithInitialCorner接口正常入參验证第三个参数为PATH_DIRECTION_CCW，第四个参数设置为左上角3
    4、OH_Drawing_PathAddRectWithInitialCorner接口正常入參验证第三个参数为PATH_DIRECTION_CW
    5、OH_Drawing_PathAddRectWithInitialCorner接口绘制图形，一半画布内，一半画布外
    6、OH_Drawing_PathAddRectWithInitialCorner接口绘制图形，全部画布外
@Step:
    1、demo中第二个选择框覆盖所有模式（cpu-xcomponent、
    gpu-xcomponent、gpu-xnode）Testcase中输入，Testcase中输入
    functionPathAddRectWithInitialCornerTest，点击draw按钮
@Result:
    1、demo打开正常不会闪退，无crash产生，4个100*100的黑色正方形，1个一半在画布外的黑色正方形
 */
void FunctionPathAddRectWithInitialCornerTest::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetWidth(pen, PEN_WIDTH);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    // OH_Drawing_PathAddRectWithInitialCorner接口正常入參验证第三个参数为PATH_DIRECTION_CCW，第四个参数设置为1
    OH_Drawing_Rect* rect1 = OH_Drawing_RectCreate(0, 0, 100, 100); // 100, 100 for test
    OH_Drawing_PathAddRectWithInitialCorner(path, rect1, PATH_DIRECTION_CCW, 1); // 1 for test
    OH_Drawing_CanvasDrawPath(canvas, path);
    // OH_Drawing_PathAddRectWithInitialCorner接口正常入參验证第三个参数为PATH_DIRECTION_CCW，第四个参数设置为2
    OH_Drawing_PathReset(path);
    OH_Drawing_Rect* rect2 = OH_Drawing_RectCreate(150, 0, 250, 100); // 150, 0, 250, 100 for test
    OH_Drawing_PathAddRectWithInitialCorner(path, rect2, PATH_DIRECTION_CCW, 2); // 2 for test
    OH_Drawing_CanvasDrawPath(canvas, path);
    // OH_Drawing_PathAddRectWithInitialCorner接口正常入參验证第三个参数为PATH_DIRECTION_CCW，第四个参数设置为3
    OH_Drawing_PathReset(path);
    OH_Drawing_Rect* rect3 = OH_Drawing_RectCreate(0, 150, 100, 250); // 0, 150, 100, 250 for test
    OH_Drawing_PathAddRectWithInitialCorner(path, rect3, PATH_DIRECTION_CCW, 3); // 3 for test
    OH_Drawing_CanvasDrawPath(canvas, path);
    // OH_Drawing_PathAddRectWithInitialCorner接口正常入參验证第三个参数为PATH_DIRECTION_CW
    OH_Drawing_PathReset(path);
    OH_Drawing_Rect* rect4 = OH_Drawing_RectCreate(150, 150, 250, 250); // 150, 150, 250, 250 for test
    OH_Drawing_PathAddRectWithInitialCorner(path, rect4, PATH_DIRECTION_CW, 1);
    OH_Drawing_CanvasDrawPath(canvas, path);
    // OH_Drawing_PathAddRectWithInitialCorner接口绘制图形，一半画布内，一半画布外
    OH_Drawing_PathReset(path);
    OH_Drawing_Rect* rect5 = OH_Drawing_RectCreate(-50, 300, 50, 400); // -50, 300, 50, 400 for test
    OH_Drawing_PathDirection pathDirection = PATH_DIRECTION_CCW;
    OH_Drawing_PathAddRectWithInitialCorner(path, rect5, pathDirection, 1);
    OH_Drawing_CanvasDrawPath(canvas, path);
    // OH_Drawing_PathAddRectWithInitialCorner接口绘制图形，全部画布外
    OH_Drawing_PathReset(path);
    OH_Drawing_Rect* rect6 = OH_Drawing_RectCreate(-200, -200, -100, -100); // -200, -200, -100, -100 for test
    OH_Drawing_PathAddRectWithInitialCorner(path, rect6, PATH_DIRECTION_CCW, 1);
    OH_Drawing_CanvasDrawPath(canvas, path);
    DRAWING_LOGI("FunctionPathAddRectWithInitialCornerTest OnTestFunction");
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PenDestroy(pen);
    OH_Drawing_PathDestroy(path);
    OH_Drawing_RectDestroy(rect1);
    OH_Drawing_RectDestroy(rect2);
    OH_Drawing_RectDestroy(rect3);
    OH_Drawing_RectDestroy(rect4);
    OH_Drawing_RectDestroy(rect5);
    OH_Drawing_RectDestroy(rect6);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_PATH_3401
@Description:
    1、OH_Drawing_PathRCubicTo接口绘制图形，一半画布内，一半画布外
    2、OH_Drawing_PathRCubicTo接口绘制图形，全部画布外
@Step:
    1、demo中第二个选择框覆盖所有模式（cpu-xcomponent、
    gpu-xcomponent、gpu-xnode）Testcase中输入，Testcase中输入
    functionPathRCubicToTest，点击draw按钮
@Result:
    1、demo打开正常不会闪退，无crash产生，1条红色曲线
 */
void FunctionPathRCubicToTest::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetWidth(pen, PEN_WIDTH);
    OH_Drawing_PenSetColor(pen, DRAW_COLORRED);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    // OH_Drawing_PathRCubicTo接口绘制图形，一半画布内，一半画布外
    OH_Drawing_PathRCubicTo(path, -200, -200, -150, -150, 200, 200); //-200, -200, -150, -150, 200, 200  for test
    OH_Drawing_CanvasDrawPath(canvas, path);
    // OH_Drawing_PathRCubicTo接口绘制图形，全部画布外
    OH_Drawing_PathReset(path);
    OH_Drawing_PathRCubicTo(path, -200, -200, -130, -130, -10, -10); // -200, -200, -130, -130, -10, -10 for test
    OH_Drawing_CanvasDrawPath(canvas, path);
    DRAWING_LOGI("FunctionPathRCubicToTest OnTestFunction");
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PenDestroy(pen);
    OH_Drawing_PathDestroy(path);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_PATH_3501
@Description:
    1、OH_Drawing_PathRConicTo接口正常入参验证第六个参数是小于等于0的数
    2、OH_Drawing_PathRConicTo接口正常入参验证第六个参数是1
    3、OH_Drawing_PathRConicTo接口绘制图形，一半画布内，一半画布外
    4、OH_Drawing_PathRConicTo接口绘制图形，全部画布外
@Step:
    1、demo中第二个选择框覆盖所有模式（cpu-xcomponent、
    gpu-xcomponent、gpu-xnode）Testcase中输入，Testcase中输入
    functionPathRConicToTest，点击draw按钮
@Result:
    1、demo打开正常不会闪退，无crash产生，3条青色曲线
 */
void FunctionPathRConicToTest::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetWidth(pen, PEN_WIDTH);
    OH_Drawing_PenSetColor(pen, 0xff00ffff);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    // OH_Drawing_PathRConicTo接口正常入参验证第六个参数是小于等于0的数
    OH_Drawing_PathRConicTo(path, 100, 100, 200, 150, 0); // 100, 100, 200, 150, 0 for test
    OH_Drawing_CanvasDrawPath(canvas, path);
    // OH_Drawing_PathRConicTo接口正常入参验证第六个参数是1
    OH_Drawing_PathReset(path);
    OH_Drawing_PathRConicTo(path, 100, 200, 200, 250, 1); // 100, 200, 200, 250, 1 for test
    OH_Drawing_CanvasDrawPath(canvas, path);
    // OH_Drawing_PathRConicTo接口绘制图形，一半画布内，一半画布外
    OH_Drawing_PathReset(path);
    OH_Drawing_PathRConicTo(path, -100, -100, 80, 100, 1); // -100, -100, 80, 100, 1 for test
    OH_Drawing_CanvasDrawPath(canvas, path);
    // OH_Drawing_PathRConicTo接口绘制图形，全部画布外
    OH_Drawing_PathReset(path);
    OH_Drawing_PathRConicTo(path, -200, -200, -150, -100, 1); // -200, -200, -150, -100, 1 for test
    OH_Drawing_CanvasDrawPath(canvas, path);
    DRAWING_LOGI("FunctionPathRConicToTest OnTestFunction");
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PenDestroy(pen);
    OH_Drawing_PathDestroy(path);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_PATH_3601
@Description:
    1、OH_Drawing_PathRQuadTo接口绘制图形，一半画布内，一半画布外
    2、OH_Drawing_PathRQuadTo接口绘制图形，全部画布外
@Step:
    1、demo中第二个选择框覆盖所有模式（cpu-xcomponent、
    gpu-xcomponent、gpu-xnode）Testcase中输入，Testcase中输入
    functionPathRQuadToTest，点击draw按钮
@Result:
    1、demo打开正常不会闪退，无crash产生，1条青色曲线
 */
void FunctionPathRQuadToTest::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetWidth(pen, PEN_WIDTH);
    OH_Drawing_PenSetColor(pen, 0xff00ffff);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    // OH_Drawing_PathRQuadTo接口绘制图形，一半画布内，一半画布外
    OH_Drawing_PathRQuadTo(path, -200, -200, 150, 100); // -200, -200, 150, 100 for test
    OH_Drawing_CanvasDrawPath(canvas, path);
    // OH_Drawing_PathRQuadTo接口绘制图形，全部画布外
    OH_Drawing_PathReset(path);
    OH_Drawing_PathRQuadTo(path, -300, -300, -150, -100); // -300, -300, -150, -100 for test
    OH_Drawing_CanvasDrawPath(canvas, path);
    DRAWING_LOGI("FunctionPathRQuadToTest OnTestFunction");
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PenDestroy(pen);
    OH_Drawing_PathDestroy(path);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_PATH_3701
@Description:
    1、OH_Drawing_PathRLineTo接口绘制图形，一半画布内，一半画布外
    2、OH_Drawing_PathRLineTo接口绘制图形，全部画布外
@Step:
    1、demo中第二个选择框覆盖所有模式（cpu-xcomponent、
    gpu-xcomponent、gpu-xnode）Testcase中输入，Testcase中输入
    functionPathRLineToTest，点击draw按钮
@Result:
    1、demo打开正常不会闪退，无crash产生，1条青色直线
 */
void FunctionPathRLineToTest::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetWidth(pen, PEN_WIDTH);
    OH_Drawing_PenSetColor(pen, 0xff00ffff);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    // OH_Drawing_PathRLineTo接口绘制图形，一半画布内，一半画布外
    OH_Drawing_PathRMoveTo(path, -100, -100); // -100, -100 for test
    OH_Drawing_PathRLineTo(path, 200, 200);   // 200, 200 for test
    OH_Drawing_CanvasDrawPath(canvas, path);
    // OH_Drawing_PathRLineTo接口绘制图形，全部画布外
    OH_Drawing_PathReset(path);
    OH_Drawing_PathRMoveTo(path, -200, -200); // -200, -200 for test
    OH_Drawing_PathRLineTo(path, -100, -100); // -100, -100 for test
    OH_Drawing_CanvasDrawPath(canvas, path);
    DRAWING_LOGI("FunctionPathRLineToTest OnTestFunction");
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PenDestroy(pen);
    OH_Drawing_PathDestroy(path);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_PATH_3801
@Description:
    1、OH_Drawing_PathRMoveTo接口绘制图形，一半画布内，一半画布外
    2、OH_Drawing_PathRMoveTo接口绘制图形，全部画布外
@Step:
    1、demo中第二个选择框覆盖所有模式（cpu-xcomponent、
    gpu-xcomponent、gpu-xnode）Testcase中输入，Testcase中输入
    functionPathRMoveToTest，点击draw按钮
@Result:
    1、demo打开正常不会闪退，无crash产生，1条青色直线
 */
void FunctionPathRMoveToTest::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetWidth(pen, PEN_WIDTH);
    OH_Drawing_PenSetColor(pen, 0xff00ffff);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    // OH_Drawing_PathRMoveTo接口绘制图形，一半画布内，一半画布外
    OH_Drawing_PathRMoveTo(path, -200, -200); // -200, -200 for test
    OH_Drawing_PathRLineTo(path, 300, 300);   // 300, 300 for test
    OH_Drawing_CanvasDrawPath(canvas, path);
    // OH_Drawing_PathRMoveTo接口绘制图形，全部画布外
    OH_Drawing_PathReset(path);
    OH_Drawing_PathRMoveTo(path, -100, -100); // -100, -100 for test
    OH_Drawing_PathRLineTo(path, -200, -200); // -200, -200 for test
    OH_Drawing_CanvasDrawPath(canvas, path);
    DRAWING_LOGI("FunctionPathRMoveToTest OnTestFunction");
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PenDestroy(pen);
    OH_Drawing_PathDestroy(path);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_PATH_3901
@Description:
    1、OH_Drawing_PathConicTo接口正常入参验证第六个参数是小于等于0的数
    2、OH_Drawing_PathConicTo接口正常入参验证第六个参数是1
    3、OH_Drawing_PathConicTo接口绘制图形，一半画布内，一半画布外
    4、OH_Drawing_PathConicTo接口绘制图形，全部画布外
@Step:
    1、demo中第二个选择框覆盖所有模式（cpu-xcomponent、
    gpu-xcomponent、gpu-xnode）Testcase中输入，Testcase中输入
    functionPathConicToTest，点击draw按钮
@Result:
    1、demo打开正常不会闪退，无crash产生，3条青色曲线
 */
void FunctionPathConicToTest::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetWidth(pen, PEN_WIDTH);
    OH_Drawing_PenSetColor(pen, 0xff00ffff);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    // OH_Drawing_PathConicTo接口正常入参验证第六个参数是小于等于0的数
    OH_Drawing_PathConicTo(path, 100, 300, 400, 400, 0); // 100, 300, 400, 400, 0 for test
    OH_Drawing_CanvasDrawPath(canvas, path);
    // OH_Drawing_PathConicTo接口正常入参验证第六个参数是1
    OH_Drawing_PathReset(path);
    OH_Drawing_PathConicTo(path, 100, 400, 400, 500, 1); // 100, 400, 400, 500, 1 for test
    OH_Drawing_CanvasDrawPath(canvas, path);
    // OH_Drawing_PathConicTo接口绘制图形，一半画布内，一半画布外
    OH_Drawing_PathReset(path);
    OH_Drawing_PathConicTo(path, -100, -100, 180, 100, 1); // -100, -100, 180, 100, 1 for test
    OH_Drawing_CanvasDrawPath(canvas, path);
    // OH_Drawing_PathConicTo接口绘制图形，全部画布外
    OH_Drawing_PathReset(path);
    OH_Drawing_PathConicTo(path, -100, -100, -180, -160, 1); // -100, -100, -180, -160, 1 for test
    OH_Drawing_CanvasDrawPath(canvas, path);
    DRAWING_LOGI("FunctionPathConicToTest OnTestFunction");
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PenDestroy(pen);
    OH_Drawing_PathDestroy(path);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_PATH_4001
@Description:
    1、OH_Drawing_PathArcTo接口正常入参验证第七个参数为负数
    2、OH_Drawing_PathArcTo接口绘制图形，一半画布内，一半画布外
    3、OH_Drawing_PathArcTo接口绘制图形，全部画布外
@Step:
    1、demo中第二个选择框覆盖所有模式（cpu-xcomponent、
    gpu-xcomponent、gpu-xnode）Testcase中输入，Testcase中输入
    functionPathRConicToTest，点击draw按钮
@Result:
    1、demo打开正常不会闪退，无crash产生，2条青色曲线
 */
void FunctionPathArcToTest::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetWidth(pen, PEN_WIDTH);
    OH_Drawing_PenSetColor(pen, 0xff00ffff);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    // OH_Drawing_PathArcTo接口正常入参验证第七个参数为负数
    OH_Drawing_PathArcTo(path, 100, 100, 300, 400, 0, -180); // 100, 100, 300, 400, 0, -180 for test
    OH_Drawing_CanvasDrawPath(canvas, path);
    // OH_Drawing_PathArcTo接口绘制图形，一半画布内，一半画布外
    OH_Drawing_PathReset(path);
    OH_Drawing_PathArcTo(path, -100, -100, 300, 500, 0, -180); // -100, -100, 300, 500, 0, -180 for test
    OH_Drawing_CanvasDrawPath(canvas, path);
    // OH_Drawing_PathArcTo接口绘制图形，全部画布外
    OH_Drawing_PathReset(path);
    OH_Drawing_PathArcTo(path, -100, -100, -300, -400, 0, -180); // -100, -100, -300, -400, 0, -180 for test
    OH_Drawing_CanvasDrawPath(canvas, path);
    DRAWING_LOGI("FunctionPathArcToTest OnTestFunction");
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PenDestroy(pen);
    OH_Drawing_PathDestroy(path);
}