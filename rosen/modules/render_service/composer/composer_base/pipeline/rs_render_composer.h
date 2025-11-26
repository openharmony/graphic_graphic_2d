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

#ifndef RENDER_SERVICE_COMPOSER_BASE_PIPELINE_RS_RENDER_COMPOSER_H
#define RENDER_SERVICE_COMPOSER_BASE_PIPELINE_RS_RENDER_COMPOSER_H

#include <mutex>
#include "common/rs_exception_check.h"
#include "event_handler.h"
#include "feature/hyper_graphic_manager/hgm_hardware_utils.h"
#include "hgm_core.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/render_thread/rs_base_render_engine.h"
#include "rs_render_composer_context.h"
#ifdef RES_SCHED_ENABLE
#include "vsync_system_ability_listener.h"
#endif

namespace OHOS::Rosen {
using ComposerFallbackCallback = std::function<void(const sptr<Surface>& surface,
    const std::vector<RSLayerPtr>& layers)>;

namespace Composer {
template<typename Task>
class ScheduledTask : public RefBase {
public:
    static auto Create(Task&& task)
    {
        sptr<ScheduledTask<Task>> t(new ScheduledTask(std::move(task)));
        return std::make_pair(t, t->task_.get_future());
    }
    void Run() { task_(); }

private:
    explicit ScheduledTask(Task&& task) : task_(std::move(task)) {}
    ~ScheduledTask() override = default;

    using Return = std::invoke_result_t<Task>;
    std::packaged_task<Return()> task_;
};
} // namespace Composer

class RSRenderComposer {
public:
    explicit RSRenderComposer(const std::shared_ptr<HdiOutput>& output);
    ~RSRenderComposer() = default;

protected:
    void ComposerPrepare(RefreshRateParam& param, uint32_t& currentRate, bool& hasGameScene, int64_t& delayTime);
    void ComposerProcess(RefreshRateParam param, uint32_t currentRate, bool hasGameScene,
        int64_t delayTime, std::shared_ptr<RSLayerTransactionData> transactionData);
    void PostTask(const std::function<void()>& task);
    void PostSyncTask(const std::function<void()>& task);
    void PostDelayTask(const std::function<void()>& task, int64_t delayTime);
    void OnScreenConnected(const std::shared_ptr<HdiOutput>& output);
    void OnScreenDisconnected();
    template<typename Task, typename Return = std::invoke_result_t<Task>>
    std::future<Return> ScheduleTask(Task&& task)
    {
        auto [scheduledTask, taskFuture] = Composer::ScheduledTask<Task>::Create(std::move(task));
#ifdef RS_ENABLE_GPU
        PostTask([t(std::move(scheduledTask))]() { t->Run(); });
#endif
        return std::move(taskFuture);
    }
    uint32_t GetUnExecuteTaskNum();
    int32_t GetAccumulatedBufferCount();
    void RefreshRateCounts(std::string& dumpString);
    void ClearRefreshRateCounts(std::string& dumpString);
    int32_t GetThreadTid() const;
    void OnScreenVBlankIdleCallback(uint64_t timestamp);
    GSError ClearFrameBuffers(bool isNeedResetContext = true);
    void ClearRedrawGPUCompositionCache(const std::set<uint64_t>& bufferIds);
    void PreAllocateProtectedBuffer(sptr<SurfaceBuffer> buffer);
    void ChangeLayersForActiveRectOutside(std::vector<std::shared_ptr<RSLayer>>& layers, ScreenId screenId);
    void DumpVkImageInfo(std::string& dumpString);
    int64_t GetDelayTime() { return delayTime_; }
    sptr<SyncFence> GetReleaseFence() const { return releaseFence_; }
    bool WaitComposerTaskExecute();
    void NotifyComposerCanExecuteTask();
    void CleanLayerBufferBySurfaceId(uint64_t surfaceId);
    void FpsDump(std::string& dumpString, std::string& layerName);
    void ClearFpsDump(std::string& dumpString, std::string& layerName);
    void DumpCurrentFrameLayers();
    void HitchsDump(std::string& dumpString, std::string& layerArg);
    void SetScreenPowerOnChanged(bool flag);

private:
    void CreateAndInitComposer(const std::shared_ptr<HdiOutput>& output);
#ifdef USE_VIDEO_PROCESSING_ENGINE
    static GraphicColorGamut ComputeTargetColorGamut(const std::vector<std::shared_ptr<RSLayer>>& layers);
    static bool IsAllRedraw(const std::vector<std::shared_ptr<RSLayer>>& layers);
    static GraphicPixelFormat ComputeTargetPixelFormat(const std::vector<std::shared_ptr<RSLayer>>& layers);
    static bool ConvertColorGamutToSpaceType(const GraphicColorGamut& colorGamut,
        HDI::Display::Graphic::Common::V1_0::CM_ColorSpaceType& colorSpaceType);
#endif
    std::shared_ptr<RSSurfaceOhos> CreateFrameBufferSurfaceOhos(const sptr<Surface>& surface);
    void RedrawScreenRCD(RSPaintFilterCanvas& canvas, const std::vector<std::shared_ptr<RSLayer>>& layers);
    void Redraw(const sptr<Surface>& surface, const std::vector<std::shared_ptr<RSLayer>>& layers);
    GraphicColorGamut ComputeTargetColorGamut(const sptr<SurfaceBuffer>& buffer);
    GraphicPixelFormat ComputeTargetPixelFormat(const sptr<SurfaceBuffer>& buffer);
    void OnPrepareComplete(sptr<Surface>& surface, const PrepareCompleteParam& param, void* data);
    void ContextRegisterPostTask();
#ifdef RES_SCHED_ENABLE
    void SubScribeSystemAbility();
    sptr<VSyncSystemAbilityListener> saStatusChangeListener_ = nullptr;
#endif
    int32_t AdaptiveModeStatus();
    bool IsDelayRequired(HgmCore& hgmCore, const RefreshRateParam& param, bool hasGameScene);
    void CalculateDelayTime(HgmCore& hgmCore, const RefreshRateParam& param,
        uint32_t currentRate, int64_t currTime);
    int64_t UpdateDelayTime(RefreshRateParam param, uint32_t currentRate, bool hasGameScene);
    void EndCheck(RSTimer timer);
    void UpdateTransactionData(std::shared_ptr<RSLayerTransactionData> transactionData);
    int64_t GetCurTimeCount();
    std::string GetSurfaceNameInLayers(const std::vector<std::shared_ptr<RSLayer>>& layers);
    std::string GetSurfaceNameInLayersForTrace(const std::vector<std::shared_ptr<RSLayer>>& layers);
    bool IsDropDirtyFrame(const std::vector<std::shared_ptr<RSLayer>>& layers);
    void RecordTimestamp(const std::vector<std::shared_ptr<RSLayer>>& layers);
    void ProcessComposerFrame(RefreshRateParam param, uint32_t currentRate, bool hasGameScene);
    void AddRefreshRateCount();
    std::shared_ptr<AppExecFwk::EventRunner> runner_ = nullptr;
    std::shared_ptr<AppExecFwk::EventHandler> handler_ = nullptr;
    std::shared_ptr<HdiOutput> hdiOutput_;
    int32_t threadTid_ = -1;
    ScreenId screenId_ = INVALID_SCREEN_ID;
    std::shared_ptr<RSRenderComposerContext> rsRenderComposerContext_;
    std::shared_ptr<RSBaseRenderEngine> uniRenderEngine_;
    HgmHardwareUtils hgmHardwareUtils_;
    ComposerFallbackCallback redrawCb_;
    std::mutex surfaceMutex_;
    std::mutex preAllocMutex_;
    std::map<uint32_t, uint64_t> refreshRateCounts_;
    std::atomic<uint32_t> unExecuteTaskNum_ = 0;
    // used for blocking mainThread when composer has 2 and more task to Execute
    mutable std::mutex composerTaskMutex_;
    std::condition_variable composerTaskCond_;
    std::atomic<int32_t> acquiredBufferCount_ = 0;
    int64_t delayTime_ = 0;
    int64_t lastCommitTime_ = 0;
    int64_t intervalTimePoints_ = 0;
    int64_t lastActualTime_ = 0;
    std::mutex frameBufferSurfaceOhosMapMutex_;
    std::unordered_map<uint64_t, std::shared_ptr<RSSurfaceOhos>> frameBufferSurfaceOhosMap_;
    int exceptionCnt_ = 0;
    ExceptionCheck exceptionCheck_;
    sptr<SyncFence> releaseFence_ = SyncFence::InvalidFence();
    bool isDisconnected_ = false;
    friend class RSRenderComposerAgent;
};
} // namespace OHOS::Rosen

#endif // RENDER_SERVICE_COMPOSER_BASE_PIPELINE_RS_RENDER_COMPOSER_H