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
#include "rs_divided_render_util.h"
#include "common/rs_optional_trace.h"
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
#include "platform/ohos/backend/rs_surface_ohos_vulkan.h"
#endif
#endif
#include "render/rs_skia_filter.h"

#include "drawing_context.h"
#include "rs_render_surface_frame.h"
#include "render_context_base.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkSurface.h"

#ifndef ROSEN_CROSS_PLATFORM
#include "surface_type.h"
#endif
#include "pipeline/rs_egl_image_manager.h"

#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_main_thread.h"
#include "render_context/render_context.h"

namespace OHOS {
namespace Rosen {
RSBaseRenderEngine::RSBaseRenderEngine()
{
}

RSBaseRenderEngine::~RSBaseRenderEngine() noexcept
{
}

void RSBaseRenderEngine::Init()
{
#if defined(NEW_RENDER_CONTEXT)
    RenderType renderType = RenderType::GLES;
#if defined(RS_ENABLE_GL)
    renderType = RenderType::GLES;
#elif defined(RS_ENABLE_VK)
    renderType = RenderType::VULKAN;
#else
    renderType = RenderType::RASTER;
#endif
    renderContext_ = RenderContextBaseFactory::CreateRenderContext(renderType);
    renderContext_->Init();
    drawingContext_ = std::make_shared<Rosen::DrawingContext>(renderContext_->GetRenderType());
    drawingContext_->SetUpDrawingContext();
#else
#if (defined RS_ENABLE_GL) || (defined RS_ENABLE_VK)
    renderContext_ = std::make_shared<RenderContext>();
    renderContext_->InitializeEglContext();
    if (RSUniRenderJudgement::IsUniRender()) {
        RS_LOGI("RSRenderEngine::RSRenderEngine set new cacheDir");
        renderContext_->SetUniRenderMode(true);
    }
#ifndef USE_ROSEN_DRAWING
    renderContext_->SetUpGrContext();
#else
    renderContext_->SetUpGpuContext();
#endif // USE_ROSEN_DRAWING
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
}

bool RSBaseRenderEngine::NeedForceCPU(const std::vector<LayerInfoPtr>& layers)
{
#ifdef RS_ENABLE_GL
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

        GraphicColorGamut srcGamut = static_cast<GraphicColorGamut>(buffer->GetSurfaceBufferColorGamut());
        GraphicColorGamut dstGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB;
        if (srcGamut != dstGamut) {
            forceCPU = true;
            break;
        }
    }

    return forceCPU;
#else
    return true;
#endif
}

#ifndef USE_ROSEN_DRAWING
sk_sp<SkImage> RSBaseRenderEngine::CreateEglImageFromBuffer(RSPaintFilterCanvas& canvas,
    const sptr<SurfaceBuffer>& buffer, const sptr<SyncFence>& acquireFence, const uint32_t threadIndex)
#else
std::shared_ptr<Drawing::Image> RSBaseRenderEngine::CreateEglImageFromBuffer(RSPaintFilterCanvas& canvas,
    const sptr<SurfaceBuffer>& buffer, const sptr<SyncFence>& acquireFence, const uint32_t threadIndex)
#endif
{
#ifdef RS_ENABLE_EGLIMAGE
    if (!RSBaseRenderUtil::IsBufferValid(buffer)) {
        RS_LOGE("RSBaseRenderEngine::CreateEglImageFromBuffer invalid param!");
        return nullptr;
    }
#ifndef USE_ROSEN_DRAWING
#ifdef NEW_SKIA
    if (canvas.recordingContext() == nullptr) {
#else
    if (canvas.getGrContext() == nullptr) {
#endif
#else
    if (canvas.GetGPUContext() == nullptr) {
#endif
        RS_LOGE("RSBaseRenderEngine::CreateEglImageFromBuffer GrContext is null!");
        return nullptr;
    }
    auto eglTextureId = eglImageManager_->MapEglImageFromSurfaceBuffer(buffer, acquireFence, threadIndex);
    if (eglTextureId == 0) {
        RS_LOGE("RSBaseRenderEngine::CreateEglImageFromBuffer MapEglImageFromSurfaceBuffer return invalid texture ID");
        return nullptr;
    }
#ifndef USE_ROSEN_DRAWING
    SkColorType colorType = (buffer->GetFormat() == GRAPHIC_PIXEL_FMT_BGRA_8888) ?
        kBGRA_8888_SkColorType : kRGBA_8888_SkColorType;
    GrGLTextureInfo grExternalTextureInfo = { GL_TEXTURE_EXTERNAL_OES, eglTextureId, GL_RGBA8 };
    GrBackendTexture backendTexture(buffer->GetSurfaceBufferWidth(), buffer->GetSurfaceBufferHeight(),
        GrMipMapped::kNo, grExternalTextureInfo);
#ifdef NEW_SKIA
    return SkImage::MakeFromTexture(canvas.recordingContext(), backendTexture,
        kTopLeft_GrSurfaceOrigin, colorType, kPremul_SkAlphaType, nullptr);
#else
    return SkImage::MakeFromTexture(canvas.getGrContext(), backendTexture,
        kTopLeft_GrSurfaceOrigin, colorType, kPremul_SkAlphaType, nullptr);
#endif
#else
    Drawing::ColorType colorType = (buffer->GetFormat() == GRAPHIC_PIXEL_FMT_BGRA_8888) ?
        Drawing::ColorType::COLORTYPE_BGRA_8888 : Drawing::ColorType::COLORTYPE_RGBA_8888;
    Drawing::BitmapFormat bitmapFormat = { colorType, Drawing::AlphaType::ALPHATYPE_PREMUL };

    Drawing::TextureInfo externalTextureInfo;
    externalTextureInfo.SetWidth(buffer->GetSurfaceBufferWidth());
    externalTextureInfo.SetHeight(buffer->GetSurfaceBufferHeight());
    externalTextureInfo.SetIsMipMapped(false);
    externalTextureInfo.SetTarget(GL_TEXTURE_EXTERNAL_OES);
    externalTextureInfo.SetID(eglTextureId);
    externalTextureInfo.SetFormat(GL_RGBA8);

    auto image = std::make_shared<Drawing::Image>();
    if (!image->BuildFromTexture(*canvas.GetGPUContext(), externalTextureInfo,
        Drawing::TextureOrigin::TOP_LEFT, bitmapFormat, nullptr)) {
        RS_LOGE("RSBaseRenderEngine::CreateEglImageFromBuffer image BuildFromTexture failed");
        return nullptr;
    }
    return image;
#endif
#else
    return nullptr;
#endif // RS_ENABLE_EGLIMAGE
}

#ifdef NEW_RENDER_CONTEXT
std::unique_ptr<RSRenderFrame> RSBaseRenderEngine::RequestFrame(
    const std::shared_ptr<RSRenderSurfaceOhos>& rsSurface,
    const BufferRequestConfig& config, bool forceCPU, bool useAFBC)
#else
std::unique_ptr<RSRenderFrame> RSBaseRenderEngine::RequestFrame(const std::shared_ptr<RSSurfaceOhos>& rsSurface,
    const BufferRequestConfig& config, bool forceCPU, bool useAFBC)
#endif
{
    if (rsSurface == nullptr) {
        RS_LOGE("RSBaseRenderEngine::RequestFrame: surface is null!");
        return nullptr;
    }
    RS_OPTIONAL_TRACE_BEGIN("RSBaseRenderEngine::RequestFrame(RSSurface)");
    rsSurface->SetSurfacePixelFormat(config.format);

    auto bufferUsage = config.usage;
#if (defined RS_ENABLE_GL) && (defined RS_ENABLE_EGLIMAGE)
    if (forceCPU) {
        bufferUsage |= BUFFER_USAGE_CPU_WRITE;
    }
#else // (defined RS_ENABLE_GL) && (defined RS_ENABLE_EGLIMAGE)
    bufferUsage |= BUFFER_USAGE_CPU_WRITE;
#endif // (defined RS_ENABLE_GL) && (defined RS_ENABLE_EGLIMAGE)
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
    if (renderContext_ != nullptr) {
        rsSurface->SetRenderContext(renderContext_.get());
    }
#endif
#endif
    auto surfaceFrame = rsSurface->RequestFrame(config.width, config.height, 0, useAFBC);
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
    const BufferRequestConfig& config, bool forceCPU, bool useAFBC)
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
    if (forceCPU) {
        rsSurface = std::make_shared<RSSurfaceOhosRaster>(targetSurface);
    } else {
        rsSurface = std::make_shared<RSSurfaceOhosGl>(targetSurface);
    }
#elif defined(RS_ENABLE_VK)
    rsSurface = std::make_shared<RSSurfaceOhosVulkan>(targetSurface);
#else
    rsSurface = std::make_shared<RSSurfaceOhosRaster>(targetSurface);
#endif // (defined RS_ENABLE_GL) && (defined RS_ENABLE_EGLIMAGE)
#endif
    RS_OPTIONAL_TRACE_END();
    return RequestFrame(rsSurface, config, forceCPU, useAFBC);
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
        RegisterDeleteBufferListener(node.GetConsumer());
        DrawImage(canvas, params);
    }
}

void RSBaseRenderEngine::SetColorFilterMode(ColorFilterMode mode)
{
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

void RSBaseRenderEngine::DrawBuffer(RSPaintFilterCanvas& canvas, BufferDrawParam& params)
{
    RS_OPTIONAL_TRACE_BEGIN("RSBaseRenderEngine::DrawBuffer(CPU)");
#ifndef USE_ROSEN_DRAWING
    SkBitmap bitmap;
#else
    Drawing::Bitmap bitmap;
#endif
    std::vector<uint8_t> newBuffer;
    if (!RSBaseRenderUtil::ConvertBufferToBitmap(params.buffer, newBuffer, params.targetColorGamut, bitmap,
        params.metaDatas)) {
        RS_LOGE("RSDividedRenderUtil::DrawBuffer: create bitmap failed.");
        RS_OPTIONAL_TRACE_END();
        return;
    }
#ifndef USE_ROSEN_DRAWING
#ifdef NEW_SKIA
    canvas.drawImageRect(bitmap.asImage(), params.srcRect, params.dstRect,
        SkSamplingOptions(), &(params.paint), SkCanvas::kStrict_SrcRectConstraint);
#else
    canvas.drawBitmapRect(bitmap, params.srcRect, params.dstRect, &(params.paint));
#endif
#else
    Drawing::Image drImage;
    drImage.BuildFromBitmap(bitmap);
    canvas.DrawImageRect(drImage, params.srcRect, params.dstRect, Drawing::SamplingOptions(),
        Drawing::SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT);
#endif
    RS_OPTIONAL_TRACE_END();
}

void RSBaseRenderEngine::DrawImage(RSPaintFilterCanvas& canvas, BufferDrawParam& params)
{
    RS_OPTIONAL_TRACE_BEGIN("RSBaseRenderEngine::DrawImage(GPU)");
    auto image = CreateEglImageFromBuffer(canvas, params.buffer, params.acquireFence, params.threadIndex);
    if (image == nullptr) {
        RS_LOGE("RSBaseRenderEngine::DrawImage: image is nullptr!");
        RS_OPTIONAL_TRACE_END();
        return;
    }
#ifndef USE_ROSEN_DRAWING
#ifdef NEW_SKIA
    canvas.drawImageRect(image, params.srcRect, params.dstRect,
        SkSamplingOptions(), &(params.paint), SkCanvas::kStrict_SrcRectConstraint);
#else
    canvas.drawImageRect(image, params.srcRect, params.dstRect, &(params.paint));
#endif
#else
    canvas.AttachBrush(params.paint);
    canvas.DrawImageRect(*image.get(), params.srcRect, params.dstRect,
        Drawing::SamplingOptions(), Drawing::SrcRectConstraint::FAST_SRC_RECT_CONSTRAINT);
    canvas.DetachBrush();
#endif
    RS_OPTIONAL_TRACE_END();
}

void RSBaseRenderEngine::RegisterDeleteBufferListener(const sptr<IConsumerSurface>& consumer, bool isForUniRedraw)
{
#ifdef RS_ENABLE_EGLIMAGE
    auto regUnMapEglImageFunc = [this, isForUniRedraw](int32_t bufferId) {
        if (isForUniRedraw) {
            eglImageManager_->UnMapEglImageFromSurfaceBufferForUniRedraw(bufferId);
        } else {
            eglImageManager_->UnMapEglImageFromSurfaceBuffer(bufferId);
        }
    };
    if (consumer == nullptr ||
        (consumer->RegisterDeleteBufferListener(regUnMapEglImageFunc, isForUniRedraw) != GSERROR_OK)) {
        RS_LOGE("RSBaseRenderEngine::RegisterDeleteBufferListener: failed to register UnMapEglImage callback.");
    }
#endif // #ifdef RS_ENABLE_EGLIMAGE
}

void RSBaseRenderEngine::RegisterDeleteBufferListener(RSSurfaceHandler& handler)
{
#ifdef RS_ENABLE_EGLIMAGE
    auto regUnMapEglImageFunc = [this](int32_t bufferId) {
        eglImageManager_->UnMapEglImageFromSurfaceBuffer(bufferId);
    };
    handler.RegisterDeleteBufferListener(regUnMapEglImageFunc);
#endif // #ifdef RS_ENABLE_EGLIMAGE
}

void RSBaseRenderEngine::ShrinkCachesIfNeeded(bool isForUniRedraw)
{
#ifdef RS_ENABLE_EGLIMAGE
    if (eglImageManager_ != nullptr) {
        eglImageManager_->ShrinkCachesIfNeeded(isForUniRedraw);
    }
#endif // RS_ENABLE_EGLIMAGE
}
} // namespace Rosen
} // namespace OHOS
