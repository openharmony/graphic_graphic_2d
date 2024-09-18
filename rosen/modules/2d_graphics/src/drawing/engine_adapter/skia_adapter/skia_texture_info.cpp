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

#include "skia_texture_info.h"
#include "utils/system_properties.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

#ifdef RS_ENABLE_VK
GrBackendTexture SkiaTextureInfo::ConvertToGrBackendVKTexture(const TextureInfo& info)
{
    GrVkImageInfo imageInfo;
    if (SystemProperties::GetGpuApiType() != GpuApiType::VULKAN &&
        SystemProperties::GetGpuApiType() != GpuApiType::DDGR) {
        GrBackendTexture backendTexture(0, 0, imageInfo);
        return backendTexture;
    }

    auto vkInfo = info.GetVKTextureInfo();
    if (!vkInfo) {
        GrBackendTexture backendTexture(info.GetWidth(), info.GetHeight(), imageInfo);
        return backendTexture;
    }
    imageInfo.fImage = vkInfo->vkImage;
    GrVkAlloc alloc;
    alloc.fMemory = vkInfo->vkAlloc.memory;
    alloc.fOffset = vkInfo->vkAlloc.offset;
    alloc.fSize = vkInfo->vkAlloc.size;
    alloc.fFlags = vkInfo->vkAlloc.flags;
    imageInfo.fAlloc = alloc;

    imageInfo.fImageTiling = vkInfo->imageTiling;
    imageInfo.fImageLayout = vkInfo->imageLayout;
    imageInfo.fFormat = vkInfo->format;
    imageInfo.fImageUsageFlags = vkInfo->imageUsageFlags;
    imageInfo.fSampleCount = vkInfo->sampleCount;
    imageInfo.fLevelCount = vkInfo->levelCount;
    imageInfo.fCurrentQueueFamily = vkInfo->currentQueueFamily;
    imageInfo.fProtected = vkInfo->vkProtected ? GrProtected::kYes : GrProtected::kNo;

    GrVkYcbcrConversionInfo ycbcrInfo = {
        .fFormat = vkInfo->ycbcrConversionInfo.format,
        .fExternalFormat = vkInfo->ycbcrConversionInfo.externalFormat,
        .fYcbcrModel = vkInfo->ycbcrConversionInfo.ycbcrModel,
        .fYcbcrRange = vkInfo->ycbcrConversionInfo.ycbcrRange,
        .fXChromaOffset = vkInfo->ycbcrConversionInfo.xChromaOffset,
        .fYChromaOffset = vkInfo->ycbcrConversionInfo.yChromaOffset,
        .fChromaFilter = vkInfo->ycbcrConversionInfo.chromaFilter,
        .fForceExplicitReconstruction = vkInfo->ycbcrConversionInfo.forceExplicitReconstruction,
        .fFormatFeatures = vkInfo->ycbcrConversionInfo.formatFeatures
    };
    imageInfo.fYcbcrConversionInfo = ycbcrInfo;

    imageInfo.fSharingMode = vkInfo->sharingMode;

    GrBackendTexture backendTexture(info.GetWidth(), info.GetHeight(), imageInfo);
    return backendTexture;
}

void SkiaTextureInfo::ConvertToVKTexture(const GrBackendTexture& backendTexture, TextureInfo& info)
{
    if (SystemProperties::GetGpuApiType() != GpuApiType::VULKAN &&
        SystemProperties::GetGpuApiType() != GpuApiType::DDGR) {
        return;
    }
    std::shared_ptr<VKTextureInfo> vkInfo = std::make_shared<VKTextureInfo>();
    info.SetWidth(backendTexture.width());
    info.SetHeight(backendTexture.height());

    GrVkImageInfo vkImageInfo;
    backendTexture.getVkImageInfo(&vkImageInfo);

    vkInfo->vkImage = vkImageInfo.fImage;

    vkInfo->vkAlloc.memory = vkImageInfo.fAlloc.fMemory;
    vkInfo->vkAlloc.offset = vkImageInfo.fAlloc.fOffset;
    vkInfo->vkAlloc.size = vkImageInfo.fAlloc.fSize;
    vkInfo->vkAlloc.flags = vkImageInfo.fAlloc.fFlags;

    vkInfo->imageTiling = vkImageInfo.fImageTiling;
    vkInfo->imageLayout = vkImageInfo.fImageLayout;
    vkInfo->format = vkImageInfo.fFormat;
    vkInfo->imageUsageFlags = vkImageInfo.fImageUsageFlags;
    vkInfo->sampleCount = vkImageInfo.fSampleCount;
    vkInfo->levelCount = vkImageInfo.fLevelCount;
    vkInfo->currentQueueFamily = vkImageInfo.fCurrentQueueFamily;
    vkInfo->vkProtected = (vkImageInfo.fProtected == GrProtected::kYes) ? true : false;

    vkInfo->ycbcrConversionInfo.format = vkImageInfo.fYcbcrConversionInfo.fFormat;
    vkInfo->ycbcrConversionInfo.externalFormat = vkImageInfo.fYcbcrConversionInfo.fExternalFormat;
    vkInfo->ycbcrConversionInfo.ycbcrModel = vkImageInfo.fYcbcrConversionInfo.fYcbcrModel;
    vkInfo->ycbcrConversionInfo.ycbcrRange = vkImageInfo.fYcbcrConversionInfo.fYcbcrRange;
    vkInfo->ycbcrConversionInfo.xChromaOffset = vkImageInfo.fYcbcrConversionInfo.fXChromaOffset;
    vkInfo->ycbcrConversionInfo.yChromaOffset = vkImageInfo.fYcbcrConversionInfo.fYChromaOffset;
    vkInfo->ycbcrConversionInfo.chromaFilter = vkImageInfo.fYcbcrConversionInfo.fChromaFilter;
    vkInfo->ycbcrConversionInfo.forceExplicitReconstruction =
        vkImageInfo.fYcbcrConversionInfo.fForceExplicitReconstruction;
    vkInfo->ycbcrConversionInfo.formatFeatures = vkImageInfo.fYcbcrConversionInfo.fFormatFeatures;

    vkInfo->sharingMode = vkImageInfo.fSharingMode;

    info.SetVKTextureInfo(vkInfo);
}
#endif

GrBackendTexture SkiaTextureInfo::ConvertToGrBackendTexture(const TextureInfo& info)
{
#ifdef RS_ENABLE_VK
    if (GetGpuApiType() == OHOS::Rosen::GpuApiType::VULKAN ||
        GetGpuApiType() == OHOS::Rosen::GpuApiType::DDGR) {
        return ConvertToGrBackendVKTexture(info);
    } else {
        GrGLTextureInfo grGLTextureInfo = { info.GetTarget(), info.GetID(), info.GetFormat() };
        GrBackendTexture backendTexture(info.GetWidth(), info.GetHeight(),
            static_cast<GrMipMapped>(info.GetIsMipMapped()), grGLTextureInfo);
        return backendTexture;
    }
#else
    GrGLTextureInfo grGLTextureInfo = { info.GetTarget(), info.GetID(), info.GetFormat() };
    GrBackendTexture backendTexture(info.GetWidth(), info.GetHeight(),
        static_cast<GrMipMapped>(info.GetIsMipMapped()), grGLTextureInfo);
    return backendTexture;
#endif
}

TextureInfo SkiaTextureInfo::ConvertToTextureInfo(const GrBackendTexture& grBackendTexture)
{
    GrGLTextureInfo* grGLTextureInfo = new GrGLTextureInfo();
    grBackendTexture.getGLTextureInfo(grGLTextureInfo);
    TextureInfo textureInfo;
    textureInfo.SetWidth(grBackendTexture.width());
    textureInfo.SetHeight(grBackendTexture.height());
    textureInfo.SetIsMipMapped(static_cast<bool>(grBackendTexture.mipmapped()));
    textureInfo.SetTarget(grGLTextureInfo->fTarget);
    textureInfo.SetID(grGLTextureInfo->fID);
    textureInfo.SetFormat(grGLTextureInfo->fFormat);
    delete grGLTextureInfo;
    return textureInfo;
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
