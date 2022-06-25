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
#include "rs_divided_render_util.h"
#include "rs_trace.h"
#include "string_utils.h"

namespace OHOS {
namespace Rosen {
RSRenderEngine::RSRenderEngine()
{
#ifdef RS_ENABLE_GL
    renderContext_ = std::make_shared<RenderContext>();
    renderContext_->InitializeEglContext();
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
        const auto bufferFormat = buffer->GetFormat();
        if (bufferFormat == PIXEL_FMT_YCRCB_420_SP || bufferFormat == PIXEL_FMT_YCBCR_420_SP) {
            forceCPU = true;
            break;
        }

        ColorGamut srcGamut = static_cast<ColorGamut>(buffer->GetSurfaceBufferColorGamut());
        ColorGamut dstGamut = ColorGamut::COLOR_GAMUT_SRGB;
        if (srcGamut != dstGamut) {
            forceCPU = true;
            break;
        }
    }

    return forceCPU;
}

std::unique_ptr<RSRenderFrame> RSRenderEngine::RequestFrame(
    const sptr<Surface>& targetSurfce,
    const BufferRequestConfig& config,
    bool forceCPU)
{
    RS_TRACE_NAME("RSRenderEngine::RequestFrame");
    if (targetSurfce == nullptr) {
        RS_LOGE("RSRenderEngine::RequestFrame: surface is null!");
        return nullptr;
    }

    std::shared_ptr<RSSurfaceOhos> rsSurface;
    auto bufferUsage = config.usage;
#if (defined RS_ENABLE_GL) && (defined RS_ENABLE_EGLIMAGE)
    if (forceCPU) {
        rsSurface = std::make_shared<RSSurfaceOhosRaster>(targetSurfce);
        bufferUsage |= HBM_USE_CPU_WRITE;
    } else {
        rsSurface = std::make_shared<RSSurfaceOhosGl>(targetSurfce);
    }
#else
    rsSurface = std::make_shared<RSSurfaceOhosRaster>(targetSurfce);
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

void RSRenderEngine::DrawLayers(
    RSPaintFilterCanvas& canvas,
    const std::vector<LayerInfoPtr>& layers,
    const ScreenInfo& screenInfo,
    bool forceCPU)
{
    for (const auto& layer : layers) {
        if (layer == nullptr) {
            continue;
        }
        if (layer->GetCompositionType() == CompositionType::COMPOSITION_DEVICE) {
            continue;
        }

        auto nodePtr = static_cast<RSBaseRenderNode*>(layer->GetLayerAdditionalInfo());
        if (nodePtr == nullptr) {
            RS_LOGE("RSRenderEngine::DrawLayers: node is nullptr!");
            continue;
        }
        const auto& clipRect = layer->GetLayerSize();
        auto saveCount = canvas.getSaveCount();
        if (nodePtr->IsInstanceOf<RSSurfaceRenderNode>()) {
            RSSurfaceRenderNode& node = *(static_cast<RSSurfaceRenderNode*>(nodePtr));
            DrawSurfaceNode(canvas, node, screenInfo, clipRect, forceCPU);
        } else if (nodePtr->IsInstanceOf<RSDisplayRenderNode>()) {
            // In uniRender mode, maybe need to handle displayNode.
            RSDisplayRenderNode& node = *(static_cast<RSDisplayRenderNode*>(nodePtr));
            DrawDisplayNode(canvas, node, screenInfo, forceCPU);
        } else {
            // Probably never reach this branch.
            RS_LOGE("RSRenderEngine::DrawLayers: unexpected node type!");
            continue;
        }
        canvas.restoreToCount(saveCount);
    }
}

static void DrawBufferPostProcess(
    RSPaintFilterCanvas& canvas,
    RSSurfaceRenderNode& node,
    BufferDrawParam& params,
    Vector2f& center)
{
    RSDividedRenderUtil::DealAnimation(canvas, node, params, center);
    RectF maskBounds(0, 0, params.dstRect.width(), params.dstRect.height());
    RSPropertiesPainter::DrawMask(
        node.GetRenderProperties(), canvas, RSPropertiesPainter::Rect2SkRect(maskBounds));
}

SkMatrix RSRenderEngine::GetSurfaceNodeGravityMatrix(
    RSSurfaceRenderNode& node, const RectF& targetRect)
{
    SkMatrix gravityMatrix = SkMatrix::I();
    const RSProperties& property = node.GetRenderProperties();
    const auto& buffer = node.GetBuffer();
    if (buffer == nullptr) {
        return gravityMatrix;
    }

    if (!RSPropertiesPainter::GetGravityMatrix(property.GetFrameGravity(), targetRect,
        buffer->GetSurfaceBufferWidth(), buffer->GetSurfaceBufferHeight(), gravityMatrix)) {
        RS_LOGD("RSRenderEngine::GetGravityMatrix did not obtain gravity matrix.");
    }

    return gravityMatrix;
}

void RSRenderEngine::DrawSurfaceNode(
    RSPaintFilterCanvas& canvas,
    RSSurfaceRenderNode& node,
    const ScreenInfo& screenInfo,
    const IRect& clipRect,
    bool forceCPU)
{
    std::string traceInfo;
    AppendFormat(traceInfo, "Node name:%s ClipRect[%d %d %d %d]", node.GetName().c_str(),
        clipRect.x, clipRect.y, clipRect.w, clipRect.h);
    RS_TRACE_NAME(traceInfo.c_str());
    RS_LOGD("RsDebug RSRenderEngine::Redraw layer composition Type:CLIENT, %s.",
        traceInfo.c_str());

    // prepare BufferDrawParam
    BufferDrawParam params = RSDividedRenderUtil::CreateBufferDrawParam(
        node, screenInfo.rotationMatrix, screenInfo.rotation);
    params.targetColorGamut = static_cast<ColorGamut>(screenInfo.colorGamut);
    params.clipRect = SkRect::MakeXYWH(clipRect.x, clipRect.y, clipRect.w, clipRect.h);
    params.matrix = params.matrix.preConcat(GetSurfaceNodeGravityMatrix(node,
        RectF {params.dstRect.x(), params.dstRect.y(), params.dstRect.width(), params.dstRect.height()}));
    if (node.GetRenderProperties().GetFrameGravity() != Gravity::RESIZE) {
        params.dstRect = params.srcRect;
    }

    // prepare PostProcessFunc
    Vector2f center(node.GetDstRect().left_ + node.GetDstRect().width_ * 0.5f,
        node.GetDstRect().top_ + node.GetDstRect().height_ * 0.5f);
    auto drawBufferPostProcessFunc = [this, &node, &center](RSPaintFilterCanvas& canvas,
        BufferDrawParam& params) -> void {
            DrawBufferPostProcess(canvas, node, params, center);
    };

    // draw buffer.
#ifdef RS_ENABLE_EGLIMAGE
    if (forceCPU) {
        RSDividedRenderUtil::DrawBuffer(canvas, params, drawBufferPostProcessFunc);
    } else {
        auto consumerSurface = node.GetConsumer();
        auto regUnMapEglImageFunc = [this](int32_t bufferId) {
            eglImageManager_->UnMapEglImageFromSurfaceBuffer(bufferId);
        };
        if (consumerSurface == nullptr ||
            (consumerSurface->RegisterDeleteBufferListener(regUnMapEglImageFunc) !=GSERROR_OK)) {
            RS_LOGE("RSRenderEngine::DrawSurfaceNode: failed to register UnMapEglImage callback.");
        }

        RSDividedRenderUtil::DrawImage(eglImageManager_, renderContext_->GetGrContext(),
            canvas, params, drawBufferPostProcessFunc);
    }
#else // RS_ENABLE_EGLIMAGE
    RSDividedRenderUtil::DrawBuffer(canvas, params, drawBufferPostProcessFunc);
#endif // RS_ENABLE_EGLIMAGE
}

void RSRenderEngine::ShrinkEGLImageCachesIfNeeded()
{
#ifdef RS_ENABLE_EGLIMAGE
    if (eglImageManager_ != nullptr) {
        eglImageManager_->ShrinkCachesIfNeeded();
    }
#endif // RS_ENABLE_EGLIMAGE
}

void RSRenderEngine::DrawDisplayNode(
    RSPaintFilterCanvas& canvas,
    RSDisplayRenderNode& node,
    const ScreenInfo& screenInfo,
    bool forceCPU)
{
    // [PLANNING]: In uniRender mode, maybe need to handle displayNode.
}
} // namespace Rosen
} // namespace OHOS
