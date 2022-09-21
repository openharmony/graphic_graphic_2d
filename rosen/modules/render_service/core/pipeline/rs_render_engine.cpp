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

#include "rs_render_engine.h"

#include "rs_divided_render_util.h"
#include "rs_trace.h"
#include "string_utils.h"

#include "pipeline/rs_uni_render_judgement.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "platform/ohos/backend/rs_surface_ohos_gl.h"
#include "platform/ohos/backend/rs_surface_ohos_raster.h"
#include "render/rs_skia_filter.h"

namespace OHOS {
namespace Rosen {
RSRenderEngine::RSRenderEngine()
{
#ifdef RS_ENABLE_GL
    renderContext_ = std::make_shared<RenderContext>();
    renderContext_->InitializeEglContext();
    renderContext_->SetUpGrContext();
#endif // RS_ENABLE_GL

#ifdef RS_ENABLE_EGLIMAGE
    eglImageManager_ = std::make_shared<RSEglImageManager>(renderContext_->GetEGLDisplay());
#endif // RS_ENABLE_EGLIMAGE
}

RSRenderEngine::~RSRenderEngine() noexcept
{
}

bool RSRenderEngine::NeedForceCPU(const std::vector<LayerInfoPtr>& layers)
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

        ColorGamut srcGamut = static_cast<ColorGamut>(buffer->GetSurfaceBufferColorGamut());
        ColorGamut dstGamut = ColorGamut::COLOR_GAMUT_SRGB;
        if (srcGamut != dstGamut) {
            forceCPU = true;
            break;
        }
    }

    return forceCPU;
}

sk_sp<SkImage> RSRenderEngine::CreateEglImageFromBuffer(
    const sptr<SurfaceBuffer>& buffer, const sptr<SyncFence>& acquireFence)
{
#ifdef RS_ENABLE_EGLIMAGE
    if (!RSBaseRenderUtil::IsBufferValid(buffer)) {
        RS_LOGE("RSRenderEngine::CreateEglImageFromBuffer invalid param!");
        return nullptr;
    }
    if (renderContext_->GetGrContext() == nullptr) {
        RS_LOGE("RSRenderEngine::CreateEglImageFromBuffer GrContext is null!");
        return nullptr;
    }
    auto eglTextureId = eglImageManager_->MapEglImageFromSurfaceBuffer(buffer, acquireFence);
    if (eglTextureId == 0) {
        RS_LOGE("RSRenderEngine::CreateEglImageFromBuffer MapEglImageFromSurfaceBuffer return invalid texture ID");
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

std::unique_ptr<RSRenderFrame> RSRenderEngine::RequestFrame(
    const std::shared_ptr<RSSurfaceOhos>& rsSurface,
    const BufferRequestConfig& config,
    bool forceCPU)
{
    RS_TRACE_NAME("RSRenderEngine::RequestFrame(RSSurface)");
    if (rsSurface == nullptr) {
        RS_LOGE("RSRenderEngine::RequestFrame: surface is null!");
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

    auto surfaceFrame = rsSurface->RequestFrame(config.width, config.height);
    if (surfaceFrame == nullptr) {
        RS_LOGE("RSRenderEngine::RequestFrame: request SurfaceFrame failed!");
        return nullptr;
    }

    return std::make_unique<RSRenderFrame>(rsSurface, std::move(surfaceFrame));
}

std::unique_ptr<RSRenderFrame> RSRenderEngine::RequestFrame(
    const sptr<Surface>& targetSurface,
    const BufferRequestConfig& config,
    bool forceCPU)
{
    RS_TRACE_NAME("RSRenderEngine::RequestFrame(Surface)");
    if (targetSurface == nullptr) {
        RS_LOGE("RSRenderEngine::RequestFrame: surface is null!");
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

    return RequestFrame(rsSurfaces_.at(surfaceId), config, forceCPU);
}

void RSRenderEngine::DrawLayers(
    RSPaintFilterCanvas& canvas,
    const std::vector<LayerInfoPtr>& layers,
    bool forceCPU,
    float mirrorAdaptiveCoefficient)
{
    for (const auto& layer : layers) {
        if (layer == nullptr) {
            continue;
        }
        if (layer->GetCompositionType() == CompositionType::COMPOSITION_DEVICE ||
            layer->GetCompositionType() == CompositionType::COMPOSITION_DEVICE_CLEAR) {
            continue;
        }
        auto nodePtr = static_cast<RSBaseRenderNode*>(layer->GetLayerAdditionalInfo());
        if (nodePtr == nullptr) {
            RS_LOGE("RSRenderEngine::DrawLayers: node is nullptr!");
            continue;
        }

        auto saveCount = canvas.getSaveCount();
        if (nodePtr->IsInstanceOf<RSSurfaceRenderNode>()) {
            RSSurfaceRenderNode& node = *(static_cast<RSSurfaceRenderNode*>(nodePtr));
            if (layer->GetCompositionType() == CompositionType::COMPOSITION_CLIENT_CLEAR ||
            layer->GetCompositionType() == CompositionType::COMPOSITION_TUNNEL) {
                ClipHoleForLayer(canvas, node);
                canvas.restoreToCount(saveCount);
                continue;
            }
            RS_LOGD("RSRenderEngine::DrawLayers dstRect[%d %d %d %d] SrcRect[%d %d %d %d]",
                layer->GetLayerSize().x, layer->GetLayerSize().y,
                layer->GetLayerSize().w, layer->GetLayerSize().h,
                layer->GetDirtyRegion().x, layer->GetDirtyRegion().y,
                layer->GetDirtyRegion().w, layer->GetDirtyRegion().h);
            DrawSurfaceNode(canvas, node, mirrorAdaptiveCoefficient, forceCPU);
        } else if (nodePtr->IsInstanceOf<RSDisplayRenderNode>()) {
            // In uniRender mode, maybe need to handle displayNode.
            RSDisplayRenderNode& node = *(static_cast<RSDisplayRenderNode*>(nodePtr));
            DrawDisplayNode(canvas, node, forceCPU);
        } else {
            // Probably never reach this branch.
            RS_LOGE("RSRenderEngine::DrawLayers: unexpected node type!");
            continue;
        }
        canvas.restoreToCount(saveCount);
    }
}

void RSRenderEngine::ClipHoleForLayer(
    RSPaintFilterCanvas& canvas,
    RSSurfaceRenderNode& node)
{
    BufferDrawParam params = RSBaseRenderUtil::CreateBufferDrawParam(node, false, true);

    std::string traceInfo;
    AppendFormat(traceInfo, "Node name:%s ClipHole[%d %d %d %d]", node.GetName().c_str(),
        params.clipRect.x(), params.clipRect.y(), params.clipRect.width(), params.clipRect.height());
    RS_LOGD("RsDebug RSRenderEngine::Redraw layer composition ClipHoleForLayer, %s.", traceInfo.c_str());
    RS_TRACE_NAME(traceInfo);

    canvas.save();
    canvas.clipRect(params.clipRect);
    canvas.clear(SK_ColorTRANSPARENT);
    canvas.restore();
    return;
}

void RSRenderEngine::SetColorFilterMode(ColorFilterMode mode)
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

void RSRenderEngine::SetColorFilterModeToPaint(SkPaint& paint)
{
    ColorFilterMode mode = static_cast<ColorFilterMode>(RSSystemProperties::GetCorrectionMode());
    if (RSBaseRenderUtil::IsColorFilterModeValid(mode)) {
        colorFilterMode_ = mode;
    }

    // for test automation
    if (colorFilterMode_ != ColorFilterMode::COLOR_FILTER_END) {
        RS_LOGD("RsDebug RSRenderEngine::SetColorFilterModeToPaint mode:%d", static_cast<int32_t>(colorFilterMode_));
    }
    RSBaseRenderUtil::SetColorFilterModeToPaint(colorFilterMode_, paint);
}

void RSRenderEngine::DrawBuffer(RSPaintFilterCanvas& canvas, BufferDrawParam& params)
{
    SkBitmap bitmap;
    std::vector<uint8_t> newBuffer;
    if (!RSBaseRenderUtil::ConvertBufferToBitmap(params.buffer, newBuffer, params.targetColorGamut, bitmap,
        params.metaDatas)) {
        RS_LOGE("RSDividedRenderUtil::DrawBuffer: create bitmap failed.");
        return;
    }
    canvas.drawBitmapRect(bitmap, params.srcRect, params.dstRect, &(params.paint));
}

void RSRenderEngine::DrawImage(RSPaintFilterCanvas& canvas, BufferDrawParam& params)
{
    RS_TRACE_NAME("RSRenderEngine::DrawImage(GPU)");
    auto image = CreateEglImageFromBuffer(params.buffer, params.acquireFence);
    if (image == nullptr) {
        RS_LOGE("RSDividedRenderUtil::DrawImage: image is nullptr!");
        return;
    }

    canvas.drawImageRect(image, params.srcRect, params.dstRect, &(params.paint));
}

void RSRenderEngine::DrawWithParams(RSPaintFilterCanvas& canvas, BufferDrawParam& params,
    PreProcessFunc preProcess, PostProcessFunc postProcess)
{
    if (params.setColorFilter) {
        SetColorFilterModeToPaint(params.paint);
    }

    canvas.save();

    RSBaseRenderUtil::SetPropertiesForCanvas(canvas, params);

    if (preProcess != nullptr) {
        preProcess(canvas, params);
    }

    if (params.useCPU) {
        RSRenderEngine::DrawBuffer(canvas, params);
    } else {
        RSRenderEngine::DrawImage(canvas, params);
    }

    if (postProcess != nullptr) {
        postProcess(canvas, params);
    }

    canvas.restore();
}

void RSRenderEngine::RSSurfaceNodeCommonPreProcess(
    RSSurfaceRenderNode& node,
    RSPaintFilterCanvas& canvas,
    BufferDrawParam& params)
{
    const auto& property = node.GetRenderProperties();

    // draw mask.
    RectF maskBounds(0, 0, params.dstRect.width(), params.dstRect.height());
    RSPropertiesPainter::DrawMask(
        node.GetRenderProperties(), canvas, RSPropertiesPainter::Rect2SkRect(maskBounds));

    // draw background filter (should execute this filter before drawing buffer/image).
    auto filter = std::static_pointer_cast<RSSkiaFilter>(property.GetBackgroundFilter());
    if (filter != nullptr) {
        auto skRectPtr = std::make_unique<SkRect>();
        skRectPtr->setXYWH(0, 0, params.srcRect.width(), params.srcRect.height());
        RSPropertiesPainter::DrawFilter(property, canvas, filter, skRectPtr, canvas.GetSurface());
    }
}

void RSRenderEngine::RSSurfaceNodeCommonPostProcess(
    RSSurfaceRenderNode& node,
    RSPaintFilterCanvas& canvas,
    BufferDrawParam& params)
{
    const auto& property = node.GetRenderProperties();

    // draw preprocess filter (should execute this filter after drawing buffer/image).
    auto filter = std::static_pointer_cast<RSSkiaFilter>(property.GetFilter());
    if (filter != nullptr) {
        auto skRectPtr = std::make_unique<SkRect>();
        skRectPtr->setXYWH(0, 0, params.srcRect.width(), params.srcRect.height());
        RSPropertiesPainter::DrawFilter(property, canvas, filter, skRectPtr, canvas.GetSurface());
    }
}

void RSRenderEngine::DrawSurfaceNodeWithParams(
    RSPaintFilterCanvas& canvas,
    RSSurfaceRenderNode& node,
    BufferDrawParam& params,
    PreProcessFunc preProcess,
    PostProcessFunc postProcess)
{
    if (!params.useCPU) {
#ifdef RS_ENABLE_EGLIMAGE
        const auto& consumerSurface = node.GetConsumer();
        auto regUnMapEglImageFunc = [this](int32_t bufferId) {
            eglImageManager_->UnMapEglImageFromSurfaceBuffer(bufferId);
        };
        if (consumerSurface == nullptr ||
            (consumerSurface->RegisterDeleteBufferListener(regUnMapEglImageFunc) != GSERROR_OK)) {
            RS_LOGE("RSRenderEngine::DrawSurfaceNode: failed to register UnMapEglImage callback.");
        }
#endif // #ifdef RS_ENABLE_EGLIMAGE
    }

    auto nodePreProcessFunc = [&preProcess, &node](RSPaintFilterCanvas& canvas, BufferDrawParam& params) {
        // call the preprocess func passed in first.
        if (preProcess != nullptr) {
            preProcess(canvas, params);
        }

        // call RSSurfaceNode's common preprocess func.
        RSRenderEngine::RSSurfaceNodeCommonPreProcess(node, canvas, params);
    };

    auto nodePostProcessFunc = [&postProcess, &node](RSPaintFilterCanvas& canvas, BufferDrawParam& params) {
        // call the postProcess func passed in first.
        if (postProcess != nullptr) {
            postProcess(canvas, params);
        }

        // call RSSurfaceNode's common postProcess func.
        RSRenderEngine::RSSurfaceNodeCommonPostProcess(node, canvas, params);
    };

    // draw shadow(should before canvas.clipRect in DrawWithParams()).
    const auto& property = node.GetRenderProperties();
    RSPropertiesPainter::DrawShadow(property, canvas, &params.clipRRect);

    DrawWithParams(canvas, params, nodePreProcessFunc, nodePostProcessFunc);
}

void RSRenderEngine::DrawSurfaceNode(
    RSPaintFilterCanvas& canvas,
    RSSurfaceRenderNode& node,
    float mirrorAdaptiveCoefficient,
    bool forceCPU)
{
    // prepare BufferDrawParam
    auto params = RSBaseRenderUtil::CreateBufferDrawParam(node, false); // in display's coordinate.
    const float adaptiveDstWidth = params.dstRect.width() * mirrorAdaptiveCoefficient;
    const float adaptiveDstHeight = params.dstRect.height() * mirrorAdaptiveCoefficient;
    params.dstRect.setWH(adaptiveDstWidth, adaptiveDstHeight);
    params.useCPU = forceCPU;

    DrawSurfaceNodeWithParams(canvas, node, params);
}

void RSRenderEngine::ShrinkCachesIfNeeded()
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

void RSRenderEngine::DrawDisplayNode(
    RSPaintFilterCanvas& canvas,
    RSDisplayRenderNode& node,
    bool forceCPU)
{
    // [PLANNING]: In uniRender mode, maybe need to handle displayNode.
    // it is unexpected in now suitation.
}
} // namespace Rosen
} // namespace OHOS
