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

#include "surface_ohos_vulkan.h"

#include <iostream>
#include <vulkan_native_surface_ohos.h>
#include <vulkan_window.h>
#include <vulkan_proc_table.h>
#include <hilog/log.h>
#include <display_type.h>
#include "window.h"


#ifdef ENABLE_NATIVEBUFFER
#include "SkColor.h"
#include "native_buffer_inner.h"
#include "native_window.h"
#include "vulkan/vulkan_core.h"
#endif

#include "rs_vulkan_context.h"
#include "include/gpu/GrBackendSemaphore.h"


#ifdef USE_ROSEN_DRAWING
#include "engine_adapter/skia_adapter/skia_surface.h"
#endif

namespace OHOS {
namespace Rosen {
SurfaceOhosVulkan::SurfaceOhosVulkan(const sptr<Surface>& producer)
    : SurfaceOhos(producer), frame_(nullptr)
{
}

SurfaceOhosVulkan::~SurfaceOhosVulkan()
{
    frame_ = nullptr;
    if (mNativeWindow_ != nullptr) {
        DestoryNativeWindow(mNativeWindow_);
        mNativeWindow_ = nullptr;
    }
    if (mVulkanWindow_ != nullptr) {
        delete mVulkanWindow_;
    }
}

void SurfaceOhosVulkan::SetNativeWindowInfo(int32_t width, int32_t height)
{
    uint64_t bufferUsage = BUFFER_USAGE_HW_RENDER | BUFFER_USAGE_HW_TEXTURE |
        BUFFER_USAGE_HW_COMPOSER | BUFFER_USAGE_MEM_DMA;
    NativeWindowHandleOpt(mNativeWindow_, SET_FORMAT, GRAPHIC_PIXEL_FMT_RGBA_8888);
#ifdef RS_ENABLE_AFBC
    if (RSSystemProperties::GetAFBCEnabled()) {
        int32_t format = 0;
        NativeWindowHandleOpt(mNativeWindow_, GET_FORMAT, &format);
        if (format == GRAPHIC_PIXEL_FMT_RGBA_8888) {
            bufferUsage =
                (BUFFER_USAGE_HW_RENDER | BUFFER_USAGE_HW_TEXTURE | BUFFER_USAGE_HW_COMPOSER | BUFFER_USAGE_MEM_DMA);
        }
    }
#endif

    NativeWindowHandleOpt(mNativeWindow_, SET_USAGE, bufferUsage);
    NativeWindowHandleOpt(mNativeWindow_, SET_BUFFER_GEOMETRY, width, height);
    NativeWindowHandleOpt(mNativeWindow_, SET_COLOR_GAMUT, colorSpace_);
}

std::unique_ptr<SurfaceFrame> SurfaceOhosVulkan::RequestFrame(int32_t width, int32_t height)
{
    if (mNativeWindow_ == nullptr) {
        mNativeWindow_ = CreateNativeWindowFromSurface(&producer_);
        if (mNativeWindow_ == nullptr) {
            LOGE("SurfaceOhosVulkan nativewindow is null");
            return nullptr;
        }
    }

    if (mVulkanWindow_ == nullptr) {
        auto vulkan_surface_ohos = std::make_unique<vulkan::VulkanNativeSurfaceOHOS>(mNativeWindow_);
        mVulkanWindow_ = new vulkan::VulkanWindow(std::move(vulkan_surface_ohos));
    }

    surface_ = std::make_shared<Drawing::Surface>();
#ifdef ENABLE_DDGR_OPTIMIZE
    auto ddgrSurface = mVulkanWindow_->AcquireSurface();
    if (!ddgrSurface) {
        LOGE("SurfaceOhosVulkan: ddgrSurface or skSurface is null, return");
        return nullptr;
    }

    if (!ddgrCanvas_) {
        ddgrCanvas_ = std::make_shared<DDGR::DDGRCanvasV2>(*(mVulkanWindow_->GetDDGRContext()));
    }
    ddgrCanvas_->SetCurSurface(ddgrSurface);
    ddgrCanvas_->BeginFrame();
    surface_->GetImpl<Drawing::DDGRSurface>()->ImportDDGRSurface(ddgrSurface);
    surface_->GetImpl<Drawing::DDGRSurface>()->SetGrContext(mVulkanWindow_->GetDDGRContext());
#else // ENABLE_DDGR_OPTIMIZE
    sk_sp<SkSurface> skSurface = mVulkanWindow_->AcquireSurface();
    surface_->GetImpl<Drawing::SkiaSurface>()->SetSkSurface(skSurface);
#endif // ENABLE_DDGR_OPTIMIZE

    frame_ = std::make_unique<SurfaceFrameOhosVulkan>(surface_, width, height);

    frame_->SetColorSpace(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);

    NativeWindowHandleOpt(mNativeWindow_, SET_BUFFER_GEOMETRY, width, height);
    NativeWindowHandleOpt(mNativeWindow_, SET_COLOR_GAMUT, frame_->GetColorSpace());

    std::unique_ptr<SurfaceFrame> ret(std::move(frame_));
    return ret;
}

void SurfaceOhosVulkan::CreateVkSemaphore(
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

int32_t SurfaceOhosVulkan::RequestNativeWindowBuffer(
    NativeWindowBuffer** nativeWindowBuffer, int32_t width, int32_t height, int& fenceFd)
{
    SetNativeWindowInfo(width, height);
    struct timespec curTime = {0, 0};
    clock_gettime(CLOCK_MONOTONIC, &curTime);
    // 1000000000 is used for transfer second to nsec
    uint64_t duration = static_cast<uint64_t>(curTime.tv_sec) * 1000000000 + static_cast<uint64_t>(curTime.tv_nsec);
    NativeWindowHandleOpt(mNativeWindow_, SET_UI_TIMESTAMP, duration);

    auto res = NativeWindowRequestBuffer(mNativeWindow_, nativeWindowBuffer, &fenceFd);
    if (res != OHOS::GSERROR_OK) {
        LOGE("RSSurfaceOhosVulkan: OH_NativeWindow_NativeWindowRequestBuffer failed %{public}d", res);
        NativeWindowCancelBuffer(mNativeWindow_, *nativeWindowBuffer);
    }
    return res;
}

std::unique_ptr<SurfaceFrame> SurfaceOhosVulkan::NativeRequestFrame(int32_t width, int32_t height)
{
    if (mNativeWindow_ == nullptr) {
        mNativeWindow_ = CreateNativeWindowFromSurface(&producer_);
        LOGI("RSSurfaceOhosVulkan: create native window");
    }

    if (drContext_ == nullptr) {
        drContext_ = RsVulkanContext::GetSingleton().CreateDrawingContext();
    }
    if (!drContext_) {
        LOGI("RSSurfaceOhosVulkan: skia context is nullptr");
        return nullptr;
    }

    NativeWindowBuffer* nativeWindowBuffer = nullptr;
    int fenceFd = -1;
    if (RequestNativeWindowBuffer(&nativeWindowBuffer, width, height, fenceFd) != OHOS::GSERROR_OK) {
        return nullptr;
    }

    surfaceList_.emplace_back(nativeWindowBuffer);
    NativeBufferUtils::NativeSurfaceInfo& nativeSurface = surfaceMap_[nativeWindowBuffer];

#ifndef USE_ROSEN_DRAWING
    if (nativeSurface.skSurface == nullptr) {
#else
    if (nativeSurface.drawingSurface == nullptr) {
#endif
        nativeSurface.window = mNativeWindow_;
        nativeSurface.graphicColorGamut = colorSpace_;
        if (!NativeBufferUtils::MakeFromNativeWindowBuffer(
            drContext_, nativeWindowBuffer, nativeSurface, width, height)) {
            LOGE("RSSurfaceOhosVulkan: MakeFromeNativeWindow failed");
            NativeWindowCancelBuffer(mNativeWindow_, nativeWindowBuffer);
            return nullptr;
        }

#ifndef USE_ROSEN_DRAWING
        if (!nativeSurface.skSurface) {
#else
        if (!nativeSurface.drawingSurface) {
#endif
            LOGE("RSSurfaceOhosVulkan: skSurface is null, return");
            surfaceList_.pop_back();
            return nullptr;
        } else {
            LOGI("RSSurfaceOhosVulkan: skSurface create success %{public}zu", surfaceMap_.size());
        }
    }

    if (fenceFd >= 0) {
        nativeSurface.fence = std::make_unique<SyncFence>(fenceFd);
        auto status = nativeSurface.fence->GetStatus();
        if (status != SIGNALED) {
            const auto& vkContext = RsVulkanContext::GetSingleton();
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
    frame_ = std::make_unique<SurfaceFrameOhosVulkan>(nativeSurface.drawingSurface, width, height);
    frame_->SetColorSpace(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    std::unique_ptr<SurfaceFrame> ret(std::move(frame_));
    surface_ = nativeSurface.drawingSurface;
    return ret;
}

bool SurfaceOhosVulkan::FlushFrame(std::unique_ptr<SurfaceFrame>& frame)
{
    if (drawingProxy_ == nullptr) {
        LOGE("drawingProxy_ is nullptr, can not FlushFrame");
        return false;
    }
    // gpu render flush
    drawingProxy_->RenderFrame();
    if (mVulkanWindow_ != nullptr) {
#ifdef ENABLE_DDGR_OPTIMIZE
        mVulkanWindow_->SwapBuffers(ddgrCanvas_);
#else
        mVulkanWindow_->SwapBuffers();
#endif
    } else {
        LOGE("mVulkanWindow_ is null");
    }
    return true;
}

bool SurfaceOhosVulkan::NativeFlushFrame(std::unique_ptr<SurfaceFrame> &frame)
{
    if (drawingProxy_ == nullptr) {
        LOGE("drawingProxy_ is nullptr, can not FlushFrame");
        return false;
    }
    // gpu render flush
    drawingProxy_->RenderFrame();

    if (surfaceList_.empty()) {
        return false;
    }

    if (!drContext_) {
        LOGE("RSSurfaceOhosVulkan: FlushFrame mSkContext is nullptr");
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

    auto& surface = surfaceMap_[surfaceList_.front()];
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
    surface.drawingSurface->Flush(&drawingFlushInfo);
    drContext_->Submit();
#endif

    int fenceFd = -1;

    auto queue = vkContext.GetQueue();
    if (vkContext.GetHardWareGrContext().get() == drContext_.get()) {
        queue = vkContext.GetHardwareQueue();
    }
    auto err = RsVulkanContext::HookedVkQueueSignalReleaseImageOHOS(
        queue, 1, &semaphore, surface.image, &fenceFd);
    if (err != VK_SUCCESS) {
        LOGE("RSSurfaceOhosVulkan QueueSignalReleaseImageOHOS failed %{public}d", err);
        return false;
    }

    auto ret = NativeWindowFlushBuffer(surface.window, surface.nativeWindowBuffer, fenceFd, {});
    if (ret != OHOS::GSERROR_OK) {
        LOGE("RSSurfaceOhosVulkan NativeWindowFlushBuffer failed");
        return false;
    }
    surfaceList_.pop_front();
    vkContext.vkDestroySemaphore(vkContext.GetDevice(), semaphore, nullptr);
    surface.fence.reset();
    return true;
}

Drawing::Canvas* SurfaceOhosVulkan::GetCanvas(std::unique_ptr<SurfaceFrame>& frame)
{
    if (drawingProxy_ == nullptr) {
        LOGE("drawingProxy_ is nullptr, can not GetCanvas");
        return nullptr;
    }
    return drawingProxy_->AcquireDrCanvas(frame);
}
SkCanvas* SurfaceOhosVulkan::GetSkCanvas(std::unique_ptr<SurfaceFrame>& frame)
{
    if (drawingProxy_ == nullptr) {
        LOGE("drawingProxy_ is nullptr, can not GetCanvas");
        return nullptr;
    }
    return drawingProxy_->AcquireSkCanvas(frame);
}
} // namespace Rosen
} // namespace OHOS
