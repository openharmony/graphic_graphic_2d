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
#include "xml_parser.h"

#include "common/rs_common_def.h"
#include "common/rs_common_hook.h"

namespace OHOS::Rosen {
static const std::string IS_ENERGY_ASSURANCE_ENABLE = "energy_assurance_enable";
static const std::string IDLE_FPS = "idle_fps";
static const std::string IDLE_DURATION = "idle_duration";
constexpr int DEFAULT_IDLE_FPS = 60;
constexpr int DEFAULT_ANIMATION_IDLE_DURATION = 2000;

HgmEnergyConsumptionPolicy::HgmEnergyConsumptionPolicy()
{
    RsCommonHook::Instance().RegisterStartNewAnimationListener(
        std::bind(&HgmEnergyConsumptionPolicy::StartNewAnimation, this));
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
    std::unordered_map<std::string, std::string> powerConfig, int32_t type)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    bool isEnergyAssuranceEnable = false;
    auto enableIt = powerConfig.find(IS_ENERGY_ASSURANCE_ENABLE);
    if (enableIt != powerConfig.end()) {
        isEnergyAssuranceEnable = enableIt->second == "true" ? true : false;
    }

    int idleFps = 60;
    auto idleFpsIt = powerConfig.find(IDLE_FPS);
    if (idleFpsIt != powerConfig.end()) {
        ConverStrToInt(idleFps, idleFpsIt->second, DEFAULT_IDLE_FPS);
    }

    switch (type) {
        case RS_ANIMATION_FRAME_RATE_TYPE: {
            isAnimationEnergyAssuranceEnable_ = isEnergyAssuranceEnable;
            animationIdleFps_ = idleFps;
            if (powerConfig.count(IDLE_DURATION) == 0) {
                animationIdleDuration_ = DEFAULT_ANIMATION_IDLE_DURATION;
            } else {
                ConverStrToInt(animationIdleDuration_, powerConfig[IDLE_DURATION], DEFAULT_ANIMATION_IDLE_DURATION);
            }
            break;
        }
        case UI_ANIMATION_FRAME_RATE_TYPE: {
            isUiAnimationEnergyAssuranceEnable_ = isEnergyAssuranceEnable;
            uiAnimationIdleFps_ = idleFps;
            break;
        }
        case DISPLAY_SYNC_FRAME_RATE_TYPE: {
            isDisplaySyncEnergyAssuranceEnable_ = isEnergyAssuranceEnable;
            displaySyncIdleFps_ = idleFps;
            break;
        }
        case ACE_COMPONENT_FRAME_RATE_TYPE: {
            isAceComponentEnergyAssuranceEnable_ = isEnergyAssuranceEnable;
            aceComponentIdleFps_ = idleFps;
            break;
        }
        case DISPLAY_SOLOIST_FRAME_RATE_TYPE: {
            isDisplaySoloistEnergyAssuranceEnable_ = isEnergyAssuranceEnable;
            displaySoloistIdleFps_ = idleFps;
            break;
        }
        default:
            break;
    }
    HGM_LOGD("HgmEnergyConsumptionPolicy::SetEnergyConsumptionConfig update config success");
}

void HgmEnergyConsumptionPolicy::SetAnimationEnergyConsumptionAssuranceMode(bool isEnergyConsumptionAssuranceMode)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (!isAnimationEnergyAssuranceEnable_ || isAnimationEnergyConsumptionAssuranceMode_ == isEnergyConsumptionAssuranceMode) {
        return;
    }
    isAnimationEnergyConsumptionAssuranceMode_ = isEnergyConsumptionAssuranceMode;
    firstAnimationTimestamp_ = HgmCore::Instance().GetCurrentTimestamp() / NS_PER_MS;
    lastAnimationTimestamp_ = firstAnimationTimestamp_;
}

void HgmEnergyConsumptionPolicy::SetUiEnergyConsumptionAssuranceMode(bool isEnergyConsumptionAssuranceMode)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    isUiEnergyConsumptionAssuranceMode_ = isEnergyConsumptionAssuranceMode;
}

void HgmEnergyConsumptionPolicy::StatisticAnimationTime(uint64_t timestamp)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (!isAnimationEnergyAssuranceEnable_ || !isAnimationEnergyConsumptionAssuranceMode_) {
        return;
    }
    lastAnimationTimestamp_ = timestamp;
}

void HgmEnergyConsumptionPolicy::StartNewAnimation()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (!isAnimationEnergyAssuranceEnable_ || !isAnimationEnergyConsumptionAssuranceMode_) {
        return;
    }
    firstAnimationTimestamp_ = HgmCore::Instance().GetCurrentTimestamp() / NS_PER_MS;
    lastAnimationTimestamp_ = firstAnimationTimestamp_;
}

void HgmEnergyConsumptionPolicy::GetAnimationIdleFps(FrameRateRange& rsRange)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (!isAnimationEnergyAssuranceEnable_ || !isAnimationEnergyConsumptionAssuranceMode_) {
        return;
    }
    if (lastAnimationTimestamp_ > firstAnimationTimestamp_ &&
        (lastAnimationTimestamp_ - firstAnimationTimestamp_) < static_cast<uint64_t>(animationIdleDuration_)) {
        return;
    }
    SetFrameRateRange(rsRange, animationIdleFps_);
}

void HgmEnergyConsumptionPolicy::GetUiIdleFps(FrameRateRange& rsRange)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (!isUiEnergyConsumptionAssuranceMode_) {
        return;
    }
    switch (rsRange.type_) {
        case UI_ANIMATION_FRAME_RATE_TYPE: {
            if (isUiAnimationEnergyAssuranceEnable_) {
                SetFrameRateRange(rsRange, uiAnimationIdleFps_);
            }
            break;
        }
        case DISPLAY_SYNC_FRAME_RATE_TYPE: {
            if (isDisplaySyncEnergyAssuranceEnable_) {
                SetFrameRateRange(rsRange, displaySyncIdleFps_);
            }
            break;
        }
        case ACE_COMPONENT_FRAME_RATE_TYPE: {
            if (isAceComponentEnergyAssuranceEnable_) {
                SetFrameRateRange(rsRange, aceComponentIdleFps_);
            }
            break;
        }
        case DISPLAY_SOLOIST_FRAME_RATE_TYPE: {
            if (isDisplaySoloistEnergyAssuranceEnable_) {
                SetFrameRateRange(rsRange, displaySoloistIdleFps_);
            }
            break;
        }
        default:
            break;
    }
}

void HgmEnergyConsumptionPolicy::SetFrameRateRange(FrameRateRange& rsRange, int idleFps)
{
    rsRange.max_ = std::min(rsRange.max_, idleFps);
    rsRange.min_ = std::min(rsRange.min_, idleFps);
    rsRange.preferred_ = std::min(rsRange.preferred_, idleFps);
}
} // namespace OHOS::Rosen