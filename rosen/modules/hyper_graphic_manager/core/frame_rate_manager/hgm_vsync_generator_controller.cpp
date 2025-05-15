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

#include "hgm_vsync_generator_controller.h"
#include <cstdint>

#include "hgm_core.h"
#include "hgm_log.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr uint32_t TARGET_TIME_TRIGGER_PULSE_NUM = 6;
}

HgmVSyncGeneratorController::HgmVSyncGeneratorController(sptr<VSyncController> rsController,
    sptr<VSyncController> appController, sptr<VSyncGenerator> vsyncGenerator)
    : rsController_(rsController),
    appController_(appController),
    vsyncGenerator_(vsyncGenerator)
{
}

HgmVSyncGeneratorController::~HgmVSyncGeneratorController() {}

int32_t HgmVSyncGeneratorController::GetAppOffset(const uint32_t controllerRate)
{
    auto alignRate = HgmCore::Instance().GetAlignRate();
    auto supportedMaxTE = HgmCore::Instance().GetSupportedMaxTE();
    if (alignRate == 0 || controllerRate == 0) {
        HGM_LOGE("HgmVSyncGeneratorController::GetAppOffset illegal alignRate or controllerRate");
        return 0;
    }
    if (alignRate < controllerRate) {
        return 0;
    }
    return static_cast<int32_t>(supportedMaxTE / controllerRate - supportedMaxTE / alignRate);
}

uint64_t HgmVSyncGeneratorController::CalcVSyncQuickTriggerTime(uint64_t lastVSyncTime, uint32_t lastRate)
{
    if (lastRate == 0) {
        HGM_LOGE("HgmVSyncGeneratorController::CalcVSyncQuickTriggerTime illegal param");
        return 0;
    }
    if (vsyncGenerator_ == nullptr) {
        HGM_LOGE("HgmVSyncGeneratorController::vsyncGenerator is nullptr");
        return 0;
    }
    uint32_t maxPluseNum = HgmCore::Instance().GetSupportedMaxTE() / lastRate;

    uint64_t pulse = vsyncGenerator_->GetVSyncPulse() >= 0 ?
        static_cast<uint64_t>(vsyncGenerator_->GetVSyncPulse()) : 0;
    uint64_t targetTime = lastVSyncTime + pulse * TARGET_TIME_TRIGGER_PULSE_NUM;
    uint64_t currTime = static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count()) + pulse;
    targetTime = targetTime > currTime ? targetTime : currTime;
    uint64_t threshodTime = lastVSyncTime + pulse * (maxPluseNum - 1); // threshod value
    if (targetTime > threshodTime) {
        return 0;
    }
    return targetTime;
}

int64_t HgmVSyncGeneratorController::ChangeGeneratorRate(const uint32_t controllerRate,
    const std::vector<std::pair<FrameRateLinkerId, uint32_t>>& appData, uint64_t targetTime, bool isNeedUpdateAppOffset)
{
    int64_t vsyncCount = 0;
    if (vsyncGenerator_ == nullptr) {
        HGM_LOGE("HgmVSyncGeneratorController::vsyncGenerator is nullptr");
        return vsyncCount;
    }
    if (auto configPluseNum = HgmCore::Instance().GetPluseNum(); configPluseNum != -1) {
        pulseNum_ = configPluseNum;
    } else {
        if (isNeedUpdateAppOffset && CheckNeedUpdateAppOffsetRefreshRate(controllerRate)) {
            pulseNum_ = 0;
        } else {
            pulseNum_ = GetAppOffset(controllerRate);
        }
    }

    VSyncGenerator::ListenerRefreshRateData listenerRate = {.cb = appController_, .refreshRates = appData};
    VSyncGenerator::ListenerPhaseOffsetData listenerPhase;

    if (currentRate_ != controllerRate) {
        HGM_LOGD("HgmVSyncGeneratorController::ChangeGeneratorRate controllerRate = %{public}d, appOffset = %{public}d",
            controllerRate, pulseNum_);
        RS_TRACE_NAME("HgmVSyncGeneratorController::ChangeGeneratorRate controllerRate: " +
            std::to_string(controllerRate) + ", appOffset: " + std::to_string(pulseNum_) +
            ", nextVSyncTargetTime =" + std::to_string(targetTime));
        listenerPhase.cb = appController_;
        listenerPhase.phaseByPulseNum = pulseNum_;
        vsyncGenerator_->ChangeGeneratorRefreshRateModel(
            listenerRate, listenerPhase, controllerRate, vsyncCount, targetTime);
        currentOffset_ = vsyncGenerator_->GetVSyncPulse() * pulseNum_;
        currentRate_ = controllerRate;
    } else {
        if (isNeedUpdateAppOffset) {
            listenerPhase.cb = appController_;
            listenerPhase.phaseByPulseNum = pulseNum_;
        }
        vsyncGenerator_->ChangeGeneratorRefreshRateModel(listenerRate, listenerPhase, controllerRate, vsyncCount);
    }
    return vsyncCount;
}
bool HgmVSyncGeneratorController::CheckNeedUpdateAppOffsetRefreshRate(uint32_t refreshRate)
{
    return refreshRate <= OLED_60_HZ;
}
} // namespace Rosen
} // namespace OHOS
