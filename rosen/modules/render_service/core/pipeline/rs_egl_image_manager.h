/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include <unordered_map>

#include <surface.h>
#include "EGL/egl.h"
#include "EGL/eglext.h"
#include "GLES/gl.h"
#include "GLES/glext.h"
#include "GLES3/gl32.h"
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
        EGLClientBuffer eglClientbuffer);
    ~ImageCacheSeq() noexcept;

    // wait release fence and generate a new fence.
    bool WaitReleaseFence();

    GLuint TextureId() const
    {
        return textureId_;
    }

private:
    // generate a texture and bind eglImage to it.
    bool BindToTexture();

    EGLDisplay eglDisplay_ = EGL_NO_DISPLAY;
    EGLImageKHR eglImage_ = EGL_NO_IMAGE_KHR;
    EGLClientBuffer eglClientbuffer_ = nullptr;
    GLuint textureId_ = 0;
    EGLSyncKHR eglSync_ = EGL_NO_SYNC_KHR;
};

class RSEglImageManager {
public:
    explicit RSEglImageManager(EGLDisplay display);
    ~RSEglImageManager() noexcept = default;

    GLuint MapEglImageFromSurfaceBuffer(sptr<OHOS::SurfaceBuffer>& buffer);
    void UnMapEglImageFromSurfaceBuffer(int32_t seqNum);
private:
    GLuint CreateImageCacheFromBuffer(const sptr<OHOS::SurfaceBuffer>& buffer);

    mutable std::mutex opMutex_;
    std::unordered_map<int32_t, std::unique_ptr<ImageCacheSeq>> imageCacheSeqs_; // guarded by opMutex_.
    EGLDisplay eglDisplay_ = EGL_NO_DISPLAY;
};
} // namespace Rosen
} // namespace OHOS

#endif // RS_EGL_IMAGE_MANAGER_H
