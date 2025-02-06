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

#include "rs_base_render_engine.h"
#include <memory>

#ifdef RS_ENABLE_EGLIMAGE
#include "src/gpu/gl/GrGLDefines.h"
#endif

#include "rs_divided_render_util.h"
#include "common/rs_optional_trace.h"
#include "memory/rs_tag_tracker.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#if defined(NEW_RENDER_CONTEXT)
#include "render_context_factory.h"
#include "rs_surface_factory.h"
#include "ohos/rs_render_surface_ohos.h"
#else
#include "platform/ohos/backend/rs_surface_ohos_gl.h"
#include "platform/ohos/backend/rs_surface_ohos_raster.h"
#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_vulkan_context.h"
#include "platform/ohos/backend/rs_surface_ohos_vulkan.h"
#endif
#endif
#include "render/rs_drawing_filter.h"
#include "render/rs_skia_filter.h"
#include "metadata_helper.h"

#include "drawable/rs_display_render_node_drawable.h"

namespace OHOS {
namespace Rosen {
RSBaseRenderEngine::RSBaseRenderEngine()
{
}

RSBaseRenderEngine::~RSBaseRenderEngine() noexcept
{
}

void RSBaseRenderEngine::Init(bool independentContext)
{
    (void)independentContext;
#if defined(NEW_RENDER_CONTEXT)
    RenderType renderType = RenderType::RASTER;
#if (defined RS_ENABLE_GL) || (defined RS_ENABLE_VK)
    if (OHOS::Rosen::RSSystemProperties::GetGpuApiType() == OHOS::Rosen::GpuApiType::OPENGL) {
        renderType = RenderType::GLES;
    } else if (RSSystemProperties::IsUseVulkan()) {
        renderType = RenderType::VULKAN;
    }
#endif
    renderContext_ = RenderContextBaseFactory::CreateRenderContext(renderType);
    renderContext_->Init();
    drawingContext_ = std::make_shared<Rosen::DrawingContext>(renderContext_->GetRenderType());
    drawingContext_->SetUpDrawingContext();
#else
#if (defined RS_ENABLE_GL) || (defined RS_ENABLE_VK)
    renderContext_ = std::make_shared<RenderContext>();
#ifdef RS_ENABLE_GL
    if (!RSSystemProperties::IsUseVulkan()) {
        renderContext_->InitializeEglContext();
    }
#endif
    if (RSUniRenderJudgement::IsUniRender()) {
        RS_LOGI("RSRenderEngine::RSRenderEngine set new cacheDir");
        renderContext_->SetUniRenderMode(true);
    }
#if defined(RS_ENABLE_VK)
    if (RSSystemProperties::IsUseVulkan()) {
        skContext_ = RsVulkanContext::GetSingleton().CreateDrawingContext(independentContext);
        vkImageManager_ = std::make_shared<RSVkImageManager>();
        renderContext_->SetUpGpuContext(skContext_);
    } else {
        renderContext_->SetUpGpuContext();
    }
#else
    renderContext_->SetUpGpuContext();
#endif
#endif // RS_ENABLE_GL || RS_ENABLE_VK
#endif
#if defined(RS_ENABLE_EGLIMAGE)
#if defined(NEW_RENDER_CONTEXT)
    std::shared_ptr<RSRenderSurfaceFrame> frame = renderContext_->GetRSRenderSurfaceFrame();
    eglImageManager_ = std::make_shared<RSEglImageManager>(frame->eglState->eglDisplay);
#else
    eglImageManager_ = std::make_shared<RSEglImageManager>(renderContext_->GetEGLDisplay());
#endif
#endif // RS_ENABLE_EGLIMAGE
#ifdef RS_ENABLE_VK
    if (RSSystemProperties::IsUseVulkan()) {
        skContext_ = RsVulkanContext::GetSingleton().CreateDrawingContext();
        vkImageManager_ = std::make_shared<RSVkImageManager>();
    }
#endif
#ifdef USE_VIDEO_PROCESSING_ENGINE
    colorSpaceConverterDisplay_ = Media::VideoProcessingEngine::ColorSpaceConverterDisplay::Create();
#endif
}

void RSBaseRenderEngine::InitCapture(bool independentContext)
{
    (void)independentContext;
    if (captureRenderContext_) {
        return;
    }

#if (defined RS_ENABLE_GL) || (defined RS_ENABLE_VK)
    captureRenderContext_ = std::make_shared<RenderContext>();
#ifdef RS_ENABLE_GL
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::OPENGL) {
        captureRenderContext_->InitializeEglContext();
    }
#endif
    if (RSUniRenderJudgement::IsUniRender()) {
        captureRenderContext_->SetUniRenderMode(true);
    }
#if defined(RS_ENABLE_VK)
    if (RSSystemProperties::IsUseVulkan()) {
        captureSkContext_ = RsVulkanContext::GetSingleton().CreateDrawingContext(independentContext);
        captureRenderContext_->SetUpGpuContext(captureSkContext_);
    } else {
        captureRenderContext_->SetUpGpuContext();
    }
#else
    captureRenderContext_->SetUpGpuContext();
#endif
#endif // RS_ENABLE_GL || RS_ENABLE_VK
}

void RSBaseRenderEngine::ResetCurrentContext()
{
    if (renderContext_ == nullptr) {
        RS_LOGE("This render context is nullptr.");
        return;
    }
#if (defined RS_ENABLE_GL)
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::OPENGL) {
        renderContext_->ShareMakeCurrentNoSurface(EGL_NO_CONTEXT);
        if (captureRenderContext_) {
            captureRenderContext_->ShareMakeCurrentNoSurface(EGL_NO_CONTEXT);
        }
    }
#endif

#if defined(RS_ENABLE_VK) // end RS_ENABLE_GL and enter RS_ENABLE_VK
    if (RSSystemProperties::IsUseVulkan()) {
        renderContext_->AbandonContext();
        if (captureRenderContext_) {
            captureRenderContext_->AbandonContext();
        }
    }
#endif // end RS_ENABLE_GL and RS_ENABLE_VK
}

bool RSBaseRenderEngine::NeedForceCPU(const std::vector<LayerInfoPtr>& layers)
{
    bool forceCPU = false;
    for (const auto& layer: layers) {
        if (layer == nullptr) {
            continue;
        }

        auto buffer = layer->GetBuffer();
        if (buffer == nullptr) {
            continue;
        }

#ifndef RS_ENABLE_EGLIMAGE
        const auto bufferFormat = buffer->GetFormat();
        if (bufferFormat == GRAPHIC_PIXEL_FMT_YCRCB_420_SP || bufferFormat == GRAPHIC_PIXEL_FMT_YCBCR_420_SP) {
            forceCPU = true;
            break;
        }
#endif
    }

    return forceCPU;
}

std::shared_ptr<Drawing::Image> RSBaseRenderEngine::CreateEglImageFromBuffer(RSPaintFilterCanvas& canvas,
    const sptr<SurfaceBuffer>& buffer, const sptr<SyncFence>& acquireFence, const uint32_t threadIndex,
    const std::shared_ptr<Drawing::ColorSpace>& drawingColorSpace)
{
#ifdef RS_ENABLE_EGLIMAGE
#if defined(RS_ENABLE_GL)
    if (!RSSystemProperties::IsUseVulkan() && canvas.GetGPUContext() == nullptr) {
        RS_LOGE("RSBaseRenderEngine::CreateEglImageFromBuffer GrContext is null!");
        return nullptr;
    }
#endif // RS_ENABLE_GL
#if defined(RS_ENABLE_VK)
    if (RSSystemProperties::IsUseVulkan() && renderContext_->GetDrGPUContext() == nullptr) {
        RS_LOGE("RSBaseRenderEngine::CreateEglImageFromBuffer GrContext is null!");
        return nullptr;
    }
#endif // RS_ENABLE_VK
    auto eglTextureId = eglImageManager_->MapEglImageFromSurfaceBuffer(buffer, acquireFence, threadIndex);
    if (eglTextureId == 0) {
        RS_LOGE("RSBaseRenderEngine::CreateEglImageFromBuffer MapEglImageFromSurfaceBuffer return invalid texture ID");
        return nullptr;
    }
    auto pixelFmt = buffer->GetFormat();
    auto bitmapFormat = RSBaseRenderUtil::GenerateDrawingBitmapFormat(buffer);

    auto image = std::make_shared<Drawing::Image>();
    Drawing::TextureInfo externalTextureInfo;
    externalTextureInfo.SetWidth(buffer->GetSurfaceBufferWidth());
    externalTextureInfo.SetHeight(buffer->GetSurfaceBufferHeight());

#ifndef ROSEN_EMULATOR
    auto surfaceOrigin = Drawing::TextureOrigin::TOP_LEFT;
#else
    auto surfaceOrigin = Drawing::TextureOrigin::BOTTOM_LEFT;
#endif

#if defined(RS_ENABLE_GL)
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::OPENGL) {
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
            RS_LOGE("RSBaseRenderEngine::CreateEglImageFromBuffer image BuildFromTexture failed");
            return nullptr;
        }
    }
#endif

#if defined(RS_ENABLE_VK)
    if (RSSystemProperties::IsUseVulkan() &&
        !image->BuildFromTexture(*renderContext_->GetDrGPUContext(), externalTextureInfo,
        surfaceOrigin, bitmapFormat, drawingColorSpace)) {
        RS_LOGE("RSBaseRenderEngine::CreateEglImageFromBuffer image BuildFromTexture failed");
        return nullptr;
    }
#endif
    return image;
#else
    return nullptr;
#endif // RS_ENABLE_EGLIMAGE
}

#ifdef NEW_RENDER_CONTEXT
std::unique_ptr<RSRenderFrame> RSBaseRenderEngine::RequestFrame(
    const std::shared_ptr<RSRenderSurfaceOhos>& rsSurface,
    const BufferRequestConfig& config, bool forceCPU, bool useAFBC,
    const FrameContextConfig& frameContextConfig)
#else
std::unique_ptr<RSRenderFrame> RSBaseRenderEngine::RequestFrame(
    const std::shared_ptr<RSSurfaceOhos>& rsSurface,
    const BufferRequestConfig& config, bool forceCPU, bool useAFBC,
    const FrameContextConfig& frameContextConfig)
#endif
{
#ifdef RS_ENABLE_VK
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        skContext_ = RsVulkanContext::GetSingleton().CreateDrawingContext(
            frameContextConfig.independentContext);
        if (renderContext_ == nullptr) {
            return nullptr;
        }
        renderContext_->SetUpGpuContext(skContext_);
    }
#endif
    if (rsSurface == nullptr) {
        RS_LOGE("RSBaseRenderEngine::RequestFrame: surface is null!");
        return nullptr;
    }
    RS_OPTIONAL_TRACE_BEGIN("RSBaseRenderEngine::RequestFrame(RSSurface)");
#ifdef RS_ENABLE_VK
    RSTagTracker tagTracker(skContext_.get(), RSTagTracker::TAGTYPE::TAG_ACQUIRE_SURFACE);
#endif
    rsSurface->SetColorSpace(config.colorGamut);
    rsSurface->SetSurfacePixelFormat(config.format);
    if (frameContextConfig.isVirtual) {
        RS_LOGD("RSBaseRenderEngine::RequestFrame: Mirror Screen Set Timeout to 0.");
        rsSurface->SetTimeOut(frameContextConfig.timeOut);
    }
    auto bufferUsage = config.usage;
#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)) && (defined RS_ENABLE_EGLIMAGE)
    if (forceCPU) {
        bufferUsage |= BUFFER_USAGE_CPU_WRITE;
    }
#else
    bufferUsage |= BUFFER_USAGE_CPU_WRITE;
#endif
    if (frameContextConfig.isProtected) {
        bufferUsage |= BUFFER_USAGE_PROTECTED;
    }
    rsSurface->SetSurfaceBufferUsage(bufferUsage);

    // check if we can use GPU context
#if defined(NEW_RENDER_CONTEXT)
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    if (forceCPU) {
        std::shared_ptr<RenderContextBase> renderContextRaster = RenderContextBaseFactory::CreateRenderContext(
            RenderType::RASTER);
        renderContextRaster->Init();
        std::shared_ptr<DrawingContext> drawingContextRaster = std::make_shared<DrawingContext>(RenderType::RASTER);
        rsSurface->SetRenderContext(renderContextRaster);
        rsSurface->SetDrawingContext(drawingContextRaster);
        RS_LOGD("RSBaseRenderEngine::RequestFrame force CPU");
    } else {
        if (renderContext_ != nullptr) {
            rsSurface->SetRenderContext(renderContext_);
        }
        if (drawingContext_ != nullptr) {
            rsSurface->SetDrawingContext(drawingContext_);
        }
    }
#endif
#else
#ifdef RS_ENABLE_GL
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::OPENGL &&
        renderContext_ != nullptr) {
        rsSurface->SetRenderContext(renderContext_.get());
    }
#endif
#ifdef RS_ENABLE_VK
    if (RSSystemProperties::IsUseVulkan() && skContext_ != nullptr) {
        std::static_pointer_cast<RSSurfaceOhosVulkan>(rsSurface)->SetSkContext(skContext_);
    }
#endif
#endif
    auto surfaceFrame = rsSurface->RequestFrame(config.width, config.height, 0, useAFBC,
        frameContextConfig.isProtected);
    RS_OPTIONAL_TRACE_END();
    if (surfaceFrame == nullptr) {
        RS_LOGE("RSBaseRenderEngine::RequestFrame: request SurfaceFrame failed!");
        return nullptr;
    }
#ifdef NEW_RENDER_CONTEXT
    return std::make_unique<RSRenderFrame>(rsSurface);
#else
    return std::make_unique<RSRenderFrame>(rsSurface, std::move(surfaceFrame));
#endif
}

std::unique_ptr<RSRenderFrame> RSBaseRenderEngine::RequestFrame(const sptr<Surface>& targetSurface,
    const BufferRequestConfig& config, bool forceCPU, bool useAFBC,
    const FrameContextConfig& frameContextConfig)
{
    RS_OPTIONAL_TRACE_BEGIN("RSBaseRenderEngine::RequestFrame(targetSurface)");
    if (targetSurface == nullptr) {
        RS_LOGE("RSBaseRenderEngine::RequestFrame: surface is null!");
        RS_OPTIONAL_TRACE_END();
        return nullptr;
    }

#if defined(NEW_RENDER_CONTEXT)
    std::shared_ptr<RSRenderSurfaceOhos> rsSurface = nullptr;
    std::shared_ptr<RSRenderSurface> renderSurface = RSSurfaceFactory::CreateRSSurface(PlatformName::OHOS,
        targetSurface);
    rsSurface = std::static_pointer_cast<RSRenderSurfaceOhos>(renderSurface);
#else
    std::shared_ptr<RSSurfaceOhos> rsSurface = nullptr;
#if (defined RS_ENABLE_GL) && (defined RS_ENABLE_EGLIMAGE)
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::OPENGL) {
        if (forceCPU) {
            rsSurface = std::make_shared<RSSurfaceOhosRaster>(targetSurface);
        } else {
            rsSurface = std::make_shared<RSSurfaceOhosGl>(targetSurface);
        }
    }
#endif
#if (defined RS_ENABLE_VK)
    if (RSSystemProperties::IsUseVulkan()) {
        rsSurface = std::make_shared<RSSurfaceOhosVulkan>(targetSurface);
    }
#endif
    if (rsSurface == nullptr) {
        rsSurface = std::make_shared<RSSurfaceOhosRaster>(targetSurface);
    }
#endif
    RS_OPTIONAL_TRACE_END();
    return RequestFrame(rsSurface, config, forceCPU, useAFBC, frameContextConfig);
}

#ifdef NEW_RENDER_CONTEXT
std::shared_ptr<RSRenderSurfaceOhos> RSBaseRenderEngine::MakeRSSurface(const sptr<Surface>& targetSurface,
    bool forceCPU)
#else
std::shared_ptr<RSSurfaceOhos> RSBaseRenderEngine::MakeRSSurface(const sptr<Surface>& targetSurface, bool forceCPU)
#endif
{
    RS_TRACE_FUNC();
    if (targetSurface == nullptr) {
        RS_LOGE("RSBaseRenderEngine::MakeRSSurface: surface is null!");
        RS_OPTIONAL_TRACE_END();
        return nullptr;
    }

#if defined(NEW_RENDER_CONTEXT)
    std::shared_ptr<RSRenderSurfaceOhos> rsSurface = nullptr;
    std::shared_ptr<RSRenderSurface> renderSurface = RSSurfaceFactory::CreateRSSurface(PlatformName::OHOS,
        targetSurface);
    rsSurface = std::static_pointer_cast<RSRenderSurfaceOhos>(renderSurface);
#else
    std::shared_ptr<RSSurfaceOhos> rsSurface = nullptr;
#if (defined RS_ENABLE_GL) && (defined RS_ENABLE_EGLIMAGE)
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::OPENGL) {
        if (forceCPU) {
            rsSurface = std::make_shared<RSSurfaceOhosRaster>(targetSurface);
        } else {
            rsSurface = std::make_shared<RSSurfaceOhosGl>(targetSurface);
        }
    }
#endif
#if (defined RS_ENABLE_VK)
    if (RSSystemProperties::IsUseVulkan()) {
        rsSurface = std::make_shared<RSSurfaceOhosVulkan>(targetSurface);
    }
#endif
    if (rsSurface == nullptr) {
        rsSurface = std::make_shared<RSSurfaceOhosRaster>(targetSurface);
    }
#endif
    return rsSurface;
}

#ifdef NEW_RENDER_CONTEXT
void RSBaseRenderEngine::SetUiTimeStamp(const std::unique_ptr<RSRenderFrame>& renderFrame,
    std::shared_ptr<RSRenderSurfaceOhos> surfaceOhos)
#else
void RSBaseRenderEngine::SetUiTimeStamp(const std::unique_ptr<RSRenderFrame>& renderFrame,
    std::shared_ptr<RSSurfaceOhos> surfaceOhos)
#endif
{
    if (surfaceOhos == nullptr) {
        RS_LOGE("RSBaseRenderEngine::SetUiTimeStamp: surfaceOhos is null!");
        return;
    }

    if (renderFrame == nullptr) {
        RS_LOGE("RSBaseRenderEngine::SetUiTimeStamp: renderFrame is null!.");
        return;
    }

#ifdef NEW_RENDER_CONTEXT
    surfaceOhos->SetUiTimeStamp();
#else
    auto& frame = renderFrame->GetFrame();
    surfaceOhos->SetUiTimeStamp(frame);
#endif
}

void RSBaseRenderEngine::DrawDisplayNodeWithParams(RSPaintFilterCanvas& canvas, RSDisplayRenderNode& node,
    BufferDrawParam& params)
{
    if (params.useCPU) {
        DrawBuffer(canvas, params);
    } else {
        auto drawable = node.GetRenderDrawable();
        if (!drawable) {
            return;
        }
        auto displayDrawable = std::static_pointer_cast<DrawableV2::RSDisplayRenderNodeDrawable>(drawable);
        RegisterDeleteBufferListener(displayDrawable->GetRSSurfaceHandlerOnDraw()->GetConsumer());
        DrawImage(canvas, params);
    }
}

void RSBaseRenderEngine::DrawDisplayNodeWithParams(RSPaintFilterCanvas& canvas, RSSurfaceHandler& surfaceHandler,
    BufferDrawParam& drawParam)
{
    if (drawParam.useCPU) {
        DrawBuffer(canvas, drawParam);
    } else {
        RegisterDeleteBufferListener(surfaceHandler.GetConsumer());
        DrawImage(canvas, drawParam);
    }
}

void RSBaseRenderEngine::SetColorFilterMode(ColorFilterMode mode)
{
    std::lock_guard<std::mutex> lock(colorFilterMutex_);
    uint32_t uMode = static_cast<uint32_t>(mode);
    uint32_t uInvertMode = static_cast<uint32_t>(ColorFilterMode::INVERT_COLOR_ENABLE_MODE);
    uint32_t ucolorFilterMode = static_cast<uint32_t>(colorFilterMode_);
    bool isInvertModeEnabled = ((ucolorFilterMode & uInvertMode) == uInvertMode);

    switch (mode) {
        case ColorFilterMode::INVERT_COLOR_DISABLE_MODE: {
            if (colorFilterMode_ != ColorFilterMode::COLOR_FILTER_END) {
                colorFilterMode_ = static_cast<ColorFilterMode>(ucolorFilterMode & ~uInvertMode);
            }
            break;
        }
        case ColorFilterMode::INVERT_COLOR_ENABLE_MODE: {
            if (colorFilterMode_ != ColorFilterMode::COLOR_FILTER_END) {
                colorFilterMode_ = static_cast<ColorFilterMode>(ucolorFilterMode | uInvertMode);
            } else {
                colorFilterMode_ = mode;
            }
            break;
        }
        case ColorFilterMode::DALTONIZATION_PROTANOMALY_MODE:
        case ColorFilterMode::DALTONIZATION_DEUTERANOMALY_MODE:
        case ColorFilterMode::DALTONIZATION_TRITANOMALY_MODE: {
            if (isInvertModeEnabled) {
                colorFilterMode_ = static_cast<ColorFilterMode>(uMode | uInvertMode);
            } else {
                colorFilterMode_ = mode;
            }
            break;
        }
        case ColorFilterMode::DALTONIZATION_NORMAL_MODE: {
            if (isInvertModeEnabled) {
                colorFilterMode_ = ColorFilterMode::INVERT_COLOR_ENABLE_MODE;
            } else {
                colorFilterMode_ = ColorFilterMode::COLOR_FILTER_END;
            }
            break;
        }

        // INVERT_DALTONIZATION_PROTANOMALY_MODE, INVERT_DALTONIZATION_DEUTERANOMALY_MODE
        // INVERT_DALTONIZATION_TRITANOMALY_MODE and COLOR_FILTER_END can not be set directly
        case ColorFilterMode::INVERT_DALTONIZATION_PROTANOMALY_MODE: // fall-through
        case ColorFilterMode::INVERT_DALTONIZATION_DEUTERANOMALY_MODE: // fall-through
        case ColorFilterMode::INVERT_DALTONIZATION_TRITANOMALY_MODE: // fall-through
        case ColorFilterMode::COLOR_FILTER_END: // fall-through
        default: {
            colorFilterMode_ = ColorFilterMode::COLOR_FILTER_END;
            break;
        }
    }
}

ColorFilterMode RSBaseRenderEngine::GetColorFilterMode()
{
    std::lock_guard<std::mutex> lock(colorFilterMutex_);
    return colorFilterMode_;
}

void RSBaseRenderEngine::SetHighContrast(bool enabled)
{
    isHighContrastEnabled_  = enabled;
}

bool RSBaseRenderEngine::IsHighContrastEnabled()
{
    return isHighContrastEnabled_;
}

void RSBaseRenderEngine::DrawBuffer(RSPaintFilterCanvas& canvas, BufferDrawParam& params)
{
    RS_TRACE_NAME("RSBaseRenderEngine::DrawBuffer(CPU)");
    Drawing::Bitmap bitmap;
    std::vector<uint8_t> newBuffer;
    if (!RSBaseRenderUtil::ConvertBufferToBitmap(params.buffer, newBuffer, params.targetColorGamut, bitmap,
        params.metaDatas)) {
        RS_LOGE("RSDividedRenderUtil::DrawBuffer: create bitmap failed.");
        return;
    }
    Drawing::Image drImage;
    drImage.BuildFromBitmap(bitmap);
    canvas.DrawImageRect(drImage, params.srcRect, params.dstRect, Drawing::SamplingOptions(),
        Drawing::SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT);
}

#ifdef USE_VIDEO_PROCESSING_ENGINE
bool RSBaseRenderEngine::ConvertDrawingColorSpaceToSpaceInfo(const std::shared_ptr<Drawing::ColorSpace>& colorSpace,
    HDI::Display::Graphic::Common::V1_0::CM_ColorSpaceInfo& colorSpaceInfo)
{
    using namespace HDI::Display::Graphic::Common::V1_0;
    CM_ColorSpaceType colorSpaceType = CM_COLORSPACE_NONE;
    if (colorSpace == nullptr || colorSpace->Equals(nullptr) || colorSpace->IsSRGB()) {
        colorSpaceType = CM_SRGB_FULL;
    } else if (colorSpace->Equals(Drawing::ColorSpace::CreateRGB(
        Drawing::CMSTransferFuncType::SRGB, Drawing::CMSMatrixType::DCIP3))) {
        colorSpaceType = CM_P3_FULL;
    } else if (colorSpace->Equals(Drawing::ColorSpace::CreateRGB(
        Drawing::CMSTransferFuncType::SRGB, Drawing::CMSMatrixType::ADOBE_RGB))) {
        colorSpaceType = CM_ADOBERGB_FULL;
    } else if (colorSpace->Equals(Drawing::ColorSpace::CreateRGB(
        Drawing::CMSTransferFuncType::SRGB, Drawing::CMSMatrixType::REC2020))) {
        colorSpaceType = CM_DISPLAY_BT2020_SRGB;
    } else {
        RS_LOGD("RSBaseRenderEngine::ConvertDrawingColorSpaceToSpaceInfo color space not supported");
        return false;
    }

    GSError ret = MetadataHelper::ConvertColorSpaceTypeToInfo(colorSpaceType, colorSpaceInfo);
    if (ret != GSERROR_OK) {
        RS_LOGE("RSBaseRenderEngine::ConvertDrawingColorSpaceToSpaceInfo ConvertColorSpaceTypeToInfo failed with \
            %{public}u.", ret);
        return false;
    }

    return true;
}

std::shared_ptr<Drawing::ColorSpace> RSBaseRenderEngine::GetCanvasColorSpace(const RSPaintFilterCanvas& canvas)
{
    auto surface = canvas.GetSurface();
    if (surface == nullptr) {
        return nullptr;
    }

    return surface->GetImageInfo().GetColorSpace();
}

bool RSBaseRenderEngine::SetColorSpaceConverterDisplayParameter(
    const BufferDrawParam& params, Media::VideoProcessingEngine::ColorSpaceConverterDisplayParameter& parameter)
{
    using namespace HDI::Display::Graphic::Common::V1_0;

    CM_HDR_Metadata_Type hdrMetadataType = CM_METADATA_NONE;
    GSError ret = MetadataHelper::GetHDRMetadataType(params.buffer, hdrMetadataType);
    if (ret != GSERROR_OK) {
        RS_LOGD("RSBaseRenderEngine::ColorSpaceConvertor GetHDRMetadataType failed with %{public}u.", ret);
    }

    parameter.inputColorSpace.metadataType = hdrMetadataType;
    parameter.outputColorSpace.metadataType = hdrMetadataType;

    ret = MetadataHelper::GetHDRStaticMetadata(params.buffer, parameter.staticMetadata);
    if (ret != GSERROR_OK) {
        RS_LOGD("RSBaseRenderEngine::ColorSpaceConvertor GetHDRStaticMetadata failed with %{public}u.", ret);
    }
    ret = MetadataHelper::GetHDRDynamicMetadata(params.buffer, parameter.dynamicMetadata);
    if (ret != GSERROR_OK) {
        RS_LOGD("RSBaseRenderEngine::ColorSpaceConvertor GetHDRDynamicMetadata failed with %{public}u.", ret);
    }

    parameter.width = params.buffer->GetWidth();
    parameter.height = params.buffer->GetHeight();
    parameter.tmoNits = params.tmoNits;
    parameter.currentDisplayNits = params.displayNits;
    parameter.sdrNits = params.sdrNits;

    RS_LOGD("RSBaseRenderEngine::ColorSpaceConvertor parameter inputColorSpace.colorSpaceInfo.primaries = %{public}u, \
            inputColorSpace.metadataType = %{public}u, outputColorSpace.colorSpaceInfo.primaries = %{public}u, \
            outputColorSpace.metadataType = %{public}u, tmoNits = %{public}f, currentDisplayNits = %{public}f, \
            sdrNits = %{public}f",
            parameter.inputColorSpace.colorSpaceInfo.primaries, parameter.inputColorSpace.metadataType,
            parameter.outputColorSpace.colorSpaceInfo.primaries, parameter.outputColorSpace.metadataType,
            parameter.tmoNits, parameter.currentDisplayNits, paramter.sdrNits);

    return true;
}

void RSBaseRenderEngine::ColorSpaceConvertor(std::shared_ptr<Drawing::ShaderEffect> &inputShader,
    BufferDrawParam& params, Media::VideoProcessingEngine::ColorSpaceConverterDisplayParameter& parameter)
{
    RS_OPTIONAL_TRACE_BEGIN("RSBaseRenderEngine::ColorSpaceConvertor");
    
    if (!SetColorSpaceConverterDisplayParameter(params, parameter)) {
        RS_OPTIONAL_TRACE_END();
        return;
    }

    if(!inputShader) {
        RS_LOGE("RSBaseRenderEngine::ColorSpaceConvertor inputShader is null");
        RS_OPTIONAL_TRACE_END();
        return;
    }
    if (params.isHdrRedraw || RSUniRenderThread::GetCaptureParam().IsSnapshot_ ||
        RSUniRenderThread::GetCaptureParam().isMirror_) {
        parameter.disableHeadRoom = true;
    }

    std::shared_ptr<Drawing::ShaderEffect> outputShader;
    auto convRet = colorSpaceConverterDisplay_->Process(inputShader, outputShader, parameter);
    if (convRet != Media::VideoProcessingEngine::VPE_ALGO_ERR_OK) {
        RS_LOGE("RSBaseRenderEngine::ColorSpaceConvertor colorSpaceConverterDisplay failed with %{public}u.", convRet);
        RS_OPTIONAL_TRACE_END();
        return;
    }
    if (outputShader == nullptr) {
        RS_LOGE("RSBaseRenderEngine::ColorSpaceConvertor outputShader is nullptr.");
        RS_OPTIONAL_TRACE_END();
        return;
    }
    params.paint.SetShaderEffect(outputShader);
    RS_OPTIONAL_TRACE_END();
}
#endif

std::shared_ptr<Drawing::ColorSpace> RSBaseRenderEngine::ConvertColorGamutToDrawingColorSpace(
    GraphicColorGamut colorGamut)
{
    std::shared_ptr<Drawing::ColorSpace>  colorSpace = nullptr;
    switch (colorGamut) {
        case GRAPHIC_COLOR_GAMUT_DISPLAY_P3:
            colorSpace = Drawing::ColorSpace::CreateRGB(
                Drawing::CMSTransferFuncType::SRGB, Drawing::CMSMatrixType::DCIP3);
            break;
        case GRAPHIC_COLOR_GAMUT_ADOBE_RGB:
            colorSpace = Drawing::ColorSpace::CreateRGB(
                Drawing::CMSTransferFuncType::SRGB, Drawing::CMSMatrixType::ADOBE_RGB);
            break;
        case GRAPHIC_COLOR_GAMUT_BT2020:
            colorSpace = Drawing::ColorSpace::CreateRGB(
                Drawing::CMSTransferFuncType::SRGB, Drawing::CMSMatrixType::REC2020);
            break;
        default:
            break;
    }

    return colorSpace;
}

std::shared_ptr<Drawing::Image> RSBaseRenderEngine::CreateImageFromBuffer(RSPaintFilterCanvas& canvas,
    BufferDrawParam& params, VideoInfo& videoInfo)
{
    auto image = std::make_shared<Drawing::Image>();
    if (!RSBaseRenderUtil::IsBufferValid(params.buffer)) {
        RS_LOGE("RSBaseRenderEngine::CreateImageFromBuffer invalid buffer!");
        return nullptr;
    }
    videoInfo.drawingColorSpace_ = Drawing::ColorSpace::CreateSRGB();

#ifdef USE_VIDEO_PROCESSING_ENGINE
    videoInfo.parameter_ = {};
    videoInfo.retGetColorSpaceInfo_ = MetadataHelper::GetColorSpaceInfo(params.buffer,
        videoInfo.parameter_.inputColorSpace.colorSpaceInfo);
    if (videoInfo.retGetColorSpaceInfo_ == GSERROR_OK) {
        videoInfo.drawingColorSpace_ = GetCanvasColorSpace(canvas);
    }
#endif

#ifdef RS_ENABLE_VK
    if (RSSystemProperties::IsUseVulkan()) {
        auto imageCache = vkImageManager_->MapVkImageFromSurfaceBuffer(params.buffer,
            params.acquireFence, params.threadIndex);
        auto bitmapFormat = RSBaseRenderUtil::GenerateDrawingBitmapFormat(params.buffer);
#ifndef ROSEN_EMULATOR
        auto surfaceOrigin = Drawing::TextureOrigin::TOP_LEFT;
#else
        auto surfaceOrigin = Drawing::TextureOrigin::BOTTOM_LEFT;
#endif
        auto contextDrawingVk = canvas.GetGPUContext();
        if (contextDrawingVk == nullptr || image == nullptr || imageCache == nullptr) {
            RS_LOGE("contextDrawingVk or image or imageCache is nullptr.");
            return nullptr;
        }
        auto& backendTexture = imageCache->GetBackendTexture();
        if (!image->BuildFromTexture(*contextDrawingVk, backendTexture.GetTextureInfo(),
            surfaceOrigin, bitmapFormat, videoInfo.drawingColorSpace_,
            NativeBufferUtils::DeleteVkImage, imageCache->RefCleanupHelper())) {
            ROSEN_LOGE("RSBaseRenderEngine::CreateImageFromBuffer: backendTexture is not valid!!!");
            return nullptr;
        }
    }
#endif // RS_ENABLE_VK

#ifdef RS_ENABLE_GL // RS_ENABLE_GL
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::OPENGL) {
        image = CreateEglImageFromBuffer(canvas, params.buffer, params.acquireFence, params.threadIndex,
            videoInfo.drawingColorSpace_);
        if (image == nullptr) {
            RS_LOGE("RSBaseRenderEngine::CreateImageFromBuffer: image is nullptr!");
            return nullptr;
        }
    }
#endif // RS_ENABLE_GL
    return image;
}

void RSBaseRenderEngine::DrawImage(RSPaintFilterCanvas& canvas, BufferDrawParam& params)
{
    RS_TRACE_NAME_FMT("RSBaseRenderEngine::DrawImage(GPU) targetColorGamut=%d", params.targetColorGamut);
    VideoInfo videoInfo;
    auto image = CreateImageFromBuffer(canvas, params, videoInfo);
    if (image == nullptr) {
        return;
    }
    Drawing::SamplingOptions samplingOptions;
    if (!RSSystemProperties::GetUniRenderEnabled()) {
        samplingOptions = Drawing::SamplingOptions();
    } else {
        if (params.isMirror) {
            samplingOptions = Drawing::SamplingOptions(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NEAREST);
        } else {
            samplingOptions = !params.useBilinearInterpolation
                                ? Drawing::SamplingOptions()
                                : Drawing::SamplingOptions(Drawing::FilterMode::LINEAR,
                                    Drawing::MipmapMode::LINEAR);
        }
    }

    if (params.targetColorGamut == GRAPHIC_COLOR_GAMUT_SRGB) {
        canvas.AttachBrush(params.paint);
        canvas.DrawImageRect(*image, params.srcRect, params.dstRect, samplingOptions,
            Drawing::SrcRectConstraint::FAST_SRC_RECT_CONSTRAINT);
        canvas.DetachBrush();
    } else {
#ifdef USE_VIDEO_PROCESSING_ENGINE

    // For sdr brightness ratio
    if (ROSEN_LNE(params.brightnessRatio, DEFAULT_BRIGHTNESS_RATIO) && !params.isHdrRedraw) {
        Drawing::Filter filter = params.paint.GetFilter();
        Drawing::ColorMatrix luminanceMatrix;
        luminanceMatrix.SetScale(params.brightnessRatio, params.brightnessRatio, params.brightnessRatio, 1.0f);
        auto luminanceColorFilter =
            std::make_shared<Drawing::ColorFilter>(Drawing::ColorFilter::FilterType::MATRIX, luminanceMatrix);
        filter.SetColorFilter(luminanceColorFilter);
        params.paint.SetFilter(filter);
    }

    if (videoInfo.retGetColorSpaceInfo_ != GSERROR_OK) {
        RS_LOGD("RSBaseRenderEngine::DrawImage GetColorSpaceInfo failed with %{public}u.",
            videoInfo.retGetColorSpaceInfo_);
        DrawImageRect(canvas, image, params, samplingOptions);
        return;
    }

    Media::VideoProcessingEngine::CM_ColorSpaceInfo& inClrInfo = videoInfo.parameter_.inputColorSpace.colorSpaceInfo;
    Media::VideoProcessingEngine::CM_ColorSpaceInfo& outClrInfo = videoInfo.parameter_.outputColorSpace.colorSpaceInfo;
    if (!ConvertDrawingColorSpaceToSpaceInfo(videoInfo.drawingColorSpace_, outClrInfo)) {
        RS_LOGD("RSBaseRenderEngine::DrawImage ConvertDrawingColorSpaceToSpaceInfo failed");
        DrawImageRect(canvas, image, params, samplingOptions);
        return;
    }
 
    if (inClrInfo.primaries == outClrInfo.primaries && inClrInfo.transfunc == outClrInfo.transfunc) {
        RS_LOGD("RSBaseRenderEngine::DrawImage primaries and transfunc equal.");
        DrawImageRect(canvas, image, params, samplingOptions);
        return;
    }
    Drawing::Matrix matrix;
    auto srcWidth = params.srcRect.GetWidth();
    auto srcHeight = params.srcRect.GetHeight();
    auto sx = params.dstRect.GetWidth() / srcWidth;
    auto sy = params.dstRect.GetHeight() / srcHeight;
    auto tx = params.dstRect.GetLeft() - params.srcRect.GetLeft() * sx;
    auto ty = params.dstRect.GetTop() - params.srcRect.GetTop() * sy;
    if (ROSEN_EQ(srcWidth, 0.0f) || ROSEN_EQ(srcHeight, 0.0f)) {
        RS_LOGE("RSBaseRenderEngine::DrawImage image srcRect params invalid.");
    }
    matrix.SetScaleTranslate(sx, sy, tx, ty);
    auto imageShader = Drawing::ShaderEffect::CreateImageShader(
        *image, Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP, samplingOptions, matrix);
    if (imageShader == nullptr) {
        RS_LOGW("RSBaseRenderEngine::DrawImage imageShader is nullptr.");
    } else {
        params.paint.SetShaderEffect(imageShader);
        ColorSpaceConvertor(imageShader, params, videoInfo.parameter_);
    }
    canvas.AttachBrush(params.paint);
    canvas.DrawRect(params.dstRect);
    canvas.DetachBrush();
#else
    DrawImageRect(canvas, image, params, samplingOptions);
#endif // USE_VIDEO_PROCESSING_ENGINE
    }
}

void RSBaseRenderEngine::DrawImageRect(RSPaintFilterCanvas& canvas, std::shared_ptr<Drawing::Image> image,
    BufferDrawParam& params, Drawing::SamplingOptions& samplingOptions)
{
    canvas.AttachBrush(params.paint);
    canvas.DrawImageRect(*image, params.srcRect, params.dstRect, samplingOptions,
        Drawing::SrcRectConstraint::FAST_SRC_RECT_CONSTRAINT);
    canvas.DetachBrush();
}

bool RSBaseRenderEngine::CheckIsHdrSurfaceBuffer(const sptr<SurfaceBuffer> surfaceBuffer)
{
    if (surfaceBuffer == nullptr) {
        return false;
    }
    if (surfaceBuffer->GetFormat() != GRAPHIC_PIXEL_FMT_YCBCR_P010 &&
        surfaceBuffer->GetFormat() != GRAPHIC_PIXEL_FMT_YCRCB_P010) {
        return false;
    }
    using namespace HDI::Display::Graphic::Common::V1_0;
    CM_ColorSpaceInfo colorSpaceInfo;
    if (MetadataHelper::GetColorSpaceInfo(surfaceBuffer, colorSpaceInfo) == GSERROR_OK) {
        if (colorSpaceInfo.transfunc == TRANSFUNC_PQ || colorSpaceInfo.transfunc == TRANSFUNC_HLG) {
            return true;
        }
    }
    return false;
}

void RSBaseRenderEngine::RegisterDeleteBufferListener(const sptr<IConsumerSurface>& consumer, bool isForUniRedraw)
{
#ifdef RS_ENABLE_VK
    if (RSSystemProperties::IsUseVulkan()) {
        auto regUnMapVkImageFunc = [this, isForUniRedraw](int32_t bufferId) {
            RSMainThread::Instance()->AddToUnmappedCacheSet(bufferId);
        };
        if (consumer == nullptr ||
            (consumer->RegisterDeleteBufferListener(regUnMapVkImageFunc, isForUniRedraw) != GSERROR_OK)) {
            RS_LOGE("RSBaseRenderEngine::RegisterDeleteBufferListener: failed to register UnMapVkImage callback.");
        }
        return;
    }
#endif // #ifdef RS_ENABLE_VK

#ifdef RS_ENABLE_EGLIMAGE
    auto regUnMapEglImageFunc = [this, isForUniRedraw](int32_t bufferId) {
        RSMainThread::Instance()->AddToUnmappedCacheSet(bufferId);
    };
    if (consumer == nullptr ||
        (consumer->RegisterDeleteBufferListener(regUnMapEglImageFunc, isForUniRedraw) != GSERROR_OK)) {
        RS_LOGE("RSBaseRenderEngine::RegisterDeleteBufferListener: failed to register UnMapEglImage callback.");
    }
#endif // #ifdef RS_ENABLE_EGLIMAGE
}

void RSBaseRenderEngine::RegisterDeleteBufferListener(RSSurfaceHandler& handler)
{
#ifdef RS_ENABLE_VK
    if (RSSystemProperties::IsUseVulkan()) {
        auto regUnMapVkImageFunc = [this](int32_t bufferId) {
            RSMainThread::Instance()->AddToUnmappedCacheSet(bufferId);
        };
        handler.RegisterDeleteBufferListener(regUnMapVkImageFunc);
        return;
    }
#endif // #ifdef RS_ENABLE_VK

#ifdef RS_ENABLE_EGLIMAGE
    auto regUnMapEglImageFunc = [this](int32_t bufferId) {
        RSMainThread::Instance()->AddToUnmappedCacheSet(bufferId);
    };
    handler.RegisterDeleteBufferListener(regUnMapEglImageFunc);
#endif // #ifdef RS_ENABLE_EGLIMAGE
}

void RSBaseRenderEngine::ShrinkCachesIfNeeded(bool isForUniRedraw)
{
#ifdef RS_ENABLE_VK
    if (RSSystemProperties::IsUseVulkan()) {
        if (vkImageManager_ != nullptr) {
            vkImageManager_->ShrinkCachesIfNeeded();
        }
        return;
    }
#endif // RS_ENABLE_VK

#ifdef RS_ENABLE_EGLIMAGE
    if (eglImageManager_ != nullptr) {
        eglImageManager_->ShrinkCachesIfNeeded(isForUniRedraw);
    }
#endif // RS_ENABLE_EGLIMAGE
}

void RSBaseRenderEngine::ClearCacheSet(const std::set<int32_t> unmappedCache)
{
#ifdef RS_ENABLE_VK
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        if (vkImageManager_ != nullptr) {
            for (auto id : unmappedCache) {
                vkImageManager_->UnMapVkImageFromSurfaceBuffer(id);
            }
        }
    }
#endif // RS_ENABLE_VK

#ifdef RS_ENABLE_EGLIMAGE
    if (eglImageManager_ != nullptr) {
        for (auto id : unmappedCache) {
            eglImageManager_->UnMapEglImageFromSurfaceBuffer(id);
        }
    }
#endif // RS_ENABLE_EGLIMAGE
}
} // namespace Rosen
} // namespace OHOS
