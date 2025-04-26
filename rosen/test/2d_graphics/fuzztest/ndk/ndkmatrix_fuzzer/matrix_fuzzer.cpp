/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#include "matrix_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "get_object.h"

#include "drawing_matrix.h"
#include "drawing_rect.h"
#include "drawing_types.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t DATA_MIN_SIZE = 2;
constexpr size_t FIT_MODE_ENUM_SIZE = 4;
constexpr size_t SIZE_OF_BUFFER = 9;
constexpr uint32_t MAX_ARRAY_SIZE = 5000;
} // namespace

namespace Drawing {
void NativeMatrixTest001(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    float degree = GetObject<float>();
    float x = GetObject<float>();
    float y = GetObject<float>();
    float px = GetObject<float>();
    float py = GetObject<float>();
    float left = GetObject<float>();
    float top = GetObject<float>();
    float right = GetObject<float>();
    float bottom = GetObject<float>();
    uint32_t fitMode = GetObject<uint32_t>();

    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreate();
    OH_Drawing_Matrix* matrixTwo = OH_Drawing_MatrixCreateRotation(degree, x, y);

    OH_Drawing_Matrix* matrixThree = OH_Drawing_MatrixCreateScale(x, y, px, py);
    OH_Drawing_Matrix* matrixFour = OH_Drawing_MatrixCreateTranslation(x, y);
    OH_Drawing_Rect* rectOne = OH_Drawing_RectCreate(left, top, right, bottom);
    OH_Drawing_Rect* rectTwo = OH_Drawing_RectCreate(left, top, right, bottom);
    OH_Drawing_MatrixSetRectToRect(nullptr,
        rectOne, rectTwo, static_cast<OH_Drawing_ScaleToFit>(fitMode % FIT_MODE_ENUM_SIZE));
    OH_Drawing_MatrixSetRectToRect(matrix,
        rectOne, rectTwo, static_cast<OH_Drawing_ScaleToFit>(fitMode % FIT_MODE_ENUM_SIZE));
    OH_Drawing_MatrixMapRect(nullptr, rectOne, rectTwo);
    OH_Drawing_MatrixMapRect(matrix, nullptr, rectTwo);
    OH_Drawing_MatrixMapRect(matrix, rectOne, rectTwo);

    OH_Drawing_MatrixDestroy(matrix);
    OH_Drawing_MatrixDestroy(matrixTwo);
    OH_Drawing_MatrixDestroy(matrixThree);
    OH_Drawing_MatrixDestroy(matrixFour);
    OH_Drawing_RectDestroy(rectOne);
    OH_Drawing_RectDestroy(rectTwo);
}

void NativeMatrixTest002(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    float degree = GetObject<float>();
    float x = GetObject<float>();
    float y = GetObject<float>();
    float px = GetObject<float>();
    float py = GetObject<float>();
    float scaleX = GetObject<float>();
    float skewX = GetObject<float>();
    float transX = GetObject<float>();
    float skewY = GetObject<float>();
    float scaleY = GetObject<float>();
    float transY = GetObject<float>();
    float persp0 = GetObject<float>();
    float persp1 = GetObject<float>();
    float persp2 = GetObject<float>();
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixSetMatrix(nullptr, scaleX, skewX, transX, skewY, scaleY, transY, persp0, persp1, persp2);
    OH_Drawing_MatrixSetMatrix(matrix, scaleX, skewX, transX, skewY, scaleY, transY, persp0, persp1, persp2);
    OH_Drawing_MatrixPreRotate(nullptr, degree, px, py);
    OH_Drawing_MatrixPreRotate(matrix, degree, px, py);
    OH_Drawing_MatrixPreScale(nullptr, transX, transY, px, py);
    OH_Drawing_MatrixPreScale(matrix, transX, transY, px, py);
    OH_Drawing_MatrixPreTranslate(nullptr, x, y);
    OH_Drawing_MatrixPreTranslate(matrix, x, y);
    OH_Drawing_MatrixPostRotate(nullptr, degree, px, py);
    OH_Drawing_MatrixPostRotate(matrix, degree, px, py);
    OH_Drawing_MatrixPostScale(nullptr, transX, transY, px, py);
    OH_Drawing_MatrixPostScale(matrix, transX, transY, px, py);
    OH_Drawing_MatrixPostTranslate(nullptr, x, y);
    OH_Drawing_MatrixPostTranslate(matrix, x, y);
    OH_Drawing_MatrixReset(nullptr);
    OH_Drawing_MatrixReset(matrix);
    OH_Drawing_MatrixDestroy(matrix);
}

void NativeMatrixTest003(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    float degree = GetObject<float>();
    float x = GetObject<float>();
    float y = GetObject<float>();
    float px = GetObject<float>();
    float py = GetObject<float>();
    float index = GetObject<float>();

    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreate();
    OH_Drawing_Matrix* matrixTwo = OH_Drawing_MatrixCreate();
    OH_Drawing_Matrix* matrixThree = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixConcat(nullptr, matrixTwo, matrixThree);
    OH_Drawing_MatrixConcat(matrix, matrixTwo, matrixThree);

    OH_Drawing_MatrixGetValue(nullptr, index);
    OH_Drawing_MatrixGetValue(matrix, GetObject<int32_t>());

    OH_Drawing_MatrixRotate(nullptr, degree, px, py);
    OH_Drawing_MatrixRotate(matrix, degree, px, py);

    OH_Drawing_MatrixTranslate(nullptr, x, y);
    OH_Drawing_MatrixTranslate(matrix, x, y);

    OH_Drawing_MatrixInvert(nullptr, matrixTwo);
    OH_Drawing_MatrixInvert(matrix, nullptr);
    OH_Drawing_MatrixInvert(matrix, matrixTwo);

    OH_Drawing_MatrixIsEqual(nullptr, matrixThree);
    OH_Drawing_MatrixIsEqual(matrixTwo, nullptr);
    OH_Drawing_MatrixIsEqual(matrixTwo, matrixThree);

    OH_Drawing_MatrixIsIdentity(nullptr);
    OH_Drawing_MatrixIsIdentity(matrix);

    OH_Drawing_MatrixDestroy(matrix);
    OH_Drawing_MatrixDestroy(matrixTwo);
    OH_Drawing_MatrixDestroy(matrixThree);
}

void NativeMatrixTest004(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    float px = GetObject<float>();
    float py = GetObject<float>();
    float transX = GetObject<float>();
    float transY = GetObject<float>();

    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixScale(nullptr, transX, transY, px, py);
    OH_Drawing_MatrixScale(matrix, transX, transY, px, py);

    uint32_t arraySize = GetObject<uint32_t>() % MAX_ARRAY_SIZE;
    OH_Drawing_Point2D* pointOne = new OH_Drawing_Point2D[arraySize];
    for (size_t i = 0; i < arraySize; i++) {
        pointOne[i] = {GetObject<float>(), GetObject<float>()};
    }
    OH_Drawing_Point2D* pointTwo = new OH_Drawing_Point2D[arraySize];
    for (size_t i = 0; i < arraySize; i++) {
        pointTwo[i] = {GetObject<float>(), GetObject<float>()};
    }

    OH_Drawing_MatrixSetPolyToPoly(nullptr, pointOne, pointTwo, arraySize);
    OH_Drawing_MatrixSetPolyToPoly(matrix, pointOne, pointTwo, arraySize);
    OH_Drawing_MatrixSetPolyToPoly(matrix, pointOne, pointTwo, -arraySize);

    OH_Drawing_MatrixMapPoints(matrix, pointOne, pointTwo, arraySize);
    OH_Drawing_MatrixMapPoints(matrix, nullptr, nullptr, -arraySize);
    OH_Drawing_MatrixMapPoints(nullptr, pointOne, pointTwo, arraySize);

    if (pointOne != nullptr) {
        delete[] pointOne;
        pointOne = nullptr;
    }
    if (pointTwo != nullptr) {
        delete[] pointTwo;
        pointTwo = nullptr;
    }
    OH_Drawing_MatrixDestroy(matrix);
}

void NativeMatrixTest005(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    float scaleX = GetObject<float>();
    float skewX = GetObject<float>();
    float transX = GetObject<float>();
    float skewY = GetObject<float>();
    float scaleY = GetObject<float>();
    float transY = GetObject<float>();
    float persp0 = GetObject<float>();
    float persp1 = GetObject<float>();
    float persp2 = GetObject<float>();
    float value[SIZE_OF_BUFFER];
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixSetMatrix(matrix, scaleX, skewX, transX, skewY, scaleY, transY, persp0, persp1, persp2);
    OH_Drawing_MatrixGetAll(matrix, value);
    OH_Drawing_MatrixGetAll(nullptr, value);

    OH_Drawing_MatrixDestroy(matrix);
}

void NativeMatrixTest006(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    float scaleX = GetObject<float>();
    float skewX = GetObject<float>();
    float transX = GetObject<float>();
    float skewY = GetObject<float>();
    float scaleY = GetObject<float>();
    float transY = GetObject<float>();
    float persp0 = GetObject<float>();
    float persp1 = GetObject<float>();
    float persp2 = GetObject<float>();
    float value[SIZE_OF_BUFFER];
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreate();
    OH_Drawing_Matrix* matrix2 = OH_Drawing_MatrixCopy(matrix);
    OH_Drawing_MatrixSetMatrix(matrix2, scaleX, skewX, transX, skewY, scaleY, transY, persp0, persp1, persp2);
    OH_Drawing_MatrixGetAll(matrix2, value);
    OH_Drawing_MatrixGetAll(nullptr, value);

    OH_Drawing_MatrixDestroy(matrix);
    OH_Drawing_MatrixDestroy(matrix2);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::NativeMatrixTest001(data, size);
    OHOS::Rosen::Drawing::NativeMatrixTest002(data, size);
    OHOS::Rosen::Drawing::NativeMatrixTest003(data, size);
    OHOS::Rosen::Drawing::NativeMatrixTest004(data, size);
    OHOS::Rosen::Drawing::NativeMatrixTest005(data, size);
    OHOS::Rosen::Drawing::NativeMatrixTest006(data, size);
    return 0;
}
