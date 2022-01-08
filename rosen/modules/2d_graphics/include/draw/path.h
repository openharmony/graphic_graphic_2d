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

#ifndef PATH_H
#define PATH_H

#include <memory>
#include <vector>

#include "engine_adapter/impl_interface/path_impl.h"
#include "utils/matrix.h"
#include "utils/point.h"
#include "utils/rect.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
enum class PathDirection {
    CW_DIRECTION,
    CCW_DIRWCTION,
};

enum class PathFillType {
    WINDING,
    EVENTODD,
    INVERSE_WINDING,
    INVERSE_EVENTODD,
};

enum class PathOp {
    DIFFERENCE,
    INTERSECT,
    UNION,
    XOR,
    REVERSE_DIFFERENCE,
};

class Path {
public:
    Path() noexcept;
    virtual ~Path();

    void MoveTo(scalar x, scalar y);
    void LineTo(scalar x, scalar y);
    void ArcTo(scalar pt1X, scalar pt1Y, scalar pt2X, scalar pt2Y, scalar startAngle, scalar sweepAngle);
    void ArcTo(const Point &pt1, const Point &pt2, scalar startAngle, scalar sweepAngle);
    void CubicTo(scalar ctrlPt1X, scalar ctrlPt1Y, scalar ctrlPt2X, scalar ctrlPt2Y,
        scalar endPtX, scalar endPtY);
    void CubicTo(const Point &ctrlPt1, const Point &ctrlPt2, const Point &endPt);
    void QuadTo(scalar ctrlPtX, scalar ctrlPtY, scalar endPtX, scalar endPtY);
    void QuadTo(const Point &ctrlPt, const Point endPt);

    void AddRect(const Rect &rect, PathDirection dir = PathDirection::CW_DIRECTION);
    void AddRect(scalar left, scalar top, scalar right, scalar bottom,
                 PathDirection dir = PathDirection::CW_DIRECTION);

    void AddOval(const Rect &oval, PathDirection dir = PathDirection::CW_DIRECTION);
    void AddArc(const Rect &oval, scalar startAngle, scalar sweepAngle);
    void AddPoly(const std::vector<Point>& points, int count, bool close);
    void AddCircle(scalar x, scalar y, scalar radius, PathDirection dir = PathDirection::CW_DIRECTION);
    void AddRoundRect(const Rect& rect, scalar xRadius, scalar yRadius,
                      PathDirection dir = PathDirection::CW_DIRECTION);

    void AddPath(const Path &src, scalar dx, scalar dy);
    void AddPath(const Path &src);
    void AddPath(const Path &src, const Matrix &matrix);

    Rect GetBounds() const;
    void SetFillStyle(PathFillType fillstyle);

    bool Interpolate(const Path& ending, scalar weight, Path& out);
    void Transform(const Matrix& matrix);
    void Offset(scalar dx, scalar dy);
    bool Op(const Path &path1, Path &path2, PathOp op);

    void Reset();

    void Close();

    template<typename T> const std::shared_ptr<T> GetImpl() const { return impl_->DowncastingTo<T>(); }
private:
    std::shared_ptr<PathImpl> impl_;
};
}
}
}
#endif
