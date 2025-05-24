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

#include "pipeline/render_thread/rs_uni_render_thread.h"
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
#ifdef RS_ENABLE_GPU
    if (RSUniRenderThread::GetCaptureParam().isSoloNodeUiCapture_) {
        RS_LOGD("RSEffectRenderNodeDrawable::OnDraw node %{public}" PRIu64 " isSoloNodeUiCapture, skip", nodeId_);
        return;
    }
    SetDrawSkipType(DrawSkipType::NONE);
    if (!ShouldPaint()) {
        SetDrawSkipType(DrawSkipType::SHOULD_NOT_PAINT);
        return;
    }

    RS_LOGD("RSEffectRenderNodeDrawable::OnDraw node: %{public}" PRIu64, nodeId_);
    auto effectParams = static_cast<RSEffectRenderParams*>(GetRenderParams().get());
    if (!effectParams) {
        SetDrawSkipType(DrawSkipType::RENDER_PARAMS_NULL);
        RS_LOGE("RSSurfaceRenderNodeDrawable::OnDraw params is nullptr");
        return;
    }
    Drawing::GPUResourceTag::SetCurrentNodeId(GetId());
    auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(&canvas);
    RSAutoCanvasRestore acr(paintFilterCanvas, RSPaintFilterCanvas::SaveType::kAll);

    paintFilterCanvas->SetEffectIntersectWithDRM(effectParams->GetEffectIntersectWithDRM());
    paintFilterCanvas->SetDarkColorMode(effectParams->GetDarkColorMode());

    effectParams->ApplyAlphaAndMatrixToCanvas(*paintFilterCanvas);
    auto& uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    SetOcclusionCullingEnabled((!uniParam || uniParam->IsOpDropped()) && GetOpDropped());
    if (IsOcclusionCullingEnabled() && QuickReject(canvas, effectParams->GetLocalDrawRect())) {
        SetDrawSkipType(DrawSkipType::OCCLUSION_SKIP);
        return;
    }
    const Drawing::Rect& bounds = effectParams->GetFrameRect();
    
    RSRenderNodeSingleDrawableLocker singleLocker(this);
    if (UNLIKELY(!singleLocker.IsLocked())) {
        singleLocker.DrawableOnDrawMultiAccessEventReport(__func__);
        RS_LOGE("RSEffectRenderNodeDrawable::OnDraw node %{public}" PRIu64 " onDraw!!!", GetId());
        if (RSSystemProperties::GetSingleDrawableLockerEnabled()) {
            SetDrawSkipType(DrawSkipType::MULTI_ACCESS);
            return;
        }
    }

    if (!GenerateEffectDataOnDemand(effectParams, canvas, bounds, paintFilterCanvas)) {
        SetDrawSkipType(DrawSkipType::GENERATE_EFFECT_DATA_ON_DEMAND_FAIL);
        return;
    }

    RSRenderNodeDrawableAdapter::DrawImpl(canvas, bounds, drawCmdIndex_.childrenIndex_);
#endif
}

bool RSEffectRenderNodeDrawable::GenerateEffectDataOnDemand(RSEffectRenderParams* effectParams,
    Drawing::Canvas& canvas, const Drawing::Rect& bounds, RSPaintFilterCanvas* paintFilterCanvas)
{
#ifdef RS_ENABLE_GPU
    if (drawCmdIndex_.childrenIndex_ == -1) {
        // case 0: No children, skip
        return false;
    } else if (drawCmdIndex_.backgroundFilterIndex_ == -1 ||
        !(RSSystemProperties::GetEffectMergeEnabled() && RSFilterCacheManager::isCCMEffectMergeEnable_) ||
        !effectParams->GetHasEffectChildren()) {
        // case 1: no blur or no need to blur, do nothing
    } else if (drawCmdIndex_.backgroundImageIndex_ == -1 || effectParams->GetCacheValid()) {
        // case 2: dynamic blur, blur the underlay content
        // case 3a: static blur with valid cache, reuse cache
        Drawing::AutoCanvasRestore acr(canvas, true);
        canvas.ClipIRect(Drawing::RectI(0, 0, bounds.GetWidth(), bounds.GetHeight()));
        RSRenderNodeDrawableAdapter::DrawImpl(canvas, bounds, drawCmdIndex_.backgroundFilterIndex_);
    } else {
        // case 3b: static blur without valid cache, draw background image and blur
        Drawing::AutoCanvasRestore acr(canvas, true);
        canvas.ClipIRect(Drawing::RectI(0, 0, bounds.GetWidth(), bounds.GetHeight()));
        auto surface = canvas.GetSurface();
        if (!surface) {
            ROSEN_LOGE("RSPropertiesPainter::DrawBackgroundImageAsEffect surface is null");
            return false;
        }
        // extract clip bounds
        auto currentRect = canvas.GetDeviceClipBounds();
        // create offscreen surface
        auto offscreenSurface = surface->MakeSurface(currentRect.GetWidth(), currentRect.GetHeight());
        if (!offscreenSurface) {
            ROSEN_LOGE("RSPropertiesPainter::DrawBackgroundImageAsEffect offscreenSurface is null");
            return false;
        }
        auto offscreenCanvas = std::make_unique<RSPaintFilterCanvas>(offscreenSurface.get());
        if (!offscreenCanvas || !paintFilterCanvas) {
            return false;
        }
        // copy current matrix to offscreen canvas, while aligned with current rect
        auto currentMatrix = canvas.GetTotalMatrix();
        currentMatrix.PostTranslate(-currentRect.GetLeft(), -currentRect.GetTop());
        offscreenCanvas->SetMatrix(currentMatrix);
        // draw background image and blur
        RSRenderNodeDrawableAdapter::DrawImpl(*offscreenCanvas, bounds, drawCmdIndex_.backgroundImageIndex_);
        RSRenderNodeDrawableAdapter::DrawImpl(*offscreenCanvas, bounds, drawCmdIndex_.backgroundFilterIndex_);
        // copy effect data from offscreen canvas to current canvas, aligned with current rect
        if (auto effectData = offscreenCanvas->GetEffectData()) {
            effectData->cachedRect_.Offset(currentRect.GetLeft(), currentRect.GetTop());
            paintFilterCanvas->SetEffectData(effectData);
        }
    }
    return true;
#else
    return false;
#endif
}

void RSEffectRenderNodeDrawable::OnCapture(Drawing::Canvas& canvas)
{
    auto& captureParam = RSUniRenderThread::GetCaptureParam();
    bool stopDrawForRangeCapture = (canvas.GetUICapture() &&
        captureParam.endNodeId_ == GetId() &&
        captureParam.endNodeId_ != INVALID_NODEID);
    if (stopDrawForRangeCapture) {
        return;
    }
    RSEffectRenderNodeDrawable::OnDraw(canvas);
}
} // namespace OHOS::Rosen::DrawableV2
