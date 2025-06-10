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

#include "rs_image_manager.h"

#include "GLES/glext.h"
#include "GLES3/gl32.h"

namespace OHOS {
namespace Rosen {
class EglImageResource : public ImageResource {
public:
    static std::unique_ptr<ImageResource> Create(EGLDisplay eglDisplay, EGLContext eglContext,
        const sptr<OHOS::SurfaceBuffer>& buffer);

    EglImageResource(EGLDisplay eglDisplay, EGLImageKHR eglImage, EGLClientBuffer eglClientBuffer)
        : eglDisplay_(eglDisplay), eglImage_(eglImage), eglClientBuffer_(eglClientBuffer) {}
    ~EglImageResource() noexcept override;

    GLuint GetTextureId() const override
    {
        return textureId_;
    }
private:
    // generate a texture and bind eglImage to it.
    bool BindToTexture();

    EGLDisplay eglDisplay_ = EGL_NO_DISPLAY;
    EGLImageKHR eglImage_ = EGL_NO_IMAGE_KHR;
    EGLClientBuffer eglClientBuffer_ = nullptr;
    GLuint textureId_ = 0;
};

class RSEglImageManager : public RSImageManager {
public:
    explicit RSEglImageManager(EGLDisplay display) : eglDisplay_(display) {};
    ~RSEglImageManager() noexcept override = default;

    void UnMapImageFromSurfaceBuffer(int32_t seqNum) override;
    std::shared_ptr<Drawing::Image> CreateImageFromBuffer(
        RSPaintFilterCanvas& canvas, const sptr<SurfaceBuffer>& buffer, const sptr<SyncFence>& acquireFence,
        const uint32_t threadIndex, const std::shared_ptr<Drawing::ColorSpace>& drawingColorSpace) override;

    GLuint MapEglImageFromSurfaceBuffer(const sptr<OHOS::SurfaceBuffer>& buffer,
        const sptr<SyncFence>& acquireFence, pid_t threadIndex);
    void UnMapEglImageFromSurfaceBufferForUniRedraw(int32_t seqNum);
    void ShrinkCachesIfNeeded(bool isForUniRedraw = false) override; // only used for divided_render
    std::unique_ptr<ImageResource> CreateImageCacheFromBuffer(const sptr<OHOS::SurfaceBuffer>& buffer,
        const sptr<SyncFence>& acquireFence) override;

private:
    void WaitAcquireFence(const sptr<SyncFence>& acquireFence);
    GLuint CreateImageCacheFromBuffer(
        const sptr<OHOS::SurfaceBuffer>& buffer, const pid_t threadIndex);

    static constexpr size_t MAX_CACHE_SIZE = 16;
    EGLDisplay eglDisplay_ = EGL_NO_DISPLAY;
    std::queue<int32_t> cacheQueue_; // fifo, size restricted by MAX_CACHE_SIZE
    std::unordered_map<int32_t, std::unique_ptr<ImageResource>> imageCacheSeqs_; // guarded by opMutex_
};
} // namespace Rosen
} // namespace OHOS

#endif // RS_EGL_IMAGE_MANAGER_H
