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

#include "pipeline/rs_divided_ui_capture.h"

#include <memory>
#include <mutex>
#ifdef ROSEN_OHOS
#include <sys/mman.h>
#endif

#ifndef USE_ROSEN_DRAWING
#include "include/core/SkMatrix.h"
#else
#include "utils/matrix.h"
#endif

#include "common/rs_common_def.h"
#include "common/rs_obj_abs_geometry.h"
#include "offscreen_render/rs_offscreen_render_thread.h"
#include "pipeline/rs_canvas_drawing_render_node.h"
#include "pipeline/rs_dirty_region_manager.h"
#include "pipeline/rs_render_node_map.h"
#include "pipeline/rs_render_thread.h"
#include "pipeline/rs_root_render_node.h"
#include "render/rs_pixel_map_util.h"
#include "transaction/rs_render_service_client.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"

namespace OHOS {
namespace Rosen {

const int FAKE_WIDTH = 10; // When the width and height of the node are not set, use the fake width
const int FAKE_HEIGHT = 10; // When the width and height of the node are not set, use the fake height

std::shared_ptr<Media::PixelMap> RSDividedUICapture::TakeLocalCapture()
{
    if (ROSEN_EQ(scaleX_, 0.f) || ROSEN_EQ(scaleY_, 0.f) || scaleX_ < 0.f || scaleY_ < 0.f) {
        ROSEN_LOGE("RSDividedUICapture::TakeLocalCapture: scale is invalid.");
        return nullptr;
    }
    auto node = RSRenderThread::Instance().GetContext().GetNodeMap().GetRenderNode<RSRenderNode>(nodeId_);
    if (node == nullptr) {
        ROSEN_LOGE("RSDividedUICapture::TakeLocalCapture node is nullptr return");
        return nullptr;
    }
    std::shared_ptr<RSDividedUICaptureVisitor> visitor =
        std::make_shared<RSDividedUICaptureVisitor>(nodeId_, scaleX_, scaleY_);
#ifndef USE_ROSEN_DRAWING
    auto recordingCanvas = std::make_shared<RSRecordingCanvas>(FAKE_WIDTH, FAKE_HEIGHT);
#else
    auto recordingCanvas = std::make_shared<ExtendRecordingCanvas>(FAKE_WIDTH, FAKE_HEIGHT);
#endif
    PostTaskToRTRecord(recordingCanvas, node, visitor);
    auto drawCallList = recordingCanvas->GetDrawCmdList();
    std::shared_ptr<Media::PixelMap> pixelmap = CreatePixelMapByNode(node);
    if (pixelmap == nullptr) {
        ROSEN_LOGE("RSDividedUICapture::TakeLocalCapture: pixelmap == nullptr!");
        return nullptr;
    }
#ifndef USE_ROSEN_DRAWING
    auto skSurface = CreateSurface(pixelmap);
    if (skSurface == nullptr) {
        RS_LOGE("RSDividedUICapture::TakeLocalCapture: skSurface == nullptr!");
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
    drawCallList->Playback(*canvas);
    return pixelmap;
}

std::shared_ptr<Media::PixelMap> RSDividedUICapture::CreatePixelMapByNode(std::shared_ptr<RSRenderNode> node) const
{
    int pixmapWidth = node->GetRenderProperties().GetBoundsWidth();
    int pixmapHeight = node->GetRenderProperties().GetBoundsHeight();
    Media::InitializationOptions opts;
    opts.size.width = ceil(pixmapWidth * scaleX_);
    opts.size.height = ceil(pixmapHeight * scaleY_);
    return Media::PixelMap::Create(opts);
}

#ifndef USE_ROSEN_DRAWING
sk_sp<SkSurface> RSDividedUICapture::CreateSurface(const std::shared_ptr<Media::PixelMap>& pixelmap) const
#else
std::shared_ptr<Drawing::Surface> RSDividedUICapture::CreateSurface(
    const std::shared_ptr<Media::PixelMap>& pixelmap) const
#endif
{
    if (pixelmap == nullptr) {
        ROSEN_LOGE("RSDividedUICapture::CreateSurface: pixelmap == nullptr");
        return nullptr;
    }
    auto address = const_cast<uint32_t*>(pixelmap->GetPixel32(0, 0));
    if (address == nullptr) {
        ROSEN_LOGE("RSDividedUICapture::CreateSurface: address == nullptr");
        return nullptr;
    }
#ifndef USE_ROSEN_DRAWING
    SkImageInfo info = SkImageInfo::Make(pixelmap->GetWidth(), pixelmap->GetHeight(),
        kRGBA_8888_SkColorType, kPremul_SkAlphaType);
    return SkSurface::MakeRasterDirect(info, address, pixelmap->GetRowBytes());
#else
    Drawing::BitmapFormat format = { Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_PREMUL };
    Drawing::Bitmap bitmap;
    bitmap.Build(pixelmap->GetWidth(), pixelmap->GetHeight(), format);
    bitmap.SetPixels(address);

    auto surface = std::make_shared<Drawing::Surface>();
    surface->Bind(bitmap);
    return surface;
#endif
}

#ifndef USE_ROSEN_DRAWING
void RSDividedUICapture::RSDividedUICaptureVisitor::SetCanvas(std::shared_ptr<RSRecordingCanvas> canvas)
#else
void RSDividedUICapture::RSDividedUICaptureVisitor::SetCanvas(std::shared_ptr<ExtendRecordingCanvas> canvas)
#endif
{
    if (canvas == nullptr) {
        ROSEN_LOGE("RSDividedUICaptureVisitor::SetCanvas: canvas == nullptr");
        return;
    }
#ifndef USE_ROSEN_DRAWING
    if (auto renderContext = RSRenderThread::Instance().GetRenderContext()) {
        canvas->SetGrRecordingContext(renderContext->GetGrContext());
    }
    canvas_ = std::make_shared<RSPaintFilterCanvas>(canvas.get());
    canvas_->scale(scaleX_, scaleY_);
    canvas_->SetRecordingState(true);
#else
    canvas_ = std::make_shared<RSPaintFilterCanvas>(canvas.get());
    canvas_->Scale(scaleX_, scaleY_);
    canvas_->SetRecordingState(true);
#endif
}

void RSDividedUICapture::RSDividedUICaptureVisitor::SetPaintFilterCanvas(std::shared_ptr<RSPaintFilterCanvas> canvas)
{
    if (canvas == nullptr) {
        ROSEN_LOGE("RSDividedUICaptureVisitor::SetCanvas: canvas == nullptr");
        return;
    }
    canvas_ = canvas;
#ifndef USE_ROSEN_DRAWING
    canvas_->scale(scaleX_, scaleY_);
#else
    canvas_->Scale(scaleX_, scaleY_);
#endif // USE_ROSEN_DRAWING
}

#ifndef USE_ROSEN_DRAWING
void RSDividedUICapture::PostTaskToRTRecord(std::shared_ptr<RSRecordingCanvas> canvas,
    std::shared_ptr<RSRenderNode> node, std::shared_ptr<RSDividedUICaptureVisitor> visitor)
#else
void RSDividedUICapture::PostTaskToRTRecord(std::shared_ptr<ExtendRecordingCanvas> canvas,
    std::shared_ptr<RSRenderNode> node, std::shared_ptr<RSDividedUICaptureVisitor> visitor)
#endif
{
    std::function<void()> recordingDrawCall = [canvas, node, visitor]() -> void {
        visitor->SetCanvas(canvas);
        if (!node->IsOnTheTree()) {
            node->ApplyModifiers();
            node->Prepare(visitor);
        }
        node->Process(visitor);
    };
    RSRenderThread::Instance().PostSyncTask(recordingDrawCall);
}

void RSDividedUICapture::RSDividedUICaptureVisitor::ProcessChildren(RSRenderNode& node)
{
    for (auto& child : *node.GetSortedChildren()) {
        child->Process(shared_from_this());
    }
}

void RSDividedUICapture::RSDividedUICaptureVisitor::ProcessRootRenderNode(RSRootRenderNode& node)
{
    if (!node.ShouldPaint()) {
        ROSEN_LOGD("RSDividedUICaptureVisitor::ProcessRootRenderNode, no need process");
        return;
    }

    if (!canvas_) {
        ROSEN_LOGE("RSDividedUICaptureVisitor::ProcessRootRenderNode, canvas is nullptr");
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

void RSDividedUICapture::RSDividedUICaptureVisitor::ProcessCanvasRenderNode(RSCanvasRenderNode& node)
{
    if (!node.ShouldPaint()) {
        ROSEN_LOGD("RSDividedUICaptureVisitor::ProcessCanvasRenderNode, no need process");
        return;
    }
    if (!canvas_) {
        ROSEN_LOGE("RSDividedUICaptureVisitor::ProcessCanvasRenderNode, canvas is nullptr");
        return;
    }
    if (node.GetId() == nodeId_) {
        // When drawing nodes, canvas will offset the bounds value, so we will move in reverse here first
        const auto& property = node.GetRenderProperties();
        auto geoPtr = (property.GetBoundsGeometry());
#ifndef USE_ROSEN_DRAWING
        SkMatrix relativeMatrix = SkMatrix::I();
        relativeMatrix.setScaleY(scaleX_);
        relativeMatrix.setScaleY(scaleY_);
        SkMatrix invertMatrix;
        if (geoPtr->GetMatrix().invert(&invertMatrix)) {
            relativeMatrix.preConcat(invertMatrix);
        }
        canvas_->setMatrix(relativeMatrix);
#else
        Drawing::Matrix relativeMatrix;
        relativeMatrix.Set(Drawing::Matrix::SCALE_X, scaleX_);
        relativeMatrix.Set(Drawing::Matrix::SCALE_Y, scaleY_);
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

void RSDividedUICapture::RSDividedUICaptureVisitor::ProcessEffectRenderNode(RSEffectRenderNode& node)
{
    if (!node.ShouldPaint()) {
        RS_LOGD("RSDividedUICapture::RSDividedUICaptureVisitor, no need process");
        return;
    }
    if (!canvas_) {
        RS_LOGE("RSDividedUICapture::RSDividedUICaptureVisitor, canvas is nullptr");
        return;
    }
    node.ProcessRenderBeforeChildren(*canvas_);
    ProcessChildren(node);
    node.ProcessRenderAfterChildren(*canvas_);
}

class RSOffscreenRenderCallback : public SurfaceCaptureCallback {
public:
    void OnSurfaceCapture(std::shared_ptr<Media::PixelMap> pixelmap) override
    {
        std::unique_lock<std::mutex> lock(mutex_);
        if (!flag_) {
            pixelMap_ = pixelmap;
            flag_ = true;
        }
        conditionVariable_.notify_one();
    }
    bool IsReady() const
    {
        return flag_;
    }
    std::shared_ptr<Media::PixelMap> GetResult(long timeOut)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        if (!conditionVariable_.wait_for(lock, std::chrono::milliseconds(timeOut), [this] { return IsReady(); })) {
            ROSEN_LOGE("wait for %{public}lu timeout", timeOut);
        }
        return pixelMap_;
    }
private:
    std::shared_ptr<Media::PixelMap> pixelMap_ = nullptr;
    std::mutex mutex_;
    std::condition_variable conditionVariable_;
    bool flag_ = false;
};

void RSDividedUICapture::RSDividedUICaptureVisitor::ProcessSurfaceRenderNode(RSSurfaceRenderNode& node)
{
    if (!canvas_) {
        ROSEN_LOGE("RSDividedUICaptureVisitor::ProcessSurfaceRenderNode, canvas is nullptr");
        return;
    }
    if (!node.GetRenderProperties().GetVisible()) {
        ROSEN_LOGI(
            "RSDividedUICaptureVisitor::ProcessSurfaceRenderNode node : %{public}" PRIu64 " is invisible",
            node.GetId());
        return;
    }
    std::shared_ptr<RSOffscreenRenderCallback> callback = std::make_shared<RSOffscreenRenderCallback>();
    auto renderServiceClient = std::make_unique<RSRenderServiceClient>();
    renderServiceClient->TakeSurfaceCapture(node.GetId(), callback, scaleX_, scaleY_, SurfaceCaptureType::UICAPTURE);
    std::shared_ptr<Media::PixelMap> pixelMap = callback->GetResult(2000);
    if (pixelMap == nullptr) {
        ROSEN_LOGE("RSDividedUICaptureVisitor::TakeLocalCapture failed to get pixelmap, return nullptr!");
        return;
    }
    // draw pixelmap in canvas
#ifndef USE_ROSEN_DRAWING
    auto image = RSPixelMapUtil::ExtractSkImage(pixelMap);
    canvas_->drawImage(
        image, node.GetRenderProperties().GetBoundsPositionX(), node.GetRenderProperties().GetBoundsPositionY());
#else
    auto image = RSPixelMapUtil::ExtractDrawingImage(pixelMap);
    canvas_->DrawImage(*image, node.GetRenderProperties().GetBoundsPositionX(),
        node.GetRenderProperties().GetBoundsPositionY(), Drawing::SamplingOptions());
#endif
    ProcessChildren(node);
}

void RSDividedUICapture::RSDividedUICaptureVisitor::PrepareChildren(RSRenderNode& node)
{
    for (auto& child : *node.GetSortedChildren()) {
        child->Prepare(shared_from_this());
    }
}

void RSDividedUICapture::RSDividedUICaptureVisitor::PrepareCanvasRenderNode(RSCanvasRenderNode& node)
{
    auto dirtyManager = std::make_shared<RSDirtyRegionManager>();
    node.Update(*dirtyManager, nullptr, false);
    PrepareChildren(node);
}

void RSDividedUICapture::RSDividedUICaptureVisitor::PrepareSurfaceRenderNode(RSSurfaceRenderNode& node)
{
    auto dirtyManager = std::make_shared<RSDirtyRegionManager>();
    node.Update(*dirtyManager, nullptr, false);
    PrepareChildren(node);
}

void RSDividedUICapture::RSDividedUICaptureVisitor::PrepareRootRenderNode(RSRootRenderNode& node)
{
    PrepareCanvasRenderNode(node);
}

void RSDividedUICapture::RSDividedUICaptureVisitor::PrepareEffectRenderNode(RSEffectRenderNode& node)
{
    auto dirtyManager = std::make_shared<RSDirtyRegionManager>();
    node.Update(*dirtyManager, nullptr, false);
    PrepareChildren(node);
}
} // namespace Rosen
} // namespace OHOS
