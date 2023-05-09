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

#ifndef PATH_CMD_LIST_H
#define PATH_CMD_LIST_H

#include <unordered_map>

#include "draw/path.h"
#include "recording/cmd_list.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class PathCmdList : public CmdList {
public:
    PathCmdList();
    explicit PathCmdList(const CmdListData& data);
    ~PathCmdList() override = default;

    /*
     * @brief  Calls the corresponding operations of all opitems in PathCmdList to the path.
     */
    void Playback(Path& path) const;
};

/* OpItem */
/*
 * @brief  Helper class for path playback.
 *         Contains the playback context and a static mapping table: { OpItemType， OpItemPlaybackFunc }.
 */
class PathPlayer {
public:
    PathPlayer(Path& path, const MemAllocator& opAllocator);
    ~PathPlayer() = default;

    /*
     * @brief  Obtain the corresponding func according to the type lookup mapping table
     *         and then invoke the func to plays opItem back to path which in context.
     */
    bool Playback(uint32_t type, const void* opItem);

    Path& path_;
    const MemAllocator& opAllocator_;

    using PathPlaybackFunc = void(*)(PathPlayer& palyer, const void* opItem);
private:
    static std::unordered_map<uint32_t, PathPlaybackFunc> opPlaybackFuncLUT_;
};

class PathOpItem : public OpItem {
public:
    PathOpItem(uint32_t type) : OpItem(type) {}
    ~PathOpItem() = default;

    enum Type : uint32_t {
        OPITEM_HEAD,
        BUILDFROMSVG_OPITEM,
        MOVETO_OPITEM,
        LINETO_OPITEM,
        ARCTO_OPITEM,
        CUBICTO_OPITEM,
        QUADTO_OPITEM,
        ADDRECT_OPITEM,
        ADDOVAL_OPITEM,
        ADDARC_OPITEM,
        ADDPOLY_OPITEM,
        ADDCIRCLE_OPITEM,
        ADDRRECT_OPITEM,
        ADDPATH_OPITEM,
        ADDPATHWITHMATRIX_OPITEM,
        REVERSEADDPATH_OPITEM,
        SETFILLSTYLE_OPITEM,
        BUILDFROMINTERPOLATE_OPITEM,
        TRANSFORM_OPITEM,
        OFFSET_OPITEM,
        PATHOPWITH_OPITEM,
        RESET_OPITEM,
        CLOSE_OPITEM,
    };
};

class BuildFromSVGOpItem : public PathOpItem {
public:
    BuildFromSVGOpItem(const std::string& str);
    ~BuildFromSVGOpItem() = default;

    /*
     * @brief         Plays the opItem back into path which hold by Player.
     * @param opItem  opItem static_cast to this class.
     */
    static void Playback(PathPlayer& player, const void* opItem);

    /*
     * @brief  Plays OpItem back into path.
     */
    void Playback(Path& path) const;
private:
    std::string str_;
};

class MoveToOpItem : public PathOpItem {
public:
    MoveToOpItem(const scalar x, const scalar y);
    ~MoveToOpItem() = default;
    static void Playback(PathPlayer& player, const void* opItem);
    void Playback(Path& path) const;
private:
    scalar x_;
    scalar y_;
};

class LineToOpItem : public PathOpItem {
public:
    LineToOpItem(const scalar x, const scalar y);
    ~LineToOpItem() = default;
    static void Playback(PathPlayer& player, const void* opItem);
    void Playback(Path& path) const;
private:
    scalar x_;
    scalar y_;
};

class ArcToOpItem : public PathOpItem {
public:
    ArcToOpItem(const Point& pt1, const Point& pt2, const scalar startAngle, const scalar sweepAngle);
    ArcToOpItem(const scalar rx, const scalar ry, const scalar angle, const PathDirection direction, const scalar endX,
                const scalar endY);
    ~ArcToOpItem() = default;
    static void Playback(PathPlayer& player, const void* opItem);
    void Playback(Path& path) const;
private:
    Point pt1_;
    Point pt2_;
    scalar startAngle_;
    scalar sweepAngle_;
    PathDirection direction_;
    const int methodIndex_;
};

class CubicToOpItem : public PathOpItem {
public:
    CubicToOpItem(const Point& ctrlPt1, const Point& ctrlPt2, const Point& endPt);
    ~CubicToOpItem() = default;
    static void Playback(PathPlayer& player, const void* opItem);
    void Playback(Path& path) const;
private:
    Point ctrlPt1_;
    Point ctrlPt2_;
    Point endPt_;
};

class QuadToOpItem : public PathOpItem {
public:
    QuadToOpItem(const Point& ctrlPt, const Point& endPt);
    ~QuadToOpItem() = default;
    static void Playback(PathPlayer& player, const void* opItem);
    void Playback(Path& path) const;
private:
    Point ctrlPt_;
    Point endPt_;
};

class AddRectOpItem : public PathOpItem {
public:
    AddRectOpItem(const Rect& rect, PathDirection dir);
    ~AddRectOpItem() = default;
    static void Playback(PathPlayer& player, const void* opItem);
    void Playback(Path& path) const;
private:
    Rect rect_;
    PathDirection dir_;
};

class AddOvalOpItem : public PathOpItem {
public:
    AddOvalOpItem(const Rect& oval, PathDirection dir);
    ~AddOvalOpItem() = default;
    static void Playback(PathPlayer& player, const void* opItem);
    void Playback(Path& path) const;
private:
    Rect rect_;
    PathDirection dir_;
};

class AddArcOpItem : public PathOpItem {
public:
    AddArcOpItem(const Rect& oval, const scalar startAngle, const scalar sweepAngle);
    ~AddArcOpItem() = default;
    static void Playback(PathPlayer& player, const void* opItem);
    void Playback(Path& path) const;
private:
    Rect rect_;
    scalar startAngle_;
    scalar sweepAngle_;
};

class AddPolyOpItem : public PathOpItem {
public:
    AddPolyOpItem(const int offset, int count, bool close);
    ~AddPolyOpItem() = default;
    static void Playback(PathPlayer& player, const void* opItem);
    void Playback(Path& path, const MemAllocator& menAllocator) const;
private:
    int offset_;
    int count_;
    bool close_;
};

class AddCircleOpItem : public PathOpItem {
public:
    AddCircleOpItem(const scalar x, const scalar y, const scalar radius, PathDirection dir);
    ~AddCircleOpItem() = default;
    static void Playback(PathPlayer& player, const void* opItem);
    void Playback(Path& path) const;
private:
    scalar x_;
    scalar y_;
    scalar radius_;
    PathDirection dir_;
};

class AddRoundRectOpItem : public PathOpItem {
public:
    AddRoundRectOpItem(const Rect& rect, const scalar xRadius, const scalar yRadius, PathDirection dir);
    AddRoundRectOpItem(const RoundRect& rrect, PathDirection dir);
    ~AddRoundRectOpItem() = default;
    static void Playback(PathPlayer& player, const void* opItem);
    void Playback(Path& path) const;
private:
    RoundRect rrect_;
    PathDirection dir_;
};

class AddPathOpItem : public PathOpItem {
public:
    AddPathOpItem(const CmdListSiteInfo& src, const scalar x, const scalar y);
    AddPathOpItem(const CmdListSiteInfo& src);
    ~AddPathOpItem() = default;
    static void Playback(PathPlayer& player, const void* opItem);
    void Playback(Path& path, const MemAllocator& memAllocator) const;
private:
    CmdListSiteInfo src_;
    scalar x_;
    scalar y_;
    const int methodIndex_;
};

class AddPathWithMatrixOpItem : public PathOpItem {
public:
    AddPathWithMatrixOpItem(const CmdListSiteInfo& src, const Matrix& matrix);
    ~AddPathWithMatrixOpItem() = default;
    static void Playback(PathPlayer& player, const void* opItem);
    void Playback(Path& path, const MemAllocator& memAllocator) const;
private:
    CmdListSiteInfo src_;
    Matrix::Buffer matrixBuffer_;
};

class ReverseAddPathOpItem : public PathOpItem {
public:
    ReverseAddPathOpItem(const CmdListSiteInfo& src);
    ~ReverseAddPathOpItem() = default;
    static void Playback(PathPlayer& player, const void* opItem);
    void Playback(Path& path, const MemAllocator& memAllocator) const;
private:
    CmdListSiteInfo src_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
