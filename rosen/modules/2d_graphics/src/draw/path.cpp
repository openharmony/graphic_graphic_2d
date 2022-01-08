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

#include "draw/path.h"

#include "impl_factory.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
Path::Path() noexcept : impl_(ImplFactory::CreatePathImpl()) {}

Path::~Path()
{
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

void Path::ArcTo(const Point &pt1, const Point &pt2, scalar startAngle, scalar sweepAngle)
{
    impl_->ArcTo(pt1.GetX(), pt1.GetY(), pt2.GetX(), pt2.GetY(), startAngle, sweepAngle);
}

void Path::CubicTo(scalar ctrlPt1X, scalar ctrlPt1Y, scalar ctrlPt2X, scalar ctrlPt2Y,
    scalar endPtX, scalar endPtY)
{
    impl_->CubicTo(ctrlPt1X, ctrlPt1Y, ctrlPt2X, ctrlPt2Y, endPtX, endPtY);
}

void Path::CubicTo(const Point &ctrlPt1, const Point &ctrlPt2, const Point &endPt)
{
    impl_->CubicTo(ctrlPt1.GetX(), ctrlPt1.GetY(), ctrlPt2.GetX(), ctrlPt2.GetY(), endPt.GetX(), endPt.GetY());
}

void Path::QuadTo(scalar ctrlPtX, scalar ctrlPtY, scalar endPtX, scalar endPtY)
{
    impl_->QuadTo(ctrlPtX, ctrlPtY, endPtX, endPtY);
}

void Path::QuadTo(const Point &ctrlPt, const Point endPt)
{
    impl_->QuadTo(ctrlPt.GetX(), ctrlPt.GetY(), endPt.GetX(), endPt.GetY());
}

void Path::AddRect(const Rect &rect, PathDirection dir)
{
    impl_->AddRect(rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom(), dir);
}

void Path::AddRect(scalar left, scalar top, scalar right, scalar bottom, PathDirection dir)
{
    impl_->AddRect(left, top, right, bottom, dir);
}

void Path::AddOval(const Rect &oval, PathDirection dir)
{
    impl_->AddOval(oval.GetLeft(), oval.GetTop(), oval.GetRight(), oval.GetBottom(), dir);
}

void Path::AddArc(const Rect &oval,scalar startAngle, scalar sweepAngle)
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

void Path::AddPath(const Path &src, scalar dx, scalar dy)
{
    impl_->AddPath(src, dx, dy);
}

void Path::AddPath(const Path &src)
{
    impl_->AddPath(src);
}

void Path::AddPath(const Path &src, const Matrix &matrix)
{
    impl_->AddPathWithMatrix(src, matrix);
}

Rect Path::GetBounds() const
{
    return impl_->GetBounds();
}

void Path::SetFillStyle(PathFillType fillstyle)
{
    impl_->SetFillStyle(fillstyle);
}

bool Path::Interpolate(const Path& ending, scalar weight, Path& out)
{
    return impl_->Interpolate(ending, weight, out);
}

void Path::Transform(const Matrix& matrix)
{
    impl_->Transform(matrix);
}

void Path::Offset(scalar dx, scalar dy)
{
    impl_->Offset(dx, dy);
}

bool Path::Op(const Path &path1, Path &path2, PathOp op)
{

    return impl_->OpWith(path1, path2, op);
}

void Path::Reset()
{
    impl_->Reset();
}

void Path::Close()
{
    impl_->Close();
}
}
}
}