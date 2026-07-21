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
#include "platform/ohos/backend/native_buffer_utils.h"
#include "platform/ohos/backend/rs_surface_ohos_vulkan.h"
#include "render_context/render_context.h"
#include "render_context/shader_cache.h"

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
    drawingSurface_ = nullptr;
    surfaceFrame_ = nullptr;
    semaphore_ = VK_NULL_HANDLE;
    if (drawCmdListCache_ != nullptr) {
        drawCmdListCache_->clear();
    }
}

void RSCanvasModifiersDrawable::CreateProducerSurface(
    std::weak_ptr<RSRenderInterface> weakRenderInterface, const std::string& cacheDir)
{
    auto renderInterface = weakRenderInterface.lock();
    if (renderInterface == nullptr) {
        RS_LOGE(
            "RSCanvasModifiersDrawable::CreateProducerSurface, null renderInterface, nodeId=%{public}" PRIu64, nodeId_);
        return;
    }
    auto gpuContext = RsVulkanContext::GetSingleton().GetRecyclableDrawingContext(cacheDir);
    if (gpuContext == nullptr) {
        RS_LOGE("RSCanvasModifiersDrawable::CreateProducerSurface, null gpuContext, nodeId=%{public}" PRIu64, nodeId_);
        return;
    }

    auto surface = renderInterface->CreateCanvasDrawingNodeSurface(nodeId_);
    if (surface == nullptr) {
        RS_LOGE("RSCanvasModifiersDrawable::CreateProducerSurface, null surface, nodeId=%{public}" PRIu64, nodeId_);
        return;
    }
    auto producerSurface = std::make_shared<RSSurfaceOhosVulkan>(surface);
    producerSurface->SetSkContext(gpuContext);
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

DestroySemaphoreInfo* RSCanvasModifiersDrawable::ResetSurface(
    int width, int height, bool sizeOutOfGpuLimit, GraphicColorGamut colorSpace)
{
    if (producerSurface_ == nullptr) {
        RS_LOGE("RSCanvasModifiersDrawable::ResetSurface: Null producer surface, nodeId=%{public}" PRIu64, nodeId_);
        return nullptr;
    }
    if (width_ == width && height_ == height) {
        RS_LOGE("RSCanvasModifiersDrawable::ResetSurface: Same width and height, nodeId=%{public}" PRIu64, nodeId_);
        return nullptr;
    }

    auto ohosSurface = std::static_pointer_cast<RSSurfaceOhosVulkan>(producerSurface_);
    // Current buffer is not flushed, cancel it.
    if (surfaceFrame_ != nullptr) {
        ohosSurface->CancelBufferForCurrentFrame();
    }

    Reset();
    if (sizeOutOfGpuLimit) {
        return nullptr;
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
    return Draw();
}

DestroySemaphoreInfo* RSCanvasModifiersDrawable::UpdateContent(
    Drawing::DrawCmdListPtr drawCmdList, bool forceFlushBuffer)
{
    if (drawCmdListCache_ == nullptr) {
        RS_LOGE("RSCanvasModifiersDrawable::UpdateContent: Null drawCmdListCache, drop drawCmdList, "
            "nodeId=%{public}" PRIu64, nodeId_);
        return nullptr;
    }

    if (drawCmdList != nullptr) {
        drawCmdListCache_->emplace_back(drawCmdList);
    }
    forceFlushBuffer_ = forceFlushBuffer;
    return Draw();
}

DestroySemaphoreInfo* RSCanvasModifiersDrawable::Draw()
{
    if (producerSurface_ == nullptr) {
        RS_LOGE("RSCanvasModifiersDrawable::Draw: Null producer surface, nodeId=%{public}" PRIu64, nodeId_);
        return nullptr;
    }
    if (drawCmdListCache_->empty() && !forceFlushBuffer_) {
        return nullptr;
    }

    if (surfaceFrame_ == nullptr) {
        surfaceFrame_ = RequestBufferAndDrawHistory();
    }
    if (surfaceFrame_ == nullptr) {
        RS_LOGE("RSCanvasModifiersDrawable::Draw: Null surfaceFrame, nodeId=%{public}" PRIu64, nodeId_);
        return nullptr;
    }
    for (auto& cmdList : *drawCmdListCache_) {
        Playback(cmdList);
        cmdList->ClearOp();
    }
    forceFlushBuffer_ = false;
    NativeBufferUtils::CreateVkSemaphore(semaphore_);
    return FlushSurfaceWithSemaphore();
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

DestroySemaphoreInfo* RSCanvasModifiersDrawable::FlushSurfaceWithSemaphore()
{
#ifdef USE_M133_SKIA
    GrBackendSemaphore backendSemaphore = GrBackendSemaphores::MakeVk(semaphore_);
#else
    GrBackendSemaphore backendSemaphore;
    backendSemaphore.initVulkan(semaphore_);
#endif
    auto& vkContext = RsVulkanContext::GetSingleton().GetRsVulkanInterface();
    DestroySemaphoreInfo* destroyInfo =
        new DestroySemaphoreInfo(vkContext.vkDestroySemaphore, vkContext.GetDevice(), semaphore_);
    Drawing::FlushInfo drawingFlushInfo;
    drawingFlushInfo.backendSurfaceAccess = true;
    drawingFlushInfo.numSemaphores = 1;
    drawingFlushInfo.backendSemaphore = static_cast<void*>(&backendSemaphore);
    drawingFlushInfo.finishedProc = [](void* context) { DestroySemaphoreInfo::DestroySemaphore(context); };
    drawingFlushInfo.finishedContext = destroyInfo;
    drawingSurface_->Flush(&drawingFlushInfo);
    return destroyInfo;
}

sptr<SurfaceBuffer> RSCanvasModifiersDrawable::OnFlushBuffer()
{
    if (nodeState_ == RSNodeState::INACTIVE) {
        return nullptr;
    }
    if (producerSurface_ == nullptr) {
        RS_LOGE("RSCanvasModifiersDrawable::OnFlushBuffer: Null producer surface, nodeId=%{public}" PRIu64, nodeId_);
        return nullptr;
    }
    if (surfaceFrame_ == nullptr || semaphore_ == VK_NULL_HANDLE) {
        return nullptr;
    }
    auto ohosSurface = std::static_pointer_cast<RSSurfaceOhosVulkan>(producerSurface_);
    if (auto buffer = ohosSurface->GetCurrentBuffer()) {
        ohosSurface->OnFlushBuffer();
        return buffer;
    }
    return nullptr;
}

void RSCanvasModifiersDrawable::OnDirtyBufferCollected(int64_t lastFlushBufferTime)
{
    surfaceFrame_ = nullptr;
    semaphore_ = VK_NULL_HANDLE;
    lastFlushBufferTime_ = lastFlushBufferTime;
}

int32_t RSCanvasModifiersDrawable::GetFenceFd()
{
    int32_t fenceFd = 0;
    NativeBufferUtils::GetFenceFdFromSemaphore(semaphore_, fenceFd);
    return fenceFd;
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
    Drawing::DrawCmdListPtr drawCmdList, const std::string& cacheDir)
{
    if (pixelMap == nullptr || rect == nullptr) {
        return false;
    }
    auto bitmapFormat = Drawing::BitmapFormat { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    auto gpuContext = RsVulkanContext::GetSingleton().GetRecyclableDrawingContext(cacheDir);
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

bool RSCanvasModifiersDrawable::GetBitmap(Drawing::Bitmap& bitmap, const std::string& cacheDir)
{
    auto bitmapFormat = Drawing::BitmapFormat { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    auto gpuContext = RsVulkanContext::GetSingleton().GetRecyclableDrawingContext(cacheDir);
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

    runner_ = AppExecFwk::EventRunner::Create("CanvasModifiersDraw");
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
    runner_->Run();
    threadStarted_.store(true);
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
    PostSyncTask([canvasModifiersDraw = shared_from_this()]() {
        RS_TRACE_NAME_FMT("RSCanvasModifiersDraw::WaitAllTasksFinish");
        if (auto gpuContext =
                RsVulkanContext::GetSingleton().GetRecyclableDrawingContext(canvasModifiersDraw->cacheDir_)) {
            gpuContext->FlushAndSubmit(true);
            gpuContext->PurgeUnlockedResources(true);
            canvasModifiersDraw->drawableMap_.clear();
            canvasModifiersDraw->canvasNewSemaphoreInfos_.clear();
            canvasModifiersDraw->canvasExpiredSemaphoreInfos_.clear();
            canvasModifiersDraw->transactionConfigList_.clear();
        }
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
        handler_ = nullptr;
    }
    if (runner_ != nullptr) {
        runner_->Stop();
        runner_ = nullptr;
    }
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

void RSCanvasModifiersDraw::QueryMaxGpuBufferSize(uint32_t& maxWidth, uint32_t& maxHeight)
{
    PostSyncTask([&maxWidth, &maxHeight] {
        RsVulkanContext::SetRecyclable(true);
        RenderContext::Create()->QueryMaxGpuBufferSize(maxWidth, maxHeight);
    });
}

void RSCanvasModifiersDraw::OnNodeCreate(NodeId nodeId, std::weak_ptr<RSRenderInterface> weakRenderInterface)
{
    std::weak_ptr<RSCanvasModifiersDraw> weakCanvasModifiersDraw = shared_from_this();
    PostTask([weakCanvasModifiersDraw, nodeId, weakRenderInterface] {
        if (auto canvasModifiersDraw = weakCanvasModifiersDraw.lock()) {
            auto& drawable = canvasModifiersDraw->drawableMap_[nodeId];
            drawable.nodeId_ = nodeId;
            drawable.CreateProducerSurface(weakRenderInterface, canvasModifiersDraw->cacheDir_);
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
        if (auto* destroySemaphoreInfo = drawable.UpdateContent(nullptr, true)) {
            canvasModifiersDraw->canvasNewSemaphoreInfos_.emplace_back(destroySemaphoreInfo);
        }
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
        if (auto* destroySemaphoreInfo = drawable.ResetSurface(width, height, sizeOutOfGpuLimit, colorSpace)) {
            canvasModifiersDraw->canvasNewSemaphoreInfos_.emplace_back(destroySemaphoreInfo);
        }
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
            ret = drawable.GetBitmap(bitmap, canvasModifiersDraw->cacheDir_);
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
            ret = drawable.GetPixelMap(pixelMap, rect, drawCmdList, canvasModifiersDraw->cacheDir_);
        }
    });
    return ret;
}

void RSCanvasModifiersDraw::UpdateCanvasContent(NodeId nodeId, Drawing::DrawCmdListPtr drawCmdList)
{
    std::weak_ptr<RSCanvasModifiersDraw> weakCanvasModifiersDraw = shared_from_this();
    PostTask([weakCanvasModifiersDraw, nodeId, drawCmdList] {
        if (auto canvasModifiersDraw = weakCanvasModifiersDraw.lock()) {
            auto& drawable = canvasModifiersDraw->drawableMap_[nodeId];
            if (auto* destroySemaphoreInfo = drawable.UpdateContent(drawCmdList, false)) {
                canvasModifiersDraw->canvasNewSemaphoreInfos_.emplace_back(destroySemaphoreInfo);
            }
        }
    });
}

void RSCanvasModifiersDraw::SubmitAndCollectCanvasBuffers()
{
    std::weak_ptr<RSCanvasModifiersDraw> weakCanvasModifiersDraw = shared_from_this();
    PostTask([weakCanvasModifiersDraw] {
        auto canvasModifiersDraw = weakCanvasModifiersDraw.lock();
        if (canvasModifiersDraw == nullptr) {
            return;
        }

        std::vector<std::pair<sptr<SurfaceBuffer>, RSCanvasModifiersDrawable*>> bufferList;
        for (auto& [_, drawable] : canvasModifiersDraw->drawableMap_) {
            if (auto buffer = drawable.OnFlushBuffer()) {
                bufferList.emplace_back(buffer, &drawable);
            }
        }
        if (!bufferList.empty()) {
            if (auto gpuContext =
                    RsVulkanContext::GetSingleton().GetRecyclableDrawingContext(canvasModifiersDraw->cacheDir_)) {
                gpuContext->Submit();
            }
            auto now = static_cast<int64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now().time_since_epoch()).count());
            for (const auto& [buffer, drawable] : bufferList) {
                canvasModifiersDraw->AppendTransactionConfig(drawable->nodeId_, buffer, drawable->GetFenceFd());
                drawable->OnDirtyBufferCollected(now);
            }
            canvasModifiersDraw->DestroyCanvasSemaphore();
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

void RSCanvasModifiersDraw::DestroyCanvasSemaphore()
{
    for (auto& semaphoreInfo: canvasExpiredSemaphoreInfos_) {
        if (!semaphoreInfo) {
            continue;
        }
        DestroySemaphoreInfo::DestroySemaphore(semaphoreInfo);
    }
    canvasExpiredSemaphoreInfos_.clear();
    std::swap(canvasNewSemaphoreInfos_, canvasExpiredSemaphoreInfos_);
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
    if (auto gpuContext = RsVulkanContext::GetSingleton().GetRecyclableDrawingContext(cacheDir_)) {
        gpuContext->FlushAndSubmit(true);
    }
    for (auto* drawable : freeDrawableList) {
        drawable->CleanBuffer();
    }
}
} // namespace Rosen
} // namespace OHOS