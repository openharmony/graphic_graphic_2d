/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef HGM_FRAME_RATE_MANAGER_H
#define HGM_FRAME_RATE_MANAGER_H

#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "animation/rs_frame_rate_range.h"
#include "common/rs_common_def.h"
#include "hgm_command.h"
#include "hgm_idle_detector.h"
#include "hgm_multi_app_strategy.h"
#include "hgm_one_shot_timer.h"
#include "hgm_screen.h"
#include "hgm_task_handle_thread.h"
#include "hgm_touch_manager.h"
#include "hgm_vsync_generator_controller.h"
#include "modifier/rs_modifier_type.h"
#include "pipeline/rs_render_frame_rate_linker.h"
#include "pipeline/rs_render_node.h"
#include "screen_manager/screen_types.h"
#include "variable_frame_rate/rs_variable_frame_rate.h"

namespace OHOS {
namespace Rosen {
using VoteRange = std::pair<uint32_t, uint32_t>;
using FrameRateLinkerMap = std::unordered_map<FrameRateLinkerId, std::shared_ptr<RSRenderFrameRateLinker>>;

enum VoteType : bool {
    REMOVE_VOTE = false,
    ADD_VOTE = true
};

enum TouchStatus : uint32_t {
    TOUCH_CANCEL = 1,
    TOUCH_DOWN = 2,
    TOUCH_MOVE = 3,
    TOUCH_UP = 4,
    TOUCH_PULL_DOWN = 12,
    TOUCH_PULL_UP = 14,
};

enum CleanPidCallbackType : uint32_t {
    LIGHT_FACTOR,
    PACKAGE_EVENT,
    TOUCH_EVENT,
    GAMES,
};

struct DvsyncInfo {
    bool isRsDvsyncOn = false;
    bool isUiDvsyncOn = false;
};

struct VoteInfo {
    std::string voterName = "";
    uint32_t min = OLED_NULL_HZ;
    uint32_t max = OLED_NULL_HZ;
    pid_t pid = DEFAULT_PID;
    std::string extInfo = "";
    std::string bundleName = "";

    void Merge(const VoteInfo& other)
    {
        this->voterName = other.voterName;
        this->pid = other.pid;
        this->extInfo = other.extInfo;
    }

    void SetRange(uint32_t min, uint32_t max)
    {
        this->min = min;
        this->max = max;
    }

    std::string ToString(uint64_t timestamp) const
    {
        std::stringstream str;
        str << "VOTER_NAME:" << voterName << ";";
        str << "PREFERRED:" << max << ";";
        str << "EXT_INFO:" << extInfo << ";";
        str << "PID:" << pid << ";";
        str << "BUNDLE_NAME:" << bundleName << ";";
        str << "TIMESTAMP:" << timestamp << ".";
        return str.str();
    }

    bool operator==(const VoteInfo& other) const
    {
        return this->min == other.min && this->max == other.max && this->voterName == other.voterName &&
            this->extInfo == other.extInfo && this->pid == other.pid && this->bundleName == other.bundleName;
    }

    bool operator!=(const VoteInfo& other) const
    {
        return !(*this == other);
    }
};

class HgmFrameRateManager {
public:
    HgmFrameRateManager();
    ~HgmFrameRateManager() = default;

    void HandleLightFactorStatus(pid_t pid, bool isSafe);
    void HandlePackageEvent(pid_t pid, const std::vector<std::string>& packageList);
    void HandleRefreshRateEvent(pid_t pid, const EventInfo& eventInfo);
    void HandleTouchEvent(pid_t pid, int32_t touchStatus, int32_t touchCnt);
    void HandleDynamicModeEvent(bool enableDynamicModeEvent);

    void CleanVote(pid_t pid);
    int32_t GetCurRefreshRateMode() const { return curRefreshRateMode_; };
    ScreenId GetCurScreenId() const { return curScreenId_; };
    std::string GetCurScreenStrategyId() const { return curScreenStrategyId_; };
    void HandleRefreshRateMode(int32_t refreshRateMode);
    void HandleScreenPowerStatus(ScreenId id, ScreenPowerStatus status);
    void HandleRsFrame();
    void SetShowRefreshRateEnabled(bool enable);
    bool IsLtpo() const { return isLtpo_; };
    bool IsAdaptive() const { return isAdaptive_.load(); };
    void UniProcessDataForLtpo(uint64_t timestamp, std::shared_ptr<RSRenderFrameRateLinker> rsFrameRateLinker,
        const FrameRateLinkerMap& appFrameRateLinkers, bool idleTimerExpired, const DvsyncInfo& dvsyncInfo);

    int32_t GetExpectedFrameRate(const RSPropertyUnit unit, float velocity) const;
    void SetForceUpdateCallback(std::function<void(bool, bool)> forceUpdateCallback)
    {
        forceUpdateCallback_ = forceUpdateCallback;
    }

    void Init(sptr<VSyncController> rsController,
        sptr<VSyncController> appController, sptr<VSyncGenerator> vsyncGenerator);
    void InitTouchManager();
    // called by RSMainTHread
    void ProcessPendingRefreshRate(uint64_t timestamp, int64_t vsyncId, uint32_t rsRate, const DvsyncInfo& dvsyncInfo);
    HgmMultiAppStrategy& GetMultiAppStrategy() { return multiAppStrategy_; }
    HgmTouchManager& GetTouchManager() { return touchManager_; }
    HgmIdleDetector& GetIdleDetector() { return idleDetector_; }
    void UpdateSurfaceTime(const std::string& surfaceName, uint64_t timestamp,
        pid_t pid, UIFWKType uiFwkType);
    void SetSchedulerPreferredFps(uint32_t schedulePreferredFps)
    {
        if (schedulePreferredFps_ != schedulePreferredFps) {
            schedulePreferredFps_ = schedulePreferredFps;
            schedulePreferredFpsChange_ = true;
        }
    }

    void SetIsNeedUpdateAppOffset(bool isNeedUpdateAppOffset)
    {
        isNeedUpdateAppOffset_ = isNeedUpdateAppOffset;
    }

    static std::pair<bool, bool> MergeRangeByPriority(VoteRange& rangeRes, const VoteRange& curVoteRange);
    std::unordered_map<std::string, pid_t> GetUiFrameworkDirtyNodes(
        std::vector<std::weak_ptr<RSRenderNode>>& uiFwkDirtyNodes);
private:
    void Reset();
    void UpdateAppSupportedState();
    void UpdateGuaranteedPlanVote(uint64_t timestamp);

    void ProcessLtpoVote(const FrameRateRange& finalRange, bool idleTimerExpired);
    void SetAceAnimatorVote(const std::shared_ptr<RSRenderFrameRateLinker>& linker, bool& needCheckAceAnimatorStatus);
    bool CollectFrameRateChange(FrameRateRange finalRange, std::shared_ptr<RSRenderFrameRateLinker> rsFrameRateLinker,
        const FrameRateLinkerMap& appFrameRateLinkers);
    void HandleFrameRateChangeForLTPO(uint64_t timestamp, bool followRs);
    void FrameRateReport();
    uint32_t CalcRefreshRate(const ScreenId id, const FrameRateRange& range);
    uint32_t GetDrawingFrameRate(const uint32_t refreshRate, const FrameRateRange& range);
    int32_t GetPreferredFps(const std::string& type, float velocity) const;
    static float PixelToMM(float velocity);

    void HandleIdleEvent(bool isIdle);
    void HandleSceneEvent(pid_t pid, EventInfo eventInfo);
    void HandleVirtualDisplayEvent(pid_t pid, EventInfo eventInfo);
    void HandleGamesEvent(pid_t pid, EventInfo eventInfo);

    void DeliverRefreshRateVote(const VoteInfo& voteInfo, bool eventStatus);
    void MarkVoteChange(const std::string& voter = "");
    bool IsCurrentScreenSupportAS();
    void ProcessAdaptiveSync(std::string voterName);
    // merge [VOTER_LTPO, VOTER_IDLE)
    bool MergeLtpo2IdleVote(
        std::vector<std::string>::iterator& voterIter, VoteInfo& resultVoteInfo, VoteRange& mergedVoteRange);
    bool ProcessRefreshRateVote(
        std::vector<std::string>::iterator& voterIter, VoteInfo& resultVoteInfo, VoteRange& voteRange);
    VoteInfo ProcessRefreshRateVote();
    void UpdateVoteRule();
    void ReportHiSysEvent(const VoteInfo& frameRateVoteInfo);
    void SetResultVoteInfo(VoteInfo& voteInfo, uint32_t min, uint32_t max);
    void UpdateEnergyConsumptionConfig();
    static void EnterEnergyConsumptionAssuranceMode();
    static void ExitEnergyConsumptionAssuranceMode();
    static void ProcessVoteLog(const VoteInfo& curVoteInfo, bool isSkip);
    void RegisterCoreCallbacksAndInitController(sptr<VSyncController> rsController,
        sptr<VSyncController> appController, sptr<VSyncGenerator> vsyncGenerator);
    void InitRsIdleTimer();

    uint32_t currRefreshRate_ = 0;
    uint32_t controllerRate_ = 0;

    // concurrency protection >>>
    std::mutex pendingMutex_;
    std::shared_ptr<uint32_t> pendingRefreshRate_ = nullptr;
    uint64_t pendingConstraintRelativeTime_ = 0;
    int64_t vsyncCountOfChangeGeneratorRate_ = -1; // default vsyncCount
    std::atomic<bool> changeGeneratorRateValid_{ true };
    // concurrency protection <<<

    std::shared_ptr<HgmVSyncGeneratorController> controller_ = nullptr;
    std::vector<std::pair<FrameRateLinkerId, uint32_t>> appChangeData_;

    std::function<void(bool, bool)> forceUpdateCallback_ = nullptr;
    HgmSimpleTimer voterLtpoTimer_;

    std::vector<std::string> voters_;
    // FORMAT: <sceneName, pid>
    std::vector<std::pair<std::string, pid_t>> sceneStack_;
    // FORMAT: "voterName, <<pid, <min, max>>, effective>"
    std::unordered_map<std::string, std::pair<std::vector<VoteInfo>, bool>> voteRecord_;
    // Used to record your votes, and clear your votes after you die
    std::unordered_set<pid_t> pidRecord_;
    std::unordered_set<std::string> gameScenes_;
    std::unordered_set<std::string> ancoScenes_;
    std::unordered_map<pid_t, std::unordered_set<CleanPidCallbackType>> cleanPidCallback_;
    // FORMAT: <timestamp, VoteInfo>
    std::vector<std::pair<int64_t, VoteInfo>> frameRateVoteInfoVec_;

    int32_t curRefreshRateMode_ = HGM_REFRESHRATE_MODE_AUTO;
    ScreenId curScreenId_ = 0;
    std::string curScreenStrategyId_ = "LTPO-DEFAULT";
    bool isLtpo_ = true;
    int32_t idleFps_ = OLED_60_HZ;
    int32_t minIdleFps_ = OLED_60_HZ;
    // rsIdleTimer_ skip rsFrame(see in SetShowRefreshRateEnabled), default value is 1 while ShowRefreshRate disabled
    int32_t skipFrame_ = 1;
    int32_t curSkipCount_ = 0xFF;
    std::unique_ptr<HgmSimpleTimer> rsIdleTimer_ = nullptr;
    VoteInfo lastVoteInfo_;
    HgmMultiAppStrategy multiAppStrategy_;
    HgmTouchManager touchManager_;
    std::atomic<bool> startCheck_ = false;
    HgmIdleDetector idleDetector_;
    bool needHighRefresh_ = false;
    int32_t lastTouchUpExpectFps_ = 0;
    bool isNeedUpdateAppOffset_ = false;
    uint32_t schedulePreferredFps_ = 60;
    int32_t schedulePreferredFpsChange_ = false;
    std::atomic<bool> isAdaptive_ = false;

    uint64_t timestamp_ = 0;
    std::shared_ptr<RSRenderFrameRateLinker> rsFrameRateLinker_ = nullptr;
    FrameRateLinkerMap appFrameRateLinkers_;
};
} // namespace Rosen
} // namespace OHOS
#endif // HGM_FRAME_RATE_MANAGER_H