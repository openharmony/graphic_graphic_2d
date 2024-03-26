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

#include "skia_path.h"

#include "include/core/SkMatrix.h"
#include "include/pathops/SkPathOps.h"
#include "include/utils/SkParsePath.h"
#include "include/core/SkPathMeasure.h"
#include "include/core/SkString.h"
#include "skia_matrix.h"

#include "src/core/SkReadBuffer.h"
#include "src/core/SkWriteBuffer.h"

#include "draw/path.h"
#include "utils/data.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

SkiaPath::SkiaPath(const SkiaPath& other) noexcept : path_(other.path_) {}

SkiaPath& SkiaPath::operator=(const SkiaPath& other) noexcept
{
    path_ = other.path_;
    return *this;
}

PathImpl* SkiaPath::Clone()
{
    return new SkiaPath(*this);
}

bool SkiaPath::InitWithSVGString(const std::string& str)
{
    return SkParsePath::FromSVGString(str.c_str(), &path_);
}

std::string SkiaPath::ConvertToSVGString() const
{
    SkString skString;
    SkParsePath::ToSVGString(path_, &skString);

    return skString.c_str();
}

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

void SkiaPath::ArcTo(scalar rx, scalar ry, scalar angle, PathDirection direction, scalar endX, scalar endY)
{
#if defined(USE_CANVASKIT0310_SKIA) || defined(NEW_SKIA)
    SkPathDirection pathDir = static_cast<SkPathDirection>(direction);
#else
    SkPath::Direction pathDir = static_cast<SkPath::Direction>(direction);
#endif
    SkPath::ArcSize arcLarge = SkPath::ArcSize::kSmall_ArcSize;
    path_.arcTo(rx, ry, angle, arcLarge, pathDir, endX, endY);
}

void SkiaPath::ArcTo(scalar x1, scalar y1, scalar x2, scalar y2, scalar radius)
{
    path_.arcTo(x1, y1, x2, y2, radius);
}

void SkiaPath::CubicTo(scalar ctrlPt1X, scalar ctrlPt1Y, scalar ctrlPt2X, scalar ctrlPt2Y, scalar endPtX, scalar endPtY)
{
    path_.cubicTo(ctrlPt1X, ctrlPt1Y, ctrlPt2X, ctrlPt2Y, endPtX, endPtY);
}

void SkiaPath::QuadTo(scalar ctrlPtX, scalar ctrlPtY, scalar endPtX, scalar endPtY)
{
    path_.quadTo(ctrlPtX, ctrlPtY, endPtX, endPtY);
}

void SkiaPath::RMoveTo(scalar dx, scalar dy)
{
    path_.rMoveTo(dx, dy);
}

void SkiaPath::RLineTo(scalar dx, scalar dy)
{
    path_.rLineTo(dx, dy);
}

void SkiaPath::RArcTo(scalar rx, scalar ry, scalar angle, PathDirection direction, scalar dx, scalar dy)
{
#if defined(USE_CANVASKIT0310_SKIA) || defined(NEW_SKIA)
    SkPathDirection pathDir = static_cast<SkPathDirection>(direction);
#else
    SkPath::Direction pathDir = static_cast<SkPath::Direction>(direction);
#endif
    SkPath::ArcSize arcLarge = SkPath::ArcSize::kSmall_ArcSize;
    path_.arcTo(rx, ry, angle, arcLarge, pathDir, dx, dy);
}

void SkiaPath::RCubicTo(scalar dx1, scalar dy1, scalar dx2, scalar dy2, scalar dx3, scalar dy3)
{
    path_.rCubicTo(dx1, dy1, dx2, dy2, dx3, dy3);
}

void SkiaPath::RQuadTo(scalar dx1, scalar dy1, scalar dx2, scalar dy2)
{
    path_.rQuadTo(dx1, dy1, dx2, dy2);
}

void SkiaPath::AddRect(scalar left, scalar top, scalar right, scalar bottom, PathDirection dir)
{
#if defined(USE_CANVASKIT0310_SKIA) || defined(NEW_SKIA)
    SkPathDirection pathDir = static_cast<SkPathDirection>(dir);
#else
    SkPath::Direction pathDir = static_cast<SkPath::Direction>(dir);
#endif
    path_.addRect(SkRect::MakeLTRB(left, top, right, bottom), pathDir);
}

void SkiaPath::AddOval(scalar left, scalar top, scalar right, scalar bottom, PathDirection dir)
{
#if defined(USE_CANVASKIT0310_SKIA) || defined(NEW_SKIA)
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
#if defined(USE_CANVASKIT0310_SKIA) || defined(NEW_SKIA)
    SkPathDirection pathDir = static_cast<SkPathDirection>(dir);
#else
    SkPath::Direction pathDir = static_cast<SkPath::Direction>(dir);
#endif
    path_.addCircle(x, y, radius, pathDir);
}

void SkiaPath::AddRoundRect(
    scalar left, scalar top, scalar right, scalar bottom, scalar xRadius, scalar yRadius, PathDirection dir)
{
#if defined(USE_CANVASKIT0310_SKIA) || defined(NEW_SKIA)
    SkPathDirection pathDir = static_cast<SkPathDirection>(dir);
#else
    SkPath::Direction pathDir = static_cast<SkPath::Direction>(dir);
#endif
    path_.addRoundRect(SkRect::MakeLTRB(left, top, right, bottom), xRadius, yRadius, pathDir);
}

void SkiaPath::AddRoundRect(const RoundRect& rrect, PathDirection dir)
{
#if defined(USE_CANVASKIT0310_SKIA) || defined(NEW_SKIA)
    SkPathDirection pathDir = static_cast<SkPathDirection>(dir);
#else
    SkPath::Direction pathDir = static_cast<SkPath::Direction>(dir);
#endif

    Rect rect = rrect.GetRect();
    SkRect outer = SkRect::MakeLTRB(rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom());

    SkVector radii[4];
    Point p;
    p = rrect.GetCornerRadius(RoundRect::TOP_LEFT_POS);
    radii[SkRRect::kUpperLeft_Corner] = { p.GetX(), p.GetY() };
    p = rrect.GetCornerRadius(RoundRect::TOP_RIGHT_POS);
    radii[SkRRect::kUpperRight_Corner] = { p.GetX(), p.GetY() };
    p = rrect.GetCornerRadius(RoundRect::BOTTOM_RIGHT_POS);
    radii[SkRRect::kLowerRight_Corner] = { p.GetX(), p.GetY() };
    p = rrect.GetCornerRadius(RoundRect::BOTTOM_LEFT_POS);
    radii[SkRRect::kLowerLeft_Corner] = { p.GetX(), p.GetY() };

    SkRRect skRRect;
    skRRect.setRectRadii(outer, radii);
    path_.addRRect(skRRect, pathDir);
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

bool SkiaPath::Contains(scalar x, scalar y) const
{
    return path_.contains(x, y);
}

void SkiaPath::ReverseAddPath(const Path& src)
{
    path_.reverseAddPath(src.GetImpl<SkiaPath>()->GetPath());
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
    return Rect(rect.left(), rect.top(), rect.right(), rect.bottom());
}

void SkiaPath::SetFillStyle(PathFillType fillstyle)
{
#if defined(USE_CANVASKIT0310_SKIA) || defined(NEW_SKIA)
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

bool SkiaPath::InitWithInterpolate(const Path& srcPath, const Path& endingPath, scalar weight)
{
    const SkPath& srcSkPath = srcPath.GetImpl<SkiaPath>()->GetPath();
    return srcSkPath.interpolate(endingPath.GetImpl<SkiaPath>()->GetPath(), weight, &path_);
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

bool SkiaPath::IsValid() const
{
    return !path_.isEmpty();
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

scalar SkiaPath::GetLength(bool forceClosed) const
{
    SkPathMeasure pathMeasure(path_, forceClosed);
    return pathMeasure.getLength();
}

bool SkiaPath::GetPositionAndTangent(scalar distance, Point& position, Point& tangent, bool forceClosed) const
{
    bool ret = false;
    SkPoint skPosition;
    SkVector skTangent;
    SkPathMeasure pathMeasure(path_, forceClosed);
    ret = pathMeasure.getPosTan(distance, &skPosition, &skTangent);
    if (ret) {
        position.SetX(skPosition.x());
        position.SetY(skPosition.y());
        tangent.SetX(skTangent.x());
        tangent.SetY(skTangent.y());
    }

    return ret;
}

std::shared_ptr<Data> SkiaPath::Serialize() const
{
    SkBinaryWriteBuffer writer;
    writer.writePath(path_);
    size_t length = writer.bytesWritten();
    std::shared_ptr<Data> data = std::make_shared<Data>();
    data->BuildUninitialized(length);
    writer.writeToMemory(data->WritableData());
    return data;
}

bool SkiaPath::Deserialize(std::shared_ptr<Data> data)
{
    if (data == nullptr) {
        LOGD("SkiaPath::Deserialize, data is invalid!");
        return false;
    }

    SkReadBuffer reader(data->GetData(), data->GetSize());
    reader.readPath(&path_);
    return true;
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
