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

#ifndef MATRIX44_IMPL_H
#define MATRIX44_IMPL_H

#include <array>

#include "base_impl.h"
#include "utils/scalar.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class Matrix;
class Matrix44;
class Matrix44Impl : public BaseImpl {
public:
    // Matrix44 is a 4x4 float type matrix.
    constexpr static int MATRIX44_SIZE = 16;

    Matrix44Impl() {}
    ~Matrix44Impl() override {}

    virtual void Translate(scalar dx, scalar dy, scalar dz) = 0;
    virtual void Scale(scalar sx, scalar sy, scalar sz) = 0;
    virtual void PreTranslate(scalar dx, scalar dy, scalar dz) = 0;
    virtual void PostTranslate(scalar dx, scalar dy, scalar dz) = 0;
    virtual void PreScale(scalar sx, scalar sy, scalar sz) = 0;
    virtual void Multiply(const Matrix44& a, const Matrix44& b) = 0;
    virtual void SetMatrix44ColMajor(const std::array<scalar, MATRIX44_SIZE>& buffer) = 0;
    virtual void SetMatrix44RowMajor(const std::array<scalar, MATRIX44_SIZE>& buffer) = 0;
    virtual Matrix ConvertToMatrix() = 0;
    virtual void SetCol(int column, scalar x, scalar y, scalar z, scalar w) = 0;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
