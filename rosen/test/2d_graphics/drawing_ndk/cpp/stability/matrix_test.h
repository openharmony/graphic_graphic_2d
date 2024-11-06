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

#ifndef STABILITY_MATRIX_TEST_H
#define STABILITY_MATRIX_TEST_H

#include <map>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_rect.h>

#include "test_base.h"
#include "test_common.h"

typedef enum {
    // Operation function
    OPERATION_FUNCTION_MATRIX_SET_RECT_TO_RECT = 0,
    OPERATION_FUNCTION_MATRIX_SET_POLY_TO_POLY,
    OPERATION_FUNCTION_MATRIX_PRE_ROTATE,
    OPERATION_FUNCTION_MATRIX_PRE_SCALE,
    OPERATION_FUNCTION_MATRIX_PRE_TRANSLATE,
    OPERATION_FUNCTION_MATRIX_POST_ROTATE,
    OPERATION_FUNCTION_MATRIX_POST_SCALE,
    OPERATION_FUNCTION_MATRIX_POST_TRANSLATE,
    OPERATION_FUNCTION_MATRIX_RESET,
    OPERATION_FUNCTION_MATRIX_CONCAT,
    OPERATION_FUNCTION_MATRIX_GET_ALL,
    OPERATION_FUNCTION_MATRIX_GET_VALUE,
    OPERATION_FUNCTION_MATRIX_ROTATE,
    OPERATION_FUNCTION_MATRIX_TRANSLATE,
    OPERATION_FUNCTION_MATRIX_SCALE,
    OPERATION_FUNCTION_MATRIX_INVERT,
    OPERATION_FUNCTION_MATRIX_MAP_POINTS,
    OPERATION_FUNCTION_MATRIX_MAP_RECT,
    OPERATION_FUNCTION_MATRIX_IS_EQUAL,
    OPERATION_FUNCTION_MATRIX_IS_IDENTITY,
    OPERATION_FUNCTION_MATRIX_MAX,
} OperationFunctionMatrix;

typedef enum {
    // setting function
    SETTING_FUNCTION_MATRIX_SET_MATRIX = 0,
    SETTING_FUNCTION_MATRIX_MAX
} SettingFunctionMatrix;

// MatrixCreate-MatrixDestroy循环
class StabilityMatrixCreate : public TestBase {
public:
    StabilityMatrixCreate()
    {
        fileName_ = "StabilityMatrixCreate";
    }
    ~StabilityMatrixCreate() override {};

protected:
    void OnTestStability(OH_Drawing_Canvas* canvas) override;
};

// MatrixCreateRotation-MatrixDestroy循环
class StabilityMatrixCreateRotation : public TestBase {
public:
    StabilityMatrixCreateRotation()
    {
        fileName_ = "StabilityMatrixCreateRotation";
    }
    ~StabilityMatrixCreateRotation() override {};

protected:
    void OnTestStability(OH_Drawing_Canvas* canvas) override;
};

// MatrixCreateScale-MatrixDestroy循环
class StabilityMatrixCreateScale : public TestBase {
public:
    StabilityMatrixCreateScale()
    {
        fileName_ = "StabilityMatrixCreateScale";
    }
    ~StabilityMatrixCreateScale() override {};

protected:
    void OnTestStability(OH_Drawing_Canvas* canvas) override;
};

// MatrixCreateTranslation-MatrixDestroy循环
class StabilityMatrixCreateTranslation : public TestBase {
public:
    StabilityMatrixCreateTranslation()
    {
        fileName_ = "StabilityMatrixCreateTranslation";
    }
    ~StabilityMatrixCreateTranslation() override {};

protected:
    void OnTestStability(OH_Drawing_Canvas* canvas) override;
};

// MatrixCreate-相关配置接口全调用-MatrixDestroy循环
class StabilityMatrixAll : public TestBase {
public:
    StabilityMatrixAll()
    {
        fileName_ = "StabilityMatrixAll";
    }
    ~StabilityMatrixAll() override {};

protected:
    void OnTestStability(OH_Drawing_Canvas* canvas) override;
    void MatrixSetMatrixTest(OH_Drawing_Matrix* matrix);
    using HandlerFunc = std::function<void(OH_Drawing_Matrix*)>;
    std::map<uint32_t, HandlerFunc> handlers_ = {
        { SETTING_FUNCTION_MATRIX_SET_MATRIX,
            [this](OH_Drawing_Matrix* Matrix) { return MatrixSetMatrixTest(Matrix); } },
    };
};

// Matrix相关操作接口随机循环调用
class StabilityMatrixRandom : public TestBase {
public:
    StabilityMatrixRandom()
    {
        fileName_ = "StabilityMatrixRandom";
        // 0,0,200,200 for test
        rectSrc_ = OH_Drawing_RectCreate(0, 0, 200, 200);
        // 200,200,500,500 for test
        rectDst_ = OH_Drawing_RectCreate(200, 200, 500, 500);
        // 10, 20 for test
        matrixA_ = OH_Drawing_MatrixCreateTranslation(10, 20);
        // 2, 1 for test
        matrixB_ = OH_Drawing_MatrixCreateScale(2, 1, 0, 0);
    }
    ~StabilityMatrixRandom() override
    {
        OH_Drawing_RectDestroy(rectSrc_);
        OH_Drawing_RectDestroy(rectDst_);
        OH_Drawing_MatrixDestroy(matrixA_);
        OH_Drawing_MatrixDestroy(matrixB_);
    };

protected:
    void OnTestStability(OH_Drawing_Canvas* canvas) override;

private:
    TestRend rand_;
    OH_Drawing_Rect* rectSrc_;
    OH_Drawing_Rect* rectDst_;
    OH_Drawing_Matrix* matrixA_;
    OH_Drawing_Matrix* matrixB_;
    void MatrixSetRectToRectTest(OH_Drawing_Matrix* matrix);
    void MatrixSetPolyToPolyTest(OH_Drawing_Matrix* matrix);
    void MatrixPreRotateTest(OH_Drawing_Matrix* matrix);
    void MatrixPreScaleTest(OH_Drawing_Matrix* matrix);
    void MatrixPreTranslateTest(OH_Drawing_Matrix* matrix);
    void MatrixPostRotateTest(OH_Drawing_Matrix* matrix);
    void MatrixPostScaleTest(OH_Drawing_Matrix* matrix);
    void MatrixPostTranslateTest(OH_Drawing_Matrix* matrix);
    void MatrixResetTest(OH_Drawing_Matrix* matrix);
    void MatrixConcatTest(OH_Drawing_Matrix* matrix);
    void MatrixGetAllTest(OH_Drawing_Matrix* matrix);
    void MatrixGetValueTest(OH_Drawing_Matrix* matrix);
    void MatrixRotateTest(OH_Drawing_Matrix* matrix);
    void MatrixTranslateTest(OH_Drawing_Matrix* matrix);
    void MatrixScaleTest(OH_Drawing_Matrix* matrix);
    void MatrixInvertTest(OH_Drawing_Matrix* matrix);
    void MatrixMapPointsTest(OH_Drawing_Matrix* matrix);
    void MatrixMapRectTest(OH_Drawing_Matrix* matrix);
    void MatrixIsEqualTest(OH_Drawing_Matrix* matrix);
    void MatrixIsIdentityTest(OH_Drawing_Matrix* matrix);
    using HandlerFunc = std::function<void(OH_Drawing_Matrix*)>;
    std::map<uint32_t, HandlerFunc> handlers_ = {
        { OPERATION_FUNCTION_MATRIX_SET_RECT_TO_RECT,
            [this](OH_Drawing_Matrix* Matrix) { return MatrixSetRectToRectTest(Matrix); } },
        { OPERATION_FUNCTION_MATRIX_SET_POLY_TO_POLY,
            [this](OH_Drawing_Matrix* Matrix) { return MatrixSetPolyToPolyTest(Matrix); } },
        { OPERATION_FUNCTION_MATRIX_PRE_ROTATE,
            [this](OH_Drawing_Matrix* Matrix) { return MatrixPreRotateTest(Matrix); } },
        { OPERATION_FUNCTION_MATRIX_PRE_SCALE,
            [this](OH_Drawing_Matrix* Matrix) { return MatrixPreScaleTest(Matrix); } },
        { OPERATION_FUNCTION_MATRIX_PRE_TRANSLATE,
            [this](OH_Drawing_Matrix* Matrix) { return MatrixPreTranslateTest(Matrix); } },
        { OPERATION_FUNCTION_MATRIX_POST_ROTATE,
            [this](OH_Drawing_Matrix* Matrix) { return MatrixPostRotateTest(Matrix); } },
        { OPERATION_FUNCTION_MATRIX_POST_SCALE,
            [this](OH_Drawing_Matrix* Matrix) { return MatrixPostScaleTest(Matrix); } },
        { OPERATION_FUNCTION_MATRIX_POST_TRANSLATE,
            [this](OH_Drawing_Matrix* Matrix) { return MatrixPostTranslateTest(Matrix); } },
        { OPERATION_FUNCTION_MATRIX_RESET, [this](OH_Drawing_Matrix* Matrix) { return MatrixResetTest(Matrix); } },
        { OPERATION_FUNCTION_MATRIX_CONCAT, [this](OH_Drawing_Matrix* Matrix) { return MatrixConcatTest(Matrix); } },
        { OPERATION_FUNCTION_MATRIX_GET_ALL, [this](OH_Drawing_Matrix* Matrix) { return MatrixGetAllTest(Matrix); } },
        { OPERATION_FUNCTION_MATRIX_GET_VALUE,
            [this](OH_Drawing_Matrix* Matrix) { return MatrixGetValueTest(Matrix); } },
        { OPERATION_FUNCTION_MATRIX_ROTATE, [this](OH_Drawing_Matrix* Matrix) { return MatrixRotateTest(Matrix); } },
        { OPERATION_FUNCTION_MATRIX_TRANSLATE,
            [this](OH_Drawing_Matrix* Matrix) { return MatrixTranslateTest(Matrix); } },
        { OPERATION_FUNCTION_MATRIX_SCALE, [this](OH_Drawing_Matrix* Matrix) { return MatrixScaleTest(Matrix); } },
        { OPERATION_FUNCTION_MATRIX_INVERT, [this](OH_Drawing_Matrix* Matrix) { return MatrixInvertTest(Matrix); } },
        { OPERATION_FUNCTION_MATRIX_MAP_POINTS,
            [this](OH_Drawing_Matrix* Matrix) { return MatrixMapPointsTest(Matrix); } },
        { OPERATION_FUNCTION_MATRIX_MAP_RECT, [this](OH_Drawing_Matrix* Matrix) { return MatrixMapRectTest(Matrix); } },
        { OPERATION_FUNCTION_MATRIX_IS_EQUAL, [this](OH_Drawing_Matrix* Matrix) { return MatrixIsEqualTest(Matrix); } },
        { OPERATION_FUNCTION_MATRIX_IS_IDENTITY,
            [this](OH_Drawing_Matrix* Matrix) { return MatrixIsIdentityTest(Matrix); } },
    };
};

#endif // STABILITY_MATRIX_TEST_H