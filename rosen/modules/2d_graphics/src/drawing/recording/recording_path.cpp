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

#include "recording/recording_path.h"

#include "recording/cmd_list_helper.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
RecordingPath::RecordingPath() noexcept : cmdList_(std::make_shared<PathCmdList>()) {}

std::shared_ptr<PathCmdList> RecordingPath::GetCmdList() const
{
    return cmdList_;
}

bool RecordingPath::BuildFromSVGString(const std::string& str)
{
    cmdList_->AddOp<BuildFromSVGOpItem>(str);
    return true;
}

void RecordingPath::MoveTo(scalar x, scalar y)
{
    cmdList_->AddOp<MoveToOpItem>(x, y);
}

void RecordingPath::LineTo(scalar x, scalar y)
{
    cmdList_->AddOp<LineToOpItem>(x, y);
}

void RecordingPath::ArcTo(scalar pt1X, scalar pt1Y, scalar pt2X, scalar pt2Y, scalar startAngle, scalar sweepAngle)
{
    cmdList_->AddOp<ArcToOpItem>(Point(pt1X, pt1Y), Point(pt2X, pt2Y), startAngle, sweepAngle);
}

void RecordingPath::ArcTo(const Point& pt1, const Point& pt2, scalar startAngle, scalar sweepAngle)
{
    cmdList_->AddOp<ArcToOpItem>(pt1, pt2, startAngle, sweepAngle);
}

void RecordingPath::ArcTo(scalar rx, scalar ry, scalar angle, PathDirection direction, scalar endX, scalar endY)
{
    cmdList_->AddOp<ArcToOpItem>(rx, ry, angle, direction, endX, endY);
}

void RecordingPath::CubicTo(scalar ctrlPt1X, scalar ctrlPt1Y, scalar ctrlPt2X, scalar ctrlPt2Y,
    scalar endPtX, scalar endPtY)
{
    cmdList_->AddOp<CubicToOpItem>(Point(ctrlPt1X, ctrlPt1Y), Point(ctrlPt2X, ctrlPt2Y), Point(endPtX, endPtY));
}

void RecordingPath::CubicTo(const Point& ctrlPt1, const Point& ctrlPt2, const Point& endPt)
{
    cmdList_->AddOp<CubicToOpItem>(ctrlPt1, ctrlPt2, endPt);
}

void RecordingPath::QuadTo(scalar ctrlPtX, scalar ctrlPtY, scalar endPtX, scalar endPtY)
{
    cmdList_->AddOp<QuadToOpItem>(Point(ctrlPtX, ctrlPtY), Point(endPtX, endPtY));
}

void RecordingPath::QuadTo(const Point& ctrlPt, const Point endPt)
{
    cmdList_->AddOp<QuadToOpItem>(ctrlPt, endPt);
}

void RecordingPath::AddRect(const Rect& rect, PathDirection dir)
{
    cmdList_->AddOp<AddRectOpItem>(rect, dir);
}

void RecordingPath::AddRect(scalar left, scalar top, scalar right, scalar bottom, PathDirection dir)
{
    cmdList_->AddOp<AddRectOpItem>(Rect(left, top, right, bottom), dir);
}

void RecordingPath::AddOval(const Rect& oval, PathDirection dir)
{
    cmdList_->AddOp<AddOvalOpItem>(oval, dir);
}

void RecordingPath::AddArc(const Rect& oval, scalar startAngle, scalar sweepAngle)
{
    cmdList_->AddOp<AddArcOpItem>(oval, startAngle, sweepAngle);
}

void RecordingPath::AddPoly(const std::vector<Point>& points, int count, bool close)
{
    auto pointsInfo = CmdListHelper::AddVectorToCmdList<Point>(*cmdList_, points);
    cmdList_->AddOp<AddPolyOpItem>(pointsInfo, count, close);
}

void RecordingPath::AddCircle(scalar x, scalar y, scalar radius, PathDirection dir)
{
    cmdList_->AddOp<AddCircleOpItem>(x, y, radius, dir);
}

void RecordingPath::AddRoundRect(const Rect& rect, scalar xRadius, scalar yRadius, PathDirection dir)
{
    cmdList_->AddOp<AddRoundRectOpItem>(rect, xRadius, yRadius, dir);
}

void RecordingPath::AddRoundRect(const RoundRect& rrect, PathDirection dir)
{
    cmdList_->AddOp<AddRoundRectOpItem>(rrect, dir);
}

void RecordingPath::AddPath(const Path& src, scalar dx, scalar dy)
{
    auto pathHandle = CmdListHelper::AddRecordedToCmdList<RecordingPath>(*cmdList_, src);
    cmdList_->AddOp<AddPathOpItem>(pathHandle, dx, dy);
}

void RecordingPath::AddPath(const Path& src)
{
    auto pathHandle = CmdListHelper::AddRecordedToCmdList<RecordingPath>(*cmdList_, src);
    cmdList_->AddOp<AddPathOpItem>(pathHandle);
}

void RecordingPath::AddPath(const Path& src, const Matrix& matrix)
{
    auto pathHandle = CmdListHelper::AddRecordedToCmdList<RecordingPath>(*cmdList_, src);
    cmdList_->AddOp<AddPathWithMatrixOpItem>(pathHandle, matrix);
}

void RecordingPath::ReverseAddPath(const Path& src)
{
    auto pathHandle = CmdListHelper::AddRecordedToCmdList<RecordingPath>(*cmdList_, src);
    cmdList_->AddOp<ReverseAddPathOpItem>(pathHandle);
}

void RecordingPath::SetFillStyle(PathFillType fillstyle)
{
    cmdList_->AddOp<SetFillStyleOpItem>(fillstyle);
}

bool RecordingPath::BuildFromInterpolate(const Path& src, const Path& ending, scalar weight)
{
    auto srcHandle = CmdListHelper::AddRecordedToCmdList<RecordingPath>(*cmdList_, src);
    auto endingHandle = CmdListHelper::AddRecordedToCmdList<RecordingPath>(*cmdList_, ending);
    cmdList_->AddOp<BuildFromInterpolateOpItem>(srcHandle, endingHandle, weight);

    return true;
}

void RecordingPath::Transform(const Matrix& matrix)
{
    cmdList_->AddOp<TransformOpItem>(matrix);
}

void RecordingPath::Offset(scalar dx, scalar dy)
{
    cmdList_->AddOp<OffsetOpItem>(dx, dy);
}

bool RecordingPath::Op(const Path& path1, Path& path2, PathOp op)
{
    auto pathHandle1 = CmdListHelper::AddRecordedToCmdList<RecordingPath>(*cmdList_, path1);
    auto pathHandle2 = CmdListHelper::AddRecordedToCmdList<RecordingPath>(*cmdList_, path2);
    cmdList_->AddOp<PathOpWithOpItem>(pathHandle1, pathHandle2, op);

    return true;
}

void RecordingPath::Reset()
{
    cmdList_->AddOp<ResetOpItem>();
}

void RecordingPath::Close()
{
    cmdList_->AddOp<CloseOpItem>();
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
