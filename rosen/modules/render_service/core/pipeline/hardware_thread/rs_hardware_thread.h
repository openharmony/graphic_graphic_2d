/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#ifndef RS_HARDWARE_THREAD_H
#define RS_HARDWARE_THREAD_H

#include <atomic>
#include <mutex>

#include "event_handler.h"
#include "hdi_backend.h"
#include "hgm_core.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "feature/hyper_graphic_manager/rs_vblank_idle_corrector.h"
#ifdef RES_SCHED_ENABLE
#include "vsync_system_ability_listener.h"
#endif

namespace OHOS::Rosen {
using UniFallbackCallback = std::function<void(const sptr<Surface>& surface, const std::vector<LayerInfoPtr>& layers,
    uint32_t screenId)>;
using OutputPtr = std::shared_ptr<HdiOutput>;
using LayerPtr = std::shared_ptr<HdiLayer>;
class ScheduledTask;

struct RefreshRateParam {
    uint32_t rate = 0;
    uint64_t frameTimestamp = 0;
    int64_t actualTimestamp = 0;
    uint64_t vsyncId = 0;
    uint64_t constraintRelativeTime = 0;
    bool isForceRefresh = false;
    uint64_t fastComposeTimeStampDiff = 0;
};

class RSHardwareThread {
public:
    static RSHardwareThread& Instance();
    void Start();
    void PostTask(const std::function<void()>& task);
    void PostSyncTask(const std::function<void()>& task);
    void PostDelayTask(const std::function<void()>& task, int64_t delayTime);
    void CommitAndReleaseLayers(OutputPtr output, const std::vector<LayerInfoPtr>& layers);
    template<typename Task, typename Return = std::invoke_result_t<Task>>
    std::future<Return> ScheduleTask(Task&& task)
    {
        auto [scheduledTask, taskFuture] = Detail::ScheduledTask<Task>::Create(std::forward<Task&&>(task));
#ifdef RS_ENABLE_GPU
        PostTask([t(std::move(scheduledTask))]() { t->Run(); });
#endif
        return std::move(taskFuture);
    }
    uint32_t GetunExecuteTaskNum();
    void RefreshRateCounts(std::string& dumpString);
    void ClearRefreshRateCounts(std::string& dumpString);
    int GetHardwareTid() const;
    GSError ClearFrameBuffers(OutputPtr output);
    void OnScreenVBlankIdleCallback(ScreenId screenId, uint64_t timestamp);
    void ClearRedrawGPUCompositionCache(const std::set<uint32_t>& bufferIds);
    void DumpEventQueue();
    void PreAllocateProtectedBuffer(sptr<SurfaceBuffer> buffer, uint64_t screenId);
    void ChangeLayersForActiveRectOutside(std::vector<LayerInfoPtr>& layers, ScreenId screenId);
    void DumpVkImageInfo(std::string &dumpString);
private:
    RSHardwareThread() = default;
    ~RSHardwareThread() = default;
    RSHardwareThread(const RSHardwareThread&);
    RSHardwareThread(const RSHardwareThread&&);
    RSHardwareThread& operator=(const RSHardwareThread&);
    RSHardwareThread& operator=(const RSHardwareThread&&);

    void OnPrepareComplete(sptr<Surface>& surface, const PrepareCompleteParam& param, void* data);
    void Redraw(const sptr<Surface>& surface, const std::vector<LayerInfoPtr>& layers, uint32_t screenId);
    void RedrawScreenRCD(RSPaintFilterCanvas& canvas, const std::vector<LayerInfoPtr>& layers);
    void PerformSetActiveMode(OutputPtr output, uint64_t timestamp, uint64_t constraintRelativeTime);
    void ExecuteSwitchRefreshRate(const OutputPtr& output, uint32_t refreshRate);
    void ChangeDssRefreshRate(ScreenId screenId, uint32_t refreshRate, bool followPipline);
    void AddRefreshRateCount(const OutputPtr& output);
    void RecordTimestamp(const std::vector<LayerInfoPtr>& layers);
    int64_t GetCurTimeCount();
    int32_t AdaptiveModeStatus(const OutputPtr &output);

    RefreshRateParam GetRefreshRateParam();
    bool IsDelayRequired(OHOS::Rosen::HgmCore& hgmCore, RefreshRateParam param,
        const OutputPtr& output, bool hasGameScene);
    void CalculateDelayTime(OHOS::Rosen::HgmCore& hgmCore, RefreshRateParam param, uint32_t currentRate,
        int64_t currTime);
    std::shared_ptr<RSSurfaceOhos> CreateFrameBufferSurfaceOhos(const sptr<Surface>& surface);
    void ContextRegisterPostTask();
#ifdef RES_SCHED_ENABLE
    void SubScribeSystemAbility();
    sptr<VSyncSystemAbilityListener> saStatusChangeListener_ = nullptr;
#endif
#ifdef USE_VIDEO_PROCESSING_ENGINE
    static GraphicColorGamut ComputeTargetColorGamut(const std::vector<LayerInfoPtr>& layers);
    static GraphicPixelFormat ComputeTargetPixelFormat(const std::vector<LayerInfoPtr>& layers);
    static bool ConvertColorGamutToSpaceType(const GraphicColorGamut& colorGamut,
        HDI::Display::Graphic::Common::V1_0::CM_ColorSpaceType& colorSpaceInfo);
#endif

    static GraphicColorGamut ComputeTargetColorGamut(const sptr<SurfaceBuffer> &buffer);
    static GraphicPixelFormat ComputeTargetPixelFormat(const sptr<SurfaceBuffer> &buffer);
    std::shared_ptr<AppExecFwk::EventRunner> runner_ = nullptr;
    std::shared_ptr<AppExecFwk::EventHandler> handler_ = nullptr;
    HdiBackend *hdiBackend_ = nullptr;
    std::shared_ptr<RSBaseRenderEngine> uniRenderEngine_;
    UniFallbackCallback redrawCb_;
    std::mutex mutex_;
    std::atomic<uint32_t> unExecuteTaskNum_ = 0;
    int hardwareTid_ = -1;
    std::unordered_map<uint64_t, std::shared_ptr<RSSurfaceOhos>> frameBufferSurfaceOhosMap_;

    HgmRefreshRates hgmRefreshRates_ = HgmRefreshRates::SET_RATE_NULL;
    RSVBlankIdleCorrector vblankIdleCorrector_;

    std::map<uint32_t, uint64_t> refreshRateCounts_;
    sptr<SyncFence> releaseFence_ = SyncFence::InvalidFence();
    int64_t delayTime_ = 0;
    int64_t lastCommitTime_ = 0;
    int64_t intervalTimePoints_ = 0;
    bool isLastAdaptive_ = false;
    std::string GetSurfaceNameInLayers(const std::vector<LayerInfoPtr>& layers);
    std::mutex preAllocMutex_;
    std::mutex frameBufferSurfaceOhosMapMutex_;
    std::mutex surfaceMutex_;

    bool needRetrySetRate_ = false;

    std::unordered_map<ScreenId, OutputPtr> outputMap_;
    RefreshRateParam refreshRateParam_;

    friend class RSUniRenderThread;
    friend class RSUifirstManager;
};
}

namespace OHOS {
namespace AppExecFwk {
class RSHardwareDumper : public Dumper {
public:
    void Dump(const std::string& message) override;
    std::string GetTag() override;
    void PrintDumpInfo();
private:
    std::string dumpInfo_;
};
} // namespace AppExecFwk
} // namespace OHOS
#endif // RS_HARDWARE_THREAD_H
