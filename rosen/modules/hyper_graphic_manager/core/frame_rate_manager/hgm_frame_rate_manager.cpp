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

#include "common/rs_optional_trace.h"
#include "common/rs_thread_handler.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "hgm_core.h"
#include "hgm_log.h"
#include "parameters.h"
#include "rs_trace.h"
#include "sandbox_utils.h"
#include "frame_rate_report.h"


namespace OHOS {
namespace Rosen {
namespace {
    constexpr float MARGIN = 0.00001;
    constexpr float MIN_DRAWING_DIVISOR = 10.0f;
    constexpr float DIVISOR_TWO = 2.0f;
    constexpr int32_t DEFAULT_PREFERRED = 60;
    constexpr int32_t IDLE_TIMER_EXPIRED = 200; // ms
    constexpr float ONE_MS_IN_NANO = 1000000.0f;
    constexpr uint32_t UNI_RENDER_VSYNC_OFFSET = 5000000; // ns
}

void HgmFrameRateManager::Init(sptr<VSyncController> rsController,
    sptr<VSyncController> appController, sptr<VSyncGenerator> vsyncGenerator)
{
    UpdateVSyncMode(rsController, appController);
    controller_ =
        std::make_shared<HgmVSyncGeneratorController>(rsController, appController, vsyncGenerator);
}

void HgmFrameRateManager::UpdateVSyncMode(sptr<VSyncController> rsController, sptr<VSyncController> appController)
{
    HgmCore::Instance().RegisterRefreshRateModeChangeCallback([rsController, appController](int32_t mode) {
        if (mode == HGM_REFRESHRATE_MODE_AUTO) {
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
}

void HgmFrameRateManager::UniProcessData(ScreenId screenId, uint64_t timestamp,
                                         std::shared_ptr<RSRenderFrameRateLinker> rsFrameRateLinker,
                                         const FrameRateLinkerMap& appFrameRateLinkers, bool forceUpdateFlag)
{
    if (screenId == INVALID_SCREEN_ID) {
        return;
    }

    auto& hgmCore = HgmCore::Instance();
    FrameRateRange finalRange;
    if (auto scenePreferred = hgmCore.GetScenePreferred(); scenePreferred != 0) {
        StopScreenTimer(screenId);
        finalRange.max_ = RANGE_MAX_REFRESHRATE;
        finalRange.preferred_ = scenePreferred;
    } else {
        finalRange = rsFrameRateLinker->GetExpectedRange();
        for (auto linker : appFrameRateLinkers) {
            finalRange.Merge(linker.second->GetExpectedRange());
        }

        if (!finalRange.IsValid()) {
            if (forceUpdateFlag) {
                finalRange.max_ = RANGE_MAX_REFRESHRATE;
                finalRange.preferred_ = DEFAULT_PREFERRED;
            } else {
                StartScreenTimer(screenId, IDLE_TIMER_EXPIRED, nullptr, expiredCallback_);
                return;
            }
        } else {
            ResetScreenTimer(screenId);
        }
    }
    Reset();
    CalcRefreshRate(screenId, finalRange);

    bool frameRateChanged = CollectFrameRateChange(finalRange, rsFrameRateLinker, appFrameRateLinkers);
    if (!hgmCore.GetLtpoEnabled()) {
        pendingRefreshRate_ = std::make_shared<uint32_t>(currRefreshRate_);
    } else if (frameRateChanged) {
        HandleFrameRateChangeForLTPO(timestamp);
        std::unordered_map<pid_t, uint32_t> rates;
        rates[GetRealPid()] = currRefreshRate_;
        if (auto alignRate = hgmCore.GetAlignRate(); alignRate != 0) {
            rates[UNI_APP_PID] = alignRate;
        }
        FRAME_TRACE::FrameRateReport::GetInstance().SendFrameRates(rates);
    }
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
    RS_TRACE_NAME_FMT("HgmFrameRateManager::UniProcessData refreshRate: %d, rsFrameRate: %d, finalRange = (%d, %d, %d)",
        currRefreshRate_, rsFrameRate, finalRange.min_, finalRange.max_, finalRange.preferred_);
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

void HgmFrameRateManager::HandleFrameRateChangeForLTPO(uint64_t timestamp)
{
    RSTaskMessage::RSTask task = [this]() {
        controller_->ChangeGeneratorRate(controllerRate_, appChangeData_);
        pendingRefreshRate_ = std::make_shared<uint32_t>(currRefreshRate_);
    };

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

int32_t HgmFrameRateManager::CalModifierPreferred(const HgmModifierProfile &hgmModifierProfile)
{
    auto& hgmCore = HgmCore::Instance();
    sptr<HgmScreen> hgmScreen = hgmCore.GetScreen(hgmCore.GetActiveScreenId());
    auto parsedConfigData = hgmCore.GetParsedConfigData();
    if (!hgmScreen) {
        return HGM_ERROR;
    }
    auto [xSpeed, ySpeed] = applyDimension(
        SpeedTransType::TRANS_PIXEL_TO_MM, hgmModifierProfile.xSpeed, hgmModifierProfile.ySpeed, hgmScreen);
    auto mixSpeed = sqrt(xSpeed * xSpeed + ySpeed * ySpeed);

    auto dynamicSettingMap = parsedConfigData->GetAnimationDynamicSettingMap(hgmModifierProfile.hgmModifierType);
    for (const auto &iter: dynamicSettingMap) {
        if (mixSpeed >= iter.second.min && (mixSpeed < iter.second.max || iter.second.max == -1)) {
            return iter.second.preferred_fps;
        }
    }
    return HGM_ERROR;
}

std::pair<float, float> HgmFrameRateManager::applyDimension(
    SpeedTransType speedTransType, float xSpeed, float ySpeed, sptr<HgmScreen> hgmScreen)
{
    auto xDpi = hgmScreen->GetXDpi();
    auto yDpi = hgmScreen->GetYDpi();
    if (xDpi < MARGIN || yDpi < MARGIN) {
        return std::pair<float, float>(0, 0);
    }
    switch (speedTransType) {
        case SpeedTransType::TRANS_MM_TO_PIXEL:
            return std::pair<float, float>(
                xSpeed * xDpi / INCH_2_MM, ySpeed * yDpi / INCH_2_MM);
        case SpeedTransType::TRANS_PIXEL_TO_MM:
            return std::pair<float, float>(
                xSpeed / xDpi * INCH_2_MM, ySpeed / yDpi * INCH_2_MM);
        default:
            return std::pair<float, float>(0, 0);
    }
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
} // namespace Rosen
} // namespace OHOS
