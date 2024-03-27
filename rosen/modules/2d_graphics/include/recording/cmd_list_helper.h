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

#include <memory>
#include <utility>

#include "image/image.h"
#include "recording/cmd_list.h"
#include "text/hm_symbol.h"
#include "text/text_blob.h"
#include "utils/log.h"
#include "utils/vertices.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class DrawOpItem;
class DRAWING_API CmdListHelper {
public:
    CmdListHelper() = default;
    ~CmdListHelper() = default;

    static OpDataHandle AddImageToCmdList(CmdList& cmdList, const Image& image);
    static OpDataHandle AddImageToCmdList(CmdList& cmdList, const std::shared_ptr<Image>& image);
    static std::shared_ptr<Image> GetImageFromCmdList(const CmdList& cmdList, const OpDataHandle& imageHandle);
    static OpDataHandle AddVerticesToCmdList(CmdList& cmdList, const Vertices& vertices);
    static std::shared_ptr<Vertices> GetVerticesFromCmdList(const CmdList& cmdList,
        const OpDataHandle& opDataHandle);
    static ImageHandle AddBitmapToCmdList(CmdList& cmdList, const Bitmap& bitmap);
    static std::shared_ptr<Bitmap> GetBitmapFromCmdList(const CmdList& cmdList, const ImageHandle& bitmapHandle);
    static OpDataHandle DRAWING_API AddImageObjectToCmdList(
        CmdList& cmdList, const std::shared_ptr<ExtendImageObject>& object);
    static std::shared_ptr<ExtendImageObject> GetImageObjectFromCmdList(
        const CmdList& cmdList, const OpDataHandle& objectHandle);
    static OpDataHandle DRAWING_API AddImageBaseObjToCmdList(
        CmdList& cmdList, const std::shared_ptr<ExtendImageBaseObj>& object);
    static std::shared_ptr<ExtendImageBaseObj> GetImageBaseObjFromCmdList(
        const CmdList& cmdList, const OpDataHandle& objectHandle);
    static OpDataHandle AddPictureToCmdList(CmdList& cmdList, const Picture& picture);
    static std::shared_ptr<Picture> GetPictureFromCmdList(const CmdList& cmdList, const OpDataHandle& pictureHandle);
    static OpDataHandle AddCompressDataToCmdList(CmdList& cmdList, const std::shared_ptr<Data>& data);
    static std::shared_ptr<Data> GetCompressDataFromCmdList(const CmdList& cmdList, const OpDataHandle& imageHandle);

    static uint32_t DRAWING_API AddDrawFuncObjToCmdList(
        CmdList& cmdList, const std::shared_ptr<ExtendDrawFuncObj>& object);
    static std::shared_ptr<ExtendDrawFuncObj> GetDrawFuncObjFromCmdList(
        const CmdList& cmdList, uint32_t objectHandle);

    template<typename RecordingType, typename CommonType>
    static CmdListHandle AddRecordedToCmdList(CmdList& cmdList, const CommonType& recorded)
    {
        if (recorded.GetDrawingType() != DrawingType::RECORDING) {
            return { 0 };
        }

        auto recording = static_cast<const RecordingType&>(recorded);
        if (recording.GetCmdList() == nullptr) {
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
            return { 0 };
        }

        auto recording = std::static_pointer_cast<RecordingType>(recorded);
        if (recording->GetCmdList() == nullptr) {
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
            return { 0 };
        }

        auto recording = static_cast<const RecordingType*>(recorded);
        if (recording->GetCmdList() == nullptr) {
            return { 0 };
        }

        return AddChildToCmdList(cmdList, recording->GetCmdList());
    }

    template<typename CmdListType, typename Type>
    static std::shared_ptr<Type> GetFromCmdList(const CmdList& cmdList, const CmdListHandle& handler)
    {
        auto childCmdList = GetChildFromCmdList<CmdListType>(cmdList, handler);
        if (childCmdList == nullptr) {
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
            return nullptr;
        }

        auto childCmdList = CmdListType::CreateFromData({ childData, childHandle.size });
        if (childCmdList == nullptr) {
            return nullptr;
        }

        if (childHandle.imageSize > 0 && cmdList.GetImageData(childHandle.imageOffset) != nullptr) {
            if (!childCmdList->SetUpImageData(cmdList.GetImageData(childHandle.imageOffset), childHandle.imageSize)) {
                LOGD("set up child image data failed!");
            }
        }

        return childCmdList;
    }

    static OpDataHandle AddTextBlobToCmdList(CmdList& cmdList, const TextBlob* textBlob, void* ctx = nullptr);
    static std::shared_ptr<TextBlob> GetTextBlobFromCmdList(const CmdList& cmdList,
        const OpDataHandle& textBlobHandle, void* ctx = nullptr);

    static OpDataHandle AddDataToCmdList(CmdList& cmdList, const Data* data);
    static std::shared_ptr<Data> GetDataFromCmdList(const CmdList& cmdList, const OpDataHandle& imageHandle);

    static OpDataHandle AddPathToCmdList(CmdList& cmdList, const Path& path);
    static std::shared_ptr<Path> GetPathFromCmdList(const CmdList& cmdList, const OpDataHandle& pathHandle);

    static OpDataHandle AddRegionToCmdList(CmdList& cmdList, const Region& region);
    static std::shared_ptr<Region> GetRegionFromCmdList(const CmdList& cmdList, const OpDataHandle& regionHandle);

    static OpDataHandle AddColorSpaceToCmdList(CmdList& cmdList, const std::shared_ptr<ColorSpace> colorSpace);
    static std::shared_ptr<ColorSpace> GetColorSpaceFromCmdList(const CmdList& cmdList,
        const OpDataHandle& imageHandle);

    static FlattenableHandle AddShaderEffectToCmdList(CmdList& cmdList, std::shared_ptr<ShaderEffect> shaderEffect);
    static std::shared_ptr<ShaderEffect> GetShaderEffectFromCmdList(const CmdList& cmdList,
        const FlattenableHandle& shaderEffectHandle);

    static FlattenableHandle AddPathEffectToCmdList(CmdList& cmdList, std::shared_ptr<PathEffect> pathEffect);
    static std::shared_ptr<PathEffect> GetPathEffectFromCmdList(const CmdList& cmdList,
        const FlattenableHandle& pathEffectHandle);

    static FlattenableHandle AddMaskFilterToCmdList(CmdList& cmdList, std::shared_ptr<MaskFilter> maskFilter);
    static std::shared_ptr<MaskFilter> GetMaskFilterFromCmdList(const CmdList& cmdList,
        const FlattenableHandle& maskFilterHandle);

    static FlattenableHandle AddColorFilterToCmdList(CmdList& cmdList, std::shared_ptr<ColorFilter> colorFilter);
    static std::shared_ptr<ColorFilter> GetColorFilterFromCmdList(const CmdList& cmdList,
        const FlattenableHandle& colorFilterHandle);

    static FlattenableHandle AddImageFilterToCmdList(CmdList& cmdList, const ImageFilter* imageFilter);
    static FlattenableHandle AddImageFilterToCmdList(CmdList& cmdList, std::shared_ptr<ImageFilter> imageFilter);
    static std::shared_ptr<ImageFilter> GetImageFilterFromCmdList(const CmdList& cmdList,
        const FlattenableHandle& imageFilterHandle);

    static SymbolOpHandle AddSymbolToCmdList(CmdList& cmdList, const DrawingHMSymbolData& symbol);
    static DrawingHMSymbolData GetSymbolFromCmdList(const CmdList& cmdList, const SymbolOpHandle& symbolHandle);

    static SymbolLayersHandle AddSymbolLayersToCmdList(CmdList& cmdList, const DrawingSymbolLayers& symbolLayers);
    static DrawingSymbolLayers GetSymbolLayersFromCmdList(const CmdList& cmdList,
        const SymbolLayersHandle& symbolLayersHandle);

    static RenderGroupHandle AddRenderGroupToCmdList(CmdList& cmdList, const DrawingRenderGroup& group);
    static DrawingRenderGroup GetRenderGroupFromCmdList(const CmdList& cmdList,
        const RenderGroupHandle& renderGroupHandle);

    static GroupInfoHandle AddGroupInfoToCmdList(CmdList& cmdList, const DrawingGroupInfo& groupInfo);
    static DrawingGroupInfo GetGroupInfoFromCmdList(const CmdList& cmdList, const GroupInfoHandle& groupInfoHandle);
#ifdef ROSEN_OHOS
    static uint32_t AddSurfaceBufferToCmdList(CmdList& cmdList, const sptr<SurfaceBuffer>& imageFilter);
    static sptr<SurfaceBuffer> GetSurfaceBufferFromCmdList(
        const CmdList& cmdList, uint32_t imageFilterHandle);
#endif
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif