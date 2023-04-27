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

#ifndef CMD_LIST_H
#define CMD_LIST_H

#include "op_item.h"
#include "recording/mem_allocator.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class CmdList {
public:
    virtual ~CmdList() = default;

    /*
     * @brief       Add OpItem to CmdList.
     * @param T     The name of OpItem class.
     * @param Args  Constructs arguments to the OpItem.
     */
    template<typename T, typename... Args>
    void AddOp(Args&&... args)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        T* op = opAllocator_.Allocate<T>(std::forward<Args>(args)...);
        if (op == nullptr) {
            return;
        }
        if (lastOpItem_ != nullptr) {
            int offset = opAllocator_.AddrToOffset(op);
            lastOpItem_->SetNextOpItemOffset(offset);
        }
        lastOpItem_ = op;
    }

    /*
     * @brief       Add a contiguous buffers to the CmdList.
     * @param src   A contiguous buffers.
     * @return      Returns the offset of the contiguous buffers and CmdList head point.
     */
    int AddCmdListData(const CmdListData& data);

    /*
     * @brief   Gets the contiguous buffers of CmdList.
     */
    CmdListData GetData() const;

    CmdList(CmdList&&) = delete;
    CmdList(const CmdList&) = delete;
    CmdList& operator=(CmdList&&) = delete;
    CmdList& operator=(const CmdList&) = delete;

protected:
    CmdList() = default;
    explicit CmdList(const CmdListData& cmdListData);

    MemAllocator opAllocator_;
    OpItem* lastOpItem_ = nullptr;
    std::mutex mutex_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif