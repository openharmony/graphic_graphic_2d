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
#ifndef USE_ROSEN_DRAWING
    auto recordingCanvas = std::make_shared<RSRecordingCanvas>(FAKE_WIDTH, FAKE_HEIGHT);
#else
    auto recordingCanvas = std::make_shared<ExtendRecordingCanvas>(FAKE_WIDTH, FAKE_HEIGHT, false);
#endif
    PostTaskToRSRecord(recordingCanvas, node, visitor);
    auto drawCallList = recordingCanvas->GetDrawCmdList();
    std::shared_ptr<Media::PixelMap> pixelmap = CreatePixelMapByNode(node);
    if (pixelmap == nullptr) {
        RS_LOGE("RSUniUICapture::TakeLocalCapture: pixelmap == nullptr!");
        return nullptr;
    }
    RS_LOGD("RSUniUICapture::TakeLocalCapture: PixelMap: (%{public}d, %{public}d)", pixelmap->GetWidth(),
        pixelmap->GetHeight());
#ifndef USE_ROSEN_DRAWING
    auto skSurface = CreateSurface(pixelmap);
    if (skSurface == nullptr) {
        RS_LOGE("RSUniUICapture::TakeLocalCapture: skSurface == nullptr!");
        return nullptr;
    }
    auto canvas = std::make_shared<RSPaintFilterCanvas>(skSurface.get());
#else
    auto drSurface = CreateSurface(pixelmap);
    if (drSurface == nullptr) {
        return nullptr;
    }
    auto canvas = std::make_shared<RSPaintFilterCanvas>(drSurface.get());
#endif
    RS_LOGD("RSUniUICapture::TakeLocalCapture: drawCallList size is %{public}zu", drawCallList->GetOpItemSize());
    drawCallList->Playback(*canvas);
    if (!isUniRender_ || isUseCpuSurface_) {
        return pixelmap;
    }
#if defined(ROSEN_OHOS) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)) && defined(RS_ENABLE_EGLIMAGE)
#ifndef USE_ROSEN_DRAWING
    sk_sp<SkImage> img(skSurface.get()->makeImageSnapshot());
#else
    auto img = drSurface->GetImageSnapshot();
#endif
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

#ifndef USE_ROSEN_DRAWING
bool RSUniUICapture::CopyDataToPixelMap(sk_sp<SkImage> img, std::shared_ptr<Media::PixelMap> pixelmap)
#else
bool RSUniUICapture::CopyDataToPixelMap(std::shared_ptr<Drawing::Image> img,
    std::shared_ptr<Media::PixelMap> pixelmap)
#endif
{
    auto size = pixelmap->GetRowBytes() * pixelmap->GetHeight();
#ifndef USE_ROSEN_DRAWING
    SkImageInfo info = SkImageInfo::Make(pixelmap->GetWidth(), pixelmap->GetHeight(),
        kRGBA_8888_SkColorType, kPremul_SkAlphaType);
#else
    Drawing::ImageInfo info = Drawing::ImageInfo(pixelmap->GetWidth(), pixelmap->GetHeight(),
        Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_PREMUL);
#endif
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
#ifndef USE_ROSEN_DRAWING
    if (!img->readPixels(info, data, pixelmap->GetRowBytes(), 0, 0)) {
#else
    if (!img->ReadPixels(info, data, pixelmap->GetRowBytes(), 0, 0)) {
#endif // USE_ROSEN_DRAWING
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
#ifndef USE_ROSEN_DRAWING
    if (!img->readPixels(info, data, pixelmap->GetRowBytes(), 0, 0)) {
#else
    if (!img->ReadPixels(info, data, pixelmap->GetRowBytes(), 0, 0)) {
#endif // USE_ROSEN_DRAWING
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

#ifndef USE_ROSEN_DRAWING
sk_sp<SkSurface> RSUniUICapture::CreateSurface(const std::shared_ptr<Media::PixelMap>& pixelmap)
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
    if (!isUniRender_) {
        return SkSurface::MakeRasterDirect(info, address, pixelmap->GetRowBytes());
    }
#if defined(ROSEN_OHOS) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)) && defined(RS_ENABLE_EGLIMAGE)
#if defined(NEW_RENDER_CONTEXT)
    auto drawingContext = RSOffscreenRenderThread::Instance().GetRenderContext();
    if (drawingContext == nullptr) {
        RS_LOGE("RSUniUICapture::CreateSurface: renderContext is nullptr");
        return nullptr;
    }
    return SkSurface::MakeRenderTarget(drawingContext->GetDrawingContext(), SkBudgeted::kNo, info);
#else
    auto renderContext = RSOffscreenRenderThread::Instance().GetRenderContext();
    if (renderContext == nullptr) {
        RS_LOGE("RSUniUICapture::CreateSurface: renderContext is nullptr");
        return nullptr;
    }
    return SkSurface::MakeRenderTarget(renderContext->GetGrContext(), SkBudgeted::kNo, info);
#endif
#endif
    return SkSurface::MakeRasterDirect(info, address, pixelmap->GetRowBytes());
}
#else
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
#if defined(ROSEN_OHOS) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)) && defined(RS_ENABLE_EGLIMAGE)
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
#endif

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
    auto targetNodeGeoPtr = (targetNodeProperty.GetBoundsGeometry());
#ifndef USE_ROSEN_DRAWING
    captureMatrix_.setScaleX(scaleX_);
    captureMatrix_.setScaleY(scaleY_);
    SkMatrix invertMatrix;
    if (targetNodeGeoPtr->GetAbsMatrix().invert(&invertMatrix)) {
        captureMatrix_.preConcat(invertMatrix);
    }
#else
    captureMatrix_.Set(Drawing::Matrix::Index::SCALE_X, scaleX_);
    captureMatrix_.Set(Drawing::Matrix::Index::SCALE_X, scaleY_);
    Drawing::Matrix invertMatrix;
    if (targetNodeGeoPtr->GetAbsMatrix().Invert(invertMatrix)) {
        captureMatrix_.PreConcat(invertMatrix);
    }
#endif
}

#ifndef USE_ROSEN_DRAWING
void RSUniUICapture::PostTaskToRSRecord(std::shared_ptr<RSRecordingCanvas> canvas,
#else
void RSUniUICapture::PostTaskToRSRecord(std::shared_ptr<ExtendRecordingCanvas> canvas,
#endif
    std::shared_ptr<RSRenderNode> node, std::shared_ptr<RSUniUICaptureVisitor> visitor)
{
    std::function<void()> recordingDrawCall = [canvas, node, visitor]() -> void {
        visitor->SetCanvas(canvas);
        if (!node->IsOnTheTree()) {
            node->ApplyModifiers();
            node->Prepare(visitor);
        }
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
#ifndef USE_ROSEN_DRAWING
    canvas_->scale(scaleX_, scaleY_);
#else
    canvas_->Scale(scaleX_, scaleY_);
#endif
    canvas_->SetDisableFilterCache(true);
}

#ifndef USE_ROSEN_DRAWING
void RSUniUICapture::RSUniUICaptureVisitor::SetCanvas(std::shared_ptr<RSRecordingCanvas> canvas)
{
    if (canvas == nullptr) {
        RS_LOGE("RSUniUICaptureVisitor::SetCanvas: canvas == nullptr");
        return;
    }
    canvas_ = std::make_shared<RSPaintFilterCanvas>(canvas.get());
    canvas_->scale(scaleX_, scaleY_);
    canvas_->SetDisableFilterCache(true);
    canvas_->SetRecordingState(true);
}
#else
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
}
#endif

void RSUniUICapture::RSUniUICaptureVisitor::ProcessChildren(RSRenderNode& node)
{
    auto sortedChildren = node.GetSortedChildren();
    if (RSSystemProperties::GetUseShadowBatchingEnabled()
        && (node.GetRenderProperties().GetUseShadowBatching())) {
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

#ifndef USE_ROSEN_DRAWING
    canvas_->save();
    ProcessCanvasRenderNode(node);
    canvas_->restore();
#else
    canvas_->Save();
    ProcessCanvasRenderNode(node);
    canvas_->Restore();
#endif
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
#ifndef USE_ROSEN_DRAWING
        SkMatrix relativeMatrix = SkMatrix::I();
        relativeMatrix.setScaleX(scaleX_);
        relativeMatrix.setScaleY(scaleY_);
        SkMatrix invertMatrix;
        if (geoPtr->GetMatrix().invert(&invertMatrix)) {
            relativeMatrix.preConcat(invertMatrix);
        }
        canvas_->setMatrix(relativeMatrix);
#else
        Drawing::Matrix relativeMatrix = Drawing::Matrix();
        relativeMatrix.Set(Drawing::Matrix::Index::SCALE_X, scaleX_);
        relativeMatrix.Set(Drawing::Matrix::Index::SCALE_Y, scaleY_);
        Drawing::Matrix invertMatrix;
        if (geoPtr->GetMatrix().Invert(invertMatrix)) {
            relativeMatrix.PreConcat(invertMatrix);
        }
        canvas_->SetMatrix(relativeMatrix);
#endif
    }
    node.ProcessRenderBeforeChildren(*canvas_);
    if (node.GetType() == RSRenderNodeType::CANVAS_DRAWING_NODE) {
        auto canvasDrawingNode = node.ReinterpretCastTo<RSCanvasDrawingRenderNode>();
        if (!canvasDrawingNode->IsOnTheTree()) {
#ifndef USE_ROSEN_DRAWING
            auto clearFunc = [id = UNI_MAIN_THREAD_INDEX](sk_sp<SkSurface> surface) {
                // The second param is null, 0 is an invalid value.
                RSUniRenderUtil::ClearNodeCacheSurface(std::move(surface), nullptr, id, 0);
            };
#else
            auto clearFunc = [id = UNI_MAIN_THREAD_INDEX](std::shared_ptr<Drawing::Surface> surface) {
                // The second param is null, 0 is an invalid value.
                RSUniRenderUtil::ClearNodeCacheSurface(std::move(surface), nullptr, id, 0);
            };
#endif
            canvasDrawingNode->SetSurfaceClearFunc({ UNI_MAIN_THREAD_INDEX, clearFunc });
            canvasDrawingNode->ProcessRenderContents(*canvas_);
        } else {
#ifndef USE_ROSEN_DRAWING
            SkBitmap bitmap = canvasDrawingNode->GetBitmap();
#ifndef NEW_SKIA
            canvas_->drawBitmap(bitmap, 0, 0);
#else
            canvas_->drawImage(bitmap.asImage(), 0, 0);
#endif
#else
            Drawing::Bitmap bitmap = canvasDrawingNode->GetBitmap();
            canvas_->DrawBitmap(bitmap, 0, 0);
#endif
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
#ifndef USE_ROSEN_DRAWING
    RSAutoCanvasRestore acr(canvas_);
#else
    Drawing::AutoCanvasRestore acr(*canvas_, true);
#endif
    canvas_->MultiplyAlpha(node.GetRenderProperties().GetAlpha());
    ProcessSurfaceViewWithUni(node);
}

void RSUniUICapture::RSUniUICaptureVisitor::ProcessSurfaceViewWithUni(RSSurfaceRenderNode& node)
{
#ifndef USE_ROSEN_DRAWING
    canvas_->save();
#else
    canvas_->Save();
#endif

    const auto& property = node.GetRenderProperties();
    auto geoPtr = (property.GetBoundsGeometry());
    if (!geoPtr) {
        RS_LOGE("RSUniUICaptureVisitor::ProcessSurfaceViewWithUni node:%{public}" PRIu64 ", get geoPtr failed",
            node.GetId());
        return;
    }
#ifndef USE_ROSEN_DRAWING
    canvas_->setMatrix(captureMatrix_);
    canvas_->concat(geoPtr->GetAbsMatrix());
#else
    canvas_->SetMatrix(captureMatrix_);
    canvas_->ConcatMatrix(geoPtr->GetAbsMatrix());
#endif

    bool isSelfDrawingSurface = node.GetSurfaceNodeType() == RSSurfaceNodeType::SELF_DRAWING_NODE;
    const RectF absBounds = { 0, 0, property.GetBoundsWidth(), property.GetBoundsHeight() };
    RRect absClipRRect = RRect(absBounds, property.GetCornerRadius());
    if (isSelfDrawingSurface) {
        RSPropertiesPainter::DrawShadow(property, *canvas_, &absClipRRect);
        RSPropertiesPainter::DrawOutline(property, *canvas_);
    }
#ifndef USE_ROSEN_DRAWING
    canvas_->save();
    if (isSelfDrawingSurface && !property.GetCornerRadius().IsZero()) {
        canvas_->clipRRect(RSPropertiesPainter::RRect2SkRRect(absClipRRect), true);
    } else {
        canvas_->clipRect(SkRect::MakeWH(property.GetBoundsWidth(), property.GetBoundsHeight()));
    }
#else
    canvas_->Save();
    if (isSelfDrawingSurface && !property.GetCornerRadius().IsZero()) {
        canvas_->ClipRoundRect(RSPropertiesPainter::RRect2DrawingRRect(absClipRRect),
            Drawing::ClipOp::INTERSECT, true);
    } else {
        canvas_->ClipRect(Drawing::Rect(0, 0, property.GetBoundsWidth(), property.GetBoundsHeight()),
            Drawing::ClipOp::INTERSECT, false);
    }
#endif
    if (isSelfDrawingSurface) {
        RSPropertiesPainter::DrawBackground(property, *canvas_);
        RSPropertiesPainter::DrawMask(property, *canvas_);
        RSPropertiesPainter::DrawFilter(property, *canvas_, FilterType::BACKGROUND_FILTER);
    } else {
#ifndef USE_ROSEN_DRAWING
        auto backgroundColor = static_cast<SkColor>(property.GetBackgroundColor().AsArgbInt());
        if (SkColorGetA(backgroundColor) != SK_AlphaTRANSPARENT) {
            canvas_->drawColor(backgroundColor);
        }
    }
    canvas_->restore();
#else
        auto backgroundColor = static_cast<Drawing::ColorQuad>(property.GetBackgroundColor().AsArgbInt());
        if (Drawing::Color::ColorQuadGetA(backgroundColor) != Drawing::Color::COLOR_TRANSPARENT) {
            canvas_->DrawColor(backgroundColor);
        }
    }
    canvas_->Restore();
#endif
    if (node.GetBuffer() != nullptr) {
#ifndef USE_ROSEN_DRAWING
        if (auto recordingCanvas = static_cast<RSRecordingCanvas*>(canvas_->GetRecordingCanvas())) {
            auto params = RSUniRenderUtil::CreateBufferDrawParam(node, false);
            auto buffer = node.GetBuffer();
            RSSurfaceBufferInfo rsSurfaceBufferInfo(buffer, params.dstRect.left(), params.dstRect.top(),
                params.dstRect.width(), params.dstRect.height());
#else
        if (auto recordingCanvas = static_cast<ExtendRecordingCanvas*>(canvas_->GetRecordingCanvas())) {
            auto params = RSUniRenderUtil::CreateBufferDrawParam(node, false);
            auto buffer = node.GetBuffer();
            DrawingSurfaceBufferInfo rsSurfaceBufferInfo(buffer, params.dstRect.GetLeft(), params.dstRect.GetTop(),
                params.dstRect.GetWidth(), params.dstRect.GetHeight());
            recordingCanvas->ConcatMatrix(params.matrix);
#endif //USE_ROSEN_DRAWING
            recordingCanvas->DrawSurfaceBuffer(rsSurfaceBufferInfo);
        } else {
            auto params = RSUniRenderUtil::CreateBufferDrawParam(node, false);
            renderEngine_->DrawSurfaceNodeWithParams(*canvas_, node, params);
        }
    }
    if (isSelfDrawingSurface) {
        RSPropertiesPainter::DrawFilter(property, *canvas_, FilterType::FOREGROUND_FILTER);
    }
#ifndef USE_ROSEN_DRAWING
    canvas_->restore();
#else
    canvas_->Restore();
#endif
    ProcessChildren(node);
}

void RSUniUICapture::RSUniUICaptureVisitor::ProcessSurfaceViewWithoutUni(RSSurfaceRenderNode& node)
{
#ifndef USE_ROSEN_DRAWING
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
        ProcessChildren(node);
        canvas_->restoreToCount(saveCnt);
        if (node.GetBuffer() != nullptr) {
            // in node's local coordinate.
            auto params = RSDividedRenderUtil::CreateBufferDrawParam(node, true, false, true, false);
            renderEngine_->DrawSurfaceNodeWithParams(*canvas_, node, params);
        }
    } else {
        canvas_->save();
        if (node.GetId() != nodeId_) {
            canvas_->concat(translateMatrix);
        }
        if (node.GetBuffer() != nullptr) {
            // in node's local coordinate.
            auto params = RSDividedRenderUtil::CreateBufferDrawParam(node, true, false, true, false);
            renderEngine_->DrawSurfaceNodeWithParams(*canvas_, node, params);
        }
        canvas_->restore();
    }
#else
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
            auto params = RSDividedRenderUtil::CreateBufferDrawParam(node, true, false, false, false);
            renderEngine_->DrawSurfaceNodeWithParams(*canvas_, node, params);
        }
    } else {
        canvas_->Save();
        if (node.GetId() != nodeId_) {
            canvas_->ConcatMatrix(translateMatrix);
        }
        if (node.GetBuffer() != nullptr) {
            // in node's local coordinate.
            auto params = RSDividedRenderUtil::CreateBufferDrawParam(node, true, false, false, false);
            renderEngine_->DrawSurfaceNodeWithParams(*canvas_, node, params);
        }
        canvas_->Restore();
    }
#endif
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
