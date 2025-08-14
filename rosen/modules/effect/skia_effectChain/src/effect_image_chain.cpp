/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "effect_image_chain.h"

#include "effect_utils.h"
#include "ge_mesa_blur_shader_filter.h"
#include "rs_trace.h"

#include "pipeline/rs_paint_filter_canvas.h"
#include "platform/common/rs_system_properties.h"
#include "render/rs_hps_blur.h"
#include "render/rs_pixel_map_util.h"
#include "render_context/render_context.h"

#ifdef RS_ENABLE_VK
#include "effect_vulkan_context.h"
#endif

namespace OHOS::Rosen {
Drawing::ColorType ImageUtil::PixelFormatToDrawingColorType(const Media::PixelFormat& pixelFormat)
{
    switch (pixelFormat) {
        case Media::PixelFormat::RGB_565: {
            return Drawing::ColorType::COLORTYPE_RGB_565;
        }
        case Media::PixelFormat::RGBA_8888: {
            return Drawing::ColorType::COLORTYPE_RGBA_8888;
        }
        case Media::PixelFormat::BGRA_8888: {
            return Drawing::ColorType::COLORTYPE_BGRA_8888;
        }
        case Media::PixelFormat::ALPHA_8: {
            return Drawing::ColorType::COLORTYPE_ALPHA_8;
        }
        case Media::PixelFormat::RGBA_F16: {
            return Drawing::ColorType::COLORTYPE_RGBA_F16;
        }
        case Media::PixelFormat::RGBA_1010102: {
            return Drawing::ColorType::COLORTYPE_RGBA_1010102;
        }
        default: {
            return Drawing::ColorType::COLORTYPE_UNKNOWN;
        }
    }
}

Drawing::AlphaType ImageUtil::AlphaTypeToDrawingAlphaType(const Media::AlphaType& alphaType)
{
    switch (alphaType) {
        case Media::AlphaType::IMAGE_ALPHA_TYPE_OPAQUE: {
            return Drawing::AlphaType::ALPHATYPE_OPAQUE;
        }
        case Media::AlphaType::IMAGE_ALPHA_TYPE_PREMUL: {
            return Drawing::AlphaType::ALPHATYPE_PREMUL;
        }
        case Media::AlphaType::IMAGE_ALPHA_TYPE_UNPREMUL: {
            return Drawing::AlphaType::ALPHATYPE_UNPREMUL;
        }
        default: {
            return Drawing::AlphaType::ALPHATYPE_UNKNOWN;
        }
    }
}

DrawingError EffectImageChain::Prepare(const std::shared_ptr<Media::PixelMap>& srcPixelMap, bool forceCPU)
{
    std::lock_guard<std::mutex> lock(apiMutex_);
    if (!CheckPixelMap(srcPixelMap)) {
        return DrawingError::ERR_ILLEGAL_INPUT;
    }
    srcPixelMap_ = srcPixelMap;

    if (InitWithoutCanvas(srcPixelMap_) != DrawingError::ERR_OK) {
        EFFECT_LOG_E("EffectImageChain::Prepare: Failed to init.");
        return DrawingError::ERR_IMAGE_NULL;
    }

    forceCPU_ = forceCPU;
    surface_ = CreateSurface(forceCPU_);
    if (surface_ == nullptr) {
        EFFECT_LOG_E("EffectImageChain::Prepare: Failed to create surface %{public}d.", forceCPU_);
        return DrawingError::ERR_SURFACE;
    }
    canvas_ = surface_->GetCanvas();

    prepared_ = true;
    return DrawingError::ERR_OK;
}

DrawingError EffectImageChain::ApplyDrawingFilter(const std::shared_ptr<Drawing::ImageFilter>& filter)
{
    if (filter == nullptr) {
        EFFECT_LOG_E("EffectImageChain::ApplyDrawingFilter: The filter is null.");
        return DrawingError::ERR_ILLEGAL_INPUT;
    }

    std::lock_guard<std::mutex> lock(apiMutex_);
    filters_ = (filters_ == nullptr) ? filter : Drawing::ImageFilter::CreateComposeImageFilter(filter, filters_);
    return DrawingError::ERR_OK;
}

DrawingError EffectImageChain::ApplyBlur(float radius, const Drawing::TileMode& tileMode)
{
    if (radius < 0.0f) { // invalid radius
        return DrawingError::ERR_ILLEGAL_INPUT;
    }

    std::lock_guard<std::mutex> lock(apiMutex_);
    if (!prepared_) { // blur need prepare first
        EFFECT_LOG_E("EffectImageChain::ApplyBlur: Not ready, need prepare first.");
        return DrawingError::ERR_NOT_PREPARED;
    }

    if (forceCPU_) {
        auto filter = Drawing::ImageFilter::CreateBlurImageFilter(radius, radius, tileMode, nullptr);
        filters_ = (filters_ == nullptr) ? filter : Drawing::ImageFilter::CreateComposeImageFilter(filter, filters_);
        return DrawingError::ERR_OK;
    }

    if (filters_ != nullptr) {
        DrawOnFilter(); // need draw first to ensure cascading
        image_ = surface_->GetImageSnapshot();
        filters_ = nullptr; // clear filters_ to avoid apply again
    }

    auto isHpsApplied = (RSSystemProperties::GetHpsBlurEnabled() && tileMode == Drawing::TileMode::CLAMP &&
        (ApplyHpsBlur(radius) == DrawingError::ERR_OK));
    if (isHpsApplied) { // apply hps blur success
        return DrawingError::ERR_OK;
    }

    return ApplyMesaBlur(radius, tileMode);
}

DrawingError EffectImageChain::ApplyMesaBlur(float radius, const Drawing::TileMode& tileMode)
{
    ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, "EffectImageChain::ApplyMesaBlur");
    Drawing::GEMESABlurShaderFilterParams params { radius, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, static_cast<int>(tileMode),
        0.f, 0.f };
    auto mesaBlurFilter = std::make_shared<GEMESABlurShaderFilter>(params);
    image_ = mesaBlurFilter->ProcessImage(*canvas_, image_,
        Drawing::Rect(0, 0, srcPixelMap_->GetWidth(), srcPixelMap_->GetHeight()),
        Drawing::Rect(0, 0, srcPixelMap_->GetWidth(), srcPixelMap_->GetHeight()));
    ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
    return DrawingError::ERR_OK;
}

DrawingError EffectImageChain::ApplyHpsBlur(float radius)
{
    ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, "EffectImageChain::ApplyHpsBlur");
    auto ret = DrawingError::ERR_OK;
    do {
        if (std::ceil(radius) == 0) { // exception when applying hps blur on 0, need handle separately
            break;
        }

        RSPaintFilterCanvas canvas(surface_.get());
        Drawing::AutoCanvasRestore autoRestore(canvas, true);
        auto hpsParam = Drawing::HpsBlurParameter(Drawing::Rect(0, 0, image_->GetWidth(), image_->GetHeight()),
            Drawing::Rect(0, 0, image_->GetWidth(), image_->GetHeight()), radius, 1.f, 1.f);
        if (!HpsBlurFilter::GetHpsBlurFilter().ApplyHpsBlur(canvas, image_, hpsParam, 1.f)) {
            EFFECT_LOG_E("EffectImageChain::ApplyHpsBlur: Failed to apply hps blur");
            ret = DrawingError::ERR_FAST_BLUR;
            break;
        }
        image_ = surface_->GetImageSnapshot();
    } while (false);

    ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
    return ret;
}

void EffectImageChain::DrawOnFilter()
{
    ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, "EffectImageChain::DrawOnFilter");
    // Step1: Setup Paint
    Drawing::Paint paint;
    paint.SetStyle(Drawing::Paint::PaintStyle::PAINT_FILL);
    paint.SetAntiAlias(true);
    paint.SetBlendMode(Drawing::BlendMode::SRC);
    if (filters_ != nullptr) {
        Drawing::Filter filter;
        filter.SetImageFilter(filters_);
        paint.SetFilter(filter);
    }

    // Step2: Draw image
    canvas_->Save();
    canvas_->ResetMatrix();
    canvas_->AttachPaint(paint);
    canvas_->DrawImage(*image_, 0, 0, Drawing::SamplingOptions());
    canvas_->DetachPaint();
    canvas_->Restore();
    ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
}

DrawingError EffectImageChain::Draw()
{
    std::lock_guard<std::mutex> lock(apiMutex_);
    ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, "EffectImageChain::Draw");
    auto ret = DrawingError::ERR_OK;
    do {
        if (!prepared_) {
            EFFECT_LOG_E("EffectImageChain::Draw: Not ready, need prepare first.");
            ret = DrawingError::ERR_NOT_PREPARED;
            break;
        }

        DrawOnFilter();

        bool needRead = (!forceCPU_ && dstPixelMap_);
        if (needRead && !canvas_->ReadPixels(imageInfo_, reinterpret_cast<void*>(dstPixelMap_->GetWritablePixels()),
            static_cast<uint32_t>(dstPixelMap_->GetRowStride()), 0, 0)) {
            EFFECT_LOG_E("EffectImageChain::Draw: Failed to readPixels to target Pixmap.");
            ret = DrawingError::ERR_PIXEL_READ;
            break;
        }
    } while (false);

    ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
    return ret;
}

bool EffectImageChain::CheckPixelMap(const std::shared_ptr<Media::PixelMap>& pixelMap)
{
    if (pixelMap == nullptr) {
        EFFECT_LOG_E("EffectImageChain::CheckPixelMap: The srcPixelMap is nullptr.");
        return false;
    }

    if (pixelMap->GetWidth() <= 0 || pixelMap->GetHeight() <= 0) {
        EFFECT_LOG_E("EffectImageChain::CheckPixelMap: The width or height is invalid.");
        return false;
    }

    if (pixelMap->GetPixelFormat() == Media::PixelFormat::UNKNOWN) {
        EFFECT_LOG_E("EffectImageChain::CheckPixelMap: The pixel format is invalid.");
        return false;
    }

    if (pixelMap->GetAlphaType() == Media::AlphaType::IMAGE_ALPHA_TYPE_UNKNOWN) {
        EFFECT_LOG_E("EffectImageChain::CheckPixelMap: The alpha type is invalid.");
        return false;
    }

    return true;
}

DrawingError EffectImageChain::InitWithoutCanvas(const std::shared_ptr<Media::PixelMap>& srcPixelMap)
{
    imageInfo_ = Drawing::ImageInfo { srcPixelMap_->GetWidth(), srcPixelMap_->GetHeight(),
        ImageUtil::PixelFormatToDrawingColorType(srcPixelMap_->GetPixelFormat()),
        ImageUtil::AlphaTypeToDrawingAlphaType(srcPixelMap_->GetAlphaType()),
        RSPixelMapUtil::GetPixelmapColorSpace(srcPixelMap_) };

    Drawing::Bitmap bitmap;
    bitmap.InstallPixels(imageInfo_, reinterpret_cast<void*>(srcPixelMap_->GetWritablePixels()),
        static_cast<uint32_t>(srcPixelMap_->GetRowStride()));
    image_ = std::make_shared<Drawing::Image>();
    image_->BuildFromBitmap(bitmap);

    Media::InitializationOptions opts;
    opts.size = { srcPixelMap_->GetWidth(), srcPixelMap_->GetHeight() };
    opts.pixelFormat = srcPixelMap_->GetPixelFormat();
    opts.alphaType = srcPixelMap_->GetAlphaType();
    opts.editable = true;
    opts.useDMA = true;
    auto dstPixelMap = Media::PixelMap::Create(opts);
    if (dstPixelMap == nullptr) {
        image_ = nullptr;
        EFFECT_LOG_E("EffectImageChain::InitWithoutCanvas: Failed to create the dstPixelMap.");
        return DrawingError::ERR_IMAGE_NULL;
    }
    dstPixelMap_ = std::shared_ptr<Media::PixelMap>(dstPixelMap.release());

    EFFECT_LOG_I("Init w %{public}d, h %{public}d, f %{public}d, alpha %{public}d, useDMA 1",
        opts.size.width, opts.size.height, srcPixelMap_->GetPixelFormat(), srcPixelMap_->GetAlphaType());
    return DrawingError::ERR_OK;
}

std::shared_ptr<Media::PixelMap> EffectImageChain::GetPixelMap()
{
    std::lock_guard<std::mutex> lock(apiMutex_);
    return dstPixelMap_;
}

std::shared_ptr<Drawing::Surface> EffectImageChain::CreateSurface(bool forceCPU)
{
    if (forceCPU) { // dstPixelMap_ must not null by called
        auto address = const_cast<uint32_t*>(dstPixelMap_->GetPixel32(0, 0));
        return Drawing::Surface::MakeRasterDirect(imageInfo_, address, dstPixelMap_->GetRowStride());
    }

#ifdef RS_ENABLE_GPU
    std::shared_ptr<Drawing::GPUContext> context = nullptr;
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::OPENGL) {
        auto renderContext = std::make_shared<RenderContext>();
        renderContext->InitializeEglContext();
        renderContext->SetUpGpuContext();
        context = renderContext->GetSharedDrGPUContext();
    }

#ifdef RS_ENABLE_VK
    if (RSSystemProperties::IsUseVulkan()) {
        context = EffectVulkanContext::GetSingleton().CreateDrawingContext();
    }
#endif

    if (context == nullptr) {
        EFFECT_LOG_E("EffectImageChain::CreateGPUSurface: create gpuContext failed.");
        return nullptr;
    }
    return Drawing::Surface::MakeRenderTarget(context.get(), false, imageInfo_);
#else
    EFFECT_LOG_W("EffectImageChain::CreateGPUSurface: GPU rendering is not supported.");
    return nullptr;
#endif
}
} // namespace OHOS::Rosen