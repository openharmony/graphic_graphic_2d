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

#include "colormatrix_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "get_object.h"
#include "effect/color_matrix.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t MATRIX_SIZE = 20;
} // namespace
namespace Drawing {
/*
 * 测试以下 ColorMatrix 接口：
 * 1. SetIdentity()
 * 2. SetScale(scalar sr, scalar sg, scalar sb, scalar sa)
 * 3. SetSaturation(scalar sat)
 */
bool ColorMatrixFuzzTest001(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    ColorMatrix colorMatrix;
    colorMatrix.SetIdentity();
    scalar sr = GetObject<scalar>();
    scalar sg = GetObject<scalar>();
    scalar sb = GetObject<scalar>();
    scalar sa = GetObject<scalar>();
    colorMatrix.SetScale(sr, sg, sb, sa);
    scalar sat = GetObject<scalar>();
    colorMatrix.SetSaturation(sat);
    return true;
}

/*
 * 测试以下 ColorMatrix 接口：
 * 1. ColorMatrix()
 * 2. SetArray(scalar src[MATRIX_SIZE])
 * 3. GetArray(scalar dst[MATRIX_SIZE])
 */
bool ColorMatrixFuzzTest002(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    ColorMatrix colorMatrix;
    scalar src[MATRIX_SIZE];
    for (size_t i = 0; i < MATRIX_SIZE; i++) {
        src[i] = GetObject<scalar>();
    }
    colorMatrix.SetArray(src);
    scalar dst[MATRIX_SIZE];
    for (size_t i = 0; i < MATRIX_SIZE; i++) {
        dst[i] = GetObject<scalar>();
    }
    colorMatrix.GetArray(dst);
    return true;
}

/*
 * 测试以下 ColorMatrix 接口：
 * 1. ColorMatrix()
 * 2. SetArray(scalar src[MATRIX_SIZE])
 * 3. SetConcat(const ColorMatrix& a, const ColorMatrix& b)
 * 4. PreConcat(const ColorMatrix& a)
 * 5. PostConcat(const ColorMatrix& a)
 */
bool ColorMatrixFuzzTest003(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    ColorMatrix colorMatrix;
    scalar src[MATRIX_SIZE];
    for (size_t i = 0; i < MATRIX_SIZE; i++) {
        src[i] = GetObject<scalar>();
    }
    colorMatrix.SetArray(src);
    ColorMatrix colorMatrixOne;
    scalar src2[MATRIX_SIZE];
    for (size_t i = 0; i < MATRIX_SIZE; i++) {
        src2[i] = GetObject<scalar>();
    }
    colorMatrixOne.SetArray(src2);
    ColorMatrix colorMatrixTwo;
    scalar src3[MATRIX_SIZE];
    for (size_t i = 0; i < MATRIX_SIZE; i++) {
        src3[i] = GetObject<scalar>();
    }
    colorMatrixTwo.SetArray(src3);
    colorMatrix.SetConcat(colorMatrixOne, colorMatrixTwo);
    colorMatrix.PreConcat(colorMatrixOne);
    colorMatrix.PostConcat(colorMatrixOne);

    return true;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::ColorMatrixFuzzTest001(data, size);
    OHOS::Rosen::Drawing::ColorMatrixFuzzTest002(data, size);
    OHOS::Rosen::Drawing::ColorMatrixFuzzTest003(data, size);
    return 0;
}
