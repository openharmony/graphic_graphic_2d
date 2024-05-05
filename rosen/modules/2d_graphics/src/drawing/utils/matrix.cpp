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

#include "utils/matrix.h"

#include "impl_factory.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
Matrix::Matrix() : matrixImplPtr(ImplFactory::CreateMatrixImpl()) {}

Matrix::Matrix(const Matrix& other) : matrixImplPtr(ImplFactory::CreateMatrixImpl(other)) {}

Matrix& Matrix::operator=(const Matrix& matrix)
{
    matrixImplPtr->Clone(matrix);
    return *this;
}

void Matrix::Rotate(scalar degree, scalar px, scalar py)
{
    matrixImplPtr->Rotate(degree, px, py);
}

void Matrix::Translate(scalar dx, scalar dy)
{
    matrixImplPtr->Translate(dx, dy);
}

void Matrix::Scale(scalar sx, scalar sy, scalar px, scalar py)
{
    matrixImplPtr->Scale(sx, sy, px, py);
}

void Matrix::SetSkew(scalar kx, scalar ky)
{
    matrixImplPtr->SetSkew(kx, ky);
}

void Matrix::SetSkew(scalar kx, scalar ky, scalar px, scalar py)
{
    matrixImplPtr->SetSkew(kx, ky, px, py);
}

void Matrix::SetScale(scalar sx, scalar sy)
{
    matrixImplPtr->SetScale(sx, sy);
}

void Matrix::SetScaleTranslate(scalar sx, scalar sy, scalar dx, scalar dy)
{
    matrixImplPtr->SetScaleTranslate(sx, sy, dx, dy);
}

void Matrix::PreRotate(scalar degree)
{
    matrixImplPtr->PreRotate(degree);
}

void Matrix::PostRotate(scalar degree)
{
    matrixImplPtr->PostRotate(degree);
}

void Matrix::PostRotate(scalar degree, scalar px, scalar py)
{
    matrixImplPtr->PostRotate(degree, px, py);
}

void Matrix::PreTranslate(scalar dx, scalar dy)
{
    matrixImplPtr->PreTranslate(dx, dy);
}

void Matrix::PostTranslate(scalar dx, scalar dy)
{
    matrixImplPtr->PostTranslate(dx, dy);
}

void Matrix::PreScale(scalar sx, scalar sy)
{
    matrixImplPtr->PreScale(sx, sy);
}

void Matrix::PostScale(scalar sx, scalar sy)
{
    matrixImplPtr->PostScale(sx, sy);
}

void Matrix::PostScale(scalar sx, scalar sy, scalar px, scalar py)
{
    matrixImplPtr->PostScale(sx, sy, px, py);
}

void Matrix::PreSkew(scalar kx, scalar ky)
{
    matrixImplPtr->PreSkew(kx, ky);
}

void Matrix::PreSkew(scalar kx, scalar ky, scalar px, scalar py)
{
    matrixImplPtr->PreSkew(kx, ky, px, py);
}

void Matrix::PostSkew(scalar kx, scalar ky)
{
    matrixImplPtr->PostSkew(kx, ky);
}

void Matrix::PostSkew(scalar kx, scalar ky, scalar px, scalar py)
{
    matrixImplPtr->PostSkew(kx, ky, px, py);
}

void Matrix::PreConcat(const Matrix& other)
{
    matrixImplPtr->PreConcat(other);
}

void Matrix::PreConcat(const Matrix44& matrix44)
{
    matrixImplPtr->PreConcat(matrix44);
}

void Matrix::PostConcat(const Matrix& other)
{
    matrixImplPtr->PostConcat(other);
}

void Matrix::PostConcat(const Matrix44& matrix44)
{
    matrixImplPtr->PostConcat(matrix44);
}

bool Matrix::Invert(Matrix& inverse) const
{
    return matrixImplPtr->Invert(inverse);
}

Matrix Matrix::operator*(const Matrix& m)
{
    matrixImplPtr->Multiply(*this, m);
    return *this;
}

bool Matrix::operator==(const Matrix& m) const
{
    return matrixImplPtr->Equals(*this, m);
}

void Matrix::SetMatrix(scalar scaleX, scalar skewX, scalar transX, scalar skewY, scalar scaleY, scalar transY,
    scalar persp0, scalar persp1, scalar persp2)
{
    matrixImplPtr->SetMatrix(scaleX, skewX, transX, skewY, scaleY, transY, persp0, persp1, persp2);
}

bool Matrix::SetRectToRect(const Rect& src, const Rect& dst, ScaleToFit stf)
{
    return matrixImplPtr->SetRectToRect(src, dst, stf);
}

void Matrix::MapPoints(std::vector<Point>& dst, const std::vector<Point>& src, uint32_t count) const
{
    matrixImplPtr->MapPoints(dst, src, count);
}

bool Matrix::MapRect(Rect& dst, const Rect& src) const
{
    return matrixImplPtr->MapRect(dst, src);
}

bool Matrix::SetPolyToPoly(const Point src[], const Point dst[], uint32_t count)
{
    return matrixImplPtr->SetPolyToPoly(src, dst, count);
}

void Matrix::Set(Index index, scalar value)
{
    matrixImplPtr->Set(index, value);
}

scalar Matrix::Get(int index) const
{
    return matrixImplPtr->Get(index);
}

void Matrix::GetAll(Buffer& buffer) const
{
    matrixImplPtr->GetAll(buffer);
}

void Matrix::SetAll(Buffer& buffer)
{
    matrixImplPtr->SetAll(buffer);
}

bool Matrix::IsIdentity() const
{
    return matrixImplPtr->IsIdentity();
}

void Matrix::PreRotate(scalar degree, scalar px, scalar py)
{
    matrixImplPtr->PreRotate(degree, px, py);
}

void Matrix::PreScale(scalar sx, scalar sy, scalar px, scalar py)
{
    matrixImplPtr->PreScale(sx, sy, px, py);
}

void Matrix::Reset()
{
    matrixImplPtr->Reset();
}

bool Matrix::GetMinMaxScales(scalar scaleFactors[2])
{
    return matrixImplPtr->GetMinMaxScales(scaleFactors);
}

bool Matrix::HasPerspective() const
{
    return matrixImplPtr->HasPerspective();
}

void Matrix::Swap(Matrix& target)
{
    std::swap(matrixImplPtr, target.matrixImplPtr);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
