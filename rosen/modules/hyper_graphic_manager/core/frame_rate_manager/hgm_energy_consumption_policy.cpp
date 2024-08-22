/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "hgm_energy_consumption_policy.h"

#include <functional>

#include "hgm_core.h"
#include "hgm_log.h"
#include "hgm_task_handle_thread.h"
#include "rs_trace.h"
#include "xml_parser.h"

#include "common/rs_common_def.h"
#include "common/rs_common_hook.h"

namespace OHOS::Rosen {
static const std::string IS_ANIMATION_ENERGY_ASSURANCE_ENABLE = "animation_energy_assurance_enable";
static const std::string ANIMATION_IDLE_FPS = "animation_idle_fps";
static const std::string ANIMATION_IDLE_DURATION = "animation_idle_duration";
static const std::unordered_map<std::string, int32_t> UI_RATE_TYPE_NAME_MAP = {
    {"ui_animation", UI_ANIMATION_FRAME_RATE_TYPE },
    {"display_sync", DISPLAY_SYNC_FRAME_RATE_TYPE },
    {"ace_component", ACE_COMPONENT_FRAME_RATE_TYPE },
    {"display_soloist", DISPLAY_SOLOIST_FRAME_RATE_TYPE },
};
constexpr int DEFAULT_ENERGY_ASSURANCE_IDLE_FPS = 60;
constexpr int DEFAULT_ANIMATION_IDLE_DURATION = 2000;
constexpr int64_t ENERGY_ASSURANCE_LOG_DELAY_TIME = 50;
static const std::string ENERGY_ASSURANCE_LOG_TASK_ID = "ENERGY_ASSURANCE_LOG_TASK_ID";

HgmEnergyConsumptionPolicy::HgmEnergyConsumptionPolicy()
{
    RsCommonHook::Instance().RegisterStartNewAnimationListener([this] () {
        HgmTaskHandleThread::Instance().PostTask([this] () { StartNewAnimation(); });
    });
}

HgmEnergyConsumptionPolicy& HgmEnergyConsumptionPolicy::Instance()
{
    static HgmEnergyConsumptionPolicy hlpp;
    return hlpp;
}

void HgmEnergyConsumptionPolicy::ConverStrToInt(int& targetNum, std::string sourceStr, int defaultValue)
{
    if (!XMLParser::IsNumber(sourceStr)) {
        targetNum = defaultValue;
        return;
    }
    targetNum = std::stoi(sourceStr.c_str());
}

void HgmEnergyConsumptionPolicy::SetEnergyConsumptionConfig(
    std::unordered_map<std::string, std::string> animationPowerConfig)
{
    if (animationPowerConfig.count(IS_ANIMATION_ENERGY_ASSURANCE_ENABLE) == 0) {
        isAnimationEnergyAssuranceEnable_ = false;
    } else {
        isAnimationEnergyAssuranceEnable_ =
            animationPowerConfig[IS_ANIMATION_ENERGY_ASSURANCE_ENABLE] == "true" ? true : false;
    }
    if (!isAnimationEnergyAssuranceEnable_) {
        HGM_LOGD("HgmEnergyConsumptionPolicy::SetEnergyConsumptionConfig isAnimationLtpoPowerEnable is false");
        return;
    }

    if (animationPowerConfig.count(ANIMATION_IDLE_FPS) == 0) {
        animationIdleFps_ = DEFAULT_ENERGY_ASSURANCE_IDLE_FPS;
    } else {
        ConverStrToInt(animationIdleFps_, animationPowerConfig[ANIMATION_IDLE_FPS], DEFAULT_ENERGY_ASSURANCE_IDLE_FPS);
    }

    if (animationPowerConfig.count(ANIMATION_IDLE_DURATION) == 0) {
        animationIdleDuration_ = DEFAULT_ANIMATION_IDLE_DURATION;
    } else {
        ConverStrToInt(
            animationIdleDuration_, animationPowerConfig[ANIMATION_IDLE_DURATION], DEFAULT_ANIMATION_IDLE_DURATION);
    }
    HGM_LOGD("HgmEnergyConsumptionPolicy::SetEnergyConsumptionConfig update config success");
}

void HgmEnergyConsumptionPolicy::SetUiEnergyConsumptionConfig(
    std::unordered_map<std::string, std::string> uiPowerConfig)
{
    uiEnergyAssuranceMap_.clear();
    for (auto config : uiPowerConfig) {
        std::string rateTypeName = config.first;
        auto it = UI_RATE_TYPE_NAME_MAP.find(rateTypeName);
        if (it == UI_RATE_TYPE_NAME_MAP.end()) {
            HGM_LOGD("HgmEnergyConsumptionPolicy::SetUiEnergyConsumptionConfig the rateType is invalid");
            continue;
        }
        int idleFps = 60;
        ConverStrToInt(idleFps, config.second, DEFAULT_ENERGY_ASSURANCE_IDLE_FPS);
        uiEnergyAssuranceMap_[it->second] = std::make_pair(true, idleFps);
    }
}

void HgmEnergyConsumptionPolicy::SetAnimationEnergyConsumptionAssuranceMode(bool isEnergyConsumptionAssuranceMode)
{
    if (!isAnimationEnergyAssuranceEnable_ ||
        isAnimationEnergyConsumptionAssuranceMode_ == isEnergyConsumptionAssuranceMode) {
        return;
    }
    isAnimationEnergyConsumptionAssuranceMode_ = isEnergyConsumptionAssuranceMode;
    firstAnimationTimestamp_ = HgmCore::Instance().GetCurrentTimestamp() / NS_PER_MS;
    lastAnimationTimestamp_ = firstAnimationTimestamp_;
}

void HgmEnergyConsumptionPolicy::SetUiEnergyConsumptionAssuranceMode(bool isEnergyConsumptionAssuranceMode)
{
    isUiEnergyConsumptionAssuranceMode_ = isEnergyConsumptionAssuranceMode;
}

void HgmEnergyConsumptionPolicy::StatisticAnimationTime(uint64_t timestamp)
{
    if (!isAnimationEnergyAssuranceEnable_ || !isAnimationEnergyConsumptionAssuranceMode_) {
        return;
    }
    lastAnimationTimestamp_ = timestamp;
}

void HgmEnergyConsumptionPolicy::StartNewAnimation()
{
    if (!isAnimationEnergyAssuranceEnable_ || !isAnimationEnergyConsumptionAssuranceMode_) {
        return;
    }
    firstAnimationTimestamp_ = HgmCore::Instance().GetCurrentTimestamp() / NS_PER_MS;
    lastAnimationTimestamp_ = firstAnimationTimestamp_;
}

void HgmEnergyConsumptionPolicy::GetAnimationIdleFps(FrameRateRange& rsRange)
{
    rsRange.isEnergyAssurance_ = false;
    if (!isAnimationEnergyAssuranceEnable_ || !isAnimationEnergyConsumptionAssuranceMode_) {
        PrintLog(rsRange, false, 0);
        return;
    }
    if (lastAnimationTimestamp_ > firstAnimationTimestamp_ &&
        (lastAnimationTimestamp_ - firstAnimationTimestamp_) < static_cast<uint64_t>(animationIdleDuration_)) {
        PrintLog(rsRange, false, 0);
        return;
    }
    SetEnergyConsumptionRateRange(rsRange, animationIdleFps_);
}

void HgmEnergyConsumptionPolicy::GetUiIdleFps(FrameRateRange& rsRange)
{
    rsRange.isEnergyAssurance_ = false;
    if (!isUiEnergyConsumptionAssuranceMode_) {
        PrintLog(rsRange, false, 0);
        return;
    }
    auto it = uiEnergyAssuranceMap_.find(rsRange.type_);
    if (it == uiEnergyAssuranceMap_.end()) {
        HGM_LOGD("HgmEnergyConsumptionPolicy::GetUiIdleFps the rateType = %{public}d is invalid", rsRange.type_);
        return;
    }
    bool isEnergyAssuranceEnable = it->second.first;
    int idleFps = it->second.second;
    if (isEnergyAssuranceEnable) {
        SetEnergyConsumptionRateRange(rsRange, idleFps);
    } else {
        PrintLog(rsRange, false, 0);
    }
}

void HgmEnergyConsumptionPolicy::SetEnergyConsumptionRateRange(FrameRateRange& rsRange, int idleFps)
{
    RS_TRACE_NAME_FMT("SetEnergyConsumptionRateRange rateType:%s, maxFps:%d", rsRange.GetExtInfo().c_str(), idleFps);
    if (rsRange.preferred_ > idleFps) {
        PrintLog(rsRange, true, idleFps);
        rsRange.isEnergyAssurance_ = true;
    }
    rsRange.max_ = std::min(rsRange.max_, idleFps);
    rsRange.min_ = std::min(rsRange.min_, idleFps);
    rsRange.preferred_ = std::min(rsRange.preferred_, idleFps);
}

void HgmEnergyConsumptionPolicy::PrintLog(FrameRateRange &rsRange, bool state, int idleFps)
{
    const auto it = energyAssuranceState_.find(rsRange.type_);
    const auto taskId = ENERGY_ASSURANCE_LOG_TASK_ID + std::to_string(rsRange.type_);
    // Enter assurance solution
    if (state) {
        HgmTaskHandleThread::Instance().RemoveEvent(taskId);
        if (it == energyAssuranceState_.end() || !it->second) {
            energyAssuranceState_[rsRange.type_] = state;
            HGM_LOGI("HgmEnergyConsumptionPolicy enter the energy consumption assurance mode, rateType:%{public}s, "
                "maxFps:%{public}d", rsRange.GetExtInfo().c_str(), idleFps);
        }

        // Continued assurance status
        auto task = [this, rsRange]() {
            energyAssuranceState_[rsRange.type_] = false;
            HGM_LOGI("HgmEnergyConsumptionPolicy exit the energy consumption assurance mode, rateType:%{public}s",
                rsRange.GetExtInfo().c_str());
        };
        HgmTaskHandleThread::Instance().PostEvent(taskId, task, ENERGY_ASSURANCE_LOG_DELAY_TIME);
        return;
    }

    // Exit assurance solution
    if (it != energyAssuranceState_.end() && it->second && !state) {
        HgmTaskHandleThread::Instance().RemoveEvent(taskId);
        energyAssuranceState_[rsRange.type_] = state;
        HGM_LOGI("HgmEnergyConsumptionPolicy exit the energy consumption assurance mode, rateType:%{public}s",
            rsRange.GetExtInfo().c_str());
    }
}

} // namespace OHOS::Rosen