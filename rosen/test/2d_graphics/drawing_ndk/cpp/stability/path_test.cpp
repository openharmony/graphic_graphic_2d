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

#include "path_test.h"

#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_round_rect.h>

#include "test_common.h"

#include "common/log_common.h"

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_PATH_0100
@Description:PathCreate-PathDestroy循环
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2、OH_Drawing_PathCreate-OH_Drawing_PathDestroy重复调用1000次
    3、执行结束后等待3分钟
    4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
@Result:
    1、程序运行正常，无crash
    2、内存存平稳，没有持续增长，执行后内存回落的到执行前
 */
void StabilityPathCreate::OnTestStability(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("StabilityPathCreate OnTestStability");
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_Path* path = OH_Drawing_PathCreate();
        OH_Drawing_PathDestroy(path);
    }
}

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_PATH_0200
@Description:PathCopy-PathDestroy循环
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2、OH_Drawing_PathCopy-OH_Drawing_PathDestroy重复调用1000次
    3、执行结束后等待3分钟
    4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
@Result:
    1、程序运行正常，无crash
    2、内存存平稳，没有持续增长，执行后内存回落的到执行前
 */
void StabilityPathCopy::OnTestStability(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("StabilityPathCopy OnTestStability");
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_Path* pathCopy = OH_Drawing_PathCopy(path);
        OH_Drawing_PathDestroy(pathCopy);
    }
    OH_Drawing_PathDestroy(path);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_PATH_0300
@Description:PathCreate-相关配置接口全调用-PathDestroy循环
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2、PathCreate-相关配置接口全调用-PathDestroy循环
    3、执行结束后等待3分钟
    4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
@Result:
    1、程序运行正常，无crash
    2、内存存平稳，没有持续增长，执行后内存回落的到执行前
 */
void StabilityPathAll::OnTestStability(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("StabilityPathAll OnTestStability");
    for (int i = 0; i < testCount_; i++) {
        index_ = i;
        uint32_t funcIndexMax = SETTING_FUNCTION_PATH_DESTROY;
        for (int j = 0; j <= funcIndexMax; j++) {
            handlers_[j](path_);
        }
    }
}

void StabilityPathAll::PathCreateTest(OH_Drawing_Path* path)
{
    path = OH_Drawing_PathCreate();
}

void StabilityPathAll::PathCopyTest(OH_Drawing_Path* path)
{
    OH_Drawing_Path* pathCopy = OH_Drawing_PathCopy(path);
    OH_Drawing_PathDestroy(pathCopy);
}

void StabilityPathAll::PathBuildFromSvgStringTest(OH_Drawing_Path* path)
{
    OH_Drawing_PathBuildFromSvgString(path, str_);
}

void StabilityPathAll::PathSetFillTypeTest(OH_Drawing_Path* path)
{
    OH_Drawing_PathSetFillType(path, DrawGetEnum(PATH_FILL_TYPE_WINDING, PATH_FILL_TYPE_INVERSE_EVEN_ODD, index_));
}

void StabilityPathAll::PathResetTest(OH_Drawing_Path* path)
{
    OH_Drawing_PathReset(path);
}

void StabilityPathAll::PathDestroyTest(OH_Drawing_Path* path)
{
    OH_Drawing_PathDestroy(path);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_PATH_0400
@Description:Path相关操作接口随机循环调用
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2、Path相关操作接口随机循环调用
    3、执行结束后等待3分钟
    4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
@Result:
    1、程序运行正常，无crash
    2、内存存平稳，没有持续增长，执行后内存回落的到执行前
 */
void StabilityPathRandom::OnTestStability(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("StabilityPathRandom OnTestStability");
    TestRend rand = TestRend();
    left_ = rand.nextRangeF(sMin_, sMax_);
    top_ = rand.nextRangeF(sMin_, sMax_);
    right_ = rand.nextRangeF(endMin_, endMax_);
    bottom_ = rand.nextRangeF(endMin_, endMax_);
    startDeg_ = rand.nextRangeF(sMin_, sMax_);
    sweepDeg_ = rand.nextRangeF(sMin_, sMax_);
    weight_ = rand.nextRangeF(sMin_, maxWeight_);
    start_ = rand.nextRangeF(sMin_, sMax_);
    xRad_ = rand.nextRangeF(radMin_, radMax_);
    yRad_ = rand.nextRangeF(radMin_, radMax_);
    startAngle_ = rand.nextRangeF(angleMin_, angleMax_);
    sweepAngle_ = rand.nextRangeF(angleMin_, angleMax_);
    distance_ = rand.nextRangeF(sMin_, sMax_);
    matrix_ = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixSetMatrix(matrix_, 1, 0, 0, 0, 1, 0, 0, 0, 1); // 1 for test
    path_ = OH_Drawing_PathCreate();
    srcPath_ = OH_Drawing_PathCreate();
    dstPath_ = OH_Drawing_PathCreate();
    rect_ = OH_Drawing_RectCreate(left_, top_, right_, bottom_);
    RoundRect_ = OH_Drawing_RoundRectCreate(rect_, xRad_, yRad_);
    for (int i = 0; i < testCount_; i++) {
        index_ = i;
        uint32_t funcIndexMax = OPERATION_FUNCTION_PATH_IS_CLOSED;
        uint32_t index = rand.nextRangeU(0, funcIndexMax);
        handlers_[index](path_);
    }
    OH_Drawing_RectDestroy(rect_);
    OH_Drawing_RoundRectDestroy(RoundRect_);
    OH_Drawing_PathDestroy(path_);
    OH_Drawing_PathDestroy(srcPath_);
    OH_Drawing_PathDestroy(dstPath_);
    OH_Drawing_MatrixDestroy(matrix_);
}

void StabilityPathRandom::PathOpTest(OH_Drawing_Path* path)
{
    OH_Drawing_PathOp(path, srcPath_, DrawGetEnum(PATH_OP_MODE_DIFFERENCE, PATH_OP_MODE_REVERSE_DIFFERENCE, index_));
}

void StabilityPathRandom::PathMoveToTest(OH_Drawing_Path* path)
{
    OH_Drawing_PathMoveTo(path, left_, top_);
}

void StabilityPathRandom::PathLineToTest(OH_Drawing_Path* path)
{
    OH_Drawing_PathLineTo(path, right_, bottom_);
}

void StabilityPathRandom::PathArcToTest(OH_Drawing_Path* path)
{
    OH_Drawing_PathArcTo(path, top_, left_, right_, bottom_, startDeg_, sweepDeg_);
}

void StabilityPathRandom::PathQuadToTest(OH_Drawing_Path* path)
{
    OH_Drawing_PathQuadTo(path, top_, left_, right_, bottom_);
}

void StabilityPathRandom::PathConicToTest(OH_Drawing_Path* path)
{
    OH_Drawing_PathConicTo(path, top_, left_, right_, bottom_, weight_);
}

void StabilityPathRandom::PathCubicToTest(OH_Drawing_Path* path)
{
    OH_Drawing_PathCubicTo(path, top_, left_, top_, left_, right_, bottom_);
}

void StabilityPathRandom::PathRMoveToTest(OH_Drawing_Path* path)
{
    OH_Drawing_PathRMoveTo(path, top_, left_);
}

void StabilityPathRandom::PathRLineToTest(OH_Drawing_Path* path)
{
    OH_Drawing_PathRLineTo(path, right_, bottom_);
}

void StabilityPathRandom::PathRQuadToTest(OH_Drawing_Path* path)
{
    OH_Drawing_PathRQuadTo(path, top_, left_, right_, bottom_);
}

void StabilityPathRandom::PathRConicToTest(OH_Drawing_Path* path)
{
    OH_Drawing_PathRConicTo(path, top_, left_, right_, bottom_, weight_);
}

void StabilityPathRandom::PathRCubicToTest(OH_Drawing_Path* path)
{
    OH_Drawing_PathRCubicTo(path, top_, left_, top_, left_, right_, bottom_);
}

void StabilityPathRandom::PathAddRectTest(OH_Drawing_Path* path)
{
    OH_Drawing_PathAddRect(
        path, top_, left_, right_, bottom_, DrawGetEnum(PATH_DIRECTION_CW, PATH_DIRECTION_CCW, index_));
}

void StabilityPathRandom::PathAddRectWithInitialCornerTest(OH_Drawing_Path* path)
{
    OH_Drawing_PathAddRectWithInitialCorner(
        path, rect_, DrawGetEnum(PATH_DIRECTION_CW, PATH_DIRECTION_CCW, index_), start_);
}

void StabilityPathRandom::PathAddRoundRectTest(OH_Drawing_Path* path)
{
    OH_Drawing_PathAddRoundRect(path, RoundRect_, DrawGetEnum(PATH_DIRECTION_CW, PATH_DIRECTION_CCW, index_));
}

void StabilityPathRandom::PathAddOvalWithInitialPointTest(OH_Drawing_Path* path)
{
    OH_Drawing_PathAddOvalWithInitialPoint(
        path, rect_, start_, DrawGetEnum(PATH_DIRECTION_CW, PATH_DIRECTION_CCW, index_));
}

void StabilityPathRandom::PathAddOvalTest(OH_Drawing_Path* path)
{
    OH_Drawing_PathAddOval(path, rect_, DrawGetEnum(PATH_DIRECTION_CW, PATH_DIRECTION_CCW, index_));
}

void StabilityPathRandom::PathAddArcTest(OH_Drawing_Path* path)
{
    OH_Drawing_PathAddArc(path, rect_, startAngle_, sweepAngle_);
}

void StabilityPathRandom::PathAddPathTest(OH_Drawing_Path* path)
{
    OH_Drawing_PathAddPath(path, srcPath_, matrix_);
}

void StabilityPathRandom::PathAddPathWithMatrixAndModeTest(OH_Drawing_Path* path)
{
    OH_Drawing_PathAddPathWithMatrixAndMode(
        path, srcPath_, matrix_, DrawGetEnum(PATH_ADD_MODE_APPEND, PATH_ADD_MODE_EXTEND, index_));
}

void StabilityPathRandom::PathAddPathWithModeTest(OH_Drawing_Path* path)
{
    OH_Drawing_PathAddPathWithMode(path, srcPath_, DrawGetEnum(PATH_ADD_MODE_APPEND, PATH_ADD_MODE_EXTEND, index_));
}

void StabilityPathRandom::PathAddPathWithOffsetAndModeTest(OH_Drawing_Path* path)
{
    OH_Drawing_PathAddPathWithOffsetAndMode(
        path, srcPath_, left_, right_, DrawGetEnum(PATH_ADD_MODE_APPEND, PATH_ADD_MODE_EXTEND, index_));
}

void StabilityPathRandom::PathAddPolygonTest(OH_Drawing_Path* path)
{
    OH_Drawing_Point2D points[] = { { 500, 550 }, { 500, 600 } }; // 500, 550, 500, 600 for test
    OH_Drawing_PathAddPolygon(path, points, count_, isClosed_);
}

void StabilityPathRandom::PathAddCircleTest(OH_Drawing_Path* path)
{
    OH_Drawing_PathAddCircle(path, top_, left_, xRad_, DrawGetEnum(PATH_DIRECTION_CW, PATH_DIRECTION_CCW, index_));
}

void StabilityPathRandom::PathContainsTest(OH_Drawing_Path* path)
{
    OH_Drawing_PathContains(path, top_, left_);
}

void StabilityPathRandom::PathTransformTest(OH_Drawing_Path* path)
{
    OH_Drawing_PathTransform(path, matrix_);
}

void StabilityPathRandom::PathTransformWithPerspectiveClipTest(OH_Drawing_Path* path)
{
    OH_Drawing_PathTransformWithPerspectiveClip(srcPath_, matrix_, dstPath_, applyPerspectiveClip_);
}

void StabilityPathRandom::PathGetLengthTest(OH_Drawing_Path* path)
{
    OH_Drawing_PathGetLength(path, forceClosed_);
}

void StabilityPathRandom::PathGetBoundsTest(OH_Drawing_Path* path)
{
    OH_Drawing_PathGetBounds(path, rect_);
}

void StabilityPathRandom::PathOffsetTest(OH_Drawing_Path* path)
{
    OH_Drawing_PathOffset(srcPath_, dstPath_, top_, left_);
}

void StabilityPathRandom::PathGetPositionTangentTest(OH_Drawing_Path* path)
{
    OH_Drawing_Point2D position[] = { { 400, 400 } }; // 400,400 for test
    OH_Drawing_Point2D tangent[] = { { 500, 500 } };  // 500,500 for test
    OH_Drawing_PathGetPositionTangent(path, forceClosed_, distance_, position, tangent);
}

void StabilityPathRandom::PathGetMatrixTest(OH_Drawing_Path* path)
{
    OH_Drawing_PathGetMatrix(path, forceClosed_, distance_, matrix_,
        DrawGetEnum(GET_POSITION_MATRIX, GET_POSITION_AND_TANGENT_MATRIX, index_));
}

void StabilityPathRandom::PathCloseTest(OH_Drawing_Path* path)
{
    OH_Drawing_PathClose(path);
}

void StabilityPathRandom::PathIsClosedTest(OH_Drawing_Path* path)
{
    OH_Drawing_PathIsClosed(path, forceClosed_);
}