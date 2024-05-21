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

#ifndef RS_SURFACE_OHOS_VULKAN_H
#define RS_SURFACE_OHOS_VULKAN_H

#include <cstdint>

#include <list>
#include <unordered_map>
#include "native_window.h"
#include "vulkan/vulkan_core.h"
#include "platform/ohos/backend/rs_vulkan_context.h"
#include "sync_fence.h"
#include "native_buffer_utils.h"
#include "image/image.h"
#include "platform/ohos/rs_surface_ohos.h"
#include "rs_surface_frame_ohos_vulkan.h"
#include <surface.h>

namespace OHOS {
namespace Rosen {
struct DestroySemaphoreInfo {
    PFN_vkDestroySemaphore mDestroyFunction;
    VkDevice mDevice;
    VkSemaphore mSemaphore;

    int mRefs = 2;
    DestroySemaphoreInfo(PFN_vkDestroySemaphore destroyFunction, VkDevice device,
                        VkSemaphore semaphore)
        : mDestroyFunction(destroyFunction), mDevice(device), mSemaphore(semaphore) {}

    static void DestroySemaphore(void *context)
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
};

class RSSurfaceOhosVulkan : public RSSurfaceOhos {
public:
    explicit RSSurfaceOhosVulkan(const sptr<Surface>& producer);
    ~RSSurfaceOhosVulkan() override;

    bool IsValid() const override
    {
        return producer_ != nullptr;
    }

    std::unique_ptr<RSSurfaceFrame> RequestFrame(
        int32_t width, int32_t height, uint64_t uiTimestamp, bool useAFBC = true, bool isProtected = false) override;
    bool FlushFrame(std::unique_ptr<RSSurfaceFrame>& frame, uint64_t uiTimestamp) override;
    void SetColorSpace(GraphicColorGamut colorSpace) override;
    void SetSurfaceBufferUsage(uint64_t usage) override;
    void SetSurfacePixelFormat(int32_t pixelFormat) override;
    sptr<SurfaceBuffer> GetCurrentBuffer() override;
    void ClearBuffer() override;
    void ResetBufferAge() override;
    void SetUiTimeStamp(const std::unique_ptr<RSSurfaceFrame>& frame, uint64_t uiTimestamp) override;
    void SetSkContext(std::shared_ptr<Drawing::GPUContext> skContext)
    {
        mSkContext = skContext;
    }
private:
    struct NativeWindow* mNativeWindow = nullptr;
    int mWidth = -1;
    int mHeight = -1;
    void SetNativeWindowInfo(int32_t width, int32_t height, bool useAFBC, bool isProtected = false);
    int32_t mPresentCount = 0;
    std::list<NativeWindowBuffer*> mSurfaceList;
    std::unordered_map<NativeWindowBuffer*, NativeBufferUtils::NativeSurfaceInfo> mSurfaceMap;
    std::shared_ptr<Drawing::GPUContext> mSkContext = nullptr;
    int32_t RequestNativeWindowBuffer(NativeWindowBuffer** nativeWindowBuffer,
        int32_t width, int32_t height, int& fenceFd, bool useAFBC, bool isProtected = false);
    void CreateVkSemaphore(VkSemaphore* semaphore,
        RsVulkanContext& vkContext, NativeBufferUtils::NativeSurfaceInfo& nativeSurface);
};

} // namespace Rosen
} // namespace OHOS

#endif // RS_SURFACE_OHOS_VULKAN_H
