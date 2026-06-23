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

#include "feature/hwc/hpae_offline/rs_hpae_offline_device.h"

#include <cmath>
#include <future>
#include <vector>

#include "display_engine/rs_luminance_control.h"
#include "rs_trace.h"
#include "string_utils.h"
#include "surface_type.h"

#include "common/rs_optional_trace.h"
#include "drawable/rs_screen_render_node_drawable.h"
#include "drawable/rs_surface_render_node_drawable.h"
#include "feature/hwc/hpae_offline/rs_offline_util.h"
#include "feature/uifirst/rs_sub_thread_manager.h"
#include "params/rs_screen_render_params.h"
#include "pipeline/render_thread/rs_uni_render_util.h"
#include "platform/common/rs_log.h"
#ifdef USE_VIDEO_PROCESSING_ENGINE
#include "metadata_helper.h"
#endif
#include "feature/hdr/rs_hdr_util.h"
namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t SCALE_MAX_NUM_INVALID_FRAME = 5;
constexpr size_t AI2020_MAX_NUM_INVALID_FRAME = 5;
constexpr uint32_t WAIT_FENCE_TIMEOUT_MS = 500;
constexpr size_t MAX_CACHE_SIZE = 2;
constexpr size_t DISABLE_ARSR_NODE_NUM = 2;
}

RSHpaeOfflineContext::RSHpaeOfflineContext(OfflineContextType type)
{
    contextType = type;
    switch (type) {
        case OfflineContextType::SCALE:
            offlineLayer = std::make_shared<RSHpaeOfflineLayer>("DeviceOfflineLayer_SCALE", INVALID_NODEID);
            maxInvalidFrames = SCALE_MAX_NUM_INVALID_FRAME;
            break;
        case OfflineContextType::AI2020:
            offlineLayer = std::make_shared<RSHpaeOfflineLayer>("DeviceOfflineLayer_AI2020", INVALID_NODEID);
            preAllocBufferSucc = true;
            maxInvalidFrames = AI2020_MAX_NUM_INVALID_FRAME;
            break;
        default:
            RS_LOGI("Invalid offline context type!");
    }
}

RSHpaeOfflineContext::~RSHpaeOfflineContext()
{
}

bool RSHpaeOfflineContext::isSkipDraw()
{
    return contextType == OfflineContextType::AI2020 ? skipDraw.load() : false;
}


RSHpaeOfflineDevice::RSHpaeOfflineDevice()
{
    LoadPreProcessHandle();
}

RSHpaeOfflineDevice::~RSHpaeOfflineDevice()
{
    if (preProcessHandle_) {
        dlclose(preProcessHandle_);
        preProcessHandle_ = nullptr;
    }
}

void RSHpaeOfflineDevice::CloseOfflineHandle(const std::string& errSymbol, const char* errNo)
{
    RS_OFFLINE_LOGW("load %{public}s failed, reason: %{public}s", errSymbol.c_str(), errNo);
    dlclose(preProcessHandle_);
    preProcessHandle_ = nullptr;
}

void RSHpaeOfflineDevice::LoadPreProcessHandle()
{
    loadSuccess_ = false;
    RS_TRACE_NAME_FMT("hpae_offline: LoadPreProcessHandle");
    preProcessHandle_ = dlopen("libprevalidate_client.z.so", RTLD_NOW);
    if (preProcessHandle_ == nullptr) {
        RS_OFFLINE_LOGW("[%{public}s]: load library failed, reason: %{public}s", __func__, dlerror());
        return;
    }

    PreValidateInitFunc initFunc = reinterpret_cast<PreValidateInitFunc>(dlsym(preProcessHandle_, "InitPrevalidate"));
    if ((initFunc == nullptr) || (initFunc() != 0)) {
        RS_OFFLINE_LOGW("prevalidate init failed");
        dlclose(preProcessHandle_);
        return;
    }

    preProcessFunc_ = reinterpret_cast<ProcessOfflineFunc>(dlsym(preProcessHandle_, "ProcessOfflineSurface"));
    if (preProcessFunc_ == nullptr) {
        CloseOfflineHandle("ProcessOfflineSurface", dlerror());
        return;
    }
    getConfigFunc_ = reinterpret_cast<GetOfflineConfigFunc>(dlsym(preProcessHandle_, "GetOfflineConfig"));
    if (getConfigFunc_ == nullptr) {
        CloseOfflineHandle("GetOfflineConfig", dlerror());
        return;
    }
    initOfflineFunc_ = reinterpret_cast<InitOfflineResourceFunc>(dlsym(preProcessHandle_, "InitOfflineResource"));
    if (initOfflineFunc_ == nullptr) {
        CloseOfflineHandle("InitOfflineResource", dlerror());
        return;
    }
    deInitOfflineFunc_ = reinterpret_cast<DeInitOfflineResourceFunc>(dlsym(preProcessHandle_, "DeInitOfflineResource"));
    if (deInitOfflineFunc_ == nullptr) {
        CloseOfflineHandle("DeInitOfflineResource", dlerror());
        return;
    }
    getFenceFunc_ = reinterpret_cast<GetFenceFunc>(dlsym(preProcessHandle_, "GetOfflineOutputFence"));
    if (getFenceFunc_ == nullptr) {
        CloseOfflineHandle("GetOfflineOutputFence", dlerror());
        return;
    }
    RS_OFFLINE_LOGI("[%{public}s]: load success", __func__);
    loadSuccess_ = true;
}

bool RSHpaeOfflineDevice::GetOutputConfig(std::shared_ptr<RSHpaeOfflineContext>& context,
    std::shared_ptr<RSSurfaceRenderNode>& node)
{
    int32_t ret = -1;
    OfflineProcessOutputInfo outputInfo;
    // get offline buffer config and rect from prevalidate
    if (getConfigFunc_) {
        ret = getConfigFunc_(node->GetId(), outputInfo);
    }
    if (ret != 0) {
        RS_OFFLINE_LOGW("getConfigFunc_ call failed!");
        return false;
    }
    std::lock_guard<std::mutex> localLock(context->offlineConfigMutex);
    context->layerConfig = {
        .width = outputInfo.bufferConfig.width,
        .height = outputInfo.bufferConfig.height,
        .format = outputInfo.bufferConfig.format,
        .usage = outputInfo.bufferConfig.usage,
        .timeout = outputInfo.bufferConfig.timeout,
        .colorGamut = static_cast<GraphicColorGamut>(outputInfo.bufferConfig.colorGamut),
    };
    if (context->layerConfig.width <= 0 || context->layerConfig.height <= 0) {
        RS_OFFLINE_LOGW("Layer config is invalid! Width/height is zero");
        return false;
    }
    context->offlineRect = outputInfo.outRect;
    RS_OFFLINE_LOGD("Got hpae offline config, layerConfig.size: %{public}d, %{public}d, "
        "format: %{public}d, colorGamut: %{public}d, timeout: %{public}d, "
        "offline rect: [%{public}d, %{public}d, %{public}d, %{public}d]",
        context->layerConfig.width, context->layerConfig.height,
        context->layerConfig.format,
        context->layerConfig.colorGamut,
        context->layerConfig.timeout,
        context->offlineRect.x, context->offlineRect.y,
        context->offlineRect.w, context->offlineRect.h);
    return true;
}

bool RSHpaeOfflineDevice::IsRSOfflineDeviceReady(std::shared_ptr<RSSurfaceRenderNode>& surfaceNode)
{
    bool offlineDeviceEnable = RSSystemProperties::GetHpaeOfflineEnabled();
    if (!offlineDeviceEnable) {
        RS_OFFLINE_LOGD("HpaeOffline is not open, offlineProcessorEnabled: %{public}d.",
            offlineDeviceEnable);
            return false;
    }
    if (!loadSuccess_) {
        RS_OFFLINE_LOGW("hpae so is not loaded.");
        return false;
    }
    std::shared_ptr<RSHpaeOfflineContext> context = GetOrCreateOfflineContext(surfaceNode);
    if (context == nullptr) {
        RS_OFFLINE_LOGI("context is nullptr, banned offline!");
        return false;
    }
    if (context->timeout) {
        RS_OFFLINE_LOGI("This scene has experienced a timeout event!!! banned hpae offline");
        return false;
    }
    if (!UpdateContext(surfaceNode, context)) {
        RS_OFFLINE_LOGI("RSHpaeOfflineDevice::UpdateContext failed");
        return false;
    }
    if (!GetOutputConfig(context, surfaceNode)) {
        RS_OFFLINE_LOGW("Get offline config failed.");
        return false;
    }
    InitHpaeOfflineResource();
    if (!context->preAllocBufferSucc) {
        RS_OFFLINE_LOGD("Buffers is not warmed up.");
        CheckAndPostPreAllocBuffersTask(context);
        return false;
    }
    return true;
}

void RSHpaeOfflineDevice::CheckAndPostPreAllocBuffersTask(std::shared_ptr<RSHpaeOfflineContext>& context)
{
    if (offlineResultSync_.GetResultPoolSize() < 1) {
        RS_OFFLINE_LOGD("Start to post Preallocbuffer task.");
        BufferRequestConfig config;
        {
            std::lock_guard<std::mutex> localLock(context->offlineConfigMutex);
            config = context->layerConfig;
        }
        offlineThreadManager_.PostTask([this, context, config = std::move(config)]() mutable {
            if (context->preAllocBufferSucc == true) {
                return;
            }
            RS_TRACE_NAME_FMT("hpae_offline: PreAllocBuffer.");
            bool ret = context->offlineLayer->PreAllocBuffers(config);
            context->preAllocBufferSucc = ret;
        });
    }
}

void RSHpaeOfflineDevice::InitHpaeOfflineResource()
{
    offlineThreadManager_.PostTask([this]() mutable {
        if (!initOfflineFunc_) {
            RS_OFFLINE_LOGW("initOfflineFunc_ is nullptr, disable offline.");
            isInitOfflineFuncSucc_ = false;
            return;
        }
        if (!isInitOfflineFuncSucc_) {
            if (initOfflineFunc_() != 0) {
                RS_OFFLINE_LOGW("initOfflineFunc_ failed, disable offline.");
                isInitOfflineFuncSucc_ = false;
                return;
            }
            isInitOfflineFuncSucc_ = true;
        }
    });
}

bool RSHpaeOfflineDevice::GetOfflineProcessInput(RSSurfaceRenderParams& params, OfflineProcessInputInfo& inputInfo,
    sptr<SurfaceBuffer>& dstSurfaceBuffer, int32_t& releaseFence, HpaeOfflineSubThreadData& taskData)
{
    // get offline buffer
    RS_OPTIONAL_TRACE_NAME_FMT("hpae_offline: Get Offline Process Input Info");
    dstSurfaceBuffer = taskData.offlineLayer->RequestSurfaceBuffer(taskData.layerConfig, releaseFence);
    if (!dstSurfaceBuffer) {
        RS_OFFLINE_LOGW("RS offline dstSurfaceHandler get buffer failed! layerConfig_: %{public}d, %{public}d",
            taskData.layerConfig.width, taskData.layerConfig.height);
        return false;
    }

    BufferHandle* dstHandle = dstSurfaceBuffer->GetBufferHandle();
    if (!dstHandle) {
        RS_OFFLINE_LOGW("Offline buffer handle is not available.");
        return false;
    }
    auto srcSurfaceBuffer = params.GetBuffer();
    inputInfo.id = params.GetId();
    inputInfo.srcHandle = srcSurfaceBuffer->GetBufferHandle();
    inputInfo.dstHandle = dstHandle;
    auto src = params.GetLayerInfo().srcRect;
    if (taskData.contextType == OfflineContextType::AI2020) {
        inputInfo.srcRect = {taskData.offlineRect.x, taskData.offlineRect.y,
            taskData.offlineRect.w, taskData.offlineRect.h};
    } else {
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
    }
    inputInfo.dstRect = {taskData.offlineRect.x, taskData.offlineRect.y,
        taskData.offlineRect.w, taskData.offlineRect.h};
    inputInfo.transform = static_cast<uint32_t>(params.GetLayerInfo().transformType);
    return true;
}

void RSHpaeOfflineDevice::FlushAndReleaseOfflineLayer(sptr<SurfaceBuffer>& dstSurfaceBuffer,
    HpaeOfflineSubThreadData& taskData)
{
    // release buffer
    taskData.flushConfig.timestamp = 0;
    taskData.offlineLayer->FlushSurfaceBuffer(dstSurfaceBuffer, -1, taskData.flushConfig);
    IConsumerSurface::AcquireBufferReturnValue returnValue;
    returnValue.fence = SyncFence::InvalidFence();
    auto surfaceHandler = taskData.offlineLayer->GetMutableRSSurfaceHandler();
    const auto surfaceConsumer = surfaceHandler->GetConsumer();
    int32_t ret = surfaceConsumer->AcquireBuffer(returnValue, 0, false);
    if (ret != OHOS::SURFACE_ERROR_OK) {
        RS_OFFLINE_LOGW("RSBaseSurfaceUtil::DropFrameProcess(node: %{public}" PRIu64 "): AcquireBuffer failed("
            " ret: %{public}d), do nothing ", surfaceHandler->GetNodeId(), ret);
        return;
    }

    ret = surfaceConsumer->ReleaseBuffer(returnValue.buffer, returnValue.fence);
    if (ret != OHOS::SURFACE_ERROR_OK) {
        RS_OFFLINE_LOGW("RSBaseSurfaceUtil::DropFrameProcess(node: %{public}" PRIu64
            "): ReleaseBuffer failed(ret: %{public}d), Acquire done ",
            surfaceHandler->GetNodeId(), ret);
    }
    surfaceHandler->SetAvailableBufferCount(static_cast<int32_t>(surfaceConsumer->GetAvailableBufferCount()));
    RS_OFFLINE_LOGD("RSBaseSurfaceUtil::DropFrameProcess (node: %{public}" PRIu64 "), drop one frame",
        surfaceHandler->GetNodeId());
}

void RSHpaeOfflineDevice::CheckAndHandleTimeoutEvent(std::shared_ptr<ProcessOfflineFuture>& futurePtr, NodeId nodeId)
{
    std::lock_guard<std::mutex> lock(futurePtr->mtx);
    if (futurePtr->timeout) {
        // to self-recovery from HPAE failed, once timeout, offline will be disabled until scene changed
        RS_OFFLINE_LOGW("hpae timeout! disable offline in this scene");
        auto context = GetOfflineContext(nodeId);
        if (context != nullptr) {
            context->timeout = true;
        }
    }
}

static void WaitFence(const sptr<SyncFence>& srcAcquireFence, int32_t releaseFenceFd)
{
    // wait acquire fence of source buffer and release fence of offline buffer
    RS_OPTIONAL_TRACE_NAME_FMT("hpae_offline: Wait Offline Acquire & Release Fence.");
    RS_OFFLINE_LOGD("start to wait fence.");
    sptr<SyncFence> dstReleaseFence = sptr<SyncFence>::MakeSptr(releaseFenceFd);
    dstReleaseFence->Wait(WAIT_FENCE_TIMEOUT_MS);
    srcAcquireFence->Wait(WAIT_FENCE_TIMEOUT_MS);
    RS_OFFLINE_LOGD("wait fence done.");
}

bool RSHpaeOfflineDevice::DoProcessOffline(
    RSSurfaceRenderParams& params, ProcessOfflineResult& processOfflineResult, HpaeOfflineSubThreadData& taskData)
{
    RS_OPTIONAL_TRACE_NAME_FMT("hpae_offline: do process offline");
    // get Hpae inputInfo
    sptr<SurfaceBuffer> dstSurfaceBuffer = nullptr;
    int32_t releaseFence = -1;
    auto srcSurfaceBuffer = params.GetBuffer();
    auto srcBufferOwnerCount = params.GetBufferOwnerCount();
    if (!srcSurfaceBuffer || !srcBufferOwnerCount) {
        RS_OFFLINE_LOGW("Offline srcSurfaceBuffer get buffer failed!");
        return false;
    }
    BufferOwnerCountGuard guard(srcBufferOwnerCount);
    if (!GetOfflineProcessInput(params, taskData.inputInfo, dstSurfaceBuffer, releaseFence, taskData)) {
        RS_OFFLINE_LOGW("Get offline process input failed.");
        return false;
    }
    WaitFence(params.GetAcquireFence(), releaseFence);

    // hpae offline process
    int32_t ret = -1;
    if (preProcessFunc_) {
        ret = preProcessFunc_(taskData.inputInfo);
    }
    if (ret != 0) {
        RS_OFFLINE_LOGW("Hpae offline process fail.");
        FlushAndReleaseOfflineLayer(dstSurfaceBuffer, taskData);
        return false;
    }

    if (getFenceFunc_) {
        ret = getFenceFunc_(taskData.offlineFenceFd);
        if (ret != 0) {
            RS_OFFLINE_LOGW("Hpae get offline fence fail.");
            FlushAndReleaseOfflineLayer(dstSurfaceBuffer, taskData);
            return false;
        }
    }

    return SubmitOfflineBuffer(taskData, params, dstSurfaceBuffer, processOfflineResult);
}

bool RSHpaeOfflineDevice::SubmitOfflineBuffer(
    HpaeOfflineSubThreadData& taskData,
    RSSurfaceRenderParams& params,
    sptr<SurfaceBuffer>& dstSurfaceBuffer,
    ProcessOfflineResult& processOfflineResult)
{
    RS_OPTIONAL_TRACE_NAME_FMT("hpae_offline: submit offline buffer");

    // flush and consume offline buffer
    taskData.flushConfig.timestamp = 0;
    taskData.flushConfig.damage = {.x = taskData.inputInfo.dstRect.x, .y = taskData.inputInfo.dstRect.y,
        .w = taskData.inputInfo.dstRect.w, .h = taskData.inputInfo.dstRect.h};
    taskData.offlineLayer->FlushSurfaceBuffer(dstSurfaceBuffer, taskData.offlineFenceFd, taskData.flushConfig);
    auto offlineSurfaceHandler = taskData.offlineLayer->GetMutableRSSurfaceHandler();
    if (!RSBaseSurfaceUtil::ConsumeAndUpdateBuffer(*offlineSurfaceHandler) || !offlineSurfaceHandler->GetBuffer()) {
        RS_OFFLINE_LOGW("DeviceOfflineLayer consume buffer failed. %{public}d",
            !offlineSurfaceHandler->GetBuffer());
        return false;
    }

    if (offlineSurfaceHandler->IsCurrentFrameBufferConsumed()) {
        auto offlinePreBufferCount = offlineSurfaceHandler->GetPreBufferOwnerCount();
        if (offlinePreBufferCount) {
            offlinePreBufferCount->DecRef();
        }
    }
    return FillOfflineResult(processOfflineResult, taskData, params, offlineSurfaceHandler);
}

bool RSHpaeOfflineDevice::FillOfflineResult(ProcessOfflineResult& processOfflineResult,
    HpaeOfflineSubThreadData& taskData, RSSurfaceRenderParams& params,
    std::shared_ptr<RSSurfaceHandler>& offlineSurfaceHandler)
{
    // set to offline result
    if (taskData.contextType == OfflineContextType::AI2020) {
        auto src = params.GetLayerInfo().srcRect;
        processOfflineResult.bufferRect = {.x = src.x, .y = src.y, .w = src.w, .h = src.h};
    } else {
        processOfflineResult.bufferRect = {
            .x = taskData.offlineRect.x, .y = taskData.offlineRect.y,
            .w = taskData.offlineRect.w, .h = taskData.offlineRect.h};
    }
    processOfflineResult.consumer = offlineSurfaceHandler->GetConsumer();
    auto damageRect = offlineSurfaceHandler->GetDamageRegion();
    if (!offlineSurfaceHandler->GetBuffer()) {
        return false;
    }
    damageRect.y = offlineSurfaceHandler->GetBuffer()->GetHeight() - damageRect.y - damageRect.h;
    processOfflineResult.damageRect = damageRect;
    processOfflineResult.preBuffer = taskData.lastProcessSuccess ? offlineSurfaceHandler->GetPreBuffer() : nullptr;
    processOfflineResult.buffer = offlineSurfaceHandler->GetBuffer();
    processOfflineResult.acquireFence = offlineSurfaceHandler->GetAcquireFence();
    processOfflineResult.bufferOwnerCount = offlineSurfaceHandler->GetBufferOwnerCount();
    RS_OFFLINE_LOGD("Offline process done, bufferRect: [%{public}d %{public}d %{public}d %{public}d],",
        processOfflineResult.bufferRect.x, processOfflineResult.bufferRect.y,
        processOfflineResult.bufferRect.w, processOfflineResult.bufferRect.h);
    return true;
}

void RSHpaeOfflineDevice::OfflineTaskFunc(RSSurfaceRenderParams* surfaceParams,
    std::shared_ptr<ProcessOfflineFuture>& futurePtr, HpaeOfflineSubThreadData& taskData)
{
    ProcessOfflineResult processOfflineResult;
    processOfflineResult.taskSuccess = DoProcessOffline(*surfaceParams, processOfflineResult, taskData);
    auto context = GetOfflineContext(taskData.nodeId);
    if (context != nullptr) {
        context->lastProcessSuccess = processOfflineResult.taskSuccess;
    }
    offlineResultSync_.MarkTaskDoneAndSetResult(futurePtr, processOfflineResult);
    CheckAndHandleTimeoutEvent(futurePtr, taskData.nodeId);
}

bool RSHpaeOfflineDevice::PostProcessOfflineTask(
    std::shared_ptr<RSSurfaceRenderNode>& surfaceNode, offlineTaskId taskId)
{
    if (!loadSuccess_) {
        RS_OFFLINE_LOGW("hpae so is not loaded.");
        return false;
    }
    if (!isInitOfflineFuncSucc_) {
        RS_OFFLINE_LOGW("initOfflineFunc fail.");
        return false;
    }
    NodeId nodeId = surfaceNode->GetId();
    std::shared_ptr<RSHpaeOfflineContext> context = GetOfflineContext(nodeId);
    if (context == nullptr) {
        return false;
    }
    // while doing direct composition, there is no IsRSHpaeOfflineDeviceReady to check status
    if (context->timeout) {
        RS_OFFLINE_LOGI("This scene has experienced a timeout event!!! banned hpae offline");
        return false;
    }
    if (!UpdateContext(surfaceNode, context)) {
        RS_OFFLINE_LOGW("RSHpaeOfflineDevice::updateContext failed");
        return false;
    }
    auto* params = surfaceNode->GetStagingRenderParams().get();
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(params);
    RS_OFFLINE_LOGD("post offline task[%{public}" PRIu64 "-%{public}" PRIu64 "] by node", taskId.first, taskId.second);
    return PostOfflineTaskCommon(context, surfaceParams, taskId);
}

bool RSHpaeOfflineDevice::PostProcessOfflineTask(
    std::shared_ptr<DrawableV2::RSSurfaceRenderNodeDrawable>& surfaceDrawable, offlineTaskId taskId)
{
    if (!loadSuccess_) {
        RS_OFFLINE_LOGW("hpae so is not loaded.");
        return false;
    }
    if (!isInitOfflineFuncSucc_) {
        RS_OFFLINE_LOGW("initOfflineFunc fail.");
        return false;
    }
    NodeId nodeId = surfaceDrawable->GetId();
    std::shared_ptr<RSHpaeOfflineContext> context = GetOfflineContext(nodeId);
    if (context == nullptr) {
        return false;
    }
    auto *params = surfaceDrawable->GetRenderParams().get();
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(params);
    RS_OFFLINE_LOGD("post offline task[%{public}" PRIu64 "-%{public}" PRIu64 "] by drawable",
        taskId.first, taskId.second);
    return PostOfflineTaskCommon(context, surfaceParams, taskId);
}

bool RSHpaeOfflineDevice::PostOfflineTaskCommon(std::shared_ptr<RSHpaeOfflineContext>& context,
    RSSurfaceRenderParams* surfaceParams, offlineTaskId taskId)
{
    if (context->isSkipDraw()) {
        return SetResultWhenSkipDraw(context, surfaceParams, taskId);
    }
    // while posting offline task in rt thread, there is prevalidate to avoid piling up, post directly
    auto futurePtr = offlineResultSync_.RegisterPostedTask(taskId);
    if (!futurePtr) {
        RS_OFFLINE_LOGE("register post task failed!");
        return false;
    }
    HpaeOfflineSubThreadData taskData;
    taskData.nodeId = context->nodeId;
    taskData.layerConfig = context->layerConfig;
    taskData.offlineRect = context->offlineRect;
    taskData.offlineLayer = context->offlineLayer;
    taskData.contextType = context->contextType;
    taskData.lastProcessSuccess = context->lastProcessSuccess;
    offlineThreadManager_.PostTask([surfaceParams, futurePtr, taskId, this, taskData = std::move(taskData)]() mutable {
        RS_TRACE_NAME("hpae_offline: ProcessOffline");
        RS_OFFLINE_LOGD("start to proces offline surface (by drawable), task[%{public}" PRIu64 "-%{public}" PRIu64 "]",
            taskId.first, taskId.second);
        OfflineTaskFunc(surfaceParams, futurePtr, taskData);
    });
    return true;
}

bool RSHpaeOfflineDevice::SetResultWhenSkipDraw(std::shared_ptr<RSHpaeOfflineContext>& context,
    RSSurfaceRenderParams* surfaceParams, offlineTaskId taskId)
{
    RS_OFFLINE_LOGD("RSHpaeOfflineDevice::skipDraw: skip offline task[%{public}" PRIu64 "-%{public}" PRIu64 "]",
        taskId.first, taskId.second);
    ProcessOfflineResult result;
    auto offlineSurfaceHandler = context->offlineLayer->GetMutableRSSurfaceHandler();

    HpaeOfflineSubThreadData taskData;
    taskData.nodeId = context->nodeId;
    taskData.layerConfig = context->layerConfig;
    taskData.offlineRect = context->offlineRect;
    taskData.offlineLayer = context->offlineLayer;
    taskData.contextType = context->contextType;
    taskData.lastProcessSuccess = context->lastProcessSuccess;
    bool fillOutRet = FillOfflineResult(result, taskData, *surfaceParams, offlineSurfaceHandler);
    if (!fillOutRet) {
        RS_OFFLINE_LOGW("RSHpaeOfflineDevice::FillOfflineResult failed");
        return false;
    }
    result.preBuffer = nullptr;  // need to set nullptr in skiwdraw
    result.taskSuccess = true;
    offlineResultSync_.SetDirectResult(taskId, result);

    // set status after skipdraw
    context->invalidFrames = 0;
    context->hasDrawn = true;
    return true;
}

void RSHpaeOfflineDevice::CheckAndPostClearOfflineResourceTask(const std::vector<uint64_t>& offlineNodeIds)
{
    if (!loadSuccess_) {
        RS_OFFLINE_LOGD("hpae so is not loaded.");
        return;
    }

    SetNodeArsrTag(offlineNodeIds);

    // deinit
    if (GetContextPoolSize() == 0 && offlineNodeIds.empty()) {
        offlineThreadManager_.PostTask([this]() mutable {
            RS_TRACE_NAME("hpae_offline: Post deInit task.");
            deInitOfflineFunc_();
            isInitOfflineFuncSucc_ = false;
        });
        return;
    }

    // clear context
    ClearOfflineContext(offlineNodeIds);
}

void RSHpaeOfflineDevice::SetNodeArsrTag(const std::vector<uint64_t>& offlineNodeIds)
{
    if (offlineNodeIds.size() < DISABLE_ARSR_NODE_NUM) {
        return;
    }
    const auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
    for (const auto& nodeId : offlineNodeIds) {
        auto node = nodeMap.GetRenderNode<RSSurfaceRenderNode>(nodeId);
        if (node != nullptr) {
            node->SetArsrTag(false);
        }
    }
}

bool RSHpaeOfflineDevice::WaitForProcessOfflineResult(offlineTaskId taskId,
    std::chrono::milliseconds timeout, ProcessOfflineResult& processOfflineResult)
{
    if (!loadSuccess_) {
        RS_OFFLINE_LOGW("hpae so is not loaded.");
        return false;
    }
    RS_TRACE_NAME("hpae_offline: Wait for node offline process");
    RS_OFFLINE_LOGD("start to wait for offline result, task[%{public}" PRIu64 "-%{public}" PRIu64 "]",
        taskId.first, taskId.second);
    bool waitSuccess = offlineResultSync_.WaitForTaskAndGetResult(taskId, timeout, processOfflineResult);
    auto context = GetOfflineContext(taskId.second);
    if (context == nullptr) {
        return waitSuccess;
    }
    context->hasDrawn = processOfflineResult.taskSuccess;
    context->invalidFrames = processOfflineResult.taskSuccess ? 0 : context->invalidFrames + 1;
    return waitSuccess;
}

int32_t RSHpaeOfflineDevice::GetContextPoolSize()
{
    std::lock_guard<std::mutex> lock(contextPoolMutex_);
    return hpaeOfflineContextPool_.size();
}

void RSHpaeOfflineDevice::ClearOfflineContext(const std::vector<uint64_t>& offlineNodeIds)
{
    std::vector<uint64_t> needClearNodeIds;
    std::vector<std::shared_ptr<RSHpaeOfflineLayer>> needClearOfflineLayers;
    {
        // step 1. get contexts need be cleared
        std::lock_guard<std::mutex> lock(contextPoolMutex_);
        for (auto it = hpaeOfflineContextPool_.begin(); it != hpaeOfflineContextPool_.end(); ++it) {
            if (std::find(offlineNodeIds.begin(), offlineNodeIds.end(), it->first) == offlineNodeIds.end()) {
                auto context = it->second;
                context->invalidFrames++;
                if (context->invalidFrames >= context->maxInvalidFrames) {
                    needClearNodeIds.push_back(it->first);
                }
            }
        }
        // step 2. clear context and get offlinelayers need be cleared
        for (auto offlineNodeId : needClearNodeIds) {
            auto context = hpaeOfflineContextPool_.at(offlineNodeId);
            needClearOfflineLayers.push_back(context->offlineLayer);
            hpaeOfflineContextPool_.erase(offlineNodeId);
        }
    }

    // step 3. post async clear task
    if (!needClearOfflineLayers.empty()) {
        offlineThreadManager_.PostTask([this, needClearOfflineLayers]() mutable {
            RS_TRACE_NAME("hpae_offline: Post ClearOfflineCacheTask.");
            for (auto offlineLayer : needClearOfflineLayers) {
                offlineLayer->CleanCache(true);
            }
        });
    }
}

std::shared_ptr<RSHpaeOfflineContext> RSHpaeOfflineDevice::GetOrCreateOfflineContext(
    std::shared_ptr<RSSurfaceRenderNode>& node)
{
    std::lock_guard<std::mutex> lock(contextPoolMutex_);
    auto nodeId = node->GetId();
    auto it = hpaeOfflineContextPool_.find(nodeId);
    if (it != hpaeOfflineContextPool_.end()) {
        return it->second;
    }
    if (hpaeOfflineContextPool_.size() >= MAX_CACHE_SIZE) {
        RS_OFFLINE_LOGI("RSHpaeOfflineDevice::can't create new context!");
        return nullptr;
    }
    std::shared_ptr<RSHpaeOfflineContext> context;
    HdrStatus hdrStatus = RSHdrUtil::CheckIsHdrSurface(*node);
    if (hdrStatus == HdrStatus::AI_HDR_VIDEO_AI2020 || hdrStatus == HdrStatus::AI_HDR_VIDEO_GAINMAP) {
        context = std::make_shared<RSHpaeOfflineContext>(OfflineContextType::AI2020);
    } else {
        context = std::make_shared<RSHpaeOfflineContext>(OfflineContextType::SCALE);
    }
    hpaeOfflineContextPool_[nodeId] = context;
    context->nodeId = nodeId;
    return context;
}

std::shared_ptr<RSHpaeOfflineContext> RSHpaeOfflineDevice::GetOfflineContext(NodeId nodeId)
{
    std::lock_guard<std::mutex> lock(contextPoolMutex_);
    auto it = hpaeOfflineContextPool_.find(nodeId);
    if (it != hpaeOfflineContextPool_.end()) {
        return it->second;
    }
    return nullptr;
}

bool RSHpaeOfflineDevice::UpdateContext(std::shared_ptr<RSSurfaceRenderNode>& node,
    std::shared_ptr<RSHpaeOfflineContext>& context)
{
    RS_OFFLINE_LOGD("RSHpaeOfflineDevice::updateContext");
    if (context->contextType != OfflineContextType::AI2020) {
        return true;
    }
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(node->GetStagingRenderParams().get());
    auto surfaceHandler = node->GetRSSurfaceHandler();
    if (!surfaceHandler || !surfaceParams) {
        return false;
    }
    // skipDraw: buffer not consumed + previous frame drawn
    bool bufferConsumed = surfaceHandler->IsCurrentFrameBufferConsumed();
    if (!bufferConsumed && context->hasDrawn) {
        context->skipDraw = true;
        RS_OFFLINE_LOGD("skipDraw: reuse previous frame");
        return true;
    }
    // normal drawing
    context->skipDraw = false;
    context->hasDrawn = false;
    RS_OFFLINE_LOGD("need hpae offline process, bufferConsumed=%{public}d", bufferConsumed);
    return true;
}

} // namespace Rosen
} // namespace OHOS