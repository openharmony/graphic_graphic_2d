/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.. All rights reserved.
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

#include "skia_surface.h"

#include "include/gpu/GrBackendSemaphore.h"

#include "draw/surface.h"
#include "utils/log.h"

#include "skia_bitmap.h"
#include "skia_canvas.h"
#ifdef ACE_ENABLE_GPU
#include "skia_gpu_context.h"
#endif
#include "skia_image.h"
#include "skia_image_info.h"
#include "skia_texture_info.h"
#include "utils/system_properties.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
#ifdef ACE_ENABLE_GPU
static constexpr int TEXTURE_SAMPLE_COUNT = 0;
static constexpr int FB_SAMPLE_COUNT = 0;
static constexpr int STENCIL_BITS = 8;
#ifdef NEW_SKIA
static constexpr uint32_t SURFACE_PROPS_FLAGS = 0;
#endif
#endif

namespace {
SkSurface::BackendHandleAccess ConvertToSkiaBackendAccess(BackendAccess access)
{
    switch (access) {
        case BackendAccess::FLUSH_READ:
            return SkSurface::BackendHandleAccess::kFlushRead_BackendHandleAccess;
        case BackendAccess::FLUSH_WRITE:
            return SkSurface::BackendHandleAccess::kFlushWrite_BackendHandleAccess;
        case BackendAccess::DISCARD_WRITE:
            return SkSurface::BackendHandleAccess::kDiscardWrite_BackendHandleAccess;
        default:
            return SkSurface::BackendHandleAccess::kFlushRead_BackendHandleAccess;
    }
    return SkSurface::BackendHandleAccess::kFlushRead_BackendHandleAccess;
}
}

SkiaSurface::SkiaSurface() {}

bool SkiaSurface::Bind(const Bitmap& bitmap)
{
    const auto &skBitmap = bitmap.GetImpl<SkiaBitmap>()->ExportSkiaBitmap();
    skSurface_ = SkSurface::MakeRasterDirect(skBitmap.info(), skBitmap.getPixels(), skBitmap.rowBytes());
    if (skSurface_ == nullptr) {
        LOGD("SkiaSurface bind Bitmap failed: skSurface is nullptr");
        return false;
    }
    return true;
}

#ifdef ACE_ENABLE_GPU
bool SkiaSurface::Bind(const Image& image)
{
    auto skiaImageImpl = image.GetImpl<SkiaImage>();
    auto skImage = skiaImageImpl->GetImage();
    auto grContext = skiaImageImpl->GetGrContext();
    if (skImage == nullptr || grContext == nullptr) {
        LOGD("SkiaSurface bind Image failed: image is not GPU image");
        return false;
    }

    if (skImage->isLazyGenerated()) {
        skImage = skImage->makeTextureImage(grContext.get());
    }

    GrSurfaceOrigin grSurfaceOrigin;
    GrBackendTexture grBackendTexture;
#ifdef NEW_SKIA
    SkSurfaceProps surfaceProps(SURFACE_PROPS_FLAGS, kRGB_H_SkPixelGeometry);
#else
    SkSurfaceProps surfaceProps = SkSurfaceProps::kLegacyFontHost_InitType;
#endif
    grBackendTexture = skImage->getBackendTexture(false, &grSurfaceOrigin);
    if (!grBackendTexture.isValid()) {
        LOGD("SkiaSurface bind Image failed: BackendTexture is invalid");
        return false;
    }

    skSurface_ = SkSurface::MakeFromBackendTexture(grContext.get(), grBackendTexture, grSurfaceOrigin,
        TEXTURE_SAMPLE_COUNT, skImage->colorType(), skImage->refColorSpace(), &surfaceProps);
    if (skSurface_ == nullptr) {
        LOGD("SkiaSurface bind Image failed: skSurface is nullptr");
        return false;
    }

    skImage_ = skImage;
    return true;
}

bool SkiaSurface::Bind(const FrameBuffer& frameBuffer)
{
    if (frameBuffer.gpuContext == nullptr) {
        LOGD("SkiaSurface bind FBO failed: gpuContext is invalid");
        return false;
    }
    auto skiaContext = frameBuffer.gpuContext->GetImpl<SkiaGPUContext>();

    GrGLFramebufferInfo framebufferInfo;
    framebufferInfo.fFBOID = frameBuffer.FBOID;
    framebufferInfo.fFormat = frameBuffer.Format;
    GrBackendRenderTarget backendRenderTarget(
        frameBuffer.width, frameBuffer.height, FB_SAMPLE_COUNT, STENCIL_BITS, framebufferInfo);

    SkColorType colorType = SkiaImageInfo::ConvertToSkColorType(frameBuffer.colorType);
    sk_sp<SkColorSpace> skColorSpace = nullptr;
    if (frameBuffer.colorSpace != nullptr) {
        skColorSpace = frameBuffer.colorSpace->GetImpl<SkiaColorSpace>()->GetColorSpace();
    }

#ifdef NEW_SKIA
    SkSurfaceProps surfaceProps(SURFACE_PROPS_FLAGS, kRGB_H_SkPixelGeometry);
#else
    SkSurfaceProps surfaceProps = SkSurfaceProps::kLegacyFontHost_InitType;
#endif

    skSurface_ = SkSurface::MakeFromBackendRenderTarget(skiaContext->GetGrContext().get(),
        backendRenderTarget, kBottomLeft_GrSurfaceOrigin, colorType, skColorSpace, &surfaceProps);
    if (skSurface_ == nullptr) {
        LOGD("SkiaSurface bind FBO failed: skSurface is nullptr");
        return false;
    }
    return true;
}

#ifdef RS_ENABLE_VK
std::shared_ptr<Surface> SkiaSurface::MakeFromBackendRenderTarget(GPUContext* gpuContext, const TextureInfo& info,
    TextureOrigin origin, ColorType colorType, std::shared_ptr<ColorSpace> colorSpace,
    void (*deleteVkImage)(void *), void* cleanHelper)
{
    if (SystemProperties::GetGpuApiType() != GpuApiType::VULKAN &&
        SystemProperties::GetGpuApiType() != GpuApiType::DDGR) {
        return nullptr;
    }
    sk_sp<GrDirectContext> grContext = nullptr;
    if (gpuContext) {
        auto skiaGpuContext = gpuContext->GetImpl<SkiaGPUContext>();
        if (skiaGpuContext) {
            grContext = skiaGpuContext->GetGrContext();
        }
    }
    GrVkImageInfo image_info;
    SkiaTextureInfo::ConvertToGrBackendTexture(info).getVkImageInfo(&image_info);
    GrBackendRenderTarget backendRenderTarget(info.GetWidth(), info.GetHeight(), 0, image_info);
    SkSurfaceProps surfaceProps(0, SkPixelGeometry::kUnknown_SkPixelGeometry);

    sk_sp<SkColorSpace> skColorSpace = nullptr;
    if (colorSpace != nullptr) {
        skColorSpace = colorSpace->GetImpl<SkiaColorSpace>()->GetColorSpace();
    } else {
        skColorSpace = SkColorSpace::MakeSRGB();
    }

    sk_sp<SkSurface> skSurface =
        SkSurface::MakeFromBackendRenderTarget(grContext.get(),
        backendRenderTarget, SkiaTextureInfo::ConvertToGrSurfaceOrigin(origin),
        SkiaImageInfo::ConvertToSkColorType(colorType),
        skColorSpace, &surfaceProps, deleteVkImage, cleanHelper);
    if (skSurface == nullptr) {
        return nullptr;
    }

    std::shared_ptr<Surface> surface = std::make_shared<Surface>();
    surface->GetImpl<SkiaSurface>()->SetSkSurface(skSurface);
    return surface;
}
std::shared_ptr<Surface> SkiaSurface::MakeFromBackendTexture(GPUContext* gpuContext, const TextureInfo& info,
    TextureOrigin origin, int sampleCnt, ColorType colorType,
    std::shared_ptr<ColorSpace> colorSpace, void (*deleteVkImage)(void *), void* cleanHelper)
{
    if (SystemProperties::GetGpuApiType() != GpuApiType::VULKAN &&
        SystemProperties::GetGpuApiType() != GpuApiType::DDGR) {
        return nullptr;
    }
    sk_sp<GrDirectContext> grContext = nullptr;
    if (gpuContext) {
        auto skiaGpuContext = gpuContext->GetImpl<SkiaGPUContext>();
        if (skiaGpuContext) {
            grContext = skiaGpuContext->GetGrContext();
        }
    }
    sk_sp<SkColorSpace> skColorSpace = nullptr;
    if (colorSpace != nullptr) {
        skColorSpace = colorSpace->GetImpl<SkiaColorSpace>()->GetColorSpace();
    } else {
        skColorSpace = SkColorSpace::MakeSRGB();
    }

    GrVkImageInfo image_info;
    SkiaTextureInfo::ConvertToGrBackendTexture(info).getVkImageInfo(&image_info);
    GrBackendTexture backendRenderTarget(info.GetWidth(), info.GetHeight(), image_info);
    SkSurfaceProps surfaceProps(0, SkPixelGeometry::kUnknown_SkPixelGeometry);

    sk_sp<SkSurface> skSurface =
        SkSurface::MakeFromBackendTexture(grContext.get(),
        backendRenderTarget, SkiaTextureInfo::ConvertToGrSurfaceOrigin(origin),
        sampleCnt, SkiaImageInfo::ConvertToSkColorType(colorType),
        skColorSpace, &surfaceProps, deleteVkImage, cleanHelper);
    if (skSurface == nullptr) {
        return nullptr;
    }

    std::shared_ptr<Surface> surface = std::make_shared<Surface>();
    surface->GetImpl<SkiaSurface>()->SetSkSurface(skSurface);
    return surface;
}
#endif

std::shared_ptr<Surface> SkiaSurface::MakeRenderTarget(GPUContext* gpuContext,
    bool budgeted, const ImageInfo& imageInfo)
{
    sk_sp<GrDirectContext> grContext = nullptr;
    if (gpuContext) {
        auto skiaGpuContext = gpuContext->GetImpl<SkiaGPUContext>();
        if (skiaGpuContext) {
            grContext = skiaGpuContext->GetGrContext();
        }
    }
    SkImageInfo skImageInfo = SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    sk_sp<SkSurface> skSurface =
        SkSurface::MakeRenderTarget(grContext.get(), static_cast<SkBudgeted>(budgeted), skImageInfo);
    if (skSurface == nullptr) {
        LOGD("skSurface nullptr, %{public}s, %{public}d [%{public}d %{public}d]", __FUNCTION__, __LINE__,
            imageInfo.GetWidth(), imageInfo.GetHeight());
        return nullptr;
    }
    std::shared_ptr<Surface> surface = std::make_shared<Surface>();
    surface->GetImpl<SkiaSurface>()->SetSkSurface(skSurface);
    return surface;
}
#endif

std::shared_ptr<Surface> SkiaSurface::MakeRaster(const ImageInfo& imageInfo)
{
    SkImageInfo skImageInfo = SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    sk_sp<SkSurface> skSurface = SkSurface::MakeRaster(skImageInfo);
    if (skSurface == nullptr) {
        LOGD("skSurface nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return nullptr;
    }
    std::shared_ptr<Surface> surface = std::make_shared<Surface>();
    surface->GetImpl<SkiaSurface>()->SetSkSurface(skSurface);
    return surface;
}

std::shared_ptr<Surface> SkiaSurface::MakeRasterDirect(const ImageInfo& imageInfo, void* pixels, size_t rowBytes)
{
    SkImageInfo skImageInfo = SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    sk_sp<SkSurface> skSurface = SkSurface::MakeRasterDirect(skImageInfo, pixels, rowBytes);
    if (skSurface == nullptr) {
        LOGD("skSurface nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return nullptr;
    }
    std::shared_ptr<Surface> surface = std::make_shared<Surface>();
    surface->GetImpl<SkiaSurface>()->SetSkSurface(skSurface);
    return surface;
}

std::shared_ptr<Surface> SkiaSurface::MakeRasterN32Premul(int32_t width, int32_t height)
{
    sk_sp<SkSurface> skSurface = SkSurface::MakeRasterN32Premul(width, height);
    if (skSurface == nullptr) {
        LOGD("skSurface nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return nullptr;
    }
    std::shared_ptr<Surface> surface = std::make_shared<Surface>();
    surface->GetImpl<SkiaSurface>()->SetSkSurface(skSurface);
    return surface;
}

std::shared_ptr<Canvas> SkiaSurface::GetCanvas() const
{
    if (skSurface_ == nullptr || skSurface_->getCanvas() == nullptr) {
        LOGD("skSurface is invalid");
        return nullptr;
    }

    auto canvas = std::make_shared<Canvas>();
    canvas->GetImpl<SkiaCanvas>()->ImportSkCanvas(skSurface_->getCanvas());
    return canvas;
}

std::shared_ptr<Image> SkiaSurface::GetImageSnapshot() const
{
    if (skSurface_ == nullptr) {
        LOGD("skSurface is nullptr");
        return nullptr;
    }

    auto skImage = skSurface_->makeImageSnapshot();
    if (skImage == nullptr) {
        LOGD("skSurface makeImageSnashot failed");
        return nullptr;
    }

    auto image = std::make_shared<Image>();
    image->GetImpl<SkiaImage>()->SetSkImage(skImage);
    return image;
}

std::shared_ptr<Image> SkiaSurface::GetImageSnapshot(const RectI& bounds) const
{
    if (skSurface_ == nullptr) {
        LOGD("skSurface is nullptr");
        return nullptr;
    }

    auto iRect = SkIRect::MakeLTRB(bounds.GetLeft(), bounds.GetTop(), bounds.GetRight(), bounds.GetBottom());
    auto skImage = skSurface_->makeImageSnapshot(iRect);
    if (skImage == nullptr) {
        LOGD("skSurface makeImageSnashot failed");
        return nullptr;
    }

    auto image = std::make_shared<Image>();
    image->GetImpl<SkiaImage>()->SetSkImage(skImage);
    return image;
}

BackendTexture SkiaSurface::GetBackendTexture(BackendAccess access) const
{
    if (skSurface_ == nullptr) {
        LOGD("skSurface is nullptr");
        return {};
    }

    GrBackendTexture grBackendTexture = skSurface_->getBackendTexture(ConvertToSkiaBackendAccess(access));
    auto backendTexture = BackendTexture(true);
#ifdef RS_ENABLE_VK
    if (SystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        SystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        TextureInfo info;
        SkiaTextureInfo::ConvertToVKTexture(grBackendTexture, info);
        backendTexture.SetTextureInfo(info);
    } else {
        backendTexture.SetTextureInfo(SkiaTextureInfo::ConvertToTextureInfo(grBackendTexture));
    }
#else
    backendTexture.SetTextureInfo(SkiaTextureInfo::ConvertToTextureInfo(grBackendTexture));
#endif
    return backendTexture;
}

std::shared_ptr<Surface> SkiaSurface::MakeSurface(int width, int height) const
{
    if (skSurface_ == nullptr) {
        LOGD("skSurface is nullptr");
        return nullptr;
    }
    auto surface = skSurface_->makeSurface(width, height);
    if (surface == nullptr) {
        LOGD("SkiaSurface::MakeSurface failed");
        return nullptr;
    }

    auto drawingSurface = std::make_shared<Surface>();
    drawingSurface->GetImpl<SkiaSurface>()->SetSkSurface(surface);
    return drawingSurface;
}

void SkiaSurface::SetSkSurface(const sk_sp<SkSurface>& skSurface)
{
    skSurface_ = skSurface;
}

sk_sp<SkSurface> SkiaSurface::GetSkSurface() const
{
    return skSurface_;
}

void SkiaSurface::FlushAndSubmit(bool syncCpu)
{
    if (skSurface_ == nullptr) {
        LOGD("skSurface is nullptr");
        return;
    }

    skSurface_->flushAndSubmit(syncCpu);
}

void SkiaSurface::Flush(FlushInfo *drawingflushInfo)
{
    if (skSurface_ == nullptr) {
        LOGD("skSurface is nullptr");
        return;
    }
    if (drawingflushInfo != nullptr) {
        GrFlushInfo flushInfo;
        flushInfo.fNumSemaphores = drawingflushInfo->numSemaphores;
        flushInfo.fSignalSemaphores = static_cast<GrBackendSemaphore*>(drawingflushInfo->backendSemaphore);
        flushInfo.fFinishedProc = drawingflushInfo->finishedProc;
        flushInfo.fFinishedContext = static_cast<GrGpuFinishedContext>(drawingflushInfo->finishedContext);
        flushInfo.fSubmittedProc = drawingflushInfo->submittedProc;
        flushInfo.fSubmittedContext = static_cast<GrGpuSubmittedContext>(drawingflushInfo->submittedContext);
        skSurface_->flush(drawingflushInfo->backendSurfaceAccess == false ?
            SkSurface::BackendSurfaceAccess::kNoAccess : SkSurface::BackendSurfaceAccess::kPresent, flushInfo);
        return;
    }
    skSurface_->flush();
}

#ifdef RS_ENABLE_VK
void SkiaSurface::Wait(int32_t time, const VkSemaphore& semaphore)
{
    if (SystemProperties::GetGpuApiType() != GpuApiType::VULKAN &&
        SystemProperties::GetGpuApiType() != GpuApiType::DDGR) {
        return;
    }

    if (skSurface_ == nullptr) {
        LOGD("skSurface is nullptr");
        return;
    }
    GrBackendSemaphore backendSemaphore;
    backendSemaphore.initVulkan(semaphore);
    skSurface_->wait(time, &backendSemaphore);
}

void SkiaSurface::SetDrawingArea(const std::vector<RectI>& rects)
{
    if (SystemProperties::GetGpuApiType() != GpuApiType::VULKAN &&
        SystemProperties::GetGpuApiType() != GpuApiType::DDGR) {
        return;
    }
    if (skSurface_ == nullptr) {
        LOGD("skSurface is nullptr");
        return;
    }
    std::vector<SkIRect> skIRects;
    for (auto &rect : rects) {
        SkIRect skIRect = {rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom()};
        skIRects.push_back(skIRect);
    }
    skSurface_->setDrawingArea(skIRects);
}

void SkiaSurface::ClearDrawingArea()
{
    if (skSurface_ == nullptr) {
        LOGD("skSurface is nullptr");
        return;
    }
    skSurface_->clearDrawingArea();
}
#endif

int SkiaSurface::Width() const
{
    if (skSurface_ == nullptr) {
        LOGW("skSurface is nullptr");
        return 0;
    }
    return skSurface_->width();
}

int SkiaSurface::Height() const
{
    if (skSurface_ == nullptr) {
        LOGW("skSurface is nullptr");
        return 0;
    }
    return skSurface_->height();
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
