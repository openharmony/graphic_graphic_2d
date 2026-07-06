/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "rs_opinc_layer_splitter_processor.h"

#include "pipeline/layer_split/buffer/rs_layer_split_surface_buffer.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_screen_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_processor.h"
#include "system/rs_system_parameters.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/render_thread/rs_uni_render_processor.h"
#include "drawable/rs_render_node_drawable.h"
#include "drawable/rs_screen_render_node_drawable.h"
#include "params/rs_screen_render_params.h"
#include "screen_manager/rs_screen_manager.h"
#include "pipeline/layer_split/selector/opinc/rs_opinc_split_node_selector.h"
#include "pipeline/layer_split/splitter/opinc/controller/rs_opinc_split_controller.h"
#include "pipeline/layer_split/surface/rs_split_surface.h"

namespace OHOS::Rosen {

RSOpincLayerSplitterProcessor::RSOpincLayerSplitterProcessor()
{
    drawDfxEnabled_ = RSSystemParameters::GetLayerSplitterDfxEnable();
}

void RSOpincLayerSplitterProcessor::Sync(const std::shared_ptr<RSLayerSplitterPlanner>& layerSplitterPlanner)
{
    if (layerSplitterPlanner == nullptr) {
        planStatus_ = PlanStatus::OFF;
        return;
    }
    planStatus_ = layerSplitterPlanner->GetPlanStatus();

    srcRect_ = layerSplitterPlanner->GetSrcRect();
    dstRect_ = layerSplitterPlanner->GetDstRect();
    splitSurface_ = layerSplitterPlanner->GetSplitSurface();
    requestController_ = layerSplitterPlanner->GetRequestController();
    if (requestController_ && requestController_->CheckNeedRequest()) {
        opIncNodes_.clear();
        for (const auto& opIncNode : layerSplitterPlanner->GetOpIncNodes()) {
            opIncNodes_.insert(opIncNode.first);
        }
    } else if (planStatus_ == PlanStatus::LEAVE) {
        opIncNodes_.clear();
    }
}

void RSOpincLayerSplitterProcessor::RequestFrame(RSSurfaceRenderParams& params)
{
    if (UNLIKELY(!requestController_) || !requestController_->CheckNeedRequest()) {
        return;
    }

    if (!splitSurface_) {
        return;
    }

    splitSurface_->RequestFrame(params);
    DrawDfx();
}

void RSOpincLayerSplitterProcessor::DrawDfx()
{
    if (!drawDfxEnabled_) {
        return;
    }

    if (planStatus_ != PlanStatus::ON) {
        return;
    }

    if (splitSurface_ == nullptr) {
        LAYER_SPLIT_LOGD("RSOpincLayerSplitterProcessor::DrawDfx failed. splitSurface == nullptr");
        return;
    }

    splitSurface_->DrawDfx(Drawing::Color::COLOR_BLUE);
}

void RSOpincLayerSplitterProcessor::CalSplitCanvasMatrix(Drawing::Canvas& canvas, NodeId nodeId)
{
    if (!splitSurface_ || !splitSurface_->splitCanvas_) {
        return;
    }
    auto totalMatrix = canvas.GetTotalMatrix();
    splitSurface_->splitCanvas_->SetMatrix(totalMatrix);
    splitSurface_->splitCanvas_->Translate(
        (srcRect_.GetLeft() - dstRect_.GetLeft()) / totalMatrix.Get(Drawing::Matrix::SCALE_X),
        (srcRect_.GetTop() - dstRect_.GetTop()) / totalMatrix.Get(Drawing::Matrix::SCALE_Y)
    );
}

void RSOpincLayerSplitterProcessor::RecordNodeWithCacheImage(NodeId nodeId)
{
    if (planStatus_ != PlanStatus::OFF) {
        return;
    }

    if (UNLIKELY(!requestController_) || !requestController_->CheckNeedRequest()) {
        return;
    }

    if (opIncNodes_.find(nodeId) == opIncNodes_.end()) {
        return;
    }

    opIncNodes_.erase(nodeId);
    if (opIncNodes_.empty()) {
        splitSurface_->FlushFrame();
    }
}

bool RSOpincLayerSplitterProcessor::NeedDrawSplitCanvas(Drawing::Canvas& canvas, NodeId nodeId)
{
    if (planStatus_ != PlanStatus::PREPARE) {
        return false;
    }

    if (opIncNodes_.find(nodeId) != opIncNodes_.end()) {
        CalSplitCanvasMatrix(canvas, nodeId);
        return true;
    }

    return false;
}

bool RSOpincLayerSplitterProcessor::CanSkipOpIncNodeDraw(NodeId nodeId)
{
    if (!splitSurface_ || !splitSurface_->splitSurfaceDrawable_) {
        return false;
    }

    if (opIncNodes_.find(nodeId) == opIncNodes_.end()) {
        return false;
    }

    if (planStatus_ == PlanStatus::ON) {
        return true;
    }

    if (planStatus_ == PlanStatus::PREPARE &&
        splitSurface_->splitSurfaceDrawable_->GetRenderParams()->GetHardwareEnabled()) {
        return true;
    }

    return false;
}

std::shared_ptr<RSPaintFilterCanvas> RSOpincLayerSplitterProcessor::GetSplitCanvas()
{
    if (!splitSurface_) {
        return nullptr;
    }
    return splitSurface_->splitCanvas_;
}

} // namespace OHOS::Rosen