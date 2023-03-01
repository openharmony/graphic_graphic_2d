/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#include "ipc_callbacks/rs_irender_mode_change_callback.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "platform/drawing/rs_vsync_client.h"
#include "platform/common/rs_event_manager.h"
#include "transaction/rs_transaction_data.h"

namespace OHOS::Rosen {
class AccessibilityObserver;
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
        return IfUseUniVisitor() ? uniRenderEngine_ : renderEngine_;
    }

    RSContext& GetContext()
    {
        return *context_;
    }
    std::thread::id Id() const
    {
        return mainThreadId_;
    }
    void RegisterApplicationAgent(uint32_t pid, sptr<IApplicationAgent> app);
    void UnRegisterApplicationAgent(sptr<IApplicationAgent> app);
    void NotifyRenderModeChanged(bool useUniVisitor);
    bool QueryIfUseUniVisitor() const;

    void RegisterOcclusionChangeCallback(sptr<RSIOcclusionChangeCallback> callback);
    void UnRegisterOcclusionChangeCallback(sptr<RSIOcclusionChangeCallback> callback);
    void CleanOcclusionListener();

    void WaitUtilUniRenderFinished();
    void NotifyUniRenderFinish();
    void SetRenderModeChangeCallback(sptr<RSIRenderModeChangeCallback> callback);
    bool IfUseUniVisitor() const;

    void ClearTransactionDataPidInfo(pid_t remotePid);
    void AddTransactionDataPidInfo(pid_t remotePid);

    void SetFocusAppInfo(
        int32_t pid, int32_t uid, const std::string &bundleName, const std::string &abilityName);

    sptr<VSyncDistributor> rsVSyncDistributor_;

    void SetDirtyFlag();
    void ForceRefreshForUni();
    void SetAppWindowNum(uint32_t num);
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
    void ReleaseAllNodesBuffer();
    void Render();
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
    void ProcessCommandForDividedRender();
    void ProcessCommandForUniRender();
    void WaitUntilUnmarshallingTaskFinished();
    void MergeToEffectiveTransactionDataMap(TransactionDataMap& cachedTransactionDataMap);

    void CheckBufferAvailableIfNeed();
    void CheckUpdateSurfaceNodeIfNeed();

    void CheckDelayedSwitchTask();
    void UpdateRenderMode(bool useUniVisitor);

    void CheckColdStartMap();
    void ClearDisplayBuffer();
    void PerfAfterAnim();
    void PerfForBlurIfNeeded();
    void PerfMultiWindow();

    std::shared_ptr<AppExecFwk::EventRunner> runner_ = nullptr;
    std::shared_ptr<AppExecFwk::EventHandler> handler_ = nullptr;
    RSTaskMessage::RSTask mainLoop_;
    std::unique_ptr<RSVsyncClient> vsyncClient_ = nullptr;
    std::unordered_map<NodeId, uint64_t> bufferTimestamps_;

    std::mutex transitionDataMutex_;
    std::unordered_map<NodeId, std::map<uint64_t, std::vector<std::unique_ptr<RSCommand>>>> cachedCommands_;
    std::map<uint64_t, std::vector<std::unique_ptr<RSCommand>>> effectiveCommands_;
    std::map<uint64_t, std::vector<std::unique_ptr<RSCommand>>> pendingEffectiveCommands_;
    std::map<uint64_t, std::vector<std::unique_ptr<RSCommand>>> followVisitorCommands_;

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

    bool waitingBufferAvailable_ = false; // uni->non-uni mode, wait for RT buffer, only used in main thread
    bool waitingUpdateSurfaceNode_ = false; // non-uni->uni mode, wait for update surfaceView, only used in main thread
    bool isUniRender_ = RSUniRenderJudgement::IsUniRender();
    sptr<RSIRenderModeChangeCallback> renderModeChangeCallback_;
    std::atomic_bool useUniVisitor_ = isUniRender_;
    std::atomic_bool delayedTargetUniVisitor_ = isUniRender_;
    std::atomic_bool switchDelayed_ = false;
    RSTaskMessage::RSTask unmarshalBarrierTask_;
    std::condition_variable unmarshalTaskCond_;
    std::mutex unmarshalMutex_;
    int32_t unmarshalFinishedCount_ = 0;

    mutable std::mutex uniRenderMutex_;
    bool uniRenderFinished_ = false;
    std::condition_variable uniRenderCond_;
    std::map<uint32_t, bool> lastPidVisMap_;
    VisibleData lastVisVec_;
    bool qosPidCal_ = false;
    bool isDirty_ = false;
    std::atomic_bool doWindowAnimate_ = false;
    uint32_t lastSurfaceCnt_ = 0;
    int32_t focusAppPid_ = -1;
    int32_t focusAppUid_ = -1;
    std::string focusAppBundleName_ = "";
    std::string focusAppAbilityName_ = "";
    uint32_t appWindowNum_ = 0;
    uint32_t requestNextVsyncNum_ = 0;

    std::shared_ptr<RSBaseRenderEngine> renderEngine_;
    std::shared_ptr<RSBaseRenderEngine> uniRenderEngine_;
    std::shared_ptr<RSBaseEventDetector> rsCompositionTimeoutDetector_;
    RSEventManager rsEventManager_;
    std::shared_ptr<AccessibilityObserver> accessibilityObserver_;
};
} // namespace OHOS::Rosen
#endif // RS_MAIN_THREAD
