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

#include "pipeline/rs_uni_ui_capture.h"

#include <functional>

#include "include/core/SkRect.h"
#include "rs_trace.h"

#include "common/rs_common_def.h"
#include "common/rs_obj_abs_geometry.h"
#include "pipeline/rs_dirty_region_manager.h"
#include "pipeline/rs_divided_render_util.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "pipeline/rs_uni_render_util.h"
#include "platform/common/rs_log.h"
#include "render/rs_skia_filter.h"

namespace OHOS {
namespace Rosen {

const int FAKE_WIDTH = 10; // When the width and height of the node are not set, use the fake width
const int FAKE_HEIGHT = 10; // When the width and height of the node are not set, use the fake height

std::shared_ptr<Media::PixelMap> RSUniUICapture::TakeLocalCapture()
{
    if (ROSEN_EQ(scaleX_, 0.f) || ROSEN_EQ(scaleY_, 0.f) || scaleX_ < 0.f || scaleY_ < 0.f) {
        RS_LOGE("RSUniUICapture::TakeLocalCapture: scale is invalid.");
        return nullptr;
    }
    auto node = RSMainThread::Instance()->GetContext().GetNodeMap().GetRenderNode<RSRenderNode>(nodeId_);
    if (node == nullptr) {
        RS_LOGE("RSUniUICapture::TakeLocalCapture node is nullptr return");
        return nullptr;
    }
    std::shared_ptr<RSUniUICaptureVisitor> visitor =
        std::make_shared<RSUniUICaptureVisitor>(nodeId_, scaleX_, scaleY_);
    auto recordingCanvas = std::make_shared<RSRecordingCanvas>(FAKE_WIDTH, FAKE_HEIGHT);
    PostTaskToRSRecord(recordingCanvas, node, visitor);
    auto drawCallList = recordingCanvas->GetDrawCmdList();
    std::shared_ptr<Media::PixelMap> pixelmap = CreatePixelMapByNode(node);
    if (pixelmap == nullptr) {
        RS_LOGE("RSUniUICapture::TakeLocalCapture: pixelmap == nullptr!");
        return nullptr;
    }
    auto skSurface = CreateSurface(pixelmap);
    if (skSurface == nullptr) {
        return nullptr;
    }
    auto canvas = std::make_shared<RSPaintFilterCanvas>(skSurface.get());
    drawCallList->Playback(*canvas);
    return pixelmap;
}

std::shared_ptr<Media::PixelMap> RSUniUICapture::CreatePixelMapByNode(std::shared_ptr<RSRenderNode> node) const
{
    int pixmapWidth = node->GetRenderProperties().GetBoundsWidth();
    int pixmapHeight = node->GetRenderProperties().GetBoundsHeight();
    Media::InitializationOptions opts;
    opts.size.width = ceil(pixmapWidth * scaleX_);
    opts.size.height = ceil(pixmapHeight * scaleY_);
    return Media::PixelMap::Create(opts);
}

sk_sp<SkSurface> RSUniUICapture::CreateSurface(const std::shared_ptr<Media::PixelMap>& pixelmap) const
{
    if (pixelmap == nullptr) {
        RS_LOGE("RSUniUICapture::CreateSurface: pixelmap == nullptr");
        return nullptr;
    }
    auto address = const_cast<uint32_t*>(pixelmap->GetPixel32(0, 0));
    if (address == nullptr) {
        RS_LOGE("RSUniUICapture::CreateSurface: address == nullptr");
        return nullptr;
    }
    SkImageInfo info = SkImageInfo::Make(pixelmap->GetWidth(), pixelmap->GetHeight(),
        kRGBA_8888_SkColorType, kPremul_SkAlphaType);
    return SkSurface::MakeRasterDirect(info, address, pixelmap->GetRowBytes());
}

RSUniUICapture::RSUniUICaptureVisitor::RSUniUICaptureVisitor(NodeId nodeId, float scaleX, float scaleY)
    : nodeId_(nodeId), scaleX_(scaleX), scaleY_(scaleY)
{
    renderEngine_ = RSMainThread::Instance()->GetRenderEngine();
    isUniRender_ = RSUniRenderJudgement::IsUniRender();
    auto node = RSMainThread::Instance()->GetContext().GetNodeMap().GetRenderNode<RSRenderNode>(nodeId_);
    if (node == nullptr) {
        RS_LOGE("RSUniUICapture::TakeLocalCapture node is nullptr return");
        return;
    }
    const auto& targetNodeProperty = node->GetRenderProperties();
    auto targetNodeGeoPtr = std::static_pointer_cast<RSObjAbsGeometry>(targetNodeProperty.GetBoundsGeometry());
    captureMatrix_.setScaleX(scaleX_);
    captureMatrix_.setScaleY(scaleY_);
    SkMatrix invertMatrix;
    if (targetNodeGeoPtr->GetAbsMatrix().invert(&invertMatrix)) {
        captureMatrix_.preConcat(invertMatrix);
    }
}

void RSUniUICapture::PostTaskToRSRecord(std::shared_ptr<RSRecordingCanvas> canvas,
    std::shared_ptr<RSRenderNode> node, std::shared_ptr<RSUniUICaptureVisitor> visitor)
{
    std::function<void()> recordingDrawCall = [canvas, node, visitor]() -> void {
        visitor->SetCanvas(canvas);
        if (!node->IsOnTheTree()) {
            node->Prepare(visitor);
        }
        node->Process(visitor);
    };
    RSMainThread::Instance()->PostSyncTask(recordingDrawCall);
}

void RSUniUICapture::RSUniUICaptureVisitor::SetCanvas(std::shared_ptr<RSRecordingCanvas> canvas)
{
    if (canvas == nullptr) {
        RS_LOGE("RSUniUICaptureVisitor::SetCanvas: canvas == nullptr");
        return;
    }
    canvas_ = std::make_shared<RSPaintFilterCanvas>(canvas.get());
    canvas_->scale(scaleX_, scaleY_);
}

void RSUniUICapture::RSUniUICaptureVisitor::ProcessBaseRenderNode(RSBaseRenderNode& node)
{
    for (auto& child : node.GetSortedChildren()) {
        child->Process(shared_from_this());
    }
    // clear SortedChildren, it will be generated again in next frame
    node.ResetSortedChildren();
}

void RSUniUICapture::RSUniUICaptureVisitor::ProcessRootRenderNode(RSRootRenderNode& node)
{
    if (!node.ShouldPaint()) {
        RS_LOGD("RSUniUICaptureVisitor::ProcessRootRenderNode, no need process");
        return;
    }

    if (!canvas_) {
        RS_LOGE("RSUniUICaptureVisitor::ProcessRootRenderNode, canvas is nullptr");
        return;
    }

    canvas_->save();
    ProcessCanvasRenderNode(node);
    canvas_->restore();
}

void RSUniUICapture::RSUniUICaptureVisitor::ProcessCanvasRenderNode(RSCanvasRenderNode& node)
{
    if (!node.ShouldPaint()) {
        RS_LOGD("RSUniUICaptureVisitor::ProcessCanvasRenderNode, no need process");
        return;
    }
    if (!canvas_) {
        RS_LOGE("RSUniUICaptureVisitor::ProcessCanvasRenderNode, canvas is nullptr");
        return;
    }
    if (node.GetId() == nodeId_) {
        // When drawing nodes, canvas will offset the bounds value, so we will move in reverse here first
        const auto& property = node.GetRenderProperties();
        auto geoPtr = std::static_pointer_cast<RSObjAbsGeometry>(property.GetBoundsGeometry());
        SkMatrix relativeMatrix = SkMatrix::I();
        relativeMatrix.setScaleX(scaleX_);
        relativeMatrix.setScaleY(scaleY_);
        SkMatrix invertMatrix;
        if (geoPtr->GetMatrix().invert(&invertMatrix)) {
            relativeMatrix.preConcat(invertMatrix);
        }
        canvas_->setMatrix(relativeMatrix);
    }
    node.ProcessRenderBeforeChildren(*canvas_);
    ProcessBaseRenderNode(node);
    node.ProcessRenderAfterChildren(*canvas_);
}


void RSUniUICapture::RSUniUICaptureVisitor::ProcessSurfaceRenderNode(RSSurfaceRenderNode& node)
{
    if (canvas_ == nullptr) {
        RS_LOGE("RSUniUICaptureVisitor::ProcessSurfaceRenderNode, canvas is nullptr");
        return;
    }

    if (!node.ShouldPaint()) {
        RS_LOGD("RSUniUICaptureVisitor::ProcessSurfaceRenderNode node: %" PRIu64 " invisible", node.GetId());
        return;
    }
    if (isUniRender_) {
        ProcessSurfaceRenderNodeWithUni(node);
    } else {
        ProcessSurfaceViewWithoutUni(node);
    }
}

void RSUniUICapture::RSUniUICaptureVisitor::ProcessSurfaceRenderNodeWithUni(RSSurfaceRenderNode& node)
{
    auto geoPtr = std::static_pointer_cast<RSObjAbsGeometry>(node.GetRenderProperties().GetBoundsGeometry());
    if (geoPtr == nullptr) {
        RS_LOGI(
            "RSUniUICaptureVisitor::ProcessSurfaceRenderNode node:%" PRIu64 ", get geoPtr failed", node.GetId());
        return;
    }

    RSAutoCanvasRestore acr(canvas_);
    canvas_->MultiplyAlpha(node.GetRenderProperties().GetAlpha());
    ProcessSurfaceViewWithUni(node);
}

void RSUniUICapture::RSUniUICaptureVisitor::ProcessSurfaceViewWithUni(RSSurfaceRenderNode& node)
{
    canvas_->save();

    const auto& property = node.GetRenderProperties();
    auto geoPtr = std::static_pointer_cast<RSObjAbsGeometry>(property.GetBoundsGeometry());
    if (!geoPtr) {
        RS_LOGE("RSUniUICaptureVisitor::ProcessSurfaceViewWithUni node:%" PRIu64 ", get geoPtr failed",
            node.GetId());
        return;
    }
    canvas_->setMatrix(captureMatrix_);
    canvas_->concat(geoPtr->GetAbsMatrix());

    bool isSelfDrawingSurface = node.GetSurfaceNodeType() == RSSurfaceNodeType::SELF_DRAWING_NODE;
    const RectF absBounds = { 0, 0, property.GetBoundsWidth(), property.GetBoundsHeight() };
    RRect absClipRRect = RRect(absBounds, property.GetCornerRadius());
    if (isSelfDrawingSurface) {
        RSPropertiesPainter::DrawShadow(property, *canvas_, &absClipRRect);
    }
    canvas_->save();
    if (isSelfDrawingSurface && !property.GetCornerRadius().IsZero()) {
        canvas_->clipRRect(RSPropertiesPainter::RRect2SkRRect(absClipRRect), true);
    } else {
        canvas_->clipRect(SkRect::MakeWH(property.GetBoundsWidth(), property.GetBoundsHeight()));
    }
    if (isSelfDrawingSurface) {
        RSPropertiesPainter::DrawBackground(property, *canvas_);
        RSPropertiesPainter::DrawMask(property, *canvas_);
        auto filter = std::static_pointer_cast<RSSkiaFilter>(property.GetBackgroundFilter());
        if (filter != nullptr) {
            auto skRectPtr = std::make_unique<SkRect>();
            skRectPtr->setXYWH(0, 0, property.GetBoundsWidth(), property.GetBoundsHeight());
            RSPropertiesPainter::DrawFilter(property, *canvas_, filter, skRectPtr, canvas_->GetSurface());
        }
    } else {
        auto backgroundColor = static_cast<SkColor>(property.GetBackgroundColor().AsArgbInt());
        if (SkColorGetA(backgroundColor) != SK_AlphaTRANSPARENT) {
            canvas_->drawColor(backgroundColor);
        }
    }
    canvas_->restore();
    if (node.GetBuffer() != nullptr) {
        auto params = RSUniRenderUtil::CreateBufferDrawParam(node, true);
        renderEngine_->DrawSurfaceNodeWithParams(*canvas_, node, params);
    }
    if (isSelfDrawingSurface) {
        auto filter = std::static_pointer_cast<RSSkiaFilter>(property.GetFilter());
        if (filter != nullptr) {
            auto skRectPtr = std::make_unique<SkRect>();
            skRectPtr->setXYWH(0, 0, property.GetBoundsWidth(), property.GetBoundsHeight());
            RSPropertiesPainter::DrawFilter(property, *canvas_, filter, skRectPtr, canvas_->GetSurface());
        }
    }
    canvas_->restore();
    ProcessBaseRenderNode(node);
}

void RSUniUICapture::RSUniUICaptureVisitor::ProcessSurfaceViewWithoutUni(RSSurfaceRenderNode& node)
{
    SkMatrix translateMatrix;
    auto parentPtr = node.GetParent().lock();
    if (parentPtr != nullptr && parentPtr->IsInstanceOf<RSSurfaceRenderNode>()) {
        // calculate the offset from this node's parent, and perform translate.
        auto parentNode = std::static_pointer_cast<RSSurfaceRenderNode>(parentPtr);
        const float parentNodeTranslateX = parentNode->GetTotalMatrix().getTranslateX();
        const float parentNodeTranslateY = parentNode->GetTotalMatrix().getTranslateY();
        const float thisNodetranslateX = node.GetTotalMatrix().getTranslateX();
        const float thisNodetranslateY = node.GetTotalMatrix().getTranslateY();
        translateMatrix.preTranslate(
            thisNodetranslateX - parentNodeTranslateX, thisNodetranslateY - parentNodeTranslateY);
    }
    if (node.GetChildrenCount() > 0) {
        if (node.GetId() != nodeId_) {
            canvas_->concat(translateMatrix);
        }
        const auto saveCnt = canvas_->save();
        ProcessBaseRenderNode(node);
        canvas_->restoreToCount(saveCnt);
        if (node.GetBuffer() != nullptr) {
            // in node's local coordinate.
            auto params = RSDividedRenderUtil::CreateBufferDrawParam(node, true, false, false, false);
            renderEngine_->DrawSurfaceNodeWithParams(*canvas_, node, params);
        }
    } else {
        canvas_->save();
        if (node.GetId() != nodeId_) {
            canvas_->concat(translateMatrix);
        }
        if (node.GetBuffer() != nullptr) {
            // in node's local coordinate.
            auto params = RSDividedRenderUtil::CreateBufferDrawParam(node, true, false, false, false);
            renderEngine_->DrawSurfaceNodeWithParams(*canvas_, node, params);
        }
        canvas_->restore();
    }
}

void RSUniUICapture::RSUniUICaptureVisitor::PrepareBaseRenderNode(RSBaseRenderNode& node)
{
    for (auto& child : node.GetSortedChildren()) {
        child->Prepare(shared_from_this());
    }
}

void RSUniUICapture::RSUniUICaptureVisitor::PrepareCanvasRenderNode(RSCanvasRenderNode& node)
{
    node.ApplyModifiers();
    auto dirtyManager = std::make_shared<RSDirtyRegionManager>();
    node.Update(*dirtyManager, nullptr, false);
    PrepareBaseRenderNode(node);
}

void RSUniUICapture::RSUniUICaptureVisitor::PrepareSurfaceRenderNode(RSSurfaceRenderNode& node)
{
    node.ApplyModifiers();
    auto dirtyManager = std::make_shared<RSDirtyRegionManager>();
    node.Update(*dirtyManager, nullptr, false);
    PrepareBaseRenderNode(node);
}

void RSUniUICapture::RSUniUICaptureVisitor::PrepareRootRenderNode(RSRootRenderNode& node)
{
    node.ApplyModifiers();
    PrepareCanvasRenderNode(node);
}

} // namespace Rosen
} // namespace OHOS
