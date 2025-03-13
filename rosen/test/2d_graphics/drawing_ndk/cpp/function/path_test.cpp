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
#include "path_test.h"

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

const int PEN_WIDTH = 35;

/*
@CaseID:SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_PATH_2601
@Description:
    1、OH_Drawing_PathReset接口绘制图形，一半画布内，一半画布外
    2、OH_Drawing_PathReset接口绘制图形，全部画布外
@Step:
    1、demo中第二个选择框覆盖所有模式（cpu-xcomponent、
    gpu-xcomponent、gpu-xnode）Testcase中输入，Testcase中输入
    functionPathReset，点击draw按钮
@Result:
    1、demo打开正常不会闪退，无crash产生，1条红色斜线
 */
void FunctionPathReset::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("FunctionPathReset OnTestFunction");
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetColor(pen, 0xFFFF0000);
    OH_Drawing_PenSetWidth(pen, PEN_WIDTH);
    OH_Drawing_CanvasAttachPen(canvas, pen);

    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    // OH_Drawing_PathReset接口绘制图形，一半画布内，一半画布外
    OH_Drawing_PathMoveTo(path, 300, 300); // 300, 300 for test
    OH_Drawing_PathLineTo(path, 400, 400); // 400, 400 for test
    OH_Drawing_CanvasDrawPath(canvas, path);
    OH_Drawing_PathReset(path);
    OH_Drawing_PathMoveTo(path, 300, 300);  // 300, 300 for test
    OH_Drawing_PathLineTo(path, -300, 300); // -300, 300 for test
    OH_Drawing_CanvasDrawPath(canvas, path);
    // OH_Drawing_PathReset接口绘制图形，全部画布外
    OH_Drawing_PathReset(path);
    OH_Drawing_PathLineTo(path, -100, -100); // -100,-100 for test
    OH_Drawing_CanvasDrawPath(canvas, path);

    OH_Drawing_PathDestroy(path);
    OH_Drawing_PenDestroy(pen);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_PATH_2701
@Description:
    1、OH_Drawing_PathAddOval接口正常入参验证(枚举值逆时针方向)
    2、OH_Drawing_PathAddOval接口绘制图形，一半画布内，一半画布外
    3、OH_Drawing_PathAddOval接口绘制图形，全部画布外
@Step:
    1、demo中第二个选择框覆盖所有模式（cpu-xcomponent、
    gpu-xcomponent、gpu-xnode）Testcase中输入，Testcase中输入
    functionPathAddOval，点击draw按钮
@Result:
    1、demo打开正常不会闪退，无crash产生，1个红色椭圆，1个一半在画布外的红色椭圆
 */
void FunctionPathAddOval::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("FunctionPathAddOval OnTestFunction");
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetColor(pen, 0xFFFF0000);
    OH_Drawing_PenSetWidth(pen, PEN_WIDTH);
    OH_Drawing_CanvasAttachPen(canvas, pen);

    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    // OH_Drawing_PathAddOval接口正常入参验证(枚举值逆时针方向)
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(0, 0, 200, 300); // 200, 300 for test
    OH_Drawing_PathAddOval(path, rect, OH_Drawing_PathDirection::PATH_DIRECTION_CCW);
    OH_Drawing_CanvasDrawPath(canvas, path);
    // OH_Drawing_PathAddOval接口绘制图形，一半画布内，一半画布外
    OH_Drawing_Rect* rect1 = OH_Drawing_RectCreate(-100, 310, 200, 600); // -100, 310, 200, 600 for test
    OH_Drawing_PathAddOval(path, rect1, OH_Drawing_PathDirection::PATH_DIRECTION_CW);
    OH_Drawing_CanvasDrawPath(canvas, path);
    // OH_Drawing_PathAddOval接口绘制图形，全部画布外
    OH_Drawing_Rect* rect2 = OH_Drawing_RectCreate(-200, 0, 0, 300); // -200, 0, 0, 300 for test
    OH_Drawing_PathAddOval(path, rect2, OH_Drawing_PathDirection::PATH_DIRECTION_CW);
    OH_Drawing_CanvasDrawPath(canvas, path);

    OH_Drawing_RectDestroy(rect);
    OH_Drawing_RectDestroy(rect1);
    OH_Drawing_RectDestroy(rect2);
    OH_Drawing_PathDestroy(path);
    OH_Drawing_PenDestroy(pen);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_PATH_2801
@Description:
    1、OH_Drawing_PathTransformWithPerspectiveClip接口正常入参验证srcpath不为空，第四个参数写true
    2、OH_Drawing_PathTransformWithPerspectiveClip接口正常入参验证srcpath不为空，第四个参数写false
    3、OH_Drawing_PathTransformWithPerspectiveClip接口正常入参验证srcpath不为空，第四个参数写false接口绘制图形，一半画布内，一半画布外
    4、OH_Drawing_PathTransformWithPerspectiveClip接口正常入参验证srcpath不为空，第四个参数写false接口绘制图形，全部画布外
@Step:
    1、demo中第二个选择框覆盖所有模式（cpu-xcomponent、
    gpu-xcomponent、gpu-xnode）Testcase中输入，Testcase中输入
    functionPathTransformWithPerspectiveClip，点击draw按钮
@Result:
    1、demo打开正常不会闪退，无crash产生，2条红色竖线，1条红色斜线
 */
void FunctionPathTransformWithPerspectiveClip::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("FunctionPathTransformWithPerspectiveClip OnTestFunction");
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetColor(pen, 0xFFFF0000);
    OH_Drawing_PenSetWidth(pen, PEN_WIDTH);
    OH_Drawing_CanvasAttachPen(canvas, pen);

    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(path, 100, 100);                                     // 100, 100 for test
    OH_Drawing_PathLineTo(path, 300, 300);                                     // 300, 300 for test
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreateRotation(45, 200, 200); // 45, 200, 200 for test
    // 1.OH_Drawing_PathTransformWithPerspectiveClip接口正常入参验证srcpath不为空，第四个参数写true
    OH_Drawing_PathTransformWithPerspectiveClip(path, matrix, nullptr, true);
    OH_Drawing_CanvasDrawPath(canvas, path);
    OH_Drawing_PathDestroy(path);
    // 2.OH_Drawing_PathTransformWithPerspectiveClip接口正常入参验证srcpath不为空，第四个参数写false
    OH_Drawing_Path* path1 = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(path1, 150, 100); // 150, 100 for test
    OH_Drawing_PathLineTo(path1, 350, 300); // 350, 300 for test
    OH_Drawing_PathTransformWithPerspectiveClip(path1, matrix, nullptr, false);
    OH_Drawing_CanvasDrawPath(canvas, path1);
    OH_Drawing_PathDestroy(path1);
    // 3.OH_Drawing_PathTransformWithPerspectiveClip接口正常入参验证srcpath不为空，第四个参数写false接口绘制图形，一半画布内，一半画布外
    OH_Drawing_Path* path2 = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(path2, 400, 400);  // 400, 400 for test
    OH_Drawing_PathLineTo(path2, -200, 400); // -200, 400 for test
    OH_Drawing_PathTransformWithPerspectiveClip(path2, matrix, nullptr, false);
    OH_Drawing_CanvasDrawPath(canvas, path2);
    OH_Drawing_PathDestroy(path2);
    // 4.OH_Drawing_PathTransformWithPerspectiveClip接口正常入参验证srcpath不为空，第四个参数写false接口绘制图形，全部画布外
    OH_Drawing_Path* path3 = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(path3, 0, 0);
    OH_Drawing_PathLineTo(path3, -200, -200); // -200, -200 for test
    OH_Drawing_PathTransformWithPerspectiveClip(path3, matrix, nullptr, false);
    OH_Drawing_CanvasDrawPath(canvas, path3);

    OH_Drawing_PathDestroy(path3);
    OH_Drawing_PenDestroy(pen);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_PATH_2901
@Description:
    1、OH_Drawing_PathAddPathWithOffsetAndMode接口绘制图形，一半画布内，一半画布外
    2、OH_Drawing_PathAddPathWithOffsetAndMode接口绘制图形，全部画布外
@Step:
    1、demo中第二个选择框覆盖所有模式（cpu-xcomponent、
    gpu-xcomponent、gpu-xnode）Testcase中输入，Testcase中输入
    FunctionPathAddPathWithOffsetAndMode，点击draw按钮
@Result:
    1、demo打开正常不会闪退，无crash产生，2组相交的红色直线
 */
void FunctionPathAddPathWithOffsetAndMode::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("FunctionPathAddPathWithOffsetAndMode OnTestFunction");
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetColor(pen, 0xFFFF0000);
    OH_Drawing_PenSetWidth(pen, PEN_WIDTH);
    OH_Drawing_CanvasAttachPen(canvas, pen);

    OH_Drawing_Path* srcPath = OH_Drawing_PathCreate();
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(srcPath, 0, 0);
    OH_Drawing_PathLineTo(srcPath, 200, 200); // 200, 200 for test
    OH_Drawing_PathMoveTo(path, 0, 0);
    OH_Drawing_PathLineTo(path, 300, 300);                                                // 300, 300 for test
    OH_Drawing_PathAddPathWithOffsetAndMode(path, srcPath, 100, 0, PATH_ADD_MODE_EXTEND); // 100 for test
    OH_Drawing_CanvasDrawPath(canvas, path);
    // OH_Drawing_PathAddPathWithOffsetAndMode接口绘制图形，一半画布内，一半画布外
    OH_Drawing_PathReset(srcPath);
    OH_Drawing_PathReset(path);
    OH_Drawing_PathMoveTo(srcPath, 100, 400);                                             // 100, 400 for test
    OH_Drawing_PathLineTo(srcPath, 100, 300);                                             // 100, 300 for test
    OH_Drawing_PathMoveTo(path, 100, 300);                                                // 100, 300 for test
    OH_Drawing_PathLineTo(path, -200, 300);                                               // -200, 300 for test
    OH_Drawing_PathAddPathWithOffsetAndMode(path, srcPath, 100, 0, PATH_ADD_MODE_EXTEND); // 100 for test
    OH_Drawing_CanvasDrawPath(canvas, path);
    // OH_Drawing_PathAddPathWithOffsetAndMode接口绘制图形，全部画布外
    OH_Drawing_PathReset(srcPath);
    OH_Drawing_PathReset(path);
    OH_Drawing_PathMoveTo(srcPath, -100, 0);   // -100 for test
    OH_Drawing_PathLineTo(srcPath, -200, 200); // -200, 200 for test
    OH_Drawing_PathMoveTo(path, 0, 0);
    OH_Drawing_PathLineTo(path, -200, 300);                                               // -200, 300 for test
    OH_Drawing_PathAddPathWithOffsetAndMode(path, srcPath, 100, 0, PATH_ADD_MODE_EXTEND); // 100 for test
    OH_Drawing_CanvasDrawPath(canvas, path);

    OH_Drawing_PathDestroy(path);
    OH_Drawing_PathDestroy(srcPath);
    OH_Drawing_PenDestroy(pen);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_PATH_3001
@Description:
    1、OH_Drawing_PathAddPathWithMode接口正常入参验证PATH_ADD_MODE_APPEND
    2、OH_Drawing_PathAddPathWithMode接口绘制图形，一半画布内，一半画布外
    3、OH_Drawing_PathAddPathWithMode接口绘制图形，全部画布外
@Step:
    1、demo中第二个选择框覆盖所有模式（cpu-xcomponent、
    gpu-xcomponent、gpu-xnode）Testcase中输入，Testcase中输入
    functionPathAddPathWithMode，点击draw按钮
@Result:
    1、demo打开正常不会闪退，无crash产生，2条红色的直线
 */
void FunctionPathAddPathWithMode::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("FunctionPathAddPathWithMode OnTestFunction");
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetColor(pen, 0xFFFF0000);
    OH_Drawing_PenSetWidth(pen, PEN_WIDTH);
    OH_Drawing_CanvasAttachPen(canvas, pen);

    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_Path* srcPath = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(path, 0, 0);
    OH_Drawing_PathLineTo(path, 200, 200);    // 200, 200 for test
    OH_Drawing_PathMoveTo(srcPath, 0, 100);   // 100 for test
    OH_Drawing_PathLineTo(srcPath, 200, 300); // 200, 300 for test
    // OH_Drawing_PathAddPathWithMode接口正常入参验证PATH_ADD_MODE_APPEND
    OH_Drawing_PathAddPathWithMode(path, srcPath, PATH_ADD_MODE_APPEND);
    OH_Drawing_CanvasDrawPath(canvas, path);
    // OH_Drawing_PathAddPathWithMode接口绘制图形，一半画布内，一半画布外
    OH_Drawing_PathReset(srcPath);
    OH_Drawing_PathReset(path);
    OH_Drawing_PathMoveTo(srcPath, 300, 300);  // 300, 300 for test
    OH_Drawing_PathLineTo(srcPath, -200, 500); // -200, 500 for test
    OH_Drawing_PathMoveTo(path, 300, 300);     // 300, 300 for test
    OH_Drawing_PathLineTo(path, 600, 300);     // 600, 300 for test
    OH_Drawing_PathAddPathWithMode(path, srcPath, PATH_ADD_MODE_APPEND);
    OH_Drawing_CanvasDrawPath(canvas, path);
    // OH_Drawing_PathAddPathWithMode接口绘制图形，全部画布外
    OH_Drawing_PathReset(srcPath);
    OH_Drawing_PathReset(path);
    OH_Drawing_PathMoveTo(srcPath, -300, 300); // -300, 300 for test
    OH_Drawing_PathLineTo(srcPath, -200, 300); // -200, 300 for test
    OH_Drawing_PathMoveTo(path, -500, 200);    // -500, 200 for test
    OH_Drawing_PathLineTo(path, -300, 300);    // -300, 300 for test
    OH_Drawing_PathAddPathWithMode(path, srcPath, PATH_ADD_MODE_APPEND);
    OH_Drawing_CanvasDrawPath(canvas, path);

    OH_Drawing_PathDestroy(path);
    OH_Drawing_PathDestroy(srcPath);
    OH_Drawing_PenDestroy(pen);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_PATH_3101
@Description:
    1、OH_Drawing_PathAddPathWithMatrixAndMode接口正常入参验证第三个参数为空，PATH_ADD_MODE_APPEND
    2、OH_Drawing_PathAddPathWithMatrixAndMode接口绘制图形，一半画布内，一半画布外
    3、OH_Drawing_PathAddPathWithMatrixAndMode接口绘制图形，全部画布外
@Step:
    1、demo中第二个选择框覆盖所有模式（cpu-xcomponent、
    gpu-xcomponent、gpu-xnode）Testcase中输入，Testcase中输入
    functionPathAddPathWithMatrixAndMode，点击draw按钮
@Result:
    1、demo打开正常不会闪退，无crash产生，1条由直线和弧线连接的曲线，1条一半在画布外的由直线和弧线连接的曲线
 */
void FunctionPathAddPathWithMatrixAndMode::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("FunctionPathAddPathWithMatrixAndMode OnTestFunction");
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetColor(pen, 0xFFFF0000);
    OH_Drawing_PenSetWidth(pen, PEN_WIDTH);
    OH_Drawing_CanvasAttachPen(canvas, pen);

    OH_Drawing_Matrix* m = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixSetMatrix(m, 1, 0, 0, 0, 1, 0, 0, 0, 1); // 1, 1, 1 for test
    OH_Drawing_Path* sourcePath = OH_Drawing_PathCreate();
    OH_Drawing_Path* targetPath = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(sourcePath, 300, 30); // 300, 30 for test
    OH_Drawing_PathMoveTo(targetPath, 0, 0);
    OH_Drawing_PathArcTo(sourcePath, 200, 200, 500, 400, 0, 180); // 200, 200, 500, 400, 0, 180 for test
    OH_Drawing_PathArcTo(targetPath, 0, 0, 300, 200, 0, 180);     // 300, 180 for test
    // OH_Drawing_PathAddPathWithMatrixAndMode接口正常入参验证第三个参数为空，PATH_ADD_MODE_APPEND
    OH_Drawing_PathAddPathWithMatrixAndMode(targetPath, sourcePath, m, PATH_ADD_MODE_EXTEND);
    OH_Drawing_CanvasDrawPath(canvas, targetPath);
    // OH_Drawing_PathAddPathWithMatrixAndMode接口绘制图形，一半画布内，一半画布外
    OH_Drawing_PathReset(sourcePath);
    OH_Drawing_PathReset(targetPath);
    OH_Drawing_PathMoveTo(sourcePath, -200, 500);                  // -200, 500 for test
    OH_Drawing_PathMoveTo(targetPath, 100, 400);                   // 100, 400 for test
    OH_Drawing_PathArcTo(sourcePath, -200, 500, 100, 600, 0, 180); // -200, 500, 100, 600, 0, 180 for test
    OH_Drawing_PathArcTo(targetPath, 100, 400, 300, 500, 0, 180);  // 100, 400, 300, 500, 0, 180 for test
    OH_Drawing_PathAddPathWithMatrixAndMode(targetPath, sourcePath, m, PATH_ADD_MODE_EXTEND);
    OH_Drawing_CanvasDrawPath(canvas, targetPath);
    // OH_Drawing_PathAddPathWithMatrixAndMode接口绘制图形，全部画布外
    OH_Drawing_PathReset(sourcePath);
    OH_Drawing_PathReset(targetPath);
    OH_Drawing_PathMoveTo(sourcePath, -200, 500);                   // -200, 500 for test
    OH_Drawing_PathMoveTo(targetPath, -100, -400);                  // -100, -400 for test
    OH_Drawing_PathArcTo(sourcePath, -200, 500, -100, 600, 0, 180); // -200, 500, -100, 600, 0, 180 for test
    OH_Drawing_PathArcTo(targetPath, -100, 400, -300, 500, 0, 180); // -100, 400, -300, 500, 0, 180 for test
    OH_Drawing_PathAddPathWithMatrixAndMode(targetPath, sourcePath, m, PATH_ADD_MODE_EXTEND);
    OH_Drawing_CanvasDrawPath(canvas, targetPath);

    OH_Drawing_MatrixDestroy(m);
    OH_Drawing_PathDestroy(sourcePath);
    OH_Drawing_PathDestroy(targetPath);
    OH_Drawing_PenDestroy(pen);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_FUNCTION_DRAWING_NDK_PATH_3201
@Description:
    1、OH_Drawing_PathAddOvalWithInitialPoint接口正常入参验证第三个参数为负数，PATH_DIRECTION_CW
    2、OH_Drawing_PathAddOvalWithInitialPoint接口正常入参验证第三个参数为小数，PATH_DIRECTION_CW
    3、OH_Drawing_PathAddOvalWithInitialPoint接口正常入参验证，PATH_DIRECTION_CCW
    4、OH_Drawing_PathAddOvalWithInitialPoint接口绘制图形，一半画布内，一半画布外
    5、OH_Drawing_PathAddOvalWithInitialPoint接口绘制图形，全部画布外
@Step:
    1、demo中第二个选择框覆盖所有模式（cpu-xcomponent、
    gpu-xcomponent、gpu-xnode）Testcase中输入，Testcase中输入
    functionPathAddOvalWithInitialPoint，点击draw按钮
@Result:
    1、demo打开正常不会闪退，无crash产生，3个红色椭圆，1个一半在画布外的红色椭圆
 */
void FunctionPathAddOvalWithInitialPoint::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("FunctionPathAddOvalWithInitialPoint OnTestFunction");
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetColor(pen, 0xFFFF0000);
    OH_Drawing_PenSetWidth(pen, PEN_WIDTH);
    OH_Drawing_CanvasAttachPen(canvas, pen);

    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(0, 0, 300, 200); // 300, 200 for test
    uint32_t start = -10;                                          // -10 for test
    // OH_Drawing_PathAddOvalWithInitialPoint接口正常入参验证第三个参数为负数，PATH_DIRECTION_CW
    OH_Drawing_PathAddOvalWithInitialPoint(path, rect, start, PATH_DIRECTION_CW);
    OH_Drawing_CanvasDrawPath(canvas, path);
    // OH_Drawing_PathAddOvalWithInitialPoint接口正常入参验证第三个参数为小数，PATH_DIRECTION_CW
    float startF = 10.6; // 10.6 for test
    OH_Drawing_PathReset(path);
    OH_Drawing_Rect* rect1 = OH_Drawing_RectCreate(0, 210, 300, 410); // 210, 300, 410 for test
    OH_Drawing_PathAddOvalWithInitialPoint(path, rect1, startF, PATH_DIRECTION_CW);
    OH_Drawing_CanvasDrawPath(canvas, path);
    // OH_Drawing_PathAddOvalWithInitialPoint接口正常入参验证，PATH_DIRECTION_CCW
    start = 0;
    OH_Drawing_PathReset(path);
    OH_Drawing_Rect* rect2 = OH_Drawing_RectCreate(350, 0, 650, 200); // 350, 0, 650, 200 for test
    OH_Drawing_PathAddOvalWithInitialPoint(path, rect2, start, PATH_DIRECTION_CCW);
    OH_Drawing_CanvasDrawPath(canvas, path);
    // OH_Drawing_PathAddOvalWithInitialPoint接口绘制图形，一半画布内，一半画布外
    OH_Drawing_PathReset(path);
    OH_Drawing_Rect* rect3 = OH_Drawing_RectCreate(-100, 450, 200, 550); // -100, 450, 200, 550 for test
    OH_Drawing_PathAddOvalWithInitialPoint(path, rect3, start, PATH_DIRECTION_CCW);
    OH_Drawing_CanvasDrawPath(canvas, path);
    // OH_Drawing_PathAddOvalWithInitialPoint接口绘制图形，全部画布外
    OH_Drawing_PathReset(path);
    OH_Drawing_Rect* rect4 = OH_Drawing_RectCreate(-200, 0, -100, 200); // -200, 0, -100, 200 for test
    OH_Drawing_PathAddOvalWithInitialPoint(path, rect4, start, PATH_DIRECTION_CCW);
    OH_Drawing_CanvasDrawPath(canvas, path);

    OH_Drawing_PathDestroy(path);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_RectDestroy(rect1);
    OH_Drawing_RectDestroy(rect2);
    OH_Drawing_RectDestroy(rect3);
    OH_Drawing_RectDestroy(rect4);
    OH_Drawing_PenDestroy(pen);
}