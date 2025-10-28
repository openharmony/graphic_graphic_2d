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

#undef LOG_TAG
#define LOG_TAG "RPHgmXMLParser"

namespace OHOS::Rosen {
namespace {
const std::string ADDITIONAL_TOUCH_RATE_CONFIG = "additional_touch_rate_config";
const std::string SOURCE_TUNING_FOR_YUV420 = "source_tuning_for_yuv420";
const std::string RS_SOLID_COLOR_LAYER_CONFIG = "rs_solid_color_layer_config";
}

int32_t RPHgmXMLParser::LoadConfiguration(const char* fileDir)
{
    HGM_LOGI("HgmXMLParser opening xml file");
    xmlDocument_ = xmlReadFile(fileDir, nullptr, 0);
    if (!xmlDocument_) {
        HGM_LOGE("HgmXMLParser xmlReadFile failed");
        return XML_FILE_LOAD_FAIL;
    }

    return Parse();
}

int32_t RPHgmXMLParser::Parse()
{
    xmlNode* root = xmlDocGetRootElement(xmlDocument_);
    if (root == nullptr) {
        HGM_LOGE("HgmXMLParser xmlDocGetRootElement failed");
        return XML_GET_ROOT_FAIL;
    }

    xmlNode* currNode = root->xmlChildrenNode;
    if (currNode == nullptr) {
        HGM_LOGE("HgmXMLParser stop parsing internal, no children nodes");
        return XML_PARSE_INTERNAL_FAIL;
    }

    for (; currNode; currNode = currNode->next) {
        if (currNode->type != XML_ELEMENT_NODE) {
            continue;
        }
        std::string paramName = ExtractPropertyValue("name", *currNode);

        if (paramName == ADDITIONAL_TOUCH_RATE_CONFIG) {
            ParseNode(*currNode, appBufferList_);
        } else if (paramName == SOURCE_TUNING_FOR_YUV420) {
            ParseNode(*currNode, sourceTuningConfig_);
        } else if (paramName == RS_SOLID_COLOR_LAYER_CONFIG) {
            ParseNode(*currNode, solidLayerConfig_);
        }
    }
    return EXEC_SUCCESS;
}

void RPHgmXMLParser::ParseNode(xmlNode& node, std::vector<std::string>& data)
{
    HGM_LOGD("HgmXMLParser ParseNode vector");
    xmlNode* currNode = &node;
    if (currNode->xmlChildrenNode == nullptr) {
        HGM_LOGD("HgmXMLParser stop parsing ParseNode, no children nodes");
        return;
    }

    currNode = currNode->xmlChildrenNode;
    for (; currNode; currNode = currNode->next) {
        if (currNode->type != XML_ELEMENT_NODE) {
            continue;
        }
        auto name = ExtractPropertyValue("name", *currNode);
        data.push_back(name);
    }
}

void RPHgmXMLParser::ParseNode(xmlNode& node, std::unordered_map<std::string, std::string>& data)
{
    HGM_LOGD("HgmXMLParser ParseNode map");
    xmlNode* currNode = &node;
    if (currNode->xmlChildrenNode == nullptr) {
        HGM_LOGD("HgmXMLParser stop parsing ParseNode, no children nodes");
        return;
    }

    currNode = currNode->xmlChildrenNode;
    for (; currNode; currNode = currNode->next) {
        if (currNode->type != XML_ELEMENT_NODE) {
            continue;
        }
        auto name = ExtractPropertyValue("name", *currNode);
        auto value = ExtractPropertyValue("value", *currNode);
        data.insert_or_assign(name, value);
    }
}

std::string RPHgmXMLParser::ExtractPropertyValue(const std::string& propName, xmlNode& node)
{
    std::string propValue = "";
    xmlChar* tempValue = nullptr;

    if (xmlHasProp(&node, reinterpret_cast<const xmlChar*>(propName.c_str()))) {
        tempValue = xmlGetProp(&node, reinterpret_cast<const xmlChar*>(propName.c_str()));
    }

    if (tempValue != nullptr) {
        HGM_LOGD("HgmXMLParser not empty tempValue");
        propValue = reinterpret_cast<const char*>(tempValue);
        xmlFree(tempValue);
        tempValue = nullptr;
    }

    return propValue;
}
} // namespace OHOS::Rosen