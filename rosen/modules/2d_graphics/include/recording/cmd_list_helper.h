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

#ifndef CMD_LIST_HELPER_H
#define CMD_LIST_HELPER_H

#include <utility>
#include <memory>

#include "image/image.h"
#include "recording/cmd_list.h"
#include "utils/log.h"

namespace OHOS {
namespace Media {
class PixelMap;
}
namespace Rosen {
namespace Drawing {
class CmdListHelper {
public:
    CmdListHelper() = default;
    ~CmdListHelper() = default;

    static ImageHandle AddImageToCmdList(CmdList& cmdList, const Image& image);
    static ImageHandle AddImageToCmdList(CmdList& cmdList, const std::shared_ptr<Image>& image);
    static std::shared_ptr<Image> GetImageFromCmdList(const CmdList& cmdList, const ImageHandle& imageHandle);
    static ImageHandle AddBitmapToCmdList(CmdList& cmdList, const Bitmap& bitmap);
    static std::shared_ptr<Bitmap> GetBitmapFromCmdList(const CmdList& cmdList, const ImageHandle& bitmapHandle);
    static ImageHandle AddPixelMapToCmdList(CmdList& cmdList, const std::shared_ptr<Media::PixelMap>& pixelMap);
    static std::shared_ptr<Media::PixelMap> GetPixelMapFromCmdList(
        const CmdList& cmdList, const ImageHandle& pixelMapHandle);
    static ImageHandle AddPictureToCmdList(CmdList& cmdList, const Picture& picture);
    static std::shared_ptr<Picture> GetPictureFromCmdList(const CmdList& cmdList, const ImageHandle& pictureHandle);
    static ImageHandle AddCompressDataToCmdList(CmdList& cmdList, const std::shared_ptr<Data>& data);
    static std::shared_ptr<Data> GetCompressDataFromCmdList(const CmdList& cmdList, const ImageHandle& imageHandle);

    template<typename RecordingType, typename CommonType>
    static CmdListHandle AddRecordedToCmdList(CmdList& cmdList, const CommonType& recorded)
    {
        if (recorded.GetDrawingType() != DrawingType::RECORDING) {
            LOGE("recorded is invalid!");
            return { 0 };
        }

        auto recording = static_cast<const RecordingType&>(recorded);
        if (recording.GetCmdList() == nullptr) {
            LOGE("recorded cmdlist is invalid!");
            return { 0 };
        }

        return AddChildToCmdList(cmdList, recording.GetCmdList());
    }

    template<typename RecordingType, typename CommonType>
    static CmdListHandle AddRecordedToCmdList(CmdList& cmdList, const std::shared_ptr<CommonType>& recorded)
    {
        if (recorded == nullptr) {
            return { 0 };
        }
        if (recorded->GetDrawingType() != DrawingType::RECORDING) {
            LOGE("recorded is invalid!");
            return { 0 };
        }

        auto recording = std::static_pointer_cast<RecordingType>(recorded);
        if (recording->GetCmdList() == nullptr) {
            LOGE("recorded cmdlist is invalid!");
            return { 0 };
        }

        return AddChildToCmdList(cmdList, recording->GetCmdList());
    }

    template<typename RecordingType, typename CommonType>
    static CmdListHandle AddRecordedToCmdList(CmdList& cmdList, const CommonType* recorded)
    {
        if (recorded == nullptr) {
            return { 0 };
        }
        if (recorded->GetDrawingType() != DrawingType::RECORDING) {
            LOGE("recorded is invalid!");
            return { 0 };
        }

        auto recording = static_cast<const RecordingType*>(recorded);
        if (recording->GetCmdList() == nullptr) {
            LOGE("recorded cmdlist is invalid!");
            return { 0 };
        }

        return AddChildToCmdList(cmdList, recording->GetCmdList());
    }

    template<typename CmdListType, typename Type>
    static std::shared_ptr<Type> GetFromCmdList(const CmdList& cmdList, const CmdListHandle& handler)
    {
        auto childCmdList = GetChildFromCmdList<CmdListType>(cmdList, handler);
        if (childCmdList == nullptr) {
            LOGE("child cmdlist is invalid!");
            return nullptr;
        }

        return childCmdList->Playback();
    }

    template<typename Type>
    static std::pair<uint32_t, size_t> AddVectorToCmdList(CmdList& cmdList, const std::vector<Type>& vec)
    {
        std::pair<uint32_t, size_t> ret(0, 0);
        if (!vec.empty()) {
            const void* data = static_cast<const void*>(vec.data());
            size_t size = vec.size() * sizeof(Type);
            auto offset = cmdList.AddCmdListData(std::make_pair(data, size));
            ret = { offset, size };
        }

        return ret;
    }

    template<typename Type>
    static std::vector<Type> GetVectorFromCmdList(const CmdList& cmdList, std::pair<uint32_t, size_t> info)
    {
        std::vector<Type> ret;
        const auto* values = static_cast<const Type*>(cmdList.GetCmdListData(info.first));
        auto size = info.second / sizeof(Type);
        if (values != nullptr && size > 0) {
            for (size_t i = 0; i < size; i++) {
                ret.push_back(*values);
                values++;
            }
        }
        return ret;
    }

    static CmdListHandle AddChildToCmdList(CmdList& cmdList, const std::shared_ptr<CmdList>& child);

    template<typename CmdListType>
    static std::shared_ptr<CmdListType> GetChildFromCmdList(const CmdList& cmdList, const CmdListHandle& childHandle)
    {
        if (childHandle.size == 0) {
            return std::make_shared<CmdListType>();
        }

        const void* childData = cmdList.GetCmdListData(childHandle.offset);
        if (childData == nullptr) {
            LOGE("child offset is invalid!");
            return nullptr;
        }

        auto childCmdList = CmdListType::CreateFromData({ childData, childHandle.size });
        if (childCmdList == nullptr) {
            LOGE("create child CmdList failed!");
            return nullptr;
        }

        if (childHandle.imageSize > 0 && cmdList.GetImageData(childHandle.imageOffset) != nullptr) {
            if (!childCmdList->SetUpImageData(cmdList.GetImageData(childHandle.imageOffset), childHandle.imageSize)) {
                LOGE("set up child image data failed!");
            }
        }

        return childCmdList;
    }
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif