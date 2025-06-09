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

#ifndef RS_IMAGE_MANAGER_H
#define RS_IMAGE_MANAGER_H

#include <memory>
#include <mutex>
#include <queue>
#include <variant>
#include <unordered_map>
#include <surface.h>

#ifdef RS_ENABLE_VK
#include "native_window.h"
#include "platform/ohos/backend/rs_vulkan_context.h"
#include "common/rs_common_def.h"
#include "draw/surface.h"
#include "platform/ohos/backend/native_buffer_utils.h"
#endif // RS_ENABLE_VK

#include "EGL/egl.h"
#include "EGL/eglext.h"
#include "GLES/gl.h"
#include "sync_fence.h"

namespace OHOS {
namespace Rosen {
class ImageResource {
public:
    static std::shared_ptr<ImageResource> EglCreate(
        EGLDisplay eglDisplay, EGLImageKHR eglImage, EGLClientBuffer eglClientBuffer);
#ifdef RS_ENABLE_VK
    static std::shared_ptr<ImageResource> VkCreate(
        NativeWindowBuffer* nativeWindowBuffer, Drawing::BackendTexture backendTexture,
        NativeBufferUtils::VulkanCleanupHelper* vulkanCleanupHelper);
#endif // RS_ENABLE_VK
    virtual ~ImageResource() = default;

    pid_t GetThreadIndex() const
    {
        return threadIndex_;
    }

    void SetThreadIndex(const pid_t threadIndex)
    {
        threadIndex_ = threadIndex;
    }

#ifdef RS_ENABLE_VK
    // Vulkan virtual functions
    virtual void SetBufferDeleteFromCacheFlag(const bool &flag) { return;}
    virtual bool GetBufferDeleteFromCacheFlag() const { return false; }
    virtual const Drawing::BackendTexture& GetBackendTexture() const { return mBackendTexture_; }
    virtual NativeBufferUtils::VulkanCleanupHelper* RefCleanupHelper() { return nullptr; }
#endif // RS_ENABLE_VK

    // EGL virtual functions
    virtual GLuint GetTextureId() const { return 0; }
    virtual void SetTextureId(GLuint textureId) { return; }
    virtual bool BindToTexture() { return false; }

    pid_t threadIndex_;
protected:
    ImageResource() = default;

#ifdef RS_ENABLE_VK
    // Vulkan specific members
    NativeWindowBuffer* mNativeWindowBuffer;
    Drawing::BackendTexture mBackendTexture_;
    NativeBufferUtils::VulkanCleanupHelper* mVulkanCleanupHelper;
    bool isBufferDeleteFromCache = false;
#endif // RS_ENABLE_VK

    // EGL specific members
    EGLDisplay eglDisplay_ = EGL_NO_DISPLAY;
    EGLImageKHR eglImage_ = EGL_NO_IMAGE_KHR;
    EGLClientBuffer eglClientBuffer_ = nullptr;
    GLuint textureId_ = 0;
};

class RSImageManager {
public:
    static std::shared_ptr<RSImageManager> Create();

    virtual ~RSImageManager() = default;

    virtual void UnMapImageFromSurfaceBuffer(int32_t seqNum) = 0;
    virtual std::shared_ptr<ImageResource> CreateImageCacheFromBuffer(const sptr<OHOS::SurfaceBuffer>& buffer,
        const sptr<SyncFence>& acquireFence) = 0;

    // Vulkan specific functions
#ifdef RS_ENABLE_VK
    virtual std::shared_ptr<ImageResource> MapVkImageFromSurfaceBuffer(
        const sptr<OHOS::SurfaceBuffer>& buffer, const sptr<SyncFence>& acquireFence,
        pid_t threadIndex, Drawing::Surface *drawingSurface = nullptr) { return nullptr; }
    virtual void DumpVkImageInfo(std::string &dumpString) { return; }
#endif // RS_ENABLE_VK

    // EGL specific functions
    virtual GLuint MapEglImageFromSurfaceBuffer(const sptr<OHOS::SurfaceBuffer>& buffer,
        const sptr<SyncFence>& acquireFence, pid_t threadIndex) { return 0; }
    virtual void UnMapEglImageFromSurfaceBufferForUniRedraw(int32_t seqNum) { return; }
    virtual void ShrinkCachesIfNeeded(bool isForUniRedraw = false) { return; }
protected:
    RSImageManager() = default;
    mutable std::mutex opMutex_;
    std::unordered_map<int32_t, std::shared_ptr<ImageResource>> imageCacheSeqs_;

    // Egl specific members
    std::queue<int32_t> cacheQueue_; // fifo, size restricted by MAX_CACHE_SIZE
};
} // namespace Rosen
} // namespace OHOS

#endif // RS_IMAGE_MANAGER_H
