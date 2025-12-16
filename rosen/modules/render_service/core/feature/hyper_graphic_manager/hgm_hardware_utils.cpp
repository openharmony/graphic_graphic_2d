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
        RS_LOGD("refreshRateSwitch is off, currRefreshRate is %{public}d", refreshRate);
        return;
    }

    auto frameRateMgr = hgmCore_.GetFrameRateMgr();
    if (frameRateMgr == nullptr) {
        RS_LOGD("FrameRateMgr is null");
        return;
    }
    auto screenRefreshRateImme = hgmCore_.GetScreenRefreshRateImme();
    if (screenRefreshRateImme > 0) {
        RS_LOGD("ExecuteSwitchRefreshRate:rate change: %{public}u -> %{public}u", refreshRate, screenRefreshRateImme);
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
    if (shouldSetRefreshRate || needRetrySetRate) {
        RS_LOGD("CommitAndReleaseLayers screenId %{public}d refreshRate %{public}d \
            needRetrySetRate %{public}d", static_cast<int>(screenId), refreshRate, needRetrySetRate);
        int32_t sceneId = (lastCurScreenId != curScreenId || needRetrySetRate) ? SWITCH_SCREEN_SCENE : 0;
        frameRateMgr->SetLastCurScreenId(curScreenId);
        int32_t status = hgmCore_.SetScreenRefreshRate(screenId, sceneId, refreshRate, shouldSetRefreshRate);
        if (retryIter != setRateRetryMap_.end()) {
            retryIter->second.first = false;
            retryIter->second.second = shouldSetRefreshRate ? 0 : retryIter->second.second;
        }
        if (status < EXEC_SUCCESS) {
            RS_LOGD("HgmContext: failed to set refreshRate %{public}d, screenId %{public}" PRIu64 "",
                refreshRate, screenId);
        }
    }
}

void HgmHardwareUtils::UpdateRetrySetRateStatus(ScreenId id, int32_t modeId, uint32_t setRateRet)
{
    if (auto retryIter = setRateRetryMap_.find(id); retryIter != setRateRetryMap_.end()) {
        auto& [needRetrySetRate, setRateRetryCount] = retryIter->second;
        needRetrySetRate = (setRateRet == static_cast<uint32_t>(StatusCode::SET_RATE_ERROR));
        if (!needRetrySetRate) {
            setRateRetryCount = 0;
        } else if (setRateRetryCount < MAX_SETRATE_RETRY_COUNT) {
            setRateRetryCount++;
        } else {
            RS_LOGW("skip retrying for ScreenId:%{public}" PRIu64 ", set refresh rate failed more than %{public}d",
                id, MAX_SETRATE_RETRY_COUNT);
            needRetrySetRate = false;
        }
        RS_LOGD_IF(needRetrySetRate,
            "RSHardwareThread: need retry set modeId %{public}" PRId32 ", ScreenId:%{public}" PRIu64, modeId, id);
    }
}

void HgmHardwareUtils::PerformSetActiveMode(const std::shared_ptr<HdiOutput>& output)
{
    RSScreenManager* scmFromHgm = hgmCore_.GetScreenManager();
    if (scmFromHgm == nullptr) {
        RS_LOGE("%{public}s, scmFromHgm == nullptr", __func__);
        return;
    }
    uint64_t timestamp = refreshRateParam_.frameTimestamp;
    uint64_t constraintRelativeTime = refreshRateParam_.constraintRelativeTime;
    vblankIdleCorrector_.ProcessScreenConstraint(output->GetScreenId(), timestamp, constraintRelativeTime);
    HgmRefreshRates newRate = RSSystemProperties::GetHgmRefreshRatesEnabled();
    if (hgmRefreshRates_ != newRate) {
        hgmRefreshRates_ = newRate;
        hgmCore_.SetScreenRefreshRate(scmFromHgm->GetDefaultScreenId(), 0, static_cast<int32_t>(hgmRefreshRates_));
    }

    std::unique_ptr<std::unordered_map<ScreenId, int32_t>> modeMap(hgmCore_.GetModesToApply());
    if (modeMap == nullptr) {
        return;
    }

    RS_TRACE_NAME_FMT("HgmContext::PerformSetActiveMode setting active mode. rate: %d",
        hgmCore_.GetScreenCurrentRefreshRate(hgmCore_.GetActiveScreenId()));
    for (auto [screenId, modeId] : *modeMap) {
        for (auto mode : scmFromHgm->GetScreenSupportedModes(screenId)) {
            RS_OPTIONAL_TRACE_NAME_FMT(
                "HgmContext check modes w:%" PRId32 ", h:%" PRId32 ", rate:%" PRIu32 ", id:%" PRId32,
                mode.GetScreenWidth(), mode.GetScreenHeight(), mode.GetScreenRefreshRate(), mode.GetScreenModeId());
        }

        uint32_t ret = scmFromHgm->SetScreenActiveMode(screenId, modeId);
        if (screenId <= MAX_HAL_DISPLAY_ID) {
            setRateRetryMap_.try_emplace(screenId, std::make_pair(false, 0));
            UpdateRetrySetRateStatus(screenId, modeId, ret);
        } else {
            RS_LOGD("UpdateRetrySetRateStatus fail, invalid ScreenId:%{public}" PRIu64, screenId);
        }

        auto pendingPeriod = hgmCore_.GetIdealPeriod(hgmCore_.GetScreenCurrentRefreshRate(screenId));
        int64_t pendingTimestamp = static_cast<int64_t>(timestamp);
        if (auto hdiBackend = HdiBackend::GetInstance(); hdiBackend != nullptr) {
            hdiBackend->SetPendingMode(output, pendingPeriod, pendingTimestamp);
            hdiBackend->StartSample(output);
        }
    }
}

void HgmHardwareUtils::UpdateRefreshRateParam(uint32_t pendingScreenRefreshRate,
    uint64_t frameTimestamp, uint64_t pendingConstraintRelativeTime)
{
    refreshRateParam_ = {
        .rate = pendingScreenRefreshRate,
        .frameTimestamp = frameTimestamp,
        .constraintRelativeTime = pendingConstraintRelativeTime,
    };
}

void HgmHardwareUtils::TransactRefreshRateParam(uint32_t& currentRate,
    uint32_t pendingScreenRefreshRate, uint64_t frameTimestamp, uint64_t pendingConstraintRelativeTime)
{
    RS_TRACE_NAME_FMT("%s", __func__);
    UpdateRefreshRateParam(pendingScreenRefreshRate, frameTimestamp, pendingConstraintRelativeTime);
    currentRate = hgmCore_.GetScreenCurrentRefreshRate(hgmCore_.GetActiveScreenId());
}

void HgmHardwareUtils::SwitchRefreshRate(const std::shared_ptr<HdiOutput>& hdiOutput)
{
    RS_TRACE_NAME_FMT("%s rate:%u", __func__, refreshRateParam_.rate);
    ScreenId screenId = hdiOutput->GetScreenId();
    auto screen = hgmCore_.GetScreen(screenId);
    if (!screen || !screen->GetSelfOwnedScreenFlag() || refreshRateParam_.rate == 0) {
        return;
    }

    ExecuteSwitchRefreshRate(screenId);
    PerformSetActiveMode(hdiOutput);
    AddRefreshRateCount(screenId);
}

void HgmHardwareUtils::AddRefreshRateCount(ScreenId screenId)
{
    uint32_t currentRefreshRate = hgmCore_.GetScreenCurrentRefreshRate(screenId);
    auto [iter, success] = refreshRateCounts_.try_emplace(currentRefreshRate, 1);
    if (!success) {
        iter->second++;
    }
    auto frameRateMgr = hgmCore_.GetFrameRateMgr();
    if (frameRateMgr == nullptr) {
        return;
    }
    frameRateMgr->HandleRsFrame();
}

void HgmHardwareUtils::RefreshRateCounts(std::string& dumpString)
{
    if (refreshRateCounts_.empty()) {
        RS_LOGE("RefreshData fail, refreshRateCounts_ is empty");
        return;
    }
    std::map<uint32_t, uint64_t>::iterator iter;
    for (iter = refreshRateCounts_.begin(); iter != refreshRateCounts_.end(); ++iter) {
        dumpString.append(
            "Refresh Rate:" + std::to_string(iter->first) + ", Count:" + std::to_string(iter->second) + ";\n");
    }
    RS_LOGD("RefreshRateCounts refresh rate counts info is displayed");
}

void HgmHardwareUtils::ClearRefreshRateCounts(std::string& dumpString)
{
    if (refreshRateCounts_.empty()) {
        RS_LOGE("ClearRefreshData fail, refreshRateCounts_ is empty");
        return;
    }
    refreshRateCounts_.clear();
    dumpString.append("The refresh rate counts info is cleared successfully!\n");
    RS_LOGD("RefreshRateCounts refresh rate counts info is cleared");
}
} // namespace Rosen
} // namespace OHOS