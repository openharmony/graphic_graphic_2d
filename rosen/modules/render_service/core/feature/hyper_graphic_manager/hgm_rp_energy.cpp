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
#include "feature/hyper_graphic_manager/hgm_rp_energy.h"

#include <algorithm>

#include "rs_frame_rate_vote.h"
#include "rs_trace.h"

#include "common/rs_common_hook.h"
#include "pipeline/render_thread/rs_base_surface_util.h"
#include "platform/common/rs_log.h"

namespace OHOS::Rosen {
namespace {
constexpr uint64_t VIDEO_CALL_PTS_OFFSET = 50 * 1000 * 1000; // 50ms
}

HgmRPEnergy::HgmRPEnergy()
{
    auto componentFpsFunc = std::bind(&HgmRPEnergy::GetComponentFps, this, std::placeholders::_1);
    RsCommonHook::Instance().SetComponentPowerFpsFunc(componentFpsFunc);

    auto startAnimationFunc = std::bind(&HgmRPEnergy::AddEnergyCommonData, this, EnergyEvent::START_NEW_ANIMATION,
        "COMPONENT_NAME", std::placeholders::_1);
    RsCommonHook::Instance().RegisterStartNewAnimationListener(startAnimationFunc);

    auto videoVoterFunc = std::bind(&HgmRPEnergy::AddEnergyCommonData, this, EnergyEvent::VOTER_VIDEO_RATE,
        std::placeholders::_1, std::placeholders::_2);
    DelayedSingleton<RSFrameRateVote>::GetInstance()->SetVoterRateFunc(videoVoterFunc);

    RSBaseSurfaceUtil::RegisterVideoCallPtsOffSet(
        [this](pid_t pid, const std::string& bufferName) { return this->GetVideoCallPtsOffset(pid, bufferName); });
}

HgmRPEnergy::~HgmRPEnergy() {}

void HgmRPEnergy::SetTouchState(bool isIdle)
{
    RS_TRACE_NAME_FMT("HgmRPEnergy::SetTouchState, %d", isIdle);
    isTouchIdle_ = isIdle;
}

void HgmRPEnergy::HgmConfigUpdateCallback(std::shared_ptr<RPHgmConfigData> configData)
{
    if (configData == nullptr) {
        ROSEN_LOGE("%{public}s configData is null", __func__);
        return;
    }
    componentPowerConfig_ = configData->GetComponentPowerConfig();
    videoCallLayerConfig_ = configData->GetVideoCallLayerConfig();
    DelayedSingleton<RSFrameRateVote>::GetInstance()->SetVideoFrameRateSwtich(configData->GetVideoSwitch());
}

void HgmRPEnergy::GetComponentFps(FrameRateRange& range)
{
    if (!isTouchIdle_) {
        return;
    }
    auto componentFpsIter = componentPowerConfig_.find(range.GetComponentName());
    if (componentFpsIter == componentPowerConfig_.end()) {
        return;
    }

    auto idleFps = componentFpsIter->second;
    if (range.preferred_ > idleFps) {
        range.isEnergyAssurance_ = true;
    }
    range.max_ = std::min(range.max_, idleFps);
    range.min_ = std::min(range.min_, idleFps);
    range.preferred_ = std::min(range.preferred_, idleFps);
}

void HgmRPEnergy::MoveEnergyCommonDataTo(EnergyCommonDataMap& commonData)
{
    std::unique_lock<std::mutex> lock(mutex_);
    std::swap(energyCommonData_, commonData);
}

void HgmRPEnergy::AddEnergyCommonData(EnergyEvent event, const std::string& key, const std::string& value)
{
    RS_TRACE_NAME_FMT("AddEnergyCommonData [%d, %s, %s]", (int32_t)event, key.c_str(), value.c_str());
    std::unique_lock<std::mutex> lock(mutex_);
    auto dataMapIter = energyCommonData_.find(event);
    if (dataMapIter == energyCommonData_.end()) {
        energyCommonData_[event] = { { key, value } };
        return;
    }
    dataMapIter->second[key] = value;
}

void HgmRPEnergy::StatisticAnimationTime(uint64_t timestamp)
{
    AddEnergyCommonData(EnergyEvent::ANIMATION_EXEC_TIME, "STATIC_ANIMATION_TIME", std::to_string(timestamp));
}

void HgmRPEnergy::SetVideoCallPid(pid_t pid)
{
    videoCallPid_.store(pid);
}

uint64_t HgmRPEnergy::GetVideoCallPtsOffset(pid_t pid, const std::string& bufferName)
{
    if (videoCallPid_.load() == DEFAULT_PID || pid != videoCallPid_.load()) {
        return 0;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    auto videoCallIter = std::find_if(videoCallLayerConfig_.begin(), videoCallLayerConfig_.end(),
        [bufferName](const auto& iter) { return bufferName.find(iter.second) != std::string::npos; });
    if (videoCallIter != videoCallLayerConfig_.end()) {
        RS_TRACE_FUNC();
        return VIDEO_CALL_PTS_OFFSET;
    }
    return 0;
}
} // namespace OHOS::Rosen
