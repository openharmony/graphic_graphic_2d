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

#include "drawable/rs_surface_render_node_drawable.h"

#include "rs_trace.h"

#include "common/rs_obj_abs_geometry.h"
#include "memory/rs_tag_tracker.h"
#include "params/rs_display_render_params.h"
#include "params/rs_surface_render_params.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_uni_render_thread.h"
#include "pipeline/rs_uni_render_util.h"
#include "platform/common/rs_log.h"

namespace OHOS::Rosen {
RSSurfaceRenderNodeDrawable::Registrar RSSurfaceRenderNodeDrawable::instance_;

RSSurfaceRenderNodeDrawable::RSSurfaceRenderNodeDrawable(std::shared_ptr<const RSRenderNode>&& node)
    : RSRenderNodeDrawable(std::move(node))
{}

RSRenderNodeDrawable::Ptr RSSurfaceRenderNodeDrawable::OnGenerate(std::shared_ptr<const RSRenderNode> node)
{
    return new RSSurfaceRenderNodeDrawable(std::move(node));
}

void RSSurfaceRenderNodeDrawable::OnDraw(Drawing::Canvas& canvas) const
{
    if (!renderNode_) {
        RS_LOGE("There is no CanvasNode in RSSurfaceRenderNodeDrawable");
        return;
    }

    auto nodeSp = std::const_pointer_cast<RSRenderNode>(renderNode_);
    auto surfaceNode = std::static_pointer_cast<RSSurfaceRenderNode>(nodeSp);

    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceNode->GetRenderParams().get());
    if (!surfaceParams) {
        RS_LOGE("RSSurfaceRenderNodeDrawable::OnDraw params is nullptr");
        return;
    }

    auto rscanvas = static_cast<RSPaintFilterCanvas*>(&canvas);
    if (!rscanvas) {
        RS_LOGE("RSSurfaceRenderNodeDrawable::OnDraw, rscanvas us nullptr");
        return;
    }

    if (surfaceParams->IsMainWindowType() && surfaceParams->GetVisibleRegion().IsEmpty()) {
        RS_TRACE_NAME_FMT("RSSurfaceRenderNodeDrawable::OnDraw occlusion skip SurfaceName:%s NodeId:%" PRIu64 "",
            surfaceNode->GetName().c_str(), surfaceParams->GetId());
        return;
    }

    // TO-DO [UI First] Check UpdateCacheSurface
    // TO-DO [DFX] Draw Context ClipRect

    RS_TRACE_NAME("RSSurfaceRenderNodeDrawable::OnDraw:[" + surfaceNode->GetName() + "] " +
                  surfaceParams->GetDstRect().ToString() + "Alpha: " + std::to_string(surfaceNode->GetGlobalAlpha()));

    RS_LOGD("RSSurfaceRenderNodeDrawable::OnDraw node:%{public}" PRIu64 ",child size:%{public}u,"
            "name:%{public}s,OcclusionVisible:%{public}d",
        surfaceParams->GetId(), surfaceNode->GetChildrenCount(), surfaceNode->GetName().c_str(),
        surfaceParams->GetOcclusionVisible());

    auto renderEngine_ = RSUniRenderThread::Instance().GetRenderEngine();

    Drawing::GPUContext* gpuContext = renderEngine_->GetRenderContext()->GetDrGPUContext();
    surfaceNode->SetDrawingGPUContext(gpuContext); // TO-DO
    RSTagTracker tagTracker(gpuContext, surfaceParams->GetId(), RSTagTracker::TAGTYPE::TAG_DRAW_SURFACENODE);

    surfaceNode->UpdateFilterCacheStatusWithVisible(true);

    // TO-DO [Sub Thread] CheckFilterCache

    RSAutoCanvasRestore acr(rscanvas);

    rscanvas->MultiplyAlpha(surfaceParams->GetAlpha());

    bool isSelfDrawingSurface = surfaceParams->GetSurfaceNodeType() == RSSurfaceNodeType::SELF_DRAWING_NODE;
    if (isSelfDrawingSurface && !surfaceParams->IsSpherizeValid()) {
        rscanvas->Save();
    }

    rscanvas->ConcatMatrix(surfaceParams->GetMatrix());

    if (isSelfDrawingSurface) {
        RSUniRenderUtil::FloorTransXYInCanvasMatrix(*rscanvas);
    }

    nodeSp->ProcessRenderBeforeChildren(*rscanvas);

    if (surfaceNode->GetBuffer() != nullptr) {
        surfaceNode->SetGlobalAlpha(1.0f); // TO-DO
        int threadIndex = 0;
        auto params = RSUniRenderUtil::CreateBufferDrawParam(*surfaceNode, false, threadIndex, true);
        params.targetColorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB;
#ifdef USE_VIDEO_PROCESSING_ENGINE
        auto screenManager = CreateOrGetScreenManager();
        auto ancestor = surfaceParams->GetAncestorDisplayNode().lock()->ReinterpretCastTo<RSDisplayRenderNode>();
        if (!ancestor) {
            RS_LOGE("surfaceNode GetAncestorDisplayNode() return nullptr");
            return;
        }
        auto ancestorParam = static_cast<RSDisplayRenderParams*>(ancestor->GetRenderParams().get());
        params.screenBrightnessNits =
            screenManager->GetScreenBrightnessNits(ancestorParam ? ancestorParam->GetScreenId() : 0);
#endif
        auto bgColor = surfaceParams->GetBackgroundColor();
        if ((surfaceParams->GetSelfDrawingNodeType() != SelfDrawingNodeType::VIDEO) &&
            (bgColor != RgbPalette::Transparent())) {
            auto bounds = RSPropertiesPainter::Rect2DrawingRect(
                { 0, 0, surfaceParams->GetBounds().GetWidth(), surfaceParams->GetBounds().GetHeight() });
            Drawing::SaveLayerOps layerOps(&bounds, nullptr);
            rscanvas->SaveLayer(layerOps);
            rscanvas->SetAlpha(1.0f);
            Drawing::Brush brush;
            brush.SetColor(Drawing::Color(bgColor.AsArgbInt()));
            rscanvas->AttachBrush(brush);
            rscanvas->DrawRoundRect(RSPropertiesPainter::RRect2DrawingRRect(surfaceParams->GetRRect()));
            rscanvas->DetachBrush();
            renderEngine_->DrawSurfaceNodeWithParams(*rscanvas, *surfaceNode, params);
            rscanvas->Restore();
        } else {
            renderEngine_->DrawSurfaceNodeWithParams(*rscanvas, *surfaceNode, params);
        }
    }

    if (isSelfDrawingSurface) {
        rscanvas->Restore();
    }

    RSRenderNodeDrawable::OnDraw(canvas);
}

void RSSurfaceRenderNodeDrawable::OnCapture(Drawing::Canvas& canvas) const
{
    RSRenderNodeDrawable::OnCapture(canvas);
}
} // namespace OHOS::Rosen
