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

#include "recording/cmd_list.h"

#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
static constexpr uint32_t OPITEM_HEAD = 0;

CmdList::CmdList(const CmdListData& cmdListData)
{
    opAllocator_.BuildFromData(cmdListData.first, cmdListData.second);
}

int CmdList::AddCmdListData(const CmdListData& data)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (lastOpItem_ == nullptr) {
        lastOpItem_ = opAllocator_.Allocate<OpItem>(OPITEM_HEAD);
    }
    void* addr = opAllocator_.Add(data.first, data.second);
    if (addr == nullptr) {
        LOGE("CmdList AddCmdListData failed!");
        return 0;
    }
    return opAllocator_.AddrToOffset(addr);
}

const void* CmdList::GetCmdListData(uint32_t offset) const
{
    return opAllocator_.OffsetToAddr(offset);
}

CmdListData CmdList::GetData() const
{
    return std::make_pair(opAllocator_.GetData(), opAllocator_.GetSize());
}

bool CmdList::SetUpImageData(const void* data, size_t size)
{
    return imageAllocator_.BuildFromData(data, size);
}

int CmdList::AddImageData(const void* data, size_t size)
{
    std::lock_guard<std::mutex> lock(mutex_);
    void* addr = imageAllocator_.Add(data, size);
    if (addr == nullptr) {
        LOGE("CmdList AddImageData failed!");
        return 0;
    }
    return imageAllocator_.AddrToOffset(addr);
}

const void* CmdList::GetImageData(uint32_t offset) const
{
    return imageAllocator_.OffsetToAddr(offset);
}

CmdListData CmdList::GetAllImageData() const
{
    return std::make_pair(opAllocator_.GetData(), opAllocator_.GetSize());
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
