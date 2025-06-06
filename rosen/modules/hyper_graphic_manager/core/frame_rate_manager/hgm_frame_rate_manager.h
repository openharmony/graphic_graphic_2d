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

#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "animation/rs_frame_rate_range.h"
#include "common/rs_common_def.h"
#include "hgm_app_page_url_strategy.h"
#include "hgm_command.h"
#include "hgm_frame_voter.h"
#include "hgm_idle_detector.h"
#include "hgm_multi_app_strategy.h"
#include "hgm_soft_vsync_manager.h"
#include "hgm_one_shot_timer.h"
#include "hgm_screen.h"
#include "hgm_task_handle_thread.h"
#include "hgm_touch_manager.h"
#include "hgm_pointer_manager.h"
#include "hgm_voter.h"
#include "hgm_vsync_generator_controller.h"
#include "hgm_user_define.h"
#include "vsync_distributor.h"
#include "modifier/rs_modifier_type.h"
#include "pipeline/rs_render_frame_rate_linker.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_render_node_map.h"
#include "screen_manager/screen_types.h"
#include "variable_frame_rate/rs_variable_frame_rate.h"

namespace OHOS {
namespace Rosen {
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
    TOUCH_BUTTON_DOWN = 8,
    TOUCH_BUTTON_UP = 9,
    TOUCH_PULL_DOWN = 12,
    TOUCH_PULL_MOVE = 13,
    TOUCH_PULL_UP = 14,
};

enum CleanPidCallbackType : uint32_t {
    LIGHT_FACTOR,
    PACKAGE_EVENT,
    TOUCH_EVENT,
    POINTER_EVENT,
    GAMES,
    APP_STRATEGY_CONFIG_EVENT,
    PAGE_URL,
};

enum LightFactorStatus : int32_t {
    // normal level
    NORMAL_HIGH = 0,
    NORMAL_LOW,
    // brightness level
    LOW_LEVEL = 3,
    MIDDLE_LEVEL,
    HIGH_LEVEL,
};

enum SupportASStatus : int32_t {
    NOT_SUPPORT = 0,
    SUPPORT_AS = 1,
    GAME_SCENE_SKIP = 2,
};

class HgmFrameRateManager {
public:
    using ChangeDssRefreshRateCbType = std::function<void(ScreenId, uint32_t, bool)>;
    HgmFrameRateManager();
    ~HgmFrameRateManager() = default;

    void HandleLightFactorStatus(pid_t pid, int32_t state);
    void HandlePackageEvent(pid_t pid, const std::vector<std::string>& packageList);
    void HandleRefreshRateEvent(pid_t pid, const EventInfo& eventInfo);
    void HandleTouchEvent(pid_t pid, int32_t touchStatus, int32_t touchCnt);
    void HandleDynamicModeEvent(bool enableDynamicModeEvent);

    void CleanVote(pid_t pid);
    int32_t GetCurRefreshRateMode() const { return curRefreshRateMode_; };
    uint32_t GetCurrRefreshRate() { return currRefreshRate_; }
    ScreenId GetCurScreenId() const { return curScreenId_.load(); };
    ScreenId GetLastCurScreenId() const { return lastCurScreenId_.load(); };
    void SetLastCurScreenId(ScreenId screenId)
    {
        lastCurScreenId_.store(screenId);
    }
    std::string GetCurScreenStrategyId() const { return curScreenStrategyId_; };
    void HandleRefreshRateMode(int32_t refreshRateMode);
    void HandleScreenPowerStatus(ScreenId id, ScreenPowerStatus status);
    void HandleScreenRectFrameRate(ScreenId id, const GraphicIRect& activeRect);
    void HandleScreenExtStrategyChange(bool status, const std::string& suffix);
    std::string GetCurScreenExtStrategyId();
    void UpdateScreenExtStrategyConfig(const PolicyConfigData::ScreenConfigMap& screenConfigs);

    // called by RSHardwareThread
    void HandleRsFrame();
    bool IsLtpo() const { return isLtpo_; };
    int32_t AdaptiveStatus() const { return isAdaptive_.load(); };
    // called by RSMainThread
    bool IsGameNodeOnTree() const { return isGameNodeOnTree_.load(); };
    void UniProcessDataForLtpo(uint64_t timestamp, std::shared_ptr<RSRenderFrameRateLinker> rsFrameRateLinker,
        const FrameRateLinkerMap& appFrameRateLinkers, const std::map<uint64_t, int>& vRatesMap);

    int32_t GetExpectedFrameRate(const RSPropertyUnit unit, float velocityPx, int32_t areaPx, int32_t lengthPx) const;
    void SetForceUpdateCallback(std::function<void(bool, bool)> forceUpdateCallback)
    {
        forceUpdateCallback_ = forceUpdateCallback;
    }

    void Init(sptr<VSyncController> rsController, sptr<VSyncController> appController,
        sptr<VSyncGenerator> vsyncGenerator, sptr<VSyncDistributor> appDistributor);
    void SetTimeoutParamsFromConfig(const std::shared_ptr<PolicyConfigData>& configData);
    void InitTouchManager();
    // called by RSMainThread
    void ProcessPendingRefreshRate(uint64_t timestamp, int64_t vsyncId, uint32_t rsRate, bool isUiDvsyncOn);
    HgmMultiAppStrategy& GetMultiAppStrategy() { return multiAppStrategy_; }
    HgmTouchManager& GetTouchManager() { return touchManager_; }
    HgmIdleDetector& GetIdleDetector() { return idleDetector_; }
    // only called by RSMainThread
    void UpdateSurfaceTime(const std::string& surfaceName, pid_t pid, UIFWKType uiFwkType);
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

    // only called by RSMainThread
    bool UpdateUIFrameworkDirtyNodes(std::vector<std::weak_ptr<RSRenderNode>>& uiFwkDirtyNodes, uint64_t timestamp);
    // only called by RSMainThread
    bool HandleGameNode(const RSRenderNodeMap& nodeMap);

    void HandleAppStrategyConfigEvent(pid_t pid, const std::string& pkgName,
        const std::vector<std::pair<std::string, std::string>>& newConfig);
    HgmSimpleTimer& GetRsFrameRateTimer() { return rsFrameRateTimer_; };
    void SetChangeDssRefreshRateCb(ChangeDssRefreshRateCbType changeDssRefreshRateCb)
    {
        changeDssRefreshRateCb_ = changeDssRefreshRateCb;
    }
    void ProcessPageUrlVote(pid_t pid, std::string strategy, const bool isAddVoter);
    void CleanPageUrlVote(pid_t pid);
    void HandlePageUrlEvent();
    void NotifyPageName(pid_t pid, const std::string &packageName, const std::string &pageName, bool isEnter);
    // called by OS_IPC thread
    bool SetVsyncRateDiscountLTPO(const std::vector<uint64_t>& linkerIds, uint32_t rateDiscount);
    HgmSoftVSyncManager& SoftVSyncMgrRef() { return softVSyncManager_; };
    HgmFrameVoter& FrameVoterRef() { return frameVoter_; }
private:
    friend class HgmUserDefineImpl;

    void InitConfig();
    void Reset();
    void UpdateAppSupportedState();
    void UpdateGuaranteedPlanVote(uint64_t timestamp);

    void ProcessLtpoVote(const FrameRateRange& finalRange);
    void SetAceAnimatorVote(const std::shared_ptr<RSRenderFrameRateLinker>& linker);
    void HandleFrameRateChangeForLTPO(uint64_t timestamp, bool followRs, bool frameRateChange);
    void DVSyncTaskProcessor(int64_t delayTime, uint64_t targetTime,
        std::vector<std::pair<FrameRateLinkerId, uint32_t>> appChangeData, int64_t controllerRate);
    void UpdateSoftVSync(bool followRs);
    void SetChangeGeneratorRateValid(bool valid);
    void FrameRateReport();
    uint32_t CalcRefreshRate(const ScreenId id, const FrameRateRange& range) const;
    int32_t GetPreferredFps(const std::string& type, float velocityMM, float areaMM, float lengthMM) const;
    template<typename T>
    static float PixelToMM(T pixel);
    template<typename T>
    static float SqrPixelToSqrMM(T sqrPixel);

    void HandleIdleEvent(bool isIdle);
    void HandleStylusSceneEvent(const std::string& sceneName);
    void HandleSceneEvent(pid_t pid, EventInfo eventInfo);
    void HandleVirtualDisplayEvent(pid_t pid, EventInfo eventInfo);
    void HandleGamesEvent(pid_t pid, EventInfo eventInfo);
    void HandleMultiSelfOwnedScreenEvent(pid_t pid, EventInfo eventInfo);
    void HandleTouchTask(pid_t pid, int32_t touchStatus, int32_t touchCnt);
    void HandlePointerTask(pid_t pid, int32_t pointerStatus, int32_t pointerCnt);
    void HandleScreenFrameRate(std::string curScreenName);
    void UpdateScreenFrameRate();
    void RegisterUpTimeoutAndDownEvent();

    void GetSupportedRefreshRates(const std::shared_ptr<PolicyConfigData>& configData,
        const std::string& modeKey, std::vector<uint32_t>& targetVec, bool handleAmbientEffect);
    void GetLowBrightVec(const std::shared_ptr<PolicyConfigData>& configData);
    void GetAncoLowBrightVec(const std::shared_ptr<PolicyConfigData>& configData);
    void GetStylusVec(const std::shared_ptr<PolicyConfigData>& configData);
    void DeliverRefreshRateVote(const VoteInfo& voteInfo, bool eventStatus);
    void MarkVoteChange(const std::string& voter = "");
    static bool IsCurrentScreenSupportAS();
    void ProcessAdaptiveSync(const std::string& voterName);
    bool CheckAncoVoterStatus() const;
    VoteInfo ProcessRefreshRateVote();
    void ReportHiSysEvent(const VoteInfo& frameRateVoteInfo);
    void SetResultVoteInfo(VoteInfo& voteInfo, uint32_t min, uint32_t max);
    void UpdateEnergyConsumptionConfig();
    void RegisterCoreCallbacksAndInitController(sptr<VSyncController> rsController,
        sptr<VSyncController> appController,
        sptr<VSyncGenerator> vsyncGenerator, sptr<VSyncDistributor> appDistributor);
    uint32_t UpdateFrameRateWithDelay(uint32_t refreshRate);
    std::string GetGameNodeName() const
    {
        std::lock_guard<std::mutex> lock(pendingMutex_);
        return curGameNodeName_;
    }
    void CheckRefreshRateChange(
        bool followRs, bool frameRateChanged, uint32_t refreshRate, bool needChangeDssRefreshRate);
    void SetGameNodeName(std::string nodeName)
    {
        std::lock_guard<std::mutex> lock(pendingMutex_);
        curGameNodeName_ = nodeName;
    }
    void FrameRateReportTask(uint32_t leftRetryTimes);
    void CheckNeedUpdateAppOffset(uint32_t refreshRate, uint32_t controllerRate);

    std::atomic<uint32_t> currRefreshRate_ = 0;

    // concurrency protection >>>
    mutable std::mutex pendingMutex_;
    std::shared_ptr<uint32_t> pendingRefreshRate_ = nullptr;
    uint64_t pendingConstraintRelativeTime_ = 0;
    uint64_t lastPendingConstraintRelativeTime_ = 0;
    uint32_t lastPendingRefreshRate_ = 0;
    int64_t vsyncCountOfChangeGeneratorRate_ = -1; // default vsyncCount
    std::atomic<bool> changeGeneratorRateValid_{ true };
    HgmSimpleTimer changeGeneratorRateValidTimer_;
    // if current game's self drawing node is on tree,default false
    std::atomic<bool> isGameNodeOnTree_ = false;
    // current game app's self drawing node name
    std::string curGameNodeName_;
    // concurrency protection <<<

    std::shared_ptr<HgmVSyncGeneratorController> controller_ = nullptr;
    std::vector<uint32_t> lowBrightVec_;
    std::vector<uint32_t> ancoLowBrightVec_;
    std::vector<uint32_t> stylusVec_;

    std::function<void(bool, bool)> forceUpdateCallback_ = nullptr;
    HgmSimpleTimer rsFrameRateTimer_;

    std::unordered_map<pid_t, std::unordered_set<CleanPidCallbackType>> cleanPidCallback_;
    // FORMAT: <timestamp, VoteInfo>
    std::vector<std::pair<int64_t, VoteInfo>> frameRateVoteInfoVec_;

    int32_t curRefreshRateMode_ = HGM_REFRESHRATE_MODE_AUTO;
    std::atomic<ScreenId> curScreenId_ = 0;
    std::atomic<ScreenId> lastCurScreenId_ = 0;
    std::string curScreenStrategyId_ = "LTPO-DEFAULT";
    std::string curScreenDefaultStrategyId_ = "LTPO-DEFAULT";
    bool isLtpo_ = true;
    std::unordered_map<std::string, std::pair<int32_t, bool>> screenExtStrategyMap_ = HGM_CONFIG_SCREENEXT_STRATEGY_MAP;
    int32_t isAmbientStatus_ = 0;
    bool isAmbientEffect_ = false;
    bool isStylusWakeUp_ = false;
    int32_t idleFps_ = OLED_60_HZ;
    VoteInfo lastVoteInfo_;
    HgmMultiAppStrategy multiAppStrategy_;
    HgmTouchManager touchManager_;
    HgmPointerManager pointerManager_;
    HgmSoftVSyncManager softVSyncManager_;
    HgmFrameVoter frameVoter_;
    HgmUserDefine userDefine_;;
    std::atomic<bool> voterTouchEffective_ = false;
    // For the power consumption module, only monitor touch up 3s and 600ms without flashing frames
    std::atomic<bool> startCheck_ = false;
    HgmIdleDetector idleDetector_;
    // only called by RSMainThread
    // exp. <"AceAnimato", pid, FROM_SURFACE>
    std::vector<std::tuple<std::string, pid_t, UIFWKType>> surfaceData_;
    uint64_t lastPostIdleDetectorTaskTimestamp_ = 0; // only called by RSMainThread
    int32_t lastTouchUpExpectFps_ = 0;
    bool isNeedUpdateAppOffset_ = false;
    uint32_t schedulePreferredFps_ = 60;
    int32_t schedulePreferredFpsChange_ = false;
    std::atomic<int32_t> isAdaptive_ = SupportASStatus::NOT_SUPPORT;
    // Does current game require Adaptive Sync
    int32_t isGameSupportAS_ = SupportASStatus::NOT_SUPPORT;

    std::atomic<uint64_t> timestamp_ = 0;
    std::shared_ptr<RSRenderFrameRateLinker> rsFrameRateLinker_ = nullptr;
    FrameRateLinkerMap appFrameRateLinkers_;
    ChangeDssRefreshRateCbType changeDssRefreshRateCb_;
    HgmAppPageUrlStrategy appPageUrlStrategy_;

    uint32_t lastLTPORefreshRate_ = 0;
    long lastLTPOVoteTime_ = 0;
};
} // namespace Rosen
} // namespace OHOS
#endif // HGM_FRAME_RATE_MANAGER_H
