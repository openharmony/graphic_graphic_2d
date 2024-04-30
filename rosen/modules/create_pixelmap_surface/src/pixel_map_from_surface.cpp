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
#include "image/image.h"
#include "native_window.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#if defined(RS_ENABLE_VK)
#include "platform/ohos/backend/native_buffer_utils.h"
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
#include "include/gpu/GrBackendSurface.h"
#include "include/gpu/GrDirectContext.h"
#include "include/gpu/gl/GrGLInterface.h"


namespace OHOS {
namespace Rosen {
using namespace OHOS::Media;

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
        eglDestroyImageKHR(disp, eglImage_);
        eglImage_ = EGL_NO_IMAGE_KHR;
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
    EGLint attrs[] = {
        EGL_IMAGE_PRESERVED,
        EGL_TRUE,
        EGL_NONE,
    };

    auto disp = eglGetDisplay(EGL_DEFAULT_DISPLAY);
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

    GrBackendTexture backendTexture(
        surfaceBuffer->GetWidth(), surfaceBuffer->GetHeight(), GrMipMapped::kNo, textureInfo);
    auto skSurface = SkSurface::MakeFromBackendTexture(context, backendTexture,
        kTopLeft_GrSurfaceOrigin, 0, kRGBA_8888_SkColorType, SkColorSpace::MakeSRGB(), nullptr);
    return skSurface;
}
#endif

class PixelMapFromSurface {
public:
    PixelMapFromSurface();
    ~PixelMapFromSurface() noexcept;

    std::unique_ptr<OHOS::Media::PixelMap> Create(sptr<Surface> surface, const OHOS::Media::Rect &srcRect);

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
#endif
    std::unique_ptr<OHOS::Media::PixelMap> CreateForVK(const sptr<Surface> &surface, const OHOS::Media::Rect &srcRect);
    bool DrawImageRectVK(const std::shared_ptr<Drawing::Image> &drawingImage,
        OHNativeWindowBuffer *nativeWindowBufferTmp, const sptr<SurfaceBuffer> &surfaceBufferTmp,
        const OHOS::Media::Rect &srcRect);
    std::shared_ptr<Drawing::Image> CreateDrawingImage();

    sptr<SurfaceBuffer> surfaceBuffer_;
    OHNativeWindowBuffer *nativeWindowBuffer_ = nullptr;
#if defined(RS_ENABLE_GL)
    GLuint texId_ = 0U;
    EGLImageKHR eglImage_ = EGL_NO_IMAGE_KHR;
#endif
    std::unique_ptr<OHOS::Media::PixelMap> CreateForGL(const sptr<Surface> &surface, const OHOS::Media::Rect &srcRect);

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
#if defined(RS_ENABLE_GL)
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::OPENGL) {
        if (eglImage_ != EGL_NO_IMAGE_KHR) {
            auto disp = eglGetDisplay(EGL_DEFAULT_DISPLAY);
            eglDestroyImageKHR(disp, eglImage_);
            eglImage_ = EGL_NO_IMAGE_KHR;
        }
        if (texId_ != 0U) {
            glDeleteTextures(1, &texId_);
            texId_ = 0U;
        }
    }
#endif

    if (nativeWindowBuffer_ != nullptr) {
        DestroyNativeWindowBuffer(nativeWindowBuffer_);
        nativeWindowBuffer_ = nullptr;
    }

    surfaceBuffer_ = nullptr;
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

std::unique_ptr<OHOS::Media::PixelMap> PixelMapFromSurface::CreateForGL(const sptr<Surface> &surface,
    const OHOS::Media::Rect &srcRect)
{
#if defined(RS_ENABLE_GL) && defined(RS_ENABLE_UNI_RENDER)
    ScopedBytrace trace(__func__);
    nativeWindowBuffer_ = GetNativeWindowBufferFromSurface(surfaceBuffer_, surface, srcRect);
    if (!nativeWindowBuffer_ || !surfaceBuffer_) {
        return nullptr;
    }

    auto gpuContext = RSBackgroundThread::Instance().GetShareGPUContext();
    if (!gpuContext) {
        return nullptr;
    }

    sk_sp<GrDirectContext> grContext = nullptr;
    auto skiaGpuContext = gpuContext->GetImpl<Drawing::SkiaGPUContext>();
    if (skiaGpuContext) {
        grContext = skiaGpuContext->GetGrContext();
    }
    if (!grContext) {
        return nullptr;
    }

    if (!CreateEGLImage()) {
        return nullptr;
    }
    DmaMem dmaMem;
    InitializationOptions options;
    options.size.width = srcRect.width;
    options.size.height = srcRect.height;
    options.srcPixelFormat = PixelFormat::RGBA_8888;
    options.pixelFormat = PixelFormat::RGBA_8888;
    auto pixelMap = PixelMap::Create(options);

    sptr<SurfaceBuffer> surfaceBuffer = DmaMem::DmaMemAlloc(srcRect.width, srcRect.height, pixelMap);
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
#else
    return nullptr;
#endif
}

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


bool PixelMapFromSurface::DrawImageRectVK(const std::shared_ptr<Drawing::Image> &drawingImage,
    OHNativeWindowBuffer *nativeWindowBufferTmp, const sptr<SurfaceBuffer> &surfaceBufferTmp,
    const OHOS::Media::Rect &srcRect)
{
#if defined(RS_ENABLE_VK)
    ScopedBytrace trace1(__func__);
    Drawing::BackendTexture backendTextureTmp =
        NativeBufferUtils::MakeBackendTextureFromNativeBuffer(nativeWindowBufferTmp,
        surfaceBufferTmp->GetWidth(), surfaceBufferTmp->GetHeight());
    if (!backendTextureTmp.IsValid()) {
        return false;
    }
    auto vkTextureInfo = backendTextureTmp.GetTextureInfo().GetVKTextureInfo();
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
        cleanUpHelper->UnRef();
        return false;
    }
    auto canvas = drawingSurface->GetCanvas();
    Drawing::Paint paint;
    paint.SetStyle(Drawing::Paint::PaintStyle::PAINT_FILL);
    canvas->AttachPaint(paint);
    canvas->DrawImageRect(*drawingImage,
        OHOS::Rosen::Drawing::Rect(srcRect.left, srcRect.top, srcRect.width, srcRect.height),
        OHOS::Rosen::Drawing::Rect(srcRect.left, srcRect.top, srcRect.width, srcRect.height),
        Drawing::SamplingOptions(Drawing::FilterMode::NEAREST),
        OHOS::Rosen::Drawing::SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT);
    {
        ScopedBytrace trace2("FlushAndSubmit");
        drawingSurface->FlushAndSubmit(true);
    }
    return true;
#else
    return false;
#endif
}

std::shared_ptr<Drawing::Image> PixelMapFromSurface::CreateDrawingImage()
{
#if defined(RS_ENABLE_VK) && defined(RS_ENABLE_UNI_RENDER)
    ScopedBytrace trace(__func__);
    backendTexture_ = NativeBufferUtils::MakeBackendTextureFromNativeBuffer(nativeWindowBuffer_,
        surfaceBuffer_->GetWidth(), surfaceBuffer_->GetHeight());
    if (!backendTexture_.IsValid()) {
        return nullptr;
    }
    auto vkTextureInfo = backendTexture_.GetTextureInfo().GetVKTextureInfo();
    if (!vkTextureInfo) {
        return nullptr;
    }

    NativeBufferUtils::VulkanCleanupHelper* cleanUpHelper =
        new NativeBufferUtils::VulkanCleanupHelper(RsVulkanContext::GetSingleton(),
        vkTextureInfo->vkImage, vkTextureInfo->vkAlloc.memory);
    if (!cleanUpHelper) {
        return nullptr;
    }

    Drawing::BitmapFormat bitmapFormat = { GetColorTypeFromVKFormat(vkTextureInfo->format),
        Drawing::AlphaType::ALPHATYPE_PREMUL };
    std::shared_ptr<Drawing::Image> drawingImage = std::make_shared<Drawing::Image>();
    if (!drawingImage) {
        cleanUpHelper->UnRef();
        return nullptr;
    }

    if (!drawingImage->BuildFromTexture(*(RSBackgroundThread::Instance().GetShareGPUContext().get()),
        backendTexture_.GetTextureInfo(),
        Drawing::TextureOrigin::TOP_LEFT, bitmapFormat, nullptr,
        NativeBufferUtils::DeleteVkImage,
        cleanUpHelper)) {
        cleanUpHelper->UnRef();
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
    if (!nativeWindowBuffer_ || !surfaceBuffer_) {
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
        return nullptr;
    }

    sptr<SurfaceBuffer> surfaceBufferTmp = LocalDmaMemAlloc(srcRect.width, srcRect.height, pixelMap);
    if (!surfaceBufferTmp) {
        return nullptr;
    }

    OHNativeWindowBuffer *nativeWindowBufferTmp = CreateNativeWindowBufferFromSurfaceBuffer(&surfaceBufferTmp);
    if (!nativeWindowBufferTmp) {
        return nullptr;
    }

    if (!DrawImageRectVK(drawingImage, nativeWindowBufferTmp, surfaceBufferTmp, srcRect)) {
        DestroyNativeWindowBuffer(nativeWindowBufferTmp);
        return nullptr;
    }
    DestroyNativeWindowBuffer(nativeWindowBufferTmp);
    return pixelMap;
#else
    return nullptr;
#endif
}

OHNativeWindowBuffer *PixelMapFromSurface::GetNativeWindowBufferFromSurface(
    sptr<SurfaceBuffer> &surfaceBuffer, const sptr<Surface> &surface, const OHOS::Media::Rect &srcRect)
{
    ScopedBytrace trace(__func__);
    // private func, surface is not nullptr.
    sptr<SyncFence> fence;
    // a 4 * 4 idetity matrix
    float matrix[16] = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };
    int ret = surface->GetLastFlushedBuffer(surfaceBuffer, fence, matrix);
    if (ret != OHOS::GSERROR_OK || surfaceBuffer == nullptr) {
        return nullptr;
    }

    int bufferWidth = surfaceBuffer->GetWidth();
    int bufferHeight = surfaceBuffer->GetHeight();
    if (srcRect.width > bufferWidth || srcRect.height > bufferHeight ||
        srcRect.left >= bufferWidth || srcRect.top >= bufferHeight ||
        srcRect.left + srcRect.width > bufferWidth || srcRect.top + srcRect.height > bufferHeight) {
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
    eglImage_ = eglCreateImageKHR(disp, EGL_NO_CONTEXT, EGL_NATIVE_BUFFER_OHOS, nativeWindowBuffer_, attrs);
    if (eglImage_ == EGL_NO_IMAGE_KHR) {
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
    auto backendTexturePtr =
        std::make_shared<GrBackendTexture>(bufferWidth, bufferHeight, GrMipMapped::kNo, grExternalTextureInfo);

    sk_sp<SkImage> image = SkImage::MakeFromTexture(context, *backendTexturePtr,
        kTopLeft_GrSurfaceOrigin, colorType, kPremul_SkAlphaType, nullptr);
    if (image == nullptr) {
        return false;
    }

    SkCanvas* canvas = surface->getCanvas();
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
        surface->flushAndSubmit(true);
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
        return nullptr;
    }
    if (srcRect.left < 0 || srcRect.top < 0 || srcRect.width <= 0 || srcRect.height <= 0) {
        RS_LOGE("invalid argument: srcRect[%{public}d, %{public}d, %{public}d, %{public}d]",
            srcRect.left, srcRect.top, srcRect.width, srcRect.height);
        return nullptr;
    }

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

std::shared_ptr<OHOS::Media::PixelMap> CreatePixelMapFromSurface(sptr<Surface> surface,
    const OHOS::Media::Rect &srcRect)
{
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    auto helper = std::make_unique<PixelMapFromSurface>();
    return helper->Create(surface, srcRect);
#else
    return nullptr;
#endif
}
} // namespace Rosen
} // namespace OHOS
