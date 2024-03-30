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

#include "drawable/rs_canvas_render_node_drawable.h"

#include "common/rs_optional_trace.h"
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_uni_render_thread.h"
#include "platform/common/rs_log.h"
#include "rs_trace.h"
#include "utils/rect.h"
#include "utils/region.h"

namespace OHOS::Rosen::DrawableV2 {
RSCanvasRenderNodeDrawable::Registrar RSCanvasRenderNodeDrawable::instance_;

RSCanvasRenderNodeDrawable::RSCanvasRenderNodeDrawable(std::shared_ptr<const RSRenderNode>&& node)
    : RSRenderNodeDrawable(std::move(node))
{}

RSRenderNodeDrawable::Ptr RSCanvasRenderNodeDrawable::OnGenerate(std::shared_ptr<const RSRenderNode> node)
{
    return new RSCanvasRenderNodeDrawable(std::move(node));
}

/*
 * This function will be called recursively many times, and the logic should be as concise as possible.
 */
void RSCanvasRenderNodeDrawable::OnDraw(Drawing::Canvas& canvas)
{
    auto& params = renderNode_->GetRenderParams();
    if (!params) {
        RS_LOGE("params is nullptr");
        return;
    }
    if (!params->GetShouldPaint()) {
        return;
    }
    auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(&canvas);
    RSAutoCanvasRestore acr(paintFilterCanvas, RSPaintFilterCanvas::SaveType::kCanvasAndAlpha);
    params->ApplyAlphaAndMatrixToCanvas(*paintFilterCanvas);
    auto uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams().get();
    if (!paintFilterCanvas->GetIsParallelCanvas() && (!uniParam || uniParam->IsOpDropped()) &&
        paintFilterCanvas->GetDirtyFlag() && QuickReject(canvas, params->GetLocalDrawRect())) {
        RS_LOGD("CanvasNode[%{public}" PRIu64 "] have no intersect with canvas's clipRegion", params->GetId());
        return;
    }

    GenerateCacheIfNeed(canvas, *params);
    CheckCacheTypeAndDraw(canvas, *params);
    RSRenderNodeDrawable::ProcessedNodeCountInc();
}

// void RSCanvasRenderNodeDrawable::DrawShadow(Drawing::Canvas& canvas)
// {
//     if (RSUniRenderThread::GetCaptureParam().isInCaptureFlag_) { // route to surface capture
//         RSCanvasRenderNodeDrawable::OnCapture(canvas);
//         return;
//     }

//     auto& params = renderNode_->GetRenderParams();
//     if (!params) {
//         RS_LOGE("params is nullptr");
//         return;
//     }
//     auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(&canvas);
//     RSAutoCanvasRestore acr(paintFilterCanvas, RSPaintFilterCanvas::SaveType::kCanvasAndAlpha);
//     params->ApplyAlphaAndMatrixToCanvas(*paintFilterCanvas);
//     auto uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams().get();
//     if ((!uniParam || uniParam->IsOpDropped()) && static_cast<RSPaintFilterCanvas*>(&canvas)->GetDirtyFlag() &&
//         QuickReject(canvas, params->GetLocalDrawRect())) {
//         return;
//     }

//     RSRenderNodeDrawable::DrawShadow(canvas);
// }

// void RSCanvasRenderNodeDrawable::DrawWithoutShadow(Drawing::Canvas& canvas)
// {
//     if (RSUniRenderThread::GetCaptureParam().isInCaptureFlag_) { // route to surface capture
//         RSCanvasRenderNodeDrawable::OnCapture(canvas);
//         return;
//     }

//     auto& params = renderNode_->GetRenderParams();
//     if (!params) {
//         RS_LOGE("params is nullptr");
//         return;
//     }
//     auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(&canvas);
//     RSAutoCanvasRestore acr(paintFilterCanvas, RSPaintFilterCanvas::SaveType::kCanvasAndAlpha);
//     params->ApplyAlphaAndMatrixToCanvas(*paintFilterCanvas);
//     auto uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams().get();
//     if ((!uniParam || uniParam->IsOpDropped()) && static_cast<RSPaintFilterCanvas*>(&canvas)->GetDirtyFlag() &&
//         QuickReject(canvas, params->GetLocalDrawRect())) {
//         return;
//     }

//     RSRenderNodeDrawable::DrawWithoutShadow(canvas);
// }

/*
 * This function will be called recursively many times, and the logic should be as concise as possible.
 */
void RSCanvasRenderNodeDrawable::OnCapture(Drawing::Canvas& canvas)
{
    auto& params = renderNode_->GetRenderParams();
    if (!params) {
        return;
    }
    if (!params->GetShouldPaint()) {
        return;
    }
    auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(&canvas);
    RSAutoCanvasRestore acr(paintFilterCanvas, RSPaintFilterCanvas::SaveType::kCanvasAndAlpha);
    params->ApplyAlphaAndMatrixToCanvas(*paintFilterCanvas);

    CheckCacheTypeAndDraw(canvas, *params);
}
} // namespace OHOS::Rosen::DrawableV2
