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

#include "rs_egl_image_manager.h"

#include "feature/uifirst/rs_sub_thread_manager.h"
#include <native_window.h>
#include <platform/common/rs_log.h>
#include "sync_fence.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/hardware_thread/rs_hardware_thread.h"
#include "rs_trace.h"
#include "common/rs_optional_trace.h"
#include "pipeline/rs_task_dispatcher.h"

#ifndef NDEBUG
#include <cassert>
#endif

#ifdef USE_M133_SKIA
#include "src/gpu/ganesh/gl/GrGLDefines.h"
#else
#include "src/gpu/gl/GrGLDefines.h"
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
    EGLContext eglContext,
    const NativeWindowBufferObject& nativeBuffer)
{
    EGLint attrs[] = {
        EGL_IMAGE_PRESERVED_KHR,
        EGL_TRUE,
        EGL_NONE,
    };

    return GetEGLCreateImageKHRFunc()(
        eglDisplay, eglContext, EGL_NATIVE_BUFFER_OHOS, CastToEGLClientBuffer(nativeBuffer), attrs);
}
} // namespace Detail

EglImageResource::~EglImageResource() noexcept
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

bool EglImageResource::BindToTexture()
{
    // no image check.
    if (eglImage_ == EGL_NO_IMAGE_KHR) {
        RS_LOGE("EglImageResource::BindToTexture: eglImage_ is null.");
        return false;
    }

    glGenTextures(1, &textureId_);
    if (textureId_ == 0) {
        RS_LOGE("EglImageResource::BindToTexture: glGenTextures error.");
        return false;
    }

    // bind this eglImage_ to textureId_.
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, textureId_);
    Detail::GetGLEGLImageTargetTexture2DOESFunc()(GL_TEXTURE_EXTERNAL_OES, eglImage_);
    return true;
}

std::unique_ptr<EglImageResource> EglImageResource::Create(
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
        RS_LOGE("EglImageResource::Create: eglCreateImageKHR failed, error %{public}s.",
            Detail::EGLErrorString(eglGetError()));
        return nullptr;
    }

    auto imageCache = std::make_unique<EglImageResource>(
        eglDisplay, img, Detail::CastToEGLClientBuffer(nativeBuffer.Release()));
    if (!imageCache->BindToTexture()) {
        return nullptr;
    }
    return imageCache;
}

void RSEglImageManager::WaitAcquireFence(const sptr<SyncFence>& acquireFence)
{
    if (acquireFence == nullptr) {
        return;
    }
    acquireFence->Wait(3000); // 3000ms
}

GLuint RSEglImageManager::CreateEglImageCacheFromBuffer(const sptr<OHOS::SurfaceBuffer>& buffer,
    const pid_t threadIndex)
{
    auto bufferId = buffer->GetSeqNum();
    auto imageCache = EglImageResource::Create(eglDisplay_, EGL_NO_CONTEXT, buffer);
    if (imageCache == nullptr) {
        RS_LOGE("RSEglImageManager::CreateEglImageCacheFromBuffer:failed to create for buffer id %{public}d.",
            bufferId);
        return 0; // return texture id 0.
    }
    imageCache->SetThreadIndex(threadIndex);
    auto textureId = imageCache->GetTextureId();
    {
        std::lock_guard<std::mutex> lock(opMutex_);
        imageCacheSeqs_[bufferId] = std::move(imageCache);
    }
    cacheQueue_.push(bufferId);
    return textureId;
}

GLuint RSEglImageManager::MapEglImageFromSurfaceBuffer(const sptr<OHOS::SurfaceBuffer>& buffer,
    const sptr<SyncFence>& acquireFence, pid_t threadIndex)
{
    WaitAcquireFence(acquireFence);
    if (buffer == nullptr) {
        RS_LOGE("RSEglImageManager::MapEglImageFromSurfaceBuffer: buffer is null.");
        return 0;
    }
    auto bufferId = buffer->GetSeqNum();
    RS_OPTIONAL_TRACE_NAME_FMT("MapEglImage seqNum: %d", bufferId);
    RS_LOGD("RSEglImageManager::MapEglImageFromSurfaceBuffer: %{public}d", bufferId);
    {
        bool isImageCacheNotFound = false;
        std::lock_guard<std::mutex> lock(opMutex_);
        isImageCacheNotFound = imageCacheSeqs_.count(bufferId) == 0 || imageCacheSeqs_[bufferId] == nullptr;
        if (!isImageCacheNotFound) {
            const auto& imageCache = imageCacheSeqs_[bufferId];
            return imageCache->GetTextureId();
        }
    }
    // cache not found, create it.
    return CreateEglImageCacheFromBuffer(buffer, threadIndex);
}

void RSEglImageManager::ShrinkCachesIfNeeded(bool isForUniRedraw)
{
    while (cacheQueue_.size() > MAX_CACHE_SIZE) {
        const int32_t id = cacheQueue_.front();
        if (isForUniRedraw) {
            UnMapEglImageFromSurfaceBufferForUniRedraw(id);
        } else {
            UnMapImageFromSurfaceBuffer(id);
        }
        cacheQueue_.pop();
    }
}

void RSEglImageManager::UnMapImageFromSurfaceBuffer(int32_t seqNum)
{
    pid_t threadIndex = 0;
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
        std::unique_ptr<EglImageResource> imageCacheSeq;
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
    RSTaskDispatcher::GetInstance().PostTask(threadIndex, func);
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

std::shared_ptr<Drawing::Image> RSEglImageManager::CreateImageFromBuffer(
    RSPaintFilterCanvas& canvas, const BufferDrawParam& params,
    const std::shared_ptr<Drawing::ColorSpace>& drawingColorSpace)
{
    const sptr<SurfaceBuffer>& buffer = params.buffer;
    const sptr<SyncFence>& acquireFence = params.acquireFence;
    const pid_t threadIndex = params.threadIndex;
    const Drawing::AlphaType alphaType = params.alphaType;

#if defined(RS_ENABLE_EGLIMAGE) && defined(RS_ENABLE_GL)
    if (canvas.GetGPUContext() == nullptr) {
        RS_LOGE("RSEglImageManager::CreateEglImageFromBuffer GrContext is null!");
        return nullptr;
    }
    if (buffer == nullptr) {
        RS_LOGE("RSEglImageManager::CreateEglImageFromBuffer buffer is null!");
        return nullptr;
    }
    auto eglTextureId = MapEglImageFromSurfaceBuffer(buffer, acquireFence, threadIndex);
    if (eglTextureId == 0) {
        RS_LOGE("RSEglImageManager::CreateEglImageFromBuffer MapEglImageFromSurfaceBuffer return invalid texture ID");
        return nullptr;
    }
    auto pixelFmt = buffer->GetFormat();
    auto bitmapFormat = RSBaseRenderUtil::GenerateDrawingBitmapFormat(buffer, alphaType);

    auto image = std::make_shared<Drawing::Image>();
    Drawing::TextureInfo externalTextureInfo;
    externalTextureInfo.SetWidth(buffer->GetSurfaceBufferWidth());
    externalTextureInfo.SetHeight(buffer->GetSurfaceBufferHeight());

    auto surfaceOrigin = Drawing::TextureOrigin::TOP_LEFT;
    externalTextureInfo.SetIsMipMapped(false);
    externalTextureInfo.SetTarget(GL_TEXTURE_EXTERNAL_OES);
    externalTextureInfo.SetID(eglTextureId);
    auto glType = GR_GL_RGBA8;
    if (pixelFmt == GRAPHIC_PIXEL_FMT_BGRA_8888) {
        glType = GR_GL_BGRA8;
    } else if (pixelFmt == GRAPHIC_PIXEL_FMT_YCBCR_P010 || pixelFmt == GRAPHIC_PIXEL_FMT_YCRCB_P010 ||
        pixelFmt == GRAPHIC_PIXEL_FMT_RGBA_1010102) {
        glType = GR_GL_RGB10_A2;
    }
    externalTextureInfo.SetFormat(glType);
    if (!image->BuildFromTexture(*canvas.GetGPUContext(), externalTextureInfo,
        surfaceOrigin, bitmapFormat, drawingColorSpace)) {
        RS_LOGE("RSEglImageManager::CreateEglImageFromBuffer image BuildFromTexture failed");
        return nullptr;
    }
    return image;
#else
    return nullptr;
#endif // RS_ENABLE_EGLIMAGE
}

std::shared_ptr<Drawing::Image> RSEglImageManager::GetIntersectImage(Drawing::RectI& imgCutRect,
    const std::shared_ptr<Drawing::GPUContext>& context, const sptr<OHOS::SurfaceBuffer>& buffer,
    const sptr<SyncFence>& acquireFence, pid_t threadIndex)
{
    auto eglTextureId = MapEglImageFromSurfaceBuffer(buffer, acquireFence, threadIndex);
    if (eglTextureId == 0) {
        RS_LOGE("RSEglImageManager::GetIntersectImageFromGL invalid texture ID");
        return nullptr;
    }

    Drawing::BitmapFormat bitmapFormat = RSBaseRenderUtil::GenerateDrawingBitmapFormat(buffer);
    Drawing::TextureInfo externalTextureInfo;
    externalTextureInfo.SetWidth(buffer->GetSurfaceBufferWidth());
    externalTextureInfo.SetHeight(buffer->GetSurfaceBufferHeight());
    externalTextureInfo.SetIsMipMapped(false);
    externalTextureInfo.SetTarget(GL_TEXTURE_EXTERNAL_OES);
    externalTextureInfo.SetID(eglTextureId);
    auto glType = GR_GL_RGBA8;
    auto pixelFmt = buffer->GetFormat();
    if (pixelFmt == GRAPHIC_PIXEL_FMT_BGRA_8888) {
        glType = GR_GL_BGRA8;
    } else if (pixelFmt == GRAPHIC_PIXEL_FMT_YCBCR_P010 || pixelFmt == GRAPHIC_PIXEL_FMT_YCRCB_P010) {
        glType = GL_RGB10_A2;
    }
    externalTextureInfo.SetFormat(glType);

    std::shared_ptr<Drawing::Image> layerImage = std::make_shared<Drawing::Image>();
    if (!layerImage->BuildFromTexture(*context, externalTextureInfo,
        Drawing::TextureOrigin::TOP_LEFT, bitmapFormat, nullptr)) {
        RS_LOGE("RSEglImageManager::GetIntersectImageFromGL image BuildFromTexture failed");
        return nullptr;
    }

    std::shared_ptr<Drawing::Image> cutDownImage = std::make_shared<Drawing::Image>();
    bool res = cutDownImage->BuildSubset(layerImage, imgCutRect, *context);
    if (!res) {
        ROSEN_LOGE("RSEglImageManager::GetIntersectImageFromVK cutDownImage BuildSubset failed.");
        return nullptr;
    }
    Drawing::ImageInfo info = Drawing::ImageInfo(imgCutRect.GetWidth(), imgCutRect.GetHeight(),
        Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL);

    std::shared_ptr<Drawing::Surface> surface = Drawing::Surface::MakeRenderTarget(context.get(), false, info);
    if (surface == nullptr) {
        RS_LOGE("RSEglImageManager::GetIntersectImageFromGL MakeRenderTarget failed.");
        return nullptr;
    }
    auto drawCanvas = std::make_shared<RSPaintFilterCanvas>(surface.get());
    drawCanvas->DrawImage(*cutDownImage, 0.f, 0.f, Drawing::SamplingOptions());
    surface->FlushAndSubmit(true);
    return surface.get()->GetImageSnapshot();
}
} // namespace Rosen
} // namespace OHOS
