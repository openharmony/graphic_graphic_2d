/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#ifndef RS_VK_IMAGE_MANAGER_H
#define RS_VK_IMAGE_MANAGER_H

#include <memory>
#include <mutex>
#include <queue>
#include <unordered_map>

#include "rs_image_manager.h"

#include "surface.h"
#include "sync_fence.h"
#include "vulkan/vulkan_core.h"

namespace OHOS {
namespace Rosen {

class VkImageResource : public ImageResource {
public:
    static std::shared_ptr<ImageResource> Create(sptr<OHOS::SurfaceBuffer> buffer);

    VkImageResource(NativeWindowBuffer* nativeWindowBuffer, Drawing::BackendTexture backendTexture,
        NativeBufferUtils::VulkanCleanupHelper* vulkanCleanupHelper)
        : mNativeWindowBuffer(nativeWindowBuffer), mBackendTexture_(backendTexture),
          mVulkanCleanupHelper(vulkanCleanupHelper) {}
    ~VkImageResource() override;

    const Drawing::BackendTexture& GetBackendTexture() const override
    {
        return mBackendTexture_;
    }

    NativeBufferUtils::VulkanCleanupHelper* RefCleanupHelper() override
    {
        return mVulkanCleanupHelper->Ref();
    }

    void SetBufferDeleteFromCacheFlag(const bool& flag)
    {
        isBufferDeleteFromCache = flag;
    }

    bool GetBufferDeleteFromCacheFlag() const
    {
        return isBufferDeleteFromCache;
    }

private:
    NativeWindowBuffer* mNativeWindowBuffer;
    Drawing::BackendTexture mBackendTexture_;
    NativeBufferUtils::VulkanCleanupHelper* mVulkanCleanupHelper;
    bool isBufferDeleteFromCache = false;
};

class RSVkImageManager : public RSImageManager {
public:
    RSVkImageManager() = default;
    ~RSVkImageManager() noexcept override = default;

    void UnMapImageFromSurfaceBuffer(int32_t seqNum) override;
    std::shared_ptr<Drawing::Image> CreateImageFromBuffer(
        RSPaintFilterCanvas& canvas, const sptr<SurfaceBuffer>& buffer, const sptr<SyncFence>& acquireFence,
        const uint32_t threadIndex, const std::shared_ptr<Drawing::ColorSpace>& drawingColorSpace) override;

    std::shared_ptr<ImageResource> MapVkImageFromSurfaceBuffer(
        const sptr<OHOS::SurfaceBuffer>& buffer, const sptr<SyncFence>& acquireFence,
        pid_t threadIndex, Drawing::Surface *drawingSurface = nullptr);
    std::shared_ptr<ImageResource> CreateImageCacheFromBuffer(const sptr<OHOS::SurfaceBuffer> buffer,
        const sptr<SyncFence>& acquireFence) override;
    void DumpVkImageInfo(std::string &dumpString) override;
private:
    std::shared_ptr<ImageResource> NewImageCacheFromBuffer(
        const sptr<OHOS::SurfaceBuffer>& buffer, pid_t threadIndex, bool isProtectedCondition);
    bool WaitVKSemaphore(Drawing::Surface *drawingSurface, const sptr<SyncFence>& acquireFence);

    std::unordered_map<int32_t, std::shared_ptr<ImageResource>> imageCacheSeqs_; // guarded by opMutex_
};

} // namespace Rosen
} // namespace OHOS
#endif // RS_VK_IMAGE_MANAGER_H