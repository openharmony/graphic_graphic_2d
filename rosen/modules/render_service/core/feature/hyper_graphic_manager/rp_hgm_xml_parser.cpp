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

#include "rp_hgm_xml_parser.h"

#include "hgm_log.h"
#include "hgm_command.h"

#undef LOG_TAG
#define LOG_TAG "RPHgmXMLParser"

namespace OHOS::Rosen {
namespace {
const std::string ADDITIONAL_TOUCH_RATE_CONFIG = "additional_touch_rate_config";
const std::string SOURCE_TUNING_FOR_YUV420 = "source_tuning_for_yuv420";
const std::string RS_SOLID_COLOR_LAYER_CONFIG = "rs_solid_color_layer_config";
}

RPHgmXMLParser::~RPHgmXMLParser()
{
    if (xmlDocument_) {
        xmlFreeDoc(xmlDocument_);
        xmlDocument_ = nullptr;
    }
}

int32_t RPHgmXMLParser::LoadConfiguration(const char* fileDir)
{
    HGM_LOGI("%{public}s opening xml file", __func__);
    xmlDocument_ = xmlReadFile(fileDir, nullptr, 0);
    if (!xmlDocument_) {
        HGM_LOGE("%{public}s xmlReadFile failed", __func__);
        return XML_FILE_LOAD_FAIL;
    }

    return Parse();
}

int32_t RPHgmXMLParser::Parse()
{
    xmlNode* root = xmlDocGetRootElement(xmlDocument_);
    if (root == nullptr) {
        HGM_LOGE("%{public}s xmlDocGetRootElement failed", __func__);
        return XML_GET_ROOT_FAIL;
    }

    int32_t setResult = EXEC_SUCCESS;
    for (xmlNode* currNode = root->xmlChildrenNode; currNode; currNode = currNode->next) {
        if (currNode->type != XML_ELEMENT_NODE) {
            continue;
        }
        std::string paramName = ExtractPropertyValue("name", *currNode);
        if (paramName == ADDITIONAL_TOUCH_RATE_CONFIG) {
            setResult = ParseNode(*currNode, appBufferList_);
        } else if (paramName == SOURCE_TUNING_FOR_YUV420) {
            setResult = ParseNode(*currNode, sourceTuningConfig_);
        } else if (paramName == RS_SOLID_COLOR_LAYER_CONFIG) {
            setResult = ParseNode(*currNode, solidLayerConfig_);
        }
        if (setResult != EXEC_SUCCESS) {
            return XML_PARSE_INTERNAL_FAIL;
        }
    }
    return setResult;
}

int32_t RPHgmXMLParser::ParseNode(xmlNode& node, std::vector<std::string>& data)
{
    HGM_LOGD("%{public}s vector", __func__);
    xmlNode* currNode = &node;
    if (currNode->xmlChildrenNode == nullptr) {
        HGM_LOGD("%{public}s stop parsing ParseNode, no children nodes", __func__);
        return XML_PARSE_INTERNAL_FAIL;
    }
    currNode = currNode->xmlChildrenNode;
    for (; currNode; currNode = currNode->next) {
        if (currNode->type != XML_ELEMENT_NODE) {
            continue;
        }
        auto name = ExtractPropertyValue("name", *currNode);
        if (name.empty()) {
            return XML_PARSE_INTERNAL_FAIL;
        }
        data.push_back(name);
    }
    return EXEC_SUCCESS;
}

int32_t RPHgmXMLParser::ParseNode(xmlNode& node, std::unordered_map<std::string, std::string>& data)
{
    HGM_LOGD("%{public}s map", __func__);
    xmlNode* currNode = &node;
    if (currNode->xmlChildrenNode == nullptr) {
        HGM_LOGD("%{public}s stop parsing ParseNode, no children nodes", __func__);
        return XML_PARSE_INTERNAL_FAIL;
    }
    currNode = currNode->xmlChildrenNode;
    for (; currNode; currNode = currNode->next) {
        if (currNode->type != XML_ELEMENT_NODE) {
            continue;
        }
        auto name = ExtractPropertyValue("name", *currNode);
        auto value = ExtractPropertyValue("value", *currNode);
        if (name.empty() || value.empty()) {
            return XML_PARSE_INTERNAL_FAIL;
        }
        HGM_LOGD("%{public}s name=%{public}s, value=%{public}s",
            __func__, name.c_str(), value.c_str());
        data.insert_or_assign(name, value);
    }
    return EXEC_SUCCESS;
}

std::string RPHgmXMLParser::ExtractPropertyValue(const std::string& propName, xmlNode& node)
{
    std::string propValue;
    xmlChar* tempValue = nullptr;

    if (xmlHasProp(&node, reinterpret_cast<const xmlChar*>(propName.c_str()))) {
        tempValue = xmlGetProp(&node, reinterpret_cast<const xmlChar*>(propName.c_str()));
    }

    if (tempValue != nullptr) {
        HGM_LOGD("%{public}s not empty tempValue", __func__);
        propValue = reinterpret_cast<const char*>(tempValue);
        xmlFree(tempValue);
        tempValue = nullptr;
    }

    return propValue;
}
} // namespace OHOS::Rosen