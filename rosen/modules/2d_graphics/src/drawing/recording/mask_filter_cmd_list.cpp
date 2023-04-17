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

#include "recording/mask_filter_cmd_list.h"

#include "recording/recording_mask_filter.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
/* MaskFilterCmdList */
std::shared_ptr<MaskFilterCmdList> MaskFilterCmdList::CreateFromData(CmdListData data)
{
    auto cmdList = std::make_shared<MaskFilterCmdList>();
    if (cmdList == nullptr) {
        LOGE("MaskFilterCmdList create from data failed!");
        return nullptr;
    }
    cmdList->allocator_.BuildFromData(data.first, data.second);
    return cmdList;
}

Offset_t MaskFilterCmdList::AddCmdListData(CmdListData src)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (lastOpItem == nullptr) {
        lastOpItem = allocator_.Allocate<OpItem>(MaskFilterOpItem::OPITEM_HEAD);
    }
    void* dst = allocator_.Add(src.first, src.second);
    if (dst == nullptr) {
        LOGE("MaskFilterCmdList AddCmdListData failed!");
        return 0;
    }
    return allocator_.AddrToOffset(dst);
}

CmdListData MaskFilterCmdList::GetData() const
{
    const void* data = allocator_.GetData();
    size_t size = allocator_.GetSize();
    return std::pair<const void*, size_t>(data, size);
}

std::shared_ptr<MaskFilter> MaskFilterCmdList::Playback() const
{
    Offset_t offset = 0;
    std::shared_ptr<MaskFilter> mf = nullptr;

    do {
        OpItem* itemPtr = static_cast<OpItem*>(allocator_.OffsetToAddr(offset));
        if (itemPtr != nullptr) {
            switch (itemPtr->GetType()) {
                case MaskFilterOpItem::CREATE_BLUR: {
                    mf = static_cast<CreateBlurMaskFilterOpItem*>(itemPtr)->Playback();
                    break;
                }
                default: {
                    LOGE("MaskFilterCmdList unknown OpItem type!");
                    break;
                }
            }
            offset = itemPtr->GetNextOpItemOffset();
        } else {
            LOGE("MaskFilterCmdList Playback failed!");
            break;
        }
    } while (offset != 0);

    return mf;
}

/* OpItem */
CreateBlurMaskFilterOpItem::CreateBlurMaskFilterOpItem(BlurType blurType, scalar sigma)
    : MaskFilterOpItem(CREATE_BLUR), blurType_(blurType), sigma_(sigma) {}

std::shared_ptr<MaskFilter> CreateBlurMaskFilterOpItem::Playback() const
{
    return MaskFilter::CreateBlurMaskFilter(blurType_, sigma_);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
