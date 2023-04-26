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
namespace {
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
class PathPlaybackFuncRegister {
public:
    PathPlaybackFuncRegister(PathOpItem::Type type, PathPlayer::PathPlaybackFunc func)
    {
        PathPlayer::opPlaybackFuncLUT_.emplace(type, func);
    }
};

std::unordered_map<uint32_t, PathPlayer::PathPlaybackFunc> PathPlayer::opPlaybackFuncLUT_ = {};

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

static PathPlaybackFuncRegister svgOpPlaybak(PathOpItem::BUILDFROMSVG_OPITEM, BuildFromSVGOpItem::Playback);
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
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
