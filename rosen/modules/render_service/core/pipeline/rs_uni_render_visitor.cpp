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

#include "common/rs_obj_abs_geometry.h"
#include "pipeline/rs_base_render_util.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_processor_factory.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_uni_render_listener.h"
#include "pipeline/rs_uni_render_util.h"
#include "pipeline/rs_uni_render_mirror_processor.h"
#include "platform/common/rs_log.h"
#include "property/rs_properties_painter.h"
#include "property/rs_transition_properties.h"
#include "rs_trace.h"
#include "render/rs_skia_filter.h"

namespace OHOS {
namespace Rosen {
RSUniRenderVisitor::RSUniRenderVisitor()
    : curSurfaceDirtyManager_(std::make_shared<RSDirtyRegionManager>())
{
    auto mainThread = RSMainThread::Instance();
    renderEngine_ = mainThread->GetRenderEngine();
}
RSUniRenderVisitor::~RSUniRenderVisitor() {}

void RSUniRenderVisitor::PrepareBaseRenderNode(RSBaseRenderNode& node)
{
    for (auto& child : node.GetSortedChildren()) {
        child->Prepare(shared_from_this());
    }
}

void RSUniRenderVisitor::PrepareDisplayRenderNode(RSDisplayRenderNode& node)
{
    currentVisitDisplay_ = node.GetScreenId();
    displayHasSecSurface_.emplace(currentVisitDisplay_, false);
    dirtySurfaceNodeMap_.clear();

    curDisplayDirtyManager_ = node.GetDirtyManager();
    curDisplayDirtyManager_->Clear();
    curDisplayNode_ = node.shared_from_this()->ReinterpretCastTo<RSDisplayRenderNode>();

    dirtyFlag_ = false;
    node.ApplyModifiers();
    PrepareBaseRenderNode(node);
    auto mirrorNode = node.GetMirrorSource().lock();
    if (mirrorNode) {
        mirroredDisplays_.insert(mirrorNode->GetScreenId());
    }
}

void RSUniRenderVisitor::PrepareSurfaceRenderNode(RSSurfaceRenderNode& node)
{
    if (node.GetSecurityLayer()) {
        displayHasSecSurface_[currentVisitDisplay_] = true;
    }
    node.ApplyModifiers();
    RSUniRenderUtil::UpdateRenderNodeDstRect(node);
    // prepare the surfaceRenderNode whose child is rootRenderNode 
    if (node.IsAppWindow()) {
        curSurfaceDirtyManager_ = node.GetDirtyManager();
        curSurfaceDirtyManager_->Clear();
        dirtyFlag_ = false;
        curDisplayNode_->UpdateSurfaceNodePos(node.GetId(), node.GetDstRect());
        if (node.GetDstRectChanged()) {
            curDisplayDirtyManager_->MergeDirtyRect(curDisplayNode_->GetLastFrameSurfacePos(node.GetId()));
        }
    } else {
        if (node.GetBuffer() != nullptr) {
            auto& surfaceHandler = static_cast<RSSurfaceHandler&>(node);
            if (surfaceHandler.IsCurrentFrameBufferConsumed() && GetSurfaceViewDirtyEnabled()) {
                curSurfaceDirtyManager_->MergeDirtyRect(node.GetDstRect());
            }
        }
    }

    if (node.GetDstRectChanged()) {
        dirtyFlag_ = true;
    }

    PrepareBaseRenderNode(node);
    if (node.GetDirtyManager() && node.GetDirtyManager()->IsDirty()) {
        std::shared_ptr<RSBaseRenderNode> nodePtr = node.shared_from_this();
        auto surfaceNodePtr = nodePtr->ReinterpretCastTo<RSSurfaceRenderNode>();
        dirtySurfaceNodeMap_.insert(std::make_pair(surfaceNodePtr->GetId(), surfaceNodePtr));
    }
}

void RSUniRenderVisitor::PrepareRootRenderNode(RSRootRenderNode& node)
{
    RSUniRenderUtil::UpdateRenderNodeDstRect(node);
    PrepareCanvasRenderNode(node);
}

void RSUniRenderVisitor::PrepareCanvasRenderNode(RSCanvasRenderNode &node)
{
    node.ApplyModifiers();
    bool dirtyFlag = dirtyFlag_;
    auto nodeParent = node.GetParent().lock();
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
            ROSEN_LOGD("DrawDirtyRegion dirty rect is invalid. dirtyRect = [%d, %d, %d, %d]",
                dirtyRect.left_, dirtyRect.top_, dirtyRect.width_, dirtyRect.height_);
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
    RS_LOGD("RSUniRenderVisitor::ProcessDisplayRenderNode node: %" PRIu64 ", child size:%u", node.GetId(),
        node.GetChildrenCount());
    isSecurityDisplay_ = node.GetSecurityDisplay();
    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    if (!screenManager) {
        RS_LOGE("RSUniRenderVisitor::ProcessDisplayRenderNode ScreenManager is nullptr");
        return;
    }
    screenInfo_ = screenManager->QueryScreenInfo(node.GetScreenId());
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

#ifdef RS_ENABLE_EGLQUERYSURFACE
        // Get displayNode buffer age in order to merge visible dirty region for displayNode.
        // And then set egl damage region to improve uni_render efficiency.
        if (RSSystemProperties::GetUniPartialRenderEnabled()) {
            uint32_t bufferAge = renderFrame->GetBufferAge();
            RS_LOGD("RSUniRenderVisitor buffer age is %d", bufferAge);
            auto dirtyRegion = RSUniRenderUtil::MergeVisibleDirtyRegion(displayNodePtr, bufferAge);
            std::vector<RectI> rects = GetDirtyRects(dirtyRegion);
            node.UpdateDisplayDirtyManager(bufferAge);
            RectI rect = CoordinateTransform(node.GetDirtyManager()->GetDirtyRegion());
            rects.push_back(rect);
            renderFrame->SetDamageRegion(rects);
        }
#endif
        canvas_ = renderFrame->GetCanvas();
        if (canvas_ == nullptr) {
            RS_LOGE("RSUniRenderVisitor::ProcessDisplayRenderNode: failed to create canvas");
            return;
        }
        canvas_->clear(SK_ColorTRANSPARENT);
        auto geoPtr = std::static_pointer_cast<RSObjAbsGeometry>(node.GetRenderProperties().GetBoundsGeometry());
        if (geoPtr != nullptr) {
            geoPtr->UpdateByMatrixFromSelf();
            canvas_->concat(geoPtr->GetMatrix());
        }

        ProcessBaseRenderNode(node);

        // the following code makes DirtyRegion visible, enable this method by turning on the dirtyregiondebug property 
        for (auto [id, surfaceNode] : dirtySurfaceNodeMap_) {
            if (surfaceNode->GetDirtyManager()) {
                curSurfaceDirtyManager_ = surfaceNode->GetDirtyManager();
                if (curSurfaceDirtyManager_->IsDirty() && curSurfaceDirtyManager_->IsDebugEnabled()) {
                    DrawDirtyRegion();
                }
            }
        }

        RS_TRACE_BEGIN("RSUniRender:FlushFrame");
        renderFrame->Flush();
        RS_TRACE_END();
        RS_TRACE_BEGIN("RSUniRender:WaitUtilUniRenderFinished");
        RSMainThread::Instance()->WaitUtilUniRenderFinished();
        RS_TRACE_END();
        processor_->ProcessDisplaySurface(node);
    }
    processor_->PostProcess();

    // We should release DisplayNode's surface buffer after PostProcess(),
    // since the buffer's releaseFence was set in PostProcess().
    auto& surfaceHandler = static_cast<RSSurfaceHandler&>(node);
    (void)RSBaseRenderUtil::ReleaseBuffer(surfaceHandler);
    RS_LOGD("RSUniRenderVisitor::ProcessDisplayRenderNode end");
}

#ifdef RS_ENABLE_EGLQUERYSURFACE
std::vector<RectI> RSUniRenderVisitor::GetDirtyRects(const Occlusion::Region &region)
{
    std::vector<Occlusion::Rect> rects = region.GetRegionRects();
    std::vector<RectI> retRects;
    for (Occlusion::Rect rect : rects) {
        // origin transformation
        retRects.emplace_back(RectI(rect.left_, screenInfo_.GetRotatedHeight() - rect.bottom_,
            rect.right_ - rect.left_, rect.bottom_ - rect.top_));
    }
    RS_LOGD("GetDirtyRects size %d", region.GetSize());
    return retRects;
}

RectI RSUniRenderVisitor::CoordinateTransform(const RectI& rect)
{
    RectI resRect = rect;
    resRect.top_ = screenInfo_.GetRotatedHeight() - rect.top_ - rect.height_;
    return resRect;
}
#endif

void RSUniRenderVisitor::ProcessSurfaceRenderNode(RSSurfaceRenderNode& node)
{
    RS_LOGD("RSUniRenderVisitor::ProcessSurfaceRenderNode node: %" PRIu64 ", child size:%u %s", node.GetId(),
        node.GetChildrenCount(), node.GetName().c_str());
    if (isSecurityDisplay_ && node.GetSecurityLayer()) {
        return;
    }
    const auto& property = node.GetRenderProperties();
    if (!property.GetVisible()) {
        RS_LOGD("RSUniRenderVisitor::ProcessSurfaceRenderNode node: %" PRIu64 " invisible", node.GetId());
        return;
    }
    if (!node.GetOcclusionVisible() && !doAnimate_ && RSSystemProperties::GetOcclusionEnabled()) {
        return;
    }

    if (!canvas_) {
        RS_LOGE("RSUniRenderVisitor::ProcessSurfaceRenderNode, canvas is nullptr");
        return;
    }
    auto geoPtr = std::static_pointer_cast<RSObjAbsGeometry>(property.GetBoundsGeometry());
    if (!geoPtr) {
        RS_LOGE("RSUniRenderVisitor::ProcessSurfaceRenderNode node:%" PRIu64 ", get geoPtr failed", node.GetId());
        return;
    }
    auto& dstRect = node.GetDstRect();
    RS_TRACE_NAME("RSUniRender::Process:" + node.GetName() + "_" + std::to_string(dstRect.left_) + " " +
        std::to_string(dstRect.top_) + " " + std::to_string(dstRect.width_) + " " + std::to_string(dstRect.height_));
    canvas_->save();
    canvas_->SaveAlpha();

    canvas_->MultiplyAlpha(property.GetAlpha());

    auto parentPtr = node.GetParent().lock();
    if (parentPtr && parentPtr->IsInstanceOf<RSCanvasRenderNode>()) {
        canvas_->save();
    }

    canvas_->concat(geoPtr->GetMatrix());

    const RectF absBounds = {
        node.GetTotalMatrix().getTranslateX(), node.GetTotalMatrix().getTranslateY(),
        property.GetBoundsWidth(), property.GetBoundsHeight()};
    RRect absClipRRect = RRect(absBounds, property.GetCornerRadius());
    RSPropertiesPainter::DrawShadow(property, *canvas_, &absClipRRect);

    canvas_->save();
    auto transitionProperties = node.GetAnimationManager().GetTransitionProperties();
    RSPropertiesPainter::DrawTransitionProperties(transitionProperties, property, *canvas_);
    boundsRect_ = SkRect::MakeWH(property.GetBoundsWidth(), property.GetBoundsHeight());
    frameGravity_ = property.GetFrameGravity();
    if (!property.GetCornerRadius().IsZero()) {
        canvas_->clipRRect(RSPropertiesPainter::RRect2SkRRect(absClipRRect), true);
    } else {
        canvas_->clipRect(boundsRect_);
    }
    auto backgroundColor = static_cast<SkColor>(property.GetBackgroundColor().AsArgbInt());
    if (SkColorGetA(backgroundColor) != SK_AlphaTRANSPARENT) {
        canvas_->drawColor(backgroundColor);
    }
    auto filter = std::static_pointer_cast<RSSkiaFilter>(property.GetBackgroundFilter());
    if (filter != nullptr) {
        auto skRectPtr = std::make_unique<SkRect>();
        skRectPtr->setXYWH(0, 0, property.GetBoundsWidth(), property.GetBoundsHeight());
        RSPropertiesPainter::DrawFilter(property, *canvas_, filter, skRectPtr, canvas_->GetSurface());
    }
    canvas_->restore();

    node.SetTotalMatrix(canvas_->getTotalMatrix());

    if (!node.IsAppWindow() && node.GetBuffer() != nullptr) {
        node.NotifyRTBufferAvailable();
        // use node's local coordinate.
        auto params = RSBaseRenderUtil::CreateBufferDrawParam(node, true, false, false, false);
        renderEngine_->DrawSurfaceNodeWithParams(*canvas_, node, params);
    }
    
    filter = std::static_pointer_cast<RSSkiaFilter>(property.GetFilter());
    if (filter != nullptr) {
        canvas_->clipRRect(RSPropertiesPainter::RRect2SkRRect(absClipRRect), true);
        auto skRectPtr = std::make_unique<SkRect>();
        skRectPtr->setXYWH(0, 0, property.GetBoundsWidth(), property.GetBoundsHeight());
        RSPropertiesPainter::DrawFilter(property, *canvas_, filter, skRectPtr, canvas_->GetSurface());
    }

    if (parentPtr && parentPtr->IsInstanceOf<RSCanvasRenderNode>()) {
        canvas_->restore();
    }

    ProcessBaseRenderNode(node);

    canvas_->RestoreAlpha();
    canvas_->restore();
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

    ColorFilterMode mode = static_cast<ColorFilterMode>(RSSystemProperties::GetCorrectionMode());
    if (RSBaseRenderUtil::IsColorFilterModeValid(mode)) {
        SkPaint paint;
        RSBaseRenderUtil::SetColorFilterModeToPaint(mode, paint);
        canvas_->saveLayer(nullptr, &paint);
    } else {
        canvas_->save();
    }
    const float rootWidth = node.GetSurfaceWidth();
    const float rootHeight = node.GetSurfaceHeight();
    SkMatrix gravityMatrix;
    (void)RSPropertiesPainter::GetGravityMatrix(frameGravity_,
        RectF {0.0f, 0.0f, boundsRect_.width(), boundsRect_.height()},
        rootWidth, rootHeight, gravityMatrix);
    canvas_->concat(gravityMatrix);
    ProcessCanvasRenderNode(node);
    canvas_->restore();
}

void RSUniRenderVisitor::ProcessCanvasRenderNode(RSCanvasRenderNode& node)
{
    if (!node.GetRenderProperties().GetVisible()) {
        RS_LOGD("RSUniRenderVisitor::ProcessCanvasRenderNode, no need process");
        return;
    }
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
