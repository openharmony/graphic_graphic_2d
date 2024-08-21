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

#include "rs_graphic_config.h"
#include "config_policy_utils.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
RSGraphicConfig::ConfigItem RSGraphicConfig::config_;
const std::map<std::string, RSGraphicConfig::ValueType> RSGraphicConfig::configItemTypeMap_ = {
    { "blurEffect",                                   RSGraphicConfig::ValueType::MAP },
    { "blurSwitchOpen",                               RSGraphicConfig::ValueType::UNDIFINED },
};

static inline bool IsNumber(std::string str)
{
    if (str.size() == 0) {
        return false;
    }
    for (int32_t i = 0; i < static_cast<int32_t>(str.size()); i++) {
        if (str.at(i) < '0' || str.at(i) > '9') {
            return false;
        }
    }
    return true;
}

static bool IsFloatingNumber(std::string str, bool allowNeg = false)
{
    if (str.size() == 0) {
        return false;
    }

    int32_t i = 0;
    if (allowNeg && str.at(i) == '-') {
        i++;
    }

    for (; i < static_cast<int32_t>(str.size()); i++) {
        if ((str.at(i) < '0' || str.at(i) > '9') &&
            (str.at(i) != '.' || std::count(str.begin(), str.end(), '.') > 1)) {
            return false;
        }
    }
    return true;
}

static std::vector<std::string> Split(std::string str, std::string pattern)
{
    if (str.empty() || pattern.empty()) {
        return {};
    }
    int32_t position;
    std::vector<std::string> result;
    str += pattern;
    int32_t strLen = static_cast<int32_t>(str.size());
    int32_t patternLen = static_cast<int32_t>(pattern.size());
    for (int32_t i = 0; i < strLen; i = position + patternLen) {
        position = static_cast<int32_t>(str.find(pattern, i));
        if (position >= strLen || position < 0) {
            break;
        }
        std::string tmp = str.substr(i, position - i);
        result.push_back(tmp);
    }
    return result;
}

std::vector<std::string> RSGraphicConfig::SplitNodeContent(const xmlNodePtr& node, const std::string& pattern)
{
    xmlChar* content = xmlNodeGetContent(node);
    if (content == nullptr) {
        RS_LOGE("[GraphicConfig] read xml node error: nodeName:(%{public}s)", node->name);
        return std::vector<std::string>();
    }

    std::string contentStr = reinterpret_cast<const char*>(content);
    xmlFree(content);
    if (contentStr.size() == 0) {
        return std::vector<std::string>();
    }
    return Split(contentStr, pattern);
}

std::string RSGraphicConfig::GetConfigPath(const std::string& configFileName)
{
    char buf[PATH_MAX];
    char* configPath = GetOneCfgFile(configFileName.c_str(), buf, PATH_MAX);
    char tmpPath[PATH_MAX] = { 0 };
    if (!configPath || strlen(configPath) == 0 || strlen(configPath) >= PATH_MAX || !realpath(configPath, tmpPath)) {
        RS_LOGI("[GraphicConfig] can not get customization config file");
        return "/system/" + configFileName;
    }
    return std::string(tmpPath);
}

void RSGraphicConfig::ReadConfig(const xmlNodePtr& rootPtr, std::map<std::string, ConfigItem>& mapValue)
{
    for (xmlNodePtr curNodePtr = rootPtr->xmlChildrenNode; curNodePtr != nullptr; curNodePtr = curNodePtr->next) {
        if (!IsValidNode(*curNodePtr)) {
            RS_LOGE("[GraphicConfig]: invalid node!");
            continue;
        }
        std::string nodeName = reinterpret_cast<const char*>(curNodePtr->name);
        if (configItemTypeMap_.count(nodeName)) {
            std::map<std::string, ConfigItem> property = ReadProperty(curNodePtr);
            if (property.size() > 0) {
                mapValue[reinterpret_cast<const char*>(curNodePtr->name)].SetProperty(property);
            }
            switch (configItemTypeMap_.at(nodeName)) {
                case ValueType::INTS: {
                    std::vector<int> v = ReadIntNumbersConfigInfo(curNodePtr);
                    mapValue[reinterpret_cast<const char*>(curNodePtr->name)].SetValue(v);
                    break;
                }
                case ValueType::FLOATS: {
                    std::vector<float> v = ReadFloatNumbersConfigInfo(curNodePtr, true);
                    mapValue[reinterpret_cast<const char*>(curNodePtr->name)].SetValue(v);
                    break;
                }
                case ValueType::POSITIVE_FLOATS: {
                    std::vector<float> v = ReadFloatNumbersConfigInfo(curNodePtr, false);
                    mapValue[reinterpret_cast<const char*>(curNodePtr->name)].SetValue(v);
                    break;
                }
                case ValueType::STRING: {
                    std::string v = ReadStringConfigInfo(curNodePtr);
                    mapValue[reinterpret_cast<const char*>(curNodePtr->name)].SetValue(v);
                    break;
                }
                case ValueType::STRINGS: {
                    std::vector<std::string> v = ReadStringsConfigInfo(curNodePtr);
                    mapValue[reinterpret_cast<const char*>(curNodePtr->name)].SetValue(v);
                    break;
                }
                case ValueType::MAP: {
                    std::map<std::string, ConfigItem> v;
                    ReadConfig(curNodePtr, v);
                    mapValue[reinterpret_cast<const char*>(curNodePtr->name)].SetValue(v);
                    break;
                }
                default:
                    break;
            }
        }
    }
}

bool RSGraphicConfig::LoadConfigXml()
{
    auto configFilePath = GetConfigPath("etc/graphic/graphic_config.xml");
    xmlDocPtr docPtr = nullptr;
    {
        std::lock_guard<std::recursive_mutex> lock(configItemMutex);
        docPtr = xmlReadFile(configFilePath.c_str(), nullptr, XML_PARSE_NOBLANKS);
    }
    RS_LOGI("[GraphicConfig] filePath: %{public}s", configFilePath.c_str());
    if (docPtr == nullptr) {
        RS_LOGE("[GraphicConfig] load xml error!");
        return false;
    }

    xmlNodePtr rootPtr = xmlDocGetRootElement(docPtr);
    if (rootPtr == nullptr || rootPtr->name == nullptr ||
        xmlStrcmp(rootPtr->name, reinterpret_cast<const xmlChar*>("Configs"))) {
        RS_LOGE("[GraphicConfig] get root element failed!");
        xmlFreeDoc(docPtr);
        return false;
    }

    std::map<std::string, ConfigItem> configMap;
    config_.SetValue(configMap);
    ReadConfig(rootPtr, *config_.mapValue);

    xmlFreeDoc(docPtr);
    return true;
}

bool RSGraphicConfig::IsValidNode(const xmlNode& currNode)
{
    if (currNode.name == nullptr || currNode.type == XML_COMMENT_NODE) {
        return false;
    }
    return true;
}

std::map<std::string, RSBaseXmlConfig::ConfigItem> RSGraphicConfig::ReadProperty(const xmlNodePtr& currNode)
{
    std::map<std::string, ConfigItem> property;
    xmlChar* propVal = xmlGetProp(currNode, reinterpret_cast<const xmlChar*>("enable"));
    if (propVal != nullptr) {
        if (!xmlStrcmp(propVal, reinterpret_cast<const xmlChar*>("true"))) {
            property["enable"].SetValue(true);
        } else if (!xmlStrcmp(propVal, reinterpret_cast<const xmlChar*>("false"))) {
            property["enable"].SetValue(false);
        }
        xmlFree(propVal);
    }

    propVal = xmlGetProp(currNode, reinterpret_cast<const xmlChar*>("name"));
    if (propVal != nullptr) {
        property["name"].SetValue(std::string(reinterpret_cast<const char*>(propVal)));
        xmlFree(propVal);
    }

    return property;
}

std::vector<int> RSGraphicConfig::ReadIntNumbersConfigInfo(const xmlNodePtr& currNode)
{
    std::vector<int> intsValue;
    auto numbers = SplitNodeContent(currNode);
    for (auto& num : numbers) {
        if (!IsNumber(num)) {
            RS_LOGE("[GraphicConfig] read int number error: nodeName:(%{public}s)", currNode->name);
            return {};
        }
        intsValue.push_back(std::stoi(num));
    }
    return intsValue;
}

std::vector<std::string> RSGraphicConfig::ReadStringsConfigInfo(const xmlNodePtr& currNode)
{
    return SplitNodeContent(currNode);
}

std::vector<float> RSGraphicConfig::ReadFloatNumbersConfigInfo(const xmlNodePtr& currNode, bool allowNeg)
{
    std::vector<float> floatsValue;
    auto numbers = SplitNodeContent(currNode);
    for (auto& num : numbers) {
        if (!IsFloatingNumber(num, allowNeg)) {
            RS_LOGE("[GraphicConfig] read float number error: nodeName:(%{public}s)", currNode->name);
            return {};
        }
        floatsValue.push_back(std::stof(num));
    }
    return floatsValue;
}

std::string RSGraphicConfig::ReadStringConfigInfo(const xmlNodePtr& currNode)
{
    std::string stringValue;
    xmlChar* context = xmlNodeGetContent(currNode);
    if (context == nullptr) {
        RS_LOGE("[GraphicConfig] read xml node error: nodeName:(%{public}s)", currNode->name);
        return {};
    }

    stringValue = std::string(reinterpret_cast<const char*>(context));
    xmlFree(context);
    return stringValue;
}

void RSGraphicConfig::DumpConfig(const std::map<std::string, ConfigItem>& config)
{
    for (auto& conf : config) {
        RS_LOGI("[GraphicConfig] %{public}s", conf.first.c_str());
        std::map<std::string, ConfigItem> propMap;
        if (conf.second.property) {
            propMap = *conf.second.property;
        }
        for (auto prop : propMap) {
            switch (prop.second.type) {
                case ValueType::BOOL:
                    RS_LOGI("[GraphicConfig] Prop: %{public}s %{public}u", prop.first.c_str(), prop.second.boolValue);
                    break;
                case ValueType::STRING:
                    RS_LOGI("[GraphicConfig] Prop: %{public}s %{public}s", prop.first.c_str(),
                        prop.second.stringValue.c_str());
                    break;
                default:
                    break;
            }
        }
        switch (conf.second.type) {
            case ValueType::INTS:
                for (auto& num : *conf.second.intsValue) {
                    RS_LOGI("[GraphicConfig] Num: %{public}d", num);
                }
                break;
            case ValueType::MAP:
                if (conf.second.mapValue) {
                    DumpConfig(*conf.second.mapValue);
                }
                break;
            case ValueType::STRING:
                RS_LOGI("[GraphicConfig] %{public}s", conf.second.stringValue.c_str());
                break;
            case ValueType::BOOL:
                RS_LOGI("[GraphicConfig] %{public}u", conf.second.boolValue);
                break;
            case ValueType::FLOATS:
                for (auto& num : *conf.second.floatsValue) {
                    RS_LOGI("[GraphicConfig] Num: %{public}f", num);
                }
                break;
            default:
                break;
        }
    }
}
} // namespace Rosen
} // namespace OHOS
