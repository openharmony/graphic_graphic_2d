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

#include "rs_divided_ui_capture.h"

#include <memory>
#include <mutex>
#ifdef ROSEN_OHOS
#include <sys/mman.h>
#endif

#include "utils/matrix.h"

#include "common/rs_common_def.h"
#include "common/rs_obj_abs_geometry.h"
#include "draw/color.h"
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
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {

const int MAX_WAIT_TIME = 2000;

std::shared_ptr<Media::PixelMap> RSDividedUICapture::TakeLocalCapture()
{
    RS_LOGI("RSDividedUICapture::TakeLocalCapture nodeId is %{public}" PRIu64, nodeId_);
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
    if (!node->IsOnTheTree()) {
        ROSEN_LOGD("RSDividedUICapture::TakeLocalCapture IsNotOnTheTree, Do ApplyModifiers");
        node->ApplyModifiers();
    }
    std::shared_ptr<Media::PixelMap> pixelmap = CreatePixelMapByNode(node);
    if (pixelmap == nullptr) {
        ROSEN_LOGE("RSDividedUICapture::TakeLocalCapture: pixelmap == nullptr!");
        return nullptr;
    }
    auto drSurface = CreateSurface(pixelmap);
    if (drSurface == nullptr) {
        return nullptr;
    }
    auto canvas = std::make_shared<RSPaintFilterCanvas>(drSurface.get());
    visitor->SetPaintFilterCanvas(canvas);
    if (!node->IsOnTheTree()) {
        ROSEN_LOGD("RSDividedUICapture::TakeLocalCapture IsNotOnTheTree, Do Prepare");
        node->Prepare(visitor);
    }
    node->Process(visitor);
#if (defined (RS_ENABLE_GL) || defined (RS_ENABLE_VK)) && (defined RS_ENABLE_EGLIMAGE)
    std::shared_ptr<Drawing::Image> img(drSurface.get()->GetImageSnapshot());
    if (!img) {
        RS_LOGE("RSDividedUICapture::Run: img is nullptr");
        return nullptr;
    }
    if (!CopyDataToPixelMap(img, pixelmap)) {
        RS_LOGE("RSDividedUICapture::Run: CopyDataToPixelMap failed");
        return nullptr;
    }
#endif
    return pixelmap;
}

std::shared_ptr<Media::PixelMap> RSDividedUICapture::CreatePixelMapByNode(std::shared_ptr<RSRenderNode> node) const
{
    int pixmapWidth = node->GetRenderProperties().GetBoundsWidth();
    int pixmapHeight = node->GetRenderProperties().GetBoundsHeight();
    Media::InitializationOptions opts;
    opts.size.width = ceil(pixmapWidth * scaleX_);
    opts.size.height = ceil(pixmapHeight * scaleY_);
    RS_LOGD("RSDividedUICapture::CreatePixelMapByNode: NodeId:[%{public}" PRIu64 "],"
        " origin pixelmap width is [%{public}u], height is [%{public}u],"
        " created pixelmap width is [%{public}u], height is [%{public}u],"
        " the scale is scaleX:[%{public}f], scaleY:[%{public}f]",
        node->GetId(), pixmapWidth, pixmapHeight, opts.size.width, opts.size.height,
        scaleX_, scaleY_);
    return Media::PixelMap::Create(opts);
}

bool CopyDataToPixelMap(std::shared_ptr<Drawing::Image> img, const std::shared_ptr<Media::PixelMap>& pixelmap)
{
    if (!img || !pixelmap) {
        RS_LOGE("RSDividedUICapture::CopyDataToPixelMap failed, img or pixelmap is nullptr");
        return false;
    }
    auto size = pixelmap->GetRowBytes() * pixelmap->GetHeight();
    auto colorType = (pixelmap->GetPixelFormat() == Media::PixelFormat::RGBA_F16) ?
        Drawing::ColorType::COLORTYPE_RGBA_F16 : Drawing::ColorType::COLORTYPE_RGBA_8888;
#ifdef ROSEN_OHOS
    int fd = AshmemCreate("RSDividedUICapture Data", size);
    if (fd < 0) {
        RS_LOGE("RSDividedUICapture::CopyDataToPixelMap AshmemCreate fd < 0");
        return false;
    }
    int result = AshmemSetProt(fd, PROT_READ | PROT_WRITE);
    if (result < 0) {
        RS_LOGE("RSDividedUICapture::CopyDataToPixelMap AshmemSetProt error");
        ::close(fd);
        return false;
    }
    void* ptr = ::mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    auto data = static_cast<uint8_t*>(ptr);
    if (ptr == MAP_FAILED || ptr == nullptr) {
        RS_LOGE("RSDividedUICapture::CopyDataToPixelMap data is nullptr");
        ::close(fd);
        return false;
    }

    Drawing::BitmapFormat format { colorType, Drawing::AlphaType::ALPHATYPE_PREMUL };
    Drawing::Bitmap bitmap;
    bitmap.Build(pixelmap->GetWidth(), pixelmap->GetHeight(), format, 0);
    bitmap.SetPixels(data);
    if (!img->ReadPixels(bitmap, 0, 0)) {
        RS_LOGE("RSDividedUICapture::CopyDataToPixelMap readPixels failed");
        ::close(fd);
        return false;
    }
    void* fdPtr = new int32_t();
    *static_cast<int32_t*>(fdPtr) = fd;
    pixelmap->SetPixelsAddr(data, fdPtr, size, Media::AllocatorType::SHARE_MEM_ALLOC, nullptr);
#else
    auto data = (uint8_t *)malloc(size);
    if (data == nullptr) {
        RS_LOGE("RSDividedUICapture::CopyDataToPixelMap data is nullptr");
        return false;
    }

    Drawing::BitmapFormat format { colorType, Drawing::AlphaType::ALPHATYPE_PREMUL };
    Drawing::Bitmap bitmap;
    bitmap.Build(pixelmap->GetWidth(), pixelmap->GetHeight(), format, 0);
    bitmap.SetPixels(data);
    if (!img->ReadPixels(bitmap, 0, 0)) {
        RS_LOGE("RSDividedUICapture::CopyDataToPixelMap readPixels failed");
        free(data);
        data = nullptr;
        return false;
    }

    pixelmap->SetPixelsAddr(data, nullptr, size, Media::AllocatorType::HEAP_ALLOC, nullptr);
#endif
    return true;
}

std::shared_ptr<Drawing::Surface> RSDividedUICapture::CreateSurface(
    const std::shared_ptr<Media::PixelMap>& pixelmap) const
{
    if (pixelmap == nullptr) {
        RS_LOGE("RSDividedUICapture::CreateSurface: pixelmap == nullptr");
        return nullptr;
    }
    auto address = const_cast<uint32_t*>(pixelmap->GetPixel32(0, 0));
    if (address == nullptr) {
        RS_LOGE("RSDividedUICapture::CreateSurface: address == nullptr");
        return nullptr;
    }
    Drawing::ImageInfo info = Drawing::ImageInfo{pixelmap->GetWidth(), pixelmap->GetHeight(),
        Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_PREMUL};

#if (defined RS_ENABLE_GL) && (defined RS_ENABLE_EGLIMAGE)
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::OPENGL) {
        auto renderContext = RSRenderThread::Instance().GetRenderContext();
        if (renderContext == nullptr) {
            RS_LOGE("RSDividedUICapture::CreateSurface: renderContext is nullptr");
            return nullptr;
        }
        renderContext->SetUpGpuContext(nullptr);
        return Drawing::Surface::MakeRenderTarget(renderContext->GetDrGPUContext(), false, info);
    }
#endif
#ifdef RS_ENABLE_VK
    if (RSSystemProperties::IsUseVulkan()) {
        return Drawing::Surface::MakeRenderTarget(
            RSRenderThread::Instance().GetRenderContext()->GetDrGPUContext(), false, info);
    }
#endif
    return Drawing::Surface::MakeRasterDirect(info, address, pixelmap->GetRowBytes());
}

void RSDividedUICapture::RSDividedUICaptureVisitor::SetCanvas(std::shared_ptr<ExtendRecordingCanvas> canvas)
{
    if (canvas == nullptr) {
        ROSEN_LOGE("RSDividedUICaptureVisitor::SetCanvas: canvas == nullptr");
        return;
    }
    canvas_ = std::make_shared<RSPaintFilterCanvas>(canvas.get());
    canvas_->Scale(scaleX_, scaleY_);
    canvas_->SetDisableFilterCache(true);
    canvas_->SetRecordingState(true);
    canvas_->SetCacheType(Drawing::CacheType::DISABLED);
}

void RSDividedUICapture::RSDividedUICaptureVisitor::SetPaintFilterCanvas(std::shared_ptr<RSPaintFilterCanvas> canvas)
{
    if (canvas == nullptr) {
        ROSEN_LOGE("RSDividedUICaptureVisitor::SetCanvas: canvas == nullptr");
        return;
    }
    canvas_ = canvas;
    canvas_->Scale(scaleX_, scaleY_);
    canvas_->SetDisableFilterCache(true);
}

void RSDividedUICapture::PostTaskToRTRecord(std::shared_ptr<ExtendRecordingCanvas> canvas,
    std::shared_ptr<RSRenderNode> node, std::shared_ptr<RSDividedUICaptureVisitor> visitor)
{
    if (!node->IsOnTheTree()) {
        RS_TRACE_NAME_FMT("RSDividedUICapture::PostTaskToRTRecord id is %llu", node->GetId());
        node->ApplyModifiers();
        node->Prepare(visitor);
    }
    node->Process(visitor);
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

    canvas_->Save();
    ProcessCanvasRenderNode(node);
    canvas_->Restore();
}

void RSDividedUICapture::RSDividedUICaptureVisitor::ProcessCanvasRenderNode(RSCanvasRenderNode& node)
{
    RS_TRACE_NAME_FMT("RSDividedUICaptureVisitor::ProcessCanvasRenderNode nodeId is %llu properties is %s",
        node.GetId(), node.GetRenderProperties().Dump().c_str());
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
        auto& geoPtr = (property.GetBoundsGeometry());
        Drawing::Matrix relativeMatrix;
        relativeMatrix.Set(Drawing::Matrix::SCALE_X, scaleX_);
        relativeMatrix.Set(Drawing::Matrix::SCALE_Y, scaleY_);
        Drawing::Matrix invertMatrix;
        if (geoPtr && geoPtr->GetMatrix().Invert(invertMatrix)) {
            relativeMatrix.PreConcat(invertMatrix);
        }
        canvas_->SetMatrix(relativeMatrix);
    }
    RSAutoCanvasRestore acr(canvas_, RSPaintFilterCanvas::SaveType::kAll);
    node.ApplyAlphaAndBoundsGeometry(*canvas_);
    node.ProcessRenderBeforeChildren(*canvas_);
    if (node.GetType() == RSRenderNodeType::CANVAS_DRAWING_NODE) {
        auto canvasDrawingNode = node.ReinterpretCastTo<RSCanvasDrawingRenderNode>();
        if (!canvasDrawingNode->IsOnTheTree()) {
            canvasDrawingNode->ProcessRenderContents(*canvas_);
        } else {
            Drawing::Bitmap bitmap = canvasDrawingNode->GetBitmap();
            canvas_->DrawBitmap(bitmap, 0, 0);
        }
    } else {
        node.ProcessRenderContents(*canvas_);
    }
    ProcessChildren(node);
    node.ProcessRenderAfterChildren(*canvas_);
}

void RSDividedUICapture::RSDividedUICaptureVisitor::ProcessEffectRenderNode(RSEffectRenderNode& node)
{
    RS_TRACE_NAME_FMT("RSDividedUICaptureVisitor::ProcessEffectRenderNode nodeId is %llu", node.GetId());
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
    void OnSurfaceCaptureHDR(std::shared_ptr<Media::PixelMap> pixelmap,
        std::shared_ptr<Media::PixelMap> pixelmapHDR) override
    {
        std::unique_lock<std::mutex> lock(mutex_);
        if (!flag_) {
            pixelMap_ = pixelmap;
            pixelMapHDR_ = pixelmapHDR;
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
            ROSEN_LOGE("wait for %{public}ld timeout", timeOut);
        }
        return pixelMap_;
    }

    std::vector<std::shared_ptr<Media::PixelMap>> GetHDRResult(long timeOut)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        if (!conditionVariable_.wait_for(lock, std::chrono::milliseconds(timeOut), [this] { return IsReady(); })) {
            ROSEN_LOGE("wait for %{public}ld timeout", timeOut);
        }
        return { pixelMap_, pixelMapHDR_ };
    }
private:
    std::shared_ptr<Media::PixelMap> pixelMap_ = nullptr;
    std::shared_ptr<Media::PixelMap> pixelMapHDR_ = nullptr;
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
    RS_LOGI("RSDividedUICaptureVisitor::ProcessSurfaceRenderNode nodeId is %{public}" PRIu64, node.GetId());
    std::shared_ptr<RSOffscreenRenderCallback> callback = std::make_shared<RSOffscreenRenderCallback>();
    auto renderServiceClient = std::make_unique<RSRenderServiceClient>();
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = scaleX_;
    captureConfig.scaleY = scaleY_;
    captureConfig.captureType = SurfaceCaptureType::UICAPTURE;
    renderServiceClient->TakeSurfaceCapture(node.GetId(), callback, captureConfig);
    std::shared_ptr<Media::PixelMap> pixelMap = callback->GetResult(MAX_WAIT_TIME);
    if (pixelMap == nullptr) {
        ROSEN_LOGE("RSDividedUICaptureVisitor::TakeLocalCapture failed to get pixelmap, return nullptr!");
        return;
    }
    // draw pixelmap in canvas
    auto image = RSPixelMapUtil::ExtractDrawingImage(pixelMap);
    if (image == nullptr) {
        ROSEN_LOGE("RSDividedUICaptureVisitor::ProcessSurfaceRenderNode, failed to extract drawing image from "
                   "pixelMap, image is nullptr");
        return;
    }
    canvas_->DrawImage(*image, node.GetRenderProperties().GetBoundsPositionX(),
        node.GetRenderProperties().GetBoundsPositionY(), Drawing::SamplingOptions());
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
