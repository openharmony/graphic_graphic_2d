/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#if defined(RS_ENABLE_GL)
#ifndef EGL_EGLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#endif // EGL_EGLEXT_PROTOTYPES
#endif

#include "pixel_map_from_surface.h"
#include <scoped_bytrace.h>
#include <string>
#include "common/rs_background_thread.h"
#include "draw/canvas.h"
#include "image/image.h"
#include "native_window.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#if defined(RS_ENABLE_VK)
#include "platform/ohos/backend/native_buffer_utils.h"
#ifdef USE_VIDEO_PROCESSING_ENGINE
#include "render/rs_colorspace_convert.h"
#endif
#endif
#include "skia_adapter/skia_gpu_context.h"
#include "surface_buffer.h"
#include "sync_fence.h"

#if defined(RS_ENABLE_GL)
#include "EGL/egl.h"
#include "EGL/eglext.h"
#include "GLES/gl.h"
#include "GLES/glext.h"
#include "GLES3/gl32.h"
#endif

#include "include/core/SkImage.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkColorSpace.h"
#include "include/core/SkImageInfo.h"
#include "include/core/SkSurface.h"
#ifdef USE_M133_SKIA
#include "include/gpu/ganesh/gl/GrGLBackendSurface.h"
#include "include/gpu/ganesh/gl/GrGLInterface.h"
#include "include/gpu/ganesh/GrBackendSurface.h"
#include "include/gpu/ganesh/GrDirectContext.h"
#include "include/gpu/ganesh/GrRecordingContext.h"
#include "include/gpu/ganesh/SkImageGanesh.h"
#include "include/gpu/ganesh/SkSurfaceGanesh.h"
#include "include/gpu/GpuTypes.h"
#else
#include "include/gpu/GrBackendSurface.h"
#include "include/gpu/GrDirectContext.h"
#include "include/gpu/gl/GrGLInterface.h"
#include "include/core/SkImage.h"
#endif

namespace OHOS {
namespace Rosen {
using namespace OHOS::Media;

#ifdef RS_ENABLE_GPU
static sptr<SurfaceBuffer> LocalDmaMemAlloc(const uint32_t &width, const uint32_t &height,
    const std::unique_ptr<Media::PixelMap>& pixelmap)
{
#if defined(_WIN32) || defined(_APPLE) || defined(A_PLATFORM) || defined(IOS_PLATFORM)
    RS_LOGE("Unsupport dma mem alloc");
    return nullptr;
#else
    if (pixelmap == nullptr) {
        return nullptr;
    }
    sptr<SurfaceBuffer> surfaceBuffer = SurfaceBuffer::Create();
    if (surfaceBuffer == nullptr) {
        return nullptr;
    }
    BufferRequestConfig requestConfig = {
        .width = width,
        .height = height,
        .strideAlignment = 0x8, // set 0x8 as default value to alloc SurfaceBufferImpl
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888, // PixelFormat
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_HW_RENDER | BUFFER_USAGE_MEM_MMZ_CACHE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
        .colorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB,
        .transform = GraphicTransformType::GRAPHIC_ROTATE_NONE,
    };
    GSError ret = surfaceBuffer->Alloc(requestConfig);
    if (ret != GSERROR_OK) {
        RS_LOGE("SurfaceBuffer Alloc failed, %{public}s", GSErrorStr(ret).c_str());
        return nullptr;
    }
    void* nativeBuffer = surfaceBuffer.GetRefPtr();
    OHOS::RefBase *ref = reinterpret_cast<OHOS::RefBase *>(nativeBuffer);
    ref->IncStrongRef(ref);
    int32_t bufferSize = pixelmap->GetByteCount();
    pixelmap->SetPixelsAddr(surfaceBuffer->GetVirAddr(), nativeBuffer, bufferSize,
        Media::AllocatorType::DMA_ALLOC, nullptr);
    return surfaceBuffer;
#endif
}
#endif

#if defined(RS_ENABLE_GL)
class DmaMem {
public:
    static sptr<SurfaceBuffer> DmaMemAlloc(const uint32_t &width, const uint32_t &height,
        const std::unique_ptr<Media::PixelMap>& pixelmap);
    sk_sp<SkSurface> GetSkSurfaceFromSurfaceBuffer(GrRecordingContext *context,
        sptr<SurfaceBuffer> surfaceBuffer);
    void ReleaseGLMemory();
private:
    EGLImageKHR eglImage_ = EGL_NO_IMAGE_KHR;
    GLuint texId_ = 0;
    OHNativeWindowBuffer* nativeWindowBuffer_ = nullptr;
};

void DmaMem::ReleaseGLMemory()
{
    if (texId_ != 0U) {
        glBindTexture(GL_TEXTURE_2D, 0);
        glDeleteTextures(1, &texId_);
        texId_ = 0U;
    }
    if (eglImage_ != EGL_NO_IMAGE_KHR) {
        auto disp = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        if (disp != EGL_NO_DISPLAY && eglDestroyImageKHR(disp, eglImage_) != EGL_FALSE) {
            eglImage_ = EGL_NO_IMAGE_KHR;
        }
    }
    if (nativeWindowBuffer_ != nullptr) {
        DestroyNativeWindowBuffer(nativeWindowBuffer_);
        nativeWindowBuffer_ = nullptr;
    }
}

sptr<SurfaceBuffer> DmaMem::DmaMemAlloc(const uint32_t &width, const uint32_t &height,
    const std::unique_ptr<Media::PixelMap>& pixelmap)
{
    return LocalDmaMemAlloc(width, height, pixelmap);
}

sk_sp<SkSurface> DmaMem::GetSkSurfaceFromSurfaceBuffer(GrRecordingContext *context, sptr<SurfaceBuffer> surfaceBuffer)
{
    ScopedBytrace trace(__func__);
    if (surfaceBuffer == nullptr) {
        return nullptr;
    }
    if (nativeWindowBuffer_ == nullptr) {
        nativeWindowBuffer_ = CreateNativeWindowBufferFromSurfaceBuffer(&surfaceBuffer);
        if (!nativeWindowBuffer_) {
            return nullptr;
        }
    }
    EGLint attrs[] = { EGL_IMAGE_PRESERVED, EGL_TRUE, EGL_NONE, };

    auto disp = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (disp == EGL_NO_DISPLAY) {
        RS_LOGE("egl get display fail in GetSkSurfaceFromSurfaceBuffer");
        return nullptr;
    }
    if (eglImage_ == EGL_NO_IMAGE_KHR) {
        eglImage_ = eglCreateImageKHR(disp, EGL_NO_CONTEXT, EGL_NATIVE_BUFFER_OHOS, nativeWindowBuffer_, attrs);
        if (eglImage_ == EGL_NO_IMAGE_KHR) {
            return nullptr;
        }
    }
    // Create texture object
    if (texId_ == 0U) {
        glGenTextures(1, &texId_);
        glBindTexture(GL_TEXTURE_2D, texId_);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        static auto glEGLImageTargetTexture2DOESFunc = reinterpret_cast<PFNGLEGLIMAGETARGETTEXTURE2DOESPROC>(
            eglGetProcAddress("glEGLImageTargetTexture2DOES"));
        if (glEGLImageTargetTexture2DOESFunc == nullptr) {
            return nullptr;
        }
        glEGLImageTargetTexture2DOESFunc(GL_TEXTURE_2D, static_cast<GLeglImageOES>(eglImage_));
    }
    GrGLTextureInfo textureInfo = { GL_TEXTURE_2D, texId_, GL_RGBA8_OES };
#ifdef USE_M133_SKIA
    GrBackendTexture backendTexture = GrBackendTextures::MakeGL(
        surfaceBuffer->GetWidth(), surfaceBuffer->GetHeight(), skgpu::Mipmapped::kNo, textureInfo);
    auto skSurface = SkSurfaces::WrapBackendTexture(context, backendTexture,
        kTopLeft_GrSurfaceOrigin, 0, kRGBA_8888_SkColorType, SkColorSpace::MakeSRGB(), nullptr);
#else
    GrBackendTexture backendTexture(
        surfaceBuffer->GetWidth(), surfaceBuffer->GetHeight(), GrMipMapped::kNo, textureInfo);
    auto skSurface = SkSurface::MakeFromBackendTexture(context, backendTexture,
        kTopLeft_GrSurfaceOrigin, 0, kRGBA_8888_SkColorType, SkColorSpace::MakeSRGB(), nullptr);
#endif
    return skSurface;
}
#endif

class PixelMapFromSurface {
public:
    PixelMapFromSurface();
    ~PixelMapFromSurface() noexcept;

    std::unique_ptr<OHOS::Media::PixelMap> Create(sptr<Surface> surface, const OHOS::Media::Rect &srcRect);
    std::unique_ptr<OHOS::Media::PixelMap> Create(sptr<SurfaceBuffer> surfaceBuffer, const OHOS::Media::Rect &srcRect);

    PixelMapFromSurface(const PixelMapFromSurface &) = delete;
    void operator=(const PixelMapFromSurface &) = delete;
    PixelMapFromSurface(const PixelMapFromSurface &&) = delete;
    void operator=(const PixelMapFromSurface &&) = delete;

private:
    OHNativeWindowBuffer *GetNativeWindowBufferFromSurface(sptr<SurfaceBuffer> &surfaceBuffer,
        const sptr<Surface> &surface, const OHOS::Media::Rect &srcRect);
    bool CreateEGLImage();
    bool DrawImage(GrRecordingContext *context, const OHOS::Media::Rect &srcRect, const sk_sp<SkSurface> &surface);
    void Clear() noexcept;
#if defined(RS_ENABLE_VK)
    static void DeleteVkImage(void *context);
    std::unique_ptr<OHOS::Media::PixelMap> GetPixelMapForVK(const OHOS::Media::Rect &srcRect);
#endif
    std::unique_ptr<OHOS::Media::PixelMap> CreateForVK(const sptr<Surface> &surface, const OHOS::Media::Rect &srcRect);
    std::unique_ptr<OHOS::Media::PixelMap> CreateForVK(
        const sptr<SurfaceBuffer> &surfaceBuffer, const OHOS::Media::Rect &srcRect);
    bool CanvasDrawImage(const std::shared_ptr<Drawing::Image> &drawingImage, const OHOS::Media::Rect &srcRect,
        std::shared_ptr<Drawing::Canvas> &canvas);
    bool DrawImageRectVK(const std::shared_ptr<Drawing::Image> &drawingImage,
        OHNativeWindowBuffer *nativeWindowBufferTmp, const sptr<SurfaceBuffer> &surfaceBufferTmp,
        const OHOS::Media::Rect &srcRect);
    std::shared_ptr<Drawing::Image> CreateDrawingImage();

    sptr<Surface> surface_ = nullptr;
    sptr<SurfaceBuffer> surfaceBuffer_ = nullptr;
    OHNativeWindowBuffer *nativeWindowBuffer_ = nullptr;
#if defined(RS_ENABLE_GL)
    GLuint texId_ = 0U;
    EGLImageKHR eglImage_ = EGL_NO_IMAGE_KHR;
    std::unique_ptr<OHOS::Media::PixelMap> CreatePixelMapForGL(sk_sp<GrDirectContext> grContext,
        const OHOS::Media::Rect &srcRect);
    std::unique_ptr<OHOS::Media::PixelMap> GetPixelMapForGL(const OHOS::Media::Rect &srcRect);
#endif
    std::unique_ptr<OHOS::Media::PixelMap> CreateForGL(const sptr<Surface> &surface, const OHOS::Media::Rect &srcRect);
    std::unique_ptr<OHOS::Media::PixelMap> CreateForGL(
        const sptr<SurfaceBuffer> &surfaceBuffer, const OHOS::Media::Rect &srcRect);

#if defined(RS_ENABLE_VK)
    mutable Drawing::BackendTexture backendTexture_ = {};
#endif
};

PixelMapFromSurface::PixelMapFromSurface()
{
}

PixelMapFromSurface::~PixelMapFromSurface() noexcept
{
    Clear();
}

void PixelMapFromSurface::Clear() noexcept
{
    if (surfaceBuffer_ && surface_) {
        if (surface_->ReleaseLastFlushedBuffer(surfaceBuffer_) != GSERROR_OK) {
            RS_LOGE("PixelMapFromSurface clear, ReleaseLastFlushedBuffer fail");
        }
    }
    surfaceBuffer_ = nullptr;
    surface_ = nullptr;

#if defined(RS_ENABLE_GL)
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::OPENGL) {
        if (texId_ != 0U) {
            glDeleteTextures(1, &texId_);
            texId_ = 0U;
        }
        if (eglImage_ != EGL_NO_IMAGE_KHR) {
            auto disp = eglGetDisplay(EGL_DEFAULT_DISPLAY);
            if (disp != EGL_NO_DISPLAY && eglDestroyImageKHR(disp, eglImage_) != EGL_FALSE) {
                eglImage_ = EGL_NO_IMAGE_KHR;
            }
        }
    }
#endif

    if (nativeWindowBuffer_ != nullptr) {
        DestroyNativeWindowBuffer(nativeWindowBuffer_);
        nativeWindowBuffer_ = nullptr;
    }
}

#if defined(RS_ENABLE_VK)
static Drawing::ColorType GetColorTypeFromVKFormat(VkFormat vkFormat)
{
    switch (vkFormat) {
        case VK_FORMAT_R8G8B8A8_UNORM:
            return Drawing::COLORTYPE_RGBA_8888;
        case VK_FORMAT_R16G16B16A16_SFLOAT:
            return Drawing::COLORTYPE_RGBA_F16;
        case VK_FORMAT_R5G6B5_UNORM_PACK16:
            return Drawing::COLORTYPE_RGB_565;
        default:
            return Drawing::COLORTYPE_RGBA_8888;
    }
}
#endif

#if defined(RS_ENABLE_GL)
std::unique_ptr<OHOS::Media::PixelMap> PixelMapFromSurface::CreatePixelMapForGL(
    sk_sp<GrDirectContext> grContext, const OHOS::Media::Rect &srcRect)
{
    DmaMem dmaMem;
    InitializationOptions options;
    options.size.width = srcRect.width;
    options.size.height = srcRect.height;
    options.srcPixelFormat = PixelFormat::RGBA_8888;
    options.pixelFormat = PixelFormat::RGBA_8888;
    auto pixelMap = PixelMap::Create(options);
    if (pixelMap == nullptr) {
        RS_LOGE("create pixelMap fail in CreatePixelMapForGL");
        return nullptr;
    }

    sptr<SurfaceBuffer> surfaceBuffer = DmaMem::DmaMemAlloc(srcRect.width, srcRect.height, pixelMap);
    if (surfaceBuffer == nullptr) {
        dmaMem.ReleaseGLMemory();
        return nullptr;
    }
    auto skSurface = dmaMem.GetSkSurfaceFromSurfaceBuffer(grContext.get(), surfaceBuffer);
    if (skSurface == nullptr) {
        dmaMem.ReleaseGLMemory();
        return nullptr;
    }
    if (!DrawImage(grContext.get(), srcRect, skSurface)) {
        dmaMem.ReleaseGLMemory();
        return nullptr;
    }
    dmaMem.ReleaseGLMemory();
    return pixelMap;
}
#endif

std::unique_ptr<OHOS::Media::PixelMap> PixelMapFromSurface::CreateForGL(const sptr<Surface> &surface,
    const OHOS::Media::Rect &srcRect)
{
#if defined(RS_ENABLE_GL) && defined(RS_ENABLE_UNI_RENDER)
    ScopedBytrace trace(__func__);
    nativeWindowBuffer_ = GetNativeWindowBufferFromSurface(surfaceBuffer_, surface, srcRect);
    return GetPixelMapForGL(srcRect);
#else
    return nullptr;
#endif
}

std::unique_ptr<OHOS::Media::PixelMap> PixelMapFromSurface::CreateForGL(const sptr<SurfaceBuffer> &surfaceBuffer,
    const OHOS::Media::Rect &srcRect)
{
#if defined(RS_ENABLE_GL) && defined(RS_ENABLE_UNI_RENDER)
    ScopedBytrace trace(__func__);
    surfaceBuffer_ = surfaceBuffer;
    nativeWindowBuffer_ = CreateNativeWindowBufferFromSurfaceBuffer(&surfaceBuffer_);
    return GetPixelMapForGL(srcRect);
#else
    return nullptr;
#endif
}

#if defined(RS_ENABLE_GL) && defined(RS_ENABLE_UNI_RENDER)
std::unique_ptr<OHOS::Media::PixelMap> PixelMapFromSurface::GetPixelMapForGL(const OHOS::Media::Rect &srcRect)
{
    if (!nativeWindowBuffer_ || !surfaceBuffer_) {
        RS_LOGE("GetPixelMapForGL nativeWindowBuffer_ or surfaceBuffer_ is null");
        return nullptr;
    }

    auto gpuContext = RSBackgroundThread::Instance().GetShareGPUContext();
    if (!gpuContext) {
        RS_LOGE("get gpuContext  fail");
        return nullptr;
    }

    sk_sp<GrDirectContext> grContext = nullptr;
    auto skiaGpuContext = gpuContext->GetImpl<Drawing::SkiaGPUContext>();
    if (skiaGpuContext) {
        grContext = skiaGpuContext->GetGrContext();
    }
    if (!grContext) {
        RS_LOGE("get gpuContext  fail");
        return nullptr;
    }

    if (!CreateEGLImage()) {
        return nullptr;
    }
    return CreatePixelMapForGL(grContext, srcRect);
}
#endif

#if defined(RS_ENABLE_VK)
void PixelMapFromSurface::DeleteVkImage(void *context)
{
    NativeBufferUtils::VulkanCleanupHelper *cleanupHelper =
        static_cast<NativeBufferUtils::VulkanCleanupHelper *>(context);
    if (cleanupHelper != nullptr) {
        cleanupHelper->UnRef();
    }
}
#endif

bool PixelMapFromSurface::CanvasDrawImage(const std::shared_ptr<Drawing::Image> &drawingImage,
    const OHOS::Media::Rect &srcRect, std::shared_ptr<Drawing::Canvas> &canvas)
{
#if defined(RS_ENABLE_VK)
    Drawing::Paint paint;
    paint.SetStyle(Drawing::Paint::PaintStyle::PAINT_FILL);

    Drawing::Rect srcDrawRect = Drawing::Rect(srcRect.left, srcRect.top, srcRect.width, srcRect.height);
    Drawing::Rect dstRect = Drawing::Rect(0, 0, srcRect.width, srcRect.height);

    GraphicPixelFormat pixelFormat = static_cast<GraphicPixelFormat>(surfaceBuffer_->GetFormat());
    if (pixelFormat == GRAPHIC_PIXEL_FMT_YCBCR_P010 || pixelFormat == GRAPHIC_PIXEL_FMT_YCRCB_P010 ||
        pixelFormat == GRAPHIC_PIXEL_FMT_RGBA_1010102) {
        Drawing::Matrix matrix; // Identity Matrix
        auto sx = dstRect.GetWidth() / srcDrawRect.GetWidth();
        auto sy = dstRect.GetHeight() / srcDrawRect.GetHeight();
        auto tx = dstRect.GetLeft() - srcDrawRect.GetLeft() * sx;
        auto ty = dstRect.GetTop() - srcDrawRect.GetTop() * sy;
        matrix.SetScaleTranslate(sx, sy, tx, ty);

        auto imageShader = Drawing::ShaderEffect::CreateImageShader(*drawingImage, Drawing::TileMode::CLAMP,
            Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::NEAREST), matrix);
        if (!imageShader) {
            RS_LOGE("[PixelMapFromSurface] CanvasDrawImage CreateImageShader fail");
            return false;
        }
#ifdef USE_VIDEO_PROCESSING_ENGINE
        sptr<SurfaceBuffer> sfBuffer(surfaceBuffer_);
        auto targetColorSpace = GRAPHIC_COLOR_GAMUT_SRGB;
        if (!RSColorSpaceConvert::Instance().ColorSpaceConvertor(imageShader, sfBuffer, paint, targetColorSpace, 0,
            DynamicRangeMode::STANDARD)) {
            RS_LOGE("[PixelMapFromSurface] CanvasDrawImage ColorSpaceConvertor fail");
            return false;
        }
#endif // USE_VIDEO_PROCESSING_ENGINE
        canvas->AttachPaint(paint);
        canvas->DrawRect(dstRect);
    } else {
        canvas->AttachPaint(paint);
        canvas->DrawImageRect(*drawingImage, srcDrawRect, dstRect,
            Drawing::SamplingOptions(Drawing::FilterMode::NEAREST),
            Drawing::SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT);
    }
    return true;
#else
    return false;
#endif
}

bool PixelMapFromSurface::DrawImageRectVK(const std::shared_ptr<Drawing::Image> &drawingImage,
    OHNativeWindowBuffer *nativeWindowBufferTmp, const sptr<SurfaceBuffer> &surfaceBufferTmp,
    const OHOS::Media::Rect &srcRect)
{
#if defined(RS_ENABLE_VK)
    ScopedBytrace trace1(__func__);
    if (RSBackgroundThread::Instance().GetShareGPUContext() == nullptr) {
        return false;
    }
    Drawing::BackendTexture backendTextureTmp = NativeBufferUtils::MakeBackendTextureFromNativeBuffer(
        nativeWindowBufferTmp, surfaceBufferTmp->GetWidth(), surfaceBufferTmp->GetHeight());
    if (!backendTextureTmp.IsValid()) {
        return false;
    }
    auto vkTextureInfo = backendTextureTmp.GetTextureInfo().GetVKTextureInfo();
    if (vkTextureInfo == nullptr) {
        return false;
    }
    vkTextureInfo->imageUsageFlags = vkTextureInfo->imageUsageFlags | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    auto cleanUpHelper = new NativeBufferUtils::VulkanCleanupHelper(RsVulkanContext::GetSingleton(),
        vkTextureInfo->vkImage, vkTextureInfo->vkAlloc.memory);
    if (cleanUpHelper == nullptr) {
        return false;
    }
    std::shared_ptr<Drawing::Surface> drawingSurface = Drawing::Surface::MakeFromBackendTexture(
        RSBackgroundThread::Instance().GetShareGPUContext().get(),
        backendTextureTmp.GetTextureInfo(),
        Drawing::TextureOrigin::TOP_LEFT,
        1, Drawing::ColorType::COLORTYPE_RGBA_8888, nullptr,
        PixelMapFromSurface::DeleteVkImage, cleanUpHelper);
    if (drawingSurface == nullptr) {
        return false;
    }
    auto canvas = drawingSurface->GetCanvas();
    if (canvas == nullptr) {
        return false;
    }
    if (!CanvasDrawImage(drawingImage, srcRect, canvas)) {
        return false;
    }
    ScopedBytrace trace2("FlushAndSubmit");
    drawingSurface->FlushAndSubmit(true);
    return true;
#else
    return false;
#endif
}

std::shared_ptr<Drawing::Image> PixelMapFromSurface::CreateDrawingImage()
{
#if defined(RS_ENABLE_VK) && defined(RS_ENABLE_UNI_RENDER)
    ScopedBytrace trace(__func__);
    if (RSBackgroundThread::Instance().GetShareGPUContext() == nullptr) {
        return nullptr;
    }
    backendTexture_ = NativeBufferUtils::MakeBackendTextureFromNativeBuffer(nativeWindowBuffer_,
        surfaceBuffer_->GetWidth(), surfaceBuffer_->GetHeight());
    if (!backendTexture_.IsValid()) {
        RS_LOGE("make backendTexture fail");
        return nullptr;
    }
    auto vkTextureInfo = backendTexture_.GetTextureInfo().GetVKTextureInfo();
    if (!vkTextureInfo) {
        RS_LOGE("make vkTextureInfo fail");
        return nullptr;
    }

    NativeBufferUtils::VulkanCleanupHelper* cleanUpHelper =
        new NativeBufferUtils::VulkanCleanupHelper(RsVulkanContext::GetSingleton(),
        vkTextureInfo->vkImage, vkTextureInfo->vkAlloc.memory);
    if (!cleanUpHelper) {
        RS_LOGE("make cleanUpHelper fail");
        return nullptr;
    }

    Drawing::BitmapFormat bitmapFormat = { GetColorTypeFromVKFormat(vkTextureInfo->format),
        Drawing::AlphaType::ALPHATYPE_PREMUL };
    std::shared_ptr<Drawing::Image> drawingImage = std::make_shared<Drawing::Image>();
    if (!drawingImage) {
        RS_LOGE("make drawingImage fail");
        cleanUpHelper->UnRef();
        return nullptr;
    }

    if (!drawingImage->BuildFromTexture(*(RSBackgroundThread::Instance().GetShareGPUContext().get()),
        backendTexture_.GetTextureInfo(), Drawing::TextureOrigin::TOP_LEFT, bitmapFormat, nullptr,
        NativeBufferUtils::DeleteVkImage, cleanUpHelper)) {
        RS_LOGE("drawingImage BuildFromTexture fail");
        return nullptr;
    }
    return drawingImage;
#else
    return nullptr;
#endif
}

std::unique_ptr<OHOS::Media::PixelMap> PixelMapFromSurface::CreateForVK(const sptr<Surface> &surface,
    const OHOS::Media::Rect &srcRect)
{
#if defined(RS_ENABLE_VK)
    ScopedBytrace trace(__func__);
    nativeWindowBuffer_ = GetNativeWindowBufferFromSurface(surfaceBuffer_, surface, srcRect);
    return GetPixelMapForVK(srcRect);
#else
    return nullptr;
#endif
}

std::unique_ptr<OHOS::Media::PixelMap> PixelMapFromSurface::CreateForVK(const sptr<SurfaceBuffer> &surfaceBuffer,
    const OHOS::Media::Rect &srcRect)
{
#if defined(RS_ENABLE_VK)
    ScopedBytrace trace(__func__);
    surfaceBuffer_ = surfaceBuffer;
    nativeWindowBuffer_ = CreateNativeWindowBufferFromSurfaceBuffer(&surfaceBuffer_);
    return GetPixelMapForVK(srcRect);
#else
    return nullptr;
#endif
}

#if defined(RS_ENABLE_VK)
std::unique_ptr<OHOS::Media::PixelMap> PixelMapFromSurface::GetPixelMapForVK(const OHOS::Media::Rect &srcRect)
{
    if (!nativeWindowBuffer_ || !surfaceBuffer_) {
        RS_LOGE("GetPixelMapForVK nativeWindowBuffer_ or surfaceBuffer_ is null");
        return nullptr;
    }

    std::shared_ptr<Drawing::Image> drawingImage = CreateDrawingImage();
    if (!drawingImage) {
        return nullptr;
    }

    InitializationOptions options;
    options.size.width = srcRect.width;
    options.size.height = srcRect.height;
    options.srcPixelFormat = PixelFormat::RGBA_8888;
    options.pixelFormat = PixelFormat::RGBA_8888;
    auto pixelMap = PixelMap::Create(options);
    if (!pixelMap) {
        RS_LOGE("create pixelMap fail in CreateForVK");
        return nullptr;
    }

    sptr<SurfaceBuffer> surfaceBufferTmp = LocalDmaMemAlloc(srcRect.width, srcRect.height, pixelMap);
    if (!surfaceBufferTmp) {
        RS_LOGE("LocalDmaMemAlloc fail");
        return nullptr;
    }

    OHNativeWindowBuffer *nativeWindowBufferTmp = CreateNativeWindowBufferFromSurfaceBuffer(&surfaceBufferTmp);
    if (!nativeWindowBufferTmp) {
        RS_LOGE("CreateNativeWindowBufferFromSurfaceBuffer fail");
        return nullptr;
    }

    if (!DrawImageRectVK(drawingImage, nativeWindowBufferTmp, surfaceBufferTmp, srcRect)) {
        DestroyNativeWindowBuffer(nativeWindowBufferTmp);
        RS_LOGE("DrawImageRectVK fail");
        return nullptr;
    }
    DestroyNativeWindowBuffer(nativeWindowBufferTmp);
    return pixelMap;
}
#endif

OHNativeWindowBuffer *PixelMapFromSurface::GetNativeWindowBufferFromSurface(
    sptr<SurfaceBuffer> &surfaceBuffer, const sptr<Surface> &surface, const OHOS::Media::Rect &srcRect)
{
    ScopedBytrace trace(__func__);
    // private func, surface is not nullptr.
    sptr<SyncFence> fence = SyncFence::InvalidFence();
    // a 4 * 4 idetity matrix
    float matrix[16] = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };
    GSError ret = surface->AcquireLastFlushedBuffer(surfaceBuffer, fence, matrix, 16, false); // 16 : matrix size
    if (ret != OHOS::GSERROR_OK || surfaceBuffer == nullptr) {
        RS_LOGE("GetLastFlushedBuffer fail, ret = %{public}d", ret);
        return nullptr;
    }

    int32_t bufferWidth = surfaceBuffer->GetWidth();
    int32_t bufferHeight = surfaceBuffer->GetHeight();
    if (srcRect.width > bufferWidth || srcRect.height > bufferHeight ||
        srcRect.left >= bufferWidth || srcRect.top >= bufferHeight ||
        static_cast<int64_t>(srcRect.left) + static_cast<int64_t>(srcRect.width) > static_cast<int64_t>(bufferWidth) ||
        static_cast<int64_t>(srcRect.top) + static_cast<int64_t>(srcRect.height) > static_cast<int64_t>(bufferHeight)) {
        RS_LOGE("invalid argument: srcRect[%{public}d, %{public}d, %{public}d, %{public}d],"
            "bufferWidth=%{public}d, bufferHeight=%{public}d",
            srcRect.left, srcRect.top, srcRect.width, srcRect.height, bufferWidth, bufferHeight);
        return nullptr;
    }

    if (fence != nullptr) {
        fence->Wait(3000); // wait at most 3000ms
    }

    return CreateNativeWindowBufferFromSurfaceBuffer(&surfaceBuffer);
}

bool PixelMapFromSurface::CreateEGLImage()
{
#if defined(RS_ENABLE_GL)
    ScopedBytrace trace(__func__);
    EGLint attrs[] = { EGL_IMAGE_PRESERVED, EGL_TRUE, EGL_NONE };
    auto disp = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (disp == EGL_NO_DISPLAY) {
        RS_LOGE("egl get display fail in CreateEGLImage");
        return false;
    }
    eglImage_ = eglCreateImageKHR(disp, EGL_NO_CONTEXT, EGL_NATIVE_BUFFER_OHOS, nativeWindowBuffer_, attrs);
    if (eglImage_ == EGL_NO_IMAGE_KHR) {
        RS_LOGE("create eglImage fail");
        return false;
    }
    glGenTextures(1, &texId_);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, texId_);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    static auto glEGLImageTargetTexture2DOESFunc = reinterpret_cast<PFNGLEGLIMAGETARGETTEXTURE2DOESPROC>(
        eglGetProcAddress("glEGLImageTargetTexture2DOES"));
    if (glEGLImageTargetTexture2DOESFunc == nullptr) {
        RS_LOGE("CreateEGLImage fail");
        return false;
    }
    glEGLImageTargetTexture2DOESFunc(GL_TEXTURE_EXTERNAL_OES, static_cast<GLeglImageOES>(eglImage_));
    return true;
#else
    return false;
#endif
}

bool PixelMapFromSurface::DrawImage(GrRecordingContext *context,
    const OHOS::Media::Rect &srcRect, const sk_sp<SkSurface> &surface)
{
#if defined(RS_ENABLE_GL)
    ScopedBytrace trace1(__func__);
    GraphicPixelFormat pixelFormat = static_cast<GraphicPixelFormat>(surfaceBuffer_->GetFormat());
    SkColorType colorType = kRGBA_8888_SkColorType;
    GLuint glType = GL_RGBA8;
    int bufferWidth = surfaceBuffer_->GetWidth();
    int bufferHeight = surfaceBuffer_->GetHeight();
    if (pixelFormat == GRAPHIC_PIXEL_FMT_BGRA_8888) {
        colorType = kBGRA_8888_SkColorType;
    } else if (pixelFormat == GRAPHIC_PIXEL_FMT_YCBCR_P010 || pixelFormat == GRAPHIC_PIXEL_FMT_YCRCB_P010 ||
        pixelFormat == GRAPHIC_PIXEL_FMT_RGBA_1010102) {
        colorType = kRGBA_1010102_SkColorType;
        glType = GL_RGB10_A2;
    }
    GrGLTextureInfo grExternalTextureInfo = { GL_TEXTURE_EXTERNAL_OES, texId_, static_cast<GrGLenum>(glType) };
#ifdef USE_M133_SKIA
    auto backendTexturePtr = std::make_shared<GrBackendTexture>(
        GrBackendTextures::MakeGL(bufferWidth, bufferHeight, skgpu::Mipmapped::kNo, grExternalTextureInfo)
    );
    if (backendTexturePtr == nullptr) {
        return false;
    }
    sk_sp<SkImage> image = SkImages::BorrowTextureFrom(context, *backendTexturePtr,
        kTopLeft_GrSurfaceOrigin, colorType, kPremul_SkAlphaType, nullptr);
#else
    auto backendTexturePtr =
        std::make_shared<GrBackendTexture>(bufferWidth, bufferHeight, GrMipMapped::kNo, grExternalTextureInfo);
    if (backendTexturePtr == nullptr) {
        return false;
    }
    sk_sp<SkImage> image = SkImage::MakeFromTexture(context, *backendTexturePtr,
        kTopLeft_GrSurfaceOrigin, colorType, kPremul_SkAlphaType, nullptr);
#endif
    if (image == nullptr) {
        RS_LOGE("make skImage fail");
        return false;
    }

    SkCanvas* canvas = surface->getCanvas();
    if (canvas == nullptr) {
        return false;
    }
    SkPaint paint;
    paint.setStyle(SkPaint::kFill_Style);
    SkSamplingOptions sampling(SkFilterMode::kNearest);
    {
        ScopedBytrace trace2("DrawImage::drawImageRect");
        canvas->drawImageRect(image,
            SkRect::MakeXYWH(srcRect.left, srcRect.top, srcRect.width, srcRect.height),
            SkRect::MakeWH(srcRect.width, srcRect.height),
            sampling, &paint, SkCanvas::kStrict_SrcRectConstraint);
    }
    {
        ScopedBytrace trace3("flushAndSubmit");
#ifdef USE_M133_SKIA
        GrDirectContext* directContext = GrAsDirectContext(surface->recordingContext());
        if (directContext == nullptr) {
            return false;
        }
        directContext->flushAndSubmit(GrSyncCpu::kYes);
#else
        surface->flushAndSubmit(true);
#endif
    }
    return true;
#else
    return false;
#endif
}

std::unique_ptr<PixelMap> PixelMapFromSurface::Create(sptr<Surface> surface, const OHOS::Media::Rect &srcRect)
{
    ScopedBytrace trace(__func__);
    if (surface == nullptr) {
        RS_LOGE("invalid argument: surface is nullptr");
        return nullptr;
    }
    if (srcRect.left < 0 || srcRect.top < 0 || srcRect.width <= 0 || srcRect.height <= 0) {
        RS_LOGE("invalid argument: srcRect[%{public}d, %{public}d, %{public}d, %{public}d]",
            srcRect.left, srcRect.top, srcRect.width, srcRect.height);
        return nullptr;
    }
#if defined(RS_ENABLE_UNI_RENDER) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    if (RSBackgroundThread::Instance().GetShareGPUContext() == nullptr) {
        RS_LOGE("check GPUContext fail");
        return nullptr;
    }
#endif
    RS_LOGI("PixelMapFromSurface::Create in, srcRect[%{public}d, %{public}d, %{public}d, %{public}d]",
        srcRect.left, srcRect.top, srcRect.width, srcRect.height);
    surface_ = surface;

    std::unique_ptr<PixelMap> pixelMap = nullptr;
#if defined(RS_ENABLE_GL)
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::OPENGL) {
        pixelMap = CreateForGL(surface, srcRect);
    }
#endif

#if defined(RS_ENABLE_VK)
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        pixelMap = CreateForVK(surface, srcRect);
    }
#endif

    if (pixelMap == nullptr) {
        RS_LOGE("Create pixelMap fail");
        Clear();
    }
    return pixelMap;
}

std::unique_ptr<PixelMap> PixelMapFromSurface::Create(
    sptr<SurfaceBuffer> surfaceBuffer, const OHOS::Media::Rect &srcRect)
{
    ScopedBytrace trace(__func__);
    if (surfaceBuffer == nullptr) {
        RS_LOGE("surfaceBuffer invalid argument: surfaceBuffer is nullptr");
        return nullptr;
    }
    if (srcRect.left < 0 || srcRect.top < 0 || srcRect.width <= 0 || srcRect.height <= 0) {
        RS_LOGE("surfaceBuffer invalid argument: srcRect[%{public}d, %{public}d, %{public}d, %{public}d]",
            srcRect.left, srcRect.top, srcRect.width, srcRect.height);
        return nullptr;
    }
#if defined(RS_ENABLE_UNI_RENDER) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    if (RSBackgroundThread::Instance().GetShareGPUContext() == nullptr) {
        RS_LOGE("surfaceBuffer check GPUContext fail");
        return nullptr;
    }
#endif
    RS_LOGI("PixelMapFromSurface::Create surfaceBuffer srcRect[%{public}d, %{public}d, %{public}d, %{public}d]",
        srcRect.left, srcRect.top, srcRect.width, srcRect.height);

    std::unique_ptr<PixelMap> pixelMap = nullptr;
#if defined(RS_ENABLE_GL)
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::OPENGL) {
        pixelMap = CreateForGL(surfaceBuffer, srcRect);
    }
#endif

#if defined(RS_ENABLE_VK)
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        pixelMap = CreateForVK(surfaceBuffer, srcRect);
    }
#endif

    if (pixelMap == nullptr) {
        RS_LOGE("surfaceBuffer Create pixelMap fail");
        Clear();
    }
    return pixelMap;
}

std::shared_ptr<OHOS::Media::PixelMap> CreatePixelMapFromSurface(sptr<Surface> surface,
    const OHOS::Media::Rect &srcRect)
{
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    auto helper = std::make_unique<PixelMapFromSurface>();
    return helper->Create(surface, srcRect);
#else
    RS_LOGE("CreatePixelMapFromSurface fail");
    return nullptr;
#endif
}

std::shared_ptr<OHOS::Media::PixelMap> CreatePixelMapFromSurfaceBuffer(sptr<SurfaceBuffer> surfaceBuffer,
    const OHOS::Media::Rect &srcRect)
{
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    auto helper = std::make_unique<PixelMapFromSurface>();
    return helper->Create(surfaceBuffer, srcRect);
#else
    RS_LOGE("CreatePixelMapFromSurfaceBuffer fail");
    return nullptr;
#endif
}
} // namespace Rosen
} // namespace OHOS
