/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef RS_EGL_IMAGE_MANAGER_H
#define RS_EGL_IMAGE_MANAGER_H

#include <memory>
#include <mutex>
#include <queue>
#include <unordered_map>

#include <surface.h>
#include "EGL/egl.h"
#include "EGL/eglext.h"
#include "GLES/gl.h"
#include "GLES/glext.h"
#include "GLES3/gl32.h"
#include "sync_fence.h"
#include "pipeline/rs_context.h"

namespace OHOS {
namespace Rosen {
class ImageCacheSeq {
public:
    static std::unique_ptr<ImageCacheSeq> Create(
        EGLDisplay eglDisplay,
        EGLContext eglContext,
        const sptr<OHOS::SurfaceBuffer>& buffer);

    ImageCacheSeq(
        EGLDisplay eglDisplay,
        EGLImageKHR eglImage,
        EGLClientBuffer eglClientBuffer);
    ~ImageCacheSeq() noexcept;

    GLuint TextureId() const
    {
        return textureId_;
    }

    pid_t GetThreadIndex() const
    {
        return threadIndex_;
    }

    void SetThreadIndex(const pid_t threadIndex)
    {
        threadIndex_ = threadIndex;
    }
private:
    // generate a texture and bind eglImage to it.
    bool BindToTexture();

    EGLDisplay eglDisplay_ = EGL_NO_DISPLAY;
    EGLImageKHR eglImage_ = EGL_NO_IMAGE_KHR;
    EGLClientBuffer eglClientBuffer_ = nullptr;
    GLuint textureId_ = 0;
    pid_t threadIndex_ = 0;
};

class RSEglImageManager {
public:
    explicit RSEglImageManager(EGLDisplay display);
    ~RSEglImageManager() noexcept = default;

    GLuint MapEglImageFromSurfaceBuffer(const sptr<OHOS::SurfaceBuffer>& buffer,
        const sptr<SyncFence>& acquireFence, pid_t threadIndex);
    void UnMapEglImageFromSurfaceBuffer(int32_t seqNum);
    void UnMapEglImageFromSurfaceBufferForUniRedraw(int32_t seqNum);
    void ShrinkCachesIfNeeded(bool isForUniRedraw = false); // only used for divided_render
    std::unique_ptr<ImageCacheSeq> CreateImageCacheFromBuffer(const sptr<OHOS::SurfaceBuffer>& buffer,
        const sptr<SyncFence>& acquireFence);
private:
    void WaitAcquireFence(const sptr<SyncFence>& acquireFence);
    GLuint CreateImageCacheFromBuffer(const sptr<OHOS::SurfaceBuffer>& buffer,
        const pid_t threadIndex);

    mutable std::mutex opMutex_;
    static constexpr size_t MAX_CACHE_SIZE = 16;
    std::queue<int32_t> cacheQueue_; // fifo, size restricted by MAX_CACHE_SIZE
    std::unordered_map<int32_t, std::unique_ptr<ImageCacheSeq>> imageCacheSeqs_; // guarded by opMutex_.
    EGLDisplay eglDisplay_ = EGL_NO_DISPLAY;
};
} // namespace Rosen
} // namespace OHOS

#endif // RS_EGL_IMAGE_MANAGER_H
