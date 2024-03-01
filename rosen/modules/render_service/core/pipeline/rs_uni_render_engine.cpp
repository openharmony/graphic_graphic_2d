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

#include "pipeline/rs_uni_render_engine.h"
#include "pipeline/rs_uni_render_util.h"

namespace OHOS {
namespace Rosen {
void RSUniRenderEngine::DrawSurfaceNodeWithParams(RSPaintFilterCanvas& canvas, RSSurfaceRenderNode& node,
    BufferDrawParam& params, PreProcessFunc preProcess, PostProcessFunc postProcess)
{
    canvas.Save();
    canvas.ConcatMatrix(params.matrix);
    if (!params.useCPU) {
        RegisterDeleteBufferListener(node.GetConsumer());
        RegisterDeleteBufferListener(node);
        DrawImage(canvas, params);
    } else {
        DrawBuffer(canvas, params);
    }
    canvas.Restore();
}

void RSUniRenderEngine::DrawLayers(RSPaintFilterCanvas& canvas, const std::vector<LayerInfoPtr>& layers, bool forceCPU,
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
        auto saveCount = canvas.Save();
        auto dstRect = layer->GetLayerSize();
        Drawing::Rect clipRect = Drawing::Rect(static_cast<float>(dstRect.x), static_cast<float>(dstRect.y),
            static_cast<float>(dstRect.w) + static_cast<float>(dstRect.x),
            static_cast<float>(dstRect.h) + static_cast<float>(dstRect.y));
        canvas.ClipRect(clipRect, Drawing::ClipOp::INTERSECT, false);
        // prepare BufferDrawParam
        auto params = RSUniRenderUtil::CreateLayerBufferDrawParam(layer, forceCPU);
        DrawHdiLayerWithParams(canvas, layer, params);
        canvas.Restore();
        canvas.RestoreToCount(saveCount);
    }
}

void RSUniRenderEngine::DrawHdiLayerWithParams(RSPaintFilterCanvas& canvas, const LayerInfoPtr& layer,
    BufferDrawParam& params)
{
    canvas.Save();
    canvas.ConcatMatrix(params.matrix);
    if (!params.useCPU) {
        RegisterDeleteBufferListener(layer->GetSurface(), true);
        DrawImage(canvas, params);
    } else {
        DrawBuffer(canvas, params);
    }
    canvas.Restore();
}
} // namespace Rosen
} // namespace OHOS
