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

#include "platform/common/rs_log.h"
#include "platform/ohos/backend/rs_surface_ohos_gl.h"
#include "platform/ohos/backend/rs_surface_ohos_raster.h"
#include "platform/common/rs_system_properties.h"
#include "property/rs_transition_properties.h"
#include "render/rs_skia_filter.h"
#include "rs_divided_render_util.h"
#include "rs_trace.h"
#include "string_utils.h"

namespace OHOS {
namespace Rosen {
namespace Detail {
constexpr uint32_t MATRIX_SIZE = 20;
static const sk_sp<SkColorFilter>& InvertColorMat()
{
    static const SkScalar colorMatrix[MATRIX_SIZE] = {
        0.402,  -1.174, -0.228, 1.0, 0.0,
        -5.598, -0.174, -0.228, 1.0, 0.0,
        -0.599, -1.175, 0.772,  1.0, 0.0,
        0.0,    0.0,    0.0,    1.0, 0.0
    };
    static auto invertColorMat = SkColorFilters::Matrix(colorMatrix);
    return invertColorMat;
}

static const sk_sp<SkColorFilter>& ProtanomalyMat()
{
    static const SkScalar colorMatrix[MATRIX_SIZE] = {
        0.829,  0.1,    0.016,   0.0, 0.0,
        1.161,  0.32,   -0.111,  0.0, 0.0,
        0.224,  -0.131, 0.979,   0.0, 0.0,
        0.0,    0.0,    0.0,     1.0, 0.0
    };
    static auto protanomalyMat = SkColorFilters::Matrix(colorMatrix);
    return protanomalyMat;
}

static const sk_sp<SkColorFilter>& DeuteranomalyMat()
{
    static const SkScalar colorMatrix[MATRIX_SIZE] = {
        0.583,  -0.072, 0.054,   0.0, 0.0,
        -0.692, 0.881,  0.089,   0.0, 0.0,
        -0.118, -0.02,  1.015,   0.0, 0.0,
        0.0,    0.0,    0.0,     1.0, 0.0
    };
    static auto deuteranomalyMat = SkColorFilters::Matrix(colorMatrix);
    return deuteranomalyMat;
}

static const sk_sp<SkColorFilter>& TritanomalyMat()
{
    static const SkScalar colorMatrix[MATRIX_SIZE] = {
        0.989,  0.074,  -0.39,   0.0, 0.0,
        -0.004, 1.026,  -0.141,  0.0, 0.0,
        -0.093, 0.608,  -2.23,   0.0, 0.0,
        0.0,    0.0,    0.0,     1.0, 0.0
    };
    static auto tritanomalyMat = SkColorFilters::Matrix(colorMatrix);
    return tritanomalyMat;
}
} // namespace Detail

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
        bufferUsage |= HBM_USE_CPU_WRITE;
    }
#else // (defined RS_ENABLE_GL) && (defined RS_ENABLE_EGLIMAGE)
    bufferUsage |= HBM_USE_CPU_WRITE;
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

void RSRenderEngine::SetColorFilterModeToPaint(SkPaint& paint)
{
    ColorFilterMode mode = static_cast<ColorFilterMode>(RSSystemProperties::GetCorrectionMode());
    if (mode <= ColorFilterMode::COLOR_FILTER_END && mode >= ColorFilterMode::INVERT_MODE) {
        colorFilterMode_ = mode;
    }

    switch (colorFilterMode_) {
        case ColorFilterMode::INVERT_MODE:
            paint.setColorFilter(Detail::InvertColorMat());
            break;
        case ColorFilterMode::PROTANOMALY_MODE:
            paint.setColorFilter(Detail::ProtanomalyMat());
            break;
        case ColorFilterMode::DEUTERANOMALY_MODE:
            paint.setColorFilter(Detail::DeuteranomalyMat());
            break;
        case ColorFilterMode::TRITANOMALY_MODE:
            paint.setColorFilter(Detail::TritanomalyMat());
            break;
        case ColorFilterMode::COLOR_FILTER_END:
            paint.setColorFilter(nullptr);
            break;
        default:
            paint.setColorFilter(nullptr);
    }
}

void RSRenderEngine::DrawBuffer(RSPaintFilterCanvas& canvas, BufferDrawParam& params)
{
    SkBitmap bitmap;
    if (!RSBaseRenderUtil::ConvertBufferToBitmap(params.buffer, params.targetColorGamut, bitmap,
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
    SetColorFilterModeToPaint(params.paint);

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

    // deal with animation.
    const auto& transitionProperties = node.GetAnimationManager().GetTransitionProperties();
    RSPropertiesPainter::DrawTransitionProperties(transitionProperties, property, canvas);

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
}
} // namespace Rosen
} // namespace OHOS
