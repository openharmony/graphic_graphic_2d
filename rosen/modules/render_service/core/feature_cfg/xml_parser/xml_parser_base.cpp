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
#include "xml_parser_base.h"

#include <algorithm>
#include "graphic_feature_param_manager.h"

namespace OHOS::Rosen {
namespace {
constexpr uint32_t XML_STRING_MAX_LENGTH = 20;
}

void XMLParserBase::Destroy()
{
    RS_LOGD("XMLParserBase Destroying the parser");
    if (xmlSysDocument_ != nullptr) {
        xmlFreeDoc(xmlSysDocument_);
        xmlSysDocument_ = nullptr;
    }
    if (xmlProdDocument_ != nullptr) {
        xmlFreeDoc(xmlProdDocument_);
        xmlProdDocument_ = nullptr;
    }
}

int32_t XMLParserBase::LoadSysConfiguration(const std::string& fileDir)
{
    for (const std::string& configPath : sysPaths_) {
        std::string graphicFilePath = configPath + fileDir;
        xmlSysDocument_ = xmlReadFile(graphicFilePath.c_str(), nullptr, 0);
        if (xmlSysDocument_ != nullptr) {
            RS_LOGD("XMLParserBase success to get sys graphic config: %{public}s", graphicFilePath.c_str());
            break;
        }
    }
    if (!xmlSysDocument_) {
        RS_LOGE("XMLParserBase read system file failed");
        return PARSE_SYS_FILE_LOAD_FAIL;
    }
    return PARSE_EXEC_SUCCESS;
}

void XMLParserBase::LoadProdConfiguration(const std::string& fileDir)
{
    std::string graphicFilePath = prodPath_ + fileDir;
    xmlProdDocument_ = xmlReadFile(graphicFilePath.c_str(), nullptr, 0);
    if (!xmlProdDocument_) {
        RS_LOGD("XMLParserBase not have prod graphic config: %{public}s", graphicFilePath.c_str());
    }
}

int32_t XMLParserBase::LoadGraphicConfiguration(const std::string& fileDir)
{
    RS_LOGI("XMLParserBase opening xml file");
    // System base config file read
    if (LoadSysConfiguration(fileDir) != PARSE_EXEC_SUCCESS) {
        return PARSE_SYS_FILE_LOAD_FAIL;
    }
    // For different feature settings in variant products
    LoadProdConfiguration(fileDir);
    return PARSE_EXEC_SUCCESS;
}

int32_t XMLParserBase::ParseSysDoc()
{
    RS_LOGI("XMLParserBase Parse SysDoc Start");
    if (!xmlSysDocument_) {
        RS_LOGE("XMLParserBase xmlSysDocument_ is empty, should do LoadGraphicConfiguration first");
        return PARSE_SYS_FILE_LOAD_FAIL;
    }
    xmlNode *root = xmlDocGetRootElement(xmlSysDocument_);
    if (root == nullptr) {
        RS_LOGE("XMLParserBase xmlDocGetRootElement failed");
        return PARSE_GET_ROOT_FAIL;
    }

    if (ParseInternal(*root) == false) {
        return PARSE_INTERNAL_FAIL;
    }
    return PARSE_EXEC_SUCCESS;
}

int32_t XMLParserBase::ParseProdDoc()
{
    RS_LOGI("XMLParserBase Parse ProdDoc Start");
    if (!xmlProdDocument_) {
        RS_LOGD("XMLParserBase xmlProdDocument_ is empty, check if need product config first");
        return PARSE_PROD_FILE_LOAD_FAIL;
    }
    xmlNode *root = xmlDocGetRootElement(xmlProdDocument_);
    if (root == nullptr) {
        RS_LOGE("XMLParserBase xmlDocGetRootElement failed");
        return PARSE_GET_ROOT_FAIL;
    }

    if (ParseInternal(*root) == false) {
        return PARSE_INTERNAL_FAIL;
    }
    return PARSE_EXEC_SUCCESS;
}

bool XMLParserBase::ParseInternal(xmlNode &node)
{
    RS_LOGI("XMLParserBase ParseInternal Start");
    xmlNode *currNode = &node;
    if (currNode->xmlChildrenNode == nullptr) {
        RS_LOGD("XMLParserBase stop parsing internal, no children nodes");
        return false;
    }
    currNode = currNode->xmlChildrenNode;
    int32_t parseSuccess = PARSE_EXEC_SUCCESS;

    for (; currNode; currNode = currNode->next) {
        if (currNode->type != XML_ELEMENT_NODE) {
            continue;
        }

        std::string featureName = ExtractPropertyValue("name", *currNode);
        RS_LOGI("XMLParserBase featureName is: %{public}s", featureName.c_str());
        if (featureName == "") {
            RS_LOGE("XMLParserBase featureName is empty");
            continue;
        }
        auto parseMap = GraphicFeatureParamManager::GetInstance().featureParseMap_;
        auto featureMap = GraphicFeatureParamManager::GetInstance().featureParamMap_;
        auto iter = parseMap.find(featureName);
        if (iter != parseMap.end()) {
            auto featureObj = iter->second;
            parseSuccess = featureObj->ParseFeatureParam(featureMap, *currNode);
            if (parseSuccess != PARSE_EXEC_SUCCESS) {
                RS_LOGE("XMLParserBase current feature : %{public}s parse fail", featureName.c_str());
                continue;
            }
        } else {
            RS_LOGD("XMLParserBase featureMap cannot find feature %{public}s", featureName.c_str());
        }
    }
    return true;
}

std::string XMLParserBase::ExtractPropertyValue(const std::string &propName, xmlNode &node)
{
    RS_LOGD("XMLParserBase extracting value : %{public}s", propName.c_str());
    std::string propValue = "";
    xmlChar *tempValue = nullptr;

    if (xmlHasProp(&node, reinterpret_cast<const xmlChar*>(propName.c_str()))) {
        tempValue = xmlGetProp(&node, reinterpret_cast<const xmlChar*>(propName.c_str()));
    }

    if (tempValue != nullptr) {
        RS_LOGD("XMLParserBase not a empty tempValue");
        propValue = reinterpret_cast<const char*>(tempValue);
        xmlFree(tempValue);
        tempValue = nullptr;
    }

    return propValue;
}

int32_t XMLParserBase::GetXmlNodeAsInt(xmlNode &node)
{
    if (!xmlStrcmp(node.name, reinterpret_cast<const xmlChar*>("FeatureSwitch"))) {
        return PARSE_XML_FEATURE_SWITCH;
    }
    if (!xmlStrcmp(node.name, reinterpret_cast<const xmlChar*>("FeatureSingleParam"))) {
        return PARSE_XML_FEATURE_SINGLEPARAM;
    }
    if (!xmlStrcmp(node.name, reinterpret_cast<const xmlChar*>("FeatureMultiParam"))) {
        return PARSE_XML_FEATURE_MULTIPARAM;
    }
    RS_LOGD("XMLParserBase failed to identify a xml node : %{public}s", node.name);
    return PARSE_XML_UNDEFINED;
}

bool XMLParserBase::IsNumber(const std::string& str)
{
    if (str.length() == 0 || str.length() > XML_STRING_MAX_LENGTH) {
        return false;
    }
    auto number = static_cast<uint32_t>(std::count_if(str.begin(), str.end(), [](unsigned char c) {
        return std::isdigit(c);
    }));
    return number == str.length() || (str.compare(0, 1, "-") == 0 && number == str.length() - 1);
}

bool XMLParserBase::ParseFeatureSwitch(std::string val)
{
    return val == "true";
}
} // namespace OHOS::Rosen
