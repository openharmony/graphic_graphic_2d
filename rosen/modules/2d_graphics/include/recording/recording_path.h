/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef RECORDING_PATH_H
#define RECORDING_PATH_H

#include "draw/path.h"
#include "recording/path_cmd_list.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class RecordingPath : public Path {
public:
    RecordingPath() noexcept;
    ~RecordingPath() override = default;

    DrawingType GetDrawingType() const override
    {
        return DrawingType::RECORDING;
    }

    std::shared_ptr<PathCmdList> GetCmdList() const;

    bool BuildFromSVGString(const std::string& str) override;

    void MoveTo(scalar x, scalar y) override;
    void LineTo(scalar x, scalar y) override;
    void ArcTo(scalar pt1X, scalar pt1Y, scalar pt2X, scalar pt2Y, scalar startAngle, scalar sweepAngle) override;
    void ArcTo(const Point& pt1, const Point& pt2, scalar startAngle, scalar sweepAngle) override;
    void ArcTo(scalar rx, scalar ry, scalar angle, PathDirection direction, scalar endX, scalar endY) override;
    void CubicTo(scalar ctrlPt1X, scalar ctrlPt1Y, scalar ctrlPt2X, scalar ctrlPt2Y,
        scalar endPtX, scalar endPtY) override;
    void CubicTo(const Point& ctrlPt1, const Point& ctrlPt2, const Point& endPt) override;
    void QuadTo(scalar ctrlPtX, scalar ctrlPtY, scalar endPtX, scalar endPtY) override;
    void QuadTo(const Point& ctrlPt, const Point endPt) override;

    void AddRect(const Rect& rect, PathDirection dir = PathDirection::CW_DIRECTION) override;
    void AddRect(scalar left, scalar top, scalar right, scalar bottom,
        PathDirection dir = PathDirection::CW_DIRECTION) override;

    void AddOval(const Rect& oval, PathDirection dir = PathDirection::CW_DIRECTION) override;
    void AddArc(const Rect& oval, scalar startAngle, scalar sweepAngle) override;
    void AddPoly(const std::vector<Point>& points, int count, bool close) override;
    void AddCircle(scalar x, scalar y, scalar radius, PathDirection dir = PathDirection::CW_DIRECTION) override;
    void AddRoundRect(
        const Rect& rect, scalar xRadius, scalar yRadius, PathDirection dir = PathDirection::CW_DIRECTION) override;
    void AddRoundRect(const RoundRect& roundRect, PathDirection dir = PathDirection::CW_DIRECTION) override;

    void AddPath(const Path& src, scalar dx, scalar dy) override;
    void AddPath(const Path& src) override;
    void AddPath(const Path& src, const Matrix& matrix) override;
    void ReverseAddPath(const Path& src) override;

    void SetFillStyle(PathFillType fillstyle) override;

    bool BuildFromInterpolate(const Path& src, const Path& ending, scalar weight) override;
    void Transform(const Matrix& matrix) override;
    void Offset(scalar dx, scalar dy) override;
    bool Op(const Path& path1, Path& path2, PathOp op) override;

    void Reset() override;
    void Close() override;
private:
    std::shared_ptr<PathCmdList> cmdList_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
