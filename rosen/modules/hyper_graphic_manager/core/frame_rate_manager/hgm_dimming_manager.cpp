/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "hgm_dimming_manager.h"

#include "hgm_core.h"
#include "hgm_frame_rate_manager.h"
#include "hgm_task_handle_thread.h"

namespace OHOS::Rosen {
namespace {
const std::string S_DIMMING_UP_TIMEOUT_MS = "dimming_up_timeout_ms";
const std::string S_DIMMING_DOWN_TIMEOUT_MS = "dimming_down_timeout_ms";
}
HgmDimmingManager::HgmDimmingManager() {}

void HgmDimmingManager::SetRefreshRateVec(std::vector<uint32_t> vec)
{
    refreshRateVec_ = std::move(vec);
    std::sort(refreshRateVec_.begin(), refreshRateVec_.end());
}

void HgmDimmingManager::SetDimmingTimeoutConfig(const std::shared_ptr<PolicyConfigData>& configData)
{
    if (configData == nullptr || configData->dimmingConfig_.empty()) {
        return;
    }
    if (auto iter = configData->dimmingConfig_.find(S_DIMMING_UP_TIMEOUT_MS);
        iter != configData->dimmingConfig_.end() && XMLParser::IsNumber(iter->second)) {
        dimmingUpTimeoutMs_ = static_cast<uint32_t>(std::stoi(iter->second));
    }
    if (auto iter = configData->dimmingConfig_.find(S_DIMMING_DOWN_TIMEOUT_MS);
        iter != configData->dimmingConfig_.end() && XMLParser::IsNumber(iter->second)) {
        dimmingDownTimeoutMs_ = static_cast<uint32_t>(std::stoi(iter->second));
    }
}

void HgmDimmingManager::RegisterDimmingEventCallback(DimmingEventCallback callback)
{
    dimmingEventCallback_ = std::move(callback);
}

uint32_t HgmDimmingManager::CalcDimmingRefreshRate(uint32_t voteRefreshRate)
{
    if (dimmingUpTimeoutMs_ == 0 && dimmingDownTimeoutMs_ == 0) {
        return voteRefreshRate;
    }
    auto currTime = std::chrono::steady_clock::now();
    if (currRefreshRate_ == voteRefreshRate) {
        if (dimmingStatus_ != DimmingStatus::NOT_DIMMING && currTime >= dimmingEndTime_) {
            dimmingStatus_ = DimmingStatus::NOT_DIMMING;
            HgmTaskHandleThread::Instance().RemoveEvent("DimmingTask");
            HGM_LOGD("Dimming end, refresh rate: %{public}d", currRefreshRate_);
            RS_TRACE_NAME_FMT("%s: Dimming end, refresh rate: %d", __func__, currRefreshRate_);
        }
        return voteRefreshRate;
    }
    uint32_t dimmingTimeoutMs = currRefreshRate_ < voteRefreshRate ? dimmingUpTimeoutMs_ : dimmingDownTimeoutMs_;
    if (dimmingTimeoutMs == 0 || lightFactorStatus_ == LightFactorStatus::NORMAL_HIGH ||
        lightFactorStatus_ == LightFactorStatus::HIGH_LEVEL) {
        dimmingStatus_ = DimmingStatus::NOT_DIMMING;
        HgmTaskHandleThread::Instance().RemoveEvent("DimmingTask");
        currRefreshRate_ = voteRefreshRate;
        return voteRefreshRate;
    }
    HGM_LOGD("Dimming processing, current refresh rate: %{public}d, vote refresh rate: %{public}d, status: %{public}d",
        currRefreshRate_, voteRefreshRate, dimmingStatus_);
    RS_TRACE_NAME_FMT("%s: Dimming processing, current refresh rate: %d, vote refresh rate: %d, status: %d", __func__,
        currRefreshRate_, voteRefreshRate, dimmingStatus_);
    auto iter = std::lower_bound(refreshRateVec_.begin(), refreshRateVec_.end(), currRefreshRate_);
    if (iter == refreshRateVec_.end() || *iter != currRefreshRate_ ||
        !std::binary_search(refreshRateVec_.begin(), refreshRateVec_.end(), voteRefreshRate)) {
        HGM_LOGW("Dimming currRefreshRate or voteRefreshRate not in refreshRateVec, exit");
        dimmingStatus_ = DimmingStatus::NOT_DIMMING;
        HgmTaskHandleThread::Instance().RemoveEvent("DimmingTask");
        currRefreshRate_ = voteRefreshRate;
        return voteRefreshRate;
    }
    int32_t currStatus = currRefreshRate_ < voteRefreshRate ? DimmingStatus::DIMMING_UP : DimmingStatus::DIMMING_DOWN;
    uint32_t index = std::distance(refreshRateVec_.begin(), iter);
    if (dimmingStatus_ == DimmingStatus::NOT_DIMMING || dimmingStatus_ != currStatus || currTime >= dimmingEndTime_) {
        dimmingStatus_ = currStatus;
        dimmingEndTime_ = currTime + std::chrono::milliseconds(dimmingTimeoutMs);
        if (dimmingEventCallback_ != nullptr) {
            dimmingEventCallback_(dimmingTimeoutMs);
        }
        currRefreshRate_ = refreshRateVec_.at(index + dimmingStatus_);
        HGM_LOGD("Dimming switch to next refresh rate: %{public}d", currRefreshRate_);
        RS_TRACE_NAME_FMT("%s: Dimming switch to next refresh rate: %d", __func__, currRefreshRate_);
    }
    return currRefreshRate_;
}
} // OHOS::Rosen
