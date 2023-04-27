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

#include "recording/image_filter_cmd_list.h"

#include "recording/color_filter_cmd_list.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
std::shared_ptr<ImageFilterCmdList> ImageFilterCmdList::CreateFromData(const CmdListData& data)
{
    auto cmdList = std::make_shared<ImageFilterCmdList>();
    cmdList->opAllocator_.BuildFromData(data.first, data.second);
    return cmdList;
}

std::shared_ptr<ImageFilter> ImageFilterCmdList::Playback() const
{
    int offset = 0;
    std::shared_ptr<ImageFilter> imageFilter = nullptr;

    do {
        OpItem* itemPtr = static_cast<OpItem*>(opAllocator_.OffsetToAddr(offset));
        if (itemPtr == nullptr) {
            LOGE("ImageFilterCmdList Playback failed!");
            break;
        }

        switch (itemPtr->GetType()) {
            case ImageFilterOpItem::CREATE_BLUR:
                imageFilter = static_cast<CreateBlurImageFilterOpItem*>(itemPtr)->Playback(opAllocator_);
                break;
            case ImageFilterOpItem::CREATE_COLOR_FILTER:
                imageFilter = static_cast<CreateColorFilterImageFilterOpItem*>(itemPtr)->Playback(opAllocator_);
                break;
            case ImageFilterOpItem::CREATE_OFFSET:
                imageFilter = static_cast<CreateOffsetImageFilterOpItem*>(itemPtr)->Playback(opAllocator_);
                break;
            case ImageFilterOpItem::CREATE_ARITHMETIC:
                imageFilter = static_cast<CreateArithmeticImageFilterOpItem*>(itemPtr)->Playback(opAllocator_);
                break;
            case ImageFilterOpItem::CREATE_COMPOSE:
                imageFilter = static_cast<CreateComposeImageFilterOpItem*>(itemPtr)->Playback(opAllocator_);
                break;
            default:
                LOGE("ImageFilterCmdList unknown OpItem type!");
                break;
        }
        offset = itemPtr->GetNextOpItemOffset();
    } while (offset != 0);

    return imageFilter;
}

/* OpItem */
CreateBlurImageFilterOpItem::CreateBlurImageFilterOpItem(
    scalar sigmaX, scalar sigmaY, TileMode mode, const CmdListSiteInfo& input)
    : ImageFilterOpItem(CREATE_BLUR), sigmaX_(sigmaX), sigmaY_(sigmaY), mode_(mode), input_(input) {}

std::shared_ptr<ImageFilter> CreateBlurImageFilterOpItem::Playback(const MemAllocator& allocator) const
{
    std::shared_ptr<ImageFilter> input = nullptr;
    if (input_.second != 0) {
        void* ptr = allocator.OffsetToAddr(input_.first);
        auto cmdList = ImageFilterCmdList::CreateFromData({ ptr, input_.second });
        if (cmdList != nullptr) {
            input = cmdList->Playback();
        }
    }

    return ImageFilter::CreateBlurImageFilter(sigmaX_, sigmaY_, mode_, input);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
