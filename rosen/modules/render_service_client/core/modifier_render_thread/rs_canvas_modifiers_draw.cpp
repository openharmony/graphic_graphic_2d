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
#include "platform/common/rs_log.h"
#include "platform/ohos/backend/native_buffer_utils.h"
#include "platform/ohos/backend/rs_surface_ohos_vulkan.h"
#include "render_context/shader_cache.h"
#include "ui/rs_canvas_drawing_node.h"

namespace OHOS {
namespace Rosen {
void RSCanvasModifiersDraw::SetCacheDir(const std::string& path)
{
    ShaderCache::Instance().SetFilePath(path);
    cacheDir_ = path;
}

std::string RSCanvasModifiersDraw::GetCacheDir()
{
    return cacheDir_;
}

void RSCanvasModifiersDraw::CleanCanvasDrawingNodeCache(
    NodeId nodeId, std::weak_ptr<RSRenderInterface> weakRenderInterface)
{
    auto surfaceEntryIt = surfaceEntryMap_.find(nodeId);
    if (surfaceEntryIt != surfaceEntryMap_.end()) {
        surfaceEntryMap_.erase(surfaceEntryIt);
    }

    auto nodeIt = canvasDrawingNodeMap_.find(nodeId);
    if (nodeIt != canvasDrawingNodeMap_.end()) {
        canvasDrawingNodeMap_.erase(nodeIt);
    }

    if (auto renderInterface = weakRenderInterface.lock()) {
        renderInterface->RemoveCanvasSurface(nodeId);
    }
}

void RSCanvasModifiersDraw::OnProducerSurfaceCreated(NodeId nodeId, std::shared_ptr<RSSurface> producerSurface)
{
    surfaceEntryMap_[nodeId].producerSurface = producerSurface;
    surfaceEntryMap_[nodeId].drawCmdListCache_ = std::make_unique<std::vector<Drawing::DrawCmdListPtr>>();
}

void RSCanvasModifiersDraw::CacheDrawCmdList(NodeId nodeId, Drawing::DrawCmdListPtr drawCmdList)
{
    auto& surfaceEntry = surfaceEntryMap_[nodeId];
    if (surfaceEntry.drawCmdListCache_ != nullptr) {
        surfaceEntry.drawCmdListCache_->emplace_back(drawCmdList);
    } else {
        RS_LOGE("%{public}s Null drawCmdListCache, drop drawCmdList, nodeId=%{public}" PRIu64, __func__, nodeId);
    }
}

void RSCanvasModifiersDraw::ResetSurface(
    int width, int height, NodeId nodeId, bool sizeOutOfGpuLimit, std::weak_ptr<RSCanvasDrawingNode> weakNode)
{
    if (weakNode.expired()) {
        RS_LOGE("%{public}s Node released already, nodeId=%{public}" PRIu64, __func__, nodeId);
        return;
    }
    if (sizeOutOfGpuLimit && surfaceEntryMap_.count(nodeId) == 0) {
        return;
    }
    auto& surfaceEntry = surfaceEntryMap_[nodeId];
    if (surfaceEntry.producerSurface == nullptr) {
        RS_LOGE("%{public}s Null producer surface, nodeId=%{public}" PRIu64, __func__, nodeId);
        return;
    }
    if (surfaceEntry.width == width && surfaceEntry.height == height) {
        RS_LOGE("%{public}s Same width and height, nodeId=%{public}" PRIu64, __func__, nodeId);
        return;
    }

    // Current buffer is not flushed, cancel it.
    if (surfaceEntry.surfaceFrame != nullptr) {
        std::static_pointer_cast<RSSurfaceOhosVulkan>(surfaceEntry.producerSurface)->CancelBufferForCurrentFrame();
    }

    surfaceEntry.Reset();
    if (sizeOutOfGpuLimit) {
        canvasDrawingNodeMap_.erase(nodeId);
        return;
    }

    canvasDrawingNodeMap_[nodeId] = weakNode;
    surfaceEntry.width = width;
    surfaceEntry.height = height;
    surfaceEntry.needResetCanvas = true;
    surfaceEntry.forceFlushBuffer = true;
    ConvertCmdListForCanvas(nodeId);
}

std::shared_ptr<Drawing::Image> RSCanvasModifiersDraw::GetImage(NodeId nodeId, const SurfaceEntry& surfaceEntry,
    const Drawing::BitmapFormat& bitmapFormat, std::shared_ptr<Drawing::GPUContext> gpuContext)
{
    if (surfaceEntry.drawingSurface == nullptr) {
        RS_LOGE("%{public}s Null surface, nodeId=%{public}" PRIu64, __func__, nodeId);
        return nullptr;
    }
    if (gpuContext == nullptr) {
        RS_LOGE("%{public}s Null GPU context, nodeId=%{public}" PRIu64, __func__, nodeId);
        return nullptr;
    }

    auto image = std::make_shared<Drawing::Image>();
    if (!image->BuildFromTexture(*gpuContext, surfaceEntry.drawingSurface->GetBackendTexture().GetTextureInfo(),
        Drawing::TextureOrigin::TOP_LEFT, bitmapFormat, nullptr)) {
        RS_LOGE("%{public}s BuildFromTexture fail, nodeId=%{public}" PRIu64, __func__, nodeId);
        return nullptr;
    }
    return image;
}

bool RSCanvasModifiersDraw::GetBitmap(Drawing::Bitmap& bitmap, std::shared_ptr<RSCanvasDrawingNode> node)
{
    if (node == nullptr) {
        RS_LOGE("%{public}s Node released already" PRIu64, __func__);
        return false;
    }
    auto nodeId = node->GetId();
    auto& surfaceEntry = surfaceEntryMap_[nodeId];
    auto bitmapFormat = Drawing::BitmapFormat{ Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    auto gpuContext = RsVulkanContext::GetSingleton().GetRecyclableDrawingContext(cacheDir_);
    auto image = GetImage(nodeId, surfaceEntry, bitmapFormat, gpuContext);
    if (image == nullptr) {
        return false;
    }
    if (!image->AsLegacyBitmap(bitmap)) {
        RS_LOGE("%{public}s AsLegacyBitmap fail, nodeId=%{public}" PRIu64, __func__, nodeId);
        return false;
    }
    return true;
}

bool RSCanvasModifiersDraw::GetPixelMap(std::shared_ptr<Media::PixelMap> pixelMap, const Drawing::Rect* rect,
    std::shared_ptr<RSCanvasDrawingNode> node, Drawing::DrawCmdListPtr drawCmdList)
{
    if (node == nullptr) {
        RS_LOGE("%{public}s Node released already" PRIu64, __func__);
        return false;
    }
    if (pixelMap == nullptr || rect == nullptr) {
        return false;
    }
    auto nodeId = node->GetId();
    auto& surfaceEntry = surfaceEntryMap_[nodeId];
    auto bitmapFormat = Drawing::BitmapFormat{ Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    auto gpuContext = RsVulkanContext::GetSingleton().GetRecyclableDrawingContext(cacheDir_);
    auto image = GetImage(nodeId, surfaceEntry, bitmapFormat, gpuContext);
    if (image == nullptr) {
        return false;
    }

    if (drawCmdList != nullptr && !drawCmdList->IsEmpty()) {
        auto imageInfo =
            Drawing::ImageInfo{surfaceEntry.width, surfaceEntry.height, bitmapFormat.colorType, bitmapFormat.alphaType};
        auto surface = Drawing::Surface::MakeRenderTarget(gpuContext.get(), false, imageInfo);
        if (auto canvas = surface != nullptr ? surface->GetCanvas() : nullptr) {
            canvas->DrawImage(*image, 0, 0, Drawing::SamplingOptions());
            drawCmdList->Playback(*canvas);
            canvas->Flush();
            image = surface->GetImageSnapshot();
        }
        if (image == nullptr) {
            RS_LOGE("%{public}s GetImageSnapshot fail, nodeId=%{public}" PRIu64, __func__, nodeId);
            return false;
        }
    }

    auto imageInfo =
        Drawing::ImageInfo{pixelMap->GetWidth(), pixelMap->GetHeight(), bitmapFormat.colorType, bitmapFormat.alphaType};
    if (!image->ReadPixels(
        imageInfo, pixelMap->GetWritablePixels(), pixelMap->GetRowStride(), rect->GetLeft(), rect->GetTop())) {
        RS_LOGE("%{public}s ReadPixels fail, nodeId=%{public}" PRIu64, __func__, nodeId);
        return false;
    }
    if (auto* surfaceBuffer = static_cast<SurfaceBuffer*>(pixelMap->GetFd())) {
        if (surfaceBuffer->FlushCache() != GSERROR_OK) {
            RS_LOGE("%{public}s FlushCache fail, nodeId=%{public}" PRIu64, __func__, nodeId);
        }
    }
    return true;
}

void RSCanvasModifiersDraw::Playback(
    const std::shared_ptr<Drawing::Surface>& surface, const Drawing::DrawCmdListPtr& cmdList)
{
    auto canvas = surface->GetCanvas();
    cmdList->Playback(*canvas);
    if (RSSystemProperties::GetHybridRenderDfxEnabled()) {
        auto matrix = canvas->GetTotalMatrix();
        canvas->SetMatrix(Drawing::Matrix());
        Drawing::Pen pen;
        pen.SetWidth(10); // DFX border width 10
        pen.SetColor(0xFFFF8000); // 0xFFFF8000 is orange
        canvas->AttachPen(pen);
        auto rect = Drawing::Rect(0, 0, surface->Width(), surface->Height());
        canvas->DrawRect(rect);
        canvas->DetachPen();
        canvas->SetMatrix(matrix);
    }
}

void RSCanvasModifiersDraw::UpdateCanvasContent(
    NodeId nodeId, std::weak_ptr<RSCanvasDrawingNode> weakNode, bool forceFlushBuffer)
{
    if (weakNode.expired()) {
        RS_LOGE("%{public}s Node released already, nodeId=%{public}" PRIu64, __func__, nodeId);
        return;
    }
    surfaceEntryMap_[nodeId].forceFlushBuffer = forceFlushBuffer;
    ConvertCmdListForCanvas(nodeId);
}

void RSCanvasModifiersDraw::ConvertCmdListForCanvas(NodeId nodeId)
{
    auto& surfaceEntry = surfaceEntryMap_[nodeId];
    if (surfaceEntry.producerSurface == nullptr) {
        RS_LOGE("%{public}s Null producer surface, nodeId=%{public}" PRIu64, __func__, nodeId);
        return;
    }

    if (!surfaceEntry.drawCmdListCache_->empty() || surfaceEntry.forceFlushBuffer) {
        ConvertCmdListByBuffer(nodeId, surfaceEntry);
    }
}

void RSCanvasModifiersDraw::ConvertCmdListByBuffer(NodeId nodeId, SurfaceEntry& surfaceEntry)
{
    RequestCanvasBuffer(nodeId);
    if (surfaceEntry.surfaceFrame == nullptr) {
        RS_LOGE("%{public}s Null surfaceFrame, nodeId=%{public}" PRIu64, __func__, nodeId);
        return;
    }
    auto drawingSurface = surfaceEntry.drawingSurface;
    for (auto& cmdList : *surfaceEntry.drawCmdListCache_) {
        Playback(drawingSurface, cmdList);
        cmdList->ClearOp();
    }
    NativeBufferUtils::CreateVkSemaphore(surfaceEntry.semaphore);
    FlushCanvasSurfaceWithSemaphore(drawingSurface, surfaceEntry.semaphore);
    surfaceEntry.forceFlushBuffer = false;
}

void RSCanvasModifiersDraw::RequestCanvasBuffer(NodeId nodeId)
{
    if (canvasDrawingNodeMap_.find(nodeId) == canvasDrawingNodeMap_.end()) {
        return;
    }
    auto& surfaceEntry = surfaceEntryMap_[nodeId];
    if (surfaceEntry.producerSurface == nullptr) {
        return;
    }
    if (surfaceEntry.surfaceFrame != nullptr) {
        return;
    }
    surfaceEntry.surfaceFrame = CheckAndDrawHistory(surfaceEntry, nodeId);
}

void RSCanvasModifiersDraw::FlushCanvasSurfaceWithSemaphore(
    const std::shared_ptr<Drawing::Surface>& surface, VkSemaphore& semaphore)
{
#ifdef USE_M133_SKIA
    GrBackendSemaphore backendSemaphore = GrBackendSemaphores::MakeVk(semaphore);
#else
    GrBackendSemaphore backendSemaphore;
    backendSemaphore.initVulkan(semaphore);
#endif
    auto& vkContext = RsVulkanContext::GetSingleton().GetRsVulkanInterface();
    DestroySemaphoreInfo* destroyInfo =
        new DestroySemaphoreInfo(vkContext.vkDestroySemaphore, vkContext.GetDevice(), semaphore);

    Drawing::FlushInfo drawingFlushInfo;
    drawingFlushInfo.backendSurfaceAccess = true;
    drawingFlushInfo.numSemaphores = 1;
    drawingFlushInfo.backendSemaphore = static_cast<void*>(&backendSemaphore);
    drawingFlushInfo.finishedProc = [](void* context) {
        DestroySemaphoreInfo::DestroySemaphore(context);
    };
    drawingFlushInfo.finishedContext = destroyInfo;
    surface->Flush(&drawingFlushInfo);
    canvasNewSemaphoreInfos_.emplace_back(destroyInfo);
}

std::unique_ptr<RSSurfaceFrame> RSCanvasModifiersDraw::CheckAndDrawHistory(SurfaceEntry& surfaceEntry, NodeId nodeId)
{
    auto ohosSurface = std::static_pointer_cast<RSSurfaceOhos>(surfaceEntry.producerSurface);
    auto renderFrame = ohosSurface->RequestFrame(surfaceEntry.width, surfaceEntry.height);
    if (renderFrame == nullptr) {
        RS_LOGE("%{public}s null renderFrame, nodeId=%{public}lu, width=%{public}d, height=%{public}d",
            __func__, nodeId, surfaceEntry.width, surfaceEntry.height);
        return nullptr;
    }
    auto drawingSurface = renderFrame->GetSurface();
    if (drawingSurface == nullptr) {
        RS_LOGE("%{public}s null drawingSurface, nodeId=%{public}lu", __func__, nodeId);
        return nullptr;
    }
    auto canvas = drawingSurface->GetCanvas();
    if (canvas == nullptr) {
        RS_LOGE("%{public}s null canvas", __func__);
        return nullptr;
    }

    if (surfaceEntry.drawingSurface != nullptr) {
        if (auto lastCanvas = surfaceEntry.drawingSurface->GetCanvas()) {
            canvas->InheritStateAndContentFrom(lastCanvas.get(), false);
        }
    }
    if (surfaceEntry.needResetCanvas) {
        canvas->Clear(Drawing::Color::COLOR_TRANSPARENT);
        canvas->RestoreToCount(1);
        canvas->SetMatrix(Drawing::Matrix());
        surfaceEntry.needResetCanvas = false;
    }
    surfaceEntry.drawingSurface = drawingSurface;
    return renderFrame;
}

void RSCanvasModifiersDraw::SubmitAndCollectCanvasBuffers()
{
    std::vector<std::tuple<NodeId, sptr<SurfaceBuffer>, SurfaceEntry*>> bufferList;
    for (const auto& [nodeId, weakNode] : canvasDrawingNodeMap_) {
        if (auto node = weakNode.lock(); node == nullptr || node->GetNodeState() == RSNodeState::INACTIVE) {
            continue;
        }
        auto& surfaceEntry = surfaceEntryMap_[nodeId];
        if (surfaceEntry.producerSurface == nullptr) {
            RS_LOGE("%{public}s Null producer surface, nodeId=%{public}" PRIu64, __func__, nodeId);
            continue;
        }
        if (surfaceEntry.surfaceFrame == nullptr || surfaceEntry.semaphore == VK_NULL_HANDLE) {
            continue;
        }
        auto ohosSurface = std::static_pointer_cast<RSSurfaceOhosVulkan>(surfaceEntry.producerSurface);
        auto buffer = ohosSurface->GetCurrentBuffer();
        if (buffer == nullptr) {
            continue;
        }
        ohosSurface->OnFlushBuffer();
        bufferList.emplace_back(nodeId, buffer, &surfaceEntry);
    }
    if (!bufferList.empty()) {
        if (auto gpuContext = RsVulkanContext::GetSingleton().GetRecyclableDrawingContext(cacheDir_)) {
            gpuContext->Submit();
        }
        auto now = static_cast<int64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count());
        for (const auto& [nodeId, buffer, surfaceEntry] : bufferList) {
            int32_t fenceFd = -1;
            NativeBufferUtils::GetFenceFdFromSemaphore(surfaceEntry->semaphore, fenceFd);
            AppendTransactionConfig(buffer, fenceFd, nodeId);
            surfaceEntry->surfaceFrame = nullptr;
            surfaceEntry->semaphore = VK_NULL_HANDLE;
            surfaceEntry->lastFlushBufferTime = now;
        }
    }
    DestroyCanvasSemaphore();
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

void RSCanvasModifiersDraw::AppendTransactionConfig(sptr<SurfaceBuffer> buffer, int fenceFd, NodeId nodeId)
{
    RSTransactionConfig config;
    config.nodeId = nodeId;
    config.transaction = new RSBufferTransaction(buffer);
    sptr<SyncFence> fence = new SyncFence(fenceFd);
    config.transaction->SetFence(fence);
    std::vector<Rect> damages{Rect{0, 0, buffer->GetWidth(), buffer->GetHeight()}};
    config.transaction->SetDamages(damages);
    transactionConfigList_.push_back(config);
}

void RSCanvasModifiersDraw::SwapTransactionConfigList(std::vector<RSTransactionConfig>& transactionConfigList)
{
    std::swap(transactionConfigList, transactionConfigList_);
}

void RSCanvasModifiersDraw::CleanFreeBuffers(int64_t maxDuration)
{
    auto now = static_cast<int64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count());
    std::vector<std::pair<std::shared_ptr<RSSurfaceOhosVulkan>, NodeId>> surfaceList;
    for (const auto& [nodeId, weakNode] : canvasDrawingNodeMap_) {
        auto& surfaceEntry = surfaceEntryMap_[nodeId];
        // No free buffers
        if (surfaceEntry.lastFlushBufferTime == 0) {
            continue;
        }
        auto duration = now - surfaceEntry.lastFlushBufferTime;
        if (duration <= maxDuration) {
            continue;
        }
        if (weakNode.expired()) {
            continue;
        }
        if (surfaceEntry.producerSurface == nullptr) {
            RS_LOGE("%{public}s Null producer surface, nodeId=%{public}" PRIu64, __func__, nodeId);
            continue;
        }
        surfaceList.push_back(
            std::make_pair(std::static_pointer_cast<RSSurfaceOhosVulkan>(surfaceEntry.producerSurface), nodeId));
    }
    if (surfaceList.empty()) {
        return;
    }
    if (auto gpuContext = RsVulkanContext::GetSingleton().GetRecyclableDrawingContext(cacheDir_)) {
        gpuContext->FlushAndSubmit(true);
    }
    for (const auto& [surface, nodeId] : surfaceList) {
        surface->CleanReleasedBuffers();
        auto& surfaceEntry = surfaceEntryMap_[nodeId];
        surfaceEntry.lastFlushBufferTime = 0;
        RS_LOGI("RSCanvasModifiersDraw::CleanFreeBuffers, nodeId=%{public}" PRIu64, nodeId);
    }
}
} // namespace Rosen
} // namespace OHOS