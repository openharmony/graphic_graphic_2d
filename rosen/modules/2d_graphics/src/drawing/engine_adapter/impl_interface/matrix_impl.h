/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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

#ifndef MATRIX_IMPL_H
#define MATRIX_IMPL_H

#include <vector>

#include "base_impl.h"

#include "utils/point.h"
#include "utils/rect.h"
#include "utils/scalar.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
enum class ScaleToFit;
class DRAWING_API Matrix;
class DRAWING_API Matrix44;
class MatrixImpl : public BaseImpl {
public:
    // Matrix is a 3x3 float type matrix.
    static constexpr int MATRIX_SIZE = 9;

    MatrixImpl() {}
    ~MatrixImpl() override {}

    virtual void Rotate(scalar degree, scalar px, scalar py) = 0;
    virtual void Translate(scalar dx, scalar dy) = 0;
    virtual void Scale(scalar sx, scalar sy, scalar px, scalar py) = 0;
    virtual void SetScale(scalar sx, scalar sy) = 0;
    virtual void SetScaleTranslate(scalar sx, scalar sy, scalar dx, scalar dy) = 0;
    virtual void SetSkew(scalar kx, scalar ky) = 0;
    virtual void SetSkew(scalar kx, scalar ky, scalar px, scalar py) = 0;

    virtual void PreRotate(scalar degree) = 0;
    virtual void PostRotate(scalar degree) = 0;
    virtual void PostRotate(scalar degree, scalar px, scalar py) = 0;
    virtual void PreTranslate(scalar dx, scalar dy) = 0;
    virtual void PostTranslate(scalar dx, scalar dy) = 0;
    virtual void PreScale(scalar sx, scalar sy) = 0;
    virtual void PostScale(scalar sx, scalar sy) = 0;
    virtual void PostScale(scalar sx, scalar sy, scalar px, scalar py) = 0;
    virtual void PreSkew(scalar kx, scalar ky) = 0;
    virtual void PostSkew(scalar kx, scalar ky) = 0;
    virtual void PreSkew(scalar kx, scalar ky, scalar px, scalar py) = 0;
    virtual void PostSkew(scalar kx, scalar ky, scalar px, scalar py) = 0;
    virtual void PreConcat(const Matrix& other) = 0;
    virtual void PreConcat(const Matrix44& other) = 0;
    virtual void PostConcat(const Matrix& other) = 0;
    virtual void PostConcat(const Matrix44& other) = 0;

    virtual bool Invert(Matrix& inverse) const = 0;
    virtual void Multiply(const Matrix& a, const Matrix& b) = 0;
    virtual bool Equals(const Matrix& a, const Matrix& b) const = 0;
    virtual void SetMatrix(scalar scaleX, scalar skewX, scalar transX, scalar skewY, scalar scaleY, scalar transY,
        scalar persp0, scalar persp1, scalar persp2) = 0;
    virtual bool SetRectToRect(const Rect& src, const Rect& dst, ScaleToFit stf);
    virtual void MapPoints(std::vector<Point>& dst, const std::vector<Point>& src, uint32_t count) const;
    virtual bool MapRect(Rect& dst, const Rect& src) const;
    virtual bool SetPolyToPoly(const Point src[], const Point dst[], uint32_t count);
    virtual void Set(int index, scalar value);
    virtual scalar Get(int index) const = 0;
    virtual void GetAll(std::array<scalar, MATRIX_SIZE>& buffer) const = 0;
    virtual void SetAll(std::array<scalar, MATRIX_SIZE>& buffer) = 0;
    virtual bool IsIdentity() const = 0;
    virtual void Clone(const Matrix&) = 0;
    virtual void PreRotate(scalar degree, scalar px, scalar py) = 0;
    virtual void PreScale(scalar sx, scalar sy, scalar px, scalar py) = 0;
    virtual void Reset() = 0;

    virtual bool GetMinMaxScales(scalar scaleFactors[2]) = 0;
    virtual bool HasPerspective() const = 0;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
