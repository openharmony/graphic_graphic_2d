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

#include "skia_path.h"

#include "include/core/SkMatrix.h"
#include "include/pathops/SkPathOps.h"
#include "include/utils/SkParsePath.h"
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
    isChanged_ = true;
}

void SkiaPath::LineTo(scalar x, scalar y)
{
    path_.lineTo(x, y);
    isChanged_ = true;
}

void SkiaPath::ArcTo(scalar pt1X, scalar pt1Y, scalar pt2X, scalar pt2Y, scalar startAngle, scalar sweepAngle)
{
    path_.arcTo(SkRect::MakeLTRB(pt1X, pt1Y, pt2X, pt2Y), startAngle, sweepAngle, false);
    isChanged_ = true;
}

void SkiaPath::ArcTo(scalar rx, scalar ry, scalar angle, PathDirection direction, scalar endX, scalar endY)
{
    SkPathDirection pathDir = static_cast<SkPathDirection>(direction);
    SkPath::ArcSize arcLarge = SkPath::ArcSize::kSmall_ArcSize;
    path_.arcTo(rx, ry, angle, arcLarge, pathDir, endX, endY);
    isChanged_ = true;
}

void SkiaPath::ArcTo(scalar x1, scalar y1, scalar x2, scalar y2, scalar radius)
{
    path_.arcTo(x1, y1, x2, y2, radius);
    isChanged_ = true;
}

void SkiaPath::CubicTo(scalar ctrlPt1X, scalar ctrlPt1Y, scalar ctrlPt2X, scalar ctrlPt2Y, scalar endPtX, scalar endPtY)
{
    path_.cubicTo(ctrlPt1X, ctrlPt1Y, ctrlPt2X, ctrlPt2Y, endPtX, endPtY);
    isChanged_ = true;
}

void SkiaPath::QuadTo(scalar ctrlPtX, scalar ctrlPtY, scalar endPtX, scalar endPtY)
{
    path_.quadTo(ctrlPtX, ctrlPtY, endPtX, endPtY);
    isChanged_ = true;
}

void SkiaPath::ConicTo(scalar ctrlX, scalar ctrlY, scalar endX, scalar endY, scalar weight)
{
    path_.conicTo(ctrlX, ctrlY, endX, endY, weight);
    isChanged_ = true;
}

void SkiaPath::RMoveTo(scalar dx, scalar dy)
{
    path_.rMoveTo(dx, dy);
    isChanged_ = true;
}

void SkiaPath::RLineTo(scalar dx, scalar dy)
{
    path_.rLineTo(dx, dy);
    isChanged_ = true;
}

void SkiaPath::RArcTo(scalar rx, scalar ry, scalar angle, PathDirection direction, scalar dx, scalar dy)
{
    SkPathDirection pathDir = static_cast<SkPathDirection>(direction);
    SkPath::ArcSize arcLarge = SkPath::ArcSize::kSmall_ArcSize;
    path_.arcTo(rx, ry, angle, arcLarge, pathDir, dx, dy);
    isChanged_ = true;
}

void SkiaPath::RCubicTo(scalar dx1, scalar dy1, scalar dx2, scalar dy2, scalar dx3, scalar dy3)
{
    path_.rCubicTo(dx1, dy1, dx2, dy2, dx3, dy3);
    isChanged_ = true;
}

void SkiaPath::RConicTo(scalar ctrlPtX, scalar ctrlPtY, scalar endPtX, scalar endPtY, scalar weight)
{
    path_.rConicTo(ctrlPtX, ctrlPtY, endPtX, endPtY, weight);
    isChanged_ = true;
}

void SkiaPath::RQuadTo(scalar dx1, scalar dy1, scalar dx2, scalar dy2)
{
    path_.rQuadTo(dx1, dy1, dx2, dy2);
    isChanged_ = true;
}

void SkiaPath::AddRect(scalar left, scalar top, scalar right, scalar bottom, PathDirection dir)
{
    SkPathDirection pathDir = static_cast<SkPathDirection>(dir);
    path_.addRect(SkRect::MakeLTRB(left, top, right, bottom), pathDir);
    isChanged_ = true;
}

void SkiaPath::AddRect(const Rect& rect, unsigned start, PathDirection dir)
{
    SkPathDirection pathDir = static_cast<SkPathDirection>(dir);
    path_.addRect(SkRect::MakeLTRB(rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom()), pathDir, start);
    isChanged_ = true;
}

void SkiaPath::AddOval(scalar left, scalar top, scalar right, scalar bottom, PathDirection dir)
{
    SkPathDirection pathDir = static_cast<SkPathDirection>(dir);
    path_.addOval(SkRect::MakeLTRB(left, top, right, bottom), pathDir);
    isChanged_ = true;
}

void SkiaPath::AddOval(scalar left, scalar top, scalar right, scalar bottom, unsigned start, PathDirection dir)
{
    SkPathDirection pathDir = static_cast<SkPathDirection>(dir);
    path_.addOval(SkRect::MakeLTRB(left, top, right, bottom), pathDir, start);
    isChanged_ = true;
}

void SkiaPath::AddArc(scalar left, scalar top, scalar right, scalar bottom, scalar startAngle, scalar sweepAngle)
{
    path_.addArc(SkRect::MakeLTRB(left, top, right, bottom), startAngle, sweepAngle);
    isChanged_ = true;
}

void SkiaPath::AddPoly(const std::vector<Point>& points, int count, bool close)
{
    std::vector<SkPoint> pt;
    for (auto i = 0; i < count; ++i) {
        pt.emplace_back(SkPoint::Make(points[i].GetX(), points[i].GetY()));
    }
    path_.addPoly(&pt[0], count, close);
    isChanged_ = true;
}

void SkiaPath::AddCircle(scalar x, scalar y, scalar radius, PathDirection dir)
{
    SkPathDirection pathDir = static_cast<SkPathDirection>(dir);
    path_.addCircle(x, y, radius, pathDir);
    isChanged_ = true;
}

void SkiaPath::AddRoundRect(
    scalar left, scalar top, scalar right, scalar bottom, scalar xRadius, scalar yRadius, PathDirection dir)
{
    SkPathDirection pathDir = static_cast<SkPathDirection>(dir);
    path_.addRoundRect(SkRect::MakeLTRB(left, top, right, bottom), xRadius, yRadius, pathDir);
    isChanged_ = true;
}

void SkiaPath::AddRoundRect(const RoundRect& rrect, PathDirection dir)
{
    SkPathDirection pathDir = static_cast<SkPathDirection>(dir);

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
    isChanged_ = true;
}

void SkiaPath::AddPath(const Path& src, scalar dx, scalar dy, PathAddMode mode)
{
    auto skPathImpl = src.GetImpl<SkiaPath>();
    if (skPathImpl != nullptr) {
        path_.addPath(skPathImpl->GetPath(), dx, dy, static_cast<SkPath::AddPathMode>(mode));
        isChanged_ = true;
    }
}

void SkiaPath::AddPath(const Path& src, PathAddMode mode)
{
    auto skPathImpl = src.GetImpl<SkiaPath>();
    if (skPathImpl != nullptr) {
        path_.addPath(skPathImpl->GetPath(), static_cast<SkPath::AddPathMode>(mode));
        isChanged_ = true;
    }
}

bool SkiaPath::Contains(scalar x, scalar y) const
{
    return path_.contains(x, y);
}

void SkiaPath::ReverseAddPath(const Path& src)
{
    path_.reverseAddPath(src.GetImpl<SkiaPath>()->GetPath());
    isChanged_ = true;
}

void SkiaPath::AddPath(const Path& src, const Matrix& matrix, PathAddMode mode)
{
    auto skPathImpl = src.GetImpl<SkiaPath>();
    auto skMatrixImpl = matrix.GetImpl<SkiaMatrix>();
    if (skPathImpl != nullptr && skMatrixImpl != nullptr) {
        path_.addPath(skPathImpl->GetPath(), skMatrixImpl->ExportSkiaMatrix(), static_cast<SkPath::AddPathMode>(mode));
        isChanged_ = true;
    }
}

Rect SkiaPath::GetBounds() const
{
    SkRect rect = path_.getBounds();
    return Rect(rect.left(), rect.top(), rect.right(), rect.bottom());
}

void SkiaPath::SetFillStyle(PathFillType fillstyle)
{
    SkPathFillType ft = static_cast<SkPathFillType>(fillstyle);
    path_.setFillType(ft);
    isChanged_ = true;
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
        isChanged_ = true;
    }
    return isSuccess;
}

bool SkiaPath::InitWithInterpolate(const Path& srcPath, const Path& endingPath, scalar weight)
{
    const SkPath& srcSkPath = srcPath.GetImpl<SkiaPath>()->GetPath();
    isChanged_ = true;
    return srcSkPath.interpolate(endingPath.GetImpl<SkiaPath>()->GetPath(), weight, &path_);
}

void SkiaPath::Transform(const Matrix& matrix)
{
    auto skMatrixImpl = matrix.GetImpl<SkiaMatrix>();
    if (skMatrixImpl != nullptr) {
        path_.transform(skMatrixImpl->ExportSkiaMatrix());
        isChanged_ = true;
    }
}

void SkiaPath::TransformWithPerspectiveClip(const Matrix& matrix, Path* dst, bool applyPerspectiveClip)
{
    auto skMatrixImpl = matrix.GetImpl<SkiaMatrix>();
    if (skMatrixImpl == nullptr) {
        LOGE("SkiaPath::TransformWithPerspectiveClip, skMatrixImpl is nullptr!");
        return;
    }
    if (dst == nullptr) {
        path_.transform(skMatrixImpl->ExportSkiaMatrix(), nullptr,
            static_cast<SkApplyPerspectiveClip>(applyPerspectiveClip));
        return;
    }
    auto dstPathImpl = dst->GetImpl<SkiaPath>();
    if (dstPathImpl == nullptr) {
        LOGE("SkiaPath::TransformWithPerspectiveClip, dstPathImpl is nullptr!");
        return;
    }
    path_.transform(skMatrixImpl->ExportSkiaMatrix(), &dstPathImpl->path_,
        static_cast<SkApplyPerspectiveClip>(applyPerspectiveClip));
    isChanged_ = true;
}

void SkiaPath::Offset(scalar dx, scalar dy)
{
    path_.offset(dx, dy);
    isChanged_ = true;
}

void SkiaPath::Offset(Path* dst, scalar dx, scalar dy)
{
    if (dst == nullptr) {
        path_.offset(dx, dy, nullptr);
        return;
    }
    auto dstPathImpl = dst->GetImpl<SkiaPath>();
    if (dstPathImpl == nullptr) {
        LOGE("SkiaPath::Offset, data is invalid!");
        return;
    }
    path_.offset(dx, dy, &dstPathImpl->path_);
    isChanged_ = true;
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

    isChanged_ = true;
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
    isChanged_ = true;
}

void SkiaPath::Close()
{
    path_.close();
    isChanged_ = true;
}

void SkiaPath::SetPath(const SkPath& path)
{
    path_ = path;
    isChanged_ = true;
}

const SkPath& SkiaPath::GetPath() const
{
    return path_;
}

SkPath& SkiaPath::GetMutablePath()
{
    isChanged_ = true;
    return path_;
}

void SkiaPath::PathMeasureUpdate(bool forceClosed)
{
    if (pathMeasure_ == nullptr) {
        pathMeasure_ = std::make_unique<SkPathMeasure>(path_, forceClosed);
        isChanged_ = false;
        forceClosed_ = forceClosed;
        return;
    }

    if (isChanged_ || forceClosed != forceClosed_) {
        pathMeasure_->setPath(&path_, forceClosed);
        isChanged_ = false;
        forceClosed_ = forceClosed;
    }
}

scalar SkiaPath::GetLength(bool forceClosed)
{
    PathMeasureUpdate(forceClosed);
    return pathMeasure_->getLength();
}

bool SkiaPath::GetPositionAndTangent(scalar distance, Point& position, Point& tangent, bool forceClosed)
{
    PathMeasureUpdate(forceClosed);
    bool ret = false;
    SkPoint skPosition;
    SkVector skTangent;
    ret = pathMeasure_->getPosTan(distance, &skPosition, &skTangent);
    if (ret) {
        position.SetX(skPosition.x());
        position.SetY(skPosition.y());
        tangent.SetX(skTangent.x());
        tangent.SetY(skTangent.y());
    }

    return ret;
}

bool SkiaPath::IsClosed(bool forceClosed)
{
    PathMeasureUpdate(forceClosed);
    return pathMeasure_->isClosed();
}

bool SkiaPath::GetMatrix(bool forceClosed, float distance, Matrix* matrix, PathMeasureMatrixFlags flag)
{
    if (matrix == nullptr) {
        return false;
    }
    PathMeasureUpdate(forceClosed);
    SkPathMeasure::MatrixFlags skFlag = SkPathMeasure::kGetPosAndTan_MatrixFlag;
    if (flag == PathMeasureMatrixFlags::GET_POSITION_MATRIX) {
        skFlag = SkPathMeasure::kGetPosition_MatrixFlag;
    } else if (flag == PathMeasureMatrixFlags::GET_TANGENT_MATRIX) {
        skFlag = SkPathMeasure::kGetTangent_MatrixFlag;
    }
    return pathMeasure_->getMatrix(distance,
        &matrix->GetImpl<SkiaMatrix>()->ExportMatrix(), skFlag);
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
