/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include <algorithm>
#include <cmath>
#include <map>

#include "common/rs_optional_trace.h"
#include "feature/hdr/rs_hdr_util.h"
#include "feature/hwc/hpae_offline/rs_gpu_offline_device.h"
#include "feature/hwc/hpae_offline/rs_offline_util.h"
#include "engine/rs_base_render_engine.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "pipeline/render_thread/rs_uni_render_util.h"
#include "pipeline/render_thread/rs_render_engine.h"
#include "platform/common/rs_system_properties.h"
#include "v2_2/cm_color_space.h"
#ifdef USE_VIDEO_PROCESSING_ENGINE
#include "metadata_helper.h"
#endif

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t MAX_NUM_INVALID_FRAME = 10;
}
using namespace HDI::Display::Graphic::Common::V1_0;

RSGPUOfflineDevice::RSGPUOfflineDevice()
{
    RS_LOGD("RSGPUOfflineDevice::Constructed");
}

RSGPUOfflineDevice::~RSGPUOfflineDevice()
{
    std::lock_guard<std::mutex> lock(cacheMutex_);
    offlineContextCache_.clear();
    RS_LOGD("RSGPUOfflineDevice::Destructed");
}

bool RSGPUOfflineDevice::IsRSOfflineDeviceReady(std::shared_ptr<RSSurfaceRenderNode>& surfaceNode)
{
    uint64_t currentVsync = RSMainThread::Instance()->GetVsyncId();
    if (currentVsyncId_ != currentVsync) {
        currentVsyncId_ = currentVsync;
        currentFrameNodeCount_ = 0;
    }
    if (currentFrameNodeCount_ >= MAX_CACHE_SIZE) {
        RS_LOGI("RSGPUOfflineDevice::Already processed %{public}zu nodes in this vsync, skip", currentFrameNodeCount_);
        return false;
    }
    if (!CheckCondition(surfaceNode)) {
        RS_LOGD("RSGPUOfflineDevice::CheckCondition failed.");
        return false;
    }
    RS_LOGD("RSGPUOfflineDevice::GPU offline condition passed for node %{public}" PRIu64, surfaceNode->GetId());
    currentFrameNodeCount_++;

    auto nodeId = surfaceNode->GetId();
    auto offlineContext = GetOrCreateOfflineContext(nodeId);
    if (offlineContext == nullptr) {
        RS_LOGI("RSGPUOfflineDevice::Context is nullptr, banned offline!!!");
        return false;
    }
    if (offlineContext->timeout) {
        RS_LOGI("RSGPUOfflineDevice::This scene has experienced a timeout event!!! banned offline");
        return false;
    }
    if (!UpdateContext(surfaceNode, offlineContext)) {
        RS_LOGI("RSGPUOfflineDevice::UpdateContext failed");
        return false;
    }
    return true;
}

bool RSGPUOfflineDevice::PostProcessOfflineTask(
    std::shared_ptr<RSSurfaceRenderNode>& surfaceNode, offlineTaskId taskId)
{
    if (!CheckCondition(surfaceNode)) {
        RS_LOGD("RSGPUOfflineDevice::CheckCondition failed.");
        return false;
    }
    auto offlineContext = GetOfflineContext(surfaceNode->GetId());
    if (offlineContext == nullptr) {
        RS_LOGI("RSGPUOfflineDevice::Context is nullptr, nodeId=%{public}" PRIu64, surfaceNode->GetId());
        return false;
    }
    if (offlineContext->timeout) {
        RS_LOGI("RSGPUOfflineDevice::This scene has experienced a timeout event!!! banned offline");
        return false;
    }
    // while doing direct composition, there is no IsRSHpaeOfflineProcessorReady to update context
    if (!UpdateContext(surfaceNode, offlineContext)) {
        RS_LOGI("RSGPUOfflineDevice::UpdateContext failed");
        return false;
    }
    auto* params = surfaceNode->GetStagingRenderParams().get();
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(params);
    RS_LOGD("RSGPUOfflineDevice::PostProcessOfflineTask task[%{public}" PRIu64 "-%{public}" PRIu64 "] by node",
        taskId.first, taskId.second);
    return PostOfflineTaskCommon(offlineContext, surfaceParams, taskId);
}

bool RSGPUOfflineDevice::PostProcessOfflineTask(
    std::shared_ptr<DrawableV2::RSSurfaceRenderNodeDrawable>& surfaceDrawable, offlineTaskId taskId)
{
    auto offlineContext = GetOfflineContext(surfaceDrawable->GetId());
    if (offlineContext == nullptr) {
        RS_LOGI("RSGPUOfflineDevice::Context is nullptr, nodeId=%{public}" PRIu64, surfaceDrawable->GetId());
        return false;
    }
    if (offlineContext->timeout) {
        RS_LOGI("RSGPUOfflineDevice::This scene has experienced a timeout event!!! banned offline");
        return false;
    }
    auto* params = surfaceDrawable->GetRenderParams().get();
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(params);
    RS_LOGD("RSGPUOfflineDevice::PostProcessOfflineTask task[%{public}" PRIu64 "-%{public}" PRIu64 "] by drawable",
        taskId.first, taskId.second);
    return PostOfflineTaskCommon(offlineContext, surfaceParams, taskId);
}

bool RSGPUOfflineDevice::PostOfflineTaskCommon(std::shared_ptr<GPUOfflineContext>& offlineContext,
    RSSurfaceRenderParams* surfaceParams, offlineTaskId taskId)
{
    if (offlineContext->skipDraw) {
        return SetResultWhenSkipDraw(offlineContext, surfaceParams, taskId);
    }
    GPUOfflineSubThreadData taskContext;
    taskContext.nodeId = offlineContext->nodeId;
    taskContext.offlineBuffer = offlineContext->offlineBuffer;
    taskContext.drawParams = offlineContext->drawParams;

    auto futurePtr = offlineResultSync_.RegisterPostedTask(taskId);
    if (!futurePtr) {
        RS_LOGE("RSGPUOfflineDevice::RegisterPostedTask failed!");
        return false;
    }
    offlineThread_.PostTask([surfaceParams, futurePtr, taskId, this,
                             taskContext = std::move(taskContext)]() mutable {
        RS_TRACE_NAME("RSGPUOfflineDevice::ProcessOffline");
        RS_LOGD("RSGPUOfflineDevice::Start to process offline surface, task[%{public}" PRIu64 "-%{public}" PRIu64 "]",
            taskId.first, taskId.second);
        OfflineTaskFuncWithData(surfaceParams, futurePtr, taskContext);
    });
    return true;
}

bool RSGPUOfflineDevice::FillOfflineResult(std::shared_ptr<RSGPUOfflineBuffer>& offlineBuffer,
    RSSurfaceRenderParams* surfaceParams, ProcessOfflineResult& result)
{
    if (offlineBuffer == nullptr) {
        return false;
    }
    result.isGPUOffline = true;
    auto offlineSurfaceHandler = offlineBuffer->GetSurfaceHandler();
    result.buffer = offlineBuffer->ConsumeAndGetBuffer();
    if (result.buffer == nullptr) {
        RS_LOGE("RSGPUOfflineDevice::FillOfflineResult buffer is nullptr");
        return false;
    }
    result.preBuffer = offlineSurfaceHandler->GetPreBuffer();
    result.acquireFence = offlineSurfaceHandler->GetAcquireFence();
    result.bufferOwnerCount = offlineSurfaceHandler->GetBufferOwnerCount();
    result.consumer = offlineSurfaceHandler->GetConsumer();
    auto damageRect = offlineSurfaceHandler->GetDamageRegion();
    damageRect.y = result.buffer->GetHeight() - damageRect.y - damageRect.h;
    result.damageRect = damageRect;
    result.bufferRect = surfaceParams->GetLayerInfo().srcRect;
    result.transformType = surfaceParams->GetLayerInfo().transformType;
    return true;
}

bool RSGPUOfflineDevice::SetResultWhenSkipDraw(std::shared_ptr<GPUOfflineContext>& offlineContext,
    RSSurfaceRenderParams* surfaceParams, offlineTaskId taskId)
{
    RS_LOGD("RSGPUOfflineDevice::SetResultWhenSkipDraw task[%{public}" PRIu64 "-%{public}" PRIu64 "]",
        taskId.first, taskId.second);
    ProcessOfflineResult result;
    auto offlineBuffer = offlineContext->offlineBuffer;
    bool fillOutRet = FillOfflineResult(offlineBuffer, surfaceParams, result);
    if (!fillOutRet) {
        RS_LOGW("RSGPUOfflineDevice::FillOfflineResult failed");
        return false;
    }
    result.preBuffer = nullptr; // need to set nullptr in skipdraw
    result.taskSuccess = true;
    offlineResultSync_.SetDirectResult(taskId, result);

    // set status after skipdraw
    offlineContext->invalidFrames = 0;
    offlineContext->hasDrawn = true;
    return true;
}

bool RSGPUOfflineDevice::WaitForProcessOfflineResult(offlineTaskId taskId,
    std::chrono::milliseconds timeout, ProcessOfflineResult& result)
{
    RS_TRACE_NAME("RSGPUOfflineDevice::WaitForProcessOfflineResult");
    bool waitSuccess = offlineResultSync_.WaitForTaskAndGetResult(taskId, timeout, result);
    auto context = GetOfflineContext(taskId.second);
    // Context may have been deleted by main thread via GetOrCreateOfflineContext when cache is full.
    // The task result is still valid, so we just return waitSuccess without updating context.
    if (context == nullptr) {
        return waitSuccess;
    }
    context->hasDrawn = result.taskSuccess;
    context->invalidFrames = result.taskSuccess ? 0 : context->invalidFrames + 1;
    return waitSuccess;
}

void RSGPUOfflineDevice::CheckAndPostClearOfflineResourceTask(const std::vector<uint64_t>& offlineNodeIds)
{
    std::vector<NodeId> nodesToRemove;
    {
        std::lock_guard<std::mutex> lock(cacheMutex_);
        for (auto& [nodeId, context] : offlineContextCache_) {
            if (context->invalidFrames >= MAX_NUM_INVALID_FRAME) {
                nodesToRemove.push_back(nodeId);
            }
            context->invalidFrames++;
        }
    }
    for (auto& nodeId : nodesToRemove) {
        TryRemoveContext(nodeId);
    }
}

void RSGPUOfflineDevice::TryRemoveContext(NodeId nodeId)
{
    std::unique_lock<std::mutex> tryLock(cacheMutex_);
    if (offlineResultSync_.GetResultPoolSizeByNode(nodeId) < 1) {
        offlineContextCache_.erase(nodeId);
        RS_LOGI("RSGPUOfflineDevice::GPU offline cache cleared for node %{public}" PRIu64, nodeId);
    }
}

bool RSGPUOfflineDevice::CanDeleteDevice()
{
    return offlineContextCache_.empty();
}

void RSGPUOfflineDevice::OfflineTaskFuncWithData(RSSurfaceRenderParams* surfaceParams,
    std::shared_ptr<ProcessOfflineFuture>& futurePtr, const GPUOfflineSubThreadData& taskContext)
{
    if (surfaceParams == nullptr) {
        return;
    }
    ProcessOfflineResult result;
    result.taskSuccess = DoProcessOfflineWithContext(*surfaceParams, result, taskContext);
    offlineResultSync_.MarkTaskDoneAndSetResult(futurePtr, result);
    CheckAndHandleTimeoutEvent(futurePtr, taskContext.nodeId);
}

void RSGPUOfflineDevice::CheckAndHandleTimeoutEvent(std::shared_ptr<ProcessOfflineFuture>& futurePtr, NodeId nodeId)
{
    std::lock_guard<std::mutex> lock(futurePtr->mtx);
    if (futurePtr->timeout) {
        RS_LOGW("RSGPUOfflineDevice::GPU timeout! disable offline in this scene");
        RS_TRACE_NAME("RSGPUOfflineDevice::Timeout! disable offline in this scene");
        auto context = GetOfflineContext(nodeId);
        if (context != nullptr) {
            context->timeout = true;
        }
    }
}

bool RSGPUOfflineDevice::CheckCondition(const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode)
{
    RS_LOGD("RSGPUOfflineDevice::CheckCondition");
    if (!RSSystemProperties::GetGPUOfflineEnabled() || !RSSystemProperties::GetXcomponentEdrEnabled()) {
        return false;
    }
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceNode->GetStagingRenderParams().get());
    auto surfaceHandler = surfaceNode->GetRSSurfaceHandler();
    if (!surfaceHandler || !surfaceParams) {
        return false;
    }

    auto srcBuffer = surfaceHandler->GetBuffer();
    if (!srcBuffer) {
        RS_LOGW("RSGPUOfflineDevice::GetBuffer failed!");
        return false;
    }
    if (!RSBaseHdrUtil::CheckIsHDRSelfProcessingBuffer(srcBuffer)) {
        RS_LOGD("RSGPUOfflineDevice::Not HDRSelfProcessingBuffer!");
        return false;
    }
    if (RSBaseHdrUtil::GetRGBA1010108Enabled()) {
        RS_LOGD("RSGPUOfflineDevice::Has b38");
        return false;
    }

    Rect crop = {0, 0, 0, 0};
    if (srcBuffer->GetCropMetadata(crop)) {
        RS_LOGD("RSGPUOfflineDevice::Has crop metadata!");
        return false;
    }
    if (surfaceParams->GetLayerInfo().transformType != GraphicTransformType::GRAPHIC_ROTATE_NONE &&
        surfaceParams->GetLayerInfo().transformType != GraphicTransformType::GRAPHIC_ROTATE_180) {
        RS_LOGD("RSGPUOfflineDevice::Has transform!");
        return false;
    }
    return true;
}

bool RSGPUOfflineDevice::UpdateContext(std::shared_ptr<RSSurfaceRenderNode>& surfaceNode,
    std::shared_ptr<GPUOfflineContext>& offlineContext)
{
    RS_LOGD("RSGPUOfflineDevice::UpdateContext");
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceNode->GetStagingRenderParams().get());
    auto surfaceHandler = surfaceNode->GetRSSurfaceHandler();
    if (!surfaceHandler || !surfaceParams) {
        return false;
    }
    // generate drawParams
    GPUOfflineDrawParams currentDrawParams = CollectDrawParams(surfaceNode, surfaceHandler, *surfaceParams);
    // skipDraw: buffer not consumed + previous frame drawn + same params
    bool bufferConsumed = surfaceHandler->IsCurrentFrameBufferConsumed();
    if (!bufferConsumed && offlineContext->hasDrawn &&
         offlineContext->drawParams == currentDrawParams &&
         currentDrawParams.switchType == SingleBufferMode::SINGLE_BUFFER_MODE_NONE) {
        offlineContext->skipDraw = true;
        RS_LOGD("RSGPUOfflineDevice::SkipDraw: reuse previous frame");
        return true;
    }
    // normal drawing
    offlineContext->skipDraw = false;
    offlineContext->hasDrawn = false;
    offlineContext->drawParams = currentDrawParams;
    RS_LOGD("RSGPUOfflineDevice::Need GPU offline process, bufferConsumed=%{public}d", bufferConsumed);
    return true;
}

SingleBufferMode RSGPUOfflineDevice::GetSingleBufferMode(
    const std::shared_ptr<RSSurfaceHandler> surfaceHandler)
{
    if (!surfaceHandler || !surfaceHandler->GetConsumer()) {
        return SingleBufferMode::SINGLE_BUFFER_MODE_NONE;
    }
    return surfaceHandler->GetConsumer()->GetAndResetSingleBufferMode();
}

GPUOfflineDrawParams RSGPUOfflineDevice::CollectDrawParams(std::shared_ptr<RSSurfaceRenderNode> surfaceNode,
    const std::shared_ptr<RSSurfaceHandler> surfaceHandler, RSSurfaceRenderParams& surfaceParams)
{
    GPUOfflineDrawParams drawParams;
    auto srcBuffer = surfaceParams.GetBuffer();
    if (srcBuffer == nullptr) {
        return drawParams;
    }
    drawParams.transformType = GraphicTransformType::GRAPHIC_ROTATE_NONE;
    drawParams.resultWidth = srcBuffer->GetWidth();
    drawParams.resultHeight = srcBuffer->GetHeight();
    drawParams.targetColorGamut = surfaceNode->GetColorSpace();

    drawParams.sdrNit = surfaceParams.GetSdrNit();
    drawParams.displayNit = surfaceParams.GetDisplayNit();
    drawParams.brightnessRatio = surfaceParams.GetBrightnessRatio();
    drawParams.layerLinearMatrix = surfaceParams.GetLayerLinearMatrix();
    drawParams.useBilinearInterpolation = surfaceParams.NeedBilinearInterpolation();
    drawParams.ignoreAlpha = surfaceParams.GetSurfaceBufferOpaque();
    drawParams.colorFollow = surfaceParams.GetColorFollow();
    drawParams.screenId = surfaceParams.GetScreenId();
    drawParams.hdrPresent = surfaceParams.GetHDRPresent();
    drawParams.gAlpha = static_cast<uint8_t>(std::clamp(surfaceParams.GetLayerInfo().alpha, 0.0f, 1.0f) * RGBA_MAX);
    drawParams.switchType = GetSingleBufferMode(surfaceHandler);
    return drawParams;
}

bool RSGPUOfflineDevice::DoProcessOfflineWithContext(
    RSSurfaceRenderParams& surfaceParams, ProcessOfflineResult& result, const GPUOfflineSubThreadData& taskContext)
{
    RS_TRACE_NAME_FMT("RSGPUOfflineDevice::DoProcessOfflineWithContext");
    RS_LOGD("RSGPUOfflineDevice::DoProcessOfflineWithContext start");
    auto srcBuffer = surfaceParams.GetBuffer();
    auto srcBufferOwnerCount = surfaceParams.GetBufferOwnerCount();
    if (!srcBuffer || !srcBufferOwnerCount) {
        RS_LOGW("RSGPUOfflineDevice::GetBuffer failed!");
        return false;
    }
    BufferOwnerCountGuard guard(srcBufferOwnerCount);
    auto offlineBuffer = taskContext.offlineBuffer;
    if (offlineBuffer == nullptr) {
        RS_LOGW("RSGPUOfflineDevice::OfflineBuffer is null");
        return false;
    }
    bool drawRet = DrawHDRImage(surfaceParams, taskContext);
    if (!drawRet) {
        RS_LOGW("RSGPUOfflineDevice::DrawHDRImage failed");
        return false;
    }
    bool fillOutRet = FillOfflineResult(offlineBuffer, &surfaceParams, result);
    if (!fillOutRet) {
        RS_LOGW("RSGPUOfflineDevice::FillOfflineResult failed");
        return false;
    }
    RS_LOGD("RSGPUOfflineDevice::offline process done, bufferRect: [%{public}d %{public}d %{public}d %{public}d]",
        result.bufferRect.x, result.bufferRect.y,
        result.bufferRect.w, result.bufferRect.h);
    return true;
}

bool RSGPUOfflineDevice::DrawHDRImage(RSSurfaceRenderParams& surfaceParams,
    const GPUOfflineSubThreadData& taskContext)
{
    auto offlineBuffer = taskContext.offlineBuffer;
    if (offlineBuffer == nullptr || surfaceParams.GetBuffer() == nullptr) {
        RS_LOGW("RSGPUOfflineDevice::OfflineBuffer is null");
        return false;
    }
    BufferRequestConfig bufferConfig = GetGPUBufferConfig(taskContext);
    auto engine = offlineThread_.GetRenderEngine();
    auto renderFrame = offlineBuffer->RequestFrame(engine, bufferConfig, false, taskContext.drawParams.switchType);
    if (renderFrame == nullptr) {
        RS_LOGW("RSGPUOfflineDevice::RequestFrame failed, force redraw next frame");
        return false;
    }

    auto canvas = renderFrame->GetCanvas();
    canvas->Save();
    canvas->Clear(Drawing::Color::COLOR_TRANSPARENT);
    canvas->SetHdrOn(taskContext.drawParams.hdrPresent);
    BufferDrawParam drawParams = CreateBufferDrawParam(taskContext);
    drawParams.acquireFence = surfaceParams.GetAcquireFence();
    drawParams.buffer = surfaceParams.GetBuffer();
    drawParams.threadIndex = canvas->GetParallelThreadId();
    canvas->ConcatMatrix(drawParams.matrix);
    engine->DrawImage(*canvas, drawParams);
    canvas->Restore();
    renderFrame->Flush();
    RS_LOGD("RSGPUOfflineDevice::DrawHDRImage finished");
    return true;
}

BufferDrawParam RSGPUOfflineDevice::CreateBufferDrawParam(const GPUOfflineSubThreadData& taskContext)
{
    BufferDrawParam params;
    params.useCPU = false;
    params.useBilinearInterpolation = taskContext.drawParams.useBilinearInterpolation;
    params.ignoreAlpha = taskContext.drawParams.ignoreAlpha;
    Drawing::Filter filter;
    filter.SetFilterQuality(Drawing::Filter::FilterQuality::LOW);
    params.paint.SetFilter(filter);
    params.paint.SetAlpha(taskContext.drawParams.gAlpha);
    params.screenId = taskContext.drawParams.screenId;

    params.matrix = Drawing::Matrix();
    params.srcRect = Drawing::Rect(0, 0, taskContext.drawParams.resultWidth,
        taskContext.drawParams.resultHeight);
    params.dstRect = params.srcRect;

#ifdef USE_VIDEO_PROCESSING_ENGINE
    params.targetColorGamut = taskContext.drawParams.targetColorGamut;
    params.sdrNits = taskContext.drawParams.sdrNit;
    params.tmoNits = taskContext.drawParams.displayNit;
    params.displayNits = params.tmoNits / std::pow(taskContext.drawParams.brightnessRatio, 2.2f);
    params.layerLinearMatrix = taskContext.drawParams.layerLinearMatrix;
    params.isHdrRedraw = true;
#endif
    params.colorFollow = taskContext.drawParams.colorFollow;
    return params;
}

BufferRequestConfig RSGPUOfflineDevice::GetGPUBufferConfig(const GPUOfflineSubThreadData& taskContext)
{
    BufferRequestConfig config = {};
    config.width = taskContext.drawParams.resultWidth;
    config.height = taskContext.drawParams.resultHeight;
    config.strideAlignment = 0x8;
    config.colorGamut = taskContext.drawParams.targetColorGamut;
    config.format = GRAPHIC_PIXEL_FMT_RGBA_8888;
    config.usage = BUFFER_USAGE_HW_RENDER | BUFFER_USAGE_HW_TEXTURE |
        BUFFER_USAGE_MEM_DMA | BUFFER_USAGE_HW_COMPOSER | BUFFER_USAGE_CPU_READ;
    config.timeout = 0;
    config.transform = GraphicTransformType::GRAPHIC_ROTATE_NONE;
    return config;
}

std::shared_ptr<GPUOfflineContext> RSGPUOfflineDevice::GetOrCreateOfflineContext(NodeId nodeId)
{
    std::unique_lock<std::mutex> lock(cacheMutex_);
    auto it = offlineContextCache_.find(nodeId);
    if (it != offlineContextCache_.end()) {
        it->second->timestamp = std::chrono::steady_clock::now().time_since_epoch().count();
        return it->second;
    }

    if (offlineContextCache_.size() >= MAX_CACHE_SIZE) {
        NodeId oldestId = FindOldestEntry();
        if (oldestId != 0) {
            offlineContextCache_.erase(oldestId);
            RS_LOGI("RSGPUOfflineDevice::TrimCache: force remove node %{public}" PRIu64, oldestId);
        }
    }

    if (offlineContextCache_.size() >= MAX_CACHE_SIZE) {
        RS_LOGI("RSGPUOfflineDevice::Can't create new context!");
        return nullptr;
    }
    auto buffer = std::make_shared<RSGPUOfflineBuffer>(std::to_string(nodeId), nodeId);
    auto offlineContext = std::make_shared<GPUOfflineContext>();
    offlineContext->nodeId = nodeId;
    offlineContext->offlineBuffer = buffer;
    offlineContext->timestamp = std::chrono::steady_clock::now().time_since_epoch().count();
    offlineContextCache_[nodeId] = offlineContext;
    return offlineContext;
}

NodeId RSGPUOfflineDevice::FindOldestEntry()
{
    NodeId oldestId = 0;
    int64_t oldestTimestamp = INT64_MAX;
    for (const auto& [id, ctx] : offlineContextCache_) {
        if (ctx->timestamp < oldestTimestamp) {
            oldestTimestamp = ctx->timestamp;
            oldestId = id;
        }
    }
    return oldestId;
}

std::shared_ptr<GPUOfflineContext> RSGPUOfflineDevice::GetOfflineContext(NodeId nodeId)
{
    std::lock_guard<std::mutex> lock(cacheMutex_);
    auto it = offlineContextCache_.find(nodeId);
    if (it != offlineContextCache_.end()) {
        return it->second;
    }
    return nullptr;
}

} // namespace Rosen
} // namespace OHOS