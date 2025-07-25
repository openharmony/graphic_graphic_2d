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

#ifndef HDI_BACKEND_HDI_OUTPUT_H
#define HDI_BACKEND_HDI_OUTPUT_H

#include <array>
#include <list>
#include <stdint.h>
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

using LayerPtr = std::shared_ptr<HdiLayer>;

// dump layer
struct LayerDumpInfo {
    uint64_t nodeId;
    uint64_t surfaceId;
    LayerPtr layer;
};

// Load Optimization Param
const std::string IS_MERGE_FENCE_SKIPPED = "isMergeFenceSkipped";
struct LoadOptParamsForHdiOutput {
    std::unordered_map<std::string, bool> switchParams;
};

class HdiOutput {
public:
    HdiOutput(uint32_t screenId);
    virtual ~HdiOutput();

    static constexpr uint32_t COMPOSITION_RECORDS_NUM = HdiLayer::FRAME_RECORDS_NUM;

    /* for RS begin */
    void SetLayerInfo(const std::vector<LayerInfoPtr> &layerInfos);
    void SetOutputDamages(const std::vector<GraphicIRect> &outputDamages);
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
    void InitLoadOptParams(LoadOptParamsForHdiOutput& loadOptParamsForHdiOutput);
    /* for RS end */

    static std::shared_ptr<HdiOutput> CreateHdiOutput(uint32_t screenId);
    RosenError Init();
    void GetLayerInfos(std::vector<LayerInfoPtr>& layerInfos);
    void GetComposeClientLayers(std::vector<LayerPtr>& clientLayers);
    const std::vector<GraphicIRect>& GetOutputDamages();
    sptr<Surface> GetFrameBufferSurface();
    std::unique_ptr<FrameBufferEntry> GetFramebuffer();
    void Dump(std::string &result) const;
    void DumpCurrentFrameLayers() const;
    void DumpFps(std::string &result, const std::string &arg) const;
    void DumpHitchs(std::string &result, const std::string &arg) const;
    void ClearFpsDump(std::string &result, const std::string &arg);
    GSError ClearFrameBuffer();

    RosenError InitDevice();
    /* only used for mock tests */
    RosenError SetHdiOutputDevice(HdiDevice* device);
    int32_t PreProcessLayersComp();
    int32_t UpdateLayerCompType();
    int32_t FlushScreen(std::vector<LayerPtr> &compClientLayers);
    int32_t SetScreenClientInfo(const FrameBufferEntry &fbEntry);
    int32_t Commit(sptr<SyncFence> &fbFence);
    int32_t CommitAndGetReleaseFence(sptr<SyncFence> &fbFence, int32_t &skipState, bool &needFlush, bool isValidated);
    int32_t UpdateInfosAfterCommit(sptr<SyncFence> fbFence);
    int32_t ReleaseFramebuffer(const sptr<SyncFence>& releaseFence);
    std::map<LayerInfoPtr, sptr<SyncFence>> GetLayersReleaseFence();
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

    void ANCOTransactionOnComplete(const LayerInfoPtr& layerInfo, const sptr<SyncFence>& previousReleaseFence);

    void SetMaskLayer(LayerPtr maskLayer)
    {
        maskLayer_ = maskLayer;
    }
    
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
    std::list<LayerPtr> layersTobeRelease_;

    // layerId -- layer ptr
    std::unordered_map<uint32_t, LayerPtr> layerIdMap_;
    // surface unique id -- layer ptr
    std::unordered_map<uint64_t, LayerPtr> surfaceIdMap_;
    // solidLayer unique id -- layer ptr
    std::unordered_map<uint64_t, LayerPtr> solidSurfaceIdMap_;
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

    // Load Optimization Params
    LoadOptParamsForHdiOutput loadOptParamsForHdiOutput_ = {};
    bool isMergeFenceSkipped_ = false;
    bool isMergeFenceSkippedDfx_ = true;

    LayerPtr maskLayer_ = nullptr;

    int32_t CreateLayerLocked(uint64_t surfaceId, const LayerInfoPtr &layerInfo);
    void DeletePrevLayersLocked();
    void ResetLayerStatusLocked();
    void ReorderLayerInfoLocked(std::vector<LayerDumpInfo> &dumpLayerInfos) const;
    void UpdatePrevLayerInfoLocked();
    void ReleaseSurfaceBuffer(sptr<SyncFence>& releaseFence);
    void RecordCompositionTime(int64_t timeStamp);
    inline bool CheckFbSurface();
    bool CheckAndUpdateClientBufferCahce(sptr<SurfaceBuffer> buffer, uint32_t& index);

    // DISPLAY ENGINE
    bool CheckIfDoArsrPre(const LayerInfoPtr &layerInfo);
    bool CheckIfDoArsrPreForVm(const LayerInfoPtr &layerInfo);
    bool CheckSupportArsrPreMetadata();
    bool CheckSupportCopybitMetadata();

    void ClearBufferCache();
    std::map<LayerInfoPtr, sptr<SyncFence>> GetLayersReleaseFenceLocked();

    bool isActiveRectSwitching_ = false;
    void DirtyRegions(uint32_t solidLayerCount, const LayerInfoPtr &layerInfo);
};
} // namespace Rosen
} // namespace OHOS

#endif // HDI_BACKEND_HDI_OUTPUT_H