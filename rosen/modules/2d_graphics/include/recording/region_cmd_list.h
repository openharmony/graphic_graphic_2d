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

#ifndef REGION_CMD_LIST_H
#define REGION_CMD_LIST_H

#include "utils/region.h"
#include "recording/cmd_list.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class DRAWING_API RegionCmdList : public CmdList {
public:
    RegionCmdList() = default;
    ~RegionCmdList() override = default;

    uint32_t GetType() const override
    {
        return Type::REGION_CMD_LIST;
    }

    /*
     * @brief       Creates a RegionCmdList with contiguous buffers.
     * @param data  A contiguous buffers.
     */
    static std::shared_ptr<RegionCmdList> CreateFromData(const CmdListData& data, bool isCopy = false);

    /*
     * @brief  Calls the corresponding operations of all opitems in RegionCmdList to the region.
     */
    std::shared_ptr<Region> Playback() const;
};

/* OpItem */
class RegionOpItem : public OpItem {
public:
    explicit RegionOpItem(uint32_t type) : OpItem(type) {}
    ~RegionOpItem() = default;

    enum Type : uint32_t {
        OPITEM_HEAD = 0,    // OPITEM_HEAD must be 0
        SETRECT_OPITEM,
        SETPATH_OPITEM,
        REGIONOPWITH_OPITEM,
    };
};

class SetRectOpItem : public RegionOpItem {
public:
    explicit SetRectOpItem(const RectI& rectI);
    ~SetRectOpItem() = default;

    void Playback(Region& region) const;
private:
    RectI rectI_;
};

class SetPathOpItem : public RegionOpItem {
public:
    SetPathOpItem(const CmdListHandle& path, const CmdListHandle& region);
    ~SetPathOpItem() = default;

    void Playback(Region& region, const CmdList& CmdList) const;
private:
    CmdListHandle path_;
    CmdListHandle region_;
};

class RegionOpWithOpItem : public RegionOpItem {
public:
    RegionOpWithOpItem(const CmdListHandle& region, RegionOp op);
    ~RegionOpWithOpItem() = default;

    void Playback(Region& region, const CmdList& cmdList) const;
private:
    CmdListHandle region_;
    RegionOp op_;
};

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif // REGION_CMD_LIST_H
