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

#include "feature/hwc/hpae_offline/rs_hpae_offline_processor.h"

#include <vector>
#include <cmath>
#include <future>

#include "hdi_layer.h"
#include "hdi_layer_info.h"
#include "display_engine/rs_luminance_control.h"
#include "rs_trace.h"
#include "string_utils.h"
#include "surface_type.h"

#include "common/rs_optional_trace.h"
#include "drawable/rs_display_render_node_drawable.h"
#include "drawable/rs_surface_render_node_drawable.h"
#include "params/rs_display_render_params.h"
#include "feature/hwc/hpae_offline/rs_hpae_offline_util.h"
#include "feature/uifirst/rs_sub_thread_manager.h"
#include "feature/round_corner_display/rs_rcd_surface_render_node.h"
#include "pipeline/render_thread/rs_uni_render_util.h"
#include "platform/common/rs_log.h"
#ifdef USE_VIDEO_PROCESSING_ENGINE
#include "metadata_helper.h"
#endif
namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t MAX_NUM_INVALID_FRAME = 120;
}

RSHpaeOfflineProcessor::RSHpaeOfflineProcessor()
{
    if (LoadPreProcessHandle()) {
        layerConfig_.strideAlignment = 0x08; // default stride is 8 Bytes.
        layerConfig_.format = GRAPHIC_PIXEL_FMT_RGBA_8888;
        layerConfig_.usage = BUFFER_USAGE_HW_COMPOSER | BUFFER_USAGE_MEM_DMA | BUFFER_USAGE_CPU_READ;
        layerConfig_.timeout = 0;
    }
}

RSHpaeOfflineProcessor::~RSHpaeOfflineProcessor()
{
    if (preProcessHandle_) {
        dlclose(preProcessHandle_);
        preProcessHandle_ = nullptr;
    }
}

std::shared_ptr<RSHpaeOfflineProcessor> RSHpaeOfflineProcessor::GetOfflineProcessor()
{
    static std::shared_ptr<RSHpaeOfflineProcessor> processor = 
        std::shared_ptr<RSHpaeOfflineProcessor>(new RSHpaeOfflineProcessor());
    return processor;
}

bool RSHpaeOfflineProcessor::LoadPreProcessHandle()
{
    loadSuccess_ = false;
    RS_TRACE_NAME_FMT("hpae_offline: LoadPreProcessHandle");
    preProcessHandle_ = dlopen("libdss_enhance.z.so", RTLD_NOW);
    if (preProcessHandle_ == nullptr) {
        RS_OFFLINE_LOGW("[%{public}s_%{public}d]: load library failed, reason: %{public}s",
            __func__, __LINE__, dlerror());
        return false;
    }
    preProcessFunc_ = reinterpret_cast<ProcessOfflineFunc>(dlsym(preProcessHandle_, "ProcessOfflineSurface"));
    if (preProcessFunc_ == nullptr) {
        RS_OFFLINE_LOGW("[%{public}s_%{public}d]: load ProcessOfflineSurface failed, reason: %{public}s",
            __func__, __LINE__, dlerror());
        dlclose(preProcessHandle_);
        preProcessHandle_ = nullptr;
        return false;
    }
    getConfigFunc_ = reinterpret_cast<GetOfflineConfigFunc>(dlsym(preProcessHandle_, "GetOfflineConfig"));
    if (getConfigFunc_ == nullptr) {
        RS_OFFLINE_LOGW("[%{public}s_%{public}d]: load GetOfflineConfig failed, reason: %{public}s",
            __func__, __LINE__, dlerror());
        dlclose(preProcessHandle_);
        preProcessHandle_ = nullptr;
        return false;
    }
    RS_OFFLINE_LOGI("[%{public}s_%{public}d]: load success", __func__, __LINE__);
    loadSuccess_ = true;
    return true;
}

bool RSHpaeOfflineProcessor::InitForOfflineProcess()
{
    int32_t ret = -1;
    OfflineProcessOutputInfo outputInfo;
    // get offline buffer config and rect from prevalidate
    if (getConfigFunc_) {
        ret = getConfigFunc_(outputInfo);
    }
    if (ret != 0) {
        RS_OFFLINE_LOGW("getConfigFunc_ call failed!");
        return false;
    }

    std::lock_guard<std::mutex> localLock(offlineConfigMutex_);
    layerConfig_.width = outputInfo.bufConfig.width;
    layerConfig_.height = outputInfo.bufConfig.height;
    layerConfig_.format = outputInfo.bufConfig.format;
    layerConfig_.usage = outputInfo.bufConfig.usage;
    layerConfig_.timeout = outputInfo.bufConfig.timeout;
    layerConfig_.colorGamut = static_cast<GraphicColorGamut>(outputInfo.bufConfig.colorGamut);
    if (layerConfig_.width <= 0 || layerConfig_.height <= 0) {
        RS_OFFLINE_LOGW("Layer config is invalid! Width/height is zero");
        return false;
    }
    offlineRect_ = outputInfo.outRect;
    RS_OFFLINE_LOGD("Got hpae offline config, layerconfig.size: %{public}d, %{public}d, "
        "format: %{public}d, colorGamut: %{public}d, timeout: %{public}d,"
        "offline rect: [%{public}d, %{public}d, %{public}d, %{public}d]",
        layerConfig_.width, layerConfig_.height, layerConfig_.format, layerConfig_.colorGamut,
        layerConfig_.timeout, offlineRect_.x, offlineRect_.y, offlineRect_.w, offlineRect_.h);
    return true;
}

bool RSHpaeOfflineProcessor::IsRSHpaeOfflineProcessorReady()
{
    if (!loadSuccess_) {
        RS_OFFLINE_LOGW("hape so is not loaded.");
        return false;
    }

    if (timeout_) {
        RS_OFFLINE_LOGE("This scene has experienced a timeout event!!! banned hpae offline");
        return false;
    }

    if (!InitForOfflineProcess()) {
        RS_OFFLINE_LOGW("Get offline config failed.");
        return false;
    }

    if (!preAllocBufferSucc_) {
        RS_OFFLINE_LOGD("Buffers is not warmed up.");
        CheckAndPostPreAllocBuffersTask();
        return false;
    }

    return true;
}

void RSHpaeOfflineProcessor::CheckAndPostPreAllocBuffersTask()
{
    if (offlineResultSync_.GetResultPoolSize() < 1 && !isBusy_) {
        RS_OFFLINE_LOGD("Start to post Preallocbuffer task.");
        auto func = [this]() mutable {
            RS_TRACE_NAME_FMT("hpae_offline: PreAllocBuffer.");
            isBusy_ = true;
            bool ret = offlineLayer_.PreAllocBuffers(layerConfig_);
            preAllocBufferSucc_ = ret;
            isBusy_ = false;
        };
        offlineThreadManager_.PostTask(func);
    }
}

bool RSHpaeOfflineProcessor::GetOfflineProcessInput(RSSurfaceRenderParams& params,
    OfflineProcessInputInfo& inputInfo, sptr<SurfaceBuffer>& dstSurfaceBuffer, int32_t& releaseFence)
{
    // get offline buffer
    RS_OPTIONAL_TRACE_NAME_FMT("hpae_offline: Get Offline Process Input Info");
    std::lock_guard<std::mutex> configLock(offlineConfigMutex_);
    dstSurfaceBuffer = offlineLayer_.RequestSurfaceBuffer(layerConfig_, releaseFence);
    if (!dstSurfaceBuffer) {
        RS_OFFLINE_LOGW("RS offline dstSurfaceHandler get buffer failed! layerConfig_: %{public}d, %{public}d",
            layerConfig_.width, layerConfig_.height);
        return false;
    }

    BufferHandle *dstHandle = dstSurfaceBuffer->GetBufferHandle();
    if (!dstHandle) {
        RS_OFFLINE_LOGW("Offline buffer handle is not available.");
        return false;
    }

    inputInfo.id = params.GetId();
    auto srcSurfaceBuffer = params.GetBuffer();
    if (!srcSurfaceBuffer) {
        RS_OFFLINE_LOGW("Offline srcSurfaceBuffer get buffer failed!");
        return false;
    }
    inputInfo.srcHandle = srcSurfaceBuffer->GetBufferHandle();
    inputInfo.dstHandle = dstHandle;
    auto src = params.GetLayerInfo().srcRect;
    Rect crop{0, 0, 0, 0};
    if (srcSurfaceBuffer->GetCropMetadata(crop)) {
        float scaleX = static_cast<float>(crop.w) / srcSurfaceBuffer->GetWidth();
        float scaleY = static_cast<float>(crop.h) / srcSurfaceBuffer->GetHeight();
        inputInfo.srcRect = {
            static_cast<uint32_t>(std::ceil(src.x * scaleX)),
            static_cast<uint32_t>(std::ceil(src.y * scaleY)),
            static_cast<uint32_t>(std::floor(src.w * scaleX)),
            static_cast<uint32_t>(std::floor(src.h * scaleY))
        };
    } else {
        inputInfo.srcRect = {src.x, src.y, src.w, src.h};
    }
    inputInfo.dstRect = {offlineRect_.x, offlineRect_.y, offlineRect_.w, offlineRect_.h};
    inputInfo.transform = static_cast<int>(params.GetLayerInfo().transformType);
    return true;
}

void RSHpaeOfflineProcessor::FlushAndReleaseOfflineLayer(sptr<SurfaceBuffer>& dstSurfaceBuffer)
{
    // release buffer
    flushConfig_.timestamp = 0;
    offlineLayer_.FlushSurfaceBuffer(dstSurfaceBuffer, -1, flushConfig_);
    IConsumerSurface::AcquireBufferReturnValue returnValue;
    returnValue.fence = SyncFence::InvalidFence();
    auto surfaceHandler = offlineLayer_.GetMutableRSSurfaceHandler();
    const auto surfaceConsumer = surfaceHandler->GetConsumer();
    int32_t ret = surfaceConsumer->AcquireBuffer(returnValue, 0, false);
    if (ret != OHOS::SURFACE_ERROR_OK) {
        RS_OFFLINE_LOGW("RSBaseRenderUtil::DropFrameProcess(node: %{public}" PRIu64 "): AcquireBuffer failed("
            " ret: %{public}d), do nothing ", surfaceHandler->GetNodeId(), ret);
        return;
    }

    ret = surfaceConsumer->ReleaseBuffer(returnValue.buffer, returnValue.fence);
    if (ret != OHOS::SURFACE_ERROR_OK) {
        RS_OFFLINE_LOGW("RSBaseRenderUtil::DropFrameProcess(node: %{public}" PRIu64
            "): ReleaseBuffer failed(ret: %{public}d), Acquire done ",
            surfaceHandler->GetNodeId(), ret);
    }
    surfaceHandler->SetAvailableBufferCount(static_cast<int32_t>(surfaceConsumer->GetAvailableBufferCount()));
    RS_OFFLINE_LOGD("RSBaseRenderUtil::DropFrameProcess (node: %{public}" PRIu64 "), drop one frame",
        surfaceHandler->GetNodeId());
}

void RSHpaeOfflineProcessor::CheckAndHandleTimeoutEvent(std::weak_ptr<ProcessOfflineStatus> statusWeakPtr)
{
    if (auto status = statusWeakPtr.lock()) {
        {
            std::lock_guard<std::mutex> lock(status->mtx);
            if (status->timeout) {
                // to self-recovery from HPAE failed, once timeout, offline will be disabled until scene changed
                RS_OFFLINE_LOGE("hpae timeout! disable offline in this scene");
                timeout_ = true;
            }
        }
    }
}

bool RSHpaeOfflineProcessor::DoProcessOffline(
    RSSurfaceRenderParams& params, ProcessOfflineResult& processOfflineResult)
{
    RS_OPTIONAL_TRACE_NAME_FMT("hpae_offline: do process offline");
    invalidFrames_ = 0;
    OfflineProcessInputInfo inputInfo;
    sptr<SurfaceBuffer> dstSurfaceBuffer = nullptr;
    int32_t releaseFence = -1;
    if (!GetOfflineProcessInput(params, inputInfo, dstSurfaceBuffer, releaseFence)) {
        RS_OFFLINE_LOGW("Get offline process input failed.");
        return false;
    }

    {
        RS_OPTIONAL_TRACE_NAME_FMT("hpae_offline: Wait Offline Acquire & Release Fence.");
        sptr<OHOS::SyncFence> releaseSyncFence = new OHOS::SyncFence(releaseFence);
        releaseSyncFence->Wait(-1);
        sptr<OHOS::SyncFence> acquireSyncFence = params.GetAcquireFence();
        acquireSyncFence->Wait(-1);
    }

    int32_t ret = -1;
    if (preProcessFunc_) {
        ret = preProcessFunc_(inputInfo);
    }
    if (ret != 0) {
        RS_OFFLINE_LOGW("Hpae offline process fail.");
        FlushAndReleaseOfflineLayer(dstSurfaceBuffer);
        return false;
    }

    flushConfig_.timestamp = 0;
    flushConfig_.damage = {.x = offlineRect_.x, .y = offlineRect_.y, .w = offlineRect_.w, .h = offlineRect_.h};
    offlineLayer_.FlushSurfaceBuffer(dstSurfaceBuffer, -1, flushConfig_);

    auto offlineSurfaceHandler = offlineLayer_.GetMutableRSSurfaceHandler();
    if (!RSBaseRenderUtil::ConsumeAndUpdateBuffer(*offlineSurfaceHandler) || !offlineSurfaceHandler->GetBuffer()) {
        RS_OFFLINE_LOGW("DeviceOfflineLayer consume buffer failed. %{public}d",
            !offlineSurfaceHandler->GetBuffer());
        return false;
    }
    processOfflineResult.bufferRect = {
        .x = offlineRect_.x, .y = offlineRect_.y, .w = offlineRect_.w, .h = offlineRect_.h};
    processOfflineResult.consumer = offlineSurfaceHandler->GetConsumer();
    auto damageRect = offlineSurfaceHandler->GetDamageRegion();
    damageRect.y = offlineSurfaceHandler->GetBuffer()->GetHeight() - damageRect.y - damageRect.h;
    processOfflineResult.damageRect = damageRect;
    processOfflineResult.preBuffer = offlineSurfaceHandler->GetPreBuffer();
    processOfflineResult.buffer = offlineSurfaceHandler->GetBuffer();
    processOfflineResult.acquireFence = offlineSurfaceHandler->GetAcquireFence();
    RS_OFFLINE_LOGD("Offline process done, bufferRect: [%{public}d %{public}d %{public}d %{public}d]",
        processOfflineResult.bufferRect.x, processOfflineResult.bufferRect.y,
        processOfflineResult.bufferRect.w, processOfflineResult.bufferRect.h);
    return true;
}

bool RSHpaeOfflineProcessor::PostProcessOfflineTask(RSSurfaceRenderNode& node, uint64_t taskId)
{
    if (!loadSuccess_) {
        RS_OFFLINE_LOGW("hape so is not loaded.");
        return false;
    }
    // while doing direct composition, there is no IsRSHpaeOfflineProcessorReady to check status
    if (timeout_) {
        RS_OFFLINE_LOGE("This scene has experienced a timeout event!!! banned hpae offline");
        return false;
    }
    auto statusWeakPtr = offlineResultSync_.RegisterPostedTask(taskId);
    auto func = [&node, statusWeakPtr, this]() mutable {
        RS_TRACE_NAME("hpae_offline: ProcessOffline");
        RS_OFFLINE_LOGD("start to proces offline surface (by node)");
        isBusy_ = true;
        ProcessOfflineResult processOfflineResult;
        auto& param = *static_cast<RSSurfaceRenderParams*>(node.GetStagingRenderParams().get());
        processOfflineResult.flag = DoProcessOffline(param, processOfflineResult);
        offlineResultSync_.MarkTaskDoneAndSetResult(statusWeakPtr, processOfflineResult);
        CheckAndHandleTimeoutEvent(statusWeakPtr);
        isBusy_ = false;
    };
    offlineThreadManager_.PostTask(func);
    return true;
}

bool RSHpaeOfflineProcessor::PostProcessOfflineTask(
    DrawableV2::RSSurfaceRenderNodeDrawable& surfaceDrawable, uint64_t taskId)
{
    if (!loadSuccess_) {
        RS_OFFLINE_LOGW("hape so is not loaded.");
        return false;
    }
    // while posting offline task in rt thread, there is prevalidate to avoid piling up, post directly
    auto statusWeakPtr = offlineResultSync_.RegisterPostedTask(taskId);
    auto func = [&surfaceDrawable, statusWeakPtr, taskId, this]() mutable {
        RS_TRACE_NAME("hpae_offline: ProcessOffline");
        RS_OFFLINE_LOGD("start to proces offline surface (by drawable)");
        isBusy_ = true;
        ProcessOfflineResult processOfflineResult;
        auto& param = *static_cast<RSSurfaceRenderParams*>(surfaceDrawable.GetRenderParams().get());
        processOfflineResult.flag = DoProcessOffline(param, processOfflineResult);
        offlineResultSync_.MarkTaskDoneAndSetResult(statusWeakPtr, processOfflineResult);
        CheckAndHandleTimeoutEvent(statusWeakPtr);
        isBusy_ = false;
    };
    offlineThreadManager_.PostTask(func);
    return true;
}

void RSHpaeOfflineProcessor::CheckAndPostClearOfflineResourceTask()
{
    if (!loadSuccess_) {
        RS_OFFLINE_LOGW("hape so is not loaded.");
        return;
    }
    if (offlineResultSync_.GetResultPoolSize() < 1 && preAllocBufferSucc_ && !isBusy_) {
        invalidFrames_++;
        if (invalidFrames_ < MAX_NUM_INVALID_FRAME) {
            return;
        }
        auto func = [this]() mutable {
            RS_TRACE_NAME("hpae_offline: Post ClearOfflineCacheTask.");
            RS_OFFLINE_LOGD("The number of disabled frames exceeds threshold, clean buffers.");
            isBusy_ = true;
            offlineLayer_.CleanCache(true);
            offlineResultSync_.ClearResultPool();
            invalidFrames_ = 0;
            preAllocBufferSucc_ = false;
            isBusy_ = false;
            timeout_ = false;
        };
        offlineThreadManager_.PostTask(func);
    }
}

bool RSHpaeOfflineProcessor::WaitForProcessOfflineResult(uint64_t taskId,
    ProcessOfflineResult& processOfflineResult, std::chrono::milliseconds timeout)
{
    if (!loadSuccess_) {
        RS_OFFLINE_LOGW("hape so is not loaded.");
        return false;
    }
    RS_TRACE_NAME("hpae_offline: Wait for node offline process");
    return offlineResultSync_.WaitForTaskAndGetResult(taskId, processOfflineResult, timeout);
}
} // namespace Rosen
} // namespace OHOS