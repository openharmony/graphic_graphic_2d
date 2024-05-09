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
    static std::unordered_map<std::string, std::string> defaultAppTypes;
    const std::string NULL_STRATEGY_CONFIG_NAME = "null";
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
    pidAppTypeMap_.clear();
    for (auto &param : pkgs_) {
        HGM_LOGI("pkg update:%{public}s", param.c_str());
        auto [pkgName, pid, appType] = AnalyzePkgParam(param);
        pidAppTypeMap_[pkgName] = { pid, appType };
    }

    CalcVote();

    return EXEC_SUCCESS;
}

void HgmMultiAppStrategy::HandleTouchInfo(const std::string& pkgName, TouchState touchState)
{
    HGM_LOGD("touch info update, pkgName:%{public}s, touchState:%{public}d", pkgName.c_str(), touchState);
    touchInfo_ = { pkgName, touchState };
    if (pkgName == "" && !pkgs_.empty()) {
        auto [focusPkgName, pid, appType] = AnalyzePkgParam(pkgs_.front());
        touchInfo_.first = focusPkgName;
    }
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

std::string HgmMultiAppStrategy::GetAppStrategyConfigName(const std::string& pkgName) const
{
    auto &appConfigMap = GetScreenSetting().appList;
    if (appConfigMap.find(pkgName) != appConfigMap.end()) {
        return appConfigMap.at(pkgName);
    }

    if (pidAppTypeMap_.find(pkgName) != pidAppTypeMap_.end()) {
        auto &appType = pidAppTypeMap_.at(pkgName).second;
        auto &appTypes = GetScreenSetting().appTypes;
        if (appTypes.find(appType) != appTypes.end()) {
            return appTypes.at(appType);
        }
    }

    return NULL_STRATEGY_CONFIG_NAME;
}

HgmErrCode HgmMultiAppStrategy::GetScreenSettingMode(PolicyConfigData::StrategyConfig& strategyRes)
{
    auto &strategyConfigs = GetStrategyConfigs();
    auto &strategyName = GetScreenSetting().strategy;
    if (strategyConfigs.find(strategyName) != strategyConfigs.end()) {
        strategyRes = strategyConfigs.at(strategyName);
        return EXEC_SUCCESS;
    }

    return HGM_ERROR;
}

HgmErrCode HgmMultiAppStrategy::GetFocusAppStrategyConfig(PolicyConfigData::StrategyConfig& strategyRes)
{
    if (pkgs_.empty()) {
        return HGM_ERROR;
    }

    auto &strategyConfigs = GetStrategyConfigs();

    auto [pkgName, pid, appType] = AnalyzePkgParam(pkgs_.front());
    auto strategyName = GetAppStrategyConfigName(pkgName);
    if (strategyName != NULL_STRATEGY_CONFIG_NAME && strategyConfigs.find(strategyName) != strategyConfigs.end()) {
        strategyRes = strategyConfigs.at(strategyName);
        return EXEC_SUCCESS;
    }

    return HGM_ERROR;
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

    auto &strategyConfigs = GetStrategyConfigs();

    auto [pkgName, pid, appType] = AnalyzePkgParam(pkgs_.front());
    auto strategyName = GetAppStrategyConfigName(pkgName);
    if (strategyName != NULL_STRATEGY_CONFIG_NAME && strategyConfigs.find(strategyName) != strategyConfigs.end()) {
        voteRes_.first = EXEC_SUCCESS;
        voteRes_.second = strategyConfigs.at(strategyName);
        UpdateStrategyByTouch(voteRes_.second, pkgName);
    } else {
        HGM_LOGD("[xml] not find pkg cfg: %{public}s", pkgName.c_str());
    }
}

void HgmMultiAppStrategy::UseMax()
{
    auto &strategyConfigs = GetStrategyConfigs();
    for (auto &param : pkgs_) {
        auto [pkgName, pid, appType] = AnalyzePkgParam(param);
        auto strategyName = GetAppStrategyConfigName(pkgName);
        if (strategyName == NULL_STRATEGY_CONFIG_NAME || strategyConfigs.find(strategyName) == strategyConfigs.end()) {
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

std::tuple<std::string, pid_t, std::string> HgmMultiAppStrategy::AnalyzePkgParam(const std::string& param)
{
    std::string pkgName = param;
    pid_t pid = DEFAULT_PID;
    std::string appType = "";

    auto index = param.find(":");
    if (index == std::string::npos) {
        return std::tuple<std::string, pid_t, std::string>(pkgName, pid, appType);
    }

    // split by : index
    pkgName = param.substr(0, index);
    auto pidAppType = param.substr(index + 1, param.size());
    index = pidAppType.find(":");
    if (index == std::string::npos) {
        if (XMLParser::IsNumber(pidAppType)) {
            pid = static_cast<pid_t>(std::stoi(pidAppType));
        }
        return std::tuple<std::string, pid_t, std::string>(pkgName, pid, appType);
    }

    // split by : index
    auto pidStr = pidAppType.substr(0, index);
    appType = pidAppType.substr(index + 1, pidAppType.size());

    if (XMLParser::IsNumber(pidStr)) {
        pid = static_cast<pid_t>(std::stoi(pidStr));
    }

    return std::tuple<std::string, pid_t, std::string>(pkgName, pid, appType);
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
        if (touchInfo->second != TouchState::IDLE_STATE) {
            strategy.min = OLED_120_HZ;
            strategy.max = OLED_120_HZ;
        }
    } else {
        if (pkgName != uniqueTouchInfo_->first) {
            return;
        }
        auto touchInfo = std::move(uniqueTouchInfo_);
        if (touchInfo->second != TouchState::IDLE_STATE) {
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