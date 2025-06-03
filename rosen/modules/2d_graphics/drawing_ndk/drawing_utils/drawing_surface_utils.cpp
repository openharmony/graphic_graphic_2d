/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "drawing_surface_utils.h"

#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/native_buffer_utils.h"
#include "sync_fence.h"
#ifdef USE_M133_SKIA
#include "include/gpu/ganesh/vk/GrVkBackendSemaphore.h"
#endif
#endif

#include "utils/log.h"
#include "utils/system_properties.h"

namespace OHOS {
namespace Rosen {

static std::mutex g_surfaceMutex;
static std::unordered_map<Drawing::Surface*, std::pair<std::shared_ptr<Drawing::Surface>, EGLSurface>> g_eglSurfaceMap;
#ifdef RS_ENABLE_VK
static std::unordered_map<Drawing::Surface*, std::tuple<std::shared_ptr<Drawing::Surface>, Drawing::GPUContext*,
    NativeBufferUtils::NativeSurfaceInfo*>> g_vulkanSurfaceMap_;
#endif

int ColorTypeToGLFormat(Drawing::ColorType colorType)
{
    switch (colorType) {
        case Drawing::ColorType::COLORTYPE_RGBA_8888:
            return GL_RGBA8;
        case Drawing::ColorType::COLORTYPE_ALPHA_8:
            return GL_R8;
        case Drawing::ColorType::COLORTYPE_RGB_565:
            return GL_RGB565;
        case Drawing::ColorType::COLORTYPE_ARGB_4444:
            return GL_RGBA4;
        default:
            return GL_RGBA8;
    }
}

#ifdef RS_ENABLE_VK
void CreateVkSemaphore(VkSemaphore& semaphore, RsVulkanContext& vkContext,
    NativeBufferUtils::NativeSurfaceInfo& nativeSurface)
{
    VkSemaphoreCreateInfo semaphoreInfo;
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreInfo.pNext = nullptr;
    semaphoreInfo.flags = 0;
    auto& vkInterface = vkContext.GetRsVulkanInterface();
    auto res = vkInterface.vkCreateSemaphore(vkInterface.GetDevice(), &semaphoreInfo, nullptr, &semaphore);
    if (res != VK_SUCCESS) {
        LOGE("DrawingSurfaceUtils: CreateVkSemaphore vkCreateSemaphore failed %{public}d", res);
        semaphore = VK_NULL_HANDLE;
        nativeSurface.fence->Wait(-1);
        return;
    }

    VkImportSemaphoreFdInfoKHR importSemaphoreFdInfo;
    importSemaphoreFdInfo.sType = VK_STRUCTURE_TYPE_IMPORT_SEMAPHORE_FD_INFO_KHR;
    importSemaphoreFdInfo.pNext = nullptr;
    importSemaphoreFdInfo.semaphore = semaphore;
    importSemaphoreFdInfo.flags = VK_SEMAPHORE_IMPORT_TEMPORARY_BIT;
    importSemaphoreFdInfo.handleType = VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_SYNC_FD_BIT;
    importSemaphoreFdInfo.fd = nativeSurface.fence->Dup();
    res = vkInterface.vkImportSemaphoreFdKHR(vkInterface.GetDevice(), &importSemaphoreFdInfo);
    if (res != VK_SUCCESS) {
        LOGE("DrawingSurfaceUtils: CreateVkSemaphore vkImportSemaphoreFdKHR failed %{public}d", res);
        vkInterface.vkDestroySemaphore(vkInterface.GetDevice(), semaphore, nullptr);
        semaphore = VK_NULL_HANDLE;
        close(importSemaphoreFdInfo.fd);
        nativeSurface.fence->Wait(-1);
    }
}

std::shared_ptr<Drawing::Surface> CreateVulkanWindowSurface(Drawing::GPUContext* gpuContext,
    const Drawing::ImageInfo& imageInfo, void* window)
{
    int fenceFd = -1;
    NativeWindowBuffer* nativeWindowBuffer = nullptr;
    OHNativeWindow *nativeWindow = reinterpret_cast<OHNativeWindow *>(window);
    auto res = NativeWindowRequestBuffer(nativeWindow, &nativeWindowBuffer, &fenceFd);
    if (res != OHOS::GSERROR_OK) {
        LOGE("CreateVulkanWindowSurface: RequestBuffer failed %{public}d", res);
        NativeWindowCancelBuffer(nativeWindow, nativeWindowBuffer);
        return nullptr;
    }

    NativeBufferUtils::NativeSurfaceInfo* nativeSurface = new(std::nothrow) NativeBufferUtils::NativeSurfaceInfo();
    if (nativeSurface == nullptr) {
        LOGE("CreateVulkanWindowSurface: create nativeSurface failed!");
        NativeWindowCancelBuffer(nativeWindow, nativeWindowBuffer);
        if (fenceFd != -1) {
            close(fenceFd);
            fenceFd = -1;
        }
        return nullptr;
    }

    NativeObjectReference(nativeWindow);
    nativeSurface->window = nativeWindow;
    NativeObjectReference(nativeWindowBuffer);
    nativeSurface->nativeWindowBuffer = nativeWindowBuffer;
    std::shared_ptr<Drawing::Surface> surface = NativeBufferUtils::CreateFromNativeWindowBuffer(
        gpuContext, imageInfo, *nativeSurface);
    if (!surface) {
        LOGE("CreateVulkanWindowSurface: create surface failed!");
        NativeWindowCancelBuffer(nativeWindow, nativeWindowBuffer);
        if (fenceFd != -1) {
            close(fenceFd);
            fenceFd = -1;
        }
        delete nativeSurface;
        return nullptr;
    }
    surface->ClearDrawingArea();

    if (fenceFd >= 0) {
        nativeSurface->fence = std::make_unique<SyncFence>(fenceFd);
        auto status = nativeSurface->fence->GetStatus();
        if (status != SIGNALED) {
            auto& vkContext = RsVulkanContext::GetSingleton();
            VkSemaphore semaphore = VK_NULL_HANDLE;
            CreateVkSemaphore(semaphore, vkContext, *nativeSurface);
            if (semaphore != VK_NULL_HANDLE) {
                surface->Wait(1, semaphore);
            }
        }
    }

    std::lock_guard<std::mutex> lock(g_surfaceMutex);
    g_vulkanSurfaceMap_.insert({surface.get(), std::make_tuple(surface, gpuContext, nativeSurface)});
    return surface;
}

bool FlushVulkanSurface(Drawing::Surface* surface)
{
    std::lock_guard<std::mutex> lock(g_surfaceMutex);
    auto iter = g_vulkanSurfaceMap_.find(surface);
    if (iter == g_vulkanSurfaceMap_.end()) {
        LOGE("FlushVulkanSurface: Can not find surface, surface is valid!");
        return false;
    }

    auto nativeSurface = std::get<2>(iter->second);
    Drawing::GPUContext* gpuContext = std::get<1>(iter->second);
    if (nativeSurface == nullptr) {
        surface->FlushAndSubmit();
        return true;
    }

    auto& vkContext = RsVulkanContext::GetSingleton().GetRsVulkanInterface();
    VkSemaphore semaphore = vkContext.RequireSemaphore();

    GrBackendSemaphore backendSemaphore;
    backendSemaphore.initVulkan(semaphore);

    auto* callbackInfo = new RsVulkanInterface::CallbackSemaphoreInfo(vkContext, semaphore, -1);

    Drawing::FlushInfo drawingFlushInfo;
    drawingFlushInfo.backendSurfaceAccess = true;
    drawingFlushInfo.numSemaphores = 1;
    drawingFlushInfo.backendSemaphore = static_cast<void*>(&backendSemaphore);
    drawingFlushInfo.finishedProc = [](void *context) {
        RsVulkanInterface::CallbackSemaphoreInfo::DestroyCallbackRefs(context);
    };
    drawingFlushInfo.finishedContext = callbackInfo;
    {
        surface->Flush(&drawingFlushInfo);
        gpuContext->Submit();
    }
    
    int fenceFd = -1;
    auto queue = vkContext.GetQueue();
    auto err = RsVulkanContext::HookedVkQueueSignalReleaseImageOHOS(
        queue, 1, &semaphore, nativeSurface->image, &fenceFd);
    if (err != VK_SUCCESS) {
        if (err == VK_ERROR_DEVICE_LOST) {
            vkContext.DestroyAllSemaphoreFence();
        }
        RsVulkanInterface::CallbackSemaphoreInfo::DestroyCallbackRefs(callbackInfo);
        callbackInfo = nullptr;
        LOGE("FlushVulkanSurface: QueueSignalReleaseImageOHOS failed %{public}d", err);
        return false;
    }
    callbackInfo->mFenceFd = ::dup(fenceFd);

    auto ret = NativeWindowFlushBuffer(nativeSurface->window, nativeSurface->nativeWindowBuffer, fenceFd, {});
    RsVulkanInterface::CallbackSemaphoreInfo::DestroyCallbackRefs(callbackInfo);
    callbackInfo = nullptr;
    if (ret != OHOS::GSERROR_OK) {
        LOGE("FlushVulkanSurface NativeWindowFlushBuffer failed");
        return false;
    }
    nativeSurface->fence.reset();
    return true;
}
#endif

std::shared_ptr<Drawing::Surface> DrawingSurfaceUtils::CreateFromWindow(Drawing::GPUContext* gpuContext,
    const Drawing::ImageInfo& imageInfo, void* window)
{
    if (Drawing::SystemProperties::IsUseGl()) {
        std::shared_ptr<Drawing::GPUContext> context = DrawingGpuContextManager::GetInstance().Find(gpuContext);
        if (context == nullptr) {
            LOGE("CreateFromWindow: gpuContext is invalid!");
            return nullptr;
        }

        std::shared_ptr<RenderContext> renderContext = DrawingGpuContextManager::GetInstance().GetRenderContext();
        if (renderContext == nullptr) {
            LOGE("CreateFromWindow: get renderContext failed.");
            return nullptr;
        }

        EGLSurface eglSurface = renderContext->CreateEGLSurface(reinterpret_cast<EGLNativeWindowType>(window));
        if (eglSurface == EGL_NO_SURFACE) {
            LOGE("CreateFromWindow: create eglSurface failed, window is invalid.");
            return nullptr;
        }
        renderContext->MakeCurrent(eglSurface);

        Drawing::FrameBuffer bufferInfo;
        bufferInfo.width = imageInfo.GetWidth();
        bufferInfo.height = imageInfo.GetHeight();
        bufferInfo.FBOID = 0;
        bufferInfo.Format = ColorTypeToGLFormat(imageInfo.GetColorType());
        bufferInfo.gpuContext = context;
        bufferInfo.colorSpace = Drawing::ColorSpace::CreateSRGB();
        bufferInfo.colorType = imageInfo.GetColorType();
        std::shared_ptr<Drawing::Surface> surface = std::make_shared<Drawing::Surface>();
        if (!surface->Bind(bufferInfo)) {
            LOGE("CreateFromWindow: create surface failed.");
            renderContext->DestroyEGLSurface(eglSurface);
            renderContext->MakeCurrent(EGL_NO_SURFACE);
            return nullptr;
        }

        std::lock_guard<std::mutex> lock(g_surfaceMutex);
        g_eglSurfaceMap.insert({surface.get(), std::make_pair(surface, eglSurface)});
        return surface;
    }

#ifdef RS_ENABLE_VK
    if (Drawing::SystemProperties::IsUseVulkan()) {
        return CreateVulkanWindowSurface(gpuContext, imageInfo, window);
    }
#endif
    return nullptr;
}

bool DrawingSurfaceUtils::FlushSurface(Drawing::Surface* surface)
{
    if (Drawing::SystemProperties::IsUseGl()) {
        if (surface != nullptr && surface->GetCanvas() != nullptr) {
            surface->GetCanvas()->Flush();
        }

        std::shared_ptr<RenderContext> renderContext = DrawingGpuContextManager::GetInstance().GetRenderContext();
        if (renderContext == nullptr) {
            LOGE("FlushSurface: get renderContext failed.");
            return false;
        }

        std::lock_guard<std::mutex> lock(g_surfaceMutex);
        auto iter = g_eglSurfaceMap.find(surface);
        if (iter == g_eglSurfaceMap.end()) {
            LOGE("FlushSurface: surface is invalid!");
            return false;
        }
        
        EGLSurface eglSurface = (iter->second).second;
        if (eglSurface != EGL_NO_SURFACE) {
            renderContext->SwapBuffers(eglSurface);
        }
        return true;
    }

#ifdef RS_ENABLE_VK
    if (Drawing::SystemProperties::IsUseVulkan()) {
        return FlushVulkanSurface(surface);
    }
#endif
    return false;
}

void DrawingSurfaceUtils::InsertSurface(std::shared_ptr<Drawing::Surface> surface, Drawing::GPUContext* gpuContext)
{
    std::lock_guard<std::mutex> lock(g_surfaceMutex);
    if (Drawing::SystemProperties::IsUseGl()) {
        g_eglSurfaceMap.insert({surface.get(), std::make_pair(surface, EGL_NO_SURFACE)});
    }

#ifdef RS_ENABLE_VK
    if (Drawing::SystemProperties::IsUseVulkan()) {
        g_vulkanSurfaceMap_.insert({surface.get(), std::make_tuple(surface, gpuContext, nullptr)});
    }
#endif
}

void DrawingSurfaceUtils::RemoveSurface(Drawing::Surface* surface)
{
    std::lock_guard<std::mutex> lock(g_surfaceMutex);
    if (Drawing::SystemProperties::IsUseGl()) {
        auto iter = g_eglSurfaceMap.find(surface);
        if (iter == g_eglSurfaceMap.end()) {
            return;
        }
        g_eglSurfaceMap.erase(iter);
    }

#ifdef RS_ENABLE_VK
    if (Drawing::SystemProperties::IsUseVulkan()) {
        auto iter = g_vulkanSurfaceMap_.find(surface);
        if (iter == g_vulkanSurfaceMap_.end()) {
            return;
        }

        auto nativeSurface = std::get<2>(iter->second);
        g_vulkanSurfaceMap_.erase(iter);
        if (nativeSurface != nullptr) {
            delete nativeSurface;
            nativeSurface = nullptr;
        }
    }
#endif
}
} // namespace Rosen
} // namespace OHOS