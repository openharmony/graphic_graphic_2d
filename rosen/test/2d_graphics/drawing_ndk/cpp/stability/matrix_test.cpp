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

#include "matrix_test.h"

#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_rect.h>

#include "test_common.h"

#include "common/log_common.h"

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_MATRIX_0100
@Description:MatrixCreate-MatrixDestroy循环
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2、OH_Drawing_MatrixCreate-OH_Drawing_MatrixDestroy循环调用1000次
    3、执行结束后等待3分钟
    4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
@Result:
    1、程序运行正常，无crash
    2、内存存平稳，没有持续增长，执行后内存回落的到执行前
 */
void StabilityMatrixCreate::OnTestStability(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("StabilityMatrixCreate OnTestStability");
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreate();
        OH_Drawing_MatrixDestroy(matrix);
    }
}

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_MATRIX_0200
@Description:MatrixCreateRotation-MatrixDestroy循环
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2、MatrixCreateRotation-MatrixDestroy循环调用1000次
    3、执行结束后等待3分钟
    4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
@Result:
    1、程序运行正常，无crash
    2、内存存平稳，没有持续增长，执行后内存回落的到执行前
 */
void StabilityMatrixCreateRotation::OnTestStability(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("StabilityMatrixCreateRotation OnTestStability");
    TestRend rand;
    float min = 0;
    float max = 30;
    for (int i = 0; i < testCount_; i++) {
        float deg = rand.nextRangeF(min, max);
        float x = rand.nextRangeF(min, max);
        float y = rand.nextRangeF(min, max);
        OH_Drawing_Matrix* rotationMatrix = OH_Drawing_MatrixCreateRotation(deg, x, y);
        OH_Drawing_MatrixDestroy(rotationMatrix);
    }
}

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_MATRIX_0300
@Description:MatrixCreateScale-MatrixDestroy循环
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2、OH_Drawing_MatrixCreateScale-OH_Drawing_MatrixDestroy循环调用1000次
    3、执行结束后等待3分钟
    4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
@Result:
    1、程序运行正常，无crash
    2、内存存平稳，没有持续增长，执行后内存回落的到执行前
 */
void StabilityMatrixCreateScale::OnTestStability(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("StabilityMatrixCreateScale OnTestStability");
    TestRend rand;
    float min = 1;
    float max = 10;
    for (int i = 0; i < testCount_; i++) {
        float sx = rand.nextRangeF(min, max);
        float sy = rand.nextRangeF(min, max);
        float px = rand.nextRangeF(min, max);
        float py = rand.nextRangeF(min, max);
        OH_Drawing_Matrix* scaleMatrix = OH_Drawing_MatrixCreateScale(sx, sy, px, py);
        OH_Drawing_MatrixDestroy(scaleMatrix);
    }
}

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_MATRIX_0400
@Description:MatrixCreateTranslation-MatrixDestroy循环
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2、OH_Drawing_MatrixCreateTranslation-OH_Drawing_MatrixDestroy循环调用1000次
    3、执行结束后等待3分钟
    4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
@Result:
    1、程序运行正常，无crash
    2、内存存平稳，没有持续增长，执行后内存回落的到执行前
 */
void StabilityMatrixCreateTranslation::OnTestStability(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("StabilityMatrixCreateTranslation OnTestStability");
    TestRend rand;
    float min = 0;
    float max = 200;
    for (int i = 0; i < testCount_; i++) {
        float dx = rand.nextRangeF(min, max);
        float dy = rand.nextRangeF(min, max);
        OH_Drawing_Matrix* translationMatrix = OH_Drawing_MatrixCreateTranslation(dx, dy);
        OH_Drawing_MatrixDestroy(translationMatrix);
    }
}

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_MATRIX_0500
@Description:MatrixCreate-相关配置接口全调用-MatrixDestroy循环
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2、MatrixCreate-相关配置接口全调用-MatrixDestroy循环调用1000次
    3、执行结束后等待3分钟
    4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
@Result:
    1、程序运行正常，无crash
    2、内存存平稳，没有持续增长，执行后内存回落的到执行前
 */
void StabilityMatrixAll::OnTestStability(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreate();

    TestRend rand = TestRend();

    for (int i = 0; i < testCount_; i++) {
        handlers_[rand.nextRangeU(0, SETTING_FUNCTION_MATRIX_MAX - 1)](matrix);
    }
    OH_Drawing_MatrixDestroy(matrix);
}

void StabilityMatrixAll::MatrixSetMatrixTest(OH_Drawing_Matrix* matrix)
{
    // 1, 2, 3, 4, 5, 6, 7, 8, 9 for test
    OH_Drawing_MatrixSetMatrix(matrix, 1, 2, 3, 4, 5, 6, 7, 8, 9);
}

/*
@CaseID:SUB_BASIC_GRAPHICS_SPECIAL_MEMORY_NDK_DRAWING_MATRIX_0600
@Description:Matrix相关操作接口随机循环调用
@Step:
    1、运行脚本，循环执行hidumper命令查询内存
    2、Matrix相关操作接口随机循环调用1000次
    3、执行结束后等待3分钟
    4、结束脚本运行，抓取的内存值转成折线图，观察内存变化
@Result:
    1、程序运行正常，无crash
    2、内存存平稳，没有持续增长，执行后内存回落的到执行前
 */
void StabilityMatrixRandom::OnTestStability(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("StabilityMatrixRandom OnTestStability");
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreateScale(1, 1, 0, 0);
    for (int i = 0; i < testCount_; i++) {
        handlers_[rand_.nextRangeU(0, OPERATION_FUNCTION_MATRIX_MAX - 1)](matrix);
    }
    OH_Drawing_MatrixDestroy(matrix);
}

void StabilityMatrixRandom::MatrixSetRectToRectTest(OH_Drawing_Matrix* matrix)
{
    OH_Drawing_MatrixSetRectToRect(
        matrix, rectSrc_, rectDst_, DrawGetEnum(SCALE_TO_FIT_FILL, SCALE_TO_FIT_END, rand_.nextU()));
}

void StabilityMatrixRandom::MatrixSetPolyToPolyTest(OH_Drawing_Matrix* matrix)
{
    // 4 ployto ploy points count
    int count = 4;
    // 0,100 for test
    OH_Drawing_Point2D src[] = { { 0, 0 }, { 100, 0 }, { 0, 100 }, { 100, 100 } };
    // 20,200 for test
    OH_Drawing_Point2D dst[] = { { 20, 20 }, { 20, 200 }, { 200, 20 }, { 200, 200 } };
    OH_Drawing_MatrixSetPolyToPoly(matrix, src, dst, count);
}

void StabilityMatrixRandom::MatrixPreRotateTest(OH_Drawing_Matrix* matrix)
{
    OH_Drawing_MatrixPreRotate(matrix, rand_.nextF(), rand_.nextF(), rand_.nextF());
}

void StabilityMatrixRandom::MatrixPreScaleTest(OH_Drawing_Matrix* matrix)
{
    OH_Drawing_MatrixPreScale(matrix, rand_.nextF(), rand_.nextF(), rand_.nextF(), rand_.nextF());
}

void StabilityMatrixRandom::MatrixPreTranslateTest(OH_Drawing_Matrix* matrix)
{
    OH_Drawing_MatrixPreTranslate(matrix, rand_.nextF(), rand_.nextF());
}

void StabilityMatrixRandom::MatrixPostRotateTest(OH_Drawing_Matrix* matrix)
{
    OH_Drawing_MatrixPostRotate(matrix, rand_.nextF(), rand_.nextF(), rand_.nextF());
}

void StabilityMatrixRandom::MatrixPostScaleTest(OH_Drawing_Matrix* matrix)
{
    OH_Drawing_MatrixPostScale(matrix, rand_.nextF(), rand_.nextF(), rand_.nextF(), rand_.nextF());
}

void StabilityMatrixRandom::MatrixPostTranslateTest(OH_Drawing_Matrix* matrix)
{
    OH_Drawing_MatrixPostTranslate(matrix, rand_.nextF(), rand_.nextF());
}

void StabilityMatrixRandom::MatrixResetTest(OH_Drawing_Matrix* matrix)
{
    OH_Drawing_MatrixReset(matrix);
}

void StabilityMatrixRandom::MatrixConcatTest(OH_Drawing_Matrix* matrix)
{
    OH_Drawing_MatrixConcat(matrix, matrixA_, matrixB_);
}

void StabilityMatrixRandom::MatrixGetAllTest(OH_Drawing_Matrix* matrix)
{
    float value[9];
    OH_Drawing_MatrixGetAll(matrix, value);
}

void StabilityMatrixRandom::MatrixGetValueTest(OH_Drawing_Matrix* matrix)
{
    // 9 matrix = 3 * 3
    for (int i = 0; i < 9; i++) {
        OH_Drawing_MatrixGetValue(matrix, i);
    }
}

void StabilityMatrixRandom::MatrixRotateTest(OH_Drawing_Matrix* matrix)
{
    OH_Drawing_MatrixRotate(matrix, rand_.nextF(), rand_.nextF(), rand_.nextF());
}

void StabilityMatrixRandom::MatrixTranslateTest(OH_Drawing_Matrix* matrix)
{
    OH_Drawing_MatrixTranslate(matrix, rand_.nextF(), rand_.nextF());
}

void StabilityMatrixRandom::MatrixScaleTest(OH_Drawing_Matrix* matrix)
{
    OH_Drawing_MatrixScale(matrix, rand_.nextF(), rand_.nextF(), rand_.nextF(), rand_.nextF());
}

void StabilityMatrixRandom::MatrixInvertTest(OH_Drawing_Matrix* matrix)
{
    OH_Drawing_MatrixInvert(matrix, matrixA_);
}

void StabilityMatrixRandom::MatrixMapPointsTest(OH_Drawing_Matrix* matrix)
{
    int count = 4;
    // 0,100 for test
    OH_Drawing_Point2D src[] = { { 0, 0 }, { 100, 0 }, { 0, 100 }, { 100, 100 } };
    // 20,200 for test
    OH_Drawing_Point2D dst[] = { { 20, 20 }, { 20, 200 }, { 200, 20 }, { 200, 200 } };
    OH_Drawing_MatrixMapPoints(matrix, src, dst, count);
}

void StabilityMatrixRandom::MatrixMapRectTest(OH_Drawing_Matrix* matrix)
{
    OH_Drawing_MatrixMapRect(matrix, rectSrc_, rectDst_);
}

void StabilityMatrixRandom::MatrixIsEqualTest(OH_Drawing_Matrix* matrix)
{
    OH_Drawing_MatrixIsEqual(matrix, matrixB_);
}

void StabilityMatrixRandom::MatrixIsIdentityTest(OH_Drawing_Matrix* matrix)
{
    OH_Drawing_MatrixIsIdentity(matrix);
}