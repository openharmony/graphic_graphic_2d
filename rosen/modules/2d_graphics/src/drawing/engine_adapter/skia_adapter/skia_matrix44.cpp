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

#include "skia_matrix44.h"

#include "utils/matrix.h"
#include "utils/matrix44.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
SkiaMatrix44::SkiaMatrix44() : skMatrix44_() {}

#ifdef NEW_SKIA
const SkM44& SkiaMatrix44::GetSkMatrix44() const
#else
const SkMatrix44& SkiaMatrix44::GetSkMatrix44() const
#endif
{
    return skMatrix44_;
}

void SkiaMatrix44::Translate(scalar dx, scalar dy, scalar dz)
{
    skMatrix44_.setTranslate(dx, dy, dz);
}

void SkiaMatrix44::Scale(scalar sx, scalar sy, scalar sz)
{
    skMatrix44_.setScale(sx, sy, sz);
}

void SkiaMatrix44::PreTranslate(scalar dx, scalar dy, scalar dz)
{
    skMatrix44_.preTranslate(dx, dy, dz);
}

void SkiaMatrix44::PostTranslate(scalar dx, scalar dy, scalar dz)
{
    skMatrix44_.postTranslate(dx, dy, dz);
}

void SkiaMatrix44::PreScale(scalar sx, scalar sy, scalar sz)
{
    skMatrix44_.preScale(sx, sy, sz);
}

void SkiaMatrix44::Multiply(const Matrix44& a, const Matrix44& b)
{
    auto m1 = a.GetImpl<SkiaMatrix44>();
    auto m2 = b.GetImpl<SkiaMatrix44>();
    skMatrix44_.setConcat(m1->GetSkMatrix44(), m2->GetSkMatrix44());
}

void SkiaMatrix44::SetCol(int column, scalar x, scalar y, scalar z, scalar w)
{
    auto skv4 = SkV4 { x, y, z, w };
    skMatrix44_.setCol(column, skv4);
}

void SkiaMatrix44::SetMatrix44ColMajor(const std::array<scalar, Matrix44Impl::MATRIX44_SIZE>& buffer)
{
    SkScalar r[16] = {
        buffer[0], buffer[1], buffer[2], buffer[3],
        buffer[4], buffer[5], buffer[6], buffer[7],
        buffer[8], buffer[9], buffer[10], buffer[11],
        buffer[12], buffer[13], buffer[14], buffer[15]
    };
    skMatrix44_ = SkM44::ColMajor(r);
}

void SkiaMatrix44::SetMatrix44RowMajor(const std::array<scalar, Matrix44Impl::MATRIX44_SIZE>& buffer)
{
    SkScalar r[16] = {
        buffer[0], buffer[1], buffer[2], buffer[3],
        buffer[4], buffer[5], buffer[6], buffer[7],
        buffer[8], buffer[9], buffer[10], buffer[11],
        buffer[12], buffer[13], buffer[14], buffer[15]
    };
    skMatrix44_ = SkM44::RowMajor(r);
}

Matrix SkiaMatrix44::ConvertToMatrix()
{
    Matrix matrix;
#ifdef NEW_SKIA
    SkMatrix skMatrix = skMatrix44_.asM33();
#else
    SkMatrix skMatrix = SkMatrix(skMatrix44_);
#endif
    matrix.SetMatrix(skMatrix[0], skMatrix[1], skMatrix[2],
                     skMatrix[3], skMatrix[4], skMatrix[5],
                     skMatrix[6], skMatrix[7], skMatrix[8]);
    return matrix;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
