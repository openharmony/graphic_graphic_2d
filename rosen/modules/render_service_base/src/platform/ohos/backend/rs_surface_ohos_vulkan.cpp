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

#include "rs_surface_ohos_vulkan.h"


#ifndef ENABLE_NATIVEBUFFER
#include <vulkan_proc_table.h>
#include <vulkan_native_surface_ohos.h>
#include "rs_trace.h"
#else // ENABLE_NATIVEBUFFER
#include <memory>
#include "SkColor.h"
#include "native_buffer_inner.h"
#include "native_window.h"
#include "vulkan/vulkan_core.h"
#include "include/gpu/GrBackendSemaphore.h"
#endif // ENABLE_NATIVEBUFFER
#include "include/gpu/GrDirectContext.h"
#include "platform/common/rs_log.h"
#include "window.h"
namespace OHOS {
namespace Rosen {

#ifdef ENABLE_NATIVEBUFFER
namespace {
bool GetNativeBufferFormatProperties(const RsVulkanContext& vkContext, VkDevice device, OH_NativeBuffer* nativeBuffer,
                                     VkNativeBufferFormatPropertiesOHOS& nbFormatProps,
                                     VkNativeBufferPropertiesOHOS& nbProps;)
{
    nbFormatProps.sType = VK_STRUCTURE_TYPE_NATIVE_BUFFER_FORMAT_PROPERTIES_OHOS;
    nbFormatProps.pNext = nullptr;

    VkResult err = vkContext.vkGetNativeBufferPropertiesOHOS(device, nativeBuffer, &nbFormatProps);
    if (VK_SUCCESS != err) {
        ROSEN_LOGE("RSSurfaceOhosVulkan GetNativeBufferPropertiesOHOS Failed ! %d", err);
        return false;
    }
    return true;
}

bool CreateVkImage(const RsVulkanContext& vkContext, VkImage& image,
    const VkNativeBufferFormatPropertiesOHOS& nbFormatProps, int width, int height)
{
    VkExternalFormatOHOS externalFormat;
    externalFormat.sType = VK_STRUCTURE_TYPE_EXTERNAL_FORMAT_OHOS;
    externalFormat.pNext = nullptr;
    externalFormat.externalFormat = 0;

    const VkExternalMemoryImageCreateInfo externalMemoryImageInfo {
        VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_IMAGE_CREATE_INFO, // sType
        &externalFormat, // pNext
        VK_EXTERNAL_MEMORY_HANDLE_TYPE_OHOS_NATIVE_BUFFER_BIT_OHOS, // handleTypes
    };
    VkImageUsageFlags usageFlags = VK_IMAGE_USAGE_SAMPLED_BIT;
    if (nbFormatProps.format != VK_FORMAT_UNDEFINED) {
        usageFlags = usageFlags | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    }

    VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL;

    VkImageCreateFlags flags = 0;

    const VkImageCreateInfo imageCreateInfo = {
        VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        &externalMemoryImageInfo,
        flags,
        VK_IMAGE_TYPE_2D,
        nbFormatProps.format,
        {width, height, 1},
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

    VkResult err = vkContext.vkCreateImage(device, &imageCreateInfo, nullptr, &image);
    if (err != VK_SUCCESS) {
        ROSEN_LOGE("RSSurfaceOhosVulkan: CreateImage failed");
        return false;
    }
}

bool AllocateDeviceMemory(const RsVulkanContext& vkContext, VkPhysicalDevice physicalDevice, VkImage& image)
{
    VkPhysicalDeviceMemoryProperties2 physicalDeviceMemProps;
    physicalDeviceMemProps.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2;
    physicalDeviceMemProps.pNext = nullptr;

    uint32_t foundTypeIndex = 0;
    uint32_t foundHeapIndex = 0;
    vkContext.vkGetPhysicalDeviceMemoryProperties2(physicalDevice, &physicalDeviceMemProps);
    uint32_t memTypeCnt = physicalDeviceMemProps.memoryProperties.memoryTypeCount;
    bool found = false;
    for (uint32_t i = 0; i < memTypeCnt; ++i) {
        if (nbProps.memoryTypeBits & (1 << i)) {
            const VkPhysicalDeviceMemoryProperties &pdmp = physicalDeviceMemProps.memoryProperties;
            uint32_t supportedFlags = pdmp.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
            if (supportedFlags == VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
                foundTypeIndex = i;
                foundHeapIndex = pdmp.memoryTypes[i].foundHeapIndex;
                found = true;
                break;
            }
        }
    }
    if (!found) {
        ROSEN_LOGE("RSSurfaceOhosVulkan not found %{public}u", nbProps.memoryTypeBits);
        vkContext.vkDestroyImage(device, image, nullptr);
        return false;
    }

    VkImportNativeBufferInfoOHOS hwbImportInfo;
    hwbImportInfo.sType = VK_STRUCTURE_TYPE_IMPORT_NATIVE_BUFFER_INFO_OHOS;
    hwbImportInfo.pNext = nullptr;
    hwbImportInfo.buffer = nativeBuffer;

    VkMemoryDedicatedAllocateInfo dedicatedAllocInfo;
    dedicatedAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_DEDICATED_ALLOCATE_INFO;
    dedicatedAllocInfo.pNext = &hwbImportInfo;
    dedicatedAllocInfo.image = image;
    dedicatedAllocInfo.buffer = VK_NULL_HANDLE;

    VkMemoryAllocateInfo allocInfo = {
        VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO, &dedicatedAllocInfo, nbProps.allocationSize, foundTypeIndex,
    };

    VkResult err = vkContext.vkAllocateMemory(device, &allocInfo, nullptr, &memory);
    if (err != VK_SUCCESS) {
        vkContext.vkDestroyImage(device, image, nullptr);
        ROSEN_LOGE("RSSurfaceOhosVulkan AllocateMemory Fail");
        return false;
    }
    return true;
}


bool BindImageMemory(VkPhysicalDevice device, const RsVulkanContext& vkContext, VkImage& image,VkDeviceMemory memory)
{
    VkBindImageMemoryInfo bindImageInfo;
    bindImageInfo.sType = VK_STRUCTURE_TYPE_BIND_IMAGE_MEMORY_INFO;
    bindImageInfo.pNext = nullptr;
    bindImageInfo.image = image;
    bindImageInfo.memory = memory;
    bindImageInfo.memoryOffset = 0;

    VkResult err = vkContext.vkBindImageMemory2(device, 1, &bindImageInfo);
    if (VK_SUCCESS != err) {
        ROSEN_LOGE("RSSurfaceOhosVulkan BindImageMemory2 failed");
        vkContext.vkDestroyImage(device, image, nullptr);
        vkContext.vkFreeMemory(device, memory, nullptr);
        return false;
    }
    return true;
}

bool MakeFromNativeWindowBuffer(sk_sp<GrDirectContext> skContext, NativeWindowBuffer* nativeWindowBuffer,
    NativeSurfaceInfo& nativeSurface, int width, int height)
{
    OH_NativeBuffer* nativeBuffer = OH_NativeBufferFromNativeWindowBuffer(nativeWindowBuffer);
    if (nativeBuffer == nullptr) {
        ROSEN_LOGE("RSSurfaceOhosVulkan: OH_NativeBufferFromNativeWindowBuffer failed");
        return false;
    }

    auto& vkContext = RsVulkanContext::GetSingleton();

    VkPhysicalDevice physicalDevice = vkContext.GetPhysicalDevice();
    VkDevice device = vkContext.GetDevice();

    VkNativeBufferFormatPropertiesOHOS nbFormatProps;
    VkNativeBufferPropertiesOHOS nbProps;
    if (!GetNativeBufferFormatProperties(vkContext, device, nativeBuffer, &nbFormatProps, &nbProps)) {
        return false;
    }

    VkImage image;
    if (!CreateVkImage(vkContext, image, nbFormatProps, width, height)) {
        return false;
    }

    VkDeviceMemory memory;
    if (!AllocateDeviceMemory(physicalDevice ,&memory)) {
        return false;
    }

    if (!BindImageMemory(device, vkContext, image, memory)) {
        retrun false;
    }

    GrVkImageInfo image_info;
    image_info.fImage_ = image;
    image_info.fImageTiling = VK_IMAGE_TILING_OPTIMAL;
    image_info.fImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_info.fFormat = nbFormatProps.format;
    image_info.fImageUsageFlags =
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    image_info.fSampleCount = 1;
    image_info.fLevelCount = 1;

    GrBackendRenderTarget backend_render_target(width, height, 0, image_info);
    SkSurfaceProps props(0, SkPixelGeometry::kUnknown_SkPixelGeometry);

    nativeSurface.skSurface = SkSurface::MakeFromBackendRenderTarget(
        skContext.get(), backend_render_target, kTopLeft_GrSurfaceOrigin, kRGBA_8888_SkColorType,
        SkColorSpace::MakeSRGB(), &props, delete_vk_image, new VulkanCleanupHelper(RsVulkanContext::GetSingleton(),
        image, memory));

    nativeSurface.image = image;
    nativeSurface.nativeWindowBuffer = nativeWindowBuffer;

    return true;
}
}
#endif //ENABLE_NATIVEBUFFER


RSSurfaceOhosVulkan::RSSurfaceOhosVulkan(const sptr<Surface>& producer) : RSSurfaceOhos(producer)
{
    bufferUsage_ = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_MEM_DMA;
}

RSSurfaceOhosVulkan::~RSSurfaceOhosVulkan()
{
#ifdef ENABLE_NATIVEBUFFER
    mSurfaceList.clear();
    mSurfaceMap.clear();
#endif // ENABLE_NATIVEBUFFER
    DestoryNativeWindow(mNativeWindow);
    mNativeWindow = nullptr;
#ifndef ENABLE_NATIVEBUFFER
    if (mVulkanWindow) {
        delete mVulkanWindow;
    }
#endif // ENABLE_NATIVEBUFFER
}

#ifdef ENABLE_NATIVEBUFFER
void CreateVkSemaphore(VkSemaphore semaphore, const RsVulkanContext& vkContext, NativeSurfaceInfo& nativeSurface)
{
    VkSemaphoreCreateInfo semaphoreInfo;
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreInfo.pNext = nullptr;
    semaphoreInfo.flags = 0;
    vkContext.vkCreateSemaphore(vkContext.GetDevice(), &semaphoreInfo, nullptr, &semaphore);

    VkImportSemaphoreFdInfoKHR importSemaphoreFdInfo;
    importSemaphoreFdInfo.sType = VK_STRUCTURE_TYPE_IMPORT_SEMAPHORE_FD_INFO_KHR;
    importSemaphoreFdInfo.pNext = nullptr;
    importSemaphoreFdInfo.semaphore = semaphore;
    importSemaphoreFdInfo.flags = VK_SEMAPHORE_IMPORT_TEMPORARY_BIT;
    importSemaphoreFdInfo.handleType = VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_SYNC_FD_BIT;
    importSemaphoreFdInfo.fd = nativeSurface.fence->Dup();
    vkContext.vkImportSemaphoreFdKHR(vkContext.GetDevice(), &importSemaphoreFdInfo);
}

void RequestNativeWindowBuffer(NativeWindowBuffer* nativeWindowBuffer)
{
    NativeWindowHandleOpt(mNativeWindow, SET_FORMAT, pixelFormat_);
#ifdef RS_ENABLE_AFBC
    if (RSSystemProperties::GetAFBCEnabled()) {
        int32_t format = 0;
        NativeWindowHandleOpt(mNativeWindow, GET_FORMAT, &format);
        if (format == GRAPHIC_PIXEL_FMT_RGBA_8888 && useAFBC) {
            bufferUsage_ =
                (BUFFER_USAGE_HW_RENDER | BUFFER_USAGE_HW_TEXTURE | BUFFER_USAGE_HW_COMPOSER | BUFFER_USAGE_MEM_DMA);
        }
    }
#endif

    NativeWindowHandleOpt(mNativeWindow, SET_USAGE, bufferUsage_);
    NativeWindowHandleOpt(mNativeWindow, SET_BUFFER_GEOMETRY, width, height);
    NativeWindowHandleOpt(mNativeWindow, GET_BUFFER_GEOMETRY, &mHeight, &mWidth);
    NativeWindowHandleOpt(mNativeWindow, SET_COLOR_GAMUT, colorSpace_);
    NativeWindowHandleOpt(mNativeWindow, SET_UI_TIMESTAMP, uiTimestamp);

    int fenceFd = -1;
    auto res = NativeWindowRequestBuffer(mNativeWindow, &nativeWindowBuffer, &fenceFd);
    if (res != OHOS::GSERROR_OK) {
        ROSEN_LOGE("RSSurfaceOhosVulkan: OH_NativeWindow_NativeWindowRequestBuffer failed %{public}d", res);
        NativeWindowCancelBuffer(mNativeWindow, nativeWindowBuffer);
    }
}

std::unique_ptr<RSSurfaceFrame> RSSurfaceOhosVulkan::RequestFrame(int32_t width, int32_t height,
    uint64_t uiTimestamp, bool useAFBC)
{
    if (mNativeWindow == nullptr) {
        mNativeWindow = CreateNativeWindowFromSurface(&producer_);
        ROSEN_LOGD("RSSurfaceOhosVulkan: create native window");
    }

    if (!mSkContext) {
        ROSEN_LOGE("RSSurfaceOhosVulkan: skia context is nullptr");
        return nullptr;
    }
    NativeWindowBuffer* nativeWindowBuffer = nullptr;

    RequestNativeWindowBuffer(nativeWindowBuffer);
    mSurfaceList.emplace_back(nativeWindowBuffer);
    NativeSurfaceInfo& nativeSurface = mSurfaceMap[nativeWindowBuffer];

    if (nativeSurface.skSurface == nullptr) {
        nativeSurface.window = mNativeWindow;
        bool suc = MakeFromNativeWindowBuffer(mSkContext, nativeWindowBuffer, nativeSurface, width, height);
        if (suc) {
            ROSEN_LOGI("RSSurfaceOhosVulkan: MakeFromeNativeWindow success");
        } else {
            ROSEN_LOGE("RSSurfaceOhosVulkan: MakeFromeNativeWindow failed");
            NativeWindowCancelBuffer(mNativeWindow, nativeWindowBuffer);
            return nullptr;
        }

        if (!nativeSurface.skSurface) {
            ROSEN_LOGE("RSSurfaceOhosVulkan: skSurface is null, return");
            mSurfaceList.pop_back();
            return nullptr;
        } else {
            ROSEN_LOGI("RSSurfaceOhosVulkan: skSurface create success %{public}zu", mSurfaceMap.size());
        }
    }

    if (fenceFd >= 0) {
        nativeSurface.fence = std::make_unique<SyncFence>(fenceFd);
        auto status = nativeSurface.fence->GetStatus();
        if (status != SIGNALED) {
            auto& vkContext = RsVulkanContext::GetSingleton();
            VkSemaphore semaphore;
            CreateVkSemaphore(&semaphore, vkContext, nativeSurface);
            GrBackendSemaphore backendSemaphore;
            backendSemaphore.initVulkan(semaphore);
            nativeSurface.skSurface->wait(1, &backendSemaphore);
        }
    }
    int32_t bufferAge = mPresentCount - nativeSurface.lastPresentedCount;
    std::unique_ptr<RSSurfaceFrameOhosVulkan> frame =
        std::make_unique<RSSurfaceFrameOhosVulkan>(nativeSurface.skSurface, width, height, bufferAge);
    std::unique_ptr<RSSurfaceFrame> ret(std::move(frame));
    return ret;
}
#else // ENABLE_NATIVEBUFFER
std::unique_ptr<RSSurfaceFrame> RSSurfaceOhosVulkan::RequestFrame(int32_t width, int32_t height,
    uint64_t uiTimestamp, bool useAFBC)
{
    if (mNativeWindow == nullptr) {
        mNativeWindow = CreateNativeWindowFromSurface(&producer_);
        ROSEN_LOGD("RSSurfaceOhosVulkan: create native window");
    }

    NativeWindowHandleOpt(mNativeWindow, SET_FORMAT, pixelFormat_);
#ifdef RS_ENABLE_AFBC
    if (RSSystemProperties::GetAFBCEnabled()) {
        int32_t format = 0;
        NativeWindowHandleOpt(mNativeWindow, GET_FORMAT, &format);
        if (format == GRAPHIC_PIXEL_FMT_RGBA_8888 && useAFBC) {
            bufferUsage_ =
                (BUFFER_USAGE_HW_RENDER | BUFFER_USAGE_HW_TEXTURE | BUFFER_USAGE_HW_COMPOSER | BUFFER_USAGE_MEM_DMA);
        }
    }
#endif

    NativeWindowHandleOpt(mNativeWindow, SET_USAGE, bufferUsage_);
    NativeWindowHandleOpt(mNativeWindow, SET_BUFFER_GEOMETRY, width, height);
    NativeWindowHandleOpt(mNativeWindow, GET_BUFFER_GEOMETRY, &mHeight, &mWidth);
    NativeWindowHandleOpt(mNativeWindow, SET_COLOR_GAMUT, colorSpace_);
    NativeWindowHandleOpt(mNativeWindow, SET_UI_TIMESTAMP, uiTimestamp);

    if (mVulkanWindow == nullptr) {
        auto vulkanSurface = std::make_unique<vulkan::VulkanNativeSurfaceOHOS>(mNativeWindow);
        mVulkanWindow = new vulkan::VulkanWindow(std::move(vulkanSurface));
        ROSEN_LOGD("RSSurfaceOhosVulkan: create vulkan window");
    }

    if (mVulkanWindow == nullptr) {
        ROSEN_LOGE("RSSurfaceOhosVulkan: Invalid VulkanWindow, return");
        return nullptr;
    }

    sk_sp<SkSurface> skSurface = mVulkanWindow->AcquireSurface();
    if (!skSurface) {
        ROSEN_LOGE("RSSurfaceOhosVulkan: skSurface is null, return");
        return nullptr;
    }

    std::unique_ptr<RSSurfaceFrameOhosVulkan> frame =
        std::make_unique<RSSurfaceFrameOhosVulkan>(skSurface, width, height);

    std::unique_ptr<RSSurfaceFrame> ret(std::move(frame));

    return ret;
}
#endif // ENABLE_NATIVEBUFFER

void RSSurfaceOhosVulkan::SetUiTimeStamp(const std::unique_ptr<RSSurfaceFrame>& frame, uint64_t uiTimestamp)
{
    if (mNativeWindow == nullptr) {
        mNativeWindow = CreateNativeWindowFromSurface(&producer_);
        ROSEN_LOGD("RSSurfaceOhosVulkan: create native window");
    }

    struct timespec curTime = {0, 0};
    clock_gettime(CLOCK_MONOTONIC, &curTime);
    // 1000000000 is used for transfer second to nsec
    uint64_t duration = static_cast<uint64_t>(curTime.tv_sec) * 1000000000 + static_cast<uint64_t>(curTime.tv_nsec);
    NativeWindowHandleOpt(mNativeWindow, SET_UI_TIMESTAMP, duration);
}

#ifdef ENABLE_NATIVEBUFFER
bool RSSurfaceOhosVulkan::FlushFrame(std::unique_ptr<RSSurfaceFrame>& frame, uint64_t uiTimestamp)
{
    if (mSurfaceList.empty()) {
        return false;
    }
    auto& vkContext = RsVulkanContext::GetSingleton();

    VkExportSemaphoreCreateInfo exportSemaphoreCreateInfo;
    exportSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_EXPORT_SEMAPHORE_CREATE_INFO;
    exportSemaphoreCreateInfo.pNext = nullptr;
    exportSemaphoreCreateInfo.handleTypes = VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_SYNC_FD_BIT;

    VkSemaphoreCreateInfo semaphoreInfo;
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreInfo.pNext = &exportSemaphoreCreateInfo;
    semaphoreInfo.flags = 0;
    VkSemaphore semaphore;
    vkContext.vkCreateSemaphore(vkContext.GetDevice(), &semaphoreInfo, nullptr, &semaphore);

    GrBackendSemaphore backendSemaphore;
    backendSemaphore.initVulkan(semaphore);

    GrFlushInfo flushInfo;
    flushInfo.fNumSemaphores = 1;
    flushInfo.fSignalSemaphores = &backendSemaphore;

    auto& surface = mSurfaceMap[mSurfaceList.front()];
    surface.skSurface->flush(SkSurface::BackendSurfaceAccess::kPresent, flushInfo);
    mSkContext->submit();

    int fenceFd = -1;

    auto err = RsVulkanContext::interceptedVkQueueSignalReleaseImageOHOS(
        vkContext.GetQueue(), 1, &semaphore, surface.image, &fenceFd);
    if (err != VK_SUCCESS) {
        ROSEN_LOGE("RSSurfaceOhosVulkan QueueSignalReleaseImageOHOS failed %{public}d", err);
        return false;
    }

    auto ret = NativeWindowFlushBuffer(surface.window, surface.nativeWindowBuffer, fenceFd, {});
    if (ret != OHOS::GSERROR_OK) {
        ROSEN_LOGE("RSSurfaceOhosVulkan NativeWindowFlushBuffer failed");
        return false;
    }
    mSurfaceList.pop_front();
    vkContext.vkDestroySemaphore(vkContext.GetDevice(), semaphore, nullptr);
    surface.lastPresentedCount = mPresentCount;
    mPresentCount++;
    surface.fence.reset();
    return true;
}
#else // ENABLE_NATIVEBUFFER
bool RSSurfaceOhosVulkan::FlushFrame(std::unique_ptr<RSSurfaceFrame>& frame, uint64_t uiTimestamp)
{
    if (mVulkanWindow == nullptr) {
        ROSEN_LOGE("RSSurfaceOhosVulkan: Invalid VulkanWindow, return");
        return false;
    }

    mVulkanWindow->SwapBuffers();
    surface.lastPresentedCount = mPresentCount;
    mPresentCount++;
    return true;
}
#endif // ENABLE_NATIVEBUFFER

void RSSurfaceOhosVulkan::SetSurfaceBufferUsage(uint64_t usage)
{
    bufferUsage_ = usage;
}

void RSSurfaceOhosVulkan::SetSurfacePixelFormat(int32_t pixelFormat)
{
    pixelFormat_ = pixelFormat;
}

void RSSurfaceOhosVulkan::ClearBuffer()
{
    if (producer_ != nullptr) {
        ROSEN_LOGD("RSSurfaceOhosVulkan: Clear surface buffer!");
        producer_->GoBackground();
    }
}

void RSSurfaceOhosVulkan::ResetBufferAge()
{
    ROSEN_LOGD("RSSurfaceOhosVulkan: Reset Buffer Age!");
}
} // namespace Rosen
} // namespace OHOS
