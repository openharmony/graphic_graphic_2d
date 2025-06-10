/*
 * Copyright (c) 2021-2025 Huawei Device Co., Ltd.
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

#include "draw/path.h"

#include "impl_factory.h"

#include "utils/log.h"
#include "utils/utils_path.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
static constexpr int FRACTION_DIM = 3;
static constexpr int MAX_APPROXIMATE_VALUES = 1000000; // max number of values to approximate a curve
Path::Path() noexcept : impl_(ImplFactory::CreatePathImpl()) {}

Path::Path(const Path& other) noexcept
{
    impl_.reset(other.impl_->Clone());
}

Path& Path::operator=(const Path &other) noexcept
{
    impl_.reset(other.impl_->Clone());
    return *this;
}

Path::~Path() {}

bool Path::BuildFromSVGString(const std::string& str)
{
    return impl_->InitWithSVGString(str);
}

std::string Path::ConvertToSVGString() const
{
    return impl_->ConvertToSVGString();
}

void Path::MoveTo(scalar x, scalar y)
{
    impl_->MoveTo(x, y);
}

void Path::LineTo(scalar x, scalar y)
{
    impl_->LineTo(x, y);
}

void Path::ArcTo(scalar pt1X, scalar pt1Y, scalar pt2X, scalar pt2Y, scalar startAngle, scalar sweepAngle)
{
    impl_->ArcTo(pt1X, pt1Y, pt2X, pt2Y, startAngle, sweepAngle);
}

void Path::ArcTo(const Point& pt1, const Point& pt2, scalar startAngle, scalar sweepAngle)
{
    impl_->ArcTo(pt1.GetX(), pt1.GetY(), pt2.GetX(), pt2.GetY(), startAngle, sweepAngle);
}

void Path::ArcTo(scalar rx, scalar ry, scalar angle, PathDirection direction, scalar endX, scalar endY)
{
    impl_->ArcTo(rx, ry, angle, direction, endX, endY);
}

void Path::ArcTo(scalar x1, scalar y1, scalar x2, scalar y2, scalar radius)
{
    impl_->ArcTo(x1, y1, x2, y2, radius);
}

void Path::CubicTo(scalar ctrlPt1X, scalar ctrlPt1Y, scalar ctrlPt2X, scalar ctrlPt2Y, scalar endPtX, scalar endPtY)
{
    impl_->CubicTo(ctrlPt1X, ctrlPt1Y, ctrlPt2X, ctrlPt2Y, endPtX, endPtY);
}

void Path::CubicTo(const Point& ctrlPt1, const Point& ctrlPt2, const Point& endPt)
{
    impl_->CubicTo(ctrlPt1.GetX(), ctrlPt1.GetY(), ctrlPt2.GetX(), ctrlPt2.GetY(), endPt.GetX(), endPt.GetY());
}

void Path::QuadTo(scalar ctrlPtX, scalar ctrlPtY, scalar endPtX, scalar endPtY)
{
    impl_->QuadTo(ctrlPtX, ctrlPtY, endPtX, endPtY);
}

void Path::QuadTo(const Point& ctrlPt, const Point endPt)
{
    impl_->QuadTo(ctrlPt.GetX(), ctrlPt.GetY(), endPt.GetX(), endPt.GetY());
}

void Path::ConicTo(scalar ctrlX, scalar ctrlY, scalar endX, scalar endY, scalar weight)
{
    impl_->ConicTo(ctrlX, ctrlY, endX, endY, weight);
}

void Path::RMoveTo(scalar dx, scalar dy)
{
    impl_->RMoveTo(dx, dy);
}

void Path::RLineTo(scalar dx, scalar dy)
{
    impl_->RLineTo(dx, dy);
}

void Path::RArcTo(scalar rx, scalar ry, scalar angle, PathDirection direction, scalar dx, scalar dy)
{
    impl_->RArcTo(rx, ry, angle, direction, dx, dy);
}

void Path::RCubicTo(scalar dx1, scalar dy1, scalar dx2, scalar dy2, scalar dx3, scalar dy3)
{
    impl_->RCubicTo(dx1, dy1, dx2, dy2, dx3, dy3);
}

void Path::RConicTo(scalar ctrlPtX, scalar ctrlPtY, scalar endPtX, scalar endPtY, scalar weight)
{
    impl_->RConicTo(ctrlPtX, ctrlPtY, endPtX, endPtY, weight);
}

void Path::RQuadTo(scalar dx1, scalar dy1, scalar dx2, scalar dy2)
{
    impl_->RQuadTo(dx1, dy1, dx2, dy2);
}

void Path::AddRect(const Rect& rect, PathDirection dir)
{
    impl_->AddRect(rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom(), dir);
}

void Path::AddRect(const Rect& rect, unsigned start, PathDirection dir)
{
    impl_->AddRect(rect, start, dir);
}

void Path::AddRect(scalar left, scalar top, scalar right, scalar bottom, PathDirection dir)
{
    impl_->AddRect(left, top, right, bottom, dir);
}

void Path::AddOval(const Rect& oval, PathDirection dir)
{
    impl_->AddOval(oval.GetLeft(), oval.GetTop(), oval.GetRight(), oval.GetBottom(), dir);
}

void Path::AddOval(const Rect& oval, unsigned start, PathDirection dir)
{
    impl_->AddOval(oval.GetLeft(), oval.GetTop(), oval.GetRight(), oval.GetBottom(), start, dir);
}

void Path::AddArc(const Rect& oval, scalar startAngle, scalar sweepAngle)
{
    impl_->AddArc(oval.GetLeft(), oval.GetTop(), oval.GetRight(), oval.GetBottom(), startAngle, sweepAngle);
}

void Path::AddPoly(const std::vector<Point>& points, int count, bool close)
{
    impl_->AddPoly(points, count, close);
}

void Path::AddCircle(scalar x, scalar y, scalar radius, PathDirection dir)
{
    impl_->AddCircle(x, y, radius, dir);
}

void Path::AddRoundRect(const Rect& rect, scalar xRadius, scalar yRadius, PathDirection dir)
{
    impl_->AddRoundRect(rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom(), xRadius, yRadius, dir);
}

void Path::AddRoundRect(const RoundRect& rrect, PathDirection dir)
{
    impl_->AddRoundRect(rrect, dir);
}

void Path::AddPath(const Path& src, scalar dx, scalar dy, PathAddMode mode)
{
    impl_->AddPath(src, dx, dy, mode);
}

void Path::AddPath(const Path& src, PathAddMode mode)
{
    impl_->AddPath(src, mode);
}

void Path::AddPath(const Path& src, const Matrix& matrix, PathAddMode mode)
{
    impl_->AddPath(src, matrix, mode);
}

bool Path::Contains(scalar x, scalar y) const
{
    return impl_->Contains(x, y);
}

void Path::ReverseAddPath(const Path& src)
{
    impl_->ReverseAddPath(src);
}

Rect Path::GetBounds() const
{
    return impl_->GetBounds();
}

void Path::SetFillStyle(PathFillType fillstyle)
{
    impl_->SetFillStyle(fillstyle);
}

PathFillType Path::GetFillStyle() const
{
    return impl_->GetFillStyle();
}

bool Path::Interpolate(const Path& ending, scalar weight, Path& out)
{
    return impl_->Interpolate(ending, weight, out);
}

int Path::CountVerbs() const
{
    return impl_->CountVerbs();
}

Point Path::GetPoint(int index) const
{
    return impl_->GetPoint(index);
}

bool Path::IsInterpolate(const Path& other)
{
    return impl_->IsInterpolate(other);
}

static void HandlePathVerbSegments(PathVerb verb, PathIter& pathIter, const Point* points,
    std::vector<Point>& approxPoints, std::vector<float>& approxLengths, float errorSquared, float errorConic)
{
    switch (verb) {
        case PathVerb::MOVE:
            UtilsPath::AddMove(points[0], approxPoints, approxLengths);
            break;
        case PathVerb::CLOSE:
            UtilsPath::AddLine(points[0], approxPoints, approxLengths);
            break;
        case PathVerb::LINE:
            UtilsPath::AddLine(points[1], approxPoints, approxLengths);
            break;
        case PathVerb::QUAD:
            UtilsPath::AddBezier(points, UtilsPath::CalculateQuadraticBezier, approxPoints, approxLengths,
                errorSquared, false);
            break;
        case PathVerb::CUBIC:
            UtilsPath::AddBezier(points, UtilsPath::CalculateCubicBezier, approxPoints, approxLengths,
                errorSquared, true);
            break;
        case PathVerb::CONIC:
            UtilsPath::AddConic(pathIter, points, approxPoints, approxLengths, errorConic);
            break;
        default:
            break;
    }
}

void Path::Approximate(scalar acceptableError, std::vector<scalar>& outApproxVals)
{
    if (acceptableError < 0.0f) {
        LOGE("Path::Approximate acceptableError is invalid");
        return;
    }
    std::vector<Point> approxPoints;
    std::vector<float> approxLengths;
    float errorSquared = acceptableError * acceptableError;
    float errorConic = acceptableError / 2; // 2 Conic error is half of the quadratic error
    PathIter pathIter(*this, false);
    Point tpoints[4]; // 4 points for cubic and quad
    PathVerb verb;
    while ((verb = pathIter.Next(tpoints)) != PathVerb::DONE) {
        HandlePathVerbSegments(verb, pathIter, tpoints, approxPoints, approxLengths, errorSquared, errorConic);
    }
    if (approxPoints.empty()) {
        int verbCount = this->CountVerbs();
        if (verbCount == 1) {
            Point pt = this->GetPoint(0);
            UtilsPath::AddMove(pt, approxPoints, approxLengths);
        } else {
            UtilsPath::AddMove(Point(0, 0), approxPoints, approxLengths);
        }
    }
    float totalLength = approxLengths.empty() ? 1.0f : approxLengths.back();
    if (totalLength <= 0.0f) {
        totalLength = 1.0f;
        approxPoints.push_back(approxPoints.empty() ? Point(0, 0) : approxPoints.back());
        approxLengths.push_back(totalLength);
    }
    size_t numPoints = approxPoints.size();
    size_t numValues = numPoints * FRACTION_DIM;
    if (numValues > MAX_APPROXIMATE_VALUES) {
        LOGW("Path::Approximate numValues is too large");
        return;
    }
    outApproxVals.resize(numValues);
    for (size_t i = 0; i < numPoints; i++) {
        outApproxVals[i * FRACTION_DIM] = approxLengths[i] / totalLength;
        outApproxVals[i * FRACTION_DIM + 1] = approxPoints[i].GetX();
        outApproxVals[i * FRACTION_DIM + 2] = approxPoints[i].GetY(); // 2 is y coordinate
    }
}

bool Path::BuildFromInterpolate(const Path& src, const Path& ending, scalar weight)
{
    return impl_->InitWithInterpolate(src, ending, weight);
}

void Path::TransformWithPerspectiveClip(const Matrix& matrix, Path* dst, bool applyPerspectiveClip)
{
    impl_->TransformWithPerspectiveClip(matrix, dst, applyPerspectiveClip);
}

void Path::Transform(const Matrix& matrix)
{
    impl_->Transform(matrix);
}

void Path::Offset(scalar dx, scalar dy)
{
    impl_->Offset(dx, dy);
}

void Path::Offset(Path* dst, scalar dx, scalar dy)
{
    impl_->Offset(dst, dx, dy);
}

bool Path::Op(const Path& path1, Path& path2, PathOp op)
{
    return impl_->OpWith(path1, path2, op);
}

bool Path::IsValid() const
{
    return impl_->IsValid();
}

void Path::Reset()
{
    impl_->Reset();
}

void Path::SetLastPoint(scalar x, scalar y)
{
    impl_->SetLastPoint(x, y);
}

void Path::ReWind()
{
    impl_->ReWind();
}

void Path::Close()
{
    impl_->Close();
}

scalar Path::GetLength(bool forceClosed) const
{
    return impl_->GetLength(forceClosed);
}

bool Path::GetPositionAndTangent(scalar distance, Point& position, Point& tangent, bool forceClosed) const
{
    return impl_->GetPositionAndTangent(distance, position, tangent, forceClosed);
}

bool Path::GetSegment(scalar start, scalar stop, Path* dst, bool startWithMoveTo, bool forceClosed) const
{
    return impl_->GetSegment(start, stop, dst, startWithMoveTo, forceClosed);
}

bool Path::IsClosed(bool forceClosed) const
{
    return impl_->IsClosed(forceClosed);
}

bool Path::IsEmpty() const
{
    return impl_->IsEmpty();
}

bool Path::IsRect(Rect* rect, bool* isClosed, PathDirection* direction) const
{
    return impl_->IsRect(rect, isClosed, direction);
}

void Path::SetPath(const Path& path)
{
    return impl_->SetPath(path);
}

bool Path::GetMatrix(bool forceClosed, float distance, Matrix* matrix, PathMeasureMatrixFlags flag)
{
    return impl_->GetMatrix(forceClosed, distance, matrix, flag);
}

std::shared_ptr<Data> Path::Serialize() const
{
    return impl_->Serialize();
}

bool Path::Deserialize(std::shared_ptr<Data> data)
{
    return impl_->Deserialize(data);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
