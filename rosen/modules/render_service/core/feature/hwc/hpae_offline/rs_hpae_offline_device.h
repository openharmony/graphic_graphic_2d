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

#ifndef RS_CORE_FEATURE_HWC_HPAE_OFFLINE_DEVICE_H
#define RS_CORE_FEATURE_HWC_HPAE_OFFLINE_DEVICE_H
#include <atomic>
#include <buffer_handle.h>
#include <cstdint>

#include "feature/hwc/hpae_offline/rs_hpae_offline_layer.h"
#include "feature/hwc/hpae_offline/rs_hpae_offline_process_syncer.h"
#include "feature/hwc/hpae_offline/rs_hpae_offline_thread_manager.h"
#include "feature/hwc/hpae_offline/rs_offline_device.h"
#include "feature/hwc/hpae_offline/rs_offline_result.h"
#include "feature/hwc/rs_uni_hwc_prevalidate_common.h"

#include <cstdint>
#include <atomic>
#include <buffer_handle.h>

namespace OHOS {
namespace Rosen {

enum OfflineContextType:: uint32_t {
    INVALID = 0,
    SCALE,
    AI2020
};

struct OfflineProcessInputInfo {
    uint64_t id;
    BufferHandle* srcHandle = nullptr;
    BufferHandle* dstHandle = nullptr;
    RequestRect srcRect;
    RequestRect dstRect;
    uint32_t transform = 0;
    int32_t acquireFence = 0;
    bool timeout = false;
};

struct OfflineBufferConfig {
    int32_t width = 0;
    int32_t height = 0;
    int32_t strideAlignment = 0;
    int32_t format = 0;
    uint64_t usage = 0;
    int32_t timeout = 0;
    int32_t colorGamut = 0;
    int32_t transform = 0;
};

struct OfflineProcessOutputInfo {
    OfflineBufferConfig bufferConfig;
    RequestRect outRect;
};

struct HpaeOfflineSubThreadData {
    OfflineContextType contextType = OfflineContextType::INVALID;
    NodeId nodeId = 0;
    std::shared_ptr<RSHpaeOfflineLayer> offlineLayer = nullptr;
    BufferRequestConfig layerConfig = {
        .width = 0,
        .height = 0,
        .strideAlignment = 0x88,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_HW_COMPOSER | BUFFER_USAGE_MEM_DMA | BUFFER_USAGE_CPU_READ,
        .timeout = 0,
    };
    RequestRect offlineRect;
    BufferFlushConfig flushConfig = {
        .damage = {.x = 0, .y = 0, .w = 0, .h = 0},
        .timestamp = 0,
        .desiredPresentTimestamp = 0,
    };
    bool lastProcessSuccess = false;
    OfflineProcessInputInfo inputInfo;
    int32_t offlineFenceFd = -1;
};

class RSHpaeOfflineContext {
public:
    explicit RSHpaeOfflineContext(OfflineContextType type);
    ~RSHpaeOfflineContext();

    bool isSkipDraw();

    OfflineContextType contextType = OfflineContextType::INVALID;
    std::shared_ptr<RSHpaeOfflineLayer> offlineLayer = nullptr;
    BufferRequestConfig layerConfig = {
        .width = 0,
        .height = 0,
        .strideAlignment = 0x88,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_HW_COMPOSER | BUFFER_USAGE_MEM_DMA | BUFFER_USAGE_CPU_READ,
        .timeout = 0,
    };
    RequestRect offlineRect;
    std::atomic<bool> lastProcessSuccess = false;
    std::atomic<bool> timeout = false;
    std::atomic<bool> preAllocBufferSucc = false;
    std::atomic<size_t> invalidFrames = 0;
    std::atomic<size_t> maxInvalidFrames = 0;
    std::atomic<bool> skipDraw = false;
    std::atomic<bool> hasDrawn = false;
    std::mutex offlineConfigMutex;
};

using ProcessOfflineFunc = int32_t (*)(const OfflineProcessInputInfo &);
using GetOfflineConfigFunc = int32_t (*)(NodeId, OfflineProcessOutputInfo &);
using InitOfflineResourceFunc = int32_t (*)();
using DeInitOfflineResourceFunc = void (*)();
using GetFenceFunc = int32_t (*)(int32_t &);

class RSHpaeOfflineDevice : public RSOfflineDevice, public std::enable_shared_from_this<RSHpaeOfflineDevice> {
public:
    explicit RSHpaeOfflineDevice();
    ~RSHpaeOfflineDevice() override;
    OfflineDeviceType GetDeviceType() const override { return OfflineDeviceType::HPAE_OFFLINE_DEVICE; }

    bool IsRSOfflineDeviceReady(std::shared_ptr<RSSurfaceRenderNode>& surfaceNode) override;
    bool PostProcessOfflineTask(
        std::shared_ptr<DrawableV2::RSSurfaceRenderNodeDrawable>& surfaceDrawable, offlineTaskId taskId) override;
    bool PostProcessOfflineTask(std::shared_ptr<RSSurfaceRenderNode>& surfaceNode, offlineTaskId taskId) override;
    bool WaitForProcessOfflineResult(
        offlineTaskId taskId, std::chrono::milliseconds timeout, ProcessOfflineResult& processOfflineResult) override;
    void CheckAndPostClearOfflineResourceTask(const std::vector<uint64_t>& offlineNodeIds) override;
    bool CanDeleteDevice() override { return false; }

private:
    RSHpaeOfflineDevice(const RSHpaeOfflineDevice&) = delete;
    RSHpaeOfflineDevice(const RSHpaeOfflineDevice&&) = delete;
    RSHpaeOfflineDevice& operator=(const RSHpaeOfflineDevice&) = delete;
    RSHpaeOfflineDevice& operator=(const RSHpaeOfflineDevice&&) = delete;
    void CloseOfflineHandle(const std::string& errSymbol, const char* errNo);
    void LoadPreProcessHandle();
    bool GetOutputConfig(std::shared_ptr<RSHpaeOfflineContext>& context, std::shared_ptr<RSSurfaceRenderNode>& node);
    void CheckAndPostPreAllocBuffersTask(std::shared_ptr<RSHpaeOfflineContext> context);
    bool GetOfflineProcessInput(RSSurfaceRenderParams& params, OfflineProcessInputInfo& inputInfo,
        sptr<SurfaceBuffer>& dstSurfaceBuffer, int32_t& releaseFence, HpaeOfflineSubThreadData& taskData);
    void FlushAndReleaseOfflineLayer(sptr<SurfaceBuffer>& dstSurfaceBuffer, HpaeOfflineSubThreadData& taskData);
    void OfflineTaskFunc(RSRenderParams* surfaceParams, 
        std::shared_ptr<ProcessOfflineFuture>& futurePtr, HpaeOfflineSubThreadData& taskData);
    bool DoProcessOffline(RSSurfaceRenderParams& params, ProcessOfflineResult& processOfflineResult,
        HpaeOfflineSubThreadData& taskData);
    bool SubmitOfflineBuffer(HpaeOfflineSubThreadData& taskData, RSSurfaceRenderParams& params,
        sptr<SurfaceBuffer>& dstSurfaceBuffer, ProcessOfflineResult& processOfflineResult);
    void CheckAndHandleTimeoutEvent(std::shared_ptr<ProcessOfflineFuture>& futurePtr, NodeId nodeId);
    void ClearOfflineContext(const std::vector<uint64_t>& offlineNodeIds);
    std::shared_ptr<RSHpaeOfflineContext> GetOrCreateOfflineContext(std::shared_ptr<RSSurfaceRenderNode>& node);
    std::shared_ptr<RSHpaeOfflineContext> GetOfflineContext(NodeId nodeId);
    int32_t GetContextPoolSize();
    bool UpdateContext(std::shared_ptr<RSSurfaceRenderNode>& node, std::shared_ptr<RSHpaeOfflineContext>& context);
    void InitHpaeOfflineResource();
    bool FillOfflineResult(ProcessOfflineResult& processOfflineResult, HpaeOfflineSubThreadData& taskData,
        RSSurfaceRenderParams& params, std::shared_ptr<RSSurfaceHander>& offlineSurfaceHandler);
    bool PostOfflineTaskCommon(std::shared_pt<RSHpaeOfflineContext>& context,
        RSSurfaceRenderParams& params, offlineTaskId taskId, NodeId nodeId);
    bool SetResultWhenSkipDraw(std::shared_ptr<RSHpaeOfflineContext>& context,
        RSSurfaceRenderParams* surfaceParams, offlineTaskId taskId);
    void SetNodeArsrTag(const std::vector<uint64_t>& offlineNodeIds);
    // so handler
    bool loadSuccess_ = false;
    std::atomic<bool> isInitOfflineFuncSucc_ = false;
    void* preProcessHandle_ = nullptr;
    ProcessOfflineFunc preProcessFunc_ = nullptr;
    GetOfflineConfigFunc getConfigFunc_ = nullptr;
    InitOfflineResourceFunc initOfflineFunc_ = nullptr;
    DeInitOfflineResourceFunc deInitOfflineFunc_ = nullptr;
    GetFenceFunc getFenceFunc_ = nullptr;

    RSHpaeOfflineProcessSyncer offlineResultSync_;
    RSHpaeOfflineThreadManager offlineThreadManager_;
    std::unordered_map<uint64_t, std:::shared_ptr<RSHpaeOfflineContext>> hpaeOfflineContextPool_;
    std::mutex contextPoolMutex_;
};
} // Rosen
} // OHOS
#endif // RS_CORE_FEATURE_HWC_HPAE_OFFLINE_DEVICE_H