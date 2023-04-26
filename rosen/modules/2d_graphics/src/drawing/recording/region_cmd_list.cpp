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

#include "recording/region_cmd_list.h"

#include "recording/path_cmd_list.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
RegionCmdList::RegionCmdList() {}

RegionCmdList::RegionCmdList(const CmdListData& data)
{
    opAllocator_.BuildFromDataWithCopy(data.first, data.second);
}

void RegionCmdList::Playback(Region& region) const
{
    int offset = 0;
    do {
        void* itemPtr = opAllocator_.OffsetToAddr(offset);
        OpItem* curOpItemPtr = static_cast<OpItem*>(itemPtr);
        if (curOpItemPtr != nullptr) {
            switch (curOpItemPtr->GetType()) {
                case RegionOpItem::SETRECT_OPITEM:
                    static_cast<SetRectOpItem*>(itemPtr)->Playback(region);
                    break;
                case RegionOpItem::SETPATH_OPITEM:
                    static_cast<SetPathOpItem*>(itemPtr)->Playback(region, opAllocator_);
                    break;
                case RegionOpItem::REGIONOPWITH_OPITEM:
                    static_cast<RegionOpWithOpItem*>(itemPtr)->Playback(region, opAllocator_);
                    break;
                default:
                    LOGE("ColorFilterCmdList unknown OpItem type!");
                    break;
            }
            offset = curOpItemPtr->GetNextOpItemOffset();
        } else {
            LOGE("RegionCmdList Playback failed!");
            break;
        }
    } while (offset != 0);
}

SetRectOpItem::SetRectOpItem(const RectI& rectI) : RegionOpItem(SETRECT_OPITEM), rectI_(rectI) {}

void SetRectOpItem::Playback(Region& region) const
{
    region.SetRect(rectI_);
}

SetPathOpItem::SetPathOpItem(const CmdListSiteInfo& path, const CmdListSiteInfo& region)
    : RegionOpItem(SETPATH_OPITEM), path_(path), region_(region) {}

void SetPathOpItem::Playback(Region& region, const MemAllocator& memAllocator) const
{
    auto* pathPtr = memAllocator.OffsetToAddr(path_.first);
    auto* regionPtr = memAllocator.OffsetToAddr(region_.first);
    if (!pathPtr || !regionPtr) {
        return;
    }

    auto pathCmdList = std::make_shared<PathCmdList>(std::make_pair(pathPtr, path_.second));
    auto regionCmdList = std::make_shared<RegionCmdList>(std::make_pair(regionPtr, region_.second));

    Path restorePath;
    pathCmdList->Playback(restorePath);
    Region restoreRegion;
    regionCmdList->Playback(restoreRegion);

    region.SetPath(restorePath, restoreRegion);
}

RegionOpWithOpItem::RegionOpWithOpItem(const CmdListSiteInfo& region, RegionOp op)
    : RegionOpItem(REGIONOPWITH_OPITEM), region_(region), op_(op) {}

void RegionOpWithOpItem::Playback(Region& region, const MemAllocator& memAllocator) const
{
    auto* regionPtr = memAllocator.OffsetToAddr(region_.first);
    auto regionCmdList = std::make_shared<RegionCmdList>(std::make_pair(regionPtr, region_.second));

    Region restoreRegion;
    regionCmdList->Playback(restoreRegion);

    region.Op(restoreRegion, op_);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
