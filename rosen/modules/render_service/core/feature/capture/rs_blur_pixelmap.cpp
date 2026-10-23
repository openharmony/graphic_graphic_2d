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
#include "rs_trace.h"
#ifdef RS_ENABLE_VK
#include "vulkan_context/rs_vulkan_context.h"
#include "vulkan_context/native_buffer_utils.h"
#include "surface_buffer.h"
#include "native_window.h"
#include "native_buffer_inner.h"
#include "sync_fence.h"
#ifdef USE_M133_SKIA
#include "include/gpu/ganesh/GrDirectContext.h"
#include "include/gpu/ganesh/GrBackendSemaphore.h"
#else
#include "include/gpu/GrDirectContext.h"
#include "include/gpu/GrBackendSemaphore.h"
#endif
#include "include/core/SkSurface.h"
#endif
#include "pipeline/render_thread/rs_uni_render_util.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint8_t ASTC_HEADER_SIZE = 16;
constexpr float MIN_VALID_BLUR_RADIUS = 1E-6f;
constexpr uint32_t DEFAULT_STRIDE_ALIGNMENT = 0x8;

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

bool CheckPixelMapDimension(RenderContext* renderContext,
    const std::shared_ptr<Media::PixelMap>& pixelMap)
{
    if (renderContext == nullptr || pixelMap == nullptr) {
        return true;
    }
    uint32_t maxWidth = 0;
    uint32_t maxHeight = 0;
    if (!renderContext->QueryMaxGpuBufferSize(maxWidth, maxHeight)) {
        RS_LOGW("CheckPixelMapDimension: QueryMaxGpuBufferSize failed, skip dimension check");
        return true;
    }
    int32_t width = pixelMap->GetWidth();
    int32_t height = pixelMap->GetHeight();
    if (width <= 0 || height <= 0 ||
        static_cast<uint32_t>(width) >= maxWidth ||
        static_cast<uint32_t>(height) >= maxHeight) {
        RS_LOGE("CheckPixelMapDimension: pixelMap dimension out of GPU limit, "
            "w:%{public}d, h:%{public}d, maxW:%{public}u, maxH:%{public}u",
            width, height, maxWidth, maxHeight);
        return false;
    }
    return true;
}
} // namespace

#ifdef RS_ENABLE_VK
class BlurPixelMapDmaMem {
public:
    BlurPixelMapDmaMem() = default;
    ~BlurPixelMapDmaMem()
    {
        ReleaseDmaMemory();
    }
    sptr<SurfaceBuffer> DmaMemAlloc(const Drawing::ImageInfo& dstInfo,
        const std::shared_ptr<Media::PixelMap>& pixelMap);
    std::shared_ptr<Drawing::Surface> GetSurfaceFromSurfaceBuffer(
        sptr<SurfaceBuffer> surfaceBuffer, Drawing::GPUContext* gpuContext,
        RenderContext* renderContext, Drawing::ColorType colorType);
    void ReleaseDmaMemory();

private:
    OHNativeWindowBuffer* nativeWindowBuffer_ = nullptr;
};

std::shared_ptr<Media::PixelMap> BlurPixelMapWithDma(
    const std::shared_ptr<Drawing::Image>& srcImage, const Drawing::ImageInfo& info,
    Drawing::GPUContext* gpuContext, RenderContext* renderContext, float blurRadius)
{
    if (gpuContext == nullptr || renderContext == nullptr) {
        RS_LOGE("BlurPixelMapWithDma: gpuContext or renderContext is nullptr");
        return nullptr;
    }
    auto colorType = info.GetColorType() == Drawing::ColorType::COLORTYPE_RGBA_F16 ?
        Drawing::ColorType::COLORTYPE_RGBA_F16 : Drawing::ColorType::COLORTYPE_RGBA_8888;

    Media::InitializationOptions outOpts;
    outOpts.size.width = info.GetWidth();
    outOpts.size.height = info.GetHeight();
    outOpts.pixelFormat = (colorType == Drawing::ColorType::COLORTYPE_RGBA_F16) ?
        Media::PixelFormat::RGBA_F16 : Media::PixelFormat::RGBA_8888;
    outOpts.alphaType = Media::AlphaType::IMAGE_ALPHA_TYPE_PREMUL;
    std::shared_ptr<Media::PixelMap> outPixelMap = Media::PixelMap::Create(outOpts);
    if (outPixelMap == nullptr) {
        RS_LOGE("BlurPixelMapWithDma: PixelMap::Create failed");
        return nullptr;
    }

    BlurPixelMapDmaMem dmaMem;
    sptr<SurfaceBuffer> surfaceBuffer = dmaMem.DmaMemAlloc(info, outPixelMap);
    if (surfaceBuffer == nullptr) {
        RS_LOGE("BlurPixelMapWithDma: DmaMemAlloc failed");
        return nullptr;
    }
    auto surface = dmaMem.GetSurfaceFromSurfaceBuffer(surfaceBuffer, gpuContext, renderContext, colorType);
    if (surface == nullptr) {
        RS_LOGE("BlurPixelMapWithDma: GetSurfaceFromSurfaceBuffer failed");
        return nullptr;
    }

    RSPaintFilterCanvas canvas(surface.get());
    canvas.DrawImage(*srcImage, 0, 0, Drawing::SamplingOptions());
    surface->FlushAndSubmit(true);
    ApplyBlurOnCanvas(canvas, surface, blurRadius);
    surface->FlushAndSubmit(true);
    RSUniRenderUtil::FlushDmaSurfaceBuffer(outPixelMap.get());
    return outPixelMap;
}

sptr<SurfaceBuffer> BlurPixelMapDmaMem::DmaMemAlloc(const Drawing::ImageInfo& dstInfo,
    const std::shared_ptr<Media::PixelMap>& pixelMap)
{
    if (pixelMap == nullptr) {
        RS_LOGE("BlurPixelMapDmaMem::DmaMemAlloc: pixelMap is nullptr");
        return nullptr;
    }
    sptr<SurfaceBuffer> surfaceBuffer = SurfaceBuffer::Create();
    if (!surfaceBuffer) {
        RS_LOGE("BlurPixelMapDmaMem::DmaMemAlloc: surfaceBuffer create failed");
        return nullptr;
    }
    auto colorGamut = pixelMap->InnerGetGrColorSpace().GetColorSpaceName() ==
        OHOS::ColorManager::ColorSpaceName::SRGB ?
        GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB : GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3;
    BufferRequestConfig requestConfig = {
        .width = dstInfo.GetWidth(),
        .height = dstInfo.GetHeight(),
        .strideAlignment = DEFAULT_STRIDE_ALIGNMENT,
        .format = dstInfo.GetColorType() == Drawing::ColorType::COLORTYPE_RGBA_F16 ?
            GRAPHIC_PIXEL_FMT_RGBA16_FLOAT : GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_HW_RENDER | BUFFER_USAGE_HW_TEXTURE |
            BUFFER_USAGE_MEM_DMA | BUFFER_USAGE_MEM_MMZ_CACHE,
        .timeout = 0,
        .colorGamut = colorGamut,
        .transform = GraphicTransformType::GRAPHIC_ROTATE_NONE,
    };
    GSError ret = surfaceBuffer->Alloc(requestConfig);
    if (ret != GSERROR_OK) {
        RS_LOGE("BlurPixelMapDmaMem::DmaMemAlloc: surfaceBuffer alloc failed, %{public}s",
            GSErrorStr(ret).c_str());
        return nullptr;
    }
    void* nativeBuffer = surfaceBuffer.GetRefPtr();
    OHOS::RefBase *ref = reinterpret_cast<OHOS::RefBase *>(nativeBuffer);
    ref->IncStrongRef(ref);
    int32_t bufferSize = pixelMap->GetByteCount();
    pixelMap->SetPixelsAddr(surfaceBuffer->GetVirAddr(), nativeBuffer, bufferSize,
        Media::AllocatorType::DMA_ALLOC, nullptr);
    return surfaceBuffer;
}

std::shared_ptr<Drawing::Surface> BlurPixelMapDmaMem::GetSurfaceFromSurfaceBuffer(
    sptr<SurfaceBuffer> surfaceBuffer, Drawing::GPUContext* gpuContext,
    RenderContext* renderContext, Drawing::ColorType colorType)
{
    if (surfaceBuffer == nullptr || gpuContext == nullptr || renderContext == nullptr) {
        RS_LOGE("BlurPixelMapDmaMem::GetSurfaceFromSurfaceBuffer: null input");
        return nullptr;
    }
    if (nativeWindowBuffer_ == nullptr) {
        nativeWindowBuffer_ = CreateNativeWindowBufferFromSurfaceBuffer(&surfaceBuffer);
        if (nativeWindowBuffer_ == nullptr) {
            RS_LOGE("BlurPixelMapDmaMem::GetSurfaceFromSurfaceBuffer: nativeWindowBuffer_ is null");
            return nullptr;
        }
    }
    auto vkInterface = RsVulkanContext::Get(renderContext->GetType()).GetRsVulkanInterface();
    if (vkInterface == nullptr) {
        RS_LOGE("BlurPixelMapDmaMem::GetSurfaceFromSurfaceBuffer: vkInterface is null");
        return nullptr;
    }
    auto backendTexture = NativeBufferUtils::MakeBackendTextureFromNativeBuffer(
        vkInterface, nativeWindowBuffer_, surfaceBuffer->GetWidth(), surfaceBuffer->GetHeight());
    if (!backendTexture.IsValid()) {
        RS_LOGE("BlurPixelMapDmaMem::GetSurfaceFromSurfaceBuffer: backendTexture invalid");
        return nullptr;
    }
    auto vkTextureInfo = backendTexture.GetTextureInfo().GetVKTextureInfo();
    if (vkTextureInfo == nullptr) {
        RS_LOGE("BlurPixelMapDmaMem::GetSurfaceFromSurfaceBuffer: vkTextureInfo is null");
        return nullptr;
    }
    vkTextureInfo->imageUsageFlags = vkTextureInfo->imageUsageFlags | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    auto cleanUpHelper = new NativeBufferUtils::VulkanCleanupHelper(
        vkInterface, vkTextureInfo->vkImage, vkTextureInfo->vkAlloc.memory);
    if (cleanUpHelper == nullptr) {
        RS_LOGE("BlurPixelMapDmaMem::GetSurfaceFromSurfaceBuffer: cleanUpHelper is null");
        return nullptr;
    }
    auto drawingSurface = Drawing::Surface::MakeFromBackendTexture(
        gpuContext,
        backendTexture.GetTextureInfo(),
        Drawing::TextureOrigin::TOP_LEFT,
        1, colorType, nullptr,
        NativeBufferUtils::DeleteVkImage, cleanUpHelper);
    if (drawingSurface == nullptr) {
        RS_LOGE("BlurPixelMapDmaMem::GetSurfaceFromSurfaceBuffer: MakeFromBackendTexture failed");
        delete cleanUpHelper;
        return nullptr;
    }
    return drawingSurface;
}

void BlurPixelMapDmaMem::ReleaseDmaMemory()
{
    if (nativeWindowBuffer_ != nullptr) {
        DestroyNativeWindowBuffer(nativeWindowBuffer_);
        nativeWindowBuffer_ = nullptr;
    }
}
#endif // RS_ENABLE_VK

bool BlurPixelMapOnGpuSurface(
    const std::shared_ptr<Drawing::Image>& image,
    const std::shared_ptr<Media::PixelMap>& pixelMap, float blurRadius,
    std::shared_ptr<Media::PixelMap>& blurredPixelMap,
    Drawing::GPUContext* gpuContext)
{
    RS_LOGI("BlurPixelMapOnGpuSurface: GL/non-DMA path, no zero-copy, w:%{public}d, h:%{public}d",
        pixelMap->GetWidth(), pixelMap->GetHeight());
    RS_TRACE_NAME("BlurPixelMap GL non-DMA path");
    auto surface = CreateGpuSurface(pixelMap->GetWidth(), pixelMap->GetHeight(), gpuContext);
    if (surface == nullptr) {
        RS_LOGE("BlurPixelMapOnGpuSurface: CreateGpuSurface failed");
        return false;
    }
    RSPaintFilterCanvas canvas(surface.get());
    canvas.DrawImage(*image, 0, 0, Drawing::SamplingOptions());
    surface->FlushAndSubmit(true);
    ApplyBlurOnCanvas(canvas, surface, blurRadius);
    surface->FlushAndSubmit(true);
    blurredPixelMap = ReadBackToPixelMap(surface, pixelMap->GetWidth(), pixelMap->GetHeight());
    RS_LOGI("BlurPixelMapOnGpuSurface end, result:%{public}d", blurredPixelMap != nullptr);
    return blurredPixelMap != nullptr;
}

bool BlurPixelMapImpl(
    const std::shared_ptr<Media::PixelMap>& pixelMap, float blurRadius,
    std::shared_ptr<Media::PixelMap>& blurredPixelMap,
    Drawing::GPUContext* gpuContext, RenderContext* renderContext)
{
    if (pixelMap == nullptr || gpuContext == nullptr) {
        RS_LOGE("BlurPixelMapImpl: invalid input, pixelMap or gpuContext is nullptr");
        return false;
    }
    RS_TRACE_NAME_FMT("BlurPixelMapImpl blurRadius:%f, w:%d, h:%d, allocatorType:%d",
        blurRadius, pixelMap->GetWidth(), pixelMap->GetHeight(),
        static_cast<int>(pixelMap->GetAllocatorType()));
    if (std::isnan(blurRadius) || std::isinf(blurRadius) || blurRadius < 0.f) {
        RS_LOGE("BlurPixelMapImpl: invalid blurRadius:%{public}f", blurRadius);
        return false;
    }
    if (blurRadius < MIN_VALID_BLUR_RADIUS) {
        RS_LOGW("BlurPixelMapImpl: blurRadius too small, skip blur");
        blurredPixelMap = pixelMap;
        return true;
    }
    if (!CheckPixelMapDimension(renderContext, pixelMap)) {
        RS_LOGE("BlurPixelMapImpl: dimension check failed");
        return false;
    }
    auto colorSpace = RSColorSpaceUtil::ColorSpaceToDrawingColorSpace(
        pixelMap->InnerGetGrColorSpace().GetColorSpaceName());
    auto colorType = pixelMap->GetPixelFormat() == Media::PixelFormat::RGBA_F16 ?
        Drawing::ColorType::COLORTYPE_RGBA_F16 : Drawing::ColorType::COLORTYPE_RGBA_8888;
    Drawing::ImageInfo info = Drawing::ImageInfo{pixelMap->GetWidth(), pixelMap->GetHeight(),
        colorType, Drawing::AlphaType::ALPHATYPE_PREMUL, colorSpace};
    auto image = CreateGpuImageFromPixelMap(pixelMap, gpuContext);
    if (image == nullptr) {
        RS_LOGE("BlurPixelMapImpl: CreateGpuImageFromPixelMap failed");
        return false;
    }
#ifdef RS_ENABLE_VK
    bool useDma = (pixelMap->GetAllocatorType() == Media::AllocatorType::DMA_ALLOC) &&
        (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
         RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR);
    if (useDma) {
        auto blurred = BlurPixelMapWithDma(image, info, gpuContext, renderContext, blurRadius);
        if (blurred != nullptr) {
            blurredPixelMap = blurred;
            RS_LOGI("BlurPixelMapImpl end (DMA), result:%{public}d", blurredPixelMap != nullptr);
            return blurredPixelMap != nullptr;
        }
        RS_LOGE("BlurPixelMapImpl: BlurPixelMapWithDma failed, fallback to GPU surface path");
        useDma = false;
    }
    if (!useDma) {
#endif
        return BlurPixelMapOnGpuSurface(image, pixelMap, blurRadius,
            blurredPixelMap, gpuContext);
#ifdef RS_ENABLE_VK
    }
#endif
    return false;
}

bool BlurPixelMapOnRenderThread(
    const std::shared_ptr<Media::PixelMap>& pixelMap, float blurRadius,
    std::shared_ptr<Media::PixelMap>& blurredPixelMap)
{
    if (pixelMap == nullptr) {
        RS_LOGE("BlurPixelMapOnRenderThread: pixelMap is nullptr");
        return false;
    }
    RS_TRACE_NAME_FMT("BlurPixelMapOnRenderThread blurRadius:%f, w:%d, h:%d",
        blurRadius, pixelMap->GetWidth(), pixelMap->GetHeight());
    RS_LOGI("BlurPixelMapOnRenderThread start, blurRadius:%{public}f, w:%{public}d, h:%{public}d, "
        "allocatorType:%{public}d, fd:%{public}p",
        blurRadius, pixelMap->GetWidth(), pixelMap->GetHeight(),
        static_cast<int>(pixelMap->GetAllocatorType()), pixelMap->GetFd());
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
    return BlurPixelMapImpl(pixelMap, blurRadius, blurredPixelMap, gpuContext, renderContext.get());
}
} // namespace Rosen
} // namespace OHOS
#endif // ROSEN_OHOS && (RS_ENABLE_GL || RS_ENABLE_VK)
