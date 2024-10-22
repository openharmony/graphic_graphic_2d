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

#include "graphic_2d_xml_parser.h"
#include "platform/common/rs_log.h"

namespace OHOS::Rosen {

int32_t Graphic2dXmlParser::LoadConfiguration(const char* fileDir)
{
    RS_LOGD("Graphic2dXmlParser opening xml file");
    xmlDocument_ = xmlReadFile(fileDir, nullptr, 0);
    if (!xmlDocument_) {
        RS_LOGE("Graphic2dXmlParser xmlReadFile failed");
        return XML_FILE_LOAD_FAIL;
    }

    if (!mParsedData_) {
        mParsedData_ = std::make_unique<Graphic2dConfigData>();
    }

    return EXEC_SUCCESS;
}

int32_t Graphic2dXmlParser::Parse()
{
    RS_LOGD("Graphic2dXmlParser Parse");
    if (!xmlDocument_) {
        RS_LOGE("Graphic2dXmlParser xmlDocument_ is empty, should do LoadConfiguration first");
        return G2C_ERROR;
    }
    xmlNode *root = xmlDocGetRootElement(xmlDocument_);
    if (root == nullptr) {
        RS_LOGE("Graphic2dXmlParser xmlDocGetRootElement failed");
        return XML_GET_ROOT_FAIL;
    }

    if (ParseInternal(*root) == false) {
        return XML_PARSE_INTERNAL_FAIL;
    }
    return EXEC_SUCCESS;
}

void Graphic2dXmlParser::Destroy()
{
    RS_LOGD("Graphic2dXmlParser Destroying the parser");
    if (xmlDocument_ != nullptr) {
        xmlFreeDoc(xmlDocument_);
        xmlDocument_ = nullptr;
    }
}

bool Graphic2dXmlParser::ParseInternal(xmlNode &node)
{
    RS_LOGD("Graphic2dXmlParser parsing an internal node");
    xmlNode *currNode = &node;
    if (currNode->xmlChildrenNode == nullptr) {
        RS_LOGD("Graphic2dXmlParser stop parsing internal, no children nodes");
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
        int xmlParamType = GetG2cXmlNodeAsInt(*currNode);
        if (xmlParamType == G2C_XML_PARAM) {
            parseSuccess = ParseParams(*currNode);
        } else if (xmlParamType == G2C_XML_PARAMS) {
            parseSuccess = ParseParams(*currNode);
        }
    }
    return true;
}

int32_t Graphic2dXmlParser::GetG2cXmlNodeAsInt(xmlNode &node)
{
    if (!xmlStrcmp(node.name, reinterpret_cast<const xmlChar*>("Param"))) {
        return G2C_XML_PARAM;
    }
    if (!xmlStrcmp(node.name, reinterpret_cast<const xmlChar*>("Params"))) {
        return G2C_XML_PARAMS;
    }
    RS_LOGD("Graphic2dXmlParser failed to identify a xml node : %{public}s", node.name);
    return G2C_XML_UNDEFINED;
}

int32_t Graphic2dXmlParser::ParseParams(xmlNode &node)
{
    std::string paraName = ExtractPropertyValue("name", node);
    if (paraName.empty()) {
        return XML_PARSE_INTERNAL_FAIL;
    }
    if (!mParsedData_) {
        RS_LOGE("Graphic2dConfigure mParsedData_ is not initialized");
        return G2C_ERROR;
    }

    int32_t setResult = EXEC_SUCCESS;
    
    if (paraName == "graphic_2d_config") {
        RS_LOGD("Graphic2dConfigure Graphic2dXmlParser paraName ParseGraphic2dConfig");
        setResult = ParseSimplex(node, mParsedData_->graphic2dConfig_);
    }

    if (setResult != EXEC_SUCCESS) {
        RS_LOGD("Graphic2dXmlParser failed to ParseParams %{public}s", paraName.c_str());
    }
    
    return EXEC_SUCCESS;
}

int32_t Graphic2dXmlParser::ParseSimplex(xmlNode &node, std::unordered_map<std::string, std::string> &config,
    const std::string &valueName, const std::string &keyName)
{
    RS_LOGD("Graphic2dXmlParser parsing simplex");
    xmlNode *currNode = &node;
    if (currNode->xmlChildrenNode == nullptr) {
        RS_LOGD("Graphic2dXmlParser stop parsing simplex, no children nodes");
        return G2C_ERROR;
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

        RS_LOGI("HgmGraphic2dXmlParser ParseSimplex %{public}s=%{public}s %{public}s=%{public}s",
                keyName.c_str(), key.c_str(), valueName.c_str(), config[key].c_str());
    }

    return EXEC_SUCCESS;
}

std::string Graphic2dXmlParser::ExtractPropertyValue(const std::string &propName, xmlNode &node)
{
    RS_LOGD("Graphic2dXmlParser extracting value : %{public}s", propName.c_str());
    std::string propValue = "";
    xmlChar *tempValue = nullptr;

    if (xmlHasProp(&node, reinterpret_cast<const xmlChar*>(propName.c_str()))) {
        tempValue = xmlGetProp(&node, reinterpret_cast<const xmlChar*>(propName.c_str()));
    }

    if (tempValue != nullptr) {
        RS_LOGD("Graphic2dXmlParser not empty tempValue");
        propValue = reinterpret_cast<const char*>(tempValue);
        xmlFree(tempValue);
        tempValue = nullptr;
    }

    return propValue;
}
}