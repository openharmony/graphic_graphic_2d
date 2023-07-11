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

#ifdef ACE_ENABLE_GPU
#include "include/gpu/GrBackendSurface.h"
#endif

#include "draw/surface.h"
#include "utils/log.h"
#include "skia_image.h"
#include "skia_bitmap.h"
#include "skia_canvas.h"
#ifdef ACE_ENABLE_GPU
#include "skia_gpu_context.h"
#endif

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

SkiaSurface::SkiaSurface() {}

bool SkiaSurface::Bind(const Bitmap& bitmap)
{
    const auto &skBitmap = bitmap.GetImpl<SkiaBitmap>()->ExportSkiaBitmap();
    skSurface_ = SkSurface::MakeRasterDirect(skBitmap.info(), skBitmap.getPixels(), skBitmap.rowBytes());
    if (skSurface_ == nullptr) {
        LOGE("SkiaSurface bind Bitmap failed: skSurface is nullptr");
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
        LOGE("SkiaSurface bind Image failed: image is not GPU image");
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
        LOGE("SkiaSurface bind Image failed: BackendTexture is invalid");
        return false;
    }

    skSurface_ = SkSurface::MakeFromBackendTexture(grContext.get(), grBackendTexture, grSurfaceOrigin,
        TEXTURE_SAMPLE_COUNT, skImage->colorType(), skImage->refColorSpace(), &surfaceProps);
    if (skSurface_ == nullptr) {
        LOGE("SkiaSurface bind Image failed: skSurface is nullptr");
        return false;
    }

    skImage_ = skImage;
    return true;
}

bool SkiaSurface::Bind(const FrameBuffer& frameBuffer)
{
    if (frameBuffer.gpuContext == nullptr) {
        LOGE("SkiaSurface bind FBO failed: gpuContext is invalid");
        return false;
    }
    auto skiaContext = frameBuffer.gpuContext->GetImpl<SkiaGPUContext>();

    GrGLFramebufferInfo framebufferInfo;
    framebufferInfo.fFBOID = frameBuffer.FBOID;
    framebufferInfo.fFormat = frameBuffer.Format;
    GrBackendRenderTarget backendRenderTarget(
        frameBuffer.width, frameBuffer.height, FB_SAMPLE_COUNT, STENCIL_BITS, framebufferInfo);

    SkColorType colorType = kRGBA_8888_SkColorType;
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
        LOGE("SkiaSurface bind FBO failed: skSurface is nullptr");
        return false;
    }
    return true;
}
#endif

std::shared_ptr<Canvas> SkiaSurface::GetCanvas() const
{
    if (skSurface_ == nullptr || skSurface_->getCanvas() == nullptr) {
        LOGE("skSurface is invalid");
        return nullptr;
    }

    auto canvas = std::make_shared<Canvas>();
    canvas->GetImpl<SkiaCanvas>()->ImportSkCanvas(skSurface_->getCanvas());
    return canvas;
}

std::shared_ptr<Image> SkiaSurface::GetImageSnapshot() const
{
    if (skSurface_ == nullptr) {
        LOGE("skSurface is nullptr");
        return nullptr;
    }

    auto skImage = skSurface_->makeImageSnapshot();
    if (skImage == nullptr) {
        LOGE("skSurface makeImageSnashot failed");
        return nullptr;
    }

    auto image = std::make_shared<Image>();
    image->GetImpl<SkiaImage>()->SetSkImage(skImage);
    return image;
}

std::shared_ptr<Image> SkiaSurface::GetImageSnapshot(const RectI& bounds) const
{
    if (skSurface_ == nullptr) {
        LOGE("skSurface is nullptr");
        return nullptr;
    }

    auto iRect = SkIRect::MakeLTRB(bounds.GetLeft(), bounds.GetTop(), bounds.GetRight(), bounds.GetBottom());
    auto skImage = skSurface_->makeImageSnapshot(iRect);
    if (skImage == nullptr) {
        LOGE("skSurface makeImageSnashot failed");
        return nullptr;
    }

    auto image = std::make_shared<Image>();
    image->GetImpl<SkiaImage>()->SetSkImage(skImage);
    return image;
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
