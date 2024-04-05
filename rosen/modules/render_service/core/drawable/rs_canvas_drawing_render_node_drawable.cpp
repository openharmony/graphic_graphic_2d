/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "drawable/rs_canvas_drawing_render_node_drawable.h"

#include "common/rs_background_thread.h"
#include "common/rs_common_def.h"
#include "pipeline/rs_task_dispatcher.h"
#include "pipeline/rs_uni_render_thread.h"
#include "pipeline/rs_uni_render_util.h"
#include "pipeline/sk_resource_manager.h"
#include "pipeline/parallel_render/rs_sub_thread_manager.h"
#include "platform/common/rs_log.h"

namespace OHOS::Rosen::DrawableV2 {
static std::mutex drawingMutex;
RSCanvasDrawingRenderNodeDrawable::Registrar RSCanvasDrawingRenderNodeDrawable::instance_;

RSCanvasDrawingRenderNodeDrawable::RSCanvasDrawingRenderNodeDrawable(std::shared_ptr<const RSRenderNode>&& node)
    : RSRenderNodeDrawable(std::move(node))
{
    auto nodeSp = std::const_pointer_cast<RSRenderNode>(renderNode_);
    auto canvasDrawingRenderNode = std::static_pointer_cast<RSCanvasDrawingRenderNode>(nodeSp);
    canvasDrawingRenderNode->InitRenderContent();
}

RSCanvasDrawingRenderNodeDrawable::~RSCanvasDrawingRenderNodeDrawable()
{
#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    if (curThreadInfo_.second && surface_) {
        curThreadInfo_.second(std::move(surface_));
    }
#endif
}

RSRenderNodeDrawable::Ptr RSCanvasDrawingRenderNodeDrawable::OnGenerate(std::shared_ptr<const RSRenderNode> node)
{
    return new RSCanvasDrawingRenderNodeDrawable(std::move(node));
}

void RSCanvasDrawingRenderNodeDrawable::OnDraw(Drawing::Canvas& canvas)
{
    auto& params = renderNode_->GetRenderParams();
    if (!params) {
        RS_LOGE("Failed to draw canvas drawing node, params is null!");
        return;
    }

    if (!params->GetShouldPaint()) {
        return;
    }
    auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(&canvas);
    RSAutoCanvasRestore acr(paintFilterCanvas, RSPaintFilterCanvas::SaveType::kCanvasAndAlpha);
    params->ApplyAlphaAndMatrixToCanvas(*paintFilterCanvas);

    auto uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams().get();
    if ((!uniParam || uniParam->IsOpDropped()) && paintFilterCanvas->GetDirtyFlag() &&
        !paintFilterCanvas->GetIsParallelCanvas() && QuickReject(canvas, params->GetLocalDrawRect())) {
        RS_LOGD("CanvasDrawingNode[%{public}" PRIu64 "] have no intersect with canvas's clipRegion", params->GetId());
        return;
    }

    auto threadIdx = paintFilterCanvas->GetParallelThreadIdx();
    auto clearFunc = [idx = threadIdx](std::shared_ptr<Drawing::Surface> surface) {
        // The second param is null, 0 is an invalid value.
        RSUniRenderUtil::ClearNodeCacheSurface(std::move(surface), nullptr, idx, 0);
    };
    auto threadId = paintFilterCanvas->GetIsParallelCanvas() ?
        RSSubThreadManager::Instance()->GetReThreadIndexMap()[threadIdx] : 0;
    SetSurfaceClearFunc({ threadIdx, clearFunc }, threadId);

    auto& bounds = params->GetBounds();
    std::lock_guard<std::mutex> lockTask(taskMutex_);
    if (!InitSurface(bounds.GetWidth(), bounds.GetHeight(), *paintFilterCanvas)) {
        RS_LOGE("Failed to init surface!");
        return;
    }

    // 1. Draw background of this drawing node by the main canvas.
    DrawBackground(canvas, bounds);

    // 2. Draw content of this drawing node by the content canvas.
    DrawRenderContent(canvas, bounds);

    // 3. Draw children of this drawing node by the main canvas.
    DrawChildren(canvas, bounds);

    // 4. Draw foreground of this drawing node by the main canvas.
    DrawForeground(canvas, bounds);
}

#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
struct SharedTextureContext {
    SharedTextureContext(std::shared_ptr<Drawing::Image> sharedImage)
        : sharedImage_(std::move(sharedImage)) {}
 
private:
    std::shared_ptr<Drawing::Image> sharedImage_;
};
 
static void DeleteSharedTextureContext(void* context)
{
    SharedTextureContext* cleanupHelper = static_cast<SharedTextureContext*>(context);
    if (cleanupHelper != nullptr) {
        delete cleanupHelper;
    }
}
#endif

void RSCanvasDrawingRenderNodeDrawable::DrawRenderContent(Drawing::Canvas& canvas, const Drawing::Rect& rect)
{
    DrawContent(*canvas_, rect);

    Rosen::Drawing::Matrix mat;
    auto& params = renderNode_->GetRenderParams();
    auto& frameRect = params->GetFrameRect();
    if (RSPropertiesPainter::GetGravityMatrix(params->GetFrameGravity(),
        { frameRect.GetLeft(), frameRect.GetTop(), frameRect.GetWidth(), frameRect.GetHeight() },
        params->GetBounds().GetWidth(), params->GetBounds().GetHeight(), mat)) {
        canvas.ConcatMatrix(mat);
    }
    auto ctx = renderNode_->GetContext().lock();
    Flush(rect.GetWidth(), rect.GetHeight(), ctx, renderNode_->GetId(),
        *static_cast<RSPaintFilterCanvas*>(&canvas)); // getimage
    std::lock_guard<std::mutex> lock(imageMutex_);
    if (image_ == nullptr) {
        RS_LOGE("Failed to draw gpu image!");
        return;
    }

    auto samplingOptions = Drawing::SamplingOptions(Drawing::FilterMode::LINEAR,
        Drawing::MipmapMode::LINEAR);
    Drawing::Paint paint;
    paint.SetStyle(Drawing::Paint::PaintStyle::PAINT_FILL);
    canvas.AttachPaint(paint);
    if (canvas.GetRecordingState()) {
        auto cpuImage = image_->MakeRasterImage();
        canvas.DrawImage(*cpuImage, 0.f, 0.f, samplingOptions);
    } else {
        canvas.DrawImage(*image_, 0.f, 0.f, samplingOptions);
    }
    canvas.DetachPaint();
}

void RSCanvasDrawingRenderNodeDrawable::OnCapture(Drawing::Canvas& canvas)
{
    // TODO
    OnDraw(canvas);
}

void RSCanvasDrawingRenderNodeDrawable::PlaybackInCorrespondThread()
{
    auto nodeId = renderNode_->GetId();
    auto ctx = renderNode_->GetContext().lock();
    auto rect = renderNode_->GetRenderParams()->GetBounds();
    auto task = [this, rect, nodeId, ctx]() {
        std::lock_guard<std::mutex> lockTask(taskMutex_);
        if (!surface_ || !canvas_) {
            return;
        }
    };
    RSTaskDispatcher::GetInstance().PostTask(threadId_, task, false);
}

bool RSCanvasDrawingRenderNodeDrawable::InitSurface(int width, int height, RSPaintFilterCanvas& canvas)
{
    if (IsNeedResetSurface()) {
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
        if (preThreadInfo_.second && surface_) {
            preThreadInfo_.second(std::move(surface_));
        }
        preThreadInfo_ = curThreadInfo_;
#endif
        if (!ResetSurface(width, height, canvas)) {
            return false;
        }
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    } else if ((isGpuSurface_) && (preThreadInfo_.first != curThreadInfo_.first)) {
        if (!ResetSurfaceWithTexture(width, height, canvas)) {
            return false;
        }
    }
#else
    }
#endif
    if (!surface_) {
        return false;
    }

    return true;
}

void RSCanvasDrawingRenderNodeDrawable::Flush(float width, float height, std::shared_ptr<RSContext> context,
    NodeId nodeId, RSPaintFilterCanvas& rscanvas)
{
    if (!recordingCanvas_) {
        if (rscanvas.GetParallelThreadIdx() != curThreadInfo_.first) {
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
            if (!backendTexture_.IsValid()) {
                RS_LOGE("RSCanvasDrawingRenderNodeDrawable::Flush backendTexture_ is nullptr");
                return;
            }
            Drawing::TextureOrigin origin = Drawing::TextureOrigin::BOTTOM_LEFT;
            Drawing::BitmapFormat info = Drawing::BitmapFormat{ image_->GetColorType(), image_->GetAlphaType() };
            SharedTextureContext* sharedContext = new SharedTextureContext(image_); // last image
            image_ = std::make_shared<Drawing::Image>();
            bool ret = image_->BuildFromTexture(*rscanvas.GetGPUContext(), backendTexture_.GetTextureInfo(), origin,
                info, nullptr, DeleteSharedTextureContext, sharedContext);
            if (!ret) {
                RS_LOGE("RSCanvasDrawingRenderNodeDrawable::Flush image BuildFromTexture failed");
                return;
            }
#endif
        } else {
            image_ = surface_->GetImageSnapshot(); // TODO: return image_
            backendTexture_ = surface_->GetBackendTexture();
            if (!backendTexture_.IsValid()) {
                RS_LOGE("RSCanvasDrawingRenderNodeDrawable::Flush !backendTexture_.IsValid() %d", __LINE__);
            }
        }

        if (image_) {
            SKResourceManager::Instance().HoldResource(image_);
        }
    } else {
        auto cmds = recordingCanvas_->GetDrawCmdList();
        if (cmds && !cmds->IsEmpty()) {
            recordingCanvas_ = std::make_shared<ExtendRecordingCanvas>(width, height, false);
            canvas_ = std::make_unique<RSPaintFilterCanvas>(recordingCanvas_.get());
            ProcessCPURenderInBackgroundThread(cmds, context, nodeId);
        }
    }
}

void RSCanvasDrawingRenderNodeDrawable::ProcessCPURenderInBackgroundThread(std::shared_ptr<Drawing::DrawCmdList> cmds,
    std::shared_ptr<RSContext> ctx, NodeId nodeId)
{
    auto surface = surface_;
    RSBackgroundThread::Instance().PostTask([this, cmds, surface, ctx, nodeId]() {
        if (!cmds || cmds->IsEmpty() || !surface || !ctx) {
            return;
        }
        if (surface != this->surface_) {
            return;
        }
        cmds->Playback(*surface->GetCanvas());
        auto image = surface->GetImageSnapshot(); // TODO: adapt multithread
        if (image) {
            SKResourceManager::Instance().HoldResource(image);
        }
        std::lock_guard<std::mutex> lock(this->imageMutex_);
        this->image_ = image;
        auto task = [ctx, nodeId = renderNode_->GetId()] () {
            if (UNLIKELY(!ctx)) {
                return;
            }
            ctx->PostTask([ctx, nodeId]() {
                if (auto node = ctx->GetNodeMap().GetRenderNode<RSCanvasDrawingRenderNode>(nodeId)) {
                    node->SetDirty();
                    ctx->RequestVsync();
                }
            });
        };
    });
}

void RSCanvasDrawingRenderNodeDrawable::ResetSurface()
{
    std::lock_guard<std::mutex> lockTask(taskMutex_);
    if (preThreadInfo_.second && surface_) {
        preThreadInfo_.second(std::move(surface_));
    }
    surface_ = nullptr;
    recordingCanvas_ = nullptr;
}

// use in IPC thread
Drawing::Bitmap RSCanvasDrawingRenderNodeDrawable::GetBitmap(const uint64_t tid)
{
    Drawing::Bitmap bitmap;
    std::lock_guard<std::mutex> lock(drawingMutex);
    if (!image_) {
        RS_LOGE("Failed to get bitmap, image is null!");
        return bitmap;
    }
    if (GetTid() != tid) {
        RS_LOGE("Failed to get bitmap: image is used by multi threads");
        return bitmap;
    }
    if (!image_->AsLegacyBitmap(bitmap)) {
        RS_LOGE("Failed to get bitmap, asLegacyBitmap failed");
    }
    return bitmap;
}

static bool WriteSkImageToPixelmap(std::shared_ptr<Drawing::Image> image, Drawing::ImageInfo info,
    std::shared_ptr<Media::PixelMap> pixelmap, const Drawing::Rect* rect)
{
    return image->ReadPixels(
        info, pixelmap->GetWritablePixels(), pixelmap->GetRowStride(),
        rect->GetLeft(), rect->GetTop());
}

// use in IPC thread
bool RSCanvasDrawingRenderNodeDrawable::GetPixelmap(const std::shared_ptr<Media::PixelMap> pixelmap,
    const Drawing::Rect* rect, const uint64_t tid, std::shared_ptr<Drawing::DrawCmdList> drawCmdList)
{
    std::lock_guard<std::mutex> lock(drawingMutex);
    if (!pixelmap || !rect) {
        RS_LOGE("RSCanvasDrawingRenderNodeDrawable::GetPixelmap: pixelmap is nullptr");
        return false;
    }

    if (!surface_) {
        RS_LOGE("RSCanvasDrawingRenderNodeDrawable::GetPixelmap: surface is nullptr");
        return false;
    }

    auto image = surface_->GetImageSnapshot();
    if (image == nullptr) {
        RS_LOGE("RSCanvasDrawingRenderNodeDrawable::GetPixelmap: GetImageSnapshot failed");
        return false;
    }

    if (GetTid() != tid) {
        RS_LOGE("RSCanvasDrawingRenderNodeDrawable::GetPixelmap: surface used by multi threads");
        return false;
    }

    Drawing::ImageInfo info = Drawing::ImageInfo { pixelmap->GetWidth(), pixelmap->GetHeight(),
        Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    if (!drawCmdList) {
        if (!WriteSkImageToPixelmap(image, info, pixelmap, rect)) {
            RS_LOGE("RSCanvasDrawingRenderNodeDrawable::GetPixelmap: readPixels failed");
            return false;
        }
        return true;
    }
    std::shared_ptr<Drawing::Surface> surface;
    std::unique_ptr<RSPaintFilterCanvas> canvas;
#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    auto gpuContext = canvas_->GetGPUContext();
    if (gpuContext == nullptr) {
        if (!WriteSkImageToPixelmap(image, info, pixelmap, rect)) {
            RS_LOGE("RSCanvasDrawingRenderNodeDrawable::GetPixelmap: readPixels failed");
        }
        return false;
    } else {
        Drawing::ImageInfo newInfo = Drawing::ImageInfo{ image->GetWidth(), image->GetHeight(),
            Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
        surface = Drawing::Surface::MakeRenderTarget(gpuContext.get(), false, newInfo);
        if (!surface) {
            if (!WriteSkImageToPixelmap(image, info, pixelmap, rect)) {
                RS_LOGE("RSCanvasDrawingRenderNodeDrawable::GetPixelmap: readPixels failed");
            }
            return false;
        }
        canvas = std::make_unique<RSPaintFilterCanvas>(surface.get());
    }
#else
    if (!WriteSkImageToPixelmap(image, info, pixelmap, rect)) {
        RS_LOGE("RSCanvasDrawingRenderNodeDrawable::GetPixelmap: readPixels failed");
    }
    return false;
#endif
    canvas->DrawImage(*image, 0, 0, Drawing::SamplingOptions());
    drawCmdList->Playback(*canvas, rect);
    auto pixelmapImage = surface->GetImageSnapshot();
    if (!WriteSkImageToPixelmap(pixelmapImage, info, pixelmap, rect)) {
        RS_LOGE("RSCanvasDrawingRenderNodeDrawable::GetPixelmap: readPixels failed");
        return false;
    }
    return true;
}

bool RSCanvasDrawingRenderNodeDrawable::IsNeedResetSurface() const
{
    return !surface_ || !surface_->GetCanvas();
}

bool RSCanvasDrawingRenderNodeDrawable::ResetSurface(int width, int height, RSPaintFilterCanvas& canvas)
{
    Drawing::ImageInfo info =
        Drawing::ImageInfo { width, height, Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };

#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    auto gpuContext = canvas.GetGPUContext();
    isGpuSurface_ = true;
    if (gpuContext == nullptr) {
        RS_LOGD("RSCanvasDrawingRenderNodeDrawable::ResetSurface: gpuContext is nullptr");
        isGpuSurface_ = false;
        surface_ = Drawing::Surface::MakeRaster(info);
    } else {
        surface_ = Drawing::Surface::MakeRenderTarget(gpuContext.get(), false, info);
        if (!surface_) {
            isGpuSurface_ = false;
            surface_ = Drawing::Surface::MakeRaster(info);
            if (!surface_) {
                RS_LOGE("RSCanvasDrawingRenderNodeDrawable::ResetSurface surface is nullptr");
                return false;
            }
            recordingCanvas_ = std::make_shared<ExtendRecordingCanvas>(width, height, false);
            canvas_ = std::make_unique<RSPaintFilterCanvas>(recordingCanvas_.get());
            return true;
        }
    }
#else
    surface_ = Drawing::Surface::MakeRaster(info);
#endif
    if (!surface_) {
        RS_LOGE("RSCanvasDrawingRenderNodeDrawable::ResetSurface surface is nullptr");
        return false;
    }
    canvas_ = std::make_shared<RSPaintFilterCanvas>(surface_.get());
    return true;
}

#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
inline void RSCanvasDrawingRenderNodeDrawable::ClearPreSurface(std::shared_ptr<Drawing::Surface>& preSurface)
{
    if (preThreadInfo_.second && preSurface) {
        preThreadInfo_.second(std::move(preSurface));
    }
}

bool RSCanvasDrawingRenderNodeDrawable::ResetSurfaceWithTexture(int width, int height, RSPaintFilterCanvas& canvas)
{
    auto preMatrix = canvas_->GetTotalMatrix();
    auto preSurface = surface_;
    if (!ResetSurface(width, height, canvas)) {
        ClearPreSurface(preSurface);
        return false;
    }
    if (!backendTexture_.IsValid()) {
        RS_LOGE("RSCanvasDrawingRenderNodeDrawable::ResetSurfaceWithTexture backendTexture_ is nullptr");
        ClearPreSurface(preSurface);
        return false;
    }

    Drawing::TextureOrigin origin = Drawing::TextureOrigin::BOTTOM_LEFT;
    Drawing::BitmapFormat bitmapFormat = { image_->GetColorType(), image_->GetAlphaType() };
    SharedTextureContext* sharedContext = new SharedTextureContext(image_); // will move image
    auto preImageInNewContext = std::make_shared<Drawing::Image>();
    if (!preImageInNewContext->BuildFromTexture(*canvas.GetGPUContext(), backendTexture_.GetTextureInfo(),
        origin, bitmapFormat, nullptr, DeleteSharedTextureContext, sharedContext)) {
        RS_LOGE("RSCanvasDrawingRenderNodeDrawable::ResetSurfaceWithTexture preImageInNewContext is nullptr");
        ClearPreSurface(preSurface);
        return false;
    }
    if (RSSystemProperties::GetRecordingEnabled()) {
        if (preImageInNewContext->IsTextureBacked()) {
            RS_LOGI("RSCanvasDrawingRenderNodeDrawable::ResetSurfaceWithTexture preImageInNewContext "
                "from texture to raster image");
            preImageInNewContext = preImageInNewContext->MakeRasterImage();
        }
    }
    canvas_->DrawImage(*preImageInNewContext, 0.f, 0.f, Drawing::SamplingOptions());
    preThreadInfo_ = curThreadInfo_;
    canvas_->SetMatrix(preMatrix);
    canvas_->Flush();
    backendTexture_ = surface_->GetBackendTexture();
    if (!backendTexture_.IsValid()) {
        RS_LOGE("RSCanvasDrawingRenderNodeDrawable::ResetSurfaceWithTexture backendTexture_ generate invalid");
    }
    image_ = preImageInNewContext;
    ClearPreSurface(preSurface);
    return true;
}
#endif


} // namespace OHOS::Rosen::DrawableV2
