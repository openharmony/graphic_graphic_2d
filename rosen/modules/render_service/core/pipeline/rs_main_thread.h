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

#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <set>

#include "refbase.h"
#include "rs_base_render_engine.h"
#include "vsync_distributor.h"
#include <event_handler.h>
#include "vsync_receiver.h"

#include "command/rs_command.h"
#include "common/rs_common_def.h"
#include "common/rs_thread_handler.h"
#include "common/rs_thread_looper.h"
#include "ipc_callbacks/iapplication_agent.h"
#include "ipc_callbacks/rs_iocclusion_change_callback.h"
#include "ipc_callbacks/rs_isurface_occlusion_change_callback.h"
#include "memory/rs_app_state_listener.h"
#include "memory/rs_memory_graphic.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "platform/drawing/rs_vsync_client.h"
#include "platform/common/rs_event_manager.h"
#include "transaction/rs_transaction_data.h"

namespace OHOS::Rosen {
#if defined(ACCESSIBILITY_ENABLE)
class AccessibilityObserver;
#endif
class HgmFrameRateManager;
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
    void ProcessDataBySingleFrameComposer(std::unique_ptr<RSTransactionData>& rsTransactionData);
    void RecvRSTransactionData(std::unique_ptr<RSTransactionData>& rsTransactionData);
    void RequestNextVSync(const std::string& fromWhom = "unknown", int64_t lastVSyncTS = 0);
    void PostTask(RSTaskMessage::RSTask task);
    void PostTask(RSTaskMessage::RSTask task, const std::string& name, int64_t delayTime,
        AppExecFwk::EventQueue::Priority priority = AppExecFwk::EventQueue::Priority::IDLE);
    void RemoveTask(const std::string& name);
    void PostSyncTask(RSTaskMessage::RSTask task);
    bool IsIdle() const;
    void QosStateDump(std::string& dumpString);
    void RenderServiceTreeDump(std::string& dumpString);
    void RsEventParamDump(std::string& dumpString);
    bool IsUIFirstOn() const;
    void GetAppMemoryInMB(float& cpuMemSize, float& gpuMemSize);
    void ClearMemoryCache(ClearMemoryMoment moment, bool deeply = false);

    template<typename Task, typename Return = std::invoke_result_t<Task>>
    std::future<Return> ScheduleTask(Task&& task)
    {
        auto [scheduledTask, taskFuture] = Detail::ScheduledTask<Task>::Create(std::forward<Task&&>(task));
        PostTask([t(std::move(scheduledTask))]() { t->Run(); });
        return std::move(taskFuture);
    }

    const std::shared_ptr<RSBaseRenderEngine>& GetRenderEngine() const
    {
        return isUniRender_ ? uniRenderEngine_ : renderEngine_;
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

    bool CheckIsHardwareEnabledBufferUpdated() const
    {
        return isHardwareEnabledBufferUpdated_;
    }

    /* Judge if rootnode has to be prepared based on it corresponding process is active
     * If its pid is in activeProcessPids_ set, return true
     */
    bool CheckNodeHasToBePreparedByPid(NodeId nodeId, bool isClassifyByRoot);
    // check if active app has static drawing cache
    bool IsDrawingGroupChanged(RSRenderNode& cacheRootNode) const;
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

    void WaitUtilUniRenderFinished();
    void NotifyUniRenderFinish();

    bool WaitUntilDisplayNodeBufferReleased(RSDisplayRenderNode& node);
    void NotifyDisplayNodeBufferReleased();

    bool WaitHardwareThreadTaskExcute();
    void NotifyHardwareThreadCanExcuteTask();

    // driven render
    void NotifyDrivenRenderFinish();
    void WaitUtilDrivenRenderFinished();

    void ClearTransactionDataPidInfo(pid_t remotePid);
    void AddTransactionDataPidInfo(pid_t remotePid);

    void SetFocusAppInfo(
        int32_t pid, int32_t uid, const std::string &bundleName, const std::string &abilityName, uint64_t focusNodeId);
    std::unordered_map<NodeId, bool> GetCacheCmdSkippedNodes() const;
    std::string GetFocusAppBundleName() const;

    sptr<VSyncDistributor> rsVSyncDistributor_;
    sptr<VSyncController> rsVSyncController_;
    sptr<VSyncController> appVSyncController_;
    sptr<VSyncGenerator> vsyncGenerator_;

    void ReleaseSurface();
#ifndef USE_ROSEN_DRAWING
    void AddToReleaseQueue(sk_sp<SkSurface>&& surface);
#else
    void AddToReleaseQueue(std::shared_ptr<Drawing::Surface>&& surface);
#endif

    void SetDirtyFlag();
    void SetColorPickerForceRequestVsync(bool colorPickerForceRequestVsync);
    void SetNoNeedToPostTask(bool noNeedToPostTask);
    void SetAccessibilityConfigChanged();
    void ForceRefreshForUni();
    void TrimMem(std::unordered_set<std::u16string>& argSets, std::string& result);
    void DumpMem(std::unordered_set<std::u16string>& argSets, std::string& result, std::string& type, int pid = 0);
    void DumpNode(std::string& result, uint64_t nodeId) const;
    void CountMem(int pid, MemoryGraphic& mem);
    void CountMem(std::vector<MemoryGraphic>& mems);
    void SetAppWindowNum(uint32_t num);
    void SetMultiInstancePidVSyncRate(std::map<uint32_t, RSVisibleLevel>& pidVisMap,
        std::vector<RSBaseRenderNode::SharedPtr>& curAllSurfaces);
    bool SetSystemAnimatedScenes(SystemAnimatedScenes systemAnimatedScenes);
    SystemAnimatedScenes GetSystemAnimatedScenes();
    void ShowWatermark(const std::shared_ptr<Media::PixelMap> &watermarkImg, bool isShow);
    void SetIsCachedSurfaceUpdated(bool isCachedSurfaceUpdated);
    pid_t GetDesktopPidForRotationScene() const;
    void SetForceUpdateUniRenderFlag(bool flag)
    {
        forceUpdateUniRenderFlag_ = flag;
    }
    void SetIdleTimerExpiredFlag(bool flag)
    {
        idleTimerExpiredFlag_ = flag;
    }
#ifndef USE_ROSEN_DRAWING
    sk_sp<SkImage> GetWatermarkImg();
#else
    std::shared_ptr<Drawing::Image> GetWatermarkImg();
#endif
    bool GetWatermarkFlag();
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
    bool GetNoNeedToPostTask();
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

    bool IsMainLooping() const
    {
        return mainLooping_.load();
    }

    bool GetDiscardJankFrames() const
    {
        return discardJankFrames_.load();
    }

    void SetDiscardJankFrames(bool discardJankFrames)
    {
        discardJankFrames_.store(discardJankFrames);
    }

    bool IsPCThreeFingerScenesListScene() const
    {
        return !threeFingerScenesList_.empty();
    }

    void SubscribeAppState();
    void HandleOnTrim(Memory::SystemMemoryLevel level);
    void NotifySurfaceCapProcFinish();
    void WaitUntilSurfaceCapProcFinished();
    void SetSurfaceCapProcFinished(bool flag);

    const std::vector<std::shared_ptr<RSSurfaceRenderNode>>& GetSelfDrawingNodes() const;
    bool GetParallelCompositionEnabled();
    std::shared_ptr<HgmFrameRateManager> GetFrameRateMgr() { return frameRateMgr_; };
private:
    using TransactionDataIndexMap = std::unordered_map<pid_t,
        std::pair<uint64_t, std::vector<std::unique_ptr<RSTransactionData>>>>;

    RSMainThread();
    ~RSMainThread() noexcept;
    RSMainThread(const RSMainThread&) = delete;
    RSMainThread(const RSMainThread&&) = delete;
    RSMainThread& operator=(const RSMainThread&) = delete;
    RSMainThread& operator=(const RSMainThread&&) = delete;

    void OnVsync(uint64_t timestamp, void* data);
    void ProcessCommand();
    void Animate(uint64_t timestamp);
    void ApplyModifiers();
    void ConsumeAndUpdateAllNodes();
    void CollectInfoForHardwareComposer();
    void CollectInfoForDrivenRender();
    void ReleaseAllNodesBuffer();
    void Render();
    void SetDeviceType();
    void ColorPickerRequestVsyncIfNeed();
    void UniRender(std::shared_ptr<RSBaseRenderNode> rootNode);
    bool CheckSurfaceNeedProcess(OcclusionRectISet& occlusionSurfaces, std::shared_ptr<RSSurfaceRenderNode> curSurface);
    void CalcOcclusionImplementation(std::vector<RSBaseRenderNode::SharedPtr>& curAllSurfaces,
        VisibleData& dstCurVisVec, std::map<uint32_t, RSVisibleLevel>& dstPidVisMap);
    void CalcOcclusion();
    bool CheckSurfaceVisChanged(std::map<uint32_t, RSVisibleLevel>& pidVisMap,
        std::vector<RSBaseRenderNode::SharedPtr>& curAllSurfaces);
    void SetVSyncRateByVisibleLevel(std::map<uint32_t, RSVisibleLevel>& pidVisMap,
        std::vector<RSBaseRenderNode::SharedPtr>& curAllSurfaces);
    void CallbackToWMS(VisibleData& curVisVec);
    void SendCommands();
    void SurfaceOcclusionCallback();
    void InitRSEventDetector();
    void RemoveRSEventDetector();
    void SetRSEventDetectorLoopStartTag();
    void SetRSEventDetectorLoopFinishTag();
    void CallbackDrawContextStatusToWMS();
    void CheckSystemSceneStatus();
    void UpdateUIFirstSwitch();
    // ROG: Resolution Online Government
    void UpdateRogSizeIfNeeded();
    uint32_t GetRefreshRate() const;
    void SkipCommandByNodeId(std::vector<std::unique_ptr<RSTransactionData>>& transactionVec, pid_t pid);

    bool DoParallelComposition(std::shared_ptr<RSBaseRenderNode> rootNode);

    void ClassifyRSTransactionData(std::unique_ptr<RSTransactionData>& rsTransactionData);
    void ProcessRSTransactionData(std::unique_ptr<RSTransactionData>& rsTransactionData, pid_t pid);
    void ProcessSyncRSTransactionData(std::unique_ptr<RSTransactionData>& rsTransactionData, pid_t pid);
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
    void ResetHardwareEnabledState();
    void CheckIfHardwareForcedDisabled();
    void CheckAndUpdateTransactionIndex(
        std::shared_ptr<TransactionDataMap>& transactionDataEffective, std::string& transactionFlags);

    bool IsResidentProcess(pid_t pid) const;
    bool IsNeedSkip(NodeId instanceRootNodeId, pid_t pid);

    // UIFirst
    bool CheckParallelSubThreadNodesStatus();
    void CacheCommands();
    bool CheckSubThreadNodeStatusIsDoing(NodeId appNodeId) const;

    // used for informing hgm the bundle name of SurfaceRenderNodes
    void InformHgmNodeInfo();
    void CheckIfNodeIsBundle(std::shared_ptr<RSSurfaceRenderNode> node);

    void SetFocusLeashWindowId();
    void ProcessHgmFrameRate(uint64_t timestamp);
    bool IsLastFrameUIFirstEnabled(NodeId appNodeId) const;
    RSVisibleLevel GetRegionVisibleLevel(const Occlusion::Region& curRegion,
        const Occlusion::Region& visibleRegion);
    void PrintCurrentStatus();
    void WaitUntilUploadTextureTaskFinishedForGL();

    std::shared_ptr<AppExecFwk::EventRunner> runner_ = nullptr;
    std::shared_ptr<AppExecFwk::EventHandler> handler_ = nullptr;
    RSTaskMessage::RSTask mainLoop_;
    std::unique_ptr<RSVsyncClient> vsyncClient_ = nullptr;
    std::unordered_map<NodeId, uint64_t> bufferTimestamps_;

    std::mutex transitionDataMutex_;
    std::unordered_map<NodeId, std::map<uint64_t, std::vector<std::unique_ptr<RSCommand>>>> cachedCommands_;
    std::map<uint64_t, std::vector<std::unique_ptr<RSCommand>>> effectiveCommands_;
    std::map<uint64_t, std::vector<std::unique_ptr<RSCommand>>> pendingEffectiveCommands_;
    std::unordered_map<pid_t, std::vector<std::unique_ptr<RSTransactionData>>> syncTransactionData_;
    int32_t syncTransactionCount_ { 0 };

    TransactionDataMap cachedTransactionDataMap_;
    TransactionDataIndexMap effectiveTransactionDataIndexMap_;
    std::map<pid_t, std::vector<std::unique_ptr<RSTransactionData>>> cachedSkipTransactionDataMap_;
    std::unordered_map<pid_t, uint64_t> transactionDataLastWaitTime_;

    uint64_t curTime_ = 0;
    uint64_t timestamp_ = 0;
    uint64_t lastAnimateTimestamp_ = 0;
    uint64_t prePerfTimestamp_ = 0;
    uint64_t lastCleanCacheTimestamp_ = 0;
    int hardwareTid_ = -1;
    std::unordered_map<uint32_t, sptr<IApplicationAgent>> applicationAgentMap_;

    std::shared_ptr<RSContext> context_;
    std::thread::id mainThreadId_;
    std::shared_ptr<VSyncReceiver> receiver_ = nullptr;
    std::map<pid_t, sptr<RSIOcclusionChangeCallback>> occlusionListeners_;
    std::mutex occlusionMutex_;

    bool isUniRender_ = RSUniRenderJudgement::IsUniRender();
    RSTaskMessage::RSTask unmarshalBarrierTask_;
    std::condition_variable unmarshalTaskCond_;
    std::mutex unmarshalMutex_;
    int32_t unmarshalFinishedCount_ = 0;
    sptr<VSyncDistributor> appVSyncDistributor_ = nullptr;

    std::condition_variable surfaceCapProcTaskCond_;
    std::mutex surfaceCapProcMutex_;
    bool surfaceCapProcFinished_ = true;

#if defined(RS_ENABLE_PARALLEL_UPLOAD) && defined(RS_ENABLE_GL)
    RSTaskMessage::RSTask uploadTextureBarrierTask_;
    std::condition_variable uploadTextureTaskCond_;
    std::mutex uploadTextureMutex_;
    int32_t uploadTextureFinishedCount_ = 0;
    EGLSyncKHR uploadTextureFence;
#endif

    mutable std::mutex uniRenderMutex_;
    bool uniRenderFinished_ = false;
    std::condition_variable uniRenderCond_;
    // used for blocking mainThread before displayNode has no freed buffer to request
    mutable std::mutex displayNodeBufferReleasedMutex_;
    bool displayNodeBufferReleased_ = false;
    // used for stalling mainThread before displayNode has no freed buffer to request
    std::condition_variable displayNodeBufferReleasedCond_;

    bool clearMemoryFinished_ = true;
    bool clearMemDeeply_ = false;

    // driven render
    mutable std::mutex drivenRenderMutex_;
    bool drivenRenderFinished_ = false;
    std::condition_variable drivenRenderCond_;

    // Used to refresh the whole display when AccessibilityConfig is changed
    bool isAccessibilityConfigChanged_ = false;

    // used for blocking mainThread when hardwareThread has 2 and more task to excute
    mutable std::mutex hardwareThreadTaskMutex_;
    std::condition_variable hardwareThreadTaskCond_;

    std::map<uint32_t, RSVisibleLevel> lastPidVisMap_;
    VisibleData lastVisVec_;
    std::map<NodeId, uint64_t> lastDrawStatusMap_;
    std::vector<NodeId> curDrawStatusVec_;
    bool qosPidCal_ = false;
    bool isDirty_ = false;
    std::atomic_bool doWindowAnimate_ = false;
    std::vector<NodeId> lastSurfaceIds_;
    int32_t focusAppPid_ = -1;
    int32_t focusAppUid_ = -1;
    const uint8_t opacity_ = 255;
    std::string focusAppBundleName_ = "";
    std::string focusAppAbilityName_ = "";
    uint64_t focusNodeId_ = 0;
    uint64_t focusLeashWindowId_ = 0;
    uint64_t lastFocusNodeId_ = 0;
    uint32_t appWindowNum_ = 0;
    uint32_t requestNextVsyncNum_ = 0;
    bool lastFrameHasFilter_ = false;
    bool vsyncControlEnabled_ = true;
    bool systemAnimatedScenesEnabled_ = false;
    bool isFoldScreenDevice_ = false;

    bool colorPickerForceRequestVsync_ = false;
    std::atomic_bool noNeedToPostTask_ = false;
    std::atomic_int colorPickerRequestFrameNum_ = 15;

    std::shared_ptr<RSBaseRenderEngine> renderEngine_;
    std::shared_ptr<RSBaseRenderEngine> uniRenderEngine_;
    std::shared_ptr<RSBaseEventDetector> rsCompositionTimeoutDetector_;
    RSEventManager rsEventManager_;
#if defined(ACCESSIBILITY_ENABLE)
    std::shared_ptr<AccessibilityObserver> accessibilityObserver_;
#endif

    // used for hardware enabled case
    bool doDirectComposition_ = true;
    bool isHardwareEnabledBufferUpdated_ = false;
    std::vector<std::shared_ptr<RSSurfaceRenderNode>> hardwareEnabledNodes_;
    std::vector<std::shared_ptr<RSSurfaceRenderNode>> selfDrawingNodes_;
    bool isHardwareForcedDisabled_ = false; // if app node has shadow or filter, disable hardware composer for all

    // used for watermark
    std::mutex watermarkMutex_;
#ifndef USE_ROSEN_DRAWING
    sk_sp<SkImage> watermarkImg_ = nullptr;
#else
    std::shared_ptr<Drawing::Image> watermarkImg_ = nullptr;
#endif
    bool isShow_ = false;
    bool doParallelComposition_ = false;

    // driven render
    bool hasDrivenNodeOnUniTree_ = false;
    bool hasDrivenNodeMarkRender_ = false;

    std::shared_ptr<HgmFrameRateManager> frameRateMgr_ = nullptr;
    std::shared_ptr<RSRenderFrameRateLinker> rsFrameRateLinker_ = nullptr;
    pid_t desktopPidForRotationScene_ = 0;
    FrameRateRange rsCurrRange_;

    // UIFirst
    std::list<std::shared_ptr<RSSurfaceRenderNode>> subThreadNodes_;
    std::unordered_map<NodeId, bool> cacheCmdSkippedNodes_;
    std::unordered_map<pid_t, std::pair<std::vector<NodeId>, bool>> cacheCmdSkippedInfo_;
    std::atomic<uint64_t> frameCount_ = 0;
    std::set<std::shared_ptr<RSBaseRenderNode>> oldDisplayChildren_;
    DeviceType deviceType_ = DeviceType::PHONE;
    bool isCachedSurfaceUpdated_ = false;
    bool isUiFirstOn_ = false;

    // used for informing hgm the bundle name of SurfaceRenderNodes
    bool noBundle_ = false;
    std::string currentBundleName_ = "";
    bool forceUpdateUniRenderFlag_ = false;
    bool idleTimerExpiredFlag_ = false;
    // for ui first
    std::mutex mutex_;
#ifndef USE_ROSEN_DRAWING
    std::queue<sk_sp<SkSurface>> tmpSurfaces_;
#else
    std::queue<std::shared_ptr<Drawing::Surface>> tmpSurfaces_;
#endif

    // for surface occlusion change callback
    std::mutex surfaceOcclusionMutex_;
    std::vector<NodeId> lastRegisteredSurfaceOnTree_;
    std::mutex systemAnimatedScenesMutex_;
    std::list<std::pair<SystemAnimatedScenes, time_t>> systemAnimatedScenesList_;
    std::list<std::pair<SystemAnimatedScenes, time_t>> threeFingerScenesList_;
    bool isReduceVSyncBySystemAnimatedScenes_ = false;
    std::unordered_map<NodeId, // map<node ID, <pid, callback, partition points vector, level>>
        std::tuple<pid_t, sptr<RSISurfaceOcclusionChangeCallback>,
        std::vector<float>, uint8_t>> surfaceOcclusionListeners_;
    std::unordered_map<NodeId, // map<node ID, <surface node, app window node>>
        std::pair<std::shared_ptr<RSSurfaceRenderNode>, std::shared_ptr<RSSurfaceRenderNode>>> savedAppWindowNode_;

    std::shared_ptr<RSAppStateListener> rsAppStateListener_;
    int32_t subscribeFailCount_ = 0;
    SystemAnimatedScenes systemAnimatedScenes_ = SystemAnimatedScenes::OTHERS;
    uint32_t leashWindowCount_ = 0;

    // for dvsync (animate requestNextVSync after mark rsnotrendering)
    bool needRequestNextVsyncAnimate_ = false;

    std::atomic_bool mainLooping_ = false;
    std::atomic_bool discardJankFrames_ = false;
    bool forceUIFirstChanged_ = false;
};
} // namespace OHOS::Rosen
#endif // RS_MAIN_THREAD
