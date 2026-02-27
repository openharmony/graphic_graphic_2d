/*
<<<<<<< HEAD
 * Copyright (c) 2025 Huawei Device Co., Ltd.
=======
 * Copyright (c) 2026 Huawei Device Co., Ltd.
>>>>>>> gitcode/master
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

<<<<<<< HEAD
#include <algorithm>

#include "rs_frame_rate_vote.h"
#include "rs_trace.h"

#include "common/rs_common_hook.h"
#include "platform/common/rs_log.h"
namespace OHOS::Rosen {
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
    dataMapIter->second.emplace(key, value);
}

void HgmRPEnergy::StatisticAnimationTime(uint64_t timestamp)
{
    AddEnergyCommonData(EnergyEvent::ANIMATION_EXEC_TIME, "STATIC_ANIMATION_TIME", std::to_string(timestamp));
}
} // namespace OHOS::Rosen
=======
#include "common/rs_optional_trace.h"
namespace OHOS {

namespace Rosen {
void HgmRPEnergy::SyncEnergyInfoToRP(const EnergyInfo& energyInfo)
{
    energyInfo_ = energyInfo;
}

void HgmRPEnergy::SetComponentDefaultFps(pid_t pid, FrameRateRange& rsRange)
{
    if (pid != energyInfo_.componentPid || energyInfo_.componentName != SWIPER_DRAG_SCENE) {
        return;
    }
    if (rsRange.componentScene_ != ComponentScene::SWIPER_FLING && !(rsRange.type_ & SWIPER_DRAG_FRAME_RATE_TYPE)) {
        return;
    }
    RS_TRACE_NAME_FMT("%s pid: %d, frameRate: %d", __func__, pid, energyInfo_.componentDefaultFps);
    rsRange.min_ = 0;
    rsRange.max_ = energyInfo_.componentDefaultFps;
    rsRange.preferred_ = energyInfo_.componentDefaultFps;
}
} // namespace Rosen
} // namespace OHOS
>>>>>>> gitcode/master
