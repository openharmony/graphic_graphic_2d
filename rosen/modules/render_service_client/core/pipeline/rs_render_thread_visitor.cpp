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
#include "common/rs_obj_abs_geometry.h"
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
    dfxDirtyType_ = RSSystemProperties::GetDirtyRegionDebugType();
    isEglSetDamageRegion_ = !isRenderForced_ && (status != PartialRenderType::DISABLED);
    isOpDropped_ = (dfxDirtyType_ == DirtyRegionDebugType::DISABLED) && !isRenderForced_ &&
        (status == PartialRenderType::SET_DAMAGE_AND_DROP_OP);
    if (partialRenderStatus_ != status) {
        ROSEN_LOGD("PartialRenderStatus: %d->%d, isRenderForced_=%d, dfxDirtyType_=%d,\
            isEglSetDamageRegion_=%d, isOpDropped_=%d", partialRenderStatus_, status,
            isRenderForced_, dfxDirtyType_, isEglSetDamageRegion_, isOpDropped_);
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
        curDirtyManager_->UpdateDebugRegionTypeEnable(dfxDirtyType_);
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

void RSRenderThreadVisitor::ResetAndPrepareChildrenNode(RSRenderNode& node,
    std::shared_ptr<RSBaseRenderNode> nodeParent)
{
    // merge last childRect as dirty if any child has been removed
    if (node.HasRemovedChild()) {
        curDirtyManager_->MergeDirtyRect(node.GetChildrenRect());
        node.ResetHasRemovedChild();
    }
    // reset childRect before prepare children
    node.ResetChildrenRect();
    node.UpdateChildrenOutOfRectFlag(false);
    PrepareBaseRenderNode(node);
    // accumulate direct parent's childrenRect
    node.UpdateParentChildrenRect(nodeParent);
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
    if (node.IsDirtyRegionUpdated() && curDirtyManager_ != nullptr &&
        curDirtyManager_->IsDebugRegionTypeEnable(DebugRegionType::CURRENT_SUB)) {
        curDirtyManager_->UpdateDirtyRegionInfoForDfx(node.GetId(), RSRenderNodeType::CANVAS_NODE,
            DirtyRegionType::UPDATE_DIRTY_REGION, node.GetOldDirty());
    }
    ResetAndPrepareChildrenNode(node, nodeParent);
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
    if (node.IsDirtyRegionUpdated() && curDirtyManager_ != nullptr &&
        curDirtyManager_->IsDebugRegionTypeEnable(DebugRegionType::CURRENT_SUB)) {
        curDirtyManager_->UpdateDirtyRegionInfoForDfx(node.GetId(), RSRenderNodeType::SURFACE_NODE,
            DirtyRegionType::UPDATE_DIRTY_REGION, node.GetOldDirty());
    }
    ResetAndPrepareChildrenNode(node, nodeParent);
    dirtyFlag_ = dirtyFlag;
}

void RSRenderThreadVisitor::DrawRectOnCanvas(const RectI& dirtyRect, const SkColor color,
    const SkPaint::Style fillType, float alpha, int strokeWidth)
{
    if (dirtyRect.IsEmpty()) {
        ROSEN_LOGD("DrawRectOnCanvas dirty rect is invalid.");
        return;
    }
    auto skRect = SkRect::MakeXYWH(dirtyRect.left_, dirtyRect.top_, dirtyRect.width_, dirtyRect.height_);
    SkPaint rectPaint;
    rectPaint.setColor(color);
    rectPaint.setAntiAlias(true);
    rectPaint.setAlphaf(alpha);
    rectPaint.setStyle(fillType);
    rectPaint.setStrokeWidth(strokeWidth);
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

    if (curDirtyManager_->IsDebugRegionTypeEnable(DebugRegionType::MULTI_HISTORY)) {
        dirtyRect = curDirtyManager_->GetDirtyRegion();
        if (dirtyRect.IsEmpty()) {
            ROSEN_LOGD("DrawDirtyRegion his dirty rect is invalid. dirtyRect = %s", dirtyRect.ToString().c_str());
        } else {
            ROSEN_LOGD("DrawDirtyRegion his dirty rect. dirtyRect = %s", dirtyRect.ToString().c_str());
            // green
            DrawRectOnCanvas(dirtyRect, 0x442FDD2F, SkPaint::kFill_Style, fillAlpha);
            DrawRectOnCanvas(dirtyRect, 0xFF2FDD2F, SkPaint::kStroke_Style, edgeAlpha);
        }
    }

    if (curDirtyManager_->IsDebugRegionTypeEnable(DebugRegionType::CURRENT_WHOLE)) {
        dirtyRect = curDirtyManager_->GetLatestDirtyRegion();
        if (dirtyRect.IsEmpty()) {
            ROSEN_LOGD("DrawDirtyRegion current frame's dirty rect is invalid. dirtyRect = %s",
                dirtyRect.ToString().c_str());
        } else {
            ROSEN_LOGD("DrawDirtyRegion cur dirty rect. dirtyRect = %s", dirtyRect.ToString().c_str());
            // yellow
            DrawRectOnCanvas(dirtyRect, 0x88FFFF00, SkPaint::kFill_Style, fillAlpha);
            DrawRectOnCanvas(dirtyRect, 0xFFFFFF00, SkPaint::kStroke_Style, edgeAlpha);
        }
    }

    if (curDirtyManager_->IsDebugRegionTypeEnable(DebugRegionType::CURRENT_SUB)) {
        const int strokeWidth = 4;
        std::map<NodeId, RectI> dirtyRegionRects_;
        curDirtyManager_->GetDirtyRegionInfo(dirtyRegionRects_, RSRenderNodeType::CANVAS_NODE,
            DirtyRegionType::UPDATE_DIRTY_REGION);
        ROSEN_LOGD("DrawDirtyRegion canvas dirtyRegionRects_ size %zu", dirtyRegionRects_.size());
        // Draw Canvas Node
        for (const auto& [nid, subRect] : dirtyRegionRects_) {
            ROSEN_LOGD("DrawDirtyRegion canvas node id %" PRIu64 " is dirty. dirtyRect = %s",
                nid, subRect.ToString().c_str());
            DrawRectOnCanvas(subRect, 0x88FF0000, SkPaint::kStroke_Style, edgeAlpha, strokeWidth);
        }
        
        curDirtyManager_->GetDirtyRegionInfo(dirtyRegionRects_, RSRenderNodeType::SURFACE_NODE,
            DirtyRegionType::UPDATE_DIRTY_REGION);
        ROSEN_LOGD("DrawDirtyRegion surface dirtyRegionRects_ size %zu", dirtyRegionRects_.size());
        // Draw Surface Node
        for (const auto& [nid, subRect] : dirtyRegionRects_) {
            ROSEN_LOGD("DrawDirtyRegion surface node id %" PRIu64 " is dirty. dirtyRect = %s",
                nid, subRect.ToString().c_str());
            DrawRectOnCanvas(subRect, 0xFFD864D8, SkPaint::kStroke_Style, edgeAlpha, strokeWidth);
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

#ifndef ROSEN_CROSS_PLATFORM
    auto surfaceNodeColorSpace = ptr->GetColorSpace();
#endif
    std::shared_ptr<RSSurface> rsSurface = RSSurfaceExtractor::ExtractRSSurface(ptr);
    if (rsSurface == nullptr) {
        ROSEN_LOGE("ProcessRoot %s: No RSSurface found", ptr->GetName().c_str());
        return;
    }
    // Update queue size for each process loop in case it dynamically changes
    queueSize_ = rsSurface->GetQueueSize();

#ifndef ROSEN_CROSS_PLATFORM
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
    ProcessCanvasRenderNode(node);

    if (childSurfaceNodeIds_ != node.childSurfaceNodeIds_) {
        auto thisSurfaceNodeId = node.GetRSSurfaceNodeId();
        std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeClearChild>(thisSurfaceNodeId);
        SendCommandFromRT(command, thisSurfaceNodeId, FollowType::FOLLOW_TO_SELF);
        for (const auto& childSurfaceNodeId : childSurfaceNodeIds_) {
            command = std::make_unique<RSBaseNodeAddChild>(thisSurfaceNodeId, childSurfaceNodeId, -1);
            SendCommandFromRT(command, thisSurfaceNodeId, FollowType::FOLLOW_TO_SELF);
        }
        node.childSurfaceNodeIds_ = std::move(childSurfaceNodeIds_);
    }
    RS_TRACE_END();

    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        ROSEN_LOGD("RSRenderThreadVisitor FlushImplicitTransactionFromRT uiTimestamp = %" PRIu64, uiTimestamp_);
        transactionProxy->FlushImplicitTransactionFromRT(uiTimestamp_);
    }

    if ((dfxDirtyType_ != DirtyRegionDebugType::DISABLED) && curDirtyManager_->IsDirty()) {
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
#ifdef RS_ENABLE_EGLQUERYSURFACE
    node.UpdateRenderStatus(curDirtyRegion_, isOpDropped_);
    if (node.IsRenderUpdateIgnored()) {
        return;
    }
#endif
    node.CheckCacheType();
    auto cacheType = node.GetCacheType();
    if (cacheType == RSRenderNode::SPHERIZE) {
        node.ProcessTransitionBeforeChildren(*canvas_);
        DrawChildRenderNode(node);
        node.ProcessTransitionAfterChildren(*canvas_);
    } else {
        node.ProcessRenderBeforeChildren(*canvas_);
        DrawChildRenderNode(node);
        node.ProcessRenderAfterChildren(*canvas_);
    }
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

    auto clipRect = RSPaintFilterCanvas::GetLocalClipBounds(*canvas_);
    if (!clipRect.has_value() ||
        clipRect->width() < std::numeric_limits<float>::epsilon() ||
        clipRect->height() < std::numeric_limits<float>::epsilon()) {
        // if clipRect is empty, this node will be removed from parent's children list.
        node.SetContextClipRegion(std::nullopt);
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
    if (childSurfaceNodeIds_ != node.childSurfaceNodeIds_) {
        auto thisSurfaceNodeId = node.GetId();
        std::unique_ptr<RSCommand> command = std::make_unique<RSBaseNodeClearChild>(thisSurfaceNodeId);
        SendCommandFromRT(command, thisSurfaceNodeId, FollowType::FOLLOW_TO_SELF);
        for (const auto& childSurfaceNodeId : childSurfaceNodeIds_) {
            command = std::make_unique<RSBaseNodeAddChild>(thisSurfaceNodeId, childSurfaceNodeId, -1);
            SendCommandFromRT(command, thisSurfaceNodeId, FollowType::FOLLOW_TO_SELF);
        }
        node.childSurfaceNodeIds_ = std::move(childSurfaceNodeIds_);
    }

    // 5. restore environments variables before continue traversal siblings
    childSurfaceNodeIds_ = std::move(siblingSurfaceNodeIds);
    parentSurfaceNodeMatrix_ = parentSurfaceNodeMatrix;

    // 6.draw border
    canvas_->save();
    auto geoPtr = std::static_pointer_cast<RSObjAbsGeometry>(node.GetRenderProperties().GetBoundsGeometry());
    canvas_->concat(geoPtr->GetMatrix());
    RSPropertiesPainter::DrawBorder(node.GetRenderProperties(), *canvas_);
    canvas_->restore();
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

    // context clipRect should be based on parent node's coordinate system, in dividend render mode, it is the
    // same as canvas coordinate system.
    auto clipRect = RSPaintFilterCanvas::GetLocalClipBounds(*canvas_);
    node.SetContextClipRegion(clipRect);

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

void RSRenderThreadVisitor::DrawChildRenderNode(RSRenderNode& node)
{
    if (node.GetCacheTypeChanged()) {
        node.ClearCacheSurface();
        node.SetCacheTypeChanged(false);
    }
    if (node.GetCacheType() != RSRenderNode::SPHERIZE) {
        ProcessBaseRenderNode(node);
        return;
    }
    if (node.GetCacheSurface()) {
        RS_TRACE_NAME("RSRenderThreadVisitor::DrawChildRenderNode Draw nodeId = " +
            std::to_string(node.GetId()));
        RSPropertiesPainter::DrawCachedSpherizeSurface(node, *canvas_, node.GetCacheSurface());
        return;
    }
    RS_TRACE_NAME("RSRenderThreadVisitor::DrawChildRenderNode Init Draw nodeId = " +
        std::to_string(node.GetId()));
    int width = std::ceil(node.GetRenderProperties().GetBoundsRect().GetWidth());
    int height = std::ceil(node.GetRenderProperties().GetBoundsRect().GetHeight());
#if ((defined RS_ENABLE_GL) && (defined RS_ENABLE_EGLIMAGE)) || (defined RS_ENABLE_VK)
    SkImageInfo info = SkImageInfo::MakeN32Premul(width, height);
    node.SetCacheSurface(SkSurface::MakeRenderTarget(canvas_->getGrContext(), SkBudgeted::kYes, info));
#else
    node.SetCacheSurface(SkSurface::MakeRasterN32Premul(width, height));
#endif
    if (!node.GetCacheSurface()) {
        RS_LOGE("RSRenderThreadVisitor::DrawChildRenderNode %" PRIu64 " Create CacheSurface failed",
            node.GetId());
        return;
    }
    auto cacheCanvas = std::make_shared<RSPaintFilterCanvas>(node.GetCacheSurface().get());
    // When drawing CacheSurface, all child node should be drawn.
    // So set isOpDropped_ = false here.
    bool isOpDropped = isOpDropped_;
    isOpDropped_ = false;

    swap(cacheCanvas, canvas_);

    node.ProcessAnimatePropertyBeforeChildren(*canvas_);
    node.ProcessRenderContents(*canvas_);
    ProcessBaseRenderNode(node);
    node.ProcessAnimatePropertyAfterChildren(*canvas_);
    swap(cacheCanvas, canvas_);

    isOpDropped_ = isOpDropped;
    RSPropertiesPainter::DrawCachedSpherizeSurface(node, *canvas_, node.GetCacheSurface());
}
} // namespace Rosen
} // namespace OHOS
