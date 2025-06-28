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
#include "display_engine/rs_luminance_control.h"
#include "feature/hdr/rs_hdr_util.h"
#include "info_collection/rs_layer_compose_collection.h"
#include "rs_uni_render_engine.h"
#include "rs_uni_render_util.h"
#ifdef RS_ENABLE_GPU
#include "feature/round_corner_display/rs_round_corner_display_manager.h"
#endif
#ifdef USE_VIDEO_PROCESSING_ENGINE
#include "metadata_helper.h"
#endif

#include "drawable/rs_display_render_node_drawable.h"
#include "drawable/rs_surface_render_node_drawable.h"

namespace OHOS {
namespace Rosen {

using RSRcdManager = RSSingleton<RoundCornerDisplayManager>;
namespace {
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
void RSUniRenderEngine::DrawLayers(RSPaintFilterCanvas& canvas, const std::vector<LayerInfoPtr>& layers, bool forceCPU,
    const ScreenInfo& screenInfo, GraphicColorGamut colorGamut)
#else
void RSUniRenderEngine::DrawLayers(RSPaintFilterCanvas& canvas, const std::vector<LayerInfoPtr>& layers, bool forceCPU,
    const ScreenInfo& screenInfo)
#endif
{
    for (const auto& layer : layers) {
        if (layer == nullptr) {
            continue;
        }
        if (layer->GetCompositionType() == GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE ||
            layer->GetCompositionType() == GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE_CLEAR ||
            layer->GetCompositionType() == GraphicCompositionType::GRAPHIC_COMPOSITION_SOLID_COLOR) {
            continue;
        }
        GraphicLayerColor layerBlackColor = {
            .r = 0,
            .g = 0,
            .b = 0,
            .a = 0
        };
        auto layerSurface = layer->GetSurface();
        if (layerSurface == nullptr) {
            const auto& layerColor = layer->GetLayerColor();
            if (layerColor.a != layerBlackColor.a || layerColor.r != layerBlackColor.r ||
                layerColor.g != layerBlackColor.g || layerColor.b != layerBlackColor.b) {
                Drawing::AutoCanvasRestore acr(canvas, true);
                const auto& dstRect = layer->GetLayerSize();
                auto color = Drawing::Color::ColorQuadSetARGB(layerColor.a, layerColor.r, layerColor.g, layerColor.b);
                Drawing::Rect clipRect = Drawing::Rect(static_cast<float>(dstRect.x), static_cast<float>(dstRect.y),
                    static_cast<float>(dstRect.w) + static_cast<float>(dstRect.x),
                    static_cast<float>(dstRect.h) + static_cast<float>(dstRect.y));
                canvas.ClipRect(clipRect, Drawing::ClipOp::INTERSECT, false);
                canvas.DrawColor(color);
            }
            continue;
        } else if (RSRcdManager::GetInstance().CheckLayerIsRCD(layerSurface->GetName())) {
            continue;
        }
        Drawing::AutoCanvasRestore acr(canvas, true);
        DrawLayerPreProcess(canvas, layer);
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
            // color temperature
            params.layerLinearMatrix = layer->GetLayerLinearMatrix();
        }
        if (RSHdrUtil::CheckIsHdrSurfaceBuffer(layer->GetBuffer()) == HdrStatus::NO_HDR) {
            params.brightnessRatio = layer->GetBrightnessRatio();
            if (RSHdrUtil::CheckIsSurfaceBufferWithMetadata(layer->GetBuffer())) {
                params.hasMetadata = true;
            }
        } else {
            params.isHdrRedraw = true;
        }
#endif
        RS_TRACE_NAME_FMT("DrawLayerWithParams, surface name: %s", layerSurface->GetName().c_str());
        DrawHdiLayerWithParams(canvas, layer, params);
        // Dfx for redraw region
        auto dstRect = layer->GetLayerSize();
        if (RSSystemProperties::GetHwcRegionDfxEnabled()) {
            RectI dst(dstRect.x, dstRect.y, dstRect.w, dstRect.h);
            RSUniRenderUtil::DrawRectForDfx(canvas, dst, Drawing::Color::COLOR_YELLOW, REDRAW_DFX_ALPHA,
                layerSurface->GetName());
        }
    }

    LayerComposeCollection::GetInstance().UpdateRedrawFrameNumberForDFX();
}

void RSUniRenderEngine::DrawLayerPreProcess(RSPaintFilterCanvas& canvas, const LayerInfoPtr& layer)
{
    const auto& dstRect = layer->GetLayerSize();
    const auto& drmCornerRadiusInfo = layer->GetCornerRadiusInfoForDRM();
    const auto& layerBackgroundColor = layer->GetBackgroundColor();
    Color backgroundColor = {
        layerBackgroundColor.r,
        layerBackgroundColor.g,
        layerBackgroundColor.b,
        layerBackgroundColor.a
    };
    // clip round rect when drm has radius info
    if (!drmCornerRadiusInfo.empty()) {
        auto rect = RectF();
        rect = {drmCornerRadiusInfo[0], drmCornerRadiusInfo[1], // 0 and 1 represent rect left and top
            drmCornerRadiusInfo[2], drmCornerRadiusInfo[3]}; // 2 and 3 represent rect width and height
        Vector4f radiusVector {drmCornerRadiusInfo[4], drmCornerRadiusInfo[5], // 4 and 5 represent corner radius
            drmCornerRadiusInfo[6], drmCornerRadiusInfo[7]}; // 6 and 7 represent corner radius
        RRect rrect = RRect(rect, radiusVector);
        canvas.ClipRoundRect(RSPropertiesPainter::RRect2DrawingRRect(rrect), Drawing::ClipOp::INTERSECT, true);
        if (backgroundColor != RgbPalette::Transparent()) {
            canvas.DrawColor(backgroundColor.AsArgbInt());
        }
        return;
    }
    // draw background color for DRM buffer and then draw layer size image when surface has scaling mode
    if (layer->GetBuffer() && (layer->GetBuffer()->GetUsage() & BUFFER_USAGE_PROTECTED) &&
        backgroundColor != RgbPalette::Transparent()) {
        const auto& boundsRect = layer->GetBoundSize();
        const auto& layerMatrix = layer->GetMatrix();
        auto skMatrix = Drawing::Matrix();
        skMatrix.SetMatrix(layerMatrix.scaleX, layerMatrix.skewX, layerMatrix.transX, layerMatrix.skewY,
            layerMatrix.scaleY, layerMatrix.transY, layerMatrix.pers0, layerMatrix.pers1, layerMatrix.pers2);
        Drawing::AutoCanvasRestore acr(canvas, true);
        canvas.ConcatMatrix(skMatrix);
        Drawing::Rect drawRect = Drawing::Rect(0.f, 0.f,
            static_cast<float>(boundsRect.w), static_cast<float>(boundsRect.h));
        Drawing::Brush rectBrush;
        rectBrush.SetColor(backgroundColor.AsArgbInt());
        canvas.AttachBrush(rectBrush);
        canvas.DrawRect(drawRect);
        canvas.DetachBrush();
    }
    Drawing::Rect clipRect = Drawing::Rect(static_cast<float>(dstRect.x), static_cast<float>(dstRect.y),
        static_cast<float>(dstRect.w) + static_cast<float>(dstRect.x),
        static_cast<float>(dstRect.h) + static_cast<float>(dstRect.y));
    canvas.ClipRect(clipRect, Drawing::ClipOp::INTERSECT, false);
}

void RSUniRenderEngine::DrawHdiLayerWithParams(RSPaintFilterCanvas& canvas, const LayerInfoPtr& layer,
    BufferDrawParam& params)
{
    RS_LOGD_IF(DEBUG_COMPOSER, "RSUniRenderEngine::DrawHdiLayerWithParams: matrix=[%{public}.2f, %{public}.2f, "
        "%{public}.2f, %{public}.2f, %{public}.2f, %{public}.2f, %{public}.2f, %{public}.2f, %{public}.2f]",
        params.matrix.Get(Drawing::Matrix::SCALE_X), params.matrix.Get(Drawing::Matrix::SKEW_X),
        params.matrix.Get(Drawing::Matrix::TRANS_X), params.matrix.Get(Drawing::Matrix::SKEW_Y),
        params.matrix.Get(Drawing::Matrix::SCALE_Y), params.matrix.Get(Drawing::Matrix::TRANS_Y),
        params.matrix.Get(Drawing::Matrix::PERSP_0), params.matrix.Get(Drawing::Matrix::PERSP_1),
        params.matrix.Get(Drawing::Matrix::PERSP_2));
    canvas.ConcatMatrix(params.matrix);
    if (!params.useCPU) {
        RegisterDeleteBufferListener(layer->GetSurface(), !RSSystemProperties::GetVKImageUseEnabled());
        DrawImage(canvas, params);
    } else {
        DrawBuffer(canvas, params);
    }
}

void RSUniRenderEngine::DrawHDRCacheWithParams(RSPaintFilterCanvas& canvas, BufferDrawParam& params)
{
    if (!params.useCPU) {
        DrawImage(canvas, params);
    } else {
        DrawBuffer(canvas, params);
    }
}
} // namespace Rosen
} // namespace OHOS
