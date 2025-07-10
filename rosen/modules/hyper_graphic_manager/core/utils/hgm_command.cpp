/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "hgm_command.h"
#include "hgm_log.h"

namespace OHOS::Rosen {
namespace {
const PolicyConfigData::ScreenSetting NULL_SCREEN_SETTING{};
}

PolicyConfigVisitorImpl::PolicyConfigVisitorImpl(const PolicyConfigData& configData)
    : configData_(configData)
{
}

const PolicyConfigData& PolicyConfigVisitorImpl::GetXmlData() const
{
    return configData_;
}

void PolicyConfigVisitorImpl::SetSettingModeId(int32_t settingModeId)
{
    if (settingModeId == HGM_REFRESHRATE_MODE_AUTO) {
        HGM_LOGI("SetSettingModeId %{public}d -> 0", settingModeId);
        settingModeId = 0; // index of auto mode is 0
    }
    if (settingModeId_ == settingModeId) {
        return;
    }
    auto xmlModeId = SettingModeId2XmlModeId(settingModeId);
    if (!xmlModeId.has_value()) {
        HGM_LOGE("SetSettingModeId %{public}d fail", settingModeId);
        return;
    }
    settingModeId_ = settingModeId;
    xmlModeId_ = xmlModeId.value();
    OnUpdate();
}

void PolicyConfigVisitorImpl::SetXmlModeId(const std::string& xmlModeId)
{
    if (xmlModeId_ == xmlModeId) {
        return;
    }
    auto settingModeId = XmlModeId2SettingModeId(xmlModeId);
    if (!settingModeId.has_value()) {
        HGM_LOGE("SetXmlModeId %{public}s fail", xmlModeId.c_str());
        return;
    }
    xmlModeId_ = xmlModeId;
    settingModeId_ = settingModeId.value();
    OnUpdate();
}

int32_t PolicyConfigVisitorImpl::GetRefreshRateModeName(int32_t refreshRateModeId) const
{
    auto iter = std::find_if(configData_.refreshRateForSettings_.begin(), configData_.refreshRateForSettings_.end(),
        [&](auto nameModeId) {
            return nameModeId.second == refreshRateModeId;
        });
    if (iter != configData_.refreshRateForSettings_.end()) {
        return iter->first;
    }
    return 0;
}

void PolicyConfigVisitorImpl::ChangeScreen(const std::string& screenConfigType)
{
    if (screenConfigType_ == screenConfigType) {
        return;
    }
    screenConfigType_ = screenConfigType;
    OnUpdate();
}

HgmErrCode PolicyConfigVisitorImpl::GetStrategyConfig(
    const std::string& strategyName, PolicyConfigData::StrategyConfig& strategyRes) const
{
    if (auto iter = configData_.strategyConfigs_.find(strategyName); iter != configData_.strategyConfigs_.end()) {
        strategyRes = iter->second;
        return EXEC_SUCCESS;
    }
    return HGM_ERROR;
}

const PolicyConfigData::ScreenSetting& PolicyConfigVisitorImpl::GetScreenSetting() const
{
    const auto& screenConfigs = configData_.screenConfigs_;
    if (auto screenConfigsIter = screenConfigs.find(screenConfigType_); screenConfigsIter != screenConfigs.end()) {
        const auto& screenConfig = screenConfigsIter->second;
        if (auto iter = screenConfig.find(xmlModeId_); iter != screenConfig.end()) {
            return iter->second;
        } else {
            HGM_LOGD("xmlModeId not existed: %{public}s", xmlModeId_.c_str());
        }
    } else {
        HGM_LOGD("curScreenStrategyId not existed: %{public}s", screenConfigType_.c_str());
    }
    return NULL_SCREEN_SETTING;
}

const PolicyConfigData::DynamicSettingMap& PolicyConfigVisitorImpl::GetAceSceneDynamicSettingMap() const
{
    return GetScreenSetting().aceSceneDynamicSettings;
}

std::string PolicyConfigVisitorImpl::GetAppStrategyConfigName(const std::string& pkgName, int32_t appType) const
{
    const auto& screenSetting = GetScreenSetting();

    const auto& appConfigMap = screenSetting.appList;
    if (auto iter = appConfigMap.find(pkgName); iter != appConfigMap.end()) {
        return iter->second;
    }

    const auto& appTypes = screenSetting.appTypes;
    if (auto iter = appTypes.find(appType); iter != appTypes.end()) {
        return iter->second;
    }

    return screenSetting.strategy;
}

HgmErrCode PolicyConfigVisitorImpl::GetAppStrategyConfig(
    const std::string& pkgName, int32_t appType, PolicyConfigData::StrategyConfig& strategyRes) const
{
    return GetStrategyConfig(GetAppStrategyConfigName(pkgName, appType), strategyRes);
}

std::string PolicyConfigVisitorImpl::GetGameNodeName(const std::string& pkgName) const
{
    const auto& appNodeMap = GetScreenSetting().gameAppNodeList;
    if (auto iter = appNodeMap.find(pkgName); iter != appNodeMap.end()) {
        return iter->second;
    }
    return "";
}

HgmErrCode PolicyConfigVisitorImpl::GetDynamicAppStrategyConfig(const std::string& pkgName,
    PolicyConfigData::StrategyConfig& strategyRes) const
{
    return HGM_ERROR;
}

std::optional<std::string> PolicyConfigVisitorImpl::SettingModeId2XmlModeId(int32_t settingModeId) const
{
    std::optional<std::string> xmlModeId;
    if (settingModeId < 0 || settingModeId >= static_cast<int32_t>(configData_.refreshRateForSettings_.size())) {
        return xmlModeId;
    }
    xmlModeId = std::to_string(configData_.refreshRateForSettings_.at(settingModeId).second);
    return xmlModeId;
}

std::optional<int32_t> PolicyConfigVisitorImpl::XmlModeId2SettingModeId(const std::string& xmlModeId) const
{
    std::optional<int32_t> settingModeId;
    auto iter = std::find_if(configData_.refreshRateForSettings_.begin(), configData_.refreshRateForSettings_.end(),
        [&](auto nameModeId) {
            return std::to_string(nameModeId.second) == xmlModeId;
        });
    if (iter != configData_.refreshRateForSettings_.end()) {
        settingModeId = static_cast<int32_t>(iter - configData_.refreshRateForSettings_.begin());
    }
    return settingModeId;
}
} // namespace OHOS::Rosen