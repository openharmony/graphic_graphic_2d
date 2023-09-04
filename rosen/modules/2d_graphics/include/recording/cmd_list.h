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

#include <optional>
#include <vector>

#include "draw/color.h"
#include "recording/op_item.h"
#include "recording/mem_allocator.h"
#include "utils/drawing_macros.h"

namespace OHOS {
namespace Media {
class PixelMap;
}
namespace Rosen {
namespace Drawing {
struct ImageHandle {
    uint32_t offset;
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

class DRAWING_API CmdList {
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
    virtual ~CmdList();

    virtual uint32_t GetType() const
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

        uint32_t offset = opAllocator_.AddrToOffset(op);
        if (lastOpItemOffset_.has_value()) {
            auto* lastOpItem = static_cast<OpItem*>(opAllocator_.OffsetToAddr(lastOpItemOffset_.value()));
            if (lastOpItem != nullptr) {
                lastOpItem->SetNextOpItemOffset(offset);
            }
        }
        lastOpItemOffset_.emplace(offset);
    }

    /*
     * @brief       Add a contiguous buffers to the CmdList.
     * @param src   A contiguous buffers.
     * @return      Returns the offset of the contiguous buffers and CmdList head point.
     */
    uint32_t AddCmdListData(const CmdListData& data);

    const void* GetCmdListData(uint32_t offset) const;

    /*
     * @brief   Gets the contiguous buffers of CmdList.
     */
    CmdListData GetData() const;

    // using for recording, should to remove after using shared memory
    bool SetUpImageData(const void* data, size_t size);
    uint32_t AddImageData(const void* data, size_t size);
    const void* GetImageData(uint32_t offset) const;
    CmdListData GetAllImageData() const;

    /*
     * @brief  return pixelmap index, negative is error.
     */
    uint32_t AddPixelMap(const std::shared_ptr<Media::PixelMap>& pixelMap);

    /*
     * @brief  get pixelmap by index.
     */
    std::shared_ptr<Media::PixelMap> GetPixelMap(uint32_t id);

    /*
     * @brief  return pixelmaplist size, 0 is no pixelmap.
     */
    uint32_t GetAllPixelMap(std::vector<std::shared_ptr<Media::PixelMap>>& pixelMapList);

    /*
     * @brief  return real setup pixelmap size.
     */
    uint32_t SetupPixelMap(const std::vector<std::shared_ptr<Media::PixelMap>>& pixelMapList);

    CmdList(CmdList&&) = delete;
    CmdList(const CmdList&) = delete;
    CmdList& operator=(CmdList&&) = delete;
    CmdList& operator=(const CmdList&) = delete;

protected:
    MemAllocator opAllocator_;
    MemAllocator imageAllocator_;
    std::optional<uint32_t> lastOpItemOffset_ = std::nullopt;
    std::mutex mutex_;
#ifdef SUPPORT_OHOS_PIXMAP
    std::vector<std::shared_ptr<Media::PixelMap>> pixelMapVec_;
    std::mutex pixelMapMutex_;
#endif
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif