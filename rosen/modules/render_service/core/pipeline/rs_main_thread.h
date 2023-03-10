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

#include "refbase.h"
#include "rs_base_render_engine.h"
#include "vsync_distributor.h"
#include <event_handler.h>
#include "vsync_receiver.h"

#include "command/rs_command.h"
#include "common/rs_thread_handler.h"
#include "common/rs_thread_looper.h"
#include "ipc_callbacks/iapplication_agent.h"
#include "ipc_callbacks/rs_iocclusion_change_callback.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "platform/drawing/rs_vsync_client.h"
#include "platform/common/rs_event_manager.h"
#include "transaction/rs_transaction_data.h"

namespace OHOS::Rosen {
#if defined(ACCESSIBILITY_ENABLE)
class AccessibilityObserver;
#endif
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
    void RecvRSTransactionData(std::unique_ptr<RSTransactionData>& rsTransactionData);
    void RequestNextVSync();
    void PostTask(RSTaskMessage::RSTask task);
    void PostSyncTask(RSTaskMessage::RSTask task);
    void QosStateDump(std::string& dumpString);
    void RenderServiceTreeDump(std::string& dumpString);
    void RsEventParamDump(std::string& dumpString);

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

    RSContext& GetContext()
    {
        return *context_;
    }

    std::thread::id Id() const
    {
        return mainThreadId_;
    }

    /* Judge if node has to be prepared based on it corresponding process is active
     * If its pid is in activeProcessPids_ set, return true
     */
    inline bool CheckNodeHasToBePreparedByPid(NodeId nodeId) const
    {
        if (activeProcessPids_.empty()) {
            return false;
        }
        pid_t pid = ExtractPid(nodeId);
        return (activeProcessPids_.find(pid) != activeProcessPids_.end());
    }

    void RegisterApplicationAgent(uint32_t pid, sptr<IApplicationAgent> app);
    void UnRegisterApplicationAgent(sptr<IApplicationAgent> app);

    void RegisterOcclusionChangeCallback(sptr<RSIOcclusionChangeCallback> callback);
    void UnRegisterOcclusionChangeCallback(sptr<RSIOcclusionChangeCallback> callback);
    void CleanOcclusionListener();

    void WaitUtilUniRenderFinished();
    void NotifyUniRenderFinish();

    void WaitUntilDisplayNodeBufferReleased(RSDisplayRenderNode& node);
    void NotifyDisplayNodeBufferReleased();

    // driven render
    void NotifyDrivenRenderFinish();
    void WaitUtilDrivenRenderFinished();

    void ClearTransactionDataPidInfo(pid_t remotePid);
    void AddTransactionDataPidInfo(pid_t remotePid);

    void SetFocusAppInfo(
        int32_t pid, int32_t uid, const std::string &bundleName, const std::string &abilityName);

    sptr<VSyncDistributor> rsVSyncDistributor_;

    void SetDirtyFlag();
    void ForceRefreshForUni();
    void TrimMem(std::unordered_set<std::u16string>& argSets, std::string& result);
    void DumpMem(std::unordered_set<std::u16string>& argSets, std::string& result);
    void SetAppWindowNum(uint32_t num);
    void ShowWatermark(const std::shared_ptr<Media::PixelMap> &watermarkImg, bool isShow);
    sk_sp<SkImage> GetWatermarkImg();
    bool GetWatermarkFlag();
    void AddActivePid(pid_t pid);
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
    void ConsumeAndUpdateAllNodes();
    void CollectInfoForHardwareComposer();
    void CollectInfoForDrivenRender();
    void ReleaseAllNodesBuffer();
    void Render();
    bool CheckSurfaceNeedProcess(OcclusionRectISet& occlusionSurfaces, std::shared_ptr<RSSurfaceRenderNode> curSurface);
    void CalcOcclusionImplementation(std::vector<RSBaseRenderNode::SharedPtr>& curAllSurfaces);
    void CalcOcclusion();
    bool CheckQosVisChanged(std::map<uint32_t, bool>& pidVisMap);
    void CallbackToQOS(std::map<uint32_t, bool>& pidVisMap);
    void CallbackToWMS(VisibleData& curVisVec);
    void SendCommands();
    void InitRSEventDetector();
    void RemoveRSEventDetector();
    void SetRSEventDetectorLoopStartTag();
    void SetRSEventDetectorLoopFinishTag();

    bool DoParallelComposition(std::shared_ptr<RSBaseRenderNode> rootNode);
    void ResetSortedChildren(std::shared_ptr<RSBaseRenderNode> node);

    void ClassifyRSTransactionData(std::unique_ptr<RSTransactionData>& rsTransactionData);
    void ProcessRSTransactionData(std::unique_ptr<RSTransactionData>& rsTransactionData, pid_t pid);
    void ProcessSyncRSTransactionData(std::unique_ptr<RSTransactionData>& rsTransactionData, pid_t pid);
    void ProcessAllSyncTransactionDatas();
    void ProcessCommandForDividedRender();
    void ProcessCommandForUniRender();
    void WaitUntilUnmarshallingTaskFinished();
    void MergeToEffectiveTransactionDataMap(TransactionDataMap& cachedTransactionDataMap);

    void CheckColdStartMap();
    void ClearDisplayBuffer();
    void PerfAfterAnim();
    void PerfForBlurIfNeeded();
    void PerfMultiWindow();
    void ResetHardwareEnabledState();

    std::shared_ptr<AppExecFwk::EventRunner> runner_ = nullptr;
    std::shared_ptr<AppExecFwk::EventHandler> handler_ = nullptr;
    RSTaskMessage::RSTask mainLoop_;
    std::unique_ptr<RSVsyncClient> vsyncClient_ = nullptr;
    std::unordered_map<NodeId, uint64_t> bufferTimestamps_;

    std::mutex transitionDataMutex_;
    std::unordered_map<NodeId, std::map<uint64_t, std::vector<std::unique_ptr<RSCommand>>>> cachedCommands_;
    std::map<uint64_t, std::vector<std::unique_ptr<RSCommand>>> effectiveCommands_;
    std::map<uint64_t, std::vector<std::unique_ptr<RSCommand>>> pendingEffectiveCommands_;
    // Collect pids of surfaceview's update(ConsumeAndUpdateAllNodes), effective commands(processCommand) and Animate
    std::unordered_set<pid_t> activeProcessPids_;
    std::unordered_map<pid_t, std::vector<std::unique_ptr<RSTransactionData>>> syncTransactionDatas_;
    int32_t syncTransactionCount_ { 0 };

    TransactionDataMap cachedTransactionDataMap_;
    TransactionDataIndexMap effectiveTransactionDataIndexMap_;
    std::unordered_map<pid_t, uint64_t> transactionDataLastWaitTime_;

    uint64_t timestamp_ = 0;
    uint64_t lastAnimateTimestamp_ = 0;
    uint64_t prePerfTimestamp_ = 0;
    uint64_t lastCleanCacheTimestamp_ = 0;
    std::unordered_map<uint32_t, sptr<IApplicationAgent>> applicationAgentMap_;

    std::shared_ptr<RSContext> context_;
    std::thread::id mainThreadId_;
    std::shared_ptr<VSyncReceiver> receiver_ = nullptr;
    std::vector<sptr<RSIOcclusionChangeCallback>> occlusionListeners_;

    bool isUniRender_ = RSUniRenderJudgement::IsUniRender();
    RSTaskMessage::RSTask unmarshalBarrierTask_;
    std::condition_variable unmarshalTaskCond_;
    std::mutex unmarshalMutex_;
    int32_t unmarshalFinishedCount_ = 0;

    mutable std::mutex uniRenderMutex_;
    bool uniRenderFinished_ = false;
    std::condition_variable uniRenderCond_;
    // used for blocking mainThread before displayNode has no freed buffer to request
    mutable std::mutex displayNodeBufferReleasedMutex_;
    bool displayNodeBufferReleased_ = false;
    // used for stalling mainThread before displayNode has no freed buffer to request
    std::condition_variable displayNodeBufferReleasedCond_;

    // driven render
    mutable std::mutex drivenRenderMutex_;
    bool drivenRenderFinished_ = false;
    std::condition_variable drivenRenderCond_;

    std::map<uint32_t, bool> lastPidVisMap_;
    VisibleData lastVisVec_;
    bool qosPidCal_ = false;
    bool isDirty_ = false;
    std::atomic_bool doWindowAnimate_ = false;
    uint32_t lastSurfaceCnt_ = 0;
    int32_t focusAppPid_ = -1;
    int32_t focusAppUid_ = -1;
    int32_t lastFocusAppPid_ = -1;
    const uint8_t opacity_ = 255;
    std::string focusAppBundleName_ = "";
    std::string focusAppAbilityName_ = "";
    uint32_t appWindowNum_ = 0;
    uint32_t requestNextVsyncNum_ = 0;

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
    bool isHardwareForcedDisabled_ = false; // if app node has shadow or filter, disable hardware composer for all

    // used for watermark
    std::mutex watermarkMutex_;
    sk_sp<SkImage> watermarkImg_ = nullptr;
    bool isShow_ = false;

    // driven render
    bool hasDrivenNodeOnUniTree_ = false;
    bool hasDrivenNodeMarkRender_ = false;
};
} // namespace OHOS::Rosen
#endif // RS_MAIN_THREAD
