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
#include "pipeline/hardware_thread/rs_hardware_thread.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr int64_t NS_MS_UNIT_CONVERSION = 1000000;
constexpr uint32_t DELAY_TIME_OFFSET = 100;
constexpr int32_t MAX_SETRATE_RETRY_COUNT = 20;
constexpr ScreenId MAX_HAL_DISPLAY_ID = 20;
}

void HgmHardwareUtils::ExecuteSwitchRefreshRate(
    const std::shared_ptr<HdiOutput> output, uint32_t refreshRate)
{
    static bool refreshRateSwitch = system::GetBoolParameter("persist.hgm.refreshrate.enabled", true);
    if (!refreshRateSwitch) {
        RS_LOGD("refreshRateSwitch is off, currRefreshRate is %{public}d", refreshRate);
        return;
    }

    auto& hgmCore = OHOS::Rosen::HgmCore::Instance();
    if (hgmCore.GetFrameRateMgr() == nullptr) {
        RS_LOGD("FrameRateMgr is null");
        return;
    }
    ScreenId id = output->GetScreenId();
    auto& hardwareThread = RSHardwareThread::Instance();
    auto screen = hgmCore.GetScreen(id);
    if (!screen || !screen->GetSelfOwnedScreenFlag()) {
        return;
    }
    auto screenRefreshRateImme = hgmCore.GetScreenRefreshRateImme();
    if (screenRefreshRateImme > 0) {
        RS_LOGD("ExecuteSwitchRefreshRate:rate change: %{public}u -> %{public}u", refreshRate, screenRefreshRateImme);
        refreshRate = screenRefreshRateImme;
    }
    ScreenId curScreenId = hgmCore.GetFrameRateMgr()->GetCurScreenId();
    ScreenId lastCurScreenId = hgmCore.GetFrameRateMgr()->GetLastCurScreenId();
    bool shouldSetRefreshRate = (refreshRate != hgmCore.GetScreenCurrentRefreshRate(id) ||
                                 lastCurScreenId != curScreenId);
    bool needRetrySetRate = false;
    auto retryIter = setRateRetryMap_.find(id);
    if (retryIter != setRateRetryMap_.end()) {
        needRetrySetRate = retryIter->second.first;
    }
    if (shouldSetRefreshRate || needRetrySetRate) {
        RS_LOGD("CommitAndReleaseLayers screenId %{public}d refreshRate %{public}d \
            needRetrySetRate %{public}d", static_cast<int>(id), refreshRate, needRetrySetRate);
        int32_t sceneId = (lastCurScreenId != curScreenId || needRetrySetRate) ? SWITCH_SCREEN_SCENE : 0;
        hgmCore.GetFrameRateMgr()->SetLastCurScreenId(curScreenId);
        int32_t status = hgmCore.SetScreenRefreshRate(id, sceneId, refreshRate, shouldSetRefreshRate);
        if (retryIter != setRateRetryMap_.end()) {
            retryIter->second.first = false;
            retryIter->second.second = shouldSetRefreshRate ? 0 : retryIter->second.second;
        }
        if (status < EXEC_SUCCESS) {
            RS_LOGD("HgmContext: failed to set refreshRate %{public}d, screenId %{public}" PRIu64 "",
                refreshRate, id);
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

void HgmHardwareUtils::PerformSetActiveMode(
    std::shared_ptr<HdiOutput> output, uint64_t timestamp, uint64_t constraintRelativeTime)
{
    auto& hgmCore = OHOS::Rosen::HgmCore::Instance();
    auto screenManager = CreateOrGetScreenManager();
    if (screenManager == nullptr) {
        return;
    }
    vblankIdleCorrector_.ProcessScreenConstraint(timestamp, constraintRelativeTime);
    HgmRefreshRates newRate = RSSystemProperties::GetHgmRefreshRatesEnabled();
    if (hgmRefreshRates_ != newRate) {
        hgmRefreshRates_ = newRate;
        hgmCore.SetScreenRefreshRate(screenManager->GetDefaultScreenId(), 0, static_cast<int32_t>(hgmRefreshRates_));
    }

    std::unique_ptr<std::unordered_map<ScreenId, int32_t>> modeMap(hgmCore.GetModesToApply());
    if (modeMap == nullptr) {
        return;
    }

    RS_TRACE_NAME_FMT("HgmContext::PerformSetActiveMode setting active mode. rate: %d",
        HgmCore::Instance().GetScreenCurrentRefreshRate(HgmCore::Instance().GetActiveScreenId()));
    for (auto mapIter = modeMap->begin(); mapIter != modeMap->end(); ++mapIter) {
        ScreenId id = mapIter->first;
        int32_t modeId = mapIter->second;

        auto supportedModes = screenManager->GetScreenSupportedModes(id);
        for (auto mode : supportedModes) {
            RS_OPTIONAL_TRACE_NAME_FMT(
                "HgmContext check modes w:%" PRId32", h:%" PRId32", rate:%" PRId32", id:%" PRId32"",
                mode.GetScreenWidth(), mode.GetScreenHeight(), mode.GetScreenRefreshRate(), mode.GetScreenModeId());
        }

        uint32_t ret = screenManager->SetScreenActiveMode(id, modeId);
        if (id <= MAX_HAL_DISPLAY_ID) {
            setRateRetryMap_.try_emplace(id, std::make_pair(false, 0));
            UpdateRetrySetRateStatus(id, modeId, ret);
        } else {
            RS_LOGD("UpdateRetrySetRateStatus fail, invalid ScreenId:%{public}" PRIu64, id);
        }

        auto pendingPeriod = hgmCore.GetIdealPeriod(hgmCore.GetScreenCurrentRefreshRate(id));
        int64_t pendingTimestamp = static_cast<int64_t>(timestamp);
        if (auto hdiBackend = HdiBackend::GetInstance(); hdiBackend != nullptr) {
            hdiBackend->SetPendingMode(output, pendingPeriod, pendingTimestamp);
            hdiBackend->StartSample(output);
        }
    }
}

void HgmHardwareUtils::UpdateRefreshRateParam()
{
    // need to sync the hgm data from main thread.
    // Temporary sync the timestamp to fix the duplicate time stamp issue.
    auto& hgmCore = OHOS::Rosen::HgmCore::Instance();
    bool directComposition = hgmCore.GetDirectCompositionFlag();
    RS_LOGI_IF(DEBUG_COMPOSER, "GetRefreshRateData period is %{public}d", directComposition);
    if (directComposition) {
        hgmCore.SetDirectCompositionFlag(false);
    }
    if (directComposition) {
        refreshRateParam_ = {
            .rate = hgmCore.GetPendingScreenRefreshRate(),
            .frameTimestamp = hgmCore.GetCurrentTimestamp(),
            .actualTimestamp = hgmCore.GetActualTimestamp(),
            .vsyncId = hgmCore.GetVsyncId(),
            .constraintRelativeTime = hgmCore.GetPendingConstraintRelativeTime(),
            .isForceRefresh = hgmCore.GetForceRefreshFlag(),
            .fastComposeTimeStampDiff = hgmCore.GetFastComposeTimeStampDiff()
        };
    } else {
        refreshRateParam_ = {
            .rate = RSUniRenderThread::Instance().GetPendingScreenRefreshRate(),
            .frameTimestamp = RSUniRenderThread::Instance().GetCurrentTimestamp(),
            .actualTimestamp = RSUniRenderThread::Instance().GetActualTimestamp(),
            .vsyncId = RSUniRenderThread::Instance().GetVsyncId(),
            .constraintRelativeTime = RSUniRenderThread::Instance().GetPendingConstraintRelativeTime(),
            .isForceRefresh = RSUniRenderThread::Instance().GetForceRefreshFlag(),
            .fastComposeTimeStampDiff = RSUniRenderThread::Instance().GetFastComposeTimeStampDiff()
        };
    }
}
} // namespace Rosen
} // namespace OHOS