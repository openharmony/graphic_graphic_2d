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

#include <functional>
#include <limits>

#include "common/rs_common_hook.h"
#include "hgm_config_callback_manager.h"
#include "hgm_core.h"
#include "hgm_energy_consumption_policy.h"
#include "hgm_frame_rate_manager.h"
#include "rs_trace.h"
#include "xml_parser.h"

namespace OHOS {
namespace Rosen {
namespace {
    static PolicyConfigData::ScreenSetting defaultScreenSetting;
    static PolicyConfigData::StrategyConfigMap defaultStrategyConfigMap;
    static PolicyConfigData::StrategyConfigMap defaultAppStrategyConfigMap;
    static PolicyConfigData::StrategyConfigMap defaultAppStrategyConfigPreMap;
    static std::vector<std::string> defaultAppBufferList;
    const std::string NULL_STRATEGY_CONFIG_NAME = "null";
    using AppStrategyConfigHandleFunc = std::function<void(const std::string&, PolicyConfigData::StrategyConfig&)>;
    const std::unordered_map<std::string, AppStrategyConfigHandleFunc> APP_STRATEGY_CONFIG_HANDLE_MAP = {
        {"min", [](const std::string& value, PolicyConfigData::StrategyConfig& appStrategyConfig) {
            appStrategyConfig.min = XMLParser::IsNumber(value) ? std::stoi(value) : appStrategyConfig.min; }},
        {"max", [](const std::string& value, PolicyConfigData::StrategyConfig& appStrategyConfig) {
            appStrategyConfig.max = XMLParser::IsNumber(value) ? std::stoi(value) : appStrategyConfig.max;  }},
        {"dynamicMode", [](const std::string& value, PolicyConfigData::StrategyConfig& appStrategyConfig) {
            appStrategyConfig.dynamicMode = XMLParser::IsNumber(value) ?
                static_cast<DynamicModeType>(std::stoi(value)) : appStrategyConfig.dynamicMode; }},
        {"isFactor", [](const std::string& value, PolicyConfigData::StrategyConfig& appStrategyConfig) {
            appStrategyConfig.isFactor = value == "1"; }},
        {"drawMin", [](const std::string& value, PolicyConfigData::StrategyConfig& appStrategyConfig) {
            appStrategyConfig.drawMin = XMLParser::IsNumber(value) ? std::stoi(value) : 0; }},
        {"drawMax", [](const std::string& value, PolicyConfigData::StrategyConfig& appStrategyConfig) {
            appStrategyConfig.drawMax = XMLParser::IsNumber(value) ? std::stoi(value) : 0; }},
        {"down", [](const std::string& value, PolicyConfigData::StrategyConfig& appStrategyConfig) {
            appStrategyConfig.drawMax = XMLParser::IsNumber(value) ? std::stoi(value) : appStrategyConfig.max; }},
    };
}

HgmMultiAppStrategy::HgmMultiAppStrategy()
    : screenSettingCache_(defaultScreenSetting), strategyConfigMapCache_(defaultStrategyConfigMap),
    appStrategyConfigMapCache_(defaultAppStrategyConfigMap),
    appStrategyConfigMapPreCache_(defaultAppStrategyConfigPreMap), appBufferListCache_(defaultAppBufferList)
{
}

HgmErrCode HgmMultiAppStrategy::HandlePkgsEvent(const std::vector<std::string>& pkgs)
{
    RS_TRACE_FUNC();
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
    if (auto configCallbackManager = HgmConfigCallbackManager::GetInstance(); configCallbackManager != nullptr) {
        configCallbackManager->SyncHgmConfigChangeCallback(foregroundPidAppMap_);
    }

    if (!pkgs_.empty()) {
        touchInfo_.pkgName = std::get<0>(AnalyzePkgParam(pkgs_.front()));
    }
    touchInfo_.upExpectFps = OLED_NULL_HZ;
    CalcVote();

    return EXEC_SUCCESS;
}

void HgmMultiAppStrategy::HandleTouchInfo(const TouchInfo& touchInfo)
{
    RS_TRACE_NAME_FMT("[HandleTouchInfo] pkgName:%s, touchState:%d, upExpectFps:%d",
        touchInfo.pkgName.c_str(), touchInfo.touchState, touchInfo.upExpectFps);
    HGM_LOGD("touch info update, pkgName:%{public}s, touchState:%{public}d, upExpectFps:%{public}d",
        touchInfo.pkgName.c_str(), touchInfo.touchState, touchInfo.upExpectFps);
    touchInfo_ = { touchInfo.pkgName, touchInfo.touchState, touchInfo.upExpectFps };
    if (touchInfo.pkgName == "" && !pkgs_.empty()) {
        auto [focusPkgName, pid, appType] = AnalyzePkgParam(pkgs_.front());
        touchInfo_.pkgName = focusPkgName;
        HGM_LOGD("auto change touch pkgName to focusPkgName:%{public}s", focusPkgName.c_str());
    }
    CalcVote();
}

void HgmMultiAppStrategy::HandleLightFactorStatus(int32_t state)
{
    RS_TRACE_NAME_FMT("[HandleLightFactorStatus] state: %d", state);
    if (lightFactorStatus_.load() == state) {
        return;
    }
    lightFactorStatus_.store(state);
    CalcVote();
}

void HgmMultiAppStrategy::SetScreenType(bool isLtpo)
{
    isLtpo_ = isLtpo;
}

void HgmMultiAppStrategy::HandleLowAmbientStatus(bool isEffect)
{
    RS_TRACE_NAME_FMT("[HandleLowAmbientStatus] isEffect: %d", isEffect);
    if (lowAmbientStatus_ == isEffect) {
        return;
    }
    lowAmbientStatus_ = isEffect;
}

void HgmMultiAppStrategy::CalcVote()
{
    RS_TRACE_FUNC();
    HgmTaskHandleThread::Instance().DetectMultiThreadingCalls();
    voteRes_ = { HGM_ERROR, {
        .min = OLED_NULL_HZ, .max = OLED_120_HZ, .dynamicMode = DynamicModeType::TOUCH_ENABLED,
        .idleFps = OLED_60_HZ, .isFactor = false, .drawMin = OLED_NULL_HZ,
        .drawMax = OLED_120_HZ, .down = OLED_120_HZ,
    }};
    uniqueTouchInfo_ = std::make_unique<TouchInfo>(touchInfo_);

    if (pkgs_.size() <= 1) {
        FollowFocus();
    } else {
        switch (screenSettingCache_.multiAppStrategyType) {
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
        if (GetAppStrategyConfig("", voteRes_.second) == EXEC_SUCCESS) {
            OnLightFactor(voteRes_.second);
        }
    }

    UpdateStrategyByTouch(voteRes_.second, "", true);
    uniqueTouchInfo_ = nullptr;

    OnStrategyChange();
}

HgmErrCode HgmMultiAppStrategy::GetVoteRes(PolicyConfigData::StrategyConfig& strategyRes) const
{
    strategyRes = voteRes_.second;
    return voteRes_.first;
}

bool HgmMultiAppStrategy::CheckPidValid(pid_t pid, bool onlyCheckForegroundApp)
{
    auto configData = HgmCore::Instance().GetPolicyConfigData();
    if ((configData != nullptr && !configData->safeVoteEnabled) || disableSafeVote_) {
        // disable safe vote
        return true;
    }
    if (onlyCheckForegroundApp) {
        return foregroundPidAppMap_.find(pid) != foregroundPidAppMap_.end();
    }
    return !backgroundPid_.Existed(pid);
}

std::string HgmMultiAppStrategy::GetGameNodeName(const std::string& pkgName)
{
    auto &appNodeMap = screenSettingCache_.gameAppNodeList;
    if (appNodeMap.find(pkgName) != appNodeMap.end()) {
        return appNodeMap.at(pkgName);
    }
    return "";
}

std::string HgmMultiAppStrategy::GetAppStrategyConfigName(const std::string& pkgName)
{
    auto &appConfigMap = screenSettingCache_.appList;
    if (appConfigMap.find(pkgName) != appConfigMap.end()) {
        return appConfigMap.at(pkgName);
    }
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
    auto [pkgName, pid, appType] = AnalyzePkgParam(pkgs_.empty() ? "" : pkgs_.front());
    return GetAppStrategyConfig(pkgName, strategyRes);
}

void HgmMultiAppStrategy::CleanApp(pid_t pid)
{
    foregroundPidAppMap_.erase(pid);
    backgroundPid_.Erase(pid);
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
    appBufferListCache_ = configData->appBufferList_;

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
    appStrategyConfigMapCache_.clear();
    appStrategyConfigMapPreCache_.clear();
}

HgmErrCode HgmMultiAppStrategy::GetStrategyConfig(
    const std::string& strategyName, PolicyConfigData::StrategyConfig& strategyRes)
{
    if (strategyConfigMapCache_.find(strategyName) != strategyConfigMapCache_.end()) {
        strategyRes = strategyConfigMapCache_.at(strategyName);
        return EXEC_SUCCESS;
    }
    return HGM_ERROR;
}

HgmErrCode HgmMultiAppStrategy::GetAppStrategyConfig(
    const std::string& pkgName, PolicyConfigData::StrategyConfig& strategyRes)
{
    if (appStrategyConfigMapCache_.find(pkgName) != appStrategyConfigMapCache_.end()) {
        strategyRes = appStrategyConfigMapCache_.at(pkgName);
        return EXEC_SUCCESS;
    }
    return GetStrategyConfig(GetAppStrategyConfigName(pkgName), strategyRes);
}

void HgmMultiAppStrategy::UseStrategyNum()
{
    auto &strategyName = screenSettingCache_.multiAppStrategyName;
    RS_TRACE_NAME_FMT("[UseStrategyNum] strategyName:%s", strategyName.c_str());
    if (strategyConfigMapCache_.find(strategyName) != strategyConfigMapCache_.end()) {
        voteRes_.first = EXEC_SUCCESS;
        voteRes_.second = strategyConfigMapCache_.at(strategyName);
        OnLightFactor(voteRes_.second);
        UpdateStrategyByTouch(voteRes_.second, touchInfo_.pkgName);
    }
}

void HgmMultiAppStrategy::FollowFocus()
{
    RS_TRACE_FUNC();
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
    HGM_LOGD("lightFactorStatus:%{public}d, isFactor:%{public}u, lowAmbientStatus:%{public}u",
        lightFactorStatus_.load(), strategyRes.isFactor, lowAmbientStatus_);
    if (!isLtpo_ && lowAmbientStatus_ && lightFactorStatus_.load() == LightFactorStatus::LOW_LEVEL) {
        strategyRes.min = strategyRes.max;
        return;
    }
    if (lightFactorStatus_.load() == LightFactorStatus::NORMAL_LOW && strategyRes.isFactor && !lowAmbientStatus_) {
        RS_TRACE_NAME_FMT("OnLightFactor, strategy change: min -> max");
        strategyRes.min = strategyRes.max;
    }
}

void HgmMultiAppStrategy::UpdateStrategyByTouch(
    PolicyConfigData::StrategyConfig& strategy, const std::string& pkgName, bool forceUpdate)
{
    auto frameRateMgr = HgmCore::Instance().GetFrameRateMgr();
    if (uniqueTouchInfo_ == nullptr || frameRateMgr == nullptr) {
        return;
    }

    if (uniqueTouchInfo_->touchState == TouchState::DOWN_STATE &&
        (!HgmCore::Instance().GetEnableDynamicMode() || strategy.dynamicMode == DynamicModeType::TOUCH_DISENABLED)) {
        return;
    }

    if (uniqueTouchInfo_->touchState == TouchState::IDLE_STATE) {
        uniqueTouchInfo_ = nullptr;
        frameRateMgr->HandleRefreshRateEvent(DEFAULT_PID, {"VOTER_TOUCH", false});
        return;
    }

    auto voteTouchFunc = [this, frameRateMgr] (const PolicyConfigData::StrategyConfig& strategy) {
        auto touchInfo = std::move(uniqueTouchInfo_);
        if (touchInfo->touchState == TouchState::DOWN_STATE) {
            RS_TRACE_NAME_FMT("[UpdateStrategyByTouch] pkgName:%s, state:%d, downFps:%d",
                touchInfo->pkgName.c_str(), touchInfo->touchState, strategy.down);
            frameRateMgr->HandleRefreshRateEvent(DEFAULT_PID, {"VOTER_TOUCH", true, strategy.down, strategy.down});
        } else if (touchInfo->touchState == TouchState::UP_STATE && touchInfo->upExpectFps > 0) {
            RS_TRACE_NAME_FMT("[UpdateStrategyByTouch] pkgName:%s, state:%d, upExpectFps:%d",
                touchInfo->pkgName.c_str(), touchInfo->touchState, touchInfo->upExpectFps);
            frameRateMgr->HandleRefreshRateEvent(DEFAULT_PID,
                {"VOTER_TOUCH", true, touchInfo->upExpectFps, touchInfo->upExpectFps});
        }
    };

    if (forceUpdate) {
        // click pkg which not config
        HGM_LOGD("force update touch info");
        PolicyConfigData::StrategyConfig settingStrategy;
        if (GetAppStrategyConfig("", settingStrategy) != EXEC_SUCCESS ||
            settingStrategy.dynamicMode == DynamicModeType::TOUCH_DISENABLED) {
            return;
        }
        voteTouchFunc(settingStrategy);
    } else {
        if (uniqueTouchInfo_->touchState == TouchState::DOWN_STATE && pkgName != uniqueTouchInfo_->pkgName) {
            return;
        }
        voteTouchFunc(strategy);
    }
}

void HgmMultiAppStrategy::OnStrategyChange()
{
    HGM_LOGD("multi app strategy change: [%{public}d, %{public}d]", voteRes_.second.min, voteRes_.second.max);
    for (const auto &callback : strategyChangeCallbacks_) {
        if (callback != nullptr) {
            callback(voteRes_.second);
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
    rsCommonHook.SetIsWhiteListForSolidColorLayerFlag(false);
    std::unordered_map<std::string, std::string>& videoConfigFromHgm = configData->sourceTuningConfig_;
    std::unordered_map<std::string, std::string>& solidLayerConfigFromHgm = configData->solidLayerConfig_;
    std::unordered_map<std::string, std::string>& hwcVideoConfigFromHgm = configData->hwcSourceTuningConfig_;
    std::unordered_map<std::string, std::string>& hwcSolidLayerConfigFromHgm = configData->hwcSolidLayerConfig_;
    HgmEnergyConsumptionPolicy::Instance().SetCurrentPkgName(pkgs);
    if (pkgs.size() > 1) {
        return;
    }
    for (auto &param: pkgs) {
        std::string pkgNameForCheck = param.substr(0, param.find(':'));
        // 1 means crop source tuning
        auto videoIter = videoConfigFromHgm.find(pkgNameForCheck);
        auto hwcVideoIter = hwcVideoConfigFromHgm.find(pkgNameForCheck);
        if ((videoIter != videoConfigFromHgm.end() && videoIter->second == "1") ||
            (hwcVideoIter != hwcVideoConfigFromHgm.end() && hwcVideoIter->second == "1")) {
            rsCommonHook.SetVideoSurfaceFlag(true);
        // 2 means skip hardware disabled by hwc node and background alpha
        } else if ((videoIter != videoConfigFromHgm.end() && videoIter->second == "2") ||
                   (hwcVideoIter != hwcVideoConfigFromHgm.end() && hwcVideoIter->second == "2")) {
            rsCommonHook.SetHardwareEnabledByHwcnodeBelowSelfInAppFlag(true);
            rsCommonHook.SetHardwareEnabledByBackgroundAlphaFlag(true);
        }
        // 1 means enable dss by solid color layer
        auto iter = solidLayerConfigFromHgm.find(pkgNameForCheck);
        auto hwcIter = hwcSolidLayerConfigFromHgm.find(pkgNameForCheck);
        if ((iter != solidLayerConfigFromHgm.end() && iter->second == "1") ||
            (hwcIter != hwcSolidLayerConfigFromHgm.end() && hwcIter->second == "1")) {
            rsCommonHook.SetIsWhiteListForSolidColorLayerFlag(true);
        }
    }
}

void HgmMultiAppStrategy::HandleAppBufferStrategy(const std::string& configName, const std::string& configValue,
    PolicyConfigData::StrategyConfig& appStrategyConfig)
{
    auto it = find(appBufferListCache_.begin(), appBufferListCache_.end(), configName);
    if (it == appBufferListCache_.end()) {
        return;
    }
    if (!XMLParser::IsNumber(configValue)) {
        return;
    }
    appStrategyConfig.bufferFpsMap[configName] = std::stoi(configValue);
}

void HgmMultiAppStrategy::SetAppStrategyConfig(
    const std::string& pkgName, const std::vector<std::pair<std::string, std::string>>& newConfig)
{
    if (pkgName.empty()) {
        HGM_LOGI("pkgName is empty, clear all config");
        appStrategyConfigMapPreCache_.clear();
        appStrategyConfigMapChanged_ = true;
        return;
    }

    if (newConfig.empty()) {
        HGM_LOGI("newConfig is empty, clear %{public}s config", pkgName.c_str());
        appStrategyConfigMapPreCache_.erase(pkgName);
        appStrategyConfigMapChanged_ = true;
        return;
    }

    PolicyConfigData::StrategyConfig appStrategyConfig;
    if (GetAppStrategyConfig(pkgName, appStrategyConfig) != EXEC_SUCCESS) {
        HGM_LOGE("not found  %{public}s strategy config", pkgName.c_str());
        return;
    }
    for (const auto& [configName, configValue] : newConfig) {
        HGM_LOGD("handle %{public}s config %{public}s = %{public}s",
            pkgName.c_str(), configName.c_str(), configValue.c_str());
        if (APP_STRATEGY_CONFIG_HANDLE_MAP.find(configName) != APP_STRATEGY_CONFIG_HANDLE_MAP.end()) {
            APP_STRATEGY_CONFIG_HANDLE_MAP.at(configName)(configValue, appStrategyConfig);
        } else {
            HandleAppBufferStrategy(configName, configValue, appStrategyConfig);
        }
    }
    appStrategyConfigMapPreCache_[pkgName] = appStrategyConfig;
    appStrategyConfigMapChanged_ = true;
}

void HgmMultiAppStrategy::UpdateAppStrategyConfigCache()
{
    if (appStrategyConfigMapChanged_) {
        HGM_LOGI("UpdateAppStrategyConfigCache on power touch idle");
        appStrategyConfigMapCache_ = appStrategyConfigMapPreCache_;
        appStrategyConfigMapChanged_ = false;
    }
}

} // namespace Rosen
} // namespace OHOS