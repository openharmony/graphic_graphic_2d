/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "modifier_render_thread/rs_canvas_modifiers_draw.h"

#ifdef USE_M133_SKIA
#include "include/gpu/ganesh/GrBackendSemaphore.h"
#include "include/gpu/ganesh/vk/GrVkBackendSemaphore.h"
#endif
#include "concurrent_task_client.h"
#include "feature/hdr/rs_colorspace_util.h"
#include "qos.h"

#include "platform/common/rs_log.h"
#include "platform/ohos/backend/rs_surface_ohos_vulkan.h"
#include "render_context/render_context.h"
#include "render_context/shader_cache.h"
#include "vulkan_context/native_buffer_utils.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr const char* CLEAN_FREE_BUFFERS_TASK_NAME = "CleanFreeBuffersTask";
constexpr int64_t CLEAN_FREE_BUFFERS_DURATION = 2000;
constexpr int64_t CLEAN_FREE_BUFFERS_IMMEDIATELY_DELAY = 50;
}

// RSCanvasModifiersDrawable Start
void RSCanvasModifiersDrawable::Reset()
{
    width_ = 0;
    height_ = 0;
    forceFlushBuffer_ = false;
    needResetCanvas_ = false;
    surfaceFrame_ = nullptr;
    if (drawCmdListCache_ != nullptr) {
        drawCmdListCache_->clear();
    }
}

void RSCanvasModifiersDrawable::CreateProducerSurface(std::weak_ptr<RSRenderInterface> weakRenderInterface,
    std::shared_ptr<Drawing::GPUContext> gpuContext, size_t& maxGpuResourceBytes)
{
    auto renderInterface = weakRenderInterface.lock();
    if (renderInterface == nullptr) {
        RS_LOGE(
            "RSCanvasModifiersDrawable::CreateProducerSurface, null renderInterface, nodeId=%{public}" PRIu64, nodeId_);
        return;
    }
    if (gpuContext == nullptr) {
        RS_LOGE("RSCanvasModifiersDrawable::CreateProducerSurface, null gpuContext, nodeId=%{public}" PRIu64, nodeId_);
        return;
    }

    auto surface = renderInterface->CreateCanvasDrawingNodeSurface(nodeId_);
    if (surface == nullptr) {
        RS_LOGE("RSCanvasModifiersDrawable::CreateProducerSurface, null surface, nodeId=%{public}" PRIu64, nodeId_);
        return;
    }

    static std::once_flag flag;
    std::call_once(flag,
        [gpuContext, &maxGpuResourceBytes]() { gpuContext->GetResourceCacheLimits(nullptr, &maxGpuResourceBytes); });
    auto producerSurface = std::make_shared<RSSurfaceOhosVulkan>(surface);
    producerSurface->SetRenderContext(renderContext_);
    producerSurface->SetTimeOut(2); // Timeout 2ms
    producerSurface_ = producerSurface;
    drawCmdListCache_ = std::make_unique<std::vector<Drawing::DrawCmdListPtr>>();
}

void RSCanvasModifiersDrawable::ReleaseProducerSurface(std::weak_ptr<RSRenderInterface> weakRenderInterface)
{
    auto renderInterface = weakRenderInterface.lock();
    if (renderInterface == nullptr) {
        RS_LOGE("RSCanvasModifiersDrawable::ReleaseProducerSurface, null renderInterface, nodeId=%{public}" PRIu64,
            nodeId_);
        return;
    }
    renderInterface->ReleaseCanvasDrawingNodeSurface(nodeId_);
}

void RSCanvasModifiersDrawable::ResetSurface(
    int width, int height, bool sizeOutOfGpuLimit, GraphicColorGamut colorSpace)
{
    if (producerSurface_ == nullptr) {
        RS_LOGE("RSCanvasModifiersDrawable::ResetSurface: Null producer surface, nodeId=%{public}" PRIu64, nodeId_);
        return;
    }
    if (width_ == width && height_ == height) {
        RS_LOGE("RSCanvasModifiersDrawable::ResetSurface: Same width and height, nodeId=%{public}" PRIu64, nodeId_);
        return;
    }

    auto ohosSurface = std::static_pointer_cast<RSSurfaceOhosVulkan>(producerSurface_);
    // Current buffer is not flushed, cancel it.
    if (surfaceFrame_ != nullptr) {
        ohosSurface->CancelBufferForCurrentFrame();
    }

    Reset();
    if (sizeOutOfGpuLimit) {
        return;
    }

    width_ = width;
    height_ = height;
    needResetCanvas_ = true;
    forceFlushBuffer_ = true;
    ohosSurface->SetColorSpace(colorSpace);
    HDIV::CM_ColorSpaceType colorSpaceType = HDIV::CM_SRGB_FULL;
    if (RSColorSpaceUtil::ConvertColorGamutToSpaceType(colorSpace, colorSpaceType)) {
        auto surface = ohosSurface->GetSurface();
        // ATTRKEY_COLORSPACE_INFO is color space key
        if (surface && surface->SetUserData("ATTRKEY_COLORSPACE_INFO", std::to_string(colorSpaceType)) != GSERROR_OK) {
            RS_LOGE("RSCanvasModifiersDrawable::ResetSurface: SetColorSpace fail, nodeId=%{public}" PRIu64, nodeId_);
        }
    }
    Draw();
}

void RSCanvasModifiersDrawable::UpdateContent(Drawing::DrawCmdListPtr drawCmdList, bool forceFlushBuffer)
{
    if (drawCmdListCache_ == nullptr) {
        RS_LOGE("RSCanvasModifiersDrawable::UpdateContent: Null drawCmdListCache, drop drawCmdList, "
            "nodeId=%{public}" PRIu64, nodeId_);
        return;
    }

    if (drawCmdList != nullptr) {
        drawCmdListCache_->emplace_back(drawCmdList);
    }
    forceFlushBuffer_ = forceFlushBuffer;
    Draw();
}

void RSCanvasModifiersDrawable::Draw()
{
    if (producerSurface_ == nullptr) {
        RS_LOGE("RSCanvasModifiersDrawable::Draw: Null producer surface, nodeId=%{public}" PRIu64, nodeId_);
        return;
    }
    if (drawCmdListCache_->empty() && !forceFlushBuffer_) {
        return;
    }

    if (surfaceFrame_ == nullptr) {
        surfaceFrame_ = RequestBufferAndDrawHistory();
    }
    if (surfaceFrame_ == nullptr) {
        RS_LOGE("RSCanvasModifiersDrawable::Draw: Null surfaceFrame, nodeId=%{public}" PRIu64, nodeId_);
        return;
    }
    for (auto& cmdList : *drawCmdListCache_) {
        cmdList->FlushImageCache();
        Playback(cmdList);
        cmdList->ClearOp();
    }
    drawCmdListCache_->clear();
    forceFlushBuffer_ = false;
}

std::unique_ptr<RSSurfaceFrame> RSCanvasModifiersDrawable::RequestBufferAndDrawHistory()
{
    auto ohosSurface = std::static_pointer_cast<RSSurfaceOhos>(producerSurface_);
    auto renderFrame = ohosSurface->RequestFrame(width_, height_);
    if (renderFrame == nullptr) {
        RS_LOGE("RSCanvasModifiersDrawable::RequestBufferAndDrawHistory: Null renderFrame, nodeId=%{public}" PRIu64
            ", width=%{public}d, height=%{public}d", nodeId_, width_, height_);
        return nullptr;
    }
    auto drawingSurface = renderFrame->GetSurface();
    if (drawingSurface == nullptr) {
        RS_LOGE("RSCanvasModifiersDrawable::RequestBufferAndDrawHistory: Null drawingSurface, nodeId=%{public}" PRIu64,
            nodeId_);
        return nullptr;
    }
    auto canvas = drawingSurface->GetCanvas();
    if (canvas == nullptr) {
        RS_LOGE(
            "RSCanvasModifiersDrawable::RequestBufferAndDrawHistory: Null canvas, nodeId=%{public}" PRIu64, nodeId_);
        return nullptr;
    }

    if (drawingSurface_ != nullptr && drawingSurface_ != drawingSurface) {
        if (auto lastCanvas = drawingSurface_->GetCanvas()) {
            canvas->InheritStateAndContentFrom(lastCanvas.get(), false);
        }
    }
    if (needResetCanvas_) {
        canvas->Clear(Drawing::Color::COLOR_TRANSPARENT);
        canvas->RestoreToCount(1);
        canvas->SetMatrix(Drawing::Matrix());
        needResetCanvas_ = false;
    }
    drawingSurface_ = drawingSurface;
    return renderFrame;
}

void RSCanvasModifiersDrawable::Playback(const Drawing::DrawCmdListPtr& cmdList)
{
    auto canvas = drawingSurface_->GetCanvas();
    cmdList->Playback(*canvas);
    if (RSSystemProperties::GetHybridRenderDfxEnabled()) {
        auto matrix = canvas->GetTotalMatrix();
        canvas->SetMatrix(Drawing::Matrix());
        Drawing::Pen pen;
        pen.SetWidth(10);         // DFX border width 10
        pen.SetColor(0xFFFF8000); // 0xFFFF8000 is orange
        canvas->AttachPen(pen);
        auto rect = Drawing::Rect(0, 0, drawingSurface_->Width(), drawingSurface_->Height());
        canvas->DrawRect(rect);
        canvas->DetachPen();
        canvas->SetMatrix(matrix);
    }
}

sptr<SurfaceBuffer> RSCanvasModifiersDrawable::GetBuffer(bool needFlushBuffer)
{
    if (producerSurface_ == nullptr) {
        RS_LOGE("RSCanvasModifiersDrawable::GetBuffer: Null producer surface, nodeId=%{public}" PRIu64, nodeId_);
        return nullptr;
    }
    if (surfaceFrame_ == nullptr) {
        return nullptr;
    }
    auto ohosSurface = std::static_pointer_cast<RSSurfaceOhosVulkan>(producerSurface_);
    if (auto buffer = ohosSurface->GetCurrentBuffer()) {
        if (needFlushBuffer) {
            ohosSurface->OnFlushBuffer();
        }
        return buffer;
    }
    return nullptr;
}

void RSCanvasModifiersDrawable::OnDirtyBufferCollected(int64_t lastFlushBufferTime)
{
    surfaceFrame_ = nullptr;
    lastFlushBufferTime_ = lastFlushBufferTime;
}

bool RSCanvasModifiersDrawable::IsFree(int64_t now, int64_t maxDuration)
{
    if (lastFlushBufferTime_ == 0) {
        return false;
    }
    auto duration = now - lastFlushBufferTime_;
    if (duration <= maxDuration) {
        return false;
    }
    if (producerSurface_ == nullptr) {
        RS_LOGE("RSCanvasModifiersDrawable::IsFree: Null producer surface, nodeId=%{public}" PRIu64, nodeId_);
        return false;
    }
    return true;
}

void RSCanvasModifiersDrawable::CleanBuffer()
{
    if (producerSurface_ == nullptr) {
        RS_LOGE("RSCanvasModifiersDrawable::CleanBuffer: Null producer surface, nodeId=%{public}" PRIu64, nodeId_);
        return;
    }
    std::static_pointer_cast<RSSurfaceOhosVulkan>(producerSurface_)->CleanReleasedBuffers();
    lastFlushBufferTime_ = 0;
    RS_LOGI("RSCanvasModifiersDrawable::CleanBuffer, nodeId=%{public}" PRIu64, nodeId_);
}

bool RSCanvasModifiersDrawable::GetPixelMap(std::shared_ptr<Media::PixelMap> pixelMap, const Drawing::Rect* rect,
    Drawing::DrawCmdListPtr drawCmdList, std::shared_ptr<Drawing::GPUContext> gpuContext)
{
    if (pixelMap == nullptr || rect == nullptr) {
        return false;
    }
    auto bitmapFormat = Drawing::BitmapFormat { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    auto image = GetImage(bitmapFormat, gpuContext);
    if (image == nullptr) {
        return false;
    }

    if (drawCmdList != nullptr && !drawCmdList->IsEmpty()) {
        auto imageInfo = Drawing::ImageInfo { width_, height_, bitmapFormat.colorType, bitmapFormat.alphaType };
        auto surface = Drawing::Surface::MakeRenderTarget(gpuContext.get(), false, imageInfo);
        if (auto canvas = surface != nullptr ? surface->GetCanvas() : nullptr) {
            canvas->DrawImage(*image, 0, 0, Drawing::SamplingOptions());
            drawCmdList->Playback(*canvas);
            canvas->Flush();
            image = surface->GetImageSnapshot();
        }
        if (image == nullptr) {
            RS_LOGE("RSCanvasModifiersDrawable::GetPixelMap: GetImageSnapshot fail, nodeId=%{public}" PRIu64, nodeId_);
            return false;
        }
    }

    auto imageInfo = Drawing::ImageInfo { pixelMap->GetWidth(), pixelMap->GetHeight(), bitmapFormat.colorType,
        bitmapFormat.alphaType };
    if (!image->ReadPixels(
        imageInfo, pixelMap->GetWritablePixels(), pixelMap->GetRowStride(), rect->GetLeft(), rect->GetTop())) {
        RS_LOGE("RSCanvasModifiersDrawable::GetPixelMap: ReadPixels fail, nodeId=%{public}" PRIu64, nodeId_);
        return false;
    }
    if (pixelMap->GetAllocatorType() == Media::AllocatorType::DMA_ALLOC) {
        auto* surfaceBuffer = static_cast<SurfaceBuffer*>(pixelMap->GetFd());
        if (surfaceBuffer != nullptr && (surfaceBuffer->GetUsage() & BUFFER_USAGE_MEM_MMZ_CACHE)) {
            surfaceBuffer->FlushCache();
        }
    }
    return true;
}

bool RSCanvasModifiersDrawable::GetBitmap(Drawing::Bitmap& bitmap, std::shared_ptr<Drawing::GPUContext> gpuContext)
{
    auto alphaType = Drawing::ALPHATYPE_PREMUL;
    const auto& format = bitmap.GetFormat();
    if (format.alphaType != Drawing::ALPHATYPE_UNKNOWN) {
        alphaType = format.alphaType;
    }
    auto bitmapFormat = Drawing::BitmapFormat { Drawing::COLORTYPE_RGBA_8888, alphaType };
    auto image = GetImage(bitmapFormat, gpuContext);
    if (image == nullptr) {
        return false;
    }
    if (!image->AsLegacyBitmap(bitmap)) {
        RS_LOGE("RSCanvasModifiersDrawable::GetBitmap: AsLegacyBitmap fail, nodeId=%{public}" PRIu64, nodeId_);
        return false;
    }
    return true;
}

std::shared_ptr<Drawing::Image> RSCanvasModifiersDrawable::GetImage(
    const Drawing::BitmapFormat& bitmapFormat, std::shared_ptr<Drawing::GPUContext> gpuContext)
{
    if (drawingSurface_ == nullptr) {
        RS_LOGE("RSCanvasModifiersDrawable::GetImage: Null surface, nodeId=%{public}" PRIu64, nodeId_);
        return nullptr;
    }
    if (gpuContext == nullptr) {
        RS_LOGE("RSCanvasModifiersDrawable::GetImage: Null GPU context, nodeId=%{public}" PRIu64, nodeId_);
        return nullptr;
    }

    auto image = std::make_shared<Drawing::Image>();
    if (!image->BuildFromTexture(*gpuContext, drawingSurface_->GetBackendTexture().GetTextureInfo(),
        Drawing::TextureOrigin::TOP_LEFT, bitmapFormat, nullptr)) {
        RS_LOGE("RSCanvasModifiersDrawable::GetImage: BuildFromTexture fail, nodeId=%{public}" PRIu64, nodeId_);
        return nullptr;
    }
    return image;
}
// RSCanvasModifiersDrawable End

// Thread-related methods
void RSCanvasModifiersDraw::StartThread()
{
    if (threadStarted_.load() || threadDestroyed_.load()) {
        return;
    }

    threadStarted_.store(true);
    runner_ = AppExecFwk::EventRunner::Create("CanvasModifiersDraw");
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
    runner_->Run();
    PostTask([] {
        OHOS::ConcurrentTask::IntervalReply reply;
        reply.tid = gettid();
        OHOS::ConcurrentTask::ConcurrentTaskClient::GetInstance().QueryInterval(
            OHOS::ConcurrentTask::QUERY_MODIFIER_DRAW, reply);
        SetThreadQos(QOS::QosLevel::QOS_USER_INTERACTIVE);
        // Init shader cache, shader save delay time differs between uni-render and hybrid-render.
        std::string vkVersion = std::to_string(VK_API_VERSION_1_2);
        auto size = vkVersion.size();
        auto& cache = ShaderCache::Instance();
        cache.InitShaderCache(vkVersion.c_str(), size, false);
    });
    CleanFreeBuffers(CLEAN_FREE_BUFFERS_DURATION, false);
    RS_LOGI("RSCanvasModifiersDraw::StartThread: Thread started");
}

void RSCanvasModifiersDraw::WaitAllTasksFinish()
{
    if (!threadStarted_.load()) {
        return;
    }
    RemoveTask(CLEAN_FREE_BUFFERS_TASK_NAME);
    PostSyncTask([canvasModifiersDraw = shared_from_this()]() {
        RS_TRACE_NAME_FMT("RSCanvasModifiersDraw::WaitAllTasksFinish");
        if (canvasModifiersDraw->gpuContext_ != nullptr) {
            canvasModifiersDraw->gpuContext_->FlushAndSubmit(false);
            canvasModifiersDraw->gpuContext_->PurgeUnlockedResources(true);
            canvasModifiersDraw->gpuContext_ = nullptr;
        }
        canvasModifiersDraw->drawableMap_.clear();
        canvasModifiersDraw->transactionConfigList_.clear();
    });
}

void RSCanvasModifiersDraw::Destroy()
{
    threadDestroyed_.store(true);
    if (!threadStarted_.load()) {
        return;
    }
    threadStarted_.store(false);

    if (handler_ != nullptr) {
        handler_->RemoveAllEvents();
    }
    if (runner_ != nullptr) {
        runner_->Stop();
    }
    runner_ = nullptr;
    handler_ = nullptr;
}

void RSCanvasModifiersDraw::PostTask(const std::function<void()>& task, const std::string& name, int64_t delayTime)
{
    if (threadDestroyed_.load()) {
        return;
    }
    if (!threadStarted_.load()) {
        StartThread();
    }
    if (handler_ != nullptr) {
        handler_->PostTask(task, name, delayTime, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}

void RSCanvasModifiersDraw::PostSyncTask(const std::function<void()>& task)
{
    if (threadDestroyed_.load()) {
        return;
    }
    if (!threadStarted_.load()) {
        StartThread();
    }
    if (handler_ != nullptr) {
        handler_->PostSyncTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}

void RSCanvasModifiersDraw::RemoveTask(const std::string& name)
{
    if (threadStarted_.load()) {
        handler_->RemoveTask(name);
    }
}
// End of thread-related methods

std::shared_ptr<Drawing::GPUContext> RSCanvasModifiersDraw::GetGpuContext()
{
    if (gpuContext_ == nullptr) {
        if (renderContext_ == nullptr) {
            renderContext_ = RenderContext::Create();
            renderContext_->Init(RenderEngineType::BASIC_RENDER, cacheDir_);
        }
        gpuContext_ = renderContext_->GetSharedDrGPUContext();
    }
    return gpuContext_;
}

void RSCanvasModifiersDraw::SetCacheDir(const std::string& cacheDir)
{
    std::weak_ptr<RSCanvasModifiersDraw> weakCanvasModifiersDraw = shared_from_this();
    PostTask([weakCanvasModifiersDraw, path = cacheDir] {
        if (auto canvasModifiersDraw = weakCanvasModifiersDraw.lock()) {
            ShaderCache::Instance().SetFilePath(path);
            canvasModifiersDraw->cacheDir_ = path;
        }
    });
}

void RSCanvasModifiersDraw::OnNodeCreate(NodeId nodeId, std::weak_ptr<RSRenderInterface> weakRenderInterface)
{
    std::weak_ptr<RSCanvasModifiersDraw> weakCanvasModifiersDraw = shared_from_this();
    PostTask([weakCanvasModifiersDraw, nodeId, weakRenderInterface] {
        if (auto canvasModifiersDraw = weakCanvasModifiersDraw.lock()) {
            auto& drawable = canvasModifiersDraw->drawableMap_[nodeId];
            drawable.nodeId_ = nodeId;
            auto gpuContext = canvasModifiersDraw->GetGpuContext();
            drawable.renderContext_ = canvasModifiersDraw->renderContext_;
            drawable.CreateProducerSurface(weakRenderInterface, gpuContext, maxGpuResourceBytes_);
        }
    });
}

void RSCanvasModifiersDraw::OnNodeRelease(NodeId nodeId, std::weak_ptr<RSRenderInterface> weakRenderInterface)
{
    std::weak_ptr<RSCanvasModifiersDraw> weakCanvasModifiersDraw = shared_from_this();
    PostTask([weakCanvasModifiersDraw, nodeId, weakRenderInterface] {
        if (auto canvasModifiersDraw = weakCanvasModifiersDraw.lock()) {
            auto& drawable = canvasModifiersDraw->drawableMap_[nodeId];
            drawable.ReleaseProducerSurface(weakRenderInterface);
            canvasModifiersDraw->drawableMap_.erase(nodeId);
        }
    });
}

void RSCanvasModifiersDraw::OnNodeStateChanged(NodeId nodeId, RSNodeState nodeState)
{
    std::weak_ptr<RSCanvasModifiersDraw> weakCanvasModifiersDraw = shared_from_this();
    PostTask([weakCanvasModifiersDraw, nodeId, nodeState] {
        auto canvasModifiersDraw = weakCanvasModifiersDraw.lock();
        if (canvasModifiersDraw == nullptr) {
            return;
        }
        auto& drawable = canvasModifiersDraw->drawableMap_[nodeId];
        drawable.nodeState_ = nodeState;
        if (nodeState != RSNodeState::ACTIVE) {
            canvasModifiersDraw->CleanFreeBuffersImmediately();
            return;
        }
        drawable.UpdateContent(nullptr, true);
    });
}

void RSCanvasModifiersDraw::ResetSurface(
    NodeId nodeId, int width, int height, bool sizeOutOfGpuLimit, GraphicColorGamut colorSpace)
{
    std::weak_ptr<RSCanvasModifiersDraw> weakCanvasModifiersDraw = shared_from_this();
    PostTask([weakCanvasModifiersDraw, nodeId, width, height, sizeOutOfGpuLimit, colorSpace] {
        auto canvasModifiersDraw = weakCanvasModifiersDraw.lock();
        if (canvasModifiersDraw == nullptr) {
            return;
        }
        auto& drawable = canvasModifiersDraw->drawableMap_[nodeId];
        drawable.ResetSurface(width, height, sizeOutOfGpuLimit, colorSpace);
        if (sizeOutOfGpuLimit) {
            canvasModifiersDraw->CleanFreeBuffersImmediately();
        }
    });
}

bool RSCanvasModifiersDraw::GetBitmap(NodeId nodeId, Drawing::Bitmap& bitmap)
{
    bool ret = false;
    std::weak_ptr<RSCanvasModifiersDraw> weakCanvasModifiersDraw = shared_from_this();
    PostSyncTask([weakCanvasModifiersDraw, nodeId, &bitmap, &ret] {
        if (auto canvasModifiersDraw = weakCanvasModifiersDraw.lock()) {
            auto& drawable = canvasModifiersDraw->drawableMap_[nodeId];
            ret = drawable.GetBitmap(bitmap, canvasModifiersDraw->GetGpuContext());
        }
    });
    return ret;
}

bool RSCanvasModifiersDraw::GetPixelMap(NodeId nodeId, std::shared_ptr<Media::PixelMap> pixelMap,
    const Drawing::Rect* rect, Drawing::DrawCmdListPtr drawCmdList)
{
    bool ret = false;
    std::weak_ptr<RSCanvasModifiersDraw> weakCanvasModifiersDraw = shared_from_this();
    PostSyncTask([weakCanvasModifiersDraw, nodeId, pixelMap, rect, drawCmdList, &ret] {
        if (auto canvasModifiersDraw = weakCanvasModifiersDraw.lock()) {
            auto& drawable = canvasModifiersDraw->drawableMap_[nodeId];
            ret = drawable.GetPixelMap(pixelMap, rect, drawCmdList, canvasModifiersDraw->GetGpuContext());
        }
    });
    return ret;
}

void RSCanvasModifiersDraw::UpdateCanvasContent(NodeId nodeId, Drawing::DrawCmdListPtr drawCmdList)
{
    std::weak_ptr<RSCanvasModifiersDraw> weakCanvasModifiersDraw = shared_from_this();
    PostTask([weakCanvasModifiersDraw, nodeId, drawCmdList] {
        auto canvasModifiersDraw = weakCanvasModifiersDraw.lock();
        if (canvasModifiersDraw == nullptr) {
            return;
        }
        if (canvasModifiersDraw->needRestoreGpuCacheLimit_) {
            if (auto gpuContext = canvasModifiersDraw->GetGpuContext()) {
                gpuContext->SetResourceCacheLimits(0, maxGpuResourceBytes_);
                canvasModifiersDraw->needRestoreGpuCacheLimit_ = false;
            }
        }
        auto& drawable = canvasModifiersDraw->drawableMap_[nodeId];
        drawable.UpdateContent(drawCmdList, false);
        canvasModifiersDraw->lastUpdateCanvasContentTime_ = static_cast<int64_t>(
            std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch())
                .count());
    });
}

DestroySemaphoreInfo* RSCanvasModifiersDraw::FlushSurfaceWithSemaphore(
    VkSemaphore& semaphore, std::shared_ptr<Drawing::Surface> drawingSurface)
{
    if (renderContext_ == nullptr) {
        RS_LOGE("RSCanvasModifiersDraw::FlushSurfaceWithSemaphore renderContext_ is nullptr");
        return nullptr;
    }
    auto vkInterface = RsVulkanContext::Get(renderContext_->GetType()).GetRsVulkanInterface();
    if (NativeBufferUtils::CreateVkSemaphore(vkInterface, semaphore) != VK_SUCCESS) {
        RS_LOGE("RSCanvasModifiersDraw::FlushSurfaceWithSemaphore: CreateVkSemaphore fail");
        semaphore = VK_NULL_HANDLE;
        return nullptr;
    }
#ifdef USE_M133_SKIA
    GrBackendSemaphore backendSemaphore = GrBackendSemaphores::MakeVk(semaphore);
#else
    GrBackendSemaphore backendSemaphore;
    backendSemaphore.initVulkan(semaphore);
#endif
    DestroySemaphoreInfo* destroySemaphoreInfo =
        new DestroySemaphoreInfo(vkInterface->vkDestroySemaphore, vkInterface->GetDevice(), semaphore);
    Drawing::FlushInfo drawingFlushInfo;
    drawingFlushInfo.backendSurfaceAccess = true;
    drawingFlushInfo.numSemaphores = 1;
    drawingFlushInfo.backendSemaphore = static_cast<void*>(&backendSemaphore);
    drawingFlushInfo.finishedProc = [](void* context) { DestroySemaphoreInfo::DestroySemaphore(context); };
    drawingFlushInfo.finishedContext = destroySemaphoreInfo;
    drawingSurface->Flush(&drawingFlushInfo);
    return destroySemaphoreInfo;
}

int32_t RSCanvasModifiersDraw::GetFenceFd(VkSemaphore& semaphore)
{
    int32_t fenceFd = INVALID_FD;
    if (renderContext_ == nullptr) {
        RS_LOGE("RSCanvasModifiersDraw::GetFenceFd renderContext_ is nullptr");
        return fenceFd;
    }
    if (semaphore != VK_NULL_HANDLE) {
        NativeBufferUtils::GetFenceFdFromSemaphore(
            RsVulkanContext::Get(renderContext_->GetType()).GetRsVulkanInterface(), semaphore, fenceFd);
    }
    return fenceFd;
}

void RSCanvasModifiersDraw::SubmitAndCollectCanvasBuffers()
{
    std::weak_ptr<RSCanvasModifiersDraw> weakCanvasModifiersDraw = shared_from_this();
    PostTask([weakCanvasModifiersDraw] {
        auto canvasModifiersDraw = weakCanvasModifiersDraw.lock();
        if (canvasModifiersDraw == nullptr) {
            return;
        }
        auto gpuContext = canvasModifiersDraw->GetGpuContext();
        if (gpuContext == nullptr) {
            RS_LOGE("RSCanvasModifiersDraw::SubmitAndCollectCanvasBuffers, null gpuContext");
            return;
        }

        bool needFlushBuffer = false;
        for (auto& [_, drawable] : canvasModifiersDraw->drawableMap_) {
            if (drawable.nodeState_ == RSNodeState::ACTIVE) {
                needFlushBuffer = true;
                break;
            }
        }
        std::vector<std::pair<sptr<SurfaceBuffer>, RSCanvasModifiersDrawable*>> bufferList;
        for (auto& [_, drawable] : canvasModifiersDraw->drawableMap_) {
            if (auto buffer = drawable.GetBuffer(needFlushBuffer)) {
                bufferList.emplace_back(buffer, &drawable);
            }
        }
        if (bufferList.empty()) {
            return;
        }
        if (!needFlushBuffer) {
            gpuContext->Submit();
            return;
        }
        VkSemaphore semaphore = VK_NULL_HANDLE;
        DestroySemaphoreInfo* destroySemaphoreInfo =
            canvasModifiersDraw->FlushSurfaceWithSemaphore(semaphore, bufferList.back().second->drawingSurface_);
        gpuContext->Submit();
        auto fenceFd = canvasModifiersDraw->GetFenceFd(semaphore);
        auto now = static_cast<int64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count());
        for (const auto& [buffer, drawable] : bufferList) {
            canvasModifiersDraw->AppendTransactionConfig(drawable->nodeId_, buffer, fenceFd);
            drawable->OnDirtyBufferCollected(now);
        }
        if (destroySemaphoreInfo != nullptr) {
            DestroySemaphoreInfo::DestroySemaphore(destroySemaphoreInfo);
        }
    });
}

void RSCanvasModifiersDraw::AppendTransactionConfig(NodeId nodeId, sptr<SurfaceBuffer> buffer, int fenceFd)
{
    RSTransactionConfig config;
    config.nodeId = nodeId;
    config.transaction = new RSBufferTransaction(buffer);
    sptr<SyncFence> fence = new SyncFence(fenceFd);
    config.transaction->SetFence(fence);
    std::vector<Rect> damages{Rect{0, 0, buffer->GetWidth(), buffer->GetHeight()}};
    config.transaction->SetDamages(damages);
    transactionConfigList_.emplace_back(config);
}

void RSCanvasModifiersDraw::SwapTransactionConfigList(std::vector<RSTransactionConfig>& transactionConfigList)
{
    std::weak_ptr<RSCanvasModifiersDraw> weakCanvasModifiersDraw = shared_from_this();
    PostSyncTask([weakCanvasModifiersDraw, &transactionConfigList] {
        if (auto canvasModifiersDraw = weakCanvasModifiersDraw.lock()) {
            std::swap(transactionConfigList, canvasModifiersDraw->transactionConfigList_);
        }
    });
}

void RSCanvasModifiersDraw::CleanFreeBuffers(int64_t delayTime, bool immediately)
{
    if (immediately) {
        RemoveTask(CLEAN_FREE_BUFFERS_TASK_NAME);
    }
    std::weak_ptr<RSCanvasModifiersDraw> weakCanvasModifiersDraw = shared_from_this();
    PostTask(
        [weakCanvasModifiersDraw, delayTime, immediately] {
            if (auto canvasModifiersDraw = weakCanvasModifiersDraw.lock()) {
                canvasModifiersDraw->DoCleanFreeBuffers(immediately ? 0 : delayTime);
                canvasModifiersDraw->CleanFreeBuffers(CLEAN_FREE_BUFFERS_DURATION, false);
            }
        },
        CLEAN_FREE_BUFFERS_TASK_NAME, delayTime);
}

void RSCanvasModifiersDraw::CleanFreeBuffersImmediately()
{
    CleanFreeBuffers(CLEAN_FREE_BUFFERS_IMMEDIATELY_DELAY, true);
}

void RSCanvasModifiersDraw::DoCleanFreeBuffers(int64_t maxDuration)
{
    auto now = static_cast<int64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count());
    std::vector<RSCanvasModifiersDrawable*> freeDrawableList;
    for (auto& [_, drawable] : drawableMap_) {
        if (drawable.IsFree(now, maxDuration)) {
            freeDrawableList.emplace_back(&drawable);
        }
    }
    if (freeDrawableList.empty()) {
        return;
    }
    for (auto* drawable : freeDrawableList) {
        drawable->CleanBuffer();
    }
    if (gpuContext_ != nullptr && now - lastUpdateCanvasContentTime_ > CLEAN_FREE_BUFFERS_DURATION) {
        gpuContext_->SetResourceCacheLimits(0, 0);
        needRestoreGpuCacheLimit_ = true;
    }
}
} // namespace Rosen
} // namespace OHOS