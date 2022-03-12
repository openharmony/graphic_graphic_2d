/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

namespace OHOS {
namespace Rosen {
namespace Drawing {
SkiaMatrix::SkiaMatrix() : skMatrix_() {}

const SkMatrix& SkiaMatrix::ExportSkiaMatrix() const
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
    std::vector<SkPoint> pt1;
    std::vector<SkPoint> pt2;
    for (uint32_t i = 0; i < count; ++i) {
        pt1.emplace_back(SkPoint::Make(dst[i].GetX(), dst[i].GetY()));
        pt2.emplace_back(SkPoint::Make(src[i].GetX(), src[i].GetY()));
    }
    skMatrix_.mapPoints(&pt1[0], &pt2[0], count);
    dst.clear();
    for (auto i = 0; i < count; ++i) {
        dst.emplace_back(Point(pt1[i].fX, pt1[i].fY));
    }
}

scalar SkiaMatrix::Get(int index) const
{
    return skMatrix_.get(index);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
