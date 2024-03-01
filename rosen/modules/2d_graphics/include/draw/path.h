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

#ifndef PATH_H
#define PATH_H

#include <memory>
#include <vector>

#include "common/rs_macros.h"
#include "drawing/engine_adapter/impl_interface/path_impl.h"
#include "utils/drawing_macros.h"
#include "utils/matrix.h"
#include "utils/point.h"
#include "utils/rect.h"

#ifdef WINDOWS_PLATFORM
#ifdef DIFFERENCE
#undef DIFFERENCE
#endif
#ifdef WINDING
#undef WINDING
#endif
#endif

namespace OHOS {
namespace Rosen {
namespace Drawing {
enum class PathDirection {
    CW_DIRECTION,
    CCW_DIRECTION,
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

class DRAWING_API Path {
public:
    Path() noexcept;
    Path(const Path& p) noexcept;
    Path &operator=(const Path& p) noexcept;
    virtual ~Path();

    virtual DrawingType GetDrawingType() const
    {
        return DrawingType::COMMON;
    }

    /*
     * @brief       Parses the SVG format string that describes the drawing path, and sets the Path.
     * @param str   A string in SVG format that describes the drawing path.
     */
    virtual bool BuildFromSVGString(const std::string& str);

    /*
     * @brief   Parses into a string in SVG format that describes the Path.
     */
    std::string ConvertToSVGString() const;
    virtual void MoveTo(scalar x, scalar y);
    virtual void LineTo(scalar x, scalar y);
    virtual void ArcTo(scalar pt1X, scalar pt1Y, scalar pt2X, scalar pt2Y, scalar startAngle, scalar sweepAngle);
    virtual void ArcTo(const Point& pt1, const Point& pt2, scalar startAngle, scalar sweepAngle);
    virtual void ArcTo(scalar rx, scalar ry, scalar angle, PathDirection direction, scalar endX, scalar endY);
    virtual void ArcTo(scalar x1, scalar y1, scalar x2, scalar y2, scalar radius);
    virtual void CubicTo(
        scalar ctrlPt1X, scalar ctrlPt1Y, scalar ctrlPt2X, scalar ctrlPt2Y, scalar endPtX, scalar endPtY);
    virtual void CubicTo(const Point& ctrlPt1, const Point& ctrlPt2, const Point& endPt);
    virtual void QuadTo(scalar ctrlPtX, scalar ctrlPtY, scalar endPtX, scalar endPtY);
    virtual void QuadTo(const Point& ctrlPt, const Point endPt);

    virtual void RMoveTo(scalar dx, scalar dy);
    virtual void RLineTo(scalar dx, scalar dy);
    virtual void RArcTo(scalar rx, scalar ry, scalar angle, PathDirection direction, scalar dx, scalar dy);
    virtual void RCubicTo(scalar dx1, scalar dy1, scalar dx2, scalar dy2, scalar dx3, scalar dy3);
    virtual void RQuadTo(scalar dx1, scalar dy1, scalar dx2, scalar dy2);

    virtual void AddRect(const Rect& rect, PathDirection dir = PathDirection::CW_DIRECTION);
    virtual void AddRect(
        scalar left, scalar top, scalar right, scalar bottom, PathDirection dir = PathDirection::CW_DIRECTION);

    virtual void AddOval(const Rect& oval, PathDirection dir = PathDirection::CW_DIRECTION);
    virtual void AddArc(const Rect& oval, scalar startAngle, scalar sweepAngle);
    virtual void AddPoly(const std::vector<Point>& points, int count, bool close);
    virtual void AddCircle(scalar x, scalar y, scalar radius, PathDirection dir = PathDirection::CW_DIRECTION);
    virtual void AddRoundRect(
        const Rect& rect, scalar xRadius, scalar yRadius, PathDirection dir = PathDirection::CW_DIRECTION);

    /*
     * @brief             Adds the circle rectangle to the Path.
     * @param roundRect   The boundary and radius of a roundRect.
     * @param dir         Direction of rotation.
     */
    virtual void AddRoundRect(const RoundRect& roundRect, PathDirection dir = PathDirection::CW_DIRECTION);

    virtual void AddPath(const Path& src, scalar dx, scalar dy);
    virtual void AddPath(const Path& src);
    virtual void AddPath(const Path& src, const Matrix& matrix);
    virtual bool Contains(scalar x, scalar y) const;

    /*
     * @brief       Adds the src from back forward to the Path.
     * @param src   To add Path.
     */
    virtual void ReverseAddPath(const Path& src);

    Rect GetBounds() const;
    virtual void SetFillStyle(PathFillType fillstyle);

    bool Interpolate(const Path& ending, scalar weight, Path& out);

    /*
     * @brief         Two equal number of point set path objects are weighted interpolated, and the sets Path.
     * @param src     The number of point sets of the src Path.
     * @param ending  The number of point sets of the ending Path.
     * @param weight  The weight value is between 0 and 1.
     */
    virtual bool BuildFromInterpolate(const Path& src, const Path& ending, scalar weight);
    virtual void Transform(const Matrix& matrix);
    virtual void Offset(scalar dx, scalar dy);
    virtual bool Op(const Path& path1, Path& path2, PathOp op);

    /*
     * @brief   Checks whether the Path is valid.
     */
    bool IsValid() const;
    virtual void Reset();

    virtual void Close();

    /*
     * @brief               Gets the length of the current path object.
     * @param forceClosed   Whether to close the Path.
     */
    scalar GetLength(bool forceClosed) const;

    /*
     * @brief              Gets the position and tangent of the distance from the starting position of the Path.
     * @param distance     The distance from the start of the Path, should be greater than 0 and less than 'GetLength()'
     * @param position     Sets to the position of distance from the starting position of the Path.
     * @param tangent      Sets to the tangent of distance from the starting position of the Path.
     * @param forceClosed  Whether to close the Path.
     */
    bool GetPositionAndTangent(scalar distance, Point& position, Point& tangent, bool forceClosed) const;

    std::shared_ptr<Data> Serialize() const;
    bool Deserialize(std::shared_ptr<Data> data);

    template<typename T>
    T* GetImpl() const
    {
        return impl_->DowncastingTo<T>();
    }

private:
    std::shared_ptr<PathImpl> impl_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
