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
#include "rs_trace.h"

#include "common/rs_obj_abs_geometry.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkMatrix.h"
#include "include/core/SkRect.h"
#include "pipeline/rs_base_render_node.h"
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
    std::shared_ptr<RSSurfaceCaptureVisitor> visitor = std::make_shared<RSSurfaceCaptureVisitor>();
    visitor->SetUniRender(RSUniRenderJudgement::IsUniRender());
    if (auto surfaceNode = node->ReinterpretCastTo<RSSurfaceRenderNode>()) {
        RS_LOGI("RSSurfaceCaptureTask::Run: Into SURFACE_NODE SurfaceRenderNodeId:[%" PRIu64 "]", node->GetId());
        pixelmap = CreatePixelMapBySurfaceNode(surfaceNode, visitor->IsUniRender());
        visitor->IsDisplayNode(false);
    } else if (auto displayNode = node->ReinterpretCastTo<RSDisplayRenderNode>()) {
        RS_LOGI("RSSurfaceCaptureTask::Run: Into DISPLAY_NODE DisplayRenderNodeId:[%" PRIu64 "]", node->GetId());
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
    visitor->SetScale(scaleX_, scaleY_);
    node->Process(visitor);
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
    return SkSurface::MakeRasterDirect(info, address, pixelmap->GetRowBytes());
}

RSSurfaceCaptureTask::RSSurfaceCaptureVisitor::RSSurfaceCaptureVisitor()
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
    for (auto& child : node.GetSortedChildren()) {
        child->Process(shared_from_this());
    }
    // clear SortedChildren, it will be generated again in next frame
    node.ResetSortedChildren();

    auto boundsGeoPtr = std::static_pointer_cast<RSObjAbsGeometry>(node.GetRenderProperties().GetBoundsGeometry());
    if (boundsGeoPtr != nullptr && canvas_ != nullptr) {
        boundsGeoPtr->UpdateByMatrixFromSelf();
        canvas_->concat(boundsGeoPtr->GetMatrix());
    }
}

void RSSurfaceCaptureTask::RSSurfaceCaptureVisitor::ProcessSurfaceRenderNodeWithUni(RSSurfaceRenderNode &node)
{
    auto geoPtr = std::static_pointer_cast<RSObjAbsGeometry>(node.GetRenderProperties().GetBoundsGeometry());
    canvas_->save();
    canvas_->scale(scaleX_, scaleY_);
    canvas_->SaveAlpha();
    canvas_->MultiplyAlpha(node.GetRenderProperties().GetAlpha() * node.GetContextAlpha());
    canvas_->concat(node.GetContextMatrix());
    auto contextClipRect = node.GetContextClipRegion();
    if (!contextClipRect.isEmpty()) {
        canvas_->clipRect(contextClipRect);
    }
    auto matrix = geoPtr->GetMatrix();
    matrix.setTranslateX(std::ceil(matrix.getTranslateX()));
    matrix.setTranslateY(std::ceil(matrix.getTranslateY()));
    canvas_->concat(matrix);
    canvas_->clipRect(SkRect::MakeWH(std::floor(node.GetRenderProperties().GetBoundsWidth()),
        std::floor(node.GetRenderProperties().GetBoundsHeight())));
    ProcessBaseRenderNode(node);
    if (node.GetConsumer() != nullptr) {
        RS_TRACE_NAME("UniRender::Process:" + node.GetName());
        if (node.GetBuffer() == nullptr) {
            RS_LOGD(
                "RSSurfaceCaptureVisitor::ProcessSurfaceRenderNode:%" PRIu64 " buffer is not available", node.GetId());
        } else {
            node.NotifyRTBufferAvailable();
            auto params = RSBaseRenderUtil::CreateBufferDrawParam(node, true); // in node's local coordinate.
            renderEngine_->DrawSurfaceNodeWithParams(*canvas_, node, params);
        }
    }
    canvas_->RestoreAlpha();
    canvas_->restore();
}

void RSSurfaceCaptureTask::RSSurfaceCaptureVisitor::ProcessRootRenderNode(RSRootRenderNode& node)
{
    if (!node.GetRenderProperties().GetVisible()) {
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
    if (!node.GetRenderProperties().GetVisible()) {
        RS_LOGD("ProcessCanvasRenderNode, no need process");
        return;
    }
    if (!canvas_) {
        RS_LOGE("ProcessCanvasRenderNode, canvas is nullptr");
        return;
    }
    node.ProcessRenderBeforeChildren(*canvas_);
    ProcessBaseRenderNode(node);
    node.ProcessRenderAfterChildren(*canvas_);
}

void RSSurfaceCaptureTask::RSSurfaceCaptureVisitor::ProcessSurfaceRenderNodeWithoutUni(RSSurfaceRenderNode& node)
{
    if (node.GetBuffer() == nullptr) {
        RS_LOGD("RSSurfaceCaptureTask::RSSurfaceCaptureVisitor::ProcessSurfaceRenderNode: node Buffer is nullptr!");
        for (auto child : node.GetSortedChildren()) {
            child->Process(shared_from_this());
        }
        if (node.GetSortedChildren().size() > 0) {
            node.ResetSortedChildren();
        }
        return;
    }
    sptr<Surface> surface = node.GetConsumer();
    if (surface == nullptr) {
        return;
    }

    for (auto child : node.GetSortedChildren()) {
        child->Process(shared_from_this());
    }
    // clear SortedChildren, it will be generated again in next frame
    node.ResetSortedChildren();

    canvas_->save();
    if (!isDisplayNode_) {
        auto processFunc = [this](SkCanvas& canvas, BufferDrawParam& params) {
            canvas.scale(scaleX_, scaleY_);
        };
        auto params = RSBaseRenderUtil::CreateBufferDrawParam(node, true); // in node's local coordinate.
        renderEngine_->DrawSurfaceNodeWithParams(*canvas_, node, params, processFunc);
    } else {
        auto processFunc = [this](SkCanvas& canvas, BufferDrawParam& params) {
            canvas.translate(
                floor(params.dstRect.left() * scaleX_ - params.dstRect.left()),
                floor(params.dstRect.top() * scaleY_ - params.dstRect.top()));
            canvas.scale(scaleX_, scaleY_);
        };
        auto params = RSBaseRenderUtil::CreateBufferDrawParam(node, false); // in display's coordinate.
        renderEngine_->DrawSurfaceNodeWithParams(*canvas_, node, params, processFunc);
    }
    canvas_->restore();
}

void RSSurfaceCaptureTask::RSSurfaceCaptureVisitor::ProcessSurfaceRenderNode(RSSurfaceRenderNode &node)
{
    RS_TRACE_NAME("RSSurfaceCaptureVisitor::Process:" + node.GetName());

    if (canvas_ == nullptr) {
        RS_LOGE("ProcessSurfaceRenderNode, canvas is nullptr");
        return;
    }

    if (!node.GetRenderProperties().GetVisible()) {
        RS_LOGD("ProcessSurfaceRenderNode node: %" PRIu64 " invisible", node.GetId());
        return;
    }

    auto geoPtr = std::static_pointer_cast<RSObjAbsGeometry>(node.GetRenderProperties().GetBoundsGeometry());
    if (geoPtr == nullptr) {
        RS_LOGI("ProcessSurfaceRenderNode node:%" PRIu64 ", get geoPtr failed", node.GetId());
        return;
    }

    if (node.GetSecurityLayer()) {
        RS_LOGD("RSSurfaceCaptureTask::RSSurfaceCaptureVisitor::ProcessSurfaceRenderNode: \
            process RSSurfaceRenderNode(id:[%" PRIu64 "]) paused, because surfaceNode is the security layer.",
            node.GetId());
        return;
    }

    if (IsUniRender()) {
        ProcessSurfaceRenderNodeWithUni(node);
    } else {
        ProcessSurfaceRenderNodeWithoutUni(node);
    }
}
} // namespace Rosen
} // namespace OHOS
