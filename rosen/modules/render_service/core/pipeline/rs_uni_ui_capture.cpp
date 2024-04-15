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
#include <memory>
#include <sys/mman.h>

#include "include/core/SkRect.h"
#include "rs_trace.h"

#include "common/rs_common_def.h"
#include "common/rs_obj_abs_geometry.h"
#include "offscreen_render/rs_offscreen_render_thread.h"
#include "pipeline/rs_canvas_drawing_render_node.h"
#include "pipeline/rs_dirty_region_manager.h"
#include "pipeline/rs_divided_render_util.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "pipeline/rs_uni_render_util.h"
#include "platform/common/rs_log.h"
#include "render/rs_skia_filter.h"
#include "pipeline/rs_render_engine.h"

namespace OHOS {
namespace Rosen {

const int FAKE_WIDTH = 10; // When the width and height of the node are not set, use the fake width
const int FAKE_HEIGHT = 10; // When the width and height of the node are not set, use the fake height

RSUniUICapture::RSUniUICapture(NodeId nodeId, float scaleX, float scaleY)
    : nodeId_(nodeId), scaleX_(scaleX), scaleY_(scaleY)
{
    isUniRender_ = RSUniRenderJudgement::IsUniRender();
}

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
    auto recordingCanvas = std::make_shared<ExtendRecordingCanvas>(FAKE_WIDTH, FAKE_HEIGHT, false);
    PostTaskToRSRecord(recordingCanvas, node, visitor);
    auto drawCallList = recordingCanvas->GetDrawCmdList();
    std::shared_ptr<Media::PixelMap> pixelmap = CreatePixelMapByNode(node);
    if (pixelmap == nullptr) {
        RS_LOGE("RSUniUICapture::TakeLocalCapture: pixelmap == nullptr!");
        return nullptr;
    }
    RS_LOGD("RSUniUICapture::TakeLocalCapture: PixelMap: (%{public}d, %{public}d)", pixelmap->GetWidth(),
        pixelmap->GetHeight());
    auto drSurface = CreateSurface(pixelmap);
    if (drSurface == nullptr) {
        return nullptr;
    }
    auto canvas = std::make_shared<RSPaintFilterCanvas>(drSurface.get());
    RS_LOGD("RSUniUICapture::TakeLocalCapture: drawCallList size is %{public}zu", drawCallList->GetOpItemSize());
    drawCallList->Playback(*canvas);
    if (!isUniRender_ || isUseCpuSurface_) {
        return pixelmap;
    }
#if defined(ROSEN_OHOS) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    auto img = drSurface->GetImageSnapshot();
    if (!img) {
        RSOffscreenRenderThread::Instance().CleanGrResource();
        RS_LOGE("RSUniUICapture::TakeLocalCapture: img is nullptr");
        return nullptr;
    }
    if (!CopyDataToPixelMap(img, pixelmap)) {
        RSOffscreenRenderThread::Instance().CleanGrResource();
        RS_LOGE("RSUniUICapture::TakeLocalCapture: CopyDataToPixelMap failed");
        return nullptr;
    }
    RSOffscreenRenderThread::Instance().CleanGrResource();
#endif
    return pixelmap;
}

bool RSUniUICapture::CopyDataToPixelMap(std::shared_ptr<Drawing::Image> img,
    std::shared_ptr<Media::PixelMap> pixelmap)
{
    auto size = pixelmap->GetRowBytes() * pixelmap->GetHeight();
    Drawing::ImageInfo info = Drawing::ImageInfo(pixelmap->GetWidth(), pixelmap->GetHeight(),
        Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_PREMUL);
#ifdef ROSEN_OHOS
    int fd = AshmemCreate("RSUniUICapture Data", size);
    if (fd < 0) {
        RS_LOGE("RSUniUICapture::CopyDataToPixelMap AshmemCreate fd < 0");
        return false;
    }
    int result = AshmemSetProt(fd, PROT_READ | PROT_WRITE);
    if (result < 0) {
        RS_LOGE("RSUniUICapture::CopyDataToPixelMap AshmemSetProt error");
        ::close(fd);
        return false;
    }
    void* ptr = ::mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    auto data = static_cast<uint8_t*>(ptr);
    if (ptr == MAP_FAILED || ptr == nullptr) {
        RS_LOGE("RSUniUICapture::CopyDataToPixelMap data is nullptr");
        ::close(fd);
        return false;
    }
    if (!img->ReadPixels(info, data, pixelmap->GetRowBytes(), 0, 0)) {
        RS_LOGE("RSUniUICapture::CopyDataToPixelMap readPixels failed");
        ::close(fd);
        return false;
    }
    void* fdPtr = new int32_t();
    *static_cast<int32_t*>(fdPtr) = fd;
    pixelmap->SetPixelsAddr(data, fdPtr, size, Media::AllocatorType::SHARE_MEM_ALLOC, nullptr);
#else
    auto data = static_cast<uint8_t *>(size);
    if (data == nullptr) {
        RS_LOGE("RSUniUICapture::CopyDataToPixelMap data is nullptr");
        return false;
    }
    if (!img->ReadPixels(info, data, pixelmap->GetRowBytes(), 0, 0)) {
        RS_LOGE("RSUniUICapture::CopyDataToPixelMap readPixels failed");
        free(data);
        data = nullptr;
        return false;
    }
    pixelmap->SetPixelsAddr(data, nullptr, size, Media::AllocatorType::HEAP_ALLOC, nullptr);
#endif
    return true;
}

std::shared_ptr<Media::PixelMap> RSUniUICapture::CreatePixelMapByNode(std::shared_ptr<RSRenderNode> node) const
{
    float pixmapWidth = node->GetRenderProperties().GetBoundsWidth();
    float pixmapHeight = node->GetRenderProperties().GetBoundsHeight();
    Media::InitializationOptions opts;
    opts.size.width = ceil(pixmapWidth * scaleX_);
    opts.size.height = ceil(pixmapHeight * scaleY_);
    return Media::PixelMap::Create(opts);
}

std::shared_ptr<Drawing::Surface> RSUniUICapture::CreateSurface(
    const std::shared_ptr<Media::PixelMap>& pixelmap)
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

    Drawing::ImageInfo info = Drawing::ImageInfo{pixelmap->GetWidth(), pixelmap->GetHeight(),
        Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL};
    if (!isUniRender_) {
        return Drawing::Surface::MakeRasterDirect(info, address, pixelmap->GetRowBytes());
    }
    std::shared_ptr<Drawing::Surface> surface;
#if defined(ROSEN_OHOS) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    auto renderContext = RSOffscreenRenderThread::Instance().GetRenderContext();
    if (renderContext == nullptr) {
        RS_LOGE("RSUniUICapture::CreateSurface: renderContext is nullptr");
        return nullptr;
    }
    surface = Drawing::Surface::MakeRenderTarget(renderContext->GetDrGPUContext(), false, info);
#else
    surface = Drawing::Surface::MakeRasterDirect(info, address, pixelmap->GetRowBytes());
    isUseCpuSurface_ = true;
#endif
    if (!surface) {
        surface = Drawing::Surface::MakeRasterDirect(info, address, pixelmap->GetRowBytes());
        isUseCpuSurface_ = true;
    }
    return surface;
}

RSUniUICapture::RSUniUICaptureVisitor::RSUniUICaptureVisitor(NodeId nodeId, float scaleX, float scaleY)
    : nodeId_(nodeId), scaleX_(scaleX), scaleY_(scaleY)
{
    // Avoid RS restart issue temperorily
    renderEngine_ = std::make_shared<RSRenderEngine>();
    renderEngine_->Init();
    isUniRender_ = RSUniRenderJudgement::IsUniRender();
    auto node = RSMainThread::Instance()->GetContext().GetNodeMap().GetRenderNode<RSRenderNode>(nodeId_);
    if (node == nullptr) {
        RS_LOGE("RSUniUICapture::TakeLocalCapture node is nullptr return");
        return;
    }
    const auto& targetNodeProperty = node->GetRenderProperties();
    auto targetNodeGeoPtr = (targetNodeProperty.GetBoundsGeometry());
    captureMatrix_.Set(Drawing::Matrix::Index::SCALE_X, scaleX_);
    captureMatrix_.Set(Drawing::Matrix::Index::SCALE_X, scaleY_);
    Drawing::Matrix invertMatrix;
    if (targetNodeGeoPtr->GetAbsMatrix().Invert(invertMatrix)) {
        captureMatrix_.PreConcat(invertMatrix);
    }
}

void RSUniUICapture::PostTaskToRSRecord(std::shared_ptr<ExtendRecordingCanvas> canvas,
    std::shared_ptr<RSRenderNode> node, std::shared_ptr<RSUniUICaptureVisitor> visitor)
{
    std::function<void()> recordingDrawCall = [canvas, node, visitor]() -> void {
        visitor->SetCanvas(canvas);
        node->ApplyModifiers();
        node->PrepareChildrenForApplyModifiers();
        node->Process(visitor);
    };
    RSMainThread::Instance()->PostSyncTask(recordingDrawCall);
}

void RSUniUICapture::RSUniUICaptureVisitor::SetPaintFilterCanvas(std::shared_ptr<RSPaintFilterCanvas> canvas)
{
    if (canvas == nullptr) {
        RS_LOGE("RSUniUICaptureVisitor::SetCanvas: canvas == nullptr");
        return;
    }
    canvas_ = canvas;
    canvas_->Scale(scaleX_, scaleY_);
    canvas_->SetDisableFilterCache(true);
}

void RSUniUICapture::RSUniUICaptureVisitor::SetCanvas(std::shared_ptr<ExtendRecordingCanvas> canvas)
{
    if (canvas == nullptr) {
        RS_LOGE("RSUniUICaptureVisitor::SetCanvas: canvas == nullptr");
        return;
    }
    auto renderContext = RSMainThread::Instance()->GetRenderEngine()->GetRenderContext();
    canvas->SetGrRecordingContext(renderContext->GetSharedDrGPUContext());
    canvas_ = std::make_shared<RSPaintFilterCanvas>(canvas.get());
    canvas_->Scale(scaleX_, scaleY_);
    canvas_->SetDisableFilterCache(true);
    canvas_->SetRecordingState(true);
    canvas_->SetCacheType(Drawing::CacheType::DISABLED);
}

void RSUniUICapture::RSUniUICaptureVisitor::ProcessChildren(RSRenderNode& node)
{
    auto sortedChildren = node.GetSortedChildren();
    if (node.GetRenderProperties().GetUseShadowBatching()) {
        for (auto& child : *sortedChildren) {
            if (auto node = child->ReinterpretCastTo<RSCanvasRenderNode>()) {
                node->ProcessShadowBatching(*canvas_);
            }
        }
    }
    for (auto& child : *sortedChildren) {
        child->Process(shared_from_this());
    }
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

    canvas_->Save();
    ProcessCanvasRenderNode(node);
    canvas_->Restore();
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
        auto geoPtr = (property.GetBoundsGeometry());
        Drawing::Matrix relativeMatrix = Drawing::Matrix();
        relativeMatrix.Set(Drawing::Matrix::Index::SCALE_X, scaleX_);
        relativeMatrix.Set(Drawing::Matrix::Index::SCALE_Y, scaleY_);
        Drawing::Matrix invertMatrix;
        if (geoPtr->GetMatrix().Invert(invertMatrix)) {
            relativeMatrix.PreConcat(invertMatrix);
        }
        canvas_->SetMatrix(relativeMatrix);
    }
    node.ProcessRenderBeforeChildren(*canvas_);
    if (node.GetType() == RSRenderNodeType::CANVAS_DRAWING_NODE) {
        auto canvasDrawingNode = node.ReinterpretCastTo<RSCanvasDrawingRenderNode>();
        if (!canvasDrawingNode->IsOnTheTree()) {
            auto clearFunc = [id = UNI_MAIN_THREAD_INDEX](std::shared_ptr<Drawing::Surface> surface) {
                // The second param is null, 0 is an invalid value.
                RSUniRenderUtil::ClearNodeCacheSurface(std::move(surface), nullptr, id, 0);
            };
            canvasDrawingNode->SetSurfaceClearFunc({ UNI_MAIN_THREAD_INDEX, clearFunc });
            canvasDrawingNode->ProcessRenderContents(*canvas_);
        } else {
            auto image = canvasDrawingNode->GetImage(UNI_MAIN_THREAD_INDEX);
            if (image) {
                canvas_->DrawImage(*image, 0, 0, Drawing::SamplingOptions());
            }
        }
    } else {
        node.ProcessRenderContents(*canvas_);
    }
    ProcessChildren(node);
    node.ProcessRenderAfterChildren(*canvas_);
}

void RSUniUICapture::RSUniUICaptureVisitor::ProcessEffectRenderNode(RSEffectRenderNode& node)
{
    if (!node.ShouldPaint()) {
        RS_LOGD("RSUniUICapture::RSUniUICaptureVisitor::ProcessEffectRenderNode, no need process");
        return;
    }
    if (!canvas_) {
        RS_LOGE("RSUniUICapture::RSUniUICaptureVisitor::ProcessEffectRenderNode, canvas is nullptr");
        return;
    }
    node.ProcessRenderBeforeChildren(*canvas_);
    ProcessChildren(node);
    node.ProcessRenderAfterChildren(*canvas_);
}

void RSUniUICapture::RSUniUICaptureVisitor::ProcessSurfaceRenderNode(RSSurfaceRenderNode& node)
{
    if (canvas_ == nullptr) {
        RS_LOGE("RSUniUICaptureVisitor::ProcessSurfaceRenderNode, canvas is nullptr");
        return;
    }

    if (!node.ShouldPaint()) {
        RS_LOGD("RSUniUICaptureVisitor::ProcessSurfaceRenderNode node: %{public}" PRIu64 " invisible", node.GetId());
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
    auto geoPtr = (node.GetRenderProperties().GetBoundsGeometry());
    if (geoPtr == nullptr) {
        RS_LOGI(
            "RSUniUICaptureVisitor::ProcessSurfaceRenderNode node:%{public}" PRIu64 ", get geoPtr failed",
                node.GetId());
        return;
    }
    Drawing::AutoCanvasRestore acr(*canvas_, true);
    canvas_->MultiplyAlpha(node.GetRenderProperties().GetAlpha());
    ProcessSurfaceViewWithUni(node);
}

void RSUniUICapture::RSUniUICaptureVisitor::ProcessSurfaceViewWithUni(RSSurfaceRenderNode& node)
{
    canvas_->Save();

    const auto& property = node.GetRenderProperties();
    auto geoPtr = (property.GetBoundsGeometry());
    if (!geoPtr) {
        RS_LOGE("RSUniUICaptureVisitor::ProcessSurfaceViewWithUni node:%{public}" PRIu64 ", get geoPtr failed",
            node.GetId());
        return;
    }
    canvas_->SetMatrix(captureMatrix_);
    canvas_->ConcatMatrix(geoPtr->GetAbsMatrix());

    bool isSelfDrawingSurface = node.GetSurfaceNodeType() == RSSurfaceNodeType::SELF_DRAWING_NODE;
    const RectF absBounds = { 0, 0, property.GetBoundsWidth(), property.GetBoundsHeight() };
    RRect absClipRRect = RRect(absBounds, property.GetCornerRadius());
    if (isSelfDrawingSurface) {
        RSPropertiesPainter::DrawShadow(property, *canvas_, &absClipRRect);
        RSPropertiesPainter::DrawOutline(property, *canvas_);
    }
    canvas_->Save();
    if (isSelfDrawingSurface && !property.GetCornerRadius().IsZero()) {
        canvas_->ClipRoundRect(RSPropertiesPainter::RRect2DrawingRRect(absClipRRect),
            Drawing::ClipOp::INTERSECT, true);
    } else {
        canvas_->ClipRect(Drawing::Rect(0, 0, property.GetBoundsWidth(), property.GetBoundsHeight()),
            Drawing::ClipOp::INTERSECT, false);
    }
    if (isSelfDrawingSurface) {
        RSPropertiesPainter::DrawBackground(property, *canvas_);
        RSPropertiesPainter::DrawMask(property, *canvas_);
        RSPropertiesPainter::DrawFilter(property, *canvas_, FilterType::BACKGROUND_FILTER);
    } else {
        auto backgroundColor = static_cast<Drawing::ColorQuad>(property.GetBackgroundColor().AsArgbInt());
        if (Drawing::Color::ColorQuadGetA(backgroundColor) != Drawing::Color::COLOR_TRANSPARENT) {
            canvas_->DrawColor(backgroundColor);
        }
    }
    canvas_->Restore();
    if (node.GetBuffer() != nullptr) {
        if (auto recordingCanvas = static_cast<ExtendRecordingCanvas*>(canvas_->GetRecordingCanvas())) {
            auto params = RSUniRenderUtil::CreateBufferDrawParam(node, false);
            auto buffer = node.GetBuffer();
            DrawingSurfaceBufferInfo rsSurfaceBufferInfo(buffer, params.dstRect.GetLeft(), params.dstRect.GetTop(),
                params.dstRect.GetWidth(), params.dstRect.GetHeight());
            recordingCanvas->ConcatMatrix(params.matrix);
            recordingCanvas->DrawSurfaceBuffer(rsSurfaceBufferInfo);
        } else {
            auto params = RSUniRenderUtil::CreateBufferDrawParam(node, false);
            renderEngine_->DrawSurfaceNodeWithParams(*canvas_, node, params);
        }
    }
    if (isSelfDrawingSurface) {
        RSPropertiesPainter::DrawFilter(property, *canvas_, FilterType::FOREGROUND_FILTER);
    }
    canvas_->Restore();
    ProcessChildren(node);
}

void RSUniUICapture::RSUniUICaptureVisitor::ProcessSurfaceViewWithoutUni(RSSurfaceRenderNode& node)
{
    Drawing::Matrix translateMatrix;
    auto parentPtr = node.GetParent().lock();
    if (parentPtr != nullptr && parentPtr->IsInstanceOf<RSSurfaceRenderNode>()) {
        // calculate the offset from this node's parent, and perform translate.
        auto parentNode = std::static_pointer_cast<RSSurfaceRenderNode>(parentPtr);
        const float parentNodeTranslateX = parentNode->GetTotalMatrix().Get(Drawing::Matrix::Index::TRANS_X);
        const float parentNodeTranslateY = parentNode->GetTotalMatrix().Get(Drawing::Matrix::Index::TRANS_Y);
        const float thisNodetranslateX = node.GetTotalMatrix().Get(Drawing::Matrix::Index::TRANS_X);
        const float thisNodetranslateY = node.GetTotalMatrix().Get(Drawing::Matrix::Index::TRANS_Y);
        translateMatrix.PreTranslate(
            thisNodetranslateX - parentNodeTranslateX, thisNodetranslateY - parentNodeTranslateY);
    }
    if (node.GetChildrenCount() > 0) {
        if (node.GetId() != nodeId_) {
            canvas_->ConcatMatrix(translateMatrix);
        }
        const auto saveCnt = canvas_->Save();
        ProcessChildren(node);
        canvas_->RestoreToCount(saveCnt);
        if (node.GetBuffer() != nullptr) {
            // in node's local coordinate.
            auto params = RSDividedRenderUtil::CreateBufferDrawParam(node, true, false, true, false);
            renderEngine_->DrawSurfaceNodeWithParams(*canvas_, node, params);
        }
    } else {
        canvas_->Save();
        if (node.GetId() != nodeId_) {
            canvas_->ConcatMatrix(translateMatrix);
        }
        if (node.GetBuffer() != nullptr) {
            // in node's local coordinate.
            auto params = RSDividedRenderUtil::CreateBufferDrawParam(node, true, false, true, false);
            renderEngine_->DrawSurfaceNodeWithParams(*canvas_, node, params);
        }
        canvas_->Restore();
    }
}

void RSUniUICapture::RSUniUICaptureVisitor::PrepareChildren(RSRenderNode& node)
{
    for (auto& child : *node.GetSortedChildren()) {
        child->Prepare(shared_from_this());
    }
}

void RSUniUICapture::RSUniUICaptureVisitor::PrepareCanvasRenderNode(RSCanvasRenderNode& node)
{
    auto dirtyManager = std::make_shared<RSDirtyRegionManager>();
    node.Update(*dirtyManager, nullptr, false);
    PrepareChildren(node);
}

void RSUniUICapture::RSUniUICaptureVisitor::PrepareSurfaceRenderNode(RSSurfaceRenderNode& node)
{
    auto dirtyManager = std::make_shared<RSDirtyRegionManager>();
    node.Update(*dirtyManager, nullptr, false);
    PrepareChildren(node);
}

void RSUniUICapture::RSUniUICaptureVisitor::PrepareRootRenderNode(RSRootRenderNode& node)
{
    PrepareCanvasRenderNode(node);
}

void RSUniUICapture::RSUniUICaptureVisitor::PrepareEffectRenderNode(RSEffectRenderNode& node)
{
    auto dirtyManager = std::make_shared<RSDirtyRegionManager>();
    node.Update(*dirtyManager, nullptr, false);
    PrepareChildren(node);
}
} // namespace Rosen
} // namespace OHOS
