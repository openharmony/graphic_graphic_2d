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

#include "memory/rs_tag_tracker.h"
#include "native_buffer_utils.h"
#include "platform/common/rs_log.h"
#include "render_context/render_context.h"
#include "pipeline/sk_resource_manager.h"
namespace OHOS::Rosen {
namespace NativeBufferUtils {
void DeleteVkImage(void* context)
{
    VulkanCleanupHelper* cleanupHelper = static_cast<VulkanCleanupHelper*>(context);
    if (cleanupHelper != nullptr) {
        cleanupHelper->UnRef();
    }
}

bool GetNativeBufferFormatProperties(RsVulkanContext& vkContext, VkDevice device, OH_NativeBuffer* nativeBuffer,
                                     VkNativeBufferFormatPropertiesOHOS* nbFormatProps,
                                     VkNativeBufferPropertiesOHOS* nbProps)
{
    nbFormatProps->sType = VK_STRUCTURE_TYPE_NATIVE_BUFFER_FORMAT_PROPERTIES_OHOS;
    nbFormatProps->pNext = nullptr;

    nbProps->sType = VK_STRUCTURE_TYPE_NATIVE_BUFFER_PROPERTIES_OHOS;
    nbProps->pNext = nbFormatProps;

    VkResult err = vkContext.GetRsVulkanInterface().vkGetNativeBufferPropertiesOHOS(device, nativeBuffer, nbProps);
    if (VK_SUCCESS != err) {
        ROSEN_LOGE("NativeBufferUtils: vkGetNativeBufferPropertiesOHOS Failed ! %d", err);
        return false;
    }
    return true;
}

bool CreateVkImage(RsVulkanContext& vkContext, VkImage* image,
    const VkNativeBufferFormatPropertiesOHOS& nbFormatProps, const VkExtent3D& imageSize,
    VkImageUsageFlags usageFlags = 0, bool isProtected = false)
{
    VkExternalFormatOHOS externalFormat;
    externalFormat.sType = VK_STRUCTURE_TYPE_EXTERNAL_FORMAT_OHOS;
    externalFormat.pNext = nullptr;
    externalFormat.externalFormat = 0;

    if (nbFormatProps.format == VK_FORMAT_UNDEFINED) {
        externalFormat.externalFormat = nbFormatProps.externalFormat;
    }

    const VkExternalMemoryImageCreateInfo externalMemoryImageInfo {
        VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_IMAGE_CREATE_INFO, // sType
        &externalFormat, // pNext
        VK_EXTERNAL_MEMORY_HANDLE_TYPE_OHOS_NATIVE_BUFFER_BIT_OHOS, // handleTypes
    };

    VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL;

    VkImageCreateFlags flags = isProtected ? VK_IMAGE_CREATE_PROTECTED_BIT : 0;

    const VkImageCreateInfo imageCreateInfo = {
        VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        &externalMemoryImageInfo,
        flags,
        VK_IMAGE_TYPE_2D,
        nbFormatProps.format,
        imageSize,
        1,
        1,
        VK_SAMPLE_COUNT_1_BIT,
        tiling,
        usageFlags,
        VK_SHARING_MODE_EXCLUSIVE,
        0,
        0,
        VK_IMAGE_LAYOUT_UNDEFINED,
    };

    VkResult err = vkContext.GetRsVulkanInterface().vkCreateImage(vkContext.GetDevice(),
        &imageCreateInfo, nullptr, image);
    if (err != VK_SUCCESS) {
        ROSEN_LOGE("NativeBufferUtils: vkCreateImage failed");
        return false;
    }
    return true;
}

bool AllocateDeviceMemory(RsVulkanContext& vkContext, VkDeviceMemory* memory, VkImage& image,
    OH_NativeBuffer* nativeBuffer, VkNativeBufferPropertiesOHOS& nbProps, bool isProtected)
{
    VkPhysicalDeviceMemoryProperties2 physicalDeviceMemProps;
    physicalDeviceMemProps.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2;
    physicalDeviceMemProps.pNext = nullptr;
    auto& vkInterface = vkContext.GetRsVulkanInterface();

    uint32_t foundTypeIndex = 0;
    VkDevice device = vkInterface.GetDevice();
    VkPhysicalDevice physicalDevice = vkInterface.GetPhysicalDevice();
    vkInterface.vkGetPhysicalDeviceMemoryProperties2(physicalDevice, &physicalDeviceMemProps);
    uint32_t memTypeCnt = physicalDeviceMemProps.memoryProperties.memoryTypeCount;
    bool found = false;
    for (uint32_t i = 0; i < memTypeCnt; ++i) {
        if (nbProps.memoryTypeBits & (1 << i)) {
            const VkPhysicalDeviceMemoryProperties& pdmp = physicalDeviceMemProps.memoryProperties;
            uint32_t supportedFlags = pdmp.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
            if (supportedFlags == VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
                foundTypeIndex = i;
                found = true;
                break;
            }
        }
    }
    if (!found) {
        ROSEN_LOGE("NativeBufferUtils: no fit memory type, memoryTypeBits is %{public}u", nbProps.memoryTypeBits);
        vkInterface.vkDestroyImage(device, image, nullptr);
        return false;
    }

    VkImportNativeBufferInfoOHOS nbImportInfo;
    nbImportInfo.sType = VK_STRUCTURE_TYPE_IMPORT_NATIVE_BUFFER_INFO_OHOS;
    nbImportInfo.pNext = nullptr;
    nbImportInfo.buffer = nativeBuffer;

    VkMemoryDedicatedAllocateInfo dedicatedAllocInfo;
    dedicatedAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_DEDICATED_ALLOCATE_INFO;
    dedicatedAllocInfo.pNext = &nbImportInfo;
    dedicatedAllocInfo.image = image;
    dedicatedAllocInfo.buffer = VK_NULL_HANDLE;

    VkMemoryAllocateInfo allocInfo = {
        VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO, &dedicatedAllocInfo, nbProps.allocationSize, foundTypeIndex,
    };

    VkResult err = vkInterface.vkAllocateMemory(device, &allocInfo, nullptr, memory);
    if (err != VK_SUCCESS) {
        vkInterface.vkDestroyImage(device, image, nullptr);
        ROSEN_LOGE("NativeBufferUtils: vkAllocateMemory Fail");
        return false;
    }
    return true;
}

bool BindImageMemory(VkDevice device, RsVulkanContext& vkContext, VkImage& image, VkDeviceMemory& memory)
{
    auto& vkInterface = vkContext.GetRsVulkanInterface();
    VkBindImageMemoryInfo bindImageInfo;
    bindImageInfo.sType = VK_STRUCTURE_TYPE_BIND_IMAGE_MEMORY_INFO;
    bindImageInfo.pNext = nullptr;
    bindImageInfo.image = image;
    bindImageInfo.memory = memory;
    bindImageInfo.memoryOffset = 0;

    VkResult err = vkInterface.vkBindImageMemory2(device, 1, &bindImageInfo);
    if (err != VK_SUCCESS) {
        ROSEN_LOGE("NativeBufferUtils: vkBindImageMemory2 failed");
        vkInterface.vkDestroyImage(device, image, nullptr);
        vkInterface.vkFreeMemory(device, memory, nullptr);
        return false;
    }
    return true;
}

bool MakeFromNativeWindowBuffer(std::shared_ptr<Drawing::GPUContext> skContext, NativeWindowBuffer* nativeWindowBuffer,
    NativeSurfaceInfo& nativeSurface, int width, int height, bool isProtected)
{
    OH_NativeBuffer* nativeBuffer = OH_NativeBufferFromNativeWindowBuffer(nativeWindowBuffer);
    if (nativeBuffer == nullptr) {
        ROSEN_LOGE("MakeFromNativeWindowBuffer: OH_NativeBufferFromNativeWindowBuffer failed");
        return false;
    }

    auto& vkContext = RsVulkanContext::GetSingleton();

    VkDevice device = vkContext.GetDevice();

    VkNativeBufferFormatPropertiesOHOS nbFormatProps;
    VkNativeBufferPropertiesOHOS nbProps;
    if (!GetNativeBufferFormatProperties(vkContext, device, nativeBuffer, &nbFormatProps, &nbProps)) {
        return false;
    }

    VkImageUsageFlags usageFlags = VK_IMAGE_USAGE_SAMPLED_BIT;
    if (nbFormatProps.format != VK_FORMAT_UNDEFINED) {
        usageFlags = usageFlags | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT
            | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
    }

    VkImage image;
    if (!CreateVkImage(vkContext, &image, nbFormatProps, {width, height, 1}, usageFlags, isProtected)) {
        return false;
    }

    VkDeviceMemory memory;
    if (!AllocateDeviceMemory(vkContext, &memory, image, nativeBuffer, nbProps, isProtected)) {
        return false;
    }

    if (!BindImageMemory(device, vkContext, image, memory)) {
        return false;
    }

    auto skColorSpace = RenderContext::ConvertColorGamutToSkColorSpace(nativeSurface.graphicColorGamut);
    Drawing::TextureInfo texture_info;
    texture_info.SetWidth(width);
    texture_info.SetHeight(height);
    std::shared_ptr<Drawing::VKTextureInfo> vkTextureInfo = std::make_shared<Drawing::VKTextureInfo>();
    vkTextureInfo->vkImage = image;
    vkTextureInfo->imageTiling = VK_IMAGE_TILING_OPTIMAL;
    vkTextureInfo->imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    vkTextureInfo->format = nbFormatProps.format;
    vkTextureInfo->imageUsageFlags = usageFlags;
    vkTextureInfo->sampleCount = 1;
    vkTextureInfo->levelCount = 1;
    vkTextureInfo->vkProtected = isProtected;
    texture_info.SetVKTextureInfo(vkTextureInfo);

    Drawing::ColorType colorType = Drawing::ColorType::COLORTYPE_RGBA_8888;
    if (nbFormatProps.format == VK_FORMAT_A2B10G10R10_UNORM_PACK32) {
        colorType = Drawing::ColorType::COLORTYPE_RGBA_1010102;
    }

    auto skiaColorSpace = std::make_shared<Drawing::SkiaColorSpace>();
    skiaColorSpace->SetColorSpace(skColorSpace);
    auto colorSpace = Drawing::ColorSpace::CreateFromImpl(skiaColorSpace);
    nativeSurface.drawingSurface = Drawing::Surface::MakeFromBackendTexture(
        skContext.get(),
        texture_info,
        Drawing::TextureOrigin::TOP_LEFT,
        1,
        colorType,
        colorSpace,
        DeleteVkImage,
        new VulkanCleanupHelper(RsVulkanContext::GetSingleton(),
            image, memory));
    if (nativeSurface.drawingSurface) {
        SKResourceManager::Instance().HoldResource(nativeSurface.drawingSurface);
    }

    nativeSurface.image = image;
    if (nativeSurface.nativeWindowBuffer != nullptr) {
        NativeObjectUnreference(nativeSurface.nativeWindowBuffer);
    }
    NativeObjectReference(nativeWindowBuffer);
    nativeSurface.nativeWindowBuffer = nativeWindowBuffer;

    return true;
}

GrVkYcbcrConversionInfo GetYcbcrInfo(VkNativeBufferFormatPropertiesOHOS& nbFormatProps)
{
    GrVkYcbcrConversionInfo ycbcrInfo = {
        .fFormat = nbFormatProps.format,
        .fExternalFormat = nbFormatProps.externalFormat,
        .fYcbcrModel = nbFormatProps.suggestedYcbcrModel,
        .fYcbcrRange = nbFormatProps.suggestedYcbcrRange,
        .fXChromaOffset = nbFormatProps.suggestedXChromaOffset,
        .fYChromaOffset = nbFormatProps.suggestedYChromaOffset,
        .fChromaFilter = VK_FILTER_NEAREST,
        .fForceExplicitReconstruction = VK_FALSE,
        .fFormatFeatures = nbFormatProps.formatFeatures
    };

    if (VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT & nbFormatProps.formatFeatures) {
        ycbcrInfo.fChromaFilter = VK_FILTER_LINEAR;
    }
    return ycbcrInfo;
}

Drawing::BackendTexture MakeBackendTextureFromNativeBuffer(NativeWindowBuffer* nativeWindowBuffer,
    int width, int height, bool isProtected)
{
    OH_NativeBuffer* nativeBuffer = OH_NativeBufferFromNativeWindowBuffer(nativeWindowBuffer);
    if (!nativeBuffer) {
        ROSEN_LOGE("MakeBackendTextureFromNativeBuffer: OH_NativeBufferFromNativeWindowBuffer failed");
        return {};
    }

    auto& vkContext = RsVulkanContext::GetSingleton();
    VkDevice device = vkContext.GetDevice();

    VkNativeBufferFormatPropertiesOHOS nbFormatProps;
    VkNativeBufferPropertiesOHOS nbProps;
    if (!GetNativeBufferFormatProperties(vkContext, device, nativeBuffer, &nbFormatProps, &nbProps)) {
        return {};
    }

    VkImageUsageFlags usageFlags = VK_IMAGE_USAGE_SAMPLED_BIT;
    if (nbFormatProps.format != VK_FORMAT_UNDEFINED) {
        usageFlags = usageFlags | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    }

    VkImage image;
    if (!CreateVkImage(vkContext, &image, nbFormatProps, {width, height, 1}, usageFlags, isProtected)) {
        return {};
    }

    VkDeviceMemory memory;
    if (!AllocateDeviceMemory(vkContext, &memory, image, nativeBuffer, nbProps, isProtected)) {
        return {};
    }

    if (!BindImageMemory(device, vkContext, image, memory)) {
        return {};
    }

    Drawing::BackendTexture backendTexture(true);
    Drawing::TextureInfo textureInfo;
    textureInfo.SetWidth(width);
    textureInfo.SetHeight(height);

    std::shared_ptr<Drawing::VKTextureInfo> imageInfo = std::make_shared<Drawing::VKTextureInfo>();
    imageInfo->vkImage = image;
    imageInfo->vkAlloc.memory = memory;
    imageInfo->vkProtected = isProtected ? true : false;
    imageInfo->vkAlloc.size = nbProps.allocationSize;
    imageInfo->imageTiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo->imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo->format = nbFormatProps.format;
    imageInfo->imageUsageFlags = usageFlags;
    imageInfo->levelCount = 1;
    imageInfo->currentQueueFamily = VK_QUEUE_FAMILY_EXTERNAL;
    imageInfo->ycbcrConversionInfo.format = nbFormatProps.format;
    imageInfo->ycbcrConversionInfo.externalFormat = nbFormatProps.externalFormat;
    imageInfo->ycbcrConversionInfo.ycbcrModel = nbFormatProps.suggestedYcbcrModel;
    imageInfo->ycbcrConversionInfo.ycbcrRange = nbFormatProps.suggestedYcbcrRange;
    imageInfo->ycbcrConversionInfo.xChromaOffset = nbFormatProps.suggestedXChromaOffset;
    imageInfo->ycbcrConversionInfo.yChromaOffset = nbFormatProps.suggestedYChromaOffset;
    imageInfo->ycbcrConversionInfo.chromaFilter = VK_FILTER_NEAREST;
    imageInfo->ycbcrConversionInfo.forceExplicitReconstruction = VK_FALSE;
    imageInfo->ycbcrConversionInfo.formatFeatures = nbFormatProps.formatFeatures;
    if (VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT & nbFormatProps.formatFeatures) {
        imageInfo->ycbcrConversionInfo.chromaFilter = VK_FILTER_LINEAR;
    }
    imageInfo->sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    textureInfo.SetVKTextureInfo(imageInfo);
    backendTexture.SetTextureInfo(textureInfo);
    return backendTexture;
}
} // namespace NativeBufferUtils
} // namespace OHOS::Rosen
