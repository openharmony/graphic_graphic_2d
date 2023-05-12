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

#include "draw/color.h"
#include "recording/op_item.h"
#include "recording/mem_allocator.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
struct ImageHandle {
    int32_t offset;
    size_t size;
    int32_t width;
    int32_t height;
    ColorType colorType;
    AlphaType alphaType;
};

struct CmdListHandle {
    uint32_t type;
    uint32_t offset;
    size_t size;
    uint32_t imageOffset;
    size_t imageSize;
};

using CmdListData = std::pair<const void*, size_t>;

class CmdList {
public:
    enum Type : uint32_t {
        CMD_LIST = 0,
        COLOR_FILTER_CMD_LIST,
        COLOR_SPACE_CMD_LIST,
        DRAW_CMD_LIST,
        IMAGE_FILTER_CMD_LIST,
        MASK_FILTER_CMD_LIST,
        PATH_CMD_LIST,
        PATH_EFFECT_CMD_LIST,
        REGION_CMD_LIST,
        SHADER_EFFECT_CMD_LIST,
    };

    CmdList() = default;
    explicit CmdList(const CmdListData& cmdListData);
    virtual ~CmdList() = default;

    virtual uint32_t GetType()
    {
        return Type::CMD_LIST;
    }

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

    const void* GetCmdListData(uint32_t offset) const;

    /*
     * @brief   Gets the contiguous buffers of CmdList.
     */
    CmdListData GetData() const;

    // using for recording, should to remove after using shared memory
    bool SetUpImageData(const void* data, size_t size);
    int AddImageData(const void* data, size_t size);
    const void* GetImageData(uint32_t offset) const;
    CmdListData GetAllImageData() const;

    CmdList(CmdList&&) = delete;
    CmdList(const CmdList&) = delete;
    CmdList& operator=(CmdList&&) = delete;
    CmdList& operator=(const CmdList&) = delete;

protected:
    MemAllocator opAllocator_;
    MemAllocator imageAllocator_;
    OpItem* lastOpItem_ = nullptr;
    std::mutex mutex_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif