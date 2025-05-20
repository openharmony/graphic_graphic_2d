/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include "xml_parser.h"
#include <algorithm>

#include "config_policy_utils.h"

namespace OHOS::Rosen {
namespace {
constexpr uint32_t FPS_MAX = 120;   // for hgm_idle_detector: default max fps of third framework
constexpr uint32_t XML_STRING_MAX_LENGTH = 8;
const static std::string S_UP_TIMEOUT_MS = "up_timeout_ms";
constexpr int32_t UP_TIMEOUT_MS = 3000;
}

int32_t XMLParser::LoadConfiguration(const char* fileDir)
{
    HGM_LOGI("XMLParser opening xml file");
    xmlDocument_ = xmlReadFile(fileDir, nullptr, 0);
    if (!xmlDocument_) {
        HGM_LOGE("XMLParser xmlReadFile failed");
        return XML_FILE_LOAD_FAIL;
    }

    if (!mParsedData_) {
        mParsedData_ = std::make_unique<PolicyConfigData>();
    }

    return EXEC_SUCCESS;
}

int32_t XMLParser::Parse()
{
    HGM_LOGD("XMLParser Parse");
    if (!xmlDocument_) {
        HGM_LOGE("XMLParser xmlDocument_ is empty, should do LoadConfiguration first");
        return HGM_ERROR;
    }
    xmlNode *root = xmlDocGetRootElement(xmlDocument_);
    if (root == nullptr) {
        HGM_LOGE("XMLParser xmlDocGetRootElement failed");
        return XML_GET_ROOT_FAIL;
    }

    if (ParseInternal(*root) == false) {
        return XML_PARSE_INTERNAL_FAIL;
    }
    int32_t upTimeoutMs = UP_TIMEOUT_MS;
    auto& timeoutStrategyConfig = mParsedData_->timeoutStrategyConfig_;
    if (timeoutStrategyConfig.find(S_UP_TIMEOUT_MS) != timeoutStrategyConfig.end() &&
        IsNumber(timeoutStrategyConfig[S_UP_TIMEOUT_MS])) {
        int32_t upTimeoutMsCfg = static_cast<int32_t>(std::stoi(timeoutStrategyConfig[S_UP_TIMEOUT_MS]));
        upTimeoutMs = upTimeoutMsCfg == 0 ? upTimeoutMs : upTimeoutMsCfg;
    }
    for (auto& [_, val] : mParsedData_->strategyConfigs_) {
        val.upTimeOut = val.upTimeOut == 0 ? upTimeoutMs : val.upTimeOut;
    }
    return EXEC_SUCCESS;
}

void XMLParser::Destroy()
{
    HGM_LOGD("XMLParser Destroying the parser");
    if (xmlDocument_ != nullptr) {
        xmlFreeDoc(xmlDocument_);
        xmlDocument_ = nullptr;
    }
}

int32_t XMLParser::GetHgmXmlNodeAsInt(xmlNode& node)
{
    if (!xmlStrcmp(node.name, reinterpret_cast<const xmlChar*>("Param"))) {
        return HGM_XML_PARAM;
    }
    if (!xmlStrcmp(node.name, reinterpret_cast<const xmlChar*>("Params"))) {
        return HGM_XML_PARAMS;
    }
    HGM_LOGD("XMLParser failed to identify a xml node : %{public}s", node.name);
    return HGM_XML_UNDEFINED;
}

bool XMLParser::ParseInternal(xmlNode& node)
{
    HGM_LOGD("XMLParser parsing an internal node");
    xmlNode *currNode = &node;
    if (currNode->xmlChildrenNode == nullptr) {
        HGM_LOGD("XMLParser stop parsing internal, no children nodes");
        return false;
    }
    currNode = currNode->xmlChildrenNode;
    int32_t parseSuccess = EXEC_SUCCESS;

    for (; currNode; currNode = currNode->next) {
        if (currNode->type != XML_ELEMENT_NODE) {
            continue;
        }
        if (parseSuccess != EXEC_SUCCESS) {
            return false;
        }
        int xmlParamType = GetHgmXmlNodeAsInt(*currNode);
        if (xmlParamType == HGM_XML_PARAM) {
            parseSuccess = ParseParam(*currNode);
        } else if (xmlParamType == HGM_XML_PARAMS) {
            parseSuccess = ParseParams(*currNode);
        }
    }
    return true;
}

int32_t XMLParser::ParseParam(xmlNode& node)
{
    HGM_LOGI("XMLParser parsing a parameter");
    if (!mParsedData_) {
        HGM_LOGE("XMLParser mParsedData_ is not initialized");
        return HGM_ERROR;
    }

    std::string paraName = ExtractPropertyValue("name", node);
    if (paraName == "default_refreshrate_mode") {
        HGM_LOGD("XMLParser parsing default_refreshrate_mode");
        std::string mode = ExtractPropertyValue("value", node);
        mParsedData_->defaultRefreshRateMode_ = mode;

        HGM_LOGI("HgmXMLParser ParseParam default_refreshrate_mode %{public}s",
                 mParsedData_->defaultRefreshRateMode_.c_str());
    }

    return EXEC_SUCCESS;
}

int32_t XMLParser::ParseSubSequentParams(xmlNode& node, std::string& paraName)
{
    int32_t setResult = EXEC_SUCCESS;

    if (paraName == "additional_touch_rate_config") {
        ParseAppBufferList(node);
    } else if (paraName == "refreshRate_strategy_config") {
        setResult = ParseStrategyConfig(node);
    } else if (paraName == "refreshRate_virtual_display_config") {
        if (ExtractPropertyValue("switch", node) == "1") {
            setResult = ParseSimplex(node, mParsedData_->virtualDisplayConfigs_, "strategy");
            mParsedData_->virtualDisplaySwitch_ = true;
        } else {
            mParsedData_->virtualDisplayConfigs_.clear();
            mParsedData_->virtualDisplaySwitch_ = false;
        }
    } else if (paraName == "safe_vote") {
        // "1": enable
        mParsedData_->safeVoteEnabled = ExtractPropertyValue("switch", node) == "1";
    } else if (paraName == "screen_strategy_config") {
        setResult = ParseSimplex(node, mParsedData_->screenStrategyConfigs_, "type");
    } else if (paraName == "screen_config") {
        setResult = ParseScreenConfig(node);
    } else if (paraName == "rs_video_frame_rate_vote_config") {
        setResult = ParseVideoFrameVoteConfig(node);
    } else if (paraName == "source_tuning_for_yuv420") {
        setResult = ParseSimplex(node, mParsedData_->sourceTuningConfig_);
    } else if (paraName == "rs_solid_color_layer_config") {
        setResult = ParseSimplex(node, mParsedData_->solidLayerConfig_);
    } else if (paraName == "timeout_strategy_config") {
        setResult = ParseSimplex(node, mParsedData_->timeoutStrategyConfig_);
    } else if (paraName == "video_call_layer_config") {
        setResult = ParseSimplex(node, mParsedData_->videoCallLayerConfig_);
    } else if (paraName == "vrate_control_config") {
        setResult = ParseSimplex(node, mParsedData_->vRateControlList_);
    } else {
        setResult = EXEC_SUCCESS;
    }

    return setResult;
}

int32_t XMLParser::ParseParams(xmlNode& node)
{
    std::string paraName = ExtractPropertyValue("name", node);
    if (paraName.empty()) {
        return XML_PARSE_INTERNAL_FAIL;
    }
    if (!mParsedData_) {
        HGM_LOGE("XMLParser mParsedData_ is not initialized");
        return HGM_ERROR;
    }

    int32_t setResult = EXEC_SUCCESS;
    if (paraName == "refresh_rate_4settings") {
        std::unordered_map<std::string, std::string> refreshRateForSettings;
        setResult = ParseSimplex(node, refreshRateForSettings);
        if (setResult != EXEC_SUCCESS) {
            mParsedData_->xmlCompatibleMode_ = true;
            setResult = ParseSimplex(node, refreshRateForSettings, "id");
        }
        mParsedData_->refreshRateForSettings_.clear();
        for (auto &[name, id]: refreshRateForSettings) {
            if (IsNumber(name) && IsNumber(id)) {
                mParsedData_->refreshRateForSettings_.emplace_back(
                    std::pair<int32_t, int32_t>(std::stoi(name), std::stoi(id)));
            }
        }
        std::sort(mParsedData_->refreshRateForSettings_.begin(), mParsedData_->refreshRateForSettings_.end(),
            [=] (auto rateId0, auto rateId1) { return rateId0.first < rateId1.first; });
    } else {
        setResult = ParseSubSequentParams(node, paraName);
    }

    if (setResult != EXEC_SUCCESS) {
        HGM_LOGI("XMLParser failed to ParseParams %{public}s", paraName.c_str());
    }
    return EXEC_SUCCESS;
}

int32_t XMLParser::ParseVideoFrameVoteConfig(xmlNode& node)
{
    mParsedData_->videoFrameRateVoteSwitch_ = ExtractPropertyValue("switch", node) == "1";
    return ParseSimplex(node, mParsedData_->videoFrameRateList_);
}

int32_t XMLParser::ParseStrategyConfig(xmlNode& node)
{
    HGM_LOGD("XMLParser parsing strategyConfig");
    xmlNode *currNode = &node;
    if (currNode->xmlChildrenNode == nullptr) {
        HGM_LOGD("XMLParser stop parsing strategyConfig, no children nodes");
        return HGM_ERROR;
    }

    // re-parse
    mParsedData_->strategyConfigs_.clear();
    currNode = currNode->xmlChildrenNode;
    for (; currNode; currNode = currNode->next) {
        if (currNode->type != XML_ELEMENT_NODE) {
            continue;
        }
        auto name = ExtractPropertyValue("name", *currNode);
        PolicyConfigData::StrategyConfig strategy;
        if (!BuildStrategyConfig(*currNode, strategy)) {
            return HGM_ERROR;
        }
        ParseBufferStrategyList(*currNode, strategy);
        mParsedData_->strategyConfigs_[name] = strategy;
        HGM_LOGI("HgmXMLParser ParseStrategyConfig name=%{public}s min=%{public}d drawMin=%{public}d",
                 name.c_str(), mParsedData_->strategyConfigs_[name].min, mParsedData_->strategyConfigs_[name].drawMin);
    }

    return EXEC_SUCCESS;
}

void XMLParser::ParseAppBufferList(xmlNode& node)
{
    HGM_LOGD("XMLParser parsing ParseAppBufferList");
    xmlNode *currNode = &node;
    if (currNode->xmlChildrenNode == nullptr) {
        HGM_LOGD("XMLParser stop parsing ParseAppBufferList, no children nodes");
        return;
    }

    mParsedData_->appBufferList_.clear();
    currNode = currNode->xmlChildrenNode;
    for (; currNode; currNode = currNode->next) {
        if (currNode->type != XML_ELEMENT_NODE) {
            continue;
        }
        auto name = ExtractPropertyValue("name", *currNode);
        mParsedData_->appBufferList_.push_back(name);
    }
}

void XMLParser::ParseBufferStrategyList(xmlNode& node, PolicyConfigData::StrategyConfig& strategy)
{
    if (mParsedData_->appBufferList_.empty()) {
        return;
    }
    for (auto& name : mParsedData_->appBufferList_) {
        auto fpsStr = ExtractPropertyValue(name, node);
        if (fpsStr == "") {
            strategy.bufferFpsMap[name] = FPS_MAX;
        } else if (IsNumber(fpsStr)) {
            auto fpsNum = std::stoi(fpsStr);
            if (fpsNum >= 0) {
                strategy.bufferFpsMap[name] = fpsNum;
            }
        }
    }
}

int32_t XMLParser::ParseScreenConfig(xmlNode& node)
{
    HGM_LOGD("XMLParser parsing screenConfig");
    xmlNode *currNode = &node;
    if (currNode->xmlChildrenNode == nullptr) {
        HGM_LOGD("XMLParser stop parsing screenConfig, no children nodes");
        return HGM_ERROR;
    }

    auto type = ExtractPropertyValue("type", *currNode);
    PolicyConfigData::ScreenConfig screenConfig;
    currNode = currNode->xmlChildrenNode;
    for (; currNode; currNode = currNode->next) {
        if (currNode->type != XML_ELEMENT_NODE) {
            continue;
        }
        auto name = ExtractPropertyValue("name", *currNode);
        if (name == "supported_mode") {
            PolicyConfigData::SupportedModeConfig supportedModeConfig;
            if (ParseSupportedModeConfig(*currNode, supportedModeConfig) != EXEC_SUCCESS) {
                HGM_LOGI("XMLParser failed to ParseScreenConfig %{public}s", name.c_str());
            }
            mParsedData_->supportedModeConfigs_[type] = supportedModeConfig;
            continue;
        }
        PolicyConfigData::ScreenSetting screenSetting;
        auto id = ExtractPropertyValue("id", *currNode);
        screenSetting.strategy = ExtractPropertyValue("strategy", *currNode);
        for (xmlNode *thresholdNode = currNode->xmlChildrenNode; thresholdNode; thresholdNode = thresholdNode->next) {
            ParseSubScreenConfig(*thresholdNode, screenSetting);
        }
        screenConfig[id] = screenSetting;
        HGM_LOGI("HgmXMLParser ParseScreenConfig id=%{public}s", id.c_str());
    }
    for (const auto& screenExtStrategy : HGM_CONFIG_SCREENEXT_STRATEGY_MAP) {
        if (size_t pos = type.find(screenExtStrategy.first); pos != std::string::npos) {
            auto defaultScreenConfig = mParsedData_->screenConfigs_.find(type.substr(0, pos));
            if (defaultScreenConfig != mParsedData_->screenConfigs_.end()) {
                ReplenishMissingScreenConfig(defaultScreenConfig->second, screenConfig);
            } else {
                HGM_LOGE("XMLParser failed to ReplenishMissingScreenConfig %{public}s", type.c_str());
                return EXEC_SUCCESS;
            }
        }
    }
    mParsedData_->screenConfigs_[type] = screenConfig;
    return EXEC_SUCCESS;
}

int32_t XMLParser::ParseSubScreenConfig(xmlNode& node, PolicyConfigData::ScreenSetting& screenSetting)
{
    xmlNode *thresholdNode = &node;
    if (thresholdNode->type != XML_ELEMENT_NODE) {
        return HGM_ERROR;
    }
    auto name = ExtractPropertyValue("name", *thresholdNode);
    int32_t setResult = EXEC_SUCCESS;
    if (name == "LTPO_config") {
        setResult = ParseSimplex(*thresholdNode, screenSetting.ltpoConfig);
    } else if (name == "property_animation_dynamic_settings") {
        setResult = ParseDynamicSetting(*thresholdNode, screenSetting.animationDynamicSettings);
    } else if (name == "ace_scene_dynamic_settings") {
        setResult = ParseDynamicSetting(*thresholdNode, screenSetting.aceSceneDynamicSettings);
    } else if (name == "small_size_property_animation_dynamic_settings") {
        setResult = ParseSmallSizeDynamicSetting(*thresholdNode, screenSetting);
    } else if (name == "scene_list") {
        setResult = ParseSceneList(*thresholdNode, screenSetting.sceneList);
    } else if (name == "game_scene_list") {
        setResult = ParseSimplex(*thresholdNode, screenSetting.gameSceneList);
    } else if (name == "anco_scene_list") {
        setResult = ParseSceneList(*thresholdNode, screenSetting.ancoSceneList);
    } else if (name == "app_list") {
        ParseMultiAppStrategy(*thresholdNode, screenSetting);
    } else if (name == "app_types") {
        setResult = ParseAppTypes(*thresholdNode, screenSetting.appTypes);
    } else if (name == "rs_animation_power_config") {
        setResult = ParseSimplex(*thresholdNode, screenSetting.animationPowerConfig);
    } else if (name == "ui_power_config") {
        setResult = ParseSimplex(*thresholdNode, screenSetting.uiPowerConfig);
    } else if (name == "component_power_config") {
        setResult = ParsePowerStrategy(*thresholdNode, screenSetting.componentPowerConfig);
    } else if (name == "app_page_url_config") {
        setResult = ParsePageUrlStrategy(*thresholdNode, screenSetting.pageUrlConfig);
    } else if (name == "performance_config") {
        setResult = ParsePerformanceConfig(*thresholdNode, screenSetting.performanceConfig);
    } else {
        setResult = EXEC_SUCCESS;
    }

    if (setResult != EXEC_SUCCESS) {
        HGM_LOGI("XMLParser failed to ParseScreenConfig %{public}s", name.c_str());
    }
    return setResult;
}

int32_t XMLParser::ParseSimplex(xmlNode& node, std::unordered_map<std::string, std::string>& config,
                                const std::string& valueName, const std::string& keyName,
                                const bool canBeEmpty)
{
    HGM_LOGD("XMLParser parsing simplex");
    xmlNode *currNode = &node;
    if (currNode->xmlChildrenNode == nullptr) {
        HGM_LOGD("XMLParser stop parsing simplex, no children nodes");
        return HGM_ERROR;
    }

    // re-parse
    config.clear();
    currNode = currNode->xmlChildrenNode;
    for (; currNode; currNode = currNode->next) {
        if (currNode->type != XML_ELEMENT_NODE) {
            continue;
        }

        auto key = ExtractPropertyValue(keyName, *currNode);
        auto value = ExtractPropertyValue(valueName, *currNode);
        if (key.empty()) {
            return XML_PARSE_INTERNAL_FAIL;
        }

        if (value.empty()) {
            if (canBeEmpty) {
                continue;
            }
            return XML_PARSE_INTERNAL_FAIL;
        }

        config[key] = value;

        HGM_LOGI("HgmXMLParser ParseSimplex %{public}s=%{public}s %{public}s=%{public}s",
                 keyName.c_str(), key.c_str(), valueName.c_str(), config[key].c_str());
    }

    return EXEC_SUCCESS;
}

int32_t XMLParser::ParsePowerStrategy(xmlNode& node, std::unordered_map<std::string, int32_t>& powerConfig)
{
    std::unordered_map<std::string, std::string> configs;
    auto result = ParseSimplex(node, configs);
    powerConfig.clear();
    if (result != EXEC_SUCCESS) {
        HGM_LOGI("XMLParser failed to powerConfig component_power_config");
        return result;
    }
    for (const auto &item: configs) {
        if (!IsNumber(item.second)) {
            continue;
        }
        powerConfig[item.first] = std::stoi(item.second.c_str());
    }
    return EXEC_SUCCESS;
}

int32_t XMLParser::ParseSmallSizeDynamicSetting(xmlNode& node, PolicyConfigData::ScreenSetting& screenSetting)
{
    auto area = ExtractPropertyValue("area", node);
    auto length = ExtractPropertyValue("length", node);
    if (!IsNumber(area) || !IsNumber(length)) {
        return HGM_ERROR;
    }
    screenSetting.smallSizeArea = std::stoi(area);
    screenSetting.smallSizeLength = std::stoi(length);
    return ParseDynamicSetting(node, screenSetting.smallSizeAnimationDynamicSettings);
}

int32_t XMLParser::ParseDynamicSetting(xmlNode& node, PolicyConfigData::DynamicSettingMap& dynamicSettingMap)
{
    HGM_LOGD("XMLParser parsing dynamicSetting");
    xmlNode *currNode = &node;
    if (currNode->xmlChildrenNode == nullptr) {
        HGM_LOGD("XMLParser stop parsing dynamicSetting, no children nodes");
        return HGM_ERROR;
    }

    // re-parse
    dynamicSettingMap.clear();
    currNode = currNode->xmlChildrenNode;
    for (; currNode; currNode = currNode->next) {
        auto dynamicSettingType = ExtractPropertyValue("name", *currNode);
        PolicyConfigData::DynamicSetting dynamicSetting;
        dynamicSettingMap[dynamicSettingType] = dynamicSetting;
        for (xmlNode *thresholdNode = currNode->xmlChildrenNode; thresholdNode; thresholdNode = thresholdNode->next) {
            if (thresholdNode->type != XML_ELEMENT_NODE) {
                continue;
            }
            auto name = ExtractPropertyValue("name", *thresholdNode);
            auto min = ExtractPropertyValue("min", *thresholdNode);
            auto max = ExtractPropertyValue("max", *thresholdNode);
            auto preferred_fps = ExtractPropertyValue("preferred_fps", *thresholdNode);
            if (!IsNumber(min) || !IsNumber(max) || !IsNumber(preferred_fps)) {
                dynamicSettingMap[dynamicSettingType].clear();
                break;
            }
            PolicyConfigData::DynamicConfig dynamicConfig;
            dynamicConfig.min = std::stoi(min);
            dynamicConfig.max = std::stoi(max);
            dynamicConfig.preferred_fps = std::stoi(preferred_fps);
            dynamicSettingMap[dynamicSettingType][name] = dynamicConfig;

            HGM_LOGI("HgmXMLParser ParseDynamicSetting dynamicType=%{public}s name=%{public}s min=%{public}d",
                     dynamicSettingType.c_str(), name.c_str(), dynamicSettingMap[dynamicSettingType][name].min);
        }
    }
    return EXEC_SUCCESS;
}

int32_t XMLParser::ParseSceneList(xmlNode& node, PolicyConfigData::SceneConfigMap& sceneList)
{
    HGM_LOGD("XMLParser parsing sceneList");
    xmlNode *currNode = &node;
    if (currNode->xmlChildrenNode == nullptr) {
        HGM_LOGD("XMLParser stop parsing sceneList, no children nodes");
        return HGM_ERROR;
    }

    // re-parse
    sceneList.clear();
    currNode = currNode->xmlChildrenNode;
    for (; currNode; currNode = currNode->next) {
        if (currNode->type != XML_ELEMENT_NODE) {
            continue;
        }
        PolicyConfigData::SceneConfig sceneConfig;
        auto name = ExtractPropertyValue("name", *currNode);
        sceneConfig.strategy = ExtractPropertyValue("strategy", *currNode);
        sceneConfig.priority = ExtractPropertyValue("priority", *currNode);
        sceneConfig.doNotAutoClear = ExtractPropertyValue("doNotAutoClear", *currNode) == "1";
        sceneConfig.disableSafeVote = ExtractPropertyValue("disableSafeVote", *currNode) == "1";
        sceneList[name] = sceneConfig;
        HGM_LOGI("HgmXMLParser ParseSceneList name=%{public}s strategy=%{public}s priority=%{public}s \
                 doNotAutoClear=%{public}s disableSafeVote=%{public}s", name.c_str(),
                 sceneList[name].strategy.c_str(), sceneList[name].priority.c_str(),
                 sceneList[name].doNotAutoClear ? "true" : "false",
                 sceneList[name].disableSafeVote ? "true" : "false");
    }

    return EXEC_SUCCESS;
}

int32_t XMLParser::ParseSupportedModeConfig(xmlNode& node, PolicyConfigData::SupportedModeConfig& supportedModeConfig)
{
    HGM_LOGD("XMLParser parsing supportedModeConfig");
    xmlNode *currNode = &node;
    if (currNode->xmlChildrenNode == nullptr) {
        HGM_LOGD("XMLParser stop parsing supportedModeConfig, no children nodes");
        return HGM_ERROR;
    }

    // re-parse
    supportedModeConfig.clear();
    currNode = currNode->xmlChildrenNode;
    for (; currNode; currNode = currNode->next) {
        if (currNode->type != XML_ELEMENT_NODE) {
            continue;
        }
        std::vector<uint32_t> supportedModeVec;
        auto name = ExtractPropertyValue("name", *currNode);
        auto value = ExtractPropertyValue("value", *currNode);
        supportedModeVec = StringToVector(value);

        supportedModeConfig[name] = supportedModeVec;
        HGM_LOGI("HgmXMLParser ParseSupportedModeConfig name=%{public}s value=%{public}s",
            name.c_str(), value.c_str());
    }
    return EXEC_SUCCESS;
}

int32_t XMLParser::ParseMultiAppStrategy(xmlNode& node, PolicyConfigData::ScreenSetting& screenSetting)
{
    auto multiAppStrategy = ExtractPropertyValue("multi_app_strategy", node);
    if (multiAppStrategy == "focus") {
        screenSetting.multiAppStrategyType = MultiAppStrategyType::FOLLOW_FOCUS;
    } else if (multiAppStrategy.find("strategy_") != std::string::npos) {
        screenSetting.multiAppStrategyType = MultiAppStrategyType::USE_STRATEGY_NUM;
        screenSetting.multiAppStrategyName = multiAppStrategy.substr(
            std::string("strategy_").size(), multiAppStrategy.size());
    } else {
        screenSetting.multiAppStrategyType = MultiAppStrategyType::USE_MAX;
    }
    ParseSimplex(node, screenSetting.gameAppNodeList, "nodeName", "name", true);
    return ParseSimplex(node, screenSetting.appList, "strategy");
}


int32_t XMLParser::ParseAppTypes(xmlNode& node, std::unordered_map<int32_t, std::string>& appTypes)
{
    HGM_LOGD("XMLParser parsing appTypes");
    xmlNode *currNode = &node;
    if (currNode->xmlChildrenNode == nullptr) {
        HGM_LOGD("XMLParser stop parsing appTypes, no children nodes");
        return HGM_ERROR;
    }

    // re-parse
    appTypes.clear();
    currNode = currNode->xmlChildrenNode;
    for (; currNode; currNode = currNode->next) {
        if (currNode->type != XML_ELEMENT_NODE) {
            continue;
        }
        auto name = ExtractPropertyValue("name", *currNode);
        if (!IsNumber(name)) {
            continue;
        }
        auto strategy = ExtractPropertyValue("strategy", *currNode);
        appTypes[std::stoi(name)] = strategy;
        HGM_LOGI("HgmXMLParser ParseAppTypes name=%{public}s strategy=%{public}s", name.c_str(), strategy.c_str());
    }

    return EXEC_SUCCESS;
}

void XMLParser::ReplenishMissingScreenAppGameConfig(PolicyConfigData::ScreenSetting& screenSetting,
    const PolicyConfigData::ScreenSetting& screenSettingDefalut)
{
    if (screenSetting.appList.empty()) {
        screenSetting.appList = screenSettingDefalut.appList;
        screenSetting.multiAppStrategyType = screenSettingDefalut.multiAppStrategyType;
        screenSetting.multiAppStrategyName = screenSettingDefalut.multiAppStrategyName;
    }
    if (screenSetting.appTypes.empty()) {
        screenSetting.appTypes = screenSettingDefalut.appTypes;
    }
    if (screenSetting.gameSceneList.empty()) {
        screenSetting.gameSceneList = screenSettingDefalut.gameSceneList;
    }
    if (screenSetting.gameAppNodeList.empty()) {
        screenSetting.gameAppNodeList = screenSettingDefalut.gameAppNodeList;
    }
}

int32_t XMLParser::ReplenishMissingScreenConfig(const PolicyConfigData::ScreenConfig& screenConfigDefault,
    PolicyConfigData::ScreenConfig& screenConfig)
{
    HGM_LOGD("HgmXMLParser ReplenishMissingScreenConfig");
    for (const auto& [id, screenSettingDefalut] : screenConfigDefault) {
        const auto& screenSetting = screenConfig.find(id);
        if (screenSetting == screenConfig.end()) {
            screenConfig[id] = screenSettingDefalut;
            continue;
        }
        if (screenSetting->second.ltpoConfig.empty()) {
            screenSetting->second.ltpoConfig = screenSettingDefalut.ltpoConfig;
        }
        if (screenSetting->second.sceneList.empty()) {
            screenSetting->second.sceneList = screenSettingDefalut.sceneList;
        }
        if (screenSetting->second.animationDynamicSettings.empty()) {
            screenSetting->second.animationDynamicSettings = screenSettingDefalut.animationDynamicSettings;
        }
        if (screenSetting->second.aceSceneDynamicSettings.empty()) {
            screenSetting->second.aceSceneDynamicSettings = screenSettingDefalut.aceSceneDynamicSettings;
        }
        if (screenSetting->second.smallSizeAnimationDynamicSettings.empty()) {
            screenSetting->second.smallSizeArea = screenSettingDefalut.smallSizeArea;
            screenSetting->second.smallSizeLength = screenSettingDefalut.smallSizeLength;
            screenSetting->second.smallSizeAnimationDynamicSettings =
                screenSettingDefalut.smallSizeAnimationDynamicSettings;
        }
        if (screenSetting->second.animationPowerConfig.empty()) {
            screenSetting->second.animationPowerConfig = screenSettingDefalut.animationPowerConfig;
        }
        if (screenSetting->second.uiPowerConfig.empty()) {
            screenSetting->second.uiPowerConfig = screenSettingDefalut.uiPowerConfig;
        }
        if (screenSetting->second.ancoSceneList.empty()) {
            screenSetting->second.ancoSceneList = screenSettingDefalut.ancoSceneList;
        }
        if (screenSetting->second.componentPowerConfig.empty()) {
            screenSetting->second.componentPowerConfig = screenSettingDefalut.componentPowerConfig;
        }
        if (screenSetting->second.pageUrlConfig.empty()) {
            screenSetting->second.pageUrlConfig = screenSettingDefalut.pageUrlConfig;
        }
        if (screenSetting->second.performanceConfig.empty()) {
            screenSetting->second.performanceConfig = screenSettingDefalut.performanceConfig;
        }
        ReplenishMissingScreenAppGameConfig(screenSetting->second, screenSettingDefalut);
    }
    return EXEC_SUCCESS;
}

int32_t XMLParser::ParsePerformanceConfig(
    xmlNode& node, std::unordered_map<std::string, std::string>& performanceConfig)
{
    HGM_LOGD("XMLParser parsing performanceConfig");
    xmlNode *currNode = &node;
    if (currNode->xmlChildrenNode == nullptr) {
        HGM_LOGD("XMLParser stop parsing performanceConfig, no children nodes");
        return HGM_ERROR;
    }

    // re-parse
    performanceConfig.clear();
    currNode = currNode->xmlChildrenNode;
    for (; currNode; currNode = currNode->next) {
        if (currNode->type != XML_ELEMENT_NODE) {
            continue;
        }
        auto name = ExtractPropertyValue("name", *currNode);
        auto value = ExtractPropertyValue("value", *currNode);
        performanceConfig[std::move(name)] = std::move(value);
        HGM_LOGI("HgmXMLParser performanceConfig name=%{public}s strategy=%{public}s", name.c_str(), value.c_str());
    }

    return EXEC_SUCCESS;
}

std::string XMLParser::ExtractPropertyValue(const std::string& propName, xmlNode& node)
{
    HGM_LOGD("XMLParser extracting value : %{public}s", propName.c_str());
    std::string propValue = "";
    xmlChar *tempValue = nullptr;

    if (xmlHasProp(&node, reinterpret_cast<const xmlChar*>(propName.c_str()))) {
        tempValue = xmlGetProp(&node, reinterpret_cast<const xmlChar*>(propName.c_str()));
    }

    if (tempValue != nullptr) {
        HGM_LOGD("XMLParser not aempty tempValue");
        propValue = reinterpret_cast<const char*>(tempValue);
        xmlFree(tempValue);
        tempValue = nullptr;
    }

    return propValue;
}

bool XMLParser::IsNumber(const std::string& str)
{
    if (str.length() == 0 || str.length() > XML_STRING_MAX_LENGTH) {
        return false;
    }
    auto number = static_cast<uint32_t>(std::count_if(str.begin(), str.end(), [](unsigned char c) {
        return std::isdigit(c);
    }));
    return number == str.length() || (str.compare(0, 1, "-") == 0 && number == str.length() - 1);
}

std::vector<uint32_t> XMLParser::StringToVector(const std::string &str, const std::string &pattern)
{
    std::vector<std::string> vstr;
    std::string::size_type wordBegin = 0;
    std::string::size_type wordEnd = str.find(pattern);
    while (wordEnd != std::string::npos) {
        vstr.push_back(str.substr(wordBegin, wordEnd - wordBegin));
        wordBegin = wordEnd + pattern.size();
        wordEnd = str.find(pattern, wordBegin);
    }
    if (wordBegin != str.length()) {
        vstr.push_back(str.substr(wordBegin));
    }

    std::vector<uint32_t> vec;
    for (const auto& s : vstr) {
        if (!IsNumber(s)) {
            continue;
        }
        vec.emplace_back(std::stoi(s));
    }
    return vec;
}

int32_t XMLParser::ParsePageUrlStrategy(xmlNode& node,
    std::unordered_map<std::string, PolicyConfigData::PageUrlConfig>& pageUrlConfigMap)
{
    pageUrlConfigMap.clear();
    HGM_LOGD("XMLParser parsing PageUrlConfig");
    xmlNode *currNode = &node;
    if (currNode->xmlChildrenNode == nullptr) {
        HGM_LOGE("XMLParser stop parsing PageUrlConfig, no children nodes");
        return HGM_ERROR;
    }

    currNode = currNode->xmlChildrenNode;
    for (; currNode; currNode = currNode->next) {
        if (currNode->type != XML_ELEMENT_NODE) {
            continue;
        }
        if (currNode->xmlChildrenNode == nullptr) {
            HGM_LOGE("XMLParser stop parsing Package, no children nodes");
            return HGM_ERROR;
        }

        xmlNode *childNode = currNode->xmlChildrenNode;
        PolicyConfigData::PageUrlConfig pageUrlConfig;
        for (; childNode; childNode = childNode->next) {
            if (childNode->type != XML_ELEMENT_NODE) {
                continue;
            }
            auto name = ExtractPropertyValue("name", *childNode);
            auto strategy = ExtractPropertyValue("strategy", *childNode);
            mParsedData_->pageNameList_.push_back(name);
            pageUrlConfig[name] = strategy;
        }
        auto packageName = ExtractPropertyValue("name", *currNode);
        pageUrlConfigMap[packageName] = pageUrlConfig;
    }
    return EXEC_SUCCESS;
}

bool XMLParser::BuildStrategyConfig(xmlNode &currNode, PolicyConfigData::StrategyConfig &strategy)
{
    auto min = ExtractPropertyValue("min", currNode);
    auto max = ExtractPropertyValue("max", currNode);
    auto dynamicMode = ExtractPropertyValue("dynamicMode", currNode);
    auto pointerMode = ExtractPropertyValue("pointerMode", currNode);
    auto idleFps = ExtractPropertyValue("idleFps", currNode);
    auto isFactor = ExtractPropertyValue("isFactor", currNode) == "1"; // 1:true, other:false
    auto drawMin = ExtractPropertyValue("drawMin", currNode);
    auto drawMax = ExtractPropertyValue("drawMax", currNode);
    auto down = ExtractPropertyValue("down", currNode);
    auto supportAS = ExtractPropertyValue("supportAS", currNode);
    auto upTimeOut = ExtractPropertyValue("upTimeOut", currNode);
    if (!IsNumber(min) || !IsNumber(max) || !IsNumber(dynamicMode)) {
        return false;
    }
    strategy.min = std::stoi(min);
    strategy.max = std::stoi(max);
    strategy.dynamicMode = static_cast<DynamicModeType>(std::stoi(dynamicMode));
    strategy.pointerMode = IsNumber(pointerMode) ? static_cast<PointerModeType>(std::stoi(pointerMode)) :
        PointerModeType::POINTER_DISENABLED;
    strategy.idleFps = IsNumber(idleFps) ? std::clamp(std::stoi(idleFps), strategy.min, strategy.max) :
        std::max(strategy.min, static_cast<int32_t>(OLED_60_HZ));
    strategy.isFactor = isFactor;
    strategy.drawMin = IsNumber(drawMin) ? std::stoi(drawMin) : 0;
    strategy.drawMax = IsNumber(drawMax) ? std::stoi(drawMax) : 0;
    strategy.down = IsNumber(down) ? std::stoi(down) : strategy.max;
    strategy.supportAS = IsNumber(supportAS) ? std::stoi(supportAS) : 0;
    strategy.upTimeOut = IsNumber(upTimeOut) ? std::stoi(upTimeOut) : 0;
    return true;
}
} // namespace OHOS::Rosen