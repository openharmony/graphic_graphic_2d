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

#include "recording/color_space_cmd_list.h"

#include "image/image.h"
#include "recording/cmd_list_helper.h"
#include "utils/data.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
std::shared_ptr<ColorSpaceCmdList> ColorSpaceCmdList::CreateFromData(const CmdListData& data)
{
    auto cmdList = std::make_shared<ColorSpaceCmdList>();
    cmdList->opAllocator_.BuildFromData(data.first, data.second);
    return cmdList;
}

std::shared_ptr<ColorSpace> ColorSpaceCmdList::Playback() const
{
    int32_t offset = 0;
    std::shared_ptr<ColorSpace> cs = nullptr;

    do {
        OpItem* itemPtr = static_cast<OpItem*>(opAllocator_.OffsetToAddr(offset));
        if (itemPtr == nullptr) {
            LOGE("ColorSpaceCmdList Playback failed!");
            break;
        }

        switch (itemPtr->GetType()) {
            case ColorSpaceOpItem::CREATE_SRGB:
                cs = static_cast<CreateSRGBOpItem*>(itemPtr)->Playback();
                break;
            case ColorSpaceOpItem::CREATE_SRGB_LINEAR:
                cs = static_cast<CreateSRGBLinearOpItem*>(itemPtr)->Playback();
                break;
            case ColorSpaceOpItem::CREATE_REF_IMAGE:
                cs = static_cast<CreateRefImageOpItem*>(itemPtr)->Playback(*this);
                break;
            case ColorSpaceOpItem::CREATE_RGB:
                cs = static_cast<CreateRGBOpItem*>(itemPtr)->Playback();
                break;
            default:
                LOGE("ColorSpaceCmdList unknown OpItem type!");
                break;
        }
        offset = itemPtr->GetNextOpItemOffset();
    } while (offset != 0);

    return cs;
}

/* OpItem */
CreateSRGBOpItem::CreateSRGBOpItem() : ColorSpaceOpItem(CREATE_SRGB) {}

std::shared_ptr<ColorSpace> CreateSRGBOpItem::Playback() const
{
    return ColorSpace::CreateSRGB();
}

CreateSRGBLinearOpItem::CreateSRGBLinearOpItem() : ColorSpaceOpItem(CREATE_SRGB_LINEAR) {}

std::shared_ptr<ColorSpace> CreateSRGBLinearOpItem::Playback() const
{
    return ColorSpace::CreateSRGBLinear();
}

CreateRefImageOpItem::CreateRefImageOpItem(const ImageHandle& image)
    : ColorSpaceOpItem(CREATE_REF_IMAGE), image_(image) {}

std::shared_ptr<ColorSpace> CreateRefImageOpItem::Playback(const CmdList& cmdList) const
{
    auto image = CmdListHelper::GetImageFromCmdList(cmdList, image_);
    if (image == nullptr) {
        return nullptr;
    }

    return ColorSpace::CreateRefImage(*image);
}

CreateRGBOpItem::CreateRGBOpItem(const CMSTransferFuncType& func, const CMSMatrixType& matrix)
    : ColorSpaceOpItem(CREATE_RGB), func_(func), matrix_(matrix) {}

std::shared_ptr<ColorSpace> CreateRGBOpItem::Playback() const
{
    return ColorSpace::CreateRGB(func_, matrix_);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
