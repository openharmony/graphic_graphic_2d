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
#include "include/gpu/vk/GrVulkanTrackerInterface.h"
#include "offscreen_render/rs_offscreen_render_thread.h"
#include "params/rs_canvas_drawing_render_params.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_task_dispatcher.h"
#include "pipeline/rs_uni_render_thread.h"
#include "pipeline/rs_uni_render_util.h"
#include "pipeline/sk_resource_manager.h"
#include "pipeline/parallel_render/rs_sub_thread_manager.h"
#include "platform/common/rs_log.h"

namespace OHOS::Rosen::DrawableV2 {
namespace {
    constexpr int EDGE_WIDTH_LIMIT = 1000;
    constexpr float DRAW_REGION_FOR_DFX_BORDER = 5.0f;
}
RSCanvasDrawingRenderNodeDrawable::Registrar RSCanvasDrawingRenderNodeDrawable::instance_;

RSCanvasDrawingRenderNodeDrawable::RSCanvasDrawingRenderNodeDrawable(std::shared_ptr<const RSRenderNode>&& node)
    : RSRenderNodeDrawable(std::move(node))
{
    auto renderNode = renderNode_.lock();
    if (renderNode == nullptr) {
        return;
    }
    auto nodeSp = std::const_pointer_cast<RSRenderNode>(renderNode);
    auto canvasDrawingRenderNode = std::static_pointer_cast<RSCanvasDrawingRenderNode>(nodeSp);
    canvasDrawingRenderNode->InitRenderContent();
}

RSCanvasDrawingRenderNodeDrawable::~RSCanvasDrawingRenderNodeDrawable()
{
#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    if (curThreadInfo_.second && surface_) {
        curThreadInfo_.second(std::move(surface_));
    }
#ifdef RS_ENABLE_VK
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        if (vulkanCleanupHelper_ && isPurge_) {
            vulkanCleanupHelper_->UnRef();
            isPurge_ = false;
        }
    }
#endif
#endif
}

RSRenderNodeDrawable::Ptr RSCanvasDrawingRenderNodeDrawable::OnGenerate(std::shared_ptr<const RSRenderNode> node)
{
    return new RSCanvasDrawingRenderNodeDrawable(std::move(node));
}

void RSCanvasDrawingRenderNodeDrawable::OnDraw(Drawing::Canvas& canvas)
{
    SetDrawSkipType(DrawSkipType::NONE);
    std::unique_lock<std::recursive_mutex> lock(drawableMutex_);
    if (!ShouldPaint()) {
        SetDrawSkipType(DrawSkipType::SHOULD_NOT_PAINT);
        return;
    }
    const auto& params = GetRenderParams();
    if (UNLIKELY(!params)) {
        SetDrawSkipType(DrawSkipType::RENDER_PARAMS_NULL);
        return;
    }
    if (params->GetCanvasDrawingSurfaceChanged()) {
        ResetSurface();
        params->SetCanvasDrawingSurfaceChanged(false);
    }
    auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(&canvas);
    RSAutoCanvasRestore acr(paintFilterCanvas, RSPaintFilterCanvas::SaveType::kCanvasAndAlpha);
    if (!canvas.GetRecordingState()) {
        params->ApplyAlphaAndMatrixToCanvas(*paintFilterCanvas);
    }

    auto& uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    if ((UNLIKELY(!uniParam) || uniParam->IsOpDropped()) && GetOpDropped() &&
        QuickReject(canvas, params->GetLocalDrawRect())) {
        SetDrawSkipType(DrawSkipType::OCCLUSION_SKIP);
        return;
    }

    auto threadIdx = paintFilterCanvas->GetParallelThreadIdx();
    auto clearFunc = [idx = threadIdx](std::shared_ptr<Drawing::Surface> surface) {
        // The second param is null, 0 is an invalid value.
        RSUniRenderUtil::ClearNodeCacheSurface(std::move(surface), nullptr, idx, 0);
    };
    auto threadId = paintFilterCanvas->GetIsParallelCanvas() ?
        RSSubThreadManager::Instance()->GetReThreadIndexMap()[threadIdx] : RSUniRenderThread::Instance().GetTid();
    SetSurfaceClearFunc({ threadIdx, clearFunc }, threadId);

    auto& bounds = params->GetBounds();
    auto surfaceParams = params->GetCanvasDrawingSurfaceParams();
    if (!InitSurface(surfaceParams.width, surfaceParams.height, *paintFilterCanvas)) {
        SetDrawSkipType(DrawSkipType::INIT_SURFACE_FAIL);
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

    // 5. Ready to clear resource.
    SetDrawCmdListsVisited(true);

    // Draw bounds rect for dfx.
    DrawRegionForDfx(canvas, bounds);
}

void RSCanvasDrawingRenderNodeDrawable::DrawRenderContent(Drawing::Canvas& canvas, const Drawing::Rect& rect)
{
    DrawContent(*canvas_, rect);
    if (!renderParams_) {
        return;
    }
    renderParams_->SetNeedProcess(false);
    Rosen::Drawing::Matrix mat;
    const auto& params = GetRenderParams();
    if (params == nullptr) {
        return;
    }
    auto& frameRect = params->GetFrameRect();
    if (RSPropertiesPainter::GetGravityMatrix(params->GetFrameGravity(),
        { frameRect.GetLeft(), frameRect.GetTop(), frameRect.GetWidth(), frameRect.GetHeight() },
        params->GetBounds().GetWidth(), params->GetBounds().GetHeight(), mat)) {
        canvas.ConcatMatrix(mat);
    }
    auto ctx = RSUniRenderThread::Instance().GetRSRenderThreadParams()->GetContext();
    Flush(rect.GetWidth(), rect.GetHeight(), ctx, nodeId_,
        *static_cast<RSPaintFilterCanvas*>(&canvas)); // getimage
    if (image_ == nullptr) {
        RS_LOGD("Failed to draw gpu image!");
        return;
    }

    auto samplingOptions = Drawing::SamplingOptions(Drawing::FilterMode::LINEAR,
        Drawing::MipmapMode::LINEAR);
    Drawing::Paint paint;
    paint.SetStyle(Drawing::Paint::PaintStyle::PAINT_FILL);
    canvas.AttachPaint(paint);
    canvas.DrawImage(*image_, 0.f, 0.f, samplingOptions);
    canvas.DetachPaint();
}

void RSCanvasDrawingRenderNodeDrawable::OnCapture(Drawing::Canvas& canvas)
{
    OnDraw(canvas);
}

void RSCanvasDrawingRenderNodeDrawable::Purge()
{
#ifdef RS_ENABLE_VK
    std::unique_lock<std::recursive_mutex> lock(drawableMutex_);
    if (curThreadInfo_.second && surface_) {
        curThreadInfo_.second(std::move(surface_));
        surface_ = nullptr;
        if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
            RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
            if (vulkanCleanupHelper_ && !isPurge_) {
                vulkanCleanupHelper_->Ref();
                isPurge_ = true;
            }
        }
    }
#endif
    RSRenderNodeDrawableAdapter::Purge();
}

void RSCanvasDrawingRenderNodeDrawable::PlaybackInCorrespondThread()
{
    auto canvasDrawingPtr = std::static_pointer_cast<DrawableV2::RSCanvasDrawingRenderNodeDrawable>(shared_from_this());
    pid_t threadId = threadId_;
    auto task = [this, canvasDrawingPtr, threadId]() {
        std::unique_lock<std::recursive_mutex> lock(drawableMutex_);
        if (!surface_ || !canvas_ || !renderParams_ || threadId != threadId_) {
            return;
        }
        if (renderParams_->GetCanvasDrawingSurfaceChanged()) {
            return;
        }
        auto rect = GetRenderParams()->GetBounds();
        DrawContent(*canvas_, rect);
        renderParams_->SetNeedProcess(false);
        canvas_->Flush();
        SetDrawCmdListsVisited(true);
    };
    RSTaskDispatcher::GetInstance().PostTask(threadId, task, false);
}

bool RSCanvasDrawingRenderNodeDrawable::InitSurface(int width, int height, RSPaintFilterCanvas& canvas)
{
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::OPENGL) {
        return InitSurfaceForGL(width, height, canvas);
    }
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return InitSurfaceForVK(width, height, canvas);
    }
#endif
    return false;
}

bool RSCanvasDrawingRenderNodeDrawable::InitSurfaceForGL(int width, int height, RSPaintFilterCanvas& canvas)
{
    if (IsNeedResetSurface()) {
        ClearPreSurface(surface_);
        preThreadInfo_ = curThreadInfo_;
        if (!ResetSurfaceForGL(width, height, canvas)) {
            return false;
        }
    } else if ((isGpuSurface_) && (preThreadInfo_.first != curThreadInfo_.first)) {
        if (!ResetSurfaceWithTexture(width, height, canvas)) {
            return false;
        }
    }
    if (!surface_) {
        return false;
    }
    return true;
}

bool RSCanvasDrawingRenderNodeDrawable::InitSurfaceForVK(int width, int height, RSPaintFilterCanvas& canvas)
{
    if (IsNeedResetSurface()) {
        ClearPreSurface(surface_);
        preThreadInfo_ = curThreadInfo_;
        if (!ResetSurfaceForVK(width, height, canvas)) {
            return false;
        }
    } else if ((isGpuSurface_) && (preThreadInfo_.first != curThreadInfo_.first)) {
        if (!ReuseBackendTexture(width, height, canvas)) {
            return false;
        }
    }
    if (!surface_) {
        return false;
    }
    return true;
}

void RSCanvasDrawingRenderNodeDrawable::FlushForGL(float width, float height, std::shared_ptr<RSContext> context,
    NodeId nodeId, RSPaintFilterCanvas& rscanvas)
{
    if (!recordingCanvas_) {
        if (rscanvas.GetParallelThreadIdx() != curThreadInfo_.first) {
            if (!backendTexture_.IsValid()) {
                RS_LOGE("RSCanvasDrawingRenderNodeDrawable::Flush backendTexture_ is nullptr");
                return;
            }
            if (rscanvas.GetGPUContext() == nullptr) {
                RS_LOGE("RSCanvasDrawingRenderNodeDrawable::Flush GPU context is nullptr");
                return;
            }
            Drawing::TextureOrigin origin = GetTextureOrigin();
            Drawing::BitmapFormat info = Drawing::BitmapFormat{ image_->GetColorType(), image_->GetAlphaType() };
            SharedTextureContext* sharedContext = new SharedTextureContext(image_); // last image
            image_ = std::make_shared<Drawing::Image>();
            ReleaseCaptureImage();
            bool ret = image_->BuildFromTexture(*rscanvas.GetGPUContext(), backendTexture_.GetTextureInfo(), origin,
                info, nullptr, SKResourceManager::DeleteSharedTextureContext, sharedContext);
            if (!ret) {
                RS_LOGE("RSCanvasDrawingRenderNodeDrawable::Flush image BuildFromTexture failed");
                return;
            }
        } else {
            image_ = surface_->GetImageSnapshot(); // planning: return image_
            backendTexture_ = surface_->GetBackendTexture();
            ReleaseCaptureImage();
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

void RSCanvasDrawingRenderNodeDrawable::FlushForVK(float width, float height, std::shared_ptr<RSContext> context,
    NodeId nodeId, RSPaintFilterCanvas& rscanvas)
{
    if (!recordingCanvas_) {
        REAL_ALLOC_CONFIG_SET_STATUS(true);
        image_ = surface_->GetImageSnapshot();
        REAL_ALLOC_CONFIG_SET_STATUS(false);
    } else {
        auto cmds = recordingCanvas_->GetDrawCmdList();
        if (cmds && !cmds->IsEmpty()) {
            recordingCanvas_ = std::make_shared<ExtendRecordingCanvas>(width, height, false);
            canvas_ = std::make_unique<RSPaintFilterCanvas>(recordingCanvas_.get());
            ProcessCPURenderInBackgroundThread(cmds, context, nodeId);
        }
    }
}

void RSCanvasDrawingRenderNodeDrawable::Flush(float width, float height, std::shared_ptr<RSContext> context,
    NodeId nodeId, RSPaintFilterCanvas& rscanvas)
{
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        FlushForVK(width, height, context, nodeId, rscanvas);
    }
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::OPENGL) {
        FlushForGL(width, height, context, nodeId, rscanvas);
    }
#endif
}

void RSCanvasDrawingRenderNodeDrawable::ProcessCPURenderInBackgroundThread(std::shared_ptr<Drawing::DrawCmdList> cmds,
    std::shared_ptr<RSContext> ctx, NodeId nodeId)
{
    auto surface = surface_;
    auto drawable = RSRenderNodeDrawableAdapter::GetDrawableById(nodeId);
    RSBackgroundThread::Instance().PostTask([drawable, cmds, surface, ctx, nodeId]() {
        if (!cmds || cmds->IsEmpty() || !surface || !ctx || !drawable) {
            return;
        }
        auto canvasDrawingDrawable = std::static_pointer_cast<DrawableV2::RSCanvasDrawingRenderNodeDrawable>(drawable);
        {
            std::unique_lock<std::recursive_mutex> lock(canvasDrawingDrawable->drawableMutex_);
            if (surface != canvasDrawingDrawable->surface_) {
                return;
            }
            cmds->Playback(*surface->GetCanvas());
            auto image = surface->GetImageSnapshot(); // planning: adapt multithread
            if (image) {
                SKResourceManager::Instance().HoldResource(image);
            }
            canvasDrawingDrawable->image_ = image;
        }
        if (UNLIKELY(!ctx)) {
            return;
        }
        RSMainThread::Instance()->PostTask([ctx, nodeId]() {
            if (auto node = ctx->GetNodeMap().GetRenderNode<RSCanvasDrawingRenderNode>(nodeId)) {
                ROSEN_LOGD("Node id %{public}" PRIu64 " set dirty, process in RSMainThread", nodeId);
                node->SetDirty();
                ctx->RequestVsync();
            }
        });
    });
}

void RSCanvasDrawingRenderNodeDrawable::ResetSurface()
{
    if (surface_ && surface_->GetImageInfo().GetWidth() > EDGE_WIDTH_LIMIT) {
        RS_LOGI("RSCanvasDrawingRenderNodeDrawable::ResetSurface id:%{public}" PRIu64 "", nodeId_);
    }
    if (preThreadInfo_.second && surface_) {
        preThreadInfo_.second(std::move(surface_));
    }
    surface_ = nullptr;
    recordingCanvas_ = nullptr;
    image_ = nullptr;
    canvas_ = nullptr;
#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    backendTexture_ = {};
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
#ifdef RS_ENABLE_VK
        if (vulkanCleanupHelper_ && isPurge_) {
            vulkanCleanupHelper_->UnRef();
            isPurge_ = false;
        }
#endif
        vulkanCleanupHelper_ = nullptr;
    }
#endif
}

// use in IPC thread
Drawing::Bitmap RSCanvasDrawingRenderNodeDrawable::GetBitmap(Drawing::GPUContext* grContext)
{
    Drawing::Bitmap bitmap;
    std::unique_lock<std::recursive_mutex> lock(drawableMutex_);
    // Judge valid of backendTexture_ by checking the image_.
    if (!image_) {
        RS_LOGE("Failed to get bitmap, image is null!");
        return bitmap;
    }
    if (!grContext) {
        RS_LOGE("Failed to get bitmap, grContext is null!");
        return bitmap;
    }
#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    Drawing::TextureOrigin origin = GetTextureOrigin();
    Drawing::BitmapFormat info = Drawing::BitmapFormat{ image_->GetColorType(), image_->GetAlphaType() };
    auto image = std::make_shared<Drawing::Image>();
    bool ret = image->BuildFromTexture(*grContext, backendTexture_.GetTextureInfo(), origin, info, nullptr);
    if (!ret) {
        RS_LOGE("RSCanvasDrawingRenderNodeDrawable::GetBitmap image BuildFromTexture failed");
        return bitmap;
    }
    if (!image->AsLegacyBitmap(bitmap)) {
        RS_LOGE("Failed to get bitmap, asLegacyBitmap failed");
    }
#endif
    return bitmap;
}

static bool WriteSkImageToPixelmap(std::shared_ptr<Drawing::Image> image, Drawing::ImageInfo info,
    std::shared_ptr<Media::PixelMap> pixelmap, const Drawing::Rect* rect)
{
    if (image == nullptr || pixelmap == nullptr || rect == nullptr) {
        return false;
    }
    return image->ReadPixels(
        info, pixelmap->GetWritablePixels(), pixelmap->GetRowStride(),
        rect->GetLeft(), rect->GetTop());
}

// use in IPC thread
bool RSCanvasDrawingRenderNodeDrawable::GetPixelmap(const std::shared_ptr<Media::PixelMap> pixelmap,
    const Drawing::Rect* rect, const uint64_t tid, std::shared_ptr<Drawing::DrawCmdList> drawCmdList)
{
    std::unique_lock<std::recursive_mutex> lock(drawableMutex_);

    if (!pixelmap || !rect) {
        RS_LOGE("RSCanvasDrawingRenderNodeDrawable::GetPixelmap: pixelmap is nullptr");
        return false;
    }
    if (!canvas_ || !image_ || !surface_) {
        RS_LOGE("RSCanvasDrawingRenderNodeDrawable::GetPixelmap: canvas/image/surface is nullptr");
        return false;
    }
    std::shared_ptr<Drawing::Image> image;
    std::shared_ptr<Drawing::GPUContext> grContext;
    if (!GetCurrentContextAndImage(grContext, image, tid)) {
        RS_LOGE("RSCanvasDrawingRenderNodeDrawable::GetPixelmap: GetCurrentContextAndImage failed");
        return false;
    }

    if (image == nullptr) {
        RS_LOGE("RSCanvasDrawingRenderNodeDrawable::GetPixelmap: image is nullptr");
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
    if (grContext == nullptr) {
        if (!WriteSkImageToPixelmap(image, info, pixelmap, rect)) {
            RS_LOGE("RSCanvasDrawingRenderNodeDrawable::GetPixelmap: readPixels failed");
        }
        return false;
    } else {
        Drawing::ImageInfo newInfo = Drawing::ImageInfo{ image->GetWidth(), image->GetHeight(),
            Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
        surface = Drawing::Surface::MakeRenderTarget(grContext.get(), false, newInfo);
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

void RSCanvasDrawingRenderNodeDrawable::ReleaseCaptureImage()
{
    RSOffscreenRenderThread::Instance().PostTask([image = captureImage_]() mutable { image = nullptr; });
    captureImage_ = nullptr;
}

void RSCanvasDrawingRenderNodeDrawable::DrawCaptureImage(RSPaintFilterCanvas& canvas)
{
#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    if (!image_) {
        OnDraw(canvas);
        return;
    }
    if (image_ && !image_->IsTextureBacked()) {
        canvas.DrawImage(*image_, 0, 0, Drawing::SamplingOptions());
        return;
    }

    if (RSSystemProperties::GetGpuApiType() == GpuApiType::OPENGL) {
        return;
    }

    if (!backendTexture_.IsValid()) {
        return;
    }
    if (canvas.GetGPUContext() == nullptr) {
        RS_LOGE("RSCanvasDrawingRenderNodeDrawable::DrawCaptureImage canvas.GetGPUContext is nullptr");
        return;
    }
    if (captureImage_ && captureImage_->IsValid(canvas.GetGPUContext().get())) {
        canvas.DrawImage(*captureImage_, 0, 0, Drawing::SamplingOptions());
        return;
    }
    Drawing::TextureOrigin origin = GetTextureOrigin();
    Drawing::BitmapFormat info = Drawing::BitmapFormat{ image_->GetColorType(), image_->GetAlphaType() };
    SharedTextureContext* sharedContext = new SharedTextureContext(image_);
    captureImage_ = std::make_shared<Drawing::Image>();
    bool ret = captureImage_->BuildFromTexture(*canvas.GetGPUContext(), backendTexture_.GetTextureInfo(), origin, info,
        nullptr, SKResourceManager::DeleteSharedTextureContext, sharedContext);
    if (!ret) {
        RS_LOGE("RSCanvasDrawingRenderNodeDrawable::DrawCaptureImage BuildFromTexture failed");
        return;
    }
    canvas.DrawImage(*captureImage_, 0, 0, Drawing::SamplingOptions());
#endif
}

bool RSCanvasDrawingRenderNodeDrawable::ResetSurfaceForVK(int width, int height, RSPaintFilterCanvas& canvas)
{
    Drawing::ImageInfo info =
        Drawing::ImageInfo { width, height, Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };

    bool isNewCreate = false;
#ifdef RS_ENABLE_VK
    auto gpuContext = canvas.GetRecordingState() ? nullptr : canvas.GetGPUContext();
    isGpuSurface_ = true;
    if (gpuContext == nullptr) {
        RS_LOGE("RSCanvasDrawingRenderNodeDrawable::ResetSurface: gpuContext is nullptr");
        isGpuSurface_ = false;
        surface_ = Drawing::Surface::MakeRaster(info);
    } else {
        if (!backendTexture_.IsValid() || !backendTexture_.GetTextureInfo().GetVKTextureInfo()) {
            backendTexture_ = RSUniRenderUtil::MakeBackendTexture(width, height);
            if (!backendTexture_.IsValid()) {
                surface_ = nullptr;
                recordingCanvas_ = nullptr;
                image_ = nullptr;
                canvas_ = nullptr;
                backendTexture_ = {};
                if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
                    RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
                    if (vulkanCleanupHelper_ && isPurge_) {
                        vulkanCleanupHelper_->UnRef();
                        isPurge_ = false;
                    }
                    vulkanCleanupHelper_ = nullptr;
                }
                RS_LOGE(
                    "RSCanvasDrawingRenderNodeDrawable::ResetSurfaceForVK size too big [%{public}d, %{public}d] failed",
                    width, height);
                return false;
            }
        }
        auto vkTextureInfo = backendTexture_.GetTextureInfo().GetVKTextureInfo();
        if (vulkanCleanupHelper_ == nullptr) {
            vulkanCleanupHelper_ = new NativeBufferUtils::VulkanCleanupHelper(
            RsVulkanContext::GetSingleton(), vkTextureInfo->vkImage, vkTextureInfo->vkAlloc.memory);
            isNewCreate = true;
        }
        REAL_ALLOC_CONFIG_SET_STATUS(true);
        surface_ = Drawing::Surface::MakeFromBackendTexture(gpuContext.get(), backendTexture_.GetTextureInfo(),
            Drawing::TextureOrigin::TOP_LEFT, 1, Drawing::ColorType::COLORTYPE_RGBA_8888, nullptr,
            NativeBufferUtils::DeleteVkImage, isNewCreate ? vulkanCleanupHelper_ : vulkanCleanupHelper_->Ref());
        REAL_ALLOC_CONFIG_SET_STATUS(false);
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
    recordingCanvas_ = nullptr;
    canvas_ = std::make_shared<RSPaintFilterCanvas>(surface_.get());
    if (isNewCreate) {
        canvas_->Clear(Drawing::Color::COLOR_TRANSPARENT);
    }
    return true;
}

bool RSCanvasDrawingRenderNodeDrawable::ResetSurfaceForGL(int width, int height, RSPaintFilterCanvas& canvas)
{
    Drawing::ImageInfo info =
        Drawing::ImageInfo { width, height, Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };

#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    auto gpuContext = canvas.GetRecordingState() ? nullptr : canvas.GetGPUContext();
    isGpuSurface_ = true;
    if (gpuContext == nullptr) {
        RS_LOGE("RSCanvasDrawingRenderNodeDrawable::ResetSurface: gpuContext is nullptr");
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
    recordingCanvas_ = nullptr;
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

bool RSCanvasDrawingRenderNodeDrawable::ReuseBackendTexture(int width, int height, RSPaintFilterCanvas& canvas)
{
    auto preMatrix = canvas_->GetTotalMatrix();
    auto preDeviceClipBounds = canvas_->GetDeviceClipBounds();
    auto preSaveCount = canvas_->GetSaveCount();
    auto preSurface = surface_;
    if (!ResetSurfaceForVK(width, height, canvas)) {
        ClearPreSurface(preSurface);
        return false;
    }
    if (preSaveCount > 1) {
        canvas_->Save();
    }
    canvas_->ClipIRect(preDeviceClipBounds);
    canvas_->SetMatrix(preMatrix);
    ClearPreSurface(preSurface);
    preThreadInfo_ = curThreadInfo_;
    ReleaseCaptureImage();
    return true;
}

bool RSCanvasDrawingRenderNodeDrawable::GetCurrentContextAndImage(std::shared_ptr<Drawing::GPUContext>& grContext,
    std::shared_ptr<Drawing::Image>& image, const uint64_t tid)
{
    if (tid == preThreadInfo_.first) {
        grContext = canvas_->GetGPUContext();
        image = image_;
    } else {
        auto realTid = gettid();
        if (realTid == RSUniRenderThread::Instance().GetTid()) {
            grContext = RSUniRenderThread::Instance().GetRenderEngine()->GetRenderContext()->GetSharedDrGPUContext();
        } else {
            if (!RSSubThreadManager::Instance()->GetGrContextFromSubThread(realTid)) {
                RS_LOGE("RSCanvasDrawingRenderNodeDrawable::GetCurrentContextAndImage get grGrContext failed");
                return false;
            }
            grContext = RSSubThreadManager::Instance()->GetGrContextFromSubThread(realTid);
        }

        if (!grContext || !backendTexture_.IsValid()) {
            return false;
        }
        Drawing::TextureOrigin origin = GetTextureOrigin();
        Drawing::BitmapFormat info = Drawing::BitmapFormat{ image_->GetColorType(), image_->GetAlphaType() };
        image = std::make_shared<Drawing::Image>();
        bool ret = image->BuildFromTexture(*grContext, backendTexture_.GetTextureInfo(), origin, info, nullptr);
        if (!ret) {
            RS_LOGE("RSCanvasDrawingRenderNodeDrawable::GetCurrentContextAndImage BuildFromTexture failed");
            return false;
        }
    }
    return true;
}

bool RSCanvasDrawingRenderNodeDrawable::ResetSurfaceWithTexture(int width, int height, RSPaintFilterCanvas& canvas)
{
    if (width > EDGE_WIDTH_LIMIT) {
        RS_LOGI("RSCanvasDrawingRenderNodeDrawable::ResetSurfaceWithTexture id:%{public}" PRIu64 " "
             "width:%{public}d height:%{public}d ", nodeId_, width, height);
    }
    auto preMatrix = canvas_->GetTotalMatrix();
    auto preDeviceClipBounds = canvas_->GetDeviceClipBounds();
    auto preSaveCount = canvas_->GetSaveCount();
    auto preSurface = surface_;
    if (!ResetSurfaceForGL(width, height, canvas)) {
        ClearPreSurface(preSurface);
        return false;
    }
    if (!backendTexture_.IsValid()) {
        RS_LOGE("RSCanvasDrawingRenderNodeDrawable::ResetSurfaceWithTexture backendTexture_ is nullptr");
        ClearPreSurface(preSurface);
        return false;
    }
    if (canvas.GetGPUContext() == nullptr) {
        RS_LOGE("RSCanvasDrawingRenderNodeDrawable::ResetSurfaceWithTexture GPU context is nullptr");
        ClearPreSurface(preSurface);
        return false;
    }

    Drawing::TextureOrigin origin = GetTextureOrigin();
    Drawing::BitmapFormat bitmapFormat = { image_->GetColorType(), image_->GetAlphaType() };
    SharedTextureContext* sharedContext = new SharedTextureContext(image_); // will move image
    auto preImageInNewContext = std::make_shared<Drawing::Image>();
    if (!preImageInNewContext->BuildFromTexture(*canvas.GetGPUContext(), backendTexture_.GetTextureInfo(),
        origin, bitmapFormat, nullptr, SKResourceManager::DeleteSharedTextureContext, sharedContext)) {
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
    if (preSaveCount > 1) {
        canvas_->Save();
    }
    canvas_->ClipIRect(preDeviceClipBounds);
    canvas_->SetMatrix(preMatrix);
    canvas_->Flush();
    backendTexture_ = surface_->GetBackendTexture();
    if (!backendTexture_.IsValid()) {
        RS_LOGE("RSCanvasDrawingRenderNodeDrawable::ResetSurfaceWithTexture backendTexture_ generate invalid");
    }
    image_ = preImageInNewContext;
    ReleaseCaptureImage();
    ClearPreSurface(preSurface);
    preThreadInfo_ = curThreadInfo_;
    return true;
}
#endif

Drawing::TextureOrigin RSCanvasDrawingRenderNodeDrawable::GetTextureOrigin()
{
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::OPENGL) {
        return Drawing::TextureOrigin::BOTTOM_LEFT;
    }
    return Drawing::TextureOrigin::TOP_LEFT;
}

void RSCanvasDrawingRenderNodeDrawable::DrawRegionForDfx(Drawing::Canvas& canvas, const Drawing::Rect& bounds)
{
    if (!RSSystemParameters::GetCanvasDrawingNodeRegionEnabled()) {
        return;
    }
    Drawing::Pen pen;
    pen.SetWidth(DRAW_REGION_FOR_DFX_BORDER);
    pen.SetColor(Drawing::Color::COLOR_RED);
    canvas.AttachPen(pen);
    canvas.DrawRect(Drawing::Rect(0, 0, bounds.GetWidth(), bounds.GetHeight()));
    canvas.DetachPen();
}

} // namespace OHOS::Rosen::DrawableV2
