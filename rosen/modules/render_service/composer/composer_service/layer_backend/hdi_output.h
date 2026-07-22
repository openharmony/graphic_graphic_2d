/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_COMPOSER_SERVICE_LAYER_BACKEND_HDI_OUTPUT_H
#define RENDER_SERVICE_COMPOSER_SERVICE_LAYER_BACKEND_HDI_OUTPUT_H

#include <array>
#include <cstdint>
#include <functional>
#include <list>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "graphic_error.h"
#include "hdi_layer.h"
#include "hdi_framebuffer_surface.h"
#include "hdi_screen.h"
#include "irs_composer_to_render_connection.h"
#include "platform/ohos/rs_composer_jank_stats.h"
#include "surface_type.h"
#include "vsync_sampler.h"

namespace OHOS {
namespace Rosen {
// dump layer
struct LayerDumpInfo {
    uint64_t nodeId;
    uint64_t surfaceId;
    std::shared_ptr<HdiLayer> hdiLayer;
};

struct PrepareCompleteParam {
    bool needFlushFramebuffer = false;
    std::vector<std::shared_ptr<RSLayer>> layers;
};

struct LayerCreatedInfo {
    uint64_t nodeId = 0;
    uint64_t tunnelLayerGeneration = 0;
};

using OnPrepareCompleteFunc = std::function<void(sptr<Surface>& surface,
                                                 const PrepareCompleteParam &param, void* data)>;
using OnLayerCreatedFunc = std::function<void(
    uint64_t nodeId, bool success, uint64_t tunnelLayerGeneration, void* data)>;

class HdiOutput {
public:
    explicit HdiOutput(uint32_t screenId);
    virtual ~HdiOutput();

    static constexpr uint32_t COMPOSITION_RECORDS_NUM = HdiLayer::FRAME_RECORDS_NUM;

    /* for RS begin */
    void SetRSLayers(const std::vector<std::shared_ptr<RSLayer>>& rsLayers);
    void SetOutputDamages(const std::vector<GraphicIRect>& outputDamages);
    uint32_t GetScreenId() const;
    // only used when composer_host dead
    void ResetDevice()
    {
        std::lock_guard<std::mutex> lock(backlightDeviceMutex_);
        device_ = nullptr;
    }
    bool IsDeviceValid() const
    {
        return device_ != nullptr;
    }
    /* for RS end */

    static std::shared_ptr<HdiOutput> CreateHdiOutput(uint32_t screenId);
    RosenError Init();
    void GetRSLayers(std::vector<std::shared_ptr<RSLayer>>& rsLayers);
    void GetComposeClientLayers(std::vector<std::shared_ptr<HdiLayer>>& clientLayers);
    const std::vector<GraphicIRect>& GetOutputDamages();
    sptr<Surface> GetFrameBufferSurface();
    std::unique_ptr<FrameBufferEntry> GetFramebuffer();
    void Dump(std::string& result, bool isSplitRender = false) const;
    void DumpCurrentFrameLayers() const;
    void DumpFps(std::string& result, const std::string& arg) const;
    void DumpHitchs(std::string& result, const std::string& arg) const;
    void ClearFpsDump(std::string& result, const std::string& arg);
    GSError ClearFrameBuffer();

    RosenError InitDevice();
    /* only used for mock tests */
    RosenError SetHdiOutputDevice(HdiDevice* device);
    int32_t PreProcessLayersComp();
    int32_t UpdateLayerCompType();
    int32_t FlushScreen(std::vector<std::shared_ptr<HdiLayer>>& compClientLayers);
    int32_t Commit(sptr<SyncFence>& fbFence);
    int32_t CommitAndGetReleaseFence(sptr<SyncFence>& fbFence, int32_t& skipState, bool& needFlush, bool isValidated);
    int32_t UpdateInfosAfterCommit(sptr<SyncFence> fbFence);
    int32_t ReleaseFramebuffer(const sptr<SyncFence>& releaseFence);
    std::unordered_map<std::shared_ptr<RSLayer>, sptr<SyncFence>> GetLayersReleaseFence();
    int32_t StartVSyncSampler(bool forceReSample = false);
    void SetPendingMode(int64_t period, int64_t timestamp);
    void ReleaseLayers(ReleaseLayerBuffersInfo& releaseLayerInfo);
    int32_t GetBufferCacheSize() const;
    void SetVsyncSamplerEnabled(bool enabled);
    bool GetVsyncSamplerEnabled();
    void SetProtectedFrameBufferState(bool state)
    {
        isProtectedBufferAllocated_.store(state);
    }
    bool GetProtectedFrameBufferState() const
    {
        return isProtectedBufferAllocated_.load();
    }
    void CleanLayerBufferBySurfaceId(uint64_t surfaceId);
    void MarkTunnelSurfaceInvalid(uint64_t surfaceId);
    void EraseTunnelSurfaceInvalid(uint64_t surfaceId);
    uint64_t GetNodeIdBySurfaceId(uint64_t surfaceId) const;
    uint64_t GetTunnelLayerGenerationBySurfaceId(uint64_t surfaceId) const;
    int32_t DestroyLayerBySurfaceId(uint64_t surfaceId);
    int32_t CommitTunnelLayerBySurfaceId(uint64_t surfaceId, uint64_t tunnelLayerId,
        const sptr<SurfaceBuffer>& buffer, const sptr<SyncFence>& acquireFence, sptr<SyncFence>& releaseFence);

    void SetActiveRectSwitchStatus(bool flag, const GraphicIRect& activeRect);
    void SetMaskLayer(const std::shared_ptr<HdiLayer>& maskLayer) { maskLayer_ = maskLayer; }
    RosenError RegPrepareComplete(OnPrepareCompleteFunc func, void* data);
    RosenError RegLayerCreated(OnLayerCreatedFunc func, void* data);
    void ClearLayerCreatedCallback();
    void Repaint();
    void SetScreenBacklight(uint32_t level);
    void SetScreenLinearMatrix(const std::vector<float>& matrix);
    int32_t GetCurrentFramePresentFd() const
    {
        return curPresentFd_;
    }
    int32_t GetThirdFrameAheadPresentFd() const
    {
        return thirdFrameAheadPresentFenceFd_;
    }
    int64_t GetThirdFrameAheadPresentTime() const
    {
        return thirdFrameAheadPresentTime_;
    }
    void UpdateThirdFrameAheadPresentFence(sptr<SyncFence>& fbFence);
    int32_t GetDisplayClientTargetProperty(int32_t& pixelFormat, int32_t& dataspace);
    int32_t GetLayerSolidFilledColor(uint64_t layerId, uint32_t& solidFilledColor);

private:
    HdiDevice *device_ = nullptr;
    // Synchronizes device_ updates with asynchronous backlight tasks.
    mutable std::mutex backlightDeviceMutex_;
    sptr<VSyncSampler> sampler_ = nullptr;

    std::vector<sptr<SyncFence>> historicalPresentfences_;
    sptr<SyncFence> thirdFrameAheadPresentFence_ = SyncFence::InvalidFence();
    int32_t presentFenceIndex_ = 0;

    sptr<SurfaceBuffer> currFrameBuffer_ = nullptr;
    sptr<SurfaceBuffer> lastFrameBuffer_ = nullptr;

    std::array<int64_t, COMPOSITION_RECORDS_NUM> compositionTimeRecords_ = {};
    uint32_t compTimeRcdIndex_ = 0;
    sptr<HdiFramebufferSurface> fbSurface_ = nullptr;
    std::list<std::shared_ptr<HdiLayer>> layersTobeRelease_;

    // layerId -- layer ptr
    std::unordered_map<uint32_t, std::shared_ptr<HdiLayer>> layerIdMap_;
    // surface unique id -- layer ptr
    std::unordered_map<uint64_t, std::shared_ptr<HdiLayer>> surfaceIdMap_;
    std::unordered_set<uint64_t> invalidTunnelSurfaceIds_;
    // surfaceId -> last failed tunnelLayerGeneration; suppresses tunnel retry until generation changes.
    std::unordered_map<uint64_t, uint64_t> tunnelFallbackGenerations_;
    /* only used for GetLayerSolidFilledColor find hdi layer id */
    std::unordered_map<uint64_t, std::shared_ptr<HdiLayer>> solidRSLayerIdMap_;
    uint32_t screenId_;
    std::vector<GraphicIRect> outputDamages_;
    bool directClientCompositionEnabled_ = true;

    std::vector<sptr<SurfaceBuffer>> bufferCache_;
    uint32_t bufferCacheCountMax_ = 0;
    mutable std::mutex mutex_;

    std::vector<uint32_t> layersId_;
    std::vector<sptr<SyncFence>> fences_;
    // DISPLAYENGINE
    bool arsrPreEnabled_ = false;
    bool arsrPreEnabledForVm_ = false;
    std::string vmArsrWhiteList_ = "";
    // Protected framebuffer is allocated in advance
    std::atomic<bool> isProtectedBufferAllocated_ = false;

    std::shared_ptr<HdiLayer> maskLayer_ = nullptr;
    std::function<std::shared_ptr<HdiLayer>(uint32_t)> createHdiLayerFunc_ = HdiLayer::CreateHdiLayer;

    int32_t CreateLayerLocked(uint64_t surfaceId, const std::shared_ptr<RSLayer>& rsLayer);
    void DestroyLayerBySurfaceIdLocked(uint64_t surfaceId);
    void UnregisterGlobalTunnelLayersLocked() const;
    void UpdateRSLayerLocked(const std::shared_ptr<RSLayer>& rsLayer);
    bool UpdateSolidColorLayerLocked(const std::shared_ptr<RSLayer>& rsLayer);
    void DeletePrevLayersLocked();
    void ResetLayerStatusLocked();
    void ReorderLayerInfoLocked(std::vector<LayerDumpInfo>& dumpLayerInfos) const;
    void UpdatePrevRSLayerLocked();
    void RecordCompositionTime(int64_t timeStamp);
    inline bool CheckFbSurface();
    bool CheckAndUpdateClientBufferCahce(sptr<SurfaceBuffer> buffer, uint32_t& index);

    // DISPLAY ENGINE
    bool CheckIfDoArsrPre(const std::shared_ptr<RSLayer>& rsLayer);
    bool CheckIfDoArsrPreForVm(const std::shared_ptr<RSLayer>& rsLayer);
    bool CheckSupportArsrPreMetadata();
    bool CheckSupportCopybitMetadata();

    void ClearBufferCache();
    std::unordered_map<std::shared_ptr<RSLayer>, sptr<SyncFence>> GetLayersReleaseFenceLocked();

    void ReorderRSLayers(std::vector<std::shared_ptr<RSLayer>>& newRSLayer);
    void OnPrepareComplete(bool needFlush, std::vector<std::shared_ptr<RSLayer>>& newRSLayer);
    int32_t PrepareCompleteIfNeed(bool needFlush);
    void DumpLayerInfoForSplitRender(std::string& result) const;
    void OnLayerCreated(uint64_t nodeId, bool success, uint64_t tunnelLayerGeneration);
    std::vector<LayerCreatedInfo> CollectPendingLayerCreatedInfosLocked();
    void ClearRecoveredInvalidTunnelSurfaceIdsLocked();
    bool IsTunnelLayerRequestedLocked(const std::shared_ptr<RSLayer>& rsLayer) const;
    bool FallbackTunnelLayerToGraphicLocked(const std::shared_ptr<HdiLayer>& hdiLayer,
        const std::shared_ptr<RSLayer>& rsLayer) const;
    void RegisterCreatedLayerLocked(
        uint64_t surfaceId, const std::shared_ptr<HdiLayer>& hdiLayer,
        const std::shared_ptr<RSLayer>& rsLayer, bool shouldEmitTunnelCreated);
    void SetLayerPerFrameParameters(uint32_t layerId, const std::shared_ptr<RSLayer>& rsLayer);
    bool IsTunnelDeclinedLocked(uint64_t surfaceId, uint64_t generation) const;
    bool ShouldFallbackTunnelLayerLocked(uint64_t surfaceId, const std::shared_ptr<RSLayer>& rsLayer) const;
    void MarkTunnelDeclinedLocked(uint64_t surfaceId, uint64_t generation);
    void ClearTunnelDeclinedLocked(uint64_t surfaceId);
    void FinalizePostCommit(bool commitSucceeded = true);

    bool isActiveRectSwitching_ = false;
    void DirtyRegions(const std::shared_ptr<RSLayer>& rsLayer);
    OnPrepareCompleteFunc onPrepareCompleteCb_ = nullptr;
    void* onPrepareCompleteCbData_ = nullptr;
    OnLayerCreatedFunc onLayerCreatedCb_ = nullptr;
    void* onLayerCreatedCbData_ = nullptr;
    int32_t thirdFrameAheadPresentFenceFd_ = 0;
    int64_t thirdFrameAheadPresentTime_ = 0;
    int32_t curPresentFd_ = 0;
    RSComposerJankStats rsComposerJankStats;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_COMPOSER_SERVICE_LAYER_BACKEND_HDI_OUTPUT_H
