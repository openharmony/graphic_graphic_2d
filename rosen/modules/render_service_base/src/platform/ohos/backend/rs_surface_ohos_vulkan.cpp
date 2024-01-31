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

#include <memory>
#include "memory/rs_tag_tracker.h"
#include "SkColor.h"
#include "native_buffer_inner.h"
#include "native_window.h"
#include "vulkan/vulkan_core.h"
#include "include/gpu/GrBackendSemaphore.h"
#include "include/gpu/GrDirectContext.h"
#include "platform/common/rs_log.h"
#include "window.h"
#include "platform/common/rs_system_properties.h"
#ifdef USE_ROSEN_DRAWING
#include "drawing/engine_adapter/skia_adapter/skia_gpu_context.h"
#include "engine_adapter/skia_adapter/skia_surface.h"
#endif
namespace OHOS {
namespace Rosen {
[[maybe_unused]] static void DestroySemaphore(void *context)
{
    if (context == nullptr) {
        return;
    }
    DestroySemaphoreInfo* info = reinterpret_cast<DestroySemaphoreInfo*>(context);
    --info->mRefs;
    if (!info->mRefs) {
        info->mDestroyFunction(info->mDevice, info->mSemaphore, nullptr);
        delete info;
    }
}

RSSurfaceOhosVulkan::RSSurfaceOhosVulkan(const sptr<Surface>& producer) : RSSurfaceOhos(producer)
{
    bufferUsage_ = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_MEM_DMA;
}

RSSurfaceOhosVulkan::~RSSurfaceOhosVulkan()
{
    mSurfaceMap.clear();
    mSurfaceList.clear();
    DestoryNativeWindow(mNativeWindow);
    mNativeWindow = nullptr;
}

void RSSurfaceOhosVulkan::SetNativeWindowInfo(int32_t width, int32_t height, bool useAFBC)
{
    if (width != mWidth || height != mHeight) {
        for (auto &[key, val] : mSurfaceMap) {
            NativeWindowCancelBuffer(mNativeWindow, key);
        }
        mSurfaceMap.clear();
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
}


void RSSurfaceOhosVulkan::CreateVkSemaphore(
    VkSemaphore* semaphore, const RsVulkanContext& vkContext, NativeBufferUtils::NativeSurfaceInfo& nativeSurface)
{
    VkSemaphoreCreateInfo semaphoreInfo;
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreInfo.pNext = nullptr;
    semaphoreInfo.flags = 0;
    vkContext.vkCreateSemaphore(vkContext.GetDevice(), &semaphoreInfo, nullptr, semaphore);

    VkImportSemaphoreFdInfoKHR importSemaphoreFdInfo;
    importSemaphoreFdInfo.sType = VK_STRUCTURE_TYPE_IMPORT_SEMAPHORE_FD_INFO_KHR;
    importSemaphoreFdInfo.pNext = nullptr;
    importSemaphoreFdInfo.semaphore = *semaphore;
    importSemaphoreFdInfo.flags = VK_SEMAPHORE_IMPORT_TEMPORARY_BIT;
    importSemaphoreFdInfo.handleType = VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_SYNC_FD_BIT;
    importSemaphoreFdInfo.fd = nativeSurface.fence->Dup();
    vkContext.vkImportSemaphoreFdKHR(vkContext.GetDevice(), &importSemaphoreFdInfo);
}

int32_t RSSurfaceOhosVulkan::RequestNativeWindowBuffer(
    NativeWindowBuffer** nativeWindowBuffer, int32_t width, int32_t height, int& fenceFd, bool useAFBC)
{
    SetNativeWindowInfo(width, height, useAFBC);
    struct timespec curTime = {0, 0};
    clock_gettime(CLOCK_MONOTONIC, &curTime);
    // 1000000000 is used for transfer second to nsec
    uint64_t duration = static_cast<uint64_t>(curTime.tv_sec) * 1000000000 + static_cast<uint64_t>(curTime.tv_nsec);
    NativeWindowHandleOpt(mNativeWindow, SET_UI_TIMESTAMP, duration);

    auto res = NativeWindowRequestBuffer(mNativeWindow, nativeWindowBuffer, &fenceFd);
    if (res != OHOS::GSERROR_OK) {
        ROSEN_LOGE("RSSurfaceOhosVulkan: OH_NativeWindow_NativeWindowRequestBuffer failed %{public}d", res);
        NativeWindowCancelBuffer(mNativeWindow, *nativeWindowBuffer);
    }
    return res;
}

std::unique_ptr<RSSurfaceFrame> RSSurfaceOhosVulkan::RequestFrame(
    int32_t width, int32_t height, uint64_t uiTimestamp, bool useAFBC)
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
    int fenceFd = -1;
    if (RequestNativeWindowBuffer(&nativeWindowBuffer, width, height, fenceFd, useAFBC) != OHOS::GSERROR_OK) {
        return nullptr;
    }

    mSurfaceList.emplace_back(nativeWindowBuffer);
    NativeBufferUtils::NativeSurfaceInfo& nativeSurface = mSurfaceMap[nativeWindowBuffer];
#ifndef USE_ROSEN_DRAWING
    if (nativeSurface.skSurface == nullptr) {
#else
    if (nativeSurface.drawingSurface == nullptr) {
#endif
        NativeObjectReference(mNativeWindow);
        nativeSurface.window = mNativeWindow;
        nativeSurface.graphicColorGamut = colorSpace_;
        if (!NativeBufferUtils::MakeFromNativeWindowBuffer(
            mSkContext, nativeWindowBuffer, nativeSurface, width, height)) {
            ROSEN_LOGE("RSSurfaceOhosVulkan: MakeFromeNativeWindow failed");
            mSurfaceList.pop_back();
            NativeWindowCancelBuffer(mNativeWindow, nativeWindowBuffer);
            mSurfaceMap.erase(nativeWindowBuffer);
            return nullptr;
        }

#ifndef USE_ROSEN_DRAWING
        if (!nativeSurface.skSurface) {
#else
        if (!nativeSurface.drawingSurface) {
#endif
            ROSEN_LOGE("RSSurfaceOhosVulkan: skSurface is null, return");
            mSurfaceList.pop_back();
            NativeWindowCancelBuffer(mNativeWindow, nativeWindowBuffer);
            mSurfaceMap.erase(nativeWindowBuffer);
            return nullptr;
        } else {
            ROSEN_LOGD("RSSurfaceOhosVulkan: skSurface create success %{public}zu", mSurfaceMap.size());
        }
    }

#ifndef USE_ROSEN_DRAWING
    nativeSurface.skSurface->clearDrawingArea();
#else
    nativeSurface.drawingSurface->ClearDrawingArea();
#endif

    if (fenceFd >= 0) {
        nativeSurface.fence = std::make_unique<SyncFence>(fenceFd);
        auto status = nativeSurface.fence->GetStatus();
        if (status != SIGNALED) {
            auto const& vkContext = RsVulkanContext::GetSingleton();
            VkSemaphore semaphore;
            CreateVkSemaphore(&semaphore, vkContext, nativeSurface);
#ifndef USE_ROSEN_DRAWING
            GrBackendSemaphore backendSemaphore;
            backendSemaphore.initVulkan(semaphore);
            nativeSurface.skSurface->wait(1, &backendSemaphore);
#else
            nativeSurface.drawingSurface->Wait(1, semaphore);
#endif
        }
    }
    int32_t bufferAge;
    if (nativeSurface.lastPresentedCount == -1) {
        bufferAge = 0;
    } else {
        bufferAge = mPresentCount - nativeSurface.lastPresentedCount;
    }
#ifndef USE_ROSEN_DRAWING
    std::unique_ptr<RSSurfaceFrameOhosVulkan> frame =
        std::make_unique<RSSurfaceFrameOhosVulkan>(nativeSurface.skSurface, width, height, bufferAge);
#else
    std::unique_ptr<RSSurfaceFrameOhosVulkan> frame =
        std::make_unique<RSSurfaceFrameOhosVulkan>(nativeSurface.drawingSurface, width, height, bufferAge);
#endif
    std::unique_ptr<RSSurfaceFrame> ret(std::move(frame));
    return ret;
}

void RSSurfaceOhosVulkan::SetUiTimeStamp(const std::unique_ptr<RSSurfaceFrame>& frame, uint64_t uiTimestamp)
{
}

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

    auto& surface = mSurfaceMap[mSurfaceList.front()];

    DestroySemaphoreInfo* destroyInfo =
        new DestroySemaphoreInfo(vkContext.vkDestroySemaphore, vkContext.GetDevice(), semaphore);
    RSTagTracker tagTracker(mSkContext.get(), RSTagTracker::TAGTYPE::TAG_ACQUIRE_SURFACE);

#ifndef USE_ROSEN_DRAWING
    GrFlushInfo flushInfo;
    flushInfo.fNumSemaphores = 1;
    flushInfo.fSignalSemaphores = &backendSemaphore;
    surface.skSurface->flush(SkSurface::BackendSurfaceAccess::kPresent, flushInfo);
    mSkContext->submit();
#else
    Drawing::FlushInfo drawingFlushInfo;
    drawingFlushInfo.backendSurfaceAccess = true;
    drawingFlushInfo.numSemaphores = 1;
    drawingFlushInfo.backendSemaphore = static_cast<void*>(&backendSemaphore);
    drawingFlushInfo.finishedProc = DestroySemaphore;
    drawingFlushInfo.finishedContext = destroyInfo;
    surface.drawingSurface->Flush(&drawingFlushInfo);
    mSkContext->Submit();
#endif
    int fenceFd = -1;

    auto queue = vkContext.GetQueue();
    if (vkContext.GetHardWareGrContext().get() == mSkContext.get()) {
        queue = vkContext.GetHardwareQueue();
    }
    auto err = RsVulkanContext::HookedVkQueueSignalReleaseImageOHOS(
        queue, 1, &semaphore, surface.image, &fenceFd);
    if (err != VK_SUCCESS) {
        DestroySemaphore(destroyInfo);
        destroyInfo = nullptr;
        ROSEN_LOGE("RSSurfaceOhosVulkan QueueSignalReleaseImageOHOS failed %{public}d", err);
        return false;
    }

    auto ret = NativeWindowFlushBuffer(surface.window, surface.nativeWindowBuffer, fenceFd, {});
    if (ret != OHOS::GSERROR_OK) {
        ROSEN_LOGE("RSSurfaceOhosVulkan NativeWindowFlushBuffer failed");
        return false;
    }
    mSurfaceList.pop_front();
    DestroySemaphore(destroyInfo);
    surface.fence.reset();
    surface.lastPresentedCount = mPresentCount;
    mPresentCount++;
    return true;
}

void RSSurfaceOhosVulkan::SetColorSpace(GraphicColorGamut colorSpace)
{
    if (colorSpace != colorSpace_) {
        colorSpace_ = colorSpace;
        for (auto &[key, val] : mSurfaceMap) {
            NativeWindowCancelBuffer(mNativeWindow, key);
        }
        mSurfaceMap.clear();
    }
}

void RSSurfaceOhosVulkan::SetSurfaceBufferUsage(uint64_t usage)
{
    bufferUsage_ = usage;
}

void RSSurfaceOhosVulkan::SetSurfacePixelFormat(int32_t pixelFormat)
{
    if (pixelFormat != pixelFormat_) {
        for (auto &[key, val] : mSurfaceMap) {
            NativeWindowCancelBuffer(mNativeWindow, key);
        }
        mSurfaceMap.clear();
    }
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
