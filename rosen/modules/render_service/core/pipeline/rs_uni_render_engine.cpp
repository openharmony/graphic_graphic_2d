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

#include "common/rs_singleton.h"
#include "info_collection/rs_layer_compose_collection.h"
#include "luminance/rs_luminance_control.h"
#include "pipeline/rs_uni_render_engine.h"
#include "pipeline/rs_uni_render_util.h"
#include "pipeline/round_corner_display/rs_round_corner_display.h"
#ifdef USE_VIDEO_PROCESSING_ENGINE
#include "metadata_helper.h"
#endif

#include "drawable/rs_display_render_node_drawable.h"
#include "drawable/rs_surface_render_node_drawable.h"

namespace OHOS {
namespace Rosen {
namespace {
const std::string RCD_TOP_LAYER_NAME = "RCDTopSurfaceNode";
const std::string RCD_BOTTOM_LAYER_NAME = "RCDBottomSurfaceNode";
const float REDRAW_DFX_ALPHA = 0.4f; // redraw dfx drawrect alpha
}
void RSUniRenderEngine::DrawSurfaceNodeWithParams(RSPaintFilterCanvas& canvas, RSSurfaceRenderNode& node,
    BufferDrawParam& params, PreProcessFunc preProcess, PostProcessFunc postProcess)
{
    RS_LOGE("RSUniRenderEngine::DrawSurfaceNodeWithParams is not support");
}

void RSUniRenderEngine::DrawSurfaceNodeWithParams(RSPaintFilterCanvas& canvas,
    DrawableV2::RSSurfaceRenderNodeDrawable& surfaceDrawable, BufferDrawParam& params, PreProcessFunc preProcess,
    PostProcessFunc postProcess)
{
    canvas.Save();
    canvas.ConcatMatrix(params.matrix);
    if (!params.useCPU) {
        RegisterDeleteBufferListener(surfaceDrawable.GetConsumerOnDraw());
        DrawImage(canvas, params);
    } else {
        DrawBuffer(canvas, params);
    }
    canvas.Restore();
}

#ifdef USE_VIDEO_PROCESSING_ENGINE
GraphicColorGamut RSUniRenderEngine::ComputeTargetColorGamut(const std::vector<LayerInfoPtr>& layers)
{
    using namespace HDI::Display::Graphic::Common::V1_0;
    GraphicColorGamut colorGamut = GRAPHIC_COLOR_GAMUT_SRGB;
    for (auto& layer : layers) {
        if (layer == nullptr) {
            continue;
        }
        auto buffer = layer->GetBuffer();
        if (buffer == nullptr) {
            RS_LOGW("RSUniRenderEngine::ComputeTargetColorGamut The buffer of layer is nullptr");
            continue;
        }

        CM_ColorSpaceInfo colorSpaceInfo;
        if (MetadataHelper::GetColorSpaceInfo(buffer, colorSpaceInfo) != GSERROR_OK) {
            RS_LOGD("RSUniRenderEngine::ComputeTargetColorGamut Get color space failed");
            continue;
        }

        if (colorSpaceInfo.primaries != COLORPRIMARIES_SRGB) {
            colorGamut = GRAPHIC_COLOR_GAMUT_DISPLAY_P3;
            break;
        }
    }

    return colorGamut;
}
#endif

void RSUniRenderEngine::DrawLayers(RSPaintFilterCanvas& canvas, const std::vector<LayerInfoPtr>& layers, bool forceCPU,
    const ScreenInfo& screenInfo)
{
    std::map<std::string, bool> rcdLayersEnableMap = {{RCD_TOP_LAYER_NAME, false}, {RCD_BOTTOM_LAYER_NAME, false}};
#ifdef USE_VIDEO_PROCESSING_ENGINE
    GraphicColorGamut colorGamut = ComputeTargetColorGamut(layers);
#endif
    for (const auto& layer : layers) {
        if (layer == nullptr) {
            continue;
        }
        if (layer->GetCompositionType() == GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE ||
            layer->GetCompositionType() == GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE_CLEAR) {
            continue;
        }
        auto layerSurface = layer->GetSurface();
        if (layerSurface != nullptr) {
            if (rcdLayersEnableMap.count(layerSurface->GetName()) > 0) {
                rcdLayersEnableMap[layerSurface->GetName()] = true;
                continue;
            }
        } else {
            RS_LOGE("RSUniRenderEngine::DrawLayers layerSurface is nullptr");
            continue;
        }
        Drawing::AutoCanvasRestore acr(canvas, true);
        auto dstRect = layer->GetLayerSize();
        Drawing::Rect clipRect = Drawing::Rect(static_cast<float>(dstRect.x), static_cast<float>(dstRect.y),
            static_cast<float>(dstRect.w) + static_cast<float>(dstRect.x),
            static_cast<float>(dstRect.h) + static_cast<float>(dstRect.y));
        canvas.ClipRect(clipRect, Drawing::ClipOp::INTERSECT, false);
        // prepare BufferDrawParam
        auto params = RSUniRenderUtil::CreateLayerBufferDrawParam(layer, forceCPU);
        params.matrix.PostScale(screenInfo.GetRogWidthRatio(), screenInfo.GetRogHeightRatio());
        params.screenId = screenInfo.id;
#ifdef USE_VIDEO_PROCESSING_ENGINE
        params.targetColorGamut = colorGamut;
        auto screenManager = CreateOrGetScreenManager();
        if (screenManager != nullptr) {
            params.sdrNits = layer->GetSdrNit();
            params.tmoNits = layer->GetDisplayNit();
            params.displayNits = params.tmoNits / std::pow(layer->GetBrightnessRatio(), 2.2f); // gamma 2.2
        }
        if (!CheckIsHdrSurfaceBuffer(layer->GetBuffer())) {
            params.brightnessRatio = layer->GetBrightnessRatio();
        } else {
            params.isHdrRedraw = true;
        }
#endif
        DrawHdiLayerWithParams(canvas, layer, params);
        // Dfx for redraw region
        if (RSSystemProperties::GetHwcRegionDfxEnabled()) {
            RectI dst(dstRect.x, dstRect.y, dstRect.w, dstRect.h);
            RSUniRenderUtil::DrawRectForDfx(canvas, dst, Drawing::Color::COLOR_YELLOW, REDRAW_DFX_ALPHA,
                layerSurface->GetName());
        }
    }

    LayerComposeCollection::GetInstance().UpdateRedrawFrameNumberForDFX();

    if (rcdLayersEnableMap[RCD_TOP_LAYER_NAME] && RSSingleton<RoundCornerDisplay>::GetInstance().GetRcdEnable()) {
        RSSingleton<RoundCornerDisplay>::GetInstance().DrawTopRoundCorner(&canvas);
    }

    if (rcdLayersEnableMap[RCD_BOTTOM_LAYER_NAME] && RSSingleton<RoundCornerDisplay>::GetInstance().GetRcdEnable()) {
        RSSingleton<RoundCornerDisplay>::GetInstance().DrawBottomRoundCorner(&canvas);
    }
}

void RSUniRenderEngine::DrawHdiLayerWithParams(RSPaintFilterCanvas& canvas, const LayerInfoPtr& layer,
    BufferDrawParam& params)
{
    canvas.ConcatMatrix(params.matrix);
    if (!params.useCPU) {
        RegisterDeleteBufferListener(layer->GetSurface(), true);
        DrawImage(canvas, params);
    } else {
        DrawBuffer(canvas, params);
    }
}

void RSUniRenderEngine::DrawUIFirstCacheWithParams(RSPaintFilterCanvas& canvas, BufferDrawParam& params)
{
    if (!params.useCPU) {
        DrawImage(canvas, params);
    } else {
        DrawBuffer(canvas, params);
    }
}
} // namespace Rosen
} // namespace OHOS
