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
#include "sandbox_utils.h"

#include "command/rs_delegate_composite_command.h"
#include "platform/common/rs_log.h"
#include "platform/ohos/backend/rs_surface_ohos_vulkan.h"
#include "render_context/render_context.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr const char* CLEAN_FREE_BUFFERS_TASK_NAME = "CleanFreeBuffersTask";
constexpr int64_t CLEAN_FREE_BUFFERS_DURATION = 2000;
constexpr int64_t CLEAN_FREE_BUFFERS_IMMEDIATELY_DELAY = 100;
}

// RSCanvasModifiersContext Start
RSCanvasModifiersContext::RSCanvasModifiersContext(std::weak_ptr<RSRenderInterface> weakRenderInterface)
    : weakRenderInterface_(weakRenderInterface)
{}

std::shared_ptr<Drawing::GPUContext> RSCanvasModifiersContext::GetGpuContext()
{
    if (renderContext_ == nullptr) {
        renderContext_ = RenderContext::Create();
        renderContext_->Init(RenderEngineType::BASIC_RENDER, cacheDir_);
        gpuContext_ = renderContext_->GetSharedDrGPUContext();
    }
    return gpuContext_;
}

void RSCanvasModifiersContext::ReleaseGpuContext()
{
    if (gpuContext_ != nullptr) {
        gpuContext_->FlushAndSubmit(false);
        gpuContext_->PurgeUnlockedResources(true);
        gpuContext_ = nullptr;
        renderContext_ = nullptr;
    }
}

void RSCanvasModifiersContext::CleanGpuCache()
{
    if (gpuContext_ != nullptr) {
        gpuContext_->SetResourceCacheLimits(0, 0);
        gpuContext_->PerformDeferredCleanup(std::chrono::milliseconds(0));
        std::set<pid_t> exitedPidSet;
        gpuContext_->PurgeUnlockedResourcesByPid(true, exitedPidSet);
        needRestoreGpuCacheLimit_ = true;
    }
}

void RSCanvasModifiersContext::GetGpuCacheLimits()
{
    if (gpuContext_ != nullptr) {
        gpuContext_->GetResourceCacheLimits(nullptr, &maxGpuResourceBytes_);
    }
}

void RSCanvasModifiersContext::RestoreGpuCacheLimits()
{
    if (!needRestoreGpuCacheLimit_) {
        return;
    }
    if (gpuContext_ != nullptr) {
        gpuContext_->SetResourceCacheLimits(0, maxGpuResourceBytes_);
        needRestoreGpuCacheLimit_ = false;
    }
}
// RSCanvasModifiersContext End

// RSCanvasModifiersDrawable Start
void RSCanvasModifiersDrawable::Reset()
{
    width_ = 0;
    height_ = 0;
    drawingSurface_ = nullptr;
    bufferRequested_ = false;
    bufferDirty_ = false;
    if (drawCmdListCache_ != nullptr) {
        drawCmdListCache_->clear();
    }
}

void RSCanvasModifiersDrawable::CreateProducerSurface(
    std::shared_ptr<RSCanvasModifiersContext> canvasModifiersContext, GraphicColorGamut colorSpace)
{
    auto renderInterface = canvasModifiersContext->weakRenderInterface_.lock();
    if (renderInterface == nullptr) {
        RS_LOGE(
            "RSCanvasModifiersDrawable::CreateProducerSurface, null renderInterface, nodeId=%{public}" PRIu64, nodeId_);
        return;
    }

    auto surface = renderInterface->CreateCanvasDrawingNodeSurface(nodeId_);
    if (surface == nullptr) {
        RS_LOGE("RSCanvasModifiersDrawable::CreateProducerSurface, null surface, nodeId=%{public}" PRIu64, nodeId_);
        return;
    }

    static std::once_flag flag;
    std::call_once(flag, [canvasModifiersContext]() { canvasModifiersContext->GetGpuCacheLimits(); });
    auto producerSurface = std::make_shared<RSSurfaceOhosVulkan>(surface);
    producerSurface->SetRenderContext(canvasModifiersContext->renderContext_);
    producerSurface->SetTimeOut(2); // Timeout 2ms
    producerSurface_ = producerSurface;
    drawCmdListCache_ = std::make_unique<std::vector<Drawing::DrawCmdListPtr>>();

    // Set ColorSpace
    producerSurface->SetColorSpace(colorSpace);
    HDIV::CM_ColorSpaceType colorSpaceType = HDIV::CM_SRGB_FULL;
    if (RSColorSpaceUtil::ConvertColorGamutToSpaceType(colorSpace, colorSpaceType)) {
        // ATTRKEY_COLORSPACE_INFO is color space key
        auto ret = surface->SetUserData("ATTRKEY_COLORSPACE_INFO", std::to_string(colorSpaceType));
        if (ret != GSERROR_OK) {
            RS_LOGE("RSCanvasModifiersDrawable::CreateProducerSurface: SetColorSpace fail, nodeId=%{public}" PRIu64
                ", colorSpace=%{public}d, ret=%{public}d", nodeId_, colorSpace, ret);
        }
    } else {
        RS_LOGE("RSCanvasModifiersDrawable::CreateProducerSurface: ConvertColorGamut fail, nodeId=%{public}" PRIu64
            ", colorSpace=%{public}d", nodeId_, colorSpace);
    }
}

void RSCanvasModifiersDrawable::ReleaseProducerSurface(std::shared_ptr<RSCanvasModifiersContext> canvasModifiersContext)
{
    auto renderInterface = canvasModifiersContext->weakRenderInterface_.lock();
    if (renderInterface == nullptr) {
        RS_LOGE("RSCanvasModifiersDrawable::ReleaseProducerSurface, null renderInterface, nodeId=%{public}" PRIu64,
            nodeId_);
        return;
    }
    renderInterface->ReleaseCanvasDrawingNodeSurface(nodeId_);
}

void RSCanvasModifiersDrawable::ResetSurface(int width, int height, bool sizeOutOfGpuLimit)
{
    if (producerSurface_ == nullptr) {
        RS_LOGE("RSCanvasModifiersDrawable::ResetSurface: Null producer surface, nodeId=%{public}" PRIu64, nodeId_);
        return;
    }
    if (width_ == width && height_ == height) {
        RS_LOGE("RSCanvasModifiersDrawable::ResetSurface: Same width and height, nodeId=%{public}" PRIu64, nodeId_);
        return;
    }
    if (width <= 0 || height <= 0) {
        RS_LOGE("RSCanvasModifiersDrawable::ResetSurface: Invalid width or height, nodeId=%{public}" PRIu64
            ", width=%{public}d, height=%{public}d", nodeId_, width, height);
        return;
    }

    auto ohosSurface = std::static_pointer_cast<RSSurfaceOhosVulkan>(producerSurface_);
    // Current buffer is not flushed, cancel it.
    if (bufferRequested_) {
        ohosSurface->CancelBufferForCurrentFrame();
    }

    Reset();
    lastBufferExpiredTime_ = static_cast<int64_t>(
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch())
            .count());
    if (sizeOutOfGpuLimit) {
        return;
    }

    width_ = width;
    height_ = height;
    if (hasDrawn_) {
        RequestBufferAndDrawHistory(width, height, true);
    }
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
    Draw(forceFlushBuffer);
}

void RSCanvasModifiersDrawable::Draw(bool forceFlushBuffer)
{
    RS_OPTIONAL_TRACE_NAME_FMT("Draw content, nodeId=%" PRIu64, nodeId_);
    if (producerSurface_ == nullptr) {
        RS_LOGE("RSCanvasModifiersDrawable::Draw: Null producer surface, nodeId=%{public}" PRIu64, nodeId_);
        return;
    }
    if (drawCmdListCache_->empty() && !forceFlushBuffer) {
        RS_TRACE_NAME_FMT("drawCmdListCache empty, nodeId=%" PRIu64, nodeId_);
        return;
    }

    if (!bufferRequested_ && !RequestBufferAndDrawHistory(width_, height_, false)) {
        return;
    }
    for (auto& cmdList : *drawCmdListCache_) {
        cmdList->FlushImageCache();
        Playback(cmdList);
        cmdList->ClearOp();
    }
    drawCmdListCache_->clear();
    bufferDirty_ = true;
    hasDrawn_ = true;
}

bool RSCanvasModifiersDrawable::RequestBufferAndDrawHistory(int width, int height, bool needResetCanvas)
{
    auto ohosSurface = std::static_pointer_cast<RSSurfaceOhos>(producerSurface_);
    auto renderFrame = ohosSurface->RequestFrame(width, height);
    if (renderFrame == nullptr) {
        RS_LOGE("RSCanvasModifiersDrawable::RequestBufferAndDrawHistory: Null renderFrame, nodeId=%{public}" PRIu64
            ", width=%{public}d, height=%{public}d", nodeId_, width, height);
        return false;
    }
    auto drawingSurface = renderFrame->GetSurface();
    if (drawingSurface == nullptr) {
        RS_LOGE("RSCanvasModifiersDrawable::RequestBufferAndDrawHistory: Null drawingSurface, nodeId=%{public}" PRIu64,
            nodeId_);
        return false;
    }
    auto canvas = drawingSurface->GetCanvas();
    if (canvas == nullptr) {
        RS_LOGE(
            "RSCanvasModifiersDrawable::RequestBufferAndDrawHistory: Null canvas, nodeId=%{public}" PRIu64, nodeId_);
        return false;
    }

    RS_OPTIONAL_TRACE_NAME_FMT("Buffer requested, nodeId=%" PRIu64 ", width=%d, height=%d", nodeId_, width, height);
    if (needResetCanvas) {
        RS_OPTIONAL_TRACE_NAME_FMT("Reset canvas, nodeId=%" PRIu64, nodeId_);
        canvas->Clear(Drawing::Color::COLOR_TRANSPARENT);
        canvas->RestoreToCount(1);
        canvas->SetMatrix(Drawing::Matrix());
    } else if (drawingSurface_ != nullptr && drawingSurface_ != drawingSurface) {
        if (auto lastCanvas = drawingSurface_->GetCanvas()) {
            canvas->InheritStateAndContentFrom(lastCanvas.get(), false);
        }
    }
    drawingSurface_ = drawingSurface;
    bufferRequested_ = true;
    return true;
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
    if (!bufferDirty_) {
        RS_OPTIONAL_TRACE_NAME_FMT(
            "Buffer not dirty, nodeId=%" PRIu64 ", width=%d, height=%d", nodeId_, width_, height_);
        return nullptr;
    }
    auto ohosSurface = std::static_pointer_cast<RSSurfaceOhosVulkan>(producerSurface_);
    if (auto buffer = ohosSurface->GetCurrentBuffer()) {
        if (needFlushBuffer) {
            RS_OPTIONAL_TRACE_NAME_FMT(
                "Flush buffer, nodeId=%" PRIu64 ", width=%d, height=%d", nodeId_, width_, height_);
            ohosSurface->OnFlushBuffer();
        }
        return buffer;
    }
    return nullptr;
}

void RSCanvasModifiersDrawable::OnDirtyBufferCollected(int64_t lastBufferExpiredTime)
{
    bufferRequested_ = false;
    bufferDirty_ = false;
    lastBufferExpiredTime_ = lastBufferExpiredTime;
}

bool RSCanvasModifiersDrawable::IsFree(int64_t now, int64_t maxDuration)
{
    if (lastBufferExpiredTime_ == 0) {
        return false;
    }
    auto duration = now - lastBufferExpiredTime_;
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
    lastBufferExpiredTime_ = 0;
    RS_LOGI("RSCanvasModifiersDrawable::CleanBuffer, nodeId=%{public}" PRIu64, nodeId_);
}

bool RSCanvasModifiersDrawable::GetPixelMap(std::shared_ptr<Media::PixelMap> pixelMap, const Drawing::Rect* rect,
    Drawing::DrawCmdListPtr drawCmdList, std::shared_ptr<RSCanvasModifiersContext> canvasModifiersContext)
{
    if (pixelMap == nullptr || rect == nullptr) {
        return false;
    }
    auto bitmapFormat = Drawing::BitmapFormat { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    auto image = GetImage(bitmapFormat, canvasModifiersContext);
    if (image == nullptr) {
        return false;
    }

    if (drawCmdList != nullptr && !drawCmdList->IsEmpty()) {
        auto imageInfo = Drawing::ImageInfo { width_, height_, bitmapFormat.colorType, bitmapFormat.alphaType };
        auto surface = Drawing::Surface::MakeRenderTarget(canvasModifiersContext->gpuContext_.get(), false, imageInfo);
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

bool RSCanvasModifiersDrawable::GetBitmap(
    Drawing::Bitmap& bitmap, std::shared_ptr<RSCanvasModifiersContext> canvasModifiersContext)
{
    auto alphaType = Drawing::ALPHATYPE_PREMUL;
    const auto& format = bitmap.GetFormat();
    if (format.alphaType != Drawing::ALPHATYPE_UNKNOWN) {
        alphaType = format.alphaType;
    }
    auto bitmapFormat = Drawing::BitmapFormat { Drawing::COLORTYPE_RGBA_8888, alphaType };
    auto image = GetImage(bitmapFormat, canvasModifiersContext);
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
    const Drawing::BitmapFormat& bitmapFormat, std::shared_ptr<RSCanvasModifiersContext> canvasModifiersContext)
{
    if (drawingSurface_ == nullptr) {
        RS_LOGE("RSCanvasModifiersDrawable::GetImage: Null surface, nodeId=%{public}" PRIu64, nodeId_);
        return nullptr;
    }
    auto gpuContext = canvasModifiersContext->gpuContext_;
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

RSCanvasModifiersDraw::RSCanvasModifiersDraw(std::weak_ptr<RSRenderInterface> weakRenderInterface)
{
    canvasModifiersContext_ = std::make_shared<RSCanvasModifiersContext>(weakRenderInterface);
}

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
    });
    RS_LOGI("RSCanvasModifiersDraw::StartThread: Thread started");
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

bool RSCanvasModifiersDraw::RemoveCleanFreeBuffersTask()
{
    if (!threadStarted_.load()) {
        return false;
    }
    RemoveTask(CLEAN_FREE_BUFFERS_TASK_NAME);
    return true;
}

void RSCanvasModifiersDraw::ReleaseResource()
{
    RS_TRACE_NAME_FMT("RSCanvasModifiersDraw::ReleaseResource");
    drawableMap_.clear();
    transactionConfigList_.clear();
    if (destroySemaphoreInfo_ != nullptr) {
        DestroySemaphoreInfo::DestroySemaphore(destroySemaphoreInfo_);
        destroySemaphoreInfo_ = nullptr;
    }
    canvasModifiersContext_->ReleaseGpuContext();
}

void RSCanvasModifiersDraw::SetCacheDir(const std::string& cacheDir)
{
    canvasModifiersContext_->cacheDir_ = cacheDir;
}

void RSCanvasModifiersDraw::OnNodeCreate(NodeId nodeId, GraphicColorGamut colorSpace)
{
    if (canvasModifiersContext_->GetGpuContext() == nullptr) {
        RS_LOGE("RSCanvasModifiersDraw::OnNodeCreate, null gpuContext, nodeId=%{public}" PRIu64, nodeId);
        return;
    }
    auto& drawable = drawableMap_[nodeId];
    drawable.nodeId_ = nodeId;
    drawable.CreateProducerSurface(canvasModifiersContext_, colorSpace);
    lastGpuContextActiveTime_ = static_cast<int64_t>(
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch())
            .count());
    // Only restart the clean task when the map transitions from empty to non-empty
    // (i.e. the first node is created), otherwise keep the existing loop running.
    if (drawableMap_.size() == 1) {
        RemoveTask(CLEAN_FREE_BUFFERS_TASK_NAME);
        CleanFreeBuffers(false);
    }
}

void RSCanvasModifiersDraw::OnNodeRelease(NodeId nodeId)
{
    auto it = drawableMap_.find(nodeId);
    if (it == drawableMap_.end()) {
        RS_LOGE("RSCanvasModifiersDraw::OnNodeRelease, node not found, nodeId=%{public}" PRIu64, nodeId);
        return;
    }
    it->second.ReleaseProducerSurface(canvasModifiersContext_);
    drawableMap_.erase(nodeId);
    lastGpuContextActiveTime_ = static_cast<int64_t>(
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch())
            .count());
}

void RSCanvasModifiersDraw::OnNodeStateChanged(NodeId nodeId, RSNodeState nodeState)
{
    auto it = drawableMap_.find(nodeId);
    if (it == drawableMap_.end()) {
        RS_LOGE("RSCanvasModifiersDraw::OnNodeStateChanged, node not found, nodeId=%{public}" PRIu64, nodeId);
        return;
    }
    auto& drawable = it->second;
    drawable.nodeState_ = nodeState;
    if (nodeState != RSNodeState::ACTIVE) {
        CleanFreeBuffers(true);
        return;
    }
    canvasModifiersContext_->RestoreGpuCacheLimits();
    drawable.UpdateContent(nullptr, true);
}

void RSCanvasModifiersDraw::ResetSurface(NodeId nodeId, int width, int height, bool sizeOutOfGpuLimit)
{
    auto it = drawableMap_.find(nodeId);
    if (it == drawableMap_.end()) {
        RS_LOGE("RSCanvasModifiersDraw::ResetSurface, node not found, nodeId=%{public}" PRIu64, nodeId);
        return;
    }
    if (!sizeOutOfGpuLimit) {
        canvasModifiersContext_->RestoreGpuCacheLimits();
    }
    it->second.ResetSurface(width, height, sizeOutOfGpuLimit);
    // DDGR resources created by buffers can only be released after submit.
    // If ResetSurface is called multiple times without content drawing, DDGR resources
    // will keep accumulating and cause memory leaks. So a submit must be guaranteed
    // after every ResetSurface.
    needForceSubmit_ = true;
    lastGpuContextActiveTime_ = static_cast<int64_t>(
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch())
            .count());
    CleanFreeBuffers(true);
}

bool RSCanvasModifiersDraw::GetBitmap(NodeId nodeId, Drawing::Bitmap& bitmap)
{
    auto it = drawableMap_.find(nodeId);
    if (it == drawableMap_.end()) {
        RS_LOGE("RSCanvasModifiersDraw::GetBitmap, node not found, nodeId=%{public}" PRIu64, nodeId);
        return false;
    }
    return it->second.GetBitmap(bitmap, canvasModifiersContext_);
}

bool RSCanvasModifiersDraw::GetPixelMap(NodeId nodeId, std::shared_ptr<Media::PixelMap> pixelMap,
    const Drawing::Rect* rect, Drawing::DrawCmdListPtr drawCmdList)
{
    auto it = drawableMap_.find(nodeId);
    if (it == drawableMap_.end()) {
        RS_LOGE("RSCanvasModifiersDraw::GetPixelMap, node not found, nodeId=%{public}" PRIu64, nodeId);
        return false;
    }
    return it->second.GetPixelMap(pixelMap, rect, drawCmdList, canvasModifiersContext_);
}

void RSCanvasModifiersDraw::UpdateCanvasContent(NodeId nodeId, Drawing::DrawCmdListPtr drawCmdList)
{
    auto it = drawableMap_.find(nodeId);
    if (it == drawableMap_.end()) {
        RS_LOGE("RSCanvasModifiersDraw::UpdateCanvasContent, node not found, nodeId=%{public}" PRIu64, nodeId);
        return;
    }
    canvasModifiersContext_->RestoreGpuCacheLimits();
    it->second.UpdateContent(drawCmdList, false);
    lastGpuContextActiveTime_ = static_cast<int64_t>(
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch())
            .count());
}

DestroySemaphoreInfo* RSCanvasModifiersDraw::FlushSurfaceWithSemaphore(
    VkSemaphore& semaphore, std::shared_ptr<Drawing::Surface> drawingSurface)
{
    if (canvasModifiersContext_->renderContext_ == nullptr) {
        RS_LOGE("RSCanvasModifiersDraw::FlushSurfaceWithSemaphore renderContext_ is nullptr");
        return nullptr;
    }
    auto vkInterface = RsVulkanContext::Get(canvasModifiersContext_->renderContext_->GetType()).GetRsVulkanInterface();
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
    if (canvasModifiersContext_->renderContext_ == nullptr) {
        RS_LOGE("RSCanvasModifiersDraw::GetFenceFd renderContext_ is nullptr");
        return fenceFd;
    }
    if (semaphore != VK_NULL_HANDLE) {
        NativeBufferUtils::GetFenceFdFromSemaphore(
            RsVulkanContext::Get(canvasModifiersContext_->renderContext_->GetType()).GetRsVulkanInterface(), semaphore,
            fenceFd);
    }
    return fenceFd;
}

void RSCanvasModifiersDraw::SubmitAndCollectCanvasBuffers()
{
    if (canvasModifiersContext_->gpuContext_ == nullptr) {
        RS_LOGE("RSCanvasModifiersDraw::SubmitAndCollectCanvasBuffers, null gpuContext");
        return;
    }

    bool needFlushBuffer = false;
    for (auto& [_, drawable] : drawableMap_) {
        if (drawable.nodeState_ == RSNodeState::ACTIVE) {
            needFlushBuffer = true;
            break;
        }
    }
    std::vector<std::pair<sptr<SurfaceBuffer>, RSCanvasModifiersDrawable*>> bufferList;
    for (auto& [_, drawable] : drawableMap_) {
        if (auto buffer = drawable.GetBuffer(needFlushBuffer)) {
            bufferList.emplace_back(buffer, &drawable);
        }
    }
    bool needForceSubmit = needForceSubmit_;
    needForceSubmit_ = false;
    if (bufferList.empty()) {
        RS_TRACE_NAME_FMT("Buffer list empty");
        if (needForceSubmit) {
            canvasModifiersContext_->gpuContext_->Submit();
        }
        return;
    }
    if (!needFlushBuffer) {
        RS_TRACE_NAME_FMT("No active node, submit only");
        canvasModifiersContext_->gpuContext_->Submit();
        return;
    }
    if (destroySemaphoreInfo_ != nullptr) {
        DestroySemaphoreInfo::DestroySemaphore(destroySemaphoreInfo_);
        destroySemaphoreInfo_ = nullptr;
    }
    VkSemaphore semaphore = VK_NULL_HANDLE;
    destroySemaphoreInfo_ = FlushSurfaceWithSemaphore(semaphore, bufferList.back().second->drawingSurface_);
    canvasModifiersContext_->gpuContext_->Submit();
    sptr<SyncFence> fence = new SyncFence(GetFenceFd(semaphore));
    auto now = static_cast<int64_t>(
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch())
            .count());
    for (const auto& [buffer, drawable] : bufferList) {
        AppendTransactionConfig(drawable->nodeId_, buffer, fence);
        drawable->OnDirtyBufferCollected(now);
    }
    RS_OPTIONAL_TRACE_NAME_FMT("SubmitAndCollectCanvasBuffers, bufferCount=%zu", transactionConfigList_.size());
}

void RSCanvasModifiersDraw::AppendTransactionConfig(NodeId nodeId, sptr<SurfaceBuffer> buffer, sptr<SyncFence> fence)
{
    RSTransactionConfig config;
    config.nodeId = nodeId;
    config.transaction = new RSBufferTransaction(buffer);
    config.transaction->SetFence(fence);
    std::vector<Rect> damages{Rect{0, 0, buffer->GetWidth(), buffer->GetHeight()}};
    config.transaction->SetDamages(damages);
    transactionConfigList_.emplace_back(config);
}

std::unique_ptr<RSCommand> RSCanvasModifiersDraw::SyncAndWait()
{
    RS_OPTIONAL_TRACE_NAME_FMT("SyncAndWait, bufferCount=%zu", transactionConfigList_.size());
    std::unique_ptr<RSCommand> command = nullptr;
    if (!transactionConfigList_.empty()) {
        // Static is safe: GetRealPid() is constant within a process, and this nodeId
        // is only used to pass server-side PID validation (ExtractPid(nodeId) == callingPid).
        // Left shift by 32 to match the standard NodeId format (pid << 32 | sequence).
        static NodeId nodeId = (NodeId)GetRealPid() << 32;
        // TransactionBufferCommand constructor swaps configs into its own configList_, leaving
        // transactionConfigList_ empty after construction — same effect as an explicit swap.
        command = std::make_unique<TransactionBufferCommand>(transactionConfigList_, nodeId);
    } else if (needForceSubmit_ && canvasModifiersContext_->gpuContext_ != nullptr) {
        // SubmitAndCollectCanvasBuffers is triggered by RSUIDirector::FlushModifier,
        // which may not be reached when there is no content update (theoretical analysis,
        // not observed in practice). Submit here as a fallback to ensure DDGR resources
        // are released in time.
        canvasModifiersContext_->gpuContext_->Submit();
        needForceSubmit_ = false;
    }
    return command;
}

void RSCanvasModifiersDraw::CleanFreeBuffers(bool immediately)
{
    if (immediately) {
        RemoveTask(CLEAN_FREE_BUFFERS_TASK_NAME);
    }
    std::weak_ptr<RSCanvasModifiersDraw> weakCanvasModifiersDraw = shared_from_this();
    PostTask(
        [weakCanvasModifiersDraw, immediately] {
            auto canvasModifiersDraw = weakCanvasModifiersDraw.lock();
            if (canvasModifiersDraw == nullptr) {
                return;
            }
            canvasModifiersDraw->DoCleanFreeBuffers(immediately ? 0 : CLEAN_FREE_BUFFERS_DURATION);
            if (!canvasModifiersDraw->drawableMap_.empty()) {
                canvasModifiersDraw->CleanFreeBuffers(false);
            }
        },
        CLEAN_FREE_BUFFERS_TASK_NAME, immediately ? CLEAN_FREE_BUFFERS_IMMEDIATELY_DELAY : CLEAN_FREE_BUFFERS_DURATION);
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
    for (auto* drawable : freeDrawableList) {
        drawable->CleanBuffer();
    }
    if (canvasModifiersContext_->gpuContext_ != nullptr && lastGpuContextActiveTime_ > 0 &&
        now - lastGpuContextActiveTime_ > CLEAN_FREE_BUFFERS_DURATION) {
        canvasModifiersContext_->CleanGpuCache();
        lastGpuContextActiveTime_ = 0;
        RS_LOGI("RSCanvasModifiersDrawable::DoCleanFreeBuffers: clean GPU resource");
    }
}
} // namespace Rosen
} // namespace OHOS