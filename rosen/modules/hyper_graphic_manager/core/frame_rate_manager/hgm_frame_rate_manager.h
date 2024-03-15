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
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "animation/rs_frame_rate_range.h"
#include "common/rs_common_def.h"
#include "modifier/rs_modifier_type.h"
#include "pipeline/rs_render_frame_rate_linker.h"
#include "screen_manager/screen_types.h"
#include "variable_frame_rate/rs_variable_frame_rate.h"
#include "hgm_one_shot_timer.h"
#include "hgm_command.h"
#include "hgm_screen.h"
#include "hgm_task_handle_thread.h"
#include "hgm_vsync_generator_controller.h"

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
    TOUCH_UP = 4
};

class HgmFrameRateManager {
public:
    HgmFrameRateManager() = default;
    ~HgmFrameRateManager() = default;

    void HandleLightFactorStatus(bool isSafe);
    void HandlePackageEvent(uint32_t listSize, const std::vector<std::string>& packageList);
    void HandleRefreshRateEvent(pid_t pid, const EventInfo& eventInfo);
    void HandleTouchEvent(int32_t touchStatus);
    void HandleTempEvent(const std::string& tempEventName, bool eventStatus, uint32_t min, uint32_t max);

    void CleanVote(pid_t pid);
    RefreshRateMode GetCurRefreshRateMode() const { return curRefreshRateMode_; };
    ScreenId GetCurScreenId() const { return curScreenId_; };
    std::string GetCurScreenStrategyId() const { return curScreenStrategyId_; };
    void HandleRefreshRateMode(RefreshRateMode refreshRateMode);
    void HandleScreenPowerStatus(ScreenId id, ScreenPowerStatus status);
    bool IsLtpo() const { return isLtpo_; };
    void UniProcessDataForLtpo(uint64_t timestamp, std::shared_ptr<RSRenderFrameRateLinker> rsFrameRateLinker,
        const FrameRateLinkerMap& appFrameRateLinkers, bool idleTimerExpired, bool isDvsyncOn);
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
    std::shared_ptr<uint32_t> GetPendingRefreshRate();
    void ResetPendingRefreshRate();
private:
    void Reset();
    bool CollectFrameRateChange(FrameRateRange finalRange, std::shared_ptr<RSRenderFrameRateLinker> rsFrameRateLinker,
        const FrameRateLinkerMap& appFrameRateLinkers);
    void HandleFrameRateChangeForLTPO(uint64_t timestamp, bool isDvsyncOn);
    void FrameRateReport() const;
    void CalcRefreshRate(const ScreenId id, const FrameRateRange& range);
    uint32_t GetDrawingFrameRate(const uint32_t refreshRate, const FrameRateRange& range);
    int32_t GetPreferredFps(const std::string& type, float velocity) const;
    static float PixelToMM(float velocity);

    void HandleIdleEvent(bool isIdle);
    void HandleSceneEvent(pid_t pid, EventInfo eventInfo);
    void HandleVirtualDisplayEvent(pid_t pid, EventInfo eventInfo);
    void SyncAppVote();

    void DeliverRefreshRateVote(pid_t pid, std::string eventName, bool eventStatus,
        uint32_t min = OLED_NULL_HZ, uint32_t max = OLED_NULL_HZ);
    static std::string GetScreenType(ScreenId screenId);
    void MarkVoteChange();
    VoteRange ProcessRefreshRateVote();
    void UpdateVoteRule();

    uint32_t currRefreshRate_ = 0;
    uint32_t controllerRate_ = 0;
    std::shared_ptr<uint32_t> pendingRefreshRate_;
    std::shared_ptr<HgmVSyncGeneratorController> controller_;
    std::vector<std::pair<FrameRateLinkerId, uint32_t>> appChangeData_;

    std::function<void(bool, bool)> forceUpdateCallback_;
    std::unordered_map<ScreenId, std::shared_ptr<HgmOneShotTimer>> screenTimerMap_;

    std::mutex pkgSceneMutex_;
    std::mutex voteMutex_;
    std::mutex voteNameMutex_;
    std::vector<std::string> voters_;
    // FORMAT: <sceneName, pid>
    std::vector<std::pair<std::string, pid_t>> sceneStack_;
    // FORMAT: "voterName, <pid, <min, max>>"
    std::unordered_map<std::string, std::vector<std::pair<pid_t, VoteRange>>> voteRecord_;
    // Used to record your votes, and clear your votes after you die
    std::unordered_set<pid_t> pidRecord_;

    std::string curPkgName_ = "";
    RefreshRateMode curRefreshRateMode_ = HGM_REFRESHRATE_MODE_AUTO;
    ScreenId curScreenId_ = 0;
    std::string curScreenStrategyId_ = "LTPO-DEFAULT";
    bool isLtpo_ = true;
    bool isReduceAllowed_ = true;
    bool isRefreshNeed_ = true;
    bool isTouchEnable_ = true;
    int32_t touchFps_ = 120;
    int32_t idleFps_ = 60;
};
} // namespace Rosen
} // namespace OHOS
#endif // HGM_FRAME_RATE_MANAGER_H