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
class PathPlaybackFuncRegister;
class PathPlayer {
public:
    PathPlayer(Path& path, const MemAllocator& opAllocator);
    ~PathPlayer() = default;

    bool Playback(uint32_t type, const void* opItem);

    Path& path_;
    const MemAllocator& opAllocator_;

    using PathPlaybackFunc = void(*)(PathPlayer& palyer, const void* opItem);
private:
    static std::unordered_map<uint32_t, PathPlaybackFunc> opPlaybackFuncLUT_;
    friend PathPlaybackFuncRegister;
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
    static void Playback(PathPlayer& player, const void* opItem);
    void Playback(Path& path) const;
private:
    std::string str_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

#endif // DRAW_CMD_LIST_H
