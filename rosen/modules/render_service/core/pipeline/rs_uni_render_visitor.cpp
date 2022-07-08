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
#include "platform/common/rs_log.h"
#include "property/rs_properties_painter.h"
#include "property/rs_transition_properties.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {
RSUniRenderVisitor::RSUniRenderVisitor() {}
RSUniRenderVisitor::~RSUniRenderVisitor() {}

void RSUniRenderVisitor::PrepareBaseRenderNode(RSBaseRenderNode& node)
{
    for (auto& child : node.GetSortedChildren()) {
        child->Prepare(shared_from_this());
    }
}

void RSUniRenderVisitor::PrepareDisplayRenderNode(RSDisplayRenderNode& node)
{
    PrepareBaseRenderNode(node);
}

void RSUniRenderVisitor::PrepareSurfaceRenderNode(RSSurfaceRenderNode& node)
{
    bool dirtyFlag = dirtyFlag_;
    dirtyFlag_ = node.Update(dirtyManager_, nullptr, dirtyFlag_);
    PrepareBaseRenderNode(node);
    dirtyFlag_ = dirtyFlag;
}

void RSUniRenderVisitor::PrepareRootRenderNode(RSRootRenderNode& node)
{
    dirtyFlag_ = false;
    PrepareCanvasRenderNode(node);
}

void RSUniRenderVisitor::PrepareCanvasRenderNode(RSCanvasRenderNode &node)
{
    bool dirtyFlag = dirtyFlag_;
    dirtyFlag_ = node.Update(dirtyManager_, nullptr, dirtyFlag_);
    PrepareBaseRenderNode(node);
    dirtyFlag_ = dirtyFlag;
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
    RS_LOGD("RSUniRenderVisitor::ProcessDisplayRenderNode node: %llu, child size:%u", node.GetId(),
        node.GetChildrenCount());
    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    if (!screenManager) {
        RS_LOGE("RSUniRenderVisitor::ProcessDisplayRenderNode ScreenManager is nullptr");
        return;
    }
    screenInfo_ = screenManager->QueryScreenInfo(node.GetScreenId());
    switch (screenInfo_.state) {
        case ScreenState::PRODUCER_SURFACE_ENABLE:
            node.SetCompositeType(RSDisplayRenderNode::CompositeType::SOFTWARE_COMPOSITE);
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
    processor_ = RSProcessorFactory::CreateProcessor(node.GetCompositeType());
    if (processor_ == nullptr) {
        RS_LOGE("RSUniRenderVisitor::ProcessDisplayRenderNode: RSProcessor is null!");
        return;
    }
    auto mirrorNode = node.GetMirrorSource().lock();
    if (!processor_->Init(node.GetScreenId(), node.GetDisplayOffsetX(), node.GetDisplayOffsetY(),
        mirrorNode ? mirrorNode->GetScreenId() : INVALID_SCREEN_ID)) {
        RS_LOGE("RSUniRenderVisitor::ProcessDisplayRenderNode: processor init failed!");
        return;
    }
    offsetX_ = node.GetDisplayOffsetX();
    offsetY_ = node.GetDisplayOffsetY();

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
#ifdef RS_ENABLE_GL
        RS_LOGD("RSUniRenderVisitor::ProcessDisplayRenderNode SetRenderContext");
        node.GetRSSurface()->SetRenderContext(
            RSMainThread::Instance()->GetRenderEngine()->GetRenderContext().get());
#endif
    }

    auto rsSurface = node.GetRSSurface();
    if (rsSurface == nullptr) {
        RS_LOGE("RSUniRenderVisitor::ProcessDisplayRenderNode No RSSurface found");
        return;
    }
    auto surfaceFrame = rsSurface->RequestFrame(screenInfo_.width, screenInfo_.height);
    if (surfaceFrame == nullptr) {
        RS_LOGE("RSUniRenderVisitor Request Frame Failed");
        return;
    }
    canvas_ = std::make_unique<RSPaintFilterCanvas>(surfaceFrame->GetSurface().get());
    canvas_->clear(SK_ColorTRANSPARENT);

    ProcessBaseRenderNode(node);
    RS_TRACE_BEGIN("RSUniRender:FlushFrame");
    rsSurface->FlushFrame(surfaceFrame);
    RSMainThread::Instance()->WaitUtilUniRenderFinished();
    RS_TRACE_END();

    processor_->ProcessDisplaySurface(node);
    processor_->PostProcess();

    // We should release DisplayNode's surface buffer after PostProcess(),
    // since the buffer's releaseFence was set in PostProcess().
    auto& surfaceHandler = static_cast<RSSurfaceHandler&>(node);
    (void)RSBaseRenderUtil::ReleaseBuffer(surfaceHandler);
    RS_LOGD("RSUniRenderVisitor::ProcessDisplayRenderNode end");
}

void RSUniRenderVisitor::ProcessSurfaceRenderNode(RSSurfaceRenderNode& node)
{
    RS_LOGD("RSUniRenderVisitor::ProcessSurfaceRenderNode node: %llu, child size:%u %s", node.GetId(),
        node.GetChildrenCount(), node.GetName().c_str());
    const auto& property = node.GetRenderProperties();
    if (!property.GetVisible()) {
        RS_LOGD("RSUniRenderVisitor::ProcessSurfaceRenderNode node: %llu invisible", node.GetId());
        return;
    }

    if (!canvas_) {
        RS_LOGE("RSUniRenderVisitor::ProcessSurfaceRenderNode, canvas is nullptr");
        return;
    }
    auto geoPtr = std::static_pointer_cast<RSObjAbsGeometry>(property.GetBoundsGeometry());
    if (!geoPtr) {
        RS_LOGE("RSUniRenderVisitor::ProcessSurfaceRenderNode node:%llu, get geoPtr failed", node.GetId());
        return;
    }
    RS_TRACE_BEGIN("RSUniRender::Process:" + node.GetName());
    canvas_->save();
    canvas_->SaveAlpha();

    canvas_->MultiplyAlpha(property.GetAlpha() * node.GetContextAlpha());

    canvas_->concat(node.GetContextMatrix());
    auto contextClipRect = node.GetContextClipRegion();
    if (!contextClipRect.isEmpty()) {
        canvas_->clipRect(contextClipRect);
    }

    canvas_->concat(geoPtr->GetMatrix());
    clipRect_ = SkRect::MakeWH(property.GetBoundsWidth(), property.GetBoundsHeight());
    frameGravity_ = property.GetFrameGravity();
    canvas_->clipRect(clipRect_);

    auto backgroundColor = SkColor4f::FromBytes_RGBA(property.GetBackgroundColor().AsRgbaInt()).toSkColor();
    if (backgroundColor != SK_ColorTRANSPARENT) {
        canvas_->clear(backgroundColor);
    }

    auto transitionProperties = node.GetAnimationManager().GetTransitionProperties();
    RSPropertiesPainter::DrawTransitionProperties(transitionProperties, property, *canvas_);
    ProcessBaseRenderNode(node);

    if (node.GetConsumer() != nullptr) {
        RS_TRACE_BEGIN("UniRender::Process:" + node.GetName());
        if (node.GetBuffer() == nullptr) {
            RS_LOGD("RSUniRenderVisitor::ProcessSurfaceRenderNode:%llu buffer is not available", node.GetId());
        } else {
            node.NotifyRTBufferAvailable();
            const auto& buffer = node.GetBuffer();
            const float frameWidth = buffer->GetSurfaceBufferWidth();
            const float frameHeight = buffer->GetSurfaceBufferHeight();
            SkMatrix gravityMatrix;
            (void)RSPropertiesPainter::GetGravityMatrix(frameGravity_,
                RectF {0.0f, 0.0f, clipRect_.width(), clipRect_.height()},
                frameWidth, frameHeight, gravityMatrix);
            canvas_->concat(gravityMatrix);

#ifdef RS_ENABLE_EGLIMAGE
            RS_LOGD("RSUniRenderVisitor::ProcessSurfaceRenderNode draw image on canvas");
            DrawImageOnCanvas(node);
#else
            RS_LOGD("RSUniRenderVisitor::ProcessSurfaceRenderNode draw buffer on canvas");
            DrawBufferOnCanvas(node);
#endif // RS_ENABLE_EGLIMAGE
        }
        RS_TRACE_END();
    }
    canvas_->RestoreAlpha();
    canvas_->restore();
    RS_TRACE_END();
}

void RSUniRenderVisitor::ProcessRootRenderNode(RSRootRenderNode& node)
{
    RS_LOGD("RSUniRenderVisitor::ProcessRootRenderNode node: %llu, child size:%u", node.GetId(),
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

    const float frameWidth = property.GetFrameWidth();
    const float frameHeight = property.GetFrameHeight();
    SkMatrix gravityMatrix;
    (void)RSPropertiesPainter::GetGravityMatrix(frameGravity_,
        RectF {0.0f, 0.0f, clipRect_.width(), clipRect_.height()},
        frameWidth, frameHeight, gravityMatrix);
    canvas_->concat(gravityMatrix);

    canvas_->save();
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

void RSUniRenderVisitor::DrawBufferOnCanvas(RSSurfaceRenderNode& node)
{
    if (!canvas_) {
        RS_LOGE("RSUniRenderVisitor::DrawBufferOnCanvas canvas is nullptr");
        return;
    }

    auto buffer = node.GetBuffer();
    auto srcRect = SkRect::MakeWH(buffer->GetSurfaceBufferWidth(), buffer->GetSurfaceBufferHeight());
    auto dstRect = srcRect;
    RSUniRenderUtil::DrawBufferOnCanvas(buffer, static_cast<ColorGamut>(screenInfo_.colorGamut), *canvas_,
        srcRect, dstRect);
}

#ifdef RS_ENABLE_EGLIMAGE
void RSUniRenderVisitor::DrawImageOnCanvas(RSSurfaceRenderNode& node)
{
    if (!canvas_) {
        RS_LOGE("RSUniRenderVisitor::DrawImageOnCanvas canvas is nullptr");
        return;
    }

    auto buffer = node.GetBuffer();
    if (buffer->GetFormat() == PIXEL_FMT_YCRCB_420_SP || buffer->GetFormat() == PIXEL_FMT_YCBCR_420_SP) {
        DrawBufferOnCanvas(node);
        return;
    }

    auto srcRect = SkRect::MakeWH(buffer->GetSurfaceBufferWidth(), buffer->GetSurfaceBufferHeight());
    auto dstRect = srcRect;
    BufferInfo bufferInfo = { buffer, node.GetAcquireFence(), node.GetConsumer() };
    RSUniRenderUtil::DrawImageOnCanvas(bufferInfo, *canvas_, srcRect, dstRect);
}
#endif // RS_ENABLE_EGLIMAGE
} // namespace Rosen
} // namespace OHOS
