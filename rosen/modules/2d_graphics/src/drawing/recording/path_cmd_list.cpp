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

#include "recording/path_cmd_list.h"

#include "recording/cmd_list_helper.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
static constexpr int32_t FUNCTION_OVERLOADING_1 = 1;
static constexpr int32_t FUNCTION_OVERLOADING_2 = 2;

std::shared_ptr<PathCmdList> PathCmdList::CreateFromData(const CmdListData& data, bool isCopy)
{
    auto cmdList = std::make_shared<PathCmdList>();
    if (isCopy) {
        cmdList->opAllocator_.BuildFromDataWithCopy(data.first, data.second);
    } else {
        cmdList->opAllocator_.BuildFromData(data.first, data.second);
    }
    return cmdList;
}

std::shared_ptr<Path> PathCmdList::Playback() const
{
    uint32_t offset = 0;
    auto path = std::make_shared<Path>();
    PathPlayer player(*path, *this);
    do {
        void* itemPtr = opAllocator_.OffsetToAddr(offset);
        OpItem* curOpItemPtr = static_cast<OpItem*>(itemPtr);
        if (curOpItemPtr != nullptr) {
            if (!player.Playback(curOpItemPtr->GetType(), itemPtr)) {
                LOGE("PathCmdList::Playback failed!");
                break;
            }

            offset = curOpItemPtr->GetNextOpItemOffset();
        } else {
            LOGE("PathCmdList::Playback failed, opItem is nullptr");
            break;
        }
    } while (offset != 0);

    return path;
}

/* OpItem */
std::unordered_map<uint32_t, PathPlayer::PathPlaybackFunc> PathPlayer::opPlaybackFuncLUT_ = {
    { PathOpItem::BUILDFROMSVG_OPITEM,          BuildFromSVGOpItem::Playback },
    { PathOpItem::MOVETO_OPITEM,                MoveToOpItem::Playback },
    { PathOpItem::LINETO_OPITEM,                LineToOpItem::Playback },
    { PathOpItem::ARCTO_OPITEM,                 ArcToOpItem::Playback },
    { PathOpItem::CUBICTO_OPITEM,               CubicToOpItem::Playback },
    { PathOpItem::QUADTO_OPITEM,                QuadToOpItem::Playback },
    { PathOpItem::ADDRECT_OPITEM,               AddRectOpItem::Playback },
    { PathOpItem::ADDOVAL_OPITEM,               AddOvalOpItem::Playback },
    { PathOpItem::ADDARC_OPITEM,                AddArcOpItem::Playback },
    { PathOpItem::ADDPOLY_OPITEM,               AddPolyOpItem::Playback },
    { PathOpItem::ADDCIRCLE_OPITEM,             AddCircleOpItem::Playback },
    { PathOpItem::ADDRRECT_OPITEM,              AddRoundRectOpItem::Playback },
    { PathOpItem::ADDPATH_OPITEM,               AddPathOpItem::Playback },
    { PathOpItem::ADDPATHWITHMATRIX_OPITEM,     AddPathWithMatrixOpItem::Playback },
    { PathOpItem::REVERSEADDPATH_OPITEM,        ReverseAddPathOpItem::Playback },
    { PathOpItem::SETFILLSTYLE_OPITEM,          SetFillStyleOpItem::Playback },
    { PathOpItem::BUILDFROMINTERPOLATE_OPITEM,  BuildFromInterpolateOpItem::Playback },
    { PathOpItem::TRANSFORM_OPITEM,             TransformOpItem::Playback },
    { PathOpItem::OFFSET_OPITEM,                OffsetOpItem::Playback },
    { PathOpItem::PATHOPWITH_OPITEM,            PathOpWithOpItem::Playback },
    { PathOpItem::RESET_OPITEM,                 ResetOpItem::Playback },
    { PathOpItem::CLOSE_OPITEM,                 CloseOpItem::Playback },
};

PathPlayer::PathPlayer(Path& path, const CmdList& cmdList) : path_(path), cmdList_(cmdList) {}

bool PathPlayer::Playback(uint32_t type, const void* opItem)
{
    if (type == PathOpItem::OPITEM_HEAD) {
        return true;
    }

    auto it = opPlaybackFuncLUT_.find(type);
    if (it == opPlaybackFuncLUT_.end() || it->second == nullptr) {
        return false;
    }

    auto func = it->second;
    (*func)(*this, opItem);

    return true;
}

BuildFromSVGOpItem::BuildFromSVGOpItem(const uint32_t offset, const size_t size)
    : PathOpItem(BUILDFROMSVG_OPITEM), offset_(offset), size_(size) {}

void BuildFromSVGOpItem::Playback(PathPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto* op = static_cast<const BuildFromSVGOpItem*>(opItem);
        op->Playback(player.path_, player.cmdList_);
    }
}

void BuildFromSVGOpItem::Playback(Path& path, const CmdList& cmdList) const
{
    std::string str(static_cast<const char*>(cmdList.GetCmdListData(offset_)), size_);
    path.BuildFromSVGString(str);
}

MoveToOpItem::MoveToOpItem(const scalar x, const scalar y) : PathOpItem(MOVETO_OPITEM), x_(x), y_(y) {}

void MoveToOpItem::Playback(PathPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto* op = static_cast<const MoveToOpItem*>(opItem);
        op->Playback(player.path_);
    }
}

void MoveToOpItem::Playback(Path& path) const
{
    path.MoveTo(x_, y_);
}

LineToOpItem::LineToOpItem(const scalar x, const scalar y) : PathOpItem(LINETO_OPITEM), x_(x), y_(y) {}

void LineToOpItem::Playback(PathPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto* op = static_cast<const LineToOpItem*>(opItem);
        op->Playback(player.path_);
    }
}

void LineToOpItem::Playback(Path& path) const
{
    path.LineTo(x_, y_);
}

ArcToOpItem::ArcToOpItem(const Point& pt1, const Point& pt2, const scalar startAngle, const scalar sweepAngle)
    : PathOpItem(ARCTO_OPITEM), pt1_(pt1), pt2_(pt2), startAngle_(startAngle), sweepAngle_(sweepAngle),
    direction_(PathDirection::CW_DIRECTION), methodIndex_(FUNCTION_OVERLOADING_1) {}

ArcToOpItem::ArcToOpItem(const scalar rx, const scalar ry, const scalar angle, const PathDirection direction,
    const scalar endX, const scalar endY) : PathOpItem(ARCTO_OPITEM), pt1_(rx, ry), pt2_(endX, endY),
    startAngle_(angle), sweepAngle_(0), direction_(direction), methodIndex_(FUNCTION_OVERLOADING_2) {}

void ArcToOpItem::Playback(PathPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto* op = static_cast<const ArcToOpItem*>(opItem);
        op->Playback(player.path_);
    }
}

void ArcToOpItem::Playback(Path& path) const
{
    if (methodIndex_ == FUNCTION_OVERLOADING_1) {
        path.ArcTo(pt1_, pt2_, startAngle_, sweepAngle_);
    } else if (methodIndex_ == FUNCTION_OVERLOADING_2) {
        path.ArcTo(pt1_.GetX(), pt1_.GetY(), startAngle_, direction_, pt2_.GetX(), pt2_.GetY());
    }
}

CubicToOpItem::CubicToOpItem(const Point& ctrlPt1, const Point& ctrlPt2, const Point& endPt)
    : PathOpItem(CUBICTO_OPITEM), ctrlPt1_(ctrlPt1), ctrlPt2_(ctrlPt2), endPt_(endPt) {}

void CubicToOpItem::Playback(PathPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto* op = static_cast<const CubicToOpItem*>(opItem);
        op->Playback(player.path_);
    }
}

void CubicToOpItem::Playback(Path& path) const
{
    path.CubicTo(ctrlPt1_, ctrlPt2_, endPt_);
}

QuadToOpItem::QuadToOpItem(const Point& ctrlPt, const Point& endPt)
    : PathOpItem(QUADTO_OPITEM), ctrlPt_(ctrlPt), endPt_(endPt) {}

void QuadToOpItem::Playback(PathPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto* op = static_cast<const QuadToOpItem*>(opItem);
        op->Playback(player.path_);
    }
}

void QuadToOpItem::Playback(Path& path) const
{
    path.QuadTo(ctrlPt_, endPt_);
}

AddRectOpItem::AddRectOpItem(const Rect& rect, PathDirection dir)
    : PathOpItem(ADDRECT_OPITEM), rect_(rect), dir_(dir) {}

void AddRectOpItem::Playback(PathPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto* op = static_cast<const AddRectOpItem*>(opItem);
        op->Playback(player.path_);
    }
}

void AddRectOpItem::Playback(Path& path) const
{
    path.AddRect(rect_, dir_);
}

AddOvalOpItem::AddOvalOpItem(const Rect& oval, PathDirection dir)
    : PathOpItem(ADDOVAL_OPITEM), rect_(oval), dir_(dir) {}

void AddOvalOpItem::Playback(PathPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto* op = static_cast<const AddOvalOpItem*>(opItem);
        op->Playback(player.path_);
    }
}

void AddOvalOpItem::Playback(Path& path) const
{
    path.AddOval(rect_, dir_);
}

AddArcOpItem::AddArcOpItem(const Rect& oval, const scalar startAngle, const scalar sweepAngle)
    : PathOpItem(ADDARC_OPITEM), rect_(oval), startAngle_(startAngle), sweepAngle_(sweepAngle) {}

void AddArcOpItem::Playback(PathPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto* op = static_cast<const AddArcOpItem*>(opItem);
        op->Playback(player.path_);
    }
}

void AddArcOpItem::Playback(Path& path) const
{
    path.AddArc(rect_, startAngle_, sweepAngle_);
}

AddPolyOpItem::AddPolyOpItem(const std::pair<uint32_t, size_t>& points, int32_t count, bool close)
    : PathOpItem(ADDPOLY_OPITEM), points_(points), count_(count), close_(close) {}

void AddPolyOpItem::Playback(PathPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto* op = static_cast<const AddPolyOpItem*>(opItem);
        op->Playback(player.path_, player.cmdList_);
    }
}

void AddPolyOpItem::Playback(Path& path, const CmdList& cmdList) const
{
    std::vector<Point> points = CmdListHelper::GetVectorFromCmdList<Point>(cmdList, points_);
    path.AddPoly(points, count_, close_);
}

AddCircleOpItem::AddCircleOpItem(const scalar x, const scalar y, const scalar radius, PathDirection dir)
    : PathOpItem(ADDCIRCLE_OPITEM), x_(x), y_(y), radius_(radius), dir_(dir) {}

void AddCircleOpItem::Playback(PathPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto* op = static_cast<const AddCircleOpItem*>(opItem);
        op->Playback(player.path_);
    }
}

void AddCircleOpItem::Playback(Path& path) const
{
    path.AddCircle(x_, y_, radius_, dir_);
}

AddRoundRectOpItem::AddRoundRectOpItem(const std::pair<uint32_t, size_t>& radiusXYData, const Rect& rect,
    PathDirection dir) : PathOpItem(ADDRRECT_OPITEM), radiusXYData_(radiusXYData), rect_(rect), dir_(dir) {}

void AddRoundRectOpItem::Playback(PathPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto* op = static_cast<const AddRoundRectOpItem*>(opItem);
        op->Playback(player.path_, player.cmdList_);
    }
}

void AddRoundRectOpItem::Playback(Path& path, const CmdList& cmdList) const
{
    auto radiusXYData = CmdListHelper::GetVectorFromCmdList<Point>(cmdList, radiusXYData_);
    RoundRect roundRect(rect_, radiusXYData);

    path.AddRoundRect(roundRect, dir_);
}

AddPathOpItem::AddPathOpItem(const CmdListHandle& src, const scalar x, const scalar y)
    : PathOpItem(ADDPATH_OPITEM), src_(src), x_(x), y_(y), methodIndex_(FUNCTION_OVERLOADING_1) {}

AddPathOpItem::AddPathOpItem(const CmdListHandle& src)
    : PathOpItem(ADDPATH_OPITEM), src_(src), x_(0), y_(0), methodIndex_(FUNCTION_OVERLOADING_2) {}

void AddPathOpItem::Playback(PathPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto* op = static_cast<const AddPathOpItem*>(opItem);
        op->Playback(player.path_, player.cmdList_);
    }
}

void AddPathOpItem::Playback(Path& path, const CmdList& cmdList) const
{
    auto srcPath = CmdListHelper::GetFromCmdList<PathCmdList, Path>(cmdList, src_);
    if (srcPath == nullptr) {
        return;
    }

    if (methodIndex_ == FUNCTION_OVERLOADING_1) {
        path.AddPath(*srcPath, x_, y_);
    } else if (methodIndex_ == FUNCTION_OVERLOADING_2) {
        path.AddPath(*srcPath);
    }
}

AddPathWithMatrixOpItem::AddPathWithMatrixOpItem(const CmdListHandle& src, const Matrix& matrix)
    : PathOpItem(ADDPATHWITHMATRIX_OPITEM), src_(src)
{
    matrix.GetAll(matrixBuffer_);
}

void AddPathWithMatrixOpItem::Playback(PathPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto* op = static_cast<const AddPathWithMatrixOpItem*>(opItem);
        op->Playback(player.path_, player.cmdList_);
    }
}

void AddPathWithMatrixOpItem::Playback(Path& path, const CmdList& cmdList) const
{
    auto srcPath = CmdListHelper::GetFromCmdList<PathCmdList, Path>(cmdList, src_);
    if (srcPath == nullptr) {
        return;
    }

    Matrix matrix;
    for (uint32_t i = 0; i < matrixBuffer_.size(); i++) {
        matrix.Set(static_cast<Matrix::Index>(i), matrixBuffer_[i]);
    }

    path.AddPath(*srcPath, matrix);
}

ReverseAddPathOpItem::ReverseAddPathOpItem(const CmdListHandle& src)
    : PathOpItem(REVERSEADDPATH_OPITEM), src_(src) {}

void ReverseAddPathOpItem::Playback(PathPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto* op = static_cast<const ReverseAddPathOpItem*>(opItem);
        op->Playback(player.path_, player.cmdList_);
    }
}

void ReverseAddPathOpItem::Playback(Path& path, const CmdList& cmdList) const
{
    auto srcPath = CmdListHelper::GetFromCmdList<PathCmdList, Path>(cmdList, src_);
    if (srcPath == nullptr) {
        return;
    }

    path.ReverseAddPath(*srcPath);
}

SetFillStyleOpItem::SetFillStyleOpItem(PathFillType fillstyle)
    : PathOpItem(SETFILLSTYLE_OPITEM), fillstyle_(fillstyle) {}

void SetFillStyleOpItem::Playback(PathPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto* op = static_cast<const SetFillStyleOpItem*>(opItem);
        op->Playback(player.path_);
    }
}

void SetFillStyleOpItem::Playback(Path& path) const
{
    path.SetFillStyle(fillstyle_);
}

BuildFromInterpolateOpItem::BuildFromInterpolateOpItem(const CmdListHandle& src, const CmdListHandle& ending,
    const scalar weight) : PathOpItem(BUILDFROMINTERPOLATE_OPITEM), src_(src), ending_(ending), weight_(weight) {}

void BuildFromInterpolateOpItem::Playback(PathPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto* op = static_cast<const BuildFromInterpolateOpItem*>(opItem);
        op->Playback(player.path_, player.cmdList_);
    }
}

void BuildFromInterpolateOpItem::Playback(Path& path, const CmdList& cmdList) const
{
    auto srcPath = CmdListHelper::GetFromCmdList<PathCmdList, Path>(cmdList, src_);
    auto endingPath = CmdListHelper::GetFromCmdList<PathCmdList, Path>(cmdList, ending_);
    if (srcPath == nullptr || endingPath == nullptr) {
        return;
    }

    path.BuildFromInterpolate(*srcPath, *endingPath, weight_);
}

TransformOpItem::TransformOpItem(const Matrix& matrix) : PathOpItem(TRANSFORM_OPITEM)
{
    matrix.GetAll(matrixBuffer_);
}

void TransformOpItem::Playback(PathPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto* op = static_cast<const TransformOpItem*>(opItem);
        op->Playback(player.path_);
    }
}

void TransformOpItem::Playback(Path& path) const
{
    Matrix matrix;
    for (uint32_t i = 0; i < matrixBuffer_.size(); i++) {
        matrix.Set(static_cast<Matrix::Index>(i), matrixBuffer_[i]);
    }

    path.Transform(matrix);
}

OffsetOpItem::OffsetOpItem(const scalar dx, const scalar dy) : PathOpItem(OFFSET_OPITEM), x_(dx), y_(dy) {}

void OffsetOpItem::Playback(PathPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto* op = static_cast<const OffsetOpItem*>(opItem);
        op->Playback(player.path_);
    }
}

void OffsetOpItem::Playback(Path& path) const
{
    path.Offset(x_, y_);
}

PathOpWithOpItem::PathOpWithOpItem(const CmdListHandle& path1, const CmdListHandle& path2, PathOp op)
    : PathOpItem(PATHOPWITH_OPITEM), path1_(path1), path2_(path2), op_(op) {}

void PathOpWithOpItem::Playback(PathPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto* op = static_cast<const PathOpWithOpItem*>(opItem);
        op->Playback(player.path_, player.cmdList_);
    }
}

void PathOpWithOpItem::Playback(Path& path, const CmdList& cmdList) const
{
    auto path1 = CmdListHelper::GetFromCmdList<PathCmdList, Path>(cmdList, path1_);
    auto path2 = CmdListHelper::GetFromCmdList<PathCmdList, Path>(cmdList, path2_);
    if (path1 == nullptr || path2 == nullptr) {
        return;
    }

    path.Op(*path1, *path2, op_);
}

ResetOpItem::ResetOpItem() : PathOpItem(RESET_OPITEM) {}

void ResetOpItem::Playback(PathPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto* op = static_cast<const ResetOpItem*>(opItem);
        op->Playback(player.path_);
    }
}

void ResetOpItem::Playback(Path& path) const
{
    path.Reset();
}

CloseOpItem::CloseOpItem() : PathOpItem(CLOSE_OPITEM) {}

void CloseOpItem::Playback(PathPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto* op = static_cast<const CloseOpItem*>(opItem);
        op->Playback(player.path_);
    }
}

void CloseOpItem::Playback(Path& path) const
{
    path.Close();
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
