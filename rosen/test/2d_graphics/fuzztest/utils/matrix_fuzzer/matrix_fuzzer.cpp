/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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
#include <securec.h>

#include "get_object.h"
#include "utils/matrix.h"
#include "utils/scalar.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t ELEMENT_SIZE = 9;
constexpr size_t ARRAY_MAX_SIZE = 5000;
constexpr size_t MATH_TWO = 2;
constexpr size_t MATH_FORE = 4;
constexpr size_t MATH_NINE = 9;
} // namespace

namespace Drawing {
/*
 * 测试以下 Matrix 接口：
 * 1. Get
 * 2. Skew
 * 3. Rotate
 * 4. Translate
 * 5. Scale
 * 6. SetScale
 * 7. SetScaleTranslate
 * 8. SetSkew
 * 9. PreRotate
 * 10. PostRotate
 * 11. PreTranslate
 * 12. PostTranslate
 * 13. PreScale
 * 14. PostScale
 * 15. PreSkew
 * 16. PostSkew
 */
bool MatrixFuzzTest000(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint32_t index = GetObject<uint32_t>();
    scalar sx = GetObject<scalar>();
    scalar sy = GetObject<scalar>();
    scalar dx = GetObject<scalar>();
    scalar dy = GetObject<scalar>();

    Matrix matrix;
    if (index < ELEMENT_SIZE) { // default matrix is 3x3 identity matrix
        matrix.Get(index);
        return true;
    }
    matrix.Skew(sx, sy);
    matrix.Rotate(sx, sy, dx);
    matrix.Translate(dx, dy);
    matrix.Scale(sx, sy, dx, dy);
    matrix.SetScale(sx, sy);
    matrix.SetScaleTranslate(sx, sy, dx, dy);
    matrix.SetSkew(sx, sy);
    matrix.SetSkew(sx, sy, dx, dy);
    matrix.PreRotate(dx);
    matrix.PostRotate(dx);
    matrix.PostRotate(sx, sy, dx);
    matrix.PreTranslate(dx, dy);
    matrix.PostTranslate(dx, dy);
    matrix.PreScale(sx, sy);
    matrix.PostScale(sx, sy);
    matrix.PreSkew(dx, dy);
    matrix.PostSkew(dx, dy);
    matrix.PreSkew(dx, dy, sx, sy);
    matrix.PostSkew(dx, dy, sx, sy);
    matrix.PostScale(dx, dy, sx, sy);
    return false;
}

/*
 * 测试以下 Matrix 接口：
 * 1. PreConcat
 * 2. PostConcat
 * 3. Invert
 * 4. SetMatrix
 * 5. SetRectToRect
 * 6. MapRect
 * 7. Set
 * 8. IsIdentity
 * 9. Reset
 * 10. HasPerspective
 * 11. Swap
 * 12. 操作符==
 * 13. 操作符*
 */
void MatrixFuzzTest001(const uint8_t* data, size_t size)
{
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    scalar sx = GetObject<scalar>();
    scalar sy = GetObject<scalar>();
    scalar sz = GetObject<scalar>();
    scalar dx = GetObject<scalar>();
    scalar dy = GetObject<scalar>();
    scalar dz = GetObject<scalar>();
    scalar tx = GetObject<scalar>();
    scalar ty = GetObject<scalar>();
    scalar tz = GetObject<scalar>();
    uint32_t stf = GetObject<uint32_t>() % MATH_FORE;
    uint32_t index = GetObject<uint32_t>() % MATH_NINE;

    Matrix matrix;
    Matrix other = Matrix(matrix);
    Matrix other1 = matrix;
    Matrix44 matrix44;
    matrix.PreConcat(other);
    matrix.PreConcat(matrix44);
    matrix.PostConcat(other);
    matrix.PostConcat(matrix44);
    matrix.Invert(other);
    matrix.SetMatrix(tx, ty, tz, sx, sy, sz, dx, dy, dz);
    Rect src;
    Rect dst;
    matrix.SetRectToRect(src, dst, static_cast<ScaleToFit>(stf));
    matrix.MapRect(dst, src);
    matrix.Set(static_cast<Matrix::Index>(index), dx);
    matrix.IsIdentity();
    matrix.PreRotate(dx, dy, sx);
    matrix.PreScale(dx, dy, sx, sy);
    matrix.Reset();
    matrix.HasPerspective();
    matrix.Swap(other);
    if (other == matrix) {}
    Matrix other2 = other * other1;
}

/*
 * 测试以下 Matrix 接口：
 * 1. MapPoints
 * 2. SetPolyToPoly
 * 3. SetAll
 * 4. GetAll
 * 5. GetMinMaxScales
 */
void MatrixFuzzTest002(const uint8_t* data, size_t size)
{
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    scalar xRad = GetObject<scalar>();
    scalar yRad = GetObject<scalar>();
    uint32_t array_size = GetObject<uint32_t>() % ARRAY_MAX_SIZE;

    Matrix matrix;
    std::vector<Point> dst;
    std::vector<Point> src;
    Point src1[array_size];
    Point dst1[array_size];
    for (size_t i = 0; i< array_size; i++) {
        src1[i].Set(GetObject<scalar>(), GetObject<scalar>());
        dst1[i].Set(GetObject<scalar>(), GetObject<scalar>());
        dst.push_back({GetObject<scalar>(), GetObject<scalar>()});
        src.push_back({GetObject<scalar>(), GetObject<scalar>()});
    }
    matrix.MapPoints(dst, src, array_size);
    matrix.SetPolyToPoly(src1, dst1, array_size);
    Matrix::Buffer buffer;
    for (size_t i = 0; i < Matrix::MATRIX_SIZE; i++) {
        buffer[i] = GetObject<scalar>();
    }
    matrix.SetAll(buffer);
    matrix.GetAll(buffer);
    scalar scaleFactors[MATH_TWO] = {xRad, yRad};
    matrix.GetMinMaxScales(scaleFactors);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::MatrixFuzzTest000(data, size);
    OHOS::Rosen::Drawing::MatrixFuzzTest001(data, size);
    OHOS::Rosen::Drawing::MatrixFuzzTest002(data, size);
    return 0;
}
