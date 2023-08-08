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

#include "recording/cmd_list_helper.h"
#include "recording/color_filter_cmd_list.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
std::shared_ptr<ImageFilterCmdList> ImageFilterCmdList::CreateFromData(const CmdListData& data, bool isCopy)
{
    auto cmdList = std::make_shared<ImageFilterCmdList>();
    if (isCopy) {
        cmdList->opAllocator_.BuildFromDataWithCopy(data.first, data.second);
    } else {
        cmdList->opAllocator_.BuildFromData(data.first, data.second);
    }
    return cmdList;
}

std::shared_ptr<ImageFilter> ImageFilterCmdList::Playback() const
{
    if (opAllocator_.GetSize() == 0) {
        return nullptr;
    }

    uint32_t offset = 0;
    std::shared_ptr<ImageFilter> imageFilter = nullptr;

    do {
        OpItem* itemPtr = static_cast<OpItem*>(opAllocator_.OffsetToAddr(offset));
        if (itemPtr == nullptr) {
            LOGE("ImageFilterCmdList Playback failed!");
            break;
        }

        switch (itemPtr->GetType()) {
            case ImageFilterOpItem::OPITEM_HEAD:
                break;
            case ImageFilterOpItem::CREATE_BLUR:
                imageFilter = static_cast<CreateBlurImageFilterOpItem*>(itemPtr)->Playback(*this);
                break;
            case ImageFilterOpItem::CREATE_COLOR_FILTER:
                imageFilter = static_cast<CreateColorFilterImageFilterOpItem*>(itemPtr)->Playback(*this);
                break;
            case ImageFilterOpItem::CREATE_OFFSET:
                imageFilter = static_cast<CreateOffsetImageFilterOpItem*>(itemPtr)->Playback(*this);
                break;
            case ImageFilterOpItem::CREATE_ARITHMETIC:
                imageFilter = static_cast<CreateArithmeticImageFilterOpItem*>(itemPtr)->Playback(*this);
                break;
            case ImageFilterOpItem::CREATE_COMPOSE:
                imageFilter = static_cast<CreateComposeImageFilterOpItem*>(itemPtr)->Playback(*this);
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
    scalar sigmaX, scalar sigmaY, TileMode mode, const CmdListHandle& input)
    : ImageFilterOpItem(CREATE_BLUR), sigmaX_(sigmaX), sigmaY_(sigmaY), mode_(mode), input_(input) {}

std::shared_ptr<ImageFilter> CreateBlurImageFilterOpItem::Playback(const CmdList& cmdList) const
{
    auto input = CmdListHelper::GetFromCmdList<ImageFilterCmdList, ImageFilter>(cmdList, input_);
    return ImageFilter::CreateBlurImageFilter(sigmaX_, sigmaY_, mode_, input);
}

CreateColorFilterImageFilterOpItem::CreateColorFilterImageFilterOpItem(const CmdListHandle& cf,
    const CmdListHandle& input) : ImageFilterOpItem(CREATE_COLOR_FILTER), cf_(cf), input_(input) {}

std::shared_ptr<ImageFilter> CreateColorFilterImageFilterOpItem::Playback(const CmdList& cmdList) const
{
    auto colorFilter = CmdListHelper::GetFromCmdList<ColorFilterCmdList, ColorFilter>(cmdList, cf_);
    if (colorFilter == nullptr) {
        LOGE("color filter invalid!");
        return nullptr;
    }

    auto input = CmdListHelper::GetFromCmdList<ImageFilterCmdList, ImageFilter>(cmdList, input_);
    return ImageFilter::CreateColorFilterImageFilter(*colorFilter, input);
}

CreateOffsetImageFilterOpItem::CreateOffsetImageFilterOpItem(scalar dx, scalar dy, const CmdListHandle& input)
    : ImageFilterOpItem(CREATE_OFFSET), dx_(dx), dy_(dy), input_(input) {}

std::shared_ptr<ImageFilter> CreateOffsetImageFilterOpItem::Playback(const CmdList& cmdList) const
{
    auto input = CmdListHelper::GetFromCmdList<ImageFilterCmdList, ImageFilter>(cmdList, input_);
    return ImageFilter::CreateOffsetImageFilter(dx_, dy_, input);
}

CreateArithmeticImageFilterOpItem::CreateArithmeticImageFilterOpItem(const std::pair<uint32_t, size_t>& coefficients,
    bool enforcePMColor, const CmdListHandle& background, const CmdListHandle& foreground)
    : ImageFilterOpItem(CREATE_ARITHMETIC), coefficients_(coefficients),
    enforcePMColor_(enforcePMColor), background_(background), foreground_(foreground) {}

std::shared_ptr<ImageFilter> CreateArithmeticImageFilterOpItem::Playback(const CmdList& cmdList) const
{
    auto background = CmdListHelper::GetFromCmdList<ImageFilterCmdList, ImageFilter>(cmdList, background_);
    auto foreground = CmdListHelper::GetFromCmdList<ImageFilterCmdList, ImageFilter>(cmdList, foreground_);

    auto coefficients = CmdListHelper::GetVectorFromCmdList<scalar>(cmdList, coefficients_);

    return ImageFilter::CreateArithmeticImageFilter(coefficients, enforcePMColor_, background, foreground);
}

CreateComposeImageFilterOpItem::CreateComposeImageFilterOpItem(const CmdListHandle& f1, const CmdListHandle& f2)
    : ImageFilterOpItem(CREATE_COMPOSE), f1_(f1), f2_(f2) {}

std::shared_ptr<ImageFilter> CreateComposeImageFilterOpItem::Playback(const CmdList& cmdList) const
{
    auto f1 = CmdListHelper::GetFromCmdList<ImageFilterCmdList, ImageFilter>(cmdList, f1_);
    auto f2 = CmdListHelper::GetFromCmdList<ImageFilterCmdList, ImageFilter>(cmdList, f2_);

    return ImageFilter::CreateComposeImageFilter(f1, f2);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
