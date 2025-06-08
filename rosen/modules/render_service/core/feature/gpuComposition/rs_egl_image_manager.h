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
    static std::shared_ptr<ImageResource> Create(
        EGLDisplay eglDisplay,
        EGLContext eglContext,
        const sptr<OHOS::SurfaceBuffer>& buffer);
    EglImageResource(EGLDisplay eglDisplay, EGLImageKHR eglImage, EGLClientBuffer eglClientBuffer)
    {
        eglDisplay_ = eglDisplay;
        eglImage_ = eglImage;
        eglClientBuffer_ = eglClientBuffer;
    }
    ~EglImageResource() noexcept override;

    GLuint GetTextureId() const override
    {
        return textureId_;
    }

    void SetTextureId(GLuint textureId) override
    {
        textureId_ = textureId;
    }
private:
    // generate a texture and bind eglImage to it.
    bool BindToTexture() override;

    friend class RSImageManager;
};

class RSEglImageManager : public RSImageManager {
public:
    explicit RSEglImageManager(EGLDisplay display) : eglDisplay_(display) {};
    ~RSEglImageManager() noexcept override = default;

    GLuint MapEglImageFromSurfaceBuffer(const sptr<OHOS::SurfaceBuffer>& buffer,
        const sptr<SyncFence>& acquireFence, pid_t threadIndex) override;
    void UnMapImageFromSurfaceBuffer(int32_t seqNum) override;
    std::shared_ptr<ImageResource> CreateImageCacheFromBuffer(const sptr<OHOS::SurfaceBuffer>& buffer,
        const sptr<SyncFence>& acquireFence) override;

    void UnMapEglImageFromSurfaceBufferForUniRedraw(int32_t seqNum) override;
    void ShrinkCachesIfNeeded(bool isForUniRedraw = false) override; // only used for divided_render

private:
    void WaitAcquireFence(const sptr<SyncFence>& acquireFence);
    GLuint CreateImageCacheFromBuffer(
        const sptr<OHOS::SurfaceBuffer>& buffer, const pid_t threadIndex);

    static constexpr size_t MAX_CACHE_SIZE = 16;
    EGLDisplay eglDisplay_ = EGL_NO_DISPLAY;
    friend class RSImageManager;
};
} // namespace Rosen
} // namespace OHOS

#endif // RS_EGL_IMAGE_MANAGER_H
