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

#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
static constexpr int FUNCTION_OVERLOADING_1 = 1;
static constexpr int FUNCTION_OVERLOADING_2 = 2;

PathCmdList::PathCmdList() {}

PathCmdList::PathCmdList(const CmdListData& data)
{
    opAllocator_.BuildFromDataWithCopy(data.first, data.second);
}

void PathCmdList::Playback(Path& path) const
{
    int offset = 0;
    PathPlayer player(path, opAllocator_);
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

PathPlayer::PathPlayer(Path& path, const MemAllocator& opAllocator) : path_(path), opAllocator_(opAllocator) {}

bool PathPlayer::Playback(uint32_t type, const void* opItem)
{
    auto it = opPlaybackFuncLUT_.find(type);
    if (it == opPlaybackFuncLUT_.end() || it->second == nullptr) {
        return false;
    }

    auto func = it->second;
    (*func)(*this, opItem);

    return true;
}

BuildFromSVGOpItem::BuildFromSVGOpItem(const std::string& str) : PathOpItem(BUILDFROMSVG_OPITEM), str_(str) {}

void BuildFromSVGOpItem::Playback(PathPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto *op = static_cast<const BuildFromSVGOpItem*>(opItem);
        op->Playback(player.path_);
    }
}

void BuildFromSVGOpItem::Playback(Path& path) const
{
    path.BuildFromSVGString(str_);
}

MoveToOpItem::MoveToOpItem(const scalar x, const scalar y) : PathOpItem(MOVETO_OPITEM), x_(x), y_(y) {}

void MoveToOpItem::Playback(PathPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto *op = static_cast<const MoveToOpItem*>(opItem);
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
        const auto *op = static_cast<const LineToOpItem*>(opItem);
        op->Playback(player.path_);
    }
}

void LineToOpItem::Playback(Path& path) const
{
    path.LineTo(x_, y_);
}

ArcToOpItem::ArcToOpItem(const Point& pt1, const Point& pt2, const scalar startAngle, const scalar sweepAngle)
    : PathOpItem(ARCTO_OPITEM), pt1_(pt1), pt2_(pt2), startAngle_(startAngle), sweepAngle_(sweepAngle),
    methodIndex_(FUNCTION_OVERLOADING_1) {}

ArcToOpItem::ArcToOpItem(const scalar rx, const scalar ry, const scalar angle, const PathDirection direction,
    const scalar endX, const scalar endY) : PathOpItem(ARCTO_OPITEM), pt1_(rx, ry), pt2_(endX, endY),
    startAngle_(angle), direction_(direction), methodIndex_(FUNCTION_OVERLOADING_2) {}

void ArcToOpItem::Playback(PathPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto *op = static_cast<const ArcToOpItem*>(opItem);
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
        const auto *op = static_cast<const CubicToOpItem*>(opItem);
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
        const auto *op = static_cast<const QuadToOpItem*>(opItem);
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
        const auto *op = static_cast<const AddRectOpItem*>(opItem);
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
        const auto *op = static_cast<const AddOvalOpItem*>(opItem);
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
        const auto *op = static_cast<const AddArcOpItem*>(opItem);
        op->Playback(player.path_);
    }
}

void AddArcOpItem::Playback(Path& path) const
{
    path.AddArc(rect_, startAngle_, sweepAngle_);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
