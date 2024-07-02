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
static const std::string IS_ANIMATION_ENERGY_ASSURANCE_ENABLE = "animation_energy_assurance_enable";
static const std::string ANIMATION_IDLE_FPS = "animation_idle_fps";
static const std::string ANIMATION_IDLE_DURATION = "animation_idle_duration";
constexpr int DEFAULT_ANIMATION_IDLE_FPS = 60;
constexpr int DEFAULT_ANIMATION_IDLE_DURATION = 2000;

HgmEnrtgyConsumptionPolicy::HgmEnrtgyConsumptionPolicy()
{
    RsCommonHook::Instance().RegisterStartNewAnimationListener(
        std::bind(&HgmEnrtgyConsumptionPolicy::StartNewAnimation, this));
}

HgmEnrtgyConsumptionPolicy& HgmEnrtgyConsumptionPolicy::Instance()
{
    static HgmEnrtgyConsumptionPolicy hlpp;
    return hlpp;
}

void HgmEnrtgyConsumptionPolicy::ConverStrToInt(int& targetNum, std::string sourceStr, int defaultValue)
{
    if (!XMLParser::IsNumber(sourceStr)) {
        targetNum = defaultValue;
        return;
    }
    targetNum = std::stoi(sourceStr.c_str());
}

void HgmEnrtgyConsumptionPolicy::SetEnergyConsumptionConfig(
    std::unordered_map<std::string, std::string> animationPowerConfig)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (animationPowerConfig.count(IS_ANIMATION_ENERGY_ASSURANCE_ENABLE) == 0) {
        isAnimationEnergyAssuranceEnable_ = false;
    } else {
        isAnimationEnergyAssuranceEnable_ =
            animationPowerConfig[IS_ANIMATION_ENERGY_ASSURANCE_ENABLE] == "true" ? true : false;
    }
    if (!isAnimationEnergyAssuranceEnable_) {
        HGM_LOGD("HgmEnrtgyConsumptionPolicy::SetEnergyConsumptionConfig isAnimationLtpoPowerEnable is false");
        return;
    }

    if (animationPowerConfig.count(ANIMATION_IDLE_FPS) == 0) {
        animationIdleFps_ = DEFAULT_ANIMATION_IDLE_FPS;
    } else {
        ConverStrToInt(animationIdleFps_, animationPowerConfig[ANIMATION_IDLE_FPS], DEFAULT_ANIMATION_IDLE_FPS);
    }

    if (animationPowerConfig.count(ANIMATION_IDLE_DURATION) == 0) {
        animationIdleDuration_ = DEFAULT_ANIMATION_IDLE_DURATION;
    } else {
        ConverStrToInt(
            animationIdleDuration_, animationPowerConfig[ANIMATION_IDLE_DURATION], DEFAULT_ANIMATION_IDLE_DURATION);
    }
    HGM_LOGD("HgmEnrtgyConsumptionPolicy::SetEnergyConsumptionConfig update config success");
}

void HgmEnrtgyConsumptionPolicy::SetEnergyConsumptionAssuranceMode(bool isEnergyConsumptionAssuranceMode)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (!isAnimationEnergyAssuranceEnable_ || isEnergyConsumptionAssuranceMode_ == isEnergyConsumptionAssuranceMode) {
        return;
    }
    isEnergyConsumptionAssuranceMode_ = isEnergyConsumptionAssuranceMode;
    firstAnimationTimestamp_ = HgmCore::Instance().GetCurrentTimestamp() / NS_PER_MS;
    lastAnimationTimestamp_ = firstAnimationTimestamp_;
}

void HgmEnrtgyConsumptionPolicy::StatisticAnimationTime(uint64_t timestamp)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (!isAnimationEnergyAssuranceEnable_ || !isEnergyConsumptionAssuranceMode_) {
        return;
    }
    lastAnimationTimestamp_ = timestamp;
}

void HgmEnrtgyConsumptionPolicy::StartNewAnimation()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (!isAnimationEnergyAssuranceEnable_ || !isEnergyConsumptionAssuranceMode_) {
        return;
    }
    firstAnimationTimestamp_ = HgmCore::Instance().GetCurrentTimestamp() / NS_PER_MS;
    lastAnimationTimestamp_ = firstAnimationTimestamp_;
}

void HgmEnrtgyConsumptionPolicy::GetAnimationIdleFps(FrameRateRange& rsRange)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (!isAnimationEnergyAssuranceEnable_ || !isEnergyConsumptionAssuranceMode_) {
        return;
    }
    if (lastAnimationTimestamp_ > firstAnimationTimestamp_ &&
        (lastAnimationTimestamp_ - firstAnimationTimestamp_) < static_cast<uint64_t>(animationIdleDuration_)) {
        return;
    }
    rsRange.max_ = std::min(rsRange.max_, animationIdleFps_);
    rsRange.min_ = std::min(rsRange.min_, animationIdleFps_);
    rsRange.preferred_ = std::min(rsRange.preferred_, animationIdleFps_);
}

} // namespace OHOS::Rosen