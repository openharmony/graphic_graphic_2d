/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "drawable/rs_effect_render_node_drawable.h"

#include "params/rs_effect_render_params.h"
#include "pipeline/rs_uni_render_thread.h"
#include "platform/common/rs_log.h"

namespace OHOS::Rosen::DrawableV2 {
RSEffectRenderNodeDrawable::Registrar RSEffectRenderNodeDrawable::instance_;

RSEffectRenderNodeDrawable::RSEffectRenderNodeDrawable(std::shared_ptr<const RSRenderNode>&& node)
    : RSRenderNodeDrawable(std::move(node))
{}

RSRenderNodeDrawable::Ptr RSEffectRenderNodeDrawable::OnGenerate(std::shared_ptr<const RSRenderNode> node)
{
    return new RSEffectRenderNodeDrawable(std::move(node));
}

void RSEffectRenderNodeDrawable::OnDraw(Drawing::Canvas& canvas)
{
    if (!ShouldPaint()) {
        return;
    }

    RS_LOGD("RSEffectRenderNodeDrawable::OnDraw node: %{public}" PRIu64, nodeId_);
    auto effectParams = static_cast<RSEffectRenderParams*>(GetRenderParams().get());
    if (!effectParams) {
        RS_LOGE("RSSurfaceRenderNodeDrawable::OnDraw params is nullptr");
        return;
    }
    auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(&canvas);
    RSAutoCanvasRestore acr(paintFilterCanvas, RSPaintFilterCanvas::SaveType::kAll);

    effectParams->ApplyAlphaAndMatrixToCanvas(*paintFilterCanvas);
    auto uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams().get();
    if ((!uniParam || uniParam->IsOpDropped()) && QuickReject(canvas, effectParams->GetLocalDrawRect())) {
        return;
    }
    Drawing::Rect bounds = GetRenderParams() ? GetRenderParams()->GetFrameRect() : Drawing::Rect(0, 0, 0, 0);

    if (drawCmdIndex_.backgroundFilterIndex_ == -1 || !RSSystemProperties::GetEffectMergeEnabled() ||
        !effectParams->ChildHasVisibleEffect()) {
        // case 1: no blur or no need to blur, do nothing
    } else if (drawCmdIndex_.backgroundImageIndex_ == -1 || effectParams->GetCacheValid()) {
        // case 2: dynamic blur, blur the underlay content
        // case 3a: static blur with valid cache, reuse cache
        RSRenderNodeDrawableAdapter::DrawImpl(canvas, bounds, drawCmdIndex_.backgroundFilterIndex_);
    } else {
        // case 3b: static blur without valid cache, draw background image and blur
        auto surface = canvas.GetSurface();
        if (!surface) {
            ROSEN_LOGE("RSPropertiesPainter::DrawBackgroundImageAsEffect surface null");
            return;
        }
        // extract clip bounds
        canvas.ClipIRect(Drawing::RectI(0, 0, bounds.GetWidth(), bounds.GetHeight()));
        auto currentRect = canvas.GetDeviceClipBounds();
        // create offscreen surface
        auto offscreenSurface = surface->MakeSurface(currentRect.GetWidth(), currentRect.GetHeight());
        auto offscreenCanvas = std::make_unique<RSPaintFilterCanvas>(offscreenSurface.get());
        // copy current matrix to offscreen canvas, while setting TRANS_X and TRANS_Y to 0
        auto currentMatrix = canvas.GetTotalMatrix();
        currentMatrix.Set(Drawing::Matrix::TRANS_X, 0);
        currentMatrix.Set(Drawing::Matrix::TRANS_Y, 0);
        offscreenCanvas->SetMatrix(currentMatrix);
        // draw background image and blur
        RSRenderNodeDrawableAdapter::DrawImpl(*offscreenCanvas, bounds, drawCmdIndex_.backgroundImageIndex_);
        RSRenderNodeDrawableAdapter::DrawImpl(*offscreenCanvas, bounds, drawCmdIndex_.backgroundFilterIndex_);
        // copy effect data from offscreen canvas to current canvas, aligned with current rect
        auto effectData = offscreenCanvas->GetEffectData();
        effectData->cachedRect_.Offset(currentRect.GetLeft(), currentRect.GetTop());
        paintFilterCanvas->SetEffectData(effectData);
    }

    RSRenderNodeDrawableAdapter::DrawImpl(canvas, bounds, drawCmdIndex_.childrenIndex_);
}

void RSEffectRenderNodeDrawable::OnCapture(Drawing::Canvas& canvas)
{
    RSEffectRenderNodeDrawable::OnDraw(canvas);
}
} // namespace OHOS::Rosen::DrawableV2
