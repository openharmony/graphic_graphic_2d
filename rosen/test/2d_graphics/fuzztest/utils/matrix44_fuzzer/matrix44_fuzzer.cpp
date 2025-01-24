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

#include "matrix44_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "get_object.h"
#include "utils/matrix.h"
#include "utils/matrix44.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t MATH_ONE = 1;
} // namespace

namespace Drawing {
/*
 * 测试以下 Matrix44 接口：
 * 1. Translate
 * 2. PreTranslate
 * 3. PostTranslate
 * 4. Scale
 * 5. PreScale
 * 6. SetCol
 */
void Martix44FuzzTest000(const uint8_t* data, size_t size)
{
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    scalar dx = GetObject<scalar>();
    scalar dy = GetObject<scalar>();
    scalar dz = GetObject<scalar>();
    scalar w = GetObject<scalar>();

    Matrix44 matrix;
    matrix.Translate(dx, dy, dz);
    matrix.PreTranslate(dx, dy);
    matrix.PreTranslate(dx, dy, dz);
    matrix.PostTranslate(dx, dy);
    matrix.PostTranslate(dx, dy, dz);
    matrix.Scale(dx, dy, dz);
    matrix.PreScale(dx, dy);
    matrix.PreScale(dx, dy, dz);
    matrix.SetCol(MATH_ONE, dx, dy, dz, w);
}

/*
 * Martix44FuzzTest001(const uint8_t* data, size_t size)
 * 测试以下 Matrix44 接口：
 * 1. SetMatrix44ColMajor
 * 2. SetMatrix44RowMajor
 * 3. 操作符*
 * 4. 静态类型转换为 Matrix
 */
void Martix44FuzzTest001(const uint8_t* data, size_t size)
{
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    scalar w = GetObject<scalar>();

    Matrix44::Buffer buffer;
    for (size_t i = 0; i < Matrix44::MATRIX44_SIZE; i++) {
        buffer[i] = w;
    }
    Matrix44 matrix;
    Matrix44 matrix1;

    matrix.SetMatrix44ColMajor(buffer);
    matrix.SetMatrix44RowMajor(buffer);
    Matrix44 matrix2 = matrix * matrix1;
    Matrix mat = static_cast<Matrix>(matrix1);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::Martix44FuzzTest000(data, size);
    OHOS::Rosen::Drawing::Martix44FuzzTest001(data, size);
    return 0;
}