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

enum class PathAddMode {
    APPEND_PATH_ADD_MODE,
    EXTEND_PATH_ADD_MODE,
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

    /**
     * @brief Parses the SVG format string that describes the drawing path, and sets the Path.
     *
     * @param str A string in SVG format that describes the drawing path.
     * @return true if build succeeded, otherwise false.
     */
    virtual bool BuildFromSVGString(const std::string& str);

    /**
     * @brief Parses into a string in SVG format that describes the Path.
     * @return Returns a string in SVG format.
     */
    std::string ConvertToSVGString() const;

    /**
     * @brief Adds beginning of contour at (x, y).
     *
     * @param x contour start x-axis
     * @param y contour start y-axis
     */
    virtual void MoveTo(scalar x, scalar y);

    /**
     * @brief Adds line from last point to Point(x, y).
     *
     * @param x x-axis of end point of added line
     * @param y y-axis of end point of added line
     */
    virtual void LineTo(scalar x, scalar y);

    /**
     * @brief Appends arc to Path. Arc added is part of ellipse bounded by oval, from startAngle through sweepAngle.
     * Both startAngle and sweepAngle are measured in degrees, where zero degrees is aligned with the positive x-axis,
     * and positive sweeps extends arc clockwise. ArcTo() adds line connecting Path last Point to initial arc Point if
     * forceMoveTo is false and Path is not empty. Otherwise, added contour begins with first point of arc.
     * Angles greater than -360 and less than 360 are treated modulo 360.
     *
     * @param pt1X left of bounds of ellipse containing arc
     * @param pt1Y top of bounds of ellipse containing arc
     * @param pt2X right of bounds of ellipse containing arc
     * @param pt2Y bottom of bounds of ellipse containing arc
     * @param startAngle starting angle of arc in degrees
     * @param sweepAngle sweep, in degrees. Positive is clockwise; treated modulo 360
     */
    virtual void ArcTo(scalar pt1X, scalar pt1Y, scalar pt2X, scalar pt2Y, scalar startAngle, scalar sweepAngle);
    virtual void ArcTo(const Point& pt1, const Point& pt2, scalar startAngle, scalar sweepAngle);
    /**
     * @brief Appends arc to Path. Arc is implemented by one or more conics weighted to describe part of oval
     * with radii (rx, ry) rotated by xAxisRotate degrees. Arc curves from last Path Point to (endX, endY), choosing
     * one of four possible routes: clockwise or counterclockwise, and smaller or larger. Arc sweep is always
     * less than 360 degrees. ArcTo() appends line to (endX, endY) if either radii are zero, or if last Path Point
     * equals (endX, endY). ArcTo() scales radii (rx, ry) to fit last Path Point and (endX, endY) if both are greater
     * than zero but too small.
     *
     * @param rx radius on x-axis before x-axis rotation
     * @param ry radius on y-axis before x-axis rotation
     * @param angle x-axis rotation in degrees; positive values are clockwise
     * @param direction chooses clockwise or counterclockwise arc
     * @param endX x-axis end of arc
     * @param endY y-axis end of arc
     */
    virtual void ArcTo(scalar rx, scalar ry, scalar angle, PathDirection direction, scalar endX, scalar endY);

    /**
     * @brief Appends arc to Path, after appending line if needed. Arc is implemented by conic weighted
     * to describe part of circle. Arc is contained by tangent from last Path point to (x1, y1), and tangent
     * from (x1, y1) to (x2, y2). Arc is part of circle sized to radius, positioned so it touches both tangent lines.
     * If last Path Point does not start Arc, ArcTo appends connecting Line to Path. The length of Vector from
     * (x1, y1) to (x2, y2) does not affect Arc. Arc sweep is always less than 180 degrees. If radius is zero,
     * or if tangents are nearly parallel, ArcTo appends Line from last Path Point to (x1, y1).
     *
     * @param x1 x-axis value common to pair of tangents
     * @param y1 y-axis value common to pair of tangents
     * @param x2 x-axis value end of second tangent
     * @param y2 y-axis value end of second tangent
     * @param radius distance from arc to circle center
     */
    virtual void ArcTo(scalar x1, scalar y1, scalar x2, scalar y2, scalar radius);

    /**
     * @brief Adds cubic from last point towards (ctrlPt1X, ctrlPt1Y), then towards (ctrlPt2X, ctrlPt2Y),
     * ending at (endPtX, endPtY).
     *
     * @param ctrlPt1X x-axis of first control Point of cubic
     * @param ctrlPt1Y y-axis of first control Point of cubic
     * @param ctrlPt2X x-axis of second control Point of cubic
     * @param ctrlPt2Y y-axis of second control Point of cubic
     * @param endPtX x-axis of end Point of cubic
     * @param endPtY y-axis of end Point of cubic
     */
    virtual void CubicTo(
        scalar ctrlPt1X, scalar ctrlPt1Y, scalar ctrlPt2X, scalar ctrlPt2Y, scalar endPtX, scalar endPtY);

    /**
     * @brief Adds cubic from last point towards Point ctrlPt1, then towards Point ctrlPt2, ending at Point endPt.
     *
     * @param ctrlPt1 first control Point of cubic
     * @param ctrlPt2 second control Point of cubic
     * @param endPt end Point of cubic
     */
    virtual void CubicTo(const Point& ctrlPt1, const Point& ctrlPt2, const Point& endPt);

    /**
     * @brief Adds quad from last point towards (ctrlPtX, ctrlPtY), to (endPtX, endPtY).
     *
     * @param ctrlPtX control Point of quad on x-axis
     * @param ctrlPtY control Point of quad on y-axis
     * @param endPtX end Point of quad on x-axis
     * @param endPtY end Point of quad on y-axis
     */
    virtual void QuadTo(scalar ctrlPtX, scalar ctrlPtY, scalar endPtX, scalar endPtY);

    /**
     * @brief Adds quad from last point towards Point ctrlPt, to Point endPt.
     *
     * @param ctrlPt control Point of added quad
     * @param endPt end Point of added quad
     */
    virtual void QuadTo(const Point& ctrlPt, const Point endPt);
    /**
     * @brief Draws a conic from the last point of a path to the target point.
     *
     * @param ctrlX Indicates the x coordinate of the control point
     * @param ctrlY Indicates the y coordinate of the control point
     * @param endX Indicates the x coordinate of the target point
     * @param endY Indicates the y coordinate of the target point
     * @param weight Indicates the weight of added conic.
     */
    virtual void ConicTo(scalar ctrlX, scalar ctrlY, scalar endX, scalar endY, scalar weight);

    /**
     * @brief Adds beginning of contour relative to last point. If Path is empty,
     * starts contour at (dx, dy). Otherwise, start contour at last point offset by (dx, dy).
     * Function name stands for "relative move to".
     *
     * @param dx offset from last point to contour start on x-axis
     * @param dy offset from last point to contour start on y-axis
     */
    virtual void RMoveTo(scalar dx, scalar dy);

    /**
     * @brief Adds line from last point to vector (dx, dy).
     *
     * @param dx offset from last point to line end on x-axis
     * @param dy offset from last point to line end on y-axis
     */
    virtual void RLineTo(scalar dx, scalar dy);

    /**
     * @brief Appends arc to Path, relative to last Path Point.
     *
     * @param rx radius before x-axis rotation
     * @param ry radius before y-axis rotation
     * @param angle x-axis rotation in degrees; positive values are clockwise
     * @param direction chooses clockwise or counterclockwise arc
     * @param dx x-axis offset end of arc from last Path Point
     * @param dy y-axis offset end of arc from last Path Point
     */
    virtual void RArcTo(scalar rx, scalar ry, scalar angle, PathDirection direction, scalar dx, scalar dy);

    /**
     * @brief Adds cubic from last point towards vector (dx1, dy1), then towards vector (dx2, dy2),
     * to vector (dx3, dy3).
     *
     * @param dx1 offset from last point to first cubic control on x-axis
     * @param dy1 offset from last point to first cubic control on y-axis
     * @param dx2 offset from last point to second cubic control on x-axis
     * @param dy2 offset from last point to second cubic control on y-axis
     * @param dx3 offset from last point to cubic end on x-axis
     * @param dy3 offset from last point to cubic end on y-axis
     */
    virtual void RCubicTo(scalar dx1, scalar dy1, scalar dx2, scalar dy2, scalar dx3, scalar dy3);

    /**
     * @brief Adds quad from last point towards vector (dx1, dy1), to vector (dx2, dy2).
     *
     * @param dx1 offset from last point to quad control on x-axis
     * @param dy1 offset from last point to quad control on y-axis
     * @param dx2 offset from last point to quad end on x-axis
     * @param dy2 offset from last point to quad end on y-axis
     */
    virtual void RQuadTo(scalar dx1, scalar dy1, scalar dx2, scalar dy2);

    /**
     * @brief Adds conic from last point towards vector (dx1, dy1), to vector (dx2, dy2),
     * weighted by w. If Path is empty, or last Path::Verb is kClose_Verb,
     * last point is set to (0, 0) before adding conic.
     *
     * @param ctrlPtX offset from last point to conic control on x-axis
     * @param ctrlPtY offset from last point to conic control on y-axis
     * @param endPtX offset from last point to conic end on x-axis
     * @param endPtY offset from last point to conic end on y-axis
     * @param weight weight of added conic
     */
    virtual void RConicTo(scalar ctrlPtX, scalar ctrlPtY, scalar endPtX, scalar endPtY, scalar weight);

    /**
     * @brief Adds a new contour to the path, defined by the rect, and wound in the specified direction.
     *
     * @param rect Rect to add as a closed contour
     * @param dir Path::PathDirection to orient the new contour
     */
    virtual void AddRect(const Rect& rect, PathDirection dir = PathDirection::CW_DIRECTION);

    /**
     * @brief Adds a new contour to the path, defined by the rect, and wound in the specified direction.
     *
     * @param rect Rect to add as a closed contour
     * @param dir Path::PathDirection to orient the new contour
     * @param start Initial corner of Rect to add
     */
    virtual void AddRect(const Rect& rect, unsigned start, PathDirection dir = PathDirection::CW_DIRECTION);
    virtual void AddRect(
        scalar left, scalar top, scalar right, scalar bottom, PathDirection dir = PathDirection::CW_DIRECTION);

    /**
     * @brief Adds oval to Path. Oval is upright ellipse bounded by Rect oval with radii equal to
     * half oval width and half oval height. Oval begins at start and continues clockwise if dir is
     * PathDirection::CW_DIRECTION, counterclockwise if dir is PathDirection::CCW_DIRECTION.
     *
     * @param oval bounds of ellipse added
     * @param dir Path::PathDirection to wind ellipse
     */
    virtual void AddOval(const Rect& oval, PathDirection dir = PathDirection::CW_DIRECTION);

    /**
     * @brief Adds oval to Path. Oval is upright ellipse bounded by Rect oval with radii equal to
     * half oval width and half oval height. Oval begins at start and continues clockwise if dir is
     * PathDirection::CW_DIRECTION, counterclockwise if dir is PathDirection::CCW_DIRECTION.
     *
     * @param oval bounds of ellipse added
     * @param start Index of initial point of ellipse
     * @param dir Path::PathDirection to wind ellipse
     */
    virtual void AddOval(const Rect& oval, unsigned start, PathDirection dir = PathDirection::CCW_DIRECTION);

    /**
     * @brief Appends arc to Path, as the start of new contour. Arc added is part of ellipse bounded by oval,
     * from startAngle through sweepAngle. Both startAngle and sweepAngle are measured in degrees, where zero
     * degrees is aligned with the positive x-axis, and positive sweeps extends arc clockwise. If sweepAngle <= -360,
     * or sweepAngle >= 360; and startAngle modulo 90 is nearly zero, append oval instead of arc. Otherwise, sweepAngle
     * values are treated modulo 360, and arc may or may not draw depending on numeric rounding.
     *
     * @param oval bounds of ellipse containing arc
     * @param startAngle starting angle of arc in degrees
     * @param sweepAngle sweep, in degrees. Positive is clockwise; treated modulo 360
     */
    virtual void AddArc(const Rect& oval, scalar startAngle, scalar sweepAngle);

    /**
     * @brief Adds contour created from line array, adding (count - 1) line segments.
     *
     * @param points array of line sharing end and start Point
     * @param count length of Point array
     * @param close true to add line connecting contour end and start
     */
    virtual void AddPoly(const std::vector<Point>& points, int count, bool close);
    virtual void AddCircle(scalar x, scalar y, scalar radius, PathDirection dir = PathDirection::CW_DIRECTION);
    virtual void AddRoundRect(
        const Rect& rect, scalar xRadius, scalar yRadius, PathDirection dir = PathDirection::CW_DIRECTION);

    /**
     * @brief Adds the circle rectangle to the Path.
     * @param roundRect The boundary and radius of a roundRect.
     * @param dir       Direction of rotation.
     */
    virtual void AddRoundRect(const RoundRect& roundRect, PathDirection dir = PathDirection::CW_DIRECTION);

    /**
     * @brief Appends src to Path, offset by (dx, dy).
     *
     * @param src Path Point, and conic weights to add
     * @param dx offset added to src Point array x-axis coordinates
     * @param dy offset added to src Point array y-axis coordinates
     * @param mode  the add path's add mode
     */
    virtual void AddPath(const Path& src, scalar dx, scalar dy, PathAddMode mode = PathAddMode::APPEND_PATH_ADD_MODE);

    /**
     * @brief Appends src to Path.
     *
     * @param src Path Point, and conic weights to add
     * @param mode  the add path's add mode
     */
    virtual void AddPath(const Path& src, PathAddMode mode = PathAddMode::APPEND_PATH_ADD_MODE);

    /**
     * @brief Appends src to Path, transformed by matrix.
     * Transformed curves may have different verbs, Point, and conic weights.
     *
     * @param src Path Point, and conic weights to add
     * @param matrix transform applied to src
     * @param mode  the add path's add mode
     */
    virtual void AddPath(const Path& src, const Matrix& matrix, PathAddMode mode = PathAddMode::APPEND_PATH_ADD_MODE);
    virtual bool Contains(scalar x, scalar y) const;

    /**
     * @brief Adds the src from back forward to the Path.
     * @param src To add Path.
     */
    virtual void ReverseAddPath(const Path& src);

    /**
     * @brief Returns minimum and maximum axes values of Point array. Returns (0, 0, 0, 0)
     * if Path contains no points. Returned bounds width and height may be larger or smaller
     * than area affected when Path is drawn.
     *
     * @return Rect bounds of all Point in Point array
     */
    Rect GetBounds() const;

    /**
     * @brief Sets PathFillType, the rule used to fill Path. While there is no check that ft is legal,
     * values outside of PathFillType are not supported.
     *
     * @param fillstyle enum PathFillType
     */
    virtual void SetFillStyle(PathFillType fillstyle);

    /**
     * @brief Interpolates between Path with Point array of equal size.
     *
     * @param ending Point array averaged with this Point array
     * @param weight contribution of this Point array, and one minus contribution of ending Point array
     * @param out Path replaced by interpolated averages
     * @return true if Path contain same number of Point
     */
    bool Interpolate(const Path& ending, scalar weight, Path& out);

    /**
     * @brief Two equal number of point set path objects are weighted interpolated, and the sets Path.
     * @param src    The number of point sets of the src Path.
     * @param ending The number of point sets of the ending Path.
     * @param weight The weight value is between 0 and 1.
     * @return true if build succeeded, otherwise false.
     */
    virtual bool BuildFromInterpolate(const Path& src, const Path& ending, scalar weight);

    /**
     * @brief Transforms Point array, and weight by matrix. Path is replaced by transformed data.
     *
     * @param matrix Matrix to apply to Path
     */
    virtual void Transform(const Matrix& matrix);

    /**
     * @brief Transforms verb array, Point array, and weight by matrix.
     * Transform may change verbs and increase their number.
     * Transformed Path replaces dst; if dst is nullptr, original data is replaced.
     *
     * @param matrix  Matrix to apply to Path
     * @param dst     Overwritten, transformed copy of Path; may be nullptr
     * @param applyPerspectiveClip    Whether to apply perspective clipping
     */
    virtual void TransformWithPerspectiveClip(const Matrix& matrix, Path* dst, bool applyPerspectiveClip);

    /**
     * @brief Offsets Point array by (dx, dy). Path is replaced by offset data.
     *
     * @param dx offset added to Point array x-axis coordinates
     * @param dy offset added to Point array y-axis coordinates
     */
    virtual void Offset(scalar dx, scalar dy);

    /**
     * @brief Offsets Point array by (dx, dy). Path is replaced by offset data.
     *
     * @param dst The pointer of point sets of the dst Path
     * @param dx offset added to Point array x-axis coordinates
     * @param dy offset added to Point array y-axis coordinates
     */
    virtual void Offset(Path* dst, scalar dx, scalar dy);

    virtual bool Op(const Path& path1, Path& path2, PathOp op);

    /**
     * @brief Checks whether the Path is valid.
     */
    bool IsValid() const;

    /**
     * @brief Sets Path to its initial state. Removes Point array, and weights, and sets PathFillType to
     * PathFillType::Winding. Internal storage associated with Path is released.
     */
    virtual void Reset();

    /**
     * @brief A closed contour connects the first and last Point with line, forming a continuous loop.
     */
    virtual void Close();

    /**
     * @brief Gets the length of the current path object.
     * @param forceClosed   Whether to close the Path.
     * @return Returns the length of the current path object.
     */
    scalar GetLength(bool forceClosed) const;

    /**
     * @brief Gets the position and tangent of the distance from the starting position of the Path.
     * @param distance     The distance from the start of the Path, should be greater than 0 and less than 'GetLength()'
     * @param position     Sets to the position of distance from the starting position of the Path.
     * @param tangent      Sets to the tangent of distance from the starting position of the Path.
     * @param forceClosed  Whether to close the Path.
     * @return Returns true if succeeded, otherwise false.
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
