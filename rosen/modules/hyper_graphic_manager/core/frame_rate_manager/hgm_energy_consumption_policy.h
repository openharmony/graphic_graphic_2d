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

#ifndef HGM_ENERGY_CONSUMPTION_POLICY_H
#define HGM_ENERGY_CONSUMPTION_POLICY_H

#include <mutex>
#include <string>
#include <unordered_map>

#include "hgm_command.h"
#include "hgm_touch_manager.h"
#include "variable_frame_rate/rs_variable_frame_rate.h"

#include "animation/rs_frame_rate_range.h"
#include "common/rs_common_def.h"

namespace OHOS::Rosen {
class HgmEnergyConsumptionPolicy {
public:
    static HgmEnergyConsumptionPolicy& Instance();
    void SetEnergyConsumptionConfig(std::unordered_map<std::string, std::string> animationPowerConfig);
    void SetUiEnergyConsumptionConfig(std::unordered_map<std::string, std::string> uiPowerConfig);
    void SetAnimationEnergyConsumptionAssuranceMode(bool isEnergyConsumptionAssuranceMode);
    // called by RSMainThread
    void StatisticAnimationTime(uint64_t timestamp);
    void StartNewAnimation(const std::string &componentName);
    // called by RSMainThread
    void GetAnimationIdleFps(FrameRateRange& rsRange);
    void SetTouchState(TouchState touchState);
    
    bool GetUiIdleFps(FrameRateRange& rsRange);
    void SetRefreshRateMode(int32_t currentRefreshMode, std::string curScreenStrategyId);
    void PrintEnergyConsumptionLog(const FrameRateRange &rsRange);
    void SetVideoCallSceneInfo(const EventInfo &eventInfo);
    // called by RSMainThread
    void StatisticsVideoCallBufferCount(pid_t pid, const std::string &surfaceName);
    // called by RSMainThread
    void CheckOnlyVideoCallExist();
    // called by RSMainThread
    bool GetVideoCallVsyncChange();
    bool GetVideoCallFrameRate(pid_t pid, const std::string &vsyncName, FrameRateRange &finalRange);
    void SetCurrentPkgName(const std::vector<std::string> &pkgs);
    void SetEnergyConsumptionAssuranceSceneInfo(const EventInfo& eventInfo);

private:
    // <rateType, <isEnable, idleFps>>
    std::unordered_map<uint32_t, std::pair<bool, int>> uiEnergyAssuranceMap_;
    std::atomic<bool> isAnimationEnergyAssuranceEnable_ = false;
    std::atomic<bool> isAnimationEnergyConsumptionAssuranceMode_ = false;
    bool isTouchIdle_ = false;
    int64_t rsAnimationTouchIdleTime_ = 1000;
    std::atomic<uint64_t> firstAnimationTimestamp_ = 0;
    std::atomic<uint64_t> lastAnimationTimestamp_ = 0;
    // Unit: ms
    int animationIdleDuration_ = 2000;
    int animationIdleFps_ = 60;
    std::string lastAssuranceLog_ = "";
    int32_t currentRefreshMode_ = -1;
    std::string curScreenStrategyId_ = "LTPO-DEFAULT";
    std::atomic<pid_t> videoCallPid_ = { DEFAULT_PID };
    std::string videoCallVsyncName_ = "";
    int videoCallMaxFrameRate_ = 0;
    std::atomic<bool> isEnableVideoCall_ = { false };
    std::atomic<int32_t> videoBufferCount_ = { 0 };
    std::atomic<bool> isSubmitDecisionTask_ = { false };
    std::atomic<bool> isOnlyVideoCallExist_ = { false };
    std::atomic<bool> isVideoCallVsyncChange_ = { false };
    // concurrency protection >>>
    mutable std::mutex videoCallLock_;
    std::string videoCallLayerName_ = "";
    // concurrency protection <<<
    std::atomic<bool> aceComponentEnable_ = { true };

    HgmEnergyConsumptionPolicy();
    ~HgmEnergyConsumptionPolicy() = default;
    HgmEnergyConsumptionPolicy(const HgmEnergyConsumptionPolicy&) = delete;
    HgmEnergyConsumptionPolicy(const HgmEnergyConsumptionPolicy&&) = delete;
    HgmEnergyConsumptionPolicy& operator=(const HgmEnergyConsumptionPolicy&) = delete;
    HgmEnergyConsumptionPolicy& operator=(const HgmEnergyConsumptionPolicy&&) = delete;
    static void ConverStrToInt(int& targetNum, std::string sourceStr, int defaultValue);
    void SetEnergyConsumptionRateRange(FrameRateRange& rsRange, int idleFps);
    int32_t GetComponentEnergyConsumptionConfig(const std::string &componentName);
    // Invoked by the render_service thread
    void GetComponentFps(FrameRateRange &rsRange);
};
} // namespace OHOS::Rosen

#endif