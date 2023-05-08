/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef COLOR_MATRIX_H
#define COLOR_MATRIX_H

#include "utils/scalar.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class ColorMatrix {
public:
    // Color matrix is a 4x5 float type matrix.
    constexpr static int MATRIX_SIZE = 20;
    ColorMatrix() noexcept;
    ~ColorMatrix();

    void SetIdentity();
    void SetArray(const scalar src[MATRIX_SIZE]);
    void GetArray(scalar dst[MATRIX_SIZE]) const;
    void SetConcat(const ColorMatrix& m1, const ColorMatrix& m2);
    void PreConcat(const ColorMatrix& m);
    void PostConcat(const ColorMatrix& m);
    void SetScale(scalar sr, scalar sg, scalar sb, scalar sa);

private:
    scalar array_[MATRIX_SIZE] = { 0 };
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
