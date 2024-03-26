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

#include "recording/draw_cmd_list.h"
#include "skia_adapter/skia_vertices.h"
#include "skia_adapter/skia_image_filter.h"
#include "skia_adapter/skia_mask_filter.h"
#include "skia_adapter/skia_color_filter.h"
#include "skia_adapter/skia_shader_effect.h"
#include "skia_adapter/skia_path_effect.h"

#include "utils/log.h"

#include "skia_adapter/skia_path.h"
#include "skia_adapter/skia_picture.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
static int ColorTypeToBytesPerPixel(ColorType colorType)
{
    // returns the number of bytes per pixel: 1byte, 2bytes, 4bytes
    switch (colorType) {
        case ColorType::COLORTYPE_ALPHA_8:
            return 1;
        case ColorType::COLORTYPE_RGB_565:
        case ColorType::COLORTYPE_ARGB_4444:
            return 2;
        case ColorType::COLORTYPE_RGBA_8888:
        case ColorType::COLORTYPE_BGRA_8888:
        case ColorType::COLORTYPE_N32:
            return 4;
        case ColorType::COLORTYPE_RGBA_F16:
            return 8;
        case ColorType::COLORTYPE_UNKNOWN:
        default:
            return 0;
    }
}

OpDataHandle CmdListHelper::AddImageToCmdList(CmdList& cmdList, const Image& image)
{
    return cmdList.AddImage(image);
}

OpDataHandle CmdListHelper::AddImageToCmdList(CmdList& cmdList, const std::shared_ptr<Image>& image)
{
    if (image == nullptr) {
        LOGD("image is nullptr!");
        return { 0 };
    }
    return CmdListHelper::AddImageToCmdList(cmdList, *image);
}

std::shared_ptr<Image> CmdListHelper::GetImageFromCmdList(const CmdList& cmdList, const OpDataHandle& opDataHandle)
{
    return (const_cast<CmdList&>(cmdList)).GetImage(opDataHandle);
}

OpDataHandle CmdListHelper::AddVerticesToCmdList(CmdList& cmdList, const Vertices& vertices)
{
    auto data = vertices.Serialize();
    if (data == nullptr || data->GetSize() == 0) {
        LOGD("vertices is valid!");
        return { 0 };
    }

    auto offset = cmdList.AddImageData(data->GetData(), data->GetSize());
    return { offset, data->GetSize() };
}

std::shared_ptr<Vertices> CmdListHelper::GetVerticesFromCmdList(
    const CmdList& cmdList, const OpDataHandle& opDataHandle)
{
    if (opDataHandle.size == 0) {
        return nullptr;
    }

    const void* ptr = cmdList.GetImageData(opDataHandle.offset);
    if (ptr == nullptr) {
        LOGD("get vertices data failed!");
        return nullptr;
    }

    auto verticesData = std::make_shared<Data>();
    verticesData->BuildWithoutCopy(ptr, opDataHandle.size);
    auto vertices = std::make_shared<Vertices>();
    if (vertices->Deserialize(verticesData) == false) {
        LOGD("vertices deserialize failed!");
        return nullptr;
    }
    return vertices;
}

ImageHandle CmdListHelper::AddBitmapToCmdList(CmdList& cmdList, const Bitmap& bitmap)
{
    auto format = bitmap.GetFormat();
    auto bpp = ColorTypeToBytesPerPixel(format.colorType);
    auto bitmapSize = bitmap.GetHeight() * bitmap.GetWidth() * bpp;
    if (bitmapSize == 0) {
        LOGD("bitmap is valid!");
        return { 0 };
    }

    auto offset = cmdList.AddBitmapData(bitmap.GetPixels(), bitmapSize);
    return { offset, bitmapSize, bitmap.GetWidth(), bitmap.GetHeight(), format.colorType, format.alphaType };
}

std::shared_ptr<Bitmap> CmdListHelper::GetBitmapFromCmdList(const CmdList& cmdList, const ImageHandle& bitmapHandle)
{
    if (bitmapHandle.size == 0) {
        return nullptr;
    }

    const void* ptr = cmdList.GetBitmapData(bitmapHandle.offset);
    if (ptr == nullptr) {
        LOGD("get bitmap data failed!");
        return nullptr;
    }

    BitmapFormat format = { bitmapHandle.colorType, bitmapHandle.alphaType };
    auto bitmap = std::make_shared<Bitmap>();
    bitmap->Build(bitmapHandle.width, bitmapHandle.height, format);
    bitmap->SetPixels(const_cast<void*>(cmdList.GetBitmapData(bitmapHandle.offset)));

    return bitmap;
}

OpDataHandle CmdListHelper::AddImageObjectToCmdList(CmdList& cmdList, const std::shared_ptr<ExtendImageObject>& object)
{
    auto index = cmdList.AddImageObject(object);
    return { index };
}

std::shared_ptr<ExtendImageObject> CmdListHelper::GetImageObjectFromCmdList(
    const CmdList& cmdList, const OpDataHandle& objectHandle)
{
    return (const_cast<CmdList&>(cmdList)).GetImageObject(objectHandle.offset);
}

OpDataHandle CmdListHelper::AddImageBaseObjToCmdList(
    CmdList& cmdList, const std::shared_ptr<ExtendImageBaseObj>& object)
{
    auto index = cmdList.AddImageBaseObj(object);
    return { index };
}

std::shared_ptr<ExtendImageBaseObj> CmdListHelper::GetImageBaseObjFromCmdList(
    const CmdList& cmdList, const OpDataHandle& objectHandle)
{
    return (const_cast<CmdList&>(cmdList)).GetImageBaseObj(objectHandle.offset);
}

OpDataHandle CmdListHelper::AddPictureToCmdList(CmdList& cmdList, const Picture& picture)
{
    auto data = picture.Serialize();
    if (data == nullptr || data->GetSize() == 0) {
        LOGD("picture is valid!");
        return { 0 };
    }

    auto offset = cmdList.AddImageData(data->GetData(), data->GetSize());
    return { offset, data->GetSize() };
}

std::shared_ptr<Picture> CmdListHelper::GetPictureFromCmdList(const CmdList& cmdList, const OpDataHandle& pictureHandle)
{
    if (pictureHandle.size == 0) {
        return nullptr;
    }

    const void* ptr = cmdList.GetImageData(pictureHandle.offset);
    if (ptr == nullptr) {
        LOGD("get picture data failed!");
        return nullptr;
    }

    auto pictureData = std::make_shared<Data>();
    pictureData->BuildWithoutCopy(ptr, pictureHandle.size);
    auto picture = std::make_shared<Picture>();
    if (picture->Deserialize(pictureData) == false) {
        LOGD("picture deserialize failed!");
        return nullptr;
    }
    return picture;
}

OpDataHandle CmdListHelper::AddCompressDataToCmdList(CmdList& cmdList, const std::shared_ptr<Data>& data)
{
    if (data == nullptr || data->GetSize() == 0) {
        LOGD("data is valid!");
        return { 0 };
    }

    auto offset = cmdList.AddImageData(data->GetData(), data->GetSize());
    return { offset, data->GetSize() };
}

std::shared_ptr<Data> CmdListHelper::GetCompressDataFromCmdList(const CmdList& cmdList, const OpDataHandle& imageHandle)
{
    if (imageHandle.size == 0) {
        return nullptr;
    }

    const void* ptr = cmdList.GetImageData(imageHandle.offset);
    if (ptr == nullptr) {
        LOGD("get image data failed!");
        return nullptr;
    }

    auto imageData = std::make_shared<Data>();
    imageData->BuildWithoutCopy(ptr, imageHandle.size);
    return imageData;
}

CmdListHandle CmdListHelper::AddChildToCmdList(CmdList& cmdList, const std::shared_ptr<CmdList>& child)
{
    CmdListHandle childHandle = { 0 };

    if (child == nullptr) {
        LOGD("child is invalid!");
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

    auto childBitmapData = child->GetAllBitmapData();
    if (childBitmapData.first != nullptr && childBitmapData.second != 0) {
        childHandle.bitmapOffset = cmdList.AddBitmapData(childBitmapData.first, childBitmapData.second);
        childHandle.bitmapSize = childBitmapData.second;
    }

    return childHandle;
}

SymbolOpHandle CmdListHelper::AddSymbolToCmdList(CmdList& cmdList, const DrawingHMSymbolData& symbol)
{
    auto symbolLayersHandle = AddSymbolLayersToCmdList(cmdList, symbol.symbolInfo_);
    auto pathHandle = AddPathToCmdList(cmdList, symbol.path_);
    return {symbolLayersHandle, pathHandle, symbol.symbolId};
}

DrawingHMSymbolData CmdListHelper::GetSymbolFromCmdList(const CmdList& cmdList,
    const SymbolOpHandle& symbolHandle)
{
    DrawingHMSymbolData symbol;

    symbol.symbolInfo_ = GetSymbolLayersFromCmdList(cmdList, symbolHandle.symbolLayerHandle);

    auto path = GetPathFromCmdList(cmdList, symbolHandle.pathHandle);
    if (path != nullptr) {
        symbol.path_ = *path;
    }
    symbol.symbolId = symbolHandle.symbolId;
    return symbol;
}

SymbolLayersHandle CmdListHelper::AddSymbolLayersToCmdList(CmdList& cmdList, const DrawingSymbolLayers& symbolLayers)
{
    auto layers = symbolLayers.layers;
    std::vector<std::pair<uint32_t, size_t>> handleVector1;
    for (size_t i = 0; i < layers.size(); i++) {
        handleVector1.push_back(AddVectorToCmdList(cmdList, layers.at(i)));
    }
    std::pair<uint32_t, size_t> layersHandle = AddVectorToCmdList(cmdList, handleVector1);

    auto groups = symbolLayers.renderGroups;
    std::vector<RenderGroupHandle> handleVector2;
    for (size_t i = 0; i < groups.size(); i++) {
        handleVector2.push_back(AddRenderGroupToCmdList(cmdList, groups.at(i)));
    }
    std::pair<uint32_t, size_t> groupsHandle = AddVectorToCmdList(cmdList, handleVector2);

    return { symbolLayers.symbolGlyphId, layersHandle, groupsHandle, static_cast<int32_t>(symbolLayers.effect)};
}

DrawingSymbolLayers CmdListHelper::GetSymbolLayersFromCmdList(const CmdList& cmdList,
    const SymbolLayersHandle& symbolLayersHandle)
{
    DrawingSymbolLayers symbolLayers;
    symbolLayers.symbolGlyphId = symbolLayersHandle.id;

    auto handleVector1 = GetVectorFromCmdList<std::pair<uint32_t, size_t>>(cmdList, symbolLayersHandle.layers);
    std::vector<std::vector<size_t>> layers;
    for (size_t i = 0; i < handleVector1.size(); i++) {
        layers.push_back(GetVectorFromCmdList<size_t>(cmdList, handleVector1.at(i)));
    }
    symbolLayers.layers = layers;

    auto handleVector2 = GetVectorFromCmdList<RenderGroupHandle>(cmdList, symbolLayersHandle.groups);
    std::vector<DrawingRenderGroup> renderGroups;
    for (size_t i = 0; i < handleVector2.size(); i++) {
        renderGroups.push_back(GetRenderGroupFromCmdList(cmdList, handleVector2.at(i)));
    }
    symbolLayers.renderGroups = renderGroups;

    symbolLayers.effect = static_cast<DrawingEffectStrategy>(symbolLayersHandle.effect);

    return symbolLayers;
}

RenderGroupHandle CmdListHelper::AddRenderGroupToCmdList(CmdList& cmdList, const DrawingRenderGroup& group)
{
    auto infos = group.groupInfos;
    std::vector<GroupInfoHandle> handleVector;
    for (size_t i = 0; i < infos.size(); i++) {
        handleVector.push_back(AddGroupInfoToCmdList(cmdList, infos.at(i)));
    }
    std::pair<uint32_t, size_t> groupInfosHandle = AddVectorToCmdList(cmdList, handleVector);
    return { groupInfosHandle, group.color };
}

DrawingRenderGroup CmdListHelper::GetRenderGroupFromCmdList(const CmdList& cmdList,
    const RenderGroupHandle& renderGroupHandle)
{
    DrawingRenderGroup group;
    group.color = renderGroupHandle.color;

    auto handleVector = GetVectorFromCmdList<GroupInfoHandle>(cmdList, renderGroupHandle.infos);
    std::vector<DrawingGroupInfo> groupInfos;
    for (size_t i = 0; i < handleVector.size(); i++) {
        groupInfos.push_back(GetGroupInfoFromCmdList(cmdList, handleVector.at(i)));
    }
    group.groupInfos = groupInfos;

    return group;
}

GroupInfoHandle CmdListHelper::AddGroupInfoToCmdList(CmdList& cmdList, const DrawingGroupInfo& groupInfo)
{
    std::pair<uint32_t, size_t> layerIndexes = AddVectorToCmdList(cmdList, groupInfo.layerIndexes);
    std::pair<uint32_t, size_t> maskIndexes = AddVectorToCmdList(cmdList, groupInfo.maskIndexes);
    return { layerIndexes, maskIndexes };
}

DrawingGroupInfo CmdListHelper::GetGroupInfoFromCmdList(const CmdList& cmdList, const GroupInfoHandle& groupInfoHandle)
{
    DrawingGroupInfo groupInfo;
    groupInfo.layerIndexes = GetVectorFromCmdList<size_t>(cmdList, groupInfoHandle.vec1);
    groupInfo.maskIndexes = GetVectorFromCmdList<size_t>(cmdList, groupInfoHandle.vec2);
    return groupInfo;
}

OpDataHandle CmdListHelper::AddTextBlobToCmdList(CmdList& cmdList, const TextBlob* textBlob, void* ctx)
{
    if (!textBlob) {
        return { 0 };
    }
    auto data = textBlob->Serialize(ctx);
    if (!data || data->GetSize() == 0) {
        LOGD("textBlob serialize invalid, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return { 0 };
    }

    auto offset = cmdList.AddImageData(data->GetData(), data->GetSize());
    return { offset, data->GetSize() };
}

std::shared_ptr<TextBlob> CmdListHelper::GetTextBlobFromCmdList(const CmdList& cmdList,
    const OpDataHandle& textBlobHandle, void* ctx)
{
    if (textBlobHandle.size == 0) {
        return nullptr;
    }

    const void* data = cmdList.GetImageData(textBlobHandle.offset);
    if (!data) {
        LOGD("textBlob data nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return nullptr;
    }

    auto textBlobData = std::make_shared<Data>();
    textBlobData->BuildWithoutCopy(data, textBlobHandle.size);
    return TextBlob::Deserialize(textBlobData->GetData(), textBlobData->GetSize(), ctx);
}

OpDataHandle CmdListHelper::AddDataToCmdList(CmdList& cmdList, const Data* srcData)
{
    if (!srcData) {
        LOGD("data nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return { 0 };
    }

    auto data = srcData->Serialize();
    if (data == nullptr || data->GetSize() == 0) {
        LOGD("srcData is invalid!");
        return { 0 };
    }

    auto offset = cmdList.AddImageData(data->GetData(), data->GetSize());
    return { offset, data->GetSize() };
}

std::shared_ptr<Data> CmdListHelper::GetDataFromCmdList(const CmdList& cmdList, const OpDataHandle& imageHandle)
{
    if (imageHandle.size == 0) {
        return nullptr;
    }

    const void* ptr = cmdList.GetImageData(imageHandle.offset);
    if (ptr == nullptr) {
        LOGD("get data failed!");
        return nullptr;
    }

    auto imageData = std::make_shared<Data>();
    imageData->BuildWithoutCopy(ptr, imageHandle.size);
    return imageData;
}

OpDataHandle CmdListHelper::AddPathToCmdList(CmdList& cmdList, const Path& path)
{
    auto data = path.Serialize();
    if (data == nullptr || data->GetSize() == 0) {
        LOGD("path is invalid, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return { 0 };
    }

    auto offset = cmdList.AddImageData(data->GetData(), data->GetSize());
    return { offset, data->GetSize() };
}

std::shared_ptr<Path> CmdListHelper::GetPathFromCmdList(const CmdList& cmdList,
    const OpDataHandle& pathHandle)
{
    if (pathHandle.size == 0) {
        return nullptr;
    }

    const void* ptr = cmdList.GetImageData(pathHandle.offset);
    if (ptr == nullptr) {
        LOGD("get path data failed!");
        return nullptr;
    }

    auto pathData = std::make_shared<Data>();
    pathData->BuildWithoutCopy(ptr, pathHandle.size);
    auto path = std::make_shared<Path>();
    if (path->Deserialize(pathData) == false) {
        LOGD("path deserialize failed!");
        return nullptr;
    }

    return path;
}

OpDataHandle CmdListHelper::AddRegionToCmdList(CmdList& cmdList, const Region& region)
{
    auto data = region.Serialize();
    if (data == nullptr || data->GetSize() == 0) {
        LOGD("region is invalid, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return { 0 };
    }

    auto offset = cmdList.AddImageData(data->GetData(), data->GetSize());
    return { offset, data->GetSize() };
}

std::shared_ptr<Region> CmdListHelper::GetRegionFromCmdList(const CmdList& cmdList, const OpDataHandle& regionHandle)
{
    if (regionHandle.size == 0) {
        return nullptr;
    }

    const void* ptr = cmdList.GetImageData(regionHandle.offset);
    if (ptr == nullptr) {
        LOGD("get region data failed!");
        return nullptr;
    }

    auto regionData = std::make_shared<Data>();
    regionData->BuildWithoutCopy(ptr, regionHandle.size);
    auto region = std::make_shared<Region>();
    if (region->Deserialize(regionData) == false) {
        LOGD("region deserialize failed!");
        return nullptr;
    }

    return region;
}

OpDataHandle CmdListHelper::AddColorSpaceToCmdList(CmdList& cmdList, const std::shared_ptr<ColorSpace> colorSpace)
{
    if (colorSpace == nullptr) {
        return { 0 };
    }

    auto data = colorSpace->Serialize();
    if (data == nullptr || data->GetSize() == 0) {
        LOGD("colorSpace is invalid, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return { 0 };
    }

    auto offset = cmdList.AddImageData(data->GetData(), data->GetSize());
    return { offset, data->GetSize() };
}

std::shared_ptr<ColorSpace> CmdListHelper::GetColorSpaceFromCmdList(const CmdList& cmdList,
    const OpDataHandle& imageHandle)
{
    if (imageHandle.size == 0) {
        return nullptr;
    }

    const void* ptr = cmdList.GetImageData(imageHandle.offset);
    if (ptr == nullptr) {
        return nullptr;
    }
    auto colorSpaceData = std::make_shared<Data>();
    colorSpaceData->BuildWithoutCopy(ptr, imageHandle.size);
    auto colorSpace = std::make_shared<ColorSpace>(ColorSpace::ColorSpaceType::REF_IMAGE);
    if (colorSpace->Deserialize(colorSpaceData) == false) {
        LOGD("colorSpace deserialize failed!");
        return nullptr;
    }

    return colorSpace;
}

FlattenableHandle CmdListHelper::AddShaderEffectToCmdList(CmdList& cmdList,
    std::shared_ptr<ShaderEffect> shaderEffect)
{
    if (shaderEffect == nullptr) {
        return { 0 };
    }
    ShaderEffect::ShaderEffectType type = shaderEffect->GetType();
    auto data = shaderEffect->Serialize();
    if (data == nullptr || data->GetSize() == 0) {
        LOGD("shaderEffect is invalid, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return { 0 };
    }
    auto offset = cmdList.AddImageData(data->GetData(), data->GetSize());
    return { offset, data->GetSize(), static_cast<uint32_t>(type) };
}

std::shared_ptr<ShaderEffect> CmdListHelper::GetShaderEffectFromCmdList(const CmdList& cmdList,
    const FlattenableHandle& shaderEffectHandle)
{
    if (shaderEffectHandle.size == 0) {
        return nullptr;
    }

    const void* ptr = cmdList.GetImageData(shaderEffectHandle.offset);
    if (ptr == nullptr) {
        return nullptr;
    }

    auto shaderEffectData = std::make_shared<Data>();
    shaderEffectData->BuildWithoutCopy(ptr, shaderEffectHandle.size);
    auto shaderEffect = std::make_shared<ShaderEffect>
        (static_cast<ShaderEffect::ShaderEffectType>(shaderEffectHandle.type));
    if (shaderEffect->Deserialize(shaderEffectData) == false) {
        LOGD("shaderEffect deserialize failed!");
        return nullptr;
    }

    return shaderEffect;
}

FlattenableHandle CmdListHelper::AddPathEffectToCmdList(CmdList& cmdList,
    std::shared_ptr<PathEffect> pathEffect)
{
    if (pathEffect == nullptr) {
        return { 0 };
    }
    PathEffect::PathEffectType type = pathEffect->GetType();
    auto data = pathEffect->Serialize();
    if (data == nullptr || data->GetSize() == 0) {
        LOGD("pathEffect is invalid, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return { 0 };
    }
    auto offset = cmdList.AddImageData(data->GetData(), data->GetSize());
    return { offset, data->GetSize(), static_cast<uint32_t>(type) };
}

std::shared_ptr<PathEffect> CmdListHelper::GetPathEffectFromCmdList(const CmdList& cmdList,
    const FlattenableHandle& pathEffectHandle)
{
    if (pathEffectHandle.size == 0) {
        return nullptr;
    }

    const void* ptr = cmdList.GetImageData(pathEffectHandle.offset);
    if (ptr == nullptr) {
        return nullptr;
    }

    auto pathEffectData = std::make_shared<Data>();
    pathEffectData->BuildWithoutCopy(ptr, pathEffectHandle.size);
    auto pathEffect = std::make_shared<PathEffect>
        (static_cast<PathEffect::PathEffectType>(pathEffectHandle.type));
    if (pathEffect->Deserialize(pathEffectData) == false) {
        LOGD("pathEffect deserialize failed!");
        return nullptr;
    }

    return pathEffect;
}

FlattenableHandle CmdListHelper::AddMaskFilterToCmdList(CmdList& cmdList,
    std::shared_ptr<MaskFilter> maskFilter)
{
    if (maskFilter == nullptr) {
        return { 0 };
    }
    MaskFilter::FilterType type = maskFilter->GetType();
    auto data = maskFilter->Serialize();
    if (data == nullptr || data->GetSize() == 0) {
        return { 0 };
    }
    auto offset = cmdList.AddImageData(data->GetData(), data->GetSize());
    return { offset, data->GetSize(), static_cast<uint32_t>(type) };
}

std::shared_ptr<MaskFilter> CmdListHelper::GetMaskFilterFromCmdList(const CmdList& cmdList,
    const FlattenableHandle& maskFilterHandle)
{
    if (maskFilterHandle.size == 0) {
        return nullptr;
    }

    const void* ptr = cmdList.GetImageData(maskFilterHandle.offset);
    if (ptr == nullptr) {
        return nullptr;
    }

    auto maskFilterData = std::make_shared<Data>();
    maskFilterData->BuildWithoutCopy(ptr, maskFilterHandle.size);
    auto maskFilter = std::make_shared<MaskFilter>
        (static_cast<MaskFilter::FilterType>(maskFilterHandle.type));
    if (maskFilter->Deserialize(maskFilterData) == false) {
        LOGD("maskFilter deserialize failed!");
        return nullptr;
    }

    return maskFilter;
}

FlattenableHandle CmdListHelper::AddColorFilterToCmdList(CmdList& cmdList,
    std::shared_ptr<ColorFilter> colorFilter)
{
    if (colorFilter == nullptr) {
        return { 0 };
    }
    ColorFilter::FilterType type = colorFilter->GetType();
    auto data = colorFilter->Serialize();
    if (data == nullptr || data->GetSize() == 0) {
        LOGD("colorFilter is invalid, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return { 0 };
    }
    auto offset = cmdList.AddImageData(data->GetData(), data->GetSize());
    return { offset, data->GetSize(), static_cast<uint32_t>(type) };
}

std::shared_ptr<ColorFilter> CmdListHelper::GetColorFilterFromCmdList(const CmdList& cmdList,
    const FlattenableHandle& colorFilterHandle)
{
    if (colorFilterHandle.size == 0) {
        return nullptr;
    }

    const void* ptr = cmdList.GetImageData(colorFilterHandle.offset);
    if (ptr == nullptr) {
        return nullptr;
    }

    auto colorFilterData = std::make_shared<Data>();
    colorFilterData->BuildWithoutCopy(ptr, colorFilterHandle.size);
    auto colorFilter = std::make_shared<ColorFilter>
        (static_cast<ColorFilter::FilterType>(colorFilterHandle.type));
    if (colorFilter->Deserialize(colorFilterData) == false) {
        LOGD("colorFilter deserialize failed!");
        return nullptr;
    }

    return colorFilter;
}

FlattenableHandle CmdListHelper::AddImageFilterToCmdList(CmdList& cmdList, const ImageFilter* imageFilter)
{
    if (imageFilter == nullptr) {
        return { 0 };
    }
    ImageFilter::FilterType type = imageFilter->GetType();
    auto data = imageFilter->Serialize();
    if (data == nullptr || data->GetSize() == 0) {
        return { 0 };
    }
    auto offset = cmdList.AddImageData(data->GetData(), data->GetSize());
    return { offset, data->GetSize(), static_cast<uint32_t>(type) };
}

FlattenableHandle CmdListHelper::AddImageFilterToCmdList(CmdList& cmdList,
    std::shared_ptr<ImageFilter> imageFilter)
{
    return AddImageFilterToCmdList(cmdList, imageFilter.get());
}

std::shared_ptr<ImageFilter> CmdListHelper::GetImageFilterFromCmdList(const CmdList& cmdList,
    const FlattenableHandle& imageFilterHandle)
{
    if (imageFilterHandle.size == 0) {
        return nullptr;
    }

    const void* ptr = cmdList.GetImageData(imageFilterHandle.offset);
    if (ptr == nullptr) {
        return nullptr;
    }

    auto imageFilterData = std::make_shared<Data>();
    imageFilterData->BuildWithoutCopy(ptr, imageFilterHandle.size);
    auto imageFilter = std::make_shared<ImageFilter>
        (static_cast<ImageFilter::FilterType>(imageFilterHandle.type));
    if (imageFilter->Deserialize(imageFilterData) == false) {
        LOGD("imageFilter deserialize failed!");
        return nullptr;
    }

    return imageFilter;
}
#ifdef ROSEN_OHOS
uint32_t CmdListHelper::AddSurfaceBufferToCmdList(CmdList& cmdList, const sptr<SurfaceBuffer>& surfaceBuffer)
{
    return cmdList.AddSurfaceBuffer(surfaceBuffer);
}

sptr<SurfaceBuffer> CmdListHelper::GetSurfaceBufferFromCmdList(
    const CmdList& cmdList, uint32_t surfaceBufferHandle)
{
    return (const_cast<CmdList&>(cmdList)).GetSurfaceBuffer(surfaceBufferHandle);
}
#endif

uint32_t CmdListHelper::AddDrawFuncObjToCmdList(CmdList &cmdList, const std::shared_ptr<ExtendDrawFuncObj> &object)
{
    return cmdList.AddDrawFuncOjb(object);
}

std::shared_ptr<ExtendDrawFuncObj> CmdListHelper::GetDrawFuncObjFromCmdList(
    const CmdList& cmdList, uint32_t objectHandle)
{
    return (const_cast<CmdList&>(cmdList)).GetDrawFuncObj(objectHandle);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
