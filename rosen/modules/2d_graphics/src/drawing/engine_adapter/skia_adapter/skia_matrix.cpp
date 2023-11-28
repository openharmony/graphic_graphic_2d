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

#include "skia_matrix.h"

#include "utils/matrix.h"
#include "skia_matrix44.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
SkiaMatrix::SkiaMatrix() : skMatrix_() {}

SkiaMatrix::SkiaMatrix(const Matrix& other) : skMatrix_(other.GetImplPtr<SkiaMatrix>()->ExportSkiaMatrix()) {}

const SkMatrix& SkiaMatrix::ExportSkiaMatrix() const
{
    return skMatrix_;
}

void SkiaMatrix::ImportMatrix(const SkMatrix& skMatrix)
{
    skMatrix_ = skMatrix;
}

SkMatrix& SkiaMatrix::ExportMatrix()
{
    return skMatrix_;
}

void SkiaMatrix::Rotate(scalar degree, scalar px, scalar py)
{
    skMatrix_.setRotate(degree, px, py);
}

void SkiaMatrix::Translate(scalar dx, scalar dy)
{
    skMatrix_.setTranslate(dx, dy);
}

void SkiaMatrix::Scale(scalar sx, scalar sy, scalar px, scalar py)
{
    skMatrix_.setScale(sx, sy, px, py);
}

void SkiaMatrix::SetScale(scalar sx, scalar sy)
{
    skMatrix_.setScale(sx, sy);
}

void SkiaMatrix::PreRotate(scalar degree)
{
    skMatrix_.preRotate(degree);
}

void SkiaMatrix::PostRotate(scalar degree)
{
    skMatrix_.postRotate(degree);
}

void SkiaMatrix::PreTranslate(scalar dx, scalar dy)
{
    skMatrix_.preTranslate(dx, dy);
}

void SkiaMatrix::PostTranslate(scalar dx, scalar dy)
{
    skMatrix_.postTranslate(dx, dy);
}

void SkiaMatrix::PreScale(scalar sx, scalar sy)
{
    skMatrix_.preScale(sx, sy);
}

void SkiaMatrix::PostScale(scalar sx, scalar sy)
{
    skMatrix_.postScale(sx, sy);
}

void SkiaMatrix::PreConcat(const Matrix& other)
{
    skMatrix_.preConcat(other.GetImplPtr<SkiaMatrix>()->ExportSkiaMatrix());
}

void SkiaMatrix::PreConcat(const Matrix44& other)
{
    skMatrix_.preConcat(other.GetImpl<SkiaMatrix44>()->GetSkMatrix44().asM33());
}

void SkiaMatrix::PostConcat(const Matrix& other)
{
    skMatrix_.postConcat(other.GetImplPtr<SkiaMatrix>()->ExportSkiaMatrix());
}

void SkiaMatrix::PostConcat(const Matrix44& other)
{
    skMatrix_.postConcat(other.GetImpl<SkiaMatrix44>()->GetSkMatrix44().asM33());
}

bool SkiaMatrix::Invert(Matrix& inverse) const
{
    SkMatrix skMatrix;
    if (skMatrix_.invert(&skMatrix)) {
        inverse.GetImpl<SkiaMatrix>()->ImportMatrix(skMatrix);
        return true;
    }
    return false;
}

void SkiaMatrix::Multiply(const Matrix& a, const Matrix& b)
{
    auto m1 = a.GetImpl<SkiaMatrix>();
    auto m2 = b.GetImpl<SkiaMatrix>();
    if (m1 != nullptr && m2 != nullptr) {
        skMatrix_.setConcat(m1->ExportSkiaMatrix(), m2->ExportSkiaMatrix());
    }
}

bool SkiaMatrix::Equals(const Matrix& a, const Matrix& b) const
{
    auto m1 = a.GetImpl<SkiaMatrix>();
    auto m2 = b.GetImpl<SkiaMatrix>();
    if (m1 != nullptr && m2 != nullptr) {
        return (m1->ExportSkiaMatrix() == m2->ExportSkiaMatrix());
    }
    return false;
}

void SkiaMatrix::SetMatrix(scalar scaleX, scalar skewX, scalar transX, scalar skewY, scalar scaleY, scalar transY,
    scalar persp0, scalar persp1, scalar persp2)
{
    skMatrix_.setAll(scaleX, skewX, transX, skewY, scaleY, transY, persp0, persp1, persp2);
}

void SkiaMatrix::MapPoints(std::vector<Point>& dst, const std::vector<Point>& src, uint32_t count) const
{
    if (count == 0) {
        return;
    }
    if (dst.size() > count) {
        for (int i = dst.size(); i > count; --i) {
            dst.pop_back();
        }
    }
    skMatrix_.mapPoints(
        reinterpret_cast<SkPoint*>(dst.data()), reinterpret_cast<const SkPoint*>(src.data()), count);
}

bool SkiaMatrix::MapRect(Rect& dst, const Rect& src) const
{
    SkRect skSrc = SkRect::MakeXYWH(src.GetLeft(), src.GetTop(), src.GetWidth(), src.GetHeight());
    SkRect skDst;
    if (skMatrix_.mapRect(&skDst, skSrc)) {
        dst = Rect(skDst.fLeft, skDst.fTop, skDst.fRight, skDst.fBottom);
        return true;
    }
    return false;
}

void SkiaMatrix::Set(int index, scalar value)
{
    skMatrix_.set(index, value);
}

scalar SkiaMatrix::Get(int index) const
{
    return skMatrix_.get(index);
}

void SkiaMatrix::GetAll(std::array<scalar, MatrixImpl::MATRIX_SIZE>& buffer) const
{
    skMatrix_.get9(buffer.data());
}

void SkiaMatrix::SetAll(std::array<scalar, MatrixImpl::MATRIX_SIZE>& buffer)
{
    skMatrix_.set9(buffer.data());
}

bool SkiaMatrix::IsIdentity() const
{
    return skMatrix_.isIdentity();
}

void SkiaMatrix::Clone(const Matrix& other)
{
    skMatrix_ = other.GetImplPtr<SkiaMatrix>()->ExportSkiaMatrix();
}

void SkiaMatrix::PreRotate(scalar degree, scalar px, scalar py)
{
    skMatrix_.preRotate(degree, px, py);
}

void SkiaMatrix::PreScale(scalar sx, scalar sy, scalar px, scalar py)
{
    skMatrix_.preScale(sx, sy, px, py);
}

void SkiaMatrix::Reset()
{
    skMatrix_.reset();
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
