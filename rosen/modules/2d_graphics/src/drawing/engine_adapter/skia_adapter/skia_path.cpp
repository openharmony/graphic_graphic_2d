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

#include "skia_path.h"

#include "include/core/SkMatrix.h"
#include "include/pathops/SkPathOps.h"
#include "skia_matrix.h"

#include "draw/path.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
SkiaPath::SkiaPath() noexcept : path_() {}

void SkiaPath::MoveTo(scalar x, scalar y)
{
    path_.moveTo(x, y);
}

void SkiaPath::LineTo(scalar x, scalar y)
{
    path_.lineTo(x, y);
}

void SkiaPath::ArcTo(scalar pt1X, scalar pt1Y, scalar pt2X, scalar pt2Y, scalar startAngle, scalar sweepAngle)
{
    path_.arcTo(SkRect::MakeLTRB(pt1X, pt1Y, pt2X, pt2Y), startAngle, sweepAngle, false);
}

void SkiaPath::CubicTo(scalar ctrlPt1X, scalar ctrlPt1Y, scalar ctrlPt2X, scalar ctrlPt2Y, scalar endPtX, scalar endPtY)
{
    path_.cubicTo(ctrlPt1X, ctrlPt1Y, ctrlPt2X, ctrlPt2Y, endPtX, endPtY);
}

void SkiaPath::QuadTo(scalar ctrlPtX, scalar ctrlPtY, scalar endPtX, scalar endPtY)
{
    path_.quadTo(ctrlPtX, ctrlPtY, endPtX, endPtY);
}

void SkiaPath::AddRect(scalar left, scalar top, scalar right, scalar bottom, PathDirection dir)
{
#if defined(USE_CANVASKIT0310_SKIA)
    SkPathDirection pathDir = static_cast<SkPathDirection>(dir);
#else
    SkPath::Direction pathDir = static_cast<SkPath::Direction>(dir);
#endif
    path_.addRect(SkRect::MakeLTRB(left, top, right, bottom), pathDir);
}

void SkiaPath::AddOval(scalar left, scalar top, scalar right, scalar bottom, PathDirection dir)
{
#if defined(USE_CANVASKIT0310_SKIA)
    SkPathDirection pathDir = static_cast<SkPathDirection>(dir);
#else
    SkPath::Direction pathDir = static_cast<SkPath::Direction>(dir);
#endif
    path_.addOval(SkRect::MakeLTRB(left, top, right, bottom), pathDir);
}

void SkiaPath::AddArc(scalar left, scalar top, scalar right, scalar bottom, scalar startAngle, scalar sweepAngle)
{
    path_.addArc(SkRect::MakeLTRB(left, top, right, bottom), startAngle, sweepAngle);
}

void SkiaPath::AddPoly(const std::vector<Point>& points, int count, bool close)
{
    std::vector<SkPoint> pt;
    for (auto i = 0; i < count; ++i) {
        pt.emplace_back(SkPoint::Make(points[i].GetX(), points[i].GetY()));
    }
    path_.addPoly(&pt[0], count, close);
}

void SkiaPath::AddCircle(scalar x, scalar y, scalar radius, PathDirection dir)
{
#if defined(USE_CANVASKIT0310_SKIA)
    SkPathDirection pathDir = static_cast<SkPathDirection>(dir);
#else
    SkPath::Direction pathDir = static_cast<SkPath::Direction>(dir);
#endif
    path_.addCircle(x, y, radius, pathDir);
}

void SkiaPath::AddRoundRect(
    scalar left, scalar top, scalar right, scalar bottom, scalar xRadius, scalar yRadius, PathDirection dir)
{
#if defined(USE_CANVASKIT0310_SKIA)
    SkPathDirection pathDir = static_cast<SkPathDirection>(dir);
#else
    SkPath::Direction pathDir = static_cast<SkPath::Direction>(dir);
#endif
    path_.addRoundRect(SkRect::MakeLTRB(left, top, right, bottom), xRadius, yRadius, pathDir);
}

void SkiaPath::AddPath(const Path& src, scalar dx, scalar dy)
{
    auto skPathImpl = src.GetImpl<SkiaPath>();
    if (skPathImpl != nullptr) {
        path_.addPath(skPathImpl->GetPath(), dx, dy);
    }
}

void SkiaPath::AddPath(const Path& src)
{
    auto skPathImpl = src.GetImpl<SkiaPath>();
    if (skPathImpl != nullptr) {
        path_.addPath(skPathImpl->GetPath());
    }
}

void SkiaPath::AddPathWithMatrix(const Path& src, const Matrix& matrix)
{
    auto skPathImpl = src.GetImpl<SkiaPath>();
    auto skMatrixImpl = matrix.GetImpl<SkiaMatrix>();
    if (skPathImpl != nullptr && skMatrixImpl != nullptr) {
        path_.addPath(skPathImpl->GetPath(), skMatrixImpl->ExportSkiaMatrix());
    }
}

Rect SkiaPath::GetBounds() const
{
    SkRect rect = path_.getBounds();
    return Rect(rect.left(), rect.top(), rect.width(), rect.height());
}

void SkiaPath::SetFillStyle(PathFillType fillstyle)
{
#if defined(USE_CANVASKIT0310_SKIA)
    SkPathFillType ft = static_cast<SkPathFillType>(fillstyle);
#else
    SkPath::FillType ft = static_cast<SkPath::FillType>(fillstyle);
#endif
    path_.setFillType(ft);
}

bool SkiaPath::Interpolate(const Path& ending, scalar weight, Path& out)
{
    bool isSuccess = false;
    auto skPathImpl1 = ending.GetImpl<SkiaPath>();
    auto skPathImpl2 = out.GetImpl<SkiaPath>();
    if (skPathImpl1 != nullptr && skPathImpl2 != nullptr) {
        SkPath interp;
        isSuccess = path_.interpolate(skPathImpl1->GetPath(), weight, &interp);
        skPathImpl2->SetPath(interp);
    }
    return isSuccess;
}

void SkiaPath::Transform(const Matrix& matrix)
{
    auto skMatrixImpl = matrix.GetImpl<SkiaMatrix>();
    if (skMatrixImpl != nullptr) {
        path_.transform(skMatrixImpl->ExportSkiaMatrix());
    }
}

void SkiaPath::Offset(scalar dx, scalar dy)
{
    path_.offset(dx, dy);
}

bool SkiaPath::OpWith(const Path& path1, const Path& path2, PathOp op)
{
    SkPathOp pathOp = static_cast<SkPathOp>(op);
    bool isOpSuccess = false;

    auto skPathImpl1 = path1.GetImpl<SkiaPath>();
    auto skPathImpl2 = path2.GetImpl<SkiaPath>();
    if (skPathImpl1 != nullptr && skPathImpl2 != nullptr) {
        isOpSuccess = Op(skPathImpl1->GetPath(), skPathImpl2->GetPath(), pathOp, &path_);
    }

    if (isOpSuccess) {
        return true;
    }
    return false;
}

void SkiaPath::Reset()
{
    path_.reset();
}

void SkiaPath::Close()
{
    path_.close();
}

void SkiaPath::SetPath(const SkPath& path)
{
    path_ = path;
}

const SkPath& SkiaPath::GetPath() const
{
    return path_;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
