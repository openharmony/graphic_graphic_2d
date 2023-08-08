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

#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
std::shared_ptr<MaskFilterCmdList> MaskFilterCmdList::CreateFromData(const CmdListData& data, bool isCopy)
{
    auto cmdList = std::make_shared<MaskFilterCmdList>();
    if (isCopy) {
        cmdList->opAllocator_.BuildFromDataWithCopy(data.first, data.second);
    } else {
        cmdList->opAllocator_.BuildFromData(data.first, data.second);
    }
    return cmdList;
}

std::shared_ptr<MaskFilter> MaskFilterCmdList::Playback() const
{
    if (opAllocator_.GetSize() == 0) {
        return nullptr;
    }

    uint32_t offset = 0;
    std::shared_ptr<MaskFilter> mf = nullptr;
    do {
        OpItem* itemPtr = static_cast<OpItem*>(opAllocator_.OffsetToAddr(offset));
        if (itemPtr == nullptr) {
            LOGE("MaskFilterCmdList Playback failed!");
            break;
        }

        switch (itemPtr->GetType()) {
            case MaskFilterOpItem::OPITEM_HEAD:
                break;
            case MaskFilterOpItem::CREATE_BLUR:
                mf = static_cast<CreateBlurMaskFilterOpItem*>(itemPtr)->Playback();
                break;
            default:
                LOGE("MaskFilterCmdList unknown OpItem type!");
                break;
        }
        offset = itemPtr->GetNextOpItemOffset();
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
