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
#include "ge_external_dynamic_loader.h"
#include "ge_mask_transition_shader_filter.h"
#include "ge_water_droplet_transition_filter.h"
#include "ge_linear_gradient_shader_mask.h"
#include "ge_mesa_blur_shader_filter.h"
#include "ge_radial_gradient_shader_mask.h"
#include "ge_variable_radius_blur_shader_filter.h"
#include "ge_sdf_from_image_filter.h"
#include "rs_trace.h"

#include "pipeline/rs_paint_filter_canvas.h"
#include "platform/common/rs_system_properties.h"
#include "render/rs_hps_blur.h"
#include "render/rs_pixel_map_util.h"
#include "surface_buffer.h"

#ifdef RS_ENABLE_VK
#include "effect_vulkan_context.h"
#include "platform/ohos/backend/native_buffer_utils.h"
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

EffectCanvas::EffectCanvas(Drawing::Surface* surface)
    : surface_(surface), canvas_(surface ? surface->GetCanvas().get() : nullptr) {}

Drawing::Surface* EffectCanvas::GetSurface() const
{
    return surface_;
}

std::array<int, 2> EffectCanvas::CalcHpsBluredImageDimension(const Drawing::HpsBlurParameter& blurParams)
{
    if (canvas_ != nullptr) {
        return canvas_->CalcHpsBluredImageDimension(blurParams);
    }
    return {0, 0};
}

Drawing::CoreCanvas& EffectCanvas::AttachBrush(const Drawing::Brush& brush)
{
    if (canvas_ != nullptr) {
        return canvas_->AttachBrush(brush);
    }
    return *this;
}

void EffectCanvas::DrawRect(const Drawing::Rect& rect)
{
    if (canvas_ != nullptr) {
        return canvas_->DrawRect(rect);
    }
}

Drawing::CoreCanvas& EffectCanvas::DetachBrush()
{
    if (canvas_ != nullptr) {
        return canvas_->DetachBrush();
    }
    return *this;
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

#ifdef RS_ENABLE_VK
DrawingError EffectImageChain::PrepareNativeBuffer(const std::shared_ptr<Media::PixelMap>& srcPixelMap,
    std::shared_ptr<OH_NativeBuffer>& dstNativeBuffer, bool forceCPU)
{
    std::lock_guard<std::mutex> lock(apiMutex_);
    // CPU not supported
    forceCPU_ = forceCPU;
    if (forceCPU_) {
        EFFECT_LOG_E("EffectImageChain::ApplyMaskTransitionFilter: Not support CPU.");
        return DrawingError::ERR_ILLEGAL_INPUT;
    }
    if (!CheckPixelMap(srcPixelMap)) {
        return DrawingError::ERR_ILLEGAL_INPUT;
    }
    srcPixelMap_ = srcPixelMap;
 
    imageInfo_ = Drawing::ImageInfo{srcPixelMap_->GetWidth(),
        srcPixelMap_->GetHeight(),
        ImageUtil::PixelFormatToDrawingColorType(srcPixelMap_->GetPixelFormat()),
        ImageUtil::AlphaTypeToDrawingAlphaType(srcPixelMap_->GetAlphaType()),
        RSPixelMapUtil::GetPixelmapColorSpace(srcPixelMap_)};
 
    Drawing::Bitmap bitmap;
    bitmap.InstallPixels(imageInfo_,
        reinterpret_cast<void *>(srcPixelMap_->GetWritablePixels()),
        static_cast<uint32_t>(srcPixelMap_->GetRowStride()));
    image_ = std::make_shared<Drawing::Image>();
    image_->BuildFromBitmap(bitmap);
 
    if (RSSystemProperties::IsUseVulkan()) {
        gpuContext_ = RsVulkanContext::GetSingleton().CreateDrawingContext();
    }
    if (gpuContext_ == nullptr) {
        EFFECT_COMM_LOG_E("EffectImageChain::CreateGPUSurface: create gpuContext failed.");
        return DrawingError::ERR_ILLEGAL_INPUT;
    }
 
    surface_ = NativeBufferUtils::CreateSurfaceFromNativeBuffer(
        RsVulkanContext::GetSingleton(), imageInfo_, dstNativeBuffer.get(), imageInfo_.GetColorSpace());
    if (surface_ == nullptr) {
        EFFECT_LOG_E("EffectImageChain::Prepare: Failed to create surface %{public}d.", forceCPU_);
        return DrawingError::ERR_SURFACE;
    }
 
    canvas_ = surface_->GetCanvas();
    prepared_ = true;
    return DrawingError::ERR_OK;
}
#else
DrawingError EffectImageChain::PrepareNativeBuffer(const std::shared_ptr<Media::PixelMap>& srcPixelMap,
    std::shared_ptr<OH_NativeBuffer>& dstNativeBuffer, bool forceCPU)
{
    EFFECT_COMM_LOG_E("EffectImageChain::PrepareDstNative: Failed. Requires Vulkan backend");
    return DrawingError::ERR_ILLEGAL_INPUT;
}
#endif

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

DrawingError EffectImageChain::ApplyBlur(float radius, const Drawing::TileMode& tileMode,
    bool isDirection, float angle)
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
        (ApplyHpsBlur(radius) == DrawingError::ERR_OK) && !isDirection);
    if (isHpsApplied) { // apply hps blur success
        return DrawingError::ERR_OK;
    }

    return ApplyMesaBlur(radius, tileMode, isDirection, angle);
}

DrawingError EffectImageChain::ApplyEllipticalGradientBlur(float blurRadius, float centerX, float centerY,
    float maskRadiusX, float maskRadiusY, const std::vector<float> &positions, const std::vector<float> &degrees)
{
    if (blurRadius < 0.0f) { // invalid radius
        return DrawingError::ERR_ILLEGAL_INPUT;
    }
    if (maskRadiusX <= 0.0f || maskRadiusY <= 0.0f) {
        return DrawingError::ERR_ILLEGAL_INPUT;
    }

    if (positions.empty()) {
        return DrawingError::ERR_ILLEGAL_INPUT;
    }
    if (degrees.empty()) {
        return DrawingError::ERR_ILLEGAL_INPUT;
    }

    // CPU not supported
    if (forceCPU_) {
        return DrawingError::ERR_ILLEGAL_INPUT;
    }

    std::lock_guard<std::mutex> lock(apiMutex_);
    if (!prepared_) { // blur need prepare first
        EFFECT_LOG_E("EffectImageChain::ApplyEllipticalGradientBlur: Not ready, need prepare first.");
        return DrawingError::ERR_NOT_PREPARED;
    }
    if (filters_ != nullptr) {
        DrawOnFilter(); // need draw first to ensure cascading
        image_ = surface_->GetImageSnapshot();
        filters_ = nullptr; // clear filters_ to avoid apply again
    }

    ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, "EffectImageChain::ApplyEllipticalGradientBlur");
    Drawing::GERadialGradientShaderMaskParams maskParams{
        {centerX, centerY}, maskRadiusX, maskRadiusY, degrees, positions};
    auto radialGradientShaderMask = std::make_shared<Drawing::GERadialGradientShaderMask>(maskParams);

    Drawing::GEVariableRadiusBlurShaderFilterParams filterParams{radialGradientShaderMask, blurRadius, true};
    auto variableRadiusBlurFilter = std::make_shared<GEVariableRadiusBlurShaderFilter>(filterParams);
    image_ = variableRadiusBlurFilter->ProcessImage(*canvas_,
        image_,
        Drawing::Rect(0, 0, srcPixelMap_->GetWidth(), srcPixelMap_->GetHeight()),
        Drawing::Rect(0, 0, srcPixelMap_->GetWidth(), srcPixelMap_->GetHeight()));
    ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
    return DrawingError::ERR_OK;
}

static std::shared_ptr<GEShaderFilter> GenerateGEXShaderFilter(uint32_t type, uint32_t len, void* param)
{
    if (!param) {
        return nullptr;
    }

    auto object = GEExternalDynamicLoader::GetInstance().CreateGEXObjectByType(type, len, param);
    if (!object) {
        return nullptr;
    }
 
    std::shared_ptr<GEShaderFilter> filterShaderr(static_cast<GEShaderFilter*>(object));
    return filterShaderr;
}

DrawingError EffectImageChain::ApplyMapColorByBrightness(
    const std::vector<Vector4f>& colors, const std::vector<float>& positions)
{
    std::lock_guard<std::mutex> lock(apiMutex_);
    if (!prepared_) {
        EFFECT_LOG_E("EffectImageChain::ApplyMapColorByBrightness: Not ready, need prepare first.");
        return DrawingError::ERR_NOT_PREPARED;
    }

    // CPU not supported
    if (forceCPU_) {
        EFFECT_LOG_E("EffectImageChain::ApplyMapColorByBrightness: Not support CPU.");
        return DrawingError::ERR_ILLEGAL_INPUT;
    }

    if (filters_ != nullptr) {
        DrawOnFilter(); // need draw first to ensure cascading
        image_ = surface_->GetImageSnapshot();
        filters_ = nullptr; // clear filters_ to avoid apply again
    }

    ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, "EffectImageChain::ApplyMapColorByBrightness");
    Drawing::GEMapColorByBrightnessFilterParams params = {colors, positions};
    auto filterShader = GenerateGEXShaderFilter(
        static_cast<uint32_t>(Drawing::GEFilterType::MAP_COLOR_BY_BRIGHTNESS),
        sizeof(Drawing::GEMapColorByBrightnessFilterParams),
        static_cast<void*>(&params));
    if (!filterShader) {
        EFFECT_LOG_E("EffectImageChain::ApplyMapColorByBrightness: Generate Filter failed.");
        ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
        return DrawingError::ERR_MEMORY;
    }

    auto width = srcPixelMap_->GetWidth();
    auto height = srcPixelMap_->GetHeight();
    image_ = filterShader->ProcessImage(*canvas_, image_,
        Drawing::Rect(0, 0, width, height), Drawing::Rect(0, 0, width, height));
    ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
    return DrawingError::ERR_OK;
}

DrawingError EffectImageChain::ApplyGammaCorrection(float gamma)
{
    std::lock_guard<std::mutex> lock(apiMutex_);
    if (!prepared_) {
        EFFECT_LOG_E("EffectImageChain::ApplyGammaCorrection: Not ready, need prepare first.");
        return DrawingError::ERR_NOT_PREPARED;
    }

    // CPU not supported
    if (forceCPU_) {
        EFFECT_LOG_E("EffectImageChain::ApplyGammaCorrection: Not support CPU.");
        return DrawingError::ERR_ILLEGAL_INPUT;
    }

    if (filters_ != nullptr) {
        DrawOnFilter(); // need draw first to ensure cascading
        image_ = surface_->GetImageSnapshot();
        filters_ = nullptr; // clear filters_ to avoid apply again
    }

    ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, "EffectImageChain::ApplyGammaCorrection");
    float gammaValue = gamma;
    auto gammaCorrectionShader = GenerateGEXShaderFilter(
        static_cast<uint32_t>(Drawing::GEFilterType::GAMMA_CORRECTION),
        sizeof(float), static_cast<void*>(&gammaValue));
    if (!gammaCorrectionShader) {
        EFFECT_LOG_E("EffectImageChain::ApplyGammaCorrection: Generate Filter failed.");
        ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
        return DrawingError::ERR_MEMORY;
    }

    auto width = srcPixelMap_->GetWidth();
    auto height = srcPixelMap_->GetHeight();
    image_ = gammaCorrectionShader->ProcessImage(*canvas_, image_,
        Drawing::Rect(0, 0, width, height), Drawing::Rect(0, 0, width, height));
    ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
    return DrawingError::ERR_OK;
}

DrawingError EffectImageChain::ApplyMesaBlur(float radius, const Drawing::TileMode& tileMode,
    bool isDirection, float angle)
{
    ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, "EffectImageChain::ApplyMesaBlur");
    Drawing::GEMESABlurShaderFilterParams params { radius, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, static_cast<int>(tileMode),
        0.f, 0.f, isDirection, angle};
    auto mesaBlurFilter = std::make_shared<GEMESABlurShaderFilter>(params);
    auto width = srcPixelMap_->GetWidth();
    auto height = srcPixelMap_->GetHeight();
    image_ = mesaBlurFilter->ProcessImage(*canvas_, image_,
        Drawing::Rect(0, 0, width, height), Drawing::Rect(0, 0, width, height));
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
        EffectCanvas canvas(surface_.get());
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

DrawingError EffectImageChain::ApplySDFCreation(int spreadFactor, bool generateDerivs)
{
    ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, "EffectImageChain::ApplySDFCreation");

    if (!prepared_) {
        EFFECT_LOG_E("EffectImageChain::ApplySDFCreation: Not ready, need prepare first.");
        return DrawingError::ERR_NOT_PREPARED;
    }

    // CPU not supported
    if (forceCPU_) {
        return DrawingError::ERR_ILLEGAL_INPUT;
    }

    std::lock_guard<std::mutex> lock(apiMutex_);

    if (filters_ != nullptr) {
        DrawOnFilter();  // need draw first to ensure cascading
        image_ = surface_->GetImageSnapshot();
        filters_ = nullptr;  // clear filters_ to avoid apply again
    }

    Drawing::GESDFFromImageFilterParams params{spreadFactor, generateDerivs};
    auto sdfFromImageFilter = std::make_shared<GESDFFromImageFilter>(params);
    image_ = sdfFromImageFilter->ProcessImage(*canvas_, image_,
        Drawing::Rect(0, 0, srcPixelMap_->GetWidth(), srcPixelMap_->GetHeight()),
        Drawing::Rect(0, 0, srcPixelMap_->GetWidth(), srcPixelMap_->GetHeight()));

    ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
    return DrawingError::ERR_OK;
}

std::shared_ptr<Drawing::Image> EffectImageChain::ConvertPixelMapToDrawingImage(
    const std::shared_ptr<Media::PixelMap>& pixelMap)
{
    if (!CheckPixelMap(pixelMap)) {
        return nullptr;
    }
 
    auto imageInfo = Drawing::ImageInfo {
        pixelMap->GetWidth(), pixelMap->GetHeight(),
        ImageUtil::PixelFormatToDrawingColorType(pixelMap->GetPixelFormat()),
        ImageUtil::AlphaTypeToDrawingAlphaType(pixelMap->GetAlphaType()),
        RSPixelMapUtil::GetPixelmapColorSpace(pixelMap)};
 
    Drawing::Bitmap bitmap;
    bitmap.InstallPixels(imageInfo,
        reinterpret_cast<void *>(pixelMap->GetWritablePixels()),
        static_cast<uint32_t>(pixelMap->GetRowStride()));
    auto image = std::make_shared<Drawing::Image>();
    image->BuildFromBitmap(bitmap);
 
    return image;
}
 
DrawingError EffectImageChain::ApplyMaskTransitionFilter(const std::shared_ptr<Media::PixelMap>& topLayerMap,
    const std::shared_ptr<Drawing::GEShaderMask>& mask, float factor, bool inverse)
{
    std::lock_guard<std::mutex> lock(apiMutex_);
    if (!prepared_) {
        EFFECT_LOG_E("EffectImageChain::ApplyMaskTransitionFilter: Not ready, need prepare first.");
        return DrawingError::ERR_NOT_PREPARED;
    }
 
    // CPU not supported
    if (forceCPU_) {
        EFFECT_LOG_E("EffectImageChain::ApplyMaskTransitionFilter: Not support CPU.");
        return DrawingError::ERR_ILLEGAL_INPUT;
    }
 
    if (filters_ != nullptr) {
        DrawOnFilter(); // need draw first to ensure cascading
        image_ = surface_->GetImageSnapshot();
        filters_ = nullptr; // clear filters_ to avoid apply again
    }
 
    Drawing::GEMaskTransitionShaderFilterParams filterParams{mask, factor, inverse};
    auto maskTransitionFilter = std::make_shared<GEMaskTransitionShaderFilter>(filterParams);
    
    Drawing::CanvasInfo canvasInfo;
    canvasInfo.geoWidth = srcPixelMap_->GetWidth();
    canvasInfo.geoHeight = srcPixelMap_->GetHeight();

    auto topLayer = ConvertPixelMapToDrawingImage(topLayerMap);
    if (!topLayer || canvasInfo.geoHeight <= 0 || canvasInfo.geoWidth <= 0) {
        EFFECT_LOG_E("EffectImageChain::ApplyMaskTransitionFilter: input image is null or invalid.");
        return DrawingError::ERR_ILLEGAL_INPUT;
    }
 
    ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, "EffectImageChain::ApplyMaskTransitionFilter");
    auto topLayerImageInfo = topLayer->GetImageInfo();
    float topLayerImageWidth = topLayerImageInfo.GetWidth();
    float topLayerImageHeight = topLayerImageInfo.GetHeight();
    auto scaleWRatio = topLayerImageWidth / canvasInfo.geoWidth;
    auto scaleHRatio = topLayerImageHeight / canvasInfo.geoHeight;
    Drawing::Matrix topLayerMatrix = canvasInfo.mat;
    topLayerMatrix.PostTranslate(-canvasInfo.tranX, -canvasInfo.tranY);
    topLayerMatrix.PostScale(scaleWRatio, scaleHRatio);
    Drawing::Matrix invertTopLayerMatrix;
    if (!topLayerMatrix.Invert(invertTopLayerMatrix)) {
        ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
        return DrawingError::ERR_ILLEGAL_INPUT;
    }
    maskTransitionFilter->SetShaderFilterCanvasinfo(canvasInfo);
    maskTransitionFilter->SetCache(topLayer, invertTopLayerMatrix);
 
    image_ = maskTransitionFilter->ProcessImage(*canvas_, image_,
        Drawing::Rect(0, 0, srcPixelMap_->GetWidth(), srcPixelMap_->GetHeight()),
        Drawing::Rect(0, 0, srcPixelMap_->GetWidth(), srcPixelMap_->GetHeight()));
    ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
 
    return DrawingError::ERR_OK;
}
 
std::shared_ptr<GEShaderFilter> GenerateWaterDropletTransitionFilter(
    const std::shared_ptr<Drawing::GEWaterDropletTransitionFilterParams>& params)
{
    if (!params) {
        return nullptr;
    }
    auto object = GEExternalDynamicLoader::GetInstance().CreateGEXObjectByType(
        static_cast<uint32_t>(Drawing::GEFilterType::WATER_DROPLET_TRANSITION),
        sizeof(Drawing::GEWaterDropletTransitionFilterParams),
        static_cast<void*>(params.get()));
    if (!object) {
        return std::make_shared<GEWaterDropletTransitionFilter>(*params);
    }
 
    std::shared_ptr<GEShaderFilter> dmShader(static_cast<GEShaderFilter*>(object));
    return dmShader;
}
 
DrawingError EffectImageChain::ApplyWaterDropletTransitionFilter(const std::shared_ptr<Media::PixelMap>& topLayerMap,
    const std::shared_ptr<Drawing::GEWaterDropletTransitionFilterParams>& geWaterDropletParams)
{
    std::lock_guard<std::mutex> lock(apiMutex_);
    if (!prepared_) {
        EFFECT_LOG_E("EffectImageChain::ApplyWaterDropletTransitionFilter: Not ready, need prepare first.");
        return DrawingError::ERR_NOT_PREPARED;
    }
 
    // CPU not supported
    if (forceCPU_) {
        EFFECT_LOG_E("EffectImageChain::ApplyWaterDropletTransitionFilter: Not support CPU.");
        return DrawingError::ERR_ILLEGAL_INPUT;
    }
 
    if (filters_ != nullptr) {
        DrawOnFilter(); // need draw first to ensure cascading
        image_ = surface_->GetImageSnapshot();
        filters_ = nullptr; // clear filters_ to avoid apply again
    }
 
    if (!geWaterDropletParams) {
        EFFECT_LOG_E("EffectImageChain::ApplyWaterDropletTransitionFilter: geWaterDropletParams is null.");
        return DrawingError::ERR_ILLEGAL_INPUT;
    }
    geWaterDropletParams->topLayer = ConvertPixelMapToDrawingImage(topLayerMap);
    if (!geWaterDropletParams->topLayer) {
        EFFECT_LOG_E("EffectImageChain::ApplyWaterDropletTransitionFilter: ConvertPixelMapToDrawingImage null.");
        return DrawingError::ERR_ILLEGAL_INPUT;
    }
    auto waterDropletTransitionFilter = GenerateWaterDropletTransitionFilter(geWaterDropletParams);
    if (!waterDropletTransitionFilter) {
        EFFECT_LOG_E("EffectImageChain::ApplyWaterDropletTransitionFilter, Generate Filter failed.");
        return DrawingError::ERR_ILLEGAL_INPUT;
    }

    ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, "EffectImageChain::ApplyWaterDropletTransitionFilter");
    Drawing::CanvasInfo canvasInfo;
    canvasInfo.geoWidth = srcPixelMap_->GetWidth();
    canvasInfo.geoHeight = srcPixelMap_->GetHeight();
    waterDropletTransitionFilter->SetShaderFilterCanvasinfo(canvasInfo);
 
    image_ = waterDropletTransitionFilter->ProcessImage(*canvas_, image_,
        Drawing::Rect(0, 0, srcPixelMap_->GetWidth(), srcPixelMap_->GetHeight()),
        Drawing::Rect(0, 0, srcPixelMap_->GetWidth(), srcPixelMap_->GetHeight()));
    ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
 
    return DrawingError::ERR_OK;
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

DrawingError EffectImageChain::DrawNativeBuffer()
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
        gpuContext_->FlushAndSubmit();
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
    opts.allocatorType = Media::AllocatorType::DMA_ALLOC;
    auto dstPixelMap = Media::PixelMap::Create(opts);
    if (dstPixelMap == nullptr) {
        image_ = nullptr;
        EFFECT_COMM_LOG_E("EffectImageChain::InitWithoutCanvas: Failed to create the dstPixelMap.");
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
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::OPENGL) {
        renderContext_ = RenderContext::Create();
        renderContext_->Init();
        renderContext_->SetUpGpuContext();
        gpuContext_ = renderContext_->GetSharedDrGPUContext();
    }

#ifdef RS_ENABLE_VK
    if (RSSystemProperties::IsUseVulkan()) {
        gpuContext_ = EffectVulkanContext::GetSingleton().CreateDrawingContext();
    }
#endif

    if (gpuContext_ == nullptr) {
        EFFECT_COMM_LOG_E("EffectImageChain::CreateGPUSurface: create gpuContext failed.");
        return nullptr;
    }
    return Drawing::Surface::MakeRenderTarget(gpuContext_.get(), false, imageInfo_);
#else
    EFFECT_LOG_W("EffectImageChain::CreateGPUSurface: GPU rendering is not supported.");
    return nullptr;
#endif
}

void EffectImageChain::Release()
{
    std::lock_guard<std::mutex> lock(apiMutex_);
    ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, "EffectImageChain::Release");

    surface_ = nullptr;
    canvas_ = nullptr;

    if (gpuContext_) {
        gpuContext_->ReleaseResourcesAndAbandonContext();
        gpuContext_ = nullptr;
    }
    renderContext_ = nullptr;
    prepared_ = false;
    ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
}

EffectImageChain::~EffectImageChain()
{
    surface_ = nullptr;
    canvas_ = nullptr;
    filters_ = nullptr;
    image_ = nullptr;
    if (gpuContext_) {
        gpuContext_->ReleaseResourcesAndAbandonContext();
        gpuContext_ = nullptr;
    }
    renderContext_ = nullptr;
}

static std::shared_ptr<GEShaderFilter> GenerateExtShaderWaterGlass(
    const std::shared_ptr<Drawing::GEWaterGlassDataParams>& params)
{
    auto object = GEExternalDynamicLoader::GetInstance().CreateGEXObjectByType(
        static_cast<uint32_t>(Drawing::GEFilterType::WATER_GLASS), sizeof(Drawing::GEWaterGlassDataParams),
        static_cast<void*>(params.get()));
    if (!object) {
        return nullptr;
    }

    std::shared_ptr<GEShaderFilter> dmShader(static_cast<GEShaderFilter*>(object));
    return dmShader;
}

static std::shared_ptr<GEShaderFilter> GenerateExtShaderReededGlass(
    const std::shared_ptr<Drawing::GEReededGlassDataParams>& params)
{
    auto object = GEExternalDynamicLoader::GetInstance().CreateGEXObjectByType(
        static_cast<uint32_t>(Drawing::GEFilterType::REEDED_GLASS), sizeof(Drawing::GEReededGlassDataParams),
        static_cast<void*>(params.get()));
    if (!object) {
        return nullptr;
    }

    std::shared_ptr<GEShaderFilter> dmShader(static_cast<GEShaderFilter*>(object));
    return dmShader;
}

DrawingError EffectImageChain::ApplyWaterGlass(const std::shared_ptr<Drawing::GEWaterGlassDataParams>& waterGlassDate)
{
    std::lock_guard<std::mutex> lock(apiMutex_);
    if (!prepared_) { // blur need prepare first
        EFFECT_LOG_E("EffectImageChain::ApplyWaterGlass: Not ready, need prepare first.");
        return DrawingError::ERR_NOT_PREPARED;
    }

    // CPU not supported
    if (forceCPU_) {
        EFFECT_LOG_E("EffectImageChain::ApplyWaterGlass: Not support CPU.");
        return DrawingError::ERR_ILLEGAL_INPUT;
    }

    if (filters_ != nullptr) {
        DrawOnFilter(); // need draw first to ensure cascading
        image_ = surface_->GetImageSnapshot();
        filters_ = nullptr; // clear filters_ to avoid apply again
    }

    ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, "EffectImageChain::ApplyWaterGlass");

    std::shared_ptr<GEShaderFilter> dmShader = GenerateExtShaderWaterGlass(waterGlassDate);
    if (dmShader == nullptr) {
        EFFECT_LOG_E("EffectImageChain::ApplyWaterGlass: GenerateExtShaderWaterGlass fail.");
        ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
        return DrawingError::ERR_ILLEGAL_INPUT;
    }

    image_ = dmShader->ProcessImage(*canvas_, image_,
        Drawing::Rect(0, 0, srcPixelMap_->GetWidth(), srcPixelMap_->GetHeight()),
        Drawing::Rect(0, 0, srcPixelMap_->GetWidth(), srcPixelMap_->GetHeight()));

    ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
    return DrawingError::ERR_OK;
}

DrawingError EffectImageChain::ApplyReededGlass(
    const std::shared_ptr<Drawing::GEReededGlassDataParams>& reededGlassDate)
{
    std::lock_guard<std::mutex> lock(apiMutex_);
    if (!prepared_) { // blur need prepare first
        EFFECT_LOG_E("EffectImageChain::ApplyReededGlass: Not ready, need prepare first.");
        return DrawingError::ERR_NOT_PREPARED;
    }

    // CPU not supported
    if (forceCPU_) {
        EFFECT_LOG_E("EffectImageChain::ApplyReededGlass: Not support CPU.");
        return DrawingError::ERR_ILLEGAL_INPUT;
    }

    if (filters_ != nullptr) {
        DrawOnFilter(); // need draw first to ensure cascading
        image_ = surface_->GetImageSnapshot();
        filters_ = nullptr; // clear filters_ to avoid apply again
    }

    ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, "EffectImageChain::ApplyReededGlass");

    std::shared_ptr<GEShaderFilter> dmShader = GenerateExtShaderReededGlass(reededGlassDate);
    if (!dmShader) {
        EFFECT_LOG_E("EffectImageChain::ApplyReededGlass: GenerateExtShaderReededGlass fail.");
        ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
        return DrawingError::ERR_ILLEGAL_INPUT;
    }

    image_ = dmShader->ProcessImage(*canvas_, image_,
        Drawing::Rect(0, 0, srcPixelMap_->GetWidth(), srcPixelMap_->GetHeight()),
        Drawing::Rect(0, 0, srcPixelMap_->GetWidth(), srcPixelMap_->GetHeight()));

    ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
    return DrawingError::ERR_OK;
}
} // namespace OHOS::Rosen