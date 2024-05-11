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

#ifndef PATH_IMPL_H
#define PATH_IMPL_H

#include <memory>

#include "base_impl.h"

#include "utils/matrix.h"
#include "utils/point.h"
#include "utils/rect.h"
#include "utils/round_rect.h"
#include "utils/scalar.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class Data;
class Path;
enum class PathDirection;
enum class PathFillType;
enum class PathOp;
enum class ArcSize;
enum class PathAddMode;
enum class PathMeasureMatrixFlags;
class PathImpl : public BaseImpl {
public:
    PathImpl() noexcept {}
    ~PathImpl() override {}
    PathImpl(const PathImpl& p) = delete;
    PathImpl &operator=(const PathImpl& p) = delete;
    virtual PathImpl* Clone() = 0;

    virtual bool InitWithSVGString(const std::string& str) = 0;
    virtual std::string ConvertToSVGString() const = 0;
    virtual bool InitWithInterpolate(const Path& srcPath, const Path& endingPath, scalar weight) = 0;
    virtual void MoveTo(scalar x, scalar y) = 0;
    virtual void LineTo(scalar x, scalar y) = 0;
    virtual void ArcTo(scalar pt1X, scalar pt1Y, scalar pt2X, scalar pt2Y, scalar startAngle, scalar sweepAngle) = 0;
    virtual void ArcTo(scalar rx, scalar ry, scalar angle, PathDirection direction, scalar endX, scalar endY) = 0;
    virtual void ArcTo(scalar x1, scalar y1, scalar x2, scalar y2, scalar radius) = 0;
    virtual void CubicTo(
        scalar ctrlPt1X, scalar ctrlPt1Y, scalar ctrlPt2X, scalar ctrlPt2Y, scalar endPtX, scalar endPtY) = 0;
    virtual void QuadTo(scalar ctrlPtX, scalar ctrlPtY, scalar endPtX, scalar endPtY) = 0;
    virtual void ConicTo(scalar ctrlX, scalar ctrlY, scalar endX, scalar endY, scalar weight) = 0;

    virtual void RMoveTo(scalar dx, scalar dy) = 0;
    virtual void RLineTo(scalar dx, scalar dy) = 0;
    virtual void RArcTo(scalar rx, scalar ry, scalar angle, PathDirection direction, scalar dx, scalar dy) = 0;
    virtual void RCubicTo(scalar dx1, scalar dy1, scalar dx2, scalar dy2, scalar dx3, scalar dy3) = 0;
    virtual void RConicTo(scalar ctrlPtX, scalar ctrlPtY, scalar endPtX, scalar endPtY, scalar weight) = 0;
    virtual void RQuadTo(scalar dx1, scalar dy1, scalar dx2, scalar dy2) = 0;

    virtual void AddRect(scalar left, scalar top, scalar right, scalar bottom, PathDirection dir) = 0;
    virtual void AddRect(const Rect& rect, unsigned start, PathDirection dir) = 0;
    virtual void AddOval(scalar left, scalar top, scalar right, scalar bottom, PathDirection dir) = 0;
    virtual void AddOval(scalar left, scalar top, scalar right, scalar bottom, unsigned start, PathDirection dir) = 0;
    virtual void AddArc(scalar left, scalar top, scalar right, scalar bottom, scalar startAngle, scalar sweepAngle) = 0;
    virtual void AddPoly(const std::vector<Point>& point, int count, bool close) = 0;
    virtual void AddCircle(scalar x, scalar y, scalar radius, PathDirection dir) = 0;
    virtual void AddRoundRect(
        scalar left, scalar top, scalar right, scalar bottom, scalar xRadius, scalar yRadius, PathDirection dir) = 0;
    virtual void AddRoundRect(const RoundRect& rrect, PathDirection dir) = 0;

    virtual void AddPath(const Path& src, scalar dx, scalar dy, PathAddMode mode) = 0;
    virtual void AddPath(const Path& src, PathAddMode mode) = 0;
    virtual bool Contains(scalar x, scalar y) const = 0;
    virtual void AddPath(const Path& src, const Matrix& matrix, PathAddMode mode) = 0;
    virtual void ReverseAddPath(const Path& src) = 0;

    virtual Rect GetBounds() const = 0;
    virtual void SetFillStyle(PathFillType fillstyle) = 0;

    virtual bool Interpolate(const Path& ending, scalar weight, Path& out) = 0;
    virtual void Transform(const Matrix& matrix) = 0;
    virtual void TransformWithPerspectiveClip(const Matrix& matrix, Path* dst, bool applyPerspectiveClip) = 0;
    virtual void Offset(scalar dx, scalar dy) = 0;
    virtual void Offset(Path* dst, scalar dx, scalar dy) = 0;
    virtual bool OpWith(const Path& path1, const Path& path2, PathOp op) = 0;

    virtual bool IsValid() const = 0;
    virtual void Reset() = 0;

    virtual void Close() = 0;

    virtual scalar GetLength(bool forceClosed) = 0;
    virtual bool GetPositionAndTangent(scalar distance, Point& position, Point& tangent, bool forceClosed) = 0;
    virtual bool IsClosed(bool forceClosed) = 0;
    virtual bool GetMatrix(bool forceClosed, float distance, Matrix* matrix, PathMeasureMatrixFlags flag) = 0;
    virtual std::shared_ptr<Data> Serialize() const = 0;
    virtual bool Deserialize(std::shared_ptr<Data> data) = 0;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
