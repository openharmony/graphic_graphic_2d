/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include <cmath>
#include <include/core/SkColor.h>
#include <include/core/SkFont.h>
#include <include/core/SkMatrix.h>
#include <include/core/SkPaint.h>
#include <include/core/SkRect.h>

#include "rs_trace.h"

#include "command/rs_base_node_command.h"
#include "common/rs_vector4.h"
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_dirty_region_manager.h"
#include "pipeline/rs_node_map.h"
#include "pipeline/rs_proxy_render_node.h"
#include "pipeline/rs_render_thread.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "platform/common/rs_log.h"
#include "platform/drawing/rs_surface.h"
#include "transaction/rs_transaction_proxy.h"
#include "ui/rs_surface_extractor.h"
#include "ui/rs_surface_node.h"

#ifdef ROSEN_OHOS
#include <frame_collector.h>
#include <frame_painter.h>
#include "platform/ohos/overdraw/rs_cpu_overdraw_canvas_listener.h"
#include "platform/ohos/overdraw/rs_gpu_overdraw_canvas_listener.h"
#include "platform/ohos/overdraw/rs_overdraw_controller.h"
#endif

namespace OHOS {
namespace Rosen {
RSRenderThreadVisitor::RSRenderThreadVisitor()
    : curDirtyManager_(std::make_shared<RSDirtyRegionManager>()), canvas_(nullptr) {}

RSRenderThreadVisitor::~RSRenderThreadVisitor() = default;

bool RSRenderThreadVisitor::IsValidRootRenderNode(RSRootRenderNode& node)
{
    auto ptr = RSNodeMap::Instance().GetNode<RSSurfaceNode>(node.GetRSSurfaceNodeId());
    if (ptr == nullptr) {
        ROSEN_LOGE("No valid RSSurfaceNode id");
        return false;
    }
    if (!node.enableRender_) {
        ROSEN_LOGD("RootNode %s: Invisible", ptr->GetName().c_str());
        return false;
    }
    if (node.GetSuggestedBufferWidth() <= 0 || node.GetSuggestedBufferHeight() <= 0) {
        ROSEN_LOGD("Root %s: Negative width or height [%f %f]", ptr->GetName().c_str(),
            node.GetSuggestedBufferWidth(), node.GetSuggestedBufferHeight());
        return false;
    }
    return true;
}

void RSRenderThreadVisitor::SetPartialRenderStatus(PartialRenderType status, bool isRenderForced)
{
    isRenderForced_ = isRenderForced;
    isEglSetDamageRegion_ = !isRenderForced_ && (status != PartialRenderType::DISABLED);
    isOpDropped_ = !isRenderForced_ && (status == PartialRenderType::SET_DAMAGE_AND_DROP_OP);
    if (partialRenderStatus_ == status) {
        ROSEN_LOGD("SetPartialRenderStatus: %d->%d, isRenderForced_=%d, isEglSetDamageRegion_=%d, isOpDropped_=%d",
            partialRenderStatus_, status, isRenderForced_, isEglSetDamageRegion_, isOpDropped_);
    }
    partialRenderStatus_ = status;
}

void RSRenderThreadVisitor::PrepareBaseRenderNode(RSBaseRenderNode& node)
{
    node.ResetSortedChildren();
    for (auto& child : node.GetChildren()) {
        if (auto renderChild = RSBaseRenderNode::ReinterpretCast<RSRenderNode>(child.lock())) {
            renderChild->ApplyModifiers();
        }
    }
    for (auto& child : node.GetSortedChildren()) {
        child->Prepare(shared_from_this());
    }
}

void RSRenderThreadVisitor::PrepareRootRenderNode(RSRootRenderNode& node)
{
    if (isIdle_) {
        curDirtyManager_ = node.GetDirtyManager();
        curDirtyManager_->Clear();
        // After the node calls ApplyModifiers, the modifiers assign the renderProperties to the node
        // Otherwise node.GetSuggestedBufferHeight always less than 0, causing black screen
        node.ApplyModifiers();
        if (!IsValidRootRenderNode(node)) {
            return;
        }
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
    node.ApplyModifiers();
    if (!node.ShouldPaint()) {
        return;
    }
    bool dirtyFlag = dirtyFlag_;
    auto nodeParent = node.GetParent().lock();
    std::shared_ptr<RSRenderNode> rsParent = nullptr;
    if (nodeParent != nullptr) {
        rsParent = nodeParent->ReinterpretCastTo<RSRenderNode>();
    }
    dirtyFlag_ = node.Update(*curDirtyManager_, rsParent ? &(rsParent->GetRenderProperties()) : nullptr, dirtyFlag_);
    if (node.IsDirtyRegionUpdated() && curDirtyManager_->IsDebugRegionTypeEnable(DebugRegionType::CURRENT_SUB)) {
        curDirtyManager_->UpdateDirtyCanvasNodes(node.GetId(), node.GetOldDirty());
    }
    PrepareBaseRenderNode(node);
    dirtyFlag_ = dirtyFlag;
}

void RSRenderThreadVisitor::PrepareSurfaceRenderNode(RSSurfaceRenderNode& node)
{
    node.ApplyModifiers();
    bool dirtyFlag = dirtyFlag_;
    auto nodeParent = node.GetParent().lock();
    std::shared_ptr<RSRenderNode> rsParent = nullptr;
    if (nodeParent != nullptr) {
        rsParent = nodeParent->ReinterpretCastTo<RSRenderNode>();
    }
    // If rt buffer switches to be available
    // set its SurfaceRenderNode's render dirty
    if (!node.IsNotifyRTBufferAvailablePre() && node.IsNotifyRTBufferAvailable()) {
        ROSEN_LOGD("NotifyRTBufferAvailable and set it dirty");
        node.SetDirty();
    }
    dirtyFlag_ = node.Update(*curDirtyManager_, rsParent ? &(rsParent->GetRenderProperties()) : nullptr, dirtyFlag_);
    if (node.IsDirtyRegionUpdated() && curDirtyManager_->IsDebugRegionTypeEnable(DebugRegionType::CURRENT_SUB)) {
        curDirtyManager_->UpdateDirtySurfaceNodes(node.GetId(), node.GetOldDirty());
    }
    PrepareBaseRenderNode(node);
    dirtyFlag_ = dirtyFlag;
}

void RSRenderThreadVisitor::DrawRectOnCanvas(const RectI& dirtyRect, const SkColor color,
    const SkPaint::Style fillType, float alpha)
{
    if (dirtyRect.width_ <= 0 || dirtyRect.height_ <= 0) {
        ROSEN_LOGD("DrawRectOnCanvas dirty rect is invalid.");
        return;
    }
    auto skRect = SkRect::MakeXYWH(dirtyRect.left_, dirtyRect.top_, dirtyRect.width_, dirtyRect.height_);
    const int defaultEdgeWidth = 6;
    SkPaint rectPaint;
    rectPaint.setColor(color);
    rectPaint.setAntiAlias(true);
    rectPaint.setAlphaf(alpha);
    rectPaint.setStyle(fillType);
    rectPaint.setStrokeWidth(defaultEdgeWidth);
    if (fillType == SkPaint::kFill_Style) {
        rectPaint.setStrokeJoin(SkPaint::kRound_Join);
    }
    canvas_->drawRect(skRect, rectPaint);
}

void RSRenderThreadVisitor::DrawDirtyRegion()
{
    auto dirtyRect = RectI();
    const float fillAlpha = 0.2;
    const float edgeAlpha = 0.4;
    const float subFactor = 2.0;

    if (curDirtyManager_->IsDebugRegionTypeEnable(DebugRegionType::MULTI_HISTORY)) {
        dirtyRect = curDirtyManager_->GetDirtyRegion();
        if (dirtyRect.IsEmpty()) {
            ROSEN_LOGD("DrawDirtyRegion his dirty rect is invalid. dirtyRect = [%d, %d, %d, %d]",
                dirtyRect.left_, dirtyRect.top_, dirtyRect.width_, dirtyRect.height_);
        } else {
            ROSEN_LOGD("DrawDirtyRegion his dirty rect. dirtyRect = [%d, %d, %d, %d]",
                dirtyRect.left_, dirtyRect.top_, dirtyRect.width_, dirtyRect.height_);
            // green
            DrawRectOnCanvas(dirtyRect, 0xFF0AFF0A, SkPaint::kFill_Style, fillAlpha / subFactor);
            DrawRectOnCanvas(dirtyRect, 0xFF0AFF0A, SkPaint::kStroke_Style, edgeAlpha);
        }
    }

    if (curDirtyManager_->IsDebugRegionTypeEnable(DebugRegionType::CURRENT_WHOLE)) {
        dirtyRect = curDirtyManager_->GetLatestDirtyRegion();
        if (dirtyRect.IsEmpty()) {
            ROSEN_LOGD("DrawDirtyRegion current frame's dirty rect is invalid. dirtyRect = [%d, %d, %d, %d]",
                dirtyRect.left_, dirtyRect.top_, dirtyRect.width_, dirtyRect.height_);
        } else {
            ROSEN_LOGD("DrawDirtyRegion cur dirty rect. dirtyRect = [%d, %d, %d, %d]",
                dirtyRect.left_, dirtyRect.top_, dirtyRect.width_, dirtyRect.height_);
            // yellow
            DrawRectOnCanvas(dirtyRect, 0xFFFFFF00, SkPaint::kFill_Style, fillAlpha);
            DrawRectOnCanvas(dirtyRect, 0xFFFFFF00, SkPaint::kStroke_Style, edgeAlpha);
        }
    }

    if (curDirtyManager_->IsDebugRegionTypeEnable(DebugRegionType::CURRENT_SUB)) {
        std::map<NodeId, RectI> dirtySubRects_;
        curDirtyManager_->GetDirtyCanvasNodes(dirtySubRects_);
        for (const auto& [nid, subRect] : dirtySubRects_) {
            ROSEN_LOGD("DrawDirtyRegion canvasNode id %" PRIu64 " is dirty. dirtyRect = [%d, %d, %d, %d]", nid,
                subRect.left_, subRect.top_, subRect.width_, subRect.height_);
            // red
            DrawRectOnCanvas(subRect, 0xFFFF0000, SkPaint::kStroke_Style, edgeAlpha / subFactor);
        }

        curDirtyManager_->GetDirtySurfaceNodes(dirtySubRects_);
        for (const auto& [nid, subRect] : dirtySubRects_) {
            ROSEN_LOGD("DrawDirtyRegion surfaceNode id %" PRIu64 " is dirty. dirtyRect = [%d, %d, %d, %d]", nid,
                subRect.left_, subRect.top_, subRect.width_, subRect.height_);
            // light purple
            DrawRectOnCanvas(subRect, 0xFFD899D8, SkPaint::kStroke_Style, edgeAlpha);
        }
    }
}

void RSRenderThreadVisitor::UpdateDirtyAndSetEGLDamageRegion(std::unique_ptr<RSSurfaceFrame>& surfaceFrame)
{
    RS_TRACE_BEGIN("UpdateDirtyAndSetEGLDamageRegion");
#ifdef RS_ENABLE_EGLQUERYSURFACE
    if (isEglSetDamageRegion_) {
        // get and update valid buffer age(>0) to merge history
        int32_t bufferAge = surfaceFrame->GetBufferAge();
        if (!curDirtyManager_->SetBufferAge(bufferAge)) {
            ROSEN_LOGD("ProcessRootRenderNode SetBufferAge with invalid buffer age %d", bufferAge);
            curDirtyManager_->ResetDirtyAsSurfaceSize();
        }
        curDirtyManager_->UpdateDirtyByAligned();
        curDirtyManager_->UpdateDirty();
        curDirtyRegion_ = curDirtyManager_->GetDirtyRegion();
        // only set damage region if dirty region and buffer age is valid(>0)
        if (bufferAge >= 0) {
            // get dirty rect coordinated from upper left to lower left corner in current surface
            RectI dirtyRectFlip = curDirtyManager_->GetRectFlipWithinSurface(curDirtyRegion_);
            // set dirty rect as eglSurfaceFrame's damage region
            surfaceFrame->SetDamageRegion(dirtyRectFlip.left_, dirtyRectFlip.top_, dirtyRectFlip.width_,
                dirtyRectFlip.height_);
            // flip aligned rect for op drops
            curDirtyRegion_ = curDirtyManager_->GetRectFlipWithinSurface(dirtyRectFlip);
            ROSEN_LOGD("GetPartialRenderEnabled buffer age %d, dirtyRectFlip = [%d, %d, %d, %d], "
                "dirtyRectAlign = [%d, %d, %d, %d]", bufferAge,
                dirtyRectFlip.left_, dirtyRectFlip.top_, dirtyRectFlip.width_, dirtyRectFlip.height_,
                curDirtyRegion_.left_, curDirtyRegion_.top_, curDirtyRegion_.width_, curDirtyRegion_.height_);
        }
    } else {
        curDirtyManager_->UpdateDirty();
        curDirtyRegion_ = curDirtyManager_->GetDirtyRegion();
    }
#else
    curDirtyManager_->UpdateDirty();
    curDirtyRegion_ = curDirtyManager_->GetDirtyRegion();
#endif
    ROSEN_LOGD("UpdateDirtyAndSetEGLDamageRegion dirtyRect = [%d, %d, %d, %d]",
        curDirtyRegion_.left_, curDirtyRegion_.top_, curDirtyRegion_.width_, curDirtyRegion_.height_);
    RS_TRACE_END();
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
    auto ptr = RSNodeMap::Instance().GetNode<RSSurfaceNode>(node.GetRSSurfaceNodeId());
    if (!IsValidRootRenderNode(node)) {
        return;
    }

    curDirtyManager_ = node.GetDirtyManager();

#if !defined(_WIN32) && !defined(__APPLE__) && !defined(__gnu_linux__)
    auto surfaceNodeColorSpace = ptr->GetColorSpace();
#endif
    std::shared_ptr<RSSurface> rsSurface = RSSurfaceExtractor::ExtractRSSurface(ptr);
    if (rsSurface == nullptr) {
        ROSEN_LOGE("ProcessRoot %s: No RSSurface found", ptr->GetName().c_str());
        return;
    }
    // Update queue size for each process loop in case it dynamically changes
    queueSize_ = rsSurface->GetQueueSize();

#if !defined(_WIN32) && !defined(__APPLE__) && !defined(__gnu_linux__)
    auto rsSurfaceColorSpace = rsSurface->GetColorSpace();
    if (surfaceNodeColorSpace != rsSurfaceColorSpace) {
        ROSEN_LOGD("Set new colorspace %d to rsSurface", surfaceNodeColorSpace);
        rsSurface->SetColorSpace(surfaceNodeColorSpace);
    }
#endif

#ifdef ACE_ENABLE_GL
    RenderContext* rc = RSRenderThread::Instance().GetRenderContext();
    rsSurface->SetRenderContext(rc);
#endif
    uiTimestamp_ = RSRenderThread::Instance().GetUITimestamp();
    RS_TRACE_BEGIN(ptr->GetName() + " rsSurface->RequestFrame");
#ifdef ROSEN_OHOS
    FrameCollector::GetInstance().MarkFrameEvent(FrameEventType::ReleaseStart);
#endif

    const auto& property = node.GetRenderProperties();
    const float bufferWidth = node.GetSuggestedBufferWidth() * property.GetScaleX();
    const float bufferHeight = node.GetSuggestedBufferHeight() * property.GetScaleY();
#ifdef ROSEN_OHOS
    auto surfaceFrame = rsSurface->RequestFrame(bufferWidth, bufferHeight, uiTimestamp_);
#else
    auto surfaceFrame = rsSurface->RequestFrame(std::round(bufferWidth), std::round(bufferHeight), uiTimestamp_);
#endif
    RS_TRACE_END();
    if (surfaceFrame == nullptr) {
        ROSEN_LOGI("ProcessRoot %s: Request Frame Failed", ptr->GetName().c_str());
#ifdef ROSEN_OHOS
        FrameCollector::GetInstance().MarkFrameEvent(FrameEventType::ReleaseEnd);
#endif
        return;
    }

    auto skSurface = surfaceFrame->GetSurface();
    if (skSurface == nullptr) {
        ROSEN_LOGE("skSurface null.");
        return;
    }
    if (skSurface->getCanvas() == nullptr) {
        ROSEN_LOGE("skSurface.getCanvas is null.");
        return;
    }

#ifdef ROSEN_OHOS
    // if listenedCanvas is nullptr, that means disabled or listen failed
    std::shared_ptr<RSListenedCanvas> listenedCanvas = nullptr;
    std::shared_ptr<RSCanvasListener> overdrawListener = nullptr;

    if (RSOverdrawController::GetInstance().IsEnabled()) {
        auto &oc = RSOverdrawController::GetInstance();
        listenedCanvas = std::make_shared<RSListenedCanvas>(skSurface.get());
        overdrawListener = oc.CreateListener<RSGPUOverdrawCanvasListener>(listenedCanvas.get());
        if (overdrawListener == nullptr) {
            overdrawListener = oc.CreateListener<RSCPUOverdrawCanvasListener>(listenedCanvas.get());
        }

        if (overdrawListener != nullptr) {
            listenedCanvas->SetListener(overdrawListener);
        } else {
            // create listener failed
            listenedCanvas = nullptr;
        }
    }

    if (listenedCanvas != nullptr) {
        canvas_ = listenedCanvas;
    } else {
        canvas_ = std::make_shared<RSPaintFilterCanvas>(skSurface.get());
    }
#else
    canvas_ = std::make_shared<RSPaintFilterCanvas>(skSurface.get());
#endif

    canvas_->SetHighContrast(RSRenderThread::Instance().isHighContrastEnabled());

    // node's surface size already check, so here we do not need to check return
    // attention: currently surfaceW/H are float values transformed into int implicitly
    (void)curDirtyManager_->SetSurfaceSize(bufferWidth, bufferHeight);
    // keep non-negative rect region within surface
    curDirtyManager_->ClipDirtyRectWithinSurface();
    // reset matrix
    const float rootWidth = property.GetFrameWidth() * property.GetScaleX();
    const float rootHeight = property.GetFrameHeight() * property.GetScaleY();
    SkMatrix gravityMatrix;
    (void)RSPropertiesPainter::GetGravityMatrix(
        Gravity::RESIZE, RectF { 0.0f, 0.0f, bufferWidth, bufferHeight }, rootWidth, rootHeight, gravityMatrix);

    if (isRenderForced_ ||
        curDirtyManager_->GetDirtyRegion().GetWidth() == 0 ||
        curDirtyManager_->GetDirtyRegion().GetHeight() == 0 ||
        !gravityMatrix.isIdentity()) {
        curDirtyManager_->ResetDirtyAsSurfaceSize();
    }
    UpdateDirtyAndSetEGLDamageRegion(surfaceFrame);

    canvas_->clipRect(SkRect::MakeWH(bufferWidth, bufferHeight));
    canvas_->clear(SK_ColorTRANSPARENT);
    isIdle_ = false;

    // clear current children before traversal, we will re-add them again during traversal
    childSurfaceNodeIds_.clear();

    canvas_->concat(gravityMatrix);
    parentSurfaceNodeMatrix_ = gravityMatrix;

    RS_TRACE_BEGIN("ProcessRenderNodes");
    needUpdateSurfaceNode_ = node.GetNeedUpdateSurfaceNode();
    ProcessCanvasRenderNode(node);

    if (childSurfaceNodeIds_ != node.childSurfaceNodeIds_ || needUpdateSurfaceNode_) {
        auto thisSurfaceNodeId = node.GetRSSurfaceNodeId();
        std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeClearSurfaceNodeChild>(thisSurfaceNodeId);
        SendCommandFromRT(command, thisSurfaceNodeId, FollowType::FOLLOW_VISITOR);
        for (const auto& childSurfaceNodeId : childSurfaceNodeIds_) {
            command = std::make_unique<RSBaseNodeAddChild>(thisSurfaceNodeId, childSurfaceNodeId, -1);
            SendCommandFromRT(command, thisSurfaceNodeId, FollowType::FOLLOW_VISITOR);
        }
        node.childSurfaceNodeIds_ = std::move(childSurfaceNodeIds_);
    }
    needUpdateSurfaceNode_ = false;
    node.SetNeedUpdateSurfaceNode(false);
    RS_TRACE_END();

    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        ROSEN_LOGD("RSRenderThreadVisitor FlushImplicitTransactionFromRT uiTimestamp = %" PRIu64, uiTimestamp_);
        transactionProxy->FlushImplicitTransactionFromRT(uiTimestamp_);
    }

    if (curDirtyManager_->IsDirty() && curDirtyManager_->IsDebugEnabled()) {
        ROSEN_LOGD("ProcessRootRenderNode %s [%" PRIu64 "] draw dirtyRect", ptr->GetName().c_str(), node.GetId());
        DrawDirtyRegion();
    }

#ifdef ROSEN_OHOS
    if (overdrawListener != nullptr) {
        overdrawListener->Draw();
    }

    FramePainter fpainter(FrameCollector::GetInstance());
    fpainter.Draw(*canvas_);
    FrameCollector::GetInstance().MarkFrameEvent(FrameEventType::ReleaseEnd);
    FrameCollector::GetInstance().MarkFrameEvent(FrameEventType::FlushStart);
#endif

    RS_TRACE_BEGIN(ptr->GetName() + " rsSurface->FlushFrame");
    ROSEN_LOGD("RSRenderThreadVisitor FlushFrame surfaceNodeId = %" PRIu64 ", uiTimestamp = %" PRIu64,
        node.GetRSSurfaceNodeId(), uiTimestamp_);
    rsSurface->FlushFrame(surfaceFrame, uiTimestamp_);
#ifdef ROSEN_OHOS
    FrameCollector::GetInstance().MarkFrameEvent(FrameEventType::FlushEnd);
#endif
    RS_TRACE_END();

    canvas_ = nullptr;
    isIdle_ = true;
}

void RSRenderThreadVisitor::ProcessCanvasRenderNode(RSCanvasRenderNode& node)
{
    if (!node.ShouldPaint()) {
        return;
    }
    if (!canvas_) {
        ROSEN_LOGE("RSRenderThreadVisitor::ProcessCanvasRenderNode, canvas is nullptr");
        return;
    }
    node.UpdateRenderStatus(curDirtyRegion_, isOpDropped_);
    if (node.IsRenderUpdateIgnored()) {
        return;
    }
    node.ProcessRenderBeforeChildren(*canvas_);
    ProcessBaseRenderNode(node);
    node.ProcessRenderAfterChildren(*canvas_);
}

static SkRect getLocalClipBounds(RSPaintFilterCanvas* canvas)
{
    SkIRect ibounds = canvas->getDeviceClipBounds();
    if (ibounds.isEmpty()) {
        return SkRect::MakeEmpty();
    }

    SkMatrix inverse;
    // if we can't invert the CTM, we can't return local clip bounds
    if (!(canvas->getTotalMatrix().invert(&inverse))) {
        return SkRect::MakeEmpty();
    }
    SkRect bounds;
    SkRect r = SkRect::Make(ibounds);
    inverse.mapRect(&bounds, r);
    return bounds;
}

void RSRenderThreadVisitor::ProcessSurfaceRenderNode(RSSurfaceRenderNode& node)
{
    if (!canvas_) {
        ROSEN_LOGE("RSRenderThreadVisitor::ProcessSurfaceRenderNode, canvas is nullptr");
        return;
    }
    if (!node.ShouldPaint()) {
        ROSEN_LOGI("RSRenderThreadVisitor::ProcessSurfaceRenderNode node : %" PRIu64 " is invisible", node.GetId());
        node.SetContextAlpha(0.0f);
        return;
    }
    // RSSurfaceRenderNode in RSRenderThreadVisitor do not have information of property.
    // We only get parent's matrix and send it to RenderService
    SkMatrix invertMatrix;
    SkMatrix contextMatrix = canvas_->getTotalMatrix();

    if (parentSurfaceNodeMatrix_.invert(&invertMatrix)) {
        contextMatrix.preConcat(invertMatrix);
    } else {
        ROSEN_LOGE("RSRenderThreadVisitor::ProcessSurfaceRenderNode, invertMatrix failed");
    }
    node.SetContextMatrix(contextMatrix);
    node.SetContextAlpha(canvas_->GetAlpha());

    // PLANNING: This is a temporary modification. Animation for surfaceView should not be triggered in RenderService.
    // We plan to refactor code here.
    node.SetContextBounds(node.GetRenderProperties().GetBounds());

    auto clipRect = getLocalClipBounds(canvas_.get());
    if (clipRect.width() < std::numeric_limits<float>::epsilon() ||
        clipRect.height() < std::numeric_limits<float>::epsilon()) {
        // if clipRect is empty, this node will be removed from parent's children list.
        return;
    }
    node.SetContextClipRegion(clipRect);

    // clip hole
    ClipHoleForSurfaceNode(node);

    // 1. add this node to parent's children list
    childSurfaceNodeIds_.emplace_back(node.GetId());

    // 2. backup and reset environment variables before traversal children
    std::vector<NodeId> siblingSurfaceNodeIds(std::move(childSurfaceNodeIds_));
    childSurfaceNodeIds_.clear();
    auto parentSurfaceNodeMatrix = parentSurfaceNodeMatrix_;
    parentSurfaceNodeMatrix_ = canvas_->getTotalMatrix();

    // 3. traversal children, child surface node will be added to childSurfaceNodeIds_
    // note: apply current node properties onto canvas if there is any child node
    ProcessBaseRenderNode(node);

    // 4. if children changed, sync children to RenderService
    if (childSurfaceNodeIds_ != node.childSurfaceNodeIds_ || needUpdateSurfaceNode_) {
        auto thisSurfaceNodeId = node.GetId();
        std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeClearSurfaceNodeChild>(thisSurfaceNodeId);
        SendCommandFromRT(command, thisSurfaceNodeId, FollowType::FOLLOW_VISITOR);
        for (const auto& childSurfaceNodeId : childSurfaceNodeIds_) {
            command = std::make_unique<RSBaseNodeAddChild>(thisSurfaceNodeId, childSurfaceNodeId, -1);
            SendCommandFromRT(command, thisSurfaceNodeId, FollowType::FOLLOW_VISITOR);
        }
        node.childSurfaceNodeIds_ = std::move(childSurfaceNodeIds_);
    }

    // 5. restore environments variables before continue traversal siblings
    childSurfaceNodeIds_ = std::move(siblingSurfaceNodeIds);
    parentSurfaceNodeMatrix_ = parentSurfaceNodeMatrix;
}

void RSRenderThreadVisitor::ProcessProxyRenderNode(RSProxyRenderNode& node)
{
    if (!canvas_) {
        ROSEN_LOGE("RSRenderThreadVisitor::ProcessProxyRenderNode, canvas is nullptr");
        return;
    }
    // RSProxyRenderNode in RSRenderThreadVisitor do not have information of property.
    // We only get parent's matrix and send it to RenderService
#ifdef ROSEN_OHOS
    SkMatrix invertMatrix;
    SkMatrix contextMatrix = canvas_->getTotalMatrix();

    if (parentSurfaceNodeMatrix_.invert(&invertMatrix)) {
        contextMatrix.preConcat(invertMatrix);
    } else {
        ROSEN_LOGE("RSRenderThreadVisitor::ProcessProxyRenderNode, invertMatrix failed");
    }
    node.SetContextMatrix(contextMatrix);
    node.SetContextAlpha(canvas_->GetAlpha());

    // for proxied nodes (i.e. remote window components), we only extract matrix & alpha, do not change their hierarchy
    // or clip or other properties.
    node.ResetSortedChildren();
#endif
}

void RSRenderThreadVisitor::ClipHoleForSurfaceNode(RSSurfaceRenderNode& node)
{
    // Calculation position in RenderService may appear floating point number, and it will be removed.
    // It caused missed line problem on surfaceview hap, so we subtract one pixel when cliphole to avoid this problem
    static int pixel = 1;
    auto x = std::ceil(node.GetRenderProperties().GetBoundsPositionX() + pixel); // x increase 1 pixel
    auto y = std::ceil(node.GetRenderProperties().GetBoundsPositionY() + pixel); // y increase 1 pixel
    auto width = std::floor(node.GetRenderProperties().GetBoundsWidth() - (2 * pixel)); // width decrease 2 pixels
    auto height = std::floor(node.GetRenderProperties().GetBoundsHeight() - (2 * pixel)); // height decrease 2 pixels
    canvas_->save();
    SkRect originRect = SkRect::MakeXYWH(x, y, width, height);
    canvas_->clipRect(originRect);
    if (node.IsNotifyRTBufferAvailable()) {
        ROSEN_LOGD("RSRenderThreadVisitor::ClipHoleForSurfaceNode node : %" PRIu64 ", clip [%f, %f, %f, %f]",
            node.GetId(), x, y, width, height);
        canvas_->clear(SK_ColorTRANSPARENT);
    } else {
        ROSEN_LOGD("RSRenderThreadVisitor::ClipHoleForSurfaceNode node : %" PRIu64 ", not clip [%f, %f, %f, %f]",
            node.GetId(), x, y, width, height);
        auto backgroundColor = node.GetRenderProperties().GetBackgroundColor();
        if (backgroundColor != RgbPalette::Transparent()) {
            canvas_->clear(backgroundColor.AsArgbInt());
        }
    }
    canvas_->restore();
}

void RSRenderThreadVisitor::SendCommandFromRT(std::unique_ptr<RSCommand>& command, NodeId nodeId, FollowType followType)
{
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommandFromRT(command, nodeId, followType);
    }
}
} // namespace Rosen
} // namespace OHOS
