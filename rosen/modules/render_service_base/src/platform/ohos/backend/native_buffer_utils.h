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

#ifndef NATIVE_BUFFER_UTILS
#define NATIVE_BUFFER_UTILS

#include <atomic>

#include "native_buffer_inner.h"
#include "sync_fence.h"
#include "rs_vulkan_context.h"
#include "native_window.h"
#include "include/gpu/GrDirectContext.h"
#include "include/gpu/GrBackendSemaphore.h"
#include "include/core/SkSurface.h"
#ifdef USE_ROSEN_DRAWING
#include "draw/surface.h"
#include "image/image.h"
#include "drawing/engine_adapter/skia_adapater/skia_color_space.h"
#endif

namespace OHOS::Rosen {
namespace NativeBufferUtils {
void DeleteVkImage(void* context);
class VulkanCleanupHelper {
public:
    VulkanCleanupHelper(RsVulkanContext& vkContext, VkImage image, VkDeviceMemory memory)
        : fDevice_(vkContext.GetDevice()),
          fImage_(image),
          fMemory_(memory),
          fDestroyImage_(vkContext.vkDestroyImage),
          fFreeMemory_(vkContext.vkFreeMemory),
          fRefCnt_(1) {}
    ~VulkanCleanupHelper()
    {
        fDestroyImage_(fDevice_, fImage_, nullptr);
        fFreeMemory_(fDevice_, fMemory_, nullptr);
    }

    VulkanCleanupHelper* Ref()
    {
        (void)fRefCnt_.fetch_add(+1, std::memory_order_relaxed);
        return this;
    }

    void UnRef()
    {
        if (fRefCnt_.fetch_add(-1, std::memory_order_acq_rel) == 1) {
            delete this;
        }
    }

private:
    VkDevice fDevice_;
    VkImage fImage_;
    VkDeviceMemory fMemory_;
    PFN_vkDestroyImage fDestroyImage_;
    PFN_vkFreeMemory fFreeMemory_;
    mutable std::atomic<int32_t> fRefCnt_;
};

struct NativeSurfaceInfo {
    NativeSurfaceInfo() = default;
    NativeSurfaceInfo(NativeSurfaceInfo &&) = default;
    NativeSurfaceInfo &operator=(NativeSurfaceInfo &&) = default;
    NativeSurfaceInfo(const NativeSurfaceInfo &) = delete;
    NativeSurfaceInfo &operator=(const NativeSurfaceInfo &) = delete;

    NativeWindow* window = nullptr;
    NativeWindowBuffer* nativeWindowBuffer = nullptr;
    VkImage image = VK_NULL_HANDLE; // skia will destroy image
    std::unique_ptr<SyncFence> fence = nullptr;
#ifndef USE_ROSEN_DRAWING
    sk_sp<SkSurface> skSurface = nullptr;
#else
    std::shared_ptr<Drawing::Surface> drawingSurface = nullptr;
#endif
    int32_t lastPresentedCount = -1;
    GraphicColorGamut graphicColorGamut = GRAPHIC_COLOR_GAMUT_INVALID;
    ~NativeSurfaceInfo()
    {
#ifndef USE_ROSEN_DRAWING
        skSurface = nullptr;
#else
        drawingSurface = nullptr;
#endif
        if (window != nullptr) {
            NativeObjectUnreference(window);
            window = nullptr;
        }
        if (nativeWindowBuffer != nullptr) {
            NativeObjectUnreference(nativeWindowBuffer);
            nativeWindowBuffer = nullptr;
        }
    }
};

#ifndef USE_ROSEN_DRAWING
bool MakeFromNativeWindowBuffer(sk_sp<GrDirectContext> skContext, NativeWindowBuffer* nativeWindowBuffer,
    NativeSurfaceInfo& nativeSurface, int width, int height);

GrBackendTexture MakeBackendTextureFromNativeBuffer(NativeWindowBuffer* nativeWindowBuffer,
    int width, int height);
#else
bool MakeFromNativeWindowBuffer(std::shared_ptr<Drawing::GPUContext> skContext, NativeWindowBuffer* nativeWindowBuffer,
    NativeSurfaceInfo& nativeSurface, int width, int height);

Drawing::BackendTexture MakeBackendTextureFromNativeBuffer(NativeWindowBuffer* nativeWindowBuffer,
    int width, int height);
#endif
}
} // OHOS::Rosen
#endif