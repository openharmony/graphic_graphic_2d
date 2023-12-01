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

#ifndef SKIA_TEXTURE_INFO_H
#define SKIA_TEXTURE_INFO_H
#ifdef ACE_ENABLE_GPU
#include "include/gpu/GrBackendSurface.h"
#endif

#include "image/image.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaTextureInfo {
public:
    static GrSurfaceOrigin ConvertToGrSurfaceOrigin(const TextureOrigin& origin)
    {
        switch (origin) {
            case TextureOrigin::TOP_LEFT:
                return GrSurfaceOrigin::kTopLeft_GrSurfaceOrigin;
            case TextureOrigin::BOTTOM_LEFT:
                return GrSurfaceOrigin::kBottomLeft_GrSurfaceOrigin;
            default:
                return GrSurfaceOrigin::kTopLeft_GrSurfaceOrigin;
        }
    }
#ifdef RS_ENABLE_VK
    static GrBackendTexture ConvertToGrBackendVKTexture(const VKTextureInfo& info)
    {
        GrVkImageInfo imageInfo;
        imageInfo.fImage = info.vkImage;

        GrVkAlloc alloc;
        alloc.fMemory = info.vkAlloc.memory;
        alloc.fOffset = info.vkAlloc.offset;
        alloc.fSize = info.vkAlloc.size;
        alloc.fFlags = info.vkAlloc.flags;
        imageInfo.vkAlloc = alloc;

        imageInfo.fImageTiling = info.imageTiling;
        imageInfo.fImageLayout = info.imageLayout;
        imageInfo.fFormat = info.format;
        imageInfo.fImageUsageFlags = info.imageUsageFlags;
        imageInfo.fSampleCount = info.sampleCount;
        imageInfo.fLevelCount = info.levelCount;
        imageInfo.fCurrentQueueFamily = info.currentQueueFamily;
        imageInfo.fProtected = info.vkProtected ? GrProtected::kYes : GrProtected::kNo;

        GrVkYcbcrConversionInfo ycbcrInfo = {
            .fFormat = info.ycbcrConversionInfo.format,
            .fExternalFormat = info.ycbcrConversionInfo.externalFormat,
            .fYcbcrModel = info.ycbcrConversionInfo.ycbcrModel,
            .fYcbcrRange = info.ycbcrConversionInfo.ycbcrRange,
            .fXChromaOffset = info.ycbcrConversionInfo.xChromaOffset,
            .fYChromaOffset = info.ycbcrConversionInfo.yChromaOffset,
            .fChromaFilter = info.ycbcrConversionInfo.chromaFilter,
            .fForceExplicitReconstruction = info.ycbcrConversionInfo.forceExplicitReconstruction,
            .fFormatFeatures = info.ycbcrConversionInfo.formatFeatures,
        };
        imageInfo.fYcbcrConversionInfo = ycbcrInfo;

        imageInfo.fSharingMode = info.sharingMode;

        GrBackendTexture backendTexture(info.width, info.height, imageInfo);
        return backendTexture;
    }

    static VKTextureInfo ConvertToVKTexture(const GrBackendTexture& backendTexture)
    {
        VKTextureInfo info;
        info.width = backendTexture.width();
        info.height = backendTexture.height();

        GrVkImageInfo vkImageInfo;
        backendTexture.getVkImageInfo(&vkImageInfo);

        info.vkImage = vkImageInfo.fImage;

        info.vkAlloc.memory = vkImageInfo.fAlloc.fMemory;
        info.vkAlloc.offset = vkImageInfo.fAlloc.fOffset;
        info.vkAlloc.size = vkImageInfo.fAlloc.fSize;
        info.vkAlloc.flags = vkImageInfo.fAlloc.fFlags;

        info.imageTiling = vkImageInfo.fImageTiling;
        info.imageLayout = vkImageInfo.fImageLayout;
        info.format = vkImageInfo.fFormat;
        info.imageUsageFlags = vkImageInfo.fImageUsageFlags;
        info.sampleCount = vkImageInfo.fSampleCount;
        info.levelCount = vkImageInfo.fLevelCount;
        info.currentQueueFamily = vkImageInfo.fCurrentQueueFamily;
        info.vkProtected = (vkImageInfo.fProtected == GrProtected::kYes) ? true : false;

        info.ycbcrConversionInfo.format = vkImageInfo.fYcbcrConversionInfo.fFormat;
        info.ycbcrConversionInfo.externalFormat = vkImageInfo.fYcbcrConversionInfo.fExternalFormat;
        info.ycbcrConversionInfo.ycbcrModel = vkImageInfo.fYcbcrConversionInfo.fYcbcrModel;
        info.ycbcrConversionInfo.ycbcrRange = vkImageInfo.fYcbcrConversionInfo.fYcbcrRange;
        info.ycbcrConversionInfo.xChromaOffset = vkImageInfo.fYcbcrConversionInfo.fXChromaOffset;
        info.ycbcrConversionInfo.yChromaOffset = vkImageInfo.fYcbcrConversionInfo.fYChromaOffset;
        info.ycbcrConversionInfo.chromaFilter = vkImageInfo.fYcbcrConversionInfo.fChromaFilter;
        info.ycbcrConversionInfo.forceExplicitReconstruction =
            vkImageInfo.fYcbcrConversionInfo.fForceExplicitReconstruction;
        info.ycbcrConversionInfo.formatFeatures = vkImageInfo.fYcbcrConversionInfo.fFormatFeatures;

        info.sharingMode = vkImageInfo.fSharingMode;
        return info;
    }
#endif

    static TextureInfo ConvertToTextureInfo(const GrBackendTexture& grBackendTexture)
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

    static GrBackendTexture ConvertToGrBackendTexture(const TextureInfo& info)
    {
#ifdef RS_ENABLE_VK
        return ConvertToGrBackendVKTexture(info);
#else
        GrGLTextureInfo grGLTextureInfo = { info.GetTarget(), info.GetID(), info.GetFormat() };
        GrBackendTexture backendTexture(info.GetWidth(), info.GetHeight(),
            static_cast<GrMipMapped>(info.GetIsMipMapped()), grGLTextureInfo);
        return backendTexture;
#endif
    }
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif // SKIA_TEXTURE_INFO_H