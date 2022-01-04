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

#include "pipeline/rs_dirty_region_manager.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "platform/common/rs_log.h"
#include "platform/drawing/rs_platform_canvas.h"
#include "platform/drawing/rs_surface.h"
#include "transaction/rs_transaction_proxy.h"
#include "platform/common/rs_log.h"

#include "pipeline/rs_render_thread.h"

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
        dirtyManager_.Clear();
        parent_ = nullptr;
        dirtyFlag_ = false;
        isIdle_ = false;
        PrepareRenderNode(node);
        isIdle_ = true;
    } else {
        PrepareRenderNode(node);
    }
}

void RSRenderThreadVisitor::PrepareRenderNode(RSRenderNode& node)
{
    bool dirtyFlag = dirtyFlag_;
    dirtyFlag_ = node.Update(dirtyManager_, parent_ ? &(parent_->GetRenderProperties()) : nullptr, dirtyFlag_);
    PrepareBaseRenderNode(node);
    dirtyFlag_ = dirtyFlag;
}

void RSRenderThreadVisitor::PrepareSurfaceRenderNode(RSSurfaceRenderNode& node)
{
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
    auto rsSurface = node.GetSurface();
    if (!isIdle_) {
        ProcessRenderNode(node);
        return;
    }
    if (rsSurface == nullptr) {
        // TODO: remove this branch after replace weston with RSSurface
        auto platformCanvas = node.GetPlatformCanvas();
        if (platformCanvas != nullptr) {
            isIdle_ = false;
            canvas_ = new RSPaintFilterCanvas(platformCanvas->AcquireCanvas());
            ProcessRenderNode(node);
            platformCanvas->FlushBuffer();
            delete canvas_;
            canvas_ = nullptr;
        }
    } else {
        // TODO: this branch is for RSSurface
        if (rsSurface == nullptr) {
            ROSEN_LOGE("No RSSurface found");
            return;
        }
#ifdef ACE_ENABLE_GL
        RenderContext* rc = RSRenderThread::Instance().GetRenderContext();
        rsSurface->SetRenderContext(rc);
#endif
        ROSEN_LOGE("RSRenderThreadVisitor RequestFrame start");
        auto surfaceFrame = rsSurface->RequestFrame(node.GetWidth(), node.GetHeight());
        if (surfaceFrame == nullptr) {
            ROSEN_LOGE("Request Frame Failed");
            return;
        }
        canvas_ = new RSPaintFilterCanvas(surfaceFrame->GetCanvas());

        isIdle_ = false;
        ProcessRenderNode(node);

        ROSEN_LOGE("RSRenderThreadVisitor FlushFrame start");
        rsSurface->FlushFrame(surfaceFrame);

        delete canvas_;
        canvas_ = nullptr;
    }
    isIdle_ = true;
}

void RSRenderThreadVisitor::ProcessRenderNode(RSRenderNode& node)
{
    if (!canvas_ || !node.GetRenderProperties().GetVisible()) {
        return;
    }
    node.ProcessRenderBeforeChildren(*canvas_);
    ProcessBaseRenderNode(node);
    node.ProcessRenderAfterChildren(*canvas_);
}

void RSRenderThreadVisitor::ProcessSurfaceRenderNode(RSSurfaceRenderNode& node)
{
    if (!canvas_) {
        return;
    }
    node.ProcessRenderBeforeChildren(*canvas_);
#ifdef ROSEN_OHOS
    node.SetMatrix(canvas_->getTotalMatrix());
    node.SetAlpha(canvas_->GetAlpha());
    node.SetParentId(node.GetParent().lock()->GetId());
#endif
    ProcessBaseRenderNode(node);
    node.ProcessRenderAfterChildren(*canvas_);
}

} // namespace Rosen
} // namespace OHOS
