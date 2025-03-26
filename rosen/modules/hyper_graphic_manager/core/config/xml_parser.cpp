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
#include <sstream>
#include <regex>

#include "config_policy_utils.h"

namespace OHOS::Rosen {
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

int32_t XMLParser::GetHgmXmlNodeAsInt(xmlNode &node)
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

bool XMLParser::ParseInternal(xmlNode &node)
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

int32_t XMLParser::ParseParam(xmlNode &node)
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

int32_t XMLParser::ParseSubSequentParams(xmlNode &node, std::string &paraName)
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
    } else if (paraName == "sync_enhancement_config") {
        if (ExtractPropertyValue("switch", node) == "1") {
            mParsedData_->syncEnhancementSwitch_ = true;
        } else {
            mParsedData_->syncEnhancementSwitch_ = false;
        }
    } else {
        setResult = EXEC_SUCCESS;
    }

    return setResult;
}

int32_t XMLParser::ParseParams(xmlNode &node)
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

int32_t XMLParser::ParseVideoFrameVoteConfig(xmlNode &node)
{
    mParsedData_->videoFrameRateVoteSwitch_ = ExtractPropertyValue("switch", node) == "1";
    return ParseSimplex(node, mParsedData_->videoFrameRateList_);
}

int32_t XMLParser::ParseStrategyConfig(xmlNode &node)
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
        auto min = ExtractPropertyValue("min", *currNode);
        auto max = ExtractPropertyValue("max", *currNode);
        auto dynamicMode = ExtractPropertyValue("dynamicMode", *currNode);
        auto isFactor = ExtractPropertyValue("isFactor", *currNode) == "1"; // 1:true, other:false
        auto drawMin = ExtractPropertyValue("drawMin", *currNode);
        auto drawMax = ExtractPropertyValue("drawMax", *currNode);
        auto down = ExtractPropertyValue("down", *currNode);
        if (!IsNumber(min) || !IsNumber(max) || !IsNumber(dynamicMode)) {
            return HGM_ERROR;
        }

        PolicyConfigData::StrategyConfig strategy;
        strategy.min = std::stoi(min);
        strategy.max = std::stoi(max);
        strategy.dynamicMode = static_cast<DynamicModeType>(std::stoi(dynamicMode));
        strategy.isFactor = isFactor;
        strategy.drawMin = IsNumber(drawMin) ? std::stoi(drawMin) : 0;
        strategy.drawMax = IsNumber(drawMax) ? std::stoi(drawMax) : 0;
        strategy.down = IsNumber(down) ? std::stoi(down) : strategy.max;
        ParseBufferStrategyList(*currNode, strategy);
        mParsedData_->strategyConfigs_[name] = strategy;
        HGM_LOGI("HgmXMLParser ParseStrategyConfig name=%{public}s min=%{public}d drawMin=%{public}d",
                 name.c_str(), mParsedData_->strategyConfigs_[name].min, mParsedData_->strategyConfigs_[name].drawMin);
    }

    return EXEC_SUCCESS;
}

void XMLParser::ParseAppBufferList(xmlNode &node)
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

void XMLParser::ParseBufferStrategyList(xmlNode &node, PolicyConfigData::StrategyConfig &strategy)
{
    if (mParsedData_->appBufferList_.empty()) {
        return;
    }
    std::unordered_map<std::string, std::string> config;
    for (auto &name : mParsedData_->appBufferList_) {
        auto fps = ExtractPropertyValue(name, node);
        if (IsNumber(fps)) {
            config.insert(make_pair(name, fps));
        }
    }
    if (config.empty()) {
        return;
    }
    for (auto &it : config) {
        if (std::stoi(it.second) == 0) {
            strategy.appBufferBlackList.push_back(it.first);
        } else {
            strategy.appBufferList.push_back(make_pair(it.first, std::stoi(it.second)));
        }
    }
    if (strategy.appBufferList.empty()) {
        return;
    }
    std::sort(strategy.appBufferList.begin(), strategy.appBufferList.end(),
        [](const std::pair<std::string, int32_t>& a, const std::pair<std::string, int32_t>& b) {
        return a.second > b.second;
    });

    return;
}

int32_t XMLParser::ParseScreenConfig(xmlNode &node)
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
    mParsedData_->screenConfigs_[type] = screenConfig;
    return EXEC_SUCCESS;
}

int32_t XMLParser::ParseSubScreenConfig(xmlNode &node, PolicyConfigData::ScreenSetting& screenSetting)
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
        setResult = ParserDynamicSetting(*thresholdNode, screenSetting.animationDynamicSettings);
    } else if (name == "ace_scene_dynamic_settings") {
        setResult = ParserDynamicSetting(*thresholdNode, screenSetting.aceSceneDynamicSettings);
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
    } else {
        setResult = EXEC_SUCCESS;
    }

    if (setResult != EXEC_SUCCESS) {
        HGM_LOGI("XMLParser failed to ParseScreenConfig %{public}s", name.c_str());
    }
    return setResult;
}

int32_t XMLParser::ParseSimplex(xmlNode &node, std::unordered_map<std::string, std::string> &config,
                                const std::string &valueName, const std::string &keyName)
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
        if (key.empty() || value.empty()) {
            return XML_PARSE_INTERNAL_FAIL;
        }
        config[key] = value;

        HGM_LOGI("HgmXMLParser ParseSimplex %{public}s=%{public}s %{public}s=%{public}s",
                 keyName.c_str(), key.c_str(), valueName.c_str(), config[key].c_str());
    }

    return EXEC_SUCCESS;
}

int32_t XMLParser::ParserDynamicSetting(xmlNode &node, PolicyConfigData::DynamicSettingMap &dynamicSettingMap)
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

            HGM_LOGI("HgmXMLParser ParserDynamicSetting dynamicType=%{public}s name=%{public}s min=%{public}d",
                     dynamicSettingType.c_str(), name.c_str(), dynamicSettingMap[dynamicSettingType][name].min);
        }
    }
    return EXEC_SUCCESS;
}

int32_t XMLParser::ParseSceneList(xmlNode &node, PolicyConfigData::SceneConfigMap &sceneList)
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

        sceneList[name] = sceneConfig;
        HGM_LOGI("HgmXMLParser ParseSceneList name=%{public}s strategy=%{public}s priority=%{public}s",
                 name.c_str(), sceneList[name].strategy.c_str(), sceneList[name].priority.c_str());
    }

    return EXEC_SUCCESS;
}

int32_t XMLParser::ParseSupportedModeConfig(xmlNode &node, PolicyConfigData::SupportedModeConfig &supportedModeConfig)
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

int32_t XMLParser::ParseMultiAppStrategy(xmlNode &node, PolicyConfigData::ScreenSetting &screenSetting)
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
    return ParseSimplex(node, screenSetting.appList, "strategy");
}


int32_t XMLParser::ParseAppTypes(xmlNode &node, std::unordered_map<int32_t, std::string> &appTypes)
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

std::string XMLParser::ExtractPropertyValue(const std::string &propName, xmlNode &node)
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
    if (str.length() == 0) {
        return false;
    }
    auto number = static_cast<uint32_t>(std::count_if(str.begin(), str.end(), [](unsigned char c) {
        return std::isdigit(c);
    }));
    return number == str.length() || (str.compare(0, 1, "-") == 0 && number == str.length() - 1);
}

std::vector<uint32_t> XMLParser::StringToVector(const std::string &str)
{
    // valid format: string consisting of only numbers and spaces
    if (!std::regex_match(str, std::regex("^\\s*(\\d+(\\s+\\d+)*)\\s*$"))) {
        HGM_LOGD("Input invalid format.");
        return {};
    }

    std::istringstream isstr(str);
    std::vector<uint32_t> vec;
    uint32_t num;
    while (isstr >> num) {
        vec.push_back(num);
    }

    return vec;
}

} // namespace OHOS::Rosen