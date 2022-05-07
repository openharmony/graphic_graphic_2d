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
#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_render_service_util.h"
#include "pipeline/rs_render_service_connection.h"
#include "pipeline/rs_surface_render_node.h"
#include "platform/common/rs_log.h"
#include "platform/drawing/rs_surface.h"
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
    if (auto surfaceNode = node->ReinterpretCastTo<RSSurfaceRenderNode>()) {
        RS_LOGI("RSSurfaceCaptureTask::Run: Into SURFACE_NODE SurfaceRenderNodeId:[%llu]", node->GetId());
        pixelmap = CreatePixelMapBySurfaceNode(surfaceNode);
        visitor->IsDisplayNode(false);
    } else if (auto displayNode = node->ReinterpretCastTo<RSDisplayRenderNode>()) {
        RS_LOGI("RSSurfaceCaptureTask::Run: Into DISPLAY_NODE DisplayRenderNodeId:[%llu]", node->GetId());
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
    std::unique_ptr<SkCanvas> canvas = CreateCanvas(pixelmap);
    if (canvas == nullptr) {
        RS_LOGE("RSSurfaceCaptureTask::Run: canvas is nullptr!");
        return nullptr;
    }
    visitor->SetCanvas(canvas.get());
    visitor->SetScale(scaleX_, scaleY_);
    node->Process(visitor);
    return pixelmap;
}

std::unique_ptr<Media::PixelMap> RSSurfaceCaptureTask::CreatePixelMapBySurfaceNode(
    std::shared_ptr<RSSurfaceRenderNode> node)
{
    if (node == nullptr) {
        RS_LOGE("CreatePixelMapBySurfaceNode: node == nullptr");
        return nullptr;
    }
    if (node->GetBuffer() == nullptr) {
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
    auto rotation = screenManager->GetRotation(screenId);
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

std::unique_ptr<SkCanvas> RSSurfaceCaptureTask::CreateCanvas(const std::unique_ptr<Media::PixelMap>& pixelmap)
{
    if (pixelmap == nullptr) {
        RS_LOGE("RSSurfaceCaptureTask::CreateCanvas: pixelmap == nullptr");
        return nullptr;
    }
    auto address = const_cast<uint32_t*>(pixelmap->GetPixel32(0, 0));
    if (address == nullptr) {
        RS_LOGE("RSSurfaceCaptureTask::CreateCanvas: address == nullptr");
        return nullptr;
    }
    SkImageInfo info = SkImageInfo::Make(pixelmap->GetWidth(), pixelmap->GetHeight(),
            kRGBA_8888_SkColorType, kPremul_SkAlphaType);
    return SkCanvas::MakeRasterDirect(info, address, pixelmap->GetRowBytes());
}

void RSSurfaceCaptureTask::RSSurfaceCaptureVisitor::SetCanvas(SkCanvas* canvas)
{
    if (canvas == nullptr) {
        RS_LOGE("RSSurfaceCaptureTask::RSSurfaceCaptureVisitor::SetCanvas: address == nullptr");
        return;
    }
    canvas_ = std::make_unique<RSPaintFilterCanvas>(canvas);
}

void RSSurfaceCaptureTask::RSSurfaceCaptureVisitor::ProcessDisplayRenderNode(RSDisplayRenderNode &node)
{
    RS_LOGD("RsDebug RSSurfaceCaptureVisitor::ProcessDisplayRenderNode child size:[%d] total size:[%d]",
        node.GetChildrenCount(), node.GetSortedChildren().size());
    for (auto child : node.GetSortedChildren()) {
        child->Process(shared_from_this());
    }
    // clear SortedChildren, it will be generated again in next frame
    node.ResetSortedChildren();
}

static void AdjustSurfaceTransform(BufferDrawParam &param, TransformType surfaceTransform)
{
    switch (surfaceTransform) {
        case TransformType::ROTATE_90: {
            param.matrix.preTranslate(0, param.clipRect.height());
            param.matrix.preRotate(-90); // rotate 90 degrees anti-clockwise at last.
            param.dstRect.setWH(param.dstRect.height(), param.dstRect.width());
            break;
        }
        case TransformType::ROTATE_180: {
            param.matrix.preTranslate(param.clipRect.width(), param.clipRect.height());
            param.matrix.preRotate(-180); // rotate 180 degrees anti-clockwise at last.
            param.dstRect.setWH(param.dstRect.height(), param.dstRect.width());
            break;
        }
        case TransformType::ROTATE_270: {
            param.matrix.preTranslate(param.clipRect.width(), 0);
            param.matrix.preRotate(-270); // rotate 270 degrees anti-clockwise at last.
            param.dstRect.setWH(param.dstRect.height(), param.dstRect.width());
            break;
        }
        default: {
            break;
        }
    }
}

void RSSurfaceCaptureTask::RSSurfaceCaptureVisitor::ProcessSurfaceRenderNode(RSSurfaceRenderNode &node)
{
    if (node.GetSecurityLayer()) {
        RS_LOGD("RSSurfaceCaptureTask::RSSurfaceCaptureVisitor::ProcessSurfaceRenderNode: \
            process RSSurfaceRenderNode(id:[%llu]) paused, because surfaceNode is the security layer.", node.GetId());
        return;
    }
    if (node.GetBuffer() == nullptr) {
        RS_LOGD("RSSurfaceCaptureTask::RSSurfaceCaptureVisitor::ProcessSurfaceRenderNode: node Buffer is nullptr!");
        return;
    }
    sptr<Surface> surface = node.GetConsumer();
    if (surface == nullptr) {
        return;
    }
    const auto surfaceTransform = surface->GetTransform();

    for (auto child : node.GetSortedChildren()) {
        child->Process(shared_from_this());
    }
    // clear SortedChildren, it will be generated again in next frame
    node.ResetSortedChildren();

    auto param = RsRenderServiceUtil::CreateBufferDrawParam(node);
    if (!isDisplayNode_) {
        if (param.clipRect.isEmpty()) {
            return;
        }
        auto existedParent = node.GetParent().lock();
        if (existedParent && existedParent->IsInstanceOf<RSSurfaceRenderNode>()) {
            param.matrix = node.GetMatrix();
            auto& parent = *std::static_pointer_cast<RSSurfaceRenderNode>(existedParent);
            auto parentRect = RsRenderServiceUtil::CreateBufferDrawParam(parent).clipRect;
            //Changes the clip area from absolute to relative to the parent window and deal with clip area with scale
            //Based on the origin of the parent window.
            param.clipRect.offsetTo(param.clipRect.left() - parentRect.left(), param.clipRect.top() - parentRect.top());
            SkMatrix scaleMatrix = SkMatrix::I();
            scaleMatrix.preScale(scaleX_, scaleY_, 0, 0);
            param.clipRect = scaleMatrix.mapRect(param.clipRect);

            param.dstRect = SkRect::MakeXYWH(
                node.GetRenderProperties().GetBoundsPositionX(), node.GetRenderProperties().GetBoundsPositionY(),
                node.GetRenderProperties().GetBoundsWidth(), node.GetRenderProperties().GetBoundsHeight());
            param.matrix.preTranslate(-param.matrix.getTranslateX() * scaleX_, -param.matrix.getTranslateY() * scaleY_);
            AdjustSurfaceTransform(param, surfaceTransform);
            RsRenderServiceUtil::DrawBuffer(*canvas_, param,
                [this](SkCanvas& canvas, BufferDrawParam& params) -> void {
                    canvas.scale(scaleX_, scaleY_);
                });
        } else {
            param.matrix = SkMatrix::I();
            param.clipRect.offsetTo(0, 0);
            param.dstRect = SkRect::MakeXYWH(0, 0, node.GetRenderProperties().GetBoundsWidth(),
                node.GetRenderProperties().GetBoundsHeight());
            AdjustSurfaceTransform(param, surfaceTransform);
            RsRenderServiceUtil::DrawBuffer(*canvas_, param, [this](SkCanvas& canvas, BufferDrawParam& params) -> void {
                    canvas.scale(scaleX_, scaleY_);
                });
        }
    } else {
        param.clipRect = SkRect::MakeXYWH(floor(param.clipRect.left() * scaleX_),
            floor(param.clipRect.top() * scaleY_), ceil(param.clipRect.width() * scaleX_),
            ceil(param.clipRect.height() * scaleY_));
        param.dstRect = SkRect::MakeXYWH(0, 0, node.GetRenderProperties().GetBoundsWidth() * scaleX_,
            node.GetRenderProperties().GetBoundsHeight() * scaleY_);
        if (surfaceTransform == TransformType::ROTATE_90 || surfaceTransform == TransformType::ROTATE_270) {
            param.dstRect.setWH(param.dstRect.height(), param.dstRect.width());
        }
        RsRenderServiceUtil::DrawBuffer(*canvas_, param, [this](SkCanvas& canvas, BufferDrawParam& params) -> void {
            canvas.translate(floor(params.dstRect.left() * scaleX_ - params.dstRect.left()),
                floor(params.dstRect.top() * scaleY_ - params.dstRect.top()));
            canvas.scale(scaleX_, scaleY_);
        });
    }
}
} // namespace Rosen
} // namespace OHOS
