/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef RS_MAIN_THREAD
#define RS_MAIN_THREAD

#include <event_handler.h>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <set>
#include <thread>

#include "refbase.h"
#include "pipeline/render_thread/rs_base_render_engine.h"
#include "pipeline/render_thread/rs_draw_frame.h"
#include "vsync_distributor.h"
#include "vsync_receiver.h"

#include "command/rs_command.h"
#include "common/rs_common_def.h"
#include "common/rs_thread_handler.h"
#include "common/rs_thread_looper.h"
#include "drawable/rs_render_node_drawable_adapter.h"
#include "ipc_callbacks/iapplication_agent.h"
#include "ipc_callbacks/rs_iocclusion_change_callback.h"
#include "ipc_callbacks/rs_isurface_occlusion_change_callback.h"
#include "ipc_callbacks/rs_iuiextension_callback.h"
#include "memory/rs_app_state_listener.h"
#include "memory/rs_memory_graphic.h"
#include "params/rs_render_thread_params.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "pipeline/hwc/rs_direct_composition_helper.h"
#include "feature/hyper_graphic_manager/hgm_context.h"
#include "feature/vrate/rs_vsync_rate_reduce_manager.h"
#include "platform/common/rs_event_manager.h"
#include "screen_manager/rs_screen_node_listener.h"
#include "platform/drawing/rs_vsync_client.h"
#include "transaction/rs_transaction_data.h"
#include "transaction/rs_uiextension_data.h"

#ifdef RES_SCHED_ENABLE
#include "vsync_system_ability_listener.h"
#endif

#include "hgm_core.h"

namespace OHOS::Rosen {
#if defined(ACCESSIBILITY_ENABLE)
class AccessibilityObserver;
#endif
class HgmFrameRateManager;
class RSUniRenderVisitor;
struct FrameRateRangeData;
namespace Detail {
template<typename Task>
class ScheduledTask : public RefBase {
public:
    static auto Create(Task&& task)
    {
        sptr<ScheduledTask<Task>> t(new ScheduledTask(std::forward<Task&&>(task)));
        return std::make_pair(t, t->task_.get_future());
    }

    void Run()
    {
        task_();
    }

private:
    explicit ScheduledTask(Task&& task) : task_(std::move(task)) {}
    ~ScheduledTask() override = default;

    using Return = std::invoke_result_t<Task>;
    std::packaged_task<Return()> task_;
};
} // namespace Detail

class RSMainThread {
public:
    static RSMainThread* Instance();

    void Init();
    void Start();
    bool IsNeedProcessBySingleFrameComposer(std::unique_ptr<RSTransactionData>& rsTransactionData);
    void UpdateFocusNodeId(NodeId focusNodeId);
    void UpdateNeedDrawFocusChange(NodeId id);
    void ProcessDataBySingleFrameComposer(std::unique_ptr<RSTransactionData>& rsTransactionData);
    void RecvRSTransactionData(std::unique_ptr<RSTransactionData>& rsTransactionData);
    void RequestNextVSync(
        const std::string& fromWhom = "unknown", int64_t lastVSyncTS = 0, const int64_t& requestVsyncTime = 0);
    void PostTask(RSTaskMessage::RSTask task);
    void PostTask(RSTaskMessage::RSTask task, const std::string& name, int64_t delayTime,
        AppExecFwk::EventQueue::Priority priority = AppExecFwk::EventQueue::Priority::IDLE);
    void RemoveTask(const std::string& name);
    void PostSyncTask(RSTaskMessage::RSTask task);
    bool IsIdle() const;
    void TransactionDataMapDump(const TransactionDataMap& transactionDataMap, std::string& dumpString);
    void RenderServiceTreeDump(std::string& dumpString, bool forceDumpSingleFrame = true,
        bool needUpdateJankStats = false);
    void RenderServiceAllNodeDump(DfxString& log);
    void RenderServiceAllSurafceDump(DfxString& log);
    void SendClientDumpNodeTreeCommands(uint32_t taskId);
    void CollectClientNodeTreeResult(uint32_t taskId, std::string& dumpString, size_t timeout);
    void RsEventParamDump(std::string& dumpString);
    void UpdateAnimateNodeFlag();
    void ResetAnimateNodeFlag();
    void GetAppMemoryInMB(float& cpuMemSize, float& gpuMemSize);
    void ClearMemoryCache(ClearMemoryMoment moment, bool deeply = false, pid_t pid = -1);
    void SetForceRsDVsync();

    template<typename Task, typename Return = std::invoke_result_t<Task>>
    std::future<Return> ScheduleTask(Task&& task)
    {
        auto [scheduledTask, taskFuture] = Detail::ScheduledTask<Task>::Create(std::forward<Task&&>(task));
        PostTask([t(std::move(scheduledTask))]() { t->Run(); });
        return std::move(taskFuture);
    }

    const std::shared_ptr<RSBaseRenderEngine> GetRenderEngine() const
    {
        RS_LOGD("You'd better to call GetRenderEngine from RSUniRenderThread directly");
#ifdef RS_ENABLE_GPU
        return isUniRender_ ? std::move(RSUniRenderThread::Instance().GetRenderEngine()) : renderEngine_;
#else
        return renderEngine_;
#endif
    }

    bool GetClearMemoryFinished() const
    {
        return clearMemoryFinished_;
    }

    RSContext& GetContext()
    {
        return *context_;
    }

    std::thread::id Id() const
    {
        return mainThreadId_;
    }

    void SetGlobalDarkColorMode(bool isDark)
    {
        isGlobalDarkColorMode_ = isDark;
    }

    bool GetGlobalDarkColorMode() const
    {
        return isGlobalDarkColorMode_;
    }

    /* Judge if rootnode has to be prepared based on it corresponding process is active
     * If its pid is in activeProcessPids_ set, return true
     */
    bool CheckNodeHasToBePreparedByPid(NodeId nodeId, bool isClassifyByRoot);
    // check if active app has static drawing cache
    bool IsDrawingGroupChanged(const RSRenderNode& cacheRootNode) const;
    // check if active instance only move or scale it's main window surface without rearrangement
    // instanceNodeId should be MainWindowType, or it cannot grep correct app's info
    void CheckAndUpdateInstanceContentStaticStatus(std::shared_ptr<RSSurfaceRenderNode> instanceNode) const;

    void RegisterApplicationAgent(uint32_t pid, sptr<IApplicationAgent> app);
    void UnRegisterApplicationAgent(sptr<IApplicationAgent> app);

    void RegisterOcclusionChangeCallback(pid_t pid, sptr<RSIOcclusionChangeCallback> callback);
    void UnRegisterOcclusionChangeCallback(pid_t pid);

    void RegisterSurfaceOcclusionChangeCallback(
        NodeId id, pid_t pid, sptr<RSISurfaceOcclusionChangeCallback> callback, std::vector<float>& partitionPoints);
    void UnRegisterSurfaceOcclusionChangeCallback(NodeId id);
    void ClearSurfaceOcclusionChangeCallback(pid_t pid);
    bool SurfaceOcclusionCallBackIfOnTreeStateChanged();

    void ClearTransactionDataPidInfo(pid_t remotePid);
    void AddTransactionDataPidInfo(pid_t remotePid);

    void SetFocusAppInfo(const FocusAppInfo& info);
    const std::unordered_map<NodeId, bool>& GetCacheCmdSkippedNodes() const;

    sptr<VSyncDistributor> rsVSyncDistributor_;
    sptr<VSyncController> rsVSyncController_;
    sptr<VSyncController> appVSyncController_;
    sptr<VSyncGenerator> vsyncGenerator_;

    void ReleaseSurface();
    void AddToReleaseQueue(std::shared_ptr<Drawing::Surface>&& surface);

    void AddUiCaptureTask(NodeId id, std::function<void()> task);
    void ProcessUiCaptureTasks();

    void SetDirtyFlag(bool isDirty = true);
    bool GetDirtyFlag();
    void SetAccessibilityConfigChanged();
    void SetScreenPowerOnChanged(bool val);
    bool GetScreenPowerOnChanged() const;
    bool IsAccessibilityConfigChanged() const;
    bool IsCurtainScreenUsingStatusChanged() const;
    bool IsFastComposeAllow(uint64_t unsignedVsyncPeriod, bool nextVsyncRequested,
        uint64_t unsignedNowTime, uint64_t lastVsyncTime);
    // check if timestamp in vsync receiver sync with mainthread timestamp, if not, return false;
    bool IsFastComposeVsyncTimeSync(uint64_t unsignedVsyncPeriod, bool nextVsyncRequested,
        uint64_t unsignedNowTime, uint64_t lastVsyncTime, int64_t vsyncTimeStamp);
    void CheckFastCompose(int64_t bufferTimeStamp);
    bool CheckAdaptiveCompose();
    void ForceRefreshForUni(bool needDelay = false);
    void TrimMem(std::unordered_set<std::u16string>& argSets, std::string& result);
    void DumpMem(std::unordered_set<std::u16string>& argSets, std::string& result, std::string& type, pid_t pid = 0);
    void DumpNode(std::string& result, uint64_t nodeId) const;
    void CountMem(int pid, MemoryGraphic& mem);
    void CountMem(std::vector<MemoryGraphic>& mems);
    void SetAppWindowNum(uint32_t num);
    bool SetSystemAnimatedScenes(SystemAnimatedScenes systemAnimatedScenes, bool isRegularAnimation = false);
    SystemAnimatedScenes GetSystemAnimatedScenes();
    bool GetIsRegularAnimation() const;
    // Save marks, and use it for SurfaceNodes later.
    void SetWatermark(const std::string& name, std::shared_ptr<Media::PixelMap> watermark);
    // Save marks, and use it for ScreenNode later.
    void ShowWatermark(const std::shared_ptr<Media::PixelMap> &watermarkImg, bool flag);
    void SetIsCachedSurfaceUpdated(bool isCachedSurfaceUpdated);
    pid_t GetDesktopPidForRotationScene() const;
    void SetForceUpdateUniRenderFlag(bool flag)
    {
        forceUpdateUniRenderFlag_ = flag;
    }
    std::shared_ptr<Drawing::Image> GetWatermarkImg();
    bool GetWatermarkFlag();

    bool IsWatermarkFlagChanged() const
    {
        return lastWatermarkFlag_ != watermarkFlag_;
    }

    uint64_t GetFrameCount() const
    {
        return frameCount_;
    }
    std::vector<NodeId>& GetDrawStatusVec()
    {
        return curDrawStatusVec_;
    }
    void SetAppVSyncDistributor(const sptr<VSyncDistributor>& appVSyncDistributor)
    {
        appVSyncDistributor_ = appVSyncDistributor;
    }

    DeviceType GetDeviceType() const;
    bool IsSingleDisplay();
    bool HasMirrorDisplay() const;
    uint64_t GetFocusNodeId() const;
    uint64_t GetFocusLeashWindowId() const;
    bool GetClearMemDeeply() const
    {
        return clearMemDeeply_;
    }

    ClearMemoryMoment GetClearMoment() const
    {
        if (!context_) {
            return ClearMemoryMoment::NO_CLEAR;
        }
        return context_->clearMoment_;
    }

    void SetClearMoment(ClearMemoryMoment moment)
    {
        if (!context_) {
            return;
        }
        context_->clearMoment_ = moment;
    }

    bool IsPCThreeFingerScenesListScene() const
    {
        return !threeFingerScenesList_.empty();
    }

    void SurfaceOcclusionChangeCallback(VisibleData& dstCurVisVec);
    void SurfaceOcclusionCallback();
    bool CheckSurfaceOcclusionNeedProcess(NodeId id);
    void SubscribeAppState();
    void HandleOnTrim(Memory::SystemMemoryLevel level);
    void SetCurtainScreenUsingStatus(bool isCurtainScreenOn);
    void AddPidNeedDropFrame(std::vector<int32_t> pid);
    void ClearNeedDropframePidList();
    bool IsNeedDropFrameByPid(NodeId nodeId);
    void SetLuminanceChangingStatus(ScreenId id, bool isLuminanceChanged);
    bool ExchangeLuminanceChangingStatus(ScreenId id);
    bool IsCurtainScreenOn() const;

    bool GetParallelCompositionEnabled();
    void SetFrameIsRender(bool isRender);
    void AddSelfDrawingNodes(std::shared_ptr<RSSurfaceRenderNode> selfDrawingNode);
    const std::vector<std::shared_ptr<RSSurfaceRenderNode>>& GetSelfDrawingNodes() const;
    void ClearSelfDrawingNodes();

    bool GetDiscardJankFrames() const
    {
        return discardJankFrames_.load();
    }

    void SetDiscardJankFrames(bool discardJankFrames)
    {
        if (discardJankFrames_.load() != discardJankFrames) {
            discardJankFrames_.store(discardJankFrames);
        }
    }

    bool GetSkipJankAnimatorFrame() const
    {
        return skipJankAnimatorFrame_.load();
    }

    bool IsFirstFrameOfDrawingCacheDFXSwitch() const
    {
        return isDrawingCacheDfxEnabledOfCurFrame_ != isDrawingCacheDfxEnabledOfLastFrame_;
    }

    void SetDrawingCacheDfxEnabledOfCurFrame(bool isDrawingCacheDfxEnabledOfCurFrame)
    {
        isDrawingCacheDfxEnabledOfCurFrame_ = isDrawingCacheDfxEnabledOfCurFrame;
    }

    void SetSkipJankAnimatorFrame(bool skipJankAnimatorFrame)
    {
        skipJankAnimatorFrame_.store(skipJankAnimatorFrame);
    }

    bool IsRequestedNextVSync();

    bool IsOcclusionNodesNeedSync(NodeId id, bool useCurWindow);

    void CallbackDrawContextStatusToWMS(bool isUniRender = false);
    void SetHardwareTaskNum(uint32_t num);
    void RegisterUIExtensionCallback(pid_t pid, uint64_t userId, sptr<RSIUIExtensionCallback> callback,
        bool unobscured = false);
    void UnRegisterUIExtensionCallback(pid_t pid);

    void SetAncoForceDoDirect(bool direct);

    bool IsSystemAnimatedScenesListEmpty() const
    {
        return systemAnimatedScenesList_.empty();
    }

    RSVsyncRateReduceManager& GetRSVsyncRateReduceManager()
    {
        return rsVsyncRateReduceManager_;
    }

    bool IsFirstFrameOfOverdrawSwitch() const
    {
        return isOverDrawEnabledOfCurFrame_ != isOverDrawEnabledOfLastFrame_;
    }

    uint64_t GetRealTimeOffsetOfDvsync(int64_t time);

    bool GetMultiDisplayChange() const
    {
        return isMultiDisplayChange_;
    }
    bool GetMultiDisplayStatus() const
    {
        return isMultiDisplayPre_;
    }

    bool HasWiredMirrorDisplay() const
    {
        return hasWiredMirrorDisplay_;
    }
    bool HasVirtualMirrorDisplay() const
    {
        return hasVirtualMirrorDisplay_;
    }
    uint64_t GetCurrentVsyncTime() const
    {
        return curTime_;
    }

    void StartGPUDraw();

    void EndGPUDraw();

    struct GPUCompositonCacheGuard {
        GPUCompositonCacheGuard()
        {
            RSMainThread::Instance()->StartGPUDraw();
        }

        ~GPUCompositonCacheGuard()
        {
            RSMainThread::Instance()->EndGPUDraw();
        }
    };

    void AddToUnmappedCacheSet(uint32_t bufferId)
    {
        std::lock_guard<std::mutex> lock(unmappedCacheSetMutex_);
        unmappedCacheSet_.insert(bufferId);
    }

    void AddToUnmappedCacheSet(const std::set<uint32_t>& seqNumSet)
    {
        std::lock_guard<std::mutex> lock(unmappedCacheSetMutex_);
        unmappedCacheSet_.insert(seqNumSet.begin(), seqNumSet.end());
    }

    void ClearUnmappedCache();
    void InitVulkanErrorCallback(Drawing::GPUContext* gpuContext);
    void NotifyUnmarshalTask(int64_t uiTimestamp);
    void NotifyPackageEvent(const std::vector<std::string>& packageList);
    void HandleTouchEvent(int32_t touchStatus, int32_t touchCnt);
    void SetBufferInfo(uint64_t id, const std::string &name, uint32_t queueSize,
        int32_t bufferCount, int64_t lastConsumeTime, bool isUrgent);
    void GetFrontBufferDesiredPresentTimeStamp(
        const sptr<IConsumerSurface>& consumer, int64_t& desiredPresentTimeStamp);
    void SetBufferQueueInfo(const std::string &name, int32_t bufferCount, int64_t lastFlushedTimeStamp);

    // Enable HWCompose
    bool IsHardwareEnabledNodesNeedSync();
    bool WaitHardwareThreadTaskExecute();
    void NotifyHardwareThreadCanExecuteTask();
    void SetTaskEndWithTime(int64_t time);

    uint32_t GetVsyncRefreshRate();
    void DVSyncUpdate(uint64_t dvsyncTime, uint64_t vsyncTime);

private:
    using TransactionDataIndexMap = std::unordered_map<pid_t,
        std::pair<uint64_t, std::vector<std::unique_ptr<RSTransactionData>>>>;
    using DrawablesVec = std::vector<std::tuple<NodeId, NodeId,
        DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr>>;

    RSMainThread();
    ~RSMainThread() noexcept;
    RSMainThread(const RSMainThread&) = delete;
    RSMainThread(const RSMainThread&&) = delete;
    RSMainThread& operator=(const RSMainThread&) = delete;
    RSMainThread& operator=(const RSMainThread&&) = delete;

    void OnVsync(uint64_t timestamp, uint64_t frameCount, void* data);
    void ProcessCommand();
    void UpdateSubSurfaceCnt();
    void Animate(uint64_t timestamp);
    void ConsumeAndUpdateAllNodes();
    void ReleaseAllNodesBuffer();
    void Render();
    void OnUniRenderDraw();
    void SetDeviceType();
    void UniRender(std::shared_ptr<RSBaseRenderNode> rootNode);
    bool CheckSurfaceNeedProcess(OcclusionRectISet& occlusionSurfaces, std::shared_ptr<RSSurfaceRenderNode> curSurface);
    RSVisibleLevel CalcSurfaceNodeVisibleRegion(const std::shared_ptr<RSScreenRenderNode>& screenNode,
        const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode, Occlusion::Region& accumulatedRegion,
        Occlusion::Region& curRegion, Occlusion::Region& totalRegion);
    void CalcOcclusionImplementation(const std::shared_ptr<RSScreenRenderNode>& screenNode,
        std::vector<RSBaseRenderNode::SharedPtr>& curAllSurfaces, VisibleData& dstCurVisVec,
        std::map<NodeId, RSVisibleLevel>& dstPidVisMap);
    void CalcOcclusion();
    void CallbackToWMS(VisibleData& curVisVec);
    void SendCommands();
    void InitRSEventDetector();
    void RemoveRSEventDetector();
    void SetRSEventDetectorLoopStartTag();
    void SetRSEventDetectorLoopFinishTag();
    void CheckSystemSceneStatus();
    void RegisterScreenNodeListener();
    void UpdateScreenNodeScreenId();

    uint32_t GetRefreshRate() const;
    uint32_t GetDynamicRefreshRate() const;
    void SkipCommandByNodeId(std::vector<std::unique_ptr<RSTransactionData>>& transactionVec, pid_t pid);
    static void OnHideNotchStatusCallback(const char *key, const char *value, void *context);
    static void OnDrawingCacheDfxSwitchCallback(const char *key, const char *value, void *context);
    static void OnFmtTraceSwitchCallback(const char *key, const char *value, void *context);

    bool DoParallelComposition(std::shared_ptr<RSBaseRenderNode> rootNode);

    void ClassifyRSTransactionData(std::shared_ptr<RSTransactionData> rsTransactionData);
    void ProcessRSTransactionData(std::unique_ptr<RSTransactionData>& rsTransactionData, pid_t pid);
    void ProcessSyncRSTransactionData(std::unique_ptr<RSTransactionData>& rsTransactionData, pid_t pid);
    void ProcessSyncTransactionCount(std::unique_ptr<RSTransactionData>& rsTransactionData);
    void StartSyncTransactionFallbackTask(std::unique_ptr<RSTransactionData>& rsTransactionData);
    void ProcessAllSyncTransactionData();
    void ProcessCommandForDividedRender();
    void ProcessCommandForUniRender();
    void WaitUntilUnmarshallingTaskFinished();
    void MergeToEffectiveTransactionDataMap(TransactionDataMap& cachedTransactionDataMap);

    void ClearDisplayBuffer();
    void PerfAfterAnim(bool needRequestNextVsync);
    void PerfForBlurIfNeeded();
    void PerfMultiWindow();
    void RenderFrameStart(uint64_t timestamp);
    void CheckAndUpdateTransactionIndex(
        std::shared_ptr<TransactionDataMap>& transactionDataEffective, std::string& transactionFlags);

    bool IsResidentProcess(pid_t pid) const;
    bool IsNeedSkip(NodeId instanceRootNodeId, pid_t pid);
    uint32_t GetForceCommitReason() const;

    // UIFirst
    bool CheckParallelSubThreadNodesStatus();
    void CacheCommands();
    bool IsSurfaceConsumerNeedSkip(sptr<IConsumerSurface> consumer);
    bool CheckSubThreadNodeStatusIsDoing(NodeId appNodeId) const;

    // used for informing hgm the bundle name of SurfaceRenderNodes
    void InformHgmNodeInfo();
    void CheckIfNodeIsBundle(std::shared_ptr<RSSurfaceRenderNode> node);

    void TraverseCanvasDrawingNodesNotOnTree();

    void SetFocusLeashWindowId();
    void ProcessHgmFrameRate(uint64_t timestamp);
    bool IsLastFrameUIFirstEnabled(NodeId appNodeId) const;
    RSVisibleLevel GetRegionVisibleLevel(const Occlusion::Region& curRegion,
        const Occlusion::Region& visibleRegion);
    void PrintCurrentStatus();
    void UpdateGpuContextCacheSize();
    void ProcessScreenHotPlugEvents();
    void WaitUntilUploadTextureTaskFinishedForGL();
#ifdef RES_SCHED_ENABLE
    void SubScribeSystemAbility();
#endif
#if defined(RS_ENABLE_CHIPSET_VSYNC)
    void ConnectChipsetVsyncSer();
    void SetVsyncInfo(uint64_t timestamp);
#endif

    void RSJankStatsOnVsyncStart(int64_t onVsyncStartTime, int64_t onVsyncStartTimeSteady,
                                 float onVsyncStartTimeSteadyFloat);
    void RSJankStatsOnVsyncEnd(int64_t onVsyncStartTime, int64_t onVsyncStartTimeSteady,
                               float onVsyncStartTimeSteadyFloat);
    int64_t GetCurrentSystimeMs() const;
    int64_t GetCurrentSteadyTimeMs() const;
    float GetCurrentSteadyTimeMsFloat() const;
    void RequestNextVsyncForCachedCommand(std::string& transactionFlags, pid_t pid, uint64_t curIndex);
    void UpdateLuminanceAndColorTemp();

    void PrepareUiCaptureTasks(std::shared_ptr<RSUniRenderVisitor> uniVisitor);
    void UIExtensionNodesTraverseAndCallback();
    bool CheckUIExtensionCallbackDataChanged() const;
    void RequestNextVSyncInner(VSyncReceiver::FrameCallback callback,
        const std::string& fromWhom = "unknown", int64_t lastVSyncTS = 0, const int64_t& requestVsyncTime = 0);

    void CheckBlurEffectCountStatistics(std::shared_ptr<RSBaseRenderNode> rootNode);
    void OnCommitDumpClientNodeTree(NodeId nodeId, pid_t pid, uint32_t taskId, const std::string& result);

    // Used for CommitAndReleaseLayers task
    void SetFrameInfo(uint64_t frameCount, bool forceRefreshFlag);

    // Record change status of multi or single display
    void MultiDisplayChange(bool isMultiDisplay);
    void DumpEventHandlerInfo();
    std::string SubHistoryEventQueue(std::string input);
    std::string SubPriorityEventQueue(std::string input);

    // Enable HWCompose
    void CollectInfoForHardwareComposer();
    void ResetHardwareEnabledState(bool isUniRender);
    void CheckIfHardwareForcedDisabled();
    bool DoDirectComposition(std::shared_ptr<RSBaseRenderNode> rootNode, bool waitForRT);
    bool ExistBufferIsVisibleAndUpdate();
    bool NeedConsumeMultiCommand(int32_t& dvsyncPid);
    bool NeedConsumeDVSyncCommand(uint32_t& endIndex,
        std::vector<std::unique_ptr<RSTransactionData>>& transactionVec);
    class RSScreenNodeListener : public RSIScreenNodeListener {
    public:
        ~RSScreenNodeListener() override = default;

        void OnScreenConnect(ScreenId id) override;
        void OnScreenDisconnect(ScreenId id) override;
    };

    void UpdateDirectCompositionByAnimate(bool animateNeedRequestNextVsync);
    void HandleTunnelLayerId(const std::shared_ptr<RSSurfaceHandler>& surfaceHandler,
        const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode);

    bool isUniRender_ = RSUniRenderJudgement::IsUniRender();
    bool needWaitUnmarshalFinished_ = true;
    bool clearMemoryFinished_ = true;
    bool clearMemDeeply_ = false;
    // Used to refresh the whole display when AccessibilityConfig is changed
    bool isAccessibilityConfigChanged_ = false;
    // Used to refresh the whole display when curtain screen status is changed
    bool isCurtainScreenUsingStatusChanged_ = false;

    const uint8_t opacity_ = 255;
    bool vsyncControlEnabled_ = true;
    bool systemAnimatedScenesEnabled_ = false;
    bool isFoldScreenDevice_ = false;
    mutable std::atomic_bool hasWiredMirrorDisplay_ = false;
    mutable std::atomic_bool hasVirtualMirrorDisplay_ = false;

#ifdef RS_ENABLE_GPU
    bool needDrawFrame_ = true;
    bool needPostAndWait_ = true;
    bool isLastFrameNeedPostAndWait_ = true;
#endif

    bool isNeedResetClearMemoryTask_ = false;
    bool watermarkFlag_ = false;
    bool lastWatermarkFlag_ = false;
    bool hasProtectedLayer_ = false;
    DeviceType deviceType_ = DeviceType::PHONE;
    bool isCachedSurfaceUpdated_ = false;
    // used for informing hgm the bundle name of SurfaceRenderNodes
    bool forceUpdateUniRenderFlag_ = false;
    // for drawing cache dfx
    bool isDrawingCacheDfxEnabledOfCurFrame_ = false;
    bool isDrawingCacheDfxEnabledOfLastFrame_ = false;
    // for dvsync (animate requestNextVSync after mark rsnotrendering)
    bool forceUIFirstChanged_ = false;
    bool lastFrameUIExtensionDataEmpty_ = false;
    // overDraw
    bool isOverDrawEnabledOfCurFrame_ = false;
    bool isOverDrawEnabledOfLastFrame_ = false;
#if defined(RS_ENABLE_CHIPSET_VSYNC)
    bool initVsyncServiceFlag_ = true;
#endif
    bool isCurtainScreenOn_ = false;
    // partial render
    bool isForceRefresh_ = false;
    // record multidisplay status change
    bool isMultiDisplayPre_ = false;
    bool isMultiDisplayChange_ = false;
#ifdef RS_ENABLE_VK
    bool needCreateVkPipeline_ = true;
#endif
    std::atomic<bool> isDirty_ = false;
    bool prevHdrSwitchStatus_ = true;
    std::atomic<bool> screenPowerOnChanged_ = false;
    std::atomic_bool doWindowAnimate_ = false;
    std::atomic<bool> isGlobalDarkColorMode_ = false;
    // for statistic of jank frames
    std::atomic_bool discardJankFrames_ = false;
    std::atomic_bool skipJankAnimatorFrame_ = false;
    bool isImplicitAnimationEnd_ = false;

    pid_t lastCleanCachePid_ = -1;
    int32_t unmarshalFinishedCount_ = 0;
    uint32_t appWindowNum_ = 0;
    pid_t desktopPidForRotationScene_ = 0;
    int32_t subscribeFailCount_ = 0;
    SystemAnimatedScenes systemAnimatedScenes_ = SystemAnimatedScenes::OTHERS;
    bool isRegularAnimation_ = false;
    uint32_t leashWindowCount_ = 0;
    pid_t exitedPid_ = -1;
    RsParallelType rsParallelType_;
    std::atomic<int32_t> focusAppPid_ = -1;
    std::atomic<int32_t> focusAppUid_ = -1;
    std::atomic<uint32_t> requestNextVsyncNum_ = 0;
    std::atomic<uint32_t> drawingRequestNextVsyncNum_ = 0;
    uint64_t curTime_ = 0;
    uint64_t timestamp_ = 0;
    uint64_t vsyncId_ = 0;
    uint64_t lastAnimateTimestamp_ = 0;
    uint64_t prePerfTimestamp_ = 0;
    uint64_t lastCleanCacheTimestamp_ = 0;
    uint64_t focusLeashWindowId_ = 0;
    std::string focusLeashWindowName_ = "";
    std::string appWindowName_ = "";
    uint32_t appPid_ = 0;
    uint64_t lastFocusNodeId_ = 0;
    uint64_t appWindowId_ = 0;
    ScreenId screenNodeScreenId_ = 0;
    std::atomic<uint64_t> focusNodeId_ = 0;
    std::atomic<uint64_t> frameCount_ = 0;
    std::atomic<bool> isRunning_ = false;
    std::shared_ptr<AppExecFwk::EventRunner> runner_ = nullptr;
    std::shared_ptr<AppExecFwk::EventHandler> handler_ = nullptr;
    std::shared_ptr<RSContext> context_;
    std::shared_ptr<VSyncReceiver> receiver_ = nullptr;
    sptr<VSyncDistributor> appVSyncDistributor_ = nullptr;
    std::shared_ptr<RSBaseRenderEngine> renderEngine_;
    std::shared_ptr<RSBaseEventDetector> rsCompositionTimeoutDetector_;
    std::shared_ptr<Drawing::Image> watermarkImg_ = nullptr; // display safterWatermask(true) or hide it(false)
    std::shared_ptr<RSAppStateListener> rsAppStateListener_;
    std::unique_ptr<RSVsyncClient> vsyncClient_ = nullptr;
    RSTaskMessage::RSTask mainLoop_;
    std::unordered_map<NodeId, uint64_t> dividedRenderbufferTimestamps_;

    std::mutex transitionDataMutex_;
    std::unordered_map<NodeId, std::map<uint64_t, std::vector<std::unique_ptr<RSCommand>>>> cachedCommands_;
    std::map<uint64_t, std::vector<std::unique_ptr<RSCommand>>> effectiveCommands_;
    std::map<uint64_t, std::vector<std::unique_ptr<RSCommand>>> pendingEffectiveCommands_;
    std::unordered_map<pid_t, std::vector<std::unique_ptr<RSTransactionData>>> syncTransactionData_;
    std::unordered_map<int32_t, int32_t> subSyncTransactionCounts_;

    TransactionDataMap cachedTransactionDataMap_;
    TransactionDataIndexMap effectiveTransactionDataIndexMap_;
    std::map<pid_t, std::vector<std::unique_ptr<RSTransactionData>>> cachedSkipTransactionDataMap_;
    std::unordered_map<pid_t, uint64_t> transactionDataLastWaitTime_;

    int64_t requestNextVsyncTime_ = -1;
    bool isHdrSwitchChanged_ = false;
    bool isColorTemperatureOn_ = false;

    /**
     * @brief A set to store buffer IDs of images that are about to be unmapped from GPU cache.
     *
     * This set is used to track images that are no longer needed and should be removed from the GPU cache.
     * When an image is unmapped, its buffer ID is added to this set. During the rendering process,
     * if an image is found in this set, it means that the image is no longer needed and can be safely
     * removed from the GPU cache.
     */
    std::set<uint32_t> unmappedCacheSet_ = {}; // must protected by unmappedCacheSetMutex_
    std::mutex unmappedCacheSetMutex_;

    /**
     * @brief An atomic integer to keep track of the GPU draw count.
     *
     * This variable is used to safely increment and decrement the count of GPU draw operations
     * across multiple threads without causing data races.
     */
    std::atomic<int> gpuDrawCount_ = 0;

    std::string transactionFlags_ = "";
    std::unordered_map<uint32_t, sptr<IApplicationAgent>> applicationAgentMap_;

    std::thread::id mainThreadId_;
    std::map<pid_t, sptr<RSIOcclusionChangeCallback>> occlusionListeners_;
    std::mutex occlusionMutex_;

    RSTaskMessage::RSTask unmarshalBarrierTask_;
    std::condition_variable unmarshalTaskCond_;
    std::mutex unmarshalMutex_;

#if defined(RS_ENABLE_PARALLEL_UPLOAD) && defined(RS_ENABLE_GL)
    RSTaskMessage::RSTask uploadTextureBarrierTask_;
    std::condition_variable uploadTextureTaskCond_;
    std::mutex uploadTextureMutex_;
    int32_t uploadTextureFinishedCount_ = 0;
    EGLSyncKHR uploadTextureFence;
#endif

    mutable std::mutex uniRenderMutex_;

    // Used to refresh the whole display when luminance is changed
    std::unordered_map<ScreenId, bool> displayLuminanceChanged_;
    std::mutex luminanceMutex_;

    // used for blocking mainThread when hardwareThread has 2 and more task to Execute
    mutable std::mutex hardwareThreadTaskMutex_;
    std::condition_variable hardwareThreadTaskCond_;

    VisibleData lastVisVec_;
    std::map<NodeId, uint64_t> lastDrawStatusMap_;
    std::vector<NodeId> curDrawStatusVec_;
    std::vector<NodeId> lastSurfaceIds_;
    std::string focusAppBundleName_ = "";
    std::string focusAppAbilityName_ = "";

    RSEventManager rsEventManager_;

    std::vector<std::shared_ptr<RSSurfaceRenderNode>> selfDrawingNodes_;
#ifdef RS_ENABLE_GPU
    std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr> selfDrawables_;
#endif

    // Enable HWCompose
    std::vector<std::shared_ptr<RSSurfaceRenderNode>> hardwareEnabledNodes_;
    DrawablesVec hardwareEnabledDrwawables_;
    bool isHardwareEnabledBufferUpdated_ = false;
    bool isHardwareForcedDisabled_ = false; // if app node has shadow or filter, disable hardware composer for all
    bool doDirectComposition_ = true;
    bool lastAnimateNeedRequestNextVsync_ = false;
    RSDirectCompositionHelper directComposeHelper_;

    // for client node tree dump
    struct NodeTreeDumpTask {
        size_t count = 0;
        size_t completionCount = 0;
        std::map<pid_t, std::optional<std::string>> data;
    };
    std::mutex nodeTreeDumpMutex_;
    std::condition_variable nodeTreeDumpCondVar_;
    std::unordered_map<uint32_t, NodeTreeDumpTask> nodeTreeDumpTasks_;

    std::unordered_map<std::string, std::shared_ptr<Media::PixelMap>> surfaceNodeWatermarks_;

    // UIFirst
    std::list<std::shared_ptr<RSSurfaceRenderNode>> subThreadNodes_;
    std::unordered_map<NodeId, bool> cacheCmdSkippedNodes_;
    std::unordered_map<pid_t, std::pair<std::vector<NodeId>, bool>> cacheCmdSkippedInfo_;
    std::set<std::shared_ptr<RSBaseRenderNode>> oldDisplayChildren_;
    // for ui first
    std::mutex mutex_;
    std::queue<std::shared_ptr<Drawing::Surface>> tmpSurfaces_;

    // for surface occlusion change callback
    std::mutex surfaceOcclusionMutex_;
    std::vector<NodeId> lastRegisteredSurfaceOnTree_;
    std::mutex systemAnimatedScenesMutex_;
    std::list<std::pair<SystemAnimatedScenes, time_t>> systemAnimatedScenesList_;
    std::list<std::pair<SystemAnimatedScenes, time_t>> threeFingerScenesList_;
    std::unordered_map<NodeId, // map<node ID, <pid, callback, partition points vector, level>>
        std::tuple<pid_t, sptr<RSISurfaceOcclusionChangeCallback>,
        std::vector<float>, uint8_t>> surfaceOcclusionListeners_;
    std::unordered_map<NodeId, // map<node ID, <surface node, app window node>>
        std::pair<std::shared_ptr<RSSurfaceRenderNode>, std::shared_ptr<RSSurfaceRenderNode>>> savedAppWindowNode_;

    // used for watermark
    std::mutex watermarkMutex_;

    // for ui captures
    std::vector<std::tuple<NodeId, std::function<void()>>> pendingUiCaptureTasks_;
    std::queue<std::tuple<NodeId, std::function<void()>>> uiCaptureTasks_;
    // uiextension
    std::mutex uiExtensionMutex_;
    UIExtensionCallbackData uiExtensionCallbackData_;
    UIExtensionCallbackData unobscureduiExtensionCallbackData_;
    // <pid, <uid, callback>>
    std::map<pid_t, std::pair<uint64_t, sptr<RSIUIExtensionCallback>>> uiExtensionListenners_ = {};
    std::map<pid_t, std::pair<uint64_t, sptr<RSIUIExtensionCallback>>> uiUnobscuredExtensionListenners_ = {};

#ifdef RS_PROFILER_ENABLED
    friend class RSProfiler;
#endif
    std::set<pid_t> exitedPidSet_;
#ifdef RS_ENABLE_GPU
    RSDrawFrame drawFrame_;
#endif
    std::unique_ptr<RSRenderThreadParams> renderThreadParams_ = nullptr; // sync to render thread
    std::unordered_set<int32_t> surfacePidNeedDropFrame_;
    RSVsyncRateReduceManager rsVsyncRateReduceManager_;

    // for record fastcompose time change
    uint64_t lastFastComposeTimeStamp_ = 0;
    uint64_t lastFastComposeTimeStampDiff_ = 0;
    std::atomic<bool> waitForDVSyncFrame_ = false;
    std::atomic<uint64_t> vsyncRsTimestamp_ = 0;
    std::string dumpInfo_;
    std::atomic<uint32_t> currentNum_ = 0;
#if defined(ACCESSIBILITY_ENABLE)
    std::shared_ptr<AccessibilityObserver> accessibilityObserver_;
#endif
#ifdef RES_SCHED_ENABLE
    sptr<VSyncSystemAbilityListener> saStatusChangeListener_ = nullptr;
#endif

    std::function<void(const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode)> consumeAndUpdateNode_;
    HgmContext hgmContext_;
};
} // namespace OHOS::Rosen
#endif // RS_MAIN_THREAD
