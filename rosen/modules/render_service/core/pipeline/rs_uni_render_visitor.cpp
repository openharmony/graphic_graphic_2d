/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "pipeline/rs_uni_render_visitor.h"

#include "include/core/SkRegion.h"
#include "rs_trace.h"

#include "common/rs_common_def.h"
#include "common/rs_obj_abs_geometry.h"
#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_base_render_util.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_processor_factory.h"
#include "pipeline/rs_proxy_render_node.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_uni_render_listener.h"
#include "pipeline/rs_uni_render_mirror_processor.h"
#include "pipeline/rs_uni_render_util.h"
#include "platform/common/rs_log.h"
#include "property/rs_properties_painter.h"
#include "render/rs_skia_filter.h"

namespace OHOS {
namespace Rosen {
RSUniRenderVisitor::RSUniRenderVisitor()
    : curSurfaceDirtyManager_(std::make_shared<RSDirtyRegionManager>())
{
    auto mainThread = RSMainThread::Instance();
    renderEngine_ = mainThread->GetRenderEngine();
    partialRenderType_ = RSSystemProperties::GetUniPartialRenderEnabled();
    isPartialRenderEnabled_ = (partialRenderType_ != PartialRenderType::DISABLED);
    isOpDropped_ = isPartialRenderEnabled_ && (partialRenderType_ != PartialRenderType::SET_DAMAGE);
}
RSUniRenderVisitor::~RSUniRenderVisitor() {}

void RSUniRenderVisitor::PrepareBaseRenderNode(RSBaseRenderNode& node)
{
    node.ClearPaintOutOfParentRect();
    node.UpdateChildrenOutOfRectFlag(false);
    node.ResetSortedChildren();
    for (auto& child : node.GetSortedChildren()) {
        child->Prepare(shared_from_this());
    }
    SetPaintOutOfParentFlag(node);
}

void RSUniRenderVisitor::SetPaintOutOfParentFlag(RSBaseRenderNode& node)
{
    if (!isPartialRenderEnabled_) {
        return;
    }
    if (node.GetType() != RSRenderNodeType::CANVAS_NODE && node.GetType() != RSRenderNodeType::SURFACE_NODE) {
        RS_LOGD("Other types do not need to processed %d", node.GetType());
        return;
    }
    auto nodeParent = node.GetParent().lock();
    std::shared_ptr<RSRenderNode> rsParent = nullptr;
    if (nodeParent == nullptr) {
        return;
    }
    rsParent = nodeParent->ReinterpretCastTo<RSRenderNode>();
    auto& property = node.shared_from_this()->ReinterpretCastTo<RSRenderNode>()->GetMutableRenderProperties();
    auto geoPtr = std::static_pointer_cast<RSObjAbsGeometry>(property.GetBoundsGeometry());
    RectI rect;
    if (geoPtr != nullptr) {
        rect = geoPtr->GetAbsRect();
    }
    RectI parentRect;
    auto& parentProperty = rsParent->GetMutableRenderProperties();
    auto parentGeoPtr = std::static_pointer_cast<RSObjAbsGeometry>(parentProperty.GetBoundsGeometry());
    if (parentGeoPtr != nullptr) {
        parentRect = parentGeoPtr->GetAbsRect();
    }
    if (node.HasChildrenOutOfRect()) {
        if (!node.GetPaintOutOfParentRect().IsInsideOf(parentRect) || parentRect.IsEmpty()) {
            nodeParent->UpdateChildrenOutOfRectFlag(true);
            nodeParent->UpdatePaintOutOfParentRect(node.GetPaintOutOfParentRect());
        }
    } else {
        if (!rect.IsInsideOf(parentRect) || parentRect.IsEmpty()) {
            nodeParent->UpdateChildrenOutOfRectFlag(true);
            nodeParent->UpdatePaintOutOfParentRect(rect);
        }
    }
}

void RSUniRenderVisitor::PrepareDisplayRenderNode(RSDisplayRenderNode& node)
{
    currentVisitDisplay_ = node.GetScreenId();
    displayHasSecSurface_.emplace(currentVisitDisplay_, false);
    dirtySurfaceNodeMap_.clear();

    RS_TRACE_NAME("RSUniRender:PrepareDisplay " + std::to_string(currentVisitDisplay_));
    curDisplayDirtyManager_ = node.GetDirtyManager();
    curDisplayDirtyManager_->Clear();
    curDisplayNode_ = node.shared_from_this()->ReinterpretCastTo<RSDisplayRenderNode>();

    dirtyFlag_ = isDirty_;

    node.ApplyModifiers();
    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    if (!screenManager) {
        RS_LOGE("RSUniRenderVisitor::ProcessDisplayRenderNode ScreenManager is nullptr");
        return;
    }
    screenInfo_ = screenManager->QueryScreenInfo(node.GetScreenId());

    parentSurfaceNodeMatrix_ = SkMatrix::I();
    auto geoPtr = std::static_pointer_cast<RSObjAbsGeometry>(node.GetRenderProperties().GetBoundsGeometry());
    if (geoPtr != nullptr) {
        geoPtr->UpdateByMatrixFromSelf();
        parentSurfaceNodeMatrix_ = geoPtr->GetAbsMatrix();
    }
    dirtyFlag_ |= node.IsRotationChanged();
    node.UpdateRotation();
    PrepareBaseRenderNode(node);
    auto mirrorNode = node.GetMirrorSource().lock();
    if (mirrorNode) {
        mirroredDisplays_.insert(mirrorNode->GetScreenId());
    }

    node.GetCurAllSurfaces().clear();
    node.CollectSurface(node.shared_from_this(), node.GetCurAllSurfaces(), true);
}

void RSUniRenderVisitor::PrepareSurfaceRenderNode(RSSurfaceRenderNode& node)
{
    if (node.GetSecurityLayer()) {
        displayHasSecSurface_[currentVisitDisplay_] = true;
    }
    node.CleanDstRectChanged();
    node.ApplyModifiers();
    bool dirtyFlag = dirtyFlag_;

    // update geoptr with ContextMatrix
    auto parentSurfaceNodeMatrix = parentSurfaceNodeMatrix_;
    auto& property = node.GetMutableRenderProperties();
    auto geoPtr = std::static_pointer_cast<RSObjAbsGeometry>(property.GetBoundsGeometry());

    // prepare the surfaceRenderNode whose child is rootRenderNode 
    if (node.IsAppWindow() || node.GetSurfaceNodeType() == RSSurfaceNodeType::STARTING_WINDOW_NODE) {
        curSurfaceDirtyManager_ = node.GetDirtyManager();
        curSurfaceDirtyManager_->Clear();
        if (auto parentNode = node.GetParent().lock()) {
            auto rsParent = RSBaseRenderNode::ReinterpretCast<RSRenderNode>(parentNode);
            dirtyFlag_ = node.Update(*curSurfaceDirtyManager_,
                &(rsParent->GetRenderProperties()), dirtyFlag_);
            if (rsParent->ReinterpretCastTo<RSSurfaceRenderNode>() &&
                rsParent->ReinterpretCastTo<RSSurfaceRenderNode>()->GetSurfaceNodeType() == RSSurfaceNodeType::LEASH_WINDOW_NODE) {
                node.SetGlobalAlpha(property.GetAlpha() * rsParent->GetMutableRenderProperties().GetAlpha()); // calculate alpha on parent's alpha
            } else {
                node.SetGlobalAlpha(property.GetAlpha());
            }
        } else {
            dirtyFlag_ = node.Update(*curSurfaceDirtyManager_, nullptr, dirtyFlag_);
        }
        geoPtr->ConcatMatrix(node.GetContextMatrix());
        node.SetDstRect(geoPtr->GetAbsRect().IntersectRect(RectI(0, 0, screenInfo_.width, screenInfo_.height)));
        curDisplayNode_->UpdateSurfaceNodePos(node.GetId(), node.GetDstRect());
    } else {
        RSUniRenderUtil::UpdateRenderNodeDstRect(node, node.GetContextMatrix());
        node.SetDstRect(geoPtr->GetAbsRect().IntersectRect(RectI(0, 0, screenInfo_.width, screenInfo_.height)));
        if (node.GetBuffer() != nullptr) {
            auto& surfaceHandler = static_cast<RSSurfaceHandler&>(node);
            if (surfaceHandler.IsCurrentFrameBufferConsumed()) {
                curSurfaceDirtyManager_->MergeDirtyRect(node.GetDstRect());
            }
        }
    }
    dirtyFlag_ |= node.GetDstRectChanged();
    parentSurfaceNodeMatrix_ = geoPtr->GetAbsMatrix();

    PrepareBaseRenderNode(node);
    // restore flags
    parentSurfaceNodeMatrix_ = parentSurfaceNodeMatrix;
    dirtyFlag_ = dirtyFlag;
    if (node.GetDirtyManager() && node.GetDirtyManager()->IsDirty()) {
        auto surfaceNodePtr = std::static_pointer_cast<RSSurfaceRenderNode>(node.shared_from_this());
        dirtySurfaceNodeMap_.emplace(surfaceNodePtr->GetId(), surfaceNodePtr);
    }
}

void RSUniRenderVisitor::PrepareProxyRenderNode(RSProxyRenderNode& node)
{
    auto rsParent = RSBaseRenderNode::ReinterpretCast<RSRenderNode>(node.GetParent().lock());
    if (rsParent == nullptr) {
        return;
    }
    auto& property = rsParent->GetMutableRenderProperties();
    auto geoPtr = std::static_pointer_cast<RSObjAbsGeometry>(property.GetBoundsGeometry());
    SkMatrix invertMatrix;
    SkMatrix contextMatrix = geoPtr->GetAbsMatrix();

    if (parentSurfaceNodeMatrix_.invert(&invertMatrix)) {
        contextMatrix.preConcat(invertMatrix);
    } else {
        ROSEN_LOGE("RSUniRenderVisitor::ProcessSurfaceRenderNode, invertMatrix failed");
    }
    node.SetContextMatrix(contextMatrix);

    PrepareBaseRenderNode(node);
}

void RSUniRenderVisitor::PrepareRootRenderNode(RSRootRenderNode& node)
{
    RSUniRenderUtil::UpdateRenderNodeDstRect(node, SkMatrix::I());
    PrepareCanvasRenderNode(node);
}

void RSUniRenderVisitor::PrepareCanvasRenderNode(RSCanvasRenderNode &node)
{
    node.ApplyModifiers();
    bool dirtyFlag = dirtyFlag_;
    auto nodeParent = node.GetParent().lock();
    while (nodeParent && nodeParent->ReinterpretCastTo<RSSurfaceRenderNode>() &&
        nodeParent->ReinterpretCastTo<RSSurfaceRenderNode>()->GetSurfaceNodeType() ==
        RSSurfaceNodeType::SELF_DRAWING_NODE) {
        nodeParent = nodeParent->GetParent().lock();
    }
    std::shared_ptr<RSRenderNode> rsParent = nullptr;
    if (nodeParent != nullptr) {
        rsParent = nodeParent->ReinterpretCastTo<RSRenderNode>();
    }
    dirtyFlag_ = node.Update(*curSurfaceDirtyManager_, rsParent ? &(rsParent->GetRenderProperties()) : nullptr,
        dirtyFlag_);
    if (node.IsDirtyRegionUpdated() && curSurfaceDirtyManager_->IsDebugRegionTypeEnable(DebugRegionType::CURRENT_SUB)) {
        curSurfaceDirtyManager_->UpdateDirtyCanvasNodes(node.GetId(), node.GetOldDirty());
    }
    PrepareBaseRenderNode(node);
    dirtyFlag_ = dirtyFlag;
}

void RSUniRenderVisitor::DrawRectOnCanvas(const RectI& dirtyRect, const SkColor color,
    const SkPaint::Style fillType, float alpha)
{
    ROSEN_LOGD("DrawRectOnCanvas current dirtyRect = [%d, %d, %d, %d]", dirtyRect.left_, dirtyRect.top_,
        dirtyRect.width_, dirtyRect.height_);
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

void RSUniRenderVisitor::DrawDirtyRegion()
{
    auto dirtyRect = RectI();
    const float fillAlpha = 0.2;
    const float edgeAlpha = 0.4;
    const float subFactor = 2.0;

    if (curSurfaceDirtyManager_->IsDebugRegionTypeEnable(DebugRegionType::MULTI_HISTORY)) {
        dirtyRect = curSurfaceDirtyManager_->GetDirtyRegion();
        if (dirtyRect.width_ <= 0 || dirtyRect.height_ <= 0) {
            ROSEN_LOGD("DrawDirtyRegion his dirty rect is invalid. dirtyRect = [%d, %d, %d, %d]",
                dirtyRect.left_, dirtyRect.top_, dirtyRect.width_, dirtyRect.height_); 
        } else {
            ROSEN_LOGD("DrawDirtyRegion his dirty rect");
            // green
            DrawRectOnCanvas(dirtyRect, 0xFF0AFF0A, SkPaint::kFill_Style, fillAlpha / subFactor);
            DrawRectOnCanvas(dirtyRect, 0xFF0AFF0A, SkPaint::kStroke_Style, edgeAlpha);
        }
    }

    if (curSurfaceDirtyManager_->IsDebugRegionTypeEnable(DebugRegionType::CURRENT_WHOLE)) {
        dirtyRect = curSurfaceDirtyManager_->GetLatestDirtyRegion();
        if (dirtyRect.width_ <= 0 || dirtyRect.height_ <= 0) {
            ROSEN_LOGD("DrawDirtyRegion dirty rect is invalid. dirtyRect = [%d, %d, %d, %d]", dirtyRect.left_,
                dirtyRect.top_, dirtyRect.width_, dirtyRect.height_);
        } else {
            ROSEN_LOGD("DrawDirtyRegion cur dirty rect");
            // yellow
            DrawRectOnCanvas(curSurfaceDirtyManager_->GetDirtyRegion(), 0xFFFFFF00, SkPaint::kFill_Style, fillAlpha);
            DrawRectOnCanvas(curSurfaceDirtyManager_->GetDirtyRegion(), 0xFFFFFF00, SkPaint::kStroke_Style, edgeAlpha);
        }
    }

    if (curSurfaceDirtyManager_->IsDebugRegionTypeEnable(DebugRegionType::CURRENT_SUB)) {
        std::map<NodeId, RectI> dirtySubRects_;
        curSurfaceDirtyManager_->GetDirtyCanvasNodes(dirtySubRects_);
        for (const auto& [nid, subRect] : dirtySubRects_) {
            ROSEN_LOGD("DrawDirtyRegion canvasNode id %d is dirty", nid);
            // red
            DrawRectOnCanvas(subRect, 0xFFFF0000, SkPaint::kStroke_Style, edgeAlpha / subFactor);
        }

        curSurfaceDirtyManager_->GetDirtySurfaceNodes(dirtySubRects_);
        for (const auto& [nid, subRect] : dirtySubRects_) {
            ROSEN_LOGD("DrawDirtyRegion surfaceNode id %d is dirty", nid);
            // light purple
            DrawRectOnCanvas(subRect, 0xFFD899D8, SkPaint::kStroke_Style, edgeAlpha);
        }
    }
}

void RSUniRenderVisitor::ProcessBaseRenderNode(RSBaseRenderNode& node)
{
    for (auto& child : node.GetSortedChildren()) {
        child->Process(shared_from_this());
    }
    // clear SortedChildren, it will be generated again in next frame
    node.ResetSortedChildren();
}

void RSUniRenderVisitor::ProcessDisplayRenderNode(RSDisplayRenderNode& node)
{
    RS_TRACE_NAME("ProcessDisplayRenderNode[" + std::to_string(node.GetScreenId()) + "]" + 
        node.GetDirtyManager()->GetDirtyRegion().ToString().c_str());
    RS_LOGD("RSUniRenderVisitor::ProcessDisplayRenderNode node: %" PRIu64 ", child size:%u", node.GetId(),
        node.GetChildrenCount());
    isSecurityDisplay_ = node.GetSecurityDisplay();
    switch (screenInfo_.state) {
        case ScreenState::PRODUCER_SURFACE_ENABLE:
            node.SetCompositeType(RSDisplayRenderNode::CompositeType::UNI_RENDER_MIRROR_COMPOSITE);
            break;
        case ScreenState::HDI_OUTPUT_ENABLE:
            node.SetCompositeType(node.IsForceSoftComposite() ?
                RSDisplayRenderNode::CompositeType::SOFTWARE_COMPOSITE :
                RSDisplayRenderNode::CompositeType::UNI_RENDER_COMPOSITE);
            break;
        default:
            RS_LOGE("RSUniRenderVisitor::ProcessDisplayRenderNode ScreenState unsupported");
            return;
    }
    offsetX_ = node.GetDisplayOffsetX();
    offsetY_ = node.GetDisplayOffsetY();
    processor_ = RSProcessorFactory::CreateProcessor(node.GetCompositeType());
    if (processor_ == nullptr) {
        RS_LOGE("RSUniRenderVisitor::ProcessDisplayRenderNode: RSProcessor is null!");
        return;
    }
    auto mirrorNode = node.GetMirrorSource().lock();
    if (!processor_->Init(node, node.GetDisplayOffsetX(), node.GetDisplayOffsetY(),
        mirrorNode ? mirrorNode->GetScreenId() : INVALID_SCREEN_ID)) {
        RS_LOGE("RSUniRenderVisitor::ProcessDisplayRenderNode: processor init failed!");
        return;
    }
    std::shared_ptr<RSBaseRenderNode> nodePtr = node.shared_from_this();
    auto displayNodePtr = nodePtr->ReinterpretCastTo<RSDisplayRenderNode>();
    if (!displayNodePtr) {
        RS_LOGE("RSUniRenderVisitor::ProcessDisplayRenderNode ReinterpretCastTo fail");
        return;
    }
    if (!node.IsSurfaceCreated()) {
        sptr<IBufferConsumerListener> listener = new RSUniRenderListener(displayNodePtr);
        if (!node.CreateSurface(listener)) {
            RS_LOGE("RSUniRenderVisitor::ProcessDisplayRenderNode CreateSurface failed");
            return;
        }
    }

    if (mirrorNode) {
        auto processor = std::static_pointer_cast<RSUniRenderMirrorProcessor>(processor_);
        if (displayHasSecSurface_[mirrorNode->GetScreenId()] && mirrorNode->GetSecurityDisplay() != isSecurityDisplay_
            && processor) {
            canvas_ = processor->GetCanvas();
            ProcessBaseRenderNode(*mirrorNode);
        } else {
            processor_->ProcessDisplaySurface(*mirrorNode);
        }
    } else {
        auto rsSurface = node.GetRSSurface();
        if (rsSurface == nullptr) {
            RS_LOGE("RSUniRenderVisitor::ProcessDisplayRenderNode No RSSurface found");
            return;
        }
        // we should request a framebuffer whose size is equals to the physical screen size.
        auto renderFrame = renderEngine_->RequestFrame(std::static_pointer_cast<RSSurfaceOhos>(rsSurface),
            RSBaseRenderUtil::GetFrameBufferRequestConfig(screenInfo_, true));
        if (renderFrame == nullptr) {
            RS_LOGE("RSUniRenderVisitor Request Frame Failed");
            return;
        }
        canvas_ = renderFrame->GetCanvas();
        if (canvas_ == nullptr) {
            RS_LOGE("RSUniRenderVisitor::ProcessDisplayRenderNode: failed to create canvas");
            return;
        }
        int saveLayerCnt = 0;
        SkRegion region;
#ifdef RS_ENABLE_EGLQUERYSURFACE
        // Get displayNode buffer age in order to merge visible dirty region for displayNode.
        // And then set egl damage region to improve uni_render efficiency.
        if (isPartialRenderEnabled_) {
            curDisplayDirtyManager_->SetSurfaceSize(screenInfo_.width, screenInfo_.height);
            CalcDirtyDisplayRegion(displayNodePtr);
            int bufferAge = renderFrame->GetBufferAge();
            RSUniRenderUtil::MergeDirtyHistory(displayNodePtr, bufferAge);
            auto dirtyRegion = RSUniRenderUtil::MergeVisibleDirtyRegion(displayNodePtr);
            SetSurfaceGlobalDirtyRegion(displayNodePtr);
            std::vector<RectI> rects = GetDirtyRects(dirtyRegion);
            RectI rect = node.GetDirtyManager()->GetDirtyRegionFlipWithinSurface();
            if (!rect.IsEmpty()) {
                rects.emplace_back(rect);
            }
            auto disH = screenInfo_.GetRotatedHeight();
            for (auto& r : rects) {
                region.op(SkIRect::MakeXYWH(r.left_, disH - r.GetBottom(), r.width_, r.height_), SkRegion::kUnion_Op);
                RS_LOGD("SetDamageRegion %s", r.ToString().c_str());
            }
            renderFrame->SetDamageRegion(rects);
        }
        if (isOpDropped_ && !region.isEmpty()) {
            if (region.isRect()) {
                canvas_->clipRegion(region);
            } else {
                SkPath dirtyPath;
                region.getBoundaryPath(&dirtyPath);
                canvas_->clipPath(dirtyPath, true);
                saveLayerCnt = canvas_->saveLayer(SkRect::MakeWH(screenInfo_.width, screenInfo_.height), nullptr);
            }
        }
#endif
        canvas_->save();
        auto geoPtr = std::static_pointer_cast<RSObjAbsGeometry>(node.GetRenderProperties().GetBoundsGeometry());
        if (geoPtr != nullptr) {
            canvas_->concat(geoPtr->GetMatrix());
        }

        ProcessBaseRenderNode(node);
        canvas_->restore();

        // the following code makes DirtyRegion visible, enable this method by turning on the dirtyregiondebug property
        for (auto [id, surfaceNode] : dirtySurfaceNodeMap_) {
            if (surfaceNode->GetDirtyManager()) {
                curSurfaceDirtyManager_ = surfaceNode->GetDirtyManager();
                if (curSurfaceDirtyManager_->IsDirty() && curSurfaceDirtyManager_->IsDebugEnabled()) {
                    DrawDirtyRegion();
                }
            }
        }
        if (saveLayerCnt > 0) {
            RS_TRACE_NAME("RSUniRender:RestoreLayer");
            canvas_->restoreToCount(saveLayerCnt);
        }

        RS_TRACE_BEGIN("RSUniRender:FlushFrame");
        renderFrame->Flush();
        RS_TRACE_END();
        RS_TRACE_BEGIN("RSUniRender:WaitUtilUniRenderFinished");
        RSMainThread::Instance()->WaitUtilUniRenderFinished();
        RS_TRACE_END();
        if (mirroredDisplays_.size() > 0) {
            node.MakeSnapshot(canvas_->GetSurface());
        }
        processor_->ProcessDisplaySurface(node);
    }
    processor_->PostProcess();

    // We should release DisplayNode's surface buffer after PostProcess(),
    // since the buffer's releaseFence was set in PostProcess().
    auto& surfaceHandler = static_cast<RSSurfaceHandler&>(node);
    (void)RSBaseRenderUtil::ReleaseBuffer(surfaceHandler);
    RS_LOGD("RSUniRenderVisitor::ProcessDisplayRenderNode end");
}

void RSUniRenderVisitor::CalcDirtyDisplayRegion(std::shared_ptr<RSDisplayRenderNode>& node) const
{
    RS_TRACE_FUNC();
    auto displayDirtyManager = node->GetDirtyManager();
    for (auto it = node->GetCurAllSurfaces().rbegin(); it != node->GetCurAllSurfaces().rend(); ++it) {
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*it);
        if (surfaceNode == nullptr) {
            continue;
        }
        auto surfaceDirtyManager = surfaceNode->GetDirtyManager();
        RectI surfaceDirtyRect = surfaceDirtyManager->GetDirtyRegion();
        const uint8_t opacity = 255;
        if (surfaceNode->GetAbilityBgAlpha() != opacity ||
            !ROSEN_EQ(surfaceNode->GetRenderProperties().GetAlpha(), 1.0f)) {
            // Handles the case of transparent surface, merge transparent dirty rect
            RectI transparentDirtyRect = surfaceNode->GetDstRect().IntersectRect(surfaceDirtyRect);
            if (!transparentDirtyRect.IsEmpty()) {
                RS_LOGD("CalcDirtyDisplayRegion merge transparent dirty rect %s rect %s",
                    surfaceNode->GetName().c_str(), transparentDirtyRect.ToString().c_str());
                displayDirtyManager->MergeDirtyRect(transparentDirtyRect);
            }
        }

        if (surfaceNode->GetZorderChanged()) {
            // Zorder changed case, merge surface dest Rect
            RS_LOGD("CalcDirtyDisplayRegion merge GetZorderChanged %s rect %s", surfaceNode->GetName().c_str(),
                surfaceNode->GetDstRect().ToString().c_str());
            displayDirtyManager->MergeDirtyRect(surfaceNode->GetDstRect());
        }

        RectI lastFrameSurfacePos = node->GetLastFrameSurfacePos(surfaceNode->GetId());
        RectI currentFrameSurfacePos = node->GetCurrentFrameSurfacePos(surfaceNode->GetId());
        if (lastFrameSurfacePos != currentFrameSurfacePos) {
            RS_LOGD("CalcDirtyDisplayRegion merge surface pos changed %s lastFrameRect %s currentFrameRect %s",
                surfaceNode->GetName().c_str(), lastFrameSurfacePos.ToString().c_str(),
                currentFrameSurfacePos.ToString().c_str());
            if (!lastFrameSurfacePos.IsEmpty()) {
                displayDirtyManager->MergeDirtyRect(lastFrameSurfacePos);
            }
            if (!currentFrameSurfacePos.IsEmpty()) {
                displayDirtyManager->MergeDirtyRect(currentFrameSurfacePos);
            }
        }
    }
    std::vector<RectI> surfaceChangedRects = node->GetSurfaceChangedRects();
    for (auto& surfaceChangedRect : surfaceChangedRects) {
        RS_LOGD("CalcDirtyDisplayRegion merge Surface closed %s", surfaceChangedRect.ToString().c_str());
        if (!surfaceChangedRect.IsEmpty()) {
            displayDirtyManager->MergeDirtyRect(surfaceChangedRect);
        }
    }
}

void RSUniRenderVisitor::SetSurfaceGlobalDirtyRegion(std::shared_ptr<RSDisplayRenderNode>& node)
{
    RS_TRACE_FUNC();
    for (auto it = node->GetCurAllSurfaces().rbegin(); it != node->GetCurAllSurfaces().rend(); ++it) {
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*it);
        if (surfaceNode == nullptr || !surfaceNode->IsAppWindow()) {
            continue;
        }
        // set display dirty region to surfaceNode
        surfaceNode->SetGloblDirtyRegion(node->GetDirtyManager()->GetDirtyRegion());
    }
    Occlusion::Region curVisibleDirtyRegion;
    for (auto it = node->GetCurAllSurfaces().begin(); it != node->GetCurAllSurfaces().end(); ++it) {
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*it);
        if (surfaceNode == nullptr || !surfaceNode->IsAppWindow()) {
            continue;
        }
        // set display dirty region to surfaceNode
        surfaceNode->SetDirtyRegionBelowCurrentLayer(curVisibleDirtyRegion);
        auto visibleDirtyRegion = surfaceNode->GetVisibleDirtyRegion();
        curVisibleDirtyRegion = curVisibleDirtyRegion.Or(visibleDirtyRegion);
    }
}

#ifdef RS_ENABLE_EGLQUERYSURFACE
std::vector<RectI> RSUniRenderVisitor::GetDirtyRects(const Occlusion::Region &region)
{
    std::vector<Occlusion::Rect> rects = region.GetRegionRects();
    std::vector<RectI> retRects;
    for (const Occlusion::Rect& rect : rects) {
        // origin transformation
        retRects.emplace_back(RectI(rect.left_, screenInfo_.GetRotatedHeight() - rect.bottom_,
            rect.right_ - rect.left_, rect.bottom_ - rect.top_));
    }
    RS_LOGD("GetDirtyRects size %d %s", region.GetSize(), region.GetRegionInfo().c_str());
    return retRects;
}
#endif

void RSUniRenderVisitor::InitCacheSurface(RSSurfaceRenderNode& node, int width, int height)
{
#if (defined RS_ENABLE_GL) && (defined RS_ENABLE_EGLIMAGE)
    SkImageInfo info = SkImageInfo::MakeN32Premul(width, height);
    node.SetCacheSurface(SkSurface::MakeRenderTarget(canvas_->getGrContext(), SkBudgeted::kYes, info));
#else
    node.SetCacheSurface(SkSurface::MakeRasterN32Premul(width, height));
#endif
}

void RSUniRenderVisitor::DrawCacheSurface(RSSurfaceRenderNode& node)
{
    canvas_->save();
    canvas_->scale(
        node.GetRenderProperties().GetBoundsWidth() / node.GetCacheSurface()->width(),
        node.GetRenderProperties().GetBoundsHeight() / node.GetCacheSurface()->height());
    SkPaint paint;
    node.GetCacheSurface()->draw(canvas_.get(), 0.0, 0.0, &paint);
    canvas_->restore();
}

void RSUniRenderVisitor::ProcessSurfaceRenderNode(RSSurfaceRenderNode& node)
{
    RS_TRACE_NAME("RSUniRender::Process:[" + node.GetName() + "]" + node.GetDstRect().ToString());
    RS_LOGD("RSUniRenderVisitor::ProcessSurfaceRenderNode node: %" PRIu64 ", child size:%u %s", node.GetId(),
        node.GetChildrenCount(), node.GetName().c_str());
    node.UpdatePositionZ();
    if (isSecurityDisplay_ && node.GetSecurityLayer()) {
        RS_TRACE_NAME("SecurityLayer Skip");
        return;
    }
    const auto& property = node.GetRenderProperties();
    if (!property.GetVisible()) {
        RS_LOGD("RSUniRenderVisitor::ProcessSurfaceRenderNode node: %" PRIu64 " invisible", node.GetId());
        return;
    }
    if (!node.GetOcclusionVisible() && !doAnimate_ && RSSystemProperties::GetOcclusionEnabled()) {
        RS_TRACE_NAME("Occlusion Skip");
        return;
    }
#ifdef RS_ENABLE_EGLQUERYSURFACE
    // skip clean surface node
    if (isOpDropped_ && node.IsAppWindow()) {
        if (!node.SubNodeNeedDraw(node.GetDstRect(), partialRenderType_)) {
            RS_TRACE_NAME("QuickReject Skip");
            RS_LOGD("RSUniRenderVisitor::ProcessSurfaceRenderNode skip: %s", node.GetName().c_str());
            return;
        }
        curSurfaceNode_ = node.ReinterpretCastTo<RSSurfaceRenderNode>();
        curSurfaceDirtyManager_ = node.GetDirtyManager();
    }
#endif

    if (!canvas_) {
        RS_LOGE("RSUniRenderVisitor::ProcessSurfaceRenderNode, canvas is nullptr");
        return;
    }
    auto geoPtr = std::static_pointer_cast<RSObjAbsGeometry>(property.GetBoundsGeometry());
    if (!geoPtr) {
        RS_LOGE("RSUniRenderVisitor::ProcessSurfaceRenderNode node:%" PRIu64 ", get geoPtr failed", node.GetId());
        return;
    }
    canvas_->save();
    canvas_->SaveAlpha();

    canvas_->MultiplyAlpha(property.GetAlpha());
    canvas_->MultiplyAlpha(node.GetContextAlpha());

    bool isSelfDrawingSurface = node.GetSurfaceNodeType() == RSSurfaceNodeType::SELF_DRAWING_NODE;
    if (isSelfDrawingSurface) {
        canvas_->save();
    }

    canvas_->concat(geoPtr->GetMatrix());

    const RectF absBounds = {0, 0, property.GetBoundsWidth(), property.GetBoundsHeight()};
    RRect absClipRRect = RRect(absBounds, property.GetCornerRadius());
    RSPropertiesPainter::DrawShadow(property, *canvas_, &absClipRRect);

    if (isSelfDrawingSurface) {
        canvas_->save();
    }

    boundsRect_ = SkRect::MakeWH(property.GetBoundsWidth(), property.GetBoundsHeight());
    frameGravity_ = property.GetFrameGravity();
    if (!property.GetCornerRadius().IsZero()) {
        canvas_->clipRRect(RSPropertiesPainter::RRect2SkRRect(absClipRRect), true);
    } else {
        canvas_->clipRect(boundsRect_);
    }

    RSPropertiesPainter::DrawBackground(property, *canvas_);
    auto filter = std::static_pointer_cast<RSSkiaFilter>(property.GetBackgroundFilter());
    if (filter != nullptr) {
        auto skRectPtr = std::make_unique<SkRect>();
        skRectPtr->setXYWH(0, 0, property.GetBoundsWidth(), property.GetBoundsHeight());
        RSPropertiesPainter::DrawFilter(property, *canvas_, filter, skRectPtr, canvas_->GetSurface());
    }

    if (isSelfDrawingSurface) {
        canvas_->restore();
    }

    node.SetTotalMatrix(canvas_->getTotalMatrix());

    if ((!node.IsAppWindow() || node.GetName() == "pointer window") && node.GetBuffer() != nullptr) {
        node.NotifyRTBufferAvailable();
        node.SetGlobalAlpha(1.0f);
        // use node's local coordinate.
        auto params = RSBaseRenderUtil::CreateBufferDrawParam(node, true, false, false, false);
        renderEngine_->DrawSurfaceNodeWithParams(*canvas_, node, params);
    }

    if (isSelfDrawingSurface) {
        canvas_->restore();
    }

    if (node.IsAppWindow()) {
        if (!node.IsAppFreeze()) {
            ProcessBaseRenderNode(node);
            node.ClearCacheSurface();
        } else if (node.GetCacheSurface()) {
            DrawCacheSurface(node);
        } else {
            InitCacheSurface(node, property.GetBoundsWidth(), property.GetBoundsHeight());
            if (node.GetCacheSurface()) {
                auto cacheCanvas = std::make_unique<RSPaintFilterCanvas>(node.GetCacheSurface().get());

                swap(cacheCanvas, canvas_);
                ProcessBaseRenderNode(node);
                swap(cacheCanvas, canvas_);

                DrawCacheSurface(node);
            } else {
                RS_LOGE("RSUniRenderVisitor::ProcessSurfaceRenderNode %s Create CacheSurface failed",
                    node.GetName().c_str());
            }
        }
    } else {
        ProcessBaseRenderNode(node);
    }

    filter = std::static_pointer_cast<RSSkiaFilter>(property.GetFilter());
    if (filter != nullptr) {
        auto skRectPtr = std::make_unique<SkRect>();
        skRectPtr->setXYWH(0, 0, property.GetBoundsWidth(), property.GetBoundsHeight());
        RSPropertiesPainter::DrawFilter(property, *canvas_, filter, skRectPtr, canvas_->GetSurface());
    }

    canvas_->RestoreAlpha();
    canvas_->restore();
}

void RSUniRenderVisitor::ProcessProxyRenderNode(RSProxyRenderNode& node)
{
    if (!canvas_) {
        ROSEN_LOGE("RSRenderThreadVisitor::ProcessProxyRenderNode, canvas is nullptr");
        return;
    }
    if (!node.GetRenderProperties().GetVisible()) {
        ROSEN_LOGI("RSRenderThreadVisitor::ProcessProxyRenderNode node : %" PRIu64 " is invisible", node.GetId());
        node.SetContextAlpha(0.0f);
        return;
    }
#ifdef ROSEN_OHOS
    node.SetContextAlpha(canvas_->GetAlpha());

    node.ResetSortedChildren();
#endif
}

void RSUniRenderVisitor::ProcessRootRenderNode(RSRootRenderNode& node)
{
    RS_LOGD("RSUniRenderVisitor::ProcessRootRenderNode node: %" PRIu64 ", child size:%u", node.GetId(),
        node.GetChildrenCount());
    const auto& property = node.GetRenderProperties();
    if (!property.GetVisible()) {
        RS_LOGD("RSUniRenderVisitor::ProcessRootRenderNode, no need process");
        return;
    }

    if (!canvas_) {
        RS_LOGE("RSUniRenderVisitor::ProcessRootRenderNode, canvas is nullptr");
        return;
    }

    canvas_->SetHighContrast(renderEngine_->isHighContrastEnabled());
    ColorFilterMode modeFromSysProperty = static_cast<ColorFilterMode>(RSSystemProperties::GetCorrectionMode());
    ColorFilterMode colorFilterMode = renderEngine_->GetColorFilterMode();
    if (RSBaseRenderUtil::IsColorFilterModeValid(modeFromSysProperty)) {
        colorFilterMode = modeFromSysProperty;
    }
    if (colorFilterMode >= ColorFilterMode::INVERT_COLOR_ENABLE_MODE &&
        colorFilterMode <= ColorFilterMode::INVERT_DALTONIZATION_TRITANOMALY_MODE) {
        RS_LOGD("RsDebug RSRenderEngine::SetColorFilterModeToPaint mode:%d", static_cast<int32_t>(colorFilterMode));
        SkPaint paint;
        RSBaseRenderUtil::SetColorFilterModeToPaint(colorFilterMode, paint);
        canvas_->saveLayer(nullptr, &paint);
    } else {
        canvas_->save();
    }
    if (node.GetParent().lock() == curSurfaceNode_) {
        const float rootWidth = property.GetFrameWidth() * property.GetScaleX();
        const float rootHeight = property.GetFrameHeight() * property.GetScaleY();
        SkMatrix gravityMatrix;
        (void)RSPropertiesPainter::GetGravityMatrix(frameGravity_,
            RectF { 0.0f, 0.0f, boundsRect_.width(), boundsRect_.height() }, rootWidth, rootHeight, gravityMatrix);
        canvas_->concat(gravityMatrix);
    }
    ProcessCanvasRenderNode(node);
    canvas_->restore();
}

void RSUniRenderVisitor::ProcessCanvasRenderNode(RSCanvasRenderNode& node)
{
    if (!node.GetRenderProperties().GetVisible()) {
        RS_LOGD("RSUniRenderVisitor::ProcessCanvasRenderNode, no need process");
        return;
    }
#ifdef RS_ENABLE_EGLQUERYSURFACE
    if (isOpDropped_ && !curSurfaceNode_->SubNodeNeedDraw(node.GetOldDirty(), partialRenderType_) &&
        !node.HasChildrenOutOfRect()) {
        return;
    }
#endif
    if (!canvas_) {
        RS_LOGE("RSUniRenderVisitor::ProcessCanvasRenderNode, canvas is nullptr");
        return;
    }
    node.ProcessRenderBeforeChildren(*canvas_);
    ProcessBaseRenderNode(node);
    node.ProcessRenderAfterChildren(*canvas_);
}
} // namespace Rosen
} // namespace OHOS
