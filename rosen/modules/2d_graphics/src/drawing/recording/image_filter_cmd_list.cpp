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
static std::shared_ptr<ImageFilter> GetImageFilter(const MemAllocator& allocator, const CmdListSiteInfo& info)
{
    void* ptr = allocator.OffsetToAddr(info.first);
    if (info.second == 0 || ptr == nullptr) {
        return nullptr;
    }

    auto cmdList = ImageFilterCmdList::CreateFromData({ ptr, info.second });
    if (cmdList == nullptr) {
        return nullptr;
    }
    std::shared_ptr<ImageFilter> imageFilter = nullptr;
    imageFilter = cmdList->Playback();
    return imageFilter;
}

CreateBlurImageFilterOpItem::CreateBlurImageFilterOpItem(
    scalar sigmaX, scalar sigmaY, TileMode mode, const CmdListSiteInfo& input)
    : ImageFilterOpItem(CREATE_BLUR), sigmaX_(sigmaX), sigmaY_(sigmaY), mode_(mode), input_(input) {}

std::shared_ptr<ImageFilter> CreateBlurImageFilterOpItem::Playback(const MemAllocator& allocator) const
{
    auto input = GetImageFilter(allocator, input_);
    return ImageFilter::CreateBlurImageFilter(sigmaX_, sigmaY_, mode_, input);
}

CreateColorFilterImageFilterOpItem::CreateColorFilterImageFilterOpItem(const CmdListSiteInfo& cf,
    const CmdListSiteInfo& input) : ImageFilterOpItem(CREATE_COLOR_FILTER), cf_(cf), input_(input) {}

std::shared_ptr<ImageFilter> CreateColorFilterImageFilterOpItem::Playback(const MemAllocator& allocator) const
{
    void* cfData = allocator.OffsetToAddr(cf_.first);
    if (cfData == nullptr) {
        LOGE("color filter data invalid!");
        return nullptr;
    }
    auto cfCmdList = ColorFilterCmdList::CreateFromData({ cfData, cf_.second });
    if (cfCmdList == nullptr) {
        LOGE("color filter cmd list invalid!");
        return nullptr;
    }
    auto colorFilter = cfCmdList->Playback();
    if (colorFilter == nullptr) {
        LOGE("color filter invalid!");
        return nullptr;
    }

    auto input = GetImageFilter(allocator, input_);
    return ImageFilter::CreateColorFilterImageFilter(*colorFilter, input);
}

CreateOffsetImageFilterOpItem::CreateOffsetImageFilterOpItem(scalar dx, scalar dy, const CmdListSiteInfo& input)
    : ImageFilterOpItem(CREATE_OFFSET), dx_(dx), dy_(dy), input_(input) {}

std::shared_ptr<ImageFilter> CreateOffsetImageFilterOpItem::Playback(const MemAllocator& allocator) const
{
    auto input = GetImageFilter(allocator, input_);
    return ImageFilter::CreateOffsetImageFilter(dx_, dy_, input);
}

CreateArithmeticImageFilterOpItem::CreateArithmeticImageFilterOpItem(std::pair<Offset_t, size_t> coefficients,
    bool enforcePMColor, const CmdListSiteInfo& background, const CmdListSiteInfo& foreground)
    : ImageFilterOpItem(CREATE_ARITHMETIC), coefficients_(coefficients),
    enforcePMColor_(enforcePMColor), background_(background), foreground_(foreground) {}

std::shared_ptr<ImageFilter> CreateArithmeticImageFilterOpItem::Playback(const MemAllocator& allocator) const
{
    auto background = GetImageFilter(allocator, background_);
    auto foreground = GetImageFilter(allocator, foreground_);

    std::vector<scalar> coefficients;
    if (coefficients_.second > 0 && allocator.OffsetToAddr(coefficients_.first) != nullptr) {
        auto* ptr = static_cast<scalar*>(allocator.OffsetToAddr(coefficients_.first));
        auto num = coefficients_.second / sizeof(scalar);
        for (size_t i = 0; i < num; i++) {
            coefficients.push_back(ptr[i]);
        }
    }

    return ImageFilter::CreateArithmeticImageFilter(coefficients, enforcePMColor_, background, foreground);
}

CreateComposeImageFilterOpItem::CreateComposeImageFilterOpItem(const CmdListSiteInfo& f1, const CmdListSiteInfo& f2)
    : ImageFilterOpItem(CREATE_COMPOSE), f1_(f1), f2_(f2) {}

std::shared_ptr<ImageFilter> CreateComposeImageFilterOpItem::Playback(const MemAllocator& allocator) const
{
    auto f1 = GetImageFilter(allocator, f1_);
    auto f2 = GetImageFilter(allocator, f2_);
    return ImageFilter::CreateComposeImageFilter(f1, f2);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
