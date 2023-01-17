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

#include "rs_divided_render_util.h"
#include "rs_trace.h"

#include "pipeline/rs_uni_render_judgement.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "platform/ohos/backend/rs_surface_ohos_gl.h"
#include "platform/ohos/backend/rs_surface_ohos_raster.h"
#include "render/rs_skia_filter.h"

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
#ifdef RS_ENABLE_GL
    renderContext_ = std::make_shared<RenderContext>();
    renderContext_->InitializeEglContext();
    if (RSUniRenderJudgement::IsUniRender()) {
        RS_LOGI("RSRenderEngine::RSRenderEngine set new cacheDir");
        renderContext_->SetUniRenderMode(true);
    }
    renderContext_->SetUpGrContext();
#endif // RS_ENABLE_GL

#ifdef RS_ENABLE_EGLIMAGE
    eglImageManager_ = std::make_shared<RSEglImageManager>(renderContext_->GetEGLDisplay());
#endif // RS_ENABLE_EGLIMAGE
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
        if (bufferFormat == PIXEL_FMT_YCRCB_420_SP || bufferFormat == PIXEL_FMT_YCBCR_420_SP) {
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
}

sk_sp<SkImage> RSBaseRenderEngine::CreateEglImageFromBuffer(
    const sptr<SurfaceBuffer>& buffer, const sptr<SyncFence>& acquireFence)
{
#ifdef RS_ENABLE_EGLIMAGE
    if (!RSBaseRenderUtil::IsBufferValid(buffer)) {
        RS_LOGE("RSBaseRenderEngine::CreateEglImageFromBuffer invalid param!");
        return nullptr;
    }
    if (renderContext_->GetGrContext() == nullptr) {
        RS_LOGE("RSBaseRenderEngine::CreateEglImageFromBuffer GrContext is null!");
        return nullptr;
    }
    auto eglTextureId = eglImageManager_->MapEglImageFromSurfaceBuffer(buffer, acquireFence);
    if (eglTextureId == 0) {
        RS_LOGE("RSBaseRenderEngine::CreateEglImageFromBuffer MapEglImageFromSurfaceBuffer return invalid texture ID");
        return nullptr;
    }
    SkColorType colorType = (buffer->GetFormat() == PIXEL_FMT_BGRA_8888) ?
        kBGRA_8888_SkColorType : kRGBA_8888_SkColorType;
    GrGLTextureInfo grExternalTextureInfo = { GL_TEXTURE_EXTERNAL_OES, eglTextureId, GL_RGBA8 };
    GrBackendTexture backendTexture(buffer->GetSurfaceBufferWidth(), buffer->GetSurfaceBufferHeight(),
        GrMipMapped::kNo, grExternalTextureInfo);
    return SkImage::MakeFromTexture(renderContext_->GetGrContext(), backendTexture,
        kTopLeft_GrSurfaceOrigin, colorType, kPremul_SkAlphaType, nullptr);
#else
    return nullptr;
#endif // RS_ENABLE_EGLIMAGE
}

std::unique_ptr<RSRenderFrame> RSBaseRenderEngine::RequestFrame(const std::shared_ptr<RSSurfaceOhos>& rsSurface,
    const BufferRequestConfig& config, bool forceCPU, bool useAFBC)
{
    RS_TRACE_NAME("RSBaseRenderEngine::RequestFrame(RSSurface)");
    if (rsSurface == nullptr) {
        RS_LOGE("RSBaseRenderEngine::RequestFrame: surface is null!");
        return nullptr;
    }

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
#ifdef RS_ENABLE_GL
    if (renderContext_ != nullptr) {
        rsSurface->SetRenderContext(renderContext_.get());
    }
#endif

    auto surfaceFrame = rsSurface->RequestFrame(config.width, config.height, 0, useAFBC);
    if (surfaceFrame == nullptr) {
        RS_LOGE("RSBaseRenderEngine::RequestFrame: request SurfaceFrame failed!");
        return nullptr;
    }

    return std::make_unique<RSRenderFrame>(rsSurface, std::move(surfaceFrame));
}

std::unique_ptr<RSRenderFrame> RSBaseRenderEngine::RequestFrame(const sptr<Surface>& targetSurface,
    const BufferRequestConfig& config, bool forceCPU, bool useAFBC)
{
    RS_TRACE_NAME("RSBaseRenderEngine::RequestFrame(Surface)");
    if (targetSurface == nullptr) {
        RS_LOGE("RSBaseRenderEngine::RequestFrame: surface is null!");
        return nullptr;
    }

    auto surfaceId = targetSurface->GetUniqueId();
    if (rsSurfaces_.count(surfaceId) == 0) {
#if (defined RS_ENABLE_GL) && (defined RS_ENABLE_EGLIMAGE)
        if (forceCPU) {
            rsSurfaces_[surfaceId] = std::make_shared<RSSurfaceOhosRaster>(targetSurface);
        } else {
            rsSurfaces_[surfaceId] = std::make_shared<RSSurfaceOhosGl>(targetSurface);
        }
#else
        rsSurfaces_[surfaceId] = std::make_shared<RSSurfaceOhosRaster>(targetSurface);
#endif // (defined RS_ENABLE_GL) && (defined RS_ENABLE_EGLIMAGE)
    }

    return RequestFrame(rsSurfaces_.at(surfaceId), config, forceCPU, useAFBC);
}

void RSBaseRenderEngine::SetUiTimeStamp(const std::unique_ptr<RSRenderFrame>& renderFrame, const uint64_t surfaceId)
{
    std::shared_ptr<RSSurfaceOhos> surfaceOhos = nullptr;
    for (auto it = rsSurfaces_.begin(); it != rsSurfaces_.end(); ++it) {
        if (it->first == surfaceId) {
            surfaceOhos = it->second;
            break;
        }
    }

    if (surfaceOhos == nullptr) {
        RS_LOGE("RSBaseRenderEngine::SetUiTimeStamp: surfaceOhos is null!");
        return;
    }

    if (renderFrame == nullptr) {
        RS_LOGE("RSBaseRenderEngine::SetUiTimeStamp: renderFrame is null!.");
        return;
    }

    auto& frame = renderFrame->GetFrame();
    surfaceOhos->SetUiTimeStamp(frame);
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
    RS_TRACE_NAME("RSBaseRenderEngine::DrawBuffer(CPU)");
    SkBitmap bitmap;
    std::vector<uint8_t> newBuffer;
    if (!RSBaseRenderUtil::ConvertBufferToBitmap(params.buffer, newBuffer, params.targetColorGamut, bitmap,
        params.metaDatas)) {
        RS_LOGE("RSDividedRenderUtil::DrawBuffer: create bitmap failed.");
        return;
    }
    canvas.drawBitmapRect(bitmap, params.srcRect, params.dstRect, &(params.paint));
}

void RSBaseRenderEngine::DrawImage(RSPaintFilterCanvas& canvas, BufferDrawParam& params)
{
    RS_TRACE_NAME("RSBaseRenderEngine::DrawImage(GPU)");
    auto image = CreateEglImageFromBuffer(params.buffer, params.acquireFence);
    if (image == nullptr) {
        RS_LOGE("RSDividedRenderUtil::DrawImage: image is nullptr!");
        return;
    }
    canvas.drawImageRect(image, params.srcRect, params.dstRect, &(params.paint));
}

void RSBaseRenderEngine::RegisterDeleteBufferListener(const sptr<Surface>& consumer)
{
#ifdef RS_ENABLE_EGLIMAGE
    auto regUnMapEglImageFunc = [](int32_t bufferId) {
        eglImageManager_->UnMapEglImageFromSurfaceBuffer(bufferId);
    };
    if (consumer == nullptr ||
        (consumer->RegisterDeleteBufferListener(regUnMapEglImageFunc) != GSERROR_OK)) {
        RS_LOGE("RSBaseRenderEngine::RegisterDeleteBufferListener: failed to register UnMapEglImage callback.");
    }
#endif // #ifdef RS_ENABLE_EGLIMAGE
}

void RSBaseRenderEngine::ShrinkCachesIfNeeded()
{
#ifdef RS_ENABLE_EGLIMAGE
    if (eglImageManager_ != nullptr) {
        eglImageManager_->ShrinkCachesIfNeeded();
    }
#endif // RS_ENABLE_EGLIMAGE

    while (rsSurfaces_.size() > MAX_RS_SURFACE_SIZE) {
        auto it = rsSurfaces_.begin();
        (void)rsSurfaces_.erase(it);
    }
}
} // namespace Rosen
} // namespace OHOS
