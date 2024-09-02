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

#include "common/rs_common_hook.h"
#include "hgm_core.h"
#include "hgm_frame_rate_manager.h"
#include "rs_trace.h"
#include "xml_parser.h"

namespace OHOS {
namespace Rosen {
namespace {
    static PolicyConfigData::ScreenSetting defaultScreenSetting;
    static PolicyConfigData::StrategyConfigMap defaultStrategyConfigMap;
    const std::string NULL_STRATEGY_CONFIG_NAME = "null";
}

HgmMultiAppStrategy::HgmMultiAppStrategy()
    : screenSettingCache_(defaultScreenSetting), strategyConfigMapCache_(defaultStrategyConfigMap)
{
    handler_ = HgmTaskHandleThread::Instance().CreateHandler();
}

HgmErrCode HgmMultiAppStrategy::HandlePkgsEvent(const std::vector<std::string>& pkgs)
{
    RS_TRACE_FUNC();
    {
        std::lock_guard<std::mutex> pkgsLock(pkgsMutex_);
        std::lock_guard<std::mutex> lock(pidAppTypeMutex_);
        // update pkgs
        if (pkgs_ == pkgs) {
            return HGM_ERROR;
        }
        pkgs_ = pkgs;
        // update pid of pkg
        for (auto &it : foregroundPidAppMap_) {
            backgroundPid_.Put(it.first);
        }
        foregroundPidAppMap_.clear();
        pidAppTypeMap_.clear();
        CheckPackageInConfigList(pkgs_);
        for (auto &param : pkgs_) {
            RS_TRACE_NAME_FMT("pkg update:%s", param.c_str());
            HGM_LOGI("pkg update:%{public}s", param.c_str());
            auto [pkgName, pid, appType] = AnalyzePkgParam(param);
            pidAppTypeMap_[pkgName] = { pid, appType };
            if (pid > DEFAULT_PID) {
                foregroundPidAppMap_[pid] = { appType, pkgName };
                backgroundPid_.Erase(pid);
            }
        }
    }

    CalcVote();

    return EXEC_SUCCESS;
}

void HgmMultiAppStrategy::HandleTouchInfo(const TouchInfo& touchInfo)
{
    RS_TRACE_NAME_FMT("[HandleTouchInfo] pkgName:%s, touchState:%d",
        touchInfo.pkgName.c_str(), touchInfo.touchState);
    HGM_LOGD("touch info update, pkgName:%{public}s, touchState:%{public}d",
        touchInfo.pkgName.c_str(), touchInfo.touchState);
    {
        std::lock_guard<std::mutex> lock(touchInfoMutex_);
        touchInfo_ = { touchInfo.pkgName, touchInfo.touchState, touchInfo.upExpectFps };
        std::lock_guard<std::mutex> pkgsLock(pkgsMutex_);
        if (touchInfo.pkgName == "" && !pkgs_.empty()) {
            auto [focusPkgName, pid, appType] = AnalyzePkgParam(pkgs_.front());
            touchInfo_.pkgName = focusPkgName;
            HGM_LOGD("auto change touch pkgName to focusPkgName:%{public}s", focusPkgName.c_str());
        }
    }
    CalcVote();
}

void HgmMultiAppStrategy::HandleLightFactorStatus(bool isSafe)
{
    RS_TRACE_NAME_FMT("[HandleLightFactorStatus] isSafe: %d", isSafe);
    if (lightFactorStatus_.load() == isSafe) {
        return;
    }
    lightFactorStatus_.store(isSafe);
    CalcVote();
}

void HgmMultiAppStrategy::CalcVote()
{
    RS_TRACE_FUNC();
    static std::mutex calcVoteMutex;
    std::unique_lock<std::mutex> lock(calcVoteMutex);
    voteRes_ = { HGM_ERROR, {
        .min = OledRefreshRate::OLED_NULL_HZ,
        .max = OledRefreshRate::OLED_120_HZ,
        .dynamicMode = DynamicModeType::TOUCH_ENABLED,
        .isFactor = false,
        .drawMin = OledRefreshRate::OLED_NULL_HZ,
        .drawMax = OledRefreshRate::OLED_120_HZ,
        .down = OledRefreshRate::OLED_120_HZ,
    }};
    {
        std::lock_guard<std::mutex> lock(touchInfoMutex_);
        uniqueTouchInfo_ = std::make_unique<TouchInfo>(touchInfo_);
    }

    MultiAppStrategyType multiAppStrategyType = MultiAppStrategyType::USE_MAX;
    {
        std::unique_lock<std::mutex> lock(updateCacheMutex_);
        multiAppStrategyType = screenSettingCache_.multiAppStrategyType;
    }
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

    if (voteRes_.first != EXEC_SUCCESS) {
        // using setting mode when fail to calc vote
        if (GetAppStrategyConfig("", voteRes_.second) == EXEC_SUCCESS) {
            OnLightFactor(voteRes_.second);
        }
    }

    UpdateStrategyByTouch(voteRes_.second, "", true);
    uniqueTouchInfo_ = nullptr;

    HGM_LOGD("final apps res: %{public}d, [%{public}d, %{public}d]",
        voteRes_.first, voteRes_.second.min, voteRes_.second.max);

    OnStrategyChange();
}

HgmErrCode HgmMultiAppStrategy::GetVoteRes(PolicyConfigData::StrategyConfig& strategyRes) const
{
    strategyRes = voteRes_.second;
    return voteRes_.first;
}

void HgmMultiAppStrategy::RegisterStrategyChangeCallback(const StrategyChangeCallback& callback)
{
    strategyChangeCallbacks_.emplace_back(callback);
}

bool HgmMultiAppStrategy::CheckPidValid(pid_t pid)
{
    auto configData = HgmCore::Instance().GetPolicyConfigData();
    if (configData != nullptr && !configData->safeVoteEnabled) {
        // disable safe vote
        return true;
    }
    std::lock_guard<std::mutex> lock(pidAppTypeMutex_);
    return !backgroundPid_.Existed(pid);
}

std::string HgmMultiAppStrategy::GetAppStrategyConfigName(const std::string& pkgName)
{
    std::unique_lock<std::mutex> cacheLock(updateCacheMutex_);
    auto &appConfigMap = screenSettingCache_.appList;
    if (appConfigMap.find(pkgName) != appConfigMap.end()) {
        return appConfigMap.at(pkgName);
    }
    std::lock_guard<std::mutex> lock(pidAppTypeMutex_);
    if (pidAppTypeMap_.find(pkgName) != pidAppTypeMap_.end()) {
        auto &appType = pidAppTypeMap_.at(pkgName).second;
        auto &appTypes = screenSettingCache_.appTypes;
        if (appTypes.find(appType) != appTypes.end()) {
            return appTypes.at(appType);
        }
    }

    return screenSettingCache_.strategy;
}

HgmErrCode HgmMultiAppStrategy::GetFocusAppStrategyConfig(PolicyConfigData::StrategyConfig& strategyRes)
{
    std::lock_guard<std::mutex> lock(pkgsMutex_);
    auto [pkgName, pid, appType] = AnalyzePkgParam(pkgs_.empty() ? "" : pkgs_.front());
    return GetAppStrategyConfig(pkgName, strategyRes);
}

std::unordered_map<std::string, std::pair<pid_t, int32_t>> HgmMultiAppStrategy::GetPidAppType()
{
    std::lock_guard<std::mutex> lock(pidAppTypeMutex_);
    return pidAppTypeMap_;
}

std::unordered_map<pid_t, std::pair<int32_t, std::string>> HgmMultiAppStrategy::GetForegroundPidApp()
{
    std::lock_guard<std::mutex> lock(pidAppTypeMutex_);
    return foregroundPidAppMap_;
}

HgmLRUCache<pid_t> HgmMultiAppStrategy::GetBackgroundPid()
{
    std::lock_guard<std::mutex> lock(pidAppTypeMutex_);
    return backgroundPid_;
}

std::vector<std::string> HgmMultiAppStrategy::GetPackages()
{
    std::lock_guard<std::mutex> lock(pkgsMutex_);
    return pkgs_;
}

void HgmMultiAppStrategy::CleanApp(pid_t pid)
{
    std::lock_guard<std::mutex> lock(pidAppTypeMutex_);
    foregroundPidAppMap_.erase(pid);
    backgroundPid_.Erase(pid);
}

void HgmMultiAppStrategy::UpdateXmlConfigCache()
{
    std::unique_lock<std::mutex> lock(updateCacheMutex_);
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

PolicyConfigData::ScreenSetting HgmMultiAppStrategy::GetScreenSetting()
{
    std::unique_lock<std::mutex> lock(updateCacheMutex_);
    return screenSettingCache_;
}

void HgmMultiAppStrategy::SetScreenSetting(const PolicyConfigData::ScreenSetting& screenSetting)
{
    std::unique_lock<std::mutex> lock(updateCacheMutex_);
    screenSettingCache_ = screenSetting;
}

PolicyConfigData::StrategyConfigMap HgmMultiAppStrategy::GetStrategyConfigs()
{
    std::unique_lock<std::mutex> lock(updateCacheMutex_);
    return strategyConfigMapCache_;
}

void HgmMultiAppStrategy::SetStrategyConfigs(const PolicyConfigData::StrategyConfigMap& strategyConfigs)
{
    std::unique_lock<std::mutex> lock(updateCacheMutex_);
    strategyConfigMapCache_ = strategyConfigs;
}

HgmErrCode HgmMultiAppStrategy::GetStrategyConfig(
    const std::string& strategyName, PolicyConfigData::StrategyConfig& strategyRes)
{
    std::unique_lock<std::mutex> lock(updateCacheMutex_);
    if (strategyConfigMapCache_.find(strategyName) != strategyConfigMapCache_.end()) {
        strategyRes = strategyConfigMapCache_.at(strategyName);
        return EXEC_SUCCESS;
    }
    return HGM_ERROR;
}

HgmErrCode HgmMultiAppStrategy::GetAppStrategyConfig(
    const std::string& pkgName, PolicyConfigData::StrategyConfig& strategyRes)
{
    return GetStrategyConfig(GetAppStrategyConfigName(pkgName), strategyRes);
}

void HgmMultiAppStrategy::UseStrategyNum()
{
    std::unique_lock<std::mutex> lock(updateCacheMutex_);
    auto &strategyName = screenSettingCache_.multiAppStrategyName;
    RS_TRACE_NAME_FMT("[UseStrategyNum] strategyName:%s", strategyName.c_str());
    if (strategyConfigMapCache_.find(strategyName) != strategyConfigMapCache_.end()) {
        voteRes_.first = EXEC_SUCCESS;
        voteRes_.second = strategyConfigMapCache_.at(strategyName);
        OnLightFactor(voteRes_.second);
        std::lock_guard<std::mutex> lock(touchInfoMutex_);
        UpdateStrategyByTouch(voteRes_.second, touchInfo_.pkgName);
    }
}

void HgmMultiAppStrategy::FollowFocus()
{
    RS_TRACE_FUNC();
    std::lock_guard<std::mutex> lock(pkgsMutex_);
    auto [pkgName, pid, appType] = AnalyzePkgParam(pkgs_.empty() ? "" : pkgs_.front());
    if (voteRes_.first = GetAppStrategyConfig(pkgName, voteRes_.second); voteRes_.first == EXEC_SUCCESS) {
        OnLightFactor(voteRes_.second);
        UpdateStrategyByTouch(voteRes_.second, pkgName);
    } else {
        HGM_LOGD("[xml] not find pkg cfg: %{public}s", pkgName.c_str());
    }
}

void HgmMultiAppStrategy::UseMax()
{
    RS_TRACE_FUNC();
    std::lock_guard<std::mutex> lock(pkgsMutex_);
    PolicyConfigData::StrategyConfig pkgStrategyConfig;
    for (const auto &param : pkgs_) {
        auto [pkgName, pid, appType] = AnalyzePkgParam(param);
        if (GetAppStrategyConfig(pkgName, pkgStrategyConfig) != EXEC_SUCCESS) {
            continue;
        }
        auto isVoteSuccess = voteRes_.first;
        OnLightFactor(pkgStrategyConfig);
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

std::tuple<std::string, pid_t, int32_t> HgmMultiAppStrategy::AnalyzePkgParam(const std::string& param)
{
    std::string pkgName = param;
    pid_t pid = DEFAULT_PID;
    int32_t appType = DEFAULT_APP_TYPE;

    auto index = param.find(":");
    if (index == std::string::npos) {
        return { pkgName, pid, appType };
    }

    // split by : index
    pkgName = param.substr(0, index);
    auto pidAppType = param.substr(index + 1, param.size());
    index = pidAppType.find(":");
    if (index == std::string::npos) {
        if (XMLParser::IsNumber(pidAppType)) {
            pid = static_cast<pid_t>(std::stoi(pidAppType));
        }
        return { pkgName, pid, appType };
    }

    // split by : index
    auto pidStr = pidAppType.substr(0, index);
    if (XMLParser::IsNumber(pidStr)) {
        pid = static_cast<pid_t>(std::stoi(pidStr));
    }

    auto appTypeStr = pidAppType.substr(index + 1, pidAppType.size());
    if (XMLParser::IsNumber(appTypeStr)) {
        appType = std::stoi(appTypeStr);
    }

    return { pkgName, pid, appType };
}

void HgmMultiAppStrategy::OnLightFactor(PolicyConfigData::StrategyConfig& strategyRes) const
{
    if (lightFactorStatus_ && strategyRes.isFactor) {
        RS_TRACE_NAME_FMT("OnLightFactor, strategy change: min -> max");
        strategyRes.min = strategyRes.max;
    }
}

void HgmMultiAppStrategy::UpdateStrategyByTouch(
    PolicyConfigData::StrategyConfig& strategy, const std::string& pkgName, bool forceUpdate)
{
    if (!HgmCore::Instance().GetEnableDynamicMode() || strategy.dynamicMode == DynamicModeType::TOUCH_DISENABLED) {
        return;
    }
    if (uniqueTouchInfo_ == nullptr) {
        return;
    }

    if (forceUpdate) {
        // click pkg which not config
        HGM_LOGD("force update touch info");
        PolicyConfigData::StrategyConfig settingStrategy;
        if (GetAppStrategyConfig("", settingStrategy) != EXEC_SUCCESS ||
            settingStrategy.dynamicMode == DynamicModeType::TOUCH_DISENABLED) {
            return;
        }

        auto touchInfo = std::move(uniqueTouchInfo_);
        if (touchInfo->touchState == TouchState::DOWN_STATE) {
            RS_TRACE_NAME_FMT("[UpdateStrategyByTouch] state:%d, downFps:%d force update",
                touchInfo->touchState, strategy.down);
            strategy.min = settingStrategy.down;
            strategy.max = settingStrategy.down;
        } else if (touchInfo->touchState == TouchState::UP_STATE && touchInfo->upExpectFps > 0) {
            RS_TRACE_NAME_FMT("[UpdateStrategyByTouch] state:%d, upExpectFps:%d force update",
                touchInfo->touchState, touchInfo->upExpectFps);
            strategy.min = touchInfo->upExpectFps;
            strategy.max = touchInfo->upExpectFps;
        }
    } else {
        if (pkgName != uniqueTouchInfo_->pkgName) {
            return;
        }
        auto touchInfo = std::move(uniqueTouchInfo_);
        if (touchInfo->touchState == TouchState::DOWN_STATE) {
            RS_TRACE_NAME_FMT("[UpdateStrategyByTouch] pkgName:%s, state:%d, downFps:%d",
                pkgName.c_str(), touchInfo->touchState, strategy.down);
            strategy.min = strategy.down;
            strategy.max = strategy.down;
            voteRes_.first = EXEC_SUCCESS;
        } else if (touchInfo->touchState == TouchState::UP_STATE && touchInfo->upExpectFps > 0) {
            RS_TRACE_NAME_FMT("[UpdateStrategyByTouch] pkgName:%s, state:%d, upExpectFps:%d force update",
                pkgName.c_str(), touchInfo->touchState, touchInfo->upExpectFps);
            strategy.min = touchInfo->upExpectFps;
            strategy.max = touchInfo->upExpectFps;
            voteRes_.first = EXEC_SUCCESS;
        }
    }
}

void HgmMultiAppStrategy::OnStrategyChange()
{
    HGM_LOGD("multi app strategy change: [%{public}d, %{public}d]", voteRes_.second.min, voteRes_.second.max);
    for (const auto &callback : strategyChangeCallbacks_) {
        if (callback != nullptr && handler_ != nullptr) {
            handler_->PostTask([callback, strategy = voteRes_.second] () {
                callback(strategy);
            });
        }
    }
}

// use in temporary scheme to check package name
void HgmMultiAppStrategy::CheckPackageInConfigList(const std::vector<std::string>& pkgs)
{
    auto& rsCommonHook = RsCommonHook::Instance();
    auto& hgmCore = HgmCore::Instance();
    auto configData = hgmCore.GetPolicyConfigData();
    if (configData == nullptr) {
        return;
    }
    rsCommonHook.SetVideoSurfaceFlag(false);
    rsCommonHook.SetHardwareEnabledByHwcnodeBelowSelfInAppFlag(false);
    rsCommonHook.SetHardwareEnabledByBackgroundAlphaFlag(false);
    std::unordered_map<std::string, std::string>& videoConfigFromHgm = configData->sourceTuningConfig_;
    if (videoConfigFromHgm.empty() || pkgs.size() > 1) {
        return;
    }
    for (auto &param: pkgs) {
        std::string pkgNameForCheck = param.substr(0, param.find(':'));
        // 1 means crop source tuning
        if (videoConfigFromHgm[pkgNameForCheck] == "1") {
            rsCommonHook.SetVideoSurfaceFlag(true);
        // 2 means skip hardware disabled by hwc node and background alpha
        } else if (videoConfigFromHgm[pkgNameForCheck] == "2") {
            rsCommonHook.SetHardwareEnabledByHwcnodeBelowSelfInAppFlag(true);
            rsCommonHook.SetHardwareEnabledByBackgroundAlphaFlag(true);
        }
    }
}
} // namespace Rosen
} // namespace OHOS