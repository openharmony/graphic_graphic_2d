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

#include "rs_egl_image_manager.h"

#include <native_window.h>
#include <platform/common/rs_log.h>
#include "sync_fence.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_hardware_thread.h"
#include "pipeline/parallel_render/rs_sub_thread_manager.h"
#include "rs_trace.h"
#include "common/rs_optional_trace.h"

#ifndef NDEBUG
#include <cassert>
#endif

namespace OHOS {
namespace Rosen {
namespace Detail {
#ifdef NDEBUG
#define RS_ASSERT(exp) (void)((exp))
#else
#define RS_ASSERT(exp) assert((exp))
#endif

#define RS_EGL_ERR_CASE_STR(value) case value: return #value
const char *EGLErrorString(GLint error)
{
    switch (error) {
        RS_EGL_ERR_CASE_STR(EGL_SUCCESS);
        RS_EGL_ERR_CASE_STR(EGL_NOT_INITIALIZED);
        RS_EGL_ERR_CASE_STR(EGL_BAD_ACCESS);
        RS_EGL_ERR_CASE_STR(EGL_BAD_ALLOC);
        RS_EGL_ERR_CASE_STR(EGL_BAD_ATTRIBUTE);
        RS_EGL_ERR_CASE_STR(EGL_BAD_CONFIG);
        RS_EGL_ERR_CASE_STR(EGL_BAD_CONTEXT);
        RS_EGL_ERR_CASE_STR(EGL_BAD_CURRENT_SURFACE);
        RS_EGL_ERR_CASE_STR(EGL_BAD_DISPLAY);
        RS_EGL_ERR_CASE_STR(EGL_BAD_MATCH);
        RS_EGL_ERR_CASE_STR(EGL_BAD_NATIVE_PIXMAP);
        RS_EGL_ERR_CASE_STR(EGL_BAD_NATIVE_WINDOW);
        RS_EGL_ERR_CASE_STR(EGL_BAD_PARAMETER);
        RS_EGL_ERR_CASE_STR(EGL_BAD_SURFACE);
        RS_EGL_ERR_CASE_STR(EGL_CONTEXT_LOST);
        default: return "Unknown";
    }
}

static PFNEGLCREATEIMAGEKHRPROC GetEGLCreateImageKHRFunc()
{
    static auto func = reinterpret_cast<PFNEGLCREATEIMAGEKHRPROC>(eglGetProcAddress("eglCreateImageKHR"));
    return func;
}

static PFNEGLDESTROYIMAGEKHRPROC GetEGLDestroyImageKHRFunc()
{
    static auto func = reinterpret_cast<PFNEGLDESTROYIMAGEKHRPROC>(eglGetProcAddress("eglDestroyImageKHR"));
    return func;
}

static PFNGLEGLIMAGETARGETTEXTURE2DOESPROC GetGLEGLImageTargetTexture2DOESFunc()
{
    static auto func = reinterpret_cast<PFNGLEGLIMAGETARGETTEXTURE2DOESPROC>(
        eglGetProcAddress("glEGLImageTargetTexture2DOES"));
    return func;
}

// RAII object for NativeWindowBuffer
class NativeWindowBufferObject {
public:
    explicit NativeWindowBufferObject(sptr<OHOS::SurfaceBuffer> buffer)
        : handle_(CreateNativeWindowBufferFromSurfaceBuffer(&buffer))
    {
    }
    explicit NativeWindowBufferObject(NativeWindowBuffer* nativeBuffer)
        : handle_(nativeBuffer)
    {
    }

    ~NativeWindowBufferObject() noexcept
    {
        if (handle_ != nullptr) {
            DestroyNativeWindowBuffer(handle_);
        }
    }

    NativeWindowBufferObject(const NativeWindowBufferObject&) = delete;
    void operator=(const NativeWindowBufferObject&) = delete;

    bool operator==(std::nullptr_t) const
    {
        return handle_ == nullptr;
    }
    bool operator!=(std::nullptr_t) const
    {
        return handle_ != nullptr;
    }

    // not explicit so you can use it to do the implicit-cast.
    // you should not delete or call DestroyNativeWindowBuffer for this pointer.
    operator NativeWindowBuffer* () const
    {
        return Get();
    }
    // you should not delete or call DestroyNativeWindowBuffer for this pointer.
    NativeWindowBuffer* Get() const
    {
        return handle_;
    }

    NativeWindowBuffer* Release()
    {
        NativeWindowBuffer* out = handle_;
        handle_ = nullptr;
        return out;
    }

private:
    NativeWindowBuffer* handle_ = nullptr;
};

NativeWindowBufferObject CreateNativeWindowBuffer(const sptr<OHOS::SurfaceBuffer>& buffer)
{
    return NativeWindowBufferObject(buffer);
}

EGLClientBuffer CastToEGLClientBuffer(NativeWindowBuffer* nativeBuffer)
{
    return static_cast<EGLClientBuffer>(nativeBuffer);
}

NativeWindowBuffer* CastFromEGLClientBuffer(EGLClientBuffer eglClientBuffer)
{
    return static_cast<NativeWindowBuffer*>(eglClientBuffer);
}

EGLImageKHR CreateEGLImage(
    EGLDisplay eglDisplay,
    EGLContext EGLContext,
    const NativeWindowBufferObject& nativeBuffer)
{
    EGLint attrs[] = {
        EGL_IMAGE_PRESERVED_KHR,
        EGL_TRUE,
        EGL_NONE,
    };

    return GetEGLCreateImageKHRFunc()(
        eglDisplay, EGLContext, EGL_NATIVE_BUFFER_OHOS, CastToEGLClientBuffer(nativeBuffer), attrs);
}
} // namespace Detail

ImageCacheSeq::ImageCacheSeq(
    EGLDisplay eglDisplay, EGLImageKHR eglImage, EGLClientBuffer eglClientBuffer)
    : eglDisplay_(eglDisplay),
      eglImage_(eglImage),
      eglClientBuffer_(eglClientBuffer)
{
}

ImageCacheSeq::~ImageCacheSeq() noexcept
{
    if (eglImage_ != EGL_NO_IMAGE_KHR) {
        Detail::GetEGLDestroyImageKHRFunc()(eglDisplay_, eglImage_);
        eglImage_ = EGL_NO_IMAGE_KHR;
    }

    if (textureId_ != 0) {
        glDeleteTextures(1, &textureId_);
        textureId_ = 0;
    }

    // auto dec ref.
    Detail::NativeWindowBufferObject nBufferDecRef(
        Detail::CastFromEGLClientBuffer(eglClientBuffer_));
}

bool ImageCacheSeq::BindToTexture()
{
    // no image check.
    if (eglImage_ == EGL_NO_IMAGE_KHR) {
        RS_LOGE("ImageCacheSeq::BindToTexture: eglImage_ is null.");
        return false;
    }

    glGenTextures(1, &textureId_);
    if (textureId_ == 0) {
        RS_LOGE("ImageCacheSeq::BindToTexture: glGenTextures error.");
        return false;
    }

    // bind this eglImage_ to textureId_.
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, textureId_);
    Detail::GetGLEGLImageTargetTexture2DOESFunc()(GL_TEXTURE_EXTERNAL_OES, eglImage_);
    return true;
}

std::unique_ptr<ImageCacheSeq> ImageCacheSeq::Create(
    EGLDisplay eglDisplay,
    EGLContext eglContext,
    const sptr<OHOS::SurfaceBuffer>& buffer)
{
    auto nativeBuffer = Detail::CreateNativeWindowBuffer(buffer);
    if (nativeBuffer == nullptr) {
        return nullptr;
    }

    EGLImageKHR img = Detail::CreateEGLImage(eglDisplay, eglContext, nativeBuffer);
    if (img == EGL_NO_IMAGE_KHR) {
        RS_LOGE("ImageCacheSeq::Create: eglCreateImageKHR failed, error %{public}s.",
            Detail::EGLErrorString(eglGetError()));
        return nullptr;
    }

    auto imageCache = std::make_unique<ImageCacheSeq>(
        eglDisplay, img, Detail::CastToEGLClientBuffer(nativeBuffer.Release()));
    if (!imageCache->BindToTexture()) {
        return nullptr;
    }
    return imageCache;
}

RSEglImageManager::RSEglImageManager(EGLDisplay display) : eglDisplay_(display)
{
}

void RSEglImageManager::WaitAcquireFence(const sptr<SyncFence>& acquireFence)
{
    if (acquireFence == nullptr) {
        return;
    }
    acquireFence->Wait(3000); // 3000ms
}

GLuint RSEglImageManager::CreateImageCacheFromBuffer(const sptr<OHOS::SurfaceBuffer>& buffer,
    const uint32_t threadIndex)
{
    auto bufferId = buffer->GetSeqNum();
    auto imageCache = ImageCacheSeq::Create(eglDisplay_, EGL_NO_CONTEXT, buffer);
    if (imageCache == nullptr) {
        RS_LOGE("RSEglImageManager::CreateImageCacheFromBuffer: failed to create ImageCache for buffer id %{public}d.",
            bufferId);
        return 0; // return texture id 0.
    }
    imageCache->SetThreadIndex(threadIndex);
    auto textureId = imageCache->TextureId();
    {
        std::lock_guard<std::mutex> lock(opMutex_);
        imageCacheSeqs_[bufferId] = std::move(imageCache);
    }
    cacheQueue_.push(bufferId);
    return textureId;
}

std::unique_ptr<ImageCacheSeq> RSEglImageManager::CreateImageCacheFromBuffer(const sptr<OHOS::SurfaceBuffer>& buffer,
    const sptr<SyncFence>& acquireFence)
{
    WaitAcquireFence(acquireFence);
    auto bufferId = buffer->GetSeqNum();
    auto imageCache = ImageCacheSeq::Create(eglDisplay_, EGL_NO_CONTEXT, buffer);
    if (imageCache == nullptr) {
        RS_LOGE("RSEglImageManager::CreateImageCacheFromBuffer: failed to create ImageCache for buffer id %{public}d.",
            bufferId);
        return nullptr;
    }
    return imageCache;
}

GLuint RSEglImageManager::MapEglImageFromSurfaceBuffer(const sptr<OHOS::SurfaceBuffer>& buffer,
    const sptr<SyncFence>& acquireFence, uint32_t threadIndex)
{
    WaitAcquireFence(acquireFence);
    auto bufferId = buffer->GetSeqNum();
    RS_OPTIONAL_TRACE_NAME_FMT("MapEglImage seqNum: %d", bufferId);
    RS_LOGD("RSEglImageManager::MapEglImageFromSurfaceBuffer: %{public}d", bufferId);
    {
        bool isImageCacheNotFound = false;
        std::lock_guard<std::mutex> lock(opMutex_);
        isImageCacheNotFound = imageCacheSeqs_.count(bufferId) == 0 || imageCacheSeqs_[bufferId] == nullptr;
        if (!isImageCacheNotFound) {
            const auto& imageCache = imageCacheSeqs_[bufferId];
            return imageCache->TextureId();
        }
    }
    // cache not found, create it.
    return CreateImageCacheFromBuffer(buffer, threadIndex);
}

void RSEglImageManager::ShrinkCachesIfNeeded(bool isForUniRedraw)
{
    while (cacheQueue_.size() > MAX_CACHE_SIZE) {
        const int32_t id = cacheQueue_.front();
        if (isForUniRedraw) {
            UnMapEglImageFromSurfaceBufferForUniRedraw(id);
        } else {
            UnMapEglImageFromSurfaceBuffer(id);
        }
        cacheQueue_.pop();
    }
}

void RSEglImageManager::UnMapEglImageFromSurfaceBuffer(int32_t seqNum)
{
    uint32_t threadIndex = UNI_MAIN_THREAD_INDEX;
    {
        std::lock_guard<std::mutex> lock(opMutex_);
        if (imageCacheSeqs_.count(seqNum) == 0) {
            return;
        }
        if (imageCacheSeqs_[seqNum]) {
            threadIndex = imageCacheSeqs_[seqNum]->GetThreadIndex();
        }
    }
    auto func = [this, seqNum]() {
        std::unique_ptr<ImageCacheSeq> imageCacheSeq;
        {
            std::lock_guard<std::mutex> lock(opMutex_);
            if (imageCacheSeqs_.count(seqNum) == 0) {
                return;
            }
            imageCacheSeq = std::move(imageCacheSeqs_[seqNum]);
        }
        imageCacheSeq.reset();
        RS_OPTIONAL_TRACE_NAME_FMT("UnmapEglImage seqNum: %d", seqNum);
        RS_LOGD("RSEglImageManager::UnMapEglImageFromSurfaceBuffer: %{public}d", seqNum);
    };
    if (threadIndex == UNI_MAIN_THREAD_INDEX) {
        RSMainThread::Instance()->PostTask(func);
    } else {
        RSSubThreadManager::Instance()->PostTask(func, threadIndex);
    }
}

void RSEglImageManager::UnMapEglImageFromSurfaceBufferForUniRedraw(int32_t seqNum)
{
    RSHardwareThread::Instance().PostTask([this, seqNum]() {
        std::lock_guard<std::mutex> lock(opMutex_);
        if (imageCacheSeqs_.count(seqNum) == 0) {
            return;
        }
        (void)imageCacheSeqs_.erase(seqNum);
        RS_LOGD("RSEglImageManager::UnMapEglImageFromSurfaceBufferForRedraw");
    });
}
} // namespace Rosen
} // namespace OHOS
