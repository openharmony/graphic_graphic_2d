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

void HgmVSyncGeneratorController::ChangeGeneratorRate(const uint32_t controllerRate,
    const std::vector<std::pair<FrameRateLinkerId, uint32_t>>& appData)
{
    int32_t pulseNum = GetAppOffset(controllerRate);

    VSyncGenerator::ListenerRefreshRateData listenerRate = {.cb = appController_, .refreshRates = appData};
    VSyncGenerator::ListenerPhaseOffsetData listenerPhase;

    if (currentRate_ != controllerRate) {
        HGM_LOGI("HgmVSyncGeneratorController::ChangeGeneratorRate controllerRate = %{public}d, appOffset = %{public}d",
            controllerRate, pulseNum);
        RS_TRACE_NAME("HgmVSyncGeneratorController::ChangeGeneratorRate controllerRate: " +
            std::to_string(controllerRate) + ", appOffset: " + std::to_string(pulseNum));
        listenerPhase.cb = appController_;
        listenerPhase.phaseByPulseNum = pulseNum;
        vsyncGenerator_->ChangeGeneratorRefreshRateModel(listenerRate, listenerPhase, controllerRate);
        currentOffset_ = vsyncGenerator_->GetVSyncPulse() * pulseNum;
        currentRate_ = controllerRate;
    } else {
        vsyncGenerator_->ChangeGeneratorRefreshRateModel(listenerRate, listenerPhase, controllerRate);
    }
}

int64_t HgmVSyncGeneratorController::GetCurrentOffset() const
{
    return currentOffset_;
}

uint32_t HgmVSyncGeneratorController::GetCurrentRate() const
{
    return currentRate_;
}
} // namespace Rosen
} // namespace OHOS