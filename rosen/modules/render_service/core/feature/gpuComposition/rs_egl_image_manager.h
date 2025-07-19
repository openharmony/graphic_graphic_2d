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

#ifndef RS_EGL_IMAGE_MANAGER_H
#define RS_EGL_IMAGE_MANAGER_H

#include <memory>
#include <mutex>
#include <queue>
#include <unordered_map>

#include "rs_image_manager.h"

#include "EGL/egl.h"
#include "EGL/eglext.h"
#include "GLES/gl.h"
#include "GLES/glext.h"
#include "GLES3/gl32.h"
#include "pipeline/render_thread/rs_base_render_util.h"
#include "sync_fence.h"

namespace OHOS {
namespace Rosen {
class EglImageResource {
public:
    static std::unique_ptr<EglImageResource> Create(EGLDisplay eglDisplay, EGLContext eglContext,
        const sptr<OHOS::SurfaceBuffer>& buffer);

    EglImageResource(EGLDisplay eglDisplay, EGLImageKHR eglImage, EGLClientBuffer eglClientBuffer)
        : eglDisplay_(eglDisplay), eglImage_(eglImage), eglClientBuffer_(eglClientBuffer) {}
    ~EglImageResource() noexcept;

    GLuint GetTextureId() const
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

class RSEglImageManager : public RSImageManager {
public:
    explicit RSEglImageManager(EGLDisplay display) : eglDisplay_(display) {};
    ~RSEglImageManager() noexcept override = default;

    void UnMapImageFromSurfaceBuffer(int32_t seqNum) override;
    std::shared_ptr<Drawing::Image> CreateImageFromBuffer(
        RSPaintFilterCanvas& canvas, const BufferDrawParam& params,
        const std::shared_ptr<Drawing::ColorSpace>& drawingColorSpace) override;
    std::shared_ptr<Drawing::Image> GetIntersectImage(Drawing::RectI& imgCutRect,
        const std::shared_ptr<Drawing::GPUContext>& context, const sptr<OHOS::SurfaceBuffer>& buffer,
        const sptr<SyncFence>& acquireFence, pid_t threadIndex = 0) override;

    void ShrinkCachesIfNeeded(bool isForUniRedraw = false) override; // only used for divided_render

private:
    void WaitAcquireFence(const sptr<SyncFence>& acquireFence);
    GLuint CreateEglImageCacheFromBuffer(const sptr<OHOS::SurfaceBuffer>& buffer,
        const pid_t threadIndex);
    GLuint MapEglImageFromSurfaceBuffer(const sptr<OHOS::SurfaceBuffer>& buffer,
        const sptr<SyncFence>& acquireFence, pid_t threadIndex);
    void UnMapEglImageFromSurfaceBufferForUniRedraw(int32_t seqNum);

    static constexpr size_t MAX_CACHE_SIZE = 16;
    std::queue<int32_t> cacheQueue_; // fifo, size restricted by MAX_CACHE_SIZE
    std::unordered_map<int32_t, std::unique_ptr<EglImageResource>> imageCacheSeqs_; // guarded by opMutex_
    EGLDisplay eglDisplay_ = EGL_NO_DISPLAY;
};
} // namespace Rosen
} // namespace OHOS

#endif // RS_EGL_IMAGE_MANAGER_H
