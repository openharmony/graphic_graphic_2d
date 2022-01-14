/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "pipeline/rs_render_thread_visitor.h"

#include <include/core/SkFont.h>

#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_dirty_region_manager.h"
#include "pipeline/rs_render_thread.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "platform/common/rs_log.h"
#include "platform/drawing/rs_surface.h"
#include "rs_trace.h"
#include "pipeline/rs_node_map.h"
#include "transaction/rs_transaction_proxy.h"

#include "ui/rs_surface_node.h"
#include "ui/rs_surface_extractor.h"

namespace OHOS {
namespace Rosen {
RSRenderThreadVisitor::RSRenderThreadVisitor() : canvas_(nullptr) {}

RSRenderThreadVisitor::~RSRenderThreadVisitor() {}

void RSRenderThreadVisitor::PrepareBaseRenderNode(RSBaseRenderNode& node)
{
    for (auto child : node.GetChildren()) {
        if (auto c = child.lock()) {
            c->Prepare(shared_from_this());
        }
    }
}

void RSRenderThreadVisitor::PrepareRootRenderNode(RSRootRenderNode& node)
{
    if (isIdle_) {
        curTreeRoot_ = &node;
        curTreeRoot_->ClearSurfaceNodeInRS();

        dirtyManager_.Clear();
        parent_ = nullptr;
        dirtyFlag_ = false;
        isIdle_ = false;
        PrepareCanvasRenderNode(node);
        isIdle_ = true;
    } else {
        PrepareCanvasRenderNode(node);
    }
}

void RSRenderThreadVisitor::PrepareCanvasRenderNode(RSCanvasRenderNode& node)
{
    bool dirtyFlag = dirtyFlag_;
    dirtyFlag_ = node.Update(dirtyManager_, parent_ ? &(parent_->GetRenderProperties()) : nullptr, dirtyFlag_);
    PrepareBaseRenderNode(node);
    dirtyFlag_ = dirtyFlag;
}

void RSRenderThreadVisitor::PrepareSurfaceRenderNode(RSSurfaceRenderNode& node)
{
    curTreeRoot_->AddSurfaceRenderNode(node.GetId());
    bool dirtyFlag = dirtyFlag_;
    dirtyFlag_ = node.Update(dirtyManager_, parent_ ? &(parent_->GetRenderProperties()) : nullptr, dirtyFlag_);
    PrepareBaseRenderNode(node);
    dirtyFlag_ = dirtyFlag;
}

void RSRenderThreadVisitor::ProcessBaseRenderNode(RSBaseRenderNode& node)
{
    for (auto child : node.GetChildren()) {
        if (auto c = child.lock()) {
            c->Process(shared_from_this());
        }
    }
}

void RSRenderThreadVisitor::ProcessRootRenderNode(RSRootRenderNode& node)
{
    std::shared_ptr<RSSurface> rsSurface = nullptr;
    NodeId surfaceId = node.GetRSSurfaceNodeId();
    auto ptr = std::static_pointer_cast<RSSurfaceNode>(RSNodeMap::Instance().GetNode(surfaceId).lock());
    if (ptr == nullptr) {
        ROSEN_LOGE("No RSSurfaceNode found");
        return;
    }
    rsSurface = RSSurfaceExtractor::ExtractRSSurface(ptr);
    if (!isIdle_) {
        ProcessCanvasRenderNode(node);
        return;
    }

    if (rsSurface == nullptr) {
        ROSEN_LOGE("No RSSurface found");
        return;
    }
#ifdef ACE_ENABLE_GL
    RenderContext* rc = RSRenderThread::Instance().GetRenderContext();
    rsSurface->SetRenderContext(rc);
#endif
    RS_TRACE_BEGIN("rsSurface->RequestFrame");
    auto surfaceFrame = rsSurface->RequestFrame(node.GetSurfaceWidth(), node.GetSurfaceHeight());
    RS_TRACE_END();
    if (surfaceFrame == nullptr) {
        ROSEN_LOGE("Request Frame Failed");
        return;
    }
    canvas_ = new RSPaintFilterCanvas(surfaceFrame->GetCanvas());
    canvas_->clear(SK_ColorTRANSPARENT);

    isIdle_ = false;
    ProcessCanvasRenderNode(node);

    RS_TRACE_BEGIN("rsSurface->FlushFrame");
    rsSurface->FlushFrame(surfaceFrame);
    RS_TRACE_END();

    delete canvas_;
    canvas_ = nullptr;

    isIdle_ = true;
}

void RSRenderThreadVisitor::ProcessCanvasRenderNode(RSCanvasRenderNode& node)
{
    if (!node.GetRenderProperties().GetVisible()) {
        return;
    }
    if (!canvas_) {
        ROSEN_LOGE("RSRenderThreadVisitor::ProcessCanvasRenderNode, canvas is nullptr");
        return;
    }
    node.ProcessRenderBeforeChildren(*canvas_);
    ProcessBaseRenderNode(node);
    node.ProcessRenderAfterChildren(*canvas_);
}

void RSRenderThreadVisitor::ProcessSurfaceRenderNode(RSSurfaceRenderNode& node)
{
    if (!canvas_) {
        ROSEN_LOGE("RSRenderThreadVisitor::ProcessSurfaceRenderNode, canvas is nullptr");
        return;
    }
    // RSSurfaceRenderNode in RSRenderThreadVisitor do not have information of property.
    // We only get parent's matrix and send it to RenderService
#ifdef ROSEN_OHOS
    node.SetMatrix(canvas_->getTotalMatrix());
    node.SetAlpha(canvas_->GetAlpha());
    node.SetParentId(node.GetParent().lock()->GetId());
#endif
    ProcessBaseRenderNode(node);
}

} // namespace Rosen
} // namespace OHOS
