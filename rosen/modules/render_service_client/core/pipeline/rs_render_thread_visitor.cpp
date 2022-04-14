/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include <include/core/SkColor.h>
#include <include/core/SkFont.h>
#include <include/core/SkPaint.h>

#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_dirty_region_manager.h"
#include "pipeline/rs_node_map.h"
#include "pipeline/rs_render_thread.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "platform/common/rs_log.h"
#include "platform/drawing/rs_surface.h"
#include "rs_trace.h"
#include "transaction/rs_transaction_proxy.h"
#include "ui/rs_surface_extractor.h"
#include "ui/rs_surface_node.h"

namespace OHOS {
namespace Rosen {
RSRenderThreadVisitor::RSRenderThreadVisitor() : canvas_(nullptr) {}

RSRenderThreadVisitor::~RSRenderThreadVisitor() {}

void RSRenderThreadVisitor::PrepareBaseRenderNode(RSBaseRenderNode& node)
{
    for (auto& child : node.GetSortedChildren()) {
        child->Prepare(shared_from_this());
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
    for (auto& child : node.GetSortedChildren()) {
        child->Process(shared_from_this());
    }
    // clear SortedChildren, it will be generated again in next frame
    node.ResetSortedChildren();
}

void RSRenderThreadVisitor::ProcessRootRenderNode(RSRootRenderNode& node)
{
    if (!isIdle_) {
        ProcessCanvasRenderNode(node);
        return;
    }
    if (!node.GetRenderProperties().GetVisible()) {
        ROSEN_LOGE("No valid RSRootRenderNode");
        return;
    }
    if (node.GetSurfaceWidth() <= 0 || node.GetSurfaceHeight() <= 0) {
        ROSEN_LOGE("RSRootRenderNode have negative width or height [%d %d]", node.GetSurfaceWidth(),
            node.GetSurfaceHeight());
        return;
    }
    auto ptr = RSNodeMap::Instance().GetNode<RSSurfaceNode>(node.GetRSSurfaceNodeId());
    if (ptr == nullptr) {
        ROSEN_LOGE("No valid RSSurfaceNode");
        return;
    }

    auto surfaceNodeColorSpace = ptr->GetColorSpace();

    std::shared_ptr<RSSurface> rsSurface = RSSurfaceExtractor::ExtractRSSurface(ptr);
    if (rsSurface == nullptr) {
        ROSEN_LOGE("No RSSurface found");
        return;
    }

    auto rsSurfaceColorSpace = rsSurface->GetColorSpace();

    if (surfaceNodeColorSpace != rsSurfaceColorSpace) {
        ROSEN_LOGD("Set new colorspace %d to rsSurface", surfaceNodeColorSpace);
        rsSurface->SetColorSpace(surfaceNodeColorSpace);
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

    sk_sp<SkSurface> skSurface = nullptr;
    auto iter = forceRasterNodes.find(node.GetId());
    if (iter != forceRasterNodes.end()) {
        ROSEN_LOGD("Force Raster draw");
        forceRasterNodes.erase(iter);
        SkImageInfo imageInfo = SkImageInfo::Make(node.GetSurfaceWidth(), node.GetSurfaceHeight(),
            kRGBA_8888_SkColorType, kOpaque_SkAlphaType, SkColorSpace::MakeSRGB());
        skSurface = SkSurface::MakeRaster(imageInfo);
        canvas_ = new RSPaintFilterCanvas(skSurface->getCanvas());
    } else {
        canvas_ = new RSPaintFilterCanvas(surfaceFrame->GetCanvas());
    }
    canvas_->clear(SK_ColorTRANSPARENT);
    isIdle_ = false;
    ProcessCanvasRenderNode(node);

    if (skSurface) {
        canvas_->flush();
        surfaceFrame->GetCanvas()->clear(SK_ColorTRANSPARENT);
        skSurface->draw(surfaceFrame->GetCanvas(), 0.f, 0.f, nullptr);
    }
    if (RSRootRenderNode::NeedForceRaster()) {
        RSRootRenderNode::MarkForceRaster(false);
        forceRasterNodes.insert(node.GetId());
        if (!skSurface) {
            RSRenderThread::Instance().RequestNextVSync();
        }
    }

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
    if (!node.GetRenderProperties().GetVisible()) {
        ROSEN_LOGI("RSRenderThreadVisitor::ProcessSurfaceRenderNode node : %llu is unvisible", node.GetId());
        return;
    }
    // RSSurfaceRenderNode in RSRenderThreadVisitor do not have information of property.
    // We only get parent's matrix and send it to RenderService
#ifdef ROSEN_OHOS
    node.SetMatrix(canvas_->getTotalMatrix());
    node.SetAlpha(canvas_->GetAlpha());
    node.SetParentId(node.GetParent().lock()->GetId());
    auto clipRect = canvas_->getDeviceClipBounds();
    node.SetClipRegion({ clipRect.left(), clipRect.top(), clipRect.width(), clipRect.height() });

    auto x = node.GetRenderProperties().GetBoundsPositionX();
    auto y = node.GetRenderProperties().GetBoundsPositionY();
    auto width = node.GetRenderProperties().GetBoundsWidth();
    auto height = node.GetRenderProperties().GetBoundsHeight();
    canvas_->save();
    canvas_->clipRect(SkRect::MakeXYWH(x, y, width, height));
    if (node.IsBufferAvailable() == true) {
        ROSEN_LOGI("RSRenderThreadVisitor::ProcessSurfaceRenderNode node : %llu, clip [%f, %f, %f, %f]",
            node.GetId(), x, y, width, height);
        canvas_->clear(SK_ColorTRANSPARENT);
    } else {
        ROSEN_LOGI("RSRenderThreadVisitor::ProcessSurfaceRenderNode node : %llu, not clip [%f, %f, %f, %f]",
            node.GetId(), x, y, width, height);
        if (node.NeedSetCallbackForRenderThreadRefresh() == true) {
            node.SetCallbackForRenderThreadRefresh([] {
                RSRenderThread::Instance().RequestNextVSync();
            });
        }
        auto backgroundColor = node.GetRenderProperties().GetBackgroundColor();
        if (backgroundColor != RgbPalette::Transparent()) {
            canvas_->clear(backgroundColor.AsArgbInt());
        } else {
            canvas_->clear(SK_ColorBLACK);
        }
    }
    canvas_->restore();

#endif
    ProcessBaseRenderNode(node);
}
} // namespace Rosen
} // namespace OHOS
