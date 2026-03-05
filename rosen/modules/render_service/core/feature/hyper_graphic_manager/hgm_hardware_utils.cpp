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

#include "feature/hyper_graphic_manager/hgm_hardware_utils.h"

#include "common/rs_optional_trace.h"
#include "parameters.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "platform/common/rs_hisysevent.h"
#include "screen_manager/rs_screen_manager.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr int64_t NS_MS_UNIT_CONVERSION = 1000000;
constexpr uint32_t DELAY_TIME_OFFSET = 100;
constexpr int32_t MAX_SETRATE_RETRY_COUNT = 20;
constexpr ScreenId MAX_HAL_DISPLAY_ID = 20;
}

void HgmHardwareUtils::ExecuteSwitchRefreshRate(ScreenId screenId)
{
    uint32_t refreshRate = refreshRateParam_.rate;
    static bool refreshRateSwitch = system::GetBoolParameter("persist.hgm.refreshrate.enabled", true);
    if (!refreshRateSwitch) {
        HGM_LOGD("refreshRateSwitch is off, currRefreshRate is %{public}u", refreshRate);
        return;
    }

    auto frameRateMgr = hgmCore_.GetFrameRateMgr();
    if (frameRateMgr == nullptr) {
        HGM_LOGD("FrameRateMgr is null");
        return;
    }
    auto screenRefreshRateImme = hgmCore_.GetScreenRefreshRateImme();
    if (screenRefreshRateImme > 0) {
        HGM_LOGD("rate change: %{public}u -> %{public}u", refreshRate, screenRefreshRateImme);
        refreshRate = screenRefreshRateImme;
    }
    ScreenId curScreenId = frameRateMgr->GetCurScreenId();
    ScreenId lastCurScreenId = frameRateMgr->GetLastCurScreenId();
    bool shouldSetRefreshRate = (refreshRate != hgmCore_.GetScreenCurrentRefreshRate(screenId) ||
                                 lastCurScreenId != curScreenId);
    bool needRetrySetRate = false;
    auto retryIter = setRateRetryMap_.find(screenId);
    if (retryIter != setRateRetryMap_.end()) {
        needRetrySetRate = retryIter->second.first;
    }
    if (shouldSetRefreshRate || setRateRetryParam_.needRetrySetRate) {
        HGM_LOGD("screenId %{public}" PRIu64 " refreshRate %{public}u needRetrySetRate %{public}d",
            screenId, refreshRate, setRateRetryParam_.needRetrySetRate);
        int32_t sceneId = (lastCurScreenId != curScreenId || setRateRetryParam_.needRetrySetRate) ?
            SWITCH_SCREEN_SCENE : 0;
        frameRateMgr->SetLastCurScreenId(curScreenId);
        int32_t status = hgmCore_.SetScreenRefreshRate(screenId, sceneId, refreshRate, shouldSetRefreshRate);
        setRateRetryParam_.needRetrySetRate = false;
        setRateRetryParam_.retryCount = shouldSetRefreshRate ? 0 : setRateRetryParam_.retryCount;
        if (status < EXEC_SUCCESS) {
            HGM_LOGD("failed to set refreshRate %{public}u, screenId %{public}" PRIu64, refreshRate, screenId);
        }
    }
}

void HgmHardwareUtils::UpdateRetrySetRateStatus(ScreenId id, int32_t modeId, uint32_t setRateRet)
{
    setRateRetryParam_.needRetrySetRate = (setRateRet == static_cast<uint32_t>(StatusCode::SET_RATE_ERROR));
    if (!setRateRetryParam_.needRetrySetRate) {
        setRateRetryParam_.retryCount = 0;
    } else if (setRateRetryParam_.retryCount < MAX_SETRATE_RETRY_COUNT) {
        setRateRetryParam_.retryCount++;
    } else {
        HGM_LOGW("skip retrying for ScreenId:%{public}" PRIu64 ", set rate failed more than %{public}d",
            id, MAX_SETRATE_RETRY_COUNT);
        setRateRetryParam_.needRetrySetRate = false;
        setRateRetryParam_.isRetryOverLimit = true;
        ReportRetryOverLimit(0, refreshRateParam_.rate);
    }
    RS_LOGD_IF(setRateRetryParam_.needRetrySetRate,
        "need retry set modeId %{public}" PRId32 ", ScreenId:%{public}" PRIu64, modeId, id);
}

void HgmHardwareUtils::PerformSetActiveMode(const std::shared_ptr<HdiOutput>& output, RSScreenManager* screenManager)
{
    uint64_t timestamp = refreshRateParam_.frameTimestamp;
    uint64_t constraintRelativeTime = refreshRateParam_.constraintRelativeTime;
    vblankIdleCorrector_.ProcessScreenConstraint(output->GetScreenId(), timestamp, constraintRelativeTime);
    HgmRefreshRates newRate = RSSystemProperties::GetHgmRefreshRatesEnabled();
    if (hgmRefreshRates_ != newRate) {
        hgmRefreshRates_ = newRate;
        hgmCore_.SetScreenRefreshRate(screenManager->GetDefaultScreenId(), 0, static_cast<int32_t>(hgmRefreshRates_));
    }

    std::unique_ptr<std::unordered_map<ScreenId, int32_t>> modeMap(hgmCore_.GetModesToApply());
    if (modeMap == nullptr) {
        return;
    }

    RS_TRACE_NAME_FMT("%s: setting active mode. rate: %u",
        __func__, hgmCore_.GetScreenCurrentRefreshRate(hgmCore_.GetActiveScreenId()));
    for (const auto [screenId, modeId] : *modeMap) {
        for (const auto& mode : screenManager->GetScreenSupportedModes(screenId)) {
            RS_OPTIONAL_TRACE_NAME_FMT(
                "%s: check modes w:%" PRId32 ", h:%" PRId32 ", rate:%" PRIu32 ", id:%" PRId32,
                __func__, mode.GetScreenWidth(), mode.GetScreenHeight(), mode.GetScreenRefreshRate(),
                mode.GetScreenModeId());
        }

        uint32_t ret = screenManager->SetScreenActiveMode(screenId, modeId);
        if (screenId <= MAX_HAL_DISPLAY_ID) {
            UpdateRetrySetRateStatus(screenId, modeId, ret);
        } else {
            HGM_LOGD("UpdateRetrySetRateStatus fail, invalid ScreenId:%{public}" PRIu64, screenId);
        }

        auto pendingPeriod = hgmCore_.GetIdealPeriod(hgmCore_.GetScreenCurrentRefreshRate(screenId));
        int64_t pendingTimestamp = static_cast<int64_t>(timestamp);
        if (auto hdiBackend = HdiBackend::GetInstance(); hdiBackend != nullptr) {
            hdiBackend->SetPendingMode(output, pendingPeriod, pendingTimestamp);
            hdiBackend->StartSample(output);
        }
    }
}

void HgmHardwareUtils::ResetRetryCount(ScreenPowerStatus status)
{
    HGM_LOGD("PowerStatus change to %{public}d", status);
    if (status == POWER_STATUS_ON && setRateRetryParam_.isRetryOverLimit) {
        HGM_LOGI(
            "when ScreenPower On and last refresh rate failed more than %{public}d", setRateRetryParam_.retryCount);
        setRateRetryParam_.needRetrySetRate = true;
        setRateRetryParam_.retryCount = 0;
        setRateRetryParam_.isRetryOverLimit = false;
    }
}

void HgmHardwareUtils::ReportRetryOverLimit(uint64_t vsyncId, uint32_t rate)
{
    if (auto frameRateMgr = HgmCore::Instance().GetFrameRateMgr()) {
        HgmTaskHandleThread::Instance().PostTask([frameRateMgr, vsyncId, rate]() {
            auto voteInfo = frameRateMgr->GetLastVoteInfo().ToSimpleString();
            RSHiSysEvent::EventWrite(RSEventName::HGM_RETRY_UPDATE_RATE_OVERLIMIT, RSEventType::RS_FAULT,
                "VSYNCID", vsyncId, "REFRESHRATE", rate, "LASTVOTEINFO", voteInfo);
        });
    }
}

void HgmHardwareUtils::TransactRefreshRateParam(uint32_t& currentRate,
    uint32_t pendingScreenRefreshRate, uint64_t frameTimestamp, uint64_t pendingConstraintRelativeTime)
{
    RS_TRACE_NAME_FMT("%s: rate:%u", __func__, pendingScreenRefreshRate);
    refreshRateParam_ = {
        .rate = pendingScreenRefreshRate,
        .frameTimestamp = frameTimestamp,
        .constraintRelativeTime = pendingConstraintRelativeTime,
    };
    currentRate = hgmCore_.GetScreenCurrentRefreshRate(hgmCore_.GetActiveScreenId());
}

void HgmHardwareUtils::SwitchRefreshRate(const std::shared_ptr<HdiOutput>& hdiOutput)
{
    RS_TRACE_NAME_FMT("%s: rate:%u", __func__, refreshRateParam_.rate);
    if (refreshRateParam_.rate == 0) {
        return;
    }
    ScreenId screenId = hdiOutput->GetScreenId();
    if (auto screen = hgmCore_.GetScreen(screenId); !screen || !screen->GetSelfOwnedScreenFlag()) {
        return;
    }

    RSScreenManager* screenManager = hgmCore_.GetScreenManager();
    if (screenManager == nullptr) {
        HGM_LOGE("screenManager is nullptr");
        return;
    }

    if (RSSystemProperties::IsFoldDeviceOfOldDss()) {
        if (auto powerStatus = screenManager->GetScreenPowerStatus(screenId);
            powerStatus == ScreenPowerStatus::POWER_STATUS_SUSPEND ||
            powerStatus == ScreenPowerStatus::POWER_STATUS_OFF) {
            return;
        }
    }

    ExecuteSwitchRefreshRate(screenId);
    PerformSetActiveMode(hdiOutput, screenManager);
    AddRefreshRateCount(screenId);
}

void HgmHardwareUtils::AddRefreshRateCount(ScreenId screenId)
{
    uint32_t currentRefreshRate = hgmCore_.GetScreenCurrentRefreshRate(screenId);
    auto [iter, success] = refreshRateCounts_.try_emplace(currentRefreshRate, 1);
    if (!success) {
        iter->second++;
    }
    if (auto frameRateMgr = hgmCore_.GetFrameRateMgr()) {
        frameRateMgr->HandleRsFrame();
    }
}

void HgmHardwareUtils::RefreshRateCounts(std::string& dumpString)
{
    if (refreshRateCounts_.empty()) {
        HGM_LOGI("refreshRateCounts is empty");
        return;
    }
    for (const auto& countInfo : refreshRateCounts_) {
        dumpString.append(
            "Refresh Rate:" + std::to_string(countInfo.first) + ", Count:" + std::to_string(countInfo.second) + ";\n");
    }
}

void HgmHardwareUtils::ClearRefreshRateCounts(std::string& dumpString)
{
    if (refreshRateCounts_.empty()) {
        HGM_LOGI("refreshRateCounts is empty");
        return;
    }
    refreshRateCounts_.clear();
    dumpString.append("The refresh rate counts info is cleared successfully!\n");
}
} // namespace Rosen
} // namespace OHOS