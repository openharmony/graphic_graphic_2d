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
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_filter.h>
#include <native_drawing/drawing_image.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_path_effect.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_sampling_options.h>
#include <native_drawing/drawing_shader_effect.h>

#include "test_common.h"

#include "common/log_common.h"

void MatrixReset::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_PathAddRect(path, 0, 0, 100, 100, OH_Drawing_PathDirection::PATH_DIRECTION_CW); // 100 矩形宽高
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixSetMatrix(matrix, 50, 0, 0, 0, 50, 0, 0, 0, 50); // 50 matrix值

    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_MatrixReset(matrix);
    }
    OH_Drawing_PathTransform(path, matrix);
    OH_Drawing_CanvasDrawPath(canvas, path);
    OH_Drawing_MatrixDestroy(matrix);
    OH_Drawing_PathDestroy(path);
}

void MatrixConcat::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_Matrix* matrix_a = OH_Drawing_MatrixCreate();
    OH_Drawing_Matrix* matrix_b = OH_Drawing_MatrixCreate();
    OH_Drawing_Matrix* total = OH_Drawing_MatrixCreate();
    OH_Drawing_PathAddRect(path, 0, 0, 100, 100, OH_Drawing_PathDirection::PATH_DIRECTION_CW); // 100 宽高
    OH_Drawing_MatrixSetMatrix(matrix_a, 50, 0, 0, 0, 50, 0, 0, 0, 50); // 50 matrix值
    OH_Drawing_MatrixSetMatrix(matrix_b, 60, 0, 0, 0, 60, 0, 0, 0, 60); // 60 matrix值

    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_MatrixConcat(total, matrix_a, matrix_b);
    }
    OH_Drawing_PathTransform(path, total);
    OH_Drawing_CanvasDrawPath(canvas, path);
    OH_Drawing_MatrixDestroy(total);
    OH_Drawing_MatrixDestroy(matrix_a);
    OH_Drawing_MatrixDestroy(matrix_b);
    OH_Drawing_PathDestroy(path);
}

void MatrixInvert::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreate();
    OH_Drawing_Matrix* inverse = OH_Drawing_MatrixCreate();
    OH_Drawing_PathAddRect(path, 0, 0, 100, 100, OH_Drawing_PathDirection::PATH_DIRECTION_CW); // 100 宽高
    OH_Drawing_MatrixSetMatrix(matrix, 50, 0, 0, 0, 50, 0, 0, 0, 50); // 50 matrix值

    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_MatrixInvert(matrix, inverse);
    }
    OH_Drawing_PathTransform(path, inverse);
    OH_Drawing_CanvasDrawPath(canvas, path);
    OH_Drawing_MatrixDestroy(matrix);
    OH_Drawing_MatrixDestroy(inverse);
    OH_Drawing_PathDestroy(path);
}

void MatrixSetPolyToPoly::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    int count = 4;
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_PathAddRect(path, 0, 0, 100, 100, OH_Drawing_PathDirection::PATH_DIRECTION_CW); // 100 宽高
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreate();
    OH_Drawing_Point2D src[] = { { 0, 0 }, { 100, 0 }, { 0, 100 }, { 100, 100 } }; // 0, 100 坐标值
    OH_Drawing_Point2D dst[] = { { 20, 20 }, { 20, 200 }, { 200, 20 }, { 200, 200 } }; // 20, 200 坐标值
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_MatrixSetPolyToPoly(matrix, src, dst, count);
    }
    OH_Drawing_PathTransform(path, matrix);
    OH_Drawing_CanvasDrawPath(canvas, path);
    OH_Drawing_MatrixDestroy(matrix);
    OH_Drawing_PathDestroy(path);
}
void MatrixPreRotate::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    // 性能测试:10000次4ms
    OH_Drawing_Matrix* pMatrix = OH_Drawing_MatrixCreate();
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixSetMatrix(matrix, 2, -1, 2, 5, -4, 5, 8, -7, 8); // 2, -1, 2, 5, -4, 5, 8, -7, 8矩阵参数
    // 将矩阵设置为矩阵左乘围绕轴心点旋转一定角度的单位矩阵后得到的矩阵
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_MatrixSetMatrix(pMatrix, 1, 2, 3, 4, 5, 6, 7, 8, 9); // 1, 2, 3, 4, 5, 6, 7, 8, 9 矩阵参数
        OH_Drawing_MatrixPreRotate(pMatrix, 90, 1, 0);                  // 90, 1矩阵角度和坐标
    };
    // 观察pMatrix转换后与matrix是否一样.来判断OH_Drawing_MatrixPreRotate接口是否调用成功
    bool result = false;
    result = OH_Drawing_MatrixIsEqual(pMatrix, matrix);
    if (result == true) {
        DRAWING_LOGI("The OH_Drawing_MatrixPreRotate interface returns a value of true,result=%{public}s",
            result ? "true" : "false");
        OH_Drawing_Rect* rect = OH_Drawing_RectCreate(0, 0, 100, 100); // 0, 0, 100, 100 创建矩形
        OH_Drawing_CanvasDrawRect(canvas, rect);
        OH_Drawing_RectDestroy(rect);
    } else {
        DRAWING_LOGI("The OH_Drawing_MatrixPreRotate interface returns a value of failed,result=%{public}s",
            result ? "true" : "false");
    }
    // 释放
    OH_Drawing_MatrixDestroy(pMatrix);
    OH_Drawing_MatrixDestroy(matrix);
}

void MatrixPostScale::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    // 性能测试:10000次27ms
    OH_Drawing_Matrix* pMatrix = OH_Drawing_MatrixCreate();
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixSetMatrix(matrix, -37, -11, -3, -99, -23, -23, 3, 1, 1); // -37, -11, -3, -99, -23, 3, 1矩阵参数
    // 将矩阵设置为矩阵右乘围绕轴心点按一定缩放因子缩放后的单位矩阵后得到的矩阵
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_MatrixSetMatrix(pMatrix, 2, 1, 3, 1, 2, 2, 3, 1, 1); // 2, 1, 3, 1, 2, 2, 3, 1矩阵参数
        OH_Drawing_MatrixPostScale(pMatrix, 4, 6, 5, 7);                // 4, 6, 5, 7缩放比例因子和坐标
    };
    // 观察pMatrix转换后与matrix是否一样.来判断OH_Drawing_MatrixPostScale接口是否调用成功
    bool result = false;
    result = OH_Drawing_MatrixIsEqual(pMatrix, matrix);
    if (result == true) {
        DRAWING_LOGI("The OH_Drawing_MatrixPostScale interface returns a value of true,result=%{public}s",
            result ? "true" : "false");
        OH_Drawing_Rect* rect = OH_Drawing_RectCreate(0, 0, 100, 100); // 0, 0, 100, 100 创建矩形
        OH_Drawing_CanvasDrawRect(canvas, rect);
        OH_Drawing_RectDestroy(rect);
    } else {
        DRAWING_LOGI("The OH_Drawing_MatrixPostScale interface returns a value of failed,result=%{public}s",
            result ? "true" : "false");
    }
    // 释放
    OH_Drawing_MatrixDestroy(pMatrix);
    OH_Drawing_MatrixDestroy(matrix);
}

void MatrixPostTranslate::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    // 性能测试:10000次2ms
    OH_Drawing_Matrix* pMatrix = OH_Drawing_MatrixCreate();
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixSetMatrix(matrix, 8, 3, 5, 13, 6, 6, 3, 1, 1); // 8, 3, 5, 13, 6, 6, 3, 1矩阵参数
    // 将矩阵设置为矩阵右乘平移一定距离后的单位矩阵后得到的矩阵.
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_MatrixSetMatrix(pMatrix, 2, 1, 3, 1, 2, 2, 3, 1, 1); // 2, 1, 3, 1, 2, 2, 3, 1矩阵参数
        OH_Drawing_MatrixPostTranslate(pMatrix, 2, 4);                  // 2, 4平移距离
    }
    // 观察pMatrix转换后与matrix是否一样.来判断OH_Drawing_MatrixPostTranslate接口是否调用成功
    bool result = false;
    result = OH_Drawing_MatrixIsEqual(pMatrix, matrix);
    if (result == true) {
        DRAWING_LOGI("The OH_Drawing_MatrixPostTranslate interface returns a value of true,result=%{public}s",
            result ? "true" : "false");
        OH_Drawing_Rect* rect = OH_Drawing_RectCreate(0, 0, 100, 100); // 0, 0, 100, 100 创建矩形
        OH_Drawing_CanvasDrawRect(canvas, rect);
        OH_Drawing_RectDestroy(rect);
    } else {
        DRAWING_LOGI("The OH_Drawing_MatrixPostTranslate interface returns a value of failed,result=%{public}s",
            result ? "true" : "false");
    }
    // 释放
    OH_Drawing_MatrixDestroy(pMatrix);
    OH_Drawing_MatrixDestroy(matrix);
}

void MatrixIsEqual::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Matrix* matrix1 = OH_Drawing_MatrixCreate();
    OH_Drawing_Matrix* matrix2 = OH_Drawing_MatrixCreate();
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(0, 0, 100, 100); // 0, 0, 100, 100 创建矩形
    bool matrixIsEqual = false;
    for (int i = 0; i < testCount_; i++) {
        matrixIsEqual = OH_Drawing_MatrixIsEqual(matrix1, matrix2);
    }
    if (matrixIsEqual) {
        OH_Drawing_CanvasDrawRect(canvas, rect);
    }
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_MatrixDestroy(matrix1);
    OH_Drawing_MatrixDestroy(matrix2);
}