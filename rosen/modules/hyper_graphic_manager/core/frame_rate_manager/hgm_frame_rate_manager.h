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

#include <mutex>
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
        return this->max == other.max && this->voterName == other.voterName &&
            this->extInfo == other.extInfo && this->pid == other.pid && this->bundleName == other.bundleName;
    }

    bool operator!=(const VoteInfo& other) const
    {
        return !(*this == other);
    }
};

class HgmFrameRateManager {
public:
    HgmFrameRateManager() = default;
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
    bool IsLtpo() const { return isLtpo_; };
    void UniProcessDataForLtpo(uint64_t timestamp, std::shared_ptr<RSRenderFrameRateLinker> rsFrameRateLinker,
        const FrameRateLinkerMap& appFrameRateLinkers, bool idleTimerExpired, const DvsyncInfo& dvsyncInfo);
    void UniProcessDataForLtps(bool idleTimerExpired);

    int32_t GetExpectedFrameRate(const RSPropertyUnit unit, float velocity) const;
    std::shared_ptr<HgmOneShotTimer> GetScreenTimer(ScreenId screenId) const;
    void ResetScreenTimer(ScreenId screenId) const;
    void StartScreenTimer(ScreenId screenId, int32_t interval,
        std::function<void()> resetCallback, std::function<void()> expiredCallback);
    void StopScreenTimer(ScreenId screenId);
    void SetForceUpdateCallback(std::function<void(bool, bool)> forceUpdateCallback)
    {
        forceUpdateCallback_ = forceUpdateCallback;
    }

    void Init(sptr<VSyncController> rsController,
        sptr<VSyncController> appController, sptr<VSyncGenerator> vsyncGenerator);
    void InitTouchManager();
    std::shared_ptr<uint32_t> GetPendingRefreshRate();
    void ResetPendingRefreshRate();
    void ProcessPendingRefreshRate(uint64_t timestamp, uint32_t rsRate, const DvsyncInfo& dvsyncInfo);
    HgmMultiAppStrategy& GetMultiAppStrategy() { return multiAppStrategy_; }
    HgmTouchManager& GetTouchManager() { return touchManager_; }
    void UpdateSurfaceTime(const std::string& surfaceName, uint64_t timestamp, pid_t pid);
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
private:
    void Reset();
    void UpdateAppSupportedState();
    void UpdateGuaranteedPlanVote(uint64_t timestamp);

    void ProcessLtpoVote(const FrameRateRange& finalRange, bool idleTimerExpired);
    void SetAceAnimatorVote(const std::shared_ptr<RSRenderFrameRateLinker>& linker, bool& needCheckAceAnimatorStatus);
    bool CollectFrameRateChange(FrameRateRange finalRange, std::shared_ptr<RSRenderFrameRateLinker> rsFrameRateLinker,
        const FrameRateLinkerMap& appFrameRateLinkers);
    void HandleFrameRateChangeForLTPO(uint64_t timestamp);
    void FrameRateReport();
    void CalcRefreshRate(const ScreenId id, const FrameRateRange& range);
    uint32_t GetDrawingFrameRate(const uint32_t refreshRate, const FrameRateRange& range);
    int32_t GetPreferredFps(const std::string& type, float velocity) const;
    static float PixelToMM(float velocity);

    void HandleIdleEvent(bool isIdle);
    void HandleSceneEvent(pid_t pid, EventInfo eventInfo);
    void HandleVirtualDisplayEvent(pid_t pid, EventInfo eventInfo);
    void HandleGamesEvent(pid_t pid, EventInfo eventInfo);

    void DeliverRefreshRateVote(const VoteInfo& voteInfo, bool eventStatus);
    static std::string GetScreenType(ScreenId screenId);
    void MarkVoteChange();
    // merge [VOTER_LTPO, VOTER_IDLE)
    bool MergeLtpo2IdleVote(
        std::vector<std::string>::iterator& voterIter, VoteInfo& resultVoteInfo, VoteRange& mergedVoteRange);
    bool CheckRefreshNeed();
    VoteInfo ProcessRefreshRateVote();
    void UpdateVoteRule();
    void ReportHiSysEvent(const VoteInfo& frameRateVoteInfo);
    void SetResultVoteInfo(VoteInfo& voteInfo, uint32_t min, uint32_t max);
    void UpdateEnergyConsumptionConfig();
    void EnterEnergyConsumptionAssuranceMode();
    void ExitEnergyConsumptionAssuranceMode();
    static void ProcessVoteLog(const VoteInfo& curVoteInfo, bool isSkip);
    void RegisterCoreCallbacksAndInitController(sptr<VSyncController> rsController,
        sptr<VSyncController> appController, sptr<VSyncGenerator> vsyncGenerator);

    uint32_t currRefreshRate_ = 0;
    uint32_t controllerRate_ = 0;
    std::shared_ptr<uint32_t> pendingRefreshRate_ = nullptr;
    uint64_t pendingConstraintRelativeTime_ = 0;
    std::shared_ptr<HgmVSyncGeneratorController> controller_ = nullptr;
    std::mutex appChangeDataMutex_;
    std::vector<std::pair<FrameRateLinkerId, uint32_t>> appChangeData_;

    std::function<void(bool, bool)> forceUpdateCallback_ = nullptr;
    std::unordered_map<ScreenId, std::shared_ptr<HgmOneShotTimer>> screenTimerMap_;

    std::mutex pkgSceneMutex_;
    std::mutex voteMutex_;
    std::mutex voteNameMutex_;
    std::vector<std::string> voters_;
    // FORMAT: <sceneName, pid>
    std::vector<std::pair<std::string, pid_t>> sceneStack_;
    // FORMAT: "voterName, <pid, <min, max>>"
    std::unordered_map<std::string, std::vector<VoteInfo>> voteRecord_;
    // Used to record your votes, and clear your votes after you die
    std::unordered_set<pid_t> pidRecord_;
    std::unordered_set<std::string> gameScenes_;
    std::unordered_set<std::string> ancoScenes_;
    std::mutex cleanPidCallbackMutex_;
    std::unordered_map<pid_t, std::unordered_set<CleanPidCallbackType>> cleanPidCallback_;
    std::mutex frameRateVoteInfoMutex_;
    // FORMAT: <timestamp, VoteInfo>
    std::vector<std::pair<int64_t, VoteInfo>> frameRateVoteInfoVec_;

    int32_t curRefreshRateMode_ = HGM_REFRESHRATE_MODE_AUTO;
    ScreenId curScreenId_ = 0;
    std::string curScreenStrategyId_ = "LTPO-DEFAULT";
    bool isLtpo_ = true;
    bool isRefreshNeed_ = true;
    int32_t idleFps_ = 60;
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
};
} // namespace Rosen
} // namespace OHOS
#endif // HGM_FRAME_RATE_MANAGER_H