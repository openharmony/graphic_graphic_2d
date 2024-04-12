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

#include "hgm_frame_rate_manager.h"

#include <algorithm>
#include <ctime>
#include "common/rs_optional_trace.h"
#include "common/rs_thread_handler.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "hgm_config_callback_manager.h"
#include "hgm_core.h"
#include "hgm_log.h"
#include "parameters.h"
#include "rs_trace.h"
#include "sandbox_utils.h"
#include "frame_rate_report.h"
#include "hgm_config_callback_manager.h"
#include "hisysevent.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr float MARGIN = 0.00001;
    constexpr float MIN_DRAWING_DIVISOR = 10.0f;
    constexpr float DIVISOR_TWO = 2.0f;
    constexpr int32_t IDLE_TIMER_EXPIRED = 200; // ms
    constexpr float ONE_MS_IN_NANO = 1000000.0f;
    constexpr uint32_t UNI_RENDER_VSYNC_OFFSET = 5000000; // ns
    constexpr uint32_t REPORT_VOTER_INFO_LIMIT = 10;
    constexpr int32_t LAST_TOUCH_DOWN_CNT = 1;

    constexpr uint32_t SCENE_BEFORE_XML = 1;
    constexpr uint32_t SCENE_AFTER_TOUCH = 3;
    // CAUTION: with priority
    const std::string VOTER_NAME[] = {
        "VOTER_THERMAL",
        "VOTER_GAMES",
        "VOTER_VIDEO",
        "VOTER_VIRTUALDISPLAY",
        "VOTER_MULTI_APP",
        "VOTER_ANCO",

        "VOTER_XML",
        "VOTER_TOUCH",
        "VOTER_LTPO",
        "VOTER_SCENE",
        "VOTER_IDLE"
    };
}

void HgmFrameRateManager::Init(sptr<VSyncController> rsController,
    sptr<VSyncController> appController, sptr<VSyncGenerator> vsyncGenerator)
{
    voters_ = std::vector<std::string>(std::begin(VOTER_NAME), std::end(VOTER_NAME));
    auto& hgmCore = HgmCore::Instance();
    curRefreshRateMode_ = hgmCore.GetCurrentRefreshRateMode();

    // hgm warning: get non active screenId in non-folding devices（from sceneboard）
    auto screenList = hgmCore.GetScreenIds();
    curScreenId_ = screenList.empty() ? 0 : screenList.front();
    isLtpo_ = (GetScreenType(curScreenId_) == "LTPO");
    std::string curScreenName = "screen" + std::to_string(curScreenId_) + "_" + (isLtpo_ ? "LTPO" : "LTPS");
    auto configData = hgmCore.GetPolicyConfigData();
    if (configData != nullptr) {
        curScreenStrategyId_ = configData->screenStrategyConfigs_[curScreenName];
        if (curScreenStrategyId_.empty()) {
            curScreenStrategyId_ = "LTPO-DEFAULT";
        }
        SyncAppVote();
        HandleIdleEvent(ADD_VOTE);
    }

    hgmCore.RegisterRefreshRateModeChangeCallback([rsController, appController](int32_t mode) {
        if (HgmCore::Instance().IsLTPOSwitchOn()) {
            rsController->SetPhaseOffset(0);
            appController->SetPhaseOffset(0);
            CreateVSyncGenerator()->SetVSyncMode(VSYNC_MODE_LTPO);
        } else {
            if (RSUniRenderJudgement::IsUniRender()) {
                rsController->SetPhaseOffset(UNI_RENDER_VSYNC_OFFSET);
                appController->SetPhaseOffset(UNI_RENDER_VSYNC_OFFSET);
            }
            CreateVSyncGenerator()->SetVSyncMode(VSYNC_MODE_LTPS);
        }
    });

    hgmCore.RegisterRefreshRateUpdateCallback([](int32_t refreshRate) {
        HgmConfigCallbackManager::GetInstance()->SyncRefreshRateUpdateCallback(refreshRate);
    });

    controller_ = std::make_shared<HgmVSyncGeneratorController>(rsController, appController, vsyncGenerator);

    touchMgr_->touchMachine_.RegisterIdleEventCallback([this] () {
        DeliverRefreshRateVote(0, "VOTER_TOUCH", REMOVE_VOTE);
    });
}

void HgmFrameRateManager::UniProcessDataForLtpo(uint64_t timestamp,
                                                std::shared_ptr<RSRenderFrameRateLinker> rsFrameRateLinker,
                                                const FrameRateLinkerMap& appFrameRateLinkers, bool idleTimerExpired,
                                                bool isDvsyncOn)
{
    RS_TRACE_FUNC();
    Reset();

    auto& hgmCore = HgmCore::Instance();
    FrameRateRange finalRange;
    FrameRateVoteInfo frameRateVoteInfo;
    frameRateVoteInfo.SetTimestamp(std::time(nullptr));
    if (curRefreshRateMode_ == HGM_REFRESHRATE_MODE_AUTO) {
        finalRange = rsFrameRateLinker->GetExpectedRange();
        if (finalRange.IsValid()) {
            frameRateVoteInfo.SetLtpoInfo(0, "ANIMATE");
        }
        for (auto linker : appFrameRateLinkers) {
            if (finalRange.Merge(linker.second->GetExpectedRange())) {
                frameRateVoteInfo.SetLtpoInfo(linker.second->GetId(), "APP_LINKER");
            }
        }

        if (finalRange.IsValid()) {
            ResetScreenTimer(curScreenId_);
            CalcRefreshRate(curScreenId_, finalRange);
            DeliverRefreshRateVote(0, "VOTER_LTPO", ADD_VOTE, currRefreshRate_, currRefreshRate_);
        } else if (idleTimerExpired) {
            // idle in ltpo
            HandleIdleEvent(ADD_VOTE);
            DeliverRefreshRateVote(0, "VOTER_LTPO", REMOVE_VOTE);
        } else {
            StartScreenTimer(curScreenId_, IDLE_TIMER_EXPIRED, nullptr, [this]() {
                forceUpdateCallback_(true, false);
            });
        }
    }

    VoteRange voteResult = ProcessRefreshRateVote(frameRateVoteInfo);
    // max used here
    finalRange = {voteResult.second, voteResult.second, voteResult.second};
    CalcRefreshRate(curScreenId_, finalRange);

    bool frameRateChanged = CollectFrameRateChange(finalRange, rsFrameRateLinker, appFrameRateLinkers);
    if (hgmCore.GetLtpoEnabled() && frameRateChanged) {
        HandleFrameRateChangeForLTPO(timestamp, isDvsyncOn);
    } else {
        pendingRefreshRate_ = std::make_shared<uint32_t>(currRefreshRate_);
        if (currRefreshRate_ != hgmCore.GetPendingScreenRefreshRate()) {
            forceUpdateCallback_(false, true);
            touchMgr_->rsIdleUpdateCallback_(false);
            FrameRateReport();
        }
    }

    if (isDvsyncOn) {
        pendingRefreshRate_ = std::make_shared<uint32_t>(currRefreshRate_);
    }

    ReportHiSysEvent(frameRateVoteInfo);
}

void HgmFrameRateManager::ReportHiSysEvent(const FrameRateVoteInfo& frameRateVoteInfo)
{
    if (frameRateVoteInfo.voterName.empty()) {
        return;
    }
    if (frameRateVoteInfoVec_.empty()) {
        frameRateVoteInfoVec_.emplace_back(frameRateVoteInfo);
        return;
    }
    if (frameRateVoteInfoVec_.back().voterName != frameRateVoteInfo.voterName ||
        frameRateVoteInfoVec_.back().preferred != frameRateVoteInfo.preferred) {
        if (frameRateVoteInfoVec_.size() >= REPORT_VOTER_INFO_LIMIT) {
            std::string msg;
            for (auto& info : frameRateVoteInfoVec_) {
                msg += info.ToString();
            }
            HiSysEventWrite(OHOS::HiviewDFX::HiSysEvent::Domain::GRAPHIC, "HGM_VOTER_INFO",
                OHOS::HiviewDFX::HiSysEvent::EventType::STATISTIC, "MSG", msg);
            frameRateVoteInfoVec_.clear();
        }
        frameRateVoteInfoVec_.emplace_back(frameRateVoteInfo);
    }
}

void HgmFrameRateManager::UniProcessDataForLtps(bool idleTimerExpired)
{
    RS_TRACE_FUNC();
    Reset();

    if (idleTimerExpired) {
        // idle in ltps
        HandleIdleEvent(ADD_VOTE);
    } else {
        StartScreenTimer(curScreenId_, IDLE_TIMER_EXPIRED, nullptr, [this]() {
            forceUpdateCallback_(true, false);
        });
    }

    FrameRateRange finalRange;
    FrameRateVoteInfo frameRateVoteInfo;
    VoteRange voteResult = ProcessRefreshRateVote(frameRateVoteInfo);
    auto& hgmCore = HgmCore::Instance();
    uint32_t lastPendingRate = hgmCore.GetPendingScreenRefreshRate();
    // max used here
    finalRange = {voteResult.second, voteResult.second, voteResult.second};
    CalcRefreshRate(curScreenId_, finalRange);
    if ((currRefreshRate_ < lastPendingRate) && !isReduceAllowed_) {
        // Can't reduce the refreshRate in ltps mode
        RS_TRACE_NAME_FMT("Can't reduce to [%d], keep [%d] please", currRefreshRate_, lastPendingRate);
        currRefreshRate_ = lastPendingRate;
    }

    pendingRefreshRate_ = std::make_shared<uint32_t>(currRefreshRate_);
    if (currRefreshRate_ != hgmCore.GetPendingScreenRefreshRate()) {
        forceUpdateCallback_(false, true);
        touchMgr_->rsIdleUpdateCallback_(false);
        FrameRateReport();
    }
    ReportHiSysEvent(frameRateVoteInfo);
}

void HgmFrameRateManager::FrameRateReport() const
{
    std::unordered_map<pid_t, uint32_t> rates;
    rates[GetRealPid()] = currRefreshRate_;
    auto alignRate = HgmCore::Instance().GetAlignRate();
    rates[UNI_APP_PID] = (alignRate == 0) ? currRefreshRate_ : alignRate;
    FRAME_TRACE::FrameRateReport::GetInstance().SendFrameRates(rates);
}

bool HgmFrameRateManager::CollectFrameRateChange(FrameRateRange finalRange,
                                                 std::shared_ptr<RSRenderFrameRateLinker> rsFrameRateLinker,
                                                 const FrameRateLinkerMap& appFrameRateLinkers)
{
    bool frameRateChanged = false;
    bool controllerRateChanged = false;
    auto rsFrameRate = GetDrawingFrameRate(currRefreshRate_, finalRange);
    controllerRate_ = rsFrameRate > 0 ? rsFrameRate : controller_->GetCurrentRate();
    if (controllerRate_ != controller_->GetCurrentRate()) {
        rsFrameRateLinker->SetFrameRate(controllerRate_);
        controllerRateChanged = true;
        frameRateChanged = true;
    }

    auto& hgmCore = HgmCore::Instance();
    auto screenCurrentRefreshRate = hgmCore.GetScreenCurrentRefreshRate(hgmCore.GetActiveScreenId());
    RS_TRACE_NAME_FMT("CollectFrameRateChange refreshRate: %d, rsFrameRate: %d, finalRange = (%d, %d, %d)",
        screenCurrentRefreshRate, rsFrameRate, finalRange.min_, finalRange.max_, finalRange.preferred_);
    RS_TRACE_INT("PreferredFrameRate", static_cast<int>(finalRange.preferred_));

    for (auto linker : appFrameRateLinkers) {
        auto appFrameRate = GetDrawingFrameRate(currRefreshRate_, linker.second->GetExpectedRange());
        if (appFrameRate != linker.second->GetFrameRate() || controllerRateChanged) {
            linker.second->SetFrameRate(appFrameRate);
            appChangeData_.emplace_back(linker.second->GetId(), appFrameRate);
            HGM_LOGD("HgmFrameRateManager: appChangeData linkerId = %{public}" PRIu64 ", %{public}d",
                linker.second->GetId(), appFrameRate);
            frameRateChanged = true;
        }
        RS_TRACE_NAME_FMT("HgmFrameRateManager::UniProcessData multiAppFrameRate: pid = %d, appFrameRate = %d, "\
            "appRange = (%d, %d, %d)", ExtractPid(linker.first), appFrameRate, linker.second->GetExpectedRange().min_,
            linker.second->GetExpectedRange().max_, linker.second->GetExpectedRange().preferred_);
    }
    return frameRateChanged;
}

void HgmFrameRateManager::HandleFrameRateChangeForLTPO(uint64_t timestamp, bool isDvsyncOn)
{
    RSTaskMessage::RSTask task = [this]() {
        controller_->ChangeGeneratorRate(controllerRate_, appChangeData_);
        pendingRefreshRate_ = std::make_shared<uint32_t>(currRefreshRate_);
        if (currRefreshRate_ != HgmCore::Instance().GetPendingScreenRefreshRate()) {
            forceUpdateCallback_(false, true);
            touchMgr_->rsIdleUpdateCallback_(false);
            FrameRateReport();
        }
    };

    if (isDvsyncOn) {
        HgmTaskHandleThread::Instance().PostTask(task);
        return;
    }
    uint64_t expectTime = timestamp + static_cast<uint64_t>(controller_->GetCurrentOffset());
    uint64_t currTime = static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count());
    int64_t delayTime = std::ceil(static_cast<int64_t>(expectTime - currTime) * 1.0f / ONE_MS_IN_NANO) + 1;
    if (delayTime <= 0) {
        HgmTaskHandleThread::Instance().PostTask(task);
    } else {
        HgmTaskHandleThread::Instance().PostTask(task, delayTime);
    }
}

void HgmFrameRateManager::CalcRefreshRate(const ScreenId id, const FrameRateRange& range)
{
    // Find current refreshRate by FrameRateRange. For example:
    // 1. FrameRateRange[min, max, preferred] is [24, 48, 48], supported refreshRates
    // of current screen are {30, 60, 90}, the result should be 30, not 60.
    // 2. FrameRateRange[min, max, preferred] is [150, 150, 150], supported refreshRates
    // of current screen are {30, 60, 90}, the result will be 90.
    auto supportRefreshRateVec = HgmCore::Instance().GetScreenSupportedRefreshRates(id);
    if (supportRefreshRateVec.empty()) {
        return;
    }
    std::sort(supportRefreshRateVec.begin(), supportRefreshRateVec.end());
    auto iter = std::lower_bound(supportRefreshRateVec.begin(), supportRefreshRateVec.end(), range.preferred_);
    if (iter != supportRefreshRateVec.end()) {
        currRefreshRate_ = *iter;
        if (currRefreshRate_ > static_cast<uint32_t>(range.max_) &&
            (iter - supportRefreshRateVec.begin()) > 0) {
            iter--;
            if (*iter >= static_cast<uint32_t>(range.min_) &&
                *iter <= static_cast<uint32_t>(range.max_)) {
                currRefreshRate_ = *iter;
            }
        }
    } else {
        currRefreshRate_ = supportRefreshRateVec.back();
    }
}

uint32_t HgmFrameRateManager::GetDrawingFrameRate(const uint32_t refreshRate, const FrameRateRange& range)
{
    // We will find a drawing fps, which is divisible by refreshRate.
    // If the refreshRate is 60, the options of drawing fps are 60, 30, 15, 12, etc.
    // 1. The preferred fps is divisible by refreshRate.
    const float currRefreshRate = static_cast<float>(refreshRate);
    const float preferredFps = static_cast<float>(range.preferred_);
    if (preferredFps < MARGIN || currRefreshRate < MARGIN) {
        return 0;
    }
    if (std::fmodf(currRefreshRate, range.preferred_) < MARGIN) {
        return static_cast<uint32_t>(preferredFps);
    }
    // 2. FrameRateRange is not dynamic, we will find the closest drawing fps to preferredFps.
    // e.g. If the FrameRateRange of a surfaceNode is [50, 50, 50], the refreshRate is
    // 90, the drawing fps of the surfaceNode should be 45.
    if (!range.IsDynamic()) {
        return static_cast<uint32_t>(currRefreshRate / std::round(refreshRate / preferredFps));
    }
    // 3. FrameRateRange is dynamic. We will find a divisible result in the range if possible.
    // If several divisible options are in the range, the smoother, the better.
    // The KPI of "smooth" is the ratio of lack.
    // e.g. The preferred fps is 58, the refreshRate is 60. When the drawing fps is 60,
    // we lack the least(the ratio is 2/60).
    // The preferred fps is 34, the refreshRate is 60, the drawing fps will be 30(the ratio is 4/30).
    int divisor = 1;
    float drawingFps = currRefreshRate;
    float dividedFps = currRefreshRate;
    float currRatio = std::abs(dividedFps - preferredFps) / preferredFps;
    float ratio = currRatio;
    const float minDrawingFps = currRefreshRate / MIN_DRAWING_DIVISOR;
    while (dividedFps > minDrawingFps - MARGIN) {
        if (dividedFps < range.min_ || dividedFps <= static_cast<float>(range.preferred_) / DIVISOR_TWO) {
            break;
        }
        if (dividedFps > range.max_) {
            divisor++;
            float preDividedFps = dividedFps;
            dividedFps = currRefreshRate / static_cast<float>(divisor);
            // If we cannot find a divisible result, the closer to the preferred, the better.
            // e.g.FrameRateRange is [50, 80, 80], refreshrate is
            // 90, the drawing frame rate is 90.
            if (dividedFps < range.min_ && (preferredFps - dividedFps) > (preDividedFps - preferredFps)) {
                drawingFps = preDividedFps;
                break;
            }
            currRatio = std::abs(dividedFps - preferredFps) / preferredFps;
            if (currRatio < ratio) {
                ratio = currRatio;
                drawingFps = dividedFps;
            }
            continue;
        }
        currRatio = std::min(std::fmodf(preferredFps, dividedFps),
            std::fmodf(std::abs(dividedFps - preferredFps), dividedFps)) / dividedFps;
        // When currRatio is almost zero, dividedFps is the perfect result
        if (currRatio < MARGIN) {
            drawingFps = dividedFps;
            break;
        }
        if (currRatio < ratio) {
            ratio = currRatio;
            drawingFps = dividedFps;
        }
        divisor++;
        dividedFps = currRefreshRate / static_cast<float>(divisor);
    }
    return static_cast<uint32_t>(std::round(drawingFps));
}

std::shared_ptr<uint32_t> HgmFrameRateManager::GetPendingRefreshRate()
{
    return pendingRefreshRate_;
}

void HgmFrameRateManager::ResetPendingRefreshRate()
{
    pendingRefreshRate_.reset();
}

void HgmFrameRateManager::Reset()
{
    currRefreshRate_ = 0;
    controllerRate_ = 0;
    pendingRefreshRate_.reset();
    appChangeData_.clear();
}

int32_t HgmFrameRateManager::GetExpectedFrameRate(const RSPropertyUnit unit, float velocity) const
{
    switch (unit) {
        case RSPropertyUnit::PIXEL_POSITION:
            return GetPreferredFps("translate", PixelToMM(velocity));
        case RSPropertyUnit::PIXEL_SIZE:
        case RSPropertyUnit::RATIO_SCALE:
            return GetPreferredFps("scale", PixelToMM(velocity));
        case RSPropertyUnit::ANGLE_ROTATION:
            return GetPreferredFps("rotation", velocity);
        default:
            return 0;
    }
}

int32_t HgmFrameRateManager::GetPreferredFps(const std::string& type, float velocity) const
{
    auto &configData = HgmCore::Instance().GetPolicyConfigData();
    if (!configData) {
        return 0;
    }
    const std::string settingMode = std::to_string(curRefreshRateMode_);
    if (configData->screenConfigs_.count(curScreenStrategyId_) &&
        configData->screenConfigs_[curScreenStrategyId_].count(settingMode) &&
        configData->screenConfigs_[curScreenStrategyId_][settingMode].animationDynamicSettings.count(type)) {
        auto& config = configData->screenConfigs_[curScreenStrategyId_][settingMode].animationDynamicSettings[type];
        auto iter = std::find_if(config.begin(), config.end(), [&velocity](const auto& pair) {
            return velocity >= pair.second.min && (velocity < pair.second.max || pair.second.max == -1);
        });
        if (iter != config.end()) {
            RS_OPTIONAL_TRACE_NAME_FMT("GetPreferredFps: type: %s, speed: %f, rate: %d",
                type.c_str(), velocity, iter->second.preferred_fps);
            return iter->second.preferred_fps;
        }
    }
    return 0;
}

float HgmFrameRateManager::PixelToMM(float velocity)
{
    float velocityMM = 0.0f;
    auto& hgmCore = HgmCore::Instance();
    sptr<HgmScreen> hgmScreen = hgmCore.GetScreen(hgmCore.GetActiveScreenId());
    if (hgmScreen && hgmScreen->GetPpi() > 0.f) {
        velocityMM = velocity / hgmScreen->GetPpi() * INCH_2_MM;
    }
    return velocityMM;
}

std::shared_ptr<HgmOneShotTimer> HgmFrameRateManager::GetScreenTimer(ScreenId screenId) const
{
    if (auto timer = screenTimerMap_.find(screenId); timer != screenTimerMap_.end()) {
        return timer->second;
    }
    return nullptr;
}

void HgmFrameRateManager::StartScreenTimer(ScreenId screenId, int32_t interval,
    std::function<void()> resetCallback, std::function<void()> expiredCallback)
{
    if (auto oldtimer = GetScreenTimer(screenId); oldtimer == nullptr) {
        auto newTimer = std::make_shared<HgmOneShotTimer>("idle_timer" + std::to_string(screenId),
            std::chrono::milliseconds(interval), resetCallback, expiredCallback);
        screenTimerMap_[screenId] = newTimer;
        newTimer->Start();
    }
}

void HgmFrameRateManager::ResetScreenTimer(ScreenId screenId) const
{
    if (auto timer = GetScreenTimer(screenId); timer != nullptr) {
        timer->Reset();
    }
}

void HgmFrameRateManager::StopScreenTimer(ScreenId screenId)
{
    if (auto timer = screenTimerMap_.find(screenId); timer != screenTimerMap_.end()) {
        screenTimerMap_.erase(timer);
    }
}

void HgmFrameRateManager::HandleLightFactorStatus(bool isSafe)
{
    // based on the light determine whether allowed to reduce the screen refresh rate to avoid screen flicker
    HGM_LOGI("HandleLightFactorStatus status:%{public}u", isSafe);
    isReduceAllowed_ = isSafe;
}

void HgmFrameRateManager::HandlePackageEvent(uint32_t listSize, const std::vector<std::string>& packageList)
{
    // the focus app agreed at the front of packageList
    std::lock_guard<std::mutex> locker(pkgSceneMutex_);

    std::string curPkgName = packageList.front();
    HGM_LOGI("HandlePackageEvent curPkg:[%{public}s] pkgNum:[%{public}d]", curPkgName.c_str(), listSize);
    if (curPkgName_ != curPkgName) {
        curPkgName_ = curPkgName;
        sceneStack_.clear();
    }

    SyncAppVote();
}

void HgmFrameRateManager::HandleRefreshRateEvent(pid_t pid, const EventInfo& eventInfo)
{
    std::string eventName = eventInfo.eventName;
    std::lock_guard<std::mutex> lock(voteNameMutex_);
    auto event = std::find(voters_.begin(), voters_.end(), eventName);
    if (event == voters_.end()) {
        HGM_LOGW("HgmFrameRateManager:unknown event, eventName is %{public}s", eventName.c_str());
        return;
    }

    HGM_LOGI("HandleRefreshRateEvent: %{public}s(%{public}d)", eventName.c_str(), pid);
    if (eventName == "VOTER_SCENE") {
        HandleSceneEvent(pid, eventInfo);
    } else if (eventName == "VOTER_VIRTUALDISPLAY") {
        HandleVirtualDisplayEvent(pid, eventInfo);
    } else {
        DeliverRefreshRateVote(pid, eventName, eventInfo.eventStatus,
            eventInfo.minRefreshRate, eventInfo.maxRefreshRate);
    }
}

void HgmFrameRateManager::HandleTouchEvent(int32_t touchStatus)
{
    HGM_LOGD("HandleTouchEvent status:%{public}d", touchStatus);
    if (!isTouchEnable_) {
        return;
    }

    static std::mutex hgmTouchEventMutex;
    std::unique_lock<std::mutex> lock(hgmTouchEventMutex);
    if (touchCnt_ < 0) {
        touchCnt_ = 0;
    }
    if (touchStatus == TOUCH_DOWN || touchStatus == TOUCH_PULL_DOWN) {
        touchCnt_++;
        if (touchCnt_ == LAST_TOUCH_DOWN_CNT) {
            DeliverRefreshRateVote(0, "VOTER_TOUCH", ADD_VOTE, touchFps_, touchFps_);
            HGM_LOGI("[touch manager] update to target %{public}d fps", touchFps_);
            StopScreenTimer(curScreenId_);
            touchMgr_->StopRSTimer(curScreenId_);
            touchMgr_->touchMachine_.TouchEventHandle(TouchEvent::DOWN);
        }
    } else if (touchStatus == TOUCH_UP || touchStatus == TOUCH_PULL_UP) {
        touchCnt_--;
        if (touchCnt_ == 0) {
            HGM_LOGI("[touch manager] touch up detect");
            StartScreenTimer(curScreenId_, TOUCH_UP_TIMEOUT_TIMER_EXPIRED, nullptr, [this]() {
                forceUpdateCallback_(true, false);
            });
            touchMgr_->touchMachine_.TouchEventHandle(TouchEvent::UP);
            touchMgr_->StartRSTimer(curScreenId_, TOUCH_RS_IDLE_TIMER_EXPIRED, nullptr, [this]() {
                touchMgr_->rsIdleUpdateCallback_(true);
            });
        }
    } else {
        HGM_LOGD("[touch manager] other touch status not support");
    }
}

void HgmFrameRateManager::HandleIdleEvent(bool isIdle)
{
    if (isIdle) {
        HGM_LOGI("HandleIdleEvent status:%{public}u", isIdle);
        // fix for down event but not refreshing frames condition
        if (touchMgr_->touchMachine_.GetState() != TouchState::DOWN) {
            DeliverRefreshRateVote(0, "VOTER_IDLE", ADD_VOTE, idleFps_, idleFps_);
        }
    } else {
        DeliverRefreshRateVote(0, "VOTER_IDLE", REMOVE_VOTE);
    }
}

void HgmFrameRateManager::HandleRefreshRateMode(int32_t refreshRateMode)
{
    HGM_LOGI("HandleRefreshRateMode curMode:%{public}d", refreshRateMode);
    if (curRefreshRateMode_ == refreshRateMode) {
        return;
    }

    curRefreshRateMode_ = refreshRateMode;
    SyncAppVote();
    HgmCore::Instance().SetLtpoConfig();
    FrameRateReport();
    HgmConfigCallbackManager::GetInstance()->SyncHgmConfigChangeCallback();
}

void HgmFrameRateManager::HandleScreenPowerStatus(ScreenId id, ScreenPowerStatus status)
{
    // hgm warning: strategy for screen off
    HGM_LOGI("HandleScreenPowerStatus curScreen:%{public}d status:%{public}d",
        static_cast<int>(curScreenId_), static_cast<int>(status));
    if (status != ScreenPowerStatus::POWER_STATUS_ON) {
        touchCnt_ = 0;
        return;
    }
    if (curScreenId_ == id) {
        return;
    }

    auto& hgmCore = HgmCore::Instance();
    auto screenList = hgmCore.GetScreenIds();
    auto screenPos = find(screenList.begin(), screenList.end(), id);
    curScreenId_ = (screenPos == screenList.end()) ? 0 : id;
    HGM_LOGI("HandleScreenPowerStatus curScreen:%{public}d", static_cast<int>(curScreenId_));

    isLtpo_ = (GetScreenType(curScreenId_) == "LTPO");
    std::string curScreenName = "screen" + std::to_string(curScreenId_) + "_" + (isLtpo_ ? "LTPO" : "LTPS");

    auto configData = hgmCore.GetPolicyConfigData();
    if (configData != nullptr) {
        curScreenStrategyId_ = configData->screenStrategyConfigs_[curScreenName];
        if (curScreenStrategyId_.empty()) {
            curScreenStrategyId_ = "LTPO-DEFAULT";
        }
    }

    SyncAppVote();
    hgmCore.SetLtpoConfig();
    FrameRateReport();
    HgmConfigCallbackManager::GetInstance()->SyncHgmConfigChangeCallback();

    // hgm warning: use !isLtpo_ instead after GetDisplaySupportedModes ready
    if (curScreenStrategyId_.find("LTPO") == std::string::npos) {
        DeliverRefreshRateVote(0, "VOTER_LTPO", REMOVE_VOTE);
    }
}

void HgmFrameRateManager::HandleSceneEvent(pid_t pid, EventInfo eventInfo)
{
    std::string sceneName = eventInfo.description;

    std::lock_guard<std::mutex> locker(pkgSceneMutex_);
    std::lock_guard<std::mutex> lock(voteMutex_);
    std::pair<std::string, pid_t> info = std::make_pair(sceneName, pid);
    auto scenePos = find(sceneStack_.begin(), sceneStack_.end(), info);
    if (eventInfo.eventStatus == ADD_VOTE) {
        if (scenePos == sceneStack_.end()) {
            sceneStack_.push_back(info);
            MarkVoteChange();
        }
    } else {
        if (scenePos != sceneStack_.end()) {
            sceneStack_.erase(scenePos);
            MarkVoteChange();
        }
    }
}

void HgmFrameRateManager::HandleVirtualDisplayEvent(pid_t pid, EventInfo eventInfo)
{
    std::string virtualDisplayName = eventInfo.description;
    auto configData = HgmCore::Instance().GetPolicyConfigData();
    if (configData == nullptr || !configData->virtualDisplaySwitch_) {
        // disable vote from virtual display in xml
        return;
    }

    auto virtualDisplayConfig = configData->virtualDisplayConfigs_;
    if (virtualDisplayConfig.count(virtualDisplayName) == 0) {
        HGM_LOGW("HandleVirtualDisplayEvent:unknow virtual display [%{public}s]", virtualDisplayName.c_str());
        DeliverRefreshRateVote(pid, "VOTER_VIRTUALDISPLAY", eventInfo.eventStatus, OLED_60_HZ, OLED_60_HZ);
    } else {
        auto curStrategy = configData->strategyConfigs_[virtualDisplayConfig[virtualDisplayName]];
        DeliverRefreshRateVote(pid, "VOTER_VIRTUALDISPLAY", ADD_VOTE, curStrategy.min, curStrategy.max);
    }
}

void HgmFrameRateManager::SyncAppVote()
{
    auto configData = HgmCore::Instance().GetPolicyConfigData();
    if (configData == nullptr) {
        return;
    }
    auto curScreenSetting =
        configData->screenConfigs_[curScreenStrategyId_][std::to_string(curRefreshRateMode_)];
    std::string curXmlStrategy;
    if (curScreenSetting.appList.count(curPkgName_) == 0) {
        curXmlStrategy = curScreenSetting.strategy;
    } else {
        curXmlStrategy = curScreenSetting.appList[curPkgName_];
    }
    DeliverRefreshRateVote(0, "VOTER_XML", ADD_VOTE,
        configData->strategyConfigs_[curXmlStrategy].min, configData->strategyConfigs_[curXmlStrategy].max);

    isTouchEnable_ = (configData->strategyConfigs_[curXmlStrategy].dynamicMode != 0);
    touchFps_ = configData->strategyConfigs_[curXmlStrategy].max;
    idleFps_ = std::max(configData->strategyConfigs_[curXmlStrategy].min, static_cast<int32_t>(OLED_60_HZ));
}

void HgmFrameRateManager::MarkVoteChange()
{
    isRefreshNeed_ = true;
    if (forceUpdateCallback_ != nullptr) {
        forceUpdateCallback_(false, true);
        touchMgr_->rsIdleUpdateCallback_(false);
    }
}

void HgmFrameRateManager::DeliverRefreshRateVote(pid_t pid, std::string eventName,
    bool eventStatus, uint32_t min, uint32_t max)
{
    RS_TRACE_NAME_FMT("Deliver voter:%s(pid:%d), status:%u, value:[%d-%d]",
        eventName.c_str(), pid, eventStatus, min, max);
    if (min > max) {
        HGM_LOGW("HgmFrameRateManager:invalid vote %{public}s(%{public}d):[%{public}d, %{public}d]",
            eventName.c_str(), pid, min, max);
        return;
    }

    std::lock_guard<std::mutex> lock(voteMutex_);
    auto& vec = voteRecord_[eventName];

    // clear
    if ((pid == 0) && (eventStatus == REMOVE_VOTE)) {
        if (!vec.empty()) {
            vec.clear();
            MarkVoteChange();
        }
        return;
    }

    for (auto it = vec.begin(); it != vec.end(); it++) {
        if ((*it).first != pid) {
            continue;
        }

        if (eventStatus == REMOVE_VOTE) {
            // remove
            it = vec.erase(it);
            MarkVoteChange();
            return;
        } else {
            if ((*it).second.first != min || (*it).second.second != max) {
                // modify
                vec.erase(it);
                vec.push_back(std::make_pair(pid, std::make_pair(min, max)));
                MarkVoteChange();
            }
            return;
        }
    }

    // add
    if (eventStatus == ADD_VOTE) {
        pidRecord_.insert(pid);
        vec.push_back(std::make_pair(pid, std::make_pair(min, max)));
        MarkVoteChange();
    }
}

VoteRange HgmFrameRateManager::ProcessRefreshRateVote(FrameRateVoteInfo& frameRateVoteInfo)
{
    if (!isRefreshNeed_) {
        uint32_t lastPendingRate = HgmCore::Instance().GetPendingScreenRefreshRate();
        RS_TRACE_NAME_FMT("Process nothing, lastRate:[%d]", lastPendingRate);
        return std::make_pair(lastPendingRate, lastPendingRate);
    }
    UpdateVoteRule();
    std::lock_guard<std::mutex> voteNameLock(voteNameMutex_);
    std::lock_guard<std::mutex> voteLock(voteMutex_);

    uint32_t min = OLED_MIN_HZ;
    uint32_t max = OLED_MAX_HZ;

    for (const auto& voter : voters_) {
        auto vec = voteRecord_[voter];
        if (vec.empty()) {
            continue;
        }
        VoteRange info = vec.back().second;
        uint32_t minTemp = info.first;
        uint32_t maxTemp = info.second;

        RS_TRACE_NAME_FMT("Process voter:%s(pid:%d), value:[%d-%d]", voter.c_str(), vec.back().first, minTemp, maxTemp);
        // FORMAT voter(pid):[min，max]
        HGM_LOGI("Process: %{public}s(%{public}d):[%{public}d, %{public}d]",
            voter.c_str(), vec.back().first, minTemp, maxTemp);

        if (minTemp > min) {
            min = minTemp;
            if (min >= max) {
                min = max;
                frameRateVoteInfo.SetVoteInfo(voter, max);
                break;
            }
        }
        if (maxTemp < max) {
            max = maxTemp;
            frameRateVoteInfo.SetVoteInfo(voter, max);
            if (min >= max) {
                max = min;
                frameRateVoteInfo.SetVoteInfo(voter, max);
                break;
            }
        }
        if (min == max) {
            frameRateVoteInfo.SetVoteInfo(voter, max);
            break;
        }
    }

    isRefreshNeed_ = false;
    HGM_LOGI("Process: Strategy:%{public}s Screen:%{public}d Mode:%{public}d -- VoteResult:{%{public}d-%{public}d}",
        curScreenStrategyId_.c_str(), static_cast<int>(curScreenId_), curRefreshRateMode_, min, max);
    return std::make_pair(min, max);
}

void HgmFrameRateManager::UpdateVoteRule()
{
    // dynamic priority for scene
    if (sceneStack_.empty()) {
        // no active scene
        DeliverRefreshRateVote(0, "VOTER_SCENE", REMOVE_VOTE);
        return;
    }
    auto configData = HgmCore::Instance().GetPolicyConfigData();
    if (configData == nullptr) {
        return;
    }
    auto curScreenSceneList =
        configData->screenConfigs_[curScreenStrategyId_][std::to_string(curRefreshRateMode_)].sceneList;
    if (curScreenSceneList.empty()) {
        // no scene configed in cur screen
        return;
    }

    std::string lastScene;
    auto scenePos = sceneStack_.rbegin();
    for (; scenePos != sceneStack_.rend(); scenePos++) {
        lastScene = (*scenePos).first;
        if (curScreenSceneList.count(lastScene) != 0) {
            break;
        }
    }
    if (scenePos == sceneStack_.rend()) {
        // no valid scene
        DeliverRefreshRateVote(0, "VOTER_SCENE", REMOVE_VOTE);
        return;
    }
    auto curSceneConfig = curScreenSceneList[lastScene];
    uint32_t scenePriority = static_cast<uint32_t>(std::stoi(curSceneConfig.priority));
    uint32_t min = static_cast<uint32_t>(configData->strategyConfigs_[curSceneConfig.strategy].min);
    uint32_t max = static_cast<uint32_t>(configData->strategyConfigs_[curSceneConfig.strategy].max);
    HGM_LOGI("UpdateVoteRule: SceneName:%{public}s", lastScene.c_str());
    DeliverRefreshRateVote((*scenePos).second, "VOTER_SCENE", ADD_VOTE, min, max);

    // restore
    std::lock_guard<std::mutex> lock(voteNameMutex_);
    voters_ = std::vector<std::string>(std::begin(VOTER_NAME), std::end(VOTER_NAME));
    std::string srcScene = "VOTER_SCENE";
    std::string dstScene = (scenePriority == SCENE_BEFORE_XML) ? "VOTER_XML" : "VOTER_TOUCH";

    // priority 1: VOTER_SCENE > VOTER_XML
    // priority 2: VOTER_SCENE > VOTER_TOUCH
    // priority 3: VOTER_SCENE < VOTER_TOUCH
    auto srcPos = find(voters_.begin(), voters_.end(), srcScene);
    auto dstPos = find(voters_.begin(), voters_.end(), dstScene);
    
    // resort
    voters_.erase(srcPos);
    if (scenePriority == SCENE_AFTER_TOUCH) {
        voters_.insert(++dstPos, srcScene);
    } else {
        voters_.insert(dstPos, srcScene);
    }
}

std::string HgmFrameRateManager::GetScreenType(ScreenId screenId)
{
    // hgm warning: use GetDisplaySupportedModes instead
    return (screenId == 0) ? "LTPO" : "LTPS";
}

void HgmFrameRateManager::CleanVote(pid_t pid)
{
    if (pidRecord_.count(pid) == 0) {
        return;
    }
    std::lock_guard<std::mutex> lock(voteMutex_);
    HGM_LOGW("CleanVote: i am [%{public}d], i died, clean my votes please.", pid);
    pidRecord_.erase(pid);

    for (auto& [key, value] : voteRecord_) {
        for (auto it = value.begin(); it != value.end(); it++) {
            if ((*it).first == pid) {
                it = value.erase(it);
                break;
            }
        }
    }
}


} // namespace Rosen
} // namespace OHOS