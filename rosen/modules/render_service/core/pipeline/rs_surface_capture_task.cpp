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

#include "pipeline/rs_surface_capture_task.h"

#include <memory>

#include "include/core/SkCanvas.h"
#include "include/core/SkMatrix.h"
#include "include/core/SkRect.h"
#include "rs_trace.h"

#include "common/rs_obj_abs_geometry.h"
#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_cold_start_thread.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_divided_render_util.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_render_service_connection.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "pipeline/rs_uni_render_util.h"
#include "platform/common/rs_log.h"
#include "platform/drawing/rs_surface.h"
#include "render/rs_skia_filter.h"
#include "screen_manager/rs_screen_manager.h"
#include "screen_manager/rs_screen_mode_info.h"

namespace OHOS {
namespace Rosen {
std::unique_ptr<Media::PixelMap> RSSurfaceCaptureTask::Run()
{
    if (ROSEN_EQ(scaleX_, 0.f) || ROSEN_EQ(scaleY_, 0.f) || scaleX_ < 0.f || scaleY_ < 0.f) {
        RS_LOGE("RSSurfaceCaptureTask::Run: SurfaceCapture scale is invalid.");
        return nullptr;
    }
    auto node = RSMainThread::Instance()->GetContext().GetNodeMap().GetRenderNode(nodeId_);
    if (node == nullptr) {
        RS_LOGE("RSSurfaceCaptureTask::Run: node is nullptr");
        return nullptr;
    }
    std::unique_ptr<Media::PixelMap> pixelmap;
    std::shared_ptr<RSSurfaceCaptureVisitor> visitor = std::make_shared<RSSurfaceCaptureVisitor>(scaleX_, scaleY_,
        RSMainThread::Instance()->IfUseUniVisitor());
    if (auto surfaceNode = node->ReinterpretCastTo<RSSurfaceRenderNode>()) {
        RS_LOGD("RSSurfaceCaptureTask::Run: Into SURFACE_NODE SurfaceRenderNodeId:[%" PRIu64 "]", node->GetId());
        pixelmap = CreatePixelMapBySurfaceNode(surfaceNode, visitor->IsUniRender());
        visitor->IsDisplayNode(false);
    } else if (auto displayNode = node->ReinterpretCastTo<RSDisplayRenderNode>()) {
        RS_LOGD("RSSurfaceCaptureTask::Run: Into DISPLAY_NODE DisplayRenderNodeId:[%" PRIu64 "]", node->GetId());
        pixelmap = CreatePixelMapByDisplayNode(displayNode);
        visitor->IsDisplayNode(true);
    } else {
        RS_LOGE("RSSurfaceCaptureTask::Run: Invalid RSRenderNodeType!");
        return nullptr;
    }
    if (pixelmap == nullptr) {
        RS_LOGE("RSSurfaceCaptureTask::Run: pixelmap == nullptr!");
        return nullptr;
    }
    auto skSurface = CreateSurface(pixelmap);
    if (skSurface == nullptr) {
        RS_LOGE("RSSurfaceCaptureTask::Run: surface is nullptr!");
        return nullptr;
    }
    visitor->SetSurface(skSurface.get());
    node->Process(visitor);
#if (defined RS_ENABLE_GL) && (defined RS_ENABLE_EGLIMAGE)
    sk_sp<SkImage> img(skSurface.get()->makeImageSnapshot());
    if (!img) {
        RS_LOGE("RSSurfaceCaptureTask::Run: img is nullptr");
        return nullptr;
    }

    auto data = (uint8_t *)malloc(pixelmap->GetRowBytes() * pixelmap->GetHeight());
    if (data == nullptr) {
        RS_LOGE("RSSurfaceCaptureTask::Run: data is nullptr");
        return nullptr;
    }
    SkImageInfo info = SkImageInfo::Make(pixelmap->GetWidth(), pixelmap->GetHeight(),
        kRGBA_8888_SkColorType, kPremul_SkAlphaType);
    if (!img->readPixels(info, data, pixelmap->GetRowBytes(), 0, 0)) {
        RS_LOGE("RSSurfaceCaptureTask::Run: readPixels failed");
        free(data);
        data = nullptr;
        return nullptr;
    }
    pixelmap->SetPixelsAddr(data, nullptr, pixelmap->GetRowBytes() * pixelmap->GetHeight(),
        Media::AllocatorType::HEAP_ALLOC, nullptr);
#endif
    return pixelmap;
}

std::unique_ptr<Media::PixelMap> RSSurfaceCaptureTask::CreatePixelMapBySurfaceNode(
    std::shared_ptr<RSSurfaceRenderNode> node, bool isUniRender)
{
    if (node == nullptr) {
        RS_LOGE("CreatePixelMapBySurfaceNode: node == nullptr");
        return nullptr;
    }
    if (!isUniRender && node->GetBuffer() == nullptr) {
        RS_LOGE("CreatePixelMapBySurfaceNode: node GetBuffer == nullptr");
        return nullptr;
    }
    int pixmapWidth = node->GetRenderProperties().GetBoundsWidth();
    int pixmapHeight = node->GetRenderProperties().GetBoundsHeight();
    Media::InitializationOptions opts;
    opts.size.width = ceil(pixmapWidth * scaleX_);
    opts.size.height = ceil(pixmapHeight * scaleY_);
    RS_LOGD("RSSurfaceCaptureTask::CreatePixelMapBySurfaceNode: origin pixelmap width is [%u], height is [%u], "\
        "created pixelmap width is [%u], height is [%u], the scale is scaleY:[%f], scaleY:[%f]",
        pixmapWidth, pixmapHeight, opts.size.width, opts.size.height, scaleX_, scaleY_);
    return Media::PixelMap::Create(opts);
}

std::unique_ptr<Media::PixelMap> RSSurfaceCaptureTask::CreatePixelMapByDisplayNode(
    std::shared_ptr<RSDisplayRenderNode> node)
{
    if (node == nullptr) {
        RS_LOGE("RSSurfaceCaptureTask::CreatePixelMapByDisplayNode: node is nullptr");
        return nullptr;
    }
    uint64_t screenId = node->GetScreenId();
    RSScreenModeInfo screenModeInfo;
    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    if (!screenManager) {
        RS_LOGE("RSSurfaceCaptureTask::CreatePixelMapByDisplayNode: screenManager is nullptr!");
        return nullptr;
    }
    auto screenInfo = screenManager->QueryScreenInfo(screenId);
    uint32_t pixmapWidth = screenInfo.width;
    uint32_t pixmapHeight = screenInfo.height;
    auto rotation = node->GetRotation();
    if (rotation == ScreenRotation::ROTATION_90 || rotation == ScreenRotation::ROTATION_270) {
        std::swap(pixmapWidth, pixmapHeight);
    }
    Media::InitializationOptions opts;
    opts.size.width = ceil(pixmapWidth * scaleX_);
    opts.size.height = ceil(pixmapHeight * scaleY_);
    RS_LOGD("RSSurfaceCaptureTask::CreatePixelMapByDisplayNode: origin pixelmap width is [%u], height is [%u], "\
        "created pixelmap width is [%u], height is [%u], the scale is scaleY:[%f], scaleY:[%f]",
        pixmapWidth, pixmapHeight, opts.size.width, opts.size.height, scaleX_, scaleY_);
    return Media::PixelMap::Create(opts);
}

sk_sp<SkSurface> RSSurfaceCaptureTask::CreateSurface(const std::unique_ptr<Media::PixelMap>& pixelmap)
{
    if (pixelmap == nullptr) {
        RS_LOGE("RSSurfaceCaptureTask::CreateSurface: pixelmap == nullptr");
        return nullptr;
    }
    auto address = const_cast<uint32_t*>(pixelmap->GetPixel32(0, 0));
    if (address == nullptr) {
        RS_LOGE("RSSurfaceCaptureTask::CreateSurface: address == nullptr");
        return nullptr;
    }
    SkImageInfo info = SkImageInfo::Make(pixelmap->GetWidth(), pixelmap->GetHeight(),
        kRGBA_8888_SkColorType, kPremul_SkAlphaType);
#if (defined RS_ENABLE_GL) && (defined RS_ENABLE_EGLIMAGE)
    auto renderContext = RSBaseRenderEngine::GetRenderContext();
    if (renderContext == nullptr) {
        RS_LOGE("RSSurfaceCaptureTask::CreateSurface: renderContext is nullptr");
        return nullptr;
    }
    renderContext->SetUpGrContext();
    return SkSurface::MakeRenderTarget(renderContext->GetGrContext(), SkBudgeted::kNo, info);
#endif
    return SkSurface::MakeRasterDirect(info, address, pixelmap->GetRowBytes());
}

RSSurfaceCaptureTask::RSSurfaceCaptureVisitor::RSSurfaceCaptureVisitor(float scaleX, float scaleY, bool isUniRender)
    : scaleX_(scaleX), scaleY_(scaleY), isUniRender_(isUniRender)
{
    renderEngine_ = RSMainThread::Instance()->GetRenderEngine();
}

void RSSurfaceCaptureTask::RSSurfaceCaptureVisitor::SetSurface(SkSurface* surface)
{
    if (surface == nullptr) {
        RS_LOGE("RSSurfaceCaptureTask::RSSurfaceCaptureVisitor::SetSurface: surface == nullptr");
        return;
    }
    canvas_ = std::make_unique<RSPaintFilterCanvas>(surface);
    canvas_->scale(scaleX_, scaleY_);
}

void RSSurfaceCaptureTask::RSSurfaceCaptureVisitor::ProcessBaseRenderNode(RSBaseRenderNode &node)
{
    for (auto& child : node.GetSortedChildren()) {
        child->Process(shared_from_this());
    }
    // clear SortedChildren, it will be generated again in next frame
    node.ResetSortedChildren();
}

void RSSurfaceCaptureTask::RSSurfaceCaptureVisitor::ProcessDisplayRenderNode(RSDisplayRenderNode &node)
{
    RS_LOGD("RsDebug RSSurfaceCaptureVisitor::ProcessDisplayRenderNode child size:[%d] total size:[%d]",
        node.GetChildrenCount(), node.GetSortedChildren().size());

    ProcessBaseRenderNode(node);
}

void RSSurfaceCaptureTask::RSSurfaceCaptureVisitor::CaptureSingleSurfaceNodeWithUni(RSSurfaceRenderNode& node)
{
    const auto& property = node.GetRenderProperties();
    auto geoPtr = std::static_pointer_cast<RSObjAbsGeometry>(property.GetBoundsGeometry());
    if (!geoPtr) {
        RS_LOGE("RSSurfaceCaptureVisitor::CaptureSingleSurfaceNodeWithUni node:%" PRIu64 ", get geoPtr failed",
            node.GetId());
        return;
    }

    canvas_->save();

    if (node.IsAppWindow()) {
        // When CaptureSingleSurfaceNodeWithUni, we should consider scale factor of canvas_ and
        // child nodes (self-drawing surfaceNode) of AppWindow should use relative coordinates
        // which is the node relative to the upper-left corner of the window.
        // So we have to get the invert matrix of AppWindow here and apply it to canvas_
        // when we calculate the position of self-drawing surfaceNode.
        captureMatrix_.setScaleX(scaleX_);
        captureMatrix_.setScaleY(scaleY_);
        SkMatrix invertMatrix;
        if (geoPtr->GetAbsMatrix().invert(&invertMatrix)) {
            captureMatrix_.preConcat(invertMatrix);
        }
    } else {
        canvas_->setMatrix(captureMatrix_);
        canvas_->concat(geoPtr->GetAbsMatrix());
    }

    bool isSelfDrawingSurface = node.GetSurfaceNodeType() == RSSurfaceNodeType::SELF_DRAWING_NODE;
    const RectF absBounds = {0, 0, property.GetBoundsWidth(), property.GetBoundsHeight()};
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
    if (node.GetSecurityLayer()) {
        RS_LOGD("RSSurfaceCaptureTask::RSSurfaceCaptureVisitor::CaptureSingleSurfaceNodeWithUni: \
            process RSSurfaceRenderNode(id:[%" PRIu64 "]) clear white since it is security layer.",
            node.GetId());
        canvas_->clear(SK_ColorWHITE);
        canvas_->restore(); // restore clipRect
        canvas_->restore(); // restore translate and concat
        return;
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
    if (!node.IsAppWindow() && node.GetBuffer() != nullptr) {
        auto params = RSUniRenderUtil::CreateBufferDrawParam(node, false);
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
    if (node.IsAppWindow() && RSColdStartManager::Instance().IsColdStartThreadRunning(node.GetId()) &&
        node.GetCachedImage() != nullptr) {
        RS_LOGD("RSSurfaceCaptureVisitor DrawCachedImage");
        RSUniRenderUtil::DrawCachedImage(node, *canvas_, node.GetCachedImage());
    } else {
        ProcessBaseRenderNode(node);
    }
}

void RSSurfaceCaptureTask::RSSurfaceCaptureVisitor::CaptureSurfaceInDisplayWithUni(RSSurfaceRenderNode& node)
{
    if (node.GetSecurityLayer()) {
        RS_LOGD("RSSurfaceCaptureTask::RSSurfaceCaptureVisitor::CaptureSurfaceInDisplayWithUni: \
            process RSSurfaceRenderNode(id:[%" PRIu64 "]) paused since it is security layer.",
            node.GetId());
        return;
    }
    bool isSelfDrawingSurface = node.GetSurfaceNodeType() == RSSurfaceNodeType::SELF_DRAWING_NODE ||
        node.GetSurfaceNodeType() == RSSurfaceNodeType::ABILITY_COMPONENT_NODE;
    if (!isSelfDrawingSurface) {
        canvas_->concat(node.GetContextMatrix());
        auto contextClipRect = node.GetContextClipRegion();
        if (!contextClipRect.isEmpty()) {
            canvas_->clipRect(contextClipRect);
        }
    }

    if (isSelfDrawingSurface) {
        canvas_->save();
    }

    const auto& property = node.GetRenderProperties();
    auto geoPtr = std::static_pointer_cast<RSObjAbsGeometry>(property.GetBoundsGeometry());
    if (geoPtr) {
        canvas_->concat(geoPtr->GetMatrix());
    }

    if (isSelfDrawingSurface) {
        canvas_->save();
    }

    const RectF absBounds = {0, 0, property.GetBoundsWidth(), property.GetBoundsHeight()};
    RRect absClipRRect = RRect(absBounds, property.GetCornerRadius());
    RSPropertiesPainter::DrawShadow(property, *canvas_, &absClipRRect);
    if (!property.GetCornerRadius().IsZero()) {
        canvas_->clipRRect(RSPropertiesPainter::RRect2SkRRect(absClipRRect), true);
    } else {
        canvas_->clipRect(SkRect::MakeWH(property.GetBoundsWidth(), property.GetBoundsHeight()));
    }
    auto backgroundColor = static_cast<SkColor>(property.GetBackgroundColor().AsArgbInt());
    if (SkColorGetA(backgroundColor) != SK_AlphaTRANSPARENT) {
        canvas_->drawColor(backgroundColor);
    }
    RSPropertiesPainter::DrawMask(property, *canvas_);

    auto filter = std::static_pointer_cast<RSSkiaFilter>(property.GetBackgroundFilter());
    if (filter != nullptr) {
        auto skRectPtr = std::make_unique<SkRect>();
        skRectPtr->setXYWH(0, 0, property.GetBoundsWidth(), property.GetBoundsHeight());
        RSPropertiesPainter::DrawFilter(property, *canvas_, filter, skRectPtr, canvas_->GetSurface());
    }
    if (isSelfDrawingSurface) {
        canvas_->restore();
    }

    if (!node.IsAppWindow() && node.GetBuffer() != nullptr) {
        auto params = RSUniRenderUtil::CreateBufferDrawParam(node, false);
        renderEngine_->DrawSurfaceNodeWithParams(*canvas_, node, params);
    }

    if (isSelfDrawingSurface) {
        canvas_->restore();
    }

    ProcessBaseRenderNode(node);
    filter = std::static_pointer_cast<RSSkiaFilter>(property.GetFilter());
    if (filter != nullptr) {
        auto skRectPtr = std::make_unique<SkRect>();
        skRectPtr->setXYWH(0, 0, property.GetBoundsWidth(), property.GetBoundsHeight());
        RSPropertiesPainter::DrawFilter(property, *canvas_, filter, skRectPtr, canvas_->GetSurface());
    }
}

void RSSurfaceCaptureTask::RSSurfaceCaptureVisitor::ProcessSurfaceRenderNodeWithUni(RSSurfaceRenderNode &node)
{
    auto geoPtr = std::static_pointer_cast<RSObjAbsGeometry>(node.GetRenderProperties().GetBoundsGeometry());
    if (geoPtr == nullptr) {
        RS_LOGI("ProcessSurfaceRenderNode node:%" PRIu64 ", get geoPtr failed", node.GetId());
        return;
    }

    canvas_->save();
    canvas_->SaveAlpha();
    canvas_->MultiplyAlpha(node.GetRenderProperties().GetAlpha() * node.GetContextAlpha());
    if (isDisplayNode_) {
        CaptureSurfaceInDisplayWithUni(node);
    } else {
        CaptureSingleSurfaceNodeWithUni(node);
    }
    canvas_->RestoreAlpha();
    canvas_->restore();
}

void RSSurfaceCaptureTask::RSSurfaceCaptureVisitor::ProcessRootRenderNode(RSRootRenderNode& node)
{
    if (!RSMainThread::Instance()->IfUseUniVisitor()) {
        return;
    }
    if (!node.ShouldPaint()) {
        RS_LOGD("ProcessRootRenderNode, no need process");
        return;
    }

    if (!canvas_) {
        RS_LOGE("ProcessRootRenderNode, canvas is nullptr");
        return;
    }

    canvas_->save();
    ProcessCanvasRenderNode(node);
    canvas_->restore();
}

void RSSurfaceCaptureTask::RSSurfaceCaptureVisitor::ProcessCanvasRenderNode(RSCanvasRenderNode& node)
{
    if (!IsUniRender()) {
        return;
    }
    if (!node.ShouldPaint()) {
        RS_LOGD("ProcessCanvasRenderNode, no need process");
        return;
    }
    if (!canvas_) {
        RS_LOGE("ProcessCanvasRenderNode, canvas is nullptr");
        return;
    }
    node.GetMutableRenderProperties().CheckEmptyBounds();
    node.ProcessRenderBeforeChildren(*canvas_);
    ProcessBaseRenderNode(node);
    node.ProcessRenderAfterChildren(*canvas_);
}

void RSSurfaceCaptureTask::RSSurfaceCaptureVisitor::CaptureSingleSurfaceNodeWithoutUni(RSSurfaceRenderNode& node)
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
    if (node.GetSecurityLayer()) {
        RS_LOGD("RSSurfaceCaptureTask::RSSurfaceCaptureVisitor::CaptureSingleSurfaceNodeWithoutUni: \
            process RSSurfaceRenderNode(id:[%" PRIu64 "]) clear white since it is security layer.",
            node.GetId());
        canvas_->save();
        canvas_->concat(translateMatrix);
        canvas_->clear(SK_ColorWHITE);
        canvas_->restore();
        return;
    }

    if (node.GetChildrenCount() > 0) {
        canvas_->concat(translateMatrix);
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
        canvas_->concat(translateMatrix);
        if (node.GetBuffer() != nullptr) {
            // in node's local coordinate.
            auto params = RSDividedRenderUtil::CreateBufferDrawParam(node, true, false, false, false);
            renderEngine_->DrawSurfaceNodeWithParams(*canvas_, node, params);
        }
        canvas_->restore();
    }
}

void RSSurfaceCaptureTask::RSSurfaceCaptureVisitor::CaptureSurfaceInDisplayWithoutUni(RSSurfaceRenderNode& node)
{
    if (node.GetSecurityLayer()) {
        RS_LOGD("RSSurfaceCaptureTask::RSSurfaceCaptureVisitor::CaptureSurfaceInDisplayWithoutUni: \
            process RSSurfaceRenderNode(id:[%" PRIu64 "]) paused since it is security layer.",
            node.GetId());
        return;
    }
    ProcessBaseRenderNode(node);
    if (node.GetBuffer() != nullptr) {
        // in display's coordinate.
        auto params = RSDividedRenderUtil::CreateBufferDrawParam(node, false, false, false, false);
        renderEngine_->DrawSurfaceNodeWithParams(*canvas_, node, params);
    }
}

void RSSurfaceCaptureTask::RSSurfaceCaptureVisitor::ProcessSurfaceRenderNodeWithoutUni(RSSurfaceRenderNode& node)
{
    if (isDisplayNode_) {
        CaptureSurfaceInDisplayWithoutUni(node);
    } else {
        CaptureSingleSurfaceNodeWithoutUni(node);
    }
}

void RSSurfaceCaptureTask::RSSurfaceCaptureVisitor::ProcessSurfaceRenderNode(RSSurfaceRenderNode &node)
{
    RS_TRACE_NAME("RSSurfaceCaptureVisitor::Process:" + node.GetName());

    if (canvas_ == nullptr) {
        RS_LOGE("ProcessSurfaceRenderNode, canvas is nullptr");
        return;
    }

    if (!node.ShouldPaint()) {
        RS_LOGD("ProcessSurfaceRenderNode node: %" PRIu64 " invisible", node.GetId());
        return;
    }

    // execute security layer in each case, ignore display snapshot and set it white for surface snapshot
    if (IsUniRender()) {
        ProcessSurfaceRenderNodeWithUni(node);
    } else {
        ProcessSurfaceRenderNodeWithoutUni(node);
    }
}
} // namespace Rosen
} // namespace OHOS
