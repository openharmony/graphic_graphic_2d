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

#include "recording/cmd_list_helper.h"
#include "recording/path_cmd_list.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
std::shared_ptr<RegionCmdList> RegionCmdList::CreateFromData(const CmdListData& data)
{
    auto cmdList = std::make_shared<RegionCmdList>();
    cmdList->opAllocator_.BuildFromData(data.first, data.second);
    return cmdList;
}

std::shared_ptr<Region> RegionCmdList::Playback() const
{
    if (opAllocator_.GetSize() == 0) {
        return nullptr;
    }

    int32_t offset = 0;
    auto region = std::make_shared<Region>();
    do {
        void* itemPtr = opAllocator_.OffsetToAddr(offset);
        OpItem* curOpItemPtr = static_cast<OpItem*>(itemPtr);
        if (curOpItemPtr == nullptr) {
            LOGE("RegionCmdList Playback failed!");
            break;
        }

        switch (curOpItemPtr->GetType()) {
            case RegionOpItem::OPITEM_HEAD:
                break;
            case RegionOpItem::SETRECT_OPITEM:
                static_cast<SetRectOpItem*>(itemPtr)->Playback(*region);
                break;
            case RegionOpItem::SETPATH_OPITEM:
                static_cast<SetPathOpItem*>(itemPtr)->Playback(*region, *this);
                break;
            case RegionOpItem::REGIONOPWITH_OPITEM:
                static_cast<RegionOpWithOpItem*>(itemPtr)->Playback(*region, *this);
                break;
            default:
                LOGE("unknown OpItem type!");
                break;
        }
        offset = curOpItemPtr->GetNextOpItemOffset();
    } while (offset != 0);

    return region;
}

SetRectOpItem::SetRectOpItem(const RectI& rectI) : RegionOpItem(SETRECT_OPITEM), rectI_(rectI) {}

void SetRectOpItem::Playback(Region& region) const
{
    region.SetRect(rectI_);
}

SetPathOpItem::SetPathOpItem(const CmdListHandle& path, const CmdListHandle& region)
    : RegionOpItem(SETPATH_OPITEM), path_(path), region_(region) {}

void SetPathOpItem::Playback(Region& region, const CmdList& cmdList) const
{
    auto path = CmdListHelper::GetFromCmdList<PathCmdList, Path>(cmdList, path_);
    auto clip = CmdListHelper::GetFromCmdList<RegionCmdList, Region>(cmdList, region_);
    if (path == nullptr || clip == nullptr) {
        return;
    }
    region.SetPath(*path, *clip);
}

RegionOpWithOpItem::RegionOpWithOpItem(const CmdListHandle& region, RegionOp op)
    : RegionOpItem(REGIONOPWITH_OPITEM), region_(region), op_(op) {}

void RegionOpWithOpItem::Playback(Region& region, const CmdList& cmdList) const
{
    auto other = CmdListHelper::GetFromCmdList<RegionCmdList, Region>(cmdList, region_);
    if (other == nullptr) {
        return;
    }
    region.Op(*other, op_);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
