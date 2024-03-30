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
    if ((!uniParam || uniParam->IsOpDropped()) && static_cast<RSPaintFilterCanvas*>(&canvas)->GetDirtyFlag() &&
        QuickReject(canvas, params->GetLocalDrawRect())) {
        RS_LOGD("CanvasDrawingNode[%{public}" PRIu64 "] have no intersect with canvas's clipRegion", params->GetId());
        return;
    }

    auto clearFunc = [](std::shared_ptr<Drawing::Surface> surface) {
        // The second param is null, 0 is an invalid value.
        RSUniRenderUtil::ClearNodeCacheSurface(std::move(surface), nullptr, UNI_RENDER_THREAD_INDEX, 0);
    };
    SetSurfaceClearFunc({ UNI_RENDER_THREAD_INDEX, clearFunc });

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
    Flush(rect.GetWidth(), rect.GetHeight(), ctx, renderNode_->GetId());
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
        // todo clear op
        // DrawContent(*canvas_, rect);
        // DrawForeground(*canvas_, rect);
        // node->SetNeedProcess(false);
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
    NodeId nodeId)
{
    if (!recordingCanvas_) {
        image_ = surface_->GetImageSnapshot();
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
        auto image = surface->GetImageSnapshot();
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

bool RSCanvasDrawingRenderNodeDrawable::GetPixelmap(const std::shared_ptr<Media::PixelMap> pixelmap,
    const Drawing::Rect* rect, const uint64_t tid, std::shared_ptr<Drawing::DrawCmdList> drawCmdList)
{
    std::lock_guard<std::mutex> lock(drawingMutex);
    if (!pixelmap || !rect) {
        RS_LOGE("RSCanvasDrawingRenderNode::GetPixelmap: pixelmap is nullptr");
        return false;
    }

    if (!surface_) {
        RS_LOGE("RSCanvasDrawingRenderNode::GetPixelmap: surface is nullptr");
        return false;
    }

    auto image = surface_->GetImageSnapshot();
    if (image == nullptr) {
        RS_LOGE("RSCanvasDrawingRenderNode::GetPixelmap: GetImageSnapshot failed");
        return false;
    }

    if (GetTid() != tid) {
        RS_LOGE("RSCanvasDrawingRenderNode::GetPixelmap: surface used by multi threads");
        return false;
    }

    Drawing::ImageInfo info = Drawing::ImageInfo { pixelmap->GetWidth(), pixelmap->GetHeight(),
        Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    if (!drawCmdList) {
        if (!WriteSkImageToPixelmap(image, info, pixelmap, rect)) {
            RS_LOGE("RSCanvasDrawingRenderNode::GetPixelmap: readPixels failed");
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
            RS_LOGE("RSCanvasDrawingRenderNode::GetPixelmap: readPixels failed");
        }
        return false;
    } else {
        Drawing::ImageInfo newInfo = Drawing::ImageInfo{ image->GetWidth(), image->GetHeight(),
            Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
        surface = Drawing::Surface::MakeRenderTarget(gpuContext.get(), false, newInfo);
        if (!surface) {
            if (!WriteSkImageToPixelmap(image, info, pixelmap, rect)) {
                RS_LOGE("RSCanvasDrawingRenderNode::GetPixelmap: readPixels failed");
            }
            return false;
        }
        canvas = std::make_unique<RSPaintFilterCanvas>(surface.get());
    }
#else
    if (!WriteSkImageToPixelmap(image, info, pixelmap, rect)) {
        RS_LOGE("RSCanvasDrawingRenderNode::GetPixelmap: readPixels failed");
    }
    return false;
#endif
    canvas->DrawImage(*image, 0, 0, Drawing::SamplingOptions());
    drawCmdList->Playback(*canvas, rect);
    auto pixelmapImage = surface->GetImageSnapshot();
    if (!WriteSkImageToPixelmap(pixelmapImage, info, pixelmap, rect)) {
        RS_LOGE("RSCanvasDrawingRenderNode::GetPixelmap: readPixels failed");
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
        RS_LOGD("RSCanvasDrawingRenderNodeContent::ResetSurface: gpuContext is nullptr");
        isGpuSurface_ = false;
        surface_ = Drawing::Surface::MakeRaster(info);
    } else {
        surface_ = Drawing::Surface::MakeRenderTarget(gpuContext.get(), false, info);
        if (!surface_) {
            isGpuSurface_ = false;
            surface_ = Drawing::Surface::MakeRaster(info);
            if (!surface_) {
                RS_LOGE("RSCanvasDrawingRenderNode::ResetSurface surface is nullptr");
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
        RS_LOGE("RSCanvasDrawingRenderNodeContent::ResetSurface surface is nullptr");
        return false;
    }
    canvas_ = std::make_shared<RSPaintFilterCanvas>(surface_.get());
    return true;
}

#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
bool RSCanvasDrawingRenderNodeDrawable::ResetSurfaceWithTexture(int width, int height, RSPaintFilterCanvas& canvas)
{
    auto preMatrix = canvas_->GetTotalMatrix();
    auto preSurface = surface_;
    if (!ResetSurface(width, height, canvas)) {
        return false;
    }
    auto image = preSurface->GetImageSnapshot();
    if (!image) {
        return false;
    }
    Drawing::TextureOrigin origin = Drawing::TextureOrigin::BOTTOM_LEFT;
    auto sharedBackendTexture = image->GetBackendTexture(false, &origin);
    if (!sharedBackendTexture.IsValid()) {
        RS_LOGE("RSCanvasDrawingRenderNode::ResetSurfaceWithTexture sharedBackendTexture is nullptr");
        return false;
    }

    Drawing::BitmapFormat bitmapFormat = { image->GetColorType(), image->GetAlphaType() };
    auto sharedTexture = std::make_shared<Drawing::Image>();
    if (!sharedTexture->BuildFromTexture(
            *canvas.GetGPUContext(), sharedBackendTexture.GetTextureInfo(), origin, bitmapFormat, nullptr)) {
        RS_LOGE("RSCanvasDrawingRenderNode::ResetSurfaceWithTexture sharedTexture is nullptr");
        return false;
    }
    if (RSSystemProperties::GetRecordingEnabled()) {
        if (sharedTexture->IsTextureBacked()) {
            RS_LOGI("RSCanvasDrawingRenderNode::ResetSurfaceWithTexture sharedTexture from texture to raster image");
            sharedTexture = sharedTexture->MakeRasterImage();
        }
    }
    canvas_->DrawImage(*sharedTexture, 0.f, 0.f, Drawing::SamplingOptions());
    if (preThreadInfo_.second && preSurface) {
        preThreadInfo_.second(std::move(preSurface));
    }
    preThreadInfo_ = curThreadInfo_;
    canvas_->SetMatrix(preMatrix);
    canvas_->Flush();
    return true;
}
#endif


} // namespace OHOS::Rosen::DrawableV2
