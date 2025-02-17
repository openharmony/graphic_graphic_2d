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

const static std::string RS_ENERGY_ASSURANCE_TASK_ID = "RS_ENERGY_ASSURANCE_TASK_ID";
static const std::unordered_map<std::string, uint32_t> UI_RATE_TYPE_NAME_MAP = {
    {"ui_animation", UI_ANIMATION_FRAME_RATE_TYPE },
    {"display_sync", DISPLAY_SYNC_FRAME_RATE_TYPE },
    {"ace_component", ACE_COMPONENT_FRAME_RATE_TYPE },
    {"display_soloist", DISPLAY_SOLOIST_FRAME_RATE_TYPE },
};
constexpr int DEFAULT_ENERGY_ASSURANCE_IDLE_FPS = 60;
constexpr int DEFAULT_ANIMATION_IDLE_DURATION = 2000;
constexpr int64_t DEFAULT_RS_ANIMATION_TOUCH_UP_TIME = 1000;
constexpr int32_t UNKNOWN_IDLE_FPS = -1;

HgmEnergyConsumptionPolicy::HgmEnergyConsumptionPolicy()
{
    RsCommonHook::Instance().RegisterStartNewAnimationListener([this](const std::string& componentName) {
        if (isAnimationEnergyConsumptionAssuranceMode_) {
            HgmTaskHandleThread::Instance().PostTask([this, componentName]() { StartNewAnimation(componentName); });
        }
    });
    RsCommonHook::Instance().SetComponentPowerFpsFunc(
        std::bind(&HgmEnergyConsumptionPolicy::GetComponentFps, this, std::placeholders::_1));
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
    auto isEnable = animationPowerConfig.find("animation_energy_assurance_enable");
    isAnimationEnergyAssuranceEnable_ = false;
    if (isEnable != animationPowerConfig.end() && isEnable->second == "true") {
        isAnimationEnergyAssuranceEnable_ = true;
    }

    if (!isAnimationEnergyAssuranceEnable_) {
        HGM_LOGD("HgmEnergyConsumptionPolicy::SetEnergyConsumptionConfig isAnimationLtpoPowerEnable is false");
        return;
    }

    auto idleFps = animationPowerConfig.find("animation_idle_fps");
    animationIdleFps_ = DEFAULT_ENERGY_ASSURANCE_IDLE_FPS;
    if (idleFps != animationPowerConfig.end()) {
        ConverStrToInt(animationIdleFps_, idleFps->second, DEFAULT_ENERGY_ASSURANCE_IDLE_FPS);
    }

    auto idleDuration = animationPowerConfig.find("animation_idle_duration");
    animationIdleDuration_ = DEFAULT_ANIMATION_IDLE_DURATION;
    if (idleDuration != animationPowerConfig.end()) {
        ConverStrToInt(animationIdleDuration_, idleDuration->second, DEFAULT_ANIMATION_IDLE_DURATION);
    }

    auto touchUpTime = animationPowerConfig.find("animation_touch_up_duration");
    rsAnimationTouchIdleTime_ = DEFAULT_RS_ANIMATION_TOUCH_UP_TIME;
    if (touchUpTime != animationPowerConfig.end()) {
        int delayTime = 0;
        ConverStrToInt(delayTime, touchUpTime->second, DEFAULT_RS_ANIMATION_TOUCH_UP_TIME);
        rsAnimationTouchIdleTime_ = delayTime;
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
    lastAnimationTimestamp_ = firstAnimationTimestamp_.load();
}

void HgmEnergyConsumptionPolicy::StatisticAnimationTime(uint64_t timestamp)
{
    if (!isAnimationEnergyAssuranceEnable_ || !isAnimationEnergyConsumptionAssuranceMode_) {
        return;
    }
    lastAnimationTimestamp_ = timestamp;
}

void HgmEnergyConsumptionPolicy::StartNewAnimation(const std::string &componentName)
{
    auto idleFps = GetComponentEnergyConsumptionConfig(componentName);
    if (idleFps != UNKNOWN_IDLE_FPS) {
        return;
    }
    if (!isAnimationEnergyAssuranceEnable_ || !isAnimationEnergyConsumptionAssuranceMode_) {
        return;
    }
    firstAnimationTimestamp_ = HgmCore::Instance().GetCurrentTimestamp() / NS_PER_MS;
    lastAnimationTimestamp_ = firstAnimationTimestamp_.load();
}

void HgmEnergyConsumptionPolicy::SetTouchState(TouchState touchState)
{
    if (touchState == TouchState::IDLE_STATE) {
        isTouchIdle_ = true;
        return;
    }

    HgmTaskHandleThread::Instance().RemoveEvent(RS_ENERGY_ASSURANCE_TASK_ID);
    // touch
    if (touchState == TouchState::DOWN_STATE) {
        isTouchIdle_ = false;
        SetAnimationEnergyConsumptionAssuranceMode(false);
    } else if (touchState == TouchState::UP_STATE) {
        HgmTaskHandleThread::Instance().PostEvent(
            RS_ENERGY_ASSURANCE_TASK_ID, [this]() { SetAnimationEnergyConsumptionAssuranceMode(true); },
            rsAnimationTouchIdleTime_);
    }
}

void HgmEnergyConsumptionPolicy::GetComponentFps(FrameRateRange& range)
{
    if (!isTouchIdle_) {
        return;
    }
    auto idleFps = GetComponentEnergyConsumptionConfig(range.GetComponentName());
    if (idleFps != UNKNOWN_IDLE_FPS) {
        SetEnergyConsumptionRateRange(range, idleFps);
    }
}

void HgmEnergyConsumptionPolicy::GetAnimationIdleFps(FrameRateRange& rsRange)
{
    if (!isAnimationEnergyAssuranceEnable_ || !isAnimationEnergyConsumptionAssuranceMode_) {
        return;
    }
    // The animation takes effect after a certain period of time
    if (lastAnimationTimestamp_ <= firstAnimationTimestamp_ ||
        (lastAnimationTimestamp_ - firstAnimationTimestamp_) < static_cast<uint64_t>(animationIdleDuration_)) {
        return;
    }
    SetEnergyConsumptionRateRange(rsRange, animationIdleFps_);
}

bool HgmEnergyConsumptionPolicy::GetUiIdleFps(FrameRateRange& rsRange)
{
    if (!isTouchIdle_) {
        return false;
    }
    auto it = uiEnergyAssuranceMap_.find(rsRange.type_ & ~ANIMATION_STATE_FIRST_FRAME);
    if (it == uiEnergyAssuranceMap_.end()) {
        HGM_LOGD("HgmEnergyConsumptionPolicy::GetUiIdleFps the rateType = %{public}d is invalid", rsRange.type_);
        return false;
    }
    bool isEnergyAssuranceEnable = it->second.first;
    int idleFps = it->second.second;
    if (isEnergyAssuranceEnable) {
        SetEnergyConsumptionRateRange(rsRange, idleFps);
    }
    return true;
}

void HgmEnergyConsumptionPolicy::SetRefreshRateMode(int32_t currentRefreshMode, std::string curScreenStrategyId)
{
    currentRefreshMode_ = currentRefreshMode;
    curScreenStrategyId_ = curScreenStrategyId;
}

void HgmEnergyConsumptionPolicy::SetEnergyConsumptionRateRange(FrameRateRange& rsRange, int idleFps)
{
    if (rsRange.preferred_ > idleFps) {
        rsRange.isEnergyAssurance_ = true;
    }
    rsRange.max_ = std::min(rsRange.max_, idleFps);
    rsRange.min_ = std::min(rsRange.min_, idleFps);
    rsRange.preferred_ = std::min(rsRange.preferred_, idleFps);
}

void HgmEnergyConsumptionPolicy::PrintEnergyConsumptionLog(const FrameRateRange& rsRange)
{
    std::string lastAssuranceLog = "NO_CONSUMPTION_ASSURANCE";
    if (!rsRange.isEnergyAssurance_) {
        if (lastAssuranceLog_ == lastAssuranceLog) {
            return;
        }
        lastAssuranceLog_ = lastAssuranceLog;
        RS_TRACE_NAME_FMT("SetEnergyConsumptionRateRange rateType:%s", lastAssuranceLog_.c_str());
        HGM_LOGD("change power policy is %{public}s", lastAssuranceLog.c_str());
        return;
    }

    if (rsRange.GetComponentName() != "UNKNOWN_SCENE") {
        lastAssuranceLog = std::string("COMPONENT_ASSURANCE[ ") + rsRange.GetComponentName() + "]";
        if (lastAssuranceLog_ == lastAssuranceLog) {
            return;
        }
        lastAssuranceLog_ = lastAssuranceLog;
        RS_TRACE_NAME_FMT("SetEnergyConsumptionRateRange rateType:%s", lastAssuranceLog_.c_str());
        HGM_LOGD("change power policy is %{public}s", lastAssuranceLog.c_str());
        return;
    }

    lastAssuranceLog = rsRange.GetExtInfo();
    if (lastAssuranceLog == "" || lastAssuranceLog_ == lastAssuranceLog) {
        return;
    }
    lastAssuranceLog_ = lastAssuranceLog;
    RS_TRACE_NAME_FMT("SetEnergyConsumptionRateRange rateType:%s", lastAssuranceLog_.c_str());
    HGM_LOGD("change power policy is %{public}s", lastAssuranceLog.c_str());
}

int32_t HgmEnergyConsumptionPolicy::GetComponentEnergyConsumptionConfig(const std::string &componentName)
{
    const auto& configData = HgmCore::Instance().GetPolicyConfigData();
    if (!configData) {
        return UNKNOWN_IDLE_FPS;
    }
    const auto settingMode = std::to_string(currentRefreshMode_);
    const auto curScreenStrategyId = curScreenStrategyId_;
    if (configData->screenConfigs_.count(curScreenStrategyId_) &&
        configData->screenConfigs_[curScreenStrategyId_].count(settingMode)) {
        auto& screenConfig = configData->screenConfigs_[curScreenStrategyId_][settingMode];
        auto idleFps = UNKNOWN_IDLE_FPS;
        if (screenConfig.componentPowerConfig.count(componentName)) {
            idleFps = screenConfig.componentPowerConfig[componentName];
        }
        return idleFps;
    }
    return UNKNOWN_IDLE_FPS;
}

} // namespace OHOS::Rosen