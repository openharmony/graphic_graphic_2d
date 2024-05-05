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
#include "hgm_multi_app_strategy.h"

#include <limits>

#include "hgm_core.h"
#include "hgm_frame_rate_manager.h"
#include "rs_trace.h"
#include "xml_parser.h"

namespace OHOS {
namespace Rosen {
namespace {
    static PolicyConfigData::ScreenSetting defaultScreenSetting;
    static PolicyConfigData::StrategyConfigMap defaultStrategyConfigMap;
}

HgmMultiAppStrategy::HgmMultiAppStrategy()
    : screenSettingCache_(defaultScreenSetting), strategyConfigMapCache_(defaultStrategyConfigMap)
{
}

HgmErrCode HgmMultiAppStrategy::HandlePkgsEvent(const std::vector<std::string>& pkgs)
{
    // update pkgs
    if (pkgs_ == pkgs) {
        return HGM_ERROR;
    }
    pkgs_ = pkgs;

    // update pid of pkg
    pidPkgNameMap_.clear();
    for (auto &param : pkgs_) {
        HGM_LOGI("pkg update:%{public}s", param.c_str());
        auto [pkgName, pid] = AnalyzePkgParam(param);
        pidPkgNameMap_[pid] = pkgName;
    }

    CalcVote();

    return EXEC_SUCCESS;
}

void HgmMultiAppStrategy::HandleTouchInfo(const std::string& pkgName, TouchState touchState)
{
    HGM_LOGD("touch info update, pkgName:%{poublic}s, touchState:%{public}d", pkgName.c_str(), touchState);
    touchInfo_ = { pkgName, touchState };
    CalcVote();
}

void HgmMultiAppStrategy::CalcVote()
{
    RS_TRACE_FUNC();
    voteRes_ = { HGM_ERROR, {
        .min = OledRefreshRate::OLED_NULL_HZ,
        .max = OledRefreshRate::OLED_120_HZ,
        .dynamicMode = DynamicModeType::TOUCH_ENABLED,
        .drawMin = OledRefreshRate::OLED_NULL_HZ,
        .drawMax = OledRefreshRate::OLED_120_HZ,
        .down = OledRefreshRate::OLED_120_HZ,
    }};
    uniqueTouchInfo_ = std::make_unique<TouchInfo>(touchInfo_);

    if (pkgs_.size() == 1) {
        // auto follow focus when there is only 1 app
        FollowFocus();
    } else {
        auto &multiAppStrategyType = GetScreenSetting().multiAppStrategyType;
        HGM_LOGD("multiAppStrategyType: %{public}d", multiAppStrategyType);
        switch (multiAppStrategyType) {
            case MultiAppStrategyType::USE_MAX:
                UseMax();
                break;
            case MultiAppStrategyType::FOLLOW_FOCUS:
                FollowFocus();
                break;
            case MultiAppStrategyType::USE_STRATEGY_NUM:
                UseStrategyNum();
                break;
            default:
                UseMax();
                break;
        }
    }

    if (voteRes_.first != EXEC_SUCCESS) {
        // using setting mode when fail to calc vote
        auto &strategyConfigs = GetStrategyConfigs();
        auto &strategyName = GetScreenSetting().strategy;
        if (strategyConfigs.find(strategyName) != strategyConfigs.end()) {
            voteRes_.second = strategyConfigs.at(strategyName);
        }
    }

    UpdateStrategyByTouch(voteRes_.second, "", true);
    HGM_LOGD("final apps res: %{public}d, [%{public}d, %{public}d]",
        voteRes_.first, voteRes_.second.min, voteRes_.second.max);

    OnStrategyChange();
}

HgmErrCode HgmMultiAppStrategy::GetVoteRes(PolicyConfigData::StrategyConfig& strategyRes)
{
    strategyRes = voteRes_.second;
    return voteRes_.first;
}

void HgmMultiAppStrategy::RegisterStrategyChangeCallback(const StrategyChangeCallback& callback)
{
    strategyChangeCallbacks_.emplace_back(callback);
}

void HgmMultiAppStrategy::UpdateXmlConfigCache()
{
    auto &hgmCore = HgmCore::Instance();
    auto frameRateMgr = hgmCore.GetFrameRateMgr();

    auto configData = hgmCore.GetPolicyConfigData();
    if (configData == nullptr || frameRateMgr == nullptr) {
        HGM_LOGD("configData or frameRateMgr is null");
        return;
    }

    // udpate strategyConfigMapCache_
    strategyConfigMapCache_ = configData->strategyConfigs_;

    // update screenSettingCache_
    auto curScreenStrategyId = frameRateMgr->GetCurScreenStrategyId();
    if (configData->screenConfigs_.find(curScreenStrategyId) == configData->screenConfigs_.end()) {
        HGM_LOGD("curScreenStrategyId not existed: %{public}s", curScreenStrategyId.c_str());
        return;
    }
    auto &screenConfig = configData->screenConfigs_[curScreenStrategyId];

    auto curRefreshRateMode = std::to_string(frameRateMgr->GetCurRefreshRateMode());
    if (screenConfig.find(curRefreshRateMode) == screenConfig.end()) {
        HGM_LOGD("curRefreshRateMode not existed: %{public}s", curRefreshRateMode.c_str());
        return;
    }

    screenSettingCache_ = screenConfig[curRefreshRateMode];
}

void HgmMultiAppStrategy::UseStrategyNum()
{
    auto &strategyConfigs = GetStrategyConfigs();
    auto &strategyName = GetScreenSetting().multiAppStrategyName;
    if (strategyConfigs.find(strategyName) != strategyConfigs.end()) {
        voteRes_.first = EXEC_SUCCESS;
        voteRes_.second = strategyConfigs.at(strategyName);
        UpdateStrategyByTouch(voteRes_.second, touchInfo_.first);
    }
}

void HgmMultiAppStrategy::FollowFocus()
{
    if (pkgs_.empty()) {
        return;
    }

    auto &appConfigMap = GetScreenSetting().appList;
    auto &strategyConfigs = GetStrategyConfigs();

    auto [pkgName, pid] = AnalyzePkgParam(pkgs_.front());
    if (appConfigMap.find(pkgName) != appConfigMap.end()) {
        auto &strategyName = appConfigMap.at(pkgName);
        if (strategyConfigs.find(strategyName) != strategyConfigs.end()) {
            voteRes_.first = EXEC_SUCCESS;
            voteRes_.second = strategyConfigs.at(strategyName);
            UpdateStrategyByTouch(voteRes_.second, pkgName);
        } else {
            HGM_LOGE("[xml] not find strategy: %{public}s", strategyName.c_str());
        }
    } else {
        HGM_LOGD("[xml] not find pkg cfg: %{public}s", pkgName.c_str());
    }
}

void HgmMultiAppStrategy::UseMax()
{
    auto &appConfigMap = GetScreenSetting().appList;
    auto &strategyConfigs = GetStrategyConfigs();
    for (auto &param : pkgs_) {
        auto [pkgName, pid] = AnalyzePkgParam(param);
        if (appConfigMap.find(pkgName) == appConfigMap.end()) {
            continue;
        }
        auto &strategyName = appConfigMap.at(pkgName);
        if (strategyConfigs.find(strategyName) == strategyConfigs.end()) {
            HGM_LOGE("[xml] not find strategy: %{public}s", strategyName.c_str());
            continue;
        }
        auto isVoteSuccess = voteRes_.first;
        auto pkgStrategyConfig = strategyConfigs.at(strategyName);
        UpdateStrategyByTouch(pkgStrategyConfig, pkgName);
        HGM_LOGD("app %{public}s res: [%{public}d, %{public}d]",
            pkgName.c_str(), voteRes_.second.min, voteRes_.second.max);
        if (isVoteSuccess != EXEC_SUCCESS) {
            voteRes_.first = EXEC_SUCCESS;
            voteRes_.second = pkgStrategyConfig;
            continue;
        }
        auto &strategyRes = voteRes_.second;
        strategyRes.min = strategyRes.min > pkgStrategyConfig.min ? strategyRes.min : pkgStrategyConfig.min;
        strategyRes.max = strategyRes.max > pkgStrategyConfig.max ? strategyRes.max : pkgStrategyConfig.max;
    }
}

HgmErrCode HgmMultiAppStrategy::GetPkgNameByPid(pid_t pid, std::string& pkgNameRes) const
{
    if (auto iter = pidPkgNameMap_.find(pid); iter != pidPkgNameMap_.end()) {
        pkgNameRes = iter->second;
        return EXEC_SUCCESS;
    }
    return HGM_ERROR;
}

std::pair<std::string, pid_t> HgmMultiAppStrategy::AnalyzePkgParam(const std::string& param)
{
    std::string pkgName = param;
    pid_t pid = DEFAULT_PID;

    auto index = param.find(":");
    if (index != std::string::npos) {
        pkgName = param.substr(0, index);
        auto pidStr = param.substr(index+1, param.size());
        if (XMLParser::IsNumber(pidStr)) {
            pid = static_cast<pid_t>(std::stoi(pidStr));
        }
    }
    return std::pair<std::string, pid_t>(pkgName, pid);
}

void HgmMultiAppStrategy::UpdateStrategyByTouch(
    PolicyConfigData::StrategyConfig& strategy, const std::string& pkgName, bool forceUpdate)
{
    if (strategy.dynamicMode == DynamicModeType::TOUCH_DISENABLED) {
        return;
    }
    if (uniqueTouchInfo_ == nullptr) {
        return;
    }

    if (forceUpdate) {
        // click pkg which not config
        HGM_LOGD("force update touch info");
        auto touchInfo = std::move(uniqueTouchInfo_);
        if (touchInfo->second != TouchState::IDLE) {
            strategy.min = OLED_120_HZ;
            strategy.max = OLED_120_HZ;
        }
    } else {
        if (pkgName != uniqueTouchInfo_->first) {
            return;
        }
        auto touchInfo = std::move(uniqueTouchInfo_);
        if (touchInfo->second != TouchState::IDLE) {
            strategy.min = strategy.down;
            strategy.max = strategy.down;
            voteRes_.first = EXEC_SUCCESS;
        }
    }
}

void HgmMultiAppStrategy::OnStrategyChange()
{
    HGM_LOGI("multi app strategy change: [%{public}d, %{public}d]", voteRes_.second.min, voteRes_.second.max);
    for (auto &callback : strategyChangeCallbacks_) {
        if (callback != nullptr) {
            HgmTaskHandleThread::Instance().PostTask([callback = callback, strategy = voteRes_.second] () {
                callback(strategy);
            });
        }
    }
}
} // namespace Rosen
} // namespace OHOS