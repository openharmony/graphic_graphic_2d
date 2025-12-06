/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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

#include "common/rs_common_def.h"
#include "draw/surface.h"
#include "native_window.h"
#include "pipeline/render_thread/rs_base_render_util.h"
#include "platform/ohos/backend/native_buffer_utils.h"
#include "platform/ohos/backend/rs_vulkan_context.h"
#include "surface.h"
#include "sync_fence.h"
#include "vulkan/vulkan_core.h"

namespace OHOS {
namespace Rosen {

using BufferInfoCache = struct {
    uint64_t bufferId;
    int32_t width;
    int32_t height;
    int32_t fd;
    uint32_t size;
    BufferDeletedFlag bufferDeletedFlag;
};

class VkImageResource {
public:
    static std::shared_ptr<VkImageResource> Create(sptr<OHOS::SurfaceBuffer> buffer);

    VkImageResource(NativeWindowBuffer* nativeWindowBuffer, Drawing::BackendTexture backendTexture,
        NativeBufferUtils::VulkanCleanupHelper* vulkanCleanupHelper)
        : mNativeWindowBuffer(nativeWindowBuffer), mBackendTexture_(backendTexture),
          mVulkanCleanupHelper(vulkanCleanupHelper) {}
    ~VkImageResource();

    const Drawing::BackendTexture& GetBackendTexture() const
    {
        return mBackendTexture_;
    }

    NativeBufferUtils::VulkanCleanupHelper* RefCleanupHelper()
    {
        return mVulkanCleanupHelper->Ref();
    }

    pid_t GetThreadIndex() const
    {
        return threadIndex_;
    }

    void SetThreadIndex(const pid_t threadIndex = UNI_RENDER_THREAD_INDEX)
    {
        threadIndex_ = threadIndex;
    }

    void SetBufferInfoCache(const BufferInfoCache& bufferInfoCache)
    {
        bufferInfoCache_ = bufferInfoCache;
    }

    BufferInfoCache GetBufferInfoCache() const
    {
        return bufferInfoCache_;
    }

private:
    NativeWindowBuffer* mNativeWindowBuffer;
    Drawing::BackendTexture mBackendTexture_;
    NativeBufferUtils::VulkanCleanupHelper* mVulkanCleanupHelper;
    pid_t threadIndex_ = UNI_RENDER_THREAD_INDEX;
    BufferInfoCache bufferInfoCache_{};
};

class RSVkImageManager : public RSImageManager {
public:
    RSVkImageManager() = default;
    ~RSVkImageManager() noexcept override = default;

    void UnMapImageFromSurfaceBuffer(uint64_t seqNum) override;
    std::shared_ptr<Drawing::Image> CreateImageFromBuffer(
        RSPaintFilterCanvas& canvas, const BufferDrawParam& params,
        const std::shared_ptr<Drawing::ColorSpace>& drawingColorSpace) override;
    std::shared_ptr<Drawing::Image> GetIntersectImage(Drawing::RectI& imgCutRect,
        const std::shared_ptr<Drawing::GPUContext>& context, const BufferDrawParam& params) override;

    void DumpVkImageInfo(std::string &dumpString) override;
private:
    std::shared_ptr<VkImageResource> MapVkImageFromSurfaceBuffer(
        sptr<OHOS::SurfaceBuffer> buffer, const sptr<SyncFence>& acquireFence,
        pid_t threadIndex, Drawing::Surface *drawingSurface = nullptr);
    std::shared_ptr<VkImageResource> CreateImageCacheFromBuffer(const sptr<OHOS::SurfaceBuffer> buffer,
        const sptr<SyncFence>& acquireFence) ;
    std::shared_ptr<VkImageResource> NewImageCacheFromBuffer(
        const sptr<OHOS::SurfaceBuffer>& buffer, pid_t threadIndex, bool isProtectedCondition);
    bool WaitVKSemaphore(Drawing::Surface *drawingSurface, const sptr<SyncFence>& acquireFence);

    std::unordered_map<uint64_t, std::shared_ptr<VkImageResource>> imageCacheSeqs_; // guarded by opMutex_
};

} // namespace Rosen
} // namespace OHOS
#endif // RS_VK_IMAGE_MANAGER_H