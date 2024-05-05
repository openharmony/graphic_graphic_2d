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

#include "pipeline/rs_render_engine.h"
#include "pipeline/rs_divided_render_util.h"
#include "string_utils.h"
#include "render/rs_drawing_filter.h"
#include "render/rs_skia_filter.h"
#include "rs_trace.h"
#include "platform/common/rs_log.h"

#include "image/image.h"

namespace OHOS {
namespace Rosen {
void RSRenderEngine::DrawSurfaceNodeWithParams(RSPaintFilterCanvas& canvas, RSSurfaceRenderNode& node,
    BufferDrawParam& params, PreProcessFunc preProcess, PostProcessFunc postProcess)
{
    if (!params.useCPU) {
        RegisterDeleteBufferListener(node.GetConsumer());
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
    RSPropertiesPainter::DrawOutline(property, canvas);

    DrawWithParams(canvas, params, nodePreProcessFunc, nodePostProcessFunc);
}

void RSRenderEngine::DrawLayers(RSPaintFilterCanvas& canvas, const std::vector<LayerInfoPtr>& layers, bool forceCPU,
    float mirrorAdaptiveCoefficient)
{
    for (const auto& layer : layers) {
        if (layer == nullptr) {
            continue;
        }
        if (layer->GetCompositionType() == GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE ||
            layer->GetCompositionType() == GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE_CLEAR) {
            continue;
        }
        auto nodePtr = static_cast<RSBaseRenderNode*>(layer->GetLayerAdditionalInfo());
        if (nodePtr == nullptr) {
            RS_LOGE("RSRenderEngine::DrawLayers: node is nullptr!");
            continue;
        }

        auto saveCount = canvas.GetSaveCount();
        if (nodePtr->IsInstanceOf<RSSurfaceRenderNode>()) {
            RSSurfaceRenderNode& node = *(static_cast<RSSurfaceRenderNode*>(nodePtr));
            if (layer->GetCompositionType() == GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT_CLEAR ||
                layer->GetCompositionType() == GraphicCompositionType::GRAPHIC_COMPOSITION_TUNNEL) {
                ClipHoleForLayer(canvas, node);
                canvas.RestoreToCount(saveCount);
                continue;
            }
            RS_LOGD("RSRenderEngine::DrawLayers dstRect[%{public}d %{public}d %{public}d %{public}d]",
                layer->GetLayerSize().x, layer->GetLayerSize().y, layer->GetLayerSize().w, layer->GetLayerSize().h);
            const std::vector<GraphicIRect>& dirtyRegions = layer->GetDirtyRegions();
            for (auto iter = dirtyRegions.begin(); iter != dirtyRegions.end(); iter++) {
                RS_LOGD("RSRenderEngine::DrawLayers SrcRect[%{public}d %{public}d %{public}d %{public}d]",
                    iter->x, iter->y, iter->w, iter->h);
            }
            DrawSurfaceNode(canvas, node, mirrorAdaptiveCoefficient, forceCPU);
        } else {
            // Probably never reach this branch.
            RS_LOGE("RSRenderEngine::DrawLayers: unexpected node type!");
            continue;
        }
        canvas.RestoreToCount(saveCount);
    }
}

void RSRenderEngine::DrawWithParams(RSPaintFilterCanvas& canvas, BufferDrawParam& params,
    PreProcessFunc preProcess, PostProcessFunc postProcess)
{
    if (params.setColorFilter) {
        SetColorFilterModeToPaint(params.paint);
    }

    canvas.Save();

    RSBaseRenderUtil::SetPropertiesForCanvas(canvas, params);

    if (preProcess != nullptr) {
        preProcess(canvas, params);
    }

    if (params.useCPU) {
        RSBaseRenderEngine::DrawBuffer(canvas, params);
    } else {
        RSBaseRenderEngine::DrawImage(canvas, params);
    }

    if (postProcess != nullptr) {
        postProcess(canvas, params);
    }

    canvas.Restore();
}

void RSRenderEngine::RSSurfaceNodeCommonPreProcess(RSSurfaceRenderNode& node, RSPaintFilterCanvas& canvas,
    BufferDrawParam& params)
{
    const auto& property = node.GetRenderProperties();

    // draw mask.
    RectF maskBounds(0, 0, params.dstRect.GetWidth(), params.dstRect.GetHeight());
    RSPropertiesPainter::DrawMask(
        node.GetRenderProperties(), canvas, RSPropertiesPainter::Rect2DrawingRect(maskBounds));

    // draw background filter (should execute this filter before drawing buffer/image).
    RSPropertiesPainter::DrawFilter(property, canvas, FilterType::BACKGROUND_FILTER,
        Drawing::Rect(0, 0, params.srcRect.GetWidth(), params.srcRect.GetHeight()));
}

void RSRenderEngine::RSSurfaceNodeCommonPostProcess(RSSurfaceRenderNode& node, RSPaintFilterCanvas& canvas,
    BufferDrawParam& params)
{
    const auto& property = node.GetRenderProperties();

    // draw preprocess filter (should execute this filter after drawing buffer/image).
    RSPropertiesPainter::DrawFilter(property, canvas, FilterType::FOREGROUND_FILTER,
        Drawing::Rect(0, 0, params.srcRect.GetWidth(), params.srcRect.GetHeight()));
}

void RSRenderEngine::DrawSurfaceNode(RSPaintFilterCanvas& canvas, RSSurfaceRenderNode& node,
    float mirrorAdaptiveCoefficient, bool forceCPU)
{
    // prepare BufferDrawParam
    auto params = RSDividedRenderUtil::CreateBufferDrawParam(node, false, false, forceCPU); // in display's coordinate.
    const float adaptiveDstWidth = params.dstRect.GetWidth() * mirrorAdaptiveCoefficient;
    const float adaptiveDstHeight = params.dstRect.GetHeight() * mirrorAdaptiveCoefficient;
    params.dstRect = Drawing::Rect(0, 0, adaptiveDstWidth, adaptiveDstHeight);
    const float translateX = params.matrix.Get(Drawing::Matrix::Index::TRANS_X) * mirrorAdaptiveCoefficient;
    const float translateY = params.matrix.Get(Drawing::Matrix::Index::TRANS_Y) * mirrorAdaptiveCoefficient;
    params.matrix.Set(Drawing::Matrix::Index::TRANS_X, translateX);
    params.matrix.Set(Drawing::Matrix::Index::TRANS_Y, translateY);
    const auto& clipRect = params.clipRect;
    auto clipLeft = clipRect.GetLeft() * mirrorAdaptiveCoefficient;
    auto clipTop = clipRect.GetTop() * mirrorAdaptiveCoefficient;
    params.clipRect = Drawing::Rect(
        clipLeft, clipTop, clipLeft + clipRect.GetWidth() * mirrorAdaptiveCoefficient,
        clipTop + clipRect.GetHeight() * mirrorAdaptiveCoefficient);

    DrawSurfaceNodeWithParams(canvas, node, params, nullptr, nullptr);
}

void RSRenderEngine::ClipHoleForLayer(RSPaintFilterCanvas& canvas, RSSurfaceRenderNode& node)
{
    BufferDrawParam params = RSDividedRenderUtil::CreateBufferDrawParam(node, false, true);

    std::string traceInfo;
    AppendFormat(traceInfo, "Node name:%s ClipHole[%d %d %d %d]", node.GetName().c_str(),
        params.clipRect.GetLeft(), params.clipRect.GetTop(), params.clipRect.GetWidth(), params.clipRect.GetHeight());
    RS_LOGD("RSRenderEngine::Redraw layer composition ClipHoleForLayer, %{public}s.", traceInfo.c_str());
    RS_TRACE_NAME(traceInfo);

    canvas.Save();
    canvas.ClipRect(params.clipRect, Drawing::ClipOp::INTERSECT, false);
    canvas.Clear(Drawing::Color::COLOR_TRANSPARENT);
    canvas.Restore();
    return;
}

void RSRenderEngine::SetColorFilterModeToPaint(Drawing::Brush& paint)
{
    // for test automation
    if (colorFilterMode_ != ColorFilterMode::COLOR_FILTER_END) {
        RS_LOGD("RSRenderEngine::SetColorFilterModeToPaint mode:%{public}d", static_cast<int32_t>(colorFilterMode_));
    }
    RSBaseRenderUtil::SetColorFilterModeToPaint(colorFilterMode_, paint);
}
} // namespace Rosen
} // namespace OHOS
