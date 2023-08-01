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

namespace OHOS::Rosen {
int32_t XMLParser::LoadConfiguration()
{
    if (!xmlDocument_) {
        HGM_LOGI("XMLParser opening xml file");
        xmlDocument_ = xmlReadFile(CONFIG_FILE, nullptr, 0);
    }

    if (!xmlDocument_) {
        HGM_LOGE("XMLParser xmlReadFile failed");
        return XML_FILE_LOAD_FAIL;
    }

    if (!mParsedData_) {
        HGM_LOGI("XMLParser initializing parsed data");
        mParsedData_ = std::make_unique<ParsedConfigData>();
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

int32_t XMLParser::GetHgmXmlNodeAsInt(xmlNode &node) const
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

        if (GetHgmXmlNodeAsInt(*currNode) == HGM_XML_PARAM) {
            parseSuccess = ParseParam(*currNode);
        } else if (GetHgmXmlNodeAsInt(*currNode) == HGM_XML_PARAMS) {
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
    }

    if (paraName == "general_config_enable") {
        HGM_LOGD("XMLParser parsing general_config_enable");
        std::string parsed = ExtractPropertyValue("value", node);
        mParsedData_->isDynamicFrameRateEnable_ = parsed;
    }

    HGM_LOGD("XMLParser parsing general_config_enable finish: %{public}s",
        mParsedData_->isDynamicFrameRateEnable_.c_str());
    return EXEC_SUCCESS;
}

int32_t XMLParser::ParseParams(xmlNode &node)
{
    std::string paraName = ExtractPropertyValue("name", node);
    if (paraName.empty()) {
        HGM_LOGD("XMLParser No name provided for %{public}s", node.name);
        return XML_PARSE_INTERNAL_FAIL;
    }
    if (!mParsedData_) {
        HGM_LOGE("XMLParser mParsedData_ is not initialized");
        return HGM_ERROR;
    }

    if (paraName == "detailed_strategies") {
        if (ParseStrat(node) != EXEC_SUCCESS) {
            HGM_LOGD("XMLParser failed to parse detailed_strategies");
        }
        return EXEC_SUCCESS;
    }

    if (paraName == "customer_setting_config") {
        if (ParseSetting(node, mParsedData_->customerSettingConfig_) != EXEC_SUCCESS) {
            HGM_LOGD("XMLParser failed to parse customer_setting_config");
        }
        return EXEC_SUCCESS;
    }

    if (paraName == "bundle_name_black_list") {
        if (ParseSetting(node, mParsedData_->bundle_black_list_) != EXEC_SUCCESS) {
            HGM_LOGD("XMLParser failed to parse bundle_black_list_");
        }
        return EXEC_SUCCESS;
    }

    if (paraName == "bundle_name_white_list") {
        if (ParseSetting(node, mParsedData_->bundle_white_list_) != EXEC_SUCCESS) {
            HGM_LOGD("XMLParser failed to parse bundle_white_list_");
        }
        return EXEC_SUCCESS;
    }

    if (paraName == "animation_dynamic_settings") {
        if (ParseSetting(node, mParsedData_->animationDynamicStrats_) != EXEC_SUCCESS) {
            HGM_LOGD("XMLParser failed to parse animation_dynamic_settings");
        }
        return EXEC_SUCCESS;
    }

    if (paraName == "property_animation_dynamic_settings") {
        if (ParserAnimationDynamicSetting(node) != EXEC_SUCCESS) {
            HGM_LOGD("XMLParser failed to parse property_animation_dynamic_settings");
        }
        return EXEC_SUCCESS;
    }

    HGM_LOGD("XMLParser parsing params finish");
    return XML_PARSE_INTERNAL_FAIL;
}

int32_t XMLParser::ParseStrat(xmlNode &node)
{
    xmlNode *currNode = &node;
    if (currNode->xmlChildrenNode == nullptr) {
        HGM_LOGD("XMLParser stop parsing strats, no children nodes");
        return HGM_ERROR;
    }
    if (!mParsedData_) {
        HGM_LOGE("XMLParser mParsedData_ is not initialized");
        return HGM_ERROR;
    }

    currNode = currNode->xmlChildrenNode;
    for (; currNode; currNode = currNode->next) {
        if (currNode->type != XML_ELEMENT_NODE) {
            continue;
        }

        ParsedConfigData::detailedStrat strat;
        strat.name = ExtractPropertyValue("name", *currNode);
        strat.isDynamic = ExtractPropertyValue("isDynamic", *currNode);
        strat.min = ExtractPropertyValue("min", *currNode);
        strat.max = ExtractPropertyValue("max", *currNode);
        mParsedData_->detailedStrategies_[strat.name] = strat;
    }

    return EXEC_SUCCESS;
}

int32_t XMLParser::ParseSetting(xmlNode &node, std::unordered_map<std::string, std::string> &config)
{
    HGM_LOGD("XMLParser parsing strats");
    xmlNode *currNode = &node;
    if (currNode->xmlChildrenNode == nullptr) {
        HGM_LOGD("XMLParser stop parsing settings, no children nodes");
        return HGM_ERROR;
    }
    currNode = currNode->xmlChildrenNode;

    for (; currNode; currNode = currNode->next) {
        if (currNode->type != XML_ELEMENT_NODE) {
            continue;
        }

        std::string name = "";
        std::string value = "";
        name = ExtractPropertyValue("name", *currNode);
        value = ExtractPropertyValue("value", *currNode);
        if (name.empty() || value.empty()) {
            return XML_PARSE_INTERNAL_FAIL;
        }
        config[name] = value;
    }

    return EXEC_SUCCESS;
}

int32_t XMLParser::ParserAnimationDynamicSetting(xmlNode &node)
{
    xmlNode *currNode = &node;
    if (currNode->xmlChildrenNode == nullptr) {
        return HGM_ERROR;
    }
    currNode = currNode->xmlChildrenNode;
    for (; currNode; currNode = currNode->next) {
        auto dynamicSettingType = ExtractPropertyValue("name", *currNode);
        if (mParsedData_->dynamicSetting_.find(dynamicSettingType) == mParsedData_->dynamicSetting_.end()) {
            continue;
        }
        for (xmlNode *thresholdNode = currNode->xmlChildrenNode; thresholdNode; thresholdNode = thresholdNode->next) {
            if (thresholdNode->type != XML_ELEMENT_NODE) {
                continue;
            }
            auto name = ExtractPropertyValue("name", *thresholdNode);
            auto min = ExtractPropertyValue("min", *thresholdNode);
            auto max = ExtractPropertyValue("max", *thresholdNode);
            auto preferred_fps = ExtractPropertyValue("preferred_fps", *thresholdNode);
            if (!IsNumber(min) || !IsNumber(max) || !IsNumber(preferred_fps)) {
                mParsedData_->dynamicSetting_[dynamicSettingType].clear();
                break;
            }
            ParsedConfigData::AnimationDynamicSetting animationDynamicSetting;
            animationDynamicSetting.min = std::stoi(ExtractPropertyValue("min", *thresholdNode));
            animationDynamicSetting.max = std::stoi(ExtractPropertyValue("max", *thresholdNode));
            animationDynamicSetting.preferred_fps = std::stoi(ExtractPropertyValue("preferred_fps", *thresholdNode));
            mParsedData_->dynamicSetting_[dynamicSettingType][name] = animationDynamicSetting;
        }
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

bool XMLParser::IsNumber(const std::string &str)
{
    if (str.length() == 0) {
        return false;
    }
    auto number = static_cast<uint32_t>(std::count_if(str.begin(), str.end(), [](unsigned char c) {
        return std::isdigit(c);
    }));
    return number == str.length() || (str.compare(0, 1, "-") == 0 && number == str.length() - 1);
}

} // namespace OHOS::Rosen