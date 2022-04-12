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
#ifdef RS_ENABLE_GL
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES

#include "rs_egl_image_manager.h"
#include <native_window.h>
#include "pipeline/rs_surface_render_node.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
RSEglImageManager::RSEglImageManager(EGLDisplay display) : eglDisplay_(display) {}

ImageCacheSeq::~ImageCacheSeq()
{
    eglDestroySyncKHR(eglGetCurrentDisplay(), eglSync_);
    eglSync_ = EGL_NO_SYNC_KHR;
    eglDestroyImageKHR(eglGetCurrentDisplay(), eglImage_);
    eglImage_ = EGL_NO_IMAGE_KHR;
    glDeleteTextures(1, &textureId_);
    textureId_ = 0;
}

uint32_t RSEglImageManager::MapEglImageFromSurfaceBuffer(sptr<OHOS::SurfaceBuffer>& buffer)
{
    std::lock_guard<std::mutex> lock(opMutex_);
    while (GL_NO_ERROR != glGetError()) {} // clear GL error
    // get seq num
    int32_t seqNum = buffer->GetSeqNum();
    // create image
    if (imageCacheSeqs_.find(seqNum) == imageCacheSeqs_.end() ||
        imageCacheSeqs_[seqNum].eglImage_ == EGL_NO_IMAGE_KHR) {
        EGLImageKHR eglImage = CreateEGLImage(buffer);
        if (eglImage == EGL_NO_IMAGE_KHR) {
            RS_LOGE("RSEglImageManager::MapEglImageFromSurfaceBuffer: fail to create EGLImage");
            return 0;
        }
        uint32_t textureId;
        glGenTextures(1, &textureId);
        if (textureId == 0) {
            glDeleteTextures(1, &textureId);
            textureId = 0;
            eglDestroyImageKHR(eglDisplay_, eglImage);
            eglImage = EGL_NO_IMAGE_KHR;
            RS_LOGE("RSEglImageManager::MapEglImageFromSurfaceBuffer: glGenTextures error");
            return 0;
        }
        if (!WaitReleaseEGLSync(seqNum)) {
            glDeleteTextures(1, &textureId);
            textureId = 0;
            eglDestroyImageKHR(eglDisplay_, eglImage);
            eglImage = EGL_NO_IMAGE_KHR;
            RS_LOGE("RSEglImageManager::MapEglImageFromSurfaceBuffer: WaitReleaseEGLSync error");
            return 0;
        }
        imageCacheSeqs_[seqNum].eglImage_ = eglImage;
        imageCacheSeqs_[seqNum].textureId_ = textureId;
    }

    EGLImageKHR img = imageCacheSeqs_[seqNum].eglImage_;
    uint32_t textureCacheId = imageCacheSeqs_[seqNum].textureId_;
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, textureCacheId);
    glEGLImageTargetTexture2DOES(GL_TEXTURE_EXTERNAL_OES, img);
    if (GL_NO_ERROR != glGetError()) {
        RS_LOGE("RSEglImageManager::MapEglImageFromSurfaceBuffer: glEGLImageTargetTexture2DOES error");
        return 0;
    }
    return textureCacheId;
}

EGLImageKHR RSEglImageManager::CreateEGLImage(const sptr<SurfaceBuffer>& buffer)
{
    sptr<SurfaceBuffer> bufferImpl = buffer;
    NativeWindowBuffer* nBuffer = CreateNativeWindowBufferFromSurfaceBuffer(&bufferImpl);
    EGLint attrs[] = {
        EGL_IMAGE_PRESERVED_KHR,
        EGL_TRUE,
        EGL_NONE,
    };

    EGLImageKHR img = eglCreateImageKHR(eglDisplay_, EGL_NO_CONTEXT, EGL_NATIVE_BUFFER_OHOS, nBuffer, attrs);
    if (img == EGL_NO_IMAGE_KHR) {
        EGLint error = eglGetError();
        RS_LOGE("failed, error %d", error);
    }
    return img;
}

bool RSEglImageManager::WaitReleaseEGLSync(int32_t seqNum)
{
    // check fence extension
    EGLSyncKHR fence = EGL_NO_SYNC_KHR;
    auto iter = imageCacheSeqs_.find(currentImageSeqs_);
    if (iter != imageCacheSeqs_.end()) {
        fence = imageCacheSeqs_[currentImageSeqs_].eglSync_;
    }
    if (fence != EGL_NO_SYNC_KHR) {
        EGLint ret = eglClientWaitSyncKHR(eglDisplay_, fence, 0, 1000000000);
        if (ret == EGL_FALSE) {
            RS_LOGE("eglClientWaitSyncKHR error 0x%x", eglGetError());
            return false;
        } else if (ret == EGL_TIMEOUT_EXPIRED_KHR) {
            RS_LOGE("eglClientWaitSyncKHR timeout");
            return false;
        }
        eglDestroySyncKHR(eglDisplay_, fence);
    }
    fence = eglCreateSyncKHR(eglDisplay_, EGL_SYNC_FENCE_KHR, NULL);
    if (fence == EGL_NO_SYNC_KHR) {
        RS_LOGE("eglCreateSyncKHR error 0x%x", eglGetError());
        return false;
    }
    glFlush();
    imageCacheSeqs_[seqNum].eglSync_ = fence;
    currentImageSeqs_ = seqNum;
    return true;
}

void RSEglImageManager::UnMapEglImageFromSurfaceBuffer(int32_t seqNum)
{
    std::lock_guard<std::mutex> lock(opMutex_);
    int32_t ret = imageCacheSeqs_.erase(seqNum);
    if (ret != 1) {
        RS_LOGE("UnMapEglImageFromSurfaceBuffer can not find error %d imageCache", seqNum);
    }
}
} // namespace Rosen
} // namespace OHOS
#endif // RS_ENABLE_GL
