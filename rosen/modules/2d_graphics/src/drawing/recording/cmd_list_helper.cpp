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

#include "recording/cmd_list_helper.h"

#include "recording/color_filter_cmd_list.h"
#include "recording/color_space_cmd_list.h"
#include "recording/draw_cmd_list.h"
#include "recording/image_filter_cmd_list.h"
#include "recording/mask_filter_cmd_list.h"
#include "recording/path_cmd_list.h"
#include "recording/path_effect_cmd_list.h"
#include "recording/region_cmd_list.h"
#include "recording/shader_effect_cmd_list.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
ImageHandle CmdListHelper::AddImageToCmdList(CmdList& cmdList, const Image& image)
{
    auto data = image.Serialize();
    if (data == nullptr || data->GetSize() == 0) {
        LOGE("image is valid!");
        return { 0 };
    }

    auto offset = cmdList.AddImageData(data->GetData(), data->GetSize());
    return { offset, data->GetSize() };
}

std::shared_ptr<Image> CmdListHelper::GetImageFromCmdList(const CmdList& cmdList, const ImageHandle& imageHandle)
{
    const void* ptr = cmdList.GetImageData(imageHandle.offset);
    if (ptr == nullptr) {
        LOGE("get image data failed!");
        return nullptr;
    }

    auto imageData = std::make_shared<Data>();
    imageData->BuildWithoutCopy(ptr, imageHandle.size);
    auto image = std::make_shared<Image>();
    if (image->Deserialize(imageData) == false) {
        LOGE("image deserialize failed!");
        return nullptr;
    }
    return image;
}

ImageHandle CmdListHelper::AddPictureToCmdList(CmdList& cmdList, const Picture& picture)
{
    auto data = picture.Serialize();
    if (data == nullptr || data->GetSize() == 0) {
        LOGE("picture is valid!");
        return { 0 };
    }

    auto offset = cmdList.AddImageData(data->GetData(), data->GetSize());
    return { offset, data->GetSize() };
}

std::shared_ptr<Picture> CmdListHelper::GetPictureFromCmdList(const CmdList& cmdList, const ImageHandle& pictureHandle)
{
    const void* ptr = cmdList.GetImageData(pictureHandle.offset);
    if (ptr == nullptr) {
        LOGE("get picture data failed!");
        return nullptr;
    }

    auto pictureData = std::make_shared<Data>();
    pictureData->BuildWithoutCopy(ptr, pictureHandle.size);
    auto picture = std::make_shared<Picture>();
    if (picture->Deserialize(pictureData) == false) {
        LOGE("picture deserialize failed!");
        return nullptr;
    }
    return picture;
}

CmdListHandle CmdListHelper::AddChildToCmdList(CmdList& cmdList, const std::shared_ptr<CmdList>& child)
{
    CmdListHandle childHandle = { 0 };

    if (child == nullptr) {
        LOGE("child is invalid!");
        return childHandle;
    }

    childHandle.type = child->GetType();

    auto childData = child->GetData();
    if (childData.first != nullptr && childData.second != 0) {
        childHandle.offset = cmdList.AddCmdListData(childData);
        childHandle.size = childData.second;
    } else {
        return childHandle;
    }

    auto childImageData = child->GetAllImageData();
    if (childImageData.first != nullptr && childImageData.second != 0) {
        childHandle.imageOffset = cmdList.AddImageData(childImageData.first, childImageData.second);
        childHandle.imageSize = childImageData.second;
    }

    return childHandle;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
