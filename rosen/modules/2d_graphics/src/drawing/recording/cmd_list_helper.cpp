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

#ifdef SUPPORT_OHOS_PIXMAP
#include "pixel_map.h"
#endif
#include "recording/color_filter_cmd_list.h"
#include "recording/color_space_cmd_list.h"
#include "recording/draw_cmd_list.h"
#include "recording/image_filter_cmd_list.h"
#include "recording/mask_filter_cmd_list.h"
#include "recording/path_cmd_list.h"
#include "recording/path_effect_cmd_list.h"
#include "recording/region_cmd_list.h"
#include "recording/shader_effect_cmd_list.h"
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
        LOGE("image is nullptr!");
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
        LOGE("vertices is valid!");
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
        LOGE("get vertices data failed!");
        return nullptr;
    }

    auto verticesData = std::make_shared<Data>();
    verticesData->BuildWithoutCopy(ptr, opDataHandle.size);
    auto vertices = std::make_shared<Vertices>();
    if (vertices->Deserialize(verticesData) == false) {
        LOGE("vertices deserialize failed!");
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
        LOGE("bitmap is valid!");
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
        LOGE("get bitmap data failed!");
        return nullptr;
    }

    BitmapFormat format = { bitmapHandle.colorType, bitmapHandle.alphaType };
    auto bitmap = std::make_shared<Bitmap>();
    bitmap->Build(bitmapHandle.width, bitmapHandle.height, format);
    bitmap->SetPixels(const_cast<void*>(cmdList.GetBitmapData(bitmapHandle.offset)));

    return bitmap;
}

OpDataHandle CmdListHelper::AddPixelMapToCmdList(CmdList& cmdList, const std::shared_ptr<Media::PixelMap>& pixelMap)
{
#ifdef SUPPORT_OHOS_PIXMAP
    auto index = cmdList.AddPixelMap(pixelMap);
    return { index };
#else
    LOGE("Not support drawing Media::PixelMap");
    return { 0 };
#endif
}

std::shared_ptr<Media::PixelMap> CmdListHelper::GetPixelMapFromCmdList(
    const CmdList& cmdList, const OpDataHandle& pixelMapHandle)
{
#ifdef SUPPORT_OHOS_PIXMAP
    return (const_cast<CmdList&>(cmdList)).GetPixelMap(pixelMapHandle.offset);
#else
    LOGE("Not support drawing Media::PixelMap");
    return nullptr;
#endif
}

OpDataHandle CmdListHelper::AddImageObjectToCmdList(CmdList& cmdList, const std::shared_ptr<ExtendImageObject>& object)
{
#ifdef SUPPORT_OHOS_PIXMAP
    auto index = cmdList.AddImageObject(object);
    return { index };
#else
    LOGE("Not support drawing ExtendImageObject");
    return { 0 };
#endif
}

std::shared_ptr<ExtendImageObject> CmdListHelper::GetImageObjectFromCmdList(
    const CmdList& cmdList, const OpDataHandle& objectHandle)
{
#ifdef SUPPORT_OHOS_PIXMAP
    return (const_cast<CmdList&>(cmdList)).GetImageObject(objectHandle.offset);
#else
    LOGE("Not support drawing ExtendImageObject");
    return nullptr;
#endif
}

OpDataHandle CmdListHelper::AddImageBaseOjToCmdList(CmdList& cmdList, const std::shared_ptr<ExtendImageBaseOj>& object)
{
    auto index = cmdList.AddImageBaseOj(object);
    return { index };
}

std::shared_ptr<ExtendImageBaseOj> CmdListHelper::GetImageBaseOjFromCmdList(
    const CmdList& cmdList, const OpDataHandle& objectHandle)
{
    return (const_cast<CmdList&>(cmdList)).GetImageBaseOj(objectHandle.offset);
}

OpDataHandle CmdListHelper::AddPictureToCmdList(CmdList& cmdList, const Picture& picture)
{
    auto data = picture.Serialize();
    if (data == nullptr || data->GetSize() == 0) {
        LOGE("picture is valid!");
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

OpDataHandle CmdListHelper::AddCompressDataToCmdList(CmdList& cmdList, const std::shared_ptr<Data>& data)
{
    if (data == nullptr || data->GetSize() == 0) {
        LOGE("data is valid!");
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
        LOGE("get image data failed!");
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

    auto childBitmapData = child->GetAllBitmapData();
    if (childBitmapData.first != nullptr && childBitmapData.second != 0) {
        childHandle.bitmapOffset = cmdList.AddBitmapData(childBitmapData.first, childBitmapData.second);
        childHandle.bitmapSize = childBitmapData.second;
    }

    return childHandle;
}

OpDataHandle CmdListHelper::AddTextBlobToCmdList(CmdList& cmdList, const TextBlob* textBlob)
{
    if (!textBlob) {
        LOGE("textBlob nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return { 0 };
    }
    auto data = textBlob->Serialize();
    if (!data || data->GetSize() == 0) {
        LOGE("textBlob serialize invalid, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return { 0 };
    }

    auto offset = cmdList.AddImageData(data->GetData(), data->GetSize());
    return { offset, data->GetSize() };
}

std::shared_ptr<TextBlob> CmdListHelper::GetTextBlobFromCmdList(const CmdList& cmdList,
    const OpDataHandle& textBlobHandle)
{
    if (textBlobHandle.size == 0) {
        return nullptr;
    }

    const void* data = cmdList.GetImageData(textBlobHandle.offset);
    if (!data) {
        LOGE("textBlob data nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return nullptr;
    }

    auto textBlobData = std::make_shared<Data>();
    textBlobData->BuildWithoutCopy(data, textBlobHandle.size);
    return TextBlob::Deserialize(textBlobData->GetData(), textBlobData->GetSize());
}

OpDataHandle CmdListHelper::AddDataToCmdList(CmdList& cmdList, const Data* srcData)
{
    if (!srcData) {
        LOGE("data nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return { 0 };
    }

    auto data = srcData->Serialize();
    if (data == nullptr || data->GetSize() == 0) {
        LOGE("srcData is invalid!");
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
        LOGE("get data failed!");
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
        LOGE("path is invalid, %{public}s, %{public}d", __FUNCTION__, __LINE__);
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
        LOGE("get path data failed!");
        return nullptr;
    }

    auto pathData = std::make_shared<Data>();
    pathData->BuildWithoutCopy(ptr, pathHandle.size);
    auto path = std::make_shared<Path>();
    if (path->Deserialize(pathData) == false) {
        LOGE("path deserialize failed!");
        return nullptr;
    }

    return path;
}

OpDataHandle CmdListHelper::AddColorSpaceToCmdList(CmdList& cmdList, const std::shared_ptr<ColorSpace> colorSpace)
{
    if (colorSpace == nullptr) {
        return { 0 };
    }

    auto data = colorSpace->Serialize();
    if (data == nullptr || data->GetSize() == 0) {
        LOGE("colorSpace is invalid, %{public}s, %{public}d", __FUNCTION__, __LINE__);
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
        LOGE("colorSpace deserialize failed!");
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
        LOGE("shaderEffect is invalid, %{public}s, %{public}d", __FUNCTION__, __LINE__);
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
        LOGE("shaderEffect deserialize failed!");
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
        LOGE("pathEffect is invalid, %{public}s, %{public}d", __FUNCTION__, __LINE__);
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
        LOGE("pathEffect deserialize failed!");
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
        LOGE("maskFilter is invalid, %{public}s, %{public}d", __FUNCTION__, __LINE__);
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
        LOGE("maskFilter deserialize failed!");
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
        LOGE("colorFilter is invalid, %{public}s, %{public}d", __FUNCTION__, __LINE__);
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
        LOGE("colorFilter deserialize failed!");
        return nullptr;
    }

    return colorFilter;
}

FlattenableHandle CmdListHelper::AddImageFilterToCmdList(CmdList& cmdList,
    std::shared_ptr<ImageFilter> imageFilter)
{
    if (imageFilter == nullptr) {
        return { 0 };
    }
    ImageFilter::FilterType type = imageFilter->GetType();
    auto data = imageFilter->Serialize();
    if (data == nullptr || data->GetSize() == 0) {
        LOGE("imageFilter is invalid, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return { 0 };
    }
    auto offset = cmdList.AddImageData(data->GetData(), data->GetSize());
    return { offset, data->GetSize(), static_cast<uint32_t>(type) };
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
        LOGE("imageFilter deserialize failed!");
        return nullptr;
    }

    return imageFilter;
}

std::vector<std::shared_ptr<DrawOpItem>> CmdListHelper::GetDrawOpItemsFromHandle(const CmdList& cmdList,
    const CmdListHandle& handle)
{
    if (handle.size == 0) {
        LOGE("handle.size == 0 !");
        return {};
    }

    const void* data = cmdList.GetCmdListData(handle.offset);
    if (data == nullptr) {
        LOGE("cmdList offset is invalid!");
        return {};
    }

    auto drawCmdList = DrawCmdList::CreateFromData({data, handle.size});
    if (drawCmdList == nullptr) {
        LOGE("create cmdList failed!");
        return {};
    }

    if (handle.imageSize > 0 && cmdList.GetImageData(handle.imageOffset) != nullptr) {
        if (!drawCmdList->SetUpImageData(cmdList.GetImageData(handle.imageOffset), handle.imageSize)) {
            LOGE("set up image data failed!");
            return {};
        }
    }

    return drawCmdList->UnmarshallingCmdList();
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
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
