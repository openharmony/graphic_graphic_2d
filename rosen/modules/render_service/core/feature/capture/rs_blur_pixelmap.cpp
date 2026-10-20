/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "rs_blur_pixelmap.h"

#if defined(ROSEN_OHOS) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
#include <cmath>
#include "draw/surface.h"
#include "image/image.h"
#include "engine/rs_base_render_engine.h"
#include "feature/hdr/rs_colorspace_util.h"
#include "ge_render.h"
#include "ge_visual_effect_container.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "render/rs_hps_blur.h"
#include "render/rs_render_mesa_blur_filter.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint8_t ASTC_HEADER_SIZE = 16;
constexpr float MIN_VALID_BLUR_RADIUS = 1E-6f;

Drawing::CompressedType PixelFormatToCompressedType(Media::PixelFormat format, bool isHdr)
{
    switch (format) {
        case Media::PixelFormat::ASTC_4x4:
            return isHdr ? Drawing::CompressedType::ASTC_RGBA10_4x4 :
                Drawing::CompressedType::ASTC_RGBA8_4x4;
        case Media::PixelFormat::ASTC_6x6:
            return Drawing::CompressedType::ASTC_RGBA8_6x6;
        case Media::PixelFormat::ASTC_8x8:
            return Drawing::CompressedType::ASTC_RGBA8_8x8;
        default:
            return Drawing::CompressedType::NoneType;
    }
}

std::shared_ptr<Drawing::Image> CreateGpuImageFromPixelMap(
    const std::shared_ptr<Media::PixelMap>& pixelMap, Drawing::GPUContext* gpuContext)
{
    if (pixelMap == nullptr || gpuContext == nullptr) {
        return nullptr;
    }
    auto colorSpace = RSColorSpaceUtil::ColorSpaceToDrawingColorSpace(
        pixelMap->InnerGetGrColorSpace().GetColorSpaceName());
    if (pixelMap->IsAstc()) {
        const void* data = pixelMap->GetPixels();
        if (data == nullptr || pixelMap->GetCapacity() <= ASTC_HEADER_SIZE) {
            RS_LOGE("CreateGpuImageFromPixelMap: invalid ASTC data");
            return nullptr;
        }
        auto fileData = std::make_shared<Drawing::Data>();
        if (!fileData->BuildWithoutCopy(
            reinterpret_cast<const void*>(reinterpret_cast<const char*>(data) + ASTC_HEADER_SIZE),
            pixelMap->GetCapacity() - ASTC_HEADER_SIZE)) {
            RS_LOGE("CreateGpuImageFromPixelMap: ASTC BuildWithoutCopy failed");
            return nullptr;
        }
        Media::ImageInfo imageInfo;
        pixelMap->GetImageInfo(imageInfo);
        Media::Size realSize;
        pixelMap->GetAstcRealSize(realSize);
        auto image = std::make_shared<Drawing::Image>();
        if (image->BuildFromCompressed(*gpuContext, fileData,
            static_cast<int>(realSize.width), static_cast<int>(realSize.height),
            PixelFormatToCompressedType(imageInfo.pixelFormat, pixelMap->IsHdr()), colorSpace)) {
            return image;
        }
        RS_LOGE("CreateGpuImageFromPixelMap: BuildFromCompressed failed");
        return nullptr;
    }
    auto colorType = pixelMap->GetPixelFormat() == Media::PixelFormat::RGBA_F16 ?
        Drawing::ColorType::COLORTYPE_RGBA_F16 : Drawing::ColorType::COLORTYPE_RGBA_8888;
    Drawing::ImageInfo info = Drawing::ImageInfo{pixelMap->GetWidth(), pixelMap->GetHeight(),
        colorType, Drawing::AlphaType::ALPHATYPE_PREMUL, colorSpace};
    auto image = Drawing::Image::MakeFromRaster(
        Drawing::Pixmap(info, const_cast<uint8_t*>(pixelMap->GetPixels()), pixelMap->GetRowBytes()),
        nullptr, nullptr);
    return image;
}

std::shared_ptr<Drawing::Surface> CreateGpuSurface(
    int32_t width, int32_t height, Drawing::GPUContext* gpuContext)
{
    if (gpuContext == nullptr || width <= 0 || height <= 0) {
        return nullptr;
    }
    Drawing::ImageInfo info = Drawing::ImageInfo{width, height,
        Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_PREMUL};
    return Drawing::Surface::MakeRenderTarget(gpuContext, false, info);
}

void ApplyBlurOnCanvas(RSPaintFilterCanvas& canvas,
    const std::shared_ptr<Drawing::Surface>& surface, float blurRadius)
{
    Drawing::AutoCanvasRestore autoRestore(canvas, true);
    canvas.ResetMatrix();
    auto image = surface->GetImageSnapshot();
    if (!image) {
        RS_LOGE("BlurPixelMap ApplyBlurOnCanvas: image is null");
        return;
    }
    auto hpsParam = Drawing::HpsBlurParameter(
        Drawing::Rect(0, 0, image->GetWidth(), image->GetHeight()),
        Drawing::Rect(0, 0, image->GetWidth(), image->GetHeight()),
        blurRadius, 1.f, 1.f);
    auto filter = HpsBlurFilter::GetHpsBlurFilter();
    if (!filter.ApplyHpsBlur(canvas, image, hpsParam, 1.f)) {
        RS_LOGW("BlurPixelMap ApplyHpsBlur failed, using MESA");
        auto mesaFilter = RSMESABlurShaderFilter(blurRadius);
        auto mesaContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
        mesaFilter.GenerateGEVisualEffect(mesaContainer);
        auto geRender = std::make_shared<GraphicsEffectEngine::GERender>();
        auto outImage = geRender->ApplyImageEffect(canvas, *mesaContainer,
            {image, Drawing::Rect(0, 0, image->GetWidth(), image->GetHeight()),
            Drawing::Rect(0, 0, image->GetWidth(), image->GetHeight()), nullptr},
            Drawing::SamplingOptions());
        if (outImage == nullptr) {
            RS_LOGE("BlurPixelMap MESA outImage is nullptr");
            return;
        }
        canvas.DrawImage(*outImage, 0, 0, Drawing::SamplingOptions());
    }
}

std::shared_ptr<Media::PixelMap> ReadBackToPixelMap(
    const std::shared_ptr<Drawing::Surface>& surface, int32_t width, int32_t height)
{
    if (surface == nullptr) {
        return nullptr;
    }
    auto image = surface->GetImageSnapshot();
    if (!image) {
        RS_LOGE("ReadBackToPixelMap: GetImageSnapshot failed");
        return nullptr;
    }
    Media::InitializationOptions opts;
    opts.size.width = width;
    opts.size.height = height;
    opts.pixelFormat = Media::PixelFormat::RGBA_8888;
    opts.alphaType = Media::AlphaType::IMAGE_ALPHA_TYPE_PREMUL;
    auto pixelMap = Media::PixelMap::Create(opts);
    if (pixelMap == nullptr) {
        RS_LOGE("ReadBackToPixelMap: PixelMap::Create failed");
        return nullptr;
    }
    Drawing::BitmapFormat format{Drawing::ColorType::COLORTYPE_RGBA_8888,
        Drawing::AlphaType::ALPHATYPE_PREMUL};
    Drawing::Bitmap bitmap;
    bitmap.Build(width, height, format);
    if (!image->ReadPixels(bitmap, 0, 0)) {
        RS_LOGE("ReadBackToPixelMap: ReadPixels failed");
        return nullptr;
    }
    auto size = static_cast<uint64_t>(pixelMap->GetRowBytes()) * pixelMap->GetHeight();
    if (memcpy_s(pixelMap->GetWritablePixels(), static_cast<size_t>(size),
        bitmap.GetPixels(), static_cast<size_t>(size)) != EOK) {
        RS_LOGE("ReadBackToPixelMap: memcpy_s failed");
        return nullptr;
    }
    return pixelMap;
}
} // namespace

bool BlurPixelMapOnRenderThread(
    const std::shared_ptr<Media::PixelMap>& pixelMap, float blurRadius,
    std::shared_ptr<Media::PixelMap>& blurredPixelMap)
{
    RS_LOGI("BlurPixelMapOnRenderThread start, blurRadius:%{public}f, w:%{public}d, h:%{public}d",
        blurRadius, pixelMap->GetWidth(), pixelMap->GetHeight());
    if (std::isnan(blurRadius) || std::isinf(blurRadius) || blurRadius < 0.f) {
        RS_LOGE("BlurPixelMapOnRenderThread: invalid blurRadius:%{public}f", blurRadius);
        return false;
    }
    if (blurRadius < MIN_VALID_BLUR_RADIUS) {
        RS_LOGW("BlurPixelMapOnRenderThread: blurRadius too small, skip blur");
        blurredPixelMap = pixelMap;
        return true;
    }
    auto renderEngine = RSUniRenderThread::Instance().GetRenderEngine();
    if (renderEngine == nullptr) {
        RS_LOGE("BlurPixelMapOnRenderThread: renderEngine is nullptr");
        return false;
    }
    auto renderContext = renderEngine->GetRenderContext();
    if (renderContext == nullptr) {
        RS_LOGE("BlurPixelMapOnRenderThread: renderContext is nullptr");
        return false;
    }
    renderContext->SetUpGpuContext();
    auto gpuContext = renderContext->GetDrGPUContext();
    if (gpuContext == nullptr) {
        RS_LOGE("BlurPixelMapOnRenderThread: gpuContext is nullptr");
        return false;
    }
    uint32_t maxWidth = 0;
    uint32_t maxHeight = 0;
    if (renderContext->QueryMaxGpuBufferSize(maxWidth, maxHeight)) {
        int32_t width = pixelMap->GetWidth();
        int32_t height = pixelMap->GetHeight();
        if (width <= 0 || height <= 0 ||
            static_cast<uint32_t>(width) >= maxWidth || static_cast<uint32_t>(height) >= maxHeight) {
            RS_LOGE("BlurPixelMapOnRenderThread: pixelMap dimension out of GPU limit, "
                "w:%{public}d, h:%{public}d, maxW:%{public}u, maxH:%{public}u",
                width, height, maxWidth, maxHeight);
            return false;
        }
    } else {
        RS_LOGW("BlurPixelMapOnRenderThread: QueryMaxGpuBufferSize failed, skip dimension check");
    }
    auto image = CreateGpuImageFromPixelMap(pixelMap, gpuContext);
    if (image == nullptr) {
        RS_LOGE("BlurPixelMapOnRenderThread: CreateGpuImageFromPixelMap failed");
        return false;
    }
    auto surface = CreateGpuSurface(pixelMap->GetWidth(), pixelMap->GetHeight(), gpuContext);
    if (surface == nullptr) {
        RS_LOGE("BlurPixelMapOnRenderThread: CreateGpuSurface failed");
        return false;
    }
    RSPaintFilterCanvas canvas(surface.get());
    canvas.DrawImage(*image, 0, 0, Drawing::SamplingOptions());
    surface->FlushAndSubmit(true);
    ApplyBlurOnCanvas(canvas, surface, blurRadius);
    surface->FlushAndSubmit(true);
    blurredPixelMap = ReadBackToPixelMap(surface, pixelMap->GetWidth(), pixelMap->GetHeight());
    RS_LOGI("BlurPixelMapOnRenderThread end, result:%{public}d", blurredPixelMap != nullptr);
    return blurredPixelMap != nullptr;
}
} // namespace Rosen
} // namespace OHOS
#endif // ROSEN_OHOS && (RS_ENABLE_GL || RS_ENABLE_VK)