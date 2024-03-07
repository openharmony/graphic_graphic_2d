/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "rs_driven_render_visitor.h"

#include "common/rs_common_def.h"
#include "common/rs_obj_abs_geometry.h"
#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_base_render_util.h"
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_surface_render_node.h"
#include "platform/common/rs_log.h"
#include "property/rs_properties_painter.h"
#include "rs_driven_render_listener.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {
RSDrivenRenderVisitor::RSDrivenRenderVisitor()
{
    auto mainThread = RSMainThread::Instance();
    renderEngine_ = mainThread->GetRenderEngine();
}

void RSDrivenRenderVisitor::PrepareChildren(RSRenderNode& node)
{
    for (auto& child : *node.GetSortedChildren()) {
        child->Prepare(shared_from_this());
    }
}

void RSDrivenRenderVisitor::PrepareCanvasRenderNode(RSCanvasRenderNode& node)
{
    auto rsParent = (node.GetParent().lock());
    if (rsParent && rsParent->IsMarkDrivenRender()) {
        auto paintItem = node.GetRenderProperties().GetFrame();
        int32_t itemIndex = node.GetItemIndex();
        currDrivenSurfaceNode_->PushFramePaintItem(paintItem, itemIndex);
        return;
    }
    PrepareChildren(node);
}

void RSDrivenRenderVisitor::PrepareDrivenSurfaceRenderNode(RSDrivenSurfaceRenderNode& node)
{
    if (node.IsInvalidSurface()) {
        return;
    }
    currDrivenSurfaceNode_ = std::static_pointer_cast<RSDrivenSurfaceRenderNode>(node.shared_from_this());
    auto canvasNode = RSBaseRenderNode::ReinterpretCast<RSCanvasRenderNode>(node.GetDrivenCanvasNode());
    if (canvasNode == nullptr) {
        RS_LOGE("RSDrivenRenderVisitor:: Driven canvasNode is null!");
        return;
    }
    auto& property = canvasNode->GetRenderProperties();

    if (node.IsContentSurface()) {
        PrepareCanvasRenderNode(*canvasNode);
    }

    auto geoPtr = (property.GetBoundsGeometry());
    RectF surfaceBounds = RectF(0, 0, screenRect_.GetWidth(), screenRect_.GetHeight());
    RectF viewPort = RectF(0, 0, screenRect_.GetWidth(), screenRect_.GetHeight());
    RectI dstRect = screenRect_;
    RectI contentAbsRect = geoPtr->GetAbsRect();

    if (node.IsContentSurface()) {
        surfaceBounds = property.GetBoundsRect();
        viewPort = property.GetBoundsRect();
        dstRect = geoPtr->GetAbsRect();
        if (property.GetClipToFrame()) {
            viewPort = RectF(property.GetFrameOffsetX(), property.GetFrameOffsetY(),
                property.GetFrameWidth(), property.GetFrameHeight());
            dstRect = geoPtr->MapAbsRect(viewPort);
        }
        auto cmdsClipFrameRect = canvasNode->GetDrivenContentClipFrameRect();
        if (!cmdsClipFrameRect.IsEmpty()) {
            viewPort = cmdsClipFrameRect;
            dstRect = geoPtr->MapAbsRect(viewPort);
        }
    }
    currDrivenSurfaceNode_->SetCurrFrameBounds(surfaceBounds, viewPort, contentAbsRect);
    currDrivenSurfaceNode_->UpdateActivateFrameState(dstRect, backgroundDirty_, contentDirty_, nonContentDirty_);
    RS_LOGD("RSDrivenRenderVisitor::PrepareDrivenSurfaceRenderNode DstRect = %{public}s, SrcRect = %{public}s",
        node.GetDstRect().ToString().c_str(), node.GetSrcRect().ToString().c_str());
}

void RSDrivenRenderVisitor::ProcessChildren(RSRenderNode& node)
{
    if (hasTraverseDrivenNode_ && currDrivenSurfaceNode_->IsBackgroundSurface()) {
        return;
    }
    for (auto& child : *node.GetSortedChildren()) {
        child->Process(shared_from_this());
    }
}

void RSDrivenRenderVisitor::ProcessCanvasRenderNode(RSCanvasRenderNode& node)
{
    if (!node.ShouldPaint()) {
        RS_LOGE("RSDrivenRenderVisitor::ProcessCanvasRenderNode, no need process");
        return;
    }
    if (!canvas_) {
        RS_LOGE("RSDrivenRenderVisitor::ProcessCanvasRenderNode, canvas is nullptr");
        return;
    }
    if (node.IsMarkDrivenRender()) {
        hasTraverseDrivenNode_ = true;
        ProcessDrivenCanvasRenderNode(node);
        return;
    }
    if (hasTraverseDrivenNode_ && currDrivenSurfaceNode_->IsBackgroundSurface()) {
        return;
    }
    node.ProcessRenderBeforeChildren(*canvas_);
    node.ProcessRenderContents(*canvas_);
    ProcessChildren(node);
    node.ProcessRenderAfterChildren(*canvas_);
}

void RSDrivenRenderVisitor::ProcessDrivenSurfaceRenderNode(RSDrivenSurfaceRenderNode& node)
{
    if (node.IsExpandedMode()) {
        RS_TRACE_NAME("RSUniRender:DrivenRenderExFrame");
        RenderExpandedFrame(node);
    } else if (node.IsReusableMode()) {
        RS_TRACE_NAME("RSUniRender:DrivenRenderReuse");
        SkipRenderExpandedFrame(node);
    } else {
        RS_TRACE_NAME("RSUniRender:DrivenRenderDisable");
    }
}

void RSDrivenRenderVisitor::ProcessDrivenCanvasRenderNode(RSCanvasRenderNode& node)
{
    if (currDrivenSurfaceNode_->IsBackgroundSurface()) {
        node.ProcessDrivenBackgroundRender(*canvas_);
    } else {
        node.ProcessDrivenContentRender(*canvas_);
        ProcessChildren(node);
        node.ProcessDrivenContentRenderAfterChildren(*canvas_);
    }
}

void RSDrivenRenderVisitor::SetDirtyInfo(bool backgroundDirty, bool contentDirty, bool nonContentDirty)
{
    backgroundDirty_ = backgroundDirty;
    contentDirty_ = contentDirty;
    nonContentDirty_ = nonContentDirty;
}

void RSDrivenRenderVisitor::SetScreenRect(const RectI& rect)
{
    screenRect_ = rect;
}

void RSDrivenRenderVisitor::SetUniProcessor(std::shared_ptr<RSProcessor> processor)
{
    uniProcessor_ = processor;
}

void RSDrivenRenderVisitor::SetUniColorSpace(GraphicColorGamut colorSpace)
{
    uniColorSpace_ = colorSpace;
}

void RSDrivenRenderVisitor::SetUniGlobalZOrder(float globalZOrder)
{
    uniGlobalZOrder_ = globalZOrder;
}

void RSDrivenRenderVisitor::RenderExpandedFrame(RSDrivenSurfaceRenderNode& node)
{
    if (uniProcessor_ == nullptr) {
        RS_LOGE("RSDrivenRenderVisitor RSProcessor is null!");
        return;
    }

    auto canvasNode = RSBaseRenderNode::ReinterpretCast<RSCanvasRenderNode>(node.GetDrivenCanvasNode());
    if (canvasNode == nullptr) {
        RS_LOGE("RSDrivenRenderVisitor canvasNode is null!");
        return;
    }

    if (node.IsInvalidSurface()) {
        RS_LOGE("RSDrivenRenderVisitor DrivenSurfaceType is NONE!");
        return;
    }

    currDrivenSurfaceNode_ = std::static_pointer_cast<RSDrivenSurfaceRenderNode>(node.shared_from_this());
    hasTraverseDrivenNode_ = false;

    auto surfaceNodePtr = node.ReinterpretCastTo<RSDrivenSurfaceRenderNode>();
    if (!node.IsSurfaceCreated()) {
        sptr<IBufferConsumerListener> listener = new RSDrivenRenderListener(surfaceNodePtr);
        if (!node.CreateSurface(listener)) {
            RS_LOGE("RSDrivenRenderVisitor::RenderExpandedFrame CreateSurface failed");
            return;
        }
    }
    auto rsSurface = node.GetRSSurface();
    if (rsSurface == nullptr) {
        RS_LOGE("RSDrivenRenderVisitor::RenderExpandedFrame no RSSurface found");
        return;
    }
    rsSurface->SetColorSpace(uniColorSpace_);
#ifdef NEW_RENDER_CONTEXT
    auto renderFrame = renderEngine_->RequestFrame(std::static_pointer_cast<RSRenderSurfaceOhos>(rsSurface),
        node.GetBufferRequestConfig());
#else
    auto renderFrame = renderEngine_->RequestFrame(std::static_pointer_cast<RSSurfaceOhos>(rsSurface),
        node.GetBufferRequestConfig());
#endif
    if (renderFrame == nullptr) {
        RS_LOGE("RSDrivenRenderVisitor Request Frame Failed");
        return;
    }
#ifdef NEW_RENDER_CONTEXT
    if (renderFrame->GetSurface() == nullptr) {
        RS_LOGE("RSDrivenRenderVisitor::RenderExpandedFrame: RSSurfaceFrame is null");
        return;
    }
    auto skSurface = renderFrame->GetSurface()->GetSurface();
#else
    if (renderFrame->GetFrame() == nullptr) {
        RS_LOGE("RSDrivenRenderVisitor::RenderExpandedFrame: RSSurfaceFrame is null");
        return;
    }
    auto skSurface = renderFrame->GetFrame()->GetSurface();
#endif
    if (skSurface == nullptr) {
        RS_LOGE("RSDrivenRenderVisitor::RenderExpandedFrame: skSurface is null");
        return;
    }
    if (skSurface->GetCanvas() == nullptr) {
        ROSEN_LOGE("RSDrivenRenderVisitor skSurface.getCanvas is null.");
        return;
    }
    canvas_ = std::make_shared<RSPaintFilterCanvas>(skSurface.get());
    canvas_->Save();
    canvas_->Translate(node.GetFrameOffsetX(), node.GetFrameOffsetY());
    canvas_->ClipRect(Drawing::Rect(node.GetFrameClipRect().GetLeft(), node.GetFrameClipRect().GetTop(),
        node.GetFrameClipRect().GetRight(), node.GetFrameClipRect().GetBottom()),
        Drawing::ClipOp::INTERSECT, false);
    canvas_->Clear(SK_ColorTRANSPARENT);

    if (node.IsBackgroundSurface()) {
        RS_LOGD("RSDrivenRenderVisitor process BACKGROUND");
        auto rsDrivenParent = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(canvasNode->GetParent().lock());
        if (rsDrivenParent == nullptr) {
            return;
        }
        auto geoPtr = (
            rsDrivenParent->GetRenderProperties().GetBoundsGeometry());
        canvas_->ConcatMatrix(geoPtr->GetAbsMatrix());
    } else {
        RS_LOGD("RSDrivenRenderVisitor process CONTENT");
    }

    ProcessCanvasRenderNode(*canvasNode);
    canvas_->Restore();
    renderFrame->Flush();
    RS_TRACE_BEGIN("RSUniRender:WaitUtilDrivenRenderFinished");
    RSMainThread::Instance()->WaitUtilDrivenRenderFinished();
    RS_TRACE_END();
    float globalZOrder = node.IsContentSurface() ? uniGlobalZOrder_ - 1 : uniGlobalZOrder_ - 2;
    node.SetGlobalZOrder(globalZOrder);
    uniProcessor_->ProcessDrivenSurface(node);
}

void RSDrivenRenderVisitor::SkipRenderExpandedFrame(RSDrivenSurfaceRenderNode& node)
{
    if (uniProcessor_ == nullptr) {
        RS_LOGE("RSDrivenRenderVisitor RSProcessor is null!");
        return;
    }
    if (node.IsBackgroundSurface()) {
        RS_LOGD("RSDrivenRenderVisitor skip process BACKGROUND");
    } else {
        RS_LOGD("RSDrivenRenderVisitor skip process CONTENT");
    }
    float globalZOrder = node.IsContentSurface() ? uniGlobalZOrder_ - 1 : uniGlobalZOrder_ - 2;
    node.SetGlobalZOrder(globalZOrder);
    uniProcessor_->ProcessDrivenSurface(node);
}
} // namespace Rosen
} // namespace OHOS
