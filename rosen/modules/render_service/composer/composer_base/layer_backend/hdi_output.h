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

#ifndef RENDER_SERVICE_COMPOSER_BASE_LAYER_BACKEND_HDI_OUTPUT_H
#define RENDER_SERVICE_COMPOSER_BASE_LAYER_BACKEND_HDI_OUTPUT_H

#include <array>
#include <list>
#include <cstdint>
#include <vector>
#include <unordered_map>

#include "graphic_error.h"
#include "surface_type.h"
#include "hdi_layer.h"
#include "hdi_framebuffer_surface.h"
#include "hdi_screen.h"
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
using OnPrepareCompleteFunc = std::function<void(sptr<Surface>& surface,
                                                 const PrepareCompleteParam &param, void* data)>;

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
    void Dump(std::string& result) const;
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
    void ReleaseLayers(sptr<SyncFence>& releaseFence);
    int32_t GetBufferCacheSize();
    void SetVsyncSamplerEnabled(bool enabled);
    bool GetVsyncSamplerEnabled();
    void SetProtectedFrameBufferState(bool state)
    {
        isProtectedBufferAllocated_.store(state);
    }
    bool GetProtectedFrameBufferState()
    {
        return isProtectedBufferAllocated_.load();
    }
    void CleanLayerBufferBySurfaceId(uint64_t surfaceId);

    void SetActiveRectSwitchStatus(bool flag);
    void ANCOTransactionOnComplete(const std::shared_ptr<RSLayer>& layerInfo,
        const sptr<SyncFence>& previousReleaseFence);

    void SetMaskLayer(const std::shared_ptr<HdiLayer>& maskLayer) { maskLayer_ = maskLayer; }
    RosenError RegPrepareComplete(OnPrepareCompleteFunc func, void* data);
    void Repaint();
    void SetScreenPowerOnChanged(bool flag);
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
    void UpdateThirdFrameAheadPresentFence(sptr<SyncFence> &fbFence);
private:
    HdiDevice *device_ = nullptr;
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
    // solidLayer unique id -- layer ptr
    std::unordered_map<uint64_t, std::shared_ptr<HdiLayer>> solidSurfaceIdMap_;
    uint32_t screenId_;
    std::vector<GraphicIRect> outputDamages_;
    bool directClientCompositionEnabled_ = true;

    std::vector<sptr<SurfaceBuffer> > bufferCache_;
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

    int32_t CreateLayerLocked(uint64_t surfaceId, const std::shared_ptr<RSLayer> &rsLayer);
    void DeletePrevLayersLocked();
    void ResetLayerStatusLocked();
    void ReorderLayerInfoLocked(std::vector<LayerDumpInfo>& dumpLayerInfos) const;
    void UpdatePrevRSLayerLocked();
    void ReleaseSurfaceBuffer(sptr<SyncFence>& releaseFence);
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

    bool isActiveRectSwitching_ = false;
    void DirtyRegions(uint32_t solidLayerCount, const std::shared_ptr<RSLayer>& rsLayer);

    OnPrepareCompleteFunc onPrepareCompleteCb_ = nullptr;
    void* onPrepareCompleteCbData_ = nullptr;
    bool screenPowerOnChanged_ = false;
    int32_t thirdFrameAheadPresentFenceFd_ = 0;
    int64_t thirdFrameAheadPresentTime_ = 0;
    int32_t curPresentFd_ = 0;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_COMPOSER_BASE_LAYER_BACKEND_HDI_OUTPUT_H