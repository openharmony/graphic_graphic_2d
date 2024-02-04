/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "effect/color_matrix.h"

#include <securec.h>

#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
// Hue RGB constant
constexpr static float HUE_R = 0.213f;
constexpr static float HUE_G = 0.715f;
constexpr static float HUE_B = 0.072f;

ColorMatrix::ColorMatrix() noexcept
{
    SetIdentity();
}

ColorMatrix::~ColorMatrix() {}

void ColorMatrix::SetIdentity()
{
    for (size_t i = 0; i < MATRIX_SIZE; i = i + 6) {
        array_[i] = 1; // identity matrix, the value of the elements on the main diagonal is 1
    }
}

void ColorMatrix::SetArray(const scalar src[MATRIX_SIZE])
{
    auto ret = memcpy_s(array_, sizeof(array_), src, sizeof(array_));
    if (ret != EOK) {
        LOGD("Drawing: ColorMatrix memcpy_s failed");
    }
}

void ColorMatrix::GetArray(scalar (&dst)[MATRIX_SIZE]) const
{
    auto ret = memcpy_s(dst, sizeof(array_), array_, sizeof(array_));
    if (ret != EOK) {
        LOGD("Drawing: ColorMatrix memcpy_s failed");
    }
}

void ColorMatrix::SetConcat(const ColorMatrix& m1, const ColorMatrix& m2)
{
    scalar tmp[MATRIX_SIZE] = { 0 };
    scalar* target;

    if (array_ == m1.array_ || array_ == m2.array_) {
        target = tmp;
    } else {
        target = array_;
    }

    int index = 0;
    for (int j = 0; j < MATRIX_SIZE; j = j + 5) {
        for (int i = 0; i < 4; i++) { // Color matrix is a 4x5 float type matrix.
            target[index++] = m1.array_[j + 0] * m2.array_[i + 0] + m1.array_[j + 1] * m2.array_[i + 5] +
                m1.array_[j + 2] * m2.array_[i + 10] + m1.array_[j + 3] * m2.array_[i + 15];
        }
        target[index++] = m1.array_[j + 0] * m2.array_[4] + m1.array_[j + 1] * m2.array_[9] +
            m1.array_[j + 2] * m2.array_[14] + m1.array_[j + 3] * m2.array_[19] + m1.array_[j + 4];
    }

    if (target != array_) {
        auto ret = memcpy_s(array_, sizeof(array_), target, sizeof(array_));
        if (ret != EOK) {
            LOGD("Drawing: ColorMatrix memcpy_s failed");
        }
    }
}

void ColorMatrix::PreConcat(const ColorMatrix& m)
{
    SetConcat(*this, m);
}

void ColorMatrix::PostConcat(const ColorMatrix& m)
{
    SetConcat(m, *this);
}

void ColorMatrix::SetScale(scalar sr, scalar sg, scalar sb, scalar sa)
{
    auto ret = memset_s(array_, sizeof(array_), 0, sizeof(array_));
    if (ret != EOK) {
        LOGD("Drawing: ColorMatrix memset_s failed");
        return;
    }
    array_[0] = sr;  // red vector scale
    array_[6] = sg;  // green vector scale
    array_[12] = sb; // blue vector scale
    array_[18] = sa; // alpha vetor scale
}

void ColorMatrix::SetSaturation(scalar sat)
{
    auto ret = memset_s(array_, sizeof(array_), 0, sizeof(array_));
    if (ret != EOK) {
        LOGD("Drawing: ColorMatrix memset_s failed");
        return;
    }

    const float R = HUE_R * (1 - sat);
    const float G = HUE_G * (1 - sat);
    const float B = HUE_B * (1 - sat);

    // red channel
    array_[SCALE_FACTOR_FOR_R] = R + sat;
    array_[G_FACTOR_FOR_R] = G;
    array_[B_FACTOR_FOR_R] = B;
    // green channel
    array_[R_FACTOR_FOR_G] = R;
    array_[SCALE_FACTOR_FOR_G] = G + sat;
    array_[B_FACTOR_FOR_G] = B;
    // blue channel
    array_[R_FACTOR_FOR_B] = R;
    array_[G_FACTOR_FOR_B] = G;
    array_[SCALE_FACTOR_FOR_B] = B + sat;
    // alpha vetor scale
    array_[SCALE_FACTOR_FOR_A] = 1;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
